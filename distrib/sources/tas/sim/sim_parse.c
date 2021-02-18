/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_parse.c                                                  */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Gilles Augustins                                           */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"

// Fonction de parse du fichier .chi ou .out
//

void sim_parse_spiceout (sim_model *model)
{
  if (!model->OUTPUT_READ)
  {
    int i, nbpoints = 0, nbexp;
    char *fileout, **argv;
    SIM_FLOAT **tab;
    chain_list *pt, *simpoints = NULL;
    chain_list *chain,*chain_detail = NULL;
    SIM_FLOAT time, step;
    sim_measure *m;
    sim_measure_detail *md;
    char *simfile;
    char  filename[1024];
    SIM_FLOAT *data;

    if ( V_BOOL_TAB[__SIM_USE_PRINT].VALUE ) {
      if (!(simfile = sim_get_output_file (model))) {
        fprintf (stderr, "[SIM ERR] no %s simulation output file\n", SIM_SPICEOUT);
        return;
      }
      
      time = sim_parameter_get_tran_tmax (model);
      step = sim_parameter_get_tran_step (model);
      
      m = NULL;
      while ((m = sim_measure_scan(model, m))) {
        md = NULL;
        while ((md = sim_measure_detail_scan (m, md))) 
          chain_detail = addchain (chain_detail,sim_measure_detail_get_name (md));
        for (chain = chain_detail ; chain ; chain = chain->NEXT)
          simpoints = addchain (simpoints, (char*)chain->DATA);
        if (chain_detail) {
          freechain (chain_detail);
          chain_detail = NULL;
        }
      }
      
      nbexp = (int)((time / step) + 0.5);
      for (pt = (chain_list*)simpoints; pt; pt = pt->NEXT) nbpoints++;
      
      argv=(char **)mbkalloc(nbpoints*sizeof(char *));
      tab = (SIM_FLOAT**)mbkalloc (nbpoints * sizeof (SIM_FLOAT*));

      for (i = 0; i < nbpoints; i++)
        tab[i] = (SIM_FLOAT*)mbkalloc ((nbexp +  1) * sizeof (SIM_FLOAT));
      
      simpoints = reverse (simpoints);
      /* Greg, le 15/10/03
      fileout = sim_getjoker (SIM_SPICEOUT, simfile);
      */
      fileout = sim_getjoker(sim_parameter_get_output_file(model), simfile);
      for (i = 0, pt = simpoints; pt; i++, pt = pt->NEXT)
        argv[i] = (char*)pt->DATA;
      
      sprintf (filename,"%s/%s",WORK_LIB,fileout); // car on drive le fichier de cmd en fonction du work lib
      switch( sim_parameter_get_tool( model ) ) {
      case SIM_TOOL_TITAN :
      case SIM_TOOL_TITAN7 :
        sim_parse_titan_ppr( filename, 
                             argv, 
                             nbpoints, 
                             nbexp + 1, 
                             tab, 
                             time, 
                             step
                           );
        break;
      default :
        sim_readspifile( filename, 
                         argv, 
                         nbpoints, 
                         nbexp + 1, 
                         tab, 
                         time, 
                         step
                       );
        break;
      }
      
      i = 0;
      m = NULL;
      while ((m = sim_measure_scan (model, m))) {
        if (m->DETAIL != NULL)
          m->DETAIL = (sim_measure_detail*) reverse ((chain_list*)m->DETAIL);
        md = NULL;
        while ((md = sim_measure_detail_scan (m, md))) {
          data = sim_measure_detail_get_data (md);
          if (data)
            mbkfree(data);
          sim_measure_detail_set_data (md, tab[i++]);
        }
      }
      
      mbkfree(argv);
      mbkfree(tab);
      freechain (simpoints);
      mbkfree(fileout);
      
    }
    model->OUTPUT_READ  = 1;
  }
}
