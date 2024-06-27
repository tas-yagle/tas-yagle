/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc.h                                                       */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <editline/readline.h>
#ifdef Linux
#include <values.h>
#else
#include <float.h>
#endif

#include MUT_H
#include EQT_H
#include MLO_H
#include MLU_H
#include ELP_H
#include MSL_H
#include SIM_H
#include MCC_H

#ifdef AVERTEC
#include AVT_H
#endif

extern long MCC_SPICEUD_FITA ;
extern long MCC_SPICEDU_FITA ;
extern long MCC_SPICEUD_FITCGNOCAPA ;
extern long MCC_SPICEDU_FITCGNOCAPA ;
extern long MCC_SPICEUD_FITCG ;
extern long MCC_SPICEDU_FITCG ;
extern long MCC_SPICEUD_FITDN ;
extern long MCC_SPICEDU_FITDN ;
extern long MCC_SPICEDU_FITDP ;
extern long MCC_SPICEUD_FITDP ;
extern long MCC_SPICEFUD_FITA ;
extern long MCC_SPICEFDU_FITA ;
extern long MCC_SPICEFUD_FITCGNOCAPA ;
extern long MCC_SPICEFDU_FITCGNOCAPA ;
extern long MCC_SPICEFUD_FITCG ;
extern long MCC_SPICEFDU_FITCG ;
extern long MCC_SPICEFUD_FITDN ;
extern long MCC_SPICEFDU_FITDN ;
extern long MCC_SPICEFDU_FITDP ;
extern long MCC_SPICEFUD_FITDP ;

extern int mcc_retkey                   __P((char *)) ;
extern double mcc_getspidouble          __P((char *,
                                             char **
                                           )) ;
extern int mcc_getspiparam              __P((char *,
                                             char *,
                                             char *
                                           )) ;
extern char *mcc_getspiline             __P((FILE *,
                                             char *
                                           )) ;
extern char *mcc_getfilename            __P((char *)) ;
/*
extern char *mcc_getjoker               __P((char *,
                                             char *
                                           )) ;
                                           */
/*
extern char *mcc_getarg                 __P((char *,
                                             char *,
                                             int
                                           )) ;
                                           */
extern char *mcc_getfisrtarg            __P((char *,
                                             char *
                                           )) ;
extern char *mcc_initstr                __P((char *)) ;
//extern void mcc_initcalcparam           __P(()) ;
//extern void mcc_calcspiparam            __P(()) ;
//extern char *mcc_getline                __P((char *)) ;
extern int mcc_gettab                   __P((
                                             char *,
                                             char **,
                                             int,
                                             int
                                           )) ;
extern int mcc_getint                   __P((
                                             char *,
                                             int,
                                             int,
                                             int
                                           )) ;
extern double mcc_getdouble             __P((char *,
                                             double,
                                             double,
                                             double
                                           )) ;
extern char *mcc_getstr                 __P((char *,
                                             char *
                                           )) ;
extern char *mcc_getword                __P((char *,
                                             char *
                                           )) ;
extern FILE *mcc_fopen                  __P((char *,
                                             char *
                                           )) ;
extern void mcc_fclose                  __P((FILE*,
                                             char *
                                           )) ;
extern void mcc_drvparam                __P(()) ;
extern int mcc_prsparam                 __P(()) ;

extern void mcc_runspice                __P((int)) ;
extern void mcc_genspi                  __P(( int ,
                                              elp_lotrs_param*,
                                              elp_lotrs_param*
                                           )) ;
extern int mcc_genelp                      (int spice, int vt, double tec, int aut, int fit, mcc_modellist **modeln, mcc_modellist **modelp,elp_lotrs_param *lotrsparam_n,elp_lotrs_param *lotrsparam_p);
