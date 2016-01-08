// -*- C++ -*-
//
// Package:    TestAnalyzer_v1/TestAnalyzer_v1
// Class:      TestAnalyzer_v1
// 
/**\class TestAnalyzer_v1 TestAnalyzer_v1.cc TestAnalyzer_v1/TestAnalyzer_v1/plugins/TestAnalyzer_v1.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Ravi Kishore Janjam
//         Created:  Mon, 04 Jan 2016 20:34:12 GMT
//
//


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
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class TestAnalyzer_v1 : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
   public:
      explicit TestAnalyzer_v1(const edm::ParameterSet&);
      ~TestAnalyzer_v1();

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
	
	TH1F *demoHisto; 
	TH1F *vtxPerfX, *vtxPerfY, *vtxPerfZ, 
	     *trackPosEta, *trackPosPt, *trackNegEta, *trackNegPt, 
	     *trackpT, *trackEta, 
		*tracksPerEvent, *primaryVerticesPerEvent;

	double nPosChargedParticles, nNegChargedParticles;


	/* Variable used inside Analyze method */
	double vxErr, vyErr, vzErr;
	double dxy, dz, dxysigma, dzsigma;
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
TestAnalyzer_v1::TestAnalyzer_v1(const edm::ParameterSet& iConfig)
:trackSrc_(iConfig.getParameter<edm::InputTag>("trackSrc")),
vertexSrc_(iConfig.getParameter<edm::InputTag>("vertexSrc")),
vertexZMax_(iConfig.getParameter<double>("vertexZMax")),
ptBins_(iConfig.getParameter<std::vector<double> >("ptBins")),
etaBins_(iConfig.getParameter<std::vector<double> >("etaBins")),
applyCuts_(iConfig.getParameter<bool>("applyCuts"))
{
   //now do what ever initialization is needed
        usesResource("TFileService");
	initHistos(fs);

}


TestAnalyzer_v1::~TestAnalyzer_v1()
{
 
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
TestAnalyzer_v1::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

/* Collections */

   Handle<reco::TrackCollection> tcol;
   iEvent.getByLabel(trackSrc_, tcol);

/* Numbers from collections */
//   int multiplicity =0;
	double ch;
   reco::TrackCollection::const_iterator track;
   cout << "Multiplicity in the events: " << tcol->size() << "\t";
   cout << "eta, pt, charge" << endl;
   for(  track = tcol->begin(); track != tcol->end() ; ++track )
   { 
	/*
	cout << "(" << track->eta() << "\t"
	     << track->pt() << "\t"
	     << track->charge() << "\t"
	     << ")" << "\t" << ",";
	*/
 	ch = track->charge();	
	if ( ch == 1 ) {
		trackPosPt->Fill(track->pt());
		trackPosEta->Fill(track->eta());
	}else if ( ch == -1 ){
		trackNegPt->Fill(track->pt());
		trackNegEta->Fill(track->eta());
	}
   }
	cout << endl;

  Handle<std::vector<reco::Vertex> > vertex;
  iEvent.getByLabel(vertexSrc_, vertex);
  cout << "Vertex size : " << vertex->size() << endl;

	std::vector<reco::Vertex> vsorted = *vertex;
	// sort the vertcies by number of tracks in descending order
	//    // use chi2 as tiebreaker
	std::sort( vsorted.begin(), vsorted.end(), TestAnalyzer_v1::vtxSort );

   // Vertex performance histograms
   int vcount = 0; 
   std::vector<reco::Vertex>::const_iterator vi; 
   for( vi = vsorted.begin(); vi != vsorted.end() ; vi++ )
   {
     vtxPerfX->Fill(vi->x());
     vtxPerfY->Fill(vi->y());
     vtxPerfZ->Fill(vi->z());

     cout << "x position of the vertex " << vi->x() << endl;
     cout << "y position of the vertex " << vi->y() << endl;
     cout << "z position of the vertex " << vi->z() << endl;

     vcount++;
   }

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
	for ( track = tcol->begin(); track != tcol->end(); track++ ){
		dxy=0, dz=0, dxysigma=0, dzsigma=0;
		dxy = track->dxy(vtxPoint);
		dz = track->dz(vtxPoint);
		dxysigma = sqrt(track->d0Error()*track->d0Error() + vxErr*vyErr);
		dzsigma = sqrt(track->dzError()*track->dzError() + vzErr*vzErr);
if( !passesTrackCuts(*track, vsorted[0]) ) continue;
		trackpT->Fill(track->pt());
		trackEta->Fill(track->pt());

	}

   // Dummy filling of histograms
   cout << "Populating the demo histogram" << endl;
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
TestAnalyzer_v1::vtxSort( const reco::Vertex &  a, const reco::Vertex & b )
{
  if( a.tracksSize() != b.tracksSize() )
    return  a.tracksSize() > b.tracksSize() ? true : false ;
  else
    return  a.chi2() < b.chi2() ? true : false ;  
}

void TestAnalyzer_v1::initHistos(const edm::Service<TFileService> & fs)
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

bool
TestAnalyzer_v1::passesTrackCuts(const reco::Track & track, const reco::Vertex & vertex){

return 1;/* Change here */
}
// ------------ method called once each job just before starting event loop  ------------
void 
TestAnalyzer_v1::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void 
TestAnalyzer_v1::endJob() 
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
TestAnalyzer_v1::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(TestAnalyzer_v1);
