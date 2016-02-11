
// system include files
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

#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include <DataFormats/VertexReco/interface/Vertex.h>
#include <DataFormats/VertexReco/interface/VertexFwd.h> 
#include <DataFormats/TrackReco/interface/Track.h>
#include <DataFormats/TrackReco/interface/TrackFwd.h>
#include <DataFormats/HepMCCandidate/interface/GenParticle.h>
#include <DataFormats/HepMCCandidate/interface/GenParticleFwd.h>
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Provenance/interface/RunID.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticle.h"
#include "SimGeneral/HepPDTRecord/interface/ParticleDataTable.h"
#include "SimDataFormats/TrackingAnalysis/interface/TrackingParticleFwd.h"
#include "HepPDT/ParticleID.hh"
#include "RecoJets/JetAlgorithms/interface/JetAlgoHelper.h"
#include "HepMC/HeavyIon.h"

using namespace std;
using namespace edm;
using namespace reco;

class pPbAnalyzer_v5 : public edm::EDAnalyzer {
   public:
      explicit pPbAnalyzer_v5(const edm::ParameterSet&);
      ~pPbAnalyzer_v5();

       static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
       static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );                   void initHistos(const edm::Service<TFileService> & fs);
       bool passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex);


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup&);
      virtual void endJob() ;

      virtual void beginRun(edm::Run const&, edm::EventSetup const&);
      virtual void endRun(edm::Run const&, edm::EventSetup const&);
      virtual void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);
      virtual void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&);

      // ----------member data ---------------------------
      // ----------member data ---------------------------
      edm::InputTag trackSrc_;
      edm::InputTag vertexSrc_;
      std::map<std::string,TH1F*> evtPerf_;
      double vertexZMax_;
	edm::Service<TFileService> fs;

	// Declaring variables for passing variables to constructor to the parameter set
	reco::TrackBase::TrackQuality trackQuality_;
   	double trackCharge;
	
	TH1F *hvtxPerfX, *hvtxPerfY, *hvtxPerfZ, 
	     *htrackPosEta, *htrackPosPt, *htrackNegEta, *htrackNegPt, 
	     *htrackpT, *htrackEta, 
	     *htrackpx, *htrackpy, *htrackpz, *hevtMult, *hvtxPerEvent,
	     *htracksPerEvent, *hprimaryVerticesPerEvent;


	double nPosChargedParticles, nNegChargedParticles;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
	double nTracksWithQualityCut,  nTracksWithoutQualityCut; 
	double trackPt, trackEta;
	bool setPtLimits, setEtaLimits;

	double etaMin_, etaMax_;
	double pTMin_, pTMax_; 
	std::vector<double> ptBins_;
	std::vector<double> etaBins_;
	bool applyCuts_;

        bool iVertexZCut, iVertexValid, iVertexFake;
	int iVertexSize, iVertexTracks;
	bool iAllCuts, iTrackCuts, iVertexCuts;

	double measuredTrackEta;
	int TrackQualityNum_;


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
pPbAnalyzer_v5::pPbAnalyzer_v5(const edm::ParameterSet& iConfig)
:trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
etaMin_(iConfig.getParameter<double>("etaMin")),
etaMax_(iConfig.getParameter<double>("etaMax")),
pTMin_(iConfig.getParameter<double>("pTMin")),
pTMax_(iConfig.getParameter<double>("pTMax")),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
applyCuts_(iConfig.getParameter<bool>("applyCuts"))
{
   //now do what ever initialization is needed
   //
        //usesResource("TFileService");
	initHistos(fs);
	trackQuality_ = TrackBase::qualityByName(iConfig.getParameter<std::string>("trackQuality"));


}


pPbAnalyzer_v5::~pPbAnalyzer_v5()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
pPbAnalyzer_v5::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

//   cout << EventNumber_t << endl;
	cout << " =========== Event Information =========== " << endl;
	cout << "Run number: " << iEvent.run() << "\t"
	     << "Event size: " << iEvent.size() << "\t"
	     << "Event id: " << iEvent.id() << endl;
	cout << " =========== Event Information =========== " << endl;

// Vertex Collection
  Handle<std::vector<reco::Vertex> > vertex;
  iEvent.getByLabel(vertexSrc_, vertex);

  // Track Collection
   Handle<reco::TrackCollection> tcol;
   iEvent.getByLabel(trackSrc_, tcol);

   // Iterators for Track and Vertex
   reco::TrackCollection::const_iterator track;
   std::vector<reco::Vertex>::const_iterator vi; 

   // Copying vertex object from data
   std::vector<reco::Vertex> vtx = *vertex;

   // Looping over vertices
   for ( vi = vtx.begin(); vi != vtx.end(); ++vi ) 
   {

   iVertexZCut = (vi->z() > -1*vertexZMax_) && (vi->z() < +1*vertexZMax_);
   iVertexSize = vertex->size();
   iVertexValid = vi->isValid();
   iVertexTracks = vi->nTracks();
   iVertexFake = vi->isFake();
 
   cout << "Information about vertices " <<  "\t"
	<< "number of vertices: " << iVertexSize << "\t"
	<< "fake: " << iVertexFake << "\t"
	<< "Validity?: " << iVertexValid << "\t"
	<< "Number of tracks per vertex: " << iVertexTracks << "\t"
	<< endl;
	
   // All vertex cuts
   iVertexCuts = (iVertexSize == 1) && ( iVertexValid ) && ( !iVertexFake );

   cout << "Status of vertex cut: " << iVertexCuts << endl;

   // Loop over tracks
//   for(  track = tcol->begin(); track != tcol->end() ; ++track )
   for(  track = tcol->begin(); track != tcol->end() && iVertexCuts ; ++track )
	{ 
	// Track Quality cut
	iTrackCuts = (track->quality(reco::TrackBase::qualityByName("tight")) == 1);
	cout << "Track Quality : " << track->quality(reco::TrackBase::qualityByName("tight")) << 1 << endl;

	// Fill histogram only if Track Quality matches
	if (iTrackCuts){
	cout << "Information about tracks: " << endl;
	cout << "Net Track Momentum " << track->p() << endl;
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
      } // close while
   }
   // close for - loop over tracks

   }// Loop over vertices

#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}
//close-analyze

void pPbAnalyzer_v5::initHistos(const edm::Service<TFileService> & fs)
{

	hevtMult = fs->make<TH1F>("hevtMult", "Event Multiplicity for Selected Tracks", 100, 0, 100);

	// Vertex performance histograms
	hvtxPerfX = fs->make<TH1F>("hvtxX","Vertex x position",1000,-1,1);
	hvtxPerfY = fs->make<TH1F>("hvtxY","Vertex x position",1000,-1,1);
	hvtxPerfZ = fs->make<TH1F>("hvtxZ","Vertex x position",1000,-1,1);

	// Track histograms
	htrackPosPt = fs->make<TH1F>("htrackPosPt","Pt of positively charged tracks",1000, 0, 200);
	htrackPosEta = fs->make<TH1F>("htrackPosEta","Eta of positively charged tracks",1000,-5, 10);

	htrackNegPt = fs->make<TH1F>("htrackNegPt","Pt of positively charged tracks",1000, 0, 200);
	htrackNegEta = fs->make<TH1F>("htrackNegEta","Eta of positively charged tracks",1000, -5, 10);

	// For all tracks
	htrackEta = fs->make<TH1F>("htrackEta","Eta of all charged tracks",1000, -5, 10);
	htrackpT = fs->make<TH1F>("htrackpT","pT of all charged tracks",100, 0, 200);

	htrackpx = fs->make<TH1F>("htrackpx","px of charged tracks",100, 0, 200);
	htrackpy = fs->make<TH1F>("htrackpy","py of charged tracks",100, 0, 200);
	htrackpz = fs->make<TH1F>("htrackpz","pz of charged tracks",100, 0, 200);

	// Number of tracks per event
	htracksPerEvent = fs->make<TH1F>("htracksPerEvent","Tracks per event",100, 0, 200);
	hprimaryVerticesPerEvent = fs->make<TH1F>("hprimaryVerticesPerEvent","Primary vertices per event", 10, 0, 10);

}


// ------------ method called once each job just before starting event loop  ------------
void 
pPbAnalyzer_v5::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
pPbAnalyzer_v5::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
pPbAnalyzer_v5::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
pPbAnalyzer_v5::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
pPbAnalyzer_v5::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
pPbAnalyzer_v5::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
pPbAnalyzer_v5::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(pPbAnalyzer_v5);


/*
 * 
 * 1. Spectra for different eta ranges
 *
 */
