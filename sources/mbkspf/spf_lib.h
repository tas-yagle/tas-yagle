/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf100.h                                                    */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

//#define SPF_DEBUG

#ifndef SPF
#define SPF


/* liste des inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

/* inclusions mbk */
#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H

/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

//extern lofig_list *spf_Annotate  __P((lofig_list *ptlofig, char *spf_file)) ;
void parsespf(char *figname);


#endif
