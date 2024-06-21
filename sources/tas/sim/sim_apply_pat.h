/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_apply_pat.h                                              */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Marc KUOCH                                                 */
/*                                                                           */
/*****************************************************************************/

extern char sim_get_type_node        (sim_model *model,char *node);
extern void sim_apply_input          (sim_model *model,spisig_list *path, SIM_FLOAT start_time,SIM_FLOAT slope, int delay_type);
extern void sim_apply_stuck          (sim_model *model, ptype_list *stucklist,cnsfig_list *cnsfig);
extern void sim_apply_init_print     (sim_model *model, spisig_list *path);
extern sim_model *sim_apply_pat      (lofig_list *fig, SIM_FLOAT start_time,SIM_FLOAT slope, ptype_list *patterns, spisig_list *path, int runspice, int delay_type, char multiplepath, cnsfig_list *cnsfig, int nbmc, chain_list *correl);
extern sim_model *sim_apply_pat2cone  (lofig_list *fig, cnsfig_list *cnsfig, cone_list *cone, char *input, char in_event, SIM_FLOAT slope, char out_event, float capaout, int delay_type);
extern void sim_get_delay_slope_cone (lofig_list *origlofig, cnsfig_list *cnsfig, cone_list *cone, char *input, char in_event, SIM_FLOAT in_slope, char out_event, SIM_FLOAT capaout,SIM_FLOAT *delay, SIM_FLOAT *slope, int delay_type);
extern char *sim_create_label        ( char *name1, char *name2);
extern void sim_apply_init_meas      (sim_model *model, spisig_list *path, int delay_type, char multiplepath );
extern void sim_apply_alims          (sim_model *model, lofig_list *fig);
extern void sim_add_label (spisig_list *spisig,  long type, char *label);
extern void sim_free_label (spisig_list *spisig);
extern char *sim_get_label_delay (spisig_list *spisig, long type);
