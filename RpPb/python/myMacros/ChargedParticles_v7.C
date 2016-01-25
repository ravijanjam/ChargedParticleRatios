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

void ChargedParticles_v7(){

	TCanvas *c1 = new TCanvas ("c1", "pPb Yield", 1000, 700);
	c1->Divide(3, 2);

	TCanvas *c2 = new TCanvas ("c2", "pp Yield", 1000, 700);
	c2->Divide(3, 2);

	TCanvas *c3 = new TCanvas ("c3", "The ratios of #p_T from pPb and pp", 1000, 700);

	double pT, dpT, dN, invariantYield;

	TH1D *hPosPt[3], *hNegPt[3], *RpPbByppRef;

	/* ===================================================================*/
	// pT Spectra
	char *fileName1, *fileName2; 
	fileName1 = "pPbAnalyzer2016-01-25-100241.root";
	fileName2 = "ppRefOutput2016-01-25-110144.root";

	TFile *inputFile1 = new TFile(fileName1);
	TFile *inputFile2 = new TFile(fileName2);

	cout << "================================================" << endl;
	cout << "Are the files open ? " << "\n"
	     << fileName1 << "\t" << inputFile1->IsOpen() << "\n"
	     << fileName2 << "\t" << inputFile1->IsOpen() << "\n";
	cout << "================================================" << endl;

	// Pt Spectra path in the root file
	TString sPosPt = "/demo/htrackPosPt";
	TString sNegPt = "/demo/htrackNegPt";

	// Eta Spectra path in the root file
	TString sPosEta = "/demo/htrackPosEta";
	TString sNegEta = "/demo/htrackNegEta";

	hPosPt[1] = (TH1D*)inputFile1->Get(sPosPt);
	hNegPt[1] = (TH1D*)inputFile1->Get(sNegPt);

	hPosPt[2] = (TH1D*)inputFile2->Get(sPosPt);
	hNegPt[2] = (TH1D*)inputFile2->Get(sNegPt);

	int nPosBins = hPosPt[1]->GetSize();
	int nNegBins = hNegPt[1]->GetSize();

	cout << "nPosbins" << nPosBins << "\t"
	     << "nNegbins" << nNegBins << "\t"
	     << endl;


	//double InvYieldPos[nPosBins], pTPos[nPosBins];
	double InvYieldPos[1002], pTPos[1002];
	for (int i=0; i < nPosBins; i++ ){

	
	// For pPb
		pT = hPosPt[1]->GetBinCenter(i);
		dpT = hPosPt[1]->GetBinWidth(i);
		dN = hPosPt[1]->GetBinContent(i);

	//	invariantYield = dN/(pT*dpT);
		invariantYield = dN/(dpT);
		InvYieldPos[i] = invariantYield;
		pTPos[i] = pT;

	// For ppRef
		pT = hPosPt[2]->GetBinCenter(i);
		dpT = hPosPt[2]->GetBinWidth(i);
		dN = hPosPt[2]->GetBinContent(i);

	//	invariantYield = dN/(pT*dpT);
		invariantYield = dN/(dpT);
		InvYieldPos[i] = invariantYield;
		pTPos[i] = pT;

		/*  
		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;
		*/

	}

	double InvYieldNeg[nNegBins], pTNeg[nNegBins];
	for (int i=0; i < nNegBins; i++ ){

	// For pPb
		pT = hNegPt[1]->GetBinCenter(i);
		dpT = hNegPt[1]->GetBinWidth(i);
		dN = hNegPt[1]->GetBinContent(i);

	//	invariantYield = dN/(pT*dpT);
		invariantYield = dN/(dpT);
		InvYieldNeg[i] = invariantYield;
		pTNeg[i] = pT;

	// For ppRef
		pT = hNegPt[2]->GetBinCenter(i);
		dpT = hNegPt[2]->GetBinWidth(i);
		dN = hNegPt[2]->GetBinContent(i);

	//	invariantYield = dN/(pT*dpT);
		invariantYield = dN/(dpT);
		InvYieldNeg[i] = invariantYield;
		pTNeg[i] = pT;
	
		/*  
		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;
		*/

	}

	c1->cd(1);
	gPad->SetLogy();
	gPad->SetGrid();
//	hPosPt[1]->GetXaxis()->SetLimits(-5, 200);
	hPosPt[1]->Draw();

	c1->cd(2);	
	gPad->SetLogy();
	gPad->SetGrid();
//	hNegPt[1]->GetXaxis()->SetLimits(-5, 200);
	hNegPt[1]->Draw();

	c1->cd(3);
	/*  This step needs to be done, if divide is used directly, 
	 *  the source histogram is overwritten */
	TH1D *hNegPtClone = (TH1D*)hNegPt[1]->Clone("hNegPtClone");
	gPad->SetLogy();
	gPad->SetGrid();
	hNegPtClone->Divide(hPosPt[1]);
//	hNegPtClone->GetXaxis()->SetLimits(-5, 200);
	hNegPtClone->Draw();


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

	c1->cd(4);
	gPad->SetLogy();
	gPad->SetGrid();
	//gPos->GetXaxis()->SetLimits(-5, 200);
	gPos->Draw();

	c1->cd(5);
	gPad->SetGrid();
	gPad->SetLogy();
	//gNeg->GetXaxis()->SetLimits(-5, 200);
	gNeg->Draw();

	c1->cd(6);
	gPad->SetGrid();
	//gRatioNegByPosInvYield->GetXaxis()->SetLimits(-5, 200);
	gRatioNegByPosInvYield->Draw();
	/* ===================================================================*/

	// Clone the histograms
	// Take the ratio of pPb by pp
		
	TH1D *hhPosPtClone[3], *hhNegPtClone[3];
	hhNegPtClone[1] = (TH1D*)hNegPt[1]->Clone("a1");
	hhPosPtClone[1] = (TH1D*)hPosPt[1]->Clone("a2");

	hhNegPtClone[2] = (TH1D*)hNegPt[2]->Clone("b1");
	hhPosPtClone[2] = (TH1D*)hPosPt[2]->Clone("b2");

	hhNegPtClone[1]->Divide(hhNegPtClone[2]);
	hhPosPtClone[1]->Divide(hhPosPtClone[2]);

	c2->cd(1);
	gPad->SetLogy();
	gPad->SetGrid();
//	hPosPt[1]->GetXaxis()->SetLimits(-5, 200);
	hPosPt[1]->Draw();

	c2->cd(2);
	gPad->SetLogy();
	gPad->SetGrid();
//	hPosPt[2]->GetXaxis()->SetLimits(-5, 200);
	hPosPt[2]->Draw();

	c2->cd(3);
	gPad->SetLogx();
	gPad->SetGrid();
	hhNegPtClone[1]->Divide(hNegPt[2]);
//	hhNegPtClone[1]->GetXaxis()->SetLimits(-5, 200);
	hhNegPtClone[1]->Draw();

	c2->cd(4);
	gPad->SetLogy();
	gPad->SetGrid();
//	hNegPt[1]->GetXaxis()->SetLimits(-5, 200);
	hNegPt[1]->Draw();

	c2->cd(5);
	gPad->SetLogy();
	gPad->SetGrid();
//	hNegPt[2]->GetXaxis()->SetLimits(-5, 200);
	hNegPt[2]->Draw();

	c2->cd(6);
	gPad->SetLogx();
	gPad->SetGrid();
	hhPosPtClone[1]->Divide(hPosPt[2]);
//	hhPosPtClone[1]->GetXaxis()->SetLimits(-5, 200);
	hhPosPtClone[1]->Draw();

	c3->cd();
	gPad->SetLogx();
	gPad->SetGrid();
	hhPosPtClone[1]->Divide(hPosPt[2]);
	hhNegPtClone[1]->Divide(hNegPt[2]);

	hhNegPtClone[1]->Scale(-1); 
//	hhPosPtClone[1]->GetXaxis()->SetLimits(-5, 200);
//	hhPosPtClone[1]->GetXaxis()->SetRangeUser(10, 200);

//	hhNegPtClone[1]->GetYaxis()->SetLimits(-5, 5);
//	hhNegPtClone[1]->GetYaxis()->SetRangeUser(-0.0001, 0.0001);
//	hhPosPtClone[1]->GetYaxis()->SetRangeUser(-0.0001, 0.0001);

	hhPosPtClone[1]->SetLineColor(kGreen);
	hhPosPtClone[1]->Draw();
	hhNegPtClone[1]->Draw("same");

	cout << hhPosPtClone[1]->GetName() << endl;
}


