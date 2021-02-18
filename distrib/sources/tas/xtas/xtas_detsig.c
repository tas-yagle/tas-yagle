/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_detsig.c                                               */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
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

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*         WORK VARIABLES                                                   */
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/* function   : name                                                        */
/* parametres :                                                             */
/*                                                                          */ 
/*                                                                          */ 
/*                                                                          */ 
/*                                                                          */ 
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
char *XtasSignalDirection(ptsig)
ttvsig_list *ptsig ;
{
 long type ;

 type = ptsig->TYPE & TTV_SIG_TYPECON ;

 switch(type)
    {
     case TTV_SIG_CI :  return("input") ;
     case TTV_SIG_CO :  return("output") ;
     case TTV_SIG_CZ :  return("tristate") ;
     case TTV_SIG_CB :  return("output") ;
     case TTV_SIG_CT :  return("bus") ;
     case TTV_SIG_CX :  return("unknown") ;
    }
 return(NULL) ;
}

/*--------------------------------------------------------------------------*/
char *XtasSignalsType(ptsig)
ttvsig_list *ptsig ;
{
 if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
  {
   if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
     return("connector command") ;
   else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
     return("connector precharge") ;
   else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
     return("connector break point") ;
   else
     return("connector") ;
  }
 if((ptsig->TYPE & TTV_SIG_N) == TTV_SIG_N)
  {
   if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
     return("internal connector command") ;
   else if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
     return("internal connector precharge") ;
   else if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
     return("internal connector break point") ;
   else
     return("internal connector") ;
  }
 if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
  {
   return("command") ;
  }
 if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
  {
   if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
     return("latch") ;
   else
     return("flip-flop") ;
  }
 if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
  {
   return("precharge") ;
  }
 if((ptsig->TYPE & TTV_SIG_I) == TTV_SIG_I)
  {
   return("factorisation signal") ;
  }
 if((ptsig->TYPE & TTV_SIG_S) == TTV_SIG_S)
  {
   return("internal signal") ;
  }
 if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
  {
   return("break point") ;
  }
 return(NULL) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsDetailOkCallback                                    */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .client_data :                                                 */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the hierarchical netlist view        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void  XtasSignalsDetailOkCallback(parent, client_data, call_data)
Widget  parent;
XtPointer client_data ;
XtPointer call_data ;
{
 XtDestroyWidget( (Widget)client_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSignalsDetail                                              */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .item   : The signal name.                                     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The drawing  function for signals detail                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalsDetail( parent, item )
Widget  parent;
char    *item;
{
int             n ;
int             w ;
int             wx ;
Arg             args[20];
XmString        text;
Widget          form, label_widget ;
Widget          detail, tmp_widget ;
ttvsig_list    *ptsig = NULL ;
chain_list     *chain = NULL ;
chain_list     *chainx ;
char            buf[2048] ;
char            buf2[2048] ;
char           *direction = NULL ;

w = strlen(item) ;

XtasPasqua();

if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
        ptsig = ttv_getsig(XtasMainParam->ttvfig,item) ;
        if(!ptsig) {
            for(chainx = XtasMainParam->ttvfig->INS ; !ptsig && chainx ; chainx = chainx->NEXT)
                ptsig = ttv_getsig((ttvfig_list*)chainx->DATA, item);
        }
  }
 else
  return ;

XtasFirePasqua();

if(ptsig != NULL)
 {
  chain = ttv_getlrcmd(XtasMainParam->ttvfig,ptsig) ;
  if((wx = strlen(XtasSignalsType(ptsig))) > w)
    w = wx ;
  if((wx = strlen(ptsig->ROOT->INFO->FIGNAME)) > w)
    w = wx ;
  if((wx = strlen(ptsig->ROOT->INSNAME)) > w)
    w = wx ;
  if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
    direction = XtasSignalDirection(ptsig) ;
 }

for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
  {
   ttv_getsigname(XtasMainParam->ttvfig,buf,((ttvevent_list *)chainx->DATA)->ROOT) ;
   if((wx = strlen(buf)) > w)
    w = wx ;
  }

if(w < 15)
  w = 18 ;
else
  w += 3 ;

n = 0;
XtSetArg( args[n], XmNtitle, XTAS_NAME": Signals Infos" ); n++;
detail = XmCreatePromptDialog( parent, "XtasInfosBox", args, n);
HelpFather = detail ;
XtUnmanageChild( XmSelectionBoxGetChild( detail, XmDIALOG_TEXT));
XtUnmanageChild( XmSelectionBoxGetChild( detail, XmDIALOG_PROMPT_LABEL));
XtUnmanageChild( XmSelectionBoxGetChild( detail, XmDIALOG_HELP_BUTTON ));
XtUnmanageChild( XmSelectionBoxGetChild( detail, XmDIALOG_CANCEL_BUTTON ));
XtAddCallback( detail, XmNokCallback, XtasSignalsDetailOkCallback, (XtPointer)detail );


n = 0;
form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, detail, args, n );

text = XmStringCreateSimple( "Signal" );
n = 0;
XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END ); n++;
XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
XtSetArg( args[n], XmNlabelString,      text              ); n++;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
XtSetArg( args[n], XmNleftOffset,        8                ); n++;
XtSetArg( args[n], XmNheight,           35                ); n++;
XtSetArg( args[n], XmNwidth ,           90                ); n++;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
XmStringFree( text );

n = 0;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
XtSetArg( args[n], XmNeditable,              False             ); n++;
tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
XmTextSetString( tmp_widget, item );

if(ptsig != NULL)
 {

  text = XmStringCreateSimple( "Net" );
  n = 0;
  XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END ); n++;
  XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
  XtSetArg( args[n], XmNheight,           35                ); n++;
  XtSetArg( args[n], XmNwidth ,           90                ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
  XmStringFree( text );
 
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
  XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
  XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
  XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
  XtSetArg( args[n], XmNeditable,              False             ); n++;
  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
  XmTextSetString( tmp_widget, ptsig->NETNAME ); 

  text = XmStringCreateSimple( "Capa" );
  n = 0;
  XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END ); n++;
  XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
  XtSetArg( args[n], XmNheight,           35                ); n++;
  XtSetArg( args[n], XmNwidth ,           90                ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
  XmStringFree( text );
 
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
  XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
  XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
  XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
  XtSetArg( args[n], XmNeditable,              False             ); n++;
  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
  sprintf (buf, "%g %s", ptsig->CAPA/1000.0,"pF") ;	
  XmTextSetString( tmp_widget, buf); 
  
  text = XmStringCreateSimple( "Type" );

  n = 0;
  XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END   ); n++;
  XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
  XtSetArg( args[n], XmNheight,           35                ); n++;
  XtSetArg( args[n], XmNwidth ,           90                ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
  XmStringFree( text );
  
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
  XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
  XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
  XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
  XtSetArg( args[n], XmNeditable,              False             ); n++;
  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
  XmTextSetString( tmp_widget, XtasSignalsType(ptsig) );
  
  if(direction != NULL)
  {
  text = XmStringCreateSimple( "Direction" );

  n = 0;
  XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END   ); n++;
  XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
  XtSetArg( args[n], XmNheight,           35                ); n++;
  XtSetArg( args[n], XmNwidth ,           90                ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
  XmStringFree( text );
  
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
  XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
  XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
  XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
  XtSetArg( args[n], XmNeditable,              False             ); n++;
  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
  XmTextSetString( tmp_widget, direction);
  }

  text = XmStringCreateSimple( "Figure" );

  n = 0;
  XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END   ); n++;
  XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
  XtSetArg( args[n], XmNheight,           35                ); n++;
  XtSetArg( args[n], XmNwidth ,           90                ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
  XmStringFree( text );
  
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
  XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
  XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
  XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
  XtSetArg( args[n], XmNeditable,              False             ); n++;
  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
  XmTextSetString( tmp_widget, ptsig->ROOT->INFO->FIGNAME );
  
  text = XmStringCreateSimple( "Instance" );

  n = 0;
  XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END   ); n++;
  XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
  XtSetArg( args[n], XmNlabelString,      text              ); n++;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
  XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
  XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
  XtSetArg( args[n], XmNheight,           35                ); n++;
  XtSetArg( args[n], XmNwidth ,           90                ); n++;
  label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
  XmStringFree( text );
  
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET); n++;
  XtSetArg( args[n], XmNtopWidget,      label_widget             ); n++;
  XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET ); n++;
  XtSetArg( args[n], XmNbottomWidget,    label_widget             ); n++;
  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
  XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
  XtSetArg( args[n], XmNeditable,              False             ); n++;
  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
  XmTextSetString( tmp_widget, ptsig->ROOT->INSNAME );
  
  if(chain != NULL)
    {
     for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
      {
       text = XmStringCreateSimple( "Command" );

       n = 0;
       XtSetArg( args[n], XmNalignment ,       XmALIGNMENT_END   ); n++;
       XtSetArg( args[n], XmNmarginRight ,     8                 ); n++;
       XtSetArg( args[n], XmNlabelString,      text              ); n++;
       XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
       XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
       XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
       XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
       XtSetArg( args[n], XmNrecomputeSize,     False            ); n++;
       XtSetArg( args[n], XmNleftOffset,        8                 ); n++;
       XtSetArg( args[n], XmNheight,           35                ); n++;
       XtSetArg( args[n], XmNwidth ,           90                ); n++;
       label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, form, args, n );
       XmStringFree( text );
 
       n = 0;
       XtSetArg( args[n], XmNtopAttachment,         XmATTACH_OPPOSITE_WIDGET); n++;
       XtSetArg( args[n], XmNtopWidget,             label_widget             ); n++;
       XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_OPPOSITE_WIDGET ); n++;
       XtSetArg( args[n], XmNbottomWidget,          label_widget             ); n++;
       XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
       XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
       XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
       XtSetArg( args[n], XmNcolumns ,              w                 ); n++;
       XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
       XtSetArg( args[n], XmNeditable,              False             ); n++;
       tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, form, args, n);
	   

	   ttv_getsigname(XtasMainParam->ttvfig, buf, ((ttvevent_list *)chainx->DATA)->ROOT);
	   
	   if ((((ttvevent_list *)chainx->DATA)->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
       		sprintf (buf2, "%s (high opens)", buf) ;	
	   else if ((((ttvevent_list *)chainx->DATA)->TYPE & TTV_NODE_DOWN) == TTV_NODE_DOWN)
       		sprintf (buf2, "%s (low opens)", buf) ;	
       XmTextSetString(tmp_widget, buf2);
      }
     freechain(chain) ;
    }    
 }

XtManageChild( detail );
}
