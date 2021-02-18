/* ###--------------------------------------------------------------### */
/*	                                                                     */
/* file		: bvl_util.h                                                */
/* date		: Jun 15 1992                                               */
/* version	: v102                                                      */
/* author	: Pirouz BAZARGAN SABET	                                    */
/* description	: This file contains declaration of global and external	*/
/*		  variables and, functions used in `bvl_util.c`		               */
/*									                                             */
/* ###--------------------------------------------------------------### */
	
extern char               BVL_ERRFLG;	   /* Error flag                    */
extern char               BVL_CURFIL[];	/* current file name	            */

void bvl_error (int code, char *str1);
void vbe_bcomperror (char *str);
void vhd_bcomperror (char *str);
char *bvl_vhdlname (char *name);
char *bvl_vectorize (char *name);
void *bvl_vectnam (void *pt_list, int *left, int *right, char **name, char type);
char *bvl_printabl (char *chaine);
char *bvl_abl2str (chain_list *expr, char *chaine, int *size_pnt);
char *bvl_abl2strbool (chain_list *expr, char *chaine, int *size_pnt);
void bvl_freenames(void);

char *vhd_bitToHex(char *str);
char *vhd_getBitStr(char *str, char *buf);

void bvl_drivecorresp(char *name, befig_list *ptbefig, int vlog);

