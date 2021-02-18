/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.00                                            */
/*    Fichier : trc_delayloop.c                                             */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.27 $
Author   : $Author: gregoire $
Date     : $Date: 2007/07/04 12:49:40 $

*/

#include "trc.h"

/*
Renvoie une estimation à la louche du delais RC d'un réseau lorsque celui
contient des boucles. Il s'agit du délai d'Elmore maximum entre 2 locon du
réseau RC pour le délai maximum, et du délai d'Elmore minimum entre 2 locon du
réseau RC pour le délai minimum.
*/
int TRC_MARK_CON = 0;                      //tmp for test

void rcx_delayloop( lofig_list *lofig,      // La figure
                    losig_list *losig,      // Le signal
                    locon_list *locon,      // Le locon emetteur
                    rcx_slope  *slopemiller,   //                maximum
                    RCXFLOAT    smax,       // Front d'entrée minimum
                    RCXFLOAT    smin,       // Front d'entrée minimum
                    RCXFLOAT    vmax,       // La tension finale
                    RCXFLOAT    vt,         // Le seuil d'un transistor
                    RCXFLOAT    extcapa,    // La capacité externe
                    RCXFLOAT   *dmax,       // Le délai maximum calculé
                    RCXFLOAT   *dmin,       //          minimum
                    RCXFLOAT   *fmax,       // Le front maximum calculé
                    RCXFLOAT   *fmin,       //          minimum
                    locon_list **cmax,      // Connecteur où dmax a été trouvé  
                    locon_list **cmin       // Connecteur où dmin a été trouvé  
                  )
{
  static losig_list  *lastcalllosig = NULL;
  static locon_list  *lastcalllocon = NULL;
  static RCXFLOAT     lastcalldmaxup;
  static RCXFLOAT     lastcalldminup;
  static RCXFLOAT     lastcalldmaxdw;
  static RCXFLOAT     lastcalldmindw;
  static locon_list  *lastcallcminup;
  static locon_list  *lastcallcmaxup;
  static locon_list  *lastcallcmindw;
  static locon_list  *lastcallcmaxdw;
  static char         lastup='N';
  static char         lastdw='N';
  rcx_list           *rcx;
  ptype_list         *ptl;

  RCXFLOAT            coefctc;

  rcx = getrcx( losig );
  if( rcx && GETFLAG( rcx->FLAG, RCXNOWIRE ) ) {
    RCX_PTRTESTANDSET( dmax, 0.0 )
    RCX_PTRTESTANDSET( dmin, 0.0 )
    RCX_PTRTESTANDSET( fmax, 0.0 )
    RCX_PTRTESTANDSET( fmin, 0.0 )
    RCX_PTRTESTANDSET( cmin, NULL )
    RCX_PTRTESTANDSET( cmax, NULL )
    return;
  }

  if( lastcalllosig == losig && lastcalllocon == locon) {
    if( slopemiller->SENS == TRC_SLOPE_UP && lastup == 'Y' ) {
      RCX_PTRTESTANDSET( dmax, lastcalldmaxup )
      RCX_PTRTESTANDSET( dmin, lastcalldminup )
      RCX_PTRTESTANDSET( fmax, lastcalldmaxup + smax )
      RCX_PTRTESTANDSET( fmin, lastcalldminup + smin )
      RCX_PTRTESTANDSET( cmin, lastcallcminup )
      RCX_PTRTESTANDSET( cmax, lastcallcmaxup )
      return;
    }
    if( slopemiller->SENS == TRC_SLOPE_DOWN && lastdw == 'Y' ) {
      RCX_PTRTESTANDSET( dmax, lastcalldmaxdw )
      RCX_PTRTESTANDSET( dmin, lastcalldmindw )
      RCX_PTRTESTANDSET( fmax, lastcalldmaxdw + smax )
      RCX_PTRTESTANDSET( fmin, lastcalldmindw + smin )
      RCX_PTRTESTANDSET( cmin, lastcallcmindw )
      RCX_PTRTESTANDSET( cmax, lastcallcmaxdw )
      return;
    }
  }

  if( lastcalllosig != losig || lastcalllocon != locon ) {
    // On change de signal, on reinitialise tout.
    lastcalllosig = losig;
    lastcalllocon = locon;
    lastcalldmaxup = -1.0;
    lastcalldminup = -1.0;
    lastcalldmaxdw = -1.0;
    lastcalldmindw = -1.0;
    lastup='N';
    lastdw='N';
    lastcallcmindw = NULL;
    lastcallcmaxdw = NULL;
    lastcallcminup = NULL;
    lastcallcmaxup = NULL;

    if( slopemiller->SENS == TRC_SLOPE_UP ) {
      RCX_PTRTESTANDSET( dmax, lastcalldmaxup );
      RCX_PTRTESTANDSET( dmin, lastcalldminup );
      RCX_PTRTESTANDSET( cmin, lastcallcminup );
      RCX_PTRTESTANDSET( cmax, lastcallcmaxup );
    }
    else {
      RCX_PTRTESTANDSET( dmax, lastcalldmaxdw );
      RCX_PTRTESTANDSET( dmin, lastcalldmindw );
      RCX_PTRTESTANDSET( cmin, lastcallcmindw );
      RCX_PTRTESTANDSET( cmax, lastcallcmaxdw );
    }
  }

  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( !ptl ) {
    return;
  }

  rcn_lock_signal( lofig, losig );


  coefctc = rcx_getcoefctc( lofig,
                            losig,
                            slopemiller,
                            RCX_MAX,
                            extcapa,
                            NULL
                          );
                          
  rcx_makenoloop( losig );
  
  rcx_loopelmore( losig,
                  locon,
                  slopemiller,
                  smax,
                  smin,
                  vmax,      // La tension finale
                  vt,        // Le seuil d'un transistor
                  extcapa,   // La capacité externe
                  coefctc,
                  dmax,
                  dmin,
                  cmax,
                  cmin
                );

  rcx_clearnoloop( losig );

  if( slopemiller->SENS == TRC_SLOPE_UP ) {
    if( dmax ) lastcalldmaxup = *dmax ;
    if( dmin ) lastcalldminup = *dmin ;
    if( cmax ) lastcallcmaxup = *cmax;
    if( cmin ) lastcallcminup = *cmin;
    lastup = 'Y';
  }
  else {
    if( dmax ) lastcalldmaxdw = *dmax ;
    if( dmin ) lastcalldmindw = *dmin ;
    if( cmax ) lastcallcmaxdw = *cmax;
    if( cmin ) lastcallcmindw = *cmin;
    lastdw = 'Y';
  }

  if( dmax ) RCX_PTRTESTANDSET( fmax, smax + *dmax );
  if( dmin ) RCX_PTRTESTANDSET( fmin, smin + *dmin );

  rcn_unlock_signal( lofig, losig );
}

/* Place un ptype RCX_NOWAY dans les résistances pour empécher un parcours des
boucles. */

void rcx_makenoloop( losig_list *losig )
{
  lowire_list   *wire;
  lonode_list   *ptnode;
  
  clearallwireflag( losig, RCN_FLAG_PASS );
  
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    ptnode = getlonode( losig, wire->NODE1 );
    if( !GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
      rcx_makenolooprec( losig, ptnode, NULL );
  }
}

void rcx_makenolooprec( losig_list *losig, 
                        lonode_list *start, 
                        lowire_list *wire 
                      )
{
  lonode_list   *ptnode;
  chain_list    *chain;
  lowire_list   *wdown;
  long           ndown;

  ptnode = start;

  
  if( wire ) {
 
    /* Si on arrive directement sur un noeud où on est déjà passé. */
    if( RCN_GETFLAG( start->FLAG, RCN_FLAG_PASS ) ) {
      if( !getptype( wire->USER, RCX_NOWAY ) )
        wire->USER = addptype( wire->USER, RCX_NOWAY, NULL );
      return ;
    }
    /* Si la résistance est déjà marquée comme une interdiction. Je met ça par
       précaution car dans ce cas, on aurait déjà eu le RCN_FLAG_PASS sur le
       noeud... */
    if( getptype( wire->USER, RCX_NOWAY ) )
      return;
  
    /* On va jusqu'au bout d'un ensemble de resistances */
    while(   RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO  ) &&
           ! RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS )     ) {
           
      RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
      wire = ptnode->WIRELIST->DATA == wire ? ptnode->WIRELIST->NEXT->DATA :
                                              ptnode->WIRELIST->DATA;
      ptnode = getlonode( losig,
                          wire->NODE1 == ptnode->INDEX ? 
                             wire->NODE2 : wire->NODE1
                        );
    }

    /* On arrive à une intersection. Si on y est déjà passé, on marque la 
       résistance RCX_NOWAY, et on a fini pour cette branche */
    if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) ) {
      if( !getptype( wire->USER, RCX_NOWAY ) )
        wire->USER = addptype( wire->USER, RCX_NOWAY, NULL );
      return;
    }
  }
  else {
    /* Le noeud de départ est déjà traité */
    if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
      return;
  }

  RCN_SETFLAG( ptnode->FLAG, RCN_FLAG_PASS );
 
  
  /* On descend sur les autres branches */
  for( chain = ptnode->WIRELIST ; chain ; chain = chain->NEXT ) {
  
    wdown = (lowire_list*)chain->DATA;
    if( wdown == wire ) continue;
    
    ndown = ( wdown->NODE1 == ptnode->INDEX ? wdown->NODE2 : wdown->NODE1 );

    rcx_makenolooprec( losig, getlonode( losig, ndown ), wdown );
  }

}

void rcx_clearnoloop( losig_list *losig )
{
  lowire_list *wire;
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    if( getptype( wire->USER, RCX_NOWAY ) )
      wire->USER = delptype( wire->USER, RCX_NOWAY );
  }
  clearallwireflag( losig, RCN_FLAG_PASS );
}

void  rcx_loopelmore( losig_list *losig, 
                      locon_list *source, 
                      rcx_slope  *slopemiller,
                      RCXFLOAT   smax,
                      RCXFLOAT   smin,
                      RCXFLOAT vmax,      // La tension finale
                      RCXFLOAT vt,        // Le seuil d'un transistor
                      RCXFLOAT extcapa,   // La capacité externe
                      RCXFLOAT coefctc,
                      RCXFLOAT *dmax,
                      RCXFLOAT *dmin,
                      locon_list **cmax,
                      locon_list **cmin
                    )
{
  num_list *nodestart;

  for( nodestart = source->PNODE ; nodestart ; nodestart = nodestart->NEXT ) {
 
    rcx_loopelmcapa( losig, 
                     nodestart->DATA, 
                     slopemiller,
                     smax,
                     smin,
                     vmax,
                     vt,
                     extcapa,
                     coefctc,
                     NULL
                   );
    
    rcx_loopelmdelay(  losig,
                       nodestart->DATA, 
                       dmax,
                       dmin,
                       cmax,
                       cmin,
                       NULL,
                       0.0
                    );
    
    rcx_loopelmclean( losig );

  }

}

void rcx_loopelmclean( losig_list *losig )
{
  lowire_list   *wire;
  lonode_list   *node;
  ptype_list    *ptl;
  
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
    node = getlonode( losig, wire->NODE1 );
    ptl = getptype( node->USER, RCXLOOPCAPA );
    if( ptl ) {
      mbkfree( ptl->DATA );
      node->USER = delptype( node->USER, RCXLOOPCAPA );
    }
    node = getlonode( losig, wire->NODE2 );
    ptl = getptype( node->USER, RCXLOOPCAPA );
    if( ptl ) {
      mbkfree( ptl->DATA );
      node->USER = delptype( node->USER, RCXLOOPCAPA );
    }
  }
}

locon_list* rcx_loopgetlocon( lonode_list *ptnode )
{
  chain_list *head;
  
  head = getloconnode( ptnode );
  
  while( head && !rcx_isvalidlocon( (locon_list*)head->DATA ) ) 
    head = head->NEXT ;

  if( head )
    return ((locon_list*)head->DATA);

  return NULL;
}

void rcx_loopelmdelay( losig_list  *losig, 
                       long         start, 
                       RCXFLOAT    *dmax,
                       RCXFLOAT    *dmin,
                       locon_list  **cmax,
                       locon_list  **cmin,
                       lowire_list *wire,
                       RCXFLOAT     ed
                     )
{
  lonode_list   *ptnode;
  chain_list    *chain;
  lowire_list   *wdown;
  long           ndown;
  ptype_list    *ptype;
  RCXFLOAT      *ptdmax;

  ptnode = getlonode( losig, start );
  
  /* On va jusqu'au bout d'un ensemble de resistances */
  if( wire ) {

    while( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO )  && 
           !getptype( wire->USER, RCX_NOWAY )                ) {
           
      ed = ed + wire->RESI * *((RCXFLOAT*)(getptype( ptnode->USER,
                                                     RCXLOOPCAPA
                                                   )->DATA         ));

      if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) ) {
        if( dmax ) {
          if( *dmax < 0.0 || ed > *dmax ) {
            *dmax = ed;

            RCX_PTRTESTANDSET( cmax, rcx_loopgetlocon( ptnode ) )

            // SOLUTION TMP
            if (TRC_MARK_CON) {
                if ((ptype = getptype ((*cmax)->USER,TRC_LOCON_AWE)) != NULL) {
                    mbkfree(ptype->DATA);
                    (*cmax)->USER = delptype ((*cmax)->USER,TRC_LOCON_AWE);
                }
                ptdmax = mbkalloc( sizeof( RCXFLOAT ) );
                *ptdmax =  *dmax;
                (*cmax)->USER = addptype ((*cmax)->USER,TRC_LOCON_AWE,
                                          ptdmax);
            }
          }
        }
        if( dmin ) {
          if( *dmin < 0.0 || ed < *dmin ) {
            *dmin = ed;
            RCX_PTRTESTANDSET( cmin, rcx_loopgetlocon( ptnode ) )
          }
        }
      }
      
      wire = ptnode->WIRELIST->DATA == wire ? ptnode->WIRELIST->NEXT->DATA :
                                              ptnode->WIRELIST->DATA;
      ptnode = getlonode( losig,
                          wire->NODE1 == ptnode->INDEX ? 
                             wire->NODE2 : wire->NODE1
                        );
    }

    if( !getptype( wire->USER, RCX_NOWAY ) ) {
    
      ed = ed + wire->RESI * *((RCXFLOAT*)(getptype( ptnode->USER,
                                                  RCXLOOPCAPA
                                                )->DATA         ));

      if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) ) {
        if( dmax ) {
          if( *dmax < 0.0 || ed > *dmax ) {
            *dmax = ed;

            RCX_PTRTESTANDSET( cmax, rcx_loopgetlocon( ptnode ) )

            // SOLUTION TMP
            if (TRC_MARK_CON) {
                if ((ptype = getptype ((*cmax)->USER,TRC_LOCON_AWE)) != NULL) {
                    mbkfree(ptype->DATA);
                    (*cmax)->USER = delptype ((*cmax)->USER,TRC_LOCON_AWE);
                }
                ptdmax = mbkalloc( sizeof( RCXFLOAT ) );
                *ptdmax =  *dmax;
                (*cmax)->USER = addptype ((*cmax)->USER,TRC_LOCON_AWE,
                                          ptdmax);
            }
          }
        }
        if( dmin ) {
          if( *dmin < 0.0 || ed < *dmin ) {
            *dmin = ed;
            RCX_PTRTESTANDSET( cmin, rcx_loopgetlocon( ptnode ) )
          }
        }
      }
    }
  }

  /* Calcul de la capacité des branches en aval */
  if( !wire || !getptype( wire->USER, RCX_NOWAY ) ) {
    for( chain = ptnode->WIRELIST ; chain ; chain = chain->NEXT ) {
    
      wdown = (lowire_list*)chain->DATA;
      if( wdown == wire ) continue;
      
      ndown = ( wdown->NODE1 == ptnode->INDEX ? wdown->NODE2 : wdown->NODE1 );
      rcx_loopelmdelay( losig, ndown, dmax, dmin, cmax, cmin, wdown, ed );
    }
  }
}

RCXFLOAT rcx_loopelmcapa( losig_list  *losig, 
                          long         start,
                          rcx_slope   *slopemiller,
                          RCXFLOAT     smax,
                          RCXFLOAT     smin,
                          RCXFLOAT     vmax,      // La tension finale
                          RCXFLOAT     vt,        // Le seuil d'un transistor
                          RCXFLOAT     extcapa,   // La capacité externe
                          RCXFLOAT     coefctc,
                          lowire_list *wire
                        )
{
  RCXFLOAT          capa = 0.0;
  lonode_list   *ptnode;
  chain_list    *chain;
  lowire_list   *wdown;
  long           ndown;
  RCXFLOAT         *ptcapa;

  if( wire && getptype( wire->USER, RCX_NOWAY ) ) return 0.0;

  ptnode = getlonode( losig, start );
  
  /* On va jusqu'au bout d'un ensemble de resistances */
  if( wire ) {

    while( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO )  && 
           !getptype( wire->USER, RCX_NOWAY )                ) {
      wire = ptnode->WIRELIST->DATA == wire ? ptnode->WIRELIST->NEXT->DATA :
                                              ptnode->WIRELIST->DATA;
      ptnode = getlonode( losig,
                          wire->NODE1 == ptnode->INDEX ? 
                             wire->NODE2 : wire->NODE1
                        );
    }
  }

  /* Calcul de la capacité des branches en aval */
  if( !wire || !getptype( wire->USER, RCX_NOWAY ) ) {
    for( chain = ptnode->WIRELIST ; chain ; chain = chain->NEXT ) {
    
      wdown = (lowire_list*)chain->DATA;
      if( wdown == wire ) continue;
      
      ndown = ( wdown->NODE1 == ptnode->INDEX ? wdown->NODE2 : wdown->NODE1 );
      capa = capa + rcx_loopelmcapa( losig,
                                     ndown,
                                     slopemiller,
                                     smax,
                                     smin,
                                     vmax,
                                     vt,
                                     extcapa,
                                     coefctc,
                                     wdown
                                   );
    }
  }

  /* On remonte en mettant à jour les capacité. On marque le premier noeud
  terminal */

  while( ptnode->INDEX != start )
  {
    /* Calcul de la capacité sur le noeud courant */
    if( !wire || !getptype( wire->USER, RCX_NOWAY ) ) {

      capa = capa + rcx_getnodecapa( losig,
                                     ptnode,
                                     extcapa,
                                     slopemiller,
                                     RCX_MAX,
                                     TRC_HALF,
                                     coefctc
                                   );

      /* Stockage de la capacité sur le noeud courant */
      ptcapa = mbkalloc( sizeof( RCXFLOAT ) );
      *ptcapa = capa;
      ptnode->USER = addptype( ptnode->USER, RCXLOOPCAPA, ptcapa );
    }

    ptnode = getlonode( losig,
                        wire->NODE1 == ptnode->INDEX ? 
                          wire->NODE2 : wire->NODE1
                      );

    if( ptnode->INDEX != start ) {
      wire = ptnode->WIRELIST->DATA == wire ? ptnode->WIRELIST->NEXT->DATA :
                                              ptnode->WIRELIST->DATA;
    }

  }

  /* Capacité sur le noeud de départ */
  {
    capa = capa + rcx_getnodecapa( losig,
                                   ptnode,
                                   extcapa,
                                   slopemiller,
                                   RCX_MAX,
                                   TRC_HALF,
                                   coefctc
                                 );

    ptcapa = mbkalloc( sizeof( RCXFLOAT ) );
    *ptcapa = capa;
    ptnode->USER = addptype( ptnode->USER, RCXLOOPCAPA, ptcapa );
  }

  return capa;
}               
