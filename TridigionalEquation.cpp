#include <iostream>
#include <string>
#include <CL/cl.h>
#include "TridigionalEquation.hpp"
#include "initializer.hpp"

TridigionalEquation::TridigionalEquation(
	float* topDiag, float* midDiag, float* downDiag, float* freeMembers, size_t size)
	: topDiag(topDiag), midDiag(midDiag), downDiag(downDiag),
	                    freeMembers(freeMembers), size(size) {}

int TridigionalEquation::inverse(float* returns) {

	cl_device_id device;
	cl_context context;

	initCl(&device, &context);
	cl_command_queue commandQueue = 
		clCreateCommandQueueWithProperties(context, device, 0, NULL);
	/*checkWith CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/

    /* Create buffers*/
	cl_mem inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers,
	       outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr, outBufFreeMembers;

	createBuffers(context, CL_MEM_READ_ONLY,
				  {(size - 1) * sizeof(float), size * sizeof(float),
				   (size-1) * sizeof(float), size * sizeof(float)},
				   {&inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers});

    createBuffers(context, CL_MEM_WRITE_ONLY,
                  {(size - 1) * sizeof(float), size * sizeof(float),
                   (size-1) * sizeof(float), size * sizeof(float)},
                  {&outBufTopDiagCr, &outBufMidDiagCr, &outBufDownDiagCr, &outBufFreeMembers});

    float* outTopDiag = new float[size - 1], *outMidDiag = new float[size],
            *outDownDiag = new float[size - 1], *outFreeMembers = new float[size];

    std::vector<size_t> sizes = {
            (size - 1)*sizeof(float), size*sizeof(float),
            (size - 1)*sizeof(float), size*sizeof(float) };

    writeBuffers (commandQueue, sizes,
                  { inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers },
                  { topDiag, midDiag, downDiag, freeMembers });

   /*Create kernels*/
	cl_kernel crKernel = getKernelBySource(
			&device, context, "/home/love/Projects/C/hpc/HybridTridigionalSolvers/kernels/cr.cl");

	setKernelArguments (crKernel, sizeof(cl_mem),
	{ &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers,
	  &outBufTopDiagCr, &outBufMidDiagCr, &outBufDownDiagCr, &outBufFreeMembers });


//    size_t numberOfEqs = size/2;
//    for(int i =0; numberOfEqs>2; i++) {
//        const size_t globalWorkSize[1] = {numberOfEqs};
//        clEnqueueNDRangeKernel(commandQueue, crKernel, 1, NULL, globalWorkSize, 0, 0, NULL, NULL);
//    }

    const size_t globalWorkSize[1] = {4};
        clEnqueueNDRangeKernel(commandQueue, crKernel, 1, NULL, globalWorkSize, 0, 0, NULL, NULL);
    
	readBuffers (
		commandQueue, sizes,
		{ outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr, outBufFreeMembers },
		{ topDiag, midDiag, downDiag, freeMembers });


	clReleaseKernel (crKernel);
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
		std::string printSpaces() { return printChar(' ', stSpaces-1); }

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
	
	for(int i =0; i<eq.size-1; ++i){
		os << eq.topDiag[i] << " ";
	}
	os << "\n";
	for(int i =0; i<eq.size; ++i){
		os << eq.midDiag[i] << " ";
	}
	os << "\n";
	for(int i =0; i<eq.size-1; ++i){
		os << eq.downDiag[i] << " ";
	}
	os << "\n";
	for(int i =0; i<eq.size; ++i){
		os << eq.freeMembers[i] << " ";
	}
	os << "\n";
}
