// system include files
#include <memory>

// user include files
#include <memory>
#include <algorithm>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

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
//
struct Info{

	string m;
};

class RpPbAnalyzerMC_v4 : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit RpPbAnalyzerMC_v4(const edm::ParameterSet&);
      ~RpPbAnalyzerMC_v4();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );
	void initHistos(const edm::Service<TFileService> & fs);
        bool passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex);
//	void messageLogger(Info info, bool t);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

      // ----------member data ---------------------------
      edm::InputTag tvertexSrc, tgenSrc;
      std::map<std::string,TH1F*> evtPerf_;
      double vertexZMax_;
	edm::Service<TFileService> fs;

	// Declaring variables for passing variables to constructor to the parameter set
	std::vector<double> ptBins_;
	std::vector<double> etaBins_;
	bool applyCuts_;
   	double trackCharge;
	
	TH1F *demoHisto; 
	TH1F *vtxPerfX, *vtxPerfY, *vtxPerfZ, 
	     *trackPosEta, *trackPosPt, *trackNegEta, *trackNegPt, 
	     *trackpT, *trackEta, 
	     *tracksPerEvent, *primaryVerticesPerEvent;

	double nPosChargedParticles, nNegChargedParticles;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
	double nTracksWithQualityCut,  nTracksWithoutQualityCut; 
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
RpPbAnalyzerMC_v4::RpPbAnalyzerMC_v4(const edm::ParameterSet& iConfig)
//:tgenSrc = (iConfig.getParameter<edm::InputTag>("trackSrc")),
//tvertexSrc = (iConfig.getParameter<edm::InputTag>("vertexSrc"))
{
   //now do what ever initialization is needed
        usesResource("TFileService");
	initHistos(fs);
	tgenSrc = (iConfig.getParameter<edm::InputTag>("trackSrc"));
	tvertexSrc = (iConfig.getParameter<edm::InputTag>("vertexSrc"));

}


RpPbAnalyzerMC_v4::~RpPbAnalyzerMC_v4()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
RpPbAnalyzerMC_v4::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   /* Read Collections */
   /* Do rapidity shift  */

   Handle<reco::GenParticleCollection> gencol;
   iEvent.getByLabel(tgenSrc, gencol);

   Handle< vector<reco::Vertex> > vertex;
   iEvent.getByLabel(tvertexSrc, vertex);


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
} //close-RpPbAnalyzerMC::analyze


bool
RpPbAnalyzerMC_v4::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
	return 0 /*Change here*/;
}

void RpPbAnalyzerMC_v4::initHistos(const edm::Service<TFileService> & fs)
{
	cout << "from inside init histos method" << endl;
	demoHisto = fs->make<TH1F>("multiplicity","Event Multiplicity (selected tracks)",500,0,500);

	// Vertex performance histograms
	vtxPerfX = fs->make<TH1F>("vtxX","Vertex x position",1000,-1,1);
	vtxPerfY = fs->make<TH1F>("vtxY","Vertex x position",1000,-1,1);
	vtxPerfZ = fs->make<TH1F>("vtxZ","Vertex x position",1000,-1,1);

	// Track histograms
	trackPosPt = fs->make<TH1F>("trackPosPt","Pt of positively charged tracks",1000,-5,10);
	trackPosEta = fs->make<TH1F>("trackPosEta","Eta of positively charged tracks",1000,-5,10);

	trackNegPt = fs->make<TH1F>("trackNegPt","Pt of positively charged tracks",1000,-5,10);
	trackNegEta = fs->make<TH1F>("trackNegEta","Eta of positively charged tracks",1000,-5,10);

	// For all tracks
	trackEta = fs->make<TH1F>("trackEta","Eta of all charged tracks",1000,-5,10);
	trackpT = fs->make<TH1F>("trackpT","pT of all charged tracks",100, 0, 200);

	// Number of tracks per event
	tracksPerEvent = fs->make<TH1F>("tracksPerEvent","Tracks per event",100,0,400);
	primaryVerticesPerEvent = fs->make<TH1F>("primaryVerticesPerEvent","Primary vertices per event",10,0,10);

}

//void RpPbAnalyzerMC_v4::messageLogger( Info in, bool t ){

bool
RpPbAnalyzerMC_v4::passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex){

return 1;/* Change here */
}
// ------------ method called once each job just before starting event loop  ------------
void 
RpPbAnalyzerMC_v4::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
RpPbAnalyzerMC_v4::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
RpPbAnalyzerMC_v4::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(RpPbAnalyzerMC_v4);
