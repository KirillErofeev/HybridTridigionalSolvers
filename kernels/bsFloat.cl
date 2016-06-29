
#define A(i) terms[(i)*4 + 2]
#define B(i) terms[(i)*4]
#define C(i) terms[(i)*4 + 3]
#define D(i) terms[(i)*4 + 1]

__kernel void bs( __global float* terms, __global float* unknowns,
                  unsigned numberOfReduction, unsigned isEvenEqs) {

    unsigned  offset = numberOfReduction-1;
    unsigned  eq;

    eq = ((get_global_id(0) + 1) << numberOfReduction) - (1<<offset) - 1;

    if((get_global_id(0) == get_global_size(0)-1) && isEvenEqs){
        /*Last eq, odd number of eqs*/
        unknowns[eq] = (D(eq) - A(eq-1)*unknowns[eq - (1<<offset)])/B(eq);
    }else{
        if (get_global_id(0) > 0) {
            /*Common case */
            unknowns[eq] = (D(eq) - A(eq - 1) * unknowns[eq - (1 << offset)]
                           - C(eq) * unknowns[eq + (1 << offset)]) / B(eq);
        }else {
            /*First eq*/
            unknowns[eq] =
                    (D(eq) - C(eq) * unknowns[eq + (1 << offset)]) / B(eq);
        }
    }
}