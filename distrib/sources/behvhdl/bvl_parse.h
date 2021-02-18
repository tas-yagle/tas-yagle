# ifndef BVL_PARSE_H
# define BVL_PARSE_H

/* ###--------------------------------------------------------------### */
/*									*/
/* file		: bvl_parse.h       	                                */
/* date		: Nov  2 1995						*/
/* version	: v111							*/
/* author	: L.A. TABUSSE & H.G. VUONG & P. BAZARGAN-SABET         */
/* description	:							*/
/*									*/
/* ###--------------------------------------------------------------### */

extern char         BVL_ERRFLG;		/* if = 1 no structure is made  */
extern int          BVL_AUXMOD;		/* simplify internal sig (= 1)	*/
extern int          BVL_CHECK;		/* activate coherency checks */
extern char         BVL_CURFIL[];		/* current file's name          */
extern befig_list  *BVL_HEDFIG;

extern FILE *vhd_bcompin;
extern int   vhd_bcompparse();

extern FILE *vbe_bcompin;
extern int   vbe_bcompparse();

# endif

