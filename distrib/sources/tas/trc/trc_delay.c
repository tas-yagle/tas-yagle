/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_delay.c                                                 */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.117 $
Author   : $Author: fabrice $
Date     : $Date: 2009/07/02 14:42:53 $

*/
#include AVT_H
#include MUT_H
#include "trc.h"

mbkcache *RCXDELAYCACHE = NULL ;
char      RCXDELAYCACHEFORQUIET     = YES ;
char      RCXDELAYCACHEFORCROSSTALK = YES ;
char      RCX_USE_MATRIX_LOAD       = NO ;

/* Fonctions internes */

void    rcx_admittance(                 losig_list*,
                                        long,
                                        RCXFLOAT*,
                                        RCXFLOAT*,
                                        RCXFLOAT*,
                                        RCXFLOAT,
                                        rcx_slope*,
                                        char,
                                        RCXFLOAT
                      );

void    rcx_recadmi(                    losig_list*,
                                        lowire_list*,
                                        long,
                                        RCXFLOAT*,
                                        RCXFLOAT*,
                                        RCXFLOAT*,
                                        RCXFLOAT,
                                        rcx_slope*,
                                        char,
                                        RCXFLOAT
                   );
void     rcx_triangle_admi( losig_list  *ptsig,
                            rcx_triangle *triangle,
                            RCXFLOAT    *y1,
                            RCXFLOAT    *y2,
                            RCXFLOAT    *y3,
                            RCXFLOAT     extcapa,
                            rcx_slope   *slope,
                            char         type,
                            RCXFLOAT     coefctc
                          );


RCXFLOAT rcx_getsumcapa( lofig_list*, losig_list*, rcx_slope*, char );

/******************************************************************************/

RCXFLOAT rcx_getdelay( lofig, loconb, locone )
  lofig_list *lofig;
  locon_list *loconb;
  locon_list *locone;
{
  RCXFLOAT dmax ;
  rcx_slope lslope;

  lslope.F0MAX  = 1.0 ;
  lslope.FCCMAX = 1.0 ;
  lslope.SENS   = TRC_SLOPE_UP ;
  lslope.CCA       =  0.0 ;
  lslope.MAX.slope =  1.0 ;
  lslope.MAX.vend  =  1.0 ;
  lslope.MAX.vt    =  0.0 ;
  lslope.MAX.vdd   =  1.0 ;
  lslope.MAX.vsat  = -1.0 ;
  lslope.MAX.rlin  =  0.0 ;
  lslope.MAX.vth   =  0.5 ;
  lslope.MAX.r     =  0.0 ;
  lslope.MAX.c1    =  0.0 ;
  lslope.MAX.c2    =  0.0 ;
  lslope.MAX.pwl   =  NULL ;
  lslope.MIN.slope =  1.0 ;
  lslope.MIN.vend  =  1.0 ;
  lslope.MIN.vt    =  0.0 ;
  lslope.MIN.vdd   =  1.0 ;
  lslope.MIN.vsat  = -1.0 ;
  lslope.MIN.rlin  =  0.0 ;
  lslope.MIN.vth   =  0.5 ;
  lslope.MIN.r     =  0.0 ;
  lslope.MIN.c1    =  0.0 ;
  lslope.MIN.c2    =  0.0 ;
  lslope.MIN.pwl   =  NULL ;
  lslope.MAX.slnrm = 1.0; // avoid UMR
  lslope.MIN.slnrm = 1.0; // avoid UMR

  rcx_getdelayslope( lofig, 
                     loconb, 
                     locone, 
                     &lslope,
                     0.0,     // capa
                     &dmax,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL
                   );

  if( dmax >= 0.0 )
    return( dmax );
  return(0.0);
}

/*******************************************************************************

rcx_getdelayslope()

Calcul des délais et/ou des fronts dans les réseaux RC. Renvoie 1 si le calcul
a pu se faire, et 0 en cas d'erreur.

*******************************************************************************/

int rcx_getdelayslope( lofig,
                       loconb, 
                       locone, 
                       slope,
                       capa, 
                       dmax, 
                       dmin, 
                       fmax,
                       fmin,
                       pwloutmax,
                       pwloutmin
                     )
lofig_list         *lofig;
locon_list	   *loconb;
locon_list         *locone;
rcx_slope          *slope ;
RCXFLOAT            capa ;
RCXFLOAT           *dmax ;
RCXFLOAT           *dmin ;
RCXFLOAT           *fmax ;
RCXFLOAT           *fmin ;
mbk_pwl           **pwloutmax ; 
mbk_pwl           **pwloutmin ; 
{
 losig_list	*losig ;
 RCXFLOAT        vmaxmax;
 RCXFLOAT        vmaxmin;
 rcx_list       *ptrcx;
 char            envbasic=NO;
 rcx_timings *rct, **head;
 num_list       *loadbytrans ;
 num_list       *pnodetrans ;
 lonode_list    *lonode ;
 chain_list     *chainlocon ;

 avt_logenterfunction( LOGTRC,2,"rcx_getdelayslope()" );
 if ( avt_islog(2,LOGTRC) ) {
   avt_log(LOGTRC,2,"lofig  %s\n", lofig->NAME );
   avt_log(LOGTRC,2,"loconb %s\n", getloconrcxname( loconb ) );
   avt_log(LOGTRC,2,"locone %s\n", getloconrcxname( locone ) );
   avt_log(LOGTRC,2,"slope { F0MAX=%g FCCMAX=%g SENS=%c CCA=%g\n", slope->F0MAX, slope->FCCMAX, slope->SENS, slope->CCA );
 }
 if( dmax || fmax ) {
   if ( avt_islog(2,LOGTRC) ) {
     avt_log(LOGTRC,2,"|  MAX { slope=%g vend=%g vt=%g vdd=%g\n", slope->MAX.slope, slope->MAX.vend, slope->MAX.vt, slope->MAX.vdd );
     avt_log(LOGTRC,2,"|       vsat=%g rlin=%g vth=%g slnrm=%g\n", slope->MAX.vsat, slope->MAX.rlin, slope->MAX.vth, slope->MAX.slnrm );
     avt_log(LOGTRC,2,"|       r=%g c1=%g c2=%g pwl=%c }\n", slope->MAX.r, slope->MAX.c1, slope->MAX.c2, slope->MAX.pwl?'Y':'N' );
   }
 }
 if( dmin || fmin ) {
   if (avt_islog(2,LOGTRC)) {
     avt_log(LOGTRC,2,"| MIN { slope=%g vend=%g vt=%g vdd=%g\n", slope->MIN.slope, slope->MIN.vend, slope->MIN.vt, slope->MIN.vdd );
     avt_log(LOGTRC,2,"|       vsat=%g rlin=%g vth=%g slnrm=%g\n", slope->MIN.vsat, slope->MIN.rlin, slope->MIN.vth, slope->MIN.slnrm );
     avt_log(LOGTRC,2,"|               r=%g c1=%g c2=%g pwl=%c }\n", slope->MIN.r, slope->MIN.c1, slope->MIN.c2, slope->MIN.pwl?'Y':'N' );
   }
 }
 if (avt_islog(2,LOGTRC)) {
   avt_log(LOGTRC,2,"|     }\n" );
   avt_log(LOGTRC,2,"capa=%g\n", capa );
 }
         
 if( dmax ) *dmax = -1.0;
 if( dmin ) *dmin = -1.0;
 if( fmax ) *fmax = -1.0;
 if( fmin ) *fmin = -1.0;

 if( pwloutmax ) *pwloutmax = NULL ;
 if( pwloutmin ) *pwloutmin = NULL ;
 
 if( !loconb || !locone ) {
   avt_log(LOGTRC,2,"return on error because a locon is missing\n" );
   avt_errmsg( TRC_ERRMSG, "028", AVT_WARNING, getloconrcxname( loconb ), getloconrcxname( locone ) );
   avt_logexitfunction(LOGTRC,2);
   return 0;
 }

 if( ( ( dmax || fmax ) && slope->MAX.slope < 0.0 ) ||
     ( ( dmin || fmin ) && slope->MIN.slope < 0.0 )    ) {
   avt_log(LOGTRC,2,"return on error because an input slope is negative\n" );
   avt_errmsg( TRC_ERRMSG, "029", AVT_ERROR, getloconrcxname( loconb ), getloconrcxname( locone ) );
   avt_logexitfunction(LOGTRC,2);
   return 0;
 }

 losig = loconb->SIG ;

 // -------------------------------------
 // rc timings: zinaps: 5/3/2004
 
 avt_log(LOGTRC,2,"finding rc timings\n" );
 if (rcx_has_rc_timings(losig))
   {
     float a, b;
     char dir;
     dir=slope->SENS == TRC_SLOPE_UP?'u':'d';
     head=rcx_get_rc_timings(loconb, locone, dir);
/*     {
       char buf0[512], buf1[512];
       printf("rcx request %s %s, sig=%s, slope=%gps\n",mbk_putconname(loconb, buf0), mbk_putconname(locone, buf1), getsigname(losig), slope->MAX.slope);
     }*/
     if (head==NULL)
       {
         char buf0[512], buf1[512];
         avt_errmsg( TRC_ERRMSG, "026", AVT_ERROR, dir, dir, mbk_putconname(loconb, buf0), mbk_putconname(locone, buf1), getsigname(losig) );
         if( dmax ) *dmax = 0;
         if( dmin ) *dmin = 0;
         if( fmax ) *fmax = slope->MAX.slope;
         if( fmin ) *fmin = slope->MIN.slope;
         avt_logexitfunction(LOGTRC,2);
         return 0;
       }
         
     rct=rcx_get_computed_data(*head, slope->MAX.slnrm, &a, &b);
     if (rct==NULL)
       {
         char buf0[512], buf1[512];
         avt_errmsg( TRC_ERRMSG, "027", AVT_ERROR, dir,dir,slope->MAX.slnrm, mbk_putconname(loconb, buf0), mbk_putconname(locone, buf1), getsigname(losig) );
         if( dmax ) *dmax = 0;
         if( dmin ) *dmin = 0;
         if( fmax ) *fmax = slope->MAX.slope;
         if( fmin ) *fmin = slope->MIN.slope;
         avt_logexitfunction(LOGTRC,2);
         return 0;
       }
     //printf("ret d=%g s=%g for %gps\n",a,b,smax);
     if( dmax ) *dmax = a;
     if( dmin ) *dmin = a;
     if( fmax ) *fmax = b;
     if( fmin ) *fmin = b;
     avt_logexitfunction(LOGTRC,2);
     return 1;
   }

 // -------------------------------------
 ptrcx = getrcx( losig );

 if( slope->SENS == TRC_SLOPE_UP && ptrcx->DRIVERSUP )
   loconb = ptrcx->DRIVERSUP ;
 if( slope->SENS == TRC_SLOPE_DOWN && ptrcx->DRIVERSDW )
   loconb = ptrcx->DRIVERSDW ;

 if( !ptrcx || GETFLAG(ptrcx->FLAG, RCXERROR ) ) {
   avt_errmsg( TRC_ERRMSG, "032", AVT_ERROR, getloconrcxname( loconb ), getloconrcxname( locone ) );
   avt_log(LOGTRC,2,"return on error because there is no valid rcx data\n" );
   avt_logexitfunction(LOGTRC,2);
   return 0 ;
 }
 
 if( GETFLAG( ptrcx->FLAG, RCXIGNORE ) ) {
   avt_log(LOGTRC,2,"return on error because rcx view is typed RCXIGNORE.\n" );
   avt_logexitfunction(LOGTRC,2);
   return 0;
 }
 
 // Convertion du front d'entrée en un front montant. Dans les réseau RC, tous
 // est linéaire.
 
 if( slope->SENS == TRC_SLOPE_UP ) {
   vmaxmax = slope->MAX.vend;
   vmaxmin = slope->MIN.vend;
 } else {
   vmaxmax = slope->MAX.vdd - slope->MAX.vend;
   vmaxmin = slope->MIN.vdd - slope->MIN.vend;
 }

 if( ( ( dmax || fmax ) && vmaxmax <= slope->MAX.vt ) || 
     ( ( dmin || fmin ) && vmaxmin <= slope->MIN.vt )    ) {
   avt_errmsg( TRC_ERRMSG, "030", AVT_ERROR, getloconrcxname( loconb ), getloconrcxname( locone )  );
   avt_log(LOGTRC,2,"return on error because there is a problem on threshold [1].\n" );
   avt_logexitfunction(LOGTRC,2);
   return 0;
 }

 if( ( ( dmax || fmax ) && slope->MAX.vth > vmaxmax ) || 
     ( ( dmin || fmin ) && slope->MIN.vth > vmaxmin )    ) {
   avt_errmsg( TRC_ERRMSG, "031", AVT_ERROR, getloconrcxname( loconb ), getloconrcxname( locone )  );
   avt_log(LOGTRC,2,"return on error because there is a problem on threshold [2].\n" );
   avt_logexitfunction(LOGTRC,2);
   return 0;
 }

 // Récupération du réseau RC.

 rcn_lock_signal( lofig, losig );

 /* On gère seulement le cas où on a détecté une erreur de modèle auparavant.
    On ne détecte pas d'erreur ici car les causes peuvent etre multiples. Mais
    il faudrait le faire... */

 if( rcx_isctkbasicmodel( ptrcx ) ) {
   envbasic=YES;
   rcx_setenvbasicmodel();
 }
 
 if( !GETFLAG( ptrcx->FLAG, RCXNOWIRE ) ) {

   loadbytrans = rcx_getnodebytransition( locone, slope->SENS );
   if( loadbytrans ) {
     for( pnodetrans = loadbytrans ; pnodetrans ; pnodetrans++ ) {
       lonode = getlonode( locone->SIG, loadbytrans->DATA ) ;
       chainlocon = getloconnode( lonode );
       if( chainlocon ) {
         locone = (locon_list*)(chainlocon->DATA) ;
         break ;
       }
     }
   }

   // Calcul des délais.

   if( RCX_USING_AWEMATRIX == RCX_USING_AWEMATRIX_NEVER ) {
     if(! rcx_isloop( ptrcx, losig ) ) {
       avt_log(LOGTRC,2,"computing delays with getawedelay\n" );
       getawedelay( lofig,
                    losig,
                    loconb, 
                    locone, 
                    slope,
                    vmaxmax,
                    vmaxmin,
                    capa,
                    dmax,
                    dmin,
                    fmax,
                    fmin,
                    pwloutmax,
                    pwloutmin
                  );
     }
     else {
       if( rcx_islosigbreakloop( losig ) ) {
         avt_log(LOGTRC,2,"computing delays with rcx_delayloop\n" );
         rcx_delayloop( lofig,
                        losig,
                        loconb, 
                        slope,
                        slope->MAX.slope,
                        slope->MIN.slope,
                        vmaxmax,
                        slope->MAX.vt,
                        capa,
                        dmax,
                        dmin,
                        fmax,
                        fmin,
                        NULL,
                        NULL
                      );
       }
       else {
         if( dmax ) *dmax = 0.0 ;
         if( dmin ) *dmin = 0.0 ;
         if( fmax ) *fmax = slope->MAX.slope ;
         if( fmin ) *fmin = slope->MIN.slope ;
       }
     }
   }
   else {
     avt_log(LOGTRC,2,"computing delays with getawedelay\n" );
     getawedelay( lofig,
                  losig,
                  loconb, 
                  locone, 
                  slope,
                  vmaxmax,
                  vmaxmin,
                  capa,
                  dmax,
                  dmin,
                  fmax,
                  fmin,
                  pwloutmax,
                  pwloutmin
                );
   }

 }
 else {
   if( dmax ) *dmax = 0.0;
   if( dmin ) *dmin = 0.0;
   if( fmax ) *fmax = slope->MAX.slope ;
   if( fmin ) *fmin = slope->MIN.slope ;
 }

 if (avt_islog(2,LOGTRC))
   avt_log(LOGTRC,2,"delay rc net %s from %s to %s. dmax=%f fmax=%f dmin=%f fmin=%f.\n",
          rcx_getsigname( losig ),
          getloconrcxname( loconb ),
          getloconrcxname( locone ),
          dmax ? *dmax : 0.0,
          fmax ? *fmax : 0.0,
          dmin ? *dmin : 0.0,
          fmin ? *fmin : 0.0
        );
 
 if( envbasic == YES ) {
   rcx_unsetenvbasicmodel();
 }

 avt_log(LOGTRC,2,"unlocking signal\n" );
 rcn_unlock_signal( lofig, losig );
 avt_logexitfunction(LOGTRC,2);
 return(1);
}

void rcx_rcnload_reset( losig_list *losig )
{
  rcx_list *ptrcx ;
  
  ptrcx = getrcx( losig );
  if( ptrcx ) {
    CLEARFLAG( ptrcx->FLAG, RCXLOADCAPA );
    CLEARFLAG( ptrcx->FLAG, RCXLOADPI   );
  }
}

/* Calcul de la charge vue à partir du noeud 'pin'.
 * Le champ model spécifie le type de charge : RCX_CAPALOAD ou RCX_PILOAD. On
 * peut demander à la fonction de déterminer elle même ce qu'elle veut faire à
 * l'aide de la valeur RCX_BESTLOAD.
 * Renvoie soit RCX_CAPALOAD, soit RCX_PILOAD, selon le type de charge calculée
 * RCX_CAPALOAD  : simple capacité dans c1.
 * RCX_PILOAD    : cellule en pi dans r, c1, c2.
 * RCX_LOADERROR : une erreur est survenue.
 *
 * Attention : cette fonction mémorise sur le signal le dernier type de 
 * charge utilisée si cette fonction est appellée avec RCX_BESTLOAD. Lors des
 * appels ultérieurs avec RCX_BESTLOAD, le test du type de charge à utiliser
 * n'est pas refait. Ce n'est genant que dans TMA où l'on fait varier le 
 * extcapa. Dans ce cas on peut réinitialiser le type de flag avec un appel
 * à la fonction rcx_rcnload_reset()
 */

char rcx_rcnload( lofig_list *lofig,
                  losig_list *ptsig, 
                  num_list   *driver,
                  RCXFLOAT   *r, 
                  RCXFLOAT   *c1, 
                  RCXFLOAT   *c2,
	          char        model,
                  RCXFLOAT    extcapa,
                  rcx_slope  *slope,
                  char        type
                )
{
  rcx_list       *ptrcx ;
  char            ret ;
  mbkcache       *cache ;
  rcx_cache_load *cacheload ;
  unsigned long int sizeallocated ;

  avt_logenterfunction( LOGTRC,2,"rcx_rcnload()" );
  if (avt_islog(2,LOGTRC)) {
    avt_log(LOGTRC,2,"lofig  %s\n", lofig->NAME );
    avt_log(LOGTRC,2,"ptsig  %s\n", rcx_getsigname( ptsig ) );
    avt_log(LOGTRC,2,"sens=%c type=%c\n", slope->SENS, type );
    avt_log(LOGTRC,2,"slope { F0MAX=%g FCCMAX=%g CCA=%g }\n", slope->F0MAX, slope->FCCMAX, slope->CCA );
    avt_log(LOGTRC,2,"capa=%g\n", extcapa );
  }
  cache = rcx_get_delay_cache() ;

  cacheload = NULL ;

  ptrcx = getrcx(ptsig);

  if( ptrcx && slope->SENS == TRC_SLOPE_UP && ptrcx->DRIVERSUP )
    driver = ptrcx->DRIVERSUP->PNODE ;
  if( ptrcx && slope->SENS == TRC_SLOPE_DOWN && ptrcx->DRIVERSDW )
    driver = ptrcx->DRIVERSDW->PNODE ;
  
  if( cache ) {
 
    mbk_cache_refresh( cache, NULL, ptsig );
    mbk_cache_lock( cache, ptsig );
    cacheload = rcx_get_cache_load( ptsig, 
                                    driver->DATA, 
                                    extcapa, 
                                    model,
                                    slope->SENS, 
                                    type, 
                                    rcx_crosstalkactive( RCX_QUERY )
                                  );
                                  
    if( cacheload ) {
      *r  = cacheload->R ;
      *c1 = cacheload->C1 ;
      *c2 = cacheload->C2 ;
      ret = cacheload->MODELCOMPUTED ;
      avt_log(LOGTRC,2,"get from cache r=%g c1=%g c2=%g\n", *r, *c1, *c2 );
      mbk_cache_unlock( cache, ptsig );
      avt_logexitfunction(LOGTRC,2);
      return ret ;
    }
  }
 
  rcn_lock_signal( lofig, ptsig );

  if( !ptrcx || !GETFLAG( ptrcx->FLAG, MODELRCN ) ) {

    if( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE != ELP_CAPA_LEVEL0 ) {
      avt_log(LOGTRC,2,"error because there is no rcx data\n" );
      avt_errmsg( TRC_ERRMSG, "033", AVT_ERROR, rcx_getsigname( ptsig ) );
    }

    ret = RCX_CAPALOAD ;
    *c1 = rcn_getcapa( lofig, ptsig );
    avt_log(LOGTRC,2,"single capa %g\n", *c1);
    rcn_unlock_signal( lofig, ptsig );
  
    if( cache ) 
      mbk_cache_unlock( cache, ptsig );
    avt_logexitfunction(LOGTRC,2);
    return ret;
  }
 
  if( rcx_isctkbasicmodel( ptrcx ) ) {
    avt_log(LOGTRC,2,"computing load with basic model\n" );
    rcx_setenvbasicmodel();
  }

  avt_log(LOGTRC,2,"computing load\n" );
  ret = rcx_rcnload_basic( lofig, ptrcx, ptsig, driver, r, c1, c2, model, extcapa, slope, type );
  
  if( rcx_isctkbasicmodel( ptrcx ) ) {
    rcx_unsetenvbasicmodel();
  }
  
  if( ret == RCX_LOADERROR ) {
    avt_log(LOGTRC,2,"entering RCX_LOADERROR\n" );
    rcx_setenvbasicmodel();
    ret = rcx_rcnload_basic( lofig, ptrcx, ptsig, driver, r, c1, c2, 
                             RCX_CAPALOAD, extcapa, slope, type );
    rcx_unsetenvbasicmodel();
    if( ret == RCX_LOADERROR ) {
      ret = RCX_CAPALOAD ;
      *c1 = rcn_getcapa( lofig, ptsig );
    }
  }

  avt_log(LOGTRC,2,"r=%g c1=%g c2=%g\n", *r, *c1, *c2 );
  rcn_unlock_signal( lofig, ptsig );
    
  if( cache ) {
    sizeallocated = rcx_add_cache_load( ptsig, 
                                        driver->DATA, 
                                        extcapa, 
                                        model,
                                        ret,
                                        slope->SENS, 
                                        type, 
                                        rcx_crosstalkactive( RCX_QUERY ),
                                       *r,
                                       *c1,
                                       *c2
                                      );

    mbk_cache_update_size( cache, NULL, sizeallocated );
    // trccheckcachecoherence();
    mbk_cache_unlock( cache, ptsig );
  }
    
  avt_logexitfunction(LOGTRC,2);
  return ret ;
}

char rcx_rcnload_basic( lofig_list *lofig,
                        rcx_list   *ptrcx,
                        losig_list *ptsig, 
                        num_list   *driver,
                        RCXFLOAT   *r, 
                        RCXFLOAT   *c1, 
                        RCXFLOAT   *c2,
	                char        model,
                        RCXFLOAT    extcapa,
                        rcx_slope  *slope,
                        char        type
                       )
{
  RCXFLOAT y1, y2, y3 ;
  RCXFLOAT tr, tc1, tc2 ;
  char	testmodel = 0;
  RCXFLOAT resi, capa = -1.0 ;
  RCXFLOAT coefctc;
  RCXFLOAT groundcapa = -1.0e6 ;

  avt_logenterfunction(LOGTRC,2,"rcx_rcnload_basic()");

  // Si on est en mode crosstalk, la charge dépend du front d'entrée.

  rcn_lock_signal( lofig, ptsig );

  if (V_BOOL_TAB[__TRC_FORCE_PILOAD].VALUE)
    model = RCX_PILOAD;

  if( model == RCX_BESTLOAD ) {
    if( GETFLAG( ptrcx->FLAG, RCXLOADCAPA ) ) model = RCX_CAPALOAD;
    if( GETFLAG( ptrcx->FLAG, RCXLOADPI   ) ) model = RCX_PILOAD;
  }

  if( ( rcx_isloop( ptrcx, ptsig ) && RCX_USE_MATRIX_LOAD==NO ) || 
      GETFLAG( ptrcx->FLAG, RCXNOWIRE )                             )
    model = RCX_CAPALOAD;
    
  if( model == RCX_BESTLOAD ) {

    avt_log(LOGTRC,2,"chose the better model\n");
    if( groundcapa < 0.0 ) {
      groundcapa = rcx_getsigcapa( lofig, 
                                   ptsig,
                                   RCX_SIGCAPA_GROUND,
                                   RCX_SIGCAPA_LOCAL,
                                   RCX_SIGCAPA_NORM,
                                   slope,
                                   type,
                                   TRC_HALF
                                 ) ;
      if( rcx_islosigexternal( ptsig ) )
        groundcapa = groundcapa + extcapa ;
    }
  
    resi = rcx_getsumresi( ptsig );
    
    capa = 2 * rcx_getsigcapa( lofig, 
                               ptsig,
                               RCX_SIGCAPA_CTK,
                               RCX_SIGCAPA_LOCAL,
                               RCX_SIGCAPA_NORM,
                               slope,
                               type,
                               TRC_HALF
                             )

           +   rcx_getsigcapa( lofig, 
                               ptsig,
                               RCX_SIGCAPA_CTK,
                               RCX_SIGCAPA_GLOBAL,
                               RCX_SIGCAPA_NORM,
                               slope,
                               type,
                               TRC_HALF
                             )

           + groundcapa;

    if( capa < 0.0 ) {
      model = RCX_LOADERROR ;
      testmodel = 0 ;
    }
    else {
      if( resi*capa < RCX_CAPALOAD_MAXRC )
        model = RCX_CAPALOAD ;	// Modele capacitif pur satisfaisant.
      else
        testmodel = RCX_PILOAD ;	// On essaie autre chose.
    }
  }
  
  if( model == RCX_PILOAD || testmodel == RCX_PILOAD ) {

    avt_log(LOGTRC,2,"computing piload\n" );
    coefctc = rcx_getcoefctc( lofig,
                              ptsig,
                              slope,
                              type,
                              extcapa,
                              &groundcapa
                            );
    if( ( rcx_isloop( ptrcx, ptsig ) || driver->NEXT ) &&
        RCX_USE_MATRIX_LOAD==YES 
      )
      rcx_piload_matrix( ptsig, 
                         driver, 
                         &y1, 
                         &y2, 
                         &y3, 
                         extcapa, 
                         slope, 
                         type, 
                         coefctc 
                       );
    else
      rcx_admittance( ptsig, 
                      driver->DATA, 
                      &y1, &y2, &y3, 
                      extcapa, 
                      slope, 
                      type, 
                      coefctc 
                    );

    if( y2 != 0.0 && y3 != 0.0 ) {
      tr  = -(y3*y3)/(y2*y2*y2);
      tc2 = (y2*y2)/y3;
      tc1 = y1 - tc2 ;
      if( tr < 0.0 || tc1 < 0.0 || tc2 < 0.0 ) {
        avt_errmsg( TRC_ERRMSG, "009", AVT_WARNING, rcx_getsigname( ptsig ) );
        model = RCX_CAPALOAD ;
      }
    }
    else {
      avt_errmsg( TRC_ERRMSG, "009", AVT_WARNING, rcx_getsigname( ptsig ) );
      model = RCX_CAPALOAD ;
      CLEARFLAG( ptrcx->FLAG, RCXLOADPI );
      CLEARFLAG( ptrcx->FLAG, RCXLOADCAPA );
    }
  }

  if( model == RCX_BESTLOAD ) {
    if( ((tr*tc2*tc2)/(tc1+tc2)) > RCX_MAXDELTALOAD )
      model = RCX_PILOAD;
    else
      model = RCX_CAPALOAD;
  }

  if( model == RCX_PILOAD ) {
    *r  = tr ;
    *c1 = tc1 ;
    *c2 = tc2 ;
  }

  if( model == RCX_CAPALOAD ) {
    avt_log(LOGTRC,2,"computing single capa load\n" );

    if( groundcapa < 0.0 ) {
      groundcapa = rcx_getsigcapa( lofig, 
                                   ptsig,
                                   RCX_SIGCAPA_GROUND,
                                   RCX_SIGCAPA_LOCAL,
                                   RCX_SIGCAPA_NORM,
                                   slope,
                                   type,
                                   TRC_HALF
                                 ) ;
      if( rcx_islosigexternal( ptsig ) )
        groundcapa = groundcapa + extcapa ;
    }

    *r  = -1.0 ;
    *c1 = groundcapa + rcx_getsigcapa( lofig,
                                       ptsig,
                                       RCX_SIGCAPA_CTK,
                                       RCX_SIGCAPA_LOCAL,
                                       RCX_SIGCAPA_MILLER,
                                       slope,
                                       type,
                                       TRC_HALF
                                     )
                     + rcx_getsigcapa( lofig,
                                       ptsig,
                                       RCX_SIGCAPA_CTK,
                                       RCX_SIGCAPA_GLOBAL,
                                       RCX_SIGCAPA_DELTA,
                                       slope,
                                       type,
                                       TRC_HALF
                                     );
                                  
    *c2 = -1.0 ;
    if( *c1 < 0.0 ) {
      avt_errmsg( TRC_ERRMSG, "010", AVT_WARNING, rcx_getsigname( ptsig ) );
      *c1 = 0.0 ;
      model = RCX_LOADERROR ;
    }
  }

  if( rcx_crosstalkactive( RCX_QUERY ) == RCX_NOCROSSTALK ) {
  
    if( model == RCX_CAPALOAD ) {
      if( GETFLAG( ptrcx->FLAG, RCXLOADPI ) ) {
        rcx_error(14,ptsig,AVT_ERROR);
        CLEARFLAG( ptrcx->FLAG, RCXLOADPI );
      }
      SETFLAG( ptrcx->FLAG, RCXLOADCAPA );
    }
    else {
      if( GETFLAG( ptrcx->FLAG, RCXLOADCAPA ) ) {
        rcx_error(46,ptsig,AVT_ERROR);
        CLEARFLAG( ptrcx->FLAG, RCXLOADCAPA );
      }
      SETFLAG( ptrcx->FLAG, RCXLOADPI );
    }
  }

  rcn_unlock_signal( lofig, ptsig );
  avt_logexitfunction(LOGTRC,2);
  return( model );
}

/* Renvoie la capacité totale d'un signal */
RCXFLOAT rcx_getsumcapa( lofig_list *lofig, 
                      losig_list    *losig, 
                      rcx_slope     *slope,
                      char           type 
                    )
{
  RCXFLOAT cl;

  cl = rcx_getsigcapa( lofig,
                       losig, 
                       RCX_SIGCAPA_GROUND | RCX_SIGCAPA_CTK,
                       RCX_SIGCAPA_LOCAL,
                       RCX_SIGCAPA_MILLER,
                       slope,
                       type,
                       TRC_HALF
                     );
  return cl;
}

/* Renvoie la somme des résistances sur un signal */
RCXFLOAT rcx_getsumresi( losig_list *ptsig )
{
  lowire_list   	*pwire;
  RCXFLOAT	         r ;
  rcx_list              *rcx;

  rcx = getrcx( ptsig );
  if( GETFLAG(rcx->FLAG, RCXNOWIRE ) )
    return 0.0;

  r=0.0;
  
  for( pwire = ptsig->PRCN->PWIRE ; pwire ; pwire = pwire->NEXT ) 
    r = r + pwire->RESI;

  return r;
}

int rcx_dw_forload( losig_list  *losig,
                    lonode_list *lonode,
                    chain_list  *chwire,
                    ht          *htpack,
                    dataforload *data
                  )
{
  chain_list   *headht ;
  chain_list   *chain ;
  rcx_triangle *tr ;
  yiload       *yd ;
  double        y1, y2, y3 ;
  double        yu1, yu2, yu3 ;
  double        yd1, yd2, yd3 ;
  double        yu11, yu21, yu31 ;
  double        yu12, yu22, yu32 ;
  double        yu1x, yu2x, yu3x ;
  lowire_list  *wire ;
  int           inode ;
  double        r ;
  chain_list   *chrep ;


  headht = GetAllHTElems( htpack );

  y1 = rcx_getnodecapa( losig, 
                        lonode, 
                        data->extcapa, 
                        data->slope, 
                        data->type, 
                        TRC_HALF,
                        data->coefctc 
                      ) ;
  y2 = 0.0 ;
  y3 = 0.0 ;

  for( chain = chwire ; chain ; chain = chain->NEXT ) {
  
    wire = (lowire_list*)chain->DATA ;
    
    inode = ( wire->NODE1 == lonode->INDEX ? wire->NODE2 : wire->NODE1 ) ;
    
    r = rcn_get_resi_para( wire ) ;
    yd = rcx_getadmiforload( data->htyi, inode );
    
    yu1 = yd->Y1 ;
    yu2 = (yd->Y2) - r*(yd->Y1)*(yd->Y1);
    yu3 = r*r*(yd->Y1)*(yd->Y1)*(yd->Y1) + (yd->Y3) - 2.0*r*(yd->Y1)*(yd->Y2);

    y1 = y1 + yu1 ;
    y2 = y2 + yu2 ;
    y3 = y3 + yu3 ;
  }
    
  for( chain = headht ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)((chain_list*)chain->DATA)->DATA ;
    tr = (rcx_triangle*)rcn_getpack( losig, wire );
    
    for( chrep = tr->REPORTED_IN ; chrep ; chrep = chrep->NEXT ) {
    
      inode = ((lonode_list*)chrep->DATA)->INDEX ;
      yd = rcx_getadmiforload( data->htyi, inode );

      y1 = y1 + yd->Y1 ;
      y2 = y2 + yd->Y2 ;
      y3 = y3 + yd->Y3 ;
     
    }

    yd = rcx_getadmiforload( data->htyi, tr->n1->INDEX );
    yd1 = yd->Y1 ;
    yd2 = yd->Y2 ;
    yd3 = yd->Y3 ;
    for( chrep = tr->REPORTED_N1 ; chrep ; chrep = chrep->NEXT ) {
      inode = ((lonode_list*)chrep->DATA)->INDEX ;
      yd = rcx_getadmiforload( data->htyi, inode );
      yd1 = yd1 + yd->Y1 ;
      yd2 = yd2 + yd->Y2 ;
      yd3 = yd3 + yd->Y3 ;
    }
    
    r  = tr->Z1 ;
    yu11 = yd1 ;
    yu21 = yd2 - r*yd1*yd1 ;
    yu31 = r*r*yd1*yd1*yd1 + yd3 - 2.0*r*yd1*yd2 ;
    
    yd = rcx_getadmiforload( data->htyi, tr->n2->INDEX );
    yd1 = yd->Y1 ;
    yd2 = yd->Y2 ;
    yd3 = yd->Y3 ;
    for( chrep = tr->REPORTED_N2 ; chrep ; chrep = chrep->NEXT ) {
      inode = ((lonode_list*)chrep->DATA)->INDEX ;
      yd = rcx_getadmiforload( data->htyi, inode );
      yd1 = yd1 + yd->Y1 ;
      yd2 = yd2 + yd->Y2 ;
      yd3 = yd3 + yd->Y3 ;
    }
    
    r  = tr->Z2 ;
    yu12 = yd1 ;
    yu22 = yd2 - r*yd1*yd1 ;
    yu32 = r*r*yd1*yd1*yd1 + yd3 - 2.0*r*yd1*yd2 ;

    yu1x = yu11 + yu12 ;
    yu2x = yu21 + yu22 ;
    yu3x = yu31 + yu32 ;

    r = tr->Z0 ;
    
    yu1 = yu1x ;
    yu2 = yu2x - r*yu1x*yu1x ;
    yu3 = r*r*yu1x*yu1x*yu1x + yu3x - 2.0*r*yu1x*yu2x;

    y1 = y1 + yu1 ;
    y2 = y2 + yu2 ;
    y3 = y3 + yu3 ;
  }

  
  for( chain = headht ; chain ; chain = chain->NEXT ) {
    wire = (lowire_list*)((chain_list*)chain->DATA)->DATA ;
    tr = (rcx_triangle*)rcn_getpack( losig, wire );
    rcx_cleantriangle_node( losig, lonode, tr );
  }

  rcx_setadmiforload( data->htyi, lonode->INDEX, y1, y2, y3 );

  freechain( headht );
  return 1 ;
}

void rcx_setadmiforload( ht *table, int node, double y1, double y2, double y3 )
{
  yiload *pt ;

  pt = (yiload*)mbkalloc( sizeof( yiload ) );
  pt->Y1 = y1 ;
  pt->Y2 = y2 ;
  pt->Y3 = y3 ;

  addhtitem( table, (void*)(long)node, (long)pt );
}

yiload* rcx_getadmiforload( ht *table, int node )
{
  yiload *pt ;

  pt = (yiload*)gethtitem( table, (void*)(long)node );
  if( (long)pt == EMPTYHT ) rcx_error( 45, NULL, AVT_FATAL );
  
  return pt ;
}

int rcx_up_pack_forload( losig_list *losig,
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

int rcx_up_forload( losig_list  *losig, 
                    lonode_list *lonode,
                    lowire_list *lowire,
                    void        *userdata
                  )
{
  rcx_triangle_node( losig, lonode, lowire );
  userdata = NULL ;
  return 1 ;
}

/* Récupère un développement limité à l'ordre 3 d'un réseau RC.
 * y(p) = y0+y1.p+y2.p^2+y3.p^3
 * y0 est toujours nul car on a pas de resistances à la masse.
 *
 * Arguments :
 *   ptsig :      signal possédant un réseau RC, une table des noeuds et ne
 *                comportant pas de boucles.
 *   src :        index du noeud de départ. Typiquement un connecteur.
 *   y1, y2, y3 : les trois termes.
 *
 * Elements pris en compte :
 *
 * Les résistances,
 * Les capacités à la masse,
 * Les capacités de couplage (coef=1),
 * Les capacités dans les connecteurs ( tlc_getloconparam() ).
*/

void    rcx_admittance( losig_list *ptsig,
                        long        src,
                        RCXFLOAT   *y1,
                        RCXFLOAT   *y2,
                        RCXFLOAT   *y3,
                        RCXFLOAT    extcapa,
                        rcx_slope  *slope,
                        char        type,
                        RCXFLOAT    coefctc
                      )
{
  lonode_list *lonode ;
  dataforload  infos ;
  chain_list  *chain, *toclean ;
  yiload      *yd ;
  lowire_list *wire ;
  
  lonode = getlonode( ptsig, src );

  infos.extcapa = extcapa ;
  infos.coefctc = coefctc ;
  infos.slope   = slope ;
  infos.type    = type ;
  infos.htyi    = addht( ptsig->PRCN->NBNODE );

  clearallwireflag( ptsig, RCN_FLAG_PASS );
  for( wire = ptsig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) 
    RCN_CLEARFLAG( wire->FLAG, RCXNODEINTR );

  rcn_treetrip( ptsig,
                lonode,
        (int (*)(losig_list*,lonode_list*,lowire_list*,void*))rcx_up_forload,
        (int (*)(losig_list*,lonode_list*,chain_list*,ht*,void*))rcx_dw_forload,
        (int (*)(losig_list*,lonode_list*,void*,void*))rcx_up_pack_forload,
                &infos,
                0
              );

  yd = rcx_getadmiforload( infos.htyi, src );
  *y1 = yd->Y1 ;
  *y2 = yd->Y2 ;
  *y3 = yd->Y3 ;

  toclean = GetAllHTElems( infos.htyi );
  for( chain = toclean ; chain ; chain = chain->NEXT )
    mbkfree( chain->DATA );
  freechain( toclean );

  delht( infos.htyi );
}

/*
{
  lonode_list    *ptnode;
  chain_list     *chwire;
  lowire_list    *ptwire;
  RCXFLOAT        yd1, yd2, yd3 ;
  rcx_triangle   *trlist, *tr;
  
  *y1 = 0.0;
  *y2 = 0.0;
  *y3 = 0.0;
  
  ptnode = getlonode( ptsig, src );
  trlist = rcx_triangle_node( ptsig, ptnode, NULL );
 
  for( tr = trlist ; tr ; tr = tr->NEXT ) {
    rcx_triangle_admi( ptsig, tr, &yd1, &yd2, &yd3, 
                       extcapa, slope, type,coefctc 
                     ); 
    *y1 = *y1 + yd1 ;
    *y2 = *y2 + yd2 ;
    *y3 = *y3 + yd3 ;
  }

  for( chwire = ptnode->WIRELIST ; chwire ; chwire = chwire->NEXT )
  {
    ptwire = (lowire_list*)chwire->DATA;
    if( !rcn_getpack( ptsig, ptwire ) ) {
      rcx_recadmi( ptsig, ptwire, src, 
                   &yd1, &yd2, &yd3, 
                   extcapa, slope, type , coefctc
                 );
      *y1 = *y1 + yd1 ;
      *y2 = *y2 + yd2 ;
      *y3 = *y3 + yd3 ;
    }
  }

  rcx_cleantriangle_node( ptsig, ptnode, trlist );
 
  *y1 = *y1 + rcx_getnodecapa( ptsig, ptnode, extcapa, slope, type, coefctc ); 
}
*/

/* Fonction internes recursive du calcul de l'admittance */

void     rcx_recadmi( losig_list  *ptsig,
                      lowire_list *curwire,
                      long         curnode,
                      RCXFLOAT    *y1,
                      RCXFLOAT    *y2,
                      RCXFLOAT    *y3,
                      RCXFLOAT     extcapa,
                      rcx_slope   *slope,
                      char         type,
                      RCXFLOAT     coefctc
                    )
{
  long            node;
  lonode_list    *ptnode;
  lowire_list    *scanwire;
  lowire_list    *w1;
  lowire_list    *w2;
  RCXFLOAT        r,c;
  RCXFLOAT        yu1, yu2, yu3;
  RCXFLOAT        yd1, yd2, yd3;
  rcx_triangle   *trlist, *tr;
 
  if( curwire->NODE1 == curnode )
    node = curwire->NODE2;
  else
    node = curwire->NODE1;

  ptnode = getlonode( ptsig, node );
  
  scanwire = curwire;
  
  while( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_TWO ) == RCN_FLAG_TWO )
  {
    w1 = (lowire_list*)ptnode->WIRELIST->DATA ;
    w2 = (lowire_list*)ptnode->WIRELIST->NEXT->DATA ;

    if( scanwire == w1 )
      scanwire = w2;
    else
      scanwire = w1;

    if( scanwire->NODE1 == node )
      node = scanwire->NODE2;
    else
      node = scanwire->NODE1;

    ptnode = getlonode( ptsig, node );
  }

  yu1 = 0.0;
  yu2 = 0.0;
  yu3 = 0.0;

  if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_CROSS ) )
  {
    chain_list  *chwire;
    lowire_list *pwire;
   
    trlist = rcx_triangle_node( ptsig, ptnode, scanwire );

    for( tr = trlist ; tr ; tr = tr->NEXT ) {
      rcx_triangle_admi( ptsig, tr, &yd1, &yd2, &yd3, 
                         extcapa, slope, type,coefctc 
                       ); 
      yu1 = yu1 + yd1 ;
      yu2 = yu2 + yd2 ;
      yu3 = yu3 + yd3 ;
    }

    for( chwire = ptnode->WIRELIST ; chwire ; chwire = chwire->NEXT )
    {
      pwire = (lowire_list*)chwire->DATA;
      if( pwire != scanwire && !rcn_getpack( ptsig, pwire ) )
      {
        rcx_recadmi( ptsig, pwire, node, 
                     &yd1, &yd2, &yd3, 
                     extcapa, slope, type, coefctc 
                   );
        yu1 = yu1 + yd1;
        yu2 = yu2 + yd2;
        yu3 = yu3 + yd3;
      }  
    }

    rcx_cleantriangle_node( ptsig, ptnode, trlist );
  }

  do
  {
    c = rcx_getnodecapa( ptsig, ptnode, extcapa, slope, type, TRC_HALF, coefctc );
    yu1 = yu1 + c ;
  
    /* Sur la résistance */
    r = scanwire->RESI;

    yd1 = yu1;
    yd2 = yu2;
    yd3 = yu3;

    yu1 = yd1;
    yu2 = yd2-r*yd1*yd1;
    yu3 = r*r*yd1*yd1*yd1+yd3-2*r*yd1*yd2;

    if( scanwire->NODE1 == node )
      node = scanwire->NODE2 ;
    else
      node = scanwire->NODE1 ;
    ptnode = getlonode( ptsig, node );

    if( node != curnode )
    {
      w1 = (lowire_list*)ptnode->WIRELIST->DATA ;
      w2 = (lowire_list*)ptnode->WIRELIST->NEXT->DATA ;

      if( scanwire == w1 )
        scanwire = w2;
      else
        scanwire = w1;
    }
  }
  while( node != curnode );

  *y1 = yu1;
  *y2 = yu2;
  *y3 = yu3;
}

void     rcx_triangle_admi( losig_list  *ptsig,
                            rcx_triangle *triangle,
                            RCXFLOAT    *y1,
                            RCXFLOAT    *y2,
                            RCXFLOAT    *y3,
                            RCXFLOAT     extcapa,
                            rcx_slope   *slope,
                            char         type,
                            RCXFLOAT     coefctc
                          )
{
  RCXFLOAT      r;
  RCXFLOAT      y11, y12, y13 ;
  RCXFLOAT      y21, y22, y23 ;
  RCXFLOAT      yd1, yd2, yd3 ;
 
  y11 = 0.0 ; y12 = 0.0 ; y13 = 0.0 ;
  y21 = 0.0 ; y22 = 0.0 ; y23 = 0.0 ;

  /* Branche 1 du triangle */
  rcx_admittance( ptsig,
                  triangle->n1->INDEX,
                  &yd1,
                  &yd2,
                  &yd3,
                   extcapa,
                   slope,
                   type,
                   coefctc
                );

  r = triangle->Z1;

  y11 = y11 + yd1;
  y12 = y12 + yd2-r*yd1*yd1;
  y13 = y13 + r*r*yd1*yd1*yd1+yd3-2*r*yd1*yd2;

  /* Branche 2 du triangle */
  rcx_admittance( ptsig,
                  triangle->n2->INDEX,
                  &yd1,
                  &yd2,
                  &yd3,
                   extcapa,
                   slope,
                   type,
                   coefctc
                );

  r = triangle->Z2;

  y21 = y21 + yd1;
  y22 = y22 + yd2-r*yd1*yd1;
  y23 = y23 + r*r*yd1*yd1*yd1+yd3-2*r*yd1*yd2;

  /* Tronçon commun */
  
  r = triangle->Z0;
  
  yd1 = y11+y21;
  yd2 = y12+y22;
  yd3 = y13+y23;
  
  *y1 = yd1;
  *y2 = yd2-r*yd1*yd1;
  *y3 = r*r*yd1*yd1*yd1+yd3-2*r*yd1*yd2;

}

void rcx_forcercdelay(losig_list *losig)
{
  rcx_list   *rcx;
  rcx = getrcx( losig );
  if( rcx )
    SETFLAG( rcx->FLAG, RCXFORCERC);
}

int rcx_isrcdelay( lofig_list *lofig, losig_list *losig )
{
  RCXFLOAT capa=0.0;
  RCXFLOAT resi=0.0;
  float    cm, ccg, ccl;
  ptype_list *ptl;
  locon_list *locon;
  chain_list *chain;
  rcx_list   *rcx;

  rcx = getrcx( losig );
  if( !rcx ) return 0;
  if (GETFLAG( rcx->FLAG, RCXFORCERC))
    return 1;

  if( GETFLAG( rcx->FLAG, RCXNOWIRE ) )
    return 0;
  
  // On commence par regarder les instances RCX pour eviter de faire un
  // refresh
  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( ptl ) {
    for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)chain->DATA;
      if( !rcx_isvalidlocon( locon ) )
        continue;
      resi = resi + rcx_getloinsresi( locon );
      rcx_getloinscapa( locon, &cm, &ccl, &ccg );
      capa = capa + cm + 2*ccl + ccg;
    }
  }

  if( rcx_islosigexternal( losig ) ) {
    if( resi != 0.0 )
      return 1;
  }

  if( resi * capa >= RCX_MINRCSIGNAL )
    return 1 ;
 
  rcn_lock_signal( lofig, losig );

  resi = resi + rcx_getsumresi( losig );

  if( rcx_islosigexternal( losig ) ) {
    rcn_unlock_signal( lofig, losig );
    if( resi == 0.0 )
      return 0;
    return 1;
  }
  
  capa = capa +
         rcx_getsigcapa( lofig,
                         losig,
                         RCX_SIGCAPA_GROUND,
                         RCX_SIGCAPA_LOCAL,
                         RCX_SIGCAPA_NORM,
                         NULL,
                         RCX_MAX,
                         TRC_HALF
                       ) +
         2 * rcx_getsigcapa( lofig,
                             losig,
                             RCX_SIGCAPA_CTK,
                             RCX_SIGCAPA_LOCAL,
                             RCX_SIGCAPA_NORM,
                             NULL,
                             RCX_MAX,
                             TRC_HALF
                           ) +
         rcx_getsigcapa( lofig,
                         losig,
                         RCX_SIGCAPA_CTK,
                         RCX_SIGCAPA_GLOBAL,
                         RCX_SIGCAPA_NORM,
                         NULL,
                         RCX_MAX,
                         TRC_HALF
                       );
 
  rcn_unlock_signal( lofig, losig );

  if( resi * capa < RCX_MINRCSIGNAL )
    return(0);

  return(1);
}

int rcx_iscrosstalk( lofig_list *lofig, losig_list *losig, char test )
{
  rcx_list *rcx;
  RCXFLOAT    ccl=0.0,  ccg=0.0,  cm=0.0;
  float       tccl, tccg, tcm;
  ptype_list *ptl;
  locon_list *locon;
  chain_list *chain;
  int         ret;

  rcx = getrcx( losig );
  if( GETFLAG( rcx->FLAG, RCXNOCTC ) )
    return 0;

  rcn_lock_signal( lofig, losig );

  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( ptl ) {
    for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
      locon = (locon_list*)chain->DATA;
      if( !rcx_isvalidlocon( locon ) )
        continue;
      rcx_getloinscapa( locon, &tcm, &tccl, &tccg );
      cm  = cm  + tcm;
      ccl = ccl + tccl;
      ccg = ccg + tccg;
    }
  }

  ccl = ccl + rcx_getsigcapa( lofig,
                              losig,
                              RCX_SIGCAPA_CTK,
                              RCX_SIGCAPA_LOCAL,
                              RCX_SIGCAPA_NORM,
                              NULL,
                              RCX_MAX,
                              TRC_HALF
                            );
  
  ccg = ccg + rcx_getsigcapa( lofig,
                              losig,
                              RCX_SIGCAPA_CTK,
                              RCX_SIGCAPA_GLOBAL,
                              RCX_SIGCAPA_NORM,
                              NULL,
                              RCX_MAX,
                              TRC_HALF
                            );
                     
  cm = cm + rcx_getsigcapa( lofig,
                            losig,
                            RCX_SIGCAPA_GROUND,
                            RCX_SIGCAPA_LOCAL,
                            RCX_SIGCAPA_NORM,
                            NULL,
                            RCX_MAX,
                            TRC_HALF
                          );

  cm = cm - ccg;
  if( cm < 0.0 ) {
    avt_errmsg( TRC_ERRMSG, "007", AVT_ERROR, rcx_getsigname( losig ), 1 );
    return 0 ;
  }

  switch( test ) {
  case RCX_TEST_LIMIT :
    if( (ccl+ccg)/(ccl+ccg+cm) < RCX_MINRELCTKSIGNAL ) 
      ret=0;
    else
      ret=1;
    break;
  case RCX_TEST_ONE :
    if( ccl + ccg > 0.0 )
      ret=1;
    else
      ret=0;
    break;
  }

  rcn_unlock_signal( lofig, losig );
  return ret;
}

/******************************************************************************\
rcx_checkcrosstalk()
- Utilise un flag RCXTREATEDNI afin de ne pas effectuer cette opération 
  inutilement.
- Marque les agresseurs ayant une importance relative inférieure à
  RCX_MINRELCTKFILTER comme étant Non Influent.
\******************************************************************************/

void rcx_checkcrosstalk( losig_list *losig )
{
  chain_list    *scanctc;
  loctc_list    *loctc;
  losig_list    *sigagr;
  ptype_list    *ptl;
  long           maxagr=0;
  losig_list   **tabsig;
  RCXFLOAT      *tabcapa;
  RCXFLOAT      *tabsum;
  long           i,j;
  long           refmin;
  RCXFLOAT       capa;
  rcx_list      *rcx;
 
  rcx = getrcx( losig );
  if( GETFLAG( rcx->FLAG, RCXTREATEDNI ) )
    return;
  SETFLAG( rcx->FLAG, RCXTREATEDNI );

  // Récupère le nombre d'agresseurs
  
  for( scanctc = losig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
  
    loctc = (loctc_list*)(scanctc->DATA);
    if( rcx_iscrosstalkcapa( loctc, losig ) ) {
      sigagr = rcn_ctcothersig( loctc, losig );
      ptl = getptype( sigagr->USER, RCX_AGREINDEX_CHK );
      if( !ptl ) {
        sigagr->USER = addptype( sigagr->USER, RCX_AGREINDEX_CHK, (void*)maxagr );
        maxagr++;
      }
    }
  }

  if( maxagr == 0 )
    return;

  // Création et remplissage des tableaux

  tabsig = (losig_list**)mbkalloc( sizeof( losig_list*) * maxagr );
  tabcapa = (RCXFLOAT*)mbkalloc( sizeof( RCXFLOAT ) * maxagr );
  tabsum = (RCXFLOAT*)mbkalloc( sizeof( RCXFLOAT ) * maxagr );
  for( i=0 ; i<maxagr ; i++ ) {
    tabsig[i]=NULL;
    tabcapa[i]=0.0;
    tabsum[i]=0.0;
  }
  
  for( scanctc = losig->PRCN->PCTC ; scanctc ; scanctc = scanctc->NEXT ) {
  
    loctc = (loctc_list*)(scanctc->DATA);
    sigagr = rcn_ctcothersig( loctc, losig );

    ptl = getptype( sigagr->USER, RCX_AGREINDEX_CHK );
    if( ptl ) {
      tabsig[ (long)ptl->DATA ]  =  sigagr;
      tabcapa[ (long)ptl->DATA ] += loctc->CAPA;
    }
  }

  // Trie des tableaux suivant la valeur des capacité : trie à bulle...

  for( i=0 ; i<maxagr ; i++ ) {
    refmin=i;
    for( j=i+1 ; j<maxagr ; j++ ) {
      if( tabcapa[j] < tabcapa[refmin] ) 
        refmin = j;
    }
    if( refmin != i ) {
      sigagr = tabsig[refmin] ;
      tabsig[refmin] = tabsig[i] ;
      tabsig[i] = sigagr;
      
      capa = tabcapa[refmin] ;
      tabcapa[refmin] = tabcapa[i] ;
      tabcapa[i] = capa;
    }
  }

  // Rempli le tableau somme

  tabsum[0] = tabcapa[0];
  for( i=1 ; i<maxagr ; i++ )
    tabsum[i]=tabsum[i-1]+tabcapa[i];
 
  
  // Marque les capacités non significatives

  for( i=0 ; i<maxagr ; i++ ) {
    if( tabsum[i] / rcn_getcapa(NULL,losig) < RCX_MINRELCTKFILTER ) {
      rcx_setnotinfluentagressor( losig, tabsig[i] );
    }
  }
  
  /*
  {
    FILE *ptf;
    ptf = fopen( "tableau.dat", "w" );
    if( !ptf ) {
      perror( "mbkfopen " );
      EXIT(1);
    }
    for( i=0 ; i<maxagr ; i++ ) {
      fprintf( ptf, "%ld %e\n", i, tabsum[i] );
    }
    fclose( ptf );

    system( "gnuplot afftableau.gplt" );
  }
  */
 
  // Mr Propre
  
  for( i=0 ; i<maxagr ; i++ ) {
    tabsig[i]->USER = delptype( tabsig[i]->USER, RCX_AGREINDEX_CHK );
  }
  mbkfree( tabsig );
  mbkfree( tabcapa );
  mbkfree( tabsum );
}

/* Facteur multiplicatif à appliquer aux capacités pour prendre en compte
l'effet global des capacités. Si on a pas déjà calculé groundcapa, il faut
mettre une valeur négative. */

RCXFLOAT rcx_getcoefctc( lofig_list        *lofig,
                         losig_list        *ptsig,
                         rcx_slope         *slope,
                         char               type,
                         RCXFLOAT           extcapa,
                         RCXFLOAT          *ptgroundcapa
                       )
{
  RCXFLOAT deltaglobalctc;
  RCXFLOAT coefctc;
  RCXFLOAT groundcapa;

  if( ptgroundcapa )
    groundcapa = *ptgroundcapa;
  else
    groundcapa = -1.0;
    
  if( rcx_crosstalkactive( RCX_QUERY ) == RCX_MILLER ) {
    deltaglobalctc = rcx_getsigcapa( lofig,
                                     ptsig, 
                                     RCX_SIGCAPA_CTK,
                                     RCX_SIGCAPA_GLOBAL,
                                     RCX_SIGCAPA_DELTA,
                                     slope,
                                     type ,
                                     TRC_HALF
                                   );

    if( deltaglobalctc != 0.0 ) {
      if( groundcapa < 0.0 ) {
        groundcapa = rcx_getsigcapa( lofig, 
                                     ptsig,
                                     RCX_SIGCAPA_GROUND,
                                     RCX_SIGCAPA_LOCAL,
                                     RCX_SIGCAPA_NORM,
                                     slope,
                                     type,
                                     TRC_HALF
                                   ) ;
        if( rcx_islosigexternal( ptsig ) )
          groundcapa = groundcapa + extcapa ;

        if( ptgroundcapa ) *ptgroundcapa = groundcapa;
      }

      coefctc    = 1.0 + deltaglobalctc / groundcapa ;
    }
    else
      coefctc    = 1.0 ;
  }
  else
    coefctc = 1.0;

  return coefctc;
}

mbkcache* rcx_get_delay_cache( void )
{
  mbkcache *cache = NULL ;

  switch( rcx_crosstalkactive( RCX_QUERY ) ) {
  
  case RCX_MILLER :
    if( rcx_is_delay_cache_for_crosstalk() )
      cache = RCXDELAYCACHE ;
    break ;
    
  case RCX_NOCROSSTALK :
    if( rcx_is_delay_cache_for_quiet() )
      cache = RCXDELAYCACHE ;
    break ;

  default :
    rcx_error( 39, NULL, AVT_ERROR );
  }
  
  return cache ;
}

void rcx_init_delay_cache( unsigned long int size )
{
  if( RCXDELAYCACHE ) {
    mbk_cache_delete( RCXDELAYCACHE, NULL );
    RCXDELAYCACHE = NULL ;
  }

  if( size )
    RCXDELAYCACHE = mbk_cache_create( 
                        (char(*)(void*,void*))NULL, 
                        (unsigned long int(*)(void*,void*))rcx_cache_fn_load,
                        (unsigned long int(*)(void*,void*))rcx_cache_fn_release,
                        size 
                                    );
}

unsigned long int rcx_cache_fn_release( void *null, losig_list *losig )
{
  ptype_list *ptl ;
  ptype_list *elem ;
  unsigned long int sizereleased = 0 ;
  
  avt_logenterfunction(LOGTRC,2,"rcx_cache_fn_release()");

  if( !losig ) {
    avt_errmsg( TRC_ERRMSG, "046", AVT_ERROR );
    avt_logexitfunction(LOGTRC,2);
    return 0;
  }

  ptl = getptype( losig->USER, RCX_DELAYCACHE );
  if( !ptl ) {
    avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
    avt_logexitfunction(LOGTRC,2);
    return 0;
  }

  if (avt_islog(2,LOGTRC))
    avt_log(LOGTRC,2,"losig = %s\n", rcx_getsigname( losig ) );

  for( elem = (ptype_list*)ptl->DATA ; elem ; elem = elem->NEXT ) {
   switch( rcx_cache_delay_model( elem->TYPE ) ) {
   case RCX_MODEL_LOAD  :
     sizereleased += rcx_cache_release_load( (rcx_cache_load*)elem->DATA );
     break ;
   case RCX_MODEL_AWE :
     sizereleased += rcx_cache_release_awe( (awecache*)elem->DATA );
     break ;
   default :
     avt_errmsg( TRC_ERRMSG, "048", AVT_ERROR, rcx_getsigname( losig ) );
     break ;
   }
   sizereleased += sizeof( ptype_list );
  }

  freeptype( (ptype_list*)ptl->DATA );
  losig->USER = delptype( losig->USER, RCX_DELAYCACHE );
  sizereleased += sizeof( ptype_list );
  null=NULL;

  avt_logexitfunction(LOGTRC,2);
  return sizereleased ;
}

unsigned long int rcx_cache_release_load( rcx_cache_load *head )
{
  unsigned long int torelease = 0;
  rcx_cache_load    *next ;

  while( head ) {
    torelease += sizeof( rcx_cache_load );
    next = head->NEXT ;
    mbkfree( head );
    head = next ;
  }
  return torelease ;
}

unsigned long int rcx_cache_fn_load( void *null, losig_list *losig )
{
  ptype_list *ptl ;

  if( !losig ) {
    avt_errmsg( TRC_ERRMSG, "046", AVT_ERROR );
    return 0 ;
  }
    
  ptl = getptype( losig->USER, RCX_DELAYCACHE );
  if( ptl ) {
    avt_errmsg( TRC_ERRMSG, "045", AVT_ERROR, rcx_getsigname( losig ) );
  }
  else
    losig->USER = addptype( losig->USER, RCX_DELAYCACHE, NULL );

  null=NULL ;

  return sizeof( ptype_list );
}

unsigned long int  rcx_add_cache_load( losig_list *losig, 
                                       int         pin, 
                                       float       extcapa, 
                                       char        modelrequest,
                                       char        modelcomputed,
                                       char        transition,
                                       char        type, 
                                       char        iscrosstalk,
                                       float       r,
                                       float       c1,
                                       float       c2
                                     )
{
  rcx_cache_load    *load ;
  ptype_list        *ptl ;
  ptype_list        *ptype ;
  long int           ntype ;
  unsigned long int  size_allocated = 0 ;

  avt_logenterfunction(LOGTRC,2,"rcx_add_cache_load()" );

  ptl = getptype( losig->USER, RCX_DELAYCACHE );
  if( !ptl ) {
    avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
    avt_log(LOGTRC,2,"no ptype\n");
    avt_logexitfunction(LOGTRC,2);
    return size_allocated ;
  }
  
  load = mbkalloc( sizeof( rcx_cache_load ) );
  size_allocated += sizeof(rcx_cache_load);
  
  load->PIN           = pin ;
  load->EXTCAPA       = extcapa ;
  load->MODELREQUEST  = modelrequest  ;
  load->MODELCOMPUTED = modelcomputed ;
  load->R             = r ;
  load->C1            = c1 ;
  load->C2            = c2 ;

  ntype =
         rcx_cache_build_ptype( transition, type, iscrosstalk, RCX_MODEL_LOAD );

  if (avt_islog(2,LOGTRC))
    avt_log(LOGTRC,2,"signal %s\n", rcx_getsigname( losig ) );
  trc_logbuildtype( ntype );
  if (avt_islog(2,LOGTRC))
    avt_log(LOGTRC,2,"pin=%d capa=%g r=%g c1=%g c2=%g\n",
           load->PIN,
           load->EXTCAPA,
           load->R,
           load->C1,
           load->C2
         );

  for( ptype = (ptype_list*)ptl->DATA ; ptype ; ptype = ptype->NEXT )
    if( ptype->TYPE == ntype )
      break ;

  if( ptype ) {
    load->NEXT  = (rcx_cache_load*)ptype->DATA ;
    ptype->DATA = load ;
    avt_log(LOGTRC,2,"added on an existing chain\n");
  }
  else {
    load->NEXT = NULL ;
    ptl->DATA = addptype( ptl->DATA, ntype, load );
    size_allocated += sizeof( ptype_list );
    avt_log(LOGTRC,2,"added on a new chain\n");
  }

  avt_logexitfunction(LOGTRC,2);
  return size_allocated ;
}

rcx_cache_load* rcx_get_cache_load( losig_list *losig,
                                    int         pin,
                                    float       extcapa,
                                    char        model,
                                    char        transition,
                                    char        type,
                                    char        iscrosstalk
                                  )
{
  ptype_list     *ptl ;
  ptype_list     *ptype ;
  long int        ntype ;
  rcx_cache_load *load ;
  rcx_cache_load *retload=NULL ;

  avt_logenterfunction(LOGTRC,2,"rcx_get_cache_load()");

  ptl = getptype( losig->USER, RCX_DELAYCACHE );
  if( !ptl ) {
    avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
    avt_log(LOGTRC,2,"no ptype found\n");
    avt_logexitfunction(LOGTRC,2);
    return NULL ;
  }

  ntype = 
         rcx_cache_build_ptype( transition, type, iscrosstalk, RCX_MODEL_LOAD );

  if (avt_islog(2,LOGTRC))
    avt_log(LOGTRC,2,"signal %s\n", rcx_getsigname( losig ) );
  trc_logbuildtype( ntype );

  for( ptype = (ptype_list*)ptl->DATA ; ptype ; ptype = ptype->NEXT ) {
    if( ptype->TYPE == ntype ) 
      break ;
  }

  if( ptype ) {
    for( load = (rcx_cache_load*)ptype->DATA ; load ; load = load->NEXT ) {
      if( load->PIN     == pin     &&
          load->EXTCAPA == extcapa &&
          ( load->MODELREQUEST  == model ||
            load->MODELCOMPUTED == model    ) ) {
        retload = load ;
        avt_log(LOGTRC,2,"pin=%d capa=%g r=%g c1=%g c2=%g\n",
                 load->PIN,
                 load->EXTCAPA,
                 load->R,
                 load->C1,
                 load->C2
               );
        break ;
      }
    }
  }

  avt_logexitfunction(LOGTRC,2);
  return retload ;
}

char rcx_cache_delay_model( long int ptype )
{
  return ptype & 0x000000FF ;
}

char rcx_cache_delay_crosstalk( long int ptype )
{
  return (ptype>>8) & 0x000000FF ;
}

char rcx_cache_delay_type( long int ptype )
{
  return (ptype>>16) & 0x000000FF ;
}

char rcx_cache_delay_transition( long int ptype )
{
  return (ptype>>24) & 0x000000FF ;
}

long int rcx_cache_build_ptype( char transition, 
                                char type, 
                                char iscrosstalk, 
                                char model
                              )
{
  long int ptype ;

  if( iscrosstalk == RCX_NOCROSSTALK ) {
    switch( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE ) {
    case ELP_CAPA_LEVEL0 :
      transition = 0 ;
      if( RCX_CAPA_NOM_MIN == RCX_CAPA_NOM_MAX )
        type = 0;
      break ;
    case ELP_CAPA_LEVEL1 :
    case ELP_CAPA_LEVEL2 :
      if( RCX_CAPA_UP_NOM_MIN == RCX_CAPA_UP_NOM_MAX && 
          RCX_CAPA_DW_NOM_MIN == RCX_CAPA_DW_NOM_MAX    )
        type = 0;
      break ;
    }
  }
  
  ptype = ((unsigned char)transition  ) << 24 |
          ((unsigned char)type        ) << 16 |
          ((unsigned char)iscrosstalk ) <<  8 |
          ((unsigned char)model       )         ;
 
  return ptype ;
}

void rcx_disable_delay_cache_for_crosstalk( void )
{
  RCXDELAYCACHEFORCROSSTALK = NO ;
  rcx_delay_cache_remove_from_cache( RCX_MILLER );
}

void rcx_enable_delay_cache_for_crosstalk( void )
{
  RCXDELAYCACHEFORCROSSTALK = YES ;
}

int rcx_is_delay_cache_for_crosstalk( void )
{
  if( RCXDELAYCACHEFORCROSSTALK == YES )
    return 1 ;
  return 0 ;
}

void rcx_disable_delay_cache_for_quiet( void )
{
  RCXDELAYCACHEFORQUIET = NO ;
  rcx_delay_cache_remove_from_cache( RCX_NOCROSSTALK );
}

void rcx_enable_delay_cache_for_quiet( void )
{
  RCXDELAYCACHEFORQUIET = YES ;
}

int rcx_is_delay_cache_for_quiet( void )
{
  if( RCXDELAYCACHEFORQUIET == YES )
    return 1 ;
  return 0 ;
}

void rcx_delay_cache_remove_from_cache( char ctkmode )
{
  long int           sizereleased ;
  chain_list        *head ;
  chain_list        *chain ;
  losig_list        *losig ;
  ptype_list        *ptl ;
  ptype_list        *prev ;
  chain_list        *torelease ;
  ptype_list        *ptype ;
  ptype_list        *next ;
  
  if( RCXDELAYCACHE ) {

    avt_logenterfunction(LOGTRC,2,"rcx_delay_cache_remove_from_cache()");
 
    sizereleased = 0 ;
    head = mbk_cache_list_content( RCXDELAYCACHE );
    torelease = NULL ;
    
    for( chain = head ; chain ; chain = chain->NEXT ) {
    
      losig = (losig_list*)chain->DATA ;

      ptl = getptype( losig->USER, RCX_DELAYCACHE );
      
      if( !ptl ) {
        avt_errmsg( TRC_ERRMSG, "047", AVT_ERROR, rcx_getsigname( losig ) );
        continue ;
      }

      prev = NULL ;
      
      for( ptype = (ptype_list*)ptl->DATA ; ptype ; ptype = next ) {
     
        next = ptype->NEXT ;
        
        if( rcx_cache_delay_crosstalk( ptype->TYPE ) == ctkmode ) {
          if( prev )
            prev->NEXT = next ;
          else
            ptl->DATA  = next ;
        
          if (avt_islog(2,LOGTRC))
            avt_log(LOGTRC,2,"removing %s\n", rcx_getsigname( losig ) );
          trc_logbuildtype( ptype->TYPE );

          switch( rcx_cache_delay_model( ptype->TYPE ) ) {
          case RCX_MODEL_LOAD :
            sizereleased = sizereleased + rcx_cache_release_load( ptype->DATA );
            break;
          case RCX_MODEL_AWE :
            sizereleased = sizereleased + rcx_cache_release_awe( ptype->DATA );
            break;
          default :
            avt_errmsg( TRC_ERRMSG, "048", AVT_ERROR, rcx_getsigname( losig ) );
            break ;
          }
          ptype->NEXT = NULL ;
          freeptype( ptype );
          sizereleased = sizereleased + sizeof( ptype_list );
        }
      }
      if( !ptl->DATA )
        torelease = addchain( torelease, losig );
    }

    freechain( head );
    mbk_cache_update_size( RCXDELAYCACHE, NULL, -sizereleased );

    for( chain = torelease ; chain ; chain = chain->NEXT ) {
      losig = (losig_list*)chain->DATA ;
      mbk_cache_release( RCXDELAYCACHE, NULL, losig );
    }

    freechain( torelease );

    avt_logexitfunction(LOGTRC,2);
  }

  // trccheckcachecoherence();
}

void trcflushdelaycache()
{
  rcx_delay_cache_remove_from_cache( RCX_MILLER );
  rcx_delay_cache_remove_from_cache( RCX_NOCROSSTALK );
}

void rcx_crosstalk_analysis( char mode )
{
  mbkcache *cache ;
 
  cache = rcx_get_delay_cache() ;
  if( !cache )
    return ;
  
  switch( mode ) {
  case YES :
    switch( RCX_CTK_SLOPE_MILLER ) {
    case RCX_SLOPE_NOMINAL :
      mbk_cache_set_limit_element( cache, NULL, 0 );
      break ;
    case RCX_SLOPE_CTK :
      rcx_disable_delay_cache_for_crosstalk();
      break ;
    }
    break ;
  case NO :
    rcx_enable_delay_cache_for_crosstalk();
    mbk_cache_set_limit_element( cache, NULL, 0 );
    break ;
  }
}

void trccheckcachecoherence()
{
  mbkcache          *cache ;
  chain_list        *head, *scan ;
  ptype_list        *ptl ;
  ptype_list        *ptype ;
  rcx_cache_load    *load;
  aweinfo_list      *awe;
  unsigned long int  cursize=0;
  losig_list        *losig ;
  awecache          *awetop ;
  int                n;
  int                nht=0;
  chain_list        *chainht ;
  chain_list        *scanht ;
  chain_list        *count ;
  
  
  cache = rcx_get_delay_cache() ;
  if( !cache )
    return ;

  head = mbk_cache_list_content(cache) ;
  
  for( scan = head ; scan ; scan = scan->NEXT ) {
    losig = (losig_list*)scan->DATA ;
    ptl = getptype( losig->USER, RCX_DELAYCACHE );
    if( !ptl ) {
      printf( "cache coherence problem\n" );
      continue ;
    }
    cursize+=sizeof(ptype_list);

    for( ptype = (ptype_list*)ptl->DATA ; ptype ; ptype = ptype->NEXT ) {
      cursize+=sizeof(ptype_list);
      switch( rcx_cache_delay_model(ptype->TYPE) ) {
      case RCX_MODEL_LOAD:
        for( load = (rcx_cache_load*)ptype->DATA ; load ; load = load->NEXT) 
          cursize+=sizeof(rcx_cache_load);
        break ;
      case RCX_MODEL_AWE:
        awetop = (awecache*)ptype->DATA ;
        cursize += sizeof( awecache );
        n=0;
        for( awe = awetop->HEAD ; awe ; awe = awe->NEXT) {
          cursize+=sizeof(aweinfo_list);
          n++;
        }
        if( n != awetop->NB )
          printf( "bad number of element stored\n" );
        if( awetop->HT ) {
          nht++;
          chainht = GetAllHTElems( awetop->HT );
            for( scanht = chainht ; scanht ; scanht = scanht->NEXT ) {
              for( count = (chain_list*)scanht->DATA ; count ; count = count->NEXT )
                cursize += sizeof( chain_list ) ;
            }
          freechain( chainht );
        }
        break ;
      default :
        printf( "unknown model\n");
        break;
      }
    }
  }
  
  freechain( head );

  if( cache->CURSIZE != cursize ) {
    printf( "size missmatch ! cache=%lu data=%lu\n", cache->CURSIZE, cursize );
  }

  printf( "cache size = %lu (awe ht=%d)\n", cursize, nht );
}


void trc_logbuildtype( long int ntype )
{
  char model[32], ctk[32], type[32], trans[32];

  if( !avt_islog( LOGTRC, 2 ) )
    return ;

  switch( rcx_cache_delay_model( ntype ) ) {
  case RCX_MODEL_LOAD :
    sprintf( model, "load" );
    break ;
  case RCX_MODEL_AWE :
    sprintf( model, "awe" );
    break ;
  default :
    sprintf( model, "unk" );
    break ;
  }

  switch( rcx_cache_delay_crosstalk( ntype ) ) {
  case RCX_MILLER :
    sprintf( ctk, "yes" );
    break ;
  case RCX_NOCROSSTALK :
    sprintf( ctk, "no" );
    break ;
  default :
    sprintf( ctk, "unk" );
    break ;
  }

  switch( rcx_cache_delay_type( ntype ) ) {
  case RCX_MAX :
    sprintf( type, "max" );
    break ;
  case RCX_MIN :
    sprintf( type, "min" );
    break ;
  default :
    sprintf( type, "unk" );
    break ;
  }

  switch( rcx_cache_delay_transition( ntype ) ) {
  case TRC_SLOPE_UP :
    sprintf( trans, "up" );
    break ;
  case TRC_SLOPE_DOWN :
    sprintf( trans, "down" );
    break ;
  default :
    sprintf( trans, "unk" );
    break ;
  }

  avt_log(LOGTRC,2,"model=%s ctk=%s type=%s trans=%s\n", model, ctk, type, trans );
}

void delaybetween( lofig_list *lofig, locon_list *start, locon_list *end, int display )
{
  rcx_slope slope ;
  RCXFLOAT  delay, slew ;
  slope.F0MAX  =  STM_DEF_SLEW ;
  slope.FCCMAX =  STM_DEF_SLEW ;
  slope.SENS   =  TRC_SLOPE_UP ;
  slope.CCA    =  -1.0 ;
  slope.MAX.slope = STM_DEF_SLEW ;
  slope.MAX.vend  = stm_mod_default_vdd() ;
  slope.MAX.vt    = stm_mod_default_vt() ;
  slope.MAX.vdd   = stm_mod_default_vdd() ;
  slope.MAX.vsat  = -1.0 ;
  slope.MAX.rlin  = -1.0 ;
  slope.MAX.vth   = stm_mod_default_vth();
  slope.MAX.r     = -1.0 ;
  slope.MAX.c1    = -1.0 ;
  slope.MAX.c2    = -1.0 ;
  slope.MAX.pwl   = NULL ;
  slope.MIN.slope = -1.0 ;
  slope.MIN.vend  = -1.0 ;
  slope.MIN.vt    = -1.0 ;
  slope.MIN.vdd   = -1.0 ;
  slope.MIN.vsat  = -1.0 ;
  slope.MIN.rlin  = -1.0 ;
  slope.MIN.vth   = -1.0 ;
  slope.MIN.r     = -1.0 ;
  slope.MIN.c1    = -1.0 ;
  slope.MIN.c2    = -1.0 ;
  slope.MIN.pwl   = NULL ;
  
  if( rcx_getdelayslope( lofig, start, end, &slope, 0.0, 
      &delay, NULL, &slew, NULL, NULL, NULL )==0 
    )
    return ;

  if( display )
    printf( "    to %s : delay=%.1f slew=%.1f\n", 
            getloconrcxname(end), delay, slew 
          );
  
}

int alldelayfromlocon( lofig_list *lofig, locon_list *locon, int display )
{
  losig_list    *losig ;
  rcx_list      *rcx ;
  chain_list    *chain ;
  locon_list    *dest ;
  double         r, c1, c2 ;
  rcx_slope      slope ;
  char           type ;

  slope.F0MAX  =  STM_DEF_SLEW ;
  slope.FCCMAX =  STM_DEF_SLEW ;
  slope.SENS   =  TRC_SLOPE_UP ;
  slope.CCA    =  -1.0 ;
  slope.MAX.slope = STM_DEF_SLEW ;
  slope.MAX.vend  = stm_mod_default_vdd() ;
  slope.MAX.vt    = stm_mod_default_vt() ;
  slope.MAX.vdd   = stm_mod_default_vdd() ;
  slope.MAX.vsat  = -1.0 ;
  slope.MAX.rlin  = -1.0 ;
  slope.MAX.vth   = stm_mod_default_vth();
  slope.MAX.r     = -1.0 ;
  slope.MAX.c1    = -1.0 ;
  slope.MAX.c2    = -1.0 ;
  slope.MAX.pwl   = NULL ;
  slope.MIN.slope = -1.0 ;
  slope.MIN.vend  = -1.0 ;
  slope.MIN.vt    = -1.0 ;
  slope.MIN.vdd   = -1.0 ;
  slope.MIN.vsat  = -1.0 ;
  slope.MIN.rlin  = -1.0 ;
  slope.MIN.vth   = -1.0 ;
  slope.MIN.r     = -1.0 ;
  slope.MIN.c1    = -1.0 ;
  slope.MIN.c2    = -1.0 ;
  slope.MIN.pwl   = NULL ;
  
  losig = locon->SIG ;
  rcx = getrcx( losig ) ;
  if( !rcx )
    return 1 ;

  printf( "  delay from %s\n", getloconrcxname( locon ) );
  fflush( stdout );

  if( locon->PNODE ) {
    type = rcx_rcnload( lofig, losig, locon->PNODE, &r, &c1, &c2, RCX_BESTLOAD, 0.0, &slope, RCX_MAX );
    switch( type ) {
    case RCX_CAPALOAD :
      printf( "load : c=%g\n", c1 );
      break ;
    case RCX_PILOAD :
      printf( "load : r=%g c1=%g c2=%g\n", r, c1, c2 );
      break ;
    default :
      printf( "load : error\n" );
      break ;
    }
  }
  if( getchain(rcx->RCXEXTERNAL, locon)==NULL )
    {
      chain_list *cl;
      for (cl=rcx->RCXEXTERNAL; cl!=NULL; cl=cl->NEXT)
        delaybetween( lofig, locon, (locon_list *)cl->DATA, display );
    }
  for( chain = rcx->RCXINTERNAL ; chain ; chain = chain->NEXT ) {
    dest = (locon_list*)chain->DATA ;
    if( dest != locon )
      delaybetween( lofig, locon, dest, display );
  }
  
  return 1 ;
}

void trc_delayforlosig( lofig_list *lofig, losig_list *losig, int display, int level )
{
  ptype_list *ptl ;
  chain_list *chain ;
  locon_list *locon ;

  printf( "processing signal %s\n", rcx_getsigname(losig) );
  fflush( stdout );
  ptl = getptype( losig->USER, LOFIGCHAIN );
  if( ptl ) {
    if( level == 1 ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        locon = (locon_list*)chain->DATA ;
        if( ! alldelayfromlocon( lofig, locon, display ) ) {
           printf( "an error occured when computing delays.\n" );
        }
      }
    }
    else {
      chain = (chain_list*)ptl->DATA ;
      if( chain ) {
        locon = (locon_list*)chain->DATA ;
        if( ! alldelayfromlocon( lofig, locon, display ) ) {
           printf( "an error occured when computing delays.\n" );
        }
      }
    }
  }
}
