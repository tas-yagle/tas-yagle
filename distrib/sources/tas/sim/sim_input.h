/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_input.h                                                  */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

/* Access function to input */

extern void  sim_input_set_stuck_level( sim_model *model, char *locon, char level );
extern void  sim_input_set_stuck_voltage( sim_model *model, char *locon, SIM_FLOAT voltage );
extern void  sim_input_set_slope_single( sim_model *model, char *locon, char level, SIM_FLOAT slope, SIM_FLOAT tstart , char locate);
extern void  sim_input_set_slope_pattern( sim_model *model, char *locon, SIM_FLOAT trise, SIM_FLOAT tfall, SIM_FLOAT period, char *pattern );
extern void  sim_input_clear( sim_model *model, char *locon );
extern void  sim_input_set_func( sim_model *model, char *locon, SIM_FLOAT (*func)(SIM_FLOAT, void*), void *user_data, char locate, char *deltavar );

/* High level function for sim library */

SIM_FLOAT sim_input_get_latest_event( sim_model *model );
sim_input* sim_input_get( sim_model *model, char *locon );
sim_input* sim_input_scan( sim_model *model, sim_input *scan );
char       sim_input_get_type( sim_input *input );
char*      sim_input_get_name( sim_input *input );
char       sim_input_get_stuck_type( sim_input *input );
char       sim_input_get_slope_type( sim_input *input );
char       sim_input_get_stuck_level( sim_input *input );
SIM_FLOAT      sim_input_get_stuck_voltage( sim_input *input );
void       sim_input_get_slope_single( sim_input *input, char *level, SIM_FLOAT *slope, SIM_FLOAT *tstart );
void       sim_input_get_slope_pattern( sim_input *input, SIM_FLOAT *trise, SIM_FLOAT *tstop, SIM_FLOAT *period, char **pattern );
void       sim_input_init( sim_model *model );
extern void       sim_input_clean( sim_model *model );
void sim_input_get_func( sim_input *input, SIM_FLOAT (**func)( SIM_FLOAT, void* ), void **data );

/* Low level function */
sim_input* sim_input_get_newinput( sim_model *model, char *locon, char locate );
sim_input* sim_input_alloc( void );
void       sim_input_free( sim_input *input );
extern SIM_FLOAT   sim_tanh_slope (SIM_FLOAT t, void *data);
extern void       *sim_builtin_tanh ();
extern void        sim_input_set_slope_tanh (sim_model *model,char *node,double start_time,double transition_time,char sense,char *deltavar);
void sim_translate_clean(sim_model *model);
extern SIM_FLOAT sim_input_get_tstart (sim_model *model, char *loconname);
extern SIM_FLOAT sim_input_get_vdd( sim_input *input );
extern SIM_FLOAT sim_input_get_vss( sim_input *input );
extern SIM_FLOAT sim_tanh_slope_tas (SIM_FLOAT t, void *data);
extern void sim_input_set_slope_mimic( sim_model *model, char *locon, char *refnode, char *refvddnode, char *refvssnode, int revert );

