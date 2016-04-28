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

	cl_mem inBufTopDiagCr = clCreateBuffer(
		context, CL_MEM_READ_ONLY, (size - 1) * sizeof(float), NULL, NULL);
	/*checkWith? CL_MEM_USE_HOST_PTR, CL_MEM_ALLOC_HOST_PTR, CL_MEM_COPY_HOST_PTR*/

	/*creating Buffers for cr*/
	cl_mem inBufMidDiagCr = clCreateBuffer(
		context, CL_MEM_READ_ONLY , size * sizeof(float), NULL, NULL);
	cl_mem inBufDownDiagCr = clCreateBuffer(
		context, CL_MEM_READ_ONLY, (size-1) * sizeof(float), NULL, NULL);
	cl_mem inBufFreeMembers = clCreateBuffer(
		context, CL_MEM_READ_ONLY, size * sizeof(float), NULL, NULL);
	cl_mem outBufTopDiagCr = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, (size - 1) * sizeof(float), NULL, NULL);
	cl_mem outBufMidDiagCr = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, size * sizeof(float), NULL, NULL);
	cl_mem outBufDownDiagCr = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, (size - 1) * sizeof(float), NULL, NULL);
	cl_mem outBufFreeMembers = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, size * sizeof(float), NULL, NULL);

	cl_kernel crKernel = getKernelBySource(&device, context, "kernels/cr.cl");

	std::vector<size_t> sizes = {
		(size - 1)*sizeof(float), size*sizeof(float), 
		(size - 1)*sizeof(float), size*sizeof(float) };
	
	writeBuffers (commandQueue, sizes,
	{ inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr, inBufFreeMembers },
	{ topDiag, midDiag, downDiag, freeMembers });

	setKernelArguments (crKernel, sizeof(cl_mem),
	{ &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr, &inBufFreeMembers,
	  &outBufTopDiagCr, &outBufMidDiagCr, &outBufDownDiagCr, &outBufFreeMembers });

	const size_t globalWorkSize[1] = {3};
	clEnqueueNDRangeKernel(commandQueue, crKernel, 1, NULL, globalWorkSize, 0, 0, NULL, NULL);

	float* outTopDiag = new float[size - 1], *outMidDiag = new float[size],
		*outDownDiag = new float[size - 1], *outFreeMembers = new float[size];
	
	readBuffers (
		commandQueue, sizes, 
		{ outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr, outBufFreeMembers },
		{ topDiag, midDiag, downDiag, freeMembers });

	std::cout << *this;

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

	} space(9); //must be > max number of symbols in number

	os << space.print(eq.midDiag[0]) << space.print(eq.topDiag[0]);
	os << space.printZero(eq.size - 2);

	os<< eq.freeMembers[0] << std::endl;
	for(int i = 0; i < eq.size-3; ++i){
		int j = i;
		os << space.printZero(j);
		os << space.print(eq.downDiag[i]) << space.print(eq.midDiag[i]) << space.print(eq.topDiag[i]);
		
		int lasts = eq.size - i - 3;
		if(lasts > 0)
			os << space.printZero(lasts);
		os << eq.freeMembers[i] << std::endl;
	}

	os << space.printZero(eq.size - 2);

	os << space.print(eq.midDiag[eq.size-2])<< space.print(eq.topDiag[eq.size - 3]) << 
	  eq.freeMembers[eq.size-2] << "\n\n\n";
	
	for(int i =0; i<eq.size-2; ++i){
		os << eq.topDiag[i] << " ";
	}
	os << "\n";
	for(int i =0; i<eq.size-1; ++i){
		os << eq.midDiag[i] << " ";
	}
	os << "\n";
	for(int i =0; i<eq.size-2; ++i){
		os << eq.downDiag[i] << " ";
	}
	os << "\n";
	for(int i =0; i<eq.size-1; ++i){
		os << eq.freeMembers[i] << " ";
	}
	os << "\n";
}

// std::ostream& operator <<(std::ostream& os, TridigionalEquation& eq){
// 	struct space{

// 		space(size_t spaces) : stSpaces(spaces){}
// 		std::string print(float f){
// 			std::string out = std::to_string(f);
// 			return out.append(printSpaces( stSpaces - out.size()-1 ));
// 		}
// 		std::string printZero(size_t n){
// 			std::string out;
// 			while(n--){
// 				out.push_back('0');
// 				out.append(printChar(' ', stSpaces));
// 			}
// 			return out;
// 		}
// 		std::string printSpaces(size_t n) { return printChar(' ', n); }
// 		std::string printSpaces() { return printChar(' ', stSpaces); }

// 	private:	
// 		std::string printChar(char ch, size_t n){
// 			std::string out;
// 			while(n--)
// 				out.push_back(ch);
// 			return out;
// 		}

// 	private:
// 		size_t stSpaces;

// 	} s(8);

// 	const char* space = " ";
// 	os << eq.midDiag[0] << space << eq.topDiag[0] << space;
// 	int f = eq.size - 2;
// 	while(f-- > 0)
// 		os << "0" << space;

// 	os<< eq.freeMembers[0] << std::endl;
// 	for(int i = 0; i < eq.size-2; ++i){
// 		int j = i;
// 		while(j-- > 0){
// 			os << "0" << space;
// 		}
// 		os << eq.downDiag[i] << space << eq.midDiag[i] << space << eq.topDiag[i] << space;
// 		int l = eq.size - i - 3;
// 		while(l-->0){
// 			os << "0" << space;
// 		}
// 		os << eq.freeMembers[i] << std::endl;
// 	}
// 	int ls = eq.size - 2;
// 	while(ls--){
// 		os << "0" << space;
// 	}
// 	os << eq.midDiag[eq.size-1] << space << eq.topDiag[eq.size - 2] << space << 
// 	  eq.freeMembers[eq.size-1] << std::endl;
// }