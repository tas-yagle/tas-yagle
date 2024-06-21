/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_util.h                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* Fonctions externes */

// USER des losig

#define RCX_ORIGIN 0x52435801
// Contient la liste chaînée des nom de signaux ayant servi à construire ce
// signal.

// USER des loins

#define RCXPSEUDOVSS 0x52435803
// Le champ DATA contient un losig correspondant à un signal de masse.

#define RCX_VSSNI 0x52435811
// Défini un signal qu'on ne connait pas mais qu'on considère comme toujours
// actif.

// USER des lonodes

#define RCXNODELEVEL 0x52435819
#define RCXSHORTCUT  0x5243581B

// Le level d'un noeud

// USER des wire

#define RCXTRIANGLE 0x5243581A
// Le triangle auxquel appartient cette résistance.


// USER des lofig

#define RCX_SIGVSS 0x52435803
// Le champ DATA contient un losig correspondant à un signal de masse.

#define RCX_MODINS 0x52435807
// Le champ DATA contient une table de hash qui permet d'obtenir un nom de
// modèle à partir du nom d'instance.


#define RCXTABCON ((long)1666)
#define RCXTABSIG ((long)1667)
#define RCX_AGRINDEX_LST 0x52435809

#define RCXNODEBUILD 0x80
#define RCXNODEINTR  0x01

// user des nodes
#define WIRECONNEXE 0x5243581C

typedef struct wirefromlocon {
  /* If this structure is changed, modify allocation mechanism that curently
     uses addptype in rcx_get_wire_connexe(), and freeptype in 
     rcx_free_wire_connexe(). */
  struct wirefromlocon  *NEXT ;
  chain_list            *NODELIST ;
  chain_list            *WIRELIST ;
} wirefromlocon_list ;

extern void        rcx_delalllosigrcx( lofig_list *lofig );
extern void        addloconrcxname( locon_list*, char* );
extern char*       getloconrcxname( locon_list* );
extern rcx_list*   getrcx( losig_list* );
extern locon_list* rcx_gethtrcxcon( loins_list*, lofig_list*, char* );
extern losig_list* rcx_gethtrcxsig( loins_list*, lofig_list*, char* );
extern void        rcx_dellofig( lofig_list* );
extern void        rcx_freetabagr( void*, size_t, int );
extern void*       rcx_buildtabagr( lofig_list*, losig_list*, size_t, int*, RCXFLOAT* );
extern int         rcx_isvisiblesignal( losig_list *losig );
extern lofig_list* rcx_getlofig(char*,ht*);
extern char*       rcx_getlofigname( lofig_list* );
extern int         rcx_gettabagrindex( losig_list *losig );
extern num_list*   rcx_getnodebytransition( locon_list *locon, char trans );
extern void        rcx_setnodebytransition( locon_list *locon, char trans, num_list *head );
void               rcx_cleannodebytransition( locon_list *locon );

rcx_list*   rcx_alloc( losig_list* );
int         delrcx( losig_list* );
void        freercx( rcx_list* );
void        givetransloconrcxname( locon_list* );
losig_list* getrcxpseudovss( ptype_list* );
losig_list* rcx_getvss( lofig_list* );
void        rcx_addhtrcxcon( loins_list*, lofig_list*, locon_list*);
void        rcx_delhtrcxcon( loins_list*, lofig_list* );
void        rcx_addhtrcxsigbyname( loins_list*, lofig_list*, losig_list*, char* );
void        rcx_delhtrcxsig( loins_list*, lofig_list* );
char*       rcx_gethtrcxmod( lofig_list*, char* );
void        rcx_addhtrcxmod( lofig_list*, char*, char* );
void        rcx_addorigin( lofig_list*, losig_list*, char* );
chain_list* rcx_getoriginlist( losig_list *losig );
int         testrcxloconname( locon_list*, char* );
ptype_list* addrcxpseudovss( ptype_list*, int, losig_list** );
int         rcx_islosigexternal( losig_list* );
int         rcx_isloconexternal( locon_list* );
losig_list* rcx_getvssni( lofig_list* );
char       *rcx_getinsname( loins_list* );

// Récupère le nom du signal utilisé pour le drive
extern char *rcx_getsigname( losig_list *losig );
extern void rcx_setsigname( losig_list *losig, char *name );

int rcx_isctkbasicmodel( rcx_list *ptrcx );
void rcx_setctkbasicmodel( losig_list *ptsig, rcx_list *ptrcx );
void rcx_setenvbasicmodel( void );
void rcx_unsetenvbasicmodel( void );
#define rcx_addhtrcxsig(ins,fig,sig)  rcx_addhtrcxsigbyname((ins),(fig),(sig),rcx_getsigname((sig)))

void rcx_levelize_node( losig_list *losig, lonode_list *start );
int rcx_getlevel_node( lonode_list *ptnode );
int rcx_testandsetlevel_node( lonode_list *ptnode, int index );
void rcx_cleanlevel_node( losig_list *losig );
rcx_triangle* rcx_triangle_node( losig_list *losig, lonode_list *lonode, lowire_list *from );
void rcx_cleantriangle_node( losig_list *losig, lonode_list *lonode, rcx_triangle *triangle );
/*
void rcx_settriangle_wire( lowire_list *wire, rcx_triangle *triangle );
rcx_triangle* rcx_gettriangle_wire( lowire_list *wire );
*/
int rcx_isonlytriangle( losig_list *losig );
int rcx_isreconvergence_node( losig_list *losig, lonode_list *lonode );

int rcx_triangle_search( losig_list *losig, lonode_list *lonode, lowire_list *wire, lowire_list *from, lowire_list **w2, lowire_list **wt );
rcx_build_tr* rcx_alloc_build_tr(void);
rcx_build_tr_bip* rcx_alloc_build_tr_bip( void );
void rcx_free_build_tr( rcx_build_tr *pt );
rcx_build_tr* rcx_build_node( losig_list *losig, lonode_list *lonode, lowire_list *from );
rcx_build_tr* rcx_find_real_one( losig_list *losig, rcx_build_tr *test, lonode_list *ptnode );
rcx_build_tr* rcx_find_real_two( losig_list *losig, rcx_build_tr *test, lonode_list *ptnode );
rcx_build_tr* rcx_find_real( losig_list *losig, rcx_build_tr *test, lonode_list *ptnode );
rcx_triangle* rcx_build_to_triangle( losig_list   *losig, lonode_list  *lonode, rcx_build_tr *pt_build );
void rcx_settriangle_build( losig_list *losig, rcx_build_tr *pt_build, rcx_triangle *triangle );
void rcx_display_triangle( rcx_triangle *triangle );
wirefromlocon_list* rcx_get_wire_connexe( losig_list *losig, num_list *driver );
int rcx_trip_connexe( losig_list *losig, lowire_list *lowire, wirefromlocon_list *connexe );
void rcx_free_wire_connexe( losig_list *losig, num_list *driver, wirefromlocon_list *connexe );
void rcx_error( int code, losig_list *losig, int type );
extern void rcx_displaymemoryusage( lofig_list *lofig );
