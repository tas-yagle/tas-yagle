/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_lofig.c                                                 */
/*                                                                          */
/*    (c) copyright 1993 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Marc LAURENTIN                        le : 27/01/1993     */
/*                                                                          */
/*    Modifie par : Anthony LESTER                      le : 20/11/1994     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

lofig_list   *YAG_HEAD_MODEL = NULL;

static lofig_list *cnsmodToLomod __P((cone_list *ptcone, int index));
static int coneConnect __P((cone_list *ptcone));
static int coneIsEmpty __P((cone_list *ptcone));
static void saveVbeConeModel __P((cone_list *ptcone, int index));
static void prepConeExt __P((cone_list *ptcone));
static void restoreConeExt __P((cone_list *ptcone));
static void delOutconeForVst __P((cnsfig_list *ptcnsfig));
static void genCatal __P((lofig_list *headModel));
static void prepConeMemory __P((cone_list *ptcone));
static void restoreConeMemory __P((cone_list *ptcone));
static void addGlueCones __P((cnsfig_list *ptcnsfig));
static void removeGlueCones __P((cnsfig_list *ptcnsfig));
static void addCellInstances __P((lofig_list *ptlofig, cnsfig_list *ptcnsfig, long vddSigIndex, long vssSigIndex));
static lofig_list *cellToLomod __P((cell_list *ptcell));
static chain_list *labelEnum __P((char *root, long num));

/****************************************************************************
 *                           function yagBuildLofig();                      *
 ****************************************************************************/
void
yagBuildLofig(ptcnsfig, ptlotrsfig)
    cnsfig_list *ptcnsfig;
    lofig_list *ptlotrsfig;
{
    ht          *funcTable;
    ht          *modelTable;
    lofig_list  *tabModel[200]; /* table des model:index donne par modelTable*/
    edge_list   *tabIncone[500];
    lofig_list  *ptlofig;
    lofig_list  *ptfig;
    losig_list  *ptsig;
    locon_list  *ptcon;
    cone_list   *ptcone, *ptxcone;
    edge_list   *ptedge;
    cell_list   *ptcell;
    ptype_list  *ptuser;
    chain_list  *signame = NULL;
    chain_list  *sigchain;
    chain_list  *ptchain;
    char        buf[YAGBUFSIZE];
    char        *model;
    char        *expr;
    char        *name;
    long        vddSigIndex;
    long        vssSigIndex;
    long        nbSig = 1;
    int         modelIdx = 1;
    int         nbModel, nbEdge;
    int         value, i;
    int         vddDone = FALSE;
    int         vssDone = FALSE;
    short       save_USE_FCF;
    float       capa;

    /* disable global analysis of any cones built */
    save_USE_FCF = YAG_CONTEXT->YAG_USE_FCF;
    YAG_CONTEXT->YAG_USE_FCF = FALSE;
    
    funcTable = addht(100);  /* table des occurences de modeles (stat..)*/
    modelTable = addht(100); /* correspondance model <-> index */

/*-----------------------------------------------------------------+
 | On supprime iterativement les outcones qui pointe sur des cones |
 | qui n'ont pas de sorties, ceci pour que la netlist structurelle |
 | soit avalable par Asimut ou par SaX                             |
 +----------------------------------------------------------------*/
    delOutconeForVst(ptcnsfig);

   /*--------------------------------------------------------+
   | On associe a chaque cone son modele (char * ~= structrel|
   | Si le model est nouveau , il est ajoute a la table de   |
   | hasch modelTable avec un index qui est incremente.      |
   | et le model logique MBK est cree                        |
   | Le pointeur sur le model logique (MBK) est ajoute dans  |
   | un tableau de pointeur, dont l'index est celui precite  |
   | ON accedera donc au  pointeur de model en cherchant     |
   | table[i] avec i = le res du hash sur l'expression du    |
   | modele                                                  |
   | Enfin, le vbe du model est crache sur disque            |
   +--------------------------------------------------------*/

    addGlueCones(ptcnsfig);

    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {

        if (ptcone->CELLS != NULL) {
            for (ptchain = ptcone->CELLS; ptchain; ptchain = ptchain->NEXT) {
                ptcell = (cell_list *)ptchain->DATA;
                if ((ptcell->TYPE & ~CNS_UNKNOWN) >= 128 && ptcell->BEFIG == NULL) break;
            }
            if (ptchain == NULL) continue;
        }
        if ((ptcone->TYPE & CNS_POWER) != 0) continue;
        if (ptcone->OUTCONE == NULL && (ptcone->TYPE & YAG_GLUECONE) == 0) continue;
        if (coneIsEmpty(ptcone) == 0) continue; 
        if (coneConnect(ptcone) == 0) continue;

        if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) prepConeExt(ptcone);
        if ((ptcone->TYPE & (CNS_MEMSYM|CNS_LATCH)) == CNS_MEMSYM)
        prepConeMemory(ptcone);

        yagMakeConeModel(ptcone);
        ptuser = getptype(ptcone->USER, YAG_MODEL_PTYPE);
        if (ptuser == NULL) continue;
        expr = (char*)ptuser->DATA;
        value = gethtitem(funcTable,expr);

        if(value == EMPTYHT) {      /* Nouveau model de cone */
            addhtitem(funcTable, expr, 1);
            addhtitem(modelTable, expr, modelIdx);
            ptfig = cnsmodToLomod(ptcone, modelIdx);
            savelofig(ptfig); 
            saveVbeConeModel(ptcone, modelIdx);
            tabModel[modelIdx++] = ptfig;
        }
        else sethtitem(funcTable,expr,value+1);
    }

/*-----------------------------------------------------+
| On cree la figure proprement dit                     |
+-----------------------------------------------------*/
    sprintf(buf, "%s_yagh", ptcnsfig->NAME);
    ptlofig = addlofig(namealloc(buf));

/*--------------------------------------------------+
| Signaux de la figure                              |
+--------------------------------------------------*/
    for(ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        nbSig++;

        if (ptcone->CELLS != NULL) {
            for (ptchain = ptcone->CELLS; ptchain; ptchain = ptchain->NEXT) {
                ptcell = (cell_list *)ptchain->DATA;
                if ((ptcell->TYPE & ~CNS_UNKNOWN) >= 128 && ptcell->BEFIG == NULL) break;
            }
            if (ptchain == NULL && (ptcone->TYPE & YAG_CELLOUT) == 0) continue;
        }
        if (ptcone->OUTCONE == NULL && (ptcone->TYPE & YAG_GLUECONE) == 0) continue;
        if ((ptcone->TYPE & CNS_POWER) == CNS_POWER) continue;
        if (coneIsEmpty(ptcone) == 0) continue;

        signame = addchain(NULL, (void *)ptcone->NAME);

        if ((ptcone->TYPE & CNS_EXT) != 0)
            ptsig = addlosig(ptlofig, ptcone->INDEX, signame, EXTERNAL);
        else ptsig = addlosig(ptlofig, ptcone->INDEX, signame, INTERNAL);
        addlorcnet(ptsig);
        rcn_addcapa(ptsig, yagGetConeCapa(ptcone, ptlofig));
    }

/*---------------------------------------------------------+
| Connecteurs externes: Ceux sur lequel un cone est monte  |
| sont craches tels quel puisque un signal ( un cone) leur |
| associe. Pour les autres (fin de branche EXT sur lequel ,|
| il n'y a pas de cone -xor de connecteur=contre exemple-) |
|il faut                                                   |
| un signal supplementaire, dont on range l'index dans un  |
| user MBK_SIG du connecteur                               |
+---------------------------------------------------------*/
    for (ptcon = ptcnsfig->LOCON; ptcon != NULL; ptcon = ptcon->NEXT) {

        if (ptcon->DIRECTION == 'X') continue;  /* Transparence */

        if (ptcon->DIRECTION == CNS_VDDC || ptcon->DIRECTION == CNS_VSSC) {
            if (ptcon->DIRECTION == CNS_VDDC && !vddDone && !YAG_CONTEXT->YAG_NO_SUPPLY) {
                vddSigIndex = nbSig;
                if (YAG_CONTEXT->YAG_ONE_SUPPLY) {
                    vddDone = TRUE;
                    name = CNS_VDDNAME;
                }
                else name = ptcon->NAME;
                ptsig = addlosig(ptlofig, nbSig, addchain(NULL, name), EXTERNAL);
                addlorcnet(ptsig);
                if (ptcon->SIG->PRCN != NULL) capa = rcn_getcapa(ptlotrsfig, ptcon->SIG);
                else capa = 0.0;
                rcn_addcapa(ptsig, capa);
                addlocon(ptlofig, name, getlosig(ptlofig,vddSigIndex), 'I');
                nbSig++;
            }
            else if (ptcon->DIRECTION == CNS_VSSC && !vssDone && !YAG_CONTEXT->YAG_NO_SUPPLY) {
                vssSigIndex = nbSig;
                if (YAG_CONTEXT->YAG_ONE_SUPPLY) {
                    vssDone = TRUE;
                    name = CNS_VSSNAME;
                }
                else name = ptcon->NAME;
                ptsig = addlosig(ptlofig, nbSig, addchain(NULL, name), EXTERNAL);
                addlorcnet(ptsig);
                if (ptcon->SIG->PRCN != NULL) capa = rcn_getcapa(ptlotrsfig, ptcon->SIG);
                else capa = 0.0;
                rcn_addcapa(ptsig, capa);
                addlocon(ptlofig, name, getlosig(ptlofig,vssSigIndex), 'I');
                nbSig++;
            }
            continue;
        }

        ptuser = getptype(ptcon->USER, CNS_EXT);
        /* connecteur sur lequel est monte un cone */

        if (ptuser != NULL) {
            ptcone = (cone_list*)ptuser->DATA;
            if (coneIsEmpty(ptcone) != 0)       /* si cone non vide */
                addlocon(ptlofig, ptcon->NAME, getlosig(ptlofig, ptcone->INDEX), ptcon->DIRECTION);
            else {
                ptsig = addlosig(ptlofig, nbSig, addchain(NULL, (void*)ptcon->NAME), EXTERNAL);
                addlorcnet(ptsig);
                if (ptcon->SIG->PRCN != NULL) capa = rcn_getcapa(ptlotrsfig, ptcon->SIG);
                else capa = 0.0;
                rcn_addcapa(ptsig, capa);
                addlocon(ptlofig, ptcon->NAME, getlosig(ptlofig, nbSig), ptcon->DIRECTION);
                ptcon->USER = addptype(ptcon->USER, CNS_SIGNAL, (void*)nbSig);
                nbSig++;
            }
        }
        else {  /* les autres */
            ptsig = addlosig(ptlofig, nbSig, NULL, EXTERNAL);
            addlorcnet(ptsig);
            if (ptcon->SIG->PRCN != NULL) capa = rcn_getcapa(ptlotrsfig, ptcon->SIG);
            else capa = 0.0;
            rcn_addcapa(ptsig, capa);
            addlocon(ptlofig, ptcon->NAME, getlosig(ptlofig, nbSig), ptcon->DIRECTION);
            ptcon->USER = addptype(ptcon->USER, CNS_SIGNAL, (void*)nbSig);
            nbSig++;
        }
    }

   /*-----------------------------------------------------+
   | On change temporairement l'index de tous les cones   |
   | VDD ou VSS. Leur index prend la valeur de l'index    |
   | du signal associe aux connecteurs ext d'alimentation |
   +-----------------------------------------------------*/
    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if((ptcone->TYPE & CNS_VDD) == CNS_VDD) {
            ptcone->USER = addptype(ptcone->USER, YAG_OLDINDEX_PTYPE, (void*)ptcone->INDEX);
            ptcone->INDEX = vddSigIndex;
        }
        else if ((ptcone->TYPE & CNS_VSS) == CNS_VSS) {
            ptcone->USER = addptype(ptcone->USER, YAG_OLDINDEX_PTYPE, (void*)ptcone->INDEX);
            ptcone->INDEX = vssSigIndex;
        }
        else continue;
    } 

    /*----------------------------------------------------+
    | on crache les instances                             |
    +----------------------------------------------------*/
    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {

        if (ptcone->CELLS != NULL) {
            for (ptchain = ptcone->CELLS; ptchain; ptchain = ptchain->NEXT) {
                ptcell = (cell_list *)ptchain->DATA;
                if ((ptcell->TYPE & ~CNS_UNKNOWN) >= 128 && ptcell->BEFIG == NULL) break;
            }
            if (ptchain == NULL) continue;
        }
        if (ptcone->OUTCONE == NULL && (ptcone->TYPE & YAG_GLUECONE) == 0) continue;
        if (coneIsEmpty(ptcone) == 0) continue;
        if (coneConnect(ptcone) == 0) continue;
        if ((ptcone->TYPE & CNS_POWER) == CNS_POWER) continue;
     
        ptuser = getptype(ptcone->USER, YAG_MODEL_PTYPE);

        if (ptuser == NULL) {
            yagBug(DBG_NO_CONEMODEL,"yagBuildLofig",ptcone->NAME,NULL,0);
        }

   /*------------------------------------------------------------------+
   | On recupere le lomodele du cone  a partir du model passe dans TsH |
   +------------------------------------------------------------------*/
        model = (char *)ptuser->DATA ;
        nbModel = gethtitem(modelTable, model);
        ptfig = tabModel[nbModel];

      /*-------------------------------------------+
      | On constitue un tableau ordonnee de incone |
      +-------------------------------------------*/
        nbEdge = 0;
        for(ptedge = ptcone->INCONE; ptedge != NULL; ptedge = ptedge->NEXT) {
            if((ptedge->TYPE & CNS_IGNORE) == CNS_IGNORE) continue ;
            if((ptedge->TYPE & YAG_NOT_FUNCTIONAL) == YAG_NOT_FUNCTIONAL) continue ;
            if((ptedge->TYPE & CNS_POWER) == CNS_POWER) continue ;
            if((ptedge->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue ;

            ptuser = getptype(ptedge->USER, YAG_WEIGHT_PTYPE);
            i = (int)((long)ptuser->DATA);
            tabIncone[i] = ptedge;
            nbEdge++;
        }

       /*-----------------------------------------+
       | On fabrique la chainlist des entrees     |
       +-----------------------------------------*/
        sigchain = NULL;
        for (i=1; i<=nbEdge; i++) {
            ptedge = tabIncone[i];

            if ((ptedge->TYPE & CNS_CONE) != 0) {
                ptxcone = ptedge->UEDGE.CONE ;
                sigchain = addchain(sigchain, (void*)getlosig(ptlofig, ptxcone->INDEX));
            }
            else { /* Entree de type Connecteur */
                ptcon = ptedge->UEDGE.LOCON;
               /*-----------------------------------------------------+
               | Si un cone est monte sur le connecteur , ATTENTION , |
               | C'est peut etre un cone vide                         |
               +-----------------------------------------------------*/
                if ((ptuser = getptype(ptcon->USER, CNS_EXT)) != NULL) {
                    ptxcone = (cone_list *)ptuser->DATA;

                    if (coneIsEmpty(ptxcone) != 0) { /* si cone non vide */
                        sigchain = addchain(sigchain, (void*)getlosig(ptlofig, ptxcone->INDEX));
                    }
                    else {            /* si cone vide, le signal 
                            qui y est attache n'a pas ete declare mais 
                            un signal a ete creee pour le connecteur  */
                        ptuser = getptype(ptcon->USER, CNS_SIGNAL);
                        sigchain=addchain(sigchain, (void*)getlosig(ptlofig,(long)ptuser->DATA));
                    }
                }
                else { /* Pas de cone monte sur le connecteur -> comme cone vide */
                    ptuser = getptype(ptcon->USER,CNS_SIGNAL);
                    sigchain=addchain(sigchain, (void*)getlosig(ptlofig,(long)ptuser->DATA));
                }
            }
        }

        /* ON RAJOUTE la SORTIE VDD et VSS puis on fait le ADDLOINS*/

        { 
            char  insname[YAGBUFSIZE];
            sigchain = addchain(sigchain,(void*)getlosig(ptlofig,ptcone->INDEX));
            sigchain = addchain(sigchain,(void*)getlosig(ptlofig,vddSigIndex));
            sigchain = addchain(sigchain,(void*)getlosig(ptlofig,vssSigIndex));
            sprintf(insname,"yagins_%s",yagVectorizeName(ptcone->NAME));
            addloins(ptlofig,namealloc(insname),ptfig,sigchain);
        }
    }

    addCellInstances(ptlofig, ptcnsfig, vddSigIndex, vssSigIndex);

   /*----------------------------------------------------+
   | On remet en place les index des cones alimentation  |
   | et les con EXT contenant un connecteur 'T'          |
   +----------------------------------------------------*/
    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        if ((ptcone->TYPE & CNS_POWER) == CNS_POWER) {
            ptuser = getptype(ptcone->USER, YAG_OLDINDEX_PTYPE);
            ptcone->INDEX = (long)ptuser->DATA;
            ptcone->USER = delptype(ptcone->USER, YAG_OLDINDEX_PTYPE);
        }
        else if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
            restoreConeExt(ptcone);
        }
        else if ((ptcone->TYPE & (CNS_MEMSYM|CNS_LATCH)) == CNS_MEMSYM) {
            restoreConeMemory(ptcone);
        }
        else continue;
    }

    removeGlueCones(ptcnsfig);

    sortlocon(&ptlofig->LOCON);
    genCatal(YAG_HEAD_MODEL);
    lofigchain(ptlofig);
    ptcnsfig->USER = addptype(ptcnsfig->USER, YAG_CONE_NETLIST_PTYPE, ptlofig);
    
    YAG_CONTEXT->YAG_USE_FCF = save_USE_FCF;
}

/****************************************************************************
 *                         function cnsmodToLomod();                        *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
| Fabrique un lofig contenant essentiellement l'interface du cone           |
+--------------------------------------------------------------------------*/
static lofig_list *
cnsmodToLomod(ptcone, index)
    cone_list     *ptcone;
    int            index;
{
    lofig_list    *ptfig;
    edge_list     *ptincone;
    ptype_list    *ptuser;
    char           figname[YAGBUFSIZE];
    long           poids;
    long           nbIn = 0;
    int            i = 0;

    for(ptincone = ptcone->INCONE; ptincone != NULL; ptincone = ptincone->NEXT) {

        if((ptincone->TYPE & CNS_IGNORE) == CNS_IGNORE) continue;
        if((ptincone->TYPE & YAG_NOT_FUNCTIONAL) == YAG_NOT_FUNCTIONAL) continue;
        if((ptincone->TYPE & CNS_POWER) == CNS_POWER) continue;
        if((ptincone->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue;

        ptuser = getptype(ptincone->USER,YAG_WEIGHT_PTYPE);
        if(ptuser == NULL) {
            yagBug(DBG_NO_INCONEUSER,"cnsmodToLomod",ptcone->NAME,"YAG_WEIGHT_PTYPE",0);
        }
        poids = (long)ptuser->DATA;

        if (poids > nbIn) nbIn = poids;
    }

    sprintf(figname,"%s_model_%d", CNS_HEADCNSFIG->NAME, index);

    YAG_HEAD_MODEL = addlomodel(YAG_HEAD_MODEL, namealloc(figname));
    ptfig = YAG_HEAD_MODEL;

    for(i=1; i<=nbIn; i++) addlosig(ptfig, i, (void*)NULL, EXTERNAL);

    addlosig(ptfig, nbIn+1, (void *)NULL, EXTERNAL); /* f */
    addlosig(ptfig, nbIn+2, (void *)NULL, EXTERNAL); /* vdd */
    addlosig(ptfig, nbIn+3, (void *)NULL, EXTERNAL); /* vss */

    for (i=1; i<=nbIn; i++) {
        char buff[6];
        sprintf(buff, "in%d", i);
        addlocon(ptfig, namealloc(buff), getlosig(ptfig,i), 'I');
    }

    if((ptcone->TYPE & CNS_TRI) == CNS_TRI) addlocon(ptfig, namealloc("f"), getlosig(ptfig,nbIn+1), 'Z');
    else addlocon(ptfig, namealloc("f"), getlosig(ptfig,nbIn+1), 'O');

    addlocon(ptfig, namealloc("vdd"), getlosig(ptfig,nbIn+2), 'I');
    addlocon(ptfig, namealloc("vss"), getlosig(ptfig,nbIn+3),'I');
    return ptfig;
}

/****************************************************************************
 *                         function coneConnect();                        *
 ****************************************************************************/
/*-------------------------------------------------------------------------+
| Retourne 0 si le cone est un cone connecteur ie il possede une branche  |
|            functionnelle unique qui est la branche maillon connecteur   |
|          -1 si le cone possede au moins une branche functionnele qui    |
|             n'est pas externe.                                          |
+-------------------------------------------------------------------------*/
static int
coneConnect(ptcone)
    cone_list    *ptcone;
{
    branch_list  *ptbranch;
    short        coneconnect = 0;
    short        funcpath = 0;

    if ((ptcone->TYPE & CNS_EXT) != CNS_EXT) return(-1);

    if(ptcone->BREXT==NULL) return(-1);

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) funcpath++;
    }
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) funcpath++;
    }
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT == NULL) coneconnect++;
        else funcpath++;
    }

    if (funcpath != 0) return(-1);
    else if (coneconnect == 1) return(0);
    else {
        yagBug(DBG_MULT_CON, "coneConnect", ptcone->NAME, NULL, 0);
    }
    return(0);
}

/****************************************************************************
 *                         function coneIsEmpty();                          *
 ****************************************************************************/
static int
coneIsEmpty(ptcone)
    cone_list   *ptcone;
{
    if ((ptcone->BRVDD == NULL)
    && (ptcone->BRVSS == NULL)
    && (ptcone->BREXT == NULL)
    && (ptcone->BRGND == NULL)
    && (ptcone->INCONE == NULL)
    && (ptcone->OUTCONE == NULL))
       return 0;
    else return -1;
}
  
/****************************************************************************
 *                         function vbeConeModel();                        *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
 | Sauvegarde sur disque le comportemental Vhdl d'un model de cone          |
 +-------------------------------------------------------------------------*/
static void 
saveVbeConeModel(ptcone, index)
    cone_list      *ptcone;
    int             index;
{
    befig_list     *ptbefig;
    edge_list      *ptin;
    cone_list      *ptincone;
    locon_list     *ptcon;
    ptype_list     *ptuser;
    biabl_list     *biexpr;
    chain_list     *expr;
    char            figname[YAGBUFSIZE];
    char            newName[YAGBUFSIZE];
    char           *name;
    long            poids;

   /*--------------------------------------------------------------+
   | On renomme temporairement tous les cones ou les connecteurs   |
   | qui sont en entree du cone. On parcourt les incones dont on   |
   | connait le poids, et in renomme le cone/connect correspondant |
   | par un ("in%s",poids) comme pour les instances                |
   | La sortie s'appellera invariablement 'f'                      |
   +--------------------------------------------------------------*/
    for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_IGNORE) == CNS_IGNORE) continue;
        if ((ptin->TYPE & YAG_NOT_FUNCTIONAL) == YAG_NOT_FUNCTIONAL) continue;
        if ((ptin->TYPE & CNS_POWER) == CNS_POWER) continue;
        if ((ptin->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue;

        if ((ptin->TYPE & CNS_CONE) != 0) {
            ptincone = ptin->UEDGE.CONE;
            ptincone->USER = addptype(ptincone->USER, YAG_OLDNAME_PTYPE, (void *)ptincone->NAME);
            ptuser = getptype(ptin->USER, YAG_WEIGHT_PTYPE);
            poids = (long)ptuser->DATA;
            sprintf(newName, "in%ld", poids);
            ptincone->NAME = namealloc(newName);
        }
        else {
            ptcon = ptin->UEDGE.LOCON;
            ptcon->USER = addptype(ptcon->USER, YAG_OLDNAME_PTYPE, (void *)ptcon->NAME);
            ptuser = getptype(ptin->USER, YAG_WEIGHT_PTYPE);
            poids = (long)ptuser->DATA;
            sprintf(newName, "in%ld", poids);
            ptcon->NAME = namealloc(newName);
        }
    }

   /*-----------------------------------------------------+
   | Ecriture du modele sur disque                        |
   +-----------------------------------------------------*/

    sprintf(figname, "%s_model_%d", CNS_HEADCNSFIG->NAME, index);
    if ((ptbefig = yagMakeLatchBehaviour(ptcone, FALSE)) == NULL) {
        ptbefig = beh_addbefig(NULL, figname);
        for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
            if ((ptin->TYPE & CNS_IGNORE) == CNS_IGNORE) continue;
            if ((ptin->TYPE & YAG_NOT_FUNCTIONAL) == YAG_NOT_FUNCTIONAL) continue;
            if ((ptin->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue;

            if ((ptin->TYPE & (CNS_POWER|CNS_CONE)) != 0) {
                name = ptin->UEDGE.CONE->NAME;
            }
            else name = ptin->UEDGE.LOCON->NAME;

            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
            ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
        }
    
        if ((ptcone->TYPE & CNS_LATCH) != 0 || (ptcone->TYPE & CNS_PRECHARGE) != 0) {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "f", 'O', 'B');
            expr = createAtom("aux");
            ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, "f", expr, NULL, 0);
            biexpr = yagMakeLatchExpr(ptcone);
            ptbefig->BEREG = beh_addbereg(ptbefig->BEREG, "aux", biexpr, NULL,0);
        }
        else if ((ptcone->TYPE & CNS_TRI) == CNS_TRI) {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "f", 'Z', 'W');
            biexpr = yagMakeTristateExpr(ptcone);
            ptbefig->BEBUS = beh_addbebus(ptbefig->BEBUS, "f", biexpr, NULL, 'W',0);
        }
        else if ((ptcone->TYPE & CNS_CONFLICT) == CNS_CONFLICT) {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "f", 'Z', 'W');
            biexpr = yagMakeTristateExpr(ptcone);
            ptbefig->BEBUS = beh_addbebus(ptbefig->BEBUS, "f", biexpr, NULL, 'W',0);
        }
        else {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, "f", 'O', 'B');
            expr = yagMakeDualExpr(ptcone);
            ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, "f", expr, NULL, 0);
        }

        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CNS_VDDNAME, 'I', 'B');
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, CNS_VSSNAME, 'I', 'B');
    }
    ptbefig->NAME = figname;
    savebefig(ptbefig, 0);
    beh_frebefig(ptbefig);

  /*----------------------------------------------------------+
  | On remet tous les noms en place                           |
  +----------------------------------------------------------*/
    for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_IGNORE) == CNS_IGNORE) continue;
        if ((ptin->TYPE & YAG_NOT_FUNCTIONAL) == YAG_NOT_FUNCTIONAL) continue;
        if ((ptin->TYPE & CNS_POWER) == CNS_POWER) continue;
        if ((ptin->TYPE & (CNS_BLEEDER|CNS_FEEDBACK)) != 0) continue;

        if ((ptin->TYPE & CNS_CONE) != 0) {
            ptincone = ptin->UEDGE.CONE;
            ptuser = getptype(ptincone->USER, YAG_OLDNAME_PTYPE);
            ptincone->NAME = (char *)ptuser->DATA;
            ptincone->USER = delptype(ptincone->USER, YAG_OLDNAME_PTYPE);
        }
        else {
            ptcon = ptin->UEDGE.LOCON;
            ptuser = getptype(ptcon->USER, YAG_OLDNAME_PTYPE);
            ptcon->NAME = (char *)ptuser->DATA;
            ptcon->USER = delptype(ptcon->USER, YAG_OLDNAME_PTYPE);
        }
    }
}

/****************************************************************************
 *                         function prepConeExt();                         *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
| sauvegarde puis supprime de la liste des  branche, la branche qui contient|
| un conecteur T , des cone montes sur un connecteur T. L'entree contenant  |
| le connecteur T est elle type IGNORE . Il va de soit que ces modif ne     |
| doivent etre que temporaires                                              |
+--------------------------------------------------------------------------*/
static void 
prepConeExt(ptcone)
    cone_list      *ptcone;
{
    ptype_list     *ptuser;
    locon_list     *ptcon;
    branch_list    *ptbranch;
    edge_list      *ptincone;

    ptuser = getptype(ptcone->USER, CNS_EXT);
    ptcon = (locon_list *)((chain_list *)ptuser->DATA)->DATA;

    if (ptcon->DIRECTION == 'I' || ptcon->DIRECTION == 'O') return;

    for (ptbranch = ptcone->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT != NULL) continue;
        ptbranch->TYPE |= CNS_IGNORE;
        ptincone = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
        ptincone->TYPE |= CNS_IGNORE;
    }
}

/****************************************************************************
 *                         function restoreConeExt();                      *
 ****************************************************************************/
/*-------------------------------------------------------------------------+
| Remet en place la branche contenant le connecteur INOUT supprimee par la |
| function prepConeExt.                                                    |
+--------------------------------------------------------------------------*/
static void 
restoreConeExt(ptcone)
    cone_list      *ptcone;
{
    ptype_list     *ptuser;
    locon_list     *ptcon;
    branch_list    *ptbranch;
    edge_list      *ptincone;

    ptuser = getptype(ptcone->USER, CNS_EXT);
    ptcon = (locon_list *)((chain_list *)ptuser->DATA)->DATA;

    if (ptcon->DIRECTION == 'I' || ptcon->DIRECTION == 'O') return;

    for (ptbranch = ptcone->BREXT; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT != NULL) continue;
        ptbranch->TYPE &= ~CNS_IGNORE;
        ptincone = yagGetEdge(ptcone->INCONE, ptbranch->LINK->ULINK.LOCON);
        ptincone->TYPE &= ~CNS_IGNORE;
    }
}

/****************************************************************************
 *                         function delOutConeForVst()                      *
 ****************************************************************************/
 /*------------------------------------------------------------------------+
 | Suprime les outcone pointant sur des cones sans sorties. Fonction       |
 | iterative, puisque d'une passe a l'autre, les cones perdent des outcones|
 +------------------------------------------------------------------------*/
static void 
delOutconeForVst(ptcnsfig)
    cnsfig_list    *ptcnsfig;
{
    short           change = 0;
    edge_list      *ptin;
    edge_list      *ptout;
    cone_list      *ptincone;
    cone_list      *ptoutcone;
    cone_list      *ptcone;

    while (change == 0) {

        change = -1;

        for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
            if (ptcone->OUTCONE != NULL) continue;

            for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
                if ((ptin->TYPE & CNS_EXT) == CNS_EXT) continue;

                ptincone = ptin->UEDGE.CONE;
                for (ptout = ptincone->OUTCONE; ptout != NULL; ptout = ptout->NEXT) {
                    if ((ptout->TYPE & CNS_EXT) == CNS_EXT) continue;

                    ptoutcone = ptout->UEDGE.CONE;
                    if (ptoutcone == ptcone) {
                        ptincone->OUTCONE = deledge(ptincone->OUTCONE, ptout);
                        change = 0;
                        break;
                    }
                }
            }
        }
    }
}

/****************************************************************************
 *                         function genCatal();                             *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
 | Genere le fichier CATAL contenant la liste des modeles suivi de ' C'     |
 +-------------------------------------------------------------------------*/
static void 
genCatal(headModel)
    lofig_list     *headModel;
{
    FILE           *ptCatal;
    lofig_list     *ptlofig;

    if ((ptCatal = mbkfopen(CATAL, NULL, WRITE_TEXT)) == NULL) {
        avt_errmsg(YAG_ERRMSG, "006", AVT_ERROR, CATAL);
        return;
    }

    for (ptlofig = headModel; ptlofig != NULL; ptlofig = ptlofig->NEXT)
        fprintf(ptCatal, "%s C\n", ptlofig->NAME);

    if (fclose(ptCatal) != 0)
        avt_errmsg(YAG_ERRMSG, "007", AVT_ERROR, CATAL);
}

/****************************************************************************
 *                         function prepConeMemory();                       *
 ****************************************************************************/
/*--------------------------------------------------------------------------+
| Cette function fait en sorte que le cone ressemble temporairement a un    |
| inverseur                                                                 |
+--------------------------------------------------------------------------*/
static void 
prepConeMemory(ptcone)
    cone_list      *ptcone;
{
    branch_list    *ptbranch;
    edge_list      *ptin;

    for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_LOOP) != CNS_LOOP) ptin->TYPE |= CNS_IGNORE;
    }

    /* ignore all branches of more than one link */
    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT != NULL) {
            ptbranch->TYPE |= CNS_IGNORE;
        }
    }
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if (ptbranch->LINK->NEXT != NULL) {
            ptbranch->TYPE |= CNS_IGNORE;
        }
    }
}

/****************************************************************************
 *                         function restoreConeMemory();                    *
 ****************************************************************************/
static void 
restoreConeMemory(ptcone)
    cone_list      *ptcone;
{
    branch_list    *ptbranch;
    edge_list      *ptin;

    for (ptin = ptcone->INCONE; ptin != NULL; ptin = ptin->NEXT) {
        if ((ptin->TYPE & CNS_LOOP) != CNS_LOOP)  ptin->TYPE &= ~CNS_IGNORE;
    }

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_IGNORE) == CNS_IGNORE) {
            ptbranch->TYPE &= ~(CNS_IGNORE);
        }
    }
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
        if ((ptbranch->TYPE & CNS_IGNORE) == CNS_IGNORE) {
            ptbranch->TYPE &= ~(CNS_IGNORE);
        }
    }
}

/****************************************************************************
 *                         function addGlueCones();                         *
 ****************************************************************************/
static void 
addGlueCones(ptcnsfig)
    cnsfig_list    *ptcnsfig;
{
    cone_list      *ptcone;
    cone_list      *headgluecone;
    ptype_list     *ptuser;
    long            index;

    ptuser = getptype(ptcnsfig->USER, YAG_GLUECONE_PTYPE);
    if (ptuser != NULL) {
        headgluecone = (cone_list *)ptuser->DATA;
        for (ptcone = ptcnsfig->CONE; ptcone->NEXT; ptcone = ptcone->NEXT);
        ptcone->NEXT = headgluecone;
        index = ptcone->INDEX + 1;
        for (ptcone = headgluecone; ptcone; ptcone = ptcone->NEXT) {
            ptcone->INDEX = index++;
        }
    }
}

/****************************************************************************
 *                         function removeGlueCones();                      *
 ****************************************************************************/
static void
removeGlueCones(ptcnsfig)
    cnsfig_list    *ptcnsfig;
{
    cone_list      *ptcone;
    cone_list      *headgluecone;
    ptype_list     *ptuser;

    ptuser = getptype(ptcnsfig->USER, YAG_GLUECONE_PTYPE);
    if (ptuser != NULL) {
        headgluecone = (cone_list *)ptuser->DATA;
        for (ptcone = ptcnsfig->CONE; ptcone->NEXT != headgluecone; ptcone = ptcone->NEXT);
        ptcone->NEXT = NULL;
    }
}

static void
addCellInstances(ptlofig, ptcnsfig, vddSigIndex, vssSigIndex)
    lofig_list     *ptlofig;
    cnsfig_list    *ptcnsfig;
    long            vddSigIndex;
    long            vssSigIndex;
{
    cell_list      *ptcell;
    cone_list      *ptcone;
    lofig_list     *ptmodel;
    losig_list     *ptsig;
    chain_list     *cellsiglist;
    chain_list     *sigchain;
    chain_list     *ptchain;
    char            insname[YAGBUFSIZE];
    ht             *modelTable;

    modelTable = addht(20);
    for (ptcell = ptcnsfig->CELL; ptcell; ptcell = ptcell->NEXT) {
        if (ptcell->BEFIG == NULL && (ptcell->TYPE & ~CNS_UNKNOWN) >= 128) continue;
        sigchain = NULL;
        ptmodel = (lofig_list *)gethtitem(modelTable, (void *)ptcell->TYPE);
        if (ptmodel == (lofig_list *)EMPTYHT) {
            ptmodel = cellToLomod(ptcell);
            addhtitem(modelTable, (void *)ptcell->TYPE, (long)ptmodel);
        }
        cellsiglist = (chain_list *)getptype(ptcell->USER, YAG_SIGLIST_PTYPE)->DATA;
        for (ptchain = cellsiglist; ptchain; ptchain = ptchain->NEXT) {
            ptsig = (losig_list *)ptchain->DATA;
            ptcone = (cone_list *)getptype(ptsig->USER, YAG_CONE_PTYPE)->DATA;
            sigchain = addchain(sigchain, getlosig(ptlofig, ptcone->INDEX));
        }
        sigchain = reverse(sigchain);
        sigchain = addchain(sigchain, getlosig(ptlofig, vddSigIndex));
        sigchain = addchain(sigchain, getlosig(ptlofig, vssSigIndex));
        sprintf(insname, "yagins_%s", ptcell->BEFIG->NAME);
        addloins(ptlofig, namealloc(insname), ptmodel, sigchain);
    }
}

static lofig_list *
cellToLomod(ptcell)
    cell_list      *ptcell;
{
    befig_list     *ptbefig;
    lofig_list     *ptlofig;
    bepor_list     *ptbepor;
    losig_list     *ptlosig;
    char            figname[YAGBUFSIZE];
    char            direction;
    long            index = 1;
    long            numset, numreset;

    switch (ptcell->TYPE & YAG_CELLMASK) {
        case YAG_CELL_MS_SC:
            sprintf(figname, "%s_ms_sc", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", NULL, NULL, NULL, "mem", "q", "qn", TRUE, NULL);
            break;
        case YAG_CELL_MSS_SC:
            sprintf(figname, "%s_mss_sc", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", "s", NULL, NULL, "mem", "q", "qn", TRUE, NULL);
            break;
        case YAG_CELL_MSR_SC:
            sprintf(figname, "%s_msr_sc", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", NULL, "r", NULL, "mem", "q", "qn", TRUE, NULL);
            break;
        case YAG_CELL_MSNR_SC:
            sprintf(figname, "%s_msnr_sc", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", NULL, NULL, "nr", "mem", "q", "qn", TRUE, NULL);
            break;
        case YAG_CELL_MS_SC_RT:
            sprintf(figname, "%s_ms_sc_rt", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", NULL, NULL, NULL, "mem", "q", "qn", FALSE, NULL);
            break;
        case YAG_CELL_MSS_SC_RT:
            sprintf(figname, "%s_mss_sc_rt", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", "s", NULL, NULL, "mem", "q", "qn", FALSE, NULL);
            break;
        case YAG_CELL_MSR_SC_RT:
            sprintf(figname, "%s_msr_sc_rt", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", NULL, "r", NULL, "mem", "q", "qn", FALSE, NULL);
            break;
        case YAG_CELL_MSNR_SC_RT:
            sprintf(figname, "%s_msnr_sc_rt", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSSC(figname, NULL, NULL, "d", "ck", NULL, NULL, "nr", "mem", "q", "qn", FALSE, NULL);
            break;
        case YAG_CELL_MSDIFF:
            sprintf(figname, "%s_msdiff", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigMSDIFF(figname, "d", "ck", "mem", "q", "qn", NULL);
            break;
        case YAG_CELL_FFT2:
            sprintf(figname, "%s_fft2", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigFFT2(figname, NULL, NULL, "d", "cpn", "ld", "re", "mem", "q", "qn", NULL);
            break;
        case YAG_CELL_FD2R:
            sprintf(figname, "%s_fd2r", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigFD2R(figname, NULL, NULL, "d", "cpn", "re", "mem", "q", "qn", NULL);
            break;
        case YAG_CELL_FD2S:
            sprintf(figname, "%s_fd2s", YAG_CONTEXT->YAG_CURCNSFIG->NAME);
            ptbefig = yagBuildBefigFD2S(figname, NULL, NULL, "d", "cpn", "re", "mem", "q", "qn", NULL);
            break;
        default:
            if ((ptcell->TYPE & ~CNS_UNKNOWN) < 128) {
                yagBug(DBG_ILL_CELLTYPE, "cellToLomod", NULL, NULL, 0);
            }
            else if ((ptbefig = fclGetBefig(ptcell->BEFIG->NAME)) == NULL) {
                yagBug(DBG_ILL_CELLTYPE, "cellToLomod", NULL, NULL, 0);
            }
            strcpy(figname, ptbefig->NAME);
    }
    savebefig(ptbefig, 0);

    YAG_HEAD_MODEL = addlomodel(YAG_HEAD_MODEL, namealloc(figname));
    ptlofig = YAG_HEAD_MODEL;

    for (ptbepor = ptbefig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
        ptlosig = addlosig(ptlofig, index++, NULL, EXTERNAL);
        switch (ptbepor->DIRECTION) {
            case 'I': direction = IN; break;
            case 'O': direction = OUT; break;
            case 'Z': direction = TRISTATE; break;
            case 'B': direction = INOUT; break;
            case 'T': direction = TRANSCV; break;
        }
        addlocon(ptlofig, ptbepor->NAME, ptlosig, direction);
    }
    ptlofig->LOCON = (locon_list *)reverse((chain_list *)ptlofig->LOCON);

    savelofig(ptlofig);
    beh_frebefig(ptbefig);

    return ptlofig;
}

static chain_list *
labelEnum(root, num)
    char        *root;
    long          num;
{
    chain_list  *labels = NULL;
    char         buf[64];
    int          i;

    for (i=1; i<= num; i++) {
        sprintf(buf, "%s%d", root, i);
        labels = addchain(labels, namealloc(buf));
    }
    return labels;
}
