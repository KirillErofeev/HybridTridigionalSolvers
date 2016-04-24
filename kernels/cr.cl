__kernel void cr(
	__global float* topDiag, __global float* midDiag, __global float* downDiag,
	__global float* outTopDiag, __global float* outMidDiag, __global float* outDownDiag) {

    int num = get_global_id(0);
	/*int n = 1;
	for(int i=440; i > 1; i++){
		n = n * 53 % 43;
	}*/
	outTopDiag[num] = num;
	outMidDiag[num] = num*num;
	outDownDiag[num] = num;
}