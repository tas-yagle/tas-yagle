/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_debug.c                                                */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/

#include "xtas.h"
#include "xtas_debug_menu.h"
#include "xtas_debug_button.h"

extern  stbdebug_list   *XtasDebug;


/*--------------------------------------------------------------------------*/
/*                             GLOBALS                                      */
/*--------------------------------------------------------------------------*/

Widget         XtasSelectDebugField  = NULL ;
Widget         XtasSelectMarginField = NULL ;
Widget         XtasDebugWidget       = NULL ;
Widget         XtasSelectDebugWidget = NULL ;
Widget         XtasGraphicWindow     = NULL ;

Widget         XtasDebugLabelPeriod  = NULL ;
Widget         XtasDebugLabelSetup   = NULL ;
Widget         XtasDebugLabelHold    = NULL ;
Widget         XtasDebugLabelMode    = NULL ;
Widget         XtasDebugLabelT1      = NULL ;
Widget         XtasDebugLabelT2      = NULL ;
Widget         XtasDebugLabelDelta      = NULL ;

              
int            XtasMargin            = 0 ;
char          *XtasDebugSigName      = NULL ;
char          *XtasDebugOtherSigName = NULL ;
              
int            XtasDebugWindowWidth  = 1033 ;
int            XtasDebugWindowHeight = 655 ;
int            XtasDrawingAreaWidth  = 868 ;
int            XtasDrawingAreaHeight = 1550 ;

stbdebug_list *XtasDebugList         = NULL ;
XtasWindowStruct XtasDebugWindow ;

long           XtasDebugChronoType   = XTAS_DEBUG_IUD | XTAS_DEBUG_OUD ;

String    XtasTranslations           = 
                             "<Btn1Down>: draw_cbk(down) ManagerGadgetArm()\n\
                              <Btn1Motion>: draw_cbk(motion) ManagerGadgetButtonMotion()\n\
							  <Btn1Up>: draw_cbk(up) ManagerGadgetActivate()\n\
                              <Btn2Down>: draw_cbk(down2) ManagerGadgetArm()\n\
                              <Btn2Motion>: draw_cbk(motion2) ManagerGadgetButtonMotion()\n\
							  <Btn2Up>: draw_cbk(up2) ManagerGadgetActivate()\n\
                              <Btn3Down>: draw_cbk(down3) ManagerGadgetArm()\n " ; 

XtTranslations translations ;
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugButtonsCallback                                       */
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
void XtasDebugButtonsCallback( widget, client_data, call_data )
Widget           widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasOptionsButtonsTreat( XtasDebugWindow.wrkwidget, &XtasDebugWindow.butwidget,
                         XtasDebugButtonMenus,  XtasDebugButtonOptions) ;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDestroyDebugWidget                                         */
/*                                                                           */
/* OBJECT   : Destroy the debug window                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasDestroyDebugWidget()
{ 
	if (XtasDebugWidget) { 
		XtDestroyWidget(XtasDebugWidget) ; 
		XtasDebugWidget = NULL ;
	}
	if (XtasDebugList) {
    	stb_freestbdebuglist(XtasDebugList) ; 
		XtasDebugList     = NULL ;
	}
        if(XtasDebugSigName != NULL)
          mbkfree(XtasDebugSigName) ;
	XtasDebugSigName = NULL ;
        if(XtasDebugOtherSigName != NULL)
          mbkfree(XtasDebugOtherSigName) ;
	XtasDebugOtherSigName = NULL ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugCloseCallback                                         */
/*                                                                           */
/* OBJECT   : Invoked on close                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
 
void XtasDebugCloseCallback(w, client_data, call_data)
	Widget     w;
	XtPointer  client_data;
	XtPointer  call_data;
{
	XtasDestroyDebugWidget() ;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugInfoCallback                                          */
/*                                                                           */
/* OBJECT   : Invoked Info                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
 
void XtasDebugInfo2Callback(w, client_data, call_data)
	Widget     w;
	XtPointer  client_data;
	XtPointer  call_data;
{
	if (XtasDebugSigName)
      {
       XalSetCursor( XtasDebugWindow.topwidget, WAIT ) ;
       XalForceUpdate(  XtasDebugWindow.topwidget ) ;
       XtasSignalsDetail( XtasDebugWindow.wrkwidget, XtasDebugSigName) ;
       XalSetCursor( XtasDebugWindow.topwidget, NORMAL ) ;
      }
    else
      {
       XalDrawMessage( XtasDebugWindow.errwidget, XTAS_NSIGERR ) ;
      }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugInfoCallback                                          */
/*                                                                           */
/* OBJECT   : Invoked Info                                                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
 
void XtasDebugInfoCallback(w, client_data, call_data)
	Widget     w;
	XtPointer  client_data;
	XtPointer  call_data;
{
	if (XtasDebugSigName && XtasDebugOtherSigName)
      {
       XalSetCursor( XtasDebugWindow.topwidget, WAIT ) ;
       XalForceUpdate(  XtasDebugWindow.topwidget ) ;
       XtasSignalsDetail( XtasDebugWindow.wrkwidget, XtasDebugOtherSigName) ;
       XalSetCursor( XtasDebugWindow.topwidget, NORMAL ) ;
      }
    else
      {
       XalDrawMessage( XtasDebugWindow.errwidget, XTAS_NSIGERR ) ;
      }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateDebugMenus                                           */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the detail  window.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/

Widget XtasCreateDebugMenus(parent)
	Widget parent;
{
	Widget menu_bar;
	Arg    args[20];
	int    n;

	for(n = 0 ; XtasDebugMenuFile[n].label ; n++)
  		XtasDebugMenuFile[n].callback_data = NULL ;
	for(n = 0 ; XtasDebugMenuTools[n].label ; n++)
  		XtasDebugMenuTools[n].callback_data = NULL ;
	for(n = 0 ; XtasDebugMenuOptions[n].label ; n++)
  		XtasDebugMenuOptions[n].callback_data = NULL ;
	for(n = 0 ; XtasDebugMenuView[n].label ; n++)
  		XtasDebugMenuView[n].callback_data = NULL ;

	n = 0 ;
	menu_bar = XmCreateMenuBar(parent, "XtasMenuBar", args, n);
	XalBuildMenus(menu_bar, XtasDebugMenu);
	return(menu_bar);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugGetPathCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDebugGetPathCallback (widget, client_data, call_data)
Widget    widget;
XtPointer client_data;
XtPointer call_data;
{
    short int mask = 0x0;
    stbdebug_list *ptdebug;
    int trouve = 0;
    char buf[512];
   
    if (!XtasDebugOtherSigName) {
        XalDrawMessage( XtasWarningWidget, XTAS_NSIGERR );
    }
    else {
	    if (XtasDebugSigName && XtasDebugOtherSigName) {
    	    XtasSetOrderByStartOrEnd (10, 1, XtasDebugOtherSigName) ;
    	    XtasSetOrderByStartOrEnd (20, 1, XtasDebugSigName) ;
 	    }

        ptdebug = XtasDebugList;
        while(ptdebug && !trouve) {
            ttv_getsigname(XtasMainParam->ttvfig, buf, ptdebug->SIG1);
            if(!strcmp(XtasDebugOtherSigName, buf))
                trouve = 1;
            else
                ptdebug = ptdebug->NEXT;
        }
                
        if(ptdebug)
            if(ptdebug->HOLD < XtasMargin)
                mask |= XTAS_HOLD;

        XtasMainToolsCallback (widget, (XtPointer)(&mask), call_data) ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugGetPathCmdCallback                                    */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDebugGetPathCmdCallback (widget, client_data, call_data)
Widget    widget;
XtPointer client_data;
XtPointer call_data;
{
    short int mask = XTAS_REG;
    ttvsig_list *ptSig, *ptOtherSig;
    chain_list  *ch, *chain;
    char buf[512], cmd_list[1024];
    int trouve = 0;
    stbdebug_list *ptdebug;

	if (XtasDebugSigName && XtasDebugOtherSigName) {
        ptSig      = ttv_getsig(XtasMainParam->ttvfig, XtasDebugSigName);
        ptOtherSig = ttv_getsig(XtasMainParam->ttvfig, XtasDebugOtherSigName);
        if(ptSig && ptOtherSig) {
            if(((ptSig->TYPE & TTV_SIG_L) != TTV_SIG_L) &&
               ((ptOtherSig->TYPE & TTV_SIG_L) != TTV_SIG_L))
                XalDrawMessage(XtasErrorWidget, "Selected signals are not register signals");
    
            else { 
                strcpy(cmd_list , "");
//                chain = ttv_getlrcmd(XtasMainParam->ttvfig, ptSig);
//                for(ch = chain; ch ; ch = ch->NEXT) {
//                    ttv_getsigname(XtasMainParam->ttvfig, buf, ((ttvevent_list*)ch->DATA)->ROOT);
//                    strcat(cmd_list, buf); 
//                    strcat(cmd_list, " ");
//                }
//                if(chain)
//                    freechain(chain);
                
                chain = ttv_getlrcmd(XtasMainParam->ttvfig, ptOtherSig);
                for(ch = chain; ch ; ch = ch->NEXT) {
                    ttv_getsigname(XtasMainParam->ttvfig, buf, ((ttvevent_list*)ch->DATA)->ROOT);
                    strcat(cmd_list, buf); 
                    strcat(cmd_list, " ");
                }
                if(chain)
                    freechain(chain);
                
                XtasSetOrderByStartOrEnd( 10, 1, "*" ) ;
                XtasSetOrderByStartOrEnd( 20, 1, cmd_list) ;
                
                ptdebug = XtasDebugList;
                while(ptdebug && !trouve) {
                    ttv_getsigname(XtasMainParam->ttvfig, buf, ptdebug->SIG1);
                    if(!strcmp(XtasDebugOtherSigName, buf))
                        trouve = 1;
                    else
                        ptdebug = ptdebug->NEXT;
                }

                if(ptdebug)
                    if(ptdebug->HOLD < XtasMargin)
                        mask |= XTAS_HOLD;


                XtasMainToolsCallback (widget, (XtPointer)(&mask), call_data) ;
            }
        }
    }
    else {
        XalDrawMessage(XtasErrorWidget, XTAS_NSIGERR);
    }
        
 
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDebugChronoChoiceCallback                                  */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/


void    XtasDebugChronoChoiceCallback   (parent, client_data, call_data)
Widget      parent;
XtPointer   client_data;
XtPointer   call_data;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data  ;

    if (state->set)
        XtasDebugChronoType = XtasDebugChronoType | (long)client_data;
    else
        XtasDebugChronoType = XtasDebugChronoType & ~((long)client_data);

    if (XtasDebug)
        XtasDrawChrono ();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbDebugCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasStbDebug ()
{
    int               n ;
    int               i ;
    Arg               args[10] ;
	Widget            main_window ;
	Widget            form ;
	Widget            form1 ;
	Widget            form2 ;
	Widget            form1signals ;
	Widget            form1buttons ;
	Widget            frame1 ;
	Widget            label  ; 
	Widget            label1 ; 
	Widget            label2 ;
	Widget            inputframe ;
	Widget            chronoframe ;
	Widget            chronolabel ;
	Widget            paned_window ;
	Widget            scrolled_window ;
	Widget            scrolled_list ;
	Widget            menu_bar ;
    Widget            buttons ;
    Widget            buttons_form ;
	Widget            subform, subform1, subform2 , separator;
    Widget            subform1buttons, row_widget;
    Widget            toggle;
    XmString          text ;
	ttvsig_list      *debugsig ;
	stbdebug_list    *ptdebug, *p ;
	char             *str ;
    char              buf[1024], buf2 [1024];
    long              v;
	int setupdir, holddir;

    if (XtasAppNotReady ()) 
		return ;
  
    if (XtasDebugWidget) { 
		XtDestroyWidget (XtasDebugWidget) ; 
		XtasDebugWidget = NULL ;
	}
	if (XtasDebugList) {
    	stb_freestbdebuglist (XtasDebugList) ; 
		XtasDebugList     = NULL ;
	}

	if (!XtasDebugSigName) 
		XalDrawMessage (XtasWarningWidget, XTAS_NVALERR) ; 
	
    if (!XtasDebugWidget && XtasDebugSigName) { 
		
		debugsig      = ttv_getsig (XtasMainParam->ttvfig, XtasDebugSigName) ;
		if (!debugsig) {
            sprintf (buf, "No signal named %s", XtasDebugSigName);
            XalDrawMessage (XtasErrorWidget, buf);
			return ;
        }
		XtasDebugList = stb_debugstberror (XtasMainParam->stbfig, debugsig, XtasMargin, STB_COMPUTE_DEBUG_CHRONO) ;
        XtasDebugList = stb_sortstbdebug (XtasDebugList) ;
													
		/* nouvelle fenetre */
	    sprintf (buf, XTAS_NAME": Debug... - %s", XtasMainParam->ttvfig->INFO->FIGNAME);	
   		XtasDebugWidget = XtVaCreatePopupShell ("XtasShellSubForm",
                                                topLevelShellWidgetClass, 
					                            XtasDeskMainForm,
                                                XmNwidth,            XtasDebugWindowWidth,
                                                XmNheight,           XtasDebugWindowHeight,
                                                XmNallowShellResize, False,
//		    									XmNtitle,            XTAS_NAME" : Debug...",
		    									XmNtitle,            buf,
                                                XmNdeleteResponse,   XmDESTROY,
                                                NULL) ;
		
		HelpFather = XtasDebugWidget ;
 		main_window = XmCreateMainWindow(XtasDebugWidget, "XtasMain", args, 0) ;
 		XtManageChild(main_window) ;
		
		/* menus  */
		
 		menu_bar = XtasCreateDebugMenus (main_window);
		XtManageChild(menu_bar) ;
		
		/* main form */
    	form = XtVaCreateManagedWidget (        "XtasSubForm", 
				                                xmFormWidgetClass, 
								                main_window, 
												XmNtopAttachment,   XmATTACH_WIDGET, 
		                                        XmNtopWidget,       menu_bar,
								                NULL) ;
                XtasDebugWindow.wrkwidget = form ;
                XtasDebugWindow.topwidget = XtasDebugWidget ;
                XtasDebugWindow.errwidget = XalCreateErrorBox(XtasDebugWindow.wrkwidget) ;
		/* buttons */
		
		buttons_form = XtVaCreateManagedWidget( "XtasStbDebugButtons", 
				                                xmFormWidgetClass, 
												form, 
												XmNtopAttachment,   XmATTACH_FORM, 
												XmNrightAttachment, XmATTACH_FORM, 
												XmNleftAttachment,  XmATTACH_FORM, 
												NULL) ;
 		n = 0;
		XtSetArg( args[n], XmNnumColumns,       1            ) ; n++;
		XtSetArg( args[n], XmNorientation,      XmHORIZONTAL ) ; n++;
		buttons = XalButtonMenus (buttons_form,  XtasDebugButtonMenus, args, n, 40, 40 ) ;
        XtasDebugWindow.butwidget = buttons;
		XtasAddDummyButton (buttons) ;
		XtManageChild (buttons) ;

		
		
		/* 2 subforms */
		
		paned_window = XtVaCreateManagedWidget( "XtasPanWindow", 
				                                xmPanedWindowWidgetClass, 
											    form, 
                                                XmNtopAttachment,    XmATTACH_WIDGET, 
                                                XmNtopWidget,        buttons_form, 
                                                XmNleftAttachment,   XmATTACH_FORM, 
                                                XmNrightAttachment,  XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_FORM, 
		                                        NULL) ;
											
    	form1 = XtVaCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, paned_window, NULL) ;
    	subform = XtVaCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, form1, 
                                                XmNtopAttachment,   XmATTACH_FORM,
//                                                XmNbottomAttachment,XmATTACH_FORM,
                                                XmNleftAttachment,  XmATTACH_FORM,
                                                XmNrightAttachment, XmATTACH_FORM,
                                                NULL) ;
        form1signals = XtVaCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, subform, 
                                                XmNtopAttachment,   XmATTACH_FORM,
                                                XmNbottomAttachment,XmATTACH_FORM,
                                                XmNleftAttachment,  XmATTACH_FORM,
                                                XmNrightAttachment, XmATTACH_POSITION,
                                                XmNrightPosition,   70,
                                                NULL );
		
        form1buttons = XtVaCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, subform, 
                                                XmNtopAttachment,   XmATTACH_FORM,
                                                XmNbottomAttachment,XmATTACH_FORM,
                                                XmNrightAttachment, XmATTACH_FORM,
                                                XmNleftAttachment,  XmATTACH_WIDGET,
                                                XmNleftWidget,      form1signals,
                                                NULL );

    	form2 = XtVaCreateManagedWidget (       "XtasSubForm", 
				                                xmFormWidgetClass, 
								                paned_window, 
								                NULL) ;

		/* entete */
		
		frame1 = XtVaCreateManagedWidget(       "XtasFrame", 
                                                xmFrameWidgetClass,
								                form1signals, 
                                                XmNborderWidth,      0,             
												XmNmarginWidth,      0, 
                                                XmNtopAttachment,    XmATTACH_FORM, 
                                                XmNleftAttachment,   XmATTACH_FORM, 
                                                XmNrightAttachment,  XmATTACH_FORM, 
								                NULL) ;
								 
		str = (char*)mbkalloc(1024 * sizeof(char)) ;
		if (XtasDebugSigName)
			sprintf(str, "%s%s", "Error Signal: ", XtasDebugSigName) ;
		else
			sprintf(str, "%s", "Error Signal: ") ;
			
    	text = XmStringCreateSimple(str) ;
		mbkfree(str) ;

    	label1 = XtVaCreateManagedWidget(       "XtasLabel", 
				                                xmPushButtonGadgetClass,
										        frame1,
    	                                        XmNlabelString,      text,
												XmNalignment,        XmALIGNMENT_BEGINNING, 
								                NULL) ;
    	XmStringFree(text) ;
		
		/* liste des signaux */
		
		inputframe = XtVaCreateManagedWidget(  "XtasFrame", 
                                                xmFrameWidgetClass,
								                form1signals, 
                                                XmNborderWidth,      0,             
                                                XmNtopAttachment,    XmATTACH_WIDGET, 
                                                XmNtopWidget,        frame1, 
                                                XmNleftAttachment,   XmATTACH_FORM, 
                                                XmNrightAttachment,  XmATTACH_FORM, 
								                NULL) ;
								 
    	text = XmStringCreateSimple("Input Signals List") ;
    	label2 = XtVaCreateManagedWidget(       "XtasLabel", 
				                                xmLabelWidgetClass, 
										        inputframe,
    	                                        XmNlabelString,      text,
												XmNmarginWidth,      10,  
												XmNalignment,        XmALIGNMENT_BEGINNING, 
								                NULL) ;
    	XmStringFree(text) ;
		
		n = 0 ;
        XtSetArg( args[n],                      XmNtopAttachment,    XmATTACH_WIDGET) ; n++;
        XtSetArg( args[n],                      XmNtopWidget,        inputframe     ) ; n++;
        XtSetArg( args[n],                      XmNtopOffset,        2              ) ; n++;
        XtSetArg( args[n],                      XmNleftAttachment,   XmATTACH_FORM  ) ; n++;
        XtSetArg( args[n],                      XmNrightAttachment,  XmATTACH_FORM  ) ; n++;
        XtSetArg( args[n],                      XmNscrollingPolicy,  XmSTATIC       ) ; n++;
        XtSetArg( args[n],                      XmNheight,           90             ) ; n++;
		scrolled_list =  XmCreateScrolledList (form1signals, "XtasUserField", args, n) ;
		
		XmListDeleteAllItems (scrolled_list) ;

		i = 1 ;
		str = (char*)mbkalloc (1024 * sizeof (char)) ;
		for (ptdebug = XtasDebugList ; ptdebug ; ptdebug = ptdebug->NEXT) {
			str = ttv_getsigname (XtasMainParam->ttvfig, str, ptdebug->SIG1) ;
            sprintf(buf,"from %s", str);
            if (ptdebug->SIG1_EVENT!=NULL)
            {
              if (ptdebug->SIG1_EVENT->TYPE & TTV_NODE_UP) strcat(buf," (R)");
              else strcat(buf," (F)");
            }
            if (ptdebug->START_CMD_EVENT!=NULL)
            {
              strcat(buf,", start cmd: ");
              str = ttv_getsigname (XtasMainParam->ttvfig, str,ptdebug->START_CMD_EVENT->ROOT) ;
              strcat(buf, str);
              if (ptdebug->START_CMD_EVENT->TYPE & TTV_NODE_UP) strcat(buf," (R)");
              else strcat(buf," (F)");
            }
            if (ptdebug->CMDNAME!=NULL)
            {
              strcat(buf,", end cmd: ");
              strcat(buf, ptdebug->CMDNAME);
            }

        	text = XmStringCreateSimple (buf) ; 
        	XmListAddItemUnselected (scrolled_list, text, i) ;
        	XmStringFree (text) ;
			i++ ;
		}
		mbkfree(str) ;
		
		XtAddCallback (label1, 
                       XmNactivateCallback, 
                       XtasDebugInfo2Callback,
					   NULL);
		XtAddCallback (scrolled_list, 
					   XmNdefaultActionCallback, 
					   XtasDrawChronoCallback, 
					   NULL);
		XtAddCallback (scrolled_list, 
					   XmNbrowseSelectionCallback, 
					   XtasDrawChronoCallback, 
					   NULL);
		XtManageChild (scrolled_list) ;


        /* titre colonnes */
        n = 0;
        XtSetArg (args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
        XtSetArg (args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg (args[n], XmNleftOffset,       58              ); n++;
        subform1buttons = XtCreateManagedWidget ("XtasForms", xmFormWidgetClass, form1buttons, args, n); 
        
        n = 0;
        text = XmStringCreateSimple ("RISE & FALL");
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNheight,          30                          ); n++;
        XtSetArg ( args[n], XmNwidth,           80                          ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_CENTER          ); n++;
        label = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, subform1buttons, args, n );
        XmStringFree (text);

        n = 0;
        text = XmStringCreateSimple ("RISE");
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNleftWidget,      label                       ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label                       ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label                       ); n++;
        XtSetArg ( args[n], XmNwidth,           80                          ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_CENTER          ); n++;
        label = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, subform1buttons, args, n );
        XmStringFree (text);

        n = 0;
        text = XmStringCreateSimple ("FALL");
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNleftWidget,      label                       ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label                       ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label                       ); n++;
        XtSetArg ( args[n], XmNlabelString,     text                        ); n++;
        XtSetArg ( args[n], XmNwidth,           80                          ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_CENTER          ); n++;
        label = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, subform1buttons, args, n );
        XmStringFree (text);

        /* buttons chrono */
        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, form1buttons,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       subform1buttons,
                                           XmNbottomAttachment,XmATTACH_FORM,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      2,
                                           XmNorientation,     XmHORIZONTAL,
                                           XmNisAligned,       True,
                                           XmNentryAlignment,  XmALIGNMENT_BEGINNING,
                                           XmNspacing,         40,
                              //             XmNradioBehavior,   True,
                              //             XmNradioAlwaysOne,  True,
                                           NULL) ;
        /* 1ere ligne */
        n = 0;
        text = XmStringCreateSimple ("Input");
        XtSetArg ( args[n], XmNlabelString, text    ); n++;
        label = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree (text);

        text = XmStringCreateSimple( " " ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasDebugChronoType & XTAS_DEBUG_IUD)==XTAS_DEBUG_IUD) ? True:False,
                                      XmNlabelString,   text,
                                      XmNalignment,     XmALIGNMENT_CENTER,
                                      NULL) ;
        XmStringFree( text ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasDebugChronoChoiceCallback, (XtPointer)XTAS_DEBUG_IUD) ;

        text = XmStringCreateSimple( " " ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasDebugChronoType & XTAS_DEBUG_IU)==XTAS_DEBUG_IU) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
        XmStringFree( text ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasDebugChronoChoiceCallback, (XtPointer)XTAS_DEBUG_IU) ;

        text = XmStringCreateSimple( " " ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasDebugChronoType & XTAS_DEBUG_ID)==XTAS_DEBUG_ID) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
        XmStringFree( text ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasDebugChronoChoiceCallback, (XtPointer)XTAS_DEBUG_ID) ;

        /* 2e ligne */
        n = 0;
        text = XmStringCreateSimple ("Output");
        XtSetArg ( args[n], XmNlabelString, text    ); n++;
        label = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, row_widget, args, n );
        XmStringFree (text);
 
        text = XmStringCreateSimple( " " ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasDebugChronoType & XTAS_DEBUG_OUD)==XTAS_DEBUG_OUD) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
        XmStringFree( text ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasDebugChronoChoiceCallback, (XtPointer)XTAS_DEBUG_OUD) ;

        text = XmStringCreateSimple( " " ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasDebugChronoType & XTAS_DEBUG_OU)==XTAS_DEBUG_OU) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
        XmStringFree( text ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasDebugChronoChoiceCallback, (XtPointer)XTAS_DEBUG_OU) ;

        text = XmStringCreateSimple( " " ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasDebugChronoType & XTAS_DEBUG_OD)==XTAS_DEBUG_OD) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
        XmStringFree( text ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasDebugChronoChoiceCallback, (XtPointer)XTAS_DEBUG_OD) ;

         

        if (XtasDebugList) {
            /* Report */
            frame1 = XtVaCreateManagedWidget("XtasFrame", 
                                                    xmFrameWidgetClass,
    								                form1, 
                                                    XmNborderWidth,      0,             
                                                    XmNtopAttachment,    XmATTACH_WIDGET, 
                                                    XmNtopWidget,        subform, 
                                                    XmNtopOffset,        2, 
                                                    XmNleftAttachment,   XmATTACH_FORM, 
                                                    XmNrightAttachment,  XmATTACH_FORM, 
    								                NULL) ;
    	    subform1= XtVaCreateManagedWidget("XtasForm", 
                                                    xmFormWidgetClass,
    								                frame1, 
    								                NULL) ;						 
    
     
            /* Report SETUP */
            subform2= XtVaCreateManagedWidget("XtasForm", 
                                                    xmFormWidgetClass,
    								                subform1, 
                                                    XmNtopAttachment,    XmATTACH_FORM,
                                                    XmNbottomAttachment, XmATTACH_FORM,
                                                    XmNleftAttachment,   XmATTACH_FORM,
                                                    XmNrightAttachment,  XmATTACH_POSITION,
                                                    XmNrightPosition,    50,
    								                NULL) ;						 
    
    
            v = XtasDebugList->SETUP;
            ptdebug = XtasDebugList;
            for (p=XtasDebugList->NEXT ; p ; p=p->NEXT) {
                if ( v > p->SETUP) {
                     v = p->SETUP;
                     ptdebug=p;
                }
            }

                /* CLOCK REF*/
        	text = XmStringCreateSimple ("CLOCK REF") ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
    												XmNmarginWidth,      10,  
    												XmNwidth,            100,  
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
            if ((ptdebug->CKREFUP_MIN==STB_NO_TIME)||(ptdebug->CKREFDN_MIN==STB_NO_TIME)) {
                if (ptdebug->PERIODE==STB_NO_TIME)
                    sprintf (buf, ": ");
                else
                    sprintf (buf, ": PERIOD = %.1f ps", ptdebug->PERIODE/TTV_UNIT);
            }
            else
                sprintf (buf, ": RISE = %.1f ps  FALL = %.1f ps  PERIOD = %.1f ps", (ptdebug->CKREFUP_MIN+ptdebug->PERIODE)/TTV_UNIT, (ptdebug->CKREFDN_MIN+ptdebug->PERIODE)/TTV_UNIT, ptdebug->PERIODE/TTV_UNIT);
        	text = XmStringCreateSimple (buf) ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                    XmNtopWidget,        label1, 
                                                    XmNleftAttachment,   XmATTACH_WIDGET, 
                                                    XmNleftWidget,       label1, 
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
                
    
            if ((ptdebug->CKUP_MIN != STB_NO_TIME) && (ptdebug->CKDN_MIN != STB_NO_TIME))  {
                /* CLOCK */
            	text = XmStringCreateSimple ("CLOCK") ;
        	    label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
                                                    XmNtopAttachment,    XmATTACH_WIDGET, 
                                                    XmNtopWidget,        label1, 
       	                                            XmNlabelString,      text,
    												XmNmarginWidth,      10,  
    												XmNwidth,            100,  
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
            	XmStringFree (text) ;
                sprintf (buf, ": RISE = %.1f ps  FALL = %.1f ps", (ptdebug->CKUP_MIN+ptdebug->PERIODE)/TTV_UNIT, (ptdebug->CKDN_MIN+ptdebug->PERIODE)/TTV_UNIT);
        	    text = XmStringCreateSimple (buf) ;
        	    label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                    XmNtopWidget,        label1, 
                                                    XmNleftAttachment,   XmATTACH_WIDGET, 
                                                    XmNleftWidget,       label1, 
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
            	XmStringFree (text) ;
            }
            
                /* DATA DELAY */
    		
        	text = XmStringCreateSimple ("DATA DELAY") ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_WIDGET, 
                                                    XmNtopWidget,        label1, 
    												XmNwidth,            100,  
    												XmNmarginWidth,      10,  
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
              
            
            if (ptdebug->CHRONO)
              sprintf (buf, ": %.1f ps", (ptdebug->CHRONO->SIG2S->U+ptdebug->PERIODE)/TTV_UNIT);
        	text = XmStringCreateSimple (buf) ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                    XmNtopWidget,        label1, 
                                                    XmNleftAttachment,   XmATTACH_WIDGET, 
                                                    XmNleftWidget,       label1, 
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
              
    
            if (ptdebug->SETUP != STB_NO_TIME) { 
                /* SETUP */
            	text = XmStringCreateSimple ("SETUP") ;
            	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
        				                                xmLabelWidgetClass, 
        										        subform2,
            	                                        XmNlabelString,      text,
                                                        XmNtopAttachment,    XmATTACH_WIDGET, 
                                                        XmNtopWidget,        label1, 
        												XmNmarginWidth,      10,  
        												XmNwidth,            100,  
        												XmNalignment,        XmALIGNMENT_BEGINNING, 
        								                NULL) ;
            	XmStringFree (text) ;
        
                sprintf (buf, ": %.1f ps", ptdebug->SETUP/TTV_UNIT);
                if (ptdebug->MARGESETUP !=0) {
                    sprintf (buf2, " | incl. %+.1fps intrinsic", ptdebug->MARGESETUP/TTV_UNIT);
                    strcat (buf, buf2);
                }
                if (ptdebug->detail[0].setup.VALUE==ptdebug->SETUP) setupdir=0; else setupdir=1;
                if (ptdebug->detail[setupdir].setup.skew!=0) {
                    sprintf (buf2, " | incl. %.1fps skew correction", ptdebug->detail[setupdir].setup.skew/TTV_UNIT);
                    strcat (buf, buf2);
                }
                if (ptdebug->detail[setupdir].setup.data_margin!=0 || ptdebug->detail[setupdir].setup.clock_margin!=0) {
                    sprintf (buf2, " | incl. (%.1fps%+.1fps) data/clock path margin", ptdebug->detail[setupdir].setup.data_margin/TTV_UNIT, -ptdebug->detail[setupdir].setup.clock_margin/TTV_UNIT);
                    strcat (buf, buf2);
                }
                        
            	text = XmStringCreateSimple (buf) ;
            	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
        				                                xmLabelWidgetClass, 
        										        subform2,
            	                                        XmNlabelString,      text,
                                                        XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                        XmNtopWidget,        label1, 
                                                        XmNleftAttachment,   XmATTACH_WIDGET, 
                                                        XmNleftWidget,       label1, 
        												XmNalignment,        XmALIGNMENT_BEGINNING, 
        								                NULL) ;
            	XmStringFree (text) ;
        
            }
    
            /* Vertical Separator */
            n = 0;
            XtSetArg ( args[n],  XmNtopAttachment,      XmATTACH_FORM   ); n++;
    //        XtSetArg ( args[n],  XmNtopOffset,          5               ); n++;
            XtSetArg ( args[n],  XmNbottomAttachment,   XmATTACH_FORM   ); n++;
    //        XtSetArg ( args[n],  XmNbottomOffset,       5               ); n++;
            XtSetArg ( args[n],  XmNleftAttachment,     XmATTACH_WIDGET ); n++;
            XtSetArg ( args[n],  XmNleftWidget,         subform2        ); n++;
    //        XtSetArg ( args[n],  XmNleftOffset,         5               ); n++;
            XtSetArg ( args[n],  XmNorientation,        XmVERTICAL      ); n++;
            separator = XtCreateManagedWidget("XtasSeparator", xmSeparatorWidgetClass, subform1 , args, n);
    
             /* Report HOLD */
    	    subform2= XtVaCreateManagedWidget("XtasForm", 
                                                    xmFormWidgetClass,
    								                subform1, 
                                                    XmNtopAttachment,    XmATTACH_FORM,
                                                    XmNbottomAttachment, XmATTACH_FORM,
                                                    XmNleftAttachment,   XmATTACH_WIDGET,
                                                    XmNleftWidget,       separator,
                                                    XmNleftOffset,       3,
                                                    XmNrightAttachment,  XmATTACH_FORM,
    								                NULL) ;						 
    
    
            v = XtasDebugList->HOLD;
            ptdebug = XtasDebugList;
            for (p=XtasDebugList->NEXT ; p ; p=p->NEXT) {
                if ( v > p->HOLD) {
                     v = p->HOLD;
                     ptdebug=p;
                }
            }

                /* CLOCK REF*/
        	text = XmStringCreateSimple ("CLOCK REF") ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
    												XmNmarginWidth,      10,  
    												XmNwidth,            100,  
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
            if ((ptdebug->CKREFUP_MAX==STB_NO_TIME)||(ptdebug->CKREFDN_MAX==STB_NO_TIME)) {
                if (ptdebug->PERIODE==STB_NO_TIME)
                    sprintf (buf, ": ");
                else
                    sprintf (buf, ": PERIOD = %.1f ps", ptdebug->PERIODE/TTV_UNIT);
            }
            else
                sprintf (buf, ": RISE = %.1f ps  FALL = %.1f ps  PERIOD = %.1f ps", ptdebug->CKREFUP_MAX/TTV_UNIT, ptdebug->CKREFDN_MAX/TTV_UNIT, ptdebug->PERIODE/TTV_UNIT);
        	text = XmStringCreateSimple (buf) ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                    XmNtopWidget,        label1, 
                                                    XmNleftAttachment,   XmATTACH_WIDGET, 
                                                    XmNleftWidget,       label1, 
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
               
            if ((ptdebug->CKUP_MAX != STB_NO_TIME) && (ptdebug->CKDN_MAX != STB_NO_TIME)) {
                /* CLOCK */
            	text = XmStringCreateSimple ("CLOCK") ;
            	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
        				                                xmLabelWidgetClass, 
        										        subform2,
                                                        XmNtopAttachment,    XmATTACH_WIDGET, 
                                                        XmNtopWidget,        label1, 
           	                                            XmNlabelString,      text,
        												XmNmarginWidth,      10,  
        												XmNwidth,            100,  
        												XmNalignment,        XmALIGNMENT_BEGINNING, 
        								                NULL) ;
            	XmStringFree (text) ;
                sprintf (buf, ": RISE = %.1f ps  FALL = %.1f ps", ptdebug->CKUP_MAX/TTV_UNIT, ptdebug->CKDN_MAX/TTV_UNIT);
            	text = XmStringCreateSimple (buf) ;
            	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
        				                                xmLabelWidgetClass, 
        										        subform2,
            	                                        XmNlabelString,      text,
                                                        XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                        XmNtopWidget,        label1, 
                                                        XmNleftAttachment,   XmATTACH_WIDGET, 
                                                        XmNleftWidget,       label1, 
        												XmNalignment,        XmALIGNMENT_BEGINNING, 
        								                NULL) ;
            	XmStringFree (text) ;
            }
    
                /* DATA DELAY */
    		
        	text = XmStringCreateSimple ("DATA DELAY") ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_WIDGET, 
                                                    XmNtopWidget,        label1, 
    												XmNwidth,            100,  
    												XmNmarginWidth,      10,  
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
              
            if (ptdebug->CHRONO)
              sprintf (buf, ": %.1f ps", (ptdebug->CHRONO->SIG2S->D+ptdebug->PERIODE)/TTV_UNIT);
        	text = XmStringCreateSimple (buf) ;
        	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
    				                                xmLabelWidgetClass, 
    										        subform2,
        	                                        XmNlabelString,      text,
                                                    XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                    XmNtopWidget,        label1, 
                                                    XmNleftAttachment,   XmATTACH_WIDGET, 
                                                    XmNleftWidget,       label1, 
    												XmNalignment,        XmALIGNMENT_BEGINNING, 
    								                NULL) ;
        	XmStringFree (text) ;
             
            if (ptdebug->HOLD != STB_NO_TIME) {
                    /* HOLD */
            	text = XmStringCreateSimple ("HOLD") ;
            	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
        				                                xmLabelWidgetClass, 
        										        subform2,
            	                                        XmNlabelString,      text,
                                                        XmNtopAttachment,    XmATTACH_WIDGET, 
                                                        XmNtopWidget,        label1, 
        												XmNmarginWidth,      10,  
        												XmNwidth,            100,  
        												XmNalignment,        XmALIGNMENT_BEGINNING, 
        								                NULL) ;
            	XmStringFree (text) ;
        
                sprintf (buf, ": %.1f ps", ptdebug->HOLD/TTV_UNIT);
                if (ptdebug->MARGEHOLD !=0){
                    sprintf (buf2, " | incl. %+.1fps intrinsic", ptdebug->MARGEHOLD/TTV_UNIT);
                    strcat (buf, buf2);
                }
                if (ptdebug->detail[0].hold.VALUE==ptdebug->HOLD) holddir=0; else holddir=1;
                if (ptdebug->detail[holddir].hold.skew!=0) {
                    sprintf (buf2, " | incl. %.1fps skew correction", ptdebug->detail[holddir].hold.skew/TTV_UNIT);
                    strcat (buf, buf2);
                }
                if (ptdebug->detail[holddir].hold.data_margin!=0 || ptdebug->detail[holddir].hold.clock_margin!=0) {
                    sprintf (buf2, " | incl. (%.1fps%+.1fps) data/clock path margin", -ptdebug->detail[holddir].hold.data_margin/TTV_UNIT, ptdebug->detail[holddir].hold.clock_margin/TTV_UNIT);
                    strcat (buf, buf2);
                }
            	text = XmStringCreateSimple (buf) ;
            	label1 = XtVaCreateManagedWidget ( "XtasLabel", 
        				                                xmLabelWidgetClass, 
        										        subform2,
            	                                        XmNlabelString,      text,
                                                        XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
                                                        XmNtopWidget,        label1, 
                                                        XmNleftAttachment,   XmATTACH_WIDGET, 
                                                        XmNleftWidget,       label1, 
        												XmNalignment,        XmALIGNMENT_BEGINNING, 
        								                NULL) ;
            	XmStringFree (text) ;
            }
    
        }
		
		/* fenetre graphique */
		
		chronoframe = XtVaCreateManagedWidget("XtasFrame", 
                                                xmFrameWidgetClass,
								                form2, 
                                                XmNborderWidth,      0,             
                                                XmNtopAttachment,    XmATTACH_FORM, 
                                                XmNleftAttachment,   XmATTACH_FORM, 
                                                XmNrightAttachment,  XmATTACH_FORM, 
								                NULL) ;
								 
    	text = XmStringCreateSimple ("Slack Timing Diagrams") ;
    	chronolabel = XtVaCreateManagedWidget ( "XtasLabel", 
				                                xmLabelWidgetClass, 
										        chronoframe,
    	                                        XmNlabelString,      text,
												XmNmarginWidth,      10,  
												XmNalignment,        XmALIGNMENT_BEGINNING, 
								                NULL) ;
    	XmStringFree (text) ;
		scrolled_window = XtVaCreateManagedWidget ("XtasScrolledWindow",
				                                xmScrolledWindowWidgetClass, 
												form2,
												XmNscrollingPolicy,  XmAUTOMATIC,
                                            XmNtopAttachment,   XmATTACH_WIDGET,
                                            XmNtopWidget,       chronoframe,
                                            XmNbottomAttachment,XmATTACH_FORM,
                                            XmNrightAttachment, XmATTACH_POSITION,
                                            XmNrightPosition,   87,
                                                XmNleftAttachment,   XmATTACH_FORM, 
/*                                                XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
												XmNtopWidget,        subform, 
                                                XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET, 
												XmNbottomWidget,     subform, 
                                                XmNleftAttachment,   XmATTACH_FORM, 
                                                XmNrightAttachment,  XmATTACH_WIDGET, 
                                                XmNrightWidget,      subform, */
								                NULL) ;
		
        subform = XtVaCreateManagedWidget ("XtasChronoDataForm", 
                                            xmFormWidgetClass,
                                            form2,
                                                XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET, 
												XmNtopWidget,        scrolled_window, 
                                                XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET, 
												XmNbottomWidget,     scrolled_window, 
                                                XmNleftAttachment,   XmATTACH_WIDGET, 
                                                XmNleftWidget,       scrolled_window,
/*                                            XmNtopAttachment,   XmATTACH_WIDGET,
                                            XmNtopWidget,       chronoframe,
                                            XmNbottomAttachment,XmATTACH_FORM,*/
                                            XmNrightAttachment, XmATTACH_FORM,
//                                            XmNwidth,           130,
                                            NULL);
        XtasDebugLabelPeriod = XtVaCreateManagedWidget ("XtasDebugLabelPeriod", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNtopAttachment,   XmATTACH_FORM,
                                                        XmNtopOffset,       10,
                                                        XmNleftAttachment,  XmATTACH_FORM,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNleftOffset,      5,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelPeriod, " ");
        XtasDebugLabelSetup = XtVaCreateManagedWidget ("XtasDebugLabelSetup", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNtopAttachment,   XmATTACH_WIDGET,
                                                        XmNtopWidget,       XtasDebugLabelPeriod,
                                                        XmNtopOffset,       30,
                                                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                                                        XmNleftWidget,      XtasDebugLabelPeriod,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelSetup, " ");
        XtasDebugLabelHold = XtVaCreateManagedWidget ("XtasDebugLabelHold", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNtopAttachment,   XmATTACH_WIDGET,
                                                        XmNtopWidget,       XtasDebugLabelSetup,
                                                        XmNtopOffset,       10,
                                                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                                                        XmNleftWidget,      XtasDebugLabelSetup,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelHold, " ");
        XtasDebugLabelDelta = XtVaCreateManagedWidget ("XtasDebugLabelDelta", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNbottomAttachment,XmATTACH_FORM,
                                                        XmNbottomOffset,    10,
                                                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                                                        XmNleftWidget,      XtasDebugLabelSetup,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelDelta, " ");
        XtasDebugLabelT2 = XtVaCreateManagedWidget ("XtasDebugLabelT2", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNbottomAttachment,XmATTACH_WIDGET,
                                                        XmNbottomWidget,    XtasDebugLabelDelta,
                                                        XmNbottomOffset,    10,
                                                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                                                        XmNleftWidget,      XtasDebugLabelSetup,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelT2, " ");
        XtasDebugLabelT1 = XtVaCreateManagedWidget ("XtasDebugLabelT1", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNbottomAttachment,XmATTACH_WIDGET,
                                                        XmNbottomWidget,    XtasDebugLabelT2,
                                                        XmNbottomOffset,    10,
                                                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                                                        XmNleftWidget,      XtasDebugLabelSetup,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelT1, " ");
        XtasDebugLabelMode = XtVaCreateManagedWidget ("XtasDebugLabelMode", 
                                                        xmLabelWidgetClass,
                                                        subform,
                                                        XmNbottomAttachment,XmATTACH_WIDGET,
                                                        XmNbottomWidget,    XtasDebugLabelT1,
                                                        XmNbottomOffset,    10,
                                                        XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET,
                                                        XmNleftWidget,      XtasDebugLabelSetup,
                                                        XmNrightAttachment, XmATTACH_FORM,
                                                        XmNheight,          30,
                                                        XmNalignment,       XmALIGNMENT_BEGINNING,
                                                        NULL );
        XtasSetLabelString (XtasDebugLabelMode, " ");

        translations = XtParseTranslationTable (XtasTranslations) ;
		
		XtasGraphicWindow = XtVaCreateManagedWidget ("XtasDrawingArea",
 												xmDrawingAreaWidgetClass, 
												scrolled_window,
												XmNtranslations,     translations,   
                                                XmNwidth,            XtasDrawingAreaWidth, 
                                                XmNheight,           XtasDrawingAreaHeight, 
                                                XmNtopAttachment,    XmATTACH_FORM, 
                                                XmNleftAttachment,   XmATTACH_FORM, 
                                                XmNrightAttachment,  XmATTACH_FORM, 
                                                XmNbottomAttachment, XmATTACH_FORM, 
												XmNbackground, 0,
												NULL) ;
		
        XtasInitGraphics () ;
		XtManageChild (XtasDebugWidget) ;
	}
}

/*---------------------------------------------------------------------------*/

void XtasSelectDebugCancelCallback (widget, client_data, call_data)
Widget    widget;
XtPointer client_data;
XtPointer call_data;
{
    XtUnmanageChild(widget);
  //XalLeaveLimitedLoop () ;
}

/*---------------------------------------------------------------------------*/

void XtasSelectDebugOkCallback (widget,client_data,call_data )
Widget    widget;
XtPointer client_data ;
XtPointer call_data;
{
	XtasMargin = atoi (XmTextGetString (XtasSelectMarginField))*TTV_UNIT ;
        XtasSetDebugSignal (XmTextGetString (XtasSelectDebugField)) ;
	XtasStbDebug () ;
    //XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/

void XtasSelectSignalCallback (widget, key, call_data)
Widget  widget;
XtPointer key;
XtPointer call_data;
{
 XtasSignalsTopWidget =  XtParent (XtasSelectDebugWidget) ;
 XtasDeskSignalsViewCallback (widget, key, call_data) ;
}

/*---------------------------------------------------------------------------*/

Widget XtasSetDebugSignal (name)
char  *name ;
{
 	if (name) {
                if((XtasDebugSigName != NULL) && (name != XtasDebugSigName))
                 {
                  mbkfree(XtasDebugSigName) ;
                  XtasDebugSigName = mbkstrdup (name) ;
                 }
                else if(XtasDebugSigName == NULL)
                 {
                  XtasDebugSigName = mbkstrdup (name) ;
                 }
 		XmTextSetString (XtasSelectDebugField, XtasDebugSigName) ;
  	} else {
                if(XtasDebugSigName != NULL)
                  mbkfree(XtasDebugSigName) ;
		XtasDebugSigName = NULL ;
 		XmTextSetString (XtasSelectDebugField, "") ;
	}

 	return XtasSelectDebugField ;
}

/*---------------------------------------------------------------------------*/

void XtasStbDebugCallback (widget, client_data, call_data)
Widget    widget ;
XtPointer client_data ;
XtPointer call_data ;
{
	int      n, i ;
	Arg      args[10] ;
	XmString text ;
	Widget   form, form_t, dbase_form, ebase_form, fbase_form ;
	Widget   frame_widget, label_widget ;
	Widget   tmp_widget ;
	char     buf[1024] ;
    Atom     WM_DELETE_WINDOW;

    if (((long)client_data==XTAS_DEBUG) && (XtasDebugOtherSigName))
        XtasDebugSigName = mbkstrdup (XtasDebugOtherSigName);
    else
        XtasDebugSigName = mbkstrdup(XtasStbSigName) ;

  	if (XtasAppNotReady ()) 
	  	return ;
  
  	if (!XtasSelectDebugWidget) {
		
		n = 0 ;
        XtSetArg (args[n], XmNtitle, XTAS_NAME": Signal Selection ..." ) ; n++ ;
        XtSetArg (args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ) ; n++ ;
        XtasSelectDebugWidget = XmCreatePromptDialog (XtasDeskMainForm, "XtasInfosBox", args, n) ;
        XtUnmanageChild (XmSelectionBoxGetChild (XtasSelectDebugWidget, XmDIALOG_TEXT)) ;
        XtUnmanageChild (XmSelectionBoxGetChild (XtasSelectDebugWidget, XmDIALOG_PROMPT_LABEL));
        XtUnmanageChild (XmSelectionBoxGetChild (XtasSelectDebugWidget, XmDIALOG_HELP_BUTTON));
        XtAddCallback (XtasSelectDebugWidget, XmNokCallback, XtasSelectDebugOkCallback, (XtPointer)XtasSelectDebugWidget) ;
        XtAddCallback (XtasSelectDebugWidget, XmNcancelCallback, XtasSelectDebugCancelCallback, (XtPointer)XtasSelectDebugWidget) ;
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasSelectDebugWidget), WM_DELETE_WINDOW, XtasSelectDebugCancelCallback, (XtPointer )XtasSelectDebugWidget);
        XmAddTabGroup (XtasSelectDebugWidget) ; 
        
        form = XtCreateManagedWidget ("XtasDeskMainForm", xmFormWidgetClass, XtasSelectDebugWidget, args, n) ;
    
		/* 1st sub form */
	
    	dbase_form = XtVaCreateManagedWidget ("XtasTitleFrame", xmFrameWidgetClass, form,
                                           XmNborderWidth,      0,                
                                           XmNtopAttachment,    XmATTACH_FORM,    
                                           XmNleftAttachment,   XmATTACH_FORM,    
                                           XmNrightAttachment,  XmATTACH_FORM,
										   NULL) ;   
    	form_t = dbase_form ;
	
		/* Signal Selection */
    
    	ebase_form = XtVaCreateManagedWidget ("XtasTitleForm", xmFormWidgetClass, dbase_form,
                                           XmNborderWidth,      0,
                                           XmNtopAttachment,    XmATTACH_FORM,    
                                           XmNleftAttachment,   XmATTACH_FORM,    
                                           XmNrightAttachment,  XmATTACH_POSITION,
                                           XmNrightPosition,    75, 
										   NULL) ;
    
    
    	frame_widget = XtVaCreateManagedWidget ("XtasTitleFrame", xmFrameWidgetClass, ebase_form,
                                           XmNtopAttachment,    XmATTACH_FORM,
                                           XmNleftAttachment,   XmATTACH_FORM,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,       2,
										   NULL) ;           
	
    	text = XmStringCreateSimple ("Signal Selection") ;
    	label_widget = XtVaCreateManagedWidget("XtasMainTitles", xmLabelWidgetClass, frame_widget,
    					    			   XmNlabelString, text,
										   NULL) ; 
    	XmStringFree (text) ;
    
    	fbase_form = XtVaCreateManagedWidget ("XtasTitleForm", xmFormWidgetClass, ebase_form,
                                           XmNborderWidth,      0,              
                                           XmNtopAttachment,    XmATTACH_WIDGET,
                                           XmNtopWidget,        frame_widget,   
                                           XmNleftAttachment,   XmATTACH_FORM,  
                                           XmNrightAttachment,  XmATTACH_FORM,
									       NULL) ;  
    
    	text = XmStringCreateSimple ("Signal") ;
    	label_widget = XtVaCreateManagedWidget ("XtasSubTitles", xmLabelGadgetClass, fbase_form, 
                                           XmNlabelString,      text,             
                                           XmNtopAttachment,    XmATTACH_FORM,    
                                           XmNtopOffset,        7,                
                                           XmNleftAttachment,   XmATTACH_FORM,    
                                           XmNheight,           35,               
                                           XmNwidth,            50,
										   NULL) ;              
    	XmStringFree (text) ;

    	XtasSelectDebugField = XtVaCreateManagedWidget ("XtasUserField", xmTextWidgetClass, fbase_form,
                                           XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET,
                                           XmNtopWidget,        label_widget,            
                                           XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET,
                                           XmNbottomWidget,     label_widget,            
                                           XmNrightAttachment,  XmATTACH_POSITION,       
                                           XmNrightPosition,    75,                      
                                           XmNleftAttachment,   XmATTACH_WIDGET,         
                                           XmNleftWidget,       label_widget,            
                                           XmNshadowThickness,  2,
										   NULL) ;           
		
    	n = 0 ;
    	XtSetArg (args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET) ; n++ ;
    	XtSetArg (args[n], XmNtopWidget,        XtasSelectDebugField) ;     n++ ;
    	XtSetArg (args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    	XtSetArg (args[n], XmNbottomWidget,     XtasSelectDebugField) ;    n++ ;
    	XtSetArg (args[n], XmNrightAttachment,  XmATTACH_FORM) ;           n++ ;
    	XtSetArg (args[n], XmNrightOffset,      3) ;                       n++ ;
    	XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET) ;         n++ ;
    	XtSetArg (args[n], XmNleftWidget,       XtasSelectDebugField) ;    n++ ;
    	XtSetArg (args[n], XmNshadowThickness,  1) ;                       n++ ;
    	tmp_widget = XmCreateMenuBar (fbase_form, "XtasMenuBar", args, n) ;

    	XmAddTabGroup (XtasSelectDebugField) ;
		if (!XtasDebugSigName)
    		XmTextSetString (XtasSelectDebugField, "") ;
		else
    		XmTextSetString (XtasSelectDebugField, XtasDebugSigName) ;


    	for (i = 0 ; i < XTAS_C_NBSIGTYPE ; i++)
        	XtasSelectDebugMenuView[i].callback_data += 60 ;

    	XalBuildMenus (tmp_widget, XtasSelectDebugSignalMenu) ;
    	XtManageChild (tmp_widget) ;

		/* 2 eme sub form */
   
    	dbase_form = XtVaCreateManagedWidget ("XtasTitleFrame", xmFrameWidgetClass, form,
                                           XmNtopAttachment,    XmATTACH_WIDGET, 
                                           XmNtopWidget,        form_t,
                                           XmNtopOffset,        5,
                                           XmNleftAttachment,   XmATTACH_FORM,
                                           XmNrightAttachment,  XmATTACH_FORM,
										   NULL) ;
    	form_t = dbase_form;
    
    	ebase_form = XtVaCreateManagedWidget ("XtasTitleForm", xmFormWidgetClass, dbase_form, NULL) ;

		/* Margin */
    
    	fbase_form = XtVaCreateManagedWidget ("XtasTitleForm", xmFormWidgetClass, ebase_form, 
                                           XmNborderWidth,      0,
                                           XmNtopAttachment,    XmATTACH_FORM,
                                           XmNleftAttachment,   XmATTACH_FORM,
                                           XmNrightAttachment,  XmATTACH_FORM,
   	   	                	               NULL) ;

    	frame_widget = XtVaCreateManagedWidget ("XtasTitleFrame", xmFrameWidgetClass, fbase_form,
                                           XmNtopAttachment,    XmATTACH_FORM,
                                           XmNleftAttachment,   XmATTACH_FORM,
                                           XmNleftOffset,   2,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                   		   NULL) ;
    
    	text = XmStringCreateSimple ("Margin") ;
    	label_widget = XtVaCreateManagedWidget ("XtasMainTitles", xmLabelWidgetClass, frame_widget,
    									   XmNlabelString,      text,
										   NULL) ;
    	XmStringFree (text) ;

    	XtasSelectMarginField = XtVaCreateManagedWidget ("XtasUserField", xmTextWidgetClass, fbase_form,
                                           XmNtopAttachment,    XmATTACH_WIDGET,
                                           XmNtopWidget,        frame_widget,            
                                           XmNleftAttachment,   XmATTACH_FORM,         
                                           XmNleftOffset,       50,
										   XmNwidth,            175,
										   NULL) ;           

		sprintf (buf, "%d", (int)(XtasMargin/TTV_UNIT)) ;
    	XmTextSetString (XtasSelectMarginField, buf) ;
		
	} else {
		if (XtasDebugSigName)
			XtasSetDebugSignal (XtasDebugSigName) ;
		else 
			XtasSetDebugSignal (NULL) ;
		sprintf (buf, "%d", (int)(XtasMargin/TTV_UNIT)) ;
    	XmTextSetString (XtasSelectMarginField, buf) ;
	}

  	XtManageChild (XtasSelectDebugWidget) ;
  //	XalLimitedLoop(XtasSelectDebugWidget) ;
}
