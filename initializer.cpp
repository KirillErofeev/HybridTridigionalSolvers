#include <CL/cl.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "initializer.hpp"

#define SUPPOSED_LOG_SIZE 1000

int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));
//	f.open(filename, (std::fstream::in | std::fstream::binary));
	if (f.is_open())
	{
		size_t fileSize;
		f.seekg(0, std::fstream::end);
		size = fileSize = (size_t)f.tellg();
		f.seekg(0, std::fstream::beg);
		str = new char[size + 1];
		f.read(str, fileSize);
		f.close();
		str[size] = '\0';
		s = str;
//        std::cout << str;
		delete[] str;
		return 0;
	}
	std::cout << "Error: failed to open file\n:" << filename << std::endl;
	return 1;
}

cl_kernel getKernelBySource(cl_device_id* device, cl_context context, 
	const char* sourceName) {

	cl_program program = getBuildBySource(sourceName, context, device);
	cl_kernel kernel = clCreateKernel(program, "cr", NULL);
	clReleaseProgram(program);
	return kernel;
}

cl_program getProgramBySource(const char* sourceName, cl_context context) {
	std::string sourceStr;
	convertToString(sourceName, sourceStr);
	const char *source = sourceStr.c_str();
	size_t sourceSize[] = { strlen(source) };

	cl_program program = clCreateProgramWithSource(
		                      context, 1, &source, sourceSize, NULL);
	return program;
}

cl_program getBuildBySource(
	const char* sourceName, cl_context context, const cl_device_id* device) {
	
	cl_program program = getProgramBySource(sourceName, context);

	clBuildProgram(program, 1, device, NULL, NULL, NULL);

	cl_ulong buildStatus = 1;
	clGetProgramBuildInfo(program, *device, CL_PROGRAM_BUILD_STATUS, 8, (void *)&buildStatus, NULL);
	if (buildStatus != CL_BUILD_SUCCESS) {
		std::cout << "Compilation error!" << std::endl;
        char* buildSt = new char[SUPPOSED_LOG_SIZE];
        clGetProgramBuildInfo(program, *device, CL_PROGRAM_BUILD_LOG, SUPPOSED_LOG_SIZE, buildSt, NULL);
        std::cout << buildSt << std::endl;
	}
	return program;

}


int initCl(cl_device_id* device, cl_context* context) {
	/*Getting platforms and choose an available one.*/
	cl_uint numPlatforms;
	cl_platform_id platform = NULL;
	cl_int	status = clGetPlatformIDs(1, &platform, &numPlatforms);
	if (status != CL_SUCCESS || numPlatforms <= 0) {
		std::cout << "Error: Getting platforms!" << std::endl;
		return 1;
	}
	/*Choose device*/
	cl_uint		 numDevices = 0;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, device, &numDevices);
	if (numDevices == 0) {
		std::cout << "No GPU device available." << std::endl;
		status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, device, &numDevices);
		if (numDevices == 0) {
			std::cout << "No CPU device available." << std::endl;
			return 1;
		}
	}
    *context = clCreateContext(NULL, 1, device, NULL, NULL, NULL);
	return status;
}

void readBuffers(cl_command_queue commandQueue, std::vector<size_t>& sizes,
 std::vector<cl_mem>&& bufers, std::vector<void*>&& outs) {

	std::vector<size_t>::iterator sizesIter = sizes.begin();
	std::vector<void*>::iterator outsIter = outs.begin();

	for(auto iter = bufers.begin(); iter != bufers.end(); ++iter, ++outsIter, ++sizesIter){
		clEnqueueReadBuffer(commandQueue, *iter, true,
			0, *sizesIter, *outsIter, 0, NULL, NULL);
		/*checkWith? blocking read*/
	}
}

void writeBuffers(cl_command_queue commandQueue, std::vector<size_t>& sizes,
 std::vector<cl_mem>&& bufers, std::vector<void*>&& outs) {

	std::vector<size_t>::iterator sizesIter = sizes.begin();
	std::vector<void*>::iterator outsIter = outs.begin();
	for(auto iter = bufers.begin(); iter != bufers.end(); ++iter, ++outsIter,++sizesIter){
		clEnqueueWriteBuffer(
			commandQueue, *iter, true, 0, *sizesIter, *outsIter, 0, NULL, NULL);
		/*checkWith? blocking write*/
	}
}

void releaseMemObject(std::vector<cl_mem>&& vec){
	for (std::vector<cl_mem>::iterator iter = vec.begin(); iter != vec.end(); ++iter){
		clReleaseMemObject(*iter);
	}
}

void setKernelArguments(
	cl_kernel kernel, size_t size, std::vector<void *>&& kernelArgs) {
	int i = 0;
	for (auto iter = kernelArgs.begin(); iter != kernelArgs.end(); ++iter, ++i) {
		clSetKernelArg(kernel, i, size, *iter);
		//*checkWith& size*/
	}
}

void createBuffers(cl_context context, cl_mem_flags flag, std::vector<size_t>&& sizes,
                    std::vector<cl_mem*>&& buffers) {

	auto sizesIter = sizes.begin();
	for(auto iter = buffers.begin(); iter != buffers.end(); ++iter, ++sizesIter){
        **iter = clCreateBuffer(
                context, flag, *sizesIter, NULL, NULL);
        /*checkWith? CL_MEM_USE_HOST_PTR, CL_MEM_ALLOC_HOST_PTR, CL_MEM_COPY_HOST_PTR*/
	}
}