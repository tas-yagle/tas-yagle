/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : MCC Version 1                                               */
/*    Fichier : mcc_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "mcc.h"

typedef struct {
  double     *dbl ;
  char       **chr ;
  lotrs_list *lotrs ;
} mccglobal ;

mccglobal* mcc_getglobal( void );
void mcc_setglobal( mccglobal* );

#define MCC_DOUBLETOINT(a)   (int)((fabs(a - ceil(a)) > fabs(a - floor(a))) ? \
                                   floor(a) : ceil(a))
extern double MCC_CBXG;
extern double MCC_CBXGU;
extern double MCC_CBXGD;

extern char *MCC_BUF ;
extern int MCC_PLOT;

extern int mcc_retkey                   __P((char *)) ;
extern long mcc_ftol                    __P((double)) ;
extern int mcc_ftoi                     __P((double)) ;
/*
extern double mcc_getspidouble          __P((char *,
                                             char **
                                           )) ;
extern int mcc_getspiparam              __P((char *,
                                             char *,
                                             char *
                                           )) ;
extern char *mcc_getspiline             __P((FILE *,
                                             char *
                                           )) ;
extern char *mcc_getjoker               __P((char *,
                                             char *
                                           )) ;
extern char *mcc_getfilename            __P((char *)) ;
extern char *mcc_getarg                 __P((char *,
                                             char *,
                                             int
                                           )) ;
                                           */
extern char *mcc_getfisrtarg            __P((char *,
                                             char *
                                           )) ;
extern char *mcc_initstr                __P((char *)) ;
extern void mcc_initcalcparam           __P((int)) ;
extern void mccenv                      __P(()) ;
extern void mcc_calcspiparam            __P((int,elp_lotrs_param*,elp_lotrs_param*)) ;
extern FILE *mcc_fopen                  __P((char *,
                                             char *
                                           )) ;
extern void mcc_fclose                  __P((FILE*,
                                             char *
                                           )) ;
extern void mcc_drvparam                __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern int mcc_prsparam                 __P((elp_lotrs_param*,elp_lotrs_param*)) ;
extern void mcc_equaterange             __P(()) ;
extern void mcc_resizerange             __P(()) ;
extern void mcc_modifyrange             __P(()) ;
extern elp_lotrs_param *mcc_lotrsparam_add ( double delvt0,
                                             double mulu0, 
                                             double sa, 
                                             double sb, 
                                             double sd, 
                                             double nf, 
                                             double m, 
                                             double nrs, 
                                             double nrd, double sc, double sca, double scb, double scc, 
                                             double vbulk);
extern elp_lotrs_param *mcc_lotrsparam_set ( elp_lotrs_param *ptinsparam,
                                             double delvt0, double mulu0,
                                             double sa, double sb, double sd, double nf,
                                             double m, double nrs, double nrd, double sc, double sca, double scb, double scc,
                                             double vbulk);
extern elp_lotrs_param *mcc_init_lotrsparam ( void );
extern int mcc_is_default_technoname ( char *name );
extern char *mcc_get_date(void);
extern int mcc_use_multicorner (void);
extern void mcc_get_area_perim ( void );
extern void mcc_init_globals ( void );
float mcc_tanh_slope_tas(float t,float vt,float vinit,float vfin, float slope);
void mcc_drive_dot_model(FILE *f, mcc_modellist *ptmodel);


