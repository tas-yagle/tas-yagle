/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : ../../....     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

static void     traverse_depth __P((cone_list *ptcone));
static char    *test_rename __P((inffig_list *ifl,char *name));
static char    *joker_name __P((char *name, char *oldname, char *newname));
static char    *joker_subst __P((char *name, char *subMotif, char *subName));
static char    *sub_elt __P((char *sub, char **addSub, char jok));
static char    *motif_in_name __P((char *nom, char *motif));

/*****************************************************************************
 *                          function yagExit()                               *
 *****************************************************************************/

void
yagExit(code)
    int code;
{
    EXIT(code);
}

void
yagTrapKill(sig)
    int sig;
{
#ifndef __ALL__WARNING__
    sig = 0;
#endif
    yagExit(100);
}

void
yagTrapCore(sig)
    int sig;
{
#ifndef __ALL__WARNING__
    sig = 0;
#endif
    fprintf(stderr, "\n[YAGLE] A fatal internal error has occured\n");
    fprintf(stderr, "        rerun with -nb option and consult report\n");
    fflush(stderr);
    yagExit(101);
}

/*****************************************************************************
 *                          function yagPrintTime()                          *
 *****************************************************************************/

/* display system and user time */

void
yagPrintTime(start, end, rstart, rend)
    struct rusage  *start, *end;
    time_t          rstart, rend;

{
    unsigned long   temps;
    unsigned long   user, syst;
    unsigned long   userM, userS, userD;
    unsigned long   systM, systS, systD;
    static int      pagesize;
    unsigned long   bytes;

    pagesize = getpagesize() / 1024;
    temps = rend - rstart;
    user = (100 * end->ru_utime.tv_sec + (end->ru_utime.tv_usec / 10000))
        - (100 * start->ru_utime.tv_sec + (start->ru_utime.tv_usec / 10000));
    syst = (100 * end->ru_stime.tv_sec + (end->ru_stime.tv_usec / 10000))
        - (100 * start->ru_stime.tv_sec + (start->ru_stime.tv_usec / 10000));

    userM = user / 6000;
    userS = (user % 6000) / 100;
    userD = (user % 100) / 10;

    systM = syst / 6000;
    systS = (syst % 6000) / 100;
    systD = (syst % 100) / 10;

    avt_log(LOGYAG, 0, " %02ldm%02lds", (long) (temps / 60), (long) (temps % 60));
    avt_log(LOGYAG, 0, "  u:%02ldm%02ld.%ld", userM, userS, userD);
    bytes = mbkprocessmemoryusage();
    avt_log(LOGYAG, 0, "  M:%ldKb\n", bytes / 1024);
}

void
yagChrono(t, rt)
    struct rusage  *t;
    time_t         *rt;
{
    getrusage(RUSAGE_SELF, t);
    time(rt);
}

/***************************************************************************8*
 *                       function yagDepthFirstProcess()                     *
 *****************************************************************************/

static int       (*processConeFunc)();
static int         coneChanges;

static long        countDepth;

int
yagDepthFirstProcess(headcone, processCone)
    cone_list      *headcone;
    int           (*processCone)();
{
    cone_list      *ptcone;
    cone_list      *ptnextcone;

    coneChanges = FALSE;
    processConeFunc = processCone;

    for (ptcone = headcone; ptcone != NULL; ptcone = ptnextcone) {
        ptnextcone = ptcone->NEXT;

        countDepth = 0;
        traverse_depth(ptcone);
    }

    for (ptcone = headcone; ptcone != NULL; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_CONESEEN;
    }
    return coneChanges;
}

static void
traverse_depth(ptcone)
    cone_list      *ptcone;
{
    edge_list      *ptedge;

    if ((ptcone->TYPE & CNS_LATCH) == CNS_LATCH && countDepth != 0) return;
    if ((ptcone->TYPE & CNS_FLIP_FLOP) == CNS_FLIP_FLOP && countDepth != 0) return;
    if ((ptcone->TYPE & CNS_MEMSYM) == CNS_MEMSYM && countDepth != 0) return;

    if ((ptcone->TYPE & YAG_CONESEEN) != 0) {
        return;
    }
    ptcone->TYPE |= YAG_CONESEEN;
    countDepth++;

    for (ptedge = ptcone->INCONE; ptedge; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) continue;
        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        if ((ptedge->TYPE & CNS_FEEDBACK) == CNS_FEEDBACK) continue;

        traverse_depth(ptedge->UEDGE.CONE);
    }

    if (processConeFunc(ptcone)) coneChanges = TRUE;
    countDepth--;
}

/*****************************************************************************
 *                       function yagDestroyBranch()                         *
 *****************************************************************************/

/* destruction of a branch in a given cone */

void
yagDestroyBranch(ptcone, ptdel)
    cone_list      *ptcone;
    branch_list    *ptdel;
{
    branch_list    *ptbranch = NULL;
    branch_list   **ptpthead;

    /* destroy the user list */
    freeptype(ptdel->USER);

    if (ptcone != NULL) {
        switch (ptdel->TYPE & (CNS_VDD | CNS_VSS | CNS_EXT | CNS_GND)) {
        case CNS_VDD:
            ptpthead = &(ptcone->BRVDD);
            break;
        case CNS_VSS:
            ptpthead = &(ptcone->BRVSS);
            break;
        case CNS_GND:
            ptpthead = &(ptcone->BRGND);
            break;
        case CNS_EXT:
            ptpthead = &(ptcone->BREXT);
            break;
        }
    }
    else
        ptpthead = &ptdel;

    /* destroy the branch */
    if (ptdel == *ptpthead) {
        /* si c'est la premiere */
        if (ptcone != NULL)
            *ptpthead = ptdel->NEXT;
        freebranch(ptdel);
    }
    else {
        for (ptbranch = *ptpthead; ptbranch; ptbranch = ptbranch->NEXT) {
            /* pour chaque branche */
            if (ptbranch->NEXT == ptdel) {
                ptbranch->NEXT = ptbranch->NEXT->NEXT;
                freebranch(ptdel);
                break;
            }
        }
    }
}

/*****************************************************************************
 *                         function yagAppendBranch()                        *
 *****************************************************************************/

/* append branches pt2 to end of branches pt1 */

branch_list    *
yagAppendBranch(pt1, pt2)
    branch_list    *pt1;
    branch_list    *pt2;
{
    branch_list    *pt = NULL;

    if (pt1 == NULL) {
        return (pt2);
    }
    else {
        for (pt = pt1; pt->NEXT != NULL; pt = pt->NEXT);
        pt->NEXT = pt2;
        return (pt1);
    }
}

/*****************************************************************************
 *                       function yagDelBranchList()                         *
 *****************************************************************************/

/* delete a list of branches  */

void
yagDelBranchList(ptlist)
    branch_list    *ptlist;
{
    branch_list    *ptbranch = NULL;
    branch_list    *ptnextbranch = NULL;

    if (ptlist == NULL)
        return;

    for (ptbranch = ptlist; ptbranch != NULL; ptbranch = ptnextbranch) {
        ptnextbranch = ptbranch->NEXT;
        freebranch(ptbranch);
    }
}

/*****************************************************************************
 *                       function yagCopyBranch()                            *
 *****************************************************************************/

/* duplicate a branch list */

branch_list    *
yagCopyBranch(pthead, ptbranch)
    branch_list    *pthead, *ptbranch;
{
    branch_list    *ptnewbranch;

    if (ptbranch == NULL) {
        yagBug(DBG_NULL_PTR, "yagCopyBranch", NULL, NULL, 0);
    }

    ptnewbranch = addbranch(pthead, ptbranch->TYPE, NULL);
    ptnewbranch->LINK = yagCopyLinkList(ptbranch->LINK);

    return ptnewbranch;
}

/*****************************************************************************
 *                       function yagCountBranches()                         *
 *****************************************************************************/

/* Count the number of branches in a cone except external connector link */

int
yagCountBranches(ptcone)
    cone_list      *ptcone;
{
    branch_list    *ptbranch;
    int             i = 0;

    for (ptbranch = ptcone->BRVDD; ptbranch; ptbranch = ptbranch->NEXT)
        i++;
    for (ptbranch = ptcone->BRVSS; ptbranch; ptbranch = ptbranch->NEXT)
        i++;
    for (ptbranch = ptcone->BREXT; ptbranch; ptbranch = ptbranch->NEXT)
        if (ptbranch->LINK->NEXT != NULL) i++;
    for (ptbranch = ptcone->BRGND; ptbranch; ptbranch = ptbranch->NEXT)
        i++;

    return i;
}

/*****************************************************************************
 *                        function yagCopyLinkList()                         *
 *****************************************************************************/

/* duplicate a link-list */

link_list      *
yagCopyLinkList(ptheadlink)
    link_list      *ptheadlink;
{
    link_list      *ptnewlist = NULL;
    link_list      *ptlink, *ptnewlink;
    link_list      *ptprevlink = NULL;

    for (ptlink = ptheadlink; ptlink != NULL; ptlink = ptlink->NEXT) {
        ptnewlink = addlink(NULL, ptlink->TYPE, ptlink->ULINK.PTR, ptlink->SIG);
        if (ptprevlink == NULL) {
            ptnewlist = ptnewlink;
        }
        else {
            ptprevlink->NEXT = ptnewlink;
        }
        ptprevlink = ptnewlink;
    }
    return ptnewlist;
}

/*****************************************************************************
 *                        function yagCountLinks()                           *
 *****************************************************************************/

/* return the number of elements in a chain_list */

int
yagCountLinks(ptheadlink)
    link_list      *ptheadlink;
{
    link_list      *ptlink;
    int             count = 0;

    for (ptlink = ptheadlink; ptlink; ptlink = ptlink->NEXT) {
        count++;
    }
    return count;
}

int
yagCountActiveLinks(ptheadlink)
    link_list      *ptheadlink;
{
    link_list      *ptlink;
    int             count = 0;

    for (ptlink = ptheadlink; ptlink; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & CNS_RESIST) != CNS_RESIST) count++;
    }
    return count;
}

/*****************************************************************************
 *                       function yagAddTransLink()                          *
 *****************************************************************************/

/* add a new transistor link */

link_list      *
yagAddTransLink(ptheadlink, pttrans, ptsig)
    link_list      *ptheadlink;
    lotrs_list     *pttrans;
    losig_list     *ptsig;
{
    lotrs_list     *ptothertrans;
    ptype_list     *ptuser;
    long            type;

    type = ((pttrans->TYPE & CNS_TN) == CNS_TN ? CNS_TNLINK : CNS_TPLINK);
    if ((pttrans->TYPE & (CAPACITE | RESIST | DIODE)) == 0) {
        type |= CNS_SW;
    }
    else {
        if ((pttrans->TYPE & CAPACITE) == CAPACITE) type |= CNS_CAPA;
        if ((pttrans->TYPE & RESIST) == RESIST) type |= CNS_RESIST;
        if ((pttrans->TYPE & DIODE) == DIODE) {
            if (((locon_list *) getptype(pttrans->USER, YAG_GRIDCON_PTYPE)->DATA)->SIG == ptsig) {
                type |= CNS_DIODE_DOWN;
            }
            else type |= CNS_DIODE_UP;
        }
    }

    if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
        type |= CNS_SWITCH;
        ptothertrans = (lotrs_list *)ptuser->DATA;
    }

    /* orient if bidir blocking is enabled */
    if (YAG_CONTEXT->YAG_BLOCK_BIDIR) {
        pttrans->TYPE |= ORIENTED;
        if (pttrans->SOURCE->SIG == ptsig) pttrans->SOURCE->DIRECTION = 'O';
        else pttrans->DRAIN->DIRECTION = 'O';
    }

    return addlink(ptheadlink, type, pttrans, ptsig);
}

/*****************************************************************************
 *                        function yagFreeEdgeList()                         *
 *****************************************************************************/

/* free all the elements in an edge (input/output) list */

void
yagFreeEdgeList(ptlist)
    edge_list      *ptlist;
{
    edge_list      *pt = NULL;
    edge_list      *nextpt = NULL;

    if (ptlist == NULL) return;
    for (pt = ptlist; pt; pt = nextpt) {
        nextpt = pt->NEXT;
        freeedge(pt);
    }
}

/*****************************************************************************
 *                         function yagCountEdges()                          *
 *****************************************************************************/

/* count the number of elements in an edge list */

int
yagCountEdges(ptlist)
    edge_list      *ptlist;
{
    edge_list      *pt = NULL;
    int             count = 0;

    for (pt = ptlist; pt; pt = pt->NEXT)
        count++;
    return count;
}

/*****************************************************************************
 *                         function yagCountActiveEdges()                          *
 *****************************************************************************/

/* count the number of non-supply elements in an edge list */

int
yagCountActiveEdges(ptlist)
    edge_list      *ptlist;
{
    edge_list      *pt = NULL;
    int             count = 0;

    for (pt = ptlist; pt; pt = pt->NEXT)
        if ((pt->TYPE & CNS_POWER) != CNS_POWER) count++;
    return count;
}

/*****************************************************************************
 *                         function yagCountConeEdges()                          *
 *****************************************************************************/

/* count the number of cone elements in an edge list */

int
yagCountConeEdges(ptlist)
    edge_list      *ptlist;
{
    edge_list      *pt = NULL;
    int             count = 0;

    for (pt = ptlist; pt; pt = pt->NEXT)
        if ((pt->TYPE & CNS_EXT) != CNS_EXT) count++;
    return count;
}

/*****************************************************************************
 *                         function yagGetNextConeEdge()                     *
 *****************************************************************************/

cone_list *
yagGetNextConeEdge(edge_list *ptlist)
{
    edge_list      *pt = NULL;

    for (pt = ptlist; pt; pt = pt->NEXT)
        if ((pt->TYPE & CNS_EXT) != CNS_EXT) break;
    if (pt) return pt->UEDGE.CONE;
    return NULL;
}

/*****************************************************************************
 *                         function yagGetEdge()                             *
 *****************************************************************************/

/* return an edge list element from the data */

edge_list      *
yagGetEdge_QUICK(edge_list *ptedgelist, void *ptinput, ht **FAST)
{
    edge_list      *pt = NULL;
    long cnt;

    if (FAST!=NULL && *FAST!=NULL)
    {
      if ((cnt=gethtitem(*FAST, ptinput))!=EMPTYHT) return (edge_list *)cnt;
      return NULL;
    }
    for (pt = ptedgelist, cnt=0; pt != NULL; pt = pt->NEXT, cnt++) {
        if (pt->UEDGE.PTR == ptinput)
            break;
    }
    if (cnt>32 && FAST!=NULL)
    {
      *FAST=addht(cnt*3>500?cnt*3:500);
      while (ptedgelist!=NULL)
      {
        addhtitem(*FAST, ptedgelist->UEDGE.PTR, (long)ptedgelist);
        ptedgelist=ptedgelist->NEXT;
      }
    }
    return (pt);
}

edge_list      *
yagGetEdge(edge_list *ptedgelist, void *ptinput)
{
  return yagGetEdge_QUICK(ptedgelist, ptinput, NULL);
}

/*****************************************************************************
 *                         function yagGetChain()                            *
 *****************************************************************************/

/* return a chain list element from the data */

chain_list     *
yagGetChain_QUICK(chain_list *pthead, void *ptdata, ht **FAST)
{
    chain_list     *pt = NULL;
    long cnt;

    if (FAST!=NULL && *FAST!=NULL)
    {
      if ((cnt=gethtitem(*FAST, ptdata))!=EMPTYHT) return (chain_list *)cnt;
      return NULL;
    }
    for (pt = pthead, cnt=0; pt != NULL; pt = pt->NEXT, cnt++) {
        if ((char *) pt->DATA == ptdata)
            break;
    }
    if (cnt>32 && FAST!=NULL)
    {
      *FAST=addht(cnt*3>500?cnt*3:500);
      while (pthead!=NULL)
      {
        addhtitem(*FAST, pthead->DATA, (long)pthead);
        pthead=pthead->NEXT;
      }
    }

    return (pt);
}

chain_list     *
yagGetChain(chain_list *pthead, void *ptdata)
{
  return yagGetChain_QUICK(pthead, ptdata, NULL);
}

/*****************************************************************************
 *                         function yagGetPtype()                            *
 *****************************************************************************/

/* return a ptype list element from the data */

ptype_list     *
yagGetPtype(pthead, ptdata)
    ptype_list     *pthead;
    void           *ptdata;
{
    ptype_list     *pt = NULL;

    for (pt = pthead; pt != NULL; pt = pt->NEXT) {
        if ((char *) pt->DATA == ptdata)
            break;
    }
    return (pt);
}

ptype_list     *
yagGetPtypeChain(pthead, ptdata)
    chain_list *pthead;
    void           *ptdata;
{
    ptype_list     *pt = NULL;
    chain_list *cl;

    for (cl = pthead; cl != NULL; cl = cl->NEXT) {
        pt=(ptype_list *)cl->DATA;
        if ((char *) pt->DATA == ptdata)
            break;
    }
    return (pt);
}

/*****************************************************************************
 *                        function yagGetChainInd()                          *
 *****************************************************************************/

 /* obtain a chain list element from  an index */

void *
yagGetChainInd(ptchain, index)
    chain_list *ptchain;
    int         index;
{
    int i;

    for (i = 1; i < index; i++) ptchain = ptchain->NEXT;
    return ptchain->DATA;
}

/*****************************************************************************
 *                        function yagRmvChain()                             *
 *****************************************************************************/

 /* destroy a chain list element from the data != delchain */

chain_list     *
yagRmvChain(pthead, ptdata)
    chain_list     *pthead;
    void           *ptdata;
{
    chain_list     *pt = NULL;
    chain_list     *ptprev = NULL;

    if ((pthead == NULL) || (ptdata == NULL)) {
        yagBug(DBG_NULL_PTR, "yagRmvChain", NULL, NULL, 0);
    }
    else if (ptdata == (char *) pthead->DATA) {
        pt = pthead->NEXT;
        pthead->NEXT = NULL;
        freechain(pthead);
        return (pt);
    }
    else {
        for (pt = pthead; pt; pt = pt->NEXT) {
            if ((char *) pt->DATA == ptdata)
                break;
            ptprev = pt;
        }
        if (pt != NULL) {
            ptprev->NEXT = pt->NEXT;
            pt->NEXT = NULL;
            freechain(pt);
            return (pthead);
        }
    }
    return (pthead);        /* non trouve */
}

/*****************************************************************************
 *                       function yagCountChains()                           *
 *****************************************************************************/

/* return the number of elements in a chain_list */

int
yagCountChains(headchain)
    chain_list     *headchain;
{
    chain_list     *ptchain;
    int             count = 0;

    for (ptchain = headchain; ptchain; ptchain = ptchain->NEXT)
        count++;
    return count;
}

/*****************************************************************************
 *                       function yagCopyChainList()                         *
 *****************************************************************************/

/* duplicate a chain list */
/* nb. order is reversed */

chain_list     *
yagCopyChainList(headchain)
    chain_list     *headchain;
{
    chain_list     *ptold;
    chain_list     *ptnew = NULL;

    if (headchain == NULL) {
        yagBug(DBG_NULL_PTR, "yagCopyChainList", NULL, NULL, 0);
    }

    for (ptold = headchain; ptold != NULL; ptold = ptold->NEXT) {
        ptnew = addchain(ptnew, ptold->DATA);
    }

    return ptnew;
}


/*****************************************************************************
 *                       function yagUnionChainList()                        *
 *****************************************************************************/

/* union of two chain list   */
/* append with unicity check */

chain_list     *
yagUnionChainList(ptchain1, ptchain2)
    chain_list     *ptchain1;
    chain_list     *ptchain2;
{
    chain_list     *ptchain;

    for (ptchain = ptchain2; ptchain; ptchain = ptchain->NEXT) {
        if (yagGetChain(ptchain1, ptchain->DATA) == NULL) {
            ptchain1 = addchain(ptchain1, ptchain->DATA);
        }
    }

    return ptchain1;
}

/*****************************************************************************
 *                         function test_rename()                            *
 *****************************************************************************/

/* cone renaming from the .inf file */

static char    *
test_rename(ifl, name)
    inffig_list *ifl;
    char           *name;
{
    chain_list     *rename;
    inf_assoc      *couple;
    char           *oldname;
    char           *newname;
    char           *test;
    chain_list     *list0;
    
    if (inf_GetPointer(ifl, INF_RENAME, "", (void **)&list0))
    {            
      for (rename = list0; rename != NULL; rename = rename->NEXT) {
  
          couple = (inf_assoc *) rename->DATA;
          oldname = couple->orig;
          newname = couple->dest;
  
          if ((test = joker_name(name, oldname, newname)) != NULL) {
              return test;
          }
          else if (namealloc(name) == oldname) {
              return newname;
          }
      }
    }
    return name;
}

/*****************************************************************************
 *                         function joker_name()                             *
 *****************************************************************************/

/* handling of wildcard cone renaming */

static char    *
joker_name(name, oldname, newname)
    char           *name;
    char           *oldname;
    char           *newname;
{
    char           *pt = oldname;

    while (*pt != '\0') {
        if (*pt == INF_JOK)
            break;
        pt++;
    }

    if (*pt == '\0')
        return NULL;

    if (yagTestJoker(name, oldname) == TRUE) {
        return (joker_subst(name, oldname, newname));
    }
    else
        return NULL;
}

int
yagTestJoker(testname, oldname)
    char           *testname;
    char           *oldname;
{
    char           *name = testname;
    char           *sub = oldname;
    char           *motif;
    int             i, j;

    if (*sub != INF_JOK) {
      if (CASE_SENSITIVE=='Y')
          {
            while (*sub == *name && *sub && *name) {
              sub++;
              name++;
            }
          }
        else
          {
            while (tolower(*sub) == tolower(*name) && *sub && *name) {
              sub++;
              name++;
            }
          }
        if (*sub != INF_JOK)
            return FALSE;
        sub = oldname;
        name = testname;
    }

    if (sub[strlen(sub)] != INF_JOK) {
        i = strlen(sub);
        j = strlen(name);
        if (CASE_SENSITIVE=='Y')
          {
            while (sub[i] == name[j]) {
              j--;
              i--;
            }
          }
        else
          {
            while (tolower(sub[i]) == tolower(name[j])) {
              j--;
              i--;
            }
          }
        if (sub[i] != INF_JOK)
            return FALSE;
    }

    while (*sub != '\0') {
        if (*sub == INF_JOK)
            sub++;
        if (*sub == '\0')
            return TRUE;
        motif = sub_elt(sub, &sub, INF_JOK);
        name = motif_in_name(name, motif);
        mbkfree(motif);
        if (name == NULL)
            return FALSE;
    }
    return TRUE;
}

static char    *
joker_subst(name, subMotif, subName)
    char           *name;
    char           *subMotif;
    char           *subName;
{
    char           *res;
    char            buff[YAGBUFSIZE];


    if ((*subMotif == INF_JOK) && (*subName != INF_JOK)) {
        sprintf(buff, "%s", sub_elt(subName, &subName, INF_JOK));
        strcat(buff, name);
    }
    else
        strcpy(buff, name);

    while (*subMotif != '\0') {
        char           *motif1;
        char           *motif2;

        if (*subMotif == INF_JOK) {
            subMotif++;
            subName++;
        }
        if ((*subMotif == '\0') && (*subName == '\0')) {
            break;
        }
        else if ((*subMotif == '\0') && (*subName != '\0')) {
            strcat(buff, subName);
            break;
        }
        else if ((*subMotif != '\0') && (*subName == '\0')) {
            motif1 = sub_elt(subMotif, &subMotif, INF_JOK);
            buff[strlen(buff) - strlen(motif1)] = '\0';
            break;
        }
        else {
            motif1 = sub_elt(subMotif, &subMotif, INF_JOK);
            motif2 = sub_elt(subName, &subName, INF_JOK);
            name = motif_in_name(name, motif1);
            buff[(strlen(buff) - strlen(name) - strlen(motif1))] = '\0';
            strcat(buff, motif2);
            strcat(buff, name);
        }
    }

    res = (char *) mbkalloc(strlen(buff) + 1);
    strcpy(res, buff);
    return (res);
}

static char    *
sub_elt(sub, addSub, jok)
    char           *sub;
    char          **addSub;
    char            jok;
{
    char            buff[YAGBUFSIZE];
    char           *res = buff;

    while ((*sub != jok) && (*sub != '\0')) {
        *res++ = *sub++;
        (*addSub)++;
    }

    *res = '\0';

    res = (char *) mbkalloc(strlen(buff) + 1);
    strcpy(res, buff);
    return (res);
}

static char    *
motif_in_name(nom, motif)
    char           *nom;
    char           *motif;
{
    char           *pt1 = NULL;
    char           *pt2 = NULL;
    char           *pt3 = NULL;

    if ((motif == NULL) || (nom == NULL)) {
        yagBug(DBG_NULL_PTR, "motif_in_name", NULL, NULL, 0);
    }

    for (pt1 = nom; (*pt1) != '\0'; pt1++) {
      pt3 = motif;
      if (CASE_SENSITIVE=='Y')
       for (pt2 = pt1; (((*pt2) != '\0') && ((*pt3) != '\0') && (*pt2 == *pt3)); pt3++, pt2++);
      else
       for (pt2 = pt1; (((*pt2) != '\0') && ((*pt3) != '\0') && (tolower(*pt2) == tolower(*pt3))); pt3++, pt2++);
      
      if (*pt3 == '\0')
       return (pt2);
    }
    return (NULL);
}

/*****************************************************************************
 *                        function yagVectorizeName()                        *
 *****************************************************************************/

/* replace toto 23 by toto[23] */

char           *
yagVectorizeName(name)
    char           *name;
{
    char            buff[YAGBUFSIZE];
    char           *res;
    char           *car = name;
    int             i = 0;

    while (*car != '\0') {
        if (*car != ' ') {
            buff[i++] = *car++;
        }
        else {
            buff[i++] = '[';
            car++;
            while ((*car >= '0') && (*car <= '9'))
                buff[i++] = *car++;
            buff[i++] = ']';
        }
    }

    buff[i] = '\0';
    res = (char *) mbkalloc(strlen(buff) + 1);
    strcpy(res, buff);
    return (res);
}

/*****************************************************************************
 *                         function yagGetName()                             *
 *****************************************************************************/

/* return the name alias with the least number of segments */

char           *
yagGetName(ifl, pt_sig)
    inffig_list *ifl;
    losig_list     *pt_sig;
{
    chain_list     *pt_namechain = NULL;
    char           *pt_char = NULL;
    char            default_res[YAGBUFSIZE];
    char           *result = NULL;
    char           *result_rename;
    char           *pttest, *ptref;
    int             refisnode = FALSE, testisnode = FALSE;
    short           n_separ = 0;
    short           n_separmin = 100;

    for (pt_namechain = pt_sig->NAMECHAIN; pt_namechain != NULL; pt_namechain = pt_namechain->NEXT) {
        /* pour chaque alias */
    //    if (infHasInfo(ifl, INF_RENAME_INFO)) {
            result_rename = test_rename(ifl, (char *) pt_namechain->DATA);
            if (result_rename != pt_namechain->DATA) {
                result = result_rename;
                break;
            }
  //      }
        n_separ = 0;
        if (pt_namechain->DATA != NULL) {
            for (pt_char = (char *) pt_namechain->DATA; *pt_char != (char) 0; pt_char++) {
                /* on parcourt cet alias */
                if (*pt_char == SEPAR) {
                    /* on compte le nombre de separateurs */
                    n_separ++;
                }
            }
            if (n_separ < n_separmin) {
                n_separmin = n_separ;
                result = (char *) pt_namechain->DATA;
            }
            else if (n_separ == n_separmin) {
                pttest = (char *) pt_namechain->DATA + strlen((char *) pt_namechain->DATA) -1;
                testisnode = FALSE;
                while (isdigit((int)(*pttest)) && pttest > (char *)pt_namechain->DATA) {
                    pttest--;
                    if (*pttest == '_') testisnode = TRUE;
                }
                if ((!testisnode && refisnode) || strlen((char *) pt_namechain->DATA) < strlen(result)) {
                    result = (char *) pt_namechain->DATA;
                    ptref = result + strlen(result) -1;
                    refisnode = FALSE;
                    while (isdigit((int)(*ptref)) && ptref > result) {
                        ptref--;
                        if (*ptref == '_') refisnode = TRUE;
                    }
                }
            }
        }
    }

    /* if no name, build a name from the index */
    if (result == NULL) {
        sprintf(default_res, "yagle_%ld", pt_sig->INDEX);
        result = namealloc(default_res);
    }

    return result;
}

/*****************************************************************************
 *                             function yagDownName()                        *
 *****************************************************************************/

/* for string 'toto' returns 'yagdn_toto' */

char           *
yagDownName(name)
    char           *name;
{
    char            res[YAGBUFSIZE];

    strcpy(res, "yagdn_");
    strcat(res, name);
    return namealloc(res);
}

/*****************************************************************************
 *                             function yagControlBdd()                      *
 *****************************************************************************/

/* conditional reset of BDD toolbox */

void
yagControlBdd(mode)
    int             mode;
{
    if (sysBdd.indiceAT > SMALL) {
        destroyBdd(1);
        initializeBdd(0);
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
    }
    else if (mode == 0) {
        if (numberNodeAllBdd() > SMALL / 2) {
            resetBdd();
            YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
        }
        else
            YAG_CONTEXT->YAG_LASTNODECOUNT = numberNodeAllBdd();
    }
    else {
        resetBdd();
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
    }
}

/*****************************************************************************
 *                             Assorted BDD encapsulation                    *
 *****************************************************************************/

pNode
yagAblToBddCct(pC, expr, abandon_env)
    pCircuit    pC;
    chain_list *expr;
    jmp_buf     abandon_env;
{
    pNode       res;

    res = ablToBddCct(pC, expr);
    if (bddSystemAbandoned()) {
        destroyBdd(1);
        initializeBdd(0);
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
        longjmp(abandon_env, 1);
    }
    return res;
}

pNode
yagApplyBdd(oper, pt, abandon_env)
    short       oper;
    chain_list *pt;
    jmp_buf     abandon_env;
{
    pNode       res;

    res = applyBdd(oper, pt);
    if (bddSystemAbandoned()) {
        destroyBdd(1);
        initializeBdd(0);
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
        longjmp(abandon_env, 1);
    }
    return res;
}

pNode
yagApplyBinBdd(oper, pBdd1, pBdd2, abandon_env)
    short       oper;
    pNode       pBdd1, pBdd2;
    jmp_buf     abandon_env;
{
    pNode       res;

    res = applyBinBdd(oper, pBdd1, pBdd2);
    if (bddSystemAbandoned()) {
        destroyBdd(1);
        initializeBdd(0);
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
        longjmp(abandon_env, 1);
    }
    return res;
}

pNode
yagNotBdd(pBdd, abandon_env)
    pNode       pBdd;
    jmp_buf     abandon_env;
{
    pNode       res;

    res = notBdd(pBdd);
    if (bddSystemAbandoned()) {
        destroyBdd(1);
        initializeBdd(0);
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
        longjmp(abandon_env, 1);
    }
    return res;
}

pNode
yagConstraintBdd(pBdd1, pBddGc, abandon_env)
    pNode       pBdd1, pBddGc;
    jmp_buf     abandon_env;
{
    pNode       res;

    res = constraintBdd(pBdd1, pBddGc);
    if (bddSystemAbandoned()) {
        destroyBdd(1);
        initializeBdd(0);
        YAG_CONTEXT->YAG_LASTNODECOUNT = 0;
        longjmp(abandon_env, 1);
    }
    return res;
}

int
yagBddSupportContained(pBdd, pBddGc)
    pNode pBdd, pBddGc;
{
    chain_list *refsupport;
    chain_list *constraintsupport;
    chain_list *ptchain;

    refsupport = supportIndexBdd(pBdd, 0);
    constraintsupport = supportIndexBdd(pBddGc, 0);

    for (ptchain = constraintsupport; ptchain; ptchain = ptchain->NEXT) {
        if (yagGetChain(refsupport, ptchain->DATA) == NULL) break;
    }
    freechain(constraintsupport);
    freechain(refsupport);
    if (ptchain == NULL) return TRUE;
    return FALSE;
}

pNode
yagSimplestBdd(pNode pBdd1, pNode pBdd2)
{
    chain_list *ptchain1, *ptchain2;
    int         count1, count2;
    
    ptchain1 = supportChain_listBdd(pBdd1);
    ptchain2 = supportChain_listBdd(pBdd2);
    count1 = yagCountChains(ptchain1);
    count2 = yagCountChains(ptchain2);
    freechain(ptchain1);
    freechain(ptchain2);
    if (count2 < count1) return pBdd2;
    return pBdd1;
}

chain_list *
yagCheckConstraint(chain_list *refsupport, chain_list *constraintsupport, chain_list *constraint, ht **FASTSEARCH)
{
    chain_list *ptchain;
    chain_list *ptoppositelist;
    chain_list *ptopchain;
    chain_list *tmpexpr;
    char       *ptname, *ptopname;
    int         found;

    for (ptchain = constraintsupport; ptchain; ptchain = ptchain->NEXT) {
        found = FALSE;
        ptname = (char *)ptchain->DATA;
        if (yagGetChain_QUICK(refsupport, ptname, FASTSEARCH) != NULL) found = TRUE;
        if (!found) {
            ptoppositelist = yagGetOppositeConstraintVars(ptchain->DATA);
            for (ptopchain = ptoppositelist; ptopchain; ptopchain = ptopchain->NEXT) {
                ptopname = (char *)ptopchain->DATA;
                if (yagGetChain_QUICK(refsupport, ptopname, FASTSEARCH) != NULL) {
                    tmpexpr = notExpr(createAtom(ptopname));
                    constraint = substExpr(constraint, ptname, tmpexpr);
                    freeExpr(tmpexpr);
                    found = TRUE;
                    break;
                }
            }
        }
        if (!found) break;
    }

    if (ptchain == NULL) return constraint;
    return NULL;
}

/*****************************************************************************
 *                             function yagGetExtLocon()                     *
 *****************************************************************************/

/* for a signal of type EXTERN returns an external connector */

locon_list     *
yagGetExtLocon(ptsig)
    losig_list     *ptsig;
{
    locon_list     *ptcon;
    chain_list     *headchain, *ptchain;

    if (ptsig->TYPE != CNS_SIGEXT && ptsig->TYPE != CNS_SIGVDD && ptsig->TYPE != CNS_SIGVSS)
        return NULL;

    headchain = (chain_list *) getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *) ptchain->DATA;
        if (ptcon->TYPE == EXTERNAL || ptcon->DIRECTION == CNS_VDDC || ptcon->DIRECTION == CNS_VSSC)
            return ptcon;
    }

    return NULL;
}

/*****************************************************************************
 *                             function yagGetExtLoconList()                 *
 *****************************************************************************/

/* for a signal of type EXTERN returns the external connector list */

chain_list     *
yagGetExtLoconList(ptsig)
    losig_list     *ptsig;
{
    locon_list     *ptcon;
    chain_list     *reschain = NULL;
    chain_list     *headchain, *ptchain;

    if (ptsig->TYPE != CNS_SIGEXT && ptsig->TYPE != CNS_SIGVDD && ptsig->TYPE != CNS_SIGVSS)
        return NULL;

    headchain = (chain_list *) getptype(ptsig->USER, LOFIGCHAIN)->DATA;
    for (ptchain = headchain; ptchain != NULL; ptchain = ptchain->NEXT) {
        ptcon = (locon_list *) ptchain->DATA;
        if (ptcon->TYPE == EXTERNAL || ptcon->DIRECTION == CNS_VDDC || ptcon->DIRECTION == CNS_VSSC)
            reschain = addchain(reschain, ptcon);
    }

    return reschain;
}

/*****************************************************************************
 *                         table generation functions                        *
 *****************************************************************************/

 /*-----------------------------------------------------*
  | allows the building of a dynamically resizable      |
  | table of generic pointers                           |
  *-----------------------------------------------------*/

static table *YAG_FREE_TABLE = NULL;

table          *
newtable()
{
    table *pttable;

    if (YAG_FREE_TABLE == NULL) {
        pttable = (table *) mbkalloc(sizeof(table));
        pttable->NEXT = NULL;
        pttable->ALLOC = 0;
        pttable->DATA = NULL;
    }
    else {
        pttable = YAG_FREE_TABLE;
        YAG_FREE_TABLE = pttable->NEXT;
        pttable->NEXT = NULL;
    }
    pttable->SIZE = 0;

    return pttable;
}

void
addtableitem(pttable, item)
    table          *pttable;
    void           *item;
{
    if (pttable->SIZE == pttable->ALLOC) {
        if (pttable->DATA == NULL) {
            pttable->DATA = mbkalloc(100 * sizeof(void *));
            pttable->ALLOC = 100;
        }
        else {
            pttable->DATA = mbkrealloc(pttable->DATA, (pttable->ALLOC + 100) * sizeof(void *));
            pttable->ALLOC += 100;
        }
    }
    (pttable->DATA)[(pttable->SIZE)++] = item;
}

void
deltable(pttable)
    table          *pttable;
{
    pttable->NEXT = YAG_FREE_TABLE;
    pttable->SIZE = 0;
    YAG_FREE_TABLE = pttable;
}

void
yag_freetable(void)
{
    table *pttable;

    while (YAG_FREE_TABLE) {
        pttable = YAG_FREE_TABLE;
        YAG_FREE_TABLE = pttable->NEXT;
        mbkfree(pttable->DATA);
        mbkfree(pttable);
    }
}

float
yagCalcBranchResistance(ptbranch)
    branch_list    *ptbranch;
{
    link_list      *ptlink;
    lotrs_list     *pttrans;
    float           resistance = 0.0;
    float           conductance = 0.0, other_conductance = 0.0;
    float           mobility = 0.0, other_mobility = 0.0;
    ptype_list     *ptuser;
    chain_list     *ptchain;

    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        if ((ptbranch->TYPE & CNS_EXT) == CNS_EXT && ptlink->NEXT == NULL)
            break;
        if ((ptlink->TYPE & CNS_TPLINK) == CNS_TPLINK) {
            mobility = 2.3;
            other_mobility = 1.0;
        }
        else {
            mobility = 1.0;
            other_mobility = 2.3;
        }
        pttrans = ptlink->ULINK.LOTRS;
        if ((ptuser = getptype(pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
            conductance = 0.0;
            for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                pttrans = (lotrs_list *)ptchain->DATA;
                conductance += (((float) pttrans->WIDTH / (float) pttrans->LENGTH));
            }
            conductance = conductance / mobility;
        }
        else {
            conductance = ((float)pttrans->WIDTH / (float)pttrans->LENGTH) / mobility;
        }
        if ((ptuser = getptype(pttrans->USER, CNS_SWITCH)) != NULL) {
            pttrans = (lotrs_list *)ptuser->DATA;
            if ((ptuser = getptype(pttrans->USER, MBK_TRANS_PARALLEL)) != NULL) {
                for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                    pttrans = (lotrs_list *)ptchain->DATA;
                    other_conductance += (((float) pttrans->WIDTH / (float) pttrans->LENGTH));
                }
                conductance += other_conductance / other_mobility;
            }
            else {
                conductance += ((float)pttrans->WIDTH / (float)pttrans->LENGTH) / other_mobility;
            }
        }
        if (conductance != 0) resistance += 1/conductance;
    }
    return resistance;
}

float
yagCalcParallelResistance(chain_list *ptbranchlist)
{
    chain_list     *ptchain;
    float           sum = 0.0;
    float           branchres;

    for (ptchain = ptbranchlist; ptchain; ptchain = ptchain->NEXT) {
        branchres = yagCalcBranchResistance((branch_list *) ptchain->DATA);
        sum += 1 / branchres;
    }
    return (1 / sum);
}

float 
yagCalcMaxDownRes(cone_list *ptcone)
{
    branch_list    *ptbranch;
    chain_list     *paralist;
    chain_list     *ptbranchlist;
    chain_list     *ptchain;
    float           maxres = 0.0;
    float           resistance;
    int             has_para = FALSE;
    branch_list    *brlist[2];
    int             i;

    brlist[0] = ptcone->BRVSS;
    brlist[1] = ptcone->BREXT;

    for (i = 0; i < 2; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0)
                continue;
            if ((ptbranch->TYPE & CNS_PARALLEL) != 0) {
                has_para = TRUE;
                continue;
            }
            resistance = yagCalcBranchResistance(ptbranch);
            if (maxres < resistance)
                maxres = resistance;
        }
    }

    if (has_para) {
        paralist = (chain_list *) getptype(ptcone->USER, CNS_PARALLEL)->DATA;
        for (ptchain = paralist; ptchain; ptchain = ptchain->NEXT) {
            ptbranchlist = (chain_list *) ptchain->DATA;
            ptbranch = (branch_list *) ptbranchlist->DATA;
            if ((ptbranch->TYPE & CNS_VSS) != CNS_VSS && (ptbranch->TYPE & CNS_EXT) != CNS_EXT)
                continue;
            resistance = yagCalcParallelResistance(ptbranchlist);
            if (maxres < resistance)
                maxres = resistance;
        }
    }

    return maxres;
}

float 
yagCalcMaxUpRes(cone_list *ptcone)
{
    branch_list    *ptbranch;
    chain_list     *paralist;
    chain_list     *ptbranchlist;
    chain_list     *ptchain;
    float           maxres = 0.0;
    float           resistance;
    int             has_para = FALSE;
    branch_list    *brlist[2];
    int             i;

    brlist[0] = ptcone->BRVDD;
    brlist[1] = ptcone->BREXT;

    for (i = 0; i < 2; i++) {
        for (ptbranch = brlist[i]; ptbranch; ptbranch = ptbranch->NEXT) {
            if ((ptbranch->TYPE & CNS_NOT_FUNCTIONAL) != 0)
                continue;
            if ((ptbranch->TYPE & CNS_PARALLEL) != 0) {
                has_para = TRUE;
                continue;
            }
            resistance = yagCalcBranchResistance(ptbranch);
            if (maxres < resistance)
                maxres = resistance;
        }
    }

    if (has_para) {
        paralist = (chain_list *) getptype(ptcone->USER, CNS_PARALLEL)->DATA;
        for (ptchain = paralist; ptchain; ptchain = ptchain->NEXT) {
            ptbranchlist = (chain_list *) ptchain->DATA;
            ptbranch = (branch_list *) ptbranchlist->DATA;
            if ((ptbranch->TYPE & CNS_VDD) != CNS_VDD && (ptbranch->TYPE & CNS_EXT) != CNS_EXT)
                continue;
            resistance = yagCalcParallelResistance(ptbranchlist);
            if (maxres < resistance)
                maxres = resistance;
        }
    }

    return maxres;
}

long 
yagGetVal_s(losig_list *ptsig)
{
    ptype_list     *ptuser;

    ptuser = getptype(ptsig->USER, YAG_VAL_S_PTYPE);
    if (ptuser == NULL) {
        return -1;
    }
    return (long) ptuser->DATA;
}

void 
yagAddVal_s(inffig_list *ifl, lofig_list *ptfig)
{
    chain_list     *ptchain;
    losig_list     *ptsig;
    long            res;
    long            i;
    chain_list *list;

    list=inf_GetEntriesByType(ifl, INF_DIROUT, INF_ANY_VALUES);
    
    for (ptsig = ptfig->LOSIG; ptsig; ptsig = ptsig->NEXT) {
        res = -1;
        for (ptchain = ptsig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
            i = yagTestVal_s(ptchain->DATA, 's', ifl, list);
            if (i > res)
                res = i;
        }
        if (res != -1) ptsig->USER = addptype(ptsig->USER, YAG_VAL_S_PTYPE, (void *) res);
    }

    freechain(list);
}

long 
yagTestVal_s(char *name, char c, inffig_list *ifl, chain_list *alldirout)
{
    char           *ptchar;
    int val;
    chain_list *cl;
    
    if (inf_GetInt(ifl, name, INF_DIROUT, &val)) return val;

    for (cl=alldirout; cl!=NULL; cl=cl->NEXT)
      if (yagTestJoker(name, (char *)cl->DATA)==TRUE)
         if (inf_GetInt(ifl, (char *)cl->DATA, INF_DIROUT, &val)) return val;
    
    if (YAG_CONTEXT->YAG_HELP_S) {
        for (ptchar = name; *ptchar != '\0'; ptchar++) {
            if (*ptchar == '_') {
                ptchar++;
                if (*ptchar == c) {
                    ptchar++;
                    if ((*ptchar == '\0') || (*ptchar == ' ') || (*ptchar == '['))
                        return 0;
                    else if ((*ptchar >= '0') && (*ptchar <= '9')) {
                        ptchar++;
                        if ((*ptchar == '\0') || (*ptchar == ' ') || (*ptchar == '['))
                            return (*(--ptchar) - '0' + 1);
                        ptchar--;
                    }
                    ptchar--;
                }
                ptchar--;
            }
        }
    }
    return -1;
}

char
yagMergeDirection(char dir1, char dir2)
{
    if (dir2 == UNKNOWN) return dir1;
    if (dir1 == UNKNOWN) return dir2;
    switch (dir1) {
        case IN:
            switch (dir2) {
                case OUT:
                    return INOUT;
                case TRISTATE:
                    return TRANSCV;
                case INOUT:
                case TRANSCV:
                case IN:
                    return dir2;
            }
        case OUT:
            switch (dir2) {
                case IN:
                    return INOUT;
                case TRISTATE:
                    return OUT;
                case OUT:
                case INOUT:
                case TRANSCV:
                    return dir2;
            }
        case TRISTATE:
            switch (dir2) {
                case IN:
                    return TRANSCV;
                case OUT:
                case INOUT:
                case TRISTATE:
                case TRANSCV:
                    return dir2;
            }
        case INOUT:
            return INOUT;
        case TRANSCV:
            switch (dir2) {
                case IN:
                    return TRANSCV;
                case OUT:
                    return INOUT;
                case TRISTATE:
                    return TRANSCV;
                case INOUT:
                case TRANSCV:
                    return dir2;
            }
		default:
			return UNKNOWN;
    }
}

/*****************************************************************************
 *                             function yagInfMarkTrans()                    *
 *****************************************************************************/

void
yagInfMarkTrans(inffig_list *ifl, lofig_list *ptlofig, chain_list *ptfulllist)
{
    int val;
    unsigned long mask;
    lotrs_list *ptlotrs;
    ptype_list *ptuser;
    chain_list *ptlist = NULL, *ptchain;
    ht *usedht;

    usedht = addht(50);
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        if (inf_GetInt(ifl, ptlotrs->TRNAME, INF_MARKTRANS, &val)) {
            addhtitem(usedht, ptlotrs->TRNAME, 0);
            mask = 0;
            if (val & INF_TRANS_BLEEDER) mask |= FCL_BLEEDER;
            if (val & INF_TRANS_FEEDBACK) mask |= FCL_FEEDBACK;
            if (val & INF_TRANS_COMMAND) mask |= FCL_COMMAND;
            if (val & INF_TRANS_NOT_FUNCTIONAL) mask |= FCL_NOT_FUNCTIONAL;
            if (val & INF_TRANS_BLOCKER) mask |= FCL_BLOCKER;
            if (val & INF_TRANS_SHORT) mask |= FCL_SHORT;
            if (val & INF_TRANS_UNUSED) mask |= FCL_NEVER;
            
            if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                ptuser->DATA = (void *)((unsigned long)ptuser->DATA | mask);
            }
            else ptlotrs->USER = addptype(ptlotrs->USER, FCL_TRANSFER_PTYPE, (void *)mask);
        }
    }
    for (ptchain = ptfulllist; ptchain; ptchain = ptchain->NEXT) {
        if (gethtitem(usedht, ptchain->DATA) == EMPTYHT) ptlist = addchain(ptlist, ptchain->DATA);
    }
    delht(usedht);

    if (ptlist == NULL) return;
    
    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
        for (ptchain = ptlist; ptchain; ptchain = ptchain->NEXT) {
            if (mbk_TestREGEX(ptlotrs->TRNAME, (char *)ptchain->DATA)) {
                if (inf_GetInt(ifl, (char *)ptchain->DATA, INF_MARKTRANS, &val)) {
                    mask = 0;
                    if (val & INF_TRANS_BLEEDER) mask |= FCL_BLEEDER;
                    if (val & INF_TRANS_FEEDBACK) mask |= FCL_FEEDBACK;
                    if (val & INF_TRANS_COMMAND) mask |= FCL_COMMAND;
                    if (val & INF_TRANS_NOT_FUNCTIONAL) mask |= FCL_NOT_FUNCTIONAL;
                    if (val & INF_TRANS_BLOCKER) mask |= FCL_BLOCKER;
                    if (val & INF_TRANS_SHORT) mask |= FCL_SHORT;
                    if (val & INF_TRANS_UNUSED) mask |= FCL_NEVER;
                    
                    if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                        ptuser->DATA = (void *)((unsigned long)ptuser->DATA | mask);
                    }
                    else ptlotrs->USER = addptype(ptlotrs->USER, FCL_TRANSFER_PTYPE, (void *)mask);
                }
            }
        }
    }
    freechain(ptlist);
}

void
yagMarkUnusedTrans(lofig_list *ptlofig)
{
    unsigned long mask;
    lotrs_list *ptlotrs;
    ptype_list *ptuser;

    for (ptlotrs = ptlofig->LOTRS; ptlotrs; ptlotrs = ptlotrs->NEXT) {
      if (getptype(ptlotrs->USER, BBOX_AS_UNUSED)!=NULL) {
        mask = FCL_NEVER;
        if ((ptuser = getptype(ptlotrs->USER, FCL_TRANSFER_PTYPE)) != NULL) {
            ptuser->DATA = (void *)((unsigned long)ptuser->DATA | mask);
        }
        else ptlotrs->USER = addptype(ptlotrs->USER, FCL_TRANSFER_PTYPE, (void *)mask);
      }
    }
}

/*****************************************************************************
 *                             function yagInfMarkSig()                      *
 *****************************************************************************/

void
yagInfMarkSig(inffig_list *ifl, lofig_list *ptlofig, chain_list *ptfulllist)
{
    int val;
    unsigned long mask = 0;
    losig_list *ptlosig;
    ptype_list *ptuser;
    chain_list *ptchain;
    chain_list *ptlist = NULL, *ptchain2;
    ht *usedht;

    usedht = addht(50);

    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        for (ptchain = ptlosig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
            if (inf_GetInt(ifl, (char *)ptchain->DATA, INF_MARKSIG, &val)) {
                addhtitem(usedht, (char *)ptchain->DATA, 0);
                mask = 0;
                if (val & INF_NET_LATCH) mask |= FCL_LATCH;
                if (val & INF_NET_MEMSYM) mask |= FCL_MEMSYM;
                if (val & INF_NET_RS) mask |= FCL_RS;
                if (val & INF_NET_FLIPFLOP) mask |= FCL_FLIPFLOP;
                if (val & INF_NET_MASTER) mask |= FCL_MASTER;
                if (val & INF_NET_SLAVE) mask |= FCL_SLAVE;
                if (val & INF_NET_BLOCKER) mask |= FCL_BLOCKER;
                if (val & INF_NET_NOFALSEBRANCH) mask |= FCL_NOFALSEBRANCH;
                if (val & INF_NET_VDD) mask |= FCL_VDD;
                if (val & INF_NET_VSS) mask |= FCL_VSS;
                
                if ((ptuser = getptype(ptlosig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                    ptuser->DATA = (void *)((unsigned long)ptuser->DATA | mask);
                }
                else ptlosig->USER = addptype(ptlosig->USER, FCL_TRANSFER_PTYPE, (void *)mask);
            }
        }
    }
    for (ptchain = ptfulllist; ptchain; ptchain = ptchain->NEXT) {
        if (gethtitem(usedht, ptchain->DATA) == EMPTYHT) ptlist = addchain(ptlist, ptchain->DATA);
    }
    delht(usedht);

    if (ptlist == NULL) return;
    
    for (ptlosig = ptlofig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        for (ptchain = ptlosig->NAMECHAIN; ptchain; ptchain = ptchain->NEXT) {
            for (ptchain2 = ptlist; ptchain2; ptchain2 = ptchain2->NEXT) {
                if (mbk_TestREGEX((char *)ptchain->DATA, (char *)ptchain2->DATA)) {
                    if (inf_GetInt(ifl, (char *)ptchain2->DATA, INF_MARKSIG, &val)) {
                        mask = 0;
                        if (val & INF_NET_LATCH) mask |= FCL_LATCH;
                        if (val & INF_NET_MEMSYM) mask |= FCL_MEMSYM;
                        if (val & INF_NET_RS) mask |= FCL_RS;
                        if (val & INF_NET_FLIPFLOP) mask |= FCL_FLIPFLOP;
                        if (val & INF_NET_MASTER) mask |= FCL_MASTER;
                        if (val & INF_NET_SLAVE) mask |= FCL_SLAVE;
                        if (val & INF_NET_BLOCKER) mask |= FCL_BLOCKER;
                        if (val & INF_NET_NOFALSEBRANCH) mask |= FCL_NOFALSEBRANCH;
                        if (val & INF_NET_VDD) mask |= FCL_VDD;
                        if (val & INF_NET_VSS) mask |= FCL_VSS;

                        if ((ptuser = getptype(ptlosig->USER, FCL_TRANSFER_PTYPE)) != NULL) {
                            ptuser->DATA = (void *)((unsigned long)ptuser->DATA | mask);
                        }
                        else ptlosig->USER = addptype(ptlosig->USER, FCL_TRANSFER_PTYPE, (void *)mask);
                    }
                }
            }
        }
    }
    freechain(ptlist);
}

static ht *checkdirectiveht = NULL;
static chain_list *checkdirectivelist = NULL;

void
yagEndAddCheckDirective(void)
{
    chain_list *ptchain;

    for (ptchain = checkdirectivelist; ptchain; ptchain = ptchain->NEXT) {
        freeptype((ptype_list *)ptchain->DATA);
    }
    freechain(checkdirectivelist);
    checkdirectivelist = NULL;
    delht(checkdirectiveht);
    checkdirectiveht = NULL;
}

int
yagAddCheckDirective(inffig_list *ifl, char *dataname, char *clockname, int clockstate)
{
    int val;
    int add = TRUE;
    char dir;
    splitint *si=(splitint *)&val;
    ptype_list *check;

    if (clockstate) dir=INF_DIRECTIVE_UP; else dir=INF_DIRECTIVE_DOWN;
    si->cval.a=0; // check
    si->cval.b=0; // data rise and fall
    si->cval.c=0; // with
    si->cval.d=INF_DIRECTIVE_CLOCK|dir; // clock rise or fall

    if (checkdirectiveht == NULL) checkdirectiveht = addht(20);
    
    if ((check = (ptype_list *)gethtitem(checkdirectiveht, dataname)) != (ptype_list *)EMPTYHT) {
        for (; check; check = check->NEXT) {
            if (check->DATA == clockname && check->TYPE == (long)val) {
                add = FALSE;
                break;
            }
        }
    }
    
    if (add) {
        inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_DIRECTIVES, "", infTasVectName(dataname), infTasVectName(clockname), val, 0, NULL);
        if ((check = (ptype_list *)gethtitem(checkdirectiveht, dataname)) != (ptype_list *)EMPTYHT) {
            append((chain_list *)check, (chain_list *)addptype(NULL, (long)val, clockname));
        }
        else {
            check = addptype(NULL, (long)val, clockname);
            addhtitem(checkdirectiveht, dataname, (long)check);
            checkdirectivelist = addchain(checkdirectivelist, check);
        }
        return TRUE;
    }
    return FALSE;
}

void
yagAddFilterDirective(inffig_list *ifl, char *outname, int outstate)
{
    int val;
    char dir;
    splitint *si=(splitint *)&val;

    if (outstate) dir=INF_DIRECTIVE_UP; else dir=INF_DIRECTIVE_DOWN;
    si->cval.a=INF_DIRECTIVE_FILTER; // filter
    si->cval.b=0; // data rise and fall
    si->cval.c=0; // with
    si->cval.d=INF_DIRECTIVE_CLOCK|dir; // clock rise or fall

    inf_AddAssociation(ifl, INF_LOADED_LOCATION, INF_DIRECTIVES, "", infTasVectName(outname), infTasVectName(outname), val, 0, NULL);
}

void yagDriveConeSignalAliases(cnsfig_list *cf, ht *renamed, ht *morealiases)
{
  cone_list *cn;
  FILE *f;
  losig_list *ls;
  chain_list *cl;
  char buf[1024];
  char *name;
  long l, l0;

  sprintf(buf,"%s.aliases",cf->NAME);
  f=fopen(buf,"w");
  if (f!=NULL)
  {
    for (cn=cf->CONE; cn!=NULL; cn=cn->NEXT)
    {
      ls = (losig_list *)getptype(cn->USER, CNS_SIGNAL)->DATA;
      if (!mbk_LosigIsVDD(ls) && !mbk_LosigIsVSS(ls) && ls->NAMECHAIN!=NULL && ls->NAMECHAIN->NEXT!=NULL)
      {
        name=getsigname(ls);
        if (renamed!=NULL && (l=gethtitem(renamed, name))!=EMPTYHT && l!=(long)name)
        {
         fprintf(f,"%s,",(char *)l);
         if ((l0=gethtitem(morealiases, name))!=EMPTYHT)
         {
           for (cl=(chain_list *)l0; cl!=NULL; cl=cl->NEXT)
             if (cl->DATA!=(void *)l) fprintf(f,"%s,", (char *)cl->DATA);
         }
        }
        for (cl=ls->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
         fprintf(f,"%s%s", (char *)cl->DATA,cl->NEXT?",":"");
        fprintf(f,"\n");
      }
    }
    fclose(f);
  }
}

void yagFreeAliasHT(ht *morealiases)
{
  chain_list *cl;
  if (morealiases)
  {
    cl=GetAllHTElems(morealiases);
    while (cl!=NULL)
    {
      freechain((chain_list *)cl->DATA);
      cl=delchain(cl,cl);
    }
    delht(morealiases);
  }
}
