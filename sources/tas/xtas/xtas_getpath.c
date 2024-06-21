/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getpath.c                                              */
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
#include "xtas_getpath_menu.h"


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Widget XtasGetPathWidget = NULL ;
static XtasSearchPathParamStruct *XtasParam ;
static Widget XTAS_GETPATH_FIELD[10];
static char *XTAS_INIT_START = "*" ;
static char *XTAS_INIT_END = "*" ;
static char *XTAS_INIT_CK = "*" ;
static char   UPUP ;
static char   UPDN ;
static char   DNUP ;
static char   DNDN ;
static char   DUAL ;
static char   MAX ;
static long   REQUEST_TYPE = (XTAS_PATHS | XTAS_CRITIC); 
static char   FILETYPE ; 


/*---------------------------------------------------------------------------*/
void XtasParamUpupCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 UPUP = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamUpdnCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 UPDN = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamDndnCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DNDN = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamDnupCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DNUP = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamMinCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 MAX = ( state->set ) ? 'N' : 'Y' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamMaxCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 MAX = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamFileTtxCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 FILETYPE = ( state->set ) ? 'T' : 'D' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamFileDtxCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 FILETYPE = ( state->set ) ? 'D' : 'T' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamTypePathsCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    Widget  form;
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if (state->set) {
        REQUEST_TYPE &= ~XTAS_ACCESS;
        REQUEST_TYPE |= XTAS_PATHS;
        form = XtParent (XTAS_GETPATH_FIELD[8]);
        XtSetSensitive (form, False);
    }
    else {
        REQUEST_TYPE &= ~XTAS_PATHS;
        REQUEST_TYPE |= XTAS_ACCESS;
        form = XtParent (XTAS_GETPATH_FIELD[8]);
        XtSetSensitive (form, True);
    }
}

/*---------------------------------------------------------------------------*/
void XtasParamTypeAccessCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    Widget  form;
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if (state->set) {
        REQUEST_TYPE &= ~XTAS_PATHS;
        REQUEST_TYPE |= XTAS_ACCESS;
        form = XtParent (XTAS_GETPATH_FIELD[8]);
        XtSetSensitive (form, True);
//        form = XtNameToWidget (XtasGetPathWidget, "*XtasToggleStart");
//        XtVaSetValues (form, XmNset, False, XmNsensitive, False, NULL);
//        form = XtNameToWidget (XtasGetPathWidget, "*XtasToggleEnd");
//        XtVaSetValues (form, XmNset, True, NULL);
    }
    else {
        REQUEST_TYPE &= ~XTAS_ACCESS;
        REQUEST_TYPE |= XTAS_PATHS;
        form = XtParent (XTAS_GETPATH_FIELD[8]);
        XtSetSensitive (form, False);
//        form = XtNameToWidget (XtasGetPathWidget, "*XtasToggleStart");
//        XtSetSensitive (form, True);
    }
}

/*---------------------------------------------------------------------------*/
void XtasParamSubtypeCriticCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 if (state->set) 
     REQUEST_TYPE |= XTAS_CRITIC ;
 else
     REQUEST_TYPE &= ~XTAS_CRITIC ;
}

/*---------------------------------------------------------------------------*/
void XtasParamSubtypeSigBySigCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 if (state->set) 
     REQUEST_TYPE |= XTAS_SBS ;
 else
     REQUEST_TYPE &= ~XTAS_SBS ;
}

/*---------------------------------------------------------------------------*/
void XtasParamSubtypeAllCallback( parent, client_data, cbs )
Widget parent;
XtPointer client_data;
XtPointer cbs;
{
    Widget  widget;
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 if (state->set)  {
     REQUEST_TYPE |= XTAS_ALL ;
     widget = XtNameToWidget (XtasGetPathWidget, "*XtasGetPathTtx");
     XtVaSetValues (widget, XmNset, False, XmNsensitive, False, NULL);
     widget = XtNameToWidget (XtasGetPathWidget, "*XtasGetPathDtx");
     XtVaSetValues (widget, XmNset, True, NULL);
     FILETYPE = 'D';
 }
 else {
     REQUEST_TYPE &= ~XTAS_ALL ;
     widget = XtNameToWidget (XtasGetPathWidget, "*XtasGetPathTtx");
     XtVaSetValues (widget, XmNsensitive, True, NULL);
 }
}

/*---------------------------------------------------------------------------*/
void XtasParamDualCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DUAL = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParamNormCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 DUAL = ( state->set ) ? 'N' : 'Y' ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStartEndSignalCallback                                     */
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
void XtasStartEndSignalCallback( widget, key, call_data )
Widget  widget;
XtPointer key;
XtPointer call_data;
{
 XtasSignalsTopWidget =  XtParent(XtasGetPathWidget) ;
 XtasDeskSignalsViewCallback( widget, key, call_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSetOrderByStartOrEnd                                       */
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
Widget XtasSetOrderByStartOrEnd( key, clear, name )
int key;
int clear ;
char *name;
{
 Widget from_or_to;
 char *text ;

 if(XtasGetPathWidget == NULL)
   {
    if ( (key >= 20) && (key <= 20+XTAS_C_NBSIGTYPE-1) )
      XTAS_INIT_END = namealloc(name) ;
    else if ( (key >= 10) && (key <= 10+XTAS_C_NBSIGTYPE-1) )
      XTAS_INIT_START = namealloc(name) ;
    return NULL;
   }

 if ( (key >= 20) && (key <= 20+XTAS_C_NBSIGTYPE-1) )
   from_or_to = XTAS_GETPATH_FIELD[1];
 else if ( (key >= 10) && (key <= 10+XTAS_C_NBSIGTYPE-1) )
   from_or_to = XTAS_GETPATH_FIELD[0];
 else
   from_or_to = NULL;

 if(clear == 1)
   XmTextSetString(from_or_to,"");

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
/* FUNCTION : XtasGetPathsCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .parent : The parent widget.                                   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Get Paths on OK Button.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetPathsOkCallback(widget,client_data,call_data )
Widget  widget;
XtPointer client_data ;
XtPointer call_data;
{
 Widget parent = (Widget) client_data ;
 char *tmp[10];
 char unvalid_instance_mask = 0;
 int i, j;
 int numindex [5] = {2,3,4,5,9};  /* liste des index pour les text box devant contenir des nombres */

/*------- check for text fields ------------------------*/

 for(i = 0 ; i<10 ; i++)
   tmp[i] = XmTextGetString( XTAS_GETPATH_FIELD[i] );
  
 
 for(i = 0 ; i < 5 ; i++)
  {
   for(j = 0 ; j < (int)strlen(tmp[numindex[i]]) ; j++)
    {
     if(!isdigit((int)tmp[numindex[i]][j]))
      {
       for( i=0; i<10; i++ ) 
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

 XtasSuppresStringSpace(tmp[7]);   
 XtasSuppresStringSpace(tmp[6]);

 if ( (strlen(tmp[6]) == 0) || (strlen(tmp[7]) == 0)
     || (!strcmp(tmp[6], "AllLevel")) || (!strcmp(tmp[7], "AllLevel")) )
  {
    XtasParam->ROOT_SIG_EDGE->LOCAL_FIG = NULL ; 
  }
 else
  { 
   XtasParam->ROOT_SIG_EDGE->LOCAL_FIG = ttv_getttvins(
                               XtasParam->ROOT_SIG_EDGE->TOP_FIG,tmp[6],tmp[7]);
   unvalid_instance_mask = (XtasParam->ROOT_SIG_EDGE->LOCAL_FIG == NULL) ;
  }
 
  if ( DUAL == 'Y' )
    XtasParam->SCAN_TYPE |= TTV_FIND_DUAL;
  else
    XtasParam->SCAN_TYPE &= ~(TTV_FIND_DUAL);
  
  if ( FILETYPE == 'T' )
    {
      XtasParam->SCAN_TYPE &= ~(TTV_FIND_LINE);
      XtasParam->SCAN_TYPE |= TTV_FIND_PATH;
    }
  else
    {
      XtasParam->SCAN_TYPE &= ~(TTV_FIND_PATH);
      XtasParam->SCAN_TYPE |= TTV_FIND_LINE;
    }

  XtasParam->REQUEST_TYPE = REQUEST_TYPE;
  if ((REQUEST_TYPE & (XTAS_PATHS |XTAS_CRITIC)) == (XTAS_PATHS |XTAS_CRITIC))
      XtasParam->CRITIC = 'Y';
  else
      XtasParam->CRITIC = 'N';
  if ((REQUEST_TYPE & XTAS_ACCESS) == XTAS_ACCESS) {
      if((strlen(tmp[8]) == 0) || (strcmp(tmp[8],"*") == 0))
        {
         if(XtasParam->CK_MASK != NULL)
          {
           freechain(XtasParam->CK_MASK) ;
	       XtasParam->CK_MASK = NULL ;
          }
        }
      else
        {
         if(XtasParam->CK_MASK != NULL)
          {
           freechain(XtasParam->CK_MASK) ;
          }
	    XtasParam->CK_MASK = XtasGetMask( XTAS_GETPATH_FIELD[8] ); 
        }
  }
          
  
  if ( MAX  == 'Y' )
    {
      XtasParam->SCAN_TYPE &= ~(TTV_FIND_MIN);
      XtasParam->SCAN_TYPE |= TTV_FIND_MAX;
    }
  else
    { 
      XtasParam->SCAN_TYPE &= ~(TTV_FIND_MAX);
      XtasParam->SCAN_TYPE |= TTV_FIND_MIN;
    }
  
  if ( DUAL =='Y' )
    {
      if((strlen(tmp[0]) == 0) || (strcmp(tmp[0],"*") == 0))
        {
         if(XtasParam->ROOT_SIG_EDGE->MASK != NULL)
          {
           freechain(XtasParam->ROOT_SIG_EDGE->MASK) ;
	   XtasParam->ROOT_SIG_EDGE->MASK = NULL ;
          }
        }
      else
        {
         if(XtasParam->ROOT_SIG_EDGE->MASK != NULL)
          {
           freechain(XtasParam->ROOT_SIG_EDGE->MASK) ;
          }
	 XtasParam->ROOT_SIG_EDGE->MASK = XtasGetMask( XTAS_GETPATH_FIELD[0] ); 
        }
      
      if((strlen(tmp[1]) == 0) || (strcmp(tmp[1],"*") == 0))
       {
        if(XtasParam->NODE_MASK != NULL)
         {
          freechain(XtasParam->NODE_MASK) ;
	  XtasParam->NODE_MASK        = NULL ;
         }
       }
      else
       {
        if(XtasParam->NODE_MASK != NULL)
         {
          freechain(XtasParam->NODE_MASK) ;
         }
	XtasParam->NODE_MASK        = XtasGetMask( XTAS_GETPATH_FIELD[1] ); 
       }
    }
  else
    {
      if((strlen(tmp[1]) == 0) || (strcmp(tmp[1],"*") == 0))
       {
        if(XtasParam->ROOT_SIG_EDGE->MASK != NULL)
         {
          freechain(XtasParam->ROOT_SIG_EDGE->MASK) ;
	  XtasParam->ROOT_SIG_EDGE->MASK  = NULL ;
         }
       }
      else
       {
        if(XtasParam->ROOT_SIG_EDGE->MASK != NULL)
         {
          freechain(XtasParam->ROOT_SIG_EDGE->MASK) ;
         }
	XtasParam->ROOT_SIG_EDGE->MASK  = XtasGetMask( XTAS_GETPATH_FIELD[1] ); 
       }
      
      if((strlen(tmp[0]) == 0) || (strcmp(tmp[0],"*") == 0))
       {
        if(XtasParam->NODE_MASK != NULL)
         {
          freechain(XtasParam->NODE_MASK) ;
	  XtasParam->NODE_MASK        = NULL ;
         }
       }
      else
       {
        if(XtasParam->NODE_MASK != NULL)
         {
          freechain(XtasParam->NODE_MASK) ;
         }
	XtasParam->NODE_MASK        = XtasGetMask( XTAS_GETPATH_FIELD[0] ); 
       }
    }
  
 if (strlen(tmp[2]) == 0) 
    XtasParam->PATH_ITEM            = XTAS_DEFAULT_PATHITEMS ;
 else
    XtasParam->PATH_ITEM            = atoi( tmp[2] ); 
  
 if(strlen(tmp[3]) == 0) 
    XtasParam->ROOT_SIG_EDGE->ITEMS = XTAS_DEFAULT_SIGITEMS ;
 else
    XtasParam->ROOT_SIG_EDGE->ITEMS = atoi( tmp[3] );
  
 if(strlen(tmp[4]) == 0)
    XtasParam->DELAY_MIN            = TTV_DELAY_MIN;
 else
    XtasParam->DELAY_MIN            = atol( tmp[4] )*TTV_UNIT;
  
 if(strlen(tmp[5]) == 0)
    XtasParam->DELAY_MAX            = TTV_DELAY_MAX;
 else
    XtasParam->DELAY_MAX            = atol( tmp[5] )*TTV_UNIT;
  
 if(strlen(tmp[9]) != 0)
    V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE             = atol( tmp[9] );
  
 if(UPUP == 'Y')
    {
      XtasParam->SCAN_TYPE &= ~TTV_FIND_NOT_UPUP ;  
    }
 else
    {
      XtasParam->SCAN_TYPE |= TTV_FIND_NOT_UPUP ;
    }

 if(DNUP == 'Y')
   {
     XtasParam->SCAN_TYPE &= ~TTV_FIND_NOT_DWUP ;
   }
 else
    {
      XtasParam->SCAN_TYPE |= TTV_FIND_NOT_DWUP ;
    }

 if(UPDN == 'Y')
    {   
      XtasParam->SCAN_TYPE &= ~TTV_FIND_NOT_UPDW ;
    }
 else
   {
     XtasParam->SCAN_TYPE |= TTV_FIND_NOT_UPDW ;
   }  

 if(DNDN == 'Y')
    {
      XtasParam->SCAN_TYPE &= ~TTV_FIND_NOT_DWDW ;
    }
 else
   {
    XtasParam->SCAN_TYPE |= TTV_FIND_NOT_DWDW ;
   }

 
 
 for(i=0; i < 10; i++)
   if(tmp[i])
    XtFree(tmp[i]) ;
 
 XtasSetLabelString( XtasDeskMessageField, XTAS_PATINFO );
 XalSetCursor( XtasTopLevel, WAIT ); 
 XalForceUpdate( XtasTopLevel );
 if((unvalid_instance_mask) || (XtasPathsList(XtasTopLevel,XtasParam) == NULL))
    {
      XalDrawMessage( XtasWarningWidget , XTAS_NPATWAR );
    }
  
 XtasSetLabelString( XtasDeskMessageField, XTAS_NULINFO );
 XalSetCursor( XtasTopLevel, NORMAL ); 
 //XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCancelGetPathsCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Get Paths on OK Button.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCancelGetPathsCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
  //XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasMainToolsCallback                                          */
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
void XtasMainToolsCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
  int      n, i;
  Arg      args[10];
  XmString text;
  Widget   form, form_t, dbase_form, ebase_form, fbase_form, gbase_form;
  Widget   row_widget, frame_widget, label_widget, toggle;
  Widget   tmp_widget, widget1=NULL, widget2=NULL;
  static   XtasHierStruct hier ;
  Atom     WM_DELETE_WINDOW;
  char     buf [8];
  
  short int *mask;
  Widget    caller=NULL;

  if (client_data) {
      mask = (short int*)client_data;
      if(*mask > 0x3)
          mask = NULL;
  }
  else 
      mask = NULL;

  if ( XtasAppNotReady() ) return;
  
  if ( !XtasGetPathWidget )
    {
      
/*--- creation d'un Prompt Dialog ---- generalites ----------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtitle, XTAS_NAME" : Get Paths..." ); n++;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
    XtasGetPathWidget = XmCreatePromptDialog( XtasDeskMainForm, 
					     "XtasInfosBox", 
					     args, n);
	HelpFather = XtasGetPathWidget ;
    XtUnmanageChild(XmSelectionBoxGetChild(XtasGetPathWidget, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(XtasGetPathWidget, XmDIALOG_PROMPT_LABEL));
    XtAddCallback( XtasGetPathWidget, XmNokCallback, XtasGetPathsOkCallback, (XtPointer)XtasGetPathWidget );
    XtAddCallback( XtasGetPathWidget, XmNcancelCallback, XtasCancelGetPathsCallback, (XtPointer)XtasGetPathWidget );
    XtAddCallback( XtasGetPathWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_SRC_GETPATH|XTAS_HELP_MAIN) );
    XmAddTabGroup( XtasGetPathWidget ); 
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(XtParent(XtasGetPathWidget), WM_DELETE_WINDOW, XtasCancelGetPathsCallback, (XtPointer )XtasGetPathWidget);
    
    
    n = 0;
    form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, XtasGetPathWidget, args, n );
    
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
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNsensitive,        (REQUEST_TYPE & XTAS_ACCESS) == XTAS_ACCESS ? True:False  ); n++;
    gbase_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, fbase_form, args, n);
    
    text = XmStringCreateSimple( "Clock(s)" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            60                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, gbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         83                ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETPATH_FIELD[8] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, gbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[8] );
    XmTextSetString( XTAS_GETPATH_FIELD[8], XTAS_INIT_CK );
    
    text = XmStringCreateSimple( "Start" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        gbase_form        ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            60                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         83                ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETPATH_FIELD[0] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[0] );
    XmTextSetString( XTAS_GETPATH_FIELD[0], XTAS_INIT_START );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET  ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPATH_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET  ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPATH_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPATH_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tmp_widget = XmCreateMenuBar( fbase_form, "XtasMenuBar", args, n );

    for (i=0; i< XTAS_C_NBSIGTYPE; i++)
        XtasGetPathsMenuView[i].callback_data += 10;

    XalBuildMenus( tmp_widget, XtasGetPathStartMenu );
    XtManageChild( tmp_widget );

    text = XmStringCreateSimple( "End" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5                 ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            60                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         83                ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETPATH_FIELD[1] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[1] );
    XmTextSetString( XTAS_GETPATH_FIELD[1], XTAS_INIT_END );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET  ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPATH_FIELD[1]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET  ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPATH_FIELD[1]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPATH_FIELD[1]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tmp_widget = XmCreateMenuBar( fbase_form, "XtasMenuBar", args, n );

    for (i=0; i< XTAS_C_NBSIGTYPE; i++)
        XtasGetPathsMenuView[i].callback_data += 10;

    XalBuildMenus( tmp_widget, XtasGetPathEndMenu );
    XtManageChild( tmp_widget );


     
/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        22              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     2               ); n++;
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

    text = XmStringCreateSimple( "Paths" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_END   ); n++;
    XtSetArg( args[n], XmNtopOffset,        15                ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            50                ); n++;
/*
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ); n++;
*/
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, ebase_form, args, n );
    XmStringFree( text );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,        XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,            label_widget    ); n++;
    XtSetArg( args[n], XmNbottomAttachment,     XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,         label_widget    ); n++;
    XtSetArg( args[n], XmNleftAttachment,       XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,           label_widget    ); n++;
    XtSetArg( args[n], XmNrightAttachment,      XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightOffset,          2               ); n++;
    XtSetArg( args[n], XmNleftOffset,           2               ); n++;
    XtSetArg( args[n], XmNshadowThickness,      2               ); n++;
    XTAS_GETPATH_FIELD[2] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, ebase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[2] );
    XmTextSetString( XTAS_GETPATH_FIELD[2], "10" );
    
    text = XmStringCreateSimple( "Signals" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_END   ); n++;
    XtSetArg( args[n], XmNtopOffset,        30                ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5                 ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            50                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, ebase_form, args, n );
    XmStringFree( text );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,          label_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
    XtSetArg( args[n], XmNleftOffset,            2                 ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XTAS_GETPATH_FIELD[3] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, ebase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[3] );
    XmTextSetString( XTAS_GETPATH_FIELD[3], "10" );
/*-------------- 2 eme sub form ------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        form_t            ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
    form_t = dbase_form;
    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

/*------------------------- Time Bounds ------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    40                ); n++;
    
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       2     ); n++;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );
    n = 0;
    text = XmStringCreateSimple( "Time Bounds" );
    XtSetArg( args[n], XmNlabelString,      text ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n );
    XmStringFree( text );
    
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
    
    text = XmStringCreateSimple( "Lower Bound" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        10                ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       4                 ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            110               ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,        XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNtopWidget,            tmp_widget                  ); n++;
    XtSetArg( args[n], XmNbottomAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNbottomWidget,         tmp_widget                  ); n++;
    XtSetArg( args[n], XmNleftAttachment,       XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,           tmp_widget                  ); n++;
    XtSetArg( args[n], XmNrightAttachment,      XmATTACH_POSITION           ); n++;
    XtSetArg( args[n], XmNrightPosition,        95                          ); n++; 
    XtSetArg( args[n], XmNshadowThickness,      2                           ); n++;
    XTAS_GETPATH_FIELD[4] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[4] );
    XmTextSetString( XTAS_GETPATH_FIELD[4] , "" );
 
    text = XmStringCreateSimple( "Upper Bound" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,          text                    ); n++;
    XtSetArg( args[n], XmNtopAttachment,        XmATTACH_WIDGET         ); n++;
    XtSetArg( args[n], XmNtopWidget,            XTAS_GETPATH_FIELD[4]   ); n++;
    XtSetArg( args[n], XmNleftAttachment,       XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNleftWidget,           tmp_widget              ); n++;
    XtSetArg( args[n], XmNheight,               35                      ); n++;
    XtSetArg( args[n], XmNwidth,                110                     ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,        XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNtopWidget,            tmp_widget                  ); n++;
    XtSetArg( args[n], XmNbottomAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNbottomWidget,         tmp_widget                  ); n++;
    XtSetArg( args[n], XmNleftAttachment,       XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,           tmp_widget                  ); n++;
    XtSetArg( args[n], XmNrightAttachment,      XmATTACH_POSITION           ); n++;
    XtSetArg( args[n], XmNrightPosition,        95                          ); n++; 
    XtSetArg( args[n], XmNshadowThickness,      2                           ); n++;
    XTAS_GETPATH_FIELD[5]  = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[5] );
    XmTextSetString( XTAS_GETPATH_FIELD[5] , "" );
     
/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        22              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     2               ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n );

/*------------------------ Request Type --------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

    text = XmStringCreateSimple( "Request Type" );
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

    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNtopOffset,        4                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
                                           XmNtopAttachment,   XmATTACH_FORM,
                                           XmNtopOffset,       3,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      24,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNspacing,         9,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );


    text = XmStringCreateSimple( "Paths" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS) ? True:False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamTypePathsCallback, &REQUEST_TYPE);

    text = XmStringCreateSimple( "Access Paths" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((REQUEST_TYPE & XTAS_ACCESS) == XTAS_ACCESS) ? True:False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamTypeAccessCallback, &REQUEST_TYPE);

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        3               ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       row_widget      ); n++;
    XtSetArg( args[n], XmNleftOffset,       24              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     0               ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, fbase_form, args, n );


 /*---------------------------------  ----------------------------------------*/
    
    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
                                           XmNtopAttachment,   XmATTACH_FORM,
                                           XmNtopOffset,       3,
                                           XmNleftAttachment,  XmATTACH_WIDGET,
                                           XmNleftWidget,      gbase_form,
                                           XmNleftOffset,      24,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNspacing,         9,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );


    text = XmStringCreateSimple( "Critic" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((REQUEST_TYPE & XTAS_CRITIC) == XTAS_CRITIC) ? True:False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamSubtypeCriticCallback, &REQUEST_TYPE);

    text = XmStringCreateSimple( "All" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((REQUEST_TYPE & XTAS_ALL) == XTAS_ALL) ? True:False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamSubtypeAllCallback, &REQUEST_TYPE);


/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        3               ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       row_widget      ); n++;
    XtSetArg( args[n], XmNleftOffset,       24              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     0               ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, fbase_form, args, n );


 /*---------------------------------  ----------------------------------------*/
    
    text = XmStringCreateLtoR( "Signal by Signal", XmSTRING_DEFAULT_CHARSET );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
                                      XmNrightAttachment,     XmATTACH_FORM,
                                      XmNrightOffset,        24,
                                      XmNbottomAttachment,     XmATTACH_FORM,
                                      XmNtopAttachment,     XmATTACH_FORM,
                                      XmNtopWidget,         25,
                                      XmNleftAttachment,    XmATTACH_WIDGET,
                                      XmNleftWidget,        gbase_form,
                                      XmNleftOffset,        24,
                                      XmNindicatorType,     XmN_OF_MANY,
                                      XmNset,               ((REQUEST_TYPE & XTAS_SBS) == XTAS_SBS) ? True:False,
                                      XmNlabelString,       text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamSubtypeSigBySigCallback, &REQUEST_TYPE);

/*----------------- 3 eme sub form -------------------------------------------------------------------*/
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

/*------------------------------------------- Slopes mask -------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    35                ); n++; 
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Slopes Mask" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       2                 ); n++;
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
                                           XmNleftOffset,      36,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNbottomAttachment,XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNspacing,         9,
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
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamUpupCallback, &UPUP );
    UPUP = 'Y';

    text = XmStringCreateSimple( "Rise Fall" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamUpdnCallback, &UPDN );
    UPDN = 'Y';

    text = XmStringCreateSimple( "Fall Fall      " );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamDndnCallback, &DNDN );
    DNDN = 'Y';

    text = XmStringCreateSimple( "Fall Rise" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamDnupCallback, &DNUP );
    DNUP = 'Y';

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        25              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5               ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    fbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n );

 /*------------------------------------------ Time sorting -------------------------------------------*/ 
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    50                ); n++; 
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
                                           XmNleftOffset,      34,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNspacing,         8,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "Max" );
    toggle = XtVaCreateManagedWidget( "XtasGetPathMax", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamMaxCallback, &MAX );
    MAX = 'Y';

    text = XmStringCreateSimple( "Min" );
    toggle = XtVaCreateManagedWidget( "XtasGetPathMin", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamMinCallback, &MAX );

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        25              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5               ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    fbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n );


 /*------------------------------------------ Graph type -------------------------------------------*/ 
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,    65                ); n++; 
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "File Type" );
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
                                           XmNleftOffset,      37,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNspacing,         8,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "ttx" );
    toggle = XtVaCreateManagedWidget( "XtasGetPathTtx", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamFileTtxCallback, &MAX );
    FILETYPE = 'T';
    if ((REQUEST_TYPE & XTAS_ALL) == XTAS_ALL) {
        XtVaSetValues(toggle, XmNset, False, XmNsensitive, False, NULL);        
        FILETYPE = 'D';
    }
    else if (XTAS_FORMAT_LOADED == XTAS_DTX) {
        XtVaSetValues(toggle, XmNset, False, NULL);
        FILETYPE = 'D';
    }
    
    text = XmStringCreateSimple( "dtx" );
    toggle = XtVaCreateManagedWidget( "XtasGetPathDtx", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamFileDtxCallback, &MAX );
    if((XTAS_FORMAT_LOADED == XTAS_DTX)||((REQUEST_TYPE & XTAS_ALL) == XTAS_ALL)) 
        XtVaSetValues(toggle, XmNset, True, NULL);

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        25              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5               ); n++;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ); n++;
    fbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n );


 /*------------------------------------------ Search Type -------------------------------------------*/ 
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form        ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++; 
    XtSetArg( args[n], XmNrightPosition,    80                ); n++; 
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Order by" );
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
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      35,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNbottomAttachment,   XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNspacing,         8,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "Start" );
    toggle = XtVaCreateManagedWidget( "XtasToggleStart", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           False,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamDualCallback, &DUAL );

    text = XmStringCreateSimple( "End" );
    toggle = XtVaCreateManagedWidget( "XtasToggleEnd", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParamNormCallback, &DUAL );
    DUAL = 'N';

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       fbase_form      ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        25              ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomOffset,     5               ); n++;
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

    text = XmStringCreateSimple( "Path Depth" );
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

    text = XmStringCreateLtoR( "Max Number\nof Cycles", XmSTRING_DEFAULT_CHARSET );
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++; 
    XtSetArg ( args[n], XmNtopWidget,       frame_widget    ); n++; 
    XtSetArg ( args[n], XmNtopOffset,       15              ); n++; 
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++; 
    XtSetArg ( args[n], XmNleftOffset,      20              ); n++; 
    XtSetArg ( args[n], XmNlabelString,     text            ); n++; 
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, fbase_form, args, n );
    XmStringFree (text);

    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg ( args[n], XmNtopWidget,       label_widget    ); n++;
    XtSetArg ( args[n], XmNtopOffset,       2               ); n++;
    XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg ( args[n], XmNbottomWidget,    label_widget    ); n++;
    XtSetArg ( args[n], XmNbottomOffset,    2               ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET ); n++;
    XtSetArg ( args[n], XmNleftWidget,      label_widget    ); n++;
    XtSetArg ( args[n], XmNleftOffset,      15              ); n++;
    XtSetArg ( args[n], XmNwidth,           40              ); n++;
    XTAS_GETPATH_FIELD[9] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[9] );
    sprintf (buf, "%d", V_INT_TAB[__TTV_MAX_PATH_PERIOD].VALUE);
    XmTextSetString( XTAS_GETPATH_FIELD[9], buf);

    

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
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( "Figure Name" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrecomputeSize,    False             ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            110               ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget                ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNbottomWidget,     label_widget                ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,       label_widget                ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION           ); n++;
    XtSetArg( args[n], XmNrightPosition,    40                          ); n++;
    XtSetArg( args[n], XmNshadowThickness,  2                           ); n++;

    XTAS_GETPATH_FIELD[6] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[6] );
    XmTextSetString( XTAS_GETPATH_FIELD[6], "" );

    text = XmStringCreateSimple( "Instance Name" );
    n = 0;

    XtSetArg( args[n], XmNlabelString,      text                    ); n++;
    XtSetArg( args[n], XmNrecomputeSize,    False                   ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNtopWidget,        XTAS_GETPATH_FIELD[6]   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNbottomWidget,     XTAS_GETPATH_FIELD[6]   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET         ); n++;
    XtSetArg( args[n], XmNleftWidget,       XTAS_GETPATH_FIELD[6]   ); n++;
    XtSetArg( args[n], XmNheight,           35                      ); n++;
    XtSetArg( args[n], XmNwidth,            110                     ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );


    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget            ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNbottomWidget,     label_widget            ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET         ); n++;
    XtSetArg( args[n], XmNleftWidget,       label_widget            ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION       ); n++;
    XtSetArg( args[n], XmNrightPosition,    80                      ); n++;
    XtSetArg( args[n], XmNshadowThickness,  2                       ); n++;

    XTAS_GETPATH_FIELD[7] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPATH_FIELD[7] );
    XmTextSetString( XTAS_GETPATH_FIELD[7], "" );

    text = XmStringCreateSimple( "List" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPATH_FIELD[7]   ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPATH_FIELD[7]   ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM           ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                       ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET         ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPATH_FIELD[7]   ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                       ); n++;
    XtSetArg( args[n], XmNlabelString,           text                    ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles",xmPushButtonWidgetClass,fbase_form, args, n );
    XmStringFree( text );

    XtasParam = XtasNewSearchPathParam( NULL );
    /* figure hieararchy */
    hier.instance = XTAS_GETPATH_FIELD[7] ;  
    hier.figure   = XTAS_GETPATH_FIELD[6] ;
    hier.param = XtasParam->ROOT_SIG_EDGE ;
    XtAddCallback(tmp_widget, XmNactivateCallback , 
                  XtasHierarchicalViewCallback, &hier) ; 
   }

  XtasParam->ROOT_SIG_EDGE->TOP_FIG = XtasMainParam->ttvfig ;
  XtasParam->ROOT_SIG_EDGE->LOCAL_FIG = NULL ;
 
  widget1 = XtNameToWidget(XtasGetPathWidget,"*XtasGetPathDtx");
  widget2 = XtNameToWidget(XtasGetPathWidget,"*XtasGetPathTtx");
  if(widget1 && widget2) {
      if((XTAS_FORMAT_LOADED == XTAS_DTX) || ((REQUEST_TYPE & XTAS_ALL) == XTAS_ALL)) {
          XtVaSetValues(widget1, XmNset, True, NULL);
          XtVaSetValues(widget2, XmNset, False, NULL);
          FILETYPE = 'D';
      }
      else {
          XtVaSetValues(widget2, XmNset, True, NULL);
          XtVaSetValues(widget1, XmNset, False, NULL);
          FILETYPE = 'T';
      }
  }
  
  widget1 = XtNameToWidget(XtasGetPathWidget,"*XtasGetPathMin");
  widget2 = XtNameToWidget(XtasGetPathWidget,"*XtasGetPathMax");
  if(widget1 && widget2) {
      if(mask && ((*mask & XTAS_HOLD)==XTAS_HOLD)) {
          XtVaSetValues(widget1, XmNset, True, NULL);
          XtVaSetValues(widget2, XmNset, False, NULL);
          MAX = 'N';
      }
      else {
          XtVaSetValues(widget2, XmNset, True, NULL);
          XtVaSetValues(widget1, XmNset, False, NULL);
          MAX = 'Y';
      }
  }
  XtManageChild( XtasGetPathWidget );
 // XalLimitedLoop( XtasGetPathWidget );
}
