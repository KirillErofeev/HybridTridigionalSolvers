#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <vector>
#include <iostream>
#include <cmath>
#include <limits>
#include <memory>

#define EPS 1e-3

template<class T>
void solveTwoUnknowsSystem(std::vector<T>&& coef, T& out1, T& out2){
    float md = (-coef[1] * coef[2] + coef[3] * coef[0]);
    out2 = (-coef[1] * coef[4] + coef[5] * coef[0]) / md;
    out1 = (-coef[5] * coef[2] + coef[4] * coef[3]) / md;

}
template<typename T>
void printArray(std::ostream& os,  const T* arr, size_t size){
    for (int i = 0; i < size; ++i) {
        os << arr[i] << " ";
    }
    os << std::endl;
}

template<class T, class = typename std::enable_if<std::is_floating_point<T>::value>::type>
bool equal(T x, T y){
    return std::fabs(x - y) < EPS;
}
template<class T, class =
        typename std::enable_if<std::is_floating_point<T>::value>::type>
bool equal(const  T*&& x, const  T*&& y, size_t size){
    for (int i = 0; i < size; ++i) {
        if(!equal(x[i], y[i])){
            std::cout << x[i] << " " << y[i];
            return false;
        }
    }
    return true;
}

template<class T, class ...Args>
std::unique_ptr<T> make_unique(Args&&... args){
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
};

template<class T, class Gen, template <class> class Dist, class F = T(T)>
std::unique_ptr<T[]> getRandomVector(size_t n, Gen& gen, Dist<T>& dist, F f = [](T o){return o;}){
    std::unique_ptr<T[]> out(new T[n]);
    for (size_t i = 0; i<n; ++i) {
        out[i] = f(dist(gen));
    }
    return out;
}

template<class T>
double average(T* arr, size_t size){
    double sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += arr[i];
    }
//    std::cout << sum << " " << size << std::endl;
    return sum/size;
}

template<class T>
T averageDifference(T* x, T* y, size_t size){
    T sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += std::abs(x[i]-y[i]);
    }
    return sum/size;
}

template<class T>
T maxDifference(T* x, T* y, size_t size){
    T max = 0;
    for (size_t i = 0; i < size; ++i) {
        if(std::abs(x[i]-y[i])>max)
            max = std::abs(x[i]-y[i]);
    }
    return max;
}

template<class T>
std::unique_ptr<T[]> make_terms(T* topDiag, T* midDiag, T* downDiag, T* constTerms, size_t size){

    std::unique_ptr<T[]> out(new T[size * 4 - 2]);
    for (int i = 0; i < size * 4 - 4; i+=4) {
        out[i] = midDiag[i/4];
        out[i+1] = constTerms[i/4];
        out[i+2] = downDiag[i/4];
        out[i+3] = topDiag[i/4];
    }
    out[size*4 - 4] = midDiag[size-1];
    out[size*4 - 3] = constTerms[size-1];
    return out;
}

#endif
