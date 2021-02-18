/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_tpiv.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/
/* calcul des temps de propagation entre une sortie de cone                 */
/* et toutes ses entrees                                                    */
/****************************************************************************/

#include "tas.h"

int TPIV_DEBUG_IBR = 0 ;

mcc_trans_mcc* tpiv_addtransmcc( tpiv *br, stm_solver_maillon *stmm, link_list *link )
{
  mcc_trans_mcc           *trsmcc ;
  chain_list              *head ;
  float                    vg ;
  double                   vbulk ;

  vg = tpiv_get_voltage_driving_lotrs( link->ULINK.LOTRS );

  if( TAS_CONTEXT->TAS_LEVEL == 2 )
    elp_lotrs_param_get( link->ULINK.LOTRS,NULL,NULL, NULL,  NULL,NULL, NULL, NULL, NULL,NULL,&vbulk,NULL,NULL,NULL,NULL );
  else {
    if( MLO_IS_TRANSP(link->ULINK.LOTRS->TYPE) )
      vbulk = tas_getparam( link->ULINK.LOTRS, TAS_CASE, TP_VDDmax );
    else
      vbulk = 0.0 ;
  }

  trsmcc = mcc_create_trans_mcc( link->ULINK.LOTRS, 
                                 tas_getparam ( link->ULINK.LOTRS, TAS_CASE, TP_VDDmax),
                                 TAS_CASE,
                                 link->SIG,
                                 vg,
                                 vbulk
                               );

  if( getptype( link->USER, TAS_LINK_CARAC ) ) {
    trsmcc->TRWIDTH  = TAS_GETWIDTH( link ) ;
    trsmcc->TRLENGTH = TAS_GETLENGTH( link );
  }
                                   
  br->HEADTRSMCC = addchain( br->HEADTRSMCC, trsmcc );
  
  head = NULL ;
  head = addchain( head, trsmcc );
  stm_solver_add_model( stmm, 
                        (char(*)(void*,float,float,float*))mcc_mcc_ids_list,
                        (char(*)(void*,float,float,float*))mcc_mcc_vds_list,
                        head,
                        head
                      );
  freechain( head );
  return trsmcc ;
}

void tpiv_addtransmcc_switch( tpiv *br, stm_solver_maillon *stmm, link_list *link, lotrs_list *lotrs )
{
  float          rate = 1.0 ;
  mcc_trans_mcc *trs1 ;
  mcc_trans_mcc *trs2 ;
  chain_list    *head ;
  float          vg ;
  double         vbulk ;

  if( !V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ) {
  
    trs1 = tpiv_addtransmcc( br, stmm, link );

    rate = tas_get_current_rate( link ) ;
    trs1->TRWIDTH  = trs1->TRWIDTH * rate ;

  }
  else {

    vg = tpiv_get_voltage_driving_lotrs( link->ULINK.LOTRS );

    if( TAS_CONTEXT->TAS_LEVEL == 2 )
      elp_lotrs_param_get( link->ULINK.LOTRS, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &vbulk,NULL,NULL,NULL,NULL );
    else {
      if( MLO_IS_TRANSP(link->ULINK.LOTRS->TYPE) )
        vbulk = tas_getparam( link->ULINK.LOTRS, TAS_CASE, TP_VDDmax );
      else
        vbulk = 0.0 ;
    }

    trs1 = mcc_create_trans_mcc( link->ULINK.LOTRS, 
                                 tas_getparam ( link->ULINK.LOTRS, TAS_CASE, TP_VDDmax),
                                 TAS_CASE,
                                 link->SIG,
                                 vg,
                                 vbulk
                               );

    if( getptype( link->USER, TAS_LINK_CARAC ) ) {
      trs1->TRWIDTH  = TAS_GETWIDTH( link ) ;
      trs1->TRLENGTH = TAS_GETLENGTH( link );
    }

    vg = tpiv_get_voltage_driving_lotrs( lotrs );

    if( TAS_CONTEXT->TAS_LEVEL == 2 )
      elp_lotrs_param_get( lotrs, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &vbulk,NULL,NULL,NULL,NULL );
    else {
      if( MLO_IS_TRANSP(lotrs->TYPE) )
        vbulk = tas_getparam( lotrs, TAS_CASE, TP_VDDmax );
      else
        vbulk = 0.0 ;
    }

    trs2 = mcc_create_trans_mcc( lotrs, 
                                 tas_getparam ( lotrs, TAS_CASE, TP_VDDmax),
                                 TAS_CASE,
                                 link->SIG,
                                 vg,
                                 vbulk
                               );
    /* the TAS_LINK_CARAC needs to update transistor dimension is not available
       here... to be done ! */

    br->HEADTRSMCC = addchain( br->HEADTRSMCC, trs1 );
    br->HEADTRSMCC = addchain( br->HEADTRSMCC, trs2 );
    
    head = NULL ;
    head = addchain( head, trs1 );
    head = addchain( head, trs2 );
    stm_solver_add_model( stmm, 
                          (char(*)(void*,float,float,float*))mcc_mcc_ids_list,
                          (char(*)(void*,float,float,float*))mcc_mcc_vds_list,
                          head,
                          head
                        );
    freechain( head );
  }
}

/******************************************************************************\
Convertie une branche au format CNS vers le format stm_solver()
Les tensions sur les grilles sont initialisées à vdd.
\******************************************************************************/
tpiv* tpiv_createbranch( link_list *head, char transtype )
{
  link_list               *cnsm;
  stm_solver_maillon      *stmm;
  mcc_trans_spice         *trsspice;
  mcc_trans_mcc           *trsmcc;
  tpiv                    *br;
  long                     w, l;
  ptype_list              *ptype;
  lotrs_list              *trspair;
  float                    rate;
  chain_list              *trlist;
  float                    vg ;
  double                   vbulk ;

  br = tpiv_alloc();
  
  /* Transforme la branche CNS en branche STM */
  for( cnsm = head ; cnsm ; cnsm = cnsm->NEXT ) {
    if ((cnsm->TYPE & CNS_EXT) == CNS_EXT) break;
    stmm = stm_solver_new_maillon();
    
    switch( transtype ) {
    
    case TAS_TRMODEL_MCCRSAT :

      trspair = NULL ;
      if( (cnsm->TYPE & CNS_SWITCH)==CNS_SWITCH ) {
        ptype = getptype( cnsm->ULINK.LOTRS->USER, TAS_TRANS_SWITCH );
        if( ptype ) 
          trspair = (lotrs_list*)ptype->DATA ;
      }

      if( trspair )
        tpiv_addtransmcc_switch( br, stmm, cnsm, trspair );
      else
        trsmcc = tpiv_addtransmcc( br, stmm, cnsm );

      break;
      
    case TAS_TRMODEL_SPICE :
    
      vg = tpiv_get_voltage_driving_lotrs( cnsm->ULINK.LOTRS );

      /*
      if( TAS_CONTEXT->TAS_LEVEL == 2 )
      */
        elp_lotrs_param_get( cnsm->ULINK.LOTRS, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &vbulk,NULL,NULL,NULL,NULL );
      /*
      else {
        if( MLO_IS_TRANSP(cnsm->ULINK.LOTRS->TYPE) )
          vbulk = tas_getparam( cnsm->ULINK.LOTRS, TAS_CASE, TP_VDDmax );
        else
          vbulk = 0.0 ;
      }
      */
        
      trsspice = mcc_create_trans_spice( cnsm->ULINK.LOTRS, 
                                         tas_getparam ( cnsm->ULINK.LOTRS, 
                                                        TAS_CASE, 
                                                        TP_VDDmax),
                                         cnsm->SIG,
                                         vg,
                                         vbulk
                                       );
      br->HEADTRSSPICE = addchain( br->HEADTRSSPICE, trsspice );
      elpLotrsGetUnShrinkDim( cnsm->ULINK.LOTRS, 
                              TAS_GETLENGTH( cnsm ), 
                              TAS_GETWIDTH( cnsm ), 
                              &l, 
                              &w,
                              TAS_CASE
                            );
      rate = tas_get_current_rate( cnsm );
      w = w * rate ;
      trsspice->TRLENGTH = ((float)l)*1e-6 / ((float)SCALE_X) ;
      trsspice->TRWIDTH = ((float)w)*1e-6 / ((float)SCALE_X) ;
      
      // Voir le deshrinkage                                  
      // trsspice->WIDTH  = TAS_GETWIDTH( cnsm );
      // trsspice->LENGTH = TAS_GETLENGTH( cnsm );
      // Voir l'influence de WIDTH sur les aires.

      trlist = addchain( NULL, trsspice );
      stm_solver_add_model( stmm,
                         (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                         (char(*)(void*,float,float,float*))mcc_spice_vds_list,
                            trlist,
                            trlist
                          );
      freechain( trlist );
      break;
   
    default :
      fprintf( stderr, "Bad transistor model.\n" );
      EXIT(1);
    }

    br->HEAD = stm_solver_maillon_addchain( br->HEAD, stmm );
  }

  br->HEAD = stm_solver_maillon_reverse( br->HEAD );

  return br;
}

/******************************************************************************\
Crée une branche pour le calcul du leakage dont le transistor activelink est bloque
\******************************************************************************/
tpiv* tpiv_createbranch_leakage( link_list *head, link_list *activelink, char transtype )
{
  link_list               *cnsm;
  stm_solver_maillon      *stmm;
  mcc_trans_spice         *trsspice;
  mcc_trans_mcc           *trsmcc;
  tpiv                    *br;
  long                     w, l;
  ptype_list              *ptype;
  lotrs_list              *trspair;
  float                    rate;
  chain_list              *trlist;
  float                    vg ;
  double                   vbulk ;

  br = tpiv_alloc();
  
  /* Transforme la branche CNS en branche STM */
  for( cnsm = head ; cnsm ; cnsm = cnsm->NEXT ) {
    if ((cnsm->TYPE & CNS_EXT) == CNS_EXT) break;
    stmm = stm_solver_new_maillon();

    if((cnsm == activelink) || (TAS_PATH_TYPE == 'm')){
      vg = tpiv_get_blocked_voltage_driving_lotrs( cnsm->ULINK.LOTRS );
    }else{
      vg = tpiv_get_voltage_driving_lotrs( cnsm->ULINK.LOTRS );
    }

    elp_lotrs_param_get( cnsm->ULINK.LOTRS, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &vbulk,NULL,NULL,NULL,NULL );
      
    trsspice = mcc_create_trans_spice( cnsm->ULINK.LOTRS, 
                                       tas_getparam ( cnsm->ULINK.LOTRS, 
                                                      TAS_CASE, 
                                                      TP_VDDmax),
                                       cnsm->SIG,
                                       vg,
                                       vbulk
                                     );
    br->HEADTRSSPICE = addchain( br->HEADTRSSPICE, trsspice );
    elpLotrsGetUnShrinkDim( cnsm->ULINK.LOTRS, 
                            TAS_GETLENGTH( cnsm ), 
                            TAS_GETWIDTH( cnsm ), 
                            &l, 
                            &w,
                            TAS_CASE
                          );
    rate = tas_get_current_rate( cnsm );
    w = w * rate ;
    trsspice->TRLENGTH = ((float)l)*1e-6 / ((float)SCALE_X) ;
    trsspice->TRWIDTH = ((float)w)*1e-6 / ((float)SCALE_X) ;
    
    trlist = addchain( NULL, trsspice );
    stm_solver_add_model( stmm,
                       (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                       (char(*)(void*,float,float,float*))mcc_spice_vds_list,
                          trlist,
                          trlist
                        );
    freechain( trlist );
   

    br->HEAD = stm_solver_maillon_addchain( br->HEAD, stmm );
  }

  br->HEAD = stm_solver_maillon_reverse( br->HEAD );

  return br;
}

tpiv* tpiv_createbranch_leakage_2( link_list *head, link_list *activelink, char transtype )
{
  link_list               *cnsm;
  stm_solver_maillon      *stmm;
  mcc_trans_spice         *trsspice;
  mcc_trans_mcc           *trsmcc;
  tpiv                    *br;
  long                     w, l;
  ptype_list              *ptype;
  lotrs_list              *trspair;
  float                    rate;
  chain_list              *trlist;
  float                    vg ;
  double                   vbulk ;
  chain_list              *chain, *ptparachain, *ptchain;
  lotrs_list *ptparatrans, *ptlotrs;
  ptype_list *ptuser;
  cone_list  *cone;

  br = tpiv_alloc();
  
  /* Transforme la branche CNS en branche STM */
  for( cnsm = head ; cnsm ; cnsm = cnsm->NEXT ) {
    if ((cnsm->TYPE & CNS_EXT) == CNS_EXT) break;
    stmm = stm_solver_new_maillon();
    
    cone = (cone_list *)getptype( cnsm->ULINK.LOTRS->USER, CNS_DRIVINGCONE )->DATA;
    if((((cone->TECTYPE & (CNS_STATE_ONE|CNS_ONE)) != 0) && MLO_IS_TRANSN(cnsm->ULINK.LOTRS->TYPE))
    || (((cone->TECTYPE & (CNS_STATE_ZERO|CNS_ZERO)) != 0) && MLO_IS_TRANSP(cnsm->ULINK.LOTRS->TYPE))){
        vg = tpiv_get_voltage_driving_lotrs( cnsm->ULINK.LOTRS );
    }else{
        vg = tpiv_get_blocked_voltage_driving_lotrs( cnsm->ULINK.LOTRS );
    }
    
    elp_lotrs_param_get( cnsm->ULINK.LOTRS, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &vbulk,NULL,NULL,NULL,NULL );
      
    trsspice = mcc_create_trans_spice( cnsm->ULINK.LOTRS, 
                                       tas_getparam ( cnsm->ULINK.LOTRS, 
                                                      TAS_CASE, 
                                                      TP_VDDmax),
                                       cnsm->SIG,
                                       vg,
                                       vbulk
                                     );
    br->HEADTRSSPICE = addchain( br->HEADTRSSPICE, trsspice );
    ptlotrs = cnsm->ULINK.LOTRS;
    if ((ptuser = getptype(ptlotrs->USER, MBK_TRANS_PARALLEL)) != NULL) {
        ptparachain = (chain_list *)ptuser->DATA;
        l = ptlotrs->LENGTH;
        w = 0;
        for (ptchain = ptparachain; ptchain; ptchain = ptchain->NEXT) {
            ptparatrans = (lotrs_list *)ptchain->DATA;
            w += ptparatrans->WIDTH * (l / ptparatrans->LENGTH);
        }
    }else{
        l = ptlotrs->LENGTH;
        w = ptlotrs->WIDTH;
    }
    
    rate = tas_get_current_rate( cnsm );
    w = w * rate ;
    trsspice->TRLENGTH = ((float)l)*1e-6 / ((float)SCALE_X) ;
    trsspice->TRWIDTH = ((float)w)*1e-6 / ((float)SCALE_X) ;
    
    trlist = addchain( NULL, trsspice );
    stm_solver_add_model( stmm,
                       (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                       (char(*)(void*,float,float,float*))mcc_spice_vds_list,
                          trlist,
                          trlist
                        );
    freechain( trlist );
   

    br->HEAD = stm_solver_maillon_addchain( br->HEAD, stmm );
  }

  br->HEAD = stm_solver_maillon_reverse( br->HEAD );

  return br;
}
/******************************************************************************\
Libère une branche 
\******************************************************************************/
void tpiv_freebranch( tpiv *br )
{
  chain_list              *scantrsmcc;
  chain_list              *scantrsspice;
  mcc_trans_mcc           *trsmcc;
  mcc_trans_spice         *trsspice;

  stm_solver_maillon_freechain( br->HEAD );
  
  for( scantrsmcc = br->HEADTRSMCC ; 
       scantrsmcc ; 
       scantrsmcc = scantrsmcc->NEXT 
     ) {
    trsmcc = (mcc_trans_mcc*)scantrsmcc->DATA;
    mcc_delete_trans_mcc( trsmcc );
  }
  freechain( br->HEADTRSMCC );

  for( scantrsspice = br->HEADTRSSPICE ; 
       scantrsspice ; 
       scantrsspice = scantrsspice->NEXT 
     ) 
  {
    trsspice = (mcc_trans_spice*)scantrsspice->DATA;
    mcc_delete_trans_spice( trsspice );
  }
  freechain( br->HEADTRSSPICE );

  tpiv_free( br );
}

/******************************************************************************\
Crée un modèle timing_iv de la branche CNS passée en paramètre.
Pour l'instant, le conflit modélisé par une capacité calculée avec MCC.
head    Premier maillon de la branche
active  Maillon qui commute
vref    Polarisation de la branche (vdd ou vss)
pconf0  Paramètre pour le calcul de la capacité
pconf1  Paramètre pour le calcul de la capacité
fin_vi  Entrée : tension initiale
fin_vf  Entrée : tension finale
fin_vs  Entrée : seuil logique
vouti   Sortie : tension initiale
\******************************************************************************/
timing_iv *tpiv_dualmodel( cone_list *cone,
                           branch_list *branch,
                           link_list *active, 
                           float      vref,
                           float      pconf0, 
                           float      pconf1,
                           float      fin_vi,
                           float      fin_vf,
                           float      fin_vs,
                           float      vouti
                         )
{
  tpiv      *br;
  timing_iv *iv = NULL;
  tpiv_i     d;
  stm_solver_maillon_list *maillon;
  link_list *link;
  float      vdd;
  double     capai;
  double     irap;
  double     temps;
  double AX, BX, rtx, rx, VT, QX, QY, thr, vddmax, vddin, imax ;
  char       trtype;
  
  tas_getmcc( branch->LINK, 
              active, 
              &vddin, 
              &AX, 
              &BX, 
              &rtx, 
              &rx, 
              &VT, 
              &QX, 
              &QY, 
              &thr, 
              &vddmax, 
              NULL, 
              NULL, 
              NULL, 
              NULL,
              NULL
            );
  imax = tas_get_current ( branch, branch->LINK, 1.0, NO );
  pconf0 = pconf0 + tas_getcapaparams (cone, branch, active, imax, QX, QY, &capai, &irap, &temps,TAS_UNKNOWN_EVENT,0);
 
  switch( TAS_CONTEXT->TAS_LEVEL ) {
  case 3:
    trtype = TAS_TRMODEL_MCCRSAT;
    break;
  case 4:
    trtype = TAS_TRMODEL_SPICE;
    break;
  default:
    trtype = TAS_TRMODEL_MCCRSAT;
    break;
  }
  br = tpiv_createbranch( branch->LINK, trtype );
  
  d.HEAD   = br->HEAD ;
  d.TRTYPE = trtype ;
  d.VDDMAX = vddmax ;
  
  for( link=branch->LINK, maillon=br->HEAD ;
       link && link != active ;
       link = link->NEXT, maillon = maillon->NEXT );
  
  if( link ) {
    d.ACTIVE = maillon ;
    d.TRS    = link->ULINK.LOTRS ;

    // Polarise le dernier élément
    for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
    maillon->MAILLON->VS=vref;

    vdd = tas_getparam( branch->LINK->ULINK.LOTRS, TAS_CASE, TP_VDDmax );
    iv = stm_modiv_create( 10, 
                           10, 
                           vdd, 
                           vdd,
                           (char(*)(void*,float,float,float*))tpiv_calc_i,
                           (void*)&d
                         );
    if( iv ) {
      stm_modiv_set_cf( iv, capai, pconf0, pconf1, irap );
      stm_modiv_set_in( iv, VT, fin_vi, fin_vf, fin_vs );
      stm_modiv_set_ti( iv, vouti );
    }
  }
  tpiv_freebranch( br );
  
  return iv;
}

char tpiv_calc_i( tpiv_i *model, float ve, float vs, float *is )
{
  chain_list *chtrs ;
  mcc_trans_mcc *trsmcc ;
  mcc_trans_spice *trsspi ;
  char ret;
  float vg;
  
  for( chtrs = model->ACTIVE->MAILLON->MODEL_VDS ; chtrs ; chtrs = chtrs->NEXT ) {
    if( model->TRTYPE == TAS_TRMODEL_MCCRSAT ) {
      trsmcc = (mcc_trans_mcc*)chtrs->DATA ;
      if( trsmcc->TYPE == model->TRS->TYPE )
        vg = ve ;
      else
        vg = model->VDDMAX - ve ;
      trsmcc->VG = vg ;
    }
    else {
      trsspi = (mcc_trans_spice*)chtrs->DATA ;
      if( ( trsspi->TRANSTYPE == MCC_NMOS && MLO_IS_TRANSN( model->TRS->TYPE ) ) ||
          ( trsspi->TRANSTYPE == MCC_PMOS && MLO_IS_TRANSP( model->TRS->TYPE ) )    )
        vg = ve ;
      else 
        vg = model->VDDMAX - ve ;
      trsspi->VG = vg ;
    }
  }
  ret = stm_solver_i( model->HEAD, vs, is );
  return ret ;
}

void tpiv_set_voltage_driving_lotrs( lotrs_list *lotrs, float voltage )
{
  ptype_list *ptl ;
  
  ptl = getptype( lotrs->GRID->SIG->USER, TAS_SIG_VOLTAGE );
  if( !ptl ) {
    lotrs->GRID->SIG->USER = addptype( lotrs->GRID->SIG->USER, TAS_SIG_VOLTAGE, 0l );
    ptl = lotrs->GRID->SIG->USER ;
  }

  ptl->DATA = (void*)((long)(voltage*1000.0));
}

void tpiv_clean_voltage_driving_lotrs( lotrs_list *lotrs )
{
  lotrs->GRID->SIG->USER = testanddelptype( lotrs->GRID->SIG->USER, TAS_SIG_VOLTAGE );
}

float tpiv_get_voltage_driving_lotrs( lotrs_list *lotrs )
{
  cone_list *cone_avant;
  alim_list *power;
  float      voltage=0.0 ;
  float      vdeg=0.0 ;
  char       usedefault ;
  ptype_list *ptl, *ptype ;

  ptl = getptype( lotrs->GRID->SIG->USER, TAS_SIG_VOLTAGE );
  if( ptl )
    return ((float)((long)(ptl->DATA)))/1000.0 ;

  usedefault = 1 ;

  cone_avant = (cone_list *)getptype( lotrs->USER, CNS_DRIVINGCONE )->DATA ;

  if( cone_avant ) {

    power = cns_get_multivoltage( cone_avant );
    
    if( power ) {

      usedefault = 0 ;

      if( ( lotrs->TYPE & CNS_TN) == CNS_TN ) {
        if( TAS_PATH_TYPE == 'M' )
          voltage = power->VDDMIN;
        else
          voltage = power->VDDMAX;
      }
      else {
        if( TAS_PATH_TYPE == 'M' )
          voltage = power->VSSMAX;
        else
          voltage = power->VSSMIN;
      }
    }
  
    /* solution provisoire : ici, on prend la dégradation du maillon courant,
       pas celle du cone d'entrée. */

    if( MLO_IS_TRANSN( lotrs->TYPE) &&
        ((cone_avant->TECTYPE & CNS_VDD_DEGRADED) == CNS_VDD_DEGRADED) ) {
      vdeg = tas_getparam( lotrs, TAS_CASE, TP_deg);
      if((ptype = getptype(cone_avant->USER, TAS_VDD_NOTDEG)) != NULL){
          vdeg += *(float*)(&ptype->DATA);
      }
      vdeg -= tas_getparam ( lotrs, TAS_CASE, TP_VDDmax);

    }
    else {
      if( MLO_IS_TRANSP( lotrs->TYPE) &&
         ((cone_avant->TECTYPE & CNS_VSS_DEGRADED) == CNS_VSS_DEGRADED) ) {
        vdeg = tas_getparam( lotrs, TAS_CASE, TP_deg);
      }
    }
  }

  /* prise en compte si degradation des inputs */

  if( usedefault == 1 ) {
    if( ( lotrs->TYPE & CNS_TN) == CNS_TN )
      voltage = tas_getparam ( lotrs, TAS_CASE, TP_VDDmax);
    else
      voltage = 0.0;
  }

  voltage = voltage + vdeg ;

  return voltage ;
}

/******************************************************************************\
Renvoie la tension de blocage d'un transistor pour le calcul du leakage
\******************************************************************************/
float tpiv_get_blocked_voltage_driving_lotrs( lotrs_list *lotrs )
{
  cone_list *cone_avant;
  alim_list *power;
  float      voltage=0.0 ;
  float      delta=0.0;
  char       usedefault ;

  usedefault = 1 ;

  cone_avant = (cone_list *)getptype( lotrs->USER, CNS_DRIVINGCONE )->DATA ;

  if( cone_avant ) {

    power = cns_get_multivoltage( cone_avant );
    
    if( power ) {
      /* tasLeakageRatio Calcul du delta voltage sur la tension de grille pour le leakage */
      if (V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].SET){
        if( TAS_PATH_TYPE == 'M' )
          delta = power->VDDMIN * V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].VALUE;
        else
          delta = power->VDDMAX * V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].VALUE;
      }

      usedefault = 0 ;

      /* Si TN alors on la tension de blocage n'est pas tout a fait 0v si delta!=0 */
      if( ( lotrs->TYPE & CNS_TN) == CNS_TN ) {
        if( TAS_PATH_TYPE == 'M' )
          voltage = power->VSSMAX + delta;
        else
          voltage = power->VSSMIN + delta;
      }
      else {
        /* Si TP alors on la tension de blocage n'est pas tout a fait VDD si delta!=0 */
        if( TAS_PATH_TYPE == 'M' )
          voltage = power->VDDMIN - delta;
        else
          voltage = power->VDDMAX - delta;
      }
    }
  }
  
  if( usedefault == 1 ) {
    if (V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].SET){
      delta = tas_getparam ( lotrs, TAS_CASE, TP_VDDmax) * V_FLOAT_TAB[__AVT_LEAKAGE_RATIO].VALUE;
    }
    if( ( lotrs->TYPE & CNS_TP) == CNS_TP )
      voltage = tas_getparam ( lotrs, TAS_CASE, TP_VDDmax) - delta;
    else
      voltage = 0.0 + delta;
  }

  return voltage ;
}


/******************************************************************************\
Calcul le courant maximum dans une branche en utilisant stm_solver.
\******************************************************************************/
char tpiv_i_brdual( head, vout, valim, imax, savepol )
link_list *head;
float      vout;
float      valim;
float     *imax;
char       savepol;
{
  tpiv *br ;
  char  r ;
  char  brtype ;
  stm_solver_maillon_list *maillon ;
  link_list *link ;

  if( ( head->TYPE & (CNS_IN | CNS_INOUT ) ) != 0 ) 
    return 0;
    
  for( link = head->NEXT ; link ; link = link->NEXT ) {
    if ((link->TYPE & CNS_EXT) == CNS_EXT) break;
    if( !V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ) {
      if( link->ULINK.LOTRS->TYPE != head->ULINK.LOTRS->TYPE )
        return 0;
    }
  }
  
  switch( TAS_CONTEXT->TAS_LEVEL ) {
  case 1:
  case 3:
    brtype = TAS_TRMODEL_MCCRSAT;
    break;
  case 2:
  case 4:
    brtype = TAS_TRMODEL_SPICE;
    break;
  default:
    brtype = TAS_TRMODEL_MCCRSAT;
    break;
  }

  br = tpiv_createbranch( head, brtype );
  for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
  maillon->MAILLON->VS=valim;

  r = stm_solver_i( br->HEAD, vout, imax );

  if( savepol == YES ) {
    for( maillon = br->HEAD, link = head ; 
         maillon ; 
         maillon = maillon->NEXT, link = link->NEXT ) {

      // old: if(link->ULINK.LOTRS->TYPE==TRANSP)
      if(MLO_IS_TRANSP(link->ULINK.LOTRS->TYPE))
        tas_set_vpol( link, valim - maillon->MAILLON->VD );
      else
        tas_set_vpol( link, maillon->MAILLON->VD );
    }
  }

  tpiv_freebranch( br );
  
  if( TPIV_DEBUG_IBR &&
      ( V_BOOL_TAB[ __TAS_CHECK_IMAX ].VALUE || V_BOOL_TAB[ __TAS_USE_BSIM_CURRENT ].VALUE ) 
    ) {
    float imcc ;
    float d ;
    static FILE *file=NULL ;

    TPIV_DEBUG_IBR = 0 ;
    for( link = head ; link ; link = link->NEXT ) {
      if( ( link->TYPE & CNS_EXT ) == CNS_EXT )
        break ;
      if( getptype( link->ULINK.LOTRS->USER, MBK_TRANS_PARALLEL ) )
        break ;
      if( getptype( link->ULINK.LOTRS->USER, TAS_TRANS_SWITCH ) )
        break ;
    }
    
    if( !link ) {
      if( V_BOOL_TAB[ __TAS_CHECK_IMAX ].VALUE ) {
        if( !file )
          file = mbkfopen( "current", "dat", "w" );
      }
      brtype = TAS_TRMODEL_SPICE;
      br = tpiv_createbranch( head, brtype );
      for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
      maillon->MAILLON->VS=valim;
      stm_solver_i( br->HEAD, vout, &imcc );
      d = (*imax-imcc)*100.0/imcc ;
      if( V_BOOL_TAB[ __TAS_CHECK_IMAX ].VALUE ) {
        fprintf( file, "error=%f\n",d );
      }
      tpiv_freebranch( br );
      if( V_BOOL_TAB[ __TAS_USE_BSIM_CURRENT ].VALUE )
        *imax = imcc ;
    }
  }

  return r;
}

/******************************************************************************\
Calcul du courant leakage d'une branche dont le transistor activelink est bloque
\******************************************************************************/
char tpiv_i_brdual_leakage( head, activelink, vout, valim, imax)
link_list *head;
link_list *activelink;
float      vout;
float      valim;
float     *imax;
{
  tpiv *br ;
  char  r ;
  char  brtype ;
  stm_solver_maillon_list *maillon ;
  link_list *link ;

  if( ( head->TYPE & (CNS_IN | CNS_INOUT ) ) != 0 ) 
    return 0;
    
  for( link = head->NEXT ; link ; link = link->NEXT ) {
    if ((link->TYPE & CNS_EXT) == CNS_EXT) break;
    if( !V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ) {
      if( link->ULINK.LOTRS->TYPE != head->ULINK.LOTRS->TYPE )
        return 0;
    }
  }
  
  brtype = TAS_TRMODEL_SPICE;

  br = tpiv_createbranch_leakage( head, activelink, brtype );
  for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
  maillon->MAILLON->VS=valim;

  r = stm_solver_i( br->HEAD, vout, imax );

  tpiv_freebranch( br );
  

  return r;
}

char tpiv_i_brdual_leakage_2( head, activelink, vout, valim, imax)
link_list *head;
link_list *activelink;
float      vout;
float      valim;
float     *imax;
{
  tpiv *br ;
  char  r ;
  char  brtype ;
  stm_solver_maillon_list *maillon ;
  link_list *link ;

  if( ( head->TYPE & (CNS_IN | CNS_INOUT ) ) != 0 ) 
    return 0;
    
  for( link = head->NEXT ; link ; link = link->NEXT ) {
    if ((link->TYPE & CNS_EXT) == CNS_EXT) break;
    if( !V_BOOL_TAB[ __AVT_NEW_SWITCH_MODEL ].VALUE ) {
      if( link->ULINK.LOTRS->TYPE != head->ULINK.LOTRS->TYPE )
        return 0;
    }
  }
  
  brtype = TAS_TRMODEL_SPICE;

  br = tpiv_createbranch_leakage_2( head, activelink, brtype );
  for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
  maillon->MAILLON->VS=valim;

  r = stm_solver_i( br->HEAD, vout, imax );

  tpiv_freebranch( br );
  

  return r;
}
/******************************************************************************\
Renvoie le courant d'une branche lorsque tous les maillons commandés par le même
signal que celui qui controle activelink valent vin. Les autres sont drivés par 
les tensions des cones d'avant.
\******************************************************************************/
int tpiv_get_i_multi_input( link0, activelink, vout, valim, vin, vinopp, trmodel, imax )
link_list *link0;
link_list *activelink;
float      vout;
float      valim;
float      vin;
float      vinopp;
char       trmodel;
float     *imax;
{
  tpiv                    *br ;
  char                     r ;
  stm_solver_maillon_list *maillon ;
  link_list               *link ;
  
  br = tpiv_createbranch( link0, trmodel );
  for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
  maillon->MAILLON->VS=valim;
  
  for( maillon = br->HEAD, link = link0 ; 
       maillon ; 
       maillon = maillon->NEXT, link = link->NEXT ) {

    if( link->ULINK.LOTRS->GRID->SIG == activelink->ULINK.LOTRS->GRID->SIG ) {
      tas_tpiv_set_vg_for_switch( maillon->MAILLON, 
                                  trmodel, 
                                  MLO_IS_TRANSN(activelink->ULINK.LOTRS->TYPE) ? 'N' : 'P', 
                                  vin, 
                                  vinopp
                                );
    }
  }

  r = stm_solver_i( br->HEAD, vout, imax );

  tpiv_freebranch( br );

  return r ;
}

/******************************************************************************\
Trace dans un fichier le courant d'un branche lorsque le maillon 'actif' varie
de 'vin_d' a 'vin_f', et lorsque la la tension de sortie varie de 'vout_d' à 'vout_f'
Calcul le courant maximum dans une branche en utilisant stm_solver.
\******************************************************************************/
void tpiv_i_trace_brdual( filename, head, actif, brtype, vin_d, vin_f, vout_d, vout_f, valim, vdd )
char      *filename;
link_list *head;
link_list *actif;
char       brtype;
float      vin_d;
float      vin_f;
float      vout_d;
float      vout_f;
float      valim;
float      vdd;
{
  tpiv *br;
  char  r;
  stm_solver_maillon_list *maillon;
  link_list *lnk;
  int        k, l ;
  FILE  *ptf;
  float v, step;
  float imax;
  char buffer[256];
  float vt;
  float timax[3], vin[3] ;
  int i;

  br = tpiv_createbranch( head, brtype );
  for( maillon = br->HEAD ; maillon->NEXT ; maillon = maillon->NEXT );
  maillon->MAILLON->VS=valim;

  k=100;

  /**** variation de vin ****/
  sprintf( buffer, "%s_vin.dat", filename );
  ptf = fopen( buffer, "w" );
  step = (vin_f-vin_d)/((float)k) ;
  
  for( l=0 , v=vin_d ; l<=k ; l++, v=v+step ) {
    for( maillon = br->HEAD, lnk = head ; maillon && lnk ; maillon = maillon->NEXT, lnk = lnk->NEXT ) {
      if( lnk->ULINK.LOTRS->GRID->SIG == actif->ULINK.LOTRS->GRID->SIG ) {
        tas_tpiv_set_vg_for_switch( maillon->MAILLON, 
                                    brtype, 
                                    MLO_IS_TRANSN(actif->ULINK.LOTRS->TYPE) ? 'N' : 'P', 
                                    v, 
                                    vdd-v 
                                  );
      }
    }

    r = stm_solver_i( br->HEAD, vout_f, &imax );
    fprintf( ptf, "%g %g\n", v, -imax );
  }

  fclose( ptf );

  /**** variation de vout ****/
  sprintf( buffer, "%s_vout.dat", filename );
  ptf = fopen( buffer, "w" );
  vt = tas_getparam( actif->ULINK.LOTRS, TAS_CASE, TP_VT ); 
  step = (vout_f-vout_d)/((float)k) ;

  if( MLO_IS_TRANSN(actif->ULINK.LOTRS->TYPE) ) {
    vin[0] = 3.0 * (vin_f-vt)/3.0 + vt ;
    vin[1] = 2.0 * (vin_f-vt)/3.0 + vt ;
    vin[2] = 1.0 * (vin_f-vt)/3.0 + vt ;
  }
  else {
    vin[0] = 0.0*(vdd-vt-vin_f)/3.0 + vin_f ;
    vin[1] = 1.0*(vdd-vt-vin_f)/3.0 + vin_f ;
    vin[2] = 2.0*(vdd-vt-vin_f)/3.0 + vin_f ;
  }

  fprintf( ptf, "#vin %g %g %g\n", vin[0], vin[1], vin[2] );
  for( l=0 , v=vout_d ; l<=k ; l++, v=v+step ) {

    for( i=0; i<=2 ; i++ ) {
      for( maillon = br->HEAD, lnk = head ; maillon && lnk ; maillon = maillon->NEXT, lnk = lnk->NEXT ) {
        if( lnk->ULINK.LOTRS->GRID->SIG == actif->ULINK.LOTRS->GRID->SIG ) {
          tas_tpiv_set_vg_for_switch( maillon->MAILLON, 
                                      brtype, 
                                      MLO_IS_TRANSN(actif->ULINK.LOTRS->TYPE) ? 'N' : 'P', 
                                      vin[i], 
                                      vdd-vin[i] 
                                    );
        }
      }

      r = stm_solver_i( br->HEAD, v, &timax[i] );
    }
    fprintf( ptf, "%g %g %g %g\n", v, -timax[0], -timax[1], -timax[2]  );
  }

  fclose( ptf );

  tpiv_freebranch( br );
}

/******************************************************************************\
Alloue une structure tpiv
\******************************************************************************/
tpiv* tpiv_alloc( void )
{
  tpiv *s;
  s = (tpiv*)addptype( NULL, 0l, NULL );
  s->HEAD=NULL;
  s->HEADTRSSPICE=NULL;
  s->HEADTRSMCC=NULL;

  return s;
}

/******************************************************************************\
Libère une structure tpiv
\******************************************************************************/
void tpiv_free( tpiv *s )
{
  ptype_list *ptl;
  ptl = (ptype_list*)s;
  ptl->NEXT = NULL;
  freeptype( ptl );
}

/******************************************************************************\
Trace la tension de sortie d'un inverseur.
\******************************************************************************/

void tpiv_inverter( char       *fname,
                    lotrs_list *tn, 
                    lotrs_list *tp, 
                    char        sens,
                    float       f,
                    float       r,
                    float       c1,
                    float       c2,
                    
                    stm_driver *driver,
                    float       tmax,
                    dualparams *scmmodel,
                    
                    char        transtype
                  )
{
  stm_solver_maillon      *stmp;
  stm_solver_maillon      *stmn;
  mcc_trans_mcc           *trsmccn;
  mcc_trans_spice         *trsspicen;
  mcc_trans_mcc           *trsmccp;
  mcc_trans_spice         *trsspicep;
  long                     w, l;
  FILE                    *file;
  double                   dt;
  double                   ve, vs, lve, lvs, v2, lv2 ;
  double                   t;
  int                      rn, rp;
  float                    in, ip, lin, lip ;
  double                   qp, qn;
  double                   te, ts ;
  float                    vbp, vbn ;
  double                   qsatn ,qsatp, qsat, qsatx ;
  double                   qovrn ,qovrp, qovr ;
  double                   qconfn ,qconfp, qconf ;
  float                    isatn, isatp, lisatn, lisatp ;
  double                   seuil1, seuil2, ts1, ts2, fth ;
  double                   seuil ;
  chain_list              *trslist;
  char                     trtype ;
  int                      reverse ;
  double                   t0 ;
  double                   dt0 ;
  ptype_list              *ptl ;
  chain_list              *chain ;
  lotrs_list              *lotrs ;
  int                      encore ;
  int                      k ;
  stm_dual_param_timing    paramtiming ;
  double                   vt, vdd, ci, capaie[4], vcap[3], vt0, vt0c, dtpwl, load ;

  vt   = scmmodel->DP[STM_VT];
  vt0  = scmmodel->DP[STM_VT0];
  vt0c = scmmodel->DP[STM_VT0C];
  vdd  = scmmodel->DP[STM_VDDMAX];
  ci   = scmmodel->DP[STM_CAPAI];
  capaie[0] = scmmodel->DP[STM_CAPAI0] ;
  capaie[1] = scmmodel->DP[STM_CAPAI1] ;
  capaie[2] = scmmodel->DP[STM_CAPAI2] ;
  capaie[3] = scmmodel->DP[STM_CAPAI3] ;
  vcap[0] = vt0 ;
  vcap[1] = vdd/2.0 ;
  vcap[2] = vt0c ;

  if( r>0 && c2>0.0 )
    load = stm_modscm_dual_capaeq (scmmodel, NULL, f, r, c1, c2, scmmodel->DP[STM_THRESHOLD], NULL );
  else
    load = c1 ;
  stm_modscm_dual_fill_param( scmmodel, f, NULL, driver, load, &paramtiming ) ;

  reverse=tpiv_inverter_config_reverse;
  if (sens=='U') t0=tpiv_inverter_config_t0r; else t0=tpiv_inverter_config_t0f;
 
  if( sens=='U' )
    printf( "ud" );
  else
    printf( "du" );
  printf( " f=%.1f r=%.3g c1=%.3gfF c2=%.3gfF ci=%.3gfF\n", f, r, c1, c2, ci );

  file = fopen( fname, "w" );
  if( !file ) {
    perror( "can't open file " );
    return;
  }

  vbn = 0.0 ;
  vbp = 0.0 ;
  
  switch( transtype ) {
  case 'S' :
    if( tn->BULK && tn->BULK->SIG ) {
      if( ! getlosigalim( tn->BULK->SIG, &vbn ) )
        vbn = 0.0 ;
    }
    if( tp->BULK && tp->BULK->SIG ) {
      if( ! getlosigalim( tp->BULK->SIG, &vbp ) )
        vbp = 0.0 ;
      else
        vbp = vbp-vdd;
    }
    break ;
  }

  if( sens=='U' ) {
    seuil1 = vdd * SIM_VTH_HIGH ;
    seuil2 = vdd * SIM_VTH_LOW ;
  }
  else {
    seuil1 = vdd * SIM_VTH_LOW ;
    seuil2 = vdd * SIM_VTH_HIGH ;
  }
  
  ci = ci / 1000.0 ;
  c1 = c1 / 1000.0 ;
  if( c2>0.0 ) c2 = c2 / 1000.0 ;

  stmp = stm_solver_new_maillon();
  stmn = stm_solver_new_maillon();

  switch( transtype ) {
    
  case 'T' :
    trtype = TAS_TRMODEL_MCCRSAT ;
    trsmccn = mcc_create_trans_mcc( tn, vdd, TAS_CASE, NULL, 0.0, 0.0 );
    elpLotrsGetShrinkDim( tn, &l, &w, NULL, NULL, NULL, NULL, NULL, NULL, TAS_CASE );
    trsmccn->TRWIDTH  = w;
    trsmccn->TRLENGTH = l;
    trslist = addchain( NULL,trsmccn );
    ptl = getptype( tn->USER, MBK_TRANS_PARALLEL );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        lotrs = (lotrs_list*)chain->DATA ;
        if( lotrs==tn ) continue ;
        trsmccn = mcc_create_trans_mcc( lotrs, vdd, TAS_CASE, NULL, 0.0, 0.0 );
        elpLotrsGetShrinkDim( lotrs, &l, &w, NULL, NULL, NULL, NULL, NULL, NULL, TAS_CASE );
        trsmccn->TRWIDTH  = w;
        trsmccn->TRLENGTH = l;
        trslist = addchain( trslist,trsmccn );
      }
    }
    stm_solver_add_model( stmn, 
                         (char(*)(void*,float,float,float*))mcc_mcc_ids_list,
                         (char(*)(void*,float,float,float*))mcc_mcc_vds_list,
                          trslist,
                          trslist
                        );
    freechain( trslist );
    
    trsmccp = mcc_create_trans_mcc( tp, vdd, TAS_CASE, NULL, 0.0, 0.0 );
    elpLotrsGetShrinkDim( tp, &l, &w, NULL, NULL, NULL, NULL, NULL, NULL, TAS_CASE );
    trsmccp->TRWIDTH  = w;
    trsmccp->TRLENGTH = l;
    trslist = addchain( NULL, trsmccp );
    ptl = getptype( tp->USER, MBK_TRANS_PARALLEL );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        lotrs = (lotrs_list*)chain->DATA ;
        if( lotrs==tp ) continue ;
        trsmccp = mcc_create_trans_mcc( lotrs, vdd, TAS_CASE, NULL, 0.0, 0.0 );
        elpLotrsGetShrinkDim( lotrs, &l, &w, NULL, NULL, NULL, NULL, NULL, NULL, TAS_CASE );
        trsmccp->TRWIDTH  = w;
        trsmccp->TRLENGTH = l;
        trslist = addchain( trslist,trsmccp );
      }
    }
    stm_solver_add_model( stmp, 
                           (char(*)(void*,float,float,float*))mcc_mcc_ids_list,
                           (char(*)(void*,float,float,float*))mcc_mcc_vds_list,
                          trslist,
                          trslist
                            );
    freechain( trslist );
    break;
      
  case 'S' :
    trtype = TAS_TRMODEL_SPICE;
    trsspicen = mcc_create_trans_spice(  tn, vdd, NULL, 0.0, 0.0 );
    trslist = addchain( NULL, trsspicen );
    ptl = getptype( tn->USER, MBK_TRANS_PARALLEL );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        lotrs = (lotrs_list*)chain->DATA ;
        if( lotrs==tn ) continue ;
        trsspicen = mcc_create_trans_spice(  lotrs, vdd, NULL, 0.0, 0.0 );
        trslist = addchain( trslist, trsspicen );
      }
    }
    stm_solver_add_model( stmn, 
                         (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                         (char(*)(void*,float,float,float*))mcc_spice_vds,
                          trslist,
                          trslist
                        );
    freechain( trslist );
    
    trsspicep = mcc_create_trans_spice(  tp, vdd, NULL, 0.0, 0.0 );
    trslist = addchain( NULL, trsspicep );
    ptl = getptype( tp->USER, MBK_TRANS_PARALLEL );
    if( ptl ) {
      for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
        lotrs = (lotrs_list*)chain->DATA ;
        if( lotrs==tp ) continue ;
        trsspicep = mcc_create_trans_spice(  lotrs, vdd, NULL, 0.0, 0.0 );
        trslist = addchain( trslist, trsspicep );
      }
    }
    stm_solver_add_model( stmp, 
                         (char(*)(void*,float,float,float*))mcc_spice_ids_list,
                         (char(*)(void*,float,float,float*))mcc_spice_vds_list,
                          trslist,
                          trslist
                        );
    freechain( trslist );
    break;
   
  default :
    fprintf( stderr, "Bad transistor model.\n" );
    EXIT(1);
  }

  dt = 0.05 ;
  if( sens == 'U' ) {
    vs = vdd ;
    ve = 0.0 ;
  }
  else {
    vs = 0.0 ;
    ve = vdd ;
  }
  v2 = vs ;

  te = 1e-12 * ( stm_modscm_dual_calte (vdd-vt, vt, vdd/2.0, f ) - stm_modscm_dual_calte (vdd-vt, vt, 0, f ) );
  dt0 = t0-te ;

  lin    =  0.0 ;
  lip    =  0.0 ;
  lisatn =  0.0 ;
  lisatp =  0.0 ;
  qn     =  0.0 ;
  qp     =  0.0 ;
  qsatn  =  0.0 ;
  qsatp  =  0.0 ;
  qovrn  =  0.0 ;
  qovrp  =  0.0 ;
  qconfn =  0.0 ;
  qconfp =  0.0 ;
  qovr   = -1.0 ;
  ts1    = -1.0 ;
  ts2    = -1.0 ;
  te     = -1.0 ;
  ts     = -1.0 ;

  fprintf( file, "# tas time origin : vt=%g, tsim=%g\n\n", vt, dt0 + 1e-12 * ( stm_modscm_dual_calte (vdd-vt, vt, vt, f ) - stm_modscm_dual_calte (vdd-vt, vt, 0, f ) ) );
  fprintf( file, "# %12s %12s %12s %12s %12s %12s %12s %12s\n\n",
                 "T", "ve", "vs", "in", "ip", "qn", "qp", "qsat"
         );
  
  t=0.0;
  encore = 0 ;
  k = 0 ;

  if( paramtiming.NTHSHRK ) {
    dtpwl = stm_get_t_pwth( paramtiming.VDDIN, vt, 0.0, paramtiming.PWTHSHRK );
  }
  
  while( ts < 0.0 || te < 0.0 || ts1 < 0.0 || ts2 < 0.0 || encore ) {
 
    lvs = vs ;
    lv2 = v2 ;
    lve = ve ;
    
    if( sens == 'U' ) {

      if( paramtiming.NTHSHRK ) 
        ve = stm_get_v_pwth( t+dtpwl, vt, vdd, STM_UP, paramtiming.PWTHSHRK );
      else
        ve = stm_get_v (t, vt, 0.0, vdd, f); 

      if( ve > vdd/2.0 ) {
        if( te < 0.0 )
          te=t ;
      }
      while( k <3 && ve >= vcap[k] )
        k++ ;
    }
    else {

      if( paramtiming.NTHSHRK ) 
        ve = stm_get_v_pwth( t+dtpwl, vt, vdd, STM_DN, paramtiming.PWTHSHRK );
      else
        ve = stm_get_v (t, vdd-vt, vdd, 0.0, f); 

      if( ve < vdd/2.0 ) {
        if( te < 0.0 )
          te=t ;
      }
      while( k <3 && ve < vcap[2-k] )
        k++ ;
    }

    //ci = capaie[k]/1000.0;

    tas_tpiv_solver_calc_ids( stmn, ve, vbn, vdd, &isatn, trtype );
    tas_tpiv_solver_calc_ids( stmp, (ve-vdd), vbp, -vdd, &isatp, trtype );
   
    rn = tas_tpiv_solver_calc_ids( stmn, ve, vbn, vs, &in, trtype );
    rp = tas_tpiv_solver_calc_ids( stmp, (ve-vdd), vbp, (vs-vdd), &ip, trtype );

    if( !reverse ) { 
      if( in < 0.0 ) 
        in = 0.0 ;
      if( ip > 0.0 ) 
        ip = 0.0 ;
    }
    
    if( rn && rp ) {
      if( r >=0.0 && c2 >= 0.0 ) {
        vs = ( (-ip)-in + ci*(ve-lve)/dt + c1*lvs/dt - r*c2*c2*lv2/(dt+r*c2)/dt + c2*lv2/dt ) / ( c1/dt + c2/(dt+r*c2));
        v2 = dt/(dt+r*c2)*(vs+r*c2*lv2/dt);
      }
      else 
        vs = ( dt * ( (-in) - ip ) + c1 * lvs + ci * ( ve - lve ) ) / c1 ;
    }
    else {
      printf( "error : ve=%g vs=%g vbn=%g vbp=%g rn=%d rp=%d\n",
              ve, vs, vbn, vbp, rn, rp 
            );
      break ;
    }


    qsatx = 0.0 ;
    if( in > 0.0 ) {
      qn = qn + ( in + lin ) * (dt*1.0e-12) / 2.0 ;
      qsatn  = qsatn + ( (isatn-in) + (lisatn-lin) ) * (dt*1.0e-12) / 2.0 ;
    }
    else {
      qovrn = qovrn + ( in + lin ) * (dt*1.0e-12) / 2.0 ;
    }

    if( ip < 0.0 ) {
      qp = qp + ( ip + lip ) * (dt*1.0e-12) / 2.0 ;
      qsatp  = qsatp + ( (isatp-ip) + (lisatp-lip) ) * (dt*1.0e-12) / 2.0 ;
    }
    else {
      qovrp = qovrp + ( ip + lip ) * (dt*1.0e-12) / 2.0 ;
    }
    
    lin    = in ;
    lip    = ip ;
    lisatn = isatn ;
    lisatp = isatp ;
   
    if( sens == 'U' ) 
      qsatx = qsatn ;
    else
      qsatx = -qsatp ;

    fprintf( file, "%12g %12g %12g %12g %12g %12g %12g %12g\n", 
             t*1e-12+dt0, ve, vs, in, ip, qn, qp, qsatx 
           );

    if( sens == 'U' ) {
      if ( vs < vdd/2.0 ) {
        if( ts < 0.0 ) {
          ts = t ;
          qsat  = qsatn ;
          qconf = -qp ;
        }
      }
      if( vs < seuil1 ) {
        if( ts1 < 0.0 ) 
          ts1 = t ;
      }
      if( vs < seuil2 ) {
        if( ts2 < 0.0 ) 
          ts2 = t ;
      }
      if( ve > vt ) {
        if( qovr < 0.0 )
          qovr  = fabs(qovrp) ;
      }
    }
    else {
      if ( vs > vdd/2.0 ) {
        if( ts < 0.0 ) {
          ts    = t ;
          qsat  = -qsatp ;
          qconf = qn;
        }
      }
      if( vs > seuil1 ) {
        if( ts1 < 0.0 )
          ts1 = t;
      }
      if( vs > seuil2 ) {
        if( ts2 < 0.0 )
          ts2 = t;
      }
      if( ve < vdd-vt ) {
        if( qovr < 0.0 )
          qovr  = fabs(qovrn) ;
      }
    }

    t = t + dt ;

    encore = 1 ;
    if( tmax > 0.0 ) {
      if( t >= tmax )
        encore = 0 ;
    }
    else {
      if( sens=='U' ) { /* ud */
        if( vs < 0.01*vdd )
          encore = 0 ;
      }
      else {
        if( vs > 0.99*vdd )
          encore = 0 ;
      }
    }
  }

  if( sens=='U' )
    fth = stm_thr2scm( ts2-ts1, seuil2/vdd, seuil1/vdd, vt, 0.0, vdd, STM_DN );
  else
    fth = stm_thr2scm( ts2-ts1, seuil1/vdd, seuil2/vdd, vt, vdd, vdd, STM_UP );
 
  printf( "   -> te=%g ts=%g tp=%g\n", te, ts, ts-te );
  printf( "      t1=%g t2=%g f=%g (fshrk=%.2f)\n", ts1, ts2, ts2-ts1, fth );
  printf( "      qsat     = %.3gfC (%.3gfF)\n",
          qsat*1.0e15, qsat/(vdd/2.0)*1.0e15 );
  printf( "      qconf    = %.3gfC (%.3gfF)\n", 
          qconf*1.0e15, qconf/(vdd/2.0)*1.0e15 );
  printf( "      qovr(vt) = %.3gfC (%.3gfF)\n",
          qovr*1.0e15, qovr/(vdd/2.0)*1.0e15
        );

  fclose( file );

  return ;
  /************** Calcul du conflit en statique ***********/

  /* seuil statique */

  seuil = tpiv_getseuil( stmn, stmp, vdd, vbn, vbp, 1, trtype );
  printf( "   Static threshold (vin where vout=vdd/2.0) (satured) : %.3gV\n", seuil );
  qn = tpiv_getconflict( stmn, stmp, vdd, vbn, vbp, vt, f, seuil, sens, dt, 1, trtype );
  printf( "   -> Satured Qconf=%.3gfF (Cconf=%.3gfF)\n", qn*1e15, qn*1e15/(vdd/2.0) );
  qn = tpiv_getconflict( stmn, stmp, vdd, vbn, vbp, vt, f, seuil, sens, dt, 0, trtype );
  printf( "   -> Real    Qconf=%.3gfF (Cconf=%.3gfF)\n", qn*1e15, qn*1e15/(vdd/2.0) );

  seuil = tpiv_getseuil( stmn, stmp, vdd, vbn, vbp, 0, trtype );
  printf( "   Static threshold (vin where vout=vdd/2.0) (real) :    %.3gV\n", seuil );
  qn = tpiv_getconflict( stmn, stmp, vdd, vbn, vbp, vt, f, seuil, sens, dt, 1, trtype );
  printf( "   -> Satured Qconf=%.3gfF (Cconf=%.3gfF)\n", qn*1e15, qn*1e15/(vdd/2.0) );
  qn = tpiv_getconflict( stmn, stmp, vdd, vbn, vbp, vt, f, seuil, sens, dt, 0, trtype );
  printf( "   -> Real    Qconf=%.3gfF (Cconf=%.3gfF)\n", qn*1e15, qn*1e15/(vdd/2.0) );

}

double tpiv_getconflict( stm_solver_maillon *stmn, stm_solver_maillon *stmp, double vdd, double vbn, double vbp, double vt, double f, double seuil, char sens, double dt, char satured, char brtype )
{
  float i, li ;
  double q ;
  double ve ;
  double vs ;
  int r ;
  double t ;
  
  li = 0.0 ;
  q  = 0.0 ;

  if( sens=='U' )
    ve = 0.0 ;
  else
    ve = vdd ;

  for( t = 0.0 ; ( sens == 'U' && ve < seuil ) || ( sens == 'D' && ve > seuil ) ; t = t + dt ) {
 
    if( sens == 'U' )
      ve = stm_get_v( t, vt, 0.0, vdd, f); 
    else
      ve = stm_get_v (t, vt, vdd, 0.0, f); 
  
    if( satured ) {
      if( sens=='U' )
        vs = vdd ;
      else
        vs = 0.0 ;
    }
    else {
      vs = tpiv_getvs( stmn, stmp, ve, vbn, vbp, vdd, brtype );
    }

    if( sens=='U' )
      r = tas_tpiv_solver_calc_ids( stmn, ve, vbn, vs, &i, brtype );
    else {
      r = tas_tpiv_solver_calc_ids( stmp, (ve-vdd), vbp, vs-vdd, &i, brtype );
      i=-i;
    }

    if( !r ) 
      break ;
    q = q + ( i + li ) * (dt*1.0e-12) / 2.0 ;
    li = i ;
  }

  return q ;
}

double tpiv_getvs( stm_solver_maillon *stmn, stm_solver_maillon *stmp, double ve, double vbn, double vbp, double vdd, char brtype )
{
  double vmin ;
  double vmax ;
  int    iter ;
  double vs ;
  int    rn ;
  int    rp ;
  float  in ;
  float  ip ;
  char   stop ;
  
  iter = 100 ;
  vmin = 0.0 ;
  vmax = vdd ;

  do {
    vs = (vmax+vmin)/2.0;

    rn = tas_tpiv_solver_calc_ids( stmn, ve, vbn, vs, &in, brtype );
    rp = tas_tpiv_solver_calc_ids( stmp, (ve-vdd), vbp, vs-vdd, &ip, brtype );

    if( !rn || !rp )
      break ;

    ip=-ip;

    if( in > ip )
      vmax = vs ;
    else
      vmin = vs ;
    iter-- ;

    stop=0;
    if( fabs(ip-in)/ip < 1e-8 )
      stop = 1 ;
  }
  while( !stop && iter );

  if( !rn || !rp )
    return vdd/2.0 ;

  return (vmax+vmin)/2.0 ;
}

double tpiv_getseuil( stm_solver_maillon *stmn, stm_solver_maillon *stmp, double vdd, double vbn, double vbp, char satured, char brtype )
{
  double vmin ;
  double vmax ;
  int    iter ;
  double ve ;
  double vs ;
  int    rn ;
  int    rp ;
  float  in ;
  float  ip ;
  char   stop ;
  
  iter = 100 ;
  vmin = 0.0 ;
  vmax = vdd ;
  vs = vdd/2.0 ;

  do {
    ve = (vmax+vmin)/2.0;

    if( satured ) {
      rn = tas_tpiv_solver_calc_ids( stmn, ve, vbn, vdd, &in, brtype );
      rp = tas_tpiv_solver_calc_ids( stmp, (ve-vdd), vbp, -vdd, &ip, brtype );
    }
    else {
      rn = tas_tpiv_solver_calc_ids( stmn, ve, vbn, vs, &in, brtype );
      rp = tas_tpiv_solver_calc_ids( stmp, (ve-vdd), vbp, vs-vdd, &ip, brtype );
    }

    if( !rn || !rp )
      break ;

    ip=-ip;

    if( in > ip )
      vmax = ve ;
    else
      vmin = ve ;
    iter-- ;

    stop=0;
    if( fabs(ip-in)/ip < 1e-8 )
      stop = 1 ;
  }
  while( !stop && iter );

  if( !rn || !rp )
    return vdd/2.0 ;

  return (vmax+vmin)/2.0 ;
}

void tpiv_carac_static( lotrs_list *lotrs,
                        float       vdd,
                        float       vdsmax,
                        float       vgsmax,
                        float       vb, /* uniquement pris en compte pour modèle spice */
                        float       vs
                      )
{
  FILE            *ptf ;
  mcc_trans_mcc   *trsmcc ;
  mcc_trans_spice *trsspi ;
  char             fname[1024] ;
  char             bname[1024] ;
  int              i, j ;
  int              ni, nj ;
  long             w, l ;
  float            vi, vj ;
  float            imcc ;
  float            ispi ;
  float            vt ;
  float            vbs = vb-vs ;
  float            vbulk ;
  
  if( MLO_IS_TRANSP(lotrs->TYPE) )
    vbulk = tas_getparam( lotrs, TAS_CASE, TP_VDDmax );
  else
    vbulk = 0.0 ;

  trsmcc = mcc_create_trans_mcc( lotrs, vdd, TAS_CASE, NULL, 0.0, vbulk );
  elpLotrsGetShrinkDim( lotrs, &l, &w, NULL, NULL, NULL, NULL, NULL, NULL, TAS_CASE );
  trsmcc->TRWIDTH  = w;
  trsmcc->TRLENGTH = l;
  
  trsspi = mcc_create_trans_spice( lotrs, vdd, NULL, 0.0, 0.0 );
  
  if (SIMUINV_PREFIX==NULL)
    sprintf( bname, "static_%c_w=%ld_l=%ld",
                  MLO_IS_TRANSN(lotrs->TYPE) ? 'n' : 'p',
                  lotrs->WIDTH,
                  lotrs->LENGTH
         );
  else
    sprintf( bname, "%s.%s.static_%c_w=%ld_l=%ld", SIMUINV_PREFIX,env_SIMUINV=='T'?"tas":"mcc",
                  MLO_IS_TRANSN(lotrs->TYPE) ? 'n' : 'p',
                  lotrs->WIDTH,
                  lotrs->LENGTH
         );

  sprintf( fname, "%s_ids=f(vds)", bname );

  ptf = mbkfopen( fname, "dat", WRITE_TEXT );
  if( !ptf )
    EXIT(1);

  ni = 1000;
  nj =    4;
  vt = trsmcc->VT ;

  for( j=1 ; j<=nj ; j++ ) {

    vj = ((float)j)/((float)nj) * (vgsmax-vt)+vt ;
    fprintf( ptf, "# vgs = %g\n\n", vj );

    fprintf( ptf, "# vds imcc ispi\n\n" );

    for( i=0 ; i<=ni ; i++ ) {
    
      vi = ((float)i)/((float)ni) * vdsmax ;

      if(  MLO_IS_TRANSN(lotrs->TYPE) ) {
        trsmcc->VG = vs+vj ;
        //trsmcc->VB = 0.0 ;
        mcc_mcc_ids( trsmcc, vs, vs+vi, &imcc );
        mcc_spice_ids( trsspi, vj, vbs, vi, &ispi );
      }
      else {
        trsmcc->VG = vs-vj ;
        //trsmcc->VB = vdd ;
        mcc_mcc_ids( trsmcc, vs, vs-vi, &imcc );
        mcc_spice_ids( trsspi, -vj, vbs, -vi, &ispi );
      }

      fprintf( ptf, "%g %g %g\n", vi, imcc, ispi );
    }
    fprintf( ptf, "\n" );
  }

  fclose( ptf );
  
  sprintf( fname, "%s_ids=f(vgs)", bname );

  ptf = mbkfopen( fname, "dat", WRITE_TEXT );
  if( !ptf )
    EXIT(1);

  ni = 1000;
  nj =    4;

  for( j=1 ; j<=nj ; j++ ) {
  
    vj = ((float)j)/((float)nj) * vdsmax ;
    fprintf( ptf, "# vds = %g\n\n", vj );

    fprintf( ptf, "# vgs imcc ispi\n\n" );

    for( i=0 ; i<=ni ; i++ ) {
    
      vi = ((float)i)/((float)ni)*vgsmax ;

      if(  MLO_IS_TRANSN(lotrs->TYPE) ) {
        trsmcc->VG = vs+vi ;
        mcc_mcc_ids( trsmcc, vs, vs+vj, &imcc );
        mcc_spice_ids( trsspi, vi, vbs, vj, &ispi );
      }
      else {
        trsmcc->VG = vs-vi ;
        mcc_mcc_ids( trsmcc, vs, vs-vj, &imcc );
        mcc_spice_ids( trsspi, -vi, vbs, -vj, &ispi );
      }

      fprintf( ptf, "%g %g %g\n", vi, imcc, ispi );
    }
    fprintf( ptf, "\n" );
  }

  fclose( ptf );

  ptf = mbkfopen( bname, "plt", WRITE_TEXT );

  fprintf( ptf, "set grid\n");
  fprintf( ptf, "set title \"%s\"\n",MLO_IS_TRANSN(lotrs->TYPE)?"NMOS":"PMOS");
  fprintf( ptf, "plot '%s_ids=f(vds).dat' using 1:2 title \"TAS\" with lines, '%s_ids=f(vds).dat' using 1:3 title \"MCC\" with lines\n", bname, bname );
  fprintf( ptf, "pause -1\n" );
  fprintf( ptf, "plot '%s_ids=f(vgs).dat' using 1:2 title \"TAS\" with lines, '%s_ids=f(vgs).dat' using 1:3 title \"MCC\" with lines\n", bname, bname );
  fprintf( ptf, "pause -1\n" );

  fclose( ptf );

}

void tas_tpiv_set_vg_for_switch( stm_solver_maillon *maillon, char brtype, char typeactive, float vgactive, float vgswitch )
{
  chain_list *chtrs ;
  mcc_trans_mcc *trsmcc ;
  mcc_trans_spice *trsspi ;
  
  for( chtrs = (chain_list*)maillon->MODEL_VDS ; chtrs ; chtrs = chtrs->NEXT ) {
    if( brtype == TAS_TRMODEL_MCCRSAT ) {
      trsmcc = (mcc_trans_mcc*)chtrs->DATA ;
      if( ( typeactive == 'N' && MLO_IS_TRANSN(trsmcc->TYPE) ) || ( typeactive == 'P' && MLO_IS_TRANSP(trsmcc->TYPE) ) )
        trsmcc->VG = vgactive ;
      else
        trsmcc->VG = vgswitch ;
    }
    else {
      trsspi = (mcc_trans_spice*)chtrs->DATA ;
      if( ( typeactive == 'N' && trsspi->TRANSTYPE == MCC_NMOS ) || ( typeactive == 'P' && trsspi->TRANSTYPE == MCC_PMOS ) )
        trsspi->VG = vgactive ;
      else
        trsspi->VG = vgswitch ;
    }
  }

}

void tas_tpiv_set_vg( stm_solver_maillon *maillon, char brtype, float vg )
{
  chain_list *chtrs ;
  mcc_trans_mcc *trsmcc ;
  mcc_trans_spice *trsspi ;
  
  for( chtrs = (chain_list*)maillon->MODEL_VDS ; chtrs ; chtrs = chtrs->NEXT ) {
    if( brtype == TAS_TRMODEL_MCCRSAT ) {
      trsmcc = (mcc_trans_mcc*)chtrs->DATA ;
      trsmcc->VG = vg ;
    }
    else {
      trsspi = (mcc_trans_spice*)chtrs->DATA ;
      trsspi->VG = vg ;
    }
  }
}

void tas_tpiv_set_vb( stm_solver_maillon *maillon, char brtype, float vb )
{
  chain_list *chtrs ;
  mcc_trans_mcc *trsmcc ;
  mcc_trans_spice *trsspi ;
  
  for( chtrs = (chain_list*)maillon->MODEL_VDS ; chtrs ; chtrs = chtrs->NEXT ) {
    if( brtype == TAS_TRMODEL_MCCRSAT ) {
      trsmcc = (mcc_trans_mcc*)chtrs->DATA ;
      trsmcc->VB = vb ;
    }
    else {
      trsspi = (mcc_trans_spice*)chtrs->DATA ;
      trsspi->VB = vb ;
    }
  }
}

char tas_tpiv_solver_calc_ids( stm_solver_maillon *maillon, float vgs, float vbs, float vds, float *ids, char brtype )
{
  tas_tpiv_set_vg( maillon, brtype, vgs );
  tas_tpiv_set_vb( maillon, brtype, vbs );
  return stm_solver_calc_ids( maillon, 0.0, vds, ids );
}
