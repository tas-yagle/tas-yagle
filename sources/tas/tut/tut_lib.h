/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Produit : LIB Version 1.00                                              */
/*    Fichier : lib100.h                                                      */
/*                                                                            */
/*    (c) copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gilles Augustins                                            */
/*                                                                            */
/******************************************************************************/

#ifndef TUT
#define TUT

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include MUT_H

#ifdef AVERTEC
#include AVT_H
#endif

/******************************************************************************/
/*     defines                                                                */
/******************************************************************************/
#define TUT_MINTEMPVOLT     -274.0

/******************************************************************************/
/*     globals                                                                */
/******************************************************************************/
extern ht  *tut_tablasharea;     
extern double TUT_MINVOLT ;                    
extern double TUT_MAXVOLT ;
extern double TUT_MINTEMP ;
extern double TUT_MAXTEMP ;

/******************************************************************************/
/*     structures                                                             */
/******************************************************************************/

/******************************************************************************/
/*     functions                                                              */
/******************************************************************************/


extern void tut_parse (char*, int);
char *lib_unsuffix (char *name, char* suffix);

#endif

