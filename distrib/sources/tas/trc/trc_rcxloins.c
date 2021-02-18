/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_rcxloins.c                                              */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/*    Description : Fonctions relatives à la mémorisation des vues RCX à    */
/*    l'intérieur d'une loins. Ces fonctions sont principalement appellées  */
/*    par le parser et les fonctions de mise à plat des vues RCX.           */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.28 $
Author   : $Author: fabrice $
Date     : $Date: 2007/11/09 13:54:47 $

*/

#include "trc.h"

/* Fonctions internes */
void        rcx_loinsfreelosig    __P(( losig_list* ));
void        rcx_lofigchain        __P(( rcx_list*   ));

losig_list* rcx_loinsgetvssni( loins_list *loins )
{
  ptype_list *ptl;
  losig_list *losig;
  
  ptl = getptype( loins->USER, RCX_VSSNI );
  if( !ptl ) {
    losig            = rcx_loinsaddlosig();
    losig->NAMECHAIN = addchain( NULL, namealloc( "vss_ni" ) );
    losig->USER  =addptype( losig->USER, RCX_VSSNI, NULL );
    loins->USER = addptype( loins->USER, RCX_VSSNI, (void*)losig );
    addlorcnet( losig );
    losig->USER = addptype( losig->USER, LOFIGCHAIN, NULL );
  }
  else
    losig = (losig_list*)(ptl->DATA);
  return losig;
}

losig_list* rcx_loinsaddlosig( void )
{
  losig_list *ptsig;

  ptsig = (losig_list*)mbkalloc(sizeof(losig_list));
  ptsig->NAMECHAIN = NULL ;
  ptsig->INDEX     = 0l ;
  ptsig->TYPE      = INTERNAL ;
  ptsig->PRCN      = NULL ;
  ptsig->USER      = NULL ;
  ptsig->RCNCACHE  = 0;
  ptsig->FLAGS     = 0;

  return( ptsig );
}

void rcx_loinsfreelosig( losig_list *ptsig )
{
  ptype_list *ptl;
  
  if( ptsig->NAMECHAIN ) {
    freechain( ptsig->NAMECHAIN );
    ptsig->NAMECHAIN = NULL ;
  }

  ptl = getptype( ptsig->USER, LOFIGCHAIN );
  if( ptl ) {
    freechain( (chain_list*)ptl->DATA );
    ptsig->USER = delptype( ptsig->USER, LOFIGCHAIN );
  }

  ptl = getptype( ptsig->USER, RCX_VSSNI );
  if( ptl ) {
    ptsig->USER = delptype( ptsig->USER, RCX_VSSNI );
  }
  
  if( ptsig->PRCN ) {
    avt_log(LOGTRC,2, "PRCN field is not empty.\n" );
  }

  if( ptsig->USER ) {
    if ( avt_islog(0,LOGTRC) ) {
      avt_log(LOGTRC,2, "freercxlosig() : USER field is not empty.\n" );
      avt_log(LOGTRC,2, "first ptype is 0x%08X\n", (unsigned int)ptsig->USER->TYPE );
    }
  }

  mbkfree( ptsig );
}

locon_list* addrcxlocon()
{
  locon_list *ptlocon ;
 
  ptlocon = (locon_list*)mbkalloc(sizeof(locon_list)) ;
  ptlocon->NEXT      = NULL ;
  ptlocon->NAME      = NULL ;
  ptlocon->SIG       = NULL ;
  ptlocon->ROOT      = NULL ;
  ptlocon->DIRECTION = UNKNOWN ;
  ptlocon->TYPE      = INTERNAL ;
  ptlocon->PNODE     = NULL;
  ptlocon->USER      = NULL ;

  ptlocon->USER = addptype( ptlocon->USER, RCX_FAKE, 0 );

  return(ptlocon);
}

void freercxlocon(pt)
locon_list      *pt;
{
  ptype_list    *ptl;
  
  ptl = getptype( pt->USER, RCXNAME );
  if( ptl )
    pt->USER = delptype( pt->USER, RCXNAME );
  ptl = getptype( pt->USER, RCX_FAKE );
  if( ptl )
    pt->USER = delptype( pt->USER, RCX_FAKE );
  ptl = getptype( pt->USER, RCX_LOCONCAPA_SPACE );
  if( ptl )
    pt->USER = delptype( pt->USER, RCX_LOCONCAPA_SPACE );

  if( pt->USER != NULL ) {
    avt_log(LOGTRC,2, "freercxlocon() : field USER not empty! (first ptype is %08X)\n", 
             (unsigned int)pt->USER->TYPE 
           );
  }

  freenum( pt->PNODE );

  mbkfree(pt);
}

/* Equivalent de l'appel a la fonction lofigchain() mais sur le signal
 * du locon RCXEXTERNAL pour une vue RCN.
 */

void rcx_lofigchain( vuercx )
rcx_list        *vuercx;
{
  chain_list    *ptlofigchain;
  chain_list    *scanchain;
  locon_list *lc;

  lc=(locon_list *)vuercx->RCXEXTERNAL->DATA;
  if( getptype( lc->SIG->USER, LOFIGCHAIN ) )
  {
    if ( avt_islog (1,LOGTRC) ) {
      avt_log(LOGTRC,2, "rcx_lofigchain()\n" );
      avt_log(LOGTRC,2, "lofigchain is up to date. nothing to do !.\n" );
    }
    return;
  }

//  ptlofigchain = addchain( NULL, vuercx->RCXEXTERNAL );
  ptlofigchain = NULL;
  for( scanchain = vuercx->RCXEXTERNAL ;
       scanchain ;
       scanchain = scanchain->NEXT
     )
    ptlofigchain = addchain( ptlofigchain, scanchain->DATA );
  for( scanchain = vuercx->RCXINTERNAL ;
       scanchain ;
       scanchain = scanchain->NEXT
     )
    ptlofigchain = addchain( ptlofigchain, scanchain->DATA );
  lc->SIG->USER = addptype( lc->SIG->USER,
                                             LOFIGCHAIN,
                                             ptlofigchain
                                           );
}

void rcx_cleanloins( loins_list *ptins )
{
  locon_list *locon;
  locon_list *rcxlocon;
  ptype_list *pt;
  chain_list *scan ;
  rcx_list   *rcxint;
  losig_list *signal;
  
  for( locon = ptins->LOCON ; locon ; locon = locon->NEXT ) {

    if( (pt = getptype(locon->USER,PTYPERCX) ) )
    {
      signal = (losig_list*)(pt->DATA) ;
      rcxint = getrcx( signal );

      if( GETFLAG( rcxint->FLAG, MODELRCN ) ) {
        rcx_lofigchain( rcxint );
        freelorcnet( signal );
        rcxint->VIEW = NULL;
      }

    for( scan = rcxint->RCXEXTERNAL ; scan ; scan = scan->NEXT ) {
        rcxlocon = (locon_list*)(scan->DATA);
        rcx_freeloconcapa( NULL, ptins, rcxlocon );
        rcxlocon->SIG = NULL ;
        freercxlocon( rcxlocon );
      }
      freechain( rcxint->RCXEXTERNAL );
      rcxint->RCXEXTERNAL = NULL;

/*      rcxlocon = rcxint->RCXEXTERNAL ;
      rcxlocon->SIG = NULL ;
      rcx_freeloconcapa( NULL, ptins, rcxint->RCXEXTERNAL );
      freercxlocon( rcxint->RCXEXTERNAL );
      rcxint->RCXEXTERNAL = NULL;*/
      
      for( scan = rcxint->RCXINTERNAL ; scan ; scan = scan->NEXT ) {
        rcxlocon = (locon_list*)(scan->DATA);
        rcx_freeloconcapa( NULL, ptins, rcxlocon );
        rcxlocon->SIG = NULL ;
        freercxlocon( rcxlocon );
      }
      freechain( rcxint->RCXINTERNAL );
      rcxint->RCXINTERNAL = NULL;
      
      freercx( rcxint );
      rcxint      = NULL;
      signal->USER = delptype( signal->USER, PTYPERCX );

      
      if( getptype( signal->USER, RCX_COPIED ) )
        signal->USER = delptype( signal->USER, RCX_COPIED );
      if( getptype( signal->USER, RCX_LOCONINS ) )
        signal->USER = delptype( signal->USER, RCX_LOCONINS );
      
      
      rcx_loinsfreelosig( signal );
      locon->USER = delptype( locon->USER, PTYPERCX );
    }
  }

  pt = getptype( ptins->USER, RCX_UNRESOLVEDSIG );
  if( pt ) {
  
    for( scan = (chain_list*)(pt->DATA) ; scan ; scan = scan->NEXT ) {
    
      signal = (losig_list*)( scan->DATA );

      if( getptype( signal->USER, RCX_UNRESOLVEDSIG ) )
        signal->USER = delptype( signal->USER, RCX_UNRESOLVEDSIG );
        
      if( getptype( signal->USER, RCX_COPIED ) )
        signal->USER = delptype( signal->USER, RCX_COPIED );
     
      if( !getptype( signal->USER, LOFIGCHAIN ) )
        signal->USER = addptype( signal->USER, LOFIGCHAIN, NULL );
      freelorcnet( signal );

      rcx_loinsfreelosig( signal );
    }
    freechain( (chain_list*)(pt->DATA) );
    ptins->USER = delptype( ptins->USER, RCX_UNRESOLVEDSIG );
  }

  pt = getptype( ptins->USER, RCXPSEUDOVSS );
  if( pt ) {
    signal = (losig_list*)(pt->DATA);
    if( signal->PRCN ) {
      if (!getptype( signal->USER, LOFIGCHAIN ) ) 
        signal->USER = addptype( signal->USER, LOFIGCHAIN, NULL );
      freelorcnet( signal );
    }
    rcx_loinsfreelosig( signal );
    ptins->USER = delptype( ptins->USER, RCXPSEUDOVSS );
  }

  pt = getptype( ptins->USER, RCX_VSSNI );
  if( pt ) {
    signal = (losig_list*)(pt->DATA);
    freelorcnet( signal );
    rcx_loinsfreelosig( signal );
    ptins->USER = delptype( ptins->USER, RCX_VSSNI );
  }
  
  rcx_delhtrcxcon( ptins, NULL );
  rcx_delhtrcxsig( ptins, NULL );
  rcx_loconcapa_freeheap( NULL, ptins );
}

float rcx_getloinsresi( locon_list *locon )
{
  float resi=0.0;
  ptype_list *pt;
  losig_list *sigint;
  lowire_list *scanwire;
  
  if( locon->TYPE != 'I' )
    return 0.0;

  pt = getptype(locon->USER,PTYPERCX);
  if( pt ) {
    sigint = (losig_list*)(pt->DATA);
    for( scanwire = sigint->PRCN->PWIRE ; scanwire ; scanwire = scanwire->NEXT )
      resi = resi + scanwire->RESI;
  }

  return resi;
}

void rcx_getloinscapa( locon_list *locon, 
                       float *cm, 
                       float *ccl,
                       float *ccg
                     )
{
  ptype_list *pt;
  losig_list *sigint;
  chain_list *scan;
  loctc_list *loctc;
  loins_list *loins;
  losig_list *pseudovssins;
  losig_list *sigagr;
  int         node;
  rcx_list   *rcxint;
  locon_list *loconint;

  *ccl = 0.0;
  *ccg = 0.0;
  *cm  = 0.0;
  
  if( locon->TYPE != 'I' || !locon->ROOT ) return ;

  loins = (loins_list*)locon->ROOT;
  pseudovssins = getrcxpseudovss( loins->USER );
  
  
  pt = getptype( locon->USER, PTYPERCX );
  if( pt ) {
    sigint = (losig_list*)(pt->DATA);
    for( scan = sigint->PRCN->PCTC ; scan ; scan = scan->NEXT ) {
      loctc = (loctc_list*)scan->DATA;

      sigagr = rcn_ctcothersig( loctc, sigint );
    
      if( sigagr == pseudovssins ) 
        *cm = *cm + loctc->CAPA;
      else {
        node = rcn_ctcnode( loctc, sigint );
        if( node )
          *ccl = *ccl + loctc->CAPA;
        else
          *ccg = *ccg + loctc->CAPA;
      }
    }

    rcxint = getrcx( sigint );
    for( scan = rcxint->RCXINTERNAL ; scan ; scan = scan->NEXT ) {
      loconint = (locon_list*)scan->DATA;
      *cm = *cm + rcx_getloconcapa( loconint, 
                                    TRC_SLOPE_UNK, TRC_CAPA_NOM, TRC_HALF,
                                    NULL 
                                  );
    }
  }
}
