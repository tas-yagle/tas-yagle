/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Avertec                      */
/*                                                                          */
/*    Produit : EFG                                                         */
/*    Fichier : efg_lib.h                                                   */
/*                                                                          */
/*    (c) copyright 1991-2003 Avertec                                       */
/*    Tous droits reserves                                                  */
/*    Support : contact@avertec.com                                         */
/*                                                                          */
/*    Auteur(s) : Marc  KUOCH                                               */
/*                                                                          */
/****************************************************************************/
/* Extraction d'une lofig concernant un chemin                              */
/****************************************************************************/

#ifndef EFG
#define EFG

/*****************************************************************************/
/*     includes                                                              */
/*****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include INF_H
#include ELP_H
#include CNS_H

/*****************************************************************************/
/*     defines                                                               */
/*****************************************************************************/
#define EFG_WITH_RC        ((int) 1)         /* */
#define EFG_WITHOUT_RC     ((int) 0)         /* */

#define EFG_BLOCK_TRS      ((char) 'B')         /* */
#define EFG_GOOD_TRS       ((char) 'G')         /* */

#define EFG_OUT_CAPA       ((char) 'C')         /* */
#define EFG_OUT_TRANSISTOR ((char) 'T')         /* */
#define EFG_OUT_NONE       ((char) 'N')         /* */

#define EFG_ALIM            ((char) 'A')         /* signal alimentation       */
#define EFG_RISE            ((char) 'U')         /* signal alimentation       */
#define EFG_FALL            ((char) 'D')         /* signal alimentation       */
#define EFG_FIG_HTSIG       ((long) 0xFF000000 ) /* htab for signal          */
#define EFG_SIG_SET_ZERO    ((void*) 0x00000000 ) /*                          */
#define EFG_SIG_SET_ONE     ((void*) 0x00000001 ) /*                          */
#define EFG_SIG_SET_FALL    ((void*) 0x00000002 ) /*                          */
#define EFG_SIG_SET_RISE    ((void*) 0x00000003 ) /*                          */
#define EFG_SIG_SET_MASK    ((long) 0x00000fff ) /*                          */
#define EFG_SIG_SET_OUTPATH ((long) 0x00001000 ) /*                          */
#define EFG_SIG_SET_PRECHARGED ((long) 0x00002000 ) /*                          */

#define EFG_SIG_CONE       ((long) 0x11000000 )
#define EFG_CORRESP        ((long) 0x11000001 ) 

#define EFG_SIG_LOCON      ((long) 0x20000000 ) /* signal => connecteur      */
#define EFG_SIG_DRV        ((long) 0x20000001 ) /* info du .inf              */
#define EFG_SIG_VDD        ((long) 0x20000002 ) /* info du .inf              */
#define EFG_SIG_VSS        ((long) 0x20000003 ) /* info du .inf              */
#define EFG_SIG_ALIM       ((long) 0x20000004 ) /* info du .inf              */
#define EFG_SIG_TERMINAL   ((long) 0x20000005 ) /* info du .inf              */
#define EFG_SIG_INITIAL    ((long) 0x20000006 ) /* info du .inf              */
#define EFG_SIG_SET        ((long) 0x20000007 ) /* info du .inf              */
#define EFG_SIG_BEG        ((long) 0x20000008 ) /* 1er sig du .inf           */
#define EFG_SIG_END        ((long) 0x20000009 ) /* dernier sig du .inf       */
#define EFG_SIG_SET_NUM    ((long) 0x2000000a ) /*                           */
#define EFG_SIG_CLK        ((long) 0x2000000b ) /*                           */
#define EFG_FORCE_IC       ((long) 0x2000000c ) /*                           */
                                                //               locon
                                                // desc : correspondance orig 
                                                // et extrait
#define EFG_CONE_STUCK_GSP        0x2000000d                                      

#define EFG_SIG_SET_HIER   ((long)0x3F000000 ) /* */
#define EFG_SIG_BEG_HIER   ((long)0x3F000001 ) /* */
#define EFG_SIG_END_HIER   ((long)0x3F000002 ) /* */
#define EFG_INS_OUTPATH    ((long)0x3F000003 ) /* */
#define EFG_CONFIG_CONINS  ((long)0x3F000004 ) /* sig fig <=> sig ins */

#define EFG_INS_NO_COPY    ((long)0x40000000 ) // instance a ne pas dupliquer
#define EFG_INS_FIG        ((long)0x40000001 ) // special lofig linked to loins
#define EFG_INS_CTXT       ((long)0x40000002 ) // contexte de l'instance

#define EFG_SPISIG_ORG_NAME ((long)0x50000000 ) // Nom original du spisig s'il
                                                // a ete modifie entre temps
#define EFG_SPISIG_LOOP_SPISIG 0x50000001 // Nom original du spisig s'il

#define EFG_ALIAS_ORG  ((long)0x60000000 ) // Nom original des signaux / lotrs
#define EFG_CONE_FCT   ((long)0x70000000 ) // le cone est sur le critic path

#define EFG_LOCON_CAPA ((long)0x80000000 ) // equivalent capa for a locon
#define EFG_SIG2CONE   ((long)0x80000001 )  
#define EFG_SIG2CONE   ((long)0x80000001 )  

#define EFG_GOODRC_INPUT 1
#define EFG_GOODRC_OUTPUT 2

#define EFG_ADDITIONAL_IC 0xfab70801

#define EFG_ADDIC_FORCE_0  0x10
#define EFG_ADDIC_FORCE_1  0x20

#define EFG_SPISIG_CMDDIR_MASK  0x1
#define EFG_SPISIG_HZ_MASK 0x2
#define EFG_SPISIG_DELAY_TO_HZ_MASK 0x4

/*****************************************************************************/
/*     Global                                                                */
/*****************************************************************************/

/*****************************************************************************/
/*     structures                                                            */
/*****************************************************************************/
typedef struct spisig
        {
         struct spisig  *NEXT;
         char           *NAME;              // nom hierarchique du signal    
         char           *critic_NAME;       // nom dans le detail du chemin    
         losig_list     *SRCSIG;            // signal de la figure originale 
         losig_list     *DESTSIG;           // signal de la figure extraite  
         //chain_list     *LOCONRC;           // si RC on sig                  
         char           *INCONRC;           // si RC on sig                  
         char           *OUTCONRC;           // si RC on sig                  
         long INCONRC_NODE, OUTCONRC_NODE;  // node num from rcx                  
         int             MINDATE, NUM;               // num du signal pr le chemin    
         long            EVENT;             // type de transition du front   
         /*
         long            TPCKUP;            // temps de montee de ck         
         long            TPCKDN;            // temps de descente de ck       
         long            TCKSTAB;           // temps de stabilite de ck      
         */
         float           ADDCAPA;           // capa additionnelle sur le sig 
         float           INITVOLT;          // init de la tension du sig     
         float           VTHLOW;            // seuil de mesure low du front
         float           VTHHIGH;           // seuil de mesure high du front
         float           VTH;               // VT pour le front d entree
         float           VTHHZ;               // VT pour les hz
         char            START;             // 1 : c'est un noeud de départ, 0 : non
         char            END;               // 1 : c'est un noeud d'arrivée, 0 : non
         char            CLK;               // 1 : c'est un noeud sur un chemin de command, 0 : non
         char            GOODRC;
         char            HASRCLINE;
         char            ADDIC;
         ptype_list     *USER;   
         cone_list *CONE;
         char *LATCHCMD;
         char LATCHCMDDIR;
         char *PREVIOUS;
         int PATHNUM;
        }
spisig_list;

typedef struct efg_srcsig {
  struct efg_srcsig    *NEXT;
  struct efg_fullsig   *ROOT;
  int                   LEVEL;
  char                 *CONTEXT;
  losig_list           *LOSIG;
  lofig_list           *LOFIG;
  int                  *TABCONV;
} efg_srcsiglist ;

typedef struct efg_fullsig {
  struct efg_fullsig   *NEXT;
  efg_srcsiglist       *SIGLIST;
  losig_list           *DESTSIG;
} efg_fullsiglist;

typedef struct efg_ctx {
  struct efg_ctx *NEXT;
  ht             *LOCONS;
  ht             *SRCSIG;
  ht             *LOINSCOPIED;   // corresp locon fig source -> fig destination
} efg_ctxlist;

typedef struct efgglobal {
  lofig_list    *SRCFIG;
  lofig_list    *DESTFIG;
  losig_list    *VSSDESTFIG;
  losig_list    *VDDDESTFIG;
  ht            *HTCONTEXT;
  efg_ctxlist   *CONTEXT;
  spisig_list   *SPISIG;
  ptype_list    *ALIMS;
  chain_list    *ALLADDEDCONES;
} efg_global;

typedef struct
{
  void *DATA;
  long TYPE;
  int mindate, date;
  int flags;
  long innode, outnode;
  char *latchcmd, *previous;
  char latchcmddir;
} efg_path_event_info;
/*****************************************************************************/
/*     Global                                                                */
/*****************************************************************************/
extern efg_global *EFGCONTEXT;
extern char        EFG_CALC_EQUI_CAPA;
extern int         EFG_TRS_INDEX;
extern char *EFG_SET_FIG_NAME;
/*****************************************************************************/
/*     functions                                                             */
/*****************************************************************************/
extern char          *efg_getconename (losig_list *sig);
extern void           efg_remove_figure_alias( lofig_list *lofig );
extern void           efg_del_corresp_alias_trs (lotrs_list *trs);
extern char          *efg_get_origtrsname (lotrs_list *sig);
extern void           efg_del_corresp_alias (losig_list *sig);
extern char          *efg_get_origsigname (losig_list *sig);
extern locon_list    *efg_get_locon (lofig_list *fig, char *name);
extern lofig_list    *efg_get_fig2ins (loins_list *loins);
extern void           efg_set_fig2ins (loins_list *loins, lofig_list *lofig);
extern losig_list    *efg_get_ht_sig (lofig_list *fig,char *signame);
extern int            efg_sig_is_onpath (losig_list *sig,char *ctxt);
extern loins_list    *efg_get_hier_loins (lofig_list *currentfig,char *insname);
extern char           efg_SigIsAlim (losig_list *sig);
extern void           efg_dont_copy_ins (loins_list *loins);
extern char          *efg_revect (char *);
extern losig_list    *efg_getlosigcone (cone_list *);
extern locon_list    *efg_get_org_con(locon_list *);
extern lotrs_list    *efg_get_org_trs(lotrs_list *);
extern losig_list    *efg_get_org_sig(losig_list *);
extern locon_list    *efg_get_ext_con(locon_list *);
extern losig_list    *efg_get_ext_sig(losig_list *);
extern lotrs_list    *efg_get_ext_trs(lotrs_list *);
extern void          *efg_get_hier_sigptype (losig_list *sig,char *ctxt,long type);

extern locon_list    *efg_FindLocon         ( lofig_list*, char * );

extern void           efg_extract_fig       ( lofig_list  *lofig,
                                              lofig_list **figext,
                                              cnsfig_list *cnsfig,
                                              spisig_list *spisig,
											  chain_list **chaincone,
											  chain_list **chaininstance,
											  chain_list  *loins2drv,
											  chain_list  *lotrs2drv,
											  chain_list  *locon2drv,
											  chain_list  *losig2drv,
											  chain_list  *cone_onpath,
                                              chain_list  *usrlist,
                                              int          markfig
                                            );

// => spisig
extern spisig_list   *efg_GetSpiSigByName (spisig_list *head,char *name);
extern void           efg_UpdateSpiSigName (spisig_list *spisig, char *name);
extern losig_list    *efg_GetHierSigByName (lofig_list*,char*,char**,loins_list**,char);
extern chain_list    *efg_AddPathEvent      ( chain_list *, char*, char, int, int, int, long, long, char *, char, char *);
extern ptype_list    *efg_CreatePathByInf   ( list_list *);
extern spisig_list   *efg_BuildSpiSigList   ( lofig_list* , chain_list*, spisig_list *, char, int *,int);
extern spisig_list   *efg_BuildSpiSigFromInf (lofig_list*,list_list*);
extern spisig_list   *efg_GetSpiSigByNum    (spisig_list *,int );
extern spisig_list   *efg_GetSpiSig         (spisig_list *,char *);
extern spisig_list   *efg_GetFirstSpiSig (spisig_list *head);
extern spisig_list   *efg_GetLastSpiSig (spisig_list *head);
extern void           efg_FreeSpiSigList    ( spisig_list* head);
extern char          *efg_GetSpiSigName     ( spisig_list*);
extern int            efg_IsWireOnSpiSig    ( spisig_list*);
extern chain_list    *efg_GetSpiSigNodeRc   (spisig_list *spisig);
extern char          *efg_GetSpiSigLoconRc  (spisig_list *spisig, char dir);
extern long           efg_GetSpiSigEvent    ( spisig_list*);

extern void           efg_FreeMarksOnExtFig ( lofig_list *lofig);
extern void           efg_FreeMarksOnFig    ( lofig_list *lofig);
extern chain_list    *efg_set_extra_capa    (lofig_list *figext,
                                             char *signame,
                                             float capaval
                                            );
extern void           efg_addctc            ( chain_list **added_ctclist,
                                              losig_list *sig,
                                              losig_list *vss,
                                              float capa
                                            );
extern void           efg_del_extra_capa    (chain_list *headctc);
extern chain_list    *efg_GetSig2Print (spisig_list *headspisig);
extern lotrs_list    *efg_addlotrs (lofig_list *, lotrs_list *, int);
extern void           efg_freecontext ( void );
spisig_list *efg_AddLatchLoopFeebackCone(lofig_list *lofig, cnsfig_list *cnsfig, spisig_list *origspisig);
int efg_GetSpiSigNum (spisig_list *spisig);
extern void           efg_add_lotrs_capa_dup (lofig_list *fig, locon_list *con);
char *efg_getlosigname(losig_list *ls);

// temp
chain_list *efg_getcone_onpath (spisig_list *hdspisig,cnsfig_list *cnsfig );
void efg_createcontext();
void efg_setsrcfig2context (lofig_list *fig);
void efg_setspisig2context (spisig_list *spisig);
void efg_AddExtraCapa (spisig_list *ptspisig, float capa);
void efg_DelHierSigPtypeFromSpisig (spisig_list *spisig);
int efg_test_signal_flags(losig_list *ls, long flags);
void efg_FreePathEvent (chain_list *path);
void efg_updateflag(losig_list *ls, int set, long mask);
char *efg_get_fig_name(lofig_list *lf, char *buf);
cone_list *efg_getcone ( losig_list *sig );
losig_list *efg_try_to_add_alim_to_the_context(losig_list *sig, float value);

#endif
