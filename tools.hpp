#ifndef TOOLS_HPP
#define TOOLS_HPP

#include <vector>
#include <iostream>
void solveTwoUnknowsSystem(std::vector<float>&& coef, float& out1, float& out2);

template<typename T>
void printArray(std::ostream& os, T* arr, unsigned size){
    for (int i = 0; i < size; ++i) {
        os << arr[i] << " ";
    }
    os << std::endl;
}

#endif
