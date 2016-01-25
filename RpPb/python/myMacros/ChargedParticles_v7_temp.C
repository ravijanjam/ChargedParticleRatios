// C++ Headers
#include <iostream>

// ROOT Headers
#include <TCanvas.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <TGraph.h>
#include <TLatex.h>

using namespace std;

void ChargedParticles_v7_temp(){

	TCanvas *c1 = new TCanvas ("c1", "pPb Yield", 1000, 700);
	c1->Divide(3, 2);

	TCanvas *c2 = new TCanvas ("c2", "pp Yield", 1000, 700);
	c2->Divide(3, 2);

	TCanvas *c3 = new TCanvas ("c3", "The ratios of #p_T from pPb and pp", 1000, 700);

	double pT, dpT, dN, invariantYield;


	/* ===================================================================*/
	// pT Spectra
	string fileName1 = "pPbAnalyzer2016-01-25-100241.root" ,
	       fileName2 = "ppRefOutput2016-01-25-110144.root"; 

	TFile *inputFile1 = new TFile("pPbAnalyzer2016-01-25-100241.root", "r");
	TFile *inputFile2 = new TFile("ppRefOutput2016-01-25-110144.root", "r");

	cout << "================================================" << endl;
	cout << "Are the files open ? " << "\n"
	     << fileName1 << "\t" << inputFile1->IsOpen() << "\n"
	     << fileName2 << "\t" << inputFile2->IsOpen() << "\n";
	cout << "================================================" << endl;

	// Pt Spectra path in the root file
	TString sPosPt = "/demo/htrackPosPt";
	TString sNegPt = "/demo/htrackNegPt";

	// Eta Spectra path in the root file
	TString sPosEta = "/demo/htrackPosEta";
	TString sNegEta = "/demo/htrackNegEta";

	TH1D *hPosPt[3], *hNegPt[3], *RpPbByppRef, *hTest;

	// pp
	hPosPt[1] = (TH1D*)inputFile1->Get(sPosPt);
	hNegPt[1] = (TH1D*)inputFile1->Get(sNegPt);

	// pPb
	hPosPt[2] = (TH1D*)inputFile2->Get(sPosPt);
	hNegPt[2] = (TH1D*)inputFile2->Get(sNegPt);

	hTest  = (TH1D*)inputFile2->Get(sPosPt);

	int nPosBins = hPosPt[1]->GetSize();
	int nNegBins = hNegPt[1]->GetSize();

	cout << "nPosbins: " << nPosBins << "\t"
	     << "nNegbins: " << nNegBins << "\t"
	     << endl;
	
	hTest->Draw();

	inputFile1->Close();
	inputFile2->Close();
	
}


