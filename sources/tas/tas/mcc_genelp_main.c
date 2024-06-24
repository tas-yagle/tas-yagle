#include "../mcc/mcc.h"
#include "../mcc/mcc_mod_util.h"
#include "../mcc/mcc_util.h"
#include "../mcc/mcc_genspi.h"
#include "../mcc/mcc_debug.h"
#include "../mcc/mcc_curv.h"
#include TTV_H
#include TAS_H

int main (argc, argv)
int argc;
char *argv[];
{
	char *pt;
	char *nomout = NULL;
	char *tn = NULL;
	char *tp = NULL;
	int i;
	int eq;
	int plot = 0;
	int add = 0;
	int aut = 0;
	int spice = 1;
	int res = 1;
	int fit = 1;
	int vt = 1;
    int lib = 0;
	int first;
	int calc = MCC_SIM_MODE;
	double tec = -1.0;
	double ps = -1.0;
	double slope = -1.0;
	double vgs = -1.0;
	double temp = -1000.0;
	int old_load_elp = -1;
	mcc_modellist *modeln = NULL;
	mcc_modellist *modelp = NULL;
    elp_lotrs_param *lotrsparam_n = NULL;
    elp_lotrs_param *lotrsparam_p = NULL;
    lofig_list *f;
    char options[128];

#ifdef AVERTEC
	avtenv ();
#endif

	mbkenv ();

    elpenv() ;

    hitas_tcl_pt=(hitas_tcl_pt_t)hitas_tcl;

    if (ELP_LOAD_FILE_TYPE != ELP_LOADELP_FILE)
     {
      old_load_elp = ELP_LOAD_FILE_TYPE;
      ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE;
     }

	if (V_INT_TAB[__MBK_SCALE_X].VALUE == 0)
		SCALE_X = 10000 ;
	else {
		if (SCALE_X < 1000)
			SCALE_X = 1000;
	}

	mcc_initcalcparam(0);
    lotrsparam_n = mcc_init_lotrsparam ();
    lotrsparam_p = mcc_init_lotrsparam ();
	mcc_prsparam (lotrsparam_n,lotrsparam_p);
	mccenv ();
	mcc_drvparam (lotrsparam_n,lotrsparam_p);

    if((pt = getenv("MCC_DEBUG_MODE")) != NULL)
      {
        if(strcmp(pt,"yes") == 0)
          MCC_DEBUG_MODE = 2 ;
        else
          MCC_DEBUG_MODE = atoi(pt) ;
      }
    if((pt = getenv("MCC_DEBUG_PASS_TRANS")) != NULL)
      {
       if (strcasecmp(pt,"N") == 0)
         MCC_DEBUG_PASS_TRANS = 'n' ;
       else if (strcasecmp(pt,"P") == 0)
         MCC_DEBUG_PASS_TRANS = 'p' ;
       else
         MCC_DEBUG_PASS_TRANS = 'X' ;
      }
    if((pt = getenv("MCC_DEBUG_SWITCH")) != NULL)
      {
       if ( MCC_DEBUG_PASS_TRANS == 'X' ) 
         {
          if (strcasecmp(pt,"N") == 0)
            MCC_DEBUG_SWITCH = 'n' ;
          else if (strcasecmp(pt,"P") == 0)
            MCC_DEBUG_SWITCH = 'p' ;
          else
            MCC_DEBUG_SWITCH = 'X' ;
         }
       else
         MCC_DEBUG_SWITCH = 'X' ;
      }
    if((pt = getenv("MCC_DEBUG_BLEEDER")) != NULL)
      {
       if ( MCC_DEBUG_PASS_TRANS != 'X' || MCC_DEBUG_SWITCH != 'X' )
         MCC_DEBUG_BLEEDER = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
       else
         MCC_DEBUG_BLEEDER = 'N';
      }
    if((pt = getenv("MCC_DEBUG_INPUT")) != NULL)
      {
       MCC_DEBUG_INPUT = *pt - 'a' ;
      }
    if((pt = getenv("MCC_DEBUG_TRANS")) != NULL)
      {
       MCC_DEBUG_TRANS = atoi(pt) ;
      }
    if((pt = getenv("MCC_DEBUG_SLOPE")) != NULL)
      {
       MCC_DEBUG_SLOPE = atoi(pt) ;
      }
    if((pt = getenv("MCC_DEBUG_CAPA")) != NULL)
      {
       MCC_DEBUG_CAPA = atoi(pt) ;
      }

    if((pt = getenv("MCC_DEBUG_LEAK")) != NULL)
      {
       MCC_DEBUG_LEAK = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_RSAT")) != NULL)
      {
       MCC_DEBUG_RSAT = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_RLIN")) != NULL)
      {
       MCC_DEBUG_RLIN = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_VTH")) != NULL)
      {
       MCC_DEBUG_VTH = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_QINT")) != NULL)
      {
       MCC_DEBUG_QINT = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_JCT_CAPA")) != NULL)
      {
       MCC_DEBUG_JCT_CAPA = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_INPUTCAPA")) != NULL)
      {
       MCC_DEBUG_INPUTCAPA = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_CARAC")) != NULL)
      {
       MCC_DEBUG_CARAC = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_CARACNEG")) != NULL)
      {
       MCC_DEBUG_CARACNEG = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_DEG")) != NULL)
      {
       MCC_DEBUG_DEG = (strcmp(pt,"yes") == 0) ? 'Y' : 'N' ;
      }
    if((pt = getenv("MCC_DEBUG_GATE")) != NULL)
      {
         char buf[1024];

         if ( MCC_DEBUG_PASS_TRANS != 'X' )
           snprintf ( buf , 1024, "%s_p%c",buf,MCC_DEBUG_PASS_TRANS);
         else if ( MCC_DEBUG_SWITCH != 'X' )
           snprintf ( buf , 1024, "%s_s%c",buf,MCC_DEBUG_SWITCH);
         if ( MCC_DEBUG_BLEEDER == 'Y' )
           snprintf ( buf , 1024, "%s_bleed",buf);
         MCC_DEBUG_GATE = buf ;
      }

    options[0] = '\0';
	for (i = 1; i != argc; i++) {
        if ( (strcmp (argv[i],"-f")) && (strcmp(argv[i],"-l")))
          sprintf (options,"%s %s",options,argv[i]);
		if (argv[i][0] != '-') {
			if (nomout == NULL) {
				nomout = mbkalloc (strlen (argv[i]) + 1);
				strcpy (nomout, argv[i]);
				MCC_ELPFILE = mbkalloc (strlen (argv[i]) + 5);
				if ((pt = strchr (nomout, (int)('.'))) != NULL)
					*pt = '\0';
				sprintf (MCC_ELPFILE, "%s.%s", nomout, "elp");
				mbkfree (nomout);
			}
			else {
				MCC_TECHFILE = mbkalloc (strlen (argv[i]) + 5);
				strcpy (MCC_TECHFILE, argv[i]);
				if (MCC_MODELFILE != NULL)
					mbkfree (MCC_MODELFILE);
				MCC_MODELFILE = mcc_initstr (MCC_TECHFILE);
			}
		}
		else {
			if ((eq = mcc_retkey (argv[i])) != -1) {
				pt = argv[i];
				pt[eq] = '\0';
				pt = argv[i];
				if (strcmp (pt, "-tec") == 0) {
					tec = atof (pt + eq + 1);
				}
				else if (strcmp (pt, "-ps") == 0) {
					ps = atof (pt + eq + 1);
				}
				else if (strcmp (pt, "-slope") == 0) {
					slope = atof (pt + eq + 1);
				}
				else if (strcmp (pt, "-vgs") == 0) {
					vgs = atof (pt + eq + 1);
				}
				else if (strcmp (pt, "-temp") == 0) {
					temp = atof (pt + eq + 1);
				}
				else if (strcmp (pt, "-tn") == 0) {
					tn = namealloc (pt + eq + 1);
				}
				else if (strcmp (pt, "-tp") == 0) {
					tp = namealloc (pt + eq + 1);
				}
				else {
				    fprintf (stderr, "\nusage\t: genelp [-f|-e|-h|-p|-a|-n|-s|-i|-v|-l|-c]\n");
                    fprintf (stderr, "[-tec=<size>] [-temp=<temp>] [-ps=<volt>] [-slope=<time>]\n");
                    fprintf (stderr, "[-vgs=<volt>] [-tn=<tnname>] [-tp=<tpname>] [elpname] [technoname]\n");
					EXIT (1);
				}
			}
			else if (strcmp (argv[i], "-f") == 0) {
				MCC_MODE = MCC_FAST_MODE;
			}
			else if (strcmp (argv[i], "-c") == 0) {
				MCC_CALC_CUR = MCC_CALC_MODE;
				calc = MCC_CALC_MODE;
			}
			else if (strcmp (argv[i], "-e") == 0) {
				MCC_MODE = MCC_EXPERT_MODE;
			}
			else if (strcmp (argv[i], "-p") == 0) {
				plot = 1;
                V_BOOL_TAB[__SIM_USE_PRINT].VALUE=1;
                MCC_PLOT = 1;
			}
			else if (strcmp (argv[i], "-a") == 0) {
				aut = 1;
			}
			else if (strcmp (argv[i], "-n") == 0) {
				add = 1;
			}
			else if (strcmp (argv[i], "-s") == 0) {
				spice = 0;
			}
			else if (strcmp (argv[i], "-i") == 0) {
				fit = 0;
			}
			else if (strcmp (argv[i], "-v") == 0) {
				vt = 0;
			}
			else if (strcmp (argv[i], "-l") == 0) {
				lib = 1;
			}
			else if (strcmp (argv[i], "-h") == 0) {
			    fprintf (stderr, "\nusage\t: genelp [-f|-e|-h|-p|-a|-n|-s|-i|-v|-l|-c]\n");
                fprintf (stderr, "[-tec=<size>] [-temp=<temp>] [-ps=<volt>] [-slope=<time>]\n");
                fprintf (stderr, "[-vgs=<volt>] [-tn=<tnname>] [-tp=<tpname>] [elpname] [technoname]\n");
				fprintf (stderr, "\t-f : fast mode use .mccgenelp file\n");
				fprintf (stderr, "\t-e : expert mode \n");
				fprintf (stderr, "\t-p : generate a gnuplot code\n");
				fprintf (stderr, "\t-a : run in automatic mode\n");
				fprintf (stderr, "\t-n : add model to elp file\n");
				fprintf (stderr, "\t-s : do not run spice\n");
				fprintf (stderr, "\t-i : do not fit parameters\n");
				fprintf (stderr, "\t-l : extract parametrisation sizes from netlist\n");
				fprintf (stderr, "\t-v : do not calc threshold voltage\n");
				fprintf (stderr, "\t-c : calc current from bsim3\n");
				fprintf (stderr, "\t-tec=<size> : technology size\n");
				fprintf (stderr, "\t-temp=<temp> : temperature\n");
				fprintf (stderr, "\t-ps=<volt> : power supply\n");
				fprintf (stderr, "\t-slope=<time> : input slope in ps\n");
				fprintf (stderr, "\t-vgs=<volt> : grid source voltage for curv\n");
				fprintf (stderr, "\t-tn=<tnname> : N transistor model name\n");
				fprintf (stderr, "\t-tp=<tpname> : P transistor model name\n");
				fprintf (stderr, "\telpname : output elp parameters file name\n");
				fprintf (stderr, "\ttechnoname : input transistor model file name\n");
				EXIT (1);
			}
			else {
			    fprintf (stderr, "\nusage\t: genelp [-f|-e|-h|-p|-a|-n|-s|-i|-v|-l|-c]\n");
                fprintf (stderr, "[-tec=<size>] [-temp=<temp>] [-ps=<volt>] [-slope=<time>]\n");
                fprintf (stderr, "[-vgs=<volt>] [-tn=<tnname>] [-tp=<tpname>] [elpname] [technoname]\n");
				EXIT (1);
			}
		}
	}

	if (tec > 0.0) {
		MCC_LN = tec;
		MCC_WN = tec * 6.0;
		MCC_LP = tec;
		MCC_WP = tec * 12.0;
		MCC_DIF = tec * 2.0;
	}

	if (ps > 0.0) {
		MCC_VDDmax = ps;
		MCC_VGS = MCC_VDDmax / 2.0 ;
	}

	if (slope > 0.0)
		MCC_SLOPE = slope;

	if (vgs > 0.0)
		MCC_VGS = vgs;

	if (temp > -274.0) {
		MCC_TEMP = temp;
	}

	if (tn != NULL) {
		if (MCC_TNMODEL != NULL)
			mbkfree (MCC_TNMODEL);
		MCC_TNMODEL = mcc_initstr (tn);
	}

	if (tp != NULL) {
		if (MCC_TPMODEL != NULL)
			mbkfree (MCC_TPMODEL);
		MCC_TPMODEL = mcc_initstr (tp);
	}

    if(MCC_CALC_CUR == MCC_SIM_MODE)
        MCC_OPTIM_MODE = 0 ;

	mcc_drvparam (lotrsparam_n,lotrsparam_p);

#ifdef AVERTEC_LICENSE
	if (avt_givetoken ("AVT_LICENSE_SERVER", "genelp") != AVT_VALID_TOKEN)
		EXIT (1);
#endif

	avt_banner ("GenElp", "Electrical parameters generator", "1998");

	first = 1;

    if (lib) {
        char *script;

        fprintf (stdout, "[INFO] Generating parametrization script ...\n");
        readlibfile ("spice", parsespice, 0);

        if (HEAD_LOFIG) {
            fprintf (stdout, "[INFO] Source netlists:\n");
            for (f = HEAD_LOFIG; f; f = f->NEXT) fprintf (stdout, "       %s\n", f->NAME);
            fprintf (stdout, "\n");
        }
        else
            fprintf (stdout, "[ERR] No source netlists\n");

        if (MCC_RESIZE_RANGE)
            script = mcc_MakeScript (HEAD_LOFIG, MCC_RANGE_MARGIN, options);
        else
            script = mcc_MakeScript (HEAD_LOFIG, 0.0, options);

        if (script)
            fprintf (stdout, "[INFO] Script '%s' generated\n", script);
        else
            fprintf (stdout, "[ERR] No script generated\n");
        res = 0;
    }

	while ( res ) {
        if ( !lotrsparam_n || !lotrsparam_p ) {
          elp_lotrs_param_free ( lotrsparam_n );
          elp_lotrs_param_free ( lotrsparam_p );
          lotrsparam_n = mcc_init_lotrsparam ();
          lotrsparam_p = mcc_init_lotrsparam ();
        }
        if ( lotrsparam_n->VBULK <= ELPMINVBULK )
	      lotrsparam_n->VBULK = 0.0;
        if ( lotrsparam_p->VBULK <= ELPMINVBULK )
	      lotrsparam_p->VBULK = MCC_VDDmax;
		res = mcc_genelp (spice, vt, tec, aut, 
                          fit, &modeln, &modelp,
                          lotrsparam_n,lotrsparam_p);

		MCC_CALC_CUR = calc;
        if (MCC_VBULKP <= ELPMINVBULK) MCC_VBULKP=MCC_VDDmax; //sav a faire
        if (MCC_VBULKN <= ELPMINVBULK) MCC_VBULKN=0.0;


		if ((modeln == NULL) && (modelp == NULL) && (aut == 1)) {
			if (first == 0)
				break;
			else {
				fprintf (stderr, "\ncan not calculate size for model %s and %s\n", MCC_TNMODEL, MCC_TPMODEL);
				EXIT (1);
			}
		}

		first = 0;

		if ((MCC_MODE == MCC_FAST_MODE) && (aut == 0))
			res = 0;

#ifdef AVERTEC_LICENSE
		if (avt_givetoken ("AVT_LICENSE_SERVER", "genelp") != AVT_VALID_TOKEN)
			EXIT (1);
#endif

		if (add == 1) {
			strcpy (elpTechnoFile, MCC_ELPFILE);
			elpLoadElp ();
			add = 0;
		}

		if ((fit == 1) && (MCC_FLAG_FIT == MCC_FIT_OK))
         {
          mcc_fit (lotrsparam_n,lotrsparam_p,0);
          if(MCC_DEBUG_MODE > 1) 
           {
            mcc_genbench() ;
           }
         }
		else if ((fit == 1) && (MCC_FLAG_FIT == MCC_FIT_KO))
			fprintf (stderr, "\nmcc warning : time simulation too short, can't fit !!!\n");
		else {
			mcc_drvelp (MCC_PARAM, MCC_TRANS_B,lotrsparam_n,lotrsparam_p,MCC_DRV_ALL_MODEL);
			mcc_gencurv (MCC_PARAM,lotrsparam_n,lotrsparam_p);
            if(MCC_DEBUG_MODE > 1) 
             {
              mcc_genbench() ;
             }
		}

#ifdef AVERTEC_LICENSE
		if (avt_givetoken ("AVT_LICENSE_SERVER", "genelp") != AVT_VALID_TOKEN)
			EXIT (1);
#endif

		if (plot != 0)
			mcc_printplot ((fit == 1) ? MCC_FIT : MCC_PARAM, 0, -1000, -1000, modeln, modelp);

		if (MCC_MODE != MCC_FAST_MODE)
			MCC_MODE = MCC_NEXT_MODE;
        elp_lotrs_param_free ( lotrsparam_n );
        lotrsparam_n = NULL;
        elp_lotrs_param_free ( lotrsparam_p );
        lotrsparam_p = NULL;
	}

	fprintf (stdout, "\n");

    eqt_term(mccEqtCtx) ;

    if ( old_load_elp > -1 )
      ELP_LOAD_FILE_TYPE = old_load_elp;

	EXIT (0);
}
