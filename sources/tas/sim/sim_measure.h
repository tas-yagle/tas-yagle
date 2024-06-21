/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_measure.h                                                */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

/* Access function to measure */

extern sim_measure *sim_measure_set_locon( sim_model *model, char *locon );
extern void         sim_measure_set_signal( sim_model *model, char *signal );
extern sim_measure *sim_measure_set_nodelist( sim_measure *m, chain_list *nodelist);
extern void         sim_measure_clear( sim_model *model, char *name, char type, char what );
extern void         sim_measure_current( sim_model *model, char *locon );

extern sim_measure* sim_measure_get( sim_model *model, char *locon ,char type, char what );
extern sim_measure* sim_measure_scan( sim_model *model, sim_measure *measure );

extern char         sim_measure_get_type( sim_measure *measure );
extern chain_list  *sim_measure_get_nodelist ( sim_measure *measure );
extern char         sim_measure_get_what( sim_measure *measure );
extern char*        sim_measure_get_name( sim_measure *measure );

/* High level functions for sim library only */
void                sim_measure_set_detail( sim_measure *measure, char *nodename,char *printname );
sim_measure_detail* sim_measure_detail_scan( sim_measure *measure, sim_measure_detail *scan );
char*               sim_measure_detail_get_nodename( sim_measure_detail *detail );
char*               sim_measure_detail_get_name( sim_measure_detail *detail );
SIM_FLOAT*          sim_measure_detail_get_data( sim_measure_detail *detail );
void                sim_measure_detail_set_data( sim_measure_detail *detail, SIM_FLOAT *data );
void                sim_measure_detail_clear_list( sim_measure *measure );
void                sim_measure_detail_clean( sim_model *model );
extern void         sim_measure_clean( sim_model *model );

/* Low level access to measure */
sim_measure*        sim_measure_alloc( void );
sim_measure_detail* sim_measure_detail_alloc( void );
void                sim_measure_detail_free( sim_measure_detail *measure );
void                sim_measure_free( sim_measure *measure );
