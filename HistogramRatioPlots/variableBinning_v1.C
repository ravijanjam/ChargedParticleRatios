#include <TH1D.h>
#include <TFile.h>

void variableBinning(){

	TFile *f = new TFile(/*Name of the file*/);
	TH1D *hist;
	
	hist = (TH1D*)f->Get(/*Path*/);
	hist->Draw();
}
