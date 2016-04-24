#include <iostream>
#include <CL/cl.h>
#include "TridigionalEquation.hpp"
#include "initializer.hpp"

TridigionalEquation::TridigionalEquation(float* topDiag, float* midDiag, float* downDiag, size_t size)
	: topDiag(topDiag), midDiag(midDiag), downDiag(downDiag), size(size) {}

int TridigionalEquation::inverse(float* returns) {
	cl_device_id device;
	cl_context context;

	initCl(&device, &context);
	cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0, NULL);
	/*checkWith CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE*/

	cl_mem inBufTopDiagCr = clCreateBuffer(
		context, CL_MEM_READ_ONLY, (size - 1) * sizeof(float), NULL, NULL);
	/*checkWith? CL_MEM_USE_HOST_PTR, CL_MEM_ALLOC_HOST_PTR, CL_MEM_COPY_HOST_PTR*/

	/*creating Buffers for cr*/
	cl_mem inBufMidDiagCr = clCreateBuffer(
		context, CL_MEM_READ_ONLY , size * sizeof(float), NULL, NULL);
	cl_mem inBufDownDiagCr = clCreateBuffer(
		context, CL_MEM_READ_ONLY, (size-1) * sizeof(float), NULL, NULL);
	cl_mem outBufTopDiagCr = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, (size - 1) * sizeof(float), NULL, NULL);
	cl_mem outBufMidDiagCr = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, size * sizeof(float), NULL, NULL);
	cl_mem outBufDownDiagCr = clCreateBuffer(
		context, CL_MEM_WRITE_ONLY, (size - 1) * sizeof(float), NULL, NULL);


	cl_kernel crKernel = getKernelBySource(&device, context, "kernels/cr.cl");

	std::vector<size_t> sizes = {
		(size - 1)*sizeof(float), size*sizeof(float), (size - 1)*sizeof(float) };
	
	writeBuffers (commandQueue, sizes,
	{ inBufTopDiagCr, inBufMidDiagCr, inBufDownDiagCr }, { topDiag, midDiag, downDiag });

	setKernelArguments (crKernel, sizeof(cl_mem),
	{ &inBufTopDiagCr, &inBufMidDiagCr, &inBufDownDiagCr,
	  &outBufTopDiagCr, &outBufMidDiagCr, &outBufDownDiagCr });

	const size_t globalWorkSize[1] = { size };
	clEnqueueNDRangeKernel(commandQueue, crKernel, 1, NULL, globalWorkSize, 0, 0, NULL, NULL);

	float* outTopDiag = new float[size - 1], *outMidDiag = new float[size],
		*outDownDiag = new float[size - 1];
	
	readBuffers (
		commandQueue, sizes, { outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr },
		{ outTopDiag, outMidDiag, outDownDiag });

	for (size_t i = 0; i < size-1; ++i) {
		int j = i;
		while (j--) {
			std::cout << "0 ";
		}
		std::cout << *(outTopDiag+i) << " " << 
			*(outMidDiag + i) <<" " << *(outDownDiag + i) << std::endl;
	}
	std::cout << 0 << " " << *(outMidDiag + size-1) << " " << 0 << std::endl;
	
	clReleaseKernel (crKernel);
	releaseMemObject(
	{ inBufTopDiagCr, inBufMidDiagCr,  inBufDownDiagCr, 
	  outBufTopDiagCr, outBufMidDiagCr, outBufDownDiagCr });
	clReleaseCommandQueue(commandQueue);
	clReleaseContext(context);
	return 0;
}
