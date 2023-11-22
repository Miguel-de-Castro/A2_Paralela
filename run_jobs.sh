#!/bin/bash

# Nome do seu arquivo batchjob
batchjob="batchjob_parameter.batchjob"
program=sequencial

job_id_1=0
job_id_2=0
job_id_3=0
job_id_4=0

for nodes in $(seq 1 4); do
    # Submetendo um trabalho SLURM para execução e obtendo o ID do trabalho
    job_id_$node=$(sbatch --nodes $nodes $batchjob $program $nodes | awk '{print $4}')
    echo $job_id_$node
done

# Esperar pela conclusão de todos os trabalhos SLURM
# for job_id in "${job_ids[@]}"; do
#     # Utilizar squeue para verificar a conclusão do trabalho
#     more batchjob_parameter.batchjob.$job_id.out
#     echo ""
#     echo ""
# done