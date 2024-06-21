/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_chain.c                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 26/05/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

#define BLOCKED     0
#define PATH_FOUND  1
#define BRLOOP      2

static ht           *TRANSHT;
static chain_list   *VISITED;
static ht           *VISITED_FAST;
static ptype_list   *INPUT_TYPE;
static int          search_depth;

static int find_inputs(inffig_list *ifl, cone_list *ptcone, locon_list *ptcon, edge_list **ptptinputs, ht **FASTSEARCH);

/****************************************************************************
 *                         function yagChainTrans();                        *
 ****************************************************************************/

            /* remplissage des champs GRID des transistors */

void
yagChainTrans(lotrs_list *pttrans)
{ 
    losig_list  *ptsig = NULL;
    ptype_list  *user = NULL;

    ptsig = pttrans->GRID->SIG;

    user = getptype(ptsig->USER, YAG_CONE_PTYPE);

    if (user != NULL && (((cone_list *)user->DATA)->TYPE & YAG_TEMPCONE) == 0) {
        pttrans->GRID = (locon_list *)user->DATA;
        pttrans->USER = addptype(pttrans->USER, CNS_DRIVINGCONE, user->DATA);
    }
    else {
        if (getptype(pttrans->USER, YAG_GRIDCON_PTYPE) == NULL) {
            pttrans->USER = addptype(pttrans->USER, YAG_GRIDCON_PTYPE, pttrans->GRID);
        }
        pttrans->GRID = NULL;
        pttrans->USER = addptype(pttrans->USER, CNS_DRIVINGCONE, NULL);
    }
}

/****************************************************************************
 *                         function yagChainDual();                         *
 ****************************************************************************/
         /*---------------------------------------------------*
          | fill INCONE fields of CMOS duals                  |
          *---------------------------------------------------*/

void
yagChainDual(cone_list *ptcone)
{
    branch_list *ptbranch = NULL;
    link_list   *ptlink = NULL;
    cone_list   *ptincone;
    edge_list   *ptedge, *ptinputs = NULL;
    lotrs_list  *ptlotrs;
    long        inputtype;

    for (ptbranch = ptcone->BRVDD; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT) continue;
            ptlotrs = ptlink->ULINK.LOTRS;
            if ((ptlink->TYPE & CNS_TPLINK) == CNS_TPLINK) {
                ptincone = (cone_list *)ptlotrs->GRID;
                if (ptincone == NULL) {
                    ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                    continue;
                }
                if (yagGetEdge(ptcone->INCONE, ptincone) == NULL) {
                    inputtype = ptincone->TYPE & (CNS_VDD|CNS_VSS);
                    if (inputtype == 0) inputtype = CNS_CONE;
                    addincone(ptcone, inputtype, ptincone);
                }
            }
            else {
                yagBug(DBG_ILL_LINKTYPE, "yagChainDual", ptcone->NAME, NULL, 0);
            }
        }
    }
    for (ptbranch = ptcone->BRVSS; ptbranch != NULL; ptbranch = ptbranch->NEXT) {
        for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {
            if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT) continue;
            ptlotrs = ptlink->ULINK.LOTRS;
            if ((ptlink->TYPE & CNS_TNLINK) == CNS_TNLINK) {
                ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
                if (ptincone == NULL) {
                    ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                    continue;
                }
                if (yagGetEdge(ptcone->INCONE, ptincone) == NULL) {
                    inputtype = ptincone->TYPE & (CNS_VDD|CNS_VSS);
                    if (inputtype == 0) inputtype = CNS_CONE;
                    addincone(ptcone, inputtype, ptincone);
                }
            }
            else {
                yagBug(DBG_ILL_LINKTYPE, "yagChainDual", ptcone->NAME, NULL, 0);
            }
        }
    }
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        ptinputs = addedge(ptinputs, ptedge->TYPE, ptedge->UEDGE.PTR);
    }
    ptcone->USER = addptype(ptcone->USER, YAG_DUALINPUTS_PTYPE, ptinputs);
}

/****************************************************************************
 *                         function yagChainBranch();                       *
 ****************************************************************************/
         /*---------------------------------------------------*
          | update INCONE fields                              |
          *---------------------------------------------------*/

void
yagChainBranch(cone_list *ptcone, branch_list *ptbranch)
{
    link_list   *ptlink;
    ptype_list  *ptuser;
    locon_list  *ptcon;
    cone_list   *ptincone;
    edge_list   *ptedge;
    lotrs_list  *ptlotrs;
    long        linktype, branchtype;
    long        inputtype;

    branchtype = ptbranch->TYPE & (CNS_VSS|CNS_VDD|CNS_EXT);

    for (ptlink = ptbranch->LINK; ptlink != NULL; ptlink = ptlink->NEXT) {

        if ((ptlink->TYPE & CNS_SHORT) == CNS_SHORT) continue;
        if ((ptlink->TYPE & CNS_DIODE_UP) == CNS_DIODE_UP) continue;
        if ((ptlink->TYPE & CNS_DIODE_DOWN) == CNS_DIODE_DOWN) continue;
        linktype = ptlink->TYPE & (CNS_TNLINK|CNS_TPLINK|CNS_IN|CNS_INOUT);
        if (linktype == 0) {
            yagBug(DBG_ILL_LINKTYPE, "yagChainBranch", ptcone->NAME, NULL, 0);
            continue;
        }

        if (linktype == CNS_TNLINK || linktype == CNS_TPLINK) {
            ptlotrs = ptlink->ULINK.LOTRS;
            ptincone = (cone_list *)ptlink->ULINK.LOTRS->GRID;
            if (ptincone == NULL) {
                ptbranch->TYPE |= CNS_NOT_FUNCTIONAL;
                continue;
            }
            inputtype = ptincone->TYPE & (CNS_VDD|CNS_VSS);
            if (inputtype == 0) inputtype = CNS_CONE;
            if ((ptlink->TYPE & CNS_RESIST) != CNS_RESIST) {
                if ((ptbranch->TYPE & CNS_BLEEDER) == CNS_BLEEDER && inputtype == CNS_CONE) {
                    inputtype |= CNS_BLEEDER|CNS_LOOP;
                }
            }
            if ((ptlink->TYPE & CNS_COMMAND) != 0) inputtype |= CNS_COMMAND;
            if ((ptedge = yagGetEdge(ptcone->INCONE, ptincone)) != NULL) {
                if ((ptedge->TYPE & YAG_FALSECONF) != 0 && (ptbranch->TYPE & YAG_FALSECONF) == 0) {
                    ptedge->TYPE &= ~YAG_FALSECONF;
                }
                if ((ptedge->TYPE & YAG_NOT_FUNCTIONAL) != 0 && (ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) {
                    ptedge->TYPE &= ~YAG_NOT_FUNCTIONAL;
                }
                ptedge->TYPE |= inputtype;
            }
            else {
                if ((ptbranch->TYPE & YAG_FALSECONF) != 0) inputtype |= YAG_FALSECONF;
                if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) inputtype |= YAG_NOT_FUNCTIONAL;
                addincone(ptcone, inputtype, ptincone);
            }
        }

        /*------------------------------+
        |  external connector link      |
        +------------------------------*/
        else {
            ptcon = ptlink->ULINK.LOCON;
            inputtype = CNS_EXT;
            if ((ptedge = yagGetEdge(ptcone->INCONE, ptcon)) != NULL) {
                if ((ptedge->TYPE & YAG_FALSECONF) != 0 && (ptbranch->TYPE & YAG_FALSECONF) == 0) {
                    ptedge->TYPE &= ~YAG_FALSECONF;
                }
                if ((ptedge->TYPE & YAG_NOT_FUNCTIONAL) != 0 && (ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) {
                    ptedge->TYPE &= ~YAG_NOT_FUNCTIONAL;
                }
            }
            else {
                if ((ptbranch->TYPE & YAG_FALSECONF) != 0) inputtype |= YAG_FALSECONF;
                if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0) inputtype |= YAG_NOT_FUNCTIONAL;
                addincone(ptcone, inputtype, ptcon);
            }
            
            /* Add CNS_CONE and CNS_LOCON ptypes to LOCON and CONE */
            ptuser = getptype(ptcon->USER, CNS_CONE);
            if (ptuser != NULL) {
                if (yagGetChain(ptuser->DATA, ptcone) == NULL) {
                    ptuser->DATA = addchain(ptuser->DATA, ptcone);
                }
            }
            else ptcon->USER = addptype(ptcon->USER, CNS_CONE, addchain(NULL, ptcone));
            ptuser = getptype(ptcone->USER, CNS_LOCON);
            if (ptuser != NULL) {
                if (yagGetChain(ptuser->DATA, ptcon) == NULL) {
                    ptuser->DATA = addchain(ptuser->DATA, ptcon);
                }
            }
            else ptcone->USER = addptype(ptcone->USER, CNS_LOCON, addchain(NULL, ptcon));
        }
    }
}

/****************************************************************************
 *                         function yagChainCone();                         *
 ****************************************************************************/
         /*---------------------------------------------------*
          | update INCONE fields after branch removal         |
          *---------------------------------------------------*/

void
yagChainCone(cone_list *ptcone)
{
    edge_list   *old_edge_list;
    edge_list   *ptedge;
    edge_list   *ptoldedge;
    locon_list  *ptcon;
    ptype_list  *ptuser;
    branch_list *ptbranch;
    branch_list *brlist[4];
    int         i;

    /* destroy connector references to cone */
    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if (ptedge->TYPE == CNS_EXT) {
            ptcon = ptedge->UEDGE.LOCON;
            ptuser = getptype(ptcon->USER, CNS_CONE);
            if (ptuser != NULL) {
                ptuser->DATA = yagRmvChain(ptuser->DATA, ptcone);
                if (ptuser->DATA == NULL) {
                    ptcon->USER = delptype(ptcon->USER, CNS_CONE);
                }
            }
        }
    }

    /* delete the INCONE list */
    old_edge_list = ptcone->INCONE;
    ptcone->INCONE = NULL;
    
    /* rebuild the INCONE list */
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;
    
    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & YAG_IGNORE) != 0) continue;
            yagChainBranch(ptcone, ptbranch);
        }
    }

    /* Recuperate edge TYPE information */
    if (old_edge_list != NULL) {
        for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
            ptoldedge = yagGetEdge(old_edge_list, ptedge->UEDGE.PTR);
            if (ptoldedge != NULL) ptedge->TYPE |= ptoldedge->TYPE & ~(CNS_COMMAND|YAG_FALSECONF);
        }
        yagFreeEdgeList(old_edge_list);
    }
}

/****************************************************************************
 *                         function yagBuildOutputs();                      *
 ****************************************************************************/
         /*---------------------------------------------------*
          | build OUTCONE fields                              |
          *---------------------------------------------------*/

void
yagBuildOutputs(cone_list *ptcone)
{
    edge_list   *ptinedge;
    cone_list   *ptincone;
    locon_list  *ptcon;
    chain_list  *ptconlist, *ptchain;
    long        outtype;

    for (ptinedge = ptcone->INCONE; ptinedge; ptinedge = ptinedge->NEXT) {
        if ((ptinedge->TYPE & (CNS_CONE|CNS_VDD|CNS_VSS)) != 0) {
            ptincone = ptinedge->UEDGE.CONE;
            outtype = CNS_CONE;
            if ((ptinedge->TYPE & CNS_LOOP) != 0) outtype |= CNS_LOOP;
            if ((ptinedge->TYPE & CNS_BLEEDER) != 0) outtype |= CNS_BLEEDER;
            if ((ptinedge->TYPE & CNS_FEEDBACK) != 0) outtype |= CNS_FEEDBACK;
            if ((ptinedge->TYPE & CNS_MEMSYM) != 0) outtype |= CNS_MEMSYM;
            if ((ptinedge->TYPE & CNS_IGNORE) != 0) outtype |= CNS_IGNORE;
            addoutcone(ptincone, outtype, ptcone);
        }
    }

    if (yagIsOutput(ptcone)) {
        ptconlist = (chain_list *)getptype(ptcone->USER, CNS_EXT)->DATA;
        for (ptchain = ptconlist; ptchain; ptchain = ptchain->NEXT) {
            ptcon = (locon_list *)ptchain->DATA;
            addoutcone(ptcone, CNS_EXT, ptcon);
        }
    }
}

/****************************************************************************
 *                         function yagGetTransList();                      *
 ****************************************************************************/

ht *
yagGetTransList(inffig_list *ifl, cone_list *ptcone, losig_list *ptsig)
{
    edge_list   *ptinputs = NULL;
    chain_list  *ptchain;
    chain_list  *locon_chain;
    locon_list  *ptcon;
    ht          *listht;
    ht *FASTSEARCH=NULL;
    
    listht = addht(50);
    TRANSHT = listht;

    VISITED = addchain(NULL, ptsig);
    VISITED_FAST = NULL;
    
    search_depth = 0;
    locon_chain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = locon_chain; ptchain; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T' && ptcon->NAME != CNS_GRIDNAME && ptcon->NAME != CNS_BULKNAME) {
            if ((((lotrs_list *)ptcon->ROOT)->TYPE & USED) == 0) {
                find_inputs(ifl, ptcone, ptcon, &ptinputs, &FASTSEARCH);
            }
        }
    }
    if (VISITED_FAST!=NULL) delht(VISITED_FAST);
    freechain(VISITED);
    if (ptinputs != NULL) yagFreeEdgeList(ptinputs);
    if (FASTSEARCH!=NULL) delht(FASTSEARCH);
    return listht;
}

/****************************************************************************
 *                         function yagGetConeInputs();                     *
 ****************************************************************************/
         /*---------------------------------------------------*
          | return up to date list of possible cone inputs    |
          *---------------------------------------------------*/

edge_list *
yagGetConeInputs(inffig_list *ifl, cone_list *ptcone)
{
    losig_list  *ptsig;
    edge_list   *ptinputs = NULL;
    chain_list  *ptchain;
    chain_list  *locon_chain;
    locon_list  *ptcon;
    edge_list   *ptedge;
    ht *FASTSEARCH=NULL;
    
    avt_log(LOGYAG,1, "Inputs for '%s'\n",ptcone->NAME);

    /* obtain inputs due to completed branches */

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        ptinputs = addedge(ptinputs, ptedge->TYPE, ptedge->UEDGE.PTR);
    }
    
    if ((ptcone->TYPE & YAG_PARTIAL) == 0) return ptinputs;
    
    TRANSHT = addht(50);

    /* inputs on trees starting on unused transistors */

    ptsig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, ptcone->INDEX);

    VISITED = addchain(NULL, ptsig);
    VISITED_FAST=NULL;
    
    INPUT_TYPE = NULL;
        
    search_depth = 0;
    locon_chain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = locon_chain; ptchain; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T' && ptcon->NAME != CNS_GRIDNAME && ptcon->NAME != CNS_BULKNAME) {
            if ((((lotrs_list *)ptcon->ROOT)->TYPE & USED) == 0) {
                find_inputs(ifl, ptcone, ptcon, &ptinputs, &FASTSEARCH);
                search_depth--;
            }
        }
    }
    if (VISITED_FAST!=NULL) delht(VISITED_FAST);
    freechain(VISITED);

    delht(TRANSHT);
    if (FASTSEARCH!=NULL) delht(FASTSEARCH);
    return ptinputs;
}

/****************************************************************************
 *                         function find_inputs();                          *
 ****************************************************************************/
         /*-------------------------------------*
          | return list of possible cone inputs |
          |     beyond a given connector        |
          *-------------------------------------*/

static int
find_inputs(inffig_list *ifl, cone_list *ptcone, locon_list *ptcon, edge_list **ptptinputs, ht **FASTSEARCH)
{
    losig_list  *ptinsig, *ptnextsig;
    chain_list  *ptnextconlist;
    chain_list  *locon_chain;
    chain_list  *ptchain;
    lotrs_list  *pttrans;
    locon_list  *ptnextcon;
    ptype_list  *ptuser;
    ptype_list  *ptintype;
    long        inputtype;
    long        insigtype;
    long        current_s, next_s;
    int         rescode;
    int         found, blocked, nonblocklatch, is_input;
    
    search_depth++;

    pttrans = (lotrs_list *)ptcon->ROOT;
    
    /* transistor already encountered */
    if ((rescode = gethtitem(TRANSHT, pttrans)) != EMPTYHT) return rescode;

    /* check for NEVER directive on transistor */
    if ((ptuser = getptype(pttrans->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & FCL_NEVER) != 0) return BLOCKED;
    }

    /* filter transistors without driver */
    if (pttrans->GRID == NULL) return BLOCKED;

    /* check that transistor is not blocked off by power supply */
    ptinsig = getlosig(YAG_CONTEXT->YAG_CURLOFIG, ((cone_list *)pttrans->GRID)->INDEX);
    if (ptinsig->TYPE == CNS_VDD && (pttrans->TYPE & CNS_TP) == CNS_TP) return BLOCKED;
    if (ptinsig->TYPE == CNS_VSS && (pttrans->TYPE & CNS_TN) == CNS_TN) return BLOCKED;

    /* refute auto-referential inputs */
    if (yagGetChain_QUICK(VISITED, ptinsig, &VISITED_FAST) != NULL) return BLOCKED;

    if (ptcon->NAME == CNS_SOURCENAME)
        ptnextsig = pttrans->DRAIN->SIG;
    else
        ptnextsig = pttrans->SOURCE->SIG;
        
    if ((next_s = yagGetVal_s(ptnextsig)) >= 0) {
        current_s = yagGetVal_s(ptcon->SIG);
        if (current_s < 0) return BLOCKED;
        if (current_s > next_s) return BLOCKED;
        if (current_s == next_s) yagWarning(WAR_SAME_S, ptcon->SIG->NAMECHAIN->DATA, ptnextsig->NAMECHAIN->DATA, NULL, 0);
    }

    if (YAG_CONTEXT->YAG_USE_CONNECTOR_DIRECTION==TRUE && !mbk_can_cross_transistor_to(ptcon->SIG, pttrans, 'i'))        {
//        printf("(3)blocked: %s from %s to %s\n", pttrans->TRNAME, getsigname(ptcon->SIG), getsigname(ptnextsig));
        return BLOCKED;
    }

    /* check for BLOCKER directive on next signal */
    nonblocklatch = FALSE;
    if ((ptuser = getptype(ptnextsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        if (((long)ptuser->DATA & FCL_BLOCKER) != 0) return BLOCKED;
        if (((long)ptuser->DATA & (FCL_LATCH|FCL_MEMSYM|FCL_MASTER|FCL_SLAVE)) != 0) nonblocklatch = TRUE;
    }

    /* check if next signal is a latch or a symmetric memory */
    if (!nonblocklatch && (ptuser = getptype(ptnextsig->USER, YAG_CONE_PTYPE)) != NULL) {
        if ((((cone_list *)ptuser->DATA)->TYPE & (CNS_LATCH|CNS_MEMSYM)) == CNS_LATCH) return BLOCKED;
        if (YAG_CONTEXT->YAG_MEMSYM_HEURISTIC && (((cone_list *)ptuser->DATA)->TYPE & CNS_MEMSYM) == CNS_MEMSYM) {
            if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM)) != 0) return BLOCKED;
        }
    }

    /* check for transistors of different types with same input */
    insigtype = ((pttrans->TYPE & CNS_TN) == CNS_TN ? CNS_TNLINK : CNS_TPLINK);
    if ((ptintype = yagGetPtype(INPUT_TYPE, ptinsig)) != NULL) {
        if (ptintype->TYPE != insigtype) return BLOCKED;
    }

    /* next signal is power supply */
    if (ptnextsig->TYPE == CNS_SIGVDD || ptnextsig->TYPE == CNS_SIGVSS) {
        if (yagGetEdge_QUICK(*ptptinputs, pttrans->GRID, FASTSEARCH) == NULL) {
            inputtype = CNS_CONE;
            if ((pttrans->TYPE & BLEEDER) != 0) {
                if (search_depth != 1) return BLOCKED;
                inputtype |= CNS_BLEEDER;
            }
            *ptptinputs = addedge(*ptptinputs, inputtype, pttrans->GRID);
            if (*FASTSEARCH!=NULL) addhtitem(*FASTSEARCH, pttrans->GRID, (long)*ptptinputs);
        }
        addhtitem(TRANSHT, pttrans, PATH_FOUND);
        return PATH_FOUND;
    }

    /* next signal is a forced input */
    if ((YAG_CONTEXT->YAG_FLAGS & YAG_HAS_INF_INPUTS)!=0 && ptnextsig->TYPE == CNS_SIGEXT) {
        ptnextconlist = yagGetExtLoconList(ptnextsig);
        is_input = FALSE;
        for (ptchain = ptnextconlist; ptchain; ptchain = ptchain->NEXT) {
            if (getptype(((locon_list *)ptchain->DATA)->USER, YAG_INPUT_PTYPE) != NULL) is_input = TRUE;
        }
        if (is_input) {
            *ptptinputs = addedge(*ptptinputs, CNS_CONE, pttrans->GRID);
            if (*FASTSEARCH!=NULL) addhtitem(*FASTSEARCH, pttrans->GRID, (long)*ptptinputs);
            for (ptchain = ptnextconlist; ptchain; ptchain = ptchain->NEXT) {
                *ptptinputs = addedge(*ptptinputs, CNS_EXT, (locon_list *)ptchain->DATA);
                if (*FASTSEARCH!=NULL) addhtitem(*FASTSEARCH, (locon_list *)ptchain->DATA, (long)*ptptinputs);
            }
            addhtitem(TRANSHT, pttrans, PATH_FOUND);
            freechain(ptnextconlist);
            return PATH_FOUND;
        }
        freechain(ptnextconlist);
    }

/*    if (search_depth == YAG_CONTEXT->YAG_MAX_LINKS) return BLOCKED;*/

    if (yagGetChain_QUICK(VISITED, ptnextsig, &VISITED_FAST) != NULL) return BRLOOP;
    VISITED = addchain(VISITED, ptnextsig);
    if (VISITED_FAST!=NULL) addhtitem(VISITED_FAST, ptnextsig, (long)VISITED);
    INPUT_TYPE = addptype(INPUT_TYPE, insigtype, ptinsig);
    
    found = FALSE;
    blocked = TRUE;
    locon_chain = (chain_list *)getptype(ptnextsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = locon_chain; ptchain; ptchain = ptchain->NEXT) {
        ptnextcon = (locon_list *)ptchain->DATA;
        if (ptnextcon->TYPE == 'T') {
            if (ptnextcon->NAME == CNS_GRIDNAME) continue;
            if (ptnextcon->NAME == CNS_BULKNAME) continue;
            
            rescode = find_inputs(ifl, ptcone, ptnextcon, ptptinputs, FASTSEARCH);
            search_depth--;
            if (rescode == PATH_FOUND) found = TRUE;
            if (rescode != BLOCKED) blocked = FALSE;
        }
        else if (ptnextcon->TYPE == 'E') {
            *ptptinputs = addedge(*ptptinputs, CNS_EXT, ptnextcon);
            if (*FASTSEARCH!=NULL) addhtitem(*FASTSEARCH, ptnextcon, (long)*ptptinputs);
            found = TRUE;
        }
    }

    if (VISITED_FAST!=NULL) delhtitem(VISITED_FAST, VISITED->DATA);
    VISITED = delchain(VISITED, VISITED);
    ptintype = INPUT_TYPE;
    INPUT_TYPE = INPUT_TYPE->NEXT;
    ptintype->NEXT = NULL;
    freeptype(ptintype);

    if (found) {
        if (yagGetEdge_QUICK(*ptptinputs, pttrans->GRID, FASTSEARCH) == NULL) {
            inputtype = CNS_CONE;
            if ((pttrans->TYPE & BLEEDER) != 0) inputtype |= CNS_BLEEDER;
            *ptptinputs = addedge(*ptptinputs, inputtype, pttrans->GRID);
            if (*FASTSEARCH!=NULL) addhtitem(*FASTSEARCH, pttrans->GRID, (long)*ptptinputs);
        }
        addhtitem(TRANSHT, pttrans, PATH_FOUND);
        return PATH_FOUND;
    }
    else if (blocked) {
        addhtitem(TRANSHT, pttrans, BLOCKED);
        return BLOCKED;
    }
    else return BRLOOP;
}

