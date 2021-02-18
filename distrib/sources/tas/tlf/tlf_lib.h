/******************************************************************************************/
/*                                                                                        */
/*                      Chaine de CAO & VLSI   AVERTEC                                    */
/*                                                                                        */
/*    Produit : TLF Version 1.00                                                          */
/*    Fichier : tlf100.h                                                                  */
/*                                                                                        */
/*    (c) copyright 2000 AVERTEC                                                          */
/*    Tous droits reserves                                                                */
/*                                                                                        */
/*    Auteur(s) : Gilles Augustins                                                        */
/*                                                                                        */
/******************************************************************************************/

#ifndef TLF_DEBUG
#define TLF_DEBUG 0
#endif

#ifndef TLF
#define TLF


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

/* mbk */
#include MUT_H
#include MLO_H
#include MLU_H
#include ELP_H
#include MCC_H
#include TUT_H
#define API_HIDE_TOKENS
#include STM_H
#include TTV_H
#include BEH_H
#include CBH_H

#ifdef AVERTEC
#include AVT_H
#endif

/******************************************************************************************/
/*     defines                                                                            */
/******************************************************************************************/

/* available time unit for TLF_TIME_UNIT                                                  */
#define TLF_NS 'N'  /* nanosecond                                                         */
#define TLF_PS 'P'  /* picosecond                                                         */

/* available capa unit for TLF_CAPA_UNIT                                                  */
#define TLF_PF 'P'  /* picofahrad                                                         */
#define TLF_FF 'F'  /* femtofahrad                                                        */ 

/* available time unit for TLF_RES_UNIT                                                   */
#define TLF_KOHM 'K'  /* kiloohm                                                          */
#define TLF_OHM  'O'  /* ohm                                                              */
                       
#define TLF_CAPA_DIV 1000
/******************************************************************************************/
/*     globals (to set)                                                                   */
/******************************************************************************************/

extern char *TLF_IN ;            /* input format, default is .tlf                         */
extern char  TLF_CAPA_UNIT ;     /* capacitance unit in tlf file, default is picofahrad   */
extern char  TLF_TIME_UNIT ;     /* time unit in tlf file, default is nanosecond          */
extern char  TLF_RES_UNIT ;      /* resistance unit in tlf file, default is kiloohm       */
extern char *TLF_TUNIT;
extern char *TLF_TOOLNAME ;      /* default is "tlf2ttv"                                  */
extern char *TLF_TOOLVERSION ;   /* default is "1.0"                                      */
extern ht  *tut_tablasharea;    
                        
/******************************************************************************************/
/*     functions                                                                          */
/******************************************************************************************/

extern void        parsetlf3   __P((char *filename)) ;
extern void        parsetlf4   __P((char *filename)) ;
extern void        tlfenv      __P(()) ;
extern chain_list *tlf_load    __P((char *filename, int version)) ; /* load all the cells */
extern void        tlf_gcf2stb __P((char *gcf_file)) ;
extern void        tlf_exit    __P(()) ;               /* exit : free memory              */
extern void        tlf3_drive  __P((chain_list*, chain_list*, char*, char*)) ;
extern void        tlf4_drive  __P((chain_list*, chain_list*, char*, char*)) ;
extern void        tlfarea_parse __P((char*));
extern char       *tlf_getlibname(void);
#endif

