#include <CL/cl.h>
#include <iostream>

cl_ulong printNumericInformation(cl_device_id device, cl_device_info paramName) {
	cl_ulong info = 0;
	clGetDeviceInfo(device, paramName, 8,
		&info, NULL);
	return info;
}

void printInterestingInformation(cl_platform_id platform, const cl_device_id* devices, size_t numDevices){
	for (size_t i = 0; i < numDevices; i++) {
		cl_device_id device = *(devices + i);
		size_t nameSize = 0;	 
		clGetDeviceInfo(device, CL_DEVICE_NAME, 0, 0, &nameSize);
		char* name = (char*) malloc(nameSize);
		clGetDeviceInfo(device, CL_DEVICE_NAME, nameSize, name, NULL);
		std::cout << "CL_DEVICE_GLOBAL_MEM_CACHE_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE) << std::endl;
		std::cout << "CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE) << std::endl;
		std::cout << "CL_DEVICE_GLOBAL_MEM_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_GLOBAL_MEM_SIZE) << std::endl;
		std::cout << "CL_DEVICE_LOCAL_MEM_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_LOCAL_MEM_SIZE) << std::endl;
		std::cout << "CL_DEVICE_MAX_COMPUTE_UNITS " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_COMPUTE_UNITS) << std::endl;
		std::cout << "CL_DEVICE_MAX_CONSTANT_ARGS " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_CONSTANT_ARGS) << std::endl;
		std::cout << "CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE) << std::endl;
		std::cout << "CL_DEVICE_MAX_COMPUTE_UNITS " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_COMPUTE_UNITS) << std::endl;
		std::cout << "CL_DEVICE_MAX_CONSTANT_ARGS " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_CONSTANT_ARGS) << std::endl;
		std::cout << "CL_DEVICE_MAX_PARAMETER_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_PARAMETER_SIZE) << std::endl;
		std::cout << "CL_DEVICE_MAX_WORK_GROUP_SIZE " << name << " " <<
			printNumericInformation(device, CL_DEVICE_MAX_WORK_GROUP_SIZE) << std::endl;
		std::cout << "CL_DEVICE_PROFILING_TIMER_RESOLUTION " << name << " " <<
			printNumericInformation(device, CL_DEVICE_PROFILING_TIMER_RESOLUTION) << std::endl;
	}
}