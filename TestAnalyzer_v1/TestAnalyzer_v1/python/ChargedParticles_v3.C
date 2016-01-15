// C++ Headers
#include <iostream>

// ROOT Headers
#include <TCanvas.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <TGraph.h>

using namespace std;

void ChargedParticles_v3( char *inputRootFile ){

	TCanvas *c = new TCanvas ("c", "Charged Particle Ratios Invariant Yield", 1000, 700);
	c->Divide(1, 2);
	
	double pT, dpT, dN, invariantYield;

	TFile *inputFile = new TFile(inputRootFile, "r");

	TString sPosPt = "/demo/trackPosPt";
	TString sNegPt = "/demo/trackNegPt";

	TString sPosEta = "/demo/trackPosEta";
	TString sNegEta = "/demo/trackNegEta";

	TH1D *hPosPt = (TH1D*)inputFile->Get(sPosPt);
	TH1D *hNegPt = (TH1D*)inputFile->Get(sNegPt);

	int nPosBins = hPosPt->GetSize();
	int nNegBins = hNegPt->GetSize();

	cout << "nPosbins" << nPosBins << "t"
	     << "nNegbins" << nNegBins << "t"
	     << endl;

	
	double tempInvYield[nPosBins], temppT[nPosBins];
	for (int i=0; i < nPosBins; i++ ){

		pT = hPosPt->GetBinCenter(i);
		dpT = hPosPt->GetBinWidth(i);
		dN = hPosPt->GetBinContent(i);

		invariantYield = dN/(pT*dpT);
		tempInvYield[i] = invariantYield;
		temppT[i] = pT;

		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;

	}

	for (int i=0; i < nNegBins; i++ ){

		pT = hNegPt->GetBinCenter(i);
		dpT = hNegPt->GetBinWidth(i);
		dN = hNegPt->GetBinContent(i);

		invariantYield = dN/(pT*dpT);
		tempInvYield[i] = invariantYield;
		temppT[i] = pT;

		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;

	}

	c->cd(1);
	hPosPt->Draw();

	c->cd(2);	
	hNegPt->Draw();


	cout << "nPosbins" << nPosBins << "t"
	     << "nNegbins" << nNegBins << "t"
	     << endl;

	//hNegPt->Draw();

	// Cosmetics for histograms
	
	// TGraph plot for Invariant Yield
	// Get the number of pt bins and pass them into an arry
	// Get the invariant yield into an array
	// TGraph *gr = new TGraph(n,x,y);
	// where x, y are arrays
	
	TGraph *g = new TGraph(nPosBins, temppT, tempInvYield);



}
