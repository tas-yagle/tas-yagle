/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : TLF Version 1.00                                            */
/*    Fichier : tlf3_drive.c                                                */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include TLF_H
#include "tlf3_drive.h" 
#include "tlf_derate.h" 
/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern ht* tut_tablasharea;
double TLF3_TIME_DIV = 1000.0 ;

/****************************************************************************/
/*     function  tlf3_drive                                                 */
/****************************************************************************/
void tlf3_drive (chain_list *figlist, chain_list *befiglist, char *name, char *suffix)
{

    FILE        *f ;
    ttvfig_list *ttvfig ;
    chain_list *ttv ;
    char        *libname;
    char        *pt;
    ttvinfo_list *ttvinfo = NULL;
    
    printf("\n\n                #################################################\n");
    printf("                #                                               #\n");
    printf("                #   Timing Library Format driver version TLF3   #\n");
    printf("                #                                               #\n");
    printf("                #################################################\n\n");
    printf("                       WRITING FILE: %s ...\n\n", name);
    
	f = mbkfopen (name, NULL, WRITE_TEXT) ;
    
    libname = strdup(name);
    if ((pt = strstr(libname,"_v3"))) *pt = '\0';
    
    if(figlist){
        if(figlist->DATA){
            ttvinfo = ((ttvfig_list*)figlist->DATA)->INFO;
        }
    }
    tlf3_driveheader(f, libname, ttvinfo);

    for(ttv = figlist ; ttv ; ttv = ttv->NEXT){
        ttvfig=(ttvfig_list*)ttv->DATA;
        
        if(befiglist){
            tlf3_drivecell (f, ttvfig, (befig_list*)befiglist->DATA, suffix);
            befiglist=befiglist->NEXT;
        }
        else
            tlf3_drivecell (f, ttvfig, NULL, suffix);
    }


	fclose (f) ;    
    printf("                       ... DONE\n\n");
}

/****************************************************************************/
/*     function  tlf3_scm2thr                                                */
/****************************************************************************/
double tlf3_scm2thr(double fscm)
{
/*    return (elpScm2Thr(fscm, STM_DEFAULT_SMINR, 
            STM_DEFAULT_SMAXR, STM_DEFAULT_VT, 
            V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, elpRISE));*/
    return fscm;
}

/****************************************************************************/
/*     function  tlf3_driveinsertion                                        */
/****************************************************************************/

void tlf3_driveinsert (FILE *f, char *buf, ttvfig_list *fig, ttvline_list *line) 
{
   timing_model      *md ;
   timing_model      *mf ;

   if (line->VALMAX != TTV_NOTIME) {
      fprintf (f, "\tinsertion_delay(\n") ;
      fprintf (f, "\t\t%s slow ", buf) ;

      if ((line->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         fprintf (f, "01 ") ;
      else
         fprintf (f, "10 ") ;

      if ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         fprintf (f, "01 ") ;
      else
         fprintf (f, "10 ") ;

      if (line->MDMAX) {
         md = stm_getmodel (fig->INFO->FIGNAME, line->MDMAX) ;
         switch (md->UTYPE) {
             case STM_MOD_MODTBL:
                if (md->UMODEL.TABLE->SET1D || md->UMODEL.TABLE->SET2D)
                    fprintf (f, "delay(%s) ", md->NAME) ;
                else
                    fprintf (f, "delay((const(%f))) ", (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT) ;
                break;
             case STM_MOD_MODPLN:
                break;
             case STM_MOD_MODSCM:
                break;
         }
      } else
         fprintf (f, "delay((const(%f))) ", (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT) ;
    
      if (line->MFMAX) {
         mf = stm_getmodel (fig->INFO->FIGNAME, line->MFMAX) ;
         switch (mf->UTYPE) {
             case STM_MOD_MODTBL:
                if (mf->UMODEL.TABLE->SET1D || mf->UMODEL.TABLE->SET2D)
                    fprintf (f, "slew(%s)", mf->NAME) ;
                else
                    fprintf (f, "slew((const(%f)))", tlf3_scm2thr((double)line->FMAX/TLF3_TIME_DIV/TTV_UNIT)) ;
                break;
             case STM_MOD_MODPLN:
                break;
             case STM_MOD_MODSCM:
                break;
         }
      } else
         fprintf (f, "slew((const(%f)))", tlf3_scm2thr((double)line->FMAX/TLF3_TIME_DIV/TTV_UNIT)) ;
      fprintf (f, "\n\t)\n\n") ;
   }

   if (line->VALMIN != TTV_NOTIME) {
      fprintf (f, "\tinsertion_delay(\n") ;
      fprintf (f, "\t\t%s fast ", buf) ;

      if ((line->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         fprintf (f, "01 ") ;
      else
         fprintf (f, "10 ") ;

      if ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
         fprintf (f, "01 ") ;
      else
         fprintf (f, "10 ") ;

      if (line->MDMIN) {
         md = stm_getmodel (fig->INFO->FIGNAME, line->MDMIN) ;
         switch (md->UTYPE) {
             case STM_MOD_MODTBL:
                if (md->UMODEL.TABLE->SET1D || md->UMODEL.TABLE->SET2D)
                    fprintf (f, "delay(%s) ", md->NAME) ;
                else
                    fprintf (f, "delay((const(%f))) ", (double)line->VALMIN/TLF3_TIME_DIV/TTV_UNIT) ;
                break;
             case STM_MOD_MODPLN:
                break;
             case STM_MOD_MODSCM:
                break;
         }
      } else
         fprintf (f, "delay((const(%f))) ", (double)line->VALMIN/TLF3_TIME_DIV/TTV_UNIT) ;
    
      if (line->MFMIN) {
         mf = stm_getmodel (fig->INFO->FIGNAME, line->MFMIN) ;
         switch (mf->UTYPE) {
             case STM_MOD_MODTBL:
                if (mf->UMODEL.TABLE->SET1D || mf->UMODEL.TABLE->SET2D)
                    fprintf (f, "slew(%s)", mf->NAME) ;
                else
                    fprintf (f, "slew((const(%f)))", tlf3_scm2thr((double)line->FMIN/TLF3_TIME_DIV/TTV_UNIT)) ;
                break;
             case STM_MOD_MODPLN:
                break;
             case STM_MOD_MODSCM:
                break;
         }
      } else
         fprintf (f, "slew((const(%f)))", tlf3_scm2thr((double)line->FMIN/TLF3_TIME_DIV/TTV_UNIT)) ;
      fprintf (f, "\n\t)\n\n") ;
   }
}


/****************************************************************************/

void tlf3_driveinsertion (FILE *f, ttvfig_list *fig) 
{
   int           i ;
   int           event ;
   ttvsig_list  *sig ;
   ptype_list   *ptype ;
   char          buf[1024] ;
   chain_list   *ch ;
   ttvline_list *line ;

   for (i = 0 ; i < fig->NBCONSIG ; i++) {
      sig = fig->CONSIG[i] ;
      ttv_expfigsig (fig, sig, fig->INFO->LEVEL, fig->INFO->LEVEL, TTV_STS_DUAL, TTV_FILE_TTX) ; 
      ttv_getsigname (fig, buf, sig) ;
         
      for (event = 0 ; event < 2 ; event++) {
         if ((ptype = getptype (sig->NODE[event].USER, TTV_NODE_DUALPATH))) {
            for (ch = (chain_list*)ptype->DATA ; ch ; ch = ch->NEXT) {
               line = (ttvline_list*)ch->DATA ;
               if ((line->ROOT->ROOT->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
                  tlf3_driveinsert (f, buf, fig, line) ;
            }
         }
      }
   }
}
    
/****************************************************************************/
/*     function  tlf3_drivecell                                             */
/****************************************************************************/
void tlf3_drivecell (FILE *f, ttvfig_list *ttvfig, befig_list *ptcellbefig, char *suffix)
{
    int n ;
    ttvsig_list *ttvsig ;
	timing_cell *cell ;
    char *name;
    int ev ;
    ttvline_list *line ;
    timing_model *tmodelmax;
    timing_model *tmodelmin;
    
    cbhseq *ptcbhseq=NULL;
    
    cell = stm_getcell (ttvfig->INFO->FIGNAME) ;
    
    name=lib_unsuffix (cell->NAME, suffix);

	fprintf (f, "cell (%s celltype (ignore)\n", name) ;
    if(tut_tablasharea)
        if(gethtitem(tut_tablasharea,name) != EMPTYHT)
            fprintf(f,"\ttiming_props(area(%.2f))\n",(double)(gethtitem(tut_tablasharea,name))/1000.0);

    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
    	for (ev = 0 ; ev < 2 ; ev++)
        	for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT){
                if(line->MDMAX && line->MDMIN){
                    tmodelmax = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMAX);
                    tmodelmin = stm_getmodel (ttvfig->INFO->FIGNAME, line->MDMIN);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        tlf3_drivemodels(f, tmodelmin, tmodelmax, 'D');
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        tlf3_drivemodels(f, tmodelmin, tmodelmax, 'D');
                    }
                }else if(line->MDMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    tlf3_drivemodel(f, tmodelmax, 'D');
                }
                if(line->MFMAX && line->MFMIN){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    tmodelmin=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMIN);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        tlf3_drivemodels(f, tmodelmin, tmodelmax, 'S');
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        tlf3_drivemodels(f, tmodelmin, tmodelmax, 'S');
                    }
                }else if(line->MFMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    tlf3_drivemodel(f, tmodelmax, 'S');
                }
            }
    }

    for (n=0; n < ttvfig->NBELCMDSIG; n++){  
        ttvsig=ttvfig->ELCMDSIG[n];
    	for (ev = 0 ; ev < 2 ; ev++)
            for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT){
                if(line->MDMIN && line->MDMAX){
                    tmodelmin=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMIN);
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf3_drivemodels(f, tmodelmin, tmodelmax, 'D');
                        }else{
                            tlf3_drivemodel(f, tmodelmax, 'D');
                            tlf3_drivemodel(f, tmodelmin, 'D');
                        }
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf3_drivemodels(f, tmodelmin, tmodelmax, 'D');
                        }else{
                            tlf3_drivemodel(f, tmodelmax, 'D');
                            tlf3_drivemodel(f, tmodelmin, 'D');
                        }
                    }
                }
                else if(line->MDMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    tlf3_drivemodel(f, tmodelmax, 'D');
                }
                if(line->MFMIN && line->MFMAX){
                    tmodelmin=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMIN);
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf3_drivemodels(f, tmodelmin, tmodelmax, 'S');
                        }else{
                            tlf3_drivemodel(f, tmodelmax, 'S');
                            tlf3_drivemodel(f, tmodelmin, 'S');
                        }
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf3_drivemodels(f, tmodelmin, tmodelmax, 'S');
                        }else{
                            tlf3_drivemodel(f, tmodelmax, 'S');
                            tlf3_drivemodel(f, tmodelmin, 'S');
                        }
                    }
                }
                else if(line->MFMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    tlf3_drivemodel(f, tmodelmax, 'S');
                }
            }
    }    

    if(ptcellbefig)
        ptcbhseq = cbh_getseqfunc(ptcellbefig, CBH_TLF_MODE);
    
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if (((ttvsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI) 
         && ((ttvsig->TYPE & TTV_SIG_CB)!=TTV_SIG_CB)
         && ((ttvsig->TYPE & TTV_SIG_CT)!=TTV_SIG_CT))
            tlf3_drivepin(f, ttvfig, ttvsig, NULL, NULL);
    }
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if (((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)
         || ((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ))
            tlf3_drivepin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq);
    }
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if ((ttvsig->TYPE & TTV_SIG_CB)==TTV_SIG_CB)
            tlf3_drivepin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq);
    }
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if (((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO)
         && ((ttvsig->TYPE & TTV_SIG_CB)!=TTV_SIG_CB)
         && ((ttvsig->TYPE & TTV_SIG_CT)!=TTV_SIG_CT)
         && ((ttvsig->TYPE & TTV_SIG_CZ)!=TTV_SIG_CZ))
            tlf3_drivepin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq);
    }
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if ((ttvsig->TYPE & TTV_SIG_CX)==TTV_SIG_CX)
            tlf3_drivepin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq);
    }
    
    tlf3_driveinsertion (f, ttvfig) ;

    if(ptcbhseq)
        tlf3_driveseqfunc(f,ptcbhseq);

    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
    	for (ev = 0 ; ev < 2 ; ev++)
        	for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT)
                tlf3_drivepath(f,line);
    }
        
    
	fprintf(f, ")\n\n") ;

}

/****************************************************************************/
/*     function  tlf3_drivepath                                             */
/****************************************************************************/
void tlf3_drivepath (FILE *f, ttvline_list *line)
{
    ttvevent_list *node,*root;
    char *nodename,*rootname,*firstr,*secotr,*namemd,*namemf;
    long nodetype,roottype;

    node=line->NODE;
    root=line->ROOT;
    nodename=node->ROOT->NAME;
    rootname=root->ROOT->NAME;
    nodetype=node->TYPE;
    roottype=root->TYPE;

    
    
   	if((line->TYPE & TTV_LINE_U)==TTV_LINE_U){
        if((nodetype & TTV_NODE_UP)==TTV_NODE_UP)
            firstr="01";
        else
            firstr="10";
        if((roottype & TTV_NODE_UP)==TTV_NODE_UP)
            secotr="posedge";
        else
            secotr="negedge";
        if(line->MDMAX)
            fprintf(f, "\tsetup(%s => %s %s %s %s)\n",nodename, rootname, firstr, secotr, line->MDMAX);
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\tsetup(%s => %s %s %s const(%.3f))\n",nodename, rootname, firstr, secotr, (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT);

    }else if((line->TYPE & TTV_LINE_O)==TTV_LINE_O){
        if((nodetype & TTV_NODE_UP)==TTV_NODE_UP)
            firstr="01";
        else
            firstr="10";
        if((roottype & TTV_NODE_UP)==TTV_NODE_UP)
            secotr="posedge";
        else
            secotr="negedge";
        if(line->MDMAX)
            fprintf(f, "\thold(%s => %s %s %s %s)\n",nodename, rootname, firstr, secotr, line->MDMAX);
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\thold(%s => %s %s %s const(%.3f))\n",nodename, rootname, firstr, secotr, (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT);

   	}else if((line->TYPE & TTV_LINE_A)==TTV_LINE_A){
        if((nodetype & TTV_NODE_UP)==TTV_NODE_UP)
            firstr="01";
        else
            firstr="10";
        if((roottype & TTV_NODE_UP)==TTV_NODE_UP)
            secotr="01";
        else
            secotr="10";
        if(line->MDMAX && line->MDMIN){
            namemd=lib_unsuffix(line->MDMAX, "_max");
            namemf=lib_unsuffix(line->MFMAX, "_max");
            fprintf(f, "\tpath(%s => %s %s %s delay(%s) slew(%s))\n",nodename, rootname, firstr, secotr, namemd, namemf);
        }else if(line->MDMAX)
            fprintf(f, "\tpath(%s => %s %s %s delay(%s) slew(%s))\n",nodename, rootname, firstr, secotr, line->MDMAX, line->MFMAX);
        else if(line->VALMAX!=TTV_NOTIME && line->VALMIN!=TTV_NOTIME && line->VALMAX!=line->VALMIN)
            fprintf(f, "\tpath(%s => %s %s %s delay(const(%.3f::%.3f)) slew(const(%.3f::%.3f)))\n",nodename, rootname, firstr, secotr, (double)line->VALMIN/TLF3_TIME_DIV/TTV_UNIT, (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT, tlf3_scm2thr((double)line->FMIN/TLF3_TIME_DIV/TTV_UNIT), tlf3_scm2thr((double)line->FMAX/TLF3_TIME_DIV/TTV_UNIT));
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\tpath(%s => %s %s %s delay(const(%.3f)) slew(const(%.3f)))\n",nodename, rootname, firstr, secotr, (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT, tlf3_scm2thr((double)line->FMAX/TLF3_TIME_DIV/TTV_UNIT));

    }else{
        if((nodetype & TTV_NODE_UP)==TTV_NODE_UP)
            firstr="01";
        else
            firstr="10";
        if((roottype & TTV_NODE_UP)==TTV_NODE_UP)
            secotr="01";
        else
            secotr="10";
        if(line->MDMAX && line->MDMIN){
            namemd=lib_unsuffix(line->MDMAX, "_max");
            namemf=lib_unsuffix(line->MFMAX, "_max");
            fprintf(f, "\tpath(%s => %s %s %s delay(%s) slew(%s))\n",nodename, rootname, firstr, secotr, namemd, namemf);
        }else if(line->MDMAX)
            fprintf(f, "\tpath(%s => %s %s %s delay(%s) slew(%s))\n",nodename, rootname, firstr, secotr, line->MDMAX, line->MFMAX);
        else if(line->VALMAX!=TTV_NOTIME && line->VALMIN!=TTV_NOTIME && line->VALMAX!=line->VALMIN)
            fprintf(f, "\tpath(%s => %s %s %s delay(const(%.3f::%.3f)) slew(const(%.3f::%.3f)))\n",nodename, rootname, firstr, secotr, (double)line->VALMIN/TLF3_TIME_DIV/TTV_UNIT, (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT, tlf3_scm2thr((double)line->FMIN/TLF3_TIME_DIV/TTV_UNIT), tlf3_scm2thr((double)line->FMAX/TLF3_TIME_DIV/TTV_UNIT));
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\tpath(%s => %s %s %s delay(const(%.3f)) slew(const(%.3f)))\n",nodename, rootname, firstr, secotr, (double)line->VALMAX/TLF3_TIME_DIV/TTV_UNIT, tlf3_scm2thr((double)line->FMAX/TLF3_TIME_DIV/TTV_UNIT));
    }        

}

/****************************************************************************/
/*     function  tlf3_driveseqfunc                                          */
/****************************************************************************/
void tlf3_driveseqfunc (FILE *f, cbhseq *ptcbhseq)
{
    if(ptcbhseq){
        if(ptcbhseq->SEQTYPE==CBH_LATCH){
            fprintf(f,"\tlatch(\n");
            if(ptcbhseq->NAME){
                fprintf(f,"\t\toutput (%s)\n",ptcbhseq->NAME);
            }
            if(ptcbhseq->NEGNAME){
                fprintf(f,"\t\tinverted_output (%s)\n",ptcbhseq->NEGNAME);
            }
            if(ptcbhseq->RESET){
                fprintf(f,"\t\tclear (");
                cbh_writeabl(f,ptcbhseq->RESET, 'T');
                fprintf(f,")\n");
            }
            if(ptcbhseq->SET){
                fprintf(f,"\t\tset (");
                cbh_writeabl(f,ptcbhseq->SET, 'T');
                fprintf(f,")\n");
            }
            if(ptcbhseq->CLOCK){
                fprintf(f,"\t\tclock (");
                cbh_writeabl(f,ptcbhseq->CLOCK, 'T');
                fprintf(f,")\n");
            }else{
                fprintf(f,"\t\tclock (1'b0)\n");
            }
            if(ptcbhseq->DATA){
                fprintf(f,"\t\tinput (");
                cbh_writeabl(f,ptcbhseq->DATA, 'T');
                fprintf(f,")\n");
            }else{
                fprintf(f,"\t\tinput (1'b0)\n");
            }
            fprintf(f,"\t)\n");
        }else if(ptcbhseq->SEQTYPE==CBH_FLIPFLOP){
            fprintf(f,"\tregister(\n");
            if(ptcbhseq->NAME){
                fprintf(f,"\t\toutput (%s)\n",ptcbhseq->NAME);
            }
            if(ptcbhseq->NEGNAME){
                fprintf(f,"\t\tinverted_output (%s)\n",ptcbhseq->NEGNAME);
            }
            if(ptcbhseq->RESET){
                fprintf(f,"\t\tclear (");
                cbh_writeabl(f,ptcbhseq->RESET, 'T');
                fprintf(f,")\n");
            }
            if(ptcbhseq->SET){
                fprintf(f,"\t\tset (");
                cbh_writeabl(f,ptcbhseq->SET, 'T');
                fprintf(f,")\n");
            }
            if(ptcbhseq->CLOCK){
                fprintf(f,"\t\tclock (");
                cbh_writeabl(f,ptcbhseq->CLOCK, 'T');
                fprintf(f,")\n");
            }else{
                fprintf(f,"\t\tclock (1'b0)\n");
            }
            if(ptcbhseq->DATA){
                fprintf(f,"\t\tinput (");
                cbh_writeabl(f,ptcbhseq->DATA, 'T');
                fprintf(f,")\n");
            }else{
                fprintf(f,"\t\tinput (1'b0)\n");
            }
            fprintf(f,"\t)\n");
        }
        
    }
}

/****************************************************************************/
/*     function  tlf3_drivepin                                              */
/****************************************************************************/
void tlf3_drivepin (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, befig_list *ptcellbefig, cbhseq *ptcbhseq)
{
    
    bepor_list *ptbepor=NULL;

    if(ptcellbefig){
        for(ptbepor=ptcellbefig->BEPOR; ptbepor; ptbepor=ptbepor->NEXT){
            if(ptbepor->NAME==ttv_devect(ttvsig->NAME)) break;
        }
    }
    
    fprintf(f,"\tpin (%s ",ttvsig->NAME);
    tlf3_drivedirection(f, ttvsig, ptbepor);
    tlf3_drivecombfunc(f,ttv_devect(ttvsig->NAME),ptcellbefig,ptcbhseq);
    tlf3_drivecapacitance(f, ttvfig, ttvsig);
    fprintf(f,"\t)\n");
}

/****************************************************************************/
/*     function  tlf3_drivecombfunc                                         */
/****************************************************************************/
void tlf3_drivecombfunc (FILE *f, char *name, befig_list *ptcellbefig, cbhseq *ptcbhseq)
{
    cbhcomb    *ptcbhcomb;

    if(ptcellbefig){
        ptcbhcomb = cbh_getcombfunc(ptcellbefig, ptcbhseq, name);
        if(ptcbhcomb){
            if(ptcbhcomb->FUNCTION){
                fprintf(f,"\t\tfunction  (");
                cbh_writeabl(f, ptcbhcomb->FUNCTION, 'T');
                fprintf(f,")\n");
            }
            if(ptcbhcomb->HZFUNC){
                fprintf(f,"\t\tenable  (");
                cbh_writeabl(f, notExpr(ptcbhcomb->HZFUNC), 'T');
                fprintf(f,")\n");
            }
        }
    }


}



/****************************************************************************/
/*     function  tlf3_drivecapacitance                                      */
/****************************************************************************/
void tlf3_drivecapacitance (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig)
{
    if(((ttvsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO) || (V_BOOL_TAB[__TMA_DRIVECAPAOUT].VALUE == 1))
        fprintf(f,"\t\ttiming_props(pin_cap(%.4f))\n",((double)ttvsig->CAPA)/TLF_CAPA_DIV);

#ifndef __ALL__WARNING_
	ttvfig = NULL;
#endif
}

/****************************************************************************/
/*     function  tlf3_drivedirection                                        */
/****************************************************************************/
void tlf3_drivedirection (FILE *f, ttvsig_list *ttvsig, bepor_list *ptbepor)
{
    char *dir ;
    char *type;
    ptype_list *ptypeck, *ptypeasyn;
    ttvline_list *line;
    int ev;
        
    if ((ttvsig->TYPE & TTV_SIG_CX)==TTV_SIG_CX){
        if(ptbepor){
            switch(ptbepor->DIRECTION){
                case 'I' :  dir="input";  break;
                case 'O' :  dir="output";  break;
                case 'Z' :  dir="output";  break;
                case 'T' :  dir="bidir";  break;
                case 'B' :  dir="output"; break;
                case 'X' :  dir="input"; break;
                default  :  dir="input";
            }
        }else{
            dir="input";
        }
        type="data";
    }
    
    else if ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT){
        dir="bidir";
        type="data tristate";
    }
    else if ((ttvsig->TYPE & TTV_SIG_CB)==TTV_SIG_CB){
        dir="output";
        type="data";
    }
    else if ((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ){
        dir="output";
        type="data tristate";
    }
    else if ((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO){
        dir="output";
        type="data";
    }
    else if ((ttvsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI){
        ptypeck=getptype(ttvsig->USER, TTV_SIG_CLOCK);
        ptypeasyn=getptype(ttvsig->USER, TTV_SIG_ASYNCHRON);
        if(ptypeck){
            dir="input";
            for (ev = 0 ; ev < 2 ; ev++){
                for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT){
   	                if((line->TYPE & TTV_LINE_U)==TTV_LINE_U){
                        if((line->ROOT->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
                            type="clock posedge";
                        }else{
                            type="clock negedge";
                        }
                        break;
                    }
                }
                type="data";
            }
        }else if(ptypeasyn){
            dir="input";
            type="control asynch";
        }else{
            dir="input";
            type="data";
        }
    }

    fprintf(f,"pintype(%s) pindir(%s)\n", type, dir);

}

/****************************************************************************/
/*     function  tlf3_driveheader                                           */
/****************************************************************************/
void tlf3_driveheader (FILE *f, char *libname, ttvinfo_list *ttvinfo)
{
  /*
	time_t    counter ;
	char     *date ;
    */
    double voltage;
    double temp, smin, smax, dth;
    char         buffer[1024];
	
    if(!ttvinfo){
        voltage=V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
        temp=STM_DEFAULT_TEMP;
        smin=STM_DEFAULT_SMINR;
        smax=STM_DEFAULT_SMAXR;
        dth = 0.5;
    }else{
        voltage = ttvinfo->VDD;
        temp = ttvinfo->TEMP;
        smax = ttvinfo->STHHIGH;
        smin = ttvinfo->STHLOW;
        dth = ttvinfo->DTH;
    }
    if(dth < 0.0){
        dth = 0.5;
    }
        
    sprintf(buffer,"tlf3 data flow `%s`\n",libname);
    avt_printExecInfo(f, "//", buffer, "");

  	/*time (&counter) ;
	date = ctime (&counter) ;
	date[strlen (date) - 1] = '\0' ;
    */
	fprintf (f, "header (\n") ;
	fprintf (f, "\tlibrary (\"%s\")\n", libname) ;
	//fprintf (f, "\tdate (\"%s\")\n", date) ;
	fprintf (f, "\ttlf_version (\"3.0\")\n") ;
    //fprintf (f, "\tgenerated_by(\"TMA\")\n") ;
	fprintf (f, ")\n\n") ;

    if(V_BOOL_TAB[__TUT_CALC_DERATE].VALUE) {
        if(V_FLOAT_TAB[__TUT_MIN_VOLT].VALUE < TUT_MINTEMPVOLT)
            TUT_MINVOLT = 0.9 * voltage ;
        if(V_FLOAT_TAB[__TUT_MAX_VOLT].VALUE < TUT_MINTEMPVOLT)
            TUT_MAXVOLT = 1.1 * voltage ;
        if(V_FLOAT_TAB[__TUT_MIN_TEMP].VALUE < TUT_MINTEMPVOLT)
            TUT_MINTEMP = 0.5 * temp ;
        if(V_FLOAT_TAB[__TUT_MAX_TEMP].VALUE < TUT_MINTEMPVOLT)
            TUT_MAXTEMP = 1.5 * temp ;
 
        tlf_drive_derate(f,TUT_MINTEMP,temp,TUT_MAXTEMP, TUT_MINVOLT,voltage,TUT_MAXVOLT) ;
    }

    fprintf (f, "timing_props(\n");
	fprintf (f, "\tvoltage (%.2f)\n", voltage) ;
	fprintf (f, "\ttemperature (%.1f)\n", temp) ;
    
    fprintf (f, "\ttable_input_threshold (%.2f)\n", dth);
    fprintf (f, "\ttable_output_threshold (%.2f)\n", dth);
    if((smin < 0.0) || (smax < 0.0))
        fprintf (stderr, "***tlf warning: slew thresholds are not set***\n"); 
    if(smin < 0.0){
        smin = 0.2 ;
    }
    if(smax < 0.0){
        smax = 0.8 ;
    }
    fprintf (f, "\ttable_transition_start (%.2f)\n", smin);
    fprintf (f, "\ttable_transition_end (%.2f)\n", smax);
    fprintf (f, ")\n\n");
            
}

/****************************************************************************/
/*     function  tlf3_drivemodel                                           */
/****************************************************************************/
void tlf3_drivemodel (FILE *f, timing_model *model, char type)
{
	int   i, j ;

    timing_table *table = model->UMODEL.TABLE;

	fprintf (f, "\tmodel (%s\n", model->NAME) ;
	if (!table->NX && !table->NY)
		fprintf (f, "\t\t(const\n") ;
	else
		fprintf (f, "\t\t(spline\n") ;
	
	/* axe X */
	
	if (table->NX > 0) {
		fprintf (f, "\t\t\t(") ;
		switch (table->XTYPE) {
			case STM_INPUT_SLEW :
				fprintf (f, "input_slew_axis") ;
				break ;
			case STM_CLOCK_SLEW :
				fprintf (f, "clock_slew_axis") ;
				break ;
			case STM_LOAD :
				fprintf (f, "load_axis") ;
				break ;
		}
        if((table->XTYPE == STM_INPUT_SLEW) || (table->XTYPE == STM_CLOCK_SLEW))
    		for (i = 0 ; i < table->NX ; i++) {
		        fprintf (f, " %.5f", tlf3_scm2thr(table->XRANGE[i]/TLF3_TIME_DIV)) ;
            }
        else
    		for (i = 0 ; i < table->NX ; i++) 
    	        fprintf (f, " %.5f", table->XRANGE[i]/TLF3_TIME_DIV) ;
		fprintf (f, ")\n") ;
	}
	
	/* axe Y */
	
	if (table->NY > 0) {
		fprintf (f, "\t\t\t(") ;
		switch (table->YTYPE) {
			case STM_INPUT_SLEW :
				fprintf (f, "input_slew_axis") ;
				break ;
			case STM_CLOCK_SLEW :
				fprintf (f, "clock_slew_axis") ;
				break ;
			case STM_LOAD :
				fprintf (f, "load_axis") ;
				break ;
		}
        if((table->YTYPE == STM_INPUT_SLEW) || (table->YTYPE == STM_CLOCK_SLEW))
	    	for (i = 0 ; i < table->NY ; i++) {
	            fprintf (f, " %.5f", tlf3_scm2thr(table->YRANGE[i]/TLF3_TIME_DIV)) ;
            }
        else
	    	for (i = 0 ; i < table->NY ; i++) 
	            fprintf (f, " %.5f", table->YRANGE[i]/TLF3_TIME_DIV) ;
		fprintf (f, ")\n") ;
	}

	/* const */
	
	if (table->CST != STM_NOVALUE){
        if(type == 'S')
            fprintf(f, "\t\t\t(%.5f)\n", tlf3_scm2thr(table->CST/TLF3_TIME_DIV)) ;
        else
            fprintf(f, "\t\t\t(%.5f)\n", table->CST/TLF3_TIME_DIV) ;
    }
	
	/* table 1D */

	if (table->SET1D) {
		fprintf(f, "\t\t\t(\n") ;
		fprintf(f, "\t\t\t\t(") ;
		for (i = 0 ; i < table->NX - 1 ; i++){
            if(type == 'S')
                fprintf (f, "%.5f ", tlf3_scm2thr(table->SET1D[i]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f ", table->SET1D[i]/TLF3_TIME_DIV) ;
        }
        if(type == 'S')
            fprintf (f, "%.5f)\n", tlf3_scm2thr(table->SET1D[table->NX - 1]/TLF3_TIME_DIV)) ;
        else
            fprintf (f, "%.5f)\n", table->SET1D[table->NX - 1]/TLF3_TIME_DIV) ;
        fprintf (f, "\t\t\t)\n") ;
	}
	
	/* table 2D */

	if (table->SET2D) {
		fprintf (f, "\t\t\t(\n") ;
		for (i = 0 ; i < table->NX - 1; i++) {
			fprintf (f, "\t\t\t\t(") ;
			for (j = 0 ; j < table->NY - 1; j++){
                if(type == 'S')
                    fprintf (f, "%.5f ", tlf3_scm2thr(table->SET2D[i][j]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f ", table->SET2D[i][j]/TLF3_TIME_DIV) ;
            }
            if(type == 'S')
                fprintf (f, "%.5f)\n", tlf3_scm2thr(table->SET2D[i][table->NY - 1]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f)\n", table->SET2D[i][table->NY - 1]/TLF3_TIME_DIV) ;
		}
		fprintf (f, "\t\t\t\t(") ;
		for (j = 0 ; j < table->NY - 1; j++){
            if(type == 'S')
                fprintf (f, "%.5f ", tlf3_scm2thr(table->SET2D[table->NX - 1][j]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f ", table->SET2D[table->NX - 1][j]/TLF3_TIME_DIV) ;
        }
        if(type == 'S')
            fprintf (f, "%.5f)\n", tlf3_scm2thr(table->SET2D[table->NX - 1][table->NY - 1]/TLF3_TIME_DIV)) ;
        else
            fprintf (f, "%.5f)\n", table->SET2D[table->NX - 1][table->NY - 1]/TLF3_TIME_DIV) ;
		fprintf (f, "\t\t\t)\n") ;
	}

	fprintf (f, "\t\t)\n") ;
	fprintf (f, "\t)\n\n") ;
}

/****************************************************************************/
/*     function  tlf3_drivemodels                                           */
/****************************************************************************/
void tlf3_drivemodels (FILE *f, timing_model *modelmin, timing_model *modelmax, char type)
{
	int   i, j ;
    char *name ;

    timing_table *tablemin = modelmin->UMODEL.TABLE;
    timing_table *tablemax = modelmax->UMODEL.TABLE;
    
    name = lib_unsuffix(modelmax->NAME, "_max");

	fprintf (f, "\tmodel (%s\n", name) ;
	if (!tablemax->NX && !tablemax->NY)
		fprintf (f, "\t\t(const\n") ;
	else
		fprintf (f, "\t\t(spline\n") ;
	
	/* axe X */
	
	if (tablemax->NX > 0) {
		fprintf (f, "\t\t\t(") ;
		switch (tablemax->XTYPE) {
			case STM_INPUT_SLEW :
				fprintf (f, "input_slew_axis") ;
				break ;
			case STM_CLOCK_SLEW :
				fprintf (f, "clock_slew_axis") ;
				break ;
			case STM_LOAD :
				fprintf (f, "load_axis") ;
				break ;
		}
        if((tablemax->XTYPE == STM_INPUT_SLEW) || (tablemax->XTYPE == STM_CLOCK_SLEW))
		    for (i = 0 ; i < tablemax->NX ; i++) {
    		    fprintf (f, " %.5f", tlf3_scm2thr(tablemax->XRANGE[i]/TLF3_TIME_DIV)) ;
            }
        else
		    for (i = 0 ; i < tablemax->NX ; i++) 
			    fprintf (f, " %.5f", tablemax->XRANGE[i]/TLF3_TIME_DIV) ;
		fprintf (f, ")\n") ;
	}
	
	/* axe Y */
	
	if (tablemax->NY > 0) {
		fprintf (f, "\t\t\t(") ;
		switch (tablemax->YTYPE) {
			case STM_INPUT_SLEW :
				fprintf (f, "input_slew_axis") ;
				break ;
			case STM_CLOCK_SLEW :
				fprintf (f, "clock_slew_axis") ;
				break ;
			case STM_LOAD :
				fprintf (f, "load_axis") ;
				break ;
		}
        if((tablemax->YTYPE == STM_INPUT_SLEW) || (tablemax->YTYPE == STM_CLOCK_SLEW))
    		for (i = 0 ; i < tablemax->NY ; i++) 
        		fprintf (f, " %.5f", tlf3_scm2thr(tablemax->YRANGE[i]/TLF3_TIME_DIV)) ;
        else
    		for (i = 0 ; i < tablemax->NY ; i++) 
	    		fprintf (f, " %.5f", tablemax->YRANGE[i]/TLF3_TIME_DIV) ;
		fprintf (f, ")\n") ;
	}

	/* const */
	
	if (tablemax->CST != STM_NOVALUE){
        if(tablemin){
            if(type == 'S')
                fprintf(f, "\t\t\t(%.5f::%.5f)\n", tlf3_scm2thr(tablemin->CST/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->CST/TLF3_TIME_DIV)) ;
            else
                fprintf(f, "\t\t\t(%.5f::%.5f)\n", tablemin->CST/TLF3_TIME_DIV, tablemax->CST/TLF3_TIME_DIV) ;
        }else{
            if(type == 'S')
                fprintf(f, "\t\t\t(%.5f)\n", tlf3_scm2thr(tablemax->CST/TLF3_TIME_DIV)) ;
            else
                fprintf(f, "\t\t\t(%.5f)\n", tablemax->CST/TLF3_TIME_DIV) ;
        }            
    }
            
            
	
	/* table 1D */

	if (tablemax->SET1D) {
		fprintf(f, "\t\t\t(\n") ;
		fprintf(f, "\t\t\t\t(") ;
		for (i = 0 ; i < tablemax->NX - 1 ; i++){
            if(tablemin){
                if(type == 'S')
                    fprintf (f, "%.5f::%.5f ", tlf3_scm2thr(tablemin->SET1D[i]/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->SET1D[i]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f::%.5f ", tablemin->SET1D[i]/TLF3_TIME_DIV, tablemax->SET1D[i]/TLF3_TIME_DIV) ;
            }else{
                if(type == 'S')
                    fprintf (f, "%.5f ", tlf3_scm2thr(tablemax->SET1D[i]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f ", tablemax->SET1D[i]/TLF3_TIME_DIV) ;
            }                
        }
        if(tablemin){
            if(type == 'S')
                fprintf (f, "%.5f::%.5f)\n", tlf3_scm2thr(tablemin->SET1D[tablemin->NX - 1]/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->SET1D[tablemax->NX - 1]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f::%.5f)\n", tablemin->SET1D[tablemin->NX - 1]/TLF3_TIME_DIV, tablemax->SET1D[tablemax->NX - 1]/TLF3_TIME_DIV) ;
        }else{
            if(type == 'S')
                fprintf (f, "%.5f)\n", tlf3_scm2thr(tablemax->SET1D[tablemax->NX - 1]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f)\n", tablemax->SET1D[tablemax->NX - 1]/TLF3_TIME_DIV) ;
        }            
        fprintf (f, "\t\t\t)\n") ;
	}
	
	/* table 2D */

	if (tablemax->SET2D) {
		fprintf (f, "\t\t\t(\n") ;
		for (i = 0 ; i < tablemax->NX - 1; i++) {
			fprintf (f, "\t\t\t\t(") ;
			for (j = 0 ; j < tablemax->NY - 1; j++) {
                if(tablemin){
                    if(type == 'S')
                        fprintf (f, "%.5f::%.5f ", tlf3_scm2thr(tablemin->SET2D[i][j]/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->SET2D[i][j]/TLF3_TIME_DIV)) ;
                    else
                        fprintf (f, "%.5f::%.5f ", tablemin->SET2D[i][j]/TLF3_TIME_DIV, tablemax->SET2D[i][j]/TLF3_TIME_DIV) ;
                }else{
                    if(type == 'S')
                        fprintf (f, "%.5f ", tlf3_scm2thr(tablemax->SET2D[i][j]/TLF3_TIME_DIV)) ;
                    else
                        fprintf (f, "%.5f ", tablemax->SET2D[i][j]/TLF3_TIME_DIV) ;
                }   
            }
            if(tablemin){
                if(type == 'S')
                    fprintf (f, "%.5f::%.5f)\n", tlf3_scm2thr(tablemin->SET2D[i][tablemin->NY - 1]/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->SET2D[i][tablemax->NY - 1]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f::%.5f)\n", tablemin->SET2D[i][tablemin->NY - 1]/TLF3_TIME_DIV, tablemax->SET2D[i][tablemax->NY - 1]/TLF3_TIME_DIV) ;
            }else{
                if(type == 'S')
                    fprintf (f, "%.5f)\n", tlf3_scm2thr(tablemax->SET2D[i][tablemax->NY - 1]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f)\n", tablemax->SET2D[i][tablemax->NY - 1]/TLF3_TIME_DIV) ;
            }   
		}
		fprintf (f, "\t\t\t\t(") ;
		for (j = 0 ; j < tablemax->NY - 1; j++) {
            if(tablemin){
                if(type == 'S')
                    fprintf (f, "%.5f::%.5f ", tlf3_scm2thr(tablemin->SET2D[tablemin->NX - 1][j]/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->SET2D[tablemax->NX - 1][j]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f::%.5f ", tablemin->SET2D[tablemin->NX - 1][j]/TLF3_TIME_DIV, tablemax->SET2D[tablemax->NX - 1][j]/TLF3_TIME_DIV) ;
            }else{
                if(type == 'S')
                    fprintf (f, "%.5f ", tlf3_scm2thr(tablemax->SET2D[tablemax->NX - 1][j]/TLF3_TIME_DIV)) ;
                else
                    fprintf (f, "%.5f ", tablemax->SET2D[tablemax->NX - 1][j]/TLF3_TIME_DIV) ;
            }                
        }
        if(tablemin){
            if(type == 'S')
                fprintf (f, "%.5f::%.5f)\n", tlf3_scm2thr(tablemin->SET2D[tablemin->NX - 1][tablemin->NY - 1]/TLF3_TIME_DIV), tlf3_scm2thr(tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f::%.5f)\n", tablemin->SET2D[tablemin->NX - 1][tablemin->NY - 1]/TLF3_TIME_DIV, tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1]/TLF3_TIME_DIV) ;
        }else{
            if(type == 'S')
                fprintf (f, "%.5f)\n", tlf3_scm2thr(tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1]/TLF3_TIME_DIV)) ;
            else
                fprintf (f, "%.5f)\n", tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1]/TLF3_TIME_DIV) ;
        }            

		fprintf (f, "\t\t\t)\n") ;
	}

	fprintf (f, "\t\t)\n") ;
	fprintf (f, "\t)\n\n") ;


}

