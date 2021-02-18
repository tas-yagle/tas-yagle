/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 2.00                                             */
/*    Fichier : sim_obj.c                                                    */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/

/* Acces function to simulator model */

extern sim_model* sim_model_create( char *name, int nbmc );
extern sim_model* sim_model_get( char *name );
extern void       sim_model_clear( char *name );

extern void              sim_init(void);
extern sim_model*        sim_model_alloc( void );
extern void              sim_model_free( sim_model *model );

extern char*             sim_get_output_file( sim_model *model );
extern void              sim_set_output_file( sim_model *model, char *file );

extern lofig_list*       sim_model_get_lofig( sim_model *model );
extern void              sim_model_set_lofig( sim_model *model, lofig_list *lofig );
extern void sim_input_add_alias(sim_model *model, char *src, char *dest);
extern char *sim_input_get_alias(sim_model *model, char *src, sim_translate **ret_st);
extern void sim_model_clear_bypointer( sim_model *model);
extern void sim_update_model_locon_name (sim_model *model, char *oldname, char *newname);
extern void sim_update_model_signal_name (sim_model *model, char *prefix);
extern void sim_shift_input (sim_model *model, SIM_FLOAT shift);
extern void sim_model_merge (sim_model *srcmodel, sim_model *model,char *insname);
extern void sim_free_context (sim_model *model);
extern sim_model* sim_model_create_new( char *name );
extern void sim_set_filename(sim_model *model,char *filename);

extern void sim_set_external_capa(sim_model *model, char *con, SIM_FLOAT value);
extern void sim_clean_netlist(sim_model *model);

extern void sim_resetmeasures(sim_model *model);
