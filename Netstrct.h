/**************************************************************************
 **************************************************************************
 ***									***
 ***				NETSTRUCT.H				***
 ***									***
 *** Notice:                                                            ***
 ***    This code is copyright (C) 1995 by David M. Skapura.  It may    ***
 ***    be used as is, or modified to suit the requirements of a  	***
 ***    specific application without permission of the author.		***
 ***    There are no royalty fees for use of this code, as long         ***
 ***    as the original author is credited as part of the final		***
 ***    work.  In exchange for this royalty free use, the user    	***
 ***    agrees that no guarantee or warantee is given or implied.	***
 ***									***
 ***									***
 **************************************************************************
 **************************************************************************/

#define BPN

#define FALSE       0
#define TRUE        1

#define UNIT_OFF  0.0
#define UNIT_ON   1.0

typedef int (*afn) ();	        /* type of activation functions 	*/

typedef void (*pfn) ();         /* type of propagation function 	*/


#define LINEAR      (afn)linear
#define SIGMOID     (afn)sigmoid
#define THRESHOLD   (afn)threshold
#define GAUSSIAN    (afn)gaussian
#define ON_CENTER   (afn)on_center_off_surround

#define RANDOM      0
#define TEST        1

#define DOT_PRODUCT (pfn)dot_product
#define TRANSFER    (pfn)one_to_one

#define COMPLETE    0
#define ONE_TO_ONE  1
#define NORMAL      2
#define VALUE	    3


typedef struct 			/* the generic layer structure		*/
 {
   int	 units;	 		/* count of units on layer		*/
   int   inputs;		/* count of units feeding this layer	*/
   int   processed;		/* index value on some layers 		*/
   float modifier;		/* modifier for activation function	*/
   float initval;		/* used to initialize some connections  */
   float *outputs;		/* pointer to array of output values    */
   float **connects;		/* pointer to array of pointers for wts */
   afn   activation; 		/* activation function for the layer 	*/
   pfn   propto;		/* propagation function for layer 	*/

   #ifdef BPN
     float **lastdelta;		/* used only by the bpn network		*/
     float *errors;
     float eta;
     float alpha;
     afn   deriv;
   #endif
 } layer;
