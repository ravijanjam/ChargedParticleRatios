// C++ Headers
#include <iostream>


// ROOT Headers
#include <TGraphAsymmErrors.h>
#include <TRandom3.h>

using namespace std;

void egTGraphAsymmErrors(){

	// Generate Data
	TRandom r;

	double gsX[100], gsY[100], gseX1[100], gseY1[100], gseX2[100], gseY2[100];
	double nSamples = 100;
	for (int i=0; i < nSamples; i++){

		// Populate x, xerrors
		gsX[i] = i;
		gseX1[i] = (r.Gaus(0, 1))/10;
		gseX2[i] = (r.Gaus(0, 1))/10;
		
		// Populate y, yerrors
		gsY[i] = r.Gaus(0, 1);
		gseY1[i] = r.Gaus(0, 1)/10;
		gseY2[i] = r.Gaus(0, 1)/10;
	}

 	TGraphAsymmErrors *gplot = new TGraphAsymmErrors(nSamples, gsX, gsY, gseX1, gseY1, gseX2, gseY2);
	gplot->SetName("TGraphAsymmErrors example");
	gplot->SetMarkerStyle(21);
	gplot->SetMarkerColor(4);
	gplot->Draw("ALP");
}
