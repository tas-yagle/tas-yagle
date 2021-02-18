/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_stb_pbar.c                                             */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"



Widget XtasStbInProgressWidget = NULL ;

void XtasCancelStbInProgress( widget, client_data, call_data )
Widget  widget ;
XtPointer client_data ;
XtPointer call_data ;
{
    printf("Cancel !\n");
//    XalLeaveLimitedLoop() ;
}





/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbAnalysisCallback                                        */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for stb analysis                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStbInProgress( itnum, max, current, mess)
int itnum;
int max;
int current;
char *mess;
{
  static      Widget XtasStbInProgressItNum  = NULL ;
  static      Widget XtasStbInProgressScale  = NULL ;
  static      Widget XtasStbInProgressPercent= NULL ;
  static      Widget XtasStbInProgressLabel  = NULL ; 
  
  int      n;
  int   percent;
  float r;
  Arg      args[20];
  XmString  text, textLabel, textItNum, textPercent;
  Widget    form, subform, label, frame;
  char it[3], pstr[4];
  static int lastpercent = -1;
  static int lastitnum = -1;
  
   if(!XtasStbInProgressWidget) {
       lastpercent = -1;
       lastitnum = -1;
   }

   r= ((float)current)/max;
   r= r*100;
   percent = (int)r;
   if(percent == lastpercent)
       return;
   lastpercent = percent;
  sprintf(it, "%d", itnum);
  sprintf(pstr, "%d", percent);

  if((itnum==0)&&(lastitnum > 0))
    textItNum = XmStringCreateSimple("-") ;
  else
    textItNum = XmStringCreateSimple(it) ;
  textPercent = XmStringCreateSimple(pstr) ;
  textLabel = XmStringCreateSimple( "" ) ;

  if (mess)
      textLabel = XmStringCreateSimple(mess);

  if( !XtasStbInProgressWidget )
   {
//    text = XmStringCreateSimple( "Stability analysis with crosstalk in progress..." ) ;
    text = XmStringCreateSimple( "Analysis in progress..." ) ;
    n = 0 ;
//    XtSetArg( args[n], XmNtitle, XTAS_NAME" : Stability Analysis " ) ; n++ ;
    XtSetArg( args[n], XmNtitle, XTAS_NAME" : Analysis " ) ; n++ ;
    XtSetArg( args[n], XmNmessageString, text ) ; n++ ;
    XtSetArg( args[n], XmNwidth, 350) ; n++ ;
    XtSetArg( args[n], XmNnoResize, True) ; n++ ;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL) ; n++ ;
    XtasStbInProgressWidget = XmCreateWorkingDialog(XtasDeskMainForm, "XtasInfosBox",args, n) ;
	HelpFather = XtasStbInProgressWidget ;
    XtUnmanageChild (XtNameToWidget (XtasStbInProgressWidget,"Help"));
    XtUnmanageChild (XtNameToWidget (XtasStbInProgressWidget,"Cancel"));
    XtUnmanageChild (XtNameToWidget (XtasStbInProgressWidget,"OK"));
    XtUnmanageChild (XmMessageBoxGetChild (XtasStbInProgressWidget,XmDIALOG_SEPARATOR));
    //XmAddTabGroup( XtasStbInProgressWidget ) ;
    XtAddCallback( XtasStbInProgressWidget, XmNcancelCallback, XtasCancelStbInProgress, (XtPointer)XtasStbInProgressWidget ) ;
    XmStringFree(text);

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, XtasStbInProgressWidget, args, n ) ;
  
/*----------------------------------------------------------------------------------------*/
/*                              form 0: label for message                                 */
/*----------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_CENTER) ; n++ ;
    XtSetArg( args[n], XmNlabelString,      textLabel     ) ; n++ ;
    XtasStbInProgressLabel = XtCreateManagedWidget( "XtasMainTitles", xmLabelGadgetClass, form, args, n ) ;


    
/*----------------------------------------------------------------------------------------*/
/*                              form 1: Iteration number                                  */
/*----------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        XtasStbInProgressLabel) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       128               ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM    ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      127               ) ; n++ ;
    frame = XtCreateManagedWidget( "XtasTitleForm", xmFrameWidgetClass, form, args, n ) ;

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    subform = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, frame, args, n ) ;


    text = XmStringCreateSimple( "Iteration " ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_END) ; n++ ;
    XtSetArg( args[n], XmNlabelString,      text          ) ; n++ ;
    label = XtCreateManagedWidget( "XtasMainTitles", xmLabelGadgetClass, subform, args, n ) ;
    XmStringFree( text ) ;

    
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       label           ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    XtSetArg( args[n], XmNlabelString,      textItNum       ) ; n++ ;
    XtasStbInProgressItNum = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, subform, args, n ) ;
//    XmStringFree( textItNum ) ;

/*----------------------------------------------------------------------------------------*/
/*                              form 2: bar                                               */
/*----------------------------------------------------------------------------------------*/
 
//    row = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, form,
//                                           XmNtopAttachment,   XmATTACH_WIDGET,
//                                           XmNtopWidget,       frame,
//                                           XmNtopOffset,       25,   
//                                           XmNleftAttachment,  XmATTACH_FORM,
//                                           XmNrightAttachment,  XmATTACH_FORM,
//                                           XmNbottomAttachment,  XmATTACH_FORM,
//                                           XmNpacking,         XmPACK_COLUMN,
//                                           //XmNspacing,         0,
//                                           //XmNmarginWidth,         0,
//                                           XmNnumColumns,      2,
//                                           XmNorientation,     XmVERTICAL,
//                                           NULL) ;
//       
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                   ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET     ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        frame               ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        25                  ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset,     10                  ) ; n++ ;
    subform = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n ) ;
    
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_END ) ; n++ ;
    XtSetArg( args[n], XmNlabelString,      textPercent     ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            30              ) ; n++ ;
    XtasStbInProgressPercent = XtCreateManagedWidget( "XtasMainTitles", xmLabelGadgetClass,subform, args, n ) ;

    text = XmStringCreateSimple( "%" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       XtasStbInProgressPercent ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    XtSetArg( args[n], XmNlabelString,      text            ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            20             ) ; n++ ;
    label = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, subform, args, n ) ;
    XmStringFree( text ) ;

   
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,        XmATTACH_FORM           ); n++;
    XtSetArg (args[n], XmNrightAttachment,      XmATTACH_FORM           ); n++;
    XtSetArg (args[n], XmNbottomAttachment,     XmATTACH_FORM           ); n++;
    XtSetArg (args[n], XmNleftAttachment,       XmATTACH_POSITION       ); n++;
    XtSetArg (args[n], XmNleftPosition,         25                      ); n++;
    XtSetArg (args[n], XmNminimum,              0                       ); n++;
    XtSetArg (args[n], XmNmaximum,              100                     ); n++;
    XtSetArg (args[n], XmNvalue,                percent                 ); n++;
    XtSetArg (args[n], XmNhighlightThickness,   0                       ); n++;
    XtSetArg (args[n], XmNorientation,          XmHORIZONTAL            ); n++;
#if XTAS_MOTIF_VERSION >= 10206
    XtSetArg (args[n], XmNslidingMode,          XmTHERMOMETER           ); n++;
#endif
#if XTAS_MOTIF_VERSION >= 20000
    XtSetArg (args[n], XmNsliderVisual,         XmSHADOWED_BACKGROUND   ); n++;
#endif
    XtSetArg (args[n], XmNshowValue,            XmNONE                  ); n++;
    XtasStbInProgressScale = XmCreateScale (subform, "load", args, n);
    XtManageChild (XtasStbInProgressScale) ;

/*----------------------------------------------------------------------------------------*/
/*                              form 3: progress                                          */
/*----------------------------------------------------------------------------------------*/
  XtManageChild( XtasStbInProgressWidget ) ;
   
   }

  else {
      if(mess)
        XtVaSetValues(XtasStbInProgressLabel, XmNlabelString, textLabel, NULL);
  }

  XtVaSetValues(XtasStbInProgressScale, XmNvalue, percent, NULL);

  if(itnum != lastitnum) 
    XtVaSetValues(XtasStbInProgressItNum, XmNlabelString, textItNum, NULL);
  lastitnum = itnum;
 
  XtVaSetValues(XtasStbInProgressPercent, XmNlabelString, textPercent, NULL);
  XtVaSetValues(XtasStbInProgressPercent, XmNwidth, 30, NULL);
  
  XmStringFree(textLabel);
  XmStringFree(textItNum);
  XmStringFree(textPercent);

  //  XtRealizeWidget (XtasStbInProgressWidget);
  XalForceUpdate(XtasStbInProgressWidget) ;
}



void XtasDestroyStbInProgress (void)
{
   if(XtasStbInProgressWidget)
     XtDestroyWidget(XtasStbInProgressWidget) ;
   XtasStbInProgressWidget = NULL;
}    
