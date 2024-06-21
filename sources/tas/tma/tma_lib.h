/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tma_lib.h                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#ifndef TMA
#define TMA

/* LISTE DES INCLUDE */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>


/****************************************************************************/
/* declaration de fonction */

typedef struct tma_context
    {
     struct tma_context *NEXT ;
     char     TMA_TYPE_TTVFIG ;
    }
tma_context_list;

typedef struct tma_leak_pow {
    struct tma_leak_pow *NEXT;
    float                VALUE;
    chain_list          *PATTERN;
}tma_leak_pow_list;

extern tma_context_list *TMA_CONTEXT;

extern int          tmaenv      (long filetype);
extern void         tma_yaginit ();
extern void         tma_GetTmaOpt (int argc, char *argv[]);
extern ttvfig_list *tma_CreateBlackBox    (ttvfig_list *fig, char *suffix);
extern void         tma_AddInsertDelays   (ttvfig_list *blackbox, ttvfig_list *fig);
extern void         tma_DetectClocksFromBeh   (ttvfig_list *fig, befig_list *befig);
extern void         tma_UpdateSetReset    (ttvfig_list *fig, befig_list *befig);
extern void         tma_DupConnectorList  (ttvfig_list *newfig, ttvfig_list *fig, chain_list *filter);
extern ttvfig_list   *tma_DupTtvFigHeader   (char *newfigname, ttvfig_list *fig);
#endif
