/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : rcx_rcx.c                                                   */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* CVS informations :

Revision : $Revision: 1.32 $
Author   : $Author: anthony $
Date     : $Date: 2010/02/13 21:33:31 $

*/

#include "trc.h"

/* Renvoie la correction en delai due au bruit */

char NOISEDEBUG='n';

RCXFLOAT rcx_noisedelay_scr( lofig_list *lofig,
                             losig_list *victime, 
                             RCXFLOAT fv,
                             rcx_slope *fvmiller,
                             RCXFLOAT rv, 
                             RCXFLOAT vdd, 
                             char type
                           )
{
  RCXFLOAT retard, bruit;

  if( rcx_crosstalkactive( RCX_QUERY ) == RCX_NOCROSSTALK ) 
    return 0.0;

  if( rcx_isctkbasicmodel( getrcx( victime ) ) )
    return 0.0 ;

  switch( RCX_CTK_NOISE ) {
  case RCX_NOISE_ALWAYS :
  case RCX_NOISE_IFSTRONG :
    bruit  = rcx_noise_scr_1( lofig, victime, fvmiller, rv, vdd, type );
    retard = bruit*fv/(vdd*RCX_SLOPE_UNSHRINK);
    break;
  case RCX_NOISE_FINE :
    retard = rcx_noise_scr_2( lofig, victime, fvmiller, fv, rv, vdd, type );
    break;
  case RCX_NOISE_CUSTOM :
    if( type == RCX_MIN )
      retard = rcx_noise_scr_2( lofig, victime, fvmiller, fv, rv, vdd, type );
    else
      retard = 0.0 ;
    break;
  case RCX_NOISE_NEVER :
    retard = 0.0 ;
    break ;
  default:
    rcx_error(6, NULL, AVT_FATAL);
  }
  
  avt_log(LOGTRC,2, "Noise effect on delay for %s %s %s : %fps\n", 
           type==RCX_MAX ? "worst" : "best", 
           fvmiller->SENS==TRC_SLOPE_UP ? "UP" : "DW",
           rcx_getsigname( victime),
           retard 
          );

  return retard;
}

/* Fonction de base de calcule du bruit sur le montage simple */

RCXFLOAT rcx_noise_basic_scr( RCXFLOAT vdd, RCXFLOAT f, RCXFLOAT rv, RCXFLOAT cc, RCXFLOAT cm )
{
  RCXFLOAT a, b, k;
  RCXFLOAT bruit;

  if( f >= 1.0e-12 ) {
    a = rv*cc;
    b = rv*(cc+cm);
    k = vdd*RCX_SLOPE_UNSHRINK/f;
    bruit = a*k*(1.0-exp(-f/(RCX_SLOPE_UNSHRINK*b)));
  }
  else {
    bruit = cc/(cc+cm);
  }

  return bruit;
}

/******************************************************************************\
rcx_noise_detail()
Calcule le bruit pour chacun des agresseurs. Pour libérer le tableau de 
résultats, il faut faire :
  rcx_freetabagr( tableau, sizeof( rcx_signal_noise ), nbagr );
Le signal victime est supposé stable.
\******************************************************************************/

rcx_signal_noise* rcx_noise_scr_detail( lofig_list *lofig,
                                        losig_list *victime, 
                                        RCXFLOAT    rv, 
                                        RCXFLOAT    vdd,
                                        int        *nbagr
                                      )
{
  rcx_signal_noise *tabagr;
  int               n;
  RCXFLOAT          cm;
  RCXFLOAT          ct;
  RCXFLOAT          cc;
  RCXFLOAT          ccm;
  rcxparam         *param;
  float             f;

  tabagr = rcx_buildtabagr( lofig, 
                            victime, 
                            sizeof( rcx_signal_noise ), 
                            nbagr, 
                            &cm 
                          );
  if( tabagr == NULL ) return NULL;

  // Calcule la capacité totale à la masse lorsque tous les agresseurs sont
  // muets.
  ct=cm;
  for( n=0 ; n < *nbagr ; n++ ) 
    ct = ct + tabagr[n].CLOCALE;
  
  /* On fait le calcul en SI : ça évite les embrouilles... */
  ct  = ct * RCX_UNIT_CAPA_ALC_TO_SI;

  for( n=0 ; n < *nbagr ; n++ ) {
 
    param = tabagr[n].PARAM ;

    cc = tabagr[n].CLOCALE + tabagr[n].CGLOBALE ;

    if( cc > 0.0 ) {
      /* On fait le calcul en SI : ça évite les embrouilles... */
      cc = cc * RCX_UNIT_CAPA_ALC_TO_SI;
         
      ccm = ct - cc ;
      if( ccm < 0.0 ) {
        avt_errmsg( TRC_ERRMSG, "007", AVT_WARNING, rcx_getsigname(victime), 2 );
        rcx_freetabagr( tabagr, sizeof( rcx_signal_noise ), *nbagr );
        return NULL ;
      }

      if( ccm > 0.0 && cc > 0.0 ) {
        switch( RCX_CTK_SLOPE_NOISE ) {
        case RCX_SLOPE_NOMINAL :
          f = param->F0UP;
          break;
        case RCX_SLOPE_CTK :
          f = param->FMINUP;
          break;
        case RCX_SLOPE_REAL:
          rcx_calc_cca_param( param );
          f = rcx_realslope( param->CC, 
                             param->CCA, 
                             param->FMINUP, 
                             param->F0UP 
                           );
          break;
        }
        
        tabagr[n].NOISE_OVER  = rcx_noise_basic_scr( vdd,
                                                     f*RCX_UNIT_TIME_TAS_TO_SI,
                                                     rv, 
                                                     cc, 
                                                     ccm 
                                                   );

        switch( RCX_CTK_SLOPE_NOISE ) {
        case RCX_SLOPE_NOMINAL :
          f = param->F0DW;
          break;
        case RCX_SLOPE_CTK :
          f = param->FMINDW;
          break;
        case RCX_SLOPE_REAL:
          rcx_calc_cca_param( param );
          f = rcx_realslope( param->CC, 
                             param->CCA, 
                             param->FMINDW, 
                             param->F0DW 
                           );
          break;
        }

        tabagr[n].NOISE_UNDER = rcx_noise_basic_scr( vdd,
                                                     f*RCX_UNIT_TIME_TAS_TO_SI,
                                                     rv, 
                                                     cc, 
                                                     ccm 
                                                   );
      }
    }
    else {
      tabagr[n].NOISE_OVER  = 0.0 ;
      tabagr[n].NOISE_UNDER = 0.0 ;
    }
  }


  return tabagr;
}

/******************************************************************************\
rcx_noise_scr_x()
Calcule le bruit due aux agresseurs marqués actif.

rv :    Résistance de driver de la victime.
type :  Le bruit est calculé pour un délai max ou min
staticvic : YES|NO. Indique si le signal victime est stable ou non ( pour 
            prendre en compte le front réel sur l'agresseur ).
\******************************************************************************/

RCXFLOAT rcx_noise_scr_1 ( lofig_list *lofig,
                           losig_list *victime,
                           rcx_slope *fvmiller,
                           RCXFLOAT rv, 
                           RCXFLOAT vdd, 
                           char type
                         )
{
  agrnoise      *tabagr;
  int            nbagr;
  int            n;
  RCXFLOAT       cm;
  RCXFLOAT       ct;
  RCXFLOAT       cc;
  RCXFLOAT       ccm;
  RCXFLOAT       bruitotal;
  RCXFLOAT       bruit;
  RCXFLOAT       f;
  RCXFLOAT       fa;
  RCXFLOAT       m;
  char           flag;
  rcxparam      *param;

  tabagr = rcx_buildtabagr( lofig, victime, sizeof( agrnoise ), & nbagr, &cm );
  if( tabagr == NULL ) return 0.0;

  // Calcule la capacité totale à la masse lorsque tous les agresseurs sont
  // muets.
  ct=cm;
  for( n=0 ; n<nbagr ; n++ ) 
    ct = ct + tabagr[n].CLOCALE;
  bruitotal = 0.0;
  
  /* On fait le calcul en SI : ça évite les embrouilles... */
  ct  = ct * RCX_UNIT_CAPA_ALC_TO_SI;

  avt_log(LOGTRC,2, "Noise on signal %s %s %s\n", 
          type==RCX_MAX ? "worst" : "best", 
          fvmiller->SENS==TRC_SLOPE_UP ? "UP" : "DW",
          rcx_getsigname( victime)
          );

  for( n=0 ; n<nbagr ; n++ ) {
  
    param =  tabagr[n].PARAM;
    flag = 'N';
    bruit = 0.0;

    if( fvmiller->SENS == TRC_SLOPE_UP && type == RCX_MAX ) {
      if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ) {
        flag = 'Y';

        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
          fa = param->FMINDW ;
          break ;
        case RCX_SLOPE_DELAY_ENHANCED :
          rcx_calc_cca_param( param );
          fa = rcx_realslope( param->CC,
                              param->CCA,
                              param->FMINDW,
                              param->F0DW
                            );
          break;
        }

        m = rcx_getmiller( fvmiller->F0MAX, 
                           fa,
                           RCX_OPPOSITE, 
                           RCX_MILLER2C 
                         );
      }
    }

    if( fvmiller->SENS == TRC_SLOPE_UP && type == RCX_MIN ) {
      if( ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) {
        flag = 'Y';
        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
        case RCX_SLOPE_DELAY_ENHANCED :
          fa = param->FMINUP;
        }

        m = rcx_getmiller( fvmiller->F0MAX, 
                           fa,
                           RCX_SAME, 
                           RCX_MILLER2C 
                         );
      }
    }

    if( fvmiller->SENS == TRC_SLOPE_DOWN && type == RCX_MAX ) {
      if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ) {
        flag = 'Y';
        switch ( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
          fa = param->FMINUP;
          break;
        case RCX_SLOPE_DELAY_ENHANCED :
          rcx_calc_cca_param( param );
          fa = rcx_realslope( param->CC,
                              param->CCA,
                              param->FMINUP,
                              param->F0UP
                            );
          break;
        }

        m = rcx_getmiller( fvmiller->F0MAX, 
                           fa,
                           RCX_OPPOSITE, 
                           RCX_MILLER2C 
                         );
      }
    }

    if( fvmiller->SENS == TRC_SLOPE_DOWN && type == RCX_MIN ) {
      if( ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) {
        flag = 'Y';
        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
        case RCX_SLOPE_DELAY_ENHANCED :
          fa = param->FMINDW;
        }

        m = rcx_getmiller( fvmiller->F0MAX, 
                           fa, 
                           RCX_SAME, 
                           RCX_MILLER2C 
                         );
      }
    }

    if( flag == 'Y' ) {

      cc = tabagr[n].CLOCALE + tabagr[n].CGLOBALE ;

      if( cc > 0.0 ) {

        /* On fait le calcul en SI : ça évite les embrouilles... */
        f   = f * RCX_UNIT_TIME_TAS_TO_SI;
        cc = cc * RCX_UNIT_CAPA_ALC_TO_SI;
        
        ccm = ct - cc ;
        if( ccm < 0.0 ) {
          avt_errmsg( TRC_ERRMSG, "007", AVT_WARNING, rcx_getsigname(victime), 3 );
          rcx_freetabagr( tabagr, sizeof( agrnoise ), nbagr );
          return 0.0 ;
        }

        if( ccm > 0.0 && cc > 0.0 ) {
          bruit = 0.0 ;
          if( RCX_CTK_NOISE == RCX_NOISE_ALWAYS )
            bruit = rcx_noise_basic_scr( vdd, f, rv, cc, ccm );
          if( RCX_CTK_NOISE == RCX_NOISE_IFSTRONG ) {
            switch( RCX_CTK_MODEL ) {
            case RCX_MILLER_0C2C :
              if( m < 0.01 || m > 1.99 )
                bruit = rcx_noise_basic_scr( vdd, f, rv, cc, ccm );
              break ;
            case RCX_MILLER_NC3C :
              if( m < -0.99 || m > 2.99 )
                bruit = rcx_noise_basic_scr( vdd, f, rv, cc, ccm );
              break ;
            default :
              rcx_error( 9, NULL, AVT_FATAL );
            }
          }
        }
        else {
          bruit = 0.0 ;
        }
      }
    }

    avt_log(LOGTRC,2, "  %50s : %fv\n", 
            rcx_getsigname( tabagr[n].PARAM->SIGNAL ), 
            bruit 
          );
    bruitotal = bruitotal + bruit;
  }

  rcx_freetabagr( tabagr, sizeof( agrnoise ), nbagr );

  return bruitotal;
}

/******************************************************************************\
Fonctions déterminant le bruit de crosstalk de façon <<précise>>
\******************************************************************************/

RCXFLOAT rcx_noise_scr_2( lofig_list *lofig,
                          losig_list *victime,
                          rcx_slope *fvmiller,
                          RCXFLOAT fv,
                          RCXFLOAT rv, 
                          RCXFLOAT vdd, 
                          char type
                        )
{
  agrnoise2     *tabagr ;       // tableau des agresseurs
  int            nbagr ;        // taille de ce tableau
  double         cm ;           // capacité de masse
  double         ct ;           // capacité totale
  int            idmax ;        // index de l'agresseur ayant le + long front
  double         tamax ;        // front le plus long
  agrnoise_data  infos ;        // regroupement des infos pour la résolution
  int            itmax ;        // le nombre maximum d'itérations
  int            n ;
  double         bt = 0.0 ;
  double         tmin ;
  double         tmax ;
  int            ret ;
  double         noise = 0.0 ;
  double         retard = 0.0 ;

  tabagr = rcx_buildtabagr( lofig, victime, sizeof( agrnoise2 ), &nbagr, &cm );
  if( tabagr == NULL ) return 0.0;
  
  ct=cm;
  for( n=0 ; n < nbagr ; n++ ) 
    ct = ct + tabagr[n].CLOCALE;

  rcx_noise_calc_ta( tabagr, nbagr, fvmiller, fv, type, victime );
  rcx_noise_get_tamax( tabagr, nbagr, &idmax, &tamax );

  if( tamax > 0.0 ) {
    rcx_noise_complete_tabagr( tabagr, nbagr, tamax, ct );
    
    infos.tabagr = tabagr ;
    infos.nbagr  = nbagr ;
    infos.tamax  = tamax ;
    infos.vdd    = vdd ;
    infos.ct     = ct ;
    infos.rv     = rv ;
    infos.tv     = fv / RCX_SLOPE_UNSHRINK ;
    infos.fv     = vdd/infos.tv ;

    itmax = 1000.0 ;
    tmin = 0.0 ;
    tmax = tamax ;
  
    noise = rcx_noise_allagr( &infos, tmax ) ;
    if( noise > vdd/2.0 ) {
      avt_log(LOGTRC,2, "noise delay correction for signal %s %s %s : noise=%f, vdd/2=%f\n",
              type == RCX_MIN ? "min" : "max",
              fvmiller->SENS=='U' ? "UP" : "DOWN",
              rcx_getsigname( victime ), 
              noise, 
              vdd/2.0 
            );
      rcx_setctkbasicmodel( victime, getrcx( victime ) );
    }
    else { 
      ret = mbk_dichotomie( (int(*)(void*, double, double*)) rcx_noise_fn,
                            NULL,
                            &infos,
                            &tmin,
                            &tmax,
                            MBK_DICHO_EQUAL,
                            &itmax,
                            0.1,
                            DBL_MAX,
                            &bt
                          );

      switch( ret ) {
      case MBK_DICHO_OK :
        noise = rcx_noise_allagr( &infos, bt );
        retard = noise/infos.fv ;
        break;
      case MBK_DICHO_ERRFN :
        avt_log(LOGTRC,2, "rcx_noise_scr_2() exit with MBK_DICHO_ERRFN\n" );
        noise = 0.0 ;
        retard = 0.0 ;
        break;
      case MBK_DICHO_MAXIT :
        avt_log(LOGTRC,2, "rcx_noise_scr_2() exit with MBK_DICHO_MAXIT\n" 
               );
        noise = 0.0 ;
        retard = 0.0 ;
        break;
      case MBK_DICHO_ERR :
        avt_log(LOGTRC,2, "rcx_noise_scr_2() exit with MBK_DICHO_ERR\n" 
               );
        noise = 0.0 ;
        retard = 0.0 ;
        break;
      }
    }
  }

  rcx_freetabagr( tabagr, sizeof( agrnoise2 ), nbagr );

  return retard ;
}

void rcx_noise_trace_noise( agrnoise_data *infos, double tmax )
{
  FILE *ptf;
  double t;
  ptf = fopen( "trace.dat", "w" );
  for( t=0.0 ; t<tmax ; t=t+1.0 ) {
    fprintf( ptf, "%f %f\n", t, rcx_noise_allagr( infos, t ) );
  }
  fclose( ptf );
}

int rcx_noise_fn( agrnoise_data *infos, double bt, double *f )
{
  double noise ;
  double dt ;
  
  noise = rcx_noise_allagr( infos, bt );
  dt = noise/infos->fv ;

  switch( RCX_CTK_MODEL ) {
  case RCX_MILLER_0C2C :
  case RCX_MILLER_NOMINAL :
    *f = infos->tamax + dt - bt - infos->tv ;
    break ;
  case RCX_MILLER_NC3C :
    *f = infos->tamax + dt - bt - infos->tv/2.0 ;
    break ;
  default :
    rcx_error(61,NULL, AVT_FATAL);
  }

  return 1 ;
}

double rcx_noise_allagr( agrnoise_data *infos, double bt )
{
  int n;
  double btotal=0.0;
  double blocal;
  double t;
  double cc;
  double ccm;
  double ta;
  double rv;
  double dva;
  FILE   *file=NULL;
 
  if( NOISEDEBUG=='y' )
    file = mbkfopen( "debugagr", "log", "w" );
    
  for( n = 0 ; n < infos->nbagr ; n++ ) {

    if( file )
      fprintf( file, "%3d %s\n", n, rcx_getsigname(infos->tabagr[n].PARAM->SIGNAL) );

    if( infos->tabagr[n].TA >= 0.0 ) {
    
      cc     = infos->tabagr[n].CC ;
      ccm    = infos->tabagr[n].CCM ;
      dva    = infos->vdd ;
      ta     = infos->tabagr[n].TA ;
      rv     = infos->rv ;
      t      = bt + infos->tabagr[n].DT ;
      if( t > 0.0 ) {
        blocal = dva*rv*cc/ta*(1.0-exp(-t/(rv*(cc+ccm))));
        btotal = btotal + blocal ;

        if( file ) 
          fprintf( file, "noise is %g\n", blocal );
      }
    }
  }

  if( file )
    fclose( file );
  return btotal ;
}

void rcx_noise_complete_tabagr( agrnoise2 *tabagr,
                                int        nbagr,
                                double     tamax,
                                double     ct
                              )
{
  int n;
  
  for( n = 0 ; n < nbagr ; n++ )
  {
    if( tabagr[n].TA >= 0 ) {
      tabagr[n].CC  = tabagr[n].CLOCALE + tabagr[n].CGLOBALE ;
      tabagr[n].CCM = ct - tabagr[n].CC ;
      tabagr[n].DT  = tabagr[n].TA - tamax ;
    }
  }
}

void rcx_noise_get_tamax( agrnoise2 *tabagr, 
                          int nbagr, 
                          int *idmax,
                          double *tamax
                        )
{
  int n ;
 
  *tamax = 0.0 ;
  
  for( n = 0 ; n < nbagr ; n++ )
  {
    if( tabagr[n].TA >= 0 ) {
      if( tabagr[n].TA > *tamax ) {
        *idmax = n ;
        *tamax = tabagr[n].TA ;
      }
    }
  }
}

// Remplis le TA de agrnoise2
void rcx_noise_calc_ta( agrnoise2 *tabagr, 
                        int nbagr, 
                        rcx_slope *fvmiller,
                        RCXFLOAT fv,
                        char type,
                        losig_list *victime
                      )
{
  int           n ;
  double        f ;
  rcxparam     *param ;
  double        fa ;
  
  for( n = 0 ; n < nbagr ; n++ ) {
  
    param = tabagr[n].PARAM;
    
    f=-1;
    
    if( fvmiller->SENS == TRC_SLOPE_UP && type == RCX_MAX ) {
      if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ) {
     
        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
          fa = param->FMINDW ;
          break ;
        case RCX_SLOPE_DELAY_ENHANCED :
          rcx_calc_cca_param( param );
          fa = rcx_realslope( param->CC,
                              param->CCA,
                              param->FMINDW,
                              param->F0DW
                            );
          break;
        }
        
        switch( RCX_CTK_MODEL ) {
        case RCX_MILLER_0C2C :
        case RCX_MILLER_NOMINAL :
          if( fv < fa )
            f = fa ;
          break ;
        case RCX_MILLER_NC3C :
          if( fv / 2 < fa )
            f = fa ;
          break ;
        default :
          rcx_error(47,NULL, AVT_FATAL);
        }
      }
    }

    if( fvmiller->SENS == TRC_SLOPE_UP && type == RCX_MIN ) {
      if( ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) {
      
        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
          fa = param->FMINUP ;
          break ;
        case RCX_SLOPE_DELAY_ENHANCED :
          rcx_calc_cca_param( param );
          fa = rcx_realslope( param->CC,
                              param->CCA,
                              param->FMINUP,
                              param->F0UP
                            );
          break;
        }

        switch( RCX_CTK_MODEL ) {
        case RCX_MILLER_0C2C :
        case RCX_MILLER_NOMINAL :
          if( fv < fa ) 
            f = fa ;
          break ;
        case RCX_MILLER_NC3C :
          if( fv / 2 < fa )
            f = fa ;
          break ;
        default :
          rcx_error(48,NULL, AVT_FATAL);
        }
      }
    }

    if( fvmiller->SENS == TRC_SLOPE_DOWN && type == RCX_MAX ) {
      if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ) {

        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
          fa = param->FMINUP ;
          break ;
        case RCX_SLOPE_DELAY_ENHANCED :
          rcx_calc_cca_param( param );
          fa = rcx_realslope( param->CC,
                              param->CCA,
                              param->FMINUP,
                              param->F0UP
                            );
          break;
        }

        switch( RCX_CTK_MODEL ) {
        case RCX_MILLER_0C2C :
        case RCX_MILLER_NOMINAL :
          if( fv < fa )
            f = fa ;
          break ;
        case RCX_MILLER_NC3C :
          if( fv / 2 < fa ) 
            f = fa ;
          break ;
        default :
          rcx_error(49,NULL, AVT_FATAL);
        }
      }
    }

    if( fvmiller->SENS == TRC_SLOPE_DOWN && type == RCX_MIN ) {
      if( ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) {

        switch( RCX_CTK_SLOPE_DELAY ) {
        case RCX_SLOPE_DELAY_CTK :
          fa = param->FMINUP ;
          break ;
        case RCX_SLOPE_DELAY_ENHANCED :
          rcx_calc_cca_param( param );
          fa = rcx_realslope( param->CC,
                              param->CCA,
                              param->FMINDW,
                              param->F0DW
                            );
          break;
        }

        switch( RCX_CTK_MODEL ) {
        case RCX_MILLER_0C2C :
        case RCX_MILLER_NOMINAL :
          if( fv < fa )
            f = fa ;
          break ;
        case RCX_MILLER_NC3C :
          if( fv / 2 < fa )
            f = fa ;
          break ;
        default :
          rcx_error(50,NULL, AVT_FATAL);
        }
      }
    }

    if( f >= 0 ) {
      tabagr[n].TA = f / RCX_SLOPE_UNSHRINK  ;
    }
    else
      tabagr[n].TA = -1.0 ;
  }

  victime = NULL;
}
