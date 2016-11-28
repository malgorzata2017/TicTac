/**************************************************************************
 **************************************************************************
 ***									                                ***
 ***				BPN.C					                            ***
 ***									                                ***
 *** Notice:                                                            ***
 ***    This code is copyright (C) 1995 by David M. Skapura. It may     ***
 ***    be used as is, or modified to suit the requirements of a  	    ***
 ***    specific application without permission of the author.		    ***
 ***    There are no royalty fees for use of this code, as long         ***
 ***    as the original author is credited as part of the final		    ***
 ***    work.  In exchange for this royalty free use, the user    	    ***
 ***    agrees that no guarantee or warantee is given or implied.	    ***
 ***									                                ***
 ***									                                ***
 **************************************************************************
 **************************************************************************/


// Adapted by George W. Taylor - December, 2003


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdarg.h>
#include "netstrct.h"
#include "BpnTypes.h"
#include "Bpn.h"

static bpn *neuralNet;

#define LAYER neuralNet->net
#define INLAYER neuralNet->net[0]
#define OUTLAYER neuralNet->net[(n->layers)-1]

#define INPUTS 18
#define OUTPUTS 9

//**************************************************************
// "From activate.c"
int sigmoid (layer *l)
{
  float tau, *outs;
  int i, units;

  tau = l->modifier;
  outs = l->outputs;
  units = l->units;

  for(i = 0; i < units; i++)
      outs[i] = (float)(1.0 / (1.0 + exp(-(outs[i] * tau))));

  return (units);
}

int sigmoid_derivative (layer *l)
{
  float *outs, *derivs;
  int i, units;

  outs = l->outputs;

  #ifdef BPN
   derivs = l->errors;
  #endif

  units = l->units;
  for(i = 0; i < units; i++)
      derivs[i] = (float)(outs[i] * ((float)1.0 - outs[i]));
  return (units);
}

int linear (layer *l)
{
  float scale, *outs;
  int i, units;

  scale = l->modifier;
  outs = l->outputs;
  units = l->units;

  for(i = 0; i < units; i++)
      outs[i] = scale * outs[i];

  return (units);
}

int linear_derivative (layer *l)
{
  float *derivs;
  int i, units;

  #ifdef BPN
   derivs = l->errors;
  #endif

  units = l->units;

  for(i = 0; i < units; i++)
      derivs[i] = 1.0;

  return (units);
}

int threshold (layer *l)
{
  float trigger, *outs;
  int i, units;

  trigger = l->modifier;
  outs = l->outputs;
  units = l->units;

  for(i = 0; i < units; i++)
      if(outs[i] >= trigger)outs[i] = UNIT_ON;
      else outs[i] = UNIT_OFF;

  return (units);
}

int on_center_off_surround (layer *l)
{
  float max, *outs;
  int i, winner, units;

  max = -100000.0;
  outs = l->outputs;
  units = l->units;

  for(i = 0; i < units; i++)
  {
      if(outs[i] > max)
      {
         max = outs[i];
         winner = i;
      }

      outs[i] = UNIT_OFF;
  }

  outs[winner] = UNIT_ON;
  return (winner);
}

void activate (layer *layer)
{
  layer->processed = layer->activation (layer);
}

// End "activate.c"
//*********************************************************************

// "From propgate.c"
void dot_product (layer *fromlayer, layer *tolayer)
{
  int i, j;
  float *wts, *outs, *ins, **connects;

  outs = tolayer->outputs;
  connects = tolayer->connects;

  for(i = 0; i < tolayer->units; i++)
  {
      outs[i] = 0;
      wts = connects[i];
      ins = fromlayer->outputs;

      for(j = 0; j < fromlayer->units; j++)
          outs[i] = outs[i] + (wts[j] * ins[j]);
  }
}

void one_to_one (layer *fromlayer, layer *tolayer)
{
  int i, units;
  float *outs, *ins;

  outs = tolayer->outputs;
  ins = fromlayer->outputs;
  units = tolayer->units;

  for(i = 0; i < units; i++)outs[i] = ins[i];
}

void propagate (layer *fromlayer, layer *tolayer)
{
  tolayer->propto(fromlayer, tolayer);
}
// End "propgate.c"
//************************************************************************

// "From initnet.c"
void set_random_weights (layer *layer)
{
  float **connects, *wts;
  int i, j, incnt, tocnt;

  connects = layer->connects;
  if (!connects) return;

  incnt = layer->inputs;
  tocnt = layer->units;

  for(i = 0; i < tocnt; i++)
  {
    wts = connects[i];
    for(j = 0; j < incnt; j++)
        wts[j] = ((float)rand() / (float)32767.0) - (float)0.5;
  }
}

void set_test_weights (layer *layer)
{
  float **connects, *wts;
  int i, j, incnt, tocnt;

  connects = layer->connects;
  if(!connects)return;

  incnt = layer->inputs;
  tocnt = layer->units;

  for(i = 0; i < tocnt; i++)
  {
      wts = connects[i];
      for(j = 0; j < incnt; j++)
      wts[j] = (float)i + ((float)j / (float)1000.0);
  }
}

void set_normal_weights (layer *layer)
{
  float **connects, *wts, wtval;
  int i, j, incnt, tocnt;

  connects = layer->connects;
  if(!connects)return;

  incnt = layer->inputs;
  tocnt = layer->units;
  wtval = (float)(1 / sqrt(incnt));

  for(i = 0; i < tocnt; i++)
  {
      wts = connects[i];
      for(j = 0; j <incnt; j++)
          wts[j] = wtval;
  }
}

void set_value_weights (layer *layer)
{
  float **connects, *wts, wtval;
  int i, j, incnt, tocnt;

  connects = layer->connects;
  if (!connects) return;

  incnt = layer->inputs;
  tocnt = layer->units;
  wtval = layer->initval;

  for(i=0; i < tocnt; i++)
  {
      wts = connects[i];
      for(j = 0; j < incnt; j++)
      wts[j] = wtval;
  }
}

void set_weights (layer *l, int how)
{
  if (how == RANDOM) set_random_weights (l);
  if (how == TEST)   set_test_weights (l);
  if (how == NORMAL) set_normal_weights (l);
  if (how == VALUE)  set_value_weights (l);
}

// End "initnet.c"
//************************************************************************

// "From buildnet.c"
int *define_layers (int layers, ...)
{
  va_list argptr;
  int *l, i;

  l = (int *) calloc (layers+1, sizeof(int));
  l[0] = layers;

  va_start (argptr, layers);
  for (i=1; i<=layers; i++) l[i] = va_arg (argptr, int); 
  va_end (argptr);

  return (l);
}

layer *build_layer (int units)
{
  layer *l;

  l = (layer *) calloc (1, sizeof(layer));
  l->units = units;
  l->inputs = 0;
  l->modifier = 0.0;
  l->initval = 1.0;
  l->outputs = (float *) calloc (units, sizeof(float));
  l->connects = NULL;
  l->activation = (afn) linear;       /* default activation function */
  l->propto = (pfn) dot_product;      /* default propagation function */

  #ifdef BPN
   l->lastdelta = NULL;
   l->errors = (float *) calloc (units, sizeof(float));
   l->deriv = (afn) linear_derivative;
   l->eta = 0.5;
   l->alpha = 0.0;
  #endif

  return (l);
}

void set_activation (layer *l, afn activation, float modifier)
{
  l->activation = activation;
  l->modifier = modifier;

  #ifdef BPN
    if (activation == SIGMOID)  l->deriv = (afn) sigmoid_derivative;
    if (activation == LINEAR)   l->deriv = (afn) linear_derivative; 
  #endif
}

void set_propagation(layer *l, pfn netx)
{
 l->propto = netx;
 set_activation(l, LINEAR, 1.0);    /* reset to default */
}


#ifdef BPN
void set_eta(layer *l, float eta)
{
 l->eta = eta;
}


void set_alpha(layer *l, float alpha)
{
   l->alpha = alpha;
}
#endif

void connect_layers(layer *inlayer, layer *tolayer)
{
 register int i;

 tolayer->inputs = inlayer->units;
 tolayer->connects = (float **)calloc(tolayer->units, sizeof(float *));

 for(i = 0; i < tolayer->units; i++)
     tolayer->connects[i] = (float *)calloc(tolayer->inputs, sizeof (float));

 #ifdef BPN
   tolayer->lastdelta =(float **)calloc(tolayer->units, sizeof(float *));

   for(i = 0; i < tolayer->units; i++)
       tolayer->lastdelta[i] = (float *)calloc(tolayer->inputs, sizeof(float *));
 #endif
}

void connect(layer *inlayer, layer *tolayer, int how, int init)
{
 if(how == COMPLETE)
 {
    connect_layers (inlayer, tolayer);
    set_propagation (tolayer, DOT_PRODUCT);
 }else
 {
    if(how == ONE_TO_ONE)set_propagation(tolayer, TRANSFER);
 }

 set_weights (tolayer, init);
}

layer **build_net(int layers, int *sizes)
{
 layer **n;
 register int i;

 n = (layer **)calloc(layers, sizeof(layer *));

 for(i = 0; i < layers; i++)
     n[i] = build_layer(sizes[i]);

 return (n);
}

void destroy_net(int layers, layer **n)
{
 int i, j;
 float *wts;
 layer *l;

 for(i = 0; i < layers; i++)
 {
     l = n[i];
     if(l->outputs != NULL)free(l->outputs);

     if(l->connects != NULL)
     {
        for(j = 0; j < l->units; j++)
		{
            wts = l->connects[j];
            if(wts != NULL)free(wts);
		}
     }

 #ifdef BPN
     if(l->lastdelta != NULL)
     {
        for(j = 0; j < l->units; j++)
		{
	        wts = l->lastdelta[j];
	        if(wts != NULL)free(wts);
		}
     }
     if(l->errors != NULL)free(l->errors);
 #endif

    free (l);
 }

 free (n);
}

// End "buildnet.c"
/************************************************************************

 The function build_bpn creates a generic backpropagation network in
 memory.  The input parameters to the function are the number of layers
 desired, and an array of integers indicating the number of units on
 each layer.  It is assumed that layer[0] is the input layer in the
 network, and layer[n-1] is the output layer.  The function returns
 a pointer to the network structure in memory.  No testing is performed
 to determine if the memory allocation was successful.

*************************************************************************/

bpn *build_bpn (int *sizes)
{
  bpn *n;
  int layers;

  n = (bpn *)calloc(1, sizeof(bpn));
  layers = sizes[0];
  n->net = build_net(layers, &sizes[1]);
  n->layers = layers;
  n->patterns = (iop *)calloc(1, sizeof(iop));
  n->patterns->dimX = -1;	/* no exemplars yet */
  n->patterns->dimY = -1;
  n->patterns->invecs = NULL;
  n->patterns->outvecs = NULL;
  n->exemplars = 0;
  n->wrong = 0;
  strcpy (n->filename, "");
  return (n);
}

/************************************************************************

 The function destroy_bpn undoes the build operation.  It takes a bpn
 structure pointer as input, then proceeds to deallocate all of the
 memory used to model the network.

*************************************************************************/

void destroy_bpn(bpn *n)
{
 int i;

 for(i = 0; i < n->exemplars; i++)
 {
     if(n->patterns->invecs[i])free (n->patterns->invecs[i]);
     if(n->patterns->outvecs[i])free (n->patterns->outvecs[i]);
 }

 if(n->patterns->invecs)free(n->patterns->invecs);
 if(n->patterns->outvecs)free(n->patterns->outvecs);

 destroy_net(n->layers, n->net);
 free (n);
}

/***********************************************************************

 The function valid_exemplars tests that the dimension of the exemplars
 read from the file match the dimension of the input vector to the
 network.

************************************************************************/

int valid_exemplars(bpn *n)
{
 return(( INLAYER->units == n->patterns->dimX) &&
	    (OUTLAYER->units == n->patterns->dimY) );
}

/***********************************************************************

 The function apply_inputs takes an array of floats, which is assumed
 to be the same dimension as the input layer, and assigns the outputs
 of the input layer to be the same as the values in the input array.

************************************************************************/

void apply_input(bpn *n, float *invals)
{
  float *outvals;
  int i, units;

  units = INLAYER->units;
  outvals = INLAYER->outputs;

  for (i = 0; i < units; i++)outvals[i] = invals[i];
}

/***********************************************************************

  The function prop_forward propagates the information in the output of
 the input layer through the entire network.  It starts at the first
 non-input layer in the network, and works its way to the output layer.
 Each layer is processed by first computing the input stimulation at
 each unit on the layer, then the input stimulation is converted to the 
 appropriate output by applying the activation function assigned to the 
 layer.  The input to this function is the pointer to the network.

************************************************************************/

void prop_forward (bpn *n)
{
 int i;

 for(i = 1; i < n->layers; i++)
 {
     propagate (LAYER[i-1], LAYER[i]);
     activate (LAYER[i]);
 }
}


// Count the number of outputs > 0.5
// if just 1 output > 0.5, return the index to that unit
// else return -1 if no units activated, or > 1 units activated
//*********************************************************
int get_result(bpn *n, int pattern)
{
  float *outs;
  int i, count_activated, unit_index;

  outs = OUTLAYER->outputs;

  count_activated = 0;
  for(i = 0; i < OUTLAYER->units; i++)
  {
      if(outs[i] > 0.5)
      {
         ++count_activated;
		 unit_index = i;
	  }	   
  }
  if(count_activated == 1)return unit_index;
  return -1;
}

/***********************************************************************
 The function restore_bpn recreates a BPN structure in memory from data 
 saved in a file.  Note that only connection weight data is restored. 
 If the BPN uses activation functions other than the default (LINEAR), 
 these must be reinstalled after recreating the network by using the
 set_parameters command.
************************************************************************/

// Restore the network and point neuralNet pointer to it
int restore_bpn (char *filename)
{
  short data[4];
  int *ldata, layers, i, j;
  FILE *fp;

  neuralNet = NULL;

  if(!(fp = fopen (filename, "rb")))
  {
     return 0;
  }

  fread(&data, sizeof(short), 4, fp);
  layers = data[0];

  ldata = (int *)calloc(layers + 1, sizeof(int));
  ldata[0] = layers;
  ldata[1] = data[1];
  ldata[2] = data[2];
  ldata[3] = data[3];

  neuralNet = build_bpn(ldata);
  free(ldata);

  if(neuralNet == NULL)return 0;

  for(i = 1; i < layers; i++)
  {
      connect(LAYER[i-1], LAYER[i], COMPLETE, RANDOM);
      for(j = 0; j < LAYER[i]->units; j++)
          fread(LAYER[i]->connects[j], sizeof(float), LAYER[i-1]->units, fp);
      fread (&LAYER[i]->eta, sizeof(float), 1, fp);
      fread (&LAYER[i]->alpha, sizeof(float), 1, fp);
  }

  fclose (fp);
  return 1;
}


/***********************************************************************
 The function set_parameters initializes a layer on the BPN.  Arguments
 to this function are a pointer to the layer, the propagation function
 to be used during feed-forward propagation, the activation function for
 the layer, and values for the learning rate and momentum terms.
************************************************************************/

void set_parameters (layer *l, pfn p, afn a, float m, float eta, float alpha)
{
  set_propagation (l, p);
  set_activation  (l, a, m);
  set_eta (l, eta);
  set_alpha (l, alpha);
}


// George - apply a single pattern and get the response...
int matchPattern (bpn *n, float maxerr)
{
  float global_error;

  n->maxerr = maxerr;
  n->exemplars = 1;
  if(!valid_exemplars(n))
  {
	 return 0;
  }

  global_error = 0.0;
  n->wrong = 0;
  n->worst = 0;

  apply_input(n, *(n->patterns->invecs));
  prop_forward (n);
  return get_result(n, 0);
}


// George -  Pass 1 exemplar and get the result...
int GetNetMove(int *board)
{
  int k;
  float *invecsArray;

  invecsArray = *(neuralNet->patterns->invecs);
  for(k = 0; k < INPUTS; k++)
  {
      invecsArray[k] = (float)board[k];
  }	  

  k = matchPattern(neuralNet, (float)0.01);
  return k;
}

// George - 
int InitNet(char *weights)
{
  float *ptrInvecArray;
  float *ptrOutvecArray;
  float **ptrptrInvecArray;
  float **ptrptrOutvecArray;
  int k;

  if(!restore_bpn(weights))return 0;

  set_parameters(LAYER[1], DOT_PRODUCT, SIGMOID, (float)1.0, (float)0.6, (float)0.3);
  set_parameters(LAYER[2], DOT_PRODUCT, SIGMOID, (float)1.0, (float)0.6, (float)0.3);

  // Create the arrays to contain the exemplar...
  ptrptrInvecArray = (float **)calloc (1, sizeof(float*));
  ptrptrOutvecArray = (float **)calloc (1, sizeof(float*));
  if(ptrptrInvecArray == NULL)return 0;
  if(ptrptrOutvecArray == NULL)return 0;

  ptrInvecArray = (float *)calloc(INPUTS, sizeof(float));
  ptrOutvecArray = (float *)calloc(OUTPUTS, sizeof(float));
  if(ptrInvecArray == NULL)return 0;
  if(ptrOutvecArray == NULL)return 0;

  for(k = 0; k < INPUTS; k++)
  {
      ptrInvecArray[k] = (float)0.0;
  }	  

  for(k = 0; k < OUTPUTS; k++)
  {
      ptrOutvecArray[k] = (float)0.0;
  }	  
  *ptrptrInvecArray = ptrInvecArray;
  *ptrptrOutvecArray = ptrOutvecArray;
  neuralNet->patterns->dimX = INPUTS;
  neuralNet->patterns->dimY = OUTPUTS;
  neuralNet->patterns->invecs = ptrptrInvecArray;
  neuralNet->patterns->outvecs = ptrptrOutvecArray;

  return 1;
}

// George - 
void KillNet(void)
{
 if(neuralNet != NULL)
 {
    destroy_bpn(neuralNet);
	neuralNet = NULL;
 }
}
