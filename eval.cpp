/* Routine for evaluating population members  */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <vector>
# include <unistd.h>
# include "rand.h"
# include "problemdef.h"

#ifdef USE_OPENMP
# include <omp.h>
#endif

/* Routine to evaluate objective function values and constraints for a population */
void evaluate_pop (population *pop)
{
#ifdef USE_MPI
  {
    div_t inds_per_mpi_task;
    inds_per_mpi_task = div(popsize,mpiProcessors);

    if(inds_per_mpi_task.quot)
    {
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
      for(int p = 1; p < mpiProcessors; p++)
      {
        int start = p * inds_per_mpi_task.quot;
        mpi_send_inds_to_worker(pop, start, inds_per_mpi_task.quot, p);
      }

#ifdef USE_OPENMP
#pragma omp parallel for
#endif
      for(int i = 0; i < inds_per_mpi_task.quot + inds_per_mpi_task.rem; i++)
      {
        if(i < inds_per_mpi_task.quot)
        {
          evaluate_ind(i, &pop->ind[i]);
        }
        else
        {
          int start = (mpiProcessors * inds_per_mpi_task.quot) + (i - inds_per_mpi_task.quot);
          evaluate_ind(start, &pop->ind[start]);
        }
      }
    }

    if(inds_per_mpi_task.quot)
    {
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
      for(int p = 1; p < mpiProcessors; p++)
      {
        mpi_recieve_inds_from_worker(pop, p);
      }
    }
  }
#else
  {
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
    for (int i = 0; i < popsize; i++)
    {
      evaluate_ind (i, &(pop->ind[i]));
    }
  }
#endif

  return;
}

/* Routine to evaluate objective function values and constraints for an individual */
void evaluate_ind (int index, individual *ind)
{
  int j;

  problemDefinition(currentGen, index, nreal, ind->xreal, nbin, ind->xbin, nbits, ind->gene, nobj, ind->obj, ncon, ind->constr, problemOptions);

  if (ncon == 0)
  {
    ind->constr_violation = 0.0;
  }
  else
  {
    ind->constr_violation = 0.0;

    if(ncon > 0)
    {
      for (j= 0; j < ncon; j++)
      {
        if (ind->constr[j] < 0.0)
        {
          ind->constr_violation += ind->constr[j];
        }
      }
    }
  }

  return;
}


#ifdef USE_MPI

void mpi_send_inds_to_worker(population *pop, int start, int length, int mpiProcessor)
{
#ifdef USE_MPI
  {
    std::vector<double> data; data.reserve(length * (nreal + nbin * max_nbits + 1) + 2);
    data.push_back(currentGen);
    data.push_back(length);

    for(int i = start; i < (start + length); i++)
    {
      data.push_back(i);
      individual *ind = &(pop->ind[i]);
      mpi_serialize_ind_from_master(ind, data);
    }

    printf("Sending [%i] => [%i] |  No Ind: %i | Data Size: %i\n", procRank, mpiProcessor, length, (int)data.size());

    MPI_Send(&data[0] , data.size() , MPI_DOUBLE, mpiProcessor, 0, MPI_COMM_WORLD);
  }
#else
  {
#ifdef USE_OPENMP
#pragma omp parallel for
#endif
    for(int i = start; i < (start + length); i++)
    {
      individual *ind = &(pop->ind[i]);
      evaluate_ind(ind);
    }
  }

#endif

}

void mpi_serialize_ind_from_master(individual *ind, std::vector<double> &data)
{
  if(nreal > 0)
  {
    for(int i = 0; i < nreal; i++)
    {
      data.push_back(ind->xreal[i]);
    }
  }

  if(nbin > 0)
  {
    for(int i = 0 ; i < nbin ; i++)
    {
      data.push_back(ind->xbin[i]);

      for(int j = 0 ; j < nbits[i]; j++)
      {
        data.push_back(ind->gene[i][j]);
      }
    }
  }
}

void mpi_recieve_inds_from_master(double *values, int size)
{
  currentGen = values[0];
  int numIndividuals = values[1];
  int currPos = 2;


  printf("[%i] => [%i] Recieving |  No Ind: %i | Data Size: %i\n", 0 , procRank, numIndividuals, size);

  individual *inds = (individual*)malloc(numIndividuals*sizeof(individual));
  int *indexes = new int[numIndividuals];


  for(int i = 0; i < numIndividuals; i++)
  {
    allocate_memory_ind(&inds[i]);
    indexes[i] = values[currPos]; currPos++;
    mpi_desrialize_ind_from_master(&inds[i],values,currPos);
  }

#ifdef USE_OPENMP
#pragma omp parallel for
#endif
  for(int i = 0; i < numIndividuals; i++)
  {
    evaluate_ind(indexes[i], &inds[i]);
  }

  std::vector<double> data; data.reserve(numIndividuals * (nobj + 1 + ncon));
  data.push_back(numIndividuals);

  for(int i = 0; i < numIndividuals; i++)
  {
    data.push_back(indexes[i]);
    mpi_serialize_ind_from_worker(&inds[i], data);
  }

  printf("Sending [%i] => [%i] |  No Ind: %i | Data Size: %i\n", procRank, 0, numIndividuals, size);

  MPI_Send(&data[0],data.size(),MPI_DOUBLE,0,0,MPI_COMM_WORLD);


  for(int i = 0; i < numIndividuals; i++)
  {
    deallocate_memory_ind(&inds[i]);
  }

  delete[] indexes;
  free(inds);
}

void mpi_desrialize_ind_from_master(individual *ind, double *values, int &currentPos)
{
  if(nreal > 0)
  {
    for(int i = 0; i < nreal; i++)
    {
      ind->xreal[i] = values[currentPos]; currentPos++;
    }
  }

  if(nbin > 0)
  {
    for(int i = 0 ; i < nbin ; i++)
    {
      ind->xbin[i] = values[currentPos]; currentPos++;

      for(int j = 0 ; j < nbits[i]; j++)
      {
        ind->gene[i][j] = values[currentPos]; currentPos++;
      }
    }
  }
}

void mpi_serialize_ind_from_worker(individual *ind, std::vector<double> &data)
{
  if(nobj > 0)
  {
    for(int i = 0; i < nobj; i++)
    {
      data.push_back(ind->obj[i]);
    }
  }

  if(ncon > 0)
  {
    for(int i = 0; i < ncon; i++)
    {
      data.push_back(ind->constr[i]);
    }
  }
}

void mpi_recieve_inds_from_worker(population *pop, int mpiProcessor)
{

  MPI_Status status;

  int result = MPI_Probe(mpiProcessor, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

  switch (result)
  {
    case MPI_SUCCESS:
      {
        int dataSize  = 0;
        MPI_Get_count(&status,MPI_DOUBLE, &dataSize);

        if(dataSize > 0)
        {
          double* data = new double[dataSize];

          result = MPI_Recv(data, dataSize, MPI_DOUBLE, mpiProcessor, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

          if(result == MPI_SUCCESS)
          {
            int indLength = data[0];
            int currPos = 1;

            printf("[%i] => [%i] Recieving |  No Ind: %i | Data Size: %i\n", mpiProcessor, procRank,indLength, dataSize);

            for(int i = 0; i < indLength; i++)
            {
              int index = data[currPos]; currPos++;
              mpi_desrialize_ind_from_worker(&pop->ind[index], data,currPos);
            }
          }

          printf("deleting data from worker...\n");
          delete[] data;
        }
      }
      break;
    case MPI_ERR_COMM:
      {
        printf("Invalid communicator. A common error is to use a null communicator in a call (not even allowed in MPI_Comm_rank).\n");
        MPI_Abort(MPI_COMM_WORLD, result);
      }
      break;
    case MPI_ERR_TAG:
      {
        printf("Invalid tag argument. Tags must be non-negative; tags in a receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be MPI_ANY_TAG. The largest tag value is available through the the attribute MPI_TAG_UB.\n");
        MPI_Abort(MPI_COMM_WORLD, result);
      }
      break;
    case MPI_ERR_RANK:
      {
        printf("Invalid source or destination rank. Ranks must be between zero and the size of the communicator minus one; ranks in a receive (MPI_Recv, MPI_Irecv, MPI_Sendrecv, etc.) may also be MPI_ANY_SOURCE.\n");
        MPI_Abort(MPI_COMM_WORLD, result);
      }
      break;
  }
}

void mpi_desrialize_ind_from_worker(individual *ind, double *values, int &currentPos)
{
  if(nobj > 0)
  {
    for(int i = 0; i < nobj; i++)
    {
      ind->obj[i] = values[currentPos]; currentPos++;
    }
  }

  if(ncon > 0)
  {
    for(int i = 0; i < ncon; i++)
    {
      ind->constr[i] = values[currentPos]; currentPos++;
    }
  }

  if (ncon == 0)
  {
    ind->constr_violation = 0.0;
  }
  else if(ncon > 0)
  {
    ind->constr_violation = 0.0;

    for (int j = 0; j < ncon; j++)
    {
      if (ind->constr[j]<0.0)
      {
        ind->constr_violation += ind->constr[j];
      }
    }
  }
}

#endif
