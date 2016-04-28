#ifndef INITIALIZER_HPP
#define INITIALIZER_HPP

#include <vector>
#include <CL/cl.h>
#include <string>

/*initialize OpenCL and return context and device*/
int initCl(cl_device_id* device, cl_context* context);
cl_kernel getKernelBySource(
	cl_device_id* device, cl_context context, const char* sourceName);
void releaseMemObject(std::vector<cl_mem>&& vec);
cl_program getBuildBySource(const char* sourceName, cl_context context, const cl_device_id* device);
void writeBuffers(cl_command_queue commandQueue, std::vector<size_t>& sizes,
 std::vector<cl_mem>&& bufers, std::vector<void*>&& outs);
void readBuffers(cl_command_queue commandQueue, std::vector<size_t>& sizes,
 std::vector<cl_mem>&& bufers, std::vector<void*>&& outs);
void setKernelArguments(cl_kernel kernel, size_t size, std::vector<void *>&& kernelArgs);
int convertToString(const char *filename, std::string& s);
#endif
