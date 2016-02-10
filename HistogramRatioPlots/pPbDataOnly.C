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

void pPbDataOnly(char *inFile){

	// Data for event normalization from DAS
	double nEventspPb = 36872579;
	

	TCanvas *c = new TCanvas ("c", "", 900, 500);
	c->Divide(3, 1);

	// Reading from the input file
	TFile *inputFile = new TFile(inFile);

	cout << "Are the files open ? " << "\n"
	     << inFile << "\t" << inputFile->IsOpen() << "\n";

	// Initiate histograms
	TH1D *hPosPt[2], *hNegPt[2];

	char *sPosPt, *sNegPt;
 	sPosPt = "/userAnalyzer/htrackNegPt";
        sNegPt = "/userAnalyzer/htrackPosPt";

	padOptions(c, 3);

	// ppRef
	hPosPt[1] = (TH1D*)inputFile->Get(sPosPt);
	hNegPt[1] = (TH1D*)inputFile->Get(sNegPt);

	c->cd(1);
	hPosPt[1]->SetTitle("Positively Charged Particles Normalized Hist pT Distribution - pPb");
	hPosPt[1]->GetYaxis()->SetTitle("Counts");
	hPosPt[1]->GetXaxis()->SetTitle("#p_{T} (GeV/c)");
	hPosPt[1]->Scale(1/nEventspPb);
	hPosPt[1]->Draw();

	c->cd(2);
	hNegPt[1]->SetTitle("Negatively Charged Particles Normalized Hist pT Distribution - pPb");
	hNegPt[1]->GetYaxis()->SetTitle("Counts");
	hNegPt[1]->GetXaxis()->SetTitle("#p_{T} (GeV/c)");
	hNegPt[1]->Scale(1/nEventspPb);
	hNegPt[1]->Draw();

	TH1D *hNegPtClone;
	hNegPtClone = (TH1D*)hNegPt[1]->Clone();

	c->cd(3);
	hNegPtClone->Divide(hPosPt[1]);
	hNegPtClone->SetTitle("Neg By Pos Normalized Hist Ratio-pPb");
	hNegPtClone->GetYaxis()->SetTitle("#frac{Pos}{Neg} (GeV/c)");
	hNegPtClone->GetXaxis()->SetTitle("#p_{T} (GeV/c)");
	gPad->SetLogy(0);
	hNegPtClone->Draw();

	// Writing to an output file
	TFile *outputFile = new TFile("outputpPbDataOnly.root", "recreate");

	TObjArray hList(0);

	hList.Add(hPosPt[1]);
	hList.Add(hNegPt[1]);
	hList.Add(hNegPtClone);

	hList.Write();


	outputFile->Close();
}
