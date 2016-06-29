/*
	 * The view
	 * b0 c0  0 0  0 ...  0    d0
	 * a0 b1 c1 0  0 ...  0    d1
	 * 0  a1 b2 c2 0 ...  0    d2
	 * .. .. .. .. .. ..  0    ..
	 * 0  0  0  0  0 an-2 bn-1 dn-1
	 *
	 * terms is b0 d0 a0 c0 b1 d1 a1 c1 ... bn-2 dn-2 an-2 cn-2 bn-1 dn-1
	 * For example: b0 d0 a0 c0 b1 d1 a1 c1 b2 d2 a2 c2 b3 d3
	 * size = 2 + 4*(n-1)
	 *
	 * b(i) = terms(i*4)
	 * c(i) = terms(i*4+3)
	 * a(i) = terms(i*4+2)
	 * d(i) = terms(i*4+1)
	 *
	 * Client must release memory of terms!
	 */

//
//#define A(i) terms[(i)*4 + 2]
//#define B(i) terms[(i)*4]
//#define C(i) terms[(i)*4 + 3]
//#define D(i) terms[(i)*4 + 1]

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