#ifndef GEN_CORRESP_H 
#define GEN_CORRESP_H

#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include ELP_H
#include RCN_H
#include MLU_H
#include BEH_H
#include BVL_H
#include FCL_H
#include GEN_H

extern chain_list *ALL_LOINS;
extern chain_list *ALL_LOTRS;
extern chain_list *ALL_SWAPS;
extern ptype_list *ALL_ENV;
extern lofig_list *current_lofig;
extern lofig_list *GENIUS_GLOBAL_LOFIG;
extern chain_list *ALL_HIERARCHY_LOTRS;
extern lofig_list *GEN_HEAD_LOFIG;
extern ht         *GEN_HT_LOFIG;
//extern chain_list *TRANS_TO_SUPPRESS;
extern ptype_list *GEN_USER;
extern char        GEN_SEPAR;
extern chain_list *expansedloins, *originalloins;

extern lofig_list *gen_getlofig                        (char *name);
void gen_delalllofig ();
extern char       *gen_vectorize                       (char *radix, int index);
extern char       *gen_losigname                       (losig_list *sig);
extern int         gen_upper_bound                     (losig_list *sig);
extern int         gen_lower_bound                     (losig_list *sig);
void gen_get_bounds (losig_list *sig, int *left, int *right);
//extern char       *gen_makename                        (char *s);
extern chain_list *gen_hierarchical_split              (char *name);
extern lotrs_list *gen_get_corresponding_transistor    (char *name);
extern lotrs_list *gen_get_corresponding_transistor    (char *name);
extern corresp_t  *gen_build_corresp_table             (lofig_list *lf, chain_list *loins, chain_list *lotrs);
extern void        gen_drive_corresp_htable            (FILE *f, char *name, char *path, int top, corresp_t *dico);
extern void        gen_free_corresp_table              (corresp_t *dico);
extern void        gen_drive_corresp_table             (FILE *f, char *path, int top, subinst_t *dico);
extern lotrs_list *gen_hcorresp_trs                    (char *name, corresp_t *corresp);
extern losig_list *gen_hcorresp_sig                    (char *name, corresp_t *corresp);
extern lotrs_list *gen_corresp_trs                     (char *name, corresp_t *corresp);
extern losig_list *gen_corresp_sig                     (char *name, corresp_t *corresp);
extern lofig_list *gen_add_real_corresp_ptypes         (loins_list *ins, corresp_t *father_tbl, corresp_t *tbl, int *swapped);
extern void        gen_clean_real_corresp_ptypes       (lofig_list *fig);
//void gen_swap_corresp_table (fclcorresp_list *fcl_clist, corresp_t *tbl, char *insname);
//extern void        gen_swap_corresp_table              (fclcorresp_list *fcl_clist, corresp_t *crp_table);
extern void        gen_update_corresp_table            (lofig_list *lf, corresp_t *crp_table, loins_list *li);
extern void        gen_update_corresp_table__recursive (lofig_list *lf, corresp_t *crp_table, loins_list *li);
void gen_update_alims (lofig_list *lf, corresp_t *root_tbl, chain_list *distrib_sigs);
void gen_add_all_lofig_weight();
corresp_t *create_blackbox_corresp_table(loins_list *ins);

#endif
