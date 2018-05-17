#!/bin/sh
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16  # 16 cores
#SBATCH --time=0-00:10:00  # 10 minutes
#SBATCH --output=my_output.txt

cp q1.c $PFSDIR/.
cd $PFSDIR
gcc q1.c -o q1 -lm -fopenmp

echo "1 core"
./q1.o 1
echo "2 cores"
./q1.o 2
echo "4 cores"
./q1.o 4
echo "6 cores"
./q1.o 6
echo "8 cores"
./q1.o 8
