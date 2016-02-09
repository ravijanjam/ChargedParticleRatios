// C++ Headers
#include <iostream>
#include <string>
#include <array>

// ROOT Headers
#include <TCanvas.h>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>
#include <TGraph.h>
#include <TLatex.h>

using namespace std;


/* Global pad options for the canvas */
TCanvas* padOptions(TCanvas *c, signed int numPads){

	// Loop over all Pads
	for (int i=0; i < numPads; i++){
		c->cd(i);
		gPad->SetGrid();
		gPad->SetLogy();
		gPad->SetLogx();
	}	

	return c;
	
}//close-padOptions

/* Cosmetics for TGraph */
TGraph* graphCosmetics(TGraph *g, int mstyle, int mcolor, double msize){

	g->SetMarkerStyle(mstyle);
	g->SetMarkerColor(mcolor);
	g->SetMarkerSize(msize);
	return g;
} // close-graphCosmetics



void ChargedParticles_v12(char *inFile1, char *inFile2){

	TCanvas *c1 = new TCanvas ("c1", "pPb Yield", 1000, 700);
	c1->Divide(3, 2);

	// Initialize histograms
	TH1D *hPosPt[3], *hNegPt[3], *RpPbByppRef;

	/* ===================================================================*/
	// pT Spectra

	TFile *inputFile1 = new TFile(inFile1);
	TFile *inputFile2 = new TFile(inFile2);
	
	// Time stamped output file
//	TFile *fWrite = new TFile("ChargedParticleRatio_)

	cout << "================================================" << endl;
	cout << "Are the files open ? " << "\n"
	     << inFile1 << "\t" << inputFile1->IsOpen() << "\n"
	     << inFile2 << "\t" << inputFile2->IsOpen() << "\n";
	cout << "================================================" << endl;

	// Pt Spectra path in the root file
	// Same for pPb and ppRef
	char *sPosPt, *sNegPt;
 	sPosPt = "/userAnalyzer/htrackNegPt";
        sNegPt = "/userAnalyzer/htrackPosPt";

	// Eta Spectra path in the root file
	// Same for pPb and ppRef
	string sPosEta = "/userAnalyzer/htrackPosEta";
	string sNegEta = "/userAnalyzer/htrackNegEta";

	// ppRef
	hPosPt[1] = (TH1D*)inputFile1->Get(sPosPt);
	hNegPt[1] = (TH1D*)inputFile1->Get(sNegPt);

	// pPb
	hPosPt[2] = (TH1D*)inputFile2->Get(sPosPt);
	hNegPt[2] = (TH1D*)inputFile2->Get(sNegPt);

	cout << "Num bins in: " << inFile2 << "\t"
		<< hPosPt[2]->GetSize() 
	     << "\t Num bins in: " << inFile1 << "\t"
		<< hPosPt[1]->GetSize() 
		<< endl;

	cout << "Num bins in: " << inFile2 << "\t"
		<< hNegPt[2]->GetSize() 
	     << "\t Num bins in: " << inFile1 << "\t"
		<< hNegPt[1]->GetSize() 
		<< endl;

	c1->cd(1);
	hPosPt[1]->SetLineColor(kRed);
	hPosPt[1]->Draw();
	hPosPt[2]->SetLineColor(kRed);
	hPosPt[2]->Draw("same");

	c1->cd(2);
	hNegPt[1]->SetLineColor(kGreen);
	hNegPt[1]->Draw();
	hNegPt[2]->SetLineColor(kGreen);
	hNegPt[2]->Draw("same");

	/* Invariant Yield Calculations */
	double nPosBins = hPosPt[1]->GetSize() ;
	double nEventspPb = 36872579;
	double nEventsppRef = 127220628;

	double pT, dpT, dN, invariantYield;
	double gInvYieldNeg[1200], gInvYieldPos[1200], 
	       gInvYieldNegByPos[1200], gInvYieldNegByPos1[1200],
	       gNPosBins[1200], gNNegBins[1200];


	double gInvYieldNegpPb[1200], gInvYieldPospPb[1200];

	// Initializing the arrays to zero
	std::fill(begin(gInvYieldNeg), end(gInvYieldNeg), 0);
	std::fill(begin(gInvYieldNegpPb), end(gInvYieldNegpPb), 0);
	std::fill(begin(gInvYieldPospPb), end(gInvYieldPospPb), 0);
	std::fill(begin(gInvYieldNegByPos1), end(gInvYieldNegByPos1), 0);

	// Initialize the arrays to 0
	// to make sure garbage is not in the arrays
	for ( int i=0; i<1200; i++ ) { 
		gInvYieldNeg[i] = 0; gInvYieldPos[i] = 0; gNPosBins[i] = 0; gNNegBins[i] = 0; 
		gInvYieldNegByPos[i] = 0;
		gInvYieldNegByPos1[i] = 0;
	}

	// Calculate invariant yield
	for ( int tBin = 0; tBin < nPosBins; tBin++ ){

		// Positively charged particles-ppRef
		dN = hPosPt[1]->GetBinContent(tBin);
		pT = hPosPt[1]->GetBinCenter(tBin);
		dpT = hPosPt[1]->GetBinWidth(tBin);
		gNPosBins[tBin] = pT;

		if ( pT >= 0) invariantYield = dN / (pT*dpT);
		gInvYieldPos[tBin] = invariantYield/nEventsppRef;


		// Positively charged particles-pPb
		dN = hPosPt[2]->GetBinContent(tBin);
		pT = hPosPt[2]->GetBinCenter(tBin);
		dpT = hPosPt[2]->GetBinWidth(tBin);
		gNPosBins[tBin] = pT;

		if ( pT >= 0) invariantYield = dN / (pT*dpT);
		gInvYieldPospPb[tBin] = invariantYield/nEventspPb;


		// Negatively charged particles-ppRef
		dN = hNegPt[1]->GetBinContent(tBin);
		pT = hNegPt[1]->GetBinCenter(tBin);
		dpT = hNegPt[1]->GetBinWidth(tBin);
		gNNegBins[tBin] = pT;

		if ( pT >= 0) invariantYield = dN / (pT*dpT);
		gInvYieldNeg[tBin] = invariantYield/nEventsppRef;

		// Negatively charged particles-pPb
		dN = hNegPt[2]->GetBinContent(tBin);
		pT = hNegPt[2]->GetBinCenter(tBin);
		dpT = hNegPt[2]->GetBinWidth(tBin);
		gNNegBins[tBin] = pT;

		if ( pT >= 0) invariantYield = dN / (pT*dpT);
		gInvYieldNegpPb[tBin] = invariantYield/nEventspPb;

		
		// Calculate the ratio
		if (gInvYieldPos[tBin] > 0 ) gInvYieldNegByPos[tBin] = gInvYieldPospPb[tBin]/gInvYieldPos[tBin];
		if (gInvYieldNeg[tBin] > 0 ) gInvYieldNegByPos1[tBin] = gInvYieldNegpPb[tBin]/gInvYieldNeg[tBin];

	} // close 

	// Initialize TGraph
	TGraph *gPos = new TGraph(nPosBins, gNPosBins, gInvYieldPos);
	TGraph *gPospPb = new TGraph(nPosBins, gNPosBins, gInvYieldPospPb);
	TGraph *gNeg = new TGraph(nPosBins, gNNegBins, gInvYieldNeg);
	TGraph *gNegByPos = new TGraph(nPosBins, gNNegBins, gInvYieldNegByPos);
//	TGraph *gNegByPos1 = new TGraph(nPosBins, gNNegBins, gInvYieldNegByPos1);

	// TGraph Cosmetics
	graphCosmetics(gPos, 20, 4, 1);
	graphCosmetics(gPospPb, 20, 4, 1);
	graphCosmetics(gNeg, 21, 2, 1);
	graphCosmetics(gNegByPos, 23, 46, 1);

	gPos->SetTitle("Spectra of positively charged particles");
	gNeg->SetTitle("Spectra of negatively charged particles");
	gNegByPos->SetTitle("Spectra of ratio of negative by positively charged particles");

	// Set Titles - X Axis
	gPos->GetXaxis()->SetTitle("p_{T} (GeV/c)");
	gNeg->GetXaxis()->SetTitle("p_{T} (GeV/c)");
	gNegByPos->GetXaxis()->SetTitle("p_{T} (GeV/c)");

	// Set Titles - Y Axis
	gPos->GetYaxis()->SetTitle("#frac{d^2N}{p_{T}dp_{T}} (GeV/c)");
	gNeg->GetYaxis()->SetTitle("#frac{d^2N}{p_{T}dp_{T}} (GeV/c)");
	gNegByPos->GetYaxis()->SetTitle("Neg by Pos Inv Yield");

	/* Set Common options for all the Pads on the Canvas */
	signed int numPads = 6;
	padOptions(c1, numPads);

	c1->cd(4);
	gPos->Draw();

	c1->cd(5);
	gNeg->Draw();

	c1->cd(6);
	gNegByPos->Draw();

}//close-main

/*
 * Normalize the inv yield with the total number of events
 *
 *
 */
