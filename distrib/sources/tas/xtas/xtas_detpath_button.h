/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_detpath_button.h                                       */
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

static struct XalButtonsItem XtasDetailButtonMenus[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,     (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasDetailCloseCallback,   (XtPointer)0 },
    { XTAS_SAVE_MAP  , True, XtasDetailSaveCallback,    (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasDetailDetailCallback,  (XtPointer)0 },
    { XTAS_CTKI_MAP  , True, XtasCtkInfoCallback,      (XtPointer)0 },
    { XTAS_VISU_MAP  , True, XtasVisualPathCallback,    (XtPointer)0 },
    { XTAS_SIMU_MAP  , True, XtasSimuPathCallback,      (XtPointer)0 },
    { -1, False, NULL, NULL }
};


static struct XalButtonsItem XtasDetailButtonOptions[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback, "File/Back To Main"     },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback, "File/Close"            },
    { XTAS_SAVE_MAP  , True, XtasDummyCallback, "File/Save Screen"      },
    { XTAS_INFO_MAP  , True, XtasDummyCallback, "View/Signals Info"     },
    { XTAS_CTKI_MAP  , True, XtasDummyCallback, "View/Crosstalk Informations"  },
    { XTAS_VISU_MAP  , True, XtasDummyCallback, "View/Visualise a Path" },
    { XTAS_SIMU_MAP  , True, XtasDummyCallback, "Tools/Simulate a Path"  },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem XtasDetailButtonMenusSimu[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,     (XtPointer)0 },
    { XTAS_SAVE_MAP  , True, XtasDetailSaveCallback,    (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasDetailCloseCallback,   (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasDetailDetailCallback,  (XtPointer)0 },
    { XTAS_VISU_MAP  , True, XtasVisualPathCallback,    (XtPointer)0 },
    { -1, False, NULL, NULL }
};


static struct XalButtonsItem XtasDetailButtonOptionsSimu[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback, "File/Back To Main"     },
    { XTAS_SAVE_MAP  , True, XtasDummyCallback, "File/Save Screen"      },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback, "File/Close"            },
    { XTAS_INFO_MAP  , True, XtasDummyCallback, "View/Signals Info"     },
    { XTAS_VISU_MAP  , True, XtasDummyCallback, "View/Visualise a Path" },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem XtasDetailButtonMenusDelay[] = {
    { XTAS_MAIN_MAP  , True, XtasMainPopupCallback,     (XtPointer)0 },
    { XTAS_SAVE_MAP  , True, XtasDetailSaveCallback,    (XtPointer)0 },
    { XTAS_CLOS_MAP  , True, XtasDetailCloseCallback,   (XtPointer)0 },
    { XTAS_INFO_MAP  , True, XtasDetailDetailCallback,  (XtPointer)0 },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem XtasDetailButtonOptionsDelay[] = {
    { XTAS_MAIN_MAP  , True, XtasDummyCallback, "File/Back To Main"     },
    { XTAS_SAVE_MAP  , True, XtasDummyCallback, "File/Save Screen"      },
    { XTAS_CLOS_MAP  , True, XtasDummyCallback, "File/Close"            },
    { XTAS_INFO_MAP  , True, XtasDummyCallback, "View/Signals Info"     },
    { -1, False, NULL, NULL }
};

