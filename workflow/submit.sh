#!/bin/bash --login

#PBS -N workflow_io
#PBS -l select=1:ncpus=1
#PBS -l walltime=0:20:0

#PBS -A z04

# Change to the directory that the job was submitted from
cd $PBS_O_WORKDIR

# Set the number of threads to 1 to ensure serial
export OMP_NUM_THREADS=1

# Run the serial executable

time ./run_benchmark.sh 1024 10
