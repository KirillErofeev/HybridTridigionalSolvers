#ifndef TRIDIGIONAL_EQUATION_HPP
#define TRIDIGIONAL_EQUATION_HPP

#include <cstddef>
#include <iostream>
struct TridigionalEquation {

	TridigionalEquation(
		float* topDiag, float* midDiag, float* downDiag, float* freeMembers, size_t size);

	int inverse(float* returns);
	friend std::ostream& operator <<(std::ostream& os, TridigionalEquation& eq);

private:
	float* topDiag;
	float* midDiag;
	float* downDiag;
	float* freeMembers;
	size_t size;

};

#endif