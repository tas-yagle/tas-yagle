/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_befig.c                                                 */
/*                                                                          */
/*    (c) copyright 1991 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 15/08/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

#define XCON_IN    1
#define XCON_OUT   2
#define XCON_INOUT 3

static void complete_befig __P((cnsfig_list *ptcnsfig, befig_list *ptbefig));
static void build_core_interface __P((lofig_list *ptlofig, cnsfig_list *ptcnsfig, befig_list *ptbefig, chain_list *instances));
static void build_interface __P((cnsfig_list *ptcnsfig, befig_list *ptbefig));
static void build_port __P((locon_list *ptcon, befig_list *ptbefig, char type, mbk_match_rules *keepbus_mr));
static int orient_connector (locon_list *ptlocon, int merge, int isintcon);
static int test_suppress __P((berin_list *ptberin));

/*****************************************************************************
 *                          function yagBuildBefig()                         *
 *****************************************************************************/

/* build the global behavioural figure for a cone net-list */

void
yagBuildBefig(ptcnsfig, name)
    cnsfig_list *ptcnsfig;
    char        *name;
{
//    chain_list  *ptchain;
    befig_list  *ptbefig;
//    befig_list  *ptinsbefig;
//    bepor_list  *ptbepor;
//    loins_list  *ptloins;
//    ht          *befight;

    ptbefig = yagInitBefig(name);
    yagInitBehDefined();
    ptcnsfig->BEFIG = ptbefig;
    ptbefig->NEXT = YAG_CONTEXT->YAG_BEFIG_LIST;
    YAG_CONTEXT->YAG_BEFIG_LIST = ptbefig;

    build_interface(ptcnsfig, ptbefig);
    complete_befig(ptcnsfig, ptbefig);

    yagCleanBehVect();
    yagCleanBehDefined();
}

void
yagBuildCoreBefig(ptlofig, ptcnsfig, instances)
    lofig_list  *ptlofig;
    cnsfig_list *ptcnsfig;
    chain_list  *instances;
{
    chain_list  *ptchain;
    befig_list  *ptbefig;
    befig_list  *ptinsbefig;
    bepor_list  *ptbepor;
    loins_list  *ptloins;
    ht          *befight;

    ptbefig = yagInitBefig(ptlofig->NAME);
    yagInitBehDefined();
    ptcnsfig->BEFIG = ptbefig;

    build_core_interface(ptlofig, ptcnsfig, ptbefig, instances);
    complete_befig(ptcnsfig, ptbefig);

    /* Add instance behaviour */
    
    if (instances != NULL) {
        beg_def_befig(ptbefig->NAME);
        befight = addht(40);
        for (ptchain = YAG_CONTEXT->YAGLE_INSTANCE_BEFIGS; ptchain; ptchain = ptchain->NEXT) {
            ptinsbefig = (befig_list *)ptchain->DATA;
            addhtitem(befight, ptbefig->NAME, (long)ptinsbefig);
        }
        ptbefig->BEPOR = (bepor_list *)reverse((chain_list *)ptbefig->BEPOR);
        for (ptbepor = ptbefig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
            beg_def_por(ptbepor->NAME, ptbepor->DIRECTION);
        }
        beg_eat_figure(ptbefig);
        for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
            ptloins = (loins_list *)ptchain->DATA;
            ptinsbefig = (befig_list *)gethtitem(befight, ptloins->INSNAME);
            if (ptinsbefig != (void *)EMPTYHT) beg_eat_figure(ptinsbefig);
        }
        ptcnsfig->BEFIG = beg_get_befig(BEG_NOVERIF);
    }
    yagCleanBehVect();
    yagCleanBehDefined();
}

void
complete_befig(ptcnsfig, ptbefig)
    cnsfig_list *ptcnsfig;
    befig_list  *ptbefig;
{
    cone_list   *ptcone;
    union beobj  ubeobj;
    cell_list   *ptcell;
    chain_list  *ptchain;
    chain_list  *latchbefigs = NULL;
    chain_list  *busbefigs = NULL;
    ptype_list  *ptuser;
    long        type;
    int         existOne = FALSE;
    int         existZero = FALSE;
    mbk_match_rules keepbus_mr;

    /* Add behavioural description of normal cones */
    
    inf_buildmatchrule(getloadedinffig(ptcnsfig->NAME), INF_KEEP_TRISTATE_BEHAVIOUR, &keepbus_mr, 0);

    for (ptcone = ptcnsfig->CONE; ptcone; ptcone = ptcone->NEXT) {
        if (ptcone->CELLS != NULL) {
            for (ptchain = ptcone->CELLS; ptchain; ptchain = ptchain->NEXT) {
                if (((cell_list *)ptchain->DATA)->BEFIG == NULL) break;
            }
            if (ptchain == NULL) continue;
        }
        
        if ((ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE)) != NULL) {
            latchbefigs = addchain(latchbefigs, ptuser->DATA);
            ptcone->USER = delptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
            continue;
        }

        if ((ptcone->TYPE & CNS_TRI) != 0) {
            if ((ptuser = getptype(ptcone->USER, YAG_BUSBEFIG_PTYPE)) != NULL) {
                busbefigs = addchain(busbefigs, ptuser->DATA);
                ptcone->USER = delptype(ptcone->USER, YAG_BUSBEFIG_PTYPE);
                continue;
            }
        }

        avt_log(LOGYAG,1,"Generating equation for '%s'\n", ptcone->NAME);

        if ((ptcone->TYPE & (CNS_VDD|CNS_VSS)) != 0) {
            if ((ptcone->TYPE & CNS_VDD) == CNS_VDD) existOne = TRUE;
            else existZero = TRUE;
        }
        else yagAddConeBehaviour(ptcone, ptbefig, ptcone, &keepbus_mr);

        if ((ptcone->TECTYPE & (CNS_ONE|CNS_ZERO)) != 0) {
            if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) existOne = TRUE;
            else existZero = TRUE;
        }
    }

    /* Add behavioural description of latchs */

    for (ptchain = latchbefigs; ptchain; ptchain = ptchain->NEXT) {
        yagAddBefigBehaviour((befig_list *)ptchain->DATA, ptbefig, FALSE);
        beh_frebefig((befig_list *)ptchain->DATA);
    }
    freechain(latchbefigs);

    /* Add behavioural description of buses */

    for (ptchain = busbefigs; ptchain; ptchain = ptchain->NEXT) {
        yagAddBefigBehaviour((befig_list *)ptchain->DATA, ptbefig, FALSE);
        beh_frebefig((befig_list *)ptchain->DATA);
    }
    freechain(busbefigs);

    /* Add behavioural description of cells */

    for (ptcell = ptcnsfig->CELL; ptcell; ptcell = ptcell->NEXT) {
        if (ptcell->BEFIG == NULL) continue;
        if (ptcell->CONES == NULL) {
            yagAddBefigBehaviour(ptcell->BEFIG, ptbefig, TRUE);
            yagReorientInterface(ptcnsfig, ptcell->BEFIG);
        }
        else yagAddBefigBehaviour(ptcell->BEFIG, ptbefig, FALSE);
        beh_frebefig(ptcell->BEFIG);
    }

    /* Add behavioural description of cell glue cones if required */

    ptuser = getptype(ptcnsfig->USER, YAG_GLUECONE_PTYPE);
    if (ptuser != NULL) {
        for (ptcone = (cone_list *)ptuser->DATA; ptcone; ptcone = ptcone->NEXT) {
            if (yagBehDefined(ptcone->NAME, &type, &ubeobj) == FALSE) {
                yagAddConeBehaviour(ptcone, ptbefig, NULL, &keepbus_mr);
            }
        }
    }

    if (existZero || FCL_NEED_ZERO) {
        yagAddBeauxVect(ptbefig, namealloc("yag_zero"), createAtom("'0'"), NULL, NULL, 0);
        yagAddBerinVect(ptbefig, namealloc("yag_zero"));
    }
    if (existOne || FCL_NEED_ONE) {
        yagAddBeauxVect(ptbefig, namealloc("yag_one"), createAtom("'1'"), NULL, NULL, 0);
        yagAddBerinVect(ptbefig, namealloc("yag_one"));
    }
    yagCleanBehDefined();
    mbk_FreeREGEX(&keepbus_mr);
}

/*****************************************************************************
 *                          function yagAddBefigBehaviour()                  *
 *****************************************************************************/

/* add the befig behaviour of a cone to the main figure */

void
yagAddBefigBehaviour(ptbefig, ptcircuitbefig, merge)
    befig_list *ptbefig;
    befig_list *ptcircuitbefig;
    int         merge;
{
    beaux_list  *ptbeaux;
    beaux_list  *ptbedly;
    bebux_list  *ptbebux;
    bebus_list  *ptbebus;
    beout_list  *ptbeout;
    bereg_list  *ptbereg;
    bemsg_list  *ptbemsg;
    bepor_list  *ptbepor;
    bequad_list *ptbequad;
    union beobj  ubeobj;
    long         type;
    int          already_defined;

    if (ptbefig->TIME_UNIT != 0) {
        ptcircuitbefig->TIME_UNIT = ptbefig->TIME_UNIT;
    }

    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        if (strncmp(ptbeout->NAME, "yagaux_", 7) == 0) continue;
        if (yagBehDefined(ptbeout->NAME, &type, &ubeobj) == FALSE) {
            if ((ptbepor = yagBeporDefined(ptbeout->NAME)) != NULL) {
                yagAddBeoutVect(ptcircuitbefig, ptbeout->NAME, ptbeout->ABL, (bequad_list *)ptbeout->NODE, NULL, ptbeout->TIME); 
            }
            else {
                ptbequad = (bequad_list *)ptbeout->NODE;
                ptbequad = (bequad_list *)yagAddBeauxVect(ptcircuitbefig, ptbeout->NAME, ptbeout->ABL, ptbequad, NULL, ptbeout->TIME)->NODE; 
                ptbequad->USER = addptype(ptbequad->USER, YAG_LOCK_PTYPE, NULL);
                yagAddBerinVect(ptcircuitbefig, ptbeout->NAME);
            }
            ptbeout->ABL=NULL; ptbeout->NODE=NULL;
        }
        else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, ptbeout->NAME, NULL, 0);
    }  
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        already_defined = yagBehDefined(ptbebus->NAME, &type, &ubeobj);
        if (merge || !already_defined) {
            if (ubeobj.BEBUX != NULL) {
                if (type == YAG_BEBUX) {
                    ubeobj.BEBUX->BIABL = yagMergeBiabl(ptbebus->NAME, ubeobj.BEBUX->BIABL, ptbebus->BIABL, FALSE);
                    ptbebus->BIABL=NULL;
                }
                else if (type == YAG_BEBUS) {
                    ubeobj.BEBUS->BIABL = yagMergeBiabl(ptbebus->NAME, ubeobj.BEBUS->BIABL, ptbebus->BIABL, FALSE);
                    ptbebus->BIABL=NULL;
                }
                else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, ptbebus->NAME, NULL, 0);
            }
            else {
                if ((ptbepor = yagBeporDefined(ptbebus->NAME)) != NULL) {
                    yagAddBebusVect(ptcircuitbefig, ptbebus->NAME, ptbebus->BIABL, ptbebus->BINODE, NULL); 
                }
                else {
                    yagAddBebuxVect(ptcircuitbefig, ptbebus->NAME, ptbebus->BIABL, ptbebus->BINODE, NULL); 
                    yagAddBerinVect(ptcircuitbefig, ptbebus->NAME);
                }
                ptbebus->BIABL=NULL; ptbebus->BINODE=NULL;
            }
        }
    }  
    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        if (yagBehDefined(ptbeaux->NAME, &type, &ubeobj) == FALSE) {
            ptbequad = (bequad_list *)ptbeaux->NODE;
            ptbequad = (bequad_list *)yagAddBeauxVect(ptcircuitbefig, ptbeaux->NAME, ptbeaux->ABL, ptbequad, NULL, ptbeaux->TIME)->NODE; 
            ptbequad->USER = addptype(ptbequad->USER, YAG_LOCK_PTYPE, NULL);
            yagAddBerinVect(ptcircuitbefig, ptbeaux->NAME);
            ptbeaux->ABL=NULL;
        }
        else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, ptbeaux->NAME, NULL, 0);
    }
    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        already_defined = yagBehDefined(ptbebux->NAME, &type, &ubeobj);
        if (merge || !already_defined) {
            if (ubeobj.BEBUX != NULL) {
                if (type == YAG_BEBUX) {
                    ubeobj.BEBUX->BIABL = yagMergeBiabl(ptbebux->NAME, ubeobj.BEBUX->BIABL, ptbebux->BIABL, FALSE);
                    ptbebux->BIABL=NULL;
                }
                else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, ptbebux->NAME, NULL, 0);
            }
            else {
                yagAddBebuxVect(ptcircuitbefig, ptbebux->NAME, ptbebux->BIABL, ptbebux->BINODE, NULL); 
                yagAddBerinVect(ptcircuitbefig, ptbebux->NAME);
                ptbebux->BIABL=NULL; ptbebux->BINODE=NULL;
            }
        }
    }
    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        already_defined = yagBehDefined(ptbereg->NAME, &type, &ubeobj);
        if (merge || !already_defined) {
            if (ubeobj.BEREG != NULL) {
                if (type == YAG_BEREG) {
                    ubeobj.BEREG->BIABL = yagMergeBiabl(ptbereg->NAME, ubeobj.BEREG->BIABL, ptbereg->BIABL, TRUE);
                    ptbereg->BIABL=NULL;
                }
                else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, ptbereg->NAME, NULL, 0);
            }
            else {
                yagAddBeregVect(ptcircuitbefig, ptbereg->NAME, ptbereg->BIABL, ptbereg->BINODE, NULL);
                yagAddBerinVect(ptcircuitbefig, ptbereg->NAME);
                ptbereg->BIABL=NULL; ptbereg->BINODE=NULL;
            }
        }
        else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, ptbereg->NAME, NULL, 0);
    }
    for (ptbedly = ptbefig->BEDLY; ptbedly; ptbedly = ptbedly->NEXT) {
        if (yagGetBedly(ptcircuitbefig, ptbedly->NAME) == NULL) {
            ptcircuitbefig->BEDLY = beh_addbeaux(ptcircuitbefig->BEDLY, ptbedly->NAME, ptbedly->ABL,NULL,0);
            yagAddBerinVect(ptcircuitbefig, ptbedly->NAME);
            ptbedly->ABL=NULL;
        }
    }
    for (ptbemsg = ptbefig->BEMSG; ptbemsg; ptbemsg = ptbemsg->NEXT) {
        ptcircuitbefig->BEMSG = beh_addbemsg(ptcircuitbefig->BEMSG, ptbemsg->LABEL, ptbemsg->LEVEL, ptbemsg->MESSAGE, ptbemsg->ABL, NULL); 
        ptbemsg->ABL=NULL;
    }
}

/*****************************************************************************
 *                          function yagMergeBiabl()                         *
 *****************************************************************************/

/* merge two biabls according to precedence setting */

biabl_list *
yagMergeBiabl(name, ptbiabl_orig, ptbiabl_new, is_mem)
    char       *name;
    biabl_list *ptbiabl_orig;
    biabl_list *ptbiabl_new;
    int         is_mem;
{
    biabl_list *ptbiabl_endorig;
    biabl_list *ptbiabl_endnew;
    biabl_list *ptprevious;
    chain_list *orig_default = NULL;
    chain_list *new_default = NULL;

    if (ptbiabl_orig == NULL) return ptbiabl_new;
    if (ptbiabl_new == NULL) return ptbiabl_orig;

    if (!YAG_CONTEXT->YAG_ASSUME_PRECEDE) {
        ptbiabl_new->FLAG |= BEH_CND_NOPRECEDE;
        return (biabl_list *)append((chain_list *)ptbiabl_orig, (chain_list *)ptbiabl_new);
    }

    ptbiabl_new->FLAG |= BEH_CND_PRECEDE;

    if (is_mem) {
        return (biabl_list *)append((chain_list *)ptbiabl_orig, (chain_list *)ptbiabl_new);
    }

    ptprevious = NULL;
    for (ptbiabl_endorig = ptbiabl_orig; ptbiabl_endorig->NEXT; ptbiabl_endorig = ptbiabl_endorig->NEXT) {
        ptprevious = ptbiabl_endorig;
    }
    if (ptprevious && ATOM(ptbiabl_endorig->CNDABL) && strcmp(VALUE_ATOM(ptbiabl_endorig->CNDABL), "'1'") == 0) {
        orig_default = ptbiabl_endorig->VALABL;
        ptbiabl_endorig->VALABL = NULL;
        ptprevious->NEXT = ptbiabl_new;
    }
    else ptbiabl_endorig->NEXT = ptbiabl_new;

    for (ptbiabl_endnew = ptbiabl_new; ptbiabl_endnew->NEXT; ptbiabl_endnew = ptbiabl_endnew->NEXT);
    if (ATOM(ptbiabl_endnew->CNDABL) && strcmp(VALUE_ATOM(ptbiabl_endnew->CNDABL), "'1'") == 0) {
        new_default = ptbiabl_endnew->VALABL;
        ptbiabl_endnew->VALABL = NULL;
    }

    if (orig_default != NULL && new_default != NULL) {
        if (equalExpr(orig_default, new_default)) {
            ptbiabl_endnew->VALABL = new_default;
            freeExpr(orig_default);
        }
        else yagWarning(WAR_MULTIPLE_BEOBJ, NULL, name, NULL, 0);
        beh_frebiabl(ptbiabl_endorig);
    }
    else if (new_default != NULL) {
        ptbiabl_endnew->VALABL = new_default;
    }
    else if (orig_default != NULL) {
        ptbiabl_endnew->NEXT = ptbiabl_endorig;
        ptbiabl_endorig->VALABL = orig_default;
    }
    return ptbiabl_orig;
}

/*****************************************************************************
 *                          function yagAddConeBehaviour()                   *
 *****************************************************************************/

/* add the cone behaviour to the figure */

void
yagAddConeBehaviour(cone_list *ptcone, befig_list *ptbefig, cone_list  *ptlinkcone, mbk_match_rules *keepbus_mr)
{
    cone_list   *ptregcone;
    chain_list  *ptabl;
    biabl_list  *ptlatcheqn, *ptbiabl;
    char        *name;
    char        buff[YAGBUFSIZE];
    int         clockedlatch, forcebus;
    bebux_list *bux;
            
    clockedlatch = ((ptcone->TYPE & (CNS_LATCH|CNS_RS)) == CNS_LATCH);

    if ((ptcone->TYPE & CNS_EXT) == 0 || clockedlatch) {
        if (clockedlatch) {
            ptlatcheqn = yagMakeLatchExpr(ptcone);
            if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
                sprintf(buff, "yagaux_%s", ptcone->NAME);
                name = namealloc(buff);
            }
            else name = ptcone->NAME;
            yagAddBeregVect(ptbefig, name, ptlatcheqn, NULL, ptlinkcone);
            yagAddBerinVect(ptbefig, name);
        }
        else if ((ptcone->TYPE & (CNS_MEMSYM|CNS_LATCH)) == CNS_MEMSYM) {
            ptregcone = (cone_list *)getptype(ptcone->USER, YAG_MEMORY_PTYPE)->DATA;
            ptabl = notExpr(createAtom(ptregcone->NAME));
            yagAddBeauxVect(ptbefig, ptcone->NAME, ptabl, NULL, ptlinkcone, 0);
            yagAddBerinVect(ptbefig, ptcone->NAME);
        }
        else if ((ptcone->TYPE & (CNS_TRI|CNS_CONFLICT)) != 0) {
            ptbiabl = yagMakeTristateExpr(ptcone);
            if (ptbiabl != NULL) {
                if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
            }
            forcebus = mbk_CheckREGEX(keepbus_mr, ptcone->NAME);
            if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0 || (ptcone->TYPE & CNS_PRECHARGE) != 0 || (YAG_CONTEXT->YAG_TRISTATE_MEMORY && !forcebus && !YAG_CONTEXT->YAG_BLEEDER_PRECHARGE)) {
                yagAddBeregVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptlinkcone);
                
            }
            else {
                bux=yagAddBebuxVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptlinkcone);
                if (forcebus) bux->FLAGS|=BEH_FLAG_FORCEBUS;
            }
            yagAddBerinVect(ptbefig, ptcone->NAME);
        }
        else {
            if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
                ptabl = createAtom(namealloc("yag_one"));
            }
            else if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
                ptabl = createAtom(namealloc("yag_zero"));
            }
            else ptabl = yagMakeDualExpr(ptcone);
            if (ptabl == NULL) {
                ptabl = createAtom("'u'");
            }
            yagAddBeauxVect(ptbefig, ptcone->NAME, ptabl, NULL, ptlinkcone, 0);
            yagAddBerinVect(ptbefig, ptcone->NAME);
        }
    }
}

/*****************************************************************************
 *                          function build_core_interface()                  *
 *****************************************************************************/

/* build the external interface to the core behavioural figure */

static void
build_core_interface(ptlofig, ptcnsfig, ptbefig, instances)
    lofig_list  *ptlofig;
    cnsfig_list *ptcnsfig;
    befig_list  *ptbefig;
    chain_list  *instances;
{
    locon_list  *ptlocon, *ptcnslocon;
    loins_list  *ptloins;
    chain_list  *ptchain;
    ptype_list  *ptuser;
    mbk_match_rules keepbus_mr;
    inf_buildmatchrule(getloadedinffig(ptcnsfig->NAME), INF_KEEP_TRISTATE_BEHAVIOUR, &keepbus_mr, 0);

    for (ptlocon = ptlofig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        ptcnslocon = (locon_list *)getptype(ptlocon->USER, YAG_LOCON_PTYPE)->DATA;
        if (((ptcnslocon->DIRECTION == CNS_VDDC) || (ptcnslocon->DIRECTION == CNS_VSSC))
        && (YAG_CONTEXT->YAG_ONE_SUPPLY||YAG_CONTEXT->YAG_NO_SUPPLY)) continue;

        ptuser = getptype(ptcnslocon->USER, CNS_EXT);
        if (ptuser == NULL) {
            if (getptype(ptcnslocon->USER, CNS_CONE) != NULL) {
                yagAddBeporVect(ptbefig, ptcnslocon->NAME, 'I', 'B');
                yagAddBerinVect(ptbefig, ptcnslocon->NAME);
            }
            else yagAddBeporVect(ptbefig, ptcnslocon->NAME, 'X', 'B');
        }
        else build_port(ptcnslocon, ptbefig, EXTERNAL, &keepbus_mr);
        ptcnslocon->SIG->USER = addptype(ptcnslocon->SIG->USER, YAG_DONE_PTYPE, NULL);
        ptlocon->USER = delptype(ptlocon->USER, YAG_LOCON_PTYPE);
    }

    for (ptchain = instances; ptchain; ptchain = ptchain->NEXT) {
        ptloins = (loins_list *)ptchain->DATA;
        for (ptlocon = ptloins->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
            if (getptype(ptlocon->SIG->USER, YAG_DONE_PTYPE) != NULL) continue;
            if (getptype(ptlocon->SIG->USER, YAG_MARKSIG_PTYPE) == NULL) {
                ptlocon->SIG->USER = addptype(ptlocon->SIG->USER, YAG_MARKSIG_PTYPE, NULL);
            }
        }
    }

    for (ptcnslocon = ptcnsfig->INTCON; ptcnslocon; ptcnslocon = ptcnslocon->NEXT) {
        if (getptype(ptcnslocon->SIG->USER, YAG_MARKSIG_PTYPE) == NULL) continue;
        ptuser = getptype(ptcnslocon->USER, CNS_EXT);
        if (ptuser == NULL) {
            if (getptype(ptcnslocon->USER, CNS_CONE) != NULL) {
                yagAddBerinVect(ptbefig, ptcnslocon->NAME);
            }
        }
        else build_port(ptcnslocon, ptbefig, INTERNAL, &keepbus_mr);
    }

    for (ptcnslocon = ptcnsfig->LOCON; ptcnslocon; ptcnslocon = ptcnslocon->NEXT) {
        if (getptype(ptcnslocon->SIG->USER, YAG_MARKSIG_PTYPE) != NULL) {
            ptcnslocon->SIG->USER = delptype(ptcnslocon->SIG->USER, YAG_MARKSIG_PTYPE);
        }
        if (getptype(ptcnslocon->SIG->USER, YAG_DONE_PTYPE) != NULL) {
            ptcnslocon->SIG->USER = delptype(ptcnslocon->SIG->USER, YAG_DONE_PTYPE);
        }
    }
    for (ptcnslocon = ptcnsfig->INTCON; ptcnslocon; ptcnslocon = ptcnslocon->NEXT) {
        if (getptype(ptcnslocon->SIG->USER, YAG_MARKSIG_PTYPE) != NULL) {
            ptcnslocon->SIG->USER = delptype(ptcnslocon->SIG->USER, YAG_MARKSIG_PTYPE);
        }
        if (getptype(ptcnslocon->SIG->USER, YAG_DONE_PTYPE) != NULL) {
            ptcnslocon->SIG->USER = delptype(ptcnslocon->SIG->USER, YAG_DONE_PTYPE);
        }
    }
    mbk_FreeREGEX(&keepbus_mr);
}

/*****************************************************************************
 *                          function build_interface()                       *
 *****************************************************************************/

/* build the external interface to the behavioural figure */

static void
build_interface(ptcnsfig, ptbefig)
    cnsfig_list *ptcnsfig;
    befig_list  *ptbefig;
{
    locon_list  *ptcon;
    ptype_list  *ptuser;

    mbk_match_rules keepbus_mr;
    inf_buildmatchrule(getloadedinffig(ptcnsfig->NAME), INF_KEEP_TRISTATE_BEHAVIOUR, &keepbus_mr, 0);

    for (ptcon = ptcnsfig->LOCON; ptcon; ptcon = ptcon->NEXT) {
        if (((ptcon->DIRECTION == CNS_VDDC) || (ptcon->DIRECTION == CNS_VSSC))
        && (YAG_CONTEXT->YAG_ONE_SUPPLY||YAG_CONTEXT->YAG_NO_SUPPLY)) continue;

        ptuser = getptype(ptcon->USER, CNS_EXT);
        if (ptuser == NULL) {
            if (getptype(ptcon->USER, CNS_CONE) != NULL) {
                yagAddBeporVect(ptbefig, ptcon->NAME, 'I', 'B');
                yagAddBerinVect(ptbefig, ptcon->NAME);
            }
            else yagAddBeporVect(ptbefig, ptcon->NAME, 'X', 'B');
        }
        else build_port(ptcon, ptbefig, EXTERNAL, &keepbus_mr);
    }

    for (ptcon = ptcnsfig->INTCON; ptcon; ptcon = ptcon->NEXT) {
        ptuser = getptype(ptcon->USER, CNS_EXT);
        if (ptuser == NULL) {
            if (getptype(ptcon->USER, CNS_CONE) != NULL) {
                yagAddBerinVect(ptbefig, ptcon->NAME);
            }
        }
        else build_port(ptcon, ptbefig, INTERNAL, &keepbus_mr);
    }

    if (YAG_CONTEXT->YAG_ONE_SUPPLY) {
        yagAddBeporVect(ptbefig, CNS_VDDNAME, 'I', 'B');
        yagAddBerinVect(ptbefig, CNS_VDDNAME);
        yagAddBeporVect(ptbefig, CNS_VSSNAME, 'I', 'B');
        yagAddBerinVect(ptbefig, CNS_VSSNAME);
    }
    mbk_FreeREGEX(&keepbus_mr);
}

/*****************************************************************************
 *                          function build_port()                            *
 *****************************************************************************/

/* add the connector to the befig */

static void
build_port(locon_list *ptcon, befig_list *ptbefig, char type, mbk_match_rules *keepbus_mr)
{
    cone_list   *ptcone;
    cone_list   *ptregcone;
    chain_list  *expr = NULL;
    biabl_list  *ptbiabl = NULL;
    befig_list  *ptcellbefig;
    beout_list  *ptbeout;
    bebus_list  *ptbebus;
    bereg_list  *ptbereg;
    chain_list  *ptchain;
    ptype_list  *ptuser;
    char        *name;
    char        buff[YAGBUFSIZE];
    int         described;
    int         clockedlatch;
    int         cellregister = FALSE;
    char        direction;
    bebux_list *bux;
    int forcebus;

    ptcone = (cone_list *)getptype(ptcon->USER, CNS_EXT)->DATA;
    clockedlatch = ((ptcone->TYPE & (CNS_LATCH|CNS_RS)) == CNS_LATCH);

    if (clockedlatch) {
        sprintf(buff, "yagaux_%s", ptcone->NAME);
        name = namealloc(buff);
    }
    else name = ptcone->NAME;

    /* add to befig */
    if (ptcon->DIRECTION == 'I' || ptcon->DIRECTION == 'X') {
        if (type == EXTERNAL) yagAddBeporVect(ptbefig, ptcone->NAME, 'I', 'B');
        if (type == EXTERNAL) yagAddBerinVect(ptbefig, ptcone->NAME);
    }
    else {
        /* generate the cone equations */
        described = FALSE;
        ptcellbefig = NULL;
        if (ptcone->CELLS != NULL) {
            for (ptchain = ptcone->CELLS; ptchain; ptchain = ptchain->NEXT) {
                if (((cell_list *)ptchain->DATA)->BEFIG != NULL) break;
            }
            if (ptchain != NULL) {
                ptcellbefig = ((cell_list *)ptchain->DATA)->BEFIG;
            }
        }
        if ((ptuser = getptype(ptcone->USER, YAG_BUSBEFIG_PTYPE)) != NULL) {
            ptcellbefig = (befig_list *)ptuser->DATA;
        }
        if ((ptuser = getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE)) != NULL) {
            ptcellbefig = (befig_list *)ptuser->DATA;
        }
        if (ptcellbefig != NULL) {
            ptbeout = yagGetBeout(ptcellbefig, ptcone->NAME);
            if (ptbeout != NULL) {
                expr = copyExpr(ptbeout->ABL);
                if (expr) described = TRUE;
            }
            if (!described) {
                ptbebus = yagGetBebus(ptcellbefig, ptcone->NAME);
                if (ptbebus != NULL) {
                    ptbiabl = beh_dupbiabl(ptbebus->BIABL);
                    if (ptbiabl) described = TRUE;
                }
            }
            if (!described) {
                ptbereg = yagGetBereg(ptcellbefig, ptcone->NAME);
                if (ptbereg != NULL) {
                    ptbiabl = beh_dupbiabl(ptbereg->BIABL);
                    if (ptbiabl) {
                        described = TRUE;
                        cellregister = TRUE;
                        sprintf(buff, "yagaux_%s", ptcone->NAME);
                        name = namealloc(buff);
                    }
                }
            }
        }
        if (!described) {
            if ((ptcone->TYPE & (CNS_TRI|CNS_CONFLICT)) != 0) {
                ptbiabl = yagMakeTristateExpr(ptcone);
                if (ptbiabl->NEXT != NULL) ptbiabl->NEXT->FLAG |= BEH_CND_PRECEDE;
            }
            else if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH) {
                expr = createAtom(name);
            }
            else if ((ptcone->TYPE & (CNS_MEMSYM|CNS_LATCH)) == CNS_MEMSYM) {
                ptregcone = (cone_list *)getptype(ptcone->USER, YAG_MEMORY_PTYPE)->DATA;
                expr = notExpr(createAtom(ptregcone->NAME));
            }
            else {
                if ((ptcone->TECTYPE & CNS_ONE) == CNS_ONE) {
                    expr = createAtom(namealloc("yag_one"));
                }
                else if ((ptcone->TECTYPE & CNS_ZERO) == CNS_ZERO) {
                    expr = createAtom(namealloc("yag_zero"));
                }
                else expr = yagMakeDualExpr(ptcone);
            }
        }

        /* add to befig */
        direction = ptcon->DIRECTION;
        if (ptcon->DIRECTION == 'O' && expr == NULL && ptbiabl != NULL) {
            yagError(ERR_CON_DIR, ptcon->NAME, NULL, NULL, 0, 0);
            direction = 'Z';
        }
        if (ptcon->DIRECTION == 'B' && expr == NULL && ptbiabl != NULL) {
            yagError(ERR_CON_DIR, ptcon->NAME, NULL, NULL, 0, 0);
            direction = 'T';
        }
        switch (direction) {

            case 'O':
                if (type == EXTERNAL) yagAddBeporVect(ptbefig, ptcone->NAME, 'O', 'B');
                if (type == INTERNAL) yagAddBerinVect(ptbefig, ptcone->NAME);
                if (type == EXTERNAL && expr != NULL && !clockedlatch) yagAddBeoutVect(ptbefig, ptcone->NAME, expr, NULL, ptcone, 0);
                if (type == EXTERNAL && expr != NULL && clockedlatch) yagAddBeoutVect(ptbefig, ptcone->NAME, expr, NULL, NULL, 0);
                if (type == INTERNAL && expr != NULL) yagAddBeauxVect(ptbefig, ptcone->NAME, expr, NULL, ptcone, 0);
                break;
            case 'B':
                if (type == EXTERNAL) yagAddBeporVect(ptbefig, ptcone->NAME, 'B', 'B');
                yagAddBerinVect(ptbefig, ptcone->NAME);
                if (type == EXTERNAL && expr != NULL && !clockedlatch) yagAddBeoutVect(ptbefig, ptcone->NAME, expr, NULL, ptcone, 0);
                if (type == EXTERNAL && expr != NULL && clockedlatch) yagAddBeoutVect(ptbefig, ptcone->NAME, expr, NULL, NULL, 0);
                if (type == INTERNAL && expr != NULL) yagAddBeauxVect(ptbefig, ptcone->NAME, expr, NULL, ptcone, 0);
                break;
            case 'Z':
                if (type == EXTERNAL && !cellregister) yagAddBeporVect(ptbefig, ptcone->NAME, 'Z', 'M');
                if (type == EXTERNAL && cellregister) yagAddBeporVect(ptbefig, ptcone->NAME, 'O', 'B');
                if (type == INTERNAL) yagAddBerinVect(ptbefig, ptcone->NAME);
                if (type == EXTERNAL && ptbiabl != NULL && !cellregister) yagAddBebusVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptcone);
                if (type == EXTERNAL && ptbiabl != NULL && cellregister) {
                    yagAddBerinVect(ptbefig, name);
                    yagAddBeoutVect(ptbefig, ptcone->NAME, createAtom(name), NULL, NULL, 0);
                    yagAddBeregVect(ptbefig, name, ptbiabl, NULL, ptcone);
                }
                forcebus=mbk_CheckREGEX(keepbus_mr, ptcone->NAME);
                if (type == INTERNAL && ptbiabl != NULL) {
                    if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0 || (YAG_CONTEXT->YAG_TRISTATE_MEMORY && !YAG_CONTEXT->YAG_BLEEDER_PRECHARGE)) {
                        yagAddBeregVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptcone);
                    }
                    else {
                            bux=yagAddBebuxVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptcone);
                           if (forcebus) bux->FLAGS|=BEH_FLAG_FORCEBUS;
                    }
                }
                break;
            case 'T':
                if (type == EXTERNAL && !cellregister) yagAddBeporVect(ptbefig, ptcone->NAME, 'T', 'M');
                if (type == EXTERNAL && cellregister) yagAddBeporVect(ptbefig, ptcone->NAME, 'B', 'B');
                yagAddBerinVect(ptbefig, ptcone->NAME);
                if (type == EXTERNAL && ptbiabl != NULL && !cellregister) yagAddBebusVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptcone);
                if (type == EXTERNAL && ptbiabl != NULL && cellregister) {
                    yagAddBerinVect(ptbefig, name);
                    yagAddBeoutVect(ptbefig, ptcone->NAME, createAtom(name), NULL, NULL, 0);
                    yagAddBeregVect(ptbefig, name, ptbiabl, NULL, ptcone);
                }
                forcebus=mbk_CheckREGEX(keepbus_mr, ptcone->NAME);
                if (type == INTERNAL && ptbiabl != NULL) {
                    if ((ptcone->TECTYPE & YAG_LEVELHOLD) != 0 || (YAG_CONTEXT->YAG_TRISTATE_MEMORY && !YAG_CONTEXT->YAG_BLEEDER_PRECHARGE)) {
                        yagAddBeregVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptcone);
                    }
                    else {
                        bux=yagAddBebuxVect(ptbefig, ptcone->NAME, ptbiabl, NULL, ptcone);
                        if (forcebus) bux->FLAGS|=BEH_FLAG_FORCEBUS;
                    }
                }
                break;
        }
    }
}

/*****************************************************************************
 *                          function yagOrientInterface()                    *
 *****************************************************************************/

/* orient the external connectors of the cone net-list */

void yagOrientInterface(ptcnsfig)
    cnsfig_list *ptcnsfig;
{
    locon_list  *ptlocon;
//    ptype_list  *ptuser;
    int         numbaddir = 0;

    /* check that external connector orientation conforms */
    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        numbaddir += orient_connector(ptlocon, TRUE, FALSE);
    }

    for (ptlocon = ptcnsfig->INTCON; ptlocon; ptlocon = ptlocon->NEXT) {
        numbaddir += orient_connector(ptlocon, FALSE, TRUE);
    }
    
    if (numbaddir != 0 && YAG_CONTEXT->YAG_STAT_MODE) {
        fprintf(YAG_CONTEXT->YAGLE_STAT_FILE, "Connector orientation conflicts : %d \n", numbaddir);
    }
}
 
static int
orient_connector(locon_list *ptlocon, int merge, int isintcon)
{
    locon_list  *ptintcon;
    chain_list  *ptchain;
    cone_list   *ptcone;
    edge_list   *ptedge;
    ptype_list  *ptuser;
    long        direction;
    long        tristate_mask;
    char        type;

    direction = 0;

    /* power supply signals */
    if (ptlocon->DIRECTION == CNS_VDDC) return 0;
    if (ptlocon->DIRECTION == CNS_VSSC) return 0;

    ptuser = getptype(ptlocon->USER, CNS_EXT);
    if (ptuser != NULL) {
        ptcone = (cone_list *)ptuser->DATA;

        if (yagIsOutput(ptcone)) direction |= XCON_OUT;
        
        /* connector can be input if non-NULL CNS_CONE list */
        if (getptype(ptlocon->USER, CNS_CONE) != NULL) {
            direction |= XCON_IN;
        }

        /* connector can be input if YAG_INOUT_PTYPE is set */
        if (getptype(ptlocon->USER, YAG_INOUT_PTYPE) != NULL) {
            direction |= XCON_IN;
        }

        /* or if cone in OUTCONE list */
        for (ptedge = ptcone->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
            if ((ptedge->TYPE & CNS_CONE) == CNS_CONE
            && (ptedge->TYPE & CNS_BLEEDER) != CNS_BLEEDER
            && (ptedge->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK) break;
        }
        if (ptedge != NULL) direction |= XCON_IN;

        if (isintcon) tristate_mask = CNS_TRI|CNS_CONFLICT|CNS_LATCH;
        else tristate_mask = CNS_TRI|CNS_CONFLICT;
        
        if (direction == XCON_IN) type = 'I';
        else if (direction == XCON_OUT) {
            if ((ptcone->TYPE & tristate_mask) != 0) type = 'Z';
            else type = 'O';
        }
        else if (direction == XCON_INOUT) {
            if ((ptcone->TYPE & tristate_mask) != 0) type = 'T';
            else type = 'B';
        }
        else {
            type = 'X';
        }
    }
    else {
        /* connector can be input if non-NULL CNS_CONE list */
        if (getptype(ptlocon->USER, CNS_CONE) != NULL) {
            type = 'I';
        }
        else {
            type = 'X';
        }
    }
    if (merge) {
        ptchain = (chain_list *)getptype(ptlocon->SIG->USER, LOFIGCHAIN)->DATA;
        for (; ptchain; ptchain = ptchain->NEXT) {
            ptintcon = (locon_list *)ptchain->DATA;
            if (ptintcon->TYPE != INTERNAL) continue;
            type = yagMergeDirection(type, ptintcon->DIRECTION);
        }
    }

    if (ptlocon->DIRECTION == 'X' || ptlocon->DIRECTION == 'T') {
        ptlocon->DIRECTION = type;
    }
    else if (ptlocon->DIRECTION != type) {
        yagError(ERR_CON_DIR, ptlocon->NAME, NULL, NULL, 0, 0);
        ptlocon->DIRECTION = type;
        return 1;
    }
    return 0;
}

/*****************************************************************************
 *                          function yagReorientPort()                       *
 *****************************************************************************/

/* re-orient an external port based on cell interface */

void
yagReorientInterface(ptcnsfig, ptcellbefig)
    cnsfig_list *ptcnsfig;
    befig_list  *ptcellbefig;
{
    bepor_list *ptbepor;
    bepor_list *ptcircuitbepor = NULL;
    locon_list *ptlocon = NULL;

    for (ptbepor = ptcellbefig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT) {
        if ((ptcircuitbepor = yagBeporDefined(ptbepor->NAME)) != NULL) {
            for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
                if (ptcircuitbepor->NAME == ptlocon->NAME) break;
            }
            if (ptlocon != NULL) {
                switch (ptlocon->DIRECTION) {
                case UNKNOWN:
                    switch (ptbepor->DIRECTION) {
                    case 'I':
                        ptlocon->DIRECTION = IN;
                        ptcircuitbepor->DIRECTION = 'I';
                        ptcircuitbepor->TYPE = 'B';
                        yagAddBerinVect(ptcnsfig->BEFIG, ptlocon->NAME);
                        break;
                    case 'O':
                        ptlocon->DIRECTION = OUT;
                        ptcircuitbepor->DIRECTION = 'O';
                        ptcircuitbepor->TYPE = 'B';
                        break;
                    case 'Z':
                        ptlocon->DIRECTION = TRISTATE;
                        ptcircuitbepor->DIRECTION = 'Z';
                        ptcircuitbepor->TYPE = 'M';
                        break;
                    case 'B':
                        ptlocon->DIRECTION = INOUT;
                        ptcircuitbepor->DIRECTION = 'B';
                        ptcircuitbepor->TYPE = 'B';
                        yagAddBerinVect(ptcnsfig->BEFIG, ptlocon->NAME);
                        break;
                    case 'T':
                        ptlocon->DIRECTION = TRANSCV;
                        ptcircuitbepor->DIRECTION = 'T';
                        ptcircuitbepor->TYPE = 'M';
                        yagAddBerinVect(ptcnsfig->BEFIG, ptlocon->NAME);
                        break;
                    }
                    break;
                case IN:
                    switch (ptbepor->DIRECTION) {
                    case 'O':
                        ptlocon->DIRECTION = INOUT;
                        ptcircuitbepor->DIRECTION = 'B';
                        ptcircuitbepor->TYPE = 'B';
                        break;
                    case 'Z':
                        ptlocon->DIRECTION = TRANSCV;
                        ptcircuitbepor->DIRECTION = 'T';
                        ptcircuitbepor->TYPE = 'M';
                        break;
                    }
                    break;
                case OUT:
                    switch (ptbepor->DIRECTION) {
                    case 'I':
                        ptlocon->DIRECTION = INOUT;
                        ptcircuitbepor->DIRECTION = 'B';
                        ptcircuitbepor->TYPE = 'B';
                        yagAddBerinVect(ptcnsfig->BEFIG, ptlocon->NAME);
                        break;
                    case 'O':
                    case 'Z':
                    case 'B':
                    case 'T':
                        yagWarning(WAR_BEPOR_CONFLICT, NULL, ptbepor->NAME, NULL, 0);
                        break;
                    }
                    break;
                case INOUT:
                    switch (ptbepor->DIRECTION) {
                    case 'O':
                    case 'Z':
                    case 'B':
                    case 'T':
                        yagWarning(WAR_BEPOR_CONFLICT, NULL, ptbepor->NAME, NULL, 0);
                        break;
                    }
                    break;
                case TRISTATE:
                    switch (ptbepor->DIRECTION) {
                    case 'I':
                        ptlocon->DIRECTION = TRANSCV;
                        ptcircuitbepor->DIRECTION = 'T';
                        ptcircuitbepor->TYPE = 'M';
                        break;
                    case 'T':
                        ptlocon->DIRECTION = TRANSCV;
                        ptcircuitbepor->DIRECTION = 'T';
                        ptcircuitbepor->TYPE = 'M';
                        yagAddBerinVect(ptcnsfig->BEFIG, ptlocon->NAME);
                        break;
                    case 'O':
                    case 'B':
                        yagWarning(WAR_BEPOR_CONFLICT, NULL, ptbepor->NAME, NULL, 0);
                        break;
                    }
                    break;
                case TRANSCV:
                    switch (ptbepor->DIRECTION) {
                    case 'O':
                    case 'B':
                        yagWarning(WAR_BEPOR_CONFLICT, NULL, ptbepor->NAME, NULL, 0);
                        break;
                    }
                    break;
                }
            }
        }
    }
}

/*****************************************************************************
 *                          function yagSimplifyExpr()                       *
 *****************************************************************************/

/* simplify BEAUX and BEOUT expressions */

void
yagSimplifyExpr(ptbefig)
    befig_list *ptbefig;
{
    beaux_list *ptbeaux;
    beout_list *ptbeout;

    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT) {
        ptbeaux->ABL = yagReduceAbl(ptbeaux->ABL);
        ptbeaux->ABL = yagMorganReduce(ptbeaux->ABL);
    }
    for (ptbeout = ptbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        ptbeout->ABL = yagReduceAbl(ptbeout->ABL);
        ptbeout->ABL = yagMorganReduce(ptbeout->ABL);
    }
}

/*****************************************************************************
 *                          function yagSimplifyProcesses()                  *
 *****************************************************************************/

/* simplify BEBUX, BEBUS and BEREG expressions */

void
yagSimplifyProcesses(ptbefig)
    befig_list *ptbefig;
{
    bebux_list *ptbebux;
    bebus_list *ptbebus;
    bereg_list *ptbereg;
    biabl_list *ptbiabl;

    for (ptbebux = ptbefig->BEBUX; ptbebux; ptbebux = ptbebux->NEXT) {
        for (ptbiabl = ptbebux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptbiabl->CNDABL = yagReduceAbl(ptbiabl->CNDABL);
            ptbiabl->VALABL = yagReduceAbl(ptbiabl->VALABL);
        }
    }
    for (ptbebus = ptbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        for (ptbiabl = ptbebus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptbiabl->CNDABL = yagReduceAbl(ptbiabl->CNDABL);
            ptbiabl->VALABL = yagReduceAbl(ptbiabl->VALABL);
        }
    }
    for (ptbereg = ptbefig->BEREG; ptbereg; ptbereg = ptbereg->NEXT) {
        for (ptbiabl = ptbereg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            if (searchOperExpr(ptbiabl->CNDABL, STABLE) == 0) {
                ptbiabl->CNDABL = yagReduceAbl(ptbiabl->CNDABL);
            }
            ptbiabl->VALABL = yagReduceAbl(ptbiabl->VALABL);
        }
    }
}

/*****************************************************************************
 *                          function yagMorganReduce()                       *
 *****************************************************************************/

/* simplify two-level expressions by application of De Morgan's laws */

chain_list *
yagMorganReduce(headexpr)
    chain_list *headexpr;
{
    chain_list *expr;
    chain_list *tmpexpr;
    int         numcdr = 0;

    if (profExpr(headexpr) != 2) return headexpr;
    if (OPER(headexpr) != AND && OPER(headexpr) != OR) return headexpr;

    for (expr = CDR(headexpr); expr; expr = CDR(expr)) {
        if (++numcdr > 2) break;
        if (OPER(CAR(expr)) != NOT) break;
    }
    if (expr == NULL) {
        if (OPER(headexpr) == AND) CAR(headexpr)->DATA = (void *)NOR;
        else if (OPER(headexpr) == OR) CAR(headexpr)->DATA = (void *)NAND;
        for (expr = CDR(headexpr); expr; expr = CDR(expr)) {
            tmpexpr = CAR(expr);
            expr->DATA = copyExpr(CADR(tmpexpr));
            freeExpr(tmpexpr);
        }
    }
    return headexpr;
}

/*****************************************************************************
 *                          function yagSuppressBeaux()                      *
 *****************************************************************************/

/* suppress auxiliary signals declared in INF_SUPPRESS list */

befig_list *
yagSuppressBeaux(ptbefig)
    befig_list *ptbefig;
{
    beaux_list *ptbeaux;
    berin_list *ptberin;
    berin_list *ptinput;
    beout_list *ptoutref;
    chain_list *outrefchain = NULL;
    beaux_list *ptauxref;
    chain_list *auxrefchain = NULL;
    beaux_list *ptdlyref;
    chain_list *dlyrefchain = NULL;
    bebus_list *ptbusref;
    chain_list *busrefchain = NULL;
    bebux_list *ptbuxref;
    chain_list *buxrefchain = NULL;
    bereg_list *ptregref;
    chain_list *regrefchain = NULL;
    biabl_list *ptbiabl;
    beaux_list *ptnextbeaux;
    chain_list *ptchain;
    chain_list *ptinputchain;
    ht         *berinht;
    int         count = 0;

    if (ptbefig->ERRFLG != 0) return ptbefig;
    for (ptberin = ptbefig->BERIN; ptberin; ptberin = ptberin->NEXT) count++;
    if (count != 0) berinht = addht(count);
    for (ptberin = ptbefig->BERIN; ptberin; ptberin = ptberin->NEXT) {
        addhtitem(berinht, ptberin->NAME, (long)ptberin);
    }

    for (ptbeaux = ptbefig->BEAUX; ptbeaux; ptbeaux = ptnextbeaux) {
        ptnextbeaux = ptbeaux->NEXT;
        if (ptbeaux->NODE && getptype(((bequad_list *)ptbeaux->NODE)->USER, YAG_LOCK_PTYPE) != NULL) {
            continue;
        }
        ptberin = (berin_list *)gethtitem(berinht, ptbeaux->NAME);
        if (test_suppress(ptberin)) {
            for (ptchain = ptberin->OUT_REF; ptchain; ptchain = ptchain->NEXT) {
                ptoutref = (beout_list *)ptchain->DATA;
                ptoutref->ABL = substExpr(ptoutref->ABL, ptberin->NAME, ptbeaux->ABL);
                ptoutref->TIME = ptoutref->TIME + ptbeaux->TIME;
                if (yagGetChain(outrefchain, ptoutref) == NULL) {
                    outrefchain = addchain(outrefchain, ptoutref);
                }
            }
            for (ptchain = ptberin->AUX_REF; ptchain; ptchain = ptchain->NEXT) {
                ptauxref = (beaux_list *)ptchain->DATA;
                ptauxref->ABL = substExpr(ptauxref->ABL, ptberin->NAME, ptbeaux->ABL);
                ptauxref->TIME = ptauxref->TIME + ptbeaux->TIME;
                if (yagGetChain(auxrefchain, ptauxref) == NULL) {
                    auxrefchain = addchain(auxrefchain, ptauxref);
                }
            }
            for (ptchain = ptberin->DLY_REF; ptchain; ptchain = ptchain->NEXT) {
                ptdlyref = (beaux_list *)ptchain->DATA;
                ptdlyref->ABL = substExpr(ptdlyref->ABL, ptberin->NAME, ptbeaux->ABL);
                if (yagGetChain(dlyrefchain, ptdlyref) == NULL) {
                    dlyrefchain = addchain(dlyrefchain, ptdlyref);
                }
            }
            for (ptchain = ptberin->BUS_REF; ptchain; ptchain = ptchain->NEXT) {
                ptbusref = (bebus_list *)ptchain->DATA;
                for (ptbiabl = ptbusref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                    ptbiabl->CNDABL = substExpr(ptbiabl->CNDABL, ptberin->NAME, ptbeaux->ABL);
                    ptbiabl->VALABL = substExpr(ptbiabl->VALABL, ptberin->NAME, ptbeaux->ABL);
                }
                if (yagGetChain(busrefchain, ptbusref) == NULL) {
                    busrefchain = addchain(busrefchain, ptbusref);
                }
            }
            for (ptchain = ptberin->BUX_REF; ptchain; ptchain = ptchain->NEXT) {
                ptbuxref = (bebux_list *)ptchain->DATA;
                for (ptbiabl = ptbuxref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                    ptbiabl->CNDABL = substExpr(ptbiabl->CNDABL, ptberin->NAME, ptbeaux->ABL);
                    ptbiabl->VALABL = substExpr(ptbiabl->VALABL, ptberin->NAME, ptbeaux->ABL);
                }
                if (yagGetChain(buxrefchain, ptbuxref) == NULL) {
                    buxrefchain = addchain(buxrefchain, ptbuxref);
                }
            }
            for (ptchain = ptberin->REG_REF; ptchain; ptchain = ptchain->NEXT) {
                ptregref = (bereg_list *)ptchain->DATA;
                for (ptbiabl = ptregref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
                    ptbiabl->CNDABL = substExpr(ptbiabl->CNDABL, ptberin->NAME, ptbeaux->ABL);
                    ptbiabl->VALABL = substExpr(ptbiabl->VALABL, ptberin->NAME, ptbeaux->ABL);
                }
                if (yagGetChain(regrefchain, ptregref) == NULL) {
                    regrefchain = addchain(regrefchain, ptregref);
                }
            }
            ptinputchain = supportChain_listExpr(ptbeaux->ABL);
            for (ptchain = ptinputchain; ptchain; ptchain = ptchain->NEXT) {
                ptinput = (berin_list *)gethtitem(berinht, (char *)ptchain->DATA);
                if (ptinput != (berin_list *)EMPTYHT) {
                    ptinput->AUX_REF = yagRmvChain(ptinput->AUX_REF, ptbeaux);
                    ptinput->OUT_REF = yagUnionChainList(ptinput->OUT_REF, ptberin->OUT_REF);
                    ptinput->AUX_REF = yagUnionChainList(ptinput->AUX_REF, ptberin->AUX_REF);
                    ptinput->DLY_REF = yagUnionChainList(ptinput->DLY_REF, ptberin->DLY_REF);
                    ptinput->BUS_REF = yagUnionChainList(ptinput->BUS_REF, ptberin->BUS_REF);
                    ptinput->BUX_REF = yagUnionChainList(ptinput->BUX_REF, ptberin->BUX_REF);
                    ptinput->REG_REF = yagUnionChainList(ptinput->REG_REF, ptberin->REG_REF);
                }
            }
            freechain(ptinputchain);
            ptbefig->BEAUX = beh_delbeaux(ptbefig->BEAUX, ptbeaux, 'Y');
            if (auxrefchain != NULL) auxrefchain = yagRmvChain(auxrefchain, ptbeaux);
            ptbefig->BERIN = beh_delberin(ptbefig->BERIN, ptberin);
        }
    }

    for (ptchain = outrefchain; ptchain; ptchain = ptchain->NEXT) {
        ptoutref = (beout_list *)ptchain->DATA;
        ptoutref->ABL = yagReduceAbl(ptoutref->ABL);
    }
    for (ptchain = auxrefchain; ptchain; ptchain = ptchain->NEXT) {
        ptauxref = (beaux_list *)ptchain->DATA;
        ptauxref->ABL = yagReduceAbl(ptauxref->ABL);
    }
    for (ptchain = dlyrefchain; ptchain; ptchain = ptchain->NEXT) {
        ptdlyref = (beaux_list *)ptchain->DATA;
        ptdlyref->ABL = yagReduceAbl(ptdlyref->ABL);
    }
    for (ptchain = busrefchain; ptchain; ptchain = ptchain->NEXT) {
        ptbusref = (bebus_list *)ptchain->DATA;
        for (ptbiabl = ptbusref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptbiabl->CNDABL = yagReduceAbl(ptbiabl->CNDABL);
            ptbiabl->VALABL = yagReduceAbl(ptbiabl->VALABL);
        }
    }
    for (ptchain = buxrefchain; ptchain; ptchain = ptchain->NEXT) {
        ptbuxref = (bebux_list *)ptchain->DATA;
        for (ptbiabl = ptbuxref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptbiabl->CNDABL = yagReduceAbl(ptbiabl->CNDABL);
            ptbiabl->VALABL = yagReduceAbl(ptbiabl->VALABL);
        }
    }
    for (ptchain = regrefchain; ptchain; ptchain = ptchain->NEXT) {
        ptregref = (bereg_list *)ptchain->DATA;
        for (ptbiabl = ptregref->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            ptbiabl->CNDABL = yagReduceRegCndAbl(ptbiabl->CNDABL);
            ptbiabl->VALABL = yagReduceAbl(ptbiabl->VALABL);
        }
    }
    freechain(outrefchain);
    freechain(auxrefchain);
    freechain(dlyrefchain);
    freechain(busrefchain);
    freechain(buxrefchain);
    freechain(regrefchain);
    if (count!=0) delht(berinht);
    return ptbefig;
}

static chain_list *YAG_BASE_EXPR;

static int
yagOccCompare(var1, var2)
    char *var1;
    char *var2;
{
    return (numberOccExpr(YAG_BASE_EXPR, var1) - numberOccExpr(YAG_BASE_EXPR, var2));
}

pCircuit yagBuildCircuitFromExpression(chain_list *expr)
{
    chain_list *support;
    pCircuit    circuit;
    chain_list *ptchain;
    char      **nametab;
    int         numvars;
    int         i = 0;

    support = supportChain_listExpr(expr);
    numvars = yagCountChains(support);
    circuit = initializeCct("reduce", numvars, 10);

    nametab = (char **)mbkalloc(numvars * sizeof(char *));
    for (ptchain = support; support; support = support->NEXT) {
        nametab[i++] = (char *)ptchain->DATA;
    }
    YAG_BASE_EXPR = expr;
    qsort(nametab, numvars, sizeof(char *), yagOccCompare);
    for (i=0; i < numvars; i++) {
        addInputCct_no_NA(circuit, nametab[i]);
    }
    mbkfree(nametab);
    freechain(support);
    return circuit;
}

chain_list *
yagReduceAbl(expr)
    chain_list *expr;
{
    pCircuit    circuit;
    chain_list *newexpr;
    pNode       bdd;

    circuit = yagBuildCircuitFromExpression(expr);
    bdd = ablToBddCct(circuit, expr);
    newexpr = bddToAblCct(circuit, bdd);
    freeExpr(expr);
    destroyCct(circuit);
    yagControlBdd(0);

    return newexpr;
}

static int
test_suppress(ptberin)
    berin_list *ptberin;
{
    beaux_list *ptbeaux;
    beout_list *ptbeout;
    chain_list *suppress;
    chain_list *support;
    chain_list *ptchain;
    char       *suppressname;
    inffig_list *ifl;

    if (YAG_CONTEXT->YAG_TAS_TIMING == YAG_NO_TIMING) {
      if ((ifl=getloadedinffig(YAG_CONTEXT->YAG_FIGNAME))!=NULL)
        {
          chain_list *suppresslist;
          
          suppresslist=inf_GetEntriesByType(ifl, INF_SUPPRESS, INF_ANY_VALUES);

          for (suppress = suppresslist; suppress; suppress = suppress->NEXT) {
            suppressname = (char *)suppress->DATA;
            if (ptberin->NAME == suppressname) return TRUE;
            else if (strchr(suppressname, (int)'*') != NULL) {
              if (mbk_TestREGEX(ptberin->NAME, suppressname)) return TRUE;
            }
          }
          freechain(suppresslist);
        }
    }

    if (YAG_CONTEXT->YAG_MINIMISE_INV) {
        if (ptberin->BUX_REF == NULL && ptberin->BUS_REF == NULL && ptberin->REG_REF == NULL && ptberin->MSG_REF == NULL && ptberin->DLY_REF == NULL) {
            for (ptchain = ptberin->OUT_REF; ptchain; ptchain = ptchain->NEXT) {
                ptbeout = (beout_list *)ptchain->DATA;
                if (ptbeout->NODE && getptype(((bequad_list *)ptbeout->NODE)->USER, YAG_LOCK_PTYPE) != NULL) return FALSE;
                support = supportChain_listExpr(ptbeout->ABL);
                if (yagCountChains(support) != 1) {
                    freechain(support);
                    break;
                }
                freechain(support);
            }
            if (ptchain == NULL) {
                for (ptchain = ptberin->AUX_REF; ptchain; ptchain = ptchain->NEXT) {
                    ptbeaux = (beaux_list *)ptchain->DATA;
                    if (ptbeaux->NODE && getptype(((bequad_list *)ptbeaux->NODE)->USER, YAG_LOCK_PTYPE) != NULL) return FALSE;
                    support = supportChain_listExpr(ptbeaux->ABL);
                    if (yagCountChains(support) != 1) {
                        freechain(support);
                        break;
                    }
                    freechain(support);
                }
            }
            if (ptchain == NULL) return TRUE;
        }
    }
    return FALSE;
}

chain_list *
yagReduceRegCndAbl(expr)
    chain_list *expr;
{
    chain_list *expr1;
    chain_list *expr2;
    chain_list *trigger = NULL;
    chain_list *condition = NULL;
    chain_list *trigexpr;

    if (!ATOM(expr)) {
        if ((OPER(expr) == AND) || (lengthExpr(expr) == 2)) {
            expr1 = CADR(expr);
            expr2 = CADR(CDR(expr));
            if (!ATOM(expr2)) {
                if (OPER(expr2) == NOT) {
	                if (!ATOM(CADR(expr2))) {
	                    if (OPER(CADR(expr2)) == STABLE) {
	                        trigger = CADR(CADR(expr2));
	                        condition = expr1;
	                    }
	                }
	            }
	        }
	        if (trigger == NULL && !ATOM(expr1)) {
                if (OPER(expr1) == NOT) {
	                if (!ATOM(CADR(expr1))) {
	                    if (OPER(CADR(expr1)) == STABLE) {
	                        trigger = CADR(CADR(expr1));
	                        condition = expr2;
	                    }
	                }
	            }
	        }
	    }
	}

	if (trigger == NULL) expr = yagReduceAbl(expr);
	else {
	    trigger = yagReduceAbl(copyExpr(trigger));
	    condition = yagReduceAbl(copyExpr(condition));
	    freeExpr(expr);
	    expr = createExpr(AND);
	    addQExpr(expr, condition);
	    trigexpr = createExpr(STABLE);
	    addQExpr(trigexpr, trigger);
	    addQExpr(expr, notExpr(trigexpr));
	}
    return expr;
}

    
