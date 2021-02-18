/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : ETAS Version 1                                              */
/*    Fichier : etas.h                                                      */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

/* les inclusions systemes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <editline.h>
#include <signal.h>
#include <setjmp.h>


/* inclusion mbk */
#include MUT_H
#include MLO_H

/* inclusions avt */

#ifdef AVERTEC
#include AVT_H
#endif

/* inclusions ttv */
#include STM_H
#include TTV_H

#define GET_MAX           ((char)'M')
#define GET_CRITICMAX     ((char)'m')
#define GET_PATH          ((char)'T')
#define GET_CRITICPATH    ((char)'t')
#define GET_PARA          ((char)'P')
#define GET_CRITICPARA    ((char)'p')
#define GET_LATCHLIST     ((char)'L')
#define GET_CMDLIST       ((char)'Q')
#define GET_PRECHARGELIST ((char)'R')
#define GET_BREAKLIST     ((char)'B')
#define GET_CONLIST       ((char)'C')
#define GET_SIGLIST       ((char)'S')
#define GET_FIGLIST       ((char)'F')
#define GET_DELAYLIST     ((char)'d')

#define GET_NODELEVEL(node) (((node->TYPE & TTV_NODE_UP) == TTV_NODE_UP) \
                            ? 'U' : 'D')
