/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_util.c                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef CBH_UTIL_H
#define CBH_UTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include AVT_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include MLO_H
#include CBH_H
#include LIB_H
#include "cbh_cmp.h"

/* taille maximum de nom pour l'affichage */
#define			CBH_MAXNL	13
#define         CBH_LOINS   ((int) 0xF120)
#define         CBH_LOFIG   ((int) 0xF121)

void            cbh_freechain        ( chain_list  *chain       );
void            cbh_freechain2       ( chain_list  *chain       ,
                                       int         display      );
void            cbh_traversetree     ( chain_list   *branch     ,
                                       int          n           ,
                                       int          mask        );
int             cbh_isclassedlofig   ( chain_list  *head        ,
                                       lofig_list  *lofig       );
pCircuit        cbh_docct            ( lofig_list  *lofig       );
void            cbh_getLoconFromLofig( lofig_list  *lofig       ,
                                       chain_list  **inout      ,
                                       int         *nbinout     );
void            cbh_getLoconFromLoins( loins_list  *loins       ,
                                       chain_list  **inout      ,
                                       int         *nbinout     );
void            cbh_getLocon         ( void        *lo_figins   ,
                                       chain_list **inout       ,
                                       int         *nbinout     ,
                                       int          type        );
void            cbh_freeInOutList    ( chain_list **inout       );
void            cbh_delcct           (                          );
void            cbh_freeLofig        ( lofig_list  *lofig       );
void            cbh_delcombtolofig   ( lofig_list  *lofig       );

#endif
