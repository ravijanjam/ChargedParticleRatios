import FWCore.ParameterSet.Config as cms
import FWCore.Utilities.FileUtils as FileUtils
from FWCore.ParameterSet.VarParsing import VarParsing


''' timeStamp the outputfile from cmsRun '''
import datetime, time
ts = time.time()
timeLabel = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d-%H%M%S')

timeStampedFileName = "ppRefOutput" + timeLabel + ".root"

import os, sys
#pyFile =  os.path.basename(sys.argv[1])
#print os.path.splitext(pyFile)[0]

''' Declare VarParsing options to be used from commandline '''
options = VarParsing('analysis')
options.register('maxNumEvents',
			1000,
			VarParsing.multiplicity.singleton,
			VarParsing.varType.int,
			"")

options.parseArguments()
''' ====================================================== '''

process = cms.Process("userAnalyzer")

process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 5000


''' Global Tag Information '''
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')

from Configuration.AlCa.GlobalTag_condDBv2 import GlobalTag
process.GlobalTag = GlobalTag(process.GlobalTag, '75X_dataRun2_Prompt_ppAt5TeV_v0', '')


''' High Level Trigger Paths '''
'''https://github.com/cms-sw/cmssw/blob/CMSSW_7_5_X/HLTrigger/Configuration/tables/HIon.txt'''
process.load("HLTrigger.HLTfilters.hltHighLevel_cfi")
process.hltL1MinimumBiasHF1AND = process.hltHighLevel.clone()
#process.hltL1MinimumBiasHF1AND.HLTPaths = ["HLT_Physics_v2"]
process.hltL1MinimumBiasHF1AND.HLTPaths = ["HLT_ZeroBias_v2"]
#process.hltL1MinimumBiasHF1AND.HLTPaths = ["HLT_ZeroBiasPixel_SingleTrack_v*"]
#process.hltL1MinimumBiasHF1AND.HLTPaths = ["HLT_FullTrack24_v2"]


''' Number of events to run this config file '''
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(options.maxNumEvents) )

''' Output histograms go here '''
process.TFileService = cms.Service("TFileService", 
	fileName = cms.string(timeStampedFileName) 
	)

process.options = cms.untracked.PSet( wantSummary = cms.untracked.bool(True) )

''' Files to run in this config file ''' 
process.source = cms.Source("PoolSource",
    fileNames = cms.untracked.vstring(
#	'/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root'
	'/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/262/271/00000/004E78B2-1396-E511-9F94-02163E01439C.root'
    )
)

'''
mylist = FileUtils.loadListFromFile ('ppRef_MinimumBias1.txt')
for fname in mylist:
 process.source.fileNames.append('%s' % (fname))
'''



''' If some product name is not found, this line makes sure the config file runs without a product not found exception '''
process.options = cms.untracked.PSet(
	SkipEvent = cms.untracked.vstring('ProductNotFound')
)

''' Timing Information per module '''
#process.Timing = cms.Service("Timing")

''' All the parameters to EDAnalyzer '''
process.userAnalyzer= cms.EDAnalyzer('ppRefAnalyzer_v5',
	trackSrc = cms.InputTag("generalTracks"),
	vertexSrc = cms.InputTag("offlinePrimaryVertices"),
	vertexZMax = cms.double(15.), # The 15cm constraint
	TrackQuality = cms.string('highPurity'),
	TrackQualityNum = cms.int32(1),
	chi2 = cms.double(0),

	etaMax = cms.double(1),
	etaMin = cms.double(-1),

	pTMin = cms.double(100),
	pTMax = cms.double(200),
	
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
   ),
	   applyCuts = cms.bool(False)
)

''' Specify the sequency in which the modules need to be run '''

''' Specify the paths to be run '''
process.p = cms.Path(process.hltL1MinimumBiasHF1AND*process.userAnalyzer )
#process.p = cms.Path(process.userAnalyzer*process.hltL1MinimumBiasHF1AND )
#process.p = cms.Path( process.userAnalyzer )
