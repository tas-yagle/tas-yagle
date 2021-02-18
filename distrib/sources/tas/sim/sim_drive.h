/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_drive.h                                                  */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire AVOT                                              */
/*                                                                           */
/*****************************************************************************/

extern char sim_run_simu( sim_model *model, int run, char *outputfilename, FILE *outputfile);
//extern char sim_run_simu( sim_model *model, int run );
extern void        sim_drive_plot_file (sim_model *model,chain_list *node2plot, chain_list *name2plot);
extern void sim_DriveContext(sim_model *sm, FILE *f);
