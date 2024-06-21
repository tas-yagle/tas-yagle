/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_main.c                                                  */
/*                                                                          */
/*    (c) copyright 2000                                                    */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : ../../....     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

/* Static function declarations */

#define YAGH_NOMODEL (long)2
#define YAGH_FOUNDLEAF (long)1
#define YAGH_NOLEAF (long)0

static long yagHierDisassemble (char *figname, lofig_list *ptlofig);
static cnsfig_list *yagLeafDisassemble (char *figname, lofig_list *ptlofig);
static void yagOrientLoins (loins_list *ptloins);
static void yagOrientLofig (lofig_list *ptlofig);
static long traverseHierarchy (char *figname);

/****************************************************************************
*                         fonction yagle_main();                            *
****************************************************************************/
cnsfig_list *
yagle_main()
{
    lofig_list      *ptlofig = NULL;
    time_t          start = 0;
    time_t          end = 0;
    struct rusage   END;
    struct rusage   START; 

    avt_banner("yagle", "Yet Another Gate Level Extractor", "2000");

#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

    if (YAG_CONTEXT == NULL) yagBug(DBG_NO_CONTEXT,"yagle_main",NULL,NULL,0);
    if (YAG_CONTEXT->YAG_OUTNAME == NULL) YAG_CONTEXT->YAG_OUTNAME = YAG_CONTEXT->YAG_FIGNAME;

/*----------------------------------------------------------------------------*
| Load the logical figure to disassemble                                      |
+-----------------------------------------------------------------------------*/

    yagChrono(&START,&start);
    yagMessage(MES_LOADING,YAG_CONTEXT->YAG_FIGNAME);
    if (YAG_CONTEXT->YAG_FILENAME)
        ptlofig = getlofig (YAG_CONTEXT->YAG_FILENAME, 'A');
    if (YAG_CONTEXT->YAG_FIGNAME != YAG_CONTEXT->YAG_FILENAME) {
        ptlofig = getloadedlofig(YAG_CONTEXT->YAG_FIGNAME);
    }
    if (ptlofig == NULL) avt_errmsg(YAG_ERRMSG, "001", AVT_FATAL);
    if (YAG_CONTEXT->YAG_ONLY_GENIUS && rcn_getlofigcache(ptlofig) != NULL) avt_errmsg(YAG_ERRMSG, "002", AVT_FATAL);
    yagChrono(&END,&end);
    yagPrintTime(&START,&END,start,end);

/*----------------------------------------------------------------------------*
| Hierarchical disassembly mode                                               |
+-----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_HIERARCHICAL_MODE == TRUE) {
        if (yagHierDisassemble(YAG_CONTEXT->YAG_OUTNAME, ptlofig) != YAGH_NOLEAF) return NULL;
    }

/*----------------------------------------------------------------------------*
| Flat disassembly mode                                                       |
+-----------------------------------------------------------------------------*/

    return yagLeafDisassemble(YAG_CONTEXT->YAG_OUTNAME, ptlofig);

}

/****************************************************************************
*                     function yagHierDisassemble();                        *
****************************************************************************/

static ht *hierTreatedHT;

long
yagHierDisassemble(char *figname, lofig_list *ptlofig)
{
    long         result;

    hierTreatedHT = addht(40);

    ptlofig->NAME = namealloc(figname);
    result = traverseHierarchy(ptlofig->NAME);

    delht(hierTreatedHT);

    return result;
}

static long
traverseHierarchy(char *figname)
{
    lofig_list  *ptlofig, *ptnewfig;
    loins_list  *ptloins, *ptnextloins;
    chain_list  *inslist = NULL;
    cnsfig_list *ptcnsfig;
    char        *yaghinsname, *yaghfigname;
    char         buffer[255];
    long         type;
    long         res;
    int          all_leaf = TRUE;
    int          no_leaf = TRUE;

    if ((type = (long)gethtitem(hierTreatedHT, figname)) != (long)EMPTYHT) {
        return type;
    }

    ptlofig = getlofig(figname, 'A');
    if (ptlofig->LOINS == NULL && ptlofig->LOTRS == NULL) {
        avt_errmsg(YAG_ERRMSG, "010", AVT_WARNING, ptlofig->NAME);
    }

    if (incatalog(figname)) {
        ptcnsfig = yagLeafDisassemble(ptlofig->NAME, ptlofig);
        if (ptcnsfig && ptcnsfig->BEFIG) {
            addhtitem(hierTreatedHT, figname, YAGH_FOUNDLEAF);
            return YAGH_FOUNDLEAF;
        }
        else {
            addhtitem(hierTreatedHT, figname, YAGH_NOMODEL);
            return YAGH_NOMODEL;
        }
    }
    for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptnextloins) {
        ptnextloins = ptloins->NEXT;
        res = traverseHierarchy(ptloins->FIGNAME);
        if (res == YAGH_NOMODEL) {
            delloins(ptlofig, ptloins->INSNAME);
            no_leaf = FALSE;
        }
        else if (res == YAGH_NOLEAF) {
            all_leaf = FALSE;
            inslist = addchain(inslist, ptloins);
        }
        else no_leaf = FALSE;
    }
    if (no_leaf) {
        addhtitem(hierTreatedHT, figname, YAGH_NOLEAF);
        freechain(inslist);
        return YAGH_NOLEAF;
    }
    else if (all_leaf) {
        if (ptlofig->LOTRS != NULL) {
            avt_errmsg(YAG_ERRMSG, "011", AVT_WARNING, ptlofig->NAME);
        }
        if (ptlofig->LOTRS && V_BOOL_TAB[__YAGLE_HIER_LOTRS_GROUP].VALUE) {
            /* create intermediate hierarchy */
            sprintf(buffer, "yagh_%s", figname);
            yaghfigname = namealloc(buffer);
            sprintf(buffer, "yaghins_%s", figname);
            yaghinsname = namealloc(buffer);
            ptnewfig = unflattenlofig(ptlofig, yaghfigname, yaghinsname, NULL);
            lofigchain(ptnewfig);
            ptlofig->LOSIG = (losig_list *)reverse((chain_list *)ptlofig->LOSIG);
            ptcnsfig = yagLeafDisassemble(ptnewfig->NAME, ptnewfig);
            if (ptcnsfig && !ptcnsfig->BEFIG) delloins(ptlofig, yaghinsname);
        }
        for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
            yagOrientLoins(ptloins);
        }
        yagOrientLofig(ptlofig);
        ptlofig->LOSIG = (losig_list *)reverse((chain_list *)ptlofig->LOSIG);
        savelofig(ptlofig);
        addhtitem(hierTreatedHT, figname, YAGH_FOUNDLEAF);
        return YAGH_FOUNDLEAF;
    }
    else if (!no_leaf && !all_leaf) {
        /* create intermediate hierarchy */
        sprintf(buffer, "yagh_%s", figname);
        yaghfigname = namealloc(buffer);
        sprintf(buffer, "yaghins_%s", figname);
        yaghinsname = namealloc(buffer);
        ptnewfig = unflattenlofig(ptlofig, yaghfigname, yaghinsname, inslist);
        lofigchain(ptnewfig);
        ptlofig->LOSIG = (losig_list *)reverse((chain_list *)ptlofig->LOSIG);
        ptcnsfig = yagLeafDisassemble(ptnewfig->NAME, ptnewfig);
        if (ptcnsfig && !ptcnsfig->BEFIG) delloins(ptlofig, yaghinsname);
        for (ptloins = ptlofig->LOINS; ptloins; ptloins = ptloins->NEXT) {
            yagOrientLoins(ptloins);
        }
        yagOrientLofig(ptlofig);
        savelofig(ptlofig);

        addhtitem(hierTreatedHT, figname, YAGH_FOUNDLEAF);
        freechain(inslist);
        return YAGH_FOUNDLEAF;
    }

    /* never reached */
    addhtitem(hierTreatedHT, figname, YAGH_NOLEAF);
    freechain(inslist);
    return YAGH_NOLEAF;
} 
 
static void
yagOrientLoins(loins_list *ptloins)
{
    lofig_list  *ptlofig;
    locon_list  *ptlocon, *ptfigcon;
    locon_list  *ptlocon_match;
    int          reverse_done = FALSE;

    ptlofig = getlofig(ptloins->FIGNAME, 'P');
    if (ptloins->LOCON->NAME != ptlofig->LOCON->NAME) {
        ptlofig->LOCON = (locon_list *)reverse((chain_list *)ptlofig->LOCON);
        reverse_done = TRUE;
    }
    for (ptlocon = ptloins->LOCON, ptfigcon = ptlofig->LOCON; ptlocon && ptfigcon; ptlocon = ptlocon->NEXT, ptfigcon = ptfigcon->NEXT) {
        if (ptlocon->NAME == ptfigcon->NAME) {
            ptlocon->DIRECTION = ptfigcon->DIRECTION;
        }
        else {
            for (ptlocon_match = ptlofig->LOCON; ptlocon_match; ptlocon_match = ptlocon_match->NEXT) {
                if (ptlocon_match->NAME == ptlocon->NAME) break;
            }
            if (ptlocon_match != NULL) {
                ptlocon->DIRECTION = ptlocon_match->DIRECTION;
            }
        }
    }
    if (reverse_done) {
        ptlofig->LOCON = (locon_list *)reverse((chain_list *)ptlofig->LOCON);
    }
}

static void
yagOrientLofig(lofig_list *ptlofig)
{
    locon_list  *ptlocon;
    locon_list  *ptintcon;
    chain_list  *ptchain;
    int          incon;
    int          outcon;
    int          tristatecon;

    lofigchain(ptlofig);
    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        incon = FALSE;
        outcon = FALSE;
        tristatecon = FALSE;
        ptchain = (chain_list *)getptype(ptlocon->SIG->USER, LOFIGCHAIN)->DATA;
        for (; ptchain; ptchain = ptchain->NEXT) {
            ptintcon = (locon_list *)ptchain->DATA;
            if (ptintcon->TYPE != INTERNAL) continue;
            if (ptintcon->DIRECTION == IN || ptintcon->DIRECTION == INOUT || ptintcon->DIRECTION == TRANSCV) {
                incon = TRUE;
            }
            if (ptintcon->DIRECTION == OUT || ptintcon->DIRECTION == INOUT) {
                outcon = TRUE;
            }
            if (ptintcon->DIRECTION == TRISTATE || ptintcon->DIRECTION == TRANSCV) {
                tristatecon = TRUE;
            }
        }
        if (incon && outcon) ptlocon->DIRECTION = INOUT;
        else if (incon && tristatecon) ptlocon->DIRECTION = TRANSCV;
        else if (outcon) ptlocon->DIRECTION = OUT;
        else if (tristatecon) ptlocon->DIRECTION = TRISTATE;
        else ptlocon->DIRECTION = IN;
    }
}

/****************************************************************************
*                     function yagLeafDisassemble();                        *
****************************************************************************/

/* Core function for flat and hierarchical Yagle */

static cnsfig_list *
yagLeafDisassemble(char *figname, lofig_list *ptlofig)
{
    cnsfig_list    *ptcnsfig = NULL;
    losig_list     *ptlosig;
    long            mode = 0;
    time_t          start = 0;
    time_t          end = 0;
    struct rusage   END;
    struct rusage   START; 
    inffig_list    *ifl;
    chain_list     *list0;
    char            buffer[YAGBUFSIZE];
    ptype_list     *ptype;
    eqt_ctx        *mc_ctx = NULL;
    
    YAG_CONTEXT->YAGLE_NB_ERR = 0;
    YAG_CONTEXT->YAG_OUTNAME = figname;

#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

/*----------------------------------------------------------------------------*
| Create new figure ignoring instances from blackbox list                     |
+-----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_BLACKBOX_SUPPRESS == TRUE) {
        strcpy(buffer, YAG_CONTEXT->YAG_OUTNAME);
        strcat(buffer, "_yagtrans");
        YAG_CONTEXT->YAG_FIGNAME = namealloc(buffer);
        YAG_CONTEXT->YAG_OUTNAME = YAG_CONTEXT->YAG_FIGNAME;
        yagChrono(&START,&start);
        yagMessage(MES_UNFLAT,YAG_CONTEXT->YAG_FIGNAME);
        lofigchain(ptlofig);
        unflatOutsideList(ptlofig, YAG_CONTEXT->YAG_FIGNAME, YAG_CONTEXT->YAG_FIGNAME);
        lofigchain(ptlofig);
        savelofig(ptlofig);
        ptlofig = getlofig(YAG_CONTEXT->YAG_FIGNAME, 'A');
        lofigchain(ptlofig);
        savelofig(ptlofig);
        yagChrono(&END,&end);
        yagPrintTime(&START,&END,start,end);
    }

/*----------------------------------------------------------*
| Flatten the figure                                        |
*-----------------------------------------------------------*/

    ptype = getptype (ptlofig->USER, PARAM_CONTEXT);
    flatten_parameters (ptlofig, NULL, ptype ? (eqt_param *) ptype->DATA : NULL , 0, 0, 0, 0, 0, 0, &mc_ctx);
    if (mc_ctx!=NULL) eqt_term(mc_ctx);

    if (YAG_CONTEXT->YAG_BLACKBOX_IGNORE == TRUE) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_FLATTENING,NULL);
        flatOutsideList(ptlofig);
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }
    else if (ptlofig->LOINS != NULL) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_FLATTENING,NULL);
        rflattenlofig(ptlofig, 'Y', NO);
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }

/*----------------------------------------------------------------------------*
| remove RC networks if specified                                             |
+-----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_NORC == TRUE) {
        for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
            if (ptlosig->PRCN) freelorcnet(ptlosig);
        }
    }

/*----------------------------------------------------------*
| Open the report File                                      |
*-----------------------------------------------------------*/

    YAG_CONTEXT->YAGLE_ERR_FILE = mbkfopen(YAG_CONTEXT->YAG_OUTNAME, "rep", WRITE_TEXT);
    if(YAG_CONTEXT->YAGLE_ERR_FILE == NULL) {
        sprintf(buffer,"%s.rep", YAG_CONTEXT->YAG_OUTNAME);
        avt_errmsg(YAG_ERRMSG, "006", AVT_FATAL, buffer);
    }
    sprintf(buffer,"Report file : %s.rep\n",YAG_CONTEXT->YAG_OUTNAME);
    avt_printExecInfo( YAG_CONTEXT->YAGLE_ERR_FILE, "#", buffer, "");
    
/*----------------------------------------------------------------------------*
| Disassemble the transistor netlist                                          |
+-----------------------------------------------------------------------------*/

#ifndef WITHOUT_TAS
    if(YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING || YAG_CONTEXT->YAG_ELP || YAG_CONTEXT->YAG_USESTMSOLVER) {
        yagPrepTiming(ptlofig);
    }
#endif
    yagChrono(&START,&start);
    avt_log(LOGYAG,0,"------------------------------------------------------------\n");
    yagMessage(MES_DISASSEMBLING, YAG_CONTEXT->YAG_OUTNAME);
    avt_log(LOGYAG,0,"------------------------------------------------------------\n");
    if (YAG_CONTEXT->YAG_BEFIG) mode |= YAG_GENBEFIG;
    if (YAG_CONTEXT->YAG_CONE_NETLIST) mode |= YAG_GENLOFIG;
    ptlofig->LOCON = (locon_list *)reverse((chain_list *)ptlofig->LOCON);
    ptcnsfig = yagDisassemble(YAG_CONTEXT->YAG_OUTNAME, ptlofig, mode);
    yagChrono(&END,&end);
    avt_log(LOGYAG,0,"TOTAL DISASSEMBLY TIME                  ");
    yagPrintTime(&START,&END,start,end);
    avt_log(LOGYAG,0,"------------------------------------------------------------\n");


#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

/*----------------------------------------------------------------------------*
| Save cone netlist figure to disk                                            |
+-----------------------------------------------------------------------------*/

    if(YAG_CONTEXT->YAG_FILE != 0 && ptcnsfig != NULL) {
        yagMessage(MES_SAVE_CNS,ptcnsfig->NAME);
        savecnsfig(ptcnsfig, ptlofig);
    }

/*----------------------------------------------------------------------------*
| Add delay information                                                       |
+-----------------------------------------------------------------------------*/

#ifndef WITHOUT_TAS
    if(YAG_CONTEXT->YAGLE_NB_ERR == 0 && YAG_CONTEXT->YAG_TAS_TIMING != YAG_NO_TIMING && ptcnsfig != NULL) {
    #ifdef AVERTEC_LICENSE
        if(avt_givetoken("YAGLE_LICENSE_SERVER", "yagtime")!=AVT_VALID_TOKEN) exit(1);
    #endif
        avt_log(LOGYAG,0,"------------------------------------------------------------\n");
        yagAddTiming(ptcnsfig, ptlofig);
        avt_log(LOGYAG,0,"------------------------------------------------------------\n");
    }
#endif

    fclose(YAG_CONTEXT->YAGLE_ERR_FILE);
    YAG_CONTEXT->YAGLE_ERR_FILE = NULL;

/*----------------------------------------------------------------------------*
| Save behavioural figure to disk                                             |
+-----------------------------------------------------------------------------*/

    ifl=getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);
    if (ptcnsfig != NULL && ptcnsfig->BEFIG != NULL) {
        ht *renamed=NULL, *morealiases=NULL;
        list0=inf_GetEntriesByType(ifl, INF_SUPPRESS, INF_ANY_VALUES);
        if (YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS == NULL && (list0 != NULL || YAG_CONTEXT->YAG_MINIMISE_INV || YAG_CONTEXT->YAG_SIMPLIFY_EXPR || YAG_CONTEXT->YAG_SIMPLIFY_PROCESSES)) {
            yagChrono(&START,&start);
            initializeBdd(0);
            beh_depend(ptcnsfig->BEFIG);
            yagMessage(MES_SUPPRESS_BEAUX, NULL);
            if (list0 != NULL || YAG_CONTEXT->YAG_MINIMISE_INV) {
                yagSuppressBeaux(ptcnsfig->BEFIG);
            }
            if (YAG_CONTEXT->YAG_SIMPLIFY_EXPR) yagSimplifyExpr(ptcnsfig->BEFIG);
            if (YAG_CONTEXT->YAG_SIMPLIFY_PROCESSES) yagSimplifyProcesses(ptcnsfig->BEFIG);
            destroyBdd(1);
            yagChrono(&END,&end);
            yagPrintTime(&START,&END,start,end);
        }
        if (ptcnsfig->LOINS != NULL && YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS != NULL) {
            beg_topLevel(&(ptcnsfig->BEFIG), YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS, YAG_CONTEXT->YAG_COMPACT_BEHAVIOUR, ptcnsfig->LOINS,&renamed,&morealiases);
            freechain(YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS);
            YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS = NULL;
        }
        yagCheckMissingDeclarations(ptcnsfig->BEFIG);
        if (V_BOOL_TAB[__YAG_DRIVE_ALIASES].VALUE)
          yagDriveConeSignalAliases(ptcnsfig, renamed, morealiases);
        if (renamed) delht(renamed);
        yagFreeAliasHT(morealiases);
        freechain(list0);
        yagMessage(MES_GEN_FILE,NULL);
        savebefig(ptcnsfig->BEFIG, 0);
        yagCleanBequad(ptcnsfig->BEFIG);
    }

/*----------------------------------------------------------------------------*
| Save logical figure to disk                                                 |
+-----------------------------------------------------------------------------*/

    if(ptcnsfig != NULL && (ptype = getptype(ptcnsfig->USER, YAG_CONE_NETLIST_PTYPE)) != NULL) {
        yagMessage(MES_MAKING_NETLIST,NULL);
        savelofig((lofig_list *)ptype->DATA);
    }

    yagMessage(MES_COMPLETED,NULL);
    
    return ptcnsfig;
}

