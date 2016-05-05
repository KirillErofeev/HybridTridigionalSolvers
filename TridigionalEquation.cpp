#include <iostream>
#include <CL/cl.h>
#include <cmath>
#include "TridigionalEquation.hpp"
#include "initializer.hpp"
#include "tools.hpp"
#include "ClInit.hpp"

TridigionalEquation::TridigionalEquation(
	float* topDiag, float* midDiag, float* downDiag, float* freeMembers, size_t size) :
		topDiag(topDiag), midDiag(midDiag), downDiag(downDiag),
		freeMembers(freeMembers), size(size), isSolve(false) { unknows = new float[size];}

TridigionalEquation::~TridigionalEquation(){
	delete [] unknows;
}
int TridigionalEquation::inverse(){

	cl_device_id device;
	cl_context context;
	ClInit::init(&device, &context);
	cl_command_queue commandQueue = clCreateCommandQueueWithProperties(context, device, NULL, NULL);
	/*checkWith CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/

    /* Create buffers*/
    /* Create kernel arguments */
    cl_mem inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers,
	       outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr, outBufFreeMembers;

    std::vector<size_t> sizes = {
            (size - 1)*sizeof(float), size*sizeof(float),
            (size - 1)*sizeof(float), size*sizeof(float) };

	createBuffers(context, CL_MEM_READ_WRITE, sizes,
				   {&inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers});

    createBuffers(context, CL_MEM_READ_WRITE, sizes,
                  {&outBufTopDiagCr, &outBufMidDiagCr, &outBufDownDiagCr, &outBufFreeMembers});


    float* outTopDiag = new float[size - 1], *outMidDiag = new float[size],
            *outDownDiag = new float[size - 1], *outFreeMembers = new float[size];

    writeBuffers (commandQueue, sizes,
                  { inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers },
                  { topDiag, midDiag, downDiag, freeMembers });

    /*Create kernels*/
    cl_program program = getBuildBySource(
            "kernels/fr.cl", context, &device);
	cl_kernel frKernel = clCreateKernel(program, "fr", NULL);
	setKernelArguments (frKernel, sizeof(cl_mem),
	{ &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers });


    /*Forward reduction*/

    size_t* isEvenNumberOfEqs = new size_t[(size_t) (log(size)/log(2)+2)];

    size_t numberOfEqs = size;
    size_t numberOfStep = 1;
    for(; numberOfEqs>2; ++numberOfStep) {
        isEvenNumberOfEqs[numberOfStep-1] = numberOfEqs & 1;
        const size_t globalWorkSize[1] = {numberOfEqs/2};
        clSetKernelArg(frKernel, 4, 4, &numberOfStep);
        clEnqueueNDRangeKernel( commandQueue, frKernel,
                                1, NULL, globalWorkSize, 0, 0, NULL, NULL);
        numberOfEqs/=2;
    }
    --numberOfStep;


//    readBuffers (commandQueue, sizes,
//                {inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers},
//                { topDiag, midDiag, downDiag, freeMembers });

    readBuffers (commandQueue, sizes,
                 {inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers},
                 { outTopDiag, outMidDiag, outDownDiag, outFreeMembers });

    if(numberOfEqs==2){
        size_t eq1 = (1 << (numberOfStep)) - 1;
        size_t eq2 = (2 << (numberOfStep)) - 1;
        solveTwoUnknowsSystem({outMidDiag[eq1], outDownDiag[eq2-1], outTopDiag[eq1],
                               outMidDiag[eq2], outFreeMembers[eq1], outFreeMembers[eq2]},
                              unknows[eq1], unknows[eq2]);
    }else{   /*numberOfEqs==1*/
        size_t eq1 = (1 << (numberOfStep)) - 1;
		unknows[eq1] = outFreeMembers[eq1]/outMidDiag[eq1];
    }

    /*Backward substitution*/
    cl_mem unknowsBuff =
            clCreateBuffer(context, CL_MEM_READ_WRITE, size*sizeof(float), NULL, NULL);
    clEnqueueWriteBuffer(
            commandQueue, unknowsBuff, true, 0, size*sizeof(float), unknows, 0, NULL, NULL);

    cl_program bsProgram = getBuildBySource(
            "kernels/bs.cl", context, &device);
    cl_kernel bsKernel = clCreateKernel(bsProgram, "bs", NULL);

    setKernelArguments(bsKernel, sizeof(cl_mem),
                       { &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers
                               , &unknowsBuff });

    for(; numberOfStep>0; --numberOfStep) {
        const size_t globalWorkSize[1] = {numberOfEqs+isEvenNumberOfEqs[numberOfStep-1]};
        clSetKernelArg(bsKernel, 5, 4, &numberOfStep);
        clSetKernelArg(bsKernel, 6, 4, isEvenNumberOfEqs+numberOfStep-1);
        clEnqueueNDRangeKernel( commandQueue, bsKernel,
                                1, NULL, globalWorkSize, 0, 0, NULL, NULL);
            numberOfEqs = (numberOfEqs<<1) + isEvenNumberOfEqs[numberOfStep-1];
    }
    clEnqueueReadBuffer(commandQueue, unknowsBuff, true,
                        0, size * sizeof(float), unknows, 0, NULL, NULL);

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
	delete [] outFreeMembers;


	return 0;
}



std::ostream& operator <<(std::ostream& os, TridigionalEquation& eq){
	struct space{

		space(size_t spaces) : stSpaces(spaces){}
		std::string print(float f){
			std::string out = cutZeros(std::to_string(f));
			return out.append(printSpaces(stSpaces - out.size() ));
		}
		std::string printZero(size_t n){
			std::string out;
			while(n--){
				out.push_back('0');
				out.append(printChar(' ', stSpaces-1));
			}
			return out;
		}
		std::string printSpaces(size_t n) { return printChar(' ', n); }

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

	} space(10); //must be > max number of symbols in number

	os << space.print(eq.midDiag[0]) << space.print(eq.topDiag[0]);
	os << space.printZero(eq.size - 2);

	os<< eq.freeMembers[0] << std::endl;
	for(size_t i = 1; i < eq.size-1; ++i){
		size_t j = i;
		os << space.printZero(j-1);
		os << space.print(eq.downDiag[i-1]) << space.print(eq.midDiag[i]) << space.print(eq.topDiag[i]);

		int lasts = eq.size - i - 2;
		if(lasts > 0)
			os << space.printZero(lasts);
		os << eq.freeMembers[i] << std::endl;
	}

	os << space.printZero(eq.size - 2);

	os << space.print(eq.downDiag[eq.size-2])<< space.print(eq.midDiag[eq.size - 1]) <<
	  eq.freeMembers[eq.size-1] << "\n\n\n";

	if(eq.isSolve) {
		os << "Unknows: ";
		printArray(os, eq.unknows, eq.size);
	}else{
		os << "System is not solved";
	}
    os << "\n\n\n";

	printArray(os, eq.topDiag, eq.size-1);
	printArray(os, eq.midDiag, eq.size);
	printArray(os, eq.downDiag, eq.size-1);
	printArray(os, eq.freeMembers, eq.size);
}
