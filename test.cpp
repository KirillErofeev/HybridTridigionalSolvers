#include "TridigionalEquation.hpp"
#include "test.hpp"

void baseTestCpp(cl::CommandQueue commandQueue){
    float inputTopDiag[] = { 2,5,3,3,2,1,5 };
    float inputMiddleDiag[] = { 1,4,2,5,2,4,3,3 };
    float inputDownDiag[] = { 3,1,4,1,2,2,1 };
    float inputFreeMembers[] = { 2, 14, 14, 35, 21, 34, 63, 27 };
//
//
        auto terms = make_terms(inputTopDiag, inputMiddleDiag, inputDownDiag,inputFreeMembers, 8);
        TridigionalEquation<float> e(
                commandQueue,
        terms.get(), 8);
        std::chrono::duration<double> t;
        e.tSolve(t);
        std::cout << t.count() << std::endl;
        e.outMatrix(std::cout);
//        printArray(std::cout, e.getUnknows().get(), 8);

//    float inputTopDiag1[] = { 2,5,3,3,2,1 };
//    float inputMiddleDiag1[] = { 1,4,2,5,2,4,3};
//    float inputDownDiag1[] = { 3,1,4,1,2,2 };
//    float inputFreeMembers1[] = { 2, 14, 14, 35, 21, 34, 28};
//
//    auto terms1 = make_terms(inputTopDiag1, inputMiddleDiag1, inputDownDiag1, inputFreeMembers1, 7);
//    TridigionalEquation<float> e1(
//            commandQueue,
//            terms1.get()
//            , 7);
//    e1.solve();

//    float inputTopDiag2[] = { 2,5};
//    float inputMiddleDiag2[] = { 1,4,2};
//    float inputDownDiag2[] = { 3,1 };
//    float inputFreeMembers2[] = { 2, 14, 5};
//
//    auto terms2 = make_terms(inputTopDiag2, inputMiddleDiag2, inputDownDiag2, inputFreeMembers2, 3);
//    TridigionalEquation<float> e2(
//            commandQueue,
//            terms2.get(), 3);
//    e2.outMatrix(std::cout);
//    e2.solve();

//    float inputTopDiag3[] = { 2,5,3,3,2};
//    float inputMiddleDiag3[] = { 1,4,2,5,2,4};
//    float inputDownDiag3[] = { 3,1,4,1,2};
//    float inputFreeMembers3[] = { 2, 14, 14, 35, 21, 28};
//
//    auto terms3 = make_terms(inputTopDiag3, inputMiddleDiag3, inputDownDiag3, inputFreeMembers3, 6);
//    TridigionalEquation<float> e3(
//            commandQueue,
//            terms3.get()
//            , 6);
//    e3.solve();
//
//    float inputTopDiag4[] = { 2,5,3,3};
//    float inputMiddleDiag4[] = { 1,4,2,5,2};
//    float inputDownDiag4[] = { 3,1,4,1};
//    float inputFreeMembers4[] = { 2, 14, 14, 35, 11};
//
//    auto terms4 = make_terms(inputTopDiag4, inputMiddleDiag4, inputDownDiag4, inputFreeMembers4, 5);
//
//    TridigionalEquation<float> e4(
//            commandQueue,
//            terms4.get()
//            , 5);
//    e4.solve();

    double inputTopDiag5[] = {  962.6, -11.9,261.2,373.9};
    double inputMiddleDiag5[] = { -518.5, 700.4, -541.0, -545.3, -511.3};
    double inputDownDiag5[] = { 269.9, -443.7, 743.3, -570.7};
    double inputFreeMembers5[] = { 85285.1, 27911.3, 48869.7, 296110, 253421};


    auto terms5 = make_terms(inputTopDiag5, inputMiddleDiag5, inputDownDiag5,inputFreeMembers5, 5);
    TridigionalEquation<double> e5(
            commandQueue,
            terms5.get(), 5);
    e5.solve();
    e5.outMatrix(std::cout);
    printArray(std::cout, e5.getUnknows().get(), 5);
}
