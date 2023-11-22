#!/bin/bash

# Nome do seu arquivo batchjob
batchjob="batchjob_parameter.batchjob"
program=sequencial

for nodes in $(seq 1 4); do
    # Submetendo um trabalho SLURM para execução e obtendo o ID do trabalho
    job_id=$(sbatch --nodes $nodes $batchjob $program $nodes | awk '{print $4}')
    # Utilizar squeue para verificar a conclusão do trabalho
    while squeue --noheader -j $job_id &>/dev/null; do
        sleep 1
    done
    more batchjob_parameter.batchjob.$job_id.out
    echo ""
done

# # Esperar pela conclusão de todos os trabalhos SLURM
# for job_id in "${job_ids[@]}"; do
#   # Utilizar squeue para verificar a conclusão do trabalho
#     more batchjob_parameter.batchjob.$job_id.out
#     echo ""
#     echo ""
# done