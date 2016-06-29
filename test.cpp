#include <random>
#include "TridigionalEquation.hpp"
#include "test.hpp"

void baseTest(){
    float inputTopDiag[] = { 2,5,3,3,2,1,5 };
    float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
    float inputDownDiag[] = { 3,1,4,1,2,2,1 };
    float inputFreeMembers[] = { 2, 14, 14, 35, 21, 34, 63, 27 };

    TridigionalEquation<float> e(inputTopDiag, inputMiddleDiag, inputDownDiag, inputFreeMembers, 8);
    solveAndShow(e, std::cout);

    float inputTopDiag1[] = { 2,5,3,3,2,1 };
    float inputMiddleDiag1[] = { 1,4,2,5,2,4,3};
    float inputDownDiag1[] = { 3,1,4,1,2,2 };
    float inputFreeMembers1[] = { 2, 14, 14, 35, 21, 34, 28};

    TridigionalEquation<float> e1 = {inputTopDiag1, inputMiddleDiag1, inputDownDiag1, inputFreeMembers1, 7};
    solveAndShow(e1, std::cout);

    float inputTopDiag2[] = { 2,5};
    float inputMiddleDiag2[] = { 1,4,2};
    float inputDownDiag2[] = { 3,1 };
    float inputFreeMembers2[] = { 2, 14, 5};

    TridigionalEquation<float> e2 =
            {inputTopDiag2, inputMiddleDiag2, inputDownDiag2, inputFreeMembers2, 3};
    solveAndShow(e2, std::cout);

    float inputTopDiag3[] = { 2,5,3,3,2};
    float inputMiddleDiag3[] = { 1,4,2,5,2,4};
    float inputDownDiag3[] = { 3,1,4,1,2};
    float inputFreeMembers3[] = { 2, 14, 14, 35, 21, 28};

    TridigionalEquation<float> e3 =
            {inputTopDiag3, inputMiddleDiag3, inputDownDiag3, inputFreeMembers3, 6};
    solveAndShow(e3, std::cout);

    float inputTopDiag4[] = { 2,5,3,3};
    float inputMiddleDiag4[] = { 1,4,2,5,2};
    float inputDownDiag4[] = { 3,1,4,1};
    float inputFreeMembers4[] = { 2, 14, 14, 35, 11};

    TridigionalEquation<float> e4 = {inputTopDiag4, inputMiddleDiag4, inputDownDiag4, inputFreeMembers4, 5};
    solveAndShow(e4, std::cout);
}

void baseTestCpp(cl::CommandQueue commandQueue){
    float inputTopDiag[] = { 2,5,3,3,2,1,5 };
    float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
    float inputDownDiag[] = { 3,1,4,1,2,2,1 };
    float inputFreeMembers[] = { 2, 14, 14, 35, 21, 34, 63, 27 };

        auto terms = make_terms(inputTopDiag, inputMiddleDiag, inputDownDiag, inputFreeMembers, 8);
        TridigionalEquation<float> e(
                commandQueue,
        terms.get(), 8);

//        e.solve();
//
//    float inputTopDiag1[] = { 2,5,3,3,2,1 };
//    float inputMiddleDiag1[] = { 1,4,2,5,2,4,3};
//    float inputDownDiag1[] = { 3,1,4,1,2,2 };
//    float inputFreeMembers1[] = { 2, 14, 14, 35, 21, 34, 28};
//
//    TridigionalEquation<float> e1(
//            commandQueue,
//            make_terms(inputTopDiag1, inputMiddleDiag1, inputDownDiag1, inputFreeMembers1, 7).get()
//            , 7);
//    e1.solve();
//
//    float inputTopDiag2[] = { 2,5};
//    float inputMiddleDiag2[] = { 1,4,2};
//    float inputDownDiag2[] = { 3,1 };
//    float inputFreeMembers2[] = { 2, 14, 5};
//
//    TridigionalEquation<float> e2(
//            commandQueue,
//            make_terms(inputTopDiag2, inputMiddleDiag2, inputDownDiag2, inputFreeMembers2, 3).get()
//            , 3);
//    e2.solve();
//
//    float inputTopDiag3[] = { 2,5,3,3,2};
//    float inputMiddleDiag3[] = { 1,4,2,5,2,4};
//    float inputDownDiag3[] = { 3,1,4,1,2};
//    float inputFreeMembers3[] = { 2, 14, 14, 35, 21, 28};
//
//    TridigionalEquation<float> e3(
//            commandQueue,
//            make_terms(inputTopDiag3, inputMiddleDiag3, inputDownDiag3, inputFreeMembers3, 6).get()
//            , 6);
//    e3.solve();
//
//    float inputTopDiag4[] = { 2,5,3,3};
//    float inputMiddleDiag4[] = { 1,4,2,5,2};
//    float inputDownDiag4[] = { 3,1,4,1};
//    float inputFreeMembers4[] = { 2, 14, 14, 35, 11};
//
//    TridigionalEquation<float> e4(
//            commandQueue,
//            make_terms(inputTopDiag4, inputMiddleDiag4, inputDownDiag4, inputFreeMembers4, 5).get()
//            , 5);
//    e4.solve();
}