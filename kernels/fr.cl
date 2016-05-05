__kernel void fr(
        __global float* topDiag, __global float* midDiag,
        __global float* downDiag, __global float* freeMembers, unsigned numberOfReduction) {

    unsigned  offset = numberOfReduction-1; //1
    unsigned  eq = ((get_global_id(0) + 1) << numberOfReduction) - 1;//3

    float ab = downDiag[eq-1] / midDiag[eq - (1<<offset)];// -2/-4.5

    if(get_global_id(0) > 0) {
        if(get_global_id(0) < get_global_size(0)-1){
            /*Common case */
            float cb = topDiag[eq] / midDiag[eq + (1<<offset)];

            downDiag[eq-1] = -downDiag[eq-(1<<offset)-1] * ab;
            midDiag[eq] = midDiag[eq] - topDiag[eq-(1<<offset)] * ab - downDiag[eq-1+(1<<offset)] * cb;
            topDiag[eq] = -topDiag[eq+(1<<offset)] * cb;
            freeMembers[eq] = freeMembers[eq] - freeMembers[eq-(1<<offset)] * ab -
                    freeMembers[eq+(1<<offset)] * cb;
        }else{
            if(!(get_global_size(0) % 2)){
                /*Last eq, even number of eqs*/

                downDiag[eq-1] = -downDiag[eq-(1<<offset)-1] * ab;
                midDiag[eq] = midDiag[eq] - topDiag[eq-(1<<offset)] * ab;
                freeMembers[eq] = freeMembers[eq] - freeMembers[eq-(1<<offset)] * ab;
            }else{
                /*Last eq, odd number of eqs*/
                float cb = topDiag[eq] / midDiag[eq + (1<<offset)];

                downDiag[eq-1] = -downDiag[eq-(1<<offset)-1] * ab;
                midDiag[eq] = midDiag[eq] - topDiag[eq-(1<<offset)] * ab
                              - downDiag[eq-1+(1<<offset)] * cb;
                freeMembers[eq] = freeMembers[eq] - freeMembers[eq-(1<<offset)] * ab -
                                     freeMembers[eq+(1<<offset)] * cb;
            }
        }
    }else{
        /*First eq*/
        float cb = topDiag[eq] / midDiag[eq + (1<<offset)]; //-3/1.33333

        midDiag[eq] = midDiag[eq] - topDiag[eq-(1<<offset)] * ab
                      - downDiag[eq-1 + (1<<offset)] * cb;//-2.5-7.5/2.25 -
        topDiag[eq] = -topDiag[eq+(1<<offset)] * cb;
        freeMembers[eq] = freeMembers[eq] - freeMembers[eq-(1<<offset)] * ab -
                             freeMembers[eq+(1<<offset)] * cb;
    }
}