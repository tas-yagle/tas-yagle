/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_fact.h                                                  */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#define TTV_SIG_NEW        ((long) 0x00001004 ) /* nouveau signal            */

#define TTV_NODE_CMPT      ((long) 0x00001001 ) /* compteur d'un noeud       */
#define TTV_NODE_FIN       ((long) 0x00001002 ) /* lien factorisation in     */
#define TTV_NODE_FOUT      ((long) 0x00001003 ) /* lien factorisation out    */

#define TTV_NODE_MIN       ((long) 0x01000000 ) /* node factorisation in     */
#define TTV_NODE_MOUT      ((long) 0x02000000 ) /* node factorisation out    */
#define TTV_NODE_PFACT     ((long) 0x04000000 ) /* node factorisation        */
#define TTV_NODE_PUSED     ((long) 0x08000000 ) /* node factorisation used   */
#define TTV_NODE_MARFACT   ((long) 0x00100000 ) /* node factorisation marq   */
#define TTV_NODE_NEWFIN    ((long) 0x00200000 ) /* node factorisation in mod */
#define TTV_NODE_NEWFOUT   ((long) 0x00400000 ) /* node factorisation out mod*/
#define TTV_NODE_OLDFACT   ((long) 0x00800000 ) /* node factorisation ancien */
#define TTV_NODE_MARIN     ((long) 0x00040000 ) /* node factorisation 1 in   */
#define TTV_NODE_MAROUT    ((long) 0x00080000 ) /* node factorisation 1 out  */
#define TTV_NODE_FACTMASK  ((long) 0x0FFC0000 ) /* bits de mask pour fact    */

#define TTV_FLIN_IN        ((long) 0x00000001 ) /* lien de factorisation in  */
#define TTV_FLIN_OUT       ((long) 0x00000002 ) /* lien de factorisation out */

#define TTV_FIND_COUPLE    ((long) 0x01000000 ) /* recherche de couple       */
#define TTV_FIND_CMPT      ((long) 0x02000000 ) /* recherche de couple       */

typedef struct ttvfline                         /* lien de factorisation     */
    {
     struct ttvfline      *NEXT ;               /* lien de suivant           */
     long                 NBIN ;                /* nb in vue par le lien     */
     long                 NBOUT ;               /* nb out vue par le lien    */
     long                 NBPATH ;              /* nb path vue par le lien   */
     struct ttvevent      *NODE ;               /* noeud du lien             */
    }
ttvfline_list ;

typedef struct ttvcmpt                          /* compteur                  */
    {
     long                 NBIN ;                /* nb in vue par le noeud    */
     long                 NBOUT ;               /* nb out vue par le noeud   */
     long                 NBPATH ;              /* nb path vue par le noeud  */
     struct ttvfline      *PATHIN ;             /* lien in nue par le noeud  */
     struct ttvfline      *PATHOUT ;            /* lien out nue par le noeud */
     ht                   *TABNODE ;            /* table de noeud            */ 
     chain_list           *TABLIST ;            /* list des tables alloues   */
     long                 NBCOUPLE ;            /* nb de couple              */
     long                 NBNODEIN ;            /* nb d'entree               */
     long                 NBNODEOUT ;           /* nb de sortie              */
     ht                   *TABIN ;              /* table de noeud            */ 
     ht                   *TABOUT ;             /* table de noeud            */ 
    }
ttvcmpt_list ;

extern chain_list *TTV_ALLOC_FLINE ;
extern chain_list *TTV_CMPT_NODE ;
extern chain_list *TTV_CMPT_SIGLIST ;
extern ttvfline_list *TTV_FREE_FLINE ;
extern long TTV_MAX_FCYCLE ;
extern long TTV_MAX_FLINE ;
extern long TTV_MAX_CMPT ;

extern void           ttv_addcouple     __P(( ttvevent_list*,
                                              ttvevent_list*,
                                              ttvevent_list*
                                           ));
extern int            ttv_checkcouple   __P(( ttvevent_list*,
                                              ttvevent_list*,
                                              ttvevent_list*
                                           ));
extern int            ttv_delcouple     __P(( ttvevent_list* ));
extern int            ttv_cmptcompar    __P(( chain_list**, chain_list** ));
extern chain_list*    ttv_classcmpt     __P(( chain_list* ));
extern void           ttv_freeallcmpt   __P(( void ));
extern void           ttv_delallcmpt    __P(( void ));
extern int            ttv_freeflinelist __P(( ttvfline_list* ));
extern int            ttv_freefline     __P(( ttvcmpt_list*, long ));
extern int            ttv_freecmpt      __P(( ttvevent_list* ));
extern ttvfline_list* ttv_allocfline    __P(( ttvcmpt_list*,
                                              ttvevent_list*,
                                              long
                                           ));
extern ttvcmpt_list*  ttv_alloccmpt     __P(( ttvevent_list* ));
extern ttvcmpt_list*  ttv_getcmpt       __P(( ttvevent_list* ));
extern int            ttv_clearcmpt     __P(( ttvcmpt_list*, long ));    
extern int            ttv_verifactsig   __P(( ttvsig_list*, long ));
extern int            ttv_checkfactsig  __P(( ttvsig_list* ));
extern chain_list*    ttv_classnodetype __P(( chain_list* ));
extern int            ttv_delcmpt       __P(( ttvevent_list*, long ));
extern int            ttv_deletebadfact __P(( long ));
extern void           ttv_deleteglobal  __P(( long ));
extern void           ttv_deletenofact  __P(( long ));
extern void           ttv_cleansiglist  __P(( void ));
extern void           ttv_cleanfactline __P(( void ));
extern long           ttv_count         __P(( ttvfig_list*,
                                              ttvevent_list*,
                                              chain_list*,
                                              long
                                           ));
extern long           ttv_countsep      __P(( ttvfig_list*, long, char ));
extern void           ttv_detectinter   __P(( ttvfig_list*, int ));
