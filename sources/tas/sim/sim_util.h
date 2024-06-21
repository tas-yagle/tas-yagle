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

#define SIM_MARKED 20021015
extern char   SIM_TR_AS_INS;

extern void simenv ();
void sim_error( char*, ... );
void sim_warning( char*, ... );
chain_list* sim_get_hier_list( char *hiername );
chain_list* sim_getfiglist( lofig_list *lofig );
chain_list *sim_getallfiglist(lofig_list *lofig );
extern void sim_find_by_name(char *name, lofig_list *topfig, locon_list **loconfound, losig_list **losigfound, chain_list **listins );
extern locon_list *sim_find_locon(char *name, lofig_list *topfig, locon_list **loconfound, chain_list **listins );

/*************************************************************/
/* FUNCTION                                                  */
/*************************************************************/
extern char *sim_vect (char *v);
extern char *sim_devect (char *v);
extern char *sim_spivect (char *v);
extern char *sim_spidevect (char *v);
extern char *sim_vectorize (char *radix, int index);
extern void sim_dec2bin (char *bin, unsigned long dec, int begin, int end);
extern void sim_hex2bin (char *bin, char *hex);
extern void sim_node_clean(sim_model *model, char *name);

// zinaps:
sim_model *simDuplicate(sim_model *src);
lofig_list *sim_GetNetlist(char *name);
sim_model *sim_SaveContext(sim_model *src, char *label);
void sim_DriveContext(sim_model *sm, FILE *f);


extern void sim_set_ctx(sim_model *ctx);
extern sim_model *sim_get_ctx();

extern char *sim_get_hier_signame (sim_model *model, char *signal);
extern int sim_SetPowerSupply (lofig_list *lofig);

extern void sim_get_THR(sim_model *model, SIM_FLOAT *th_in, SIM_FLOAT *th_out, SIM_FLOAT *slopelow, SIM_FLOAT *slopehigh);
extern void sim_get_multivoltage_values (cone_list *cone, losig_list *ls, double *vssl, double *vssh, double *vddl, double *vddh);
extern void sim_set_result_file_extension(char mode, int dc, char **var);

