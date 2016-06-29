#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

#include <vector>
#include <CL/cl.h>
#include <string>
#include <cassert>
#include <CL/cl.hpp>

/*initialize OpenCL and return context and device*/
//int initCl(cl_device_id* device, cl_context* context);
cl_kernel getKernelBySource(
	cl_device_id* device, cl_context context, const char* sourceName);
void releaseMemObject(std::vector<cl_mem>&& vec);
cl_program getBuildBySource(const char* sourceName, cl_context context, const cl_device_id* device);
void writeBuffers(cl_command_queue commandQueue, std::vector<size_t>& sizes,
 std::vector<cl_mem>&& bufers, std::vector<void*>&& outs);
void readBuffers(cl_command_queue commandQueue, std::vector<size_t>& sizes,
 std::vector<cl_mem>&& bufers, std::vector<void*>&& outs);
void setKernelArguments(cl_kernel kernel, size_t size, std::vector<void *>&& kernelArgs);
void createBuffers(cl_context context, cl_mem_flags flag, std::vector<size_t>&& sizes,
				   std::vector<cl_mem*>&& buffers);
void createBuffers(cl_context context, cl_mem_flags flag, std::vector<size_t>& sizes,
				   std::vector<cl_mem*>&& buffers);
int convertToString(const char *filename, std::string& s);

void buildProgramWithOutputError(
		const cl::Program& program, const VECTOR_CLASS<cl::Device>& devices, std::ostream& out);

cl::Kernel createKernelWithOuputError(const cl::Program& program,
									  const char* kernelName, std::ostream& out);
void setKernelArgsWithOutputError(cl::Kernel& kernel, size_t index, cl::Buffer& buffer, std::ostream& out);

void setKernelArgsWithOutputError(cl::Kernel& kernel, size_t index, size_t size, void* ptr, std::ostream& out);
template <class T>
void getStringSource(const std::string& sourceName, std::string& out){
	std::cout << "Missing template in getFrKernel call or Unknown type of kernel's args" << std::endl;
	assert(0);
}

template <>
inline void getStringSource<float>(const std::string& sourceName, std::string& out){
	if(sourceName == "fr") {
		convertToString("kernels/frFloat.cl", out);
		return;
	}
	if(sourceName == "bs"){
		convertToString("kernels/bsFloat.cl", out);
		return;
	}
	std::cout << "Unknown name of source code" << std::endl;
	assert(0);
}

template <>
inline void getStringSource<double>(const std::string& sourceName, std::string& out){
	if(sourceName == "fr") {
		convertToString("kernels/frDouble.cl", out);
		return;
	}
	if(sourceName == "bs"){
		convertToString("kernels/bsDouble.cl", out);
		return;
	}
	std::cout << "Unknown name of source code" << std::endl;
	assert(0);
}

template<class T>
cl_program getFrKernel(cl_context context, const cl_device_id* device){
	std::cout << "Missing template in getFrKernel call or Unknown type of kernel's args" << std::endl;
	assert(0);
}

template<class T>
cl_program getBsKernel(cl_context context, const cl_device_id* device){
	std::cout << "Missing template in getBsKernel call or Unknown type of kernel's args" << std::endl;
	assert(0);
}

template<>
inline cl_program getFrKernel<float>(cl_context context, const cl_device_id* device){
//	static cl_program r = getBuildBySource("kernels/frFloat.cl", context, device);
	return getBuildBySource("kernels/old/frFloat.cl", context, device);
}

template<>
inline cl_program getFrKernel<double>(cl_context context, const cl_device_id* device){
//	static cl_program r = getBuildBySource("kernels/frDouble.cl", context, device);
	return getBuildBySource("kernels/old/frDouble.cl", context, device);
}

template<>
inline cl_program getBsKernel<float>(cl_context context, const cl_device_id* device){
//	static cl_program r = getBuildBySource("kernels/bsFloat.cl", context, device);
	return getBuildBySource("kernels/old/bsFloat.cl", context, device);
}

template<>
inline cl_program getBsKernel<double>(cl_context context, const cl_device_id* device){
//	static cl_program r = getBuildBySource("kernels/bsDouble.cl", context, device);
	return getBuildBySource("kernels/old/bsDouble.cl", context, device);
}

#endif
