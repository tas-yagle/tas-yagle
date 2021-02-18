/******************************************************************************/
/*                                                                            */
/*                      Chaine de CAO & VLSI   AVERTEC                        */
/*                                                                            */
/*    Fichier : mcc_mod_util.h                                                */
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
/* Defines                                                                    */
/******************************************************************************/
#define MCC_ROUND(a) ((int)((a)+0.5))
#define MCC_MAX(a,b) (((a)>(b))?(a):(b))

/******************************************************************************/
/* Structure                                                                  */
/******************************************************************************/

/******************************************************************************/
/* globals                                                                    */
/******************************************************************************/

#define __MCC_GETPRM_VTH0   1
#define __MCC_GETPRM_DELVTO 2
#define __MCC_GETPRM_CGDO   3
#define __MCC_GETPRM_CGDL   4
#define __MCC_GETPRM_PTA    5
#define __MCC_GETPRM_CTA    6
#define __MCC_GETPRM_PTP    7
#define __MCC_GETPRM_CTP    8
#define __MCC_GETPRM_XL     9
#define __MCC_GETPRM_LD     10
#define __MCC_GETPRM_XW     11
#define __MCC_GETPRM_WD     12

extern int MCC_LINE ; 
extern int mcc_modparse() ;
extern FILE *mcc_modin    ;
extern mcc_technolist *MCC_HEADTECHNO ;
extern eqt_ctx *mccEqtCtx;


/******************************************************************************/
/* functions                                                                  */
/******************************************************************************/

extern int              mcc_parserfile(char *technoname) ;
extern mcc_technolist  *mcc_gettechno(char *technoname) ;
extern mcc_technolist  *mcc_addtechno(char *technoname) ;	
extern void	            mcc_drvmod() ;
extern mcc_explist     *mcc_addexp(mcc_explist *head,
                                   char *name, 
                                   char *exp ,
                                   double defaut) ;
extern mcc_explist     *mcc_setexp(mcc_explist *head,
                                   char *name, 
                                   char *explist ,
                                   double defaut) ;
extern mcc_explist     *mcc_getexp(mcc_explist *head,
                                   char *name) ; 
extern void             mcc_freeexp(mcc_explist *head) ; 
extern mcc_modellist   *mcc_addmodel(char *technoname, 
                                     char *transname, 
                                     char *subcktname,
                                     int transtype, 
                                     int transcase) ;	
extern void	            mcc_delmodel(char *technoname,
                                     char *transname) ;
extern mcc_modellist   *mcc_getmodel(char *technoname,
                                     char *transname,
                                     int transtype, 
                                     int transcase,
                                     double L,
                                     double W, int forceifsubckt) ;
extern mcc_modellist   *mcc_getnextmodel(char *technoname,
                                         char *transname,
                                         int transtype, 
                                         int transcase,
                                         mcc_modellist *model) ;
extern mcc_paramlist   *mcc_addparam(mcc_modellist *ptmodel,
                                     char *name,
                                     double value,
                                     int typevalue) ;
mcc_paramlist *mcc_addparam_quick(mcc_modellist *ptmodel, int idx, char *name, double value, int typevalue);

extern void             mcc_setparam(mcc_modellist *ptmodel,
                                     char *name,
                                     double value) ;
void mcc_setparam_quick(mcc_modellist *ptmodel, int idx, char *name, double value);
extern void             mcc_initparam(mcc_modellist *ptmodel,
                                      char *name,
                                      double value,
                                      char *exp,
                                      int typevalue) ;
void mcc_initparam_quick(mcc_modellist *ptmodel, int idx, double value, char *exp, int typevalue);
extern void             mcc_delparam(mcc_modellist *ptmodel,
                                     char *name) ;
extern void 	        mcc_evalmodel(mcc_modellist *ptmodel,double L, double W,int modtype) ;
extern double	        mcc_evalparam(mcc_paramlist *param) ;
extern double	        mcc_getparam(mcc_modellist *ptmodel,
                                     char *name) ;
extern char *	        mcc_getparamexp(mcc_modellist *ptmodel,
                                        char *name) ;
char *mcc_getparamexp_quick(mcc_modellist *ptmodel, int idx);
extern mcc_paramlist   *mcc_chrparam(mcc_modellist *ptmodel,
                                     char *name) ;
mcc_paramlist *mcc_chrparam_quick(mcc_modellist *ptmodel, int idx);

extern double           mcc_getprm(mcc_modellist *ptmodel,
                                   char *name) ;
double mcc_getprm_quick(mcc_modellist *ptmodel, int pt);

extern int              mcc_cmpname ( char *name,
                                     char *nameindx) ;
extern int              mcc_getparamtype ( mcc_modellist *ptmodel,
                                          char *name ) ;
int mcc_getparamtype_quick(mcc_modellist *ptmodel, int idx);

extern void             free_all_model ( mcc_technolist *pttechnolist) ;
extern char            *mcc_cutname (char*) ;
extern char            *mcc_getsubckt ( char *technoname,
                                        char *transname,
                                        int transtype, 
                                        int transcase,
                                        double L,
                                        double W) ;
extern int              mcc_setsubcktsize ( mcc_modellist  *ptmodel,
                                            int transtype, 
                                            double L,
                                            double W) ;
extern double           binning    (mcc_modellist *ptmodel, double binunit, 
                                    double Leff, double Weff, char *param_name, int mcclog);
double binning_quick(mcc_modellist *ptmodel, double binunit, double Leff, double Weff, int idx,int mcclog);
extern double           binningval (mcc_modellist *ptmodel, double binunit,
                                    double Leff, double Weff, char *param_name,
                                    double param_value, int mcclog);
double binningval_quick (mcc_modellist *ptmodel, double binunit, double Leff, double Weff, int idx, double param_value, int mcclog);
extern void             mcc_affect_value (double *param, double value);
extern double           mcc_update_vth0 (elp_lotrs_param *,double vth0 );
extern double           mcc_update_u0 ( elp_lotrs_param *,double u0 );
extern int              mcc_check_vbs ( mcc_modellist *model, double l, double w, double vbs );
extern int              mcc_check_param ( int type );
extern int              mcc_is_same_model ( mcc_modellist *ptmodel, 
                                            double L, double W, int NF,
                                            mcc_modellist *previous_model,
                                            double previous_L,
                                            double previous_W,
                                            int previous_NF,
                                            elp_lotrs_param *lotrsparam,
                                            chain_list *previous_longkey
                                            );
extern char           *mcc_get_modelfilename ( int corner );
extern void            mcc_update_area_perim ( int corner , int lotrscase );
