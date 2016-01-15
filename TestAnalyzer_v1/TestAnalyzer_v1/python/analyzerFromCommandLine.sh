#!/bin/bash


DataSet=(
/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root \
/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root
)


cmsRun ppRef_v3_1_cfg.py \
	maxNumEvents=-1 \
#	inputFiles=/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root \
	inputFiles=ppRef_MinimumBias10.txt
#	inputFiles=ppRef_MinimumBias10.txt
	outputFile=ppRefMinBias1.root

count=1
for fileName in ${DataSet[*]}
do

	let count=count+1
	echo $fileName
done

