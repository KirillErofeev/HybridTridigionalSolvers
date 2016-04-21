struct TridigionalEquation {

	TridigionalEquation(float* topDiag, float* midDiag, float* downDiag, size_t size);

	int inverse(float* returns);

private:
	float* topDiag;
	float* midDiag;
	float* downDiag;
	size_t size;

};