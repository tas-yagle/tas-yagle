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

#include SIM_H
#include "sim.h"


char sim_run_simu( sim_model *model, int run, char *outputfilename, FILE *outputfile)
{
  char r=0;
  int  oldSPI_TRANSISTOR_AS_INSTANCE=V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE;

  V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE=model->PARAMETER.TRANSISTOR_AS_INSTANCE;

  mfe_cleanLabel(model->LLABELS);




  switch( model->PARAMETER.TOOL ) {
  
  case SIM_TOOL_NGSPICE :
  case SIM_TOOL_LTSPICE :
  case SIM_TOOL_ELDO: 
  case SIM_TOOL_TITAN: 
  case SIM_TOOL_TITAN7: 
  case SIM_TOOL_HSPICE: 
  case SIM_TOOL_MSPICE: 
  case SIM_TOOL_SPICE:
    r = sim_run_simu_spice( model, run, outputfilename, outputfile);
    break;
    
  default :
    sim_error( "sim_run_simu(): bad value for tool.\n" );
  }
  model->OUTPUT_READ    = 0;

  V_INT_TAB[__SIM_TRANSISTOR_AS_INSTANCE].VALUE=oldSPI_TRANSISTOR_AS_INSTANCE;
  return r;
}


/*---------------------------------------------------------------------------*/

void print_white_space (FILE *file, int whitespace)
{
    int i;
    for (i = 0 ; i < whitespace ; i++)
        fprintf (file," ");
}

/*---------------------------------------------------------------------------*/

void sim_drive_plot_file (sim_model *model, chain_list *sig2plot, chain_list *name2plot)
{
    chain_list         *chain;
    SIM_FLOAT         **tab;
    SIM_FLOAT           vddmax,time,step,printstep;
    sim_measure        *measure = NULL;
    sim_measure_detail *measuredetail = NULL;
    char                datfilename[1024];
    char                pltfilename[1024];
    char                *nodename;
    int                 i,j,nbnode = 0,nbval, rcdone;
    FILE               *file;
    locon_list         *locon;
    losig_list         *losig;
    spisig_list        *spisignal;


    sprintf (datfilename,"%s.dat",model->FIGNAME);
    if (!(file = fopen (datfilename,"w"))) {
        fprintf (stderr, "sim_drive_plot_file : can't open file %s!!!\n",
                datfilename);
        return;
    }
    time = sim_parameter_get_tran_tmax (model);
    step = sim_parameter_get_tran_step (model);
    vddmax = sim_parameter_get_alim (model, 'i');
    nbval = (int)((time / step)+0.5);

    // Recupere tous les tableaux de valeurs des nodes
    fprintf (file,"#time");
    for (chain = name2plot ; chain ; chain = chain->NEXT)
        nbnode++;
    tab = (double**)mbkalloc(nbnode*sizeof (double*));
    i = 0;
    spisignal = (spisig_list *)sig2plot->DATA;
    nodename = efg_GetSpiSigLoconRc(spisignal,'I');
    if (!nodename) nodename = efg_GetSpiSigLoconRc(spisignal,'O');
    if (!nodename) nodename = sim_devect(efg_GetSpiSigName(spisignal));
    rcdone = 0;
    for (chain = name2plot ; chain ; chain = chain->NEXT) {
        if (!spisignal && rcdone) {
            fprintf (stderr,"sim_drive_plot_file : signal and name incoherency !!!\n");
            fclose (file);
            return;
        }
        if (i == 0) print_white_space (file,5);
        else print_white_space (file,9);
        fprintf (file,"%s",(char *)chain->DATA);
        tab[i] = (double*)mbkalloc(nbval*sizeof(double)) ;
        measure = NULL;
        sim_find_by_name(nodename, sim_model_get_lofig(model), &locon, &losig, NULL);
        if (locon) {
            measure = sim_measure_get(model, nodename, SIM_MEASURE_LOCON, SIM_MEASURE_VOLTAGE);
        }
        if(!measure && losig) {
            measure = sim_measure_get(model, nodename, SIM_MEASURE_SIGNAL, SIM_MEASURE_VOLTAGE);
        }
        if (!measure) {
            fprintf (stderr,"sim_drive_plot_file : no measure found on node %s !!!\n",nodename);
            fclose (file);
            return;
        }
        else {
            measuredetail = sim_measure_detail_scan(measure,NULL);
            if (!measuredetail) {
                fprintf (stderr,"sim_drive_plot_file : no measure detail found on node %s !!!\n",nodename);
                fclose (file);
                return;
            }
            else tab[i] = sim_measure_detail_get_data(measuredetail);
        }
        if (spisignal) {
            if (rcdone || !spisignal->HASRCLINE || efg_GetSpiSigLoconRc(spisignal,'O') == NULL) {
                rcdone = 0;
                sig2plot = sig2plot->NEXT;
                if (sig2plot) {
                    spisignal = (spisig_list *)sig2plot->DATA;
                    nodename = efg_GetSpiSigLoconRc(spisignal,'I');
                    if (!nodename) nodename = sim_devect(efg_GetSpiSigName(spisignal));
                }
                else spisignal = NULL;
            }
            else {
                nodename = efg_GetSpiSigLoconRc(spisignal,'O');
                rcdone = 1;
            }
        }
        else rcdone = 1;
        i++;
    }
    
    if (sig2plot) {
        fprintf (stderr,"sim_drive_plot_file : signal and name incoherency !!!\n");
        fclose (file);
        return;
    }
    // Drive the file.dat
    fprintf (file,"\n");
    printstep = 0.0;
    for (j = 0 ; j <= nbval ; j++) {
        fprintf (file, "%.3e ",printstep);
        for (i = 0 ; i < nbnode ; i++) fprintf (file, "%.3e ",tab[i][j]);
        fprintf (file, "\n");
        printstep += step ;
    }
    fprintf (file,"\n");

    mbkfree (tab);
    if (fclose(file) != 0) {
        fprintf (stderr, "sim_drive_plot_file  can't close file %s\n", datfilename);
        return;
    }
    
    // Drive the file.plt
    sprintf (pltfilename,"%s.plt",model->FIGNAME);
    if (!(file = fopen (pltfilename,"w"))) {
        fprintf (stderr, "sim_drive_plot_file : can't open file %s!!!\n", pltfilename);
        return;
    }
    fprintf (file,"set term %s\n", V_STR_TAB[__AVT_GNUPLOT_TERM].VALUE);
    fprintf(file,"set xlabel \"TIME (PS)\"\n");
    fprintf(file,"set ylabel \"VOUT (V)\"\n");
    fprintf(file,"plot [0:%e] [0.0:%d] ", time,(int)vddmax + 1);
    j = 2 ;
    for (chain = name2plot ; chain->NEXT ; chain = chain->NEXT) {
        fprintf(file,"'%s' using 1:%d title '%s' with lines, ", datfilename,j,(char *)chain->DATA);
        j++;
    }
    fprintf(file,"'%s' using 1:%d title '%s' with lines\n", datfilename,j,(char *)chain->DATA);
    fprintf(file,"pause -1 'Hit CR to finish'\n");

    if (fclose(file) != 0) {
        fprintf (stderr, "sim_drive_plot_file  can't close file %s\n", pltfilename);
        return;
    }
}
