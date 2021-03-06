#ifndef TEST_HPP
#define TEST_HPP

#include <chrono>
#include <random>

void baseTest();
void baseTestCpp(cl::CommandQueue commandQueue);

template <class T>
void solveAndShow(TridigionalEquation<T> &eq, std::ostream &os){
    eq.inverse();
    os << eq;
}

#define MIN_RANGE -1e3
#define MAX_RANGE 1e3

template <class T>
void test(cl::CommandQueue& commandQueue, size_t numberOfEquation, size_t dimMin, size_t dimMax,
          std::chrono::duration<double>& diff, bool isGpuSolving=true, bool isTestForEqual=true){
    std::random_device rd;
    std::ranlux48_base gen(rd());
    std::uniform_real_distribution<T> floatGenerator(MIN_RANGE, MAX_RANGE);
    std::uniform_int_distribution<size_t> intGenerator(dimMin, dimMax);
    bool isFail = false;
    std::unique_ptr<double[]> averageErrorForAll;
    std::unique_ptr<double[]> maxErrors;
    double maxError = 0;
    if(!isTestForEqual) {
        averageErrorForAll.reset(new double[numberOfEquation]);
        maxErrors.reset(new double[numberOfEquation]);
    }
    for (size_t k = 0; k < numberOfEquation && (!isFail || !isTestForEqual); ++k) {
        size_t size = intGenerator(gen);
#ifndef WELL_CONDITIONED_TEST
        std::function<T(T)> f = [](T t){return t*100;}; 
        auto top = getRandomVector(size - 1, gen, floatGenerator);
        auto mid = getRandomVector(size, gen, floatGenerator, f);
        auto down = getRandomVector(size - 1, gen, floatGenerator);
        auto xs = getRandomVector(size, gen, floatGenerator);       
#else
    //std::function
    auto top = getRandomVector(size - 1, gen, floatGenerator);
    auto mid = getRandomVector(size, gen, floatGenerator, []());
    auto xs = getRandomVector(size, gen, floatGenerator);
    down = top;
#endif

        std::unique_ptr<T[]> constTerms(new T[size]);
        constTerms[0] = mid[0] * xs[0] + top[0] * xs[1];
        for (size_t i = 1; i < size - 1; ++i) {
                constTerms[i] = down[i - 1] * xs[i - 1] + mid[i] * xs[i] + top[i] * xs[i + 1];
        }
        constTerms[size - 1] = down[size - 2] * xs[size - 2] + mid[size - 1] * xs[size - 1];

        auto terms = make_terms(top.get(), mid.get(), down.get(), constTerms.get(), size);
        TridigionalEquation<T> e(commandQueue, terms.get(), size);
        auto start = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> t;
        if(isGpuSolving)
            e.tSolve(t);
        else
            e.tSolve(t);
        auto end = std::chrono::high_resolution_clock::now();
        if(!isGpuSolving)
            diff = end - start;
        else
            diff = t;

        if(isTestForEqual) {
            if (!equal(xs.get(), e.getUnknows().get(), size)) {
                std::cout << "Failed!" << std::endl;
                std::cout << "Test number " << k << std::endl;
                if (size < 317) {
                    std::cout << "Actual uknowns: " << std::endl;
                    printArray(std::cout, xs.get(), size);
                    std::cout << "Received unknows:" << std::endl;
                    printArray(std::cout, e.getUnknows().get(), size);
                }
                isFail = true;
            }
            if (size < 7) {
                e.outMatrix(std::cout);
                std::cout << "Actual uknowns: " << std::endl;
                printArray(std::cout, xs.get(), size);
                std::cout << "Received unknows:" << std::endl;
                printArray(std::cout, e.getUnknows().get(), size);
            }
        }else{
            averageErrorForAll[k] = averageDifference(xs.get(), e.getUnknows().get(), size);
            double maxErrorEq = maxDifference(xs.get(), e.getUnknows().get(), size);
            maxErrors[k] = maxErrorEq;
            if(maxErrorEq > maxError)
                maxError = maxErrorEq;
        }
    }
    if(!isFail && isTestForEqual)
        std::cout << "Tests Successful!" << std::endl;
    if(!isTestForEqual){
        std::cout << "Average error is " << average(averageErrorForAll.get(), numberOfEquation) << std::endl;
        std::cout << "Max error is " << maxError << std::endl;
        std::cout << "Average max errors is " << average(maxErrors.get(), numberOfEquation) << std::endl;
    }
}

#endif
