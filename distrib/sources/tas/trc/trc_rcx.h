/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc_rcx.h                                                   */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

/* Type de LOG */
#define RCXLOGINFO    'I'
#define RCXLOGWARNING 'W'
#define RCXLOGERROR   'E'
#define RCXLOGDEBUG   'D'

extern FILE *RCXFILELOG ;

// USER des lofig

#define RCX_HTAGR 0x5243580E
// Table de hash pour retrouver la liste des agresseur. rcx_agragrlistfast().

// USER des losig

#define RCX_BELLOW 0x52435800
// Lorsque ce ptype est présent, ce signal n'existe pas au niveau courant, mais
// est défini dans une des instances de la figure courante..
// Le nom du signal est la chaîne de caractère dans le champs DATA, sous la
// forme NOMINSTANCE.NOMSIGNAL.

#define RCX_COPIED 0x52435806
// Dans la fonction rcxbuild(), indique que ce signal a été traité, et qu'il
// est devenu le losig pointé par le champ data.

#define RCXLOSIGFLAG 0x52435812
// Contient des flags. Ce ptype est utilisé dans rcn_coupledinformation, il 
// correspond donc aux valeurs 0x52435812 et 0x52435813

#define RCX_NODE_BASE 0x52435814
// Contient le décalage à appliquer sur un noeud de signal d'instance.

// Flag de couplage.
#define RCX_NI_LOSIG 0x1

extern char RCX_FASTMODE;
extern char RCX_FREEUNUSEDCTC;
extern char RCX_USEREALSLOPE;

// Arguments where pour la fonction rcx_get_new_num_node()
#define RCX_LOINS 'i'
#define RCX_LOFIG 'f'

// Arguments pour la fonction rcx_iscrosstalk
#define RCX_TEST_ONE    'o'
#define RCX_TEST_LIMIT  'l'

/* Fonctions externes */

/* Indique que les crosstalk actifs seront pris en compte. Si cette 
fonction n'est pas appellée, les capacités de couplage sont prise à leur valeurs
nominales (pas d'effet de couplage). Cette fonction ne doit pas être utilisée 
pendant la construction des vues RCX. */
extern int rcx_crosstalkactive( int );

int  rcx_state( rcx_list*, int );
extern void rcx_create( lofig_list* );

/* Construction de la vue RCX d'une lofig */
extern void buildrcx( lofig_list* );

extern chain_list* rcx_getagrlist( lofig_list*, losig_list*, char*, chain_list* );
extern void rcx_freeagrlist( lofig_list*, losig_list*, chain_list* );
extern RCXFLOAT rcx_getmiller( RCXFLOAT fvic, RCXFLOAT fagr, char type, char model );
RCXFLOAT rcx_capamiller( losig_list*, loctc_list*, rcx_slope*, char );
void rcx_exit( int );
void rcx_setflagcoupled( losig_list *victime, losig_list *agresseur, long flag );
int  rcx_getflagcoupled( losig_list *victime, losig_list *agresseur, long flag );
void rcx_clearflagcoupled ( losig_list *victime, losig_list *agresseur, long flag );
int  rcx_isnotinfluentagressor( losig_list *victime, losig_list *agresseur );
void rcx_setnotinfluentagressor( losig_list *victime, losig_list *agresseur );
void rcx_clearnotinfluentagressor( losig_list *victime, losig_list *agresseur );
int rcx_iscrosstalkcapa( loctc_list*, losig_list* );

/* Sélectionne le type de représentation d'un losig */
extern int setrcxmodel( lofig_list*, losig_list*, int );

void rcx_freewire( losig_list* );
void rcx_freectc( losig_list* );
char* rcx_isbellow( losig_list* );
int rcx_isloop( rcx_list*, losig_list* );
void rcx_env_error( char*, char* );
extern void rcx_env( void );
int rcx_islosigbreakloop( losig_list* );
extern void rcx_setlosigbreakloop( losig_list* );
void rcx_freeflagcoupled( losig_list* );
void rcx_addbellow( losig_list *ptsig, char *sigpointed );
void rcx_clear_new_num_node( lofig_list *lofig );
int rcx_get_new_num_node( losig_list *losig, int node, char where, char withnowire );
void rcx_build_new_num_node( lofig_list *lofig );
int rcx_get_node_base( losig_list *losig );
int rcx_set_node_base( losig_list *losig, int index );
losig_list* rcx_get_out_ins_sig( losig_list *losig );
void rcx_copyrcx( lofig_list *lofig, losig_list *sigext, loins_list *loins, losig_list *sigint );
void rcx_trsfloinscapa( lofig_list *lofig, loins_list *loins, losig_list *sigint ) ;
void rcx_buildrcxnowire( lofig_list*, losig_list* );
void merge_ext( rcxfile *file, losig_list *ptsig, locon_list *ptcon, char iscache, char keeponenode );
void set_ext_rcn( losig_list *ptsig );
void rcx_addsignal( lofig_list *fig );
void rcx_name_locon_trs( lofig_list *currentfig );
void rcx_addfigcon( lofig_list *fig );
void rcx_set_lofig_savesig( lofig_list *fig );
losig_list* rcx_addsigbellow ( lofig_list*, char* );
rcxparam* rcx_allocrcxparam  ( void );

void rcx_clear_node_base( losig_list *losig );
void rcx_clear_new_num_node( lofig_list *lofig );
void rcx_trsfnodelocon( locon_list *locon, char where );

chain_list* rcx_getagrlistfast( lofig_list *lofig, char *insname, losig_list *victime );
void rcx_setagrlistfast( lofig_list *lofig, char *insname, losig_list *victime, chain_list *headagr );
void rcx_dumpnet( rcxfile *file, losig_list *losig );
void rcn_flatnet( lofig_list *lofig );
void rcx_mergercn( lofig_list *lofig, losig_list *sigext, loins_list *scanloins, losig_list *sigint, losig_list *vss, losig_list *vssni );
void rcx_repair_pnode( losig_list *losig );
char rcx_isvalidlocon( locon_list *locon );
RCXFLOAT rcx_realslope( RCXFLOAT cc, RCXFLOAT cca, RCXFLOAT fcc, RCXFLOAT f0 );
int rcx_calc_cca( rcx_slope *slope, losig_list *victime );
int rcx_calc_cca_param( rcxparam *param );
void rcx_set_locon_one_pnode( locon_list *locon );
char rcx_is_locon_one_pnode( locon_list *locon );

int rcx_dw_forloop( losig_list *losig, lonode_list *lonode, chain_list *chwire, ht *htpack, void *userdata);
int rcx_up_pack_forloop( losig_list *losig, lonode_list *lonode, rcx_triangle *tr, void *userdata );
int rcx_up_forloop( losig_list *losig, lonode_list *lonode, lowire_list *lowire, void *userdata );

extern void rcx_setdriver( losig_list *losig, num_list *driversup, num_list *driversdw );
extern void rcx_cleardriver( losig_list *losig );

void rcx_trsfdriver( lofig_list *currentfig, losig_list *sigext, rcx_list *rcxext, losig_list *sigint, rcx_list *rcxint );
