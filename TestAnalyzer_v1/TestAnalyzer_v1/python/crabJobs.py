from CRABClient.UserUtilities import config
config = config()

config.General.requestName = 'tutorial_Data_analysis_test5'
config.General.workArea = 'crab_projects'

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = 'pset_tutorial_analysis.py'

config.Data.inputDataset = '/SingleMu/Run2012B-13Jul2012-v1/AOD'
config.Data.inputDBS = 'global'
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 20
config.Data.lumiMask = 'https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions12/8TeV/Prompt/Cert_190456-208686_8TeV_PromptReco_Collisions12_JSON.txt'
config.Data.runRange = '193093-193999' # '193093-194075'
config.Data.outLFN = '/store/user/janjamrk' # or '/store/group/<subdir>'
config.Data.publication = True
config.Data.publishDataName = 'CRAB3_tutorial_Data_analysis_test5'

config.Site.storageSite = 'T2_US_Vanderbilt'
