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


void InvYield_smallerpTRange(char *inFile, string collisionSystem){

	// Data for event normalization from DAS
	double nEvents_pPb = 36872579, nEvents_pp=127220628;

	cout << "====================================================" << endl;
	cout << "Collision System chosen: " << collisionSystem << endl;
	cout << "====================================================" << endl;

	double nEvents; 
	double pTMax, pTMin;
	double gYLow, gYHigh,
	       gXLow, gXHigh,
	       hXLow, hXHigh;

	char *outputFileName;

	// Pass variables to the program based on 
	// the collision system
	
	gXLow=1; gXHigh=10;
	hXLow=1; hXHigh=100;
	pTMin=0; pTMax=10; 

	if (collisionSystem == "pp"){
	 nEvents = nEvents_pPb; 
	 gYLow=1e-7; gYHigh=1; 
	 outputFileName = "output_ppInvariantYieldForSelectedpTRange.root";

	}

	else if (collisionSystem == "pPb"){
	 nEvents = nEvents_pp;
	 gYLow=1e-3; gYHigh=1e+3; 
	 outputFileName = "output_ppInvariantYieldForSelectedpTRange.root";
	}

	else {cout << "Invalid Option, please choose either pp or pPb" << endl; exit(1);}
	

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
	TH1D *hPosPt, *hNegPt;

	// Inv Yield Histograms
	TH1D *hPosInvYield, *hNegInvYield, *hNegByPosInvYield;

	// New Histograms for selected pT Range
	TH1D *hnewPosInvYield, *hnewNegInvYield,
	     *hnewPosPt, *hnewNegPt;

	// Path to the histograms in the file
	char *sPosPt, *sNegPt;
 	sPosPt = "/userAnalyzer/htrackNegPt";
        sNegPt = "/userAnalyzer/htrackPosPt";

	// Set gPad options globally
	padOptions(c, 3);
	padOptions(cInvYield, 3);

	hPosPt = (TH1D*)inputFile->Get(sPosPt);
	hNegPt = (TH1D*)inputFile->Get(sNegPt);


	// Number of Bins in histograms for
	// Positive and Negative from Data
	double maxNPosBins = hPosPt->GetSize();
	double maxNNegBins = hNegPt->GetSize();

	// Check if the num of bins for pos and neg are same
	if ( maxNPosBins == maxNNegBins ) cout << "The number of bins in Positive and Negative distributions are identical" << endl;

	double pT, dpT, dN;
	double gPosInvYield[1000], gNegInvYield[1000], gNegByPosInvYield[1000], nPosBins[1000], nNegBins[1000];


	// Loop over the histogram data for 
	// positive and negative charged particles
	for (int tBin=0; tBin < 1000; tBin++)
	{
		// For Positively Charged Particles
		pT = hPosPt->GetBinCenter(tBin);	
		dpT = hPosPt->GetBinWidth(tBin);	
		dN = hPosPt->GetBinContent(tBin);

		gPosInvYield[tBin]=0; nPosBins[tBin] = 0;
	
		nPosBins[tBin] = pT;
		if (pT > pTMin && pT < pTMax) gPosInvYield[tBin] = dN /(dpT*pT*nEvents);
		else gPosInvYield[tBin] = 0;

		// For Negatively Charged Particles
		pT = hNegPt->GetBinCenter(tBin);	
		dpT = hNegPt->GetBinWidth(tBin);	
		dN = hNegPt->GetBinContent(tBin);

		gNegInvYield[tBin]=0; nNegBins[tBin]=0;

		nNegBins[tBin] = pT;
		if (pT > pTMin && pT < pTMax) gNegInvYield[tBin] = dN /(dpT*pT*nEvents);
		else gNegInvYield[tBin] = 0;

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

	double gTitleOffset = 1.4;
	char *gYAxisTitle;
	gYAxisTitle = "#frac{1}{N_{evt}}#frac{d^{2}N}{p_{t}dp_{T}}";
	// Drawing on to the gPads
	cInvYield->cd(1);
	graphPosInvYield->GetYaxis()->SetRangeUser(gYLow, gYHigh);
	graphPosInvYield->GetXaxis()->SetRangeUser(gXLow, gXHigh);
	graphPosInvYield->GetYaxis()->SetTitle(gYAxisTitle);
	graphPosInvYield->GetYaxis()->SetTitleOffset(gTitleOffset);
	graphPosInvYield->GetYaxis()->CenterTitle(true);
	graphPosInvYield->GetXaxis()->SetTitle("p_{T}(GeV/c)");
	graphPosInvYield->Draw("ACP");

	cInvYield->cd(2);
	graphNegInvYield->GetYaxis()->SetRangeUser(gYLow, gYHigh);
	graphNegInvYield->GetXaxis()->SetRangeUser(gXLow, gXHigh);
	graphNegInvYield->GetYaxis()->SetTitleOffset(gTitleOffset);
	graphNegInvYield->GetYaxis()->CenterTitle(true);
	graphNegInvYield->GetYaxis()->SetTitle(gYAxisTitle);
	graphNegInvYield->GetXaxis()->SetTitle("p_{T}(GeV/c)");
	graphNegInvYield->Draw("ACP");

	cInvYield->cd(3);
	gPad->SetLogy(0);
	gPad->SetLogx(0);
	graphNegByPosInvYield->GetYaxis()->SetRangeUser(0, 2);
	graphNegByPosInvYield->GetXaxis()->SetRangeUser(gXLow, gXHigh);
//	graphNegByPosInvYield->GetXaxis()->SetRangeUser(0, 100);
	graphNegByPosInvYield->GetYaxis()->SetTitle("Inv Yield Neg By Pos");
	graphNegByPosInvYield->GetYaxis()->CenterTitle(true);
	graphNegByPosInvYield->GetYaxis()->SetTitleOffset(gTitleOffset);
	graphNegByPosInvYield->GetXaxis()->SetTitle("p_{T}(GeV/c)");
	graphNegByPosInvYield->Draw("ACP");


	// Ratio plots for histograms
	c->cd(1);
	hPosPt->SetTitle("Positively Charged Particles Normalized Hist pT Distribution");
	hPosPt->GetYaxis()->SetTitle("Counts");
	hPosPt->GetXaxis()->SetTitle("p_{T} (GeV/c)");
	hPosPt->SetAxisRange(hXLow, hXHigh, "X");
	hPosPt->Scale(1/nEvents);
	hPosPt->Draw();

	c->cd(2);
	hNegPt->SetTitle("Negatively Charged Particles Normalized Hist pT Distribution");
	hNegPt->GetYaxis()->SetTitle("Counts");
	hNegPt->GetXaxis()->SetTitle("p_{T} (GeV/c)");
	hNegPt->SetAxisRange(hXLow, hXHigh, "X");
	hNegPt->Scale(1/nEvents);
	hNegPt->Draw();

	TH1D *hNegPtClone;
	hNegPtClone = (TH1D*)hNegPt->Clone();

	c->cd(3);
	hNegPtClone->Divide(hPosPt);
	hNegPtClone->SetTitle("Neg By Pos Normalized Hist Ratio");
	hNegPtClone->GetYaxis()->SetTitle("#frac{Pos}{Neg} (GeV/c)");
	hNegPtClone->GetXaxis()->SetTitle("p_{T} (GeV/c)");
	hNegPtClone->Draw();
	gPad->SetLogy(0);


	// Writing to an output file
	TFile *outputFile = new TFile(outputFileName, "recreate");

	TObjArray hList(0);

	// Adding hist and graphs to the 
	// output file
	hList.Add(hPosPt);
	hList.Add(hNegPt);
	hList.Add(hNegPtClone);
	hList.Add(graphPosInvYield);
	hList.Add(graphNegInvYield);
	hList.Add(graphNegByPosInvYield);

	hList.Write();

	outputFile->Close();

}
