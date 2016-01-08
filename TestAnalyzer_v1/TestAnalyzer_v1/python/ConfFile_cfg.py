import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

''' Number of events to run this config file '''
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )

''' Output histograms go here '''
process.TFileService = cms.Service("TFileService", 
	fileName = cms.string("myhistogram.root") 
	)

''' Files to run in this config file '''
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
	'/store/hidata/HIRun2015/HIFlowCorr/AOD/PromptReco-v1/000/262/548/00000/F8A8960E-D39A-E511-B151-02163E014506.root'
    )
)

''' If some product name is not found, this line makes sure the config file runs without a product not found exception '''
process.options = cms.untracked.PSet(
	SkipEvent = cms.untracked.vstring('ProductNotFound')
)

''' All the parameters to EDAnalyzer '''
process.demo = cms.EDAnalyzer('TestAnalyzer_v1',
	trackSrc = cms.InputTag("hiGeneralTracks"),
#	vertexSrc = cms.InputTag("offlinePrimaryVerticesWithBS"),
	vertexSrc = cms.InputTag("hiSelectedVertex"),
	vertexZMax = cms.double(15.), # The 15cm constraint
        ptBins = cms.vdouble(
        0.0, 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45,
        0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 0.95,
        1.1, 1.2, 1.4, 1.6, 1.8, 2.0, 2.2, 2.4, 3.2, 4.0, 4.8, 5.6, 6.4,
        7.2, 9.6, 12.0, 14.4, 19.2, 24.0, 28.8, 35.2, 41.6, 48.0, 60.8, 73.6, 86.4, 103.6,
        130.0
   ),
	   etaBins = cms.vdouble( 
        -2.46, -2.36, -2.26, -2.16, -2.06, -1.96, -1.86, -1.76, -1.66, -1.56,
        -1.46, -1.36, -1.26, -1.16, -1.06, -0.96, -0.86, -0.76, -0.66, -0.56,
        -0.46, -0.36, -0.26, -0.16, -0.06, 0.04, 0.14, 0.24, 0.34, 0.44, 0.54, 
        0.64, 0.74, 0.84, 0.94, 1.04, 1.14, 1.24, 1.34, 1.44, 1.54, 1.64, 1.74,
        1.84, 1.94, 2.04, 2.14, 2.24, 2.34, 2.44
   )
)


''' Specify the paths to be run '''
process.p = cms.Path(process.demo)
