/**********************************************************************
  nn_util.h
 **********************************************************************

  nn_util - Simple multi-layer Neural Network routines.
  Copyright ©2001-2002, The Regents of the University of California.
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

  Synopsis:	Multi-layer NN trained using backpropagation with
		momentum.

  Last Updated:	28 Jan 2002 SAA changes for distribution with GAUL.
  		25 Jan 2002 SAA	Header file stuff split from nn.c.  By default, standalone code is not compiled - change required for incorporation into GAUL example directory.

 **********************************************************************/

#ifndef NN_UTIL_H_INCLUDED
#define NN_UTIL_H_INCLUDED

/*
 * Includes.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "SAA_header.h"

#ifndef NN_DEBUG
# ifdef DEBUG
#  define NN_DEBUG DEBUG
# else
#  define NN_DEBUG 0
# endif
#endif

#include "memory_util.h"
#include "str_util.h"
#include "random_util.h"

/*
 * The NN layer data structure.  This is used to represent a single
 * layer of neurons.
 */
typedef struct
  {
  int        neurons;         /* Number of neurons in this layer. */
  float      *output;         /* Neuron's output. */
  float      *error;          /* Node error factor. */
  float      **weight;        /* Synapse weights. */
  float      **weight_save;   /* Saved weights for stopped training. */
  float      **weight_change; /* Last weight deltas for momentum. */
  } layer_t;

/*
 * The NN data structure.  This is used to represent a Neural Network
 * which consists of an arbitrary number of layers.
 */
typedef struct
  {
  float      momentum;        /* Momentum factor. */
  float      rate;            /* Learning rate. */
  float      gain;            /* Gain of sigmoidal function. */
  float      bias;            /* Network bias. */
  float      error;           /* Total network error. */
  layer_t    *layer;          /* Layers of neurons. */
  int        num_layers;      /* Number of layers of neurons (incl. input_output). */
  } network_t;

/*
 * Compilation constants.
 */
#define NN_MAX_FNAME_LEN	128
#define NN_DATA_ALLOC_SIZE	1024

/*
 * Default parameter constants.
 */
#define NN_SIGNAL_OFF		0.1
#define NN_SIGNAL_ON		0.9
#define NN_DEFAULT_BIAS		1.0

#define NN_DEFAULT_SEED		42

#define NN_DEFAULT_MOMENTUM	0.75
#define NN_DEFAULT_RATE		0.1
#define NN_DEFAULT_GAIN		1.0

#define NN_DEFAULT_MAX_EPOCHS	10000
#define NN_DEFAULT_TEST_STEP	20
#define NN_DEFAULT_STOP_RATIO	1.25


/*
 * Prototypes.
 *
 * I wouldn't anticipate that all of these are directly useful, but
 * they are available for the slight chance that you wish to create
 * your own training scheme.
 */

void NN_diagnostics(void);
void NN_display_summary(network_t *network);
network_t *NN_new(int num_layers, int *neurons);
void NN_set_bias(network_t *network, const float bias);
void NN_set_gain(network_t *network, const float gain);
void NN_set_rate(network_t *network, const float rate);
void NN_set_momentum(network_t *network, const float momentum);
void NN_write(network_t *network, char *fname);
network_t *NN_read(char *fname);
void NN_destroy(network_t *network);
void NN_randomize_weights(network_t *network);
void NN_randomize_weights_01(network_t *network);
void NN_input(network_t *network, float *input);
void NN_output(network_t *network, float *output);
void NN_save_weights(network_t *network);
void NN_restore_weights(network_t *network);
void NN_propagate(network_t *network);
void NN_output_error(network_t *network, float *target);
void NN_backpropagate(network_t *network);
void NN_adjust_weights(network_t *network);
void NN_simulate(network_t *network, float *input, float *target);
void NN_simulate_with_output(network_t *network, float *input, float *target, float *output);
void NN_simulate_with_output(network_t *network, float *input, float *output);
void NN_train_random(network_t *network, const int num_epochs);
void NN_train_systematic(network_t *network, const int num_epochs);
void NN_test(network_t *network, float *trainerror, float *testerror);
void NN_evaluate(network_t *network);
void NN_predict(network_t *network);
int read_fingerprint_binary_header(FILE *fp);
void read_prop(char *fname, float ***data, char ***labels, int *num_prop, int *num_data, int dimensions);
#ifdef NN_STANDALONE
void write_usage(void);
int main(int argc, char **argv);
#endif /* NN_STANDALONE */

#endif /* NN_UTIL_H_INCLUDED */

