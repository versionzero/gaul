/**********************************************************************
  fitting.c
 **********************************************************************

  fitting - Test/example program for GAUL.
  Copyright ©2002-2004, Stewart Adcock <stewart@linux-domain.com>
  All rights reserved.

  The latest version of this program should be available at:
  http://gaul.sourceforge.net/

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

  Synopsis:	Test/example program for GAUL.

		This program aims to fit a function of the form
		y = Ax exp{Bx+C} + D
		through an input dataset.

  Last Updated:	19 Mar 2004 SAA	Custom mutation function since existing function produced mutations that were far too significant.
		21 Nov 2002 SAA	New seeding function.
		17 Nov 2002 SAA	Initial version.

 **********************************************************************/

#include "gaul.h"

/*
 * Datastructure used to demonstrate attachment of data to specific
 * populations.
 * It is used to store the training data.
 */

typedef struct
  {
  int		num_data;
  int		max_data;
  double	*x;
  double	*y;
  } fitting_data_t;

/**********************************************************************
  fitting_score()
  synopsis:	Fitness function.
  parameters:
  return:
  updated:	19 Mar 2004
 **********************************************************************/

boolean fitting_score(population *pop, entity *entity)
  {
  int			i;		/* Loop variable over training points. */
  double		score=0.0;	/* Mean of squared deviations. */
  double		*params;	/* Fitting parameters. */
  fitting_data_t	*data;		/* Training data. */

  entity->fitness = 0;

  data = (fitting_data_t *)pop->data;
  params = (double *)entity->chromosome[0];

  for (i=0; i<data->num_data; i++)
    {
    score += SQU(data->y[i]-(data->x[i]*params[0]*exp(data->x[i]*params[1]+params[2])+params[3]));
    }

  entity->fitness = -sqrt(score / data->num_data);
  
  return TRUE;
  }


/**********************************************************************
  fitting_generation_callback()
  synopsis:	Generation callback
  parameters:
  return:
  updated:	17 Nov 2002
 **********************************************************************/

boolean fitting_generation_callback(int generation, population *pop)
  {

  printf( "%d: y = %fx exp(%fx + %f) + %f (fitness = %f)\n",
            generation,
            ((double *)pop->entity_iarray[0]->chromosome[0])[0],
            ((double *)pop->entity_iarray[0]->chromosome[0])[1],
            ((double *)pop->entity_iarray[0]->chromosome[0])[2],
            ((double *)pop->entity_iarray[0]->chromosome[0])[3],
            pop->entity_iarray[0]->fitness );

  return TRUE;
  }


/**********************************************************************
  fitting_seed()
  synopsis:	Seed genetic data.
  parameters:	population *pop
		entity *adam
  return:	success
  last updated: 21 Nov 2002
 **********************************************************************/

boolean fitting_seed(population *pop, entity *adam)
  {

/* Checks. */
  if (!pop) die("Null pointer to population structure passed.");
  if (!adam) die("Null pointer to entity structure passed.");

/* Seeding. */
  ((double *)adam->chromosome[0])[0] = random_double(2.0);
  ((double *)adam->chromosome[0])[1] = random_double(2.0);
  ((double *)adam->chromosome[0])[2] = random_double(2.0);
  ((double *)adam->chromosome[0])[3] = random_double(4.0)-2.0;

  return TRUE;
  }


/**********************************************************************
  ga_mutate_double_singlepoint_drift()
  synopsis:	Cause a single mutation event in which a single
		allele is adjusted.  (0.1*Unit Gaussian distribution.)
  parameters:
  return:
  last updated: 19 Mar 2004
 **********************************************************************/

void fitting_mutate_double_singlepoint_drift( population *pop,
                                          entity *father, entity *son )
  {
  int		i;		/* Loop variable over all chromosomes */
  int		chromo;		/* Index of chromosome to mutate */
  int		point;		/* Index of allele to mutate */
  double	amount=random_unit_gaussian()*0.1;	/* The amount of drift. */

/* Checks */
  if (!father || !son) die("Null pointer to entity structure passed");

/* Select mutation locus. */
  chromo = (int) random_int(pop->num_chromosomes);
  point = (int) random_int(pop->len_chromosomes);

/*
 * Copy unchanged data.
 */
  for (i=0; i<pop->num_chromosomes; i++)
    {
    memcpy(son->chromosome[i], father->chromosome[i], pop->len_chromosomes*sizeof(double));
    if (i!=chromo)
      {
      ga_copy_data(pop, son, father, i);
      }
    else
      {
      ga_copy_data(pop, son, NULL, i);
      }
    }

/*
 * Mutate by tweaking a single allele.
 */
  ((double *)son->chromosome[chromo])[point] += amount;

  if (((double *)son->chromosome[chromo])[point]>DBL_MAX-1.0) ((double *)son->chromosome[chromo])[point]=DBL_MIN+1.0;
  if (((double *)son->chromosome[chromo])[point]<DBL_MIN+1.0) ((double *)son->chromosome[chromo])[point]=DBL_MAX-1.0;

  return;
  }


/**********************************************************************
  get_data()
  synopsis:	Read training data from standard input.
  parameters:
  return:
  updated:	17 Nov 2002
 **********************************************************************/

void get_data(fitting_data_t *data)
  {
  int           line_count=0;                   /* Number of lines read from stdin. */
  char          buffer[MAX_LINE_LEN], *line;    /* Buffer for input. */

  if (!data) die("Null pointer to data structure passed.");

/*
 * Read lines.  Each specifies one x,y pair except those starting with '#'
 * or '!' which are comment lines and are ignored.  Don't bother parsing
 * blank lines either.
 */
  while ( !feof(stdin) && fgets(buffer, MAX_LINE_LEN, stdin)!=NULL )
    {
    line = buffer;

    /* Skip leading whitespace. */
    while (*line == ' ' || *line == '\t') line++;

    if (*line == '#' || *line == '!' || *line == '\n')
      { /* Ignore this line */
/*    printf("Ignoring line: %s\n", line);*/
      }
    else
      {
/* Ensure sufficient memory is available. */
      if (data->num_data == data->max_data)
        {
        data->max_data += 256;
        data->x = s_realloc(data->x, sizeof(double)*data->max_data);
        data->y = s_realloc(data->y, sizeof(double)*data->max_data);
        }

      sscanf(line, "%lf %lf", &(data->x[data->num_data]), &(data->y[data->num_data]));
      printf("Read %d: %f %f\n", data->num_data, data->x[data->num_data], data->y[data->num_data]);

      data->num_data++;
      }

    line_count++;
    }

  plog(LOG_NORMAL, "Read %f data points from %d lines.\n", data->num_data, line_count);

  return;
  }


/**********************************************************************
  main()
  synopsis:	Main function.
  parameters:
  return:
  updated:	17 Nov 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population		*pop;			/* Population of solutions. */
  fitting_data_t	data={0,0,NULL,NULL};	/* Training data. */

  random_seed(23091975);

  pop = ga_genesis_double(
       200,				/* const int              population_size */
       1,				/* const int              num_chromo */
       4,				/* const int              len_chromo */
       fitting_generation_callback,	/* GAgeneration_hook      generation_hook */
       NULL,				/* GAiteration_hook       iteration_hook */
       NULL,				/* GAdata_destructor      data_destructor */
       NULL,				/* GAdata_ref_incrementor data_ref_incrementor */
       fitting_score,			/* GAevaluate             evaluate */
       fitting_seed,			/* GAseed                 seed */
       NULL,				/* GAadapt                adapt */
       ga_select_one_linearrank,	/* GAselect_one           select_one */
       ga_select_two_linearrank,	/* GAselect_two           select_two */
       fitting_mutate_double_singlepoint_drift,	/* GAmutate               mutate */
       ga_crossover_double_mean,	/* GAcrossover            crossover */
       NULL,				/* GAreplace              replace */
       NULL				/* vpointer	User data */
            );

  ga_population_set_parameters(
       pop,				/* population      *pop */
       GA_SCHEME_DARWIN,		/* const ga_scheme_type     scheme */
       GA_ELITISM_PARENTS_DIE,		/* const ga_elitism_type   elitism */
       0.8,				/* double  crossover */
       0.8,				/* double  mutation */
       0.0      		        /* double  migration */
                              );

  get_data(&data);
  pop->data = &data;

  ga_evolution(
       pop,				/* population	*pop */
       200				/* const int	max_generations */
              );

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


