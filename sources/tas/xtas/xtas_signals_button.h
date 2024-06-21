/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_signals_button.h                                       */
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

/*-------------------------*/
/* Connectors              */
/*-------------------------*/

static struct XalButtonsItem Xtasc_buttons[] = {
    { XTAS_MAIN_MAP, True, XtasMainPopupCallback,     (XtPointer)0                },
    { XTAS_CLOS_MAP, True, XtasFileCloseCallback,     (XtPointer)XTAS_C_CONNECTOR },
    { XTAS_INFO_MAP, True, XtasSignalsDetailCallback, (XtPointer)XTAS_C_CONNECTOR },
    { XTAS_CTKI_MAP, True, XtasSignalsCtkInfoCallback,(XtPointer)XTAS_C_CONNECTOR },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem Xtasc_options[] = {
    { XTAS_MAIN_MAP, True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP, True, XtasDummyCallback, "File/Close"    },
    { XTAS_INFO_MAP, True, XtasDummyCallback, "VIew/Signals Info"  },
    { XTAS_CTKI_MAP, True, XtasDummyCallback, "VIew/Crosstalk Info" },
    { -1, False, NULL, NULL }
};

/*-------------------------*/
/* Commands                */
/*-------------------------*/

static struct XalButtonsItem Xtaso_buttons[] = {
    { XTAS_MAIN_MAP, True, XtasMainPopupCallback,     (XtPointer)0               },
    { XTAS_CLOS_MAP, True, XtasFileCloseCallback,     (XtPointer)XTAS_C_COMMANDS },
    { XTAS_INFO_MAP, True, XtasSignalsDetailCallback, (XtPointer)XTAS_C_COMMANDS },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem Xtaso_options[] = {
    { XTAS_MAIN_MAP, True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP, True, XtasDummyCallback, "File/Close"    },
    { XTAS_INFO_MAP, True, XtasDummyCallback, "VIew/Signals Info"  },
    { -1, False, NULL, NULL }
};

/*-------------------------*/
/* Mem points              */
/*-------------------------*/

static struct XalButtonsItem Xtasm_buttons[] = {
    { XTAS_MAIN_MAP, True, XtasMainPopupCallback,     (XtPointer)0                },
    { XTAS_CLOS_MAP, True, XtasFileCloseCallback,     (XtPointer)XTAS_C_MEMORIZE  },
    { XTAS_INFO_MAP, True, XtasSignalsDetailCallback, (XtPointer)XTAS_C_MEMORIZE  },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem Xtasm_options[] = {
    { XTAS_MAIN_MAP, True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP, True, XtasDummyCallback, "File/Close"    },
    { XTAS_INFO_MAP, True, XtasDummyCallback, "VIew/Signals Info"  },
    { -1, False, NULL, NULL }
};

/*-------------------------*/
/* Precharge               */
/*-------------------------*/

static struct XalButtonsItem Xtasp_buttons[] = {
    { XTAS_MAIN_MAP, True, XtasMainPopupCallback,     (XtPointer)0                },
    { XTAS_CLOS_MAP, True, XtasFileCloseCallback,     (XtPointer)XTAS_C_PRECHARGE },
    { XTAS_INFO_MAP, True, XtasSignalsDetailCallback, (XtPointer)XTAS_C_PRECHARGE },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem Xtasp_options[] = {
    { XTAS_MAIN_MAP, True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP, True, XtasDummyCallback, "File/Close"    },
    { XTAS_INFO_MAP, True, XtasDummyCallback, "VIew/Signals Info"  },
    { -1, False, NULL, NULL }
};

/*-------------------------*/
/* Break                   */
/*-------------------------*/

static struct XalButtonsItem Xtasb_buttons[] = {
    { XTAS_MAIN_MAP, True, XtasMainPopupCallback,     (XtPointer)0                },
    { XTAS_CLOS_MAP, True, XtasFileCloseCallback,     (XtPointer)XTAS_C_BREAK },
    { XTAS_INFO_MAP, True, XtasSignalsDetailCallback, (XtPointer)XTAS_C_BREAK },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem Xtasb_options[] = {
    { XTAS_MAIN_MAP, True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP, True, XtasDummyCallback, "File/Close"    },
    { XTAS_INFO_MAP, True, XtasDummyCallback, "VIew/Signals Info"  },
    { -1, False, NULL, NULL }
};

/*-------------------------*/
/* All sigs                */
/*-------------------------*/

static struct XalButtonsItem Xtasa_buttons[] = {
    { XTAS_MAIN_MAP, True, XtasMainPopupCallback,      (XtPointer)0              },
    { XTAS_CLOS_MAP, True, XtasFileCloseCallback,      (XtPointer)XTAS_C_OTHERSIGS },
    { XTAS_INFO_MAP, True, XtasSignalsDetailCallback,  (XtPointer)XTAS_C_OTHERSIGS },
    { XTAS_CTKI_MAP, True, XtasSignalsCtkInfoCallback, (XtPointer)XTAS_C_OTHERSIGS },
    { -1, False, NULL, NULL }
};

static struct XalButtonsItem Xtasa_options[] = {
    { XTAS_MAIN_MAP, True, XtasDummyCallback, "File/Back To Main"   },
    { XTAS_CLOS_MAP, True, XtasDummyCallback, "File/Close"          },
    { XTAS_INFO_MAP, True, XtasDummyCallback, "VIew/Signals Info"   },
    { XTAS_CTKI_MAP, True, XtasDummyCallback, "VIew/Crosstalk Info" },
    { -1, False, NULL, NULL }
};

