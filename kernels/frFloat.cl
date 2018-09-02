//#define A(i) terms[(i)*4 + 2]
//#define B(i) terms[(i)*4]
//#define C(i) terms[(i)*4 + 3]
//#define D(i) terms[(i)*4 + 1]

#pragma OPENCL EXTENSION cl_khr_fp64: enable
__kernel void fr( __global float* terms, unsigned numberOfReduction, unsigned isEven) {
    unsigned offset = numberOfReduction-1;
    unsigned eq = ((get_global_id(0) + 1) << numberOfReduction) - 1;

    float ab = terms[(eq-1)*4 + 2] / terms[(eq - (1<<offset))*4];

    if(get_global_id(0) > 0) {
        if(get_global_id(0) < get_global_size(0)-1){

            float cb = terms[(eq)*4 + 3] / terms[(eq + (1<<offset))*4];


            terms[(eq-1)*4 + 2] = -terms[(eq-(1<<offset)-1)*4 + 2] * ab;
            terms[(eq)*4] = terms[(eq)*4] - terms[(eq-(1<<offset))*4 + 3] * ab - terms[(eq-1+(1<<offset))*4 + 2] * cb;
            terms[(eq)*4 + 3] = -terms[(eq+(1<<offset))*4 + 3] * cb;
            terms[(eq)*4 + 1] = terms[(eq)*4 + 1] - terms[(eq-(1<<offset))*4 + 1] * ab -
                                terms[(eq+(1<<offset))*4 + 1] * cb;
        }else{
            if(!(isEven)){


                terms[(eq-1)*4 + 2] = -terms[(eq-(1<<offset)-1)*4 + 2] * ab;
                terms[(eq)*4] = terms[(eq)*4] - terms[(eq-(1<<offset))*4 + 3] * ab;
                terms[(eq)*4 + 1] = terms[(eq)*4 + 1] - terms[(eq-(1<<offset))*4 + 1] * ab;
            }else{

                float cb = terms[(eq)*4 + 3] / terms[(eq + (1<<offset))*4];

                terms[(eq-1)*4 + 2] = -terms[(eq-(1<<offset)-1)*4 + 2] * ab;
                terms[(eq)*4] = terms[(eq)*4] - terms[(eq-(1<<offset))*4 + 3] * ab
                                - terms[(eq-1+(1<<offset))*4 + 2] * cb;
                terms[(eq)*4 + 1] = terms[(eq)*4 + 1] - terms[(eq-(1<<offset))*4 + 1] * ab -
                                    terms[(eq+(1<<offset))*4 + 1] * cb;
            }
        }
    }else{

        float cb = terms[(eq)*4 + 3] / terms[(eq + (1<<offset))*4];

        terms[(eq)*4] = terms[(eq)*4] - terms[(eq-(1<<offset))*4 + 3] * ab
                        - terms[(eq-1 + (1<<offset))*4 + 2] * cb;
        terms[(eq)*4 + 3] = -terms[(eq+(1<<offset))*4 + 3] * cb;
        terms[(eq)*4 + 1] = terms[(eq)*4 + 1] - terms[(eq-(1<<offset))*4 + 1] * ab -
                            terms[(eq+(1<<offset))*4 + 1] * cb;
    }
}
