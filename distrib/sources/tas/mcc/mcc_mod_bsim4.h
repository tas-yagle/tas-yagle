/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_bsim3v3.h                                             */
/*                                                                            */
/*                                                                            */
/*    (c) copyright 2001 AVERTEC                                              */
/*    Tous droits reserves                                                    */
/*                                                                            */
/*    Auteur(s) : Marc KUOCH                                                  */
/*----------------------------------------------------------------------------*/
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/******************************************************************************/
/* includes                                                                   */
/******************************************************************************/

/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/

extern void    mcc_initparam_bsim4(mcc_modellist *ptmodel) ;

// specific func for bsim43
extern int mcc_need_stress_bs43 ( mcc_modellist *ptmodel, 
                                  elp_lotrs_param *lotrsparam
                                );

extern double mcc_update_Kstress_vsat_bs43 ( mcc_modellist *ptmodel,
                                             elp_lotrs_param *lotrsparam,
                                             double vsat_orig,
                                             double temp,
                                             double L,
                                             double W
                                           );

extern double mcc_update_Kstress_ueff_bs43 ( mcc_modellist *ptmodel,
                                             elp_lotrs_param *lotrsparam,
                                             double ueff_orig,
                                             double temp,
                                             double L,
                                             double W
                                           );

extern double mcc_update_Kstress_eta0_bs43 ( mcc_modellist *ptmodel,
                                             elp_lotrs_param *lotrsparam,
                                             double K2_orig,
                                             double L,
                                             double W,
                                             int NF
                                           );

extern double mcc_update_Kstress_k2_bs43 ( mcc_modellist *ptmodel,
                                           elp_lotrs_param *lotrsparam,
                                           double K2_orig,
                                           double L,
                                           double W
                                         );

extern double mcc_update_Kstress_vth0_bs43 ( mcc_modellist *ptmodel,
                                             elp_lotrs_param *lotrsparam,
                                             double VTH0_orig,
                                             double L,
                                             double W
                                           );

extern double mcc_calc_Kstress_vth0_bs43 ( mcc_modellist *ptmodel,
                                           double L,
                                           double W,
                                           int NF, elp_lotrs_param *lotrsparam);

extern double mcc_calc_Pueff_bs43 ( mcc_modellist *ptmodel,
                                    double inv_sa, double inv_sb,
                                    double temp,
                                    double L,
                                    double W,
                                    int NF, elp_lotrs_param *lotrsparam );

extern double mcc_calc_Kstress_u0_bs43 ( mcc_modellist *ptmodel,
                                         double temp,
                                         double L,
                                         double W,
                                         int NF, elp_lotrs_param *lotrsparam);

extern void    mcc_calc_inv_sa_bs43 ( mcc_modellist *ptmodel, 
                                      elp_lotrs_param *lotrsparam,
                                      double L,
                                      double *inv_sa,
                                      double *inv_saref);
extern void    mcc_calc_inv_sb_bs43 ( mcc_modellist *ptmodel, 
                                      elp_lotrs_param *lotrsparam,
                                      double L,
                                      double *inv_sa,
                                      double *inv_saref);
// low level func
extern double mcc_calc_coxe_bsim4 ( mcc_modellist *ptmodel );
extern double mcc_calc_coxp_bsim4 ( mcc_modellist *ptmodel );
extern double mcc_calc_coxeff_bsim4 (mcc_modellist *ptmodel,
                                     double Vgsteff, double temp,
                                     double Leff, double Weff,
                                     double L, double W,
                                     double Vgseff,double Vbseff, double Vfbeff,
                                     double BINUNIT, int cv_model,
                                     double *coxeff_acc_dep,
                                     elp_lotrs_param *lotrsparam,
                                     int getparm_flag);
extern double  mcc_calc_gamma1_bsim4 (mcc_modellist *ptmodel, double Leff,double Weff, double BINUNIT);
extern double  mcc_calc_gamma2_bsim4 (mcc_modellist *ptmodel);
extern double  mcc_calc_ni_bsim4 (mcc_modellist *model,double temp);
extern double  mcc_calc_phis_bsim4 (mcc_modellist *ptmodel,double Leff,double Weff,
                                    double BINUNIT,double temp);
extern double  mcc_calc_vbi_bsim4 (mcc_modellist *ptmodel,double temp,
                                   double Leff, double Weff, int getparm_flag);
extern double  mcc_calc_vbx_bsim4 (mcc_modellist *ptmodel,double Leff,double Weff,
                                   double BINUNIT,double temp);

// dimensions
extern double  mcc_calc_leff_bsim4 (mcc_modellist *ptmodel,elp_lotrs_param *lotrsparam,double L,double W);
extern double  mcc_calc_weffcj_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double L,double W);
extern double  mcc_calc_weff_bsim4 (mcc_modellist *ptmodel,elp_lotrs_param *lotrsparam,double L,double W);
extern double  mcc_calcDW_bsim4  (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,double L, double W) ;
extern double  mcc_calcDL_bsim4  (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,double L, double W) ;
extern double  mcc_calcDLC_bsim4  (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,double L, double W) ;
extern double  mcc_calcDWC_bsim4  (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,double L, double W) ;
extern double  mcc_calcDWCJ_bsim4  (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam, double L, double W) ;

// efficace voltage
extern double mcc_calc_vbseff_bsim4  (mcc_modellist *ptmodel,
                                      double Vbs,
                                      double Leff, double Weff,
                                      double Temp, double K2,
                                      int getparm_flag,elp_lotrs_param *lotrsparam);
extern double mcc_calc_vgsteffcv_bsim4 (mcc_modellist *ptmodel,
                                        double Vth, double Vbseff, 
                                        double Vds, double Vgs,
                                        double temp,
                                        double Leff, double Weff,
                                        double L, double W,
                                        elp_lotrs_param *lotrsparam,
                                        int getparm_flag
                                        );
extern double mcc_calc_vgsteff_bsim4 (mcc_modellist *ptmodel,
                                      double Vth, double Vbseff, 
                                      double Vds, double Vgs,
                                      double temp,
                                      double Leff, double Weff,
                                      double L, double W,
                                      double *Vgseff_r,
                                      elp_lotrs_param *lotrsparam,
                                      int getparm_flag
                                      );
extern void mcc_calc_vdseff_bsim4 ( mcc_modellist *ptmodel,
                                    double *Vdseff, double *Vdsat,
                                    double *Esat, double *ueffT,
                                    double *Abulk_r, double *Rds_r,
                                    double *VsatT_r, double *lambda_r,
                                    double *Rfactor_r, double *Weff_v_r,
                                    double Vth, double Vbseff, 
                                    double Vds, double Vgsteff,
                                    double temp,
                                    double L, double W,
                                    elp_lotrs_param *lotrsparam,
                                    int getparm_flag
                                  );

// high level func
extern double  mcc_calcVTH_bsim4 (mcc_modellist *ptmodel, 
                                  double L, 
                                  double W, 
                                  double temp,
                                  double vbs,
                                  double vds,
                                  int capa,
                                  elp_lotrs_param *lotrsparam,
                                  int mcclog) ;

extern double  mcc_calcIDS_bsim4 (mcc_modellist *ptmodel,
                                  double Vbstrue,
                                  double Vgs,
                                  double Vds,
                                  double W,
                                  double L, 
                                  double Temp,
                                  elp_lotrs_param *lotrsparam);
double mcc_calcCGP_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,double vdd, 
                          double L, double W, double *ptQov);
double mcc_calcCDS_bsim4 (mcc_modellist *ptmodel, double temp, double vbx1, double vbx2);
double mcc_calcCDP_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                          double temp, double vbx1,double vbx2);
double mcc_calcCDW_bsim4 (mcc_modellist *ptmodel, elp_lotrs_param *lotrsparam,
                          double temp, double vbx1, double vbx2, 
                          double vdd, double L, double W) ;
/*
double mcc_calcIdiode_bsim4 (mcc_modellist *ptmodel,  double AX, 
                             double PX, double temp,
                             double L, double W, double vbx);
                             */
void   mcc_calcQint_bsim4  (mcc_modellist *ptmodel, double L, double W,
                            double temp, double vgs, double vbs, double vds,
                            double *ptQg, double *ptQs, double *ptQd,
                            double *ptQb,elp_lotrs_param *lotrsparam);
double mcc_calcCGD_bsim4 (mcc_modellist *ptmodel,
                          double L, double W, 
                          double temp, 
                          double vgs0,
                          double vgs1,
                          double vbs,
                          double vds,
                          elp_lotrs_param *elp_lotrs_param) ;
double mcc_calcCGSI_bsim4 (mcc_modellist *ptmodel,
                           double L, double W, 
                           double temp, 
                           double vgs,
                           double vbs,
                           double vds,
                           elp_lotrs_param *elp_lotrs_param) ;
double mcc_calcIgb_bsim4 (mcc_modellist *ptmodel, double L, double W, double temp, 
                          double Vgs, double Vds, double Vbs, 
                          elp_lotrs_param *lotrsparam);
void mcc_calcIgixl_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                          double *ptIgidl, double *ptIgisl,
                          double temp, double Vgs, double Vds, double Vbs, 
                          elp_lotrs_param *lotrsparam);
void    mcc_calcIxb_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                          double *ptIdb, double *Isb,  
                          double temp, double Vds, double Vbs, 
                          double AD, double PD, double AS, double PS,
                          elp_lotrs_param *lotrsparam);
void mcc_calcIgx_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                        double *ptIgd, double *ptIgs,  
                        double temp, double Vds, double Vgs, 
                        elp_lotrs_param *lotrsparam);
void mcc_calcIgcx_bsim4 (mcc_modellist *ptmodel, double L, double W, 
                         double *ptIgcd, double *ptIgcs,  
                         double temp, double Vds, double Vgs, double Vbs,
                         elp_lotrs_param *lotrsparam);
void mcc_calcPAfromgeomod_bsim4( lotrs_list      *lotrs,
                                 mcc_modellist   *model,
                                 elp_lotrs_param *lotrsparam,
                                 double          *as,
                                 double          *ad,
                                 double          *ps,
                                 double          *pd
                               );
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
                     );
void mcc_calcnumberfingerdiff( double nf,
                               double minsd,
                               double *nuintd,
                               double *nuend,
                               double *nuints,
                               double *nuends
                             );
               

double mcc_update_wpe_ueff( mcc_modellist   *model,
                            elp_lotrs_param *lotrsparam,
                            double l,
                            double w,
                            double leff,
                            double weff,
                            int binunit,
                            double ueff_orig
                          );
double mcc_update_wpe_k2( mcc_modellist   *model,
                          elp_lotrs_param *lotrsparam,
                          double l,
                          double w,
                            double leff,
                            double weff,
                            int binunit,
                          double k2_orig
                        );
double mcc_update_wpe_vth0( mcc_modellist   *model,
                            elp_lotrs_param *lotrsparam,
                            double l,
                            double w,
                            double leff,
                            double weff,
                            int binunit,
                            double vt0_orig
                          );
int mcc_get_wpe( mcc_modellist   *model, 
                 elp_lotrs_param *lotrsparam, 
                 double l, 
                 double w, 
                 double *sca, 
                 double *scb, 
                 double *scc 
               );
               
