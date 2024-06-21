/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_hierarchical.c                                         */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Mathieu OKUYAMA                     Date : 06/22/1998     */
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
#include "xtas.h"

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
static Widget          XtasHierView = NULL ;
static chain_list     *XtasTtvFigList = NULL ;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasHViewCancelCallback                                        */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .type : Not signifiant                                         */
/*            .call_data : Not signifiant                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the hierarchical netlist view        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void  XtasHViewCancelCallback(parent, client_data, call_data)
Widget  parent;
XtPointer     client_data;
XtPointer call_data;
{
// XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasHViewOkCallback                                            */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .paths_set :                                                   */
/*            .cbs : Toolkit informations.                                   */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the hierarchical netlist view        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void  XtasHViewOkCallback(parent, client_data, call_data)
Widget  parent;
XtPointer client_data ;
XtPointer call_data ;
{
 XtasHierStruct *hier = (XtasHierStruct *)client_data ;
 XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data ;
 int  n;
 chain_list *head;

 head = XtasTtvFigList;

 if ( head->NEXT != NULL )
 for( n = 1; n < cbs->item_position ; n++ )
  {
   head = head->NEXT;
  } 

 hier->param->LOCAL_FIG = head->DATA;

 XmTextSetString(hier->instance,
                 hier->param->LOCAL_FIG->INSNAME);

 XmTextSetString(hier->figure,
                 hier->param->LOCAL_FIG->INFO->FIGNAME);

 XtUnmanageChild( XtasHierView );
// XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasHViewAllLevelCallback                                      */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .hier :                                                        */
/*            .call_data : Not signifiant                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the "All Level" Button               */
/*                                          hierarchical netlist view        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasHViewAllLevelCallback(parent, client_data , call_data)
Widget  parent;
XtPointer client_data ;
XtPointer call_data;
{
 XtasHierStruct *hier = (XtasHierStruct *)client_data ;
 XmTextSetString(hier->instance,"");
 XmTextSetString(hier->figure,"");
 hier->param->LOCAL_FIG = NULL ;
/* XalLeaveLimitedLoop();*/
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasHierarchicalViewCallback                                   */
/*                                                                           */
/* IN  ARGS : .parent : The widget on which the event occurs.                */
/*            .hier :                                                        */
/*            .call_data : Not signifiant                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the hierarchical netlist view        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasHierarchicalViewCallback( parent, client_data, call_data )
Widget  parent;
XtPointer client_data ;
XtPointer call_data;
{
 XtasHierStruct *hier = (XtasHierStruct *)client_data ;
 int             n;
 Arg             args[20];
 chain_list      *head;
 XmString        motif_string,text;
 int             position = 0;
 Widget          hview_form, label_widget;
 ttvfig_list    *instance;
 Widget          list_hview;
 Atom            WM_DELETE_WINDOW;
 char            text_title[128];

if(XtasHierView != NULL)
  XtDestroyWidget( XtasHierView );

if( XtasTtvFigList != NULL )
freechain( XtasTtvFigList );

XtasTtvFigList = NULL ;
XtasHierView = NULL ;

n = 0;
sprintf (text_title, "Hierarchy - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
//XtSetArg( args[n], XmNtitle, "Hierarchy" ); n++;
XtSetArg( args[n], XmNtitle, text_title ); n++;
XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
XtasHierView = XmCreatePromptDialog( parent, "XtasHexplorer", args, n);
XtUnmanageChild( XmSelectionBoxGetChild( XtasHierView, XmDIALOG_TEXT));
XtUnmanageChild( XmSelectionBoxGetChild( XtasHierView, XmDIALOG_PROMPT_LABEL)); 
XtVaSetValues(
	      XtasHierView,
	      XmNokLabelString, XmStringCreateLtoR("All Level", XmSTRING_DEFAULT_CHARSET ),
	      NULL
             );

HelpFather = XtasHierView ;
XtAddCallback( XtasHierView, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_SRC_HIER|XTAS_HELP_MAIN));
XtAddCallback( XtasHierView, XmNcancelCallback, XtasHViewCancelCallback, (XtPointer)XtasHierView );
XtAddCallback( XtasHierView, XmNokCallback, XtasHViewAllLevelCallback, (XtPointer)hier );

WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(XtasHierView), WM_DELETE_WINDOW, XtasHViewCancelCallback, (XtPointer )XtasHierView);


n = 0;
hview_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, XtasHierView, args, n );

/*-----------------------------------------------------*/
/*   affichage du nom du pere de la figure             */
/*-----------------------------------------------------*/
text = XmStringCreateSimple(hier->param->TOP_FIG->INFO->FIGNAME);
label_widget = XtVaCreateManagedWidget("XtasMainTitles",
                                        xmLabelWidgetClass,hview_form,
                                        XmNlabelString,     text,
                                        XmNtopAttachment,   XmATTACH_FORM,
                                        XmNleftAttachment,  XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,
                                        XmNtopOffset,       2,
                                        XmNrightOffset,     2,
                                        XmNleftOffset,      2,
                                        NULL);
XmStringFree( text );

n=0;
XtSetArg( args[n], XmNtopAttachment,          XmATTACH_WIDGET      ); n++;
XtSetArg( args[n], XmNtopWidget,              label_widget         ); n++;
XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
XtSetArg( args[n], XmNvisibleItemCount,       15                   ); n++;
list_hview = XmCreateScrolledList( hview_form, "XtasUserField", args, n );
/*XtAddCallback(list_hview, XmNdefaultActionCallback, XtasHViewOkCallback , (XtPointer)hier);*/
XtAddCallback(list_hview, XmNbrowseSelectionCallback, XtasHViewOkCallback , (XtPointer)hier);


XmAddTabGroup( list_hview );
XtManageChild( list_hview );

XtManageChild( XtasHierView );
XmListDeleteAllItems( list_hview );

XtasTtvFigList = ttv_getttvfiglist( hier->param->TOP_FIG );
head = XtasTtvFigList;

while(head != NULL)
   {
    char buf[1024] ;
    instance = head->DATA;
//    strcpy(buf, instance->INFO->FIGNAME);
//    if(instance != hier->param->TOP_FIG)
//    {
//        strcat(buf," ");
//        strcat(buf, instance->INFO->FIGNAME);
//    }
        
//   ttv_getinsname(XtasTtvFigList, buf, instance);
    ttv_getinsname(hier->param->TOP_FIG, buf, instance);
    motif_string = XmStringCreateLtoR(buf,XmSTRING_DEFAULT_CHARSET); 
    XmListAddItemUnselected(list_hview,motif_string,++position);
    XmStringFree(motif_string);
    head = head->NEXT;
   }

//XalLimitedLoop(XtasHierView) ;
}
