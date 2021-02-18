/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_beh.h                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Stephane Picault                                        */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef ZEN_BEH_H
#define ZEN_BEH_H

#define ZEN_HTSIZE                 100
#define ZEN_EXPLODEDBDD     ((pNode) 0xFFFFFFFF)

/*------------------------------------------------------------\
|                                                             |
|                           Functions                         |
|                                                             |
\------------------------------------------------------------*/

//extern  pNode            derivSigByIndex    ();

extern  void             display_befig      ( befig_list    *pt_befig    );
extern  void             display_BeMachin   ( befig_list    *beh        ,
                                              char          *name        );
extern  void             display_binode     ( pCircuit       ptCct      ,
                                              binode_list   *ptBinod     );
extern  void             display_biabl      ( biabl_list    *ptBiabl     );
extern  void             display_berin_ref  ( berin_list    *ptBeRin     );

extern  bebux_list      *searchBeBux        ( befig_list    *beh        ,
                                              char          *name        );
extern  bebus_list      *searchBeBus        ( befig_list    *beh        ,
                                              char          *name        );
extern  bereg_list      *searchBeReg        ( befig_list    *beh        ,
                                              char          *name        );
extern  beout_list      *searchBeOut        ( befig_list    *beh        ,
                                              char          *name        );
extern  berin_list      *searchBeRin        ( befig_list    *beh        ,
                                              char          *name        );
extern  beaux_list      *searchBeAux        ( befig_list    *beh        ,
                                              char          *name        );

extern  befig_list      *zen_remplacebebus  ( befig_list    *pt_befig    );
extern  befig_list      *zen_remplacebebux  ( befig_list    *pt_befig    );

        void             zen_mbddtot        ( befig_list    *pt_befig    );
extern  void             zen_makbehbdd      ( befig_list    *pt_befig    );
extern  chain_list      *zen_cutAbl         ( chain_list    *abl         );
extern  pNode            zen_ablToBddCct    ( pCircuit       pC         ,
                                              chain_list    *expr       /*,
                                              pCircuit       iCct */       );
        void             zen_indexbdd       ( befig_list    *pt_fig      );
        void             zen_initCct        ( befig_list    *befig       );

#endif

