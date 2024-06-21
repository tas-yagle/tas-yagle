/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_graph.c                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                        le : 20/06/1994     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include "yag_headers.h"

#define GR_NOERR 0
#define GR_LOOP 1
#define GR_HOLE 2
#define GR_STOP 4

#define GR_PRIMVAR 0
#define GR_NOPRIMVAR 1

#define GRAPH_PACKET 4
#define NODE_PACKET 20

static graph        *EMPTY_GRAPH = NULL;
static graph        *ALLOC_GRAPH = NULL;

static gnode_list   *EMPTY_NODE = NULL;

static graph    *CUR_GRAPH;
static short    CUR_TREE;
static cone_list *CUR_ROOT;

static chain_list   *VAR_LIST;

static int      local;
//static int      incomplete;
static int      numholes;

static int      total_son_arcs;
static int      visited_son_arcs;

static int      add_constraints;

#define convergence(node) (node->NUMSONS >= 2 || (node->NUMSONS == 1 && node->DEPTH == 0))
#define mark_treebit(node) (node->TREEBIT |= (1 << CUR_TREE))
#define unmark_treebit(node) (node->TREEBIT ^= (1 << CUR_TREE))
#define in_other_tree(node) (((node->TREEBIT & ~(1 << CUR_TREE)) != 0) ? TRUE : FALSE)
#define check_complete(ptgraph) (ptgraph->ROOTNODES == NULL ? TRUE : (ptgraph->ROOTNODES->TYPE & HOLENODE) == 0)

static gnode_list *build(edge_list *ptedge, int depth, int *ptrescode, int invonly, int force);
static void update_depth(gnode_list *ptnode, int depth);
static void check_memsym_constraint(cone_list *ptcone, graph *ptgraph);
static gnode_list *add_node(void *object, long type, short deep);
static void extra_constraints(gnode_list *ptnode, graph *ptgraph);
static void number_sons(gnode_list *ptnode);
static void traverse(gnode_list *ptnode);
static void unmark(gnode_list *ptnode);
static void count_sons(gnode_list *ptnode);
static void pick_prims(gnode_list *ptnode);
static chain_list *add_primvar(chain_list *list, gnode_list *ptnode);
static void reduce(graph *ptgraph, gnode_list *ptnode);
static void prop_search(gnode_list *ptnode);

/****************************************************************************
 *                         function yagBuildGraph();                        *
 ****************************************************************************/

graph *
yagBuildGraph(edge_list *ptedgelist, cone_list *rootcone, int force)
{
    edge_list   *ptedge;
    gnode_list  *ptnode;
    locon_list  *ptcon;
    cone_list   *ptcone;
    int         rescode;
    int         stopcone;
    int         save_hasdual;

    if ((rootcone && rootcone->TYPE & YAG_HASDUAL) != 0) save_hasdual = TRUE;
    else save_hasdual = FALSE;
    if (rootcone) rootcone->TYPE &= ~YAG_HASDUAL;
    
    CUR_GRAPH = yagNewGraph();
    CUR_ROOT = rootcone;

    local = FALSE;
    if (rootcone && (rootcone->TYPE & YAG_STOP) == YAG_STOP) stopcone = TRUE;
    else stopcone = FALSE;
    
    /* create root nodes separately in order to */
    /* detect reconvergence onto a root         */
    
    for (ptedge = ptedgelist; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        CUR_TREE = CUR_GRAPH->WIDTH;
        if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
            ptcon = ptedge->UEDGE.LOCON;
            ptnode = add_node(ptcon, ROOT_NODE|EXT, 0);
            if (getptype(ptcon->USER, YAG_CONSTRAINT_PTYPE) != NULL) {
                ptnode->TYPE |= CONSTRAINT_NODE;
            }
        }
        else {
            ptcone = ptedge->UEDGE.CONE;
            ptnode = add_node(ptcone, ROOT_NODE|CONE_TYPE, 0);
            if ((ptcone->TYPE & YAG_CONSTRAINT) == YAG_CONSTRAINT) {
                ptnode->TYPE |= CONSTRAINT_NODE;
            }
            if ((ptcone->TYPE & CNS_MEMSYM) != 0) check_memsym_constraint(ptcone, CUR_GRAPH);
        }
        CUR_GRAPH->ROOTNODES = addptype(CUR_GRAPH->ROOTNODES, 0, ptnode);
        CUR_GRAPH->WIDTH++;
    }
    
    for (ptedge = ptedgelist; ptedge != NULL; ptedge = ptedge->NEXT) {
        if ((ptedge->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
        CUR_TREE = CUR_GRAPH->WIDTH;
        ptnode = build(ptedge, 0, &rescode, TRUE, force);
    }
    if (rootcone && save_hasdual) rootcone->TYPE |= YAG_HASDUAL;
    return CUR_GRAPH;
}

/****************************************************************************
 *                         function yagRootGraph();                           *
 ****************************************************************************/

gnode_list *
yagRootGraph(graph *ptgraph, cone_list *ptcone)
{
    gnode_list *ptnode;
    ptype_list *ptlist;

    CUR_GRAPH = ptgraph;
    ptnode = add_node(ptcone, CONE_TYPE|ROOT_NODE, 0);

    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        ptnode->FATHERS = addchain(ptnode->FATHERS, ptlist->DATA);
        ((gnode_list *)ptlist->DATA)->TYPE &= ~ROOT_NODE;
    }
    freeptype(ptgraph->ROOTNODES);
    ptgraph->ROOTNODES = addptype(NULL, 0, ptnode);

    return ptnode;
}

/****************************************************************************
 *                         function build();                                *
 ****************************************************************************/

static gnode_list *
build(edge_list  *ptedge, int depth, int *ptrescode, int invonly, int force)
{
    cone_list  *ptcone;
    locon_list *ptcon;
    gnode_list *ptnode;
    gnode_list *ptinnode;
    edge_list  *input;
    edge_list  *inputlist;
    long        nodetype;
    int         tostop;
    int         stop = FALSE;
    int         hole = FALSE;
    int         addfathers;
    
    *ptrescode = GR_NOERR;

    /* connector node */
    if ((ptedge->TYPE & CNS_EXT) == CNS_EXT) {
        ptcon = ptedge->UEDGE.LOCON; 
        ptnode = (gnode_list *)gethtitem(CUR_GRAPH->HASHTAB, ptcon);
        if (ptnode != (gnode_list *)EMPTYHT) {
            if (depth > ptnode->DEPTH) ptnode->DEPTH = depth;
        }
        else {
            ptnode = add_node(ptcon, EXT, depth);
            /* check for a constraint */
            if (getptype(ptcon->USER, YAG_CONSTRAINT_PTYPE) != NULL) {
                ptnode->TYPE |= CONSTRAINT_NODE;
            }
        }
        stop = TRUE;
    }
    else {
    /* cone node */

        ptcone = ptedge->UEDGE.CONE;

        /* loop handling */

        if ((ptcone->TYPE & YAG_VISITED) != 0) {
            *ptrescode = GR_LOOP;
            return NULL;
        }
        ptcone->TYPE |= YAG_VISITED;
        
        nodetype = CONE_TYPE;
        if ((ptcone->TYPE & CNS_POWER) == CNS_POWER) nodetype |= CONSTBIT;
        if ((ptcone->TYPE & YAG_FORCEPRIM) == YAG_FORCEPRIM) nodetype |= FORCEPRIM_NODE;
        
        /* check depth limit */
        tostop = FALSE;
        if (!invonly) {
            if (depth >= YAG_CONTEXT->YAG_DEPTH * 2) tostop = TRUE;
        }
        else {
            if (depth >= (YAG_CONTEXT->YAG_DEPTH<9?9:YAG_CONTEXT->YAG_DEPTH) * 2) tostop = TRUE;
        }
            
        /* set depth to zero if reconverging */
        /* onto a rootnode                   */
        ptnode = (gnode_list *)gethtitem(CUR_GRAPH->HASHTAB, ptcone);
        if (ptnode != (gnode_list *)EMPTYHT) {
            if (ptnode->DEPTH == 0) depth = 0;
        }

        /* check stop conditions */
        if (!force || depth != 0) {
            tostop = ((ptcone->TYPE & (CNS_POWER|CNS_LATCH|CNS_MEMSYM|CNS_RS|YAG_CONSTRAINT|YAG_STOP)) != 0);    
            tostop = (tostop || ((ptcone->TYPE & CNS_CONFLICT) != 0 && (ptcone->TYPE & YAG_HASDUAL) == 0 && (ptcone->TECTYPE & YAG_RESCONF) == 0));
            if ((ptcone->TYPE & CNS_EXT) != 0) tostop = (tostop || !yagIsOutput(ptcone));
            if (!YAG_CONTEXT->YAG_PROP_HZ) tostop = (tostop || ((ptcone->TYPE & CNS_TRI) != 0));

        }
            
        /* check for hole for graph completeness */
        hole = !tostop && ((ptcone->TYPE & YAG_PARTIAL) != 0 && (ptcone->TECTYPE & CNS_DUAL_CMOS) == 0);
            
        if ((!tostop && !hole) || (force && depth == 0)) {
    
            if (ptnode != (gnode_list *)EMPTYHT) {
 
                /* return if reconverging onto a loop cut */
                if ((ptnode->TYPE & LOOP_NODE) != 0) {
                    ptcone->TYPE &= ~YAG_VISITED;
                    return ptnode;
                }

                /* return if reconverging onto */
                /* a node of greater depth     */ 
                if (depth > 0) {
                    if (ptnode->DEPTH >= depth) {
                        ptcone->TYPE &= ~YAG_VISITED;
                        return ptnode;
                    }
                    else update_depth(ptnode, depth);
                }
            }
            else ptnode = add_node(ptcone, nodetype, depth);

            addfathers = (ptnode->FATHERS == NULL);
                    
            stop = TRUE;

            //if ((ptcone->TYPE & CNS_CONFLICT) != 0 && (ptcone->TYPE & YAG_HASDUAL) != 0) {
            if ((ptcone->TYPE & YAG_HASDUAL) != 0 && (!force || depth != 0)) {
                inputlist = (edge_list *)getptype(ptcone->USER, YAG_DUALINPUTS_PTYPE)->DATA;
            }
            else inputlist = ptcone->INCONE;

            if (!(inputlist && (inputlist->NEXT == NULL))) invonly=0;

            for (input = inputlist; input != NULL; input = input->NEXT) {
                
                if ((input->TYPE & CNS_BLEEDER) == CNS_BLEEDER) continue;
                ptinnode = build(input, depth+1, ptrescode, invonly, force);
                if (ptinnode != NULL) {
                    if (addfathers) {
                        ptnode->FATHERS = addchain(ptnode->FATHERS, ptinnode);
                    }
                }
                else if  (*ptrescode == GR_LOOP) {
                    if (!force || depth != 0) {
                        if (ptnode->FATHERS != NULL) {
                            freechain(ptnode->FATHERS);
                            ptnode->FATHERS = NULL;
                        }
                        ptnode->TYPE |= LOOP_NODE;
                        stop = TRUE;
                        break;
                    }
                }
                if (((*ptrescode) & GR_STOP) == 0) stop = FALSE;
            }
        }
        else {
            ptnode = (gnode_list *)gethtitem(CUR_GRAPH->HASHTAB, ptcone);
            if (ptnode != (gnode_list *)EMPTYHT) {
                if (ptnode->DEPTH != 0) {
                    if (depth > ptnode->DEPTH) ptnode->DEPTH = depth;
                }
            }
            else {
                ptnode = add_node((void *)ptcone, nodetype, depth);
            }
            if (tostop || hole) stop = TRUE;

            /* check for a contraint */
            if ((ptcone->TYPE & YAG_CONSTRAINT) == YAG_CONSTRAINT) {
                ptnode->TYPE |= CONSTRAINT_NODE;
            }
            if ((ptcone->TYPE & CNS_MEMSYM) != 0) check_memsym_constraint(ptcone, CUR_GRAPH);
        }
        ptcone->TYPE &= ~YAG_VISITED;
        if (ptcone == CUR_ROOT) hole = FALSE;
    }

    if (hole && !tostop) {
        ptnode->TYPE |= HOLENODE;
    }
    if (stop) {
        ptnode->TYPE |= STOPNODE;
        *ptrescode |= GR_STOP;
    }
    
    return ptnode;
}

static void
update_depth(gnode_list *ptnode, int depth)
{
    chain_list  *ptchain;

    /* reconverging onto a root */
    if (ptnode->DEPTH == 0) return;

    if (depth > ptnode->DEPTH) {
        ptnode->DEPTH = depth;

        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            update_depth((gnode_list *)ptchain->DATA, depth+1);
        }
    }
}

static void
check_memsym_constraint(cone_list *ptcone, graph *ptgraph)
{
    cone_list *ptloopcone;
    gnode_list *ptnode, *ptloopnode;
    ptype_list *ptuser;
    
    if ((ptcone->TYPE & CNS_MEMSYM) != CNS_MEMSYM) return;
    if ((ptnode = (gnode_list *)gethtitem(CUR_GRAPH->HASHTAB, ptcone)) == (gnode_list *)EMPTYHT) return;
    if ((ptuser = getptype(ptcone->USER, YAG_MEMORY_PTYPE)) != NULL) {
        ptloopcone = (cone_list *)ptuser->DATA;
        if ((ptloopnode = (gnode_list *)gethtitem(CUR_GRAPH->HASHTAB, ptloopcone)) != (gnode_list *)EMPTYHT) {
            ptnode->TYPE |= MEMSYM_NODE;
            ptloopnode->TYPE |= MEMSYM_NODE;
        }
    }
}

/****************************************************************************
 *                         function add_node();                             *
 ****************************************************************************/

static gnode_list *
add_node(void *object, long type, short deep)
{
    gnode_list *ptnode;
    int         i;
    char       *name;
    
    if (EMPTY_NODE == NULL) {
        ptnode = (gnode_list *)mbkalloc(NODE_PACKET * sizeof(gnode_list));
        EMPTY_NODE = ptnode;
        for (i=1; i<NODE_PACKET; i++) {
            ptnode->NEXT = ptnode + 1;
            ptnode += 1;
        }
        ptnode->NEXT = NULL;
    }
    ptnode = EMPTY_NODE;
    EMPTY_NODE = EMPTY_NODE->NEXT;
    ptnode->NEXT = CUR_GRAPH->HEADNODE;
    CUR_GRAPH->HEADNODE = ptnode;
    
    ptnode->OBJECT.PTR = object;
    ptnode->VISITED = FALSE;
    ptnode->TYPE = type;
    ptnode->DEPTH = deep;
    if (local) ptnode->TREEBIT = 1 << CUR_TREE;
    else ptnode->TREEBIT = 0;
    ptnode->NUMSONS = 0;
    ptnode->SONS = NULL;
    ptnode->FATHERS = NULL;
    
    addhtitem(CUR_GRAPH->HASHTAB, object, (long)ptnode);
    
    if ((type & CONE_TYPE) == CONE_TYPE) name = ((cone_list *)object)->NAME;
    else name = ((locon_list *)object)->NAME;
    addhtitem(CUR_GRAPH->NAMEHASHTAB, name, (long)ptnode);

    return ptnode;
}

/****************************************************************************
 *                         function yagNewGraph();                            *
 ****************************************************************************/

graph *
yagNewGraph()
{
    graph  *ptgraph;
    int     i;

    if (EMPTY_GRAPH == NULL) {
        ptgraph = (graph *)mbkalloc(GRAPH_PACKET * sizeof(graph));
        EMPTY_GRAPH = ptgraph;
        for (i=1; i<GRAPH_PACKET; i++) {
            ptgraph->NEXT = ptgraph + 1;
            ptgraph += 1;
        }
        ptgraph->NEXT = NULL;
    }
    ptgraph = EMPTY_GRAPH;
    EMPTY_GRAPH = EMPTY_GRAPH->NEXT;
    ptgraph->NEXT = ALLOC_GRAPH;
    ALLOC_GRAPH = ptgraph;

    ptgraph->ROOTNODES = NULL;
    ptgraph->PRIMVARS = NULL;
    ptgraph->HEADNODE = NULL;
    ptgraph->HASHTAB = addht(40);
    ptgraph->NAMEHASHTAB = addht(40);
    ptgraph->CONSTRAINTS = NULL;
    ptgraph->EXTRA_CONSTRAINTS = NULL;
    ptgraph->COMPLETE = FALSE;
    ptgraph->WIDTH = 0;
    
    return ptgraph;
}

/****************************************************************************
 *                         function yagFreeGraph();                           *
 ****************************************************************************/

void
yagFreeGraph(graph *ptgraph)
{
    gnode_list  *ptnode, *ptlastnode = NULL;
    ptype_list  *ptptype;

    if (ptgraph == NULL) yagBug(DBG_NULL_PTR, "yagFreeGraph", NULL, NULL, 0);
    
    delht(ptgraph->HASHTAB);
    delht(ptgraph->NAMEHASHTAB);
    
    for (ptnode = ptgraph->HEADNODE; ptnode != NULL; ptnode = ptnode->NEXT) {
        freechain(ptnode->FATHERS);
        freechain(ptnode->SONS);
        ptlastnode = ptnode;
    }
    if (ptlastnode != NULL) {
        ptlastnode->NEXT = EMPTY_NODE;
        EMPTY_NODE = ptgraph->HEADNODE;
        freeptype(ptgraph->ROOTNODES);
        freechain(ptgraph->PRIMVARS);
        freeptype(ptgraph->CONSTRAINTS);
        for (ptptype = ptgraph->EXTRA_CONSTRAINTS; ptptype; ptptype = ptptype->NEXT) {
            freechain((chain_list *)ptptype->TYPE);
            freeExpr((chain_list *)ptptype->DATA);
        }
        freeptype(ptgraph->EXTRA_CONSTRAINTS);
    }

    ALLOC_GRAPH = ptgraph->NEXT;
    ptgraph->NEXT = EMPTY_GRAPH;
    EMPTY_GRAPH = ptgraph;
}

/****************************************************************************
 *                         function yagTraverseGraph();                       *
 ****************************************************************************/

void
yagAddExtraConstraints(graph *ptgraph)
{
    ptype_list  *ptlist;

    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        extra_constraints((gnode_list *)ptlist->DATA, ptgraph);
    }
    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        unmark((gnode_list *)ptlist->DATA);
    }
}

static void
extra_constraints(gnode_list *ptnode, graph *ptgraph)
{
    chain_list  *ptchain;
    ptype_list  *ptuser;
    cone_list   *ptcone, *ptinvcone;
    gnode_list  *ptinvnode;
    chain_list  *support;
    chain_list  *tempexpr;

    if (ptnode->VISITED) return;
    ptnode->VISITED = TRUE;
    
    if ((ptnode->TYPE & EXT) == 0) {
        ptcone = ptnode->OBJECT.CONE;
        if ((ptuser = getptype(ptcone->USER, CNS_INVCONE)) != NULL) {
            for (ptchain = (chain_list *)ptuser->DATA; ptchain; ptchain = ptchain->NEXT) {
                ptinvcone = (cone_list *)ptchain->DATA;
                support = addchain(NULL, ptcone->NAME);
                support = addchain(support, ptinvcone->NAME);
                tempexpr = createExpr(OR);
                addQExpr(tempexpr, createAtom(ptcone->NAME));
                addQExpr(tempexpr, createAtom(ptinvcone->NAME));
                ptgraph->CONSTRAINTS = addptype(ptgraph->CONSTRAINTS, (long)support, (void *)tempexpr);
                ptgraph->EXTRA_CONSTRAINTS = addptype(ptgraph->EXTRA_CONSTRAINTS, (long)support, (void *)tempexpr);
                tempexpr = createExpr(AND);
                addQExpr(tempexpr, createAtom(ptcone->NAME));
                addQExpr(tempexpr, createAtom(ptinvcone->NAME));
                tempexpr = notExpr(tempexpr);
                ptgraph->CONSTRAINTS = addptype(ptgraph->CONSTRAINTS, (long)support, (void *)tempexpr);
                ptgraph->EXTRA_CONSTRAINTS = addptype(ptgraph->EXTRA_CONSTRAINTS, (long)yagCopyChainList(support), (void *)tempexpr);
            }
        }
        if (((ptnode->TYPE & MEMSYM_NODE) == MEMSYM_NODE) && (ptuser = getptype(ptcone->USER, YAG_MEMORY_PTYPE)) != NULL) {
            ptinvcone = (cone_list *)ptuser->DATA;
            ptinvnode = (gnode_list *)gethtitem(CUR_GRAPH->HASHTAB, ptinvcone);
            if (ptinvnode != (gnode_list *)EMPTYHT) {
                ptinvnode->VISITED = TRUE;
                support = addchain(NULL, ptcone->NAME);
                support = addchain(support, ptinvcone->NAME);
                tempexpr = createExpr(OR);
                addQExpr(tempexpr, createAtom(ptcone->NAME));
                addQExpr(tempexpr, createAtom(ptinvcone->NAME));
                ptgraph->CONSTRAINTS = addptype(ptgraph->CONSTRAINTS, (long)support, (void *)tempexpr);
                ptgraph->EXTRA_CONSTRAINTS = addptype(ptgraph->EXTRA_CONSTRAINTS, (long)support, (void *)tempexpr);
                tempexpr = createExpr(AND);
                addQExpr(tempexpr, createAtom(ptcone->NAME));
                addQExpr(tempexpr, createAtom(ptinvcone->NAME));
                tempexpr = notExpr(tempexpr);
                ptgraph->CONSTRAINTS = addptype(ptgraph->CONSTRAINTS, (long)support, (void *)tempexpr);
                ptgraph->EXTRA_CONSTRAINTS = addptype(ptgraph->EXTRA_CONSTRAINTS, (long)yagCopyChainList(support), (void *)tempexpr);
            }
        }
    }
    if ((ptnode->TYPE & PRIMNODE) == 0) {
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            extra_constraints((gnode_list *)ptchain->DATA, ptgraph);
        }
    }
}
 
/****************************************************************************
 *                         function yagTraverseGraph();                       *
 ****************************************************************************/

void
yagTraverseGraph(graph *ptgraph)
{
    ptype_list  *ptlist;

    numholes = 0;
    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        number_sons((gnode_list *)ptlist->DATA);
    }
    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        unmark((gnode_list *)ptlist->DATA);
    }

    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        traverse((gnode_list *)ptlist->DATA);
    }

    VAR_LIST = NULL;
    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        pick_prims((gnode_list *)ptlist->DATA);
    }
    for (ptlist = ptgraph->ROOTNODES; ptlist; ptlist = ptlist->NEXT) {
        unmark((gnode_list *)ptlist->DATA);
    }
    ptgraph->COMPLETE = (numholes < 1);
    ptgraph->PRIMVARS = VAR_LIST;
}

static void
number_sons(gnode_list *ptnode)
{
    chain_list  *ptchain;
    gnode_list  *ptfather;
    int          outdepth = FALSE;
    int          forceprim = FALSE;

    ptnode->NUMSONS++;
    if (ptnode->VISITED) return;
    ptnode->VISITED = TRUE;

    if (ptnode->DEPTH >= YAG_CONTEXT->YAG_DEPTH) {
        if (ptnode->FATHERS != NULL) {
            freechain(ptnode->FATHERS);
            ptnode->FATHERS = NULL;
        }
        return;
    }

    if ((ptnode->TYPE & HOLENODE) != 0) {
        numholes++;
    }

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        ptfather = (gnode_list *)ptchain->DATA;
        if (ptfather->DEPTH > YAG_CONTEXT->YAG_DEPTH) outdepth = TRUE;
        if ((ptfather->TYPE & FORCEPRIM_NODE) != 0) forceprim = TRUE;
    }
    if (outdepth && !forceprim) {
        freechain(ptnode->FATHERS);
        ptnode->FATHERS = NULL;
        return;
    }

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        number_sons((gnode_list *)ptchain->DATA);
    }
}

static void
traverse(gnode_list *ptnode)
{
    chain_list  *ptchain;

    if ((ptnode->TYPE & TRAVERSED) != 0) return;
    ptnode->TYPE |= TRAVERSED;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        traverse((gnode_list *)ptchain->DATA);
    }

    total_son_arcs = 0;
    visited_son_arcs = 0;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        count_sons((gnode_list *)ptchain->DATA);
    }
    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        unmark((gnode_list *)ptchain->DATA);
    }

    if (total_son_arcs == visited_son_arcs) ptnode->TYPE |= PRIMNODE;
}

static void
unmark(gnode_list *ptnode)
{
    chain_list  *ptchain;

    if (ptnode->VISITED == FALSE) return;
    ptnode->VISITED = FALSE;

    if ((ptnode->TYPE & PRIMNODE) != 0) return;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        unmark((gnode_list *)ptchain->DATA);
    }
}

static void
count_sons(gnode_list *ptnode)
{
    chain_list  *ptchain;

    visited_son_arcs++;

    if (ptnode->VISITED) return;
    ptnode->VISITED = TRUE;

    total_son_arcs += ptnode->NUMSONS;

    /* add additional dependancy to constrained node to force it PRIM */
    if ((ptnode->TYPE & (CONSTBIT|CONSTRAINT_NODE|FORCEPRIM_NODE|MEMSYM_NODE)) != 0) total_son_arcs++;

    if ((ptnode->TYPE & PRIMNODE) == 0) {
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            count_sons((gnode_list *)ptchain->DATA);
        }
    }
}

static void
pick_prims(gnode_list *ptnode)
{
    chain_list  *ptchain;

    if (ptnode->VISITED) return;
    ptnode->VISITED = TRUE;
    
    if ((ptnode->TYPE & CONSTRAINT_NODE) != 0) {
        if ((ptnode->TYPE & EXT) != 0) yagTagConstraint(ptnode->OBJECT.LOCON->NAME);
        else yagTagConstraint(ptnode->OBJECT.CONE->NAME);
    }

    if ((ptnode->TYPE & PRIMNODE) != 0) {
        VAR_LIST = add_primvar(VAR_LIST, ptnode);
    }
    else {
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            pick_prims((gnode_list *)ptchain->DATA);
        }
    }
}
 
/****************************************************************************
 *                         miscellaneous utility functions                  *
 ****************************************************************************/

 /*--------------------------------------------------*
 | Add a node to the main list of primary variables  |
 *--------------------------------------------------*/

static chain_list *
add_primvar(chain_list *list, gnode_list *ptnode)
{
    return addchain(list, ptnode);
}

 /*----------------------------------------------*
 | Return the node pointer to the named father   |
 | of the given node                             |
 *-----------------------------------------------*/

gnode_list *
yagGetNodeFather(gnode_list *ptnode, char *name)
{
    chain_list  *ptchain;
    gnode_list  *ptfather;
    char        *nodename;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        ptfather = (gnode_list *)ptchain->DATA;
        if ((ptfather->TYPE & EXT) != 0) {
            nodename = ptfather->OBJECT.LOCON->NAME;
        }
        else {
            nodename = ptfather->OBJECT.CONE->NAME;
        }
        if (nodename == name) break;
    }
    if (ptchain == NULL) {
        yagBug(DBG_NO_FATHER, "yagGetNodeFather", name, ptnode->OBJECT.CONE->NAME, 0);
        return NULL;
    }
    else return ptfather;
}

/****************************************************************************
 *                         function yagExtendSupportRoot();                   *
 ****************************************************************************/
 /*------------------------------------------*
 | Extend the support graph root of a given  |
 | graph by the given object                 |
 *-------------------------------------------*/

int
yagExtendSupportRoot(graph *ptgraph, cone_list *ptcone, long roottype)
{
    gnode_list *ptnode;
    ptype_list *ptroot;
    int         result;

    local = TRUE;
    CUR_TREE = ptgraph->WIDTH;
    CUR_GRAPH = ptgraph;

    result = TRUE;
    ptnode = (gnode_list *)gethtitem(ptgraph->HASHTAB, ptcone);
    if (ptnode == (gnode_list *)EMPTYHT) {
        ptnode = add_node(ptcone, CONE_TYPE, 0);
        result = FALSE;
    }
    else mark_treebit(ptnode);

    ptgraph->ROOTNODES = addptype(ptgraph->ROOTNODES, roottype, ptnode);

    for (ptroot = ptgraph->ROOTNODES; ptroot; ptroot = ptroot->NEXT) {
        ((gnode_list *)ptroot->DATA)->TYPE |= ptroot->TYPE;
    }

    ptgraph->WIDTH++;
    local = FALSE;
    return result;
}

/****************************************************************************
 *                         function yagExtendSupportGraph();                       *
 ****************************************************************************/
 /*------------------------------------------*
 | Update the given graph given a node and   |
 | its newly obtained father list            |
 *-------------------------------------------*/

void
yagExtendSupportGraph(graph *ptgraph, gnode_list *ptnode, chain_list *ptfatherlist)
{
    chain_list *ptchain;
    cone_list  *ptcone;
    gnode_list *ptfather;

    CUR_TREE = ptgraph->WIDTH - 1;
    CUR_GRAPH = ptgraph;

    for (ptchain = ptfatherlist; ptchain; ptchain = ptchain->NEXT) {
        ptcone = ((gnode_list *)ptchain->DATA)->OBJECT.CONE;

        ptfather = (gnode_list *)gethtitem(ptgraph->HASHTAB, ptcone);
        if (ptfather == (gnode_list *)EMPTYHT) {
            ptfather = add_node(ptcone, CONE_TYPE, 0);
        }
        ptfather->NUMSONS++;
        ptfather->SONS = addchain(ptfather->SONS, ptnode);
        ptnode->FATHERS = addchain(ptnode->FATHERS, ptfather);
    }
}

/****************************************************************************
 *                         function yagReduceSupportGraph();                       *
 ****************************************************************************/
 /*------------------------------------------*
 | Reduce the given graph by removing the    |
 | most recently added root node             |
 *-------------------------------------------*/

void
yagReduceSupportGraph(graph *ptgraph)
{
    gnode_list *ptnode;
    ptype_list *ptroot;
    
    CUR_TREE = ptgraph->WIDTH - 1;

    if (ptgraph == NULL) yagBug(DBG_NULL_PTR, "yagReduceSupportGraph", NULL, NULL, 0);
    
    for (ptroot = ptgraph->ROOTNODES; ptroot; ptroot = ptroot->NEXT) {
        ((gnode_list *)ptroot->DATA)->TYPE &= ~(TN_NODE|TP_NODE);
    }

    ptnode = (gnode_list *)ptgraph->ROOTNODES->DATA;
    reduce(ptgraph, ptnode);
    ptroot = ptgraph->ROOTNODES;
    ptgraph->ROOTNODES = ptgraph->ROOTNODES->NEXT;
    ptroot->NEXT = NULL;
    freeptype(ptroot);
    ptgraph->WIDTH--;
    ptgraph->COMPLETE = check_complete(ptgraph);
}

static void 
reduce(graph *ptgraph, gnode_list *ptnode)
{
    chain_list *ptchain;
    gnode_list *ptfather;

    unmark_treebit(ptnode);
    if (in_other_tree(ptnode)) return;

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        ptfather = (gnode_list *)ptchain->DATA;
        ptfather->SONS = yagRmvChain(ptfather->SONS, ptnode);
        ptfather->NUMSONS--;
        if (ptfather->NUMSONS == 0) {
            ptfather->TYPE |= DELETED_NODE;
            delhtitem(ptgraph->HASHTAB, ptfather->OBJECT.PTR);
        }
    }

    freechain(ptnode->FATHERS);
    ptnode->FATHERS = NULL;

    /* beware : node may be its own father hence already treated */
    if ((ptnode->TYPE & DELETED_NODE) == 0 && ptnode->NUMSONS == 0) {
        ptnode->TYPE |= DELETED_NODE;
        delhtitem(ptgraph->HASHTAB, ptnode->OBJECT.PTR);
    }
}

/****************************************************************************
 *                         function yagGetPrimVars();                         *
 ****************************************************************************/
 /*---------------------------------------------*
 | Return list of primary variables which       |
 | influence a given node                       |
 *---------------------------------------------*/

chain_list *
yagGetPrimVars(graph *ptgraph, gnode_list *ptnode)
{
    chain_list *ptchain;
    gnode_list *ptprimnode;

    unmark(ptnode);
    VAR_LIST = NULL;
    CUR_GRAPH = ptgraph;
    add_constraints = FALSE;
    prop_search(ptnode);
    
    if (add_constraints) {
        for (ptchain = ptgraph->PRIMVARS; ptchain; ptchain = ptchain->NEXT) {
            ptprimnode = (gnode_list *)ptchain->DATA;
            if ((ptprimnode->TYPE & CONSTRAINT_NODE) != 0 && !ptprimnode->VISITED) {
                VAR_LIST = addchain(VAR_LIST, ptprimnode);
            }
        }
    }

    unmark(ptnode);
    return VAR_LIST;
}

chain_list *
yagGetJustPrimVars(graph *ptgraph, gnode_list *ptnode)
{
    unmark(ptnode);
    VAR_LIST = NULL;
    CUR_GRAPH = ptgraph;
    prop_search(ptnode);
    unmark(ptnode);
    return VAR_LIST;
}

static void
prop_search(gnode_list *ptnode)
{
    chain_list *ptchain;

    if (ptnode->VISITED) return;
    ptnode->VISITED = TRUE;
    if ((ptnode->TYPE & PRIMNODE) != 0) {
        if ((ptnode->TYPE & CONSTRAINT_NODE) != 0) add_constraints = TRUE;
        VAR_LIST = addchain(VAR_LIST, ptnode);
        return;
    }
    else {
        for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
            prop_search((gnode_list *)ptchain->DATA);
        }
    }
}

static chain_list *RESCHAIN;

static void
isNonPrimaryUsed(gnode_list *ptnode)
{
    chain_list     *ptchain;
    gnode_list     *ptinnode;

    if (ptnode->VISITED)
        return;
    ptnode->VISITED = TRUE;
    if ((ptnode->TYPE & PRIMNODE) != 0) return;

    RESCHAIN = addchain(RESCHAIN, ptnode);

    for (ptchain = ptnode->FATHERS; ptchain; ptchain = ptchain->NEXT) {
        ptinnode = (gnode_list *) ptchain->DATA;
        isNonPrimaryUsed(ptinnode);
    }
}

chain_list *
yagGetNonPrimaryUsedNodes(graph *ptgraph)
{
    ptype_list     *ptroots;
    gnode_list     *ptnode;

    RESCHAIN = NULL;
    
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        isNonPrimaryUsed(ptnode);
    }
    for (ptroots = ptgraph->ROOTNODES; ptroots; ptroots = ptroots->NEXT) {
        ptnode = (gnode_list *) ptroots->DATA;
        unmark(ptnode);
    }
    return RESCHAIN;
}
    
