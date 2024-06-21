#include "yag_headers.h"

static void displayNode (gnode_list *ptnode);
static void displayNodeLog (int lib, int loglevel, gnode_list *ptnode);
static void unmark (gnode_list *ptnode);
static int testCycle (gnode_list *ptnode);
static void dumpLoop (chain_list *ptlist);

static chain_list *VISIT_LIST;

void 
yagViewBefig(befig_list *fig)
{
    struct bepor   *por;
    struct beout   *out;
    struct berin   *in;
    struct bereg   *reg;
    struct beaux   *aux;
    struct beaux   *dly;
    struct bebus   *bus;
    struct bebux   *bux;
    struct biabl   *ptbiabl;

    printf("Figure : %s\n\n", fig->NAME);

    printf("BEPOR :-\n\n");
    for (por = fig->BEPOR; por; por = por->NEXT) {
        printf("\t%s : dir=%c : type = %c\n", por->NAME, por->DIRECTION, por->TYPE);
    }
    printf("\n");
    printf("BEOUT :-\n\n");
    for (out = fig->BEOUT; out; out = out->NEXT) {
        printf("\t%s :- ", out->NAME);
        displayExpr(out->ABL); printf("\n");
    }
    printf("\n");
    printf("BERIN :-\n\n");
    for (in = fig->BERIN; in; in = in->NEXT) {
        printf("\t%s :- \n", in->NAME);
    }
    printf("\n");
    printf("BEREG :-\n\n");
    for (reg = fig->BEREG; reg; reg = reg->NEXT) {
        printf("\t%s :- \n", reg->NAME);
        for (ptbiabl = reg->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            printf("\t\texpression :- ");
            displayExpr(ptbiabl->VALABL); printf("\n");
            printf("\t\tcondition :- ");
            displayExpr(ptbiabl->CNDABL); printf("\n");
        }
    }
    printf("\n");
    printf("BEAUX :-\n\n");
    for (aux = fig->BEAUX; aux; aux = aux->NEXT) {
        printf("\t%s :- ", aux->NAME);
        displayExpr(aux->ABL); printf("\n");
    }
    printf("\n");
    printf("BEDLY :-\n\n");
    for (dly = fig->BEDLY; dly; dly = dly->NEXT) {
        printf("\t%s :- ", dly->NAME);
        displayExpr(dly->ABL); printf("\n");
    }
    printf("\n");
    printf("BEBUS :-\n\n");
    for (bus = fig->BEBUS; bus; bus = bus->NEXT) {
        printf("\t%s :- \n", bus->NAME);
        for (ptbiabl = bus->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            printf("\t\texpression :- ");
            displayExpr(ptbiabl->VALABL); printf("\n");
            printf("\t\tcondition :- ");
            displayExpr(ptbiabl->CNDABL); printf("\n");
        }
    }
    printf("\n");
    printf("BEBUX :-\n\n");
    for (bux = fig->BEBUX; bux; bux = bux->NEXT) {
        printf("\t%s :- \n", bux->NAME);
        for (ptbiabl = bux->BIABL; ptbiabl; ptbiabl = ptbiabl->NEXT) {
            printf("\t\texpression :- ");
            displayExpr(ptbiabl->VALABL); printf("\n");
            printf("\t\tcondition :- ");
            displayExpr(ptbiabl->CNDABL); printf("\n");
        }
    }
}

void 
yagDisplayGraph(graph *ptgraph)
{
    gnode_list     *ptnode, *ptinnode;
    chain_list     *ptchain;
    ptype_list     *ptptype;
    ptype_list     *ptroots;

    printf("\n");
    printf("Displaying Graph :-\n\n");
    printf("Contraints :-\n");
    for (ptptype = ptgraph->CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
        displayExpr((chain_list *) ptptype->DATA);
    }
    printf("\n");
    printf("Rootnodes :-\n");
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            printf("\t%ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            printf("\tX (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    printf("\n");
    printf("Primary Variables :-\n");
    for (ptchain = ptgraph->PRIMVARS; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *) ptchain->DATA;
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            printf("\t%ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            printf("\tX (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    printf("\n");
    for (ptnode = ptgraph->HEADNODE; ptnode; ptnode = ptnode->NEXT) {
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            printf("node %ld (%s) %lx of depth %d has %d sons\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME, ptnode->TYPE, ptnode->DEPTH, ptnode->NUMSONS);
        }
        else {
            printf("node X (%s) %lx of depth %d has %d sons\n", ptnode->OBJECT.LOCON->NAME, ptnode->TYPE, ptnode->DEPTH, ptnode->NUMSONS);
        }
        if (ptnode->FATHERS != NULL)
            printf("depends upon :-\n");
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            ptinnode = (gnode_list *) ptchain->DATA;
            if ((ptinnode->TYPE & CONE_TYPE) == CONE_TYPE) {
                printf("\t%ld (%s)\n", ptinnode->OBJECT.CONE->INDEX, ptinnode->OBJECT.CONE->NAME);
            }
            else {
                printf("\tX (%s)\n", ptinnode->OBJECT.LOCON->NAME);
            }
        }
    }
    printf("\n");
    fflush(stdout);
}

static void
dumpNodeBehaviour(befig_list *ptbefig, gnode_list *ptnode)
{
    char      *name;
    cone_list *ptcone;
    ptype_list *ptuser;
    chain_list *expr;
    chain_list *ptchain;
    gnode_list *ptinnode;

    if (ptnode->VISITED)
        return;
    ptnode->VISITED = TRUE;

    if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) name = ptnode->OBJECT.CONE->NAME;
    else name = ptnode->OBJECT.LOCON->NAME;
    if ((ptnode->TYPE & PRIMNODE) == PRIMNODE) {
        ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, name, 'I', 'B');
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
    }
    else {
        ptcone = ptnode->OBJECT.CONE;
        if ((ptcone->TYPE & YAG_HASDUAL) != 0) {
           ptuser = getptype(ptcone->USER, YAG_DUALEXPR_PTYPE);
        }
        else ptuser = getptype(ptcone->USER, CNS_UPEXPR);
        if (ptuser->DATA == NULL) expr = createAtom("'0'");
        else expr = copyExpr((chain_list *)ptuser->DATA);
        ptbefig->BEAUX = beh_addbeaux(ptbefig->BEAUX, name, expr, NULL, 0);
        ptbefig->BERIN = beh_addberin(ptbefig->BERIN, name);
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            ptinnode = (gnode_list *)ptchain->DATA;
            dumpNodeBehaviour(ptbefig, ptinnode);
        }
    }
}

void
yagDumpBranchBefig(branch_list *ptbranch, lotrs_list *ptnewtrans, graph *ptgraph, char *name)
{
    befig_list *ptbefig;
    ptype_list *ptuser;
    chain_list *expr;
    chain_list *ptchain, *ptchain1;
    ptype_list *ptroots;
    gnode_list *ptnode;
    link_list  *ptlink;
    gnode_list *ptinnode;
    lotrs_list *pttrans;
    cone_list *ptcone;
    chain_list *conechain = NULL;
    
    for (ptlink = ptbranch->LINK; ptlink; ptlink = ptlink->NEXT) {
        if ((ptlink->TYPE & (CNS_IN|CNS_INOUT)) != 0) break;
        pttrans = ptlink->ULINK.LOTRS;
        if ((pttrans->TYPE & DIODE) != 0) continue;
        conechain = addchain(conechain, (cone_list *)pttrans->GRID);
    }
    if (ptnewtrans) conechain = addchain(conechain, (cone_list *)ptnewtrans->GRID);
        
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
    }

    ptbefig = beh_addbefig(NULL, name);
    for (ptchain = conechain; ptchain; ptchain = ptchain->NEXT) {
        ptcone = (cone_list *)ptchain->DATA;
        ptnode = (gnode_list *)gethtitem(YAG_CONTEXT->YAG_CONE_GRAPH->HASHTAB, ptcone);
        if ((ptnode->TYPE & PRIMNODE) == PRIMNODE) {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, ptcone->NAME, 'B', 'B');
        }
        else {
            ptbefig->BEPOR = beh_addbepor(ptbefig->BEPOR, ptcone->NAME, 'O', 'B');
            if ((ptcone->TYPE & YAG_HASDUAL) != 0) {
               ptuser = getptype(ptcone->USER, YAG_DUALEXPR_PTYPE);
            }
            else ptuser = getptype(ptcone->USER, CNS_UPEXPR);
            if (ptuser->DATA == NULL) expr = createAtom("'0'");
            else expr = copyExpr((chain_list *)ptuser->DATA);
            ptbefig->BEOUT = beh_addbeout(ptbefig->BEOUT, ptcone->NAME, expr, NULL, 0);
            for (ptchain1 = ptnode->FATHERS; ptchain1; ptchain1 = ptchain1->NEXT) {
                ptinnode = (gnode_list *)ptchain1->DATA;
                dumpNodeBehaviour(ptbefig, ptinnode);
            }
        }
    }

    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
    }
    beh_depend(ptbefig);
    if (YAG_CONTEXT->YAG_MINIMISE_INV) yagSuppressBeaux(ptbefig);
    if (YAG_CONTEXT->YAG_SIMPLIFY_EXPR) yagSimplifyExpr(ptbefig);
    if (YAG_CONTEXT->YAG_SIMPLIFY_PROCESSES) yagSimplifyProcesses(ptbefig);
    savebefig(ptbefig, 0);
}

void 
yagDisplayUsedGraph(graph *ptgraph, int level)
{
    gnode_list     *ptnode;
    chain_list     *ptchain;
    ptype_list     *ptptype;
    ptype_list     *ptroots;

    printf("\n");
    printf("Displaying Graph :-\n\n");
    if (ptgraph->CONSTRAINTS != NULL) {
        printf("Contraints :-\n");
        for (ptptype = ptgraph->CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
            displayExpr((chain_list *) ptptype->DATA);
        }
        printf("\n");
    }
    printf("Rootnodes :-\n");
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            printf("\t%ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            printf("\tX (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    printf("\n");
    printf("Primary Variables :-\n");
    for (ptchain = ptgraph->PRIMVARS; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *) ptchain->DATA;
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            printf("\t%ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            printf("\tX (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    printf("\n");

    if (level != 0) {
        for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
            ptnode = (gnode_list *) ptroots->DATA;
            displayNode(ptnode);
        }
    }
    fflush(stdout);
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
    }
}

void 
yagDisplayUsedGraphLog(int lib, int loglevel, graph *ptgraph, int level)
{
    gnode_list     *ptnode;
    chain_list     *ptchain;
    ptype_list     *ptptype;
    ptype_list     *ptroots;

    avt_log(lib, loglevel, "\n");
    avt_log(lib, loglevel, "Displaying Graph :-\n\n");
    if (ptgraph->CONSTRAINTS != NULL) {
        avt_log(lib, loglevel, "Contraints :-\n");
        for (ptptype = ptgraph->CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
            displayExprLog(lib, loglevel, (chain_list *) ptptype->DATA);
        }
        avt_log(lib, loglevel, "\n");
    }
    avt_log(lib, loglevel, "Rootnodes :-\n");
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            avt_log(lib, loglevel, "\t%ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            avt_log(lib, loglevel, "\tX (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    avt_log(lib, loglevel, "\n");
    avt_log(lib, loglevel, "Primary Variables :-\n");
    for (ptchain = ptgraph->PRIMVARS; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *) ptchain->DATA;
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            avt_log(lib, loglevel, "\t%ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            avt_log(lib, loglevel, "\tX (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    avt_log(lib, loglevel, "\n");

    if (level != 0) {
        for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
            ptnode = (gnode_list *) ptroots->DATA;
            displayNodeLog(lib, loglevel, ptnode);
        }
    }
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
    }
}

static void 
displayNode(gnode_list *ptnode)
{
    chain_list     *ptchain;
    gnode_list     *ptinnode;

    if (ptnode->VISITED)
        return;
    ptnode->VISITED = TRUE;

    if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
        printf( "node %ld (%s) %lx of depth %d has %d sons\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME, ptnode->TYPE, ptnode->DEPTH, ptnode->NUMSONS);
    }
    else {
        printf( "node X (%s) %lx of depth %d has %d sons\n", ptnode->OBJECT.LOCON->NAME, ptnode->TYPE, ptnode->DEPTH, ptnode->NUMSONS);
    }
    if (ptnode->FATHERS != NULL/* && (ptnode->TYPE & PRIMNODE) != PRIMNODE*/) {
        printf( "depends upon :-\n");
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            ptinnode = (gnode_list *) ptchain->DATA;
            if ((ptinnode->TYPE & CONE_TYPE) == CONE_TYPE) {
                printf( "\t%ld (%s)\n", ptinnode->OBJECT.CONE->INDEX, ptinnode->OBJECT.CONE->NAME);
            }
            else {
                printf( "\tX (%s)\n", ptinnode->OBJECT.LOCON->NAME);
            }
        }
        printf( "\n");

        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            ptinnode = (gnode_list *) ptchain->DATA;
            displayNode(ptinnode);
        }
    }
    else {
        printf( "\n");
    }
}

static void 
displayNodeLog(int lib, int loglevel, gnode_list *ptnode)
{
    chain_list     *ptchain;
    gnode_list     *ptinnode;

    if (ptnode->VISITED)
        return;
    ptnode->VISITED = TRUE;

    if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
        avt_log(lib, loglevel, "node %ld (%s) %lx of depth %d has %d sons\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME, ptnode->TYPE, ptnode->DEPTH, ptnode->NUMSONS);
    }
    else {
        avt_log(lib, loglevel, "node X (%s) %lx of depth %d has %d sons\n", ptnode->OBJECT.LOCON->NAME, ptnode->TYPE, ptnode->DEPTH, ptnode->NUMSONS);
    }
    if (ptnode->FATHERS != NULL/* && (ptnode->TYPE & PRIMNODE) != PRIMNODE*/) {
        avt_log(lib, loglevel, "depends upon :-\n");
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            ptinnode = (gnode_list *) ptchain->DATA;
            if ((ptinnode->TYPE & CONE_TYPE) == CONE_TYPE) {
                avt_log(lib, loglevel, "\t%ld (%s)\n", ptinnode->OBJECT.CONE->INDEX, ptinnode->OBJECT.CONE->NAME);
            }
            else {
                avt_log(lib, loglevel, "\tX (%s)\n", ptinnode->OBJECT.LOCON->NAME);
            }
        }
        avt_log(lib, loglevel, "\n");

        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            ptinnode = (gnode_list *) ptchain->DATA;
            displayNodeLog(lib, loglevel, ptinnode);
        }
    }
    else {
        avt_log(lib, loglevel, "\n");
    }
}

static void 
unmark(gnode_list *ptnode)
{
    chain_list     *ptchain;

    ptnode->VISITED = FALSE;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        unmark((gnode_list *) ptchain->DATA);
    }
}

int 
yagTestGraphCycle(graph *ptgraph)
{
    ptype_list     *ptroots;
    gnode_list     *ptnode;
    int             loop = FALSE;

    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        VISIT_LIST = addchain(NULL, ptnode);
        if (testCycle(ptnode))
            loop = TRUE;
        freechain(VISIT_LIST);
    }
    return loop;
}

static int 
testCycle(gnode_list *ptnode)
{
    gnode_list     *ptfather;
    chain_list     *ptchain;
    int             loop = FALSE;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        ptfather = (gnode_list *) ptchain->DATA;
        if (yagGetChain(VISIT_LIST, ptfather) != NULL) {
            dumpLoop(VISIT_LIST);
            loop = TRUE;
        }
        else {
            VISIT_LIST = addchain(VISIT_LIST, ptfather);
            if (testCycle(ptfather))
                loop = TRUE;
            VISIT_LIST = delchain(VISIT_LIST, VISIT_LIST);
        }
    }
    return loop;
}

static void 
dumpLoop(chain_list *ptlist)
{
    chain_list     *ptchain;
    gnode_list     *ptnode;

    printf("\nLoop at :-\n");
    for (ptchain = ptlist; ptchain; ptchain = ptchain->NEXT) {
        ptnode = (gnode_list *) ptchain->DATA;
        if ((ptnode->TYPE & CONE_TYPE) == CONE_TYPE) {
            printf("\tnode %ld (%s)\n", ptnode->OBJECT.CONE->INDEX, ptnode->OBJECT.CONE->NAME);
        }
        else {
            printf("\tnode X (%s)\n", ptnode->OBJECT.LOCON->NAME);
        }
    }
    printf("\n");
    fflush(stdout);
}

void 
yagCountDualTrans(cnsfig_list *ptcnsfig)
{
    lotrs_list     *pttrans;
    int             numused = 0;
    int             numtrans = 0;
    int             numorient = 0;

    for (pttrans = ptcnsfig->LOTRS; pttrans != NULL; pttrans = pttrans->NEXT) {
        numtrans++;
        if ((pttrans->TYPE & USED) != 0)
            numused++;
        if ((pttrans->TYPE & ORIENTED) != 0)
            numorient++;
    }

    avt_log(LOGYAG,1, "Dual cones use %d out of %d transistors\n", numused, numtrans);
    if (YAG_CONTEXT->YAG_ORIENT) {
        avt_log(LOGYAG,1, "%d transistors have been oriented\n", numorient);
    }
}

int 
yagCountCones(cone_list *headcone)
{
    int             count = 0;
    cone_list      *ptcone;

    for (ptcone = headcone; ptcone; ptcone = ptcone->NEXT)
        count++;
    return count;
}

chain_list *
yagCircuitLoops(cnsfig_list *ptcnsfig)
{
    cone_list      *ptcone;
    cone_list      *ptincone;
    cone_list      *ptsummitcone;
    cone_list      *ptauxsummit;
    cone_list      *ptbackcone;
    cone_list      *ptsavecone;
    edge_list      *ptin;
    locon_list     *ptlocon;
    chain_list     *summitchain = NULL;
    chain_list     *ptchain;
    chain_list     *list_loop = NULL;
    chain_list     *loop = NULL;
    ptype_list     *ptuser;
    int             numloops = 0;

    for (ptlocon = ptcnsfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
        if((ptlocon->DIRECTION == 'O') || (ptlocon->DIRECTION == 'B')
        || (ptlocon->DIRECTION == 'Z') || (ptlocon->DIRECTION == 'T')) {
            ptuser = getptype(ptlocon->USER,CNS_EXT);
            if (ptuser != NULL) {
                ptcone = (cone_list *)ptuser->DATA;
                summitchain = addchain(summitchain, ptcone);
            }
        }
    }

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        ptcone->USER = addptype(ptcone->USER, YAG_BACK_PTYPE, (long) 0);
        ptcone->USER = addptype(ptcone->USER, YAG_INCONE_PTYPE, ptcone->INCONE);

        if ((ptcone->TYPE & (CNS_LATCH|CNS_MEMSYM|CNS_FLIP_FLOP)) != 0) {
            summitchain = addchain(summitchain, ptcone);
            for (ptin = ptcone->INCONE; ptin; ptin = ptin->NEXT) {
                if ((ptin->TYPE & CNS_COMMAND) == CNS_COMMAND) {
                    if ((ptcone->TYPE & YAG_VISITED) != YAG_VISITED) {
                        ptincone = ptin->UEDGE.CONE;
                        ptincone->TYPE |= YAG_VISITED;
                        summitchain = addchain(summitchain, ptincone);
                    }
                }
            }
        }
    }

    for (ptchain = summitchain; ptchain; ptchain = ptchain->NEXT) {
        ptsummitcone = (cone_list *)summitchain->DATA;

        while (((getptype(ptsummitcone->USER, YAG_BACK_PTYPE)->DATA) != NULL)
        || (ptsummitcone->INCONE != NULL)) {

            if ((ptsummitcone->INCONE != NULL)
            && (((ptsummitcone->INCONE->UEDGE.CONE)->TYPE & CNS_LATCH) == 0)
            && (((ptsummitcone->INCONE->UEDGE.CONE)->TYPE & CNS_FLIP_FLOP) == 0)
            && (((ptsummitcone->INCONE->UEDGE.CONE)->TYPE & CNS_MEMSYM) == 0)
            && ((ptsummitcone->INCONE->TYPE & (CNS_EXT | CNS_FEEDBACK | CNS_VDD | CNS_VSS
            | CNS_COMMAND  | CNS_BLEEDER)) == 0)) {
                ptauxsummit = ptsummitcone->INCONE->UEDGE.CONE;

                if (((ptauxsummit->TYPE & YAG_MARK) == YAG_MARK)
                && (ptauxsummit->INCONE != NULL)) {
                    numloops++;

                    for (ptcone = ptsummitcone; ptcone != ptauxsummit; ptcone = ptbackcone) {
                        ptbackcone = (cone_list *)(getptype(ptcone->USER, YAG_BACK_PTYPE)->DATA);
                        loop = addchain(loop, (char *)ptcone);
                    }
                    loop = addchain(loop, (char *)ptauxsummit);
                }

                if ((numloops != 0) && (loop != NULL)) {
                    list_loop = addchain(list_loop, (char *)loop);
                    loop = NULL;
                    ptsummitcone->INCONE->TYPE |= (CNS_FEEDBACK);
                    ptsummitcone->INCONE = ptsummitcone->INCONE->NEXT;
                    ptauxsummit = ptsummitcone;
                }
                else
                    (getptype(ptauxsummit->USER, YAG_BACK_PTYPE)->DATA) = ptsummitcone;

                ptauxsummit->TYPE |= YAG_MARK;
                ptsummitcone = ptauxsummit;
            }

            else {
                if (ptsummitcone->INCONE == NULL) {
                    ptsavecone = ptsummitcone;

                    ptsummitcone = (getptype(ptsummitcone->USER, YAG_BACK_PTYPE)->DATA);
                    (getptype(ptsavecone->USER, YAG_BACK_PTYPE)->DATA) = 0;
                }

                if (ptsummitcone->INCONE != NULL)
                    ptsummitcone->INCONE = ptsummitcone->INCONE->NEXT;
            }
        }
    }

    freechain(summitchain);

    for (ptcone = ptcnsfig->CONE; ptcone != NULL; ptcone = ptcone->NEXT) {
        ptcone->TYPE &= ~YAG_VISITED;
        ptcone->USER = delptype(ptcone->USER, YAG_BACK_PTYPE);
        ptcone->INCONE = (edge_list *)getptype(ptcone->USER, YAG_INCONE_PTYPE)->DATA;
        ptcone->USER = delptype(ptcone->USER, YAG_INCONE_PTYPE);
    }

    return list_loop;
}

void
yagSaveCircuitLoops(chain_list *list_loop)
{
    cone_list     *ptcone;
    chain_list    *ptchain0;
    chain_list    *ptchain1;
    long           numloops, i = 0;

    numloops = yagCountChains(list_loop);

    if (YAG_CONTEXT->YAGLE_LANG == 'E') {
        if(numloops == 1) fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"one loop was detected :\n") ;
        else fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"%ld loops were detected :\n\n",numloops) ;
    }
    else if(YAG_CONTEXT->YAGLE_LANG == 'F') {
        if(numloops == 1) fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"une boucle a ete detectee :\n") ;
        else fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"%ld boucles ont ete detectees :\n",numloops) ;
    }

    for (ptchain0 = list_loop; ptchain0; ptchain0 = ptchain0->NEXT) {

        if(YAG_CONTEXT->YAGLE_LANG == 'E')
        fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"loop no %ld :\n",i) ;
        else if(YAG_CONTEXT->YAGLE_LANG == 'F')
        fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"boucle no %ld :\n",i) ;
        i++ ;

        for (ptchain1 = (chain_list *)ptchain0->DATA; ptchain1; ptchain1 = ptchain1->NEXT) {
            ptcone = (cone_list *)ptchain1->DATA;
            fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"%ld (%s)\n", ptcone->INDEX, ptcone->NAME) ;
        }

        fprintf(YAG_CONTEXT->YAGLE_LOOP_FILE,"\n") ;
        fflush(YAG_CONTEXT->YAGLE_LOOP_FILE);
        freechain((chain_list*)ptchain0->DATA) ;
    }

    freechain(list_loop);
}

void
yagStatLofig(lofig_list *lofig)
{
    int n_r           = 0, cnt ;
    int n_c           = 0 ;
    int n_losig       = 0 ;
    int n_losigwithrc = 0 ;
    int n_pmos        = 0 ;
    int n_nmos        = 0 ;
    int n_omos        = 0 ;
    int n_diode       = 0 ;
    char flag ;
    chain_list *cl;
    mbk_parse_error *mpr;
    
    losig_list  *losig ;
    lowire_list *lowire ;
    chain_list  *chainctc ;
    loctc_list  *loctc ;
    lotrs_list  *lotrs ;

    if( !YAG_CONTEXT->YAGLE_STAT_FILE || !lofig ) return ;
    
    for ( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
        n_losig++ ;
    
        if( losig->PRCN ) {
       
            flag = 0 ;
            
            for( lowire = losig->PRCN->PWIRE ; lowire ; lowire = lowire->NEXT ) {
                n_r++ ;
                flag = 1 ;
            }

            for( chainctc = losig->PRCN->PCTC ; chainctc ; chainctc = chainctc->NEXT ) {
        
                loctc = (loctc_list*)chainctc->DATA ;
            
                if( loctc->SIG1 == losig ) {

                    if( (cnt=rcn_isCapaDiode( loctc ))!=0 ) {
                        n_diode+=cnt ;
                    }
                    else {
                        n_c++ ;
                        flag = 1 ;
                    }
                }
            }

            if( flag ) {
                n_losigwithrc++ ;
            }
        }  
    }

    for( lotrs = lofig->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) {
        flag = 0 ;
        
        if( MLO_IS_TRANSN( lotrs->TYPE ) ) {
            n_nmos++ ;
            flag=1 ;
        }

        if( MLO_IS_TRANSP( lotrs->TYPE ) ) {
            n_pmos++ ;
            flag = 1 ;
        }

        if( flag==0 ) {
            n_omos++ ;
        }
    }

    
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "Number of devices for circuit %s\n", lofig->NAME );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE,     "net                        : %6d\n", n_losig );
    if( n_losigwithrc > 0 )
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "  net with RC description  : %6d\n", n_losigwithrc );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE,     "R devices                  : %6d\n", n_r );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE,     "C devices                  : %6d\n", n_c );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE,     "NMOS devices               : %6d\n", n_nmos );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE,     "PMOS devices               : %6d\n", n_pmos );
    if( n_omos > 0 )
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "other MOS devices          : %6d\n", n_omos );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE,     "diode devices              : %6d\n", n_diode );
    fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "\n" );

    for (cl=MBK_ALL_PARSE_ERROR; cl!=NULL; cl=cl->NEXT)
    {
      mpr=(mbk_parse_error *)cl->DATA;
      if (mpr->NB_LOTRS+mpr->NB_RESI+mpr->NB_CAPA+mpr->NB_LOTRS+mpr->NB_NET+mpr->NB_INSTANCE+mpr->NB_DIODE!=0)
      {
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "Devices ignored during parse of %s\n", mpr->filename);
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "R devices                  : %6lu\n", mpr->NB_RESI  );
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "C devices                  : %6lu\n", mpr->NB_CAPA  );
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "Diode devices              : %6lu\n", mpr->NB_DIODE );
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "Nets                       : %6lu\n", mpr->NB_NET   );
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "Instances                  : %6lu\n", mpr->NB_INSTANCE );
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "Transistors                : %6lu\n", mpr->NB_LOTRS );
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "\n" );
      } 
      else
      {
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "No devices ignored during parse of %s\n", mpr->filename);
        fprintf( YAG_CONTEXT->YAGLE_STAT_FILE, "\n" );
      }
    }
    
    mbk_reset_errors();
}

void
yagDisplayBddExprLog(int lib, int loglevel, char *message, pCircuit theCct, pNode bdd)
{
    chain_list *tempabl;

    avt_log(lib,loglevel,"%s : ", message);
    tempabl = bddToAblCct(theCct, bdd);
    displayInfExprLog(lib, loglevel, tempabl);
    freeExpr(tempabl);
    avt_log(lib, loglevel,"\n");
}
