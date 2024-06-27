/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS version 5                                               */
/*    Fichier : tas.h                                                       */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fichier header a inclure dans tous les fichiers .c                       */
/****************************************************************************/

#ifndef _TAS_H_
#define _TAS_H_

/* les inclusions systemes */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#ifdef Solaris
#include <ieeefp.h>
#endif

/* inclusion mbk */
#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H

/* inclusion spice */
#include MSL_H

/* inclusion information */

#ifdef AVERTEC
#include AVT_H
#endif

/* inclusion information */
#include INF_H

/* inclusion techno */
#include ELP_H

/* inclusion cns */
#include CNS_H

/* inclusion yagle */
#include YAG_H

/* inclusions tlc */
#include TLC_H

/* inclusions trc */
#include TRC_H

/* inclusions stm */
#include STM_H

/* inclusions ttv */
#include TTV_H

/* inclusions mcc */
#include MCC_H

/* inclusions sim */
#include SIM_H

/* inclusions tas */
#include TAS_H

/* inclusions fcl */
#include FCL_H

/* inclusions beh */
#include BEH_H

/* inclusions cbh */
#include CBH_H

/* inclusions tut */
#include TUT_H

/* inclusions lib */
#include LIB_H

/* inclusions tlf */
#include TLF_H

/* inclusions stb */
#include STB_H

/* inclusion fichier .h de tas */
#include "tas_class.h"
#include "tas_err.h"
#include "tas_main.h"
#include "tas_matherr.h"
#include "tas_pre.h"
#include "tas_tec.h"
#include "tas_tp.h"
#include "tas_tpiv.h"
#include "tas_util.h"
#include "tas_visu.h"
#include "tas_alloc.h"
#include "tas_parscns.h"
#include "tas_builthfig.h"
#include "tas_drislo.h"
#include "tas_lut.h"
#include "tas_spice.h"
#include "tas_noise.h"
#include "tas_models.h"
#include "tas_cnsmemory.h"

#endif //_TAS_H_
