from CRABClient.UserUtilities import config, getUsernameFromSiteDB
config = config()

''' timeStamp the outputfile to get a unique folder name each time job is submitted '''
import datetime, time

ts = time.time()
timeLabel = datetime.datetime.fromtimestamp(ts).strftime('%Y-%m-%d-%H%M%S')

timeStampedFolderName = "ppRefAnalyzer" + "info1" + timeLabel 

''' ==================================== '''

config.General.requestName = timeStampedFolderName
config.General.workArea = 'ppRef_crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = True 

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'ppRef_v5_cfg.py'

config.Data.inputDataset = '/MinimumBias10/Run2015E-PromptReco-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions15/5TeV/Cert_262168-262172_5TeV_PromptReco_Collisions15_25ns_LOWPU_JSON.txt'
config.Data.runRange = '262168-262172' # '193093-194075'
config.Data.outLFNDirBase = '/store/user/%s/' % (getUsernameFromSiteDB())
config.Data.publication = True
config.Data.outputDatasetTag = 'ppRef_Spectra'

config.Site.storageSite = 'T2_US_Vanderbilt'