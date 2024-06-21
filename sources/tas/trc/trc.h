/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc.h                                                       */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#ifndef TRCH
#define TRCH

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <stdarg.h>
#ifdef Solaris
#include <ieeefp.h>
#endif
#include <alloca.h>
#ifdef Linux
#include <values.h>
#else
#include <float.h>
#endif

#include AVT_H
#include MUT_H
#include MLO_H
#include RCN_H
#include TRC_H
#include TLC_H
#include ELP_H
#include API_H
#include STM_H

#define RCX_VSSNI 0x52435811
// Défini un signal qu'on ne connait pas mais qu'on considère comme toujours
// actif.

#define RCX_LOCONINS 0x52435817
// Champs user des losig dans les instances. Pointe vers un connecteur externe.

#define RCX_LOCONONEPNODE 0x52435818
// Indique qu'il ne faut considérer que le premier pnode sur ce locon

/*************************************** FLAG *********************************/

#define MODELRCN     ((long) 0) // Le champ view contient une vue RCN.
#define RCXCTCDONE   ((long) 1) // Le test d'existance des CTC est fait.
#define SAVESIG      ((long) 2) // Ce signal doit etre sauvegardé.
#define RCXERROR     ((long) 3) // On ne sait pas calculer des TP sur le signal
#define RCXNODELAY   ((long) 5) // On a pas de délai.
#define RCXHASLOOP   ((long) 6) // Le réseau RC contient des boucles.
#define RCXHASNOLOOP ((long) 7) // Le réseau RC ne contient pas de boucles.
#define RCXCTKBASIC  ((long) 8) // Utilise un model simple et robuste.
#define RCXHASNOGCTC ((long) 9) // Il n'y a pas de ctc globales sur la vue RCN.
#define RCXLOADCAPA  ((long)10) // La charge doit être une simple capa.
#define RCXLOADPI    ((long)11) // La charge doit être une cellule en pi.
#define RCXBREAKLOOP ((long)12) // Il faut casser les boucles sur le réseau RC.
#define RCXNOCTC     ((long)13) // Il faut forcer les ctc à la masse.
#define RCXNOWIRE    ((long)14) // Il ne faut pas prendre en compte les RC.
#define RCXTREATEDNI ((long)15) // On a passé le checkcrosstalk sur ce siganl.
#define RCXIGNORE    ((long)16) // Le signal a été évalué par Genius. On y touche pas
#define RCXMLTDRIVER ((long)17) // Il y a plusieurs cones driver.
#define RCXFORCERC   ((long)18) // Il y a plusieurs cones driver.

#define SETFLAG(m,b) (m = m | (1<<b))
#define CLEARFLAG(m,b) (m = m &(~(1<<b)))
#define GETFLAG(m,b) (m & (1<<b))

/******************************************************************************/

#define RCXFILEEXTENTION "rcx"


#define RCXFILECAR_GND   ((char)'G')
#define RCXFILECAR_NI    ((char)'N')
#define RCXFILECAR_XTALK ((char)'X')


// Si cette variable existe, effectue des tests de conformité des structures
// de données.

#define RCX_CHECK

#include "trc_prsrcx.h"
#include "trc_drvrcx.h"
#include "trc_rcx.h"
#include "trc_util.h"
#include "trc_rcxloins.h"
#include "trc_hier.h"
#include "trc_fifo.h"
#include "trc_delay.h"
#include "trc_awepolynomes.h"
#include "trc_awematrice.h"
#include "trc_awe.h"
#include "trc_cache.h"
#include "trc_noise.h"
#include "trc_delayloop.h"
#include "trc_capa.h"
#include "trc_spice.h"
#include "trc_rcxi.h"
#include "trc_awe_generalized_moment.h"
#include "trc_piload_generalized.h"

extern trc_fifo *FIFOAWE ;

#endif
