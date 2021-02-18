/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : TLF Version 1.00                                            */
/*    Fichier : tlf4_drive.c                                                */
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
#include "tlf4_drive.h" 
#include "tlf_derate.h" 
/****************************************************************************/
/*     defines                                                              */
/****************************************************************************/

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

extern ht* tut_tablasharea;
char buftunitdmin[1024];
char buftunitdmax[1024];
char buftunitfmin[1024];
char buftunitfmax[1024];


/****************************************************************************/
/*     function  tlf4_drive                                                 */
/****************************************************************************/
void tlf4_drive (chain_list *figlist, chain_list *befiglist, char *name, char *suffix)
{

    FILE        *f ;
    ttvfig_list *ttvfig ;
    chain_list  *ttv ;
    double temp = STM_DEFAULT_TEMP, voltage = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    char        *libname;
    char        *pt;
    ttvinfo_list *ttvinfo = NULL;
	
    if(figlist){
        if(figlist->DATA){
            ttvinfo = ((ttvfig_list*)figlist->DATA)->INFO;
            voltage = ttvinfo->VDD;
            temp = ttvinfo->TEMP;
        }
    }
    
    printf("\n\n                #################################################\n");
    printf("                #                                               #\n");
    printf("                #   Timing Library Format driver version TLF4   #\n");
    printf("                #                                               #\n");
    printf("                #################################################\n\n");
    printf("                       WRITING FILE: %s ...\n\n", name);
    
    f = mbkfopen (name, NULL, WRITE_TEXT) ;
    
    libname = strdup(name);
    if ((pt = strstr(libname,"_v4"))) *pt = '\0';
    
    tlf4_driveheader(f, libname);
    if(V_BOOL_TAB[__TUT_CALC_DERATE].VALUE) {
        /* drive the derating models */
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

    tlf4_driveproperty(f, ttvinfo);

    for(ttv = figlist ; ttv ; ttv = ttv->NEXT){
        ttvfig=(ttvfig_list*)ttv->DATA;
        
        if(befiglist){
            tlf4_drivecell (f, ttvfig, (befig_list*)befiglist->DATA, suffix);
            befiglist=befiglist->NEXT;
        }
        else
            tlf4_drivecell (f, ttvfig, NULL, suffix);
    }

    fclose (f) ;    
    printf("                       ... DONE\n\n");
}

/****************************************************************************/
/*     function  tlf4_scm2thr                                                */
/****************************************************************************/
double tlf4_scm2thr(double fscm)
{
/*    return (elpScm2Thr(fscm, STM_DEFAULT_SMINR, 
            STM_DEFAULT_SMAXR, STM_DEFAULT_VT, 
            V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, elpRISE));*/
    return fscm;
}

/****************************************************************************/
/*     function  tlf4_driveinsert                                           */
/****************************************************************************/

void tlf4_driveinsert (FILE *f, char *buf, ttvfig_list *fig, ttvline_list *line) 
{
   timing_model      *md ;
   timing_model      *mf ;

   if (line->VALMAX != TTV_NOTIME) {
      fprintf (f, "\tinsertion_delay (\n") ;
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
         if (md->UMODEL.TABLE->SET2D || md->UMODEL.TABLE->SET1D || md->UMODEL.TABLE->CST)
            fprintf (f, "delay (%s) ", md->NAME) ;
         else
            fprintf (f, "delay ((const (%s))) ", ptlf4long(buftunitdmax, line->VALMAX)) ;
      } else
         fprintf (f, "delay ((const (%s))) ", ptlf4long(buftunitdmax, line->VALMAX)) ;
    
      if (line->MFMAX) {
         mf = stm_getmodel (fig->INFO->FIGNAME, line->MFMAX) ;
         if (mf->UMODEL.TABLE->SET2D || mf->UMODEL.TABLE->SET1D || mf->UMODEL.TABLE->CST)
            fprintf (f, "slew (%s)", mf->NAME) ;
         else
            fprintf (f, "slew ((const (%s)))", ptlf4long(buftunitfmax, tlf4_scm2thr(line->FMAX))) ;
      } else
         fprintf (f, "slew ((const (%s)))", ptlf4long(buftunitfmax, tlf4_scm2thr(line->FMAX))) ;
      fprintf (f, "\n\t)\n\n") ;
   }

   if (line->VALMIN != TTV_NOTIME) {
      fprintf (f, "\tinsertion_delay (\n") ;
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
         if (md->UMODEL.TABLE->SET2D || md->UMODEL.TABLE->SET1D || md->UMODEL.TABLE->CST)
            fprintf (f, "delay (%s) ", md->NAME) ;
         else
            fprintf (f, "delay ((const (%s))) ", ptlf4long(buftunitdmin, line->VALMIN)) ;
      } else
         fprintf (f, "delay ((const (%s))) ", ptlf4long(buftunitdmin, line->VALMIN)) ;
    
      if (line->MFMIN) {
         mf = stm_getmodel (fig->INFO->FIGNAME, line->MFMIN) ;
         if (mf->UMODEL.TABLE->SET2D || mf->UMODEL.TABLE->SET1D || mf->UMODEL.TABLE->CST)
            fprintf (f, "slew (%s)", mf->NAME) ;
         else
            fprintf (f, "slew ((const (%s)))", ptlf4long(buftunitfmin, tlf4_scm2thr(line->FMIN))) ;
      } else
         fprintf (f, "slew ((const (%s)))", ptlf4long(buftunitfmin, tlf4_scm2thr(line->FMIN))) ;
      fprintf (f, "\n\t)\n\n") ;
   }
}


/****************************************************************************/

void tlf4_driveinsertion (FILE *f, ttvfig_list *fig) 
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
                  tlf4_driveinsert (f, buf, fig, line) ;
            }
         }
      }
   }
}
    
    
/****************************************************************************/
/*     function  tlf4_get_chainhz                                           */
/****************************************************************************/
chain_list *tlf4_get_chainhz(ttvfig_list *ttvfig)
{
    chain_list *chain = NULL, *chainhz = NULL;
    int ev, n;
    ttvsig_list *ttvsig;
    ttvline_list *line;
    
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        for (ev = 0 ; ev < 2 ; ev++)
            for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT)
                if((line->TYPE & TTV_LINE_HZ) == TTV_LINE_HZ){
                    if(chainhz){
                        for(chain = chainhz; chain; chain=chain->NEXT){
                            if(line->NODE->ROOT == chain->DATA) break ;
                            else chainhz = addchain(chainhz, ttvsig);
                        }
                    }else chainhz = addchain(chainhz, line->NODE->ROOT);
                }
    }
    return chainhz;
}

/****************************************************************************/
/*     function  tlf4_drivecell                                             */
/****************************************************************************/
void tlf4_drivecell (FILE *f, ttvfig_list *ttvfig, befig_list *ptcellbefig, char *suffix)
{
    
    cbhseq *ptcbhseq=NULL;
    int n, ninter ;
    ttvsig_list *ttvsig, *bussig ;
    timing_cell *cell ;
    char *name;
    int ev ;
    ttvline_list *line ;
    timing_model *tmodelmax;
    timing_model *tmodelmin;
    int flagbus = 0, lsb = 0, msb = 0;
    char *previousname = NULL, *devectname, *busname;
    chain_list *chainhz = NULL;
    
    
    cell = stm_getcell (ttvfig->INFO->FIGNAME) ;
    
    name=lib_unsuffix (cell->NAME, suffix);

    fprintf (f, "cell (%s\n", name) ;
        
    chainhz = tlf4_get_chainhz(ttvfig);
    
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        for (ev = 0 ; ev < 2 ; ev++)
            for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT){
                if(line->MDMAX && line->MDMIN){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    tmodelmin=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMIN);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        tlf4_drivemodels(f, tmodelmin, tmodelmax, 'D');
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        tlf4_drivemodels(f, tmodelmin, tmodelmax, 'D');
                    }
                }else if(line->MDMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    tlf4_drivemodel(f, tmodelmax, 'D');
                }
                if(line->MFMAX && line->MFMIN){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    tmodelmin=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMIN);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        tlf4_drivemodels(f, tmodelmin, tmodelmax, 'S');
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        tlf4_drivemodels(f, tmodelmin, tmodelmax, 'S');
                    }
                }else if(line->MFMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    tlf4_drivemodel(f, tmodelmax, 'S');
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
                            tlf4_drivemodels(f, tmodelmin, tmodelmax, 'D');
                        }else{
                            tlf4_drivemodel(f, tmodelmax, 'D');
                            tlf4_drivemodel(f, tmodelmin, 'D');
                        }
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf4_drivemodels(f, tmodelmin, tmodelmax, 'D');
                        }else{
                            tlf4_drivemodel(f, tmodelmax, 'D');
                            tlf4_drivemodel(f, tmodelmin, 'D');
                        }
                    }
                }
                else if(line->MDMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    tlf4_drivemodel(f, tmodelmax, 'D');
                }
                if(line->MFMIN && line->MFMAX){
                    tmodelmin=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMIN);
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    if (stm_modtbl_sametablesize (tmodelmin->UMODEL.TABLE, tmodelmax->UMODEL.TABLE))
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf4_drivemodels(f, tmodelmin, tmodelmax, 'S');
                        }else{
                            tlf4_drivemodel(f, tmodelmax, 'S');
                            tlf4_drivemodel(f, tmodelmin, 'S');
                        }
                    else {
                        stm_modtbl_fitonmax(&tmodelmin->UMODEL.TABLE, &tmodelmax->UMODEL.TABLE);
                        if(strcmp(line->ROOT->ROOT->NAME,"insertion")){
                            tlf4_drivemodels(f, tmodelmin, tmodelmax, 'S');
                        }else{
                            tlf4_drivemodel(f, tmodelmax, 'S');
                            tlf4_drivemodel(f, tmodelmin, 'S');
                        }
                    }
                }
                else if(line->MFMAX){
                    tmodelmax=stm_getmodel(ttvfig->INFO->FIGNAME, line->MFMAX);
                    tlf4_drivemodel(f, tmodelmax, 'S');
                }
            }
    }


    if(tut_tablasharea)
        if(gethtitem(tut_tablasharea,name) != EMPTYHT)
            fprintf(f,"\tarea (%.2f)\n",(double)(gethtitem(tut_tablasharea,name))/1000.0);

    
    if(ptcellbefig)
        ptcbhseq = cbh_getseqfunc(ptcellbefig, CBH_TLF_MODE);

    flagbus = 0;
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];

            devectname = ttv_devect(ttvsig->NAME);
            if(flagbus)
                if(strcmp(busname,vectorradical(devectname))){
                    flagbus = 0;
                    fprintf(f, "\t)\n\n");
                }
            if((ttvsig->NAME != devectname) && (!flagbus)){
                ninter = n;
                flagbus = 1;
                lsb = vectorindex(devectname);
                busname = vectorradical(devectname);
                bussig = ttvsig;
                for(ninter=n; ninter < ttvfig->NBCONSIG; ninter++){
                    ttvsig = ttvfig->CONSIG[ninter];
                    devectname = ttv_devect(ttvsig->NAME);
                    if((vectorradical(devectname) != previousname) && (n != ninter))
                        break;
                    previousname = vectorradical(devectname);
                    msb = vectorindex(devectname);
                }
                fprintf(f, "\tbus (%s[%d:%d]\n", busname, msb, lsb);
                tlf4_drivebusdirection (f, bussig, NULL);

            }
            ttvsig = ttvfig->CONSIG[n];
            tlf4_drivepin(f, ttvsig, ptcellbefig, ptcbhseq, flagbus);
            if(flagbus)
                if((n + 1)  == ttvfig->NBCONSIG){
                    flagbus = 0;
                    fprintf(f, "\t)\n");
                }
    }
    if(flagbus){
        flagbus = 0;
        fprintf(f, "\t)\n\n");
    }
    
    tlf4_driveinsertion (f, ttvfig) ;

    if(ptcbhseq)
        tlf4_driveseqfunc(f,ptcbhseq);

    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        for (ev = 0 ; ev < 2 ; ev++)
            for(line = (&ttvsig->NODE[ev])->INPATH ; line ; line = line->NEXT)
                tlf4_drivepath(f,line,chainhz);
    }
        
    
    fprintf(f, ")\n\n") ;
    freechain(chainhz);

}

/****************************************************************************/
/*     function  tlf4_is_type_enable                                        */
/****************************************************************************/
int tlf4_is_type_enable(ttvsig_list *ttvsig, chain_list *chainhz)
{
    chain_list *chain;
    for(chain=chainhz; chain; chain=chain->NEXT)
        if(ttvsig == chain->DATA){
            return 1;
        }
    return 0;
}

/****************************************************************************/
/*     function  tlf4_drivepath                                             */
/****************************************************************************/
void tlf4_drivepath (FILE *f, ttvline_list *line, chain_list *chainhz)
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
            fprintf(f, "\tsetup (%s => %s %s %s %s)\n",nodename, rootname, firstr, secotr, line->MDMAX);
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\tsetup (%s => %s %s %s (const (%s)))\n",nodename, rootname, firstr, secotr, ptlf4long(buftunitdmax, line->VALMAX));

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
            fprintf(f, "\thold (%s => %s %s %s %s)\n",nodename, rootname, firstr, secotr, line->MDMAX);
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\thold (%s => %s %s %s (const (%s)))\n",nodename, rootname, firstr, secotr, ptlf4long(buftunitdmax, line->VALMAX));

    }else if((line->TYPE & TTV_LINE_A)==TTV_LINE_A){
        if((nodetype & TTV_NODE_UP)==TTV_NODE_UP)
            firstr="01";
        else
            firstr="10";
        if((roottype & TTV_NODE_UP)==TTV_NODE_UP){
            if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)
                secotr="0Z";
            else if(tlf4_is_type_enable(line->NODE->ROOT, chainhz))
                secotr="Z1";
            else
                secotr="01";
        }else{
            if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)
                secotr="1Z";
            else if(tlf4_is_type_enable(line->NODE->ROOT, chainhz))
                secotr="Z0";
            else
                secotr="10";
        }
        if(line->MDMAX && line->MDMIN){
            namemd=lib_unsuffix(line->MDMAX, "_max");
            namemf=lib_unsuffix(line->MFMAX, "_max");
            fprintf(f, "\tpath (%s => %s %s %s delay (%s) slew (%s))\n",nodename, rootname, firstr, secotr, namemd, namemf);
        }else if(line->MDMAX)
            fprintf(f, "\tpath (%s => %s %s %s delay (%s) slew (%s))\n",nodename, rootname, firstr, secotr, line->MDMAX, line->MFMAX);
        else if(line->VALMAX!=TTV_NOTIME && line->VALMIN!=TTV_NOTIME && line->VALMAX!=line->VALMIN)
            fprintf(f, "\tpath (%s => %s %s %s delay (const (%s::%s)) slew (const (%s::%s)))\n",nodename, rootname, firstr, secotr, ptlf4long(buftunitdmin, line->VALMIN), ptlf4long(buftunitdmax, line->VALMAX), ptlf4long(buftunitfmin, tlf4_scm2thr(line->FMIN)), ptlf4long(buftunitfmax, tlf4_scm2thr(line->FMAX)));
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\tpath (%s => %s %s %s delay (const (%s)) slew (const (%s)))\n",nodename, rootname, firstr, secotr, ptlf4long(buftunitdmax, line->VALMAX), ptlf4long(buftunitfmax, tlf4_scm2thr(line->FMAX)));

    }else{
        if((nodetype & TTV_NODE_UP)==TTV_NODE_UP)
            firstr="01";
        else
            firstr="10";
        if((roottype & TTV_NODE_UP)==TTV_NODE_UP){
            if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)
                secotr="0Z";
            else if(tlf4_is_type_enable(line->NODE->ROOT, chainhz))
                secotr="Z1";
            else
                secotr="01";
        }else{
            if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)
                secotr="1Z";
            else if(tlf4_is_type_enable(line->NODE->ROOT, chainhz))
                secotr="Z0";
            else
                secotr="10";
        }
        if(line->MDMAX && line->MDMIN){
            namemd=lib_unsuffix(line->MDMAX, "_max");
            namemf=lib_unsuffix(line->MFMAX, "_max");
            fprintf(f, "\tpath (%s => %s %s %s delay (%s) slew (%s))\n",nodename, rootname, firstr, secotr, namemd, namemf);
        }else if(line->MDMAX)
            fprintf(f, "\tpath (%s => %s %s %s delay (%s) slew (%s))\n",nodename, rootname, firstr, secotr, line->MDMAX, line->MFMAX);
        else if(line->VALMAX!=TTV_NOTIME && line->VALMIN!=TTV_NOTIME && line->VALMAX!=line->VALMIN)
            fprintf(f, "\tpath (%s => %s %s %s delay (const (%s::%s)) slew (const (%s::%s)))\n",nodename, rootname, firstr, secotr, ptlf4long(buftunitdmin, line->VALMIN), ptlf4long(buftunitdmax, line->VALMAX), ptlf4long(buftunitfmin, tlf4_scm2thr(line->FMIN)), ptlf4long(buftunitfmax, tlf4_scm2thr(line->FMAX)));
        else if(line->VALMAX!=TTV_NOTIME)
            fprintf(f, "\tpath (%s => %s %s %s delay (const (%s)) slew (const (%s)))\n",nodename, rootname, firstr, secotr, ptlf4long(buftunitdmax, line->VALMAX), ptlf4long(buftunitfmax, tlf4_scm2thr(line->FMAX)));
    }        

}

/****************************************************************************/
/*     function  tlf4_driveseqfunc                                          */
/****************************************************************************/
void tlf4_driveseqfunc (FILE *f, cbhseq *ptcbhseq)
{
    if(ptcbhseq){
        if(ptcbhseq->SEQTYPE==CBH_LATCH){
            fprintf(f,"\tlatch (\n");
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
            fprintf(f,"\tregister (\n");
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
/*     function  tlf4_drivepin                                              */
/****************************************************************************/
void tlf4_drivepin (FILE *f, ttvsig_list *ttvsig, befig_list *ptcellbefig, cbhseq *ptcbhseq, int bus)
{
    bepor_list *ptbepor=NULL;

    if(ptcellbefig){
        for(ptbepor=ptcellbefig->BEPOR; ptbepor; ptbepor=ptbepor->NEXT){
            if(ptbepor->NAME==ttv_devect(ttvsig->NAME)) break;
        }
    }
    
    
    if (bus) fprintf (f, "\t");
    fprintf(f,"\tpin (%s\n",ttvsig->NAME);
    tlf4_drivedirection(f, ttvsig, ptbepor, bus);
    tlf4_drivecombfunc(f,ttv_devect(ttvsig->NAME),ptcellbefig,ptcbhseq, bus);
    tlf4_drivecapacitance(f, ttvsig, bus);
    if (bus) fprintf (f, "\t");
    fprintf(f,"\t)\n\n");

}

/****************************************************************************/
/*     function  tlf4_drivecombfunc                                         */
/****************************************************************************/
void tlf4_drivecombfunc (FILE *f, char *name, befig_list *ptcellbefig, cbhseq *ptcbhseq, int bus)
{
    cbhcomb    *ptcbhcomb;

    if(ptcellbefig){
        ptcbhcomb = cbh_getcombfunc(ptcellbefig, ptcbhseq, name);
        if(ptcbhcomb){
            if(ptcbhcomb->FUNCTION){
                if (bus) fprintf (f, "\t");
                fprintf(f,"\t\tfunction (");
                cbh_writeabl(f, ptcbhcomb->FUNCTION, 'T');
                fprintf(f,")\n");
            }
            if(ptcbhcomb->HZFUNC){
                if (bus) fprintf (f, "\t");
                fprintf(f,"\t\tenable (");
                cbh_writeabl(f, notExpr(ptcbhcomb->HZFUNC), 'T');
                fprintf(f,")\n");
            }
        }
    }


}


/****************************************************************************/
/*     function  tlf4_drivecapacitance                                      */
/****************************************************************************/
void tlf4_drivecapacitance (FILE *f, ttvsig_list *ttvsig, int bus)
{
    if(((ttvsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO) || (V_BOOL_TAB[__TMA_DRIVECAPAOUT].VALUE == 1)){
        if (bus) fprintf (f, "\t");
        fprintf(f,"\t\tcapacitance (%.1f)\n",((double)ttvsig->CAPA));
    }

}

/****************************************************************************/
/*     function  tlf4_drivedirection                                        */
/****************************************************************************/
void tlf4_drivedirection (FILE *f, ttvsig_list *ttvsig, bepor_list *ptbepor, int bus)
{
    char *dir, *type=NULL ;
    ptype_list *ptypeck;
    
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
    }

    
    else if ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)
        dir="bidir";
    else if ((ttvsig->TYPE & TTV_SIG_CB)==TTV_SIG_CB)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI){
        dir="input";
        ptypeck=getptype(ttvsig->USER, TTV_SIG_CLOCK);
        if(ptypeck){
            type="clock_pin";
        }
    }
    if (bus) fprintf (f, "\t");
    fprintf(f,"\t\tpintype (%s)\n", dir);
    if(type) {
        if (bus) fprintf (f, "\t");
        fprintf(f,"\t\t%s\n", type);
    }
}

/****************************************************************************/
/*     function  tlf4_drivebusdirection                                     */
/****************************************************************************/
void tlf4_drivebusdirection (FILE *f, ttvsig_list *ttvsig, befig_list *ptcellbefig)
{
    bepor_list *ptbepor=NULL;
    char *dir ;
    

    if(ptcellbefig){
        for(ptbepor=ptcellbefig->BEPOR; ptbepor; ptbepor=ptbepor->NEXT){
            if(ptbepor->NAME==ttvsig->NAME) break;
        }
    }
    
    if ((ttvsig->TYPE & TTV_SIG_CX)==TTV_SIG_CX){
        if(ptbepor){
            switch(ptbepor->DIRECTION){
                case 'I' :  dir="input";  break;
                case 'O' :  dir="output";  break;
                case 'Z' :  dir="output";  break;
                case 'T' :  dir="bidir";  break;
                case 'B' :  dir="output"; 
            }
        }else{
            dir="input";
        }
    }

    
    else if ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)
        dir="bidir";
    else if ((ttvsig->TYPE & TTV_SIG_CB)==TTV_SIG_CB)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI)
        dir="input";
    
    fprintf(f,"        bustype (%s)\n", dir);

}

/****************************************************************************/
/*     function  tlf4_driveheader                                           */
/****************************************************************************/
void tlf4_driveheader (FILE *f, char *libname)
{
    /*
    time_t    counter ;
    char     *date ;

    time (&counter) ;
    date = ctime (&counter) ;
    date[strlen (date) - 1] = '\0' ;
    */
    char         buffer[1024];
    
    sprintf(buffer,"tlf4.3 data flow `%s`\n",libname);
    avt_printExecInfo(f, "//", buffer, "");
    
    fprintf (f, "header (\n") ;
    fprintf (f, "\tlibrary (\"%s\")\n", libname) ;
    //fprintf (f, "\tdate (\"%s\")\n", date) ;
    fprintf (f, "\ttlf_version (\"4.3\")\n") ;
    //fprintf (f, "\tgenerated_by (\"TMA\")\n") ;
    fprintf (f, ")\n\n") ;

}

/****************************************************************************/
/*     function  tlf4_driveproperty                                         */
/****************************************************************************/
void tlf4_driveproperty (FILE *f, ttvinfo_list *ttvinfo)
{
    double voltage;
    double temp, sminr, smaxr, sminf, smaxf, dth;
    
    if(!ttvinfo){
        voltage=V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
        temp=STM_DEFAULT_TEMP;
        sminr=STM_DEFAULT_SMINR;
        smaxr=STM_DEFAULT_SMAXR;
        sminf=STM_DEFAULT_SMINF;
        smaxf=STM_DEFAULT_SMAXF;
        dth = 0.5;
    }else{
        voltage = ttvinfo->VDD;
        temp = ttvinfo->TEMP;
        smaxr = smaxf = ttvinfo->STHHIGH;
        sminr = sminf = ttvinfo->STHLOW;
        dth = ttvinfo->DTH;
    }
    if(dth < 0.0){
        dth = 0.5;
    }
    
    fprintf (f, "properties (\n") ;
    fprintf (f, "\tunit (\n") ;
    if(tut_tablasharea)
        fprintf (f, "\t\tarea_unit (1squ)\n");
    fprintf (f, "\t\tcap_unit (1fF)\n");
    fprintf (f, "\t\ttemperature_unit (1c)\n");
    fprintf (f, "\t\ttime_unit (1%s)\n", V_STR_TAB[__TMA_TUNIT].VALUE);
    fprintf (f, "\t\tvolt_unit (1v)\n");
    fprintf (f, "\t)\n") ;
    fprintf (f, "\tvoltage (%.2f)\n", voltage);
    fprintf (f, "\ttemperature (%.1f)\n", temp);
    fprintf (f, "\tinput_threshold_pct (rise (%.1f) fall (%.1f))\n", dth*100, dth*100);
    fprintf (f, "\toutput_threshold_pct (rise (%.1f) fall (%.1f))\n", dth*100, dth*100);
    if((sminr < 0.0) || (smaxr < 0.0) || (sminf < 0.0) || (smaxf < 0.0))
        fprintf (stderr, "***tlf warning: slew thresholds are not set***\n"); 
    if(sminr < 0.0){
        sminr = 0.2;
    }
    if(smaxr < 0.0){
        smaxr = 0.8;
    }
    if(sminf < 0.0){
        sminf = 0.2;
    }
    if(smaxf < 0.0){
        smaxf = 0.8;
    }
    fprintf (f, "\tslew_lower_threshold_pct (rise (%.1f) fall (%.1f))\n", sminr*100, sminf*100);
    fprintf (f, "\tslew_upper_threshold_pct (rise (%.1f) fall (%.1f))\n", smaxr*100, smaxf*100);
    
    fprintf (f, ")\n\n") ;
} 

/****************************************************************************/
/*     function  tlf4_drivemodel                                            */
/****************************************************************************/
void tlf4_drivemodel (FILE *f, timing_model *model, char type)
{
	int   i, j ;
    timing_table *table = model->UMODEL.TABLE;

	fprintf (f, "\ttiming_model (%s\n", model->NAME) ;
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
		for (i = 0 ; i < table->NX ; i++) 
            if((table->XTYPE==STM_INPUT_SLEW)||(table->XTYPE==STM_CLOCK_SLEW)){
                fprintf(f," %s", ptlf4float(buftunitdmax,tlf4_scm2thr(table->XRANGE[i])));
            }
            else
                fprintf(f," %.2f", table->XRANGE[i]);
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
		for (i = 0 ; i < table->NY ; i++) 
            if((table->YTYPE==STM_INPUT_SLEW)||(table->YTYPE==STM_CLOCK_SLEW)){
                fprintf(f," %s", ptlf4float(buftunitdmax,tlf4_scm2thr(table->YRANGE[i])));
            }
            else
                fprintf(f," %.2f", table->YRANGE[i]);
		fprintf (f, ")\n") ;
	}

	/* const */
	
	if (table->CST != STM_NOVALUE){
        if(type == 'S')
            fprintf(f, "\t\t\t(%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(table->CST))) ;
        else
            fprintf(f, "\t\t\t(%s)\n", ptlf4float(buftunitdmax, table->CST)) ;
    }            
            
	
	/* table 1D */

	if (table->SET1D) {
		fprintf(f, "\t\t\t(\n") ;
		fprintf(f, "\t\t\t\t(") ;
		for (i = 0 ; i < table->NX - 1 ; i++){
            if(type == 'S')
                fprintf (f, "%s ", ptlf4float(buftunitdmax, tlf4_scm2thr(table->SET1D[i]))) ;
            else
                fprintf (f, "%s ", ptlf4float(buftunitdmax, table->SET1D[i])) ;
        }
        if(type == 'S')
            fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(table->SET1D[table->NX - 1]))) ;
        else
            fprintf (f, "%s)\n", ptlf4float(buftunitdmax, table->SET1D[table->NX - 1])) ;
        fprintf (f, "\t\t\t)\n") ;
	}
	
	/* table 2D */

	if (table->SET2D) {
		fprintf (f, "\t\t\t(\n") ;
		for (i = 0 ; i < table->NX - 1; i++) {
			fprintf (f, "\t\t\t\t(") ;
			for (j = 0 ; j < table->NY - 1; j++) {
                if(type == 'S')
                    fprintf (f, "%s ", ptlf4float(buftunitdmax, tlf4_scm2thr(table->SET2D[i][j]))) ;
                else
                    fprintf (f, "%s ", ptlf4float(buftunitdmax, table->SET2D[i][j])) ;
            }
            if(type == 'S')
                fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(table->SET2D[i][table->NY - 1]))) ;
            else
                fprintf (f, "%s)\n", ptlf4float(buftunitdmax, table->SET2D[i][table->NY - 1])) ;
		}
		fprintf (f, "\t\t\t\t(") ;
		for (j = 0 ; j < table->NY - 1; j++){
            if(type == 'S')
                fprintf (f, "%s ", ptlf4float(buftunitdmax, tlf4_scm2thr(table->SET2D[table->NX - 1][j]))) ;
            else
                fprintf (f, "%s ", ptlf4float(buftunitdmax, table->SET2D[table->NX - 1][j])) ;
        }
        if(type == 'S')
            fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(table->SET2D[table->NX - 1][table->NY - 1]))) ;
        else
            fprintf (f, "%s)\n", ptlf4float(buftunitdmax, table->SET2D[table->NX - 1][table->NY - 1])) ;

		fprintf (f, "\t\t\t)\n") ;
	}

	fprintf (f, "\t\t)\n") ;
	fprintf (f, "\t)\n\n") ;
}

/****************************************************************************/
/*     function  tlf4_drivemodels                                           */
/****************************************************************************/
void tlf4_drivemodels (FILE *f, timing_model *modelmin, timing_model *modelmax, char type)
{
	int   i, j ;
    char *name ;

    timing_table *tablemin = modelmin->UMODEL.TABLE;
    timing_table *tablemax = modelmax->UMODEL.TABLE;
    
    name=lib_unsuffix(modelmax->NAME, "_max");

	fprintf (f, "\ttiming_model (%s\n", name) ;
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
		for (i = 0 ; i < tablemax->NX ; i++) 
            if((tablemax->XTYPE==STM_INPUT_SLEW)||(tablemax->XTYPE==STM_CLOCK_SLEW)){
                fprintf(f," %s", ptlf4float(buftunitdmax,tlf4_scm2thr(tablemax->XRANGE[i])));
            }
            else
                fprintf(f," %.2f", tablemax->XRANGE[i]);
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
		for (i = 0 ; i < tablemax->NY ; i++) 
            if((tablemax->YTYPE==STM_INPUT_SLEW)||(tablemax->YTYPE==STM_CLOCK_SLEW)){
                fprintf(f," %s", ptlf4float(buftunitdmax,tlf4_scm2thr(tablemax->YRANGE[i])));
            }
            else
                fprintf(f," %.2f", tablemax->YRANGE[i]);
		fprintf (f, ")\n") ;
	}

	/* const */
	
	if (tablemax->CST != STM_NOVALUE){
        if(tablemin){
            if(type == 'S')
                fprintf(f, "\t\t\t(%s::%s)\n", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->CST)), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->CST))) ;
            else
                fprintf(f, "\t\t\t(%s::%s)\n", ptlf4float(buftunitdmin, tablemin->CST), ptlf4float(buftunitdmax, tablemax->CST)) ;
        }else{
            if(type == 'S')
                fprintf(f, "\t\t\t(%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->CST))) ;
            else
                fprintf(f, "\t\t\t(%s)\n", ptlf4float(buftunitdmax, tablemax->CST)) ;
        }            
    }
            
            
	
	/* table 1D */

	if (tablemax->SET1D) {
		fprintf(f, "\t\t\t(\n") ;
		fprintf(f, "\t\t\t\t(") ;
		for (i = 0 ; i < tablemax->NX - 1 ; i++){
            if(tablemin){
                if(type == 'S')
                    fprintf (f, "%s::%s ", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->SET1D[i])), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET1D[i]))) ;
                else
                    fprintf (f, "%s::%s ", ptlf4float(buftunitdmin, tablemin->SET1D[i]), ptlf4float(buftunitdmax, tablemax->SET1D[i])) ;
            }else{
                if(type == 'S')
                    fprintf (f, "%s ", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET1D[i]))) ;
                else
                    fprintf (f, "%s ", ptlf4float(buftunitdmax, tablemax->SET1D[i])) ;
            }                
        }
        if(tablemin){
            if(type == 'S')
                fprintf (f, "%s::%s)\n", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->SET1D[tablemin->NX - 1])), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET1D[tablemax->NX - 1]))) ;
            else
                fprintf (f, "%s::%s)\n", ptlf4float(buftunitdmin, tablemin->SET1D[tablemin->NX - 1]), ptlf4float(buftunitdmax, tablemax->SET1D[tablemax->NX - 1])) ;
        }else{
            if(type == 'S')
                fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET1D[tablemax->NX - 1]))) ;
            else
                fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tablemax->SET1D[tablemax->NX - 1])) ;
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
                        fprintf (f, "%s::%s ", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->SET2D[i][j])), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[i][j]))) ;
                    else
                        fprintf (f, "%s::%s ", ptlf4float(buftunitdmin, tablemin->SET2D[i][j]), ptlf4float(buftunitdmax, tablemax->SET2D[i][j])) ;
                }else{
                    if(type == 'S')
                        fprintf (f, "%s ", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[i][j]))) ;
                    else
                        fprintf (f, "%s ", ptlf4float(buftunitdmax, tablemax->SET2D[i][j])) ;
                }   
            }
            if(tablemin){
                if(type == 'S')
                    fprintf (f, "%s::%s)\n", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->SET2D[i][tablemin->NY - 1])), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[i][tablemax->NY - 1]))) ;
                else
                    fprintf (f, "%s::%s)\n", ptlf4float(buftunitdmin, tablemin->SET2D[i][tablemin->NY - 1]), ptlf4float(buftunitdmax, tablemax->SET2D[i][tablemax->NY - 1])) ;
            }else{
                if(type == 'S')
                    fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[i][tablemax->NY - 1]))) ;
                else
                    fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tablemax->SET2D[i][tablemax->NY - 1])) ;
            }   
		}
		fprintf (f, "\t\t\t\t(") ;
		for (j = 0 ; j < tablemax->NY - 1; j++) {
            if(tablemin){
                if(type == 'S')
                    fprintf (f, "%s::%s ", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->SET2D[tablemin->NX - 1][j])), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[tablemax->NX - 1][j]))) ;
                else
                    fprintf (f, "%s::%s ", ptlf4float(buftunitdmin, tablemin->SET2D[tablemin->NX - 1][j]), ptlf4float(buftunitdmax, tablemax->SET2D[tablemax->NX - 1][j])) ;
            }else{
                if(type == 'S')
                    fprintf (f, "%s ", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[tablemax->NX - 1][j]))) ;
                else
                    fprintf (f, "%s ", ptlf4float(buftunitdmax, tablemax->SET2D[tablemax->NX - 1][j])) ;
            }                
        }
        if(tablemin){
            if(type == 'S')
                fprintf (f, "%s::%s)\n", ptlf4float(buftunitdmin, tlf4_scm2thr(tablemin->SET2D[tablemin->NX - 1][tablemin->NY - 1])), ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1]))) ;
            else
                fprintf (f, "%s::%s)\n", ptlf4float(buftunitdmin, tablemin->SET2D[tablemin->NX - 1][tablemin->NY - 1]), ptlf4float(buftunitdmax, tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1])) ;
        }else{
            if(type == 'S')
                fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tlf4_scm2thr(tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1]))) ;
            else
                fprintf (f, "%s)\n", ptlf4float(buftunitdmax, tablemax->SET2D[tablemax->NX - 1][tablemax->NY - 1])) ;
        }            

		fprintf (f, "\t\t\t)\n") ;
	}

	fprintf (f, "\t\t)\n") ;
	fprintf (f, "\t)\n\n") ;


}

/****************************************************************************/
/*     function  ptlf4long                                                   */
/****************************************************************************/
char *ptlf4long (char *buf, long time)
{

    if(!strcmp(V_STR_TAB[__TMA_MARGIN].VALUE,"ps")){
        sprintf (buf, "%.1f", (float)time/TTV_UNIT) ;
    }else if(!strcmp(V_STR_TAB[__TMA_MARGIN].VALUE,"ns")){
        sprintf (buf, "%.4f", (float)time/1000.0/TTV_UNIT) ;
    }
    
	return (buf) ;
}
/****************************************************************************/
/*     function  ptlf4float                                                   */
/****************************************************************************/
char *ptlf4float (char *buf, float time)
{

    if(!strcmp(V_STR_TAB[__TMA_MARGIN].VALUE,"ps")){
        sprintf (buf, "%.1f", time/TTV_UNIT) ;
    }else if(!strcmp(V_STR_TAB[__TMA_MARGIN].VALUE,"ns")){
        sprintf (buf, "%.4f", time/1000.0/TTV_UNIT) ;
    }
    
	return (buf) ;
}

