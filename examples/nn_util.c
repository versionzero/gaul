/**********************************************************************
  nn_util.c
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

		Warning: This code contains almost no error checking!

		This code uses neuronal input/response in the range 0.0>=x>=1.0.

		For a standalone version, compile with something like:
		gcc protnn.c -o protnn -g -O2 \
			-DNN_STANDALONE \
			-Lmethods/ -I. -I.. -Imethods/ -Imolstruct/ \
                        -lm -lstr_util -lmethods -lrandom -Wall

  Last Updated:	25 Jan 2002 SAA	By default, standalone code is not compiled - change required for incorporation into GAUL example directory.  Renamed to nn_util.c and split off a nn_util.h file.  NN_diagnostics() added.  Renamed some defines for consistency.
		24 Dec 2002 SAA Removed stupid error calculation from NN_predict().
		12 Dec 2001 SAA Fixed read_prop() bug.
		10 Dec 2001 SAA read_prop() now reads data with variable number of fields.  Default behaviour is to initialize weights within the range 0.0-1.0.  Comma delimited data is now usable.
  		05 Dec 2001 SAA	Read any size fingerprint.
		04 Dec 2001 SAA Completed usage message.  Defining layers on command-line.  NN_display_summary() added.
		03 Dec 2001 SAA Substantial generalisations.  More comments.  New I/O functions.  Initial support for labelled data.
		30 Nov 2001 SAA	Changes for improved clarity.
		29 Nov 2001 SAA Renamed and some comments added.  Some code generalised.  Introduced use of memory_util library.  All this is in preparation of optimisation using GAUL.

  To do:	Remove global variables, ready for making thread-safe.

 **********************************************************************/

#include "nn_util.h"

/*
 * Yucky global variables.
 */
float      **train_data=NULL;       /* Input data for training. */
float      **test_data=NULL;        /* Input data for testing. */
float      **eval_data=NULL;        /* Input data for evaluation. */
int        num_train_data=0;        /* Number of training target items. */
int        num_test_data=0;         /* Number of testing target items. */
int        num_eval_data=0;         /* Number of evaluation target items. */
int        max_train_data=0;        /* Maximum number of training target items. */
int        max_test_data=0;         /* Maximum number of testing target items. */
int        max_eval_data=0;         /* Maximum number of evaluation target items. */

float      **train_property=NULL;   /* Training target property. */
float      **test_property=NULL;    /* Testing target property. */
float      **eval_property=NULL;    /* Evaluation target property. */
int        num_train_prop=0;        /* Number of training target properties. */
int        num_test_prop=0;         /* Number of testing target properties. */
int        num_eval_prop=0;         /* Number of evaluation target properties. */

float      **predict_data=NULL;     /* Input data for prediction. */
int        num_predict_data=0;      /* Number of sets of input data to predict. */
int        max_predict_data=0;      /* Maximum number of sets of input data to predict. */

char       **train_labels=NULL;     /* Labels for training data. */
char       **test_labels=NULL;      /* Labels for test data. */
char       **eval_labels=NULL;      /* Labels for evaluation data. */
char       **predict_labels=NULL;   /* Labels for prediction data. */


/**********************************************************************
  NN_diagnostics()
  synopsis:     Display diagnostic information. 
  parameters:   none
  return:       none
  last updated: 25 Jan 2002
 **********************************************************************/

void NN_diagnostics(void)
  {

  printf("=== nn_util diagnostic information ===========================\n");
  printf("Build date:                %s\n", BUILD_DATE_STRING);
  printf("NN_DEBUG:                  %d\n", NN_DEBUG);
  printf("NN_MAX_FNAME_LEN:          %d\n", NN_MAX_FNAME_LEN);
  printf("NN_DATA_ALLOC_SIZE:        %d\n", NN_DATA_ALLOC_SIZE);
  printf("NN_SIGNAL_OFF:             %d\n", NN_SIGNAL_OFF);
  printf("NN_SIGNAL_ON:              %d\n", NN_SIGNAL_ON);
  printf("NN_DEFAULT_BIAS:           %d\n", NN_DEFAULT_BIAS);
  printf("NN_DEFAULT_SEED:           %d\n", NN_DEFAULT_SEED);
  printf("NN_DEFAULT_MOMENTUM:       %d\n", NN_DEFAULT_MOMENTUM);
  printf("NN_DEFAULT_RATE:           %d\n", NN_DEFAULT_RATE);
  printf("NN_DEFAULT_GAIN:           %d\n", NN_DEFAULT_GAIN);
  printf("NN_DEFAULT_MAX_EPOCHS:     %d\n", NN_DEFAULT_MAX_EPOCHS);
  printf("NN_DEFAULT_TEST_STEP:      %d\n", NN_DEFAULT_TEST_STEP);
  printf("NN_DEFAULT_STOP_RATIO:     %d\n", NN_DEFAULT_STOP_RATIO);
  printf("--------------------------------------------------------------\n");
  printf("structure                  sizeof\n");
  printf("layer_t:                   %Zd\n", sizeof(layer_t));
  printf("network_t:                 %Zd\n", sizeof(network_t));
  printf("--------------------------------------------------------------\n");

  return;
  }


/**********************************************************************
  NN_display_summary()
  synopsis:     Display a summary of a Neural Network datastructure.
  parameters:   network_t *network
  return:       none
  last updated: 04 Dec 2001
 **********************************************************************/

void NN_display_summary(network_t *network)
  {
  int		l;		/* Layer index. */

  printf("num_layers = %d num_neurons =", network->num_layers);

  for (l=0; l<network->num_layers; l++)
    printf(" %d", network->layer[l].neurons);

  printf("\nmomentum = %f rate = %f gain = %f bias = %f\n",
             network->momentum, network->rate, network->gain, network->bias);

  return;
  }


/**********************************************************************
  NN_new()
  synopsis:     Allocate and initialise a Neural Network datastructure.
  parameters:   int num_layers	Number of layers (incl. input+output)
		int *neurons	Array containing number of nodes per layer.
  return:       network_t *network
  last updated: 29 Nov 2001
 **********************************************************************/

network_t *NN_new(int num_layers, int *neurons)
  {
  network_t	*network;	/* The new network. */
  int		l;		/* Layer index. */
  int		i;		/* Neuron index. */

  network = (network_t*) s_malloc(sizeof(network_t));
  network->layer = (layer_t*) s_malloc(num_layers*sizeof(layer_t));
  network->num_layers = num_layers;

  network->layer[0].neurons     = neurons[0];
  network->layer[0].output      = (float*) s_calloc(neurons[0]+1, sizeof(float));
  network->layer[0].error       = (float*) s_calloc(neurons[0]+1, sizeof(float));
  network->layer[0].weight      = NULL;
  network->layer[0].weight_save  = NULL;
  network->layer[0].weight_change = NULL;
  network->layer[0].output[0]   = NN_DEFAULT_BIAS;
   
  for (l=1; l<num_layers; l++)
    {
    network->layer[l].neurons     = neurons[l];
    network->layer[l].output      = (float*)  s_calloc(neurons[l]+1, sizeof(float));
    network->layer[l].error       = (float*)  s_calloc(neurons[l]+1, sizeof(float));
    network->layer[l].weight      = (float**) s_calloc(neurons[l]+1, sizeof(float*));
    network->layer[l].weight_save  = (float**) s_calloc(neurons[l]+1, sizeof(float*));
    network->layer[l].weight_change = (float**) s_calloc(neurons[l]+1, sizeof(float*));
    network->layer[l].output[0]   = NN_DEFAULT_BIAS;
      
    for (i=1; i<=neurons[l]; i++)
      {
      network->layer[l].weight[i]      = (float*) s_calloc(neurons[l-1]+1, sizeof(float));
      network->layer[l].weight_save[i]  = (float*) s_calloc(neurons[l-1]+1, sizeof(float));
      network->layer[l].weight_change[i] = (float*) s_calloc(neurons[l-1]+1, sizeof(float));
      }
    }

/* Tuneable parameters: */
  network->momentum = NN_DEFAULT_MOMENTUM;
  network->rate = NN_DEFAULT_RATE;
  network->gain = NN_DEFAULT_GAIN;
  network->bias = NN_DEFAULT_BIAS;

  return network;
  }


/**********************************************************************
  NN_set_bias()
  synopsis:     Change the bias of a network to a given value.
  parameters:   network_t *network
		float    bias
  return:       none
  last updated: 3 Dec 2001
 **********************************************************************/

void NN_set_bias(network_t *network, float bias)
  {
  int l; 	/* Loop variable over layers. */

  if (network->bias != bias)
    {
    network->bias = bias;

    for (l=0; l<network->num_layers; l++)
      network->layer[l].output[0] = bias;
    }

  return;
  }


/**********************************************************************
  NN_set_gain()
  synopsis:     Change the gain of a network to a given value.
  parameters:   network_t *network
		float    gain
  return:       none
  last updated: 3 Dec 2001
 **********************************************************************/

void NN_set_gain(network_t *network, float gain)
  {

  network->gain = gain;

  return;
  }


/**********************************************************************
  NN_set_rate()
  synopsis:     Change the learning rate of a network to a given value.
  parameters:   network_t *network
		float    rate
  return:       none
  last updated: 3 Dec 2001
 **********************************************************************/

void NN_set_rate(network_t *network, float rate)
  {

  network->rate = rate;

  return;
  }


/**********************************************************************
  NN_set_momentum()
  synopsis:     Change the momentum of a network to a given value.
  parameters:   network_t *network
		float    momentum
  return:       none
  last updated: 3 Dec 2001
 **********************************************************************/

void NN_set_momentum(network_t *network, float momentum)
  {

  network->momentum = momentum;

  return;
  }


/**********************************************************************
  NN_write()
  synopsis:     Write a network_t structure and its contents to disk
		in a binary format.
  parameters:   network_t *network
  return:       none
  last updated: 30 Nov 2001
 **********************************************************************/

void NN_write(network_t *network, char *fname)
  {
  FILE		*fp;				/* File handle. */
  char		*fmt_str="FORMAT NN: 001\n";	/* File identifier tag. */
  int		l;				/* Layer index. */
  int		i;				/* Neuron index. */

  if ( !(fp = fopen(fname, "w")) ) dief("Unable to open file \"%s\" for output.\n", fname);

  fwrite(fmt_str, sizeof(char), strlen(fmt_str), fp);

  fwrite(&(network->momentum), sizeof(float), 1, fp);
  fwrite(&(network->gain), sizeof(float), 1, fp);
  fwrite(&(network->rate), sizeof(float), 1, fp);
  fwrite(&(network->bias), sizeof(float), 1, fp);

  fwrite(&(network->num_layers), sizeof(int), 1, fp);

  for (l=0;l<network->num_layers; l++)
    {
    fwrite(&(network->layer[l].neurons), sizeof(int), 1, fp);
    }

  for (l=1; l<network->num_layers; l++)
    {
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      fwrite(network->layer[l].weight[i], sizeof(float), network->layer[l-1].neurons, fp);
      }
    }

  fclose(fp);

  return;
  }


/**********************************************************************
  NN_read()
  synopsis:     Read (and allocate) a network_t structure and its
		contents from a binary format file on disk.
  parameters:   network_t *network
  return:       none
  last updated: 30 Nov 2001
 **********************************************************************/

network_t *NN_read(char *fname)
  {
  FILE		*fp;				/* File handle. */
  char		*fmt_str="FORMAT NN: 001\n";	/* File identifier tag. */
  char		*fmt_str_in="                ";	/* File identifier tag. */
  network_t	*network;			/* The new network. */
  int		l;				/* Layer index. */
  int		i;				/* Neuron index. */

  if ( !(fp = fopen(fname, "r")) ) dief("Unable to open file \"%s\" for input.\n", fname);

  fread(fmt_str_in, sizeof(char), strlen(fmt_str), fp);
  
  if (strncmp(fmt_str, fmt_str_in, strlen(fmt_str)))
    die("Invalid neural network file header")

  network = (network_t*) s_malloc(sizeof(network_t));

  fread(&(network->momentum), sizeof(float), 1, fp);
  fread(&(network->gain), sizeof(float), 1, fp);
  fread(&(network->rate), sizeof(float), 1, fp);
  fread(&(network->bias), sizeof(float), 1, fp);

  fread(&(network->num_layers), sizeof(int), 1, fp);
  network->layer = (layer_t*) s_malloc(network->num_layers*sizeof(layer_t));

  fread(&(network->layer[0].neurons), sizeof(int), 1, fp);
  network->layer[0].output      = (float*) s_calloc(network->layer[0].neurons+1, sizeof(float));
  network->layer[0].error       = (float*) s_calloc(network->layer[0].neurons+1, sizeof(float));
  network->layer[0].weight      = NULL;
  network->layer[0].weight_save  = NULL;
  network->layer[0].weight_change = NULL;
  network->layer[0].output[0]   = network->bias;
   
  for (l=1; l<network->num_layers; l++)
    {
    fread(&(network->layer[l].neurons), sizeof(float), 1, fp);
    network->layer[l].output      = (float*)  s_calloc(network->layer[l].neurons+1, sizeof(float));
    network->layer[l].error       = (float*)  s_calloc(network->layer[l].neurons+1, sizeof(float));
    network->layer[l].weight      = (float**) s_calloc(network->layer[l].neurons+1, sizeof(float*));
    network->layer[l].weight_save  = (float**) s_calloc(network->layer[l].neurons+1, sizeof(float*));
    network->layer[l].weight_change = (float**) s_calloc(network->layer[l].neurons+1, sizeof(float*));
    network->layer[l].output[0]   = network->bias;
      
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      network->layer[l].weight[i]      = (float*) s_calloc(network->layer[l-1].neurons+1, sizeof(float));
      fread(network->layer[l].weight[i], sizeof(float), network->layer[l-1].neurons, fp);
      network->layer[l].weight_save[i]  = (float*) s_calloc(network->layer[l-1].neurons+1, sizeof(float));
      network->layer[l].weight_change[i] = (float*) s_calloc(network->layer[l-1].neurons+1, sizeof(float));
      }
    }

  fclose(fp);

  return network;
  }


/**********************************************************************
  NN_destroy()
  synopsis:     Deallocate a network_t structure and its contents.
  parameters:   network_t *network
  return:       none
  last updated: 29 Nov 2001
 **********************************************************************/

void NN_destroy(network_t *network)
  {
  int l,i;

  for (l=0; l<network->num_layers; l++)
    {
    if (l != 0)
      {
      for (i=1; i<=network->layer[l].neurons; i++)
        {
        s_free(network->layer[l].weight[i]);
        s_free(network->layer[l].weight_save[i]);
        s_free(network->layer[l].weight_change[i]);
        }

      s_free(network->layer[l].output);
      s_free(network->layer[l].error);
      s_free(network->layer[l].weight);
      s_free(network->layer[l].weight_save);
      s_free(network->layer[l].weight_change);
      }
    }

  s_free(network->layer);
  s_free(network);

  return;
  }


/**********************************************************************
  NN_randomize_weights()
  synopsis:     Randomize the weights of all neurons in a network.
		Random values selected from a linear distribution
		between -1.0 and 1.0
  parameters:   network_t *network
  return:       none
  last updated: 29 Nov 2001
 **********************************************************************/

void NN_randomize_weights(network_t *network)
  {
  int l,i,j;
   
  for (l=1; l<network->num_layers; l++)
    {
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      for (j=0; j<=network->layer[l-1].neurons; j++)
        {
        network->layer[l].weight[i][j] = random_float_range(-1.0, 1.0);
        }
      }
    }

  return;
  }


/**********************************************************************
  NN_randomize_weights_01()
  synopsis:     Randomize the weights of all neurons in a network.
		Random values selected from a linear distribution
		between 0.0 and 1.0
  parameters:   network_t *network
  return:       none
  last updated: 3 Dec 2001
 **********************************************************************/

void NN_randomize_weights_01(network_t *network)
  {
  int l,i,j;
   
  for (l=1; l<network->num_layers; l++)
    {
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      for (j=0; j<=network->layer[l-1].neurons; j++)
        {
        network->layer[l].weight[i][j] = random_float(1.0);
        }
      }
    }

  return;
  }


/**********************************************************************
  NN_input()
  synopsis:     Write input values into network.
  parameters:   network_t *network
		float *input
  return:       none
  last updated: 
 **********************************************************************/

void NN_input(network_t *network, float *input)
  {
  int i;
   
  for (i=1; i<=network->layer[0].neurons; i++)
    {
    network->layer[0].output[i] = input[i-1];
    }

  return;
  }


/**********************************************************************
  NN_output()
  synopsis:     Read output values from network.
  parameters:   network_t *network
		float *input
  return:       none
  last updated: 
 **********************************************************************/

void NN_output(network_t *network, float *output)
  {
  int i;
   
  for (i=1; i<=network->layer[network->num_layers-1].neurons; i++)
    {
    output[i-1] = network->layer[network->num_layers-1].output[i];
    }

  return;
  }


/**********************************************************************
  NN_save_weights()
  synopsis:     Internally save the weights in a network.
  parameters:   network_t *network
  return:       none
  last updated: 
 **********************************************************************/

void NN_save_weights(network_t *network)
  {
  int l,i,j;

  for (l=1; l<network->num_layers; l++)
    {
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      for (j=0; j<=network->layer[l-1].neurons; j++)
        {
        network->layer[l].weight_save[i][j] = network->layer[l].weight[i][j];
        }
      }
    }

  return;
  }


/**********************************************************************
  NN_restore_weights()
  synopsis:     Restore internally saved weights in a network.
  parameters:   network_t *network
  return:       none
  last updated: 
 **********************************************************************/

void NN_restore_weights(network_t *network)
  {
  int l,i,j;

  for (l=1; l<network->num_layers; l++)
    {
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      for (j=0; j<=network->layer[l-1].neurons; j++)
        {
        network->layer[l].weight[i][j] = network->layer[l].weight_save[i][j];
        }
      }
    }

  return;
  }


/**********************************************************************
  NN_propagate()
  synopsis:     Propagate errors through network.
  parameters:   network_t *network
  return:       none
  last updated: 
 **********************************************************************/

void NN_propagate(network_t *network)
  {
  int l;
  int  i,j;
  float sum;
   
  for (l=0; l<network->num_layers-1; l++)
    {
    for (i=1; i<=network->layer[l+1].neurons; i++)
      {
      sum = 0;
      for (j=0; j<=network->layer[l].neurons; j++)
        {
        sum += network->layer[l+1].weight[i][j] * network->layer[l].output[j];
        }
      network->layer[l+1].output[i] = 1 / (1 + exp(-network->gain * sum));
      }
    }

  return;
  }


/**********************************************************************
  NN_output_error()
  synopsis:     Assess the error of a network against a given output
		vector.
  parameters:   network_t *network
		float *target
  return:       none
  last updated: 
 **********************************************************************/

void NN_output_error(network_t *network, float *target)
  {
  int  i;
  float out, err;
   
  network->error = 0;
  for (i=1; i<=network->layer[network->num_layers-1].neurons; i++)
    {
    out = network->layer[network->num_layers-1].output[i];
    err = target[i-1]-out;
    network->layer[network->num_layers-1].error[i] = network->gain * out * (1-out) * err;
    network->error += 0.5 * SQU(err);
    }

  return;
  }


/**********************************************************************
  NN_backpropagate()
  synopsis:     Perform one step of error back-propagation.
  parameters:   network_t *network
  return:       none
  last updated:
 **********************************************************************/

void NN_backpropagate(network_t *network)
  {
  int l;
  int  i,j;
  float out, err;
   
  for (l=network->num_layers-1; l>1; l--)
    {
    for (i=1; i<=network->layer[l-1].neurons; i++)
      {
      out = network->layer[l-1].output[i];
      err = 0;
      for (j=1; j<=network->layer[l].neurons; j++)
        {
        err += network->layer[l].weight[j][i] * network->layer[l].error[j];
        }
      network->layer[l-1].error[i] = network->gain * out * (1-out) * err;
      }
    }

  return;
  }


/**********************************************************************
  NN_adjust_weights()
  synopsis:     Modify network weights according to back-propagated
		error.
  parameters:   network_t *network
  return:       none
  last updated:
 **********************************************************************/

void NN_adjust_weights(network_t *network)
  {
  int  l,i,j;
  float out, err;
   
  for (l=1; l<network->num_layers; l++)
    {
    for (i=1; i<=network->layer[l].neurons; i++)
      {
      for (j=0; j<=network->layer[l-1].neurons; j++)
        {
        out = network->layer[l-1].output[j];
        err = network->layer[l].error[i];
        network->layer[l].weight[i][j] += network->rate * err * out
                                        + network->momentum * network->layer[l].weight_change[i][j];
        network->layer[l].weight_change[i][j] = network->rate * err * out;
        }
      }
    }

  return;
  }


/**********************************************************************
  NN_simulate()
  synopsis:     Training simulation.
  parameters:   network_t *network
		float *input
		float *target
  return:       none
  last updated:
 **********************************************************************/

void NN_simulate(network_t *network, float *input, float *target)
  {
/*int i;  debug */

  NN_input(network, input);
  NN_propagate(network);
   
  NN_output_error(network, target);

/* Debug:
  for (i=1; i<=network->layer[network->num_layers-1].neurons; i++)
    printf("%f ", network->layer[network->num_layers-1].output[i]);
  printf("\n");
*/

  return;
  }


/**********************************************************************
  NN_simulate_with_output()
  synopsis:     Training simulation which also returns the output
		vector.
  parameters:   network_t *network
		float *input
		float *target
		float *output
  return:       none
  last updated:
 **********************************************************************/

void NN_simulate_with_output(network_t *network, float *input, float *target, float *output)
  {

  NN_input(network, input);
  NN_propagate(network);
  NN_output(network, output);
   
  NN_output_error(network, target);

  return;
  }


/**********************************************************************
  NN_run()
  synopsis:     Prediction simulation.
  parameters:   network_t *network
		float *input
		float *output
  return:       none
  last updated:
 **********************************************************************/

void NN_simulate_with_output(network_t *network, float *input, float *output)
  {

  NN_input(network, input);
  NN_propagate(network);
  NN_output(network, output);
   
  return;
  }


/**********************************************************************
  NN_train()
  synopsis:     Train network using back-propagation.
  parameters:   network_t *network
		int num_epochs
  return:       none
  last updated:
 **********************************************************************/

void NN_train(network_t *network, int num_epochs)
  {
  int  item, n;

  for (n=0; n<num_epochs*num_train_data; n++)
    {
    item = random_int(num_train_data);
    NN_simulate(network, train_data[item], train_property[item]);

    NN_backpropagate(network);
    NN_adjust_weights(network);
    }
 
  return;
  }


/**********************************************************************
  NN_test()
  synopsis:     Test network.
  parameters:   network_t *network
		float *trainerror
		float *testerror
  return:       none
  last updated:
 **********************************************************************/

void NN_test(network_t *network, float *trainerror, float *testerror)
  {
  int  item;

  *trainerror = 0;
  for (item=0; item<num_train_data; item++)
    {
    NN_simulate(network, train_data[item], train_property[item]);
    *trainerror += network->error;
    }
  *trainerror /= num_train_data;

  *testerror = 0;
  for (item=0; item<num_test_data; item++)
    {
    NN_simulate(network, test_data[item], test_property[item]);
    *testerror += network->error;
    }
  *testerror /= num_test_data;

  return;
  }


/**********************************************************************
  NN_evaluate()
  synopsis:     Evaluate network and write result.
  parameters:   network_t *network
  return:       none
  last updated:
 **********************************************************************/

void NN_evaluate(network_t *network)
  {
  int		i;		/* Loop variable over output neurons. */
  int		item;		/* Loop variable over evaluation data. */
  float	*output;	/* Output results. */
  float	evalerror=0;	/* Network's output error. */

  output = (float *) s_malloc(network->layer[network->num_layers-1].neurons*sizeof(float));

  printf("\n\nItem  Field  Actual  Prediction\n\n");
  for (item=0; item<num_eval_data; item++)
    {
    NN_simulate_with_output(network, eval_data[item], eval_property[item], output);
    evalerror += network->error;
    printf("%4d  0    %0.4f  %0.4f\n",
           item, eval_property[item][0], output[0]);
    for (i=1; i<network->layer[network->num_layers-1].neurons; i++)
      {
      printf("     %3d  %0.4f  %0.4f\n",
             i, eval_property[item][i], output[i]);
      }
    }
  evalerror /= num_eval_data;

  printf("Error is %f on evaluation set.\n", evalerror);

  s_free(output);

  return;
  }


/**********************************************************************
  NN_predict()
  synopsis:     Use network to make predictions and write results.
  parameters:   network_t *network
  return:       none
  last updated:	24 Jan 2002
 **********************************************************************/

void NN_predict(network_t *network)
  {
  int		i;		/* Loop variable over output neurons. */
  int		item;		/* Loop variable over evaluation data. */
  float	*output;	/* Output results. */

  output = (float *) s_malloc(network->layer[network->num_layers-1].neurons*sizeof(float));

  printf("\n\nItem  Field  Prediction\n\n");
  for (item=0; item<num_eval_data; item++)
    {
    NN_run(network, eval_data[item], output);
    printf("%4d  0    %0.4f\n", item, output[0]);
    for (i=1; i<network->layer[network->num_layers-1].neurons; i++)
      {
      printf("     %3d  %0.4f\n", i, output[i]);
      }
    }

  s_free(output);

  return;
  }


/**********************************************************************
  read_fingerprint_binary_header()
  synopsis:     Read binary fingerprint info from given filehandle.
                Designed for future expansion rather than current
                utility.
  parameters:   filehandle
  return:       int size
  last updated: 28 Nov 2001
 **********************************************************************/

int read_fingerprint_binary_header(FILE *fp)
  {
  char *fmt_str="FORMAT FP: 001\n";
  char fmt_str_in[16];
  int size;

  fread(fmt_str_in, sizeof(char), strlen(fmt_str), fp);

  if (strncmp(fmt_str, fmt_str_in, strlen(fmt_str)))
    die("Invalid fingerprint header");

  fread(&size, sizeof(int), 1, fp);

  return size;
  }


/**********************************************************************
  read_data()
  synopsis:     Read binary fingerprint with label from given file.
  parameters:   filename
                data
  return:       int size
  last updated: 3 Dec 2001
 **********************************************************************/

int read_data(char *fname, float ***data, char ***labels, int *num_data, int *max_data)
  {
  FILE  *fp;		/* Filehandle. */
  int   label_len;	/* Label length. */
  int	size;		/* Dimensions of fingerprint. */

  if ( !(fp = fopen(fname, "r")) ) dief("Unable to open file \"%s\" for input.\n", fname);

  size = read_fingerprint_binary_header(fp);	/* Check validity of file. */

  while (fread(&label_len, sizeof(int), 1, fp) > 0)
    {
    if (*num_data == *max_data)
      {
      *max_data += NN_DATA_ALLOC_SIZE;
      *data = (float **) s_realloc(*data, sizeof(float *) * *max_data);
      *labels = (char **) s_realloc(*labels, sizeof(char *) * *max_data);
      }

    (*labels)[*num_data] = (char *) s_malloc(sizeof(char)*label_len+1);
    fread((*labels)[*num_data], sizeof(char), label_len, fp);
    (*labels)[*num_data][label_len] = '\0';

    (*data)[*num_data] = (float *) s_malloc(sizeof(float)*size);	/* Fixme: Magic number. */
    fread((*data)[*num_data], sizeof(float), size, fp);	/* Fixme: Magic number. */

    (*num_data)++;
    }

  fclose(fp);

  return size;
  }


/**********************************************************************
  split_data()
  synopsis:     Split a single dataset into several datasets.
  parameters:
  return:       none.
  last updated: 10 Dec 2001
 **********************************************************************/

void split_data(float ***data1, float ***prop1, char ***labels1, int *num_data1, int *max_data1,
                float ***data2, float ***prop2, char ***labels2, int *num_data2, int *max_data2,
                float ***data3, float ***prop3, char ***labels3, int *num_data3, int *max_data3,
                int num2, int num3)
  {

  die("Function incomplete");

  return;
  }


/**********************************************************************
  read_comma_delimited_data()
  synopsis:     Read non-fuzzy data with no labels from given ASCII
		file.  First line specifies number of classes and
		number of parameters.
  parameters:   filename
                data
  return:       none.
  last updated: 10 Dec 2001
 **********************************************************************/

void read_comma_delimited_data(char *fname, float ***data, float ***prop, char ***labels, int *num_data, int *max_data)
  {
  FILE  *fp;    			/* Filehandle. */
  char  line_buffer[MAX_LINE_LEN];	/* Line buffer. */
  char  *line;                          /* Line pointer. */
  int   num_class=0;        		/* Dimensions of output data. */
  int   data_size=0;	        	/* Dimensions of input data. */
  int   label_len=4;		        /* Default label length. */
  int   data_count;	        	/* Current data field. */
  int   class;                          /* Current classification. */

  if ( !(fp = fopen(fname, "r")) )
    dief("Unable to open file \"%s\" for input.\n", fname);

  if (str_nreadline(fp, MAX_LINE_LEN, line_buffer)<1)
    dief("Unable to read from file \"%s\" for input.\n", fname);

  sscanf(line_buffer, "%d %d\n", &num_class, &data_size);

  while (str_nreadline(fp, MAX_LINE_LEN, line_buffer)>0)
    {
    line = line_buffer;

    if (*num_data == *max_data)
      {
      *max_data += NN_DATA_ALLOC_SIZE;
      *data = (float **) s_realloc(*data, sizeof(float *)*(*max_data));
      *prop = (float **) s_realloc(*prop, sizeof(float *)*(*max_data));
      *labels = (char **) s_realloc(*labels, sizeof(char *)*(*max_data));
      }

    (*labels)[*num_data] = (char *) s_malloc(sizeof(char)*label_len+1);
    (*data)[*num_data] = (float *) s_malloc(sizeof(float)*data_size);
    (*prop)[*num_data] = (float *) s_malloc(sizeof(float)*num_class);

    line = strtok(line, ",");
    class = atoi(line);

    snprintf((*labels)[*num_data], label_len, "%d", *num_data);
    (*labels)[*num_data][label_len] = '\0';

    for (data_count=0; data_count<num_class; data_count++ )
      {
      if (data_count == class)
        (*prop)[*num_data][data_count] = 1.0;
      else
        (*prop)[*num_data][data_count] = 0.0;
      }

    if (data_count!=num_class) die("Property size mismatch");

    for (data_count=0; data_count<data_size && (line = strtok(NULL, ","))!=NULL; data_count++ )
      {
      (*data)[*num_data][data_count] = atof(line);
      }

    if (data_count!=data_size) die("Data size mismatch");

    (*num_data)++;
    }

  fclose(fp);

  return;
  }


/**********************************************************************
  read_prop()
  synopsis:     Read properties from given file.
  parameters:   filename
                data
  return:       none.
  last updated: 10 Dec 2001
 **********************************************************************/

void read_prop(char *fname, float ***data, char ***labels, int *num_prop, int *num_data, int dimensions)
  {
  FILE  *fp;		        	/* Filehandle. */
  char  line_buffer[MAX_LINE_LEN];	/* Line buffer. */
  char  *line;                          /* Line pointer. */
  int   data_count;		        /* Number of fields input from current record. */

  if ( !(fp = fopen(fname, "r")) ) dief("Unable to open file \"%s\" for input.\n", fname);

  *data = (float **) s_realloc(*data, sizeof(float*)*(*num_data));

  while (str_nreadline(fp, MAX_LINE_LEN, line_buffer)>0)
    {
    if (*num_prop > *num_data) die("Too many property records input.");

    line = line_buffer;

    if (strncmp((*labels)[*num_prop], line, strlen((*labels)[*num_prop]))!=0)
      dief("Label mismatch \"%s\" to \"%s\"", (*labels)[*num_prop], line);

    (*data)[*num_prop] = (float *) s_malloc(dimensions*sizeof(float));

    line = strtok(&(line[strlen((*labels)[*num_prop])]), " ");
    (*data)[*num_prop][0] = (float) atof(line);
    data_count=1;

    while ( (line = strtok(NULL, " "))!=NULL )
      {
      if (data_count==dimensions) die("Too many data items.");

      (*data)[*num_prop][data_count] = (float) atof(line);

      data_count++;
      }

    if (data_count!=dimensions) dief("Too few data items (%d instead of %d).", data_count, dimensions);
/*
    sscanf(&(line[strlen((*labels)[*num_prop])]), "%f %f %f %f\n",
              &((*data)[*num_prop][0]), &((*data)[*num_prop][1]),
              &((*data)[*num_prop][2]), &((*data)[*num_prop][3]) );
*/

/* FIXME: Kludge required for unnormalized input data.
    (*data)[*num_prop][0] /= 360.0;
    (*data)[*num_prop][1] /= 360.0;
    (*data)[*num_prop][2] /= 360.0;
    (*data)[*num_prop][3] /= 360.0;
 */

    (*num_prop)++;
    }

  fclose(fp);

  return;
  }


#ifdef NN_STANDALONE
/**********************************************************************
  write_usage()
  synopsis:     Display usage details.
  parameters:   none
  return:       none
  updated:      3 Dec 2001
 **********************************************************************/

void write_usage(void)
  {

  printf("\n"
         "protnn - Neural network thing\n"
         "Copyright ©2001, \"Stewart Adcock\" <stewart@bellatrix.pcl.ox.ac.uk>\n"
         "\n"
         "protnn switches...\n"
         "\n"
         "Available switches are:\n"
         "    --version            Version information.\n"
         "    --[help|usage]       This stuff.\n"
         "    --seed INTEGER       PRNG seed value.\n"
         "    --epochs INTEGER     Maximum number of 'stopped-training' epochs.\n"
         "    --teststep INTEGER   Number of epochs per testing phase.\n"
         "    --stopfactor REAL    Stopped training factor.\n"
         "    --rate REAL          Learning rate.\n"
         "    --momentum REAL      Learning momentum.\n"
         "    --bias REAL          Network bias.\n"
         "    --gain REAL          Neuronal gain.\n"
         "    --readnn FILENAME    Read NN from file.\n"
         "    --writenn FILENAME   Write NN to file.\n"
         "    --nowritenn          Do not write NN to file.\n"
         "    --notrain            Do not train NN.\n"
         "    --noevaluate         Do not evaluate data with NN.\n"
         "    --predict            Perform prediction with NN.\n"
         "    --random01           NN weights initialized between 0.0 and 1.0. [default]\n"
         "    --random11           NN weights initialized between -1.0 and 1.0.\n"
         "    --layers INTEGER     Number of layers (incl. input+output).\n"
         "    --neurons INTEGER... Number of neurons in each layer.\n"
         "    --trainprop FILENAME Training properties file.\n"
         "    --testprop FILENAME  Testing properties file.\n"
         "    --evalprop FILENAME  Evaluation properties file.\n"
         "    --trainfp FILENAME   Training fingerprints file.\n"
         "    --testfp FILENAME    Testing fingerprints file.\n"
         "    --evalfp FILENAME    Evaluation fingerprints file.\n"
         "    --predictfp FILENAME Prediction fingerprints file.\n"
         "    --comma FILENAME     Comma-delimited input data file.\n"
         "\n");

  return;
  }


/**********************************************************************
  main()
  synopsis:     The main function.
  parameters:   int argc, char **argv
  return:       2
  last updated: 3 Dec 2001
 **********************************************************************/

int main(int argc, char **argv)
  {
  network_t  *network;				/* NN structure. */
  int        i, j;				/* Loop variables. */
  int        epoch;				/* Count of training cycles. */
  int        teststep=NN_DEFAULT_TEST_STEP;	/* Number of training epochs between each test step. */
  float      trainerror, testerror, mintesterror;	/* Error measurements. */
  int        seed=NN_DEFAULT_SEED;		/* PRNG seed. */
  boolean    do_readnn=FALSE, do_writenn=TRUE;	/* Whether to read/write NN. */
  boolean    do_train=TRUE, do_evaluate=TRUE;	/* Whether to train/evaluate NN. */
  boolean    do_predict=FALSE;			/* Whether to use NN for prediction. */
  int        num_layers=0;			/* Number of layers in NN. */
  int        *neurons=NULL;			/* Number of neurons in each layer. */
  float      stop_ratio=NN_DEFAULT_STOP_RATIO;	/* Stopping criterion. */
  int        max_epochs=NN_DEFAULT_MAX_EPOCHS;	/* Maximum training epochs. */
  boolean    do_randomize01=TRUE;		/* Initialize weights within range 0.0->1.0. */
  float      rate=NN_DEFAULT_RATE;		/* Network learning rate. */
  float      momentum=NN_DEFAULT_MOMENTUM;	/* Network learning momentum. */
  float      gain=NN_DEFAULT_GAIN;		/* Neuronal gain (sigmodial function gain). */
  float      bias=NN_DEFAULT_BIAS;		/* Network bias. */
  char       nn_outfname[NN_MAX_FNAME_LEN] = "out.nn";		/* Neural Network output file. */
  char       nn_infname[NN_MAX_FNAME_LEN] = "in.nn";		/* Neural Network input file. */
  char       train_prop_infname[NN_MAX_FNAME_LEN] = "train.prop";	/* Training data properties. */
  char       test_prop_infname[NN_MAX_FNAME_LEN] = "test.prop";	/* Testing data properties. */
  char       eval_prop_infname[NN_MAX_FNAME_LEN] = "eval.prop";	/* Evaluation data properties. */
  char       train_fp_infname[NN_MAX_FNAME_LEN] = "train.fp";	/* Training data fingerprints. */
  char       test_fp_infname[NN_MAX_FNAME_LEN] = "test.fp";	/* Testing data fingerprints. */
  char       eval_fp_infname[NN_MAX_FNAME_LEN] = "eval.fp";	/* Evaluation data fingerprints. */
  char       predict_fp_infname[NN_MAX_FNAME_LEN] = "predict.fp";	/* Prediction data fingerprints. */
  char       comma_infname[NN_MAX_FNAME_LEN] = "\0";       	/* Comma-delimited data file. */

  printf("Neural Network prototype code.\n");

  for(i=1;i<argc;i++)
    {
    if (!strcmp(argv[i],"--version"))
      {
      printf("Prototype version.\n");
      exit(2);
      }
    else if (!strcmp(argv[i],"--help") || !strcmp(argv[i],"-usage"))
      {
      write_usage();
      exit(2);
      }
    else if (!strcmp(argv[i],"--seed"))
      {
      i++;
      seed = atoi(argv[i]);
      printf("The random number generator seed value will be: %d\n", seed);
      }
    else if (!strcmp(argv[i],"--layers"))
      {
      i++;
      num_layers = atoi(argv[i]);
      printf("The number of layers (incl. input+output) will be: %d\n", num_layers);
      }
    else if (!strcmp(argv[i],"--neurons"))
      {
      if (num_layers==0) die("Must specify number of layers prior to number of neurons");
      neurons = (int *) s_malloc(sizeof(int)*num_layers);
      for (j=0; j<num_layers; j++)
        {
        i++;
        neurons[j] = atoi(argv[i]);
        printf("The number of neurons in layer %d will be: %d\n", j, neurons[j]);
        }
      }
    else if (!strcmp(argv[i],"--epochs"))
      {
      i++;
      max_epochs = atoi(argv[i]);
      printf("The maximum number of training epochs will be: %d\n", max_epochs);
      }
    else if (!strcmp(argv[i],"--teststep"))
      {
      i++;
      teststep = atoi(argv[i]);
      printf("The number of epochs per testing step will be: %d\n", teststep);
      }
    else if (!strcmp(argv[i],"--stopfactor"))
      {
      i++;
      stop_ratio = atof(argv[i]);
      printf("The stopped training factor will be: %f\n", stop_ratio);
      }
    else if (!strcmp(argv[i],"--rate"))
      {
      i++;
      rate = atof(argv[i]);
      printf("The learning rate will be: %f\n", rate);
      }
    else if (!strcmp(argv[i],"--momentum"))
      {
      i++;
      momentum = atof(argv[i]);
      printf("The learning momentum will be: %f\n", momentum);
      }
    else if (!strcmp(argv[i],"--bias"))
      {
      i++;
      bias = atof(argv[i]);
      printf("The network bias will be: %f\n", bias);
      }
    else if (!strcmp(argv[i],"--gain"))
      {
      i++;
      gain = atof(argv[i]);
      printf("The neuronal gain will be: %f\n", gain);
      }
    else if (!strcmp(argv[i],"--readnn"))
      {
      i++;
      strncpy(nn_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("NN will be read from file: \"%s\"\n", nn_infname);
      do_readnn = TRUE;
      }
    else if (!strcmp(argv[i],"--writenn"))
      {
      i++;
      strncpy(nn_outfname, argv[i], NN_MAX_FNAME_LEN);
      printf("NN will be written to file: \"%s\"\n", nn_outfname);
      do_writenn = TRUE;
      }
    else if (!strcmp(argv[i],"--nowritenn"))
      {
      printf("NN will not be written a file.\n");
      do_writenn = FALSE;
      }
    else if (!strcmp(argv[i],"--notrain"))
      {
      printf("NN will not be trained.\n");
      do_train = FALSE;
      }
    else if (!strcmp(argv[i],"--noevaluate"))
      {
      printf("NN will not be evaluated.\n");
      do_evaluate = FALSE;
      }
    else if (!strcmp(argv[i],"--predict"))
      {
      printf("NN will used for prediction.\n");
      do_predict = TRUE;
      }
    else if (!strcmp(argv[i],"--random01"))
      {
      printf("NN weights will be initialized between 0.0 and 1.0.\n");
      do_randomize01 = TRUE;
      }
    else if (!strcmp(argv[i],"--random11"))
      {
      printf("NN weights will be initialized between -1.0 and +1.0.\n");
      do_randomize01 = FALSE;
      }
    else if (!strcmp(argv[i],"--trainprop"))
      {
      i++;
      strncpy(train_prop_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Training properties will be read from file: \"%s\"\n", train_prop_infname);
      }
    else if (!strcmp(argv[i],"--testprop"))
      {
      i++;
      strncpy(test_prop_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Testing properties will be read from file: \"%s\"\n", test_prop_infname);
      }
    else if (!strcmp(argv[i],"--evalprop"))
      {
      i++;
      strncpy(eval_prop_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Evaluation proprties will be read from file: \"%s\"\n", eval_prop_infname);
      }
    else if (!strcmp(argv[i],"--trainfp"))
      {
      i++;
      strncpy(train_fp_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Training fingerprints will be read from file: \"%s\"\n", train_fp_infname);
      }
    else if (!strcmp(argv[i],"--testfp"))
      {
      i++;
      strncpy(test_fp_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Testing fingerprints will be read from file: \"%s\"\n", test_fp_infname);
      }
    else if (!strcmp(argv[i],"--evalfp"))
      {
      i++;
      strncpy(eval_fp_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Evaluation fingerprints will be read from file: \"%s\"\n", eval_fp_infname);
      }
    else if (!strcmp(argv[i],"--predictfp"))
      {
      i++;
      strncpy(predict_fp_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Prediction fingerprints will be read from file: \"%s\"\n", predict_fp_infname);
      }
    else if (!strcmp(argv[i],"--comma"))
      {
      i++;
      strncpy(comma_infname, argv[i], NN_MAX_FNAME_LEN);
      printf("Data will be read from comma-delimited file: \"%s\"\n", comma_infname);
      }
    else
      {
      dief("Unable to parse command-line arguments.  Current argument (%d) = \"%s\"\n", i, argv[i]);
      }
    }

/*
 * Initialize random number generator.
 */
  random_init();
  random_seed(seed);

  if (do_readnn)
    {	/* Read a pre-computed NN. */
    network = NN_read(nn_infname);
    }
  else
    {
/*
 * Generate a new neural net.
 *
 * Do we have all the required info?
 */
    if (neurons==NULL || num_layers==0)
      {
      while (num_layers<2)
        {
        printf("Enter number of layers (incl. input and output)\n");
        scanf("%d", &num_layers);
        }
      if (neurons==NULL) neurons = (int *) s_malloc(sizeof(int)*num_layers);
      for (i=0; i<num_layers; i++)
        {
        do
          {
          printf("Enter dimensions of layer %d.\n", i);
          scanf("%d", &(neurons[i]));
          } while(neurons[i]<1);
        }
      }

    network = NN_new(num_layers, neurons);

    NN_set_rate(network, rate);
    NN_set_momentum(network, momentum);
    NN_set_gain(network, gain);
    NN_set_bias(network, bias);

    if (do_randomize01)
      NN_randomize_weights(network);
    else
      NN_randomize_weights(network);
    }

  NN_display_summary(network);

/*
 * Read the datasets.
 */
  if (comma_infname[0] != '\0')
    {   /* Read comma-delimited data.  This is designed for testing and benchmarking use. */
    read_comma_delimited_data(comma_infname, &train_data, &train_property, &train_labels,
                              &num_train_data, &max_train_data);
    split_data(&train_data, &train_property, &train_labels, &num_train_data, &max_train_data,
               &test_data, &test_property, &test_labels, &num_test_data, &max_test_data,
               &eval_data, &eval_property, &eval_labels, &num_eval_data, &max_eval_data,
               10,10);
    }
  else
    {   /* Read sets of data files.  Intended for reading output from "rfp". */
    if (do_train)
      {
      if ( network->layer[0].neurons !=
           read_data(train_fp_infname, &train_data, &train_labels,
                     &num_train_data, &max_train_data) )
        die("Input data dimension mismatch");
      if ( network->layer[0].neurons !=
           read_data(test_fp_infname, &test_data, &test_labels,
                     &num_test_data, &max_test_data) )
        die("Input data dimension mismatch");
      read_prop(train_prop_infname, &train_property, &train_labels,
                    &num_train_prop, &num_train_data,
                    network->layer[network->num_layers-1].neurons);
      read_prop(test_prop_infname, &test_property, &test_labels,
                    &num_test_prop, &num_test_data,
                    network->layer[network->num_layers-1].neurons);
      }

    if (do_evaluate)
      {
      if ( network->layer[0].neurons !=
           read_data(eval_fp_infname, &eval_data, &eval_labels,
                     &num_eval_data, &max_eval_data) )
        die("Input data dimension mismatch");
      read_prop(eval_prop_infname, &eval_property, &eval_labels,
                    &num_eval_prop, &num_eval_data,
                    network->layer[network->num_layers-1].neurons);
      }
      
    if (do_predict)
      {
      if ( network->layer[0].neurons !=
           read_data(predict_fp_infname, &predict_data, &predict_labels,
                     &num_predict_data, &max_predict_data) )
        die("Input data dimension mismatch");
      }
    }

  printf("\n");
  printf("num_train_data = %d/%d (%s)\n", num_train_data, max_train_data, train_data?"full":"empty");
  printf("num_test_data = %d/%d (%s)\n", num_test_data, max_test_data, test_data?"full":"empty");
  printf("num_eval_data = %d/%d (%s)\n", num_eval_data, max_eval_data, eval_data?"full":"empty");
  printf("num_predict_data = %d/%d (%s)\n", num_predict_data, max_predict_data, predict_data?"full":"empty");
  printf("\n");
  printf("num_train_prop = %d/(%d) (%s)\n", num_train_prop, max_train_data, train_property?"full":"empty");
  printf("num_test_prop = %d/(%d) (%s)\n", num_test_prop, max_test_data, test_property?"full":"empty");
  printf("num_eval_prop = %d/(%d) (%s)\n", num_eval_prop, max_eval_data, eval_property?"full":"empty");
  printf("\n");

/*
 * Train the network.
 */
  if (do_train)
    {
    printf("Training with max_epochs = %d teststep = %d stop_ratio = %f\n",
           max_epochs, teststep, stop_ratio);

    NN_test(network, &trainerror, &testerror);
    printf("Initial (random) error is %0.5f on training set and %0.5f on test set.",
             trainerror, testerror);
    mintesterror = testerror;

/*
 * Stop if:
 * (a) Max. epochs done.
 * (b) Over training is likely.
 * (c) Convergence.
 */
    for (epoch=0; epoch<max_epochs &&
                  testerror < stop_ratio * mintesterror &&
                  testerror > TINY; epoch+=teststep)
      {
      NN_train(network, teststep);
      NN_test(network, &trainerror, &testerror);
      printf("\n%d: error is %f on training set and %f on test set.",
               epoch, trainerror, testerror);
      if (testerror < mintesterror)
        {
        printf(" - saving weights.");
        mintesterror = testerror;
        NN_save_weights(network);
        }
      }
    printf(" - stopping training and restoring weights.\n");
    printf("%f %f\n", testerror, mintesterror);
    NN_restore_weights(network);

    NN_test(network, &trainerror, &testerror);
    printf("Final error is %f on training set and %f on test set.\n",
             trainerror, testerror);
    }

  if (do_evaluate)
    {
    NN_evaluate(network);
    }

  if (do_predict)
    {
    NN_predict(network);
    }

  if (do_writenn)
    {
    NN_write(network, nn_outfname);
    }

  NN_destroy(network);

  exit(2);
  }
#endif /* NN_STANDALONE defined */


