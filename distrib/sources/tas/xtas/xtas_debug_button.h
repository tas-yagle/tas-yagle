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

static struct XalButtonsItem XtasDebugButtonMenus[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,         (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasDebugCloseCallback,        (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasDebugInfoCallback,         (XtPointer)0 },
    { XTAS_PATH_MAP  , True, XtasDebugGetPathCallback,      (XtPointer)0 },
    { XTAS_DEBG_MAP  , True, XtasStbDebugCallback,          (XtPointer)XTAS_DEBUG },
    { XTAS_CMD_MAP   , True, XtasDebugGetPathCmdCallback,   (XtPointer)0 },
    { -1, False, NULL, NULL }
};


static struct XalButtonsItem XtasDebugButtonOptions[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback,"File/Back To Main"      },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback,"File/Close"             },
    { XTAS_INFO_MAP  , True, XtasDummyCallback,"Tools/Signal Info"      },
    { XTAS_PATH_MAP  , True, XtasDummyCallback,"Tools/Paths"            },
    { XTAS_DEBG_MAP  , True, XtasDummyCallback,"Tools/Debug"            },
    { XTAS_CMD_MAP   , True, XtasDummyCallback,"Tools/Paths to Command" },
    { -1, False, NULL, NULL }
};
