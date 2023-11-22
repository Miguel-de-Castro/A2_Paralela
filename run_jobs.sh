#!/bin/bash

# Nome do seu arquivo batchjob
batchjob="batchjob_parameter.batchjob"
program=sequencial

# for nodes in $(seq 1 4); do
#     # Submetendo um trabalho SLURM para execução e obtendo o ID do trabalho
#     job_id_$node=$(sbatch --nodes $nodes $batchjob $program $nodes | awk '{print $4}')
#     echo $job_id_$node
# done

# Esperar pela conclusão de todos os trabalhos SLURM
# for job_id in "${job_ids[@]}"; do
#     # Utilizar squeue para verificar a conclusão do trabalho
#     more batchjob_parameter.batchjob.$job_id.out
#     echo ""
#     echo ""
# done

sbatch --nodes 1 $batchjob $program 1
sbatch --nodes 2 $batchjob $program 2
sbatch --nodes 3 $batchjob $program 3
sbatch --nodes 4 $batchjob $program 4