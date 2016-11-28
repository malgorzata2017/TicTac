#ifndef BPNTYPES_H
	#define BPNTYPES_H

typedef struct
 {
  int dimX;
  int dimY;
  float **invecs;
  float **outvecs;
 }iop;


typedef struct
 {
  int    layers;		/* number of layers in the network	*/
  int	 exemplars;		/* number of training pairs in net	*/
  float	 maxerr;		/* maximum error value for each output  */
  float  worst;			/* worst error for all training pairs	*/
  int	 wrong;			/* count of how many wrong state units	*/
  iop	 *patterns;		/* training pairs structure pointer	*/
  layer  **net;                 /* use the basic network structure	*/
  char   filename[40];		/* default name for network file	*/
 }bpn;

#endif