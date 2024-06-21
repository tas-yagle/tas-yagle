/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_util.h                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef ZEN_UTIL_H
#define ZEN_UTIL_H

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H
#include ZEN_H
extern  ptype_list      *zen_loadPath       ( char          *FileName   );
        char            *zen_traductName    ( char          *name       );  
        char             zen_getSlope       ( char          *line       );

#endif
