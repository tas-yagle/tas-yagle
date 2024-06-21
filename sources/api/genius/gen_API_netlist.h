#ifndef GEN_API_NETLIST_H 
#define GEN_API_NETLIST_H

#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include ELP_H
#include RCN_H
#include MLU_H
#include BEH_H
#include TRC_H
#include BVL_H
#include FCL_H
#include GEN_H

extern void mccenv ();

extern lofig_list *gns_FlattenNetlist             (lofig_list *fig, int rc);
extern void        gns_CleanNetlist               (lofig_list *fig);
extern void        gns_SetLoad                    (lofig_list *netlist, char *connector, double load) ;
extern void        gns_FreeFlatNetlist            (lofig_list *netlist);
extern void        gns_ReduceInstance             (lofig_list *fig, char *ins_name);
extern void        gns_AddExternalTransistors     (char *str);

extern int         gen_strcount                   (char *sig, char c);
extern void        gen_update_signal              (lofig_list *fig);
extern void        gen_replace_lofig              (lofig_list *fig_to_replace, lofig_list *new_fig);
//extern int         gen_take_this_rc               (chain_list *lofigch, int rc);
extern int         gen_search_external            (chain_list *lofigch);
extern void        gen_add_blocked_trans          (lofig_list *fig, locon_list *ptcon, losig_list *gnd, losig_list *vdd, locon_list *cir_ptcon);
extern void        gen_rcenv                      ();
extern losig_list *gen_get_ground                 (lofig_list *fig);
extern losig_list *gen_get_alim                   (lofig_list *fig);
extern double      gen_get_transistor_capa        (locon_list *cir_ptcon);
extern void        gen_add_equicapa_from_cir      (losig_list *ls, losig_list *gnd, locon_list *cir_ptcon);
extern void        gen_add_external_capas         (lofig_list* fig, corresp_t *table);
extern lotrs_list *gen_get_transistor             (lofig_list *netlist, char *tr_name);
extern locon_list *gen_get_connector              (lofig_list *netlist, char *con_name);
extern losig_list *gen_get_signal                 (lofig_list *netlist, char *signame);
extern loins_list *gen_find_ins                   (lofig_list *fig, char *insname);
extern void        gen_reduce_inslist             (lofig_list *fig, chain_list *inslist);
extern chain_list *gen_add_trs2sup                (char *insname, loins_list *ins, chain_list *trs2sup);
extern void        gen_reduce_ins                 (lofig_list *fig, char *insname);
extern void        gen_freelotrs                  (lotrs_list *trs);
extern void        gen_freelosig                  (losig_list *sig);
extern chain_list *gen_get_lofigchain             (losig_list *sig) ;
extern int         gen_isalim                     (losig_list *sig);
extern void        gen_del_flagged_losig          (lofig_list *fig, char flag);
extern void        gen_del_flagged_locon          (lofig_list *fig, char flag);
extern void        gen_del_flagged_lotrs          (lofig_list *fig, char flag);
extern int        gen_suppress_trans             (lofig_list *fig, chain_list *trs2sup, chain_list *trs2keep);
extern void        gen_set_charac_capa            (lofig_list *fig, char *locon, double capa);
extern void        gen_clean_lofig                (lofig_list *fig);
extern lofig_list *gen_getlofig                   (char *figname);
extern int         gen_verif_locon                (locon_list *con,char *insname2verif,char **fullinsname);
extern int         gen_comp_awe_delay_con         (const void *cona, const void *conb);
extern int         gen_comp_awe_delay_con_loop    (const void *cona, const void *conb);
extern void        gen_leftunconcatname           (char *name, char **left, char **right, char separ);
extern void        gen_rightunconcatname          (char *name, char **left, char **right, char separ);
extern void        gen_get_min_max_instance_index (lofig_list *fig, char *inst_radical, int *min, int *max);
extern chain_list *gen_order_locon_awe            (awelist *sig,locon_list **cmax,locon_list **cmin);
extern chain_list *gen_order_locon_awe_loop       (losig_list *sig);
extern char       *gen_get_instance_loopdelayawe  (lofig_list *lofig, char type, char *insname, locon_list *begin,double vdd, double smax, double smin, double *tmax, double *tmin, double *fmax, double *fmin);
extern char       *gns_GetWorstInstanceDelayAWE   (lofig_list *lofig, char *insname, locon_list *begin,double vdd);
extern char       *gns_GetBestInstanceDelayAWE    (lofig_list *lofig, char *insname, locon_list *begin,double vdd);
extern lofig_list *gns_KeepBestInstanceDelayAWE   (lofig_list *hierfig, lofig_list *flatfig, char *insname, locon_list *begin,double vdd);
extern lofig_list *gns_KeepWorstInstanceDelayAWE  (lofig_list *hierfig, lofig_list *flatfig, char *insname, locon_list *begin,double vdd);
extern int        gen_del_flagged_loins          (lofig_list *fig, long flag);
void gen_add_parallel_transistors (lofig_list *fig, corresp_t *table);
void gen_Destroy_MC(lofig_list *fig);
void transfert_needed_lofigs(lofig_list *lf, mbkContext *ctx);
#endif
