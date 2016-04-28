__kernel void cr(
	__global float* topDiag, __global float* midDiag,
	__global float* downDiag, __global float* freeMembers,
	__global float* outTopDiag, __global float* outMidDiag, 
	__global float* outDownDiag, __global float* outFreeMembers) {

    int eq = get_global_id(0) * 2 + 2;

	float ab = downDiag[eq-1]/midDiag[eq-1];
	float cb = topDiag[eq]/midDiag[eq+1];

	outDownDiag[eq-1] = -downDiag[eq-2] * ab;
	outMidDiag[eq] = midDiag[eq] - topDiag[eq-1] * ab - downDiag[eq]*cb;
	outTopDiag[eq] = -topDiag[eq+1] * cb;
	outFreeMembers[eq] = freeMembers[eq] - freeMembers[eq-1] * ab - freeMembers[eq+1] * cb;
}