#include "ClInit.hpp"
void ClInit::init(cl_device_id* device, cl_context* context, cl_command_queue* commandQueue){

    if(ClInit::device == nullptr) {
        ClInit::device = new cl_device_id;
        ClInit::context = new cl_context;
        ClInit::commandQueue = new cl_command_queue;

        initCl(ClInit::device, ClInit::context, ClInit::commandQueue);
    }

    *device = *ClInit::device;
    *context = *ClInit::context;
    *commandQueue = *ClInit::commandQueue;
}

void ClInit::init(cl_device_id* device, cl_context* context){

    if(ClInit::device == nullptr) {
        ClInit::device = new cl_device_id;
        ClInit::context = new cl_context;

        initCl(ClInit::device, ClInit::context);
    }

    *device = *ClInit::device;
    *context = *ClInit::context;
}

int ClInit::initCl(cl_device_id* device, cl_context* context, cl_command_queue* commandQueue) {

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
    *commandQueue = clCreateCommandQueueWithProperties(*context, *device, 0, NULL);
    return status;
}

int ClInit::initCl(cl_device_id* device, cl_context* context) {

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

void ClInit::release(){
    delete device;
    delete context;
    delete commandQueue;
}
cl_device_id* ClInit::device = nullptr;
cl_context* ClInit::context = nullptr;
cl_command_queue* ClInit::commandQueue = nullptr;