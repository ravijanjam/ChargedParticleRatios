
// C++ Headers
#include <iostream>

// ROOT Headers
#include <TH1D.h>
#include <TFile.h>
#include <TCanvas.h>

using namespace std;

void variableBinning_v1(char *inFile){

	TCanvas *cOrig = new TCanvas("cOrig", "", 700, 400);
	TCanvas *cRebin = new TCanvas("cRebin", "", 700, 400);

	TFile *f = new TFile(inFile);
	
	// Initiate histograms
	TH1D *hPosPt, *hNegPt;
	
	char *sPosPt, *sNegPt;
 	sPosPt = "/userAnalyzer/htrackNegPt";
        sNegPt = "/userAnalyzer/htrackPosPt";

	hPosPt = (TH1D*)f->Get(sPosPt);
	hNegPt = (TH1D*)f->Get(sNegPt);

	double binArr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 20, 50, 90, 130, 180};
	int nBins = sizeof(binArr)/sizeof(double)-1;

	TH1D *hPosVarBin;
	hPosVarBin = (TH1D*)hPosPt->Clone();
	hPosVarBin = (TH1D*)hPosVarBin->Rebin(nBins, "hPosVarBin", binArr);	


	cout << "=======================================" << endl;
	cout << "Number of bins in original histogram: " 
	     << hPosPt->GetNbinsX()
	     << endl;

	cout << "Number of bins in rebinned histogram: " 
	     << hPosVarBin->GetNbinsX()
	     << endl;
	cout << "=======================================" << endl;
	

	cOrig->cd();
	hPosPt->Draw();
	
	cRebin->cd();
	hPosVarBin->Draw();
}
