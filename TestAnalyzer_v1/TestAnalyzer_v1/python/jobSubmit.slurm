#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=1G
#SBATCH --time=0-01:00:00     # 15 minutes
#SBATCH --output=my.stdout
#SBATCH --mail-user=ravi.k.janjam@vanderbilt.edu
#SBATCH --mail-type=ALL
#SBATCH --job-name="just_a_test"

eval `scramv1 runtime -sh`
