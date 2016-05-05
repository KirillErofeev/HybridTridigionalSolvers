#ifndef CL_INIT_HPP
#define CL_INIT_HPP

#include <iostream>
#include "CL/cl.h"
struct ClInit{

    static void init(cl_device_id* device, cl_context* context, cl_command_queue* commandQueue);
    static void init(cl_device_id* device, cl_context* context);
    static void release();

private:
   static cl_device_id* device;
   static cl_context* context;
   static cl_command_queue* commandQueue;

private:
    static int initCl(cl_device_id* device, cl_context* context, cl_command_queue* commandQueue);
    static int initCl(cl_device_id* device, cl_context* context);


};

#endif