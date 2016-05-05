#include "Kernels.hpp"

cl_kernel Kernels::getBsKernel(){
    if(bsKernel==nullptr){
        cl_device_id device;
        cl_context context;
        ClInit::init(&device, &context);
        bsKernel = new cl_kernel;
        cl_program  program = getBuildBySource("kernels/bs.cl", context, &device);
        *bsKernel = clCreateKernel(program, "bs", nullptr);
    }
    return *bsKernel;
}

cl_kernel Kernels::getFrKernel(){
    if(frKernel==nullptr){
        cl_device_id device;
        cl_context context;
        ClInit::init(&device, &context);
        frKernel = new cl_kernel;
        cl_program  program = getBuildBySource("kernels/fr.cl", context, &device);
        *frKernel = clCreateKernel(program, "fr", nullptr);
    }
    return *frKernel;
}

cl_kernel* Kernels::frKernel = nullptr;
cl_kernel* Kernels::bsKernel = nullptr;
