/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_main.c                                                  */
/*                                                                          */
/*    (c) copyright 2001                                                    */
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
#include "yag_timing.h"

#ifndef WITHOUT_TAS
#include MCC_H
#include LIB_H
#include TLF_H
#include STM_H
#include TRC_H
#endif


/* Static function declarations */

static short yagGetArgs __P((int argc, char *argv[]));
static char *optionIdent __P((char *name, char *chaine));

/****************************************************************************
*                         fonction main();                                 *
****************************************************************************/
int main(argc, argv)
    int     argc;
    char    *argv[];
{
    char error[YAGBUFSIZE];
    
#ifdef AVERTEC
    avtenv();
#endif

    mbkenv();               /* MBK environment */

    cnsenv();               /* CNS environment */
    yagenv(NULL);           /* YAG environment */
    fclenv();               /* FCL environment */
    rcnenv();
    rcx_env();
#ifdef AVERTEC_LICENSE
    if(avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

/*----------------------------------------------------------------------------*
|      OPTIONS                                                                |
+-----------------------------------------------------------------------------*/

    if(yagGetArgs(argc, argv) == -1) {
        printf("\nUsage   : yagle [options] <file1> [file2]\n");
        printf("Options : -i        reads the '.inf' file\n");
        printf("          -fig=str  str is the name of the figure to handle\n");
        printf("          -v        vectorises the vhdl description\n");
        printf("          -hier     hierarchical disassembly mode\n");
        printf("          -p=n      n is the depth for functional analysis\n");
        printf("          -nc       no detection of complex gates (cells)\n");
        printf("          -cs       allow cells to share gates\n");
        printf("          -ffs      mark slave instead of master in a flip-flop\n");
        printf("          -nl       no latch recognition\n");
        printf("          -la       automatic latch detection\n");
        printf("          -ba       automatic bus analysis\n");
        printf("          -fcl      transistor netlist recognition\n");
        printf("          -xfcl     only transistor netlist recognition\n");
        printf("          -gns      hierarchical recognition\n");
        printf("          -xg       only hierarchical recognition\n");
        printf("          -tdmax    behavior with max delays\n");
        printf("          -tdmin    behavior with min delays\n");
        printf("          -tdmed    behavior with average delays\n");
        printf("          -elp      use the technology file '.elp'\n");
        printf("          -o        transistor orientation\n");
        printf("          -a        to take the '_s' convention into account\n");
        printf("          -z        functional analysis through HZ nodes\n");
        printf("          -s        only one vdd and vss in the vhdl description\n");
        printf("          -nb       no vhdl file generated\n");
        printf("          -ch       generates a hierarchical cone netlist\n");
        printf("          -d        generates a .cns file\n");
        printf("          -c        generates a .cns file with signals and RC networks\n");
        printf("          -bx       generates an intermediate netlist without black boxes\n");
        printf("          -bi       black boxes become instances\n");
        printf("          -ns       unconnected transistor gates are warnings\n");
        printf("          -nrc      suppression of RC before disassembly\n");
        printf("          -nrp      suppress parallel transistor reduction\n");
        printf("          -mi       remove simple inversions\n");
        printf("          -t        display execution trace\n");
        printf("          file2     is the vhdl file to be generated (default is file1)\n");
        printf("\n");
        yagExit(-1);
    }

/*----------------------------------------------------------------------------*
| Environment for transistor models and library preload                       |
+-----------------------------------------------------------------------------*/

#ifndef WITHOUT_TAS
    elpenv();
    mccenv();
    stmenv();
    elpLoadOnceElp();

    readlibfile("lib", lib_parse, 0);
    readlibfile("tlf3", parsetlf3, 0);
    readlibfile("tlf4", parsetlf4, 0);
#endif
    cbhenv();
    
/*----------------------------------------------------------------------------*
| Install the signal handlers                                                 |
+-----------------------------------------------------------------------------*/

    avt_TrapKill ();
    avt_PushKillExit (yagExit, 100);
    sprintf (error, "%s%s", "\n[YAGLE] A fatal internal error has occured\n",
                            "        rerun with -nb option and consult report\n");
    avt_TrapSegV ();
    avt_PushSegVMessage (error);
    avt_PushSegVExit (yagExit, 100);

    yagle_main();
    EXIT(0) ;
}

/****************************************************************************
*                         function yagGetArgs();                            *
****************************************************************************/
/*--------------------------------------------*/
/* Read the command line options              */
/*--------------------------------------------*/

static short 
yagGetArgs(argc, argv)
    int             argc;
    char           *argv[];
{
    int             i;
    short           fileExist = 0;
    char           *car;
    char           *pt;

    if (argc < 2)
    return (-1);

    for (i = 1; i < argc; i++) {
        /*----------------------------+
        | Traitement des options      |
        +----------------------------*/
        if (*(argv[i]) == '-') {

            /*--------------------------+
            | mode trace                |
            +--------------------------*/
            if (strcmp(argv[i], "-t") == 0) {
                avt_setlibloglevel (1,LOGYAG);
                continue;
            }

            /*----------------------+
            | func. analysis depth  |
            +----------------------*/
            else if ((car = optionIdent(argv[i], "-p=")) != NULL) {
                for (pt = car; *pt != '\0'; pt++)
                    if ((*pt < '0') && (*pt > '9')) return (-1);
                YAG_CONTEXT->YAG_DEPTH = atoi(car);
                if (YAG_CONTEXT->YAG_DEPTH == 0) YAG_CONTEXT->YAG_USE_FCF = FALSE;
                continue;
            }
            /*----------------------+
            | figure name           |
            +----------------------*/
            else if ((car = optionIdent(argv[i], "-fig=")) != NULL) {
                YAG_CONTEXT->YAG_FIGNAME = namealloc(car);
                continue;
            }
            /*----------------------+
            | hierarchical mode     |
            +----------------------*/
            else if (strcmp(argv[i], "-hier") == 0) {
                YAG_CONTEXT->YAG_HIERARCHICAL_MODE = TRUE;
                continue;
            }
            /*------------------------+
            | single supply in vbe    |
            +------------------------*/
            else if (strcmp(argv[i], "-s") == 0) {
                YAG_CONTEXT->YAG_ONE_SUPPLY = TRUE;
                continue;
            }
            /*----------------------+
            | inf info file         |
            +----------------------*/
            else if (strcmp(argv[i], "-i") == 0) {
                avt_sethashvar("avtReadInformationFile", inf_GetDefault_AVT_INF_Value());
                continue;
            }
            /*----------------------+
            | no behavioural file   |
            +----------------------*/
            else if (strcmp(argv[i], "-nb") == 0) {
                YAG_CONTEXT->YAG_BEFIG = FALSE;
                continue;
            }
            /*----------------------+
            | hierarchical netlist  |
            +----------------------*/
            else if (strcmp(argv[i], "-ch") == 0) {
                YAG_CONTEXT->YAG_CONE_NETLIST = TRUE;
                continue;
            }
            /*----------------------+
            | old cns netlist file  |
            +----------------------*/
            else if (strcmp(argv[i], "-d") == 0) {
                YAG_CONTEXT->YAG_FILE = TRUE;
                continue;
            }
            /*----------------------+
            | cone netlist file     |
            +----------------------*/
            else if (strcmp(argv[i], "-c") == 0) {
                YAG_CONTEXT->YAG_FILE = TRUE;
                continue;
            }
            /*----------------------+
            | orientation           |
            +----------------------*/
            else if (strcmp(argv[i], "-o") == 0) {
                YAG_CONTEXT->YAG_ORIENT = TRUE;
                continue;
            }
            /*----------------------+
            | hz node traversal     |
            +----------------------*/
            else if (strcmp(argv[i], "-z") == 0) {
                YAG_CONTEXT->YAG_PROP_HZ = TRUE;
                continue;
            }
            /*----------------------+
            | no cell detection     |
            +----------------------*/
            else if (strcmp(argv[i], "-nc") == 0) {
                YAG_CONTEXT->YAG_MAKE_CELLS = FALSE;
                continue;
            }
            /*----------------------+
            | cells can share cones |
            +----------------------*/
            else if (strcmp(argv[i], "-cs") == 0) {
                YAG_CONTEXT->YAG_CELL_SHARE = TRUE;
                continue;
            }
            /*----------------------+
            | no latch detection    |
            +----------------------*/
            else if (strcmp(argv[i], "-nl") == 0) {
                YAG_CONTEXT->YAG_DETECT_LATCHES = FALSE;
                continue;
            }
            /*----------------------+
            | loop analysis         |
            +----------------------*/
            else if (strcmp(argv[i], "-la") == 0) {
                YAG_CONTEXT->YAG_LOOP_ANALYSIS = TRUE;
                continue;
            }
            /*----------------------+
            | bus analysis          |
            +----------------------*/
            else if (strcmp(argv[i], "-ba") == 0) {
                YAG_CONTEXT->YAG_BUS_ANALYSIS = TRUE;
                continue;
            }
            /*----------------------+
            | GENIUS                |
            +----------------------*/
            else if (strcmp(argv[i], "-gns") == 0) {
                YAG_CONTEXT->YAG_GENIUS = TRUE;
                continue;
            }
            else if (strcmp(argv[i], "-xg") == 0) {
                YAG_CONTEXT->YAG_ONLY_GENIUS = TRUE;
                continue;
            }
            /*----------------------+
            | netlist detection     |
            +----------------------*/
            else if (strcmp(argv[i], "-fcl") == 0) {
                YAG_CONTEXT->YAG_FCL_DETECT = TRUE;
                continue;
            }
            else if (strcmp(argv[i], "-xfcl") == 0) {
                YAG_CONTEXT->YAG_FCL_DETECT = TRUE;
                YAG_CONTEXT->YAG_ONLY_FCL = TRUE;
                continue;
            }
            /*----------------------+
            | blackbox elimination  |
            +----------------------*/
            else if (strcmp(argv[i], "-bx") == 0) {
                YAG_CONTEXT->YAG_BLACKBOX_SUPPRESS = TRUE;
                continue;
            }
            /*----------------------+
            | blackbox ignore       |
            +----------------------*/
            else if (strcmp(argv[i], "-bi") == 0) {
                YAG_CONTEXT->YAG_BLACKBOX_IGNORE = TRUE;
                continue;
            }
            /*----------------------+
            | not strict errors     |
            +----------------------*/
            else if (strcmp(argv[i], "-ns") == 0) {
                YAG_CONTEXT->YAG_NOTSTRICT = TRUE;
                continue;
            }
            /*----------------------+
            | remove RC networks    |
            +----------------------*/
            else if (strcmp(argv[i], "-nrc") == 0) {
                YAG_CONTEXT->YAG_NORC = TRUE;
                continue;
            }
            /*----------------------+
            | remove parallel pairs |
            +----------------------*/
            else if (strcmp(argv[i], "-nrp") == 0) {
                YAG_CONTEXT->YAG_REMOVE_PARA = FALSE;
                continue;
            }
            /*----------------------+
            | remove inversions     |
            +----------------------*/
            else if (strcmp(argv[i], "-mi") == 0) {
                YAG_CONTEXT->YAG_MINIMISE_INV = TRUE;
                continue;
            }
            /*----------------------+
            | elp techno file       |
            +----------------------*/
            else if (strcmp(argv[i], "-elp") == 0) {
                YAG_CONTEXT->YAG_ELP = TRUE;
                continue;
            }
            /*----------------------+
            | tas timings           |
            +----------------------*/
            else if (strcmp(argv[i], "-tdmax") == 0) {
                YAG_CONTEXT->YAG_TAS_TIMING = YAG_MAX_TIMING;
                continue;
            }
            else if (strcmp(argv[i], "-tdmin") == 0) {
                YAG_CONTEXT->YAG_TAS_TIMING = YAG_MIN_TIMING;
                continue;
            }
            else if (strcmp(argv[i], "-tdmed") == 0) {
                YAG_CONTEXT->YAG_TAS_TIMING = YAG_MED_TIMING;
                continue;
            }
            /*----------------------+
            | use _s convention     |
            +----------------------*/
            else if (strcmp(argv[i], "-a") == 0) {
                YAG_CONTEXT->YAG_HELP_S = TRUE;
                continue;
            }
            else return (-1);
        }
        if (fileExist == 0) {
            YAG_CONTEXT->YAG_FILENAME = namealloc(argv[i]);
            YAG_CONTEXT->YAG_OUTNAME = YAG_CONTEXT->YAG_FIGNAME;
        }
        else {
            YAG_CONTEXT->YAG_OUTNAME = namealloc(argv[i]);
        }
        fileExist++;
    }

    if (YAG_CONTEXT->YAG_FIGNAME == NULL) {
        YAG_CONTEXT->YAG_FIGNAME = YAG_CONTEXT->YAG_FILENAME;
    }
    if ((fileExist == 0) || (fileExist > 2))
    return (-1);

    return (0);
}

/****************************************************************************
*                         function optionIdent();                          *
****************************************************************************/

static char *
optionIdent(name, chaine)
    char           *name;
    char           *chaine;
{
    int             i = 0;

    for (i = 0; (name[i] != '\0') && (chaine[i] != '\0'); i++)
    if (name[i] != chaine[i])
        break;

    if (chaine[i] != '\0')
    return (NULL);
    else
    return (&name[i]);
}

