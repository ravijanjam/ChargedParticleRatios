#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=5G
#SBATCH --time=0-06:00:00     # 15 minutes
#SBATCH --output=my1.stdout
#SBATCH --mail-user=ravi.k.janjam@vanderbilt.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name="analyzerFromCommandLine"

export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
eval `scramv1 runtime -sh`

#./analyzerFromCommandLine.sh
cmsRun pPbAnalysis_cfg.py maxNumEvents=-1
