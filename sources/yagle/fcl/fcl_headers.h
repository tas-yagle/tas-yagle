/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_headers.h                                               */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#ifndef MACOS
#include <sys/types.h>
#include <sys/time.h>
#endif
#include <sys/resource.h>

#include MUT_H
#include MLO_H
#include MLU_H
#include LOG_H
#include MSL_H

#include BEH_H
#include BHL_H
#include BEF_H
#include BVL_H
#include INF_H

#ifdef AVERTEC
#include AVT_H
#endif

#include "yag_lib.h"
#include "fcl_lib.h"
#include "genius_lib.h"

#include "fcl_constants.h"

#include "fcl_library.h"
#include "fcl_extendmsl.h"
#include "fcl_findcell.h"
#include "fcl_partition.h"
#include "fcl_phase1.h"
#include "fcl_matrix.h"
#include "fcl_phase2.h"
#include "fcl_solutions.h"
#include "fcl_util.h"
#include "fcl_constrain.h"


