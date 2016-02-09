
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

class pPbAnalyzer_v3 : public edm::EDAnalyzer {
   public:
      explicit pPbAnalyzer_v3(const edm::ParameterSet&);
      ~pPbAnalyzer_v3();

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
	std::vector<double> ptBins_;
	std::vector<double> etaBins_;
	bool applyCuts_;
	reco::TrackBase::TrackQuality trackQuality_;
   	double trackCharge;
	
	TH1F *hvtxPerfX, *hvtxPerfY, *hvtxPerfZ, 
	     *htrackPosEta, *htrackPosPt, *htrackNegEta, *htrackNegPt, 
	     *htrackpT, *htrackEta, 
	     *htracksPerEvent, *hprimaryVerticesPerEvent;

	double nPosChargedParticles, nNegChargedParticles;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
	double nTracksWithQualityCut,  nTracksWithoutQualityCut; 
	double trackPt, trackEta;
	bool setPtLimits;

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
pPbAnalyzer_v3::pPbAnalyzer_v3(const edm::ParameterSet& iConfig)
:trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
applyCuts_(iConfig.getParameter<bool>("applyCuts"))
{
   //now do what ever initialization is needed
   //
        //usesResource("TFileService");
	initHistos(fs);
	trackQuality_ = TrackBase::qualityByName(iConfig.getParameter<std::string>("TrackQuality"));


}


pPbAnalyzer_v3::~pPbAnalyzer_v3()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
pPbAnalyzer_v3::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   Handle<reco::TrackCollection> tcol;
   iEvent.getByLabel(trackSrc_, tcol);

   reco::TrackCollection::const_iterator track;
   double multiplicityPerEvent = tcol->size();
   cout << "eta, pt, charge" << endl;

   // Apply Track Quality cuts 
   // Populate the histograms for pt, eta based on charge
   // trackQuality_ = 2 for "highPurity"
   cout << "Track Quality String: " << trackQuality_ << endl;
   nTracksWithQualityCut = 0;  nTracksWithoutQualityCut = 0;
   if (trackQuality_ == 2){
   for(  track = tcol->begin(); track != tcol->end() ; ++track )
   { 
 	trackCharge = track->charge();	
	/*  
	cout << "Track charge : " << trackCharge << "\t" 
	     << "Track pT : " << track->pt()
	     << endl;
	 */
	trackPt = track->pt();
	trackEta = track->eta();

	// Setting limits for high pT tracks
	setPtLimits = trackPt > 10 && trackPt < 200;

	if ( trackCharge == 1 ) {
		htrackPosPt->Fill(trackPt);
		htrackPosEta->Fill(trackEta);
	} else if ( trackCharge == -1 ){
		htrackNegPt->Fill(trackPt);
		htrackNegEta->Fill(trackEta);
	}
	nTracksWithQualityCut++;
   }
	cout << endl;
  }else { 
	  
	  nTracksWithoutQualityCut = multiplicityPerEvent - nTracksWithQualityCut ; 
  }

   cout << "================================================================" << endl;
   cout << "Multiplicity per Events: " << multiplicityPerEvent << endl;
   cout << "Tracks WITH Track Quality cut " << nTracksWithQualityCut << endl;
   cout << "Tracks WITHOUT CHOSEN Track Quality cut " << nTracksWithoutQualityCut << endl;
   cout << "================================================================" << endl;


#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif
   
#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}

void pPbAnalyzer_v3::initHistos(const edm::Service<TFileService> & fs)
{

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

	// Number of tracks per event
	htracksPerEvent = fs->make<TH1F>("htracksPerEvent","Tracks per event",100, 0, 200);
	hprimaryVerticesPerEvent = fs->make<TH1F>("hprimaryVerticesPerEvent","Primary vertices per event", 10, 0, 10);

}


// ------------ method called once each job just before starting event loop  ------------
void 
pPbAnalyzer_v3::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
pPbAnalyzer_v3::endJob() 
{
}

// ------------ method called when starting to processes a run  ------------
void 
pPbAnalyzer_v3::beginRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a run  ------------
void 
pPbAnalyzer_v3::endRun(edm::Run const&, edm::EventSetup const&)
{
}

// ------------ method called when starting to processes a luminosity block  ------------
void 
pPbAnalyzer_v3::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method called when ending the processing of a luminosity block  ------------
void 
pPbAnalyzer_v3::endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&)
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
pPbAnalyzer_v3::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(pPbAnalyzer_v3);


/*
 * 
 * 1. Spectra for different eta ranges
 *
 */
