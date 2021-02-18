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
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

/* Value for Inode                                                            */
#define SIM_INODE_INDEX 20021104

/* Enought room for a 500 points (x,y) = 1000 */
#define TABSIZE 1000

/* Pour les générateurs de courant                                            */
typedef struct drive_current {
  struct drive_current *NEXT;
  char                 *MEASPINNAME;
  char                 *PINNAME;
  sim_measure          *MEAS;
} drive_current;

/* Toutes les variables utilisées pour le drive. */
typedef struct drive_spice{
  sim_model     *MODEL;
  FILE          *PTF;
  SIM_FLOAT      TAB[TABSIZE];
  chain_list    *DRIVEDFILE;
  char           SUBCKTFILENAME[BUFSIZ];
  char           CMDFILENAME[BUFSIZ];
  char           CURDIR[10000];
  char           MEMOUTLO[5];
  int            MEMNAMENODES;
  drive_current *CURRENT;
  int            NBCURRENT;
  int            NBPINMEAS;
  char          *OUTPUT_SPICE_FILE_NAME;
  FILE          *OUTPUT_SPICE_FILE_POINTER;
} drive_spice;

//extern char sim_run_simu_spice( sim_model *model, int run );
char sim_run_simu_spice( sim_model *model, int run, char *outputname, FILE *outputfile);
char* sim_run_simu_spice_drive_pin( char *loconname, char *pinname, drive_spice *context );
void sim_run_simu_spice_drive_measure_current( drive_spice *context );
int  sim_run_simu_spice_drive_subckt( drive_spice* );
void sim_run_simu_spice_create_cmdfilename( drive_spice* );
void sim_run_simu_spice_drive_input( drive_spice* );
void sim_run_simu_spice_drive_measure( drive_spice* );
int  sim_run_simu_spice_drive_cmd( drive_spice* );
void sim_run_simu_spice_drive_x( drive_spice* );
void sim_run_simu_spice_drive_ic( drive_spice* );
void sim_run_simu_spice_drive_remove_files( drive_spice* );
void sim_calc_func( sim_model *model, sim_input *input, SIM_FLOAT *result, int *nbelem );
void sim_run_simu_spice_drive_alim( drive_spice *context );
void sim_run_simu_spice_restoreenv( drive_spice *context );
void sim_run_simu_spice_setenv( drive_spice *context );
void sim_run_simu_spice_drive_clear_index_inode( lofig_list *lofig );
void sim_run_simu_spice_drive_set_inode( drive_spice *context, lofig_list *lofig);
void sim_run_simu_spice_drive_del_inode( drive_spice *context, lofig_list *lofig);
int sim_run_simu_spice_drive_get_index_inode( losig_list *losig );
void sim_run_simu_spice_drive_eldo_current( FILE *ptf, lofig_list *lofig, void *context );
void sim_run_simu_spice_drive_ngspice_current( FILE *ptf, lofig_list *lofig, void *context );
extern void sim_run_simu_spice_name_signal( sim_model *model, chain_list *headins, losig_list *losig, int pnode, char force, char *hiername );
extern int sim_run_simu_spice_get_one_node( losig_list *losig );
void sim_run_simu_spice_drive_delta( drive_spice *context );
