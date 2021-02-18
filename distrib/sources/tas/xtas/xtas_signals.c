/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_signals.c                                              */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
/*                                                                          */
/*    Modified by : Stephane Picault                  Date : 10/13/1998     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"
#include "xtas_signals_menu.h"
#include "xtas_signals_button.h"

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
static char            *Xtascurrent_signal[XTAS_C_NBSIGTYPE];
static int             Xtassigs_state;
static Widget          XtasSignalsTopLevel[XTAS_C_NBSIGTYPE];
static Widget          XtasSignalsWorkWidget[XTAS_C_NBSIGTYPE];
static Widget          Xtaslist_widget[XTAS_C_NBSIGTYPE];
static Widget          Xtasbuttons_widget[XTAS_C_NBSIGTYPE];
static Widget          XtasSignalsMainWindow, XtasSignalsMenuWidget;
static Widget          Xtasbuttons_form;
       Widget          XtasSignalsTopWidget = NULL ;
static XtasSigListSetStruct  *XtasSignalSet[XTAS_C_NBSIGTYPE] ;
static XtasHierStruct  XtasSignalHier[XTAS_C_NBSIGTYPE] ;

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
void XtasSignalsSaveCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasNotYetCallback( widget, client_data , call_data );
}

/*---------------------------------------------------------------------------*/
void XtasFileCloseCallback( widget, type, call_data )
Widget  widget;
XtPointer type;
XtPointer call_data;
{
XtPopdown( XtasSignalsTopLevel[ (int)(long)type ] );
}


/*----------------------------------------------------------------------------*/
void XtasSignalsSetState( new_state )
int new_state;
{
 Xtassigs_state = new_state;
}

/*----------------------------------------------------------------------------*/
void XtasInitializeSignalsWindow()
{
int i;

for ( i=0; i<XTAS_C_NBSIGTYPE; i++ )
    {
    XtasSignalsTopLevel[i] = NULL ;
    }
}

/*----------------------------------------------------------------------------*/
void XtasSigsRemove()
{
 int i;

 for ( i=0; i<XTAS_C_NBSIGTYPE; i++ )
   {
    if ( XtasSignalsTopLevel[i] != NULL  )
      {
       XtDestroyWidget( XtasSignalsTopLevel[i] );
       XtasFreeSigListSet(XtasSignalSet[i]) ;
      }
    XtasSignalsTopLevel[i] = NULL ;
   }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsFocusCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client : The signal type.                                     */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsFocusCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
 XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data ;
 int client = (int)(long)client_data ;
 int i ;
 char *item;

   XmStringGetLtoR( cbs->item, XmSTRING_DEFAULT_CHARSET, &item );

   for ( i=0; (i<(int)strlen(item)-1) && item[i] !=']'; i++);
   
   if(item[i] ==']')
    {
     strcpy(item,item+i+1);
    }

   XtasSuppresStringSpace(item) ;

   if (cbs->reason != XmCR_BROWSE_SELECT)
   {
      if ( Xtassigs_state >= 10 )
      {
         XtasSetFromOrTo( Xtassigs_state, item );
         if(XtasSignalsTopWidget == NULL)
           XtasMainPopupCallback(widget,NULL,NULL);
         else
           XtPopup(XtasSignalsTopWidget, XtGrabNone ) ;
         XtasSignalsTopWidget = NULL ;
         XtPopdown( XtasSignalsTopLevel[client] );
      }
    else
      {
         XalSetCursor( XtasSignalsTopLevel[client], WAIT ); 
         XalForceUpdate( XtasSignalsTopLevel[client] );
         XtasSignalsDetail( XtasSignalsWorkWidget[client], item );
         XalSetCursor( XtasSignalsTopLevel[client], NORMAL ); 
      }
      XtFree( item );
   }
   else
   {
      if ( Xtascurrent_signal[client] != NULL )
      {
         XtFree( Xtascurrent_signal[client] );
      }
     Xtascurrent_signal[client] = item;
   }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsTreatNextCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .signal_type : Signals type drawn in this window.              */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of a user ask for a list of signals.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillSigList( signalset )

 XtasSigListSetStruct *signalset;

{
   int           position = 0;
   chain_list   *head;
   XmString      motif_string;
   long          type;
   ttvsig_list  *sig;
   char          buffer[1024];
   char          buffer2[1024];

   XmListDeleteAllItems( signalset->SIGLIST_SCROLL );

   head = signalset->CUR_SIG_WIN->DATA;

   if ( head == NULL )
   {
      XtasSetLabelString( signalset->INFO_USER, XTAS_NSIGWAR ); 
   }
   else
   {  
      XalSetCursor( signalset->TOP_LEVEL, WAIT );
      XalForceUpdate( signalset->TOP_LEVEL );
      while ( (position <= signalset->CUR_SIG_WIN->SIZE )
              &&(head != NULL) )
      {
         if (( signalset->SIGNAL->TYPE & TTV_SIG_S) != TTV_SIG_S )
         {
            sig = head->DATA;
            type = sig->TYPE;
            ttv_getsigname(signalset->SIGNAL->TOP_FIG, buffer, head->DATA);
            if((type & TTV_SIG_CQ) == TTV_SIG_CQ)
               sprintf( buffer2, "[CON CMD] %s", buffer ); 
            else
            if((type & TTV_SIG_CT) == TTV_SIG_CT)
               sprintf( buffer2, "[CON TST] %s", buffer ); 
            else 
            if((type & TTV_SIG_CZ) == TTV_SIG_CZ)
               sprintf( buffer2, "[CON  HZ] %s", buffer ); 
            else 
            if((type & TTV_SIG_CB) == TTV_SIG_CB)
               sprintf( buffer2, "[CON I/O] %s", buffer ); 
            else 
            if((type & TTV_SIG_CO) == TTV_SIG_CO)
               sprintf( buffer2, "[CON OUT] %s", buffer ); 
            else 
            if((type & TTV_SIG_CI) == TTV_SIG_CI)
               sprintf( buffer2, "[CON  IN] %s", buffer ); 
            else
            if((type & TTV_SIG_CX) == TTV_SIG_CX)
               sprintf( buffer2, "[CON  ??]  %s", buffer ); 
            else 
            if((type & TTV_SIG_C) == TTV_SIG_C)
               sprintf( buffer2, "[CON  ??]  %s", buffer ); 
            else 
            if((type & TTV_SIG_Q) == TTV_SIG_Q)
               sprintf( buffer2, "[CMD] %s", buffer ); 
            else 
            if((type & TTV_SIG_LL) == TTV_SIG_LL)
               sprintf( buffer2, "[LATCH] %s", buffer ); 
            else 
            if((type & TTV_SIG_LF) == TTV_SIG_LF)
               sprintf( buffer2, "[FLIP/FLOP] %s", buffer ); 
            else 
            if((type & TTV_SIG_L) == TTV_SIG_L)
               sprintf( buffer2, "[LATCH] %s", buffer ); 
            else 
            if((type & TTV_SIG_R) == TTV_SIG_R)
               sprintf( buffer2, "[PRE] %s", buffer ); 
            else 
            if((type & TTV_SIG_B) == TTV_SIG_B)
               sprintf( buffer2, "[BREAK] %s", buffer ); 
            else 
               sprintf( buffer2, "[?*?] %s", buffer ); 
         }
         else
            sprintf( buffer2, "[INT] %s", (char *)head->DATA); 

         motif_string = XmStringCreateLtoR( buffer2, XmSTRING_DEFAULT_CHARSET ); 
         XmListAddItemUnselected( signalset->SIGLIST_SCROLL, motif_string, ++position );
         XmStringFree( motif_string );
         head = head->NEXT;
      }
      XalSetCursor( signalset->TOP_LEVEL, NORMAL );
   }   
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsTreatNextCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .signal_type : Signals type drawn in this window.              */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of a user ask for a list of signals.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsTreatNextCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;
XtasChainJmpList  *head = signalset->CUR_SIG_WIN; 

XtasSetLabelString( signalset->INFO_USER, XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate( signalset->TOP_LEVEL );

if(head != NULL)
if ( head->NEXT != NULL )
 {
  signalset->CUR_SIG_WIN = head->NEXT;
  XtasFillSigList( signalset );
 }

XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
void XtasSignalsTreatFastFwdCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;

XtasChainJmpList  *head = signalset->CUR_SIG_WIN; 

XtasSetLabelString( signalset->INFO_USER, XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate( signalset->TOP_LEVEL );

if(head != NULL)
if ( head->NEXT != NULL )
{
 head = head->NEXT;
 if ( head->NEXT != NULL )
  {
   head = head->NEXT;
   signalset->CUR_SIG_WIN = head; 
   XtasFillSigList( signalset );
  }
 else signalset->CUR_SIG_WIN = head; 
 XtasFillSigList( signalset );
}


XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
void XtasSignalsTreatEndCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;

XtasChainJmpList  *head = signalset->CUR_SIG_WIN; 

XtasSetLabelString( signalset->INFO_USER, XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate( signalset->TOP_LEVEL );

if(head != NULL)
if ( head->NEXT != NULL )
 { 
  while ( head->NEXT != NULL )
   {
    head = head->NEXT;
   }
  signalset->CUR_SIG_WIN = head;
  XtasFillSigList( signalset );
 }

XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
void XtasSignalsTreatPrevCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;

XtasChainJmpList  *head = signalset->CUR_SIG_WIN; 

XtasSetLabelString( signalset->INFO_USER, XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate( signalset->TOP_LEVEL );

if(head != NULL)
if ( head->PREV != NULL )
 {
  signalset->CUR_SIG_WIN = head->PREV;
  XtasFillSigList( signalset );
 }

XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 

}

/*---------------------------------------------------------------------------*/
void XtasSignalsTreatFastRewCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;

XtasChainJmpList  *head = signalset->CUR_SIG_WIN; 

XtasSetLabelString( signalset->INFO_USER, XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate( signalset->TOP_LEVEL );

if(head != NULL)
if ( head->PREV != NULL )
{
 head = head->PREV;
 if ( head->PREV != NULL )
  {
   head = head->PREV;
   signalset->CUR_SIG_WIN = head; 
   XtasFillSigList( signalset );
  }
 else signalset->CUR_SIG_WIN = head; 
 XtasFillSigList( signalset );
}

XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 

}
/*---------------------------------------------------------------------------*/
void XtasSignalsTreatTopCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;

XtasChainJmpList  *head = signalset->CUR_SIG_WIN; 

XtasSetLabelString( signalset->INFO_USER, XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate( signalset->TOP_LEVEL );

if(head != NULL)
if ( head->PREV != NULL )
 { 
  while ( head->PREV != NULL )
   {
    head = head->PREV;
   }
  signalset->CUR_SIG_WIN = head;
  XtasFillSigList( signalset );
 }

XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsTreatCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .signal_type : Signals type drawn in this window.              */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Treatment of a user ask for a list of signals.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsTreatCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasSigListSetStruct *signalset = ( XtasSigListSetStruct *)client_data ;
XtasSearchSigParamStruct *param;
char                     *items;

XtasSetLabelString( signalset->INFO_USER , XTAS_SIGINFO );
XalSetCursor( signalset->TOP_LEVEL, WAIT ); 
XalForceUpdate(  signalset->TOP_LEVEL );
param = signalset->SIGNAL;

items = XmTextGetString( signalset->ITEMS );
if (strlen( items ) == 0 )
 {
  XtFree( items );
  XalDrawMessage( XtasErrorWidget, XTAS_NDIGERR);
  XtasSetLabelString( signalset->INFO_USER , XTAS_NULINFO );
  XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 
  return;
 }
signalset->SIGNAL->ITEMS = atoi(items);
XtFree( items );

freechain( param->MASK );

if ( ( param->MASK = XtasGetMask( signalset->MASK )) == NULL )
  {
   XalDrawMessage(XtasErrorWidget, XTAS_NSIGMAS );  
  } 
else 
 {
  if (  XtasTtvGetSigList( signalset ) == NULL  )
   {
     XalDrawMessage(XtasWarningWidget, XTAS_NSIGWAR );  
   }
 else
   {
     XtasFillSigList( signalset );
   }
}

XtasSetLabelString( signalset->INFO_USER , XTAS_NULINFO );
XalSetCursor( signalset->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsButtonsCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .signal_type : Signals type drawn in this window.              */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Button's command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsButtonsCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
struct XalButtonsItem *Xtasbutton_menus, *Xtasbutton_options;
int signal_type = (int)(long)client_data ;

switch (signal_type)
    {
    case XTAS_C_CONNECTOR : Xtasbutton_menus   = Xtasc_buttons;
                            Xtasbutton_options = Xtasc_options;
                            break;

    case XTAS_C_COMMANDS  : Xtasbutton_menus   = Xtaso_buttons;
                            Xtasbutton_options = Xtaso_options;
                            break;
    
    case XTAS_C_MEMORIZE  : Xtasbutton_menus   = Xtasm_buttons;
                            Xtasbutton_options = Xtasm_options;
                            break;

    case XTAS_C_PRECHARGE : Xtasbutton_menus   = Xtasp_buttons;
                            Xtasbutton_options = Xtasp_options;
                            break;

    case XTAS_C_BREAK     : Xtasbutton_menus   = Xtasb_buttons;
                            Xtasbutton_options = Xtasb_options;
                            break;

    case XTAS_C_OTHERSIGS : Xtasbutton_menus   = Xtasa_buttons;
                            Xtasbutton_options = Xtasa_options;
                            break;

    default               : Xtasbutton_menus   = Xtasa_buttons;
                            Xtasbutton_options = Xtasa_options;

    }
XtasOptionsButtonsTreat( XtasSignalsWorkWidget[signal_type], &Xtasbuttons_widget[signal_type],
                         Xtasbutton_menus, Xtasbutton_options );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsDetailCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client : The signal type.                                     */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View of signals detail           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsDetailCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
 int client = (int)(long)client_data ;
 XalSetCursor( XtasSignalsTopLevel[client], WAIT ); 
 XalForceUpdate( XtasSignalsTopLevel[client] );
 if(Xtascurrent_signal[client] != NULL)
   XtasSignalsDetail( XtasSignalsWorkWidget[client], Xtascurrent_signal[client] );
 else
    XalDrawMessage( XtasErrorWidget, XTAS_NSIGERR );
 XalSetCursor( XtasSignalsTopLevel[client], NORMAL );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCtkInfoAnswerEventCallback                                 */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client : The signal type.                                     */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View of signals detail           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasSignalsCtkInfoAnswerEventCallback (parent, client_data, call_data)
    Widget      parent ;
    XtPointer   client_data;
    XtPointer   call_data;
{
    Widget       toggle;
    int          client;
    Boolean      b;
    ttvsig_list *ptsig;
    
    client = *((int*) client_data);
    toggle = XtNameToWidget (parent, "*XtasCtkUp");
    XtVaGetValues (toggle, XmNset, &b, NULL);
    XtUnmanageChild (parent); 
    ptsig = ttv_getsig (XtasMainParam->ttvfig, Xtascurrent_signal[client]);
    if(ptsig) {
        if (b)
            XtasDisplayCtkInfo( XtasSignalsWorkWidget[client], &ptsig->NODE[1], XtasErrorWidget);
        else
            XtasDisplayCtkInfo( XtasSignalsWorkWidget[client], &ptsig->NODE[0], XtasErrorWidget);
    }
//    XalLeaveLimitedLoop();
    
}

///*---------------------------------------------------------------------------*/
///*                                                                           */
///* FUNCTION : XtasSignalsCtkCancelCallback                                   */
///*                                                                           */
///*                                                                           */
///*---------------------------------------------------------------------------*/
//void    XtasSignalsCtkCancelCallback (parent, client_data, call_data)
//    Widget      parent      ;
//    XtPointer   client_data ;
//    XtPointer   call_data   ;
//{
//    XtDestroyWidget ((Widget) client_data);
//}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsCtkInfoAskEvent                                     */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasSignalsCtkInfoAskEvent (client)
    int      client ;
{
    static  Widget      widget = NULL;
    Widget      form, row_w, toggle, label_w;
    Arg         args[15];
    int         n;
    char        str[] = "Choose an event :";
    XmString    text;
    Atom        WM_DELETE_WINDOW;
    static  int c;
    char        text_title[128];

    c = client;

    if (!widget) {
        n = 0;
        sprintf (text_title, XTAS_NAME" : Crosstalk Information - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
//        XtSetArg ( args[n], XmNtitle,           XTAS_NAME" : Crosstalk Information"); n++;
        XtSetArg ( args[n], XmNtitle, text_title); n++;
        XtSetArg ( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;
        widget = XmCreatePromptDialog (XtasSignalsWorkWidget[client], "XtasCTkaskEvent", args, n);
        XtUnmanageChild (XmSelectionBoxGetChild (widget, XmDIALOG_TEXT));
        XtUnmanageChild (XmSelectionBoxGetChild (widget, XmDIALOG_PROMPT_LABEL));
        XtUnmanageChild (XmSelectionBoxGetChild (widget, XmDIALOG_HELP_BUTTON));
        XtAddCallback   (widget, XmNokCallback, XtasSignalsCtkInfoAnswerEventCallback, (XtPointer) &c);
        XtAddCallback   (widget, XmNcancelCallback, XtasCancelCallback, (XtPointer) widget);
        WM_DELETE_WINDOW = XmInternAtom( XtDisplay(widget), "WM_DELETE_WINDOW", False );
        XmAddWMProtocolCallback( XtParent(widget), WM_DELETE_WINDOW, XtasCancelCallback, widget);
        
        n = 0;
        form = XtCreateManagedWidget ("XtasForms", xmFormWidgetClass, widget, args, n);
        
        n = 0;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNmarginHeight,        5               ); n++;
        XtSetArg ( args[n], XmNmarginWidth,         35               ); n++;
        XtSetArg ( args[n], XmNspacing,             5               ); n++;
        XtSetArg ( args[n], XmNorientation,         XmVERTICAL      ); n++;
        XtSetArg ( args[n], XmNnumColumns,          1               ); n++;
        XtSetArg ( args[n], XmNradioBehavior,       True            ); n++;
        XtSetArg ( args[n], XmNradioAlwaysOne,      True            ); n++;
        XtSetArg ( args[n], XmNisAligned,           True            ); n++;
        XtSetArg ( args[n], XmNpacking,             XmPACK_COLUMN   ); n++;
        row_w = XtCreateManagedWidget ("XtasRow", xmRowColumnWidgetClass, form, args, n);
        
        text = XmStringCreateSimple ("Up");
        n = 0;
        XtSetArg ( args[n], XmNset,         True    ); n++;
        XtSetArg ( args[n], XmNlabelString, text    ); n++;
        toggle = XtCreateManagedWidget ("XtasCtkUp", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree (text);
        
        text = XmStringCreateSimple ("Down");
        n = 0;
        XtSetArg ( args[n], XmNset,         False   ); n++;
        XtSetArg ( args[n], XmNlabelString, text    ); n++;
        toggle = XtCreateManagedWidget ("XtasCtkDown", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree (text);
    
        text = XmStringCreateSimple (str);
        n = 0;
        XtSetArg ( args[n], XmNlabelString,         text            ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_WIDGET ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        row_w           ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (text);
    }

    XtManageChild (widget);
//    XalLimitedLoop (widget);
    
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsCtkInfoCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client : The signal type.                                     */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View of signals detail           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsCtkInfoCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
 int client = (int)(long)client_data ;
 
 XalSetCursor( XtasSignalsTopLevel[client], WAIT ); 
 XalForceUpdate( XtasSignalsTopLevel[client] );
 if(Xtascurrent_signal[client] != NULL) {
     if((XTAS_CTX_LOADED & XTAS_FROM_ANALYSIS) != XTAS_FROM_ANALYSIS)
            XalDrawMessage( XtasErrorWidget, 
                    "Crosstalk analysis must have been run from XTas before");
        else if(XtasMainParam->stbfig == NULL)
            XalDrawMessage( XtasErrorWidget, 
                    "Crosstalk Informations display impossible :\nno stbfig in memory.");
        else {     
            XtasSignalsCtkInfoAskEvent (client);
        }
 }
 else
    XalDrawMessage( XtasErrorWidget, XTAS_NSIGERR );
 XalSetCursor( XtasSignalsTopLevel[client], NORMAL );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateSignalsMenus                                         */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*            .signal_type : The windows type (connectors, prech ....)       */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the signals window.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateSignalsMenus( parent, signal_type )
Widget parent;
int    signal_type;
{
Widget menu_bar;
Arg    args[20];
int    n;

/*-------------------*/
/* Create a menu_bar */
/*-------------------*/
n = 0;
menu_bar = XmCreateMenuBar( parent, "XtasMenuBar", args, n );

for(n = 0 ; XtasSignalsMenuFile[n].label != NULL ; n++)
  XtasSignalsMenuFile[n].callback_data = (XtPointer) signal_type;

switch (signal_type)
    {
    case XTAS_C_CONNECTOR : XtasSignalsMenu[1].subitems = XtasSignalsMenuViewC;
                            XtasSignalsMenu[2].subitems = XtasSignalsMenuOptC;
                            XtasSignalsMenu[3].subitems = XtasSignalsMenuHelpA;
                            break;

    case XTAS_C_COMMANDS  : XtasSignalsMenu[1].subitems = XtasSignalsMenuViewO;
                            XtasSignalsMenu[2].subitems = XtasSignalsMenuOptO;
                            XtasSignalsMenu[3].subitems = XtasSignalsMenuHelpX;
                            break;
    
    case XTAS_C_MEMORIZE  : XtasSignalsMenu[1].subitems = XtasSignalsMenuViewM;
                            XtasSignalsMenu[2].subitems = XtasSignalsMenuOptM;
                            XtasSignalsMenu[3].subitems = XtasSignalsMenuHelpX;
                            break;

    case XTAS_C_PRECHARGE : XtasSignalsMenu[1].subitems = XtasSignalsMenuViewP;
                            XtasSignalsMenu[2].subitems = XtasSignalsMenuOptP;
                            XtasSignalsMenu[3].subitems = XtasSignalsMenuHelpX;
                            break;

    case XTAS_C_BREAK     : XtasSignalsMenu[1].subitems = XtasSignalsMenuViewB;
                            XtasSignalsMenu[2].subitems = XtasSignalsMenuOptB;
                            XtasSignalsMenu[3].subitems = XtasSignalsMenuHelpX;
                            break;

    case XTAS_C_OTHERSIGS : XtasSignalsMenu[1].subitems = XtasSignalsMenuViewA;
                            XtasSignalsMenu[2].subitems = XtasSignalsMenuOptA;
                            XtasSignalsMenu[3].subitems = XtasSignalsMenuHelpA;
                            break;

    }

/*---------------------------------------------------------------------------*/
/* This function is called from the "libXal" library. It provides a simple   */
/* way to build the menu bar with a set of functions and submenus.           */
/* To use this function, you have to make up a set of XalMenus structure     */
/* as described in the XalBuildMenus manual.                                 */
/*---------------------------------------------------------------------------*/
XalBuildMenus( menu_bar,             /* The Menu Bar widget id              */
               XtasSignalsMenu );   /* The main menu XalMenus structure    */

return( menu_bar );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateSignalsList                                          */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .signal_type : The windows type (connectors, prech ....)       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation of the signals window objects.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateSignalsList( parent, signal_type , signalset )

 Widget                 parent;
 int                    signal_type;
 XtasSigListSetStruct  *signalset;

{
 GC          gc;
 Pixel       fg;
 Pixel       bg;

   struct XalButtonsItem  *buttons_menu;
          XmString         text;
          Widget           label_widget;
          Widget           form;
          Widget           sub_form;
          Widget           XtasSignalsMainForm;
          Widget           button;
          Widget           frame;
          Widget	   pan_widget;
          Widget           separator1, separator2;
          Arg              args[20];
          int              n;
          char 		 	  *default_mask =  "*"; 
          char 		 	  *default_item_number =  "20"; 

/*---------------------------------------------------------------*/
/* Create The Main Form                                          */
/*---------------------------------------------------------------*/
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_POSITION ); n++;
   XtSetArg( args[n], XmNbottomPosition,   99                ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_POSITION ); n++;
   XtSetArg( args[n], XmNleftPosition,     1                 ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
   XtSetArg( args[n], XmNrightPosition,    99                ); n++;
   XtSetArg( args[n], XmNallowResize,      False             ); n++;
   
   XtasSignalsMainForm = XtCreateWidget( "XtasMainPan",
                                          xmFormWidgetClass,
                                          parent,
                                         args, n);
         
   HelpFather = XtasSignalsMainForm ;

   /*------------------------------------------------*/
   /* Create a form to manage the first row children */
   /*------------------------------------------------*/
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM); n++;
   Xtasbuttons_form = XtCreateManagedWidget( "xtasMainButtons", 
                                              xmFormWidgetClass, 
                                              XtasSignalsMainForm,
                                             args, n );
   /*--- new widget ---*/
   text = XmStringCreateSimple( "Search" );
   n = 0;
   XtSetArg( args[n], XmNwidth,             60                  ); n++;
   XtSetArg( args[n], XmNheight,            40                  ); n++;
   XtSetArg( args[n], XmNlabelString,       text                ); n++;
   XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM       ); n++;
   XtSetArg( args[n], XmNrightOffset,       5                   ); n++;
   XtSetArg( args[n], XmNtopAttachment,     XmATTACH_FORM       ); n++;
   XtSetArg( args[n], XmNtopOffset,         2                   ); n++;
   XtSetArg( args[n], XmNpushButtonEnabled, True                ); n++;
   XtSetArg( args[n], XmNshadowType,        XmSHADOW_ETCHED_OUT ); n++;
   XtSetArg( args[n], XmNshadowThickness,   2                   ); n++;
   button = XtCreateManagedWidget( "SearchButton", 
                                    xmPushButtonWidgetClass,
                                    Xtasbuttons_form,
                                   args, n);
   XmStringFree( text );
   XtAddCallback( button, XmNactivateCallback, 
                  XtasSignalsTreatCallback, (XtPointer)signalset );
   XtManageChild(button);

switch (signal_type)
    {
    case XTAS_C_CONNECTOR : buttons_menu = Xtasc_buttons; break;
    case XTAS_C_COMMANDS  : buttons_menu = Xtaso_buttons; break;
    case XTAS_C_MEMORIZE  : buttons_menu = Xtasm_buttons; break;
    case XTAS_C_PRECHARGE : buttons_menu = Xtasp_buttons; break;
    case XTAS_C_BREAK     : buttons_menu = Xtasb_buttons; break;
    case XTAS_C_OTHERSIGS : buttons_menu = Xtasa_buttons; break;
    default : buttons_menu = Xtasa_buttons; break;
    }

Xtascurrent_signal[signal_type] = NULL;

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNnumColumns,     1               ); n++;
XtSetArg( args[n], XmNorientation,    XmHORIZONTAL    ); n++;
XtSetArg( args[n], XmNtopAttachment,  XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNtopOffset,      2               ); n++;
XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNleftOffset,     5               ); n++;
Xtasbuttons_widget[signal_type] = XalButtonMenus( Xtasbuttons_form, buttons_menu, args, n, 40, 40 );
XtasAddDummyButton( Xtasbuttons_widget[signal_type] );
XtManageChild( Xtasbuttons_widget[signal_type] );
XmAddTabGroup( Xtasbuttons_widget[signal_type] );
XmAddTabGroup( button );

/*------------------------------------------------*/
/* Create a separator                             */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET            ); n++;
XtSetArg( args[n], XmNtopWidget,       Xtasbuttons_form           ); n++;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM              ); n++;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM              ); n++;
XtSetArg( args[n], XmNleftOffset,      3                          ); n++;
XtSetArg( args[n], XmNrightOffset,     3                          ); n++;
separator1 = XtCreateManagedWidget( "separator1", xmSeparatorWidgetClass, XtasSignalsMainForm, args, n );

/*------------------------------------------------*/
/* Create a form to manage the next pan  children */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET            ); n++;
XtSetArg( args[n], XmNtopWidget,       separator1 ); n++;
XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM            ); n++;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, XtasSignalsMainForm, args, n );

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrecomputeSize,     False          ); n++; 
XtSetArg( args[n], XmNleftOffset,       8               ); n++;
XtSetArg( args[n], XmNheight,           35              ); n++; 
XtSetArg( args[n], XmNwidth,            110             ); n++;
label_widget = XtCreateManagedWidget( "XtasSubTitles", 
                                       xmLabelGadgetClass, 
                                       form,
                                       args, n );

XtasSetLabelString( label_widget, "Signal Mask " );

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET  ); n++;
XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET  ); n++;
XtSetArg( args[n], XmNbottomWidget,          label_widget      ); n++;
XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
XtSetArg( args[n], XmNrightPosition,         80                ); n++;
XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, True              ); n++;
XtSetArg( args[n], XmNsensitive,             True              ); n++;
XtSetArg( args[n], XmNeditable,              True              ); n++;
signalset->MASK = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n );
XmTextSetString( signalset->MASK, default_mask );
XtAddCallback(signalset->MASK, XmNactivateCallback, XtasSignalsTreatCallback, (XtPointer)signalset );
XmAddTabGroup( signalset->MASK );


/*------------------------------------------------*/
/* Create a separator                             */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM    ); n++;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM    ); n++;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET  ); n++;
XtSetArg( args[n], XmNtopWidget,       form             ); n++;
XtSetArg( args[n], XmNleftOffset,      3                ); n++;
XtSetArg( args[n], XmNrightOffset,     3                ); n++;
separator1 = XtCreateManagedWidget( "separator12", xmSeparatorWidgetClass, XtasSignalsMainForm, args, n );

/*---------------------------------------------------------------------------*/
/* Create the message area                                                   */
/*---------------------------------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNtopOffset,              2                 ); n++;
XtSetArg( args[n], XmNleftOffset,             2                 ); n++;
XtSetArg( args[n], XmNrightOffset,            2                 ); n++;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, XtasSignalsMainForm, args, n );

label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, form,
                                         XmNtopAttachment,   XmATTACH_FORM,
                                         XmNleftAttachment,  XmATTACH_FORM,
                                         XmNleftOffset,      2,
                                         NULL);
XtasSetLabelString( label_widget, "Informations Area" );

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,         XmATTACH_WIDGET   ); n++;
XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
XtSetArg( args[n], XmNrightPosition,         48                ); n++;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
n = 0;
XtSetArg( args[n], XmNheight,                 20                       ); n++;
signalset->INFO_USER = XtCreateManagedWidget( "XtasUserInfo", xmLabelWidgetClass, frame, args, n);
XtasSetLabelString( signalset->INFO_USER, XTAS_NULINFO );

n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,        frame                    ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++; 
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNleftWidget,       frame           ); n++;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
n = 0;
XtSetArg( args[n], XmNheight,           20              ); n++;
signalset->INFO_HELP = XtCreateManagedWidget( "XtashelpInfo", xmLabelWidgetClass, frame, args, n);
XtasSetLabelString( signalset->INFO_HELP, XTAS_NULINFO );


/*------------------------------------------------*/
/* Create a separator                             */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET            ); n++;
XtSetArg( args[n], XmNbottomWidget,     form    ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM              ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ); n++;
XtSetArg( args[n], XmNleftOffset,       3                          ); n++;
XtSetArg( args[n], XmNrightOffset,      3                          ); n++;
separator2 = XtCreateManagedWidget( "separator2", xmSeparatorWidgetClass, XtasSignalsMainForm, args, n );



  /*--------------------------------------------------------------------------*/
 /*    paned window pour la scrolled list et divers parametres d'affichage   */
/*--------------------------------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET            ); n++;
XtSetArg( args[n], XmNbottomWidget,     separator2    ); n++;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET            ); n++;
XtSetArg( args[n], XmNtopWidget,        separator1    ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM              ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ); n++;
pan_widget = XtCreateManagedWidget( "XtasPanWindow", xmPanedWindowWidgetClass, XtasSignalsMainForm, args, n );


  /*---------------------------------------------------------------------*/
 /* Form contenant les parametres de recherche : mask, criteres divers  */ 
/*---------------------------------------------------------------------*/
n = 0;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, pan_widget, args, n );

   /*--- background, foreground ---*/
XtVaGetValues( pan_widget, XmNforeground, &fg, XmNbackground, &bg, NULL );
gc = XCreateGC(XtDisplay(pan_widget), RootWindowOfScreen(XtScreen(pan_widget)), 0, 0);
XSetForeground(XtDisplay(pan_widget), gc, fg);
XSetBackground(XtDisplay(pan_widget), gc, bg);
XFreeGC(XtDisplay(pan_widget),gc) ;

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,  XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNforeground, fg                ); n++;
XtSetArg( args[n], XmNbackground, bg                ); n++;
XtSetArg( args[n], XmNrecomputeSize,  False         ); n++; 
XtSetArg( args[n], XmNtopOffset,      6             ); n++;
XtSetArg( args[n], XmNleftOffset,     5             ); n++;
XtSetArg( args[n], XmNheight,         35            ); n++;
XtSetArg( args[n], XmNwidth,          110           ); n++;
label_widget = XtCreateManagedWidget( "XtasUserField", xmLabelGadgetClass, form, args, n );
XtasSetLabelString( label_widget, "Figure Name" );

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,             label_widget             ); n++;
XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNbottomWidget,          label_widget             ); n++;
XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET          ); n++;
XtSetArg( args[n], XmNleftWidget,            label_widget             ); n++;
XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION        ); n++;
XtSetArg( args[n], XmNrightPosition,         60                       ); n++;
XtSetArg( args[n], XmNshadowThickness,       2                        ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, False                    ); n++;
XtSetArg( args[n], XmNsensitive,             True                     ); n++;
XtSetArg( args[n], XmNeditable,              False                    ); n++;
XtasSignalHier[signal_type].figure = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
XmTextSetString( XtasSignalHier[signal_type].figure, signalset->SIGNAL->TOP_FIG->INFO->FIGNAME ) ;

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,      label_widget    ); n++;
XtSetArg( args[n], XmNtopOffset,      6               ); n++;
XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNforeground, fg                  ); n++;
XtSetArg( args[n], XmNbackground, bg                  ); n++;
XtSetArg( args[n], XmNleftOffset,     5               ); n++;
XtSetArg( args[n], XmNrecomputeSize,  False           ); n++;
XtSetArg( args[n], XmNheight,         35              ); n++; 
XtSetArg( args[n], XmNwidth,          110             ); n++;
label_widget = XtCreateManagedWidget( "XtasUserField", xmLabelGadgetClass, form, args, n );
XtasSetLabelString( label_widget, "Instance Name" );

   /*--- new widget ---*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,             label_widget             ); n++;
XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNbottomWidget,          label_widget             ); n++;
XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET          ); n++;
XtSetArg( args[n], XmNleftWidget,            label_widget             ); n++;
XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION        ); n++;
XtSetArg( args[n], XmNrightPosition,         60                       ); n++;
XtSetArg( args[n], XmNshadowThickness,       2                        ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, False                    ); n++;
XtSetArg( args[n], XmNsensitive,             True                     ); n++;
XtSetArg( args[n], XmNeditable,              False                    ); n++;
XtasSignalHier[signal_type].instance = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
XmTextSetString( XtasSignalHier[signal_type].instance, signalset->SIGNAL->TOP_FIG->INSNAME ) ;

button = XtVaCreateManagedWidget("dbutton", xmPushButtonWidgetClass, form,
                                 XmNpushButtonEnabled, True,
                                 XmNshadowType,        XmSHADOW_ETCHED_OUT,
                                 XmNleftAttachment,    XmATTACH_WIDGET,
				 XmNleftWidget,	       XtasSignalHier[signal_type].instance ,
                                 XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
				 XmNtopWidget,	       XtasSignalHier[signal_type].instance,
                                 XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
				 XmNbottomWidget,      XtasSignalHier[signal_type].instance,
                                 XmNshadowThickness,   2,
                                 NULL);
XtasSetLabelString( button , "List" );
XtasSignalHier[signal_type].param = signalset->SIGNAL ;
XtAddCallback(button, XmNactivateCallback,XtasHierarchicalViewCallback, (XtPointer)&XtasSignalHier[signal_type]);
XmAddTabGroup( button );



/*----------------------- champ de saisie du nombre d'items -------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
XtSetArg( args[n], XmNwidth,                 60              ); n++;
signalset->ITEMS = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
XmTextSetString( signalset->ITEMS, default_item_number );
XtAddCallback(signalset->ITEMS, XmNactivateCallback, XtasSignalsTreatCallback, (XtPointer)signalset );
XmAddTabGroup( signalset->ITEMS );

n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET  ); n++;
XtSetArg( args[n], XmNtopWidget,        signalset->ITEMS ); n++;
XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET  ); n++;
XtSetArg( args[n], XmNbottomWidget,        signalset->ITEMS ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      signalset->ITEMS ); n++;
XtSetArg( args[n], XmNwidth,            50              ); n++;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
XtasSetLabelString( label_widget, "Items :" ); 

/*---------------------------------------------------*/
/*  en tete de la scrolled list			     */
/*---------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,       button          ); n++;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNborderWidth,     0               ); n++;
frame = XtCreateManagedWidget( "XtasShellSubForm", xmFrameWidgetClass, form, args, n );

n = 0;
sub_form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, frame, args, n );

/*--------------------- titre -----------------------------------------------*/
label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                                        XmNtopAttachment,    XmATTACH_FORM,
                                        XmNbottomAttachment, XmATTACH_FORM,
                                        XmNleftAttachment,   XmATTACH_FORM,
                                        XmNrightOffset,      2,
                                        XmNleftOffset,       2,
                                        NULL);
XtasSetLabelString( label_widget, "Signals List" );

/*------------------- arrow buttons pour les sauts dans la liste ------------*/
/*--------------- end of list button ------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNwidth,            60              ); n++; 
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " ->| ");
XtAddCallback( button, XmNactivateCallback, XtasSignalsTreatEndCallback, (XtPointer)signalset );
XmAddTabGroup( button );
/*------------------ fast forward button -------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button          ); n++;
XtSetArg( args[n], XmNwidth,            60              ); n++; 
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " ->-> ");
XtAddCallback(button, XmNactivateCallback, XtasSignalsTreatFastFwdCallback, (XtPointer)signalset );
XmAddTabGroup( button );

/*----------------- next button -----------------------------------------------*/ 
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button          ); n++;
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " -> ");
XtAddCallback(button, XmNactivateCallback, XtasSignalsTreatNextCallback, (XtPointer)signalset );
XmAddTabGroup( button );
/*----------------- middle button ---------------------------------------------*/
/*
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button          ); n++;
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " ->|<- ");
XtAddCallback(button, XmNactivateCallback, XtasSignalsTreatMiddleCallback, (XtPointer)signalset );
XmAddTabGroup( button );
*/
/*----------------- prev button -----------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button          ); n++;
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " <- ");
XtAddCallback(button, XmNactivateCallback, XtasSignalsTreatPrevCallback, (XtPointer)signalset );
XmAddTabGroup( button );
/*--------------- fast review button ------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button          ); n++;
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " <-<- ");
XtAddCallback(button, XmNactivateCallback, XtasSignalsTreatFastRewCallback, (XtPointer)signalset );
XmAddTabGroup( button );
/*-------------- start list button --------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button          ); n++;
button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, sub_form, args, n );
XtasSetLabelString( button, " |<- ");
XtAddCallback(button, XmNactivateCallback, XtasSignalsTreatTopCallback, (XtPointer)signalset );
XmAddTabGroup( button );

/*------------ pagination -----------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNrightWidget,      button      ); n++;
XtSetArg( args[n], XmNrightOffset,  4            ); n++;
signalset->INFO_PAGE = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
XtasSetLabelString( signalset->INFO_PAGE, XTAS_NULINFO );


/*--------------------- fin de l'en-tete ----------------------------*/

/*------------------------*/
/*  The Scrolled List     */
/*------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET            ); n++;
XtSetArg( args[n], XmNtopWidget,       frame ); n++;
XtSetArg( args[n], XmNbottomAttachment,   XmATTACH_FORM            ); n++;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM              ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM              ); n++;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form, args, n );

   /*--- new widget ---*/
n=0;
XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNvisibleItemCount,       15                   ); n++;
Xtaslist_widget[signal_type] = XmCreateScrolledList( form, "XtasUserField", args, n );
XtManageChild( Xtaslist_widget[signal_type] );
XmAddTabGroup( Xtaslist_widget[signal_type] );
signalset->SIGLIST_SCROLL = Xtaslist_widget[signal_type]; 
 

XtAddCallback(Xtaslist_widget[signal_type], XmNdefaultActionCallback,   XtasSignalsFocusCallback, (XtPointer)signal_type);
XtAddCallback(Xtaslist_widget[signal_type], XmNbrowseSelectionCallback, XtasSignalsFocusCallback, (XtPointer)signal_type);

/*-------------------------- end scrolled list -----------------------*/

XtManageChild( XtasSignalsMainForm );
return( XtasSignalsMainForm );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsList                                                */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .signal_type : Indicates the windows type.                     */
/*                           0 : Connectors.                                 */
/*                           1 : Registers.                                  */
/*                           2 : Precharged Points.                          */
/*                           3 : All Signals.                                */
/*                           4 : Commands.				     */
/*            .window_title : Indicates the window title.                    */
/*                                                                           */
/* OUT ARGS : The shell widget id.                                           */
/*                                                                           */
/* OBJECT   : The main routine of Signals List Main window command           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasSignalsList( parent, signal_type, window_title )
Widget parent;
int    signal_type;
char   *window_title;
{
Arg args[20];
int n;
XtasSigListSetStruct *signalset;
char        buf[100];
char        text_title[128];

if ( !XtasSignalsTopLevel[signal_type] )
    {
    signalset = XtasNewSigListSet( NULL );
    XtasSignalSet[signal_type] = signalset ;

    sprintf (buf, " - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
    strcpy (text_title, window_title);
    strcat (text_title, buf);
    XtasSignalsTopLevel[signal_type] = XtVaCreatePopupShell( NULL, topLevelShellWidgetClass, parent,
                                                       XmNwidth,            500,
                                                       XmNheight,           480,
                                                       XmNtitle,            text_title,
                                                       XmNallowShellResize, False,
                                                       XmNdeleteResponse,   XmUNMAP,
                                                       NULL );
    signalset->TOP_LEVEL = XtasSignalsTopLevel[signal_type];

    n = 0;
    XtasSignalsMainWindow = XmCreateMainWindow( XtasSignalsTopLevel[signal_type], "XtasMain", args, n );
    XtManageChild( XtasSignalsMainWindow );

    /*------------------------------------------------*/
    /* Create the Alliance applications standard Icon */
    /*------------------------------------------------*/
    switch (signal_type)
        {
        case XTAS_C_CONNECTOR : signalset->SIGNAL->TYPE = TTV_SIG_C;
				XalCustomIcon( XtasSignalsTopLevel[signal_type], XTAS_CONN_MAP   ); break;
        case XTAS_C_COMMANDS  : signalset->SIGNAL->TYPE = TTV_SIG_Q;
				XalCustomIcon( XtasSignalsTopLevel[signal_type], XTAS_COMM_MAP   ); break;
        case XTAS_C_MEMORIZE  : signalset->SIGNAL->TYPE = TTV_SIG_L;
				XalCustomIcon( XtasSignalsTopLevel[signal_type], XTAS_MEMO_MAP   ); break;
        case XTAS_C_PRECHARGE : signalset->SIGNAL->TYPE = TTV_SIG_R;
				XalCustomIcon( XtasSignalsTopLevel[signal_type], XTAS_PREC_MAP   ); break;
        case XTAS_C_BREAK     : signalset->SIGNAL->TYPE = TTV_SIG_B;
				XalCustomIcon( XtasSignalsTopLevel[signal_type], XTAS_BREA_MAP   ); break;
        case XTAS_C_OTHERSIGS : signalset->SIGNAL->TYPE = TTV_SIG_S;
				XalCustomIcon( XtasSignalsTopLevel[signal_type], XTAS_ALLS_MAP   ); break;
        }

    /*------------------------------------------------------------------*/
    /* Create the menu bar now, so it can be in the motif default areas */
    /*------------------------------------------------------------------*/
    XtasSignalsMenuWidget = XtasCreateSignalsMenus( XtasSignalsMainWindow, signal_type );
    XtManageChild( XtasSignalsMenuWidget );

    XtasSignalsWorkWidget[signal_type]  =  XtasCreateSignalsList( XtasSignalsMainWindow, signal_type, signalset );
    XtManageChild( XtasSignalsWorkWidget[signal_type] );

    /*----------------------------------------*/
    /* Creates a Motif Error and Warning Boxs */
    /*----------------------------------------*/

    XmMainWindowSetAreas( XtasSignalsMainWindow,
                          XtasSignalsMenuWidget,
                          (Widget) NULL,
                          (Widget) NULL,
                          (Widget) NULL,
                          XtasSignalsWorkWidget[signal_type]   );
    }
XtPopup( XtasSignalsTopLevel[signal_type], XtGrabNone );

return (XtasSignalsTopLevel[signal_type]);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDeskSignalsViewCallback                                    */
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
void XtasDeskSignalsViewCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
 int key = (int)(long) client_data ;

if ( XtasAppNotReady() ) return;

XtasSignalsSetState( key );

while ( key >= 10 ) key -= 10;

XtasSetLabelString( XtasDeskMessageField, XTAS_SGWINFO );
XalSetCursor( XtasTopLevel, WAIT );
XalForceUpdate( XtasTopLevel );

switch (key)
    {
    case XTAS_C_CONNECTOR : XtasSignalsList( XtasTopLevel, key, XTAS_NAME": Connectors window" ); break;
    case XTAS_C_COMMANDS  : XtasSignalsList( XtasTopLevel, key, XTAS_NAME": Commands window" ); break;
    case XTAS_C_MEMORIZE  : XtasSignalsList( XtasTopLevel, key, XTAS_NAME": Registers window" );   break;
    case XTAS_C_PRECHARGE : XtasSignalsList( XtasTopLevel, key, XTAS_NAME": Precharges window" );  break;
    case XTAS_C_BREAK     : XtasSignalsList( XtasTopLevel, key, XTAS_NAME": Break points window" );  break;
    case XTAS_C_OTHERSIGS : XtasSignalsList( XtasTopLevel, key, XTAS_NAME": All Signals window" ); break;
    }
XtasSetLabelString( XtasDeskMessageField, XTAS_NULINFO );
XalSetCursor( XtasTopLevel, NORMAL );
}

