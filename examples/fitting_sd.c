/**********************************************************************
  fitting_sd.c
 **********************************************************************

  fitting_sd - Test/example program for GAUL.
  Copyright ©2002, Stewart Adcock <stewart@linux-domain.com>
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

  Synopsis:	Test/example program for GAUL demonstrating use
		of the steepest ascent algorithm.

		This program aims to fit a function of the form
		y = Ax exp{Bx+C} + D
		through an input dataset.

  Last Updated:	21 Nov 2002 SAA	Based on examples/fitting_simplex.c

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
  fitting_double_to_double()
  synopsis:     Convert to double array.
  parameters:
  return:
  last updated: 21 Nov 2002
 **********************************************************************/

boolean fitting_to_double(population *pop, entity *entity, double *array)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  array[0] = ((double *)entity->chromosome[0])[0];
  array[1] = ((double *)entity->chromosome[0])[1];
  array[2] = ((double *)entity->chromosome[0])[2];

  return TRUE;
  }


/**********************************************************************
  fitting_from_double()
  synopsis:     Convert from double array.
  parameters:
  return:
  last updated: 21 Nov 2002
 **********************************************************************/

boolean fitting_from_double(population *pop, entity *entity, double *array)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  if (!entity->chromosome) die("Entity has no chromsomes.");

  ((double *)entity->chromosome[0])[0] = array[0];
  ((double *)entity->chromosome[0])[1] = array[1];
  ((double *)entity->chromosome[0])[2] = array[2];

  return TRUE;
  }


/**********************************************************************
  fitting_score()
  synopsis:	Fitness function.
  parameters:
  return:
  updated:	17 Nov 2002
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
    score += SQU(data->y[i]-(data->x[i]*params[0]*exp(params[1])+params[2]));
    }

  entity->fitness = -score/data->num_data;
  
  return TRUE;
  }


/**********************************************************************
  fitting_analytical_gradient()
  synopsis:     Calculate gradients analytically.
		FIXME: Not correct!
  parameters:
  return:
  last updated: 21 Nov 2002
 **********************************************************************/

double fitting_analytical_gradient(population *pop, entity *entity, double *params, double *grad)
  {
  int			i;		/* Loop variable over training points. */
  fitting_data_t	*data;		/* Training data. */
  double		E, F;		/* Intermediate values. */
  double		grms=0.0;	/* RMS gradient. */

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  data = (fitting_data_t *)pop->data;

  grad[0] = 0.0;
  grad[1] = 0.0;
  grad[2] = 0.0;

  for (i=0; i<data->num_data; i++)
    {
    E = data->x[i]*params[0]*exp(params[1]);
    F = 2*E - 2*data->y[i] - 2*params[2];

    grad[0] += -data->x[i]*exp(data->x[i]*params[1]+params[2])*F;

    grad[1] += data->x[i]*E*F;

    grad[3] += 2*data->y[i] + 2*params[2] - 2*E;
    }

  grad[0] /= SQU(data->num_data);
  grad[1] /= SQU(data->num_data);
  grad[2] /= SQU(data->num_data);

  grms = sqrt(grad[0]*grad[0]+grad[1]*grad[1]+grad[2]*grad[2]);

  return grms;
  }


/**********************************************************************
  fitting_numerical_gradient()
  synopsis:     Calculate gradients numerically.
  parameters:
  return:
  last updated: 21 Nov 2002
 **********************************************************************/

#define DELTA		1.0e-6

double fitting_numerical_gradient(population *pop, entity *entity, double *params, double *grad)
  {
  int			i;		/* Loop variable over training points. */
  fitting_data_t	*data;		/* Training data. */
  double		grms=0.0;	/* RMS gradient. */

  if (!pop) die("Null pointer to population structure passed.");
  if (!entity) die("Null pointer to entity structure passed.");

  data = (fitting_data_t *)pop->data;

  grad[0] = 0.0;
  grad[1] = 0.0;
  grad[2] = 0.0;

  for (i=0; i<data->num_data; i++)
    {
    grad[0] += SQU(data->y[i]-(data->x[i]*(params[0]+DELTA)*exp(params[1])+params[2]))
               - SQU(data->y[i]-(data->x[i]*(params[0]-DELTA)*exp(params[1])+params[2]));
    grad[1] += SQU(data->y[i]-(data->x[i]*params[0]*exp(params[1]+DELTA)+params[2]))
               - SQU(data->y[i]-(data->x[i]*params[0]*exp(params[1]-DELTA)+params[2]));
    grad[2] += SQU(data->y[i]-(data->x[i]*params[0]*exp(params[1])+params[2]+DELTA))
               - SQU(data->y[i]-(data->x[i]*params[0]*exp(params[1])+params[2]-DELTA));
    }

  grad[0] /= -2*data->num_data*DELTA;
  grad[1] /= -2*data->num_data*DELTA;
  grad[2] /= -2*data->num_data*DELTA;

  grms = sqrt(grad[0]*grad[0]+grad[1]*grad[1]+grad[2]*grad[2]);

  return grms;
  }


/**********************************************************************
  fitting_iteration_callback()
  synopsis:	Generation callback
  parameters:
  return:
  updated:	21 Nov 2002
 **********************************************************************/

boolean fitting_iteration_callback(int iteration, entity *solution)
  {

  printf( "%d: y = %f x exp(%f) + %f (fitness = %f)\n",
            iteration,
            ((double *)solution->chromosome[0])[0],
            ((double *)solution->chromosome[0])[1],
            ((double *)solution->chromosome[0])[2],
            solution->fitness );

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
  ((double *)adam->chromosome[0])[0] = random_double(5.0);
  ((double *)adam->chromosome[0])[1] = -random_double(2.0);
  ((double *)adam->chromosome[0])[2] = random_double(10.0)+10.0;

  return TRUE;
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
  updated:	21 Nov 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population		*pop;			/* Population of solutions. */
  fitting_data_t	data={0,0,NULL,NULL};	/* Training data. */
  entity		*solution;		/* Optimised solution. */

  random_seed(23091975);

  pop = ga_genesis_double(
       50,				/* const int              population_size */
       1,				/* const int              num_chromo */
       3,				/* const int              len_chromo */
       NULL,				/* GAgeneration_hook      generation_hook */
       fitting_iteration_callback,	/* GAiteration_hook       iteration_hook */
       NULL,				/* GAdata_destructor      data_destructor */
       NULL,				/* GAdata_ref_incrementor data_ref_incrementor */
       fitting_score,			/* GAevaluate             evaluate */
       fitting_seed,			/* GAseed                 seed */
       NULL,				/* GAadapt                adapt */
       NULL,				/* GAselect_one           select_one */
       NULL,				/* GAselect_two           select_two */
       NULL,				/* GAmutate               mutate */
       NULL,				/* GAcrossover            crossover */
       NULL,				/* GAreplace              replace */
       NULL				/* vpointer		User data */
            );

  ga_population_set_gradient_parameters(
       pop,				/* population		*pop */
       fitting_to_double,		/* const GAto_double	to_double */
       fitting_from_double,		/* const GAfrom_double	from_double */
       fitting_analytical_gradient,	/* const GAgradient	gradient */
#if 0
       fitting_numerical_gradient,	/* const GAgradient	gradient */
#endif
       3,				/* const int		num_dimensions */
       0.01				/* const double		step_size */
                       );

  get_data(&data);
  pop->data = &data;

  /* Evaluate and sort the initial population members (i.e. select best of 50 random solutions. */
  ga_population_score_and_sort(pop);

  /* Use the best population member. */
  solution = ga_get_entity_from_rank(pop, 0);

  ga_steepestascent(
       pop,				/* population		*pop */
       solution,			/* entity		*solution */
       1000				/* const int		max_iterations */
            );

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


