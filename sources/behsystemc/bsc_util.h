/****************************************************************************/
/*                                                                          */
/* file         : bsc_util.h                                                */
/* date         : March 2003                                                */
/* version      : v100                                                      */
/* author(s)    : PINTO A.                                                  */
/*                                                                          */
/* description  : SystemC HDL driver                                        */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*{{{                    Defines                                            */
/****************************************************************************/
#define BSC_NAMDFN     0
#define BSC_NEWDFN     1
#define BSC_PNTDFN     7        /* pnt_val field of dct_recrd               */

/*}}}************************************************************************/
/*{{{                    Global Variables                                   */
/****************************************************************************/
extern char               BSC_ERRFLG;    /* Error flag                      */
extern char               BSC_CURFIL[];  /* current file name               */

/*}}}************************************************************************/
/*{{{                    Functions                                          */
/****************************************************************************/
void  bsc_error (int code, char *str1);
void  bsc_bcomperror (char *str);
char *bsc_name (char *name);
char *bsc_vectorize (char *name);
void *bsc_vectnam (void *pt_list, int *left, int *right, char **name, char type);
char *bsc_printabl (char *chaine);
char *bsc_abl2str (chain_list *expr, char *chaine, int *size_pnt);
char *bsc_abl2strbool (chain_list *expr, char *chaine, int *size_pnt);
void  bsc_freenames(void);
char *bsc_getBitStr(char *str);


/*}}}************************************************************************/
