#include <iostream>
#include <TFile.h>

using namespace std;

void test(char *f1, char *f2){

	char *p;
	p = "pPbAnalyzer2016-01-25-100241.root";
	
	TFile *input1 = new TFile(f1);
	TFile *input2 = new TFile(f2);

	input1->IsOpen();
	input2->IsOpen();
	

}
