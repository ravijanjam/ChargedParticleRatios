#!/bin/bash


DataSet=(
/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root \
/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root
)


cmsRun ppRef_v3_1_cfg.py \
	maxNumEvents=10 \
#	inputFile='/store/data/Run2015E/MinimumBias1/AOD/PromptReco-v1/000/261/397/00000/D6A5BA37-3A8E-E511-9DC8-02163E014418.root' \
	inputFile='ppRef_MinimumBias10.txt'
	outputFileName=output_10.root

count=1
for fileName in ${DataSet[*]}
do

	let count=count+1
	echo $fileName
done

