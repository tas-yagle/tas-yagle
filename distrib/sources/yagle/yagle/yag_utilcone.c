/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_utilcone.c                                              */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 14/05/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

/****************************************************************************
 *                         function yagResetCone();                           *
 ****************************************************************************/

                  /* vide tous les champs d un cone */

void yagResetCone(cone_list *cone)
{
    edge_list       *in, *out;
    edge_list       *innext, *outnext;
    edge_list       *ptdel;
    locon_list      *ptcon;
    losig_list      *ptsig;
    lotrs_list      *pttrans;
    cell_list       *ptcell;
    ptype_list      *ptuser;
    chain_list      *ptchain;

    /*----------------------------------------*
    *  on detruit tous les chemins            *
    *-----------------------------------------*/
    while(cone->BRVDD != NULL)
        yagDestroyBranch(cone, cone->BRVDD); 
    while(cone->BRVSS != NULL)
        yagDestroyBranch(cone, cone->BRVSS); 
    while(cone->BRGND != NULL)
        yagDestroyBranch(cone, cone->BRGND); 
    while(cone->BREXT != NULL)
        yagDestroyBranch(cone, cone->BREXT); 
    
    /*----------------------------------------*
    *  on detruit tous INCONES                *
    *-----------------------------------------*/
    for (in = cone->INCONE; in != NULL; in = innext) {
        innext = in->NEXT;
        /* et les OUTCONES reciproque */
        if ((in->TYPE & CNS_CONE) == CNS_CONE) {
            ptdel = yagGetEdge(in->UEDGE.CONE->OUTCONE, cone);
            if (ptdel != NULL)
                in->UEDGE.CONE->OUTCONE = deledge(in->UEDGE.CONE->OUTCONE, ptdel);
        }
        else if ((in->TYPE & CNS_EXT) == CNS_EXT) {
            /* remove cone from connector CNS_CONE list */
            ptuser = getptype(in->UEDGE.LOCON->USER, CNS_CONE);
            if (ptuser != NULL) {
                ptuser->DATA = yagRmvChain((chain_list *)ptuser->DATA, cone);
                if (ptuser->DATA == NULL) {
                    in->UEDGE.LOCON->USER = delptype(in->UEDGE.LOCON->USER, CNS_CONE);
                }
            }
        }
        freeedge(in);
    }
    cone->INCONE = NULL;
    
    /*-----------------------------------------*
    *  on detruit tous OUTCONES               *
    *-----------------------------------------*/
    for (out = cone->OUTCONE; out != NULL; out = outnext) {
        outnext = out->NEXT;
        /* et les INCONES reciproque */
        if ((out->TYPE & CNS_CONE) == CNS_CONE) {
            ptdel = yagGetEdge(out->UEDGE.CONE->INCONE, cone);
            if (ptdel != NULL)
                out->UEDGE.CONE->INCONE = deledge(out->UEDGE.CONE->INCONE, ptdel);
        }
        freeedge(out);
    }
    cone->OUTCONE=NULL;
    
    ptuser = getptype(cone->USER, CNS_UPEXPR);
    if (ptuser != NULL) {
        freeExpr((chain_list *)ptuser->DATA);
    }

    ptuser = getptype(cone->USER, CNS_DNEXPR);
    if (ptuser != NULL) {
        freeExpr((chain_list *)ptuser->DATA);
    }
    
    ptuser = getptype(cone->USER, YAG_DUALEXPR_PTYPE);
    if (ptuser != NULL) {
        freeExpr((chain_list *)ptuser->DATA);
    }

    ptuser = getptype(cone->USER, CNS_EXT);
    if (ptuser != NULL) {
        for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
            ptcon = (locon_list *)ptchain->DATA;
            ptcon->USER = delptype(ptcon->USER, CNS_EXT);
        }
        freechain((chain_list *)ptuser->DATA);
        cone->USER = delptype(cone->USER, CNS_EXT);
    }
    
    ptuser = getptype(cone->USER, CNS_SWITCH); 
    if ((ptuser != NULL) && (ptuser->DATA != NULL)) {
        for (ptchain = (chain_list*)ptuser->DATA; ptchain != NULL; ptchain = ptchain->NEXT) {
            freechain((chain_list *)ptchain->DATA);
        }
        freechain(ptuser->DATA);
    }
    
    ptuser = getptype(cone->USER, CNS_PARALLEL); 
    if ((ptuser != NULL) && (ptuser->DATA != NULL)) {
        for (ptchain = (chain_list*)ptuser->DATA; ptchain; ptchain=ptchain->NEXT) {
            freechain((chain_list *)ptchain->DATA);
        }
        freechain(ptuser->DATA);
    }
    
    ptuser = getptype(cone->USER, CNS_BLEEDER); 
    if ((ptuser != NULL) && (ptuser->DATA != NULL)) {
        freechain((chain_list *)ptuser->DATA);
    }

    ptuser = getptype(cone->USER, CNS_PARATRANS); 
    if ((ptuser != NULL) && (ptuser->DATA != NULL)) {
        freechain((chain_list *)ptuser->DATA);
    }

    ptuser = getptype(cone->USER, CNS_LOCON); 
    if (ptuser != NULL) freechain((chain_list *)ptuser->DATA);

    ptuser = getptype(cone->USER, CNS_EXT); 
    if (ptuser != NULL) freechain((chain_list *)ptuser->DATA);

    /* remove transistor references to cone */

    ptsig = (losig_list *)getptype(cone->USER, CNS_SIGNAL)->DATA;
    ptchain = (chain_list *)getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (; ptchain != NULL; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *)ptchain->DATA;
        if (ptcon->TYPE == 'T' && ptcon->NAME == CNS_GRIDNAME) {
            pttrans = (lotrs_list *)ptcon->ROOT;
            pttrans->GRID = NULL;
            pttrans->USER = addptype(pttrans->USER, YAG_GRIDCON_PTYPE, ptcon);
            ptuser = getptype(pttrans->USER, CNS_DRIVINGCONE);
            ptuser->DATA = NULL;
        }
    }

    /* remove signal references to cone */
    
    ptsig->USER = delptype(ptsig->USER, YAG_CONE_PTYPE);

    /* remove cell references to cone */

    for (ptchain = cone->CELLS; ptchain; ptchain = ptchain->NEXT) {
        ptcell = (cell_list *)ptchain->DATA;
        ptcell->CONES = yagRmvChain(ptcell->CONES, cone);
    }

    freeptype(cone->USER);
    cone->USER = NULL;
    cone->TYPE |= YAG_DELETED;

}

/****************************************************************************
 *                         function yagCleanCone();                         *
 ****************************************************************************/

void yagCleanCone(cone_list *ptcone)
{
    branch_list *brlist[4];
    branch_list *ptbranch;
    ptype_list *pt;
    int          i;

    ptcone->TYPE &= 0x0000ffff;
    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BRVSS;
    brlist[2] = ptcone->BREXT;
    brlist[3] = ptcone->BRGND;

    for (i=0; i<4; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            ptbranch->TYPE &= 0x000fffff;
        }
    }

    if ((pt=getptype(ptcone->USER, YAG_DUALEXPR_PTYPE))!=NULL) {
       freeExpr((chain_list *)pt->DATA);
       ptcone->USER=delptype(ptcone->USER, YAG_DUALEXPR_PTYPE);
    }

    if ((pt=getptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE))!=NULL)
    {
       beh_frebefig((befig_list *)pt->DATA);
       ptcone->USER=delptype(ptcone->USER, YAG_LATCHBEFIG_PTYPE);
    }
    
    if ((pt=getptype(ptcone->USER, YAG_BUSBEFIG_PTYPE))!=NULL)
    {
       beh_frebefig((befig_list *)pt->DATA);
       ptcone->USER=delptype(ptcone->USER, YAG_BUSBEFIG_PTYPE);
    }
    if ((pt=getptype(ptcone->USER, CNS_INVCONE))!=NULL)
    {
       freechain((chain_list *)pt->DATA);
       ptcone->USER=delptype(ptcone->USER, CNS_INVCONE);
    }
    
}

/****************************************************************************
 *                         function yagMakeCone();                          *
 ****************************************************************************/

cone_list *yagMakeCone(cone_list *headcone, losig_list *ptsig, branch_group *ptbranches, long type, long tectype)
{
    locon_list      *ptcon;
    cone_list       *ptcone;
    branch_list     *ptbrext, *ptbrvdd, *ptbrvss, *ptbrgnd;
    cell_list       *ptcell;
    chain_list      *ptchain;
    ptype_list      *ptuser;
    char            *name;
    long            index;
    long            fcltype;

    index = (long)ptsig->INDEX;

    if ((type & CNS_EXT) == CNS_EXT) {
        ptcon = yagGetExtLocon(ptsig);
        name = ptcon->NAME;
    }
    else name = (char *)(ptsig->NAMECHAIN->DATA);

    if (ptbranches != NULL) {
        ptbrext = ptbranches->BREXT;
        ptbrvdd = ptbranches->BRVDD;
        ptbrvss = ptbranches->BRVSS;
        ptbrgnd = ptbranches->BRGND;
    }
    else {
        ptbrext = NULL;
        ptbrvdd = NULL;
        ptbrvss = NULL;
        ptbrgnd = NULL;
    }

    ptcone = addcone(headcone, index, name, type, tectype, 0, 0, 0, 0, NULL, NULL, ptbrext, ptbrvdd, ptbrgnd, ptbrvss, NULL);

    ptsig->USER = addptype(ptsig->USER, YAG_CONE_PTYPE, (void *)ptcone);
    ptcone->USER = addptype(ptcone->USER, CNS_SIGNAL, (void *)ptsig);
    
    /* transfer nettype of findcell from signal to cone */
    if ((ptuser = getptype(ptsig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
        fcltype = (long)ptuser->DATA;
        if ((fcltype & FCL_FLIPFLOP) != 0)
            ptcone->TYPE |= CNS_FLIP_FLOP;
        if ((fcltype & FCL_MASTER) != 0)
            ptcone->TYPE |= CNS_MASTER;
        if ((fcltype & FCL_SLAVE) != 0)
            ptcone->TYPE |= CNS_SLAVE;
        if ((fcltype & FCL_LATCH) != 0)
            ptcone->TYPE |= CNS_LATCH;
        if ((fcltype & FCL_MEMSYM) != 0)
            ptcone->TYPE |= CNS_MEMSYM;
        if ((fcltype & FCL_RS) != 0)
            ptcone->TYPE |= CNS_RS;
        if ((fcltype & FCL_STOP) != 0)
            ptcone->TYPE |= YAG_STOP;
        if ((fcltype & FCL_CELLOUT) != 0)
            ptcone->TYPE |= YAG_CELLOUT;
        if ((fcltype & FCL_SENSITIVE) != 0)
            ptcone->TECTYPE |= YAG_SENSITIVE;
        if ((fcltype & FCL_NOTLATCH) != 0)
            ptcone->TECTYPE |= YAG_NOTLATCH;
    }

    if ((ptcone->TYPE & (YAG_TEMPCONE|YAG_GLUECONE)) == 0) {
        if ((ptuser = getptype(ptsig->USER, FCL_CELL_PTYPE)) != NULL) {
            ptcone->CELLS = (chain_list *)ptuser->DATA;
            for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptcell = (cell_list *)ptchain->DATA;
                ptcell->CONES = addchain(ptcell->CONES, ptcone);
            }
        }
    }

    return (ptcone);
}

/****************************************************************************
 *                         function yagMakeSupplyCone();                    *
 ****************************************************************************/

cone_list *yagMakeSupplyCone(cone_list *headcone, losig_list *ptsig)
{
    cone_list       *ptcone;
    chain_list      *expr;
    long            type;

    type = (ptsig->TYPE == CNS_SIGVDD) ? CNS_VDD : CNS_VSS;
    ptcone = yagMakeCone(headcone, ptsig, NULL, type, 0);
    
    if (ptsig->TYPE == CNS_SIGVDD) {
        expr = createAtom("'1'");
        ptcone->NAME = namealloc("yag_one");
        ptcone->TECTYPE |= CNS_ONE;
        ptcone->TYPE |= YAG_FORCEPRIM;
    }
    else {
        expr = createAtom("'0'");
        ptcone->NAME = namealloc("yag_zero");
        ptcone->TECTYPE |= CNS_ZERO;
        ptcone->TYPE |= YAG_FORCEPRIM;
    }

    ptcone->USER = addptype(ptcone->USER, CNS_UPEXPR, (void *)expr);

    return ptcone;
}

/****************************************************************************
 *                         function yagMakeInCone();                          *
 ****************************************************************************/

cone_list *yagMakeInCone(cone_list *headcone, losig_list *ptsig, long type, long tectype)
{
    cone_list       *ptcone;
    locon_list      *ptlocon;
    chain_list      *ptloconlist, *ptchain;
    link_list       *ptlink = NULL;

    ptcone = yagMakeCone(headcone, ptsig, NULL, type|CNS_EXT, tectype);
    
    ptloconlist = yagGetExtLoconList(ptsig);

    ptcone->USER = addptype(ptcone->USER, CNS_EXT, ptloconlist);
    ptcone->USER = addptype(ptcone->USER, CNS_LOCON, dupchainlst(ptloconlist));
    for (ptchain = ptloconlist; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        ptlocon->USER = addptype(ptlocon->USER, CNS_EXT, ptcone);
        ptlocon->USER = addptype(ptlocon->USER, CNS_CONE, addchain(NULL, ptcone));
        ptlink = addlink(NULL, CNS_IN, ptlocon, ptsig);
        ptcone->BREXT = addbranch(ptcone->BREXT, CNS_EXT, ptlink);
        ptcone->INCONE = addedge(ptcone->INCONE, CNS_EXT, ptlocon);
    }
    return ptcone;
}

/****************************************************************************
 *                         function yagMakeOutCone();                         *
 ****************************************************************************/

cone_list *yagMakeOutCone(cone_list *headcone, losig_list *ptsig, branch_group *ptbranches, long type, long tectype)
{
    cone_list       *ptcone;
    locon_list      *ptlocon;
    chain_list      *ptloconlist, *ptchain;

    ptcone = yagMakeCone(headcone, ptsig, ptbranches, type|CNS_EXT, tectype);
    
    ptloconlist = yagGetExtLoconList(ptsig);

    ptcone->USER = addptype(ptcone->USER, CNS_EXT, ptloconlist);
    for (ptchain = ptloconlist; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        ptlocon->USER = addptype(ptlocon->USER, CNS_EXT, ptcone);
    }
    return ptcone;
}

/****************************************************************************
 *                         function yagMakeInoutCone();                       *
 ****************************************************************************/

cone_list *yagMakeInoutCone(cone_list *headcone, losig_list *ptsig, branch_group *ptbranches, long type, long tectype)
{
    cone_list       *ptcone;
    locon_list      *ptlocon;
    chain_list      *ptloconlist, *ptchain;
    link_list       *ptlink = NULL;

    ptcone = yagMakeCone(headcone, ptsig, ptbranches, type|CNS_EXT, tectype);
    
    ptloconlist = yagGetExtLoconList(ptsig);

    ptcone->USER = addptype(ptcone->USER, CNS_EXT, ptloconlist);
    ptcone->USER = addptype(ptcone->USER, CNS_LOCON, dupchainlst(ptloconlist));
    for (ptchain = ptloconlist; ptchain; ptchain = ptchain->NEXT) {
        ptlocon = (locon_list *)ptchain->DATA;
        ptlocon->USER = addptype(ptlocon->USER, CNS_EXT, ptcone);
        ptlocon->USER = addptype(ptlocon->USER, CNS_CONE, addchain(NULL, ptcone));
        ptlink = addlink(NULL, CNS_INOUT, ptlocon, ptsig);
        ptcone->BREXT = addbranch(ptcone->BREXT, CNS_EXT, ptlink);
        ptcone->INCONE = addedge(ptcone->INCONE, CNS_EXT, ptlocon);
    }
    return ptcone;
}

/****************************************************************************
 *                         function yagCheckBranches();                       *
 ****************************************************************************/
                /* check if cone has branches */

int yagCheckBranches(cone_list *ptcone)
{
    branch_list *ptbrext;

    if (ptcone->BRVDD == NULL && ptcone->BRVSS == NULL && ptcone->BRGND == NULL) {
        for (ptbrext = ptcone->BREXT; ptbrext; ptbrext = ptbrext->NEXT) {
            if ((ptbrext->TYPE & YAG_MARKFALSE) != YAG_MARKFALSE) break;
        }
        if (ptbrext == NULL) {
            if ((ptcone->TYPE & CNS_VDD) == 0 && (ptcone->TYPE & CNS_VSS) == 0) {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/****************************************************************************
 *                         function yagCheckExtOut();                         *
 ****************************************************************************/
                /* check if external cone can drive output */
                /* called as part of the global analysis   */

int yagCheckExtOut(cone_list *ptcone)
{
    link_list   *ptextlink = NULL;
    branch_list *ptbrext;
    branch_list *ptbranch;
    chain_list  *ptextlinkchain = NULL;
    chain_list  *ptchain;
    int         numbrext = 0;
    int         numvddfunc = 0;
    int         numvssfunc = 0;

    if ((ptcone->TYPE & CNS_EXT) != 0) {
        /* obtain external connector link */
        for (ptbrext = ptcone->BREXT; ptbrext; ptbrext = ptbrext->NEXT) {
            if ((ptbrext->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL) continue;
            numbrext++;
            if (ptbrext->LINK->NEXT == NULL) {
                ptextlinkchain = addchain(ptextlinkchain, ptbrext->LINK);
            }
        }
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) numvddfunc++;
        }
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) numvssfunc++;
        }
        if (numvddfunc == 0 && numvssfunc == 0
        && numbrext == yagCountChains(ptextlinkchain)) {
            for (ptchain = ptextlinkchain; ptchain; ptchain = ptchain->NEXT) {
                ptextlink = (link_list *)ptchain->DATA;
                ptextlink->TYPE &= ~CNS_INOUT;
                ptextlink->TYPE |= CNS_IN;
            }
            freechain(ptextlinkchain);
            ptcone->TYPE &= 0xffff0000;
            ptcone->TYPE |= CNS_EXT;
            ptcone->TECTYPE |= CNS_CMOS;
            return FALSE;
        }
    }
    freechain(ptextlinkchain);
    return TRUE;
}

/****************************************************************************
 *                         function yagCheckExtIn();                          *
 ****************************************************************************/
                /* check if input cone can drive anything */
                /* called after building of OUTCONE list  */

void yagCheckExtIn(cone_list *ptcone)
{
    branch_list *ptbrext = NULL, *ptnextbrext;
    locon_list  *ptcon;
    chain_list  *ptconlist;
    edge_list   *ptedge;
    ptype_list  *ptuser;

    if ((ptcone->TYPE & CNS_EXT) != 0) {
        ptconlist = (chain_list *)getptype(ptcone->USER, CNS_EXT)->DATA;
        for (ptedge = ptcone->OUTCONE; ptedge; ptedge = ptedge->NEXT) {
            if (yagGetChain(ptconlist, ptedge->UEDGE.PTR) == NULL
            && (ptedge->TYPE & CNS_BLEEDER) != CNS_BLEEDER
            && (ptedge->TYPE & CNS_FEEDBACK) != CNS_FEEDBACK) break;
        }
        if (ptedge == NULL) {
        /* nothing driven except maybe connector */
            for (ptbrext = ptcone->BREXT; ptbrext; ptbrext = ptnextbrext) {
                ptnextbrext = ptbrext->NEXT;
                if (ptbrext->LINK->NEXT == NULL) {
                    ptcon = ptbrext->LINK->ULINK.LOCON;
                    /* remove cone from connector CNS_CONE list */
                    ptuser = getptype(ptcon->USER, CNS_CONE);
                    if (ptuser != NULL) {
                        ptuser->DATA = yagRmvChain((chain_list *)ptuser->DATA, ptcone);
                        if (ptuser->DATA == NULL) {
                            ptcon->USER = delptype(ptcon->USER, CNS_CONE);
                        }
                    }
                    /* we remove the branch */
                    yagDestroyBranch(ptcone, ptbrext);
                }
            }
            
            if (yagCheckBranches(ptcone) != FALSE) {
                yagChainCone(ptcone);
                yagAnalyseCone(ptcone);
                ptcone->TYPE &= ~YAG_MARK;
            }
        }
    }
}

int yagIsOutput(cone_list *ptcone)
{
    branch_list    *ptbranch;
    branch_list    *ptbrext;
    int         numvddfunc = 0;
    int         numvssfunc = 0;
    int         numextfunc = 0;

    if ((ptcone->TYPE & CNS_EXT) != 0) {
        for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) numvddfunc++;
        }
        for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) numvssfunc++;
        }
        for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) == 0) {
                if (ptbranch->LINK->NEXT != NULL) numextfunc++;
                ptbrext = ptbranch;
            }
        }
        if (numvddfunc == 0 && numvssfunc == 0) {
            if (numextfunc != 0) return TRUE;
            else return FALSE;
        }
        else return TRUE;
    }
    return FALSE;
}

void yagRmvFalseBrext(cone_list *ptcone)
{
    branch_list *ptbrext, *ptnextbrext;
    cone_list   *ptextcone;
    locon_list  *ptextlocon;
    link_list   *ptlink;
    ptype_list  *ptuser;
    int         changed = FALSE;

    if ((ptcone->TYPE & CNS_EXT) == CNS_EXT) {
        if (yagIsOutput(ptcone) == FALSE) return;
    }

    for (ptbrext = ptcone->BREXT; ptbrext; ptbrext = ptnextbrext) {
        ptnextbrext = ptbrext->NEXT;
        ptlink = ptbrext->LINK;
        if (ptlink->NEXT == NULL) continue;
        for (; ptlink->NEXT != NULL; ptlink = ptlink->NEXT);
        ptextlocon = ptlink->ULINK.LOCON;
        ptuser = getptype(ptextlocon->SIG->USER, YAG_CONE_PTYPE);
        if (ptuser != NULL) {
            ptextcone = (cone_list *)ptuser->DATA;
            if ((ptextcone->TYPE & (CNS_TRI|CNS_CONFLICT)) == 0 && yagIsOutput(ptextcone)) {
                yagDestroyBranch(ptcone, ptbrext);
                changed = TRUE;
                ptextlocon->USER = addptype(ptextlocon->USER, YAG_INOUT_PTYPE, 0);
            }
        }
    }
    if (yagCheckBranches(ptcone) == FALSE) {
        yagResetCone(ptcone);
        CNS_HEADCNSFIG->CONE = delcone(CNS_HEADCNSFIG->CONE, ptcone);
    }
    else if (changed) {
        cnsConeFunction(ptcone, FALSE);
        yagChainCone(ptcone);
        yagAnalyseCone(ptcone);
        ptcone->TYPE &= ~YAG_MARK;
    }
}

float yagGetConeCapa(cone_list *ptcone, lofig_list *ptlofig)
{
    branch_list    *ptbranch;
    losig_list     *ptsig;

    if ((ptcone->TYPE & CNS_POWER) == CNS_POWER) {
        ptsig = (losig_list *)getptype(ptcone->USER, CNS_SIGNAL)->DATA;
        if (ptsig->PRCN != NULL) return(rcn_getcapa(ptlofig, ptsig));
        else return(0.0);
    }
    else {
        ptbranch = ptcone->BRVDD;
        if (ptbranch == NULL) ptbranch = ptcone->BRVSS;
        if (ptbranch == NULL) ptbranch = ptcone->BREXT;
        if (ptbranch->LINK->SIG->PRCN != NULL) return(rcn_getcapa(ptlofig, ptbranch->LINK->SIG));
        else return(0.0);
    }
}

int yagCheckUniqueConnection(cone_list *ptincone, cone_list *ptcone)
{
    losig_list *ptlosig;
    locon_list *ptcon;
    lotrs_list *pttrans;
    ptype_list *ptuser;
    chain_list *transchain;
    chain_list *loconchain;
    chain_list *ptchain;

    if (ptincone->OUTCONE != NULL && ptincone->OUTCONE->NEXT == NULL && ptincone->OUTCONE->UEDGE.CONE == ptcone) {
        if ((ptuser = getptype(ptincone->USER, CNS_LOTRS)) == NULL) {
            addconelotrs(ptincone);
            ptuser = getptype(ptincone->USER, CNS_LOTRS);
        }
        if (ptuser) {
            transchain = (chain_list *)ptuser->DATA;
            ptlosig = (losig_list *)getptype(ptincone->USER, CNS_SIGNAL)->DATA;
            loconchain = (chain_list *)getptype(ptlosig->USER, LOFIGCHAIN)->DATA;
            for (ptchain = loconchain; ptchain; ptchain = ptchain->NEXT) {
                ptcon = (locon_list *)ptchain->DATA;
                if (ptcon->TYPE == 'T') {
                    if (ptcon->NAME == CNS_GRIDNAME) continue;
                    if (ptcon->NAME == CNS_BULKNAME) continue;
                    pttrans = (lotrs_list *)ptcon->ROOT;
                    if (yagGetChain(transchain, pttrans) == NULL) break;
                }
            }
            if (ptchain == NULL) return TRUE;
        }
    }
    return FALSE;
}
