/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modscm_eval.h                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#ifndef STM_MODSCM_DUAL_EVAL_H
#define STM_MODSCM_DUAL_EVAL_H

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include STM_H

typedef struct {
  float F, VDD, VT, T0 ;
} stmtanhdata ;

typedef struct {
  stm_dual_param_timing *DUALSCM ;
} paramforslewpwl ;

typedef struct {
  double A ;
  double B ;
  double T0 ;
  double TS ;
  double QSAT0 ;
} stm_qsat_v ;

typedef struct {
  // common parameter
  double       IMAX ;
  double       C ;
  double       VDD ;
  // enhanced qsat
  stm_qsat_v  *QSATV ;
  // simple qsat
  double       QSAT2 ;
  double       DT ;
  double       TSATMAX ;
} stm_qsat ;

// #define QSAT(qsat,t,dt,tmax) ( (t)<0.0 ? 0.0 : ( (t)>(tmax) ? (qsat)*((tmax)-(dt))*((tmax)-(dt)) : (qsat)*((t)-(dt))*((t)-(dt)) ) )

#define CALTS_AMJAD_OK 1
#define CALTS_AMJAD_NC 2

#define CALTS_DICHO_OK    1
#define CALTS_DICHO_NOSOL 2
#define CALTS_DICHO_NC    3

double stm_modscm_dual_get_qsat2( stm_dual_param_timing *param, double threshold, double ceqrsat );
extern float stm_modscm_dual_cconf (dualparams *params, float slew);
extern float stm_modscm_dual_imax (dualparams *params);
extern float stm_modscm_dual_vth (dualparams *params);
extern float stm_modscm_dual_delay (dualparams *params, float slew, stm_pwl *pwl, float load);
extern float stm_modscm_dual_slew (dualparams *params, float slew, stm_pwl *pwl, stm_pwl **ptpwl, float load);
extern float stm_modscm_dual_slope (dualparams *params, float load, float slew);
extern double stm_modscm_dual_calte (double vddin, double VT, double seuil, double fin);
void stm_modscm_dual_calts_final_numeric_pi( stm_dual_param_timing *paramtiming, float rl, float cl1, float cl2, float *tabv, float *tabt, int   nbpoint );
double stm_modscm_dual_calts (double imax, double an, double bn, double U, double seuil, double c, double F, double dt, double vt, double vddin, double ci, double *cie, double *vcap, double slew, stm_pwth *pwth, int nth, double qinit, double strans, double icf0, stm_qsat *qsat );
int stm_modscm_dual_calts_amjad (double imax, double bn, double U, double seuil, double c, double F, double vt, double vddin, double ci, double slew, double dtin, double qinit, stm_qsat *qsat, double strans, double icf0, double *t );
int stm_modscm_dual_calts_dichotomie( double imax, double bn, double U, double seuil, double c, double F, double vt, double vddin, double ci, double slew, double dtin, double qinit, stm_qsat *qsat, double strans, double icf0, double *t, char checksol );
int stm_modscm_dual_calts_valid( double imax, double bn, double U, double F, double dtin, stm_qsat *qsat, double t, char checksol) ;
double stm_get_qsat_derivative( stm_qsat *qsat, double t );
double stm_modscm_dual_calts_final (double imax, double an, double bn, double U, double seuil, double c, double F, double vt, double vddin, double ci, double slew, double dtin, double qinit, double strans, double icf0, stm_qsat *qsat );
double stm_modscm_dual_calts_rsat_full_range (double capai, double imax, double an, double bn, double U, double VT, double vddmax, double threshold, double seuil, double rsat, double rlin, double c, double fin, double F, double te, double t0, double vddin, stm_pwl *pwl);
double stm_modscm_dual_calts_rsat (double capai, double imax, double an, double bn, double U, double VT, double vddmax, double seuil, double rsat, double c, double fin, double F, double te, double t0, double vddin, double *rsat_r, double *rlin_r, double *vsat_r ) ;
extern double stm_modscm_dual_calslew (double imax, double an, double bn, double U, double VT, double vddmax, double c, double fin, double F, double ts);
extern long stm_modscm_dual_calslope (double imax, double an, double bn, double vddin, double VT, double seuil, double c, double fin);
extern double stm_modscm_dual_ceqrsat (double imax, double vt, double vmax, double f, double vddin, double fin, double rsat, double rlin, double an, double bn, double seuil, double threshold, double te, double ts);
extern float stm_modscm_dual_capaeq ( dualparams *dual, stm_dual_param_timing *param, float slew, float r, float c1, float c2, float vth, char *signame );
char stm_isat( stm_param_isat *param, double t, double *i );
extern void stm_modscm_dual_cal_UF (double imax, double an, double bn, double vddin, double vx, double fin, double *U, double *F);
void stm_modscm_dual_slopei(double k, double B, double U, double IMAX, double Fe, double rsat, double vddmax, double threshold, double *imax, double *Fs);
void stm_modscm_dual_modifslope(double vddin, double vt, double F, double vdd, double rbr, double cbr, double imax, double Fs, double *te, double *Fm);
double stm_modscm_dual_get_ir(double t, double A, double B, double vin, double F, double Ur, double Fr, double rbr, double cbr, double imax, double Fs, double vdd, double rnt, double rns, double rni, double vint, double vt, double vout);
double stm_modscm_dual_get_ia(double t, double A, double B, double Ua, double Fa, double vin, double fin, double imax, double vdd, double rnt, double rns, double rni, double vint, double vt, double vout );
double stm_modscm_dual_get_k4Q(double A, double B, double vin, double Fin, double Ur, double Fr, double Ua, double Fa, double rbr, double cbr, double imax, double Fs, double vdd, double rns, double rnt, double rni, double vint, double vt );
double stm_modscm_dual_get_Q( double ts, double k, double A, double B, double vin, double Fin, double Ur, double Fr, double Ua, double Fa, double rbr, double cbr, double imax, double Fs, double tm, double vdd, double vt, double rns, double rnt, double rni, double vint, double fout, double threshold );
double stm_modscm_dual_voltage_rc( double t, double rbr, double cbr, double imax, double Fs );
float stm_modscm_dual_vf_input (dualparams *params);
float stm_modscm_dual_vdd_input (dualparams *params);
double stm_modscm_dual_calc_vin( double vddin, double vt, double slew, double ts );
extern double stm_modscm_dual_calculduts(dualparams *params, float slew, float load, stm_pwl *pwl);
extern double stm_modscm_dual_calculduts_threshold (dualparams *params, float slew, float load, double seuil, stm_pwl *pwl );
extern float stm_modscm_dual_slew_old (dualparams *params, float slew, float load);
extern void  stm_modscm_dual_timing( dualparams *params, float fin, stm_pwl *pwlin, stm_driver* , float r, float c1, float c2, float *delay, float *fout, stm_pwl **pwlout, char *modelname );
double stm_modscm_dual_calts_with_param_timing( stm_dual_param_timing *paramtiming, double threshold );
extern void  stm_modscm_dual_fill_param( dualparams *params, float fin, stm_pwl *pwlin, stm_driver*, float load, stm_dual_param_timing *paramtiming ) ;
double stm_modscm_dual_calc_i( double imax, double isat, double a, double b, double rnt, double rns, double rni, double vint, double vdd, double vt, double vgs, double vds );

int stm_modscm_dual_calts_final_numeric_find_next_seuil( int index, float *tabv, int nbpoint );
void stm_modscm_dual_calts_final_numeric( stm_dual_param_timing *paramtiming, float *tabv, float *tabt, int nbpoint );

typedef struct {
  double IMAX ;
  double A ;
  double B ;
  double U ;
  double RNT ;
  double RNS ;
  double RINT ;
  double VINT ;
  double VT ;
  double VDDOUT ;
  double VOUT ;
  double VIN ;
  double VINR ;
} param_final_i ;

double stm_modscm_dual_calts_final_i( param_final_i *p );
extern void stm_modscm_dual_calc_rsat( double imax, double isat, double a, double b, double rnt, double rns, double rni, double vint, double vdd, double vt, double vgs, double *rsat_r, double *rlin_r, double *rint_r, double *vsat_r, double *vlin_r); 
int stmtanhfn( stmtanhdata *data, double t, double *v );
float stm_dv( float t, mbk_pwl *linetanh, float rdriver, float cin, float capai );
double stm_modscm_dual_ceqrsat_numeric( stm_dual_param_timing *param, double seuil );
inline double stm_get_qsat( stm_qsat *qsat, double t );
void stm_modscm_dual_calts_qsat( stm_dual_param_timing *paramtiming, double threshold, stm_qsat_v *qsatv, stm_qsat *qsat, double *ceqrsat, double *ceqrsat0 );
double stm_modscm_dual_calts_newctk( stm_dual_param_timing *param, 
                                     double threshold, 
                                     double capasup,
                                     newctktrace *debug
                                   );
#endif
