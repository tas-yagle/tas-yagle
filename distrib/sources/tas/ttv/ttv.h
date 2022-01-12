/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Version 1                                               */
/*    Fichier : ttv.h                                                       */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : DIOURY Karim                                              */
/*                                                                          */
/****************************************************************************/
/* fichier header a inclure dans tous les fichiers .c                       */
/****************************************************************************/

#ifndef __TTV_H__
#define __TTV_H__

/* les inclusions systemes */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <signal.h>

/* inclusion mbk */
#include MUT_H
#include MLO_H
#include RCN_H

/* inclusion inf */
#include INF_H

/* inclusion stm */
#include STM_H

/* inclusion rc */
#include TLC_H
#include TRC_H

/* inclusion techno */
#include ELP_H

/* inclusion mcc */
#include MCC_H

/* inclusions ttv */
#include TTV_H

/* inclusions ttv */
#include STB_H


/* inclusion fichier .h de ttv */
#include "ttv_alloc.h"
#include "ttv_drittv.h"
#include "ttv_parsttv.h"
#include "ttv_fig.h"
#include "ttv_critic.h"
#include "ttv_fact.h"
#include "ttv_error.h"
#include "ttv_util.h"
#include "ttv_falsepath.h"
#include "ttv_ctx.h"

#endif //__TTV_H__
