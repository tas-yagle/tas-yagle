/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_headers.h                                               */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

#ifdef AVERTEC
#include AVT_H
#endif

#include BEH_H
#include BEF_H
#include BEG_H
#include BHL_H
#include BVL_H
#include MUT_H
#include MLO_H
#include MLU_H
#include MSL_H

#include LOG_H
#include ELP_H
#include CBH_H

#ifndef WITHOUT_TAS
#include "yag_timing.h"
#endif

#include INF_H
#include YAG_H
#include INF_H
#include FCL_H
#include GEN_H

#include "yag_graph.h"
#include "yag_supply.h"
#include "yag_analyse.h"
#include "yag_befig.h"
#include "yag_chain.h"
#include "yag_cells.h"
#include "yag_cellbeh.h"
#include "yag_cone.h"
#include "yag_constrain.h"
#include "yag_duals.h"
#include "yag_detect.h"
#include "yag_drivers.h"
#include "yag_bus.h"
#include "yag_error.h"
#include "yag_instance.h"
#include "yag_latch.h"
#include "yag_lofig.h"
#include "yag_loops.h"
#include "yag_mux.h"
#include "yag_resolve.h"
#include "yag_split.h"
#include "yag_hierbeh.h"
#include "yag_test.h"
#include "yag_trans.h"
#include "yag_useabl.h"
#include "yag_util.h"
#include "yag_utilbeh.h"
#include "yag_utilcone.h"
#include "yag_utillo.h"

