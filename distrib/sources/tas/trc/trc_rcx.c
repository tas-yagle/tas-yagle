/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_rcx.c                                                   */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.182 $
Author   : $Author: fabrice $
Date     : $Date: 2008/03/20 16:23:04 $

*/

#include "trc.h"
#include MCC_H

int RCX_CROSSTALK=RCX_NOCROSSTALK;
char RCX_FREEUNUSEDCTC='Y';
char RCX_USECACHE='Y';
char RCX_FASTMODE=1;

/* Valeur du produit RC sur un signal en dessous duquel on considère qu'il n'y 
a pas de délais RC à prendre en compte. */
RCXFLOAT RCX_MINRCSIGNAL=1.0;

/* Valeur relative entre 0 et 1 de la part de capacité de couplage sur un signal
en dessous de laquelle on considère que le signal n'est pas sujet au couplage */
RCXFLOAT RCX_MINRELCTKSIGNAL=0.05;

/* Filtrage des capacités de couplage : valeur relative entre 0 et 1 de la part
des capacités à négliger */
RCXFLOAT RCX_MINRELCTKFILTER=0.05;

/* Paramètres de la fonction rcx_rcnload() lorsque l'argument flag vaut
   RCX_BESTLOAD.

  RCX_CAPALOAD_MAXRC : Temps (en pico secondes) en dessous duquel le produit
  de la somme des résistances x somme des capacités doit être inférieur pour 
  qu'on prenne systématiquement une charge purement capacitive.

  RCX_MAXDELTALOAD : Ecart maximum autorisé entre la cellule en pi et la
  capacité pure lorsque le générateur de courant est constant (valeur sans
  influence).
*/
RCXFLOAT RCX_CAPALOAD_MAXRC = 5.0;
RCXFLOAT RCX_MAXDELTALOAD   = 1.0;

char RCX_CTK_MODEL = RCX_MILLER_NOMINAL ;
char RCX_CTK_NOISE = RCX_NOISE_FINE ;
char RCX_CTK_SLOPE_NOISE  = RCX_SLOPE_CTK ; 
char RCX_CTK_SLOPE_MILLER = RCX_SLOPE_NOMINAL ;
char RCX_CTK_SLOPE_DELAY  = RCX_SLOPE_DELAY_CTK ;

double RCX_SLOPE_UNSHRINK = 0.6 ;

/* Prend en compte les fronts améliorés pour l'effet miller */
#define RCX_CORREC_MILLER

/*******************************************************************************
*                                                                              *
* rcx_state()                                                                  *
* Renvoie un nombre caracterisant l'etat de la vue RCX. L'argument peut etre   *
* NULL.                                                                        *
*                                                                              *
*       -1   Pas de vue RCX (ptrcx = NULL)                                     *
*        0   Vue RCX vierge                                                    *
*        1   Vue RCX normale                                                   *
*        2   Vue RCX non extraite (RCXNODELAY seul)                            *
* L'argument where permet de savoir si il s'agit d'une vue interne ou externe. *
* 1=vue interne                                                                *
* 2=vue externe                                                                *
* autre=pas de verification                                                    *
*******************************************************************************/

// where :
#define RCX_STATE_NOCHECK     0
#define RCX_STATE_LOINS       1
#define RCX_STATE_LOFIG       2

// code retour :
#define RCX_STATE_NORCX      -1
#define RCX_STATE_EMPTY       0
#define RCX_STATE_TYPED       1

int rcx_state( ptrcx, where )
rcx_list        *ptrcx;
int              where;
{
  if( !ptrcx )
    return(RCX_STATE_NORCX); 

  if( !GETFLAG( ptrcx->FLAG, MODELRCN ) )
  {
    if( where == RCX_STATE_LOINS )      // Dans une instance, on a forcement une
      rcx_error( 21, NULL, AVT_ERROR ); // vue RCN.

    return(RCX_STATE_EMPTY);
  }

  if(  GETFLAG( ptrcx->FLAG, MODELRCN )  )
  {
    if( where == RCX_STATE_LOFIG && !ptrcx->VIEW )
      rcx_error( 22, NULL, AVT_ERROR );
    if( where == RCX_STATE_LOINS && ( !ptrcx->VIEW || !ptrcx->RCXEXTERNAL ) )
      rcx_error( 23, NULL, AVT_ERROR );
    return(RCX_STATE_TYPED);
  }

  rcx_error( 24, NULL, AVT_FATAL );

  /* never reached */
  return( 1 );
}

/*******************************************************************************
*                                                                              *
* merge_ext()                                                                  *
* Ajoute un connecteur dans la vue RCX d'un losig d'une lofig, en regroupant   *
* ses connecteurs sans faire de boucles.                                       *
*                                                                              *
*******************************************************************************/

void merge_ext( file, ptsig, ptcon, iscache, keeponenode )
rcxfile         *file;
losig_list      *ptsig;
locon_list      *ptcon;
char             iscache;
char             keeponenode;
{
  chain_list    *chemin;
  num_list      *sn;
  rcx_list      *ptrcx;
  char           buffer[1024];
  int            verif;

  verif = 1;

  if( chkloop( ptsig ) != 0 )
    verif = 0;

  if( ptcon->PNODE && keeponenode == YES )
  {
    /* On ne garde que le premier noeud. */
    for( sn = ptcon->PNODE->NEXT ; sn ; sn = sn->NEXT )
    {
      if( verif )
      {
        chemin = getway( ptsig, ptcon->PNODE->DATA, sn->DATA );
        if( !chemin ) {
          rcx_drive_wire( file, 
                          rcx_get_new_num_node( ptsig,
                                                ptcon->PNODE->DATA, 
                                                RCX_LOFIG,
                                                YES
                                              ),
                          rcx_get_new_num_node( ptsig,
                                                sn->DATA,
                                                RCX_LOFIG,
                                                YES
                                              ),
                          0.0, 
                          0.0 
                        );
          if( !iscache )
            addlowire( ptsig,
                       0,
                       0.0,
                       0.0,
                       ptcon->PNODE->DATA,
                       sn->DATA
                     );
        }
        freechain( chemin );
      }
    }
    while( ptcon->PNODE->NEXT )
      delloconnode( ptcon, ptcon->PNODE->NEXT->DATA );

  }

  ptrcx = getrcx( ptsig );
  ptrcx->RCXINTERNAL = addchain( ptrcx->RCXINTERNAL, ptcon );

  if( ptcon->TYPE == 'I' ) {
    sprintf( buffer,
             "%s%c%s",
             rcx_getinsname( (loins_list*)(ptcon->ROOT) ), SEPAR,
             getloconrcxname( ptcon )
           );
    addloconrcxname( ptcon, buffer );
  }
}

void set_ext_rcn( ptsig )
losig_list      *ptsig;
{
  rcx_list      *ptrcx;

  ptrcx = getrcx( ptsig );

  #ifdef RCX_CHECK
  if( rcx_state( ptrcx, RCX_STATE_LOFIG  ) != RCX_STATE_EMPTY || !ptsig->PRCN )
    rcx_error( 25, ptsig, AVT_WARNING );
  #endif

  ptrcx->VIEW = ptsig->PRCN;
  SETFLAG( ptrcx->FLAG, MODELRCN );
}

void rcx_addbellow( losig_list *ptsig, char *sigpointed )
{
   ptsig->USER = addptype( ptsig->USER, RCX_BELLOW, sigpointed );
}

losig_list* rcx_addsigbellow( lofig_list *ptfig, char *sigpointed )
{
  int         maxidx = 0;
  losig_list *losig;
  char        buffer[1024];

  for( losig = ptfig->LOSIG ; losig ; losig = losig->NEXT )
    if( losig->INDEX > maxidx )
      maxidx = losig->INDEX ;
      
  maxidx++;
  
  sprintf( buffer, "rcx_bellow_%d", maxidx );
  
  losig = addlosig( ptfig, 
                    ++maxidx, 
                    addchain( NULL, namealloc( buffer ) ), 
                    INTERNAL 
                  );

  losig->USER = addptype( losig->USER, RCX_BELLOW, sigpointed );
  losig->USER = addptype( losig->USER, LOFIGCHAIN, NULL );

  return( losig );
}

/******************************************************************************\
Transfert la vue rcx d'une loins dans la vue externe.
\******************************************************************************/

void rcx_copyrcx( lofig_list *lofig, 
                  losig_list *sigext, 
                  loins_list *loins,
                  losig_list *sigint )
{

  rcx_list      *rcxext;
  rcx_list      *rcxint;
  locon_list    *loconint;
  num_list      *headnum;
  num_list      *scannum;
  ptype_list    *pt;
  chain_list    *scanchain;
  
  rcxext = getrcx( sigext );
  rcxint = getrcx( sigint );

  /* On deplace les locon de la vue interne vers la vue externe :
  * il suffit juste de les enlever du lofigchain du signal
  * interne */
  
  
  for( scanchain = rcxint->RCXINTERNAL ; 
       scanchain ; 
       scanchain=scanchain->NEXT
     )
  {
    loconint  = (locon_list*)(scanchain->DATA);

    headnum = NULL ;
    
    // On commence par calculer les noeuds du locon avant de toucher
    // à autre chose.
    rcx_trsfnodelocon( loconint, RCX_LOINS );

    while(loconint->PNODE)
    {
      headnum = addnum( headnum, loconint->PNODE->DATA );
      delloconnode( loconint, loconint->PNODE->DATA );
    }
            
    loconint->SIG = sigext ;
    loconint->ROOT  = NULL;
    rcxext->RCXINTERNAL = addchain( rcxext->RCXINTERNAL, loconint  );

    pt = getptype( sigext->USER, LOFIGCHAIN );
    pt->DATA = addchain( ((chain_list*)(pt->DATA)),
                                          loconint
                                        );

    pt = getptype( loconint->USER, RCXNAME );
    if( pt )
      loconint->USER = delptype( loconint->USER, RCXNAME );

    for( scannum = headnum ; scannum ; scannum = scannum->NEXT )
    {
      if( !scannum->DATA )
        rcx_error( 26, sigint, AVT_ERROR );
      else
        setloconnode( loconint, scannum->DATA );
    }

    freenum( headnum );

    // On transfert la capacité associée au locon ( a cause des allocations
    // par tat ).

    rcx_transfert_capa_from_instance_to_figure( loins, lofig, loconint );
  }

  // On ne touche plus à ces connecteurs.
  freechain( rcxint->RCXINTERNAL );
  rcxint->RCXINTERNAL = NULL;

}

/******************************************************************************\
rcx_dump_loins() :
- Drive les RC
- Crée les BELLOW dans la lofig.

Drive les éléments RC d'un signal d'une instance dans le fichier. Crée des 
signaux BELLOW dans la lofig pour les agresseurs internes à la loins. Ils sont
accessibles par le ptype RCX_COPIED mis dans les losigs typés UNRESOLVED de
l'instance.
\******************************************************************************/
void rcx_dump_loins( rcxfile *file, 
                     lofig_list *lofig, 
                     loins_list *loins, 
                     losig_list *losig
                   )
{
  lowire_list   *wire;
  losig_list    *pseudovssins;
  losig_list    *pseudovssinsni;
  chain_list    *scanchain;
  loctc_list    *loctc;
  int            node;
  losig_list    *sigagr;
  int            nodeagr;
  ptype_list    *pt;
  losig_list    *sigbellow;
  rcx_list      *rcxint;
  losig_list    *sigext;
  rcx_list      *rcxext;
  losig_list    *sigagrext;

  rcxint = getrcx( losig );
  sigext = rcx_get_out_ins_sig( losig );
  rcxext = getrcx( sigext );
 
  if( !GETFLAG( rcxext->FLAG, RCXNOWIRE ) ) {
    for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT )
      rcx_drive_wire( file, 
                     rcx_get_new_num_node( losig, wire->NODE1, RCX_LOINS, YES ),
                     rcx_get_new_num_node( losig, wire->NODE2, RCX_LOINS, YES ),
                      wire->RESI, 
                      wire->CAPA 
                    );
  }

  pseudovssins = getrcxpseudovss( loins->USER );
  pseudovssinsni = rcx_loinsgetvssni( loins );
  
  for( scanchain = losig->PRCN->PCTC ;
       scanchain ; 
       scanchain = scanchain->NEXT ) 
  {
  
    loctc   = (loctc_list*)scanchain->DATA;
    node    = rcn_ctcnode( loctc, losig );
    sigagr  = rcn_ctcothersig( loctc, losig );
    nodeagr = rcn_ctcothernode( loctc, losig );

    if( sigagr == pseudovssins ) 
      rcx_drive_ground_capa( file,
                            rcx_get_new_num_node( losig, node, RCX_LOINS, YES ),
                             loctc->CAPA 
                           );
      
    if( sigagr == pseudovssinsni )
      rcx_drive_ctcni_capa( file, 
                            rcx_get_new_num_node( losig, node, RCX_LOINS, YES ),
                            loctc->CAPA 
                          );
      
    if( sigagr != pseudovssins && sigagr != pseudovssinsni ) {
    
      if( getptype( sigagr->USER, RCX_UNRESOLVEDSIG ) ) {
        // L'agresseur n'a pas été lu, il correspond à un signal interne. 
        pt = getptype( sigagr->USER, RCX_COPIED );
        
        if( !pt ) {
          sigbellow = rcx_addsigbellow( lofig,
                                        concatname( rcx_getinsname( loins ),
                                                    rcx_getsigname( sigagr )
                                                  ) 
                                      );
          sigagr->USER = addptype( sigagr->USER, RCX_COPIED, sigbellow );
          addlorcnet( sigbellow );
        }
        else 
          sigbellow = (losig_list*)(pt->DATA);

        rcx_drive_ctc_capa( file, 
                            rcx_get_new_num_node( losig, node, RCX_LOINS, YES ),
                            loctc->CAPA, 
                            rcx_getsigname( sigbellow ), 
                            0 
                          );
      }
      else {
        // On teste le cas où il n'y a pas de vue RCX sur le signal externe.
        /*
        locon_agr = (locon_list*)getptype( sigagr->USER, RCX_LOCONINS )->DATA;
        */
        sigagrext = rcx_get_out_ins_sig( sigagr );
        if( getrcx( sigagrext ) ) {
          if( sigagrext != sigext ) {
            rcx_drive_ctc_capa( file,
                                rcx_get_new_num_node( losig, 
                                                      node, 
                                                      RCX_LOINS, 
                                                      YES 
                                                    ), 
                                loctc->CAPA, 
                                rcx_getsigname( sigagrext ),
                                rcx_get_new_num_node( sigagr, 
                                                      nodeagr, 
                                                      RCX_LOINS,
                                                      YES
                                                    )
                              );
          }
        }
        else {
          if( tlc_islosigalim( sigagrext ) )
            rcx_drive_ctcni_capa( file, 
                            rcx_get_new_num_node( losig, node, RCX_LOINS, YES ),
                                  loctc->CAPA 
                                );
          else
            rcx_drive_ground_capa( file,
                            rcx_get_new_num_node( losig, node, RCX_LOINS, YES ),
                                   loctc->CAPA 
                                 );

        }
      }
    }
  }
}


/******************************************************************************\
Crée les réseaux RC à plat en mémoire lorsqu'il n'y a pas de cache
\******************************************************************************/
void rcn_flatnet( lofig_list *lofig )
{
  losig_list     *vss=NULL;
  losig_list     *vssni=NULL;
  ptype_list     *ptl;
  losig_list     *losig;
  chain_list     *chain;
  locon_list     *locon;
  losig_list     *sigint;
  loins_list     *loins;
  lowire_list    *wire;
  loctc_list     *ptctc;
  chain_list     *ctc;
  rcx_list       *rcx;
  int             n1, n2;

  vss   = rcx_getvss( lofig );
  vssni = rcx_getvssni( lofig );

  /* Il faut renuméroter tous les RC de la lofig avant d'y ajouter les éléments
     RC des instances pour ne pas avoir de problèmes avec les CTC/
  */
  
  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
  
    if( !losig->PRCN ) continue;

    rcx = getrcx( losig );
    if( !rcx ) continue;

    freetable( losig );
    for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT )
      rcn_changewirenodes( losig,
                           wire, 
                      rcx_get_new_num_node( losig, wire->NODE1, RCX_LOFIG, NO ),
                      rcx_get_new_num_node( losig, wire->NODE2, RCX_LOFIG, NO )
                         );
    
    for( ctc  = losig->PRCN->PCTC ; ctc ; ctc = ctc->NEXT ) {
    
      ptctc = (loctc_list*)ctc->DATA;
      
      if( ptctc->SIG1 == losig ) 
        n1 = rcx_get_new_num_node( losig, ptctc->NODE1, RCX_LOFIG, NO );
      else
        n1 = ptctc->NODE1;
      
      if( ptctc->SIG2 == losig ) 
        n2 = rcx_get_new_num_node( losig, ptctc->NODE2, RCX_LOFIG, NO );
      else
        n2 = ptctc->NODE2;

      rcn_changectcnodes( ptctc, n1, n2 );
    }
  }

  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
  
    if( !losig->PRCN ) continue;
    
    rcx = getrcx( losig );
    if( !rcx ) continue;
 
    ptl = getptype( losig->USER, LOFIGCHAIN );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA;
        if( !rcx_isvalidlocon( locon ) )
          continue;
        if( locon->TYPE == 'I' ) {
          ptl = getptype( locon->USER, PTYPERCX );
          if( ptl ) {
            sigint = (losig_list*)ptl->DATA;
            loins  = (loins_list*)locon->ROOT;
            rcx_mergercn( lofig, losig, loins, sigint, vss, vssni );
          }
        }
      }
    } 
  }

  // Nettoyage des ctc

  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
    if( !losig->PRCN )
      continue;
    for( ctc = losig->PRCN->PCTC ; ctc ; ctc = ctc->NEXT ) {
      ptctc = (loctc_list*)ctc->DATA;
      if( getptype( ptctc->USER, RCX_COPIED ) )
        ptctc->USER = delptype( ptctc->USER, RCX_COPIED );
    }
    ptl = getptype( losig->USER, LOFIGCHAIN );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA;
        if( !rcx_isvalidlocon( locon ) )
          continue;
        if( locon->TYPE == 'I' ) {
          ptl = getptype( locon->USER, PTYPERCX );
          if( ptl ) {
            sigint = (losig_list*)ptl->DATA;
            for( ctc = sigint->PRCN->PCTC ; ctc ; ctc = ctc->NEXT ) {
              ptctc = (loctc_list*)ctc->DATA;
              if( getptype( ptctc->USER, RCX_COPIED ) )
                ptctc->USER = delptype( ptctc->USER, RCX_COPIED );
            }
          }
        }
      }
    } 
  }
}

/******************************************************************************\
Drive les éléments RC d'un signal d'une lofig.
\******************************************************************************/
void rcx_dumpnet( rcxfile *file, losig_list *losig )
{
  lowire_list   *wire;
  chain_list    *scanchain;
  loctc_list    *loctc;
  int            node;
  losig_list    *sigagr;
  int            nodeagr;
  rcx_list      *rcxagr;
  rcx_list      *rcxint;

  rcxint = getrcx( losig );
  if( !GETFLAG( rcxint->FLAG, RCXNOWIRE ) ) {
    for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
      rcx_drive_wire( file, 
                     rcx_get_new_num_node( losig, wire->NODE1, RCX_LOFIG, YES ),
                     rcx_get_new_num_node( losig, wire->NODE2, RCX_LOFIG, YES ),
                      wire->RESI, 
                      wire->CAPA 
                    );
    }
  }

  for( scanchain = losig->PRCN->PCTC ;
       scanchain ; 
       scanchain = scanchain->NEXT ) 
  {
  
    loctc   = (loctc_list*)scanchain->DATA;
    node    = rcn_ctcnode( loctc, losig );
    sigagr  = rcn_ctcothersig( loctc, losig );
    nodeagr = rcn_ctcothernode( loctc, losig );
    
    rcxagr = getrcx( sigagr );

    if( !rcxagr && !rcx_isbellow( sigagr ) ) {
      if( rcx_isnotinfluentagressor( losig, sigagr ) ) 
        rcx_drive_ctcni_capa( file, 
                            rcx_get_new_num_node( losig, node, RCX_LOFIG, YES ),
                              loctc->CAPA 
                            );
      else
        rcx_drive_ground_capa( file, 
                            rcx_get_new_num_node( losig, node, RCX_LOFIG, YES ),
                               loctc->CAPA 
                             );
    }
    else
      rcx_drive_ctc_capa( file, 
                          rcx_get_new_num_node( losig, node, RCX_LOFIG, YES ),
                          loctc->CAPA, 
                          rcx_getsigname( sigagr ), 
                         rcx_get_new_num_node( sigagr, nodeagr, RCX_LOFIG, YES )
                        );
  } 
}

/******************************************************************************\
rcx_mergercn()
Réalise le merge d'un losig d'une instance sur un signal externe. Utilise les
BELLOW créés par rcx_dump_loins.
Les vrais capacités de couplage qui ont été copiées ont un ptype RCX_COPIED, il
ne faudra plus les copier.
\******************************************************************************/
void rcx_mergercn( lofig_list *lofig, 
                   losig_list *sigext, 
                   loins_list *scanloins, 
                   losig_list *sigint,
                   losig_list *vss,
                   losig_list *vssni
                 )
{
  losig_list    *pseudovssins;
  losig_list    *pseudovssinsni;
  lowire_list   *wire;
  chain_list    *scanchain;
  loctc_list    *loctc;
  int            node;
  int            nodeagr;
  losig_list    *sigagr;
  losig_list    *sigagrext;
  ptype_list    *pt;
  losig_list    *sigbellow;
  rcx_list      *rcxext;

  pseudovssins = getrcxpseudovss( scanloins->USER );
  pseudovssinsni = rcx_loinsgetvssni( scanloins );
  
  if( !sigint->PRCN ) return;
 
  rcxext = getrcx( sigext );
  for( wire = sigint->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    addlowire( sigext,
               wire->FLAG,
               wire->RESI,
               wire->CAPA,
               rcx_get_new_num_node( sigint, wire->NODE1, RCX_LOINS, NO ), 
               rcx_get_new_num_node( sigint, wire->NODE2, RCX_LOINS, NO )
             );

  for( scanchain = sigint->PRCN->PCTC ;
       scanchain ; 
       scanchain = scanchain->NEXT ) 
  {
  
    loctc   = (loctc_list*)scanchain->DATA;
    node    = rcn_ctcnode( loctc, sigint );
    sigagr  = rcn_ctcothersig( loctc, sigint );
    nodeagr = rcn_ctcothernode( loctc, sigint );

    if( sigagr == pseudovssins ) 
      addloctc( sigext, rcx_get_new_num_node( sigint, node, RCX_LOINS, NO ),
                vss, 0,
                loctc->CAPA
              );
      
    if( sigagr == pseudovssinsni )
      addloctc( sigext, rcx_get_new_num_node( sigint, node, RCX_LOINS, NO ),
                vssni, 0,
                loctc->CAPA
              );
      
    if( sigagr != pseudovssins   && 
        sigagr != pseudovssinsni &&
        !getptype( loctc->USER, RCX_COPIED ) ) 
    {
    
      if( getptype( sigagr->USER, RCX_UNRESOLVEDSIG ) ) {
        // L'agresseur n'a pas été lu, il correspond à un signal interne. 
        pt = getptype( sigagr->USER, RCX_COPIED );
        
        if( !pt ) {
          // ce cas n'arrive jamais. Il a été traité dans rcx_dump_loins()
          rcx_error(27,sigagr, AVT_ERROR );
        }
        else  {
          sigbellow = (losig_list*)(pt->DATA);

          addloctc( sigext, rcx_get_new_num_node( sigint, node, RCX_LOINS, NO ),
                    sigbellow, 0,
                    loctc->CAPA
                  );
        }
      }
      else {
        pt = getptype( loctc->USER, RCX_COPIED );
        if( !pt ) {
          loctc->USER = addptype( loctc->USER, RCX_COPIED, NULL );
          /*
          sigagrext = ((locon_list*)
                       (getptype( sigagr->USER, RCX_LOCONINS )->DATA ))->SIG;
          */
          sigagrext = rcx_get_out_ins_sig( sigagr );
          if( getrcx( sigagrext ) ) {
            if( sigext != sigagrext ) {
              addloctc( 
                    sigext, 
                    rcx_get_new_num_node( sigint, node, RCX_LOINS, NO ),
                    sigagrext, 
                    rcx_get_new_num_node( sigagr, nodeagr, RCX_LOINS, NO ),
                    loctc->CAPA
                      );
            }
          }
          else {
            // Cas où l'agresseur est un signal ordinaire qui est connecté en 
            // externe à vdd ou vss.
            if( tlc_islosigalim( sigagrext ) ) 
              addloctc( sigext, 
                        rcx_get_new_num_node( sigint, node, RCX_LOINS, NO ),
                        vss, 
                        0,
                        loctc->CAPA
                      );
            else
              addloctc( sigext, 
                        rcx_get_new_num_node( sigint, node, RCX_LOINS, NO ),
                        vssni, 
                        0,
                        loctc->CAPA
                      );

          }
        }
      }
    }
  }

  lofig=NULL; //unused parameter
}

/******************************************************************************\
rcx_trsfloinscapa()
Dans le cas où les résistance ne sont pas prise en compte, transfère la 
capacité des connecteurs internes vers le connecteur externe.
sigint est un losig dans une instance.
\******************************************************************************/
void rcx_trsfloinscapa( lofig_list *lofig, 
                        loins_list *loins, 
                        losig_list *sigint )
{
  rcx_list *rcxint;
  locon_list *loconext;
  locon_list *loconint;
  chain_list *chain;
  
  loconext = (locon_list*)( getptype( sigint->USER, RCX_LOCONINS )->DATA );
  rcxint   = getrcx( sigint ); 
  
  for( chain = rcxint->RCXINTERNAL ; chain ; chain = chain->NEXT ) {
    loconint = (locon_list*)(chain->DATA);
    rcx_add_capa_from_instance_to_figure( loins, lofig, loconint, loconext );
  }

  if( rcxint->RCXEXTERNAL ) {
    chain_list *cl;
    for (cl=rcxint->RCXEXTERNAL; cl!=NULL; cl=cl->NEXT)
      rcx_add_capa_from_instance_to_figure( loins, 
                                            lofig, 
                                            (locon_list *)cl->DATA, 
                                            loconext 
                                          );
  }
}

/******************************************************************************\
rcx_buildrcxnowire();
Conserve le nombre minimum de locon sur une vue RCX si celle-ci est type nowire.
Stratégie : 
Si le signal est externe, on ne conserve que le connecteur externe.
Si le signal est interne, on ne conserve que le connecteur qui a le meme nom que
le signal.
Si aucun connecteur ne correspond, on prend un connecteur et on lui colle le
nom du signal.
\******************************************************************************/
void rcx_buildrcxnowire( lofig_list *lofig, losig_list *losig )
{
  rcx_list      *rcx;
  chain_list    *chain;
  RCXFLOAT       c;
  char          *name;
  locon_list    *locon;
  locon_list    *goodlocon;

  rcx = getrcx( losig );

  if( !rcx->RCXEXTERNAL && !rcx->RCXINTERNAL )
    return;

  // On cherche le locon qui va rester.
  
  goodlocon = NULL;
  if( rcx->RCXEXTERNAL ) {
    goodlocon = rcx->RCXEXTERNAL->DATA;
    name = getloconrcxname( goodlocon );
  }
  if( !goodlocon ) {
    name = rcx_getsigname( losig );
    for( chain = rcx->RCXINTERNAL ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)(chain->DATA);
      if( getloconrcxname( locon ) == name ) {
        goodlocon = locon;
        break;
      }
    }
  }
  if( !goodlocon ) {
    for( chain = rcx->RCXINTERNAL ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)(chain->DATA);
      if( locon->ROOT ) {
        goodlocon = locon;
        break;
      }
    }
  }
  if( !goodlocon )
    goodlocon = (locon_list*)(rcx->RCXINTERNAL->DATA);
      
  // On efface les autres.
  
  c=0.0;
  for( chain = rcx->RCXINTERNAL ; chain ; chain = chain->NEXT ) {
    locon = (locon_list*)(chain->DATA);
    if( locon == goodlocon )
      continue;
    rcx_add_capa_locon( lofig, goodlocon, locon );
    rcx_freeloconcapa( lofig, NULL, locon );
    if( !locon->ROOT ) { 
      // Libère les locons alloués par RCX
      freenum( locon->PNODE ); locon->PNODE = NULL;
      locon->SIG = NULL;
      freercxlocon( locon );
    }
  }
  freechain( rcx->RCXINTERNAL ); rcx->RCXINTERNAL = NULL;
  if( !rcx->RCXEXTERNAL ) 
    rcx->RCXINTERNAL = addchain( NULL, goodlocon );
  
  if( getloconrcxname( goodlocon ) != name )
    addloconrcxname( goodlocon, name );

}

/******************************************************************************\
buildrcx()
Met les vues RCX à plat. Fonctionne avec ou sans cache.
Dans la boucle des losigs, les réseaux RC ont leurs index de noeud d'origine. 
Par contre, les connecteurs sont renumérotés au fur et à mesure qu'on les 
rencontre.
\******************************************************************************/

void buildrcx( currentfig )
lofig_list	*currentfig;
{
  losig_list     *sigext;
  losig_list     *sigint;
  rcx_list       *rcxext;
  rcx_list       *rcxint;
  loins_list     *scanloins;
  locon_list     *scanlocon;
  ptype_list	 *pt;
  char           *name;
  chain_list     *chain;
  chain_list     *chainlosig;
  rcxfile        *file;
  char            fileacces;
  MBK_OFFSET_MAX  offset;
  chain_list     *headlosig;
  char            iscache;
  char            buffer[1024];
  lowire_list *lw, *nlw;
  int origcache;

  if( (origcache=(rcn_getlofigcache( currentfig ))!=NULL) || V_BOOL_TAB[__TAS_ALWAYS_USE_CACHE].VALUE)
    iscache = 1;
  else
    iscache = 0;
  
  file = rcx_init_driver( currentfig, iscache );

  if( iscache )
    fileacces = mbk_getfileacces( file->FD );

  headlosig = rcx_driver_sort_losig( currentfig );
  rcx_build_new_num_node( currentfig );

  for( chainlosig = headlosig ; chainlosig ; chainlosig = chainlosig->NEXT )
  {
    if( chainlosig->DATA == NULL ) {
      rcx_end_external( file );
      continue;
    }
    
    sigext = (losig_list*)chainlosig->DATA;
    rcxext = getrcx( sigext );
    if( !rcxext )
      continue;

    if( rcx_has_rc_timings( sigext ) )
      SETFLAG( rcxext->FLAG, RCXIGNORE );

    if( rcx_state( rcxext, RCX_STATE_LOFIG ) == RCX_STATE_EMPTY )
      set_ext_rcn( sigext );

    rcn_lock_signal( currentfig, sigext );

    if( !rcx_iscrosstalk( currentfig, sigext, RCX_TEST_ONE ))
      SETFLAG( rcxext->FLAG, RCXNOCTC );

    if( setrcxmodel( currentfig, sigext, RCX_BEST ) == RCX_NORCTC &&
        V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL0 ) 
    {
      rcn_unlock_signal( currentfig, sigext );
      continue;
    }

    rcx_repair_pnode( sigext );

    rcx_drive_signal_header( file, sigext, rcxext );
    if( iscache ) {
      if( !mbkftell( file->FD, &offset ))
        avt_errmsg( TRC_ERRMSG, "034", AVT_FATAL, file->FILENAME );

      mbk_losig_setfilepos( currentfig,
                            sigext,
                            file->FD,
                            &offset,
                            RCX_FLAGTAB,
                            RCX_FLAGHEAP
                          );
    }
    rcx_drive_begin_net( file );

    rcx_dumpnet( file, sigext );
    
    for( chain = (chain_list*)getptype( sigext->USER, LOFIGCHAIN )->DATA ;
         chain ;
         chain = chain->NEXT
       )
    {
      scanlocon = (locon_list*)chain->DATA;
      if( !rcx_isvalidlocon( scanlocon ) )
        continue;
     
      {
        
        if( scanlocon->TYPE == 'I' ) {
          scanloins = (loins_list*)scanlocon->ROOT;
          if( (pt = getptype(scanlocon->USER,PTYPERCX) ) ) {
            sigint = (losig_list*)(pt->DATA); 
            rcxint = getrcx( sigint );
            if (countchain(rcxint->RCXEXTERNAL)>1)
              avt_errmsg( TRC_ERRMSG, "053", AVT_FATAL, scanloins->FIGNAME);
          }
          else {
            sigint = NULL;
            rcxint = NULL;
          }

          if( rcx_state( rcxint, RCX_STATE_LOINS ) == RCX_STATE_NORCX ) {
            merge_ext( file, sigext, scanlocon, iscache, YES );
            rcx_trsfnodelocon( scanlocon, RCX_LOFIG );
          }
          else {
            rcx_dump_loins( file, currentfig, scanloins, sigint );
            rcx_trsfdriver( currentfig, sigext, rcxext, sigint, rcxint );
            name = concatname( rcx_getinsname( scanloins ), 
                               rcx_getsigname( sigint ) 
                             );
            rcx_addorigin( currentfig, sigext, name );
            if( !GETFLAG( rcxext->FLAG, RCXNOWIRE ) && 
                !GETFLAG( rcxint->FLAG, RCXNOWIRE )    )
              rcx_copyrcx( currentfig, sigext, scanloins, sigint );
            else {
              rcx_trsfloinscapa( currentfig, scanloins, sigint );
              merge_ext( file, sigext, scanlocon, iscache, NO );
              rcx_trsfnodelocon( scanlocon, RCX_LOFIG );
            }
            rcn_addcapa( sigext, rcn_getnetcapa( sigint ) );
          }
        }

        if( scanlocon->TYPE == 'T' ) {
          merge_ext( file, sigext, scanlocon, iscache, YES );
          rcx_trsfnodelocon( scanlocon, RCX_LOFIG );
        }

        if( scanlocon->TYPE == 'E' ) {
          rcx_trsfnodelocon( scanlocon, RCX_LOFIG );
        }
      }
    }

    if( GETFLAG( rcxext->FLAG, RCXNOWIRE ) ) {
      rcx_buildrcxnowire( currentfig, sigext );
    }
    
    rcx_drive_end_net( file );
    rcxprintlocon( file, rcxext );
    rcxprintorigin( file, sigext );
    rcx_drive_signal_end( file, sigext );

    rcn_unlock_signal( currentfig, sigext );
    if( rcn_islock_signal( currentfig, sigext ) == YES )
      avt_errmsg( TRC_ERRMSG, "041", AVT_WARNING, rcx_getsigname( sigext ) );
    rcn_flush_signal( currentfig, sigext );
  }
  
  freechain( headlosig );

  if( !iscache )
    rcn_flatnet( currentfig );

  rcx_clear_new_num_node( currentfig );

  rcx_drive_end( file, currentfig );

  // Vire le contenu des instances
  for( scanloins = currentfig->LOINS ; scanloins ; scanloins = scanloins->NEXT )
    rcx_cleanloins( scanloins );

  // Création des tables de hash
  for( sigext = currentfig->LOSIG ; sigext ; sigext = sigext->NEXT )
  {
    chain_list *cl;
    if( tlc_islosigalim( sigext ) )
      continue;

    /* Modif Grégoire le 19 Mai 2003. Grosse bidouille immonde.
       TAS récupère les vecteurs sous la forme toto[23] car c'est comme ca que
       Yagle crée les noms. Pb : MBK/RCX n'est pas informé de ce nommage. On le
       fait donc maintenant ici.
    */
    strcpy( buffer, rcx_getsigname( sigext ) );
    rcx_vect( buffer );
    rcx_addhtrcxsigbyname( NULL, currentfig, sigext, namealloc( buffer ) );

    rcxext = getrcx( sigext );
    if( !rcxext ) continue;

    for (chain=rcxext->RCXEXTERNAL; chain!=NULL; chain=chain->NEXT)
      rcx_addhtrcxcon( NULL, currentfig, (locon_list *)chain->DATA );
    for( chain = rcxext->RCXINTERNAL ; chain ; chain = chain->NEXT )
      rcx_addhtrcxcon( NULL, currentfig, (locon_list*)chain->DATA );
  }

  for( scanloins = currentfig->LOINS ; scanloins ; scanloins = scanloins->NEXT )
    rcx_addhtrcxmod( currentfig, 
                     rcx_getinsname( scanloins ), 
                     scanloins->FIGNAME 
                   );

  if( iscache ) {
    // Elimination de l'ancien cache
    rcn_disable_cache( currentfig );
    if (!origcache)
    {
      for( sigext = currentfig->LOSIG ; sigext ; sigext = sigext->NEXT )
       {
          if (sigext->PRCN)
          {
            for (lw=sigext->PRCN->PWIRE; lw!=NULL; lw=nlw)
            {
               nlw=lw->NEXT;
               freelowire( lw );
            }
            sigext->PRCN->PWIRE=NULL;
            while( sigext->PRCN->PCTC )
              delloctc( (loctc_list*)sigext->PRCN->PCTC->DATA );
            sigext->PRCN->PCTC=NULL;
          }
       }
    }
    // Met en route le nouveau cache
    rcx_reload( currentfig, fileacces );
  }

}

void rcx_exit( code )
int code;
{
  fflush( stdout );
  fprintf( stderr, 
           "\n\n*** RCX : abnormal terminaison. Exit code %d ***\n\n", 
           code 
         );
  avt_log(LOGTRC,2, "Exit code %d\n", code );
  EXIT( 1 );
}

/******************************************************************************\
Mémorise et retrouve les agresseurs d'un signal.
insname est le nom d'instance de la lofig.
rcx_getagrlistfast() renvoie EMPTYHY si on a jamais traité ce signal dans 
l'instance insname.
\******************************************************************************/
chain_list* rcx_getagrlistfast( lofig_list *lofig,
                                char       *insname, 
                                losig_list *victime
                              )
{
  ptype_list    *ptl;
  ht            *htins;
  ht            *htsig;
  chain_list    *headagr;
  chain_list    *scan;
  rcxparam      *param;
  chain_list    *chain;
  losig_list    *losig;
 
  // Récupère la table des instances de cette lofig.
  // Il s'agit des différents noms sous laquelle est instanciée cette lofig. Il
  // ne s'agit pas d'un table pour accéder rapidement à un LOINS de cette lofig.
  ptl = getptype( lofig->USER, RCX_HTAGR );
  if( !ptl ) return (chain_list*)EMPTYHT;
  htins=(ht*)ptl->DATA;

  // Récupère la table des signaux.
  htsig = (ht*)gethtitem( htins, insname );
  if( htsig == (ht*)EMPTYHT || htsig == (ht*)DELETEHT )
    return (chain_list*)EMPTYHT;

  // Récupère la liste des agresseurs.
  headagr = (chain_list*)gethtitem( htsig, victime );
  if( headagr == (chain_list*)EMPTYHT || headagr == (chain_list*)DELETEHT )
    return (chain_list*)EMPTYHT;
  
  // Il ne reste plus qu'a placer les REALAGRESSOR.
  for( scan = headagr ; scan ; scan = scan->NEXT ) {
    param = (rcxparam*)scan->DATA;
    for( chain = param->SIGLOCAL ; chain ; chain = chain->NEXT ) {
      losig = (losig_list*)chain->DATA;
      losig->USER = addptype( losig->USER, RCX_REALAGRESSOR, param );
    }
    if( !getptype( param->SIGNAL->USER, RCX_REALAGRESSOR ) )
      param->SIGNAL->USER = addptype( param->SIGNAL->USER, 
                                      RCX_REALAGRESSOR, 
                                      param 
                                    );
  }

  return headagr;
}

void rcx_setagrlistfast( lofig_list *lofig,
                         char       *insname, 
                         losig_list *victime,
                         chain_list *headagr
                       )
{
  ptype_list    *ptl;
  ht            *htins;
  ht            *htsig;
  #ifdef RCX_CHECK
  chain_list    *oldagr;
  #endif
 
  // Récupère la table des instances de cette lofig.
  // Il s'agit des différents noms sous laquelle est instanciée cette lofig. Il
  // ne s'agit pas d'un table pour accéder rapidement à un LOINS de cette lofig.
  ptl = getptype( lofig->USER, RCX_HTAGR );
  if( !ptl ) {
    lofig->USER = addptype( lofig->USER, RCX_HTAGR, addht(1) );
    ptl = lofig->USER;
  }
  htins=(ht*)ptl->DATA;

  // Récupère la table des signaux.
  htsig=(ht*)gethtitem( htins, insname );
  if( htsig == (ht*)EMPTYHT || htsig == (ht*)DELETEHT ) {
    htsig = addht(10);
    addhtitem( htins, insname, (long)htsig );
  }
  

  // Récupère la liste des agresseurs.
  #ifdef RCX_CHECK
  oldagr = (chain_list*)gethtitem( htsig, victime );
  if( oldagr != (chain_list*)EMPTYHT && oldagr != (chain_list*)DELETEHT )
    rcx_error( 30, NULL, AVT_ERROR );
  #endif
  addhtitem( htsig, victime, (long)headagr );
}

/******************************************************************************\

rcx_getagrlist()
- Récupère une liste de rcxparam correspondant aux agresseurs possédant une vue
  rcx et n'étant pas marqué NI par la fonction rcx_checkcrosstalk().
- Place dans chaque agresseur local un ptype RCX_REALAGRESSOR vers le rcxparam.

\******************************************************************************/

chain_list* rcx_getagrlist( lofig_list *lofig,
                            losig_list *victime, 
                            char       *insname, 
                            chain_list *chainfig  
                          )
{
  rcx_list      *rcx;
  chain_list    *scanctc;
  loctc_list    *ctc;
  losig_list    *agresseur;
  losig_list    *real;
  chain_list    *headagr;
  chain_list    *scanagr;
  rcxparam      *param;
  char          *defins;
  lofig_list    *deffig;
  rcx_list      *rcxagr;
  ptype_list    *ptl;

  if( RCX_FASTMODE ) {
    headagr = rcx_getagrlistfast( lofig, insname, victime );
    if( headagr != (chain_list*)EMPTYHT )
      return headagr;
  }
  
  rcx = getrcx( victime );
  #ifdef RCX_CHECK
  if( rcx_state( rcx, RCX_STATE_LOFIG ) != RCX_STATE_TYPED )
    return NULL;
  if( !GETFLAG( rcx->FLAG, MODELRCN ) )
    rcx_error(31,NULL, AVT_ERROR);

  #endif

  headagr = NULL;

  if( ! GETFLAG( rcx->FLAG, RCXCTCDONE ) ) {
    if( !rcx_iscrosstalk( lofig, victime, RCX_TEST_LIMIT ))
      SETFLAG( rcx->FLAG, RCXNOCTC );
    SETFLAG( rcx->FLAG, RCXCTCDONE );
  }
  
  if( ! GETFLAG( rcx->FLAG, RCXNOCTC ) )
  {
    rcn_lock_signal( lofig, victime );
    rcx_checkcrosstalk( victime );

    for( scanctc = victime->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
    
      ctc = (loctc_list*)scanctc->DATA;
      agresseur = rcn_ctcothersig( ctc, victime );
      
      if( tlc_islosigalim( agresseur ) )
        continue;

      if( rcx_isnotinfluentagressor( victime, agresseur ) )
        continue;

      param = NULL;
      ptl = getptype( agresseur->USER, RCX_REALAGRESSOR );
      if( ptl ) 
        param = (rcxparam*)ptl->DATA;
      else {
        real = rcx_gethierlosig( agresseur, 
                                 insname, 
                                 chainfig, 
                                 &defins, 
                                 &deffig   
                               );
        if( real ) {
          if( real != agresseur )
            ptl = getptype( real->USER, RCX_REALAGRESSOR );
 
          if( !ptl ) {
            rcxagr = getrcx( real );
            if( rcx_state( rcxagr, RCX_STATE_LOFIG ) == RCX_STATE_TYPED ) {
            
              param = rcx_allocrcxparam();
              param->LOFIG    = deffig;
              param->INSNAME  = defins;
              param->SIGNAL   = real;
              param->SIGLOCAL = addchain( NULL, agresseur );
            
              agresseur->USER = addptype( agresseur->USER,
                                          RCX_REALAGRESSOR, 
                                          param
                                        );
              if( real != agresseur )
                real->USER = addptype( real->USER,
                                       RCX_REALAGRESSOR, 
                                       param
                                     );
              headagr = addchain( headagr, param );

            }
          }
          else {
            param = (rcxparam*)ptl->DATA;
            param->SIGLOCAL = addchain( param->SIGLOCAL, agresseur );
            agresseur->USER = addptype( agresseur->USER,
                                        RCX_REALAGRESSOR, 
                                        param
                                      );
          }
        }
      }

      if( param )
        param->CC = param->CC + ctc->CAPA ;
    }
    rcn_unlock_signal( lofig, victime );
  }

  for( scanagr = headagr ; scanagr ; scanagr = scanagr->NEXT ) {
    param = (rcxparam*)(scanagr->DATA);
    param->CCA = -1.0 ;
  }
  
  if( RCX_FASTMODE )
    rcx_setagrlistfast( lofig, insname, victime, headagr );

  return( headagr );
}

rcxparam* rcx_allocrcxparam( void )
{
  rcxparam *param;
  param = (rcxparam*)mbkalloc( sizeof( rcxparam ) );
  param->LOFIG    = NULL;
  param->INSNAME  = NULL;
  param->SIGNAL   = NULL;
  param->FMINUP   = 0.0;
  param->F0UP     = 0.0;
  param->FMINDW   = 0.0;
  param->F0DW     = 0.0;
  param->CC       = 0.0;
  param->CCA      = 0.0;
  param->ACTIF    = 0;
  param->USER     = NULL;
  param->SIGLOCAL = NULL;
  return( param );
}

void rcx_freeagrlist( lofig_list *lofig, 
                      losig_list *victime, 
                      chain_list *agresseurs 
                    )
{
  chain_list    *scan;
  chain_list    *siglocal;
  rcxparam      *param;
  losig_list    *losig;
  
  for( scan = agresseurs ; scan ; scan=scan->NEXT ) {

    param = ((rcxparam*)(scan->DATA));
    
    #ifdef RCX_CHECK
    if( param->USER ) {
      rcx_error(20,NULL, AVT_WARNING);
    }
    #endif
    for( siglocal = param->SIGLOCAL ; siglocal ; siglocal = siglocal->NEXT ) {
      losig = (losig_list*)siglocal->DATA;
      #ifdef RCX_CHECK
      if( !getptype( losig->USER, RCX_REALAGRESSOR ) )
        rcx_error(32,NULL, AVT_WARNING);
      #endif
      losig->USER = delptype( losig->USER, RCX_REALAGRESSOR );
    }
    if( getptype( param->SIGNAL->USER, RCX_REALAGRESSOR ) )
      param->SIGNAL->USER = delptype( param->SIGNAL->USER, RCX_REALAGRESSOR );
      
    if( !RCX_FASTMODE ) {
      freechain( param->SIGLOCAL );
      mbkfree( param );
    }
  }
  
  if( !RCX_FASTMODE )
    freechain( agresseurs );

  lofig=NULL;
  victime=NULL;
}

/******************************************************************************\
Renvoie le front réel sans la contribution du signal. 
cc=0    -> FCC
cc=cca  -> F0

arguments :

cc  : somme des capacité de couplage entre la paire de signaux.
cca : somme des capacité de couplage sur le signal.
fcc : front calculé avec les agresseurs actifs
f0  : front sans agression.

Si cca vaut 0, le fcc est renvoyé.
\******************************************************************************/

RCXFLOAT rcx_realslope( RCXFLOAT cc, RCXFLOAT cca, RCXFLOAT fcc, RCXFLOAT f0 )
{
  RCXFLOAT f;
  
  if( cca > 0.0 ) 
    f = ( f0 - fcc ) * ( cc - cca ) / cca + f0 ;
  else
    f = fcc ;

  return f;
}

/******************************************************************************\
Calcule l'effet miller entre 2 fronts.
type = RCX_SAME | RCX_OPPOSITE
model = RCX_MILLER2C ( capa entre 0C 2C ) | RCX_MILLER4C ( capa entre -C et 4C )
\******************************************************************************/

RCXFLOAT rcx_getmiller( RCXFLOAT fvic, RCXFLOAT fagr, char type, char model )
{
  RCXFLOAT m=1.0;

  if( type == RCX_SAME ) {
    if( model == RCX_MILLER2C ) {
      if( fvic < fagr )
        m = (fagr-fvic)/fagr;
      else
        m = 0.0;
    }
    else {
      if( fvic < 2.0 * fagr )
        m = (fagr-fvic)/fagr;
      else
        m = -1.0;
    }
  }

  if( type == RCX_OPPOSITE ) {
    if( model == RCX_MILLER2C ) {
      if( fvic < fagr )
        m = (fvic+fagr)/fagr;
      else
        m = 2.0;
    }
    else {
      if( fvic < 2.0 * fagr )
        m = (fvic+fagr)/fagr;
      else
        m = 3.0;
    }
  }

  return m;
}

/******************************************************************************\
Renvoie la valeur d'une capacité prenant en compte l'effet Miller. Les fronts
sont corrigés pour ne pas être pris de façon trop pessimiste.
type :  RCX_MIN | RCX_MAX.
\******************************************************************************/

RCXFLOAT rcx_capamiller( losig_list *victime, 
                         loctc_list *loctc,
                         rcx_slope  *slope,
                         char type 
                       )
{
  RCXFLOAT          cc;
  losig_list       *sigagr;
  ptype_list       *ptl;
  rcxparam         *param;
  RCXFLOAT          famin;
  RCXFLOAT          fvmax;
  RCXFLOAT          m;
 
  cc = loctc->CAPA ; 

  if( rcx_crosstalkactive( RCX_QUERY ) == RCX_NOCROSSTALK )
    return cc;

  sigagr = (loctc->SIG1 == victime ? loctc->SIG2 : loctc->SIG1 );

  ptl = getptype( sigagr->USER, RCX_REALAGRESSOR );

  if( ptl ) {
    
    param = (rcxparam*)ptl->DATA;
     
    if( type == RCX_MIN && (param->ACTIF & RCX_AGRBEST) == RCX_AGRBEST ) {

      switch( RCX_CTK_SLOPE_MILLER ) {

      case RCX_SLOPE_NOMINAL :
      
        fvmax = slope->F0MAX ;
        if( slope->SENS == TRC_SLOPE_UP )
          famin = param->F0UP;
        else
          famin = param->F0DW;
        break;

      case RCX_SLOPE_CTK :
      
        fvmax = slope->FCCMAX ;
        if( slope->SENS == TRC_SLOPE_UP )
          famin = param->FMINUP;
        else
          famin = param->FMINDW;
        break;

      default :
        rcx_error( 33, victime, AVT_FATAL );
      }
      
      switch( RCX_CTK_MODEL ) {
      
      case RCX_MILLER_0C2C :
        m=0.0 ;
        break;
        
      case RCX_MILLER_NC3C :
        m = rcx_getmiller( fvmax, famin, RCX_SAME, RCX_MILLER4C );
        break;

      case RCX_MILLER_NOMINAL :
        m = rcx_getmiller( fvmax, famin, RCX_SAME, RCX_MILLER2C );
        break;

      default :
        rcx_error( 34, victime, AVT_FATAL );
      }

      cc = loctc->CAPA * m ;
    }

    if( type == RCX_MAX && (param->ACTIF & RCX_AGRWORST) == RCX_AGRWORST ) {

      switch( RCX_CTK_SLOPE_MILLER ) {

      case RCX_SLOPE_NOMINAL :
      
        fvmax = slope->F0MAX ;
        if( slope->SENS == TRC_SLOPE_UP )
          famin = param->F0DW;
        else
          famin = param->F0UP;
          
        break;
        
      case RCX_SLOPE_CTK :
      
        fvmax = slope->FCCMAX ;
        if( slope->SENS == TRC_SLOPE_UP )
          famin = param->FMINDW;
        else
          famin = param->FMINUP;

        break;
        
      default :
        rcx_error(35,victime, AVT_FATAL);
      }
      
      switch( RCX_CTK_MODEL ) {
      
      case RCX_MILLER_0C2C :
        m=2.0 ;
        break;
        
      case RCX_MILLER_NC3C :
        m = rcx_getmiller( fvmax, famin, RCX_OPPOSITE, RCX_MILLER4C );
        break;
        
      case RCX_MILLER_NOMINAL :
        m = rcx_getmiller( fvmax, famin, RCX_OPPOSITE, RCX_MILLER2C );
        break;

      default :
        rcx_error( 13, victime, AVT_FATAL );
      }

      cc = loctc->CAPA * m ;
    }

  }
  
  else {

    if( rcx_isnotinfluentagressor( victime, sigagr ) ) {
      switch( type ) {
      case RCX_MIN:
        cc = 0.0;
        break;
      case RCX_MAX:
        cc = 2.0 * loctc->CAPA;
        break;
      }
    }
  }

  return( cc );
}

/******************************************************************************\
Flags entre deux signaux couplés.
La fonction rcx_getflagcoupled() renvoie seulement les bits sélectionnés si flag
est différent de 0, et la totalité des bits si flag vaut 0.
\******************************************************************************/

void rcx_setflagcoupled( losig_list *victime, losig_list *agresseur, long flag )
{
  long f;
  
  f = rcn_getcoupledinformation( victime, agresseur, RCXLOSIGFLAG );
  rcn_setcoupledinformation( victime, agresseur, f|flag, RCXLOSIGFLAG );
}

int rcx_getflagcoupled( losig_list *victime, losig_list *agresseur, long flag )
{
  long f;
  
  f = rcn_getcoupledinformation( victime, agresseur, RCXLOSIGFLAG );
  if( flag == 0 )
    return f;
  return f & flag;
}

void rcx_clearflagcoupled( losig_list *victime, 
                          losig_list *agresseur, 
                          long flag 
                        )
{
  long f;
  f = rcn_getcoupledinformation( victime, agresseur, RCXLOSIGFLAG );
  rcn_setcoupledinformation( victime, agresseur, f & ~flag, RCXLOSIGFLAG );
}

void rcx_freeflagcoupled( losig_list *victime )
{
  rcn_freecoupledinformation( victime, RCXLOSIGFLAG );
}

/******************************************************************************\
Impose que l'agresseur est à considérer comme non influent, c'est à dire qu'on
le considère toujours actif pour ne pas l'avoir dans le stboverlapdev.
\******************************************************************************/

int rcx_isnotinfluentagressor( losig_list *victime, losig_list *agresseur )
{
  if( rcx_getflagcoupled( victime, agresseur, RCX_NI_LOSIG )== RCX_NI_LOSIG )
    return 1;
  if( getptype( agresseur->USER, RCX_VSSNI ) )
    return 1;
  return 0;
}

void rcx_setnotinfluentagressor( losig_list *victime, losig_list *agresseur )
{
  rcx_list *rcx;

  rcx = getrcx( victime );
  if( !rcx || GETFLAG( rcx->FLAG, RCXNOCTC ) ) return;

  rcx_setflagcoupled( victime, agresseur, RCX_NI_LOSIG );
}

void rcx_clearnotinfluentagressor( losig_list *victime, losig_list *agresseur )
{
  rcx_clearflagcoupled( victime, agresseur, RCX_NI_LOSIG );
}

int rcx_iscrosstalkcapa( loctc_list *loctc, losig_list *losig )
{
  rcx_list *rcx;
  losig_list *sigagr;

  rcx = getrcx( losig );
  if( !rcx )
    return 1;

  if( GETFLAG( rcx->FLAG, RCXNOCTC ) )
    // On considère que toutes les CTC sont des capacités à la masse.
    return 0;

  sigagr = rcn_ctcothersig( loctc, losig );

  if( getrcx( sigagr ) ) {
    return 1;
  }

  if( rcx_isnotinfluentagressor( losig, sigagr ) ) {
    // Capa de couplage non significative, mais capa de couplage quand même...
    return 1;
  }

  if( rcx_isbellow( sigagr ) ) {
    return 1;
  }

  return 0;
}

int rcx_crosstalkactive( int model )
{
  if( model != RCX_QUERY )
    RCX_CROSSTALK = model;
  return RCX_CROSSTALK;
}

void rcx_freewire( losig_list *losig )
{
  rcx_list *rcx;
  rcx = getrcx( losig );
  SETFLAG( rcx->FLAG, RCXNOWIRE );
  freetable(losig);
}

void rcx_freectc( losig_list *losig )
{
  rcx_list *rcx;
  rcx = getrcx( losig );
  if( !rcx ) return;
  SETFLAG( rcx->FLAG, RCXNOCTC );
}

int setrcxmodel( lofig_list *lofig, losig_list *losig, int request )
{
  int hasdelayrc;
  int retval=RCX_ALL;
  ptype_list *ptl;
  locon_list *locon;
  chain_list *chain;
  rcx_list   *rcx;
  float       ccl, ccg, cm;

 
  if( !losig->PRCN )
    return( RCX_NORCTC );

  if( rcx_isbellow( losig ) ) 
    return( RCX_NORCTC );

  if( tlc_islosigalim( losig ) ) {
    return( RCX_NORCTC );
  }

  rcx = getrcx( losig );
  if( !rcx ) return( RCX_NORCTC );

  switch( request ) {
  case RCX_NOCTC :
    SETFLAG( rcx->FLAG, RCXNOCTC );
    break;
  case RCX_NOR :
    if( losig->TYPE!='E')
      SETFLAG( rcx->FLAG, RCXNOWIRE );
    break;
  case RCX_NORCTC :
    SETFLAG( rcx->FLAG, RCXNOWIRE );
    SETFLAG( rcx->FLAG, RCXNOCTC );
    break;
  case RCX_BEST :
    break;
  default :
    rcx_error( 36, losig, AVT_FATAL );
  }

  hasdelayrc   = rcx_isrcdelay( lofig, losig );
  if( !hasdelayrc ) rcx_freewire( losig );
 
  if( GETFLAG( rcx->FLAG, RCXNOWIRE ) && GETFLAG( rcx->FLAG, RCXNOCTC ) )
    retval = RCX_NORCTC;
  if( !GETFLAG( rcx->FLAG, RCXNOWIRE ) && GETFLAG( rcx->FLAG, RCXNOCTC ) )
    retval = RCX_NOCTC;
  if( GETFLAG( rcx->FLAG, RCXNOWIRE ) && !GETFLAG( rcx->FLAG, RCXNOCTC ) )
    retval = RCX_NOR;
  if( !GETFLAG( rcx->FLAG, RCXNOWIRE ) && !GETFLAG( rcx->FLAG, RCXNOCTC ) )
    retval = RCX_ALL;
    
  if( retval == RCX_NORCTC && V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL0 ) {
    // On met seulement la capa à jour
    ptl = getptype( losig->USER, LOFIGCHAIN );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA;
        if( !rcx_isvalidlocon( locon ) )
          continue;
        rcx_getloinscapa( locon, &cm, &ccl, &ccg );
        rcn_addcapa( losig, cm + ccl );
      }
    }
    // Force le calcul de la capa de RCN
    rcn_getcapa( lofig, losig );
    delrcx( losig );
  }

  return retval;
}

char* rcx_isbellow( losig )
losig_list *losig;
{
  ptype_list *ptl;
  ptl = getptype( losig->USER, RCX_BELLOW );
  if( ptl )
    return (char*)ptl->DATA;
  return NULL;
}

int rcx_isloop( ptrcx, losig )
rcx_list	*ptrcx;
losig_list      *losig;
{
  int           r ;
  lowire_list  *wire ;
  lonode_list  *ptnode ;
  ht           *htpack ;
  chain_list   *headpack ;
  chain_list   *scanpack ;
  rcx_triangle *pack ;
  
  if( GETFLAG( ptrcx->FLAG, RCXHASNOLOOP ) ) return 0;
  if( GETFLAG( ptrcx->FLAG, RCXHASLOOP ) ) return 1;

  if( RCX_USING_AWEMATRIX == RCX_USING_AWEMATRIX_FORCE ) {
    /* Le 6/04/2005, il y a une boucle infinie chez Sony dans le 
       parcours des rc : lorsqu'on est en mode force, on considère
       que tous les rc ont des boucles et il ne faut surtout pas
       passer par rcn_treetrip */
    SETFLAG( ptrcx->FLAG, RCXHASLOOP );
    return 1 ;
  }

  avt_logenterfunction(LOGTRC,2, "rcx_isloop()");

  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );
  
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
  
    ptnode = getlonode( losig, wire->NODE1 ) ;
    if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_PASS ) )
      continue ;
      
    r = rcn_treetrip( losig, 
                      ptnode, 
         (int (*)(losig_list*,lonode_list*,lowire_list*,void*)) rcx_up_forloop, 
         (int (*)(losig_list*,lonode_list*,chain_list*,ht*,void*))
                                                                 rcx_dw_forloop,
         (int (*)(losig_list*,lonode_list*,void*,void*)) rcx_up_pack_forloop,
                      NULL,
                      0
                    );

    if( r == RCNTREETRIPLOOP ) {
      SETFLAG( ptrcx->FLAG, RCXHASLOOP );
      break ;
    }
  }

  if( !wire )
    SETFLAG( ptrcx->FLAG, RCXHASNOLOOP );

  if( GETFLAG( ptrcx->FLAG, RCXHASNOLOOP ) ) {
    if ( avt_islog(2,LOGTRC) ) {
      avt_log(LOGTRC,2,"no loop found\n" );
    }
    avt_logexitfunction(LOGTRC,2);
    return 0;
  }

  if( GETFLAG( ptrcx->FLAG, RCXHASLOOP ) ) {
    avt_log(LOGTRC,2,"loop found\n" );
    /* il faut effacer les triangles qui ont commencés à être construits */
    htpack = addht( 100 );
    headpack = NULL ;
    for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
      pack = rcn_getpack( losig, wire );
      if( pack ) {
        if( gethtitem( htpack, pack ) == EMPTYHT ) {
          headpack = addchain( headpack, pack );
          addhtitem( htpack, pack, 1 );
        }
      }
    }
    for( scanpack = headpack ; scanpack ; scanpack = scanpack->NEXT ) {
      pack = (rcx_triangle*)scanpack->DATA ;
      rcx_cleantriangle_node( losig, NULL, pack );
    }
    freechain( headpack );
    delht( htpack ) ;
  }

  avt_logexitfunction(LOGTRC,2);
  return 1 ;
}

int rcx_dw_forloop( losig_list  *losig,
                    lonode_list *lonode,
                    chain_list  *chwire,
                    ht          *htpack,
                    void        *userdata
                  )
{
  chain_list   *headht ;
  chain_list   *scanht ;
  rcx_triangle *tr ;
  lowire_list  *wire ;

  headht = GetAllHTElems( htpack );

  for( scanht = headht ; scanht ; scanht = scanht->NEXT ) {
    wire = (lowire_list*)((chain_list*)scanht->DATA)->DATA ;
    tr = (rcx_triangle*)rcn_getpack( losig, wire );
    rcx_cleantriangle_node( losig, lonode, tr );
  }

  chwire = NULL ;
  userdata = NULL ;

  freechain( headht );
  return 1 ;
}

int rcx_up_pack_forloop( losig_list *losig,
                         lonode_list *lonode,
                         rcx_triangle *tr,
                         void         *userdata
                       )
{
  chain_list  *chain ;
  lonode_list *tstnode ;

  if( tr->n0 != lonode ) rcx_triangle_node( losig, tr->n0, NULL );
  if( tr->n1 != lonode ) rcx_triangle_node( losig, tr->n1, NULL );
  if( tr->n2 != lonode ) rcx_triangle_node( losig, tr->n2, NULL );
  for( chain = tr->REPORTED_IN ; chain ; chain = chain->NEXT ) {
    tstnode = (lonode_list*)chain->DATA ;
    if( tstnode != lonode ) rcx_triangle_node( losig, tstnode, NULL );
  }
  for( chain = tr->REPORTED_N1 ; chain ; chain = chain->NEXT ) {
    tstnode = (lonode_list*)chain->DATA ;
    if( tstnode != lonode ) rcx_triangle_node( losig, tstnode, NULL );
  }
  for( chain = tr->REPORTED_N2 ; chain ; chain = chain->NEXT ) {
    tstnode = (lonode_list*)chain->DATA ;
    if( tstnode != lonode ) rcx_triangle_node( losig, tstnode, NULL );
  }

  userdata = NULL ;
  return 1 ;
}

int rcx_up_forloop( losig_list  *losig, 
                    lonode_list *lonode,
                    lowire_list *lowire,
                    void        *userdata
                  )
{
  rcx_triangle_node( losig, lonode, lowire );
  userdata = NULL ;
  return 1 ;
}

void rcx_env_error( char *msg, char *was )
{
  avt_errmsg( TRC_ERRMSG, "016", AVT_FATAL, msg, was );
}

void rcx_env( void )
{
  char *str;
  char *ptend;
  RCXFLOAT value;
  unsigned long int size;

  if( STM_DEFAULT_SMAXR >= 0.0 && STM_DEFAULT_SMINR >= 0.0 )
    RCX_SLOPE_UNSHRINK = STM_DEFAULT_SMAXR - STM_DEFAULT_SMINR ;
  else
    RCX_SLOPE_UNSHRINK = 0.6 ;

    if( V_FLOAT_TAB[__RCX_MINRCSIGNAL].VALUE < 0.0 ) 
      rcx_env_error( "RCX_MINRCSIGNAL must be a positive RCXFLOATing number.",
                     str
                   );
    RCX_MINRCSIGNAL = V_FLOAT_TAB[__RCX_MINRCSIGNAL].VALUE;

    if( V_FLOAT_TAB[__RCX_MINRELCTKSIGNAL].VALUE < 0.0 || V_FLOAT_TAB[__RCX_MINRELCTKSIGNAL].VALUE > 1.0 )
      rcx_env_error( 
            "RCX_MINRELCTKSIGNAL must be a RCXFLOATing number between 0 and 1.",
                     str
                   );
    RCX_MINRELCTKSIGNAL = V_FLOAT_TAB[__RCX_MINRELCTKSIGNAL].VALUE;

    if( V_FLOAT_TAB[__RCX_MINRELCTKFILTER].VALUE < 0.0 || V_FLOAT_TAB[__RCX_MINRELCTKFILTER].VALUE > 1.0 )
      rcx_env_error( 
            "RCX_MINRELCTKFILTER must be a RCXFLOATing number between 0 and 1.",
                     str
                   );
    RCX_MINRELCTKFILTER = V_FLOAT_TAB[__RCX_MINRELCTKFILTER].VALUE ;

    if( V_FLOAT_TAB[__RCX_CAPALOAD_MAXRC].VALUE < 0.0 )
      rcx_env_error( 
                    "RCX_CAPALOAD_MAXRC must be a positive RCXFLOATing number.",
                     str
                   );
    RCX_CAPALOAD_MAXRC = V_FLOAT_TAB[__RCX_CAPALOAD_MAXRC].VALUE ;
  
    if( V_FLOAT_TAB[__RCX_MAXDELTALOAD].VALUE < 0.0 )
      rcx_env_error( "RCX_MAXDELTALOAD must be a positive RCXFLOATing number.",
                     str
                   );
    RCX_MAXDELTALOAD = V_FLOAT_TAB[__RCX_MAXDELTALOAD].VALUE ;

  str = getenv("RCX_FREEUNUSEDCTC");
  if( str ) {
    if( strcmp( str, "yes" ) == 0 )
      RCX_FREEUNUSEDCTC = 'Y';
    else {
      if( strcmp( str, "no" ) == 0 )
        RCX_FREEUNUSEDCTC = 'N';
      else
        rcx_env_error( "Allowed value for RCX_FREEUNUSEDCTC is 'yes' or 'no'.",
                       str
                     );
    }
  }

  str = getenv("RCX_USECACHE");
  if( str ) {
    if( strcmp( str, "yes" ) == 0 )
      RCX_USECACHE = 'Y';
    else {
      if( strcmp( str, "no" ) == 0 )
        RCX_USECACHE = 'N';
      else
        rcx_env_error( "Allowed value for RCX_USECACHE is 'yes' or 'no'.",
                       str
                     );
    }
  }

      RCX_USE_MATRIX_LOAD = V_BOOL_TAB[__RCX_USE_MATRIX_LOAD].VALUE?'Y':'N';

  str = V_STR_TAB[__RCX_CTK_MODEL].VALUE;
  if( str ) {
    RCX_CTK_MODEL = 0;
    if( !strcmp( str, "MILLER_0C2C" ) )
      RCX_CTK_MODEL = RCX_MILLER_0C2C ;
    if( !strcmp( str, "MILLER_NC3C" ) )
      RCX_CTK_MODEL = RCX_MILLER_NC3C ;
    if( !strcmp( str, "MILLER_NOMINAL" ) )
      RCX_CTK_MODEL = RCX_MILLER_NOMINAL ;
    if( !RCX_CTK_MODEL )
      rcx_env_error( 
"Allowed value for RCX_CTK_MODEL : MILLER_0C2C | MILLER_NOMINAL | MILLER_NC3C.",
                     str
                   );
  }
  
  str = V_STR_TAB[__RCX_CTK_NOISE].VALUE;
  if( str ) {
    RCX_CTK_NOISE = 0;
    if( !strcmp( str, "NOISE_NEVER" ) )
      RCX_CTK_NOISE = RCX_NOISE_NEVER ;
    if( !strcmp( str, "NOISE_ALWAYS" ) )
      RCX_CTK_NOISE = RCX_NOISE_ALWAYS ;
    if( !strcmp( str, "NOISE_IFSTRONG" ) )
      RCX_CTK_NOISE = RCX_NOISE_IFSTRONG ;
    if( !strcmp( str, "NOISE_CUSTOM" ) )
      RCX_CTK_NOISE = RCX_NOISE_CUSTOM ;
    if( !strcmp( str, "NOISE_FINE" ) )
      RCX_CTK_NOISE = RCX_NOISE_FINE ;
    if( !RCX_CTK_NOISE )
      rcx_env_error( "Allowed value for RCX_CTK_NOISE : NOISE_NEVER | NOISE_ALWAYS | NOISE_IFSTRONG | NOISE_CUSTOM | NOISE_FINE.\n",
                     str
                   );
  }

      RCX_FASTMODE = V_BOOL_TAB[__RCX_FASTMODE].VALUE;

  str = getenv("RCX_AWESTRAIGHT");
  if( str ) {
    if( strcmp( str, "yes" ) == 0 )
      RCX_AWESTRAIGHT = 1;
    else {
      if( strcmp( str, "no" ) == 0 )
        RCX_AWESTRAIGHT = 0;
      else
        rcx_env_error( "Allowed value for RCX_AWESTRAIGHT is 'yes' or 'no'.", 
                       str 
                     );
    }
  }

  str = getenv("RCX_STEPTANH");
  if( str ) {
    if( strcmp( str, "yes" ) == 0 )
      RCX_STEPTANH = 1;
    else {
      if( strcmp( str, "no" ) == 0 )
        RCX_STEPTANH = 0;
      else
        rcx_env_error( "Allowed value for STEPTANH is 'yes' or 'no'.", str );
    }
  }

  str = V_STR_TAB[__RCX_CTK_SLOPE_NOISE].VALUE;
  if( str ) {
    RCX_CTK_SLOPE_NOISE = RCX_SLOPE_NONE ;
    if( strcmp( str, "SLOPE_CTK" ) == 0 )
      RCX_CTK_SLOPE_NOISE = RCX_SLOPE_CTK ;
    if( strcmp( str, "SLOPE_NOMINAL" ) == 0 )
      RCX_CTK_SLOPE_NOISE = RCX_SLOPE_NOMINAL ;
    if( strcmp( str, "SLOPE_REAL" ) == 0 )
      RCX_CTK_SLOPE_NOISE = RCX_SLOPE_REAL ;
    if( RCX_CTK_SLOPE_NOISE == RCX_SLOPE_NONE )
      rcx_env_error( "Allowed value for RCX_CTK_SLOPE_NOISE is SLOPE_CTK | SLOPE_NOMINAL | SLOPE_REAL\n",
                     str
                   );
  }

  str = getenv("RCX_CTK_SLOPE_MILLER");
  if( str ) {
    RCX_CTK_SLOPE_MILLER = RCX_SLOPE_NONE ;
    if( strcmp( str, "SLOPE_CTK" ) == 0 )
      RCX_CTK_SLOPE_MILLER = RCX_SLOPE_CTK ;
    if( strcmp( str, "SLOPE_NOMINAL" ) == 0 )
      RCX_CTK_SLOPE_MILLER = RCX_SLOPE_NOMINAL ;
    if( RCX_CTK_SLOPE_MILLER == RCX_SLOPE_NONE )
      rcx_env_error( 
        "Allowed value for RCX_CTK_SLOPE_MILLER is SLOPE_CTK | SLOPE_NOMINAL\n",
                     str
                   );
  }

  str = V_STR_TAB[__RCX_CTK_SLOPE_DELAY].VALUE;
  if( str ) {
    RCX_CTK_SLOPE_DELAY = RCX_SLOPE_DELAY_NONE;
    if( strcmp( str, "SLOPE_DELAY_CTK" ) == 0 )
      RCX_CTK_SLOPE_DELAY = RCX_SLOPE_DELAY_CTK ;
    if( strcmp( str, "SLOPE_DELAY_ENHANCED" ) == 0 )
      RCX_CTK_SLOPE_DELAY = RCX_SLOPE_DELAY_ENHANCED ;
    if( RCX_CTK_SLOPE_DELAY == RCX_SLOPE_DELAY_NONE )
      rcx_env_error( "Allowed value for RCX_CTK_SLOPE_DELAY is SLOPE_DELAY_CTK | SLOPE_DELAY_ENHANCED\n",
                     str
                   );
  }

  str = V_STR_TAB[__RCX_USING_AWEMATRIX].VALUE;
  if( str ) {
    RCX_USING_AWEMATRIX=0;
    if( strcmp( str, "never"  )==0 ) RCX_USING_AWEMATRIX = RCX_USING_AWEMATRIX_NEVER ;
    if( strcmp( str, "always" )==0 ) RCX_USING_AWEMATRIX = RCX_USING_AWEMATRIX_ALWAYS ;
    if( strcmp( str, "ifneed" )==0 ) RCX_USING_AWEMATRIX = RCX_USING_AWEMATRIX_IFNEED ;
    if( strcmp( str, "force"  )==0 ) RCX_USING_AWEMATRIX = RCX_USING_AWEMATRIX_FORCE ;
    if( !RCX_USING_AWEMATRIX ) {
      rcx_env_error( "Allowed value for RCX_USING_AWEMATRIX is never | always | ifneed | force\n", str );
    }
  }

  str = getenv("RCX_AWE_FAST_MODE") ;
  if( str ) {
    if( strcmp( str, "yes" ) == 0 )
      AWE_FAST_MODE = 'Y';
    else {
      if( strcmp( str, "no" ) == 0 )
        AWE_FAST_MODE = 'N';
      else
        rcx_env_error( "Allowed value for AWE_FAST_MODE is 'yes' or 'no'.", str );
    }
  }

  str = V_STR_TAB[__RCX_DELAY_CACHE].VALUE;
  if( str ) {
    size = strtoul( str, &ptend, 10 );
    
    if( *ptend != '\0' ) {
      if( strcasecmp( ptend, "kb" )==0 )
        size = size * 1024;
      else {
        if( strcasecmp( ptend, "mb" )==0 )
          size = size * 1048576;
        else {
          if( strcasecmp( ptend, "gb" )==0 )
            size = size * 1073741824;
          else {
            avt_errmsg( RCN_ERRMSG, "002", AVT_FATAL );
          }
        }
      }
    }
    rcx_init_delay_cache( size );
  }
  else
    rcx_init_delay_cache( 10*1024*1024 );
  
  str = getenv( "RCX_AWE_ONE_PNODE" );
  if( str ) {
    RCX_AWE_ONE_PNODE = 0;
    if( strcmp( str, "yes" )==0 ) RCX_AWE_ONE_PNODE='Y' ;
    if( strcmp( str, "no" )==0 ) RCX_AWE_ONE_PNODE='N' ;
    if( !RCX_AWE_ONE_PNODE )
      rcx_env_error( "Allowed value for RCX_AWE_ONE_PNODE is yes or no\n", str );
    switch( RCX_AWE_ONE_PNODE ) {
    case 'Y' : printf( "yes\n" ); break ;
    case 'N' : printf( "no\n" ); break ;
    }
  }
 
  str = getenv( "RCX_AWE_MIN_RESI" );
  if( str ) {
    AWE_MIN_RESI_VALUE = atof( str );
    if( AWE_MIN_RESI_VALUE <= 1.0e-6 ) {
      rcx_env_error( "Argument for RCX_AWE_MIN_RESI must be a floating number greater than 1.0e-6\n", str );
    }
  }

  str = getenv( "RCX_PLOT_AWE" );
  if( str )
    RCX_PLOT_AWE = namealloc(str);
  
  if( mcc_use_multicorner() ) {
 
    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    
    case ELP_CAPA_LEVEL0 :
      RCX_CAPA_NOM_MIN    = 0 ;
      RCX_CAPA_NOM_MAX    = 1 ;
      RCX_CAPA_NOMF_MIN   = 2 ;
      RCX_CAPA_NOMF_MAX   = 3 ;
      RCX_NB_CAPA_L0      = 4 ;
      break ;

    case ELP_CAPA_LEVEL1 :
      RCX_CAPA_UP_NOM_MIN  = 0 ;
      RCX_CAPA_UP_NOM_MAX  = 1 ;
      RCX_CAPA_DW_NOM_MIN  = 2 ;
      RCX_CAPA_DW_NOM_MAX  = 3 ;
      RCX_CAPA_UP_NOMF_MIN = 4 ;
      RCX_CAPA_UP_NOMF_MAX = 5 ;
      RCX_CAPA_DW_NOMF_MIN = 6 ;
      RCX_CAPA_DW_NOMF_MAX = 7 ;
      RCX_NB_CAPA_L1       = 8 ;
      break ;

    case ELP_CAPA_LEVEL2 :
      RCX_CAPA_UP_MIN      =  0 ;
      RCX_CAPA_UP_NOM_MIN  =  1 ;
      RCX_CAPA_UP_NOM_MAX  =  2 ;
      RCX_CAPA_UP_MAX      =  3 ;
      RCX_CAPA_DW_MIN      =  4 ;
      RCX_CAPA_DW_NOM_MIN  =  5 ;
      RCX_CAPA_DW_NOM_MAX  =  6 ;
      RCX_CAPA_DW_MAX      =  7 ;
      RCX_CAPA_UP_NOMF_MIN =  8 ;
      RCX_CAPA_UP_NOMF_MAX =  9 ;
      RCX_CAPA_DW_NOMF_MIN = 10 ;
      RCX_CAPA_DW_NOMF_MAX = 11 ;
      RCX_NB_CAPA_L2       = 12 ;
      break ;
    }
  }
  else {
  
    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    
    case ELP_CAPA_LEVEL0 :
      RCX_CAPA_NOM_MIN    = 0 ;
      RCX_CAPA_NOM_MAX    = 0 ;
      RCX_CAPA_NOMF_MIN   = 1 ;
      RCX_CAPA_NOMF_MAX   = 1 ;
      RCX_NB_CAPA_L0      = 2 ;
      break ;

    case ELP_CAPA_LEVEL1 :
      RCX_CAPA_UP_NOM_MIN   = 0 ;
      RCX_CAPA_UP_NOM_MAX   = 0 ;
      RCX_CAPA_DW_NOM_MIN   = 1 ;
      RCX_CAPA_DW_NOM_MAX   = 1 ;
      RCX_CAPA_UP_NOMF_MIN  = 2 ;
      RCX_CAPA_UP_NOMF_MAX  = 2 ;
      RCX_CAPA_DW_NOMF_MIN  = 3 ;
      RCX_CAPA_DW_NOMF_MAX  = 3 ;
      RCX_NB_CAPA_L1        = 4 ;
      break ;

    case ELP_CAPA_LEVEL2 :
      RCX_CAPA_UP_MIN       = 0 ;
      RCX_CAPA_UP_NOM_MIN   = 1 ;
      RCX_CAPA_UP_NOM_MAX   = 1 ;
      RCX_CAPA_UP_MAX       = 2 ;
      RCX_CAPA_DW_MIN       = 3 ;
      RCX_CAPA_DW_NOM_MIN   = 4 ;
      RCX_CAPA_DW_NOM_MAX   = 4 ;
      RCX_CAPA_DW_MAX       = 5 ;
      RCX_CAPA_UP_NOMF_MIN  = 6 ;
      RCX_CAPA_UP_NOMF_MAX  = 6 ;
      RCX_CAPA_DW_NOMF_MIN  = 7 ;
      RCX_CAPA_DW_NOMF_MAX  = 7 ;
      RCX_NB_CAPA_L2        = 8 ;
      break ;
    }
  }
}

void rcx_setlosigbreakloop( losig_list *losig )
{
  rcx_list      *ptrcx;

  ptrcx = getrcx( losig );
  if( !ptrcx ) {
    return;
  }

  SETFLAG( ptrcx->FLAG, RCXBREAKLOOP );
}

int rcx_islosigbreakloop( losig_list *losig )
{
  rcx_list      *ptrcx;

  ptrcx = getrcx( losig );
  
  if( !ptrcx ) {
    return 0;
  }

  if( GETFLAG( ptrcx->FLAG, RCXBREAKLOOP ) )
    return 1;
  return 0;
}


/******************************************************************************\
Fonction qui répare les locon de la lofig qui ont leur PNODE à 0.

Hypothèses :

- Le signal doit être présent en mémoire.
- On a pas le droit de modifier des PNODE existant car ils ont peut être été
référencés par d'autres signaux via la fonction rcx_get_new_num_node().

\******************************************************************************/
void rcx_repair_pnode( losig_list *losig )
{
  ptype_list *ptl;
  rcx_list   *rcx;
  chain_list *chain;
  locon_list *locon;
  losig_list *sigint;
  rcx_list   *rcxint;
  num_list   *scan;
  int         goodnode;

  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( !ptl ) return;

  goodnode = -1;
  for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
    locon = (locon_list*)chain->DATA;
    if( !rcx_isvalidlocon( locon ) )
      continue;
    if( locon->PNODE ) {
      goodnode = locon->PNODE->DATA;
      break;
    }
  }

  rcx = getrcx( losig );

  if( goodnode == -1 ) // aucun locon n'a de PNODE.
    goodnode = losig->PRCN->NBNODE;

  // Idée de bug : ca n'arrivera jamais, mais que ce passe t'il si il y 
  // a des résistances...

  for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
  
    locon = (locon_list*)chain->DATA;
    
    if( !locon->PNODE ) {
      chain_list *cl;
      ptl = getptype( locon->USER, PTYPERCX );
      if( ptl ) {
        sigint = (losig_list*)ptl->DATA;
        rcxint = getrcx( sigint );
        for (cl=rcxint->RCXEXTERNAL; cl!=NULL; cl=cl->NEXT)
          for( scan = ((locon_list *)cl->DATA)->PNODE ; scan ; scan = scan->NEXT ) {
            setloconnode( locon, goodnode );
          }
      }
      else {
        setloconnode( locon, goodnode );
      }
    }
  }
}

/******************************************************************************\

Fonctions utilisées dans buildrcx pour déterminer le numéro d'un noeud d'un
signal.
Hypothèse : 
- On ne connait pas le NBNODE sur les signaux de la lofig (si on a jamais eu
  de refresh sur le signal).
- On connait le NBNODE sur les signaux des instances.

\******************************************************************************/

int rcx_get_new_num_node( losig_list *losig, 
                          int node, 
                          char where, 
                          char withnowire )
{
  int         base ;
  int         base_ext ;
  int         newnode;
  rcx_list   *rcxint ;
  rcx_list   *ptrcx_ext ;
  locon_list *locon_ext ;
  num_list   *num_int, *num_ext ;
  chain_list *cl;
  
  base = rcx_get_node_base( losig );

  switch( where ) {
  case RCX_LOINS :
  
    rcxint = getrcx( losig );
    locon_ext = (locon_list*)getptype( losig->USER, RCX_LOCONINS )->DATA ;
    ptrcx_ext = getrcx( locon_ext->SIG );

    // Cas où le connecteur externe ne contient pas de pnode
    if( !locon_ext->PNODE ) {
      if( withnowire==YES && GETFLAG( ptrcx_ext->FLAG, RCXNOWIRE ) ) {
        base_ext = 1;
      }
      else {
        base_ext = rcx_get_node_base( locon_ext->SIG ) + 
                   locon_ext->SIG->PRCN->NBNODE ;
      }

      for (cl=rcxint->RCXEXTERNAL; cl!=NULL; cl=cl->NEXT)
        for( num_int = ((locon_list *)cl->DATA)->PNODE ;
             num_int ; 
             num_int = num_int->NEXT )
          setloconnode( locon_ext, base_ext );
    }
  
    if( withnowire==YES && GETFLAG( ptrcx_ext->FLAG, RCXNOWIRE ) )
      newnode = 1;
    else {
      if( withnowire==YES && GETFLAG( rcxint->FLAG, RCXNOWIRE ) ) {
        newnode = rcx_get_new_num_node( locon_ext->SIG, 
                                        locon_ext->PNODE->DATA,
                                        RCX_LOFIG,
                                        withnowire
                                      );
      }
      else {
        for (cl=rcxint->RCXEXTERNAL; cl!=NULL; cl=cl->NEXT)
          for( num_int = ((locon_list *)cl->DATA)->PNODE,
               num_ext = locon_ext->PNODE ;
               num_int ; 
               num_int = num_int->NEXT, num_ext = num_ext->NEXT )
            if( num_int->DATA == node )
              break;
            
        if( num_int )
          newnode = rcx_get_new_num_node( locon_ext->SIG, 
                                          num_ext->DATA, 
                                          RCX_LOFIG,
                                          withnowire
                                        );
        else
          newnode = node + base;
      }
    }
    break;
    
  case RCX_LOFIG :
    ptrcx_ext = getrcx( losig );
    if( withnowire==YES && GETFLAG( ptrcx_ext->FLAG, RCXNOWIRE ) )
      newnode = 1;
    else
      newnode = node + base;
    break;
  }
  
  return newnode;
}

// Créé les infos pour les signaux qui auront des noeuds renumérotés.

void rcx_build_new_num_node( lofig_list *lofig )
{
  losig_list *sigext, *sigint ;
  ptype_list *ptl ;
  int         n ;
  chain_list *chain ;
  locon_list *locon ;

  for( sigext = lofig->LOSIG ; sigext ; sigext = sigext->NEXT ) {

    n = 0;

    ptl = getptype( sigext->USER, LOFIGCHAIN );
    
    if( ptl ) {
      
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        
        locon = (locon_list*)chain->DATA ;
        if( !rcx_isvalidlocon( locon ) )
          continue;

        if( locon->TYPE == 'I' ) {

          ptl = getptype( locon->USER, PTYPERCX ) ;
          if( ptl ) {
            
            sigint = (losig_list*)(ptl->DATA) ;
            if (rcx_set_node_base( sigint, n ))
              n = n + sigint->PRCN->NBNODE ;
          }
        }
      }
    }
    rcx_set_node_base( sigext, n );
  }
}

// Libère les infos pour les signaux qui auront des noeuds renumérotés.

void rcx_clear_new_num_node( lofig_list *lofig )
{
  losig_list *sigext, *sigint ;
  ptype_list *ptl ;
  chain_list *chain ;
  locon_list *locon ;

  for( sigext = lofig->LOSIG ; sigext ; sigext = sigext->NEXT ) {
  
    ptl = getptype( sigext->USER, LOFIGCHAIN );
    
    if( ptl ) {
      
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
      
        locon = (locon_list*)chain->DATA ;
        if( !rcx_isvalidlocon( locon ) )
          continue;

        if( locon->TYPE == 'I' ) {

          ptl = getptype( locon->USER, PTYPERCX ) ;
          if( ptl ) {
          
            sigint = (losig_list*)(ptl->DATA) ;
            rcx_clear_node_base( sigint ) ;
          }
        }
      }
    }
    rcx_clear_node_base( sigext ) ;
  }
}

// Le losig est interne à une loins. Récupère le nom du signal à l'extérieur de
// la loins.
losig_list* rcx_get_out_ins_sig( losig_list *losig )
{
  locon_list *locon_ext;
  losig_list *sig_ext;
  
  locon_ext = (locon_list*)getptype( losig->USER, RCX_LOCONINS )->DATA ;
  sig_ext = locon_ext->SIG;

  return sig_ext;
}

int rcx_get_node_base( losig_list *losig ) 
{
  ptype_list *ptl;
  ptl = getptype( losig->USER, RCX_NODE_BASE );
  if( !ptl ) return 0;
  return (int)(long)ptl->DATA;
}

int rcx_set_node_base( losig_list *losig, int index )
{
  ptype_list *ptl;
  ptl = getptype( losig->USER, RCX_NODE_BASE );
  if( ptl ) {
    if (countchain(getrcx(losig)->RCXEXTERNAL)>1) return 0;
    rcx_error( 12, losig, AVT_ERROR );
    losig->USER = delptype( losig->USER, RCX_NODE_BASE );
  }
  losig->USER = addptype( losig->USER, RCX_NODE_BASE, (void*)(long)index );
  return 1;
}

void rcx_clear_node_base( losig_list *losig )
{
  ptype_list *ptl;
  ptl = getptype( losig->USER, RCX_NODE_BASE );
  if( ptl )
    losig->USER = delptype( losig->USER, RCX_NODE_BASE );
}

/******************************************************************************\
Transfert au niveau supérieur les informations sur les drivers.
Si au niveau supérieur il y a deja des informations de driver (cas multi driver
due à la hiérarchie), alors on en prend aucune en compte. Pour indiquer ce cas,
on utilise le flag MULTIDRIVER dans la vue RCX externe.
\******************************************************************************/
void rcx_trsfdriver( lofig_list *currentfig, losig_list *sigext, rcx_list *rcxext, losig_list *sigint, rcx_list *rcxint )
{
  num_list *headnodeup ;
  num_list *headnodedw ;
  num_list *node ;

  if( GETFLAG( rcxext->FLAG, RCXMLTDRIVER ) )
    return ;

  if( ( rcxext->DRIVERSUP || rcxext->DRIVERSDW ) &&
      ( rcxint->DRIVERSUP || rcxint->DRIVERSDW )    ) {
    rcx_cleardriver( sigext );
    SETFLAG( rcxext->FLAG, RCXMLTDRIVER );
  }

  headnodeup = NULL ;
  headnodedw = NULL ;

  if( rcxint->DRIVERSUP ) {
    for( node = rcxint->DRIVERSUP->PNODE ; node ; node = node->NEXT ) 
      headnodeup = addnum( headnodeup, rcx_get_new_num_node( sigint, node->DATA, RCX_LOINS, YES ) ) ;
  }
  
  if( rcxint->DRIVERSDW ) {
    for( node = rcxint->DRIVERSDW->PNODE ; node ; node = node->NEXT ) 
      headnodedw = addnum( headnodedw, rcx_get_new_num_node( sigint, node->DATA, RCX_LOINS, YES ) ) ;
  }

  rcx_setdriver( sigext, headnodeup, headnodedw );
  currentfig = NULL ;
}

/******************************************************************************\
Calcule les nouveaux PNODE d'un locon.
\******************************************************************************/
void rcx_trsfnodelocon( locon_list *locon, char where )
{
  num_list *headup ;
  num_list *headdn ;
  num_list *scannum;
  for( scannum = locon->PNODE ; scannum ; scannum = scannum->NEXT ) {
    scannum->DATA = rcx_get_new_num_node( locon->SIG, 
                                          scannum->DATA,
                                          where,
                                          NO
                                        );
  }

  headup = rcx_getnodebytransition( locon, TRC_SLOPE_UP ) ;
  if( headup && locon->PNODE != headup ) {
    for( scannum = headup ; scannum ; scannum = scannum->NEXT ) 
      scannum->DATA = rcx_get_new_num_node( locon->SIG, 
                                            scannum->DATA,
                                            where,
                                            NO
                                          );
  }
  headdn = rcx_getnodebytransition( locon, TRC_SLOPE_DOWN ) ;
  if( headdn && locon->PNODE != headdn ) {
    for( scannum = headdn ; scannum ; scannum = scannum->NEXT ) 
      scannum->DATA = rcx_get_new_num_node( locon->SIG, 
                                            scannum->DATA,
                                            where,
                                            NO
                                          );
  }
}

/******************************************************************************\
Création d'une vue RCX sur une lofig, qui peut contenir des instances ou des
transistors.
\******************************************************************************/

void rcx_create( currentfig )
lofig_list      *currentfig;
{
  loins_list	*scanins;

  // Ajoute une vue RCX sur tous les losigs.
  rcx_addsignal( currentfig );

  // Récupère les informations correspondant aux instances.
  for( scanins = currentfig->LOINS ; scanins ; scanins=scanins->NEXT )
    rcxparse( scanins, NULL, scanins->FIGNAME, NULL );

  // Ajoute des noms de connecteur sur les locons des transistors.
  rcx_name_locon_trs( currentfig );
  
  // Ajoute les connecteurs externes de la lofig dans le champs EXTERNAL des
  // vues RCX.
  rcx_addfigcon( currentfig );

  // Sélectionne les signaux à sauvegarder dans le fichier RCX.
  rcx_set_lofig_savesig( currentfig );
}

void rcx_name_locon_trs( lofig_list *currentfig )
{
  losig_list    *scansig;
  locon_list    *locon ;
  ptype_list *ptype ;
  chain_list *chain ;

  for( scansig = currentfig->LOSIG ; scansig ; scansig = scansig->NEXT ) {
  
    if(tlc_islosigalim(scansig))
      continue ;

    if((ptype = getptype(scansig->USER,LOFIGCHAIN)) != NULL) {

      for( chain = (chain_list *)ptype->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list *)chain->DATA ;
        if( !rcx_isvalidlocon( locon ) )
          continue;
        if(locon->TYPE == 'T')
          givetransloconrcxname(locon) ;
      }
    }
  }
  givetransloconrcxname(NULL) ;
}

void rcx_addsignal( lofig_list *fig )
{
  losig_list	*scansig;

  for( scansig = fig->LOSIG ; scansig ; scansig = scansig->NEXT )
  {
    if(( scansig->PRCN == NULL ) || tlc_islosigalim(scansig))
      continue ;
    
    rcx_alloc( scansig );
  }
}

void rcx_set_lofig_savesig( lofig_list *fig )
{
  losig_list *scanlosig;
  rcx_list   *rcxview;
  for( scanlosig = fig->LOSIG ; scanlosig ; scanlosig = scanlosig->NEXT ) {
    rcxview = getrcx( scanlosig );
    if( rcxview ) {
      SETFLAG( rcxview->FLAG, SAVESIG );
    }
  }
}

void rcx_addfigcon( lofig_list *fig )
{
  locon_list	*scanlocon;
  losig_list	*sig;
  rcx_list	*rcxview;

  for(scanlocon=fig->LOCON;scanlocon;scanlocon=scanlocon->NEXT)
  {
    sig=scanlocon->SIG;
    rcxview = getrcx( sig );
    
    if( rcxview == NULL)
      continue;

    if( rcx_isvalidlocon( scanlocon ) )
      rcxview->RCXEXTERNAL=addchain(rcxview->RCXEXTERNAL, scanlocon);
  }
}

char rcx_isvalidlocon( locon_list *locon )
{
  char r;
  
  switch( locon->TYPE ) {
  case 'I':
  case 'E':
  case 'T':
    r=1;
    break;
  default:
    r=0;
    break;
  }

  return r;
}

int rcx_calc_cca_param( rcxparam *param )
{
  if( param->CCA < 0.0 ) {
    param->CCA = rcx_getsigcapa( param->LOFIG,
                                 param->SIGNAL,
                                 RCX_SIGCAPA_CTK,
                                 RCX_SIGCAPA_LOCAL|RCX_SIGCAPA_GLOBAL,
                                 RCX_SIGCAPA_NORM,
                                 NULL,
                                 0,
                                 TRC_HALF
                               );
    return 1;
  }
  return 0;
}

int rcx_calc_cca( rcx_slope *slope, losig_list *victime )
{
  if( slope->CCA < 0.0 ) {
    slope->CCA = rcx_getsigcapa( NULL,
                                 victime,
                                 RCX_SIGCAPA_CTK,
                                 RCX_SIGCAPA_LOCAL|RCX_SIGCAPA_GLOBAL,
                                 RCX_SIGCAPA_NORM,
                                 NULL,
                                 0,
                                 TRC_HALF
                               );
    return 1 ;
  }
  return 0;
}

void rcx_cleardriver( losig_list *losig ) 
{
  rcx_list *rcx ;

  rcx = getrcx( losig );
  
  if( rcx ) {
  
    if( rcx->DRIVERSUP ) {
      freercxlocon( rcx->DRIVERSUP );
      rcx->DRIVERSUP = NULL ;
    }

    if( rcx->DRIVERSDW ) {
      freercxlocon( rcx->DRIVERSDW );
      rcx->DRIVERSDW = NULL ;
    }
  }
}

void rcx_setdriver( losig_list *losig, num_list *driversup, num_list *driversdw )
{
  rcx_list *rcx ;

  rcx = getrcx( losig );
  
  if( rcx ) {
    if( driversup ) {
      if( !rcx->DRIVERSUP ) {
        rcx->DRIVERSUP = addrcxlocon();
        rcx->DRIVERSUP->NAME = "multi-driver-up" ;
        rcx->DRIVERSUP->SIG  = losig ;
      }
      if( rcx->DRIVERSUP->PNODE )
        freenum( rcx->DRIVERSUP->PNODE );
      rcx->DRIVERSUP->PNODE = driversup ;
    }
    if( driversdw ) {
      if( !rcx->DRIVERSDW ) {
        rcx->DRIVERSDW = addrcxlocon();
        rcx->DRIVERSDW->NAME = "multi-driver-down" ;
        rcx->DRIVERSDW->SIG  = losig ;
      }
      if( rcx->DRIVERSDW->PNODE )
        freenum( rcx->DRIVERSDW->PNODE );
      rcx->DRIVERSDW->PNODE = driversdw ;
    }
  }
  else {
    freenum( driversup );
    freenum( driversdw );
  }
}
