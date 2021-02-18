/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_ctk_score.c                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire AVOT                                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>
#include <unistd.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H
#include STM_H

#include "stb_util.h"
#include "stb_init.h"
#include "stb_error.h"
#include "stb_transfer.h"
#include "stb_relaxation.h"
#include "stb_overlap.h"
#include "stb_ctk.h"
#include "stb_ctk_noise.h"
#include "stb_ctk_debug.h"
#include "stb_ctk_score.h"

int STB_CTK_COEF_NOISE    = 4 ;
int STB_CTK_COEF_CTK      = 2 ;
int STB_CTK_COEF_INTERVAL = 3 ;
int STB_CTK_COEF_ACTIVITY = 1 ;

int STB_CTK_MIN_NOISE     = 3 ;
int STB_CTK_MIN_CTK       = 0 ;
int STB_CTK_MIN_INTERVAL  = 0 ;
int STB_CTK_MIN_ACTIVITY  = 0 ;

char STB_CTK_STAT_COMPARE = STB_CTK_SORT_SCORE_TOTAL ;

int stb_ctk_good_score( int score )
{
  if( score < 0 )  score = 0 ;
  if( score > 10 ) score = 10 ;

  return score ;
}

int stb_ctk_get_min_noise()
{
  return STB_CTK_MIN_NOISE ;
}

int stb_ctk_get_min_interval()
{
  return STB_CTK_MIN_INTERVAL ;
}

int stb_ctk_get_min_ctk()
{
  return STB_CTK_MIN_CTK ;
}

int stb_ctk_get_min_activity()
{
  return STB_CTK_MIN_ACTIVITY ;
}

void stb_ctk_set_min_score( int noise, int ctk, int interval, int activity )
{
  if( noise >= 0 ) 
    STB_CTK_MIN_NOISE     = noise ;
  if( ctk >= 0 )
    STB_CTK_MIN_CTK       = ctk ;
  if( interval >= 0 )
    STB_CTK_MIN_INTERVAL  = interval;
  if( activity >= 0 )
    STB_CTK_MIN_ACTIVITY  = activity ;
}

int stb_ctk_get_coef_noise()
{
  return STB_CTK_COEF_NOISE ;
}

int stb_ctk_get_coef_ctk()
{
  return STB_CTK_COEF_CTK ;
}

int stb_ctk_get_coef_interval()
{
  return STB_CTK_COEF_INTERVAL ;
}

int stb_ctk_get_coef_activity()
{
  return STB_CTK_COEF_ACTIVITY ;
}

/******************************************************************************\
stb_ctk_set_coef_score()
Initialise les 4 coefficients de pondérations. 
Les valeurs négatives ne modifient pas le coefficients.
Les valeurs sont bornées à 10.
\******************************************************************************/
void stb_ctk_set_coef_score( int noise, int ctk, int interval, int activity )
{
  if( noise >= 0 ) 
    STB_CTK_COEF_NOISE    = stb_ctk_good_score( noise );
  if( ctk >= 0 )
    STB_CTK_COEF_CTK      = stb_ctk_good_score( ctk );
  if( interval >= 0 )
    STB_CTK_COEF_INTERVAL = stb_ctk_good_score( interval );
  if( activity >= 0 )
    STB_CTK_COEF_ACTIVITY = stb_ctk_good_score( activity );
}

int stb_ctk_get_score_total( stb_ctk_tab_stat *stat )
{
  int n ;

  n = stb_ctk_get_score_noise( stat )    * STB_CTK_COEF_NOISE    +
      stb_ctk_get_score_ctk( stat )      * STB_CTK_COEF_CTK      +
      stb_ctk_get_score_interval( stat ) * STB_CTK_COEF_INTERVAL +
      stb_ctk_get_score_activity( stat ) * STB_CTK_COEF_ACTIVITY   ;
  
  return n ;
  
}

int stb_ctk_get_score_noise( stb_ctk_tab_stat *stat )
{
  return stat->SCORE_NOISE ;
}

int stb_ctk_get_score_ctk( stb_ctk_tab_stat *stat )
{
  return stat->SCORE_CTK ;
}

int stb_ctk_get_score_interval( stb_ctk_tab_stat *stat )
{
  return stat->SCORE_INTERVAL ;
}

int stb_ctk_get_score_activity( stb_ctk_tab_stat *stat )
{
  return stat->SCORE_ACTIVITY ;
}

int stb_ctk_get_score_probability( stb_ctk_tab_stat *stat )
{
  return stat->SCORE_PROBABILITY ;
}

/******************************************************************************\
stb_ctk_score_noise()

Note l'effet du bruit.
\******************************************************************************/
int stb_ctk_score_noise( stbfig_list *stbfig, stb_ctk_detail *detail )
{
  int            score = 0 ;
  float          vth ;
  float          noise ;
  float          vdd ;


  vth = detail->NOISE_VTH ;

  if( ( detail->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
    noise = detail->NOISE_OVR ;
  else
    noise = detail->NOISE_UND ;

  if( ( detail->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) {
    if( noise > vth )
      score = 10 ;
    else
      score = ceil( noise/vth*10.0 );
  }
  else {
    vdd = stb_ctk_signal_get_vdd( stbfig, detail->NODE );
    if( (vdd-noise) < vth )
      score = 10 ;
    else
      score = ceil( noise/(vdd-vth)*10.0 );
  }
  score = stb_ctk_good_score( score );
  return score ;
}

/******************************************************************************\
stb_ctk_score_interval()

Note l'impact de la simultanéité des agresseurs. C'est la part maximum de 
crosstalk qui peut etre activée à un instant donné.
\******************************************************************************/
int stb_ctk_score_interval( stbfig_list *stbfig, stb_ctk_detail *detail )
{
  int                            score = 0 ;
  float                          s;
  float                          sdw;
  float                          sup;
  stb_ctk_detail_agr_list       *agr;
  chain_list                    *chain ;
  float                          ccalways[2] ;
  float                          cgap ;
  float                          cgapmax[2] ;
  stbgap_list                   *gap ;
  int                            i ;
  char                           calcul[] = { TTV_NODE_DOWN, TTV_NODE_UP };
  stb_ctk_gap                   *ctkgap ;

  for( i = 0 ; i <= 1 ; i++) {

    ccalways[i]  = 0.0 ;
    cgapmax[i]   = 0.0 ;
    
    ctkgap = stb_fill_gap( stbfig, detail, calcul[i] );

    for( chain = ctkgap->NOINFO ; chain ; chain = chain->NEXT ) {
      agr = (stb_ctk_detail_agr_list*)chain->DATA ;
      if( agr )
        ccalways[i] = ccalways[i] + agr->CC ;
    }

    /* Prend en compte les agresseurs considérés comme toujours actifs */
    for( chain = ctkgap->OTHERDOMAIN ; chain ; chain = chain->NEXT ) {
      agr = stb_ctk_get_node_detail( ((ttvevent_list*)chain->DATA)->ROOT );
      if( agr ) 
        ccalways[i] = ccalways[i] + agr->CC ;
    }

    /* Récupère le crosstalk maximum sur les différents gap */
    for( gap = ctkgap->GAPACTIF ; gap ; gap = gap->NEXT ) {
      cgap = 0.0 ;
      for( chain = gap->SIGNALS ; chain ; chain = chain->NEXT ) {
        agr = stb_ctk_get_node_detail( ((ttvevent_list*)chain->DATA)->ROOT );
        if( agr )
          cgap = cgap + agr->CC ;
      }
      if( cgap > cgapmax[i] )
        cgapmax[i] = cgap ;
    }
  }

  sdw = ccalways[0] + cgapmax[0] ;
  sup = ccalways[1] + cgapmax[1] ;

  if( detail->CC > 0.0 ) {
    if( sup > sdw ) 
      s = 10.0 * sup / detail->CC ;
    else
      s = 10.0 * sdw / detail->CC ;
  }
  else
    s = 0.0 ;

  score = ceil( s );
  score = stb_ctk_good_score( score );
  return score ;
}

/******************************************************************************\
stb_ctk_score_ctk()

Repère les signaux dont le crosstalk est majoritairement due à un nombre
reduit d'agresseurs.
\******************************************************************************/
int stb_ctk_score_ctk( stbfig_list *stbfig, stb_ctk_detail *detail )
{
  int                            score = 0 ;
  stb_ctk_detail_agr_list       *agr ;
  int                            maxagr ;
  int                            i ;
  float                         *tabcapa ;
  float                          sum ;
  float                          climit ;

  for( agr = detail->AGRLIST, maxagr=0 ; agr ; agr = agr->NEXT, maxagr++ ) ;
  if( maxagr == 0 )
    return 0 ;
  if( maxagr == 1 )
    return 10 ;

  tabcapa = (float*)mbkalloc( sizeof( float ) * maxagr );
  
  for( agr = detail->AGRLIST, i=0 ; agr ; agr = agr->NEXT, i++ ) 
    tabcapa[i] = agr->CC ;
 
  qsort( tabcapa, 
         maxagr, 
         sizeof( float ), 
         (int (*)(const void*, const void*))stb_ctk_qsort_float 
       );

  sum = 0.0 ;
  climit = detail->CC / 2.0 ;
  
  for( i=0 ; i < maxagr ; i++ ) {
    sum = sum + tabcapa[i] ;
    if( sum > climit )
      break;
  }

  score = ceil(( 10.0 * (2*i - (maxagr-1)) ) / (maxagr-1)) ;
  score = stb_ctk_good_score( score );

  mbkfree( tabcapa );
  stbfig = NULL ; // unused parameter 
  return score ;
}

/******************************************************************************\
stb_ctk_score_activity()

Evalue l'impact de l'activité d'un agresseur.
Pour l'instant, seules les clock ont une activité à peu près connue.
La note renvoyée sur 10 correspond à la part de crosstalk due aux agresseurs
considérés comme systématiquement actifs.

Idée :

Voir si il ne faut pas affecter une note intermédiaire aux agresseurs pour
lesquels l'activité n'est pas connue, une note maximum si l'activité est
certaine et une note minimum si il n'est pas actif.
\******************************************************************************/
int stb_ctk_score_activity( stbfig_list *stbfig, stb_ctk_detail *detail )
{
  int                            score = 0 ;
  stb_ctk_detail_agr_list       *agr ;
  float                          s[2] ;
  stbnode                       *node ;
  int                            i ;
  float                          r ;

  s[0] = 0.0 ;
  s[1] = 0.0 ;

  for( i = 0 ; i <= 1 ; i++ ) {
    s[i] = 0.0 ;

    for( agr = detail->AGRLIST ; agr ; agr = agr->NEXT ) {
   
      if( agr->TTVAGR ) {
        node = stb_getstbnode( &(agr->TTVAGR->NODE[i]) );
        if( !node )
          continue ;

        if( node->CK ) 
          s[i] = s[i] + agr->CC ;
      }
    }
  }

  if( detail->CC > 0.0 ) {
    if( s[0] > s[1] ) 
      r = 10.0 * s[0] / detail->CC ;
    else
      r = 10.0 * s[1] / detail->CC ;
  }
  else
    r = 0.0 ;
    
  score = ceil( r );
  score = stb_ctk_good_score( score );

  stbfig = NULL ; // unused  parameter
  return score ;
}

// probabilite d'occurence de l'agression
int stb_ctk_score_propability( stbfig_list *stbfig, stb_ctk_detail *detail )
{
  int                            score = 0 ;
  stb_ctk_detail_agr_list       *agr ;
  int                            maxagr ;
  int                            i ;
  float                         *tabcapa ;
  float                          sum ;
  float                          proba=1 ;
  ptype_list *pt;

  for( agr = detail->AGRLIST ; agr ; agr = agr->NEXT) 
  {
    if( agr->TTVAGR )
    {
      if ((pt=getptype(agr->TTVAGR->USER, STB_TRANSITION_PROBABILITY))!=NULL)
         proba*=*(float *)&pt->DATA;
    }
  }

  score = (int)mbk_long_round(10-log(1/proba));
  if (score<0) score=0;
  stbfig = NULL ; // unused parameter 
  return score ;
}

/******************************************************************************\
stb_ctk_calc_score()
\******************************************************************************/
void stb_ctk_calc_score( stbfig_list      *stbfig, 
                         stb_ctk_detail   *ctkdetail, 
                         stb_ctk_tab_stat *stat 
                       )
{
  stat->SCORE_NOISE       = stb_ctk_score_noise( stbfig, ctkdetail );
  stat->SCORE_INTERVAL    = stb_ctk_score_interval( stbfig, ctkdetail );
  stat->SCORE_CTK         = stb_ctk_score_ctk( stbfig, ctkdetail );
  stat->SCORE_ACTIVITY    = stb_ctk_score_activity( stbfig, ctkdetail );
  stat->SCORE_PROBABILITY = stb_ctk_score_propability( stbfig, ctkdetail );
}

int stb_ctk_qsort_float( float *f1, float *f2 )
{
  if( *f1 < *f2 ) return -1 ;
  if( *f1 > *f2 ) return  1 ;
  return 0;
}

/******************************************************************************\
stb_ctk_signal_threshold()
Calcule la tension admissible au maximum sur un signal.
si <0, c'est qu'on a pas pu la déterminer (cas d'une sortie).
\******************************************************************************/

float stb_ctk_signal_threshold( stbfig_list *stbfig, ttvevent_list *event )
{
  ptype_list    *ptype ;
  chain_list    *chline ;
  chain_list    *headline ;
  long           level ;
  float          vth ;
  float          vthmax = -1.0 ;
  ttvline_list  *ptline ;
  ttvevent_list *nodet ;
  timing_model  *model ;
  int            found ;

  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL ;
  else
    level = 0 ;

  ttv_expfigsig( stbfig->FIG,
                 event->ROOT, 
                 level, 
                 stbfig->FIG->INFO->LEVEL, 
                 TTV_STS_CLS_FED|TTV_STS_DUAL_FED, 
                 TTV_FILE_DTX
               );

  if((ptype = getptype(event->USER,TTV_NODE_DUALLINE)) != NULL)
    headline = (chain_list *)ptype->DATA ;
  else
    headline = NULL ;

  found = 0 ;
  
  for( chline = headline ; chline ; chline = chline->NEXT ) {
  
    ptline = (ttvline_list*)chline->DATA ;

    vth = -1.0 ;
    if( !ttv_islinelevel( stbfig->FIG, ptline, level ) )
      continue ;

    nodet = ptline->ROOT ;

    if( (ptline->TYPE & TTV_LINE_RC ) == TTV_LINE_RC ) {
      vth = stb_ctk_signal_threshold( stbfig, nodet );
    }
    else {
      model = stm_getmodel( ptline->FIG->INFO->FIGNAME, ptline->MDMAX );
      if( model ) {
        if( stm_noise_getmodeltype( model ) == STM_NOISE_SCR ) {
          vth = stm_noise_scr_invth( stm_noise_getmodel_scr(model) ) ;
        }
      }
    }

    if( ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) {
      if( found == 0 || vth < vthmax ) {
        vthmax = vth ;
        found = 1 ;
      }
    }
    else {
      if( found == 0 || vth > vthmax ) {
        vthmax = vth ;
        found = 1 ;
      }
    }
  }

  return vthmax ;
}

float stb_ctk_signal_get_vdd( stbfig_list *stbfig, ttvevent_list *event )
{
  long           level ;
  float          vddmin = FLT_MAX ;
  float          vdd ;
  ttvline_list  *ptline ;
  timing_model  *model ;
  int            found ;

  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL ;
  else
    level = 0 ;

  ttv_expfigsig( stbfig->FIG,
                 event->ROOT, 
                 level, 
                 stbfig->FIG->INFO->LEVEL, 
                 TTV_STS_CLS_FED|TTV_STS_DUAL_FED, 
                 TTV_FILE_DTX
               );

  found = 0 ;
  for( ptline = event->INLINE ; ptline ; ptline = ptline->NEXT ) {
  
    if( !ttv_islinelevel( stbfig->FIG, ptline, level ) )
      continue ;

    model = stm_getmodel( ptline->FIG->INFO->FIGNAME, ptline->MDMAX );
    if( model ) {
      vdd = stm_mod_vdd( model ) ;

      if( found == 0 || vdd < vddmin ) {
        vddmin = vdd ;
        found = 1 ;
      }
    }
  }

  if( found == 0 )
    vddmin = stm_mod_default_vdd() ;

  return vddmin ;
  
}

float stb_ctk_signal_threshold_from_input( stbfig_list *stbfig, 
                                           ttvevent_list *event 
                                         )
{
  long           level ;
  float          vth ;
  float          vddmin = FLT_MAX ;
  float          vdd ;
  ttvline_list  *ptline ;
  timing_model  *model ;

  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL ;
  else
    level = 0 ;

  ttv_expfigsig( stbfig->FIG,
                 event->ROOT, 
                 level, 
                 stbfig->FIG->INFO->LEVEL, 
                 TTV_STS_CLS_FED|TTV_STS_DUAL_FED, 
                 TTV_FILE_DTX
               );

  for( ptline = event->INLINE ; ptline ; ptline = ptline->NEXT ) {
  
    if( !ttv_islinelevel( stbfig->FIG, ptline, level ) )
      continue ;

    model = stm_getmodel( ptline->FIG->INFO->FIGNAME, ptline->MDMAX );
    if( model ) {
      vdd = stm_mod_vdd( model ) ;

      if( vdd < vddmin )
        vddmin = vdd ;
    }
  }

  if( vddmin < FLT_MAX )
    vth = vddmin / 2.0 ;
  else
    vth = stm_mod_default_vdd() / 2.0 ;

  return vth ;
}

/******************************************************************************\
Fonction de comparaison pour trier les tableaux de statistiques. Le critère
de trie doit être mis dans la variable globale STB_CTK_STAT_COMPARE.
\******************************************************************************/
int stb_ctk_score_compare( stb_ctk_tab_stat *n1, stb_ctk_tab_stat *n2 )
{
  int s1, s2 ;
  int r, trs1, trs2 ;

  switch( STB_CTK_STAT_COMPARE ) {
  case STB_CTK_SORT_SCORE_TOTAL :
    s1 = stb_ctk_get_score_total( n1 );
    s2 = stb_ctk_get_score_total( n2 );
    break;
  case STB_CTK_SORT_SCORE_NOISE :
    s1 = stb_ctk_get_score_noise( n1 );
    s2 = stb_ctk_get_score_noise( n2 );
    break;
  case STB_CTK_SORT_SCORE_CTK :
    s1 = stb_ctk_get_score_ctk( n1 );
    s2 = stb_ctk_get_score_ctk( n2 );
    break;
  case STB_CTK_SORT_SCORE_INTERVAL :
    s1 = stb_ctk_get_score_interval( n1 );
    s2 = stb_ctk_get_score_interval( n2 );
    break;
  case STB_CTK_SORT_SCORE_ACTIVITY :
    s1 = stb_ctk_get_score_activity( n1 );
    s2 = stb_ctk_get_score_activity( n2 );
    break;
  default :
    s1 = 0;
    s2 = 0;
  }

  if( s1 < s2 ) return  1 ;
  if( s1 > s2 ) return -1 ;
  r =strcmp( n1->NODE->ROOT->NAME, n2->NODE->ROOT->NAME );
  if( r )
    return r ;

  if( ( n1->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
    trs1=1;
  else
    trs1=0;

  if( ( n2->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP )
    trs2=1;
  else
    trs2=0;

  if( trs1>trs2 ) return  1;
  if( trs2>trs1 ) return -1;
  return 0;
}

int stb_ctk_stat_to_keep( stb_ctk_tab_stat *stat )
{
  if( stb_ctk_get_score_noise( stat )    < stb_ctk_get_min_noise()    ||
      stb_ctk_get_score_interval( stat ) < stb_ctk_get_min_interval() ||
      stb_ctk_get_score_ctk( stat )      < stb_ctk_get_min_ctk()      ||
      stb_ctk_get_score_activity( stat ) < stb_ctk_get_min_activity()    )
    return 0 ;
  return 1 ;
}

void stb_ctk_fix_min( stb_ctk_stat *stat )
{
  int               tabend ;
  int               curpos ;
  stb_ctk_tab_stat  tmp;

  tabend = stat->NBELEM ;
  curpos = 0 ;
  while( curpos < tabend ) {
    if( ! stb_ctk_stat_to_keep( &stat->TAB[curpos] ) ) {
      if( curpos != tabend-1 ) {
        memcpy( &tmp, &stat->TAB[curpos], sizeof( stb_ctk_tab_stat ) ) ;
        memcpy( &stat->TAB[curpos], &stat->TAB[tabend-1], sizeof( stb_ctk_tab_stat ) ) ;
        memcpy( &stat->TAB[tabend-1], &tmp, sizeof( stb_ctk_tab_stat ) ) ;
      }
      tabend-- ;
    }
    else {
      curpos++ ;
    }
  }
  stat->NBDISPLAY = tabend ;
}
