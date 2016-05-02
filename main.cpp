#include "TridigionalEquation.hpp"
#include <CL/cl.h>
#include <string>
#include "initializer.hpp"
#include <iostream>
int main(int argc, char* argv[]){

	float inputTopDiag[] = { 2,5,3,3,2,1,5 };
	float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
	float inputDownDiag[] = { 3,1,4,1,2,2,1 };
	float inputFreeMembers[] = { 2, 14, 14, 35, 21, 34, 63, 27 };

	TridigionalEquation e(inputTopDiag, inputMiddleDiag, inputDownDiag, inputFreeMembers, 8);

	std::cout << e << std::endl;
	float* returns = new float[8];

	e.inverse(returns);

	std::cout << e << std::endl;

	return 0;
}