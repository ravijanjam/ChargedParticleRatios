#include <iostream>

#include "TCanvas.h"

using namespace std;

void canvasDivide(){
	
	TCanvas *c = new TCanvas("c", "", 100, 200);
	c->Divide(1, 2);

}
