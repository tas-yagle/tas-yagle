/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_main.c                                                  */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include STB_H
#include TRC_H

#include "stb_analysis.h"
#include "stb_drive.h"
#include "stb_util.h"
#include "stb_init.h"

/*****************************************************************************
* global variables                                                           *
*****************************************************************************/

static char *STB_FIGNAME;

static int   STB_TEST_MODE  = 0;

/*****************************************************************************
* static function declarations                                               *
*****************************************************************************/

static int stb_options __P((int argc, char *argv[]));

/****************************************************************************/
/*                           fonction main()                                */
/****************************************************************************/
int
main(argc, argv)
    int             argc;
    char           *argv[];
{
    stbfig_list    *ptstbfig = NULL;
    ttvfig_list    *ptttvfig = NULL;
    STB_MODE_VAR = STB_STABILITY_FF ;
    STB_CTK_VAR = STB_CTK_NOT|STB_CTK_REPORT ;

#ifdef AVERTEC
    avtenv();
#endif

#ifdef MACOS
    argc = ccommand(&argv);
    initmacenv(argv[0]);
#endif

    mbkenv();
    rcnenv();
    elpenv(); /* pour le ELP_CAPA_LEVEL */
    rcx_env();
    stb_env();
    stb_ctk_env();
    mccenv();
    stmenv();
    ttvenv();

    STB_CTK_VAR |= STB_CTX_REPORT ;
    
#ifdef AVERTEC_LICENSE
    if(avt_givetoken("HITAS_LICENSE_SERVER", STB_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif


    if (stb_options(argc, argv) == -1) {
        printf("\nUsage   : stb [options] <file> \n");
        if(STB_LANG=='F') {
            printf("Options : -dg    utilise le graphe detaille\n");
            printf("          -da    analyse detaillee des intervalles\n");
            printf("          -w     analyse pire cas\n");
            printf("          -ff    les latchs monophases sont des bascules\n");
            printf("          -lt    les latchs monophases sont transparents\n");
            printf("          -er    les latchs monophases sont des erreurs\n");
            printf("          -setup calcule seulement les setup\n");
            printf("          -hold  calcule seulement les hold\n");
            printf("          -tl    ne tient compte que du dernier niveau\n");
            printf("          -fe    sauvegarde des erreurs dans le fichier .ste\n");
            printf("          -s     mode silencieux\n");
            printf("          -c     mode diaphonie\n");
            printf("          -cl    sauvegarde les crosstalk dans le fichier dtx\n");
            printf("          -cw    commence les crosstalk dans les conditions pessimistes\n");
            printf("          -cobs  methode des agressions observables\n");
            printf("          -cninf pas de fichier d'information de cross talk\n" );
            printf("          -lm=<valeur>    limitation de la taille de memoire (Mo)\n" );
            printf("          -slope=<valeur> front d'entree (ps)\n" );
            printf("          file   est le nom du fichier '.stb'\n");
            printf("\n");
        }
        else if(STB_LANG == 'E') {
            printf("Options : -dg    uses the detailed graph\n");
            printf("          -da    detailed interval analysis\n");
            printf("          -w     worst case analysis\n");
            printf("          -ff    monophase latchs are flip-flop\n");
            printf("          -lt    monophase latchs are transparent\n");
            printf("          -er    monophase latchs are errors\n");
            printf("          -setup only setup times are calculated\n");
            printf("          -hold  only hold times are calculated\n");
            printf("          -tl    top level paths are taken into account\n");
            printf("          -fe    errors saved in .ste file\n");
            printf("          -s     silent mode\n");
            printf("          -c     cross talk mode\n");
            printf("          -cl    save crosstalk in dtx\n");
            printf("          -cw    crosstalk begin with worst conditions\n");
            printf("          -cninf no crosstalk report\n" );
            printf("          -cobs  crosstalk with observable aggressions\n");
            printf("          -lm=<valeur>    limits memory size (Mo)\n" );
            printf("          -slope=<valeur> input slope (ps)\n" );
            printf("          file   is the name of the '.stb' file\n");
            printf("\n");
        }
        stb_exit(-1);
    }

    if( STB_DEF_SLEW != STB_NO_TIME )
      STM_DEF_SLEW = STB_DEF_SLEW ;

    ttv_disablecache() ;
/*    ttv_setcachesize(STB_LIMITS,STB_LIMITL) ;
    TTV_MAX_SIG = TTV_ALLOC_MAX;*/
   
    ptttvfig = ttv_getttvfig(STB_FIGNAME, TTV_FILE_INF);
    if (ptttvfig != NULL) {
      ttv_init_stm(ptttvfig);
      if (STB_TEST_MODE)
        stb_testSTB(ptttvfig);
      else
        ptstbfig = stb_analysis(ptttvfig, STB_ANALYSIS_VAR, STB_GRAPH_VAR, STB_MODE_VAR, STB_CTK_VAR);
    }

    stb_exit(0) ;
	EXIT(0);
    // just to avoid warning
    return 0;
}

/****************************************************************************/

short stb_RetKey (char *chaine)
{
   unsigned int i;
 
   for (i = 0; (i < strlen (chaine)) && (chaine[i] != '='); i++);
 
   return (i == strlen (chaine)) ? -1 : (short)i;
}
/****************************************************************************/
/*                           fonction stb_option()                          */
/****************************************************************************/
static int
stb_options(argc, argv)
    int             argc;
    char           *argv[];
{
    int             i;
    double          limitmem;
    short           eq ;
    int             slope ;

    if (argc < 2) return -1;

    for (i = 1; i < argc; i++) {
        if ((eq = stb_RetKey (argv[i])) == -1) {
            if (*(argv[i]) != '-') {
                STB_FIGNAME = argv[i];
            }
            else if (strcmp(argv[i], "-dg") == 0) {
                STB_GRAPH_VAR = STB_DET_GRAPH;
            }
            else if (strcmp(argv[i], "-da") == 0) {
                STB_ANALYSIS_VAR = STB_DET_ANALYSIS;
            }
            else if (strcmp(argv[i], "-w") == 0) {
                STB_MODE_VAR |= STB_STABILITY_WORST;
                STB_MODE_VAR &= ~(STB_STABILITY_BEST);
            }
            else if (strcmp(argv[i], "-ff") == 0) {
                STB_MODE_VAR |= STB_STABILITY_FF;
                STB_MODE_VAR &= ~(STB_STABILITY_LT|STB_STABILITY_ER);
            }
            else if (strcmp(argv[i], "-lt") == 0) {
                STB_MODE_VAR |= STB_STABILITY_LT;
                STB_MODE_VAR &= ~(STB_STABILITY_FF|STB_STABILITY_ER);
            }
            else if (strcmp(argv[i], "-er") == 0) {
                STB_MODE_VAR |= STB_STABILITY_ER;
                STB_MODE_VAR &= ~(STB_STABILITY_FF|STB_STABILITY_LT);
            }
            else if (strcmp(argv[i], "-setup") == 0) {
                STB_MODE_VAR |= STB_STABILITY_SETUP;
            }
            else if (strcmp(argv[i], "-hold") == 0) {
                STB_MODE_VAR |= STB_STABILITY_HOLD;
            }
            else if (strcmp(argv[i], "-tl") == 0) {
                STB_MODE_VAR |= STB_STABILITY_LAST;
            }
            else if (strcmp(argv[i], "-nr") == 0) {
                STB_REPORT = 'N' ;
            }
            else if (strcmp(argv[i], "-no") == 0) {
                STB_OUT = 'N' ;
            }
            else if (strcmp(argv[i], "-fe") == 0) {
                STB_SILENT = 'F' ;
            }
            else if (strcmp(argv[i], "-s") == 0) {
                STB_SILENT = 'Y' ;
            }
            else if (strcmp(argv[i], "-c") == 0) {
                STB_CTK_VAR |= STB_CTK ;
            }
            else if (strcmp(argv[i], "-cl") == 0) {
                STB_CTK_VAR |= STB_CTK|STB_CTK_LINE ;
            }
            else if (strcmp(argv[i], "-cw") == 0) {
                STB_CTK_VAR |= STB_CTK|STB_CTK_WORST ;
            }
            else if (strcmp(argv[i], "-cobs") == 0) {
                STB_CTK_VAR |= STB_CTK|STB_CTK_OBSERVABLE ;
            }
            else if (strcmp(argv[i], "-cninf") == 0) {
                STB_CTK_VAR &= ~STB_CTK_REPORT ;
            }
            else if (strcmp(argv[i], "-teststb") == 0) {
                STB_TEST_MODE = 1 ;
            }
            else return -1;
        }else{
            argv[i][eq] = '\0';
            if (!strcmp (argv[i],"-lm")) {
                if (*(argv[i] + eq + 1) != '\0'){
                    if((limitmem = atof(argv[i]+eq+1)) <= (double)0.0){
                        return -1;
                    }else{
                        STB_LIMITS = (double)(limitmem * (double)0.1);
                        STB_LIMITL = (double)(limitmem * (double)0.9);
                    }
                }
            }
            else if (!strcmp( argv[i],"-slope")) {
                if (*(argv[i] + eq + 1) != '\0'){
                    if((slope = atol(argv[i]+eq+1)) <= 0){
                        return -1;
                    }else{
                        STB_DEF_SLEW = slope ;
                    }
                }
            } 
            else return -1;
            
        }

    }

    if(STB_FIGNAME == NULL)
      return -1;
    else
      return 0;
}
