/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_noise.h                                                  */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                Marc   Kuoch                                               */
/*                                                                           */
/*****************************************************************************/

extern chain_list *SIM_HEAD_NOISE;

// noise function
extern SIM_FLOAT sim_find_vmax (SIM_FLOAT *tab,int ind1,int ind2,int *tvmax);
extern SIM_FLOAT sim_find_vmin (SIM_FLOAT *tab,int ind1,int ind2,int *tvmin);
extern sim_noise *sim_noise_extract (sim_model *model,char type, char *name, SIM_FLOAT vthnoise,SIM_FLOAT tinit, SIM_FLOAT tfinal);
extern sim_noise *sim_noise_add (sim_noise *hdnoise,char *name,char *nodename);
extern void sim_noise_set_vth (sim_noise *ptnoise,SIM_FLOAT vth);
extern void sim_noise_set_tab (sim_noise *ptnoise,SIM_FLOAT *tab);
extern void sim_noise_set_idxpeak (sim_noise *ptnoise,int peakindex);
extern void sim_noise_set_idxc (sim_noise *ptnoise,int cindex);
extern sim_noise *sim_noise_get (char *name, char noisetype);
extern SIM_FLOAT sim_noise_get_vth (sim_noise *noise);
extern SIM_FLOAT *sim_noise_get_tab (sim_noise *noise);
extern chain_list *sim_noise_get_idxpeak (sim_noise *noise);
extern chain_list *sim_noise_get_idxc (sim_noise *noise);
extern void sim_noise_free (sim_noise *noise);
extern void sim_noise_free_all (void);
extern SIM_FLOAT sim_noise_get_time_byindex (sim_model *model,int index);
extern SIM_FLOAT sim_noise_get_volt_byindex (sim_noise *noise,int index);
extern SIM_FLOAT sim_noise_get_min_peak_value (sim_noise *noise);
extern SIM_FLOAT sim_noise_get_max_peak_value (sim_noise *noise);
extern SIM_FLOAT sim_noise_get_min_peak_time (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_max_peak_time (sim_model *model,sim_noise *noise);
void sim_noise_get_idx_around_peak (sim_noise *noise,int peak_indice,int *before_peak,int *after_peak);
extern SIM_FLOAT sim_noise_get_time_before_peak (sim_model *model,sim_noise *noise, int peak_indice);
extern SIM_FLOAT sim_noise_get_time_after_peak (sim_model *model,sim_noise *noise, int peak_indice);
extern SIM_FLOAT sim_noise_get_peak_duration (sim_model *model,sim_noise *noise, int peak_indice);
extern int sim_noise_get_min_peak_idx (sim_model *model,sim_noise *noise);
extern int sim_noise_get_max_peak_idx (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_min_ci_before_peak (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_max_ci_before_peak (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_min_ci_after_peak (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_max_ci_after_peak (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_min_peak_duration (sim_model *model,sim_noise *noise);
extern SIM_FLOAT sim_noise_get_max_peak_duration (sim_model *model,sim_noise *noise);

