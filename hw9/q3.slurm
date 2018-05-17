#!/bin/sh
#SBATCH --nodes=1
#SBATCH --cpus-per-task=16  # 16 cores
#SBATCH --time=0-00:10:00  # 10 minutes
#SBATCH --output=my_output.txt

cp q3.c $PFSDIR/.
cd $PFSDIR
gcc q3.c -o q1 -lm -lrt -pthread

echo "1 core"
./q3.o 1
echo "2 cores"
./q3.o 2
echo "4 cores"
./q3.o 4
echo "6 cores"
./q3.o 6
echo "8 cores"
./q3.o 8
