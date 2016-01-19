// C++ Headers
#include <iostream>

// ROOT Headers
#include <TCanvas.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <TGraph.h>

using namespace std;

void ChargedParticles_v4( char *inputRootFile ){

	TCanvas *c = new TCanvas ("c", "Charged Particle Ratios Invariant Yield", 1000, 700);
	c->Divide(3, 2);
	
	double pT, dpT, dN, invariantYield;

	TFile *inputFile = new TFile(inputRootFile, "r");

	// pT Spectra
	TString sPosPt = "/demo/trackPosPt";
	TString sNegPt = "/demo/trackNegPt";

	// Eta Spectra
	TString sPosEta = "/demo/trackPosEta";
	TString sNegEta = "/demo/trackNegEta";

	TH1D *hPosPt = (TH1D*)inputFile->Get(sPosPt);
	TH1D *hNegPt = (TH1D*)inputFile->Get(sNegPt);

	int nPosBins = hPosPt->GetSize();
	int nNegBins = hNegPt->GetSize();

	cout << "nPosbins" << nPosBins << "t"
	     << "nNegbins" << nNegBins << "t"
	     << endl;


	double InvYieldPos[nPosBins], pTPos[nPosBins];
	for (int i=0; i < nPosBins; i++ ){

		pT = hPosPt->GetBinCenter(i);
		dpT = hPosPt->GetBinWidth(i);
		dN = hPosPt->GetBinContent(i);

		invariantYield = dN/(pT*dpT);
		InvYieldPos[i] = invariantYield;
		pTPos[i] = pT;

		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;

	}

	double InvYieldNeg[nNegBins], pTNeg[nNegBins];
	for (int i=0; i < nNegBins; i++ ){

		pT = hNegPt->GetBinCenter(i);
		dpT = hNegPt->GetBinWidth(i);
		dN = hNegPt->GetBinContent(i);

		invariantYield = dN/(pT*dpT);
		InvYieldNeg[i] = invariantYield;
		pTNeg[i] = pT;

		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;

	}

	c->cd(1);
	hPosPt->Draw();

	c->cd(2);	
	hNegPt->Draw();

	c->cd(3);
	/*  This step needs to be done, if divide is used directly, 
	 *  the source histogram is overwritten */
	TH1D *hNegPtClone = (TH1D*)hNegPt->Clone("hNegPtClone");
	hNegPtClone->Divide(hPosPt);
	hNegPtClone->Draw();

	/*  
	cout << "nPosbins" << nPosBins << "t"
	     << "nNegbins" << nNegBins << "t"
	     << endl;
	*/

	// Cosmetics for histograms
	
	// TGraph plot for Invariant Yield
	// Get the number of pt bins and pass them into an arry
	// Get the invariant yield into an array
	// TGraph *gr = new TGraph(n,x,y);
	// where x, y are arrays
	
	TGraph *gPos = new TGraph(nPosBins, pTPos, InvYieldPos);
	TGraph *gNeg = new TGraph(nNegBins, pTNeg, InvYieldNeg);

	// Calculate the ratios of invariant yields
	double ratioNegByPosInvYield[nPosBins];
	for (int tBin = 0; tBin < nPosBins; tBin++) {
	
		if ( InvYieldPos[tBin] != 0 ) ratioNegByPosInvYield[tBin] = InvYieldNeg[tBin]/InvYieldPos[tBin];
		else ratioNegByPosInvYield[tBin]=0;

		cout << "Invariant Yield - pos, neg, ratio(neg/pos) :  " 
		     << InvYieldPos[tBin] << "\t" << InvYieldNeg[tBin] << "\t" <<  ratioNegByPosInvYield[tBin] << endl;
	}
	TGraph *gRatioNegByPosInvYield = new TGraph(nNegBins, pTNeg, ratioNegByPosInvYield);

	c->cd(4);
	gPad->SetLogy();
	gPos->Draw();

	c->cd(5);
	gPad->SetLogy();
	gNeg->Draw();

	c->cd(6);
	gRatioNegByPosInvYield->Draw();

}


/*  Log  */
/*  
 * v4_01 : Apply the check that number of bins for pT and Eta must be the same for the invariant yield and assign only one variable
 *
 *  */
