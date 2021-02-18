/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_zenfig.h                                                */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"

#ifndef ZEN_ZENFIG_H
#define ZEN_ZENFIG_H

zennod_list     *zen_addNodeToZenfig    ( zenfig_list   *zenfig         ,
                                          char          *ablName        ,
                                          char          *ablName2       ,
                                          void          *be             ,
                                          int            type           ,
                                          chain_list   **abl             );
zennod_list     *zen_newNode            ( befig_list    *BeFigure       ,
                                          zenfig_list   *ZenFig         ,
                                          char          *NameAbl        ,
                                          char          *NameAbl2        );
zenfig_list     *zen_createzenfig       ( befig_list    *BeFigure       ,
                                          ptype_list    *Path            );
zenfig_list     *zen_alloczenfig        ( void                           );
zenfig_list     *zen_addzenfig          ( zenfig_list   *lastzenfig     ,
                                          char          *name            );  
void             zen_freezenfig         ( zenfig_list   *listzenfig      );
void             zen_viewzenfig         ( zenfig_list   *figure          );

#endif
