/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Fichier : stm_API_cache.h                                             */
/*                                                                          */
/*    (c) copyright 2003 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef STM_API_CACHE_H
#define STM_API_CACHE_H

#include <math.h>
#ifdef Solaris
#include <ieeefp.h>
#endif

#include STM_H
#include GEN_H

typedef struct stm_current
{
  struct stm_current    *NEXT;
  char                  *INS;
  timing_table          *DELAY;
  timing_table          *SLOPE;
  double                 VSLOPE;
  double                 VLOAD;
  double                 VDELAY;
  int                    TRESRES;
  char                   TYPE;
}
stm_current;

typedef struct stm_tree4cache
{
  struct stm_tree4cache *NEXT;
  char                  *ORIG;
  char                  *DEST;
  char                  *DIRECTION;
  char                   FLAG;
  double                 DELAY;
  double                 SLOPE;
}
stm_tree4cache;

#define          STM_THRESHOLD_MATCH_Y   0x1
#define          STM_THRESHOLD_MATCH_X   0x2
#define          STM_THRESHOLD_MATCH_XY  0x3
#define          STM_THRESHOLD_MATCH     0x4
#define          STM_THRESHOLD_NOMATCH   0x8

#define          STM_CACHE_SIZE          1

#define          STM_MODE_DELAY          1
#define          STM_MODE_SLOPE          2
#define          STM_ACCURACY            ((double) 1e-4)

//#define          _PRINT

#define          STM_CURRENT_TYPE    ((long) 0xc1c01163)

#endif
