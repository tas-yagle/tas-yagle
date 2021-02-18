/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getdelay.c                                             */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
/*                                                                          */
/*    Modified by :Mathieu OKUYAMA                    Date : 06/01/1998     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"
#include "xtas_getdelay_menu.h"

#define   XTAS_DELAY_LOFFSET    6

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Widget XtasGetDelayWidget = NULL ;
static XtasSearchPathParamStruct *XtasDelay ;
static Widget XTAS_GETDELAY_FIELD[7];
static char   UPUP ;
static char   UPDN ;
static char   DNUP ;
static char   DNDN ;
static char   DELAY ;
static char   MAX ;

/*---------------------------------------------------------------------------*/
void XtasDelayUpupCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 UPUP = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayUpdnCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 UPDN = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayDndnCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DNDN = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayDnupCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DNUP = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayMinCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 MAX = ( state->set ) ? 'N' : 'Y' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayMaxCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 MAX = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayRCCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DELAY = ( state->set ) ? 'R' : 'G' ;
}

/*---------------------------------------------------------------------------*/
void XtasDelayGateCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DELAY = ( state->set ) ? 'G' : 'R' ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStartEndDelayCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .key : The command mnemonic.                                   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStartEndDelayCallback( widget, key, call_data )
Widget  widget;
XtPointer key;
XtPointer call_data;
{
 XtasSignalsTopWidget =  XtParent(XtasGetDelayWidget) ;
 XtasDeskSignalsViewCallback( widget, key, call_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSetDelayByStartOrEnd                                       */
/*                                                                           */
/* IN  ARGS : .key : Indicates which widget we are working on.               */
/*            .name : The signal name.                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This function is called from the signals window in order to    */
/*            help the user choosing the from & to paths edges.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasSetDelayByStartOrEnd( key, name )
int key;
char *name;
{
 Widget from_or_to;
 char *text ;

 if ( (key >= 40) && (key <= 40+XTAS_C_NBSIGTYPE-1) )
   from_or_to = XTAS_GETDELAY_FIELD[0];
 else if ( (key >= 50) && (key <= 50+XTAS_C_NBSIGTYPE-1) )
   from_or_to = XTAS_GETDELAY_FIELD[1];

 if(name != NULL)
  {
   text = XmTextGetString(from_or_to);
   XtasSuppresStringSpace(text) ;
   if(strcmp(text,"*") == 0)
     XmTextSetString(from_or_to,"");
   else if(text[0] != '\0')
    XmTextInsert(from_or_to, 0, " ");
   XtFree(text) ;
  }

 XmTextInsert(from_or_to, 0, name);
 return(from_or_to);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetDelayOkCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .parent : The parent widget.                                   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Get Delay on OK Button.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetDelayOkCallback(widget,client_data,call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
 Widget parent = (Widget) client_data ;
 char *tmp[8];
 char unvalid_instance_mask = 0;
 int i, j;

/*------- check for text fields ------------------------*/

 for(i = 0 ; i<7 ; i++)
   tmp[i] = XmTextGetString( XTAS_GETDELAY_FIELD[i] );
  
 for(i = 2 ; i < 5 ; i++)
  {
   for(j = 0 ; j < (int)strlen(tmp[i]) ; j++)
    {
     if(!isdigit((int)tmp[i][j]))
      {
       for( i=0; i<7; i++ ) 
        { 
         if(tmp[i]) 
         XtFree( tmp[i] ); 
        }
       XtManageChild( parent ); 
       XalDrawMessage( XtasErrorWidget, XTAS_NDIGERR ); 
       return;
      }
    }
  }

 XtasSuppresStringSpace(tmp[6]);   
 XtasSuppresStringSpace(tmp[5]);

 if ( (strlen(tmp[5]) == 0) || (strlen(tmp[6]) == 0)
     || (!strcmp(tmp[5], "AllLevel")) || (!strcmp(tmp[6], "AllLevel")) )
  {
    XtasDelay->ROOT_SIG_EDGE->LOCAL_FIG = NULL ; 
  }
 else
  { 
   XtasDelay->ROOT_SIG_EDGE->LOCAL_FIG = ttv_getttvins(
                               XtasDelay->ROOT_SIG_EDGE->TOP_FIG,tmp[5],tmp[6]);
   unvalid_instance_mask = (XtasDelay->ROOT_SIG_EDGE->LOCAL_FIG == NULL) ;
  }
 
  if ( DELAY == 'R' )
   {
    XtasDelay->SCAN_TYPE |= TTV_FIND_RC;
    XtasDelay->SCAN_TYPE &= ~(TTV_FIND_GATE);
   }
  else
   {
    XtasDelay->SCAN_TYPE |= TTV_FIND_GATE;
    XtasDelay->SCAN_TYPE &= ~(TTV_FIND_RC);
   }
  
  XtasDelay->CRITIC = 'D' ;
  
  if ( MAX  == 'Y' )
    {
      XtasDelay->SCAN_TYPE &= ~(TTV_FIND_MIN);
      XtasDelay->SCAN_TYPE |= TTV_FIND_MAX;
    }
  else
    { 
      XtasDelay->SCAN_TYPE &= ~(TTV_FIND_MAX);
      XtasDelay->SCAN_TYPE |= TTV_FIND_MIN;
    }
  
  if((strlen(tmp[0]) == 0) || (strcmp(tmp[0],"*") == 0))
    {
     if(XtasDelay->ROOT_SIG_EDGE->MASK != NULL)
      {
       freechain(XtasDelay->ROOT_SIG_EDGE->MASK) ;
       XtasDelay->ROOT_SIG_EDGE->MASK = NULL ;
      }
    }
  else
    {
     if(XtasDelay->ROOT_SIG_EDGE->MASK != NULL)
      {
       freechain(XtasDelay->ROOT_SIG_EDGE->MASK) ;
      }
     XtasDelay->ROOT_SIG_EDGE->MASK = XtasGetMask( XTAS_GETDELAY_FIELD[0] ); 
    }
      
  if((strlen(tmp[1]) == 0) || (strcmp(tmp[1],"*") == 0))
   {
    if(XtasDelay->NODE_MASK != NULL)
     {
      freechain(XtasDelay->NODE_MASK) ;
      XtasDelay->NODE_MASK        = NULL ;
     }
   }
  else
   {
    if(XtasDelay->NODE_MASK != NULL)
     {
      freechain(XtasDelay->NODE_MASK) ;
     }
    XtasDelay->NODE_MASK        = XtasGetMask( XTAS_GETDELAY_FIELD[1] ); 
   }
  
 if (strlen(tmp[2]) == 0) 
    XtasDelay->PATH_ITEM            = XTAS_DEFAULT_SIGITEMS ;
 else
    XtasDelay->PATH_ITEM            = atoi( tmp[2] ); 
  
 if(strlen(tmp[3]) == 0)
    XtasDelay->DELAY_MIN            = TTV_DELAY_MIN;
 else
    XtasDelay->DELAY_MIN            = atol( tmp[3] );
  
 if(strlen(tmp[4]) == 0)
    XtasDelay->DELAY_MAX            = TTV_DELAY_MAX;
 else
    XtasDelay->DELAY_MAX            = atol( tmp[4] );
  
 if(UPUP == 'Y')
    {
      XtasDelay->SCAN_TYPE &= ~TTV_FIND_NOT_UPUP ;  
    }
 else
    {
      XtasDelay->SCAN_TYPE |= TTV_FIND_NOT_UPUP ;
    }

 if(DNUP == 'Y')
   {
     XtasDelay->SCAN_TYPE &= ~TTV_FIND_NOT_DWUP ;
   }
 else
    {
      XtasDelay->SCAN_TYPE |= TTV_FIND_NOT_DWUP ;
    }

 if(UPDN == 'Y')
    {   
      XtasDelay->SCAN_TYPE &= ~TTV_FIND_NOT_UPDW ;
    }
 else
   {
     XtasDelay->SCAN_TYPE |= TTV_FIND_NOT_UPDW ;
   }  

 if(DNDN == 'Y')
    {
      XtasDelay->SCAN_TYPE &= ~TTV_FIND_NOT_DWDW ;
    }
 else
   {
    XtasDelay->SCAN_TYPE |= TTV_FIND_NOT_DWDW ;
   }

 for(i=0; i < 7; i++)
   if(tmp[i])
    XtFree(tmp[i]) ;
  
// XalLeaveLimitedLoop();
 XtasSetLabelString( XtasDeskMessageField, XTAS_PATINFO );
 XalSetCursor( XtasTopLevel, WAIT ); 
 XalForceUpdate( XtasTopLevel );
  
 if((unvalid_instance_mask) || (XtasDelayList(XtasTopLevel,XtasDelay) == NULL))
    {
      XalDrawMessage( XtasWarningWidget , XTAS_NPATWAR );
    }
  
 XtasSetLabelString( XtasDeskMessageField, XTAS_NULINFO );
 XalSetCursor( XtasTopLevel, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCancelGetDelayCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Get Delay on OK Button.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCancelGetDelayCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
//  XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetDelayCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Tools command                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetDelayCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
  int      n, i;
  Arg      args[10];
  XmString text;
  Widget   form, form_t, dbase_form, ebase_form, fbase_form;
  Widget   row_widget, frame_widget, label_widget, toggle;
  Widget   tmp_widget;
  static XtasHierStruct hier ;
  Atom     WM_DELETE_WINDOW;

  
  if ( XtasAppNotReady() ) return;
  
  if ( !XtasGetDelayWidget )
    {
      
/*--- creation d'un Prompt Dialog ---- generalites ----------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtitle, XTAS_NAME" : Get Delay..." ); n++;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
    XtasGetDelayWidget = XmCreatePromptDialog( XtasDeskMainForm, 
					     "XtasInfosBox", 
					     args, n);
    XtUnmanageChild(XmSelectionBoxGetChild(XtasGetDelayWidget, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(XtasGetDelayWidget, XmDIALOG_PROMPT_LABEL));
    XtAddCallback( XtasGetDelayWidget, XmNokCallback, XtasGetDelayOkCallback, (XtPointer)XtasGetDelayWidget );
    XtAddCallback( XtasGetDelayWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_SRC_GETDELAY|XTAS_HELP_MAIN));
    XtAddCallback( XtasGetDelayWidget, XmNcancelCallback, XtasCancelGetDelayCallback, (XtPointer)XtasGetDelayWidget );
    XmAddTabGroup( XtasGetDelayWidget ); 
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(XtParent(XtasGetDelayWidget), WM_DELETE_WINDOW, XalLeaveBox, (XtPointer )XtasGetDelayWidget);
    
    n = 0;
    form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, XtasGetDelayWidget, args, n );
    
/*-------------------------------  1 ere sub form -------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );

    form_t = dbase_form;
    n = 0; 
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );
    
/*------------------------- Signals Bounds ------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    75                ); n++;
    
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       2     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );
    n = 0;
    text = XmStringCreateSimple( "Signals Bounds" );
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );
    
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,         frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
    
    text = XmStringCreateSimple( "Start" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            50                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         83                ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETDELAY_FIELD[0] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[0] );
    XmTextSetString( XTAS_GETDELAY_FIELD[0], "*" );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETDELAY_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETDELAY_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETDELAY_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tmp_widget = XmCreateMenuBar( fbase_form, "XtasMenuBar", args, n );

    for (i=0; i< XTAS_C_NBSIGTYPE; i++)
        XtasGetDelayMenuView[i].callback_data += 40;

    XalBuildMenus( tmp_widget, XtasGetDelayStartMenu );
    XtManageChild( tmp_widget );

    text = XmStringCreateSimple( "End" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5                 ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            50                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         83                ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETDELAY_FIELD[1] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[1] );
    XmTextSetString( XTAS_GETDELAY_FIELD[1], "*" );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETDELAY_FIELD[1]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETDELAY_FIELD[1]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETDELAY_FIELD[1]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tmp_widget = XmCreateMenuBar( fbase_form, "XtasMenuBar", args, n );

    for (i=0; i< XTAS_C_NBSIGTYPE; i++)
        XtasGetDelayMenuView[i].callback_data += 10;

    XalBuildMenus( tmp_widget, XtasGetDelayEndMenu );
    XtManageChild( tmp_widget );
/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,    22   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset, 2   ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n );

/*------------------------ Blocks Items Size --------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

    text = XmStringCreateSimple( "Pages Items Size" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,      2     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );

    text = XmStringCreateSimple( "Delay" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            50                ); n++;
/*
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ); n++;
*/
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, ebase_form, args, n );
    XmStringFree( text );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,           label_widget   ); n++;
    XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,        label_widget   ); n++;
    XtSetArg( args[n], XmNleftAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,        label_widget   ); n++;
    XtSetArg( args[n], XmNrightAttachment,     XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,         2                 ); n++;
    XtSetArg( args[n], XmNleftOffset,          2                 ); n++;
    XtSetArg( args[n], XmNshadowThickness,     2                 ); n++;
    XTAS_GETDELAY_FIELD[2] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, ebase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[2] );
    XmTextSetString( XTAS_GETDELAY_FIELD[2], "10" );
    
/*----------------- 2 eme sub form -------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        form_t       ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
    form_t = dbase_form;
    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

/*------------------------------------------- Slopes mask -------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    45                ); n++; 
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Slopes Mask" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,   2    ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, fbase_form, args, n );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       frame_widget,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      20,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNbottomAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      2,
                                           XmNorientation,     XmHORIZONTAL,
                                           XmNisAligned,       True,
                                           NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "Rise Rise" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayUpupCallback, &UPUP );
    UPUP = 'Y';

    text = XmStringCreateSimple( "Rise Fall" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayUpdnCallback, &UPDN );
    UPDN = 'Y';

    text = XmStringCreateSimple( "Fall Fall" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayDndnCallback, &DNDN );
    DNDN = 'Y';

    text = XmStringCreateSimple( "Fall Rise" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayDnupCallback, &DNUP );
    DNUP = 'Y';

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,    25   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset, 5   ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    fbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n );

 /*------------------------------------------ Time sorting -------------------------------------------*/ 
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    75                ); n++; 
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Max/Min" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, fbase_form, args, n );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       frame_widget,
                                            XmNbottomAttachment, XmATTACH_FORM,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      15,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "Max" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayMaxCallback, &MAX );

    text = XmStringCreateSimple( "Min" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayMinCallback, &MAX );
    MAX = 'Y';

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,    25   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset, 5   ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    fbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n );


 /*------------------------------------------ Search Type -------------------------------------------*/ 
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Order by" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,  2     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, fbase_form, args, n );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       frame_widget,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      15,
                                           XmNrightAttachment, XmATTACH_FORM,
                                            XmNbottomAttachment,   XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "Gate" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayGateCallback, &DELAY );

    text = XmStringCreateSimple( "RC" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDelayRCCallback, &DELAY );
    DELAY = 'G';

/*-------------- 3 eme sub form ------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        form_t        ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
    form_t = dbase_form;
    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       2     ); n++;
    XtSetArg( args[n], XmNrightOffset,      2     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, dbase_form, args, n );
    n = 0;
    text = XmStringCreateSimple( "Time Bounds" );
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );



    text = XmStringCreateSimple( "Lower Bound" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            110               ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, dbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             tmp_widget     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,          tmp_widget     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            tmp_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         40                ); n++; 
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETDELAY_FIELD[3] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, dbase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[3] );
    XmTextSetString( XTAS_GETDELAY_FIELD[3] , "" );
 
    text = XmStringCreateSimple( "Upper Bound" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text                  ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETDELAY_FIELD[3]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETDELAY_FIELD[3]     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETDELAY_FIELD[3]      ); n++;
    XtSetArg( args[n], XmNheight,           35                    ); n++;
    XtSetArg( args[n], XmNwidth,            110                   ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, dbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET  ); n++;
    XtSetArg( args[n], XmNtopWidget,          tmp_widget  ); n++;
    XtSetArg( args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,       tmp_widget     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            tmp_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,                         80); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETDELAY_FIELD[4]  = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, dbase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[4] );
    XmTextSetString( XTAS_GETDELAY_FIELD[4] , "" );
    
/*-------------- 4 eme sub form ------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        form_t            ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
    form_t = dbase_form;
    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ); 
  /*  
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,   2                     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Search Level" );
    n = 0 ;
     XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,         frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Figure Name" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrecomputeSize,     False              ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            100                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         40 ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;

    XTAS_GETDELAY_FIELD[5] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[5] );
    XmTextSetString( XTAS_GETDELAY_FIELD[5], "" );

    text = XmStringCreateSimple( "Instance Name" );
    n = 0;

    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNrecomputeSize,     False              ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETDELAY_FIELD[5]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETDELAY_FIELD[5]     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETDELAY_FIELD[5]      ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            100               ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );


    n = 0;
    XtSetArg( args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET  ); n++;
    XtSetArg( args[n], XmNtopWidget,          label_widget  ); n++;
    XtSetArg( args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,       label_widget     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,                         80); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;

    XTAS_GETDELAY_FIELD[6] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETDELAY_FIELD[6] );
    XmTextSetString( XTAS_GETDELAY_FIELD[6], "" );

    text = XmStringCreateSimple( "List" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETDELAY_FIELD[6]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETDELAY_FIELD[6]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETDELAY_FIELD[6]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles",xmPushButtonWidgetClass,fbase_form, args, n );
    XmStringFree( text );

    XtasDelay = XtasNewSearchPathParam( NULL );
    /* figure hieararchy */
    hier.instance = XTAS_GETDELAY_FIELD[6] ;  
    hier.figure   = XTAS_GETDELAY_FIELD[5] ;
    hier.param = XtasDelay->ROOT_SIG_EDGE ;
    XtAddCallback(tmp_widget, XmNactivateCallback , 
                  XtasHierarchicalViewCallback, &hier) ; 
   }

  XtasDelay->ROOT_SIG_EDGE->TOP_FIG = XtasMainParam->ttvfig ;
  XtasDelay->ROOT_SIG_EDGE->LOCAL_FIG = NULL ;

  XtManageChild( XtasGetDelayWidget );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDelayColumnsTitleMoveWithHSBCallback                       */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : The shell widget id.                                           */
/*                                                                           */
/* OBJECT   : The main routine of Detail List Main window command            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDelayColumnsTitleMoveWithHSBCallback (hsb, label, call_data)
    Widget  hsb;
    XtPointer   label;
    XtPointer   call_data;
{
    int         loffset = XTAS_DELAY_LOFFSET;
    int         value;

    XtVaGetValues (hsb, XmNvalue, &value, NULL);

    XtVaSetValues (label, XmNleftOffset, (loffset - value), NULL);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDelayList                                                  */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : The shell widget id.                                           */
/*                                                                           */
/* OBJECT   : The main routine of Detail List Main window command            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasDelayList(parent,param)
Widget             parent;
XtasSearchPathParamStruct *param ;
{
 XtasPathListSetStruct path_set ;
 
 path_set.PARAM = param ;
 
 return(XtasPathDetailList( parent, &path_set )) ;
}

/*---------------------------------------------------------------------------*/
/* FUNCTION : XtasCreate2SubForm2                                            */
/*                                                                           */
/* Create the 2 subforms and display the infos on 2 signals named buf1 & buf2*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCreate2SubForm2(detail_set,buf1,buf2)
XtasDetailPathSetStruct *detail_set;
char  *buf1 ;
char  *buf2 ;
{
 Widget         paned_window ;
 Widget         w1,w2 ;
 static Widget  last_paned_widget ;
 static XtasDetailPathSetStruct* lastXtasDetPathSet ;
 Arg            args[10];
 int            n ;

    if (( last_paned_widget ) && ( lastXtasDetPathSet == detail_set ))
        XtDestroyWidget ( last_paned_widget ) ;
    n=0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    paned_window = XtCreateManagedWidget( "XtasPanedWindow",
                                           xmPanedWindowWidgetClass,
                                           detail_set->INFO_PAGE, 
                                           args, n );
    last_paned_widget = paned_window ;

    n=0;
    XtSetArg( args[n], XmNheight, 130) ; n++ ;
    w1 = XtCreateManagedWidget (  "XtasNewDisp",
                                       xmFormWidgetClass,
                                       paned_window,
                                       args, n );
    n=0;
    XtSetArg( args[n], XmNheight, 130) ; n++ ;
    w2 = XtCreateManagedWidget (  "XtasNewDisp",
                                       xmFormWidgetClass,
                                       paned_window,
                                       args, n );
    XtasFillInfosForm( w1, buf1 ) ;
    XtasFillInfosForm( w2, buf2 ) ;

    lastXtasDetPathSet = detail_set;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGet2InfoCallback2                                          */
/*                                                                           */
/* Displays the infos of the begining and the end of a path                  */
/* selected in the scrolled list                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasGet2InfoCallback2 (widget, client_data , call_data)
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
    XtasDetailPathSetStruct *detail_set = (XtasDetailPathSetStruct *)client_data ;
    XmListCallbackStruct  *cbs ;
    ttvcritic_list        *critic1;
    ttvcritic_list        *critic2;
    chain_list            *head;
    int                    i;

    cbs  = (XmListCallbackStruct*)call_data ;
    head = detail_set->CUR_DETAIL->DATA;
    XTAS_DISABLE_INFO = 'Y' ;

    i    = 1 ;
    while (head && (cbs) && (i < cbs->item_position)) {
        head = head->NEXT ;
        i++ ;
    }
    if ( head ) {
        critic1 = head->DATA ;
        critic2 = critic1->NEXT ;
        XtasSetDetailInfo(detail_set,critic1) ;
    }
    if ( critic1 && critic2 )
        XtasCreate2SubForm2(detail_set,critic1->NAME,critic2->NAME) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* New display of the delay list                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillDelayList(detail_set)
XtasDetailPathSetStruct *detail_set;
{
 int     size;
 chain_list *head;
 ttvcritic_list *critic;
 
 char       text_line[2048] ;
 char       buf[1024] ;
 char       *tp, *capa, *slope ;
 char       type_rc_gate[1024] ;
 char       type_front1[1024] ;
 char       type_front2[1024] ;
 char       *signame1 ;
 char       *signame2 ;
 int        i ;
 XmString   text ;
 Widget     list_widget, scrollW, tmp ;
 Widget     label_widget ;
 int        n ;
 Arg        args[20] ;
 int        lengthsigref1 ;
 int        lengthsigref2 ;
 int        lengthsig1 ;
 int        lengthsig2 ;
 static Widget form_widget, THE_label_widget;
 static XtasDetailPathSetStruct *lastpath;

 if ( form_widget && lastpath == detail_set)
     XtDestroyWidget ( form_widget ) ;
 n = 0;
 XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
 XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
 XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
 XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
 form_widget= XtCreateManagedWidget( "XtasSubForm", 
                                      xmFormWidgetClass,
                                      detail_set->SCROLL_DETAIL,
                                      args, n );
 
 head = detail_set->CUR_DETAIL->DATA;
 
 /* determination des longueurs max des noms des signaux */
 lengthsigref1 = XTAS_MIN_CARLENGTH;
 lengthsigref2 = XTAS_MIN_CARLENGTH;

 size = 0 ;
 while((head != NULL) && (size++ < detail_set->CUR_DETAIL->SIZE))
 {
        critic = head->DATA;
        while ( critic != NULL )
        {
            lengthsig1 = strlen(critic->NAME) ;
            critic = critic->NEXT ;
            lengthsig2 = strlen(critic->NAME) ;
            if (lengthsig1 >= lengthsigref1)
                lengthsigref1 = lengthsig1 ;
            if (lengthsig2 >= lengthsigref2)
                lengthsigref2 = lengthsig2 ;
            critic = critic->NEXT ;
        }
        head = head->NEXT ;
 }
 if(lengthsigref1 >= XTAS_MAX_CARLENGTH)
     lengthsigref1 = XTAS_MAX_CARLENGTH;
 if(lengthsigref2 >= XTAS_MAX_CARLENGTH)
     lengthsigref2 = XTAS_MAX_CARLENGTH;

 /* Affiche les labels au dessus de la scroll list       */

text = XmStringCreateSimple( "From" );

label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, 
                                        form_widget,
                                        XmNlabelString,     text,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_FORM,
                                        XmNwidth,           lengthsigref1*7,
                                        XmNtopOffset,       2,
                                        XmNrightOffset,     2,
                                        XmNleftOffset,      XTAS_DELAY_LOFFSET,
                                        XmNheight,          25,
                                        NULL);
XmStringFree( text );
THE_label_widget = label_widget;

 text = XmStringCreateSimple( "To" );
 label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, 
                                        form_widget,
                                        XmNlabelString,     text,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_WIDGET,
                                        XmNleftWidget,      label_widget,
                                        XmNwidth,           lengthsigref2*7,
                                        XmNtopOffset,       2,
                                        XmNrightOffset,     2,
                                        XmNleftOffset,      90,
                                        XmNheight,          25,
                                        NULL);
 XmStringFree( text );
text = XmStringCreateSimple( "Capa" );
label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, 
                                        form_widget,
                                        XmNlabelString,     text,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_WIDGET,
                                        XmNleftWidget,      label_widget,
                                        XmNwidth,           80,
                                        XmNtopOffset,       2,
                                        XmNrightOffset,     2,
                                        XmNleftOffset,      10,
                                        XmNheight,          25,
                                        NULL);
XmStringFree( text );

text = XmStringCreateSimple( "Output Slope" );
label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass,
                                        form_widget,
                                        XmNlabelString,     text,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_WIDGET,
                                        XmNleftWidget,      label_widget,
                                        XmNwidth,            80,
                                        XmNtopOffset,       2,
                                        XmNrightOffset,     2,
                                        XmNleftOffset,      2,
                                        XmNheight,          25,
                                        NULL);
XmStringFree( text );

text = XmStringCreateSimple( "Tp" );
label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass,
                                        form_widget,
                                        XmNlabelString,     text,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_WIDGET,
                                        XmNleftWidget,      label_widget,
                                        XmNwidth,           100,
                                        XmNtopOffset,       2,
                                        XmNrightOffset,     2,
                                        XmNleftOffset,      10,
                                        XmNheight,          25,
                                        NULL);
XmStringFree( text );

 /*   Creation  de la scroll list                        */
 n=0;
 XtSetArg( args[n], XmNtopAttachment,          XmATTACH_WIDGET      ); n++;
 XtSetArg( args[n], XmNtopWidget    ,          label_widget         ); n++;
 XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
 XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
 XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
 list_widget = XmCreateScrolledList ( form_widget, "XtasTextualDisplay", args, n );
 XmListDeleteAllItems (list_widget) ; 
 XtManageChild( list_widget );

 /* Remplissage de la scroll list                        */
 head = detail_set->CUR_DETAIL->DATA;
 size = 0 ;
 i = 1 ;
  while((head != NULL) && (size++ < detail_set->CUR_DETAIL->SIZE))
  {
    critic = head->DATA;
    while ( critic != NULL )
       {
        /*--------  first signal   -----------------------------*/
        /*- Type de front -*/
        if(critic->SNODE == TTV_UP)
            sprintf(type_front1," R") ;
        else
            sprintf(type_front1," F") ;
    
        /*- Type de front -*/
        sprintf(buf,"%s",critic->NAME );
        signame1 = XtasPlaceString(buf,lengthsigref1) ;
        
        critic = critic->NEXT;
        /*--------  second signal   -----------------------------*/
        /*- Type de front -*/
        if(critic->SNODE == TTV_UP)
            sprintf(type_front2,"R ") ;
        else
            sprintf(type_front2,"F ") ;
    
        /*- Type de front -*/
        sprintf(buf,"%s",critic->NAME );
        signame2 = XtasPlaceString(buf,lengthsigref2) ;
        
        /*- type : RC or GATE -*/
        if(critic != head->DATA) {
           if((critic->TYPE & TTV_FIND_RC) != TTV_FIND_RC)
            sprintf(type_rc_gate," GT "); 
           else
            sprintf(type_rc_gate," RC ");
        }
        else
            sprintf(type_rc_gate,"    ");

        /*- Capacite -*/
        capa = XtasPlaceFloatString(critic->CAPA/1000.0) ; /* pF */
    
        /*- Tp -*/
        tp = XtasPlaceNbStringTTVUnit(critic->DELAY,TTV_UNIT) ;
    
        /*- Slope -*/
        slope = XtasPlaceNbStringTTVUnit(critic->SLOPE,TTV_UNIT) ;
    
    
        sprintf(text_line,"%s %s  %s  %s %s %s pF %s pS %s pS  %d", signame1, 
                type_front1, type_rc_gate, type_front2, signame2, capa, slope, tp, i) ;
        mbkfree(signame1) ;
        mbkfree(signame2) ;
        mbkfree(capa) ;
        mbkfree(tp) ;
        mbkfree(slope) ;
    
        text = XmStringCreateSimple (text_line) ;
        XmListAddItemUnselected ( list_widget, text, i) ;
        XmStringFree (text) ;
        
        i++ ;
        critic = critic->NEXT;
       }
   head = head->NEXT ;
  }
  
  /*--------- List des call back sur la scroll list */
   XtAddCallback (list_widget,
                  XmNbrowseSelectionCallback,
                  XtasGet2InfoCallback2,
                  (XtPointer)detail_set);
   
   scrollW = XtParent (list_widget); /* on recupere la ScrolledWindow associee a la ScrolledList */
   XtVaGetValues (scrollW, XmNhorizontalScrollBar, &tmp, NULL);
   XtAddCallback (tmp, XmNvalueChangedCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNdragCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNincrementCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNdecrementCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNpageIncrementCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNpageDecrementCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNtoTopCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
   XtAddCallback (tmp, XmNtoBottomCallback, XtasDelayColumnsTitleMoveWithHSBCallback, THE_label_widget);
 
   lastpath = detail_set ;
}
