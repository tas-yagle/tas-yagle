/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_zennod.h                                                */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"

#ifndef ZEN_ZENNOD_H
#define ZEN_ZENNOD_H

zennod_list     *zen_alloczennod        ( void                      );
void             zen_freezennod         ( zennod_list   *listzennod );
void             zen_viewzennod         ( zennod_list   *node       );
zennod_list     *zen_searchzennod       ( zenfig_list   *ZenFig     ,
                                          char          *name       );
zennod_list     *zen_addzennod          ( zennod_list   *lastzennod ,
                                          char          *name       ,
                                          chain_list    *abl        );
zennod_list     *zen_addOutNode         ( zenfig_list   *zenfig     ,
                                          beout_list    *beout      ,
                                          char          *name       ,
                                          chain_list   **abl        );
zennod_list     *zen_addRegNode         ( zenfig_list   *zenfig     ,
                                          bereg_list    *bereg      ,
                                          char          *name1      ,
                                          char          *name2      ,
                                          chain_list   **abl        );
zennod_list     *zen_addAuxNode         ( zenfig_list   *zenfig     ,
                                          beaux_list    *beaux      ,
                                          char          *name       ,
                                          chain_list   **abl        );
zennod_list     *zen_addRinNode         ( zenfig_list   *zenfig     ,
                                          char          *name       );

#endif
