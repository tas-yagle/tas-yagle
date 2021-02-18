/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : zen                                                         */
/*    Fichier : zen_all.h                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#ifndef ZEN_ALL_H
#define ZEN_ALL_H

#define RET(X)        res = (X); goto fin;

extern  int ZEN_FAST;
extern  int ZEN_LEVEL;

// added to prevent warning
int          zen_simulNod           ( zennod_list *znod         ,
                                      int          level        );
int          zen_simulOutNod        ( zennod_list *znod         ,
                                      zennod_list *inznod       ,
                                      int          level        );

int          toto                   ( zennod_list *ZenNod       ,
                                      char         Value        );
int          zen_nbResOne           ( pNode        Bdd          );
int          zen_nbResZero          ( pNode        Bdd          );
int          zen_algo1              ( zenfig_list *ZenFig       );
int          zen_algo1Loc           ( zenfig_list *zfig         ,
                                      chain_list  *Path         );
int          zen_algo2              ( zenfig_list *ZenFig       );
int          zen_algo2Loc           ( zenfig_list *ZenFig       );
int          zen_resteIn            ( zenfig_list *ZenFig       );
int          zen_impderiv           ( zenfig_list *zfig         ,
                                      chain_list  *path         );
chain_list  *zen_searchSetNod       ( zennod_list *znod         );
chain_list  *zen_searchUnSetNode    ( zennod_list *ChainNod     );
int          zen_decision           ( zenfig_list *ZenFig       ,
                                      zennod_list *Node         ,
                                      char         Value        );
chain_list  *zen_desimplic          ( chain_list  *Chain        );
int          zen_setNodValue        ( zennod_list *ZenNod       ,
                                      char         Value        ,
                                      int          level        );
void         vv                     ( chain_list  *chain        ,
                                      char        *string       );
pNode        zen_deriveBdd          ( zennod_list *nod          ,
                                      zennod_list *nextNod      );
void         zen_setDelay           ( int          delay        );
int          zen_doBddFSetNod       ( zennod_list *zennod       );
chain_list  *zen_freechain          ( chain_list  *chainx       );
int          toto2one               ( pNode        bdd          ,
                                      chain_list  *inList       ,
                                      zennod_list *zennod       );
int          toto2zero              ( pNode        bdd          ,
                                      chain_list  *inList       ,
                                      zennod_list *zennod       );
void         zen_cleanTreatedNod    ( void                      );

chain_list  *zen_inWithoutAList     ( zennod_list *zennod       ,
									  zennod_list *A            );
void         zen_cleanNBdd          ( zenfig_list *zenfig	    );
int          zen_ZNodGotValue       ( zennod_list *znod	        );
int          zen_bddIsTerm1         ( pNode        bdd	        );
int          zen_bddIsTerm0         ( pNode        bdd	        );
chain_list  *zen_buildAbl           ( zennod_list *znod         ,
									  chain_list  *inChain	    );
int          zen_simulAOutNod       ( zennod_list *znod         ,
                                      int          level        );
int          zen_reduceBdd          ( zenfig_list *zfig         ,
                                      zennod_list *znod	        );


#endif
