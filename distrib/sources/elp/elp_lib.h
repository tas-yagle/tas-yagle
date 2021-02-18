/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : ELP Verison 1                                               */
/*    Fichier : elp110.h                                                    */
/*                                                                          */
/*    (c) copyright 1991-1995 Laboratoire MASI equipe CAO & VLSI            */
/*    Tous droits reserves                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                                                                          */
/****************************************************************************/

#ifndef ELP
#define ELP

/* les inclusions */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include RCN_H
#include CNS_H

/* les definitions */
//extern int ELP_CAPA_LEVEL;
//extern int ELP_DRV_FILE;
extern int ELP_LOAD_FILE_TYPE;
extern int ELP_GENPARAM ;
//extern double ELP_MULU0_MARGIN;
//extern double ELP_DELVT0_MARGIN;
//extern double ELP_SA_MARGIN;
//extern double ELP_SB_MARGIN;
//EXTErn double ELP_SD_MARGIN;
extern double ELP_NF_MARGIN;
extern double ELP_M_MARGIN;
extern double ELP_NRS_MARGIN;
extern double ELP_NRD_MARGIN;

#define ELP_LOTRS_MODEL ((long) 0x70000000 ) // the lotrs will contain its elpmodel

#define elpBADLW   -1.0 

#define elperrorFCT  /* identification des fonctions du fichier elperror.c */
#define elpFCT       /* identification des fonctions du fichier elp.c      */

extern char elpLang ;
#define elpSTRINGSIZE    256       /* taille des chaines de caracteres */
#define elpENGLISH      'E'        /* langue anglaise                  */
#define elpFRENCH       'F'        /* langue francaise                 */
#define elpDEFLANG      elpENGLISH /* langue par defaut                */

/* les informations sur la technologie utilisee */ 
extern char   elpTechnoName[elpSTRINGSIZE] ;
extern char   elpTechnoFile[elpSTRINGSIZE] ;
extern char   elpGenTechnoFile[elpSTRINGSIZE] ;
extern double elpTechnoVersion             ;
extern char  *elpoldtechnofilename;
extern int    elpres_load;

/* les parametres du simulateur electrique */ 
extern char   elpEsimName[elpSTRINGSIZE] ;

/* les parametres generaux */
#define elpGENERALNUM 9 /* nombre de parametres du tableau General    */
extern double elpGeneral[elpGENERALNUM] ;
#define elpACM        0  /* Area Calulation Mode: affecte RLEV et ALEV pour AD,AS,PS,PD methode de calcul des capacites dynamiques  */
#define elpTEMP       1  /* temperature de simulation et d'analyse     */
#define elpSLOPE      2  /* front sur les connecteurs d'entree         */
#define elpGVDDMAX    3  /* tension d'alim general                     */
#define elpGVDDBEST   4  /* tension d'alim general best corner         */
#define elpGVDDWORST  5  /* tension d'alim general worst corner        */
#define elpGDTHR      6  /* seuil general                              */
#define elpGSHTHR     7  /* seuil front HAUT                           */
#define elpGSLTHR     8  /* seuil front BAS                            */

#define ELPMINVOLTAGE  (double)(-1.0)
#define ELPINITTHR     (double)(-1.0) 
#define ELPINITTEMP    (double)(-1000.0)
#define ELPMINTEMP     (double)(-274.0) 
#define ELPINITVBULK   (double)(-100.0) 
#define ELPMINVBULK    (double)(-90.0) 
#define ELPPRECISION   (double)(1.0e6)
#define ELPPRECISION2  (double)(1.0e12)
#define ELPINITVALUE   (double)(1.0e-31)
#define ELPMINVALUE    (double)(1.0e-30)

#define elpVoltage4UnusedLotrs (double)(1.0e-3)

/* types de transistors */
#define elpTRANSNUM      3         /* nombre de type de transistor     */
#define elpNMOS          0         /* pour les parametres du NMOS      */
#define elpPMOS          1         /* pour les parametres du PMOS      */
#define elpUNKNOWN       2         /*                                  */

/* types de transistors */
#define elpSLOPENUM      2         /* nombre de type de front          */
#define elpRISE          0         /* pour les parametres du RISE      */
#define elpFALL          1         /* pour les parametres du FALL      */

/* les parametres du shrink du transistor */
#define elpSHRINKNUM 7          /* nombre de parametres du tableau Shrink     */
#define elpLMLT     0           /* parametre multiplicatif de la longueur     */
#define elpWMLT     1           /* parametre multiplicatif de la largeur      */
#define elpDL       2           /* parametre de reduction de la longueur NMOS */
#define elpDW       3           /* parametre de reduction de la largeur NMOS  */
#define elpDLC      4           /* parametre de reduction de la longueur NMOS */
#define elpDWC      5           /* parametre de reduction de la largeur NMOS  */
#define elpDWCJ     6           /* parametre de reduction de la largeur NMOS  */

/* les parametres de validité du model */
#define elpRANGENUM 4          /* nombre de parametres du tableau range      */
#define elpLMIN     0           /* longueur min                              */
#define elpLMAX     1           /* longueur max                              */
#define elpWMIN     2           /* largeur min                               */
#define elpWMAX     3           /* largeur max                               */

/* les parametres du modele des transistors (modele MCC) */
#define elpMULU0    0   /* facteur de mobilite                     */
#define elpDELVT0   1   /* shrink du vth                           */
#define elpSA       2   /* specific instance for bs43              */
#define elpSB       3   /* specific instance for bs43              */
#define elpSD       4   /* specific instance for bs43              */
#define elpNF       5   /* specific instance, number of finger     */
#define elpM        6   /* number of parallel transistors          */
#define elpNRS      7   /* number of squares 4 source resistance   */
#define elpNRD      8   /* number of squares 4 drain resistance    */
#define elpSC       9   /* number of squares 4 drain resistance    */
#define elpSCA     10   /* number of squares 4 drain resistance    */
#define elpSCB     11   /* number of squares 4 drain resistance    */
#define elpSCC     12   /* number of squares 4 drain resistance    */
/*-----------------------------------------------------------------*\
 * CAUTION: All specific instance parameters defines must be declared
            before elpVT
\*-----------------------------------------------------------------*/
#define elpVT       13   /* tension de seuil                        */
#define elpA        14   /* parametre A                             */
#define elpB        15   /* parametre B                             */
#define elpRT       16   /* resistance en mode lineaire             */
#define elpRS       17   /* resistance en mode saturé               */
#define elpKT       18   /* resistance en mode saturé               */
#define elpKS       19   /* resistance en mode saturé               */
#define elpKR       20   /* resistance en mode saturé               */
#define elpKRT      21   /* vlin=krt*vdd                            */
#define elpKRS      22   /* vsat=krs*vdd                            */
#define elpVT0      23   /* VT techno                               */
#define elpMODELNUM 24   /* nombre de parametres pour chaque modele  */

/* les parametres du type et du nom de model                       */
#define elpMOS      0   /* type de model de transistor mosfet      */
 
/* les parametres du type et du nom de model                       */
#define elpNOINDEX  -1   /* pas d'index                            */

/* les parametres max ou min  | best typical worst                 */
#define elpWORST    0   /* delai max                               */
#define elpBEST     1   /* delai min                               */
#define elpTYPICAL  2   /* tous les delais                         */

/* les parametres definissant les tensions */
#define elpVOLTNUM 4   /* nombre d'elements du tableau Voltage */
#define elpVDEG    0   /* tension du niveau degrade haut       */
#define elpVDDMAX  1   /* Tension d'alimentation               */
#define elpVBULK   2   /* Tension du bulk                      */
#define elpVTI     3   /* tension intermediaire dans 1 branche */

/* les parametres definissant les capacites dynamiques */
#define elpCGS      0   /* capacite grille contribution en surface   */
#define elpCGS0     1   /* capacite grille contribution en surface pour vds=0  */
#define elpCGSU     2   /* capacite grille contribution en surface for up transistion*/
#define elpCGSU0    3   /* capacite grille contribution en surface for up transistion pour vds=0*/
#define elpCGSUMIN  4   /* capacite grille contribution en surface for up transistion*/
#define elpCGSUMAX  5   /* capacite grille contribution en surface for up transistion*/
#define elpCGSD     6   /* capacite grille contribution en surface for down transition*/
#define elpCGSD0    7   /* capacite grille contribution en surface for down transition pour vds=0*/
#define elpCGSDMIN  8   /* capacite grille contribution en surface for down transition*/
#define elpCGSDMAX  9   /* capacite grille contribution en surface for down transition*/
#define elpCGP      10   /* capacite grille contribution en perimetre */
#define elpCGPUMIN  11   /* capacite grille contribution en perimetre */
#define elpCGPUMAX  12   /* capacite grille contribution en perimetre */
#define elpCGPDMIN  13  /* capacite grille contribution en perimetre */
#define elpCGPDMAX  14  /* capacite grille contribution en perimetre */
#define elpCGD      15  /* capacite grille drain surfacique          */
#define elpCGDC     16  /* capacite grille drain surfacique conflit  */
#define elpCGSI     17  /* capacite grille drain surfacique          */
#define elpCGSIC    18  /* capacite grille drain surfacique conflit  */
#define elpCDS      19  /* capacite drain contribution en surface    */
#define elpCDSU     20  /* capacite drain contribution en surface    */
#define elpCDSD     21  /* capacite drain contribution en surface    */
#define elpCDP      22  /* capacite drain contribution en perimetre  */
#define elpCDPU     23  /* capacite drain contribution en perimetre  */
#define elpCDPD     24  /* capacite drain contribution en perimetre  */
#define elpCDW      25  /* capacite drain contribution en largeur    */
#define elpCDWU     26  /* capacite drain contribution en largeur    */
#define elpCDWD     27  /* capacite drain contribution en largeur    */
#define elpCSS      28  /* capacite source contribution en surface   */
#define elpCSSU     29  /* capacite source contribution en surface   */
#define elpCSSD     30  /* capacite source contribution en surface   */
#define elpCSP      31  /* capacite source contribution en perimetre */
#define elpCSPU     32  /* capacite source contribution en perimetre */
#define elpCSPD     33  /* capacite source contribution en perimetre */
#define elpCSW      34  /* capacite source contribution en largeur   */
#define elpCSWU     35  /* capacite source contribution en largeur   */
#define elpCSWD     36  /* capacite source contribution en largeur   */
#define elpCGPO     37
#define elpCGPOC    38
#define elpCGSUF    39
#define elpCGSDF    40
#define elpCGD0     41
#define elpCGD1     42
#define elpCGD2     43
#define elpCGDC0    44
#define elpCGDC1    45
#define elpCGDC2    46
#define elpCAPANUM  47  /* nombre de capacites carrees               */

/* Les parametres pour le calcul de la résistance en petits signaux */
#define elpRSSNUM  3   /* Nombre de parametres */
#define elpRSSL    0   /* Coefficient du L */
#define elpRSSC    1   /* Constante        */
#define elpRSSW    2   /* Coefficient du W */

/* Parameter for access resistance */
#define elpRACCNUM 2   /* Number of parameter     */
#define elpRACCS   0   /* access drain resistance */
#define elpRACCD   1   /* access drain resistance */

#define ELPMAXLONG (long)(0x7FFFFFFF)

#define elpNoTechno 0
#define elpBSIM3V3  1
#define elpBSIM4    2
#define elpPSP      3
#define elpMOS2     4
#define elpMM9      5
#define elpEXTMOD   6

extern char *elpTabTechnoName[];

typedef struct elpmodel
  {
   struct elpmodel *NEXT ;
   int             elpModelIndex ;                /* Index du Model          */
   int             elpTransType ;                 /* NMOS PMOS               */
   int             elpTransModel ;                /* MOS                     */
   int             elpTransIndex ;                /* INDEX                   */
   int             elpTransCase ;                 /* BEST WORST TYPICAL      */
   int             elpTransTechno ;
   char            *elpModelName ;                /* NOM                     */
   chain_list      *elpModelNameAlias;            /* Alias de nom            */
   double          elpShrink[elpSHRINKNUM] ;      /* DL DW LMLT WMLT         */
   long            elpRange[elpRANGENUM] ;        /* LMIN LMAX WMIN WMAX     */
   double          elpModel[elpMODELNUM] ;        /* A B RT VT RS            */
   double          elpVoltage[elpVOLTNUM] ;       /* VDDMAX VDEG             */
   double          elpTemp ;                      /* TEMP                    */
   double          elpCapa[elpCAPANUM] ;          /* CAPA                    */
   int             elpSWJUNCAP ;                  /* junction capacitance model (psp only) */
   double          elpRss[elpRSSNUM];             /* RSSL RSSC RSSW          */
   double          elpRacc[elpRACCNUM];           /* RS and RD               */
   chain_list *longkey;
  }
elpmodel_list;

#define elpNameLevel       (long)0
#define elpLengthLevel     (long)1
#define elpWidthLevel      (long)2
#define elpVDDLevel        (long)3
#define elpMULU0Level      (long)4
#define elpDELVT0Level     (long)5
#define elpSALevel         (long)6
#define elpSBLevel         (long)7
#define elpSDLevel         (long)8
#define elpNFLevel         (long)9
#define elpVBULKLevel      (long)10
#define elpMLevel          (long)11
#define elpNRSLevel        (long)12
#define elpNRDLevel        (long)13
#define elpSCLevel         (long)14
#define elpSCALevel        (long)15
#define elpSCBLevel        (long)16
#define elpSCCLevel        (long)17

#if 1
// activation du paramcontext pour le share des modeles
#define elpParamContextLevel        (long)18
#define elpCaseLevel       (long)19
#else
// desactivation du paramcontext pour le share des modeles
#define elpParamContextLevel        (long)19
#define elpCaseLevel       (long)18
#endif

#define ELP_APPROX_EXCLUDE_FOR_KEY


/* defines of different kind of input capacitances */ 
#define ELP_CAPA_LEVEL0  (int)0  // 1 capa  : - CGS 
#define ELP_CAPA_LEVEL1  (int)1  // 2 capas : - CGSU & CGSD => CGS = (CGSU+CGSD)/2
#define ELP_CAPA_LEVEL2  (int)2  // 6 capas : - CGSU & CGSD => CGS = (CGSU+CGSD)/2
                                 //           - CGSUMIN & CGSUMAX
                                 //           - CGSDMIN & CGSDMAX

#define ELP_CAPA_TYPICAL (int)0  // (cgu + cgd) / 2.0
#define ELP_CAPA_UP      (int)1  // cgu computed for input transition [0:vdd/2]
#define ELP_CAPA_UP_MIN  (int)2  // cgu computed for input transition [10%:90%]
#define ELP_CAPA_UP_MAX  (int)3  // cgu computed for input transition [10%:90%]
#define ELP_CAPA_DN      (int)4  // cgd computed for input transition [0:vdd/2]
#define ELP_CAPA_DN_MIN  (int)5  // cgd computed for input transition [10%:90%]
#define ELP_CAPA_DN_MAX  (int)6  // cgd computed for input transition [10%:90%]
#define ELP_CAPA_UPF     (int)7  // cgu computed for input transition [0:vdd/2]
#define ELP_CAPA_DNF     (int)8  // cgu computed for input transition [0:vdd/2]

#define ELP_NOCAPA        ((float)-3.14159e11)
#define ELP_LOFIG_TABCAPA 20021108
#define ELP_CAPASWITCH    20040924

#define ELP_LOFIG_CAPAS_ID (long)(0x77700007) // to know if the lofig was already annoted
                                              // with the elpCapaGrid and elpCapaDiff

#define ELP_LOADELP_FILE      (int)0
#define ELP_DONTLOAD_FILE     (int)1
#define ELP_DEFAULTLOAD_FILE  (int)2

struct elpcapa {
  int    NBSIG;
  float *TAB;
  float *TAB_UP;
  float *TAB_UP_MIN;
  float *TAB_UP_MAX;
  float *TAB_DN;
  float *TAB_DN_MIN;
  float *TAB_DN_MAX;
};


typedef struct elpcapaswitch 
  {
  float CUP;
  float CUPMIN;
  float CUPMAX;
  float CDN;
  float CDNMIN;
  float CDNMAX;
  }
elpcapaswitch_list;

/* tree structure of elp model to speed the research */
typedef struct elptree
  {
   struct elptree *NEXT ;
   long            DATA1 ;
   long            DATA2 ;
   int             LEVEL ;
   void           *DOWN ;
  }
elptree_list;

/* les parametres definissant les parametres specifiques des instances du lotrs*/
#define elpPARAMNUM 13   /* nombre d'elements specifiques des instances du lotrs*/

enum mcc_saved_enum
{
   __MCC_SAVED_VTH0,
   __MCC_SAVED_K2,
   __MCC_SAVED_VFB,
   __MCC_LAST_VBS,
   __MCC_LAST_VGS,
   __MCC_LAST_VDS,
   __MCC_SAVED_VTHFINAL,
   __MCC_LAST_VBSEFF,
   __MCC_LAST_VGSTEFF,
   __MCC_SAVED_VDSAT,
   __MCC_SAVED_ESAT,
   __MCC_SAVED_UEFFT,
   __MCC_SAVED_ABULK,
   __MCC_SAVED_RDS,
   __MCC_SAVED_VSATT,
   __MCC_SAVED_LAMBDA,
   __MCC_SAVED_RFACTOR,
   __MCC_SAVED_WEFFV,
   __MCC_SAVED_VTHFINAL_UNBIAS,
   __MCC_SAVED_NEED_STRESS,
   __MCC_LAST_VBS_FORVBSEFF,
   __MCC_SAVED_VBSEFF,
   __MCC_SAVED_CALC_THETA0,
   __MCC_SAVED_CALC_DELT_VTH,
   __MCC_SAVED_CALC_DELT_VTHW,
   __MCC_SAVED_CALC_DIBL_SFT_DVD,
   __MCC_SAVED_CALC_DVTH0T,
   __MCC_LAST_SAVED
};

typedef struct {
  double   PARAM[elpPARAMNUM];
  double   VBULK;
  double   VBS;
  char     ISVBSSET;
  chain_list *longkey;
  double   MCC_SAVED[__MCC_LAST_SAVED];
  char *SUBCKTNAME;
} elp_lotrs_param;

/* liste des modeles */ 
extern int ELPLINE ;

/* liste des modeles */ 
extern elpmodel_list *ELP_MODEL_LIST ;
extern elptree_list *ELP_HEAD_TREE ;
extern FILE *elpin ;
extern int ELP_CALC_ONLY_CAPA;

// Modification de la netlist
#define ELP_HACK_NONE            0
#define ELP_HACK_FIX_CAPA_TO_GND 1
#define ELP_HACK_FIX_CAPA        2

/* les fonctions externes */
extern int             elpDouble2int          (double value);
extern long            elpDouble2Long         (double value);
extern elpmodel_list  *elpAddModel            (char          *name,
                                               chain_list    *name_alias,
                                               int            type,
                                               int            index,
                                               long           lmin,
                                               long           lmax,
                                               long           wmin,
                                               long           wmax,
                                               double         dl,
                                               double         dw,
                                               double         ml,
                                               double         mw,
                                               double         vdd,
                                               int            trcase,
                                               int            trlevel,
                                               double         mulu0,
                                               double         delvt0,
                                               double         sa,
                                               double         sb,
                                               double         sd,
                                               double         nf,
                                               double         m,
                                               double         nrs,
                                               double         nrd,
                                               double         vbulk,
                                               double         sc,
                                               double         sca,
                                               double         scb,
                                               double         scc,
                                               chain_list *longkey);
extern short           elpError               (short          errnum, ...);
extern elptree_list   *elpBuiltTreeModel      (elptree_list  *head,
                                               elpmodel_list *model,
                                               int            level);
extern void            elpFreeTreeModel       (elptree_list  *head,
                                               int            level);
extern void            elpPrintTreeModel      (elptree_list  *head,
                                               int            level);
extern elpmodel_list  *elpSearchOrGenModel (lotrs_list      *lotrs,
                                            double           vdd,
                                            int              transcase,
                                            double           mulu0,
                                            double           delvt0,
                                            double           sa,
                                            double           sb,
                                            double           sd,
                                            double           nf,
                                            double           m,
                                            double           nrs,
                                            double           nrd,
                                            double           vbulk,
                                            double           sc,
                                            double           sca,
                                            double           scb,
                                            double           scc
                                            );
extern elpmodel_list  *elpGetModelWithLotrsParams (lotrs_list      *lotrs,
                                                   elp_lotrs_param *params,
                                                   double           vdd,
                                                   int              transcase);
extern elpmodel_list  *elpGetModel            (lotrs_list    *lotrs,
                                               double         vdd,
                                               int            transcase);
extern elpmodel_list  *elpGetParamModel       (char          *name,
                                               double         l,
                                               double         w,
                                               char           type,
                                               double         vdd,
                                               int            transcase,
                                               elp_lotrs_param *params);
extern elpmodel_list  *elpGetModelType        (char           typet);
extern elpmodel_list  *elpGetModelIndex       (int            index);
extern int             elpenv                 (void);         
extern int             elpVerifModel          (elpmodel_list *model);
extern short           elpLoadElp             (void);         
extern void            elpDriveElp            (char          *filename);
extern void            elpLotrsShrink         (lotrs_list    *lotrs,int transcase);
extern float           elpLotrsCapaDrain      (lotrs_list    *lotrs,int capatype,int transcase);
extern float           elpLotrsCapaSource     (lotrs_list    *lotrs,int capatype,int transcase);
extern float           elpLotrsCapaGrid       (lotrs_list    *lotrs,int transcase);
extern float           elpLotrsInCapa         (lofig_list    *lofig, lotrs_list    *lotrs, int capatype, int transcase, int correct );
extern void            elpLofigCapaDiff       (lofig_list    *lofig,int transcase);
extern void            elpLofigAddCapas       (lofig_list    *lofig,int transcase);
extern void            elpLofigCapaGrid       (lofig_list    *lofig, int transcase);
extern double          elpLotrsResiCanal      (lotrs_list    *lotrs, int transcase);
extern void            elpLofigShrink         (lofig_list    *lofig,int transcase);
extern double          elpScm2Thr             (double         f,
                                               double         smin,
                                               double         smax,
                                               double         Vt,
                                               double         Vfinal,
                                               double         Vdd,
                                               int            type);
extern double          elpThr2Scm             (double         f,
                                               double         smin,
                                               double         smax,
                                               double         Vt,
                                               double         Vfinal,
                                               double         Vdd,
                                               int            type);
extern void          (*elpSetDefaultParam)    ( void );
extern void            elpdefaultfct2         ( void );
extern void            elpSetFct2             (void         (*f)(void));
extern void          (*elpGenParam)           (char          *tname,
                                               int            type,
                                               double         l,
                                               double         w,
                                               double         vdd,
                                               lotrs_list    *lotrs,
                                               int           transcase,
                                               elp_lotrs_param *ptmodel_inst);
extern void            elpdefault             (char*          tname,
                                               int            type,
                                               double         l,
                                               double         w,
                                               double         vdd,
                                               lotrs_list    *lotrs,
                                               int            transcase,
                                               elp_lotrs_param *ptmodel_inst);
extern void            elpSetFct              (void         (*f)(char*  p0,
                                                                 int    p1,
                                                                 double p2,
                                                                 double p3,
                                                                 double p4,
                                                                 lotrs_list    *lotrs,
                                                                 int    transcase,
                                                                 elp_lotrs_param *ptmodel_inst));
extern double (*elpCalcIleak)(char* tname, int type, double l, double w,
                       lotrs_list *lotrs, int transcase, 
                       double vgs, double vds, double vbs,
                       double AD, double PD, double AS, double PS,
                       double *BLeak, double *DLeak, double *SLeak,
                       elp_lotrs_param *ptlotrs_param) ;

extern double          elpLeakDfltFct         (char*          tname,
                                               int            type,
                                               double         l,
                                               double         w,
                                               lotrs_list    *lotrs,
                                               int            transcase,
                                               double         vgs,
                                               double         vds,
                                               double         vbs,
                                               double         AD,
                                               double         PD,
                                               double         AS,
                                               double         PS,
                                               double        *BLeak,
                                               double        *DLeak,
                                               double        *SLeak,
                                               elp_lotrs_param *ptmodel_inst);
extern void            elpSetLeakFct           (double       (*f)(char*  p0,
                                                                  int    p1,
                                                                  double p2,
                                                                  double p3,
                                                                  lotrs_list    *lotrs,
                                                                  int    transcase,
                                                                  double p4,
                                                                  double p5,
                                                                  double p6,
                                                                  double p7,
                                                                  double p8,
                                                                  double p9,
                                                                  double p10,
                                                                  double *Bleak,
                                                                  double *Dleak,
                                                                  double *Sleak,
                                                                  elp_lotrs_param *ptmodel_inst));
extern int             elpLoadOnceElp         (void);         
extern float           elpGetCapaFromLocon    (locon_list    *locon, int capatype, int transcase);
extern float           elpGetCapaSig          (lofig_list    *lofig,
                                               losig_list    *losig,
                                               int            type);
extern float           elpGetTotalCapaSig     (lofig_list    *lofig,
                                               losig_list    *losig,
                                               int            type);
extern void            elpSetCapaSig          (lofig_list    *lofig,
                                               losig_list    *losig,
                                               float          capa,
                                               int            type);
extern char            elpIsCapaSig           (lofig_list    *lofig,
                                               losig_list    *losig,
                                               int            type);
extern float           elpAddCapaSig          (lofig_list    *lofig,
                                               losig_list    *losig,
                                               float          capa,
                                               int            type);
extern void            elpFreeCapaLofig       (lofig_list    *lofig);
extern void            elpLofigUnShrink       (lofig_list *lofig, int transcase);
extern void            elpLotrsUnShrink       (lotrs_list *lotrs, int transcase);
extern void            elpLotrsGetUnShrinkDim (lotrs_list *lotrs,
                                               long           length,
                                               long           width,
                                               long          *length_s,
                                               long          *width_s,
                                               int            transcase);
extern void            elpLotrsGetShrinkDim   (lotrs_list    *lotrs,
                                               long          *length_s,
                                               long          *width_s,
                                               long          *xs_s,
                                               long          *xd_s,
                                               long          *ps_s,
                                               long          *pd_s,
                                               long          *ptlactive,
                                               long          *ptwactive,
                                               int            transcase
                                               );
extern void            elpGetDeltaShrinkWidth (lotrs_list    *lotrs,
                                               long          *ptdeff,
                                               long          *ptdactive,
                                               long          *ptwmlt,
                                               int            transcase
                                               );
extern long            elpGetShrinkedLength   (lotrs_list    *lotrs,
                                               elpmodel_list *model);
extern long            elpGetShrinkedWidth    (lotrs_list    *lotrs,
                                               elpmodel_list *model);
extern elp_lotrs_param *elp_lotrs_param_alloc (void);
extern void             elp_lotrs_param_free (elp_lotrs_param *ptmodel_inst);
extern elp_lotrs_param *elp_lotrs_param_create (lotrs_list *lotrs);
extern elp_lotrs_param *elp_lotrs_param_dup ( elp_lotrs_param *src );
extern void             elp_lotrs_param_get (lotrs_list *lotrs, 
                                             double *ptmulu0,
                                             double *ptdelvt0,
                                             double *ptsa,
                                             double *ptsb,
                                             double *ptsd,
                                             double *ptnf,
                                             double *ptm,
                                             double *ptnrs,
                                             double *ptnrd,
                                             double *ptvbulk,
                                             double *ptsc,
                                             double *ptsca,
                                             double *ptscb,
                                             double *ptscc
                                            );
extern int              elp_is_valcomprise ( long val1,
                                             long val2,
                                             double precision );
extern double           elp_scale_vth ( lotrs_list *lotrs, elpmodel_list *model );
extern double           elp_scale_a ( lotrs_list *lotrs, elpmodel_list *model );
extern void elpDriveOneModel (FILE *file, elpmodel_list *model);
extern void elpDriveModel (char *filename, elpmodel_list *model);
extern void elp_addlotrsalim(lotrs_list *lotrs, alim_list *alim);
extern int elp_dellotrsalim(lotrs_list *lotrs);
extern int elp_getlotrsalim(lotrs_list *lotrs, char type, float *alim);
extern void elpDelModel2Lotrs (lotrs_list *lotrs);
extern int elpSetModel2Lotrs (elpmodel_list *model, lotrs_list *lotrs);
extern elpmodel_list *elpGetModelFromLotrs (lotrs_list *lotrs);
extern double elpGetVddFromCorner ( lotrs_list *lotrs, int corner );
extern double elpCalcIleakage( lotrs_list *lotrs,
                               int lotrstype, 
                               int transcase, 
                               double vgs, double vds, double vbs,
                               double *BLeak, double *DLeak, double *SLeak
                               ) ;
extern void elpSetCalcPAFct( void(*f)(lotrs_list*,char*,int,int,double,elp_lotrs_param*,double*,double*,double*,double*) );
void fill_elp_lotrs_param( elp_lotrs_param *pt,
                           double mulu0,
                           double delvt0,
                           double sa,
                           double sb,
                           double sd,
                           double nf,
                           double m,
                           double nrs,
                           double nrd,
                           double vbulk,
                           double sc,
                           double sca,
                           double scb,
                           double scc,
                           lotrs_list *lt
                          );
int elp_is_same_paramcontext(chain_list *longkey0, chain_list *longkey1);
void elpHackNetlistCapa( lofig_list *lofig, int whatjob );
void elpHackNetlistResi( lofig_list *lofig );
void elpHack_AddCtcCapa( locon_list *locon1, locon_list *locon2, float capa );
void elpHack_AddCapa( lotrs_list *lotrs, losig_list *vss, char where, float capa );
int elpHack_GetNodeLocon( locon_list *locon );

#endif

