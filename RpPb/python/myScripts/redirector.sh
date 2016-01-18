#!/bin/bash

#$A=/store/data/Run2015E/MinimumBias11/AOD/PromptReco-v1/000/261/397/00000/0033CCDB-3B8E-E511-853E-02163E0146B8.root

#edmDumpEventContent root://xrootd-cms.infn.it/$A >> Run2015E_MinimumBias11_AOD_PromptReco_v1

for i in {1..12};do das_client --query="file dataset=/MinimumBias$i/Run2015E-PromptReco-v1/AOD" --limit=1; done | grep store | edmDumpEventContent 
