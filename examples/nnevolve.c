/**********************************************************************
  nnevolve.c
 **********************************************************************

  nnevolve - GAUL example: Evolve a fixed topology neural network.
  Copyright ©2002-2003, The Regents of the University of California.
  All rights reserved.
  Primary author: "Stewart Adcock" <stewart@linux-domain.com>

  The latest version of this program should be available at:
  http://www.linux-domain.com/

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

		Both crossover and mutation rates are comparatively
		low, whilst the Lamarckian adaptation affects all
		members of the population.

 **********************************************************************/

#include "gaul.h"
#include "nn_util.h"
#include "timer_util.h"

/*
 * Global data storage.
 */
static float        **train_data=NULL;       /* Input data for training. */
static float        **train_property=NULL;   /* Training properties. */
static unsigned int num_train_data=0;        /* Number of training items. */

/*
 * Compilation constants.
 */
#define NNEVOLVE_NUM_SCORE	500
#define NNEVOLVE_NUM_TRAIN	5000


/**********************************************************************
  nnevolve_display_evaluation()
  synopsis:	Write score information.
  parameters:
  return:
  updated:	06 Feb 2002
 **********************************************************************/

boolean nnevolve_display_evaluation(population *pop, entity *entity)
  {
  int		n;		/* Loop over all items. */
  network_t	*nn=(network_t *)entity->chromosome[0];
  float		error=0.0;	/* Summed network errors. */
  float		property[4];	/* Prediction from NN. */
  int		wrong_count=0;	/* Number of incorrect predictions. */

  for (n=0; n<num_train_data; n++)
    {
    NN_simulate_with_output(nn, train_data[n], train_property[n], property);
    error += nn->error;

/*
 * This looks a mess, but it uses the fewest 'if' statements in the
 * average case.
 */
    if (property[0] > property[1])
      {
      if (property[0] > property[2])
        {
        if (train_property[n][0] < train_property[n][1] ||
            train_property[n][0] < train_property[n][2])
          wrong_count++;
        }
      else
        {
        if (train_property[n][2] < train_property[n][0] ||
            train_property[n][2] < train_property[n][1])
          wrong_count++;
        }
      }
    else if (property[1] > property[2])
      {
      if (train_property[n][1] < train_property[n][0] ||
          train_property[n][1] < train_property[n][2])
        wrong_count++;
      }
    else
      {
      if (train_property[n][2] < train_property[n][0] ||
          train_property[n][2] < train_property[n][1])
        wrong_count++;
      }

    printf("%d: %f %f %f ... %f %f %f --- %d ( %f )\n", n, property[0], property[1], property[2], train_property[n][0], train_property[n][1], train_property[n][2], wrong_count, nn->error);
    }

  printf("Total network error is %f with %d incorrect predictions.  Score = %f\n",
	 error, wrong_count, entity->fitness);

  return TRUE;
  }


/**********************************************************************
  nnevolve_evaluate_all()
  synopsis:	Score solution - measure average total network error
  		for all data items.
  parameters:
  return:
  updated:	11 Jun 2002
 **********************************************************************/

boolean nnevolve_evaluate_all(population *pop, entity *entity)
  {
  int		n;		/* Loop over all items. */
  network_t	*nn=(network_t *)entity->chromosome[0];
  float		error=0.0;	/* Summed network errors. */
  float		property[4];	/* Prediction from NN. */
  int		wrong_count=0;	/* Number of incorrect predictions. */

  for (n=0; n<num_train_data; n++)
    {
    NN_simulate_with_output(nn, train_data[n], train_property[n], property);
    error += nn->error;

/*
 * This looks a mess, but it uses the fewest 'if' statements in the
 * average case.
 */
    if (property[0] > property[1])
      {
      if (property[0] > property[2])
        {
        if (train_property[n][0] < train_property[n][1] ||
            train_property[n][0] < train_property[n][2])
          wrong_count++;
        }
      else
        {
        if (train_property[n][2] < train_property[n][0] ||
            train_property[n][2] < train_property[n][1])
          wrong_count++;
        }
      }
    else if (property[1] > property[2])
      {
      if (train_property[n][1] < train_property[n][0] ||
          train_property[n][1] < train_property[n][2])
        wrong_count++;
      }
    else
      {
      if (train_property[n][2] < train_property[n][0] ||
          train_property[n][2] < train_property[n][1])
        wrong_count++;
      }
    }

  /*entity->fitness = 1.0/(1.0+(5*error+wrong_count)/(num_train_data*5));*/
/* Added contribution to keep gain close to 1.0 */
  entity->fitness = 1.0/(1.0+(10*error+wrong_count+1000*SQU(1.0-nn->gain)));

  return TRUE;
  }


/**********************************************************************
  nnevolve_evaluate1()
  synopsis:	Score solution - measure network error.
  parameters:
  return:
  updated:	11 Jun 2002
 **********************************************************************/

boolean nnevolve_evaluate1(population *pop, entity *entity)
  {
  int		item, n;
  network_t	*nn=(network_t *)entity->chromosome[0];
  float		error=0.0;	/* Summed network errors. */

  for (n=0; n<NNEVOLVE_NUM_SCORE; n++)
    {
    item = (int) random_int(num_train_data);
    NN_simulate(nn, train_data[item], train_property[item]);
    error += nn->error;
    }

  entity->fitness = 1.0/(1.0+error);

  return TRUE;
  }


/**********************************************************************
  nnevolve_evaluate2()
  synopsis:	Score solution via alternative method - count incorrect
  		assignments in addition to the network error.
  parameters:
  return:
  updated:	07 Jul 2003
 **********************************************************************/

boolean nnevolve_evaluate2(population *pop, entity *entity)
  {
  int		item, n;
  network_t	*nn=(network_t *)entity->chromosome[0];
  float		property[4];	/* Prediction from NN. */
  double	fitness=0.0;	/* Fitness score. */
  float		error=0.0;	/* Summed network errors. */

  fitness = 0.0;

  for (n=0; n<NNEVOLVE_NUM_SCORE; n++)
    {
    item = (int) random_int(num_train_data);
    NN_run(nn, train_data[item], property);
    if (NN_IS_ON(property[0])  != NN_IS_ON(train_property[item][0]))  fitness += 1.0;
    if (NN_IS_OFF(property[0]) != NN_IS_OFF(train_property[item][0])) fitness += 1.0;
    if (NN_IS_ON(property[1])  != NN_IS_ON(train_property[item][1]))  fitness += 1.0;
    if (NN_IS_OFF(property[1]) != NN_IS_OFF(train_property[item][1])) fitness += 1.0;
    if (NN_IS_ON(property[2])  != NN_IS_ON(train_property[item][2]))  fitness += 1.0;
    if (NN_IS_OFF(property[2]) != NN_IS_OFF(train_property[item][2])) fitness += 1.0;
    error += nn->error;
    }

  entity->fitness = 1.0/(1.0+(error+fitness)/NNEVOLVE_NUM_SCORE);

  return TRUE;
  }


/**********************************************************************
  nnevolve_seed()
  synopsis:	Seed initial solutions.
  parameters:
  return:
  updated:	29 Jan 2002
 **********************************************************************/

boolean nnevolve_seed(population *pop, entity *adam)
  {
  network_t	*nn=(network_t *)adam->chromosome[0];

  NN_randomize_weights_01(nn);

  NN_set_momentum(nn, random_float_range(0.4,0.9));
  NN_set_rate(nn, random_float_range(0.1,0.4));
  NN_set_gain(nn, 1.0);
  NN_set_bias(nn, 1.0);

  return TRUE;
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
  entity	*adult;		/* Adapted solution. */
  network_t	*nn=(network_t *)child->chromosome[0];
/*  int		n, item; */

/*
 * We must generate a new solution by copying the original solution.
 * This function copys all genomic, and if appropriate, phenomic data.
 * It is never safe to adapt the solution in place.
 */
  adult = ga_entity_clone(pop, child);

/*
 * Train network using back-propagation with momentum using
 * randomly selected training data.  (NNEVOLVE_NUM_TRAIN items)
 */
#if 0
  for (n=0; n<NNEVOLVE_NUM_TRAIN; n++)
    {
    item = (int) random_int(num_train_data);
    NN_simulate(nn, train_data[item], train_property[item]);

    NN_backpropagate(nn);
    NN_adjust_weights(nn);
    }
#endif

  NN_train_random(nn, 2000);

  ga_entity_evaluate(pop, adult);

  printf("DEBUG: Fitness %f -> %f\n", child->fitness, adult->fitness);

  return adult;
  }


/**********************************************************************
  nnevolve_crossover_layerwise()
  synopsis:	Crossover by swapping whole layers.
		Note that this doesn't work particularly well.
  parameters:
  return:
  updated:	24 Dec 2002
 **********************************************************************/

void nnevolve_crossover_layerwise(population *pop, entity *mother, entity *father, entity *daughter, entity *son)
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

  for (l=1; l<nn1->num_layers; l++)
    {
    if (random_boolean())
      {
      for (i=1; i<=nn1->layer[l].neurons; i++)
        {
        memcpy(nn1->layer[l].weight[i], ((network_t *)father->chromosome[0])->layer[l].weight[i], sizeof(float)*nn1->layer[l-1].neurons+1);
        memcpy(nn2->layer[l].weight[i], ((network_t *)mother->chromosome[0])->layer[l].weight[i], sizeof(float)*nn1->layer[l-1].neurons+1);
        }
      }
    }

/*  printf("DEBUG: Crossover: rate = %f %f -> %f %f momentum = %f %f -> %f %f\n", ((network_t *)father->chromosome[0])->rate, ((network_t *)mother->chromosome[0])->rate, nn1->rate, nn2->rate, ((network_t *)father->chromosome[0])->momentum, ((network_t *)mother->chromosome[0])->momentum, nn1->momentum, nn2->momentum);*/

  return;
  }


/**********************************************************************
  nnevolve_crossover_out()
  synopsis:	Crossover by swapping clusters of neurons from an
  		output node.
  parameters:
  return:
  updated:	08 Feb 2002
 **********************************************************************/

void nnevolve_crossover_out(population *pop, entity *mother, entity *father, entity *daughter, entity *son)
  {
  int		h, i, j, k;	/* Layer, neuron, number. */
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

/*
 * This algorithm would naturally be recursive, but unrolled for the sake
 * of speed.  I assume four layers in total.
 */
  h = (int) random_int(nn1->layer[3].neurons)+1;
  
  for (i=1; i<=nn1->layer[2].neurons; i++)
    {
    nn1->layer[3].weight[h][i] = ((network_t *)father->chromosome[0])->layer[3].weight[h][i];
    nn2->layer[3].weight[h][i] = ((network_t *)mother->chromosome[0])->layer[3].weight[h][i];

    if (random_boolean_prob(0.2))
      {
      for (j=1; j<=nn1->layer[1].neurons; j++)
	{
        nn1->layer[2].weight[i][j] = ((network_t *)father->chromosome[0])->layer[2].weight[i][j];
        nn2->layer[2].weight[i][j] = ((network_t *)mother->chromosome[0])->layer[2].weight[i][j];

        if (random_boolean_prob(0.2))
          {
          for (k=1; k<=nn1->layer[0].neurons; k++)
            {
            nn1->layer[1].weight[j][k] = ((network_t *)father->chromosome[0])->layer[1].weight[j][k];
            nn2->layer[1].weight[j][k] = ((network_t *)mother->chromosome[0])->layer[1].weight[j][k];
	    }
	  }
        }
      }
    }

/*  printf("DEBUG: Crossover: rate = %f %f -> %f %f momentum = %f %f -> %f %f\n", ((network_t *)father->chromosome[0])->rate, ((network_t *)mother->chromosome[0])->rate, nn1->rate, nn2->rate, ((network_t *)father->chromosome[0])->momentum, ((network_t *)mother->chromosome[0])->momentum, nn1->momentum, nn2->momentum);*/

  return;
  }


/**********************************************************************
  nnevolve_crossover_in()
  synopsis:	Crossover by swapping clusters of neurons from an
  		input node.
  parameters:
  return:
  updated:	08 Feb 2002
 **********************************************************************/

void nnevolve_crossover_in(population *pop, entity *mother, entity *father, entity *daughter, entity *son)
  {
  int		h, i, j, k;	/* Layer, neuron, number. */
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

/*
 * This algorithm would naturally be recursive, but unrolled for the sake
 * of speed.  I assume four layers in total.
 */
  h = (int) random_int(nn1->layer[0].neurons+1);
  
  for (i=1; i<=nn1->layer[1].neurons; i++)
    {
    nn1->layer[1].weight[i][h] = ((network_t *)father->chromosome[0])->layer[1].weight[i][h];
    nn2->layer[1].weight[i][h] = ((network_t *)mother->chromosome[0])->layer[1].weight[i][h];

    if (random_boolean_prob(0.2))
      {
      for (j=1; j<=nn1->layer[2].neurons; j++)
	{
        nn1->layer[2].weight[j][i] = ((network_t *)father->chromosome[0])->layer[2].weight[j][i];
        nn2->layer[2].weight[j][i] = ((network_t *)mother->chromosome[0])->layer[2].weight[j][i];

        if (random_boolean_prob(0.2))
          {
          for (k=1; k<=nn1->layer[3].neurons; k++)
            {
            nn1->layer[3].weight[k][j] = ((network_t *)father->chromosome[0])->layer[3].weight[k][j];
            nn2->layer[3].weight[k][j] = ((network_t *)mother->chromosome[0])->layer[3].weight[k][j];
	    }
	  }
        }
      }
    }

/*  printf("DEBUG: Crossover: rate = %f %f -> %f %f momentum = %f %f -> %f %f\n", ((network_t *)father->chromosome[0])->rate, ((network_t *)mother->chromosome[0])->rate, nn1->rate, nn2->rate, ((network_t *)father->chromosome[0])->momentum, ((network_t *)mother->chromosome[0])->momentum, nn1->momentum, nn2->momentum);*/

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
 * Chance for tweaking either momentum or decay equals
 * the chance for randomly tweaking one weight.
 */
  event = (int) random_int(4);

  switch (event)
    {
    case 0:
      nn->momentum += random_float_range(-0.05,0.05);
      break;
    case 1:
      nn->rate += random_float_range(-0.05,0.05);
      break;
    default:
/*
 * This random selection is open to improvement.
 * (a) only 1 random number should be needed.
 * (b) currently weights are not selected with equal probability.
 */
      l = (int) random_int(nn->num_layers-1)+1;
      i = (int) random_int(nn->layer[l].neurons)+1;
      j = (int) random_int(nn->layer[l-1].neurons+1);

      nn->layer[l].weight[i][j] += random_float_range(-0.7,0.7);
    }

/*  printf("DEBUG: Mutate: rate = %f -> %f momentum = %f -> %f\n", ((network_t *)mother->chromosome[0])->rate, nn->rate, ((network_t *)mother->chromosome[0])->momentum, nn->momentum);*/

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

  printf( "%d: fitness = %f momentum = %f rate = %f\n",
            generation,
            best->fitness,
            ((network_t *)best->chromosome[0])->momentum,
            ((network_t *)best->chromosome[0])->rate );

  return TRUE;
  }


/**********************************************************************
  nnevolve_chromosome_constructor()
  synopsis:	Chromosome constructor function.
  parameters:
  return:
  updated:	29 Jan 2002
 **********************************************************************/

boolean nnevolve_chromosome_constructor(const population *pop, entity *embryo)
  {
  int        num_layers=4;   /* Number of layers in NN. */
  int        neurons[4]={11,20,20,3};  /* Number of neurons in each layer. */


  if (!pop) die("Null pointer to population structure passed.");
  if (!embryo) die("Null pointer to entity structure passed.");

  if (embryo->chromosome!=NULL)
    die("This entity already contains chromosomes.");

  embryo->chromosome = s_malloc(sizeof(network_t *));
  embryo->chromosome[0] = NN_new(num_layers, neurons);

  return TRUE;
  }


/**********************************************************************
  nnevolve_chromosome_destructor()
  synopsis:     Chromosome destructor function.
  parameters:
  return:
  last updated: 29 Jan 2002
 **********************************************************************/

void nnevolve_chromosome_destructor(const population *pop, entity *corpse)
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

void nnevolve_chromosome_replicate( const population *pop,
                                    entity *src, entity *dest,
                                    const int chromosomeid )
  {

  if (!pop) die("Null pointer to population structure passed.");
  if (!src || !dest) die("Null pointer to entity structure passed.");
  if (!src->chromosome || !dest->chromosome) die("Entity has no chromosomes.");
  if (!src->chromosome[0] || !dest->chromosome[0]) die("Entity has empty chromosomes.");
  if (chromosomeid != 0) die("Invalid chromosome index passed.");

  NN_copy((network_t *)src->chromosome[0],
           (network_t *)dest->chromosome[0]);

  return;
  }


/**********************************************************************
  nnevolve_chromosome_to_bytes()
  synopsis:     Convert chromosome to contiguous form.
  		FIXME: incorrect.
  parameters:
  return:       Number of bytes processed.
  last updated: 24 Dec 2002
 **********************************************************************/

unsigned int nnevolve_chromosome_to_bytes( const population *pop,
                                      entity *joe,
                                      byte **bytes,
                                      unsigned int *max_bytes )
  {
  unsigned int	num_bytes;	/* Actual size of genes. */

  dief("Function not implemented");

  if (!pop) die("Null pointer to population structure passed.");
  if (!joe) die("Null pointer to entity structure passed.");

  if (*max_bytes!=0) die("Internal error.");

  if (!joe->chromosome || !joe->chromosome[0]) die("Entity has no chromosome.");

  num_bytes = (unsigned int) sizeof(network_t);

  *bytes = (byte *)joe->chromosome[0];

  return num_bytes;
  }


/**********************************************************************
  nnevolve_chromosome_from_bytes()
  synopsis:     Convert contiguous form to chromosome.
  		FIXME: incorrect.
  parameters:
  return:
  last updated: 29 Jan 2002
 **********************************************************************/

void nnevolve_chromosome_from_bytes(const population *pop, entity *joe, byte *bytes)
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
  		This function is not really needed in this application.
  parameters:
  return:
  last updated: 21 Aug 2002
 **********************************************************************/

char *nnevolve_chromosome_to_string(
                              const population *pop, const entity *joe,
                              char *text, size_t *textlen)
  {

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
  updated:	29 Apr 2002
 **********************************************************************/

void nnevolve_setup_data(void)
  {
  int		i, j;
  float		min_data[13],max_data[13];

#include "wine.data"	/* This is a quick kludge! */

  train_property = s_malloc(sizeof(float *)*179);
  train_data = s_malloc(sizeof(float *)*179);

/*
 * Transfer data into global data arrays.
 * Then normalize the data.
 */
  for (j=0; j<13; j++)
    {
    max_data[j] = min_data[j] = data[0][j+3];
    }

  for (i=0; i<178; i++)
    {
    train_property[num_train_data] = s_malloc(sizeof(float)*4);
    train_data[num_train_data] = s_malloc(sizeof(float)*14);

    for (j=0; j<4; j++)
      {
      train_property[num_train_data][j] = data[num_train_data][j];
      }
    for (j=0; j<13; j++)
      {
      train_data[num_train_data][j] = data[num_train_data][j+3];

      if (max_data[j]<train_data[num_train_data][j])
        max_data[j]=train_data[num_train_data][j];
      else if (min_data[j]>train_data[num_train_data][j])
        min_data[j]=train_data[num_train_data][j];
      }
    num_train_data++;
    }

  for (j=0; j<13; j++)
    {
    for (i=0; i<178; i++)
      {
      train_data[i][j] = (train_data[i][j]-min_data[j])/(max_data[j]-min_data[j]);
      }
   }

  return;
  }


/**********************************************************************
  nnevolve_display_message()
  synopsis:	Display an introductory message.
  parameters:	none
  return:	none
  updated:	09 Aug 2002
 **********************************************************************/

void nnevolve_display_message(void)
  {

  printf("nnevolve - GAUL example: Evolve a fixed topology neural network.\n");
  printf("Copyright ©2002-2003, The Regents of the University of California.\n");
  printf("Primary author: \"Stewart Adcock\" <stewart@linux-domain.com>\n");
  printf("\n");
  printf("This program is distributed under the terms of the GNU General\n");
  printf("Public License.  See documentation for further details.\n");
  printf("\n");

  return;
  }


/**********************************************************************
  main()
  synopsis:	Evolve a fixed topology neural network.
  parameters:
  return:	2, on success.
  updated:	09 Aug 2002
 **********************************************************************/

int main(int argc, char **argv)
  {
  population	*pop=NULL;	/* Population of solutions. */
  entity	*entity=NULL;	/* Used to test standard back-prop. */
  chrono_t	lga_timer, bp_timer;	/* Timers. */

/*
 * Initialise logging.
 */
  log_set_level(LOG_NORMAL);

/*
 * Display an introductory message.
 */
  nnevolve_display_message();

/*
 * Initialize random number generator.
 */
  random_seed(20022002);

/*
 * Allocate a new popuation structure.
 * stable num. individuals = 50
 * num. chromosomes        = 1
 * length of chromosomes   = 0 (This is ignored by the constructor)
 */
  pop = ga_population_new( 50, 1, 0 );
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

/*
  pop->evaluate = nnevolve_evaluate_all;
  pop->evaluate = nnevolve_evaluate1;
*/
  pop->evaluate = nnevolve_evaluate2;
  pop->seed = nnevolve_seed;
  pop->adapt = nnevolve_adapt;
/*
  pop->select_one = ga_select_one_bestof2;
  pop->select_two = ga_select_two_bestof2;
*/
  pop->select_one = ga_select_one_sus;
  pop->select_two = ga_select_two_sus;
  pop->mutate = nnevolve_mutate;
/*
  pop->crossover = nnevolve_crossover_layerwise;
  pop->crossover = nnevolve_crossover_in;
*/
  pop->crossover = nnevolve_crossover_out;
  pop->replace = NULL;

/*
 * Seed the initial population.
 */
  ga_population_seed(pop);

/*
 * Set the GA parameters:
 * Evolutionary scheme = Lamarckian adaptation on entire population.
 * Elitism scheme      = Parents may pass to next generation.
 * Crossover ratio     = 0.20
 * Mutation ratio      = 0.10
 * Migration ration    = 0.0
 */
  ga_population_set_parameters( pop, GA_SCHEME_LAMARCK_ALL, GA_ELITISM_PARENTS_DIE, 0.20, 0.10, 0.0 );

/*
 * Setup the data for NN simulation.
 */
  nnevolve_setup_data();
  NN_define_train_data(num_train_data, train_data, train_property);

/*
 * Perform the Lamarckian evolution for 100 generations.
 */
  timer_start(&lga_timer);
  ga_evolution( pop, 100 );
  timer_check(&lga_timer);

  printf("The fitness of the final solution found was: %f\n",
		 ga_entity_get_fitness(ga_get_entity_from_rank(pop,0)) );
  
  nnevolve_display_evaluation(pop, ga_get_entity_from_rank(pop,0));

/*
 * Write the best solution to disk.
 */
  NN_write((network_t *)ga_get_entity_from_rank(pop,0)->chromosome[0], "ga_best.nn");

/*
 * For comparison, try standard back-propagation.
 */
  entity = ga_get_free_entity(pop);
  NN_randomize_weights((network_t *)entity->chromosome[0], -0.8, 0.8);
  NN_set_momentum((network_t *)entity->chromosome[0], 0.5);
  NN_set_rate((network_t *)entity->chromosome[0], 0.3);
  NN_set_gain((network_t *)entity->chromosome[0], 1.0);
  NN_set_bias((network_t *)entity->chromosome[0], 1.0);

  timer_start(&bp_timer);
  NN_train_random((network_t *)entity->chromosome[0], 10000);
  timer_check(&bp_timer);

  ga_entity_evaluate(pop, entity);
  nnevolve_display_evaluation(pop, entity);

  NN_write((network_t *)entity->chromosome[0], "bp_best.nn");

  ga_extinction(pop);

  exit(EXIT_SUCCESS);
  }


