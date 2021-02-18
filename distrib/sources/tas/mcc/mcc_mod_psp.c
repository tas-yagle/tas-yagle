/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_psp.c                                                 */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2003 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Gregoire AVOT                                               */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include <alloca.h>
#include "sk.h"

#include MCC_H
#include "mcc_util.h"
#include "mcc_mod_util.h"
#include "mcc_mod_spice.h"
#include "mcc_mod_psp.h"
#include "mcc_mod_psp_interface.h"

#define EPSILON 0.001

void mcc_initparam_psp( mcc_modellist *ptmodel )
{
  static int  f = 0 ;
  ptype_list *ptl ;

  if( !f )
    if( !psp_loaddynamiclibrary() )
      exit(1);

  ptl = getptype( ptmodel->USER, PSPCACHECHARGE );
  if( ptl ) {
    printf( "warning : previous cache charge not empty !!!\n " );
    mcc_clean_psp( ptmodel );
  }
  
  mcc_clean_psp_interface( ptmodel, 1 );

  f = 1 ;
  ptmodel = NULL ;
}

void mcc_clean_psp( mcc_modellist *mccmodel )
{
  ptype_list     *ptl ;
  cachepspcharge *cache, *next ;

  ptl = getptype( mccmodel->USER, PSPCACHECHARGE );
  if ( ptl ) {
    for( cache = (cachepspcharge*)ptl->DATA ; cache ; cache = next ) {
      next = cache->next ;
      free( cache );
    }

    mccmodel->USER = delptype( mccmodel->USER, PSPCACHECHARGE );
  }

  mcc_clean_psp_interface( mccmodel, 0 );
}

void mcc_addtuneeffect( mcc_modellist *mccmodel, 
                        psptunedparam *tparam, 
                        int flag, 
                        pspjuncapconfig *juncapconfig
                      )
{
  if( ( flag & MCC_PSP_TUNE_NO_JUNCTION ) == MCC_PSP_TUNE_NO_JUNCTION ) {
    /* desactivate junction capacitance calculation 
       see doc psp 102.0 issued 06/2006 : p9 #6            */
    tparam->param[ tparam->n   ] = "SWJUNCAP" ; 
    tparam->value[ tparam->n++ ] = 0.0 ;
  }
  else {
    if( juncapconfig ) {
      tparam->param[ tparam->n   ] = "SWJUNCAP" ; 
      tparam->value[ tparam->n++ ] = 1.0 ;

      tparam->param[ tparam->n   ] = "ABDRAIN" ; 
      tparam->value[ tparam->n++ ] = juncapconfig->ab ;
      tparam->param[ tparam->n   ] = "LSDRAIN" ; 
      tparam->value[ tparam->n++ ] = juncapconfig->ls ;
      tparam->param[ tparam->n   ] = "LGDRAIN" ; 
      tparam->value[ tparam->n++ ] = juncapconfig->lg ;

      tparam->param[ tparam->n   ] = "ABSOURCE" ; 
      tparam->value[ tparam->n++ ] = juncapconfig->ab ;
      tparam->param[ tparam->n   ] = "LSSOURCE" ; 
      tparam->value[ tparam->n++ ] = juncapconfig->ls ;
      tparam->param[ tparam->n   ] = "LGSOURCE" ; 
      tparam->value[ tparam->n++ ] = juncapconfig->lg ;
    }
  }
  
  if( ( flag & MCC_PSP_TUNE_NO_EXTRINSIC ) == MCC_PSP_TUNE_NO_EXTRINSIC ) {

    if( mccmodel->MODELTYPE == MCC_MPSP ) {

      /* set to 0 extrinsic overlap charge Qsov and Qdov by setting CGOV value to 0 
         see doc psp 102.0 issued 06/2006 : p11 #53, p39 #3.75, p67 #4.187,4.188 */
      tparam->param[ tparam->n   ] = "LOV" ;       
      tparam->value[ tparam->n++ ] = 0.0 ;
      
      /* set to 0 extrinsic outer fringe charge Qofs and Qofd by setting CFR to 0
         see doc psp 102.0 issued 06/2006 : p14 #143, p39 #3.77, p67 #4.190,#4.191 */
      tparam->param[ tparam->n   ] = "CFRW" ;
      tparam->value[ tparam->n++ ] = 0.0 ;

      /* set to 0 extrinsic bulk overlap charge Qbov by setting CGBOV to 0
         see doc psp 102.0 issued 06/2006 : p14 #142, p39 #3.76, p67 #4.189 */
      tparam->param[ tparam->n   ] = "CGBOVL" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
    }
    else { /* level==1021 */
      tparam->param[ tparam->n   ] = "POCGOV" ;    /* p45 #3.150 */
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PLCGOV" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PWCGOV" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PLWCGOV" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
    
      tparam->param[ tparam->n   ] = "POCFR" ;    /* p46 #3.152 */
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PLCFR" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PWCFR" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PLWCFR" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
     
      tparam->param[ tparam->n   ] = "POCGBOV" ;    /* p46 #3.151 */
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PLCGBOV" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PWCGBOV" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
      tparam->param[ tparam->n   ] = "PLWCGBOV" ;
      tparam->value[ tparam->n++ ] = 0.0 ;
    }
  }
}

void extractchargecomponent( pspbrcharge *qin, pspbrcharge *qout, int nin, int nout, char *label )
{
  char           tabpin[]={'D','G','S','B'};
  int            i, j ;

  for( j=0 ; j<nout ; j++ ) 
    qout[j].charge = 0.0 ;

  for( i=0 ; i<nin ; i++ ) {

    if( qin[i].charge >= MCC_Q || qin[i].charge <= -MCC_Q ) {

      for( j=0 ; j<nout ; j++ ) {

        if( qin[i].nplus == qout[j].nplus && qin[i].nminus == qout[j].nminus ) {
          qout[j].charge = qout[j].charge + qin[i].charge ;
          break ;
        }

        if( qin[i].nplus == qout[j].nminus && qin[i].nminus == qout[j].nplus ) {
          qout[j].charge = qout[j].charge - qin[i].charge ;
          break ;
        }
      }

      if( j >= nout ) {
        printf( "%s between nodes %c and %c (charge=%gC) is not handled by mcc\n",
                label,
                tabpin[qin[i].nplus],
                tabpin[qin[i].nminus],
                qin[i].charge
              );
      }
    }
  }
}

void copycharge( pspcharge *chargeorg, pspcharge *chargedest )
{
    chargedest->qg    = chargeorg->qg ;
    chargedest->qb    = chargeorg->qb ;
    chargedest->qd    = chargeorg->qd ;
    chargedest->qs    = chargeorg->qs ;
    chargedest->qgsov = chargeorg->qgsov ;
    chargedest->qgdov = chargeorg->qgdov ;
    chargedest->qgb   = chargeorg->qgb ;
    chargedest->qjbd  = chargeorg->qjbd ;
    chargedest->qjbs  = chargeorg->qjbs ;
}

char* cachechargegenkey( double           vgs,
                         double           vds,
                         double           vbs,
                         pspjuncapconfig *juncapconfig 
                       )
{
  char buf[1024];
  char *pt ;

  if( juncapconfig )
    sprintf( buf, "%.2f %.2f %.2f %.2e %.2e %.2e", vgs, 
                                                   vds,
                                                   vbs, 
                                                   juncapconfig->ab,
                                                   juncapconfig->ls,
                                                   juncapconfig->lg
           );
  else
    sprintf( buf, "%.2f %.2f %.2f", vgs,
                                    vds,
                                    vbs
           );

  pt = namealloc( buf );

  return pt ;
}

pspcharge* findchargeincache( mcc_modellist   *mccmodel,
                              double           vgs,
                              double           vds,
                              double           vbs,
                              pspjuncapconfig *juncapconfig
                            )
{
  char            *key ;
  ptype_list      *ptl ;
  cachepspcharge *cache ;
  
  ptl = getptype( mccmodel->USER, PSPCACHECHARGE );
  if( !ptl )
    return NULL ;
    
  key = cachechargegenkey( vgs, vds, vbs, juncapconfig );
  for( cache = (cachepspcharge*)ptl->DATA ; cache ; cache = cache->next ) {
    if( cache->key == key )
      break ;
  }

  if( cache )
    return &(cache->charge) ;

  return NULL ;
}

void addchargeincache( mcc_modellist   *mccmodel,
                       pspcharge       *charge,
                       double           vgs,
                       double           vds,
                       double           vbs,
                       pspjuncapconfig *juncapconfig
                     )
{
  ptype_list      *ptl ;
  cachepspcharge *cache ;
  
  ptl = getptype( mccmodel->USER, PSPCACHECHARGE );
  if( !ptl ) {
    mccmodel->USER = addptype( mccmodel->USER, PSPCACHECHARGE, NULL );
    ptl = mccmodel->USER ;
  }

  cache = mbkalloc( sizeof( cachepspcharge ) );
  cache->next = ptl->DATA ;
  ptl->DATA   = cache ;
  cache->key  = cachechargegenkey( vgs, vds, vbs, juncapconfig );
  
  copycharge( charge, &(cache->charge) );
    
}

void mcc_getcharge( mcc_modellist   *mccmodel,
                    double           L,
                    double           W,
                    double           temp,
                    double           vgs,
                    double           vds,
                    double           vbs,
                    elp_lotrs_param *lotrsparam,
                    pspjuncapconfig *juncapconfig,
                    pspcharge       *charge 
                  )
{
  trs            model ;
  psptunedparam  tparam ;
  int            base ;
  pspbrcharge   *qq[3] ;
  int            calc[3] ;
  int            n ;
  int            p ;
  int            nbbr ;
  float         *qint[4] ;
  pspbrcharge    qout[3] ;
  pspcharge     *cache ;

  if( V_BOOL_TAB[__AVT_USE_CACHE_PSP].VALUE )
    cache = findchargeincache( mccmodel, vgs, vds, vbs, juncapconfig );
  else
    cache = NULL ;

  if( cache )
    copycharge( cache, charge );
  else {
    /* we suppose the simkit interface use the following node order :
       0 : D
       1 : G
       2 : S
       3 : B
    */

    /* compute the 3 charge components :
       1 : intrinsic
       2 : extrinsic (gs and gd overlap)
       3 : junction
       since psp doesn't provide individual access to theses parameter,
       3 models evaluation are done. 
       For each evaluation some component are removed by hacking the model.
       Charge branches results are store in the 3 qq branch table.
       We assume branches set returned by psp are exactly the same for
       these 3 evaluation.
    */
    
    tparam.param = (char**)alloca( sizeof( char* ) * 16 );
    tparam.value = (sk_real*)alloca( sizeof( sk_real ) * 16 );

    
    calc[0] = MCC_PSP_TUNE_NO_JUNCTION | MCC_PSP_TUNE_NO_EXTRINSIC ;
    calc[1] = MCC_PSP_TUNE_NO_JUNCTION ;
    calc[2] = MCC_PSP_TUNE_NO_EXTRINSIC ;

    for( n=0 ; n<3 ; n++ ) {

      tparam.n = 0 ;
      mcc_addtuneeffect( mccmodel, &tparam, calc[n], juncapconfig );
      
      initialize( &model, mccmodel, lotrsparam, L, W, temp, &tparam );
      if( model.model->n_terminals != 4 ) {
        printf( "number of external nodes differs than 4. can't extract charge values\n" );
        return ;
      }
      
      set_polarization( &model, vgs, vds, vbs );

      nbbr = (int)model.model->n_dynamic_branches ;
      qq[n] = (pspbrcharge*)alloca( sizeof(pspcharge)*nbbr );
      base = model.model->n_static_branches + model.model->n_static_linear_branches ;
      for( p=0 ; p< nbbr ; p++ ) {
        qq[n][p].nplus  = (int)model.variables->p_output_ev[p+base].p_ev_descrip->p_pos_node->number ;
        qq[n][p].nminus = (int)model.variables->p_output_ev[p+base].p_ev_descrip->p_neg_node->number ;
        qq[n][p].charge = (float)model.variables->p_output_ev[p+base].value ;
      }
      terminate( &model );
    }

    /* deduce the charge component for extrinsic and junction */
    for( n=1 ; n<3 ; n++ ) {
    
      for( p=0 ; p<nbbr ; p++ ) {

        if( qq[n][p].nplus != qq[0][p].nplus )
          printf( "branche order has changed !" );
          
        if( qq[n][p].nminus != qq[0][p].nminus )
          printf( "branche order has changed !" );
          
        qq[n][p].charge = qq[n][p].charge - qq[0][p].charge ;
      }
    }

    /* fill intrinsic component */
    qint[0] = & charge->qd ;
    qint[1] = & charge->qg ;
    qint[2] = & charge->qs ;
    qint[3] = & charge->qb ;
    for( n=0 ; n<4 ; n++ ) 
      *(qint[n])=0.0 ;

    for( n=0 ; n < nbbr ; n++ ) {
      *(qint[ qq[0][n].nplus  ]) = *(qint[ qq[0][n].nplus  ]) + qq[0][n].charge ;
      *(qint[ qq[0][n].nminus ]) = *(qint[ qq[0][n].nminus ]) - qq[0][n].charge ;
    }

    /* fill extrinsic component */

    /* qgsov */
    qout[0].nplus  = 1 ; // G
    qout[0].nminus = 2 ; // S
    /* qgdov */
    qout[1].nplus  = 1 ; // G
    qout[1].nminus = 0 ; // D
    /* qgdov */
    qout[2].nplus  = 1 ; // G
    qout[2].nminus = 3 ; // B
    
    extractchargecomponent( qq[1], qout, nbbr, 3, "extrinsic charge" );

    charge->qgsov = qout[0].charge ;
    charge->qgdov = qout[1].charge ;
    charge->qgb   = qout[2].charge ;

    /* fill junction component */

    /* qjbd */
    qout[0].nplus  = 3 ; // B
    qout[0].nminus = 0 ; // D
    /* qjbs */
    qout[1].nplus  = 3 ; // B
    qout[1].nminus = 2 ; // S

    extractchargecomponent( qq[2], qout, nbbr, 2, "junction charge" );
    
    charge->qjbd = qout[0].charge ;
    charge->qjbs = qout[1].charge ;

    if( V_BOOL_TAB[__AVT_USE_CACHE_PSP].VALUE )
      addchargeincache( mccmodel, charge, vgs, vds, vbs, juncapconfig );
  }
}

void mcc_calcQint_psp( mcc_modellist *ptmodel, 
                       double L, 
                       double W,
                       double temp, 
                       double vgs, 
                       double vbs, 
                       double vds,
                       double *ptQg, 
                       double *ptQs, 
                       double *ptQd,
                       double *ptQb,
                       elp_lotrs_param *lotrsparam
                     )
{
  pspcharge charge ;
  double    s ;

  if( ptQg ) *ptQg = 0.0 ;
  if( ptQs ) *ptQs = 0.0 ;
  if( ptQd ) *ptQd = 0.0 ;
  if( ptQb ) *ptQb = 0.0 ;

  mcc_getcharge( ptmodel, L, W, temp, vgs, vds, vbs, lotrsparam, NULL, &charge );

  s = W*L ;

  if( ptQd ) *ptQd = charge.qd / s ;;
  if( ptQg ) *ptQg = charge.qg / s ;;
  if( ptQs ) *ptQs = charge.qs / s ;;
  if( ptQb ) *ptQb = charge.qb / s ;;
}

double mcc_calcCGP_psp( mcc_modellist   *ptmodel,
                        elp_lotrs_param *lotrsparam, 
                        double           vgx, 
                        double           L, 
                        double           W, 
                        double           temp,
                        double          *ptQov 
                      )
{
  pspcharge charge ;
  double    cgp ;
  
  mcc_getcharge( ptmodel, L, W, temp, vgx, 0.0, 0.0, lotrsparam, NULL, &charge );

  if( vgx > EPSILON || vgx < -EPSILON )
    cgp = fabs(charge.qgdov/vgx) ;
  else
    cgp = 0.0 ;
  
  if( ptQov )
    *ptQov = fabs(charge.qgdov/W) ;

  return cgp/W ;
}

double mcc_calcCGD_psp( mcc_modellist *ptmodel, 
                        double L, 
                        double W, 
                        double temp, 
                        double vgs0, 
                        double vgs1, 
                        double vbs, 
                        double vds,
                        elp_lotrs_param *lotrsparam
                      )
{
  pspcharge charge0 ;
  pspcharge charge1 ;
  double    cgd ;
  double    s ;

  mcc_getcharge( ptmodel, L, W, temp, vgs0, vds, vbs, lotrsparam, NULL, &charge0 );
  mcc_getcharge( ptmodel, L, W, temp, vgs1, vds, vbs, lotrsparam, NULL, &charge1 );

  s = W*L ;
  cgd = fabs( ( charge1.qd - charge0.qd ) / ( vgs1 - vgs0 ) )/s ;
  
  return cgd ;
}

double mcc_calcCGSI_psp( mcc_modellist *ptmodel, 
                         double L, 
                         double W, 
                         double temp, 
                         double vgs, 
                         double vbs, 
                         double vds,
                         elp_lotrs_param *lotrsparam
                       )
{
  pspcharge charge0 ;
  pspcharge charge1 ;
  double    cgs ;
  double    s ;

  mcc_getcharge( ptmodel, L, W, temp, 0.0, vds, vbs, lotrsparam, NULL, &charge0 );
  mcc_getcharge( ptmodel, L, W, temp, vgs, vds, vbs, lotrsparam, NULL, &charge1 );

  s = W*L ;
  cgs = fabs( ( charge1.qs - charge0.qs ) / vgs )/s ;

  return cgs ;
}

double mcc_calcVTH_psp( mcc_modellist   *mccmodel,
                        double           L,
                        double           W,
                        double           temp,
                        double           vbs,
                        double           vdd,
                        elp_lotrs_param *lotrsparam 
                      )
{
  trs      model ;
  double   vth ;
  sk_unint id_vth ;
 
  initialize( &model, mccmodel, lotrsparam, L, W, temp, NULL );
  id_vth = get_id_param( &model, PSP_OP_PARAM_VTH );

  set_polarization( &model, vdd, vdd, vbs );
  vth = model.variables->p_opo_values[ id_vth ] ;

  terminate( &model );

  if( mccmodel->TYPE == MCC_TRANS_P )
    vth = -vth ;
    
  return vth ;
}

double mcc_calcIDS_psp( mcc_modellist *mccmodel, 
                        double vbs,
                        double vgs,
                        double vds, 
                        double W,
                        double L, 
                        double temp,
                        elp_lotrs_param *lotrsparam
                      )
{
  trs      model ;
  double   ids ;
  sk_unint id_ids ;
 
  initialize( &model, mccmodel, lotrsparam, L, W, temp, NULL );
  id_ids = get_id_param( &model, PSP_OP_PARAM_IDS );
  set_polarization( &model, vgs, vds, vbs );
  
  ids = model.variables->p_opo_values[ id_ids ] ;

  terminate( &model );
  return fabs(ids) ;
}

double mcc_calcDWCJ_psp( mcc_modellist *mccmodel, 
                         elp_lotrs_param *lotrsparam, 
                         double temp,
                         double L, 
                         double W
                       )
{
  trs           model ;
  double        dwcj ;
  int           id_weff ;
  double        weff ;

  initialize( &model, mccmodel, lotrsparam, L, W, temp, NULL );
  id_weff = get_id_param( &model, PSP_OP_PARAM_WEFF );
  set_polarization( &model, 0.0, 0.0, 0.0 );
  
  weff = model.variables->p_opo_values[ id_weff ] ;
  dwcj = weff - W ;

  terminate( &model );
  return dwcj ;
}

double mcc_calcCDS_psp( mcc_modellist   *ptmodel, 
                        elp_lotrs_param *lotrsparam,
                        double           temp, 
                        double           vbx0, 
                        double           vbx1,
                        double           L,
                        double           W
                      )
{
  pspjuncapconfig  juncapconfig ;
  double           cds ;
  pspcharge        charge0 ;
  pspcharge        charge1 ;

  juncapconfig.ab = W*W ;
  juncapconfig.ls = 0.0 ;
  juncapconfig.lg = 0.0 ;

  mcc_getcharge( ptmodel, L, W, temp, 0.0, vbx0, 0.0, lotrsparam, &juncapconfig, &charge0 );
  mcc_getcharge( ptmodel, L, W, temp, 0.0, vbx1, 0.0, lotrsparam, &juncapconfig, &charge1 );
  
  cds = fabs( (charge1.qjbd-charge0.qjbd)/(vbx1-vbx0) )/juncapconfig.ab ;

  return cds ;
}

double mcc_calcCDP_psp( mcc_modellist *ptmodel, 
                        elp_lotrs_param *lotrsparam,
                        double temp, 
                        double vbx0, 
                        double vbx1,
                        double L,
                        double W
                      )
{
  pspjuncapconfig  juncapconfig ;
  double           cdp ;
  pspcharge        charge0 ;
  pspcharge        charge1 ;

  juncapconfig.ab = 0.0 ;
  juncapconfig.ls = W ;
  juncapconfig.lg = 0.0 ;

  mcc_getcharge( ptmodel, L, W, temp, 0.0, vbx0, 0.0, lotrsparam, &juncapconfig, &charge0 );
  mcc_getcharge( ptmodel, L, W, temp, 0.0, vbx1, 0.0, lotrsparam, &juncapconfig, &charge1 );
  
  cdp = fabs( (charge1.qjbd-charge0.qjbd)/(vbx1-vbx0) )/juncapconfig.ls ;

  return cdp ;
}

double mcc_calcCDW_psp( mcc_modellist *ptmodel, 
                        elp_lotrs_param *lotrsparam,
                        double temp, 
                        double vbx0, 
                        double vbx1, 
                        double L, 
                        double W 
                      )
{
  pspcharge        charge0 ;
  pspcharge        charge1 ;
  double           cdw ;
  pspjuncapconfig  juncapconfig ;
  
  juncapconfig.ab = 0.0 ;
  juncapconfig.ls = 0.0 ;
  juncapconfig.lg = W ;

  mcc_getcharge( ptmodel, L, W, temp, 0.0, vbx0, 0.0, lotrsparam, &juncapconfig, &charge0 );
  mcc_getcharge( ptmodel, L, W, temp, 0.0, vbx1, 0.0, lotrsparam, &juncapconfig, &charge1 );
    
  cdw = fabs( (charge1.qjbd-charge0.qjbd)/(vbx1-vbx0) )/juncapconfig.lg ;

  return cdw ;
}
