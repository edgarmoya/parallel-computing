#!/bin/bash
#SBATCH -p gpu 
#SBATCH -J cuda
#SBATCH -o cuda.out
#SBATCH -e cuda.err
#SBATCH -t 0-12:00:00
#SBATCH --nodes=1
#SBATCH --gres=gpu:1
#SBATCH --tasks-per-node=1

module load CUDA
srun ./main.exe