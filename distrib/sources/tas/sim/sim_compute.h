/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_compute.h                                                */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                Marc   Kuoch                                               */
/*                                                                           */
/*****************************************************************************/

extern chain_list *SIM_HEAD_TMG;

extern char sim_gettype (char type);
// Delay and slope standard extraction 
extern sim_timing *sim_GetTmgList (sim_model *model, char type_n1, char *n1, char type_n2, char *n2);
extern SIM_FLOAT   sim_get_tmg_delay (sim_timing *tmg, char dtype);
extern SIM_FLOAT   sim_get_tmg_slope (sim_timing *tmg, char dtype);
extern SIM_FLOAT   sim_getdelay   (sim_model *model, char dtype, char type_n1, char *n1, char type_n2, char *n2);
extern SIM_FLOAT sim_get_trans_delay (sim_model *model, char delaytype, char type_n1, char *n1, char type_n2, char *n2, char *trans,  SIM_FLOAT th_in, SIM_FLOAT th_out);
extern SIM_FLOAT sim_get_trans_slope (sim_model *model, char slopetype, char type, char *name, char *trans, SIM_FLOAT th_in, SIM_FLOAT th_out);
extern SIM_FLOAT sim_getslope        (sim_model *model, char slopetype, char type_node, char *node);
extern SIM_FLOAT sim_get_commut_instant (sim_model *model, char *name, char type, SIM_FLOAT voltage);
extern void      sim_get_delay_slope (sim_model *model, SIM_FLOAT *delay, SIM_FLOAT *slope, SIM_FLOAT *tab, int *indice, char *trans, SIM_FLOAT vth, SIM_FLOAT vthl, SIM_FLOAT vthh, SIM_FLOAT time);
extern char      sim_get_tc_between_2_indices (sim_model *model, SIM_FLOAT *delay, SIM_FLOAT *slope, SIM_FLOAT *tab, int *indice1, int indice2,char *trans, SIM_FLOAT vth, SIM_FLOAT vthl, SIM_FLOAT vthh, SIM_FLOAT time);

// stability function
extern SIM_FLOAT sim_get_setup_time (sim_model *model, char *data, SIM_FLOAT tstart_d, SIM_FLOAT tslope_d, char sens_d, char *cmd, SIM_FLOAT tstart_min_c, SIM_FLOAT tstart_max_c, SIM_FLOAT tslope_c, char sens_c, char *mem, int data_val);
extern SIM_FLOAT sim_get_hold_time (sim_model *model, char *data, SIM_FLOAT tstart_d, SIM_FLOAT tslope_d, char sens_d, char *cmd, SIM_FLOAT tstart_min_c, SIM_FLOAT tstart_max_c, SIM_FLOAT tslope_c, char sens_c, char *mem, int data_val);
extern SIM_FLOAT sim_get_access_time (sim_model *model, char *dout, int dout_val, char *cmd, SIM_FLOAT tstart_c, SIM_FLOAT tslope_c, char sens_c, char *mem, int data_val, double *out_slope); 
// timing function
extern sim_timing *sim_timing_add (sim_timing *head_timing, char *root4usr, char *node4usr);
extern sim_timing *sim_timing_get (char *root4usr, char *node4usr);
extern sim_timing *sim_timing_get_by_event (char *root4usr, char *node4usr, char *event);
extern void        sim_timing_setnodesname (sim_timing *head_timing, char *root, char *node);
extern void        sim_timing_setdelay   (sim_timing *pt_timing, SIM_FLOAT delay);
extern void        sim_timing_setslope   (sim_timing *pt_timing, SIM_FLOAT slope);
extern void        sim_timing_setnodeeventidx (sim_timing *pt_timing, long idx);
extern void        sim_timing_setrooteventidx (sim_timing *pt_timing, long idx);
extern void        sim_timing_setnodeevent (sim_timing *pt_timing, char event);
extern void        sim_timing_setrootevent (sim_timing *pt_timing, char event);
extern SIM_FLOAT   sim_timing_getdelay     (sim_timing *pt_timing);
extern SIM_FLOAT   sim_timing_getdelaytype (sim_timing *pt_timing,char dtype);
extern SIM_FLOAT   sim_timing_getslope     (sim_timing *pt_timing);
extern SIM_FLOAT   sim_timing_getslopetype (sim_timing *pt_timing,char stype);
extern long        sim_timing_getnodeeventidx (sim_timing *pt_timing);
extern long        sim_timing_getrooteventidx (sim_timing *pt_timing);
extern char        sim_timing_getnodeevent (sim_timing *pt_timing);
extern char        sim_timing_getrootevent (sim_timing *pt_timing);
extern void        sim_timing_free         (sim_timing *head_timing);
extern void        sim_timing_free_all     (void);
extern void        sim_timing_print        (void);
extern char        sim_get_last_node_event (sim_model *model,char *name, char type);
extern void        sim_get_nodes_events (sim_model *model,char *filename,char *node1, char type1, char *node2, char type2, char type);
extern int         sim_get_rc_delay_slope (sim_model *model,spisig_list *spisig, SIM_FLOAT *delay, SIM_FLOAT *slope, char **nodercin,char **nodercout, char type);
extern int         sim_get_rc_delay_slope_meas (sim_model *model,spisig_list *spisig, SIM_FLOAT *delay, SIM_FLOAT *slope, char type);
extern void sim_dump_delay (sim_model *model, char *name1, char *name2, char *transition, void (*func)(char *start, float starttime, float startslope, char *end, float endtime, float endslope, void *data), void *data);
