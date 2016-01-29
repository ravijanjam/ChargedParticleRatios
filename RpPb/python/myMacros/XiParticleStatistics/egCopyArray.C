// C++ Headers
#include <iostream>
#include <cmath>

using namespace std;

void egCopyArray(){

	int maxRandomValue = 10;
	int nSamples = 10;

	int arrOrig[10], arrCopy[10];

	for (int i=0; i < nSamples; i++) arrOrig[i] = rand()%maxRandomValue; 
	copy(begin(arrOrig), end(arrOrig), begin(arrCopy));

	// Copied array
	for (int i=0; i < nSamples; i++) cout << arrCopy[i] << "\t";
}

