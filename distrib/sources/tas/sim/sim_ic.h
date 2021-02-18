/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_ic.h                                                     */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

/* Access function for IC */

extern void sim_ic_set_voltage( sim_model *model, char *name, char locate, SIM_FLOAT voltage );
extern sim_ic *sim_ic_set_level( sim_model *model, char *name, char locate, char level );
extern void sim_ic_clear( sim_model *model, char *name, char locate );

/* High level function for sim library */
sim_ic* sim_ic_get_newic( sim_model *model, char *name, char locate );
char    sim_ic_get_type( sim_ic *ic );
chain_list *sim_ic_get_nodelist ( sim_ic *ic );
sim_ic *sim_ic_set_nodelist ( sim_ic *ic , chain_list *nodelist );
char    sim_ic_get_level( sim_ic *ic );
SIM_FLOAT   sim_ic_get_voltage( sim_ic *ic );
char*   sim_ic_get_name( sim_ic *ic );
sim_ic* sim_ic_get( sim_model *model, char *name, char locate );
sim_ic* sim_ic_scan( sim_model *model, sim_ic *scan );
extern void    sim_ic_clean( sim_model *model );
char    sim_ic_get_locate( sim_ic *ic );

/* Low level function */
sim_ic* sim_ic_alloc( void );
void sim_ic_free( sim_ic *ic );
extern SIM_FLOAT sim_ic_get_vdd( sim_ic *ic );
extern SIM_FLOAT sim_ic_get_vss( sim_ic *ic );

