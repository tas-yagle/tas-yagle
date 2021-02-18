/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_visualise_path.c                                       */
/*                                                                          */
/*    Author(s) : Caroline BLED                       Date : 24/12/2002     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#include "xtas.h"


Widget  XtasVisualPathWidget = NULL;
char    TYPE = XTAS_HILIGHT;

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasCancelVisualPathCallback                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasCancelVisualPathCallback( widget, client_data , call_data )
    Widget      widget;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtUnmanageChild (XyagTopLevel);
    XtUnmanageChild ((Widget) client_data);
//    XalLeaveLimitedLoop ();
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasVisualPathCallback                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasVisualPathCallback( widget, client_data , call_data )
Widget widget ;
XtPointer client_data  ;
XtPointer call_data ;
{
    Arg          args[10];
    int          n;
    Widget       row_widget, toggle;
    XmString     text;
    Atom         WM_DELETE_WINDOW;
    static XtasWindowStruct *tas_winfos = NULL;


    if ( XtasAppNotReady() ) return;

    
    if (!tas_winfos)
        tas_winfos = (XtasWindowStruct *) mbkalloc (sizeof(XtasWindowStruct));
   
    *tas_winfos = *((XtasWindowStruct*)client_data);

    if ( !XtasVisualPathWidget )
    {
        n = 0;
        XtSetArg( args[n], XmNtitle, XTAS_NAME": Visualize Path" ); n++;
        XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
        XtasVisualPathWidget = XmCreatePromptDialog( XtParent(XtParent(widget)), "XtasInfosBox", args, n);
        HelpFather = XtasVisualPathWidget ;
        XtUnmanageChild(XmSelectionBoxGetChild(XtasVisualPathWidget, XmDIALOG_TEXT));
        XtUnmanageChild(XmSelectionBoxGetChild(XtasVisualPathWidget, XmDIALOG_PROMPT_LABEL));
        XtAddCallback( XtasVisualPathWidget, XmNokCallback, XtasVisualPathOkCallback, (XtPointer)tas_winfos);
        XtAddCallback( XtasVisualPathWidget, XmNcancelCallback, XtasCancelVisualPathCallback, (XtPointer)XtasVisualPathWidget );
        XtAddCallback( XtasVisualPathWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_SRC_VISUPATH|XTAS_HELP_MAIN) );
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasVisualPathWidget), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )XtasVisualPathWidget);
        XmAddTabGroup( XtasVisualPathWidget ); 

        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, XtasVisualPathWidget,
                                              XmNpacking,         XmPACK_COLUMN,
                                              XmNnumColumns,      2,
                                              XmNorientation,     XmHORIZONTAL,
                                              XmNspacing,         15,
                                              XmNmarginHeight,    10,
                                              XmNmarginWidth,     55,
                                              XmNisAligned,       True,
                                              XmNradioBehavior,   True,
                                              XmNradioAlwaysOne,  True,
                                              NULL);

        text = XmStringCreateSimple( "Highlight" );
        toggle = XtVaCreateManagedWidget( "XtasHighlight", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType, XmONE_OF_MANY,
                XmNset,           True,
                XmNlabelString,   text,
                NULL);
        XmStringFree( text );
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasHighlightCallback, (XtPointer)NULL );

        text = XmStringCreateSimple( "Extract" );
        toggle = XtVaCreateManagedWidget( "XtasExtract", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType, XmONE_OF_MANY,
                XmNset,           False,
                XmNlabelString,   text,
                NULL);
        XmStringFree( text );
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasExtractCallback, (XtPointer)NULL );

    }
    XtManageChild (XyagTopLevel);
    XtManageChild( XtasVisualPathWidget );
//    XalLimitedLoop(XtasVisualPathWidget);

}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasVisualPathOkCallback                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasVisualPathOkCallback( widget, tas_winfos , call_data )
Widget widget ;
XtPointer tas_winfos ;
XtPointer call_data ;
{
    XtasDetailPathSetStruct *detail_set = (XtasDetailPathSetStruct *)((XtasWindowStruct *)tas_winfos)->userdata;
    chain_list              *head;
    ttvcritic_list          *critic, *c;
    cgv_interaction         *cgvhead=NULL;
    char                    mess[100];
    char                    ext[4];

    XtUnmanageChild(widget);
   
    XtasPasqua();
    if (sigsetjmp( XtasMyEnv , 1 ) == 0) {
        if (XyagFileOpenByType(XtasMainParam->ttvfig->INFO->FIGNAME, CGV_FROM_CNS) == 1)
            if ((XyagFileOpenByType(XtasMainParam->ttvfig->INFO->FIGNAME, CGV_FROM_LOFIG)) == 1) {
                if (!strcmp(IN_LO, "vlg"))          sprintf (ext, vlg_sfx);
                else if (!strcmp(IN_LO, "vhd"))     sprintf (ext, vhd_sfx);
                else                                sprintf (ext, IN_LO);
                sprintf (mess, "Unable to open file %s.cns or %s.%s", XtasMainParam->ttvfig->INFO->FIGNAME, XtasMainParam->ttvfig->INFO->FIGNAME, ext);
                XtasFirePasqua ();
                XtUnmanageChild (XyagTopLevel);
                XalDrawMessage (((XtasWindowStruct *)tas_winfos)->errwidget, mess);
//                XalLeaveLimitedLoop ();
                return;
            }
        head = (chain_list *)detail_set->CUR_DETAIL->DATA;
        critic = (ttvcritic_list *)(head->DATA);


        for(c=critic ; c != NULL ; c = c->NEXT)
            cgvhead = cgv_NetObject(cgvhead, c->NETNAME);


//    cgvhead = cgv_NetObject(cgvhead, "a");
//    cgvhead = cgv_NetObject(cgvhead, "y");
        if (TYPE == XTAS_EXTRACT)
            XyagExtract(cgvhead, 1);
        else
            XyagHiLight(cgvhead, 1);
    }
    XtasGetWarningMess ();
    XtasFirePasqua ();

    
    XtPopup (XyagTopLevel, XtGrabNone);
//    XalLeaveLimitedLoop();

}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasHighlightCallback                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasHighlightCallback( widget, client_data, call_data )
Widget widget ;
XtPointer client_data ;
XtPointer call_data ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data  ;
 TYPE = ( state->set ) ? XTAS_HILIGHT : XTAS_EXTRACT  ;
    
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasExtractCallback                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasExtractCallback( widget, client_data, call_data )
Widget widget ;
XtPointer client_data ;
XtPointer call_data ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data  ;
 TYPE = ( state->set ) ? XTAS_EXTRACT : XTAS_HILIGHT  ;

}


