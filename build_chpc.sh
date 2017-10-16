#!/bin/csh

#SBATCH --account=usu-em
#SBATCH --partition=ember-freecycle
#SBATCH --job-name=swmm_2_32
#SBATCH --time=1-00:00:00
#SBATCH --ntasks=4
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=12

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
echo "loading intel/2017.4.196 mvapich2"
module load intel/2017.4.196 mvapich2

module load ddt

#echo "loading igcc/4.9.2 mvapich"
#module load gcc/4.9.2 mvapich2
echo ""
echo "#==================================================================================================="

#add mpi
echo "Adding local mpi libraries to path"
setenv LD_LIBRARY_PATH "/uufs/chpc.utah.edu/sys/installdir/intel/compilers_and_libraries_2017.0.098/linux/mpi/intel64/lib:$LD_LIBRARY_PATH"
echo ""
echo "#==================================================================================================="


if ($perform_make =~ "Yes") then


    echo ""
    echo "#==================================================================================================="
    echo "Making SWMMParallelNSGAIILib"
    echo "#==================================================================================================="
    echo ""
    cd ../SWMMParallelNSGAIILib/
    make -f Makefile.chpc clean
    rm -rf ./lib/libSWMMParallelNSGAIILib*
    #find . -name "*gen*ind*" -type f -delete
    make -j 8 -f Makefile.chpc all
    

    echo ""
    echo "#==================================================================================================="
    echo "Making SWMM"
    echo "#==================================================================================================="
    echo ""
    cd ../SWMM_Source
    make clean
    rm -rf swmm
    make -j 8


    echo ""
    echo "#==================================================================================================="
    echo "Making ParallelNSGAII"
    echo "#==================================================================================================="
    echo ""
    cd ../ParallelNSGAII
    make -f Makefile.chpc clean
    rm -rf parallelnsga2r
    make -j 8 -f Makefile.chpc
    cd ../RunConfig

endif

if ($run_hpc =~ "No") then
    echo "no hpc..."
    #gdb ../ParallelNSGAII/parallelnsga2r
    ddt ../ParallelNSGAII/parallelnsga2r 0.1 ../SWMMParallelNSGAIILib/test_problem/swmm_opt.in
    #mpirun -np 4 -genv MV2_ENABLE_AFINITY 0 -genv KMP_AFFINITY warnings,compact ../ParallelNSGAII/parallelnsga2r 0.1 ../SWMMParallelNSGAIILib/test_problem/testing/swmm_opt_v1.in
    #mpirun -n 1 ../ParallelNSGAII/parallelnsga2r 0.1 ./input_data/zdt5.in
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

    setenv OMP_NUM_THREADS $omp_threads

    echo "running on hpc..."
    
    mpirun -np $SLURM_NTASKS -genv MV2_ENABLE_AFINITY 0 -genv KMP_AFFINITY warnings,compact ../ParallelNSGAII/parallelnsga2r 0.1 ../SWMMParallelNSGAIILib/test_problem/swmm_opt.in

endif