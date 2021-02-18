/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_disa.c                                                  */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 12/04/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

yag_context_list *YAG_CONTEXT = NULL;

/* initialise first context to default values */

static void
yagInitialise()
{
    YAG_CONTEXT->YAG_FILE = FALSE;
    YAG_CONTEXT->YAG_CONE_NETLIST = FALSE;
    YAG_CONTEXT->YAG_BEFIG = TRUE;
    YAG_CONTEXT->YAG_BLACKBOX_SUPPRESS = FALSE;
    YAG_CONTEXT->YAG_BLACKBOX_IGNORE = FALSE;
    YAG_CONTEXT->YAG_NORC = FALSE;
    YAG_CONTEXT->YAG_HIERARCHICAL_MODE = FALSE;
    YAG_CONTEXT->YAG_CURLOFIG = NULL;
    YAG_CONTEXT->YAG_CURCNSFIG = NULL;
    YAG_CONTEXT->YAG_STAT_MODE = FALSE;
    YAG_CONTEXT->YAG_LOOP_MODE = FALSE;
    YAG_CONTEXT->YAG_SILENT_MODE = FALSE;
    YAG_CONTEXT->YAG_HELP_S = FALSE;
    YAG_CONTEXT->YAG_DEBUG_CONE = NULL;
    YAG_CONTEXT->YAGLE_TOOLNAME = "yagle";
    YAG_CONTEXT->YAGLE_LANG = 'E';
    YAG_CONTEXT->YAGLE_ERR_FILE = NULL;
    YAG_CONTEXT->YAGLE_STAT_FILE = NULL;
    YAG_CONTEXT->YAGLE_LOOP_FILE = NULL;
    YAG_CONTEXT->YAGLE_NB_ERR = 0;   
    YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS = NULL;
    YAG_CONTEXT->YAG_MAX_LINKS = 6;
    YAG_CONTEXT->YAG_RELAX_LINKS = 6;
    YAG_CONTEXT->YAG_BDDCEILING = 10000;
    YAG_CONTEXT->YAG_MAXNODES = 0;
    YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
    YAG_CONTEXT->YAG_FILENAME = NULL;
    YAG_CONTEXT->YAG_FIGNAME = NULL;
    YAG_CONTEXT->YAG_OUTNAME = NULL;
    YAG_CONTEXT->YAG_REQUIRE_COMPLETE_GRAPH = TRUE;
    YAG_CONTEXT->YAG_USE_FCF = 1;           
    YAG_CONTEXT->YAG_ONE_SUPPLY = 0;        
    YAG_CONTEXT->YAG_NO_SUPPLY = 0;        
    YAG_CONTEXT->YAG_ORIENT = 0;            
    YAG_CONTEXT->YAG_PROP_HZ = 0;           
    YAG_CONTEXT->YAG_MAKE_CELLS = 1;        
    YAG_CONTEXT->YAG_GENIUS = 0;            
    YAG_CONTEXT->YAG_ONLY_GENIUS = 0;       
    YAG_CONTEXT->YAG_CELL_SHARE = 0;        
    YAG_CONTEXT->YAG_DETECT_LATCHES = 1;    
    YAG_CONTEXT->YAG_DETECT_PRECHARGE = 0;    
    YAG_CONTEXT->YAG_BLEEDER_STRICTNESS = 1;   
    YAG_CONTEXT->YAG_LOOP_ANALYSIS = 1;     
    YAG_CONTEXT->YAG_AUTO_RS = 1;
    YAG_CONTEXT->YAG_BUS_ANALYSIS = 0;      
    YAG_CONTEXT->YAG_FCL_DETECT = 0;        
    YAG_CONTEXT->YAG_ONLY_FCL = 0;          
    YAG_CONTEXT->YAG_DEPTH = 9;             
    YAG_CONTEXT->YAG_NOTSTRICT = 0;         
    YAG_CONTEXT->YAG_REMOVE_PARA = 1;       
    YAG_CONTEXT->YAG_MINIMISE_CONES = 0;    
    YAG_CONTEXT->YAG_RELAX_ALGO = 0;        
    YAG_CONTEXT->YAG_MINIMISE_INV = 0;      
    YAG_CONTEXT->YAG_GEN_SIGNATURE = 1;     
    YAG_CONTEXT->YAG_ASSUME_PRECEDE = 0;    
    YAG_CONTEXT->YAG_BLEEDER_PRECHARGE = 0; 
    YAG_CONTEXT->YAG_TRISTATE_MEMORY = 0;   
    YAG_CONTEXT->YAG_MARK_TRISTATE_MEMORY = 0;   
    YAG_CONTEXT->YAG_INTERFACE_VECTORS = 0; 
    YAG_CONTEXT->YAG_SIMPLIFY_EXPR = 0;     
    YAG_CONTEXT->YAG_SIMPLIFY_PROCESSES = 0;
    YAG_CONTEXT->YAG_COMPACT_BEHAVIOUR = 0;
    YAG_CONTEXT->YAG_ELP = 0;               
    YAG_CONTEXT->YAG_BLOCK_BIDIR = 0;       
    YAG_CONTEXT->YAG_STRICT_CKLATCH = 0;       
    YAG_CONTEXT->YAG_THRESHOLD = 3.0;      
    YAG_CONTEXT->YAG_TAS_TIMING = YAG_NO_TIMING; 
    YAG_CONTEXT->YAG_SPLITTIMING_RATIO = 3.0;
    YAG_CONTEXT->YAG_SENSITIVE_RATIO = 0.0;
    YAG_CONTEXT->YAG_SENSITIVE_MAX = 0;
    YAG_CONTEXT->YAG_CONSTRAINT_LIST = NULL;
    YAG_CONTEXT->YAG_BIABL_INDEX = 0;
    YAG_CONTEXT->YAG_CURCIRCUIT = NULL;
    YAG_CONTEXT->YAG_CONE_GRAPH = NULL;
    YAG_CONTEXT->YAG_SUPPORT_GRAPH = NULL;
    YAG_CONTEXT->YAG_ITERATIONS = 0;
    YAG_CONTEXT->YAG_KEEP_REDUNDANT = 0;
    YAG_CONTEXT->YAG_KEEP_GLITCHERS = 0;
    YAG_CONTEXT->YAG_AUTOLOOP_CEILING = 15;
    YAG_CONTEXT->YAG_AUTOLOOP_DEPTH = 9;
    YAG_CONTEXT->YAG_BUS_DEPTH = 9;
    YAG_CONTEXT->YAG_USESTMSOLVER = 0;
    YAG_CONTEXT->YAG_UNMARK_BISTABLES = 0;
    YAG_CONTEXT->YAG_LATCH_REQUIRE_CLOCK = 0;
    YAG_CONTEXT->YAG_AUTO_FLIPFLOP = 1;
    YAG_CONTEXT->YAG_AUTO_ASYNC = 0;
    YAG_CONTEXT->YAG_AUTO_MEMSYM = 0;
    YAG_CONTEXT->YAG_DETECT_REDUNDANT = 1;
    YAG_CONTEXT->YAG_DETECT_LEVELHOLD = 0;
    YAG_CONTEXT->YAG_SIMPLE_LATCH = 0;
    YAG_CONTEXT->YAG_USE_CONNECTOR_DIRECTION = 0;
    YAG_CONTEXT->YAG_CLOCK_GATE = 0;
    YAG_CONTEXT->YAG_BEFIG_LIST = NULL;
    YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM = 0;
    YAG_CONTEXT->YAG_MEMSYM_HEURISTIC = 0;
    YAG_CONTEXT->YAG_STUCK_LATCH = 1;
}

/****************************************************************************
 *                       function yagDisassemble();                         *
 ****************************************************************************/

                   /**** disassembly of a transistor netlist ****/

cnsfig_list *yagDisassemble(char *figname, lofig_list *ptmbkfig, long mode)
{
    cnsfig_list     *ptcnsfig = NULL;
    lofig_list      *ptrootlofig = NULL;
    lofig_list      *ptcorelofig = NULL;
    locon_list      *ptcon = NULL;
    locon_list      *ptheadintcon = NULL;
    locon_list      *ptintcon = NULL;
    losig_list      *ptsig = NULL;
    loins_list      *ptins = NULL;
    cone_list       *ptcone = NULL;
    cone_list       *ptsymcone = NULL;
    cone_list       *ptloopcone = NULL;
    cone_list       *ptnextcone;
    cell_list       *ptcell;
    cell_list       *ptnextcell;
    edge_list       *ptinputs;
    lotrs_list      *pttrans = NULL, *ptothertrans = NULL;
    chain_list      *ptchain;
    chain_list      *recognised;
    ptype_list      *ptuser;
    befig_list      *ptbefig;
    char            *name;
    char             buffer[1024];
    time_t          start = 0;
    time_t          end = 0;
    time_t          counter;
    struct rusage   END;
    struct rusage   START; 
    long            numpartial = 0;
    long            conesleft;
    long            previous;
    long            type;
    long            fcltype, otherfcltype;
    int             count;
    int             changes;
    int             build_befig;
    int             num_unused = 0;
    int             num_undrivengate = 0;
    int             num_bleeder = 0, num_levelhold = 0, num_latch = 0, num_rs = 0, num_flipflop = 0, num_dlatch = 0, num_bad = 0;
    int             num_loops = 0;
    int             num_noinputs = 0;
    int             num_nodrive = 0;
    int             num_bidir = 0;
    int             num_conf = 0;
    int             num_hz = 0;
    int             num_precharge = 0;
    int             numconunused = 0;
    long            num_cmosdual = 0;
    long            index = 1;
    long            value;
    int             relax = FALSE;
    int             close_err_file = FALSE;
    int             infval;
    inffig_list     *ifl;
    chain_list      *list0, *list1;
    int             loglvl1=0;
    int             stuck;

    mbk_comcheck( mbk_signlofig(ptmbkfig), 0, 0 );

#ifdef AVT_EVAL
    if (yagCountChains((chain_list *)ptmbkfig->LOTRS) > 3000) {
        printf("\n\n");
        printf("****************************************************\n");
        printf("*  Evaluation Version limited to 3000 Transistors  *\n");
        printf("****************************************************\n");
        EXIT(0);
    }
#endif

    loglvl1 = avt_islog(1,LOGYAG);
    if (YAG_CONTEXT == NULL) yagBug(DBG_NO_CONTEXT,"yagDisassemble",NULL,NULL,0);    
    if (YAG_CONTEXT->YAG_FIGNAME == NULL) YAG_CONTEXT->YAG_FIGNAME = ptmbkfig->NAME;
    if (figname != NULL) YAG_CONTEXT->YAG_OUTNAME = figname;
    if (YAG_CONTEXT->YAG_OUTNAME == NULL) YAG_CONTEXT->YAG_OUTNAME = YAG_CONTEXT->YAG_FIGNAME;
    if ((mode & YAG_GENBEFIG) != 0) YAG_CONTEXT->YAG_BEFIG = TRUE;
    else YAG_CONTEXT->YAG_BEFIG = FALSE;
#ifdef AVERTEC_LICENSE
    if (YAG_CONTEXT->YAG_BEFIG) {
        if(avt_givetoken("YAGLE_LICENSE_SERVER", "yagle")!=AVT_VALID_TOKEN) EXIT(1);
    }
#endif

    ifl=getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);
    
    if (ptmbkfig->LOTRS == NULL && ptmbkfig->LOINS == NULL) {
        avt_errmsg(YAG_ERRMSG, "010", AVT_WARNING, ptmbkfig->NAME);
        YAG_CONTEXT->YAG_CURCNSFIG = addcnsfig(CNS_HEADCNSFIG, YAG_CONTEXT->YAG_OUTNAME, ptmbkfig->LOCON, NULL, ptmbkfig->LOTRS, ptmbkfig->LOINS, NULL, NULL, NULL, NULL);
        return YAG_CONTEXT->YAG_CURCNSFIG;
    }
    YAG_CONTEXT->YAG_CURLOFIG = ptmbkfig;

#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif


/*------------------------*
|  Open .stat file        |
*-------------------------*/

    if (YAG_CONTEXT->YAG_STAT_MODE) {
        YAG_CONTEXT->YAGLE_STAT_FILE=mbkfopen(YAG_CONTEXT->YAG_OUTNAME,"stat",WRITE_TEXT);
        if(YAG_CONTEXT->YAGLE_STAT_FILE == NULL) {
            char buff[YAGBUFSIZE];
            sprintf(buff,"%s.stat",YAG_CONTEXT->YAG_OUTNAME);
            avt_errmsg(YAG_ERRMSG, "006", AVT_FATAL, buff);
        }
        sprintf(buffer,"Statistic file : %s.stat\n",YAG_CONTEXT->YAG_OUTNAME);
        avt_printExecInfo( YAG_CONTEXT->YAGLE_STAT_FILE, "#", buffer, "");
    }

/*------------------------*
|  Open .rep file         |
*-------------------------*/

    if (YAG_CONTEXT->YAGLE_ERR_FILE == NULL) {
        YAG_CONTEXT->YAGLE_ERR_FILE=mbkfopen(YAG_CONTEXT->YAG_OUTNAME,"rep",WRITE_TEXT);
        if(YAG_CONTEXT->YAGLE_ERR_FILE == NULL) {
            char buff[YAGBUFSIZE];
            sprintf(buff,"%s.rep",YAG_CONTEXT->YAG_OUTNAME);
            avt_errmsg(YAG_ERRMSG, "006", AVT_FATAL, buff);
        }
        
        sprintf(buffer,"Report file : %s.rep\n",YAG_CONTEXT->YAG_OUTNAME);
        avt_printExecInfo( YAG_CONTEXT->YAGLE_ERR_FILE, "#", buffer, "");

        close_err_file = TRUE;
    }

/*---------------------------------------------------------------------------*
| Report lofig statistics                                                    |
*----------------------------------------------------------------------------*/

    if( YAG_CONTEXT->YAG_STAT_MODE == TRUE ) {
        yagStatLofig(ptmbkfig) ;
    }

/*---------------------------------------------------------------------------*
| Configuration check level II                                               |
*----------------------------------------------------------------------------*/
    
    if (1 /*TODO*/) {
        chain_list *data, *lst;

        data = NULL;

        for (ptsig = ptmbkfig->LOSIG; ptsig; ptsig = ptsig->NEXT)
        {
          lst=dupchainlst(ptsig->NAMECHAIN);
          data = append(lst, data);
        }
        if (ifl) {
            inf_CheckRegistry(stdout,ifl,2,data);
        }

        freechain(data);
    }

    
/*---------------------------------------------------------------------------*
| Mark power supply signals                                                  |
*----------------------------------------------------------------------------*/

    yagFindSupplies(ifl, ptmbkfig, FALSE);
    yagFindInternalSupplies(ifl, ptmbkfig, FALSE);
    if(sim_SetPowerSupply (ptmbkfig)) stm_init();
    mbk_CheckPowerSupplies(ptmbkfig);

/*--------------------------------------------------------------------------*
| Clean up the signal & connector names                                     |
+---------------------------------------------------------------------------*/

    yagAddVal_s(ifl, ptmbkfig);

    for (ptsig = ptmbkfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
        chain_list *savenames;
        if ( loglvl1 ) {
            ptsig->USER = addptype(ptsig->USER, YAG_CHECK_PTYPE, NULL);
        }
        name = yagGetName(ifl, ptsig);
        savenames = ptsig->NAMECHAIN;
        ptsig->NAMECHAIN = addchain(NULL, name);
        for (ptchain = savenames; ptchain; ptchain = ptchain->NEXT) {
            if (ptchain->DATA != name) ptsig->NAMECHAIN = addchain(ptsig->NAMECHAIN, ptchain->DATA);
        }
        ptsig->NAMECHAIN = reverse(ptsig->NAMECHAIN);
        freechain(savenames);
    }

    if ( loglvl1 ) {
        for (pttrans = ptmbkfig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
            if (getptype(pttrans->SOURCE->SIG->USER, YAG_CHECK_PTYPE) == NULL) {
                fprintf(stderr, "Incoherent netlist at signal '%s'\n", (char *)pttrans->SOURCE->SIG->NAMECHAIN->DATA);
                yagExit(1);
            }
            if (getptype(pttrans->DRAIN->SIG->USER, YAG_CHECK_PTYPE) == NULL) {
                fprintf(stderr, "Incoherent netlist at signal '%s'\n", (char *)pttrans->DRAIN->SIG->NAMECHAIN->DATA);
                yagExit(1);
            }
            if (getptype(pttrans->GRID->SIG->USER, YAG_CHECK_PTYPE) == NULL) {
                fprintf(stderr, "Incoherent netlist at signal '%s'\n", (char *)pttrans->GRID->SIG->NAMECHAIN->DATA);
                yagExit(1);
            }
        }
    }
        
/*----------------------------------------------------------------------*
| Chain connector list to each signal                                   |
*-----------------------------------------------------------------------*/

    time(&start);
    lofigchain(ptmbkfig);
    time(&end);
    end -= start;
    fflush(stdout);

/*---------------------------------------------------------------------------*
| Mark the transparences                                                     |
*----------------------------------------------------------------------------*/

    yagTestTransparence(ptmbkfig);
    
/*---------------------------------------------------------------------------*
| Mark Blackbox transistors as unused                                        |
*----------------------------------------------------------------------------*/

    yagMarkUnusedTrans(ptmbkfig);

/*---------------------------------------------------------------------------*
| Test des transistors                                                       |
*----------------------------------------------------------------------------*/

    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_TRANS_CHECKING,NULL);
    yagTestTransistors(ptmbkfig, FALSE);
    yagChrono(&END,&end);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);

/*---------------------------------------------------------------------------*
| Transfer INF_MARKSIG & INF_MARKTRANS                                       |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_MARKTRANS, INF_ANY_VALUES);
    if (list0 != NULL) {
        yagInfMarkTrans(ifl, ptmbkfig, list0);
    }
    freechain(list0);
    list0=inf_GetEntriesByType(ifl, INF_MARKSIG, INF_ANY_VALUES);
    if (list0 != NULL) {
        yagInfMarkSig(ifl, ptmbkfig, list0);
    }
    freechain(list0);

/*---------------------------------------------------------------------------*
| Hierarchical pattern recognition                                           |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_GENIUS || YAG_CONTEXT->YAG_ONLY_GENIUS) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_GENIUS,NULL);
        recognised = genius_main(ptmbkfig, NULL, NULL, YAG_CONTEXT->YAG_OUTNAME);
        
        if (recognised != NULL) {

            if (YAG_CONTEXT->YAG_ONLY_GENIUS) ptmbkfig = yagCutLofig(ifl, ptmbkfig, recognised, &ptrootlofig, FALSE);
            else fclMarkInstances(ptmbkfig, recognised, FALSE, FALSE);
            YAG_CONTEXT->YAG_CURLOFIG = ptmbkfig;
            for (ptchain = recognised; ptchain; ptchain = ptchain->NEXT) {
                ptins = (loins_list *)ptchain->DATA;
                fclCleanTransList(ptins);
                if ((ptbefig = beg_get_befigByName(ptins->INSNAME)) != NULL) {
                    YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS = addchain(YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS, ptbefig);
                }
            }
        }

        /* recreate LOFIGCHAIN */
        lofigchain(ptmbkfig);

        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }
    
    if (YAG_CONTEXT->YAG_ONLY_GENIUS) {
        if (recognised) {
            lofig_list *ptrealrootfig;
            char buf[YAGBUFSIZE];

            sprintf(buf, "%s_yagroot", YAG_CONTEXT->YAG_FIGNAME);
            ptrealrootfig = yagBuildGeniusBehHierarchy(ptrootlofig, ptmbkfig, recognised, namealloc(buf));
            savelofig(ptrealrootfig);
    
            /* delete GENIUS instances */
            for (ptchain = recognised; ptchain; ptchain = ptchain->NEXT) {
                fclDeleteInstance((loins_list *)ptchain->DATA);
            }
            freechain(recognised);

            /* remove power supply markings and save core figure */
            for (ptcon = ptmbkfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
                ptsig = ptcon->SIG;
                if (ptsig->TYPE != 'E') {
                    ptsig->TYPE = 'E';
                    ptcon->DIRECTION = 'I';
                }
            }
            for (ptsig = ptmbkfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
                if (ptsig->TYPE != 'E' && ptsig->TYPE != 'I') {
                    ptsig->TYPE = 'I';
                }
            }
            savelofig(ptmbkfig);
        }
        if (YAG_CONTEXT->YAG_STAT_MODE) fclose(YAG_CONTEXT->YAGLE_STAT_FILE);
        fclose(YAG_CONTEXT->YAGLE_ERR_FILE);
        return NULL;
    }

/*---------------------------------------------------------------------------*
| Memory allocation and data structure initialisation                        |
*----------------------------------------------------------------------------*/

    ptcnsfig = addcnsfig(CNS_HEADCNSFIG, YAG_CONTEXT->YAG_OUTNAME, ptmbkfig->LOCON, NULL, ptmbkfig->LOTRS, ptmbkfig->LOINS, NULL, NULL, ptmbkfig, NULL);
    YAG_CONTEXT->YAG_CURCNSFIG = ptcnsfig;

/*---------------------------------------------------------------------------*
| Match cells from transistor net-list library                               |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_FCL_DETECT) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_MAKING_CELLS,NULL);
        ptcnsfig->CELL = fclFindCells(ptmbkfig);
        YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS = append(YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS, FCL_INSTANCE_BEFIGS);
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }
    if (YAG_CONTEXT->YAG_ONLY_FCL) {
        if (YAG_CONTEXT->YAG_STAT_MODE) fclose(YAG_CONTEXT->YAGLE_STAT_FILE);
        fclose(YAG_CONTEXT->YAGLE_ERR_FILE);
        return NULL;
    }

/*---------------------------------------------------------------------------*
| Check switches after FCL                                                   |
*----------------------------------------------------------------------------*/
    for (pttrans = ptmbkfig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
        fcltype = 0; otherfcltype = 0; 
        if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
            ptothertrans = (lotrs_list *)ptuser->DATA;
            if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                fcltype = (long)ptuser->DATA;
            }
            if ((ptuser = getptype(ptothertrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                otherfcltype = (long)ptuser->DATA;
            }
            if (fcltype != otherfcltype) {
                pttrans->USER = delptype(pttrans->USER, CNS_SWITCH);
                ptothertrans->USER = delptype(ptothertrans->USER, CNS_SWITCH);
            }
        }
    }
            
/*---------------------------------------------------------------------------*
| Restore parallel transistors                                               |
*----------------------------------------------------------------------------*/
    yagTransferParallel(ptmbkfig);
    mbk_restoreparallel(ptmbkfig);
    mbk_markparallel(ptmbkfig);
    ptcnsfig->LOTRS = ptmbkfig->LOTRS;

    ifl=getloadedinffig(YAG_CONTEXT->YAG_FIGNAME);

/*---------------------------------------------------------------------------*
| Create internal instance connectors                                        |
*----------------------------------------------------------------------------*/

    if ((ptuser = getptype(ptmbkfig->USER, FCL_LOCON_PTYPE)) != NULL) {
        ptcnsfig->INTCON = ptuser->DATA;
    }
    ptheadintcon = NULL;
    for (ptins = ptmbkfig->LOINS; ptins; ptins = ptins->NEXT) {
        /* check for NEVER directive on instance */
        if ((ptuser = getptype(ptins->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
        }
        if ((ptbefig = beg_get_befigByName(ptins->INSNAME)) != NULL) {
           YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS = addchain(YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS, ptbefig);
        }
        for (ptcon = ptins->LOCON; ptcon; ptcon = ptcon->NEXT) {
            ptsig = ptcon->SIG;
            if (ptsig->TYPE == CNS_SIGINT) {
                ptsig->TYPE = CNS_SIGEXT;
                ptintcon = (locon_list *)mbkalloc(sizeof(locon_list));
                ptintcon->NAME      = (char *)ptsig->NAMECHAIN->DATA;
                ptintcon->SIG       = ptsig;
                ptintcon->ROOT      = NULL;
                ptintcon->DIRECTION = 'X';
                ptintcon->TYPE      = EXTERNAL;
                ptintcon->USER      = NULL;
                ptintcon->NEXT      = ptheadintcon;
                ptintcon->PNODE     = NULL;
                ptheadintcon = ptintcon;
            }
        }
    }
    for (ptcon = ptheadintcon; ptcon; ptcon = ptcon->NEXT) {
        ptcon->SIG->TYPE = 'I';
    }
    if (ptheadintcon != NULL) {
        ptcnsfig->INTCON = (locon_list *)append((chain_list *)ptcnsfig->INTCON, (chain_list *)ptheadintcon);
    }

/*---------------------------------------------------------------------------*
| Additional CNS initialisation                                              |
*----------------------------------------------------------------------------*/

    ptcnsfig->LOCON = ptmbkfig->LOCON;
    ptcnsfig->LOTRS = ptmbkfig->LOTRS;
    ptcnsfig->LOINS = ptmbkfig->LOINS;

    for (ptcon=ptcnsfig->LOCON;ptcon != NULL;ptcon=ptcon->NEXT) {
//        delloconuser( ptcon );
        ptcon->ROOT = (void *)ptcnsfig;
//        ptcon->USER = NULL;
    }
    if (ptcnsfig->INTCON != NULL) {
        locon_list *ptprevcon = NULL;
        locon_list *ptnextcon = NULL;

        for (ptcon = ptcnsfig->INTCON; ptcon; ptcon = ptnextcon) {
            ptnextcon = ptcon->NEXT;
            ptcon->ROOT = (void *)ptcnsfig;
            if (ptcon->SIG->TYPE == 'I') {
                ptcon->SIG->TYPE = 'E';
                ptuser = getptype(ptcon->SIG->USER, LOFIGCHAIN);
                if (ptuser != NULL) {
                    ptuser->DATA = addchain(ptuser->DATA, ptcon);
                }
                ptprevcon = ptcon;
            }
            else {
                if (ptprevcon == NULL) ptcnsfig->INTCON = ptnextcon;
                else ptprevcon->NEXT = ptnextcon;
                delloconuser(ptcon);
                mbkfree(ptcon);
            }
        }
    }

/*---------------------------------------------------------------------------*
| Mark INF_INPUT connectors                                                  |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_INPUTS, INF_ANY_VALUES);
    if (list0 != NULL) {
        for (ptcon = ptcnsfig->LOCON; ptcon != NULL; ptcon = ptcon->NEXT) {
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_TestREGEX(ptcon->NAME, (char *)ptchain->DATA) || mbk_LosigTestREGEX(ptcon->SIG, (char *)ptchain->DATA)) {
                    ptcon->USER = addptype(ptcon->USER, YAG_INPUT_PTYPE, NULL);
                }
            }
        }
        YAG_CONTEXT->YAG_FLAGS|=YAG_HAS_INF_INPUTS;
    }
    freechain(list0);

/*---------------------------------------------------------------------------*
| Report oriented signals                                                    |
*----------------------------------------------------------------------------*/

    for (ptsig = ptmbkfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
        if ((value = yagGetVal_s(ptsig)) >= 0) {
            yagWarning(WAR_VALUE_S, NULL, (char *)ptsig->NAMECHAIN->DATA, NULL, (int)value);
        }
    }

/*---------------------------------------------------------------------------*
| Initialisation of Bdd toolbox                                              |
*----------------------------------------------------------------------------*/

    initializeBdd(0);

/*---------------------------------------------------------------------------*
| Fabrication of CMOS dual cones                                             |
*----------------------------------------------------------------------------*/

    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_DUAL_EXTRACT,NULL);
    ptcnsfig->CONE = yagMakeConeList(ifl, ptmbkfig, ptcnsfig);
    yagChrono(&END,&end);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);

    if ( loglvl1 ) yagCountDualTrans(ptcnsfig);

/*---------------------------------------------------------------------------*
| Configuration check level III                                              |
*----------------------------------------------------------------------------*/
    
    if (1 /*TODO*/) {
        chain_list *data;
        ptype_list *pt;

        data = NULL;

        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT)
        {
           pt=getptype(ptcone->USER, CNS_SIGNAL);
           if (pt!=NULL)
             data=append(dupchainlst(((losig_list *)pt->DATA)->NAMECHAIN), data);
           else
             data = addchain(data,ptcone->NAME);
        }
        if (ifl) {
            inf_CheckRegistry(stdout,ifl,3,data);
        }

        freechain(data);
    }

/*---------------------------------------------------------------------------*
| Mark INF_STOP cones                                                        |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_STOP, INF_ANY_VALUES);
    if (list0!=NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    ptcone->TYPE |= YAG_STOP;
                }
            }
        }
    }
    freechain(list0);

/*---------------------------------------------------------------------------*
| Mark INF_SENSITIVE cones                                                   |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_SENSITIVE, INF_ANY_VALUES);
    if (list0 != NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    ptcone->TECTYPE |= YAG_SENSITIVE;
                }
            }
        }
    }
    freechain(list0);

/*---------------------------------------------------------------------------*
| Mark INF_DLATCH cones                                                        |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_DLATCH, INF_YES);
    list1=inf_GetEntriesByType(ifl, INF_DLATCH, INF_NO);
    if (list0 != NULL || list1 != NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | YAG_INFODLATCH);
                    }
                    else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)YAG_INFODLATCH);
                }
            }
            for (ptchain = list1; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | YAG_INFONOTDLATCH);
                    }
                    else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)YAG_INFONOTDLATCH);
                }
            }
        }
    }
    freechain(list0);
    freechain(list1);

/*---------------------------------------------------------------------------*
| Mark INF_PRECHARGE cones                                                    |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_PRECHARGE, INF_YES);
    list1=inf_GetEntriesByType(ifl, INF_PRECHARGE, INF_NO);
    if (list0 != NULL || list1 != NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    ptcone->TYPE |= CNS_PRECHARGE;
                    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | YAG_INFOPRECHARGE);
                    }
                    else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)YAG_INFOPRECHARGE);
                }
            }
            for (ptchain = list1; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | YAG_INFONOTPRECHARGE);
                    }
                    else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)YAG_INFONOTPRECHARGE);
                }
            }
        }
    }
    freechain(list0);
    freechain(list1);

/*---------------------------------------------------------------------------*
| Mark INF_MARKRS cones                                                    |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_MARKRS, INF_ANY_VALUES);
    if (list0 != NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if ((name = mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) != NULL) {
                    inf_GetInt (ifl, name, INF_MARKRS, &infval);
                    switch (infval) {
                        case INF_RS_ILLEGAL: type = YAG_INFORSILLEGAL; break;
                        case INF_RS_LEGAL: type = YAG_INFORSLEGAL; break;
                        case INF_RS_MARKONLY: type = YAG_INFORSMARK; break;
                        default: type = 0;
                    }
                    if (type != 0) {
                        if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                            ptuser->DATA = (void *)((long)ptuser->DATA | type);
                        }
                        else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)type);
                    }
                }
            }
        }
    }
    freechain(list0);

/*---------------------------------------------------------------------------*
| Mark INF_MEMSYM cones                                                   |
*----------------------------------------------------------------------------*/

    if (inf_GetPointer(ifl, INF_MEMSYM, "", (void **)&list0)) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, ((inf_assoc *)ptchain->DATA)->orig)) {
                    ptcone->TYPE |= CNS_MEMSYM;
                }
                if (mbk_LosigTestREGEX(ptsig, ((inf_assoc *)ptchain->DATA)->dest)) {
                    ptcone->TYPE |= CNS_MEMSYM;
                }
            }
        }
    }

/*---------------------------------------------------------------------------*
| Mark INF_MODELLOOP cones                                                    |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_MODELLOOP, INF_YES);
    list1=inf_GetEntriesByType(ifl, INF_MODELLOOP, INF_NO);
    if (list0 != NULL || list1 != NULL) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain = list0; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | YAG_INFOMODELLOOP);
                    }
                    else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)YAG_INFOMODELLOOP);
                }
            }
            for (ptchain = list1; ptchain; ptchain = ptchain->NEXT) {
                if (mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcone->USER, YAG_INFO_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | YAG_INFONOTMODELLOOP);
                    }
                    else ptcone->USER = addptype(ptcone->USER, YAG_INFO_PTYPE, (void *)YAG_INFONOTMODELLOOP);
                }
            }
        }
    }
    freechain(list0);
    freechain(list1);

/*---------------------------------------------------------------------------*
| Chain the transistor gates to the driving cone                              |
*----------------------------------------------------------------------------*/

    for (pttrans = ptcnsfig->LOTRS; pttrans != NULL; pttrans = pttrans->NEXT) {
        yagChainTrans(pttrans);
    }

/*---------------------------------------------------------------------------*
| Reset the BDD toolbox                                                      |
*----------------------------------------------------------------------------*/

    yagControlBdd(1);

/*---------------------------------------------------------------------------*
| Chaining of CMOS dual cones                                                |
*----------------------------------------------------------------------------*/

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if ((ptcone->TECTYPE & CNS_DUAL_CMOS) != 0) {
            yagChainDual(ptcone);
            if (yagDetectTransfer(ptcone)) {
                yagChainCone(ptcone);
                ptcone->TYPE &= ~YAG_HASDUAL;
                cnsConeFunction(ptcone, FALSE);
            }
        }
    }

/*---------------------------------------------------------------------------*
| Mark inversion substitution for switches                                   |
*----------------------------------------------------------------------------*/

    yagAddSwitchInversion(ptcnsfig);

/*---------------------------------------------------------------------------*
| Bleeder Detection                                                          |
*----------------------------------------------------------------------------*/

    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_BLEED_EXTRACT,NULL);
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TECTYPE & CNS_DUAL_CMOS) != 0) {
            count = yagCountActiveEdges(ptcone->INCONE);
            if (count == 1 || (count == 2 && YAG_CONTEXT->YAG_BLEEDER_STRICTNESS < 2)) {
                yagExtractBleeder(ifl, ptcone);
            }
        }
    }
    yagChrono(&END,&end);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);

/*---------------------------------------------------------------------------*
| Chaining of temporary cones                                                |
*----------------------------------------------------------------------------*/
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat (NULL, 1);
#endif

    ptuser = getptype(ptcnsfig->USER, YAG_TEMPCONE_PTYPE);
    if (ptuser != NULL) {
        for (ptcone = (cone_list *)ptuser->DATA; ptcone != NULL; ptcone = ptcone->NEXT) {
            yagChainDual(ptcone);
        }
    }
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("chaindual:", 0);
#endif
    
/*---------------------------------------------------------------------------*
| Parallel branch and transistor detection on CMOS dual cones                |
*----------------------------------------------------------------------------*/

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if ((ptcone->TECTYPE & CNS_DUAL_CMOS) != 0) {
            yagDetectParallelBranches(ptcone);
            yagDetectParaTrans(ptcone);
        }
    }

#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("detect // branches:", 0);
#endif
    
/*---------------------------------------------------------------------------*
| Mark INF_CONSTRAINT cones and connectors                                   |
*----------------------------------------------------------------------------*/

    list0=inf_GetEntriesByType(ifl, INF_STUCK, (char *)(long)0);
    list1=inf_GetEntriesByType(ifl, INF_STUCK, (char *)(long)1);
    if (list0 != NULL || list1!=NULL) {
        for (ptcon = ptcnsfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
            for (ptchain=list0; ptchain!=NULL; ptchain=ptchain->NEXT) {
                if (mbk_TestREGEX(ptcon->NAME, (char *)ptchain->DATA) || mbk_LosigTestREGEX(ptcon->SIG, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcon->USER, CNS_TYPELOCON)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | CNS_ZERO);
                    }
                    else ptcon->USER = addptype(ptcon->USER, CNS_TYPELOCON, (void *)CNS_ZERO);
                }
            }
            for (ptchain=list1; ptchain!=NULL; ptchain=ptchain->NEXT) {
                if (mbk_TestREGEX(ptcon->NAME, (char *)ptchain->DATA) || mbk_LosigTestREGEX(ptcon->SIG, (char *)ptchain->DATA)) {
                    if ((ptuser = getptype(ptcon->USER, CNS_TYPELOCON)) != NULL) {
                        ptuser->DATA = (void *)((long)ptuser->DATA | CNS_ONE);
                    }
                    else ptcon->USER = addptype(ptcon->USER, CNS_TYPELOCON, (void *)CNS_ONE);
                }
            }
        }
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
            for (ptchain=list0; ptchain!=NULL; ptchain=ptchain->NEXT) {
                if (mbk_TestREGEX(ptcone->NAME, (char *)ptchain->DATA) || mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    ptcone->TECTYPE |= CNS_ZERO;
                    ptcone->TYPE |= YAG_FORCEPRIM;
                    ptcone->TYPE &= ~YAG_PARTIAL;
                }
            }
            for (ptchain=list1; ptchain!=NULL; ptchain=ptchain->NEXT) {
                if (mbk_TestREGEX(ptcone->NAME, (char *)ptchain->DATA) || mbk_LosigTestREGEX(ptsig, (char *)ptchain->DATA)) {
                    ptcone->TECTYPE |= CNS_ONE;
                    ptcone->TYPE |= YAG_FORCEPRIM;
                    ptcone->TYPE &= ~YAG_PARTIAL;
                }
            }
        }
    }
    freechain(list0);
    freechain(list1);

#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("stuck:", 0);
#endif

/*---------------------------------------------------------------------------*
| Propagate stuck-at values                                                  |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagBuildOutputs(ptcone);
    }
    changes = FALSE;
    do {
        changes = FALSE;
        for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TECTYPE & (CNS_ZERO|CNS_ONE)) != 0) {
                if (yagPropagateStuck(ptcone)) changes = TRUE;
            }
        }
    } while (changes);

#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("propstuck:", 0);
#endif

/*---------------------------------------------------------------------------*
| Building of global graph                                                   |
*----------------------------------------------------------------------------*/

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & YAG_PARTIAL) != 0) {
            numpartial++;
            ptinputs = yagGetConeInputs(ifl, ptcone);
            ptcone->USER = addptype(ptcone->USER, YAG_INPUTS_PTYPE, ptinputs);
        }
    }

    ptuser = getptype(ptcnsfig->USER, YAG_GLUECONE_PTYPE);
    if (ptuser != NULL) {
        for (ptcone = (cone_list *)ptuser->DATA; ptcone != NULL; ptcone = ptcone->NEXT) {
            ptinputs = yagGetConeInputs(ifl, ptcone);
            ptcone->USER = addptype(ptcone->USER, YAG_INPUTS_PTYPE, ptinputs);
        }
    }
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("global graph:", 0);
#endif

/*---------------------------------------------------------------------------*
| Detect two cone loops in global graph                                      |
*----------------------------------------------------------------------------*/

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagMarkLoopConf(ptcone);
    }

#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("detect loop:", 0);
#endif

/*---------------------------------------------------------------------------*
| Contraint initialisation                                                   |
*----------------------------------------------------------------------------*/

    yagInitConstraints(ifl);

/*---------------------------------------------------------------------------*
| Mark multi-voltage                                                         |
*----------------------------------------------------------------------------*/

#ifndef WITHOUT_TAS
    if (YAG_CONTEXT->YAG_USESTMSOLVER) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            yagPrepStmSolver(ptcone);
        }
    }
#endif
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("init constraint + solver:", 0);
#endif

/*---------------------------------------------------------------------------*
| Glitcher detection in CMOS Duals                                           |
*----------------------------------------------------------------------------*/

    if (V_INT_TAB[__YAGLE_KEEP_GLITCHERS].VALUE == 2) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            yagDetectDualGlitcherBranches(ptcone);
        }
    }

/*---------------------------------------------------------------------------*
| Building the cones                                                         |
*----------------------------------------------------------------------------*/

    YAG_CONTEXT->YAG_REQUIRE_COMPLETE_GRAPH = TRUE;
    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_MAKING_GATES, NULL);
    YAG_CONTEXT->YAG_ITERATIONS = 0;
    conesleft = numpartial;
    while (conesleft != 0) {
        previous = conesleft;
        conesleft = yagScanCones(ifl, ptcnsfig->CONE);
        if (previous == conesleft) {
            YAG_CONTEXT->YAG_MAX_LINKS = YAG_CONTEXT->YAG_RELAX_LINKS;
            if (YAG_CONTEXT->YAG_RELAX_ALGO) {
                YAG_CONTEXT->YAG_USE_FCF = FALSE;
            }
            else YAG_CONTEXT->YAG_REQUIRE_COMPLETE_GRAPH = FALSE;
            relax = TRUE;
        }
    }
    yagChrono(&END,&end);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);

/*---------------------------------------------------------------------------*
| Delete cells containing no cones and no befig                              |
*----------------------------------------------------------------------------*/

    for (ptcell = ptcnsfig->CELL; ptcell; ptcell = ptnextcell) {
        ptnextcell = ptcell->NEXT;
        if (ptcell->CONES == NULL && ptcell->BEFIG == NULL) {
            ptcnsfig->CELL = delcell(ptcnsfig->CELL, ptcell);
        }
    }

/*---------------------------------------------------------------------------*
| Final pre-relaxation loop analysis and standard latch detection            |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_LOOP_ANALYSIS || YAG_CONTEXT->YAG_DETECT_LATCHES || YAG_CONTEXT->YAG_SIMPLE_LATCH) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_LATCH_DETECTION, NULL);
    }

    if (YAG_CONTEXT->YAG_SIMPLE_LATCH) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            yagMatchSimpleLatch(ptcone);
        }
    }
    
    if (YAG_CONTEXT->YAG_LOOP_ANALYSIS) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if (yagDetectFalseConf(ptcone)) yagChainCone(ptcone);
            yagAnalyseLoop(ptcone, TRUE);
        }
        do { 
            changes = FALSE;
            for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
                if (yagDetectFalseConf(ptcone)) ptcone->TYPE |= YAG_MARK;
                yagRmvThruLatch(ptcone);
                yagRmvThruBleed(ptcone);
                if ((ptcone->TYPE & YAG_MARK) != 0) {
                    changes = TRUE;
                    if ((ptcone->TYPE & YAG_AUTOLATCH) != 0) {
                        yagUnmarkLatch(ptcone, FALSE, FALSE);
                    }
                    yagChainCone(ptcone);
                    yagAnalyseCone(ptcone);
                    ptcone->TYPE &= ~YAG_MARK;
                }
            }
        } while (changes);
    }

    if (YAG_CONTEXT->YAG_DETECT_LATCHES) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_RS|CNS_PRECHARGE)) == 0
              && ptcone->CELLS == NULL && (ptcone->TECTYPE & YAG_NOTLATCH) == 0) {
                yagMatchLatch(ptcone);
            }
        }
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TYPE & YAG_FALSECONF) != 0) {
                yagRmvThruLatch(ptcone);
                yagChainCone(ptcone);
            }
            if ((ptcone->TYPE & YAG_LOOPCONF) != 0) {
                yagDetectLoopConf(ptcone);
            }
        }
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptnextcone) {
            ptnextcone = ptcone->NEXT;
            if ((ptcone->TYPE & YAG_MARK) == YAG_MARK) {
                yagAnalyseCone(ptcone);
                ptcone->TYPE &= ~YAG_MARK;
            }
        }
    }

    if (YAG_CONTEXT->YAG_LOOP_ANALYSIS || YAG_CONTEXT->YAG_DETECT_LATCHES) {
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }

/*---------------------------------------------------------------------------*
| Bleeder Detection                                                          |
*----------------------------------------------------------------------------*/
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_RS|CNS_MASTER|CNS_SLAVE)) != 0) continue;
        yagDetectBleeder(ptcone);
    }
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & YAG_FALSECONF) != 0) {
            yagRmvThruBleed(ptcone);
        }
        if ((ptcone->TYPE & YAG_LOOPCONF) != 0) {
            yagDetectLoopConf(ptcone);
        }
        if ((ptcone->TYPE & YAG_MARK) != 0) {
            yagChainCone(ptcone);
            yagAnalyseCone(ptcone);
            ptcone->TYPE &= ~YAG_MARK;
            relax = TRUE;
        }
    }

/*---------------------------------------------------------------------------*
| Select a latch in symmetric registers                                      |
*----------------------------------------------------------------------------*/

    /* pair off CNS_MEMSYM cones */
    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & CNS_MEMSYM) != 0) {
            if (getptype(ptcone->USER, YAG_MEMORY_PTYPE) == NULL) {
                yagPairMemsym(ifl, ptcone);
            }
            if (getptype(ptcone->USER, YAG_MEMORY_PTYPE) == NULL) {
                ptcone->TYPE &= ~CNS_MEMSYM;
            }
        }
    }

/*---------------------------------------------------------------------------*
| Update two-cone loop marks and re-verify if necessary                      |
*----------------------------------------------------------------------------*/

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagDetectLoopConf(ptcone);
    }
    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & YAG_FALSECONF) != 0) {
            yagDetectFalseConf(ptcone);
            if ((ptcone->TYPE & YAG_FALSECONF) == 0) {
                yagChainCone(ptcone);
                yagAnalyseCone(ptcone);
            }
        }
    }

/*---------------------------------------------------------------------------*
| Desb-like relaxation                                                       |
*----------------------------------------------------------------------------*/
    if (relax == TRUE) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_RELAX, NULL);
        YAG_CONTEXT->YAG_USE_FCF = TRUE;
        changes = TRUE;
        YAG_CONTEXT->YAG_ITERATIONS = 1;
        while (changes) {
            avt_log(LOGYAG,1, "Starting relaxation iteration %d\n\n", YAG_CONTEXT->YAG_ITERATIONS++);
            changes = yagDepthFirstProcess(ptcnsfig->CONE, yagRemoveFalseBranches);
        }
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }

/*---------------------------------------------------------------------------*
| Mark non-functional external branches                                      |
*----------------------------------------------------------------------------*/ 
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat (NULL, 1);
#endif

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if (ptcone->BREXT != NULL) yagRmvFalseBrext(ptcone);
    }
    
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("remove false branch:", 0);
#endif

/*---------------------------------------------------------------------------*
| Loop Analysis after relaxation                                             |
*----------------------------------------------------------------------------*/ 

    if (YAG_CONTEXT->YAG_LOOP_ANALYSIS) {
        int changed = FALSE;
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            count = yagCountLoops(ptcone, FALSE);
            if (count > 0) {
                yagAnalyseLoop(ptcone, FALSE);
                changed = TRUE;
                ptcone->TYPE |= YAG_MARK;
            }
        }
        if (changed) {
            for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
                if ((ptcone->TYPE & YAG_FALSECONF) != 0) {
                    yagRmvThruLatch(ptcone);
                    yagChainCone(ptcone);
                }
                if ((ptcone->TYPE & YAG_LOOPCONF) != 0) {
                    yagDetectLoopConf(ptcone);
                }
            }
            for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptnextcone) {
                ptnextcone = ptcone->NEXT;
                if ((ptcone->TYPE & YAG_MARK) == YAG_MARK) {
                    yagAnalyseCone(ptcone);
                    ptcone->TYPE &= ~YAG_MARK;
                }
            }
        }
    }
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("loop analysis:", 0);
#endif

/*---------------------------------------------------------------------------*
| Final check for any FALSECONF and DEGRADED                                 |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagDetectDegraded(ptcone);
        if ((ptcone->TYPE & YAG_FALSECONF) == YAG_FALSECONF) yagAnalyseCone(ptcone);
    }
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("false conf:", 0);
#endif

/*---------------------------------------------------------------------------*
| check for poor redundant branches (partly heuristic)                       |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_DETECT_REDUNDANT) {
        for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TECTYPE & CNS_DUAL_CMOS) != CNS_DUAL_CMOS) {
                yagDetectRedundant(ptcone);
                yagDetectDegraded(ptcone);
            }
        }
    }

/*---------------------------------------------------------------------------*
| Correction for switch transistor                                           |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagCorrectSwitch(ptcone);
    }
   
/*---------------------------------------------------------------------------*
| Chain cone outputs                                                         |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if (ptcone->OUTCONE) {
            yagFreeEdgeList(ptcone->OUTCONE);
            ptcone->OUTCONE = NULL;
        }
    }
    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagBuildOutputs(ptcone);
    }

/*---------------------------------------------------------------------------*
| Check possible input cones                                                 |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptnextcone) {
        ptnextcone = ptcone->NEXT;
        yagCheckExtOut(ptcone);
        yagCheckExtIn(ptcone);
    }

/*---------------------------------------------------------------------------*
| Remove empty cones unless in NOTSTRICT mode                                |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptnextcone) {
        ptnextcone = ptcone->NEXT;
        if ((ptcone->TYPE & CNS_VDD) == CNS_VDD) continue;
        if ((ptcone->TYPE & CNS_VSS) == CNS_VSS) continue;
        if ((ptcone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0 && ptcone->OUTCONE != NULL) continue;
        if (ptcone->OUTCONE != NULL && YAG_CONTEXT->YAG_NOTSTRICT) continue;
        if (yagCheckBranches(ptcone) == FALSE) {
            yagResetCone(ptcone);
            CNS_HEADCNSFIG->CONE = delcone(CNS_HEADCNSFIG->CONE, ptcone);
        }
    }

/*---------------------------------------------------------------------------*
| Select a latch in bistables (after building OUTCONE list )                 |
*----------------------------------------------------------------------------*/

    if ((YAG_CONTEXT->YAG_AUTO_RS & YAG_RS_LATCH) != 0) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TYPE &(CNS_RS|CNS_LATCH)) == CNS_RS) {
                ptsymcone = (cone_list *)getptype(ptcone->USER, YAG_BISTABLE_PTYPE)->DATA;
                if ((ptsymcone->TYPE &(CNS_RS|CNS_LATCH)) == CNS_RS) {
                    yagSelectRSLatch(ptcone, ptsymcone);
                }
            }
        }
    }

/*---------------------------------------------------------------------------*
| Disable the appropriate timing arcs                                        |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_AUTO_RS) {
        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TYPE & (CNS_RS|YAG_MARK)) == CNS_RS) {
                ptsymcone = (cone_list *)getptype(ptcone->USER, YAG_BISTABLE_PTYPE)->DATA;
                type = ptcone->TECTYPE & (CNS_NAND|CNS_NOR);
                if (type != 0) {
                    yagDisableRSArcs(ptcone, ptsymcone, ptcone, ptsymcone, type, (YAG_CONTEXT->YAG_AUTO_RS & (YAG_RS_LEGAL|YAG_RS_ILLEGAL)));
                }
                ptsymcone->TYPE |= YAG_MARK;
            }
            ptcone->TYPE &= ~YAG_MARK;
        }
    }

/*---------------------------------------------------------------------------*
| Loop detection                                                             |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagDetectLoops(ptcone);
    }

/*---------------------------------------------------------------------------*
| Build glue cones for befig                                                 |
*----------------------------------------------------------------------------*/

    if ((mode & YAG_GENBEFIG) != 0) {
        if ((ptuser = getptype(ptcnsfig->USER, YAG_GLUECONE_PTYPE)) != NULL) {
            for (ptcone = (cone_list *)ptuser->DATA; ptcone; ptcone = ptcone->NEXT) {
                yagExpandCone(ifl, ptcone);
            }
        }
    }

/*---------------------------------------------------------------------------*
| Cell matching                                                             |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_MAKE_CELLS || YAG_CONTEXT->YAG_AUTO_FLIPFLOP) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_MAKING_CELLS, NULL);
        build_befig = ((mode & (YAG_GENBEFIG|YAG_GENLOFIG)) != 0);
        if (YAG_CONTEXT->YAG_MAKE_CELLS) yagMatchFlipFlopCells(ptcnsfig->CONE, build_befig);
        if (YAG_CONTEXT->YAG_AUTO_FLIPFLOP) yagAutoFlipFlop(ptcnsfig->CONE, build_befig);
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }
#ifdef DELAY_DEBUG_STAT
    mbk_debugstat ("reste:", 0);
#endif

/*---------------------------------------------------------------------------*
| Delayed-RS                                                                 |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_DELAYED_RS) {
        yagDetectDelayedRS(ptcnsfig);
    }

/*---------------------------------------------------------------------------*
| External connector verification                                            |
*----------------------------------------------------------------------------*/

    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_VERIF_EXTCON, NULL);
    yagOrientInterface(ptcnsfig);
    yagChrono(&END,&end);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);

#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

/*---------------------------------------------------------------------------*
| Check transistor usage                                                     |
*----------------------------------------------------------------------------*/

    for (pttrans = ptmbkfig->LOTRS; pttrans != NULL; pttrans = pttrans->NEXT) {
        pttrans->TYPE &= ~USED;
    }
    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagMarkUsedTrans(ptcone);
    }

    for (pttrans = ptmbkfig->LOTRS; pttrans != NULL; pttrans = pttrans->NEXT) {
        if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            if (((long)ptuser->DATA & FCL_NEVER) != 0) continue;
        }
        if ((pttrans->TYPE & USED) == 0) {
            pttrans->USER = addptype(pttrans->USER, CNS_UNUSED, NULL);
            if ((pttrans->TYPE & CNS_TN) != 0) {
                yagWarning(WAR_UNUSED,NULL,"N",(char*)pttrans,0);
            }
            else {
                yagWarning(WAR_UNUSED,NULL,"P",(char*)pttrans,0);
            }
            num_unused++;
        }
        else {
            if (getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA == NULL) {
                ptsig = ((locon_list *)getptype(pttrans->USER, YAG_GRIDCON_PTYPE)->DATA)->SIG;
                yagError(ERR_GATE_UNDRIVEN, NULL, ptsig->NAMECHAIN->DATA, NULL, 0, 0);
                num_undrivengate++;
            }
        }
        if (getptype(pttrans->USER, YAG_BIDIR_PTYPE) != NULL) {
            yagWarning(WAR_BIDIR_BLOCKED, NULL, "N", (char*)pttrans, 0);
            num_bidir++;
        }
        pttrans->TYPE &= (TRANSN|TRANSP);
    }
    if (YAG_CONTEXT->YAG_STAT_MODE) {
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "------------------------------------------------------------\n");
        if (num_unused > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d unused transistors\n", num_unused);
        if (num_undrivengate > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d undriven transistor gates\n", num_undrivengate);
        if (num_bidir > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d potentially bidirectional transistors\n", num_bidir);
    }
            
/*---------------------------------------------------------------------------*
| Check figure                                                               |
*----------------------------------------------------------------------------*/

    yagChrono(&START,&start);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_FIG_CHECKING, NULL);

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        ptcone->INDEX = index++;
        stuck = FALSE;
        if ((ptcone->TYPE & (CNS_VDD|CNS_VSS)) != 0) continue;
        if ((ptcone->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS) num_cmosdual++;
        if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
            yagWarning(WAR_STUCK_ZERO, NULL, ptcone->NAME, NULL, 0);
            stuck = TRUE;
        }
        if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
            yagWarning(WAR_STUCK_ONE, NULL, ptcone->NAME, NULL, 0);
            stuck = TRUE;
        }
        if (!stuck) {
            count = yagCountLoops(ptcone, FALSE);
            if (count > 0 && YAG_CONTEXT->YAG_LOOP_ANALYSIS) {
                yagAnalyseLoop(ptcone, FALSE);
            }
        }
        if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0) {
            num_levelhold++;
            yagWarning(WAR_LEVELHOLD_LOOP, NULL, ptcone->NAME, NULL, 0);
        }
        else if (getptype(ptcone->USER, CNS_BLEEDER) != NULL) {
            num_bleeder++;
            yagWarning(WAR_BLEED_LOOP, NULL, ptcone->NAME, NULL, 0);
        }
        if ((ptcone->TECTYPE & YAG_BADCONE) != 0) {
            num_bad++;
            yagWarning(WAR_BADCONE, NULL, ptcone->NAME, NULL, 0);
        }
        if ((ptcone->TYPE & CNS_LATCH) != 0 && (ptcone->TYPE & CNS_RS) == 0) {
            long info = 0;
            ptuser = getptype(ptcone->USER, YAG_LATCHINFO_PTYPE);
            if (ptuser != NULL) info = (long)ptuser->DATA;
            if (info == DLATCH) num_dlatch++;
            else num_latch++;
            if (ptcone->CELLS != NULL) ptcell = (cell_list *)ptcone->CELLS->DATA;
            else ptcell = NULL;
            if ((ptcone->TYPE & CNS_MASTER) == CNS_MASTER) {
                yagWarning(WAR_MASTER_LATCH_LOOP, (char *)ptcell, ptcone->NAME, (char *)ptcone->INDEX, info);
            }
            else if ((ptcone->TYPE & CNS_SLAVE) == CNS_SLAVE) {
                yagWarning(WAR_SLAVE_LATCH_LOOP, (char *)ptcell, ptcone->NAME, (char *)ptcone->INDEX, info);
            }
            else yagWarning(WAR_LATCH_LOOP, (char *)ptcell, ptcone->NAME, (char *)ptcone->INDEX, info);
        }
        if ((ptcone->TYPE & (CNS_RS)) == (CNS_RS)) {
            if (getptype(ptcone->USER, YAG_MARK_PTYPE) != NULL) {
                ptcone->USER = delptype(ptcone->USER, YAG_MARK_PTYPE);
            }
            else {
                num_rs++;
                ptloopcone = (cone_list *)getptype(ptcone->USER, YAG_BISTABLE_PTYPE)->DATA;
                yagWarning(WAR_BISTABLE_LOOP, ptcone->NAME, ptloopcone->NAME, NULL, 0);
                ptloopcone->USER = addptype(ptloopcone->USER, YAG_MARK_PTYPE, 0);
            }
        }
        if ((ptcone->TYPE & CNS_FLIP_FLOP) != 0) {
            num_flipflop++;
            yagWarning(WAR_FLIP_FLOP_LOOP, (char *)ptcell, ptcone->NAME, (char *)ptcone->INDEX, 0);
        }
        if ((ptcone->TYPE & CNS_PRECHARGE) != 0) {
            num_precharge++;
            yagWarning(WAR_PRECHARGE, NULL, ptcone->NAME, (char *)ptcone->INDEX, 0);
        }
        if ((ptcone->TYPE & CNS_CONFLICT) != 0) {
            num_conf++;
            if (getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE) != NULL) {
                yagWarning(WAR_LATCH_CONFLICT, NULL, ptcone->NAME, NULL, 0);
            }
            else if (getptype(ptcone->USER, YAG_BUSBEFIG_PTYPE) != NULL) {
                yagWarning(WAR_BUS_CONFLICT, NULL, ptcone->NAME, NULL, 0);
            }
            else yagWarning(WAR_CONFLICT, NULL, ptcone->NAME, NULL, 0);
        }
        if ((ptcone->TYPE & CNS_TRI) != 0) {
            num_hz++;
            if (getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE) != NULL) {
                yagWarning(WAR_LATCH_HZ, NULL, ptcone->NAME, NULL, 0);
            }
            else yagWarning(WAR_HZ, NULL, ptcone->NAME, NULL, 0);
        }
        if (!stuck && ptcone->OUTCONE == NULL) {
            num_nodrive++;
            yagWarning(WAR_DOESNT_DRIVE, NULL, ptcone->NAME, NULL, 0);
        }
        if (ptcone->INCONE == NULL) {
            num_noinputs++;
            yagWarning(WAR_NO_INPUTS, NULL, ptcone->NAME, NULL, 0);
        }
    }
    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        num_loops += yagCountLoops(ptcone, TRUE);
    }
    yagChrono(&END,&end);
    if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    if (YAG_CONTEXT->YAG_STAT_MODE) {
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "------------------------------------------------------------\n");
        if (num_bad > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d cones contain long current paths (Check MUTEX constraints)\n", num_bad);
        if (num_bleeder > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d bleeder loops\n", num_bleeder);
        if (num_levelhold > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d level-hold loops\n", num_levelhold);
        if (num_latch > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d latches\n", num_latch);
        if (num_dlatch > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d dynamic latches\n", num_dlatch);
        if (num_precharge > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d precharged signals\n", num_precharge);
        if (num_rs > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d RS bistables\n", num_rs);
        if (num_flipflop > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d flip-flops\n", num_flipflop);
        if (num_loops > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d unknown loops\n", num_loops);
        if (num_noinputs > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d signals without drivers\n", num_noinputs);
        if (num_nodrive > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d signal which drive nothing\n", num_nodrive);
        if (num_conf > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d potentially conflictual signals\n", num_conf);
        if (num_hz > 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d potentially tristate signals\n", num_hz);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "------------------------------------------------------------\n");
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "CMOS DUAL cones : %ld\n", num_cmosdual);
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Total cones : %ld\n", index-1);
    }

/*---------------------------------------------------------------------------*
| Build the behavioural figure                                               |
*----------------------------------------------------------------------------*/

    if ((mode & YAG_GENBEFIG) != 0 && YAG_CONTEXT->YAGLE_NB_ERR == 0) {
        chain_list *modeled = NULL;
        chain_list *unmodeled = NULL;
        char       *savename;

        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_BUILD_BEFIG, NULL);
        yagSortModeledInstances(ptmbkfig, &modeled, &unmodeled);
        if (unmodeled != NULL) {
            ptcorelofig = yagBuildHierarchy(ptcnsfig, ptmbkfig, &ptrootlofig, unmodeled);
            savename = ptrootlofig->NAME;
            if (YAG_CONTEXT->YAG_OUTNAME != NULL) ptrootlofig->NAME = YAG_CONTEXT->YAG_OUTNAME;
            yagGuessRootConnectorDirections(ptrootlofig);
            savelofig(ptrootlofig);
            ptrootlofig->NAME = savename;
            yagBuildCoreBefig(ptcorelofig, ptcnsfig, modeled);
            dellofig(ptcorelofig->NAME);
        }
        else yagBuildBefig(ptcnsfig, YAG_CONTEXT->YAG_OUTNAME);
        freechain(modeled);
        freechain(unmodeled);
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }

/*---------------------------------------------------------------------------*
| Check connectors                                                           |
*----------------------------------------------------------------------------*/

    for (ptcon = ptcnsfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
        if (ptcon->DIRECTION == UNKNOWN) {
            numconunused++;
            yagWarning(WAR_CON_UNUSED, NULL, ptcon->NAME, NULL, 0);
        }
    }
    if (YAG_CONTEXT->YAG_STAT_MODE) {
        if (numconunused != 0) fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "%d unused connectors\n", numconunused);
    }
 
/*---------------------------------------------------------------------------*
| Build the logical figure                                                   |
*----------------------------------------------------------------------------*/

    if ((mode & YAG_GENLOFIG) != 0 && YAG_CONTEXT->YAGLE_NB_ERR == 0) {
        yagChrono(&START,&start);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagMessage(MES_BUILD_LOFIG, NULL);
        yagBuildLofig(ptcnsfig, ptmbkfig);
        yagChrono(&END,&end);
        if (!YAG_CONTEXT->YAG_SILENT_MODE) yagPrintTime(&START,&END,start,end);
    }

/*---------------------------------------------------------------------------*
| Add signature                                                              |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_GEN_SIGNATURE && YAG_CONTEXT->YAGLE_NB_ERR == 0) {
        for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if ((ptcone->TYPE & (CNS_LATCH|CNS_MASTER|CNS_SLAVE|CNS_MEMSYM)) != 0) continue;
            if ((mode & YAG_GENLOFIG) == 0) {
                yagMakeConeModel(ptcone);
            }
            ptuser = getptype(ptcone->USER, YAG_MODEL_PTYPE);
            if (ptuser != NULL) {
                if (ptuser->DATA != NULL && strlen(ptuser->DATA) > 0) {
                    ptcone->USER = addptype(ptcone->USER, CNS_SIGNATURE, ptuser->DATA);
                }
            }
        }
    }

/*---------------------------------------------------------------------------*
| Destruction of constraints                                                 |
*----------------------------------------------------------------------------*/

    yagDeleteConstraints();

#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

/*---------------------------------------------------------------------------*
|  Generate .loop file                                                       |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_LOOP_MODE) {
        chain_list *loops = yagCircuitLoops(ptcnsfig);

        if (loops != NULL) {
            YAG_CONTEXT->YAGLE_LOOP_FILE=mbkfopen(YAG_CONTEXT->YAG_OUTNAME,"loop",WRITE_TEXT);
            if(YAG_CONTEXT->YAGLE_LOOP_FILE == NULL) {
                char buff[YAGBUFSIZE];
                sprintf(buff,"%s.loop",YAG_CONTEXT->YAG_OUTNAME);
                avt_errmsg(YAG_ERRMSG, "006", AVT_FATAL, buff);
            }
            sprintf(buffer,"Loop file : %s.loop\n",YAG_CONTEXT->YAG_OUTNAME);
            avt_printExecInfo( YAG_CONTEXT->YAGLE_LOOP_FILE, "#", buffer, "");
            yagSaveCircuitLoops(loops);
            fclose(YAG_CONTEXT->YAGLE_LOOP_FILE);
        }
    }

/*---------------------------------------------------------------------------*
|  Generate .mutex file                                                       |
*----------------------------------------------------------------------------*/

    if (V_BOOL_TAB[__YAGLE_GUESS_MUTEX].VALUE) {
        FILE *fpmutex = mbkfopen(YAG_CONTEXT->YAG_OUTNAME,"mutex",WRITE_TEXT);
        chain_list *guess_mutex = yagGuessMutex(ptcnsfig);
        chain_list *ptmutexchain;
        int count = 0;

        if (guess_mutex != NULL) {
            fpmutex = mbkfopen(YAG_CONTEXT->YAG_OUTNAME,"mutex",WRITE_TEXT);
            if(fpmutex == NULL) {
                char buff[YAGBUFSIZE];
                sprintf(buff,"%s.mutex",YAG_CONTEXT->YAG_OUTNAME);
                avt_errmsg(YAG_ERRMSG, "006", AVT_FATAL, buff);
            }
            sprintf(buffer,"MUTEX Guess file : %s.mutex\n",YAG_CONTEXT->YAG_OUTNAME);
            avt_printExecInfo(fpmutex, "#", buffer, "");
            for (ptmutexchain = guess_mutex; ptmutexchain; ptmutexchain = ptmutexchain->NEXT) {
                count++;
                fprintf(fpmutex, "MUTEX Group %d:\n", count);
                for (ptchain = ptmutexchain->DATA; ptchain; ptchain = ptchain->NEXT) {
                    fprintf(fpmutex, "    %s\n", (char *)ptchain->DATA);
                }
                fprintf(fpmutex, "\n");
            }
            fclose(fpmutex);
        }
    }

/*---------------------------------------------------------------------------*
|  Unmark latches and precharges not on given clock paths                    |
*----------------------------------------------------------------------------*/

    yagDetectClockLatch(ifl, ptcnsfig);
    yagDetectNotClockLatch(ifl, ptcnsfig);
    yagDetectClockPrech(ifl, ptcnsfig);
    yagInfUnmarkLatches(ifl, ptcnsfig);
    if (YAG_CONTEXT->YAG_CLOCK_GATE) yagDetectClockGating(ifl, ptcnsfig);

    avt_log(LOGYAG,0,"------------------------------------------------------------\n");
    if (YAG_CONTEXT->YAGLE_NB_ERR > 0) {
        avt_log(LOGYAG,0," %ld ERROR(S) detected\n",YAG_CONTEXT->YAGLE_NB_ERR);
    }
    avt_log(LOGYAG,0,"See file '%s.rep' for more information\n",YAG_CONTEXT->YAG_OUTNAME);
    avt_log(LOGYAG,0,"------------------------------------------------------------\n");

/*---------------------------------------------------------------------------*
| Destruction of Bdd toolbox                                                 |
*----------------------------------------------------------------------------*/

    destroyBdd(1);
    yag_freetable();

/*---------------------------------------------------------------------------*
| Close files                                                                |
*----------------------------------------------------------------------------*/

    if (YAG_CONTEXT->YAG_STAT_MODE) {
        fclose(YAG_CONTEXT->YAGLE_STAT_FILE);
    }
    if (close_err_file) {
        fclose(YAG_CONTEXT->YAGLE_ERR_FILE);
        YAG_CONTEXT->YAGLE_ERR_FILE = NULL;
    }
    else fflush(YAG_CONTEXT->YAGLE_ERR_FILE);

/*---------------------------------------------------------------------------*
| Cone type cleaning, supply cone renaming and lofig restoration             |
*----------------------------------------------------------------------------*/

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        yagCleanCone(ptcone);
        ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
        if ((ptcone->TYPE & (CNS_VDD|CNS_VSS)) != 0) {
            ptcone->NAME = ptsig->NAMECHAIN->DATA;
        }
        ptchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
        for (; ptchain != NULL; ptchain = ptchain->NEXT) {
            ptcon = (locon_list *)ptchain->DATA;
            if (ptcon->TYPE == 'T' && ptcon->NAME == CNS_GRIDNAME) {
                pttrans = (lotrs_list *)ptcon->ROOT;
                pttrans->GRID = ptcon;
            }
        }
        if (getptype(ptcone->USER, YAG_INFO_PTYPE) != NULL) {
            ptcone->USER = delptype(ptcone->USER, YAG_INFO_PTYPE);
        }
    }
    for (pttrans = ptcnsfig->LOTRS; pttrans; pttrans = pttrans->NEXT) {
        if (getptype(pttrans->USER, CNS_DRIVINGCONE)->DATA == NULL) {
            pttrans->GRID = (locon_list *)getptype(pttrans->USER, YAG_GRIDCON_PTYPE)->DATA;
            pttrans->USER = delptype(pttrans->USER, CNS_DRIVINGCONE);
        }
    }
    for (ptcon = ptcnsfig->INTCON; ptcon; ptcon = ptcon->NEXT) {
        ptcon->TYPE = 'C';
        if (ptcon->SIG->TYPE == 'E') {
            ptcon->SIG->TYPE = 'I';
        }
    }
    for (ptcell = ptcnsfig->CELL; ptcell != NULL; ptcell = ptcell->NEXT) {
        if ((ptuser=getptype(ptcell->USER, YAG_SIGLIST_PTYPE))!=NULL) {
            freechain(ptuser->DATA);
            ptcell->USER=delptype(ptcell->USER, YAG_SIGLIST_PTYPE);
        }
    }
    list0=inf_GetEntriesByType(ifl, INF_SUPPRESS, INF_ANY_VALUES);
    if (YAG_CONTEXT->YAG_TAS_TIMING == YAG_NO_TIMING && list0 == NULL && YAG_CONTEXT->YAG_MINIMISE_INV == FALSE) {
        yagCleanBequad(ptcnsfig->BEFIG);
    }
    freechain(list0);

/*---------------------------------------------------------------------------*
| DISASSEMBLED!!!                                                            |
*----------------------------------------------------------------------------*/

    mbk_comcheck( 0, cns_signcns(ptcnsfig), 0 );
    return ptcnsfig;
}

/****************************************************************************
*                         function yagenv();                                *
****************************************************************************/

/**** read the environment variables ****/

void yagenv(void (*initfunc)())
{
    yag_context_list *newcontext;
    char             *str = NULL;
    char              buf[1024];

    newcontext = (yag_context_list *)mbkalloc(sizeof(yag_context_list));
    if (YAG_CONTEXT != NULL) {
        *newcontext = *YAG_CONTEXT;
        newcontext->NEXT = YAG_CONTEXT;
        YAG_CONTEXT = newcontext;
    }
    else {
        newcontext->NEXT = NULL;
        YAG_CONTEXT = newcontext;
        yagInitialise();
    }
    if (initfunc != NULL) initfunc();

    YAG_CONTEXT->YAG_STAT_MODE = V_BOOL_TAB[__YAGLE_STAT_MODE].VALUE;
    YAG_CONTEXT->YAG_LOOP_MODE = V_BOOL_TAB[__YAGLE_LOOP_MODE].VALUE;
    YAG_CONTEXT->YAG_SILENT_MODE = V_BOOL_TAB[__YAGLE_SILENT_MODE].VALUE;
    
    str = V_STR_TAB[__AVT_LANGUAGE].VALUE;
    if (str != NULL) {
        if ((str[0]=='e') || (str[0]=='E')) {
            YAG_CONTEXT->YAGLE_LANG = 'E';
        }
        else YAG_CONTEXT->YAGLE_LANG = 'F'; 
    }
    
    YAG_CONTEXT->YAG_MAX_LINKS = V_INT_TAB[__YAGLE_MAX_LINKS].VALUE; 
    YAG_CONTEXT->YAG_RELAX_LINKS = V_INT_TAB[__YAGLE_RELAX_LINKS].VALUE; 
    YAG_CONTEXT->YAG_BDDCEILING = V_INT_TAB[__YAGLE_BDDCEILING].VALUE; 
    YAG_CONTEXT->YAG_THRESHOLD = V_FLOAT_TAB[__YAGLE_THRESHOLD].VALUE; 
    YAG_CONTEXT->YAG_SPLITTIMING_RATIO = V_FLOAT_TAB[__YAGLE_SPLITTIMING_RATIO].VALUE; 
    YAG_CONTEXT->YAG_SENSITIVE_RATIO = V_FLOAT_TAB[__YAGLE_SENSITIVE_RATIO].VALUE; 
    YAG_CONTEXT->YAG_SENSITIVE_MAX = V_INT_TAB[__YAGLE_SENSITIVE_MAX].VALUE; 
    YAG_CONTEXT->YAG_RELAX_ALGO = V_BOOL_TAB[__YAGLE_RELAX_ALGO].VALUE;
    YAG_CONTEXT->YAG_MINIMISE_CONES = V_BOOL_TAB[__YAGLE_MINIMISE_CONES].VALUE;
    YAG_CONTEXT->YAG_ASSUME_PRECEDE = V_BOOL_TAB[__YAGLE_BEH_ASSUME_PRECEDE].VALUE;
    YAG_CONTEXT->YAG_BLEEDER_PRECHARGE = V_BOOL_TAB[__YAGLE_BLEEDER_PRECHARGE].VALUE;
    YAG_CONTEXT->YAG_TRISTATE_MEMORY = V_BOOL_TAB[__YAGLE_TRISTATE_MEMORY].VALUE;
    YAG_CONTEXT->YAG_INTERFACE_VECTORS = V_BOOL_TAB[__YAGLE_INTERFACE_VECTORS].VALUE;
    YAG_CONTEXT->YAG_SIMPLIFY_EXPR = V_BOOL_TAB[__YAGLE_SIMPLIFY_EXPRESSIONS].VALUE;
    YAG_CONTEXT->YAG_SIMPLIFY_PROCESSES = V_BOOL_TAB[__YAGLE_SIMPLIFY_PROCESSES].VALUE;
    YAG_CONTEXT->YAG_MINIMISE_INV = V_BOOL_TAB[__YAGLE_MINIMISE_INVERTORS].VALUE;
    YAG_CONTEXT->YAG_COMPACT_BEHAVIOUR = V_BOOL_TAB[__YAGLE_COMPACT_BEHAVIOUR].VALUE;
    YAG_CONTEXT->YAG_GEN_SIGNATURE = V_BOOL_TAB[__YAGLE_GEN_SIGNATURE].VALUE;

    str = V_STR_TAB[__YAGLE_DEBUG_CONE].VALUE;
    if (str != NULL) {
        YAG_CONTEXT->YAG_DEBUG_CONE = namealloc(str); 
    }

    YAG_CONTEXT->YAG_BLOCK_BIDIR = V_BOOL_TAB[__YAGLE_BLOCK_BIDIR].VALUE;
    YAG_CONTEXT->YAG_REMOVE_PARA = V_BOOL_TAB[__YAGLE_REMOVE_PARATRANS].VALUE;
    YAG_CONTEXT->YAG_BLEEDER_STRICTNESS = V_INT_TAB[__YAGLE_BLEEDER_STRICTNESS].VALUE; 
    YAG_CONTEXT->YAG_DEPTH = V_INT_TAB[__YAGLE_DEPTH].VALUE; 
    if (YAG_CONTEXT->YAG_DEPTH == 0) YAG_CONTEXT->YAG_USE_FCF = FALSE;
    YAG_CONTEXT->YAG_AUTOLOOP_DEPTH =  V_INT_TAB[__YAGLE_AUTOLOOP_DEPTH].VALUE; 
    YAG_CONTEXT->YAG_BUS_DEPTH = V_INT_TAB[__YAGLE_BUS_DEPTH].VALUE; 
    YAG_CONTEXT->YAG_AUTOLOOP_CEILING = V_INT_TAB[__YAGLE_AUTOLOOP_CEILING].VALUE; 

    YAG_CONTEXT->YAG_ORIENT = V_BOOL_TAB[__YAGLE_ORIENT].VALUE;
    YAG_CONTEXT->YAG_PROP_HZ = V_BOOL_TAB[__YAGLE_PROP_HZ].VALUE;

    if (V_BOOL_TAB[__YAGLE_MAKE_CELLS].SET)
        YAG_CONTEXT->YAG_MAKE_CELLS = V_BOOL_TAB[__YAGLE_MAKE_CELLS].VALUE;

    YAG_CONTEXT->YAG_GENIUS = V_BOOL_TAB[__YAGLE_GENIUS].VALUE;
    YAG_CONTEXT->YAG_ONLY_GENIUS = V_BOOL_TAB[__YAGLE_ONLY_GENIUS].VALUE;
    YAG_CONTEXT->YAG_CELL_SHARE = V_BOOL_TAB[__YAGLE_CELL_SHARE].VALUE;
    YAG_CONTEXT->YAG_DETECT_LATCHES = V_BOOL_TAB[__YAGLE_DETECT_LATCHES].VALUE;
    YAG_CONTEXT->YAG_LOOP_ANALYSIS = V_BOOL_TAB[__YAGLE_LOOP_ANALYSIS].VALUE;
    YAG_CONTEXT->YAG_AUTO_FLIPFLOP = V_BOOL_TAB[__YAGLE_AUTO_FLIPFLOP].VALUE;

    str = V_STR_TAB[__YAGLE_AUTO_ASYNC].VALUE;
    if (str != NULL) {
        if ((str[0]=='y') || (str[0]=='Y')) {
            YAG_CONTEXT->YAG_AUTO_ASYNC = TRUE;
        }
        else if (!strcasecmp(str, "remove")) {
            YAG_CONTEXT->YAG_AUTO_ASYNC = 2;
        }
        else YAG_CONTEXT->YAG_AUTO_ASYNC = FALSE; 
    }

    str = V_STR_TAB[__YAGLE_AUTO_RS].VALUE;
    if (str != NULL) {
        downstr(str, buf);
        if (!strcmp(buf, "no")) {
            YAG_CONTEXT->YAG_AUTO_RS = 0;
        }
        else {
            YAG_CONTEXT->YAG_AUTO_RS = 1;
            if (strstr(buf, "latch") != NULL) {
                YAG_CONTEXT->YAG_AUTO_RS |= YAG_RS_LATCH;
            }
            if (strstr(buf, "model") != NULL) {
                YAG_CONTEXT->YAG_AUTO_RS |= YAG_RS_MODEL;
            }
            if (strstr(buf, "tolerant") != NULL) {
                YAG_CONTEXT->YAG_AUTO_RS |= YAG_RS_TOLERANT;
            }
            if (strstr(buf, "illegal") != NULL) {
                YAG_CONTEXT->YAG_AUTO_RS |= YAG_RS_ILLEGAL;
            }
            else if (strstr(buf, "legal") != NULL) {
                YAG_CONTEXT->YAG_AUTO_RS |= YAG_RS_LEGAL;
            }
        }
    }

    str = V_STR_TAB[__YAGLE_AUTO_MEMSYM].VALUE;
    if (str != NULL) {
        if ((str[0]=='y') || (str[0]=='Y')) {
            YAG_CONTEXT->YAG_AUTO_MEMSYM = TRUE;
        }
        else YAG_CONTEXT->YAG_AUTO_MEMSYM = FALSE; 
    }

    YAG_CONTEXT->YAG_BUS_ANALYSIS = V_BOOL_TAB[__YAGLE_BUS_ANALYSIS].VALUE;
    YAG_CONTEXT->YAG_FCL_DETECT = V_BOOL_TAB[__YAGLE_FCL_DETECT].VALUE;
    YAG_CONTEXT->YAG_ONLY_FCL = V_BOOL_TAB[__YAGLE_ONLY_FCL].VALUE;
    YAG_CONTEXT->YAG_ONE_SUPPLY = V_BOOL_TAB[__YAGLE_ONE_SUPPLY].VALUE;
    YAG_CONTEXT->YAG_NO_SUPPLY = V_BOOL_TAB[__YAGLE_NO_SUPPLY].VALUE;

    if (V_BOOL_TAB[__YAGLE_NOTSTRICT].SET)
        YAG_CONTEXT->YAG_NOTSTRICT = V_BOOL_TAB[__YAGLE_NOTSTRICT].VALUE;

    YAG_CONTEXT->YAG_ELP = V_BOOL_TAB[__YAGLE_ELP].VALUE;

    str = V_STR_TAB[__YAGLE_TAS_TIMING].VALUE;
    if (str != NULL) {
        if (!strcasecmp(str, "max")) {
            YAG_CONTEXT->YAG_TAS_TIMING = YAG_MAX_TIMING;
        }
        else if (!strcasecmp(str, "med")) {
            YAG_CONTEXT->YAG_TAS_TIMING = YAG_MED_TIMING;
        }
        else if (!strcasecmp(str, "min")) {
            YAG_CONTEXT->YAG_TAS_TIMING = YAG_MIN_TIMING;
        }
    }

    YAG_CONTEXT->YAG_HELP_S = V_BOOL_TAB[__YAGLE_HELP_S].VALUE;

    if (V_STR_TAB[__YAGLE_FILENAME].SET) {
        YAG_CONTEXT->YAG_FILENAME = namealloc(V_STR_TAB[__YAGLE_FILENAME].VALUE);
    }

    if (V_STR_TAB[__YAGLE_FIGNAME].SET) {
        YAG_CONTEXT->YAG_FIGNAME = namealloc(V_STR_TAB[__YAGLE_FIGNAME].VALUE);
    }

    if (V_STR_TAB[__YAGLE_OUTNAME].SET) {
        YAG_CONTEXT->YAG_OUTNAME = namealloc(V_STR_TAB[__YAGLE_OUTNAME].VALUE);
    }

    YAG_CONTEXT->YAG_FILE = V_BOOL_TAB[__YAGLE_FILE].VALUE;
    YAG_CONTEXT->YAG_CONE_NETLIST = V_BOOL_TAB[__YAGLE_CONE_NETLIST].VALUE;
    YAG_CONTEXT->YAG_BEFIG = V_BOOL_TAB[__YAGLE_VBE].VALUE;
    YAG_CONTEXT->YAG_BLACKBOX_SUPPRESS = V_BOOL_TAB[__YAGLE_BLACKBOX_SUPPRESS].VALUE;
    YAG_CONTEXT->YAG_BLACKBOX_IGNORE = V_BOOL_TAB[__YAGLE_BLACKBOX_IGNORE].VALUE;
    YAG_CONTEXT->YAG_NORC = V_BOOL_TAB[__YAGLE_NORC].VALUE;
    YAG_CONTEXT->YAG_HIERARCHICAL_MODE = V_BOOL_TAB[__YAGLE_HIERARCHICAL_MODE].VALUE;
    YAG_CONTEXT->YAG_MARK_TRISTATE_MEMORY = V_BOOL_TAB[__YAGLE_MARK_TRISTATE_MEMORY].VALUE;
    YAG_CONTEXT->YAG_KEEP_REDUNDANT = V_BOOL_TAB[__YAGLE_KEEP_REDUNDANT].VALUE;
    YAG_CONTEXT->YAG_KEEP_GLITCHERS = V_INT_TAB[__YAGLE_KEEP_GLITCHERS].VALUE;
    YAG_CONTEXT->YAG_DETECT_PRECHARGE = V_BOOL_TAB[__YAGLE_DETECT_PRECHARGE].VALUE;
    YAG_CONTEXT->YAG_STRICT_CKLATCH = V_BOOL_TAB[__YAGLE_STRICT_CKLATCH].VALUE;
    YAG_CONTEXT->YAG_USESTMSOLVER = V_BOOL_TAB[__YAGLE_USESTMSOLVER].VALUE;
    YAG_CONTEXT->YAG_LATCH_REQUIRE_CLOCK = V_BOOL_TAB[__YAGLE_LATCH_REQUIRE_CLOCK].VALUE;
    YAG_CONTEXT->YAG_DETECT_REDUNDANT = V_BOOL_TAB[__YAGLE_DETECT_REDUNDANT].VALUE;

    str = V_STR_TAB[__YAGLE_SIMPLE_LATCH].VALUE;
    if (str != NULL) {
        downstr(str, buf);
        YAG_CONTEXT->YAG_SIMPLE_LATCH = FALSE;
        YAG_CONTEXT->YAG_DETECT_LEVELHOLD = 0;
        YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM = FALSE;
        if (strstr(buf, "latch") != NULL) {
            YAG_CONTEXT->YAG_SIMPLE_LATCH = TRUE;
        }
        if (strstr(buf, "memsym") != NULL) {
            YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM = TRUE;
        }
        if (strstr(buf, "strictlevelhold") != NULL) {
            YAG_CONTEXT->YAG_DETECT_LEVELHOLD = 2;
        }
        else if (strstr(buf, "levelhold") != NULL) {
            YAG_CONTEXT->YAG_DETECT_LEVELHOLD = 1;
        }
    }

    str = getenv("YAGLE_DETECT_SIMPLE_MEMSYM");
    if (str != NULL) {
        if ((str[0]=='y') || (str[0]=='Y')) {
            YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM = TRUE;
        }
        else YAG_CONTEXT->YAG_DETECT_SIMPLE_MEMSYM = FALSE; 
    }

    YAG_CONTEXT->YAG_USE_CONNECTOR_DIRECTION = V_BOOL_TAB[__YAGLE_USE_CONNECTOR_DIRECTION].VALUE;

    str = V_STR_TAB[__YAGLE_CLOCK_GATE].VALUE;
    if (str != NULL) {
        downstr(str, buf);
        YAG_CONTEXT->YAG_CLOCK_GATE = 0;
        if (strstr(buf, "yes") != NULL) {
            YAG_CONTEXT->YAG_CLOCK_GATE |= (short)0x3;
        }
        if (strstr(buf, "check") != NULL) {
            YAG_CONTEXT->YAG_CLOCK_GATE |= (short)0x1;
        }
        if (strstr(buf, "filter") != NULL) {
            YAG_CONTEXT->YAG_CLOCK_GATE |= (short)0x2;
        }
    }

    YAG_CONTEXT->YAG_MEMSYM_HEURISTIC = V_BOOL_TAB[__YAGLE_MEMSYM_HEURISTIC].VALUE;
    YAG_CONTEXT->YAG_STUCK_LATCH = V_BOOL_TAB[__YAGLE_STUCK_LATCH].VALUE;
    YAG_CONTEXT->YAG_DELAYED_RS = V_BOOL_TAB[__YAGLE_DELAYED_RS].VALUE;

    YAG_CONTEXT->YAG_FLAGS=0;
}

void
yagrestore()
{
    yag_context_list *oldcontext;

    if (YAG_CONTEXT == NULL) yagBug(DBG_NO_CONTEXT,"yagrestore",NULL,NULL,0);
    oldcontext = YAG_CONTEXT;
    YAG_CONTEXT = YAG_CONTEXT->NEXT;
    mbkfree(oldcontext);
}
