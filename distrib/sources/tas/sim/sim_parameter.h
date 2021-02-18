/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_parameter.h                                              */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

/* Acces function to simulator parameter */

extern void  sim_parameter_set_slope( sim_model *model, SIM_FLOAT alim );
extern SIM_FLOAT sim_parameter_get_slope( sim_model *model );

extern void  sim_parameter_set_alim( sim_model *model, SIM_FLOAT alim, char where );
extern SIM_FLOAT sim_parameter_get_alim( sim_model *model, char where );

extern void  sim_parameter_set_temperature( sim_model *model, SIM_FLOAT temp );
extern SIM_FLOAT sim_parameter_get_temperature( sim_model *model );

extern void  sim_parameter_set_tran_tmax( sim_model *model, SIM_FLOAT tmax );
extern SIM_FLOAT sim_parameter_get_tran_tmax( sim_model *model );

extern void  sim_parameter_set_tran_step( sim_model *model, SIM_FLOAT step );
extern SIM_FLOAT sim_parameter_get_tran_step( sim_model *model );

extern void  sim_parameter_set_simu_step( sim_model *model, SIM_FLOAT step );
extern SIM_FLOAT sim_parameter_get_simu_step( sim_model *model );

extern void  sim_parameter_set_slopeVTHH( sim_model *model, SIM_FLOAT vthh );
extern SIM_FLOAT sim_parameter_get_slopeVTHH( sim_model *model );

extern void  sim_parameter_set_slopeVTHL( sim_model *model, SIM_FLOAT vthl );
extern SIM_FLOAT sim_parameter_get_slopeVTHL( sim_model *model );

extern void  sim_parameter_set_delayVTH( sim_model *model, SIM_FLOAT vthstart, SIM_FLOAT vthend );
extern SIM_FLOAT sim_parameter_get_delayVTHSTART( sim_model *model );
extern SIM_FLOAT sim_parameter_get_delayVTHEND( sim_model *model );

extern void  sim_parameter_set_tool_cmd( sim_model *model, char *cmd );
extern char* sim_parameter_get_tool_cmd( sim_model *model );

extern void  sim_parameter_set_output_file( sim_model *model, char *out );
extern char* sim_parameter_get_output_file( sim_model *model );

extern void  sim_parameter_set_stdoutput_file( sim_model *model, char *out );
extern char* sim_parameter_get_stdoutput_file( sim_model *model );

extern void  sim_parameter_set_tool( sim_model *model, int tool );
extern int   sim_parameter_get_tool( sim_model *model );

extern void sim_parameter_set_drive_netlist( sim_model *model, char need );
extern char sim_parameter_get_drive_netlist( sim_model *model );

extern void  sim_parameter_set_remove_files( sim_model *model, char ovr );
extern char  sim_parameter_get_remove_files( sim_model *model );

extern void  sim_parameter_set_noise_type( sim_model *model, char type);
extern char  sim_parameter_get_noise_type( sim_model *model );

extern void  sim_parameter_set_allow_overwrite_files( sim_model *model, char ovr );
extern char  sim_parameter_get_allow_overwrite_files( sim_model *model );

extern chain_list* sim_parameter_get_techno_file_head( sim_model *model );
extern void sim_parameter_add_techno_file( sim_model *model, char *techno );

/* High level function for sim library */
void sim_parameter_init( sim_model *model);
extern void sim_parameter_clean( sim_model *model);
extern SIM_FLOAT sim_parameter_get_vss( sim_model *model, char where );
extern void  sim_parameter_set_vss( sim_model *model, SIM_FLOAT alim, char where );

