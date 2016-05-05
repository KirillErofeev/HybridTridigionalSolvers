#ifndef TRIDIGIONAL_EQUATION_HPP
#define TRIDIGIONAL_EQUATION_HPP

#include <cstddef>
#include <iostream>
struct TridigionalEquation {

	TridigionalEquation(
		float* topDiag, float* midDiag, float* downDiag, float* freeMembers, size_t size);
    TridigionalEquation(TridigionalEquation& eq) = delete;
    TridigionalEquation& operator=(TridigionalEquation& eq) = delete;
	int inverse();
	friend std::ostream& operator <<(std::ostream& os, TridigionalEquation& eq);

    ~TridigionalEquation();

private:
	float* topDiag;
	float* midDiag;
	float* downDiag;
	float* freeMembers;
	size_t size;

    float* unknows;
    bool isSolve;

};

#endif