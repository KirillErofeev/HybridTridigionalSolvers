#include <chrono>
//#include <random>
#include "TridigionalEquation.hpp"
#include "test.hpp"
#include "fstream"

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
    cl::Device device = devices[1];
    devices[1].getInfo(CL_DEVICE_NAME, &deviceName);
    std::cout << "Device " << deviceName << " was selected" << std::endl;
    while(devices.size()>=1)
        devices.pop_back();
    cl::Context context(device);

    cl::CommandQueue commandQueue(context, device);
    std::cout << "Test start!" << std::endl;
    //baseTestCpp(commandQueue);

    size_t n, dim;
    //std::vector<size_t> ns = {100, 500, 1000, 2000, 5000, 10000, 30000, 70000, 100000, 250000, 500000, 700000, 1000000, 2000000, 3000000, 5000000, 7000000, 8000000, 9000000, 10000000, 20000000, 30000000, 40000000, 50000000};
    std::vector<size_t> ns = {10000, 100000, 1000000};
    std::ofstream outF("all float1.txt");
    //std::ofstream outD("all double.txt");
    std::chrono::duration<double> diff;
    std::chrono::duration<double> diffDouble;
    for( auto n: ns){
        {
            //std::cin >> n >> dim;
            std::cout << "Test with float" << std::endl;
            test<float>(commandQueue, 10, n, n, diff, true, false);
            std::cout << diff.count()/10 << "\n";
            outF << n << "\t" << diff.count()/10 << "\t";
        }

       // {
       //     std::cout << "Test with double" << std::endl;
       //     test<double>(commandQueue, 10, n, n, diffDouble, true, false);
       //     std::cout << diffDouble.count()/10 << "\n";
       //     outD << n << "\t" << diffDouble.count()/10 << "\t";
       // }

        {
            std::cout << "Serial Test with float" << std::endl;
            test<float>(commandQueue, 10, n, n, diff, false, false);
            //std::cout << diff.count()/10 << "\n";
            outF << diff.count()/10 << "\n";
        }

       // {
       //     std::cout << "Serial Test with double" << std::endl;
       //     test<double>(commandQueue, 10, n, n, diffDouble, false, false);
       //     //std::cout << diffDouble.count()/10 << "\n";
       //     outD << "\t" << diffDouble.count()/10 << "\n";
       // }
    }
    
    return 0;
}
