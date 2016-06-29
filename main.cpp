#include <chrono>
#include <random>
#include "TridigionalEquation.hpp"
#include "test.hpp"

int main(int argc, char* argv[]){
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    if(platforms.size()<=0){
        std::cout << "No platforms are available" << std::endl;
        return 1;
    }
    std::vector<cl::Device> devices;
    platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices);
    if(devices.size()<=0) {
        std::cout << "No GPU are available, CPU device will be selected" << std::endl;
        platforms[0].getDevices(CL_DEVICE_TYPE_CPU, &devices);
        if(devices.size()<=0) {
            std::cout << "No CPU are available" << std::endl;
            return 1;
        }
    }
    std::string deviceName;
    devices[0].getInfo(CL_DEVICE_NAME, &deviceName);
    std::cout << "Device " << deviceName << " was selected" << std::endl;
    while(devices.size()>=1)
        devices.pop_back();
    cl::Context context(devices[0]);

    cl::CommandQueue commandQueue(context, devices[0]);
    baseTestCpp(commandQueue);

    float inputTopDiag[] = { 2,5,3,3,2,1,5 };
    float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
    float inputDownDiag[] = { 3,1,4,1,2,2,1 };
    float inputFreeMembers[] = { 2, 14, 14, 35, 21, 34, 63, 27 };

    auto terms = make_terms(inputTopDiag, inputMiddleDiag, inputDownDiag, inputFreeMembers, 8);
    TridigionalEquation<float> e(
            commandQueue,
            terms.get(), 8);

    e.solve();

//    std::chrono::duration<double> diff;
//
//    size_t n, dim;
//    std::cin >> n >> dim;
//    std::cout << "Test with float" << std::endl;
//
//    test<float>(n, dim, dim, diff, false);
//    std::cout << diff.count() << "\n";
//
//    std::chrono::duration<double> diffDouble;
//    std::cout << "Test with double" << std::endl;
//    test<double>(n, dim, dim, diffDouble, false);
//    std::cout << diffDouble.count() << "\n";
//
//    ClInit::release();
	return 0;
}