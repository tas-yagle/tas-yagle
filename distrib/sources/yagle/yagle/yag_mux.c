/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_mux.c                                                   */
/*                                                                          */
/*    (c) copyright 1991 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc LAURENTIN                        le : 06/09/1991     */
/*                                                                          */
/*    Modifie par : Anthony LESTER                      le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static chain_list *muxFcfDrive (list_list *varTot, chain_list *doubMu, chain_list *doubMd, chain_list *orOneTot, chain_list *doubCu, chain_list *andZerTot, chain_list *doubCd);
static chain_list *muxAddDoublet (chain_list *list, chain_list *doublet);
static short muxDoubletInList (chain_list *list, chain_list *doublet);
static short muxVarInList (list_list *list, char *var);
static list_list *muxAddVar (list_list *ptlist, char *name);
static list_list *muxMakeVarList (chain_list *liste);
static list_list *muxMergeList (list_list *list1, list_list *list2);
static chain_list *muxMakeDoubletList (chain_list *liste);
static list_list *muxAddList (list_list *headList, char *data, long type);
static list_list *muxChainToList (chain_list *chainliste);
static chain_list *muxDoubleToAbl (short oper, chain_list *doublet);
static chain_list *muxChaineToAbl (short oper, chain_list *liste);

/******************************************************************************
 *                         function yagMuxToConstraint()                      *
 ******************************************************************************/

chain_list *
yagMuxToConstraint(inffig_list *ifl)
{
    chain_list     *doubletListeMu = NULL;
    chain_list     *doubletListeMd = NULL;
    chain_list     *doubletListeCu = NULL;
    chain_list     *doubletListeCd = NULL;
    list_list      *varListeMu = NULL;
    list_list      *varListeMd = NULL;
    list_list      *varListeCu = NULL;
    list_list      *varListeCd = NULL;
    list_list      *varListe = NULL;
    chain_list     *res = NULL;
    chain_list *xu, *xd, *pu, *pd;

    if (!inf_GetPointer(ifl, INF_MUXU, "", (void **)&xu)) xu=NULL;
    if (!inf_GetPointer(ifl, INF_MUXD, "", (void **)&xd)) xd=NULL;
    if (!inf_GetPointer(ifl, INF_CMPU, "", (void **)&pu)) pu=NULL;
    if (!inf_GetPointer(ifl, INF_CMPD, "", (void **)&pd)) pd=NULL;

    /* SI PAS DE CONTRAINTES */
    if ((xu == NULL) &&
        (xd == NULL) &&
        (pu == NULL) &&
        (pd == NULL))
        return (NULL);


    varListeMu = muxMakeVarList(xu);
    varListeMd = muxMakeVarList(xd);
    varListeCu = muxMakeVarList(pu);
    varListeCd = muxMakeVarList(pd);

    varListe = muxMergeList(varListe, varListeMd);
    varListe = muxMergeList(varListe, varListeMu);
    varListe = muxMergeList(varListe, varListeCu);
    varListe = muxMergeList(varListe, varListeCd);

    doubletListeMu = muxMakeDoubletList(xu);
    doubletListeMd = muxMakeDoubletList(xd);
    doubletListeCu = muxMakeDoubletList(pu);
    doubletListeCd = muxMakeDoubletList(pd);

    res = muxFcfDrive(varListe, doubletListeMu, doubletListeMd, pu, doubletListeCu, pd, doubletListeCd);

/*    freechain(xu);
    freechain(xd);
    freechain(pu);
    freechain(pd);*/
    return (res);
}

/******************************************************************************
 *                         function muxFcfDrive()                             *
 ******************************************************************************/
/*---------------------------------------------------------------------------+
| Fabrique la base de donnee pour FcF concernant les contrainte externes     |
|                                                                            |
| Entree: Les liste de doublets pour les AND et les liste de variables pour  |
|         les OR les AND (contrainte 'completes').                           |
|                                                                            |
| sortie: Une chain_liste: Le premier DATA contient la liste (chain_liste)   |
|         de tous les connecteurs concernes par des contraintes.             |
|         Le  nieme (n>1) DATA contient  la nieme liste                      |
|         list_list)  de variables constituant UNE contrainte.               |
|         Le USER du premier (list_list) de chaque liste contient l'ABL      |
|         exprimant la contrainte                                            |
|                                                                            |
| Exemple  2 contraintes : A AND B =0 , A AND C =0;                          |
|                                                                            |
|         DATA(1) pointe sur (list_list)A->B->C->NULL;                       |
|         DATA(2) pointe sur (list_list) A->B->NULL;                         |
|                                        |                                   |
|                                 user : +-> ABL (A AND B)                   |
|                                                                            |
|         DATA(3) pointe sur (list_list) A->C->NULL;                         |
|                                        |                                   |
|                                 user:  +-> ABL (A AND C)                   |
|                                                                            |
+---------------------------------------------------------------------------*/

static chain_list *
muxFcfDrive(list_list *varTot, chain_list *doubMu, chain_list *doubMd, chain_list *orOneTot, chain_list *doubCu, chain_list *andZerTot, chain_list *doubCd)
{
    chain_list     *chain = NULL;
    chain_list     *res = NULL;

   /*--------------------------------------------+
   | Constitution des I AND J = 0 muxUp          |
   +--------------------------------------------*/
    for (chain = doubMu; chain != NULL; chain = chain->NEXT) {
        chain_list     *doublet;
        list_list      *contr = NULL;        /*contrainte a construire*/
        chain_list     *abl = NULL;

        doublet = (chain_list *) chain->DATA;
        contr = muxChainToList(doublet);
        abl = muxDoubleToAbl(MUXUP, doublet);
        contr->USER = (ptype_list *) abl;

        res = addchain(res, (void *) contr);
    }

   /*--------------------------------------------+
   | Constitution des I AND J = 0 cmpUp          |
   +--------------------------------------------*/
    for (chain = doubCu; chain != NULL; chain = chain->NEXT) {
        chain_list     *doublet;
        list_list      *contr = NULL;
        chain_list     *abl = NULL;

        doublet = (chain_list *) chain->DATA;
        contr = muxChainToList(doublet);
        abl = muxDoubleToAbl(CMPUP, doublet);
        contr->USER = (ptype_list *) abl;

        res = addchain(res, (void *) contr);
    }

   /*--------------------------------------------+
   | Constitution des I OR J = 1 muxDn           |
   +--------------------------------------------*/
    for (chain = doubMd; chain != NULL; chain = chain->NEXT) {
        chain_list     *doublet;
        list_list      *contr = NULL;
        chain_list     *abl = NULL;

        doublet = (chain_list *) chain->DATA;
        contr = muxChainToList(doublet);
        abl = muxDoubleToAbl(MUXDN, doublet);
        contr->USER = (ptype_list *) abl;

        res = addchain(res, (void *) contr);
    }

   /*--------------------------------------------+
   | Constitution des I OR J = 1 cmpDn           |
   +--------------------------------------------*/
    for (chain = doubCd; chain != NULL; chain = chain->NEXT) {
        chain_list     *doublet;
        list_list      *contr = NULL;
        chain_list     *abl = NULL;

        doublet = (chain_list *) chain->DATA;
        contr = muxChainToList(doublet);
        abl = muxDoubleToAbl(CMPDN, doublet);
        contr->USER = (ptype_list *) abl;

        res = addchain(res, (void *) contr);
    }

   /*--------------------------------------------+
   | Constitution des I OR J OR K..= 1 cmpUp     |
   +--------------------------------------------*/
    for (chain = orOneTot; chain != NULL; chain = chain->NEXT) {
        chain_list     *liste;        /* liste de variable */
        list_list      *contr = NULL;
        chain_list     *abl;

        liste = (chain_list *) chain->DATA;
        contr = muxChainToList(liste);
        abl = muxChaineToAbl(CMPUP, liste);
        contr->USER = (ptype_list *) abl;
        res = addchain(res, (void *) contr);
    }

   /*--------------------------------------------+
   | Constitution des I AND J AND K..= 0 cmpDn   |
   +--------------------------------------------*/
    for (chain = andZerTot; chain != NULL; chain = chain->NEXT) {
        chain_list     *liste;
        list_list      *contr = NULL;
        chain_list     *abl;

        liste = (chain_list *) chain->DATA;
        contr = muxChainToList(liste);
        abl = muxChaineToAbl(CMPDN, liste);
        contr->USER = (ptype_list *) abl;
        res = addchain(res, (void *) contr);
    }

   /*-------------------------------------------+
   | On ajoute la liste de TOUTES les variables |
   +-------------------------------------------*/
    res = addchain(res, (void *) varTot);
    return (res);
}

/*****************************************************************************
 *                         function muxAddDoublet()                          *
 *****************************************************************************/

static chain_list *
muxAddDoublet(list, doublet)
    chain_list     *list;
    chain_list     *doublet;
{

    if (doublet == NULL) {
        printf("mux2vcx addCoupleChain doublet=NULL\n");
        yagExit(-1);
    }

    if (muxDoubletInList(list, doublet) == -1) {
        list = addchain(list, (void *) doublet);
    }

    return (list);
}

/*****************************************************************************
 *                         function doubletInList()                          *
 *****************************************************************************/

static short
muxDoubletInList(list, doublet)
    chain_list     *list;
    chain_list     *doublet;
{
    chain_list     *chain;
    chain_list     *couple;

    if (doublet == NULL) {
        printf("mux2vcx doubletInList : doublet=NULL\n");
        yagExit(-1);
    }

    for (chain = list; chain != NULL; chain = chain->NEXT) {
        couple = (chain_list *) chain->DATA;

        if (((char *) doublet->DATA == (char *) couple->DATA) &&
            ((char *) doublet->NEXT->DATA == (char *) couple->NEXT->DATA))
            break;

        if (((char *) doublet->DATA == (char *) couple->NEXT->DATA) &&
            ((char *) doublet->NEXT->DATA == (char *) couple->DATA))
            break;
    }

    if (chain == NULL)
        return (-1);
    else
        return (0);
}

/*****************************************************************************
 *                         function muxVarInList()                           *
 *****************************************************************************/
/*--------------------------------------------------+
| teste si var (char*)est dans la liste(list_list) |
| Retourne 0 si OUI                                 |
|         -1 si NON                                 |
+--------------------------------------------------*/

static short
muxVarInList(list_list *list, char *var)
{
    list_list     *ptlist;

    for (ptlist = list; ptlist != NULL; ptlist = ptlist->NEXT) {
        if (var == (char *) ptlist->DATA)break;
    }

    if (ptlist == NULL) return (-1);
    else return (0);
}

/*****************************************************************************
 *                          function muxAddVar()                             *
 *****************************************************************************/
/*--------------------------------------------------+
| Ajoute une variable 'name' dans la liste 'list'   |
| si elle n'y est pas deja                          |
+--------------------------------------------------*/

static list_list *
muxAddVar(list_list *ptlist, char *name)
{
    if (name[0] == '~')
        name++;

    name = namealloc(name);
    if (muxVarInList(ptlist, name) == -1) {
        ptlist = muxAddList(ptlist, name, 0);
    }
    return (ptlist);
}

/*****************************************************************************
 *                          function muxMakeVarList()                        *
 *****************************************************************************/
/*---------------------------------------------------+
| Entree : Un chain_list de chainlist  de variable   |
| Sortie : Un chain_list unique de variables dans    |
|          laquelle il n'y a pas de redondance       |
+---------------------------------------------------*/

static list_list *
muxMakeVarList(chain_list *ptlist)
{
    list_list     *result = NULL;
    chain_list    *intList;
    chain_list    *intVar;

    for (intList = ptlist; intList != NULL; intList = intList->NEXT) {
        for (intVar = (chain_list *) intList->DATA; intVar != NULL; intVar = intVar->NEXT) {
            result = muxAddVar(result, (char *) intVar->DATA);
        }
    }
    return (result);
}

/*****************************************************************************
 *                          function muxMergeList()                          *
 *****************************************************************************/
/*-------------------------------------------------+
| Ajoute a la liste list1, toutes les variables de |
| la liste list2 qui ne sont pas deja dans list1   |
| Entree: deux chain_list de char * non redondant  |
| Sortie: Une liste unique de variables            |
+-------------------------------------------------*/

static list_list *
muxMergeList(list1, list2)
    list_list     *list1;
    list_list     *list2;
{
    list_list     *ptlist = NULL;

    for (ptlist = list2; ptlist != NULL; ptlist = ptlist->NEXT) {
        list1 = muxAddVar(list1, (char *) ptlist->DATA);
    }
    return (list1);
}

/*****************************************************************************
 *                         function muxMakeDoubletList()                     *
 *****************************************************************************/
/*------------------------------------------------------------------+
| Constitue une liste de doublets a partie d'une LISTE (chain_list*)|
| de liste (chain_list*). Pour chaque liste, {A,B,C...} sont        |
| constitues les doublet {A,B} {A,C}...puis {B,C}.....              |
| Entree: Une LISTE de liste.                                       |
| Sortie: Une liste de doublets                                     |
+------------------------------------------------------------------*/

static chain_list *
muxMakeDoubletList(liste)
    chain_list     *liste;
{
    chain_list     *result = NULL;
    chain_list     *listList;
    chain_list     *varList;

    for (listList = liste; listList != NULL; listList = listList->NEXT) {
        for (varList = (chain_list *) listList->DATA;
             varList != NULL;
             varList = varList->NEXT) {
            chain_list     *chain;

            for (chain = varList->NEXT; chain != NULL; chain = chain->NEXT) {
                chain_list     *doublet;

                doublet = NULL;
                doublet = addchain(doublet, (void *) varList->DATA);
                doublet = addchain(doublet, (void *) chain->DATA);

                result = muxAddDoublet(result, doublet);
            }
        }
    }
    return (result);
}

/*****************************************************************************
 *                          function muxAddList()                            *
 *****************************************************************************/

static list_list *
muxAddList(headList, data, type)
    list_list      *headList;
    char           *data;
    long            type;
{
    list_list      *ptlist = NULL;

    ptlist = (list_list *) mbkalloc(sizeof(list_list));

    ptlist->NEXT = headList;
    ptlist->TYPE = type;
    ptlist->DATA = data;
    ptlist->USER = NULL;
    ptlist->SUPDATA = NULL;
    ptlist->COMMON = NULL;
    return (ptlist);
}

/*****************************************************************************
 *                          function muxChainToList()                        *
 *****************************************************************************/

static list_list *
muxChainToList(chainliste)
    chain_list     *chainliste;
{
    chain_list     *chain;
    list_list      *res = NULL;

    for (chain = chainliste; chain != NULL; chain = chain->NEXT) {
        char           *varName = (char *) chain->DATA;

        if (varName[0] == '~')
            varName++;
        varName = namealloc(varName);
        res = muxAddList(res, varName, (long) 0);
    }
    return (res);
}

/*****************************************************************************
 *                         function muxDoubleToAbl()                         *
 *****************************************************************************/

static chain_list *
muxDoubleToAbl(oper, doublet)
    short           oper;
    chain_list     *doublet;
{
    chain_list     *abl = NULL;
    chain_list     *chain;

    switch (oper) {
        /* le produit fait Zero */
    case MUXUP:
    case CMPUP:
        {
            abl = createExpr(AND);

            for (chain = doublet; chain != NULL; chain = chain->NEXT) {
                char           *var;

                var = (char *) chain->DATA;
                if (var[0] == '~') {
                    var++;
                    var = namealloc(var);
                    addQExpr(abl, notExpr(createAtom(var)));
                }
                else
                    addQExpr(abl, createAtom(var));
            }

            return (notExpr(abl));
        }

        /* La somme fait Un */
    case MUXDN:
    case CMPDN:
        {
            abl = createExpr(OR);

            for (chain = doublet; chain != NULL; chain = chain->NEXT) {
                char           *var;

                var = (char *) chain->DATA;
                if (var[0] == '~') {
                    var++;
                    var = namealloc(var);
                    addQExpr(abl, notExpr(createAtom(var)));
                }
                else
                    addQExpr(abl, createAtom(var));
            }

            return (abl);
        }
    }
    return NULL;
}

/*****************************************************************************
 *                         function muxChaineToAbl()                         *
 *****************************************************************************/

static chain_list *
muxChaineToAbl(oper, liste)
    short           oper;
    chain_list     *liste;
{
    chain_list     *chaine;
    chain_list     *abl;

    switch (oper) {
        /* la somme fait Un */
        case CMPUP:
            abl = createExpr(OR);
            for (chaine = liste; chaine != NULL; chaine = chaine->NEXT) {
                char           *var;

                var = (char *) chaine->DATA;
                if (var[0] == '~') {
                    var++;
                    var = namealloc(var);
                    addQExpr(abl, notExpr(createAtom(var)));
                }
                else
                    addQExpr(abl, createAtom(var));
            }
            return (abl);
            break;

        /* le produit fait Zero */
        case CMPDN:
            abl = createExpr(AND);
            for (chaine = liste; chaine != NULL; chaine = chaine->NEXT) {
                char           *var;

                var = (char *) chaine->DATA;
                if (var[0] == '~') {
                    var++;
                    var = namealloc(var);
                    addQExpr(abl, notExpr(createAtom(var)));
                }
                else
                    addQExpr(abl, createAtom(var));
            }
            return (notExpr(abl));
            break;
    }
    return NULL;
}
