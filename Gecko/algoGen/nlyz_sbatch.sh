#!/bin/bash
#SBATCH -n 1                    # Number of cores
#SBATCH -N 1                    # Ensure that all cores are on one machine
#####SBATCH -t 1-10:00              # Runtime in D-HH:MM
##SBATCH -t 99-10:00              # Runtime in D-HH:MM
#SBATCH -t 9-10:00              # Runtime in D-HH:MM
#SBATCH --mem=60000
#SBATCH -o plot_analysis_%j.out      # File to which STDOUT will be written
#SBATCH -e plot_analysis_%j.err      # File to which STDERR will be written


#module load system/Python-3.6.3

#module load WitteLab python3/3.9.1

python3 -m pip show scikit-learn

echo $1, $2, $3, $4, $5

python3 plotter_for_eachhistorylog.py $1 $2 $3 $4 $5 $6 > $1/nlyzlog 2>&1

