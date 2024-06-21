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

extern void    mcc_initparam_bsim3v3(mcc_modellist *ptmodel) ;
extern int     mcc_check_hsp_acm_bsim3v3 (mcc_modellist *ptmodel);

extern double  mcc_calcDL_bsim3v3  (mcc_modellist *ptmodel, double L, double W) ; 
extern double  mcc_calcDLC_bsim3v3  (mcc_modellist *ptmodel, double L, double W) ; 

extern double  mcc_calcDW_bsim3v3  (mcc_modellist *ptmodel, double L, double W) ;
extern double  mcc_calcDWC_bsim3v3  (mcc_modellist *ptmodel, double L, double W) ;

extern double  mcc_calcVTH_bsim3v3 (mcc_modellist *ptmodel, double L, double W, 
                                    double temp, double vbs, double vds,
                                    elp_lotrs_param *lotrsparam,int mcclog) ;

extern double  mcc_calcIDS_bsim3v3 (mcc_modellist *ptmodel, double vbs, 
                                    double vgs, double vds, double W, double L, 
                                    //double AX, double PX,
                                    double temp,
                                    elp_lotrs_param *lotrsparam) ;

extern double mcc_calcLeff_bsim3v3 (mcc_modellist *ptmodel, double L, double W) ;
extern double mcc_calcWeff_bsim3v3 (mcc_modellist *ptmodel, double L, double W) ;
extern double mcc_calcCGP_bsim3v3  (mcc_modellist *ptmodel,
                                    double vdd, double L, double W,double *ptQov) ;

extern double mcc_calcIdiode_bsim3v3(mcc_modellist *ptmodel,  double AX, 
                                     double PX, double temp, double vbx) ;
extern void   mcc_calcQint_bsim3v3(mcc_modellist *ptmodel, double L, double W,
                                   double temp, double vgs, double vbs, double vds,
                                   double *ptQg, double *ptQs, double *ptQd,
                                   double *ptQb,elp_lotrs_param *lotrsparam);
extern double mcc_calcCGD_bsim3v3(mcc_modellist *ptmodel, double L, double W, 
                                  double temp, double vgs0, double vgs1, double vbs,
                                  double vds,elp_lotrs_param *lotrsparam) ;
extern double mcc_calcCGSI_bsim3v3(mcc_modellist *ptmodel, double L, double W, 
                                  double temp, double vgs, double vbs,
                                  double vds,elp_lotrs_param *lotrsparam) ;
void mcc_calcPAfromgeomod_bsim3( lotrs_list      *lotrs,
                                 mcc_modellist   *model,
                                 elp_lotrs_param *lotrsparam,
                                 double          *as,
                                 double          *ad,
                                 double          *ps,
                                 double          *pd
                               );
