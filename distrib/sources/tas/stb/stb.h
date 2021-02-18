/****************************************************************************/
/* les structures de donnees de STB                                         */
/****************************************************************************/

#ifndef STB_DEBUG
#define STB_DEBUG 0
#endif

#ifndef STB_HEADER
#define STB_HEADER


/* liste des inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

/* inclusion mbk */
#include MUT_H
#include MLO_H

#ifdef AVERTEC
#include AVT_H
#endif

/* inclusion techno */
#include ELP_H

/* inclusion mcc */
#include MCC_H

/* inclusion ttv */
#define API_HIDE_TOKENS
#include STM_H
#include TTV_H


/* liste des constantes */

#define STB_GLOB_ANALYSIS  1           /* analyse globale                  */
#define STB_DET_ANALYSIS   2           /* analyse detaillee 
                          (plages de stabilite)            */
#define STB_RED_GRAPH      1           /* graphe reduit                    */
#define STB_DET_GRAPH      2           /* graphe detaille                  */

#define STB_SLOPE      (char)(0x01)    /* front actif                      */
#define STB_SLOPE_UP   (char)(0x11)    /* front haut actif                 */
#define STB_SLOPE_DN   (char)(0x21)    /* front bas actif                  */
#define STB_SLOPE_ALL  (char)(0x31)    /* 2 front actifs                   */

#define STB_STATE      (char)(0x02)    /* etat actif                       */
#define STB_STATE_UP   (char)(0x12)    /* etat actif haut                  */
#define STB_STATE_DN   (char)(0x22)    /* etat actif bas                   */
#define STB_STATE_ALL  (char)(0x32)    /* 2 etat actif                     */

#define STB_ACTIVE_UPUP  (char)(0x04)    /* 2 etat actif                    */
#define STB_ACTIVE_UPDN  (char)(0x08)    /* 2 etat actif                    */
#define STB_ACTIVE_DNUP  (char)(0x40)    /* 2 etat actif                    */
#define STB_ACTIVE_DNDN  (char)(0x80)    /* 2 etat actif                    */

#define STB_UP         (char)(0x10)    /* etat haut                        */
#define STB_DN         (char)(0x20)    /* etat bas                         */
#define STB_HZ         (char)(0x40)    /* etat hz                          */

#define STB_NO_VERIF             (char)(0x01)   /* pas de verif setup/hold          */
#define STB_VERIF_EDGE           (char)(0x02)   /* verif par rapport au front       */
#define STB_VERIF_STATE          (char)(0x04)   /* verif par rapport a l'etat       */
//#define STB_VERIF_IS_TRANSPARENT (char)(0x08)   /* marquage de latch transparent    */

#define STB_TYPE_CLOCK     'C'         /* clock                            */
#define STB_TYPE_CLOCKPATH 'K'         /* on clock path                    */
#define STB_TYPE_COMMAND   'D'         /* command                          */
#define STB_TYPE_LATCH     'L'         /* latch                            */
#define STB_TYPE_FLIPFLOP  'F'         /* bascule                          */
#define STB_TYPE_BREAK     'B'         /* break                            */
#define STB_TYPE_PRECHARGE 'P'         /* precharge                        */
#define STB_TYPE_EVAL      'E'         /* precharge                        */
#define STB_TYPE_PREHZ     'Z'         /* precharge                        */
#define STB_TYPE_EVALHZ    'X'         /* precharge                        */
#define STB_TYPE_DIRECTIVECLOCK 'd'         /* on clock path                    */

#define STB_NO_INDEX   (char)(0x7F)    /* signal sans index                */

#define STB_NO_TIME  (long)(0x7FFFFFFF) /* pas de delay hold setup          */
#define STB_MIN_TIME (long)(0x80000000) /* pas de delay hold setup          */

#define STB_CHANGED        1           /* figure modifie                   */
#define STB_NOT_CHANGED    0           /* figure non modifie               */

#define STB_STABLE         1           /* figure stable                    */
#define STB_UNSTABLE       0           /* figure non stable                */

#define STB_RC_DELAY       1           /* delay RC                         */
#define STB_NO_RC_DELAY    0           /* pas de delay RC                  */

// flag stbfig
#define STB_CLOCK_INFO_READY 1
#define STB_HAS_FALSE_ACCESS 2



#define STB_NODE_NODATA     (char)(0x01) /* figure stable                    */
#define STB_NODE_DATA       (char)(0x02) /* figure stable                    */
#define STB_NODE_NOSETUP    (char)(0x04) /* pas de calcul de setup           */
#define STB_NODE_NOHOLD     (char)(0x08) /* pas de calcul de hold            */
#define STB_NODE_DIVERGING  (char)(0x10) /* pas de calcul de hold            */
#define STB_NODE_DATA_ON_PRECHARGE  (char)(0x20) /* data controlant la mise a 1 d'une precharge */
#define STB_NODE_STABCORRECT       (char)(0x40) /*  */

#define STB_STABILITY_BEST  (char)(0x01) /* figure stable                    */
#define STB_STABILITY_WORST (char)(0x02) /* figure non stable                */
#define STB_STABILITY_SETUP (char)(0x04) /* figure non stable                */
#define STB_STABILITY_HOLD  (char)(0x08) /* figure non stable                */
#define STB_STABILITY_LAST  (char)(0x10) /* figure non stable                */
#define STB_STABILITY_LT    (char)(0x20) /* figure non stable                */
#define STB_STABILITY_FF    (char)(0x40) /* figure non stable                */
#define STB_STABILITY_ER    (char)(0x80) /* figure non stable                */

#define STB_CTK_NOT         (char)(0x00) /* figure sans ctk                  */
#define STB_CTK             (char)(0x01) /* figure avec ctk                  */
#define STB_CTK_LINE        (char)(0x02) /* remplacement du dtx              */
#define STB_CTK_WORST       (char)(0x04) /* analyse pire cas                 */
#define STB_CTK_OBSERVABLE  (char)(0x10) /* analyse avec observable          */
#define STB_CTK_REPORT      (char)(0x20) /* drive fichier de report          */
#define STB_PROPAGATE       (char)(0x40) /* propage les delais et fronts     */
#define STB_CTX_REPORT      (char)(0x80) /* drive fichier ctx                */

/* Arguments pour stb_ctk_sort_noise */
#define STB_CTK_SORT_INSIDE_ALIM_MAX    'A'
#define STB_CTK_SORT_INSIDE_ALIM_REAL   'a'
#define STB_CTK_SORT_RISE_MAX           'R'
#define STB_CTK_SORT_RISE_REAL          'r'
#define STB_CTK_SORT_FALL_MAX           'F'
#define STB_CTK_SORT_FALL_REAL          'f'
#define STB_CTK_SORT_SCORE_TOTAL        't'
#define STB_CTK_SORT_SCORE_NOISE        'n'
#define STB_CTK_SORT_SCORE_CTK          'c'
#define STB_CTK_SORT_SCORE_INTERVAL     'i'
#define STB_CTK_SORT_SCORE_ACTIVITY     'y'

#define STB_CTK_SORT_ABS_DELAY          1
#define STB_CTK_SORT_ABS_MIN_DELAY      2
#define STB_CTK_SORT_ABS_MAX_DELAY      3
#define STB_CTK_SORT_REL_DELAY          4
#define STB_CTK_SORT_REL_MIN_DELAY      5
#define STB_CTK_SORT_REL_MAX_DELAY      6
#define STB_CTK_SORT_ABS_SLOPE          7
#define STB_CTK_SORT_ABS_MIN_SLOPE      8
#define STB_CTK_SORT_ABS_MAX_SLOPE      9
#define STB_CTK_SORT_REL_SLOPE         10
#define STB_CTK_SORT_REL_MIN_SLOPE     11
#define STB_CTK_SORT_REL_MAX_SLOPE     12

/* constantes pour le format du fichier d'entree */
#define STB_STB_FORMAT    'A' /* format STB d'AVERTEC        */
#define STB_SDC_FORMAT    'S' /* format SDC de SYNOPSYS      */

/* constantes pour le format du fichier de sortie */
#define STB_SEPARATE_INTERVALS    1 /* format des intervalles        */
#define STB_MIX_INTERVALS         0 /* format des intervalles        */

/* constantes pour le mode du fichier de sortie */
#define STB_DRIVE_INTERNALS       1 /* intervalles des noeuds internes */
#define STB_IGNORE_INTERNALS      0 /* ignorer intervalles internes    */

/* constantes pour le suffix du fichier */
#define STB_SUFFIX_STO            1 
#define STB_SUFFIX_STB            0

/* mot clef du champ USER des ttvevent                                */
#define STB_NODE             ((long) 0x60000001 )
#define STB_CTK_DETAIL       ((long) 0x60000002 )
#define STB_BREAK_TEST_EVENT ((long) 0x60000024 )
#define STB_PATH_LIST        ((long) 0x60000026 )
#define STB_ONE_OR_NO_CLOCK_EVENT  ((long) 0x60000030 )

/* mot clef du champ USER des ttvsig                                  */
#define STB_WENABLE          ((long) 0x60000010 )
#define STB_SETUP            ((long) 0x60000011 )
#define STB_HOLD             ((long) 0x60000012 )
#define STB_INVERTED_CLOCK   ((long) 0x60000013 )
#define STB_IDEAL_CLOCK      ((long) 0x60000014 )
#define STB_IS_CLOCK         ((long) 0x60000015 )
#define STB_CLOCK_TREE_INFO  ((long) 0x60000016 )
#define STB_DATA_ON_PRECHARGE_NODE  ((long) 0x60000017 )
#define STB_DELAYMARGINS     ((long) 0x60000018 )
#define STB_MARK_DEBUG       ((long) 0x60000019 )
#define STB_CLOCK_LOCAL_LATENCY ((long) 0x60000020 )
#define STB_VIRTUAL_CLOCK      ((long) 0x60000021 )
//#define STB_DIRECTIVES    ((long) 0x60000022 )
#define STB_CTK_FAST_SLOPE  ((long) 0x60000023 )
#define STB_TRANSITION_PROBABILITY 0x60000028
//#define STB_DIRECTIVES_TEMP  ((long) 0x60000029 )

/* mot clef du champ USER des ttvfig                                */
#define STB_FIGURE           ((long) 0x60000002 )

/* mot clef du champ USER des stbfig                                */
#define STB_DOMAIN           ((long) 0x60000003 )
#define STB_EQUIVALENT       ((long) 0x60000004 )
#define STB_FIG_FALSESLACK   ((long) 0x60000005 )

/* mot clef du champ USER des line                                 */
#define STB_TRANSFERT_CORRECTION_PTYPE ((long) 0x60000027 )

/* Type de detection pour stb_overlap                                   */
#define STB_STD              ((long) 0x00000001 )
#define STB_OBS              ((long) 0x00000002 )

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/* flags pour stb debug */

#define STB_COMPUTE_DEBUG_CHRONO        1
#define STB_UPDATE_SETUP_HOLD           2
#define STB_DIFFERENTIATE_INPUT_EVENTS  4
#define STB_DIFFERENTIATE_INPUT_COMMANDS 8
#define STB_REMOVE_LAGGING_PRECHARGES 16
#define STB_COMPUTE_SETUP_ONLY 32
#define STB_COMPUTE_HOLD_ONLY 64
/* */
#define __FINDUP__ (TTV_FIND_NOT_UPDW | TTV_FIND_NOT_DWDW)
#define __FINDDOWN__ (TTV_FIND_NOT_UPUP | TTV_FIND_NOT_DWUP)

#define STB_DEBUG_HZPATH                    0x1
#define STB_DEBUG_SETUP_UPCK                0x2
#define STB_DEBUG_HOLD_UPCK                 0x4
#define STB_DEBUG_DIRECTIVE                 0x8
#define STB_DEBUG_DIRECTIVE_DATA_IS_CLOCK   0x10
#define STB_DEBUG_DIRECTIVE_CLOCK_IS_DATA   0x20
#define STB_DEBUG_DIRECTIVE_RISING          0x40
#define STB_DEBUG_DIRECTIVE_FALLING         0x80
#define STB_DEBUG_SPECOUTMODE               0x100
#define STB_DEBUG_DIRECTIVE_DELAYMODE       0x200

#define STBCK_ONCLOCK_PATH     0x01
#define STBCK_MIN_EDGE_NOT_HZ  0x02
#define STBCK_MAX_EDGE_NOT_HZ  0x04
#define STBCK_FAKE_UP          0x08
#define STBCK_FAKE_DOWN        0x10
#define STBCK_ONFILTER_PATH    0x20
#define STBCK_MIN_EDGE_HZ      0x40
#define STBCK_MAX_EDGE_HZ      0x80

// flags pour stbtransferstbline
#define STB_TRANSFERT_NOFILTERING    1 
#define STB_TRANSFERT_NOPHASECHANGE  2
#define STB_TRANSFERT_CORRECTION     4
#define STB_TRANSFERT_NOT_COMMANDED  8

/* types de stb */         

typedef struct stbpair            /* couple de variables de stabilite */
    {                             /* d < u  par definition            */
     struct stbpair   *NEXT ;     /* Prochaine plage d'instabilite    */
     long                 D ;     /* debut de l'instabilite           */
     long                 U ;     /* fin de l'instabilite             */
     unsigned char phase_D, phase_U; /* phase d'origine du D et du U */
    }
stbpair_list ;

typedef struct stbck               /* definition des horloges          */
    {                             
     struct stbck    *NEXT;        /* pour multiple setup/hold         */
     long             SUPMAX ;     /* front montant max                */
     long             SDNMAX ;     /* front descendant max             */
     long             SUPMIN ;     /* front montant min                */
     long             SDNMIN ;     /* front descendant max             */
     long             PERIOD ;     /* periode de l'horloge             */
     long             SETUP ;      /* marge d'etablissement            */
     long             HOLD ;       /* marge de maintien                */
     long             ACCESSMAX ;  /* marge d'acces max                */
     long             ACCESSMIN ;  /* marge d'acces min                */
     char             CKINDEX ;    /* indice de l'horloge              */
     char             ACTIVE ;     /* front ou etat actif              */
     char             VERIF ;      /* setup front ou etat              */
     char             TYPE ;       /* type de clock                    */
     ttvevent_list    *CMD ;       /* command associe                  */
     char             CTKCKINDEX ; /* equivalent to CKINDEX, for all   
                                      clock paths nodes, not only on 
                                      connectors or command            */
     char             FLAGS;
     ttvevent_list    *ORIGINAL_CLOCK; /* event de clock d'origine */
    }
stbck ;

typedef struct stbnode            /* type qui sera ajoute dans le
                                     champ USER des noeuds du graphe
                                     de causalite                     */
    {  
     ttvevent_list *EVENT;        /* evenement associe                */
     stbpair_list **SPECIN;       /* contraintes sur les entrees      */
     stbpair_list **SPECOUT;      /* contraintes sur les sorties      */
     stbck        *CK;            /* horloge locale                   */
     stbpair_list **STBTAB;       /* variable de stabilité            */
     stbpair_list **STBHZ;        /* variable de stabilité HZ         */
     long          SETUP ;        /* setup du latch ou de la commande */
     long          HOLD ;         /* hold du latch ou de la commande  */
     char          NBINDEX ;      /* indice d'horloge max             */
     char          FLAGRC ;       /* flag d'un lien RC                */
     char          FLAG ;         /* flag d'un node en erreur         */
    }
stbnode ;

typedef struct stbchrono         /* description des chronogramme    */
    {
     struct stbchrono *NEXT;      /* evenement associe                */
     stbpair_list     *SPECS;     /* spec pour la sortie sig2         */
     stbpair_list     *SPECS_U;   /* spec pour la sortie sig2         */
     stbpair_list     *SPECS_D;   /* spec pour la sortie sig2         */
     stbpair_list     *SIG1S;     /* stabilite pour l'entree sig1     */
     stbpair_list     *SIG1S_U;   /* stabilite pour l'entree sig1     */
     stbpair_list     *SIG1S_D;   /* stabilite pour l'entree sig1     */
     stbpair_list     *SIG2S;     /* stabilite pour la sortie sig2    */
     stbpair_list     *SIG2S_U;   /* stabilite pour la sortie sig2    */
     stbpair_list     *SIG2S_D;   /* stabilite pour la sortie sig2    */
     long              CKUP_MIN;  /* horloge au niveau du noeud       */
     long              CKUP_MAX;  /* horloge au niveau du noeud       */
     long              CKDN_MIN;  /* horloge au niveau du noeud       */
     long              CKDN_MAX;  /* horloge au niveau du noeud       */
     char              VERIF;     /* type de contraintes              */
     char              TYPE;      /* type de données                  */
     char              SLOPE;     /* type de front ou d'etat          */
    }
stbchrono_list ;

typedef struct
{
  long value;
  ttvevent_list *common;
  long clock_margin, data_margin, clock_uncertainty;
  char same_origin;
} Extended_Setup_Hold_Computation_Data_Item;

typedef struct
{
  Extended_Setup_Hold_Computation_Data_Item setup_clock_gap[2], hold_clock_gap[2];
} Extended_Setup_Hold_Computation_Data;

typedef struct
{
  long VALUE;
  long instab, clock, margin, period, misc, skew;
  long datamoved, clock_margin, data_margin, moved_clock_period;
  long mc_setup_period, mc_hold_period, uncertainty, lag;
  ttvevent_list *skew_common_node;
  int nb_setup_cycle, nb_hold_cycle;
  short flags;
  unsigned char phase_origin;
} Setup_Hold_Computation_Detail_INFO;

typedef struct
{
  Setup_Hold_Computation_Detail_INFO setup, hold;
} Setup_Hold_Computation_Detail;

typedef struct stbdebug           /* structure d'affichage des chronogramme
                                     de debuggage                     */
    {
     struct stbdebug *NEXT;       /* evenement associe                */
     ttvsig_list     *SIG1;       /* signal d'origine d'erreur        */
     ttvevent_list   *SIG1_EVENT; /* noeud d'origine d'erreur/ peut etre NULL */
     ttvsig_list     *SIG2;       /* signal errone                    */
     long             PERIODE;    /* periode                          */
     char            *CKNAME;     /* nom de la clock                  */
     char            *CMDNAME;     /* nom de la clock                  */
     ttvevent_list   *CMD_EVENT;     /* nom de la clock                  */
     long             CKREFUP_MIN;    /* horloge de reference UP          */
     long             CKREFUP_MAX;    /* horloge de reference UP          */
     long             CKREFDN_MIN;    /* horloge de reference DN          */
     long             CKREFDN_MAX;    /* horloge de reference DN          */
     long             CKUP_MIN;       /* horloge au niveau du latch UP    */
     long             CKUP_MAX;       /* horloge au niveau du latch UP    */
     long             CKDN_MIN;       /* horloge au niveau du latch DN    */
     long             CKDN_MAX;       /* horloge au niveau du latch DN    */
     long             CKORGUP_MIN;    /* horloge au niveau du latch UP origine   */
     long             CKORGUP_MAX;    /* horloge au niveau du latch UP origine   */
     long             CKORGDN_MIN;    /* horloge au niveau du latch DN origine   */
     long             CKORGDN_MAX;    /* horloge au niveau du latch DN origine   */
     long             CKORGPERIOD;  /* period de la clock de l'entree */
     long             MARGESETUP; /* setup                            */
     long             MARGEHOLD;  /* hold                             */
     long             SETUP;      /* setup                            */
     long             HOLD;       /* hold                             */
     stbchrono_list  *CHRONO;     /* Chronogramme                     */
     long             HOLDPERIOD; /* changement du front de hold */
     long UPDELTA;
     long DOWNDELTA;
     Setup_Hold_Computation_Detail detail[2];
     ttvevent_list   *START_CMD_EVENT;     /* event de la commande de depart */
     ttvevent_list   *START_CLOCK;     /* event de la commande de depart */
     char            FALSEACCESS;
    }
stbdebug_list ;

typedef struct stbgap             /* description des intersections
                                     de diaphonie                     */
    {
     struct stbgap *NEXT ;        /* interval suivant                 */
     long           START ;       /* debut interval                   */
     char           SINCL ;       /* debut inclus                     */
     long           END ;         /* fin interval                     */
     char           EINCL ;       /* fin inclus                       */
     chain_list *   SIGNALS ;     /* list des signaux                 */
    } 
stbgap_list ;

typedef struct stbdomain
    {
     struct stbdomain *NEXT ; 
     char              CKMIN ;    /* index min du domaine             */
     char              CKMAX ;    /* index max du domaine             */
    }
stbdomain_list ;

typedef struct stbpriority
    {
     char        *MASK ;
     ttvsig_list *CLOCK ;
    }
stbpriority ;
    
typedef struct stbfig             /* informations concernant l'analyse  
                                     en cours                         */
    {
     struct stbfig      *NEXT ;    /* Figure suivante                  */
     ttvfig_list         *FIG ;    /* figure analysee                  */
     chain_list     *INSTANCE ;    /* liste des instances              */
     chain_list    *CONNECTOR ;    /* liste des connecteurs            */
     chain_list       *MEMORY ;    /* liste des points memorisant      */
     chain_list      *COMMAND ;    /* liste des commandes              */
     chain_list    *PRECHARGE ;    /* liste des precharges             */
     chain_list        *BREAK ;    /* liste des break points           */
     chain_list         *NODE ;    /* liste des noeuds intermediaires
                                      classes par profondeur           */
     chain_list    *PRIOCLOCK ;    /* liste des clocks prioritaires    */
     chain_list        *CLOCK ;    /* liste des connecteurs d'horloges */
     stbdomain_list *CKDOMAIN ;    /* liste des domaines d'horloge     */
     long               SETUP ;    /* specifie par l'utilisateur       */
     long                HOLD ;    /* specifie par l'utilisateur       */
     long         CLOCKPERIOD ;    /* periode d'horloge                */
     long          CHANGEFLAG ;    /* la stabilite a-t-elle evolue durant
                                     la derniere iteration ?           */ 
     char           **DISABLE ;    /* supprime des transitions         */
     char         PHASENUMBER ;    /* nombre de phases d'horloge       */
     char       STABILITYFLAG ;    /* la stabilite temporelle est-elle
                                      verifiee ?                       */ 
     char       STABILITYMODE ;    /* type de graphe de causalite      */
     char             CTKMODE ;    /* type de calcul de cross talk     */
     char            ANALYSIS ;    /* type d'analyse                   */
     char               GRAPH ;    /* type de graphe de causalite      */
     char FLAGS;
     ptype_list         *USER ;    /* user defined                     */
     HeapAlloc           CLOCK_TREE_HEAP;
     long       PHASE_DATE[128];

//     HeapAlloc           DIRECTIVE_HEAP;
    }
stbfig_list ;

/* Structure related to ctk */

typedef struct stb_ctk_detail_agr_list {
  struct stb_ctk_detail_agr_list  *NEXT;
  ttvsig_list                     *TTVAGR;
  char                            *NETNAME;
  char                             ACT_WORST;
  char                             ACT_BEST;
  char                             ACT_MUTEX_WORST;
  char                             ACT_MUTEX_BEST;
  char                             NOISE_RISE_PEAK ;
  char                             NOISE_RISE_EXCLUDED ;
  char                             NOISE_FALL_PEAK ;
  char                             NOISE_FALL_EXCLUDED ;
  float                            CC;
} stb_ctk_detail_agr_list;

typedef struct {
  char         FILLED ;
  chain_list  *OTHERDOMAIN ;
  chain_list  *CONDACTIF ;
  stbgap_list *GAPACTIF ;
  chain_list  *NOINFO ;
} stb_ctk_gap ;

typedef struct stb_ctk_detail {
  ttvevent_list                   *NODE;
  losig_list                      *LOSIG;
  float                            CM;
  float                            CC;
  char                            *MODEL_OVR;
  char                            *MODEL_UND;
  float                            NOISE_OVR;
  float                            NOISE_UND;
  float                            NOISE_MAX_OVR;
  float                            NOISE_MAX_UND;
  float                            NOISE_VTH;
  struct stb_ctk_detail_agr_list  *AGRLIST;
  stb_ctk_gap                      GAP_UP;
  stb_ctk_gap                      GAP_DW;
} stb_ctk_detail;

typedef struct stb_ctk_tab_stat {
  ttvevent_list         *NODE ;
  char                   SCORE_NOISE ;
  char                   SCORE_INTERVAL ;
  char                   SCORE_CTK ;
  char                   SCORE_ACTIVITY ;
  char                   SCORE_PROBABILITY ;
  float                  RISE_PEAK_REAL ;
  float                  FALL_PEAK_REAL ;
  float                  RISE_PEAK_MAX ;
  float                  FALL_PEAK_MAX ;
  char                  *NOISE_MODEL ;
  float                  CNET_GND ;
  float                  CNET_CC ;
  float                  CGATE ;
  float                  VTH ;
} stb_ctk_tab_stat ;

typedef struct stb_ctk_tab_delay {
  ttvline_list          *LINE ; 
} stb_ctk_tab_delay ;

typedef struct stb_ctk_stat {
  stb_ctk_tab_stat      *TAB ;
  int                    NBELEM ;
  int                    NBDISPLAY ;
  stb_ctk_tab_delay     *TABD ;
  int                    NBDELEM ;
  int                    NBDDISPLAY ;
} stb_ctk_stat ;

typedef struct
{
  chain_list *headagr;
  losig_list *sigvic;
  lofig_list *figvic;
} ctk_exchange;

typedef struct stbfile {
  FILE        *FD;
  stbfig_list *STBFIG ;
} stbfile;

typedef struct
{
  int level;
  long delaymin, delaymax;
  ttvevent_list *previous_divergence, *previous_divergence_hz;
} stb_clock_tree;

typedef struct
{
  long setup_data, setup_clock;
  long hold_data, hold_clock;
} stb_delaymargins;

typedef struct
{
  char *master;
  int edges, haslatency;
  stbck original_waveform;
  stbck latencies;
} stb_propagated_clock_to_clock;

typedef struct
{
  long date, delay, period;
  ttvevent_list *clock_event;
} stb_clock_latency_information;
/*
typedef struct stb_directive
{
  struct stb_directive *next;
  ttvsig_list *target2;
  long margin;
  char filter, target1_dir, target2_dir, operation;
} stb_directive;
*/
typedef struct
{
//  long validmin, validmax;
  stbpair_list *realpair;
} valid_range_info;

/* variables globales de stb */         

extern stbfig_list *HEAD_STBFIG ;
extern char         STB_LANG;
extern char         STB_TRACE_MODE;
extern char         STB_REPORT ;
extern char         STB_OUT ;
extern char         STB_SILENT ;
extern char         STB_GRAPH_VAR ;
extern char         STB_ANALYSIS_VAR ;
extern char         STB_MODE_VAR ;
extern char         STB_CTK_VAR ;
extern char         STB_OPEN_LATCH_PHASE ;

/* Fonctions related to ctk */

void                stb_env              __P(( void ));
void                stb_ctk_env          __P(( void ));
void                stb_ctk_sort_delay   __P(( stb_ctk_stat *stat, 
                                               char criterion, 
                                               float delta 
                                            ));
void                stb_ctk_sort_stat    __P(( stb_ctk_stat *tabstat, 
                                               char criterion
                                            ));
void                stb_ctk_clean        __P(( stbfig_list* ));
int                 stb_ctk              __P((stbfig_list* 
                                            ));
stb_ctk_detail*     stb_ctk_get_detail   __P(( stbfig_list *stbfig, 
                                               ttvevent_list *ptevent 
                                            ));
void                stb_ctk_free_detail  __P(( stbfig_list *stbfig, 
                                               stb_ctk_detail *detail 
                                            ));
stb_ctk_stat*       stb_ctk_get_stat     __P(( stbfig_list *stbfig ));
void                stb_ctk_clean_stat   __P(( stbfig_list *stbfig ));
stb_ctk_stat*       stb_ctk_fill_stat    __P(( stbfig_list *stbfig,int fast ));

void                stb_ctk_set_coef_score     __P(( int, int, int, int ));
void                stb_ctk_set_min_score      __P(( int, int, int, int ));
int                 stb_ctk_get_coef_noise     __P((void));
int                 stb_ctk_get_coef_ctk       __P((void));
int                 stb_ctk_get_coef_interval  __P((void));
int                 stb_ctk_get_coef_activity  __P((void));
int                 stb_ctk_get_score_total    __P(( stb_ctk_tab_stat* ));
int                 stb_ctk_get_score_noise    __P(( stb_ctk_tab_stat* ));
int                 stb_ctk_get_score_ctk      __P(( stb_ctk_tab_stat* ));
int                 stb_ctk_get_score_interval __P(( stb_ctk_tab_stat* ));
int                 stb_ctk_get_score_activity __P(( stb_ctk_tab_stat* ));
int stb_ctk_get_score_probability( stb_ctk_tab_stat *stat );
int                 stb_ctk_get_min_noise      __P(());
int                 stb_ctk_get_min_interval   __P(());
int                 stb_ctk_get_min_ctk        __P(());
int                 stb_ctk_get_min_activity   __P(());
void                stb_ctk_parse_agression    __P(( stbfig_list *stbfig ));


/* fonctions de stb */         

void            stb_ctk_env          __P(( void ));
void            stb_delerrorlist     __P((chain_list    *chainerr
                                        )) ; 
chain_list     *stb_geterrorlist     __P((stbfig_list   *stbfig,
                                          long           margin,
                                          int            nberror,
                                          long          *minsetup,
                                          long          *minhold,
                                          int           *errnumb
                                        )) ; 
stbfig_list    *stb_analysis         __P((ttvfig_list   *ttvfig, 
                                         char            analysemode, 
                                         char            graphmode, 
                                         char            stabilitymode,
                                         char            ctkmode
                                        ));
stbfig_list    *stb_getstbfig        __P((ttvfig_list   *ttvfig));

int             stb_savestbfig       __P((stbfig_list   *ptstbfig, 
                                          int            mode, 
                                          int            format, 
                                          int            suffix
                                        ));
int             stb_loadstbfig       __P((stbfig_list   *ptstbfig
                                        ));
stbnode        *stb_getstbnode       __P((ttvevent_list *ptnode
                                        ));
extern chain_list   *stb_overlap        __P((stbfig_list   *stbfig, 
                                             ttvevent_list *victim, 
                                             chain_list    *aggr_list,
                                             long           margin,
                                             long           mode
                                           ));
extern stbgap_list  *stb_overlapdev     __P((stbfig_list *stbfig, 
                                             ttvevent_list *eventref, 
                                             chain_list *eventlist, 
                                             long margin
                                           )); 
void            stb_freegaplist      __P((stbgap_list   *head
                                        )) ;
stbdebug_list  *stb_sortstbdebug     __P((stbdebug_list *debug)) ;
stbdebug_list  *stb_debugstberror    __P((stbfig_list   *stbfig,
                                          ttvsig_list   *sig,
                                          long margin,
                                          long computeflags
                                        )) ;
void            stb_freestbdebuglist __P((stbdebug_list *head
                                        )) ; 
stbpair_list   *stb_addstbpair       __P((stbpair_list  *head, 
                                         long            d, 
                                         long            u
                                        ));
stbpair_list   *stb_delstbpair       __P((stbpair_list  *head, 
                                          stbpair_list  *del
                                        ));
void            stb_freestbpair      __P((stbpair_list  *head
                                        ));
stbpair_list   *stb_dupstbpairlist   __P((stbpair_list  *ptheadlist
                                        ));
stbpair_list   *stb_globalstbtab __P((stbpair_list **tab, char size));
stbpair_list   *stb_globalstbpair __P((stbpair_list *head));
extern void     stb_init_var __P((void));
extern void     ctk_setprogressbar   __P(( void (*callfn)( int, int, int, char* ) ));
extern int      stb_delstbfig __P((stbfig_list *ptstbfig));
void            stb_getstblatchclose __P((stbfig_list *stbfig, ttvevent_list *latch, ttvevent_list *cmd, long *closemin, long *closemax, ttvevent_list **clockevent));
void            stb_getstblatchaccess __P((stbfig_list *stbfig, ttvevent_list *latch, ttvevent_list *cmd, long *accessmin, long *accessmax, ttvevent_list **clockevent));
extern void     stb_getstbdelta __P((stbfig_list *stbfig, ttvevent_list *latch, long *deltamin, long *deltamax, ttvevent_list *cmd));
int stb_cmd_can_generate_event (stbfig_list *stbfig, ttvevent_list *event, ttvevent_list *cmd);
extern void     stb_geteventphase( stbfig_list*, ttvevent_list*, char*, char *, char* , int clock, int data);
extern void     stb_getdelayedge( stbfig_list*, ttvevent_list*, char, long*, long*, char, int, char *, char * );
extern void     stb_getlatchslope( stbfig_list*, ttvevent_list*, ttvevent_list*, long*, long*, long*, long*, long*, int, char, long *);
void stb_computedelaymargin(stbfig_list *sbf);
void stb_delaymargins_getval(ttvevent_list *tve, long *data, long *clock, int setup);
void stb_delaymargins_freeval(ttvevent_list *tve);


stbfig_list *stb_parseSTO(ttvfig_list *ttvfig, char *filename);

extern void stb_set_ctk_information(stbfig_list *stbfig, ttvevent_list *node, long type, ctk_exchange *res);
extern void stb_release_ctk_information(ctk_exchange *res);

stbck *stb_gooddir(stbck *clock);
stbck *stb_getgoodclock_and_status(stbfig_list *sb, stbck *clock, ttvevent_list *tve, ttvevent_list *latch, int *inverted);
//void stb_getclockandtestslopes(ttvsig_list *tvs, stbck *clock, ttvsig_list **tvs_setup, long *dir_setup, ttvsig_list **tvs_hold, long *dir_hold);
extern stbfile* stb_info_open          __P(( stbfig_list *stbfig ));
extern void stb_info_close             __P(( stbfile *file ));
extern void stb_ctk_drive_stat( stbfile *report, stbfig_list *stbfig, stb_ctk_stat *stat );
extern void stb_ctk_clean_stat( stbfig_list *stbfig );
void stb_get_setup_and_hold_slopes_delta(stbck *clock, long *updelta, long *downdelta);
long stb_gettruncatedaccess(ttvevent_list *latch, ttvevent_list *cmd, int max);
extern void CtkMutexInit(ttvfig_list *tvf);
extern void CtkMutexFree(ttvfig_list *tvf);

void stb_report __P((stbfig_list *ptstbfig, char *filename));
int stb_get_setup_and_hold_test_events(stbck *clock, int hz, ttvevent_list **setup_event, ttvevent_list **hold_event, stbnode *node, int *setup_mustbehz, int *hold_mustbehz);
stbpair_list   *stb_nodeglobalpair __P((stbpair_list **tabpair, long periode, char size, char index));
stbck          *stb_getclocknode __P((stbfig_list *ptstbfig, char phase, char *namebuf, ttvevent_list **ptevent, stbck *ck));

chain_list *get_clock_latency_tree(ttvevent_list *tve, long type, chain_list *cl);
//long stb_getfilterlag(long abs_time, ttvevent_list *tve, long type);
int stb_get_filter_directive_info(stbfig_list *sf, ttvevent_list *tve, char phase, long *start, long *startmax, long *end, long *endmin, char *state, long *move, ttvevent_list **clockedsig, stbck *origedge);
int stb_has_filter_directive(ttvevent_list *tve);

long stb_getsetupslope(stbfig_list *stbfig, stbnode *node, stbck *ck, int hz, char *phase);
long stb_get_directive_setup_slope(stbfig_list *stbfig, stbnode *node, char *phase, long *period, int filter);
//int stb_isgateoutput(ttvsig_list *tvs);

extern stbpair_list* stb_getpairnode    __P(( stbfig_list *stbfig, ttvevent_list *event, long margin ));
stbpair_list* stb_getlimitpairnode( stbfig_list   *stbfig, ttvevent_list *event, long margin);

int stb_IsClockCK(ttvevent_list *tve, stbck *ck);
stbck *stb_GetClockCK(ttvevent_list *tve, stbck *ck);

void stb_setdetail(Setup_Hold_Computation_Detail_INFO *detail, long val, long instab, long clock, long margin, long period, long misc, Extended_Setup_Hold_Computation_Data_Item *eshcdi, short flags, unsigned char phase, long movedperiod);
int stb_isfalseslack(stbfig_list *stbfig, ttvevent_list *startck, ttvevent_list *eventin, ttvevent_list *eventout, ttvevent_list *endck, int type);
long stb_getperiod (stbfig_list *ptstbfig, char phase);
void stb_getmulticycleperiod(ttvevent_list *inpute, ttvevent_list *outpute, long inputperiod, long outputperiod, long *setupP, long *holdP, int *nb_setup_cycle, int *nb_hold_cycle);
//int stb_temp_setdirectives (ttvfig_list *tvf, inffig_list *ifl, HeapAlloc *myheap);
//void stb_temp_freedirectives(ttvfig_list *tvf, HeapAlloc *myheap);
//ptype_list *stb_get_temp_directive_slopes(ttvevent_list *tve, int setup);

#define STB_DECIFEQUALPHASE 1
#define STB_SPECOUTMODE     2

long stb_synchronize_slopes(stbfig_list *ptstbfig, char startphase, char endphase, long endper, int mode);
int stb_cmpphase(stbfig_list *sf, char a, char b);

int path_false_slack_check(ttvpath_list *tp, ttvevent_list *opencmd, ttvevent_list *topopenclock);
#define PRECHTEST
#define PHASEFORALL

//#define RELAX_CORRECT_DEBUG
#endif
