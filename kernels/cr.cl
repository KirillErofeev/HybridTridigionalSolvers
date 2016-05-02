__kernel void cr(
        __global float* topDiag, __global float* midDiag,
        __global float* downDiag, __global float* freeMembers,
        __global float* outTopDiag, __global float* outMidDiag,
        __global float* outDownDiag, __global float* outFreeMembers) {

    unsigned  i = get_global_id(0);
    unsigned  eq = (i<<1) + 1;

    if(i>0) {
        if(i < get_global_size(0)-1){
            /*Common case */
            float ab = downDiag[eq-1] / midDiag[eq - 1];
            float cb = topDiag[eq] / midDiag[eq + 1];

            outDownDiag[i-1] = -downDiag[eq-2] * ab;
            outMidDiag[i] = midDiag[eq] - topDiag[eq-1] * ab - downDiag[eq] * cb;
            outTopDiag[i] = -topDiag[eq+1] * cb;
            outFreeMembers[i] = freeMembers[eq] - freeMembers[eq-1] * ab - freeMembers[eq+1] * cb;
        }else{
            if(!(get_global_size(0) % 2)){

                /*Last eq, even number of eqs*/
                float ab = downDiag[eq-1] / midDiag[eq - 1];

                outDownDiag[i-1] = -downDiag[eq-2] * ab;
                outMidDiag[i] = midDiag[eq] - topDiag[eq-1] * ab;
                outFreeMembers[i] = freeMembers[eq] - freeMembers[eq-1] * ab;
            }else{
                /*Last eq, odd number of eqs*/
                float ab = downDiag[eq-1] / midDiag[eq-1];
                float cb = topDiag[eq] / midDiag[eq + 1];

                outDownDiag[i-1] = -downDiag[eq-2] * ab;
                outMidDiag[i] = midDiag[eq] - topDiag[eq-1] * ab - downDiag[eq] * cb;
                outFreeMembers[i] = freeMembers[eq] - freeMembers[eq-1] * ab - freeMembers[eq+1] * cb;
            }
        }
    }else{
        /*First eq*/
        float ab = downDiag[eq-1] / midDiag[eq - 1];
        float cb = topDiag[eq] / midDiag[eq + 1]; //2.5

        outMidDiag[i] = midDiag[eq] - topDiag[eq-1] * ab - downDiag[eq] * cb; //4-6 - 1,25
        outTopDiag[i] = -topDiag[eq+1] * cb;
        outFreeMembers[i] = freeMembers[eq] - freeMembers[eq-1] * ab - freeMembers[eq+1] * cb;
    }
}