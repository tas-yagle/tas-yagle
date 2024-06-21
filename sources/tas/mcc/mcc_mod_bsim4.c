/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_bsim4                                                 */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2003 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include MCC_H
#include "mcc_util.h"
#include "mcc_mod_util.h"
#include "mcc_mod_bsim3v3.h"
#include "mcc_mod_bsim4.h"
#include "mcc_mod_spice.h"

/******************************************************************************\
 FUNCTION : mcc_get_nf      
\******************************************************************************/
int mcc_get_nf ( elp_lotrs_param *lotrsparam, mcc_modellist *ptmodel )
{
  int NF=1;

  if ( lotrsparam && (lotrsparam->PARAM[elpNF] > ELPMINVALUE) ) 
    NF = MCC_ROUND (lotrsparam->PARAM[elpNF]);
  else
    NF = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_NF));

  return NF;
}

/******************************************************************************\
 FUNCTION : mcc_compute_vth0
\******************************************************************************/
double mcc_compute_vth0 ( mcc_modellist *ptmodel, 
                        double Leff, double Weff,
                        double L, double W,
                        elp_lotrs_param *lotrsparam,
                        int need_stress_bs43,
                        int binunit
                      )
{
  double VTH0;
#ifdef OPTIM11
  if (lotrsparam->MCC_SAVED[__MCC_SAVED_VTH0]!=ELPINITVALUE)
     return lotrsparam->MCC_SAVED[__MCC_SAVED_VTH0];
#endif
  //VTH0 = mcc_getprm_quick (ptmodel,"VTH0") + mcc_getprm (ptmodel,"DELVT0");
  VTH0 = mcc_getprm_quick (ptmodel,__MCC_GETPRM_VTH0) + mcc_getprm_quick (ptmodel,__MCC_GETPRM_DELVTO);
  if ( need_stress_bs43 )
    VTH0 = mcc_update_Kstress_vth0_bs43 ( ptmodel,
                                          lotrsparam,
                                          VTH0,
                                          L,
                                          W
                                        );
  VTH0 = mcc_update_wpe_vth0( ptmodel, lotrsparam, L, W, Leff, Weff, binunit, VTH0 );
  VTH0 = mcc_update_vth0 ( lotrsparam, VTH0 );
  VTH0 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VTH0,VTH0,MCC_NO_LOG) ;

  lotrsparam->MCC_SAVED[__MCC_SAVED_VTH0]=VTH0;
  return VTH0;
}
/******************************************************************************\
 FUNCTION : mcc_compute_k2
\******************************************************************************/
double mcc_compute_k2 ( mcc_modellist *ptmodel, 
                        double Leff, double Weff,
                        double L, double W,
                        elp_lotrs_param *lotrsparam,
                        int need_stress_bs43,
                        int binunit
                      )
{
  double K2;

#ifdef OPTIM6
  if (lotrsparam->MCC_SAVED[__MCC_SAVED_K2]!=ELPINITVALUE)
     return lotrsparam->MCC_SAVED[__MCC_SAVED_K2];
#endif
  K2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_K2);   
  if ( need_stress_bs43 )
    K2 = mcc_update_Kstress_k2_bs43 ( ptmodel,
                                      lotrsparam,
                                      K2,
                                      L,
                                      W);
  K2 = mcc_update_wpe_k2( ptmodel, lotrsparam, L, W, Leff, Weff, binunit, K2 );
  K2 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K2,K2,MCC_NO_LOG) ;
  lotrsparam->MCC_SAVED[__MCC_SAVED_K2]=K2;

  return K2;
}

/******************************************************************************\
 FUNCTION : mcc_need_stress_bs43
\******************************************************************************/
int mcc_need_stress_bs43 ( mcc_modellist *ptmodel, 
                           elp_lotrs_param *lotrsparam
                         )
{
  int VERSION;
  int res = 0,NF;
  double SA,SB,SD;
  int SA_set = 0, SB_set = 0, SD_set = 0;
  
  if( !lotrsparam )
    return res ;

#ifdef OPTIM14
  if (lotrsparam->MCC_SAVED[__MCC_SAVED_NEED_STRESS]!=ELPINITVALUE)
     return lotrsparam->MCC_SAVED[__MCC_SAVED_NEED_STRESS];
#endif

  VERSION = MCC_ROUND (10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION));

  NF = mcc_get_nf (lotrsparam,ptmodel);

  if ( VERSION >= 43 ) {
    if ( (lotrsparam->PARAM[elpSA] > ELPMINVALUE ) ||
          (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_SA) == MCC_SETVALUE) ) {
      if ( ( SA = lotrsparam->PARAM[elpSA] ) < ELPMINVALUE )
        SA = mcc_getparam_quick(ptmodel,__MCC_QUICK_SA);
      if ( SA > 0.0 )
        SA_set = 1;
    }
    if ( (lotrsparam->PARAM[elpSB] > ELPMINVALUE ) ||
          (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_SB) == MCC_SETVALUE) ) {
      if ( ( SB = lotrsparam->PARAM[elpSB] ) < ELPMINVALUE )
        SB = mcc_getparam_quick(ptmodel,__MCC_QUICK_SB);
      if ( SB > 0.0 )
        SB_set = 1;
    }
    if ( (lotrsparam->PARAM[elpSD] > ELPMINVALUE ) ||
          (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_SD) == MCC_SETVALUE) ) {
      if ( ( SD = lotrsparam->PARAM[elpSD] ) < ELPMINVALUE )
        SD = mcc_getparam_quick(ptmodel,__MCC_QUICK_SD);
      if ( SD > 0.0 )
        SD_set = 1;
    }
         
    if ( (SA_set && SB_set && (NF==1)) || 
       ( SA_set && SB_set && SD_set && NF >= 1 ) ) 
      res = 1;
  }
  lotrsparam->MCC_SAVED[__MCC_SAVED_NEED_STRESS]=res;
  return res;
}

/******************************************************************************\
 FUNCTION : mcc_update_Kstress_vsat_bs43
\******************************************************************************/
double mcc_update_Kstress_vsat_bs43 ( mcc_modellist *ptmodel,
                                      elp_lotrs_param *lotrsparam,
                                      double vsat_orig,
                                      double temp,
                                      double L,
                                      double W
                                    )
{
  double vsat;
  double KVSAT;
  double Pueff,Pueff_ref;
  double inv_sa,inv_saref,inv_sb,inv_sbref;
  int NF ;

  KVSAT = mcc_getparam_quick(ptmodel, __MCC_QUICK_KVSAT);
  mcc_calc_inv_sa_bs43 ( ptmodel, lotrsparam, L, &inv_sa, &inv_saref);
  mcc_calc_inv_sb_bs43 ( ptmodel, lotrsparam, L, &inv_sb, &inv_sbref);

  NF = mcc_get_nf (lotrsparam,ptmodel);
  Pueff =  mcc_calc_Pueff_bs43 ( ptmodel, inv_sa, inv_sb, temp, L, W, NF, lotrsparam);
  Pueff_ref =  mcc_calc_Pueff_bs43 ( ptmodel, inv_saref, inv_sbref, temp, L, W, NF, lotrsparam);

  vsat = vsat_orig * ( 1.0 + KVSAT*Pueff )/( 1.0 + KVSAT*Pueff_ref );

  return vsat;
}

/******************************************************************************\
 FUNCTION : mcc_update_Kstress_ueff_bs43
\******************************************************************************/
double mcc_update_Kstress_ueff_bs43 ( mcc_modellist *ptmodel,
                                      elp_lotrs_param *lotrsparam,
                                      double ueff_orig,
                                      double temp,
                                      double L,
                                      double W
                                    )
{
  double ueff;
  double Pueff,Pueff_ref;
  double inv_sa,inv_saref,inv_sb,inv_sbref;
  int NF ;

  mcc_calc_inv_sa_bs43 ( ptmodel, lotrsparam, L, &inv_sa, &inv_saref);
  mcc_calc_inv_sb_bs43 ( ptmodel, lotrsparam, L, &inv_sb, &inv_sbref);

  NF = mcc_get_nf (lotrsparam,ptmodel);
  Pueff =  mcc_calc_Pueff_bs43 ( ptmodel, inv_sa, inv_sb, temp, L, W, NF, lotrsparam);
  Pueff_ref =  mcc_calc_Pueff_bs43 ( ptmodel, inv_saref, inv_sbref, temp, L, W,NF, lotrsparam);

  ueff = ueff_orig * ( 1.0 + Pueff )/( 1.0 + Pueff_ref );

  return ueff;
}

/******************************************************************************\
 FUNCTION : mcc_update_Kstress_eta0_bs43
\******************************************************************************/
double mcc_update_Kstress_eta0_bs43 ( mcc_modellist *ptmodel,
                                      elp_lotrs_param *lotrsparam,
                                      double ETA0_orig,
                                      double L,
                                      double W, 
                                      int NF
                                      
                                    )
{
  double STETA0,LODETA0;
  double ETA0;
  double Kstress_vth0;
  double inv_sa,inv_saref,inv_sb,inv_sbref;

  STETA0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_STETA0);
  LODETA0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_LODETA0);
  mcc_calc_inv_sa_bs43 ( ptmodel, lotrsparam, L, &inv_sa, &inv_saref);
  mcc_calc_inv_sb_bs43 ( ptmodel, lotrsparam, L, &inv_sb, &inv_sbref);
  Kstress_vth0 = mcc_calc_Kstress_vth0_bs43 ( ptmodel, L ,W, NF, lotrsparam);
  ETA0 = ETA0_orig + STETA0/pow(Kstress_vth0,LODETA0)*(inv_sa+inv_sb-inv_saref-inv_sbref);

  return ETA0;
}

/******************************************************************************\
 FUNCTION : mcc_update_Kstress_k2_bs43
\******************************************************************************/
double mcc_update_Kstress_k2_bs43 ( mcc_modellist *ptmodel,
                                    elp_lotrs_param *lotrsparam,
                                    double K2_orig,
                                    double L,
                                    double W
                                  )
{
  double STK2,K2;
  double LODK2;
  double Kstress_vth0;
  double inv_sa,inv_saref,inv_sb,inv_sbref;
  int NF = mcc_get_nf (lotrsparam,ptmodel);

  STK2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_STK2);
  LODK2 = mcc_getparam_quick(ptmodel, __MCC_QUICK_LODK2);
  mcc_calc_inv_sa_bs43 ( ptmodel, lotrsparam, L, &inv_sa, &inv_saref);
  mcc_calc_inv_sb_bs43 ( ptmodel, lotrsparam, L, &inv_sb, &inv_sbref);
  Kstress_vth0 = mcc_calc_Kstress_vth0_bs43 ( ptmodel, L ,W, NF, lotrsparam);
  K2 = K2_orig + STK2/pow(Kstress_vth0,LODK2)*(inv_sa+inv_sb-inv_saref-inv_sbref);

  return K2;
}

/******************************************************************************\
 FUNCTION : mcc_update_Kstress_vth0_bs43
\******************************************************************************/
double mcc_update_Kstress_vth0_bs43 ( mcc_modellist *ptmodel,
                                      elp_lotrs_param *lotrsparam,
                                      double VTH0_orig,
                                      double L,
                                      double W
                                    )
{
  double KVTH0,VTH0;
  double Kstress_vth0;
  double inv_sa,inv_saref,inv_sb,inv_sbref;
  int NF = mcc_get_nf (lotrsparam,ptmodel);

  KVTH0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_KVTH0);
  mcc_calc_inv_sa_bs43 ( ptmodel, lotrsparam, L, &inv_sa, &inv_saref);
  mcc_calc_inv_sb_bs43 ( ptmodel, lotrsparam, L, &inv_sb, &inv_sbref);
  Kstress_vth0 = mcc_calc_Kstress_vth0_bs43 ( ptmodel, L ,W, NF, lotrsparam);
  VTH0 = VTH0_orig + KVTH0/Kstress_vth0*(inv_sa+inv_sb-inv_saref-inv_sbref);

  return VTH0;
}

/******************************************************************************\
 FUNCTION : mcc_calc_Kstress_vth0_bs43
\******************************************************************************/
double mcc_calc_Kstress_vth0_bs43 ( mcc_modellist *ptmodel,
                                    double L,
                                    double W,
                                    int NF,
                                    elp_lotrs_param *lotrsparam
                                  )
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0;
  static double prevW = 0.0;
  static int    prevNF = 0.0;
  static double LKVTH0;
  static double WKVTH0;
  static double PKVTH0;
  static double XL;
  static double XW;
  static double LLODVTH;
  static double WLODVTH;
  static double WLOD;
  static double c0,c1;
  static double Kstress_vth0;
  static chain_list *previous_longkey=NULL;

  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) {
    LKVTH0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_LKVTH0);
    WKVTH0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_WKVTH0);
    PKVTH0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_PKVTH0);
    XL = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL);
    XW = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW);
    LLODVTH = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLODVTH);
    WLODVTH = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLODVTH);
    WLOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLOD);
 
    c0 = pow (L+XL,LLODVTH);
    c1 = pow (W+XW+WLOD,WLODVTH);
 
    Kstress_vth0 = 1.0 + LKVTH0/c0 + WKVTH0/c1 + PKVTH0/(c0*c1);
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;

  return Kstress_vth0;
}


/******************************************************************************\
 FUNCTION : mcc_calc_Pueff_bs43
\******************************************************************************/
double mcc_calc_Pueff_bs43 ( mcc_modellist *ptmodel,
                             double inv_sa, double inv_sb,
                             double temp,
                             double L,
                             double W,
                             int NF, elp_lotrs_param *lotrsparam )
{
  double KU0,Pueff;
  double Kstress_u0;

  KU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_KU0);

  Kstress_u0 = mcc_calc_Kstress_u0_bs43 ( ptmodel, temp, L, W, NF, lotrsparam);

  Pueff = KU0/Kstress_u0 * ( inv_sa + inv_sb );

  return Pueff;
}

/******************************************************************************\
 FUNCTION : mcc_calc_Kstress_u0_bs43
\******************************************************************************/
double mcc_calc_Kstress_u0_bs43 ( mcc_modellist *ptmodel,
                                  double temp,
                                  double L,
                                  double W,
                                  int NF, elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0;
  static double prevW = 0.0;
  static int prevNF = 0.0;
  static double LKU0;
  static double LLODKU0;
  static double XL;
  static double WKU0;
  static double PKU0;
  static double WLODKU0;
  static double XW;
  static double WLOD;
  static double TKU0;
  static double TNOM;
  static double Temp;
  static double c0,c1;
  static double Kstress_u0;
  static chain_list *previous_longkey=NULL;

  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) {
    LKU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_LKU0);
    WKU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_WKU0);
    PKU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_PKU0);
    XL = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL);
    XW = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW);
    WLOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLOD);
    LLODKU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLODKU0);
    WLODKU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLODKU0);
    TKU0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_TKU0);
    TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) + MCC_KELVIN;
    Temp = temp + MCC_KELVIN;
 
    c0 = pow(L+XL,LLODKU0);
    c1 = pow(W+XW+WLOD,WLODKU0);
    Kstress_u0 = (1.0 + LKU0/c0 + WKU0/c1 + PKU0/(c0*c1) ) * (1.0+TKU0*(Temp/TNOM-1.0));
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;

  return Kstress_u0;
}

/******************************************************************************\
 FUNCTION : mcc_calc_inv_sa_bs43
\******************************************************************************/
void mcc_calc_inv_sa_bs43 (mcc_modellist *ptmodel,
                           elp_lotrs_param *lotrsparam,
                           double L,
                           double *inv_sa,
                           double *inv_saref)
{
  int i,NF = 1;
  double SA,SD;
  double SAREF;

  if ( ( SA = lotrsparam->PARAM[elpSA] ) < ELPMINVALUE )
    SA = mcc_getparam_quick(ptmodel, __MCC_QUICK_SA);
  SAREF = mcc_getparam_quick(ptmodel, __MCC_QUICK_SAREF);
  if ( ( SD = lotrsparam->PARAM[elpSD] ) < ELPMINVALUE )
    SD = mcc_getparam_quick(ptmodel, __MCC_QUICK_SD);

  NF = mcc_get_nf (lotrsparam,ptmodel);
  if ( inv_sa ) {
    if ( NF == 1 )
      *inv_sa = 1.0 / ( SA + 0.5 * L );
    else {
      *inv_sa = 0.0;
      for ( i = 0 ; i <= NF - 1 ; i++ )
        *inv_sa += 1.0/(NF*(SA+0.5*L+i*(SD+L)));
    }
  }
  if ( inv_saref ) {
    if ( NF == 1 )
      *inv_saref = 1.0 / ( SAREF + 0.5 * L );
    else {
      *inv_saref = 0.0;
      for ( i = 0 ; i <= NF - 1 ; i++ )
        *inv_saref += 1.0/(NF*(SAREF+0.5*L+i*(SD+L)));
    }
  }
}

/******************************************************************************\
 FUNCTION : mcc_calc_inv_sb_bs43
\******************************************************************************/
void mcc_calc_inv_sb_bs43 (mcc_modellist *ptmodel,
                           elp_lotrs_param *lotrsparam,
                           double L,
                           double *inv_sb,
                           double *inv_sbref)
{
  int i,NF = 1;
  double SB,SD;
  double SBREF;

  if ( ( SB = lotrsparam->PARAM[elpSB] ) < ELPMINVALUE )
    SB = mcc_getparam_quick(ptmodel, __MCC_QUICK_SB);
  SBREF = mcc_getparam_quick(ptmodel, __MCC_QUICK_SBREF);
  if ( ( SD = lotrsparam->PARAM[elpSD] ) < ELPMINVALUE )
    SD = mcc_getparam_quick(ptmodel, __MCC_QUICK_SD);

  NF = mcc_get_nf (lotrsparam,ptmodel);
  if ( inv_sb ) {
    if ( NF == 1 )
      *inv_sb = 1.0 / ( SB + 0.5 * L );
    else {
      *inv_sb = 0.0;
      for ( i = 0 ; i <= NF - 1 ; i++ )
        *inv_sb += 1.0/(NF*(SB+0.5*L+i*(SD+L)));
    }
  }
  if ( inv_sbref ) {
    if ( NF == 1 )
      *inv_sbref = 1.0 / ( SBREF + 0.5 * L );
    else {
      *inv_sbref = 0.0;
      for ( i = 0 ; i <= NF - 1 ; i++ )
        *inv_sbref += 1.0/(NF*(SBREF+0.5*L+i*(SD+L)));
    }
  }
}


/******************************************************************************\
 FUNCTION : mcc_calc_leff_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_leff_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                            double L, double W)
{
  double LL, LW, LWL, LLN, LWN, XL, XW, LINT ;
  double Lprinted, Wprinted, dL, Leff, NF ;

  LL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LL)      ;
  LW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LW)      ;
  LWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWL)     ;
  LLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLN)     ;
  LWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWN)     ;
  XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
  XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
  LINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT)    ;
  NF = mcc_get_nf (lotrsparam,ptmodel);

  Lprinted   = L + XL ;
  Wprinted   = W/NF + XW ;
  dL         = LINT + LL/pow(Lprinted, LLN) + LW/pow(Wprinted, LWN) 
               + LWL/(pow(Lprinted,LLN) * pow(Wprinted, LWN)) ;
  Leff       = Lprinted - 2.0*dL ;

  if ( Leff <= 0.0 ) {
    avt_errmsg(MCC_ERRMSG, "009", AVT_ERROR, ptmodel->NAME,mcc_ftol(L*1e09),mcc_ftol(W*1e09),mcc_ftol(L*1e09));
    Leff = L;
  }

  return Leff ;
}

/******************************************************************************\
 FUNCTION : mcc_calc_weffcj_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_weffcj_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                              double L, double W)
{
  return W + mcc_calcDWCJ_bsim4 (ptmodel, lotrsparam, L, W) ;
}


/******************************************************************************\
 FUNCTION : mcc_calc_weff_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_weff_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double L, double W)
{
  double WL, WW, WWL, WLN, WWN, XL, XW, WINT ;
  double Lprinted, Wprinted, dW, NF,Weff ;
  
  WL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WL)      ;
  WW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WW)      ;
  WWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWL)     ;
  WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
  WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;
  XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
  XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
  WINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_WINT)    ;
  NF = mcc_get_nf (lotrsparam,ptmodel);

  Lprinted   = L + XL ;
  Wprinted   = W/NF + XW ;
  dW         = WINT + WL/pow(Lprinted, WLN) + WW/pow(Wprinted, WWN) 
               + WWL/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;
  Weff       = Wprinted - 2.0*dW ;

  return Weff ;
}

/******************************************************************************\
 FUNCTION : mcc_calc_vbi_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_vbi_bsim4 (mcc_modellist *ptmodel, double temp,
                           double Leff, double Weff, int getparm_flag) 
{
  static double NSD, NDEP, ni, vbi;
  static double vt, BINUNIT;

  if ( getparm_flag ) {
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT);
    vt   = mcc_calc_vt (temp);
    ni   = mcc_calc_ni_bsim4 (ptmodel,temp);
    NSD  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NSD,MCC_NO_LOG) ;
    NSD *= 1.0e6 ;
    NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG);
    if(NDEP <= 1.0e20)
        NDEP *= 1.0e6 ;
    vbi = vt * log (NSD*NDEP/pow(ni, 2.0));
  }

  return vbi;
}

/*----------------------------------------------------------------------------*\

 EFFICACE Voltage computation

\*----------------------------------------------------------------------------*/

/******************************************************************************\
 FUNCTION : mcc_calc_vbseff_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_vbseff_bsim4 (mcc_modellist *ptmodel,
                              double Vbs,
                              double Leff, double Weff,
                              double temp, double K2,
                              int getparm_flag, elp_lotrs_param *lotrsparam) 
{
  double delta1 = 0.001;
  double c0,c1,c2;
  static double Vbseff,Vbc, phis;
  static double K1,BINUNIT;
  static double VBM; // additionnal var for ng src

#ifdef OPTIM15
  if (!getparm_flag && lotrsparam->MCC_SAVED[__MCC_LAST_VBS_FORVBSEFF]==Vbs && lotrsparam->MCC_SAVED[__MCC_SAVED_VBSEFF]!=ELPINITVALUE)
     return lotrsparam->MCC_SAVED[__MCC_SAVED_VBSEFF];
#endif
  
  if ( getparm_flag ) {
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT);
    K1 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);
    K2 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K2,MCC_NO_LOG);
    VBM = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VBM,MCC_NO_LOG);
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
  }
  if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE != SIM_TOOLMODEL_ELDO ) {
    if((K2 != 0.0) && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_K2) == MCC_SETVALUE)) {
      if ( K2 < 0.0 ) {
        Vbc = 0.9*(phis-K1*K1/(4.0*K2*K2));
        if ( Vbc > -3.0 ) Vbc = -3.0;
        else if ( Vbc < -30.0 ) Vbc = -30.0;
      }
      else 
        Vbc = -30.0;
      if ( Vbc > VBM ) Vbc = VBM;
      c0 = Vbs - Vbc - delta1;
      c1 = sqrt(c0 * c0 - 0.004* Vbc);
      if ( c0 >= 0.0 ) 
        Vbseff = Vbc + 0.5 * (c0 + c1);
      else { 
        c2 = -0.002 / (c1 - c0);
        Vbseff = Vbc * (1.0 + c2);
      }
    }
    else
      Vbseff = Vbs;
  }
  else {              // ELDO case
    if((K2 != 0.0) && (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_K2) == MCC_SETVALUE)) {
      Vbc = 0.9*(phis-K1*K1/(4.0*K2*K2));
      Vbseff = Vbc + 0.5*(Vbs-Vbc-delta1 + sqrt(pow(Vbs-Vbc-delta1,2.0)-4.0*delta1*Vbc));
    }
    else
      Vbseff = Vbs;
  }
  if (Vbseff < Vbs) //from ng src
    Vbseff = Vbs;
  lotrsparam->MCC_SAVED[__MCC_LAST_VBS_FORVBSEFF]=Vbs;
  lotrsparam->MCC_SAVED[__MCC_SAVED_VBSEFF]=Vbseff;
  return Vbseff;
}

/******************************************************************************\
Function : mcc_calc_vgsteffcv_bsim4 ()
\******************************************************************************/
double mcc_calc_vgsteffcv_bsim4 (mcc_modellist *ptmodel,
                                 double Vth, double Vbseff, 
                                 double Vds, double Vgs,
                                 double temp,
                                 double Leff, double Weff,
                                 double L, double W,
                                 elp_lotrs_param *lotrsparam,
                                 int getparm_flag
                                 )
{
  double Vgsteffcv, Vgseff;
  static double Vt, phis, Coxe;
  static double BINUNIT, NFACTOR, VTH0,
                CDSC, CDSCD, CDSCB, DVT1, CIT, 
                DVT2, VOFFCV, NGATE, NDEP,NOFF,K1;
  double Cdsc_Term, n, Lt, Xdep, Vfb, K,c_0,c_1;
  double T0, T1, T2, T3, T4, Theta0;
  int need_stress_bs43=0;
        
  if ( getparm_flag ) {
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;   
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
    Vt = mcc_calc_vt (temp);

    NOFF = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NOFF,MCC_NO_LOG) ;
    NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG) ;
    if(NDEP <= 1.0e20)
      NDEP *= 1.0e6 ;
    NGATE = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NGATE,MCC_NO_LOG) ;
    if(NGATE <= 1.0e23)
      NGATE = NGATE *1.0e6 ;
    NFACTOR = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NFACTOR,MCC_NO_LOG) ;
    CDSC    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CDSC,MCC_NO_LOG) ;
    CDSCD   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CDSCD,MCC_NO_LOG) ;
    CDSCB   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CDSCB,MCC_NO_LOG) ;
    DVT1    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DVT1,MCC_NO_LOG) ;
    DVT2    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DVT2,MCC_NO_LOG) ;
    CIT     = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIT,MCC_NO_LOG) ;
    VOFFCV  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VOFFCV,MCC_NO_LOG) ;
    K1      = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);

    Coxe = mcc_calc_coxe_bsim4 ( ptmodel );
  }
  need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);
  VTH0 = mcc_compute_vth0 ( ptmodel, 
                            Leff, Weff,
                            L, W,
                            lotrsparam,
                            need_stress_bs43,
                            BINUNIT
                          );
  if ( ptmodel->TYPE == MCC_PMOS )
    VTH0 = -VTH0;
  Xdep = sqrt((2.0*MCC_EPSSI/(MCC_Q*NDEP)))*sqrt(phis - Vbseff) ; 
  Lt = sqrt(MCC_EPSSI*Xdep/Coxe) * (1.0 + DVT2 * Vbseff) ;

  T0 = DVT1 * Leff/Lt;
  if ( T0 < MCC_EXP_THRESHOLD ) {
    T1 = exp(T0);
    T2 = T1 - 1.0;
    T3 = T2 * T2;
    T4 = T3 + 2.0 * T1 * MCC_MIN_EXP;
    Theta0 = T1 / T4;
  }
  else {
    Theta0 = 1.0 / (MCC_MAX_EXP - 2.0);
  }
    
  Cdsc_Term = (CDSC + CDSCD*Vds + CDSCB*Vbseff ) * Theta0;
  n = 1.0 + (NFACTOR*MCC_EPSSI/Xdep+ (Cdsc_Term + CIT)) / Coxe;

  c_0 = n*Vt;

  // Expression de Vgseff en fonction du parametre NGATE 
  if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NGATE) == MCC_SETVALUE ) {
#ifdef OPTIM10
    if (lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]!=ELPINITVALUE)
      Vfb=lotrsparam->MCC_SAVED[__MCC_SAVED_VFB];
    else
#endif
    {
      if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VFB) != MCC_SETVALUE)
        Vfb = VTH0 - phis - K1 * sqrt (phis) ;
      else
        Vfb = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG) ;
      lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]=Vfb;
    }
    K = MCC_Q*MCC_EPSSI*NGATE/pow(Coxe, 2.0) ;
    if (K != 0.0) {
      Vgseff = Vfb + phis + K*( sqrt(1.0+(2.0*(Vgs-Vfb-phis)/K)) - 1.0 ) ;
    }
    else
      Vgseff = Vgs ;
  }
  else
    Vgseff = Vgs ;
  
  // Final expression for Vgsteffcv
  
  c_1 = (Vgseff-Vth-VOFFCV)/(NOFF*c_0);

  if (c_1 > MCC_EXP_THRESHOLD) 
    Vgsteffcv = Vgseff -Vth - VOFFCV;
  else if (c_1 < -MCC_EXP_THRESHOLD)
    Vgsteffcv = c_0*NOFF* log(1.0 + MCC_MIN_EXP);
  else
    Vgsteffcv = NOFF*c_0*log ( 1.0 + exp (c_1) );

  return Vgsteffcv;
}

/******************************************************************************\
Function : mcc_calc_vgsteff_bsim4 ()
\******************************************************************************/
double mcc_calc_vgsteff_bsim4 (mcc_modellist *ptmodel,
                               double Vth, double Vbseff, 
                               double Vds, double Vgs,
                               double temp,
                               double Leff, double Weff,
                               double L, double W,
                               double *Vgseff_r,
                               elp_lotrs_param *lotrsparam,
                               int getparm_flag
                               )
{
  double Vgsteff, Vgseff, numerateur, denominateur;
  static double K1ox,m_star, Voff_p, Vt, phis, Coxe;
  static double BINUNIT, MINV, NFACTOR, VTH0,
                CDSC, CDSCD, CDSCB, DVT1, CIT, K1,
                DVT2, VOFF, VOFFL, NGATE, NDEP, VOFFT, TVOFF, TNOM ;
  double Cdsc_Term, n, Lt, Xdep, Vfb, K;
  double T0, T1, T2, T3, T4, Theta0;
  double c0;
  int need_stress_bs43=0;
        
  if ( getparm_flag ) {
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;   
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
    Vt = mcc_calc_vt (temp);

    VOFFL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_VOFFL) ;   
    MINV = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_MINV,MCC_NO_LOG) ;
    NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG) ;
    if(NDEP <= 1.0e20)
      NDEP *= 1.0e6 ;
    NGATE = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NGATE,MCC_NO_LOG) ;
    if(NGATE <= 1.0e23)
      NGATE = NGATE *1.0e6 ;
    NFACTOR = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NFACTOR,MCC_NO_LOG) ;
    CDSC    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CDSC,MCC_NO_LOG) ;
    CDSCD   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CDSCD,MCC_NO_LOG) ;
    CDSCB   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CDSCB,MCC_NO_LOG) ;
    DVT1    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DVT1,MCC_NO_LOG) ;
    DVT2    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DVT2,MCC_NO_LOG) ;
    CIT     = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIT,MCC_NO_LOG) ;
    VOFF    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VOFF,MCC_NO_LOG) ;
    TVOFF   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_TVOFF,MCC_NO_LOG) ;
    K1      = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);
    K1ox    = K1 * (mcc_getparam_quick(ptmodel,__MCC_QUICK_TOXE)/mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM)) ;
    TNOM    = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);

    VOFFT = VOFF * ( 1.0 - TVOFF*( temp - TNOM ) );

    m_star = 0.5 + atan (MINV)/MCC_PI;
    Coxe = mcc_calc_coxe_bsim4 ( ptmodel );
    Voff_p = VOFFT + VOFFL/Leff;
  }
  need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);
  VTH0 = mcc_compute_vth0 ( ptmodel, 
                            Leff, Weff,
                            L, W,
                            lotrsparam,
                            need_stress_bs43,
                            BINUNIT
                          );
  if ( ptmodel->TYPE == MCC_PMOS )
    VTH0 = -VTH0;
  Xdep = sqrt((2.0*MCC_EPSSI/(MCC_Q*NDEP)))*sqrt(phis - Vbseff) ; 
  Lt = sqrt(MCC_EPSSI*Xdep/Coxe) * (1.0 + DVT2 * Vbseff) ;

  T0 = DVT1 * Leff/Lt;
  if ( T0 < MCC_EXP_THRESHOLD ) {
    T1 = exp(T0);
    T2 = T1 - 1.0;
    T3 = T2 * T2;
    T4 = T3 + 2.0 * T1 * MCC_MIN_EXP;
    Theta0 = T1 / T4;
  }
  else {
    Theta0 = 1.0 / (MCC_MAX_EXP - 2.0);
  }
    
  Cdsc_Term = (CDSC + CDSCD*Vds + CDSCB*Vbseff ) * Theta0;
  n = 1.0 + (NFACTOR*MCC_EPSSI/Xdep + (Cdsc_Term + CIT)) / Coxe;

  // Expression de Vgseff en fonction du parametre NGATE 
  if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NGATE) == MCC_SETVALUE){
  
#ifdef OPTIM10
    if (lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]!=ELPINITVALUE)
      Vfb=lotrsparam->MCC_SAVED[__MCC_SAVED_VFB];
    else
#endif
    {
      if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VFB) != MCC_SETVALUE)
        Vfb = VTH0 - phis - K1 * sqrt (phis) ;
      else
        Vfb = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG) ;
      lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]=Vfb;
    }
    K = MCC_Q*MCC_EPSSI*NGATE/pow(Coxe, 2.0) ;
    c0 = Vfb+phis;
    if (K != 0.0 && Vgs > c0) {
      double phi,T1,T2,T3,T4,T5,T6,T7,T8;

      phi = phis+Vfb;
      T1 = MCC_Q* MCC_EPSSI * NGATE / (Coxe * Coxe);
      T8 = Vgs - phi;
      T4 = sqrt(1.0 + 2.0 * T8 / T1);
      T2 = 2.0 * T8 / (T4 + 1.0);
      T3 = 0.5 * T2 * T2 / T1;
      T7 = 1.12 - T3 - 0.05;
      T6 = sqrt(T7 * T7 + 0.224);
      T5 = 1.12 - 0.5 * (T7 + T6);
      Vgseff = Vgs - T5;

    }
    else
      Vgseff = Vgs ;
  }
  else
    Vgseff = Vgs ;
  
  // Final expression for Vgsteff 
  
  numerateur = n*Vt* log (1.0 + exp (m_star*(Vgseff - Vth)/(n*Vt)));
  denominateur = m_star + n*Coxe*sqrt (2.0*phis/(MCC_Q*NDEP*MCC_EPSSI))
                 * exp (- ( (1.0-m_star)*(Vgseff-Vth)-Voff_p ) / (n*Vt) );

  Vgsteff = numerateur / denominateur;
  if ( Vgseff_r ) *Vgseff_r = Vgseff;
  return Vgsteff;
}

/******************************************************************************\
Function : mcc_calc_vdseff_bsim4 ()
\******************************************************************************/
void mcc_calc_vdseff_bsim4 ( mcc_modellist *ptmodel,
                             double *Vdseff_r, double *Vdsat_r, 
                             double *Esat_r, double *ueffT_r,
                             double *Abulk_r, double *Rds_r,
                             double *VsatT_r, double *lambda_r,
                             double *Rfactor_r, double *Weff_v_r,
                             double Vth, double Vbseff, 
                             double Vds, double Vgsteff,
                             double temp,
                             double L, double W,
                             elp_lotrs_param *lotrsparam,
                             int getparm_flag
                           )
{
  double Co,Xdep,F_doping,Rds,a,b,c,V1;
  double ueffT,Esat,Vdsat,Vdseff,c_0,c_1,c_2,c_3,c_4,Weff_v;
  static double deltemp;
  static double T,tnom,BINUNIT,Leff,Weff,Weffcj,RDSMOD,Vt,VSAT,
                TNOM,MOBMOD,TOXE,TOXM;
  static double VsatT,u0T,uaT,ubT,ucT,udT,fleff; // mobility parameters
  static double AT,U0,UTE,UA,UA1,UB,UB1,UC,UC1,UD,UD1,UP,LP,K1ox,K1,K2ox,K2,
                EU,VFB,VTH0,K3B,A1,A2,DELTA,phis;
  static double RDSWMIN,WR,PRT,RDSW,PRWB,PRWG,rdswminT,rdswT;
  static double A0,XJ,AGS,B0,B1,W0,KETA,LPEB,NDEP,PARAMCHK,
                Abulk,lambda,Coxe,DWG,DWB,ratio_temp;
  int need_stress_bs43 = 0;
  static int VERSION,TEMPMOD;
  double T0,T1,T2,T3,T4;

  if ( getparm_flag ) {
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT);
    VERSION = MCC_ROUND (10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION));
    TEMPMOD = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_TEMPMOD));
    Leff = mcc_calc_leff_bsim4 (ptmodel,lotrsparam,L,W);
    Weff = mcc_calc_weff_bsim4 (ptmodel,lotrsparam,L,W);
    Weffcj = mcc_calc_weffcj_bsim4 (ptmodel,lotrsparam,L,W);
    TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
    T = temp + MCC_KELVIN;
    tnom = TNOM + MCC_KELVIN;
    Vt   = mcc_calc_vt (temp);
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
    RDSMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_RDSMOD) ;
    MOBMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_MOBMOD) ;
    TOXE   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE) ;
    TOXM   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM) ;

    // => Abulk
    K1   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);
    K1ox = K1*TOXE/TOXM ;
    A0   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_A0,MCC_NO_LOG) ;
    XJ   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XJ,MCC_NO_LOG) ;
    AGS  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AGS,MCC_NO_LOG);
    B0   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_B0,MCC_NO_LOG) ;
    B1   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_B1,MCC_NO_LOG) ;
    W0   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_W0,MCC_NO_LOG) ;
    KETA = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_KETA,MCC_NO_LOG);
    K3B  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K3B,MCC_NO_LOG);
    LPEB = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_LPEB,MCC_NO_LOG);
    NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG) ;
    if(NDEP <= 1.0e20)
        NDEP *= 1.0e6 ;
    DELTA= binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DELTA,MCC_NO_LOG) ;

    // => ueffT
    EU   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_EU,MCC_NO_LOG) ;
    VSAT = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VSAT,MCC_NO_LOG) ;
    AT   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AT,MCC_NO_LOG) ;
    A1   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_A1,MCC_NO_LOG) ;
    A2   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_A2,MCC_NO_LOG) ;
    UTE  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UTE,MCC_NO_LOG) ;
    UA   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UA,MCC_NO_LOG) ;
    UA1  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UA1,MCC_NO_LOG) ;
    UB   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UB,MCC_NO_LOG) ;
    UB1  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UB1,MCC_NO_LOG) ;
    UC   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UC,MCC_NO_LOG) ;
    UC1  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UC1,MCC_NO_LOG) ;
    UD   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UD,MCC_NO_LOG) ;
    UD1  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UD1,MCC_NO_LOG) ;
    UP   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_UP,MCC_NO_LOG) ;
    LP   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_LP,MCC_NO_LOG) ;

    // => Rds
    RDSWMIN = mcc_getparam_quick(ptmodel, __MCC_QUICK_RDSWMIN) ;
    PARAMCHK = mcc_getparam_quick(ptmodel, __MCC_QUICK_PARAMCHK) ;
    WR   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_WR,MCC_NO_LOG) ;
    PRT  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PRT,MCC_NO_LOG) ;
    RDSW = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_RDSW,MCC_NO_LOG) ;
    PRWB = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PRWB,MCC_NO_LOG) ;
    PRWG = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PRWG,MCC_NO_LOG) ;
    DWG  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DWG,MCC_NO_LOG) ;
    DWB  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DWB,MCC_NO_LOG) ;
    
    ratio_temp = T/tnom - 1.0;

    if( RDSW < 0.0 )
      RDSW = 0.0 ;

    // calcul de ueffT
    if (( VERSION >= 43 ) && ( TEMPMOD >= 1)) {
      deltemp = T - tnom;
      // calcul de Rds  
      rdswminT = RDSWMIN * (1.0+PRT*deltemp);
      if (MCC_ROUND(PARAMCHK)!=0 && rdswminT < 0.0) rdswminT = 0.0; // ADD-ON
      rdswT = RDSW * (1.0 + PRT*deltemp);
      VsatT = VSAT *(1.0 - AT*deltemp);       
      uaT = UA *(1.0 + UA1*deltemp) ;
      ubT = UB *(1.0 + UB1*deltemp) ;
      ucT = UC *(1.0 + UC1*deltemp) ;
      udT = UD *(1.0 + UD1*deltemp) ;
    }
    else {
      // calcul de Rds
      rdswminT = RDSWMIN + PRT*ratio_temp;
      if (MCC_ROUND(PARAMCHK)!=0 && rdswminT < 0.0) rdswminT = 0.0; // ADD-ON
      rdswT = RDSW + PRT*ratio_temp;
      VsatT = VSAT - AT*ratio_temp;       
      uaT = UA + UA1 * ratio_temp;
      ubT = UB + UB1 * ratio_temp;
      ucT = UC + UC1 * ratio_temp;
      udT = UD + UD1 * ratio_temp;
    }
  }

#ifdef OPTIM8
  if (lotrsparam->MCC_SAVED[__MCC_SAVED_VDSAT]!=ELPINITVALUE)
  {
    Vdsat=lotrsparam->MCC_SAVED[__MCC_SAVED_VDSAT];
    Esat=lotrsparam->MCC_SAVED[__MCC_SAVED_ESAT];
    ueffT=lotrsparam->MCC_SAVED[__MCC_SAVED_UEFFT];
    Abulk=lotrsparam->MCC_SAVED[__MCC_SAVED_ABULK];
    Rds=lotrsparam->MCC_SAVED[__MCC_SAVED_RDS];
    VsatT=lotrsparam->MCC_SAVED[__MCC_SAVED_VSATT];
    lambda=lotrsparam->MCC_SAVED[__MCC_SAVED_LAMBDA];
    c_3=lotrsparam->MCC_SAVED[__MCC_SAVED_RFACTOR];
    Weff_v=lotrsparam->MCC_SAVED[__MCC_SAVED_WEFFV];
  }
  else
#endif
  {
  need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);
  if ( need_stress_bs43 ) {
    if (( VERSION >= 43 ) && ( TEMPMOD >= 1)) 
      VsatT = VSAT *(1.0 - AT*deltemp);       
    else 
      VsatT = VSAT - AT*ratio_temp;       
    VsatT =  mcc_update_Kstress_vsat_bs43 ( ptmodel,
                                            lotrsparam,
                                            VsatT,
                                            temp,
                                            L,
                                            W
                                          );
  }
  K2 = mcc_compute_k2 ( ptmodel, 
                        Leff, Weff,
                        L, W,
                        lotrsparam,
                        need_stress_bs43,
                        BINUNIT
                      );
  K2ox = K2*TOXE/TOXM ;

  U0 = mcc_getparam_quick(ptmodel,__MCC_QUICK_U0) * mcc_getparam_quick(ptmodel,__MCC_QUICK_MULU0);
  U0 = mcc_update_u0 ( lotrsparam , U0 );
  U0 = binningval_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_U0,U0,MCC_NO_LOG) ;
  if(U0 > 1.0)
      U0 *= 1.0e-4 ;
  // calcul de ueffT
  u0T = U0 * pow((T/tnom), UTE) ; 

  // calcule de ueff en fct de la valeur de MOBMOD
  
  // declaration de qlq constantes utiles
  c_0 = (Vgsteff+2.0*Vth)/TOXE;
  if ( Vbseff <= 0.0 )
    c_1 = sqrt (phis - Vbseff);
  else
    c_1 = (phis*sqrt(phis))/(phis + 0.5 * Vbseff) ;
  c_2 = Vgsteff+2.0*Vt;

  if( VERSION >= 46 )
    c_3 = Vt*TOXE/(Vgsteff+2*sqrt(Vt*Vt+0.0001));
  else
    c_3 = Vt*TOXE/(Vgsteff+2*Vt);

  if( VERSION >= 45 ) {
    fleff = 1.0 - UP*exp(-Leff/LP);
  }
  else 
    fleff = 1.0 ;
  
  if (MCC_ROUND ( MOBMOD ) == 0)
    ueffT = u0T*fleff / ( 1.0 + (uaT + ucT*Vbseff) * c_0 + ubT*pow(c_0, 2.0) + udT*pow(c_3,2.0) ) ; 
  else if (MCC_ROUND (MOBMOD) == 1)
    ueffT = u0T*fleff / ( 1.0 + (uaT*c_0 + ubT*pow(c_0, 2.0))*(1.0 + ucT*Vbseff) + udT*pow(c_3,2.0) ) ;
  else if (MCC_ROUND (MOBMOD) == 2) {
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTH0) == MCC_SETVALUE) {
        VTH0 = mcc_compute_vth0 ( ptmodel, 
                                  Leff, Weff,
                                  L, W,
                                  lotrsparam,
                                  need_stress_bs43,
                                  BINUNIT
                                );
    }
    else
        VTH0 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG)
               + phis + K1ox*sqrt(phis) ;
    if (ptmodel->TYPE == MCC_PMOS)
      VTH0 = -VTH0;
#ifdef OPTIM10
    if (lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]!=ELPINITVALUE)
      VFB=lotrsparam->MCC_SAVED[__MCC_SAVED_VFB];
    else
#endif
    {
      // Flat band voltage
      if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VFB) != MCC_SETVALUE)
        VFB = VTH0 - phis - K1 * sqrt (phis) ;
      else
        VFB = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG) ;
      lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]=VFB;
    }
    if (ptmodel->TYPE == MCC_NMOS) 
      Co = 2.0;
    else
      Co = 2.5;
    ueffT = u0T / ( 1.0 + (uaT + ucT*Vbseff) * pow ( (Vgsteff + Co * (VTH0 - VFB - phis))/TOXE , EU) );
  }
  else  {
    avt_errmsg(MCC_ERRMSG, "007", AVT_ERROR);
    ueffT   = (u0T / ( 1.0 + (uaT + ucT*Vbseff) 
              * c_0+ubT*pow(c_0, 2.0) )) ; 
  }
  
  //------------------------ update ueff for bs43 ----------------
  if ( need_stress_bs43 ) {
    ueffT =  mcc_update_Kstress_ueff_bs43 ( ptmodel,
                                            lotrsparam,
                                            ueffT,
                                            temp,
                                            L,
                                            W
                                          );
  }

  ueffT = mcc_update_wpe_ueff( ptmodel, lotrsparam, L, W, Leff, Weff, BINUNIT, ueffT );
  

  //------------------------ calcul de Esat  ---------------------
  Esat = 2.0*VsatT / ueffT;
  
  //------------------------ calcul de Abulk ---------------------
  Xdep = sqrt((2.0*MCC_EPSSI/(MCC_Q*NDEP)))*c_1 ; 
  F_doping = sqrt (1.0+LPEB/Leff)*K1ox/(2.0*c_1)
             + K2ox - K3B*TOXE*phis/(Weff+W0);

  Abulk = (1.0 +  F_doping * ( (A0*Leff/(Leff+2.0*sqrt (XJ*Xdep)))
          * (1.0 - AGS*Vgsteff * pow ( Leff/(Leff + 2.0*sqrt (XJ*Xdep)) ,2.0) )
          + B0/(Weff+B1) )) * 1.0/(1.0 + KETA*Vbseff); 

  if (Abulk < 0.1) {
    T0 = 1.0 / (3.0 - 20.0 * Abulk);
    Abulk = (0.2 - Abulk) * T0;
  }

  
  //------------------------ calcul des param Vdsat extrinsic case
  lambda = A1*Vgsteff + A2                  ;
  
  if ( MCC_ROUND (RDSMOD) == 1 ) // from ng src
    Rds = 0.0;
  else {
    T0 = 1.0+PRWG*Vgsteff;
    T1 = PRWB*( c_1 - sqrt (phis));
    T2 = 1.0/T0+T1;
    T3 = T2+sqrt(T2*T2+0.01);
    T4 = rdswT/2.0;

    Rds = (rdswminT + T3 * T4)/pow (1.0e6*Weffcj,WR);
  }

  Coxe = mcc_calc_coxe_bsim4 (ptmodel);

  Weff_v = Weff-2.0*DWG*Vgsteff-2.0*DWB*(c_1-sqrt(phis)) ;
  c_3 = Weff_v * VsatT*Coxe*Rds;

  a = pow(Abulk, 2.0) * c_3 + ((1.0/lambda)-1.0) * Abulk ;
  b = -c_2 * ((2.0/lambda)-1.0) - Abulk * (Esat*Leff + 3.0*c_2*c_3) ;
  c = Esat*Leff*c_2 + 2.0*c_3*pow(c_2, 2.0) ;
  
  //------------------------ calcul de Vdsat suivant le selecteur 
  // Intrinsic Case
  if ( MCC_ROUND (RDSMOD) == 1 )
    Vdsat = (Esat*Leff*c_2) / (Abulk*Esat*Leff + c_2) ;
  // Extrinsic Case
  else if ( MCC_ROUND(RDSMOD) == 0 )
    Vdsat = (-b - sqrt(pow(b, 2.0) - 4.0*a*c)) / (2.0*a) ; 
  else  {
    avt_errmsg(MCC_ERRMSG, "011", AVT_ERROR);
    Vdsat = (-b - sqrt(pow(b, 2.0) - 4.0*a*c)) / (2.0*a) ; 
  }
  }

  V1 = Vdsat - Vds - DELTA ;
 
  c_4 = sqrt(V1 * V1 + 4.0 * DELTA * Vdsat);
  if ( V1 >= 0.0 )
    Vdseff = Vdsat - 0.5*(V1 + c_4 ) ;
  else 
    Vdseff = Vdsat * ( 1.0 - (2.0*DELTA) / (c_4 - V1)); // from ng src
  
  if ( Vdseff_r ) *Vdseff_r = Vdseff;
  if ( Vdsat_r ) *Vdsat_r = Vdsat;
  if ( Esat_r ) *Esat_r = Esat;
  if ( ueffT_r ) *ueffT_r = ueffT;
  if ( Abulk_r ) *Abulk_r = Abulk;
  if ( Rds_r ) *Rds_r = Rds;
  if ( VsatT_r ) *VsatT_r = VsatT;
  if ( lambda_r) *lambda_r = lambda;
  if ( Rfactor_r ) *Rfactor_r = c_3 ; 
  if ( Weff_v_r ) *Weff_v_r = Weff_v ; 
  lotrsparam->MCC_SAVED[__MCC_SAVED_VDSAT]= Vdsat;
  lotrsparam->MCC_SAVED[__MCC_SAVED_ESAT]= Esat;
  lotrsparam->MCC_SAVED[__MCC_SAVED_UEFFT]= ueffT;
  lotrsparam->MCC_SAVED[__MCC_SAVED_ABULK]= Abulk;
  lotrsparam->MCC_SAVED[__MCC_SAVED_RDS]= Rds;
  lotrsparam->MCC_SAVED[__MCC_SAVED_VSATT]= VsatT;
  lotrsparam->MCC_SAVED[__MCC_SAVED_LAMBDA]= lambda;
  lotrsparam->MCC_SAVED[__MCC_SAVED_RFACTOR]= c_3 ;
  lotrsparam->MCC_SAVED[__MCC_SAVED_WEFFV]= Weff_v;

}

/******************************************************************************\
 FUNCTION : mcc_calc_coxe_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_coxe_bsim4 (mcc_modellist *ptmodel) 
{
  double EPSROX, TOXE, Coxe = 0.0;

  EPSROX = mcc_getparam_quick(ptmodel, __MCC_QUICK_EPSROX);
  TOXE   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE);

  Coxe = EPSROX*MCC_EPSO/TOXE;
  return Coxe;
}

/******************************************************************************\
 FUNCTION : mcc_calc_coxp_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_coxp_bsim4 (mcc_modellist *ptmodel) 
{
  double EPSROX, TOXP, Coxp = 0.0;

  EPSROX = mcc_getparam_quick(ptmodel, __MCC_QUICK_EPSROX);
  TOXP   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXP);

  Coxp = EPSROX*MCC_EPSO/TOXP;
  return Coxp;
}

/******************************************************************************\
 FUNCTION : mcc_calc_coxeff_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_coxeff_bsim4 (mcc_modellist *ptmodel,
                              double Vgsteff, double temp,
                              double Leff, double Weff,
                              double L, double W,
                              double Vgseff,double Vbseff, double Vfbeff,
                              double BINUNIT, int cv_model,
                              double *coxeff_acc_dep,
                              elp_lotrs_param *lotrsparam,
                              int getparm_flag)
{
  double Ccen,Xdc,Coxeff,Xdc_acc_dep;
  double X0,dx,Xmax,var;
  static double Coxp,VTH0,VFB,phis,TOXP;
  static double K1,NDEP,Ldebye,ACDE;
  int need_stress_bs43=0;

  if ( getparm_flag ) {
    Coxp = mcc_calc_coxp_bsim4 (ptmodel) ;
    TOXP   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXP);
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
    ACDE = binning_quick(ptmodel, BINUNIT,Leff,Weff, __MCC_QUICK_ACDE,MCC_NO_LOG);
//    NDEP = mcc_getparam_quick(ptmodel, __MCC_QUICK_NDEP);
    NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG)    ;
    if(NDEP <= 1.0e20)
        NDEP = NDEP *1.0e6 ;
  }
  need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);
  VTH0 = mcc_compute_vth0 ( ptmodel, 
                            Leff, Weff,
                            L, W,
                            lotrsparam,
                            need_stress_bs43,
                            BINUNIT
                          );
  if (ptmodel->TYPE == MCC_PMOS)
    VTH0 = -VTH0;
#ifdef OPTIM10
    if (lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]!=ELPINITVALUE)
      VFB=lotrsparam->MCC_SAVED[__MCC_SAVED_VFB];
    else
#endif
    {
      if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VFB) == MCC_INITVALUE ) {
        K1 = binning_quick(ptmodel,BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);
        VFB = VTH0 - phis - K1 * sqrt (phis) ;
      }
      else 
        VFB  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VFB,MCC_NO_LOG) ;
      lotrsparam->MCC_SAVED[__MCC_SAVED_VFB]=VFB;
    }
 if ( !cv_model ) {
    var = 4.0*(VTH0-VFB-phis);
    if ( var < 0.0 )
      var = 0.0;
    Xdc = 1.9e-9/(1.0 + pow ((Vgsteff+var)/(2.0e8*TOXP) ,0.7) );
  }
  else {
    // Xdc for accumulation & depletion region
    Ldebye = (1.0/3.0)*sqrt (MCC_EPSSI * mcc_calc_vt (mcc_getparam_quick(ptmodel,__MCC_QUICK_TNOM))/(MCC_Q*NDEP)) ;
    Xdc = Ldebye*exp (ACDE*pow(NDEP/2.0e22,-0.25)*(Vgseff-Vbseff-Vfbeff)/(1.0e8*TOXP));
    Xmax = Ldebye;
    dx = 1.0e-3*TOXP;
    X0 = Xmax-Xdc-dx;
    Xdc_acc_dep = Xmax - 0.5*(X0+ sqrt(X0*X0+4.0*dx*Xmax));
    if ( coxeff_acc_dep ) {
      Ccen = MCC_EPSSI/Xdc_acc_dep;
      *coxeff_acc_dep = Coxp*Ccen/(Coxp+Ccen);
    }

    // Xdc for strong inversion region
    var = 4.0*(VTH0-VFB-phis);
    if ( var < 0.0 )
      var = 0.0;
    Xdc = 1.9e-9/(1.0 + pow ((Vgsteff+var)/(2.0e8*TOXP) ,0.7) );
  }
  Ccen = MCC_EPSSI/Xdc;
  Coxeff = Coxp*Ccen/(Coxp+Ccen); // from ngspice src, false in doc Coxe <=> Coxp

  return Coxeff;
}

/******************************************************************************\
 FUNCTION : mcc_calc_gamma1_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_gamma1_bsim4 (mcc_modellist *ptmodel, double Leff,double Weff,
                            double BINUNIT) 
{
  double NDEP, Coxe = 0.0, gamma1;

//  NDEP = mcc_getparam_quick(ptmodel, __MCC_QUICK_NDEP);
  NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG)    ;
  if(NDEP <= 1.0e20)
      NDEP = NDEP *1.0e6 ;
  Coxe = mcc_calc_coxe_bsim4 (ptmodel);
  gamma1 = sqrt (2.0*MCC_Q*MCC_EPSSI*NDEP) / Coxe;

  return gamma1;
}

/******************************************************************************\
 FUNCTION : mcc_calc_gamma2_bsim4 ()                                                
\******************************************************************************/
double mcc_calc_gamma2_bsim4 (mcc_modellist *ptmodel) 
{
  double NSUB, Coxe = 0.0, gamma2;

  NSUB = mcc_getparam_quick(ptmodel, __MCC_QUICK_NSUB);
  if(NSUB <= 1.0e20)
      NSUB *= 1.0e6 ;
  Coxe = mcc_calc_coxe_bsim4 (ptmodel);
  gamma2 = sqrt (2.0*MCC_Q*MCC_EPSSI*NSUB) / Coxe;

  return gamma2;
}

/******************************************************************************\
 FUNCTION : mcc_calc_ni_bsim4 ()                                                

 Intrinsic carrier concentration of Si
\******************************************************************************/
double mcc_calc_ni_bsim4 (mcc_modellist *ptmodel, double temp) 
{
  double ni = 0.0,tnomk,tnom;
  double NI      = 1.45e10*1.0e6 ; 

  tnom = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
  tnomk= tnom + MCC_KELVIN  ;
  ni  = NI * pow((tnomk/300.15), 1.5)*exp(21.5565981 - mcc_calc_eg(tnom)/(2.0*mcc_calc_vt (temp))) ;

  return ni;
}

/******************************************************************************\
 FUNCTION : mcc_calc_phis_bsim4 ()                                                

 Surface potential
\******************************************************************************/
double mcc_calc_phis_bsim4 (mcc_modellist *ptmodel, double Leff,double Weff,
                            double BINUNIT,double temp) 
{
  double ni,NDEP,PHIN, phis = 0.0;
  double T = temp + MCC_KELVIN;
  double TNOM = mcc_getparam_quick(ptmodel,__MCC_QUICK_TNOM);
  int UPDATEPHI = 0;

  PHIN = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PHIN,MCC_NO_LOG);
//  NDEP = mcc_getparam_quick(ptmodel, __MCC_QUICK_NDEP);
  NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG)    ;
  if(NDEP <= 1.0e20)
      NDEP = NDEP *1.0e6 ;
  if ( UPDATEPHI ) {
    ni = mcc_calc_ni_bsim4 (ptmodel,temp);
    phis = 0.4 + MCC_KB*T/MCC_Q * log (NDEP/ni) + PHIN;
  }
  else {
    ni = mcc_calc_ni_bsim4 (ptmodel,TNOM);
    phis = 0.4 + MCC_KB*(TNOM+MCC_KELVIN)/MCC_Q * log (NDEP/ni) + PHIN;
  }

  return phis;
}

/******************************************************************************\
 FUNCTION : mcc_calc_vbx_bsim4 ()                                                

 Body bias
\******************************************************************************/
double mcc_calc_vbx_bsim4 (mcc_modellist *ptmodel, double Leff, double Weff, 
                           double BINUNIT,double temp) 
{
  double NDEP, XT, phisT;
  double vbx = 0.0;

//  NDEP = mcc_getparam_quick(ptmodel, __MCC_QUICK_NDEP);
  NDEP = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG)    ;
  if(NDEP <= 1.0e20)
      NDEP = NDEP *1.0e6 ;
  XT   = binning_quick(ptmodel, BINUNIT, Leff,Weff,__MCC_QUICK_XT,MCC_NO_LOG);
  phisT = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
  vbx = phisT - MCC_Q*NDEP*XT*XT / (2.0 * MCC_EPSSI);

  return vbx;
}

/******************************************************************************/
/* FUNCTION : mcc_initparam_bsim4 ()                                        */
/******************************************************************************/
void mcc_initparam_bsim4(mcc_modellist *ptmodel) 
{
    mcc_initparam_com(ptmodel)                                      ;

/*-------  Model Selectors / Controllers --------------------------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_VERSION, 4.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BINUNIT, 1.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PARAMCHK, 1.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MOBMOD, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDSMOD, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_IGCMOD, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_IGBMOD, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CAPMOD, 2.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RGATEMOD, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RBODYMOD, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TRNQSMOD, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ACNQSMOD, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_FNOIMOD, 1.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TNOIMOD, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DIOMOD, 1.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PERMOD, 1.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_GEOMOD, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RGEOMOD, 0.0, NULL, MCC_INITVALUE)     ;

/*-------  Process Parameters            --------------------------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_TNOM, V_FLOAT_TAB[__SIM_TNOM].VALUE, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EPSROX, 3.9, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TOXE, 3.0e-9, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TOXP, mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_TOXE), 
                                   MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TOXM, mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_TOXE), 
                                   MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DTOX, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XJ, 1.5e-7, NULL, MCC_INITVALUE)       ;
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_NDEP) == MCC_INITVALUE &&
       mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA1) == MCC_SETVALUE) {
        char buf[1024] ;
        char gamma1[1024] ;
        char *pt1 ;
        char *pt ;
        double coxe = mcc_calc_coxe_bsim4 (ptmodel);

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_GAMMA1)) != NULL)
           sprintf(gamma1,"%s",pt1) ;
        else
           sprintf(gamma1,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_GAMMA1)) ;
        if ( pt1 ) 
         {
          sprintf(buf,"pow (%s * %g,2.0) / (2.0 * %g *%g)",
                       gamma1,coxe,MCC_Q,MCC_EPSSI) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_NDEP, (pow (mcc_getparam_quick(ptmodel,__MCC_QUICK_GAMMA1)*coxe,2.0)/(2.0*MCC_Q*MCC_EPSSI)), 
                      pt, MCC_INITVALUE) ;
    }
    else
	  mcc_initparam_quick(ptmodel, __MCC_QUICK_NDEP, 1.7e-17, NULL, MCC_INITVALUE)    ; // unit cm3
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NSUB, 6.0e-16, NULL, MCC_INITVALUE)    ; // unit cm3
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA1) == MCC_INITVALUE) {
        char buf[1024] ;
        char ndef[1024] ;
        char *pt1 ;
        char *pt ;
        double coxe = mcc_calc_coxe_bsim4 (ptmodel);

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_NDEF)) != NULL)
           sprintf(ndef,"%s",pt1) ;
        else
           sprintf(ndef,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_NDEF)) ;
        if ( pt1 ) 
         {
          sprintf(buf,"sqrt (2.0 * %g * %g * %s) / %g",
                       MCC_Q,MCC_EPSSI,ndef,coxe) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMMA1, (sqrt (2.0*MCC_Q*MCC_EPSSI*mcc_getparam_quick(ptmodel,__MCC_QUICK_NDEF))/coxe), 
                      pt, MCC_INITVALUE) ;
    }
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_GAMMA2) == MCC_INITVALUE) {
        char buf[1024] ;
        char nsub[1024] ;
        char ndef[1024] ;
        char *pt1 ;
        char *pt ;
        double coxe = mcc_calc_coxe_bsim4 (ptmodel);

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_NSUB)) != NULL)
           sprintf(nsub,"%s",pt1) ;
        else
           sprintf(nsub,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_NSUB)) ;
        if ( pt1 ) 
         {
          sprintf(buf,"sqrt (2.0 * %g * %g * %s) / %g",
                       MCC_Q,MCC_EPSSI,ndef,coxe) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_GAMMA2, (sqrt (2.0*MCC_Q*MCC_EPSSI*mcc_getparam_quick(ptmodel,__MCC_QUICK_NSUB))/coxe), 
                      pt, MCC_INITVALUE) ;
    }
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NGATE, 0.0    , NULL, MCC_INITVALUE)   ; // unit cm3
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NSD, 1.0e20 , NULL, MCC_INITVALUE)     ; // unit cm3
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XT, 1.55e-7 , NULL, MCC_INITVALUE)     ; // unit m
    // from threshold param to compute vbx init value               
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PHIN, 0.0 , NULL, MCC_INITVALUE)       ;
/*	mcc_initparam_quick(ptmodel, __MCC_QUICK_VBX, mcc_calc_vbx_bsim4 (ptmodel,0.0,0.0,0.0,tnom) ,
                  NULL, MCC_INITVALUE)   ; */
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RSH, 0.0 , NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RSHG, 1.0 , NULL, MCC_INITVALUE)       ;

/*-------  THRESHOLD PARAMETERS  ----------------------------------*/
	
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VTH0, ptmodel->TYPE == MCC_NMOS ? 0.7:-0.7,
                           NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DELVT0, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VFB, -1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K1, 0.50, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K2, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K3, 80.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_K3B, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_W0, 2.5e-6, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LPE0, 1.74e-7, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LPEB, 0.0    , NULL, MCC_INITVALUE)    ;
    // specifics init for binning parameters ( from ng src )
    if ( V_INT_TAB[__SIM_TOOLMODEL].VALUE != SIM_TOOLMODEL_ELDO ) {
      mcc_initparam_quick(ptmodel, __MCC_QUICK_LLPEB, mcc_getparam_quick(ptmodel, __MCC_QUICK_LLPE0), 
                                      mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LLPE0), 
                                      MCC_INITVALUE) ; 
      mcc_initparam_quick(ptmodel, __MCC_QUICK_WLPEB, mcc_getparam_quick(ptmodel, __MCC_QUICK_WLPE0), 
                                      mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WLPE0), 
                                      MCC_INITVALUE) ; 
      mcc_initparam_quick(ptmodel, __MCC_QUICK_PLPEB, mcc_getparam_quick(ptmodel, __MCC_QUICK_PLPE0), 
                                      mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PLPE0), 
                                      MCC_INITVALUE) ; 
    }
    // end of specifics init
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VBM, -3.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT0, 2.2, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT1, 0.53, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT2, -3.2e-2, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVTP0, 0.0   , NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVTP1, 0.0   , NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT0W, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT1W, 5.3e6, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DVT2W, -3.2e-2, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_U0, ptmodel->TYPE == MCC_NMOS ? 0.067:0.025,
                  NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MULU0, 1.0, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UA, 
                  MCC_ROUND(mcc_getparam_quick(ptmodel,__MCC_QUICK_MOBMOD)) == 2 ? 1.0e-15:1.0e-9,
                  NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UB, 1.0e-19, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UC, 
                  MCC_ROUND(mcc_getparam_quick(ptmodel,__MCC_QUICK_MOBMOD)) == 1 ? -0.0465:-0.0465e-9,
                  NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UD, 
                  MCC_ROUND(10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION)) >= 45  ? 1E14 : 0.0,
                  NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EU, ptmodel->TYPE == MCC_NMOS ? 1.67:1.0,
                  NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VSAT,  8.0e4, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A0,  1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AGS, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_B0, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_B1, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KETA, -0.047, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A1, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_A2, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WINT, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LINT, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DWG, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DWB, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VOFF, -0.08, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VOFFL, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MINV, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NFACTOR, 1.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETA0, 0.08, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ETAB, -0.07, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DROUT, 0.56, NULL, MCC_INITVALUE)      ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DSUB, mcc_getparam_quick(ptmodel, __MCC_QUICK_DROUT), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_DROUT),
                                   MCC_INITVALUE) ; 
    
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIT, 0.0, NULL, MCC_INITVALUE)         ;

/*-------  SUBTHRESHOLD PARAMETERS  ---------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_CDSC, 2.4e-4, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CDSCB, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CDSCD, 0.0, NULL, MCC_INITVALUE)       ;

/*-------  SATURATION PARAMETERS  ---------------------------------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_PCLM, 1.3, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDIBLC1, 0.39, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDIBLC2, 0.0086, NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDIBLCB, 0.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PSCBE1, 4.24e8, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PSCBE1, 1.0e-5, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PVAG, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DELTA, 0.01, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_FPROUT, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDITS, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDITLS, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PDITSD, 0.0, NULL, MCC_INITVALUE)      ;

/*----  Asymmetric and Bias dependent Rds                      ----*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDSW, 200.0, NULL, MCC_INITVALUE)      ; // unit ohm/um
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDSWMIN, 0.0, NULL, MCC_INITVALUE)     ; // unit ohm/um
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDW, 100.0, NULL, MCC_INITVALUE)       ; // unit ohm/um
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDWMIN, 0.0, NULL, MCC_INITVALUE)      ; // unit ohm/um
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDS, 100.0, NULL, MCC_INITVALUE)       ; // unit ohm/um
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RDSMIN, 0.0, NULL, MCC_INITVALUE)      ; // unit ohm/um
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PRWG, 1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PRWB, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WR, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NRS, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NRD, 1.0, NULL, MCC_INITVALUE)         ;
	
/*----  IONIZATION CURRENT PARAMETERS  ------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPHA0, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ALPHA1, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BETA0, 30.0, NULL, MCC_INITVALUE)      ;

/*----  Gate-induced drain leakage model ----*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_AGIDL, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BGIDL, 2.3e9, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGIDL, 0.5 , NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EGIDL, 0.8 , NULL, MCC_INITVALUE)     ;

	mcc_initparam_quick(ptmodel, __MCC_QUICK_AGISL, mcc_getparam_quick( ptmodel, __MCC_QUICK_AGIDL ), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BGISL, mcc_getparam_quick( ptmodel, __MCC_QUICK_BGIDL ), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGISL, mcc_getparam_quick( ptmodel, __MCC_QUICK_CGIDL ), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EGISL, mcc_getparam_quick( ptmodel, __MCC_QUICK_EGIDL ), NULL, MCC_INITVALUE) ;

/*----  Gate-induced tunneling current model */
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AIGBACC, 0.43, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BIGBACC, 0.054, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIGBACC, 0.075, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NIGBACC, 1.0  , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AIGBINV, 0.35 , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BIGBINV, 0.03 , NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIGBINV, 0.006 , NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EIGBINV, 1.1   , NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NIGBINV, 3.0   , NULL, MCC_INITVALUE)  ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AIGC, ptmodel->TYPE == MCC_NMOS ? 0.054:0.31,
                           NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BIGC, ptmodel->TYPE == MCC_NMOS ? 0.054:0.024,
                           NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIGC, ptmodel->TYPE == MCC_NMOS ? 0.075:0.03,
                           NULL, MCC_INITVALUE)      ;
                           
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AIGSD, ptmodel->TYPE == MCC_NMOS ? 0.43:0.31, NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BIGSD, ptmodel->TYPE == MCC_NMOS ? 0.054:0.024, NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIGSD, ptmodel->TYPE == MCC_NMOS ? 0.075:0.03, NULL, MCC_INITVALUE) ;
        mcc_initparam_quick(ptmodel, __MCC_QUICK_DLCIG, mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT), mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LINT), MCC_INITVALUE) ;
       
        /* begin bsim 4.6.0 */
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AIGS, mcc_getparam_quick(ptmodel, __MCC_QUICK_AIGSD), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AIGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_AIGSD), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BIGS, mcc_getparam_quick(ptmodel, __MCC_QUICK_BIGSD), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BIGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_BIGSD), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIGS, mcc_getparam_quick(ptmodel, __MCC_QUICK_CIGSD), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CIGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_CIGSD), NULL, MCC_INITVALUE) ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DLCIGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_DLCIG), NULL, MCC_INITVALUE) ;
        /* end bsim 4.6.0 */

	mcc_initparam_quick(ptmodel, __MCC_QUICK_NIGC, 1.0 , NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_POXEDGE, 1.0 , NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PIGCD, 1.0 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NTOX, 1.0 , NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TOXREF, 3.0e-9, NULL, MCC_INITVALUE)   ;

/*---------  DYNAMIC PARAMETERS  ------------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_XPART, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGBO, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSL, 0.0, NULL, MCC_INITVALUE)        ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDL, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CKAPPAS, 0.6, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CKAPPAD, mcc_getparam_quick(ptmodel, __MCC_QUICK_CKAPPAS), 
                                      mcc_getparamexp_quick(ptmodel, __MCC_QUICK_CKAPPAS), 
                                      MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CLC, 1.0e-7, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CLE, 0.6, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT), 
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LINT),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DWC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WINT),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WINT),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VFBCV, -1.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NOFF, 1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VOFFCV, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_ACDE, 1.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MOIN, 15.0, NULL, MCC_INITVALUE)       ;

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDO) == MCC_INITVALUE) {
       double coxe = mcc_calc_coxe_bsim4 (ptmodel);
       if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DLC) == MCC_SETVALUE) 
            && (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) > 0.0)) {
            char buf[1024] ;
            char dlc[1024] ;
            char cgdl[1024] ;
            char *pt1 ;
            char *pt2 ;
            char *pt ;

            if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_DLC)) != NULL)
               sprintf(dlc,"%s",pt1) ;
            else
               sprintf(dlc,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC)) ;

            if((pt2 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_CGDL)) != NULL)
               sprintf(cgdl,"%s",pt2) ;
            else
               sprintf(cgdl,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDL)) ;

            if((pt1 != NULL) || (pt2 != NULL))
             {
              sprintf(buf,"%s * %g - %s",dlc,coxe,cgdl) ;
              pt = mbkstrdup(buf) ;
             }
            else
              pt = NULL ;

            mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) 
                         *coxe - mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDL)), pt, MCC_INITVALUE) ;
       if (mcc_getparam_quick(ptmodel, __MCC_QUICK_CGDO) < 0.0)
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, 0.0, NULL, MCC_INITVALUE) ;
       }
       else {
          char buf[1024] ;
          char xj[1024] ;
          char *pt1 ;
          char *pt ;
       
          if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_XJ)) != NULL)
             sprintf(xj,"%s",pt1) ;
          else
             sprintf(xj,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)) ;
       
          if(pt1 != NULL)
           {
            sprintf(buf,"0.6 * %s * %g",xj,coxe) ;
            pt = mbkstrdup(buf) ;
           }
          else
            pt = NULL ;
       
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGDO, (0.6*mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)
                         *coxe), pt, MCC_INITVALUE) ;
       }
    }

    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSO) == MCC_INITVALUE) {
       double coxe = mcc_calc_coxe_bsim4 (ptmodel);
       if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_DLC) == MCC_SETVALUE) 
            && (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) > 0.0)) {
            char buf[1024] ;
            char dlc[1024] ;
            char cgsl[1024] ;
            char *pt1 ;
            char *pt2 ;
            char *pt ;

            if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_DLC)) != NULL)
               sprintf(dlc,"%s",pt1) ;
            else
               sprintf(dlc,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC)) ;

            if((pt2 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_CGSL)) != NULL)
               sprintf(cgsl,"%s",pt2) ;
            else
               sprintf(cgsl,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)) ;

            if((pt1 != NULL) || (pt2 != NULL))
             {
              sprintf(buf,"%s * %g - %s",dlc,coxe,cgsl) ;
              pt = mbkstrdup(buf) ;
             }
            else
              pt = NULL ;

            mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, (mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC) 
                         *coxe - mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSL)), pt, MCC_INITVALUE) ;
       if (mcc_getparam_quick(ptmodel, __MCC_QUICK_CGSO) < 0.0)
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, 0.0, NULL, MCC_INITVALUE) ;
       }
       else {
          char buf[1024] ;
          char xj[1024] ;
          char *pt1 ;
          char *pt ;
       
          if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_XJ)) != NULL)
             sprintf(xj,"%s",pt1) ;
          else
             sprintf(xj,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)) ;
       
          if(pt1 != NULL)
           {
            sprintf(buf,"0.6 * %s * %g",xj,coxe) ;
            pt = mbkstrdup(buf) ;
           }
          else
            pt = NULL ;
       
           mcc_initparam_quick(ptmodel, __MCC_QUICK_CGSO, (0.6*mcc_getparam_quick(ptmodel, __MCC_QUICK_XJ)
                         *coxe), pt, MCC_INITVALUE) ;
       }
    }
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CF) == MCC_INITVALUE) {
        char buf[1024] ;
        char toxe[1024] ;
        char epsrox[1024] ;
        char *pt1,*pt2 ;
        char *pt ;

        if((pt1 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_TOXE)) != NULL)
           sprintf(toxe,"%s",pt1) ;
        else
           sprintf(toxe,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE)) ;
        if((pt2 = mcc_getparamexp_quick(ptmodel,__MCC_QUICK_EPSROX)) != NULL)
           sprintf(epsrox,"%s",pt2) ;
        else
           sprintf(epsrox,"%g",mcc_getparam_quick(ptmodel, __MCC_QUICK_EPSROX)) ;

        if (pt1 || pt2) 
         {
          sprintf(buf,"(2.0 * %s * %g / %g) * log (1.0+4.07e-7 / %s)",
                       epsrox,MCC_EPSO,MCC_PI,toxe) ;
          pt = mbkstrdup(buf) ;
         }
        else
          pt = NULL ;

        mcc_initparam_quick(ptmodel, __MCC_QUICK_CF, ((2.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_EPSROX)*MCC_EPSO/MCC_PI)
                      *log(1.0+4.0e-7/mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE))), 
                      pt, MCC_INITVALUE) ;
    }
	
/*----  High Speed / RF Model Paramaters ----*/
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XRCRG1, 12.0, NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XRCRG2, 1.0 , NULL, MCC_INITVALUE)     ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RBPB, 50.0 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RBPD, 50.0 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RBPS, 50.0 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RBDB, 50.0 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RBSB, 50.0 , NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_GBMIN, 1.0e-12, NULL, MCC_INITVALUE)   ;

/*---------  NOISE PARAMETERS  --------------*/
	if(ptmodel->TYPE == MCC_NMOS) {
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIA, 6.25e41, NULL, MCC_INITVALUE) ;
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIB, 3.125e26, NULL, MCC_INITVALUE);
	}
	else {
	    mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIA, 6.188e40, NULL, MCC_INITVALUE);
		mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIB, 1.5e25, NULL, MCC_INITVALUE)  ;
	}
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NOIC, 8.75, NULL, MCC_INITVALUE)       ;

	mcc_initparam_quick(ptmodel, __MCC_QUICK_EM, 4.1e7, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AF, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_EF, 1.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KF, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NTNOI, 1.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TNOIA, 1.5, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TNOIB, 3.5, NULL, MCC_INITVALUE)       ;

/*----  Layout-Dependent Parasitics    ------*/
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DMCG, 0.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DMCI, mcc_getparam_quick(ptmodel, __MCC_QUICK_DMCG), 
                                   mcc_getparamexp_quick(ptmodel, __MCC_QUICK_DMCG), 
                                   MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DMDG, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_DMCGT, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NF, 1.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_DWJ, mcc_getparam_quick(ptmodel, __MCC_QUICK_DWC),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_DWC),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MIN, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XGW, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XGL, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NGCON, 1.0, NULL, MCC_INITVALUE)      ;
	
/*----  Asymmetric Source/Drain Junction Diode -*/
	mcc_initparam_quick(ptmodel, __MCC_QUICK_IJTHSREV, 0.1, NULL, MCC_INITVALUE)   ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_IJTHDREV, mcc_getparam_quick(ptmodel, __MCC_QUICK_IJTHSREV),
                                       mcc_getparamexp_quick(ptmodel, __MCC_QUICK_IJTHSREV),
                                       MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_IJTHSFWD, 0.1, NULL, MCC_INITVALUE)   ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_IJTHDFWD, mcc_getparam_quick(ptmodel, __MCC_QUICK_IJTHSFWD),
                                       mcc_getparamexp_quick(ptmodel, __MCC_QUICK_IJTHSFWD),
                                       MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XJBVS, 1.0, NULL, MCC_INITVALUE)      ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_XJBVD, mcc_getparam_quick(ptmodel, __MCC_QUICK_XJBVS),
                                    mcc_getparamexp_quick(ptmodel, __MCC_QUICK_XJBVS),
                                    MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_BVS, 10.0, NULL, MCC_INITVALUE)       ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_BVD, mcc_getparam_quick(ptmodel, __MCC_QUICK_BVS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_BVS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_JSS, 1.0e-4, NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_JSD, mcc_getparam_quick(ptmodel, __MCC_QUICK_JSS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_JSS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_JSWS, 0.0  , NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_JSWD, mcc_getparam_quick(ptmodel, __MCC_QUICK_JSWS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_JSWS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_JSWGS, 0.0  , NULL, MCC_INITVALUE)     ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_JSWGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_JSWGS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_JSWGS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CJS, 5.0e-4, NULL, MCC_INITVALUE)    ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJD, mcc_getparam_quick(ptmodel, __MCC_QUICK_CJS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_CJS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MJS, 0.5   , NULL, MCC_INITVALUE)    ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_MJD, mcc_getparam_quick(ptmodel, __MCC_QUICK_MJS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_MJS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSWS, 0.33  , NULL, MCC_INITVALUE)    ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSWD, mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSWS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_MJSWS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSWS, 5.0e-10, NULL, MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSWD, mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSWS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_CJSWS),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSWGS, mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSWS),
                                     mcc_getparamexp_quick(ptmodel, __MCC_QUICK_CJSWS),
                                     MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_CJSWGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSWS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_CJSWS),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSWGS, mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSWS),
                                     mcc_getparamexp_quick(ptmodel, __MCC_QUICK_MJSWS),
                                     MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_MJSWGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSWS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_MJSWS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PBS, 1.0, NULL, MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PBD, mcc_getparam_quick(ptmodel, __MCC_QUICK_PBS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PBS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSWS, 1.0, NULL, MCC_INITVALUE) ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSWD, mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PBSWS),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSWGS, mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWS),
                                     mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PBSWS),
                                     MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_PBSWGD, mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWS),
                                     mcc_getparamexp_quick(ptmodel, __MCC_QUICK_PBSWS),
                                     MCC_INITVALUE) ; 

/*----  TEMPERATURE EFFECT PARAMETERS  ------*/
    // TNOM already done

	mcc_initparam_quick(ptmodel, __MCC_QUICK_UTE, -1.5, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KT1, -0.11, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KT1L, 0.0, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KT2,  0.022, NULL, MCC_INITVALUE)      ; // careful : sign change /t bsim3
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UA1, 1.0e-9, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UB1, -1.0e-18, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UC1, 
                  MCC_ROUND(mcc_getparam_quick(ptmodel,__MCC_QUICK_MOBMOD)) == 1 ? 0.067:0.025,
                  NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UD1, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TVOFF, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TVFBSDOFF, 0.0, NULL, MCC_INITVALUE)      ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_UP, 0.0, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LP, 1.0e-8, NULL, MCC_INITVALUE)    ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_AT, 3.3e4, NULL, MCC_INITVALUE)        ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PRT, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_NJS, 1.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_NJD, mcc_getparam_quick(ptmodel, __MCC_QUICK_NJS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_NJS),
                                  MCC_INITVALUE) ; 
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XTIS, 3.0, NULL, MCC_INITVALUE)        ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_XTID, mcc_getparam_quick(ptmodel, __MCC_QUICK_XTIS),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_XTIS),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TPB, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TPBSW, 0.0, NULL, MCC_INITVALUE)       ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TPBSWG, 0.0, NULL, MCC_INITVALUE)      ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TCJ, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_TCJSW, 0.0, NULL, MCC_INITVALUE)   ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TCJSWG, 0.0, NULL, MCC_INITVALUE)      ;

/*----  WIDTH AND LEGNTH PARAMETERS  --------*/

	mcc_initparam_quick(ptmodel, __MCC_QUICK_WL, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WLN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WW, 0.0, NULL, MCC_INITVALUE)          ;  
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WWN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WWL, 0.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LL, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LLN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LW, 0.0, NULL, MCC_INITVALUE)          ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LWN, 1.0, NULL, MCC_INITVALUE)         ;
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LWL, 0.0, NULL, MCC_INITVALUE)         ;
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LL),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LL),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LWC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LW),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LW),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_LWLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_LWL),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_LWL),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_WLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WL),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WL),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_WWC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WW),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WW),
                                  MCC_INITVALUE) ; 
    mcc_initparam_quick(ptmodel, __MCC_QUICK_WWLC, mcc_getparam_quick(ptmodel, __MCC_QUICK_WWL),
                                  mcc_getparamexp_quick(ptmodel, __MCC_QUICK_WWL),
                                  MCC_INITVALUE) ; 

/*----  BSIM4.3.0        PARAMETERS  --------*/

//----  Stress effects   
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SA, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SB, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SD, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SC, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SAREF, 1.0e-6, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_SBREF, 1.0e-6, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WLOD, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KVSAT, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TKU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LKU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WKU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PKU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LLODKU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WLODKU0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_KVTH0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LKVTH0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WKVTH0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_PKVTH0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LLODVTH, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_WLODVTH, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STK2, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LODK2, 1.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_STETA0, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LODETA0, 1.0, NULL, MCC_INITVALUE);
//----  Unified Current Saturation
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LAMBDA, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_VTL, 2.0e5, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_LC, 0.0, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_XN, 3.0, NULL, MCC_INITVALUE);
//----  Temperature Model
	mcc_initparam_quick(ptmodel, __MCC_QUICK_TEMPMOD, 0.0, NULL, MCC_INITVALUE);
//----  Holistic Thermal Noise
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RNOIA, 0.577, NULL, MCC_INITVALUE);
	mcc_initparam_quick(ptmodel, __MCC_QUICK_RNOIB, 0.37, NULL, MCC_INITVALUE);

/*----  BSIM4.5.0 PARAMETERS ----*/

//---- Well Proximity Effect

        mcc_initparam_quick(ptmodel, __MCC_QUICK_WEB,     0.0, NULL, MCC_INITVALUE );
        mcc_initparam_quick(ptmodel, __MCC_QUICK_WEC,     0.0, NULL, MCC_INITVALUE );
        mcc_initparam_quick(ptmodel, __MCC_QUICK_KVTH0WE, 0.0, NULL, MCC_INITVALUE );
        mcc_initparam_quick(ptmodel, __MCC_QUICK_K2WE,    0.0, NULL, MCC_INITVALUE );
        mcc_initparam_quick(ptmodel, __MCC_QUICK_KU0WE,   0.0, NULL, MCC_INITVALUE );
        mcc_initparam_quick(ptmodel, __MCC_QUICK_SCREF,   0.0, NULL, MCC_INITVALUE );

}

/*****************************************************************************\
Function : mcc_calcDW_bsim4

 Calcul du parametre de shrink DW d'un modele pour obtenir Weff       
 DW sera utilise pour le calcul du courant                            
\*****************************************************************************/
double mcc_calcDW_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                         double L, double W)
{
    double Lprinted, Wprinted, dW ;
    double XL, XW, WINT, WL, WLN, WW, WWN, WWL ;
    double NF;
    double mcc_dw = 0.0;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    WINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_WINT)    ;
    WL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WL)      ;
    WW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_WW)      ;
    WWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWL)     ;
    WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
    WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;
    NF = mcc_get_nf (lotrsparam,ptmodel);

    Lprinted   = L + XL ;
    Wprinted   = W/NF + XW ;
    
    dW         = WINT + WL/pow(Lprinted, WLN) + WW/pow(Wprinted, WWN) 
                 + WWL/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;
    mcc_dw = (W + NF*(XW - 2.0*dW -W)) / NF ;

    return mcc_dw ;
}

/*****************************************************************************\
Function : mcc_calcDWCJ_bsim4

 Calcul du parametre de shrink DWC d'un modele pour obtenir Wactive   
 DWC sera utilise pour le calcul des capacites                        
\*****************************************************************************/
double mcc_calcDWCJ_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, 
                           double L, double W)
{
    double Lprinted, Wprinted, dWcv ;
    double XL, XW, DWJ, WLC, WLN, WWC, WWN, WWLC ;
    double NF, mcc_dwc = 0.0;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    DWJ  = mcc_getparam_quick(ptmodel, __MCC_QUICK_DWJ)     ;
    WLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLC)     ;
    WWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWC)     ;
    WWLC = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWLC)    ;
    WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
    WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;
    NF = mcc_get_nf (lotrsparam,ptmodel);

    Lprinted   = L + XL ;
    Wprinted   = W/NF + XW ;
    
    dWcv       = DWJ + WLC/pow(Lprinted, WLN) + WWC/pow(Wprinted, WWN) 
                 + WWLC/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;

    mcc_dwc = (W + NF * (XW - 2.0*dWcv - W)) / NF;

    return mcc_dwc;
}

/************************************************************************/
/* Calcul du parametre de shrink DLC d'un modele pour obtenir Lactive   */
/* DLC sera utilise pour le calcul des capacites                        */
/************************************************************************/
double mcc_calcDL_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                           double L, double W)
{
  double Lprinted, Wprinted, dL, NF ;
  double XL, XW, LINT, LL, LLN, LW, LWN, LWL ;
  
  NF = mcc_get_nf (lotrsparam,ptmodel);
  XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
  XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
  LINT = mcc_getparam_quick(ptmodel, __MCC_QUICK_LINT)    ;
  LL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LL)      ;
  LW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_LW)      ;
  LWL  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWL)     ;
  LLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLN)     ;
  LWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWN)     ;

  Lprinted   = L + XL ;
  Wprinted   = W/NF + XW ;
  
  dL         = LINT + LL/pow(Lprinted, LLN) + LW/pow(Wprinted, LWN) 
               + LWL/(pow(Lprinted,LLN) * pow(Wprinted, LWN)) ;

  return (XL - 2.0 * dL)  ;
}


/************************************************************************/
/* Calcul du parametre de shrink DLC d'un modele pour obtenir Lactive   */
/* DLC sera utilise pour le calcul des capacites                        */
/************************************************************************/
double mcc_calcDLC_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                           double L, double W)
{
    double Lprinted, Wprinted, dLcv ;
    double XL, XW, DLC, LLC, LLN, LWC, LWN, LWLC ;
    double NF;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    DLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_DLC)     ;
    LLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLC)     ;
    LWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWC)     ;
    LWLC = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWLC)    ;
    LLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LLN)     ;
    LWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_LWN)     ;

    NF = mcc_get_nf (lotrsparam,ptmodel);

    Lprinted   = L + XL ;
    Wprinted   = W/NF + XW ;
    
    dLcv       = DLC + LLC/pow(Lprinted, LLN) + LWC/pow(Wprinted, LWN) 
                 + LWLC/(pow(Lprinted,LLN) * pow(Wprinted, LWN)) ;

    return(XL - 2.0 * dLcv) ;
}


/*****************************************************************************\
Function : mcc_calcDWC_bsim4

 Calcul du parametre de shrink DWC d'un modele pour obtenir Wactive   
 DWC sera utilise pour le calcul des capacites                        
\*****************************************************************************/
double mcc_calcDWC_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,double L, double W)
{
    double Lprinted, Wprinted, dWcv ;
    double XL, XW, DWC, WLC, WLN, WWC, WWN, WWLC ;
    double NF, mcc_dwc = 0.0;
    
    XL   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XL)      ;
    XW   = mcc_getparam_quick(ptmodel, __MCC_QUICK_XW)      ;
    DWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_DWC)     ;
    WLC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLC)     ;
    WWC  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWC)     ;
    WWLC = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWLC)    ;
    WLN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WLN)     ;
    WWN  = mcc_getparam_quick(ptmodel, __MCC_QUICK_WWN)     ;
    NF = mcc_get_nf (lotrsparam,ptmodel);

    Lprinted   = L + XL ;
    Wprinted   = W/NF + XW ;
    
    dWcv       = DWC + WLC/pow(Lprinted, WLN) + WWC/pow(Wprinted, WWN) 
                 + WWLC/(pow(Lprinted, WLN) * pow(Wprinted, WWN)) ;

    mcc_dwc = (W + NF * (XW - 2.0*dWcv - W)) / NF;

    return mcc_dwc;
}

/******************************************************************************\
 Function mcc_calcVTH_bsim4 ()

 Fonction qui calcule les tesions de seuil VTH d'un transistor en fct de L, W et de la temperature 
 LA FONCTION DE CALCUL DE VTH CONSIDERE LE MODELE BSIM3V3 et EGALEMENT Vbstrue  = 0               
 La variation en fonction de la temperature est : VTH(T) = VTH(Tnom) + DVTH(T)                    
                                                                                                  
 CAS PARTICULIER POUR LE CALCUL DE VTH D'UN TRANSISTOR P :                                        
 Pour calculer VTH d'un transistor P, il faut se mettre dans les memes conditions qu'un NMOS      
 1) On inverse le parametre VTH0 du PMOS                                                          
 2) On inverse les tensions appliquees aux bornes du PMOS <=> vbs et vds deviennent positifs      
 3) On calcule donc VTH du PMOS comme pour un NMOS puis on inverse le resultat final car VTHP < 0 

\******************************************************************************/
double	mcc_calcVTH_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                           double temp, double vbstrue, double vds, int capa,
                           elp_lotrs_param *lotrsparam, int mcclog)
{
  int calcul_dimension;
  static mcc_modellist *previous_model=NULL ;
  static double previous_L=0.0 ;
  static double previous_W=0.0 ;
  static int previous_NF=0.0 ;
  static chain_list *previous_longkey=NULL;

  // Parametres generaux ou presents dans les parametres du modele

  static int UPDATEPHI = 0 ;

  static double binunit ;
  static double KT2, Cox ;
  static double tnomk, TNOM ;
  static double VTH0, K1, K2 ,K1ox, K2ox, K3, K3B, TOXE, TOXM, W0, LPE0, LPEB ;
  static double DVT0, DVT1, DVT2, DSUB ;
  static double NDEP, ETA0, ETAB, DVT0W, DVT1W, DVT2W, KT1, KT1L ;
  static int VERSION, TEMPMOD ;

  // Parametres intermediaires pour le calcul de VTH

  static double T, VbiT ;  
  static double sqrtPhisT ;
  static double Leff, Weff, DVth0T, Xdep ;
  static double Xdep0 ;
  static double Lt, Ltw, Lt0, phis, vbseff ;

  // Param pour Vbs
  static double EPSROX, DVTP0,DVTP1 ;

  // finalement notre tension de seuil
  static double Vt, Cdsc_Term, CIT,NFACTOR, n;
  static double CDSC,CDSCD,CDSCB;
  static double VTHfinal ; 
  static int need_stress_bs43 = 0;
  double T0,T1,T2,T3,T4,T5,T9,c0,c1;
  double Theta0, theta0vb0, Delt_vth, Delt_vthw, DIBL_Sft_dVd;
  int NF, unbias_mode=0, use_saved_calc=0 ;

  if (vbstrue==0 && vds==0 && capa==1) unbias_mode=1;

  /******* Obtention des parametres du modele non binnable ********/
  NF = mcc_get_nf (lotrsparam, ptmodel) ;
  if ( !mcc_is_same_model (ptmodel,L,W,NF,previous_model,previous_L,previous_W,previous_NF,lotrsparam,previous_longkey) ) {
    calcul_dimension = 1 ;
  }
  else {
#ifdef OPTIM9
    if (!unbias_mode) {
      if (mbk_cmpdouble(lotrsparam->MCC_SAVED[__MCC_LAST_VBS], vbstrue, 1e5) == 0) {
        if (mbk_cmpdouble(lotrsparam->MCC_SAVED[__MCC_LAST_VDS], vds, 1e5)==0 && lotrsparam->MCC_SAVED[__MCC_SAVED_VTHFINAL]!=ELPINITVALUE) {
          return lotrsparam->MCC_SAVED[__MCC_SAVED_VTHFINAL];
        }
#ifdef OPTIM16
        else use_saved_calc=1;
#endif
      }
    }
    else {
      if (lotrsparam->MCC_SAVED[__MCC_SAVED_VTHFINAL_UNBIAS]!=ELPINITVALUE) {
        return lotrsparam->MCC_SAVED[__MCC_SAVED_VTHFINAL_UNBIAS];
      }
    }
#endif
    calcul_dimension = 0 ;
  }
  need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);

  VERSION = MCC_ROUND (10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION));
  TEMPMOD = MCC_ROUND (mcc_getparam_quick(ptmodel,__MCC_QUICK_TEMPMOD));

  /********** Effectives Dimensions COMPUTATION *******/
  if (calcul_dimension == 1) {
    TOXE   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE)  ;
    TOXM   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM)  ;
    EPSROX = mcc_getparam_quick(ptmodel, __MCC_QUICK_EPSROX)  ;
    Leff = mcc_calc_leff_bsim4 (ptmodel,lotrsparam,L,W);
    Weff = mcc_calc_weff_bsim4 (ptmodel,lotrsparam,L,W);

    // Logs
    if ( avt_islog(2,LOGMCC) ) {
      avt_log(LOGMCC,2,"--------- [Begin mcc_calcVTH_bsim4] ---------\n");
      avt_log(LOGMCC,2,"model %s, L=%g, W=%g, temp=%g, vbs=%g, vds=%g\n",
                              ptmodel->NAME,L,W,temp,vbstrue,vds);
      if ( ptmodel->SUBCKTNAME ) avt_log(LOGMCC,2,"Subcktname: %s\n",ptmodel->SUBCKTNAME);
      avt_log(LOGMCC,2,"Leff = %g, Weff = %g\n",Leff,Weff);
    }


    /*-----------------------------------------------*/
    /*   COMPUTATION OF BINNING PARAMETERS           */
    /*-----------------------------------------------*/
    binunit = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT)       ;   
    LPEB  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_LPEB,mcclog)    ;
    K1    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K1,mcclog)      ;
    K3    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K3,mcclog)      ;
    K3B   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K3B,mcclog)     ;
    W0    =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_W0,mcclog)      ;
    LPE0  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_LPE0,mcclog)    ;
    DVT0  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT0,mcclog)    ;
    DVT1  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT1,mcclog)    ;
    DVT2  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT2,mcclog)    ;
    DVT0W =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT0W,mcclog)   ;
    DVT1W =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT1W,mcclog)   ;
    DVT2W =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVT2W,mcclog)   ;
    DSUB  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DSUB,mcclog)    ;
    NDEP  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NDEP,mcclog)    ; 
    if (NDEP <= 1.0e20) NDEP = NDEP *1.0e6 ;
    ETAB  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_ETAB,mcclog)    ;
    KT1   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KT1,mcclog)     ;
    KT2   =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KT2,mcclog)     ;
    KT1L  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KT1L,mcclog)    ;
    CDSC  =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSC,mcclog)    ;
    CDSCB =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSCB,mcclog)    ;
    CDSCD =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CDSCD,mcclog)    ;
    DVTP0 =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVTP0,mcclog)    ;
    DVTP1 =  binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_DVTP1,mcclog)    ;
    CIT   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CIT,mcclog) ;
    NFACTOR = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NFACTOR,mcclog);
    K1ox = K1*TOXE/TOXM ;
  }

  ETA0 = mcc_getparam_quick(ptmodel, __MCC_QUICK_ETA0);   
  if ( need_stress_bs43 ) {
    ETA0 = mcc_update_Kstress_eta0_bs43(ptmodel, lotrsparam, ETA0, L, W, NF);
  }
  ETA0 = binningval_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_ETA0,ETA0,mcclog);
  K2 = mcc_compute_k2(ptmodel, Leff, Weff, L, W, lotrsparam, need_stress_bs43, binunit);
  K2ox = K2*TOXE/TOXM ;

  /******* Premiere serie de calculs pour les parametres intermediaires ********/

  TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
  tnomk = TNOM + MCC_KELVIN;
  T = temp + MCC_KELVIN;

  if (VERSION >= 45 && TEMPMOD >= 2) {
    Vt = mcc_calc_vt (TNOM);
  }
  else Vt = mcc_calc_vt (temp);

  if (calcul_dimension) {
    if (UPDATEPHI == 0) {
      phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,binunit,mcc_getparam_quick(ptmodel,__MCC_QUICK_TNOM));
      VbiT = mcc_calc_vbi_bsim4  (ptmodel,mcc_getparam_quick(ptmodel,__MCC_QUICK_TNOM),Leff,Weff,calcul_dimension);
    }
    else if (UPDATEPHI == 1){
      phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,binunit,temp);
      VbiT = mcc_calc_vbi_bsim4  (ptmodel,temp,Leff,Weff,calcul_dimension);
    }
  }

  if(ptmodel->TYPE == MCC_PMOS) {
    vds = fabs(vds) ;
    vbstrue = -vbstrue ;
  }

  vbseff  = mcc_calc_vbseff_bsim4 (ptmodel,vbstrue,Leff,Weff, temp,K2,calcul_dimension,lotrsparam);

  if (vbseff <= 0.0) sqrtPhisT  = sqrt(phis - vbseff); 
  else sqrtPhisT = (phis*sqrt(phis))/(phis + 0.5 * vbseff);

  #ifdef OPTIM16
  if (use_saved_calc)
  {
    Theta0 = lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_THETA0];
    Delt_vth = lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DELT_VTH];
    Delt_vthw = lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DELT_VTHW];
    DIBL_Sft_dVd = lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DIBL_SFT_DVD];
    DVth0T = lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DVTH0T];
  }
  else
  #endif
  {
    Cox = mcc_calc_coxe_bsim4 (ptmodel);

    /*----------------------------------------*/
    if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_VTH0) == MCC_SETVALUE) {
      VTH0 = mcc_compute_vth0 ( ptmodel, Leff, Weff, L, W, lotrsparam, need_stress_bs43, binunit);
    }
    else
      VTH0 = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VFB,mcclog) + phis + K1ox*sqrt(phis) ;

    if (ptmodel->TYPE == MCC_PMOS) VTH0 = -VTH0;

    DVth0T     = (KT1+KT1L/Leff+KT2*vbseff)*(T/tnomk-1.0) ;
    Xdep0      = sqrt((2.0*MCC_EPSSI*phis)/(MCC_Q*NDEP)) ;
    Xdep       = Xdep0*sqrtPhisT/sqrt(phis);

    T0 = DVT2 * vbseff;
    if ( T0 > -0.5 )
      T1 = 1.0 + T0;
    else {
      T4 = 1.0 / (3.0 + 8.0 * T0);
      T1 = (1.0 + 3.0 * T0) * T4;
    }
    Lt         = sqrt(MCC_EPSSI*Xdep/Cox)*T1;

    Lt0        = sqrt(MCC_EPSSI*Xdep0/Cox) ;

    T0 = DVT2W * vbseff;
    if ( T0 > -0.5 )
      T1 = 1.0 + T0;
    else {
      T4 = 1.0 / (3.0 + 8.0 * T0);
      T1 = (1.0 + 3.0 * T0) * T4;
    }
    Ltw        = sqrt(MCC_EPSSI*Xdep/Cox)*T1;

    /********              Equation finale de VTH                        ********/

    T0 = DVT1 * Leff/Lt;
    if ( T0 < MCC_EXP_THRESHOLD ) {
      T1 = exp(T0);
      T2 = T1 - 1.0;
      T3 = T2 * T2;
      T4 = T3 + 2.0 * T1 * MCC_MIN_EXP;
      Theta0 = T1 / T4;
    }
    else {
      Theta0 = 1.0 / (MCC_MAX_EXP - 2.0);
    }
    Delt_vth = DVT0 * Theta0 * (VbiT - phis);
  }
  if ( !capa ) {
    c0 = sqrt (1.0+LPEB/Leff) * (K1ox * sqrtPhisT - K1 * sqrt(phis));
    Cdsc_Term = (CDSC + CDSCD*vds + CDSCB*vbseff ) * Theta0;
    n = 1.0 + (NFACTOR*MCC_EPSSI/Xdep + (Cdsc_Term + CIT)) / Cox;

    if ( DVTP0 > 0.0 )
      c1 = n*Vt * log( Leff / (Leff+DVTP0*(1.0+exp(-DVTP1*vds)))) ;
    else
      c1 = 0.0 ;
  }
  else {
    c0 = 0.0;
    c1 = 0.0;
  }

  #ifdef OPTIM16
  if (!use_saved_calc)
  #endif
  {
    T0 = DSUB * Leff/Lt0;
    if ( T0 < MCC_EXP_THRESHOLD ) {
      T1 = exp(T0);
      T2 = T1 - 1.0;
      T3 = T2 * T2;
      T4 = T3 + 2.0 * T1 * MCC_MIN_EXP;
      theta0vb0 = T1 / T4;
    }
    else {
      theta0vb0 = 1.0 / (MCC_MAX_EXP - 2.0);
    }

    T3 = ETA0 + ETAB * vbseff;
    if (T3 < 1.0e-4) { 
      T9 = 1.0 / (3.0 - 2.0e4 * T3);
      T3 = (2.0e-4 - T3) * T9;
    }

    DIBL_Sft_dVd = T3 * theta0vb0;

    T0 = DVT1W * Leff*Weff/Ltw;
    if ( T0 < MCC_EXP_THRESHOLD ) {
      T1 = exp(T0);
      T2 = T1 - 1.0;
      T3 = T2 * T2;
      T4 = T3 + 2.0 * T1 * MCC_MIN_EXP;
      T5 = T1 / T4;
    }
    else {
      T5 = 1.0 / (MCC_MAX_EXP - 2.0);
    }

    Delt_vthw = DVT0W * T5 * (VbiT - phis);

  }
  VTHfinal =  VTH0 + DVth0T 
            + c0 - K2ox * vbseff 
            + K1ox*sqrt(phis)*(sqrt(1.0+LPE0/Leff)-1.0) 
            + (K3 + K3B * vbseff)*TOXE*phis/(Weff+W0)
            - Delt_vth - Delt_vthw
            - DIBL_Sft_dVd * vds
            - c1;

  if (ptmodel->TYPE == MCC_PMOS)
    VTHfinal = - VTHfinal;

  // log 
  if ( mcclog == MCC_DRV_LOG ) {
    avt_log(LOGMCC,2,"[Main params] VTH0=%g, DVth0T=%g, c0=%g, K2ox=%g, vbseff=%g, K1ox=%g\n",VTH0,DVth0T,c0,K2ox,vbseff,K1ox);
    avt_log(LOGMCC,2,"[Main params] sqrt(phis)=%g, TOXE=%g, phis=%g\n",sqrt(phis),TOXE,phis);
    avt_log(LOGMCC,2,"[Main params] Theta0=%g, Delt_vth=%g, Delt_vthw=%g, c1=%g, DIBL_Sft_dVd=%g\n",Theta0,Delt_vth,Delt_vthw,c1,DIBL_Sft_dVd);
    avt_log(LOGMCC,2,"[params] n=%g, Cdsc_Term=%g, Ltw=%g, Lt0=%g, Lt=%g\n",n,Cdsc_Term,Ltw,Lt0,Lt);
    avt_log(LOGMCC,2,"[params] Xdep=%g, Xdep0=%g, DVth0T=%g, Cox=%g, vbseff=%g\n",Xdep,Xdep0,DVth0T,Cox,vbseff);
    avt_log(LOGMCC,2,"[Sub params] sqrtPhisT =%g, VbiT=%g, tnomk=%g, K2=%g, ETA0=%g\n",sqrtPhisT,VbiT,tnomk,K2,ETA0);
    avt_log(LOGMCC,2,"[Sub params] TOXE=%g, TOXM=%g, EPSROX=%g\n",TOXE,TOXM,EPSROX);
    avt_log(LOGMCC,2,"### VTHfinal = %g ###\n",VTHfinal);
    avt_log(LOGMCC,2,"--------- [ end  mcc_calcVTH_bsim4] ---------\n\n");
  }

  previous_model = ptmodel ;
  previous_L = L ;
  previous_W = W ;
  previous_NF = NF ;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }

  if (!unbias_mode)
  {
    lotrsparam->MCC_SAVED[__MCC_SAVED_VTHFINAL] = VTHfinal;
    lotrsparam->MCC_SAVED[__MCC_LAST_VBS] = vbstrue;
    lotrsparam->MCC_SAVED[__MCC_LAST_VDS] = vds;
    lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DVTH0T] = DVth0T;
    lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_THETA0] = Theta0;
    lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DELT_VTH] = Delt_vth;
    lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DELT_VTHW] = Delt_vthw;
    lotrsparam->MCC_SAVED[__MCC_SAVED_CALC_DIBL_SFT_DVD] = DIBL_Sft_dVd;
  }
  else
  {
    lotrsparam->MCC_SAVED[__MCC_SAVED_VTHFINAL_UNBIAS] = VTHfinal;  
  }

  return VTHfinal ;
}


/******************************************************************************\
Function : mcc_calcIDS_bsim4

 Fonction qui calcule le courant Ids pour BSIM3V3 en fonction 
 des parametres du modele 
 ainsi que de Vbs, Vgs, Vds,VTH, W, L, temp                                               
 NB sur les valeurs effectives utilisees: Vgseff  = Vgs si param NGATE non fourni         
                                          Vgsteff = (Vgs-Vth)eff                          
                                                                                          
 CAS DU CALCUL DE IDS D'UN TRANSISTOR P :                                                 
 ELDO calcul ids comme pour un transistor N, il ne prend que
 les valeurs positives de VTH ainsi que des tensions Vgs et Vds.
 ELDO renvoi finalement l'oppose du courant calcule dans le cas d'un transistor P.                                                                                 

\******************************************************************************/
void mcc_check_saved_resetstep2( double Vbseff, double Vgsteff, int calculdim, elp_lotrs_param *lotrsparam)
{
        
  if (mbk_cmpdouble(lotrsparam->MCC_SAVED[__MCC_LAST_VBSEFF], Vbseff, 1e5)!=0
  || mbk_cmpdouble(lotrsparam->MCC_SAVED[__MCC_LAST_VGSTEFF], Vgsteff, 1e5)!=0
  || calculdim)
  {
    lotrsparam->MCC_SAVED[__MCC_SAVED_VDSAT]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_ESAT]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_UEFFT]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_ABULK]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_RDS]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_VSATT]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_LAMBDA]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_RFACTOR]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_SAVED_WEFFV]=ELPINITVALUE;
    lotrsparam->MCC_SAVED[__MCC_LAST_VBSEFF]=Vbseff;
    lotrsparam->MCC_SAVED[__MCC_LAST_VGSTEFF]=Vgsteff;
  }
}

double mcc_calcIDS_bsim4 (mcc_modellist *ptmodel, 
                          double Vbstrue, double Vgs, double Vds, 
                          double W, double L, 
                          double Temp,elp_lotrs_param *lotrsparam)
{
  static int UPDATEPHI = 0 ; 
  // new parameters
  static double F,FPROUT,PVAG,PCLM,VsatT,Vb;
  static double Cclm,Coxe,Coxeff,Litl,lambda,phis;
  static double NDEP,Xdep0,PDITS,PDITSL,PDITSD;
  static double BINUNIT,PDIBLCB,PDIBLC1,PDIBLC2 ;
  static double DROUT,Lt0,Thetarout,PSCBE1,PSCBE2;
  static mcc_modellist *previous_model=NULL ;
  static double previous_L=0.0,Leff,Weff,previous_W=0.0;
  static chain_list *previous_longkey=NULL;
  static int previous_NF=0 ;
  int NF;
  static int    calcul_dimension,bs43=0 ;
 
  double Ids_1, Ids_2, Ids_3,Ids_4, Ids_5, diffVds,Rfactor,Weff_v ;
 
  /*********************************************************/
  /* parametres principaux dans l'expression de Ids        */
  /*********************************************************/
   
  static double Vdsat   ;                   
  static double Vth     ;                    // TENSION DE SEUIL
  static double Ids     ;                    // Courant de drain
  static double Rds     ;                    // Channel resistance
  static double Vdseff  ;                    // Effective vds
  static double Va      ;                    // EARLY voltage
  static double Vascbe  ;                    // EARLY voltage due to substrate current
  static double Ids0    ;                    // Linear drain current in the channel
 
  static double Vbseff ;      // Pour recalculer Vbs avec precision (faible influence)    
  static double Vadits,Esat,Abulk,TNOM,XJ,Qch0,EsatL ;
  static double Vgsteff, Vt, Vasat,Vadibl,Vaclm,ueffT; 
  static double LAMBDA,K2,VTL,XN,LC;
  int need_stress_bs43=0;
  double c0,r,MM,VsHD,VsBT;
  double T0,T1,T2,T3,T4,T5;
  mcc_debug_internal_trans *trans ;

  if( MCC_DEBUG_INTERNAL == YES ) {
    if( ptmodel->TYPE == MCC_NMOS ) 
      trans = & MCC_DEBUG_INTERNAL_DATA.TN ;
    else
      trans = & MCC_DEBUG_INTERNAL_DATA.TP ;
  }
  else
    trans = NULL ;

  /****************************************************************************\
                           VALEURS GENENRALES                       
  \****************************************************************************/
 
  /****************************************************************************/
  /*                         OBTENTION PARAMETRES DU MODEL                        */
  /****************************************************************************/
  if (Vds == 0.0)
    return 0.0;
 
  NF = mcc_get_nf (lotrsparam,ptmodel);
  if ( !mcc_is_same_model (ptmodel,L,W,NF,previous_model,previous_L,previous_W,previous_NF,lotrsparam, previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;
  
  /*****************************************************************/
  /*               Calcul de Leff et Weff                          */ 
  /*****************************************************************/
  if (calcul_dimension == 1) {
    Leff = mcc_calc_leff_bsim4 (ptmodel,lotrsparam,L,W);
    Weff = mcc_calc_weff_bsim4 (ptmodel,lotrsparam,L,W);
    TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
    LAMBDA = mcc_getparam_quick(ptmodel, __MCC_QUICK_LAMBDA);
    if (MCC_ROUND (10.0*mcc_getparam_quick(ptmodel,__MCC_QUICK_VERSION)) >= 43)
      bs43 = 1;
  }
  
  /*********************************************************/
  /* CALCUL DE VTH  et inversion des tensions pour le PMOS */
  /*********************************************************/
  
  Vth = mcc_calcVTH_bsim4 (ptmodel, L, W, Temp, Vbstrue, Vds,0,lotrsparam,MCC_NO_LOG) ;
  if (ptmodel->TYPE == MCC_PMOS) {
  	Vth = -Vth ;
    Vbstrue = -Vbstrue ;
    Vgs = fabs(Vgs) ;
    Vds = fabs(Vds) ;
  }
 
  /*****************************************************************/
  /*                         BINNING PROCESS                       */
  /*****************************************************************/
  if (calcul_dimension == 1) {
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;   
    FPROUT = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_FPROUT,MCC_NO_LOG) ;
    PVAG   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PVAG,MCC_NO_LOG) ;
    PCLM   = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PCLM,MCC_NO_LOG) ;
    PDIBLCB= binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PDIBLCB,MCC_NO_LOG) ;
    PDIBLC1= binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PDIBLC1,MCC_NO_LOG) ;
    PDIBLC2= binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PDIBLC2,MCC_NO_LOG) ;
    DROUT  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DROUT,MCC_NO_LOG) ;
    NDEP  =  binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG)    ; 
    if (NDEP <= 1.0e20)
        NDEP = NDEP *1.0e6 ;
    PDITS  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PDITS,MCC_NO_LOG) ;
    PDITSL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PDITSL,MCC_NO_LOG) ;
    PDITSD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PDITSD,MCC_NO_LOG) ;
    PSCBE1 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PSCBE1,MCC_NO_LOG) ;
    PSCBE2 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PSCBE2,MCC_NO_LOG) ;
    XJ     = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XJ,MCC_NO_LOG) ;
    VTL    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VTL,MCC_NO_LOG) ;
    XN    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XN,MCC_NO_LOG) ;
    LC    = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_LC,MCC_NO_LOG) ;
  }
  need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);
  K2 = mcc_compute_k2 ( ptmodel, 
                        Leff, Weff,
                        L, W,
                        lotrsparam,
                        need_stress_bs43,
                        BINUNIT
                      );
  
 
  /*---------------------------------------------------------------*/
  /*                         BEGIN EQUATIONS                       */
  /*---------------------------------------------------------------*/
 
  /*---------------------------------------------------------------*/
  /*               Efficace voltage                                */
  /*---------------------------------------------------------------*/
  Vbseff  = mcc_calc_vbseff_bsim4 (ptmodel,
                                   Vbstrue,
                                   Leff, Weff,
                                   Temp, K2,
                                   calcul_dimension,lotrsparam); 
  Vgsteff = mcc_calc_vgsteff_bsim4 (ptmodel,
                                    Vth, Vbseff, 
                                    Vds, Vgs,
                                    Temp,
                                    Leff, Weff,
                                    L, W,
                                    NULL,
                                    lotrsparam,
                                    calcul_dimension);

  mcc_check_saved_resetstep2(Vbseff, Vgsteff, calcul_dimension, lotrsparam);

  mcc_calc_vdseff_bsim4 ( ptmodel,
                          &Vdseff, &Vdsat, 
                          &Esat, &ueffT,
                          &Abulk,&Rds,
                          &VsatT, &lambda,
                          &Rfactor, &Weff_v,
                           Vth, Vbseff, 
                           Vds, Vgsteff,
                           Temp, L, W,
                           lotrsparam,
                           calcul_dimension);

  diffVds = Vds - Vdseff;
 
  if ( calcul_dimension ) {
    if (UPDATEPHI == 0)
      phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,TNOM);
    else if (UPDATEPHI == 1)
      phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,Temp);
    Vt = mcc_calc_vt (Temp);
  }
  
  //--- Ids0  ---------------------------------------------------->
  Coxe = mcc_calc_coxe_bsim4 (ptmodel);

  Coxeff = mcc_calc_coxeff_bsim4 (ptmodel,
                                  Vgsteff, Temp,
                                  Leff, Weff,
                                  L, W,
                                  0.0,0.0,0.0,
                                  BINUNIT, 0,
                                  NULL,
                                  lotrsparam,
                                  calcul_dimension);
  Qch0 = Coxeff*Vgsteff;
  Vb = (Vgsteff + 2.0*Vt) / Abulk;

  Litl = sqrt(MCC_EPSSI*XJ/Coxe) ;

  // velocity overshoot
  EsatL=Esat;
  if ( (mcc_getparamtype_quick(ptmodel,__MCC_QUICK_LAMBDA) == MCC_SETVALUE) && LAMBDA > 0.0 ) {
    c0 = pow ( 1.0 + diffVds/(Esat*Litl),2.0);
    EsatL *= (1.0 + LAMBDA/(Leff*ueffT) * (c0-1.0)/(c0+1.0));
  }

  Ids0 = (Weff_v*ueffT*Qch0*Vdseff*(1.0-Vdseff/(2.0*Vb))) 
         / (Leff*(1.0+Vdseff/(EsatL*Leff)));

  if ( Vdseff != 0.0 ) // ADD-ON : la simplification est legale
                       // car Vdseff = 0 lq Ids0 = 0
    Ids_1 = Ids0/(1.0+(Rds*Ids0/Vdseff)); // 1ere composante de Ids
  else
    Ids_1 = Ids0;

  //--- Cclm  ---------------------------------------------------->
  if ( FPROUT <= 0.0 )
    F = 1.0;
  else
    F = 1.0/(1.0+FPROUT*sqrt (Leff) / (Vgsteff+2.0*Vt));

  if ((PCLM > 0.0) && (diffVds > 1.0e-10)) // ngspice src
    Cclm = (F*(1.0+PVAG*Vgsteff/(EsatL*Leff)) * (1.0+Rds*Ids_1/Vdseff) 
           * (Leff+Vdsat/Esat) ) / (PCLM * Litl);
  else
    Cclm = MCC_MAX_EXP;
  
  //--- Vaclm, Vasat, Va ----------------------------------------->
  Vaclm = fabs ( Cclm*diffVds); // from ngspice src
 
  Vasat = ( EsatL*Leff + Vdsat + 2.0*Rfactor*Vgsteff*
           (1.0-Abulk*Vdsat/ (2.0*Vgsteff+2.0*Vt)) )
          / ( Rfactor*Abulk-1.0+2.0/lambda );

  Va = Vasat + Vaclm;
  
  //--- Vadibl --------------------------------------------------->
  Xdep0 = sqrt((2.0*MCC_EPSSI*phis)/(MCC_Q*NDEP)) ;
  Lt0 = sqrt(MCC_EPSSI*Xdep0/Coxe) ;
  T0 = DROUT*Leff/Lt0;
  if ( T0 < MCC_EXP_THRESHOLD ) {
    T1 = exp(T0);
    T2 = T1 - 1.0;
    T3 = T2 * T2;
    T4 = T3 + 2.0 * T1 * MCC_MIN_EXP;
    T5 = T1 / T4;
  }
  else {
    T5 = 1.0 / (MCC_MAX_EXP - 2.0);
  }
  Thetarout = PDIBLC1 * T5 + PDIBLC2;

  if ( Thetarout > MCC_MIN_EXP)
    Vadibl = (Vgsteff+2.0*Vt) / (Thetarout*(1.0+PDIBLCB*Vbseff)) 
            * (1.0- Abulk*Vdsat / (Abulk*Vdsat+Vgsteff+2.0*Vt))
            * (1.0 + PVAG*Vgsteff/(EsatL*Leff));
  else
    Vadibl = MCC_MAX_EXP;
  
  //--- Vadits --------------------------------------------------->
  if ( PDITS > MCC_MIN_EXP ){
    if( PDITSD*Vds > MCC_EXP_THRESHOLD ){ //from Berkeley
      Vadits = (F* ( 1.0+(1.0+PDITSL*Leff)*MCC_MAX_EXP )) / PDITS;
    }else{
      Vadits = (F* ( 1.0+(1.0+PDITSL*Leff)*exp(PDITSD*Vds) )) / PDITS;
    }
  }else{
    Vadits = MCC_MAX_EXP;
  }
  
  //--- Vascbe --------------------------------------------------->
  if ( PSCBE2 > 0.0 ){
    if(diffVds > PSCBE1*Litl/MCC_EXP_THRESHOLD){ //from Berkeley
      Vascbe = (Leff/PSCBE2) * exp((PSCBE1*Litl)/(diffVds)) ;
    }else{
      Vascbe = MCC_MAX_EXP * (Leff/PSCBE2);
    }
  }else{
    Vascbe = MCC_MAX_EXP;
  }
  
  // =>  Calcul de Ids 
  Ids_2 = 1.0 + log (Va/Vasat) / Cclm ;    // 2eme composante de Ids
  Ids_3 = 1.0 + diffVds/Vadibl ;    // 3eme composante de Ids
  Ids_4 = 1.0 + diffVds/Vadits ;    // 4eme composante de Ids
  Ids_5 = 1.0 + diffVds/Vascbe ;    // 5eme composante de Ids

  Ids = Ids_1 * Ids_2 * Ids_3 * Ids_4 * Ids_5;
    
  if( trans ) {
    trans->I1 = Ids_1 ;
    trans->I2 = Ids_2 ;
    trans->I3 = Ids_3 ;
    trans->I4 = Ids_4 ;
    trans->I5 = Ids_5 ;
    trans->I6 = 1.0 ;
  }
  
  // Source End Velocity Limit
  if ( (mcc_getparamtype_quick(ptmodel,__MCC_QUICK_VTL) == MCC_SETVALUE) && VTL > 0.0 ) {
    MM = 3.0;
    VsHD = Ids/(Qch0*Weff_v);
    r = Leff/(XN*Leff+LC);
    VsBT = (1.0-r)/(1.0+r)*VTL;
    c0 = 1.0/ pow ((1.0+pow(VsHD/VsBT,2.0*MM)),1.0/(2.0*MM));

    Ids =  Ids*c0;
    if( trans )
      trans->I6 = c0 ;
  }

  if( trans ) 
    trans->IDS= Ids ;

  Ids = Ids * NF ;

  if (ptmodel->TYPE == MCC_PMOS) 
    Ids = -Ids ;
 
  previous_model = ptmodel ;
  previous_L = L ;
  previous_W = W ;
  previous_NF = NF ;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
  
  return Ids ;
}

/******************************************************************************\
 Intrinsic Capacitance
\******************************************************************************/

/********************************************************************************************/
/* Fonction qui calcule la capacite de grille CGP d'un transistor                           */
/********************************************************************************************/
double mcc_calcCGP_bsim4 ( mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double vgx, 
                           double L, double W, double *ptQov) 
{
    double Qov = 0.0 ;
    double cgp = 0.0 ;
    double CGDO = 0.0 ;
    double CGDL = 0.0 ;
    double CAPMOD, CKAPPAD ;
    double Vgov, binunit ;
    double CF, Leff, Weff, NF ;
   
    if ( ptmodel->TYPE == MCC_PMOS )
      vgx = -vgx;

    NF = mcc_get_nf (lotrsparam,ptmodel);
    binunit =mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ; 
    CAPMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_CAPMOD) ;
    //CGDO   = mcc_getprm(ptmodel, "CGDO")     ;
    //CGDL   = mcc_getprm(ptmodel, "CGDL")     ;
    CGDO   = mcc_getprm_quick(ptmodel, __MCC_GETPRM_CGDO)     ;
    CGDL   = mcc_getprm_quick(ptmodel, __MCC_GETPRM_CGDL)     ;
    
    /* dimensions effectives */
    Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W) ; 
    Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W) ; 

    /* Binning parameters */
    CF     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CF,MCC_NO_LOG)          ; 
    CKAPPAD = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CKAPPAD,MCC_NO_LOG)    ;
    if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGDL) == MCC_SETVALUE)
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGDL,MCC_NO_LOG)      ;
    else if (mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGD1) == MCC_SETVALUE)
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGD1,MCC_NO_LOG)      ;
    else if(mcc_getparamtype_quick(ptmodel, __MCC_QUICK_CGSL) == MCC_SETVALUE)
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGSL,MCC_NO_LOG)      ;
    else
        CGDL = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CGS1,MCC_NO_LOG)      ;

    /* Formules de Berkeley et non ELDO */

    /***************************/
    /* CAPMOD = 0              */
    /***************************/
    if (MCC_ROUND (CAPMOD) == 0) {
        cgp = CF + CGDO ;
        Qov = cgp*vgx;
    }

    /***************************/
    /* CAPMOD = 1 ou 2         */
    /***************************/
    else {
        Vgov = 0.5*(vgx+0.02-sqrt(pow((vgx+0.02), 2.0)+4.0*0.02)) ;
 
        Qov = (CF+CGDO)*vgx + CGDL*(vgx-Vgov-(CKAPPAD/2.0)*(-1.0+sqrt(1.0-4.0*Vgov/CKAPPAD))) ;
        if ( vgx != 0.0 )
          cgp = fabs(Qov/vgx) ;
        else
          cgp = 0.0 ;
    }
    if ( ptQov ) {
      if ( ptmodel->TYPE == MCC_PMOS )
        Qov = -Qov;
      *ptQov = NF*Qov;
    }
    return cgp*NF ;
}

/******************************************************************************\
 Junction Capacitance : 
-----------------------

       Cbs = ASeff * Cjbs + PSeff * Cjbssw + Weffcj*NF*Cjbsswg
       
\******************************************************************************/

/******************************************************************************\
   Fonction qui calcule la capacite de drain CDS d'un transistor            
   Contribution surfacique.

   CDS = CJS                                                               
\******************************************************************************/
double mcc_calcCDS_bsim4 (mcc_modellist *ptmodel, double temp, double vbx1, double vbx2)
{
  double cds = 0.0 ;
  double Tnom, dT, TPB, TCJ;
  double MJS, CJS, PBS ;
  double CJ_T = 0.0 ;
  double PB_T = 0.0 ;
  //double aire=0.0;
                              
  if( ptmodel->TYPE == MCC_TRANS_P ) {
    vbx1 = -vbx1 ;
    vbx2 = -vbx2 ;
  }

  Tnom    = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) ;
  dT      = temp - Tnom;

  CJS = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJS)    ;
  MJS = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJS)    ;
  PBS = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBS)    ;
  TPB = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPB)    ;
  TCJ = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJ)    ;
  if ((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_OPTACM) == MCC_SETVALUE) &&
        (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) >= 10.0)) {
    PB_T = PBS ;
    CJ_T = CJS ;
  }
  else {
    PB_T = PBS - TPB*dT ;
    CJ_T = CJS * (1.0 + TCJ * dT);
    if ( PB_T  < 0.1 )
      PB_T = 0.1;
    if ( CJ_T < 0.0 )
      CJ_T = 0.0;
  }

  /*
  if ( avt_islog (2,LOGMCC) ) {
    aire = (ptmodel->TYPE==MCC_NMOS) ? MCC_ADN:MCC_ADP;
    aire *= 1.0e-6;
    cds = CJ_T*pow((1.0-vbx1/PB_T), -MJS) ;
    avt_log(LOGMCC,2,"Model %s : cds(%g) = %8.3g \n",ptmodel->NAME,vbx1,cds*aire*1.0e12);
    cds = CJ_T*pow((1.0-vbx2/PB_T), -MJS) ;
    avt_log(LOGMCC,2,"Model %s : cds(%g) = %8.3g \n",ptmodel->NAME,vbx2,cds*aire*1.0e12);
  }
  */

  cds = mcc_integfordw ( CJ_T, vbx1, vbx2, PB_T, MJS);
  
  return cds ;
}

/******************************************************************************\
   Fonction qui calcule la capacite de drain CDP d'un transistor               
   Contribution perimetrique.

   CDP = CJSW                                                                 
\******************************************************************************/
double mcc_calcCDP_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                          double temp, double vbx1, double vbx2)
                          
{
  double cdp = 0.0 ;
  double Tnom = 0.0 ;
  double dT = 0.0 ;
  double MJSWS = 0.0 ; 
  double CJSWS = 0.0 ; 
  double PBSWS = 0.0 ;
  double CJSW_T = 0.0 ;
  double PBSW_T = 0.0 ;
  double TPBSW;
  double TCJSW,NF;
  //double perim;
  
  if( ptmodel->TYPE == MCC_TRANS_P ) {
    vbx1 = -vbx1 ;
    vbx2 = -vbx2 ;
  }
  Tnom    = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
  dT      = temp - Tnom ;
  NF = mcc_get_nf (lotrsparam,ptmodel);
  
  CJSWS = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSWS)    ;
  MJSWS = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSWS)    ;
  PBSWS = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWS)    ;
  TPBSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPBSW)    ;
  TCJSW = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJSW)    ;
  if((mcc_getparamtype_quick(ptmodel, __MCC_QUICK_OPTACM) == MCC_SETVALUE) &&
          (mcc_getparam_quick(ptmodel, __MCC_QUICK_ACM) >= 10.0)){
      PBSW_T = PBSWS ;
      CJSW_T = CJSWS ;
  }
  else { 
    PBSW_T = PBSWS - TPBSW * dT;
    CJSW_T = CJSWS *(1.0  + TCJSW * dT); // erreur dans la doc
    if ( PBSW_T < 0.1 )
      PBSW_T = 0.1;
    if ( CJSW_T < 0.0 )
      CJSW_T = 0.0;
  }
  /* influence de vbx */
  /*
  if ( avt_islog(2,LOGMCC) ) {
    perim = (ptmodel->TYPE==MCC_NMOS) ? MCC_PDN:MCC_PDP;
    perim *= 1.0e-6;
    cdp = CJSW_T*pow((1.0-vbx1/PBSW_T), -MJSWS) ;
    avt_log(LOGMCC,2,"Model %s : cdp(%g) = %8.3g \n",ptmodel->NAME,vbx1,cdp*perim*1.0e12);
    cdp = CJSW_T*pow((1.0-vbx2/PBSW_T), -MJSWS) ;
    avt_log(LOGMCC,2,"Model %s : cdp(%g) = %8.3g \n",ptmodel->NAME,vbx2,cdp*perim*1.0e12);
  }
  */


  cdp = mcc_integfordw ( CJSW_T, vbx1, vbx2, PBSW_T, MJSWS);

  return cdp  ;
}

/********************************************************************************************/
/* Fonction qui calcule la capacite de grille CDW d'un transistor                           */
/* CDW = CSW =  CGP  + Capa (contribion en largeur face a la grille)                        */ 
/********************************************************************************************/
double mcc_calcCDW_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                          double temp, double vbx1, double vbx2, double vgx, double L, double W) 
{
  double cdw = 0.0 ;
  double CJSWGS ;
  double Tnom ;
  double MJSWGS, PBSWGS; 
  double Cbxg = 0.0 ;
  /* param pour CJGATE */
  double dT ;
  double PbswT = 0.0 ;
  /* contribution de CGP */
  double Cgp = 0.0;
  double TPBSWG;
  double TCJSWG;
  double CjswgsT;
  double PERMOD,NF,cdp=0.0;
  double cdw_cdp;
  double wact, weffcj ;

  Tnom  = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) ;
  dT    = temp - Tnom;

  CJSWGS = mcc_getparam_quick(ptmodel, __MCC_QUICK_CJSWGS)  ;
  NF = mcc_get_nf (lotrsparam,ptmodel);

  if ((PERMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_PERMOD)) != 0.0) 
    cdp = mcc_calcCDP_bsim4 (ptmodel, lotrsparam, temp, vbx1, vbx2);

  /* Contribution de la capacite Cgp (overlap capacitance) */
  Cgp = mcc_calcCGP_bsim4 (ptmodel, lotrsparam,vgx, L, W, NULL) ;
  wact   = W + mcc_calcDWC_bsim4( ptmodel, lotrsparam, L, W );
  weffcj = W + mcc_calcDWCJ_bsim4( ptmodel, lotrsparam, L, W );
  if( weffcj > 0.0 )
    Cgp = Cgp * wact/weffcj ;

  /* Debut du calcul de la capacite cdw en fonction de diolev */
  MJSWGS = mcc_getparam_quick(ptmodel, __MCC_QUICK_MJSWGS) ;
  PBSWGS = mcc_getparam_quick(ptmodel, __MCC_QUICK_PBSWGS) ;
  TPBSWG = mcc_getparam_quick(ptmodel, __MCC_QUICK_TPBSWG) ;
  TCJSWG = mcc_getparam_quick(ptmodel, __MCC_QUICK_TCJSWG) ;
  /* temp effect */
  PbswT = PBSWGS - TPBSWG * dT;
  CjswgsT = CJSWGS * (1.0 + TCJSWG*dT);
  if ( PbswT < 0.1 )
    PbswT = 0.1;
  if ( CjswgsT < 0.0 )
    CjswgsT = 0.0;

  /* vbx effect */
  /*
  if ( avt_islog(2,LOGMCC) ) {
    if ( ptmodel->TYPE == MCC_NMOS ) 
      weffcj = MCC_XWN*MCC_WN+MCC_DWCJN;
    else 
      weffcj = MCC_XWP*MCC_WP+MCC_DWCJP;
    weffcj *= 1.e-6;
    Cbxg = NF*CJSWGS*pow((1.0-vbx1/PBSWGS), -MJSWGS) - cdp ;
    avt_log(LOGMCC,2,"Model %s : Weffcj = %8.3g \n",ptmodel->NAME,weffcj);
    avt_log(LOGMCC,2,"Model %s : cbxg(%g) = %8.3g \n",ptmodel->NAME,vbx1,Cbxg*weffcj*1.0e12);
    Cbxg = NF*CJSWGS*pow((1.0-vbx2/PBSWGS), -MJSWGS) - cdp ;
    avt_log(LOGMCC,2,"Model %s : cbxg(%g) = %8.3g \n",ptmodel->NAME,vbx2,Cbxg*weffcj*1.0e12);
  }
  */

  if( ptmodel->TYPE == MCC_TRANS_P ) {
    vbx1 = -vbx1 ;
    vbx2 = -vbx2 ;
  }
  Cbxg = mcc_integfordw ( CJSWGS, vbx1, vbx2, PBSWGS, MJSWGS);

  cdw  = (Cgp + NF*Cbxg) ; 
  
  cdw_cdp = (cdw-NF*cdp);

  MCC_CBXG = cdw_cdp-Cgp;
  return cdw_cdp ;
}

/******************************************************************************\
Function : mcc_calcQint_bsim4

 Fonction qui calcule les charges intrinseques  d'un transistor              
 En considerant que le transistor fonctionne en lin/sature                    
\******************************************************************************/
void mcc_calcQint_bsim4 (mcc_modellist *ptmodel, double L, double W,
                         double temp, double vgs, double vbs, double vds,
                         double *ptQg, double *ptQs, double *ptQd,
                         double *ptQb,elp_lotrs_param *lotrsparam)
{
  int calcul_dimension;
  double T0,T1,T3;
  static int UPDATEPHI = 0 ;
  static double Qg = 0.0 ,Qb,Qs,Qd;
  static double Coxe, TOXE;
  static double Leff, Weff ;
  static double CAPMOD, /*VFB, */VFBCV, CLC, CLE ;
  static double Abulk, Abulk_p,K1, K1ox,K2ox, TOXM, phis, A0, XJ, Xdep, B0, B1, KETA ;
  static double Vth, Vth_bias,vbseff, vdsat_cv, Vt0,VtT, Vgsteff_cv ; 
  static double K2,K3B,PhisT, NDEP ;
  static double TNOM,NSD,Vgseff, NGATE, binunit,AGS ;
  /* Vgsteff -> n */
  static double W0 ;
  /* Accumulation charge        */
  static double Qacc, vgbacc;
  /* Substrate depletion charge */
  static double Qsub0, dQsub ;
  /* Inversion charge           */
  static double Qinv,La;
  static double MOIN,LPEB ;
  static double phib,BCT,Coxeff ;
  static double delta, V3,V4, Vfbeff, V1, Vcveff, Vdsat ;
  static double IGCMOD,VTH0,Vfbzb,Vgsteff,XPART ;
  static double F_doping;
  static mcc_modellist *previous_model = NULL ;
  static double previous_L=0.0 ;
  static double previous_W=0.0 ;
  double c0,c1,c2,c3,c4,c5;
  double vbseff_cv;
  int xpart_int;
  int need_stress_bs43=0;
  static int previous_NF=0;
  static chain_list *previous_longkey;
  int NF = mcc_get_nf (lotrsparam,ptmodel);
  
  if ( !mcc_is_same_model (ptmodel,L,W,NF,previous_model,previous_L,previous_W,previous_NF,lotrsparam,previous_longkey) )
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;

  if ( calcul_dimension )
    XPART = mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART);
  if ( XPART < 0.0) {
    mcc_affect_value (ptQb,0.0);
    mcc_affect_value (ptQg,0.0);
    mcc_affect_value (ptQd,0.0);
    mcc_affect_value (ptQs,0.0);
    return ;
  }
  
 if ( calcul_dimension ) {
  /* dimensions effectives */
  Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W) ; 
  Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W) ; 

  /* Obtention des parametres du modele */
  binunit= mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;
  IGCMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_IGCMOD) ;
  TNOM   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM) ;
  CAPMOD = mcc_getparam_quick(ptmodel, __MCC_QUICK_CAPMOD) ;
  TOXE   = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE) ;
  Coxe   = mcc_calc_coxe_bsim4 (ptmodel);
  LPEB   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_LPEB,MCC_NO_LOG);
  VFBCV  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_VFBCV,MCC_NO_LOG)    ;
  CLC    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CLC,MCC_NO_LOG)      ;
  W0     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_W0,MCC_NO_LOG) ;
  AGS    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_AGS,MCC_NO_LOG) ;
  CLE    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_CLE,MCC_NO_LOG)      ;
  TOXM   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_TOXM,MCC_NO_LOG)     ;
  A0     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_A0,MCC_NO_LOG)       ;
  B0     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_B0,MCC_NO_LOG)       ;
  B1     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_B1,MCC_NO_LOG)       ;
  KETA   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_KETA,MCC_NO_LOG)     ;
  NDEP   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NDEP,MCC_NO_LOG)     ;
  if (NDEP <= 1.0e20)
    NDEP *= 1.0e6 ;
  K1     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG)       ;
  K3B    = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_K3B,MCC_NO_LOG)      ;
  XJ     = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_XJ,MCC_NO_LOG)       ;
  NGATE  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NGATE,MCC_NO_LOG)    ;
  if (NGATE <= 1.0e23)
    NGATE *= 1.0e6 ;
  NSD  = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_NSD,MCC_NO_LOG) ;
  NSD *= 1.0e6 ;
  MOIN   = binning_quick(ptmodel, binunit, Leff, Weff, __MCC_QUICK_MOIN,MCC_NO_LOG)     ;

  /* Potentiel de surface phis */
  Vt0 = mcc_calc_vt (TNOM);
  VtT = mcc_calc_vt (temp);
  if (UPDATEPHI == 0) 
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,binunit,TNOM);
  else 
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,binunit,temp);

  K1ox = K1*(TOXE/TOXM) ;
 }
 need_stress_bs43 = mcc_need_stress_bs43 (ptmodel,lotrsparam);
 K2 = mcc_compute_k2 ( ptmodel, 
                       Leff, Weff,
                       L, W,
                       lotrsparam,
                       need_stress_bs43,
                       binunit
                     );
 K2ox = K2*(TOXE/TOXM) ;
 VTH0 = mcc_compute_vth0 ( ptmodel, 
                           Leff, Weff,
                           L, W,
                           lotrsparam,
                           need_stress_bs43,
                           binunit
                         );

 // Obtain biased Vth
 Vth_bias = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, vbs, vds,0,lotrsparam,MCC_NO_LOG) ;
 
 if (ptmodel->TYPE == MCC_PMOS) {
   VTH0 = -VTH0;
   vbs = -vbs;
   vgs = fabs(vgs);
   vds = fabs(vds);
   Vth_bias = - Vth_bias;
 }
 

  /* vbseff */
  vbseff  = mcc_calc_vbseff_bsim4 (ptmodel,
                                   vbs, 
                                   Leff, Weff,
                                   temp, K2,
                                   1,lotrsparam); 
  if (vbseff <= 0.0) {
    PhisT  = phis - vbseff ; 
    vbseff_cv = vbseff;
  }
  else {
    PhisT  = pow((phis*sqrt(phis))/(phis + 0.5 * vbseff), 2.0) ;
    vbseff_cv = phis-PhisT;
  }
 
  //------------------------ calcul de Abulk ---------------------
  Xdep = sqrt((2.0*MCC_EPSSI/(MCC_Q*NDEP)))*sqrt(PhisT)/sqrt(phis) ; 
  F_doping = sqrt (1.0+LPEB/Leff)*K1ox/(2.0*sqrt (PhisT))
             + K2ox - K3B*TOXE*phis/(Weff+W0);

  //-> Obtain Vth
  Vth = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, 0.0, 0.0,1,lotrsparam,MCC_NO_LOG) ;
  if (ptmodel->TYPE == MCC_PMOS) 
    Vth = -Vth;

  Vgsteff = mcc_calc_vgsteff_bsim4 (ptmodel,
                                    Vth_bias, vbseff_cv, 
                                    vds, vgs,
                                    temp,
                                    Leff, Weff,
                                    L, W,
                                    &Vgseff,
                                    lotrsparam,
                                    1);
  
  Abulk = (1.0 + F_doping * ( (A0*Leff/(Leff+2.0*sqrt (XJ*Xdep)))
          * (1.0 - AGS*Vgsteff * pow ( Leff/(Leff + 2.0*sqrt (XJ*Xdep)) ,2.0) )
          + B0/(Weff+B1) ))/(1.0 + KETA*vbseff);
  
  if (Abulk < 0.1) {
    T0 = 1.0 / (3.0 - 20.0 * Abulk);
    Abulk = (0.2 - Abulk) * T0;
  }

  La = L + mcc_calcDLC_bsim4 (ptmodel, lotrsparam, L, W) ;
  Abulk_p = Abulk* ( 1.0 + pow (CLC/La,CLE));
  
  /******************************************************************************/
  /*                        CAPMOD = 0                                          */
  /******************************************************************************/
  if(MCC_ROUND(CAPMOD) == 0) {
    if(MCC_ROUND (mcc_getparam_quick(ptmodel, __MCC_QUICK_VFBFLAG)) == 1)
       Vth = VTH0 + K1ox*sqrt(PhisT) - K1ox*sqrt(phis) ; 
    else
       Vth = VFBCV + phis + K1ox*sqrt(PhisT) ; /* VFBCV = VFB par defo et utilisation  */
                                               /* de ce param unikement pour capmod =0 */

    vdsat_cv = (vgs-Vth)/Abulk_p ;
  
    //===> subthreshold region
      if ( vgs < Vth ) {
        // SUBTRESHOLD
        c0 = K1ox*K1ox;
        Qb = - Coxe*c0/2.0 * (-1.0+sqrt (1.0+(4.0*(vgs-VFBCV-vbs)/(c0))));
        /*
        if (ptmodel->TYPE == MCC_PMOS) {
          Qg = -Qg; 
          Qb = -Qb;
        }
        */
        mcc_affect_value (ptQb,Qb*NF);
        mcc_affect_value (ptQg,-Qb*NF);
        mcc_affect_value (ptQd,0.0);
        mcc_affect_value (ptQs,0.0);
      }
      else if ( vgs >= Vth ) {
        // STRONG INVERSION
        vdsat_cv = (vgs-Vth)/Abulk_p ;
        xpart_int = MCC_ROUND ( XPART*10.0 );

        if ( xpart_int == 5 ) { //--> XPART = 0.5 : 50/50
          if ( vds < vdsat_cv ) {
            // Linear 
            c1 = vgs-Vth-Abulk_p*vds/2.0;
            c2 = vgs - Vth;
            c3 = Abulk_p*vds;
            c4 = 1.0-Abulk_p;
            
            Qg = Coxe*(vgs-VFBCV-phis-vds/2.0+c3*vds/(12.0*c1));
            Qb = Coxe*(VFBCV-Vth+phis+c4*vds/2.0-c4*c3*vds/(12.0*c1));
            Qinv = -Coxe*(c2-c3/2.0+c3*c3/(12.0*c1));
            Qd = Qs = 0.5*Qinv;
          }
          else {
            // Saturation
            Qg = Coxe*(vgs - VFBCV - phis - vdsat_cv/3.0) ;
            Qb = Coxe*(VFBCV+phis-Vth+vdsat_cv*(1.0-Abulk_p)/3.0);
            Qs = Qd = -Coxe*(vgs-Vth)/3.0;
          }
        }
        else if ( xpart_int < 5 ) {                         //--> XPART = 0 : 40/60
          if ( vds < vdsat_cv ) {
            // Linear 
            c1 = vgs-Vth-Abulk_p*vds/2.0;
            c2 = vgs - Vth;
            c3 = Abulk_p*vds;

            Qg = Coxe*(vgs - VFBCV - phis - vds/2.0 
                 + (Abulk_p*vds*vds /(12.0*c1)));
            Qb = Coxe * (VFBCV-Vth+phis+ (1.0-Abulk_p)*vds/2.0 -
                 (1.0-Abulk_p)*(Abulk_p*vds*vds)/ (12.0*c1));
            Qd = -Coxe * (c2/2.0 - c3 /2.0 + c3
                * ( c2*c2/6.0 - c3*c2/8.0 + c3*c3/40.0 ) / ( c1*c1 ) );
            Qs = - (Qg+Qb+Qd);
          }
          else {
            // Saturation
            Qg = Coxe*(vgs - VFBCV - phis - vdsat_cv/3.0) ;
            Qb = Coxe*(VFBCV+phis-Vth + vdsat_cv*(1.0-Abulk_p)/3.0);
            Qd = -4.0/15.0*Coxe * (vgs-Vth);
            Qs = - (Qg+Qb+Qd);
          }
        }
        else {                      //--> XPART = 1 : 0/100
          if ( vds < vdsat_cv ) {
            // Linear 
            c1 = vgs-Vth-Abulk_p*vds/2.0;
            c2 = vgs - Vth;
            c3 = Abulk_p*vds;

            Qg = Coxe*(vgs-VFBCV-phis-vds/2.0+c3*vds/(12.0*c1));
            Qb = Coxe*(VFBCV-Vth+phis+vds*(1.0-Abulk_p)/2.0 - 
                 (1.0-Abulk_p)*c3*vds/ (12.0*c1));
            Qd = Coxe* (3.0*c3/4.0-c2/2.0-c3*c3/(8.0*c1));
            Qs = - (Qg+Qb+Qd);
          }
          else {
            // Saturation
            Qg = Coxe*(vgs - VFBCV - phis - vdsat_cv/3.0) ;
            Qb = Coxe*(VFBCV+phis-Vth + vdsat_cv*(1.0-Abulk_p)/3.0);
            Qd = 0.0;
            Qs = - (Qg+Qb);
          }
        }
        /*
        if (ptmodel->TYPE == MCC_PMOS) {
          Qg = -Qg; 
          Qb = -Qb;
          Qs = -Qs;
          Qd = -Qd;
        }
        */
        mcc_affect_value (ptQb,Qb*NF);
        mcc_affect_value (ptQg,Qg*NF);
        mcc_affect_value (ptQd,Qd*NF);
        mcc_affect_value (ptQs,Qs*NF);
      }
  }
  else {
    /*******************************************************/
    /* Common parameter Vgsteff_cv for capmod = 1, 2, 3    */
    /* Expression de Vgseff en fonction du parametre NGATE */
    /*******************************************************/
    xpart_int = MCC_ROUND ( XPART*10.0 );

    //-> Obtain Vgsteff_cv
    Vgsteff_cv = mcc_calc_vgsteffcv_bsim4 (ptmodel,
                                           Vth_bias, vbseff_cv, 
                                           vds, vgs,
                                           temp,
                                           Leff, Weff,
                                           L, W,
                                           lotrsparam,
                                           calcul_dimension
                                           );
    
    /* Vfbeff */
    delta = 2.0e-2 ;
    Vfbzb = Vth - phis- K1*sqrt(phis);
    V3 = Vfbzb-(Vgseff-vbs)-delta ;
    if(Vfbzb <= 0.0)
      Vfbeff = Vfbzb-0.5*(V3+sqrt(pow(V3, 2.0)-4.0*delta*Vfbzb)) ;
    else 
      Vfbeff = Vfbzb-0.5*(V3+sqrt(pow(V3, 2.0)+4.0*delta*Vfbzb)) ;

    /***************************************************************************/
    /*                        CAPMOD = 1                                       */
    /***************************************************************************/
    if (MCC_ROUND (CAPMOD) == 1) {
      Qacc = -Coxe*(Vfbeff-Vfbzb);
      vdsat_cv = Vgsteff_cv/Abulk_p ;
      V4 = vdsat_cv - vds - delta;
      Vcveff = vdsat_cv - 0.5 * (V4 + sqrt (V4*V4+4.0*delta*vdsat_cv)) ;
      Qsub0 = -Coxe*pow(K1ox, 2.0)/2.0*( -1.0+sqrt(1.0
               +4.0*(Vgseff-Vfbeff-vbseff_cv-Vgsteff)/pow(K1ox, 2.0)) ) ;

      Qinv = -Coxe*( Vgsteff_cv-Abulk_p*Vcveff/2.0+pow((Abulk_p*Vcveff), 2.0)
              /(12.0*(Vgsteff_cv-Abulk_p*Vcveff/2.0)) ) ;

      dQsub = Coxe*((1.0-Abulk_p)*Vcveff/2.0-(1-Abulk_p)*Abulk_p*pow(Vcveff, 2.0)
              /(12.0*(Vgsteff_cv-Abulk_p*Vcveff/2))) ;

      Qb = Qacc+Qsub0+dQsub;
      Qg = -(Qinv+Qb) ;
      c1 = Abulk_p*Vcveff;
      c2 = 2.0*pow((Vgsteff_cv-Abulk_p*Vcveff/2.0),2.0);
      c3 = pow(c1,2.0);
      c4 = pow(c1,3.0);

      if ( xpart_int == 5 )            //--> XPART = 0.5 : 50/50
        Qs = Qd = 0.5*Qinv;
      else if ( xpart_int < 5 ) {                         //--> XPART = 0 : 40/60
        Qs = -Coxe/c2*(pow(Vgsteff_cv,3.0)-4.0/3.0*Vgsteff_cv*Vgsteff_cv
             *c1+2.0/3.0*Vgsteff_cv*c3-2.0/15.0*c4); 
        Qd = -Coxe/c2*(pow(Vgsteff_cv,3.0)-5.0/3.0*Vgsteff_cv*Vgsteff_cv
             *c1+Vgsteff_cv*c3-1.0/5.0*c4); 
      }
      else {                      //--> XPART = 1 : 0/100
        c2 = (Vgsteff_cv-Abulk_p*Vcveff/2.0);

        Qs = -(Coxe/2)*(Vgsteff_cv+c1/2.0-c3/(12.0*c2));
        Qd = -(Coxe/2)*(Vgsteff_cv-3.0*c1/2.0+c3/(4.0*c2));
      }
    }
    /***************************************************************************/
    /*                        CAPMOD = 2 : Charge Thickness Model              */
    /***************************************************************************/
    else { 
      double Coxeff_acc_dep;
      /* Xdc of inversion charge */
      phib = phis ;
      Coxeff = mcc_calc_coxeff_bsim4 (
                                      ptmodel,
                                      Vgsteff_cv, temp,
                                      Leff, Weff,
                                      L, W,
                                      Vgseff,vbseff_cv,Vfbeff,
                                      binunit, 1,
                                      &Coxeff_acc_dep,
                                      lotrsparam,
                                      calcul_dimension
                                      );
      
      /* Body Charge Thickness in inversion charge */

      BCT = VtT*log( (Vgsteff_cv*(Vgsteff_cv+2.0*K1ox*sqrt(phib)))
              /(MOIN*K1ox*K1ox*VtT)+1.0 ) ;

      Vdsat = (Vgsteff_cv-BCT)/Abulk_p ;
      V1 = Vdsat-vds-delta ;
      if ( V1 >= 0.0 )
        Vcveff = Vdsat -0.5*(V1+sqrt(pow(V1, 2.0)+4.0*delta*Vdsat)) ;
      else 
        Vcveff = Vdsat *(1.0 - 2.0*delta/ (sqrt(V1*V1+4.0*delta*Vdsat) -V1));
      
      vgbacc = Vfbeff-Vfbzb;
      
      Qacc =-Coxeff_acc_dep*vgbacc ;

      T0 = 0.5 * K1ox;
      T3 = Vgseff - Vfbeff - vbseff_cv - Vgsteff_cv;
      if (K1ox == 0.0)
        T1 = 0.0;
      else if (T3 < 0.0)
        T1 = T0 + T3 / K1ox;
      else
        T1 = sqrt(T0 * T0 + T3);

      Qsub0 = -Coxeff_acc_dep * K1ox * (T1 - T0);
      
      Qinv = -Coxeff*(Vgsteff_cv-BCT-0.5*Abulk_p*Vcveff
             + ( pow((Abulk_p*Vcveff), 2.0)/(12.0*(Vgsteff_cv-BCT-Abulk_p*Vcveff/2.0)) )) ;
      
      dQsub = Coxeff*( (1.0-Abulk_p)*Vcveff/2.0-(1.0-Abulk_p)*Abulk_p*pow(Vcveff, 2.0)
              /(12.0*(Vgsteff_cv-BCT-Abulk_p*Vcveff/2.0)) ) ;

      Qb = dQsub + Qsub0 + Qacc ;
      Qg = -(Qinv+Qb) ;

      c1 = Vgsteff_cv - BCT;
      c2 = pow (c1,2.0);
      c3 = pow (c1,3.0);
      c4 = Abulk_p*Vcveff;
      c5 = c1 - c4/2.0;

      if ( xpart_int == 5 )           //--> XPART = 0.5 : 50/50
        Qs = Qd = 0.5*Qinv;
      else if ( xpart_int < 5 ) {                         //--> XPART = 0 : 40/60
        Qs = -Coxeff/(2.0*c5*c5)*(c3-4.0/3.0*c2*c4
              +2.0/3.0*c1*c4*c4-2.0/15.0*pow(c4,3.0));
        Qd = -Coxeff/(2.0*c5*c5)*(c3-5.0/3.0*c2*c4
              +c1*c4*c4-1.0/5.0*pow(c4,3.0));
      }
      else {                      //--> XPART = 1 : 0/100
        Qs = -Coxeff/2.0*(c1+0.5*c4-c4*c4/(12.0*c5));
        Qd = -Coxeff/2.0*(c1-1.5*c4+c4*c4/(4.0*c5));
      }
    }
    /*
    if (ptmodel->TYPE == MCC_PMOS) {
      Qg = -Qg; 
      Qb = -Qb;
      Qs = -Qs;
      Qd = -Qd;
    }
    */
    mcc_affect_value (ptQb,Qb*NF);
    mcc_affect_value (ptQg,Qg*NF);
    mcc_affect_value (ptQd,Qd*NF);
    mcc_affect_value (ptQs,Qs*NF);
  }
  previous_model = ptmodel ;
  previous_L = L ;
  previous_W = W ;
  previous_NF = NF ;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
}

/******************************************************************************\
 Fonction qui calcule la capacite de grille/drain CGD d un transistor                  
\******************************************************************************/
double mcc_calcCGD_bsim4 (mcc_modellist *ptmodel, double L, double W, double temp, 
                          double vgs0, double vgs1, double vbs, double vds,elp_lotrs_param *lotrsparam)
{
  double Qinit=0.0,Qfinal=0.0,cgd=0.0;

  if ( mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART) < 0.0)
    return 0.0;

  mcc_calcQint_bsim4  (ptmodel, L, W,
                       temp, vgs0, vbs, vds,
                       NULL, NULL, &Qinit, NULL,lotrsparam);
  mcc_calcQint_bsim4  (ptmodel, L, W,
                       temp, vgs1, vbs, vds,
                       NULL, NULL, &Qfinal, NULL,lotrsparam);
  cgd = fabs((Qfinal-Qinit)/(vgs1-vgs0));
  return cgd;
}

/******************************************************************************\
 Fonction qui calcule la capacite de grille/source CGSI d un transistor                  
\******************************************************************************/
double mcc_calcCGSI_bsim4 (mcc_modellist *ptmodel, double L, double W, double temp, 
                          double vgs, double vbs, double vds,elp_lotrs_param *lotrsparam)
{
  double Qinit=0.0,Qfinal=0.0,cgsi=0.0;

  if ( mcc_getparam_quick(ptmodel,__MCC_QUICK_XPART) < 0.0)
    return 0.0;

  mcc_calcQint_bsim4  (ptmodel, L, W,
                       temp, 0.0, vbs, vds,
                       NULL, &Qinit,NULL,  NULL,lotrsparam);
  mcc_calcQint_bsim4  (ptmodel, L, W,
                       temp, vgs, vbs, vds,
                       NULL, &Qfinal, NULL, NULL,lotrsparam);
  cgsi = fabs((Qfinal-Qinit)/vgs);
  return cgsi;
}

/******************************************************************************\
FUNC :  mcc_calcIgb_bsim4
        Igb = Igbacc + Igbinv

\******************************************************************************/
double mcc_calcIgb_bsim4 (mcc_modellist *ptmodel, double L, double W, double temp, 
                          double Vgs, double Vds, double Vbs, 
                          elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0;
  static double prevW = 0.0;
  double Igb,Igbacc,Igbinv,Vgb;
  static double Leff,Weff,Vbseff,K2,c0,c1,c2,c3,c4;
  static double Toxratio, Vaux,TOXE,AIGBACC,BIGBACC,CIGBACC,Voxacc;
  static double AIGBINV,BIGBINV,CIGBINV,sqrtphis,NIGBACC,NIGBINV,EIGBINV;
  static double TOXREF,NTOX,VtT,Vfbzb,Vth,Vth_bias,phis,K1,K1ox,Voxdepinv;
  static double BINUNIT,Vfbeff,Vgsteff,Vgseff;
  double A = 4.97232e-7;
  double B = 7.45669e11;
  int calcul_dimension;
  static int prevNF=0;
  int NF = mcc_get_nf (lotrsparam,ptmodel);
  static chain_list *previous_longkey=NULL;
  
  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;

  if ( calcul_dimension ) {
    Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W);
    Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W);
    TOXE = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE) ;
    TOXREF = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXREF) ;
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;   
    NTOX = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NTOX,MCC_NO_LOG);
    AIGBACC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AIGBACC,MCC_NO_LOG);
    BIGBACC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BIGBACC,MCC_NO_LOG);
    CIGBACC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIGBACC,MCC_NO_LOG);
    NIGBACC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NIGBACC,MCC_NO_LOG);
    AIGBINV = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AIGBINV,MCC_NO_LOG);
    BIGBINV = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BIGBINV,MCC_NO_LOG);
    CIGBINV = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIGBINV,MCC_NO_LOG);
    NIGBINV = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NIGBINV,MCC_NO_LOG);
    EIGBINV = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_EIGBINV,MCC_NO_LOG);
    K1 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);
    K2 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K2,MCC_NO_LOG);
    K1ox = K1 * TOXE/mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM) ;
    Toxratio = pow(TOXREF/TOXE,NTOX) / (TOXE*TOXE);
  }

  Vth = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, 0.0, 0.0,1,lotrsparam,MCC_NO_LOG) ;
  if (ptmodel->TYPE == MCC_PMOS)
  	Vth = -Vth ;

  Vth_bias = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, Vbs, Vds,0,lotrsparam,MCC_NO_LOG) ;
  if (ptmodel->TYPE == MCC_PMOS)
  	Vth_bias = -Vth_bias ;

  Vbseff  = mcc_calc_vbseff_bsim4 (ptmodel,Vbs,Leff,Weff,
                                  temp,K2,calcul_dimension,lotrsparam);

  Vgsteff = mcc_calc_vgsteff_bsim4 (ptmodel,
                                    Vth_bias, Vbseff, 
                                    Vds, Vgs,
                                    temp,
                                    Leff, Weff,
                                    L, W,
                                    &Vgseff,
                                    lotrsparam,
                                    calcul_dimension);

  Vgb = Vgseff-Vbseff; // correction par rapport a ng src

  VtT = mcc_calc_vt (temp);

  phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);

  sqrtphis = sqrt(phis);

  Vfbzb = Vth - phis- K1*sqrtphis;

  c0 = -(Vgb-Vfbzb)/(NIGBACC*VtT);
  if (c0 > MCC_EXP_THRESHOLD)
    Vaux = -(Vgb-Vfbzb);
  else if (c0 < -MCC_EXP_THRESHOLD)
    Vaux = (NIGBACC*VtT)*log(1.0 + MCC_MIN_EXP);
  else
    Vaux = NIGBACC*VtT*log (1.0+exp(c0));

  //Vfbeff = Vfbzb - 0.5*(Vfbzb-Vgb-0.02+sqrt(pow(Vfbzb-Vgb-0.02,2.0)+0.08*Vfbzb));

  c1 = Vfbzb - Vgseff + Vbseff - 0.02;
  if (Vfbzb <= 0.0)
      c2 = sqrt(c1 * c1 - 4.0 * 0.02 * Vfbzb);
  else
      c2 = sqrt(c1 * c1 + 4.0 * 0.02 * Vfbzb);
  Vfbeff = Vfbzb - 0.5 * (c1 + c2);

  Voxacc = Vfbzb - Vfbeff;

  c0 = Weff*Leff*Toxratio*Vgb;
  
  c1 = -B*TOXE*(AIGBACC-BIGBACC*Voxacc)*(1.0+CIGBACC*Voxacc);
  if ( c1 > MCC_EXP_THRESHOLD)
    c2 = MCC_MAX_EXP;
  else if (c1 < -MCC_EXP_THRESHOLD)
    c2 = MCC_MIN_EXP;
  else
    c2 = exp(c1);

  //------------------------------------------------------------------------------
  Igbacc = A*Vaux*c0*c2;
  //------------------------------------------------------------------------------
  
  if ( ptmodel->TYPE == MCC_PMOS )
    Igbacc = - Igbacc;

  A = 3.75956e-7;
  B = 9.82222e11;

  //Voxdepinv = K1ox*sqrtphis+Vgsteff;

  c1 = 0.5 * K1ox;
  c2 = Vgseff - Vfbeff - Vbseff - Vgsteff;
  if (K1ox == 0.0)
    Voxdepinv = 0.0;
  else if (c2 < 0.0)
    Voxdepinv = -c2;
  else { 
    c3 = sqrt(c1 * c1 + c2);
    c4 = c1 / c3;
    Voxdepinv =K1ox * (c3 - c1);
  }
  Voxdepinv += Vgsteff;

  c1 = (Voxdepinv - EIGBINV)/(NIGBINV*VtT);
  if ( c1 > MCC_EXP_THRESHOLD)
    Vaux = Voxdepinv - EIGBINV;
  else if ( c1 < -MCC_EXP_THRESHOLD )
    Vaux = (Voxdepinv - EIGBINV) * log(1.0 + MCC_MIN_EXP);
  else 
    Vaux = NIGBINV*VtT*log (1.0+exp(c1));

  c1 = -B*TOXE*(AIGBINV-BIGBINV*Voxdepinv)*(1.0+CIGBINV*Voxdepinv);
  if ( c1 > MCC_EXP_THRESHOLD )
    c2 = MCC_MAX_EXP;
  else if ( c1 < -MCC_EXP_THRESHOLD )
    c2 =  MCC_MIN_EXP;
  else
    c2 = exp(c1);

  //------------------------------------------------------------------------------
  Igbinv = A*Vaux*c0*c2;
  //------------------------------------------------------------------------------
  if ( ptmodel->TYPE == MCC_PMOS )
    Igbinv = - Igbinv;
  
  Igb = Igbacc + Igbinv;

  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }

  return Igb;
}

/******************************************************************************\
FUNC :  mcc_calcIgidl_bsim4

calc IGIDL and IGISL

\******************************************************************************/
void mcc_calcIgixl_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                          double *ptIgidl, double *ptIgisl, 
                          double temp, double Vgs, double Vds, double Vbs, 
                          elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0,Leff;
  static double prevW = 0.0,Weff,Weffcj;
  double Igidl, Igisl,Vgdeff,Vgseff,Vbseff,Vth_bias;
  static double BINUNIT,K2,TOXE, AGIDL, BGIDL, CGIDL, EGIDL, AGISL, BGISL, CGISL, EGISL ;
  double type = 1.0;
  double Vgd,Vdb,Vbd;
  static double c0,c2,c3,c4;
  static double d0,d1,d2,d3;
  int calcul_dimension;
  static int prevNF=0;
  int NF = mcc_get_nf (lotrsparam,ptmodel);
  static chain_list *previous_longkey=NULL;
  
  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;

  if ( calcul_dimension ) {
    Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W);
    Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W);
    Weffcj = mcc_calc_weffcj_bsim4 (ptmodel,lotrsparam,L,W);
    TOXE = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE) ;
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;   
    K2 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K2,MCC_NO_LOG);
    AGIDL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AGIDL,MCC_NO_LOG);
    BGIDL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BGIDL,MCC_NO_LOG);
    CGIDL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CGIDL,MCC_NO_LOG);
    EGIDL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_EGIDL,MCC_NO_LOG);
    AGISL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AGISL,MCC_NO_LOG);
    BGISL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BGISL,MCC_NO_LOG);
    CGISL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CGISL,MCC_NO_LOG);
    EGISL = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_EGISL,MCC_NO_LOG);
    c0 = 3.0*TOXE;
  }

  Vth_bias = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, Vbs, Vds,0,lotrsparam,MCC_NO_LOG) ;
  if (ptmodel->TYPE == MCC_PMOS) {
  	Vth_bias = -Vth_bias ;
    type = -1.0;
  }

  Vdb = Vds-Vbs;
  Vbd = -Vdb;
  Vgd = Vgs-Vds;

  Vbseff  = mcc_calc_vbseff_bsim4 (ptmodel,Vbs,Leff,Weff,
                                  temp,K2,calcul_dimension,lotrsparam);

  mcc_calc_vgsteff_bsim4 (ptmodel,
                          Vth_bias, Vbseff, 
                          Vds, Vgs,
                          temp,
                          Leff, Weff,
                          L, W,
                          &Vgseff,
                          lotrsparam,
                          calcul_dimension);

  mcc_calc_vgsteff_bsim4 (ptmodel,
                          Vth_bias, Vbseff, 
                          Vds, Vgd,
                          temp,
                          Leff, Weff,
                          L, W,
                          &Vgdeff,
                          lotrsparam,
                          calcul_dimension);

  c2 = 3.0 * TOXE;
  c3 = (Vds - Vgseff - EGIDL ) / c2;
  if ((AGIDL <= 0.0) || (BGIDL <= 0.0)
      || (c3 <= 0.0) || (CGIDL <= 0.0) || (Vbd > 0.0))
    Igidl = 0.0;
  else {
    c4 = BGIDL / c3;
    if (c4 < 100.0)
      Igidl = AGIDL * Weffcj * c3 * exp(-c4);
    else { 
      Igidl = AGIDL * Weffcj * 3.720075976e-44;
      Igidl *= c3;
    }
    d0 = Vbd * Vbd;
    d1 = -Vbd * d0;
    d2 = CGIDL + d1;
    d3 = d1 / d2;
    Igidl *= d3;
    Igidl *= type;
  }
  //Igidl = AGIDL*Weffcj*NF*(Vds-Vgseff-EGIDL)/c0*exp(-(c0*BGIDL)/(Vds-Vgseff-EGIDL))*c1/(CGIDL*c1);

  c2 = 3.0 * TOXE;
  c3 = (-Vds - Vgdeff - EGISL ) / c2;
  if ((AGISL <= 0.0) || (BGISL <= 0.0)
      || (c3 <= 0.0) || (CGISL <= 0.0) || (Vbs > 0.0))
    Igisl = 0.0;
  else {
    c4 = BGISL / c3;
    if (c4 < 100.0)
      Igisl = AGISL * Weffcj * c3 * exp(-c4);
    else { 
      Igisl = AGISL * Weffcj * 3.720075976e-44;
      Igisl *= c3;
    }
    d0 = Vbs * Vbs;
    d1 = -Vbs * d0;
    d2 = CGISL + d1;
    d3 = d1 / d2;
    Igisl *= d3;
    Igisl *= type;
  }
  //Igisl = AGIDL*Weffcj*NF*(-Vds-Vgseff-EGIDL)/c0*exp(-(c0*BGIDL)/(Vds-Vgseff-EGIDL))*c1/(CGIDL*c1);

  if ( ptIgidl ) *ptIgidl = Igidl;
  if ( ptIgisl ) *ptIgisl = Igisl;
  
  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
}

/******************************************************************************\
FUNC :  mcc_calcIxb_bsim4

\******************************************************************************/
void mcc_calcIxb_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                        double *ptIdb, double *ptIsb,  
                        double temp, double Vds, double Vbs, 
                        double AD, double PD, 
                        double AS, double PS, 
                        elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0,Leff;
  static double prevW = 0.0,Weff;
  static double DIOMOD,BINUNIT,NJD,BVD,XJBVD,TNOM;
  static double JSD,JSSWD,JSSWGD;
  static double JSS,JSSWS,JSSWGS,BVS,XJBVS,NJS;
  static double Weffcj;
  static double XTID,XTIS;
  double Idb=0.0,Isb = 0.0,fbreakdown_d,fbreakdown_s,Vbd,Gmin=1.0e-12;
  double Isbd,JsdT,JsswdT,JsswgdT;
  double Isbs,JssT,JsswsT,JsswgsT;
  double T,EgT,EgTnom,VtT,VtTnom;
  double c0,c1;
  double type = 1.0;
  int calcul_dimension;
  static int prevNF=0;
  int NF = mcc_get_nf (lotrsparam,ptmodel);
  static chain_list *previous_longkey=NULL;

  if (ptmodel->TYPE == MCC_PMOS ) {
    type = -1.0;
  }
  Vbd = Vbs-Vds;

  if ( AD < 0.0 || PD < 0.0 || AS < 0.0 || PS < 0.0 ) {
    mcc_get_area_perim();
    if ( ptmodel->TYPE == MCC_NMOS ) {
      AD = MCC_ADN;
      PD = MCC_PDN;
      AS = MCC_ASN;
      PS = MCC_PSN;
    }
    else {
      AD = MCC_ADP;
      PD = MCC_PDP;
      AS = MCC_ASP;
      PS = MCC_PSP;
    }
  }

  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;
  
  if ( calcul_dimension ) {
    Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W);
    Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W);
    Weffcj = mcc_calc_weffcj_bsim4 (ptmodel,lotrsparam,L,W);
    TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM)+MCC_KELVIN;
    //TNOM = TNOM + MCC_KELVIN;
    T = temp+MCC_KELVIN;
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT) ;   
    // Drain param
    JSD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_JSD,MCC_NO_LOG);
    JSSWD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_JSSWD,MCC_NO_LOG);
    JSSWGD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_JSSWGD,MCC_NO_LOG);
    BVD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BVD,MCC_NO_LOG);
    NJD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NJD,MCC_NO_LOG);
    XJBVD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XJBVD,MCC_NO_LOG);
    XTID = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XTID,MCC_NO_LOG);
    // Source param
    JSS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_JSS,MCC_NO_LOG);
    JSSWS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_JSSWS,MCC_NO_LOG);
    JSSWGS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_JSSWGS,MCC_NO_LOG);
    BVS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BVS,MCC_NO_LOG);
    NJS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NJS,MCC_NO_LOG);
    XJBVS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XJBVS,MCC_NO_LOG);
    XTIS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_XTIS,MCC_NO_LOG);
  }

  if( BVS < 0.0 ) BVS = 10.0 ;

  // temperature effect 
  VtT = MCC_KB * T / MCC_Q;
  VtTnom = MCC_KB * TNOM / MCC_Q;
  EgTnom = 1.16-7.02e-4*pow(TNOM, 2.0)/(TNOM+1108.0);
  EgT    = 1.16-7.02e-4*(T*T)/(T+1108.0);

  // Drain side
  c0 = exp((EgTnom/VtTnom-EgT/VtT+XTID*log(T/TNOM))/NJD);
  JsdT = JSD*c0;
  JsswdT = JSSWD*c0;
  JsswgdT = JSSWGD*c0;

  // Source side
  c1 = exp((EgTnom/VtTnom-EgT/VtT+XTIS*log(T/TNOM))/NJS);
  JssT = JSS*c1;
  JsswsT = JSSWS*c1;
  JsswgsT = JSSWGS*c1;
  // END temperature effect 

  Isbd = AD*JsdT + PD*JsswdT + Weffcj*NF*JsswgdT;
  fbreakdown_d = 1.0+XJBVD*exp(-MCC_Q*(BVD+Vbd)/(NJD*MCC_KB*TNOM));
  Isbs = AS*JssT + PS*JsswsT + Weffcj*NF*JsswgsT;
  fbreakdown_s = 1.0+XJBVS*exp(-MCC_Q*(BVS+Vbs)/(NJS*MCC_KB*TNOM));

  switch ( MCC_ROUND (DIOMOD) ) {
    case 0 : 
             Idb = Isbd*(exp(MCC_Q*Vbd/NJD*MCC_KB*TNOM)-1.0)*fbreakdown_d+Vbd*Gmin;
             Isb = Isbs*(exp(MCC_Q*Vbs/NJS*MCC_KB*TNOM)-1.0)*fbreakdown_s+Vbs*Gmin;
             break;
    case 1 :
             Idb = Isbd*(exp(MCC_Q*Vbd/(NJD*MCC_KB*TNOM))-1.0)+Vbd*Gmin;
             Isb = Isbs*(exp(MCC_Q*Vbs/(NJS*MCC_KB*TNOM))-1.0)+Vbs*Gmin;
             break;
    case 2 :
             if (XJBVD <= 0.0) XJBVD = 1.0;
             if (XJBVS <= 0.0) XJBVS = 1.0;
             Idb = Isbd*(exp(MCC_Q*Vbd/NJD*MCC_KB*TNOM)-1.0)*fbreakdown_d+Vbd*Gmin;
             Isb = Isbs*(exp(MCC_Q*Vbs/NJS*MCC_KB*TNOM)-1.0)*fbreakdown_s+Vbs*Gmin;
             break;
  }
  Idb *= type;
  Isb *= type;

  if ( ptIdb ) *ptIdb = Idb;
  if ( ptIsb ) *ptIsb = Isb;

  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
}

/******************************************************************************\
FUNC :  mcc_calcIgx_bsim4

\******************************************************************************/
void mcc_calcIgx_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                        double *ptIgd, double *ptIgs,  
                        double temp, double Vds, double Vgs, 
                        elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0,Leff;
  static double prevW = 0.0,Weff;
  double Igd=0.0,Igs=0.0,type = 1.0;
  double Vgs_p,A,B,Vgd,Vgd_p;
  static double ToxRatioEdge,DLCIG, DLCIGD, BINUNIT,TOXE, POXEDGE, TNOM;
  static double NGATE,AIGS, AIGD, BIGS, BIGD, CIGS, CIGD, TOXREF,NTOX,NSD,VFBSDOFF, TVFBSDOFF, VFBSDOFFT ;
  double Vfbsd;
  double c0,c1;
  int calcul_dimension;
  static int prevNF=0;
  int NF = mcc_get_nf (lotrsparam,ptmodel);
  static chain_list *previous_longkey=NULL;

  if (ptmodel->TYPE == MCC_PMOS ) {
    type = -1.0;
    A = 3.42537e-7;
    B = 1.16645e12;
  }
  else {
    A = 4.97232e-7;
    B = 7.45669e11;
  }
  Vgd = Vgs-Vds;

  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;

  if ( calcul_dimension ) {
    Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W);
    Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W);
    TNOM = mcc_getparam_quick(ptmodel, __MCC_QUICK_TNOM);
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT);
    TOXE = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE) ;
    TOXREF = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXREF) ;
    NTOX = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NTOX,MCC_NO_LOG);
    POXEDGE = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_POXEDGE,MCC_NO_LOG);
    DLCIGD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DLCIGD,MCC_NO_LOG);
    AIGD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AIGSD,MCC_NO_LOG);
    BIGD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BIGSD,MCC_NO_LOG);
    CIGD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIGSD,MCC_NO_LOG);
    DLCIG = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_DLCIG,MCC_NO_LOG);
    AIGS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AIGS,MCC_NO_LOG);
    BIGS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BIGS,MCC_NO_LOG);
    CIGS = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIGS,MCC_NO_LOG);
    VFBSDOFF = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_VFBSDOFF,MCC_NO_LOG);
    TVFBSDOFF = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_TVFBSDOFF,MCC_NO_LOG);
    NGATE = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NGATE,MCC_NO_LOG) ;
    if(NGATE <= 1.0e23)
      NGATE = NGATE *1.0e6 ;
    NSD  = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NSD,MCC_NO_LOG) ;
    NSD *= 1.0e6 ;
    ToxRatioEdge = pow (TOXREF/(TOXE*POXEDGE),NTOX)/(pow(TOXE*POXEDGE,2.0));
  }

  VFBSDOFFT = VFBSDOFF * ( 1.0 + TVFBSDOFF *( temp-TNOM ) ) ;

  if ( NGATE > 0.0 ) 
    Vfbsd = (MCC_KB*(temp+MCC_KELVIN)/MCC_Q)*log(NGATE/NSD)+VFBSDOFFT;
  else 
    Vfbsd = 0.0;

  c0 = Vgs-Vfbsd;
  Vgs_p = sqrt(c0*c0+1.0e-4);
  c0 = Vgd-Vfbsd;
  Vgd_p = sqrt(c0*c0+1.0e-4);

  c0 = -B*TOXE*POXEDGE*(AIGS-BIGS*Vgs_p)*(1.0+CIGS*Vgs_p);
  if ( c0 > MCC_EXP_THRESHOLD )
    c1 = MCC_MAX_EXP;
  else if ( c0 < -MCC_EXP_THRESHOLD)
    c1 = MCC_MIN_EXP;
  else 
    c1 = exp (c0);
  Igs = type*Weff*DLCIG*A*ToxRatioEdge*Vgs*Vgs_p*c1;

  c0 = -B*TOXE*POXEDGE*(AIGD-BIGD*Vgd_p)*(1.0+CIGD*Vgd_p);
  if ( c0 > MCC_EXP_THRESHOLD )
    c1 = MCC_MAX_EXP;
  else if ( c0 < -MCC_EXP_THRESHOLD)
    c1 = MCC_MIN_EXP;
  else 
    c1 = exp (c0);

  Igd = type*Weff*DLCIGD*A*ToxRatioEdge*Vgd*Vgd_p*c1;

  if ( ptIgs ) *ptIgs = Igs;
  if ( ptIgd ) *ptIgd = Igd;

  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
}

/******************************************************************************\
FUNC :  mcc_calcIgcx_bsim4

\******************************************************************************/
void mcc_calcIgcx_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                         double *ptIgcd, double *ptIgcs,  
                         double temp, double Vds, double Vgs, double Vbs,
                         elp_lotrs_param *lotrsparam)
{
  static mcc_modellist *prevmodel = NULL;
  static double prevL = 0.0,Leff;
  static double prevW = 0.0,Weff;
  double Igcd=0.0,Igcs=0.0,type = 1.0;
  static double PIGCD,K2,BINUNIT,TOXE,TOXREF,NTOX,ToxRatio;
  static double phis,sqrtphis,K1,K1ox;
  static double AIGC,BIGC,CIGC,NIGC,VTH0;
  double Vfbzb,Vfbeff;
  double VtT,Vaux, Vgseff, Voxdepinv;
  double c0,c1,c2,c3,Igc0;
  double Vbseff,Vth,Vth_unbias,Vdseff,Vgsteff,A,B;
  int calcul_dimension;
  static int prevNF=0;
  int NF = mcc_get_nf (lotrsparam,ptmodel);
  static chain_list *previous_longkey=NULL;

  if ( !mcc_is_same_model (ptmodel,L,W,NF,prevmodel,prevL,prevW,prevNF,lotrsparam,previous_longkey) ) 
      calcul_dimension = 1 ;
  else
      calcul_dimension = 0 ;

  if ( calcul_dimension ) {
    Leff = mcc_calc_leff_bsim4 (ptmodel, lotrsparam, L, W);
    Weff = mcc_calc_weff_bsim4 (ptmodel, lotrsparam, L, W);
    TOXE = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXE);
    TOXREF = mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXREF);
    BINUNIT = mcc_getparam_quick(ptmodel, __MCC_QUICK_BINUNIT);
    K2 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K2,MCC_NO_LOG);
    AIGC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_AIGC,MCC_NO_LOG);
    BIGC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_BIGC,MCC_NO_LOG);
    CIGC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_CIGC,MCC_NO_LOG);
    NIGC = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NIGC,MCC_NO_LOG);
    NTOX = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_NTOX,MCC_NO_LOG);
    K1 = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_K1,MCC_NO_LOG);
    K1ox = K1 * TOXE/mcc_getparam_quick(ptmodel, __MCC_QUICK_TOXM) ;
    phis = mcc_calc_phis_bsim4 (ptmodel,Leff,Weff,BINUNIT,temp);
    sqrtphis = sqrt(phis);
    ToxRatio = pow(TOXREF/TOXE,NTOX) / (TOXE*TOXE);
  }

  VTH0 = mcc_compute_vth0 ( ptmodel, 
                            Leff, Weff,
                            L, W,
                            lotrsparam,
                            mcc_need_stress_bs43(ptmodel,lotrsparam),
                            BINUNIT
                          );

  Vth_unbias = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, 0.0, 0.0,1,lotrsparam,MCC_NO_LOG) ;
  Vth = mcc_calcVTH_bsim4 (ptmodel, L, W, temp, Vbs, Vds,0,lotrsparam,MCC_NO_LOG) ;
  if (ptmodel->TYPE == MCC_PMOS ) {
    Vth = - Vth;
    Vth_unbias = - Vth_unbias;
    VTH0 = -VTH0;
    type = -1.0;
    A = 3.42537e-7;
    B = 1.16645e12;
  }
  else {
    A = 4.97232e-7;
    B = 7.45669e11;
  }

  Vbseff  = mcc_calc_vbseff_bsim4 (ptmodel,Vbs,Leff,Weff,
                                   temp,K2,calcul_dimension,lotrsparam);

  Vgsteff = mcc_calc_vgsteff_bsim4 (ptmodel,
                                    Vth, Vbseff, 
                                    Vds, Vgs,
                                    temp,
                                    Leff, Weff,
                                    L, W,
                                    &Vgseff,
                                    lotrsparam,
                                    calcul_dimension);

  mcc_calc_vdseff_bsim4 ( ptmodel,
                          &Vdseff, NULL, 
                          NULL, NULL,
                          NULL,NULL,
                          NULL, NULL,
                          NULL, NULL,
                          Vth, Vbseff, 
                          Vds, Vgsteff,
                          temp, L, W,
                          lotrsparam,
                          calcul_dimension);

  if ( mcc_getparamtype_quick(ptmodel, __MCC_QUICK_PIGCD) == MCC_SETVALUE )
    PIGCD = binning_quick(ptmodel, BINUNIT, Leff, Weff, __MCC_QUICK_PIGCD,MCC_NO_LOG);
  else
    PIGCD = (B*TOXE/(Vgsteff*Vgsteff)*(1.0-Vdseff/(2.0*Vgsteff)));

  // -> Igc0 computation
  VtT = mcc_calc_vt (temp);

  c0 = NIGC*VtT;
  Vaux = c0*log(1.0+exp((Vgseff-VTH0)/c0));

  Vfbzb = Vth_unbias - phis- K1*sqrtphis;
  c1 = Vfbzb - Vgseff + Vbseff - 0.02;
  if (Vfbzb <= 0.0)
      c2 = sqrt(c1 * c1 - 4.0 * 0.02 * Vfbzb);
  else
      c2 = sqrt(c1 * c1 + 4.0 * 0.02 * Vfbzb);
  Vfbeff = Vfbzb - 0.5 * (c1 + c2);

  c1 = 0.5 * K1ox;
  c2 = Vgseff - Vfbeff - Vbseff - Vgsteff;
  if (K1ox == 0.0)
    Voxdepinv = 0.0;
  else if (c2 < 0.0)
    Voxdepinv = -c2;
  else { 
    c3 = sqrt(c1 * c1 + c2);
    Voxdepinv =K1ox * (c3 - c1);
  }
  Voxdepinv += Vgsteff;

  c0 = -B*TOXE*(AIGC-BIGC*Voxdepinv)*(1.0+CIGC*Voxdepinv);
  if (c0 > MCC_EXP_THRESHOLD)
    c1 = MCC_MAX_EXP;
  else if (c0 < -MCC_EXP_THRESHOLD)
    c1 = MCC_MIN_EXP;
  else
    c1 = exp(c0);

  Igc0 = Weff*Leff*A*ToxRatio*Vgseff*Vaux*c1;

  c0 = PIGCD*Vdseff;
  c1 = c0*c0;

  if (-c0 > MCC_EXP_THRESHOLD)
    c2 = MCC_MAX_EXP;
  else if (c0 > MCC_EXP_THRESHOLD)
    c2 = MCC_MIN_EXP;
  else
    c2 = exp(-c0);

  Igcs = type*Igc0*(c0 + c2 - 1.0 + 1.0e-4)/(c1+2.0e-4);
  Igcd = type*Igc0*(1.0-(c0+1.0)*c2+1.0e-4)/(c1+2.0e-4);

  if (ptIgcd) *ptIgcd = Igcd;
  if (ptIgcs) *ptIgcs = Igcs;

  prevmodel = ptmodel;
  prevL = L;
  prevW = W;
  prevNF = NF;
  if (calcul_dimension)
  {
    freechain(previous_longkey);
    previous_longkey=dupchainlst(lotrsparam->longkey);
  }
}

void mcc_calcnumberfingerdiff( double nf,
                               double minsd,
                               double *nuintd,
                               double *nuendd,
                               double *nuints,
                               double *nuends
                             )
{
  int i_nf ;

  i_nf = MCC_ROUND( nf );

  if( (i_nf%2) != 0 ) {

    *nuendd = *nuends = 1.0 ;
    *nuintd = *nuints = 2.0 * MCC_MAX( (nf-1.0) / 2.0, 0.0 );
    
  }
  else {
 
    if( minsd == 1 ) {

      *nuendd = 2.0 ;
      *nuintd = 2.0 * MCC_MAX( nf/2.0 - 1.0, 0.0 );
      *nuends = 0.0 ;
      *nuints = nf ;
      
    }
    else {

      *nuendd = 0.0 ;
      *nuintd = nf ;
      *nuends = 2.0 ;
      *nuints = 2.0 * MCC_MAX( nf/2.0 - 1.0, 0.0 );
    }
  }
}

void mcc_calcPA_bsim4( double nf,
                 double geomod,
                 double minsd,
                 double weffcj,
                 double dmcg,
                 double dmci,
                 double dmdg,
                 double *ps,
                 double *pd,
                 double *as,
                 double *ad
               )
{
  double nuintd, nuendd, nuints, nuends ;
  double psiso, pdiso, pssha, pdsha, psmer, pdmer, asiso, adiso, assha, adsha, asmer, admer ;
  double t0, t1, t2 ;

  if( geomod < 9 )
    mcc_calcnumberfingerdiff( nf, minsd, &nuintd, &nuendd, &nuints, &nuends );
  else {
    nuintd = 0.0 ;
    nuendd = 0.0 ;
    nuints = 0.0 ;
    nuends = 0.0 ;
  }

  t0 = dmcg + dmci ;
  t1 = dmcg + dmcg ;
  t2 = dmdg + dmdg ;

  psiso = pdiso = t0 + t0 + weffcj ;
  pssha = pdsha = t1 ;
  psmer = pdmer = t2 ;

  asiso = adiso = t0 * weffcj ;
  assha = adsha = dmcg * weffcj ;
  asmer = admer = dmdg * weffcj ;

  switch( MCC_ROUND(geomod) ) {

  case 0 :
    if( ps ) *ps = nuends * psiso + nuints * pssha ;
    if( pd ) *pd = nuendd * pdiso + nuintd * pdsha ;
    if( as ) *as = nuends * asiso + nuints * assha ;
    if( ad ) *ad = nuendd * adiso + nuintd * adsha ;
    break ;

  case 1 :
    if( ps ) *ps = nuends * psiso + nuints * pssha ;
    if( pd ) *pd = ( nuendd + nuintd ) * pdsha ;
    if( as ) *as = nuends * asiso + nuints * assha ;
    if( ad ) *ad = ( nuendd + nuintd ) * adsha ;
    break ;

  case 2 :
    if( ps ) *ps = ( nuends + nuints ) * pssha ;
    if( pd ) *pd = nuendd * pdiso + nuintd * pdsha ;
    if( as ) *as = ( nuends + nuints ) * assha ;
    if( ad ) *ad = nuendd * adiso + nuintd * adsha ;
    break ;

  case 3 :
    if( ps ) *ps = ( nuends + nuints ) * pssha ;
    if( pd ) *pd = ( nuendd + nuintd ) * pdsha ;
    if( as ) *as = ( nuends + nuints ) * assha ;
    if( ad ) *ad = ( nuendd + nuintd ) * adsha ;
    break ;

  case 4 :
    if( ps ) *ps = nuends * psiso + nuints * pssha ;
    if( pd ) *pd = nuendd * pdmer + nuintd * pdsha ;
    if( as ) *as = nuends * asiso + nuints * assha ;
    if( ad ) *ad = nuendd * admer + nuintd * adsha ;
    break ;

  case 5 :
    if( ps ) *ps = ( nuends + nuints ) * pssha ;
    if( pd ) *pd = nuendd * pdmer + nuintd * pdsha ;
    if( as ) *as = ( nuends + nuints ) * assha ;
    if( ad ) *ad = nuendd * admer + nuintd * adsha ;
    break ;

  case 6 :
    if( ps ) *ps = nuends * psmer + nuints * pssha ;
    if( pd ) *pd = nuendd * pdiso + nuintd * pdsha ;
    if( as ) *as = nuends * asmer + nuints * assha ;
    if( ad ) *ad = nuendd * adiso + nuintd * adsha ;
    break ;

  case 7 :
    if( ps ) *ps = nuends * psmer + nuints * pssha ;
    if( pd ) *pd = ( nuendd + nuintd ) * pdsha ;
    if( as ) *as = nuends * asmer + nuints * assha ;
    if( ad ) *ad = ( nuendd + nuintd ) * adsha ;
    break ;

  case 8 :
    if( ps ) *ps = nuends * psmer + nuints * pssha ;
    if( pd ) *pd = nuendd * pdmer + nuintd * pdsha ;
    if( as ) *as = nuends * asmer + nuints * assha ;
    if( ad ) *ad = nuendd * admer + nuintd * adsha ;
    break ;

  case 9 :
    if( ps ) *ps = psiso + ( nf - 1.0 ) * pssha ;
    if( pd ) *pd = nf * pdsha ;
    if( as ) *as = asiso + ( nf - 1.0 ) * assha ;
    if( ad ) *ad = nf * adsha ;
    break ;

  case 10 :
    if( ps ) *ps = nf * pssha ;
    if( pd ) *pd = pdiso + ( nf - 1.0 ) * pdsha ;
    if( as ) *as = nf * assha ;
    if( ad ) *ad = adiso + ( nf - 1.0 ) * adsha ;
    break ;
  }
}

void mcc_calcPAfromgeomod_bsim4( lotrs_list      *lotrs,
                                 mcc_modellist   *model,
                                 elp_lotrs_param *lotrsparam,
                                 double          *as,
                                 double          *ad,
                                 double          *ps,
                                 double          *pd
                               )
{
  double l, w ;
  double weffcj, 
         nf,
         geomod,
         dmcg,
         dmci,
         dmdg,
         minsd ;
  int    status ;

  l = ((double)lotrs->LENGTH) / ((double)SCALE_X) * 1.0e-6 ;
  w = ((double)lotrs->WIDTH)  / ((double)SCALE_X) * 1.0e-6 ;

  weffcj = mcc_calc_weffcj_bsim4( model, lotrsparam, l, w );
  nf     = mcc_get_nf( lotrsparam, model );

  geomod = getlotrsparam (lotrs, MBK_GEOMOD, NULL, &status );
  if( status != 1 )
    geomod = mcc_getparam_quick( model, __MCC_QUICK_GEOMOD  );

  dmcg   = mcc_getparam_quick( model, __MCC_QUICK_DMCG );
  dmci   = mcc_getparam_quick( model, __MCC_QUICK_DMCI );
  dmdg   = mcc_getparam_quick( model, __MCC_QUICK_DMDG );
  minsd  = mcc_getparam_quick( model, __MCC_QUICK_MIN  );

  mcc_calcPA_bsim4( nf, geomod, minsd, weffcj, dmcg, dmci, dmdg, ps, pd, as, ad );
}

double mcc_update_wpe_vth0( mcc_modellist   *model,
                            elp_lotrs_param *lotrsparam,
                            double l,
                            double w,
                            double leff,
                            double weff,
                            int binunit,
                            double vth0_orig
                          )
{
  double KVTH0WE ;
  double WEB ;
  double WEC ;
  double vth0 ;
  double sca ;
  double scb ;
  double scc ;
  int need_wpe ;
  
  need_wpe = mcc_get_wpe( model, lotrsparam, l, w, &sca, &scb, &scc );

  if( need_wpe ) {
  
    KVTH0WE = mcc_getparam_quick( model, __MCC_QUICK_KVTH0WE );
    KVTH0WE  = binningval_quick( model, binunit, leff, weff, __MCC_QUICK_KVTH0WE, KVTH0WE, MCC_NO_LOG ) ;
    WEB     = mcc_getparam_quick( model, __MCC_QUICK_WEB );
    WEC     = mcc_getparam_quick( model, __MCC_QUICK_WEC );
   
    vth0 = vth0_orig + KVTH0WE * ( sca + WEB*scb + WEC*scc );
  }
  else
    vth0 = vth0_orig ;

  return vth0 ;

}

double mcc_update_wpe_k2( mcc_modellist   *model,
                          elp_lotrs_param *lotrsparam,
                          double l,
                          double w,
                          double leff,
                          double weff,
                          int binunit,
                          double k2_orig
                        )
{
  double K2WE ;
  double WEB ;
  double WEC ;
  double k2 ;
  double sca ;
  double scb ;
  double scc ;
  int need_wpe ;
  
  need_wpe = mcc_get_wpe( model, lotrsparam, l, w, &sca, &scb, &scc );

  if( need_wpe ) {
  
    K2WE    = mcc_getparam_quick( model, __MCC_QUICK_K2WE );
    K2WE    = binningval_quick( model, binunit, leff, weff, __MCC_QUICK_K2WE, K2WE, MCC_NO_LOG ) ;
    WEB     = mcc_getparam_quick( model, __MCC_QUICK_WEB );
    WEC     = mcc_getparam_quick( model, __MCC_QUICK_WEC );
   
    k2 = k2_orig + K2WE * ( sca + WEB*scb + WEC*scc );
  }
  else
    k2 = k2_orig ;

  return k2 ;

}

double mcc_update_wpe_ueff( mcc_modellist   *model,
                            elp_lotrs_param *lotrsparam,
                            double l,
                            double w,
                            double leff,
                            double weff,
                            int binunit,
                            double ueff_orig
                          )
{
  double KU0WE ; 
  double WEB ;
  double WEC ;
  double ueff ;
  double sca ;
  double scb ;
  double scc ;
  int need_wpe ;
  
  need_wpe = mcc_get_wpe( model, lotrsparam, l, w, &sca, &scb, &scc );

  if( need_wpe ) {
  
    KU0WE   = mcc_getparam_quick( model, __MCC_QUICK_KU0WE );
    KU0WE   = binningval_quick( model, binunit, leff, weff, __MCC_QUICK_KU0WE, KU0WE, MCC_NO_LOG ) ;
    WEB     = mcc_getparam_quick( model, __MCC_QUICK_WEB );
    WEC     = mcc_getparam_quick( model, __MCC_QUICK_WEC );
   
    ueff = ueff_orig * ( 1.0 + KU0WE * ( sca + WEB*scb + WEC*scc ) );
  }
  else
    ueff = ueff_orig ;

  return ueff ;

}

int mcc_get_wpe( mcc_modellist   *model, 
                 elp_lotrs_param *lotrsparam, 
                 double l, 
                 double w, 
                 double *sca, 
                 double *scb, 
                 double *scc 
               )
{
  int    VERSION ;
  double SCREF ;
  double isSC ;
  double isSCA ;
  double isSCB ;
  double isSCC ;

  l=0.0 ; /* unused */
  
  VERSION = MCC_ROUND (10.0*mcc_getparam_quick(model,__MCC_QUICK_VERSION));
  SCREF = mcc_getparam_quick( model, __MCC_QUICK_SCREF );   

  if( sca ) *sca = 0.0 ;
  if( scb ) *scc = 0.0 ;
  if( scc ) *scc = 0.0 ;

  if( VERSION < 45 )
    return 0 ;
    
  isSC = lotrsparam->PARAM[elpSC] ;
  if( isSC <= ELPMINVALUE )
    return 0;
  
  if( sca ) {
  
    isSCA = lotrsparam->PARAM[elpSCA] ;
    
    if( isSCA > ELPMINVALUE ) 
      *sca = isSCA ;
   else
      *sca = SCREF*SCREF/w * ( 1.0/isSC - 1.0/(isSC+w) );
  }
  
  if( sca ) {
  
    isSCA = lotrsparam->PARAM[elpSCA] ;
    
    if( isSCA > ELPMINVALUE ) 
      *sca = isSCA ;
   else
      *sca = SCREF*SCREF/w * ( 1.0/isSC - 1.0/(isSC+w) );
  }
  
  if( scb ) {

    isSCB = lotrsparam->PARAM[elpSCB] ;

    if( isSCB > ELPMINVALUE )
      *scb = isSCB ;
    else
      *scb = 1.0/(w*SCREF) * (   SCREF/10.0*isSC*exp(-10.0*isSC/SCREF)
                               + SCREF*SCREF/100.0*exp(-10.0*isSC/SCREF)
                               - SCREF/10.0*(isSC+w)*exp(-10.0*(isSC+w)/SCREF)
                               - SCREF*SCREF/100.0*exp(-10.0*(isSC+w)/SCREF)
                             ) ;
                               
  }

  if( scc ) {

    isSCC = lotrsparam->PARAM[elpSCC] ;

    if( isSCC > ELPMINVALUE ) 
      *scc = isSCC ;
    else
      *scc = 1.0/(w*SCREF) + (   SCREF/20.0*isSC*exp(-20.0*isSC/SCREF)
                               + SCREF*SCREF/400.0*exp(-20.0*isSC/SCREF)
                               - SCREF/20.0*(isSC+w)*exp(-20.0*(isSC+w)/SCREF)
                               - SCREF*SCREF/400.0*exp(-20.0*(isSC+w)/SCREF)
                             ) ;
  }

  return 1;
}
