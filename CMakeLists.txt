cmake_minimum_required(VERSION 3.5)
project(HybridTridigionalSolver_)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-declarations")

set(ATISTREAMSDKROOT /home/love/Programs/OpenCL/AMDAPPSDK-3.0)

set(clLib /home/love/Programs/OpenCL/AMDAPPSDK-3.0/lib/x86_64/sdk/*.*)

add_library(cl SHARED IMPORTED)
set_property(TARGET cl PROPERTY
        IMPORTED_LOCATION /home/love/Programs/OpenCL/AMDAPPSDK-3.0/lib/x86_64/sdk/libOpenCL.so)

include_directories(${ATISTREAMSDKROOT}/include)

set(SOURCE_FILES main.cpp ClInit.cpp initializer.cpp test.cpp)

add_executable(HybridTridigionalSolver_ ${SOURCE_FILES})
target_link_libraries(HybridTridigionalSolver_  cl)