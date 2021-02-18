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

#ifndef TRCLIB_H
#define TRCLIB_H

#define RCXFLOAT double

// Bidouilles de Marc. Pour l'instant, ça marche.
#define TRC_LOCON_AWE    (long)0x00000007  // TMP
extern int TRC_MARK_CON; // SOLUTION TMP


// Les convertions d'unités entre TAS, RCX et le SI...

// Tas et TTV : capa=fF, délais=pS
// Alliance :   capa=pF, délais=pS

#define RCX_PTRTESTANDSET( p, v ) if(p) *p=v;
#define RCX_UNIT_TIME_TAS_TO_SI (1e-12)
#define RCX_UNIT_CAPA_ALC_TO_SI (1e-12)

// Structures temporaires crée sur les agresseurs créée par rcx_getagrlist() et 
// libérée par rcx_freeagrlist(). L'utilisateur doit remplir les champs FMINUP,
// FMINDW, EVENT et ACTIF.

typedef struct rcxparam {
  char                  *INSNAME;
  lofig_list            *LOFIG;
  losig_list            *SIGNAL;
  RCXFLOAT               FMINUP;        // Front minimum avec crosstalk
  RCXFLOAT               F0UP;          // Front minimum sans crosstalk
  RCXFLOAT               FMINDW;
  RCXFLOAT               F0DW;
  RCXFLOAT               CC;            // Capacité de couplage vers l'agresseur
  RCXFLOAT               CCA;           // Valeur totale des ctc sur l'agresseur
  char                   ACTIF;
  chain_list            *SIGLOCAL;
  ptype_list            *USER;
} rcxparam ;

// Champ ACTIF :
#define RCX_AGRBEST   0x01
#define RCX_AGRWORST  0x02
#define RCX_MTX_BEST  0x04 
#define RCX_MTX_WORST 0x08
#define RCX_QUIET     0x10
/* Les trois dernieres valeurs ne sont pas prise en compte par RCX. Elle servent
à indiquer que cet agresseur a été éliminé car il existe une contrainte de
mutuelle exclusion avec un autre agresseur. C'est géré par STB-CTK. */

// Ptype sur le losig qui pointe vers le rcxparam.
#define RCX_REALAGRESSOR 0x52435808


// Information sur la transition du signal

/* Note concernant les slopes :

  Les champs FCCMAX et F0MAX servent à calculer les capacités équivalentes par
  effet Miller. Les champs slope des structures MIN et MAX sont les fronts à
  appliquer à l'entrée du réseau RC. Le MAX.slope n'est pas forcement égale au
  FCCMAX, car FCCMAX est le front le plus mous sur tout le réseau RC et 
  MAX.slope est le front en entrée du réseau RC. 
*/

typedef struct {
  RCXFLOAT      slope ;
  RCXFLOAT      vend ;
  RCXFLOAT      vt ;
  RCXFLOAT      vdd ;
  RCXFLOAT      vsat ;
  RCXFLOAT      rlin ;
  RCXFLOAT      vth ;
  RCXFLOAT      r ;
  RCXFLOAT      c1 ;
  RCXFLOAT      c2 ;
  RCXFLOAT      slnrm ;
  mbk_pwl      *pwl ;
} rcx_slope_param ;

typedef struct {
  RCXFLOAT      F0MAX;  // Front max sans crosstalk
  RCXFLOAT      FCCMAX; // Front max avec crosstalk
  char          SENS;   // Sens de transition
  RCXFLOAT      CCA;    // Somme des ctc sur le signal victime
  rcx_slope_param MIN ;
  rcx_slope_param MAX ;
} rcx_slope; 

// La vue RCX.

typedef struct rcx
{
  struct rcx	*NEXT;
  unsigned int	 FLAG;
  lorcnet_list  *VIEW;
  locon_list    *DRIVERSUP;
  locon_list    *DRIVERSDW;
  chain_list	*RCXEXTERNAL;
  chain_list	*RCXINTERNAL;
} rcx_list;



// Définition du sens d'une transition

#define TRC_SLOPE_UP   'U'
#define TRC_SLOPE_DOWN 'D'

// Type de résultat
#define RCX_MIN 'm'
#define RCX_MAX 'M'

// Sens relatifs des transitions
#define RCX_SAME     's'
#define RCX_OPPOSITE 'o'

// Champ USER des locon
#define RCXNAME    ((long)1665)
#define RCX_LOCONCAPA_HEAP  0x5243580A
#define RCX_LOCONCAPA_SPACE 0x5243580D
#define RCX_LOCON_UP 0x5243580B
#define RCX_LOCON_DN 0x5243580C
#define RCX_FAKE 0x524358FA

// Champ USER des losig
#define PTYPERCX   ((long)1664)
#define RCX_FAKE 0x524358FA

// Arguments pour la fonction setrcxmodel()
#define RCX_BEST   ((int)1)
#define RCX_NOR    ((int)2)
#define RCX_NOCTC  ((int)3)
#define RCX_NORCTC ((int)4)
#define RCX_ALL    ((int)0)

// Arguments pour la fonction rcx_crosstalkactive()
#define RCX_NOCROSSTALK  0
#define RCX_MILLER       1
#define RCX_QUERY       -1

// Arguments pour la fonction rcx_rcnload()
#define RCX_CAPALOAD  0x1
#define RCX_PILOAD    0x2
#define RCX_BESTLOAD  0x3
#define RCX_LOADERROR 0x4

// Interface de la fonction rcx_getsigcapa()

#define RCX_SIGCAPA_GROUND ((char)0x01)
#define RCX_SIGCAPA_CTK    ((char)0x02)

#define RCX_SIGCAPA_LOCAL  ((char)0x01)
#define RCX_SIGCAPA_GLOBAL ((char)0x02)

#define RCX_SIGCAPA_NORM   ((char)1)
#define RCX_SIGCAPA_MILLER ((char)2)
#define RCX_SIGCAPA_DELTA  ((char)3)

extern RCXFLOAT RCX_MINRCSIGNAL;
extern RCXFLOAT RCX_MINRELCTKSIGNAL;
extern RCXFLOAT RCX_MINRELCTKFILTER;
extern RCXFLOAT RCX_CAPALOAD_MAXRC;
extern RCXFLOAT RCX_MAXDELTALOAD;

typedef struct rcx_signal_noise {
  rcxparam      *PARAM;
  RCXFLOAT       CGLOBALE;
  RCXFLOAT       CLOCALE;
  RCXFLOAT       NOISE_OVER;
  RCXFLOAT       NOISE_UNDER;
} rcx_signal_noise;

typedef struct {
  rcxparam   *PARAM;
  RCXFLOAT    CGLOBALE;
  RCXFLOAT    CLOCALE;
} rcxmodagr;

typedef struct awelist {
  struct awelist *NEXT;
  locon_list     *LOCON;
  RCXFLOAT        DMAX;
  RCXFLOAT        DMIN;
  RCXFLOAT        FMAX;
  RCXFLOAT        FMIN;
} awelist;

typedef struct rcxfile {
  FILE  *FD;
  char  *FILENAME;
  char   ISCACHE;
} rcxfile;

/* Configuration du modèle de crosstalk */

#define RCX_MILLER_NONE      '-'
#define RCX_MILLER_0C2C      'm'
#define RCX_MILLER_NOMINAL   'n'
#define RCX_MILLER_NC3C      '3'
extern char RCX_CTK_MODEL ;

#define RCX_NOISE_NONE          '-'
#define RCX_NOISE_NEVER         'n'
#define RCX_NOISE_ALWAYS        'a'
#define RCX_NOISE_IFSTRONG      's'
#define RCX_NOISE_CUSTOM        'c'
#define RCX_NOISE_FINE          'f'
extern char RCX_CTK_NOISE ;

#define RCX_SLOPE_NONE          '-'
#define RCX_SLOPE_CTK           'c'
#define RCX_SLOPE_NOMINAL       'n'
#define RCX_SLOPE_REAL          'e'
extern char RCX_CTK_SLOPE_NOISE ;
extern char RCX_CTK_SLOPE_MILLER ;

#define RCX_SLOPE_DELAY_NONE     '-'
#define RCX_SLOPE_DELAY_CTK      'c'
#define RCX_SLOPE_DELAY_ENHANCED 'e'
extern char RCX_CTK_SLOPE_DELAY ;

/* Fonctions de haut niveau rcx */
#define RCXI_GND                ((int)0x00000001)
#define RCXI_CTK                ((int)0x00000002)
#define RCXI_MAX                ((int)0x00000004)
#define RCXI_MIN                ((int)0x00000008)
#define RCXI_FORCED             ((int)0x00000010)
#define RCXI_ALLWIRE            ((int)0x00000080)

#define RCXI_UP                 ((char)'u')
#define RCXI_DW                 ((char)'d')

// Modèle Miller : 0c-2c ou -1c-3c

#define RCX_MILLER2C ((char)2)
#define RCX_MILLER4C ((char)4)

typedef struct rcx_build_tr_bip {
  struct rcx_build_tr_bip *NEXT;
  lowire_list             *RWIRE;
  lowire_list             *EWIRE;
} rcx_build_tr_bip;

typedef struct rcx_build_tr {
  struct rcx_build_tr *NEXT;
  rcx_build_tr_bip    *LIST;
  lowire_list         *FINAL;
} rcx_build_tr;

typedef struct rcx_triangle {
  struct rcx_triangle *NEXT ;
  lonode_list         *n0 ;
  lonode_list         *n1 ;
  lonode_list         *n2 ;
  RCXFLOAT             Z0 ;
  RCXFLOAT             Z1 ;
  RCXFLOAT             Z2 ;
  chain_list          *REPORTED_IN ;
  chain_list          *REPORTED_N1 ;
  chain_list          *REPORTED_N2 ;
  rcx_build_tr        *BUILD ;
} rcx_triangle ;

// zinaps : 4/3/2004

#define RCX_ORIGIN_GROUP_PTYPE 0x04030400
#define RCX_DESTINATION_GROUP_PTYPE 0x04030401
#define RCX_RC_TIMINGS_PTYPE 0x04030402

typedef struct rcx_timings
{
  struct rcx_timings *NEXT;
  float input_slope;
  float computed_delay;
  float computed_slope;
} rcx_timings;

typedef struct
{
  int nb_origins, nb_destinations;
  rcx_timings **origin_destination_array_UU;
  rcx_timings **origin_destination_array_DD;
} rcx_delays;

/* Argument 'slope' pour rcx_getloconcapa */
// #define TRC_SLOPE_UP   'U'
// #define TRC_SLOPE_DOWN 'D'
   #define TRC_SLOPE_UNK  'n'

/* Argument 'type' pour rcx_getloconcapa */
#define TRC_CAPA_MIN 'm'
#define TRC_CAPA_MAX 'M'
#define TRC_CAPA_NOM 'n'
#define TRC_CAPA_NOM_MIN 'i'
#define TRC_CAPA_NOM_MAX 'a'

/* Valeur 'status' renvoyée par rcx_getloconcapa */
#define RCX_CAPA_OK    'y'
#define RCX_CAPA_EQUIV 'e'

extern int RCX_NB_CAPA_L0,
           RCX_NB_CAPA_L1, 
           RCX_NB_CAPA_L2 ;

extern int RCX_CAPA_UP_MIN,
           RCX_CAPA_UP_MAX,
           RCX_CAPA_DW_MIN,
           RCX_CAPA_DW_MAX ;

extern int RCX_CAPA_NOM_MIN,
           RCX_CAPA_NOM_MAX,
           RCX_CAPA_UP_NOM_MIN,
           RCX_CAPA_UP_NOM_MAX,
           RCX_CAPA_DW_NOM_MIN,
           RCX_CAPA_DW_NOM_MAX ;

extern int RCX_CAPA_NOMF_MIN,
           RCX_CAPA_NOMF_MAX,
           RCX_CAPA_UP_NOMF_MIN,
           RCX_CAPA_UP_NOMF_MAX,
           RCX_CAPA_DW_NOMF_MIN,
           RCX_CAPA_DW_NOMF_MAX ; 

extern double RCX_SLOPE_UNSHRINK ;

#define TRC_HALF 'h'
#define TRC_END  'e'
#define TRC_FULL 'f'
