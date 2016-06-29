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


#define A(i) terms[(i)*4 + 2]
#define B(i) terms[(i)*4]
#define C(i) terms[(i)*4 + 3]
#define D(i) terms[(i)*4 + 1]

__kernel void fr( __global double* terms, unsigned numberOfReduction, unsigned isEven) {

    unsigned  offset = numberOfReduction-1; //1
    unsigned  eq = ((get_global_id(0) + 1) << numberOfReduction) - 1;//3

    double ab = A(eq-1) / B(eq - (1<<offset));

    if(get_global_id(0) > 0) {
        if(get_global_id(0) < get_global_size(0)-1){
            /*Common case */
            double cb = C(eq) / B(eq + (1<<offset));


            A(eq-1) = -A(eq-(1<<offset)-1) * ab;
            B(eq) = B(eq) - C(eq-(1<<offset)) * ab - A(eq-1+(1<<offset)) * cb;
            C(eq) = -C(eq+(1<<offset)) * cb;
            D(eq) = D(eq) - D(eq-(1<<offset)) * ab -
                    D(eq+(1<<offset)) * cb;
        }else{
            if(!(isEven)){
                /*Last eq, even number of eqs*/

                A(eq-1) = -A(eq-(1<<offset)-1) * ab;
                B(eq) = B(eq) - C(eq-(1<<offset)) * ab;
                D(eq) = D(eq) - D(eq-(1<<offset)) * ab;
            }else{
                /*Last eq, odd number of eqs*/
                double cb = C(eq) / B(eq + (1<<offset));

                A(eq-1) = -A(eq-(1<<offset)-1) * ab;
                B(eq) = B(eq) - C(eq-(1<<offset)) * ab
                        - A(eq-1+(1<<offset)) * cb;
                D(eq) = D(eq) - D(eq-(1<<offset)) * ab -
                        D(eq+(1<<offset)) * cb;
            }
        }
    }else{
        /*First eq*/
        double cb = C(eq) / B(eq + (1<<offset)); //-3/1.33333

        B(eq) = B(eq) - C(eq-(1<<offset)) * ab
                - A(eq-1 + (1<<offset)) * cb;//-2.5-7.5/2.25 -
        C(eq) = -C(eq+(1<<offset)) * cb;
        D(eq) = D(eq) - D(eq-(1<<offset)) * ab -
                D(eq+(1<<offset)) * cb;
    }
}