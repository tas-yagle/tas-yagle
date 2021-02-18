/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_overlap.h                                               */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     manipulation of data structure                                       */
/****************************************************************************/

typedef struct 
{
  struct 
  {
    float delayval, lineval;
    stbpair_list *pairnode;
  } ev[2];
} stb_fastslope;

extern long STB_CTK_MARGIN;

extern stbgap_list  *stb_addgap         __P((stbgap_list *head, 
                                             long         start, 
                                             char         sincl,
			                                 long         end, 
                                             char         eincl, 
								             chain_list  *signals));
extern void          stb_freegap        __P((stbgap_list *head));
extern void          stb_freegaplist    __P((stbgap_list *head));
extern int           stb_dumpgaplist    __P((stbgap_list *list));

/****************************************************************************/

extern stbgap_list  *stb_creategaplist  __P((stbpair_list *stbpair, 
			                                 ttvevent_list *name));
extern stbgap_list  *stb_revgaplist     __P((stbgap_list *gap));
extern stbgap_list  *stb_union          __P((stbgap_list *gap0, stbgap_list *gap1));

/****************************************************************************/

extern chain_list   *stb_dupchain       __P((chain_list *ch));

/****************************************************************************/

extern stbpair_list *stb_beforedstbpair __P((stbpair_list *list, long d)); 
extern stbpair_list *stb_insertstbpair  __P((stbpair_list *head, long d, long u));
extern stbpair_list *stb_insertckpair   __P((stbpair_list *pair, stbnode *node, long type));
extern stbpair_list *stb_periodpack     __P((stbpair_list *list, long ckperiod));
extern stbpair_list *stb_packstbpair    __P((stbpair_list *list));
extern int           stb_dumppairlist   __P((stbpair_list *list));
extern int           stb_intersect      __P((stbpair_list *victim_pair, stbpair_list *aggr_pair));
extern stbpair_list* stb_getpairnode    __P(( stbfig_list *stbfig, ttvevent_list *event, long margin ));
extern stbpair_list* stb_getlimitpairnode __P(( stbfig_list *stbfig, ttvevent_list *event, long margin ));

/****************************************************************************/
/*     functions for overlap processing                                     */
/****************************************************************************/

extern chain_list   *stb_mkanalyselist  __P((ttvfig_list   *fig));
extern int           stb_numberofdomain  __P(( stbfig_list *stbfig, stbnode *node ));
extern chain_list *stb_diftdomain __P(( stbfig_list *stbfig, ttvevent_list *ref, chain_list *testlist ));

extern stbgap_list  *stb_overlapdev     __P((stbfig_list *stbfig, ttvevent_list *eventref, chain_list *eventlist, long margin)); 
extern chain_list   *stb_overlap        __P((stbfig_list   *stbfig, 
                                             ttvevent_list *victim, 
                                             chain_list    *aggr_list,
                                             long           margin,
                                             long           mode
                                           ));
extern int           stb_testoverlap    __P(());
extern char          stb_getckindex     __P((stbck *cklist));
