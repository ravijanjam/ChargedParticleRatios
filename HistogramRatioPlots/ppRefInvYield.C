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
#include <TObjArray.h>

using namespace std;


/* Global pad options for the canvas */
TCanvas* padOptions(TCanvas *c, signed int numPads){

	// Loop over all Pads
	for (int i=1; i <= numPads; i++){
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


void ppRefInvYield(char *inFile){

	// Data for event normalization from DAS
	double nEventsppRef = 127220628;
	

	TCanvas *c = new TCanvas ("c", "", 900, 500);
	c->Divide(3, 1);

	TCanvas *cInvYield = new TCanvas("cInvYield", "", 900, 500);
	cInvYield->Divide(3, 1);

	// Reading from the input file
	TFile *inputFile = new TFile(inFile);


	cout << "=============================================" << endl;
	cout << "Are the files open ? " << "\n"
	     << inFile << "\t" << inputFile->IsOpen() << "\n";
	cout << "=============================================" << endl;

	// Initiate histograms
	TH1D *hPosPt[2], *hNegPt[2];

	// Inv Yield Histograms
	TH1D *hPosInvYield, *hNegInvYield, *hNegByPosInvYield;

	char *sPosPt, *sNegPt;
 	sPosPt = "/userAnalyzer/htrackNegPt";
        sNegPt = "/userAnalyzer/htrackPosPt";

	// Set gPad options globally
	padOptions(c, 3);
	padOptions(cInvYield, 3);

	// ppRef
	hPosPt[1] = (TH1D*)inputFile->Get(sPosPt);
	hNegPt[1] = (TH1D*)inputFile->Get(sNegPt);


	double maxNPosBins = hPosPt[1]->GetSize();
	double maxNNegBins = hNegPt[1]->GetSize();

	double pT, dpT, dN;
	double gPosInvYield[1000], gNegInvYield[1000], gNegByPosInvYield[1000], nPosBins[1000], nNegBins[1000];
	for (int tBin=0; tBin < 1000; tBin++)
	{
		// For Positively Charged Particles
		pT = hPosPt[1]->GetBinCenter(tBin);	
		dpT = hPosPt[1]->GetBinWidth(tBin);	
		dN = hPosPt[1]->GetBinContent(tBin);

		gPosInvYield[tBin]=0; nPosBins[tBin] = 0;
	
		nPosBins[tBin] = pT;
		if (pT > 0) gPosInvYield[tBin] = dN /(dpT*pT*nEventsppRef);

		// For Negatively Charged Particles
		pT = hNegPt[1]->GetBinCenter(tBin);	
		dpT = hNegPt[1]->GetBinWidth(tBin);	
		dN = hNegPt[1]->GetBinContent(tBin);

		gNegInvYield[tBin]=0; nNegBins[tBin]=0;

		nNegBins[tBin] = pT;
		if (pT > 0) gNegInvYield[tBin] = dN /(dpT*pT*nEventsppRef);

		// Neg by Pos Inv Yield
		if (gPosInvYield[tBin] > 0) gNegByPosInvYield[tBin] = gNegInvYield[tBin] / gPosInvYield[tBin];
		
	}


	TGraph *graphPosInvYield = new TGraph( maxNPosBins, nPosBins, gPosInvYield );
	TGraph *graphNegInvYield = new TGraph( maxNNegBins, nNegBins, gNegInvYield );
	TGraph *graphNegByPosInvYield = new TGraph( maxNNegBins, nNegBins, gNegByPosInvYield );

	// Set Titles for TGraphs
	graphPosInvYield->SetTitle("Invariant Yield of Positively Charged Particles");
	graphNegInvYield->SetTitle("Invariant Yield of Negatively Charged Particles");
	graphNegByPosInvYield->SetTitle("Inv Yield of Neg / Inv Yield of Pos");
	
	// Cosmetics to TGraph
	graphCosmetics(graphPosInvYield, 20, kOrange+10, 0.7);
	graphCosmetics(graphNegInvYield, 28, kBlue+4, 0.7);
	graphCosmetics(graphNegByPosInvYield, 28, kYellow+4, 0.7);

	cInvYield->cd(1);
	graphPosInvYield->GetYaxis()->SetRangeUser(1e-10, 1);
	graphPosInvYield->Draw();

	cInvYield->cd(2);
	graphNegInvYield->GetYaxis()->SetRangeUser(1e-10, 1);
	graphNegInvYield->Draw();

	cInvYield->cd(3);
	gPad->SetLogy(0);
	graphNegByPosInvYield->GetYaxis()->SetRangeUser(-1, 3);
	graphNegByPosInvYield->Draw();


	// Ratio plots for histograms
	c->cd(1);
	hPosPt[1]->SetTitle("Positively Charged Particles Normalized Hist pT Distribution - ppRef");
	hPosPt[1]->GetYaxis()->SetTitle("Counts");
	hPosPt[1]->GetXaxis()->SetTitle("#p_{T} (GeV/c)");
	hPosPt[1]->Scale(1/nEventsppRef);
	hPosPt[1]->Draw();

	c->cd(2);
	hNegPt[1]->SetTitle("Negatively Charged Particles Normalized Hist pT Distribution - ppRef");
	hNegPt[1]->GetYaxis()->SetTitle("Counts");
	hNegPt[1]->GetXaxis()->SetTitle("#p_{T} (GeV/c)");
	hNegPt[1]->Scale(1/nEventsppRef);
	hNegPt[1]->Draw();

	TH1D *hNegPtClone;
	hNegPtClone = (TH1D*)hNegPt[1]->Clone();

	c->cd(3);
	hNegPtClone->Divide(hPosPt[1]);
	hNegPtClone->SetTitle("Neg By Pos Normalized Hist Ratio-ppRef");
	hNegPtClone->GetYaxis()->SetTitle("#frac{Pos}{Neg} (GeV/c)");
	hNegPtClone->GetXaxis()->SetTitle("#p_{T} (GeV/c)");
	hNegPtClone->Draw();
	gPad->SetLogy(0);


	// Writing to an output file
	TFile *outputFile = new TFile("outputppRefInvariantYield.root", "recreate");

	TObjArray hList(0);
	hList.Add(hPosPt[1]);
	hList.Add(hNegPt[1]);
	hList.Add(hNegPtClone);
	hList.Add(graphPosInvYield);
	hList.Add(graphNegInvYield);
	hList.Add(graphNegByPosInvYield);

	hList.Write();

	outputFile->Close();

}
