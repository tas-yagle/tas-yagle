/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_optim.c                                                 */
/*                                                                          */
/*    (c) copyright 1991-2003 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc KUOCH                                                */
/*                                                                          */
/****************************************************************************/

#include "mcc_optim.h"
#include "mcc_util.h"
#include "mcc_genspi.h"

/******************************************************************************\
 GLOBALS
\******************************************************************************/
mbk_pwl *MCC_OPTIM_IDNSAT = NULL;
mbk_pwl *MCC_OPTIM_IDNRES = NULL;
mbk_pwl *MCC_OPTIM_IDNVGS = NULL;
mbk_pwl *MCC_OPTIM_IDPSAT = NULL;
mbk_pwl *MCC_OPTIM_IDPRES = NULL;
mbk_pwl *MCC_OPTIM_IDPVGS = NULL;
sim_model *MCC_SIM_MODEL_FITA   = NULL;
sim_model *MCC_SIM_MODEL_FITCG  = NULL;
sim_model *MCC_SIM_MODEL_FITCGNOCAPA  = NULL;
sim_model *MCC_SIM_MODEL_FITCDN = NULL;
sim_model *MCC_SIM_MODEL_FITCDP = NULL;

/******************************************************************************\
 Courant spice
\******************************************************************************/
char mcc_optim_spice_ids ( mcc_trans_spice *trs, 
                           double vgs, 
                           double vbs, 
                           double vds, 
                           double *ids 
                         )
{
  *ids = mcc_calcIDS( trs->MODELFILE,
                      trs->TRANSNAME,
                      trs->TRANSTYPE,
                      trs->TRANSCASE,
                      vbs,
                      vgs,
                      vds,
                      trs->TRLENGTH,
                      trs->TRWIDTH,
                      trs->TEMP,
                      trs->PARAM
                    );
  return 1;
}

/****************************************************************************\
 Function : mcc_ids_vgs
\****************************************************************************/
char mcc_spice_ids_vgs ( mcc_optim_ids *optim_ids_cond , double vgs , double *ids)
{
  if( optim_ids_cond->TRS->TRANSTYPE == MCC_TRANS_P )
    vgs = -vgs ;
    
  mcc_optim_spice_ids ( optim_ids_cond->TRS,
                        vgs,
                        optim_ids_cond->VBS,
                        optim_ids_cond->VDS,
                        ids);
  return 1;
}

/****************************************************************************\
 Function : mcc_ids_vds
\****************************************************************************/
char mcc_spice_ids_vds ( mcc_optim_ids *optim_ids_cond, double vds , double *ids)
{
  if( optim_ids_cond->TRS->TRANSTYPE ==MCC_TRANS_P )
    vds = -vds ;
    
  mcc_optim_spice_ids (optim_ids_cond->TRS,
                       optim_ids_cond->VGS,
                       optim_ids_cond->VBS,
                       vds,
                       ids);
  return 1;
}

/****************************************************************************\
 Function : mcc_ids_vbs
\****************************************************************************/
char mcc_spice_ids_vbs ( mcc_optim_ids *optim_ids_cond, double vbs , double *ids)
{
  mcc_optim_spice_ids (optim_ids_cond->TRS,
                       optim_ids_cond->VGS,
                       vbs,
                       optim_ids_cond->VDS,
                       ids);
  return 1;
}

/****************************************************************************\
 * Function : mcc_optim_addspidata
\****************************************************************************/
void mcc_optim_addspidata(int type, elp_lotrs_param *lotrsparam_n,
                          elp_lotrs_param *lotrsparam_p)
{
  double vdddeg ;
  double vssdeg ;
  int tabsize,i ;
  mcc_trans_spice tr_nmos;
  mcc_trans_spice tr_pmos;
  mcc_optim_ids nmos_cond;
  mcc_optim_ids pmos_cond;
  mbk_pwl_param *param;
  double ids,step,k_vdd,half_vdd,k2_vdd;
  int residx,satidx,vgsidx;
 
  mcc_optim_resetsearch_for_new_pwl();

  tabsize = mcc_ftoi(MCC_VDDmax / MCC_DC_STEP) ;
  tabsize++ ;
 
  half_vdd = MCC_VDDmax/2.0 ;
  k_vdd = 3.0*MCC_VDDmax/4.0 ;
  k2_vdd = MCC_VDDmax/6.0 ;
  if((type == MCC_TRANS_N) || (type == MCC_TRANS_B)) {   
    vdddeg =  mcc_spicevdeg(MCC_TRANS_N,lotrsparam_n) ;
    //=> Create mcc_trans_spice
    tr_nmos.MODELFILE  = MCC_MODELFILE;
    tr_nmos.MODELTYPE  = mcc_getmodeltype (MCC_MODELFILE);
    tr_nmos.TRANSNAME  = MCC_TNMODEL;
    tr_nmos.TRANSTYPE  = MCC_NMOS;
    tr_nmos.TRANSCASE  = MCC_NCASE;
    tr_nmos.TRLENGTH   = MCC_LN*1.0e-6;
    tr_nmos.TRWIDTH    = MCC_WN*1.0e-6;
    tr_nmos.AD         = MCC_WN*1.0e-6*MCC_DIF*1.0e-6;
    tr_nmos.PD         = 2.0*(MCC_WN*1.0e-6*MCC_DIF*1.0e-6);
    tr_nmos.AS         = tr_nmos.AD;
    tr_nmos.PS         = tr_nmos.PD;
    tr_nmos.TEMP       = MCC_TEMP;
    tr_nmos.VDD        = MCC_VDDmax;
    tr_nmos.PARAM      = lotrsparam_n;
    
    //=> Create nmos simu condition
    nmos_cond.TRS = &tr_nmos;
    nmos_cond.VGS = MCC_VDDmax;
    nmos_cond.VDS = MCC_VDDmax;
    nmos_cond.VBS = lotrsparam_n->VBULK;
 
  /*----------------------------------------*\
    compute NMOS current by approximation
  \*----------------------------------------*/
    param = mbk_pwl_get_default_param ( (char (*)(void*, double, double*))mcc_spice_ids_vgs,
                                        (void*) &nmos_cond,
                                        0.0,
                                        MCC_VDDmax
                                      );
    param->PARAM.DEFAULT.DELTAXMIN = MCC_VDDmax*MCC_DC_STEP;
    // NB : le delta y par defaut tolere une erreur de 2% de Imax, on la passe a 1%
    param->PARAM.DEFAULT.DELTAYMAX /= V_FLOAT_TAB[ __AVT_OPTIM_STEP ].VALUE ;
    mbk_pwl_add_param_point( param, half_vdd );
    mbk_pwl_add_param_point( param, k_vdd );
    mbk_pwl_add_param_point( param, k2_vdd );
    mbk_pwl_add_param_point( param, k_vdd*(MCC_VGS-MCC_VTN)/(MCC_VDDmax-MCC_VTN) );
    mbk_pwl_add_param_point( param, k2_vdd*(MCC_VGS-MCC_VTN)/(MCC_VDDmax-MCC_VTN) );
    mbk_pwl_add_param_point( param, MCC_VTN );
    mbk_pwl_add_param_point( param, MCC_VDDmax );
    MCC_OPTIM_IDNRES = mbk_pwl_create( (char (*)(void*, double, double*))mcc_spice_ids_vds,
                                   (void*) &nmos_cond,
                                   0.0,
                                   MCC_VDDmax,
                                   param
                                   );
    if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)) {
      nmos_cond.VGS = MCC_VGS;
      MCC_OPTIM_IDNVGS = mbk_pwl_create( (char (*)(void*, double, double*))mcc_spice_ids_vds,
                                     (void*) &nmos_cond,
                                     0.0,
                                     MCC_VDDmax,
                                     param
                                     );
      nmos_cond.VGS = MCC_VDDmax;
    }
    else MCC_OPTIM_IDNVGS=NULL;
    MCC_OPTIM_IDNSAT = mbk_pwl_create( (char (*)(void*, double, double*))mcc_spice_ids_vgs,
                                   (void*) &nmos_cond,
                                   0.0,
                                   MCC_VDDmax,
                                   param
                                   );
    mbk_pwl_free_param( param );
    if ( MCC_OPTIM_IDNSAT && MCC_OPTIM_IDNRES ) {
#ifndef OPTIM5               
      step = 0.0 ;
      residx = satidx = vgsidx = -1;
      for(i = 0 ; i < tabsize ; i ++) {
        MCC_VDDDEG[i] = vdddeg ;
        mbk_pwl_get_value_bytabindex (MCC_OPTIM_IDNSAT,step,&ids,&satidx);
        MCC_IDNSAT[i] = ids;
        mbk_pwl_get_value_bytabindex (MCC_OPTIM_IDNRES,step,&ids,&residx);
        MCC_IDNRES[i] = ids;
        if ( MCC_OPTIM_IDNVGS ) {
          mbk_pwl_get_value_bytabindex (MCC_OPTIM_IDNVGS,step,&ids,&vgsidx);
          MCC_IDNVGS[i] = ids;
        }
        step += MCC_DC_STEP ;
      }
#else
//      MCC_IDNSAT=MCC_IDNRES=MCC_IDNVGS=NULL; //(void *)1; // bus error
      MCC_VDDDEG[1]=vdddeg;
#endif
    }
    else
       avt_errmsg(MCC_ERRMSG, "035", AVT_FATAL, MCC_TNMODEL, MCC_VDDmax, MCC_TEMP);
  }
  if((type == MCC_TRANS_P) || (type == MCC_TRANS_B)) {   
    vssdeg =  mcc_spicevdeg(MCC_TRANS_P,lotrsparam_p) ;
    //=> Create mcc_trans_spice
    tr_pmos.MODELFILE  = MCC_MODELFILE;
    tr_pmos.MODELTYPE  = mcc_getmodeltype (MCC_MODELFILE);
    tr_pmos.TRANSNAME  = MCC_TPMODEL;
    tr_pmos.TRANSTYPE  = MCC_PMOS;
    tr_pmos.TRANSCASE  = MCC_PCASE;
    tr_pmos.TRLENGTH   = MCC_LP*1.0e-6;
    tr_pmos.TRWIDTH    = MCC_WP*1.0e-6;
    tr_pmos.AD         = MCC_WP*1.0e-6*MCC_DIF*1.0e-6;
    tr_pmos.PD         = 2.0*(MCC_WN*1.0e-6*MCC_DIF*1.0e-6);
    tr_pmos.AS         = tr_pmos.AD;
    tr_pmos.PS         = tr_pmos.PD;
    tr_pmos.TEMP       = MCC_TEMP;
    tr_pmos.VDD        = MCC_VDDmax;
    tr_pmos.PARAM      = lotrsparam_p;

    //=> Create pmos simu condition
    pmos_cond.TRS = &tr_pmos;
    pmos_cond.VGS = -MCC_VDDmax;
    pmos_cond.VDS = -MCC_VDDmax;
    pmos_cond.VBS = lotrsparam_p->VBULK-MCC_VDDmax;
 
  /*----------------------------------------*\
    compute PMOS current by approximation
  \*----------------------------------------*/
    param = mbk_pwl_get_default_param ( (char (*)(void*, double, double*))mcc_spice_ids_vgs,
                                        (void*) &pmos_cond,
                                        0.0,
                                        MCC_VDDmax
                                      );
    param->PARAM.DEFAULT.DELTAXMIN = MCC_VDDmax*MCC_DC_STEP;
    // NB : le delta y par defaut tolere une erreur de 2% de Imax , on la passe a 1%
    param->PARAM.DEFAULT.DELTAYMAX /= V_FLOAT_TAB[ __AVT_OPTIM_STEP ].VALUE ;
    mbk_pwl_add_param_point( param, half_vdd );
    mbk_pwl_add_param_point( param, k_vdd );
    mbk_pwl_add_param_point( param, k2_vdd );
    mbk_pwl_add_param_point( param, k_vdd*(MCC_VGS-MCC_VTP)/(MCC_VDDmax-MCC_VTP) );
    mbk_pwl_add_param_point( param, k2_vdd*(MCC_VGS-MCC_VTP)/(MCC_VDDmax-MCC_VTP) );
    mbk_pwl_add_param_point( param, MCC_VTP );
    mbk_pwl_add_param_point( param, MCC_VDDmax );
    MCC_OPTIM_IDPRES = mbk_pwl_create( (char (*)(void*, double, double*))mcc_spice_ids_vds,
                                   (void*) &pmos_cond,
                                   0.0,
                                   MCC_VDDmax,
                                   param
                                   );
    if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0)) {
      pmos_cond.VGS = MCC_VGS;
      MCC_OPTIM_IDPVGS = mbk_pwl_create( (char (*)(void*, double, double*))mcc_spice_ids_vds,
                                     (void*) &pmos_cond,
                                     0.0,
                                     MCC_VDDmax,
                                     param
                                     );
      pmos_cond.VGS = MCC_VDDmax;
    }
    else MCC_OPTIM_IDPVGS=NULL;
    MCC_OPTIM_IDPSAT = mbk_pwl_create( (char (*)(void*, double, double*))mcc_spice_ids_vgs,
                                   (void*) &pmos_cond,
                                   0.0,
                                   MCC_VDDmax,
                                   param
                                   );
    mbk_pwl_free_param( param );

    if ( MCC_OPTIM_IDPSAT && MCC_OPTIM_IDPRES ) {
#ifndef OPTIM5               
      step = 0.0 ;
      residx = satidx = vgsidx = -1;
      for(i = 0 ; i < tabsize ; i ++) {
        MCC_VSSDEG[i] = vssdeg ;
        mbk_pwl_get_value_bytabindex (MCC_OPTIM_IDPSAT,step,&ids,&satidx);
        MCC_IDPSAT[i] = fabs (ids);
        mbk_pwl_get_value_bytabindex (MCC_OPTIM_IDPRES,step,&ids,&residx);
        MCC_IDPRES[i] = fabs (ids);
        if ( MCC_OPTIM_IDPVGS ) {
          mbk_pwl_get_value_bytabindex (MCC_OPTIM_IDPVGS,step,&ids,&vgsidx);
          MCC_IDPVGS[i] = fabs (ids);
        }
        step += MCC_DC_STEP ;
      }
#else
//      MCC_IDPSAT=MCC_IDPRES=MCC_IDPVGS=NULL; //(void *)1; // bus error
      MCC_VSSDEG[1]=vssdeg;
#endif
    }
    else
       avt_errmsg(MCC_ERRMSG, "035", AVT_FATAL, MCC_TPMODEL, MCC_VDDmax, MCC_TEMP);
  }
  MCC_CALC_CUR = MCC_SIM_MODE ;
}

/****************************************************************************\
 * Function : mcc_add_print_meas
\****************************************************************************/
void mcc_add_print_meas (FILE *file, char *filename) 
{
  sim_model *model;
  double sfl_ud,sfl_du;
  double sfh_ud,sfh_du;
  int titan=0;

  if (V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN || V_INT_TAB[__SIM_TOOL].VALUE == SIM_TOOL_TITAN7)
     titan=1;

  if ( !file || !filename )
    return;
  // Positionnement des seuils de mesure des fronts
  // NB : on divise par VDDmax car ce sont des ratios
  sfl_ud = (MCC_VTN / MCC_VDDmax);
  sfl_du = (MCC_VDDmax - MCC_VTP)/(4.0*MCC_VDDmax);
  sfh_ud = (3.0*MCC_VDDmax+MCC_VTN)/(4.0*MCC_VDDmax);
  sfh_du = (MCC_VDDmax-MCC_VTP)/MCC_VDDmax;

  if ( filename == MCC_SPICEFILE[0] ) {
    /*------------------------------*\
    | fit_a_sim                      |
    \*------------------------------*/

    MCC_SIM_MODEL_FITA = sim_model_create ("fit_a_sim",0);
    model = MCC_SIM_MODEL_FITA;

    sim_parameter_set_slopeVTHL (MCC_SIM_MODEL_FITA,sfl_ud);
    sim_parameter_set_slopeVTHH (MCC_SIM_MODEL_FITA,sfh_ud);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_UD_FIT_A_LABEL,
                                 MCC_SLOPE_UD_FIT_A_LABEL,
                                 MCC_UD_SIG1,
                                 MCC_UD_SIG2, 
                                 titan ? "U1D1" : "UD",
                                 SIM_MAX
                                 );
    sim_parameter_set_slopeVTHL (MCC_SIM_MODEL_FITA,sfl_du);
    sim_parameter_set_slopeVTHH (MCC_SIM_MODEL_FITA,sfh_du);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_DU_FIT_A_LABEL,
                                 MCC_SLOPE_DU_FIT_A_LABEL,
                                 MCC_DU_SIG1,
                                 MCC_DU_SIG2, 
                                 titan ? "D1U1" : "DU",
                                 SIM_MAX
                                 );
  }
  else if ( filename == MCC_SPICEFILE[1] ) {
    /*------------------------------*\
    | fit_cg_sim                     |
    \*------------------------------*/
    MCC_SIM_MODEL_FITCG = sim_model_create ("fit_cg_sim",0);
    model = MCC_SIM_MODEL_FITCG;

    sim_parameter_set_slopeVTHL (model,sfl_ud);
    sim_parameter_set_slopeVTHH (model,sfh_ud);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_UD_FIT_CG_LABEL,
                                 MCC_SLOPE_UD_FIT_CG_LABEL,
                                 MCC_UD_SIG1,
                                 MCC_UD_SIG2, 
                                 titan ? "U1D1" : "UD",
                                 SIM_MAX
                                 );
    sim_parameter_set_slopeVTHL (model,sfl_du);
    sim_parameter_set_slopeVTHH (model,sfh_du);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_DU_FIT_CG_LABEL,
                                 MCC_SLOPE_DU_FIT_CG_LABEL,
                                 MCC_DU_SIG1,
                                 MCC_DU_SIG2, 
                                 titan ? "D1U1" : "DU",
                                 SIM_MAX
                                 );
  }
  else if ( filename == MCC_SPICEFILE[2] ) {
    /*------------------------------*\
    | fit_cg_sim                     |
    \*------------------------------*/
    MCC_SIM_MODEL_FITCGNOCAPA = sim_model_create ("fit_cg_nocapa_sim",0);
    model = MCC_SIM_MODEL_FITCGNOCAPA;

    sim_parameter_set_slopeVTHL (model,sfl_ud);
    sim_parameter_set_slopeVTHH (model,sfh_ud);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_UD_FIT_CGNOCAPA_LABEL,
                                 MCC_SLOPE_UD_FIT_CGNOCAPA_LABEL,
                                 MCC_UD_SIG1,
                                 MCC_UD_SIG2, 
                                 titan ? "U1D1" : "UD",
                                 SIM_MAX
                                 );
    sim_parameter_set_slopeVTHL (model,sfl_du);
    sim_parameter_set_slopeVTHH (model,sfh_du);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_DU_FIT_CGNOCAPA_LABEL,
                                 MCC_SLOPE_DU_FIT_CGNOCAPA_LABEL,
                                 MCC_DU_SIG1,
                                 MCC_DU_SIG2, 
                                 titan ? "D1U1" : "DU",
                                 SIM_MAX
                                 );
  }
  else if ( filename == MCC_SPICEFILE[3] ) {
    /*------------------------------*\
    | fit_cdn_sim                    |
    \*------------------------------*/
    MCC_SIM_MODEL_FITCDN = sim_model_create ("fit_cdn_sim",0);
    model = MCC_SIM_MODEL_FITCDN;

    sim_parameter_set_slopeVTHL (model,sfl_ud);
    sim_parameter_set_slopeVTHH (model,sfh_ud);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_UD_FIT_CDN_LABEL,
                                 MCC_SLOPE_UD_FIT_CDN_LABEL,
                                 MCC_UD_SIG1,
                                 MCC_UD_SIG2, 
                                 titan ? "U1D1" : "UD",
                                 SIM_MAX
                                 );
    sim_parameter_set_slopeVTHL (model,sfl_du);
    sim_parameter_set_slopeVTHH (model,sfh_du);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_DU_FIT_CDN_LABEL,
                                 MCC_SLOPE_DU_FIT_CDN_LABEL,
                                 MCC_DU_SIG1,
                                 MCC_DU_SIG2, 
                                 titan ? "D1U1" : "DU",
                                 SIM_MAX
                                 );
  }
  else if ( filename == MCC_SPICEFILE[4] ) {
    /*------------------------------*\
    | fit_cdp_sim                    |
    \*------------------------------*/
    MCC_SIM_MODEL_FITCDP = sim_model_create ("fit_cdp_sim",0);
    model = MCC_SIM_MODEL_FITCDP;

    sim_parameter_set_slopeVTHL (model,sfl_ud);
    sim_parameter_set_slopeVTHH (model,sfh_ud);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_UD_FIT_CDP_LABEL,
                                 MCC_SLOPE_UD_FIT_CDP_LABEL,
                                 MCC_UD_SIG1,
                                 MCC_UD_SIG2, 
                                 titan ? "U1D1" : "UD",
                                 SIM_MAX
                                 );
    sim_parameter_set_slopeVTHL (model,sfl_du);
    sim_parameter_set_slopeVTHH (model,sfh_du);
    sim_addMeasWithoutNameCheck (model,
                                 MCC_DELAY_DU_FIT_CDP_LABEL,
                                 MCC_SLOPE_DU_FIT_CDP_LABEL,
                                 MCC_DU_SIG1,
                                 MCC_DU_SIG2, 
                                 titan ? "D1U1" : "DU",
                                 SIM_MAX
                                 );
  }
  sim_printMeasCmd(model, file);
}

/****************************************************************************\
 * Function : mcc_optim_readspidata
\****************************************************************************/
void mcc_optim_readspidata (int fit, elp_lotrs_param *lotrsparam_n,
                                     elp_lotrs_param *lotrsparam_p)
{
  double delay ;
  double slope ;
  double *ptparam[8] ;
  int tabsize ;
  int i, f;
  int valid;
  char *outfile,buf[1024];
  sim_model *model;
 
  MCC_FLAG_FIT = MCC_FIT_OK ;
 
  if ( !V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
    tabsize = mcc_ftoi(MCC_VDDmax / MCC_DC_STEP) ;
    tabsize++ ;
    mcc_allocspidata(MCC_TRANS_B) ;
    i = 0 ;
    ptparam[i++] = MCC_IDNSAT ;
    ptparam[i++] = MCC_IDNRES ;
    if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
      ptparam[i++] = MCC_IDNVGS ;
    ptparam[i++] = MCC_IDPSAT ;
    ptparam[i++] = MCC_IDPRES ;
    if(mcc_ftol(MCC_VGS * 1000.0) != mcc_ftol(MCC_VDDmax * 1000.0))
      ptparam[i++] = MCC_IDPVGS ;
    ptparam[i++] = MCC_VDDDEG ;
    ptparam[i++] = MCC_VSSDEG ;
   
    if(MCC_CALC_CUR == MCC_SIM_MODE)
     mcc_prsspifile(MCC_PARAM,i,tabsize,ptparam) ;
    else {
      if ( MCC_PLOT || !MCC_OPTIM_MODE ) {
        mcc_addspidata(MCC_TRANS_B,lotrsparam_n,lotrsparam_p) ;
        mcc_drvspitab(MCC_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
      }
      if ( MCC_OPTIM_MODE ) {
        mcc_optim_addspidata (MCC_TRANS_B,lotrsparam_n,lotrsparam_p) ;
        if ( !MCC_PLOT )
          mcc_drvspitab(MCC_PARAM,ptparam,i,tabsize,lotrsparam_n,lotrsparam_p) ;
      }
    }
  }
 
  if(fit == 0)
   return ;
 
  /*---------------------------------------------*\
  |  Recupere les donnees des mesures             |
  \*---------------------------------------------*/

  for(f = 0 ; f < MCC_SPICENB - 1 ; f++) {
    switch(f) {
       case MCC_FIT_A : model = MCC_SIM_MODEL_FITA;
                        sim_set_output_file( model, MCC_SPICEFILE[f] );
                        outfile = sim_getjoker(SIM_SPICEOUT,MCC_SPICEFILE[f]);
                        sprintf(buf,"%s/%s",WORK_LIB,outfile);
                        mfe_fileextractlabel(buf,model->LRULES,model->LLABELS,0);

                        valid = sim_getMeas (MCC_SIM_MODEL_FITA, 
                                             MCC_DELAY_UD_FIT_A_LABEL,
                                             MCC_SLOPE_UD_FIT_A_LABEL,
                                             &delay,
                                             &slope
                                             );
                        if ( valid ) {
                          MCC_SPICEUD_FITA  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFUD_FITA = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEUD_FITA = 0;
                          MCC_SPICEFUD_FITA = 0;
                        }
                        valid = sim_getMeas (MCC_SIM_MODEL_FITA, 
                                             MCC_DELAY_DU_FIT_A_LABEL,
                                             MCC_SLOPE_DU_FIT_A_LABEL,
                                             &delay,
                                             &slope
                                             );
                        if ( valid ) {
                          MCC_SPICEDU_FITA  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFDU_FITA = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEDU_FITA = 0;
                          MCC_SPICEFDU_FITA = 0;
                        }
                        if((MCC_SPICEUD_FITA == 0) ||
                           (MCC_SPICEDU_FITA == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
       case MCC_FIT_CG_NOCAPA:
                        model = MCC_SIM_MODEL_FITCGNOCAPA;
                        sim_set_output_file( model, MCC_SPICEFILE[f] );
                        outfile = sim_getjoker(SIM_SPICEOUT,MCC_SPICEFILE[f]);
                        sprintf(buf,"%s/%s",WORK_LIB,outfile);
                        mfe_fileextractlabel(buf,model->LRULES,model->LLABELS,0);

                        valid = sim_getMeas (MCC_SIM_MODEL_FITCG, 
                                              MCC_DELAY_UD_FIT_CG_LABEL,
                                              MCC_SLOPE_UD_FIT_CG_LABEL,
                                              &delay,
                                              &slope
                                              );
                        if ( valid ) {
                          MCC_SPICEUD_FITCGNOCAPA  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFUD_FITCGNOCAPA = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEUD_FITCGNOCAPA = 0;
                          MCC_SPICEFUD_FITCGNOCAPA = 0;
                        }
                        valid = sim_getMeas (MCC_SIM_MODEL_FITCGNOCAPA, 
                                             MCC_DELAY_DU_FIT_CGNOCAPA_LABEL,
                                             MCC_SLOPE_DU_FIT_CGNOCAPA_LABEL,
                                             &delay,
                                             &slope
                                             );
                        if ( valid ) {
                          MCC_SPICEDU_FITCGNOCAPA  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFDU_FITCGNOCAPA = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEDU_FITCGNOCAPA = 0;
                          MCC_SPICEFDU_FITCGNOCAPA = 0;
                        }
                        if((MCC_SPICEUD_FITCGNOCAPA == 0) ||
                           (MCC_SPICEDU_FITCGNOCAPA == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
       case MCC_FIT_CG : model = MCC_SIM_MODEL_FITCG;
                        sim_set_output_file( model, MCC_SPICEFILE[f] );
                        outfile = sim_getjoker(SIM_SPICEOUT,MCC_SPICEFILE[f]);
                        sprintf(buf,"%s/%s",WORK_LIB,outfile);
                        mfe_fileextractlabel(buf,model->LRULES,model->LLABELS,0);

                        valid = sim_getMeas (MCC_SIM_MODEL_FITCG, 
                                              MCC_DELAY_UD_FIT_CG_LABEL,
                                              MCC_SLOPE_UD_FIT_CG_LABEL,
                                              &delay,
                                              &slope
                                              );
                        if ( valid ) {
                          MCC_SPICEUD_FITCG  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFUD_FITCG = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEUD_FITCG = 0;
                          MCC_SPICEFUD_FITCG = 0;
                        }
                        valid = sim_getMeas (MCC_SIM_MODEL_FITCG, 
                                             MCC_DELAY_DU_FIT_CG_LABEL,
                                             MCC_SLOPE_DU_FIT_CG_LABEL,
                                             &delay,
                                             &slope
                                             );
                        if ( valid ) {
                          MCC_SPICEDU_FITCG  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFDU_FITCG = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEDU_FITCG = 0;
                          MCC_SPICEFDU_FITCG = 0;
                        }
                        if((MCC_SPICEUD_FITCG == 0) ||
                           (MCC_SPICEDU_FITCG == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
       case MCC_FIT_CDN : model = MCC_SIM_MODEL_FITCDN;
                        sim_set_output_file( model, MCC_SPICEFILE[f] );
                        outfile = sim_getjoker(SIM_SPICEOUT,MCC_SPICEFILE[f]);
                        sprintf(buf,"%s/%s",WORK_LIB,outfile);
                        mfe_fileextractlabel(buf,model->LRULES,model->LLABELS,0);

                        valid = sim_getMeas (MCC_SIM_MODEL_FITCDN, 
                                               MCC_DELAY_UD_FIT_CDN_LABEL,
                                               MCC_SLOPE_UD_FIT_CDN_LABEL,
                                               &delay,
                                               &slope
                                               );
                        if ( valid ) {
                          MCC_SPICEUD_FITDN  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFUD_FITDN = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEUD_FITDN = 0;
                          MCC_SPICEFUD_FITDN = 0;
                        }
                        valid = sim_getMeas (MCC_SIM_MODEL_FITCDN, 
                                             MCC_DELAY_DU_FIT_CDN_LABEL,
                                             MCC_SLOPE_DU_FIT_CDN_LABEL,
                                             &delay,
                                             &slope
                                             );
                        if ( valid ) {
                          MCC_SPICEDU_FITDN  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFDU_FITDN = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEDU_FITDN = 0;
                          MCC_SPICEFDU_FITDN = 0;
                        }
                        if((MCC_SPICEUD_FITDN == 0) ||
                           (MCC_SPICEDU_FITDN == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
       case MCC_FIT_CDP : model = MCC_SIM_MODEL_FITCDP;
                        sim_set_output_file( model, MCC_SPICEFILE[f] );
                        outfile = sim_getjoker(SIM_SPICEOUT,MCC_SPICEFILE[f]);
                        sprintf(buf,"%s/%s",WORK_LIB,outfile);
                        mfe_fileextractlabel(buf,model->LRULES,model->LLABELS,0);

                        valid = sim_getMeas (MCC_SIM_MODEL_FITCDP, 
                                               MCC_DELAY_UD_FIT_CDP_LABEL,
                                               MCC_SLOPE_UD_FIT_CDP_LABEL,
                                               &delay,
                                               &slope
                                               );
                        if ( valid ) {
                          MCC_SPICEUD_FITDP  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFUD_FITDP = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEUD_FITDP = 0;
                          MCC_SPICEFUD_FITDP = 0;
                        }
                        valid = sim_getMeas (MCC_SIM_MODEL_FITCDP, 
                                             MCC_DELAY_DU_FIT_CDP_LABEL,
                                             MCC_SLOPE_DU_FIT_CDP_LABEL,
                                             &delay,
                                             &slope
                                             );
                        if ( valid ) {
                          MCC_SPICEDU_FITDP  = mcc_ftol (delay*1.0e12);
                          MCC_SPICEFDU_FITDP = mcc_ftol (slope*1.0e12);
                        }
                        else {
                          MCC_SPICEDU_FITDP = 0;
                          MCC_SPICEFDU_FITDP = 0;
                        }
                        if((MCC_SPICEUD_FITDP == 0) ||
                           (MCC_SPICEDU_FITDP == 0))
                            MCC_FLAG_FIT = MCC_FIT_KO ;
                        break ;
    }
  }
  sim_model_clear_bypointer ( MCC_SIM_MODEL_FITA   );
  sim_model_clear_bypointer ( MCC_SIM_MODEL_FITCG  );
  sim_model_clear_bypointer ( MCC_SIM_MODEL_FITCDN );
  sim_model_clear_bypointer ( MCC_SIM_MODEL_FITCDP );
}

/****************************************************************************\
 * Function : mcc_optim_calcdiffmodelsat
\****************************************************************************/
double mcc_optim_calcdiffmodelsat (int type, mbk_pwl *optim_ids, double exitvalue, mcc_corner_info *info)
{
  double var,var2 ;
  double res = 0.0 ;
  double imod,imod2 ;
  double ispice,ispice2 ;
  double val ;
  double vmin ;
  int cont = 0 ;
  int nbdroite,i;
  int tpos ;
 
  if( V_BOOL_TAB[ __MCC_NEW_DIFFMODELSAT ].VALUE )
    return mcc_optim_calcdiffmodelsat_new( type, optim_ids, exitvalue, info );
    
  if( V_BOOL_TAB[ __MCC_FIT_VT_THRESHOLD ].VALUE && info) {
    vmin = info->vgsmin ;
  }
  else {
    vmin = -1.0 ;
  }
 
  nbdroite = optim_ids->N;
 
  for ( i = 0 ; i < nbdroite  ; i++ ) {
    var = optim_ids->DATA[i].X0;
    if( var < vmin )
      continue ;

    if ( ( i+1 ) < nbdroite )
      var2 = ( optim_ids->DATA[i].X0 + optim_ids->DATA[i+1].X0 ) /2.0 ;
    else 
      var2 = ( optim_ids->DATA[i].X0 + optim_ids->X1 ) /2.0 ;
    imod = mcc_current(var,MCC_VDDmax,0.0,type) ;
    tpos = i ;
    mbk_pwl_get_value_bytabindex(optim_ids,var,&ispice,&tpos);
    ispice = fabs (ispice);
    if(ispice == 0.0)
      continue ;
    val = (imod - ispice) * (imod - ispice) ;
    if(val > 1.0)
      continue ;
    res += val ;
    cont ++ ;
    if (exitvalue>=0 && res>exitvalue) return res;
#ifndef OPTIM4
    imod2 = mcc_current(var2,MCC_VDDmax,0.0,type) ;
    mbk_pwl_get_value_bytabindex(optim_ids,var2,&ispice2,&tpos);
    ispice2 = fabs (ispice2);
    val = (imod2 - ispice2) * (imod2 - ispice2) ;
    if(val > 1.0)
      continue ;
    res += val ;
    if (exitvalue>=0 && res>exitvalue) return res;
    cont ++ ;
#endif
  }
  var = optim_ids->X1;
  imod = mcc_current(var,MCC_VDDmax,0.0,type) ;
  mbk_pwl_get_value_bytabindex (optim_ids,var,&ispice,&tpos);
  ispice = fabs (ispice);
  val = (imod - ispice) * (imod - ispice) ;
  if(val <= 1.0) {
    res += val ;
    cont ++ ;
  }
  return(res
#ifndef OPTIM0                  
                  /(double)cont
#endif                  
                  ) ;
}

double prim_dte( int type, mbk_pwl_data *p, double x )
{
  double i ;
  double a ;
  double b ;
 
  if( type == MCC_TRANS_N ) {
    a = p->A ;
    b = p->B ;
  }
  else {
    a = -p->A ;
    b = -p->B ;
  }
    
  i = 0.5*a*x*x + b*x ;

  return i ;
}

double prim_amj( int type, double vgs )
{
  double i ;
  double a, b, vt ;
  double x ;

  if( type == MCC_TRANS_N ) {
    a = MCC_AN*MCC_WNeff/MCC_LNeff ;
    b = MCC_BN ;
    vt = MCC_VTN ;
  }
  else {
    a = MCC_AP*MCC_WPeff/MCC_LPeff ;
    b = MCC_BP ;
    vt = MCC_VTP ;
  }

  x = vgs-vt ;

  i = a*x*x/(2.0*b) - a*x/(b*b) + a*log(1.0+b*x)/(b*b*b);
  
  return i ;
}

double integ_dte( int type, mbk_pwl_data *p, double vgs1, double vgs2 )
{
  double p1, p2, i ;

  p1 = prim_dte( type, p, vgs1 );
  p2 = prim_dte( type, p, vgs2 );

  i = p2 - p1 ;

  return i ;
}

double integ_amj( int type, double vgs1, double vgs2 )
{
  double p1, p2, i ;

  p1 = prim_amj( type, vgs1 );
  p2 = prim_amj( type, vgs2 );

  i = p2 - p1 ;

  return i ;
}

typedef struct {
  double vgs0 ;
  double vgs1 ;
}  find_cross_data ;

void find_cross( mbk_pwl_data *line, int type, double vgs0, double vgs1, find_cross_data *cross, int *nbc )
{
  double a ;
  double b ;
  double vt ;
  double m ;
  double n ;
  double t1 ;
  double t2 ;
  double t3 ;
  double delta ;
  double v0 ;
  double v1 ;
  
  if( type == MCC_TRANS_N ) {
    a = MCC_AN ;
    b = MCC_BN ;
    vt = MCC_VTN ;
    m = line->A ;
    n = line->B ;
  }
  else {
    a = MCC_AP ;
    b = MCC_BP ;
    vt = MCC_VTP ;
    m = -line->A ;
    n = -line->B ;
  }
  

  t1 = a - m*b ;
  t2 = -2.0*a*vt - m + m*b*vt - n*b ;
  t3 = a*vt*vt - n + n*b*vt ;

  delta = t2*t2-4.0*t1*t3 ;

  if( delta < 0 ) {
  
    cross[0].vgs0 = vgs0 ;
    cross[0].vgs1 = vgs1 ;
    *nbc = 1 ;
    
  }
  else {
  
    v0 = (-t2-sqrt(delta))/(2.0*t1) ;
    v1 = (-t2+sqrt(delta))/(2.0*t1) ;

    if( v0 > v1 ) {
    
      t1 = v0 ;
      v0 = v1 ;
      v1 = t1 ;
      
    }

    if( v0 < vgs0 || v0 > vgs1 )
      v0 = -1.0 ;
      
    if( v1 < vgs0 || v1 > vgs1 )
      v1 = -1.0 ;

    if( v0 > 0.0 && v1 > 0.0 ) {
      *nbc = 3 ;
      cross[0].vgs0 = vgs0 ;
      cross[0].vgs1 = v0 ;
      cross[1].vgs0 = v0 ;
      cross[1].vgs1 = v1 ;
      cross[2].vgs0 = v1 ;
      cross[2].vgs1 = vgs1 ;
    }
    else {
      t1 = -1.0 ;
      if( v0 > 0.0 ) t1 = v0 ;
      if( v1 > 0.0 ) t1 = v1 ;

      if( t1 > 0 ) {
        *nbc = 2 ;
        cross[0].vgs0 = vgs0 ;
        cross[0].vgs1 = t1 ;
        cross[1].vgs0 = t1 ;
        cross[1].vgs1 = vgs1 ;
      }
      else {
        *nbc = 1 ;
        cross[0].vgs0 = vgs0 ;
        cross[0].vgs1 = vgs1 ;
      }
    }
  }
}

double mcc_optim_calcdiffmodelsat_new(int type, mbk_pwl *pwl, double exitvalue, mcc_corner_info *info)
{
  double          stot ;
  double          sdte ;
  double          samj ;
  int             i ;
  int             j ;
  double          vgs0 ;
  double          vgs1 ;
  double          vt ;
  int             n ;
  int             ni ;
  find_cross_data tabvgs[3] ;
  static int      fordebug = 0 ;

  fordebug++ ;

  if( type == MCC_TRANS_N ) 
    vt = MCC_VTN ;
  else
    vt = MCC_VTP ;

  stot = 0.0 ;

  for( i=0 ; i<pwl->N ; i++ ) {
    ni = i+1 ;
    
    vgs0 = pwl->DATA[i].X0;
    
    if( ni < pwl->N ) 
      vgs1 = pwl->DATA[ni].X0;
    else
      vgs1 = pwl->X1;

    if(  V_BOOL_TAB[ __MCC_FIT_VT_THRESHOLD ].VALUE && info ) {
      if( vgs1 <= info->vgsmin )
        continue ;

      if( info->vgsmin > vgs0 )
        vgs0 = info->vgsmin ;
    }
    
    if( vt > vgs1 ) {

      sdte = integ_dte( type, &(pwl->DATA[i]), vgs0, vgs1 );
      stot = stot + sdte;

    }
    else {
    
      if( vt > vgs0 ) {
        sdte = integ_dte( type, &(pwl->DATA[i]), vgs0, vt );
        stot = stot + sdte ;
        vgs0 = vt ;
      }

      find_cross( &(pwl->DATA[i]), type, vgs0, vgs1, tabvgs, &n ) ;

      for( j=0 ; j<n ; j++ ) {
        sdte = integ_dte( type, &(pwl->DATA[i]), tabvgs[j].vgs0, tabvgs[j].vgs1 );
        samj = integ_amj( type, tabvgs[j].vgs0, tabvgs[j].vgs1 );
        stot = stot + fabs( sdte - samj );
      }
    }

    if( exitvalue > 0 && stot > exitvalue )
      break ;
  }

  return stot ;
}
