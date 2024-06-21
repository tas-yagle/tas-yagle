/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_ctk_noise.c                                            */
/*                                                                          */
/*    Author(s) : Caroline BLED                       Date : 03/24/2003     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#include "xtas.h"
#include "xtas_ctk_noise_menu.h"
#include "xtas_ctk_noise_button.h"
#include "xtas_stb_pbar.h"


#define  XTAS_NOISE_NAME_MAX_LENGTH       50
#define  XTAS_NOISE_NAME_MIN_LENGTH       11

#define  XTAS_NOISE_LABEL_RANK_WIDTH_T      5*7   /* 7 = largeur moyenne d'un caractere */
#define  XTAS_NOISE_LABEL_RANK_WIDTH_G      5*7   /* 7 = largeur moyenne d'un caractere */
#define  XTAS_NOISE_LABEL_TRANS_WIDTH_T     5*7    /* Textual Display */  
#define  XTAS_NOISE_LABEL_TRANS_WIDTH_G     5*7    /* Graphical Display */
#define  XTAS_NOISE_LABEL_NAMES_WIDTH_G     121    
#define  XTAS_NOISE_LABEL_MODEL_WIDTH_T     5*7
#define  XTAS_NOISE_LABEL_MODEL_WIDTH_G     6*7
#define  XTAS_NOISE_LABEL_NOISE_WIDTH_T     7*7
#define  XTAS_NOISE_LABEL_NOISE_WIDTH_G     10*7
#define  XTAS_NOISE_LABEL_SCORE_WIDTH_T     9*7
#define  XTAS_NOISE_LABEL_SCORE_WIDTH_G     9*7
#define  XTAS_NOISE_OFFSET_T                7
#define  XTAS_NOISE_OFFSET_T2               3
#define  XTAS_NOISE_OFFSET_G                3
#define  XTAS_NOISE_OFFSET_GROUP_T          3*7
#define  XTAS_NOISE_OFFSET_GROUP_G          7   
#define  XTAS_NOISE_LEFT_OFFSET_T           8
#define  XTAS_NOISE_LEFT_OFFSET_G           8   /* offset a gauche pour le titre de la premiere colonne */

#define  XTAS_NOISE_LABEL_OVR_UND_WIDTH_T   ( XTAS_NOISE_LABEL_MODEL_WIDTH_T    + \
                                              2*XTAS_NOISE_LABEL_NOISE_WIDTH_T  + \
                                              2*XTAS_NOISE_OFFSET_T )

#define  XTAS_NOISE_LABEL_OVR_UND_WIDTH_G   ( XTAS_NOISE_LABEL_MODEL_WIDTH_G    + \
                                              2*XTAS_NOISE_LABEL_NOISE_WIDTH_G  + \
                                              2*XTAS_NOISE_OFFSET_G )            

#define  XTAS_NOISE_LABEL_SCORES_WIDTH_T    ( 5*XTAS_NOISE_LABEL_SCORE_WIDTH_T  + \
                                              4*XTAS_NOISE_OFFSET_T)

#define  XTAS_NOISE_LABEL_SCORES_WIDTH_G    ( 5*XTAS_NOISE_LABEL_SCORE_WIDTH_G  + \
                                              4*XTAS_NOISE_OFFSET_G)

#define  XTAS_NOISE_WINDOW_WIDTH_T          XTAS_NOISE_LEFT_OFFSET_T            + \
                                            XTAS_NOISE_LABEL_RANK_WIDTH_T       + \
                                            XTAS_NOISE_LABEL_TRANS_WIDTH_T      + \
                                            2*XTAS_NOISE_LABEL_OVR_UND_WIDTH_T  + \
                                            XTAS_NOISE_LABEL_SCORES_WIDTH_T     + \
                                            3*XTAS_NOISE_OFFSET_T               + \
                                            4*XTAS_NOISE_OFFSET_GROUP_T         + \
                                            40 /* ecart fixe a droite du tableau pour
                                                  le voir entierement */

#define  XTAS_NOISE_WINDOW_WIDTH_G          XTAS_NOISE_LEFT_OFFSET_G            + \
                                            XTAS_NOISE_LABEL_RANK_WIDTH_G       + \
                                            XTAS_NOISE_LABEL_TRANS_WIDTH_G      + \
                                            XTAS_NOISE_LABEL_NAMES_WIDTH_G    + \
                                            2*XTAS_NOISE_LABEL_OVR_UND_WIDTH_G  + \
                                            XTAS_NOISE_LABEL_SCORES_WIDTH_G     + \
                                            2*XTAS_NOISE_OFFSET_G               + \
                                            4*XTAS_NOISE_OFFSET_GROUP_G         + \
                                            40 /* ecart fixe a droite du tableau pour
                                                  le voir entierement */

#define  XTAS_NOISE_SORTBUTTON_WIDTH_G      ((  XTAS_NOISE_LABEL_RANK_WIDTH_G     + \
                                                XTAS_NOISE_LABEL_TRANS_WIDTH_G    + \
                                                XTAS_NOISE_LABEL_NAMES_WIDTH_G    + \
                                                XTAS_NOISE_OFFSET_GROUP_G ) / 2)
                                              
#define  XTAS_NOISE_SORTBUTTON_WIDTH_T      ((  XTAS_NOISE_LABEL_RANK_WIDTH_T     + \
                                                XTAS_NOISE_LABEL_TRANS_WIDTH_T    + \
                                                2*XTAS_NOISE_OFFSET_GROUP_T  ) / 2)



#define  XTAS_NOISE_NO_SELECT           -1

#define  XTAS_CTK_OFFSET                 2

static int   XTAS_NOISE_MAX_NBSIG_WINDOW;

Widget               XtasNoiseParamWidget = NULL;
Widget               XtasNoiseResultsWidget = NULL;
Widget               XtasNoiseScoresWidget = NULL;
stb_ctk_stat        *XtasNoiseTab = NULL;
int                  XtasNoiseLabelSigNameLength = XTAS_NOISE_NAME_MIN_LENGTH ; 
int                  XtasNoiseLabelNetNameLength = XTAS_NOISE_NAME_MIN_LENGTH ; 
int                  XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT; 
int                  XtasNoiseCurrentTopIndex = 0; 


/* functions prototypes */
void XtasNoiseFillGraphicalList ( int new, Widget widget, XtasWindowStruct *win_struct);
void XtasUpdateNoiseTextualScrollList (Widget list);


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseColumnsTitleMoveWithHSBCallback                       */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .client_data :                                                 */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the button "OK" of Crosstalk         */
/*            informations window.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseColumnsTitleMoveWithHSBCallback (hsb, label, call_data)
    Widget  hsb;
    XtPointer   label;
    XtPointer   call_data;
{
    int         loffset ;
    int         value;

    if (XTAS_DISPLAY_NEW == 0)
        loffset = XTAS_NOISE_LEFT_OFFSET_G;
    else
        loffset = XTAS_NOISE_LEFT_OFFSET_T;

    XtVaGetValues (hsb, XmNvalue, &value, NULL);

    XtVaSetValues (label, XmNleftOffset, (loffset - value), NULL);

}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseScoreOkCallback                                       */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .client_data :                                                 */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the button "OK" of Crosstalk         */
/*            informations window.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseScoreOkCallback (parent, client_data, call_data)
    Widget  parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    Widget      spinbox, scroll;
    int         pos_coef_noise, pos_coef_interval, pos_coef_ctk, pos_coef_activity;
    int         pos_min_noise, pos_min_interval, pos_min_ctk, pos_min_activity;
    XtasWindowStruct    *tas_winfos  = (XtasWindowStruct *)client_data;
    char        *text, *ptend;
    Widget      pan_widget, label;
    int         nb, i;
    WidgetList  widget_list;

#if XTAS_MOTIF_VERSION >= 20100        
    /**** Spin Boxes ****/
    /* noise update */
    spinbox = XtNameToWidget (parent, "*XtasNoiseCoefBox");
    XtVaGetValues (spinbox, XmNposition, &pos_coef_noise, NULL);
    spinbox = XtNameToWidget (parent, "*XtasNoiseMinBox");
    XtVaGetValues (spinbox, XmNposition, &pos_min_noise, NULL);
    
    /* interval update */
    spinbox = XtNameToWidget (parent, "*XtasIntervalCoefBox");
    XtVaGetValues (spinbox, XmNposition, &pos_coef_interval, NULL);
    spinbox = XtNameToWidget (parent, "*XtasIntervalMinBox");
    XtVaGetValues (spinbox, XmNposition, &pos_min_interval, NULL);

    /* crosstalk update */
    spinbox = XtNameToWidget (parent, "*XtasCtkCoefBox");
    XtVaGetValues (spinbox, XmNposition, &pos_coef_ctk, NULL);
    spinbox = XtNameToWidget (parent, "*XtasCtkMinBox");
    XtVaGetValues (spinbox, XmNposition, &pos_min_ctk, NULL);

    /* activity update */
    spinbox = XtNameToWidget (parent, "*XtasActivityCoefBox");
    XtVaGetValues (spinbox, XmNposition, &pos_coef_activity, NULL);
    spinbox = XtNameToWidget (parent, "*XtasActivityMinBox");
    XtVaGetValues (spinbox, XmNposition, &pos_min_activity, NULL);
#else
    /**** Text Boxes **** -> il faut verifier la valeur */ 
    /* noise update */
    text = XmTextGetString (XtNameToWidget (parent, "*XtasNoiseCoefBox"));
    if (text) {
        pos_coef_noise = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_coef_noise < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Noise Coef.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
    text = XmTextGetString (XtNameToWidget (parent, "*XtasNoiseMinBox"));
    if (text) {
        pos_min_noise = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_min_noise < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Noise Min.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
        
    /* interval update */
    text = XmTextGetString (XtNameToWidget (parent, "*XtasIntervalCoefBox"));
    if (text) {
        pos_coef_interval = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_coef_interval < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Interval Coef.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
    text = XmTextGetString (XtNameToWidget (parent, "*XtasIntervalMinBox"));
    if (text) {
        pos_min_interval = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_min_interval < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Interval Min.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
    
    /* crosstalk update */
    text = XmTextGetString (XtNameToWidget (parent, "*XtasCtkCoefBox"));
    if (text) {
        pos_coef_ctk = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_coef_ctk < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Crosstalk Coef.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
    text = XmTextGetString (XtNameToWidget (parent, "*XtasCtkMinBox"));
    if (text) {
        pos_min_ctk = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_min_ctk < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Crosstalk Min.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
    
    /* activity update */
    text = XmTextGetString (XtNameToWidget (parent, "*XtasActivityCoefBox"));
    if (text) {
        pos_coef_activity = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_coef_activity < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Interval Coef.\nMust be a positive integer between 0 and 10." );
            return ;
        }
        XtFree (text);
    }
    text = XmTextGetString (XtNameToWidget (parent, "*XtasActivityMinBox"));
    if (text) {
        pos_min_activity = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(pos_min_activity < 0)||(pos_coef_noise > 10)) {
            XtFree (text);
            XtManageChild (XtasNoiseScoresWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Noise Min.\nMust be a positive integer. between 0 and 10" );
            return ;
        }
        XtFree (text);
    }
#endif

    stb_ctk_set_coef_score (pos_coef_noise, pos_coef_ctk, pos_coef_interval, pos_coef_activity);
    stb_ctk_set_min_score (pos_min_noise, pos_min_ctk, pos_min_interval, pos_min_activity);
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_SCORE_TOTAL);
    if (XTAS_DISPLAY_NEW == 0) {
        scroll = XtNameToWidget ((Widget)tas_winfos->wrkwidget, "*XtasScrollNoiseWidget");
//        if (!scroll) {
//            pan_widget = XtNameToWidget ((Widget)tas_winfos->wrkwidget, "*XtasPanWindow");
//            XtVaGetValues (pan_widget, XmNnumChildren, &nb, XmNchildren, &widget_list, NULL);
////            for (i=0 ; i<nb ; i++) 
////               XtDestroyWidget (widget_list[i]); 
//            XtDestroyWidget (widget_list[0]); 
//            label = XtNameToWidget ((Widget)tas_winfos->wrkwidget, "*XtasTitlesInsideAlimMax");
//            if (pan_widget && label)
//                XtasCreateNoiseGraphicalScrollList (pan_widget, tas_winfos, label);
//        }
//        else 
            XtasNoiseTreatTopCallback (NULL, (XtPointer)scroll, NULL);
    }
    else {
        scroll = XtNameToWidget ((Widget)tas_winfos->wrkwidget, "*XtasTextualDisplay");
        XtasUpdateNoiseTextualScrollList (scroll);
    }
    
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseScoreConfigCallback                                   */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .client_data :                                                 */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the button "OK" of Crosstalk         */
/*            informations window.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseScoreConfigCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    Arg         args[10];
    int         n;
    char       *spinname, *tname;
    Widget      row, frame_form, form, formsb, label, spinbox, spin_TF;
    Atom        WM_DELETE_WINDOW;
    XmString    text;
    char        buf[64];

    if (!XtasNoiseScoresWidget) {
        n = 0;
        XtSetArg ( args[n], XmNtitle, XTAS_NAME": Scores Configuration" ); n++;
        XtSetArg ( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
        XtSetArg( args[n], XmNwidth, 255) ; n++ ;
        XtasNoiseScoresWidget = XmCreatePromptDialog(XtasNoiseResultsWidget, "XtasInfosBox",args, n) ;
        HelpFather = XtasNoiseScoresWidget;
        XtUnmanageChild(XmSelectionBoxGetChild(XtasNoiseScoresWidget,XmDIALOG_TEXT)) ;
        XtUnmanageChild(XmSelectionBoxGetChild(XtasNoiseScoresWidget,XmDIALOG_PROMPT_LABEL)) ;
        XtUnmanageChild(XmSelectionBoxGetChild(XtasNoiseScoresWidget,XmDIALOG_HELP_BUTTON)) ;
        XtAddCallback( XtasNoiseScoresWidget, XmNokCallback, XtasNoiseScoreOkCallback, (XtPointer)client_data ) ;
        XtAddCallback( XtasNoiseScoresWidget, XmNcancelCallback, XtasCancelCallback, (XtPointer)XtasNoiseScoresWidget ) ;
//        XtAddCallback( XtasNoiseScoresWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|)) ;
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasNoiseResultsWidget), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasNoiseScoresWidget), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )XtasNoiseScoresWidget);
 
       
        n = 0;
        frame_form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, XtasNoiseScoresWidget, args, n ) ;
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, frame_form, args, n ) ;

        text = XmStringCreateSimple( "MIN" ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightOffset,     3                           ); n++;
        XtSetArg ( args[n], XmNwidth,           54                          ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
        
        text = XmStringCreateSimple( "COEF" ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label                       ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           54                          ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
        
        text = XmStringCreateSimple( " " ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label                       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
   
        n = 0;
        row = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, frame_form,                                          XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       form,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNbottomAttachment,XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           NULL) ;

        /* Noise Score */
        n = 0;
        form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, row, args, n ) ;

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM                 ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_min_noise()     ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasNoiseMinBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname); tname = NULL;
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasNoiseMinBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_min_noise());
        XmTextSetString (spinbox, buf);
#endif
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET               ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                        ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                             ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;


#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_coef_noise()    ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasNoiseCoefBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname); tname = NULL;
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasNoiseCoefBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_coef_noise());
        XmTextSetString (spinbox, buf);
#endif

        text = XmStringCreateSimple( "Noise" ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       formsb                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    formsb                      ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
        
        
        /* Interval Score */
        n = 0;
        form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, row, args, n ) ;

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM                 ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        False                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_min_interval()  ); n++;
        XtSetArg ( args[n], XmNwrap,            False                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasIntervalMinBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname);
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasIntervalMinBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_min_interval());
        XmTextSetString (spinbox, buf);
#endif
                
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET               ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                        ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                             ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_coef_interval() ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasIntervalCoefBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname); tname = NULL;
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasIntervalCoefBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_coef_interval());
        XmTextSetString (spinbox, buf);
#endif
      
        text = XmStringCreateSimple( "Interval" ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       formsb                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    formsb                      ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
      
        /* Crosstalk Score */
        n = 0;
        form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, row, args, n ) ;

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM                 ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_min_ctk()       ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasCtkMinBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname);
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasCtkMinBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_min_ctk());
        XmTextSetString (spinbox, buf);
#endif
         
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET               ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                        ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                             ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_coef_ctk()      ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasCtkCoefBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname); tname = NULL;
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasCtkCoefBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_coef_ctk());
        XmTextSetString (spinbox, buf);
#endif
 
        text = XmStringCreateSimple( "Crosstalk" ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       formsb                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    formsb                      ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
        
        /* Activity Score */
        n = 0;
        form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, row, args, n ) ;

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM                 ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_min_activity()  ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasActivityMinBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname);
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasActivityMinBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_min_activity());
        XmTextSetString (spinbox, buf);
#endif
              
       n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM                 ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET               ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                        ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                             ); n++;
        formsb = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;

#if XTAS_MOTIF_VERSION >= 20100        
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNcolumns,         2                           ); n++;
        XtSetArg ( args[n], XmNspinBoxChildType,XmNUMERIC                   ); n++;  
        XtSetArg ( args[n], XmNminimumValue,    0                           ); n++;
        XtSetArg ( args[n], XmNmaximumValue,    10                          ); n++;
        XtSetArg ( args[n], XmNincrementValue,  1                           ); n++;
        XtSetArg ( args[n], XmNeditable,        FALSE                       ); n++;
        XtSetArg ( args[n], XmNpositionType,    XmPOSITION_VALUE            ); n++;
        XtSetArg ( args[n], XmNposition,        stb_ctk_get_coef_activity() ); n++;
        XtSetArg ( args[n], XmNwrap,            FALSE                       ); n++;
        spinbox = XtCreateManagedWidget ("XtasActivityCoefBox", xmSimpleSpinBoxWidgetClass, formsb, args, n);
        spinname = XtName (spinbox);
        tname = XtMalloc ((unsigned) strlen (spinname) + 4);
        sprintf (tname, "%s_TF", spinname);
        spin_TF = XtNameToWidget (spinbox, tname);
        XtVaSetValues (spin_TF, XmNcursorPositionVisible, False, NULL);
        XtFree (tname); tname = NULL;
#else
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,         XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNwidth,                   55                          ); n++;
        XtSetArg ( args[n], XmNeditable,                True                        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   True                        ); n++;
        spinbox = XtCreateManagedWidget ("XtasActivityCoefBox", xmTextWidgetClass, formsb, args, n);
        sprintf (buf, "%d", stb_ctk_get_coef_activity());
        XmTextSetString (spinbox, buf);
#endif
 
        text = XmStringCreateSimple( "Activity" ) ;
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       formsb                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    formsb                      ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     formsb                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form, args, n ) ;
        
        
    }
    
    XtManageChild (XtasNoiseScoresWidget);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCtkInfoOkCallback                                          */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .client_data :                                                 */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the button "OK" of Crosstalk         */
/*            informations window.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void  XtasCtkInfoOkCallback(parent, client_data, call_data)
    Widget  parent;
    XtPointer client_data ;
    XtPointer call_data ;
{
    XtDestroyWidget( (Widget)client_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCtkInfoOkCallback                                          */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .client_data :                                                 */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the button "OK" of Crosstalk         */
/*            informations window.                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int XtasSortAgrList (stb_ctk_detail_agr_list **p1, stb_ctk_detail_agr_list **p2)
{
    if ((*p1)->CC > (*p2)->CC) return -1;
    if ((*p1)->CC < (*p2)->CC) return +1;
    return 0;
}

void  XtasFillAgressorsList(agr_list, nb_agr, list_widget, length1, length2)
    stb_ctk_detail_agr_list *agr_list;    
    int nb_agr;
    Widget  list_widget;
    int length1;
    int length2;
{
    stb_ctk_detail_agr_list     *p;
    char                         textline[2048];
    char                        *name;
    char                        *netname;
    char                         b, w, r, f;
    XmString                     text;
    int                          c;

    stb_ctk_detail_agr_list     **tab;

    XmListDeleteAllItems(list_widget) ;


    if(agr_list == NULL) {
        text = XmStringCreateSimple ("no aggressor") ;
        XmListAddItemUnselected (list_widget, text, c) ;
        XmStringFree (text) ;
    }

    tab = (stb_ctk_detail_agr_list**)mbkalloc(sizeof(stb_ctk_detail_agr_list*)*nb_agr);
    
    for ( c=0, p=agr_list ; p ; p=p->NEXT, c++) 
        tab[c] = p; 

    qsort (tab, nb_agr, sizeof(stb_ctk_detail_agr_list*), (int (*)(const void *, const void*))XtasSortAgrList);

    for ( c = 0 ; c < nb_agr ; c++) {
        if(tab[c]->TTVAGR)
            name = XtasPlaceString(tab[c]->TTVAGR->NAME,length1);
        else
            name = XtasPlaceString("",length1);

        netname = XtasPlaceString(tab[c]->NETNAME,length2);

        if( tab[c]->ACT_WORST ) 
          w='W';
        else {
          if( tab[c]->ACT_MUTEX_WORST )
            w='w';
          else
            w=' ';
        }
        
        if( tab[c]->ACT_BEST ) 
          b='B';
        else {
          if( tab[c]->ACT_MUTEX_BEST )
            b='b';
          else
            b=' ';
        }
        
        if( tab[c]->NOISE_RISE_PEAK ) 
          r='R';
        else {
          if( tab[c]->NOISE_RISE_EXCLUDED )
            r='r';
          else
            r=' ';
        }
        
        if( tab[c]->NOISE_FALL_PEAK ) 
          f='F';
        else {
          if( tab[c]->NOISE_FALL_EXCLUDED )
            f='f';
          else
            f=' ';
        }

        sprintf(textline,"%s %s  %c%c%c%c  %.3ffF",name, netname, b, w, r, f, ((tab[c]->CC)*1000.0));
        mbkfree(name);
        mbkfree(netname);

        text = XmStringCreateSimple (textline) ;
        XmListAddItemUnselected (list_widget, text, c+1) ;
        XmStringFree (text) ;

    }
    mbkfree(tab);

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCtkInfoDetailCallback                                      */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCtkInfoDetailCallback(parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data ;
    Widget      widget = (Widget)client_data; 
    char        *s;
    char        *item;
    
    XalSetCursor( widget, WAIT ); 
    XalForceUpdate(  widget );
    
    XmStringGetLtoR( cbs->item, XmSTRING_DEFAULT_CHARSET, &item );
    s = strchr (item, ' ');
    if (s)  *s = '\0';
    if (strlen(item) != 0)
        XtasSignalsDetail( widget, item);
    else
        XalDrawMessage (XtasErrorWidget, "Cannot display information on signal: no signame.");
    XalSetCursor( widget, NORMAL ); 
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCtkInfoColumnsTitleMoveWithHSBCallback                     */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .client_data :                                                 */
/*            .call_data   : Toolkit informations.                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCtkInfoColumnsTitleMoveWithHSBCallback( hsb, label, call_data)
    Widget      hsb;
    XtPointer   label;
    XtPointer   call_data;
{
    int         loffset ;
    int         value;

    loffset = XTAS_CTK_OFFSET;               
                                                          
    XtVaGetValues (hsb, XmNvalue, &value, NULL);          

    XtVaSetValues ((Widget)label, XmNleftOffset, (loffset - value), NULL); 

}

int XtasAlphaStrCmp (char **s1, char **s2)
{
    return strcmp(*s1, *s2);
}

int XtasReverseAlphaStrCmp (char **s1, char **s2)
{
    return strcmp(*s2, *s1);
}

int XtasAscendCapaValueCmp (char **s1, char **s2)
{
    char    *c, *end;
    float   f1, f2;

    c = strrchr (*s1, ' ');
    f1 = strtod (c, &end);
    c = strrchr (*s2, ' ');
    f2 = strtod (c, &end);

    if (f1<f2) return -1;
    if (f1>f2) return +1;
    return 0;
}
int XtasDescendCapaValueCmp (char **s1, char **s2)
{
    char    *c, *end;
    float   f1, f2;

    c = strrchr (*s1, ' ');
    f1 = strtod (c, &end);
    c = strrchr (*s2, ' ');
    f2 = strtod (c, &end);

    if (f1>f2) return -1;
    if (f1<f2) return +1;
    return 0;
}

void XtasCtkInfoSortByNameCallback (parent, list_w, call_data)
    Widget      parent;
    XtPointer   list_w;
    XtPointer   call_data;
{
   char   **tabagressors;
   char    *buf;
   int      i, c;  

   XmStringTable    itemTable;
   XmString         motif_string;
   static   int     click = -1;
   Widget           Arrow;
   
   XtVaGetValues (list_w, XmNitems, &itemTable, XmNitemCount, &c, NULL);
   click = -click;

   tabagressors = (char**)mbkalloc (sizeof(char *)*c);
    
   for (i=0 ; i<c ; i++) {
        XmStringGetLtoR( itemTable[i], XmSTRING_DEFAULT_CHARSET, &tabagressors[i] );
   }

   if (click > 0) {
       qsort( tabagressors, c, sizeof(char*), (int (*)(const void*, const void*))XtasAlphaStrCmp);
//       XtVaSetValues (XtNameToWidget (XtParent(parent), "*XtasArrowSigButton"), XmNarrowDirection, XmARROW_DOWN, NULL);
   }
   else {
       qsort( tabagressors, c, sizeof(char*), (int (*)(const void*, const void*))XtasReverseAlphaStrCmp);
//       XtVaSetValues (XtNameToWidget (XtParent(parent), "*XtasArrowSigButton"), XmNarrowDirection, XmARROW_UP, NULL);
   }

   XmListDeleteAllItems (list_w);
   for (i=0; i<c ; i++) {
        motif_string = XmStringCreateSimple (tabagressors[i]) ;
        XmListAddItemUnselected (list_w, motif_string, i+1) ;
        XmStringFree ( motif_string ) ;        
   }

   mbkfree (tabagressors);
}

void XtasCtkInfoSortByCapaValueCallback (parent, list_w, call_data)
    Widget      parent;
    XtPointer   list_w;
    XtPointer   call_data;
{
   char   **tabagressors;
   char    *buf;
   int      i, c;  

   XmStringTable    itemTable;
   XmString         motif_string;
   static   int     click = -1;
   
   XtVaGetValues (list_w, XmNitems, &itemTable, XmNitemCount, &c, NULL);

   click = -click;
   
   tabagressors = (char**)mbkalloc (sizeof(char *)*c);
    
   for (i=0 ; i<c ; i++) {
        XmStringGetLtoR( itemTable[i], XmSTRING_DEFAULT_CHARSET, &tabagressors[i] );
   }

   if (click > 0)
       qsort( tabagressors, c, sizeof(char*), (int (*)(const void*, const void*))XtasDescendCapaValueCmp);
   else
       qsort( tabagressors, c, sizeof(char*), (int (*)(const void*, const void*))XtasAscendCapaValueCmp);

   XmListDeleteAllItems (list_w);
   for (i=0; i<c ; i++) {
        motif_string = XmStringCreateSimple (tabagressors[i]) ;
        XmListAddItemUnselected (list_w, motif_string, i+1) ;
        XmStringFree ( motif_string ) ;        
   }

   mbkfree (tabagressors);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDisplayCtkInfo                                             */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .client_data :                                                 */
/*            .call_data   : Toolkit informations.                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The drawing function for crosstalk informations                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDisplayCtkInfo( parent, event, errorWidget)
    Widget                   parent;
    ttvevent_list           *event;
    XalMessageWidgetStruct  *errorWidget;
{
    int             n ;
    Arg             args[20];
    XmString        text;
    Widget          form, sub_form, sub_sub_form, frame, frame2, label_widget ;
    Widget          ctkInfoWidget, tmp_widget, row_widget ;
    Widget          paned_window, form1_window, list_widget;
    stb_ctk_detail *detail = NULL;
    char            buf[1024];
    int             lengthName, lengthNetname, length;
    stb_ctk_detail_agr_list *p;
    Pixmap          r_pixmap, f_pixmap, v_pixmap;
    Pixel           tas_state_color,tas_backg_color ;
    char            text_title[128];
    Widget          SIG_label_widget,/* ARROWSIG_label_widget, */CAPA_label_widget, scrollW;
    int             nb_agr;
    ttvevent_list  *stbevent;
    ttvfig_list    *ttvfig;
    ttvline_list   *line;

    XtasPasqua();
    if(sigsetjmp( XtasMyEnv , 1 ) == 0) {
        stbevent = event ;
        if( ( event->ROOT->TYPE & TTV_SIG_CO ) == TTV_SIG_CO ) {
            ttvfig = XtasMainParam->stbfig->FIG ;
            ttv_expfigsig( ttvfig, event->ROOT, ttvfig->INFO->LEVEL, ttvfig->INFO->LEVEL, TTV_STS_T, TTV_FILE_DTX);
            for( line = event->INLINE ; line ; line = line->NEXT ) {
              if ( ( line->TYPE & TTV_LINE_RC) == TTV_LINE_RC ) 
                break;
            }
            if( line )
              stbevent = event->INLINE->NODE ;
        }
        detail = stb_ctk_get_detail(XtasMainParam->stbfig, stbevent);
    }
    
    else {
        XtasFirePasqua();
        return ;
    }
    XtasGetWarningMess ();
    XtasFirePasqua();

    if(detail != NULL) {
        n = 0;
        sprintf (text_title, XTAS_NAME": Crosstalk Infos - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
//        XtSetArg( args[n], XmNtitle,    XTAS_NAME": Crosstalk Infos" ); n++;
        XtSetArg( args[n], XmNtitle, text_title ); n++;
        XtSetArg( args[n], XmNminWidth,  450 ); n++;
        XtSetArg( args[n], XmNminHeight, 410 ); n++;
        XtSetArg( args[n], XmNmaxHeight, 410 ); n++;
        XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
        ctkInfoWidget = XmCreatePromptDialog( parent, "XtasInfosBox", args, n);
        HelpFather = ctkInfoWidget ;
        XtUnmanageChild( XmSelectionBoxGetChild( ctkInfoWidget, XmDIALOG_TEXT));
        XtUnmanageChild( XmSelectionBoxGetChild( ctkInfoWidget, XmDIALOG_PROMPT_LABEL));
        XtUnmanageChild( XmSelectionBoxGetChild( ctkInfoWidget, XmDIALOG_CANCEL_BUTTON ));
        XtAddCallback( ctkInfoWidget, XmNokCallback, XtasCtkInfoOkCallback, (XtPointer)ctkInfoWidget );
        XtAddCallback( ctkInfoWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_CTKINFO)) ;
        n = 0;
        form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, ctkInfoWidget, args, n );


        /*------------- frame Noise -------------*/
        n = 0;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM           ); n++;
        frame2 = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000        
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else        
        XtSetArg ( args[n], XmNchildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#endif       
        sub_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame2, args, n );


        text = XmStringCreateSimple("Noise");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                    ); n++;
#if XTAS_MOTIF_VERSION >= 20000        
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD     ); n++;
#else        
        XtSetArg ( args[n], XmNchildType,  XmFRAME_TITLE_CHILD          ); n++;
#endif
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, frame2, args, n );
        XmStringFree(text);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
        sub_sub_form = XtCreateManagedWidget ("XtasSubSubForm", xmFormWidgetClass, sub_form, args, n );


        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              8                               ); n++;
        XtSetArg( args[n], XmNrightAttachment,      XmATTACH_FORM                   ); n++;
        XtSetArg( args[n], XmNrightOffset,          95                              ); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_sub_form, args, n);
        sprintf(buf,"%.3f V",(detail->NOISE_VTH));
        XmTextSetString( tmp_widget, buf );
        
        
        text = XmStringCreateSimple("Vth:");
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           tmp_widget          ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        tmp_widget          ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNrightWidget,         tmp_widget          ); n++;
        XtSetArg ( args[n], XmNrightOffset,         10                  ); n++;
        XtSetArg ( args[n], XmNlabelString,         text                    ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_sub_form, args, n );       


        
        
        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, sub_form,
                XmNtopAttachment,   XmATTACH_WIDGET,
                XmNtopWidget,       sub_sub_form,
                XmNtopOffset,       5,
                XmNbottomAttachment,XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNpacking,         XmPACK_COLUMN,
                XmNorientation,     XmHORIZONTAL,
                XmNentryAlignment,  XmALIGNMENT_CENTER,
                XmNnumColumns,      4,
                XmNisAligned,       True,
                XmNspacing,         5,
                NULL);

        n = 0;
        sub_sub_form = XtCreateManagedWidget( "XtasForm", xmFormWidgetClass, row_widget, args, n);

        n = 0;
        text = XmStringCreateSimple("Rise Peak");
        XtSetArg( args[n], XmNlabelString,  text                ); n++;
        XtSetArg( args[n], XmNalignment,    XmALIGNMENT_CENTER  ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree(text);

        n = 0;
        text = XmStringCreateSimple("Fall Peak");
        XtSetArg( args[n], XmNlabelString,  text                ); n++;
        XtSetArg( args[n], XmNalignment,    XmALIGNMENT_CENTER  ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree(text);

        n = 0;
        text = XmStringCreateSimple("Model");
        XtSetArg( args[n], XmNlabelString, text); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree(text);

        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              XTAS_NOISE_LABEL_MODEL_WIDTH_G / 7); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, row_widget, args, n);
        XmTextSetString( tmp_widget, detail->MODEL_OVR );

        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              XTAS_NOISE_LABEL_MODEL_WIDTH_G / 7); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, row_widget, args, n);
        XmTextSetString( tmp_widget, detail->MODEL_UND );

        n = 0;
        text = XmStringCreateSimple("Max Noise");
        XtSetArg( args[n], XmNlabelString, text); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree(text);

        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              XTAS_NOISE_LABEL_NOISE_WIDTH_G / 7); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, row_widget, args, n);
        sprintf(buf,"%.0f mV",(detail->NOISE_MAX_OVR*1000.0));
        XmTextSetString( tmp_widget, buf );

        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              XTAS_NOISE_LABEL_NOISE_WIDTH_G / 7); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, row_widget, args, n);
        sprintf(buf,"%.0f mV",(detail->NOISE_MAX_UND*1000.0));
        XmTextSetString( tmp_widget, buf );

        n = 0;
        text = XmStringCreateSimple("Real Noise");
        XtSetArg( args[n], XmNlabelString, text); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree(text);

        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              XTAS_NOISE_LABEL_NOISE_WIDTH_G / 7); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, row_widget, args, n);
        sprintf(buf,"%.0f mV",(detail->NOISE_OVR*1000.0));
        XmTextSetString( tmp_widget, buf );

        n = 0;
        XtSetArg( args[n], XmNshadowThickness,      1                               ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                           ); n++;
        XtSetArg( args[n], XmNeditable,             False                           ); n++;
        XtSetArg( args[n], XmNcolumns,              XTAS_NOISE_LABEL_NOISE_WIDTH_G / 7); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, row_widget, args, n);
        sprintf(buf,"%.0f mV",(detail->NOISE_UND*1000.0));
        XmTextSetString( tmp_widget, buf );

        /*------------- frame Agressors list -------------*/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET ); n++;
        XtSetArg ( args[n], XmNleftWidget,      frame2          ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5               ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET   ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    frame2                     ); n++;
        frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg (args[n], XmNframeChildType,    XmFRAME_WORKAREA_CHILD  ); n++;
#else
        XtSetArg (args[n], XmNchildType,    XmFRAME_WORKAREA_CHILD  ); n++;
#endif
        sub_form = XtCreateManagedWidget("XtasForm",xmFormWidgetClass, frame, args, n);

        
        text = XmStringCreateSimple("Agressors List");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                    ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD  ); n++;
#else
        XtSetArg ( args[n], XmNchildType,  XmFRAME_TITLE_CHILD  ); n++;
#endif
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_CENTER      ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, frame, args, n );
        XmStringFree(text);

        /* calcul des largeurs max pour le name et le netname */
        lengthName      = XTAS_MIN_CARLENGTH;       
        lengthNetname   = XTAS_MIN_CARLENGTH;       

        for (nb_agr=0, p=detail->AGRLIST ; p ; p=p->NEXT, nb_agr++) {
            if(p->TTVAGR)
                if((length = strlen(p->TTVAGR->NAME)) >= lengthName)
                    lengthName = length;
            if((length = strlen(p->NETNAME)) >= lengthNetname)
                lengthNetname = length;
        }

        if(lengthName >= XTAS_MAX_CARLENGTH)
            lengthName = XTAS_MAX_CARLENGTH;
        if(lengthNetname >= XTAS_MAX_CARLENGTH)
            lengthNetname = XTAS_MAX_CARLENGTH;


        text = XmStringCreateSimple("Signal Name");
        n = 0;
        XtSetArg (args[n], XmNtopAttachment,     XmATTACH_FORM      ); n++;
        XtSetArg (args[n], XmNtopOffset,         7                  ); n++;
    //    XtSetArg (args[n], XmNtopWidget,         frame2          ); n++;
        XtSetArg (args[n], XmNleftAttachment,    XmATTACH_FORM      ); n++;
        XtSetArg (args[n], XmNleftOffset,        XTAS_CTK_OFFSET    ); n++;
        XtSetArg (args[n], XmNlabelString,       text               ); n++;
        XtSetArg (args[n], XmNwidth,             lengthName*7       ); n++;    /* 7 = largeur max d'un caractere */
        SIG_label_widget = XtCreateManagedWidget("XtasSigButton", xmPushButtonWidgetClass, sub_form, args, n);
        XmStringFree(text);
/*        n = 0;
        XtSetArg (args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg (args[n], XmNtopWidget,        SIG_label_widget            ); n++;
        XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg (args[n], XmNbottomWidget,     SIG_label_widget            ); n++;
        XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
        XtSetArg (args[n], XmNleftWidget,       SIG_label_widget            ); n++;
        XtSetArg (args[n], XmNarrowDirection,   XmARROW_UP                  ); n++;
        ARROWSIG_label_widget = XtCreateManagedWidget("XtasArrowSigButton", xmArrowButtonWidgetClass, sub_form, args, n);
*/   
        text = XmStringCreateSimple("Net Name");
        n = 0;
        XtSetArg (args[n], XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET   ); n++;
        XtSetArg (args[n], XmNtopWidget,         SIG_label_widget           ); n++;
        XtSetArg (args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET   ); n++;
        XtSetArg (args[n], XmNbottomWidget,      SIG_label_widget           ); n++;
        XtSetArg (args[n], XmNleftAttachment,    XmATTACH_WIDGET            ); n++;
        XtSetArg (args[n], XmNleftWidget,        SIG_label_widget           ); n++;
        XtSetArg (args[n], XmNleftOffset,        7                          ); n++;
        XtSetArg (args[n], XmNlabelString,       text                       ); n++;
        XtSetArg (args[n], XmNwidth,             lengthNetname*7            ); n++;    /* 7 = largeur max d'un caractere */
        label_widget = XtCreateManagedWidget("XtasLabel", xmLabelWidgetClass, sub_form, args, n);
        XmStringFree(text);

        text = XmStringCreateSimple(" ");
        n = 0;
        XtSetArg (args[n], XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET   ); n++;
        XtSetArg (args[n], XmNtopWidget,         label_widget               ); n++;
        XtSetArg (args[n], XmNleftAttachment,    XmATTACH_WIDGET            ); n++;
        XtSetArg (args[n], XmNleftWidget,        label_widget               ); n++;
        XtSetArg (args[n], XmNleftOffset,        2*7                        ); n++;
        XtSetArg (args[n], XmNlabelString,       text                       ); n++;
        XtSetArg (args[n], XmNwidth,             4*7                        ); n++;    /* 7 = largeur max d'un caractere */
        label_widget = XtCreateManagedWidget("XtasLabel", xmLabelWidgetClass, sub_form, args, n);
        XmStringFree(text);

        text = XmStringCreateSimple("Capa");
        n = 0;
        XtSetArg (args[n], XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET   ); n++;
        XtSetArg (args[n], XmNtopWidget,         label_widget               ); n++;
        XtSetArg (args[n], XmNleftAttachment,    XmATTACH_WIDGET            ); n++;
        XtSetArg (args[n], XmNleftWidget,        label_widget               ); n++;
        XtSetArg (args[n], XmNleftOffset,        2*7                        ); n++;
        XtSetArg (args[n], XmNlabelString,       text                       ); n++;
        XtSetArg (args[n], XmNwidth,             8*7                       ); n++;    /* 7 = largeur max d'un caractere */
        CAPA_label_widget = XtCreateManagedWidget("XtasLabel", xmPushButtonWidgetClass, sub_form, args, n);
        XmStringFree(text);

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget,        CAPA_label_widget    ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
        paned_window = XtCreateManagedWidget( "XtasPanedWindow",
                xmPanedWindowWidgetClass,
                sub_form,
                args, n );

        form1_window = XtVaCreateManagedWidget (       "XtasSubForm",
                xmFormWidgetClass,
                paned_window,
                NULL) ; 

        n=0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;       
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;     
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;     
        XtSetArg( args[n], XmNvisibleItemCount, 15              ); n++;
        XtSetArg( args[n], XmNscrollBarPlacement,       XmBOTTOM_RIGHT  ); n++;
        XtSetArg( args[n], XmNscrollBarDisplayPolicy,   XmAS_NEEDED     ); n++;
        XtSetArg( args[n], XmNscrollingPolicy,  XmAUTOMATIC             ); n++;      
        list_widget = XmCreateScrolledList(  form1_window, "XtasTextualDisplay", args, n );

        scrollW = XtParent (list_widget); /* on recupere la ScrolledWindow associee a la ScrolledList */
        XtVaGetValues (scrollW, XmNhorizontalScrollBar, &tmp_widget, NULL);
        XtAddCallback (tmp_widget, XmNvalueChangedCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNdragCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNincrementCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNdecrementCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNpageIncrementCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNpageDecrementCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNtoTopCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
        XtAddCallback (tmp_widget, XmNtoBottomCallback, XtasCtkInfoColumnsTitleMoveWithHSBCallback, SIG_label_widget);
           
        XtManageChild( list_widget );
        XtasFillAgressorsList(detail->AGRLIST, nb_agr, list_widget, lengthName, lengthNetname); 
        XtAddCallback (list_widget, XmNdefaultActionCallback, XtasCtkInfoDetailCallback, ctkInfoWidget);
        
        XtAddCallback (SIG_label_widget, XmNactivateCallback, XtasCtkInfoSortByNameCallback, list_widget);
//        XtAddCallback (ARROWSIG_label_widget, XmNactivateCallback, XtasCtkInfoSortByNameCallback, list_widget);
        XtAddCallback (CAPA_label_widget, XmNactivateCallback, XtasCtkInfoSortByCapaValueCallback, list_widget);



        /*------------- frame General -------------*/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       frame                       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNleftWidget,      frame2                      ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNrightWidget,     frame2                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    frame2                      ); n++;
        XtSetArg ( args[n], XmNbottomOffset,    5                           ); n++;
        frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );

        text = XmStringCreateSimple ("General");
        n= 0;
        XtSetArg ( args[n], XmNlabelString,     text                ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_widget = XtCreateManagedWidget ("XatsLabels", xmLabelWidgetClass, frame, args, n);
        XmStringFree(text);

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg( args[n], XmNframeChildType,   XmFRAME_WORKAREA_CHILD  ); n++;
#else        
        XtSetArg( args[n], XmNchildType,   XmFRAME_WORKAREA_CHILD  ); n++;
#endif        
        sub_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame, args, n );


        text = XmStringCreateSimple ("Signal:");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     text                    ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNtopOffset,       7                       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNleftOffset,      7                       ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                    ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        XtSetArg ( args[n], XmNheight,          25                      ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
        XmStringFree(text);

        XtVaGetValues(sub_form,XmNforeground,&tas_state_color,
                XmNbackground,&tas_backg_color,NULL );
        r_pixmap = XalGetPixmap( sub_form, XTAS_REDG_MAP, tas_state_color, tas_backg_color );
        f_pixmap = XalGetPixmap( sub_form, XTAS_FEDG_MAP, tas_state_color, tas_backg_color );

        if ( (r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP) )
        {
            text = ( ( event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? XmStringCreateSimple("(Rising)") : XmStringCreateSimple("(Falling)"); 
            tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                    xmLabelWidgetClass,
                    sub_form,
                    XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
                    XmNtopWidget,      label_widget,
                    XmNlabelString,    text,
                    XmNleftAttachment, XmATTACH_WIDGET,
                    XmNleftWidget,     label_widget,
                    XmNleftOffset,     5,
                    XmNwidth,          40,
                    XmNheight,         25,
                    NULL);
            XmStringFree( text );
        }
        else
        {
            v_pixmap = ( (event->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? r_pixmap : f_pixmap;
            tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                    xmLabelWidgetClass,
                    sub_form,
                    XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
                    XmNtopWidget,      label_widget,
                    XmNlabelType,      XmPIXMAP,
                    XmNlabelPixmap,    v_pixmap,
                    XmNleftAttachment, XmATTACH_WIDGET,
                    XmNleftWidget,     label_widget,
                    XmNleftOffset,     5,
                    XmNwidth,          40,
                    XmNheight,         25,
                    NULL);
        }
       
        ttv_getsigname (XtasMainParam->stbfig->FIG, buf, event->ROOT);
        text = XmStringCreateSimple (buf);
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
        XtSetArg ( args[n], XmNtopWidget,       tmp_widget              ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET         ); n++;
        XtSetArg ( args[n], XmNleftWidget,      tmp_widget              ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                       ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                    ); n++;
        XtSetArg ( args[n], XmNheight,          25                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
        XmStringFree(text);

        text = XmStringCreateSimple( "Ground Capacitance:" );
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET         ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_widget            ); n++;
        XtSetArg ( args[n], XmNtopOffset,       5                       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNleftOffset,      7                       ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                    ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           150                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
        XmStringFree(text);

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,        XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg( args[n], XmNtopWidget,            label_widget                ); n++;
        XtSetArg( args[n], XmNbottomAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg( args[n], XmNbottomWidget,         label_widget                ); n++;
        XtSetArg( args[n], XmNleftAttachment,       XmATTACH_WIDGET             ); n++;
        XtSetArg( args[n], XmNleftWidget,           label_widget                ); n++;
        XtSetArg( args[n], XmNleftOffset,           5                           ); n++;
        XtSetArg( args[n], XmNshadowThickness,      1                           ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                       ); n++;
        XtSetArg( args[n], XmNeditable,             False                       ); n++;
        XtSetArg( args[n], XmNcolumns,              10                          ); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);
        sprintf(buf,"%.3f fF",(detail->CM*1000.0));
        XmTextSetString( tmp_widget, buf );

        text = XmStringCreateSimple( "Crosstalk Capacitance:" );
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_widget                ); n++;
        XtSetArg ( args[n], XmNtopOffset,       5                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNleftOffset,      7                           ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNheight,          30                          ); n++;
        XtSetArg ( args[n], XmNwidth,           150                         ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
        XmStringFree(text);


        n = 0;
        XtSetArg( args[n], XmNtopAttachment,        XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg( args[n], XmNtopWidget,            label_widget                ); n++;
        XtSetArg( args[n], XmNbottomAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg( args[n], XmNbottomWidget,         label_widget                ); n++;
        XtSetArg( args[n], XmNleftAttachment,       XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg( args[n], XmNleftWidget,           tmp_widget                  ); n++;
        XtSetArg( args[n], XmNshadowThickness,      1                           ); n++;
        XtSetArg( args[n], XmNcursorPositionVisible,False                       ); n++;
        XtSetArg( args[n], XmNeditable,             False                       ); n++;
        XtSetArg( args[n], XmNcolumns,              10                          ); n++;
        tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);
        sprintf(buf,"%.3f fF",(detail->CC*1000.0));
        XmTextSetString( tmp_widget, buf );


        stb_ctk_free_detail(XtasMainParam->stbfig,detail);
        XtManageChild( ctkInfoWidget );

    }
    else
        XalDrawMessage(errorWidget,
                "Crosstalk informations display impossible:\nno information.");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseCloseCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .top : The window toplevel widget id.                          */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseCloseCallback( widget, client_data , call_data )
    Widget widget ;
    XtPointer client_data  ;
    XtPointer call_data ;
{
    XtUnmanageChild (XtasNoiseResultsWidget);
    XtDestroyWidget (XtasNoiseResultsWidget);
    if (XtasNoiseScoresWidget) {
        XtDestroyWidget (XtasNoiseScoresWidget);
        XtasNoiseScoresWidget = NULL;
    }
    XtasNoiseResultsWidget = NULL;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseDetailCallback                                        */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseDetailCallback(parent, client_data, call_data)
    Widget      parent      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    XtasWindowStruct        *tas_winfos = (XtasWindowStruct *)client_data;
    char buf[1024];

    if (XtasNoiseSelectedIndex == XTAS_NOISE_NO_SELECT )
        XalDrawMessage( tas_winfos->errwidget, XTAS_NSIGERR );
    else {
        XalSetCursor( tas_winfos->topwidget, WAIT ); 
        XalForceUpdate(  tas_winfos->topwidget );
        ttv_getsigname (XtasMainParam->stbfig->FIG, buf, XtasNoiseTab->TAB[XtasNoiseSelectedIndex].NODE->ROOT);
        XtasSignalsDetail( tas_winfos->wrkwidget, buf);
        XalSetCursor( tas_winfos->topwidget, NORMAL ); 
    }

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseButtonsCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Button's command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseButtonsCallback( widget, client_data, call_data )
    Widget           widget;
    XtPointer client_data;
    XtPointer call_data;
{
    XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;
    XtasOptionsButtonsTreat( widtab->wrkwidget, &widtab->butwidget,
                XtasNoiseButtonMenus,  XtasNoiseButtonOptions );
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseCtkInfoCallback                                       */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseCtkInfoCallback(parent, client_data, call_data)
    Widget      parent      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    XtasWindowStruct        *tas_winfos = (XtasWindowStruct *)client_data;
        
    if (XtasNoiseSelectedIndex == XTAS_NOISE_NO_SELECT )
            XalDrawMessage( tas_winfos->errwidget, XTAS_NSIGERR );
    else {
        XalSetCursor( tas_winfos->topwidget, WAIT );
        XtasSetLabelString( tas_winfos->meswidget, "Looking for Crosstalk Informations ...");
        XalForceUpdate( tas_winfos->topwidget );

        XtasDisplayCtkInfo(tas_winfos->topwidget, 
                           XtasNoiseTab->TAB[XtasNoiseSelectedIndex].NODE,
                           tas_winfos->errwidget);

        XalSetCursor( tas_winfos->topwidget, NORMAL );
        XtasSetLabelString( tas_winfos->meswidget, " ");
        XalForceUpdate( tas_winfos->topwidget );
    }

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseGetIndexSelectedCallback                              */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseGetSelectedIndexCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    Widget  list_widget = (Widget) client_data;
    int    *pos_list, pos_count;
    if (XmListGetSelectedPos(list_widget, &pos_list, &pos_count))
        XtasNoiseSelectedIndex = pos_list[0]-1;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseGetTextualInfoCallback                                */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseGetTextualInfoCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    char    *signame=(char*)mbkalloc(1024);
    XmListCallbackStruct    *cbs = (XmListCallbackStruct *) call_data;
    Arg                      args[5] ;
    int                      n ;
    static Widget            info_form = NULL, prev_parent = NULL;
  
    
    if ((info_form && (prev_parent == (Widget)client_data)) || (info_form && (XtasNoiseTab->NBDISPLAY==0)))
        XtDestroyWidget ( info_form ) ;

    if (XtasNoiseTab->NBDISPLAY > 0) {
        prev_parent = (Widget)client_data;
        
        n = 0  ;
        XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM ) ; n++ ;
        info_form = XtCreateManagedWidget( "XtasSubForm",
                                           xmFormWidgetClass,
                                           (Widget)client_data, 
                                           args, n ) ;
    
        ttv_getsigname(XtasMainParam->stbfig->FIG, signame, XtasNoiseTab->TAB[(cbs->item_position)-1].NODE->ROOT);
        XtasFillInfosForm (info_form, signame);
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasUpdateNoiseTextualScrollList                               */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasUpdateNoiseTextualScrollList (list)
    Widget  list;
{
    char         buf[1024];
    int          i, position = 0 ;
    XmString     motif_string ;
    char        *signame;
    char        *ovrmod, *undmod;
    ttvsig_list *ptsig;
    char         textline[1024];
    int          score_global, score_noise, score_ctk, score_interval, score_activity;
    int          width;
    Widget       labelSIGNAME;

    XmListDeleteAllItems (list);

    if((XtasNoiseTab == NULL) || (XtasNoiseTab->NBDISPLAY == 0))
    {
        sprintf(textline,"no noise") ;
        motif_string = XmStringCreateSimple(textline) ;

        XmListAddItemUnselected( list, motif_string, ++position ) ;
        XmStringFree( motif_string ) ;
        return ;
    }
    
    /* recherche de la longueur max des noms de signaux */
    for ( i = 0 ; i < XtasNoiseTab->NBDISPLAY ; i++ ) {
        ptsig = XtasNoiseTab->TAB[i].NODE->ROOT;
        ttv_getsigname (XtasMainParam->stbfig->FIG, buf, ptsig);
        if ((width = strlen(buf)) > XtasNoiseLabelSigNameLength ) 
            XtasNoiseLabelSigNameLength = width;
    }
    labelSIGNAME = XtNameToWidget (XtParent(XtParent(XtParent(XtParent(list)))), "*XtasNoiseSigName");
    XtVaSetValues (labelSIGNAME, XmNwidth, ((XtasNoiseLabelSigNameLength + 1) * 7), NULL);
    
    position = 1 ;
    for( i = 0 ; i < XtasNoiseTab->NBDISPLAY ; i++)
    {
        ptsig = XtasNoiseTab->TAB[i].NODE->ROOT;
        ttv_getsigname (XtasMainParam->stbfig->FIG, buf, ptsig);
        signame = XtasPlaceString (buf,             XtasNoiseLabelSigNameLength+2 );
        strcpy ( textline, " " );
        strcat ( textline, XtasNoiseTab->TAB[i].NOISE_MODEL);
        ovrmod  = XtasPlaceString ( textline,  (XTAS_NOISE_LABEL_MODEL_WIDTH_T / 7)  );
        strcpy ( textline, " " );
        strcat ( textline, XtasNoiseTab->TAB[i].NOISE_MODEL);
        undmod  = XtasPlaceString ( textline,  (XTAS_NOISE_LABEL_MODEL_WIDTH_T / 7)  );

        score_global   = stb_ctk_get_score_total (&XtasNoiseTab->TAB[i]);
        score_noise    = stb_ctk_get_score_noise (&XtasNoiseTab->TAB[i]);
        score_interval = stb_ctk_get_score_interval (&XtasNoiseTab->TAB[i]);
        score_ctk      = stb_ctk_get_score_ctk (&XtasNoiseTab->TAB[i]);
        score_activity = stb_ctk_get_score_activity (&XtasNoiseTab->TAB[i]);
         
        sprintf(textline, "%5d   %5s %s   %5s %4.0f mV %4.0f mV   %5s %4.0f mV %4.0f mV   %9d %9d %9d %9d %9d", 
                    i+1, ((XtasNoiseTab->TAB[i].NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? "Low ":"High",
                    signame,
                    ovrmod,XtasNoiseTab->TAB[i].RISE_PEAK_MAX*1000.0, XtasNoiseTab->TAB[i].RISE_PEAK_REAL*1000.0,
                    undmod,XtasNoiseTab->TAB[i].FALL_PEAK_MAX*1000.0, XtasNoiseTab->TAB[i].FALL_PEAK_REAL*1000.0,
                    score_global, score_noise, score_interval, score_ctk, score_activity) ;
        mbkfree(signame) ;
        motif_string = XmStringCreateSimple (textline) ;
        XmListAddItemUnselected (list, motif_string, position) ;
        XmStringFree ( motif_string ) ;
        position++  ;
    }

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateNoiseTextualScrollList                               */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateNoiseTextualScrollList(scform, tas_winfos, label_widget)
    Widget              scform   ;
    XtasWindowStruct   *tas_winfos; 
    Widget              label_widget   ;
{ 
    Arg          args[10]    ;
    int          i, n, position = 0 ;
    Widget       list_form , info_form, scroll, scrollW, tmp_widget ;
    XmString     motif_string ;
    char        *signame;
    char        *ovrmod, *undmod;
    ttvsig_list *ptsig;
    char         textline[1024];
    int          score_global, score_noise, score_interval, score_ctk, score_activity;


    n = 0  ;
    XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM); n++;
    list_form = XtCreateManagedWidget( "XtasSubForm",
            xmFormWidgetClass,
            scform,
            args, n ) ;
    n = 0  ;
    XtSetArg( args[n], XmNheight, 170) ; n++ ;
    info_form = XtCreateManagedWidget( "XtasSubForm",
            xmFormWidgetClass,
            scform,
            args, n ) ;

    n=0 ;
    XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNscrollingPolicy,        XmSTATIC             ) ; n++ ;
    XtSetArg( args[n], XmNvisibleItemCount,       25                   ) ; n++ ;
//    XtSetArg( args[n], XmNheight,                 250                  ) ; n++ ;
    scroll = XmCreateScrolledList( list_form, "XtasTextualDisplay", args , n ) ;
    XmListDeleteAllItems( scroll ) ;

    if((XtasNoiseTab == NULL) || (XtasNoiseTab->NBDISPLAY == 0))
    {
        sprintf(textline,"no noise") ;
        motif_string = XmStringCreateSimple(textline) ;

        XmListAddItemUnselected( scroll, motif_string, ++position ) ;
        XmStringFree( motif_string ) ;
    }
       
    if (XtasNoiseTab == NULL) {
        XtManageChild( scroll) ;
        return scroll;        
    }
        
    position = 1 ;
    for( i = 0 ; i < XtasNoiseTab->NBDISPLAY ; i++)
    {

            ptsig = XtasNoiseTab->TAB[i].NODE->ROOT;
           
            ttv_getsigname (XtasMainParam->stbfig->FIG, textline, ptsig);

            signame = XtasPlaceString (textline,        XtasNoiseLabelSigNameLength+1 );
/*            netname = XtasPlaceString (ptsig->NETNAME,  XtasNoiseLabelNetNameLength+1 ); */
            strcpy ( textline, " " );
            strcat ( textline, XtasNoiseTab->TAB[i].NOISE_MODEL);
            ovrmod  = XtasPlaceString ( textline,  (XTAS_NOISE_LABEL_MODEL_WIDTH_T / 7)  );
            strcpy ( textline, " " );
            strcat ( textline, XtasNoiseTab->TAB[i].NOISE_MODEL);
            undmod  = XtasPlaceString ( textline,  (XTAS_NOISE_LABEL_MODEL_WIDTH_T / 7)  );

            score_global    = stb_ctk_get_score_total (&XtasNoiseTab->TAB[i]);
            score_noise     = stb_ctk_get_score_noise (&XtasNoiseTab->TAB[i]);
            score_interval  = stb_ctk_get_score_interval (&XtasNoiseTab->TAB[i]);
            score_ctk       = stb_ctk_get_score_ctk (&XtasNoiseTab->TAB[i]);
            score_activity  = stb_ctk_get_score_activity (&XtasNoiseTab->TAB[i]);
             
            sprintf(textline, "%5d   %5s %s   %5s %4.0f mV %4.0f mV   %5s %4.0f mV %4.0f mV   %9d %9d %9d %9d %9d", 
                        i+1, ((XtasNoiseTab->TAB[i].NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? "Low ":"High",
                        signame,
                        ovrmod,XtasNoiseTab->TAB[i].RISE_PEAK_MAX*1000.0, XtasNoiseTab->TAB[i].RISE_PEAK_REAL*1000.0,
                        undmod,XtasNoiseTab->TAB[i].FALL_PEAK_MAX*1000.0, XtasNoiseTab->TAB[i].FALL_PEAK_REAL*1000.0,
                        score_global, score_noise, score_interval, score_ctk, score_activity) ;
            mbkfree(signame) ;
            motif_string = XmStringCreateSimple (textline) ;
            XmListAddItemUnselected (scroll, motif_string, position) ;
            XmStringFree ( motif_string ) ;
            position++  ;
    }

//    mbkfree(XtasNoiseTab) ;

    XtAddCallback (scroll,
            XmNbrowseSelectionCallback,
            XtasNoiseGetSelectedIndexCallback,
            (XtPointer)scroll);

    XtAddCallback (scroll,
            XmNbrowseSelectionCallback,
            XtasNoiseGetTextualInfoCallback,
            (XtPointer)info_form);

    XtAddCallback (scroll,
            XmNdefaultActionCallback,
            XtasNoiseCtkInfoCallback,
            (XtPointer)tas_winfos);

    scrollW = XtParent (scroll); /* on recupere la ScrolledWindow associee a la ScrolledList */
    XtVaGetValues (scrollW, XmNhorizontalScrollBar, &tmp_widget, NULL);
    XtAddCallback (tmp_widget, XmNvalueChangedCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNdragCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNincrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNdecrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNpageIncrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNpageDecrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNtoTopCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNtoBottomCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);


    XtManageChild( scroll) ;
    return (scroll);

}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseFocusCallback                                         */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseFocusCallback (parent, client_data, call_data)
    Widget      parent      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    XtVaSetValues (parent, XmNcursorPositionVisible, True, NULL);
    XtasNoiseSelectedIndex = (int)(long) client_data;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasLosingNoiseFocusCallback                                   */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasLosingNoiseFocusCallback (parent, client_data, call_data)
    Widget      parent      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    XtVaSetValues (parent, XmNcursorPositionVisible, False, NULL);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseFillGraphicalList                                     */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseFillGraphicalList ( new, widget, win_struct)
    int     new     ;
    Widget  widget ;
    XtasWindowStruct    *win_struct;
{
    static Widget   row_widget=NULL;
    int             n, i;
    Arg             args[10];
    Widget          sub_form, label_widget, txt_widget;
    char            text[1024];
    XmString        motif_string;
    static XtasWindowStruct *tas_winfos;
    int             position = 0;

    if (row_widget && (new==0)) {
        XtUnmanageChild (row_widget);
        XtDestroyWidget (row_widget);
    }
    else if (new == 1)
        tas_winfos = win_struct;
    else if (new == 2) {
        tas_winfos = win_struct;
        return;
    }

//    if (!row_widget && (new==0)) {
//        sub_form = XtNameToWidget (widget, "*XtasUserField");
//        if (sub_form) {
//            XtUnmanageChild (sub_form);
//            XtDestroyWidget (sub_form);
//        }
//    }

   
    if((XtasNoiseTab == NULL) || (XtasNoiseTab->NBDISPLAY == 0))
    {
        n=0 ;
        XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
        XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
        XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
        XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
        XtSetArg( args[n], XmNrightOffset,            10                   ) ; n++ ;
        XtSetArg( args[n], XmNheight,                 342                  ) ; n++ ;
        
   //     XtSetArg( args[n], XmNvisibleItemCount,       25                   ) ; n++ ;
        row_widget = XmCreateList( widget, "XtasUserField", args , n ) ;
        XmListDeleteAllItems( row_widget ) ;

        sprintf(text,"no noise                                                     \
                                                                                   \
                                                                                   \
       ") ;
        motif_string = XmStringCreateSimple(text) ;

        XmListAddItemUnselected( row_widget, motif_string, ++position ) ;
        XmStringFree( motif_string ) ;
    }

    else {
        row_widget = XtVaCreateWidget( "XtasNoiseRow",
                xmRowColumnWidgetClass,
                widget,
                XmNorientation, XmVERTICAL,
                NULL);

        for ( i = XtasNoiseCurrentTopIndex ; (i < (XtasNoiseCurrentTopIndex+XTAS_NOISE_MAX_NBSIG_WINDOW)) && (i < XtasNoiseTab->NBDISPLAY) ; i++) {
            n = 0;
            sub_form = XtCreateManagedWidget( "XtasSubForm",
                    xmFormWidgetClass,
                    row_widget,
                    args, n );
    
            sprintf(text, "%d", i+1);
            motif_string = XmStringCreateSimple (text);
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNlabelString,     motif_string    ); n++;
            XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END ); n++;
            XtSetArg ( args[n], XmNwidth,           XTAS_NOISE_LABEL_RANK_WIDTH_G ); n++;
            XtSetArg ( args[n], XmNheight,          25              ); n++;
            label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, sub_form, args, n);
            XmStringFree( motif_string );
    
    
            motif_string = ( ( XtasNoiseTab->TAB[i].NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? XmStringCreateSimple("Low ") : XmStringCreateSimple("High"); 
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                    xmLabelWidgetClass,
                    sub_form,
                    XmNtopAttachment,  XmATTACH_FORM,
                    XmNbottomAttachment,  XmATTACH_FORM,
                    XmNlabelString,    motif_string,
                    XmNleftAttachment, XmATTACH_WIDGET,
                    XmNleftWidget,     label_widget,
                    XmNleftOffset,     XTAS_NOISE_OFFSET_GROUP_G,
                    XmNwidth,          XTAS_NOISE_LABEL_TRANS_WIDTH_G,
                    XmNheight,         25,
                    NULL);
            XmStringFree( motif_string );
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            label_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_NAMES_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            ttv_getsigname (XtasMainParam->stbfig->FIG, text, XtasNoiseTab->TAB[i].NODE->ROOT);
            XmTextSetString( txt_widget, text);
    
            XtAddCallback( txt_widget,
                           XmNactivateCallback,
                           XtasNoiseCtkInfoCallback,
                           (XtPointer)tas_winfos );
            XtAddCallback( txt_widget,
                           XmNfocusCallback,
                           XtasNoiseFocusCallback,
                           (XtPointer)(long)i ); 
            XtAddCallback( txt_widget,
                           XmNlosingFocusCallback,
                           XtasLosingNoiseFocusCallback,
                           (XtPointer)(long)i ); 
     
    
    /*
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_NAMES_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            XmTextSetString( txt_widget, XtasNoiseTab[i].NODE->ROOT->NETNAME );
    
            XtAddCallback( txt_widget,
                           XmNactivateCallback,
                           XtasNoiseCtkInfoCallback,
                           (XtPointer)tas_winfos );
            XtAddCallback( txt_widget,
                           XmNfocusCallback,
                           XtasNoiseFocusCallback,
                           (XtPointer)i ); 
            XtAddCallback( txt_widget,
                           XmNlosingFocusCallback,
                           XtasLosingNoiseFocusCallback,
                           (XtPointer)i ); 
    */ 
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_GROUP_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_MODEL_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            XmTextSetString( txt_widget, XtasNoiseTab->TAB[i].NOISE_MODEL );
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_NOISE_WIDTH_G            ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%.0f mV",XtasNoiseTab->TAB[i].RISE_PEAK_MAX*1000.0);
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_NOISE_WIDTH_G            ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%.0f mV",XtasNoiseTab->TAB[i].RISE_PEAK_REAL*1000.0);
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_GROUP_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_MODEL_WIDTH_G            ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            XmTextSetString( txt_widget, XtasNoiseTab->TAB[i].NOISE_MODEL );
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_NOISE_WIDTH_G            ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%.0f mV",XtasNoiseTab->TAB[i].FALL_PEAK_MAX*1000.0);
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_NOISE_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%.0f mV",XtasNoiseTab->TAB[i].FALL_PEAK_REAL*1000.0);
            XmTextSetString( txt_widget, text);
    
    
            /* SCORES */
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_GROUP_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_SCORE_WIDTH_G            ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%9d",stb_ctk_get_score_total (&XtasNoiseTab->TAB[i]));
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_SCORE_WIDTH_G            ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%9d",stb_ctk_get_score_noise (&XtasNoiseTab->TAB[i]));
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_SCORE_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%9d",stb_ctk_get_score_interval (&XtasNoiseTab->TAB[i]));
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_SCORE_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%9d",stb_ctk_get_score_ctk (&XtasNoiseTab->TAB[i]));
            XmTextSetString( txt_widget, text);
    
            n = 0;
            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNleftWidget,            txt_widget    ); n++;
            XtSetArg( args[n], XmNleftOffset,            XTAS_NOISE_OFFSET_G    ); n++;
            XtSetArg( args[n], XmNwidth,                 XTAS_NOISE_LABEL_SCORE_WIDTH_G             ); n++;
            XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
            XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
            XtSetArg( args[n], XmNeditable,              False           ); n++;
            txt_widget = XtCreateManagedWidget( "XtasUserField",
                    xmTextWidgetClass,
                    sub_form,
                    args, n);
            sprintf (text, "%9d",stb_ctk_get_score_activity (&XtasNoiseTab->TAB[i]));
            XmTextSetString( txt_widget, text);
        
        }
    }
    XtManageChild(row_widget);




}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseFillNbVisualizeResults                                */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseFillNbVisualizeResults (new, label_widget)
    int     new         ;
    Widget  label_widget;
{
    static Widget   label   ;
    char            str[50] = "Results       " ;
    char            text[6];
    int             end;


    if (XtasNoiseTab->NBDISPLAY == 0)
        strcpy (str, " ");

    else {
        sprintf (text, "%d", XtasNoiseCurrentTopIndex+1);
        strcat  (str, text);
        strcat  (str, " - ");

        if ((XtasNoiseCurrentTopIndex + XTAS_NOISE_MAX_NBSIG_WINDOW) <= XtasNoiseTab->NBDISPLAY)
            end = XtasNoiseCurrentTopIndex + XTAS_NOISE_MAX_NBSIG_WINDOW;
        else
            end = XtasNoiseTab->NBDISPLAY;
        sprintf (text, "%d", end);
        strcat  (str, text);
        strcat  (str, " / ");

        sprintf (text, "%d", XtasNoiseTab->NBDISPLAY);
        strcat  (str, text);
    }
        
        
    if (new == 1) /*  une nouvelle fenetre de resultats est en train d'etre creee 
                      c'est le premier appel a cette fonction */
        label = label_widget;

    XtasSetLabelString (label, str);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateNoiseGraphicalScrollList                             */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateNoiseGraphicalScrollList(scform, tas_winfos,label_widget)
    Widget              scform    ;
    XtasWindowStruct   *tas_winfos; 
    Widget              label_widget    ;
{ 
    int         n, position;
    Arg         args[10];
    Widget      list_form, scroll, sc_nonoise;
    char        text[512];
    XmString    motif_string;
    Widget      tmp_widget;

    n = 0  ;
    list_form = XtCreateManagedWidget( "XtasSubForm",
            xmFormWidgetClass,
            scform,
            args, n ) ;

    n=0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNscrollingPolicy,  XmSTATIC        ); n++;
    XtSetArg( args[n], XmNheight,           350             ); n++;
    scroll = XmCreateScrolledWindow(  list_form,
                                          "XtasScrollNoiseWidget",
                                           args, n );
    
    
//    if((XtasNoiseTab == NULL) || (XtasNoiseTab->NBDISPLAY == 0))
//    {
//        n=0 ;
//        XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
//        XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
//        XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
//        XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
//        XtSetArg( args[n], XmNrightOffset,            10                   ) ; n++ ;
//        XtSetArg( args[n], XmNheight,                 342                  ) ; n++ ;
//        
//   //     XtSetArg( args[n], XmNvisibleItemCount,       25                   ) ; n++ ;
//        sc_nonoise = XmCreateList( scroll, "XtasUserField", args , n ) ;
//        XmListDeleteAllItems( sc_nonoise ) ;
//
//        sprintf(text,"no noise                                                     \
//                                                                                   \
//                                                                                   \
//       ") ;
//        motif_string = XmStringCreateSimple(text) ;
//
//        XmListAddItemUnselected( sc_nonoise, motif_string, ++position ) ;
//        XmStringFree( motif_string ) ;
//        XtManageChild( sc_nonoise );
//
//    }

    
    XtVaGetValues (scroll, XmNhorizontalScrollBar, &tmp_widget, NULL);
    XtAddCallback (tmp_widget, XmNvalueChangedCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNdragCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNincrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNdecrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNpageIncrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNpageDecrementCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNtoTopCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp_widget, XmNtoBottomCallback, XtasNoiseColumnsTitleMoveWithHSBCallback, label_widget);
  
//    if((XtasNoiseTab == NULL) || (XtasNoiseTab->NBDISPLAY == 0))
//        XtasNoiseFillGraphicalList (2, scroll, tas_winfos);
//    else
        XtasNoiseFillGraphicalList (1, scroll, tas_winfos);
    
    XtManageChild( scroll );

   return (scroll);   

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseTreatEndCallback                                      */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseTreatEndCallback( parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtasNoiseCurrentTopIndex = XtasNoiseTab->NBDISPLAY - XTAS_NOISE_MAX_NBSIG_WINDOW;
    XtasNoiseFillGraphicalList (0, (Widget) client_data, NULL);
    XtasNoiseFillNbVisualizeResults (0, NULL);
    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), False);
    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormPrev"), True);
}
    
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseTreatFastFwdCallback                                  */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseTreatFastFwdCallback( parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtasNoiseCurrentTopIndex += 3 * XTAS_NOISE_MAX_NBSIG_WINDOW;
    if(XtasNoiseCurrentTopIndex > XtasNoiseTab->NBDISPLAY - XTAS_NOISE_MAX_NBSIG_WINDOW) {
        XtasNoiseCurrentTopIndex = XtasNoiseTab->NBDISPLAY - XTAS_NOISE_MAX_NBSIG_WINDOW;
        XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), False);
    }

    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormPrev"), True);
    XtasNoiseFillGraphicalList (0, (Widget) client_data, NULL);
    XtasNoiseFillNbVisualizeResults (0, NULL);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseTreatNextCallback                                     */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseTreatNextCallback( parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtasNoiseCurrentTopIndex += XTAS_NOISE_MAX_NBSIG_WINDOW;
    if(XtasNoiseCurrentTopIndex > XtasNoiseTab->NBDISPLAY - XTAS_NOISE_MAX_NBSIG_WINDOW) {
        XtasNoiseCurrentTopIndex = XtasNoiseTab->NBDISPLAY - XTAS_NOISE_MAX_NBSIG_WINDOW;
        XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), False);
    }
    
    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormPrev"), True);
    XtasNoiseFillGraphicalList (0, (Widget) client_data, NULL);
    XtasNoiseFillNbVisualizeResults (0, NULL);

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseTreatPrevCallback                                     */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseTreatPrevCallback( parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtasNoiseCurrentTopIndex -= XTAS_NOISE_MAX_NBSIG_WINDOW;
    if(XtasNoiseCurrentTopIndex < 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormPrev"), False);
    }
    
    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), True);
    XtasNoiseFillGraphicalList (0, (Widget) client_data, NULL);
    XtasNoiseFillNbVisualizeResults (0, NULL);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseTreatFastRewCallback                                  */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseTreatFastRewCallback( parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtasNoiseCurrentTopIndex -=  3 * XTAS_NOISE_MAX_NBSIG_WINDOW;
    if(XtasNoiseCurrentTopIndex < 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormPrev"), False);
    }
    
    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), True);
    XtasNoiseFillGraphicalList (0, (Widget) client_data, NULL);
    XtasNoiseFillNbVisualizeResults (0, NULL);

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseTreatTopCallback                                      */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseTreatTopCallback( parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtasNoiseCurrentTopIndex = 0;
    XtasNoiseFillGraphicalList (0, (Widget) client_data, NULL);
    XtasNoiseFillNbVisualizeResults (0, NULL);
    XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormPrev"), False);
    if(XtasNoiseTab->NBDISPLAY >= XTAS_NOISE_MAX_NBSIG_WINDOW)
        XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), True);
    else
        XtSetSensitive (XtNameToWidget (XtasNoiseResultsWidget, "*XtasButtonsFormNext"), False);

}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateNoiseList                                            */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseSortInsideAlimMaxCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_INSIDE_ALIM_MAX);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}
      
void XtasNoiseSortInsideAlimRealCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_INSIDE_ALIM_REAL);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}

void XtasNoiseSortRiseMaxCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_RISE_MAX);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}

void XtasNoiseSortRiseRealCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_RISE_REAL);                                      
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}

void XtasNoiseSortFallMaxCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_FALL_MAX);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}

void XtasNoiseSortFallRealCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_FALL_REAL);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}

void XtasNoiseSortScoreGlobalCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_SCORE_TOTAL);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}

void XtasNoiseSortScoreCrosstalkCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_SCORE_CTK);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}
void XtasNoiseSortScoreNoiseCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_SCORE_NOISE);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}
void XtasNoiseSortScoreIntervalCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_SCORE_INTERVAL);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}
void XtasNoiseSortScoreActivityCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    stb_ctk_sort_stat(XtasNoiseTab, STB_CTK_SORT_SCORE_ACTIVITY);
    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    if (XTAS_DISPLAY_NEW == 0) {
        XtasNoiseCurrentTopIndex = 0;
        XtasNoiseTreatTopCallback (parent, (Widget)client_data, NULL);
    }
    else {
        XtasUpdateNoiseTextualScrollList ((Widget)client_data);
    }
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateNoiseList                                            */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateNoiseList( parent, tas_winfos)
    Widget              parent     ;
    XtasWindowStruct   *tas_winfos ;
{
    Widget             form_widget, form, dbase_form, pan_widget ;
    Widget             label_widget, button  ;
    Widget             frame  ;
    Widget             buttons_form  ;
    Widget             b_rise_max, b_rise_real, b_fall_max, b_fall_real;
    Widget             b_ins_max, b_ins_real;
    Widget             b_score_global, b_score_noise, b_score_int, b_score_ctk, b_score_act;
    Widget             separator1, separator2 ;
    XmString           text ;
    Arg                args[20] ;
    int                n;

    form_widget = XtVaCreateWidget( "XtasMainPan",
            xmFormWidgetClass,
            parent,
            XmNtopAttachment,    XmATTACH_FORM,
            XmNbottomAttachment, XmATTACH_POSITION,
            XmNbottomPosition,   99,
            XmNleftAttachment,   XmATTACH_POSITION,
            XmNleftPosition,     1,
            XmNrightAttachment,  XmATTACH_POSITION,
            XmNrightPosition,    99,
            NULL) ;
    HelpFather = form_widget ;

    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ) ; n++ ;
    buttons_form = XtCreateManagedWidget( "XtasNoiseButtons", xmFormWidgetClass, form_widget, args, n ) ;
    for ( n=0 ; XtasNoiseButtonMenus[n].pix_file != -1  ; n++ )
        XtasNoiseButtonMenus[n].callback_data = (XtPointer) tas_winfos ;
    n = 0 ;
    XtSetArg( args[n], XmNnumColumns,      1             ) ; n++ ;
    XtSetArg( args[n], XmNorientation,     XmHORIZONTAL  ) ; n++ ;
    tas_winfos->butwidget = XalButtonMenus( buttons_form, XtasNoiseButtonMenus, args,
            n, 40, 40 ) ;
    XtasAddDummyButton( tas_winfos->butwidget ) ;
    XtManageChild( tas_winfos->butwidget ) ;

    /*---------------------------------------------------------------------------*/
    /* creation d'un separateur                                                  */
    /*---------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM             ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM             ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET           ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,       buttons_form              ) ; n++ ;
    separator1 = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, form_widget, args, n ) ;

    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM             ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET           ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,       separator1                ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM             ) ; n++ ;
    form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form_widget, args, n ) ;

    /* Max Noise label */
    //sprintf(buffer,"Max noise :     %.0f mV (rise peak)", XtasNoiseTab[0].OVR_MAX*1000.0) ;
    //text = XmStringCreateSimple (buffer);
    //n = 0 ;
    //XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM            ) ; n++ ;
    //XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM            ) ; n++ ;
    //XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
    //frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
    //XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
    //XtSetArg( args[n], XmNlabelString,      text                     ) ; n++ ;
    //XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING    ) ; n++ ;
    //label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
    //XmStringFree (text);

    ///* On signal label */
    //sprintf(buffer,"On signal :   (%s)  %s ",((XtasNoiseTab[0].NODE->TYPE & TTV_NODE_UP) == TTV_NODE_UP) ? "Rise":"Fall", 
    //                                        XtasNoiseTab[0].NODE->ROOT->NAME) ;
    //text = XmStringCreateSimple (buffer);
    //n = 0 ;
    //XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET          ) ; n++ ;
    //XtSetArg( args[n], XmNtopWidget,        frame                    ) ; n++ ;
    //XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM            ) ; n++ ;
    //XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
    //frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
    //XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
    //XtSetArg( args[n], XmNlabelString,      text                     ) ; n++ ;
    //XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING    ) ; n++ ;
    //label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
    //XmStringFree (text);



    /**/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0               ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
//    XtSetArg( args[n], XmNtopWidget,        frame           ) ; n++ ;
//    XtSetArg( args[n], XmNtopOffset,        10              ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      2               ) ; n++ ;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n ) ;
    


    text = XmStringCreateSimple( "Inside Alim Max" ) ;
    b_ins_max = XtVaCreateManagedWidget( "XtasTitlesInsideAlimMax",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_FORM,
            XmNwidth,          XTAS_DISPLAY_NEW ? (XTAS_NOISE_SORTBUTTON_WIDTH_T+(((XtasNoiseLabelSigNameLength + 1) * 7)/2)):XTAS_NOISE_SORTBUTTON_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_LEFT_OFFSET_T:XTAS_NOISE_LEFT_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    
    text = XmStringCreateSimple( "Inside Alim Real" ) ;
    b_ins_real = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_ins_max,
            XmNwidth,          XTAS_DISPLAY_NEW ? (XTAS_NOISE_SORTBUTTON_WIDTH_T+(((XtasNoiseLabelSigNameLength + 1) * 7)/2)):XTAS_NOISE_SORTBUTTON_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T2:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    
    text = XmStringCreateSimple( "Rank" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      b_ins_max,
            XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
            XmNleftWidget,     b_ins_max,
            XmNalignment,      XmALIGNMENT_END,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_RANK_WIDTH_T:XTAS_NOISE_LABEL_RANK_WIDTH_G,
            XmNtopOffset,      2,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

    text = XmStringCreateSimple( "Level" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      b_ins_max,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_TRANS_WIDTH_T : XTAS_NOISE_LABEL_TRANS_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_GROUP_T:XTAS_NOISE_OFFSET_GROUP_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

    text = XmStringCreateSimple( "Signal Name" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasNoiseSigName",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      b_ins_real,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? ((XtasNoiseLabelSigNameLength + 1) * 7) :XTAS_NOISE_LABEL_NAMES_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

/*    text = XmStringCreateSimple( "Net Name" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      b_ins_real,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? ((XtasNoiseLabelNetNameLength + 1) * 7) :XTAS_NOISE_LABEL_NAMES_WIDTH_G ,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
*/
    text = XmStringCreateSimple( "RISE PEAK" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_OVR_UND_WIDTH_T:XTAS_NOISE_LABEL_OVR_UND_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_GROUP_T+5:XTAS_NOISE_OFFSET_GROUP_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Model" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      label_widget,
            XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_MODEL_WIDTH_T:XTAS_NOISE_LABEL_MODEL_WIDTH_G,
            XmNtopOffset,      2,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

    text = XmStringCreateSimple( "Max" ) ;
    b_rise_max = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      label_widget,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_NOISE_WIDTH_T:XTAS_NOISE_LABEL_NOISE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

    text = XmStringCreateSimple( "Real" ) ;
    b_rise_real = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      b_rise_max,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_rise_max,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_NOISE_WIDTH_T:XTAS_NOISE_LABEL_NOISE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G, 
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;


    text = XmStringCreateSimple( "FALL PEAK" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_rise_real,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_OVR_UND_WIDTH_T:XTAS_NOISE_LABEL_OVR_UND_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_GROUP_T:XTAS_NOISE_OFFSET_GROUP_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Model" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      label_widget,
            XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_MODEL_WIDTH_T:XTAS_NOISE_LABEL_MODEL_WIDTH_G,
            XmNtopOffset,      2,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
 
    text = XmStringCreateSimple( "Max" ) ;
    b_fall_max = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      label_widget,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_NOISE_WIDTH_T:XTAS_NOISE_LABEL_NOISE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

    text = XmStringCreateSimple( "Real" ) ;
    b_fall_real = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      b_fall_max,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_fall_max,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_NOISE_WIDTH_T:XTAS_NOISE_LABEL_NOISE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    
    text = XmStringCreateSimple( "SCORES" ) ;
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
            xmLabelWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_FORM,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_fall_real,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_SCORES_WIDTH_T:XTAS_NOISE_LABEL_SCORES_WIDTH_G,
            XmNtopOffset,      2,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_GROUP_T:XTAS_NOISE_OFFSET_GROUP_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Global" ) ;
    b_score_global = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_WIDGET,
            XmNtopWidget,      label_widget,
            XmNleftAttachment, XmATTACH_OPPOSITE_WIDGET,
            XmNleftWidget,     label_widget,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_SCORE_WIDTH_T:XTAS_NOISE_LABEL_SCORE_WIDTH_G,
            XmNtopOffset,      2,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Noise" ) ;
    b_score_noise = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      b_score_global,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_score_global,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_SCORE_WIDTH_T:XTAS_NOISE_LABEL_SCORE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Interval" ) ;
    b_score_int = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      b_score_noise,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_score_noise,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_SCORE_WIDTH_T:XTAS_NOISE_LABEL_SCORE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Crosstalk" ) ;
    b_score_ctk = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      b_score_int,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_score_int,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_SCORE_WIDTH_T:XTAS_NOISE_LABEL_SCORE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Activity" ) ;
    b_score_act = XtVaCreateManagedWidget( "XtasMainTitles",
            xmPushButtonWidgetClass,
            dbase_form,
            XmNlabelString,    text,
            XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET,
            XmNtopWidget,      b_score_ctk,
            XmNleftAttachment, XmATTACH_WIDGET,
            XmNleftWidget,     b_score_ctk,
            XmNwidth,          XTAS_DISPLAY_NEW ? XTAS_NOISE_LABEL_SCORE_WIDTH_T:XTAS_NOISE_LABEL_SCORE_WIDTH_G,
            XmNleftOffset,     XTAS_DISPLAY_NEW ? XTAS_NOISE_OFFSET_T:XTAS_NOISE_OFFSET_G,
            XmNheight,         25,
            NULL) ;
    XmStringFree( text ) ;

    n = 0 ;
    XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,              2                 ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,             2                 ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,            2                 ) ; n++ ;
    form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form_widget, args, n ) ;

    /* begin of old display */
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNleftOffset,      2,
            NULL) ;
    XtasSetLabelString( label_widget, "Informations Area" ) ;

    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,             label_widget      ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ) ; n++ ;
    XtSetArg( args[n], XmNrightPosition,         48                ) ; n++ ;
    frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNheight,                 20                       ) ; n++ ;
    tas_winfos->meswidget = XtCreateManagedWidget( "XtasUserInfo", xmLabelWidgetClass, frame, args, n) ;
    XtasSetLabelString( tas_winfos->meswidget, XTAS_NULINFO ) ;


    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        frame                    ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       frame           ) ; n++ ; 
    frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNheight,           20              ) ; n++ ;
    tas_winfos->hlpwidget = XtCreateManagedWidget( "XtasHelpInfo", xmLabelWidgetClass, frame, args, n) ;
    XtasSetLabelString( tas_winfos->hlpwidget, XTAS_NULINFO ) ;

    n = 0 ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET            ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,     form    ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM              ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       3                          ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      3                          ) ; n++ ;
    separator2 = XtCreateManagedWidget( "separator2", xmSeparatorWidgetClass, form_widget, args, n ) ;

    n = 0 ;
//    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET            ) ; n++ ;
//    XtSetArg( args[n], XmNbottomWidget,     separator2    ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET            ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        dbase_form    ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM              ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
    pan_widget = XtCreateManagedWidget( "XtasPanWindow", xmPanedWindowWidgetClass, form_widget, args, n ) ;

    if (XTAS_DISPLAY_NEW == 0)
    {
        if((form = XtasCreateNoiseGraphicalScrollList(pan_widget, tas_winfos, b_ins_max))) {
            XtAddCallback( b_ins_max, XmNactivateCallback, XtasNoiseSortInsideAlimMaxCallback, (XtPointer) form);
            XtAddCallback( b_ins_real, XmNactivateCallback, XtasNoiseSortInsideAlimRealCallback, (XtPointer) form);
            XtAddCallback( b_rise_max, XmNactivateCallback, XtasNoiseSortRiseMaxCallback, (XtPointer) form);
            XtAddCallback( b_rise_real, XmNactivateCallback, XtasNoiseSortRiseRealCallback, (XtPointer) form);
            XtAddCallback( b_fall_max, XmNactivateCallback, XtasNoiseSortFallMaxCallback, (XtPointer) form);
            XtAddCallback( b_fall_real, XmNactivateCallback, XtasNoiseSortFallRealCallback, (XtPointer) form);
            XtAddCallback( b_score_global, XmNactivateCallback, XtasNoiseSortScoreGlobalCallback, (XtPointer) form);
            XtAddCallback( b_score_noise, XmNactivateCallback, XtasNoiseSortScoreNoiseCallback, (XtPointer) form);
            XtAddCallback( b_score_ctk, XmNactivateCallback, XtasNoiseSortScoreCrosstalkCallback, (XtPointer) form);
            XtAddCallback( b_score_int, XmNactivateCallback, XtasNoiseSortScoreIntervalCallback, (XtPointer) form);
            XtAddCallback( b_score_act, XmNactivateCallback, XtasNoiseSortScoreActivityCallback, (XtPointer) form);

            n = 0;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET          ) ; n++ ;
            XtSetArg( args[n], XmNbottomWidget,     separator2               ) ; n++ ;
            XtSetArg( args[n], XmNbottomOffset,     5                        ) ; n++ ;
            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM            ) ; n++ ;
            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
            XtSetArg( args[n], XmNrightOffset,      5                        ) ; n++ ;
            dbase_form = XtCreateManagedWidget( "XtasFrame", xmFormWidgetClass, form_widget, args, n ) ;

            XtVaSetValues (pan_widget, XmNbottomAttachment, XmATTACH_WIDGET, XmNbottomWidget, dbase_form,  NULL);

            n = 0;
            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNleftOffset,       5               ); n++;
            label_widget = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, dbase_form, args, n);
            XtasNoiseFillNbVisualizeResults (1, label_widget);

            n = 0;
            XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_FORM ); n++;
            XtSetArg ( args[n], XmNsensitive,           XtasNoiseTab->NBDISPLAY <= XTAS_NOISE_MAX_NBSIG_WINDOW ? False:True ); n++;
            buttons_form = XtCreateManagedWidget ("XtasButtonsFormNext", xmFormWidgetClass, dbase_form, args, n);

            n = 0;
            XtSetArg ( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
            XtSetArg ( args[n], XmNwidth,            60            ); n++;
            button = XtCreateManagedWidget( "XtasShellSubForm",
                    xmPushButtonWidgetClass,
                    buttons_form,
                    args, n );
            XtasSetLabelString( button, " ->| " );

            XtAddCallback( button,
                    XmNactivateCallback,
                    XtasNoiseTreatEndCallback,
                    (XtPointer) form);

            n = 0;
            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNrightWidget,      button          ); n++;
            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNwidth,            60              ); n++;
            button = XtCreateManagedWidget( "XtasShellSubForm",
                    xmPushButtonWidgetClass,
                    buttons_form,
                    args, n );
            XtasSetLabelString( button, " ->-> " );
            XtAddCallback( button,
                    XmNactivateCallback,
                    XtasNoiseTreatFastFwdCallback,
                    (XtPointer) form);

            n = 0;
            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNrightWidget,      button          ); n++;
            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNwidth,            60              ); n++;
            button = XtCreateManagedWidget( "XtasShellSubForm",
                    xmPushButtonWidgetClass,
                    buttons_form,
                    args, n );
            XtasSetLabelString( button, " -> " );
            XtAddCallback( button,
                    XmNactivateCallback,
                    XtasNoiseTreatNextCallback,
                    (XtPointer)form);

            n = 0;
            XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET  ); n++;
            XtSetArg ( args[n], XmNrightWidget,         buttons_form     ); n++;
            XtSetArg ( args[n], XmNsensitive,           False            ); n++;
            buttons_form = XtCreateManagedWidget ("XtasButtonsFormPrev", xmFormWidgetClass, dbase_form, args, n);

            n = 0;
            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNwidth,            60              ); n++;
            button = XtCreateManagedWidget( "XtasShellSubForm",
                    xmPushButtonWidgetClass,
                    buttons_form,
                    args, n );
            XtasSetLabelString( button, " <- " );
            XtAddCallback( button,
                    XmNactivateCallback,
                    XtasNoiseTreatPrevCallback,
                    (XtPointer)form);

            n = 0;
            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNrightWidget,      button          ); n++;
            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNwidth,            60              ); n++;
            button = XtCreateManagedWidget( "XtasShellSubForm",
                    xmPushButtonWidgetClass,
                    buttons_form,
                    args, n );
            XtasSetLabelString( button, " <-<- " );
            XtAddCallback( button,
                    XmNactivateCallback,
                    XtasNoiseTreatFastRewCallback,
                    (XtPointer)form);

            n = 0;
            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
            XtSetArg( args[n], XmNrightWidget,      button          ); n++;
            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
            XtSetArg( args[n], XmNwidth,            60              ); n++;
            button = XtCreateManagedWidget( "XtasShellSubForm",
                    xmPushButtonWidgetClass,
                    buttons_form,
                    args, n );
            XtasSetLabelString( button, " |<- " );
            XtAddCallback( button,
                    XmNactivateCallback,
                    XtasNoiseTreatTopCallback,
                    (XtPointer)form);
        }
    }
    else {
        form = XtasCreateNoiseTextualScrollList(pan_widget, tas_winfos, b_ins_max) ;
        XtAddCallback( b_ins_max, XmNactivateCallback, XtasNoiseSortInsideAlimMaxCallback, (XtPointer) form);
        XtAddCallback( b_ins_real, XmNactivateCallback, XtasNoiseSortInsideAlimRealCallback, (XtPointer) form);
        XtAddCallback( b_rise_max, XmNactivateCallback, XtasNoiseSortRiseMaxCallback, (XtPointer) form);
        XtAddCallback( b_rise_real, XmNactivateCallback, XtasNoiseSortRiseRealCallback, (XtPointer) form);
        XtAddCallback( b_fall_max, XmNactivateCallback, XtasNoiseSortFallMaxCallback, (XtPointer) form);
        XtAddCallback( b_fall_real, XmNactivateCallback, XtasNoiseSortFallRealCallback, (XtPointer) form);
        XtAddCallback( b_score_global, XmNactivateCallback, XtasNoiseSortScoreGlobalCallback, (XtPointer) form);
        XtAddCallback( b_score_noise, XmNactivateCallback, XtasNoiseSortScoreNoiseCallback, (XtPointer) form);
        XtAddCallback( b_score_ctk, XmNactivateCallback, XtasNoiseSortScoreCrosstalkCallback, (XtPointer) form);
        XtAddCallback( b_score_int, XmNactivateCallback, XtasNoiseSortScoreIntervalCallback, (XtPointer) form);
        XtAddCallback( b_score_act, XmNactivateCallback, XtasNoiseSortScoreActivityCallback, (XtPointer) form);
    }



    XtManageChild( form_widget ) ;
    return(form_widget) ;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateNoiseMenus                                           */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the detail  window.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateNoiseMenus( parent , tas_winfos )
    Widget parent ;
    XtasWindowStruct *tas_winfos ;
{
    Widget menu_bar ;
    Arg    args[20] ;
    int    n ;

    for(n = 0  ; XtasNoiseMenuFile[n].label != NULL  ; n++)
        XtasNoiseMenuFile[n].callback_data    = (XtPointer) tas_winfos ;
    for(n = 0  ; XtasNoiseMenuView[n].label != NULL  ; n++)
        XtasNoiseMenuView[n].callback_data    = (XtPointer) tas_winfos ;
    for(n = 0  ; XtasNoiseMenuOptions[n].label != NULL  ; n++)
        XtasNoiseMenuOptions[n].callback_data = (XtPointer) tas_winfos ;

    n = 0  ;
    menu_bar = XmCreateMenuBar( parent, "XtasMenuBar", args, n ) ;
    XalBuildMenus( menu_bar, XtasNoiseMenu ) ;
    return( menu_bar ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseAnalysis                                              */
/*                                                                           */
/* IN  ARGS : number of item per page                                        */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : the noise analysis function.                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int    XtasNoiseAnalysis (void)
{
    int                  n                  ;
    XtasWindowStruct    *tas_winfos         ;
    Arg                  args[10]           ;
    Widget               main_window        ;
    int                  width              ;
    ttvsig_list         *ptsig              ;
    char                 buf[1024]          ;

    XtasPasqua();
    if(sigsetjmp( XtasMyEnv , 1 ) == 0) {
        ctk_setprogressbar(XtasStbInProgress);
        XtasNoiseTab = stb_ctk_get_stat(XtasMainParam->stbfig);
        XtasDestroyStbInProgress();
    }
    else {
        XtasFirePasqua();
        return 1;
    }

        
    XtasFirePasqua();
   
    if (!XtasNoiseTab) 
        return 1;

    if (XTAS_DISPLAY_NEW) { 
        for ( n = 0 ; n < XtasNoiseTab->NBDISPLAY ; n++ ) {
            ptsig = XtasNoiseTab->TAB[n].NODE->ROOT;

            ttv_getsigname (XtasMainParam->stbfig->FIG, buf, ptsig);

            if ((width = strlen(buf)) > XtasNoiseLabelSigNameLength ) {
///                if (width < XTAS_NOISE_NAME_MAX_LENGTH)
                    XtasNoiseLabelSigNameLength = width;
///                else
///                    XtasNoiseLabelSigNameLength = XTAS_NOISE_NAME_MAX_LENGTH;
            }

/*            if ((width = strlen(ptsig->NETNAME)) > XtasNoiseLabelNetNameLength ) {
                if (width < XTAS_NOISE_NAME_MAX_LENGTH)
                    XtasNoiseLabelNetNameLength = width;
                else
                    XtasNoiseLabelNetNameLength = XTAS_NOISE_NAME_MAX_LENGTH;
            }
*/
        }
//        width = XTAS_NOISE_WINDOW_WIDTH_T + (XtasNoiseLabelSigNameLength*7) + (XtasNoiseLabelNetNameLength*7);

        width = XTAS_NOISE_WINDOW_WIDTH_T + (XtasNoiseLabelSigNameLength*7);
    }
    
    /* Creation de la structure pour la fenetre de resultats */
    tas_winfos = (XtasWindowStruct *) mbkalloc (sizeof(XtasWindowStruct));
    tas_winfos->topwidget = XtVaCreatePopupShell (NULL, topLevelShellWidgetClass, 
            XtasTopLevel,
            XmNallowShellResize, False,
            XmNdeleteResponse,   XmDESTROY,
            NULL) ;
    XtasNoiseResultsWidget = tas_winfos->topwidget;

    n = 0 ;
    sprintf (buf, XTAS_NAME": Noise Analysis Results - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
//    XtSetArg (args[n], XmNtitle , XTAS_NAME": Noise Analysis Results");n++ ;
    XtSetArg (args[n], XmNtitle , buf);n++ ;
    XtSetArg (args[n], XmNwidth , XTAS_DISPLAY_NEW ? width:XTAS_NOISE_WINDOW_WIDTH_G); n++ ;
    XtSetValues (tas_winfos->topwidget, args, n);

    XalCustomIcon (tas_winfos->topwidget, XTAS_NOISE_MAP);
    main_window = XmCreateMainWindow (tas_winfos->topwidget, "XtasMain", args, n);
    XtManageChild (main_window);
    tas_winfos->menwidget = XtasCreateNoiseMenus (main_window, tas_winfos);
    XtManageChild (tas_winfos->menwidget);
    
    tas_winfos->wrkwidget = XtasCreateNoiseList (main_window, tas_winfos);
    
    tas_winfos->errwidget = XalCreateErrorBox (tas_winfos->wrkwidget);
    tas_winfos->warwidget = XalCreateWarningBox (tas_winfos->wrkwidget);

    XmMainWindowSetAreas (main_window,
            tas_winfos->menwidget,
            (Widget) NULL,
            (Widget) NULL,
            (Widget) NULL,
            tas_winfos->wrkwidget);

    XtPopup (tas_winfos->topwidget, XtGrabNone);
    return 0;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseOkCallback                                            */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .client_data :                                                 */
/*            .call_data   : Toolkit informations.                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The drawing function for crosstalk informations                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasNoiseOkCallback ( parent, client_data, call_data)
    Widget      parent      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    char    *text;
    char    *ptend;
    long     nb_item;
    
    XtasWindowStruct *stb_result = (XtasWindowStruct*) client_data;

    if (XtasNoiseResultsWidget) {
        XtDestroyWidget (XtasNoiseResultsWidget) ; 
		XtasNoiseResultsWidget = NULL;
    }

    if (XtasNoiseTab) {
//        mbkfree (XtasNoiseTab);
        XtasNoiseTab = NULL;
    }

    XtasNoiseSelectedIndex = XTAS_NOISE_NO_SELECT;
    XtasNoiseCurrentTopIndex = 0;
  
    if (XTAS_DISPLAY_NEW == 0) {
        text = XmTextGetString(XtNameToWidget(parent, "*XtasNoiseNbItemText"));
        nb_item = strtol (text, &ptend, 10);
        if((*ptend != '\0') || (nb_item < 0)) {
            XtManageChild (parent);
            XalDrawMessage ( stb_result->errwidget,
                "Bad value for Number of items per page:\nMust be a positive integer.");
            XtFree (text);
            return;
        }
        XtFree (text);
    
        XTAS_NOISE_MAX_NBSIG_WINDOW = nb_item;
    
        XtUnmanageChild (parent);
    }
    XalSetCursor( stb_result->topwidget, WAIT );
    XtasSetLabelString( stb_result->meswidget, "Noise Analysis is running...");
    XalForceUpdate( stb_result->topwidget );

    if (XtasNoiseAnalysis () != 0) {
        XalSetCursor( stb_result->topwidget, NORMAL );
        XtasSetLabelString( stb_result->meswidget, "No Noise");
        XalForceUpdate( stb_result->topwidget );
    }
    else {
        XalSetCursor( stb_result->topwidget, NORMAL );
        XtasSetLabelString( stb_result->meswidget, "Noise Analysis Completed");
        XalForceUpdate( stb_result->topwidget );
    }
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCancelNoiseCallback                                        */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .client_data :                                                 */
/*            .call_data   : Toolkit informations.                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The drawing function for crosstalk informations                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasCancelNoiseCallback ( parent, client_data, call_data)
    Widget      parent      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    XtUnmanageChild ((Widget)client_data);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNoiseCallback                                              */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .client_data :                                                 */
/*            .call_data   : Toolkit informations.                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The drawing function for crosstalk informations                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNoiseCallback (parent, client_data, call_data)
    Widget      parent;
    XtPointer   client_data;
    XtPointer   call_data;
{
    Arg         args[20];
    int         n       ;
    Atom        WM_DELETE_WINDOW;
    Widget      form, frame, sub_form, label_widget, text_widget;
    XmString    text    ;
    XtasWindowStruct *tas_winfos = (XtasWindowStruct *)client_data;

    if (XtasAppNotReady ()) 
		return;

    if ((XTAS_CTX_LOADED & XTAS_FROM_ANALYSIS) != XTAS_FROM_ANALYSIS) {
        XalDrawMessage(tas_winfos->errwidget,
                        "Crosstalk Analysis must have been run before");
        return;
    }
    
    if (XTAS_DISPLAY_NEW == 1) {
        XtasNoiseOkCallback (XtasMainWindow, (XtPointer)client_data, NULL);
    }
    else {
        if(!XtasNoiseParamWidget) {
            n = 0;
            XtSetArg ( args[n], XmNtitle,       XTAS_NAME": Noise Parameterization"   ); n++;
            XtSetArg ( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL             ); n++;
//            XtSetArg ( args[n], XmNminWidth,    200                                    ); n++;
//            XtSetArg ( args[n], XmNmaxWidth,    200                                    ); n++;
//            XtSetArg ( args[n], XmNminHeight,   200                                    ); n++;
//            XtSetArg ( args[n], XmNmaxHeight,   200                                    ); n++;
            XtasNoiseParamWidget = XmCreatePromptDialog (tas_winfos->topwidget, "XtasParamBox", args, n);
    	    HelpFather = XtasNoiseParamWidget;
            XtUnmanageChild(XmSelectionBoxGetChild(XtasNoiseParamWidget,XmDIALOG_TEXT));
            XtUnmanageChild(XmSelectionBoxGetChild(XtasNoiseParamWidget,XmDIALOG_PROMPT_LABEL));
            XtUnmanageChild(XmSelectionBoxGetChild(XtasNoiseParamWidget,XmDIALOG_HELP_BUTTON));
            XtAddCallback( XtasNoiseParamWidget, XmNokCallback, XtasNoiseOkCallback, (XtPointer)client_data );
            XtAddCallback( XtasNoiseParamWidget, XmNcancelCallback, XtasCancelNoiseCallback, (XtPointer)XtasNoiseParamWidget );
            WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
            XmAddWMProtocolCallback(XtParent(XtasNoiseParamWidget), WM_DELETE_WINDOW, XtasCancelNoiseCallback, (XtPointer )XtasNoiseParamWidget);
            
            n = 0;
            form = XtCreateManagedWidget( "XtasForm", xmFormWidgetClass, XtasNoiseParamWidget, args, n );
    
    
            /*--------- frame <Number of signals to display> ---------*/
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM   ); n++;
            frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
    
            text = XmStringCreateSimple ("Results Display"); 
            n = 0;
#if XTAS_MOTIF_VERSION >= 20000
            XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else        
            XtSetArg ( args[n], XmNchildType,  XmFRAME_TITLE_CHILD ); n++;
#endif
            XtSetArg ( args[n], XmNlabelString,     text                ); n++;
            label_widget = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, frame, args, n );
            XmStringFree (text);
    
            n = 0;
#if XTAS_MOTIF_VERSION >= 20000
            XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else        
            XtSetArg ( args[n], XmNchildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#endif
           sub_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, frame, args, n);

            n = 0;
            sub_form = XtCreateManagedWidget( "XtasNoiseMaxSigForm", xmFormWidgetClass, sub_form, args, n );

            text = XmStringCreateSimple ("Number of items per page");
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNleftOffset,      5               ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNlabelString,     text            ); n++;
            XtSetArg ( args[n], XmNheight,          30              ); n++;
            label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n);
            XmStringFree (text);
    
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNbottomOffset,    5               ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET ); n++;
            XtSetArg ( args[n], XmNleftWidget,      label_widget    ); n++;
            XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
            XtSetArg ( args[n], XmNrightOffset,     5               ); n++;
            XtSetArg ( args[n], XmNheight,          30              ); n++;
            XtSetArg ( args[n], XmNwidth,           50              ); n++;
            text_widget = XtCreateManagedWidget( "XtasNoiseNbItemText", xmTextWidgetClass, sub_form, args, n);
            XmTextSetString (text_widget, "15");

        }
        
        XtManageChild (XtasNoiseParamWidget);
    }
        
}

