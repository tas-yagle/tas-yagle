/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas600.h                                                    */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* les structures et base de donnees de TAS                                 */
/****************************************************************************/

#ifndef TAS
#define TAS

//#define USEOLDTEMP

/* LISTE DES INCLUDE */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>

#define TAS_UNKNOWN_EVENT   ((char)'X')

/* LISTE DES DEFINE */
#define CARAFRONT          1
#define CARACAPA           2
#define CARALES2           3
#define INTERNE            0           
#define FCARAC             1
#define CCARAC             2

#define TAS_LUT_INT        1
#define TAS_LUT_CPL        2
#define TAS_SCM_INT        3
#define TAS_SCM_CPL        4
#define TAS_PLY_INT        5
#define TAS_PLY_CPL        6    

#define TAS_KEEP_LOFIG      ((int)  0x0c1c)

#define TAS_TRMODEL_MCCRSAT ((char)'M')
#define TAS_TRMODEL_SPICE   ((char)'S')

#define TAS_MODEL_SCM       ((char)'m')
#define TAS_MODEL_IV        ((char)'i')

/* erreurs de tas */
#define TAS_NULL           0
#define TAS_ERROR          1
#define TAS_WARNING        2
#define TAS_BLOC_SIZE      (long)1024
#define TAS_NBEMETMAX      (long)20
#define TAS_NBRECMAX       (long)20
#define TAS_NBCONMAX       (long)35
#define TAS_DELTADELAY     (long)25
#define TAS_DIFFDELAY      (long)30
#define TAS_MAXDELAY       (long)100

/* defines utilises pour des affectations */
#define TAS_NOTIME         (LONG_MIN          ) /* pas de TPxx  */
#define TAS_NOFRONT        (LONG_MIN          ) /* pas de front */
#define TAS_NORES          ((long) 0x00000000 ) /* pas de res  */
#define TAS_NOS            (LONG_MIN          ) /* pas de s     */
#define TAS_NOCAPA         ((float) 0x00000000 ) /* pas de c     */
#define TAS_NOWIDTH        ((long) 0x00000000 ) /* pas de w     */
#define TAS_NOLENGTH       ((long) 0x00000000 ) /* pas de l     */
#define TAS_ELM            ((char) 'E'        ) /* elmore save  */
#define TAS_RCN            ((char) 'R'        ) /* rcn save     */

/* defines utilises pour des affectations en double */
#define TAS_NOFRONT_DBL    (DBL_MAX) /* pas de front */
/* mots clefs des champs USER des locon */
#define TAS_CON_CARAC      ((long) 0x20000001 )
#define TAS_CON_BYPASSIN   ((long) 0x20000002 )
#define TAS_CON_BYPASSOUT  ((long) 0x20000003 )
#define TAS_CON_TAGCONE    ((long) 0x20000004 )
#define TAS_CON_TAGOUT     ((long) 0x20000005 )
#define TAS_CON_TAGUSED    ((long) 0x20000006 )
#define TAS_CON_TAGCAPA    ((long) 0x20000007 )
#define TAS_CON_BLDUP      ((long) 0x20000008 )
#define TAS_CON_BLDDOWN    ((long) 0x20000009 )
#define TAS_CON_MARQUE     ((long) 0x2000000a )
#define TAS_CON_INCONE     ((long) 0x2000000b )

/* mots clefs des champs USER des lotrs */
#define TAS_BTCONFL        ((long) 0x20000010 )
#define TAS_UCONFL         ((long) 0x20000020 )
#define TAS_CAPA_PASS      ((long) 0x20000030 )
#define TAS_TRANS_TAGCONE  ((long) 0x20000040 )
#define TAS_TRANS_TAGOUT   ((long) 0x20000050 )
#define TAS_TRANS_TAGCAPA  ((long) 0x20000060 )
#define TAS_TRANS_TAGUSED  ((long) 0x20000070 )
#define TAS_TRANS_USED     ((long) 0x20000080 )
#define TAS_CAPA_USED      ((long) 0x20000090 )
#define TAS_TRANS_PARA     ((long) 0x200000a0 )
#define TAS_TRANS_BLDUP    ((long) 0x200000b0 )
#define TAS_TRANS_BLDDOWN  ((long) 0x200000c0 )
#define TAS_TRANS_INOUT    ((long) 0x200000d0 )
#define TAS_TRANS_SWITCH   ((long) 0x200000e0 )
#define TAS_TRANS_LINK     ((long) 0x200000f0 )
#define TAS_MARK_LOTRS_CAPAI ((long) 0x200000f1 )

/* mots clefs des champs USER des losig */
#define TAS_ONACTIVEPATH   ((long) 20060321)
#define TAS_SIGNAL_LEVEL   ((long) 20060324)
#define TAS_SIG_VOLTAGE    ((long) 20061121)

/* mots clefs des champs USER des cones */
#define TAS_SLOPE_MIN      ((long) 0x20000100 )
#define TAS_SLOPE_MAX      ((long) 0x20000200 )
#define TAS_INCONE         ((long) 0x20000300 )
#define TAS_IN_BACK        ((long) 0x20000400 )
#define TAS_SIG_CONE       ((long) 0x20000500 )
#define TAS_RC_CONE        ((long) 0x20000600 )
#define TAS_RC_MAXHH       ((long) 0x20000700 )
#define TAS_RC_MAXLL       ((long) 0x20000800 )
#define TAS_RC_MINHH       ((long) 0x20000900 )
#define TAS_RC_MINLL       ((long) 0x20000a00 )
#define TAS_RC_INDEX       ((long) 0x20000b00 )
#define TAS_CONE_NAME      ((long) 0x20000c00 )
#define TAS_CONE_BASENAME  ((long) 0x20000d00 )
#define TAS_CONE_MARQUE    ((long) 0x20000e00 )
#define TAS_DELAY_UP_MAX   ((long) 0x20000f00 )
#define TAS_DELAY_DOWN_MAX ((long) 0x20001000 )
#define TAS_DELAY_UP_MIN   ((long) 0x20001100 )
#define TAS_DELAY_DOWN_MIN ((long) 0x20001200 )
#define TAS_OUT_CARAC_MIN  ((long) 0x20001400 )
#define TAS_OUT_CARAC_MAX  ((long) 0x20001500 )
#define TAS_DISABLE_GATE_DELAY     0x20001501
#define TAS_VDD_NOTDEG             0x20001502
#define TAS_LEAKAGE                0x20001503

/* mots clefs des champs USER des cnsfigs */
#define TAS_ALL_DISABLE_GATE_DELAY     0x20001501

/* defines utilises pour masquage pour link */
#define TAS_INCONE_NINOUT  ((long) 0x10000000 )

/* mots clefs des champs USER des incone */
#define TAS_DELAY_MIN      ((long) 0x20001000 )
#define TAS_DELAY_MAX      ((long) 0x20002000 )
#define TAS_IN_CONESYM     ((long) 0x20003000 )
#define TAS_RC_LOCON       ((long) 0x20004000 )
#define TAS_SIG_INCONE     ((long) 0x20005000 )
#define TAS_DELAY_MINT     ((long) 0x20006000 )
#define TAS_DELAY_MAXT     ((long) 0x20007000 )
#define TAS_IN_RCIN        ((long) 0x20008000 )
#define TAS_INCONE_NAME    ((long) 0x20009000 )

/* mots clefs des champs USER des branch */
#define TAS_RESIST         ((long) 0x20010000 )

/* defines utilises pour masquage pour link */
#define TAS_LINK_BL        ((long) 0x10000000 )
#define TAS_LINK_INOUT     ((long) 0x20000000 )

/* mots clefs des champs USER des link */
#define TAS_LINK_CARAC     ((long) 0x20100000 )
#define TAS_LINK_BLDUP     ((long) 0x20200000 )
#define TAS_LINK_BLDDOWN   ((long) 0x20300000 )
//#define TAS_LINK_DELAYRC   ((long) 0x20400000 )
//#define TAS_LINK_RESISRC   ((long) 0x20500000 )
#define TAS_LINK_CAPA      ((long) 0x20600000 )
#define TAS_LINK_VPOL      ((long) 0x20700000 )
#define TAS_LINK_UNUSED_SWITCH_COMMAND ((long) 0x20800000 )

/* marquage des branches contenant des switch */
#define TAS_SWITCH         ((long) 0x10000000 ) /* part of switch         */
#define TAS_SWITCH_DEG     ((long) 0x40000000 ) /* switch degraded        */

/* marquage des branches et des incone */
#define TAS_PATH_INOUT     ((long) 0x01000000 ) /* branches INOUT          */
#define TAS_EXT_IN         ((long) 0x02000000 ) 
#define TAS_EXT_OUT        ((long) 0x04000000 ) 
#define TAS_IN_MEMSYM      ((long) 0x08000000 ) 
#define TAS_IN_NOTMEMSYM   ((long) 0x00100000 ) 
#define TAS_NORC_INCONE    ((long) 0x00200000 ) 
#define TAS_RCUU_INCONE    ((long) 0x00400000 ) 
#define TAS_RCDD_INCONE    ((long) 0x00800000 ) 
#define TAS_NORCT_INCONE   ((long) 0x80000000 ) 
#define TAS_INCONE_MASK    ((long) 0x000FFFFF ) /* initialisation du type*/

/* defines utilises pour masquage pour cones */
#define TAS_MARQUE         ((long) 0x10000000 )
#define TAS_DEJAEMPILE     ((long) 0x20000000 )
#define TAS_NOPRECH        ((long) 0x40000000 ) /* noeud connecteur  */
#define TAS_CONE_CONNECT   ((long) 0x80000000 ) /* pour typer un cone con*/
#define TAS_LCOMMAND       ((long) 0x01000000 )
#define TAS_PRECHARGE      ((long) 0x02000000 ) /* noeud precharge   */
#define TAS_INTER          ((long) 0x04000000 ) /* noeud precharge   */
#define TAS_BREAK          ((long) 0x08000000 ) /* noeud precharge   */
#define TAS_BYPASSOUT      ((long) 0x00100000 ) /* cone non-important OUT*/
#define TAS_BYPASSIN       ((long) 0x00200000 ) /* cone non-important IN */
#define TAS_CONE_INV       ((long) 0x00400000 ) /* cone inverseur        */
#define TAS_CONE_BUF       ((long) 0x00800000 ) /* cone buffer           */
#define TAS_RLCOMMAND      ((long) 0x00010000 )
#define TAS_ECONE          ((long) 0x00020000 ) 
#define TAS_NORISING       ((long) 0x00040000 ) 
#define TAS_NOFALLING      ((long) 0x00080000 ) 
#define TAS_CONE_MASK      ((long) 0x0000FFFF ) /* initialisation du type*/

/* defines utilises pour figure hierarchique */
 
#define TAS_ALIM           ((char) 'A')         /* signal alimentation       */

/* mots clefs des champs USER de la figure hierarchique */

#define TAS_SIG_LOCON      ((long) 0x40000001 ) /* signal => connecteur      */
#define TAS_LOCON_SIG      ((long) 0x40000002 ) /* connecteur => signal      */
#define TAS_SIG_NEWSIG     ((long) 0x40000003 ) /* signal <=> signal         */
#define TAS_LOCON_NSIG     ((long) 0x40000004 ) /* connecteur => signal      */
#define TAS_DELAY_NNRC     ((long) 0x40000005 ) /* delay rc entre NN         */
#define TAS_LOSIG_BUF      ((long) 0x40000006 ) /* info du .inf              */
#define TAS_SIG_BYPASSIN   ((long) 0x40000007 ) /* info du .inf              */
#define TAS_SIG_BYPASSOUT  ((long) 0x40000008 ) /* info du .inf              */
#define TAS_SIG_INTER      ((long) 0x40000009 ) /* info du .inf              */
#define TAS_SIG_ELM        ((long) 0x4000000a ) /* info du .inf              */
#define TAS_SIG_RCN        ((long) 0x4000000b ) /* info du .inf              */
#define TAS_SIG_RC         ((long) 0x4000000d ) /* info du .inf              */
#define TAS_SIG_NORC       ((long) 0x4000000e ) /* info du .inf              */
#define TAS_SIG_NORCDELAY  ((long) 0x4000000f ) /* info du .inf              */
#define TAS_LOFIGCHAIN     ((long) 0x40000010 ) /* info du .inf              */
#define TAS_SIG_LOSIG      ((long) 0x40000011 ) /* info du .inf              */
#define TAS_SIG_DRV        ((long) 0x40000012 ) /* info du .inf              */
#define TAS_SIG_VDD        ((long) 0x40000013 ) /* info du .inf              */
#define TAS_SIG_VSS        ((long) 0x40000014 ) /* info du .inf              */
#define TAS_SIG_ALIM       ((long) 0x40000015 ) /* info du .inf              */
#define TAS_SIG_NAME       ((long) 0x40000016 ) /* info du .inf              */
#define TAS_SIG_INDEX      ((long) 0x40000017 ) /* info du .inf              */
#define TAS_SIG_TERMINAL   ((long) 0x40000019 ) /* info du .inf              */
#define TAS_SIG_INITIAL    ((long) 0x40000020 ) /* info du .inf              */
#define TAS_SIG_MAXELM     ((long) 0x40000021 ) /* info du .inf              */
#define TAS_SIG_MINELM     ((long) 0x40000023 ) /* info du .inf              */
#define TAS_LOCON_NAME     ((long) 0x40000024 ) /* info du .inf              */
#define TAS_SIG_BREAK      ((long) 0x40000025 ) /* info du .inf              */
#define TAS_SIG_SET        ((long) 0x40000026 ) /* info du .inf              */
#define TAS_SIG_LOCONEXT   ((long) 0x40000027 ) /* info du .inf              */
#define TAS_SIG_BEG        ((long) 0x40000028 ) /* 1er sig du .inf           */
#define TAS_SIG_END        ((long) 0x40000029 ) /* dernier sig du .inf       */
#define TAS_SIG_HZ         ((long) 0x4000002a ) /* signal pouvant etre HZ    */
#define TAS_CONE_NUMBER    ((long) 0x4000002b ) /* numerotation des cone     */
#define TAS_SIG_NOD        ((long) 0x4000002c ) /* info du .inf              */
#define TAS_SIG_INTERFACE  ((long) 0x4000002d ) /* info du .inf              */
#define TAS_MODEL_HTAB     ((long) 0x4000002e ) /* table de hash pour tmodel */
#define TAS_SIG_CAPA       ((long) 0x4000002f ) /* capacite d'un signal      */
#define TAS_SIG_TYPE       ((long) 0x40000030 ) /* type d'un signal          */
#define TAS_LOCON_INLIST   ((long) 0x40000031 ) /* list des entree d'un locon*/
#define TAS_LOCON_OUTCONE  ((long) 0x40000032 ) /* list des entree d'un locon*/
#define TAS_SIG_ONLYEND    ((long) 0x40000033 ) /* list des entree d'un locon*/
#define TAS_FIG_LOSIGRCX   ((long) 0x40000034 ) /* list des entree d'un locon*/
#define TAG_CONE_OUTPUT_CAPACITANCE ((long) 0x40000035 ) // TAS_CAPAOUT/connecteur
#define TAS_SIG_FORCERCDELAY  ((long) 0x400000036 ) /* info du .inf              */

#define TAS_CASE           ((int)(mcc_use_multicorner() == 0 ) ? elpTYPICAL : (TAS_PATH_TYPE == 'm') ? elpBEST : elpWORST)

#define TP_R           0
#define TP_A           1
#define TP_B           2
#define TP_RT          3
#define TP_VT          4
#define TP_deg         5
#define TP_VDDmax      6
#define TP_SEUIL       7
#define TP_RDD         8
#define TP_KDD         9
#define TP_RDF        10
#define TP_RUD        11
#define TP_KUD        12
#define TP_RUF        13
#define TP_KG         14
#define TP_Q          15
#define TP_K          16
#define TP_IFB        17
#define TP_CGP        18
#define TP_CGD        19
#define TP_CDS        20
#define TP_DL         21
#define TP_DW         22
#define TP_LMLT       23
#define TP_WMLT       24
#define TP_RS         25
#define TP_KT         26
#define TP_MULU0      27
#define TP_DELTAVT0   28
#define TP_CGDC       29
#define TP_CGSI       30
#define TP_CGSIC      31
#define TP_RACCS      32
#define TP_RACCD      33
#define TP_KRT        34
#define TP_CGPO       35
#define TP_CGPOC      36
#define TP_CGD0       37
#define TP_CGD1       38
#define TP_CGD2       39
#define TP_CGDC0      40
#define TP_CGDC1      41
#define TP_CGDC2      42
#define TP_VT0        43

/* les pseudo-fonctions */
#define TAS_GETCLINK(link)  ((caraclink_list *)getptype(link->USER, \
                              TAS_LINK_CARAC)->DATA)
#define TAS_GETWIDTH(link)  ((long)((caraclink_list *)getptype(link->USER, \
                                     TAS_LINK_CARAC)->DATA)->WIDTH)
#define TAS_GETLENGTH(link) ((long)((caraclink_list *)getptype(link->USER, \
                                     TAS_LINK_CARAC)->DATA)->LENGTH)
#define TAS_GETCAPARA(link) (((TAS_PATH_TYPE == 'M')||V_INT_TAB[__TAS_CAPARA_DEPTH].VALUE) ? \
                             ((double)((caraclink_list *)getptype(link->USER, \
                                     TAS_LINK_CARAC)->DATA)->CRAM) : (double)0)
#define TAS_GETCAPAEQ(link) ((double)((caraclink_list *)getptype(link->USER, \
                                     TAS_LINK_CARAC)->DATA)->CEQUI)
#define TAS_F1(a,b,r)       ((double)((a)*sqrt( (b)/((b)+((r)*(a))) )))
#define TAS_F2(a,b,r)       ((double)sqrt( (b)*((b)+((r)*(a))) ))
#define TAS_F3(a,b,i)       ((double) ((((i)*(b))+sqrt((i)*(b)*(i)*(b)+(4*(a)*(i)))) / (2*(a))) )
#define TAS_F4(u,vdd,fin)   ((double)((fin)*(u)/(vdd)))
#define TAS_F5(yp,yn,qp,qn) ((double)(sqrt(((yp)*(qp))/((yn)*(qn)))))
#define TAS_F6(u,b)         ((double)(0.5*(((1.0+(b))*(log(1.0+(u)))/(1.0-(b)))\
                              -log(1-(u))-2*(log(1+(b)*(u)))/((b)-(b)*(b)))))
#define TAS_TGH(x)          ((double)((exp(x)-exp((-(x))))/(exp(x)+exp((-(x))))))
#define TAS_GETCAPA(losig)  ((double)(getcapa(losig) * (float)1000.0))

typedef struct {
#ifndef USEOLDTEMP
  float        SLOPE ;         /* threshold for measure.               */
  float        VTH ;           /* voltage threshold for measure.       */
  
  float        VT ;            /* slope parameter for tanh model.      */
  float        VF ;
  float        VDD ;
  
  float        VSAT ;          /* slope parameter for the end of the   */
  float        RLIN ;          /* transition.                          */
  float        R, C1, C2 ;
#else
  double        SLOPE ;         /* threshold for measure.               */
  double        VTH ;           /* voltage threshold for measure.       */
  
  double        VT ;            /* slope parameter for tanh model.      */
  double        VF ;
  double        VDD ;
  
  double        VSAT ;          /* slope parameter for the end of the   */
  double        RLIN ;          /* transition.                          */
  double        R, C1, C2 ;
#endif
  char          MODEL ;
  char          VALID ;
} output_carac_trans ;

typedef struct {
  output_carac_trans    UP ;
  output_carac_trans    DW ;
} output_carac ;

/* STRUCTURES DE TAS */

typedef struct delay
        {
         long    TPLH,TPHL;
         long    TPLL,TPHH;
         long    FLH,FHL;
         long    FLL,FHH;
         long    RCLL,RCHH;
         long    FRCLL,FRCHH;
#ifdef USEOLDTEMP
         //--
         long    RLH,RHL;
         long    RLL,RHH;
         long    SLH,SHL;
         long    SLL,SHH;
         //--
#endif
         timing_model  *TMLH;
         timing_model  *TMHL;
         timing_model  *TMLL;
         timing_model  *TMHH;
         timing_model  *FMLH;
         timing_model  *FMHL;
         timing_model  *FMLL;
         timing_model  *FMHH;
#ifdef USEOLDTEMP
         stm_pwl       *PWLTPHL;
         stm_pwl       *PWLTPLH;
         stm_pwl       *PWLRCLL;
         stm_pwl       *PWLRCHH;
#endif
         output_carac  *CARAC;
        }
delay_list;

typedef struct {
  float R ;
  float V ;
} tas_driver ;

typedef struct front
        {
         long        FUP,FDOWN;
         stm_pwl    *PWLUP, *PWLDN;
         tas_driver  DRIVERUP, DRIVERDN ;
        }
front_list;

typedef struct caraccon
        {
         long      RUPMAX,RDOWNMAX;
         long      RUPMIN,RDOWNMIN;
         float     C;
		 double   *SLOPEIN ;
		 double   *CAPAOUT ;
		 int       NSLOPE ;
		 int       NCAPA ;
        }
caraccon_list;

typedef struct caraclink
        {
         long      WIDTH;
         long      LENGTH;
         float     CRAM;
         float     CEQUI;
         float     CLINK;
        }
caraclink_list;

typedef struct caracnode
        {
         long      FMAX;
         long      FMIN;
        }
caracnode_list;

typedef struct tpiv {
  stm_solver_maillon_list       *HEAD;
  chain_list                    *HEADTRSSPICE;
  chain_list                    *HEADTRSMCC;
} tpiv;

typedef struct tpiv_i {
  stm_solver_maillon_list       *HEAD;
  stm_solver_maillon_list       *ACTIVE;
  char                           TRTYPE;
  lotrs_list                    *TRS;
  float                          VDDMAX;
} tpiv_i;

typedef struct leakage {
  float L_UP_MAX;
  float L_UP_MIN;
  float L_DN_MAX;
  float L_DN_MIN;
} leakage_list;

typedef struct information /* informations relatives au circuit */
    {
     long lo_t     ; /* temps du chargement de le netlist logique */
     long db_t     ; /* temps du desassemblage                    */
     long el_t     ; /* temps du calcul des temps elementaires    */
     long gr_t     ; /* temps du parcours de graphe               */
     long pr_t     ; /* temps de generation du perfmodule         */
     long nb_trans ; /* nombre de transistors                     */
     long nb_cone  ; /* nombre de cones                           */
     long comp     ; /* complexite du circuit                     */
     long maxdelay ; /* delai max du bloc                         */
     long mindelay ; /* delai min du bloc                         */
     short argc    ; /* nombre d'arguments de la ligne de commande*/
     char **argv   ; /* arguments de la ligne de commande         */
    }
info_list ;

typedef struct tas_context
    {
     struct tas_context *NEXT           ;
     double      TAS_CAPARAPREC         ;
     double      TAS_CAPASWITCH         ;
     double      TAS_CAPAOUT            ;
     char        *TAS_FILENAME          ;
     char        *TAS_FILEIN            ;
     char        *TAS_FILEOUT           ;
     char        *TAS_TOOLNAME          ;
     char        *TAS_RC_FILE           ;
     double      TAS_LIMITMEM           ;
     int         TAS_TRACE_MODE         ;
     char        TAS_DEBUG_MODE         ;
     char        TAS_VALID_SCM          ;
     char        TAS_CURVE              ;
     float       TAS_CURVE_START        ;
     float       TAS_CURVE_END          ;
     char        TAS_DELAY_PROP         ;
     char        TAS_DELAY_SWITCH       ;
     char        TAS_NO_PROP            ;
     char        TAS_SHORT_MODELNAME    ;
     char        TAS_SIMU_CONE          ;
     char        TAS_LEVEL              ;
     char        TAS_PERFILE            ;
     char        TAS_FACTORISE          ;
     char        TAS_FIND_MIN           ;
     char        TAS_NB_VERSION[80]     ;
     char        TAS_DATE_VER[80]       ;
     char        TAS_SLOFILE            ;
     char        TAS_PERFINT            ;
     char        TAS_INT_END            ;
     char        TAS_NOTAS              ;
     char        TAS_DIF_LATCH          ;
     char        TAS_LANG               ;
     char        TAS_PWL                ;
     char        TAS_CNS_FILE           ;
     char        TAS_CNS_LOAD           ;
     char        TAS_CNS_ANNOTATE_LOFIG ;
     char        TAS_TREATPRECH         ;
     char        TAS_SILENTMODE         ;
     char        TAS_HIER               ;
     char        TAS_NHIER              ;
     char        TAS_CALCRCN            ;
     char        TAS_MERGERCN           ;
     char        TAS_SUPSTMMODEL        ;
     char        TAS_BREAKLOOP          ;
     char        TAS_SUPBLACKB          ;
     char        TAS_IGNBLACKB          ;
     char        TAS_FLATCELLS          ;
     char        TAS_CHECKRCDELAY       ;
     char        TAS_CARAC              ;
     char        TAS_CARAC_MEMORY       ;
     char        TAS_STABILITY          ;
     char        TAS_PRES_CON_DIR       ;
     int         TAS_CARACMODE          ;
     char        *TAS_TRANSINSNAME      ;
     char        TAS_SAVE_BEFIG         ;
     befig_list  *TAS_BEFIG             ;
     lofig_list  *TAS_HIERLOFIG         ;
     lofig_list  *TAS_LOFIG             ;
     cnsfig_list *TAS_CNSFIG            ;
     char        TAS_SAVERCN            ;
     char        TAS_CALCRCX            ;
     int         TAS_SAVE_OUT           ;
     int         TAS_SAVE_ERR           ;
     char        TAS_CHECK_PROP         ;
     double      FRONT_CON              ;
     double      FRONT_NOT_SHRINKED     ;
     int         CARAC_VALUES           ;
     list_list  *INF_SIGLIST;    
  }
tas_context_list;

typedef struct 
{
  chain_list *slopes, *capas;
  chain_list *result;        
  int maxthreads;
} tas_spice_charac_struct;

/* declaration de variable */
 
extern tas_context_list *TAS_CONTEXT;
extern info_list      TAS_INFO ;
extern char          *CELL ;
extern chain_list    *DETAILED_MODELS ;
extern int TAS_PVT_COUNT;


/* declaration de fonction */

extern void           tas_initcnsfigalloc   __P(( cnsfig_list* ));
extern void           tas_freecnsfigalloc   __P(( cnsfig_list* ));
extern void           tas_freecnsfig        __P(( cnsfig_list*  ));
extern void           tas_freelofig         __P(( lofig_list*  ));
extern int            tas_timing            __P(( cnsfig_list*, lofig_list *, 
                                                  ttvfig_list * 
                                               ));
extern int            tas_drislo            __P(( cnsfig_list*, ttvfig_list* ));
extern int            tas_error             __P(( short, char*, long ));
extern ttvfig_list*   tas_cns2ttv           __P(( cnsfig_list*, ttvfig_list *,
                                                  lofig_list *
                                               ));
extern double         tas_getparam          __P(( lotrs_list*, int, int)) ;
extern int            tas_capara            __P(( cnsfig_list* ));
extern int            tas_DetectConeConnect __P(( cnsfig_list* ));
extern long           tas_prelink           __P(( cnsfig_list* ));
extern int            tas_para              __P(( cone_list* ));
extern int            tas_switch_cmos       __P(( cone_list* ));
extern void           tas_AddCapaOut        __P(( cnsfig_list* ));
extern void           tas_AddCapaDiff       __P(( lofig_list* ));
extern int            tas_StuckAt           __P(( cnsfig_list* ));
extern list_list*     tas_GetAliasSig       __P(( lofig_list*, list_list* ));
extern int            tas_DifLatch          __P(( cnsfig_list* ));
extern int            tas_TechnoParameters  __P(( void ));
extern int            tas_tpd               __P(( cone_list* ));
extern int            tas_PwlFile           __P(( void ));
extern int            tas_setenv            __P(( void ));
extern void           tas_restore           __P(( void ));
extern void           tas_yaginit           __P(( void ));
extern int            tas_option            __P(( int, char**, char ));
extern int            tas_CloseTerm         __P(( void ));
extern int            tas_RestoreTerm       __P(( void ));
extern void           tas_GetKill           __P(( int ));
extern info_list      tas_InitInfo          __P(( struct information ));
extern int            tas_PrintTime         __P(( struct rusage,
                                                  struct rusage,
                                                  long
                                               ));
extern void           tas_visu              __P(( void ));
extern void           tas_init              __P(( void )) ;
extern ttvfig_list* tas_main __P(( void )) ;
extern ttvfig_list* tas_core __P(( int keeplofig )) ;
extern double*      tas_DynamicSlopes    __P((long, int));
extern double*      tas_DynamicCapas     __P((double, int));
extern void           tas_handler_core      __P(( void ));
extern float          tas_gettotalcapa (lofig_list *lofig, losig_list *losig, int type);

// pour les API et surtout ttv_API
extern void        tas_simu_set_progression ( void (*callfn)( void*, char*, int ) );

extern double      tas_get_input_slope      ( ttvcritic_list* );

extern int         tas_simu_netlist         ( ttvfig_list*, lofig_list* ,
                                              cnsfig_list*,
                                              spisig_list*,
                                              chain_list*,
                                              char,
                                              int,
                                              char *,
                                              FILE *,
                                              tas_spice_charac_struct *tscs,
                                              int mcruns,
                                              int plot
                                            );
// pour les API et surtout ttv_API
extern ttvfig_list *tas_builtfig(lofig_list *lofig);
extern void tas_loconorient        (lofig_list *lofig, ttvfig_list *ttvfig);
extern void tas_setsigname         (lofig_list *lofig);
extern void tas_builtrcxview       (lofig_list *lofig, ttvfig_list *ttvfig);
extern void tas_detecloconsig      (lofig_list *lofig, ttvfig_list *ttvfig);
extern void tas_builtline          (ttvfig_list *ttvfig, long type);
extern void tas_cleanfig           (lofig_list *lofig, ttvfig_list *ttvfig);
extern int  tas_deleteflatemptyfig (ttvfig_list *ttvfig, long type, char flat);
extern void tas_detectloop         (ttvfig_list *ttvfig, long type);
extern void tas_calcfigdelay       (ttvfig_list *ttvfig, lofig_list *lofig, long type, long typefile);
extern void tas_calcrcxdelay       (lofig_list*, ttvfig_list*, long);
extern void tas_capasig            (lofig_list*);
extern void tas_detectinout        (cnsfig_list*);
extern int  tas_version            ();
extern int  tas_top_main           (int argc, char *argv[]);

// pour le solveur STM
extern timing_iv*          tpiv_dualmodel( cone_list *cone, branch_list *head, link_list *active, float vref, float pconf0, float pconf1, float fin_vi, float fin_vf, float fin_vs, float vouti );

extern char                tpiv_calc_i( tpiv_i *model, float ve, float vs, float *is );
extern tpiv*               tpiv_createbranch( link_list *head, char transtype );
extern void                tpiv_freebranch( tpiv *br );
extern tpiv*               tpiv_alloc( void );
extern void                tpiv_free( tpiv *s );
extern void                tpiv_carac_static( lotrs_list *lotrs, float vdd, float vdsmax, float vgsmax, float vb, float vs );
extern void                tas_tpiv_set_vg( stm_solver_maillon *maillon, char brtype, float vg );
extern void                tas_tpiv_set_vb( stm_solver_maillon *maillon, char brtype, float vb );
extern void                tas_update_mcctemp (inffig_list *ifl);
extern float tas_get_current_leakage_2( branch_list *branch, link_list *head, link_list *activelink );

int ttv_path_is_activable(ttvfig_list *tvf, cnsfig_list *cf, ttvpath_list *tc, chain_list *pathlist, char *divname, char *convname);
float tas_get_cone_output_capacitance(cone_list *cone);
ttvfig_list *hitas_tcl (char *figname);

#endif
