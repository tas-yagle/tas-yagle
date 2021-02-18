/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_stb_button.h                                           */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) :                                     Date : 01/04/1993     */
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

static struct XalButtonsItem XtasStbButtonMenus[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,     (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasStbCloseCallback,      (XtPointer)0 },
    { XTAS_SAVE_MAP  , True, XtasStbSaveCallback,       (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasStbDetailCallback,     (XtPointer)0 },
    { XTAS_PATH_MAP  , True, XtasStbGetPathCallback,    (XtPointer)0 },
    { XTAS_CMD_MAP   , True, XtasStbGetPathCmdCallback, (XtPointer)0 },
    { XTAS_DEBG_MAP  , True, XtasStbDebugCallback,      (XtPointer)XTAS_STB },
    { XTAS_NOISE_MAP , True, XtasNoiseCallback,         (XtPointer)0 },
    { -1, False, NULL, NULL }
};


static struct XalButtonsItem XtasStbButtonOptions[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback,  "File/Back To Main"     },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback,  "File/Close"            },
    { XTAS_SAVE_MAP  , True, XtasDummyCallback, "File/Save Screen"    },
    { XTAS_INFO_MAP  , True, XtasDummyCallback,  "View/Signals Info"     },
    { XTAS_PATH_MAP  , True, XtasDummyCallback, "Tools/Paths"            },
    { XTAS_CMD_MAP   , True, XtasDummyCallback, "Tools/Paths To Command" },
    { XTAS_DEBG_MAP  , True, XtasDummyCallback, "Tools/Debug"            },
    { XTAS_NOISE_MAP , True, XtasDummyCallback, "Tools/Noise Analysis"   },
    { -1, False, NULL, NULL }
};

