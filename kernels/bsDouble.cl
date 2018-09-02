
#define A(i) terms[(i)*4 + 2]
#define B(i) terms[(i)*4]
#define C(i) terms[(i)*4 + 3]
#define D(i) terms[(i)*4 + 1]

#pragma OPENCL EXTENSION cl_khr_fp64: enable
__kernel void bs( __global double* terms, __global double* unknows,
                  unsigned numberOfReduction, unsigned isEvenEqs) {

    unsigned  offset = numberOfReduction-1;
    unsigned  eq;

    eq = ((get_global_id(0) + 1) << numberOfReduction) - (1<<offset) - 1;

    if((get_global_id(0) == get_global_size(0)-1) && isEvenEqs){
        /*Last eq, odd number of eqs*/
        unknows[eq] = (D(eq) - A(eq-1)*unknows[eq - (1<<offset)])/B(eq);
    }else{
        if (get_global_id(0) > 0) {
            /*Common case */
            unknows[eq] = (D(eq) - A(eq - 1) * unknows[eq - (1 << offset)]
                           - C(eq) * unknows[eq + (1 << offset)]) / B(eq);
        }else {
            /*First eq*/
            unknows[eq] = (D(eq) - C(eq) * unknows[eq + (1 << offset)]) / B(eq);
        }
    }
}
