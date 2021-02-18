/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_instance.c                                              */
/*                                                                          */
/*    (c) copyright 1993 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc LAURENTIN                        le : 27/01/1993     */
/*                                                                          */
/*    Modifie par : Anthony LESTER                      le : 02/05/1995     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static void calcVarOccur __P((cone_list *ptcone));
static void calcVarWeights __P((cone_list *ptcone));
static void cleanWeightsIncone __P((cone_list *ptcone));
static int cmpOccur __P((edge_list *in1, edge_list *in2));
static void addPathModel __P((branch_list *ptbranch));
static void addConePathModel __P((cone_list *ptcone));
static void cleanConePathWeights __P((cone_list *ptcone));
static short cmpPathModel __P((char *exp1, char *exp2, int signe));
static void addConeModel __P((cone_list *ptcone));

/****************************************************************************
 *                         fonction yagMakeConeModel();                     *
 ****************************************************************************/
void
yagMakeConeModel(ptcone)
    cone_list      *ptcone;
{
    calcVarOccur(ptcone);
    calcVarWeights(ptcone);
    addConePathModel(ptcone);
    addConeModel(ptcone);
    cleanConePathWeights(ptcone);
    cleanWeightsIncone(ptcone);
}


/****************************************************************************
 *                         fonction calcVarOccur();                         *
 ****************************************************************************/
static void
calcVarOccur(ptcone)
    cone_list      *ptcone;
{
    branch_list    *brlist[4];
    branch_list    *ptbranch;
    link_list      *ptlink;
    cone_list      *ptincone;
    locon_list     *ptcon;
    ptype_list     *ptuser;
    biint          *occur;
    int             i;

   /*------------------------------------------------+
   | On incremente l'occurence de chaque variable    |
   +------------------------------------------------*/
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_NOT_FUNCTIONAL|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
            if (i == 2 && ptbranch->LINK->NEXT == NULL) continue;
            for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {

                if ((ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK)) != 0) {
                    ptincone = (cone_list *)getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE)->DATA;
                    ptuser = getptype(ptincone->USER, YAG_WEIGHT_PTYPE);
                    if (ptuser == NULL) {
                        occur = (biint *) mbkalloc(sizeof(biint));
                        occur->NEG = 0;
                        occur->POS = 0;
                        ptincone->USER = addptype(ptincone->USER, YAG_WEIGHT_PTYPE, (void *)occur);
                    }
                    else occur = (biint *)ptuser->DATA;

                    if ((ptlink->TYPE & CNS_TNLINK) != 0) occur->NEG++;
                    else occur->POS++;
                }
                else { /* maillon connecteur */
                    ptcon = ptlink->ULINK.LOCON;
                    ptuser = getptype(ptcon->USER, YAG_WEIGHT_PTYPE);
                    if (ptuser == NULL) {
                        occur = (biint *) mbkalloc(sizeof(biint));
                        occur->NEG = 0;
                        occur->POS = 1;
                        ptcon->USER = addptype(ptcon->USER, YAG_WEIGHT_PTYPE, (void *)occur);
                    }
                    else {
                        occur = (biint *)ptuser->DATA;
                        occur->POS++;
                    }
                }
            }
        }
    }
}

/****************************************************************************
 *                         fonction calcVarWeights();                        *
 ****************************************************************************/
static void
calcVarWeights(ptcone)
/*-------------------------------------------------------------------------+
| On donne le poids + eleve  aux variable d'occurence la + elevee          |
| En cas d'egalite c'est le premier qui prendra le poid le + faible        |
| Le poids est range dans le user 'YAG_WEIGHT_PTYPE' du cone/connecteur    |
| d'entree, PUIS dans le user 'YAG_WEIGHT_PTYPE' du incone designant le    |
| cone/connecteur                                                          |
+-------------------------------------------------------------------------*/
    cone_list      *ptcone;
{
    table          *tabIn;
    edge_list      *ptincone;
    ptype_list     *ptuser;
    long            poids = 1;
    int             i = 0;
    int             j = 0;

   /*-----------------------------------+
   | On constitue le tableau de incones |
   +-----------------------------------*/
    tabIn = newtable();
    for (ptincone = ptcone->INCONE; ptincone != NULL; ptincone = ptincone->NEXT) {
        if ((ptincone->TYPE & CNS_IGNORE) != 0) continue;
        if ((ptincone->TYPE & YAG_NOT_FUNCTIONAL) != 0) continue;
        if ((ptincone->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue;
        if ((ptincone->TYPE & (CNS_CONE|CNS_POWER)) != 0) {
            if (getptype(ptincone->UEDGE.CONE->USER, YAG_WEIGHT_PTYPE) == NULL) {
                continue;
            }
        }
        else {
            if (getptype(ptincone->UEDGE.LOCON->USER, YAG_WEIGHT_PTYPE) == NULL) {
                continue;
            }
        }
        addtableitem(tabIn, ptincone);
    }

   /*-----------------------------------------+
   | On effectue le tri sur le tableau de in  |
   +-----------------------------------------*/
    for (i=0; i < tabIn->SIZE-1; i++) {
        for (j=0; j < tabIn->SIZE-1 - i; i++) {
            if (cmpOccur(tabIn->DATA[j], tabIn->DATA[j+1]) == 1) {
                edge_list *save = tabIn->DATA[j];
                tabIn->DATA[j] = tabIn->DATA[j+1];
                tabIn->DATA[j+1] = save;
            }
        }
    }

   /*------------------------------------------------+
   | le poids est determine puis range dans le user  |
   | du cone : il remplace l'occurence biint         |
   | on le recopie aussi dans le user du incone      |
   +------------------------------------------------*/
    for (i=0; i<tabIn->SIZE; i++) {
        ptincone = tabIn->DATA[i];
        if ((ptincone->TYPE & (CNS_CONE|CNS_POWER)) != 0) {
            ptuser = getptype(ptincone->UEDGE.CONE->USER, YAG_WEIGHT_PTYPE);
        }
        else {
            ptuser = getptype(ptincone->UEDGE.LOCON->USER, YAG_WEIGHT_PTYPE);
        }
        mbkfree(ptuser->DATA);
        ptuser->DATA = (void *)poids;
        ptincone->USER = addptype(ptincone->USER, YAG_WEIGHT_PTYPE, (void *)poids);
        poids++;
    }

    deltable(tabIn);
    return;
}

/****************************************************************************
 *                         fonction cleanWeightsIncone();                       *
 ****************************************************************************/
static void
cleanWeightsIncone(ptcone)
    cone_list      *ptcone;
{
    edge_list      *in;

    for (in = ptcone->INCONE; in != NULL; in = in->NEXT) {
        if ((in->TYPE & CNS_IGNORE) != 0) continue;
        if ((in->TYPE & YAG_NOT_FUNCTIONAL) != 0) continue;
        if ((in->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue;
        if ((in->TYPE & (CNS_CONE|CNS_POWER)) != 0) {
            if (getptype(in->UEDGE.CONE->USER, YAG_WEIGHT_PTYPE) != NULL) {
                in->UEDGE.CONE->USER = delptype(in->UEDGE.CONE->USER, YAG_WEIGHT_PTYPE);
            }
        }
        else {
            if (getptype(in->UEDGE.LOCON->USER, YAG_WEIGHT_PTYPE) != NULL) {
                in->UEDGE.LOCON->USER = delptype(in->UEDGE.LOCON->USER, YAG_WEIGHT_PTYPE);
            }
        }
    }
}

/****************************************************************************
 *                         fonction cmpOccur();                            *
 ****************************************************************************/
static int
cmpOccur(in1, in2)
    edge_list      *in1;
    edge_list      *in2;
{
    biint          *oc1, *oc2;

    if ((in1->TYPE & (CNS_CONE|CNS_POWER)) != 0) {
        oc1 = (biint *)getptype(in1->UEDGE.CONE->USER, YAG_WEIGHT_PTYPE)->DATA;
    }
    else {
        oc1 = (biint *)getptype(in1->UEDGE.LOCON->USER, YAG_WEIGHT_PTYPE)->DATA;
    }

    if ((in2->TYPE & (CNS_CONE|CNS_POWER)) != 0) {
        oc2 = (biint *)getptype(in2->UEDGE.CONE->USER, YAG_WEIGHT_PTYPE)->DATA;
    }
    else {
        oc2 = (biint *)getptype(in2->UEDGE.LOCON->USER, YAG_WEIGHT_PTYPE)->DATA;
    }

    if (oc1->POS > oc2->POS) return (1);
    else if (oc1->POS < oc2->POS) return (2);
    else {
        if (oc1->NEG > oc2->NEG) return (1);
        else if (oc1->NEG < oc2->NEG) return (2);
        else return (0);
    }
}

/****************************************************************************
 *                         fonction addPathModel();                         *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
| exprime la fonctionalite de la branche enfonction du poids des variable   |
| Les variable de poids les plus faibles sont a gauche des variables de     |
| poids + eleve. Le resultat est un char * qui est range dans le USER       |
| YAG_WEIGHT_PTYPE du pointeur de branche 'path'                                   |
+--------------------------------------------------------------------------*/
static void
addPathModel(ptbranch)
    branch_list  *ptbranch;
{
    cone_list      *ptincone;
    locon_list     *ptcon;
    link_list      *ptlink;
    ptype_list     *ptuser;
    long            tabVar[64];
    short           idx = 0;
    short           j = 0;
    short           iMax = 0;
    char           *expr;
    char            buff[YAGBUFSIZE], tmp[20];
    int             save;

    buff[0] = '\0';

    /*----------------------------------------------------------+
   | On constitue le tableau exprimant le produit de variables |
   | que l'on ordone ensuite en fonction du poids              |
   +----------------------------------------------------------*/
    for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {

        if ((ptlink->TYPE & CNS_TPLINK) == CNS_TPLINK) {
            ptincone = (cone_list *)getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE)->DATA;
            ptuser = getptype(ptincone->USER, YAG_WEIGHT_PTYPE);
            if (ptuser == NULL) return;
            tabVar[idx++] = -((long)ptuser->DATA);
        }
        else if ((ptlink->TYPE & CNS_TNLINK) == CNS_TNLINK) {
            ptincone = (cone_list *)getptype(ptlink->ULINK.LOTRS->USER, CNS_DRIVINGCONE)->DATA;
            ptuser = getptype(ptincone->USER, YAG_WEIGHT_PTYPE);
            if (ptuser == NULL) return;
            tabVar[idx++] = ((long)ptuser->DATA);
        }
        else { /* maillon connecteur */
            ptcon = ptlink->ULINK.LOCON;
            ptuser = getptype(ptcon->USER, YAG_WEIGHT_PTYPE);
            if (ptuser == NULL) return;
            tabVar[idx++] = ((long)ptuser->DATA);
        }
    }

    iMax = idx;

   /*-----------------------------------------+
   | On effectue le tri sur le tableau de Var |
   +-----------------------------------------*/
    for (idx = 0; idx < iMax-1; idx++) {
        for (j = 0; j < iMax-1-idx; j++) {
            if (tabVar[j] > tabVar[j+1]) {
                save = tabVar[j];
                tabVar[j] = tabVar[j+1];
                tabVar[j+1] = save;
            }
        }
    }

   /*---------------------------------------------+
   | On construit l'expression  du terme          |
   +---------------------------------------------*/
    for (idx = 0; idx < iMax; idx++) {
        if (tabVar[idx] >= 0) {
            sprintf(tmp, "+%ld", tabVar[idx]);
        }
        else {
            sprintf(tmp, "%ld", tabVar[idx]);
        }
        strcat(buff, tmp);
    }

    expr = (char *)mbkalloc(strlen(buff)+1);
    strcpy(expr, buff);

   /*-------------------------------------------+
   | On range l'expression dans le USER du path |
   +-------------------------------------------*/
    ptbranch->USER = addptype(ptbranch->USER, YAG_WEIGHT_PTYPE, (void *)expr);
}

/****************************************************************************
 *                         fonction addConePathModel();                     *
 ****************************************************************************/
static void
addConePathModel(ptcone)
    cone_list      *ptcone;
{
    branch_list      *brlist[4];
    branch_list      *ptbranch;
    int               i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_BLEEDER|CNS_FEEDBACK|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
            if (i == 2 && ptbranch->LINK->NEXT == NULL) continue;
            addPathModel(ptbranch);
        }
    }
}

/****************************************************************************
 *                         fonction cleanConePathWeights();                   *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
| nettoie tous les user 'YAG_WEIGHT_PTYPE' des branche du cone                     |
+--------------------------------------------------------------------------*/
static void
cleanConePathWeights(ptcone)
    cone_list      *ptcone;
{
    branch_list      *brlist[4];
    branch_list      *ptbranch;
    ptype_list       *ptuser;
    int               i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptuser = getptype(ptbranch->USER, YAG_WEIGHT_PTYPE)) != NULL) {
                mbkfree(ptuser->DATA);
                ptbranch->USER = delptype(ptbranch->USER, YAG_WEIGHT_PTYPE);
            }
        }
    }
}

/****************************************************************************
 *                         fonction cmpPathModel();                         *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
| compare les expression modeles des path en vue de les classer             |
| retourne 1 si exp1 > exp2                                                 |
|          2 si exp1 < exp2                                                 |
|          0 si exp1 = exp2                                                 |
+--------------------------------------------------------------------------*/
static short
cmpPathModel(exp1, exp2, signe)
    char           *exp1;
    char           *exp2;
    int             signe;
{
    char           *p1;
    char           *p2;

    p1 = exp1;
    p2 = exp2;

    if (*p1 == '+' || *p1 == '-' || *p1 == '\0' ||
        *p2 == '+' || *p2 == '-' || *p2 == '\0') {
      /*----------------+
      | cas  '+' vs 'x' |
      +----------------*/
        if (*p1 == '+' && *p2 == '+') {
            p1++;
            p2++;
            return (cmpPathModel(p1, p2, 1));
        }
        else if (*p1 == '+') { /* *p2=='-' || *p2=='\0' */
            return (1);
        }
      /*----------------+
      | cas  '-' vs 'x' |
      +----------------*/
        else if (*p1 == '-' && *p2 == '+') {
            return (2);
        }
        else if (*p1 == '-' && *p2 == '-') {
            p1++;
            p2++;
            return (cmpPathModel(p1, p2, -1));
        }
        else if (*p1 == '-' && *p2 == '\0') {
            return (1);
        }
      /*-----------------+
      | cas  '\0' vs 'x' |
      +-----------------*/
        else if (*p1 == '\0' && *p2 == '\0') {
            return (0);
        }
        else if (*p1 == '\0') { /*  *p2=='+' || *p2=='-'  */
            return (2);
        }
        else {
            printf("[err] cmpPathModel ambiguous case \n");
            fflush(stdout);
            yagExit(-1);
        }
    }
    else {
        char            tp1[64];
        char            tp2[64];
        short           i = 0;
        int             ps1;
        int             ps2;

        while ((*p1 != '+') && (*p1 != '-') && (*p1 != '\0')) {
            tp1[i++] = *p1;
            p1++;
        }

        tp1[i] = '\0';
        i = 0;

        while ((*p2 != '+') && (*p2 != '-') && (*p2 != '\0')) {
            tp2[i++] = *p2;
            p2++;
        }
        tp2[i] = '\0';

        ps1 = atoi(tp1) * signe;
        ps2 = atoi(tp2) * signe;

        if (ps1 > ps2) return (1);
        else if (ps1 < ps2) return (2);
    }
    return (cmpPathModel(p1, p2, signe));
}

/****************************************************************************
 *                         fonction addConeModel();                         *
 ****************************************************************************/
/*-------------------------------------------------------------------------+
| fabrique  l'expression Modele ordonnee                                   |
| On le range dans le user 'YAG_MODEL_PTYPE'                               |
+-------------------------------------------------------------------------*/
static void
addConeModel(ptcone)
    cone_list      *ptcone;
{
    branch_list    *brlist[4];
    branch_list    *ptbranch;
    ptype_list     *ptuser;
    table          *tabBranch, *tabExpr;
    char           *res;
    long            type;
    unsigned        maxLength;
    int             i = 0;
    int             j = 0;

   /*--------------------------------------------+
   | On constitue le tableau des expressions sur |
   | lequel sera effectue le tri                 |
   +--------------------------------------------*/

    tabBranch = newtable();
    tabExpr = newtable();

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & (CNS_BLEEDER|CNS_FEEDBACK|CNS_MASK_PARA|CNS_IGNORE)) != 0) continue;
            if (i == 2 && ptbranch->LINK->NEXT == NULL) continue;
            ptuser = getptype(ptbranch->USER, YAG_WEIGHT_PTYPE);
            if (ptuser == NULL) { /* give up signature */
                deltable(tabBranch);
                deltable(tabExpr);
                return;
            }
            addtableitem(tabBranch, ptbranch);
            addtableitem(tabExpr, ptuser->DATA);
        }
    }

   /*-------------------------------------------_+
   | On effectue le tri                          |
   +--------------------------------------------*/
    for (i=0; i < tabBranch->SIZE-1; i++) {
        for (j=0; j < tabBranch->SIZE-1 - i; j++) {

            if (cmpPathModel(tabExpr->DATA[j], tabExpr->DATA[j+1], 1) == 1) {
                char           *save;
                list_list      *lsave;

                save = tabExpr->DATA[j];
                tabExpr->DATA[j] = tabExpr->DATA[j+1];
                tabExpr->DATA[j+1] = save;

                lsave = tabBranch->DATA[j];
                tabBranch->DATA[j] = tabBranch->DATA[j+1];
                tabBranch->DATA[j+1] = lsave;
            }
        }
    }

/*-----------------------------------------------+
| On constitue la 'somme' ordonnee de tout ca !!  |
+-----------------------------------------------*/
    res = (char *)mbkalloc(200);
    maxLength = 100;
    res[0] = '\0';
    j = 1;

    for (i = 0; i < tabBranch->SIZE-1; i++) {
        type = ((branch_list *)tabBranch->DATA[i])->TYPE;

        if ((type & CNS_VDD) == CNS_VDD) strcat(res, "u");
        else if ((type & CNS_VSS) == CNS_VSS) strcat(res, "d");
        else strcat(res, "e");

        strcat(res, tabExpr->DATA[i]);
        strcat(res, "_");

        if (strlen(res) > maxLength) {
            res = (char *)mbkrealloc(res, (200) + (j * 100));
            maxLength += 100;
            j++;
        }
    }

    if (tabBranch->SIZE > 0) {
        type = ((branch_list *)tabBranch->DATA[tabBranch->SIZE-1])->TYPE;

        if ((type & CNS_VDD) == CNS_VDD) strcat(res, "u");
        else if ((type & CNS_VSS) == CNS_VSS) strcat(res, "d");
        else strcat(res, "e");

        strcat(res, tabExpr->DATA[tabExpr->SIZE-1]);
    }

    if (strlen(res) < 1024) {
        ptcone->USER = addptype(ptcone->USER, YAG_MODEL_PTYPE, (void *)namealloc(res));
    }
    mbkfree(res);

    deltable(tabBranch);
    deltable(tabExpr);
}

