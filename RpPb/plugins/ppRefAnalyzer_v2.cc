
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

class ppRefAnalyzer_v2 : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit ppRefAnalyzer_v2(const edm::ParameterSet&);
      ~ppRefAnalyzer_v2();

      static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
	static bool vtxSort( const reco::Vertex &  a, const reco::Vertex & b );
	void initHistos(const edm::Service<TFileService> & fs);
        bool passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex);

   private:
      virtual void beginJob() override;
      virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
      virtual void endJob() override;

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
        int TrackQualityNum_;
	reco::TrackBase::TrackQuality trackQuality_;
   	double trackCharge;
	
	TH1F *demoHisto; 
	TH1F *vtxPerfX, *vtxPerfY, *vtxPerfZ, 
	     *trackPosEta, *trackPosPt, *trackNegEta, *trackNegPt, 
	     *trackpT, *trackEta, 
		*tracksPerEvent, *primaryVerticesPerEvent;

	double nPosChargedParticles, nNegChargedParticles;
	TH2D *vtxDistrXY;
	TH1F *evtMult, *vtxPerEvent;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
	double nTracksWithQualityCut,  nTracksWithoutQualityCut; 
	double measuredTrackEta, trackChi2;
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
ppRefAnalyzer_v2::ppRefAnalyzer_v2(const edm::ParameterSet& iConfig)
:trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
applyCuts_(iConfig.getParameter<bool>("applyCuts")),
TrackQualityNum_(iConfig.getParameter<int>("TrackQualityNum"))
{
   //now do what ever initialization is needed
        usesResource("TFileService");
	initHistos(fs);
	trackQuality_ = TrackBase::qualityByName(iConfig.getParameter<std::string>("TrackQuality"));

}


ppRefAnalyzer_v2::~ppRefAnalyzer_v2()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
ppRefAnalyzer_v2::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{

/* Collections */

   Handle<reco::TrackCollection> tcol;
//   Handle<reco::Track> tcol;
   iEvent.getByLabel(trackSrc_, tcol);

   reco::TrackCollection::const_iterator track;
//   reco::Track::const_iterator track;
// Fill event multiplicity only when its greater than zero
   double multiplicityPerEvent = tcol->size();
   if (multiplicityPerEvent > 0 ) evtMult->Fill(multiplicityPerEvent);

   cout << "multiplicity per event " << multiplicityPerEvent << endl;
   cout << "eta, pt, charge" << endl;

   // Apply Track Quality cuts 
   // Populate the histograms for pt, et based on charge
   // trackQuality_ = 2 for "highPurity"
   cout << "Track Quality String: " << trackQuality_ << endl;
   nTracksWithQualityCut = 0;  nTracksWithoutQualityCut = 0;
   double trackEtaMax = 2.4, trackEtaMin = -2.4;

   // 2 = highPurity, 6 = highuritySetWithPV
   if (trackQuality_ == TrackQualityNum_){
   for(  track = tcol->begin(); track != tcol->end() ; ++track )
   { 
   	cout << "Track momentum: " << track->momentum() << "\t"
   	     << "Track ndof: " << track->ndof() << "\t" 
   	     << "Track chi2: " << track->chi2() << endl;

 	//trackCharge = track->charge();	

   /* Applying eta cut for the track */
   measuredTrackEta = track->eta(); 
   bool trackEtaCut = ( measuredTrackEta < trackEtaMax ) 
		     && ( measuredTrackEta < trackEtaMin );

	if ( trackCharge == 1 && trackEtaCut ) {
		trackPosPt->Fill(track->pt());
		trackPosEta->Fill(track->eta());
	}else if ( trackCharge == -1 && trackEtaCut ){
		trackNegPt->Fill(track->pt());
		trackNegEta->Fill(track->eta());
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

  Handle<std::vector<reco::Vertex> > vertex;
  iEvent.getByLabel(vertexSrc_, vertex);
  // Number of vertices per event

  vtxPerEvent->Fill(vertex->size());

	std::vector<reco::Vertex> vsorted = *vertex;
	// sort the vertcies by number of tracks in descending order
	//    // use chi2 as tiebreaker
	std::sort( vsorted.begin(), vsorted.end(), ppRefAnalyzer_v2::vtxSort );

  cout << "======== INFO RELATED TO VERTICES =========" << "\n";
  cout << "Vertex size : " << vertex->size() << endl;

   // Vertex performance histograms
   int vcount = 0; 
   if (vcount == 0){/*  Do nothing  */};
   std::vector<reco::Vertex>::const_iterator vi; 
   for( vi = vsorted.begin(); vi != vsorted.end() ; vi++ )
   {

  
  cout << "Is the vertex fake: " << vi->isFake() << "\t"
       << "Is the vertex valid: " << vi->isValid() << "\t"
       << "How many tracks does the vertex have: " << vi->nTracks() << "\n";

  vtxDistrXY->Fill(vi->x(), vi->y());

     /*  
     vtxPerfX->Fill(vi->x());
     vtxPerfY->Fill(vi->y());
     vtxPerfZ->Fill(vi->z());

     cout << "Position of the vertex (x, y, z): " 
	  << vi->x() << "\t"
          << vi->y() << "\t"
          << vi->z() << "\t"
	  << endl;

     vcount++;
     */
   }
  cout << "===========================================" << "\n";

   // skip events with no PV, this should not happen
   if( vsorted.size() == 0) return;
   // skip events failing vertex cut
   if( fabs(vsorted[0].z()) > vertexZMax_ ) return;

	//initHistos(fs);
	//
	// user vertex with most tracks as primary vertex
	math::XYZPoint vtxPoint(0, 0, 0);
	vzErr = 0, vxErr = 0, vyErr = 0;
	if ( vsorted.size() != 0 ){
		vtxPoint = vsorted.begin()->position();
		vxErr = vsorted.begin()->xError();
		vyErr = vsorted.begin()->yError();
		vzErr = vsorted.begin()->zError();
	}

	// Loop through reconstructed tracks and fill
	// track spectrum and performance histograms
	cout << "Iterating over tracks" << endl;
	for ( track = tcol->begin(); track != tcol->end(); track++ ){
		dxy=0, dz=0, dxysigma=0, dzsigma=0;
		dxy = track->dxy(vtxPoint);
		dz = track->dz(vtxPoint);
		dxysigma = sqrt(track->d0Error()*track->d0Error() + vxErr*vyErr);
		dzsigma = sqrt(track->dzError()*track->dzError() + vzErr*vzErr);
	if( !passesTrackCuts(*track, vsorted[0]) ) continue;
		trackpT->Fill(track->pt());
		trackEta->Fill(track->pt());
	/*  	
		cout << "dxy, dx, dy, dzsigma :  " << "\t"
		     << dxy << "\t"
		     << dz << "\t"
		     << dzsigma << "\t"
		     << endl;
	*/

	}

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
ppRefAnalyzer_v2::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
	return 0 /*Change here*/;
}

void ppRefAnalyzer_v2::initHistos(const edm::Service<TFileService> & fs)
{
	cout << "from inside init histos method" << endl;
	demoHisto = fs->make<TH1F>("multiplicity","Event Multiplicity (selected tracks)",500,0,500);

	evtMult = fs->make<TH1F>("evtMult", "Event Multiplicity for Selected Tracks", 100, 0, 100);

	vtxPerEvent = fs->make<TH1F>("vtxPerEvent", "Vertices Per Event", 10, 0, 10);
	// Vertex performance histograms
	vtxPerfX = fs->make<TH1F>("vtxX","Vertex x position",10,-1,1);
	vtxPerfY = fs->make<TH1F>("vtxY","Vertex x position",10,-1,1);
	vtxPerfZ = fs->make<TH1F>("vtxZ","Vertex x position",10,-1,1);

	// Track histograms
	trackPosPt = fs->make<TH1F>("trackPosPt","Pt of positively charged tracks",100,0,200);
	trackPosEta = fs->make<TH1F>("trackPosEta","Eta of positively charged tracks",100,-5,10);

	trackNegPt = fs->make<TH1F>("trackNegPt","Pt of positively charged tracks",100,0,200);
	trackNegEta = fs->make<TH1F>("trackNegEta","Eta of positively charged tracks",100,-5,10);

	// For all tracks
	trackEta = fs->make<TH1F>("trackEta","Eta of all charged tracks",100,-5,10);
	trackpT = fs->make<TH1F>("trackpT","pT of all charged tracks",100, 0, 200);

	// Number of tracks per event
	tracksPerEvent = fs->make<TH1F>("tracksPerEvent","Tracks per event",100,0,200);
	primaryVerticesPerEvent = fs->make<TH1F>("primaryVerticesPerEvent","Primary vertices per event",10,0,10);

	vtxDistrXY = fs->make<TH2D>("vtxDistrXY", "2D Vertex Distribution", 100, -10, 10, 100, -10, 10);

}

bool
ppRefAnalyzer_v2::passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex){

return 1;/* Change here */
}
// ------------ method called once each job just before starting event loop  ------------
void 
ppRefAnalyzer_v2::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
ppRefAnalyzer_v2::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
ppRefAnalyzer_v2::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(ppRefAnalyzer_v2);
