/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_tpiv.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

extern int TPIV_DEBUG_IBR ;

extern char         tpiv_i_brdual( link_list *head, float vout, float valim, float *imax, char savepol );
extern char         tpiv_i_brdual_leakage( link_list *head, link_list*activelink, float vout, float valim, float *imax );
extern char         tpiv_i_brdual_leakage_2( link_list *head, link_list*activelink, float vout, float valim, float *imax );
float               tpiv_get_voltage_driving_lotrs( lotrs_list *lotrs );
float               tpiv_get_blocked_voltage_driving_lotrs( lotrs_list *lotrs );
extern timing_iv*   tpiv_dualmodel( cone_list *cone, branch_list *head, link_list *active, float vref, float pconf0, float pconf1, float fin_vi, float fin_vf, float fin_vs, float vouti );

char                tpiv_calc_i( tpiv_i *model, float ve, float vs, float *is );
tpiv*               tpiv_createbranch( link_list *head, char transtype );
tpiv*               tpiv_createbranch_leakage( link_list *head, link_list *activelink, char transtype );
tpiv*               tpiv_createbranch_leakage_2( link_list *head, link_list *activelink, char transtype);
void                tpiv_freebranch( tpiv *br );
tpiv*               tpiv_alloc( void );
void                tpiv_free( tpiv *s );
void tpiv_inverter( char *fname, lotrs_list *tn, lotrs_list *tp, char sens, float f, float r, float c1, float c2, stm_driver *driver, float tmax, dualparams *scmmodel, char transtype );
void tpiv_carac_static( lotrs_list *lotrs, float vdd, float vdsmax, float vgsmax, float vb, float vs );
int tpiv_get_i_multi_input( link_list*, link_list*, float, float, float, float, char, float* );
void tas_tpiv_set_vg_for_switch( stm_solver_maillon *maillon, char brtype, char typeactive, float vgactive, float vgswitch );

double tpiv_getseuil( stm_solver_maillon *stmn, stm_solver_maillon *stmp, double vdd, double vbn, double vbp, char satured, char brtype );
double tpiv_getconflict( stm_solver_maillon *stmn, stm_solver_maillon *stmp, double vdd, double vbn, double vbp, double vt, double f, double seuil, char sens, double dt, char satured, char brtype );
double tpiv_getvs( stm_solver_maillon *stmn, stm_solver_maillon *stmp, double ve, double vbn, double vbp, double vdd, char brtype );
mcc_trans_mcc* tpiv_addtransmcc( tpiv *br, stm_solver_maillon *stmm, link_list *link );
void tpiv_addtransmcc_switch( tpiv *br, stm_solver_maillon *stmm, link_list *link, lotrs_list *lotrs );
void tas_tpiv_set_vg( stm_solver_maillon *maillon, char brtype, float vg );
void tas_tpiv_set_vb( stm_solver_maillon *maillon, char brtype, float vb );
char tas_tpiv_solver_calc_ids( stm_solver_maillon *maillon, float vgs, float vbs, float vds, float *ids, char brtype );
void tpiv_i_trace_brdual( char*, link_list*, link_list*, char, float, float, float, float, float, float );
void tpiv_clean_voltage_driving_lotrs( lotrs_list *lotrs );
void tpiv_set_voltage_driving_lotrs( lotrs_list *lotrs, float voltage );
