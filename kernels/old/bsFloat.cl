__kernel void bs(
        __global float* topDiag, __global float* midDiag,
        __global float* downDiag, __global float* freeMembers,
        __global float* unknows, unsigned numberOfReduction, unsigned isEvenEqs) {

    unsigned  offset = numberOfReduction-1;//1
    unsigned  eq;//3

    eq = ((get_global_id(0) + 1) << numberOfReduction) - (1<<offset) - 1;

    if((get_global_id(0) == get_global_size(0)-1) && isEvenEqs){
        /*Last eq, odd number of eqs*/
        unknows[eq] = (freeMembers[eq] - downDiag[eq-1]*unknows[eq - (1<<offset)])/midDiag[eq];
    }else{
        if (get_global_id(0) > 0) {
                /*Common case */
                unknows[eq] = (freeMembers[eq] - downDiag[eq - 1] * unknows[eq - (1 << offset)]
                               - topDiag[eq] * unknows[eq + (1 << offset)]) / midDiag[eq];
        }else {
          /*First eq*/
            unknows[eq] =
                    (freeMembers[eq] - topDiag[eq] * unknows[eq + (1 << offset)]) / midDiag[eq];
        }
    }
}