/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_getpara.c                                              */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Mathieu OKUYAMA                     Date : 04/22/1998     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
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
#include "xtas_getpara_menu.h"

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
static Widget   XtasGetParaWidget = NULL ;
static XtasSearchPathParamStruct *XtasParaParam;
XtasPathListSetStruct  *XtasPathParaSet ;
static Widget XTAS_GETPARAPATH_FIELD[8];
   
static char   XTAS_AND    = 'N'; 
static char   XTAS_OR     = 'Y';
static char   XTAS_NOT    = 'N';
static char   DUAL   = 'N';
static char   MAX    = 'Y';
static char   CRITIC = 'N';

/*--------------------------------------------------------------------------*/
/* function   : name                                                        */
/* parametres :                                                             */
/*                                                                          */ 
/*                                                                          */ 
/*                                                                          */ 
/*                                                                          */ 
/*--------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
void XtasParaSignalCallback( widget, key, call_data )
Widget  widget;
XtPointer key;
XtPointer call_data;
{
 XtasSignalsTopWidget =  XtParent(XtasGetParaWidget) ;
 XtasDeskSignalsViewCallback( widget, key , call_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasParaParamTypeCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget      : The widget on which the event occurs.           */
/*            .client_data : The changing variable address.                  */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when slopes type toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasParaParamAndCallback( widget, client_data, cbs )
Widget widget; 
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ; 

   XTAS_AND = ( state->set ) ? 'Y' : 'N' ;
}


/*---------------------------------------------------------------------------*/
void XtasParaParamOrCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
   XTAS_OR = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
void XtasParaParamNotCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
   XTAS_NOT = ( state->set ) ? 'Y' : 'N' ;
}


/*---------------------------------------------------------------------------*/
void XtasParaParamMinCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
   MAX = ( state->set ) ? 'N' : 'Y' ;
}


/*---------------------------------------------------------------------------*/
void XtasParaParamMaxCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
   MAX = ( state->set ) ? 'Y' : 'N' ;
}


/*---------------------------------------------------------------------------*/
void XtasParaParamCriticCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
   CRITIC = ( state->set ) ? 'Y' : 'N' ;
}


/*---------------------------------------------------------------------------*/
void XtasParaParamDualCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
   DUAL = ( state->set ) ? 'Y' : 'N' ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* Copie coller mathieu                                                      */
/* FUNCTION : XtasSetFromOrTo                                                */
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
void XtasSetFromOrTo (key, name)
int    key;
char  *name;
{
	Widget from_or_to ; 
	char *text ;

	if ((key >= 30) && (key <= 30 + XTAS_C_NBSIGTYPE - 1)) 
		from_or_to = XTAS_GETPARAPATH_FIELD[0];
 	else 
	if ( (key >= 40) && (key <= 50 + XTAS_C_NBSIGTYPE - 1)) {
   		XalForceUpdate (XtasSetDelayByStartOrEnd (key, name)) ;
   		return ;
  	}
 	else 
	if ( (key >= 60) && (key <= 60 + XTAS_C_NBSIGTYPE - 1)) {
   		XalForceUpdate (XtasSetDebugSignal (name)) ;
		return ;
  	}
 	else {
   		XalForceUpdate (XtasSetOrderByStartOrEnd (key, 0, name)) ;
   		return ;
  	}
      
  	if (name) {
   		text = XmTextGetString (from_or_to);
   		XtasSuppresStringSpace (text) ;
   		if (!strcmp(text, "*"))
    		XmTextSetString (from_or_to, "") ;
   		else if (text[0] != '\0')
    		XmTextInsert (from_or_to, 0, " ") ;
   		XtFree (text) ;
  	}

 	XmTextInsert (from_or_to, 0, name) ;
 	XalForceUpdate (from_or_to) ;  
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetParaPathsOkCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .PathListSet :                                                 */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Get Paths on OK Button.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetParaPathsOkCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   Widget parent = (Widget) client_data ;
   XtasWindowStruct       *window ;
   char               unvalid_instance_mask = 0;     
   char              *tmp[8];
   int                i; 
   int                j;

   for ( i = 0 ; i<6 ; i++ )
   {
      tmp[i] = XmTextGetString( XTAS_GETPARAPATH_FIELD[i] );
   }
  

   for ( i = 1 ; i<4 ; i++ )
   {
      for ( j = 0 ; j<(int)strlen( tmp[i] ) ; j++ )
      {
         if ( !isdigit( (int)tmp[i][j] ) )
         {
            for ( i=0; i<6; i++ ) 
            { 
               if ( tmp[i] ) 
               XtFree( tmp[i] ); 
            }
            XtManageChild( parent ); 
            XalDrawMessage( XtasErrorWidget, XTAS_NDIGERR ); 
            return;
         }
      }
   }

   XtasSuppresStringSpace(tmp[4]);   
   XtasSuppresStringSpace(tmp[5]);

   if ((strlen(tmp[4]) == 0) || (strlen(tmp[5]) == 0))
   {
      XtasParaParam->ROOT_SIG_EDGE->LOCAL_FIG = NULL ; 
   }
   else
   { 
      XtasParaParam->ROOT_SIG_EDGE->LOCAL_FIG =
	(ttvfig_list*) 
	  ttv_getttvins
	   ((ttvfig_list*) (XtasParaParam->ROOT_SIG_EDGE->TOP_FIG),
		            tmp[4],
		            tmp[5]
		           );

      unvalid_instance_mask = (XtasParaParam->ROOT_SIG_EDGE->LOCAL_FIG == NULL);

   }
  
   if ( DUAL == 'Y' )
   {
      XtasParaParam->SCAN_TYPE |= TTV_FIND_DUAL;
   }
   else
   {
      XtasParaParam->SCAN_TYPE &= ~(TTV_FIND_DUAL);
   }

   if ( MAX  == 'Y' )
   {
      XtasParaParam->SCAN_TYPE &= ~(TTV_FIND_MIN);
      XtasParaParam->SCAN_TYPE |= TTV_FIND_MAX;
   }
   else
   { 
      XtasParaParam->SCAN_TYPE &= ~(TTV_FIND_MAX);
      XtasParaParam->SCAN_TYPE |= TTV_FIND_MIN;
   }

   if((strlen(tmp[0]) == 0) || (strcmp(tmp[0],"*") == 0))
   {
      if(XtasParaParam->MASK != NULL)
        {
         freechain(XtasParaParam->MASK) ;
         XtasParaParam->MASK  = NULL ;
        }
   }
   else
   {
      if(XtasParaParam->MASK != NULL)
        {
         freechain(XtasParaParam->MASK) ;
        }
      XtasParaParam->MASK = XtasGetMask( XTAS_GETPARAPATH_FIELD[0] );
   }

   if (strlen(tmp[1]) == 0)
   { 
      XtasParaParam->PATH_ITEM            = XTAS_DEFAULT_SIGITEMS ;
   }
   else
   {
      XtasParaParam->PATH_ITEM            = atoi( tmp[1] ); 
   }

   if(strlen(tmp[2]) == 0)
   {
      XtasParaParam->DELAY_MIN            = TTV_DELAY_MIN;
   }
   else
   {
      XtasParaParam->DELAY_MIN            = atol( tmp[2] );
   }
  
   if(strlen(tmp[3]) == 0)
   {
      XtasParaParam->DELAY_MAX            = TTV_DELAY_MAX;
   }
   else
   {
      XtasParaParam->DELAY_MAX            = atol( tmp[3] );
   }
  
   if(XTAS_AND == 'Y')
   {
      XtasParaParam->SCAN_TYPE |= TTV_FIND_MASKAND;
   }
   else
   {
      XtasParaParam->SCAN_TYPE &= ~(TTV_FIND_MASKAND);
   }
 
   if(XTAS_OR == 'Y')
   {
      XtasParaParam->SCAN_TYPE |= TTV_FIND_MASKOR;
   }
   else
   {
      XtasParaParam->SCAN_TYPE &= ~(TTV_FIND_MASKOR);
   }
  
   if(XTAS_NOT == 'Y')
   {
      XtasParaParam->SCAN_TYPE |= TTV_FIND_MASKNOT;
   }
   else
   {
      XtasParaParam->SCAN_TYPE &= ~(TTV_FIND_MASKNOT);
   }
  
   for ( i=0; i<6; i++ ) 
   {
     XtFree( tmp[i] );
   }

   XtasParaParam->CRITIC = 'P';

   XtasSetLabelString( XtasDeskMessageField, XTAS_PATINFO );
   XalSetCursor( XtasTopLevel, WAIT );
   XalForceUpdate( XtasTopLevel );

   if((unvalid_instance_mask) || ((window=XtasPathsList(XtasPathParaSet->TOP_LEVEL,
                                  XtasParaParam)) == NULL))
   {
    XalDrawMessage( XtasWarningWidget , XTAS_NPATWAR );
   }
   else
   { 
    XtasPathParaSet->PARA_SESSION = addchain(XtasPathParaSet->PARA_SESSION,window) ;
    ((XtasPathListSetStruct *)window->userdata)->PARENT = XtasPathParaSet ;
   }

  XtasSetLabelString( XtasDeskMessageField, XTAS_NULINFO );
  XalSetCursor( XtasTopLevel, NORMAL ); 
 // XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCancelGetParaPathsCallback                                 */
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
void XtasCancelGetParaPathsCallback( widget, client_data, call_data )
Widget    widget;
XtPointer   client_data;
XtPointer   call_data;
{
    XtUnmanageChild(widget);
  // XalLeaveLimitedLoop();
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasParaButtonCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data :                                 */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the para button command              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasParaButtonCallback( widget, client_data, call_data )
Widget    widget;
XtPointer   client_data;
XtPointer   call_data;
{
   int             n;
   Arg             args[10];
   XmString        text;
   Widget          form; 
   Widget          form_t; 
   Widget          dbase_form; 
   Widget          ebase_form; 
   Widget          fbase_form;
   Widget          row_widget; 
   Widget          frame_widget; 
   Widget          label_widget; 
   Widget          toggle;
   Widget          tmp_widget;
   Boolean         bool[10];
   XtasWindowStruct  *tas_winfos = (XtasWindowStruct *)client_data ;
   XtasPathListSetStruct *pathset = (XtasPathListSetStruct *)tas_winfos->userdata ;
   static XtasHierStruct hier ;

   Atom            WM_DELETE_WINDOW;

   if ( XtasAppNotReady() ) 
   {
      return;
   }

   if ( pathset->PARAM->LOOK_PATH == NULL )
   {
    XalDrawMessage( XtasWarningWidget, XTAS_NPATERR );
    return ;
   }

   if ( !XtasGetParaWidget )
   {
      if ( XTAS_AND == 'N' )  
      {
         bool[0]=FALSE; 
      }
      else  
      {
         bool[0]=TRUE;
      }
      if ( XTAS_OR  == 'N' )  
      {
         bool[1]=FALSE;
      } 
      else 
      { 
         bool[1]=TRUE;
      }
      if ( XTAS_NOT == 'N' )
      {
         bool[2]=FALSE;
      }
      else 
      {
         bool[2]=TRUE;
      } 
      if ( DUAL  == 'N' )
      {
         bool[6]=FALSE;
         bool[7]=TRUE;
      }
      else
      {
         bool[6]=TRUE;
         bool[7]=FALSE;
      }
      if ( MAX  == 'N' )
      {
         bool[4]=FALSE;
         bool[5]=TRUE;
      }
      else
      {
         bool[4]=TRUE;
         bool[5]=FALSE;
      }

/*--- creation d'un Prompt Dialog ---- generalites ----------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtitle, XTAS_NAME" : Get Parallel Paths..." ); n++;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;

    XtasGetParaWidget = XmCreatePromptDialog( XtasDeskMainForm, 
					     "XtasInfosBox", 
					     args, n);
	HelpFather = XtasGetParaWidget ;

    XtUnmanageChild(XmSelectionBoxGetChild(XtasGetParaWidget, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(XtasGetParaWidget, XmDIALOG_PROMPT_LABEL));

    XtAddCallback( XtasGetParaWidget, XmNokCallback, XtasGetParaPathsOkCallback, (XtPointer)XtasGetParaWidget );
    XtAddCallback( XtasGetParaWidget, XmNcancelCallback, XtasCancelGetParaPathsCallback, (XtPointer)XtasGetParaWidget );
    XtAddCallback( XtasGetParaWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_SRC_GETPARA|XTAS_HELP_MAIN) );
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(XtParent(XtasGetParaWidget), WM_DELETE_WINDOW, XtasCancelGetParaPathsCallback, (XtPointer )XtasGetParaWidget);
    XmAddTabGroup( XtasGetParaWidget ); 
    
    n = 0;
    form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, XtasGetParaWidget, args, n );
    
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
    
    text = XmStringCreateSimple( "On Path" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            70                ); n++;
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
    XTAS_GETPARAPATH_FIELD[0] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPARAPATH_FIELD[0] );
    XmTextSetString( XTAS_GETPARAPATH_FIELD[0], "*" );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPARAPATH_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPARAPATH_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPARAPATH_FIELD[0]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tmp_widget = XmCreateMenuBar( fbase_form, "XtasMenuBar", args, n );

    for(n = 0 ; XtasGetParaMenuView[n].label != NULL ; n++)
      XtasGetParaMenuView[n].callback_data += 30;

    XalBuildMenus( tmp_widget, XtasGetParaOnPathMenu );
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

    text = XmStringCreateSimple( "Paths" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            50                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, ebase_form, args, n );
    XmStringFree( text );
    
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,           label_widget   );             n++;
    XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg( args[n], XmNbottomWidget,        label_widget   );    n++;
    XtSetArg( args[n], XmNleftAttachment,      XmATTACH_WIDGET  );  n++;
    XtSetArg( args[n], XmNleftWidget,          label_widget   );    n++;
    XtSetArg( args[n], XmNrightAttachment,     XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,         2                 ); n++;
    XtSetArg( args[n], XmNleftOffset,          2                 ); n++;
    XtSetArg( args[n], XmNshadowThickness,     2                 ); n++;
    XTAS_GETPARAPATH_FIELD[1] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, ebase_form, args, n);
    XmAddTabGroup( XTAS_GETPARAPATH_FIELD[1] );
    XmTextSetString( XTAS_GETPARAPATH_FIELD[1], "10" );
/*----------------- 2 eme sub form -------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        form_t       );      n++;
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

    row_widget = XtVaCreateManagedWidget("XtasRow", 
					 xmRowColumnWidgetClass, 
					 fbase_form,
					 XmNtopAttachment,     XmATTACH_WIDGET,
					 XmNtopWidget,         frame_widget,   
					 XmNleftAttachment,    XmATTACH_FORM,
					 XmNleftOffset,        20,
					 /* XmNrightAttachment,   XmATTACH_FORM,*/
					 XmNbottomAttachment,  XmATTACH_FORM,
					 XmNpacking,           XmPACK_COLUMN,
					 XmNnumColumns,        2,
					 XmNorientation,       XmHORIZONTAL,
					 XmNisAligned,         True,
					 XmNradioBehavior,     TRUE,
					 XmNisHomogeneous,     TRUE,
				         XmNentryClass,xmToggleButtonWidgetClass,
					 NULL);
    XmAddTabGroup( row_widget );

    text = XmStringCreateSimple( "And" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", 
				      xmToggleButtonWidgetClass, 
				      row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           bool[0],
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParaParamAndCallback,
          &XTAS_AND );

    text = XmStringCreateSimple( "Or" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", 
				      xmToggleButtonWidgetClass, 
				      row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           bool[1],
                                      XmNlabelString,   text,
                                      NULL);
     XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParaParamOrCallback,
          &XTAS_OR );

    text = XmStringCreateSimple( "Not" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", 
				      xmToggleButtonWidgetClass, 
				      row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           bool[2],
                                      XmNlabelString,   text,
                                      NULL);

    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParaParamNotCallback,
          &XTAS_NOT );

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
                                      XmNset,           bool[4],
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParaParamMaxCallback, &MAX );

    text = XmStringCreateSimple( "Min" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           bool[5],
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParaParamMinCallback, &MAX );


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

    text = XmStringCreateSimple( "Search Direction" );
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

    text = XmStringCreateSimple( "From Start" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           bool[6],
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasParaParamDualCallback, &DUAL );

    text = XmStringCreateSimple( "From End" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           bool[7],
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    /* no callback for this Widget : OK ( radiobox ) */

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
    XTAS_GETPARAPATH_FIELD[2] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, dbase_form, args, n);
    XmAddTabGroup( XTAS_GETPARAPATH_FIELD[2] );
    XmTextSetString( XTAS_GETPARAPATH_FIELD[2] , "" );
 
    text = XmStringCreateSimple( "Upper Bound" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text                  ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPARAPATH_FIELD[2]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPARAPATH_FIELD[2]     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPARAPATH_FIELD[2]      ); n++;
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
    XTAS_GETPARAPATH_FIELD[3]  = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, dbase_form, args, n);
    XmAddTabGroup( XTAS_GETPARAPATH_FIELD[3] );
    XmTextSetString( XTAS_GETPARAPATH_FIELD[3] , "" );
    
    CRITIC='N';
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
/*    n = 0;
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
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopOffset,        7                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            110                ); n++;
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

    XTAS_GETPARAPATH_FIELD[4] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPARAPATH_FIELD[4] );
    XmTextSetString( XTAS_GETPARAPATH_FIELD[4], "" );

    text = XmStringCreateSimple( "Instance Name" );
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPARAPATH_FIELD[4]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPARAPATH_FIELD[4]     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPARAPATH_FIELD[4]      ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            110                ); n++;
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

    XTAS_GETPARAPATH_FIELD[5] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmAddTabGroup( XTAS_GETPARAPATH_FIELD[5] );
    XmTextSetString( XTAS_GETPARAPATH_FIELD[5], "" );

    text = XmStringCreateSimple( "List" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNtopWidget,             XTAS_GETPARAPATH_FIELD[5]     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,          XTAS_GETPARAPATH_FIELD[5]     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,           3                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            XTAS_GETPARAPATH_FIELD[5]     ); n++;
    XtSetArg( args[n], XmNshadowThickness,       2                 ); n++;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasSubTitles",xmPushButtonWidgetClass, fbase_form, args, n );
    XmStringFree( text );

    XtasParaParam    = XtasNewSearchPathParam( NULL );
    hier.instance = XTAS_GETPARAPATH_FIELD[5] ;
    hier.figure   = XTAS_GETPARAPATH_FIELD[4] ;
    hier.param = XtasParaParam->ROOT_SIG_EDGE ;

    XtAddCallback(tmp_widget, XmNactivateCallback, XtasHierarchicalViewCallback, &hier); 
  }

 XtasParaParam->ROOT_SIG_EDGE->TOP_FIG = pathset->PARAM->ROOT_SIG_EDGE->TOP_FIG ;
 XtasParaParam->ROOT_SIG_EDGE->LOCAL_FIG = NULL ;
 XtasParaParam->LOOK_PATH = pathset->PARAM->LOOK_PATH ;
 XtasParaParam->SCAN_TYPE = pathset->PARAM->SCAN_TYPE ;
 XtasPathParaSet = pathset ;

 XtManageChild( XtasGetParaWidget );
// XalLimitedLoop(XtasGetParaWidget);
}
