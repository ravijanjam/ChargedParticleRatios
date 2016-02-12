#include <TH1D.h>

void variableBinning(){

	float bins[] = {-29, -20, -13, -10, -8, -3, 0, 3, 8, 10, 13, 20, 29};
	int nBins = sizeof(bins)/sizeof(int)-1;
	TH1D *h = new TH1D("n", "title", nBins, bins);
	TH1D *h2 = new TH1D("n2", "title", 100, -5, 5);
	h->FillRandom("gaus", 10000);
	h2->FillRandom("gaus", 10000);
	h->Draw();
	h2->Draw("same");
}
