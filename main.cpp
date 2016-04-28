#include <iostream>
#include <CL/cl.h>
#include <string>
#include <cstring>
#include "TridigionalEquation.hpp"
#include "initializer.hpp"

int main(int argc, char* argv[]){
	float inputTopDiag[] = { 2,5,3,3,2,1,5 };
	float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
	float inputDownDiag[] = { 3,1,4,1,2,2,1 };

	TridigionalEquation e(inputTopDiag, inputMiddleDiag, inputDownDiag, 8);

	float* returns = new float[8];

	e.inverse(returns);

	delete [] returns;

	
	// cl_uint numPlatforms;
	// cl_platform_id platform = NULL;
	// cl_int	status = clGetPlatformIDs(1, &platform, &numPlatforms);
	
	// cl_uint		 numDevices = 0;
	// cl_device_id device;
	// status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, &numDevices);
	
	// cl_context context;
	// context = clCreateContext(NULL, 1, &device, NULL, NULL, NULL);
	// cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0, NULL);
	
	// cl_mem inBufTopDiagCr = clCreateBuffer(
	// 	context, CL_MEM_READ_ONLY, 8 * sizeof(int), NULL, NULL);

	// std::string sourceStr;
	// convertToString("HelloWorld_kernel.cl", sourceStr);
	// const char *source = sourceStr.c_str();
	// size_t sourceSize[] = { strlen(source) };

	// cl_program program = clCreateProgramWithSource(context, 1, &source, sourceSize, NULL);
	// clBuildProgram(program, 1, &device, NULL, NULL, NULL);
	
	// cl_ulong buildStatus = 1;
	// clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_STATUS, 8, (void *)&buildStatus, NULL);
	// if (buildStatus != CL_BUILD_SUCCESS)
	// 	std::cout << "Compilation error!" << std::endl;

	// cl_kernel kernel = clCreateKernel(program, "cr", NULL);

	return 0;
}

