/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas.h                                                      */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#define XTAS_MOTIF_VERSION  XmVERSION*10000+(XmREVISION*100)+XmUPDATE_LEVEL

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   STANDARD INCLUDES                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   X11 INCLUDES                                                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   MOTIF INCLUDES                                                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include <Xm/Xm.h>
#include <Xm/ArrowBG.h>
#include <Xm/CascadeBG.h>
#include <Xm/DrawnB.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Frame.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/PanedW.h>
#include <Xm/Protocols.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/Separator.h>

#if XTAS_MOTIF_VERSION >= 20100        
#include <Xm/SSpinB.h>
#endif

#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <Xm/DrawingA.h>

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   ALLIANCE INCLUDES                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include MUT_H
#include MLO_H
#include MLU_H
#include BEH_H
#include BHL_H 
#include BVL_H 
#include INF_H 
#include STM_H 
#include TTV_H
#include STB_H
#include CNS_H
#include YAG_H
#include TLC_H
#include TRC_H
#include EFG_H
#include TAS_H
#include TMA_H
#include MCC_H
#include LOG_H
#include LIB_H
#include TLF_H
#include SIM_H
//#include ZEN_H
#include XAL_H
#include CGV_H
#include XYA_H
#include FCL_H

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   AVERTEC INCLUDES                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#ifdef AVERTEC
#include AVT_H
#endif

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   Globals                                                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   define                                                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/


#define HELP_TOPICS    5
#define HELP_SOURCES   26

#define XTAS_HELP_MAIN     0x0000
#define XTAS_HELP_FILE     0x0001
#define XTAS_HELP_TOOLS    0x0002 
#define XTAS_HELP_VIEW     0x0003
#define XTAS_HELP_OPTIONS  0x0004

#define XTAS_SRC_DESK       0x0000
#define XTAS_SRC_SIGNALS    0x0100
#define XTAS_SRC_HIER       0x0200
#define XTAS_SRC_ALLSIGS    0x0300
#define XTAS_SRC_GETPATH    0x0400
#define XTAS_SRC_PATHS      0x0500
#define XTAS_SRC_GETDETAIL  0x0600
#define XTAS_SRC_DETAIL     0x0700
#define XTAS_SRC_DETAILCTK  0x0800
#define XTAS_SRC_GETPARA    0x0900
#define XTAS_SRC_PARA       0x0a00
#define XTAS_SRC_FLSPATHS   0x0b00
#define XTAS_SRC_VISUPATH   0x0c00
#define XTAS_SRC_SIMUPARAM  0x0d00
#define XTAS_SRC_DETAILSIMU 0x0e00
#define XTAS_SRC_GETDELAY   0x0f00
#define XTAS_SRC_DELAY      0x1000
#define XTAS_SRC_STABPARAM  0x1100
#define XTAS_SRC_STABRES    0x1200
#define XTAS_SRC_DEBUG      0x1300
#define XTAS_SRC_NOISERES   0x1400
#define XTAS_SRC_CTKINFO    0x1500
#define XTAS_SRC_TASPARAM   0x1600
#define XTAS_SRC_TASADV     0x1700
#define XTAS_SRC_TMAPARAM   0x1800
#define XTAS_SRC_NOISESCR   0x1900

#define XTAS_C_CONNECTOR 0
#define XTAS_C_MEMORIZE  1
#define XTAS_C_COMMANDS  2
#define XTAS_C_PRECHARGE 3
#define XTAS_C_BREAK     4
#define XTAS_C_OTHERSIGS 5

#define XTAS_C_NBSIGTYPE 6

#define XTAS_AL2S_MAP      0
#define XTAS_ALLP_MAP      1
#define XTAS_ALLS_MAP      2
#define XTAS_BREA_MAP      3
#define XTAS_CAPA_MAP      4
#define XTAS_CLOS_MAP      5
#define XTAS_COMM_MAP      6
#define XTAS_CONN_MAP      7
#define XTAS_DEBG_MAP      8
#define XTAS_DETA_MAP      9
#define XTAS_DUMM_MAP     10
#define XTAS_FPAT_MAP     11
#define XTAS_FEDG_MAP     12
#define XTAS_GATE_MAP     13
#define XTAS_HRES_MAP     14
#define XTAS_LRES_MAP     15
#define XTAS_INFO_MAP     16
#define XTAS_MAIN_MAP     17
#define XTAS_MEMO_MAP     18
#define XTAS_OPEN_MAP     19
#define XTAS_PATH_MAP     20
#define XTAS_PREC_MAP     21
#define XTAS_RCLN_MAP     22
#define XTAS_REDG_MAP     23
#define XTAS_SAVE_MAP     24
#define XTAS_TIME_MAP     25
#define XTAS_EXEC_MAP     26
#define XTAS_STBA_MAP     27
#define XTAS_FALS_MAP     28
#define XTAS_TRUE_MAP     29
#define XTAS_TAS_MAP      30
#define XTAS_TMA_MAP      31
#define XTAS_XYA_MAP      32
#define XTAS_VISU_MAP     33
#define XTAS_CMD_MAP      34
#define XTAS_SIMU_MAP     35
#define XTAS_CTKI_MAP     36
#define XTAS_NOISE_MAP    37
#define XTAS_SCORE_MAP    38

#define XTAS_NODELEVEL(node) (((node->TYPE & TTV_NODE_UP) == TTV_NODE_UP) \
                            ? 'U' : 'D')


#define XTAS_TTX    'T'
#define XTAS_DTX    'D'

#define TAS_TOOL  'S'  
#define TMA_TOOL  'M'
#define NO_TOOL   'N'

#define XTAS_HOLD   0x1
#define XTAS_REG    0x2

#define XTAS_STB    ((long) 0x00000001)
#define XTAS_DEBUG  ((long) 0x00000002)

#define XTAS_NONE  (long)0x00
#define XTAS_SIMU  (long)0x01
#define XTAS_CTK   (long)0x02

/* Valeurs possibles pour la variable XTAS_CTX_LOADED */
#define XTAS_NOT_LOADED     0x0000
#define XTAS_LOADED         0x0001
#define XTAS_FROM_ANALYSIS  0x0011
#define XTAS_FROM_FILE      0x0021

/* values for REQUEST_TYPE - GetPath*/
#define     XTAS_NOTYPE     0x000
#define     XTAS_PATHS      0x010
#define     XTAS_ACCESS     0x020
#define     XTAS_CRITIC     0x001 
#define     XTAS_ALL        0x002
#define     XTAS_SBS        0x100

/* valeur pour le choix du trace des chornogrammes */
#define     XTAS_DEBUG_IUD      0x01
#define     XTAS_DEBUG_IU       0x02
#define     XTAS_DEBUG_ID       0x04
#define     XTAS_DEBUG_OUD      0x10
#define     XTAS_DEBUG_OU       0x20
#define     XTAS_DEBUG_OD       0x40
#define     XTAS_DEBUG_NOACTION 0xff

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   STRUCTUR DECLARATION                                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

typedef struct XtasWindow
    {
     Widget topwidget ,
            menwidget ,
            butwidget ,
            wrkwidget ,
            meswidget ,
            hlpwidget ;
     XalMessageWidgetStruct        *errwidget ,
                                   *warwidget ;
     void  *userdata ;
     long   userinfos ;
    }
XtasWindowStruct;

typedef struct XtasMainInfo
    {
     ttvfig_list *ttvfig ;
     befig_list *befig ;
     stbfig_list *stbfig ;
	 stbdebug_list *debugfig ;
    }
XtasMainInfoStruct ;

/*---------------------------------------------------------------------*/
typedef struct XtasWinJmp    
{
 struct XtasWinJmp  *NEXT ;       /*  adresse de saut suivant dans la liste */
 struct XtasWinJmp  *PREV ;       /*  adresse de saut precedent dans la liste */
 int                 SIZE ;       /*  taille de la fenetre */
 int                 INDEX ;      /*  index globale par rapport a la liste */
 long                TYPE ;       /*  type de liste pointee */
 void               *DATA ;       /*  pointeur vers l'element de la liste */
}
XtasChainJmpList;

/*------------ parametres de recherche des signaux --------------------*/
typedef struct XtasSearchSigParams
{
 long         TYPE;
 chain_list  *MASK;
 ttvfig_list *TOP_FIG;
 ttvfig_list *LOCAL_FIG;
 int          ITEMS;
}
XtasSearchSigParamStruct;

/*------------ parametres de recherche des chemins --------------------*/
typedef struct XtasSearchPathParams
{
 XtasSearchSigParamStruct *ROOT_SIG_EDGE;
 chain_list               *NODE_MASK;
 chain_list               *CK_MASK;
 chain_list               *CUR_SIG;
 ttvpath_list             *LOOK_PATH;
 long                      DELAY_MIN;
 long                      DELAY_MAX;
 long                      SCAN_TYPE;
 long                      REQUEST_TYPE;
 char                      CRITIC;
 int                       PATH_ITEM;
 chain_list               *MASK; 
}
XtasSearchPathParamStruct;

/*------------ objets pour la gestion de l'affichage des signaux ------*/
typedef struct XtasSigListSet
{
 Widget                     TOP_LEVEL;
 Widget                     SIGLIST_SCROLL;
 Widget                     MASK;
 Widget                     ITEMS;
 Widget                     INFO_PAGE;
 Widget                     INFO_HELP;
 Widget                     INFO_USER;
 XtasSearchSigParamStruct  *SIGNAL;
 chain_list                *SIG_LIST;
 XtasChainJmpList          *SIG_LIST_WIN;
 XtasChainJmpList          *CUR_SIG_WIN;
}
XtasSigListSetStruct;

/*------------ objets pour la gestion de l'affichage des chemins ------*/
typedef struct XtasPathListSet
{
 Widget                     TOP_LEVEL;
 Widget                     PATHLIST_2INFO ;
 Widget                     PATHLIST_SCROLL;
 Widget                     PATHLIST_ROW;
 Widget                     INFO_HELP;
 Widget                     INFO_USER;
 Widget                     INFO_PAGE;
 Widget                     INFO_CUR_SIG;
 XtasSigListSetStruct      *SIGNAL_LIST_SET;
 ttvpath_list              *PATH_LIST;
 XtasChainJmpList          *PATH_LIST_WIN;
 XtasChainJmpList          *CUR_PATH_WIN;
 chain_list                *INDEX_TAB;
 chain_list                *DETAIL_SESSION;
 chain_list                *PARA_SESSION;
 struct XtasPathListSet    *PARENT ;
 XtasSearchPathParamStruct *PARAM;
}
XtasPathListSetStruct;

/*--------------------------------------------------------------------------*/
typedef struct XtasPathIndexTab
{
 XtasPathListSetStruct *PATH_SET;
 ttvpath_list          *PATH;
}
XtasPathIndexTabElt;

/*--------------------------------------------------------------------------*/
typedef struct XtasDetailPathSet
{
 Widget                 TOP_LEVEL;
 Widget                 SCROLL_DETAIL;
 Widget                 ROW_DETAIL;
 Widget                 INFO_USER;
 Widget                 INFO_HELP;
 Widget                 INFO_PAGE;
 Widget                 ROOT_NAME;
 Widget                 ROOT_RF;/*Rise/Fall*/
 Widget                 NODE_NAME;
 Widget                 NODE_RF;/*Rise/Fall*/
 Widget                 TOTAL_DELAY;
 XtasChainJmpList      *DETAIL_LIST;
 XtasChainJmpList      *CUR_DETAIL;
 XtasPathListSetStruct *PARENT;
}
XtasDetailPathSetStruct;

/*----------  Pour sauvegarder et restaurer les contextes de zoom  ---------*/
typedef struct XtasZoomContext
{
 struct XtasZoomContext    *NEXT;
 long                       TMIN;
 long                       TMAX;
}
XtasZoomContextStruct;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*   APPLICATION SPECIFIC INCLUDES                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
#include "xtas_common.h"
#include "xtas_desk.h"
#include "xtas_detsig.h"
#include "xtas_files.h"
#include "xtas_help.h"
#include "xtas_hierarchical.h"
#include "xtas_init.h"
#include "xtas_main.h"
#include "xtas_tas.h"
#include "xtas_ttv.h"
#include "xtas_unix.h"
#include "xtas_messages.h"
#include "xtas_getpara.h"
#include "xtas_getpath.h"
#include "xtas_getdelay.h"
#include "xtas_getdetail.h"
#include "xtas_detpath.h"
#include "xtas_paths.h"
#include "xtas_flspaths.h"
#include "xtas_signals.h"
#include "xtas_stb.h"
#include "xtas_stb_pbar.h"
#include "xtas_debug.h"
#include "xtas_graphic.h"
#include "xtas_newdisp_util.h"
#include "xtas_xyagle.h"
#include "xtas_visualise_path.h"
#include "xtas_simu_path.h"
#include "xtas_ctk_noise.h"
#include "xtas_exit.h"

