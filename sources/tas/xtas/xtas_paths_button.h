/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_paths_button.h                                         */
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

static struct XalButtonsItem XtasPathsButtonMenus[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,         (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasPathsCloseCallback,        (XtPointer)0 },
    { XTAS_SAVE_MAP  , True, XtasPathsSaveCallback,         (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasSignalInfoCallback,        (XtPointer)0 },
    { XTAS_PATH_MAP  , True, XtasPathsGetPathCallback,      (XtPointer)0 },
    { XTAS_CMD_MAP   , True, XtasPathsGetPathCmdCallback,   (XtPointer)0 },
    { XTAS_DETA_MAP  , True, XtasPathsDetailCallback,       (XtPointer)0 },
    { XTAS_ALLP_MAP  , True, XtasParaButtonCallback,        (XtPointer)0 },
//    { XTAS_FPAT_MAP  , True, XtasPathsFalsCallback,         (XtPointer)0 },
//    { XTAS_AL2S_MAP  , True, XtasNotYetCallback,     (XtPointer)0 },

    { -1, False, NULL, NULL }
};


static struct XalButtonsItem XtasPathsButtonOptions[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback, "File/Close"    },
    { XTAS_SAVE_MAP  , True, XtasDummyCallback, "File/Save Screen"    },
    { XTAS_INFO_MAP  , True, XtasDummyCallback, "View/Signal Info"   },
    { XTAS_PATH_MAP  , True, XtasDummyCallback, "Tools/Get Paths"     },
    { XTAS_CMD_MAP   , True, XtasDummyCallback, "Tools/Paths To Command" },
    { XTAS_DETA_MAP  , True, XtasDummyCallback, "Tools/Path Detail"   },
    { XTAS_ALLP_MAP  , True, XtasDummyCallback, "Tools/All Paths"     },
//    { XTAS_FPAT_MAP  , True, XtasDummyCallback, "Tools/False Paths"   },
/*    { XTAS_AL2S_MAP  , True, XtasDummyCallback, "Tools/Spice Extract" },
*/
    { -1, False, NULL, NULL }
};

