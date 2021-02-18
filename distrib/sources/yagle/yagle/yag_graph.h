/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v3.50                                                */
/*    Fichier : yag_graph.h                                                 */
/*                                                                          */
/*    (c) copyright 1994 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/* node types */
#define DELETED_NODE     ((long) 0x00000001 )
#define EXT              ((long) 0x00000004 )
#define CONE_TYPE        ((long) 0x00000008 )
#define CONSTBIT         ((long) 0x00000010 )
#define TN_NODE          ((long) 0x00000020 )
#define TP_NODE          ((long) 0x00000040 )
#define ROOT_NODE        ((long) 0x00000080 )
#define CONSTRAINT_NODE  ((long) 0x00000100 )
#define LOOP_NODE        ((long) 0x00000200 )
#define FORCEPRIM_NODE   ((long) 0x00000400 )
#define MEMSYM_NODE      ((long) 0x00000800 )
#define PRIMNODE         ((long) 0x08000000 )
#define HOLENODE         ((long) 0x20000000 )
#define STOPNODE         ((long) 0x40000000 )
#define TRAVERSED        ((long) 0x80000000 )

/* standard graph functions */

graph          *yagBuildGraph(edge_list *ptedgelist, cone_list *rootcone, int force);
gnode_list     *yagRootGraph(graph *ptgraph, cone_list *ptcone);
graph          *yagNewGraph(void);
void            yagFreeGraph(graph *ptgraph);
void            yagTraverseGraph(graph *ptgraph);
gnode_list     *yagGetNodeFather(gnode_list *ptnode, char *name);
chain_list     *yagGetPrimVars(graph *ptgraph, gnode_list *ptnode);
chain_list     *yagGetJustPrimVars(graph *ptgraph, gnode_list *ptnode);
chain_list     *yagGetNonPrimaryUsedNodes(graph *ptgraph);
void            yagAddExtraConstraints(graph *ptgraph);

/* support graph functions */

int             yagExtendSupportRoot(graph *ptgraph, cone_list *ptcone, long roottype);
void            yagExtendSupportGraph(graph *ptgraph, gnode_list *ptnode, chain_list *ptfatherlist);
void            yagReduceSupportGraph(graph *ptgraph);

