#ifndef TRIDIGIONAL_EQUATION_HPP
#define TRIDIGIONAL_EQUATION_HPP

#include <cstddef>
#include <iostream>
#include <memory>
#include <CL/cl.hpp>
#include "initializer.hpp"
#include "ClInit.hpp"
#include "tools.hpp"

//#define HTS_DEBUG
//#define float cl_float
template<class T=float> struct TridigionalEquation;

template <class T>
std::ostream& operator<< (std::ostream& os, const TridigionalEquation<T>& eq);

template<class T>
struct TridigionalEquation {

	/*
	 * The view
	 * b0 c0  0 0  0 ...  0    d0
	 * a0 b1 c1 0  0 ...  0    d1
	 * 0  a1 b2 c2 0 ...  0    d2
	 * .. .. .. .. .. ..  0    ..
	 * 0  0  0  0  0 an-2 bn-1 dn-1
	 *
	 * terms is b0 d0 a0 c0 b1 d1 a1 c1 ... bn-2 dn-2 an-2 cn-2 bn-1 dn-1
	 * For example: b0 d0 a0 c0 b1 d1 a1 c1 b2 d2 a2 c2 b3 d3
	 * size = 2 + 4*(n-1)
	 *
	 * b[i] = terms[i*4]
	 * c[i] = terms[i*4+3]
	 * a[i] = terms[i*4+2]
	 * d[i] = terms[i*4+1]
	 *
	 * Client must release memory of terms!
	 */
	TridigionalEquation(cl::CommandQueue commandQueue, T* terms, size_t size);
	TridigionalEquation(
			T* topDiag, T* midDiag, T* downDiag, T* constTerms, size_t size);
    TridigionalEquation(TridigionalEquation& eq) = delete;
    TridigionalEquation& operator=(TridigionalEquation& eq) = delete;
	int inverse();
	void solve();
	const std::unique_ptr<T[]>& getUnknows() const{
		return unknowns;
	}
	size_t getSize();
	friend std::ostream& operator<< <T> (std::ostream& os, const TridigionalEquation<T>& eq);
	void outMatrix(std::ostream&);
private:
	T* topDiag;
	T* midDiag;
	T* downDiag;
	T* constTerms;
    bool isSolve;

	cl::CommandQueue commandQueue;
	T* terms;
	size_t size; // is number of equation
	std::unique_ptr<T[]> unknowns;
	size_t numberOfTerms; // number of elements in terms
};
template<class T>
TridigionalEquation<T>::TridigionalEquation(cl::CommandQueue commandQueue, T* terms, size_t size)
		: commandQueue(commandQueue), terms(terms), size(size),
		  	unknowns(new T[size]), numberOfTerms(4*size-2){}

template<class T>
void TridigionalEquation<T>::solve(){
	cl::Device device;
	cl::Context context;
	commandQueue.getInfo<cl::Context>(CL_QUEUE_CONTEXT, &context);
	commandQueue.getInfo<cl::Device>(CL_QUEUE_DEVICE, &device);
	cl::Buffer in(context, CL_MEM_READ_WRITE, numberOfTerms*sizeof(T));
	/*Non blocking write can be faster?*/
	commandQueue.enqueueWriteBuffer(in, CL_TRUE, 0, numberOfTerms*sizeof(T), terms);//Map!
	std::string frSource;
	getStringSource<T>("fr", frSource);
	cl::Program frProgram(context, frSource);
	buildProgramWithOutputError(frProgram, {device}, std::cout);
	cl::Kernel frKernel = createKernelWithOuputError(frProgram, "fr", std::cout);

	size_t* isEvenNumberOfEqs = new size_t[(size_t)(log(size)/log(2)+2)];
	size_t numberOfEqs = size;
	size_t numberOfStep = 1;
	//frKernel.setArg(0, sizeof(T)*numberOfTerms, terms); //Why doesn't work?
	setKernelArgsWithOutputError(frKernel, 0, in, std::cout);
	/*Forward reduction*/
	for(; numberOfEqs>2; ++numberOfStep) {
		isEvenNumberOfEqs[numberOfStep-1] = numberOfEqs & 1;
		const size_t globalWorkSize[1] = {numberOfEqs/2};
		setKernelArgsWithOutputError(frKernel, 1, sizeof(cl_uint), &numberOfStep, std::cout);
		setKernelArgsWithOutputError(
				frKernel, 2 ,sizeof(cl_uint), &isEvenNumberOfEqs[numberOfStep-1], std::cout);
		if(int err = commandQueue.enqueueNDRangeKernel(
				frKernel, cl::NullRange, cl::NDRange(globalWorkSize[0]), cl::NullRange)){
			std::cout<<" Error creating kernel: error number " <<  err << "\n";
			exit(1);
		}
		numberOfEqs/=2;
	}
	--numberOfStep;
	commandQueue.enqueueReadBuffer(in, CL_TRUE, 0, numberOfTerms*sizeof(T), terms);
	printArray(std::cout, terms, numberOfTerms);

	/*Solve two unknowns system*/
	if(numberOfEqs==2){
		size_t eq1 = (1 << (numberOfStep)) - 1;
		size_t eq2 = (2 << (numberOfStep)) - 1;
		solveTwoUnknowsSystem({terms[(eq1)*4], terms[(eq2-1)*4 + 2], terms[(eq1)*4 + 3],
							   terms[(eq2)*4], terms[(eq1)*4 + 1], terms[(eq2)*4 + 1]},
							  unknowns[eq1], unknowns[eq2]);
	}else{
		size_t eq1 = (1 << (numberOfStep)) - 1;
		unknowns[eq1] = terms[(eq1)*4 + 1]/terms[(eq1)*4];
	}

	/*Backward substitution*/
	cl::Buffer unknownsBuf(context, CL_MEM_READ_WRITE, size*sizeof(T));
	/*Non blocking write can be faster?*/
	commandQueue.enqueueWriteBuffer(unknownsBuf, CL_TRUE, 0, size*sizeof(T), unknowns.get());

	std::string bsSource;
	getStringSource<T>("bs", bsSource);
	cl::Program bsProgram(context, bsSource);

	buildProgramWithOutputError(bsProgram, {device}, std::cout);
	cl::Kernel bsKernel = createKernelWithOuputError(bsProgram, "bs", std::cout);
	setKernelArgsWithOutputError(bsKernel, 0, in, std::cout);
	setKernelArgsWithOutputError(bsKernel, 1, unknownsBuf, std::cout);
	for(; numberOfStep>0; --numberOfStep) {
		const size_t globalWorkSize[1] = {numberOfEqs+isEvenNumberOfEqs[numberOfStep-1]};
		setKernelArgsWithOutputError(bsKernel, 2, sizeof(cl_uint), &numberOfStep, std::cout);
		setKernelArgsWithOutputError(
				bsKernel, 3, sizeof(cl_uint), &isEvenNumberOfEqs[numberOfStep-1], std::cout);
		if(int err = commandQueue.enqueueNDRangeKernel(
				bsKernel, cl::NullRange, cl::NDRange(globalWorkSize[0]), cl::NullRange)){
			std::cout<<" Error creating kernel: error number " <<  err << "\n";
			exit(1);
		}
		numberOfEqs = (numberOfEqs<<1) + isEvenNumberOfEqs[numberOfStep-1];
	}
	commandQueue.enqueueReadBuffer(unknownsBuf, CL_TRUE, 0, size*sizeof(T), unknowns.get());
	isSolve = true;
	printArray(std::cout, unknowns.get(), size);
}


template<class T>
int TridigionalEquation<T>::inverse(){


	cl_device_id device;
	cl_context context;
	ClInit::init(&device, &context);
	cl_command_queue commandQueue = clCreateCommandQueue(context, device, NULL, NULL);
	/*checkWith CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/

	/* Create buffers*/
	/* Create kernel arguments */
	cl_mem inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers,
			outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr, outBufFreeMembers;

	std::vector<size_t> sizes = {
			(size - 1)*sizeof(T), size*sizeof(T),
			(size - 1)*sizeof(T), size*sizeof(T) };

	createBuffers(context, CL_MEM_READ_WRITE, sizes,
				  {&inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers});

//	createBuffers(context, CL_MEM_READ_WRITE, sizes,
//				  {&outBufTopDiagCr, &outBufMidDiagCr, &outBufDownDiagCr, &outBufFreeMembers});
//

	T* outTopDiag = new T[size - 1], *outMidDiag = new T[size],
			*outDownDiag = new T[size - 1], *outConstTerms = new T[size];

	writeBuffers (commandQueue, sizes,
				  { inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers },
				  { topDiag, midDiag, downDiag, constTerms });

	/*Create kernels*/
	cl_program program = getFrKernel<T>(context, &device);
	cl_kernel frKernel = clCreateKernel(program, "fr", NULL);
	setKernelArguments (frKernel, sizeof(cl_mem),
						{ &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers });


	/*Forward reduction*/
	size_t* isEvenNumberOfEqs = new size_t[(size_t)(log(size)/log(2)+2)];
	size_t numberOfEqs = size;
	size_t numberOfStep = 1;
	for(; numberOfEqs>2; ++numberOfStep) {
		isEvenNumberOfEqs[numberOfStep-1] = numberOfEqs & 1;
		const size_t globalWorkSize[1] = {numberOfEqs/2};
		clSetKernelArg(frKernel, 4, 4, &numberOfStep);
		clSetKernelArg(frKernel, 5, 4, &isEvenNumberOfEqs[numberOfStep-1]);
		clEnqueueNDRangeKernel( commandQueue, frKernel,
								1, NULL, globalWorkSize, 0, 0, NULL, NULL);
		numberOfEqs/=2;
	}
	--numberOfStep;

	readBuffers (commandQueue, sizes,
				 {inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers},
				 { outTopDiag, outMidDiag, outDownDiag, outConstTerms });

	if(numberOfEqs==2){
		size_t eq1 = (1 << (numberOfStep)) - 1;
		size_t eq2 = (2 << (numberOfStep)) - 1;
		solveTwoUnknowsSystem({outMidDiag[eq1], outDownDiag[eq2-1], outTopDiag[eq1],
							   outMidDiag[eq2], outConstTerms[eq1], outConstTerms[eq2]},
							  unknowns[eq1], unknowns[eq2]);
	}else{   /*numberOfEqs==1*/
		size_t eq1 = (1 << (numberOfStep)) - 1;
		unknowns[eq1] = outConstTerms[eq1]/outMidDiag[eq1];
	}


	/*Backward substitution*/
	cl_mem unknownsBuff =
			clCreateBuffer(context, CL_MEM_READ_WRITE, size*sizeof(T), NULL, NULL);
	clEnqueueWriteBuffer(
			commandQueue, unknownsBuff, true, 0, size*sizeof(T), unknowns.get(), 0, NULL, NULL);
	cl_program bsProgram = getBsKernel<T>(context, &device);

	cl_kernel bsKernel = clCreateKernel(bsProgram, "bs", NULL);

	setKernelArguments(bsKernel, sizeof(cl_mem),
					   { &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers
							   , &unknownsBuff });

	for(; numberOfStep>0; --numberOfStep) {
		const size_t globalWorkSize[1] = {numberOfEqs+isEvenNumberOfEqs[numberOfStep-1]};
		clSetKernelArg(bsKernel, 5, 4, &numberOfStep);
		clSetKernelArg(bsKernel, 6, 4, isEvenNumberOfEqs+numberOfStep-1);
		clEnqueueNDRangeKernel( commandQueue, bsKernel,
								1, NULL, globalWorkSize, 0, 0, NULL, NULL);
		numberOfEqs = (numberOfEqs<<1) + isEvenNumberOfEqs[numberOfStep-1];
	}

	clEnqueueReadBuffer(commandQueue, unknownsBuff, true,
						0, size * sizeof(T), unknowns.get(), 0, NULL, NULL);
	isSolve = true;
	clFinish(commandQueue);

	clReleaseKernel (frKernel);
	clReleaseKernel (bsKernel);
	clReleaseProgram(program);
	clReleaseProgram(bsProgram);
	releaseMemObject(
			{ inBufTopDiagCr, inBufMidDiagCr,  inBufDownDiagCr,
			  outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr });
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);

	delete [] outTopDiag;
	delete [] outMidDiag;
	delete [] outDownDiag;
	delete [] outConstTerms;

	return 0;
}


template<class T>
std::ostream& operator <<(std::ostream& os, const TridigionalEquation<T>& eq){

#ifdef HTS_DEBUG
	os << space.print(eq.midDiag[0]) << space.print(eq.topDiag[0]);
	os << space.printZero(eq.size - 2);

	os<< eq.constTerms[0] << std::endl;
	for(size_t i = 1; i < eq.size-1; ++i){
		size_t j = i;
		os << space.printZero(j-1);

		os << space.print(eq.downDiag[i-1]) << space.print(eq.midDiag[i])
			<< space.print(eq.topDiag[i]);

		long lasts = ((long) eq.size) - i - 2;

		if(lasts > 0)
			os << space.printZero(lasts);
		os << eq.constTerms[i] << std::endl;
	}
	os << space.printZero(eq.size - 2);

	os << space.print(eq.downDiag[eq.size-2])<< space.print(eq.midDiag[eq.size - 1]) <<
	  eq.constTerms[eq.size-1] << "\n\n\n";
#endif

	if(eq.isSolve) {
		os << "Unknowns: ";
		printArray(os, eq.unknowns.get(), eq.size);
	}else{
		os << "System is not solved";
	}
#ifdef HTS_DEBUG
	os << "\n\n\n";
#endif

#ifdef HTS_DEBUG
	printArray(os, eq.topDiag, eq.size-1);
	printArray(os, eq.midDiag, eq.size);
	printArray(os, eq.downDiag, eq.size-1);
	printArray(os, eq.constTerms, eq.size);
#endif
    return os;
}

template<class T>
TridigionalEquation<T>::TridigionalEquation(
		T* topDiag, T* midDiag, T* downDiag, T* constTerms, size_t size) :
		topDiag(topDiag), midDiag(midDiag), downDiag(downDiag),
		constTerms(constTerms), size(size), unknowns(new T[size]), isSolve(false) {}
template<class T>
size_t TridigionalEquation<T>::getSize(){
	return size;
}

template<class T>
void TridigionalEquation<T>::outMatrix(std::ostream& os){
	struct space{
		space(size_t spaces) : stSpaces(spaces){}

		std::string print(double f){
			std::string out = cutZeros(std::to_string(f));

			if(stSpaces > out.size())
				return out.append(printSpaces(stSpaces - out.size()));
			return out;
		}
		std::string printZero(size_t n){
			std::string out;

			while(n--){
				out.push_back('0');
				out.append(printChar(' ', stSpaces-1));
			}

			return out;
		}
		std::string printSpaces(size_t n) {
			return printChar(' ', n);
		}

		std::string cutZeros(std::string string){
			int i =0;
			size_t size = string.size();
			while(i < size && string.at(size - i - 1) == '0'){
				++i;
			}
			if(string.at(size-i-1)=='.')
				++i;
			return string.substr(0,size-i);
		}

	private:
		std::string printChar(char ch, size_t n){
			std::string out;

			while(n--)
				out.push_back(ch);
			return out;
		}

	private:
		size_t stSpaces;

	} space(17); //must be > max number of symbols in number

	os << space.print(midDiag[0]) << space.print(topDiag[0]);
	os << space.printZero(size - 2);

	os<< constTerms[0] << std::endl;
	for(size_t i = 1; i < size-1; ++i){
		size_t j = i;
		os << space.printZero(j-1);

		os << space.print(downDiag[i-1]) << space.print(midDiag[i])
		<< space.print(topDiag[i]);

		long lasts = ((long) size) - i - 2;

		if(lasts > 0)
			os << space.printZero(lasts);
		os << constTerms[i] << std::endl;
	}
	os << space.printZero(size - 2);

	os << space.print(downDiag[size-2])<< space.print(midDiag[size - 1]) <<
	constTerms[size-1] << "\n\n\n";
}
#endif //TRIDIGIONAL_EQUATION_HPP