#ifndef HYBRIDTRIDIGIONALSOLVER_KERNELS_HPP
#define HYBRIDTRIDIGIONALSOLVER_KERNELS_HPP

#include <CL/cl.h>
#include "initializer.hpp"
#include "ClInit.hpp"

struct Kernels {

    static cl_kernel getBsKernel();
    static cl_kernel getFrKernel();

    static void release(){
        delete frKernel;
        delete bsKernel;
    }

private:
    static cl_kernel* frKernel;
    static cl_kernel* bsKernel;
};

#endif //HYBRIDTRIDIGIONALSOLVER_KERNELS_HPP
