/* Memory allocation and deallocation routines */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>

# include "global.h"
# include "rand.h"

/* Function to allocate memory to a population */
void allocate_memory_pop (population *pop, int size)
{
  int i;
  pop->ind = (individual *)malloc(size*sizeof(individual));

  for (i= 0; i < size; i++)
  {
    allocate_memory_ind (&(pop->ind[i]));
  }

  return;
}

/* Function to allocate memory to an individual */
void allocate_memory_ind (individual *ind)
{
  int j;

  if (nreal > 0)
  {
    ind->xreal = (double *)malloc(nreal*sizeof(double));
  }
  else
  {
    ind->xreal = NULL;
  }

  if (nbin > 0)
  {
    ind->xbin = (double *)malloc(nbin * sizeof(double));
    ind->gene = (int **)malloc(nbin * sizeof(int*));

    for (j = 0; j < nbin; j++)
    {
      ind->gene[j] = (int *) malloc (nbits[j]*sizeof(int));
    }
  }
  else
  {
    ind->xbin = NULL;
    ind->gene = NULL;
  }


  ind->obj = (double *)malloc(nobj*sizeof(double));

  if (ncon > 0)
  {
    ind->constr = (double *)malloc(ncon*sizeof(double));
  }
  else
  {
    ind->constr = NULL;
  }

  return;
}

/* Function to deallocate memory to a population */
void deallocate_memory_pop (population *pop, int size)
{
  int i;

  for (i=0; i<size; i++)
  {
    deallocate_memory_ind (&(pop->ind[i]));
  }

  free (pop->ind);
  pop->ind = NULL;

  return;
}

/* Function to deallocate memory to an individual */
void deallocate_memory_ind (individual *ind)
{
  int j;

  if (nreal > 0)
  {
    free(ind->xreal);
    ind->xreal = NULL;
  }

  if (nbin > 0)
  {

    for (j=0; j<nbin; j++)
    {
      free(ind->gene[j]);
      ind->gene[j] = NULL;
    }

    free(ind->xbin);
    ind->xbin = NULL;

    free(ind->gene);
    ind->gene =NULL;
  }

  free(ind->obj);
  ind->obj = NULL;

  if (ncon > 0)
  {
    free(ind->constr);
    ind->constr = NULL;
  }

  return;
}
