/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_flspaths.c                                             */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
/*                                                                          */
/*    Modified by : Mathieu OKUYAMA                   Date : 05/21/1998     */
/*                                                                          */
/*    Modified by : Stephane PICAULT                  Date : 10/13/1998     */
/*    Modified by : Antony PINTO                      Date : 13/09/2002     */
/*    Modified by : Caroline BLED                     Date : 04/11/2002     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"
#include ZEN_H
/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
        befig_list      *XtasBeFig = NULL;
        char             XTASFALS_SUPPRESS;
        char             XTASFALS_SAVEPATHTYPE;
static  char            *XTASFALS_FILENAME;
static  ptype_list      *XTASFALS_TASPATH;

/*}}}************************************************************************/
/*{{{                    XtasLoadBefig()                                    */
/*                                                                          */
/* chargement du comportement                                               */
/****************************************************************************/
befig_list *XtasLoadBefig(path_name)
char *path_name;
{
  befig_list    *befig = NULL;

  if(XtasMainParam->befig == NULL) initializeBdd(LARGE_BDD);

  zen_setDelay(1);

  XtasPasqua();

  if (sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
    befig = loadbefig(NULL,path_name,BVL_KEEPAUX);
    // = vhdlloadbefig( NULL, path_name, BVL_KEEPAUX) ;

    if ((befig!=NULL)&&(!beh_chkbefig(befig,0)))
    {
      beh_depend(befig);
      zen_makbehbdd(befig);
      //      beh_makbdd( befig );
    }
    else return NULL;

    if(XtasMainParam->befig != NULL)
    {
      beh_frebefig( XtasMainParam->befig );
      XtasMainParam->befig = befig ;
    }

    XtasGetWarningMess() ;
    /*if (befig)*/ XtasFirePasqua() ;

    return befig;
  }
  else return NULL;
}

/*}}}************************************************************************/
/*{{{                    XtasFalsLoadPath()                                 */
/*                                                                          */
/* chargement du chemin                                                     */
/****************************************************************************/
ptype_list *XtasFalsLoadPath(param,zenpath,taspath)
XtasSearchPathParamStruct   *param;
ptype_list                 **zenpath;
ptype_list                 **taspath;
{
  chain_list        *head       = NULL;
  ttvcritic_list    *pathList   = NULL;
  ptype_list        *zenpathLoc = NULL;
  ptype_list        *taspathLoc = NULL;
  long               MASK1, MASK2;
  ttvpath_list      *path       = param->LOOK_PATH;
  ttvfig_list       *fig        = param->ROOT_SIG_EDGE->TOP_FIG;

  if ((param->SCAN_TYPE & TTV_FIND_DUAL)==TTV_FIND_DUAL)
  {
    MASK1= path->TYPE | TTV_FIND_DUAL | TTV_FIND_LINE;
    MASK2= MASK1 & ~(TTV_FIND_PATH);
  }
  else
  {
    MASK1 = (path->TYPE|TTV_FIND_LINE)&~(TTV_FIND_DUAL);
    MASK2 = MASK1 & ~(TTV_FIND_PATH);
  }
  if ( param->CRITIC == 'P' )
  {
    head = ttv_getcriticpara(fig,path->FIG,path->ROOT,path->NODE,path->DELAY,MASK1,NULL,path->TTV_MORE_SEARCH_OPTIONS);
    if (head) pathList = head->DATA;
  }
  else
  {
    pathList = ttv_getcritic(fig,path->FIG,path->ROOT,path->NODE,path->LATCH,path->CMDLATCH, MASK2);
    head = addchain( head, pathList );
  }
  for (pathList=head->DATA;pathList;pathList=pathList->NEXT)
  {
    zenpathLoc = addptype(zenpathLoc,pathList->SNODE,zen_traductName(pathList->NAME));
    taspathLoc = addptype(taspathLoc,pathList->SNODE,pathList->NAME);
  } 
  *zenpath = zenpathLoc;
  *taspath = taspathLoc;
  return *zenpath;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsPrintPath(file,path,mode)
FILE        *file;
ptype_list  *path;
char         mode;
{
  if (path)
  {
    XtasFalsPrintPath(file,path->NEXT,(mode=='d')?'d':'n');
    if ((mode!='n')||(!path->NEXT))
      fprintf(file,"%s < %s > ",(char*)path->DATA,(path->TYPE=='U')?"UP":"DOWN");
  }
  else return;
}
/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsPrintFalsPath(filename,path,mode)
char        *filename;
ptype_list  *path;
char         mode;
{
  FILE      *file;
  char       buf[256];
  //int        existFalsePath = 0;

  sprintf(buf,"%s.inf",filename);
  file = fopen(buf,"a");
  
  fprintf(file,"\nFALSEPATH\n\n");
  XtasFalsPrintPath(file,path,mode);
  fprintf(file,";\n\nEND\n");
  
  fclose(file);
}

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

//void XtasFalsCancelCallback( widget, path_set, call_data )
//
// Widget                  widget;
// XtasPathListSetStruct  *path_set;
// XtPointer               call_data;
//
//{
//   XalLeaveLimitedLoop();
//}

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/
void XtasDisplayNewFlsPathResult(Widget list_widget, char *res)
{
    int             *pos_list, pos_count, oldTextLength, item_count;
    XmStringTable    item_table;
    XmString         oldItem, newItem;
    char            *oldText, newText[1024];
    
    
    pos_list = (int *)mbkalloc(sizeof(int));
    
    if(XmListGetSelectedPos(list_widget, &pos_list, &pos_count)) {
      XtVaGetValues(list_widget, XmNitems, &item_table, XmNitemCount, &item_count, NULL);
      oldItem = item_table[(*pos_list)-1]; 
      oldText = XmCvtXmStringToCT(oldItem);
      oldTextLength = strlen(oldText);
      
      if(*(oldText+oldTextLength-1) != 'e') {
        sprintf(newText,"%s  %s",oldText,res);
        newItem = XmStringCreateSimple(newText);
        XmListReplaceItemsPosUnselected(list_widget, &newItem, 1, *pos_list);
      }

      XtFree((char *) oldText);
      
      
    }

    mbkfree(pos_list);
    
            
}
/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/
void XtasDisplayFlsPathResultInList(Widget list_widget, char *res)
{
    XmString    text;
    Pixmap      pixmap;
    Pixel       tas_state_color, tas_backg_color;
    
    if(!XTAS_DISPLAY_NEW) {
         XtVaSetValues(XTAS_FLSPATHRESLABEL, XmNtopOffset, 3, 0);               
         XtVaGetValues(XTAS_FLSPATHRESLABEL,XmNforeground,&tas_state_color,
                   XmNbackground,&tas_backg_color,NULL );
         
         switch(*res) {
             case 'F' :  pixmap = XalGetPixmap(XTAS_FLSPATHRESLABEL , 
                                               XTAS_FALS_MAP, 
                                               tas_state_color, tas_backg_color );
                         
                         break;
    
             case 'T' :  pixmap = XalGetPixmap(XTAS_FLSPATHRESLABEL , 
                                               XTAS_TRUE_MAP, 
                                               tas_state_color, tas_backg_color );
                         break;
         }


                         
         if(pixmap == XmUNSPECIFIED_PIXMAP) {
                text = XmStringCreateSimple(res);
                XtVaSetValues(XTAS_FLSPATHRESLABEL, 
                                XmNlabelType, XmSTRING, 
                                XmNlabelString, text, 0);               
         }
         else
                XtVaSetValues(XTAS_FLSPATHRESLABEL,
                              XmNlabelType, XmPIXMAP,
                              XmNlabelPixmap, pixmap, 0);     
    }
    else
         XtasDisplayNewFlsPathResult(list_widget,res);
 
}




/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsHelpCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
   XtManageChild(XtasFrontWidget);
}



/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsResultOkCallback( widget, client_data , call_data )
Widget          widget;
XtPointer       client_data ;
XtPointer       call_data;
{
  if (XTASFALS_SUPPRESS != 'n')
    XtasFalsPrintFalsPath(XTASFALS_FILENAME, XTASFALS_TASPATH, XTASFALS_SAVEPATHTYPE);
//  XalLeaveLimitedLoop();
  return;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsResultHelpCallback( widget, client_data , call_data )
Widget          widget;
XtPointer       client_data ;
XtPointer       call_data;
{
  return;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
//void XtasFalsResultCancelCallback( widget, client_data , call_data )
//Widget          widget;
//XtPointer       client_data ;
//XtPointer       call_data;
//{
//  XTASFALS_SAVEPATHTYPE = 'n';
//  return;
//}


/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsSetSaveModeD()
{
  XTASFALS_SAVEPATHTYPE = 'd';
}
/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsSetSaveModeT()
{
  XTASFALS_SAVEPATHTYPE = 't';
}
/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsInit(char res)
{
    if(res == 'F')
        XTASFALS_SUPPRESS = 'y';
    else
        XTASFALS_SUPPRESS = 'n';
        
  XtasFalsSetSaveModeD();
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFlsPathsFocusCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    if ( !XTAS_DISPLAY_NEW )
   	    XtVaSetValues (widget, XmNcursorPositionVisible, True, NULL) ;
}


/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasLosingFlsPathsFocusCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
 	XtVaSetValues(widget, XmNcursorPositionVisible, False, NULL) ;
}

/******************************************************************************/

void XtasFalsPathSupressCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) {
     XtSetSensitive((Widget) client_data, True);
     XTASFALS_SUPPRESS = 'y';
 }
 
 else {
     XtSetSensitive((Widget) client_data, False);
     XTASFALS_SUPPRESS = 'n';
 }         
 
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasPathsFalsResultCallback(parent, resultString)
Widget   parent;
char    *resultString;
{
  static Widget   XtasFalsResultWidget = 0;
  void            XtasDummyCallback();
  XmString        text;
  Widget          form;
  Widget          dbase_form;
  Widget          ebase_form;
  Widget          fbase_form;
  Pixel           tas_state_color;
  Pixel           tas_backg_color;
  Widget          row_widget;
  Widget          label_widget;
  Widget          toggle;
  Pixmap          r_pixmap, f_pixmap, v_pixmap;
  Cardinal        n;
  Arg             args[10];
  Widget          result;
  ptype_list     *path;
  Atom            WM_DELETE_WINDOW;
  char            text_title[128];

  XtasFalsInit(*resultString);
  
  n = 0;
  XtSetArg( args[n], XmNwidth ,        420 ); n++;

  /*--- creation d'un Prompt Dialog ---- generalites -----------------*/





  sprintf (text_title, XTAS_NAME" : False Path Test - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
//  XtSetArg( args[n], XmNtitle, XTAS_NAME" : False Path Test" ); n++;
  XtSetArg( args[n], XmNtitle, text_title); n++;
  XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL); n++;

  XtasFalsResultWidget = XmCreatePromptDialog( parent,
      "XtasInfosBox",
      args, n);

  XtUnmanageChild( XmSelectionBoxGetChild( XtasFalsResultWidget,
      XmDIALOG_TEXT));

  XtUnmanageChild( XmSelectionBoxGetChild( XtasFalsResultWidget,
      XmDIALOG_PROMPT_LABEL));

  XtAddCallback(  XtasFalsResultWidget,
      XmNokCallback,
      XtasFalsResultOkCallback,
        NULL);
  XtAddCallback(  XtasFalsResultWidget,
      XmNcancelCallback,
      XtasCancelCallback,
        NULL);
  WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
  XmAddWMProtocolCallback(XtParent(XtasFalsResultWidget), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )XtasFalsResultWidget);
  
  XtUnmanageChild(XtNameToWidget( XtasFalsResultWidget,"Cancel"));
      

 /* pas de help pour l'instant mais penser au limited loop */
 /* XtAddCallback(  XtasFalsResultWidget,   
      XmNhelpCallback,
      XtasHelpCallback,
      (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_FLSPATHS)); */

//  XtAddCallback( XtasFalsResultWidget,
//      XmNcancelCallback,
//      XtasFalsResultCancelCallback,
//      (XtPointer)NULL );


  n = 0;
  form = XtCreateManagedWidget( "XtasDeskMainForm",
      xmFormWidgetClass,
      XtasFalsResultWidget,
      args, n );
  /*-------------------------------  1e sub form -------------------------------------------------------*/
  n = 0;
  ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n );

   XtVaGetValues(ebase_form,XmNforeground,&tas_state_color,
                              XmNbackground,&tas_backg_color,NULL );

   r_pixmap = XalGetPixmap( ebase_form, 
                           XTAS_REDG_MAP, 
                           tas_state_color, tas_backg_color );
   f_pixmap = XalGetPixmap( ebase_form, 
                           XTAS_FEDG_MAP,
                           tas_state_color, tas_backg_color );
   
  for(path = XTASFALS_TASPATH; path->NEXT!=NULL; path=path->NEXT); 
   v_pixmap = ( path->TYPE == 'U') ? r_pixmap : f_pixmap;

  text = XmStringCreateSimple( "PATH:   From " );
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM ) ; n++ ;
  XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM ) ; n++ ;
  XtSetArg( args[n], XmNlabelString,      text ); n++;
  label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, ebase_form, args, n );
  XmStringFree( text );

  
  label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                               xmLabelWidgetClass,
                                               ebase_form,
                                               XmNtopAttachment,  XmATTACH_FORM,
                                               XmNbottomAttachment,  XmATTACH_FORM,
                                               XmNlabelType,      XmPIXMAP,
                                               XmNlabelPixmap,    v_pixmap,
                                               XmNleftAttachment, XmATTACH_WIDGET,
                                               XmNleftWidget,     label_widget,
                                               XmNleftOffset,     7,
                                               XmNwidth,          20,
                                               XmNheight,         15,
                                              NULL);
  n = 0 ;
//  XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM   ) ; n++ ;
//  XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM   ) ; n++ ;
  XtSetArg( args[n], XmNleftAttachment,         XmATTACH_WIDGET ) ; n++ ;
  XtSetArg( args[n], XmNleftWidget,             label_widget    ) ; n++ ;
  XtSetArg( args[n], XmNleftOffset,             7               ) ; n++ ;
  XtSetArg( args[n], XmNwidth,                  95              ) ; n++ ;
  XtSetArg( args[n], XmNshadowThickness,        2               ) ; n++ ;
  XtSetArg( args[n], XmNeditable,               False           ) ; n++ ;
  XtSetArg( args[n], XmNcursorPositionVisible,  False           ) ; n++ ;
  XtSetArg( args[n], XmNalignment,              XmALIGNMENT_CENTER) ; n++ ;

  result = XtCreateManagedWidget( "XtasUserField", xmTextFieldWidgetClass, ebase_form, args, n) ;
//  //XmAddTabGroup( NBERROR ) ;
  XmTextSetString( result , (char *)path->DATA ) ;
  XtAddCallback( result, XmNfocusCallback, XtasFlsPathsFocusCallback, NULL); 
  XtAddCallback( result, XmNlosingFocusCallback, XtasLosingFlsPathsFocusCallback, NULL); 

         
  text = XmStringCreateSimple( "   To   " );
  n = 0 ;
  XtSetArg( args[n], XmNlabelString,      text ); n++;
  XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM ) ; n++ ;
  XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM ) ; n++ ;
  XtSetArg( args[n], XmNleftAttachment,         XmATTACH_WIDGET ) ; n++ ;
  XtSetArg( args[n], XmNleftWidget,             result    ) ; n++ ;
  label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, ebase_form, args, n );
  XmStringFree( text );

  v_pixmap = ( XTASFALS_TASPATH->TYPE == 'U') ? r_pixmap : f_pixmap;
  
  label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                               xmLabelWidgetClass,
                                               ebase_form,
                                               XmNtopAttachment,  XmATTACH_FORM,
                                               XmNbottomAttachment,  XmATTACH_FORM,
                                               XmNlabelType,      XmPIXMAP,
                                               XmNlabelPixmap,    v_pixmap,
                                               XmNleftAttachment, XmATTACH_WIDGET,
                                               XmNleftWidget,     label_widget,
                                               XmNleftOffset,     7,
                                               XmNwidth,          20,
                                               XmNheight,         15,
                                              NULL);

  
  n = 0 ;
  XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM   ) ; n++ ;
  XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM   ) ; n++ ;
  XtSetArg( args[n], XmNleftAttachment,         XmATTACH_WIDGET ) ; n++ ;
  XtSetArg( args[n], XmNleftWidget,             label_widget    ) ; n++ ;
  XtSetArg( args[n], XmNleftOffset,             7               ) ; n++ ;
  XtSetArg( args[n], XmNwidth,                  95              ) ; n++ ;
  XtSetArg( args[n], XmNshadowThickness,        2               ) ; n++ ;
  XtSetArg( args[n], XmNeditable,               False           ) ; n++ ;
  XtSetArg( args[n], XmNcursorPositionVisible,  False           ) ; n++ ;
  XtSetArg( args[n], XmNalignment,              XmALIGNMENT_CENTER) ; n++ ;

  result = XtCreateManagedWidget( "XtasUserField", xmTextFieldWidgetClass, ebase_form, args, n) ;
  //XmAddTabGroup( NBERROR ) ;
  XmTextSetString( result, (char *)XTASFALS_TASPATH->DATA ) ;

  XtAddCallback( result, XmNfocusCallback, XtasFlsPathsFocusCallback, NULL); 
  XtAddCallback( result, XmNlosingFocusCallback, XtasLosingFlsPathsFocusCallback, NULL); 

  




  
  /*-------------------------------  2e sub form -------------------------------------------------------*/
//  n = 0;
//  XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//  ebase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
//
//  n = 0;
//  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET     ); n++;
//  XtSetArg( args[n], XmNtopWidget    ,    ebase_form     ); n++;
//  XtSetArg( args[n], XmNtopOffset,    25    ); n++;
//  ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n );
//
//
//
//  /* affiche le resultat */         
//  n = 0;
//  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNleftOffset,       10                ); n++;
//  XtSetArg( args[n], XmNrightOffset,      20               ); n++;
//  XtSetArg( args[n], XmNwidth,            170               ); n++ ;
//  XtSetArg( args[n], XmNalignment,              XmALIGNMENT_END) ; n++ ;
//
//  text = XmStringCreateSimple( "RESULT:" );
//  XtSetArg( args[n], XmNlabelString,      text ); n++;
//  label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, ebase_form, args, n );
//  XmStringFree( text );
//
//  n = 0 ;
//  XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM   ) ; n++ ;
//  XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM   ) ; n++ ;
//  XtSetArg( args[n], XmNleftAttachment,         XmATTACH_WIDGET ) ; n++ ;
//  XtSetArg( args[n], XmNleftWidget,             label_widget    ) ; n++ ;
//  XtSetArg( args[n], XmNwidth,                  60              ) ; n++ ;
//  XtSetArg( args[n], XmNshadowThickness,        2               ) ; n++ ;
//  XtSetArg( args[n], XmNeditable,               False           ) ; n++ ;
//  XtSetArg( args[n], XmNcursorPositionVisible,  False           ) ; n++ ;
//  XtSetArg( args[n], XmNalignment,              XmALIGNMENT_CENTER) ; n++ ;
//
//  result = XtCreateManagedWidget( "XtasUserField", xmTextFieldWidgetClass, ebase_form, args, n) ;
//  //XmAddTabGroup( NBERROR ) ;
//  XmTextSetString( result ,resultString ) ;

  
  /*-------------------------------  horizontal separator    ----------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,    ebase_form   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    5   ) ; n++ ;
 //   XtSetArg( args[n], XmNbottomOffset, 25   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, form, args, n ) ;

   
 
  /*-------------------------------  3e sub form    -------------------------------------------------------*/
//  n = 0;
//  XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//  XtSetArg( args[n], XmNtopOffset,        10                ); n++;
//  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//  XtSetArg( args[n], XmNtopWidget,        ebase_form        ); n++;
//  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
//  dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
 // XtSetArg( args[n], XmNtopOffset,    25   ); n++;
  XtSetArg( args[n], XmNtopWidget,        ebase_form        ); n++;
  XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_FORM   ); n++;
  dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n );



  
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
  ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

  /* affiche le resultat */         
  n = 0;
  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNtopOffset,    20   ); n++;
  XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNwidth,            170               ); n++ ;
  XtSetArg( args[n], XmNalignment,        XmALIGNMENT_CENTER) ; n++ ;

  text = XmStringCreateSimple( "RESULT:" );
  XtSetArg( args[n], XmNlabelString,      text ); n++;
  label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, ebase_form, args, n );
  XmStringFree( text );

  n = 0 ;
  XtSetArg( args[n], XmNtopAttachment,         XmATTACH_WIDGET   ) ; n++ ;
  XtSetArg( args[n], XmNtopWidget,             label_widget    ) ; n++ ;
  XtSetArg( args[n], XmNtopOffset,             10    ) ; n++ ;
  XtSetArg( args[n], XmNleftAttachment,       XmATTACH_FORM  ) ; n++ ;
  XtSetArg( args[n], XmNleftOffset,            58              ) ; n++ ;
  XtSetArg( args[n], XmNwidth,                  60              ) ; n++ ;
  XtSetArg( args[n], XmNshadowThickness,        2               ) ; n++ ;
  XtSetArg( args[n], XmNeditable,               False           ) ; n++ ;
  XtSetArg( args[n], XmNcursorPositionVisible,  False           ) ; n++ ;

  result = XtCreateManagedWidget( "XtasUserField", xmTextFieldWidgetClass, ebase_form, args, n) ;
  //XmAddTabGroup( NBERROR ) ;
  XmTextSetString( result ,resultString ) ;

  /*-------------------------------  vertical separator    ----------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,  XmATTACH_FORM ) ; n++ ;
//    XtSetArg( args[n], XmNtopOffset,  10 ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment,  XmATTACH_FORM ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,        ebase_form   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,        25   ) ; n++ ;
 //   XtSetArg( args[n], XmNbottomOffset,     25   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n ) ;


  n = 0;
  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET     ); n++;
  XtSetArg( args[n], XmNleftWidget,        ebase_form        ); n++;
  XtSetArg( args[n], XmNtopAttachment,  XmATTACH_FORM     ); n++;
  XtSetArg( args[n], XmNrightAttachment,    XmATTACH_FORM   ); n++;
  XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
  fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );




  /*------------------------- Choice of suppression  ------------------------------------------------------*/

//  n = 0;
//  XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNtopOffset,        15     ); n++;
//  XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//  XtSetArg( args[n], XmNleftOffset,       15     ); n++;

  text = XmStringCreateSimple( "Suppress False Path from :" );
//  XtSetArg( args[n], XmNlabelString,      text ); n++;
//  label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, fbase_form, args, n );
  toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
                                      XmNtopAttachment, XmATTACH_FORM,
                                      XmNtopOffset,     15,
                                      XmNleftAttachment, XmATTACH_FORM,
                                      XmNrightAttachment, XmATTACH_FORM,
                                      XmNleftOffset,     15,
                                      XmNtopAttachment, XmATTACH_FORM,
                                      XmNset,           True,
                                      XmNlabelString,   text,
                                      NULL) ;
  XmStringFree( text );

  if (*resultString=='T')  {
      XtVaSetValues(toggle, XmNset, False, 0);
      XtSetSensitive(toggle,False);
  }
      
  row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
      XmNtopAttachment,   XmATTACH_WIDGET,
      XmNtopWidget,       toggle,
      XmNtopOffset,       5,
      XmNbottomAttachment,XmATTACH_FORM,
      XmNleftAttachment,  XmATTACH_FORM,
      XmNleftOffset,      20,
      XmNrightAttachment, XmATTACH_FORM,
      XmNpacking,         XmPACK_COLUMN,
      XmNnumColumns,      1,
      XmNorientation,     XmVERTICAL,
      XmNsensitive,       False,
      XmNisAligned,       True,
      XmNradioBehavior,   True,
      XmNradioAlwaysOne,  True,
      NULL);
  //XmAddTabGroup( row_widget );

  XtAddCallback( toggle, XmNvalueChangedCallback, XtasFalsPathSupressCallback, (XtPointer)row_widget ) ;

  text = XmStringCreateSimple( ".dtx" );
  toggle = XtVaCreateManagedWidget( "XtasSubTitles",
      xmToggleButtonWidgetClass, row_widget,
      XmNindicatorType, XmONE_OF_MANY,
      XmNset,           True,
      XmNlabelString,   text,
      NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, XtasFalsSetSaveModeD, NULL);
  XmStringFree( text );

  text = XmStringCreateSimple( ".ttx" );
  toggle = XtVaCreateManagedWidget( "XtasSubTitles",
      xmToggleButtonWidgetClass, row_widget,
      XmNindicatorType, XmONE_OF_MANY,
      XmNset,           False,
      XmNlabelString,   text,
      NULL);
  XtAddCallback(toggle, XmNvalueChangedCallback, XtasFalsSetSaveModeT, NULL);
  XmStringFree( text );

  if (*resultString=='T')  
    XtSetSensitive(row_widget,False);
  else  
    XtSetSensitive(row_widget,True);
  
  XtManageChild(XtasFalsResultWidget);
//  XalLimitedLoop(XtasFalsResultWidget);
}


/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasFalsOkCallback( widget, client_data , call_data )
Widget      widget;
XtPointer   client_data ;
XtPointer   call_data;
{
  XtasPathListSetStruct *path_set   = (XtasPathListSetStruct *)client_data ;
  ptype_list            *zenpath    = NULL;
  ptype_list            *taspath    = NULL;
  zenfig_list           *XtasZenFig = NULL;
  static char           *figName    = NULL;
  
  /* chargement de la figure vhd                                              */
  if ((figName!=XtasMainParam->ttvfig->INFO->FIGNAME))
  {
    XtasBeFig = NULL;
    if ((XtasBeFig= XtasLoadBefig(XtasMainParam->ttvfig->INFO->FIGNAME)))
    {
      figName = XtasMainParam->ttvfig->INFO->FIGNAME;
      zen_remplacebebux(XtasBeFig);
      zen_remplacebebus(XtasBeFig);
    }
  }
  
  XTASFALS_FILENAME = figName;
  /* recherche du chemin                                                       */
  XtasFalsLoadPath(path_set->PARAM,&zenpath,&taspath);
  XTASFALS_TASPATH = taspath;
    
  /* analyse du chemin                                                         */
  if (XtasBeFig)
  {
      XtasPasqua();

      if (sigsetjmp( XtasMyEnv , 1 ) == 0)
      {
          XtasZenFig = zen_createzenfig( XtasBeFig, zenpath );
          switch ( zen_algo1( XtasZenFig ) )
          {
              case 0 : XtasPathsFalsResultCallback(widget, "FALSE");
                       XtasDisplayFlsPathResultInList(path_set->PATHLIST_SCROLL,"False");
                       break;

              default: XtasPathsFalsResultCallback(widget, "TRUE");
                       XtasDisplayFlsPathResultInList(path_set->PATHLIST_SCROLL,"True");
                       freeptype(taspath);
                       break;
          }
          zen_freezenfig( XtasZenFig );
          XtasFirePasqua();
      }
      else XalDrawMessage( XtasWarningWidget , "Compatibility problem..." );
  }
  else XalDrawMessage( XtasWarningWidget , "Can't load Befig..." );
  freeptype(zenpath);

}


/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void XtasPathsFalsCallback( widget, client_data , call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
    static Widget   XtasWarningFalsePath = NULL;
    int             n;
    Arg             args[10];
    XmString        motif_string, textOk, textCancel;
    char            message[] = "This function is under development.\nUse with caution.";
    Atom            WM_DELETE_WINDOW;
  
    XtasWindowStruct      *tas_winfos = (XtasWindowStruct *)client_data ;
    XtasPathListSetStruct *path_set = (XtasPathListSetStruct *)tas_winfos->userdata ;
  
  if(path_set->PARAM->LOOK_PATH != NULL)  {
    if(!XtasWarningFalsePath) {
        motif_string = XmStringCreateLtoR(message, XmSTRING_DEFAULT_CHARSET);
        textOk = XmStringCreateSimple("   OK   ");
        textCancel = XmStringCreateSimple(" Cancel ");
        
        n = 0;
        XtSetArg( args[n], XmNmessageString,        motif_string            ); n++;
        XtSetArg( args[n], XmNtitle,                XTAS_NAME": Warning"   ); n++;
        XtSetArg( args[n], XmNokLabelString,        textOk                  ); n++;
        XtSetArg( args[n], XmNcancelLabelString,    textCancel              ); n++;
        XtSetArg( args[n], XmNdialogStyle,          XmDIALOG_APPLICATION_MODAL  ); n++;
        XtasWarningFalsePath = XmCreateWarningDialog(XtasMainWindow, "XtasWarningFalsePathWidget", args, n);
        
        XtUnmanageChild(XmMessageBoxGetChild(XtasWarningFalsePath, XmDIALOG_HELP_BUTTON));
        XmStringFree(motif_string);
        XmStringFree(textOk);
        XmStringFree(textCancel);

        XtAddCallback(XtasWarningFalsePath, XmNokCallback, XtasFalsOkCallback, (XtPointer)path_set);
        XtAddCallback(XtasWarningFalsePath, XmNcancelCallback, XtasCancelCallback, (XtPointer)XtasWarningFalsePath);
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasMainWindow), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasWarningFalsePath), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )XtasWarningFalsePath);
    }

    XtManageChild(XtasWarningFalsePath);
//    XalLimitedLoop(XtasWarningFalsePath);


  }
//      XtasFalsOkCallback(tas_winfos->topwidget,path_set,0);
  else 
      XalDrawMessage( XtasWarningWidget, XTAS_NPATERR );
  
}

