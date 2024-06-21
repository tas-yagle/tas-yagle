/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : FCL  v1.02                                                  */
/*    Fichier : fcl_library.c                                               */
/*                                                                          */
/*    (c) copyright 1996 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include "fcl_headers.h"

/*
   ####====================================================================####
   ##                                                                        ##
   ##                         EN TETE DES FONCTIONS                          ##
   ##                                                                        ##
   ####====================================================================####
 */

static long     count_trs();        /* Compte le nombre de TRS d'une lofig */
static void     orient_lofig();
static int      behaviour_exists();

/*
   ####====================================================================####
   ##                                                                        ##
   ##                         VARIABLES GLOBALES                             ##
   ##                                                                        ##
   ####====================================================================####
 */

short           FCL_FILE = 0;
short           FCL_USECUT = 0;
FILE           *FCL_OUTPUT;

char           *FCL_LIB_NAME = NULL;
char           *FCL_LIB_PATH = NULL;

chain_list     *FCL_ANY_NMOS = NULL;
chain_list     *FCL_ANY_PMOS = NULL;

short           FCL_TRACE_LEVEL = 0;

short           FCL_DISPLAY_MATRIX = 0;

num_list       *FCL_ANY_NMOS_IDX = NULL;
num_list       *FCL_ANY_PMOS_IDX = NULL;

char           *FCL_VERSION;

short           FCL_CUT = 0;

int             FCL_SIZE_TOLERANCE = 0;

befig_list     *FCL_HEAD_BEFIG;

chain_list     *FCL_INSTANCE_BEFIGS = NULL;

ht             *FCL_REAL_CORRESP_HT = NULL;

fclcorresp_list *FCL_REAL_CORRESP_LIST = NULL;

/*
   ####====================================================================####
   ##                                                                        ##
   ##                           find_biblio()                                ##
   ##                                                                        ## 
   ## Charge une liste de transistors representant des points memoires a     ##
   ## retrouver dans le circuit et effectue la recherche                     ##
   ##                                                                        ## 
   ####====================================================================####
   ##  entree : pointeur sur la lofig du circuit                             ##
   ##         : pointeur sur le nom du fichier de bibliotheque               ## 
   ##  sortie : une variable d'etat                                          ##
   ####====================================================================####
 */

cell_list     *
fclFindCells(ptlofig_c)
    lofig_list     *ptlofig_c;
{
    FILE           *ptfile;        /* Ptr sur le fichier biblio     */
    char            filemem[100];
    lofig_list     *ptlofig_m;        /* ptr sur la netlist d'un model */
    befig_list     *ptbefig_m;        /* ptr sur la befig d'un model   */
    lofig_list     *ptlofig;        /* ptr sur la netlist du circuit */
    ptype_list     *ptuser;
    chain_list     *ptmodelchain = NULL;
    chain_list     *ptchain, *ptprevchain;
    long            i = 128;        /* Numero du modele              */
    long            nb_trs;        /* Nb de transistors             */

    char            save_in_lo[5];        /* ancien format de la netlist   */
    char           *save_work_lib;        /* ancien work lib               */

    char            mode;
    int             order;
    int             num;
    inffig_list    *ifl;
    char *oldconfig;


    ifl=getloadedinffig(ptlofig_c->NAME);

    FCL_NEED_ZERO = FALSE;
    FCL_NEED_ONE = FALSE;

    /* Ouverture de fichier rapport */
    if (FCL_FILE) {
        time_t          counter;

        FCL_OUTPUT = mbkfopen(ptlofig_c->NAME, "fcl", WRITE_TEXT);
        if (FCL_OUTPUT == NULL) {
            char            buff[80];

            sprintf(buff, "%s.fcl", ptlofig_c->NAME);
            fprintf(stderr, "\n *** FCL error: Can't open '%s' ***\n", buff);
            FCL_FILE = FALSE;
        }
        else {
            fprintf(FCL_OUTPUT, "FCL V%s %s.fcl", FCL_VERSION, ptlofig_c->NAME);
            time(&counter);
            fprintf(FCL_OUTPUT, " %s\n\n", ctime(&counter));
        }
    }

    /* select library path */
    save_work_lib = WORK_LIB;
    WORK_LIB = FCL_LIB_PATH;

    /* Test si le fichier bibliotheque existe */
    if (!(ptfile = mbkfopen(FCL_LIB_NAME, (char *) NULL, READ_TEXT))) {
        WORK_LIB = save_work_lib;
        return NULL;
    }

    /* Passe en format SPI */
    strcpy(save_in_lo, IN_LO);
    strcpy(IN_LO, "spi");
      
    oldconfig=V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE;
    avt_sethashvar_sub("avtSpiKeepNames", "All", 0, AVT_VAR_SET_BY_TCLCONFIG);

    /* ******************************************************************* *
     * Chargement des netlists presentent dans le fichier de bibliotheque  *
     * ******************************************************************* */

    /* user extension to mbkspice */
    fclExtendSpice();

    do {
        mode = 'A';
        if ((num = fscanf(ptfile, "%s %c %d", filemem, &mode, &order)) == 2) order = 0;
        /* modes 'A', 'P' and 'M' are legal */

        if (num >= 2) {
            ptlofig_m = addlofig(filemem);
            loadlofig(ptlofig_m, filemem, 'A');
            if (behaviour_exists(filemem) && (mode == 'A' || mode == 'P')) {
                if (mode == 'A') {
                    ptbefig_m = loadbefig(NULL, filemem, 0);
                    if (ptbefig_m->NAME != ptlofig_m->NAME) 
                        fprintf (stderr, "\n *** FCL warning: pattern '%s' and model '%s' mismatch - model ignored\n", 
                                          ptlofig_m->NAME, ptbefig_m->NAME);
                }
                else {
                    ptbefig_m = loadbefig(NULL, filemem, BVL_CHECKEMPTY);
                }
                ptbefig_m->NEXT = FCL_HEAD_BEFIG;
                FCL_HEAD_BEFIG = ptbefig_m;

                orient_lofig(ptlofig_m, ptbefig_m);
                if (mode == 'P') {
                    while (ptbefig_m->BEOUT != NULL) {
                        ptbefig_m->BEOUT = beh_delbeout(ptbefig_m->BEOUT, ptbefig_m->BEOUT, 'N');
                    }
                    FCL_HEAD_BEFIG = beh_delbefig(FCL_HEAD_BEFIG, ptbefig_m, 'Y');
                }
            }
            if (mode == 'M') {
                ptlofig_m->USER = addptype(ptlofig_m->USER, FCL_MARK_PTYPE, NULL);
            }

            /* On charge les netlists des modeles                            */
            lofigchain(ptlofig_m);
            yagFindSupplies(ifl, ptlofig_m, TRUE);
            yagFindInternalSupplies(ifl, ptlofig_m, TRUE);
            yagTestTransistors(ptlofig_m, TRUE);

            /* A chaque Model correspond un numero de model                  */
            ptlofig_m->USER = addptype(ptlofig_m->USER, FCL_MODEL_PTYPE, (void *) i++);
            /* Pour chaque model, un ptype est ajoute avec son nombre de TRS ou -ORDER*/
            if (order == 0) nb_trs = count_trs(ptlofig_m);
            else nb_trs = LONG_MAX - order;
            ptlofig_m->USER = addptype(ptlofig_m->USER, FCL_NUMTRANS_PTYPE, (void *) nb_trs);

            /* Les points memoires sont classes par ordre croissant */
            if (ptmodelchain == NULL) ptmodelchain = addchain(NULL, ptlofig_m);
            else {
                ptprevchain = NULL;
                for (ptchain = ptmodelchain; ptchain; ptchain = ptchain->NEXT) {
                    ptuser = getptype(((lofig_list *)ptchain->DATA)->USER, FCL_NUMTRANS_PTYPE);
                    if ((long)ptuser->DATA <= nb_trs) break;
                    ptprevchain = ptchain;
                }
                if (ptprevchain == NULL) {
                    ptmodelchain = addchain(ptmodelchain, ptlofig_m);
                }
                else ptprevchain->NEXT = addchain(ptchain, ptlofig_m);
            }
        }
    }
    while (!feof(ptfile));

    fclose(ptfile);

    FCL_CELL_LIST = NULL;
    FCL_INSTANCE_BEFIGS = NULL;
    FCL_LOCON_LIST = NULL;

    /* ****************************************** *
     * La recherche est faite par ordre croissant * 
     * ****************************************** */

    for (ptchain = ptmodelchain; ptchain; ptchain = ptchain->NEXT) {
        ptlofig = (lofig_list *)ptchain->DATA;

        if (FCL_TRACE_LEVEL > 0) {
            printf("%s\n", ptlofig->NAME);
        }

        if (FCL_FILE) {
            fprintf(FCL_OUTPUT, "%s\n\n", ptlofig->NAME);
        }

        ptbefig_m = fclGetBefig(ptlofig->NAME);
        if (!FCL_USECUT) FCL_BUILD_CELLS = TRUE;
        else FCL_BUILD_CELLS = FALSE;

        if (getptype(ptlofig->USER, FCL_MARK_PTYPE) != NULL) {
            FCL_BUILD_CELLS = FALSE;
            ptlofig->USER = delptype(ptlofig->USER, FCL_MARK_PTYPE);
        }
        if (ptbefig_m != NULL && FCL_USECUT) {
            FCL_INSTANCE_LIST = NULL;
            FCL_BUILD_INSTANCES = TRUE;
            fclFind(ptlofig_c, ptlofig, ptbefig_m);
            fclMarkInstances(ptlofig_c, FCL_INSTANCE_LIST, FALSE, FALSE);
            freechain(FCL_INSTANCE_LIST);
            FCL_BUILD_INSTANCES = FALSE;
        }
        else {
            fclFind(ptlofig_c, ptlofig, ptbefig_m);
        }
        dellofig(ptlofig->NAME);
    }
    
    FCL_BUILD_CELLS = FALSE;

    /* restore file format */
    strcpy(IN_LO, save_in_lo);

    avt_sethashvar_sub("avtSpiKeepNames", oldconfig, 0, AVT_VAR_SET_BY_TCLCONFIG);

    /* restore working directory */
    WORK_LIB = save_work_lib;

    /* remove extension */
    fclRmvSpiceExtensions();
    /* remove share markings */
    fclCleanShareMarks(ptlofig_c);

/*---------------------------------------------------------------------------*
| Close files                                                                |
*----------------------------------------------------------------------------*/

    if (FCL_FILE) {
        fclose(FCL_OUTPUT);
    }

    return FCL_CELL_LIST;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                           count_trs()                                  ##
   ##                                                                        ## 
   ## Compte le nombre de transistors present dans une netlist               ##
   ##                                                                        ## 
   ####====================================================================####
 */
static long
count_trs(ptlofig_m)
    lofig_list     *ptlofig_m;
{
    lotrs_list     *ptlotrs;
    long            i = 0;

    for (ptlotrs = ptlofig_m->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT)
        i++;

    return i;
}

static void
orient_lofig(ptlofig, ptbefig)
    lofig_list     *ptlofig;
    befig_list     *ptbefig;
{
    bepor_list     *ptbepor;
    locon_list     *ptlocon;

    for (ptbepor = ptbefig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
        ptlocon = getlocon(ptlofig, ptbepor->NAME);
        if (ptlocon != NULL)
            ptlocon->DIRECTION = ptbepor->DIRECTION;
    }
}

static int
behaviour_exists(figname)
    char           *figname;
{
    FILE           *fp;

    if ((fp = mbkfopen(figname, bef_getsuffix(), READ_TEXT)) != NULL) {
        fclose(fp);
        return TRUE;
    }
    else {
        return FALSE;
    }
}

befig_list     *
fclGetBefig(name)
    char           *name;
{
    befig_list     *ptbefig;

    for (ptbefig = FCL_HEAD_BEFIG; ptbefig; ptbefig = ptbefig->NEXT) {
        if (ptbefig->NAME == name) {
            return ptbefig;
        }
    }
    return NULL;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                           fclFindInstances                             ##
   ##                                                                        ## 
   ## Find all instances of a given model                                    ##
   ##                                                                        ## 
   ####====================================================================####
 */

void fclUpdateLofigchain(chain_list *inslist, char *figurename)
{
    locon_list     *ptlocon, *ptsigcon;
    ptype_list     *ptuser;
    chain_list     *ptchain, *ptchain1, *ptnewchain;
    int             preserve;
    /* create false lofigchain */
    for (ptchain = inslist; ptchain; ptchain = ptchain->NEXT) {
        ptuser = getptype(((loins_list *)ptchain->DATA)->USER, FCL_TRANSLIST_PTYPE);
        if (ptuser != NULL) fclMarkTrans((chain_list *)ptuser->DATA);
    }

    for (ptchain = inslist; ptchain; ptchain = ptchain->NEXT) {
        for (ptlocon = ((loins_list *)ptchain->DATA)->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            ptnewchain = NULL;
            preserve = FALSE;
            ptuser = getptype(ptlocon->SIG->USER, LOFIGCHAIN);
            if (ptuser != NULL) {
                for (ptchain1 = (chain_list *)ptuser->DATA; ptchain1; ptchain1 = ptchain1->NEXT) {
                    ptsigcon = (locon_list *)ptchain1->DATA;
                    if (ptsigcon->TYPE == 'I') { /* check if already visited */
                        if (((loins_list *)ptsigcon->ROOT)->FIGNAME == figurename) {
                            ptnewchain = (chain_list *)ptuser->DATA;
                            preserve = TRUE;
                            break;
                        }
                    }
                    if (ptsigcon->TYPE == 'T') {
                        if (isFclMarked(((lotrs_list *)ptsigcon->ROOT))) continue;
                    }
                    ptnewchain = addchain(ptnewchain, ptsigcon);
                }
                ptnewchain = addchain(ptnewchain, ptlocon);
                if (!preserve) freechain(ptuser->DATA);
                ptuser->DATA = ptnewchain;
            }
        }
    }

    for (ptchain = inslist; ptchain; ptchain = ptchain->NEXT) {
        ptuser = getptype(((loins_list *)ptchain->DATA)->USER, FCL_TRANSLIST_PTYPE);
        if (ptuser != NULL) fclUnmarkTrans((chain_list *)ptuser->DATA);
    }
}

chain_list *
fclFindInstances(ptlofig_c, ptlofig_m)
    lofig_list     *ptlofig_c;
    lofig_list     *ptlofig_m;
{
    time_t          start = 0;
    time_t          end = 0;
    struct rusage   END;
    struct rusage   START; 
    inffig_list    *ifl;
    /* Ouverture de fichier rapport */
    if (FCL_FILE) {
        time_t          counter;

        FCL_OUTPUT = mbkfopen(ptlofig_m->NAME, "fclins", WRITE_TEXT);
        if (FCL_OUTPUT == NULL) {
            char            buff[80];

            sprintf(buff, "%s.fcl", ptlofig_c->NAME);
            fprintf(stderr, " *** FCL error : Can't open '%s' ***\n", buff);
        }
        fprintf(FCL_OUTPUT, "FCL V%s %s.fclins", FCL_VERSION, ptlofig_m->NAME);
        time(&counter);
        fprintf(FCL_OUTPUT, " %s\n\n", ctime(&counter));
    }

    ifl=getloadedinffig(ptlofig_c->NAME);
    FCL_INSTANCE_LIST = NULL;
    FCL_BUILD_INSTANCES = TRUE;

    while (ptlofig_m->LOINS != NULL)
        flattenlofig(ptlofig_m, ptlofig_m->LOINS->INSNAME, 'Y');

    lofigchain(ptlofig_m);
    yagFindSupplies(ifl, ptlofig_m, TRUE);
    yagFindInternalSupplies(ifl,ptlofig_m, TRUE);
    yagTestTransistors(ptlofig_m, TRUE);

    fclChrono(&START,&start);
    fclFind(ptlofig_c, ptlofig_m, fclGetBefig(ptlofig_m->NAME));

    /* create false lofigchain */
    fclUpdateLofigchain(FCL_INSTANCE_LIST, ptlofig_m->NAME);

    FCL_INSTANCE_LIST = reverse(FCL_INSTANCE_LIST);
    FCL_BUILD_INSTANCES = FALSE;

    if (FCL_FILE) {
        fclose(FCL_OUTPUT);
    }

    if (FCL_TRACE_LEVEL > 0) {
        printf("\n[FCL] Identification of all '%s' took: ", ptlofig_m->NAME);
        fclChrono(&END,&end);
        fclPrintTime(&START,&END,start,end);
    }

    return FCL_INSTANCE_LIST;
}

static chain_list *AddOrLeave(chain_list *cl, char *name)
{
  chain_list *ch=cl;
  while (cl!=NULL && cl->DATA!=name) cl=cl->NEXT;
  if (cl==NULL) return addchain(ch, name);
  return ch;
}

/*
   ####====================================================================####
   ##                                                                        ##
   ##                           fclFindCorrespondance                        ##
   ##                                                                        ## 
   ## Find all correspondances after a remapping of a model interface        ##
   ##                                                                        ## 
   ####====================================================================####
 */
fclcorresp_list *
fclFindCorrespondance(lofig_list *ptlofig_m)
{
    fclcorresp_list *rescorresp; 
    locon_list      *ptlocon;
    loins_list      *ptloins;
    losig_list      *ptlosig;
    losig_list      *ptkey = NULL;
    partition_list  *ptsolution;
    ptype_list      *ptuser, *ptuser2;
    long             index;
    time_t          start = 0;
    time_t          end = 0;
    struct rusage   END;
    struct rusage   START;

    if (FCL_TRACE_LEVEL > 0) {
        printf("\n[FCL] Starting Correspondance mapping of '%s'\n", ptlofig_m->NAME);
        fclChrono(&START,&start);
    }

    if (FCL_FILE) {
        time_t          counter;

        FCL_OUTPUT = mbkfopen(ptlofig_m->NAME, "fclcor", WRITE_TEXT);
        if (FCL_OUTPUT == NULL) {
            char            buff[80];

            sprintf(buff, "%s.fclcor", ptlofig_m->NAME);
            fprintf(stderr, " *** FCL error : Can't open '%s' ***\n", buff);
        }
        fprintf(FCL_OUTPUT, "FCL V%s %s.fclcor", FCL_VERSION, ptlofig_m->NAME);
        time(&counter);
        fprintf(FCL_OUTPUT, " %s\n\n", ctime(&counter));
    }

    FCL_BUILD_CORRESP = TRUE;

    FCL_REAL_CORRESP_HT = addht(20);
    FCL_REAL_CORRESP_LIST = NULL;

    /* create interface correspondance hash table and select key/solution */
    for (ptlocon = ptlofig_m->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptuser = getptype(ptlocon->USER, FCL_REAL_CORRESP_PTYPE);
        if (ptuser == NULL) {
            if ((ptuser2 = getptype(ptlocon->SIG->USER, FCL_TRANSFER_PTYPE)) == NULL) {
                ptlocon->SIG->USER = addptype(ptlocon->SIG->USER, FCL_TRANSFER_PTYPE, (void *)FCL_MATCHNAME);
            }
            else ptuser2->DATA = (void *)((long)ptuser2->DATA | FCL_MATCHNAME);
            fclConstrainConnectedInstances(ptlocon->SIG, ptlocon->SIG);
            continue;
        }
        ptlosig = (losig_list *)ptuser->DATA;
        addhtitem(FCL_REAL_CORRESP_HT, ptlocon->NAME, (long)ptlosig->NAMECHAIN->DATA);
        fclConstrainConnectedInstances(ptlocon->SIG, ptlosig);
        if (ptkey == NULL && ptlocon->NAME != (char *)ptlosig->NAMECHAIN->DATA) {
            ptkey = ptlocon->SIG;
            ptsolution = addpartition(NULL, ptlosig, 0, 'S');
        }
    }
    if (ptkey == NULL) {
        ptkey = ptlofig_m->LOCON->SIG;
        ptsolution = addpartition(NULL, ptlofig_m->LOCON->SIG, 0, 'S');
    }

    /* Mark couplings on signals */
    for (ptloins = ptlofig_m->LOINS; ptloins; ptloins = ptloins->NEXT) {
        for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            if ((ptuser = getptype(ptlocon->USER, FCL_COUPLING_PTYPE)) == NULL) continue;
            index = (long)ptuser->DATA;
            if ((ptuser = getptype(ptlocon->SIG->USER, FCL_COUP_LIST_PTYPE)) != NULL) {
                ptuser->DATA = addnum((num_list *)ptuser->DATA, index);
            }
            else ptlocon->SIG->USER = addptype(ptlocon->SIG->USER, FCL_COUP_LIST_PTYPE, addnum(NULL, index));
        }
    }

    fclPhase2(ptlofig_m, ptlofig_m, NULL, ptsolution, ptkey, FALSE);
    fclCleanShareMarks(ptlofig_m);
    fclCleanTransferMarks(ptlofig_m);
    fclCleanCouplingMarks(ptlofig_m);
    delht(FCL_REAL_CORRESP_HT);
    FCL_REAL_CORRESP_HT = NULL;
    rescorresp = FCL_REAL_CORRESP_LIST;
    FCL_REAL_CORRESP_LIST = NULL;
    FCL_BUILD_CORRESP = FALSE;
    if (FCL_FILE) {
        fclose(FCL_OUTPUT);
    }

    if (FCL_TRACE_LEVEL > 0) {
        printf("\n[FCL] Correspondance mapping of '%s' took: ", ptlofig_m->NAME);
        fclChrono(&END,&end);
        fclPrintTime(&START,&END,start,end);
    }

    return rescorresp;
}

void 
fclenv()
{
    chain_list     *ptchain;
    char           *str = NULL;
    char            buf[1024];

    buf[0] = 0;

    FCL_VERSION = namealloc("1.10");

    str = V_STR_TAB[__FCL_LIB_NAME].VALUE;
    if (FCL_LIB_NAME!=NULL) mbkfree(FCL_LIB_NAME);
    if (str != NULL) {
        FCL_LIB_NAME = mbkstrdup(str);
    }
    else {
        FCL_LIB_NAME = mbkalloc(strlen("LIBRARY") + 1);
        strcpy(FCL_LIB_NAME, "LIBRARY");
    }

    str = V_STR_TAB[__FCL_LIB_PATH].VALUE;
    if (FCL_LIB_PATH!=NULL) mbkfree(FCL_LIB_PATH);
    if (str != NULL) {
        FCL_LIB_PATH = mbkstrdup(str);
    }
    else {
        FCL_LIB_PATH = mbkalloc(strlen(WORK_LIB) + strlen("/cells") + 1);
        strcpy(FCL_LIB_PATH, WORK_LIB);
        strcat(FCL_LIB_PATH, "/cells");
    }

            FCL_FILE = V_BOOL_TAB[__FCL_FILE].VALUE;

            FCL_USECUT = V_BOOL_TAB[__FCL_USECUT].VALUE;

        FCL_SIZE_TOLERANCE = V_INT_TAB[__FCL_SIZE_TOLERANCE].VALUE;

        FCL_TRACE_LEVEL = V_INT_TAB[__FCL_TRACE_LEVEL].VALUE;

    str = V_STR_TAB[__FCL_ANY_NMOS].VALUE;
    if (str != NULL && strlen(str) < 1024) {
        char *pttok;
        strcpy(buf, str);
        FCL_ANY_NMOS = addchain(FCL_ANY_NMOS, namealloc(strtok(buf, ":")));
        while ((pttok = strtok(NULL, ":"))) {
            FCL_ANY_NMOS = addchain(FCL_ANY_NMOS, namealloc(pttok));
        }
    }
    else {
        FCL_ANY_NMOS = addchain(FCL_ANY_NMOS, namealloc("tn"));
        FCL_ANY_NMOS = addchain(FCL_ANY_NMOS, namealloc("TN"));
    }
    for (ptchain = FCL_ANY_NMOS; ptchain; ptchain = ptchain->NEXT) {
        FCL_ANY_NMOS_IDX = addnum(FCL_ANY_NMOS_IDX, (long)addlotrsmodel(NULL, (char *)ptchain->DATA));
        TNMOS = AddOrLeave(TNMOS, (char *)ptchain->DATA);
    }

    str = V_STR_TAB[__FCL_ANY_PMOS].VALUE;
    if (str != NULL && strlen(str) < 1024) {
        char *pttok;
        strcpy(buf, str);
        FCL_ANY_PMOS = addchain(FCL_ANY_PMOS, namealloc(strtok(buf, ":")));
        while ((pttok = strtok(NULL, ":"))) {
            FCL_ANY_PMOS = addchain(FCL_ANY_PMOS, namealloc(pttok));
        }
    }
    else {
        FCL_ANY_PMOS = addchain(FCL_ANY_PMOS, namealloc("tp"));
        FCL_ANY_PMOS = addchain(FCL_ANY_PMOS, namealloc("TP"));
    }
    for (ptchain = FCL_ANY_PMOS; ptchain; ptchain = ptchain->NEXT) {
        FCL_ANY_PMOS_IDX = addnum(FCL_ANY_PMOS_IDX, (long)addlotrsmodel(NULL, (char *)ptchain->DATA));
        TPMOS = AddOrLeave(TPMOS, (char *)ptchain->DATA);
    }

    str = getenv("FCL_DISPLAY_MATRIX");
    if (str != NULL) {
        if ((str[0] == 'y') || (str[0] == 'Y')) {
            FCL_DISPLAY_MATRIX = TRUE;
        }
        else
            FCL_DISPLAY_MATRIX = FALSE;
    }
    else
        FCL_DISPLAY_MATRIX = FALSE;

}
