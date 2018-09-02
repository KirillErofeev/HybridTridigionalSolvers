//#include <CL/cl.h>
#include <iostream>
//#include <cstring>
#include <fstream>
#include "initializer.hpp"

#define SUPPOSED_LOG_SIZE 1000

int convertToString(const char *filename, std::string& s)
{
	size_t size;
	char*  str;
	std::fstream f(filename, (std::fstream::in | std::fstream::binary));
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
	clReleaseProgram(program); //TODO debug
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
        size_t n = 0;
        clGetProgramBuildInfo(
                program, *device,   CL_PROGRAM_BUILD_STATUS  , SUPPOSED_LOG_SIZE, buildSt, &n);
        std::cout << buildSt << "   " << n << std::endl;
		delete [] buildSt ;
	}
	return program;

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

void createBuffers(cl_context context, cl_mem_flags flag, std::vector<size_t>& sizes,
				   std::vector<cl_mem*>&& buffers) {

	auto sizesIter = sizes.begin();
	for(auto iter = buffers.begin(); iter != buffers.end(); ++iter, ++sizesIter){
		**iter = clCreateBuffer(
				context, flag, *sizesIter, NULL, NULL);
		/*checkWith? CL_MEM_USE_HOST_PTR, CL_MEM_ALLOC_HOST_PTR, CL_MEM_COPY_HOST_PTR*/
	}
}

void setKernelArgsWithOutputError(
		cl::Kernel& kernel, size_t index, cl::Buffer& buffer, std::ostream& out){
	if(int err = kernel.setArg(index, buffer)){
		out<<" Error set kernel Arg: error number " << err << "\n";
		exit(1);
	}
}
void setKernelArgsWithOutputError(cl::Kernel& kernel, size_t index, size_t size, void* ptr, std::ostream& out){
	if(int err = kernel.setArg(index, size, ptr)){
		out<<" Error set kernel Arg: error number " << err << "\n";
		exit(1);
	}
}
int buildProgramWithOutputError(const cl::Program& program, const VECTOR_CLASS<cl::Device>& devices, std::ostream& out){
	if(program.build(devices)!=CL_SUCCESS){
		std::string log;
		for (int i = 0; i < devices.size(); ++i) {
			program.getBuildInfo<std::string>(devices[i], CL_PROGRAM_BUILD_LOG, &log);
			std::string deviceName; 
                        std::cout << "Number of devices =" << devices.size() << "\n" << 
                        "CL get info status " << devices[0].getInfo(CL_DEVICE_NAME, &deviceName) <<std::endl;
			out <<" Error building on " << deviceName << " \n" << log << "\n";
                        out << "Status =" << program.build(devices) << std::endl; 
		}
		//exit(1);
	}
	return program.build(devices);
}

cl::Kernel createKernelWithOuputError(const cl::Program& program,
									  const char* kernelName, std::ostream& out){
	int e=14;
	cl::Kernel kernel(program, kernelName, &e);
	if(e){
		out<<" Error creating kernel: error number " << e << "\n";
		exit(1);
	}
	return kernel;
}
