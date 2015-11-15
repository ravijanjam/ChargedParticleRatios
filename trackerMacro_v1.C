// C++ Headers
#include <iostream>

// ROOT Headers
#include "TH1D.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TAttMarker.h"
#include "TString.h"
#include "TLatex.h"
#include "TLegend.h"

using namespace std; 

void trackerMacro_v1(char *rootFile, int s){


// Open a Canvas
TCanvas *c = new TCanvas("c", " Tracks ", 1000, 700);
//c->Divide(1, 3);

// Declare histograms
TH1D *h, *h1eta, *h1phi, *h1chi2, *h1pt, *h1vtxColl; 
// TFile *inputFile = new TFile("output.root", "r");
TFile *inputFile = new TFile(rootFile, "r");
cout << "Is the file open? 1 = yes, 0 = no    " << inputFile->IsOpen() << endl;

// Get the histograms from the root file produced by EDM Analyzer
h = (TH1D*)inputFile->Get("demo/tracks");
h1eta = (TH1D*)inputFile->Get("demo/h1negeta");
h1pt = (TH1D*)inputFile->Get("demo/h1poseta");
h1vtxColl = (TH1D*)inputFile->Get("demo/h1vtxColl");

if (s == 1) {
c->cd();
gPad->SetGrid();
h->SetMarkerStyle(20);
h->SetMarkerColor(kRed);
h->GetXaxis()->SetTitle("total number of tracks for all events");
h->Draw();
}
else if (s == 2) {
c->cd();
gPad->SetGrid();

h1eta->SetMarkerStyle(22);
h1eta->SetMarkerColor(kBlue);
h1eta->GetYaxis()->SetTitle("#eta distribution of negatively charged tracks for all events");

h1pt->SetMarkerStyle(30);
h1pt->SetMarkerColor(kGreen);
h1pt->GetXaxis()->SetTitle("#eta distribution of positively charged tracks for all events");


TLegend *leg = new TLegend(0.1, 0.7, 0.48, 0.9);
leg->AddEntry("h1pt", "blah blah", "F");

h1eta->Draw();
h1pt->Draw("same");
}

else if (s == 3){
c->cd();
gPad->SetGrid();
h1vtxColl->SetMarkerStyle(30);
h1vtxColl->SetMarkerColor(kBlue);
h1vtxColl->GetXaxis()->SetTitle("no of tracks from Vertex Collection for all events");
h1vtxColl->Draw();
}

}
