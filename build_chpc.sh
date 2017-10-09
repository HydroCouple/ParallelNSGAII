#!/bin/csh

#SBATCH --account=u0660135
#SBATCH --partition=usu-em
#SBATCH --job-name=hydrocouple_job
#SBATCH --time=1-00:00:00
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --sockets-per-node=4
#SBATCH --cores-per-socket=8

set cur_dir = $PWD
echo "Current working directory: "$cur_dir
echo ""

set debug_or_release="release" 
set perform_make="No"
set run_hpc="No"

if ($#argv >= 2) then
  set perform_make=$argv[1]
  echo "Perform Make:" $perform_make
  set debug_or_release=$argv[2]
  echo "Make Mode: "$debug_or_release
  echo "" 
endif

if ($#argv >= 3) then
 set run_hpc=$argv[3]
endif

#load latest gcc
echo "loading gcc/4.9.2 mvapich2"
module load gcc/4.9.2 mvapich2
echo ""
echo "#==================================================================================================="

#add mpi
echo "Adding local mpi libraries to path"
setenv LD_LIBRARY_PATH "/uufs/chpc.utah.edu/sys/installdir/intel/compilers_and_libraries_2017.0.098/linux/mpi/intel64/lib:$LD_LIBRARY_PATH"
echo ""
echo "#==================================================================================================="

make -f Makefile.orig clean
make -j 8 -f Makefile.orig


if ($run_hpc =~ "No") then
    echo "no hpc..."
    mpirun -n 8 ./parallelnsga2r 0.1 ./input_data/zdt5.in
else

    set omp_threads = 1
    # Set OMP_NUM_THREADS to the same value as -c
    # with a fallback in case it isn't set.
    # SLURM_CPUS_PER_TASK is set to the value of -c, but only if -c is explicitly set
    if $?SLURM_CPUS_PER_TASK == 1 then
        set omp_threads=$SLURM_CPUS_PER_TASK
        echo $omp_threads
    endif
    
    echo "Omp number of threads: "$omp_threads

    # setenv OMP_NUM_THREADS $omp_threads

    echo "running on hpc..."
    
    srun ./parallelnsga2r 0.1 ./input_data/zdt6.in

endif