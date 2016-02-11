
// C++ Headers
#include <memory>
#include <algorithm>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

// ROOT Headers
#include <TH1.h>
#include <TH2.h>
#include <TH3.h>
#include <TGraph.h>
#include <TF1.h>
#include <TH1D.h>

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <DataFormats/VertexReco/interface/Vertex.h>
#include <DataFormats/VertexReco/interface/VertexFwd.h> 
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/TrackReco/interface/TrackFwd.h>
#include <DataFormats/HepMCCandidate/interface/GenParticle.h>
#include <DataFormats/HepMCCandidate/interface/GenParticleFwd.h>
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "HepPDT/ParticleID.hh"
#include "RecoJets/JetAlgorithms/interface/JetAlgoHelper.h"
#include "HepMC/HeavyIon.h"

using namespace edm;
using namespace std;
using namespace reco;
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class ppRefAnalyzer_v5 : public edm::one::EDAnalyzer<edm::one::SharedResources>  
{
   public:
      explicit ppRefAnalyzer_v5(const edm::ParameterSet&);
      ~ppRefAnalyzer_v5();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );
	void initHistos(const edm::Service<TFileService> & fs);
        bool passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      std::map<std::string,TH1F*> evtPerf_;
	edm::Service<TFileService> fs;

	// Declaring variables for passing variables to constructor to the parameter set

	/* Variables from python config file */
	edm::InputTag trackSrc_;
	edm::InputTag vertexSrc_;
	double vertexZMax_;
	std::vector<double> ptBins_;
	double pTMin_, pTMax_;
	double etaMin_;
	double etaMax_;
	std::vector<double> etaBins_;
	bool applyCuts_;
	int TrackQualityNum_;
	reco::TrackBase::TrackQuality trackQuality_;
	
	TH1D *demoHisto; 
	TH1D *hvtxPerfX, *hvtxPerfY, *hvtxPerfZ, 
	     *htrackPosEta, *htrackPosPt, *htrackNegEta, *htrackNegPt, 
	     *htrackpT, *htrackEta, *htrackNetMom, *htrackpx, *htrackpy, *htrackpz,
		*htracksPerEvent, *hprimaryVerticesPerEvent;

	double nPosChargedParticles, nNegChargedParticles;
	TH2D *hvtxDistrXY;
	TH1D *hevtMult, *hvtxPerEvent;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
	double nTracksWithQualityCut,  nTracksWithoutQualityCut; 
	double measuredTrackEta, trackChi2;
	double trackCharge;


	bool iVertexZCut, iVertexValid, iVertexFake;
	int iVertexSize, iVertexTracks;
	bool iAllCuts, iTrackCuts, iVertexCuts;

	int trackQ;

};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
ppRefAnalyzer_v5::ppRefAnalyzer_v5(const edm::ParameterSet& iConfig)
:trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
pTMin_(iConfig.getParameter<double>("pTMin")),
pTMax_(iConfig.getParameter<double>("pTMax")),
etaMin_(iConfig.getParameter<double>("etaMin")),
etaMax_(iConfig.getParameter<double>("etaMax")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
applyCuts_(iConfig.getParameter<bool>("applyCuts")),
TrackQualityNum_(iConfig.getParameter<int>("TrackQualityNum"))
{
   //now do what ever initialization is needed
        usesResource("TFileService");
	initHistos(fs);
	trackQuality_ = TrackBase::qualityByName(iConfig.getParameter<std::string>("TrackQuality"));

}


ppRefAnalyzer_v5::~ppRefAnalyzer_v5()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
ppRefAnalyzer_v5::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

	iEvent.getRun();
/* Collections */


  // Vertex Collection
  Handle<std::vector<reco::Vertex> > vertex;
  iEvent.getByLabel(vertexSrc_, vertex);

  // Track Collection
   Handle<reco::TrackCollection> tcol;
   iEvent.getByLabel(trackSrc_, tcol);

   // Iterators for Track and Vertex
   reco::TrackCollection::const_iterator track;
   std::vector<reco::Vertex>::const_iterator vi; 

// Fill event multiplicity only when its greater than zero
   double multiplicityPerEvent = tcol->size();
   if (multiplicityPerEvent > 0 ) hevtMult->Fill(multiplicityPerEvent);

   cout << "multiplicity per event " << multiplicityPerEvent << endl;
   cout << "eta, pt, charge" << endl;

   // Apply Track Quality cuts 
   // Populate the histograms for pt, et based on charge
   // trackQuality_ = 2 for "highPurity"
   nTracksWithQualityCut = 0;  nTracksWithoutQualityCut = 0;

   // Loop over vertices
   std::vector<reco::Vertex> vtx = *vertex;
   for ( vi = vtx.begin(); vi != vtx.end(); ++vi ) {

   iVertexZCut = (vi->z() > -1*vertexZMax_) && (vi->z() < +1*vertexZMax_);
   iVertexSize = vertex->size();
   iVertexValid = vi->isValid();
   iVertexTracks = vi->nTracks();
   iVertexFake = vi->isFake();
 
   cout << "Information about vertices " << iEvent.id() << "\t"
	<< "number of vertices: " << iVertexSize << "\t"
	<< "fake: " << iVertexFake << "\t"
	<< "Validity?: " << iVertexValid << "\t"
	<< "Number of tracks per vertex: " << iVertexTracks << "\t"
	<< endl;
	
   iVertexCuts = (iVertexSize == 1) && ( iVertexValid ) && ( !iVertexFake );

   // 2 = highPurity, 6 = highuritySetWithPV
//   for(  track = tcol->begin(); track != tcol->end() && iVertexCuts ; ++track )
   for(  track = tcol->begin(); track != tcol->end() && iVertexCuts; ++track )
   { 
	cout << "Net Track Momentum " << track->p() << endl;
	cout << "Track Quality: " << track->quality(reco::TrackBase::qualityByName("tight")) << endl;

//	iTrackCuts = ( TrackQualityNum_ ==  track->quality(reco::TrackBase::qualityByName("highPurity")));
	iTrackCuts = ( 1  ==  track->quality(reco::TrackBase::qualityByName("tight")));

	if ( iTrackCuts ){
	htrackpx->Fill(track->px());
	htrackpy->Fill(track->py());
	htrackpz->Fill(track->pz());

	// Fill all the tracks for pT distribution
   	htrackpT->Fill(track->pt());
   	cout << "Track momentum: " << track->momentum() << "\t"
   	     << "Track ndof: " << track->ndof() << "\t" 
   	     << "Track chi2: " << track->chi2() << endl;


   /* Applying eta cut for the track */
   measuredTrackEta = track->eta(); 
 	trackCharge = track->charge();	

   bool trackEtaCut = ( measuredTrackEta < etaMax_ ) 
		     && ( measuredTrackEta < etaMin_ );

	if ( trackCharge == 1 && trackEtaCut ) {
		htrackPosPt->Fill(track->pt());
		htrackPosEta->Fill(track->eta());
	}else if ( trackCharge == -1 && trackEtaCut ){
		htrackNegPt->Fill(track->pt());
		htrackNegEta->Fill(track->eta());
	}
	nTracksWithQualityCut++;
   }//close if
   }//close for-track loop
	cout << endl;
	  
	  nTracksWithoutQualityCut = multiplicityPerEvent - nTracksWithQualityCut ; 
  }//close for-vertex loop

   cout << "================================================================" << endl;
   cout << "Multiplicity per Events: " << multiplicityPerEvent << endl;
   cout << "Tracks WITH Track Quality cut " << nTracksWithQualityCut << endl;
   cout << "Tracks WITHOUT CHOSEN Track Quality cut " << nTracksWithoutQualityCut << endl;
   cout << "================================================================" << endl;


  hvtxPerEvent->Fill(vertex->size());

   // Dummy filling of histograms
//   cout << "Populating the demo histogram" << endl;
   for (int i=0; i<10000; i++){
	demoHisto->Fill(i);
	}

/* Processing on collections */


#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
} //close-analyze


bool
ppRefAnalyzer_v5::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
	return 0 /*Change here*/;
}

void ppRefAnalyzer_v5::initHistos(const edm::Service<TFileService> & fs)
{
	cout << "from inside init histos method" << endl;
	demoHisto = fs->make<TH1D>("multiplicity","Event Multiplicity (selected tracks)",500,0,500);

	hevtMult = fs->make<TH1D>("hevtMult", "Event Multiplicity for Selected Tracks", 100, 0, 100);


	// Vertex performance histograms
	hvtxPerEvent = fs->make<TH1D>("hvtxPerEvent", "Vertices Per Event", 10, 0, 10);
	hvtxPerfX = fs->make<TH1D>("hvtxX","Vertex x position",10,-1,1);
	hvtxPerfY = fs->make<TH1D>("hvtxY","Vertex x position",10,-1,1);
	hvtxPerfZ = fs->make<TH1D>("hvtxZ","Vertex x position",10,-1,1);

	// Track histograms
	htrackPosPt = fs->make<TH1D>("htrackPosPt","Pt of positively charged tracks",1000,0,200);
	htrackPosEta = fs->make<TH1D>("htrackPosEta","Eta of positively charged tracks",100,-5,10);

	htrackNegPt = fs->make<TH1D>("htrackNegPt","Pt of positively charged tracks",1000, 0,200);
	htrackNegEta = fs->make<TH1D>("trackNegEta","Eta of positively charged tracks",100,-5,10);

	// For all tracks
	htrackEta = fs->make<TH1D>("htrackEta","Eta of all charged tracks",100,-5,10);
	htrackpT = fs->make<TH1D>("htrackpT","pT of all charged tracks",100, 0, 200);
	htrackpx = fs->make<TH1D>("htrackpx","px of charged tracks",100, 0, 200);
	htrackpy = fs->make<TH1D>("htrackpy","py of charged tracks",100, 0, 200);
	htrackpz = fs->make<TH1D>("htrackpz","pz of charged tracks",100, 0, 200);
	htrackNetMom = fs->make<TH1D>("htrackNetMom","Net momentum of the track",1000, 0, 200);

	// Number of tracks per event
	htracksPerEvent = fs->make<TH1D>("htracksPerEvent","Tracks per event",1000,0,200);
	hprimaryVerticesPerEvent = fs->make<TH1D>("hprimaryVerticesPerEvent","Primary vertices per event",10,0,10);

	hvtxDistrXY = fs->make<TH2D>("hvtxDistrXY", "2D Vertex Distribution", 100, -10, 10, 100, -10, 10);

}

bool
ppRefAnalyzer_v5::passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex){

return 1;/* Change here */
}
// ------------ method called once each job just before starting event loop  ------------
void 
ppRefAnalyzer_v5::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ppRefAnalyzer_v5::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ppRefAnalyzer_v5::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ppRefAnalyzer_v5);