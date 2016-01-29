// C++ Headers
#include <iostream>
#include <string>

// ROOT Headers
#include <TCanvas.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <TGraph.h>
#include <TLatex.h>

using namespace std;

void ChargedParticles_v8(){

	TCanvas *c1 = new TCanvas ("c1", "pPb Yield", 1000, 700);

	double pT, dpT, dN, invariantYield;

	TH1D *hPosPt[3], *hNegPt[3], *RpPbByppRef;

	/* ===================================================================*/
	// pT Spectra
	char *fileName1 ; 
	fileName1 = "ppRefOutput2016-01-29-125836.root";


	TFile *inputFile1 = new TFile(fileName1);

	cout << "================================================" << endl;
	cout << "Are the files open ? " << "\n"
	     << fileName1 << "\t" << inputFile1->IsOpen() << "\n";
	cout << "================================================" << endl;

	// Pt Spectra path in the root file
	char *sPosPt, *sNegPt;
 	sPosPt = "/userAnalyzer/htrackNegPt";
        sNegPt = "/userAnalyzer/htrackPosPt";

	// Eta Spectra path in the root file
	string sPosEta = "/userAnalyzer/htrackPosEta";
	string sNegEta = "/userAnalyzer/htrackNegEta";

	hPosPt[0] = (TH1D*)inputFile1->Get(sPosPt);
	hNegPt[0] = (TH1D*)inputFile1->Get(sNegPt);

	hPosPt[0]->Draw();

}
