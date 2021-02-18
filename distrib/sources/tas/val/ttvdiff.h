/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVDIFF Version 1                                           */
/*    Fichier : ttvdiff.h                                                   */
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
#include <stdarg.h>
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

#define DIFF_OPT_NDELAY     ((long) 0x00000001)
#define DIFF_OPT_NSLOPE     ((long) 0x00000002)
#define DIFF_OPT_NCAPA      ((long) 0x00000004)
#define DIFF_OPT_NETNAME    ((long) 0x00000008)
#define DIFF_OPT_DTX        ((long) 0x00000010)
#define DIFF_OPT_NDTX       ((long) 0xFFFFFFEF)
#define DIFF_OPT_NTTX       ((long) 0x00000020)
#define DIFF_OPT_MODEL      ((long) 0x00000040)
#define DIFF_OPT_INFO       ((long) 0x00000080)
#define DIFF_OPT_DELTAD     ((long) 0x00000100)
#define DIFF_OPT_DELTAS     ((long) 0x00000200)
#define DIFF_OPT_DELTAC     ((long) 0x00000400)
#define DIFF_OPT_DELTA      ((long) 0x00000800)
#define DIFF_OPT_DMASK      (DIFF_OPT_DELTA|DIFF_OPT_DELTAD|DIFF_OPT_DELTAS)
#define DIFF_OPT_DELTADP    ((long) 0x00001000)
#define DIFF_OPT_DELTASP    ((long) 0x00002000)
#define DIFF_OPT_DELTACP    ((long) 0x00004000)
#define DIFF_OPT_DELTAP     ((long) 0x00008000)
#define DIFF_OPT_DMASKP     (DIFF_OPT_DELTAP|DIFF_OPT_DELTADP|DIFF_OPT_DELTASP)
#define DIFF_OPT_ICHANGE    ((long) 0x00010000)
#define DIFF_OPT_IMISS      ((long) 0x00020000)
#define DIFF_OPT_IEXTRA     ((long) 0x00040000)
#define DIFF_OPT_HELP       ((long) 0x00080000)
#define DIFF_OPT_X          ((long) 0x00100000)
#define DIFF_OPT_XIN        ((long) 0x00200000)
#define DIFF_OPT_XOUT       ((long) 0x00400000)
#define DIFF_OPT_HIER       ((long) 0x00800000)
#define DIFF_OPT_NHIER      ((long) 0xFF7FFFFF)
#define DIFF_OPT_SCREEN     ((long) 0x01000000)
#define DIFF_OPT_NSIG       ((long) 0x02000000)
#define DIFF_OPT_TEST       ((long) 0x04000000)
#define DIFF_OPT_MODF       ((long) 0x08000000)
#define DIFF_OPT_MODC       ((long) 0x10000000)
#define DIFF_OPT_MODK       ((long) 0x20000000)
#define DIFF_OPT_MOD        ((long) 0x38000000)

#define DIFF_MAX_MODNB      32

extern long        DELTA;
extern long        DELTAS;
extern long        DELTAD;
extern long        DELTAC;
extern float       MODF[DIFF_MAX_MODNB]; 
extern float       MODC[DIFF_MAX_MODNB]; 
extern float       MODK[DIFF_MAX_MODNB]; 
extern int         NBMODF;
extern int         NBMODC;
extern int         NBMODK;

#include "ttv_comp.h"

