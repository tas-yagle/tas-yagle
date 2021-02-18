/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_desk_button.h                                          */
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
/*-------------------------*/
/* Button's menu structure */
/*-------------------------*/

static struct XalButtonsItem XtasDeskButtonMenus[] = {
    { XTAS_OPEN_MAP  , True, XtasOpenDataBaseCallback,    (XtPointer)0 },
/*
    { XTAS_SAVE_MAP  , True, XtasFileSaveCallback,        (XtPointer)0 },
*/
    { XTAS_CLOS_MAP  , True, XtasFileExitCallback,        (XtPointer)0 },
    { XTAS_CONN_MAP  , True, XtasDeskSignalsViewCallback, (XtPointer)XTAS_C_CONNECTOR },
    { XTAS_MEMO_MAP  , True, XtasDeskSignalsViewCallback, (XtPointer)XTAS_C_MEMORIZE  },
    { XTAS_COMM_MAP  , True, XtasDeskSignalsViewCallback, (XtPointer)XTAS_C_COMMANDS  },
    { XTAS_PREC_MAP  , True, XtasDeskSignalsViewCallback, (XtPointer)XTAS_C_PRECHARGE },
    { XTAS_BREA_MAP  , True, XtasDeskSignalsViewCallback, (XtPointer)XTAS_C_BREAK },
    { XTAS_ALLS_MAP  , True, XtasDeskSignalsViewCallback, (XtPointer)XTAS_C_OTHERSIGS },
    { XTAS_PATH_MAP  , True, XtasMainToolsCallback,       (XtPointer)0 },
    { XTAS_EXEC_MAP  , True, XtasGetDelayCallback,        (XtPointer)0 },
    { XTAS_STBA_MAP  , True, XtasStbAnalysisCallback,     (XtPointer)0 },
/*    { XTAS_TAS_MAP   , True, XtasNewSessionCallback,      (XtPointer)TAS_TOOL },
    { XTAS_TMA_MAP   , True, XtasNewSessionCallback,      (XtPointer)TMA_TOOL },
    { XTAS_XYA_MAP   , True, XtasXyagleCallback,          (XtPointer)0 },*/
    { -1             , False, NULL,                       NULL}
};

static struct XalButtonsItem XtasDeskButtonOptions[] = {
    { XTAS_OPEN_MAP  , True, XtasDummyCallback, "File/New Session"    },
/*
    { XTAS_SAVE_MAP  , True, XtasDummyCallback, "File/Save"           },
*/
    { XTAS_CLOS_MAP  , True, XtasDummyCallback, "File/Close"           },
    { XTAS_CONN_MAP  , True, XtasDummyCallback, "View/Connectors"     },
    { XTAS_MEMO_MAP  , True, XtasDummyCallback, "View/Memorizing"     },
    { XTAS_COMM_MAP  , True, XtasDummyCallback, "View/Commands"       },
    { XTAS_PREC_MAP  , True, XtasDummyCallback, "View/Precharge"      },
    { XTAS_BREA_MAP  , True, XtasDummyCallback, "View/Breakpoints"    },
    { XTAS_ALLS_MAP  , True, XtasDummyCallback, "View/All Signals"    },
    { XTAS_PATH_MAP  , True, XtasDummyCallback, "Tools/Paths"         },
    { XTAS_EXEC_MAP  , True, XtasDummyCallback, "Tools/Delays"        },
    { XTAS_STBA_MAP  , True, XtasDummyCallback, "Tools/Stability"     },
/*    { XTAS_TAS_MAP   , True, XtasDummyCallback, "Tools/Timing Analysis"     },
    { XTAS_TMA_MAP   , True, XtasDummyCallback, "Tools/Timing Model Abstraction"     },
    { XTAS_XYA_MAP   , True, XtasDummyCallback, "Tools/Xyagle"     },*/
    { -1             , False, NULL,             NULL                }
};

