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
	/*Destroy terms solve methods*/
	int inverse();
	/*Cyclic reduction method*/
	void solve();
	/*Gaussian elimination*/
	void tSolve();

	const std::unique_ptr<T[]>& getUnknows() const{
		return unknowns;
	}
	size_t getSize();
	friend std::ostream& operator<< <T> (std::ostream& os, const TridigionalEquation<T>& eq);
	void outMatrix(std::ostream&);
private:
	cl::CommandQueue commandQueue;
	T* terms;
	size_t size; // is number of equation
	std::unique_ptr<T[]> unknowns;
	size_t numberOfTerms; // number of elements in terms
	bool isSolve;
};
template<class T>
TridigionalEquation<T>::TridigionalEquation(cl::CommandQueue commandQueue, T* terms, size_t size)
		: commandQueue(commandQueue), terms(terms), size(size),
		  	unknowns(new T[size]), numberOfTerms(4*size-2), isSolve(false){}

template<class T>
void TridigionalEquation<T>::solve(){
	cl::Device device;
	cl::Context context;
	commandQueue.getInfo<cl::Context>(CL_QUEUE_CONTEXT, &context);
	commandQueue.getInfo<cl::Device>(CL_QUEUE_DEVICE, &device);
	cl::Buffer in(context, CL_MEM_READ_WRITE, numberOfTerms*sizeof(T));
	/*Non blocking write can be faster?*/
	commandQueue.enqueueWriteBuffer(in, CL_TRUE, 0, numberOfTerms*sizeof(T), terms);//Map!
	static std::string frSource;
	static int g = getStringSource<T>("fr", frSource);
	static cl::Program frProgram(context, frSource);
	static int bpwedr = buildProgramWithOutputError(frProgram, {device}, std::cout);
	static cl::Kernel frKernel = createKernelWithOuputError(frProgram, "fr", std::cout);

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
	static std::string bsSource;
	static int dbs = getStringSource<T>("bs", bsSource);
	static cl::Program bsProgram(context, bsSource);

	static int bpwoe = buildProgramWithOutputError(bsProgram, {device}, std::cout);
	static cl::Kernel bsKernel = createKernelWithOuputError(bsProgram, "bs", std::cout);
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

}
template<class T>
std::ostream& operator <<(std::ostream& os, const TridigionalEquation<T>& eq){
	if(eq.isSolve) {
		os << "Unknowns: ";
		printArray(os, eq.unknowns.get(), eq.size);
	}else{
		os << "System is not solved";
	}
    return os;
}

template<class T>
size_t TridigionalEquation<T>::getSize(){
	return size;
}

template<class T>
void TridigionalEquation<T>::tSolve(){
	size--;
	terms[(0)*4 + 3] /= terms[(0)*4];
	terms[(0)*4 + 1] /= terms[(0)*4];

	for (int i = 1; i < size; i++) {
		terms[(i)*4 + 3] /= terms[(i)*4] - terms[(i)*4 + 2]*terms[(i-1)*4 + 3];
		terms[(i)*4 + 1] =
				(terms[(i)*4 + 1] - terms[(i)*4 + 2]*terms[(i-1)*4 + 1])
				/ (terms[(i)*4] - terms[(i)*4 + 2]*terms[(i-1)*4 + 3]);
	}

	terms[(size)*4 + 1] =
			(terms[(size)*4 + 1] - terms[(size)*4 + 2]*terms[(size-1)*4 + 1])
			/ (terms[(size)*4] - terms[(size)*4 + 2]*terms[(size-1)*4 + 3]);

	for (int i = size; i-- > 0;) {
		unknowns[(i)*4 + 1] -= terms[(i)*4 + 3]*terms[(i+1)*4 + 1];
	}
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

	} space(17);

	os << space.print(terms[(0)*4]) << space.print(terms[(0)*4 + 3]);
	os << space.printZero(size - 2);

	os<< terms[(0)*4 + 1] << std::endl;
	for(size_t i = 1; i < size-1; ++i){
		size_t j = i;
		os << space.printZero(j-1);

		os << space.print(terms[(i-1)*4 + 2]) << space.print(terms[(i)*4])
		<< space.print(terms[(i)*4 + 3]);

		long lasts = ((long) size) - i - 2;

		if(lasts > 0)
			os << space.printZero(lasts);
		os << terms[(i)*4 + 1] << std::endl;
	}
	os << space.printZero(size - 2);

	os << space.print(terms[(size-2)*4 + 2])<< space.print(terms[(size - 1)*4]) <<
	terms[(size-1)*4 + 1] << "\n\n\n";
}
#endif //TRIDIGIONAL_EQUATION_HPP