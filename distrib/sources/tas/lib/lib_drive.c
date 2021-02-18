/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : LIB Version 1.00                                            */
/*    Fichier : lib_drive.c                                                 */
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

#include "lib.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
extern ht* tut_tablasharea;
int FLAGBUS = 0;
char buftunit[1024];
static ptype_list *libpowerptype=NULL;

static inline ttvline_list *lib_getnodeinputline(ttvevent_list *ev, int ttx)
{
   if (ttx) return ev->INPATH;
   else return ev->INLINE;
}

/****************************************************************************/
/*     function  lib_drive                                                  */
/****************************************************************************/
void lib_drive (chain_list *figlistx, chain_list *befiglistx, char *name, char *suffix)
{
    FILE        *f ;
    ttvfig_list *ttvfig ;
	 chain_list  *ttv ;
    chain_list  *chain;
    timing_cell *cell;
    int cmpt=0;
    ttvinfo_list *ttvinfo = NULL;
    chain_list *figlist, *befiglist, *tofree ;
   
    lib_drive_remove_null( figlistx, befiglistx, &figlist, &befiglist );
    
    printf("\n\n                       ###################################\n");
    printf("                       #                                 #\n");
    printf("                       #   liberty driver version 1.00   #\n");
    printf("                       #                                 #\n");
    printf("                       ###################################\n\n");
    printf("                       WRITING FILE: %s ...\n\n", name);
    
    if(!(f = mbkfopen (name, NULL, WRITE_TEXT))){
        fprintf (stderr, "LIB ERROR : no file %s\n", name);
        EXIT (EXIT_FAILURE) ;
    }

    ttv_search_mode(1, TTV_MORE_OPTIONS_USE_NODE_NAME_ONLY);
    
    for(chain=figlist;chain;chain=chain->NEXT){
        ttvfig=(ttvfig_list*)chain->DATA;
	    cell = stm_getcell (ttvfig->INFO->FIGNAME) ;
	    stm_modtbl_findtemplates(cell);
    }

    if(figlist){
        if(figlist->DATA)
            ttvinfo = ((ttvfig_list*)figlist->DATA)->INFO;
    }

    if(V_STR_TAB[__TMA_LIBRARY].VALUE)
        lib_driveheader(f, V_STR_TAB[__TMA_LIBRARY].VALUE, ttvinfo);
    else
        lib_driveheader(f, name, ttvinfo);

    if(V_BOOL_TAB[__TUT_CALC_DERATE].VALUE) {
        lib_driveopcond(f, "OP1", LIB_NOM_PROCESS, TUT_MAXTEMP, TUT_MAXVOLT);
        lib_driveopcond(f, "OP2", LIB_NOM_PROCESS, TUT_MAXTEMP, TUT_MINVOLT);
        lib_driveopcond(f, "OP3", LIB_NOM_PROCESS, TUT_MINTEMP, TUT_MAXVOLT);
        lib_driveopcond(f, "OP4", LIB_NOM_PROCESS, TUT_MINTEMP, TUT_MINVOLT);
    }

    saut(f,1);
	
    if(STM_TEMPLATES_CHAIN){
        lib_drivetemplate(f);
    }

     
    saut(f,1);

    lib_drive_power_supply_group(f, figlist, ttvinfo);

    saut(f,1);
	
    lib_drivebustype(f, figlist);

    
    saut(f,1);
    tofree=befiglist;
    for(ttv = figlist ; ttv ; ttv = ttv->NEXT){
        saut(f,1);
        ttvfig=(ttvfig_list*)ttv->DATA;
        if(befiglist){
            lib_drivecell (f, ttvfig, (befig_list*)befiglist->DATA, suffix);
            befiglist=befiglist->NEXT;
        }
        else
            lib_drivecell (f, ttvfig, NULL, suffix);
        cmpt++;
        printf("                       -> writing cell%d: %s\n", cmpt, lib_unsuffix(ttvfig->INFO->FIGNAME, suffix));
    }

    saut(f,1);
    fprintf(f,"}\n");

	fclose (f) ;    
    printf("\n                       ...DONE\n\n");

    ttv_search_mode(0, TTV_MORE_OPTIONS_USE_NODE_NAME_ONLY);

    freechain( figlist );
    freechain( tofree );
}

/****************************************************************************/
/*     function  lib_drivebusorpin                                          */
/****************************************************************************/
void lib_drivebusorpin (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, befig_list *ptcellbefig, cbhseq *ptcbhseq, int *flagbus, int n, char**busname, int nbrail)
{
    int ninter;
    int lsb = 0, msb = 0;
    char *previousname = NULL, *devectname;
    ttvsig_list *bussig;
    
            devectname = ttv_devect(ttvsig->NAME);
            if(*flagbus)
                if(strcmp(*busname,vectorradical(devectname))){
                    *flagbus = 0;
                    space(f,8);
                    fprintf(f, "}\n\n");
                }
            if((ttvsig->NAME != devectname) && (!*flagbus)){
                ninter = n;
                *flagbus = 1;
                lsb = vectorindex(devectname);
                *busname = vectorradical(devectname);
                bussig = ttvsig;
                for(ninter=n; ninter < ttvfig->NBCONSIG; ninter++){
                    ttvsig = ttvfig->CONSIG[ninter];
                    devectname = ttv_devect(ttvsig->NAME);
                    if((vectorradical(devectname) != previousname) && (n != ninter))
                        break;
                    previousname = vectorradical(devectname);
                    msb = vectorindex(devectname);
                }
                space(f,8);
                fprintf(f, "bus (%s) {\n", *busname);
                space(f,12);
                fprintf(f, "bus_type : bus_%d_%d ;\n", msb, lsb);

            }
            ttvsig = ttvfig->CONSIG[n];
            FLAGBUS = *flagbus;
            lib_drivepin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq, nbrail);
            FLAGBUS = 0;
            if(*flagbus)
                if((n + 1)  == ttvfig->NBCONSIG){
                    *flagbus = 0;
                    space(f,8);
                    fprintf(f, "}\n");
                }
}

/****************************************************************************/
/*     function  lib_getpinname                                             */
/****************************************************************************/
char *lib_getpinname(char *str)
{
    char       *signame;
    static char buf[1024];
    
    signame = ttv_devect(str);
    if(vectorindex(signame) == -1){
        return signame;
    }else{
        if(LIB_BUS_DELIM[1] != '\0')
            sprintf(buf, "%s%c%d%c", vectorradical(signame), LIB_BUS_DELIM[0], vectorindex(signame), LIB_BUS_DELIM[1]);
        else
            sprintf(buf, "%s%c%d", vectorradical(signame), LIB_BUS_DELIM[0], vectorindex(signame));
        return buf;
    }
}

/****************************************************************************/
/*     function  lib_drive_generated_clocks                                 */
/****************************************************************************/
void lib_drive_generated_clock (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, ptype_list *ptype)
{
    chain_list *chain=NULL;
    ptype_list *pt;
    int   mbdb;
    float duty_cycle;
    char master[1024];
    
    space(f,8);
    fprintf(f,"generated_clock (%s) {\n", lib_getpinname(ttvsig->NAME));
    sscanf(ptype->DATA, "%d %g%% %s", &mbdb, &duty_cycle, master);
    space(f,12);
    fprintf(f,"clock_pin : %s ;\n", lib_getpinname(ttvsig->NAME));
    if ((pt=getptype(ttvfig->USER, LIB_GENERATED_CLOCK_MASTERS))==NULL){
        chain = addchain(chain, namealloc(master));
        ttvfig->USER=addptype(ttvfig->USER, LIB_GENERATED_CLOCK_MASTERS, chain);
    }else{
        pt->DATA = addchain(pt->DATA, namealloc(master));
    }
    space(f,12);
    fprintf(f,"master_pin : %s ;\n", lib_getpinname(master));
    if(mbdb < 0){
        space(f,12);
        fprintf(f,"multiplied_by : %d ;\n", abs(mbdb));
    }else{
        space(f,12);
        fprintf(f,"divided_by : %d ;\n", mbdb);
    }
    space(f,12);
    fprintf(f,"duty_cycle : %g ;\n", duty_cycle);
    space(f,8);
    fprintf(f,"}\n");
}

/****************************************************************************/
/*     function  lib_drive_generated_clocks                                 */
/****************************************************************************/
void lib_drive_generated_clocks (FILE *f, ttvfig_list *ttvfig)
{
    ptype_list *ptype;
    ttvsig_list *ttvsig ;
    int n;

    for (n=0; n < ttvfig->NBEBREAKSIG; n++){
        ttvsig=ttvfig->EBREAKSIG[n];
        if ((ttvsig->TYPE & TTV_SIG_C)!=TTV_SIG_C){
            ptype = getptype(ttvsig->USER, LIB_GENERATED_CLOCK_INFO);
            if(ptype)
                lib_drive_generated_clock(f, ttvfig, ttvsig, ptype);
        }
    }
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        ptype = getptype(ttvsig->USER, LIB_GENERATED_CLOCK_INFO);
        if(ptype)
            lib_drive_generated_clock(f, ttvfig, ttvsig, ptype);
    }
}

/****************************************************************************/
/*     function  lib_get_pscale                                             */
/****************************************************************************/
float lib_get_pscale()
{
    float pscale;

    if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "1mW"))
        pscale = 1e3;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "100uW"))
        pscale = 1e4;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "10uW"))
        pscale = 1e5;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "1uW"))
        pscale = 1e6;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "100nW"))
        pscale = 1e7;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "10nW"))
        pscale = 1e8;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "1nW"))
        pscale = 1e9;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "100pW"))
        pscale = 1e10;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "10pW"))
        pscale = 1e11;
    else if(!strcmp(V_STR_TAB[__TMA_PUNIT].VALUE, "1pW"))
        pscale = 1e12;
    else
        pscale = 1e6;

    return pscale;
}

/****************************************************************************/
/*     function  lib_drivecell                                              */
/****************************************************************************/
void lib_drivecell (FILE *f, ttvfig_list *ttvfig, befig_list *ptcellbefig, char *suffix)
{
    int n ;
    ttvsig_list *ttvsig ;
    char *name, *busname;
    cbhseq *ptcbhseq=NULL;
    int flagbus = 0, nbrail;
    ptype_list  *ptype;
    tma_leak_pow_list *tlp;
    float pscale = lib_get_pscale();

    name=lib_unsuffix (ttvfig->INFO->FIGNAME, suffix);
    tab(f);
    fprintf(f,"cell (%s) {\n", name);
    if(tut_tablasharea){
        tab(f);
        tab(f);
        if(gethtitem(tut_tablasharea,name) != EMPTYHT)
            fprintf(f,"area : %.2f ;\n",(double)(gethtitem(tut_tablasharea,name))/1000.0);
        else
            fprintf(f,"area : 0.0 ;\n");
    }else{
        tab(f);
        tab(f);
        fprintf(f,"area : 0.0 ;\n");
    }

    if((ptype = getptype(ttvfig->USER, TTV_CELL_LEAKAGE_POWER)) != NULL){
        tab(f);
        tab(f);
        fprintf(f,"cell_leakage_power : %.2g ;\n", *(float*)&ptype->DATA * pscale);
    }
    
    if((ptype = getptype(ttvfig->USER, TTV_LEAKAGE_POWER)) != NULL){
        for(tlp = (tma_leak_pow_list*)ptype->DATA; tlp; tlp=tlp->NEXT){
            space(f,8);
            fprintf(f,"leakage_power () {\n");
            space(f,12);
            fprintf(f,"when : \"");
            cbh_writeabl(f,tlp->PATTERN, 'L');
            fprintf(f,"\" ;\n");
            space(f,12);
            fprintf(f,"value : %.2g ;\n", tlp->VALUE * pscale );
            space(f,8);
            fprintf(f,"}\n");
            
        }
    }

    if(ptcellbefig){
        if((ptype = getptype(ttvfig->USER, TTV_FIG_CBHSEQ)) != NULL){
            ptcbhseq = (cbhseq*)ptype->DATA;
            lib_driveseqfunc(f,ptcbhseq);
        }
    }
    nbrail = lib_drive_rail_connection(f, ttvfig);
    lib_drive_generated_clocks(f, ttvfig);
    flagbus = 0;
    for (n=0; n < ttvfig->NBEBREAKSIG; n++){
        ttvsig=ttvfig->EBREAKSIG[n];
        if ((ttvsig->TYPE & TTV_SIG_C)!=TTV_SIG_C)
            lib_drivepin(f, ttvfig, ttvsig, NULL, NULL, nbrail);
    }
    if(flagbus){
        flagbus = 0;
        space(f,8);
        fprintf(f, "}\n\n");
    }
    flagbus = 0;
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if (((ttvsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI) 
         && ((ttvsig->TYPE & TTV_SIG_CB)!=TTV_SIG_CB)
         && ((ttvsig->TYPE & TTV_SIG_CT)!=TTV_SIG_CT))
            lib_drivebusorpin(f, ttvfig, ttvsig, NULL, NULL, &flagbus, n, &busname, nbrail);
    }
    if(flagbus){
        flagbus = 0;
        space(f,8);
        fprintf(f, "}\n\n");
    }
    flagbus = 0;
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if ((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO)
            lib_drivebusorpin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq, &flagbus, n, &busname, nbrail);
    }
    if(flagbus){
        flagbus = 0;
        space(f,8);
        fprintf(f, "}\n\n");
    }
    flagbus = 0;
    for (n=0; n < ttvfig->NBCONSIG; n++){
        ttvsig=ttvfig->CONSIG[n];
        if ((ttvsig->TYPE & TTV_SIG_CX)==TTV_SIG_CX)
            lib_drivebusorpin(f, ttvfig, ttvsig, ptcellbefig, ptcbhseq, &flagbus, n, &busname, nbrail);
    }
    if(flagbus){
        flagbus = 0;
        space(f,8);
        fprintf(f, "}\n\n");
    }
    tab(f);
    fprintf(f,"}\n");
    if (ptcbhseq) cbh_delseq(ptcbhseq);
        
}
/****************************************************************************/
/*     function  lib_driveseqfunc                                           */
/****************************************************************************/
void lib_driveseqfunc (FILE *f, cbhseq *ptcbhseq)
{
    if(ptcbhseq){
        if(ptcbhseq->SEQTYPE==CBH_LATCH){
            space(f, 8);
            fprintf(f,"latch(");
            if(ptcbhseq->NAME){
                fprintf(f,"%s",ptcbhseq->NAME);
                if(ptcbhseq->NEGNAME)
                    fprintf(f,",%s",ptcbhseq->NEGNAME);
                fprintf(f,") {\n");
            }else if(ptcbhseq->NEGNAME){
                fprintf(f,"%s) {\n",ptcbhseq->NEGNAME);
            }
            if(ptcbhseq->CLOCK){
                space(f, 12);
                fprintf(f,"enable : \"");
                cbh_writeabl(f,ptcbhseq->CLOCK, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->DATA){
                space(f, 12);
                fprintf(f,"data_in : \"");
                cbh_writeabl(f,ptcbhseq->DATA, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->RESET){
                space(f, 12);
                fprintf(f,"clear : \"");
                cbh_writeabl(f,ptcbhseq->RESET, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->SET){
                space(f, 12);
                fprintf(f,"preset : \"");
                cbh_writeabl(f,ptcbhseq->SET, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->RSCONF){
                space(f, 12);
                fprintf(f,"clear_preset_var1 : ");
                cbh_writeabl(f,ptcbhseq->RSCONF, 'L');
                fprintf(f," ;\n");
            }
            if(ptcbhseq->RSCONFNEG){
                space(f, 12);
                fprintf(f,"clear_preset_var2 : ");
                cbh_writeabl(f,ptcbhseq->RSCONFNEG, 'L');
                fprintf(f," ;\n");
            }
            saut(f,1);
            space(f, 8);
            fprintf(f,"}\n");
        }else if(ptcbhseq->SEQTYPE==CBH_FLIPFLOP){
            space(f, 8);
            fprintf(f,"ff(");
            if(ptcbhseq->NAME){
                fprintf(f,"%s",ptcbhseq->NAME);
                if(ptcbhseq->NEGNAME)
                    fprintf(f,",%s",ptcbhseq->NEGNAME);
                fprintf(f,") {\n");
            }else if(ptcbhseq->NEGNAME){
                fprintf(f,"%s) {\n",ptcbhseq->NEGNAME);
            }
            if(ptcbhseq->CLOCK){
                space(f, 12);
                fprintf(f,"clocked_on : \"");
                cbh_writeabl(f,ptcbhseq->CLOCK, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->SLAVECLOCK){
                space(f, 12);
                fprintf(f,"clocked_on_also : \"");
                cbh_writeabl(f,ptcbhseq->SLAVECLOCK, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->DATA){
                space(f, 12);
                fprintf(f,"next_state : \"");
                cbh_writeabl(f,ptcbhseq->DATA, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->RESET){
                space(f, 12);
                fprintf(f,"clear : \"");
                cbh_writeabl(f,ptcbhseq->RESET, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->SET){
                space(f, 12);
                fprintf(f,"preset : \"");
                cbh_writeabl(f,ptcbhseq->SET, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhseq->RSCONF){
                space(f, 12);
                fprintf(f,"clear_preset_var1 : ");
                cbh_writeabl(f,ptcbhseq->RSCONF, 'L');
                fprintf(f," ;\n");
            }
            if(ptcbhseq->RSCONFNEG){
                space(f, 12);
                fprintf(f,"clear_preset_var2 : ");
                cbh_writeabl(f,ptcbhseq->RSCONFNEG, 'L');
                fprintf(f," ;\n");
            }
            saut(f,1);
            space(f, 8);
            fprintf(f,"}\n");
        }
    }
}

/****************************************************************************/
/*     function  lib_get_tg_pinname                                             */
/****************************************************************************/
char *lib_get_tg_pinname(char *str)
{
    char       *signame;
    static char buf[1024];
    
    signame = ttv_devect(str);
    if(vectorindex(signame) == -1){
        return signame;
    }else{
        if(LIB_BUS_DELIM[1] != '\0')
            sprintf(buf, "%s_%d_", vectorradical(signame), vectorindex(signame));
        else
            sprintf(buf, "%s_%d", vectorradical(signame), vectorindex(signame));
        return buf;
    }
}
/****************************************************************************/
/*     function  lib_clock_true                                             */
/****************************************************************************/
void lib_drive_clock_true(FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig)
{
    ptype_list *pt;
    chain_list *chain;
    if ((pt=getptype(ttvfig->USER, LIB_GENERATED_CLOCK_MASTERS))!=NULL){
        for(chain=pt->DATA; chain; chain=chain->NEXT){
            if((char*)chain->DATA == ttvsig->NAME){
                space(f,12);
                fprintf(f,"clock : true ;\n");
                break;
            }
        }
    }
}


/****************************************************************************/
/*     function  lib_drivepin                                               */
/****************************************************************************/
void lib_drivepin (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, befig_list *ptcellbefig, cbhseq *ptcbhseq, int nbrail)
{
    chain_list *chtsname=NULL;
    bepor_list *ptbepor=NULL;
    char       *signame;
    char       *pinname;

    signame = ttv_devect(ttvsig->NAME);
    if(ptcellbefig){
        for(ptbepor=ptcellbefig->BEPOR; ptbepor; ptbepor=ptbepor->NEXT){
            if(ptbepor->NAME==signame) break;
        }
    }
    
    space(f,8);
    pinname = lib_getpinname(ttvsig->NAME);
    if(isdigit(pinname[0]))
        fprintf(f,"pin (\"%s\") {\n", pinname);
    else
        fprintf(f,"pin (%s) {\n", pinname);
    chtsname=lib_drivecombfunc(f,signame,ptcellbefig,ptcbhseq);
    lib_drivedirection(f, ttvsig, ptbepor, chtsname, nbrail);
    lib_drive_clock_true(f, ttvfig, ttvsig);
    if((ttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C) 
        lib_drivecapacitance(f, ttvsig);
    if(!getptype (ttvsig->USER, TTV_SIG_ASYNCHRON)){
        lib_drivetiming(f, ttvfig, ttvsig, chtsname,ptcbhseq, ptcellbefig);
    }
    if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 )
     ||( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 5 ))
        lib_drivepower(f, ttvfig, ttvsig, chtsname, ptcellbefig);
    freechain(chtsname);
    space(f,8);
    fprintf(f,"}\n");
    
}

/****************************************************************************/
/*     function  lib_drivecombfunc                                          */
/****************************************************************************/
chain_list* lib_drivecombfunc (FILE *f, char *name, befig_list *ptcellbefig, cbhseq *ptcbhseq)
{
    cbhcomb    *ptcbhcomb;
    chain_list *chtsname=NULL;

    if(ptcellbefig){
        ptcbhcomb = cbh_getcombfunc(ptcellbefig, ptcbhseq, name);
        if(ptcbhcomb){
            if(ptcbhcomb->FUNCTION){
                space(f,12);
                fprintf(f,"function : \"");
                cbh_writeabl(f, ptcbhcomb->FUNCTION, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhcomb->HZFUNC){
                chtsname=supportChain_listExpr(ptcbhcomb->HZFUNC);

                space(f,12);
                fprintf(f,"three_state : \"");
                cbh_writeabl(f, ptcbhcomb->HZFUNC, 'L');
                fprintf(f,"\" ;\n");
            }
            if(ptcbhcomb->CONFLICT){
                space(f,12);
                fprintf(f,"x_function : \"");
                cbh_writeabl(f, ptcbhcomb->CONFLICT, 'L');
                fprintf(f,"\" ;\n");
            }
            cbh_delcomb(ptcbhcomb);
        }
   }
   return chtsname;
}
        
/****************************************************************************/
/*     function  lib_drivecapacitance                                       */
/****************************************************************************/
void lib_drivecapacitance (FILE *f, ttvsig_list *ttvsig)
{
    float cumin,cumax,cdmin,cdmax;
    double capaupmin,capaupmax,capadnmin,capadnmax;
    int   flag = 0;
    float max_capa;
    ptype_list *ptype;

    if(((ttvsig->TYPE & TTV_SIG_CI) == TTV_SIG_CI) || (V_BOOL_TAB[__TMA_DRIVECAPAOUT].VALUE == 1)){
        if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 ){
           if(ttv_getsigcapas ( ttvsig,
                             NULL, &cumin, &cumax,
                             NULL, &cdmin, &cdmax)){
               capaupmin = cumin*1000.0;
               capaupmax = cumax*1000.0;
               capadnmin = cdmin*1000.0;
               capadnmax = cdmax*1000.0;
               if( 0.99*(double)ttvsig->CAPA < ((double)capaupmin))
                   capaupmin =  0.99*(double)ttvsig->CAPA;
               if( 0.99*(double)ttvsig->CAPA < ((double)capadnmin))
                   capadnmin =  0.99*(double)ttvsig->CAPA;
               if( 1.01*(double)ttvsig->CAPA > ((double)capaupmax))
                   capaupmax =  1.01*(double)ttvsig->CAPA;
               if( 1.01*(double)ttvsig->CAPA > ((double)capadnmax))
                   capadnmax =  1.01*(double)ttvsig->CAPA;
               flag = 1;
           }else{
               flag = 0;
           }
        }
        if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"ff")){
            space(f,12);
            fprintf(f,"capacitance : %.2f ;\n",(double)ttvsig->CAPA);
            if (( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 ) && flag) {
                space(f,12);
                fprintf(f,"rise_capacitance_range ( %.2f , %.2f ) ;\n", 
                          capaupmin,capaupmax);
                space(f,12);
                fprintf(f,"fall_capacitance_range ( %.2f , %.2f ) ;\n",
                          capadnmin,capadnmax);
            }
        }else if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"pf")){
            space(f,12);
            fprintf(f,"capacitance : %.5f ;\n",(double)ttvsig->CAPA / 1000.0);
            if (( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 ) && flag) {
              space(f,12);
              fprintf(f,"rise_capacitance_range ( %.5f , %.5f ) ;\n",(double)capaupmin/1000.0,(double)capaupmax/1000.0);
              space(f,12);
              fprintf(f,"fall_capacitance_range ( %.5f , %.5f ) ;\n",(double)capadnmin/1000.0,(double)capadnmax/1000.0);
            }
        }
    }
    if((ttvsig->TYPE & TTV_SIG_CO) == TTV_SIG_CO){
        ptype = getptype(ttvsig->USER, LIB_MAX_CAPACITANCE);
        if(ptype){
            max_capa = *(float*)&ptype->DATA;
            if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"ff")){
                space(f,12);
                fprintf(f,"max_capacitance : %.2f ;\n", max_capa * 1e15);
            }else if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"pf")){
                space(f,12);
                fprintf(f,"max_capacitance : %.5f ;\n", max_capa * 1e12);
            }
        }
    }

}

/****************************************************************************/
/*     function  lib_drivedirection                                         */
/****************************************************************************/
void lib_drivedirection (FILE *f, ttvsig_list *ttvsig, bepor_list *ptbepor, chain_list *chtsname, int nbrail)
{
    char *dir, *level=NULL ;
    float low, high, alim, nom_voltage;
    ptype_list *ptype;
    
    if ((ttvsig->TYPE & TTV_SIG_CX)==TTV_SIG_CX){
        if(ptbepor){
            switch(ptbepor->DIRECTION){
                case 'I' :  dir="input";  break;
                case 'O' :  dir="output";  break;
                case 'Z' :  dir="output";  break;
                case 'T' :  dir="inout";  break;
                case 'B' :  dir="output"; break;
                case 'X' :  dir="input"; break;
                default  :  dir="input";
            }
        }else{
            dir="input";
        }
    }
    else if ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)
        dir="inout";
    else if ((ttvsig->TYPE & TTV_SIG_CB)==TTV_SIG_CB)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ)
        dir="output";
    else if ((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO)
        dir="output";
    else
        dir="input";

    if ((ttvsig->TYPE & TTV_SIG_C)!=TTV_SIG_C) 
        dir="internal";

    space(f,12);
    fprintf(f,"direction : %s ;\n", dir);

    if(libpowerptype && (nbrail > 1)){
        nom_voltage = ttvsig->ROOT->INFO->VDD;
        for(ptype = libpowerptype; ptype && !level; ptype = ptype->NEXT){
            if (!ttv_get_signal_swing(ttvsig->ROOT, ttvsig, &low, &high)){
                alim = high - low;
                if(!mbk_cmpdouble(*(float*)&ptype->DATA, alim, 1e6)){
                    level = (char*)ptype->TYPE;
                }
            }else if(high > 0.0){
                alim = high - low;
                if(!mbk_cmpdouble(nom_voltage, alim, 1e6)){
                    level = namealloc("VDD0");
                }
            }
        }
        if(!strcmp(dir, "inout")){
            space(f,12);
            fprintf(f, "input_signal_level : %s ;\n", level);
            space(f,12);
            fprintf(f, "output_signal_level : %s ;\n", level);
        }else if((!strcmp(dir, "input"))/*||(!strcmp(dir, "internal"))*/){
            space(f,12);
            fprintf(f, "input_signal_level : %s ;\n", level);
        }else{
            space(f,12);
            fprintf(f, "output_signal_level : %s ;\n", level);
        }
    }
}

/****************************************************************************/
/*     function  lib_drive_pos                                              */
/****************************************************************************/
void lib_drive_pos(FILE *f, ttvfig_list *ttvfig, chain_list *chain_posname, chain_list *chain_pos, char type, chain_list *chain_nonunate, befig_list *ptcellbefig)
{
    int nline = 0;
    char typel;
    chain_list *ch, *chx, *abl ;
    ttvline_list *line;
    char buf[1024];
    char bufsig[1024];
    char bufttype[1024];
    char *relpinname;
    char *pinname;

    ttv_getsigname(ttvfig, bufsig, ((ttvline_list*)chain_pos->DATA)->ROOT->ROOT);
    
    for(chx=chain_posname; chx; chx=chx->NEXT){
        pinname = lib_get_tg_pinname(bufsig);
        if(type == 'M')
            sprintf(buf, "maxd_%s_", pinname);
        else
            sprintf(buf, "mind_%s_", pinname);
        relpinname = lib_get_tg_pinname((char*)chx->DATA);
        strcat(buf, relpinname);
                    
        nline=0;
        for(ch=chain_pos; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;

	       	if(!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA)){
                typel = line->ROOT->TYPE;
                nline++;
            }
        }
        if(nline==1){
            if((typel & TTV_NODE_UP) == TTV_NODE_UP)
                sprintf(bufttype,"preset");
            else
                sprintf(bufttype,"clear");
            strcat(buf, "_");
            strcat(buf, bufttype);
        }
        strcat(buf, "_positive_unate");
        space(f,12);
        fprintf(f,"timing (%s) {\n", buf);
        relpinname = lib_getpinname((char*)chx->DATA);
        space(f,16);
        fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
        if(chain_nonunate && ptcellbefig){
            abl = NULL;
            for(ch=chain_nonunate; ch; ch=ch->NEXT){
                if(!strcmp(relpinname,(char*)ch->DATA)){
                    abl = cbh_calccondition(ptcellbefig, pinname, relpinname, CBH_NONINVERT);
                    break;
                }
            }
            if(abl){
                space(f,16);
                fprintf(f,"when : \"");
                cbh_writeabl(f, abl, 'L');
                fprintf(f,"\" ;\n");
                space(f,16);
                fprintf(f,"sdf_cond : \"");
                cbh_writeabl(f, abl, 'L');
                fprintf(f,"\" ;\n");
            }
        }
        space(f,16);
        fprintf(f,"timing_sense : positive_unate ;\n");
        if(nline==1){
            space(f,16);
            fprintf(f,"timing_type : %s ;\n", bufttype);
        }
        for(ch=chain_pos; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if(!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA)){
                if(type == 'M')
    		        lib_drivedelaymax(f, ttvfig, line);
                else
    		        lib_drivedelaymin(f, ttvfig, line);
            }
        }
        space(f,12);
        fprintf(f,"}\n");
    }
}

/****************************************************************************/
/*     function  lib_drive_neg                                              */
/****************************************************************************/
void lib_drive_neg(FILE *f, ttvfig_list *ttvfig, chain_list *chain_negname, chain_list *chain_neg, char type, chain_list *chain_nonunate, befig_list *ptcellbefig)
{
    int nline = 0;
    char typel;
    chain_list *ch, *chx, *abl ;
    ttvline_list *line;
    char buf[1024];
    char bufsig[1024];
    char bufttype[1024];
    char *relpinname;
    char *pinname;

    ttv_getsigname(ttvfig, bufsig, ((ttvline_list*)chain_neg->DATA)->ROOT->ROOT);
    
    for(chx=chain_negname; chx; chx=chx->NEXT){
        pinname = lib_get_tg_pinname(bufsig);
        if(type == 'M')
            sprintf(buf, "maxd_%s_", pinname);
        else
            sprintf(buf, "mind_%s_", pinname);
        relpinname = lib_get_tg_pinname((char*)chx->DATA);
        strcat(buf, relpinname);
                    
        nline=0;
        for(ch=chain_neg; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if(!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA)){
                typel = line->ROOT->TYPE;
                nline++;
            }
        }
        if(nline==1){
            if((typel & TTV_NODE_UP) == TTV_NODE_UP)
                sprintf(bufttype,"preset");
            else
                sprintf(bufttype,"clear");
            strcat(buf, "_");
            strcat(buf, bufttype);
        }
        strcat(buf, "_negative_unate");
        space(f,12);
        fprintf(f,"timing (%s) {\n", buf);
        relpinname = lib_getpinname((char*)chx->DATA);
        space(f,16);
        fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
        if(chain_nonunate && ptcellbefig){
            abl = NULL;
            for(ch=chain_nonunate; ch; ch=ch->NEXT){
                if(!strcmp(relpinname,(char*)ch->DATA)){
                    abl = cbh_calccondition(ptcellbefig, pinname, relpinname, CBH_INVERT);
                    break;
                }
            }
            if(abl){
                space(f,16);
                fprintf(f,"when : \"");
                cbh_writeabl(f, abl, 'L');
                fprintf(f,"\" ;\n");
                space(f,16);
                fprintf(f,"sdf_cond : \"");
                cbh_writeabl(f, abl, 'L');
                fprintf(f,"\" ;\n");
            }
        }
        space(f,16);
        fprintf(f,"timing_sense : negative_unate ;\n");
        if(nline==1){
            space(f,16);
            fprintf(f,"timing_type : %s ;\n", bufttype);
        }
        for(ch=chain_neg; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if(!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA)){
                if(type == 'M')
        		    lib_drivedelaymax(f, ttvfig, line);
                else
    		        lib_drivedelaymin(f, ttvfig, line);
            }
        }
        space(f,12);
        fprintf(f,"}\n");
    }
}

/****************************************************************************/
/*     function  lib_drive_tri                                              */
/****************************************************************************/
void lib_drive_tri(FILE *f, ttvfig_list *ttvfig, chain_list *chain_triname, chain_list *chain_tri, char type)
{
    int nline = 0, tristate = 0, ttype = 0, tsense = 0;
    char typel;
    chain_list *ch, *chx ;
    ttvline_list *line;
    char buf[1024];
    char bufsig[1024];
    char bufttype[1024];
    char buftsense[1024];
    char *relpinname;
    char *pinname;

    ttv_getsigname(ttvfig, bufsig, ((ttvline_list*)chain_tri->DATA)->ROOT->ROOT);

    for(chx=chain_triname; chx; chx=chx->NEXT){
        pinname = lib_get_tg_pinname(bufsig);
        if(type == 'M')
            sprintf(buf, "maxd_%s_", pinname);
        else
            sprintf(buf, "mind_%s_", pinname);
        relpinname = lib_get_tg_pinname((char*)chx->DATA);
        strcat(buf, relpinname);

        nline=0;
        tristate=0;
        ttype=0;
        tsense=0;
        for(ch=chain_tri; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if((!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA))
              &&((line->TYPE & TTV_LINE_HZ)!=TTV_LINE_HZ)){
                if(((line->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && (nline == 0)){
                    sprintf(buftsense,"positive_unate");
                    tsense=1;
                }else if(nline == 0){
                    sprintf(buftsense, "negative_unate");
                    tsense=1;
                }
                typel = line->ROOT->TYPE;
                nline++;
            }else if((!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA))
              &&((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)){
                tristate = 1;
            }
        }
        if(nline==1 && tristate){
            if((typel & TTV_NODE_UP) == TTV_NODE_UP)
                sprintf(bufttype,"three_state_enable_rise");
            else
                sprintf(bufttype,"three_state_enable_fall");
            ttype=1;
        }else if(tristate){
            sprintf(bufttype,"three_state_enable");
            ttype=1;
        }

        if(ttype){
            strcat(buf, "_");
            strcat(buf, bufttype);
        }
        if(tsense){
            strcat(buf, "_");
            strcat(buf, buftsense);
        }

        if(tsense || ttype){
            space(f,12);
            fprintf(f,"timing (%s) {\n", buf);
            relpinname = lib_getpinname((char*)chx->DATA);
            space(f,16);
            fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
        }

        if(tsense){
            space(f,16);
            fprintf(f,"timing_sense : %s ;\n", buftsense);
        }
        if(ttype){
            space(f,16);
            fprintf(f,"timing_type : %s ;\n", bufttype);
        }
        
        for(ch=chain_tri; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if((!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA))
              &&((line->TYPE & TTV_LINE_HZ)!=TTV_LINE_HZ)){
                if(type == 'M')
        		    lib_drivedelaymax(f, ttvfig, line);
                else
    		        lib_drivedelaymin(f, ttvfig, line);
            }
        }
        if(tsense || ttype){
            space(f,12);
            fprintf(f,"}\n");
        }
    }
    for(chx=chain_triname; chx; chx=chx->NEXT){
        pinname = lib_get_tg_pinname(bufsig);
        if(type == 'M')
            sprintf(buf, "maxd_%s_", pinname);
        else
            sprintf(buf, "mind_%s_", pinname);
        relpinname = lib_get_tg_pinname((char*)chx->DATA);
        strcat(buf, relpinname);
        
        nline=0;
        ttype=0;
        tsense=0;
        for(ch=chain_tri; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if((!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA))
              &&((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)){
                if(((line->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) && (nline == 0)){
                    sprintf(buftsense,"positive_unate");
                    tsense=1;
                }else if(nline == 0){
                    sprintf(buftsense,"negative_unate");
                    tsense=1;
                }
                typel = line->ROOT->TYPE;
                nline++;
            }
        }
        if(nline==1){
            if((typel & TTV_NODE_UP) == TTV_NODE_UP){
                sprintf(bufttype,"three_state_disable_fall");
            }else{
                sprintf(bufttype,"three_state_disable_rise");
            }
            ttype=1;
        }else if(nline>1){
            sprintf(bufttype,"three_state_disable");
            ttype=1;
        }
        if(ttype){
            strcat(buf, "_");
            strcat(buf, bufttype);
        }
        if(tsense){
            strcat(buf, "_");
            strcat(buf, buftsense);
        }
                    
        if(tsense || ttype){
            space(f,12);
            fprintf(f,"timing (%s) {\n", buf);
            relpinname = lib_getpinname((char*)chx->DATA);
            space(f,16);
            fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
        }
        
        if(tsense){
            space(f,16);
            fprintf(f,"timing_sense : %s ;\n", buftsense);
        }
        if(ttype){
            space(f,16);
            fprintf(f,"timing_type : %s ;\n", bufttype);
        }
        
        for(ch=chain_tri; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
	       	if((!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA))
              &&((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ)){
                if(type == 'M')
        		    lib_drivedelaymax(f, ttvfig, line);
                else
    		        lib_drivedelaymin(f, ttvfig, line);
            }
        }
        if(tsense || ttype){
            space(f,12);
            fprintf(f,"}\n");
        }
    }
}

/****************************************************************************/
/*     function  lib_drivepower_with_sdfcond                                */
/****************************************************************************/
void lib_drivepower_with_sdfcond (FILE *f, ttvfig_list *ttvfig, befig_list *ptcellbefig, chain_list *chain_name, chain_list *chain, int type, chain_list *chain_nonunate)
{
    ttvline_list *line;
    timing_model *tmodel;
    chain_list   *chx, *ch;
    char buf[1024];
    char bufsig[1024];
    char *relpinname;
    char *pinname;
    int driveok, rise, fall;
    chain_list *abl;
    
    for(ch=chain; ch; ch=ch->NEXT){
        if(ch->DATA){
            ttv_getsigname(ttvfig, bufsig, ((ttvline_list*)ch->DATA)->ROOT->ROOT);
            break;
        }
    }

    for(chx=chain_name; chx; chx=chx->NEXT){
        driveok = 0;
        for(ch=chain; ch; ch=ch->NEXT){
            line=(ttvline_list*)ch->DATA;
            if(!line) continue;
            if(!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA)){
                if(line->MDMAX){
                    tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                    if(tmodel->ENERGYTYPE == STM_ENERGY_TABLE){
                        driveok = 1;
                    }
                }
            }
        }
        if(chain_nonunate && !ptcellbefig && (type == CBH_INVERT)){
            for(ch=chain_nonunate; ch; ch=ch->NEXT){
                if(!strcmp(relpinname,(char*)ch->DATA)){
                    driveok = 0;
                    break;
                }
            }
        }
        if(driveok){
            abl = NULL;
            pinname = lib_get_tg_pinname(bufsig);
            if(type == CBH_NONINVERT){
                sprintf(buf, "energy_pos_%s_", pinname);
            }else if(type == CBH_INVERT){
                sprintf(buf, "energy_neg_%s_", pinname);
            }else{
                sprintf(buf, "energy_nun_%s_", pinname);
            }
            relpinname = lib_get_tg_pinname((char*)chx->DATA);
            strcat(buf, relpinname);
            relpinname = lib_getpinname((char*)chx->DATA);
            if(chain_nonunate && ptcellbefig){
                for(ch=chain_nonunate; ch; ch=ch->NEXT){
                    if(!strcmp(relpinname,(char*)ch->DATA)){
                        abl = cbh_calccondition(ptcellbefig, pinname, relpinname, type);
                        break;
                    }
                }
            }
            space(f,12);
            fprintf(f,"internal_power (%s) {\n", buf);
            space(f,16);
            fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
            if(ptcellbefig && abl){
                space(f,16);
                fprintf(f,"when : \"");
                cbh_writeabl(f, abl, 'L');
                fprintf(f,"\" ;\n");
            }
            rise = 0;
            fall = 0;
            for(ch=chain; ch; ch=ch->NEXT){
                line=(ttvline_list*)ch->DATA;
                if(!line) continue;
            	if(!strcmp(line->NODE->ROOT->NAME,(char*)chx->DATA)){
                    if(line->MDMAX && line->MFMAX){
                        tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, line->MDMAX);
                        if(tmodel->ENERGYTYPE == STM_ENERGY_TABLE){
                            if((line->ROOT->TYPE & TTV_NODE_UP)==TTV_NODE_UP && !rise){
                                rise = 1;
                                space(f,16);
                                if(tmodel->ENERGYMODEL.ETABLE->CST != STM_NOVALUE){
                                    fprintf(f,"rise_power (scalar) {\n");
                                    lib_drivelutconst(f, tmodel->ENERGYMODEL.ETABLE->CST, 'E');
                                }else{
                                    fprintf(f,"rise_power (energy_%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
                                    lib_drivelut (f, tmodel, 'E');
                                }
                                space(f,16);
                                fprintf(f,"}\n");
                            }else if(!fall){
                                fall = 1;
                                space(f,16);
                                if(tmodel->ENERGYMODEL.ETABLE->CST != STM_NOVALUE){
                                    fprintf(f,"fall_power (scalar) {\n");
                                    lib_drivelutconst(f, tmodel->ENERGYMODEL.ETABLE->CST, 'E');
                                }else{
                                    fprintf(f,"fall_power (energy_%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
                                    lib_drivelut (f, tmodel, 'E');
                                }
                                space(f,16);
                                fprintf(f,"}\n");
                            }
                        }
                    }
                }
            }
            space(f,12);
            fprintf(f,"}\n");
        }
    }
}
/****************************************************************************/
/*     function  lib_drivepower                                             */
/****************************************************************************/
void lib_drivepower (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, chain_list *chtsname, befig_list *ptcellbefig)
{
    ttvevent_list *outevent, *outevent2, *event_rise, *event_fall;
    ttvline_list *line, *line2, *line3;
    timing_model *tmodel, *tmodel2, *tmodel3;
    chain_list   *chain_pos, *chain_neg, *chain_tri, *chain_posname, *chain_negname, *chain_triname, *chain_nonunate;
    int ev, ev2;
    int ttx;
    char buf[1024];
    char bufsig[1024];
    char *pinname;
    ptype_list *ptyperise, *ptypefall;
    char flagtri;
    chain_list *ch, *ch2, *ch3, *abl;
    
    
    if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_TTX)==TTV_FILE_TTX)
      ttx=1;
    else if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)
      ttx=0;
    else
      ttx=1;
    
    chain_pos = NULL;
    chain_neg = NULL;
    chain_tri = NULL;
    chain_posname = NULL;
    chain_negname = NULL;
    chain_triname = NULL;
    chain_nonunate = NULL;
    if((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO) {
        for(ev=0;ev < 2; ev++){
            outevent=&ttvsig->NODE[ev];
            for (line = lib_getnodeinputline(outevent, ttx) ; line ; line = line->NEXT){
                flagtri = 0;
                if(chtsname){
                    for(ch=chtsname; ch; ch=ch->NEXT){
                        if(!strcmp(line->NODE->ROOT->NAME,(char*)ch->DATA)){
                            flagtri = 1;
                            break;
                        }
                    }
                }else{
                    if(((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ) || ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)){
                        for(ev2=0;ev2 < 2; ev2++){
                            outevent2=&ttvsig->NODE[ev2];
                            for (line2 = lib_getnodeinputline(outevent2, ttx) ; line2 ; line2 = line2->NEXT){
    	                        if(((line2->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) && 
                                    (line->NODE->ROOT==line2->NODE->ROOT) &&
                                    (line->ROOT->ROOT==line2->ROOT->ROOT)){
                                    flagtri = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
               	if(((line->TYPE & TTV_LINE_A)==TTV_LINE_A) ||
                    (getptype (line->NODE->ROOT->USER, TTV_SIG_CLOCK) && !getptype (line->ROOT->ROOT->USER, TTV_SIG_CLOCK) && V_BOOL_TAB[__LIB_DRIVE_COMB_AS_ACCESS].VALUE)){
                    flagtri = 1;
                }
 
                if(flagtri){
                    chain_tri=addchain(chain_tri, line);
                    chain_triname=addname(chain_triname, line->NODE->ROOT->NAME);
                }else{
                    if((line->NODE->TYPE ) == (line->ROOT->TYPE )){
                        if(ptcellbefig){
                            abl = cbh_calccondition(ptcellbefig, line->ROOT->ROOT->NAME, line->NODE->ROOT->NAME, CBH_NONINVERT);
                            if(!abl || (abl && (strcmp (VALUE_ATOM (abl), "'0'")))) {
                                chain_pos=addchain(chain_pos, line);
                                chain_posname=addname(chain_posname, line->NODE->ROOT->NAME);
                            }
                        }else{
                            chain_pos=addchain(chain_pos, line);
                            chain_posname=addname(chain_posname, line->NODE->ROOT->NAME);
                        }
                    }else{
                        if(ptcellbefig){
                            abl = cbh_calccondition(ptcellbefig, line->ROOT->ROOT->NAME, line->NODE->ROOT->NAME, CBH_INVERT);
                            if(!abl || (abl && (strcmp (VALUE_ATOM (abl), "'0'")))) {
                                chain_neg=addchain(chain_neg, line);
                                chain_negname=addname(chain_negname, line->NODE->ROOT->NAME);
                            }
                        }else{
                            chain_neg=addchain(chain_neg, line);
                            chain_negname=addname(chain_negname, line->NODE->ROOT->NAME);
                        }
                    }
                }
            }
        }
        for(ch=chain_posname; ch; ch=ch->NEXT){
           for(ch2=chain_negname; ch2; ch2=ch2->NEXT){
               if(!strcmp((char*)ch->DATA,(char*)ch2->DATA)){
                   chain_nonunate=addname(chain_nonunate, ch->DATA);
               }
           }
        }
/*        if(!ptcellbefig){
            for(ch=chain_nonunate; ch; ch=ch->NEXT){
                for(ch2=chain_pos; ch2; ch2=ch2->NEXT){
                    line2=(ttvline_list*)ch2->DATA;
                    if(!line2) continue;
                    if(!strcmp(line2->NODE->ROOT->NAME,(char*)ch->DATA)){
                        for(ch3=chain_neg; ch3; ch3=ch3->NEXT){
                            line3=(ttvline_list*)ch3->DATA;
                            if(!line3) continue;
                            if(!strcmp(line3->NODE->ROOT->NAME,(char*)ch->DATA)){
                                if(line2->ROOT == line3->ROOT){
                                    if(line2->MDMAX && line3->MDMAX){
                                        tmodel2=stm_getmodel(ttvfig->INFO->FIGNAME, line2->MDMAX);
                                        tmodel3=stm_getmodel(ttvfig->INFO->FIGNAME, line3->MDMAX);
                                        if((tmodel2->ENERGYTYPE == STM_ENERGY_TABLE) && (tmodel3->ENERGYTYPE == STM_ENERGY_TABLE)){
                                            if(stm_modtbl_value_minslew_maxcapa(tmodel2->ENERGYMODEL.ETABLE) <= stm_modtbl_value_minslew_maxcapa(tmodel3->ENERGYMODEL.ETABLE)){
                                                ch2->DATA = NULL;
                                            }else{
                                                ch3->DATA = NULL;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }*/
        if(chain_posname){
            if(chain_pos){
                lib_drivepower_with_sdfcond (f, ttvfig, ptcellbefig, chain_posname, chain_pos, CBH_NONINVERT, chain_nonunate);
                freechain(chain_pos);
            }
            freechain(chain_posname);
        }
        if(chain_negname){
            if(chain_neg){
                lib_drivepower_with_sdfcond (f, ttvfig, ptcellbefig, chain_negname, chain_neg, CBH_INVERT, chain_nonunate);
                freechain(chain_neg);
            }
            freechain(chain_negname);
        }
        if(chain_triname){
            if(chain_tri){
                lib_drivepower_with_sdfcond (f, ttvfig, ptcellbefig, chain_triname, chain_tri, 0, NULL);
                freechain(chain_tri);
            }
            freechain(chain_triname);
        }
        if(chain_nonunate){
            freechain(chain_nonunate);
        }
    }else{
        event_rise = &ttvsig->NODE[1];
        ptyperise = getptype (event_rise->USER, LIB_CONNECTOR_ENERGY_MODEL_NAME) ;
        event_fall = &ttvsig->NODE[0];
        ptypefall = getptype (event_fall->USER, LIB_CONNECTOR_ENERGY_MODEL_NAME) ;
        if(ptyperise || ptypefall){
            ttv_getsigname(ttvfig, bufsig, ttvsig);
            pinname = lib_get_tg_pinname(bufsig);
            sprintf(buf, "energy_%s", pinname);
            space(f,12);
            fprintf(f,"internal_power (%s) {\n", buf);
            if(ptyperise && ptypefall){
                tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, (char*)ptyperise->DATA);
                space(f,16);
                if(tmodel->ENERGYMODEL.ETABLE->CST != STM_NOVALUE){
                    fprintf(f,"rise_power (scalar) {\n");
                    lib_drivelutconst(f, tmodel->ENERGYMODEL.ETABLE->CST, 'E');
                }else{
                    fprintf(f,"rise_power (energy_%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
                    lib_drivelut (f, tmodel, 'E');
                }
                space(f,16);
                fprintf(f,"}\n");
                tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, (char*)ptypefall->DATA);
                space(f,16);
                if(tmodel->ENERGYMODEL.ETABLE->CST != STM_NOVALUE){
                    fprintf(f,"fall_power (scalar) {\n");
                    lib_drivelutconst(f, tmodel->ENERGYMODEL.ETABLE->CST, 'E');
                }else{
                    fprintf(f,"fall_power (energy_%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
                    lib_drivelut (f, tmodel, 'E');
                }
                space(f,16);
                fprintf(f,"}\n");
            }else{
                if(ptyperise){
                    tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, (char*)ptyperise->DATA);
                }else{
                    tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, (char*)ptypefall->DATA);
                }
                space(f,16);
                if(tmodel->ENERGYMODEL.ETABLE->CST != STM_NOVALUE){
                    fprintf(f,"power (scalar) {\n");
                    lib_drivelutconst(f, tmodel->ENERGYMODEL.ETABLE->CST, 'E');
                }else{
                    fprintf(f,"power (energy_%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
                    lib_drivelut (f, tmodel, 'E');
                }
                space(f,16);
                fprintf(f,"}\n");
            }
            space(f,12);
            fprintf(f,"}\n");
        }
    }
}

/****************************************************************************/
/*     function  lib_drivetiming                                            */
/****************************************************************************/
void lib_drivetiming (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, chain_list *chtsname, cbhseq *ptcbhseq, befig_list *ptcellbefig)
{
    int ev, ev2 ;
    ttvevent_list *ckevent ;
    ttvevent_list *outevent, *outevent2 ;
    ttvevent_list *inevent ;
    chain_list *ch,*ch2;
    chain_list *chain, *chain_pos, *chain_neg, *chain_posname, *chain_negname, *chain_tri, *chain_triname, *abl, *chain_nonunate;
    ptype_list *ptype ;
    ttvline_list *line, *line2 ;
    char flagtri;
    char flagmin = 1;
    char flag_async_direct = 0;
    char LIB_DRIVEDELAY = 'M';
    int ttx;

    if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_TTX)==TTV_FILE_TTX)
      ttx=1;
    else if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)
      ttx=0;
    else
      ttx=1;

    if (!strcasecmp (V_STR_TAB[__TMA_DRIVEDELAY].VALUE, "max"))       LIB_DRIVEDELAY = 'M';
    else if (!strcasecmp (V_STR_TAB[__TMA_DRIVEDELAY].VALUE, "min"))  LIB_DRIVEDELAY = 'm';
    else if (!strcasecmp (V_STR_TAB[__TMA_DRIVEDELAY].VALUE, "both")) LIB_DRIVEDELAY = 'B';
    
    if  ((((ttvsig->TYPE & TTV_SIG_CI)==TTV_SIG_CI) 
      && ((ttvsig->TYPE & TTV_SIG_CB)!=TTV_SIG_CB)) 
      || ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)){
        
        if (ttx)
          ttv_expfigsig (ttvfig, ttvsig, ttvfig->INFO->LEVEL, ttvfig->INFO->LEVEL, TTV_STS_DUAL_P|TTV_STS_DUAL_J|TTV_STS_DUAL_T, TTV_FILE_TTX) ;
        else
          ttv_expfigsig (ttvfig, ttvsig, ttvfig->INFO->LEVEL, ttvfig->INFO->LEVEL, TTV_STS_DUAL_F|TTV_STS_DUAL_E|TTV_STS_DUAL_D, TTV_FILE_DTX) ;
        
        chain=NULL;
        for(ev=0; ev < 2; ev++){
            inevent=&ttvsig->NODE[ev];
            if (ttx)
              ptype = getptype (inevent->USER, TTV_NODE_DUALPATH) ;
            else
              ptype = getptype (inevent->USER, TTV_NODE_DUALLINE) ;

            if (ptype)
                for (ch = (chain_list*)ptype->DATA ; ch ; ch = ch->NEXT)
					if((((ttvline_list*)ch->DATA)->TYPE & TTV_LINE_U)==TTV_LINE_U)
                 	    chain=addevent(chain, ((ttvline_list*)ch->DATA)->ROOT);
        }
        for(ch = chain; ch; ch=ch->NEXT){
            ckevent=(ttvevent_list*)ch->DATA;
            if(ckevent)
                lib_drivesetup_group(f, ttvfig, ttvsig, ckevent);
        }
        freechain(chain);
            
        chain=NULL;
        for(ev=0; ev < 2; ev++){
            inevent=&ttvsig->NODE[ev];
            if (ttx)
              ptype = getptype (inevent->USER, TTV_NODE_DUALPATH) ;
            else 
              ptype = getptype (inevent->USER, TTV_NODE_DUALLINE) ;
            if (ptype)
                for (ch = (chain_list*)ptype->DATA ; ch ; ch = ch->NEXT)
					if((((ttvline_list*)ch->DATA)->TYPE & TTV_LINE_O)==TTV_LINE_O)
                        chain=addevent(chain, ((ttvline_list*)ch->DATA)->ROOT);
        }
        for(ch = chain; ch; ch=ch->NEXT){
            ckevent=(ttvevent_list*)ch->DATA;
            if(ckevent)
                lib_drivehold_group(f, ttvfig, ttvsig, ckevent);
        }
        freechain(chain);
    }
    if((ttvsig->TYPE & TTV_SIG_CO)==TTV_SIG_CO) {
        chain=NULL;
        flagmin = 1;
        for(ev=0;ev < 2; ev++){
            outevent=&ttvsig->NODE[ev];
            for (line = lib_getnodeinputline(outevent, ttx) ; line ; line = line->NEXT){
               	if(((line->TYPE & TTV_LINE_A)==TTV_LINE_A) ||
                     (getptype (line->NODE->ROOT->USER, TTV_SIG_CLOCK) && !getptype (line->ROOT->ROOT->USER, TTV_SIG_CLOCK) && V_BOOL_TAB[__LIB_DRIVE_COMB_AS_ACCESS].VALUE)){
                    chain=addevent(chain, line->NODE);
                    if((LIB_DRIVEDELAY == 'm') || (LIB_DRIVEDELAY == 'B')){
                        if(!line->MDMIN)
                            flagmin = 0;
                    }else{
                        flagmin = 0;
                    }
                }
            }
        }
        for(ch = chain; ch; ch=ch->NEXT){
            ckevent=(ttvevent_list*)ch->DATA;
            if(ckevent){
                flag_async_direct = 0;
                for(ev=0;ev < 2; ev++){
                    outevent=&ttvsig->NODE[ev];
                    for (line = lib_getnodeinputline(outevent, ttx) ; line ; line = line->NEXT){
                        if((line->NODE == ckevent) && getptype (ckevent->ROOT->USER, TTV_SIG_ASYNCHRON) && ((line->TYPE & TTV_LINE_A)!=TTV_LINE_A))
                            flag_async_direct = 1;
                    }
                }
                if(flag_async_direct) continue;
                if((LIB_DRIVEDELAY == 'M') || (LIB_DRIVEDELAY == 'B'))
                    lib_driveaccess_group(f, ttvfig, ttvsig, ckevent, ptcbhseq, 'M');
                if(flagmin)
                    lib_driveaccess_group(f, ttvfig, ttvsig, ckevent, ptcbhseq, 'm');
            }
        }
        freechain(chain);
        chain_tri=NULL;
        chain_triname=NULL;
        chain_posname=NULL;
        chain_negname=NULL;
        chain_pos=NULL;
        chain_neg=NULL;
        chain_nonunate = NULL;
        for(ev=0;ev < 2; ev++){
            outevent=&ttvsig->NODE[ev];
            for (line = lib_getnodeinputline(outevent, ttx) ; line ; line = line->NEXT){
                if(getptype(line->USER, LIB_DRIVED_LINE)) continue;
        	    if(((line->TYPE & TTV_LINE_U)!=TTV_LINE_U)
         		  &&((line->TYPE & TTV_LINE_O)!=TTV_LINE_O)
         		  &&((line->TYPE & TTV_LINE_A)!=TTV_LINE_A)){
                    flagtri = 0;
                    if(chtsname){
                        for(ch=chtsname; ch; ch=ch->NEXT){
                            if(!strcmp(line->NODE->ROOT->NAME,(char*)ch->DATA)){
                                flagtri = 1;
                                break;
                            }
                        }
                    }else{
                        if(((ttvsig->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ) || ((ttvsig->TYPE & TTV_SIG_CT)==TTV_SIG_CT)){
                            for(ev2=0;ev2 < 2; ev2++){
                                outevent2=&ttvsig->NODE[ev2];
                                for (line2 = lib_getnodeinputline(outevent2, ttx) ; line2 ; line2 = line2->NEXT){
        	                        if(((line2->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) && 
                                        (line->NODE->ROOT==line2->NODE->ROOT) &&
                                        (line->ROOT->ROOT==line2->ROOT->ROOT)){
                                        flagtri = 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    if(flagtri){
                        chain_tri=addchain(chain_tri, line);
                        for(chain = chain_triname; chain; chain = chain->NEXT)
                            if(!strcmp(line->NODE->ROOT->NAME, (char*)chain->DATA))
                                break;
                        if(!chain)
                            chain_triname=addname(chain_triname, line->NODE->ROOT->NAME);
                    }else{
                        if((line->NODE->TYPE ) == (line->ROOT->TYPE )){
                            if(ptcellbefig){
                                abl = cbh_calccondition(ptcellbefig, line->ROOT->ROOT->NAME, line->NODE->ROOT->NAME, CBH_NONINVERT);
                                if(!abl || (abl && (strcmp (VALUE_ATOM (abl), "'0'")))) {
                                    chain_pos=addchain(chain_pos, line);
                                    chain_posname=addname(chain_posname, line->NODE->ROOT->NAME);
                                }
                            }else{
                                chain_pos=addchain(chain_pos, line);
                                chain_posname=addname(chain_posname, line->NODE->ROOT->NAME);
                            }
                        }else{
                            if(ptcellbefig){
                                abl = cbh_calccondition(ptcellbefig, line->ROOT->ROOT->NAME, line->NODE->ROOT->NAME, CBH_INVERT);
                                if(!abl || (abl && (strcmp (VALUE_ATOM (abl), "'0'")))) {
                                    chain_neg=addchain(chain_neg, line);
                                    chain_negname=addname(chain_negname, line->NODE->ROOT->NAME);
                                }
                            }else{
                                chain_neg=addchain(chain_neg, line);
                                chain_negname=addname(chain_negname, line->NODE->ROOT->NAME);
                            }
                        }
                    }
                }
            }
        }
        for(ch=chain_posname; ch; ch=ch->NEXT){
           for(ch2=chain_negname; ch2; ch2=ch2->NEXT){
               if(!strcmp((char*)ch->DATA,(char*)ch2->DATA)){
                   chain_nonunate=addname(chain_nonunate, ch->DATA);
               }
           }
        }
        if(chain_posname){
            if(chain_pos){
                if((LIB_DRIVEDELAY == 'M') || (LIB_DRIVEDELAY == 'B'))
                    lib_drive_pos(f, ttvfig, chain_posname, chain_pos, 'M', chain_nonunate, ptcellbefig);
                if((LIB_DRIVEDELAY == 'm') || (LIB_DRIVEDELAY == 'B')){
                    flagmin = 1;
                    for(chain = chain_pos; chain; chain = chain->NEXT)
                        if(!((ttvline_list*)chain->DATA)->MDMIN)
                            flagmin = 0;
                    if(flagmin)
                        lib_drive_pos(f, ttvfig, chain_posname, chain_pos, 'm', chain_nonunate, ptcellbefig);
                }
                freechain(chain_pos);
            }
            freechain(chain_posname);
        }
        if(chain_negname){
            if(chain_neg){
                if((LIB_DRIVEDELAY == 'M') || (LIB_DRIVEDELAY == 'B'))
                    lib_drive_neg(f, ttvfig, chain_negname, chain_neg, 'M', chain_nonunate, ptcellbefig);
                if((LIB_DRIVEDELAY == 'm') || (LIB_DRIVEDELAY == 'B')){
                    flagmin = 1;
                    for(chain = chain_neg; chain; chain = chain->NEXT)
                        if(!((ttvline_list*)chain->DATA)->MDMIN)
                            flagmin = 0;
                    if(flagmin)
                        lib_drive_neg(f, ttvfig, chain_negname, chain_neg, 'm', chain_nonunate, ptcellbefig);
                }
                freechain(chain_neg);
            }
            freechain(chain_negname);
        }
        if(chain_triname){
            if(chain_tri){
                if((LIB_DRIVEDELAY == 'M') || (LIB_DRIVEDELAY == 'B'))
                    lib_drive_tri(f, ttvfig, chain_triname, chain_tri, 'M');
                if((LIB_DRIVEDELAY == 'm') || (LIB_DRIVEDELAY == 'B')){
                    flagmin = 1;
                    for(chain = chain_tri; chain; chain = chain->NEXT)
                        if((!((ttvline_list*)chain->DATA)->MDMIN) && ((((ttvline_list*)chain->DATA)->TYPE & TTV_LINE_HZ)!=TTV_LINE_HZ))
                            flagmin = 0;
                    if(flagmin)
                        lib_drive_tri(f, ttvfig, chain_triname, chain_tri, 'm');
                }
                freechain(chain_tri);
            }
            freechain(chain_triname);
        }
        if(chain_nonunate){
            freechain(chain_nonunate);
        }
        for(ev=0;ev < 2; ev++){
            outevent=&ttvsig->NODE[ev];
            for (line = lib_getnodeinputline(outevent, ttx) ; line ; line = line->NEXT)
                if(getptype(line->USER, LIB_DRIVED_LINE))
                    line->USER = delptype(line->USER, LIB_DRIVED_LINE);
        }
    }
}

/****************************************************************************/
/*     function  lib_drivesetup_group                                       */
/****************************************************************************/
void lib_drivesetup_group (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, ttvevent_list *ckevent)
{
    char buf[1024];
    ttvline_list *line ;
    char buftiming[1024];
    char bufsig[1024];
    char bufttype[1024];
    char *relpinname;
    char *pinname;
    int ttx;

    if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_TTX)==TTV_FILE_TTX)
      ttx=1;
    else if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)
      ttx=0;
    else
      ttx=1;

    ttv_getsigname(ttvfig, bufsig, ttvsig);
    pinname = lib_get_tg_pinname(bufsig);
    sprintf(buftiming, "%s_", pinname);
    ttv_getsigname(ttvfig, buf, ckevent->ROOT);
    relpinname = lib_get_tg_pinname(buf);
    strcat(buftiming, relpinname);
    if((ckevent->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
        if(getptype (ttvsig->USER, TTV_SIG_ASYNCHRON))
            sprintf(bufttype, "recovery_rising");
        else
            sprintf(bufttype, "setup_rising");
    }else{
        if(getptype (ttvsig->USER, TTV_SIG_ASYNCHRON))
            sprintf(bufttype, "recovery_falling");
        else
            sprintf(bufttype, "setup_falling");
    }
    strcat(buftiming, "_");
    strcat(buftiming, bufttype);

	space(f,12);
    fprintf(f,"timing (%s) {\n", buftiming);
    
    space(f,16);
    fprintf(f,"timing_type : %s ;\n", bufttype);

    relpinname = lib_getpinname(buf);
    space(f,16);
    fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
    
    for(line=lib_getnodeinputline(ckevent, ttx);line;line=line->NEXT)
        if(line->NODE->ROOT==ttvsig)
            if((line->TYPE & TTV_LINE_U)==TTV_LINE_U)
                lib_drivesetup(f, ttvfig, line);

    space(f,12);
    fprintf(f,"}\n");
}

/****************************************************************************/
/*     function  lib_drivehold_group                                        */
/****************************************************************************/
void lib_drivehold_group (FILE *f, ttvfig_list *ttvfig, ttvsig_list *ttvsig, ttvevent_list *ckevent)
{
    char buf[1024];
    ttvline_list *line ;
    char buftiming[1024];
    char bufsig[1024];
    char bufttype[1024];
    char *relpinname;
    char *pinname;
    int ttx;

    if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_TTX)==TTV_FILE_TTX)
      ttx=1;
    else if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)
      ttx=0;
    else
      ttx=1;

    ttv_getsigname(ttvfig, bufsig, ttvsig);
    pinname = lib_get_tg_pinname(bufsig);
    sprintf(buftiming, "%s_", pinname);
    ttv_getsigname(ttvfig, buf, ckevent->ROOT);
    relpinname = lib_get_tg_pinname(buf);
    strcat(buftiming, relpinname);
    if((ckevent->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
        if(getptype (ttvsig->USER, TTV_SIG_ASYNCHRON))
            sprintf(bufttype, "removal_rising");
        else
            sprintf(bufttype, "hold_rising");
    }else{
        if(getptype (ttvsig->USER, TTV_SIG_ASYNCHRON))
            sprintf(bufttype, "removal_falling");
        else
            sprintf(bufttype, "hold_falling");
    }
    strcat(buftiming, "_");
    strcat(buftiming, bufttype);
	
    space(f,12);
    fprintf(f,"timing (%s) {\n", buftiming);
    
    space(f,16);
    fprintf(f,"timing_type : %s ;\n", bufttype);
    
    relpinname = lib_getpinname(buf);
    space(f,16);
    fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
    
    for(line=lib_getnodeinputline(ckevent, ttx);line;line=line->NEXT)
        if(line->NODE->ROOT==ttvsig)
            if((line->TYPE & TTV_LINE_O)==TTV_LINE_O)
                lib_drivehold(f, ttvfig, line);
    
    space(f,12);
    fprintf(f,"}\n");
}

/****************************************************************************/
/*     function  lib_driveaccess_group                                      */
/****************************************************************************/
void lib_driveaccess_group (FILE *f, ttvfig_list *ttvfig, ttvsig_list *outsig, ttvevent_list *ckevent, cbhseq *ptcbhseq, char type)
{
    char buf[1024];
    int ev ;
    ttvline_list *line ;
    chain_list *chain, *ch;
    int flag=0;
    int access=0;
    char buftiming[1024];
    char bufsig[1024];
    char bufttype[1024];
    char buftsense[1024];
    char *relpinname;
    char *pinname;
    int ttx;

    if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_TTX)==TTV_FILE_TTX)
      ttx=1;
    else if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)
      ttx=0;
    else
      ttx=1;

    ttv_getsigname(ttvfig, bufsig, outsig);
    pinname = lib_get_tg_pinname(bufsig);
    if(type == 'M')
        sprintf(buftiming, "maxd_%s_", pinname);
    else
        sprintf(buftiming, "mind_%s_", pinname);
    
    ttv_getsigname(ttvfig, buf, ckevent->ROOT);
    relpinname = lib_get_tg_pinname(buf);
    strcat(buftiming, relpinname);

    if(ptcbhseq){
        if(ptcbhseq->RESET){
            chain=supportChain_listExpr(ptcbhseq->RESET);
            for(ch=chain; ch; ch=ch->NEXT)
                if(!strcmp(buf,(char*)ch->DATA)){
                    if(!strcmp(outsig->NAME,ptcbhseq->PIN))
                        flag=1;
                    else if(!strcmp(outsig->NAME,ptcbhseq->NEGPIN))
                        flag=2;
                    break;
                }
            freechain(chain);
        }
        if(ptcbhseq->SET){
            chain=supportChain_listExpr(ptcbhseq->SET);
            for(ch=chain; ch; ch=ch->NEXT)
                if(!strcmp(buf,(char*)ch->DATA)){
                    if(!strcmp(outsig->NAME,ptcbhseq->PIN))
                        flag=2;
                    else if(!strcmp(outsig->NAME,ptcbhseq->NEGPIN))
                        flag=1;
                    break;
                }
            freechain(chain);
        }
    }
    if(!flag){
        if((ckevent->TYPE & TTV_NODE_UP)==TTV_NODE_UP)
            sprintf(bufttype,"rising_edge");
        else
            sprintf(bufttype,"falling_edge");
        strcat(buftiming, "_");
        strcat(buftiming, bufttype);

        space(f,12);
        fprintf(f,"timing (%s) {\n", buftiming);
        space(f,16);
        fprintf(f,"timing_type : %s ;\n", bufttype);
        relpinname = lib_getpinname(buf);
        space(f,16);
        fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
    }
    

    
	for (ev = 0 ; ev < 2 ; ev++){ 
        access = 0;
    	for(line = lib_getnodeinputline(&outsig->NODE[ev], ttx) ; line ; line = line->NEXT)
        	if(line->NODE == ckevent)
            	if((line->TYPE & TTV_LINE_A)==TTV_LINE_A) access=1;
    	for(line = lib_getnodeinputline(&outsig->NODE[ev], ttx) ; line ; line = line->NEXT)
        	if(line->NODE == ckevent)
            	if(((line->TYPE & TTV_LINE_A)==TTV_LINE_A) || 
                    (((line->TYPE & TTV_LINE_A)!=TTV_LINE_A) && getptype (line->NODE->ROOT->USER, TTV_SIG_CLOCK) && !getptype (line->ROOT->ROOT->USER, TTV_SIG_CLOCK) &&
                       !access && V_BOOL_TAB[__LIB_DRIVE_COMB_AS_ACCESS].VALUE)){

                    if(!getptype(line->USER, LIB_DRIVED_LINE))
                        line->USER = addptype(line->USER, LIB_DRIVED_LINE, NULL);
                    if(flag){
                        if((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                            sprintf(bufttype,"preset");
                        else
                            sprintf(bufttype,"clear");
                            
                            
                        if(ckevent->TYPE == line->ROOT->TYPE )
                            sprintf(buftsense,"positive_unate");
                        else
                            sprintf(buftsense,"negative_unate");
                        strcat(buftiming, "_");
                        strcat(buftiming, bufttype);
                        strcat(buftiming, "_");
                        strcat(buftiming, buftsense);

                        space(f,12);
                        fprintf(f,"timing (%s) {\n", buftiming);
                        space(f,16);
                        fprintf(f,"timing_type : %s ;\n", bufttype);
                        space(f,16);
                        fprintf(f,"timing_sense : %s ;\n", buftsense);
                        relpinname = lib_getpinname(buf);
                        space(f,16);
                        fprintf(f,"related_pin : \"%s\" ;\n", relpinname);
                    }
                    if(type == 'M')
        		        lib_drivedelaymax(f, ttvfig, line);
                    else
    		            lib_drivedelaymin(f, ttvfig, line);
                    if(flag){
                        space(f,12);
                        fprintf(f,"}\n");
                    }
                }
    }
    if(!flag){
        space(f,12);
        fprintf(f,"}\n");
    }
    
}

/****************************************************************************/
/*     function  lib_get_timing_sense                                       */
/****************************************************************************/
char *lib_get_timing_sense (ttvsig_list *outsig, ttvsig_list *ptsig)
{
    int ev ;
    ttvline_list *line ;
    int neg = 0 ;
    int pos = 0 ;
    
    // a ajouter si utilise un jour: prise en compte ttvfig en dtx
    
	for (ev = 0 ; ev < 2 ; ev++)
    	for(line = (&outsig->NODE[ev])->INPATH ; line ; line = line->NEXT)
        	if(line->NODE->ROOT == ptsig)
        		if(((line->TYPE & TTV_LINE_U)!=TTV_LINE_U)
         		&&((line->TYPE & TTV_LINE_O)!=TTV_LINE_O)
         		&&((line->TYPE & TTV_LINE_A)!=TTV_LINE_A))
                {
                 if((line->ROOT->TYPE & (TTV_NODE_UP|TTV_NODE_DOWN)) == (line->NODE->TYPE & (TTV_NODE_UP|TTV_NODE_DOWN)))  
                     pos = 1 ;
                 if((line->ROOT->TYPE & (TTV_NODE_UP|TTV_NODE_DOWN)) != (line->NODE->TYPE & (TTV_NODE_UP|TTV_NODE_DOWN)))  
                     neg = 1 ;
                }

    if((neg == 1) && (pos == 1))
        return("non_unate") ;
    else if(neg == 1)
        return("negative_unate") ;
    else if(pos == 1)
        return("positive_unate") ;
    else
        return(NULL) ;
    
}

/****************************************************************************/
/*     function  lib_drivesetup                                             */
/****************************************************************************/
void lib_drivesetup (FILE *f, ttvfig_list *ttvfig, ttvline_list *line)
{
    
    if((line->NODE->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
        if(line->MDMAX)
            lib_driveconstraintmodel(f, ttvfig, line->MDMAX, 'R');
        else
            lib_drive_cst_constraint(f, line->VALMAX/TTV_UNIT, 'R');
    }else{
        if(line->MDMAX)
            lib_driveconstraintmodel(f, ttvfig, line->MDMAX, 'F');
        else
            lib_drive_cst_constraint(f, line->VALMAX/TTV_UNIT, 'F');
    }
}


/****************************************************************************/
/*     function  lib_drivehold                                              */
/****************************************************************************/
void lib_drivehold (FILE *f, ttvfig_list *ttvfig, ttvline_list *line)
{
    
    if((line->NODE->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
        if(line->MDMAX)
            lib_driveconstraintmodel(f, ttvfig, line->MDMAX, 'R');
        else
            lib_drive_cst_constraint(f, line->VALMAX/TTV_UNIT, 'R');
    }else{
        if(line->MDMAX)
            lib_driveconstraintmodel(f, ttvfig, line->MDMAX, 'F');
        else
            lib_drive_cst_constraint(f, line->VALMAX/TTV_UNIT, 'F');
    }
}

/****************************************************************************/
/*     function  lib_drivedelaymax                                          */
/****************************************************************************/
void lib_drivedelaymax (FILE *f, ttvfig_list *ttvfig, ttvline_list *line)
{
	lofig_list *lofig;
    char       typemax ;
    double     rmax, c1max, c2max ;
    locon_list *locon=NULL ;
    rcx_list   *ptrcx ;
    rcx_slope  slope;
    long       search;
    float      capa;
    char       trans;
    
	lofig = ttv_getrcxlofig (ttvfig);
    if(lofig == NULL)
     {
      c1max = line->ROOT->ROOT->CAPA / 1000.0 ;
      c2max = 0.0 ;
      rmax = -1.0;
      typemax = RCX_CAPALOAD ;
     }
    else
     {
      locon = rcx_gethtrcxcon(NULL,lofig,line->ROOT->ROOT->NAME) ;

      if((locon == NULL) || (locon->PNODE == NULL) || 
         ((ptrcx = getrcx(locon->SIG)) == NULL))
       {
        c1max = line->ROOT->ROOT->CAPA / 1000.0 ;
        c2max = 0.0 ;
        rmax = -1.0 ;
        typemax = RCX_CAPALOAD ;
       }
      else
       {
        if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
          slope.F0MAX  = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                           line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                           line->ROOT,
                                           search        | 
                                           TTV_FIND_GATE | 
                                           TTV_FIND_RC   | 
                                           TTV_FIND_MAX,
                                           TTV_MODE_LINE
                                         )/TTV_UNIT;
          slope.FCCMAX = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                           line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                           line->ROOT,
                                           search        | 
                                           TTV_FIND_GATE | 
                                           TTV_FIND_RC   | 
                                           TTV_FIND_MAX,
                                           TTV_MODE_DELAY
                                         )/TTV_UNIT;
        }
        else {
          slope.F0MAX  = -1.0 ;
          slope.FCCMAX = -1.0 ;
        }
        
        slope.SENS   = ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                       ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
        slope.CCA   = -1.0 ;

        typemax = rcx_rcnload( lofig, 
                               locon->SIG, 
                               locon->PNODE,
                               &rmax, &c1max, &c2max, 
                               RCX_BESTLOAD, 
                               0.0,
                               &slope, 
                               RCX_MAX 
                             ) ;

       }
     }
     capa = ((typemax==RCX_CAPALOAD) ? c1max*1000.0 : (c1max+c2max)*1000.0);
    
    if((line->ROOT->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
        trans = 'R';
        if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) trans = 'F';
        if(line->MDMAX)
            lib_drivedelaymodel(f, ttvfig, line->MDMAX, trans, capa);
        else
            lib_driveintrinsic(f, line->VALMAX/TTV_UNIT, trans);
        if(line->MFMAX)
            lib_driveslewmodel(f, ttvfig, line->MFMAX, trans, capa);
        else
            lib_driveslew(f, line->FMAX/TTV_UNIT, trans);
    }else{
        trans = 'F';
        if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) trans = 'R';
        if(line->MDMAX)
            lib_drivedelaymodel(f, ttvfig, line->MDMAX, trans, capa);
        else
            lib_driveintrinsic(f, line->VALMAX/TTV_UNIT, trans);
        if(line->MFMAX)
            lib_driveslewmodel(f, ttvfig, line->MFMAX, trans, capa);
        else
            lib_driveslew(f, line->FMAX/TTV_UNIT, trans);
    }
}

/****************************************************************************/
/*     function  lib_drivedelaymin                                          */
/****************************************************************************/
void lib_drivedelaymin (FILE *f, ttvfig_list *ttvfig, ttvline_list *line)
{
	lofig_list *lofig;
    char       typemin ;
    double     rmin, c1min, c2min ;
    locon_list *locon=NULL ;
    rcx_list   *ptrcx ;
    rcx_slope  slope;
    long       search;
    float      capa;
    char       trans;
    
	lofig = ttv_getrcxlofig (ttvfig);
    if(lofig == NULL)
     {
      c1min = line->ROOT->ROOT->CAPA / 1000.0 ;
      c2min = 0.0 ;
      rmin = -1.0;
      typemin = RCX_CAPALOAD ;
     }
    else
     {
      locon = rcx_gethtrcxcon(NULL,lofig,line->ROOT->ROOT->NAME) ;

      if((locon == NULL) || (locon->PNODE == NULL) || 
         ((ptrcx = getrcx(locon->SIG)) == NULL))
       {
        c1min = line->ROOT->ROOT->CAPA / 1000.0 ;
        c2min = 0.0 ;
        rmin = -1.0 ;
        typemin = RCX_CAPALOAD ;
       }
      else
       {
        if( rcx_crosstalkactive( RCX_QUERY ) != RCX_NOCROSSTALK ) {
          slope.F0MAX  = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                           line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                           line->ROOT,
                                           search        | 
                                           TTV_FIND_GATE | 
                                           TTV_FIND_RC   | 
                                           TTV_FIND_MAX,
                                           TTV_MODE_LINE
                                         )/TTV_UNIT;
          slope.FCCMAX = ttv_getslopenode( line->ROOT->ROOT->ROOT,
                                           line->ROOT->ROOT->ROOT->INFO->LEVEL,
                                           line->ROOT,
                                           search        | 
                                           TTV_FIND_GATE | 
                                           TTV_FIND_RC   | 
                                           TTV_FIND_MAX,
                                           TTV_MODE_DELAY
                                         )/TTV_UNIT;
        }
        else {
          slope.F0MAX  = -1.0 ;
          slope.FCCMAX = -1.0 ;
        }
        
        slope.SENS   = ((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP) 
                       ? TRC_SLOPE_UP : TRC_SLOPE_DOWN; 
        slope.CCA   = -1.0 ;

        typemin = rcx_rcnload( lofig, 
                               locon->SIG, 
                               locon->PNODE,
                               &rmin, &c1min, &c2min, 
                               RCX_BESTLOAD, 
                               0.0,
                               &slope, 
                               RCX_MIN 
                             ) ;

       }
     }
     capa = ((typemin==RCX_CAPALOAD) ? c1min*1000.0 : (c1min+c2min)*1000.0);
    
    if((line->ROOT->TYPE & TTV_NODE_UP)==TTV_NODE_UP){
        trans = 'R';
        if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) trans = 'F';
        if(line->MDMIN)
            lib_drivedelaymodel(f, ttvfig, line->MDMIN, trans, capa);
        else
            lib_driveintrinsic(f, line->VALMIN/TTV_UNIT, trans);
        if(line->MFMIN)
            lib_driveslewmodel(f, ttvfig, line->MFMIN, trans, capa);
        else
            lib_driveslew(f, line->FMIN/TTV_UNIT, trans);
    }else{
        trans = 'F';
        if((line->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) trans = 'R';
        if(line->MDMIN)
            lib_drivedelaymodel(f, ttvfig, line->MDMIN, trans, capa);
        else
            lib_driveintrinsic(f, line->VALMIN/TTV_UNIT, trans);
        if(line->MFMIN)
            lib_driveslewmodel(f, ttvfig, line->MFMIN, trans, capa);
        else
            lib_driveslew(f, line->FMIN/TTV_UNIT, trans);
    }
}

/****************************************************************************/
/*     function  lib_driveheader                                            */
/****************************************************************************/
void lib_driveheader (FILE *f, char *libname, ttvinfo_list *ttvinfo)
{

    char  *tecname;
    char  *dmname;
    char  *vunit;
    char  *tunit;
    char  *punit;
    char  *cunit;
    char  *runit;
    char  *curunit;
    double voltage;
    double temp, sutr, sltr, sutf, sltf, dtr, dtf;
	time_t counter;
	char  *date;
	double rise_temp_derating = 0.0 ;
	double fall_temp_derating = 0.0 ;
	double rise_volt_derating = 0.0 ;
	double fall_volt_derating = 0.0 ;
	char         buffer[2048];
    float  slewderate;
    
  	time (&counter) ;
	date = ctime (&counter) ;
	date[strlen (date) - 1] = '\0' ;
    tecname="cmos";
    dmname="table_lookup";
    vunit="1V";
    tunit=V_STR_TAB[__TMA_TUNIT].VALUE;
    cunit=V_STR_TAB[__TMA_CUNIT].VALUE;
    punit=V_STR_TAB[__TMA_PUNIT].VALUE;
    runit="1ohm";
    curunit="1mA";
    slewderate = V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE;

    if(!ttvinfo){
        voltage = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
        temp = STM_DEFAULT_TEMP;
        sutr = STM_DEFAULT_SMAXR;
        sltr = STM_DEFAULT_SMINR;
        sutf = STM_DEFAULT_SMAXF;
        sltf = STM_DEFAULT_SMINF;
        dtr = 0.5;
        dtf = 0.5;
    }else{
        voltage = ttvinfo->VDD;
        temp = ttvinfo->TEMP;
        sutr = sutf = ttvinfo->STHHIGH;
        sltr = sltf = ttvinfo->STHLOW;
        dtr  = dtf  = ttvinfo->DTH;
    }

    /* Derating */
    if(V_BOOL_TAB[__TUT_CALC_DERATE].VALUE) {
        if(V_FLOAT_TAB[__TUT_MIN_VOLT].VALUE < TUT_MINTEMPVOLT)
            TUT_MINVOLT = 0.9 * voltage ;
        if(V_FLOAT_TAB[__TUT_MAX_VOLT].VALUE < TUT_MINTEMPVOLT)
            TUT_MAXVOLT = 1.1 * voltage ;
        if(V_FLOAT_TAB[__TUT_MIN_TEMP].VALUE < TUT_MINTEMPVOLT)
            TUT_MINTEMP = 0.5 * temp ;
        if(V_FLOAT_TAB[__TUT_MAX_TEMP].VALUE < TUT_MINTEMPVOLT)
            TUT_MAXTEMP = 1.5 * temp ;
 
 	   rise_temp_derating = lib_derate_temp(TUT_MINTEMP,TUT_MAXTEMP,LIB_RISING);
 	   fall_temp_derating = lib_derate_temp(TUT_MINTEMP,TUT_MAXTEMP,LIB_FALLING);
 	   rise_volt_derating = lib_derate_tension(TUT_MINVOLT,TUT_MAXVOLT,LIB_RISING);
 	   fall_volt_derating = lib_derate_tension(TUT_MINVOLT,TUT_MAXVOLT,LIB_FALLING);
    }

#if 1
    sprintf(buffer,"liberty data flow `%s`\n",libname);
    avt_printExecInfoFlourish(f, "/*", buffer, "*/");
#else
    fprintf(f,"/*****************************************************************\n");
    fprintf(f,"*\n");
    fprintf(f,"*                 LIBRARY :   %s\n", libname);
    fprintf(f,"*\n");
    fprintf(f,"*                 Comment :   This library was created by\n");
    fprintf(f,"*                             AVERTEC with TMA\n");
    fprintf(f,"*\n");
    fprintf(f,"*                 date    :   %s\n", date);
    fprintf(f,"*\n");
    fprintf(f,"*****************************************************************/\n");
#endif

    saut(f,2);
    
    fprintf(f,"library (%s) {\n",libname);
    saut(f,1);
    tab(f);
    fprintf(f,"technology (%s) ;\n",tecname);
    tab(f);
    fprintf(f,"date                          : \"%s\" ;\n",date);
//    if(STM_TEMPLATES_CHAIN){
        tab(f);
        fprintf(f,"delay_model                   : %s ;\n",dmname);
        if(V_BOOL_TAB[__TUT_CALC_DERATE].VALUE) {
             tab(f);
             fprintf(f,"k_process_rise_propagation    : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_rise_propagation       : %.4f ;\n",
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_rise_propagation       : %.4f ;\n",
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_fall_propagation    : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_fall_propagation       : %.4f ;\n",
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_fall_propagation       : %.4f ;\n",
                     fall_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_rise_transition     : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_rise_transition        : %.4f ;\n",
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_rise_transition        : %.4f ;\n",
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_fall_transition     : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_fall_transition        : %.4f ;\n",
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_fall_transition        : %.4f ;\n",
                     fall_volt_derating) ;
 		   	
             tab(f);
             fprintf(f,"k_process_hold_rise           : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_process_hold_fall           : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_hold_rise              : %.4f ;\n",
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_temp_hold_fall              : %.4f ;\n",
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_hold_rise              : %.4f ;\n",
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_volt_hold_fall              : %.4f ;\n",
                     fall_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_setup_rise          : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_process_setup_fall          : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_setup_rise             : %.4f ;\n",
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_temp_setup_fall             : %.4f ;\n",
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_setup_rise             : %.4f ;\n",
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_volt_setup_fall             : %.4f ;\n",
                     fall_volt_derating) ;
        }
/*    }else{
        tab(f);
        fprintf(f,"default_intrinsic_rise        : 1.0 ;\n");
        tab(f);
        fprintf(f,"default_intrinsic_fall        : 1.0 ;\n");
        tab(f);
        fprintf(f,"default_slope_rise            : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_slope_fall            : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_inout_pin_rise_res    : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_inout_pin_fall_res    : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_output_pin_rise_res   : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_output_pin_fall_res   : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_rise_pin_resistance   : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_fall_pin_resistance   : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_rise_delay_intercept  : 0.0 ;\n");
        tab(f);
        fprintf(f,"default_fall_delay_intercept  : 0.0 ;\n");
        if(V_BOOL_TAB[__TUT_CALC_DERATE].VALUE) {
             tab(f);
             fprintf(f,"k_process_intrinsic_fall      : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_process_intrinsic_rise      : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_intrinsic_fall         : %.4f ;\n", 
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_temp_intrinsic_rise         : %.4f ;\n", 
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_intrinsic_fall         : %.4f ;\n", 
                     fall_volt_derating) ;
             tab(f);
             fprintf(f,"k_volt_intrinsic_rise         : %.4f ;\n", 
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_slope_fall          : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_process_slope_rise          : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_slope_fall             : %.4f ;\n", 
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_temp_slope_rise             : %.4f ;\n", 
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_slope_fall             : %.4f ;\n", 
                     fall_volt_derating) ;
             tab(f);
             fprintf(f,"k_volt_slope_rise             : %.4f ;\n", 
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_hold_rise           : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_process_hold_fall           : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_hold_rise              : %.4f ;\n",
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_temp_hold_fall              : %.4f ;\n",
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_hold_rise              : %.4f ;\n",
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_volt_hold_fall              : %.4f ;\n",
                     fall_volt_derating) ;
             tab(f);
             fprintf(f,"k_process_setup_rise          : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_process_setup_fall          : %.4f ;\n", LIB_DERATE_PROCESS) ;
             tab(f);
             fprintf(f,"k_temp_setup_rise             : %.4f ;\n",
                     rise_temp_derating) ;
             tab(f);
             fprintf(f,"k_temp_setup_fall             : %.4f ;\n",
                     fall_temp_derating) ;
             tab(f);
             fprintf(f,"k_volt_setup_rise             : %.4f ;\n",
                     rise_volt_derating) ;
             tab(f);
             fprintf(f,"k_volt_setup_fall             : %.4f ;\n",
                     fall_volt_derating) ;
        }
    }*/
        
    
    tab(f);
    fprintf(f,"nom_voltage                   : %.2f ;\n",voltage);
    tab(f);
    fprintf(f,"nom_temperature               : %.1f ;\n",temp);
    tab(f);
    fprintf(f,"nom_process                   : %.1f ;\n",LIB_NOM_PROCESS);
    tab(f);
    fprintf(f,"slew_derate_from_library      : %.1f ;\n",slewderate);
    if(!strcasecmp(cunit,"ff")){
    tab(f);
    fprintf(f,"default_fanout_load           : 1000.0 ;\n");
    }else if(!strcasecmp(cunit,"pf")){
    tab(f);
    fprintf(f,"default_fanout_load           : 1.0 ;\n");
    }
    if(!strcasecmp(cunit,"ff")){
    tab(f);
    fprintf(f,"default_inout_pin_cap         : 1000.0 ;\n");
    }else if(!strcasecmp(cunit,"pf")){
    tab(f);
    fprintf(f,"default_inout_pin_cap         : 1.0 ;\n");
    }
    if(!strcasecmp(cunit,"ff")){
    tab(f);
    fprintf(f,"default_input_pin_cap         : 1000.0 ;\n");
    }else if(!strcasecmp(cunit,"pf")){
    tab(f);
    fprintf(f,"default_input_pin_cap         : 1.0 ;\n");
    }
    tab(f);
    fprintf(f,"default_output_pin_cap        : 0.0 ;\n");
    tab(f);
    fprintf(f,"voltage_unit                  : \"%s\" ;\n",vunit);
    tab(f);
    fprintf(f,"time_unit                     : \"1%s\" ;\n",tunit);
    tab(f);
    fprintf(f,"capacitive_load_unit            (1,%s) ;\n",cunit);
    tab(f);
    fprintf(f,"pulling_resistance_unit       : \"%s\" ;\n",runit);
    tab(f);
    fprintf(f,"current_unit                  : \"%s\" ;\n",curunit);
    if(V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE && (V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE != 5)){
      tab(f);
      fprintf(f,"leakage_power_unit            : \"%s\" ;\n", punit);
      tab(f);
      fprintf(f,"default_cell_leakage_power    : 0.0 ;\n");
    }
    if(dtr < 0.0){
        dtr = 0.5;
    }
    if(dtf < 0.0){
        dtf = 0.5;
    }
    tab(f);
    fprintf(f,"input_threshold_pct_rise      : %.1f ;\n", dtr*100);
    tab(f);
    fprintf(f,"input_threshold_pct_fall      : %.1f ;\n", dtf*100);
    tab(f);
    fprintf(f,"output_threshold_pct_rise     : %.1f ;\n", dtr*100);
    tab(f);
    fprintf(f,"output_threshold_pct_fall     : %.1f ;\n", dtf*100);
    if((sltf < 0.0) || (sutf < 0.0) || (sltr < 0.0) || (sutr < 0.0))
        fprintf (stderr, "***lib warning: slew thresholds are not set***\n"); 
    if(sltf < 0.0){
        sltf = 0.2;
    }
    if(sutf < 0.0){
        sutf = 0.8;
    }
    if(sltr < 0.0){
        sltr = 0.2;
    }
    if(sutr < 0.0){
        sutr = 0.8;
    }
    tab(f);
    fprintf(f,"slew_lower_threshold_pct_fall : %.1f ;\n", sltf*100);
    tab(f);
    fprintf(f,"slew_upper_threshold_pct_fall : %.1f ;\n", sutf*100);
    tab(f);
    fprintf(f,"slew_lower_threshold_pct_rise : %.1f ;\n", sltr*100);
    tab(f);
    fprintf(f,"slew_upper_threshold_pct_rise : %.1f ;\n", sutr*100);
    fflush (f);
    
}

/****************************************************************************/
/*     function  lib_scm2thr                                                */
/****************************************************************************/
double lib_scm2thr(double fscm)
{
/*    return (elpScm2Thr(fscm, STM_DEFAULT_SMINR, 
            STM_DEFAULT_SMAXR, STM_DEFAULT_VT, 
            V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, elpRISE));*/
    return fscm;
}

/****************************************************************************/
/*     function  lib_driveindex                                             */
/****************************************************************************/
void lib_driveindex (FILE *f, timing_ttable *template, int n)
{
    int x,y;
    
        space(f,n);
        fprintf(f,"index_1 (\"");
        for(x=0 ; x < template->NX ; x++){
            if((template->XTYPE==STM_INPUT_SLEW) || (template->XTYPE==STM_CLOCK_SLEW)){
                fprintf(f,"%s", pfloat(buftunit,lib_scm2thr(template->XRANGE[x])/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE));
            }
            else{
                if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"ff")){
                    fprintf(f,"%.2f", template->XRANGE[x]);
                }else if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"pf")){
                    fprintf(f,"%.5f", template->XRANGE[x]/1000.0);
                }
            }
            if((template->NX - x) > 1){
                fprintf(f,", ");
            }
        }
        fprintf(f,"\");\n");
    
        if(template->NY){
            space(f,n);
            fprintf(f,"index_2 (\"");
            for(y=0 ; y < template->NY ; y++){
                if((template->YTYPE==STM_INPUT_SLEW) || (template->YTYPE==STM_CLOCK_SLEW)){
                    fprintf(f,"%s", pfloat(buftunit,lib_scm2thr(template->YRANGE[y])/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE));
                }
                else{
                    if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"ff")){
                        fprintf(f,"%.2f", template->YRANGE[y]);
                    }else if(!strcasecmp(V_STR_TAB[__TMA_CUNIT].VALUE,"pf")){
                        fprintf(f,"%.5f", template->YRANGE[y]/1000.0);
                    }
                }
                if((template->NY - y) > 1){
                    fprintf(f,", ");
                }
            }
            fprintf(f,"\");\n");
        }

}

/****************************************************************************/
/*     function  lib_drivelut                                               */
/****************************************************************************/
void lib_drivelut (FILE *f, timing_model *tmodel, char type)
{
    int x,y;
    char nx;
    char ny;
    float *values1d;
    float **values2d;

    if(type == 'E'){
        nx=tmodel->ENERGYMODEL.ETABLE->NX;
        ny=tmodel->ENERGYMODEL.ETABLE->NY;
        values1d=tmodel->ENERGYMODEL.ETABLE->SET1D;
        values2d=tmodel->ENERGYMODEL.ETABLE->SET2D;
    }else{
        nx=tmodel->UMODEL.TABLE->NX;
        ny=tmodel->UMODEL.TABLE->NY;
        values1d=tmodel->UMODEL.TABLE->SET1D;
        values2d=tmodel->UMODEL.TABLE->SET2D;
    }

    if (V_BOOL_TAB[__LIB_DRIVE_TABLE_INDEX].VALUE)
        lib_driveindex(f, tmodel->UMODEL.TABLE->TEMPLATE, 20);
    if(values1d){
        space(f,20);
        fprintf(f,"values (\"");
        for(x=0 ; x < nx ; x++){
            if(type == 'S')
                fprintf(f,"%s", pfloat(buftunit, lib_scm2thr(values1d[x])/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE));
            else if(type == 'D')
                fprintf(f,"%s", pfloat(buftunit, values1d[x]));
            else
                fprintf(f,"%s", ppower(buftunit, values1d[x]));
            if((nx-x) > 1){
                fprintf(f,", ");
            }
        }
        fprintf(f,"\");\n");
    }else if(values2d){
        space(f,20);
        fprintf(f,"values (\"");
        for(x=0 ; x < nx ; x++){
            for(y=0 ; y < ny ; y++){
                if(type == 'S')
                    fprintf(f,"%s", pfloat(buftunit, lib_scm2thr(values2d[x][y])/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE));
                else if(type == 'D')
                    fprintf(f,"%s", pfloat(buftunit, values2d[x][y]));
                else
                    fprintf(f,"%s", ppower(buftunit, values2d[x][y]));
                if((ny-y) > 1){
                    fprintf(f,", ");
                }else if((nx-x) > 1){
                    fprintf(f,"\", \\\n");
                    space(f, 28);
                    fprintf(f,"\"");
                }
                else
                    fprintf(f,"\");\n");
            }
        }
    }
}

/****************************************************************************/
/*     function  lib_drivelutconst                                          */
/****************************************************************************/
void lib_drivelutconst (FILE *f, float value, char type)
{
//    int x,y;
    

    space(f,20);
    if(type == 'E')
        fprintf(f,"values (\"%s\");\n", ppower(buftunit, value));
    else
        fprintf(f,"values (\"%s\");\n", pfloat(buftunit, value));
/*    fprintf(f,"values (\"");
    for(x=0 ; x < 2 ; x++){
        for(y=0 ; y < 2 ; y++){
            fprintf(f,"%s", pfloat(buftunit, value));
            if((2-y) > 1){
                fprintf(f,", ");
            }else if((2-x) > 1){
                fprintf(f,"\", \\\n");
                space(f, 28);
                fprintf(f,"\"");
            }
            else
                fprintf(f,"\");\n");
        }
    }*/
}

int lib_drive_rail_connection(FILE *f, ttvfig_list *ttvfig)
{
    int i, nbrail=0;
    ttvsig_list *ttvsig;
    float low=-1.0, high=-1.0, alim, nom_voltage;
    ptype_list *ptype;
    char *name;
    char buf[1024];
    chain_list *cl=NULL,*chain;
    char flagdefault;
    
    nom_voltage = ttvfig->INFO->VDD;


    if(libpowerptype){
        cl = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_B,NULL) ;
        for (chain = cl; chain; chain = chain->NEXT){
            ttvsig=(ttvsig_list *)chain->DATA;
            if (ttv_get_signal_swing(ttvfig, ttvsig, &low, &high) && (high > 0.0)){
                alim = high - low;
                if(!mbk_cmpdouble(nom_voltage, alim, 1e6)){
                    flagdefault = 1;
                    break;
                }
            }
        }

        i = 0;
        for(ptype = libpowerptype; ptype; ptype = ptype->NEXT){
            for (chain = cl; chain; chain = chain->NEXT){
                ttvsig=(ttvsig_list *)chain->DATA;
                if (!ttv_get_signal_swing(ttvfig, ttvsig, &low, &high)){
                    alim = high - low;
                    if(!mbk_cmpdouble(*(float*)&ptype->DATA, alim, 1e6)){
                        if(flagdefault){
//                            space(f,8);
//                            fprintf(f, "rail_connection (PV0, VDD0) ;\n");
                            nbrail++;
                            flagdefault = 0;
                        }
                        i++;
                        sprintf (buf, "PV%d", i);
                        name = namealloc(buf);
//                        space(f,8);
//                        fprintf(f, "rail_connection (%s, %s) ;\n", name, (char*)ptype->TYPE);
                        nbrail++;
                        break;
                    }
                }
            }
        }
        if(cl) freechain(cl);
    }
    return nbrail;
}

void lib_drive_power_supply_group(FILE *f, chain_list *figlist, ttvinfo_list *ttvinfo)
{
    int i;
    chain_list  *ttv,*cl,*chain;
    ttvfig_list *ttvfig;
    ttvsig_list *ttvsig;
    float low, high, alim, nom_voltage;
    ptype_list *ptype;
    char *name;
    char buf[1024];
    
    if(!ttvinfo){
        nom_voltage = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    }else{
        nom_voltage = ttvinfo->VDD;
    }

    i = 0;
    for(ttv = figlist ; ttv ; ttv = ttv->NEXT){
        ttvfig=(ttvfig_list*)ttv->DATA;
        cl = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_B,NULL) ;
        for (chain = cl; chain; chain = chain->NEXT){
            ttvsig=(ttvsig_list *)chain->DATA;
            if (!ttv_get_signal_swing(ttvfig, ttvsig, &low, &high)){
                alim = high - low;
                if(mbk_cmpdouble(nom_voltage, alim, 1e6)){
                    for(ptype = libpowerptype; ptype; ptype = ptype->NEXT){
                        if(!mbk_cmpdouble(*(float*)&ptype->DATA, alim, 1e6)) break;
                    }
                    if(!ptype){
                        i++;
                        sprintf (buf, "VDD%d", i);
                        name = namealloc(buf);
                        libpowerptype = addptype(libpowerptype, (long)name, NULL);
                        *(float*)&libpowerptype->DATA = alim;
                    }
                }
            }
        }
        if(cl) freechain(cl);
    }
    if(libpowerptype){
        libpowerptype = (ptype_list*)reverse((chain_list*)libpowerptype);
        tab(f);
        fprintf(f, "power_supply () {\n");
        space(f,8);
        fprintf(f, "default_power_rail : VDD0 ;\n");
        for(ptype = libpowerptype; ptype; ptype = ptype->NEXT){
            space(f,8);
            fprintf(f, "power_rail (%s, %.2f) ;\n", (char*)ptype->TYPE, *(float*)&ptype->DATA);
        }
        tab(f);
        fprintf(f, "}\n");
    }
}

/****************************************************************************/
/*     function  lib_drivebustype                                           */
/****************************************************************************/
void lib_drivebustype (FILE *f, chain_list *figlist)
{
    chain_list  *ttv;
    chain_list  *buschain = NULL, *chain;
    ttvfig_list *ttvfig;
    ttvsig_list *ttvsig;
    int flagbus = 0, lsb = 0, msb = 0, width = 0;
    char *previousname = NULL, *devectname, *busname;
    int n, ninter ;
    typedef struct msblsb {
        int MSB;
        int LSB;
    } msblsb;
    msblsb *vect;

    
    for(ttv = figlist ; ttv ; ttv = ttv->NEXT){
        ttvfig=(ttvfig_list*)ttv->DATA;
        flagbus = 0;
        for (n=0; n < ttvfig->NBCONSIG; n++){
            ttvsig=ttvfig->CONSIG[n];
    
                devectname = ttv_devect(ttvsig->NAME);
                if(flagbus)
                    if(strcmp(busname,vectorradical(devectname))){
                        flagbus = 0;
                    }
                if((ttvsig->NAME != devectname) && (!flagbus)){
                    ninter = n;
                    flagbus = 1;
                    lsb = vectorindex(devectname);
                    busname = vectorradical(devectname);
                    for(ninter=n; ninter < ttvfig->NBCONSIG; ninter++){
                        ttvsig = ttvfig->CONSIG[ninter];
                        devectname = ttv_devect(ttvsig->NAME);
                        if((vectorradical(devectname) != previousname) && (n != ninter))
                            break;
                        previousname = vectorradical(devectname);
                        msb = vectorindex(devectname);
                    }
                    if(!buschain){
                        vect = (msblsb*)mbkalloc (sizeof (struct msblsb));
                        vect->MSB = msb;
                        vect->LSB = lsb;
                        buschain = addchain(buschain, vect);
                    }else{
                        for(chain = buschain; chain; chain = chain->NEXT){
                            if((((msblsb*)chain->DATA)->MSB == msb) && (((msblsb*)chain->DATA)->LSB == lsb))
                                break;
                        }
                        if(!chain){
                            vect = (msblsb*)mbkalloc (sizeof (struct msblsb));
                            vect->MSB = msb;
                            vect->LSB = lsb;
                            buschain = addchain(buschain, vect);
                        }
                    }
                }
                ttvsig = ttvfig->CONSIG[n];
                if(flagbus)
                    if((n + 1)  == ttvfig->NBCONSIG){
                        flagbus = 0;
                    }
        }
        if(flagbus){
            flagbus = 0;
        }
    }
    if(buschain){
        if(LIB_BUS_DELIM[1] != '\0'){
            tab(f);
            fprintf(f, "bus_naming_style : \"%%s%c%%d%c\" ;\n", LIB_BUS_DELIM[0], LIB_BUS_DELIM[1]);
        }
        else {
            tab(f);
            fprintf(f, "bus_naming_style : \"%%s%c%%d\" ;\n", LIB_BUS_DELIM[0]);
        }
    }
    for(chain = buschain; chain; chain = chain->NEXT){
        msb = ((msblsb*)chain->DATA)->MSB;
        lsb = ((msblsb*)chain->DATA)->LSB;
        tab(f);
        fprintf(f, "type (bus_%d_%d) {\n", msb, lsb);
        tab(f);
        fprintf(f, "base_type : array;\n");
        tab(f);
        fprintf(f, "data_type : bit;\n");
        if(msb > lsb)
            width = msb - lsb + 1;
        else
            width = lsb - msb + 1;
        tab(f);
        fprintf(f, "bit_width : %d;\n", width);
        tab(f);
        fprintf(f, "bit_from : %d;\n", msb);
        tab(f);
        fprintf(f, "bit_to : %d;\n", lsb);
        if(msb > lsb) {
            tab(f);
            fprintf(f, "downto : true;\n");
        }
        else {
            tab(f);
            fprintf(f, "downto : false;\n");
        }
        tab(f);
        fprintf(f, "}\n");

        mbkfree(chain->DATA);
    }
    freechain(buschain);
    buschain = NULL;
    
}

/****************************************************************************/
/*     function  lib_drivetemplate                                          */
/****************************************************************************/
void lib_drivetemplate (FILE *f)
{
	chain_list *tempchain;
	char *var1,*var2;
	timing_ttable *template;


	for(tempchain=STM_TEMPLATES_CHAIN;tempchain;tempchain=tempchain->NEXT){
		template=(timing_ttable*)(tempchain->DATA);
        tab(f);
        fprintf(f,"lu_table_template (%s) {\n",template->NAME);
        space(f,8);
		if(template->XTYPE==STM_INPUT_SLEW && template->YTYPE==STM_CLOCK_SLEW){
			var1="constrained_pin_transition";
			var2="related_pin_transition";
		}
        else if(template->XTYPE==STM_CLOCK_SLEW && template->YTYPE==STM_INPUT_SLEW){
			var2="constrained_pin_transition";
			var1="related_pin_transition";
		}
		else if(template->XTYPE==STM_CLOCK_SLEW)
			var1="input_net_transition";
		else if(template->XTYPE==STM_LOAD)
			var1="total_output_net_capacitance";
		else if(template->XTYPE==STM_INPUT_SLEW){
            if(template->XTYPEBIS)
                var1="constrained_pin_transition";
            else
			    var1="input_net_transition";
        }
		if(template->YTYPE==STM_LOAD)
			var2="total_output_net_capacitance";
		
		
        fprintf(f,"variable_1 : %s;\n",var1);
        if(template->NY){
            space(f,8);
            fprintf(f,"variable_2 : %s;\n",var2);
        }
        lib_driveindex (f, template, 8);

        tab(f);
        fprintf(f,"}\n");
	}
    
    if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 )
     ||( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 5 )){
		for(tempchain=STM_TEMPLATES_CHAIN;tempchain;tempchain=tempchain->NEXT){
			template=(timing_ttable*)(tempchain->DATA);
			if((template->XTYPE==STM_INPUT_SLEW && template->YTYPE==STM_CLOCK_SLEW)
              || (template->XTYPE==STM_CLOCK_SLEW && template->YTYPE==STM_INPUT_SLEW))
                continue;
            tab(f);
            fprintf(f,"power_lut_template (energy_%s) {\n",template->NAME);
            space(f,8);
			if((template->XTYPE==STM_CLOCK_SLEW)
            || (template->XTYPE==STM_INPUT_SLEW))
				var1="input_transition_time";
			else if(template->XTYPE==STM_LOAD)
				var1="total_output_net_capacitance";
			if(template->YTYPE==STM_LOAD)
				var2="total_output_net_capacitance";
			
            fprintf(f,"variable_1 : %s;\n",var1);
            if(template->NY){
                space(f,8);
                fprintf(f,"variable_2 : %s;\n",var2);
            }
            lib_driveindex (f, template, 8);
    
            tab(f);
            fprintf(f,"}\n");
		}
    }
/*    tab(f);
    fprintf(f,"lu_table_template (%s) {\n", TEMP_CONST_NAME);
    space(f,8);
    fprintf(f,"variable_1 : input_net_transition;\n");
    space(f,8);
    fprintf(f,"variable_2 : total_output_net_capacitance;\n");
    space(f,8);
    fprintf(f,"index_1 (\"1.0, 2.0\");\n");
    space(f,8);
    fprintf(f,"index_2 (\"1.0, 2.0\");\n");
    tab(f);
    fprintf(f,"}\n");
    tab(f);
    fprintf(f,"lu_table_template (%s) {\n", TEMP_SETHOL_CONST_NAME);
    space(f,8);
    fprintf(f,"variable_1 : constrained_pin_transition;\n");
    space(f,8);
    fprintf(f,"variable_2 : related_pin_transition;\n");
    space(f,8);
    fprintf(f,"index_1 (\"1.0, 2.0\");\n");
    space(f,8);
    fprintf(f,"index_2 (\"1.0, 2.0\");\n");
    tab(f);
    fprintf(f,"}\n");*/

}


/****************************************************************************/
/*     function  lib_driveconstraintmodel                                   */
/****************************************************************************/
void lib_driveconstraintmodel (FILE *f, ttvfig_list *ttvfig, char* name, char type)
{
    timing_model *tmodel ;
    float  constante ;
    
    tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, name);
    constante=tmodel->UMODEL.TABLE->CST;

    if(constante!=STM_NOVALUE){
        lib_drive_cst_constraint(f, constante, type);
    }else{        
        space(f,16);
        if(type=='R')
            fprintf(f,"rise_constraint (%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
        else if(type=='F')
            fprintf(f,"fall_constraint (%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);

        lib_drivelut (f, tmodel, 'D');
        space(f,16);
        fprintf(f,"}\n");
    }
}

/****************************************************************************/
/*     function  lib_drivedelaymodel                                        */
/****************************************************************************/
void lib_drivedelaymodel (FILE *f, ttvfig_list *ttvfig, char* name, char type, float capa)
{
    timing_model *tmodel ;
    float  constante ;
    
    tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, name);
    constante=tmodel->UMODEL.TABLE->CST;

    if(constante!=STM_NOVALUE){
        lib_driveintrinsic(f, constante, type);
    }else{        
        space(f,16);
        if(type=='R')
            fprintf(f,"cell_rise (%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
        else if(type=='F')
            fprintf(f,"cell_fall (%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
        if (getptype(ttvfig->USER, TTV_FIG_CAPAOUT_HANDLED)==NULL)
        {
          if(V_BOOL_TAB[__TMA_DRIVECAPAOUT].VALUE == 0 && !V_BOOL_TAB[__TMA_CHARAC_PRECISION].VALUE)
             stm_mod_shift(tmodel, capa);
        }
        lib_drivelut (f, tmodel, 'D');
        space(f,16);
        fprintf(f,"}\n");
    }
}

/****************************************************************************/
/*     function  lib_driveslewmodel                                             */
/****************************************************************************/
void lib_driveslewmodel (FILE *f, ttvfig_list *ttvfig, char* name, char type, float capa)
{
    timing_model *tmodel ;
    float  constante ;
    
    tmodel=stm_getmodel(ttvfig->INFO->FIGNAME, name);
    constante=tmodel->UMODEL.TABLE->CST;


    if(constante!=STM_NOVALUE){
        lib_driveslew(f, constante, type);
    }else{
        space(f,16);
        if(type=='R')
            fprintf(f,"rise_transition (%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
        else if(type=='F')
            fprintf(f,"fall_transition (%s) {\n",tmodel->UMODEL.TABLE->TEMPLATE->NAME);
        if (getptype(ttvfig->USER, TTV_FIG_CAPAOUT_HANDLED)==NULL)
        {
          if(V_BOOL_TAB[__TMA_DRIVECAPAOUT].VALUE == 0 && !V_BOOL_TAB[__TMA_CHARAC_PRECISION].VALUE)
             stm_mod_shift(tmodel, capa);
        }
        lib_drivelut (f, tmodel, 'S');
        space(f,16);
        fprintf(f,"}\n");
    }
}

/****************************************************************************/
/*     function  lib_drive_cst_constraint                                   */
/****************************************************************************/
void lib_drive_cst_constraint(FILE *f, float value, char type)
{
    space(f,16);
//    if(STM_TEMPLATES_CHAIN){
        if(type=='R')
            fprintf(f,"rise_constraint (scalar) {\n");
        else if(type=='F')
            fprintf(f,"fall_constraint (scalar) {\n");
        lib_drivelutconst (f, value, 'D');
        space(f,16);
        fprintf(f,"}\n");

/*    }else{
        if(type=='R'){
            fprintf(f,"intrinsic_rise : %s ;\n", pfloat(buftunit,value));
        }if(type=='F'){
            fprintf(f,"intrinsic_fall : %s ;\n", pfloat(buftunit,value));
        }
    }*/

}


/****************************************************************************/
/*     function  lib_driveintrinsic                                         */
/****************************************************************************/
void lib_driveintrinsic (FILE *f, float value, char type)
{
    space(f,16);
//    if(STM_TEMPLATES_CHAIN){
        if(type=='R')
            fprintf(f,"cell_rise (scalar) {\n");
        else if(type=='F')
            fprintf(f,"cell_fall (scalar) {\n");
        lib_drivelutconst (f, value, 'D');
        space(f,16);
        fprintf(f,"}\n");

/*    }else{
        if(type=='R'){
            fprintf(f,"intrinsic_rise : %s ;\n", pfloat(buftunit,value));
        }if(type=='F'){
            fprintf(f,"intrinsic_fall : %s ;\n", pfloat(buftunit,value));
        }
    }*/
}

/****************************************************************************/
/*     function  lib_driveslew                                              */
/****************************************************************************/
void lib_driveslew (FILE *f, float value, char type)
{
    space(f,16);
//    if(STM_TEMPLATES_CHAIN){
        if(type=='R')
            fprintf(f,"rise_transition (scalar) {\n");
        else if(type=='F')
            fprintf(f,"fall_transition (scalar) {\n");
        lib_drivelutconst (f, lib_scm2thr(value)/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE, 'S');
        space(f,16);
        fprintf(f,"}\n");

/*    }else{
        if(type=='R')
            fprintf(f,"slope_rise : %s ;\n", pfloat(buftunit,lib_scm2thr(value)/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE));
        if(type=='F')
            fprintf(f,"slope_fall : %s ;\n", pfloat(buftunit,lib_scm2thr(value)/V_FLOAT_TAB[__LIB_SLEW_DERATE].VALUE));
    }*/

}


/****************************************************************************/
/*     function  space                                                      */
/****************************************************************************/
void space (FILE *f, int nb)
{
    int i;
    
    if(FLAGBUS) tab(f);
    for (i=0 ; i < nb ;i++){
        fprintf(f," ");
    }
}

/****************************************************************************/
/*     function  tab                                                        */
/****************************************************************************/
void tab (FILE *f)
{
    fprintf(f,"    ");
}

/****************************************************************************/
/*     function  saut                                                       */
/****************************************************************************/
void saut (FILE *f, int nb)
{
    int i;
    
    for (i=0 ; i < nb ;i++){
        fprintf(f,"\n");
    }
}

/****************************************************************************/
/*     function  addevent                                                   */
/****************************************************************************/
chain_list *addevent (chain_list *chain, ttvevent_list *event)
{
    chain_list *ch ;

    for(ch=chain; ch; ch=ch->NEXT){
        if(ch->DATA==event)
            return chain;
    }
    return(addchain(chain, event));
}
/****************************************************************************/
/*     function  addname                                                   */
/****************************************************************************/
chain_list *addname (chain_list *chain, char *name)
{
    chain_list *ch ;

    for(ch=chain; ch; ch=ch->NEXT){
        if(ch->DATA==name)
            return chain;
    }
    return(addchain(chain, name));
}
/****************************************************************************/
/*     function  pfloat                                                   */
/****************************************************************************/
char *pfloat (char *buf, float time)
{

    if(!strcasecmp (V_STR_TAB[__TMA_TUNIT].VALUE, "ps")){
        sprintf (buf, "%.1f", time) ;
    }else if(!strcasecmp(V_STR_TAB[__TMA_TUNIT].VALUE,"ns")){
        sprintf (buf, "%.4f", time/1000.0) ;
    }
    
	return (buf) ;
}

/****************************************************************************/
/*     function  ppower                                                     */
/****************************************************************************/
char *ppower (char *buf, float power)
{

    if(!strcasecmp (V_STR_TAB[__TMA_CUNIT].VALUE, "pf"))
        sprintf (buf, "%.4f", power*1e12) ;
    else if(!strcasecmp (V_STR_TAB[__TMA_CUNIT].VALUE, "ff"))
        sprintf (buf, "%.1f", power*1e15) ;
    
	return (buf) ;
}
/****************************************************************************/
/*     function  lib_driveopcond : operating conditions                     */
/****************************************************************************/
void lib_driveopcond (FILE *f, char *opcondname, double process, double temp,
                      double volt)
{
    tab(f);
    fprintf(f,"operating_conditions(%s) {\n", opcondname) ; 
    space(f, 8);
    fprintf(f,"process : %.1f ;\n", process) ; 
    space(f, 8);
    fprintf(f,"temperature : %.1f ;\n", temp) ; 
    space(f, 8);
    fprintf(f,"voltage : %.2f ;\n", volt) ; 
    tab(f);
    fprintf(f,"}\n") ;
}

/****************************************************************************/
/*     function  lib_derate_tension :                                       */
/*     tpfinal = tpinitial*(1.0+dV*derate_tension)                          */
/****************************************************************************/
double lib_derate_tension(double V0, double V1, int transition)
{
    double derate_tension = 0.0 ;
    double rapport_Ids = 1.0 ;

    switch(transition) {
        case LIB_RISING :  rapport_Ids = mcc_calcRapIdsVolt(MCC_TECHFILE,
                                         NULL, MCC_PMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         12.0*MCC_TECSIZE*1.0e-6, MCC_TEMP,
                                         V0, V1) ;
                           break ;
        case LIB_FALLING : rapport_Ids = mcc_calcRapIdsVolt(MCC_TECHFILE,
                                         NULL, MCC_NMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         6.0*MCC_TECSIZE*1.0e-6, MCC_TEMP,
                                         V0, V1) ;
                           break ;
    }

    derate_tension = 1.0/(rapport_Ids-1.0)-1.0/(V1-V0) ;
    
    return(derate_tension) ;
}

/****************************************************************************/
/*     function  lib_derate_temp :                                          */
/*     tpfinal = tpinitial*(1.0+dV*derate_temp)                             */
/****************************************************************************/
double lib_derate_temp(double T0, double T1, int transition)
{
    double derate_temp = 0.0 ;
    double rapport_Ids = 1.0 ;

    switch(transition) {
        case LIB_RISING :  rapport_Ids = mcc_calcRapIdsTemp(MCC_TECHFILE,
                                         NULL, MCC_PMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         12.0*MCC_TECSIZE*1.0e-6, MCC_VDDmax,
                                         T0, T1) ;
                           derate_temp = (rapport_Ids-1.0)/(T1-T0) ;
                           break ;
        case LIB_FALLING : rapport_Ids = mcc_calcRapIdsTemp(MCC_TECHFILE,
                                         NULL, MCC_NMOS,
                                         MCC_TYPICAL, MCC_TECSIZE*1.0e-6,
                                         6.0*MCC_TECSIZE*1.0e-6, MCC_VDDmax,
                                         T0, T1) ;
                           derate_temp = (1.0/rapport_Ids-1.0)/(T1-T0) ;
                           break ;
    }
    
    return(derate_temp) ;
}

/****************************************************************************/
/*     function  lib_drive_remove_null                                      */
/*     fonction de blindage lors de l'appel depuis les api : enleve les     */
/*     champs null de la liste.                                             */
/****************************************************************************/
void lib_drive_remove_null( chain_list *figlistx, 
                            chain_list *befiglistx, 
                            chain_list **figlist, 
                            chain_list **befiglist 
                          )
{
  chain_list *chaintv, *chainbe ;
  
  *figlist   = NULL ;
  *befiglist = NULL ;
  
  for( chaintv = figlistx, chainbe = befiglistx ; 
       chaintv ;
       chaintv = chaintv->NEXT, 
       chainbe = ( chainbe ? chainbe->NEXT : NULL ) 
     )
  {
    if( chaintv->DATA ) {
      *figlist = addchain( *figlist, chaintv->DATA );
      if( chainbe )
        *befiglist = addchain( *befiglist, chainbe->DATA );
    }
  }

  *figlist = reverse( *figlist );
  *befiglist = reverse( *befiglist );
}
