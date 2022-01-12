/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_ctk.c                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                Grégoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <unistd.h>


#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

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
#include "stb_ctk_mutex.h"
#include "stb_ctk_agr.h"

#include "stb_relaxation_correction.h"
#include "stb_ctk_report.h"

// pour les apis
#include "api_communication.h"
#include "stb_communication.c"
// -------------

// Variation de front minimum pour considérer un changement significatif
long STB_CTK_MINSLOPECHANGE=0;

// Nombre maximum d'itération lorsqu'il n'y a plus de nouvelles agressions
// détectées.
int  STB_CTK_MAXLASTITER=3;

// Autorise plus de consommation mémoire pour aller plus vite
char STB_CTK_FASTMODE=1;

// Comportement des agresseurs pour lesquels il n'y a pas d'information temporelle
//int STB_CTK_NOINFO_ACTIF=0;

// Le fichier de report
long  CTK_REPORT_DELTA_DELAY_MIN = 0;
long  CTK_REPORT_DELTA_SLOPE_MIN = 0;
float CTK_REPORT_CTK_MIN         = 0.0;
long  CTK_REPORT_NOISE_MIN       = 0;
// La marge pour la détection des agressions
long STB_CTK_MARGIN=0l;

long NBPTYPE=0l;

/*
------------------------------------------------------------------------------

            FONCTIONS D'AFFICHAGE ET DE DEBUGGAGE 

------------------------------------------------------------------------------
*/

/* Largeur de la barre de progression */
#define STB_PROGRESS_WIDTH 50

/* Fonction à appeller pour la barre de progression */
void (*CTK_PROGRESSBAR)(int, int, int, char*) = stb_progress ;
/* Fonction à appeller pour l'affichage d'informations */
void (*CTK_PRINTINFO)(char*) = stb_print ;

int STBCTK_NBDELAYCALC;

void stb_ctk_error( int code, char *fmt, ... )
{
  va_list       index;
  va_start( index, fmt );

  fflush( stdout );
  fprintf( stderr, "\n\n*** ERROR %d IN STB/CTK ***\n\n", code );
  vfprintf( stderr, fmt, index );
  fprintf( stderr, "\n" );
  EXIT(1);
}

void stb_print( char *msg )
{
  printf( "%s\n", msg );
}

void stb_progressbar( int max, int current )
{
  static char ligne [1024];
  long   pos;
  long   r;
  int    i;
  static int last;
  static int lastr;
  if( ! isatty(1) || max == 0 ) return;
  
  if( current == 0 ) {
  
    for( i=0 ; i<1024 ; i++ ) ligne[i]=' ';

    ligne[STB_PROGRESS_WIDTH+1]='\0';

  }
  
  pos = ((long)current)*STB_PROGRESS_WIDTH/max;
  r   = ((long)current)*100/max;
  if( lastr != r || last != pos || current==0) {
    for( i=(last<0?0:last) ; i<=pos ; i++ ) {
      ligne[i]='#';
    }
    avt_fprintf( stdout, "\r%3ld%%  [" AVT_BLUE "%s" AVT_RESET "] %7ldKb", r, ligne, mbkprocessmemoryusage()/1024 );
    fflush( stdout );
    last  = pos;
    lastr = r;
  }
}

void stb_progress( int iteration, int max, int pos, char *msg )
{
  char bufchrono[128] ;
  static mbk_chrono chrono;
  static int lastiter = -1 ;

  if( msg ) {
    printf( "\n%s\n", msg );
    mbk_StartChrono( &chrono );
  }
  else {
    if( iteration && lastiter != iteration ) 
      printf( "Iteration #%d\n", iteration );
    stb_progressbar( max, pos );
    if( pos == max ) {
      printf( "\n" );
      mbk_StopChrono( &chrono );
      mbk_GetUserChrono( &chrono, bufchrono );
      printf("  User CPU time : %s\n", bufchrono );
      mbk_StartChrono( &chrono );
    }
  }

  lastiter = iteration ;
}

/*
------------------------------------------------------------------------------

            MEMORISATION DES AGRESSIONS ACTIVES DANS STB

------------------------------------------------------------------------------

Principe :

Dans beaucoups de cas, il y a très peu d'agresseurs actifs. Une ptype_list
pour stocker l'agresseur (champs DATA) et le type d'agression (champs TYPE)
est donc satisfaisante, même si entre les fonctions stb_fillactifrcxparam(), 
stb_saveactifrcxparam(), stb_getagressiontype() et stb_setagressiontype() cela
conduit à des algos en n².

Lorsque le n² devient inacceptable ( STB_CTK_MAXCHAIN²), les ptype_list sont
remplacées par des tables de hash. Le gain est double : la consomation mémoire
( la table de hash est un tableau de 2 valeurs, il n'y a pas de champs NEXT)
et le temps de calcul sont réduit.

Q : Pourquoi ne pas avoir utilisé alors systématiquement une table de hash ?

Parceque le malloc() standard utilisé par défaut dans addht() consomme en
réalité 24 octets au lieu de 12, et si on prend un petit nombre initial (par
exemple 2), le malloc des htitem consomme 24 octets au lieu de 16. Pour les 
petits nombres d'agresseurs actifs (jusqu'à 6), la table de hash est donc plus 
coûteuse en mémoire, pour un gain CPU très faible.


On ne peut pas utiliser les flags de rcx/rcn car les vues rcx sont factorisées :
Deux ttvsig peuvent correspondre à un losig.
*/

/*****************************************************************************
*                           fonction stb_getagressiontype()                  *
******************************************************************************
* Récupère l'agression entre deux signaux.                                   *
*****************************************************************************/
char stb_getagressiontype( victime, agresseur )
ttvsig_list     *victime;
ttvsig_list     *agresseur;
{
  ptype_list    *ptl;
  long           v;

  ptl = getptype( victime->USER, STB_CTK_RCXAGRPT );
  
  if( ptl ) {
  
    for( ptl = ((ptype_list*)(ptl->DATA)) ; 
         ptl && ((ttvsig_list*)(ptl->DATA)) != agresseur ;
         ptl = ptl->NEXT );
         
    if( ptl )
      return ((char)(ptl->TYPE)) ;
      
    return 0;
  }

  ptl = getptype( victime->USER, STB_CTK_RCXAGRHT );

  if( ptl ) {

    v = gethtitem( ((ht*)(ptl->DATA)), agresseur );
    if( v != EMPTYHT )
      return ((char)v) ;

    return 0;
  }

  return 0;
}

/*****************************************************************************
*                           fonction stb_setagressiontype()                  *
******************************************************************************
* Mémorise l'agression entre deux signaux.                                   *
*****************************************************************************/
void stb_setagressiontype( victime, agresseur, type )
ttvsig_list     *victime;
ttvsig_list     *agresseur;
char             type;
{
  ptype_list    *ptl;
  ptype_list    *scan;
  int            n;
  ht            *newht;

  ptl = getptype( victime->USER, STB_CTK_RCXAGRHT );

  if( ptl ) {
    sethtitem( ((ht*)(ptl->DATA)), agresseur, (long)((unsigned char)type) );
    return ;
  }

  ptl = getptype( victime->USER, STB_CTK_RCXAGRPT );

  if( !ptl ) {

    victime->USER = addptype( victime->USER,
                              STB_CTK_RCXAGRPT,
                              NULL
                            );
    ptl = victime->USER;
  }
  
  for( scan = ((ptype_list*)(ptl->DATA)), n=0 ; 
       scan && ((ttvsig_list*)(scan->DATA)) != agresseur ;
       scan = scan->NEXT, n++ );
         
  if( scan ) {
    scan->TYPE = (char) type;
    return ;
  }

  if( n > STB_CTK_MAXCHAIN ) {

    newht = addht( STB_CTK_MAXCHAIN + 1 );

    for( scan = ((ptype_list*)( ptl->DATA )) ; scan ; scan = scan->NEXT )
      addhtitem( newht, scan->DATA, (long)((unsigned char)scan->TYPE) );
    addhtitem( newht, agresseur, (long)((unsigned char)type) );
   
    freeptype( ((ptype_list*)(ptl->DATA)) );
    victime->USER = delptype( victime->USER, STB_CTK_RCXAGRPT );
    ptl = NULL;

    victime->USER = addptype( victime->USER, STB_CTK_RCXAGRHT, newht );
      
  }
  else {

    ptl->DATA = addptype( ((ptype_list*)(ptl->DATA)),
                                           (char)type,
                                           agresseur
                                         );
  }

  return;

}

void stb_cleanagressiontype( ttvsig_list *signal )
{
  ptype_list    *ptl;

  ptl = getptype( signal->USER, STB_CTK_RCXAGRPT );
  if( ptl ) {
    freeptype( (ptype_list*)ptl->DATA );
    signal->USER = delptype( signal->USER, STB_CTK_RCXAGRPT );
  }
  
  ptl = getptype( signal->USER, STB_CTK_RCXAGRHT );
  if( ptl ) {
    delht( (ht*)ptl->DATA );
    signal->USER = delptype( signal->USER, STB_CTK_RCXAGRHT );
  }
}

/*
  Marque les driver de victime. Renvoie la chain_list des ttvsig marqués pour
  les nettoyer plus tard

  victime est un ttvsig en sortie de gate
  Ne fonctionne qu'à plat.
*/

chain_list* stb_mark_input( ttvfig_list *ttvfig, 
                            ttvsig_list *victime, 
                            long level 
                          )
{
  int i, j;
  ttvline_list  *line ;
  ttvline_list  *linerc ;
  ttvsig_list   *ttvsig ;
  ttvsig_list   *ttvsigrc ;
  chain_list    *toclean ;

  ttv_expfigsig( ttvfig, victime, level, ttvfig->INFO->LEVEL, 
                 TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX
               );
  
  toclean = NULL ;

  for( i=0 ; i<= 1 ; i++ ) {

    for( line = victime->NODE[i].INLINE ; line ; line = line->NEXT ) {

      if( !ttv_islinelevel( ttvfig, line, level ) )
        continue ;

      ttvsig = line->NODE->ROOT ;

      if( !getptype( ttvsig->USER, STB_CTK_DRIVER ) ) {
        ttvsig->USER = addptype( ttvsig->USER, STB_CTK_DRIVER, 0l );
        toclean = addchain( toclean, ttvsig );
      }

      /* on marque également les driver des entrées rc */

      for( j=0 ; j<=1 ; j++ ) {
        for( linerc = ttvsig->NODE[j].INLINE ; linerc ; linerc = linerc->NEXT ) 
        {
        
          if( ((linerc->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
              !ttv_islinelevel( ttvfig, linerc, level )        ) 
            continue ;

          ttvsigrc = linerc->NODE->ROOT ;
          if( !getptype( ttvsigrc->USER, STB_CTK_DRIVER ) ) {
            ttvsigrc->USER = addptype( ttvsigrc->USER, STB_CTK_DRIVER, 0l );
            toclean = addchain( toclean, ttvsigrc );
          }
        }
      }
    }
  }

  return toclean ;
}

int stb_is_mark_input( ttvsig_list *ttvsig ) 
{
  if( getptype( ttvsig->USER, STB_CTK_DRIVER ) )
    return 1 ;
  return 0;
}

void stb_clean_mark_input( chain_list *tocleanmark )
{
  chain_list *chain ;
  ttvsig_list *ttvsig ;
  
  for( chain = tocleanmark ; chain ; chain = chain->NEXT ) {
    ttvsig = (ttvsig_list*)chain->DATA ;
    ttvsig->USER = delptype( ttvsig->USER, STB_CTK_DRIVER );
  }
  freechain( tocleanmark );
}

static void stb_set_min_slope(ttvfig_list *ttvfig, ttvevent_list *tve, int level, int mode)
{
  ptype_list *pt;
  long delayval, lineval;
  stb_fastslope *sfs;
  int i;
  if ((pt=getptype(tve->ROOT->USER, STB_CTK_FAST_SLOPE))==NULL)
  {
     sfs=(stb_fastslope *)mbkalloc(sizeof(stb_fastslope));
     pt=tve->ROOT->USER=addptype(tve->ROOT->USER, STB_CTK_FAST_SLOPE, sfs);
     for (i=0; i<2; i++) 
     {
       sfs->ev[i].delayval=sfs->ev[i].lineval=STM_DEF_SLEW;
       sfs->ev[i].pairnode=NULL;
     }
  }

  sfs=(stb_fastslope *)pt->DATA;
      
  if (tve->TYPE & TTV_NODE_UP) i=1; else i=0;
  if (mode & 2)
  {
  sfs->ev[i].delayval = ttv_getslopenode( ttvfig, 
                               level, 
                               tve, 
                   TTV_FIND_LINE | TTV_FIND_GATE | TTV_FIND_RC | TTV_FIND_MIN,
                                        TTV_MODE_DELAY
                                      )/TTV_UNIT;
  if( sfs->ev[i].delayval == 0 ) sfs->ev[i].delayval = STM_DEF_SLEW ;
  }
  if (mode & 1)
  {
    sfs->ev[i].lineval   = ttv_getslopenode( ttvfig, 
                                level, 
                                tve, 
                   TTV_FIND_LINE | TTV_FIND_GATE | TTV_FIND_RC | TTV_FIND_MIN,
                                        TTV_MODE_LINE
                                      )/TTV_UNIT;
  if( sfs->ev[i].lineval == 0 ) sfs->ev[i].lineval = STM_DEF_SLEW ;
  }
}

void stb_setupall_slopes(stbfig_list *stbfig, int level)
{
  ttvevent_list *tve;
  chain_list *chain;
  for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
  {
    tve=(ttvevent_list *)chain->DATA;
    stb_set_min_slope(stbfig->FIG, tve, level, 3);
  }
}
void stb_cleanup_slopes(stbfig_list *stbfig)
{
  ttvevent_list *tve;
  chain_list *chain;
  ptype_list *pt;
  int i;
  stb_fastslope *sfs;
  for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
  {
    tve=(ttvevent_list *)chain->DATA;
    if ((pt=getptype(tve->ROOT->USER, STB_CTK_FAST_SLOPE))!=NULL)
    {
     sfs=(stb_fastslope *)pt->DATA;
     for (i=0; i<2; i++)
       stb_freestbpair(sfs->ev[i].pairnode);
     mbkfree(sfs);
     tve->ROOT->USER=delptype(tve->ROOT->USER, STB_CTK_FAST_SLOPE);
    }
  }
}
void stb_release_fast_nodepair(ttvevent_list *tve)
{
  ptype_list *pt;
  stb_fastslope *sfs;
  int i;
  if (tve->TYPE & TTV_NODE_UP) i=1; else i=0;
  if ((pt=getptype(tve->ROOT->USER, STB_CTK_FAST_SLOPE))!=NULL)
  {
   sfs=(stb_fastslope *)pt->DATA;
   stb_freestbpair(sfs->ev[i].pairnode);
   sfs->ev[i].pairnode=NULL;
  }
}

void stb_cleanup_fast_nodepair(stbfig_list *stbfig)
{
  ttvevent_list *tve;
  chain_list *chain;
  for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
  {
    tve=(ttvevent_list *)chain->DATA;
    stb_release_fast_nodepair(tve);
  }
}

/*****************************************************************************
*                           fonction stb_fillttvsigrcxparam()                *
******************************************************************************
* Ajoute dans tous les rcxparam d'une liste le ttvsig correspondant. Remplie *
* egalement les champs FMINUP et FMINDW. Lorsque plusieurs agresseurs sont   *
* présents, on prend le pire en terme de fronts.                             *
*****************************************************************************/
void stb_fillttvsigrcxparam( ttvfig, level, type, headlist )
ttvfig_list     *ttvfig;
long             level;
long             type;
chain_list      *headlist;
{
  chain_list    *chain;
  rcxparam      *param;
  ttvsig_list   *ttvsig;
  ptype_list *pt;
  stb_fastslope *sfs;

  stb_ctkprint( 1, "Filling agressor parameter :\n" );


  for( chain = headlist ; chain ; chain = chain->NEXT ) {
  
    param   = (rcxparam*)(chain->DATA);

   
    ttvsig = ttv_getttvsig( ttvfig, 
                            level,
                            type,
                            param->INSNAME, 
                            param->SIGNAL,
                            STB_CTK_FASTMODE
                          );

    if( ttvsig  ) {

      param->USER = addptype( param->USER, STB_CTK_RCXTTVSIG, ttvsig );
      if ((pt=getptype(ttvsig->USER, STB_CTK_FAST_SLOPE))!=NULL)
      {
        sfs=(stb_fastslope *)pt->DATA;
        param->FMINUP=sfs->ev[1].delayval;
        param->FMINDW=sfs->ev[0].delayval;
        param->F0UP=sfs->ev[1].lineval;
        param->F0DW=sfs->ev[0].lineval;
      }
      else
      {
      param->FMINUP = ttv_getslopenode( ttvfig, 
                                        level, 
                                        ttvsig->NODE+1, 
                   TTV_FIND_LINE | TTV_FIND_GATE | TTV_FIND_RC | TTV_FIND_MIN,
                                        TTV_MODE_DELAY
                                      )/TTV_UNIT;
      if( param->FMINUP == 0 ) param->FMINUP = STM_DEF_SLEW ;

      param->FMINDW = ttv_getslopenode( ttvfig, 
                                        level, 
                                        ttvsig->NODE, 
                   TTV_FIND_LINE | TTV_FIND_GATE | TTV_FIND_RC | TTV_FIND_MIN,
                                        TTV_MODE_DELAY
                                  )/TTV_UNIT;
      if( param->FMINDW == 0 ) param->FMINDW = STM_DEF_SLEW ;

      param->F0UP   = ttv_getslopenode( ttvfig, 
                                        level, 
                                        ttvsig->NODE+1, 
                   TTV_FIND_LINE | TTV_FIND_GATE | TTV_FIND_RC | TTV_FIND_MIN,
                                        TTV_MODE_LINE
                                      )/TTV_UNIT;
      if( param->F0UP == 0 ) param->F0UP = STM_DEF_SLEW ;

      param->F0DW   = ttv_getslopenode( ttvfig, 
                                        level, 
                                        ttvsig->NODE, 
                   TTV_FIND_LINE | TTV_FIND_GATE | TTV_FIND_RC | TTV_FIND_MIN,
                                        TTV_MODE_LINE
                                  )/TTV_UNIT;
      if( param->F0DW == 0 ) param->F0DW = STM_DEF_SLEW ;
      }

      stb_ctkprint( 1, 
   "  %s (netname=%s) : Fmin Up = %g, F0 Up = %g, Fmin Dw = %g, F0 Dw = %g\n",
                    ttvsig->NAME,
                    ttvsig->NETNAME,
                    param->FMINUP,
                    param->F0UP,
                    param->FMINDW,
                    param->F0DW
                  );
    }
    else {
      stb_ctkprint( 1, "  No corresponding ttvsig for signal %s\n",
                       getsigname( param->SIGNAL )
                  );
      param->FMINUP = STM_DEF_SLEW;
      param->FMINDW = STM_DEF_SLEW;
      param->F0UP   = STM_DEF_SLEW;
      param->F0DW   = STM_DEF_SLEW;
    }
  }
}

/*****************************************************************************
*                           fonction stb_delttvsigrcxparam()                 *
******************************************************************************
* Libère dans tous les rcxparam d'une liste le ttvsig correspondant.         *
*****************************************************************************/
void stb_delttvsigrcxparam( headlist )
chain_list      *headlist;
{
  chain_list    *chain;
  rcxparam      *param;
  ptype_list    *ptl;

  for( chain = headlist ; chain ; chain = chain->NEXT ) {
  
    param   = (rcxparam*)(chain->DATA);
    
    ptl = getptype( param->USER, STB_CTK_RCXTTVSIG );
    if( !ptl ) 
      continue;
   
    param->USER = delptype( param->USER, STB_CTK_RCXTTVSIG );
  }
}

/*****************************************************************************
*                           fonction stb_getttvsigrcxparam()                 *
******************************************************************************
* Récupère le ttvsig correspondant à un rcxparam                             *
*****************************************************************************/
ttvsig_list* stb_getttvsigrcxparam( param )
rcxparam        *param;
{
  ptype_list    *ptl;
  
  ptl = getptype( param->USER, STB_CTK_RCXTTVSIG );

  if( !ptl ) {
    return( NULL );
  }

  return ((ttvsig_list*)(ptl->DATA));
}

/*****************************************************************************
*                           fonction stb_getrcxparamfromevent()              *
******************************************************************************
* Récupère le rcxparam d'un event.                                           *
*****************************************************************************/
rcxparam* stb_getrcxparamfromevent( event )
ttvevent_list *event;
{
  rcxparam   *param;
  ptype_list *ptl;

  ptl = getptype( event->USER, STB_CTK_RCXPARAM );
  if( !ptl ) {
    fflush( stdout );
    fprintf( stderr, "*** Error in STB/CTK : Can't find rcxparam from event.\n"
           );
    EXIT(1);
  }
  param = ((rcxparam*)(ptl->DATA));

  return param ;
}

/*****************************************************************************
*                           fonction stb_addrcxparamfromevent()              *
******************************************************************************
* Récupère le rcxparam d'un event.                                           *
*****************************************************************************/
void stb_addrcxparamfromevent( event, param )
ttvevent_list *event;
rcxparam   *param;
{
  ptype_list *ptl;

  ptl = getptype( event->USER, STB_CTK_RCXPARAM );
  if( ptl ) {
    fflush( stdout );
    fprintf( stderr, "*** Error in STB/CTK : event contain an rcxparam.\n"
           );
    EXIT(1);
  }

  NBPTYPE++;
  event->USER = addptype( event->USER, STB_CTK_RCXPARAM, param );
}

/*****************************************************************************
*                           fonction stb_getrcxparamfromevent()              *
******************************************************************************
* Récupère le rcxparam d'un event.                                           *
*****************************************************************************/
void stb_delrcxparamfromevent( event )
ttvevent_list *event;
{
  ptype_list *ptl;

  ptl = getptype( event->USER, STB_CTK_RCXPARAM );
  if( !ptl ) {
    fflush( stdout );
    fprintf( stderr, "*** Error in STB/CTK : Can't find rcxparam from event.\n"
           );
    EXIT(1);
  }

  event->USER = delptype( event->USER, STB_CTK_RCXPARAM );
  NBPTYPE--;
}

/*****************************************************************************
*                           fonction stb_fillactifrcxparam()                 *
******************************************************************************
* Remplis le champs ACTIF des structures rcxparam à partir des valeurs       *
* calculées aux itérations précédentes.                                      *
* mutex vaut 'Y' ou 'N'. A 'N', seul les aggressions sont renvoyées. A 'Y',  *
* les résultats de mutex sont pris en compte.                                *
*****************************************************************************/
#define STB_MAX_AGR 10000

typedef struct
{
  rcxparam *param;
  float proba;
} agr_sort_proba_info;

static int stb_compare_proba_capa(const void *a, const void *b)
{
  agr_sort_proba_info *aspia=(agr_sort_proba_info *)a;
  agr_sort_proba_info *aspib=(agr_sort_proba_info *)b;
  float mixa, mixb;
  mixa=aspia->param->CC*aspia->proba;
  mixb=aspib->param->CC*aspib->proba;
  if (mixa<mixb) return 1;
  if (mixa>mixb) return -1;
  return 0;
}

void stb_choose_sub_agressor_list_depending_on_probability(chain_list *headlist, float probamin)
{
  agr_sort_proba_info tab[STB_MAX_AGR];
  int i, j, ck;
  float curproba, futureproba;
  rcxparam *param;
  ttvsig_list *ttvsig;
  ptype_list *pt;
  stbnode *node;

  if (probamin!=0)
    {
      i=0;
      while (headlist) 
        {
          param=(rcxparam *)headlist->DATA;
          tab[i].param=param;
          ttvsig = stb_getttvsigrcxparam( param );
          if (ttvsig!=NULL)
          {
            ck=0;
            node = stb_getstbnode( &ttvsig->NODE[0]);
            if (node->CK!=NULL) ck=1;
            else 
            { 
              node = stb_getstbnode( &ttvsig->NODE[1]);
              if (node->CK!=NULL) ck=1;
            }
            if (ck)
              tab[i].proba=1, i++;
            else if ((pt=getptype(ttvsig->USER, STB_TRANSITION_PROBABILITY))!=NULL)
              tab[i].proba=*(float *)&pt->DATA, i++;
            else if(V_BOOL_TAB[__STB_CTK_NOINFO_ACTIF].VALUE)
              tab[i].proba=1, i++;
          }
          else
            if(V_BOOL_TAB[__STB_CTK_NOINFO_ACTIF].VALUE)
              tab[i].proba=1, i++;
          headlist=headlist->NEXT;
        }

      qsort(tab, i, sizeof(agr_sort_proba_info), stb_compare_proba_capa);
      
      curproba=1;
      for (j=0; j<i; j++)
        {
          futureproba=curproba*tab[j].proba;
          if (futureproba<=probamin)
            tab[j].param->ACTIF=0;
          else 
            curproba=futureproba;
        }
    }
}

void stb_fillactifrcxparam( ttvfig, victime, headlist, mutex )
ttvfig_list     *ttvfig;
ttvevent_list   *victime;
chain_list      *headlist;
char             mutex;
{
  chain_list    *chain;
  rcxparam      *param;
  char           type;
  ttvsig_list   *ttvsig;

  stb_ctkprint( 0, "  Previous agressors :\n" );
  for( chain = headlist ; chain ; chain = chain->NEXT ) {
  
    param = (rcxparam*)(chain->DATA);
    param->ACTIF = 0;
  
    ttvsig = stb_getttvsigrcxparam( param );
   
    if( ttvsig ) {

      type = stb_getagressiontype( victime->ROOT, ttvsig );
      
      if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
            ( type & STB_CTK_UP_WRST ) == STB_CTK_UP_WRST         ) ||
          ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
            ( type & STB_CTK_DW_WRST ) == STB_CTK_DW_WRST         )    )
        param->ACTIF = param->ACTIF | RCX_AGRWORST ;
     
      if( mutex == 'Y' )
        if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
              ( type & STB_CTK_UP_MTX_WORST ) == STB_CTK_UP_MTX_WORST  ) ||
            ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
              ( type & STB_CTK_DW_MTX_WORST ) == STB_CTK_DW_MTX_WORST  )    )
          param->ACTIF = ( param->ACTIF | RCX_MTX_WORST ) & ~RCX_AGRWORST ;

      if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
            ( type & STB_CTK_UP_BEST ) == STB_CTK_UP_BEST         ) ||
          ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
            ( type & STB_CTK_DW_BEST ) == STB_CTK_DW_BEST         )    )
        param->ACTIF = param->ACTIF | RCX_AGRBEST ;

      if( mutex == 'Y' )
        if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
              ( type & STB_CTK_UP_MTX_BEST ) == STB_CTK_UP_MTX_BEST      ) ||
            ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
              ( type & STB_CTK_DW_MTX_BEST ) == STB_CTK_DW_MTX_BEST      )    )
          param->ACTIF = ( param->ACTIF | RCX_MTX_BEST ) & ~RCX_AGRBEST ;
        
    }
    else {
      // On a pas d'infos sur l'agresseur
      if( V_BOOL_TAB[__STB_CTK_NOINFO_ACTIF].VALUE )
        param->ACTIF = param->ACTIF | RCX_AGRWORST | RCX_AGRBEST ;
    }

    stb_ctkprint( 0, "  - %s.%s : %s %s\n",
                  param->INSNAME,
                  ttvsig ? ttvsig->NAME : getsigname( param->SIGNAL ),
                  ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ?
                    "BEST":"",
                  ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ?
                    "WORST":""
                );
    
  }
  
  stb_choose_sub_agressor_list_depending_on_probability(headlist, V_FLOAT_TAB[__STB_MIN_PROBABILITY].VALUE);

  ttvfig = NULL; // unused parameter : avoid a warning at compilation
}

/*****************************************************************************
*                           fonction stb_fillinactifrcxparam()               *
******************************************************************************
* Remplis le champs ACTIF des structures rcxparam à partir des valeurs       *
* calculées aux itérations précédentes.                                      *
*****************************************************************************/
void stb_fillinactifrcxparam( ttvfig, victime, headlist, mutex )
ttvfig_list     *ttvfig;
ttvevent_list   *victime;
chain_list      *headlist;
char             mutex;
{
  chain_list    *chain;
  rcxparam      *param;
  char           type;
  ttvsig_list   *ttvsig;

  stb_ctkprint( 0, "  Previous agressors :\n" );
  for( chain = headlist ; chain ; chain = chain->NEXT ) {
  
    param = (rcxparam*)(chain->DATA);
   
    ttvsig = stb_getttvsigrcxparam( param );
    if( !ttvsig ) {
      if( V_BOOL_TAB[__STB_CTK_NOINFO_ACTIF].VALUE )
        param->ACTIF = RCX_AGRWORST | RCX_AGRBEST ;
      continue;
    }

    param->ACTIF = RCX_AGRWORST | RCX_AGRBEST ;

    type = stb_getagressiontype( victime->ROOT, ttvsig );
    
    if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
          ( type & STB_CTK_UP_NOWRST ) == STB_CTK_UP_NOWRST         ) ||
        ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
          ( type & STB_CTK_DW_NOWRST ) == STB_CTK_DW_NOWRST         )    )
      param->ACTIF = param->ACTIF & ~RCX_AGRWORST ;

    if( mutex == 'Y' ) 
      if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
            ( type & STB_CTK_UP_MTX_WORST ) == STB_CTK_UP_MTX_WORST  ) ||
          ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
            ( type & STB_CTK_DW_MTX_WORST ) == STB_CTK_DW_MTX_WORST  )    )
        param->ACTIF = ( param->ACTIF | RCX_MTX_WORST ) & ~RCX_AGRWORST ;

    if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
          ( type & STB_CTK_UP_NOBEST ) == STB_CTK_UP_NOBEST         ) ||
        ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
          ( type & STB_CTK_DW_NOBEST ) == STB_CTK_DW_NOBEST         )    )
      param->ACTIF = param->ACTIF & ~RCX_AGRBEST ;

    if( mutex == 'Y' )
      if( ( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP && 
            ( type & STB_CTK_UP_MTX_BEST ) == STB_CTK_UP_MTX_BEST      ) ||
          ( ( victime->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
            ( type & STB_CTK_DW_MTX_BEST ) == STB_CTK_DW_MTX_BEST      )    )
        param->ACTIF = ( param->ACTIF | RCX_MTX_BEST ) & ~RCX_AGRBEST ;

    stb_ctkprint( 0, "  - %s.%s : %s %s\n",
                  param->INSNAME,
                  ttvsig->NAME,
                  ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ?
                    "BEST":"",
                  ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ?
                    "WORST":""
                );
    
  }

  stb_choose_sub_agressor_list_depending_on_probability(headlist, V_FLOAT_TAB[__STB_MIN_PROBABILITY].VALUE);

  ttvfig = NULL; // unused parameter : avoid a warning at compilation
}

/*****************************************************************************
*                           fonction stb_saveactifrcxparam()                 *
******************************************************************************
Mémorise dans STB les nouvelles agressions détectées. Renvoie 1 si il y en
a eu au moins une nouvelle, et 0 sinon. 
Les nouvelles agressions sont stockées de manière définitive, qu'elles soient 
MUTEX ou non. L'information MUTEX est mémorisée, mais de manière non définitive.
*****************************************************************************/
int stb_saveactifcoupling( ttvevent_list *victime, ttvsig_list *aggressor, int rcxaggression )
{
  char           oldtype;
  char           newtype;
  char           newagr=0;

  oldtype = stb_getagressiontype( victime->ROOT, aggressor );
  newtype = oldtype; 
  

  if( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) {

    if( ( rcxaggression & RCX_AGRBEST ) == RCX_AGRBEST )
      newtype = newtype | STB_CTK_UP_BEST;

    if( ( rcxaggression & RCX_MTX_BEST ) == RCX_MTX_BEST ) 
      newtype = newtype | STB_CTK_UP_BEST | STB_CTK_UP_MTX_BEST;
    else
      newtype = newtype & ~STB_CTK_UP_MTX_BEST ;
      
    if( ( rcxaggression & RCX_AGRWORST ) == RCX_AGRWORST )
      newtype = newtype | STB_CTK_UP_WRST;

    if( ( rcxaggression & RCX_MTX_WORST ) == RCX_MTX_WORST ) 
      newtype = newtype | STB_CTK_UP_WRST | STB_CTK_UP_MTX_WORST;
    else
      newtype = newtype & ~STB_CTK_UP_MTX_WORST ;

  }
  else {

    if( ( rcxaggression & RCX_AGRBEST ) == RCX_AGRBEST )
      newtype = newtype | STB_CTK_DW_BEST;

    if( ( rcxaggression & RCX_MTX_BEST ) == RCX_MTX_BEST ) 
      newtype = newtype | STB_CTK_DW_BEST | STB_CTK_DW_MTX_BEST;
    else
      newtype = newtype & ~STB_CTK_DW_MTX_BEST ;
      
    if( ( rcxaggression & RCX_AGRWORST ) == RCX_AGRWORST )
      newtype = newtype | STB_CTK_DW_WRST;

    if( ( rcxaggression & RCX_MTX_WORST ) == RCX_MTX_WORST ) 
      newtype = newtype | STB_CTK_DW_WRST | STB_CTK_DW_MTX_WORST;
    else
      newtype = newtype & ~STB_CTK_DW_MTX_WORST ;
  }

  if( newtype != oldtype ) {
    stb_setagressiontype( victime->ROOT, aggressor, newtype );
    newagr = 1;
  }

  return newagr ;
}

int stb_saveactifrcxparam( ttvfig, victime, headlist )
ttvfig_list     *ttvfig;
ttvevent_list   *victime;
chain_list      *headlist;
{
  chain_list    *chain;
  rcxparam      *param;
  char           newagr=0;
  ttvsig_list   *ttvsig ;

  for( chain = headlist ; chain ; chain = chain->NEXT ) {
  
    param = (rcxparam*)(chain->DATA);

    if( param->ACTIF & RCX_QUIET )
      continue ;

    ttvsig = stb_getttvsigrcxparam( param );
    if( !ttvsig )
      continue ;
   
    if( stb_saveactifcoupling( victime, ttvsig, param->ACTIF ) )
      newagr = 1 ;
  }

  ttvfig = NULL; // unused parameter : avoid a warning at compilation
  return( newagr );
}

/*****************************************************************************
*                           fonction stb_saveinactifrcxparam()               *
******************************************************************************
* Mémorise dans STB les nouvelles agressions détectées. Renvoie 1 si il y en *
* a eu au moins une nouvelle, et 0 sinon.                                    *
*****************************************************************************/

int stb_saveinactifcoupling( ttvevent_list *victime, ttvsig_list *aggressor, int rcxaggression ) 
{
  char           oldtype;
  char           newtype;
  char           newagr=0;

  oldtype = stb_getagressiontype( victime->ROOT, aggressor );
  newtype = oldtype; 
  

  if( ( victime->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) {

    if( ( rcxaggression & RCX_AGRBEST ) != RCX_AGRBEST )
      newtype = newtype | STB_CTK_UP_NOBEST;

    if( ( rcxaggression & RCX_MTX_BEST ) == RCX_MTX_BEST ) 
      newtype = ( newtype & ~STB_CTK_UP_NOBEST ) | STB_CTK_UP_MTX_BEST;
    else
      newtype = newtype & ~STB_CTK_UP_MTX_BEST ;

    if( ( rcxaggression & RCX_AGRWORST ) != RCX_AGRWORST )
      newtype = newtype | STB_CTK_UP_NOWRST;

    if( ( rcxaggression & RCX_MTX_WORST ) == RCX_MTX_WORST ) 
      newtype = ( newtype & ~STB_CTK_UP_NOWRST ) | STB_CTK_UP_MTX_WORST;
    else
      newtype = newtype & ~STB_CTK_UP_MTX_WORST ;
  }
  else {

    if( ( rcxaggression & RCX_AGRBEST ) != RCX_AGRBEST )
      newtype = newtype | STB_CTK_DW_NOBEST;

    if( ( rcxaggression & RCX_MTX_BEST ) == RCX_MTX_BEST ) 
      newtype = ( newtype & ~STB_CTK_DW_NOBEST ) | STB_CTK_DW_MTX_BEST;
    else
      newtype = newtype & ~STB_CTK_DW_MTX_BEST ;

    if( ( rcxaggression & RCX_AGRWORST ) != RCX_AGRWORST )
      newtype = newtype | STB_CTK_DW_NOWRST;

    if( ( rcxaggression & RCX_MTX_WORST ) == RCX_MTX_WORST ) 
      newtype = ( newtype & ~STB_CTK_DW_NOWRST ) | STB_CTK_DW_MTX_WORST;
    else
      newtype = newtype & ~STB_CTK_DW_MTX_WORST ;
  }

  if( newtype != oldtype ) {
    stb_setagressiontype( victime->ROOT, aggressor, newtype );
    newagr = 1;
  }

  return newagr ;
}

int stb_saveinactifrcxparam( ttvfig, victime, headlist )
ttvfig_list     *ttvfig;
ttvevent_list   *victime;
chain_list      *headlist;
{
  chain_list    *chain;
  rcxparam      *param;
  char           newagr=0;
  ttvsig_list   *ttvsig;

  for( chain = headlist ; chain ; chain = chain->NEXT ) {
  
    param = (rcxparam*)(chain->DATA);

    if( param->ACTIF & RCX_QUIET )
      continue ;

    ttvsig = stb_getttvsigrcxparam( param );
    if( !ttvsig )
      continue ;

    if( stb_saveinactifcoupling( victime, ttvsig, param->ACTIF ) )
      newagr = 1 ;
  }

  ttvfig = NULL; // unused parameter : avoid a warning at compilation
  return( newagr );
}

/*
------------------------------------------------------------------------------

            FONCTIONS  PRINCIPALES

------------------------------------------------------------------------------
*/

/*****************************************************************************
*                           fonction stb_marqsigfromgap()                    *
******************************************************************************
* Marque tous les signaux d'un gap comme étant des agresseurs actifs.        *
*****************************************************************************/
void stb_marqactiffromgap( evtvic, gap )
ttvevent_list   *evtvic;
stbgap_list     *gap;
{
  chain_list    *scan;
  ttvevent_list *event;
  rcxparam      *param;
 
  stb_ctkprint( 0, "  - New aggressors :\n" );
  for( scan = gap->SIGNALS ; scan ; scan = scan->NEXT ) {
  
    event = ((ttvevent_list*)(scan->DATA));
    param = stb_getrcxparamfromevent( event );

    if( ( (  event->TYPE & TTV_NODE_UP   ) == TTV_NODE_UP   &&
          ( evtvic->TYPE & TTV_NODE_UP   ) == TTV_NODE_UP      ) ||
        ( (  event->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN &&
          ( evtvic->TYPE & TTV_NODE_DOWN ) == TTV_NODE_DOWN    )    ) {
      if( (param->ACTIF & RCX_AGRBEST ) != RCX_AGRBEST )
        stb_ctkprint( 0, "      %s %s : Best case agressor.\n",
                      ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                     "DOWN",
                      event->ROOT->NAME
                    );
      param->ACTIF = param->ACTIF | RCX_AGRBEST ;
    }
    else {
      if( (param->ACTIF & RCX_AGRWORST ) != RCX_AGRWORST )
        stb_ctkprint( 0, "      %s %s : Worst case agressor.\n",
                      ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                     "DOWN",
                      event->ROOT->NAME
                    );
      param->ACTIF = param->ACTIF | RCX_AGRWORST ;
    }
  }
}

/*****************************************************************************
*                           fonction stb_createchainevent()                  *
******************************************************************************
* Crée une liste d'event à partir d'une liste de rcxparam.                   *
*****************************************************************************/
chain_list* stb_createchainevent( headagr, type )
chain_list      *headagr;
long             type;
{
  chain_list    *eventlist = NULL ;
  chain_list    *scanagr;
  rcxparam      *param;
  ttvevent_list *event;
  ttvsig_list   *ttvsig;

  for( scanagr = headagr ; scanagr ; scanagr = scanagr->NEXT ) {
  
    param = (rcxparam*)scanagr->DATA;
    ttvsig = stb_getttvsigrcxparam( param );
    if( !ttvsig ) 
      continue;

    if( type == TTV_NODE_DOWN )
      event = ttvsig->NODE+0;
    else
      event = ttvsig->NODE+1;
      
    eventlist = addchain( eventlist, event );
    
    stb_addrcxparamfromevent( event, param );
  }

  return( eventlist );
}

/*****************************************************************************
*                           fonction stb_getchainevent()                     *
******************************************************************************
* Cette fonction a deux fonctionnalités :                                    *
*   - Renvoie une liste chainée des event correspondant aux agresseurs ;     *
*   - Ajoute un ptype STB_CTK_RCXPARAM dans les event ( utilisé par la       *
*     fonction stb_marqsigfromgap() ).                                       *
* La liste et les ptypes doivent être effacés par un appel à la fonction     *
* stb_freechainevent().                                                      *
*****************************************************************************/
chain_list *stb_getchainevent( evtvic, type, headagr )
ttvevent_list   *evtvic;
char             type;
chain_list      *headagr;
{
  chain_list    *eventlist;
  
  if( ( evtvic->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) {
    if( type == STB_CTK_WORST_AGR )
      eventlist = stb_createchainevent( headagr, TTV_NODE_DOWN );
    else
      eventlist = stb_createchainevent( headagr, TTV_NODE_UP );
  }
  else {
    if( type == STB_CTK_WORST_AGR )
      eventlist = stb_createchainevent( headagr, TTV_NODE_UP );
    else
      eventlist = stb_createchainevent( headagr, TTV_NODE_DOWN );
  }
  
  return( eventlist );
}

/*****************************************************************************
*                           fonction stb_freechainevent()                    *
******************************************************************************
* Libère la chaine et les ptypes alloués par la fonction stb_getchainevent().*
*****************************************************************************/
void stb_freechainevent( eventlist )
chain_list      *eventlist;
{
  chain_list    *chain;
  ttvevent_list *event;

  for( chain = eventlist ; chain ; chain = chain->NEXT ) {
  
    event = ((ttvevent_list*)(chain->DATA));
    stb_delrcxparamfromevent( event );
  }

  freechain( eventlist );
}

/*****************************************************************************
*                           fonction stb_detectavtiveagressorworst()         *
******************************************************************************
* Positionne dans les structures rcxparam de la chain_list headagr les bits  *
* indiquant les agressions observables. Cette fonction renvoie 1 si au moins *
* une nouvelle agression a été détectée, et 0 sinon.                         *
* L'approche est pire cas : toutes les agressions sont initialement prises   *
* en compte et éliminées progressivement.                                    *
*****************************************************************************/
void stb_detectactiveagressorworst( stbfig, level, type, evtvic, headagr )
stbfig_list     *stbfig;
long             level;
long             type;
ttvevent_list   *evtvic;
chain_list      *headagr;
{
  chain_list    *eventlist;
  chain_list    *realagrlist;
  chain_list    *alwaysagrlist;
  chain_list    *testagrlist;
  chain_list    *chain;
  stbnode       *stbvic;
  rcxparam      *param;
  ttvevent_list *event;
  char           oldactif;
  
  if( ! headagr ) return;
  
  stbvic = stb_getstbnode( evtvic );
  if( !stbvic ) {
    fflush( stdout );
    fprintf( stderr, "\n*** Error in STB/CTK : no stability on event.\n" );
    EXIT(1);
  }
  
  /* WORST CASE */
  
  stb_ctkprint( 0, "  Checking worst case agression.\n" );

  // Récupère la liste des agresseurs.
  eventlist   = stb_getchainevent( evtvic, STB_CTK_WORST_AGR, headagr );
  stb_debug_stab( stbfig, evtvic, eventlist );

  // Récupère la liste des agresseurs qui ne sont pas sur la même phase que
  // evtvic.
  alwaysagrlist = stb_diftdomain( stbfig, evtvic, eventlist );
  testagrlist = subchain( eventlist, alwaysagrlist );
 
  stb_log_chain( stbfig, alwaysagrlist, 
                 "  agressors located on another clock domain" 
               );
  // Récupère la liste des intervales qui recouvrent la victime.
  
  realagrlist = stb_overlap( stbfig, 
                             evtvic, 
                             testagrlist, 
                             STB_CTK_MARGIN, 
                             STB_STD 
                           );

  // Marque les event qu'il faut considérer comme agresseur
  for( chain = realagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    event->USER = addptype( event->USER, STB_CTK_MARKREALACTIF, NULL );
  }
  for( chain = alwaysagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    event->USER = addptype( event->USER, STB_CTK_MARKREALACTIF, NULL );
  }
 
  // Reporte l'information.
  for( chain = eventlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    if( getptype( event->USER, STB_CTK_MARKREALACTIF ) ) {
      event->USER = delptype( event->USER, STB_CTK_MARKREALACTIF );
    }
    else {
      param = stb_getrcxparamfromevent( event );
      oldactif = param->ACTIF;
      param->ACTIF = param->ACTIF & ~RCX_AGRWORST;
      if( param->ACTIF != oldactif ) {
        stb_ctkprint( 0, "    Removing agressor %s %s\n",
                      ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                     "DOWN",
                      event->ROOT->NAME
                    );
      }
    }
  }

  freechain( realagrlist );
  freechain( alwaysagrlist );
  freechain( testagrlist );
  stb_ctk_handle_mutex( stbfig->FIG, evtvic, eventlist, RCX_AGRWORST );
  stb_freechainevent( eventlist );
  
  /* BEST CASE */
    
  stb_ctkprint( 0, "  Checking best case agression.\n" );

  eventlist = stb_getchainevent( evtvic, STB_CTK_BEST_AGR, headagr );
  stb_debug_stab( stbfig, evtvic, eventlist );

  alwaysagrlist = stb_diftdomain( stbfig, evtvic, eventlist );
  testagrlist = subchain( eventlist, alwaysagrlist );
  
  stb_log_chain( stbfig, alwaysagrlist, 
                 "agressors located on another clock domain" 
               );
  realagrlist = stb_overlap( stbfig, 
                             evtvic, 
                             testagrlist, 
                             STB_CTK_MARGIN, 
                             STB_STD 
                           );
  
  for( chain = realagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    event->USER = addptype( event->USER, STB_CTK_MARKREALACTIF, NULL );
  }
  for( chain = testagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    event->USER = addptype( event->USER, STB_CTK_MARKREALACTIF, NULL );
  }
 
  for( chain = eventlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    if( getptype( event->USER, STB_CTK_MARKREALACTIF ) ) {
      event->USER = delptype( event->USER, STB_CTK_MARKREALACTIF );
    }
    else {
      param = stb_getrcxparamfromevent( event );
      oldactif = param->ACTIF;
      param->ACTIF = param->ACTIF & ~RCX_AGRBEST;
      if( param->ACTIF != oldactif ) {
        stb_ctkprint( 0, "    Removing agressor %s %s\n",
                      ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                     "DOWN",
                      event->ROOT->NAME
                    );
      }
    }
  } 

  freechain( realagrlist );
  freechain( alwaysagrlist );
  freechain( testagrlist );

  stb_ctk_handle_mutex( stbfig->FIG, evtvic, eventlist, RCX_AGRBEST );
  stb_freechainevent( eventlist );

  level = 0l; // unused parameter : avoid a warning at compilation
  type = 0l; // unused parameter : avoid a warning at compilation
}

/*****************************************************************************
*                           fonction stb_detectavtiveagressorbest()          *
******************************************************************************
* Positionne dans les structures rcxparam de la chain_list headagr les bits  *
* indiquant les agressions observables.                                      *
* L'approche est meilleurs cas : Aucune agression n'est initialement prise   *
* en compte et détectées progressivement.                                    *
*****************************************************************************/
void stb_detectactiveagressorbest( stbfig, level, type, evtvic, headagr )
stbfig_list       *stbfig;
long               level;
long               type;
ttvevent_list     *evtvic;
chain_list        *headagr;
{
  chain_list      *eventlist;
  stbnode         *stbvic;
  chain_list      *realagrlist;
  chain_list      *testagrlist;
  chain_list      *alwaysagrlist;
  ttvevent_list   *event;
  chain_list      *chain;
  rcxparam        *param;
  char             oldactif;
 
  stb_ctkprint( 1, 
                "Detecting best case agressor for event %s.\n", 
                evtvic->ROOT->NAME 
              );

  if( ! headagr ) return;
  
  stbvic = stb_getstbnode( evtvic );
  if( !stbvic ) {
    fflush( stdout );
    fprintf( stderr, "\n*** Error in STB/CTK : no stability on event.\n" );
    EXIT(1);
  }
 
  /* WORST CASE */
  
  stb_ctkprint( 0, "  Checking worst case agression.\n" );
 
  // Récupère la liste des event correspondant aux agresseurs.
  eventlist = stb_getchainevent( evtvic, STB_CTK_WORST_AGR, headagr );
  stb_debug_stab( stbfig, evtvic, eventlist );

  alwaysagrlist = stb_diftdomain( stbfig, evtvic, eventlist );
  testagrlist   = subchain( eventlist, alwaysagrlist );

  stb_log_chain( stbfig, alwaysagrlist, 
                 "agressors located on another clock domain" 
               );
  if( ( stbfig->CTKMODE & STB_CTK_OBSERVABLE ) == STB_CTK_OBSERVABLE ) 
    realagrlist = stb_overlap( stbfig, 
                               evtvic, 
                               testagrlist, 
                               STB_CTK_MARGIN, 
                               STB_OBS 
                             );
  else 
    realagrlist = stb_overlap( stbfig, 
                               evtvic, 
                               testagrlist, 
                               STB_CTK_MARGIN, 
                               STB_STD 
                             );

  for( chain = alwaysagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    param = stb_getrcxparamfromevent( event );
    oldactif = param->ACTIF;
    param->ACTIF = param->ACTIF | RCX_AGRWORST;
    if( param->ACTIF != oldactif ) {
      stb_ctkprint( 0, "    Setting agressor %s %s\n",
                    ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                   "DOWN",
                    event->ROOT->NAME
                  );
    }
  }
  for( chain = realagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    param = stb_getrcxparamfromevent( event );
    oldactif = param->ACTIF;
    param->ACTIF = param->ACTIF | RCX_AGRWORST;
    if( param->ACTIF != oldactif ) {
      stb_ctkprint( 0, "    Setting agressor %s %s\n",
                    ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                   "DOWN",
                    event->ROOT->NAME
                  );
    }
  }

  freechain( realagrlist );
  freechain( testagrlist );
  freechain( alwaysagrlist );

  /* remove aggression if they are in the same mutex. removed aggression are
     marked RCX_MTX_WORST */
  stb_ctk_handle_mutex( stbfig->FIG, evtvic, eventlist, RCX_AGRWORST );
  stb_freechainevent( eventlist );
  
  /* BEST CASE */
    
  stb_ctkprint( 0, "  Checking best case agression.\n" );
    
  eventlist = stb_getchainevent( evtvic, STB_CTK_BEST_AGR, headagr );
  stb_debug_stab( stbfig, evtvic, eventlist );

  alwaysagrlist = stb_diftdomain( stbfig, evtvic, eventlist );
  testagrlist   = subchain( eventlist, alwaysagrlist );

  stb_log_chain( stbfig, alwaysagrlist, 
                 "agressors located on another clock domain" 
               );
  if( ( stbfig->CTKMODE & STB_CTK_OBSERVABLE ) == STB_CTK_OBSERVABLE ) 
    realagrlist = stb_overlap( stbfig, 
                               evtvic, 
                               testagrlist, 
                               STB_CTK_MARGIN, 
                               STB_OBS 
                             );
  else 
    realagrlist = stb_overlap( stbfig, 
                               evtvic, 
                               testagrlist, 
                               STB_CTK_MARGIN,
                               STB_STD 
                             );

  for( chain = alwaysagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    param = stb_getrcxparamfromevent( event );
    oldactif = param->ACTIF;
    param->ACTIF = param->ACTIF | RCX_AGRBEST;
    if( param->ACTIF != oldactif ) {
      stb_ctkprint( 0, "    Setting agressor %s %s\n",
                    ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                   "DOWN",
                    event->ROOT->NAME
                  );
    }
  }
  for( chain = realagrlist ; chain ; chain = chain->NEXT ) {
    event = ((ttvevent_list*)chain->DATA);
    param = stb_getrcxparamfromevent( event );
    oldactif = param->ACTIF;
    param->ACTIF = param->ACTIF | RCX_AGRBEST;
    if( param->ACTIF != oldactif ) {
      stb_ctkprint( 0, "    Setting agressor %s %s\n",
                    ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" :
                                                                   "DOWN",
                    event->ROOT->NAME
                  );
    }
  }

  freechain( realagrlist );
  freechain( testagrlist );
  freechain( alwaysagrlist );

  /* remove aggression if they are in the same mutex. removed aggression are
     marked RCX_MTX_BEST */
  stb_ctk_handle_mutex( stbfig->FIG, evtvic, eventlist, RCX_AGRBEST );
  stb_freechainevent( eventlist );

  level = 0l; // unused parameter : avoid a warning at compilation
  type = 0l; // unused parameter : avoid a warning at compilation
}

/*****************************************************************************
*                           fonction stb_getminoldslope()                    *
*                           fonction stb_getmaxoldslope()                    *
*                           fonction stb_setminoldslope()                    *
*                           fonction stb_setmaxoldslope()                    *
******************************************************************************
* Récupère l'ancien le front mémorisé à l'itération précédante.              *
*****************************************************************************/
long stb_getminoldslope( node )
ttvevent_list *node;
{
  ptype_list    *ptl;
  ptl = getptype( node->USER, STB_CTK_OLDMINSLOPE );
  if( ptl )
    return (long) ptl->DATA ;
  return TTV_NOSLOPE ;
}

long stb_getmaxoldslope( node )
ttvevent_list *node;
{
  ptype_list    *ptl;
  ptl = getptype( node->USER, STB_CTK_OLDMAXSLOPE );
  if( ptl )
    return (long) ptl->DATA ;
  return TTV_NOSLOPE ;
}

void stb_setminoldslope( node, slope )
ttvevent_list *node;
long           slope;
{
  ptype_list *ptl;

  ptl = getptype( node->USER, STB_CTK_OLDMINSLOPE );
  if( !ptl ) {
    node->USER = addptype( node->USER, STB_CTK_OLDMINSLOPE, NULL );
    ptl = node->USER;
  }

  ptl->DATA = (void*) slope;
}

void stb_setmaxoldslope( node, slope )
ttvevent_list *node;
long           slope;
{
  ptype_list *ptl;

  ptl = getptype( node->USER, STB_CTK_OLDMAXSLOPE );
  if( !ptl ) {
    node->USER = addptype( node->USER, STB_CTK_OLDMAXSLOPE, NULL );
    ptl = node->USER;
  }

  ptl->DATA = (void*) slope;
}

void stb_cleanoldslope( node )
ttvevent_list *node;
{
  ptype_list *ptl;
  ptl = getptype( node->USER, STB_CTK_OLDMAXSLOPE );
  if( ptl )
    node->USER = delptype( node->USER, STB_CTK_OLDMAXSLOPE );
  ptl = getptype( node->USER, STB_CTK_OLDMINSLOPE );
  if( ptl )
    node->USER = delptype( node->USER, STB_CTK_OLDMINSLOPE );
}

/*****************************************************************************
*                           fonction stb_hasslopechanged()                   *
******************************************************************************
* Indique si le front sur le noeud a changé depuis la dernière itération.    *
* Renvoie STB_NO, STB_YES ou STB_UNK.                                        *
*****************************************************************************/

int stb_hasslopechanged( node, ttvfig, level, type )
ttvevent_list *node;
ttvfig_list *ttvfig;
long        level;
long        type;
{
  ttvdelay_list *delay;
  long           foldmin;
  long           foldmax;
  long           delta;

  delay = ttv_getnodedelay( node );
  if( delay ) {
    foldmin = stb_getminoldslope( node );
    foldmax = stb_getmaxoldslope( node );

    if( foldmax == TTV_NOSLOPE || foldmin == TTV_NOSLOPE )
      return STB_UNK;

    delta = foldmin - delay->FMIN ;
    if( delta < 0 ) delta = -delta;
    if( delta >= STB_CTK_MINSLOPECHANGE )
      return STB_YES;
      
    delta = foldmax - delay->FMAX ;
    if( delta < 0 ) delta = -delta;
    if( delta >= STB_CTK_MINSLOPECHANGE )
      return STB_YES;

    return STB_NO;
  }

  ttvfig=NULL; // unused parameter : avoid a warning at compilation
  level=0l; // unused parameter : avoid a warning at compilation
  type=0l; // unused parameter : avoid a warning at compilation
  return STB_UNK;
}

/*****************************************************************************
*                           fonction stb_saveslope()                         *
******************************************************************************
* Sauvegarde tous les fronts de la figure.                                   *
*****************************************************************************/

void stb_saveslope( node, ttvfig, level, type )
ttvevent_list *node;
ttvfig_list   *ttvfig;
long           level;
long           type;
{
  chain_list    *scan;
  ttvdelay_list *delay;
  ptype_list    *ptype;
  ttvline_list  *line;

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
    ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                   TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
    if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
      scan = (chain_list *)ptype->DATA ;
    else
      scan = NULL ;
  }
  else
  {
    ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                   TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
    if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
      scan = (chain_list *)ptype->DATA ;
    else
      scan = NULL ;
  }

  delay = ttv_getnodedelay( node );
  if( delay ) {
    stb_setminoldslope( node, delay->FMIN );
    stb_setmaxoldslope( node, delay->FMAX );
  }

  for(  ; scan ; scan = scan->NEXT ) {
    line = (ttvline_list*)scan->DATA;
    if( ( ( line->TYPE & TTV_LINE_RC ) != TTV_LINE_RC ) ||
        !ttv_islinelevel( ttvfig, line, level )           ) 
      continue ;

    node = line->ROOT;
    delay = ttv_getnodedelay( node );
    if( delay ) {
      stb_setminoldslope( node, delay->FMIN );
      stb_setmaxoldslope( node, delay->FMAX );
    }
  }
}

/*****************************************************************************
*                           fonction stb_needeval()                          *
******************************************************************************
* Indique si le node doit être réévalué. Renvoie STB_YES, STB_NO ou STB_UNK. *
*****************************************************************************/

int stb_needeval( ttvfig, level, node, type, mode, headagr )
ttvfig_list     *ttvfig ;
long             level ;
ttvevent_list   *node ;
long             type ;
char             mode ;
chain_list      *headagr ;
{
  ttvline_list  *line ;
  ttvevent_list *nodefrom ;
  int            thereisunk = 0;
  chain_list    *scan;
  chain_list    *chain;
  rcxparam      *param;
  int            i;
  ttvsig_list   *signal;
  ptype_list    *ptype;

  if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
  {
    ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                   TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
    line = node->INLINE ;
  }
  else
  {
    ttv_expfigsig (ttvfig,node->ROOT, level, ttvfig->INFO->LEVEL, 
                   TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
    line = node->INPATH ;
  }

  for(; line != NULL ; line = line->NEXT)
  {
    if( ((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC) ||
        !ttv_islinelevel( ttvfig, line, level )        )
      continue ;

    nodefrom = line->NODE;

    switch ( stb_hasslopechanged( nodefrom, ttvfig, level, type ) ) {
    case STB_YES :
      return STB_YES;
      break;
    case STB_NO :
      break;
    case STB_UNK :
      thereisunk=1;
      break;
    }
  }

  for( scan = headagr ; scan ; scan = scan->NEXT ) {
  
    param = (rcxparam*)scan->DATA;
    signal = stb_getttvsigrcxparam( param );
    if( !signal )
      continue;
    
    if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
    {
      ttv_expfigsig (ttvfig,signal,level,ttvfig->INFO->LEVEL, 
                     TTV_STS_CLS_FED|TTV_STS_DUAL_FED, TTV_FILE_DTX);
    }
    else
    {
      ttv_expfigsig (ttvfig,signal,level,ttvfig->INFO->LEVEL, 
                     TTV_STS_CL_PJT|TTV_STS_DUAL_PJT, TTV_FILE_TTX);
    }

    for( i=0; i <=1 ; i++ ) {
    
      nodefrom = &(signal->NODE[i]);
      
      switch ( stb_hasslopechanged( nodefrom, ttvfig, level, type ) ) {
      case STB_YES :
        return STB_YES;
        break;
      case STB_NO :
        break;
      case STB_UNK :
        thereisunk=1;
        break;
      }

      if((type & TTV_FIND_LINE) == TTV_FIND_LINE)
      {
        if((ptype = getptype(node->USER,TTV_NODE_DUALLINE)) != NULL)
          chain = (chain_list *)ptype->DATA ;
        else
          chain = NULL ;
      }
      else
      {
        if((ptype = getptype(node->USER,TTV_NODE_DUALPATH)) != NULL)
          chain = (chain_list *)ptype->DATA ;
        else
          chain = NULL ;
      }

      for( ; chain ; chain = chain->NEXT ) {

        line = (ttvline_list *)chain->DATA ;

        if( ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
            !ttv_islinelevel( ttvfig, line, level )           )
          continue ;

        nodefrom = line->ROOT;
        switch ( stb_hasslopechanged( nodefrom, ttvfig, level, type ) ) {
        case STB_YES :
          return STB_YES;
          break;
        case STB_NO :
          break;
        case STB_UNK :
          thereisunk=1;
          break;
        }
      }
    }
  }

  mode=0; // unused parameter : avoid a warning at compilation

  if( thereisunk == 1 )
    return STB_UNK;
  return STB_NO;
}

void stb_remove_input( ttvfig_list *ttvfig, ttvsig_list *ttvsig, long level, chain_list *headagr )
{
  chain_list *toclean ;
  chain_list *chain ;
  rcxparam   *param ;

  toclean = stb_mark_input( ttvfig, ttvsig, level );

  for( chain = headagr ; chain ; chain = chain->NEXT ) {

    param   = (rcxparam*)(chain->DATA);

    ttvsig = stb_getttvsigrcxparam( param );
    
    if( ttvsig  ) {
      if( stb_is_mark_input( ttvsig ) ) {
        param->ACTIF = RCX_QUIET ;
      }
    }
  }
  
  stb_clean_mark_input(toclean);
}

/*****************************************************************************
*                           fonction stb_calcctkdelaynode()                  *
*****************************************************************************/
int stb_calcctkdelaynode(stbfig,node,level,type,mode,deltamax, iteration, debugmode)
stbfig_list     *stbfig;
ttvevent_list   *node;
long             level ;
long             type ;
char             mode ;
long            *deltamax ;
int              iteration ;
int              debugmode ;
{ 
 int res = 0 ;
 char           *insname;
 losig_list     *sigvic;
 chain_list     *chainfig;
 chain_list     *headagr;
 lofig_list     *figvic;
 
 chain_list     *chain;
 rcxparam       *param;
 ttvsig_list    *ttvsig;
 int             enablenode;

 stb_ctkprint( 0, "Computing delays for node %s %s (netname=%s)\n",
               (node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" : "DOWN",
               node->ROOT->NAME,
               node->ROOT->NETNAME
             );

 if( debugmode ) {
   enablenode = 0 ;
   if( getptype( node->ROOT->USER, STB_MARK_DEBUG ) )
     enablenode = 1 ;
 }
 else
   enablenode = 1 ;

 if( enablenode ) {
   sigvic = ttv_getlosigfromevent( stbfig->FIG, 
                                   node->ROOT, 
                                   &insname, 
                                   &chainfig, 
                                   &figvic 
                                 );
 }
 else {
   sigvic   = NULL ;
   insname  = NULL ;
   figvic   = NULL ;
   chainfig = NULL ;
 }

 if( sigvic ) {
 
   headagr = rcx_getagrlist( figvic, sigvic, insname, chainfig );
   freechain( chainfig );

   if( headagr ) {

     stb_fillttvsigrcxparam( stbfig->FIG, level, type, headagr );

     if( ( stbfig->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST ) {
       stb_fillinactifrcxparam( stbfig->FIG, node, headagr, 'N' );
       if( iteration > 0 )
         stb_detectactiveagressorworst( stbfig, level, type, node, headagr );
       stb_remove_input( stbfig->FIG, node->ROOT, level, headagr );
       res = stb_saveinactifrcxparam( stbfig->FIG, node, headagr );
     }
     else {
       stb_fillactifrcxparam( stbfig->FIG, node, headagr, 'N' );
       if( iteration > 0 )
         stb_detectactiveagressorbest( stbfig, level, type, node, headagr );
       stb_remove_input( stbfig->FIG, node->ROOT, level, headagr );
       res = stb_saveactifrcxparam( stbfig->FIG, node, headagr );
     }
   }
   else {
     stb_ctkprint( 0, "-> No agressor.\n" );
     stb_debug_stab( stbfig, node, NULL );
   }
 }
 else {
   headagr = NULL;
   stb_debug_stab( stbfig, node, NULL );
 }

 stb_saveslope( node, stbfig->FIG, level, type );
 if( enablenode == 1 && (
       res || 
       stb_needeval( stbfig->FIG, level, node, type, mode, headagr ) != STB_NO ||
       iteration <= 1 // pour avoir au moins une fois la propagation des fronts
     )
   ) {
   
   if( CTK_LOGFILE ) {
   
     stb_ctkprint( 0, 
                   "*** Aggressor for computing %s %s\n",
                   (node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP" : "DW",
                   node->ROOT->NAME
                 );
                  
     for( chain = headagr ; chain ; chain = chain->NEXT ) {
       param = (rcxparam*)chain->DATA ;
       ttvsig = stb_getttvsigrcxparam( param );
       if( ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST )
         stb_ctkprint( 0, "B" );
       else
         stb_ctkprint( 0, " " );
       if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST )
         stb_ctkprint( 0, "W " );
       else
         stb_ctkprint( 0, "  " );
       stb_ctkprint( 0, 
                     "%s (%s)\n",
                     ttvsig ? ttvsig->NAME : "-no ttvsig-",
                     rcx_getsigname( param->SIGNAL )
                   );
        
     }
   }
   *deltamax = ttv_calcnodedelayslope(stbfig->FIG,level,node,type,mode) ;
   STBCTK_NBDELAYCALC++;
 }
 else
   ttv_updatenodedelayslope( stbfig->FIG,level,node,type,mode );

 if( headagr ) {
 
   stb_delttvsigrcxparam( headagr );
   rcx_freeagrlist( figvic, sigvic, headagr );

 }

 stb_set_min_slope(stbfig->FIG, node, level, 2);
 stb_release_fast_nodepair(node);

 /*
 if( rcn_islock_signal( figvic, sigvic ) == YES ) 
   printf( "lock\n" );
 */
 return(res) ;
}

/*****************************************************************************
*                           fonction stb_calcctkdelay()                      *
*****************************************************************************/
int stb_calcctkdelay(stbfig ,deltamax, iteration )
stbfig_list *stbfig;
long        *deltamax;
int          iteration;
{
 ttvevent_list *ptevent;
 chain_list     *chain;
 long level ;
 long type = 0 ;
 char mode ;
 int res = 0 ;
 long delta = 0;
 int  pb_max, pb_cur;
 static int debugmode=0;

 STBCTK_NBDELAYCALC=0;
 
 if( deltamax ) *deltamax = 0;

 if (stbfig->GRAPH == STB_RED_GRAPH)
  {
   type = TTV_FIND_PATH ;
   fflush( stdout );
   fprintf( stderr, "\n*** Error in STB/CTK: Crosstalk analysis requires detailed graph analysis.\n" );
   EXIT(1);
  }
 else if (stbfig->GRAPH == STB_DET_GRAPH)
  {
   type = TTV_FIND_LINE ;
  }

 type |= TTV_FIND_MIN | TTV_FIND_MAX ;

 mode = TTV_MODE_DELAY ;

 if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
   level = stbfig->FIG->INFO->LEVEL ;
 else
   level = 0 ;

 if( iteration == 0 ) {
   debugmode = stb_init_debug_node( stbfig, level, type );
 }

 for(chain = stbfig->NODE, pb_max=0; chain ; chain = chain->NEXT, pb_max++ );

 CTK_PROGRESSBAR( iteration, pb_max, 0, NULL );
 for(chain = stbfig->NODE, pb_cur=1; chain ; chain = chain->NEXT, pb_cur++ ) {
    CTK_PROGRESSBAR( iteration, pb_max, pb_cur, NULL );

    ptevent = (ttvevent_list *)chain->DATA;

    if( !ptevent->INLINE ||
        ( ptevent->INLINE && 
          (ptevent->INLINE->TYPE & TTV_LINE_RC) != TTV_LINE_RC )
      )
    {
      if(stb_calcctkdelaynode(stbfig,ptevent,level,type,mode, &delta, iteration, debugmode)
         != 0)
        res++ ;
      if( deltamax ) {
        if( delta > *deltamax )
          *deltamax = delta;
      }
    }
    if( NBPTYPE > 0l ) 
      printf( "ptevent=%p NBPTYPE %ld\n", ptevent, NBPTYPE );
   }
   
 return(res) ;
}

void stb_ctk_env(void) 
{
  char *env;
  long  t;
  char *ptend;
  double d;
  char  valid;
  
  STB_CTK_MINSLOPECHANGE = 2 * TTV_UNIT ;
  CTK_LOGFILE = NULL;
  
  env = getenv("CTK_LOGFILE");
  if( env ) {
    CTK_LOGFILE = mbkfopen( env, NULL, WRITE_TEXT );
  }
  env = getenv("CTK_LOGLEVEL");
  if( env ) {
    CTK_LOGLEVEL = atoi(env);
  }

    CTK_REPORT_DELTA_DELAY_MIN=V_INT_TAB[__CTK_REPORT_DELTA_DELAY_MIN].VALUE;
  
    CTK_REPORT_DELTA_SLOPE_MIN=V_INT_TAB[__CTK_REPORT_DELTA_SLOPE_MIN].VALUE;
  
    CTK_REPORT_NOISE_MIN=V_INT_TAB[__CTK_REPORT_NOISE_MIN].VALUE;
  
    if(V_FLOAT_TAB[__CTK_REPORT_CTK_MIN].VALUE<0.0 || V_FLOAT_TAB[__CTK_REPORT_CTK_MIN].VALUE>100.0 )
      stb_ctk_error( 4, 
                "Bad value for CTK_REPORT_CTK_MIN. Must be a floating number.\n"
                   );
    CTK_REPORT_CTK_MIN=V_FLOAT_TAB[__CTK_REPORT_CTK_MIN].VALUE;
  
    if(V_FLOAT_TAB[__STB_NOISE_DEFAULT_RESI].VALUE<0.0 )
      stb_ctk_error( 4, 
            "Bad value for STB_NOISE_DEFAULT_RESI. Must be a floating number.\n"
                   );
    STB_NOISE_DEFAULT_RESI=V_FLOAT_TAB[__STB_NOISE_DEFAULT_RESI].VALUE;

    if( V_BOOL_TAB[__STB_CTK_FASTMODE].VALUE ) {
      valid = 'y';
      STB_CTK_FASTMODE=1;
    } else {
      valid = 'y';
      STB_CTK_FASTMODE=0;
    }

    STB_CTK_MARGIN=V_INT_TAB[__STB_CTK_MARGIN].VALUE*TTV_UNIT;

    STB_CTK_MINSLOPECHANGE=V_INT_TAB[__STB_CTK_MINSLOPECHANGE].VALUE*TTV_UNIT;

    STB_CTK_MAXLASTITER=V_INT_TAB[__STB_CTK_MAXLASTITER].VALUE;

    stb_ctk_set_min_score( V_INT_TAB[__STB_CTK_MIN_NOISE].VALUE, -1, -1, -1 );

    stb_ctk_set_min_score( -1, V_INT_TAB[__STB_CTK_MIN_CTK].VALUE, -1, -1 );

    stb_ctk_set_min_score( -1, -1, V_INT_TAB[__STB_CTK_MIN_INTERVAL].VALUE, -1 );

    stb_ctk_set_min_score( -1, -1, -1, V_INT_TAB[__STB_CTK_MIN_ACTIVITY].VALUE );

    stb_ctk_set_coef_score( V_INT_TAB[__STB_CTK_COEF_NOISE].VALUE, -1, -1, -1 );

    stb_ctk_set_coef_score( -1, V_INT_TAB[__STB_CTK_COEF_CTK].VALUE, -1, -1 );

    stb_ctk_set_coef_score( -1, -1, V_INT_TAB[__STB_CTK_COEF_INTERVAL].VALUE, -1 );

    stb_ctk_set_coef_score( -1, -1, -1, V_INT_TAB[__STB_CTK_COEF_ACTIVITY].VALUE );

}

/*****************************************************************************
*                           fonction stb_ctk_clean()                         *
*****************************************************************************/

void stb_ctk_clean( stbfig )
stbfig_list *stbfig;
{
  chain_list    *chain;
  ttvevent_list *ptevent;
  
  for(chain = stbfig->NODE ; chain ; chain = chain->NEXT )
  {
    ptevent = (ttvevent_list *)chain->DATA;
    stb_cleanagressiontype( ptevent->ROOT );
  }

  CtkMutexFree(stbfig->FIG);
}

/*****************************************************************************
*                           fonction stb_ctk_clean_oldslope()                *
*****************************************************************************/

void stb_ctk_clean_oldslope( stbfig )
stbfig_list *stbfig;
{
  chain_list    *chain;
  ttvevent_list *ptevent;
  
  for(chain = stbfig->NODE ; chain ; chain = chain->NEXT )
  {
    ptevent = (ttvevent_list *)chain->DATA;
    stb_cleanoldslope( ptevent );
  }
}

/******************************************************************************\
Construit les tables de hash dans la hiérarchie TTV.
\******************************************************************************/
void stb_built_ttv_htab( ttvfig_list *rootfig )
{
  ttv_builthtabfig(  rootfig, TTV_STS_L | TTV_STS_S );
}

/******************************************************************************\
Remet à jour les infos de stb, notament les holorloges avec les nouveaux délais.
\******************************************************************************/
void stb_resync( stbfig_list *stbfig )
{
  chain_list *chain;
  ttvevent_list *ttvnode;
  stbnode       *stbnode;
  stbck         *ptstbck;

  for( chain = stbfig->NODE ; chain ; chain = chain->NEXT ) {
    ttvnode = (ttvevent_list*)chain->DATA;
    stbnode = stb_getstbnode( ttvnode );
    for( ptstbck = stbnode->CK ; ptstbck ; ptstbck = ptstbck->NEXT ) {
      if( ptstbck && ptstbck->TYPE != STB_TYPE_CLOCK ) {
        ptstbck->SUPMAX = STB_NO_TIME ;
        ptstbck->SUPMIN = STB_NO_TIME ;
        ptstbck->SDNMAX = STB_NO_TIME ;
        ptstbck->SDNMIN = STB_NO_TIME ;
      }
    }
    ttvnode->USER=testanddelptype (ttvnode->USER, STB_NODE_CLOCK);
  }
  stb_initclock( stbfig,1);
/*  for( chain = stbfig->NODE ; chain ; chain = chain->NEXT ) {
    ttvnode = (ttvevent_list*)chain->DATA;
    stb_initckpath( stbfig, ttvnode );
  }*/

  if(stbfig->GRAPH == STB_DET_GRAPH)
     stb_propagate_signal(stbfig, stbfig->CLOCK);

}

/******************************************************************************\
fonction stb_ctk()
Détermine les agressions et calcule les délais élémentaires en présence de 
crosstalk.
Après avoir appellé cette fonction, les agressions calculées sont accessibles
par la fonction stb_getagressiontype().
On libère cette information avec la fonction stb_ctk_clean().
\******************************************************************************/


int stb_ctk(stbfig)
stbfig_list *stbfig;
{
  int i ;
  int changedelay=1 ;
  char oldsilent ;
  long deltafrontmax=0;
  char doitagain;
  int  iterwithoutagr=0, level;
  stbfile *report;
  stb_ctk_stat *stat;
  char buf[80];
 
  oldsilent = STB_SILENT ;
  STB_SILENT = 'Y' ;

  #ifdef AVERTEC_LICENSE
    if(avt_givetoken("HITAS_LICENSE_SERVER","ctk")!=AVT_VALID_TOKEN)
          EXIT(1) ;
  #endif

  rcx_crosstalkactive( RCX_MILLER );
  rcx_crosstalk_analysis( YES );

  // pour les apis
  STB_COM_STBFIG=stbfig;
  STB_COM=stb_communication;

  stb_built_ttv_htab( stbfig->FIG );

  stb_ctk_clean(stbfig) ;

  CtkMutexInit( stbfig->FIG );

  if( stbfig->CTKMODE & STB_CTK_REPORT )
    report = stb_info_open( stbfig );
  else
    report = NULL ;

  CTK_PROGRESSBAR( 0, 1, 0, "Computing initial state." );
    
  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
   level = stbfig->FIG->INFO->LEVEL ;
  else
   level = 0 ;
  stb_setupall_slopes(stbfig, level);

  stb_calcctkdelay( stbfig, NULL, 0 ) ;
  stb_resync( stbfig );
  stb_cleanup_fast_nodepair(stbfig);
  
  stb_ctk_drive_iteration_report(stbfig, 0);

  CTK_PROGRESSBAR( 0, 1, 0, "Crosstalk analysis." );

  trcflushdelaycache();
  i=0;
  do
  {
   i++ ;
   stb_ctkprint( 0, "\nItération %d\n\n", i );

   stb_ctk_drive_iteration_report_save_last_iteration_info(stbfig);
   
//   stb_clean_relax_correction_info(stbfig);
   
   stb_relaxation(stbfig) ;

   changedelay = stb_calcctkdelay(stbfig, &deltafrontmax, i) ;
   stb_resync( stbfig );
   stb_cleanup_fast_nodepair(stbfig);
   
   sprintf( buf, "  -> %d signals evaluated", STBCTK_NBDELAYCALC );
   CTK_PRINTINFO( buf );

   if( ( stbfig->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST )
     sprintf( buf, "  -> %d signals with one or more agression removed",
              changedelay 
            );
   else
     sprintf( buf, "  -> %d signals with one or more new agression", 
              changedelay 
            );
   CTK_PRINTINFO( buf );
              
   sprintf( buf, "  -> Maximum slope variation : %.1fps", 
            deltafrontmax/TTV_UNIT 
          );
   CTK_PRINTINFO( buf );
  
   stb_ctk_drive_iteration_report(stbfig, i);

   // Condition de sortie de la boucle
   if( changedelay ) {
     doitagain=1;
     iterwithoutagr=0;
   }
   else {
     iterwithoutagr++;
     if( deltafrontmax <= STB_CTK_MINSLOPECHANGE ||
         iterwithoutagr >= STB_CTK_MAXLASTITER      )
       doitagain=0;
     else
       doitagain=1;
   }

   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","ctk")!=AVT_VALID_TOKEN)
           EXIT(1) ;
   #endif

   trcflushdelaycache();
  }
 while( doitagain && i < V_INT_TAB[__STB_CTK_MAX_ITER].VALUE );

 stb_ctk_drive_agression( stbfig );

 stb_cleanup_slopes(stbfig);

 if( report ) {
   stat = stb_ctk_fill_stat( stbfig, 1 );
   stb_ctk_drive_stat( report, stbfig, stat );
   stb_info_close( report );
 }

 //stb_display_mem( stbfig );
 CTK_PROGRESSBAR( 0, 1, 0, "Driving crosstalk file." );
  
 if( (stbfig->CTKMODE & STB_CTX_REPORT) == STB_CTX_REPORT )
     ttv_ctxdrive( stbfig->FIG );

 STB_SILENT = oldsilent ;

 stb_ctk_clean_oldslope( stbfig );

 ttv_freehtabfig( stbfig->FIG, TTV_STS_L | TTV_STS_S );

 STB_COM_STBFIG=NULL;
 STB_COM=NULL;

 rcx_crosstalkactive( RCX_NOCROSSTALK );
 rcx_crosstalk_analysis( NO );

 ctk_calc_constraint( stbfig );

 stb_clean_ttvfig( stbfig );
 
 return(i) ;
}

stbfile* stb_info_open( stbfig_list *stbfig )
{
 static stbfile file;

 file.FD = mbkfopen( stbfig->FIG->INFO->FIGNAME, "ctk", "w" );
 if( file.FD == NULL ) {
   stb_ctk_error( 5, "Can't open file %s.ctk for writting.\n",
                     stbfig->FIG->INFO->FIGNAME );
 }

 stb_info( &file, 
             "# Crosstalk information for circuit %s.\n\n",
             stbfig->FIG->INFO->FIGNAME
           );

 file.STBFIG = stbfig ;

 stb_info( &file, "Score configuration :    Coefficient  Miniumum\n" );
 stb_info( &file, "               Noise       %2d/10       %2d/10\n",
                  stb_ctk_get_coef_noise(), stb_ctk_get_min_noise() );
 stb_info( &file, "            Interval       %2d/10       %2d/10\n",
                  stb_ctk_get_coef_interval(), stb_ctk_get_min_interval() );
 stb_info( &file, "           Crosstalk       %2d/10       %2d/10\n",
                  stb_ctk_get_coef_ctk(), stb_ctk_get_min_ctk() );
 stb_info( &file, "            Activity       %2d/10       %2d/10\n",
                  stb_ctk_get_coef_activity(), stb_ctk_get_min_activity() );
 return &file;
}

void stb_info( stbfile *file, ... )
{
  va_list       index;
  char         *fmt;

  if( !file ) return;

  va_start( index, file );
  fmt = va_arg( index, char* );

  vfprintf( file->FD, fmt, index );
}

void stb_info_close( stbfile *file )
{
  if( !file ) return;
  
  fclose( file->FD );
}

void stb_drive_line( stbfile *report,
                     ttvfig_list *ttvfig, 
                     ttvline_list *line
                   )
{
  float           dmin;
  float           dmax;
  float           fmin;
  float           fmax;
  float           dmincc;
  float           dmaxcc;
  float           fmincc;
  float           fmaxcc;
  char            title=0;

  dmin = line->VALMIN/TTV_UNIT;
  dmax = line->VALMAX/TTV_UNIT;
  fmin = line->FMIN/TTV_UNIT;
  fmax = line->FMAX/TTV_UNIT;
  dmincc = ttv_getdelaymin( line )/TTV_UNIT;
  fmincc = ttv_getslopemin( line )/TTV_UNIT;
  dmaxcc = ttv_getdelaymax( line )/TTV_UNIT;
  fmaxcc = ttv_getslopemax( line )/TTV_UNIT;

  if( abs(dmin-dmincc) >= CTK_REPORT_DELTA_DELAY_MIN && 
      ttv_getdelaymin( line ) != TTV_NOTIME ) {
    if( title==0 ) {
      stb_info( report, 
                "  %-38s %-38s\n",
                stb_display_ttvevent( ttvfig, line->NODE ), 
                stb_display_ttvevent( ttvfig, line->ROOT ) 
              );
      title=1;
    }
    stb_info( report, "    delay min : %.1f -> %.1f\n", dmin, dmincc );
  }
  if( abs(dmax-dmaxcc) >= CTK_REPORT_DELTA_DELAY_MIN && 
      ttv_getdelaymax( line ) != TTV_NOTIME ) {
    if( title==0 ) {
      stb_info( report, 
                "  %-38s %-38s\n", 
                stb_display_ttvevent( ttvfig, line->NODE ), 
                stb_display_ttvevent( ttvfig, line->ROOT ) 
              );
      title=1;
    }
    stb_info( report, "    delay max : %.1f -> %.1f\n", dmax, dmaxcc );
  }
  if( abs(fmin-fmincc) >= CTK_REPORT_DELTA_SLOPE_MIN && 
      ttv_getslopemin( line ) != TTV_NOSLOPE ){
    if( title==0 ) {
      stb_info( report, 
                "  %-38s %-38s\n", 
                stb_display_ttvevent( ttvfig, line->NODE ), 
                stb_display_ttvevent( ttvfig, line->ROOT ) 
              );
      title=1;
    }
    stb_info( report, "    slope min : %.1f -> %.1f\n", fmin, fmincc );
  }
  if( abs(fmax-fmaxcc) >= CTK_REPORT_DELTA_SLOPE_MIN &&
      ttv_getslopemax( line ) != TTV_NOSLOPE ){
    if( title==0 ) {
      stb_info( report, 
                "  %-38s %-38s\n", 
                stb_display_ttvevent( ttvfig, line->NODE ), 
                stb_display_ttvevent( ttvfig, line->ROOT ) 
              );
      title=1;
    }
    stb_info( report, "    slope max : %.1f -> %.1f\n", fmax, fmaxcc );
  }

  ttvfig = NULL; //unused parameter
}

char* stb_display_ttvevent( ttvfig_list *ttvfig, ttvevent_list *node )
{
  static int i=0;
  static char tab[2][1024];
  char ttvname[1024];
  char netname[1024];
  i=(i+1)%2;

  ttv_getsigname( ttvfig, ttvname, node->ROOT );
  ttv_getnetname( ttvfig, netname, node->ROOT );
  if( strcmp( netname, ttvname ) == 0 ) {
    sprintf( tab[i], "%s %s", 
                     ( node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP": "DW",
                     ttvname
           );
  }
  else {
    sprintf( tab[i], "%s %s (%s)", 
                     ( node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "UP": "DW",
                     ttvname,
                     netname
           );
  }
   
  return tab[i];
}

char* stb_display_ttvevent_noise( ttvfig_list *ttvfig, ttvevent_list *node )
{
  static int i=0;
  static char tab[2][1024];
  char ttvname[1024];
  char netname[1024];
  i=(i+1)%2;

  ttv_getsigname( ttvfig, ttvname, node->ROOT );
  ttv_getnetname( ttvfig, netname, node->ROOT );
  if( strcmp( netname, ttvname ) == 0 ) {
    sprintf( tab[i], "%s %s", 
                   ( node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "LOW ": "HIGH",
                     ttvname
           );
  }
  else {
    sprintf( tab[i], "%s %s (%s)", 
                   ( node->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ? "LOW ": "HIGH",
                     ttvname,
                     netname
           );
  }
   
  return tab[i];
}

char* stb_display_ttvsig( ttvfig_list *fig, ttvsig_list *ttvsig )
{
  static int i=0;
  static char tab[2][1024];
  char ttvname[1024];
  char netname[1024];
  i=(i+1)%2;

  ttv_getsigname( fig, ttvname, ttvsig );
  ttv_getnetname( fig, netname, ttvsig );
  if( strcmp( netname, ttvname ) == 0 ) {
    sprintf( tab[i], "%s", ttvname );
  }
  else {
    sprintf( tab[i], "%s (%s)", ttvname, netname );
  }
   
  return tab[i];
}

stb_ctk_detail* stb_ctk_get_detail( stbfig_list *stbfig, 
                                    ttvevent_list *ptevent 
                                  )
{
  losig_list     *sigvic;
  char           *insname;
  chain_list     *chainfig;
  rcxparam       *param;
  lofig_list     *figvic;
  stb_ctk_detail *detail;
  stb_ctk_detail_agr_list *detail_list;
  long           level ;
  long           type=0 ;
  chain_list    *scanagr;
  chain_list    *headagr;
  RCXFLOAT       cm;
  RCXFLOAT       dcm;
  RCXFLOAT       cc;
  rcxmodagr     *tabagr;
  int            nbagr;
  int            n;
  ttvsig_list   *ttvagr;
  int            agridx;
  float          vth ;

  if (stbfig->GRAPH == STB_RED_GRAPH) {

    type = TTV_FIND_PATH ;
    fflush( stdout );
    fprintf( stderr, "\n*** Error in STB/CTK: Crosstalk analysis requires detailed graph analysis.\n" );
    EXIT(1);
  }
  else if (stbfig->GRAPH == STB_DET_GRAPH) {

    type = TTV_FIND_LINE ;
  }

  type |= TTV_FIND_MIN | TTV_FIND_MAX ;

  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL ;
  else
    level = 0 ;
    
  if( ptevent->INLINE && 
      (ptevent->INLINE->TYPE & TTV_LINE_RC) == TTV_LINE_RC 
    )
    return NULL;
    
  sigvic = ttv_getlosigfromevent( stbfig->FIG, 
                                  ptevent->ROOT, 
                                  &insname, 
                                  &chainfig,
                                  &figvic
                                );
  if( !sigvic ) return NULL;

  detail = (stb_ctk_detail*)mbkalloc( sizeof( stb_ctk_detail ) );
  detail->NODE          = ptevent;
  detail->LOSIG         = sigvic;
  detail->CM            = 0.0;
  detail->CC            = 0.0;
  detail->NOISE_OVR     = 0.0;
  detail->NOISE_UND     = 0.0;
  detail->NOISE_MAX_OVR = 0.0;
  detail->NOISE_MAX_UND = 0.0;
  detail->AGRLIST       = NULL;
  detail->GAP_UP.FILLED = 0 ;
  detail->GAP_DW.FILLED = 0 ;

  headagr = rcx_getagrlist( figvic, sigvic, insname, chainfig );
  stb_fillttvsigrcxparam( stbfig->FIG, level, type, headagr );
  if( ( stbfig->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST )
    stb_fillinactifrcxparam( stbfig->FIG, ptevent, headagr, 'Y' );
  else
    stb_fillactifrcxparam( stbfig->FIG, ptevent, headagr, 'Y' );
  
  tabagr = rcx_buildtabagr( figvic, 
                            sigvic, 
                            sizeof( rcxmodagr ), 
                            &nbagr, 
                            &cm 
                          );

  cc = 0.0;
  dcm = 0.0;
  for( n = 0 ; n < nbagr ; n++ ) {
    cc  = cc+tabagr[n].CLOCALE + tabagr[n].CGLOBALE;
    dcm = dcm + tabagr[n].CGLOBALE ;
  }

  detail->CM = cm - dcm ;
  detail->CC = cc ;

  for( scanagr = headagr ; scanagr ; scanagr = scanagr->NEXT ) {
    param = (rcxparam*)scanagr->DATA;

    detail_list = ( stb_ctk_detail_agr_list* )
                                  mbkalloc( sizeof( stb_ctk_detail_agr_list ) );

    detail_list->NEXT = detail->AGRLIST ;
    detail->AGRLIST   = detail_list ;

    detail_list->NOISE_RISE_PEAK = 0;
    detail_list->NOISE_FALL_PEAK = 0;
    detail_list->NOISE_RISE_EXCLUDED = 0;
    detail_list->NOISE_FALL_EXCLUDED = 0;

    ttvagr = stb_getttvsigrcxparam(param);
    detail_list->TTVAGR = ttvagr;
    if( ttvagr ) 
      ttvagr->USER = addptype( ttvagr->USER, STB_CTK_DETAIL, detail_list );
    else {
      detail_list->NOISE_RISE_PEAK = 1;
      detail_list->NOISE_FALL_PEAK = 1;
    }

    detail_list->NETNAME = (char*)mbkalloc( sizeof( char ) * 1024 );
    sprintf( detail_list->NETNAME, 
             "%s.%s", 
             param->INSNAME, 
             rcx_getsigname( param->SIGNAL )
           );
    
    if( ( param->ACTIF & RCX_AGRBEST  ) == RCX_AGRBEST  )
      detail_list->ACT_BEST  = 1;
    else
      detail_list->ACT_BEST  = 0;

   if( ( param->ACTIF & RCX_MTX_BEST ) == RCX_MTX_BEST )
     detail_list->ACT_MUTEX_BEST = 1 ;
   else
     detail_list->ACT_MUTEX_BEST = 0 ;
     
   if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST )
      detail_list->ACT_WORST = 1;
    else
      detail_list->ACT_WORST = 0;

   if( ( param->ACTIF & RCX_MTX_WORST ) == RCX_MTX_WORST )
     detail_list->ACT_MUTEX_WORST = 1 ;
   else
     detail_list->ACT_MUTEX_WORST = 0 ;

    agridx = rcx_gettabagrindex( param->SIGNAL );
    detail_list->CC = tabagr[agridx].CLOCALE + tabagr[agridx].CGLOBALE ;
  }

  rcx_freetabagr( tabagr, sizeof( rcxmodagr ), nbagr );

  detail->AGRLIST = (stb_ctk_detail_agr_list*)
                                        reverse( (chain_list*)detail->AGRLIST );
  
  stb_ctk_noise_node( stbfig, 
                      figvic,
                      detail,
                      headagr,
                      level, 
                      type,
                     &detail->MODEL_OVR,
                     &detail->NOISE_MAX_OVR,
                     &detail->NOISE_OVR,
                     &detail->MODEL_UND,
                     &detail->NOISE_MAX_UND,
                     &detail->NOISE_UND
                    );
                      
  vth = stb_ctk_signal_threshold( stbfig, detail->NODE );
  if( vth < 0.0 )
    vth = stb_ctk_signal_threshold_from_input( stbfig, detail->NODE ) ;
  detail->NOISE_VTH = vth ;

  stb_delttvsigrcxparam( headagr );
  rcx_freeagrlist( figvic, sigvic, headagr );
  freechain( chainfig ); chainfig = NULL ;

  return detail;
}

stb_ctk_gap* stb_fill_gap( stbfig_list *stbfig, stb_ctk_detail *detail, char transition )
{
  stb_ctk_gap             *gap ;
  stb_ctk_detail_agr_list *agr ;
  chain_list              *eventlist ;
  int                      node ;
 
  if( transition == TTV_NODE_UP ) {
    node = 1 ;
    gap = & detail->GAP_UP ;
  }
  else {
    node = 0 ;
    gap = & detail->GAP_DW ;
  }

  if( gap->FILLED )
    return gap ;

  eventlist = NULL ;
  gap->OTHERDOMAIN = NULL ;
  gap->CONDACTIF   = NULL ;
  gap->GAPACTIF    = NULL ;
  gap->NOINFO      = NULL ;

  for( agr = detail->AGRLIST ; agr ; agr = agr->NEXT ) {

    if( agr->TTVAGR ) {
      eventlist = addchain( eventlist, &( agr->TTVAGR->NODE[node] ) );
    }
    else {
      gap->NOINFO = addchain( gap->NOINFO, agr );
    }
  }
    
  gap->OTHERDOMAIN = stb_diftdomain( stbfig, detail->NODE, eventlist );
  gap->CONDACTIF = subchain( eventlist, gap->OTHERDOMAIN );

  gap->GAPACTIF = stb_overlapdev( stbfig, detail->NODE, gap->CONDACTIF, 0l );
  freechain( eventlist );

  gap->FILLED = 1 ;

  return gap ;
}

void stb_ctk_free_detail( stbfig_list *stbfig, stb_ctk_detail *detail )
{
  stb_ctk_detail_agr_list       *list;
  stb_ctk_detail_agr_list       *next;
  ttvsig_list                   *ttvsig;
  ptype_list                    *ptl;
  
  for( list = detail->AGRLIST ; list ; list = next ) {
  
    ttvsig  = list->TTVAGR ;
    if( ttvsig ) {
      ptl = getptype( ttvsig->USER, STB_CTK_DETAIL );
      if( !ptl ) {
        fprintf( stderr, "fatal error in stb_ctk_free_detail().\n" );
        EXIT(1);
      }
      ttvsig->USER = delptype( ttvsig->USER, STB_CTK_DETAIL );
    }
    
    next   = list->NEXT ;
    mbkfree( list->NETNAME ) ;
    mbkfree( list ) ;
  }

  if( detail->GAP_UP.FILLED ) {
    freechain( detail->GAP_UP.OTHERDOMAIN );
    freechain( detail->GAP_UP.CONDACTIF );
    freechain( detail->GAP_UP.NOINFO );
    stb_freegaplist( detail->GAP_UP.GAPACTIF );
  }
  if( detail->GAP_DW.FILLED ) {
    freechain( detail->GAP_DW.OTHERDOMAIN );
    freechain( detail->GAP_DW.CONDACTIF );
    freechain( detail->GAP_DW.NOINFO );
    stb_freegaplist( detail->GAP_DW.GAPACTIF );
  }
  
  mbkfree( detail );
  stbfig=NULL;
}

stb_ctk_detail_agr_list* stb_ctk_get_node_detail( ttvsig_list *ttvsig )
{
  ptype_list *ptl;
  ptl = getptype( ttvsig->USER, STB_CTK_DETAIL );
  if( ptl )
    return (stb_ctk_detail_agr_list*)ptl->DATA;
  return NULL;
}

void stb_ctk_drive_stat( stbfile *report, stbfig_list *stbfig, stb_ctk_stat *stat )
{
  int i;
  ttvevent_list *event ;
  stb_ctk_detail *ctkdetail ;

  CTK_PROGRESSBAR( 0, 1, 0, "Driving crosstalk report file" );
    
  stb_info( report, "BeginDelay\n" );
  for( i=0 ; i<stat->NBDDISPLAY ; i++ ) {
    CTK_PROGRESSBAR( 0, stat->NBDDISPLAY, i, NULL );
    stb_drive_line( report, stbfig->FIG, stat->TABD[i].LINE );
  }
  stb_info( report, "EndDelay\n\n" );
  CTK_PROGRESSBAR( 0, stat->NBDDISPLAY, stat->NBDDISPLAY, NULL );

  CTK_PROGRESSBAR( 0, 1, 0, "Driving detailled aggression in report file" );
  
  stb_drive_crosstalk_start( report );
  for( i=0 ; i<stat->NBDISPLAY ; i++ ) {
    CTK_PROGRESSBAR( 0, stat->NBDISPLAY, i, NULL );
    event = stat->TAB[i].NODE ;
    ctkdetail = stb_ctk_get_detail( stbfig, event );
    stb_drive_crosstalk_detail( report, ctkdetail, &(stat->TAB[i]) );
    stb_ctk_free_detail( stbfig, ctkdetail );
  }
  stb_drive_crosstalk_end( report );
  CTK_PROGRESSBAR( 0, stat->NBDISPLAY, stat->NBDISPLAY, NULL );
 
  stb_ctk_noise_report( report, stat );
}

int stb_drive_crosstalk_start( stbfile *report )
{
 stb_info( report, "# Crosstalk information :\n" ); 
 stb_info( report, "# -----------------------\n\n" );
 stb_info( report, "BeginCrosstalk\n" );
 return 1;
}

int sort_drive_detail( stb_ctk_detail_agr_list **ag1, stb_ctk_detail_agr_list**ag2 )
{
  if( (*ag1)->CC > (*ag2)->CC ) return  1 ;
  if( (*ag1)->CC < (*ag2)->CC ) return -1 ;
  return strcmp( (*ag1)->NETNAME, (*ag2)->NETNAME );
}

int stb_drive_crosstalk_detail( stbfile *report, 
                                stb_ctk_detail *detail, 
                                stb_ctk_tab_stat *stat 
                              )
{
  float                    pct ;
  stb_ctk_detail_agr_list *list ;
  int                      x, y ;
  stb_ctk_detail_agr_list **sort_tab;
 
  x = 10 * ( stb_ctk_get_coef_noise()    +
             stb_ctk_get_coef_activity() +
             stb_ctk_get_coef_ctk()      +
             stb_ctk_get_coef_interval()   );

  pct = detail->CM+detail->CC > 0.0 ? detail->CC/(detail->CM+detail->CC):0.0;
  pct = 100.0*pct;

  if( pct > CTK_REPORT_CTK_MIN ) {
  
    stb_info( report, "  Node : %s\n", 
                      stb_display_ttvevent( report->STBFIG->FIG, detail->NODE )
            );
    stb_info( report, "    Ground capacitance : %fpF\n", detail->CM );

    stb_info( report, "    Score : Total               %3d/%d\n", 
                      stb_ctk_get_score_total( stat ), x );
    stb_info( report, "            Noise               %3d/10\n", 
                      stb_ctk_get_score_noise( stat ) );
    stb_info( report, "            Interval            %3d/10\n", 
                      stb_ctk_get_score_interval( stat ) );
    stb_info( report, "            Ctk                 %3d/10\n", 
                      stb_ctk_get_score_ctk( stat ) );
    stb_info( report, "            Activity            %3d/10\n", 
                      stb_ctk_get_score_activity( stat ) );

    for( list = detail->AGRLIST, x=0 ; list ; list = list->NEXT, x++ ) ;
    sort_tab = (stb_ctk_detail_agr_list**)mbkalloc( sizeof( stb_ctk_detail_agr_list* ) * x );
    
    for( list = detail->AGRLIST, x=0 ; list ; list = list->NEXT, x++ )
      sort_tab[x] = list ;

    qsort( sort_tab, x, sizeof( stb_ctk_detail_agr_list* ), (int(*)(const void*,const void*))sort_drive_detail );

    stb_info( report, "    Agressor :\n" );
    
    for( y=0 ; y<x ; y++) {

      list = sort_tab[y];

      if( list->TTVAGR ) 
        stb_info( report, "      %-65s ", 
          stb_display_ttvsig(report->STBFIG->FIG, list->TTVAGR) );
      else
        stb_info( report, "    * %-65s ", list->NETNAME );
     
      if( list->ACT_BEST ) 
        stb_info( report, "B " ); 
      else {
        if( list->ACT_MUTEX_BEST )
          stb_info( report, "b " ); 
        else
          stb_info( report, "  " );
      }
        
      if( list->ACT_WORST ) 
        stb_info( report, "W " ); 
      else {
        if( list->ACT_MUTEX_WORST )
          stb_info( report, "w " ); 
        else
          stb_info( report, "  " );
      }
        
      if( list->NOISE_RISE_PEAK )
        stb_info( report, "R " );
      else { 
        if( list->NOISE_RISE_EXCLUDED )
          stb_info( report, "r " );
        else
          stb_info( report, "  " );
      }
        
      if( list->NOISE_FALL_PEAK )
        stb_info( report, "F " );
      else {
        if( list->NOISE_FALL_EXCLUDED )
          stb_info( report, "f " );
        else
          stb_info( report, "  " );
      }
        
      stb_info( report, " Cc=%fpF\n", list->CC );
    }

    mbkfree( sort_tab );
    stb_info( report, "  %-65s                 ----------\n", "" );
    stb_info( report, "  %-65s                 %f (%05.1f%%)\n", 
                      "", 
                      detail->CC,
                      pct
            );
  }
  return 1;
}

int stb_drive_crosstalk_end( stbfile *report )
{
  stb_info( report, "EndCrosstalk\n\n" );
  return 1;
}

void ctk_setprint( void (*print)( char* ) )
{
  CTK_PRINTINFO = print ;
}

void ctk_setprogressbar( void (*callfn)( int, int, int, char* ) )
{
  CTK_PROGRESSBAR = callfn ;
}

void ctk_display_event( stbfig_list *stbfig, ttvevent_list *refevent )
{
    stbpair_list  *refpair;
    stbpair_list  *pair;
    
    refpair = stb_getpairnode( stbfig, refevent, STB_CTK_MARGIN );
    for( pair = refpair ; pair ; pair = pair->NEXT )
      stb_ctkprint( 1, "[%.1f;%.1f] ", pair->D/TTV_UNIT, pair->U/TTV_UNIT );
    stb_freestbpair (refpair);
    stb_ctkprint( 1, "\n" );
}

void ctk_calc_constraint( stbfig_list *stbfig )
{
  long           level;
  char           mode;
  long           type;
  chain_list    *chain;
  ttvevent_list *event;
  
  mode = TTV_MODE_DELAY ;

  level = stbfig->FIG->INFO->LEVEL ;
  type = TTV_FIND_LINE ;
   
  for( chain = stbfig->NODE ; chain ; chain = chain->NEXT ) {
    event = (ttvevent_list*)chain->DATA ;
    ttv_calcnodeconstraint( stbfig->FIG, level, event, type, mode );
  }
}

void stb_ctk_set_stat( stbfig_list *stbfig, stb_ctk_stat *stat )
{
  ptype_list   *ptl ;
  stb_ctk_stat *old;
  
  ptl = getptype( stbfig->USER, STB_CTK_STAT );
  if( ptl ) {
    old = (stb_ctk_stat*)ptl->DATA ;
    stb_ctk_free_stat( old );
  }
  else {
    stbfig->USER = addptype( stbfig->USER, STB_CTK_STAT, NULL );
    ptl = stbfig->USER ;
  }

  ptl->DATA = stat ;
}

stb_ctk_stat* stb_ctk_get_stat( stbfig_list *stbfig )
{
  ptype_list    *ptl ;
  stb_ctk_stat  *stat ;
  
  ptl = getptype( stbfig->USER, STB_CTK_STAT );
  if( ptl ) {
    stat = (stb_ctk_stat*)ptl->DATA ;
  }
  else {
    stat = NULL ;
  }

  return stat ;
}

void stb_ctk_clean_stat( stbfig_list *stbfig )
{
  ptype_list    *ptl ;
  stb_ctk_stat  *stat ;
  
  ptl = getptype( stbfig->USER, STB_CTK_STAT );
  if( ptl ) {
    stat = (stb_ctk_stat*)ptl->DATA ;
    stb_ctk_free_stat( stat );
    stbfig->USER = delptype( stbfig->USER, STB_CTK_STAT );
  }
}

/******************************************************************************\
stb_ctk_infos()
Fill a stb_ctk_stat structure. 
Possibly, call user's fonction fn_xxxx for each detail corresponding to an event.
\******************************************************************************/

stb_ctk_stat* stb_ctk_fill_stat( stbfig_list *stbfig, int fast )
{
  stb_ctk_stat          *stat ;
  long level;

  stb_ctk_clean_stat( stbfig );

  stat = mbkalloc( sizeof( stb_ctk_stat ) );
  stat->TAB        = NULL ;
  stat->NBELEM     = 0 ;
  stat->NBDISPLAY  = 0 ;
  stat->TABD       = NULL ;
  stat->NBDELEM    = 0 ;
  stat->NBDDISPLAY = 0 ;
  
  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
   level = stbfig->FIG->INFO->LEVEL ;
  else
   level = 0 ;
  
  if (fast) stb_setupall_slopes(stbfig, level);

  
  stb_ctk_fill_stat_tab( stat, stbfig );
  stb_ctk_fill_stat_line( stat, stbfig );
  
  //CTK_PROGRESSBAR( 0, 1, 0, "Sorting results" );

  stb_ctk_sort_stat( stat, STB_CTK_SORT_SCORE_TOTAL );
  stb_ctk_sort_delay( stat, STB_CTK_SORT_ABS_DELAY, 0.0 );

  stb_ctk_set_stat( stbfig, stat );

  if (fast) stb_cleanup_slopes(stbfig);

  return stat ;
}

int stb_ctk_sort_delay_cmp( sortdelaycmp *delay1, sortdelaycmp *delay2 )
{
  int r ;
  int trs1n, trs1r, trs2n, trs2r, val1, val2 ;

  if( delay1->VALUE > delay2->VALUE ) return -1 ;
  if( delay1->VALUE < delay2->VALUE ) return  1 ;
  
  r = strcmp( delay1->LINE->NODE->ROOT->NAME, delay2->LINE->NODE->ROOT->NAME );
  if( r )
    return r ;
    
  r = strcmp( delay1->LINE->ROOT->ROOT->NAME, delay2->LINE->ROOT->ROOT->NAME );
  if( r )
    return r ;

  if( ( delay1->LINE->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
    trs1n = 1 ;
  else
    trs1n = 0 ;
    
  if( ( delay1->LINE->ROOT->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
    trs1r = 1 ;
  else
    trs1r = 0 ;
    
  if( ( delay2->LINE->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
    trs2n = 1 ;
  else
    trs2n = 0 ;
    
  if( ( delay2->LINE->ROOT->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
    trs2r = 1 ;
  else
    trs2r = 0 ;

  val1 = trs1n*2+trs1r ;
  val2 = trs2n*2+trs2r ;

  if( val1 < val2 )
    return -1 ;
  if( val1 > val2 )
    return  1 ;
  return 0 ;
}

void filltabdelayvalue( sortdelaycmp *elem, char criterion )
{
  float deltamax ;
  float deltamin ;
  float vnommax ;
  float vnommin ;
  float vctkmax ;
  float vctkmin ;

  deltamax    = 0.0 ;
  deltamin    = 0.0 ;
  elem->VALUE = 0.0 ;

  switch( criterion ) {
 
  case STB_CTK_SORT_ABS_DELAY :
  
    vnommax  = elem->LINE->VALMAX ;
    vctkmax  = ttv_getdelaymax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME )
      deltamax = vctkmax - vnommax ;

    vnommin  = elem->LINE->VALMIN ;
    vctkmin  = ttv_getdelaymin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME )
      deltamin = vnommin - vctkmin ;

    if( deltamax > deltamin )
      elem->VALUE = deltamax ;
    else
      elem->VALUE = deltamin ;

    break ;
   
  case STB_CTK_SORT_ABS_MAX_DELAY :
  
    vnommax  = elem->LINE->VALMAX ;
    vctkmax  = ttv_getdelaymax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME )
      deltamax = vctkmax - vnommax ;

    elem->VALUE = deltamax ;
    
    break ;
 
  case STB_CTK_SORT_ABS_MIN_DELAY :
  
    vnommin  = elem->LINE->VALMIN ;
    vctkmin  = ttv_getdelaymin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME )
      deltamin = vnommin - vctkmin ;

    elem->VALUE = deltamin ;

    break ;
   
  case STB_CTK_SORT_ABS_SLOPE :
  
    vnommax  = elem->LINE->FMAX ;
    vctkmax  = ttv_getslopemax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME )
      deltamax = vctkmax - vnommax ;

    vnommin  = elem->LINE->FMIN ;
    vctkmin  = ttv_getslopemin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME )
      deltamin = vnommin - vctkmin ;

    if( deltamax > deltamin )
      elem->VALUE = deltamax ;
    else
      elem->VALUE = deltamin ;

    break ;

  case STB_CTK_SORT_ABS_MAX_SLOPE :
  
    vnommax  = elem->LINE->FMAX ;
    vctkmax  = ttv_getslopemax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME )
      deltamax = vctkmax - vnommax ;

    elem->VALUE = deltamax ;
    
    break ;
 
  case STB_CTK_SORT_ABS_MIN_SLOPE :
  
    vnommin  = elem->LINE->FMIN ;
    vctkmin  = ttv_getslopemin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME )
      deltamin = vnommin - vctkmin ;

    elem->VALUE = deltamin ;

    break ;

  case STB_CTK_SORT_REL_DELAY :
  
    vnommax  = elem->LINE->VALMAX ;
    vctkmax  = ttv_getdelaymax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME && vnommax >= 1.0 )
      deltamax = (vctkmax - vnommax)/vnommax ;
      
    vnommin  = elem->LINE->VALMIN ;
    vctkmin  = ttv_getdelaymin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME && vnommin >= 1.0 )
      deltamin = (vnommin - vctkmin)/vnommin ;

    if( deltamax > deltamin )
      elem->VALUE = deltamax ;
    else
      elem->VALUE = deltamin ;

    break ;
   
  case STB_CTK_SORT_REL_MAX_DELAY :
  
    vnommax  = elem->LINE->VALMAX ;
    vctkmax  = ttv_getdelaymax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME && vnommax >= 1.0 )
      deltamax = (vctkmax - vnommax)/vnommax ;

    elem->VALUE = deltamax ;
    
    break ;
 
  case STB_CTK_SORT_REL_MIN_DELAY :
  
    vnommin  = elem->LINE->VALMIN ;
    vctkmin  = ttv_getdelaymin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME && vnommin >= 1.0 )
      deltamin = (vnommin - vctkmin)/vnommin ;

    elem->VALUE = deltamin ;

    break ;
   
  case STB_CTK_SORT_REL_SLOPE :
  
    vnommax  = elem->LINE->FMAX ;
    vctkmax  = ttv_getslopemax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME && vnommax >= 1.0 )
      deltamax = (vctkmax - vnommax)/vnommax ;

    vnommin  = elem->LINE->FMIN ;
    vctkmin  = ttv_getslopemin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME && vnommin >= 1.0 )
      deltamin = (vnommin - vctkmin)/vnommin ;

    if( deltamax > deltamin )
      elem->VALUE = deltamax ;
    else
      elem->VALUE = deltamin ;

    break ;

  case STB_CTK_SORT_REL_MAX_SLOPE :
  
    vnommax  = elem->LINE->FMAX ;
    vctkmax  = ttv_getslopemax( elem->LINE ) ;
    if( vnommax != TTV_NOTIME && vctkmax != TTV_NOTIME && vnommax >= 1.0 )
      deltamax = (vctkmax - vnommax)/vnommax ;

    elem->VALUE = deltamax ;
    
    break ;
 
  case STB_CTK_SORT_REL_MIN_SLOPE :
  
    vnommin  = elem->LINE->FMIN ;
    vctkmin  = ttv_getslopemin( elem->LINE ) ;
    if( vnommin != TTV_NOTIME && vctkmin != TTV_NOTIME && vnommin >= 1.0 )
      deltamin = (vnommin - vctkmin)/vnommin ;

    elem->VALUE = deltamin ;

    break ;
   
  }
}

void stb_ctk_sort_delay( stb_ctk_stat *stat, char criterion, float delay )
{
  sortdelaycmp *tabdelay;
  int           i ;
  
  tabdelay = (sortdelaycmp*)mbkalloc( sizeof(sortdelaycmp) * stat->NBDELEM );
  
  for( i=0 ; i<stat->NBDELEM ; i++ ) {
    tabdelay[i].LINE = stat->TABD[i].LINE ;
    filltabdelayvalue( &(tabdelay[i]), criterion );
  }
  
  qsort( tabdelay, stat->NBDELEM, sizeof( sortdelaycmp ), ( int(*)(const void*, const void*) )stb_ctk_sort_delay_cmp );

  for( i=0 ; i<stat->NBDELEM ; i++ ) {
    stat->TABD[i].LINE = tabdelay[i].LINE ;
  }

  mbkfree( tabdelay );
}

void stb_ctk_fill_stat_line( stb_ctk_stat *stat,
                             stbfig_list *stbfig
                           )
{
  long           level ;
  chain_list    *chain ;
  ttvevent_list *ptevent ;
  ttvline_list  *line ;
  ptype_list    *ptype ;
  int            nbline ;
  chain_list    *scline ;
 
  //CTK_PROGRESSBAR( 0, 1, 0, "Reporting delay variations" );

  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL ;
  else
    level = 0 ;

  nbline = 0 ;

  /* comptage du nombre de line */
  for(chain = stbfig->NODE ; chain ; chain = chain->NEXT )
  {
    ptevent = (ttvevent_list *)chain->DATA;

    ttv_expfigsig( stbfig->FIG,
                   ptevent->ROOT, 
                   level, 
                   stbfig->FIG->INFO->LEVEL, 
                   TTV_STS_CLS_FED|TTV_STS_DUAL_FED, 
                   TTV_FILE_DTX
                 );
                 
    line = ptevent->INLINE ;
    ptype = getptype( ptevent->USER, TTV_NODE_DUALLINE );
    if( ptype )
      scline = (chain_list *)ptype->DATA ;
    else
      scline = NULL ;

    for(; line != NULL ; line = line->NEXT) {
      if( ( ( line->TYPE & TTV_LINE_RC ) == TTV_LINE_RC ) ||
          ( ( line->TYPE & TTV_LINE_CONT ) == TTV_LINE_CONT ) ||
          !ttv_islinelevel( stbfig->FIG, line, level )       )
        continue ;
      nbline++;
    }

    for(; scline != NULL ; scline = scline->NEXT) {
      line = (ttvline_list *)scline->DATA ;

      if( ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
          !ttv_islinelevel( stbfig->FIG, line, level )        )
        continue ;
      nbline++;
    } 
  }

  stat->TABD = (stb_ctk_tab_delay*)mbkalloc( nbline * sizeof( stb_ctk_tab_delay ) );
  stat->NBDELEM    = nbline ;
  stat->NBDDISPLAY = nbline ;

  /* remplissage des infos */
  nbline = 0 ;
  for(chain = stbfig->NODE ; chain ; chain = chain->NEXT )
  {
    ptevent = (ttvevent_list *)chain->DATA;

    ttv_expfigsig( stbfig->FIG,
                   ptevent->ROOT, 
                   level, 
                   stbfig->FIG->INFO->LEVEL, 
                   TTV_STS_CLS_FED|TTV_STS_DUAL_FED, 
                   TTV_FILE_DTX
                 );
                 
    line = ptevent->INLINE ;
    ptype = getptype( ptevent->USER, TTV_NODE_DUALLINE );
    if( ptype )
      scline = (chain_list *)ptype->DATA ;
    else
      scline = NULL ;

    for(; line != NULL ; line = line->NEXT) {
      if( ( ( line->TYPE & TTV_LINE_RC ) == TTV_LINE_RC ) ||
          ( ( line->TYPE & TTV_LINE_CONT ) == TTV_LINE_CONT ) ||
          !ttv_islinelevel( stbfig->FIG, line, level )       )
        continue ;
      stb_ctk_fill_stat_line_one( &stat->TABD[nbline], line );
      nbline++;
    }

    for(; scline != NULL ; scline = scline->NEXT) {
      line = (ttvline_list *)scline->DATA ;

      if( ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC) ||
          !ttv_islinelevel( stbfig->FIG, line, level )        )
        continue ;
      stb_ctk_fill_stat_line_one( &stat->TABD[nbline], line );
      nbline++;
    } 
  }
}

char calc_pct( long vi, long vf )
{
  float fvi, fvf, pct ;
  char  ret=0 ;
  
  if( vi == 0 ) {
    ret = 0;
    if( vf > 0 )  ret =  100 ;
    if( vf < 0 )  ret = -100 ;
  }
  else {
    fvi = (float)vi ;
    fvf = (float)vf ;
    pct = ( fvf - fvi ) / fvi * 100.0 ;
    if( pct >  100.0 ) pct =  100.0 ;
    if( pct < -100.0 ) pct = -100.0 ;
    ret = (char)pct ;
  }
  
  return ret ;
}

void stb_ctk_fill_stat_line_one( stb_ctk_tab_delay *delay, ttvline_list *line )
{
  delay->LINE = line ;
}

void stb_ctk_fill_stat_tab( stb_ctk_stat *stat,
                            stbfig_list *stbfig
                          )
{
  stb_ctk_detail        *ctkdetail ;
  chain_list            *chain ;
  ttvevent_list         *event ;
  int                    nbevent ;
  int                    n ;
  int                    nm ;

  CTK_PROGRESSBAR( 0, 1, 0, "Computing noises and scores" );

    
  /* build and initialize data strucure */
  for( chain = stbfig->NODE, nbevent = 0 ; chain ; chain = chain->NEXT, nbevent++ ) ;
    
  if( nbevent == 0 )
    return ;
  
  CTK_PROGRESSBAR( 0, nbevent, 0, NULL );

  stat->TAB = mbkalloc( sizeof( stb_ctk_tab_stat ) * nbevent );
  for( n = 0 ; n < nbevent ; n++ ) {
    stat->TAB[n].NODE           = NULL ;
    stat->TAB[n].SCORE_NOISE    = 0 ;
    stat->TAB[n].SCORE_INTERVAL = 0 ;
    stat->TAB[n].SCORE_CTK      = 0 ;
    stat->TAB[n].SCORE_ACTIVITY = 0 ;
    stat->TAB[n].RISE_PEAK_REAL = 0 ;
    stat->TAB[n].FALL_PEAK_REAL = 0 ;
    stat->TAB[n].RISE_PEAK_MAX  = 0 ;
    stat->TAB[n].FALL_PEAK_MAX  = 0 ;
    stat->TAB[n].VTH            = 0.0 ;
    stat->TAB[n].NOISE_MODEL    = "" ;
    stat->TAB[n].CNET_GND       = 0.0 ;
    stat->TAB[n].CNET_CC        = 0.0 ;
    stat->TAB[n].CGATE          = 0.0 ;
  }
 
  /* filling report crosstalk information */
 
  n  = 0 ;
  nm = 1 ;
  for( chain = stbfig->NODE ; chain ; chain = chain->NEXT ) {
  
    event = (ttvevent_list*)chain->DATA ;

    CTK_PROGRESSBAR( 0, nbevent, nm, NULL );
    nm++ ;

    ctkdetail = stb_ctk_get_detail( stbfig, event );

    if( ctkdetail ) {
      stb_ctk_stat_event( stbfig, ctkdetail, &(stat->TAB[n]) );
      stb_ctk_calc_score( stbfig, ctkdetail, &(stat->TAB[n]) );
      stb_ctk_free_detail( stbfig, ctkdetail );
      n++;
    }
  }
  
  stat->NBELEM = n ;
  stat->NBDISPLAY = n ;

}

void stb_ctk_free_stat( stb_ctk_stat *stat )
{
  mbkfree( stat->TABD );
  mbkfree( stat->TAB );
  mbkfree( stat );
}

void stb_ctk_stat_event( stbfig_list        *stbfig,
                         stb_ctk_detail     *ctkdetail,
                         stb_ctk_tab_stat   *stat
                       )
{
  float capa ;

  /* Remplis les infos dont on dispose immédiatement */
  
  stat->NODE           = ctkdetail->NODE ;
  
  stat->RISE_PEAK_REAL = ctkdetail->NOISE_OVR ;
  stat->FALL_PEAK_REAL = ctkdetail->NOISE_UND ;
  stat->RISE_PEAK_MAX  = ctkdetail->NOISE_MAX_OVR ;
  stat->FALL_PEAK_MAX  = ctkdetail->NOISE_MAX_UND ;
  stat->VTH            = ctkdetail->NOISE_VTH ;

  /* Dans ctk_detail, on différencie les modèles de bruit rise et fall.
     Dans la pratique, c'est le même. On conserve celui qui va vers vdd/2 */

  if( ( ctkdetail->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP ) 
    stat->NOISE_MODEL = ctkdetail->MODEL_OVR ;
  else
    stat->NOISE_MODEL = ctkdetail->MODEL_UND ;
  
  /* Remplis les infos de capa */

  capa = rcx_get_all_locon_capa( ctkdetail->LOSIG, TRC_SLOPE_UNK, TRC_CAPA_NOM, TRC_HALF );

  stat->CNET_CC  = ctkdetail->CC ;
  stat->CGATE    = capa ;
  stat->CNET_GND = ctkdetail->CM - capa ;
  if( stat->CNET_GND < 0.0 )
    stat->CNET_GND = 0.0 ;
  stbfig = NULL;
}

/******************************************************************************\
Si on est en mode STB_CTK_LINE :

Met à jour les champs delai et front des line avec crosstalk et nettoie les 
structures delay de la ttvfig si on est en mode line.
\******************************************************************************/

void stb_clean_ttvfig( stbfig_list *stbfig )
{
  if( ( stbfig->CTKMODE & STB_CTK_LINE ) == STB_CTK_LINE ) 
    ttv_movedelayline( stbfig->FIG, TTV_LINE_D | TTV_LINE_E | TTV_LINE_F );
}

/******************************************************************************\
stb_ctk_handle_mutex()

Enleve le flag RCX_AGRBEST ou RCX_AGRWORST des structures rcxparam des 
event qui ne peuvent pas se produire (mutex). Lorsqu'un de ces flags est
enlevé, on le remplace par RCX_MTX_BEST ou RCX_MTX_WORST pour le report.

\******************************************************************************/
void stb_ctk_handle_mutex( ttvfig_list *ttvfig, 
                           ttvevent_list *event, 
                           chain_list* headagr,
                           char actif 
                         )
{
  chain_list      *chain ;
  sortedmutex     *mutex ;
  sortedmutexlist *ml ;
  rcxparam        *param ;
  ttvevent_list   *node ;
  RCXFLOAT         ccmax ;
  rcxparam        *parammax ;

  mutex = stb_ctk_sort_by_mutex( ttvfig, event, headagr );
  if( !mutex ) return ;

  for( ml = mutex->LIST ; ml ; ml = ml->NEXT ) {

    if( ml->MUTEX == mutex->MUTEX ) {
    
      for( chain = ml->LIST ; chain ; chain = chain->NEXT ) {
      
        node = (ttvevent_list*)chain->DATA ;
        param = stb_getrcxparamfromevent( node );
        
        if( actif == RCX_AGRBEST ) {
          if( ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) 
            param->ACTIF = (param->ACTIF & ~RCX_AGRBEST) | RCX_MTX_BEST ;
        }
        else {
          if( ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST )
            param->ACTIF = (param->ACTIF & ~RCX_AGRWORST) | RCX_MTX_WORST ;
        }
      }
    }
    else {

      ccmax = 0.0 ;
      parammax = NULL ;
      
      for( chain = ml->LIST ; chain ; chain = chain->NEXT ) {
      
        node = (ttvevent_list*)chain->DATA ;
        param = stb_getrcxparamfromevent( node );
        
        if( ( actif == RCX_AGRBEST && 
              ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) ||
            ( actif == RCX_AGRWORST &&
              ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST )     ) {
              
          if( param->CC > ccmax ) {
            ccmax = param->CC ;
            parammax = param ;
          }
        }
      }
      
      if( parammax ) {
      
        for( chain = ml->LIST ; chain ; chain = chain->NEXT ) {
        
          node = (ttvevent_list*)chain->DATA ;
          param = stb_getrcxparamfromevent( node );
          
          if( actif == RCX_AGRBEST && 
              ( param->ACTIF & RCX_AGRBEST ) == RCX_AGRBEST ) {
              
            if( param != parammax )
              param->ACTIF = (param->ACTIF & ~RCX_AGRBEST) | RCX_MTX_BEST ;
          }
          
          if( actif == RCX_AGRWORST && 
              ( param->ACTIF & RCX_AGRWORST ) == RCX_AGRWORST ) {
              
            if( param != parammax )
              param->ACTIF = (param->ACTIF & ~RCX_AGRWORST) | RCX_MTX_WORST ;
          }
        }
      }
    }
  }
  stb_ctk_free_sortedmutex( ttvfig, mutex );
}

void stb_set_ctk_information(stbfig_list *stbfig, ttvevent_list *node, long type, ctk_exchange *res)
{
  char           *insname;
  chain_list     *chainfig; 
  long level;

  if((stbfig->STABILITYMODE & STB_STABILITY_LAST) == STB_STABILITY_LAST)
    level = stbfig->FIG->INFO->LEVEL ;
  else
    level = 0 ;

  res->sigvic = ttv_getlosigfromevent( stbfig->FIG, node->ROOT, &insname, &chainfig, &res->figvic );
  if( res->sigvic )
    {
      res->headagr = rcx_getagrlist( res->figvic, res->sigvic, insname, chainfig );
      freechain( chainfig );

      if( res->headagr ) 
        {

          stb_fillttvsigrcxparam( stbfig->FIG, level, type, res->headagr );

          if( ( stbfig->CTKMODE & STB_CTK_WORST ) == STB_CTK_WORST ) 
            {
              stb_fillinactifrcxparam( stbfig->FIG, node, res->headagr, 'N' );
            }
          else 
            {
              stb_fillactifrcxparam( stbfig->FIG, node, res->headagr, 'N' );
            }
          stb_remove_input( stbfig->FIG, node->ROOT, level, res->headagr );
        }
    }
  else
    res->headagr=NULL;
}

void stb_release_ctk_information(ctk_exchange *res)
{
  if( res->headagr )
    {
      stb_delttvsigrcxparam( res->headagr );
      rcx_freeagrlist( res->figvic, res->sigvic, res->headagr );
    }
}
