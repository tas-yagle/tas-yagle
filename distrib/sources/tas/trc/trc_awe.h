/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TRC Version 1.01                                            */
/*    Fichier : trc101.h                                                    */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gregoire AVOT                                             */
/*                                                                          */
/****************************************************************************/

extern int AWECACHE ;
// Lorsque cette variable est à 1, le cache est actif.

extern char RCX_AWESTRAIGHT;
extern char RCX_STEPTANH;
extern char RCX_USING_AWEMATRIX;
extern char RCX_AWE_ONE_PNODE;
extern char *RCX_PLOT_AWE;
extern char AWE_FAST_MODE ;

#define RCX_USING_AWEMATRIX_IFNEED 'i'
#define RCX_USING_AWEMATRIX_NEVER  'n'
#define RCX_USING_AWEMATRIX_ALWAYS 'a'
#define RCX_USING_AWEMATRIX_FORCE  'f'

typedef struct {
  RCXFLOAT   extcapa ;
  rcx_slope *slope ;
  char       type ;
  RCXFLOAT   coefctc ;
  char       reset ;
}
awetreetrip ;

typedef struct {
  RCXFLOAT k ;
  RCXFLOAT extcapa ;
  int      ordre ;
}
aweinfmmt;

extern losig_list *AWE_DEBUG_NET ;
extern locon_list *AWE_DEBUG_DRIVER ;
extern locon_list *AWE_DEBUG_RECEIVER ;

/* Ordre d'évalutation par défaut pour AWE */
#define AWE_MAX_ORDER  3
#define AWE_MAX_MOMENT 5

/* ptype dans les lonodes */
#define AWENODE 0x30000000

/* ptype dans les losig */
#define AWEINFO      0x30000001
#define AWEUPMAXINFO 0x30000002
#define AWEUPMININFO 0x30000003
#define AWEDWMAXINFO 0x30000004
#define AWEDWMININFO 0x30000005

/* Information ajoutée dans les losig, accessible par le ptype AWEINFO */

typedef struct sawefiltre {
  RCXFLOAT     POLE[AWE_MAX_ORDER] ;
  RCXFLOAT     RESIDU[AWE_MAX_ORDER] ;
  int           ORDER;
} awefiltre;

typedef struct aweinfo {
  struct aweinfo        *NEXT;
  locon_list            *LOCON_DRIVER;
  long                   NODE_DRIVER;
  locon_list            *LOCON_LOAD;
  long                   NODE_LOAD;
  RCXFLOAT               K;
  unsigned char          FLAG;
  union {
    RCXFLOAT  MOMENT[AWE_MAX_MOMENT];
    awefiltre  FILTER;
  }                      DATA;
  RCXFLOAT              EXTCAPA;
} aweinfo_list;

typedef struct {
  aweinfo_list *HEAD ;
  ht           *HT ;
  int           NB ;
} awecache ;

typedef struct {
  double F;
  double VMAX;
  double VT;
  double A;
  double TR;
  double RLIN;
  double VSAT;
  double TSAT;
  double P1;
  double P2;
  double C1;
  double C2;
  double R;
  double RL;
  double V10;
  double V20;
} awe_tanh_data;

/* Valeur pour le champ FLAG de la structure aweinfo */
#define AWE_FLAGVALID           ((unsigned char)(0x80))
#define AWE_FLAGERROR           ((unsigned char)(0x40))

//FLAGVALID :
#define AWE_FLAGNODELAY         ((unsigned char)(0x03))
#define AWE_FLAGFILTER          ((unsigned char)(0x02))
#define AWE_FLAGMOMENT          ((unsigned char)(0x01))

//FLAGERROR :
#define AWE_FLAGALLORDERFAILED  ((unsigned char)(0x01))

#define AWE_MOMENT      ( AWE_FLAGVALID | AWE_FLAGMOMENT  )
#define AWE_FILTER      ( AWE_FLAGVALID | AWE_FLAGFILTER  )
#define AWE_NODELAY     ( AWE_FLAGVALID | AWE_FLAGNODELAY )
#define AWE_ERROR       ( AWE_FLAGERROR )

#define AWESIZEINFO 16

/* Information ajoutée dans les noeuds des réseaux RC */

typedef struct sawetabnode {
  RCXFLOAT     CAPA ;
  RCXFLOAT     SUMMOMENTCAPA;
  RCXFLOAT     MOMENT[ AWE_MAX_MOMENT ];
} awetabnode ;

typedef union uawetabnode_list {
  awetabnode              TABNODE;
  union uawetabnode_list *NEXT;
} awetabnode_list ;

typedef struct {
  aweinfo_list *AWE ;
  mbk_laplace  *LAPLACE ;
  float         VMAX ;
} awefilter ;

/* ptype temporaire dans les wires pour la fonction rcx_is_multiple_pnode() */
#define AWESUMRESI 0x5243581F


#define AWESIZETABNODE 200               // Taille d'un bloc alloué

// Mémoire pour le calcul des moments. Allocation par bloc, sans libération
// (pas de free)
awetabnode* aweallocnodes( void );
void aweunallocnodes( awetabnode* );
void awecleannodes( losig_list*, lonode_list* );
void awebuildnodes( losig_list *losig, 
                    lonode_list *node, 
                    RCXFLOAT    extcapa,
                    rcx_slope   *slopemiller,
                    char         type,
                    RCXFLOAT     coefctc,
                    char         reset
                  );
void build_awe_moment_without_loop( losig_list *losig, 
                                    locon_list *driver, 
                                    RCXFLOAT    extcapa, 
                                    rcx_slope  *slopemiller, 
                                    char        type, 
                                    RCXFLOAT    coefctc
                                  );
void build_awe_moment_with_loop( losig_list *losig, 
                                 locon_list *driver, 
                                 RCXFLOAT    extcapa, 
                                 rcx_slope  *slopemiller, 
                                 char        type, 
                                 RCXFLOAT    coefctc
                               );

// Calcul des moments.
void awemoment( losig_list*, 
                    lonode_list*, 
                    int, 
                    RCXFLOAT, 
                    RCXFLOAT 
                  );
RCXFLOAT awedownstream( losig_list *, lonode_list*, lowire_list*, int );
RCXFLOAT awemaxmoment( losig_list*, lonode_list* );

// Mémorisation des moments ou des coéfficients du filtre. Allocation par bloc, sans libération.
aweinfo_list* aweaddnodeinfo( losig_list*, 
                              locon_list*, 
                              long , 
                              locon_list*, 
                              long, 
                              RCXFLOAT*, 
                              RCXFLOAT,
                              RCXFLOAT
                            );
void awefreenodeinfo( losig_list* );
void awebuildinfo( losig_list*, locon_list*, long, RCXFLOAT, RCXFLOAT );
chain_list* getaweinfo( losig_list   *losig,
                        locon_list   *load,
                        locon_list   *driver,
                        RCXFLOAT     extcapa,
                        rcx_slope    *slopemiller,
                        char          type,
                        RCXFLOAT         coefctc
                      );
aweinfo_list* awegetnewinfo( void );
aweinfo_list* awegetheapinfo( void );
void awefreeinfolist( chain_list *head );
void aweallocinfo( void );
void awesaveaweinfo( losig_list *losig, long type );
void awefreeinfo( aweinfo_list *awe );
void aweselectaweinfo( losig_list *losig, long type );

// Transformation des moments en filtre.
void awemomentfiltre( aweinfo_list* );

// Calcul des délais et des fronts.
RCXFLOAT awevoltage_straight( aweinfo_list *awe, 
		               RCXFLOAT t,
		               RCXFLOAT vmax, 
		               RCXFLOAT a,
                               RCXFLOAT b
                             );
RCXFLOAT aweinstant_straight( aweinfo_list *awe,
                               RCXFLOAT vf,
                               RCXFLOAT vmax,
                               RCXFLOAT a,
                               RCXFLOAT b
                             );
RCXFLOAT awedelay( aweinfo_list*, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT, mbk_pwl* );
RCXFLOAT aweslope( aweinfo_list *awe, RCXFLOAT vmax, mbk_pwl **pwlout, rcx_slope *slope, char type, double coefctc ) ;

// Transformation tangente hyperbolique en droite.
void awe_tanh_to_straight( RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT*, RCXFLOAT* );

mbk_pwl* awe_tanh_to_pwl( RCXFLOAT inputslope, RCXFLOAT vmax, RCXFLOAT vt, RCXFLOAT vsat, RCXFLOAT rl, RCXFLOAT r, RCXFLOAT c1, RCXFLOAT c2 );
char awe_tanh( awe_tanh_data *data, double t, double *v );
double awe_tanh_tmax( awe_tanh_data *data );
RCXFLOAT awevoltage_pwl( aweinfo_list *awe, RCXFLOAT t, mbk_laplace *laplace );
RCXFLOAT aweinstant_pwl( aweinfo_list *awe, RCXFLOAT vf, RCXFLOAT vmax, mbk_laplace *laplace, int *status );
mbk_laplace* awe_pwl_to_laplace( mbk_pwl*, aweinfo_list* );
RCXFLOAT awe_pi_get_v20( RCXFLOAT tsat, RCXFLOAT inputslope, RCXFLOAT vmax, RCXFLOAT vt, RCXFLOAT r, RCXFLOAT c2 );

#define U(t) ((t)>=0.0?1.0:0.0)
#define INVTH( F, VT, VMAX, VTH ) ((F)*atanh(((VTH)-(VT))/((VMAX)-(VT))))

extern int getawedelay( lofig_list*, losig_list*, locon_list*, locon_list*, rcx_slope*, RCXFLOAT, RCXFLOAT, RCXFLOAT, RCXFLOAT*, RCXFLOAT*, RCXFLOAT*, RCXFLOAT*, mbk_pwl**, mbk_pwl** );
void trc_awecleancache( losig_list *ptsig );
extern awelist* getawedelaylist( lofig_list *lofig, losig_list *losig, locon_list *locon_emt, rcx_slope *slopemiller, RCXFLOAT extcapa );
extern void freeawedelaylist( awelist *headlist );
awelist *addawelist( awelist *head, locon_list *locon, RCXFLOAT dmax, RCXFLOAT dmin, RCXFLOAT fmax, RCXFLOAT fmin );
void awe_tanh_point_measure( RCXFLOAT vt, RCXFLOAT vmax, RCXFLOAT f, RCXFLOAT *t1, RCXFLOAT *v1, RCXFLOAT *t2, RCXFLOAT *v2 );

int awe_up_forcreatetriangle( losig_list *losig, lonode_list *lonode, lowire_list *lowire, void *userdata );
int awe_packup_forcreatetriangle( losig_list *losig, lonode_list *lonode, rcx_triangle *tr, void *userdata );
int awe_dw_forcleantriangle( losig_list *losig, lonode_list *lonode, chain_list *chwire, ht *htpack, void *userdata );
void awe_create_triangle( losig_list *losig, lonode_list *driver );
void awe_clean_triangle( losig_list *losig, lonode_list *driver );

int awe_dw_forcleannodes( losig_list *losig, lonode_list *lonode, chain_list *chwire, ht *htpack, void *userdata );

int awe_dw_formaxmoment( losig_list *losig, lonode_list *lonode, chain_list *chwire, ht *htpack, RCXFLOAT *ptmax );

int rcx_imp_clean( losig_list *losig, lonode_list *lonode, lowire_list *lowire, void *data );
int rcx_imp_prop( losig_list *losig, lonode_list *lonode, lowire_list *lowire, void *data );
int rcx_is_multiple_pnode( losig_list *losig, locon_list *driver );
void awe_error( int, int );
void awevoltage_pwl_plot( aweinfo_list *awe,
                          RCXFLOAT      tmax,
                          mbk_pwl      *lines,
                          mbk_laplace  *laplace,
                          float         t_unit,
                          float         t_vth,
                          float         vmax,
                          char         *filename
                        );
char awe_get_time_for_pwl( awefilter *filter, float v, float *t );
RCXFLOAT awevoltage_fast( aweinfo_list *awe, double f );


unsigned long int rcx_cache_release_awe( awecache *head );
unsigned long int rcx_add_cache_awe( losig_list *losig, char transition, char type, char iscrosstalk );
awecache* rcx_get_cache_awe( losig_list *losig, char transition, char type, char iscrosstalk );
unsigned long int rcx_awe_cache_update_ht( awecache *awetop, aweinfo_list *head );
void rcx_awe_cache_add_ht( ht *ht, aweinfo_list *aweinfo );
long rcx_awe_cache_fn( int isnew, chain_list *head, aweinfo_list *awe );

