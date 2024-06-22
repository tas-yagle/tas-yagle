/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : RCX - AWE support.                                          */
/*    Fichier : trc_awe.c                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.107 $
Author   : $Author: fabrice $
Date     : $Date: 2008/03/27 15:16:20 $

*/

//#define AWEDEBUG
#define AWE_NORMALISE

#include "trc.h"

/******************************************************************************\
*                             Code                                             *
\******************************************************************************/

int AWECACHE;
awetabnode_list *AWEHEADTABNODE = NULL ; // Allocation par bloc
char RCX_AWESTRAIGHT=0;
char RCX_STEPTANH=0;
#ifdef AWEDEBUG
int awestatcallinfo=0;
int awestatcallmoment=0;
int awestatcallfilter=0;
int awestatnbheapinfo=0;
#endif
char RCX_USING_AWEMATRIX=RCX_USING_AWEMATRIX_IFNEED;
char RCX_AWE_ONE_PNODE='N';

losig_list *AWE_DEBUG_NET ;
locon_list *AWE_DEBUG_DRIVER ;
locon_list *AWE_DEBUG_RECEIVER ;
char *RCX_PLOT_AWE=NULL;
char *AWE_DUMP_NETNAME ;
char  AWE_DUMP_SLOPE ;
char AWE_FAST_MODE='N';
int AWE_MAX_ITER=1000;

awetabnode* aweallocnodes( void )
{
  awetabnode_list *pt;
  int              i;
  
  if( AWEHEADTABNODE == NULL ) {
    AWEHEADTABNODE = (awetabnode_list*)(mbkalloc( sizeof( awetabnode_list ) * AWESIZETABNODE ) );
    pt = AWEHEADTABNODE ;
    for( i=1 ; i < AWESIZETABNODE ; i++ ) {
      pt->NEXT = pt+1;
      pt++;
    }
    pt->NEXT = NULL;
  }

  pt = AWEHEADTABNODE;
  AWEHEADTABNODE = AWEHEADTABNODE->NEXT;

  /* initialisation des moments et de la capacité */
  for( i=0 ; i < AWE_MAX_MOMENT ; i++ )
    pt->TABNODE.MOMENT[i] = 0.0 ;
  pt->TABNODE.SUMMOMENTCAPA = 0.0;
  
  return((awetabnode*)pt);
}

void aweunallocnodes( awetabnode *tofree )
{
  awetabnode_list *pt;

  pt = (awetabnode_list*)(tofree);

  pt->NEXT    = AWEHEADTABNODE;
  AWEHEADTABNODE = pt;
}

int awe_dw_forbuildnodes( losig_list  *losig,
                          lonode_list *lonode,
                          chain_list  *chwire,
                          ht          *htpack,
                          awetreetrip *data
                        )
{
  chain_list   *headht ;
  rcx_triangle *tr ;
  awetabnode   *tabnode;
  awetabnode   *ltabnode;
  lonode_list  *ptnode ;
  chain_list   *chain ;
  chain_list   *chrep ;
  RCXFLOAT      c ;
  lowire_list  *wire ;
  int           inode ;
  ptype_list   *ptl ;
  
  if( !data->reset ) {
    tabnode = aweallocnodes();
    if( getptype( lonode->USER, AWENODE ) ) {
      awe_error( 0, AVT_INTERNAL );
    }
    lonode->USER = addptype( lonode->USER, AWENODE, tabnode );
  }
  else {
    ptl = getptype( lonode->USER, AWENODE );
    if( !ptl ) {
      awe_error( 1, AVT_INTERNAL );
    }
    tabnode = (awetabnode*)ptl->DATA ;
  }

  c = rcx_getnodecapa( losig, 
                       lonode, 
                       data->extcapa, 
                       data->slope, 
                       data->type, 
                       TRC_HALF,
                       data->coefctc 
                     );
  tabnode->CAPA = c ;
  tabnode->SUMMOMENTCAPA = c ; 

  for( chain = chwire ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)chain->DATA ;
    inode = (wire->NODE1 == lonode->INDEX ? wire->NODE2 : wire->NODE1 );
    ptnode = getlonode( losig, inode );
    ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
    tabnode->SUMMOMENTCAPA = tabnode->SUMMOMENTCAPA + ltabnode->SUMMOMENTCAPA ;
  }

  headht = GetAllHTElems( htpack );

  for( chain = headht ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)((chain_list*)chain->DATA)->DATA ;
    tr = (rcx_triangle*)rcn_getpack( losig, wire ) ;
    
    ltabnode = (awetabnode*)getptype( tr->n1->USER, AWENODE )->DATA;
    tabnode->SUMMOMENTCAPA = tabnode->SUMMOMENTCAPA + ltabnode->SUMMOMENTCAPA ;
    
    ltabnode = (awetabnode*)getptype( tr->n2->USER, AWENODE )->DATA;
    tabnode->SUMMOMENTCAPA = tabnode->SUMMOMENTCAPA + ltabnode->SUMMOMENTCAPA ;

    for( chrep = tr->REPORTED_IN ; chrep ; chrep = chrep->NEXT ) {
      ptnode = (lonode_list*)chrep->DATA ;
      ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
      tabnode->SUMMOMENTCAPA = tabnode->SUMMOMENTCAPA + ltabnode->SUMMOMENTCAPA;
    }

    for( chrep = tr->REPORTED_N1 ; chrep ; chrep = chrep->NEXT ) {
      ptnode = (lonode_list*)chrep->DATA ;
      ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
      tabnode->SUMMOMENTCAPA = tabnode->SUMMOMENTCAPA + ltabnode->SUMMOMENTCAPA;
    }

    for( chrep = tr->REPORTED_N2 ; chrep ; chrep = chrep->NEXT ) {
      ptnode = (lonode_list*)chrep->DATA ;
      ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
      tabnode->SUMMOMENTCAPA = tabnode->SUMMOMENTCAPA + ltabnode->SUMMOMENTCAPA;
    }
  }
  
  freechain( headht );

  return 1 ;
}

void     awebuildnodes( losig_list  *losig, 
                        lonode_list *node, 
                        RCXFLOAT     extcapa,
                        rcx_slope   *slopemiller,
                        char         type,
                        RCXFLOAT     coefctc,
                        char         reset
                      )
{
  awetreetrip infos ;
  lowire_list *wire ;
 
  infos.extcapa = extcapa ;
  infos.slope   = slopemiller ;
  infos.type    = type ;
  infos.coefctc = coefctc ;
  infos.reset   = reset ;

  avt_logenterfunction(LOGTRC,2, "awebuildnodes()" );
  
  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );

  rcn_treetrip( losig, 
                node,
                NULL,
  (int (*)(losig_list*,lonode_list*,chain_list*,ht*,void*))awe_dw_forbuildnodes,
                NULL,
                &infos,
                0
              );
  avt_logexitfunction(LOGTRC,2);
}

int awe_up_formoment( losig_list *losig,
                      lonode_list *lonode,
                      lowire_list *lowire,
                      aweinfmmt   *info
                    )
{
  awetabnode  *moment;
  awetabnode  *prevmoment;
  int          inode ;
  lonode_list *prevnode ;

  if( lowire ) {
    inode = (lowire->NODE1 == lonode->INDEX ) ? lowire->NODE2 : lowire->NODE1 ;
    prevnode = getlonode( losig, inode );
    
    moment     = (awetabnode*)getptype( lonode->USER, AWENODE )->DATA ;
    prevmoment = (awetabnode*)getptype( prevnode->USER, AWENODE )->DATA ;
   
    moment->MOMENT[info->ordre-1] = prevmoment->MOMENT[info->ordre-1] -
                 rcn_get_resi_para( lowire ) / info->k * moment->SUMMOMENTCAPA ;
  }
  return 1 ;
}

int awe_dw_formoment( losig_list  *losig,
                      lonode_list *lonode,
                      chain_list  *chwire,
                      ht          *htpack,
                      aweinfmmt   *info
                    )
{
  awetabnode   *moment ;
  RCXFLOAT      smc = 0.0 ;
  chain_list   *chain ;
  chain_list   *headht ;
  rcx_triangle *tr ;
  awetabnode   *ltabnode ;
  lowire_list  *wire ;
  int           inode ;
  lonode_list  *ptnode ;
  chain_list   *chrep ;
  
  moment = (awetabnode*)getptype( lonode->USER, AWENODE )->DATA ;

  for( chain = chwire ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;

    inode  = (wire->NODE1 == lonode->INDEX ) ? wire->NODE2 : wire->NODE1 ;
    ptnode = getlonode( losig, inode );

    ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA ;
    
    smc = smc + ltabnode->SUMMOMENTCAPA ;
  }

  headht = GetAllHTElems( htpack );

  for( chain = headht ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)((chain_list*)chain->DATA)->DATA ;
    tr = (rcx_triangle*)rcn_getpack( losig, wire ) ;
    
    ltabnode = (awetabnode*)getptype( tr->n1->USER, AWENODE )->DATA;
    smc = smc + ltabnode->SUMMOMENTCAPA ;
    
    ltabnode = (awetabnode*)getptype( tr->n2->USER, AWENODE )->DATA;
    smc = smc + ltabnode->SUMMOMENTCAPA ;

    for( chrep = tr->REPORTED_IN ; chrep ; chrep = chrep->NEXT ) {
      ptnode = (lonode_list*)chrep->DATA ;
      ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
      smc = smc + ltabnode->SUMMOMENTCAPA ;
    }

    for( chrep = tr->REPORTED_N1 ; chrep ; chrep = chrep->NEXT ) {
      ptnode = (lonode_list*)chrep->DATA ;
      ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
      smc = smc + ltabnode->SUMMOMENTCAPA ;
    }

    for( chrep = tr->REPORTED_N2 ; chrep ; chrep = chrep->NEXT ) {
      ptnode = (lonode_list*)chrep->DATA ;
      ltabnode = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA;
      smc = smc + ltabnode->SUMMOMENTCAPA ;
    }
  }

  smc = smc + moment->CAPA * moment->MOMENT[info->ordre-1];

  moment->SUMMOMENTCAPA = smc ;
  
  freechain( headht );

  return 1 ;
}

int awe_packup_formoment( losig_list   *losig,
                          lonode_list  *lonode,
                          rcx_triangle *tr,
                          aweinfmmt   *info
                        )
{
  awetabnode  *moment ;
  awetabnode  *repmoment ;
  awetabnode  *mmtn1 ;
  awetabnode  *mmtn2 ;
  RCXFLOAT     immt ;
  RCXFLOAT     mmc1 ;
  RCXFLOAT     mmc2 ;
  chain_list  *chain ;
  lonode_list *ptnode ;
  
  moment     = (awetabnode*)getptype( lonode->USER, AWENODE )->DATA ;
  
  for( chain = tr->REPORTED_IN ; chain ; chain = chain->NEXT ) {
  
    ptnode = (lonode_list*)chain->DATA ;
  
    repmoment = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA ;
    repmoment->MOMENT[info->ordre-1] = moment->MOMENT[info->ordre-1] ;
  }

  mmtn1 = (awetabnode*)getptype( tr->n1->USER, AWENODE )->DATA ;
  mmtn2 = (awetabnode*)getptype( tr->n2->USER, AWENODE )->DATA ;

  mmc1 = mmtn1->SUMMOMENTCAPA ;
  mmc2 = mmtn2->SUMMOMENTCAPA ;
  
  for( chain = tr->REPORTED_N1 ; chain ; chain = chain->NEXT ) {
    ptnode = (lonode_list*)chain->DATA ;
    repmoment = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA ;
    mmc1 = mmc1 + repmoment->SUMMOMENTCAPA ;
  }
  for( chain = tr->REPORTED_N2 ; chain ; chain = chain->NEXT ) {
    ptnode = (lonode_list*)chain->DATA ;
    repmoment = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA ;
    mmc2 = mmc2 + repmoment->SUMMOMENTCAPA ;
  }

  immt = moment->MOMENT[info->ordre-1] - tr->Z0 / info->k * ( mmc1 + mmc2 ) ;
  
  mmtn1->MOMENT[info->ordre-1] = immt - tr->Z1 / info->k * mmc1 ;
  mmtn2->MOMENT[info->ordre-1] = immt - tr->Z2 / info->k * mmc2 ;

  for( chain = tr->REPORTED_N1 ; chain ; chain = chain->NEXT ) {
  
    ptnode = (lonode_list*)chain->DATA ;
  
    repmoment = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA ;
    repmoment->MOMENT[info->ordre-1] = mmtn1->MOMENT[info->ordre-1] ;
  }

  for( chain = tr->REPORTED_N2 ; chain ; chain = chain->NEXT ) {
  
    ptnode = (lonode_list*)chain->DATA ;
  
    repmoment = (awetabnode*)getptype( ptnode->USER, AWENODE )->DATA ;
    repmoment->MOMENT[info->ordre-1] = mmtn2->MOMENT[info->ordre-1] ;
  }

  losig = NULL ;
  return 1 ;
}

void awemoment( losig_list *losig, 
                lonode_list *node, 
                int ordre,
                RCXFLOAT k,
                RCXFLOAT extcapa
              )
{
  aweinfmmt    infos ;
  lowire_list *wire;

  avt_logenterfunction(LOGTRC,2, "awemoment()" );
  
  infos.ordre   = ordre ;
  infos.extcapa = extcapa ;
  infos.k       = k ;
  
  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );

  rcn_treetrip( losig,
                node,
     (int (*)(losig_list*,lonode_list*,lowire_list*,void*))awe_up_formoment,
     (int (*)(losig_list*,lonode_list*,chain_list*,ht*,void*))awe_dw_formoment,
     (int (*)(losig_list*,lonode_list*,void*,void*))awe_packup_formoment,
                &infos,
                0
              );
  avt_logexitfunction(LOGTRC,2);
}

int awe_dw_forcleannodes( losig_list  *losig, 
                          lonode_list *lonode,
                          chain_list  *chwire,
                          ht          *htpack,
                          void        *userdata
                        )
{
  ptype_list *ptl ;
  ptl = getptype( lonode->USER, AWENODE );
  if( !ptl ) {
    awe_error( 2, AVT_INTERNAL );
  }
  aweunallocnodes( (awetabnode*) ptl->DATA );
  lonode->USER = delptype( lonode->USER, AWENODE );

  userdata = NULL ;
  chwire   = NULL ;
  htpack   = NULL ;
  losig    = NULL ;

  return 1 ;
}

void awecleannodes( losig_list *losig, 
                    lonode_list *node 
                  )
{
  lowire_list *wire ;

  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );
    
  rcn_treetrip( losig, node, NULL, awe_dw_forcleannodes, NULL, NULL, 0 );
}

RCXFLOAT awedelay( aweinfo_list *awe, 
		   RCXFLOAT inputslope,
		   RCXFLOAT vmax,
		   RCXFLOAT vt,
		   RCXFLOAT v,
                   RCXFLOAT vsat,
                   RCXFLOAT rl,
                   RCXFLOAT r,
                   RCXFLOAT c1,
                   RCXFLOAT c2,
                   mbk_pwl  *pwl
		 )
{
  RCXFLOAT a, b;
  RCXFLOAT ts, t;
  double   te ;
  mbk_pwl  *lines;
  mbk_pwl  *tanhpwl;
  mbk_laplace *laplace;
  static int fordebug=0;
  int status;

  avt_logenterfunction(LOGTRC,2, "awedelay()" );
  
  fordebug++;

  if( awe->FLAG == AWE_NODELAY ) {
    avt_log(LOGTRC,2, "no delay\n" );
    avt_logexitfunction(LOGTRC,2);
    return( 0.0 );
  }

  t = 0.0 ;

  if( RCX_AWESTRAIGHT ) {
    avt_log(LOGTRC,2,"old algorithm : rcx_awestraight\n");
    awe_tanh_to_straight( inputslope, vmax, vt, &a, &b );
    ts = aweinstant_straight( awe, v, vmax, a, b );
    te = ( v - b ) / a ;
    t = ts - te ;
  }
  else {
    if( pwl ) {
      avt_log(LOGTRC,2,"using input pwl\n");
      lines = pwl ;
      tanhpwl = NULL ;
    }
    else {
      avt_log(LOGTRC,2,"building pwl from hyperbolic tangent\n");
      tanhpwl = awe_tanh_to_pwl( inputslope, vmax, vt, vsat, rl, r, c1, c2 );
      lines = tanhpwl ;
    }

    if( lines ) {
      laplace = awe_pwl_to_laplace( lines, awe );
      if( laplace ) {
        ts = aweinstant_pwl( awe, v, vmax, laplace, &status );
        if( status ) {
          avt_log(LOGTRC,2,"find ts=%g\n", ts );
          if( mbk_pwl_get_inv_value( lines, v, &te ) ) {
            t = ts - te ;
            if( t<0.0 ) { /* possible car les solutions trouvées pour
                             ts et te sont approximatives. */
              if( -t / ts > 0.01 ) {
                /* on considère que si t représente plus de 1% de ts, c'est
                   qu'il y a un problème d'évaluation de awe */
                avt_errmsg( TRC_ERRMSG, "001", AVT_WARNING, 
                            rcx_getsigname( AWE_DEBUG_NET ) );
              }
              
            }
          }
          else {
            t = ts ;
            avt_errmsg( TRC_ERRMSG, "042", AVT_WARNING, 
                        rcx_getsigname( AWE_DEBUG_NET ) );
          }
        }
        else {
          avt_log(LOGTRC,2,"can't determine output transition time\n");
          avt_errmsg( TRC_ERRMSG, "043", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
        }
        mbk_free_laplace( laplace );
      }
      else {
        avt_log(LOGTRC,2,"Laplace transfrom not found\n");
        avt_errmsg( TRC_ERRMSG, "044", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
      }
      mbk_pwl_free_pwl( tanhpwl );
    }
    else {
      avt_log(LOGTRC,2, "no pwl\n" );
      avt_errmsg( TRC_ERRMSG, "044", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
    }
  }

  if( t < 0.0 ) {
    t = 0.0 ;
  }

  avt_logexitfunction(LOGTRC,2);
  return t ;
  
}

RCXFLOAT aweslope( aweinfo_list *awe, 
		    RCXFLOAT     vmax,
                    mbk_pwl    **pwlout,
                    rcx_slope   *slope,
                    char         type,
                    double       coefctc
		  )
{
  RCXFLOAT t1, t2, v1, v2 ;
  RCXFLOAT a, b, f ;
  mbk_pwl *tanhpwl;
  mbk_pwl *lines;
  mbk_laplace *laplace;
  static int fordebug=0;
  awefilter datapwltanh;
  int status ;
  double capa, capaf, tp2, t20, tp ;
  RCXFLOAT     inputslope ;
  RCXFLOAT     vt ;
  RCXFLOAT     v ;
  RCXFLOAT     vsat ;
  RCXFLOAT     rl ;
  RCXFLOAT     r ;
  RCXFLOAT     c1 ;
  RCXFLOAT     c2 ;
  mbk_pwl     *pwlin ;
  rcx_slope_param *sparam ;
  lonode_list *node;
  chain_list  *headlocon;

  if( type == RCX_MAX ) 
    sparam = &(slope->MAX) ;
  else
    sparam = &(slope->MIN) ;

  inputslope = sparam->slope ;
  vt         = sparam->vt ;
  v          = sparam->vth ;
  vsat       = sparam->vsat ;
  rl         = sparam->rlin ;
  r          = sparam->r ;
  c1         = sparam->c1 ;
  c2         = sparam->c2 ;
  pwlin      = sparam->pwl ;
  
  avt_logenterfunction(LOGTRC,2, "aweslope()" );

  fordebug++;

  if( pwlout )
    *pwlout = NULL ;

  if( awe->FLAG == AWE_NODELAY ) {
    avt_log(LOGTRC,2, "call aweslope() with AWE_NODELAY filter.\n" );
    avt_logexitfunction(LOGTRC,2);
    return( inputslope );
  }

  // Le calcul s'effectue à partir de la tangente à la tension v, en prenant
  // deux mesures de la tension séparée de 5%.

  if( RCX_AWESTRAIGHT ) {

    avt_log(LOGTRC,2, "old algorithm used : RCX_AWESTRAIGHT\n" );
    awe_tanh_to_straight( inputslope, vmax, vt, &a, &b );
  
    v1 = v;
    t1 = aweinstant_straight( awe, v1, vmax, a, b );
    if( t1 < 0.0 ) {
      avt_log(LOGTRC,2, "error when computing t1\n" );
      avt_logexitfunction(LOGTRC,2);
      return( inputslope );
    }
  
    t2 = t1*0.95;
    v2 = awevoltage_straight( awe, t2, vmax, a, b );
    if( v2 < 0.0 ) {
      avt_log(LOGTRC,2, "error when computing v2\n" );
      avt_logexitfunction(LOGTRC,2);
      return( inputslope );
    }

    // La dérivée
    a=(v2-v1)/(t2-t1);

    // Le front calculé à partir de la dérivée. Formule semblabe à celle
    // décrite dans la thèse d'Amjad Hajjar pIII/9 (3-3-a et 3-3-b), généralisée
    // à n'importe quelle tension de seuil.

    f = (vmax-v)*(vmax+v-2.0*vt)/(a*(vmax-vt));
  }
  else {
    if( pwlin ) {
      avt_log(LOGTRC,2, "using input pwl\n" );
      lines = pwlin ;
      tanhpwl = NULL ;
    }
    else {
      avt_log(LOGTRC,2, "building pwl from hyperbolic tangent\n" );
      tanhpwl = awe_tanh_to_pwl( inputslope, vmax, vt, vsat, rl, r, c1, c2 );
      lines = tanhpwl ;
    }
    if( !lines ) {
      avt_log(LOGTRC,2, "no pwl\n" );
      avt_logexitfunction(LOGTRC,2);
      return inputslope;
    }

    laplace = awe_pwl_to_laplace( lines, awe );
    if( !laplace ) {
      mbk_pwl_free_pwl( tanhpwl );
      avt_log(LOGTRC,2,"can't build Laplace transfom\n" );
      avt_logexitfunction(LOGTRC,2);
      return inputslope;
    }
      
    awe_tanh_point_measure( vt, vmax, inputslope, &t1, &v1, &t2, &v2 );

    t1 = aweinstant_pwl( awe, v1, vmax, laplace, &status );
    if( !status ) {
      avt_errmsg( TRC_ERRMSG, "043", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
      mbk_free_laplace( laplace );
      mbk_pwl_free_pwl( tanhpwl );
      avt_log(LOGTRC,2,"can't find t1\n");
      avt_logexitfunction(LOGTRC,2);
      return inputslope;
    }

    t2 = aweinstant_pwl( awe, v2, vmax, laplace, &status );
    if( !status ) {
      avt_errmsg( TRC_ERRMSG, "043", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
      mbk_free_laplace( laplace );
      mbk_pwl_free_pwl( tanhpwl );
      avt_log(LOGTRC,2,"can't find t2\n");
      avt_logexitfunction(LOGTRC,2);
      return inputslope;
    }

    if( V_BOOL_TAB[ __TAS_USE_FINAL_CAPACITANCE ].VALUE ) {
      node = getlonode( awe->LOCON_LOAD->SIG, awe->NODE_LOAD );
      /* critère  : un seul locon et une seul resistance, r>10k */
      headlocon = getloconnode( node );
      if( headlocon && !headlocon->NEXT ) {
        if( node->WIRELIST && !node->WIRELIST->NEXT && ((lowire_list*)(node->WIRELIST->DATA))->RESI > 10000 ) {
          if (mbk_pwl_get_inv_value( lines, v2, &t20 )) {
            tp = t2-t20 ;
            if( tp > 0.0 ) {
              capa  = rcx_getnodecapa( awe->LOCON_LOAD->SIG,
                                       getlonode( awe->LOCON_LOAD->SIG, awe->NODE_LOAD ),
                                       awe->EXTCAPA,
                                       slope,
                                       type,
                                       TRC_HALF,
                                       coefctc
                                     );
              capaf = rcx_getnodecapa( awe->LOCON_LOAD->SIG,
                                       getlonode( awe->LOCON_LOAD->SIG, awe->NODE_LOAD ),
                                       awe->EXTCAPA,
                                       slope,
                                       type,
                                       TRC_END,
                                       coefctc
                                     );
              tp2 = tp*capaf/capa ;
              t2 = t20+tp2 ;
            }
          }
          else {
            avt_log(LOGTRC,2, "error when computing v2 crossing time\n" );
          }
        }
      }
    }
    
    f = (t2-t1)/(atanh((v2-vt)/(vmax-vt)-atanh((v1-vt)/(vmax-vt))));

    avt_log(LOGTRC,2, "slope computed : %g\n" ,f );

    if( AWE_DUMP_NETNAME ) {
      char name[1024];
      sprintf( name, "rc_%s_%s", AWE_DUMP_NETNAME, AWE_DUMP_SLOPE==TRC_SLOPE_UP ? "up":"dw" );
      awevoltage_pwl_plot( awe, 2.0*t2, lines, laplace, 1.0e-12, 0.0, vmax, name );
    }

    if( pwlout ) {
      avt_log(LOGTRC,2,"build pwlout\n");
      datapwltanh.LAPLACE = laplace ;
      datapwltanh.AWE     = awe ;
      datapwltanh.VMAX    = vmax ;
      *pwlout = mbk_create_pwl_according_tanh( (char (*)(void*, float, float*))awe_get_time_for_pwl, 
                                               (void*)&datapwltanh, 
                                               vt , 
                                               0.9*vmax 
                                             );
    }
    
    mbk_free_laplace( laplace );
    mbk_pwl_free_pwl( tanhpwl );
    
    if( f < inputslope ) {
      avt_log(LOGTRC,2,"an error occured. try the simplest model for driver\n" );
      lines = awe_tanh_to_pwl( inputslope, vmax, vt, 
                               -1.0, -1.0, -1.0, -1.0, -1.0 );
      if( !lines ) {
        avt_logexitfunction(LOGTRC,2);
        return inputslope;
      }

      laplace = awe_pwl_to_laplace( lines, awe );
      if( !laplace ) {
        mbk_pwl_free_pwl( lines );
        avt_logexitfunction(LOGTRC,2);
        return inputslope;
      }
        
      awe_tanh_point_measure( vt, vmax, inputslope, &t1, &v1, &t2, &v2 );

      t1 = aweinstant_pwl( awe, v1, vmax, laplace, &status );
      if( !status ) {
        avt_errmsg( TRC_ERRMSG, "043", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
        mbk_free_laplace( laplace );
        mbk_pwl_free_pwl( lines );
        avt_log(LOGTRC,2,"can't find t1\n");
        avt_logexitfunction(LOGTRC,2);
        return inputslope;
      }

      t2 = aweinstant_pwl( awe, v2, vmax, laplace, &status );
      mbk_free_laplace( laplace );
      mbk_pwl_free_pwl( lines );
      if( !status ) {
        avt_errmsg( TRC_ERRMSG, "043", AVT_WARNING, rcx_getsigname( AWE_DEBUG_NET ) );
        avt_log(LOGTRC,2,"can't find t2\n");
        avt_logexitfunction(LOGTRC,2);
        return inputslope ;
      }

      f = (t2-t1)/(atanh((v2-vt)/(vmax-vt)-atanh((v1-vt)/(vmax-vt))));
      avt_log(LOGTRC,2, "slope computed : %g\n" ,f );
    }
  }
 
  avt_logexitfunction(LOGTRC,2);
  return( f );
}

char awe_get_time_for_pwl( awefilter *filter, float v, float *t )
{
  int status ;
  *t = aweinstant_pwl( filter->AWE, v, filter->VMAX, filter->LAPLACE, &status );
  return status;
}

void awe_tanh_point_measure( RCXFLOAT vt, 
                             RCXFLOAT vmax, 
                             RCXFLOAT f,
                             RCXFLOAT *t1,
                             RCXFLOAT *v1, 
                             RCXFLOAT *t2,
                             RCXFLOAT *v2
                           )
{
  *v1=vt;
  *t1=0.0;

  *v2=3.0*(vmax-vt)/4.0+vt;
  *t2=f*atanh( (*v2-vt)/(vmax-vt) );
}

RCXFLOAT awe_pi_get_v20( RCXFLOAT tsat, RCXFLOAT inputslope, RCXFLOAT vmax, RCXFLOAT vt, RCXFLOAT r, RCXFLOAT c2 )
{
  mbk_pwl       *lines ;
  RCXFLOAT       v20 ;
  RCXFLOAT       tr ;
  int            i ;
  mbk_laplace   *laplace ;
  RCXFLOAT       a ;

  v20 = -1.0 ;
  
  lines = awe_tanh_to_pwl( inputslope, vmax, vt, -1.0, -1.0, -1.0, -1.0, -1.0 );
  
  if( lines ) {
  
    laplace = mbk_pwl_to_laplace( lines, sizeof( mbk_laplace_data ) );
    
    if( laplace ) {

      v20 = 0.0 ;
      
      for( i = 0 ; 
           i < laplace->N && tsat > ((mbk_laplace_data*)laplace->DATA)[i].T ; 
           i++ ) 
      {
        tr = tsat - ((mbk_laplace_data*)laplace->DATA)[i].T ;
        a = ((mbk_laplace_data*)laplace->DATA)[i].A ;
        v20=v20+a*(tr-r*c2*(1.0-exp(-tr/(r*c2))));
      }

      mbk_free_laplace( laplace );
    }
    mbk_pwl_free_pwl( lines );
  }
  
  return v20 ;
}

/* Convertie une tangente hyperbolique en un ensemble de droites */
mbk_pwl* awe_tanh_to_pwl( RCXFLOAT inputslope,
                          RCXFLOAT vmax,
                          RCXFLOAT vt,
                          RCXFLOAT vsat,
                          RCXFLOAT rl,
                          RCXFLOAT r,
                          RCXFLOAT c1,
                          RCXFLOAT c2
                        )
{
  mbk_pwl       *lines ;
  awe_tanh_data  data ;
  mbk_pwl_param *param;
  RCXFLOAT       t1, v1, t2, v2 ;
  RCXFLOAT       delta ;
  RCXFLOAT       v20 ;
  RCXFLOAT       tsat ;
  double         tmax ;
  char           piload ;
  RCXFLOAT       dxmin ;
  RCXFLOAT       sdmin ;
  static int     fordebug=0;

  fordebug++;
  
  data.F     = inputslope ;
  data.VT    = vt ;
  data.VMAX  = vmax ;
  data.A     = ( vmax - vt ) / inputslope ;
  
  data.TSAT  = FLT_MAX ;

  piload = 0 ;

  if( vsat > 0.0 && rl>0.0 && r>0.0 && c1>0.0 && c2>0.0 ) {
  
    delta = r*r*c2*c2 + rl*rl*(c1+c2)*(c1+c2) - 2*r*rl*c2*(c1-c2) ;
    tsat  = inputslope * atanh( ( vsat - vt ) / ( vmax - vt ) ) ;
    v20   = awe_pi_get_v20( tsat, inputslope, vmax, vt, r, c2 );
    
    if( delta > 0.0 && v20 > 0.0 ) {
      piload     = 1 ;
      data.RLIN  = rl ;
      data.VSAT  = vsat ;
      data.TSAT  = tsat ;
      data.P1    = (-r*c2-rl*(c1+c2)-sqrt(delta))/(2.0*r*rl*c1*c2) ;
      data.P2    = (-r*c2-rl*(c1+c2)+sqrt(delta))/(2.0*r*rl*c1*c2) ;
      data.V20   = v20 ;
      data.V10   = vsat ;
      data.C1    = c1 ;
      data.C2    = c2 ;
      data.R     = r ;
      data.RL    = rl ;
    }
  }

  // Calcule l'instant td<0 de départ de la tanh en supposant qu'entre td et 0
  // le front est une droite dont la pente vaut la dérivée de la tanh en t=0
  if( RCX_STEPTANH )
    data.TR = 0.0 ;
  else
    data.TR   = - vt / data.A ;

  tmax = awe_tanh_tmax( &data );
  
  param = mbk_pwl_get_default_param( (char (*)(void*, double, double*))awe_tanh,
                                     (void*) &data,
                                     data.TR, 
                                     tmax
                                   );

  /* Demande une bonne modélisation de la partie correspondant à la tanh */
  dxmin = data.F/5.0 ;
  sdmin = data.F/100.0 ;
  if( param->PARAM.DEFAULT.DELTAXMIN > dxmin )
    param->PARAM.DEFAULT.DELTAXMIN = dxmin ;
  if( param->PARAM.DEFAULT.SDERIVE > sdmin )
    param->PARAM.DEFAULT.SDERIVE = sdmin ;
  if( piload ) {
     t1 = 0.7*r*c2 ;
     if( t1 > 2.0*data.F )
      mbk_pwl_add_param_point( param, t1 );
  }

  awe_tanh_point_measure( vt, vmax, inputslope, &t1, &v1, &t2, &v2 );
  if( piload )
    mbk_pwl_add_param_point( param, tsat );
  
  if( !RCX_STEPTANH )
    mbk_pwl_add_param_point( param, t1 );
  mbk_pwl_add_param_point( param, t2 );  
                                     
  lines = mbk_pwl_create( (char (*)(void*, double, double*))awe_tanh, 
                          (void*) &data, 
                          data.TR, 
                          tmax,
                          param 
                        );

  mbk_pwl_free_param( param );
  return lines;
  
}

/* Renvoie l'instant à partir duquel le front est considéré comme terminé */
double awe_tanh_tmax( awe_tanh_data *data )
{
  double t ;
  double v ;
  double vmax ;
  
  t = data->F ;
  vmax = 0.99 * data->VMAX ;
  
  do {
    t = 3.0 * t ;

    if( !awe_tanh( data, t, &v ) )
      return t;
  }
  while( v < vmax );

  return t ;
}

/* Fonction locale de tangente hyperbolique */
char awe_tanh( awe_tanh_data *data, double t, double *v )
{
  double r, c, vmin ;

  if( t >= 0.0 ) 
    if( t <= data->TSAT )
      *v = data->VT + (data->VMAX - data->VT) * tanh( t/data->F );
    else {
    
      t = t - data->TSAT ;
      /* pour eviter une diminution de la tension lorsqu'on passe du
         générateur modélisé par une tension à une résistance linéaire */
      r = data->R  + data->RL ;
      c = data->C1 + data->C2 ;
      vmin = data->VMAX*(1.0-exp(-t/(r*c))) +
             data->V10*exp(-t/(r*c)) ;
           
      *v =   ( data->C1*data->V10 + data->C2*data->V20 + 
               data->VMAX*data->C2*data->R/data->RL )
           / ( data->R*data->C1*data->C2) 
           * ( exp(data->P1*t) - exp(data->P2*t ))
           / (data->P1-data->P2) ;
      *v = *v + data->V10 
           * ( data->P2*exp(data->P2*t) - data->P1*exp(data->P1*t) )
           / ( data->P2 - data->P1 ) ;
      *v = *v - data->VMAX / ( data->R * data->RL * data->C1 * data->C2 )
           * ( data->P1*exp(data->P2*t) - data->P2*exp(data->P1*t)
               + ( data->P2 - data->P1 ) )
           / ( data->P1 * data->P2 * ( data->P1 - data->P2 )) ;
      if( *v < vmin ) {
        *v = vmin ;
      }
    }
  else {
    if( t >= data->TR )
      *v = data->VT + t * data->A ;
    else
      *v = 0.0 ;
  }
  return 1;
}

void plot_awe_tanh( awe_tanh_data *data, float t_unit, float tmax, char *name )
{
  FILE *ptf ;
  double t, v ;
  ptf = mbkfopen( name, "dat", "w" );
  for( t=0.0 ; t<tmax ; t=t+0.01 ) {
    awe_tanh( data, t, &v );
    fprintf( ptf, "%g %g\n", t*t_unit, v );
  }
  fclose( ptf );
}

/* Converti une tangente hyperbolique en une droite */

void awe_tanh_to_straight( RCXFLOAT inputslope,
                           RCXFLOAT vmax,
                           RCXFLOAT vt,
                           RCXFLOAT *a,
                           RCXFLOAT *b
                         )
{
  RCXFLOAT v1, v2, t1, t2;

  v1 = 0.2*vmax ;
  v2 = 0.8*vmax ;
  t1 = INVTH( inputslope, vt, vmax, v1 );
  t2 = INVTH( inputslope, vt, vmax, v2 );

  *a = (v1-v2)/(t1-t2);
  *b = v1-t1*(*a) ;
}

void awevoltage_pwl_plot( aweinfo_list *awe,
                          RCXFLOAT      tmax,
                          mbk_pwl      *lines,
                          mbk_laplace  *laplace,
                          float         t_unit,
                          float         t_vth,
                          float         vmax,
                          char         *filename
                        )
{
  FILE *file;
  double ts;
  double t;
  double t0;
  double ve;
  double lve;
  double vs;
  double dt;
  
  file = mbkfopen( filename, "dat", "w" );
  if( !file ) {
    printf( "Can't open file.\n" );
    return;
  }
  
  ts = tmax/1000.0 ;

  t0 = aweinstant_pwl( awe, vmax/2.0, vmax, laplace, NULL );
  dt = t_vth - t0 ;
 
  lve=0.0;
  for( t=-lines->DATA[0].B/lines->DATA[0].A ; t<tmax ; t=t+ts ) {
    switch( mbk_pwl_get_value( lines, t, &ve ) ) {
    case MBK_PWL_FOUND :
      lve = ve ;
      break;
    case MBK_PWL_ERROR :
    case MBK_PWL_EXTPL :
      ve = lve ;
      break ;
    }
    vs = awevoltage_pwl( awe, t, laplace );  
    if( AWE_DUMP_SLOPE == TRC_SLOPE_UP )
      fprintf( file, "%g %g %g\n", t*t_unit, ve, vs );
    else
      fprintf( file, "%g %g %g\n", t*t_unit, vmax-ve, vmax-vs );
  }

  fclose( file );
}
  
mbk_laplace* awe_pwl_to_laplace( mbk_pwl *lines, aweinfo_list *awe )
{
  mbk_laplace *l;
  int i;
 
  avt_logenterfunction(LOGTRC,2,"awe_pwl_to_laplace");
  if( awe->FLAG == AWE_MOMENT )
    awemomentfiltre( awe );

  if( awe->FLAG != AWE_FILTER ) {
    avt_logexitfunction(LOGTRC,2);
    return( NULL );
  }

  l = mbk_pwl_to_laplace( lines, sizeof( mbk_laplace_data ) );

  // Normalise les pentes et les délais.
  for( i = 0 ; i < l->N ; i++ ) {
    ((mbk_laplace_data*)l->DATA)[i].A = 
                                    ((mbk_laplace_data*)l->DATA)[i].A * awe->K ;
    ((mbk_laplace_data*)l->DATA)[i].T = 
                                    ((mbk_laplace_data*)l->DATA)[i].T / awe->K ;
  }
  avt_logexitfunction(LOGTRC,2);

  return l;
}

RCXFLOAT awevoltage_fast( aweinfo_list *awe, double f )
{
  double v ;
  double k ;
  double p ;
  double vmax ;
  double vtotal ;
  int    l ;
  static int fordebug=0 ;

  avt_logenterfunction(LOGTRC,2, "awevoltage_fast()" );

  fordebug++;
 
  if( AWE_FAST_MODE == 'N' ) {
    avt_log(LOGTRC,2, "AWE_FAST_MODE disable\n" );
    avt_logexitfunction(LOGTRC,2);
    return -1.0 ;
  }

  if( awe->FLAG == AWE_NODELAY ) {
    avt_log(LOGTRC,2, "nodelay\n" );
    avt_logexitfunction(LOGTRC,2);
    return -1.0 ;
  }

  if( awe->FLAG == AWE_MOMENT )
    awemomentfiltre( awe );

  if( awe->FLAG != AWE_FILTER ) {
    avt_log(LOGTRC,2,"no filter\n");
    avt_logexitfunction(LOGTRC,2);
    return -1.0;
  }

  vtotal = 0.0 ;
  vmax   = 0.0 ;
 
  v = 0.0 ;
  
  for( l=0 ; l<awe->DATA.FILTER.ORDER ; l++ ) {

    k = awe->DATA.FILTER.RESIDU[l] ;
    p = -awe->DATA.FILTER.POLE[l] ;

    v = v + k/p/p ;
  }
  v = v * awe->K;

  if( v > f/10.0 ) {
    avt_logexitfunction(LOGTRC,2);
    return -1 ;
  }

  /* Une bonne approximation du temps de propagation du réseau RC est
     cet écart. */

  avt_log(LOGTRC,2, "approx value : %g\n", v );
  avt_logexitfunction(LOGTRC,2);
  return v ;

}

RCXFLOAT awevoltage_pwl( aweinfo_list *awe,
                         RCXFLOAT      t,
                         mbk_laplace  *laplace
                       )
{
  int    i, l ;
  double p, k, v, tr ;
  
  t = t / awe->K ;

  // Calcule la réponse temporelle.
 
  v = 0.0 ;
  for( i = 0 ; i < laplace->N && t > ((mbk_laplace_data*)laplace->DATA)[i].T ; i++ ) {
  
    tr = t - ((mbk_laplace_data*)laplace->DATA)[i].T ;
      
    for( l=0 ; l<awe->DATA.FILTER.ORDER ; l++ ) {

      k = awe->DATA.FILTER.RESIDU[l] ;
      p = awe->DATA.FILTER.POLE[l] ;
       
      v = v + (((mbk_laplace_data*)laplace->DATA)[i].A) * k / p * ( -tr - 1.0/p + exp( tr*p )/p ) ;
      
      if( i == 0 ) 
        v = v - laplace->B * k/p*( 1.0 - exp( tr*p ) ) ;
    }
  }

  return v;
}

RCXFLOAT aweinstant_pwl( aweinfo_list *awe,
                         RCXFLOAT      vf,
                         RCXFLOAT      vmax,
                         mbk_laplace  *laplace,
                         int          *status
                       )
{
  RCXFLOAT t1, t2, lt1, lt2, tp, v, v1, v2 ;
  int n;
  static int fordebug=0;
  fordebug++;

  avt_logenterfunction(LOGTRC,2, "aweinstant_pwl()" );
  if( status ) *status = 0 ;

  if( awe->FLAG == AWE_NODELAY )
  {
    avt_log(LOGTRC,2, "aweinstant_pwl() : call with AWE_NODELAY filter.\n" );
    avt_logexitfunction(LOGTRC,2);
    return( 0.0 );
  }

  /* Calcul du tp par dichotomie */
  t1  = 0.0 ; 
  t2  = 2*awe->K;
 
  n = AWE_MAX_ITER ;
  do { // On vérifie que la solution recherchée est entre t1 et v2.
    
    v = awevoltage_pwl( awe, t2, laplace );
    
    if( v<vf ) {
      t1=t2;
      t2+=t2;
    }

    n--;
  }
  while( v < vf && n );

  if( !n ) {
    avt_log(LOGTRC,2, "aweinstant_pwl() : can't solve #1\n" );
    avt_logexitfunction(LOGTRC,2);
    return( 0.0 );
  }
  
  v1 = awevoltage_pwl( awe, t1, laplace );
  v2 = v;
  
  n = AWE_MAX_ITER ;
  do
  {
    // tp = (t2+t1)/2.0;
    tp = (t2-t1)/(v2-v1)*(vf-v1)+t1;
    lt1=t1;
    lt2=t2;

    v = awevoltage_pwl( awe, tp, laplace );
   
    if( v == vf ) { // ça arrive !
      t1 = tp;
      t2 = tp;
      v1 = v;
      v2 = v;
    } else {
      if( v > vf ) {
        t2 = tp;
        v2 = v;
      }
      else {
        t1 = tp;
        v1 = v;
      }  
    }

    n--;
  }
  while( fabs(v-vf)>vmax/1000.0 && n );
 
  if( !n ) {
    avt_log(LOGTRC,2, "aweinstant_pwl() : can't solve #2\n" );
    avt_logexitfunction(LOGTRC,2);
    return( 0.0 );
  }
  
  if( status ) *status = 1 ;
  avt_logexitfunction(LOGTRC,2);
  return( tp );
}
                        
/* Tension à l'instant T lorsque l'entrée est une droite v(t)=a.t+b
  jusqu'à vmax et constant après */
  
RCXFLOAT awevoltage_straight( aweinfo_list *awe, 
		               RCXFLOAT t,
		               RCXFLOAT vmax, 
		               RCXFLOAT a,
                               RCXFLOAT b
		             )
{
  int       l;
  RCXFLOAT v, k, p, tm;

  if( awe->FLAG == AWE_MOMENT )
    awemomentfiltre( awe );

  if( awe->FLAG != AWE_FILTER )
    return( -1.0 );

  a = a * awe->K ;
  t = t / awe->K ;
  v=0.0;
  tm = (vmax-b)/a ;

  for( l=0 ; l<awe->DATA.FILTER.ORDER ; l++ ) {

    k = awe->DATA.FILTER.RESIDU[l] ;
    p = awe->DATA.FILTER.POLE[l] ;
    
    v = v + a*k*( -t/p - 1.0/p/p*(1.0-exp(p*t)) ) - b*k/p*(1.0-exp(p*t)) ;
    if( t > tm )
      v = v - (a*k*(-(t-tm)/p-1.0/p/p*(1.0-exp(p*(t-tm)))));

  }
  return( v );
}

/* Calcul par dichotomie l'instant de passage de la tension
   par vf. L'entrée est une droite */
   
RCXFLOAT aweinstant_straight( aweinfo_list *awe,
                               RCXFLOAT vf,
                               RCXFLOAT vmax,
                               RCXFLOAT a,
                               RCXFLOAT b
                             )
{
  RCXFLOAT t1, t2, lt1, lt2, tp, v, v1, v2 ;
 
  if( awe->FLAG == AWE_NODELAY )
  {
    avt_log(LOGTRC,2, "awe : call aweinstant_straight() with AWE_NODELAY filter.\n" 
           );
    return( -1.0 );
  }

  /* Calcul du tp par dichotomie */
  t1  = 0.0 ; 
  t2  = 2*awe->K;
  
  do { // On vérifie que la solution recherchée est entre t1 et v2.
    
    v = awevoltage_straight( awe, t2, vmax, a, b );
    if( v < 0.0 )
      return( -1.0 );
    
    if( v<vf ) {
      t1=t2;
      t2+=t2;
    }
  }
  while( v < vf );

  v1 = awevoltage_straight( awe, t1, vmax, a, b );
  v2 = v;
  
  do
  {
    // tp = (t2+t1)/2.0;
    tp = (t2-t1)/(v2-v1)*(vf-v1)+t1;
    lt1=t1;
    lt2=t2;

    v = awevoltage_straight( awe, tp, vmax, a, b );
    if( v < 0.0 )
      return( -1.0 );
   
    if( v == vf ) { // ça arrive !
      t1 = tp;
      t2 = tp;
      v1 = v;
      v2 = v;
    } else {
      if( v > vf ) {
        t2 = tp;
        v2 = v;
      }
      else {
        t1 = tp;
        v1 = v;
      }  
    }
  }
  while( fabs(v-vf)>vmax/1000.0 );
 
  return( tp );
}

/* Fonction utilisée pour normaliser les délais. Renvoie le plus grand moment de 1° ordre 
 * sur le reseau ( ce nombre est <0 )
 */
int awe_dw_formaxmoment( losig_list  *losig, 
                         lonode_list *lonode,
                         chain_list  *chwire,
                         ht          *htpack,
                         RCXFLOAT    *ptmax
                       )
{
  ptype_list *ptl ;
  awetabnode *tabnode ;
  RCXFLOAT    moment ;

  ptl = getptype( lonode->USER, AWENODE );
  if( !ptl ) {
    awe_error( 3, AVT_INTERNAL );
  }
  tabnode = (awetabnode*)ptl->DATA ;
  moment = tabnode->MOMENT[0] ;

  if( moment < *ptmax )
    *ptmax = moment ;

  chwire   = NULL ;
  htpack   = NULL ;
  losig    = NULL ;
  
  return 1 ;
}

RCXFLOAT awemaxmoment( losig_list *losig, lonode_list *node )
{
  RCXFLOAT     max ;
  lowire_list *wire ;

  max = FLT_MAX ;

  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );
    
  rcn_treetrip( losig,
                node,
                NULL,
   (int (*)(losig_list*,lonode_list*,chain_list*,ht*,void*))awe_dw_formaxmoment,
                NULL,
                (void*)&max,
                0
              );
  return max ;
}

// Renvoie le temps de propagation d'un réseau RC calculé avec AWE à l'ordre
// 3. Comme le calcul des moments est assez long, on ne le fait qu'une fois pour toute
// pour un émetteur donné, et les moments de tous les récepteurs sont mémorisés.
// L'évaluation des poles et résidus étant également couteuse, on ne la fait 
// qu'a la demande.
// Il faut qu'à tous les appels le réseau et extcapa soient identiques. Sinon, il faut appeler
// avant la fonction awereset().
// Renvoie 1 en cas de succès, 0 sinon.

int getawedelay( lofig_list *lofig,       // La figure
                 losig_list *losig,       // Le signal
                 locon_list *locon_emt,   // L'emetteur
                 locon_list *locon_rec,   // Le recepteur
                 rcx_slope  *slopemiller, // Front d'entrée pour miller
                 RCXFLOAT    vmaxmax,     // Tension max pour delai max
                 RCXFLOAT    vmaxmin,     // Tension max pour delai min
                 RCXFLOAT    extcapa,     // La capacité externe
                 RCXFLOAT    *dmax,       // Le délai maximum calculé
                 RCXFLOAT    *dmin,       //          minimum
                 RCXFLOAT    *fmax,       // Le front maximum calculé
                 RCXFLOAT    *fmin,       //          minimum
                 mbk_pwl     **pwloutmax,
                 mbk_pwl     **pwloutmin
               )
{
  aweinfo_list     *awe;
  RCXFLOAT          v;
  RCXFLOAT          d;
  RCXFLOAT          groundcapa = -1e6 ;
  RCXFLOAT          coefctc ;
  char              flagmin=0, flagmax=0;
  int               ctkactive;
  mbk_pwl          *pwl;
  char              computedfast ;
  chain_list       *headawe;
  chain_list       *scanawe;

  avt_logenterfunction(LOGTRC,2,"getawedelay()");
  avt_log(LOGTRC,2, "vmaxmax=%g vmaxmin=%g\n", vmaxmax, vmaxmin );

  if( pwloutmax )
    *pwloutmax = NULL ;
  if( pwloutmin )
    *pwloutmin = NULL ;

  if( RCX_PLOT_AWE ) {
    if( rcx_getsigname( losig ) == RCX_PLOT_AWE ) {
      AWE_DUMP_NETNAME = RCX_PLOT_AWE ;
      AWE_DUMP_SLOPE = slopemiller->SENS ;
    }
  }
  else {
    AWE_DUMP_NETNAME = NULL ;
  }
  
  AWE_DEBUG_NET      = losig ;
  AWE_DEBUG_DRIVER   = locon_emt ;
  AWE_DEBUG_RECEIVER = locon_rec ;

  if( slopemiller->MIN.slope < 1.0 )
    slopemiller->MIN.slope = 1.0;
  if( slopemiller->MAX.slope < 1.0 )
    slopemiller->MAX.slope = 1.0;

  ctkactive = rcx_crosstalkactive( RCX_QUERY );

  if( fmin ) *fmin = -1.0 ;
  if( fmax ) *fmax = -1.0 ;
  if( dmin ) *dmin = -1.0 ;
  if( dmax ) *dmax = -1.0 ;

    
  if( fmin || dmin ) {
    avt_log(LOGTRC,2, "computing minimum delays\n" );

    coefctc = 
     rcx_getcoefctc( lofig, losig, slopemiller, RCX_MIN, extcapa, &groundcapa );

    headawe = getaweinfo( losig, 
                          locon_rec, 
                          locon_emt, 
                          extcapa, 
                          slopemiller,
                          RCX_MIN, 
                          coefctc
                        );

    for( scanawe = headawe ; scanawe ; scanawe = scanawe->NEXT ) {
      awe = (aweinfo_list*)scanawe->DATA ;
      if( awe->FLAG == AWE_NODELAY ) 
        continue ;
      
      flagmin=1;

      v = -1.0 ; // un nombre qui indique qu'on a pas de valeur calculée.
      computedfast = 'n' ;

      if( dmin ) {

        d = awevoltage_fast( awe, slopemiller->MIN.slope );
        computedfast = 'y' ;
        
        if( d > 0.0 )
          v = d ;
        else
          v = awedelay( awe, 
                        slopemiller->MIN.slope, 
                        vmaxmin, 
                        slopemiller->MIN.vt, 
                        slopemiller->MIN.vth, 
                        slopemiller->MIN.vsat, 
                        slopemiller->MIN.rlin, 
                        slopemiller->MIN.r, 
                        slopemiller->MIN.c1, 
                        slopemiller->MIN.c2,
                        slopemiller->MIN.pwl
                      );
        
        if( v >= 0.0 ) {
          if( *dmin < 0.0 || v < *dmin )
            *dmin = v;
        }
      }

      v = -1.0 ;
        
      if( fmin ) {
      
        if( computedfast == 'n' )
          d = awevoltage_fast( awe, slopemiller->MIN.slope );

        pwl = NULL ;

        if( d > 0.0 ) {
          if( pwloutmin ) {
            if( slopemiller->MIN.pwl )
              pwl = mbk_pwl_duplicate( slopemiller->MIN.pwl );
          }
          v = slopemiller->MIN.slope ;
        }
        else 
          v = aweslope( awe, 
                        vmaxmin, 
                        pwloutmin ? &pwl : NULL,
                        slopemiller,
                        RCX_MIN,
                        coefctc
                      );

        if( v >= 0.0 ) {
          if( *fmin < 0.0 || v < *fmin ) {
            *fmin = v;
            if( pwloutmin ) {
              if( *pwloutmin ) mbk_pwl_free_pwl( *pwloutmin );
              *pwloutmin = pwl ;
            }
          }
          else
            mbk_pwl_free_pwl( pwl );
        }
        else {
          mbk_pwl_free_pwl( pwl );
        }
      }
    }
   
    awefreeinfolist( headawe );
    // Pas de connexion entre les deux locons
    if( !flagmin ) {
      avt_log(LOGTRC,2, "can't compute delays for min\n" );
      if( fmin ) *fmin=slopemiller->MIN.slope;
      if( dmin ) *dmin=0.0;
    }
  }

  if( dmax || fmax ) {

    avt_log(LOGTRC,2, "computing maximum delays\n" );
  
    /* récupere le delaycache */

    coefctc = 
      rcx_getcoefctc( lofig, losig, slopemiller, RCX_MAX, extcapa, &groundcapa );

    headawe = getaweinfo( losig, 
                          locon_rec, 
                          locon_emt, 
                          extcapa,
                          slopemiller,
                          RCX_MAX,
                          coefctc
                        );
    for( scanawe = headawe ; scanawe ; scanawe = scanawe->NEXT ) {
      awe = (aweinfo_list*)scanawe->DATA ;
      if( awe->FLAG == AWE_NODELAY ) 
        continue ;

      flagmax = 1;
      
      v = -1.0 ; // un nombre qui indique qu'on a pas de valeur calculée.
      computedfast = 'n' ;
      
      if( dmax ) {
        
        d = awevoltage_fast( awe, slopemiller->MAX.slope );
        computedfast = 'y' ;

        if( d > 0.0 ) 
          v = d ;
        else 
          v = awedelay( awe, 
                        slopemiller->MAX.slope, 
                        vmaxmax, 
                        slopemiller->MAX.vt, 
                        slopemiller->MAX.vth, 
                        slopemiller->MAX.vsat, 
                        slopemiller->MAX.rlin, 
                        slopemiller->MAX.r, 
                        slopemiller->MAX.c1, 
                        slopemiller->MAX.c2,
                        slopemiller->MAX.pwl
                      );

        if( v >= 0.0 ) {
          if( *dmax < 0.0 || v > *dmax )
            *dmax = v;
        }
      }

      v = -1.0;

      if( fmax ) {
        
        if( computedfast == 'n' )
          d = awevoltage_fast( awe, slopemiller->MAX.slope );

        pwl = NULL ;
        if( d > 0.0 ) {
          if( pwloutmax ) {
            if( slopemiller->MAX.pwl )
              pwl = mbk_pwl_duplicate( slopemiller->MAX.pwl );
          }
          v = slopemiller->MAX.slope ;
        }
        else 
          v = aweslope( awe, 
                        vmaxmax, 
                        pwloutmax ? &pwl : NULL,
                        slopemiller,
                        RCX_MAX,
                        coefctc
                      );

        if( v >= 0.0 ) {
          if( *fmax < 0.0 || v > *fmax ) {
            *fmax = v;
            if( pwloutmax ) {
              if( *pwloutmax ) mbk_pwl_free_pwl( *pwloutmax );
              *pwloutmax = pwl ;
            }
          }
          else
            mbk_pwl_free_pwl( pwl );
        }
        else {
          mbk_pwl_free_pwl( pwl );
        }
      }
    }

    awefreeinfolist( headawe );
    
    // Pas de connexion entre les deux locons
    if( !flagmax ) {
      avt_log(LOGTRC,2, "can't compute delays for max\n" );
      if( fmax ) *fmax=slopemiller->MAX.slope;
      if( dmax ) *dmax=0.0;
    }
  }

  avt_logexitfunction(LOGTRC,2);

  return(1);
}

/******************************************************************************\
Fonctions qui orientent le réseau RC.
Il faut le faire pour chaque émetteur car les triangles supposent qu'on entre
par le noeud n0. 
\******************************************************************************/

int awe_up_forcreatetriangle( losig_list *losig,
                              lonode_list *lonode,
                              lowire_list *lowire,
                              void *userdata
                            )
{
  rcx_triangle_node( losig, lonode, lowire );
  userdata = NULL;
  return 1 ;
}

int awe_packup_forcreatetriangle( losig_list   *losig,
                                  lonode_list  *lonode,
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

  userdata = NULL;
  return 1 ;
}

int awe_dw_forcleantriangle( losig_list  *losig,
                             lonode_list *lonode,
                             chain_list  *chwire,
                             ht          *htpack,
                             void        *userdata
                           )
{
  chain_list *headht, *chain ;
  rcx_triangle *tr ;
  lowire_list  *wire ;

  headht = GetAllHTElems( htpack );

  for( chain = headht ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)((chain_list*)chain->DATA)->DATA ;
    tr = (rcx_triangle*)rcn_getpack( losig, wire ) ;
    rcx_cleantriangle_node( losig, lonode, tr );
  }
  
  freechain( headht );
  userdata = NULL;
  chwire = NULL;
  return 1 ;
}


void awe_create_triangle( losig_list *losig, lonode_list *driver )
{
  lowire_list *wire ;
 
  avt_logenterfunction(LOGTRC,2,"awe_create_triangle()");
  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );

  rcn_treetrip( losig, 
                driver,
                awe_up_forcreatetriangle,
                NULL,
                (int (*)(losig_list*,lonode_list*,void*,void*))
                                                   awe_packup_forcreatetriangle,
                NULL,
                0
              );
  avt_logexitfunction(LOGTRC,2);
}

void awe_clean_triangle( losig_list *losig, lonode_list *driver )
{
  lowire_list *wire ;
  
  clearallwireflag( losig, RCN_FLAG_PASS );
  for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );

  rcn_treetrip( losig,
                driver,
                NULL,
                awe_dw_forcleantriangle,
                NULL,
                NULL,
                0
              );
}

/******************************************************************************\
Fonction qui récupère les moments sur le réseau RC.
\******************************************************************************/
chain_list* getaweinfo( losig_list   *losig,
                        locon_list   *load,
                        locon_list   *driver,
                        RCXFLOAT      extcapa,
                        rcx_slope    *slopemiller,
                        char          type,
                        RCXFLOAT      coefctc
                      )
{
  ptype_list   *ptl ;
  rcx_list     *rcx ;
  int           isloop ;
  mbkcache     *cache ;
  chain_list   *head = NULL ;
  chain_list   *list ;
  chain_list   *removeextra = NULL ;
  aweinfo_list *awe ;
  unsigned long int sizeallocated=0;
  awecache     *awetop ;

  #ifdef AWEDEBUG
  awestatcallinfo++;
  #endif
  
  avt_logenterfunction(LOGTRC,2,"getaweinfo()");
  
  cache = rcx_get_delay_cache();
  if( cache ) {
    mbk_cache_refresh( cache, NULL, losig );
    mbk_cache_lock( cache, losig );
    awetop = rcx_get_cache_awe( losig, 
                                slopemiller->SENS, 
                                type, 
                                rcx_crosstalkactive( RCX_QUERY ) 
                              );
            
    if( awetop && awetop->HEAD ) {
      if( awetop->HT ) {
        list = (chain_list*)gethtitem( awetop->HT, load );
        if( list != (chain_list*)EMPTYHT && list != (chain_list*)DELETEHT ) {
          while( list ) {
            awe = (aweinfo_list*)list->DATA ;
            if( awe->LOCON_DRIVER == driver  &&
                awe->EXTCAPA      == extcapa    )
              head = addchain( head, awe );
            list = list->NEXT ;
          }
        }
      }
      else {
        awe = awetop->HEAD ;
        while( awe ) {
          if( awe->LOCON_DRIVER == driver  &&
              awe->LOCON_LOAD   == load    &&
              awe->EXTCAPA      == extcapa    )
            head = addchain( head, awe );
          awe = awe->NEXT ;
        }
      }
    }

    if( head ) {
      avt_logexitfunction(LOGTRC,2);
      mbk_cache_unlock( cache, losig );
      return head;
    }
  }

  // Il faut calculer tous les moments sur les récepteurs à partir de l'emetteur driver.
  if( !driver->PNODE  ) {
    awe_error( 4, AVT_WARNING );
    avt_logexitfunction(LOGTRC,2);
    mbk_cache_unlock( cache, losig );
    return NULL ;
  }

  rcx = getrcx( losig );
  if( rcx )
    isloop = rcx_isloop( rcx, losig );
  else
    isloop = 1 ;

  if( ( RCX_USING_AWEMATRIX==RCX_USING_AWEMATRIX_ALWAYS || 
        RCX_USING_AWEMATRIX==RCX_USING_AWEMATRIX_FORCE     )    ||
      ( RCX_USING_AWEMATRIX==RCX_USING_AWEMATRIX_IFNEED  &&
        ( isloop ||
          rcx_is_multiple_pnode( losig, driver )    )       )  )
    build_awe_moment_with_loop( losig, 
                                driver, 
                                extcapa, 
                                slopemiller, 
                                type, 
                                coefctc 
                              );
  else
    build_awe_moment_without_loop( losig, 
                                   driver, 
                                   extcapa, 
                                   slopemiller, 
                                   type, 
                                   coefctc 
                                 );

  ptl     = getptype( losig->USER, AWEINFO );

  if( ptl ) { // Cas des composantes connexes
    
    for( awe = (aweinfo_list*)(ptl->DATA) ; awe ; awe = awe->NEXT ) {
      if( awe->LOCON_DRIVER == driver  &&
          awe->LOCON_LOAD   == load    &&
          awe->EXTCAPA      == extcapa    )
        head = addchain( head, awe );
      else {
        if( !cache )
          removeextra = addchain( removeextra, awe );
      }
    }

    if( cache ) {
      sizeallocated = rcx_add_cache_awe( losig,
                                         slopemiller->SENS, 
                                         type, 
                                         rcx_crosstalkactive( RCX_QUERY )
                                       );
      mbk_cache_update_size( cache, NULL, sizeallocated );
      // trccheckcachecoherence();
    }
    else {
      awefreeinfolist( removeextra );
    }

    losig->USER = delptype( losig->USER, AWEINFO );
  }

  if( cache )
    mbk_cache_unlock( cache, losig );

  avt_logexitfunction(LOGTRC,2);
  return head ;
}

/* return 1 if pnodes for locon 'driver' should be considered as mulptiple
simultaneous driver, or 0 if they can be considers as equivalent. */

int rcx_is_multiple_pnode( losig_list *losig, locon_list *driver )
{
  int          n ;
  int          valid ;
  num_list    *pnode ;
  num_list    *onode ;
  lonode_list *lonode ;
  ptype_list  *ptl ;
  int          resi ;
  RCXFLOAT     capa ;
 
  avt_logenterfunction(LOGTRC,2,"rcx_is_multiple_pnode()");

  valid = 0 ;
  
  if( !driver->PNODE ) {
    avt_log(LOGTRC,2, "no pnode found on locon %s\n", getloconrcxname( driver ) );
    avt_logexitfunction(LOGTRC,2);
    return 0;
  }
    
  /* first test : check the number of pnode */
  n=0 ;
  for( pnode = driver->PNODE ; pnode ; pnode = pnode->NEXT )
    n++;

  if( n > 3 ) {
    avt_log(LOGTRC,2, "more than 3 pnode found on locon %s\n", getloconrcxname( driver ) );
    avt_logexitfunction(LOGTRC,2);
    return 1 ;
  }

  capa = rcx_getcapa( NULL, losig );

  for( pnode = driver->PNODE ; pnode ; pnode = pnode->NEXT ) {

    avt_log(LOGTRC,2,"computing from node %ld\n", pnode->DATA );

    rcn_treetrip( losig, 
                  getlonode( losig, pnode->DATA ), 
                  rcx_imp_prop, 
                  NULL, 
                  NULL, 
                  NULL, 
                  5 
                );
    for( onode = pnode->NEXT ; onode ; onode = onode->NEXT ) {
      lonode = getlonode( losig, onode->DATA );
      ptl = getptype( lonode->USER, AWESUMRESI );
      if( ptl ) {
        resi = ((long)ptl->DATA)/1000.0 ;
        if( resi * capa > 1.0 ) 
          break ;
      }
      else
        /* the number of wire between pnode and onode exceed the value of the 
           last argument of rcn_treetrip */
        break ;
    }

    if( onode )
      valid=1;

    avt_log(LOGTRC,2, "cleanning\n" );
    rcn_treetrip( losig, 
                  getlonode( losig, pnode->DATA ), 
                  rcx_imp_clean, 
                  NULL, 
                  NULL, 
                  NULL, 
                  5 
                );
                
    if( valid )
      break;
  }

  avt_logexitfunction(LOGTRC,2);
  return valid ;
}

int rcx_imp_clean( losig_list  *losig, 
                   lonode_list *lonode, 
                   lowire_list *lowire, 
                   void        *data 
                 )
{
  ptype_list  *ptl ;

  ptl = getptype( lonode->USER, AWESUMRESI );
  if( ptl ) {
    lonode->USER = delptype( lonode->USER, AWESUMRESI );
  }

  losig=NULL;
  lowire=NULL;
  data=NULL;
  return 1 ;
}

int rcx_imp_prop( losig_list  *losig, 
                  lonode_list *lonode, 
                  lowire_list *lowire, 
                  void        *data 
                )
{
  ptype_list  *ptl ;
  int          resi ;
  int          ifrom ;
  lonode_list *pfrom ;

  ptl = getptype( lonode->USER, AWESUMRESI );
  if( ptl ) {
    /* big problem : there is a loop not detected by rcn_treetrip */
    awe_error( 5, AVT_INTERNAL );
  }
  if( lowire ) {
    ifrom = (lowire->NODE1 == lonode->INDEX ? lowire->NODE2 : lowire->NODE1 );
    pfrom = getlonode( losig, ifrom );
    ptl = getptype( pfrom->USER, AWESUMRESI );
    if( ptl )
      resi = ((long)ptl->DATA);
    else
      resi = 0;
    resi = resi + ((int)(1000.0*lowire->RESI));
    lonode->USER = addptype( lonode->USER, AWESUMRESI, (void*)((long)resi) );

    data=NULL;
  }
  return 1;
}

void build_awe_moment_without_loop( losig_list *losig, 
                                    locon_list *driver, 
                                    RCXFLOAT    extcapa, 
                                    rcx_slope  *slopemiller, 
                                    char        type, 
                                    RCXFLOAT    coefctc )
{
  RCXFLOAT     k;
  num_list     *scan_driver;
  lonode_list  *node_driver;
  int           n;

  avt_logenterfunction(LOGTRC,2,"build_awe_moment_without_loop()");

  for( scan_driver = driver->PNODE ; scan_driver ; scan_driver = scan_driver->NEXT ) {

    avt_log(LOGTRC,2, "from driver %d\n", scan_driver->DATA );

    #ifdef AWEDEBUG
    // On ne compte que les vrais appels. Pas les récursifs.
    awestatcallmoment++;
    #endif

    // Premiere étape, on reserve la mémoire pour les moments sur tous les noeuds du reseau RC.

    node_driver = getlonode( losig, scan_driver->DATA );

    awe_create_triangle( losig, node_driver );

    awebuildnodes( losig, 
                   node_driver, 
                   extcapa, 
                   slopemiller,
                   type, 
                   coefctc,
                   0
                 );

    // Ensuite, on construit le premier moment : il sert à normaliser les résultats pour ne pas
    // avoir trop d'erreur d'arrondi dans les calculs.

    #ifdef AWE_NORMALISE
    avt_log(LOGTRC,2,"normalisation\n");
    awemoment( losig, node_driver, 1, 1.0, extcapa );
    k = -awemaxmoment( losig, node_driver );
    awebuildnodes( losig, 
                   node_driver, 
                   extcapa, 
                   slopemiller,
                   type, 
                   coefctc,
                   1
                 ); // Réinitialise les SUMMOMENTCAPA
    #else
    k = 1.0;
    #endif

    if( k > 1e-6 ) {
      avt_log(LOGTRC,2,"building moment\n");
      // Enfin, on construit définitivement les moments.
      for( n=1;n<=AWE_MAX_MOMENT;n++)
        awemoment( losig, node_driver, n, k, extcapa );
    
      // On les mémorise finalement dans les structures aweinfo.
      awebuildinfo( losig, driver, node_driver->INDEX, k, extcapa );
    }
    // Puis on efface ceux qui ne servent à rien.
    avt_log(LOGTRC,2,"cleanning\n");
    awecleannodes( losig, node_driver );
    awe_clean_triangle( losig, node_driver );
  }

  avt_logexitfunction(LOGTRC,2);
}

void awebuildinfo( losig_list *ptsig, 
                   locon_list *driver, 
                   long node_driver, 
                   RCXFLOAT k, 
                   RCXFLOAT extcapa 
                 )
{
  chain_list    *scanlocon ;
  num_list      *scannode ;
  locon_list    *load;
  awetabnode    *tabnode;
  lonode_list   *ptnode;
  ptype_list    *ptl;
  double         m[AWE_MAX_MOMENT];
  int            i;

  for( i=0 ; i<AWE_MAX_MOMENT ; i++ )
    m[i] = 0.0 ;

  for( scanlocon = (chain_list*)(getptype( ptsig->USER, LOFIGCHAIN )->DATA ) ;
       scanlocon ;
       scanlocon = scanlocon->NEXT                                                ) {
    
    load = (locon_list*)(scanlocon->DATA);
    if( !rcx_isvalidlocon( load ) )
      continue;
    
    if( load != driver ) {
      
      for( scannode = load->PNODE ; scannode ; scannode = scannode->NEXT ) {

        if( node_driver == scannode->DATA ) {
          aweaddnodeinfo( ptsig,
                          driver,
                          node_driver, 
                          load,
                          scannode->DATA,
                          m,
                          k,
                          extcapa
                        );
          continue;
        }

        ptnode = getlonode( ptsig, scannode->DATA );
        
        // lorsqu'on a plusieurs composantes connexes, il est possible
        // de ne pas avoir de ptype construits.
        ptl = getptype( ptnode->USER, AWENODE );
        if( !ptl ) {
          aweaddnodeinfo( ptsig,
                          driver,
                          node_driver, 
                          load,
                          scannode->DATA,
                          m,
                          k,
                          extcapa
                        );
          continue;
        }

        tabnode = (awetabnode*)(ptl->DATA );
        
        aweaddnodeinfo( ptsig,
                        driver,
                        node_driver, 
                        load,
                        scannode->DATA,
                        tabnode->MOMENT,
                        k,
                        extcapa
                      );
      }
    }
  }
}

aweinfo_list* awegetheapinfo( void )
{
  aweinfo_list *pt=NULL;
  
  pt = (aweinfo_list*)mbkalloc(sizeof( aweinfo_list ));
  return pt ;
}

void awefreeinfo( aweinfo_list *awe )
{
  mbkfree( awe );
}

aweinfo_list* awegetnewinfo( void )
{
  aweinfo_list *pt;

  pt = awegetheapinfo();
  return pt;
}

aweinfo_list* aweaddnodeinfo( losig_list   *signal,
                              locon_list   *driver,
                              long          node_driver,
                              locon_list   *load,
                              long          node_load,
                              RCXFLOAT    *moment,
                              RCXFLOAT     k,
                              RCXFLOAT     extcapa
                            )
{
  aweinfo_list  *pt;
  int            i;
  ptype_list    *ptl;
  
  pt = awegetnewinfo( ); 

  pt->LOCON_DRIVER = driver ;
  pt->NODE_DRIVER  = node_driver ;
  pt->LOCON_LOAD   = load ;
  pt->NODE_LOAD    = node_load ;
  pt->K            = k ;
  pt->EXTCAPA      = extcapa ;

  // Cas où tous les moments sont nuls : l'émetteur et le récepteur sont
  // sur le meme noeud.

  for( i=0 ; i < AWE_MAX_MOMENT ; i++ ) {
    if( moment[i] != 0.0 )
      break;
  }
 
  if( i==AWE_MAX_MOMENT ) {
    pt->FLAG = AWE_NODELAY ;
  } else {
    pt->FLAG = AWE_MOMENT ;
  }
  
  for( i = 0 ; i < AWE_MAX_MOMENT ; i++ ) {
    pt->DATA.MOMENT[i] = moment[i];
  }

  ptl = getptype( signal->USER, AWEINFO );
  if( !ptl ) {
    signal->USER = addptype( signal->USER, AWEINFO, NULL ) ;
    ptl = signal->USER ;
  }
  
  pt->NEXT = (aweinfo_list*)(ptl->DATA );
  ptl->DATA = pt ;
 
  return( pt );
}

void awemomentfiltre( aweinfo_list *info )
{
  matrice     *a, *c, *alpha ;
  int          l, m, k ;
  matrice     *polea, *polec, *polex ;
  RCXFLOAT    poles[AWE_MAX_ORDER], poly[AWE_MAX_ORDER+1];
  int          ordre ;

  avt_logenterfunction(LOGTRC,2,"awemomentfiltre()");

  if( info->FLAG == AWE_NODELAY ) {
    avt_log(LOGTRC,2,"nodelay\n");
    avt_logexitfunction(LOGTRC,2);
    return;
  }
 
  #ifdef AWEDEBUG
  awestatcallfilter++;
  #endif

  ordre = AWE_MAX_ORDER+1;
 
  do
  {
    ordre--;
    avt_log(LOGTRC,2,"order %d\n", ordre );

    if( ordre == 0 ) {
      info->FLAG = (AWE_FLAGERROR | AWE_FLAGALLORDERFAILED );

      avt_errmsg( TRC_ERRMSG, "002", AVT_WARNING, 
                  rcx_getsigname( AWE_DEBUG_NET ) );
    }
    else {
   
      avt_log(LOGTRC,2, "building matrix\n");
      a     = mat_create( ordre, ordre );
      alpha = mat_create( ordre, 1 );
      c     = mat_create( ordre, 1 );
      polea = mat_create( ordre, ordre );
      polec = mat_create( ordre, 1 );
      polex = mat_create( ordre, 1 );

      for( l = 0 ; l < ordre ; l++ )
      {
        for( m = 0, k=l ; m < ordre ; m++, k++ )
          MATELEM( a, l, m ) = ( (k==0) ? 1.0 : info->DATA.MOMENT[k-1] );
      }

      for( l = 0 ; l < ordre ; l++ )
        MATELEM( c, l, 0 ) = -info->DATA.MOMENT[ordre+l-1];

      // Détermination du dénominateur de la fonction de transfert

      avt_log(LOGTRC,2,"finding denominator\n" );

      if( !mat_solve( a, c, alpha ) ) {
        avt_log(LOGTRC,2, "singular matrix at order %d on signal %s.\n",
                 ordre,
                 rcx_getsigname( info->LOCON_DRIVER->SIG )
               );
        mat_free( a );
        mat_free( alpha );
        mat_free( c );
        mat_free( polea );
        mat_free( polec );
        mat_free( polex );

        continue;
      }

      // Vérifier qu'on a pas de nan :
      //

      /* on recupère dans alpha le dénominateur de la fonction de transfert :
       * bn.s^n+...+b2.s^2+b1.s+1  -> (bn,...,b2,b1)
       */

      for( l=0 ; l<ordre ; l++ )
        poly[l+1] = MATELEM(alpha, ordre-1-l, 0);
      poly[0]=1.0;

      // On récupère les racines de ce polynome. Si on a pas pu les obtenir, on abandonne
      avt_log(LOGTRC,2,"finding root\n");

      if( poly_findroot( poly, ordre, poles ) == 0 ) {
        avt_log(LOGTRC,2, "awe : can't find roots at order %d on signal %s.\n",
                 ordre,
                 rcx_getsigname( info->LOCON_DRIVER->SIG )
               );
        mat_free( a );
        mat_free( alpha );
        mat_free( c );
        mat_free( polea );
        mat_free( polec );
        mat_free( polex );
        continue;
      }

      /* En principe, toutes les racines sont censées être négatives. On le vérifie
       */
      for( l = 0 ; l < ordre ; l++ )
        if( poles[l]>=0.0 ) {
          avt_log(LOGTRC,2, "awe : a positive root has been found at order %d on signal %s.\n",
                   ordre,
                   rcx_getsigname( info->LOCON_DRIVER->SIG )
               );
          break;
        }
      if( l<ordre ) {
        mat_free( a );
        mat_free( alpha );
        mat_free( c );
        mat_free( polea );
        mat_free( polec );
        mat_free( polex );
        continue;
      }
      
      /* on détermine les résidus */
     
      avt_log(LOGTRC,2,"find residues\n");
      for( l = 0 ; l < ordre ; l++ )
        for( m = 0 ; m < ordre ; m++ )
          MATELEM( polea, l, m ) = 1.0/pow(poles[m],l+1.0);
     
      MATELEM( polec, 0, 0 ) = -1.0;
      for( l = 1 ; l < ordre ; l++ )
        MATELEM( polec, l, 0 ) = -info->DATA.MOMENT[l-1];

      if( !mat_solve( polea, polec, polex ) ) {
        avt_log(LOGTRC,2, "awe : risidues matrix is singular at order %d on signal %s.\n",
                 ordre,
                 rcx_getsigname( info->LOCON_DRIVER->SIG )
               );
        mat_free( a );
        mat_free( alpha );
        mat_free( c );
        mat_free( polea );
        mat_free( polec );
        mat_free( polex );
        continue;
      }

      // C'est fini : on range le tout.

      avt_log(LOGTRC,2,"parameter determined\n");
      info->FLAG = AWE_FILTER ;
      for( l=0 ; l<ordre ; l++ ) {
        info->DATA.FILTER.POLE[l]   = poles[l];
        info->DATA.FILTER.RESIDU[l] = MATELEM( polex, l, 0 ) ;
      } 
      info->DATA.FILTER.ORDER = ordre;

      mat_free( a );
      mat_free( alpha );
      mat_free( c );
      mat_free( polea );
      mat_free( polec );
      mat_free( polex );
    }
  }
  while( info->FLAG == AWE_MOMENT );
  avt_logexitfunction(LOGTRC,2);
}

/******************************************************************************\
Renvoie une liste de délais.
\******************************************************************************/

awelist* getawedelaylist( lofig_list *lofig,
                          losig_list *losig,
                          locon_list *locon_emt,
                          rcx_slope  *slopemiller,
                          RCXFLOAT    extcapa
                        )
{
  chain_list    *chain;
  locon_list    *locon;
  int            res;
  RCXFLOAT       dmax, dmin, fmax, fmin;
  awelist       *headlist=NULL;
  
  for( chain = ((chain_list*)(getptype( losig->USER, LOFIGCHAIN )->DATA) ) ;
       chain ;
       chain = chain->NEXT
     ) {
     
    locon = (locon_list*)chain->DATA;
    if( !rcx_isvalidlocon( locon ) )
      continue;
    
    if( locon == locon_emt )
      continue;

    res = getawedelay( lofig, losig, locon_emt, locon,
                       slopemiller,
                       slopemiller->MAX.vend,
                       slopemiller->MIN.vend,
                       extcapa,
                       &dmax,
                       &dmin,
                       &fmax,
                       &fmin,
                       NULL,
                       NULL
                     );
                     
    if( res )
      headlist = addawelist( headlist, locon, dmax, dmin, fmax, fmin );
  }

  return headlist;
}

void freeawedelaylist( awelist *headlist )
{
  awelist *next;

  while( headlist ) {
    next = headlist->NEXT;
    mbkfree( headlist );
    headlist = next;
  }
}

awelist *addawelist( awelist *head, locon_list *locon, 
                     RCXFLOAT dmax, RCXFLOAT dmin, RCXFLOAT fmax, RCXFLOAT fmin
                   )
{
  awelist *new;

  new = mbkalloc( sizeof( awelist ) );
  new->LOCON = locon;
  new->DMAX  = dmax;
  new->DMIN  = dmin;
  new->FMAX  = fmax;
  new->FMIN  = fmin;
  new->NEXT  = head;
  
  return new;
}

void awe_error( int subcode, int type )
{
    avt_errmsg( TRC_ERRMSG, "000", type,
                subcode,
                rcx_getsigname( AWE_DEBUG_NET ),
                getloconrcxname( AWE_DEBUG_DRIVER ),
                getloconrcxname( AWE_DEBUG_RECEIVER )
              );
}

void awefreeinfolist( chain_list *head )
{
  mbkcache   *cache ;
  chain_list *scan ;
  
  cache = rcx_get_delay_cache();

  if( !cache ) {
    for( scan = head ; scan ; scan = scan->NEXT ) {
      awefreeinfo( (aweinfo_list*)scan->DATA );
    }
  }
  
  freechain( head );
}

awecache* rcx_get_cache_awe( losig_list     *losig, 
                             char            transition, 
                             char            type, 
                             char            iscrosstalk 
                           )
{

  ptype_list   *ptl ;
  long int      ntype ;
  ptype_list   *ptype ;
  awecache     *awe ;

  ptl = getptype( losig->USER, RCX_DELAYCACHE );
  if( !ptl ) {
    avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
    return NULL ;
  }
  
  ntype = 
         rcx_cache_build_ptype( transition, type, iscrosstalk, RCX_MODEL_AWE );

  for( ptype = (ptype_list*)ptl->DATA ; ptype ; ptype = ptype->NEXT ) {

    if( ptype->TYPE == ntype ) {
      awe = (awecache*)ptype->DATA ;
      return awe ;
    }
  }

  return NULL ;
}

unsigned long int rcx_add_cache_awe( losig_list *losig,
                                     char        transition, 
                                     char        type, 
                                     char        iscrosstalk
                                   )
{
  unsigned long int size_allocated = 0 ;
  ptype_list        *ptl ;
  long int           ntype ;
  ptype_list        *ptype ;
  aweinfo_list      *head ;
  awecache          *awetop ;

  ptl = getptype( losig->USER, RCX_DELAYCACHE );
  if( !ptl ) {
    avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
    return 0 ;
  }
  
  ntype = 
         rcx_cache_build_ptype( transition, type, iscrosstalk, RCX_MODEL_AWE );

  for( ptype = (ptype_list*)ptl->DATA ; ptype ; ptype = ptype->NEXT ) {

    if( ptype->TYPE == ntype )
      break ;
  }

  if( !ptype ) {

    awetop = mbkalloc( sizeof( awecache ) );
    awetop->HEAD = NULL ;
    awetop->HT   = NULL ;
    awetop->NB   = 0 ;
    size_allocated += sizeof( awecache );
    
    ptl->DATA = addptype( (ptype_list*)ptl->DATA, ntype, awetop );
    size_allocated += sizeof( ptype_list );

  }
  else {
    awetop = (awecache*)ptype->DATA ; 
  }

  ptl = getptype( losig->USER, AWEINFO );
  if( ptl ) {

    head = (aweinfo_list*)ptl->DATA ;
    
    if( head ) {
    
      while( head->NEXT ) {
        size_allocated += sizeof( aweinfo_list );
        awetop->NB++ ;
        head = head->NEXT ;
      }
      size_allocated += sizeof( aweinfo_list );
      awetop->NB++ ;
      
      size_allocated += rcx_awe_cache_update_ht( awetop, 
                                                 (aweinfo_list*)ptl->DATA 
                                               );

      head->NEXT = awetop->HEAD ;
      awetop->HEAD = (aweinfo_list*)ptl->DATA ;
    }

    ptl->DATA = NULL ;
  }
  else {
    avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
  }
  
  return size_allocated ;
}

unsigned long int rcx_awe_cache_update_ht( awecache *awetop, 
                                           aweinfo_list *head 
                                         )
{
  aweinfo_list      *scan ;
  unsigned long int  size=0;
 
  /* pas de prise en compte de la taille de la table de hash car sa taille
     peut varier */
  if( awetop->HT == NULL ) {
    if( awetop->NB > 30 ) {
      awetop->HT = addht(30);
      for( scan = awetop->HEAD ; scan ; scan = scan->NEXT ) {
        size += sizeof( chain_list );
        rcx_awe_cache_add_ht( awetop->HT, scan );
      }
    }
  }
  else {
    for( scan = head ; scan ; scan = scan->NEXT ) {
      size += sizeof( chain_list );
      rcx_awe_cache_add_ht( awetop->HT, scan );
    }
  }

  return size ;
}

void rcx_awe_cache_add_ht( ht *ht, aweinfo_list *aweinfo )
{
  controlled_addhtitem( ht,
                        aweinfo->LOCON_LOAD,
                        (long(*)(int,long,void*))rcx_awe_cache_fn, 
                        (void*)aweinfo 
                      );
}

long rcx_awe_cache_fn( int isnew, chain_list *head, aweinfo_list *awe )
{
  if( isnew )
    return (long) addchain( NULL, awe );
  return (long) addchain( head, awe );
}

unsigned long int rcx_cache_release_awe( awecache *awetop )
{
  unsigned long int torelease = 0;
  aweinfo_list *next ;
  aweinfo_list *head ;
  chain_list   *chainht ;
  chain_list   *scanht ;
  chain_list   *count ;

  head = awetop->HEAD ;
  while( head ) {
    torelease += sizeof( aweinfo_list );
    next = head->NEXT ;
    awefreeinfo( head );
    head = next ;
  }
  if( awetop->HT ) {
    chainht = GetAllHTElems( awetop->HT );
    for( scanht = chainht ; scanht ; scanht = scanht->NEXT ) {
      for( count = (chain_list*)scanht->DATA ; count ; count = count->NEXT )
        torelease += sizeof( chain_list ) ;
      freechain( (chain_list*)scanht->DATA );
    }
    freechain( chainht );
    delht( awetop->HT );
  }
  mbkfree( awetop );
  torelease += sizeof( awecache );


  return torelease ;
}
