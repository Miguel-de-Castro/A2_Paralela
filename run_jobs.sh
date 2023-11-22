#!/bin/bash

# Nome do seu arquivo batchjob
batchjob="batchjob_parameter.batchjob"
program=sequencial

> output_$program.txt

# Loop para variar o número de nodos de 1 a 4
for nodes in $(seq 1 4); do
  # Submetendo um trabalho SLURM para execução
  sbatch --nodes $nodes $batchjob $program $nodes
done
