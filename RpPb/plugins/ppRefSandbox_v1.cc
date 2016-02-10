
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

class ppRefSandbox_v1 : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit ppRefSandbox_v1(const edm::ParameterSet&);
      ~ppRefSandbox_v1();

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
	double etaMin_;
	double etaMax_;
	std::vector<double> etaBins_;
	bool applyCuts_;
	int TrackQualityNum_;
	reco::TrackBase::TrackQuality trackQuality_;
	
	TH1D *demoHisto; 
	TH1D *vtxPerfX, *vtxPerfY, *vtxPerfZ, 
	     *trackPosEta, *trackPosPt, *trackNegEta, *trackNegPt, 
	     *trackpT, *trackEta, *trackNetMom, *trackpx, *trackpy, *trackpz,
		*tracksPerEvent, *primaryVerticesPerEvent;

	double nPosChargedParticles, nNegChargedParticles;
	TH2D *vtxDistrXY;
	TH1D *evtMult, *vtxPerEvent;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
	double nTracksWithQualityCut,  nTracksWithoutQualityCut; 
	double measuredTrackEta, trackChi2;
	double trackCharge;


	bool iVertexZCut, iVertexValid, iVertexFake;
	int iVertexSize, iVertexTracks;
	bool iVertexCuts;

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
ppRefSandbox_v1::ppRefSandbox_v1(const edm::ParameterSet& iConfig)
:trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
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


ppRefSandbox_v1::~ppRefSandbox_v1()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
ppRefSandbox_v1::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

	iEvent.getRun();
/* Collections */


  // Vertex Collection
  Handle<std::vector<reco::Vertex> > vertex;
  iEvent.getByLabel(vertexSrc_, vertex);

   std::vector<reco::Vertex>::const_iterator vi; 
   std::vector<reco::Vertex> vtx = *vertex;

   for ( vi = vtx.begin(); vi != vtx.end(); ++vi ) {

   iVertexZCut = (vi->z() > -1*vertexZMax_) && (vi->z() < +1*vertexZMax_);
   iVertexSize = vertex->size();
   iVertexValid = vi->isValid();
   iVertexTracks = vi->nTracks();
   iVertexFake = vi->isFake();

  
   cout << "Information about vertices " << iEvent.id() << "\t"
	<< "validity: " << iVertexSize << "\t"
	<< "fake: " << iVertexFake << "\t"
	<< "Number of tracks per vertex: " << iVertexTracks << "\t"
	<< endl;
   }


//	cout << "number of vertices: " << iVertexSize << "\t";
//	cout << "validity: " << iVertexSize << "\t";

   iVertexCuts = (iVertexSize == 1) && ( iVertexValid ) && ( iVertexFake );

   // 2 = highPurity, 6 = highuritySetWithPV

  vtxPerEvent->Fill(vertex->size());

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
}


bool
ppRefSandbox_v1::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
	return 0 /*Change here*/;
}

void ppRefSandbox_v1::initHistos(const edm::Service<TFileService> & fs)
{
	cout << "from inside init histos method" << endl;
	demoHisto = fs->make<TH1D>("multiplicity","Event Multiplicity (selected tracks)",500,0,500);

	evtMult = fs->make<TH1D>("evtMult", "Event Multiplicity for Selected Tracks", 100, 0, 100);


	// Vertex performance histograms
	vtxPerEvent = fs->make<TH1D>("vtxPerEvent", "Vertices Per Event", 10, 0, 10);
	vtxPerfX = fs->make<TH1D>("vtxX","Vertex x position",10,-1,1);
	vtxPerfY = fs->make<TH1D>("vtxY","Vertex x position",10,-1,1);
	vtxPerfZ = fs->make<TH1D>("vtxZ","Vertex x position",10,-1,1);

	// Track histograms
	trackPosPt = fs->make<TH1D>("trackPosPt","Pt of positively charged tracks",1000,0,200);
	trackPosEta = fs->make<TH1D>("trackPosEta","Eta of positively charged tracks",100,-5,10);

	trackNegPt = fs->make<TH1D>("trackNegPt","Pt of positively charged tracks",1000, 0,200);
	trackNegEta = fs->make<TH1D>("trackNegEta","Eta of positively charged tracks",100,-5,10);

	// For all tracks
	trackEta = fs->make<TH1D>("trackEta","Eta of all charged tracks",100,-5,10);
	trackpT = fs->make<TH1D>("trackpT","pT of all charged tracks",100, 0, 200);
	trackpx = fs->make<TH1D>("trackpx","px of charged tracks",100, 0, 200);
	trackpy = fs->make<TH1D>("trackpy","py of charged tracks",100, 0, 200);
	trackpz = fs->make<TH1D>("trackpz","pz of charged tracks",100, 0, 200);
	trackNetMom = fs->make<TH1D>("trackNetMom","Net momentum of the track",1000, 0, 200);

	// Number of tracks per event
	tracksPerEvent = fs->make<TH1D>("tracksPerEvent","Tracks per event",1000,0,200);
	primaryVerticesPerEvent = fs->make<TH1D>("primaryVerticesPerEvent","Primary vertices per event",10,0,10);

	vtxDistrXY = fs->make<TH2D>("vtxDistrXY", "2D Vertex Distribution", 100, -10, 10, 100, -10, 10);

}

bool
ppRefSandbox_v1::passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex){

return 1;/* Change here */
}
// ------------ method called once each job just before starting event loop  ------------
void 
ppRefSandbox_v1::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ppRefSandbox_v1::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ppRefSandbox_v1::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ppRefSandbox_v1);
