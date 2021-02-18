/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC                                                         */
/*    Fichier : trc_delay.h                                                 */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/


typedef struct {
  RCXFLOAT   extcapa ;
  rcx_slope *slope ;
  char       type ;
  RCXFLOAT   coefctc ;
  ht        *htyi ;
}
dataforload ;

typedef struct {
  double Y1, Y2, Y3 ;
}
yiload ;

#define RCX_AGREINDEX_CHK 0x52435805

#define RCX_DELAYCACHE 0x52435821
#define RCX_MODEL_LOAD  0x01
#define RCX_MODEL_AWE   0x02

typedef struct rcx_cache_load {
  struct rcx_cache_load *NEXT ;
  int                    PIN ;
  float                  EXTCAPA ;
  char                   MODELREQUEST ;
  float                  R ;
  float                  C1 ;
  float                  C2 ;
  char                   MODELCOMPUTED ;
} rcx_cache_load ;

extern mbkcache *RCXDELAYCACHE ;
extern char RCX_USE_MATRIX_LOAD ;

/* Fonctions visibles */

extern char     rcx_rcnload( lofig_list*, losig_list*, num_list*, RCXFLOAT*, RCXFLOAT*, RCXFLOAT*, char, RCXFLOAT, rcx_slope*, char);
char     rcx_rcnload_basic( lofig_list*, rcx_list*, losig_list*, num_list*, RCXFLOAT*, RCXFLOAT*, RCXFLOAT*, char, RCXFLOAT, rcx_slope*, char);
extern int      rcx_getdelayslope( lofig_list*, locon_list*, locon_list*, rcx_slope*, RCXFLOAT, RCXFLOAT*, RCXFLOAT*, RCXFLOAT*, RCXFLOAT*, mbk_pwl**, mbk_pwl** );
extern RCXFLOAT rcx_getdelay( lofig_list*, locon_list*, locon_list*);
extern void rcx_rcnload_reset( losig_list* );
RCXFLOAT   rcx_getsumresi( losig_list* );

int      rcx_iscrosstalk( lofig_list*, losig_list*, char );
int      rcx_isrcdelay( lofig_list*, losig_list* );
void     rcx_checkcrosstalk( losig_list* );
RCXFLOAT rcx_getcoefctc( lofig_list*, losig_list*, rcx_slope*, char, RCXFLOAT, RCXFLOAT*);

int rcx_dw_forload( losig_list *losig, lonode_list *lonode, chain_list *chwire, ht *htpack, dataforload *data );
int rcx_up_pack_forload( losig_list *losig, lonode_list *lonode, rcx_triangle *tr, void *userdata );
int rcx_up_forload( losig_list *losig, lonode_list *lonode, lowire_list *lowire, void *userdata );
yiload* rcx_getadmiforload( ht *table, int node );
void rcx_setadmiforload( ht *table, int node, double y1, double y2, double y3 );
long int rcx_cache_build_ptype( char transition, char type, char iscrosstalk, char model );
rcx_cache_load* rcx_get_cache_load( losig_list *losig, int pin, float extcapa, char model, char transition, char type, char iscrosstalk );
unsigned long int rcx_add_cache_load( losig_list *ptsig, int pin, float extcapa, char modelrequest, char modelcomputed, char transition, char type, char iscrosstalk, float r, float c1, float c2 );
unsigned long int rcx_cache_fn_load( void *null, losig_list *losig );
unsigned long int rcx_cache_fn_release( void *null, losig_list *losig );
mbkcache* rcx_get_delay_cache( void );
void rcx_init_delay_cache( unsigned long int size );
char rcx_cache_delay_model( long int ptype );
char rcx_cache_delay_crosstalk( long int ptype );
char rcx_cache_delay_type( long int ptype );
char rcx_cache_delay_transition( long int ptype );
void trc_logbuildtype( long int ntype );
unsigned long int rcx_cache_release_load( rcx_cache_load *load );

extern void rcx_disable_delay_cache_for_crosstalk( void );
extern void rcx_enable_delay_cache_for_crosstalk( void );
extern int rcx_is_delay_cache_for_crosstalk( void );
extern void rcx_disable_delay_cache_for_quiet( void );
extern void rcx_enable_delay_cache_for_quiet( void );
extern int rcx_is_delay_cache_for_quiet( void );
void rcx_delay_cache_remove_from_cache( char ctkmode );
extern void trcflushdelaycache();
void trccheckcachecoherence();
extern void rcx_crosstalk_analysis( char mode );

void delaybetween( lofig_list *lofig, locon_list *start, locon_list *end, int display );
extern int  alldelayfromlocon( lofig_list *lofig, locon_list *locon, int display );
extern void trc_delayforlosig( lofig_list *lofig, losig_list *losig, int display, int level );
extern void rcx_forcercdelay(losig_list *losig);

