/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_spice.h                                               */
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
/* defines                                                                    */
/******************************************************************************/

/******************************************************************************/
/* structures                                                                 */
/******************************************************************************/

/******************************************************************************/
/* globals                                                                    */
/******************************************************************************/

/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/

void mcc_cleanmodel( mcc_modellist *model );
extern double  mcc_integfordw ( double c, double vbd0, double vbd1, double p, double m);
extern int     mcc_getmodeltype (char *technoname) ;
extern int     mcc_get_modeltype (mcc_modellist *ptmodel,char *name) ;
extern void    mcc_deltechnofile(char *technoname) ;
extern mcc_technolist  *mcc_gettechnofile (char *technoname) ;

extern void    mcc_initallparam(char *technoname) ;
extern void    mcc_initmodel (mcc_modellist *ptmodel) ;

extern double  mcc_getLMIN (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_getLMAX (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_getWMIN (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double  mcc_getWMAX (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W) ;

extern double mcc_calcCGPO( char *technoname, char *transname,
                            int transtype, int transcase, double L, double W, 
                            double temp, double vg,
                            double vd1, double vd2,
                            elp_lotrs_param *lotrsparam );
extern double  mcc_calcDL (char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcDW (char *technoname, char *transname, 
                           int transtype, int transcase, 
                           double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcDWCJ (char *technoname, char *transname, 
                             int transtype, int transcase, 
                             double L, double W, elp_lotrs_param *lotrsparam, double temp) ;

extern double  mcc_calcDLC (char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcDWC (char *technoname, char *transname, 
                            int transtype, int transcase, 
                            double L, double W, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcVTH(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vbs, double vds,elp_lotrs_param*,int mcclog) ;

extern double  mcc_calcIDS(char *technoname, char *transname, 
                           int transtype, int transcase, double vbs, double vgs, 
                           double vds, double L, double W, 
                           double temp,elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcVDDDEG(char *technoname, char *transname, 
                              int transtype, int transcase, double L, double W, 
                              double vdd, double temp, 
                              double step,elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcVSSDEG(char *technoname, char *transname, 
                              int transtype, int transcase, double L, double W, 
                              double vdd, double temp,
                              double step,elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCGD(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double temp, double vgs0, double vgs1, double vbs, double vds,
                           elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCGSI(char *technoname, char *transname, 
                            int transtype, int transcase, double L, double W, 
                            double temp, double vgs, double vbs, double vds,
                            elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCGP(char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W,
                           double vgx, double *ptQov, elp_lotrs_param *lotrsparam, double temp) ;

extern double  mcc_calcCGPU (char *technoname, char *transname, 
                             int transtype, int transcase, double L, double W,
                             double vdd) ;

extern double  mcc_calcCGPD (char *technoname, char *transname, 
                             int transtype, int transcase, double L, double W,
                             double vdd) ;

extern double  mcc_calcCDS(char *technoname, char *transname, int transtype, 
                           int transcase, double L, double W, double temp, 
                           double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCDP(char *technoname, char *transname, int transtype, 
                           int transcase, double L, double W,
                           double temp, double vbx1, double vbx2,
                           elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCDW(char *technoname, char *transname, int transtype, 
                           int transcase, double L, double W, double temp, 
                           double vbx1, double vbx2, double vgx,
                           elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCSS(char *technoname, char *transname, int transtype, 
                           int transcase, double L, double W, double temp,
                           double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCSP(char *technoname, char *transname, int transtype, 
                           int transcase, double L, double W, 
                           double temp, double vbx1, double vbx2, elp_lotrs_param *lotrsparam) ;

extern double  mcc_calcCSW(char *technoname, char *transname, int transtype, 
                           int transcase, double L, double W, double temp,
                           double vbx1, double vbx2, double vgx,
                           elp_lotrs_param *lotrsparam) ;

extern double  mcc_gettechnolevel(char *technoname ) ;

extern int     mcc_gettransindex(char *technoname, char *transname, 
                                 int transtype, int transcase, double L, double W) ;

extern char   *mcc_getmodelname(char *technoname, char *transname, 
                                int transtype, int transcase, double L, double W) ;

extern double  mcc_getXL (char *technoname, char *transname, 
                          int transtype, int transcase, double L, double W) ;

extern double  mcc_getXW (char *technoname, char *transname, 
                          int transtype, int transcase, double L, double W) ;

extern double  mcc_calcCGS_com(mcc_modellist *ptmodel) ;
extern double  mcc_calcCGP_com(mcc_modellist *ptmodel) ;
extern double  mcc_calcCDS_com(mcc_modellist *ptmodel, double temp, double vbx1, double vbx2) ;
extern double  mcc_calcCDP_com(mcc_modellist *ptmodel, double temp, double vbx1, double vbx2) ;
extern double  mcc_calcCDW_com(mcc_modellist *ptmodel, double temp, double vbx1, double vbx2, 
                               double vgx, double L, double W) ; 
extern double  mcc_calcDL_com (mcc_modellist *ptmodel)  ; 
extern double  mcc_calcDW_com (mcc_modellist *ptmodel)  ;


extern void mcc_initparam_com (mcc_modellist *ptmodel) ;
extern void mcc_initparam_diode (mcc_modellist *ptmodel) ;

extern double mcc_calcRapIdsTemp(char *technoname, char *transname, int transtype,
                              int transcase, double L, double W, double vdd, double T0, double T1) ;
extern double mcc_calcRapIdsVolt(char *technoname, char *transname, int transtype,
                                 int transcase, double L, double W, double temp, double V0, double V1) ;
extern double  mcc_calcVDDDEG_com (mcc_modellist *ptmodel, char *transname,double L, double W,
                                   double vdd, double temp, double step, 
                                   elp_lotrs_param *lotrsparam) ;
extern double  mcc_calcVSSDEG_com (mcc_modellist *ptmodel, char *transname,double L, double W, 
                                   double vdd, double temp, double step, 
                                   elp_lotrs_param *lotrsparam);
extern void mcc_PrintQint (char *technoname, char *transname, 
                    int transtype, int transcase, double L, double W,
                    double temp, double vdd,elp_lotrs_param *lotrsparam, char *optnamevdd, char *optnamevss, int usechannel, char location);
extern void mcc_DisplayInfos (char *technoname, char *transname,
                              int transtype, int transcase, double L, double W, 
                              double temp, double vgs, double vbs, double vds,
                              double vdd,elp_lotrs_param *lotrsparam);
extern void mcc_calcQint (char *technoname, char *transname,
                          int transtype, int transcase, double L, double W, 
                          double temp, double vgs,double vbs, double vds,
                          double *ptQg,double *ptQs, double *ptQd, double *ptQb,
                          elp_lotrs_param *lotrsparam);
extern double mcc_calcCGSD (char *technoname, char *transname,
                            int transtype, int transcase, double L, double W, 
                            double temp, double vdd, double vfinal,int vdsnull,elp_lotrs_param *lotrsparam);
extern double mcc_calcCGSU (char *technoname, char *transname,
                            int transtype, int transcase, double L, double W, 
                            double temp, double vdd, double vfinal,int vdsnull,elp_lotrs_param *lotrsparam);
double mcc_calcCGS( char *technoname, char *transname, int transtype, int transcase, double L, double W, double temp, double vgsi, double vgsf, double vdsi, double vdsf, elp_lotrs_param *lotrsparam );
extern void mcc_GetInputCapa ( char *technoname, char *transname,
                               int transtype, int transcase, double L, double W, 
                               double temp, double vg1, double vg2, 
                               double vd1, double vd2, double vs1, double vs2,
                               elp_lotrs_param *lotrsparam, 
                               double *ptcgs, double *ptcgd, double *ptcgp);
extern double mcc_calc_vt (double temp);
extern double mcc_calc_eg (double temp);
extern double mcc_calcDioCapa ( char *technoname, char *dioname,
                                int modtype, int modcase, 
                                double Va, double Vc, double temp,
                                double area, double perim
                              );
extern double mcc_dio_calcCapa ( mcc_modellist *ptmodel, 
                                 double Va, double Vc, double temp,
                                 double area, double perim
                                 );
extern double mcc_dio_calcCapa_l1 ( mcc_modellist *ptmodel, 
                                    double Va, double Vc, double temp,
                                    double area, double perim
                                  );
extern double mcc_dio_calcCapa_l2 ( mcc_modellist *ptmodel, 
                                    double Va, double Vc,
                                    double temp,
                                    double area 
                                  );
extern double mcc_dio_calcCapa_l3 ( mcc_modellist *ptmodel, 
                                    double area
                                  );
extern double mcc_dio_calcCapa_l8 ( mcc_modellist *ptmodel, 
                                    double Va,double Vc,
                                    double temp,
                                    double area, double perim
                                  );
extern double mcc_calcIgb (char *technoname, char *transname,
                           int transtype, int transcase, 
                           double vbs, double vgs, double vds, 
                           double L, double W, 
                           double temp,elp_lotrs_param *lotrsparam);
extern void mcc_calcIgixl (char *technoname, char *transname,
                           int transtype, int transcase, 
                           double vbs, double vgs, double vds, 
                           double L, double W, double temp,
                           double *ptIgidl, double *ptIgisl,
                           elp_lotrs_param *lotrsparam);
extern void mcc_calcIxb (char *technoname, char *transname,
                         int transtype, int transcase, 
                         double vbs, double vds, 
                         double L, double W, double temp,
                         double AD, double PD, double AS, double PS,
                         double *ptIdb, double *ptIsb,
                         elp_lotrs_param *lotrsparam) ;
extern void mcc_calcIgx (char *technoname, char *transname,
                         int transtype, int transcase, 
                         double vds, double vgs, 
                         double L, double W, double temp,
                         double *ptIgd, double *ptIgs,
                         elp_lotrs_param *lotrsparam); 
extern void mcc_calcIgcx (char *technoname, char *transname,
                          int transtype, int transcase, 
                          double vds, double vgs, double vbs,
                          double L, double W, double temp,
                          double *ptIgcd, double *ptIgcs,
                          elp_lotrs_param *lotrsparam);
extern void mcc_compute_RD_RS ( mcc_modellist *ptmodel, double Weff,
                                double *ptRS, double *ptRD, elp_lotrs_param *lotrsparam );
extern double mcc_calcILeakage (char *technoname, char *transname,
                                int transtype, int transcase, 
                                double vbs, double vds, double vgs,
                                double L, double W, double temp,
                                double AD, double PD, double AS, double PS,
                                double *BLeak, double *DLeak, double *SLeak,
                                elp_lotrs_param *lotrsparam);
extern double mcc_calcVTI (char *technoname, char *transname, 
                           int transtype, int transcase, double L, double W, 
                           double vdd, double temp, 
                           double step, elp_lotrs_param *lotrsparam);
extern void mcc_calcPAfromgeomod( lotrs_list      *lotrs,
                                  char            *modelname,
                                  int              type,
                                  int              lotrscase,
                                  double           vdd,
                                  elp_lotrs_param *lotrsparam,
                                  double          *as, 
                                  double          *ad, 
                                  double          *ps, 
                                  double          *pd 
                                );

void mcc_calcRACCESS( char *technoname, char *transname, 
                      int transtype, int transcase, double L, double W,
                      elp_lotrs_param *lotrsparam,
                      double *RS, double *RD
                    );
int mcc_getspicetechno( char *technoname, char *transname, int transtype, int transcase, double L, double W );
void mcc_check_capa_print( char trans, char *technoname, char *transname, int transtype, int transcase, double L, double W, double temp, double vdd, elp_lotrs_param *lotrsparam );
int mcc_get_swjuncap( char *technoname, char *transname, int transtype, int transcase, double L, double W );
void mcc_check_quick_param_namealloc();
