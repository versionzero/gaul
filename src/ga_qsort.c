/**********************************************************************
  ga_qsort.c
 **********************************************************************

  ga_qsort - GA population sorting routines.
  Copyright �2000-2001, Stewart Adcock <stewart@bellatrix.pcl.ox.ac.uk>

  The latest version of this program should be available at:
  http://www.stewart-adcock.co.uk/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.  Alternatively, if your project
  is incompatible with the GPL, I will probably agree to requests
  for permission to use the terms of any other license.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY WHATSOEVER.

  A full copy of the GNU General Public License should be in the file
  "COPYING" provided with this distribution; if not, see:
  http://www.gnu.org/

 **********************************************************************

  Synopsis:	Sort the population by fitness.  Use the
		quicksort algorithm, but since it is inefficient for
		small arrays we use a shuffle sort to sort blocks
		of less than 8 elements.

  Implementation note:	I can clearly choose functions to inline better
			than gcc/egcs can.  (Shame)

 **********************************************************************/

#include "ga_qsort.h"

/*
 * Nice and useful macro
 */
#define swap_e(x, y)	{entity *t; t = x; x = y; y = t; }

#ifdef GA_QSORT_TIME
#include <time.h>

typedef struct
  {
  clock_t	begin_clock, save_clock;
  time_t	begin_time, save_time;
  } time_record;

static time_record	tim;

void start_timer(void)
  {
  tim.begin_clock = tim.save_clock = clock();
  tim.begin_time = tim.save_time = time(NULL);

  plog(LOG_NORMAL, "Timer started: %d", clock());
  }


double check_timer(void)
  {
  double	user_time, real_time;

  plog(LOG_NORMAL, "Timer checked: %d", clock());

  user_time = (clock() - tim.save_clock) / (double) CLOCKS_PER_SEC;
  real_time = difftime(time(NULL), tim.save_time);
  tim.save_clock = clock();
  tim.save_time = time(NULL);

  plog(LOG_NORMAL, "User time: %f seconds.", user_time);
  plog(LOG_NORMAL, "Real time: %f seconds.", real_time);

  return user_time;
  }

#endif


/**********************************************************************
  quicksort_population()
  Synopsis:	Sorts an array of pointers to compound objects by
		their fitnesss.  This implements the quicksort
		algorithm, but uses a shuffle sort for arrays of
		less than 8 elements.
  Parameters:	int *first, int *last - pointers to first and last
		elements to be sorted.
  Return:	nought be returned.
 **********************************************************************/

#ifdef __GNUC__
inline int partition(entity **array_of_ptrs, int first, int last, double pivot)
#else
int partition(entity **array_of_ptrs, int first, int last, double pivot)
#endif
  {
#if GA_QSORT_DEBUG>2
  printf("DEBUG: partitioning first %d last %d at %f\n", first, last, pivot);
#endif

  while (first < last)
    {
    while (array_of_ptrs[first]->fitness > pivot) first++;
    while (array_of_ptrs[last]->fitness < pivot) last--;
    if (first < last)
      {
      swap_e(array_of_ptrs[first], array_of_ptrs[last]);
      first++;
      last--;
      }
    }

  return first;
  }


#ifdef __GNUC__
inline boolean find_pivot(entity **array_of_ptrs, int first, int last, double *pivot_ptr)
#else
boolean find_pivot(entity **array_of_ptrs, int first, int last, double *pivot_ptr)
#endif
  {
  double a, b;

  a = array_of_ptrs[first]->fitness;
  b = array_of_ptrs[last]->fitness;

  while (a == b && first != last)
    {
    first++;
    a = array_of_ptrs[first]->fitness;
    }

  if (first < last)
    {
    *pivot_ptr = 0.5*(a+b);
    return TRUE;
    }
  
/* All values are the same */
  return FALSE;
  }


static void qksort_population(entity **array_of_ptrs, int first, int last)
  {
  double	pivot;
  int		k;
 
#if GA_QSORT_DEBUG>1
  printf("DEBUG: first %d last %d\n", first, last);
#endif

#if GA_QSORT_DEBUG>2
  for (k = first ; k < last ; k++)
    printf("%u: %f\n", k, array_of_ptrs[k]->fitness);
#endif

/* Don't bother sorting small ranges
  if ( last - first < 7) return;
*/

  if (find_pivot(array_of_ptrs, first, last, &pivot) == FALSE) return;

  k = partition(array_of_ptrs, first, last, pivot);

#if GA_QSORT_DEBUG>1
  printf("DEBUG: partition %d pivot %f\n", k, pivot);
#endif

/*
 * Don't really need these comparisions, unless we selected a poor pivot?
 * But actually, I've realised that since the population often has degenerate
 * fitness scores, this is a good way to cut down on the sort's workload.
 */
  if (k==first)
    {
    while (array_of_ptrs[k]->fitness == array_of_ptrs[first]->fitness && first<last)
      {
      first++;
      k++;
      }
    }
  else if (k==last)
    {
    while (array_of_ptrs[k]->fitness == array_of_ptrs[last]->fitness && last>first)
      {
      last--;
      }
    }

  if (k > first+7) qksort_population(array_of_ptrs, first, k-1);
  if (k < last-6) qksort_population(array_of_ptrs, k, last);

  return;
  }


void quicksort_population(population *pop)
  {
  int		k;		/* Loop variable. */
  int		first=0, last=pop->size-1;	/* Indices into population. */
  entity	**array_of_ptrs=pop->entity_iarray;
  boolean	done=FALSE;	/* Whether shuffle sort is complete. */

  plog(LOG_VERBOSE, "Sorting population with %d members.", pop->size);

#ifdef GA_QSORT_TIME
  start_timer();
#endif

#if GA_QSORT_DEBUG>2
  printf("Unsorted:\n");
  for (i=0; i<pop->size; i++)
    printf("%6d: %f\n", i, pop->entity_iarray[i]->fitness);
#endif

/*
 * Do the actual quicksort.
 * But don't bother if the array is really small.
 */
  if (last > 7) qksort_population(array_of_ptrs, 0, last);
  
#if GA_QSORT_DEBUG>2
  printf("Sorted:\n");
  for (i=0; i<pop->size; i++)
    printf("%6d: %f\n", i, pop->entity_iarray[i]->fitness);
#endif

/*
 * A bi-directional bubble sort (shuffle sort, apparently) to
 * complete the sort.
 * NB/ Could optimise more by moving this into the qksort_population()
 * function, thus avoiding many unnecessary comparisons.
 */

/*  for (i=0;i<3;i++) */
  while (done == FALSE)
    {
    for (k = first ; k < last ; k++)
      {
      if ( array_of_ptrs[k]->fitness < array_of_ptrs[k+1]->fitness )
        {
        swap_e(array_of_ptrs[k], array_of_ptrs[k+1]);
        }
      }
    last--;	/* The last one *MUST* be correct now. */

    done = TRUE;

    for (k = last ; k > first ; k--)
      {
      if ( array_of_ptrs[k]->fitness > array_of_ptrs[k-1]->fitness )
        {
        swap_e(array_of_ptrs[k], array_of_ptrs[k-1]);
        done = FALSE;
        }
      }
    first++;	/* The first one *MUST* be correct now. */
    }

#if GA_QSORT_DEBUG>0
/* Check that the population is correctly sorted. */
  for (k = 1 ; k < pop->size ; k++)
    if ( array_of_ptrs[k-1]->fitness < array_of_ptrs[k]->fitness )
      {
      plog(LOG_DEBUG, "Population is incorrectly ordered.");
      }
#endif

#ifdef GA_QSORT_TIME
  check_timer();
#endif

  return;
  }


/* To test+time these functions, compile with something like:
   gcc ga_qsort.c -DGA_QSORT_TIME -DGA_QSORT_COMPILE_MAIN ga_core.o \
     -o qsort `gtk-config --cflags` \
     -DNO_TRACE -DMEMORY_ALLOC_DEBUG \
     -DQSORT_DEBUG=3
 */
#if GA_QSORT_COMPILE_MAIN
int main(int argc, char **argv)
#else
boolean ga_qsort_test(void)
#endif
  {
  int		i;		/* Loop variable */
  population	*pop=NULL;	/* Test population */

  pop = ga_population_new(50000, 50000, 4, 32);

/* Randomly assigned fitnesses */
  for (i=0; i<50000; i++)
    {
    pop->entity_array[i].fitness=(double) rand()/RAND_MAX;
    pop->entity_iarray[i]=&(pop->entity_array[i]);
    }
  pop->size=50000;

  plog(LOG_NORMAL, "Sorting random list.");
  quicksort_population(pop);

  plog(LOG_NORMAL, "Sorting ordered list.");
  quicksort_population(pop);

/* Reverse population */
  for (i=0; i<50000/2; i++)
    swap_e(pop->entity_iarray[i],pop->entity_iarray[24999-i]);

  plog(LOG_NORMAL, "Sorting reverse-ordered list.");
  quicksort_population(pop);

/* Write list */
/*
  for (i=0; i<50000; i++)
    printf("%6d: %f\n", i, pop->entity_iarray[i]->fitness);
*/

#if GA_QSORT_COMPILE_MAIN
  exit(OKAY);
#else
  return TRUE;
#endif
  }

/*
 * SLang intrinsic wrapper.
 */
#if HAVE_SLANG==1
void ga_population_sort(int *id)
  {
  quicksort_population(ga_get_population_from_id(*id));
  return;
  }
#endif

