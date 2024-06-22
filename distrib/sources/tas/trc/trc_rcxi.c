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

Revision : $Revision: 1.8 $
Author   : $Author: gregoire $
Date     : $Date: 2007/07/04 12:49:42 $

*/

#include "trc.h"

/******************************************************************************\
rcxi_error

Chaine de caractère indiquant la raison d'une erreur sur la dernière fonction
rcxi appellée.

\******************************************************************************/

char rcxi_error[1024];

/******************************************************************************\
rcxi_getcapa()

Renvoie la capacité totale sur un net. 

Entrée

  lofig
  losig
  level         RCXI_UP, RCXI_DW (valeur)
  defslope      Front sans crosstalk en ps.
  type          RCXI_GND ou ( RCXI_CTK | RCXI_MAX | RCXI_MIN )

Sortie

  capa          La somme des capacités demandées.
\******************************************************************************/

int rcxi_getcapa( lofig_list *lofig,
                  losig_list *losig,
                  char        level,
                  float       defslope,
                  int         type,
                  float      *capa
                )
{
  float c = 0.0 ;
  rcx_slope refslope ;

  switch( level ) {
  case RCXI_UP :
    refslope.SENS = TRC_SLOPE_UP ;
    break ;
  case RCXI_DW :
    refslope.SENS = TRC_SLOPE_DOWN ;
    break;
  default :
    sprintf( rcxi_error, "rcxi_getcapa() : bad value for level parameter." );
    return 0;
  }
  
  refslope.F0MAX  = defslope ;
  refslope.FCCMAX = defslope ;
  refslope.CCA    = -1.0 ;

  if( ( type & RCXI_GND ) == RCXI_GND ) {
    c    = rcx_getsigcapa( lofig, 
                           losig, 
                           RCX_SIGCAPA_GROUND, 
                           RCX_SIGCAPA_LOCAL, 
                           RCX_SIGCAPA_NORM,
                           NULL,
                           0,
                           TRC_HALF
                         ) - 
           rcx_getsigcapa( lofig,
                           losig,
                           RCX_SIGCAPA_CTK,
                           RCX_SIGCAPA_GLOBAL,
                           RCX_SIGCAPA_NORM,
                           NULL,
                           0,
                           TRC_HALF
                         );
  }

  if( ( type & RCXI_CTK ) == RCXI_CTK ) {
  
    if( ( type & RCXI_MAX ) == 0 && ( type & RCXI_MIN ) == 0 ) 
      c    = rcx_getsigcapa( lofig,
                             losig,
                             RCX_SIGCAPA_CTK,
                             RCX_SIGCAPA_LOCAL | RCX_SIGCAPA_GLOBAL,
                             RCX_SIGCAPA_NORM,
                             NULL,
                             0,
                             TRC_HALF
                           );
    if( ( type & RCXI_MAX ) == RCXI_MAX )
      c    = rcx_getsigcapa( lofig,
                             losig,
                             RCX_SIGCAPA_CTK,
                             RCX_SIGCAPA_LOCAL | RCX_SIGCAPA_GLOBAL,
                             RCX_SIGCAPA_NORM,
                             &refslope,
                             RCX_MAX,
                             TRC_HALF
                           );
    if( ( type & RCXI_MIN ) == RCXI_MIN )
      c    = rcx_getsigcapa( lofig,
                             losig,
                             RCX_SIGCAPA_CTK,
                             RCX_SIGCAPA_LOCAL | RCX_SIGCAPA_GLOBAL,
                             RCX_SIGCAPA_NORM,
                             &refslope,
                             RCX_MIN,
                             TRC_HALF
                           );

  }

  *capa = c;
  return 1 ;
}

/******************************************************************************\
rcxi_getnodecapa()
type = RCXI_MIN | RCXI_MAX
\******************************************************************************/
int rcxi_getnodecapa( lofig_list *lofig, 
                      losig_list *losig, 
                      lonode_list *lonode,
                      char level, 
                      float defslope,
                      int type,
                      float coef,
                      float *capa
                    )
{
  rcx_slope slope;
  char      ltype;

  switch( level ) {
  case RCXI_UP :
    slope.SENS = TRC_SLOPE_UP ;
    break ;
  case RCXI_DW :
    slope.SENS = TRC_SLOPE_DOWN ;
    break;
  default :
    sprintf( rcxi_error, 
             "rcxi_getnodecapa() : bad value for level parameter." 
           );
    return 0;
  }
  slope.F0MAX  = defslope ;
  slope.FCCMAX = defslope ;
  slope.CCA    = -1.0 ;

  switch( type ) {
  case RCXI_MAX :
    ltype = RCX_MAX ;
    break;
  case RCXI_MIN :
    ltype = RCX_MIN ;
    break;
  default :
    sprintf( rcxi_error, "rcxi_getnodecapa() : bad value for type parameter." );
    return 0;
  }

  *capa = rcx_getnodecapa( losig, lonode, 0.0, &slope, ltype, TRC_HALF, coef );
 
  lofig = NULL;

  return 1;
}

/******************************************************************************\
rcxi_getresi()
type = RCXI_ALLWIRE
\******************************************************************************/
int rcxi_getresi( lofig_list *lofig, losig_list *losig, int type, float *resi )
{
  if( type == RCXI_ALLWIRE ) {
    rcn_lock_signal( lofig, losig );
    *resi = rcx_getsumresi( losig );
    rcn_unlock_signal( lofig, losig );
    return 1;
  }

  sprintf( rcxi_error, "rcxi_getresi() : bad value for type parameter." );
  return 0;
}

/******************************************************************************\
rcxi_getpiload()
type = RCXI_MAX | RCXI_MIN
\******************************************************************************/
int rcxi_getpiload( lofig_list *lofig, 
                    losig_list *losig, 
                    int pin,
                    char level,
                    float defslope,
                    int type, 
                    float *c1, 
                    float *c2, 
                    float *r 
                  )
{
  rcx_slope slope ;
  RCXFLOAT rc1, rc2, rr;
  num_list *driver ;

  switch( level ) {
  case RCXI_UP :
    slope.SENS = TRC_SLOPE_UP ;
    break ;
  case RCXI_DW :
    slope.SENS = TRC_SLOPE_DOWN ;
    break;
  default :
    sprintf( rcxi_error, "rcxi_getpiload() : bad value for level parameter." );
    return 0;
  }
  slope.F0MAX  = defslope ;
  slope.FCCMAX = defslope ;
  slope.CCA    = -1.0 ;
  
  driver = addnum( NULL, pin );

  if( (type & RCXI_MAX ) == RCXI_MAX ) {
    rcx_rcnload( lofig,
                 losig,
                 driver,
                 &rr,
                 &rc1,
                 &rc2,
                 RCX_PILOAD,
                 0.0,
                 &slope,
                 RCX_MAX
               );
    *c1 = rc1 ;
    *c2 = rc2 ;
    *r  = rr ;
    freenum( driver );
    return 1;
  }

  if( (type & RCXI_MIN ) == RCXI_MIN ) {
    rcx_rcnload( lofig,
                 losig,
                 driver,
                 &rr,
                 &rc1,
                 &rc2,
                 RCX_PILOAD,
                 0.0,
                 &slope,
                 RCX_MIN
               );
    *c1 = rc1 ;
    *c2 = rc2 ;
    *r  = rr ;
    freenum( driver );
    return 1;
  }

  sprintf( rcxi_error, "rcxi_getpiload() : bad value for type parameter." );
  return 0 ;
}

/******************************************************************************\
rcxi_getfigname()
\******************************************************************************/
char* rcxi_getfigname( lofig_list *lofig )
{
  return rcx_getlofigname( lofig );
}

/******************************************************************************\
rcxi_getagrlist()
Renvoie la liste des agresseurs d'un signal et initialise des informations
permettant les calculs avec crosstalk.
\******************************************************************************/
int rcxi_getagrlist( lofig_list *lofig, losig_list *losig, chain_list **head )
{
  chain_list *chainfig;
  chain_list *r;

  chainfig = addchain( NULL, lofig );
  r = rcx_getagrlist( lofig, losig, lofig->NAME, chainfig );

  freechain( chainfig );
  *head = r;

  return 1;
}

/******************************************************************************\
rcxi_freeagrlist()
Libère toutes les infos précédement allouées par rcx_getagrlist()
\******************************************************************************/
int rcxi_freeagrlist( lofig_list *lofig, losig_list *losig, chain_list *head )
{
  rcx_freeagrlist( lofig, losig, head );
  return 1;
}

/******************************************************************************\
rcxi_getrcxparam()
\******************************************************************************/
rcxparam* rcxi_getrcxparam( lofig_list *lofig, losig_list *losig )
{
  ptype_list *ptl;

  ptl = getptype( losig->USER, RCX_REALAGRESSOR );
  if( !ptl ) return NULL;

  lofig = NULL;

  return (rcxparam*)ptl->DATA;
}

/******************************************************************************\
rcxi_getmiller()
Calcul l'effet Miller entre un signal victime et son agresseur. Il faut
avoir executé un rcxi_getagrlist() sur victime.
type = RCXI_MAX | RCXI_MIN | RCX_FORCED
\******************************************************************************/
int rcxi_getmiller( lofig_list *lofig, 
                    losig_list *victime, 
                    losig_list *agresseur, 
                    char level,
                    float defslope,
                    int type,
                    float *miller
                  )
{
  int       r, t, f ;
  rcxparam *param ;
  float     m=1.0 ;
  ptype_list *ptl;
 
  t = 0;

  if( ( type & RCXI_MIN ) == RCXI_MIN )
    t = RCXI_MIN ;
  if( ( type & RCXI_MAX ) == RCXI_MAX )
    t = RCXI_MAX ;
  if( !t ) {
    sprintf( rcxi_error, "bad value for type." );
    return 0;
  }

  if( ( type & RCXI_FORCED ) == RCXI_FORCED )
    f = 1 ;
  else
    f = 0;
  
  ptl = getptype( agresseur->USER, RCX_REALAGRESSOR );
  if( ptl ) {
    param = (rcxparam*)ptl->DATA;

    switch( level ) {
    
    case RCXI_UP:
      r=1;
      if( t == RCXI_MAX ) {
        if( (f == 1) || (param->ACTIF & RCX_AGRWORST) )
          m = rcx_getmiller( defslope, 
                             param->F0DW, 
                             RCX_OPPOSITE, 
                             RCX_MILLER2C 
                           );
      }
      else {
        if( (f == 1) || (param->ACTIF & RCX_AGRBEST) )
          m = rcx_getmiller( defslope, param->F0UP, RCX_SAME, RCX_MILLER2C );
      }
      break;
      
    case RCXI_DW:
      r=1;
      if( t == RCXI_MAX ) {
        if( (f == 1) || (param->ACTIF & RCX_AGRWORST) )
          m = rcx_getmiller( defslope, 
                             param->F0UP, 
                             RCX_OPPOSITE, 
                             RCX_MILLER2C 
                           );
      }
      else {
        if( (f == 1) || (param->ACTIF & RCX_AGRBEST) )
          m = rcx_getmiller( defslope, param->F0DW, RCX_SAME, RCX_MILLER2C );
      }
      break;
      
    default:
      sprintf( rcxi_error,"bad value for level." );
      r=0;
    }
  }

  *miller = m;
  lofig = NULL;
  victime = NULL;
  return r;
}

/******************************************************************************\
rcxi_getcoefctc()
Détermine le coefficient à appliquer sur les capacités à la masse pour prendre
en compte l'effet des capacités de couplage globales.
type = RCXI_MAX | RCXI_MIN
\******************************************************************************/
int rcxi_getcoefctc( lofig_list *lofig,
                     losig_list *losig,
                     char level,
                     float defslope,
                     char type,
                     float *coef
                   )
{
  rcx_slope slope;
  char      ltype;
  
  switch( level ) {
  case RCXI_UP :
    slope.SENS = TRC_SLOPE_UP ;
    break ;
  case RCXI_DW :
    slope.SENS = TRC_SLOPE_DOWN ;
    break;
  default :
    sprintf( rcxi_error, "rcxi_getcoefctc() : bad value for level parameter." );
    return 0;
  }
  slope.F0MAX  = defslope ;
  slope.FCCMAX = defslope ;
  slope.CCA    = -1.0 ;

  switch( type ) {
  case RCXI_MAX :
    ltype = RCX_MAX ;
    break;
  case RCXI_MIN :
    ltype = RCX_MIN ;
    break;
  default :
    sprintf( rcxi_error, "rcxi_getcoefctc() : bad value for type parameter." );
    return 0;
  }
  
  *coef = rcx_getcoefctc( lofig,
                          losig,
                          &slope,
                          type,
                          0.0,
                          NULL
                        );
                         
  return 1;
}
