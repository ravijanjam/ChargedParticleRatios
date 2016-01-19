
	/* ===================================================================*/
	// pT Spectra
	TFile *inputFile2 = new TFile(inputRootFile2, "r");
	TString sPosPt2 = "/demo/htrackPosPt";
	TString sNegPt2 = "/demo/htrackNegPt";

	// Eta Spectra
	TString sPosEta2 = "/demo/htrackPosEta";
	TString sNegEta2 = "/demo/htrackNegEta";

	TH1D *hPosPt[2], *hNegPt[2];
	hPosPt[2] = (TH1D*)inputFile2->Get(sPosPt);
	hNegPt[2] = (TH1D*)inputFile2->Get(sNegPt);

	nPosBins = hPosPt[2]->GetSize();
	nNegBins = hNegPt[2]->GetSize();

	cout << "nPosbins" << nPosBins << "t"
	     << "nNegbins" << nNegBins << "t"
	     << endl;


	//InvYieldPos[nPosBins], pTPos[nPosBins];
	for (int i=0; i < nPosBins; i++ ){

		pT = hPosPt[2]->GetBinCenter(i);
		dpT = hPosPt[2]->GetBinWidth(i);
		dN = hPosPt[2]->GetBinContent(i);

		//invariantYield = dN/(pT*dpT);
		invariantYield = dN/(dpT);
		InvYieldPos[i] = invariantYield;
		pTPos[i] = pT;

		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;

	}

	//double InvYieldNeg[nNegBins], pTNeg[nNegBins];
	for (int i=0; i < nNegBins; i++ ){

		pT = hNegPt[2]->GetBinCenter(i);
		dpT = hNegPt[2]->GetBinWidth(i);
		dN = hNegPt[2]->GetBinContent(i);

	//	invariantYield = dN/(pT*dpT);
		invariantYield = dN/(dpT);
		InvYieldNeg[i] = invariantYield;
		pTNeg[i] = pT;

		cout << pT << "\t"
		     <<	dpT << "\t"
		     <<	dN << endl;

	}

	c->cd(1);
	gPad->SetLogy();
	gPad->SetGrid();
	hPosPt[2]->GetXaxis()->SetLimits(-5, 200);
	hPosPt[2]->Draw();

	c->cd(2);	
	gPad->SetLogy();
	gPad->SetGrid();
	hNegPt[2]->GetXaxis()->SetLimits(-5, 200);
	hNegPt[2]->Draw();

	c->cd(3);
	/*  This step needs to be done, if divide is used directly, 
	 *  the source histogram is overwritten */
	TH1D *hNegPtClone2 = (TH1D*)hNegPt[1]->Clone("hNegPtClone");
	gPad->SetLogy();
	gPad->SetGrid();
	hNegPtClone2->Divide(hPosPt[1]);
	hNegPtClone2->GetXaxis()->SetLimits(-5, 200);
	hNegPtClone2->Draw();


	// Cosmetics for histograms
	
	// TGraph plot for Invariant Yield
	// Get the number of pt bins and pass them into an arry
	// Get the invariant yield into an array
	// TGraph *gr = new TGraph(n,x,y);
	// where x, y are arrays
	
	TGraph *gPos2 = new TGraph(nPosBins, pTPos, InvYieldPos);
	TGraph *gNeg2 = new TGraph(nNegBins, pTNeg, InvYieldNeg);

	// Calculate the ratios of invariant yields
	for (int tBin = 0; tBin < nPosBins; tBin++) {
	
		if ( InvYieldPos[tBin] != 0 ) ratioNegByPosInvYield[tBin] = InvYieldNeg[tBin]/InvYieldPos[tBin];
		else ratioNegByPosInvYield[tBin]=0;

		cout << "Invariant Yield - pos, neg, ratio(neg/pos) :  " 
		     << InvYieldPos[tBin] << "\t" << InvYieldNeg[tBin] << "\t" <<  ratioNegByPosInvYield[tBin] << endl;
	}
	TGraph *gRatioNegByPosInvYield2 = new TGraph(nNegBins, pTNeg, ratioNegByPosInvYield);

	c->cd(4);
	gPad->SetLogy();
	gPad->SetGrid();
	//gPos->GetXaxis()->SetLimits(-5, 200);
	gPos2->Draw();

	c->cd(5);
	gPad->SetGrid();
	gPad->SetLogy();
	//gNeg->GetXaxis()->SetLimits(-5, 200);
	gNeg2->Draw();

	c->cd(6);
	gPad->SetGrid();
	//gRatioNegByPosInvYield->GetXaxis()->SetLimits(-5, 200);
	gRatioNegByPosInvYield2->Draw();
	/* ===================================================================*/
