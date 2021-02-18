/*===========================================================*\
||                                                           ||
|| Tool    :                     ZEN                         ||
||                                                           ||
|| File    :                  zen_main.c                     ||
||                                                           ||
|| Authors :               Stephane Picault                  ||
||                                                           ||
|| Date    :                   xx.xx.xx                      ||
||                                                           ||
\\===========================================================*/

#ifndef ZEN_101_H
#define ZEN_101_H

#include <stdio.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include LOG_H
#include BEH_H
#include BEF_H
#include BHL_H
#include BVL_H

#ifdef AVERTEC
#include AVT_H
#endif

/*------------------------------------------------------------\
|                                                             |
|                         Constants                           |
|                                                             |
\------------------------------------------------------------*/

#define ZEN_VALUE_UNKNOWN   'X'
#define ZEN_VALUE_ONE       '1'
#define ZEN_VALUE_ZERO      '0'
#define ZEN_VALUE_UP        'U'
#define ZEN_VALUE_DOWN      'D'

#define ZEN_BEAUX           ((int) 0x0001)
#define ZEN_BEREG           ((int) 0x0002)
#define ZEN_BEOUT           ((int) 0x0003)
#define ZEN_BERIN           ((int) 0x0004)

#define ZEN_BDDDONE         ((long) 0x00F00000)

extern  int             ZEN_TESTMODE;
extern  int             ZEN_TRACEMODE;
/*------------------------------------------------------------\
|                                                             |
|                           Macros                            |
|                                                             |
\------------------------------------------------------------*/

#define ZEN_ERROR(E,S)    zen_error  ((E),(S),__FILE__,__LINE__);
#define ZEN_WARNING(E,S)  zen_warning((E),(S),__FILE__,__LINE__);
#define ZEN_TRACE(E,S)    if (ZEN_TRACEMODE)\
                          zen_trace  ((E),(S),__FILE__,__LINE__);
#define ZEN_TST(E,S)      if (ZEN_TESTMODE) \
                          zen_test   ((E),(S),__FILE__,__LINE__);

/*------------------------------------------------------------\
|                                                             |
|                         Structures                          |
|                                                             |
\------------------------------------------------------------*/

/*------------------------------------------------------------\
|                                                             |
|                            Node                             |
|                                                             |
\------------------------------------------------------------*/

typedef struct zennod
{
  struct zennod     *NEXT;
  char              *NAME;   /*             nom               */
  long               INDEX;  /*    index du bdd associe       */
  pCircuit           CCT;
  pNode              BDD;    /*          bdd associe          */
  pNode              NBDD;
  pNode              SBDD;
  char               VALUE;  /*       valeur courante         */
  chain_list        *OUTNOD; /* liste des noeuds qu'il attack */
  chain_list        *INNOD;  /* liste des noeuds qui l'attack */  
  long               FLAGS; 
  int                NBONE;
  int                NBZER;
} zennod_list;

/*------------------------------------------------------------\
|                                                             |
|                           Figure                            |
|                                                             |
\------------------------------------------------------------*/

typedef struct zenfig
{
   struct zenfig  *NEXT;
   char           *NAME;      /*          nom               */
   struct zennod  *NOD;       /*          noeuds            */
   chain_list     *NOD_IN;    /* liste des noeuds "entrees" */
   chain_list     *NOD_OUT;   /* liste des noeuds "sorties" */
   chain_list     *NOD_PATH;  /* liste des noeuds "on path" */
   struct circuit *CIRCUI;    /* circuit  (pour les bdd)    */
   
}  zenfig_list;

extern  void         zen_setDelay       ( int        delay          );
extern  void         zen_freezennod     ();
extern  void         zen_freezenfig     ();
    
extern  zennod_list *zen_addzennod      ();
extern  zenfig_list *zen_addzenfig      ( );

extern  void         zen_viewzennod     ();
extern  void         zen_viewzenfig     ();

extern  zenfig_list *zen_createzenfig   ();
extern  int          zen_algo1          ();   
extern  int          zen_algo2          ();   

extern  void         zen_error          ();
extern  void         zen_warning        ();
extern  void         zen_trace          ();

extern  void         zen_makbehbdd      ( befig_list    *pt_befig    );
extern  char        *zen_traductName    ( char          *name       );  
extern  befig_list  *zen_remplacebebus  ( befig_list    *pt_befig    );
extern  befig_list  *zen_remplacebebux  ( befig_list    *pt_befig    );

# endif

