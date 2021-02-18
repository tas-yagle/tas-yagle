/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_desk.c                                                 */
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


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "Asim.h"
#ifdef AVERTEC
#include "Avertec.h"
#else
#include "Xtaslogo.h"
#endif
#include "xtas.h"
#include "xtas_desk_menu.h"
#include "xtas_desk_button.h"

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
Widget                  XtasDeskButtonsBarWidget;
Widget           	XtasDeskMessageField;
Widget                  XtasDataBaseName;

static   Widget    WMax_Line;
static   Widget    WMax_Sig;
static   int       XTAS_PREV_DISP = 0 ;

char XTAS_FORMAT_LOADED = XTAS_TTX;
int  XTAS_CTX_LOADED = XTAS_NOT_LOADED;

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

/*---------------------- call back  Set New / Textual Display ---------------*/
void XtasSetNewDisplayTypeCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 XTAS_DISPLAY_NEW = ( state->set ) ? 1 : 0 ;
}

/*----------------------  call back  Set Old / Graphical Display ------------*/
void XtasSetOldDisplayTypeCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 XTAS_DISPLAY_NEW = ( state->set ) ? 0 : 1 ;
}

/*----------------------  call back  cancel  Display Settings ---------------*/
void XtasDispCancelCallback( widget, root_widget, call_data )

 Widget  widget;
 XtPointer  root_widget;
 XtPointer call_data;

{
   XTAS_DISPLAY_NEW = XTAS_PREV_DISP ; 
   XtUnmanageChild( root_widget );
//   XalLeaveLimitedLoop();
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFileSaveCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/Save command.               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFileSaveCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFileExitCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/Exit  command.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFileExitCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XalExit( XtParent(widget) );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOpenDataBaseCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New/OpenDataBase command.   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOpenDataBaseCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasSelectFileCallback(widget,(XtPointer)XTAS_OPEN_MAP,call_data) ;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOpenDataBaseOk                                             */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New/OpenDataBase command.   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOpenDataBaseOk (void)
{
  char  text_title[64];
  Arg   args[10];
  int   n;
  char *p;
  FILE *ctxin;
  char *s1, *s2;
   
 if(XtasDataFileName != NULL)
  {
   XTAS_CTX_LOADED = XTAS_NOT_LOADED;
   XalSetCursor( XtasTopLevel, WAIT );
   if ((s1=strstr (XtasDataFileName, ".dtx")) != NULL) {
       if ((s2=strstr (XtasDataFileName, ".ttx")) != NULL) {
           if (s1 > s2)
               XTAS_FORMAT_LOADED = XTAS_DTX;
           else
               XTAS_FORMAT_LOADED = XTAS_TTX;
       }
       else
           XTAS_FORMAT_LOADED = XTAS_DTX;
   }
   else 
       XTAS_FORMAT_LOADED = XTAS_TTX;
      
   XtasRemovePathSession(NULL);
   XtasRemoveDetailPathSession(NULL) ;
   XtasRemoveStbSession(NULL) ;
   XtasSigsRemove();
   XTAS_SIMU_NEW_NETLIST = 'Y';
   XtUnmanageChild( XtasDataBaseRequester );
   if(XtasLoadTtvfig(XtasDataFileName) != NULL)
     {
         XtasSetLabelString(XtasDeskMessageField,
                         "New DataBase Successfully Loaded");
      sprintf(text_title,"Xtas Main Window - %s",
                          XtasMainParam->ttvfig->INFO->FIGNAME);
      n = 0;
      XtSetArg( args[n], XmNtitle, text_title ); n++;
      if(XTAS_FORMAT_LOADED == XTAS_DTX)  {
          if((ctxin = ttv_openfile(XtasMainParam->ttvfig,TTV_FILE_CTX,READ_TEXT))) {
               fclose(ctxin);
               XtasLoadCtx(XtasMainParam->ttvfig);
          }
      }
     }
    else
     {
      XalDrawMessage( XtasErrorWidget, "Unable To Open This DataBase");
      sprintf( text_title, "Xtas Main Window - No Current Data Base Loaded");
      n = 0;
      XtSetArg( args[n], XmNtitle, text_title ); n++;
     }
    XalSetCursor( XtasTopLevel, NORMAL );
    XtSetValues( XtasTopLevel, args, n);
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptionsButtonsCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Button's command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptionsButtonsCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
XtasOptionsButtonsTreat( XtasDeskMainForm, &XtasDeskButtonsBarWidget,
                         XtasDeskButtonMenus, XtasDeskButtonOptions );
}

/*---------------------------------------------------------------------------*/
/*                                                                       PSY */
/* FUNCTION : XtasMemoryOkCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Memory  command when     */
/*            the OK Button is selected.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasMemoryOkCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
   double   Sizes;
   double   Sizel;


   Sizes = atof( XmTextGetString( WMax_Sig  ) );
   Sizel = atof( XmTextGetString( WMax_Line ) );

   ttv_setcachesize( Sizes, Sizel );

//   XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                       PSY */
/* FUNCTION : XtasDisplayOkCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Display command when     */
/*            the OK Button is selected.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDisplayOkCallback( widget, client_data , call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
//   XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptionsDisplayTypeTreat                                    */
/*                                                                           */
/* IN  ARGS : .parent : parent widget                                        */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : build the Display type widget.                                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptionsDisplayTypeTreat( parent )

 Widget    parent;
{
   int       n;
   Arg       args[10];
   Widget    row_widget ;
   Widget    toggle ;
   Widget    prompt ;
   Widget    form   ;
   XmString  text;
   Boolean   toggle1 ;
   Boolean   toggle2 ;
   Atom      WM_DELETE_WINDOW;

   n = 0;
   XtSetArg( args[n], XmNtitle,  XTAS_NAME": Display Type Configuration" ); n++;
   XtSetArg( args[n], XmNdialogStyle,  XmDIALOG_APPLICATION_MODAL ); n++;
   prompt = XmCreatePromptDialog( parent,
                                  "XtasInfosBox",
                                  args, n );
   XtUnmanageChild(XmSelectionBoxGetChild(prompt, XmDIALOG_TEXT));
   XtUnmanageChild(XmSelectionBoxGetChild(prompt, XmDIALOG_PROMPT_LABEL));
   XtUnmanageChild(XmSelectionBoxGetChild(prompt, XmDIALOG_HELP_BUTTON));

   XtAddCallback(  prompt,
                   XmNokCallback,
                   XtasDisplayOkCallback,
                  (XtPointer)prompt );
   XtAddCallback(  prompt,
                   XmNcancelCallback,
                   XtasDispCancelCallback,
                  (XtPointer)prompt );
 
   WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
   XmAddWMProtocolCallback(XtParent(prompt), WM_DELETE_WINDOW, XtasDispCancelCallback, (XtPointer )prompt);

   n = 0;
   XtSetArg( args[n], XmNwidth,           250                ); n++;
   form = XmCreateForm(prompt, "XtasMainForm", args, n);
   XtManageChild( form );
   
    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, form,
                                           XmNtopAttachment,   XmATTACH_FORM,
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

    if ( XTAS_DISPLAY_NEW == 0 ) {
        toggle1 = True ;
        toggle2 = False ;
    }
    else {
        toggle2 = True ;
        toggle1 = False ;
    }
    XTAS_PREV_DISP = XTAS_DISPLAY_NEW ;

    text = XmStringCreateSimple( "Graphical Display" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", 
                                      xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           toggle1,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback ( toggle, 
                    XmNvalueChangedCallback, 
                    XtasSetOldDisplayTypeCallback, 
                   &XTAS_DISPLAY_NEW );

    text = XmStringCreateSimple( "Textual Display" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles",
                                      xmToggleButtonWidgetClass, 
                                      row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           toggle2,
                                      XmNlabelString,   text,
                                      NULL);
    XmStringFree( text );
    XtAddCallback ( toggle,
                    XmNvalueChangedCallback,
                    XtasSetNewDisplayTypeCallback,
                   &XTAS_DISPLAY_NEW );

   XtManageChild( prompt );
//   XalLimitedLoop( prompt );
}
    
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptionsDisplayTypeCallback                                 */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Display Type's command   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptionsDisplayTypeCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasOptionsDisplayTypeTreat( XtasDeskMainForm); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptionsMemorySizeTreat                                     */
/*                                                                           */
/* IN  ARGS : .parent : parent widget                                        */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : build the memory size widget.                                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptionsMemorySizeTreat( parent )

 Widget    parent;
{
   int       n;
   Arg       args[10];
   Widget    prompt;
   Widget    form;
   Widget    label_dms;
   Widget    label_sms;
   Widget    label_current;
   Widget    label_max;
   Widget    label_vide;
   Widget    WNumb_Sig;
   Widget    WNumb_Line;
   Widget    main_row;
            
   char      SNumb_Sig[16];
   char      SNumb_Line[16];
   char      SMax_Sig[16];
   char      SMax_Line[16];
   
   Atom     WM_DELETE_WINDOW;


   sprintf(SMax_Sig,"%g",(double)ttv_getsigcachesize());
   sprintf(SMax_Line,"%g",(double)ttv_getlinecachesize());

   sprintf(SNumb_Sig,"%g",(double)ttv_getsigmemorysize());
   sprintf(SNumb_Line,"%g",(double)ttv_getlinememorysize());

   n = 0;
   XtSetArg( args[n], XmNtitle,  XTAS_NAME": Memory Size Configuration" ); n++;
   XtSetArg( args[n], XmNdialogStyle,  XmDIALOG_APPLICATION_MODAL ); n++;
   prompt = XmCreatePromptDialog( parent,
                                  "XtasInfosBox",
                                  args, n );
   XtUnmanageChild(XmSelectionBoxGetChild(prompt, XmDIALOG_TEXT));
   XtUnmanageChild(XmSelectionBoxGetChild(prompt, XmDIALOG_PROMPT_LABEL));

   XtAddCallback(  prompt,
                   XmNokCallback,
                   XtasMemoryOkCallback,
                  (XtPointer)prompt );
   XtAddCallback(  prompt,
                   XmNcancelCallback,
                   XtasCancelCallback,
                  (XtPointer)prompt );
    
   WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
   XmAddWMProtocolCallback(XtParent(prompt), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )prompt);

   /*= Form form =*/
   n = 0;
   form = XmCreateForm(prompt, "XtasMainForm", args, n);
   XtManageChild( form );

   /*= Widget Row main_row =*/
   n = 0;
   XtSetArg( args[n], XmNpacking,        XmPACK_COLUMN   ); n++;
   XtSetArg( args[n], XmNisAligned,      True            ); n++;
   XtSetArg( args[n], XmNnumColumns,     3               ); n++;
   XtSetArg( args[n], XmNorientation,    XmHORIZONTAL    ); n++;
   XtSetArg( args[n], XmNentryAlignment, XmALIGNMENT_END ); n++;
   main_row = XtCreateManagedWidget( "XtasRow",
                                      xmRowColumnWidgetClass,
                                      form,
                                      args, n );

   /*= Gadget Label : label_vide =*/
   n = 0;
   label_vide = XtCreateManagedWidget( "XtasMainTitle",
                                        xmLabelGadgetClass,
                                        main_row,
                                        args, n );
   XtasSetLabelString( label_vide, "");
 
   /*= Gadget Label : label_max =*/
   n = 0;
   label_max = XtCreateManagedWidget( "XtasMainTitle",
                                       xmLabelGadgetClass,
                                       main_row,
                                       args, n );
   XtasSetLabelString( label_max, "Max");
   
   /*= Gadget Label : label_current =*/
   n = 0;
   label_current = XtCreateManagedWidget( "XtasMainTitle",
                                           xmLabelGadgetClass,
                                           main_row,
                                           args, n );
   XtasSetLabelString( label_current, "Current");

   /*= Gadget Label : label_dms =*/
   n = 0;
   label_dms = XtCreateManagedWidget( "XtasMainTitle",
                                       xmLabelGadgetClass, 
                                       main_row,
                                       args, n );
   XtasSetLabelString( label_dms, "Delay Memory Size (MB) :");


   /*= Widget TextField : WMax_Line=*/
   n = 0;
   WMax_Line= XtCreateManagedWidget( "XtasUserField",
                                      xmTextFieldWidgetClass, 
                                      main_row, 
                                        args, n );
   XmTextSetString( WMax_Line, SMax_Line );
   XtManageChild( WMax_Line );
 
   /*= Widget Text : WNumb_Line =*/
   n = 0;
   XtSetArg( args[n], XmNeditable, False ); n++;
   WNumb_Line = XtCreateManagedWidget( "UserField",
                                        xmTextWidgetClass, 
                                        main_row,
                                        args, n );
   XmTextSetString( WNumb_Line, SNumb_Line );

   /*= Gadget Label : label_sms =*/
   n = 0;
   label_sms = XtCreateManagedWidget( "XtasMainTitle",
                                       xmLabelGadgetClass,
                                       main_row,
                                       args, n );
   XtasSetLabelString( label_sms, "Signal Memory Size (MB) :");

   /*= Widget TextField WMax_Sig =*/
   n = 0;
   WMax_Sig = XtCreateManagedWidget( "XtasUserField",
                                      xmTextFieldWidgetClass,
                                      main_row,
                                      args, n );
   XmTextSetString( WMax_Sig, SMax_Sig );
   XtManageChild( WMax_Sig );

   /*= Widget TextField WNumb_Sig =*/
   n = 0;
   XtSetArg( args[n], XmNeditable, False ); n++;
   WNumb_Sig = XtCreateManagedWidget( "UserField",
                                       xmTextWidgetClass,
                                       main_row,
                                       args, n );
   XmTextSetString( WNumb_Sig, SNumb_Sig );


   XtManageChild( prompt );
//   XalLimitedLoop( prompt );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptionsMemorySizeCallback                                  */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Memory Size's command    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptionsMemorySizeCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasOptionsMemorySizeTreat( XtasDeskMainForm); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateMainMenus                                            */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the main window.                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateMainMenus( parent )
Widget parent;
{
Widget menu_bar;
Arg    args[20];
int    n;

/*-------------------*/
/* Create a menu_bar */
/*-------------------*/
n = 0;
menu_bar = XmCreateMenuBar( parent, "XtasMenuBar", args, n );

/*---------------------------------------------------------------------------*/
/* This function is called from the "libXal" library. It provides a simple   */
/* way to build the menu bar with a set of functions and submenus.           */
/* To use this function, you have to make up a set of XalMenus structure     */
/* as described in the XalBuildMenus manual.                                 */
/*---------------------------------------------------------------------------*/
XalBuildMenus( menu_bar,             /* The Menu Bar widget id               */
               XtasDeskMenu );      /* The main menu XalMenus structure     */

return( menu_bar );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateDesk                                                 */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation of the main window objects.                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateDesk( parent )
Widget parent;
{
Widget   label_widget, form_t;
Widget   form, frame;
Widget   separator1, separator2;
Arg      args[20];
int      n;

/*------------------------------------------------*/
/* Create button Menus                            */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM  ); n++;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM  ); n++;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM  ); n++;
form = XtCreateManagedWidget( "XtasMainButtons",
                               xmFormWidgetClass,
                               parent,
                               args, n );

n = 0;
XtSetArg( args[n], XmNnumColumns,      1             ); n++;
XtSetArg( args[n], XmNorientation,     XmHORIZONTAL  ); n++;

XtasDeskButtonsBarWidget = XalButtonMenus( form, XtasDeskButtonMenus, args, n, 40, 40 );
XtasAddDummyButton( XtasDeskButtonsBarWidget );

XtManageChild( XtasDeskButtonsBarWidget );
XmAddTabGroup( XtasDeskButtonsBarWidget );

/*------------------------------------------------*/
/* Create a separator                             */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET          ); n++;
XtSetArg( args[n], XmNtopWidget,       form                     ); n++;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM            ); n++;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM            ); n++;
XtSetArg( args[n], XmNleftOffset,      3		        ); n++;
XtSetArg( args[n], XmNrightOffset,     3		        ); n++;
separator1 = XtCreateManagedWidget( "XtasShellSubForm", xmSeparatorWidgetClass, parent, args, n );

/*-------------------------*/
/* Create the message area */
/*-------------------------*/
n = 0;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNleftOffset,       2               ); n++;
XtSetArg( args[n], XmNrightOffset,      2               ); n++;
XtSetArg( args[n], XmNtopOffset,        2		); n++;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, parent, args, n );

label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, form ,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_FORM,
                                        XmNleftOffset,      2,
                                        NULL);
XtasSetLabelString( label_widget, "Informations Area");

n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
XtSetArg( args[n], XmNrightPosition,         48                ); n++;
frame = XtCreateManagedWidget( "XtasFrame",  xmFrameWidgetClass, form, args, n );
n = 0;
XtSetArg( args[n], XmNheight,                20                ); n++;
label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, frame, args, n);
XtasDataBaseName = label_widget;
XtasSetLabelString( label_widget, XTAS_NULINFO);

n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,        frame                    ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM            ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ); n++;
XtSetArg( args[n], XmNleftWidget,       frame                    ); n++;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
n = 0;
XtSetArg( args[n], XmNheight,                20                ); n++;
label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, frame, args, n);
XtasDeskMessageField = label_widget;
XtasSetLabelString( label_widget, XTAS_NULINFO);

/*------------------------------------------------*/
/* Create a separator                             */
/*------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNbottomWidget,     form            ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNleftOffset,       3		); n++;
XtSetArg( args[n], XmNrightOffset,      3		); n++;
separator2 = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, parent, args, n );


/*----------------------------------------------------------------------*/
/* le contenu du bureau                                                */
/*--------------------------------------------------------------------*/
n = 0;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNtopWidget,        separator1      ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNbottomWidget,     separator2      ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, parent, args, n );

/*-------------------------------------*/
/* Create a logo to make things pretty */
/*-------------------------------------*/
n = 0;
XtSetArg( args[n], XmNshadowType, 	XmSHADOW_IN   ); n++;
XtSetArg( args[n], XmNborderWidth,      0             ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNleftOffset,       5             ); n++;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNtopOffset,        3             ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNbottomOffset,     3             ); n++;
form_t = XtCreateManagedWidget( "XtasLogoFrame", xmFrameWidgetClass, form, args, n );

n = 0;
label_widget = XmCreateLabel( form_t, "ColorXtasLogo", args, n);

#ifdef AVERTEC
label_widget = XalCreatePixmapLabel( label_widget, LogoAvertec, NULL );
#else
label_widget = XalCreatePixmapLabel( label_widget, XtasLogo, NULL );
#endif

XtManageChild( label_widget );

#ifndef AVERTEC
n = 0;
XtSetArg( args[n], XmNborderWidth,      0             ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
XtSetArg( args[n], XmNrightOffset,      5             ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
form_t = XtCreateManagedWidget( "AsimLogoForm", xmFormWidgetClass, form, args, n );

n = 0;
label_widget = XmCreateLabel( form_t, "ColorAsimLogo", args, n);
label_widget = XalCreatePixmapLabel( label_widget, LogoAsim, NULL );
XtManageChild( label_widget );
#endif

n = 0;
XtSetArg( args[n], XmNborderWidth,      0               ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
XtSetArg( args[n], XmNleftWidget,       form_t          ); n++;
XtSetArg( args[n], XmNleftOffset,       5               ); n++;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNtopOffset,        3               ); n++;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNbottomOffset,     3               ); n++;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
XtSetArg( args[n], XmNrightOffset,      3               ); n++;
form_t = XtCreateManagedWidget( "XtasVersionForm", xmFormWidgetClass, form, args, n );


return( parent );
}
