#include "TridigionalEquation.hpp"
#include "test.hpp"
#include <string>
#include "initializer.hpp"
#include "ClInit.hpp"
#include "Kernels.hpp"
#include <iostream>
int main(int argc, char* argv[]){

	float inputTopDiag[] = { 2,5,3,3,2,1,5 };
	float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
	float inputDownDiag[] = { 3,1,4,1,2,2,1 };
	float inputFreeMembers[] = { 2, 14, 14, 35, 21, 34, 63, 27 };

	TridigionalEquation e(inputTopDiag, inputMiddleDiag, inputDownDiag, inputFreeMembers, 8);
    testWithFullOutput(e, std::cout);

    float inputTopDiag1[] = { 2,5,3,3,2,1 };
    float inputMiddleDiag1[] = { 1,4,2,5,2,4,3};
    float inputDownDiag1[] = { 3,1,4,1,2,2 };
    float inputFreeMembers1[] = { 2, 14, 14, 35, 21, 34, 28};

    TridigionalEquation e1 = {inputTopDiag1, inputMiddleDiag1, inputDownDiag1, inputFreeMembers1, 7};
      testWithFullOutput(e1, std::cout);

    float inputTopDiag2[] = { 2,5};
    float inputMiddleDiag2[] = { 1,4,2};
    float inputDownDiag2[] = { 3,1 };
    float inputFreeMembers2[] = { 2, 14, 5};

    TridigionalEquation e2 =
            {inputTopDiag2, inputMiddleDiag2, inputDownDiag2, inputFreeMembers2, 3};
    testWithFullOutput(e2, std::cout);

    float inputTopDiag3[] = { 2,5,3,3,2};
    float inputMiddleDiag3[] = { 1,4,2,5,2,4};
    float inputDownDiag3[] = { 3,1,4,1,2};
    float inputFreeMembers3[] = { 2, 14, 14, 35, 21, 28};

    TridigionalEquation e3 =
            {inputTopDiag3, inputMiddleDiag3, inputDownDiag3, inputFreeMembers3, 6};
    testWithFullOutput(e3, std::cout);

    float inputTopDiag4[] = { 2,5,3,3};
    float inputMiddleDiag4[] = { 1,4,2,5,2};
    float inputDownDiag4[] = { 3,1,4,1};
    float inputFreeMembers4[] = { 2, 14, 14, 35, 11};

    TridigionalEquation e4 = {inputTopDiag4, inputMiddleDiag4, inputDownDiag4, inputFreeMembers4, 5};
    testWithFullOutput(e4, std::cout);

    ClInit::release();
	return 0;
}