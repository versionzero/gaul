/**********************************************************************
  nnevolve.c
 **********************************************************************

  nnevolve - GAUL example: Evolve a fixed topology neural network.
  Copyright ©2002, The Regents of the University of California.
  Primary author: "Stewart Adcock" <stewart@linux-domain.com>

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

  Synopsis:	Example program for GAUL.

		This program aims to evolve a fixed topology neural
		network.  Although the topology of the network is
		fixed, certian parameters are evolved along with the
		weights.  I haven't seen this in the literature, but
		I haven't looked particularly hard either.

		The genome consists of a single chromosome which is
		simply the neural network structure, as defined in
		nn_util.h.  This is an example where
		population->len_chromosome is ignored.

 **********************************************************************/

#include "gaul.h"
#include "nn_util.h"

/*
 * Global data storage.
 */
float      **train_data=NULL;       /* Input data for training. */
int        num_train_data=0;        /* Number of training target items. */
float      **train_property=NULL;   /* Training target property. */
int        num_train_prop=0;        /* Number of training target properties. */
float      **score_data=NULL;       /* Input data for training. */
int        num_score_data=0;        /* Number of training target items. */
float      **score_property=NULL;   /* Training target property. */
int        num_score_prop=0;        /* Number of training target properties. */

/**********************************************************************
  nnevolve_evaluate()
  synopsis:	Score solution.
  parameters:
  return:
  updated:	29 Jan 2002
 **********************************************************************/

boolean nnevolve_evaluate(population *pop, entity *entity)
  {
  int		item, n;
  network_t	*nn=(network_t *)entity->chromosome[0];
  float		error=0;	/* Summed network errors. */

  for (n=0; n<num_score_data; n++)
    {
    item = random_int(num_score_data);
/*
printf("DEBUG: item %d = %d/%d\n", n, item, num_score_data);
printf("DEBUG: %f %f\n", score_data[item][0], score_property[item][0]);
*/
    NN_simulate(nn, score_data[item], score_property[item]);
    error += nn->error;
    }

  entity->fitness = error;

  return TRUE;
  }


/**********************************************************************
  nnevolve_seed()
  synopsis:	Seed initial solutions.
  parameters:
  return:
  updated:	29 Jan 2002
 **********************************************************************/

void nnevolve_seed(population *pop, entity *adam)
  {
  network_t	*nn=(network_t *)adam->chromosome[0];

  NN_randomize_weights(nn);

  NN_set_momentum(nn, random_float_range(0.1,1.0));
  NN_set_rate(nn, random_float_range(0.01,0.5));
  NN_set_gain(nn, random_float_range(0.9,1.1));
  NN_set_bias(nn, random_float_range(0.9,1.1));

  return;
  }


/**********************************************************************
  nnevolve_adapt()
  synopsis:	Adapt NN through back-propogation with momentum.
  parameters:
  return:
  updated:	29 Jan 2002
 **********************************************************************/

entity *nnevolve_adapt(population *pop, entity *child)
  {
  entity        *adult;         /* Adapted solution. */
  network_t	*nn=(network_t *)child->chromosome[0];
  int		n, item;

/*
 * We must generate a new solution by copying the original solution.
 * This function copys all genomic, and if appropriate, phenomic data.
 * It is never safe to adapt the solution in place.
 */
  adult = ga_entity_clone(pop, child);

/*
 * Train network using back-propagation with momentum using
 * randomly selected training data.  (10 items)
 */
  for (n=0; n<10; n++)
    {
    item = random_int(num_train_data);
    NN_simulate(nn, train_data[item], train_property[item]);

    NN_backpropagate(nn);
    NN_adjust_weights(nn);
    }

  return adult;
  }


/**********************************************************************
  nnevolve_crossover()
  synopsis:	Crossover.
		FIXME: Only quickly hacked together.
  parameters:
  return:
  updated:	28 Jan 2002
 **********************************************************************/

void nnevolve_crossover(population *pop, entity *mother, entity *father, entity *daughter, entity *son)
  {
  int		l, i;	/* Layer, neuron, number. */
  network_t     *nn1=(network_t *)son->chromosome[0], *nn2=(network_t *)daughter->chromosome[0];
  float		temp;

  NN_copy((network_t *)mother->chromosome[0], nn1);
  NN_copy((network_t *)father->chromosome[0], nn2);

  if (random_boolean())
    {
    temp = nn1->momentum;
    nn1->momentum = nn2->momentum;
    nn2->momentum = temp;
    }

  if (random_boolean())
    {
    temp = nn1->rate;
    nn1->rate = nn2->rate;
    nn2->rate = temp;
    }

  if (random_boolean())
    {
    temp = nn1->gain;
    nn1->gain = nn2->gain;
    nn2->gain = temp;
    }

  if (random_boolean())
    {
    temp = nn1->bias;
    nn1->bias = nn2->bias;
    nn2->bias = temp;
    }

  for (l=1; l<nn1->num_layers; l++)
    {
    if (random_boolean())
      {
      for (i=1; i<=nn1->layer[l].neurons; i++)
        {
        memcpy(nn1->layer[l].weight[i], ((network_t *)father->chromosome[0])->layer[l].weight[i], nn1->layer[l-1].neurons+1);
        memcpy(nn2->layer[l].weight[i], ((network_t *)mother->chromosome[0])->layer[l].weight[i], nn1->layer[l-1].neurons+1);
        }
      }
    }

  return;
  }


/**********************************************************************
  nnevolve_mutate()
  synopsis:	Mutation.
  parameters:
  return:
  updated:	28 Jan 2002
 **********************************************************************/

void nnevolve_mutate(population *pop, entity *mother, entity *son)
  {
  network_t	*nn=(network_t *)son->chromosome[0];
  int		l,i,j,event;

/*
 * Copy the network from the parent.
 */
  NN_copy((network_t *)mother->chromosome[0], nn);

/*
 * Equal chance for tweaking momentum, gain, rate, bias, or
 * randomly setting one weight.
 */
  event = random_int(5);

  switch (event)
    {
    case 0:
      nn->momentum += random_float_range(-0.2,0.2);
      break;
    case 1:
      nn->gain += random_float_range(-0.2,0.2);
      break;
    case 2:
      nn->rate += random_float_range(-0.2,0.2);
      break;
    case 3:
      NN_set_bias(nn, nn->bias+random_float_range(-0.2,0.2));
      break;
    default:
/*
 * This random selection is open to improvement.
 * (a) only 1 random number should be needed.
 * (b) currently weights are not selected with equal probability.
 */
      l = random_int(nn->num_layers-1)+1;
      i = random_int(nn->layer[l].neurons)+1;
      j = random_int(nn->layer[l-1].neurons+1);

      nn->layer[l].weight[i][j] = random_float(1.0);
    }

  return;
  }


/**********************************************************************
  nnevolve_ga_callback()
  synopsis:	Analysis callback.
  parameters:
  return:
  updated:	28 Jan 2002
 **********************************************************************/

boolean nnevolve_generation_hook(int generation, population *pop)
  {
  entity	*best;		/* Top ranked solution. */

  best = ga_get_entity_from_rank(pop, 0);

  printf( "%d: fitness = %f momentum = %f gain = %f rate = %f bias = %f\n",
            generation,
            best->fitness,
            ((network_t *)best->chromosome[0])->momentum,
            ((network_t *)best->chromosome[0])->gain,
            ((network_t *)best->chromosome[0])->rate,
            ((network_t *)best->chromosome[0])->bias );

  return TRUE;
  }


/**********************************************************************
  nnevolve_chromosome_constructor()
  synopsis:	Chromosome constructor function.
  parameters:
  return:
  updated:	29 Jan 2002
 **********************************************************************/

void nnevolve_chromosome_constructor(population *pop, entity *embryo)
  {
  int        num_layers=4;   /* Number of layers in NN. */
  int        neurons[4]={11,10,10,3};  /* Number of neurons in each layer. */


  if (!pop) die("Null pointer to population structure passed.");
  if (!embryo) die("Null pointer to entity structure passed.");

  if (embryo->chromosome!=NULL)
    die("This entity already contains chromosomes.");

  embryo->chromosome = s_malloc(sizeof(network_t *));
  embryo->chromosome[0] = NN_new(num_layers, neurons);

  return;
  }


/**********************************************************************
  nnevolve_chromosome_destructor()
  synopsis:     Chromosome destructor function.
  parameters:
  return:
  last updated: 29 Jan 2002
 **********************************************************************/

void nnevolve_chromosome_destructor(population *pop, entity *corpse)
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!corpse) die("Null pointer to entity structure passed.");

  if (corpse->chromosome==NULL)
    die("This entity already contains no chromosomes.");

  NN_destroy(corpse->chromosome[0]);
  s_free(corpse->chromosome);
  corpse->chromosome=NULL;

  return;
  }


/**********************************************************************
  nnevolve_chromosome_replicate()
  synopsis:     Chromosome duplication function.
  parameters:
  return:
  last updated: 29 Jan 2002
 **********************************************************************/

void nnevolve_chromosome_replicate( population *pop,
                                    entity *parent, entity *child,
                                    const int chromosomeid )
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!parent || !child) die("Null pointer to entity structure passed.");
  if (!parent->chromosome || !child->chromosome) die("Entity has no chromosomes.");
  if (!parent->chromosome[0] || !child->chromosome[0]) die("Entity has empty chromosomes.");
  if (chromosomeid != 0) die("Invalid chromosome index passed.");

  NN_copy((network_t *)child->chromosome[chromosomeid],
           (network_t *)parent->chromosome[chromosomeid]);

  return;
  }


/**********************************************************************
  nnevolve_chromosome_to_bytes()
  synopsis:     Convert chromosome to contiguous form.
  		FIXME: incorrect.
  parameters:
  return:       Number of bytes processed.
  last updated: 04 Feb 2002
 **********************************************************************/

unsigned int nnevolve_chromosome_to_bytes(population *pop, entity *joe,
                                     byte **bytes, unsigned int *max_bytes)
  {
  int           num_bytes;      /* Actual size of genes. */

  dief("Function not implemented");

  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

  if (*max_bytes!=0) die("Internal error.");

  if (!joe->chromosome || !joe->chromosome[0]) die("Entity has no chromosome.");

  num_bytes = sizeof(network_t);

  *bytes = (byte *)joe->chromosome[0];

  return (unsigned int) num_bytes;
  }


/**********************************************************************
  nnevolve_chromosome_from_bytes()
  synopsis:     Convert contiguous form to chromosome.
  		FIXME: incorrect.
  parameters:
  return:
  last updated: 29 Jan 2002
 **********************************************************************/

void nnevolve_chromosome_from_bytes(population *pop, entity *joe, byte *bytes)
  {
  dief("Function not implemented");

  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

  if (!joe->chromosome || !joe->chromosome[0]) die("Entity has no chromosome.");

  memcpy(joe->chromosome[0], bytes, sizeof(network_t));

  return;
  }


/**********************************************************************
  nnevolve_chromosome_to_string()
  synopsis:     Chromosome conversion to human readable static string.
  		FIXME: incorrect.
  parameters:
  return:
  last updated: 29 Jan 2002
 **********************************************************************/

char *nnevolve_chromosome_to_string(population *pop, entity *joe)
  {
  static char   *text=NULL;     /* String for display. */

  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

  dief("Function not implemented");

  return text;
  }


/**********************************************************************
  nnevolve_setup_data()
  synopsis:	Some simple hard-coded data.
  		FIXME: Need to read more challenging data from disk.
		FIXME: Also need to free this data eventually.
  parameters:	none
  return:	none
  updated:	04 Feb 2002
 **********************************************************************/

void nnevolve_setup_data(void)
  {
  int		i, j;
#include "wine.data"

  train_property = s_malloc(sizeof(float *)*178);
  train_data = s_malloc(sizeof(float *)*178);
  score_property = s_malloc(sizeof(float *)*178);
  score_data = s_malloc(sizeof(float *)*178);

  for (i=0; i<178; i++)
    {
    if (random_boolean_prob(0.1))
      {
      score_property[num_score_prop] = s_malloc(sizeof(float)*3);
      score_data[num_score_data] = s_malloc(sizeof(float)*13);

      for (j=0; j<4; j++)
        {
        score_property[num_score_prop][j] = data[num_score_prop][j];
        }
      for (j=4; j<16; j++)
        {
        score_data[num_score_data][j-4] = data[num_score_data][j];
        }
      num_score_data++;
      num_score_prop++;
      }
    else
      {
      train_property[num_train_prop] = s_malloc(sizeof(float)*3);
      train_data[num_train_data] = s_malloc(sizeof(float)*13);

      for (j=0; j<4; j++)
        {
        train_property[num_train_prop][j] = data[num_train_prop][j];
        }
      for (j=4; j<16; j++)
        {
        train_data[num_train_data][j-4] = data[num_train_data][j];
        }
      num_train_data++;
      num_train_prop++;
      }
    }

  printf("Data has been partitioned into %d training and %d scoring items.\n", num_score_data, num_train_data);

  return;
  }


/**********************************************************************
  main()
  synopsis:	Evolve a fixed topology neural network.
  parameters:
  return:	2, on success.
  updated:	28 Jan 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;	/* Population of solutions. */

/*
 * Initialize random number generator.
 */
  random_init();
  random_seed(42);

/*
 * Allocate a new popuation structure.
 * max. individuals        = 200
 * stable num. individuals = 40
 * num. chromosomes        = 1
 * length of chromosomes   = 0 (This is ignored by the constructor)
 */
  pop = ga_population_new( 200, 40, 1, 0 );
  if ( !pop ) die("Unable to allocate population.");

/*
 * Define chromosome handling functions for the custom
 * NN chromosome type.
 */
  pop->chromosome_constructor = nnevolve_chromosome_constructor;
  pop->chromosome_destructor = nnevolve_chromosome_destructor;
  pop->chromosome_replicate = nnevolve_chromosome_replicate;
  pop->chromosome_to_bytes = nnevolve_chromosome_to_bytes;
  pop->chromosome_from_bytes = nnevolve_chromosome_from_bytes;
  pop->chromosome_to_string = nnevolve_chromosome_to_string;

/*
 * Define all of the needed callback functions.
 */
  pop->generation_hook = nnevolve_generation_hook;
  pop->iteration_hook = NULL;
  pop->data_destructor = NULL;
  pop->data_ref_incrementor = NULL;

  pop->evaluate = nnevolve_evaluate;
  pop->seed = nnevolve_seed;
  pop->adapt = nnevolve_adapt;
  pop->select_one = ga_select_one_roulette;
  pop->select_two = ga_select_two_roulette;
  pop->mutate = nnevolve_mutate;
  pop->crossover = nnevolve_crossover;
  pop->replace = NULL;

/*
 * Seed the initial population.
 */
  ga_population_seed(pop);

/*
 * Set the GA parameters:
 * Crossover ratio  = 0.7
 * Mutation ratio   = 0.1
 * Migration ration = 0.0
 */
  ga_population_set_parameters( pop, 0.7, 0.1, 0.0 );

/*
 * Setup the data for NN simulation.
 */
  nnevolve_setup_data();

/*
 * Perform Lamarckian evolution for 200 generations.
 */
  ga_evolution( pop, GA_CLASS_LAMARCK_ALL, GA_ELITISM_PARENTS_SURVIVE, 200 );

  printf("The fitness of the final solution found was: %f\n",
		  ga_get_entity_from_rank(pop,0)->fitness);

/*
 * Write the best solution to disk.
 */
  NN_write((network_t *)ga_get_entity_from_rank(pop,0), "best.nn");

  ga_extinction(pop);

  exit(2);
  }


