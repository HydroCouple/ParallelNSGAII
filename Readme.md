Parallel NSGAII
========================================================================
This code has been modified from the original NSGA-II code 
developed by Kalyanmoy Deb so that it can be executed in parallel
on HPC systems. It uses OpenMP for fine scale parallelism and MPI for coarse scale parallelism.
Additionally, a user can specify an external shared library to be used in calculating the values of the objective functions and constraints


About the Algorithm
--------------------------------------------------------------------------
NSGA-II: Non-dominated Sorting Genetic Algorithm - II

Please refer to the following paper for details about the algorithm:

Authors: Dr. Kalyanmoy Deb, Sameer Agrawal, Amrit Pratap, T Meyarivan
Paper Title: A Fast and Elitist multi-objective Genetic Algorithm: NSGA-II
Journal: IEEE Transactions on Evolutionary Computation (IEEE-TEC)
Year: 2002
Volume: 6
Number: 2
Pages: 182-197

The original source code was developed by [Dr. Kalyanmoy Deb](http://www.iitk.ac.in/kangal

How to compile and run the program
---------------------------------------------------------------------------
Makefile has been provided for compiling the program on linux (and unix-like)
systems. Edit the Makefile to suit your need. By default, provided Makefile
attempts to compile and link all the existing source files into one single
executable. Comment out the USE_MPI option to disable MPI. Comment out the USE_OPENMP option to disable OpenMP.

Name of the executable produced is: parallelnsga2r

To run the program type the following:
    
    mpirun -n x ./parallelnsga2r random_seed -f <inp_file.in>


where x is the number of MPI processes to use and random_seed is a real number in (0,1) which is used as a seed for random number generator. 

About the input parameters
---------------------------------------------------------------------------
1. popsize: This variable stores the population size (a multiple of 4)
2. ngen: Number of generations
3. nobj: Number of objectives
4. ncon: Number of constraints
5. nreal: Number of real variables
6. min_realvar[i]: minimum value of i^{th} real variable
7. max_realvar[i]: maximum value of i^{th} real variable
9. pcross_real: probability of crossover of real variable
10. pmut_real: probability of mutation of real variable It is recommended each decision
11. variable is mutated with a probability of 1 / L, where L is the number of decision
12. variables. This results in one mutation per offspring on average.
13. eta_c: distribution index for real variable SBX crossover
14. eta_m: distribution index for real variable polynomial mutation
15. nbin: number of binary variables
16. nbits[i]: number of bits for i^{th} binary variable
17. min_binvar[i]: minimum value of i^{th} binary variable
19. max_binvar[i]: maximum value of i^{th} binary variable
20. pcross_bin: probability of crossover for binary variable
21. pmut_bin: probability of mutation for binary variable
22. problem definition: *-t x* for one of the test problems or -f <shared_library.in> funcname for a custom problem. Wheere x in is the index for the test problem and  <shared_library.in> is the path to the shared library, and funcname is the name of the function for the problem definition.
23. remaining lines: will be read as an array of strings to passed as an argument to the problem definition

About the output files
---------------------------------------------------------------------------
* initial_pop.out: This file contains all the information about initial population.
* final_pop.out: This file contains the data of final population.
* all_pop.out: This file containts the data of populations at all generations.
* best_pop.out: This file contains the best solutions obtained at the end of simulation run.
* params.out: This file contains the information about input parameters as read by the program.

Defining the Test Problem
---------------------------------------------------------------------------
Edit the source file problemdef.c to define your test problem. Some sample
problems (24 test problems from Dr. Deb's book - Multi-Objective Optimization
using Evolutionary Algorithms) have been provided as examples to guide you
define your own objective and constraint functions. You can also link other
source files with the code depending on your need.
Following points are to be kept in mind while writing objective and constraint
functions.
1. The code has been written for minimization of objectives (min f_i). If you want to
maximize a function, you may use negetive of the function value as the objective value.
2. A solution is said to be feasible if it does not violate any of the constraints.
Constraint functions should evaluate to a quantity greater than or equal to zero
(g_j >= 0), if the solution has to be feasible. A negetive value of constraint means,
it is being violated.
3. If there are more than one constraints, it is advisable (though not mandatory)
to normalize the constraint values by either reformulating them or dividing them
by a positive non-zero constant.

About the files
---------------------------------------------------------------------------
    global.h: Header file containing declaration of global variables and functions
    rand.h: Header file containing declaration of variables and functions for random number generator
    allocate.c: Memory allocation and deallocation routines
    auxiliary.c: auxiliary routines (not part of the algorithm)
    crossover.c: Routines for real and binary crossover
    crowddist.c: Crowding distance assignment routines
    decode.c: Routine to decode binary variables
    dominance.c: Routine to perofrm non-domination checking
    eval.cpp: Routine to evaluate constraint violation
    fillnds.c: Non-dominated sorting based selection
    initialize.c: Routine to perform random initialization to population members
    list.c: A custom doubly linked list implementation
    merge.c: Routine to merge two population into one larger population
    mutation.c: Routines for real and binary mutation
    parallelnsga2r.cpp: Implementation of main function and the NSGA-II framework
    problemdef.c: Test problem definitions
    rand.c: Random number generator related routines
    rank.c: Rank assignment routines
    report.c: Routine to write the population information in a file
    sort.c: Randomized quick sort implementation
    tourselect.c: Tournament selection routine

---------------------------------------------------------------------------
Contact [me](caleb.buahin@usu.edu) with questions or comments.


[highlight.js]: http://softwaremaniacs.org/soft/highlight/en/
