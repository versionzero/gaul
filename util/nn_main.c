/**********************************************************************
  nn_main.c
 **********************************************************************

  nn_main - Example use of libnn_util.
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
		optional momentum and decay.

		Warning: This code contains almost no error checking!

		This code uses neuronal input/response in the range 0.0>=x>=1.0.
		Note that best results will be acheived if data is
		similarly normalized.

  Last Updated:	22 Jul 2002 SAA	Added new '--random' option.
  		18 Jul 2002 SAA	Fixed typo causing erronerous data dimension mismatches.
		17 Jul 2002 SAA	Simplified/improved handling of data dimensionality from users perspective.
		26 Jun 2002 SAA	Added facility for writing the training and testing errors to a file.  This is intended for use with gnuplot, or some other plotting utility.
		04 Apr 2002 SAA write_usage() tweaked slightly.
		12 Mar 2002 SAA In standalone test program code, introduced the ability to select the alternative training functions.  Split the code for the standalone program version into a seperate file, nn_main.c.  Added some new options.
		01 Mar 2002 SAA	Added weight decay functionality.  Added NN_set_layer_bias().  Broken compatibility in NN_write() and modified argument passing filename to const.  NN_read() renamed to NN_read_compat(), and new NN_read() implemented.  Per-layer bias is now available.  Added NN_adjust_weights_momentum() and NN_adjust_weights_decay().  Modified NN_adjust_weights() to perform classic back-propagation only.
  		25 Feb 2002 SAA	Added code for batch mode training; NN_train_batch_systematic(), NN_train_batch_random(), NN_output_error_sum() and NN_simulate_batch().
		06 Feb 2002 SAA Fixed bug in NN_train_systematic() that caused segfault if num_epochs>1.
		04 Feb 2002 SAA	All global variables are now declared static.  Functions for defining data from external source added.
		28 Jan 2002 SAA Modifications for distribution with GAUL.  Renamed NN_train() to NN_train_random() and added NN_train_systematic().  Added NN_clone() and NN_copy().
  		25 Jan 2002 SAA	By default, standalone code is not compiled - change required for incorporation into GAUL example directory.  Renamed to nn_util.c and split off a nn_util.h file.  NN_diagnostics() added.  Renamed some defines for consistency.
		24 Dec 2002 SAA Removed stupid error calculation from NN_predict().
		12 Dec 2001 SAA Fixed read_prop() bug.
		10 Dec 2001 SAA read_prop() now reads data with variable number of fields.  Default behaviour is to initialize weights within the range 0.0-1.0.  Comma delimited data is now usable.
  		05 Dec 2001 SAA	Read any size fingerprint.
		04 Dec 2001 SAA Completed usage message.  Defining layers on command-line.  NN_display_summary() added.
		03 Dec 2001 SAA Substantial generalisations.  More comments.  New I/O functions.  Initial support for labelled data.
		30 Nov 2001 SAA	Changes for improved clarity.
		29 Nov 2001 SAA Renamed and some comments added.  Some code generalised.  Introduced use of memory_util library.  All this is in preparation of optimisation using GAUL.

  To do:	Need to define data from external sources.
		Alternative switching functions.
		Automated functions for "leave-one-out" validation.
		Full support for weight decay method starting at a given epoch.

 **********************************************************************/

#include "nn_util.h"

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
  write_usage()
  synopsis:     Display usage details.
  parameters:   none
  return:       none
  updated:      12 Mar 2002
 **********************************************************************/

void write_usage(void)
  {

  printf("\n"
         "nn - Simple standalone neural network utility\n"
         "Copyright ©2001-2002, Regents of the Universtity of California.\n"
         "primary Author: \"Stewart Adcock\" <stewart@linux-domain.com>\n"
         "\n"
         "Usage: nn [switches...]\n"
         "\n"
         "Available switches are:\n"
         "    --version              Version information.\n"
         "    --{help,usage}         This stuff.\n"
         "    --diagnostics          Display diagnostic information.\n"
         "    --seed INTEGER         PRNG seed value.\n"
         "    --epochs INTEGER       Maximum number of 'stopped-training' epochs.\n"
         "    --teststep INTEGER     Number of epochs per testing phase.\n"
         "    --stopfactor REAL      Stopped training factor.\n"
         "    --rate REAL            Learning rate.\n"
         "    --momentum REAL        Learning momentum.\n"
         "    --bias REAL            Network bias.\n"
         "    --gain REAL            Neuronal gain.\n"
         "    --decay REAL           Weight decay factor.\n"
         "    --history FILENAME     Write training and testing errors to file after every epoch.\n"
         "    --readnn FILENAME      Read NN from file.\n"
         "    --writenn FILENAME     Write NN to file.\n"
         "    --nowritenn            Do not write NN to file.\n"
         "    --notrain              Do not train NN.\n"
         "    --trainsystematic      Train using ordered data. (singular mode)\n"
         "    --trainrandom          Train using random data. (singular mode) [default]\n"
         "    --trainsystematicbatch Train using ordered data. (batch mode)\n"
         "    --trainrandombatch     Train using random data. (batch mode)\n"
         "    --noevaluate           Do not evaluate data with NN.\n"
         "    --predict              Perform prediction with NN.\n"
         "    --random01             NN weights initialized between 0.0 and 1.0. [default]\n"
         "    --random11             NN weights initialized between -1.0 and 1.0.\n"
         "    --fixed REAL           NN weights initialized to fixed value.\n"
         "    --trainrandom          Train with randomly ordered data. [default]\n"
         "    --trainordered         Train with data ordered as input.\n"
         "    --layers INTEGER       Number of layers (incl. input+output).\n"
         "    --neurons INTEGER...   Number of neurons in each layer.\n"
         "    --trainprop FILENAME   Training properties file.\n"
         "    --testprop FILENAME    Testing properties file.\n"
         "    --evalprop FILENAME    Evaluation properties file.\n"
         "    --trainfp FILENAME     Training fingerprints file.\n"
         "    --testfp FILENAME      Testing fingerprints file.\n"
         "    --evalfp FILENAME      Evaluation fingerprints file.\n"
         "    --predictfp FILENAME   Prediction fingerprints file.\n"
         "    --comma FILENAME       Comma-delimited input data file.\n"
         "\n");

  return;
  }


/**********************************************************************
  main()
  synopsis:     The main function.
  parameters:   int argc, char **argv
  return:       2
  last updated: 12 Mar 2002
 **********************************************************************/

typedef enum initmode_enum {fixed, randomize, randomize01, randomize11} initmode_t;

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
  NN_training_func training_func=NN_train_random;	/* Function to use for training. */
  initmode_t initmode=randomize11;		/* How to initialize weights */
  float      initval=0.5, initval1=-0.5, initval2=0.5;	/* Values for weight initialization. */
  int        num_layers=0;			/* Number of layers in NN. */
  int        *neurons=NULL;			/* Number of neurons in each layer. */
  float      stop_ratio=NN_DEFAULT_STOP_RATIO;	/* Stopping criterion. */
  int        max_epochs=NN_DEFAULT_MAX_EPOCHS;	/* Maximum training epochs. */
  float      rate=NN_DEFAULT_RATE;		/* Network learning rate. */
  float      momentum=NN_DEFAULT_MOMENTUM;	/* Network learning momentum. */
  float      gain=NN_DEFAULT_GAIN;		/* Neuronal gain (sigmodial function gain). */
  float      bias=NN_DEFAULT_BIAS;		/* Network bias. */
  float      decay=NN_DEFAULT_DECAY;		/* Weight decay. */
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

  FILE		*historyfp=NULL;	/* Training history data file. */

  int		input_layer_dim=-1, output_layer_dim=-1;	/* Specified layer dimensions. */
  int		input_data_dim=-1, output_data_dim=-1;		/* Read layer dimensions. */

  printf("Stewart's simple standalone neural network utility.\n");

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
    else if (!strcmp(argv[i],"--diagnostics"))
      {
      NN_diagnostics();
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
      input_layer_dim = neurons[0];
      output_layer_dim = neurons[num_layers-1];
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
    else if (!strcmp(argv[i],"--decay"))
      {
      i++;
      gain = atof(argv[i]);
      printf("The neuronal weight decay will be: %f\n", decay);
      }
    else if (!strcmp(argv[i],"--history"))
      {
      i++;
      printf("NN will be written to file: \"%s\"\n", argv[i]);
      if (!(historyfp=fopen(argv[i], "w")))
        dief("Unable to open file \"%s\" for output.", argv[i]);
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
    else if (!strcmp(argv[i],"--trainrandom"))
      {
      printf("NN will be trained using singular randomly-ordered data.\n");
      training_func = NN_train_random;
      }
    else if (!strcmp(argv[i],"--trainsystematic"))
      {
      printf("NN will be trained using singular ordered data.\n");
      training_func = NN_train_systematic;
      }
    else if (!strcmp(argv[i],"--trainrandombatch"))
      {
      printf("NN will be trained using batched randomly-ordered data.\n");
      training_func = NN_train_batch_random;
      }
    else if (!strcmp(argv[i],"--trainsystematicbatch"))
      {
      printf("NN will be trained using batched ordered data.\n");
      training_func = NN_train_batch_systematic;
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
      initmode = randomize01;
      }
    else if (!strcmp(argv[i],"--random11"))
      {
      printf("NN weights will be initialized between -1.0 and +1.0.\n");
      initmode = randomize11;
      }
    else if (!strcmp(argv[i],"--random"))
      {
      i++;
      initval1 = atof(argv[i]);
      i++;
      initval2 = atof(argv[i]);
      printf("NN weights will be initialized using random values in range %f to %f.\n", initval1, initval2);
      initmode = randomize;
      }
    else if (!strcmp(argv[i],"--fixed"))
      {
      i++;
      initval = atof(argv[i]);
      printf("NN weights will be initialized to %f.\n", initval);
      initmode = fixed;
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

/*
 * Read the datasets.
 */
  if (comma_infname[0] != '\0')
    {   /* Read comma-delimited data.  This is designed for testing and benchmarking use. */
die("FIXME: code broken.");
    read_comma_delimited_data(comma_infname, &train_data, &train_property, &train_labels,
                              &num_train_data, &max_train_data);

    if ( input_layer_dim == -1 ) 
      input_layer_dim = input_data_dim;
    else if ( input_layer_dim != input_data_dim)
      die("Input data dimension mismatch");

    split_data(&train_data, &train_property, &train_labels, &num_train_data, &max_train_data,
               &test_data, &test_property, &test_labels, &num_test_data, &max_test_data,
               &eval_data, &eval_property, &eval_labels, &num_eval_data, &max_eval_data,
               10,10);
    }
  else
    {   /* Read sets of data files.  Intended for reading output from "rfp". */
    if (do_train)
      {
      input_data_dim = NN_read_data(train_fp_infname, &train_data, &train_labels,
                     &num_train_data, &max_train_data);

      if ( input_layer_dim == -1 ) 
        input_layer_dim = input_data_dim;
      else if ( input_layer_dim != input_data_dim)
        die("Input data dimension mismatch");

      if ( input_data_dim !=
           NN_read_data(test_fp_infname, &test_data, &test_labels,
                     &num_test_data, &max_test_data) )
        die("Input data dimension mismatch");

      NN_read_prop(train_prop_infname, &train_property, &train_labels,
                    &num_train_prop, &num_train_data,  &output_data_dim);
      if (output_data_dim != output_layer_dim)
        die("Output data dimension mismatch");
      NN_read_prop(test_prop_infname, &test_property, &test_labels,
                    &num_test_prop, &num_test_data, &output_data_dim);
      if (output_data_dim != output_layer_dim)
        die("Output data dimension mismatch");
      }

    if (do_evaluate)
      {
      input_data_dim = NN_read_data(eval_fp_infname, &eval_data, &eval_labels,
                     &num_eval_data, &max_eval_data);

      if ( input_layer_dim == -1 )
        input_layer_dim = input_data_dim;
      else if ( input_layer_dim != input_data_dim)
        die("Input data dimension mismatch");

      NN_read_prop(eval_prop_infname, &eval_property, &eval_labels,
                    &num_eval_prop, &num_eval_data, &output_data_dim);
      if (output_data_dim != output_layer_dim)
        die("Output data dimension mismatch");
      }
      
    if (do_predict)
      {
      input_data_dim =  NN_read_data(predict_fp_infname, &predict_data, &predict_labels,
                     &num_predict_data, &max_predict_data);

      if ( input_layer_dim == -1 ) 
        input_layer_dim = input_data_dim;
      else if ( input_layer_dim != input_data_dim)
        die("Input data dimension mismatch");
      }
    }

  NN_define_train_data(num_train_data, train_data, train_property);
  NN_define_test_data(num_test_data, test_data, test_property);
  NN_define_eval_data(num_eval_data, eval_data, eval_property);
  NN_define_predict_data(num_predict_data, predict_data);

  printf("\n");
  printf("Data sets are:\n");
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
 * Prepare neural network.
 */
  if (do_readnn)
    {   /* Read a pre-computed NN. */
    network = NN_read(nn_infname);
    }
  else
    {
/*
 * Generate a new neural net.
 *
 * Do we have all the required info?
 */
    while (num_layers<2)
      {
      printf("Enter number of layers (incl. input and output)\n");
      scanf("%d", &num_layers);
      }
    if (neurons==NULL)
      {
      neurons = (int *) s_malloc(sizeof(int)*num_layers);
      printf("Dimensions of input layer are %d.\n", input_layer_dim);
      neurons[0] = input_layer_dim;
      for (i=1; i<num_layers-1; i++)
        {
        do
          {
          printf("Enter dimensions of layer %d.\n", i);
          scanf("%d", &(neurons[i]));
          } while(neurons[i]<1);
        }
      neurons[num_layers-1] = output_layer_dim;
      printf("Dimensions of output layer are %d.\n", output_layer_dim);
      }

    network = NN_new(num_layers, neurons);

    NN_set_rate(network, rate);
    NN_set_momentum(network, momentum);
    NN_set_gain(network, gain);
    NN_set_bias(network, bias);
    NN_set_decay(network, decay);

    switch (initmode)
      {
      case randomize11:
        NN_randomize_weights_11(network);
        break;
      case randomize01:
        NN_randomize_weights_01(network);
        break;
      case randomize:
        NN_randomize_weights(network, initval1, initval2);
        break;
      case fixed:
        NN_set_all_weights(network, initval);
        break;
      default:
        die("Unknown weight initialization mode.");
      }
    }

  NN_display_summary(network);

/*
 * Train the network.
 */
  if (do_train)
    {
    printf("Training with max_epochs = %d teststep = %d stop_ratio = %f\n",
           max_epochs, teststep, stop_ratio);

    NN_test(network, &trainerror, &testerror);
    printf("Initial error is %0.5f on training set and %0.5f on test set.",
             trainerror, testerror);
    mintesterror = testerror;

    if (historyfp) fprintf(historyfp, "0 %f %f\n", trainerror, testerror);

/*
 * Stop if:
 * (a) Max. epochs done.
 * (b) Over training is likely.
 * (c) Convergence.
 */
    epoch = 0;
    while ( epoch<max_epochs &&
            testerror < stop_ratio * mintesterror &&
            testerror > TINY )
      {
      for (i=0; i<teststep; i++)
        {
        epoch++;
        training_func(network, 1);
        NN_test(network, &trainerror, &testerror);
        if (historyfp) fprintf(historyfp, "%d %f %f\n", epoch, trainerror, testerror);
        }
      
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

  if (historyfp) fclose(historyfp);

  exit(2);
  }


