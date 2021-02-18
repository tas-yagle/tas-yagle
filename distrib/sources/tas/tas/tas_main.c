/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_main.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fichier principal. Elle fait appel aux differnts modules                 */
/****************************************************************************/

#include "tas.h"

/*****************************************************************************
 *                           fonction tas_gettoolname                        *
******************************************************************************/
int TAS_PVT_COUNT=0;

char *tas_gettoolname (toolname)
char toolname[];
{
    return ("hitas");
}

/*****************************************************************************
 *                           fonction tas_top_main()                         *
******************************************************************************/
int tas_top_main (int argc, char *argv[])
{
    ttvfig_list *ttvfig;

#ifdef AVERTEC
    avtenv ();
#endif

    mbkenv ();                    /* sinon => traitement normal */
    /* forcage de SCALE_X: la variable d'environnement ne sert plus */

    SCALE_X = V_INT_TAB[__MBK_SCALE_X].VALUE;
    if (SCALE_X < 1000)
        SCALE_X = 1000;

    /* environnement cns */
    cnsenv ();

    /* environnement yagle */
    yagenv (tas_yaginit);
    fclenv ();

    /* environnement elp */
    elpenv ();

    /* environnement mcc */
    mccenv ();

    tlcenv ();
    rcnenv ();
    rcx_env ();
    stb_env ();
    stb_ctk_env ();
    tas_setenv ();

    TAS_CONTEXT->TAS_TOOLNAME = tas_gettoolname (argv[0]);
    signal (SIGINT, (void (*)(int))tas_GetKill);

    if (TAS_CONTEXT->TAS_DEBUG_MODE == 'N') {
        signal (SIGSEGV, (void (*)(int))tas_handler_core);
        signal (SIGBUS, (void (*)(int))tas_handler_core);
    }

#ifdef AVERTEC_LICENSE
    if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
        EXIT (1);
#endif

    tas_option (argc, argv, 1);
    libenv ();
    tlfenv ();
    cbhenv ();
    ttvenv ();

/* traitement des options en ligne de commande */
    tas_option (argc, argv, 0);

    TAS_INFO.argc = argc;
    TAS_INFO.argv = argv;

    TTV_INFO_ARGC = argc;
    TTV_INFO_ARGV = argv;


    ttvfig = tas_main ();

    tas_GetKill (0);
    EXIT (0);
}

/*****************************************************************************
 *                           fonction tas_main()                             *
******************************************************************************/
static int has_fctmodel(ttvfig_list *tvf)
{
  timing_cell *cell;
  timing_model *tm;
  cell = stm_getcell(tvf->INFO->FIGNAME);
  if (cell!=NULL)
  {
     chain_list *cl;
     for (cl=cell->MODEL_LIST; cl!=NULL; cl=cl->NEXT)
      {
        tm=(timing_model *)cl->DATA;
        if (tm->UTYPE== STM_MOD_MODFCT) break;
      }
     if (cl!=NULL) return 1;
  }
  return 0;
}

ttvfig_list *tas_main ()
{
    ttvfig_list *ttvfig;
    char bannername[1024];
    long mode;

    TAS_INFO = tas_InitInfo (TAS_INFO);

    TTV_INFO_TOOLDATE = namealloc (TAS_CONTEXT->TAS_DATE_VER);

    /* banniere */
    strcpy (bannername, "HiTas");
    TAS_CONTEXT->TAS_TOOLNAME = "hitas";

    avt_banner (bannername, "CMOS-VLSI Timing Analyzer", "1998");

    stmenv ();

    if (YAG_CONTEXT->YAG_GENIUS == TRUE)
        mode = TAS_KEEP_LOFIG;
    else
        mode = ~TAS_KEEP_LOFIG;

    ttvfig = tas_core (mode);
    stm_pwl_finish ();

    // zinaps : 5/5/2003, genius needs the lofig until the end
    if (mode == TAS_KEEP_LOFIG && TAS_CONTEXT->TAS_LOFIG && TAS_CONTEXT->TAS_NOTAS == 'N') {
        if (!has_fctmodel(ttvfig))
        {
          unlocklofig (TAS_CONTEXT->TAS_LOFIG);
          tas_freelofig (TAS_CONTEXT->TAS_LOFIG);
        }
    }
    // ---

    if (TAS_CONTEXT->TAS_SUPBLACKB == 'Y') {
        if (ttvfig == NULL) {
            TAS_CONTEXT->TAS_SUPBLACKB = 'N';
            TAS_CONTEXT->TAS_HIER = 'Y';
            ttvfig = tas_core (~TAS_KEEP_LOFIG);
        }
    }

    return ttvfig;
}

/*****************************************************************************
*                            fonction tas_core()                             *
*****************************************************************************/
ttvfig_list *tas_core (int keeplofig)
{
    struct rusage Tstart, Tend, start, end;
    long tTstart, tTend, tstart, tend;
    lofig_list *lofig = NULL;
    cnsfig_list *cnsfig=NULL;
    ttvfig_list *ttvfig = NULL;
    char filename[256];
    char *tasfilename = NULL;
    char *tasfileout = NULL;
    timing_cell *cell;
    FILE *inffile;
    int flat_message = 1;
    spisig_list *spisig = NULL;
    ptype_list *path = NULL;
    char *yagtool;
    char *yagout;
    char *env;
    inffig_list *ifl = NULL;
    long trun;
    long tuse;
    double cpu;
    ptype_list *ptype;
    eqt_ctx *mc_ctx = NULL;


    TAS_PVT_COUNT=0;

    if (TAS_CONTEXT->TAS_TOOLNAME == NULL)
        TAS_CONTEXT->TAS_TOOLNAME = "hitas";

#ifdef AVERTEC_LICENSE
    if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
        EXIT (1);
#endif

    getrusage (RUSAGE_SELF, &Tstart);
    time (&tTstart);

    ttv_setcachesize ((double)(TAS_CONTEXT->TAS_LIMITMEM * (double)0.1),
                      (double)(TAS_CONTEXT->TAS_LIMITMEM * (double)0.9));

    tas_TechnoParameters ();


    if (TAS_CONTEXT->TAS_PWL == 'Y') {
        fprintf (stdout, "WRITING OF 'PwlRise' and 'PwlFall'\n");
        tas_PwlFile ();
        fprintf (stdout, "-----------------------------------\n");
        fflush (stdout);
    }

    if (TAS_CONTEXT->TAS_NOTAS == 'N') {
        char tool[8];
        strcpy (tool, "TAS");
        if ((TAS_CONTEXT->TAS_CNS_LOAD == 'Y') && (TAS_CONTEXT->TAS_HIER == 'N')) {
            fprintf (stdout, "LOADING FILE %s.cns\n", TAS_CONTEXT->TAS_FILENAME);
            fflush (stdout);
            getrusage (RUSAGE_SELF, &start);
            time (&tstart);
            /* Lecture du fichier INFO */
            inf_DisplayLoad (1);
            if (TAS_CONTEXT->TAS_FILEIN != NULL) {
                ifl = getinffig (TAS_CONTEXT->TAS_FILEIN);
            }
            else {
                ifl = getinffig (TAS_CONTEXT->TAS_FILENAME);
            }
            inf_DisplayLoad (0);
            if (ifl) {
                /*ttv_addaxis (ifl);*/
                tas_traiteinout (ifl);
                tas_update_mcctemp (ifl);
            }
#ifdef AVERTEC_LICENSE
            if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                EXIT (1);
#endif

            cnsfig = getcnsfig (TAS_CONTEXT->TAS_FILENAME, NULL);
            lofig = getlofig (cnsfig->NAME, 'A');
            if(sim_SetPowerSupply (lofig)) stm_init();
            
            TAS_CONTEXT->TAS_LOFIG = lofig;
            cnsfig->NAME = TAS_CONTEXT->TAS_FILEOUT;
            infclone (cnsfig->NAME, ifl);    // <- on suit le changement de nom
            TAS_CONTEXT->TAS_CNSFIG = cnsfig;
            getrusage (RUSAGE_SELF, &end);
            time (&tend);
            tas_PrintTime (start, end, tend - tstart);
            TAS_INFO.lo_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                    (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
            TAS_INFO.db_t = (long)(0);
            fprintf (stdout, "-----------------------------------\n");
            fflush (stdout);
            mcc_InitGlobalMonteCarloDistributions(lofig);
        }
        else {
     /*----------------------------------------------------------------------*/
            /* sinon, on charge la figure MBK, on la met a plat, on rajoute les     */
            /* capas des transistors, on la desassemble et, s'il le faut, on crache */
            /* le fichier .cns                                                      */
     /*----------------------------------------------------------------------*/
            /* Lecture du fichier INFO */
            inf_DisplayLoad (1);
            if (TAS_CONTEXT->TAS_FILEIN != NULL) {
                ifl = getinffig (TAS_CONTEXT->TAS_FILEIN);
            }
            else {
                ifl = getinffig (TAS_CONTEXT->TAS_FILENAME);
            }
            inf_DisplayLoad (0);
            if (ifl != NULL) {
                /*ttv_addaxis (ifl);*/
                tas_traiteinout (ifl);
                tas_update_mcctemp (ifl);
            }

#ifdef AVERTEC_LICENSE
            if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                EXIT (1);
#endif
            /* chargement */
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat (NULL, 1);
#endif

            fprintf (stdout, "LOADING FILE %s\n", TAS_CONTEXT->TAS_FILENAME);
            fflush (stdout);
            getrusage (RUSAGE_SELF, &start);
            time (&tstart);

            lofig = getlofig (TAS_CONTEXT->TAS_FILENAME, 'A');

            if (TAS_CONTEXT->TAS_FILEIN != NULL) {
                ifl = getloadedinffig (TAS_CONTEXT->TAS_FILEIN);
            }
            else {
                ifl = getloadedinffig (TAS_CONTEXT->TAS_FILENAME);
            }

            if (ifl != NULL)
                TAS_CONTEXT->INF_SIGLIST = inf_create_INFSIGLIST (ifl);

            if (TAS_CONTEXT->TAS_FILEIN != NULL)
                if (strcmp (lofig->NAME, TAS_CONTEXT->TAS_FILEIN) != 0) {
                    lofig = getlofig (TAS_CONTEXT->TAS_FILEIN, 'A');
                    TAS_CONTEXT->TAS_FILENAME = lofig->NAME;
                }
            // if special operating cond were specified in spi file...
            tas_update_mcctemp (ifl);

            if (lofig) TAS_CONTEXT->TAS_FILENAME = lofig->NAME;

            if (TAS_CONTEXT->TAS_SUPBLACKB == 'Y') {
                tasfilename = TAS_CONTEXT->TAS_FILENAME;
                tasfileout = TAS_CONTEXT->TAS_FILEOUT;
                TAS_CONTEXT->TAS_HIERLOFIG = lofig;
                if (TAS_CONTEXT->TAS_TRANSINSNAME == NULL) {
                    strcpy (filename, TAS_CONTEXT->TAS_FILENAME);
                    strcat (filename, "_tastrans");
                }
                else {
                    strcpy (filename, TAS_CONTEXT->TAS_TRANSINSNAME);
                }
                TAS_CONTEXT->TAS_FILENAME = namealloc (filename);
                TAS_CONTEXT->TAS_FILEOUT = TAS_CONTEXT->TAS_FILENAME;
                lofigchain (lofig);
                unflatOutsideList (lofig, TAS_CONTEXT->TAS_FILENAME, TAS_CONTEXT->TAS_FILENAME);
                lofig = getlofig (TAS_CONTEXT->TAS_FILENAME, 'A');
                lofigchain (lofig);
            }

            TAS_CONTEXT->TAS_LOFIG = lofig;

            /* mise a plat */
            if (TAS_CONTEXT->TAS_HIER == 'N') {
                flat_message = 1;
                if (flat_message && lofig->LOINS) {
                    fprintf (stdout, "FLATENNING THE FIGURE\n");
                    flat_message = 0;
                }
                ptype = getptype (lofig->USER, PARAM_CONTEXT);
                flatten_parameters (lofig, NULL, ptype ? (eqt_param *) ptype->DATA : NULL , 0, 0, 0, 0, 0, 0, &mc_ctx);
                if (mc_ctx!=NULL) eqt_term(mc_ctx);
                if (TAS_CONTEXT->TAS_IGNBLACKB == 'N')
                    flattenlofig (lofig, NULL, 'Y');
                else
                    flatOutsideList (lofig);


#ifdef DELAY_DEBUG_STAT
                mbk_debugstat ("flatten:", 0);
#endif
                tas_capasig (lofig);
                lofigchain (lofig);

                /* les shrinks */
                //elpLofigShrink (lofig);
            }
            else {
                lofigchain (lofig);
                tas_flattenhfig (lofig);
                tas_capasig (lofig);
                lofigchain (lofig);
            }

            if (ifl != NULL) {
                list_list *ll;
                if ((ifl->LOADED.INF_SIGLIST) && (ifl->LOADED.INF_SIGLIST->TYPE == INF_LL_PATHSIGS))
                    path = (ptype_list *) reverse ((chain_list *) efg_CreatePathByInf (ifl->LOADED.INF_SIGLIST));
                TAS_CONTEXT->INF_SIGLIST = tas_GetAliasSig (lofig, TAS_CONTEXT->INF_SIGLIST);
            }
            /* information */
            getrusage (RUSAGE_SELF, &end);
            time (&tend);
            tas_PrintTime (start, end, tend - tstart);
            TAS_INFO.lo_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                    (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
            fprintf (stdout, "-----------------------------------\n");
            fflush (stdout);

            if(sim_SetPowerSupply (lofig)) stm_init();

            mcc_InitGlobalMonteCarloDistributions(lofig);

            if (TAS_CONTEXT->TAS_HIER == 'N') {
                /* desassemblage */
                fprintf (stdout, "DISASSEMBLING:\n");

                fflush (stdout);
                getrusage (RUSAGE_SELF, &start);
                time (&tstart);

#ifdef AVERTEC_LICENSE
                if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                    EXIT (1);
#endif

                yagtool = YAG_CONTEXT->YAGLE_TOOLNAME;
                YAG_CONTEXT->YAGLE_TOOLNAME = TAS_CONTEXT->TAS_TOOLNAME;
#ifndef ELPONLYNOCNS
                if (TAS_CONTEXT->TAS_SAVE_BEFIG == 'Y') {
                    cnsfig = yagDisassemble (TAS_CONTEXT->TAS_FILEOUT, lofig, YAG_GENBEFIG);
                    TAS_CONTEXT->TAS_BEFIG = cnsfig->BEFIG;
                    cnsfig->BEFIG = NULL;
                }
                else {
                    if( TAS_CONTEXT->TAS_CNS_ANNOTATE_LOFIG == 'Y' ) {
                        printf( "tas annotation\n" );
                        cnsfig = getcnsfig (TAS_CONTEXT->TAS_FILENAME, TAS_CONTEXT->TAS_LOFIG);
                    }
                    else
                        cnsfig = yagDisassemble (TAS_CONTEXT->TAS_FILEOUT, lofig, 0);
                }
                YAG_CONTEXT->YAGLE_TOOLNAME = yagtool;

                if (YAG_CONTEXT->YAGLE_NB_ERR > 0)
                    tas_error (45, cnsfig->NAME, TAS_ERROR);


                cnsfig->NAME = TAS_CONTEXT->TAS_FILEOUT;
                lofig->NAME = TAS_CONTEXT->TAS_FILEOUT;
                infclone (cnsfig->NAME, ifl);

                TAS_CONTEXT->TAS_CNSFIG = cnsfig;
                getrusage (RUSAGE_SELF, &end);
                time (&tend);
                tas_PrintTime (start, end, tend - tstart);
                TAS_INFO.db_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                        (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                fprintf (stdout, "-----------------------------------\n");
                fflush (stdout);

                /* driver du fichier .cns si l'utilisateur l'a demande */
                if (TAS_CONTEXT->TAS_CNS_FILE == 'Y' && TAS_CONTEXT->TAS_CNS_LOAD == 'N' && TAS_CONTEXT->TAS_CNS_ANNOTATE_LOFIG == 'N' ) {
                    fprintf (stdout, "CNS FILE %s.cns:\n", cnsfig->NAME);
                    fflush (stdout);
                    getrusage (RUSAGE_SELF, &start);
                    time (&tstart);
#ifdef AVERTEC_LICENSE
                    if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                        EXIT (1);
#endif
                    savecnsfig (cnsfig, TAS_CONTEXT->TAS_LOFIG);
                    getrusage (RUSAGE_SELF, &end);
                    time (&tend);
                    tas_PrintTime (start, end, tend - tstart);
                    fprintf (stdout, "-----------------------------------\n");
                    fflush (stdout);
                }
#endif
            }
        }

        if (TAS_CONTEXT->TAS_HIER == 'Y')
            lofig->NAME = TAS_CONTEXT->TAS_FILEOUT;

        /* calcul des temps elementaires */
        fprintf (stdout, "COMPUTING GATE DELAYS:\n");
        fflush (stdout);
        getrusage (RUSAGE_SELF, &start);
        time (&tstart);

#ifdef DELAY_DEBUG_STAT
        mbk_debugstat (NULL, 1);
#endif
            
#ifdef AVERTEC_LICENSE
        if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
            EXIT (1);
#endif

     /*-----------------------------------------------------------------*/
        /* calcul des capacites dynamiques: les capas de grille sont       */
        /* systematiquement calculees. Les capas de diff ne sont calculees */
        /* que si les dimensions du transistor sont non-nulles.            */
     /*-----------------------------------------------------------------*/
        if (cnsfig!=NULL) {
            cns_addmultivoltage (ifl, cnsfig);
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat ("addmultivoltage:", 0);
#endif
            tas_log_multivoltage (lofig);
            tas_AffectConnectorsOutputCapacitance(ifl, cnsfig);        
        }

        if (TAS_CONTEXT->TAS_HIER == 'Y' && lofig->LOTRS != NULL) {
           tas_error (50, lofig->NAME, TAS_ERROR);
        }

        elpLofigAddCapas (lofig, TAS_CASE);

#ifdef DELAY_DEBUG_STAT
        mbk_debugstat ("elpcapagrid:", 0);
#endif
#ifdef ELPONLY
        exit(0);
#endif
        if (TAS_CONTEXT->TAS_HIER == 'N') {
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat (NULL, 1);
#endif
            tas_detectinout (cnsfig);
            tas_initcnsfigalloc (cnsfig);
            tas_AddCapaOut (cnsfig);
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat ("pre:", 0);
#endif
            ttvfig = tas_builtfig (lofig);
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat ("builtfig:", 0);
#endif
            tas_loconorient (lofig, ttvfig);
            tas_setsigname (lofig);
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat ("lcori+setsig:", 0);
#endif
            tas_timing (cnsfig, lofig, ttvfig);
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat ("timing:", 0);
#endif
            if (keeplofig != TAS_KEEP_LOFIG)
              tas_RemoveRC(lofig);
            tas_cns2ttv (cnsfig, ttvfig, lofig);
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat ("cns2ttv:", 0);
#endif
            if (lofig != NULL) {
                if (getptype (ttvfig->INFO->USER, TTV_FIG_LOFIG) == NULL)
                    ttvfig->INFO->USER = addptype (ttvfig->INFO->USER, TTV_FIG_LOFIG, lofig);
                locklofig (lofig);
            }
        }
        else {
            ttvfig = tas_builtttvfig (lofig);
            ttv_getinffile(ttvfig);
        }

        avt_log (LOGTAS, 3, "\n\nFinal Parameters\n\n");
        tas_visu ();

        if ((TAS_CONTEXT->TAS_PERFINT == 'Y') && (TAS_CONTEXT->TAS_INT_END == 'Y')) {
            if ((TAS_CONTEXT->TAS_CARACMODE == TAS_LUT_CPL) ||
                (TAS_CONTEXT->TAS_CARACMODE == TAS_SCM_CPL) || (TAS_CONTEXT->TAS_CARACMODE == TAS_PLY_CPL))
                ttv_cleanfigmodel (ttvfig, TTV_LINE_D | TTV_LINE_E | TTV_LINE_F, TTV_LINE_T | TTV_LINE_J | TTV_LINE_P);
            else
                ttv_cleanfigmodel (ttvfig, TTV_LINE_D, TTV_LINE_E | TTV_LINE_F | TTV_LINE_T | TTV_LINE_J | TTV_LINE_P);
        }

        if (TAS_CONTEXT->TAS_CHECKRCDELAY == 'Y')
            tas_testelmore (lofig);
        ttv_setttvdate (ttvfig, TTV_DATE_LOCAL);
        {
            char technoversion[20];

            ttvfig->INFO->TOOLNAME = namealloc (TAS_CONTEXT->TAS_TOOLNAME);
            ttvfig->INFO->TOOLVERSION = AVT_FULLVERSION;
            ttvfig->INFO->STHHIGH = elpGeneral[elpGSHTHR];
            ttvfig->INFO->STHLOW = elpGeneral[elpGSLTHR];
            ttvfig->INFO->DTH = elpGeneral[elpGDTHR];
            ttvfig->INFO->TEMP = elpGeneral[elpTEMP];
            ttvfig->INFO->TNOM = V_FLOAT_TAB[__SIM_TNOM].VALUE;
            ttvfig->INFO->VDD = elpGeneral[elpGVDDMAX];
            if (elpTechnoName[0] == '\0') {
                ttvfig->INFO->TECHNONAME = namealloc ("unknown");
                sprintf (technoversion, "%.2f", 0.0);
                ttvfig->INFO->TECHNOVERSION = namealloc (technoversion);
            }
            else {
                ttvfig->INFO->TECHNONAME = namealloc (elpTechnoName);
                sprintf (technoversion, "%.2f", elpTechnoVersion);
                ttvfig->INFO->TECHNOVERSION = namealloc (technoversion);
            }
            ttvfig->INFO->SLOPE = (long)TAS_CONTEXT->FRONT_CON;
            ttvfig->INFO->CAPAOUT = (TAS_CONTEXT->TAS_CAPAOUT * 1000.0);
            if (TAS_CONTEXT->TAS_HIER == 'N') {
                TTV_INFO_CAPARM = (long)(TAS_CONTEXT->TAS_CAPARAPREC * 100.0);
                TTV_INFO_CAPASW = (long)(TAS_CONTEXT->TAS_CAPASWITCH * 100.0);
            }
            else {
                TTV_INFO_CAPARM = (long)(-1);
                TTV_INFO_CAPASW = (long)(-1);
            }
        }

#ifdef DELAY_DEBUG_STAT
        mbk_debugstat ("reste:", 0);
#endif

        getrusage (RUSAGE_SELF, &end);
        time (&tend);
        tas_PrintTime (start, end, tend - tstart);
        TAS_INFO.el_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
        fprintf (stdout, "-----------------------------------\n");
        fflush (stdout);

        /* fichier des fronts du cone de la figure */
        if (TAS_CONTEXT->TAS_SLOFILE == 'Y') {
            fprintf (stdout, "SIGNALS SLOPES %s.slo:\n", TAS_CONTEXT->TAS_FILEOUT);
            fflush (stdout);
            getrusage (RUSAGE_SELF, &start);
            time (&tstart);
#ifdef AVERTEC_LICENSE
            if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                EXIT (1);
#endif
            if (TAS_CONTEXT->TAS_HIER == 'N')
                tas_drislo (cnsfig, NULL);
            else
                tas_drislo (NULL, ttvfig);
            getrusage (RUSAGE_SELF, &end);
            time (&tend);
            tas_PrintTime (start, end, tend - tstart);
            fprintf (stdout, "-----------------------------------\n");
            fflush (stdout);
        }

        // tas_displaymemoryusage( cnsfig );

        /*liberation de la memoire */
        if (TAS_CONTEXT->TAS_HIER == 'N') {
            cns_delmultivoltage (cnsfig);
            tas_freecnsfigalloc (cnsfig);
            tas_freecnsfig (cnsfig);
        }

        lofig->NAME = TAS_CONTEXT->TAS_FILENAME;
        if (keeplofig != TAS_KEEP_LOFIG) {
            unlocklofig (lofig);
            tas_freelofig (lofig);
        }


        if (getptype (ttvfig->INFO->USER, TTV_FIG_LOFIG))
            ttvfig->INFO->USER = delptype (ttvfig->INFO->USER, TTV_FIG_LOFIG);

        if ((TAS_CONTEXT->TAS_FACTORISE == 'Y') || (TAS_CONTEXT->TAS_FACTORISE == 'G')) {
            long type;

            fprintf (stdout, "FACTORIZATION POINTS DETECTION:\n");
            fflush (stdout);
            getrusage (RUSAGE_SELF, &start);
            time (&tstart);

            if (TAS_CONTEXT->TAS_FACTORISE == 'G')
                type = TTV_FIND_FACTGLO;
            else
                type = (long)0;

            if (TAS_CONTEXT->TAS_HIER == 'N') {
                ttv_detectinter (ttvfig, TTV_FIND_LINE | type);
            }
            else {
                ttv_detectinter (ttvfig, TTV_FIND_PATH | type);
            }

            getrusage (RUSAGE_SELF, &end);
            time (&tend);
            tas_PrintTime (start, end, tend - tstart);
            fprintf (stdout, "-----------------------------------\n");
            fflush (stdout);
        }

        /* stb dans le cas -t (sur dtx) */
        if ((TAS_CONTEXT->TAS_STABILITY == 'Y')
            && ((TAS_CONTEXT->TAS_PERFINT == 'Y') || ((STB_CTK_VAR & STB_CTK) == STB_CTK))) {
            fprintf (stdout, "STABILITY ANALYSIS:\n");
            fflush (stdout);
            getrusage (RUSAGE_SELF, &start);
            time (&tstart);

            if (TAS_CONTEXT->TAS_DELAY_PROP == 'Y')
                STB_CTK_VAR |= STB_PROPAGATE;
            STB_CTK_VAR |= STB_CTK_LINE;
            stb_analysis (ttvfig, STB_ANALYSIS_VAR, STB_GRAPH_VAR, STB_MODE_VAR, STB_CTK_VAR);

            getrusage (RUSAGE_SELF, &end);
            time (&tend);
            tas_PrintTime (start, end, tend - tstart);
            fprintf (stdout, "-----------------------------------\n");
            fflush (stdout);
        }

        ttv_post_traitment (ttvfig);

        /*perfmodule intermediaire */
        if ((TAS_CONTEXT->TAS_PERFINT == 'Y') && (TAS_CONTEXT->TAS_INT_END == 'Y'))
            if ((TAS_CONTEXT->TAS_PERFILE == 'X') || (TAS_CONTEXT->TAS_PERFILE == 'A')) {
                fprintf (stdout, "INTERMEDIATE PERFMODULE ");
                fprintf (stdout, "%s.dtx:\n", TAS_CONTEXT->TAS_FILEOUT);
                fflush (stdout);
                getrusage (RUSAGE_SELF, &start);
                time (&tstart);

#ifdef AVERTEC_LICENSE
                if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                    EXIT (1);
#endif
                mbk_comcheck( 0, 0, ttv_signtimingfigure( ttvfig ) );
                ttv_drittv (ttvfig, TTV_FILE_DTX, (long)0, NULL);

                // SSTA STUFF
                if (MBK_SOCK!=-1)
                {
                  if (TAS_PVT_COUNT>0)
                  {
                    char buf[1024];
                    sprintf(buf,"info: warning: UTD has %d PVT condition errors", TAS_PVT_COUNT);
                    mbk_sendmsg( MBK_COM_DATA, buf, strlen(buf)+1);
                  }
                  else
                  {
                     sprintf(filename,"%s.dtx",ttvfig->INFO->FIGNAME);
                     mbk_comm_send_text_file(filename);
                     if (ifl!=NULL)
                     {
                        sprintf(filename,"%s.ssta.dtx.inf",ttvfig->INFO->FIGNAME);
                        infDrive(ifl, filename, INF_LOADED_LOCATION, NULL);
                        mbk_comm_send_text_file(filename);
                     }
                     ttv_sendttvdelays(ttvfig, 0, TAS_PVT_COUNT);
                  }
                }

                getrusage (RUSAGE_SELF, &end);
                time (&tend);
                tas_PrintTime (start, end, tend - tstart);
                fprintf (stdout, "-----------------------------------\n");
                fflush (stdout);
                if ((cell = stm_getcell (ttvfig->INFO->FIGNAME)) != NULL)
                    if (cell->MODEL_LIST != NULL) {
                        fprintf (stdout, "TIMING MODEL ");
                        fprintf (stdout, "%s.stm:\n", TAS_CONTEXT->TAS_FILEOUT);
                        fflush (stdout);
                        getrusage (RUSAGE_SELF, &start);
                        time (&tstart);

#ifdef AVERTEC_LICENSE
                        if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                            EXIT (1);
#endif
                        if (DETAILED_MODELS && 0) {
                            stm_freemodellist (ttvfig->INFO->FIGNAME, DETAILED_MODELS);
                            freechain (DETAILED_MODELS);
                        }
                        stm_drivecell (ttvfig->INFO->FIGNAME);

                        getrusage (RUSAGE_SELF, &end);
                        time (&tend);
                        tas_PrintTime (start, end, tend - tstart);
                        TAS_INFO.pr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                                (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                        fprintf (stdout, "-----------------------------------\n");
                        fflush (stdout);
                    }
            }

        if ((TAS_CONTEXT->TAS_PERFINT == 'Y') && (TAS_CONTEXT->TAS_INT_END == 'Y'))
            if ((TAS_CONTEXT->TAS_PERFILE == 'V') || (TAS_CONTEXT->TAS_PERFILE == 'A')) {
                fprintf (stdout, "INTERMEDIATE PERFMODULE ");
                fprintf (stdout, "%s.dtv:\n", TAS_CONTEXT->TAS_FILEOUT);
                fflush (stdout);
                getrusage (RUSAGE_SELF, &start);
                time (&tstart);

#ifdef AVERTEC_LICENSE
                if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                    EXIT (1);
#endif
                if ((TAS_CONTEXT->TAS_HIER == 'Y') || (TAS_CONTEXT->TAS_FACTORISE != 'N')
                    || ((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y')))
                    ttv_drittvold (ttvfig, TTV_FILE_DTV, (long)0);
                else
                    ttv_drittv (ttvfig, TTV_FILE_DTV, (long)0, NULL);

                getrusage (RUSAGE_SELF, &end);
                time (&tend);
                tas_PrintTime (start, end, tend - tstart);
                fprintf (stdout, "-----------------------------------\n"); fflush (stdout);

                if ((cell = stm_getcell (ttvfig->INFO->FIGNAME)) != NULL)
                    if (cell->MODEL_LIST != NULL) {
                        fprintf (stdout, "TIMING MODEL ");
                        fprintf (stdout, "%s.stm:\n", TAS_CONTEXT->TAS_FILEOUT);

                        fflush (stdout);
                        getrusage (RUSAGE_SELF, &start);
                        time (&tstart);
#ifdef AVERTEC_LICENSE
                        if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                            EXIT (1);
#endif
                        if (DETAILED_MODELS && 0) {
                            stm_freemodellist (ttvfig->INFO->FIGNAME, DETAILED_MODELS);
                            freechain (DETAILED_MODELS);
                        }
                        stm_drivecell (ttvfig->INFO->FIGNAME);

                        getrusage (RUSAGE_SELF, &end);
                        time (&tend);
                        tas_PrintTime (start, end, tend - tstart);
                        TAS_INFO.pr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                                (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                        fprintf (stdout, "-----------------------------------\n"); fflush (stdout);
                    }
            }

        if (TAS_CONTEXT->TAS_INT_END == 'N') {
            /* parcours de graphe et perfmodule */
            fprintf (stdout, "SEARCH OF CRITICAL PATHS:\n");
            fflush (stdout);
            getrusage (RUSAGE_SELF, &start);
            time (&tstart);

            {
                if (TAS_CONTEXT->TAS_HIER == 'Y') {
                    if (TAS_CONTEXT->TAS_NHIER == 'Y')
                        ttv_builtpath (ttvfig, TTV_FIND_PATH | TTV_FIND_CMD);
                    else
                        ttv_builtpath (ttvfig, TTV_FIND_HIER | TTV_FIND_PATH | TTV_FIND_CMD);
                }
                else {
                    if (TAS_CONTEXT->TAS_NHIER == 'Y')
                        ttv_builtpath (ttvfig, TTV_FIND_LINE | TTV_FIND_CMD);
                    else
                        ttv_builtpath (ttvfig, TTV_FIND_HIER | TTV_FIND_LINE | TTV_FIND_CMD);
                }

                if (TAS_CONTEXT->TAS_PERFINT == 'Y') {
                    if ((TAS_CONTEXT->TAS_CARACMODE == TAS_LUT_CPL) ||
                        (TAS_CONTEXT->TAS_CARACMODE == TAS_SCM_CPL) || (TAS_CONTEXT->TAS_CARACMODE == TAS_PLY_CPL))
                        ttv_cleanfigmodel (ttvfig, TTV_LINE_D | TTV_LINE_E | TTV_LINE_F | TTV_LINE_T,
                                           TTV_LINE_J | TTV_LINE_P);
                    else
                        ttv_cleanfigmodel (ttvfig, TTV_LINE_D | TTV_LINE_T,
                                           TTV_LINE_E | TTV_LINE_F | TTV_LINE_J | TTV_LINE_P);
                }
                else {
                    ttv_cleanfigmodel (ttvfig, TTV_LINE_T,
                                       TTV_LINE_D | TTV_LINE_E | TTV_LINE_F | TTV_LINE_J | TTV_LINE_P);
                }


                getrusage (RUSAGE_SELF, &end);
                time (&tend);
                tas_PrintTime (start, end, tend - tstart);
                TAS_INFO.gr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                        (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                fprintf (stdout, "-----------------------------------\n"); fflush (stdout);

                /* stb dans le cas pas d'option (alors sur ttx) */
                if ((TAS_CONTEXT->TAS_STABILITY == 'Y')
                    && ((TAS_CONTEXT->TAS_PERFINT == 'N') && ((STB_CTK_VAR & STB_CTK) != STB_CTK))) {
                    fprintf (stdout, "STABILITY ANALYSIS:\n"); fflush (stdout);
                    getrusage (RUSAGE_SELF, &start);
                    time (&tstart);

                    if (TAS_CONTEXT->TAS_DELAY_PROP == 'Y') STB_CTK_VAR |= STB_PROPAGATE;
                    STB_CTK_VAR |= STB_CTK_LINE;
                    stb_analysis (ttvfig, STB_ANALYSIS_VAR, STB_GRAPH_VAR, STB_MODE_VAR, STB_CTK_VAR);

                    getrusage (RUSAGE_SELF, &end);
                    time (&tend);
                    tas_PrintTime (start, end, tend - tstart);
                    fprintf (stdout, "-----------------------------------\n"); fflush (stdout);
                }

                if ((TAS_CONTEXT->TAS_PERFINT == 'Y') && (TAS_CONTEXT->TAS_INT_END == 'N'))
                    if ((TAS_CONTEXT->TAS_PERFILE == 'X') || (TAS_CONTEXT->TAS_PERFILE == 'A')) {
                        fprintf (stdout, "INTERMEDIATE PERFMODULE ");
                        fprintf (stdout, "%s.dtx:\n", TAS_CONTEXT->TAS_FILEOUT); fflush (stdout);
                        getrusage (RUSAGE_SELF, &start);
                        time (&tstart);
#ifdef AVERTEC_LICENSE
                        if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN) EXIT (1);
#endif
                        ttv_drittv (ttvfig, TTV_FILE_DTX, (long)0, NULL);

                        getrusage (RUSAGE_SELF, &end);
                        time (&tend);
                        tas_PrintTime (start, end, tend - tstart);
                        fprintf (stdout, "-----------------------------------\n"); fflush (stdout);

                        
                    }

                /* ecriture perfmodule */
                if ((TAS_CONTEXT->TAS_PERFILE == 'X') || (TAS_CONTEXT->TAS_PERFILE == 'A')) {
                    fprintf (stdout, "GENERAL PERFMODULE ");
                    fprintf (stdout, "%s.ttx:\n", TAS_CONTEXT->TAS_FILEOUT); fflush (stdout);
                    getrusage (RUSAGE_SELF, &start);
                    time (&tstart);

#ifdef AVERTEC_LICENSE
                    if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN) EXIT (1);
#endif
                    ttv_drittv (ttvfig, TTV_FILE_TTX, (long)0, NULL);

                    getrusage (RUSAGE_SELF, &end);
                    time (&tend);
                    tas_PrintTime (start, end, tend - tstart);
                    TAS_INFO.pr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                            (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                    fprintf (stdout, "-----------------------------------\n"); fflush (stdout);
                }

                if ((cell = stm_getcell (ttvfig->INFO->FIGNAME)) != NULL)
                    if (cell->MODEL_LIST != NULL) {
                        fprintf (stdout, "TIMING MODEL ");
                        fprintf (stdout, "%s.stm:\n", TAS_CONTEXT->TAS_FILEOUT);


                        fflush (stdout);
                        getrusage (RUSAGE_SELF, &start);
                        time (&tstart);

#ifdef AVERTEC_LICENSE
                        if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                            EXIT (1);
#endif
                        if (DETAILED_MODELS && 0) {
                            stm_freemodellist (ttvfig->INFO->FIGNAME, DETAILED_MODELS);
                            freechain (DETAILED_MODELS);
                        }
                        stm_drivecell (ttvfig->INFO->FIGNAME);

                        getrusage (RUSAGE_SELF, &end);
                        time (&tend);
                        tas_PrintTime (start, end, tend - tstart);
                        TAS_INFO.pr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                                (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                        fprintf (stdout, "-----------------------------------\n");
                        fflush (stdout);
                    }


                if (TAS_CONTEXT->TAS_SILENTMODE == 'N') {
                    ttvpath_list *ptpathmax = NULL;
                    ttvpath_list *ptpathmin = NULL;
                    long complexity = (long)0;
                    long nbchain = (long)0;
                    long nbcouple = (long)0;
                    long i, j;
                    ttvlbloc_list *ptlbloc[3];
                    ttvline_list *ptline;
                    chain_list *chain;

                    fprintf (stdout, "TIMING ANALYSIS REPORT:\n");

                    fflush (stdout);
                    getrusage (RUSAGE_SELF, &start);
                    time (&tstart);

                    ttv_checkallinstool (ttvfig);
                    for (chain = ttvfig->INS; chain != NULL; chain = chain->NEXT)
                        ttv_checkfigins (ttvfig, (ttvfig_list *) chain->DATA);
                    ptlbloc[0] = ttvfig->FBLOC;
                    ptlbloc[1] = ttvfig->EBLOC;
                    ptlbloc[2] = ttvfig->DBLOC;
                    for (i = 0; i < 3; i++)
                        for (; ptlbloc[i] != NULL; ptlbloc[i] = ptlbloc[i]->NEXT) {
                            for (j = 0; j < TTV_MAX_LBLOC; j++) {
                                ptline = ptlbloc[i]->LINE + j;
                                if ((ptline->TYPE & TTV_LINE_FR) != TTV_LINE_FR)
                                    complexity++;
                            }
                        }

                    ptlbloc[0] = ttvfig->PBLOC;
                    ptlbloc[1] = ttvfig->JBLOC;
                    ptlbloc[2] = ttvfig->TBLOC;
                    for (i = 0; i < 2; i++)
                        for (; ptlbloc[i] != NULL; ptlbloc[i] = ptlbloc[i]->NEXT) {
                            for (j = 0; j < TTV_MAX_LBLOC; j++) {
                                ptline = ptlbloc[i]->LINE + j;
                                if ((ptline->TYPE & TTV_LINE_FR) != TTV_LINE_FR)
                                    nbchain++;
                            }
                        }

                    for (; ptlbloc[2] != NULL; ptlbloc[2] = ptlbloc[2]->NEXT) {
                        for (j = 0; j < TTV_MAX_LBLOC; j++) {
                            ptline = ptlbloc[2]->LINE + j;
                            if ((ptline->TYPE & TTV_LINE_FR) != TTV_LINE_FR)
                                nbcouple++;
                        }
                    }

                    if (complexity != (long)0) {
                        TAS_INFO.comp = complexity;
                        fprintf (stdout, "Timing arcs in DTX file: %ld\n", complexity);
                        fflush (stdout);
                    }

                    ttv_getinffile (ttvfig);

                    ptpathmax = ttv_getcriticpath (ttvfig, NULL, TTV_NAME_IN, TTV_NAME_OUT,
                                                   TTV_DELAY_MAX, TTV_DELAY_MIN, 1, TTV_FIND_MAX);
                    ptpathmin = ttv_getcriticpath (ttvfig, NULL, TTV_NAME_IN, TTV_NAME_OUT,
                                                   TTV_DELAY_MAX, TTV_DELAY_MIN, 1, TTV_FIND_MIN);
                    {
                        char buf[1024];
                        if (ptpathmax != NULL) {
                            TAS_INFO.maxdelay = ptpathmax->DELAY;
                            fprintf (stdout, "The circuit worst case delay is %.1fpS\n", ptpathmax->DELAY / TTV_UNIT);

                            if ((ptpathmax->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                                fprintf (stdout, "--> from _/- ");
                            else
                                fprintf (stdout, "--> from -\\_ ");

                            fprintf (stdout, "%s\n--> to ", ttv_getsigname (ttvfig, buf, ptpathmax->NODE->ROOT));

                            if ((ptpathmax->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ)
                                fprintf (stdout, "HighZ ");
                            else if ((ptpathmax->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                                fprintf (stdout, "_/- ");
                            else
                                fprintf (stdout, "-\\_ ");

                            fprintf (stdout, "%s\n", ttv_getsigname (ttvfig, buf, ptpathmax->ROOT->ROOT));
                        }

                        if (ptpathmin != NULL) {
                            TAS_INFO.mindelay = ptpathmin->DELAY;
                            fprintf (stdout, "The circuit best case delay is %.1fpS\n", ptpathmin->DELAY / TTV_UNIT);

                            if ((ptpathmin->NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                                fprintf (stdout, "--> from _/- ");
                            else
                                fprintf (stdout, "--> from -\\_ ");

                            fprintf (stdout, "%s\n--> to ", ttv_getsigname (ttvfig, buf, ptpathmin->NODE->ROOT));

                            if ((ptpathmin->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ)
                                fprintf (stdout, "HighZ ");
                            else if ((ptpathmin->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
                                fprintf (stdout, "_/- ");
                            else
                                fprintf (stdout, "-\\_ ");

                            fprintf (stdout, "%s\n", ttv_getsigname (ttvfig, buf, ptpathmin->ROOT->ROOT));
                        }

                        if ((nbcouple != (long)0) || (nbchain != (long)0)) {
                            fprintf (stdout, "External timing paths in TTX file: %ld\n", nbcouple);
                            fprintf (stdout, "Internal timing paths in TTX file: %ld\n", nbchain);
                        }
                        if (ptpathmax == NULL && ptpathmin == NULL) fprintf (stdout, "No signal propagation\n");
                        else {
                            ttv_freepathlist (ptpathmax);
                            ttv_freepathlist (ptpathmin);
                        }
                    }

                    getrusage (RUSAGE_SELF, &end);
                    time (&tend);
                    tas_PrintTime (start, end, tend - tstart);
                    TAS_INFO.gr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                            (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                    fprintf (stdout, "-----------------------------------\n"); fflush (stdout);
                }

                if ((TAS_CONTEXT->TAS_PERFINT == 'Y') && (TAS_CONTEXT->TAS_INT_END == 'N'))
                    if ((TAS_CONTEXT->TAS_PERFILE == 'V') || (TAS_CONTEXT->TAS_PERFILE == 'A')) {
                        fprintf (stdout, "INTERMEDIATE PERFMODULE ");
                        fprintf (stdout, "%s.dtv:\n", TAS_CONTEXT->TAS_FILEOUT); fflush (stdout);
                        getrusage (RUSAGE_SELF, &start);
                        time (&tstart);

#ifdef AVERTEC_LICENSE
                        if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN) EXIT (1);
#endif
                        if ((TAS_CONTEXT->TAS_HIER == 'Y') || (TAS_CONTEXT->TAS_FACTORISE != 'N')
                            || ((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y')))
                            ttv_drittvold (ttvfig, TTV_FILE_DTV, (long)0);
                        else
                            ttv_drittv (ttvfig, TTV_FILE_DTV, (long)0, NULL);

                        getrusage (RUSAGE_SELF, &end);
                        time (&tend);
                        tas_PrintTime (start, end, tend - tstart);
                        fprintf (stdout, "-----------------------------------\n");
                        fflush (stdout);
                    }

                if ((TAS_CONTEXT->TAS_PERFILE == 'V') || (TAS_CONTEXT->TAS_PERFILE == 'A')) {
                    long find = (long)0;

                    fprintf (stdout, "GENERAL PERFMODULE ");
                    fprintf (stdout, "%s.ttv :\n", TAS_CONTEXT->TAS_FILEOUT); fflush (stdout);
                    getrusage (RUSAGE_SELF, &start);
                    time (&tstart);

                    if ((TAS_CONTEXT->TAS_NHIER == 'N') && (TAS_CONTEXT->TAS_HIER == 'N'))
                        find |= TTV_FIND_HIER;

                    if ((TAS_CONTEXT->TAS_HIER == 'Y') && (TAS_CONTEXT->TAS_PERFINT == 'N'))
                        find |= TTV_FIND_NOTSAVE;

#ifdef AVERTEC_LICENSE
                    if (avt_givetoken ("HITAS_LICENSE_SERVER", TAS_CONTEXT->TAS_TOOLNAME) != AVT_VALID_TOKEN)
                        EXIT (1);
#endif
                    if ((TAS_CONTEXT->TAS_HIER == 'Y') || (TAS_CONTEXT->TAS_FACTORISE != 'N')
                        || ((TAS_CONTEXT->TAS_MERGERCN == 'N') && (TAS_CONTEXT->TAS_CALCRCX == 'Y')))
                        ttv_drittvold (ttvfig, TTV_FILE_TTV, find);
                    else
                        ttv_drittv (ttvfig, TTV_FILE_TTV, find, NULL);

                    getrusage (RUSAGE_SELF, &end);
                    time (&tend);
                    tas_PrintTime (start, end, tend - tstart);
                    TAS_INFO.pr_t = (long)(((end.ru_utime.tv_sec + end.ru_stime.tv_sec) -
                                            (start.ru_utime.tv_sec + start.ru_stime.tv_sec)));
                    fprintf (stdout, "-----------------------------------\n"); fflush (stdout);
                }
            }
        }
        ttv_unlockttvfig (ttvfig);
    }

/* fin du traitement */
    getrusage (RUSAGE_SELF, &Tend);
    time (&tTend);

    fprintf (stdout, "TOTAL RUN TIME:\n");
    tas_PrintTime (Tstart, Tend, tTend - tTstart);
    trun = 10L * (tTend - tTstart);
    if (trun == 0L)
        trun = 10L;
    tuse = (long)(((10L * Tend.ru_utime.tv_sec) + (Tend.ru_utime.tv_usec / 100000L))
                  - ((10L * Tstart.ru_utime.tv_sec) + (Tstart.ru_utime.tv_usec / 100000L))
                  + ((10L * Tend.ru_stime.tv_sec) + (Tend.ru_stime.tv_usec / 100000L))
                  - ((10L * Tstart.ru_stime.tv_sec) + (Tstart.ru_stime.tv_usec / 100000L)));
    cpu = 100.0 * ((double)tuse / trun);
    if (cpu > 100.0)
        cpu = 100.0;
    fprintf (stdout, "%.2f%% CPU---end!!!\n", cpu);
    fprintf (stdout, "-----------------------------------\n");
    fflush (stdout);

    inffreell (TAS_CONTEXT->INF_SIGLIST);
    TAS_CONTEXT->INF_SIGLIST = NULL;

    if (TAS_CONTEXT->TAS_SUPBLACKB == 'Y') {
        TAS_CONTEXT->TAS_FILENAME = tasfilename;
        TAS_CONTEXT->TAS_FILEOUT = tasfileout;
        ttv_freettvfigmemory (ttvfig, TTV_STS_S | TTV_STS_F | TTV_STS_P);
        return (NULL);
    }

    return ttvfig;
}

ttvfig_list *hitas_tcl (char *figname)
{
    ttvfig_list *ttvfig;

    mbkenv ();
    cnsenv ();
    yagenv (tas_yaginit);
    fclenv ();
    elpenv ();
    mccenv ();
    tlcenv ();
    rcnenv ();
    rcx_env ();
    stb_env ();
    stb_ctk_env ();
    tas_setenv ();
    tas_version ();
    libenv ();
    tlfenv ();
    cbhenv ();
    ttvenv ();

    TAS_CONTEXT->TAS_FILENAME = namealloc (figname);

    if (getenv("TASREADCNS"))
       TAS_CONTEXT->TAS_CNS_LOAD='Y';

    { // configuration check level I
      lofig_list *lf;
      locon_list *lc;
      chain_list *data;
      inffig_list *ifl;

      data = NULL;

      lf = getloadedlofig(TAS_CONTEXT->TAS_FILENAME);
      ifl = getloadedinffig(TAS_CONTEXT->TAS_FILENAME);
      if (lf)
      {
        for (lc = lf->LOCON; lc; lc = lc->NEXT)
          data = addchain(data,lc->NAME);

        if (ifl)
        {
          inf_CheckRegistry(stdout,ifl,1,data);
        }
      }
      freechain(data);
    }


    
    TAS_CONTEXT->TAS_FILEIN = TAS_CONTEXT->TAS_FILENAME;
    YAG_CONTEXT->YAG_FIGNAME = TAS_CONTEXT->TAS_FILEIN;
    if (!TAS_CONTEXT->TAS_FILEOUT)
        TAS_CONTEXT->TAS_FILEOUT = TAS_CONTEXT->TAS_FILEIN;

    if ((ttvfig=ttv_gethtmodel(TAS_CONTEXT->TAS_FILEOUT))!=NULL)
      {
        ttv_freettvfig(ttvfig);
      }

    if(TAS_CONTEXT->TAS_SILENTMODE == 'Y')
        tas_CloseTerm() ;

    ttvfig = tas_main ();

    if(TAS_CONTEXT){
        mbkfree (TAS_CONTEXT);
        TAS_CONTEXT = NULL;
    }

    ttv_getinffile(ttvfig);

    return ttvfig;
}

