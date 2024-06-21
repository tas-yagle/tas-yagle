/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVREN Version 1                                            */
/*    Fichier : ttvren.h                                                    */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

/* les inclusions systemes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

/* inclusion mbk */
#include MUT_H
#include MLO_H

#ifdef AVERTEC
#include AVT_H
#endif

/* inclusions ttv */
#include STM_H
#include TTV_H

#define REN_OPT_EXT        ((long) 0x00000001)
#define REN_OPT_DTX        ((long) 0x00000002)
#define REN_OPT_TTX        ((long) 0x00000004)
#define REN_OPT_DTXTTX     ((long) REN_OPT_DTX|REN_OPT_TTX)
#define REN_OPT_TOPLEVEL   ((long) 0x00000008)
#define REN_OPT_HIER       ((long) 0x00000010)
#define REN_OPT_HELP       ((long) 0x00000020)
#define REN_OPT_STM        ((long) 0x00000040)
#define REN_OPT_SILENT     ((long) 0x00000080)

extern  char            NEWNAME[1024];
extern  ttvfig_list     *TTVFIG;

#include "ttv_ren.h"

