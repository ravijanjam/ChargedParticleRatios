from CRABClient.UserUtilities import config, getUsernameFromSiteDB


''' timeStamp the outputfile from cmsRun '''

import datetime, time

ts = time.time()
timeLabel = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d-%H%M%S')

timeStampedFolderName = "pPbAnalyzerFull" + "info1" + timeLabel 


''' ==================================== '''

config = config()

config.General.requestName = timeStampedFolderName 
config.General.workArea = 'pPb_crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'pPbAnalysis_v4_cfg.py'

config.Data.inputDataset = '/PAHighPt/HIRun2013-28Sep2013-v1/RECO'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions13/pPb/Prompt/Cert_210498-211631_HI_PromptReco_Collisions13_JSON_v2.txt'
config.Data.runRange = '210498-211631' # '193093-194075'
config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'RpPb_PAHighPt_Sept2013'

config.Site.storageSite = 'T2_US_Vanderbilt'
