/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_prsrcx.h                                                */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

#define RCXPARSE_LOAD 'L'
#define RCXPARSE_MEMPOS 'M'
#define RCXPARSE_CACHE 'C'
#define RCXPARSE_SKIP 'S'

// USER des lofig

#define RCX_LISTINSNAME 0x52435804
// Utilisé uniquement pour debugger. Contient la liste chaînée des noms 
// des instances contenues dans la figure lue par rcxparse().

// USER des loins ET des losig

#define RCX_UNRESOLVEDSIG 0x52435802
// Lorsque la loins contient des vues RCX non encore mergées avec la lofig, ce
// ptype contient la liste des losig qui sont référencés par des CTC, mais
// qui n'appartiennent pas au niveau courant. Ils seront transférés dans la
// lofig dans la catégorie des RCX_BELLOW.

// USER des lofig
#define RCX_FLAGTAB  0x52435815
#define RCX_FLAGHEAP 0x52435816

/* Initialise les structures RCX sur les signaux, et lit la description RCX des
instances de la lofig */
extern void getrcxview ( lofig_list* ) ;

/* Initialise les structures RCX sur les signaux d'une lofig au niveau 
transistor */
extern void flatrcx ( lofig_list* );

extern int rcxparse ( loins_list*, lofig_list *,char*, ht* ) ;
unsigned long int rcxparsesignalparasitics ( FILE *datafile, char *filename, int *nbligne, lofig_list *lofig, loins_list *loins, losig_list *ptsig, long *sigindex, losig_list *pseudovss, losig_list *pseudovssni, char mode, chain_list **siglist);

/* Fonctions internes */

void addfigconrcx        __P(( lofig_list* )) ;
void addsignalrcx        __P(( lofig_list* )) ;
chain_list* rcx_readline __P(( FILE*, int* )) ;
int nbfield              __P(( chain_list* )) ;
float getfloat           __P(( char*, int* )) ;
long getint              __P(( char*, int* )) ;
void trc_addlocondir     __P(( locon_list*, char )) ;
char* rcx_devect         __P(( char* ));
void fatalerror          __P(( char*, int ));
void rcx_parse_free      __P(( lofig_list* ));
void rcx_reload( lofig_list *lofig, char fileacces );
void rcx_parselocon( lofig_list *lofig, loins_list *loins, locon_list *locon, RCXFLOAT *capa, int nbcapa, int version, char *filename, int nbligne );
