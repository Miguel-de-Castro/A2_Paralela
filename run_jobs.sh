#!/bin/bash

# Nome do seu arquivo batchjob
batchjob="batchjob_parameter_par.batchjob"
program=paralelo

sbatch --nodes 1 $batchjob $program 1
sbatch --nodes 2 $batchjob $program 2
sbatch --nodes 3 $batchjob $program 3
sbatch --nodes 4 $batchjob $program 4