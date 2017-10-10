/* NSGA-II routine (implementation of the 'main' function) */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <fstream>
# include <list>
# include <sys/stat.h>
# include <dlfcn.h>
# include <vector>
# include <stdio.h>
# include <algorithm>
# include "global.h"
# include "rand.h"
# include "problemdef.h"

#ifdef USE_MPI
#include <mpi.h>
#endif

#ifdef USE_OPENMP
#include <omp.h>
#endif


int nreal;
int nbin;
int nobj;
int ncon;
int popsize;
double pcross_real;
double pcross_bin;
double pmut_real;
double pmut_bin;
double eta_c;
double eta_m;
int ngen;
int nbinmut;
int nrealmut;
int nbincross;
int nrealcross;
int *nbits;
double *min_realvar;
double *max_realvar;
double *min_binvar;
double *max_binvar;
int bitlength;
int mpiProcessors = 1;
int numThreads = 1;
int procRank = 0;
std::vector<std::string> problemOptions;
problemDef problemDefinition = nullptr;
int problemIndex = 0;
std::string sharedLibraryPath;
void *fhandle = nullptr;
int currentGen = 1;
int max_nbits = 0;
int DIE_TAG = 9999;

std::vector<std::string> splitText(const std::string &text, const std::string& delim)
{
  std::vector<std::string> parsed;

  size_t start = 0;
  size_t end = text.find(delim);

  while (end != std::string::npos)
  {
    parsed.push_back(text.substr(start, end - start));
    start = end + delim.length();
    end = text.find(delim, start);
  }

  parsed.push_back(text.substr(start, end));

  return parsed;
}

std::string fileExtension(const std::string& filePath)
{
  if(filePath.find(".") != std::string::npos)
  {
    return filePath.substr(filePath.find_last_of("."));
  }
  else
  {
    return "";
  }
}

bool replace(std::string& str, const std::string& from, const std::string& to)
{
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos)
    return false;
  str.replace(start_pos, from.length(), to);
  return true;
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
  if(from.empty())
    return;

  size_t start_pos = 0;

  while((start_pos = str.find(from, start_pos)) != std::string::npos)
  {
    str.replace(start_pos, from.length(), to);
    start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
  }
}

bool fileExists(const std::string& name)
{
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0);
}


void abortNSGA(int rc = 0)
{
#ifdef USE_MPI
  printf("abort called \n");
  MPI_Abort(MPI_COMM_WORLD, rc);
#else
  exit(1);
#endif
}

void setProblemDefinitionFromIndex(int problemIndex)
{
  switch (problemIndex)
  {
    case 0:
      {
        problemDefinition = &test_problem_sch1;
      }
      break;
    case 1:
      {
        problemDefinition = &test_problem_sch2;
      }
      break;
    case 2:
      {
        problemDefinition = &test_problem_fon;
      }
      break;
    case 3:
      {
        problemDefinition = &test_problem_kur;
      }
      break;
    case 4:
      {
        problemDefinition = &test_problem_pol;
      }
      break;
    case 5:
      {
        problemDefinition = &test_problem_vnt;
      }
      break;
    case 6:
      {
        problemDefinition = &test_problem_zdt1;
      }
      break;
    case 7:
      {
        problemDefinition = &test_problem_zdt2;
      }
      break;
    case 8:
      {
        problemDefinition = &test_problem_zdt3;
      }
      break;
    case 9:
      {
        problemDefinition = &test_problem_zdt4;
      }
      break;
    case 10:
      {
        problemDefinition = &test_problem_zdt5;
      }
      break;
    case 11:
      {
        problemDefinition = &test_problem_zdt6;
      }
      break;
    case 12:
      {
        problemDefinition = &test_problem_bnh;
      }
      break;
    case 13:
      {
        problemDefinition = &test_problem_osy;
      }
      break;
    case 14:
      {
        problemDefinition = &test_problem_srn;
      }
      break;
    case 15:
      {
        problemDefinition = &test_problem_tnk;
      }
      break;
    case 16:
      {
        problemDefinition = &test_problem_ctp1;
      }
      break;
    case 17:
      {
        problemDefinition = &test_problem_ctp2;
      }
      break;
    case 18:
      {
        problemDefinition = &test_problem_ctp3;
      }
      break;
    case 19:
      {
        problemDefinition = &test_problem_ctp4;
      }
      break;
    case 20:
      {
        problemDefinition = &test_problem_ctp5;
      }
      break;
    case 21:
      {
        problemDefinition = &test_problem_ctp6;
      }
      break;
    case 22:
      {
        problemDefinition = &test_problem_ctp7;
      }
      break;
    case 23:
      {
        problemDefinition = &test_problem_ctp8;
      }
      break;
    default:
      {
        problemDefinition = &test_problem_sch1;
      }
      break;
  }
}

void setProblemDefinitionFromLibrary(const std::string& libraryPath, const std::string& functionName , void* handle)
{
  if(fileExists(libraryPath))
  {
#ifdef _WIN32 // note the underscore: without it, it's not msdn official!

#else
    handle = dlopen(libraryPath.c_str(), RTLD_LAZY);

    if(handle)
    {
      problemDefinition = nullptr;
      *reinterpret_cast<void**>(&problemDefinition) = dlsym(handle, functionName.c_str());

      if(problemDefinition == nullptr)
      {
        printf("Could not open library function: %s" , libraryPath.c_str());
        abortNSGA();
      }
    }
    else
    {
      printf("Could not open library: %s" , libraryPath.c_str());
      abortNSGA();
    }

#endif
  }
}

void closeLibrary(void* handle)
{
  if(handle != nullptr)
  {
#ifdef _WIN32 // note the underscore: without it, it's not msdn official!

#else
    dlclose(handle);
    handle = nullptr;
#endif
  }
}

int main (int argc, char **argv)
{

  bool printAllIndividuals = false;

  if (argc < 2)
  {
    printf("\n Usage ./nsga2r random_seed <inputfile>\n");
    exit(1);
  }

#ifdef USE_MPI
  {
    int allowed = 0;
    int rc = MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &allowed);
    //int rc = MPI_Init(&argc,&argv);

    if (rc != MPI_SUCCESS)
    {
      printf ("Error starting MPI program. Terminating.\n");
      abortNSGA(rc);
    }
    else
    {
      printf("MPI initialized successfully.\n");
      MPI_Comm_size(MPI_COMM_WORLD, &mpiProcessors);
      MPI_Comm_rank(MPI_COMM_WORLD, &procRank);
      int len = 0;
      char processor_name[MPI_MAX_PROCESSOR_NAME];
      MPI_Get_processor_name(processor_name, &len);
      printf("Proc %i/%i\n", procRank, mpiProcessors);
    }
  }
#else
  {
    printf("MPI is disabled\n");
  }
#endif

  //read parameters
  seed = (double)atof(argv[1]);

  if (seed<=0.0 || seed>=1.0)
  {
    printf("\n Entered seed value is wrong, seed value must be in (0,1) \n");
    abortNSGA();
  }

  if(argc > 3)
  {
    if(std::string(argv[3]) == "-p")
      printAllIndividuals = true;
  }

  std::string inputfile(argv[2]);
  std::fstream inputFStream;
  inputFStream.open(inputfile,std::fstream::in);

  std::string line;

  if(inputFStream.is_open() && std::getline(inputFStream, line))
  {
    popsize = stod(line);

    if (popsize < 1)
    {
      printf("\n population size read is : %d",popsize);
      printf("\n Wrong population size entered, hence exiting \n");
      abortNSGA();
    }

    if(std::getline(inputFStream, line))
    {
      ngen = stoi(line);

      if(ngen < 1)
      {
        printf("\n number of generations read is : %d",ngen);
        printf("\n Wrong nuber of generations entered, hence exiting \n");
        abortNSGA();
      }
    }
    else
    {
      printf("\n File Ended Prematurely \n");
      abortNSGA();
    }


    if(std::getline(inputFStream, line))
    {
      nobj = stoi(line);

      if(nobj < 1)
      {
        printf("\n number of objectives entered is : %d",nobj);
        printf("\n Wrong number of objectives entered, hence exiting \n");
        abortNSGA();
      }
    }
    else
    {
      printf("\n File Ended Prematurely \n");
      abortNSGA();
    }

    if(std::getline(inputFStream, line))
    {
      ncon = stoi(line);

      if(ncon < 0)
      {
        printf("\n number of constraints entered is : %d",ncon);
        printf("\n Wrong number of constraints enetered, hence exiting \n");
        abortNSGA();
      }
    }
    else
    {
      printf("\n File Ended Prematurely \n");
      abortNSGA();
    }

    if(std::getline(inputFStream, line))
    {
      nreal = stoi(line);

      if(nreal < 0)
      {
        printf("\n number of real variables entered is : %d",nreal);
        printf("\n Wrong number of variables entered, hence exiting \n");
        abortNSGA();
      }
    }
    else
    {
      printf("\n File Ended Prematurely \n");
      abortNSGA();
    }

    if(nreal > 0)
    {
      min_realvar = (double *)malloc(nreal*sizeof(double));
      max_realvar = (double *)malloc(nreal*sizeof(double));

      for(int i = 0 ; i < nreal; i++)
      {
        if(std::getline(inputFStream, line))
        {
          std::vector<std::string> split = splitText(line," ");

          if(split.size() == 2)
          {
            min_realvar[i] = stod(split[0]);
            max_realvar[i] = stod(split[1]);

            if (max_realvar[i] <= min_realvar[i])
            {
              printf("\n Wrong limits entered for the min and max bounds of real variable, hence exiting \n");
              abortNSGA();
            }
          }
          else
          {
            printf("\n File Ended Prematurely \n");
            abortNSGA();
          }

        }
        else
        {
          printf("\n File Ended Prematurely \n");
          abortNSGA();
        }
      }

      if(std::getline(inputFStream, line))
      {
        pcross_real = stod(line);

        if(pcross_real<0.0 || pcross_real>1.0)
        {
          printf("\n Probability of crossover entered is : %e",pcross_real);
          printf("\n Entered value of probability of crossover of real variables is out of bounds, hence exiting \n");
          abortNSGA();
        }
      }
      else
      {
        printf("\n File Ended Prematurely \n");
        abortNSGA();
      }

      if(std::getline(inputFStream, line))
      {
        pmut_real = stod(line);

        if(pmut_real < 0.0 || pmut_real > 1.0)
        {
          printf("\n Probability of mutation entered is : %e",pmut_real);
          printf("\n Entered value of probability of mutation of real variables is out of bounds, hence exiting \n");

          abortNSGA();
        }
      }
      else
      {
        printf("\n File Ended Prematurely \n");
        abortNSGA();
      }

      if(std::getline(inputFStream, line))
      {
        eta_c = stod(line);

        if(eta_c <= 0.0)
        {
          printf("\n The value entered is : %e",eta_c);
          printf("\n Wrong value of distribution index for crossover entered, hence exiting \n");

          abortNSGA();
        }
      }
      else
      {
        printf("\n File Ended Prematurely \n");
        abortNSGA();
      }

      if(std::getline(inputFStream, line))
      {
        eta_m = stod(line);

        if(eta_m <= 0.0)
        {
          printf("\n The value entered is : %e",eta_m);
          printf("\n Wrong value of distribution index for mutation entered, hence exiting \n");

          abortNSGA();
        }
      }
      else
      {
        printf("\n File Ended Prematurely \n");
        abortNSGA();
      }

    }

    if(std::getline(inputFStream, line))
    {
      nbin = stod(line);

      if(nbin < 0.0)
      {
        printf ("\n number of binary variables entered is : %d",nbin);
        printf ("\n Wrong number of binary variables entered, hence exiting \n");
        abortNSGA();
      }
    }
    else
    {
      printf("\n File Ended Prematurely \n");
      abortNSGA();
    }

    if(nbin > 0)
    {
      nbits = (int *)malloc(nbin*sizeof(int));
      min_binvar = (double *)malloc(nbin*sizeof(double));
      max_binvar = (double *)malloc(nbin*sizeof(double));

      for (int i = 0; i < nbin; i++)
      {
        if(std::getline(inputFStream, line))
        {
          std::vector<std::string> split = splitText(line," ");

          if(split.size() == 3)
          {
            int nbitssize = stoi(split[0]);
            nbits[i] = nbitssize;

            min_binvar[i] = stod(split[1]);
            max_binvar[i] = stod(split[2]);

            if (max_binvar[i] <= min_binvar[i])
            {
              printf("\n Wrong limits entered for the min and max bounds of binary variable, hence exiting \n");
              abortNSGA();
            }
          }
          else
          {
            printf("\n File Ended Prematurely \n");
            abortNSGA();
          }
        }
      }

      max_nbits = *std::max_element(nbits,nbits);

      if(std::getline(inputFStream, line))
      {
        pcross_bin = stod(line);

        if(pcross_bin< 0.0 || pcross_bin > 1.0)
        {
          printf("\n Probability of crossover entered is : %e",pcross_bin);
          printf("\n Entered value of probability of crossover of binary variables is out of bounds, hence exiting \n");
          abortNSGA();
        }
      }
      else
      {
        printf("\n File Ended Prematurely \n");
        abortNSGA();
      }


      if(std::getline(inputFStream, line))
      {
        pmut_bin = stod(line);

        if(pmut_bin<0.0 || pmut_bin>1.0)
        {
          printf("\n Probability of mutation entered is : %e",pmut_bin);
          printf("\n Entered value of probability  of mutation of binary variables is out of bounds, hence exiting \n");
          abortNSGA();
        }
      }
      else
      {
        printf("\n File Ended Prematurely \n");
        abortNSGA();
      }
    }

    if(std::getline(inputFStream,line))
    {
      std::vector<std::string> split = splitText(line," ");

      if(split.size() >= 2)
      {
        if(split[0] == "-t")
        {
          problemIndex =  stoi(split[1]);
          setProblemDefinitionFromIndex(problemIndex);
        }
        else if(split[0] == "-f" && split.size() > 2)
        {
          std::string tempSharedLibraryPath = split[1];

          if(fileExists(tempSharedLibraryPath))
          {
            std::string functionName = split[2];
            setProblemDefinitionFromLibrary(tempSharedLibraryPath,functionName,fhandle);
            sharedLibraryPath = tempSharedLibraryPath;
            problemIndex = -1;
          }
          else
          {
            printf ("Could not open inputfile. Terminating.\n");
            abortNSGA();
          }
        }
      }
    }

    problemOptions.clear();

    while (std::getline(inputFStream,line))
    {
//      printf("%s\n", line.c_str());
      problemOptions.push_back(line);
    }

    inputFStream.close();
  }
  else
  {
    printf ("Could not open inputfile. Terminating.\n");
    abortNSGA();
  }

  if(procRank == 0)
  {

    population *parent_pop;
    population *child_pop;
    population *mixed_pop;

    std::string ext = fileExtension(inputfile);
    std::string tempFile = inputfile; replace(tempFile,ext,"_initial_pop.out");
    FILE *fpt1 = fopen(tempFile.c_str(),"w");

    tempFile = inputfile; replace(tempFile,ext,"_final_pop.out");
    FILE *fpt2 = fopen(tempFile.c_str(),"w");

    tempFile = inputfile; replace(tempFile,ext,"_best_pop.out");
    FILE *fpt3 = fopen(tempFile.c_str(),"w");

    FILE *fpt4 = nullptr;

    if(printAllIndividuals)
    {
      tempFile = inputfile; replace(tempFile,ext,"_all_pop.out");
      fpt4 = fopen(tempFile.c_str(),"w");
    }

    tempFile = inputfile; replace(tempFile,ext,"_params.out");
    FILE *fpt5 = fopen(tempFile.c_str(),"w");

    fprintf(fpt1,"# This file contains the data of initial population\n");
    fprintf(fpt2,"# This file contains the data of final population\n");
    fprintf(fpt3,"# This file contains the data of final feasible population (if found)\n");

    if(printAllIndividuals)
      fprintf(fpt4,"# This file contains the data of all generations\n");

    fprintf(fpt5,"# This file contains information about inputs as read by the program\n");


    printf("\nInput data successfully read, now performing initialization\n");
    fprintf(fpt5,"\n Population size = %d",popsize);
    fprintf(fpt5,"\n Number of generations = %d",ngen);
    fprintf(fpt5,"\n Number of objective functions = %d",nobj);
    fprintf(fpt5,"\n Number of constraints = %d",ncon);
    fprintf(fpt5,"\n Number of real variables = %d",nreal);

    if (nreal > 0)
    {
      for (int i = 0; i < nreal; i++)
      {
        fprintf(fpt5,"\n Lower limit of real variable %d = %e",i+1,min_realvar[i]);
        fprintf(fpt5,"\n Upper limit of real variable %d = %e",i+1,max_realvar[i]);
      }

      fprintf(fpt5,"\n Probability of crossover of real variable = %e",pcross_real);
      fprintf(fpt5,"\n Probability of mutation of real variable = %e",pmut_real);
      fprintf(fpt5,"\n Distribution index for crossover = %e",eta_c);
      fprintf(fpt5,"\n Distribution index for mutation = %e",eta_m);

    }

    fprintf(fpt5,"\n Number of binary variables = %d",nbin);

    if (nbin > 0)
    {
      for (int  i= 0; i < nbin ; i++)
      {
        fprintf(fpt5,"\n Number of bits for binary variable %d = %d",i+1,nbits[i]);
        fprintf(fpt5,"\n Lower limit of binary variable %d = %e",i+1,min_binvar[i]);
        fprintf(fpt5,"\n Upper limit of binary variable %d = %e",i+1,max_binvar[i]);
      }

      fprintf(fpt5,"\n Probability of crossover of binary variable = %e",pcross_bin);
      fprintf(fpt5,"\n Probability of mutation of binary variable = %e",pmut_bin);
    }

    fprintf(fpt5,"\n Seed for random number generator = %e",seed);

    bitlength = 0;

    fprintf(fpt1,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nobj,ncon,nreal,bitlength);
    fprintf(fpt2,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nobj,ncon,nreal,bitlength);
    fprintf(fpt3,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nobj,ncon,nreal,bitlength);

    if(printAllIndividuals)
      fprintf(fpt4,"# of objectives = %d, # of constraints = %d, # of real_var = %d, # of bits of bin_var = %d, constr_violation, rank, crowding_distance\n",nobj,ncon,nreal,bitlength);

    nbinmut = 0;
    nrealmut = 0;
    nbincross = 0;
    nrealcross = 0;

    parent_pop = (population *)malloc(sizeof(population));
    child_pop = (population *)malloc(sizeof(population));
    mixed_pop = (population *)malloc(sizeof(population));

    allocate_memory_pop (parent_pop, popsize);
    allocate_memory_pop (child_pop, popsize);
    allocate_memory_pop (mixed_pop, 2*popsize);

    randomize();

    initialize_pop (parent_pop);

    printf("\nInitialization done, now computing first generation\n");

    decode_pop(parent_pop);

    evaluate_pop (parent_pop);

    assign_rank_and_crowding_distance (parent_pop);

    report_pop (parent_pop, fpt1);

    if(printAllIndividuals)
    {
      fprintf(fpt4,"# gen = 1\n");
      report_pop(parent_pop, fpt4);
    }

    printf("\n gen = 1 \n");

    fflush(stdout);
    fflush(fpt1);
    fflush(fpt2);
    fflush(fpt3);
    fflush(fpt4);
    fflush(fpt5);

    for (int i = 2; i <= ngen; i++)
    {
      currentGen = i;
      selection (parent_pop, child_pop);
      mutation_pop (child_pop);
      decode_pop(child_pop);
      evaluate_pop(child_pop);
      merge (parent_pop, child_pop, mixed_pop);
      fill_nondominated_sort (mixed_pop, parent_pop);
      /* Comment following three lines if information for all
        generations is not desired, it will speed up the execution */

      if(printAllIndividuals)
      {
        fprintf(fpt4,"# gen = %d\n",i);
        report_pop(parent_pop,fpt4);
        //        fflush(fpt4);
      }

      printf("\n gen = %d\n",i);
    }

    printf("\n Generations finished, now reporting solutions\n");
    report_pop(parent_pop,fpt2);
    report_feasible(parent_pop,fpt3);

    if (nreal!=0)
    {
      fprintf(fpt5,"\n Number of crossover of real variable = %d",nrealcross);
      fprintf(fpt5,"\n Number of mutation of real variable = %d",nrealmut);
    }

    if (nbin!=0)
    {
      fprintf(fpt5,"\n Number of crossover of binary variable = %d",nbincross);
      fprintf(fpt5,"\n Number of mutation of binary variable = %d",nbinmut);
    }

    fflush(stdout);

    fflush(fpt1);
    fflush(fpt2);
    fflush(fpt3);
    if(printAllIndividuals)
      fflush(fpt4);
    fflush(fpt5);

    fclose(fpt1);
    fclose(fpt2);
    fclose(fpt3);
    if(printAllIndividuals)
      fclose(fpt4);
    fclose(fpt5);


    if (nbin!=0)
    {
      for (int i = 0; i < nbin; i++)
      {
        bitlength += nbits[i];
      }
    }

    if (nreal!=0)
    {
      free (min_realvar);
      free (max_realvar);
    }

    if (nbin!=0)
    {
      free (min_binvar);
      free (max_binvar);
      free (nbits);
    }

    deallocate_memory_pop (parent_pop, popsize);
    deallocate_memory_pop (child_pop, popsize);
    deallocate_memory_pop (mixed_pop, 2*popsize);

    free (parent_pop);
    free (child_pop);
    free (mixed_pop);

    if(fhandle)
    {
      closeLibrary(fhandle);
    }
  }
#ifdef USE_MPI
  else
  {
    MPI_Status status;

    int result = 0;
    int dataSize  = 0;

    while((result = MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status)) == MPI_SUCCESS && status.MPI_TAG != DIE_TAG &&
          (result = MPI_Get_count(&status,MPI_DOUBLE, &dataSize)) == MPI_SUCCESS && dataSize > 0)
    {
      double* data = new double[dataSize];

      result = MPI_Recv(data, dataSize, MPI_DOUBLE, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

      if(result == MPI_SUCCESS)
      {
        mpi_recieve_inds_from_master(data, dataSize);
      }

      delete[] data;
    }

    switch (result)
    {
      case MPI_ERR_COMM:
        {
          printf("Invalid communicator. A common error is to use a null communicator in a call (not even allowed in MPI_Comm_rank).\n");
          abortNSGA();
        }
        break;
      case MPI_ERR_TAG:
        {
          printf("Invalid tag argument. Tags must be non-negative; tags in a receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be MPI_ANY_TAG. The largest tag value is available through the the attribute MPI_TAG_UB.\n");
          abortNSGA();
        }
        break;
      case MPI_ERR_RANK:
        {
          printf("Invalid source or destination rank. Ranks must be between zero and the size of the communicator minus one; ranks in a receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be MPI_ANY_SOURCE.\n");
          abortNSGA();
        }
        break;
    }
  }

  if(procRank == 0 && mpiProcessors > 1)
  {
    printf("kill all children\n");

    for(int i = 1; i < mpiProcessors; i++)
    {
      double test = 0;
      MPI_Send(&test,1,MPI_DOUBLE,i,DIE_TAG, MPI_COMM_WORLD);
    }
  }

  printf("Finalizing Proc: % i\n", procRank);

  MPI_Finalize();

  printf("Finalized Proc: % i\n", procRank);

  return 0;

#endif

}
