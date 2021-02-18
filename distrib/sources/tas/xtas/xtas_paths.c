/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_paths.c                                                */
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
#include "xtas_paths_menu.h"
#include "xtas_paths_button.h"



#define  XTAS_PATH_FROM_LOFFSET_G  54
#define  XTAS_PATH_FROM_LOFFSET_T  6

/*--------------------------------------------------------------------------*/
/*                             DEFINE                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
chain_list *XTAS_PATH_SESSION = NULL;
char       *XTAS_SIGNAL       = NULL;
Widget      XTAS_FLSPATHRESLABEL = NULL;
Widget      XTAS_PATHSSELECTEDWIDGET = NULL;
XtasPathListSetStruct *XtasPathsSavePathset = NULL;

/* Pathlist variables */
static int      XtasCarLength1 ;    /* max length of the name of the input  */
static int      XtasCarLength2 ;    /* max length of the name of the output */
/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasFillPathInfosForm (FocusSet)
 	XtasPathIndexTabElt  *FocusSet;
{
    Widget label;
    ttvpath_list    *path;
    char             buf [1024];

    path = FocusSet->PATH;
    /* From*/
    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosFromLabel");
    if (!label) 
        return;
    sprintf (buf, "%s", path->NODE->ROOT->NAME);
    XmTextSetString (label, buf);
    /* To*/
    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosToLabel");
    if (!label) 
        return;
    if (path->CMD)
        sprintf (buf, "%s commanded by %s", path->ROOT->ROOT->NAME, path->CMD->ROOT->NAME);
    else
        sprintf (buf, "%s", path->ROOT->ROOT->NAME);
    XmTextSetString (label, buf);
    XmUpdateDisplay (label);
    XmUpdateDisplay (XtParent(label));
    /* Starting Time*/
    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosSTimeLabel");
    if (!label) 
        return;
    sprintf (buf, "%.1f ps", path->DELAYSTART/TTV_UNIT);
    XtasSetLabelString (label, buf);
    /* Starting Slope */
    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosSSlopeLabel");
    if (!label) 
        return;
    sprintf (buf, "%.1f ps", path->SLOPESTART/TTV_UNIT);
    XtasSetLabelString (label, buf);
    /* DataLag */
    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosDataLagLabel");
    if (!label) 
        return;
    sprintf (buf, "%.1f ps", path->DATADELAY/TTV_UNIT);
    XtasSetLabelString (label, buf);
    /* Latch */
    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosLatchLabel");
    if (!label) 
        return;
    if (path->LATCH) {
        if (path->CMDLATCH)
            if (path->LATCH->ROOT == path->CMDLATCH->ROOT)
                sprintf (buf, "Prech: %s", path->LATCH->ROOT->NAME);
            else
                sprintf (buf, "Latch: %s commanded by %s", path->LATCH->ROOT->NAME, path->CMDLATCH->ROOT->NAME);
        else
            sprintf (buf, "Latch: %s",  path->LATCH->ROOT->NAME);
        XtasSetLabelString (label, buf);
    }
    else {   
        sprintf (buf, " ");
        XtasSetLabelString (label, buf);
    }
//    /* Latch Command */
//    label = XtNameToWidget (FocusSet->PATH_SET->TOP_LEVEL, "*XtasPathInfosLatchCmdLabel");
//    if (!label) 
//        return;
//    if (path->CMDLATCH)
//        sprintf (buf, "%s", path->CMDLATCH->ROOT->NAME);
//    else
//        sprintf (buf, " ");
//    XtasSetLabelString (label, buf);
    
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathsGetPathCmdCallback (widget, client_data, call_data)
    Widget      widget      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    XtasWindowStruct    *tas_winfos = (XtasWindowStruct *) client_data;
    short int mask = XTAS_REG, i;
    ttvsig_list *ptsig;
    chain_list  *ch, *chain;
    char buf[256], cmd_list[1024];
    stbnode *ptnode;
    ptype_list *ptype;
    long hold;
    
    if(XTAS_SIGNAL != NULL)
    {
        ptsig = ttv_getsig(XtasMainParam->ttvfig, XTAS_SIGNAL);
        if(ptsig) {
            if((ptsig->TYPE & TTV_SIG_L) != TTV_SIG_L)
                XalDrawMessage(tas_winfos->errwidget, "No command found: \n the selected signal is not a register signal");
            else {   
                strcpy(cmd_list , "");
                chain = ttv_getlrcmd(XtasMainParam->ttvfig, ptsig);
                for(ch = chain; ch ; ch = ch->NEXT) {
                    ttv_getsigname(XtasMainParam->ttvfig, buf, ((ttvevent_list*)ch->DATA)->ROOT);
                    strcat(cmd_list, buf); 
                    strcat(cmd_list, " ");
                }
                if(chain)
                    freechain(chain);

                XtasSetOrderByStartOrEnd( 10, 1, "*" ) ;
                XtasSetOrderByStartOrEnd( 20, 1, cmd_list) ;
                
                if((ptype = getptype(ptsig->USER,STB_HOLD)) != NULL)
                    hold = (long)ptype->DATA ;

                if(ptype == NULL)
                {
                    hold  = STB_NO_TIME;
                    for(i = 0  ; i < 2  ; i++)
                    {
                        ptnode = stb_getstbnode(ptsig->NODE+i) ;
                        if(ptnode != NULL)
                            if(ptnode->HOLD < hold)
                                hold = ptnode->HOLD  ;
                    }
                }

                if( hold < XtasMargin)
                    mask |= XTAS_HOLD;

                XtasMainToolsCallback( widget, (XtPointer)(&mask), call_data ) ;
            }
        }
    }
    else 
        XalDrawMessage(tas_winfos->errwidget, XTAS_NSIGERR);
    
}

void XtasPathsGetPathCallback (widget, client_data, call_data)
    Widget      widget      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    short int mask = 0x0;
    
    if(XTAS_SIGNAL != NULL)
    {
        XtasSetOrderByStartOrEnd( 10, 1, "*" ) ;
        XtasSetOrderByStartOrEnd( 20, 1, XTAS_SIGNAL) ;
    }
    XtasMainToolsCallback( widget, (XtPointer)(&mask), call_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasRemovePathSession(window)
XtasWindowStruct *window ;
{
	chain_list            *head = XTAS_PATH_SESSION;
 	chain_list            *chain ;
 	XtasWindowStruct      *data;
 	XtasPathListSetStruct *path_set;

  	if(head != NULL) {
		for(; head != NULL; head = head->NEXT ) {
        	data = head->DATA;
          	if((window == NULL) || (window == data)) {
				path_set = data->userdata;
            	if(path_set->PARENT != NULL) {
              		for(chain = path_set->PARENT->PARA_SESSION ; chain != NULL; chain = chain->NEXT ) {
                		if((window == NULL) || (chain->DATA == window))
                 			chain->DATA = NULL ;
               		}
             	}
            	for(chain = path_set->DETAIL_SESSION ; chain != NULL; chain = chain->NEXT ) {
               		if(chain->DATA != NULL) {
                 		XtasRemoveDetailPathSession((XtasWindowStruct *)chain->DATA) ;
                	}
              	}
            	for(chain = path_set->PARA_SESSION ; chain != NULL; chain = chain->NEXT ) {
               		if(chain->DATA != NULL) {
                 		XtasRemovePathSession((XtasWindowStruct *)chain->DATA) ;
                	}
              	}
            	XtasFreePathListSet( path_set );
            	XtDestroyWidget( data->topwidget );
            	mbkfree( data );
            	if(window == data)
             		break ;
           	}
        }
      	if(window == NULL) {
        	freechain( XTAS_PATH_SESSION );
        	XTAS_PATH_SESSION = NULL;
       	}
      	else if(head != NULL)
        	XTAS_PATH_SESSION = delchain(XTAS_PATH_SESSION,head) ;
    }
    XTAS_PATHSSELECTEDWIDGET = NULL;
}

/*---------------------------------------------------------------------------*/
void XtasSavePaths(file,pathset)
FILE *file ;
XtasPathListSetStruct *pathset ;
{
 ttvpath_list * path = pathset->CUR_PATH_WIN->DATA ;
 int nbpath = pathset->PARAM->PATH_ITEM ;
 char type  ;
 char *typedelay ;
 long delay ;
 int sizetype ;
 int sizedelay ;
 int sizeslope ;
 int sizecumul ;
 int sizename1 ;
 int sizename2 ;
 char name[1024] ;
 char namx[1024] ;
 char c[48] ;
 int i ;

 if(pathset->PARAM->CRITIC == 'Y')
  type = 'M' ;
 else if(pathset->PARAM->CRITIC == 'P')
  type = 'P' ;
 else
  type = 'D' ;

 XtasCalcSize(pathset->PARAM->ROOT_SIG_EDGE->TOP_FIG,
              &sizetype,&sizedelay,NULL,&sizecumul,NULL,&sizeslope,NULL,
              &sizename1,&sizename2,type,(chain_list *)path,&delay,NULL,XTAS_NONE,nbpath) ;

 XtasPrintFirstEnd(file,sizetype,sizedelay,0,sizecumul,0,sizeslope,0,
                   sizename1,sizename2,XTAS_NONE) ;
 XtasPrintLinesepar(file,sizetype,sizedelay,0,sizecumul,0,sizeslope,0,
                    sizename1,sizename2) ;

 i = 1 ;

 while(path != NULL)
  {
   if((path->TYPE & TTV_FIND_MAX) == TTV_FIND_MAX)
       {
        sprintf(name,"%c %s",type,"MAX") ;
       }
     else
       {
        sprintf(name,"%c %s",type,"MIN") ;
       }
   XtasPrintType(i,name,c,sizetype) ;
   typedelay = c ;
   XtasPrintLine(file,typedelay,sizetype,path->DELAY,sizedelay,(long)0,0,(long)0,
                 sizecumul,(long)0,0,path->SLOPE,sizeslope,(long)0,0,
                 ttv_getsigname(pathset->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                 name,path->NODE->ROOT),sizename1,
                 XTAS_NODELEVEL(path->NODE),
                 ((path->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) ? 'Z' :
                                         XTAS_NODELEVEL(path->ROOT),
                 ttv_getsigname(pathset->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                 namx,path->ROOT->ROOT),sizename2) ;
   path = path->NEXT ;
   i++ ;
   if(i > nbpath)
    break ;
  }

 XtasPrintLinesepar(file,sizetype,sizedelay,0,sizecumul,0,sizeslope,0,
                    sizename1,sizename2) ;
 XtasPrintFirstEnd(file,sizetype,sizedelay,0,sizecumul,0,sizeslope,0,
                   sizename1,sizename2,XTAS_NONE) ;
 fprintf(file,"\n\n") ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathsSaveCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;
 XtasPathsSavePathset = (XtasPathListSetStruct *)widtab->userdata;

 XtasSelectFileCallback( widget, (XtPointer)XTAS_PATH_MAP, call_data ) ;
}

void XtasPathsSelectFileOk (void)
{
  FILE *file ;
  
  if(XtasDataFileName != NULL)
  {
   file = fopen(XtasDataFileName,"a") ;
   if(file != NULL)
    {
     XtasSavePaths(file,XtasPathsSavePathset) ;
     fclose(file) ;
    }
   else
    {
     XalDrawMessage( XtasErrorWidget, XTAS_NOPENFL);
    }
  }
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

void XtasSignalInfoCallback (widget, client_data , call_data)
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
 XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;

 	XalSetCursor ( widtab->topwidget, WAIT); 
 	XalForceUpdate ( widtab->topwidget);

    if (XTAS_SIGNAL) 
		XtasSignalsDetail ( widtab->topwidget, XTAS_SIGNAL) ;               
    else if ( XTAS_DISABLE_INFO == 'Y' ) 
		XalDrawMessage (XtasErrorWidget, XTAS_INFOERR) ;
 	else 
		XalDrawMessage (XtasErrorWidget, XTAS_NSIGERR) ;
 
 	XalSetCursor ( widtab->topwidget, NORMAL) ;
}

/*---------------------------------------------------------------------------*/
/* FUNCTION : XtasCreate2SubForm                                             */
/*                                                                           */
/* Create the 2 subforms and display the infos on 2 signals named buf1 & buf2*/
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCreate2SubForm(XtasPathSet,buf1,buf2)
XtasPathListSetStruct *XtasPathSet ; 
char  *buf1 ;
char  *buf2 ;
{
 Widget         paned_window ;
 Widget         w1,w2 ;
 static Widget  last_paned_widget ;
 static XtasPathListSetStruct* lastXtasPathSet ;
 Arg            args[10];
 int            n ;
 XtWidgetGeometry   size;
 
    if (( last_paned_widget ) && ( lastXtasPathSet == XtasPathSet ))
        XtDestroyWidget ( last_paned_widget ) ;
    n=0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomAttachment,  XmATTACH_FORM   ); n++;
    paned_window = XtCreateManagedWidget( "XtasPanedWindow",
                                           xmPanedWindowWidgetClass,
                                           XtasPathSet->PATHLIST_2INFO, 
                                           args, n );
    last_paned_widget = paned_window ;

    n=0;
    w1 = XtCreateManagedWidget (  "XtasNewDisp",
                                       xmFormWidgetClass,
                                       paned_window,
                                       args, n );
    w2 = XtCreateManagedWidget (  "XtasNewDisp",
                                       xmFormWidgetClass,
                                       paned_window,
                                       args, n );
    XtasFillInfosForm( w1, buf1 ) ;
    XtasFillInfosForm( w2, buf2 ) ;

    size.request_mode = CWHeight;
    XtQueryGeometry (w1, NULL, &size);
    XtVaSetValues (w1, XmNpaneMaximum, size.height, NULL);
    XtQueryGeometry (w2, NULL, &size);
    XtVaSetValues (w2, XmNpaneMaximum, size.height, NULL);
    
    lastXtasPathSet = XtasPathSet ;
}

/*---------------------------------------------------------------------------*/
/*      Focus the path so we can get detail path                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasPathsFocus2Callback (widget, client_data , call_data)
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
    XmListCallbackStruct  *cbs ;
    chain_list            *headsav = (chain_list *)client_data ;
    chain_list            *head;
    int                    i ;

    head = headsav ;

    cbs  = (XmListCallbackStruct*)call_data ;

    i    = 1 ;
    while (head && (cbs) && (i < cbs->item_position)) {
        head = head->NEXT ;
        i++ ;
    }

    if ( head->DATA )
       XtasPathsFocusCallback( widget, (XtPointer)head->DATA, call_data ) ;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGet2InfoCallback                                           */
/*                                                                           */
/* Displays the infos of the begining and the end of a path                  */
/* selected in the scrolled list                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasGet2InfoCallback (widget, client_data , call_data)
Widget  widget;
XtPointer client_data ;
XtPointer call_data ;
{
    XtasPathListSetStruct *XtasPathSet = (XtasPathListSetStruct *)client_data ;
    static XtasPathListSetStruct *LastXtasPathSet ; 
    XmListCallbackStruct  *cbs ;
    ttvpath_list          *path;
    char                   buf1[1024];
    char                   buf2[1024];
    static char           *sig1;
    static char           *sig2;
    int                    i;

    XTAS_DISABLE_INFO = 'Y' ;
    cbs  = (XmListCallbackStruct*)call_data ;

    path = XtasPathSet->CUR_PATH_WIN->DATA;
    i    = 1 ;
    while (path&& (cbs) && (i < cbs->item_position)) {
        path = path->NEXT ;
        i++ ;
    }

    ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG, buf1,path->NODE->ROOT);
    ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG, buf2,path->ROOT->ROOT);
    if ( sig1 && (LastXtasPathSet == XtasPathSet))
        mbkfree ( sig1 ) ;
    if ( sig2 && (LastXtasPathSet == XtasPathSet))
        mbkfree ( sig2 ) ;
    sig1 = strdup(buf1) ;
    sig2 = strdup(buf2) ;
    XtasCreate2SubForm(XtasPathSet,sig1,sig2) ;
    LastXtasPathSet = XtasPathSet ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsCloseCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .top : The window toplevel widget id.                          */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathsCloseCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
 XtasWindowStruct * top = (XtasWindowStruct *)client_data ;

 XtasRemovePathSession(top) ;
// XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsFocusCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .id : The path id, 0 if losing the focus.                      */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSignalFocusCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	XTAS_SIGNAL = (char*)client_data ;
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsDeselect                                              */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathsDeselect( widget )
Widget widget;
{
    Widget  parent;
    Pixel   fg, bg;

    parent = XtParent (widget);
    XtVaGetValues(parent, XmNbackground,&bg,NULL );
    XtVaSetValues(widget, XmNcursorPositionVisible, False, NULL) ;
    XtVaSetValues (parent, XmNborderColor, bg, NULL); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetFocusParentCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : the false path result label.                    */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasGetFocusParentCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
	XTAS_FLSPATHRESLABEL = (Widget)client_data ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsFocusCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .id : The path id, 0 if losing the focus.                      */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathsFocusCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
    Pixel   fg, bg;
    Widget  parent;
    
 	XtasPathIndexTabElt  *FocusSet = ( XtasPathIndexTabElt  *)client_data ;
    
    if ( !XTAS_DISPLAY_NEW ) {
        if (XTAS_PATHSSELECTEDWIDGET != NULL)
            XtasPathsDeselect (XTAS_PATHSSELECTEDWIDGET);
        XTAS_PATHSSELECTEDWIDGET = widget;
        parent = XtParent (XTAS_PATHSSELECTEDWIDGET);  
        XtVaGetValues(parent , XmNforeground, &fg,NULL );
   	    XtVaSetValues (XTAS_PATHSSELECTEDWIDGET, XmNcursorPositionVisible, True, NULL) ;
   	    XtVaSetValues (parent, XmNborderColor, fg, NULL) ;
    }
   	FocusSet->PATH_SET->PARAM->LOOK_PATH = FocusSet->PATH ;

    XtasFillPathInfosForm (FocusSet);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsLosingFocusCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .id : The path id, 0 if losing the focus.                      */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasLosingPathsFocusCallback( widget, client_data, call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFillPathList                                               */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : The Set of the current window widgets.               */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillPathList( XtasPathSet)
XtasPathListSetStruct  *XtasPathSet;
{
 Widget         row_widget;
 Widget         list_widget;
 Pixel          tas_state_color;
 Pixel          tas_backg_color;
 XmString       text;
 char           value[1024];
 Widget         label_widget, text_from, text_to, text_slope, text_time;
 Pixmap         r_pixmap, f_pixmap, v_pixmap;
 Arg            args[10];
 int            n;
 chain_list    *head;
 ttvpath_list  *path;
 int            c, offset, limit, size;

   list_widget = XtasPathSet->PATHLIST_SCROLL;
   row_widget = XtasPathSet->PATHLIST_ROW;

   if ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_SBS) == XTAS_SBS) {
       offset = 10;
       limit = 5;
   }
   else {
       offset = 10;
       limit = 10;
   }
       

   if ( row_widget != NULL ) 
   {
      if (XTAS_PATHSSELECTEDWIDGET != NULL) {
         XtasPathsDeselect (XTAS_PATHSSELECTEDWIDGET);
         XTAS_PATHSSELECTEDWIDGET = NULL;
      }
      XtDestroyWidget(row_widget);
   }

   row_widget = XtVaCreateWidget( "XtasPathsRow",
                                   xmRowColumnWidgetClass,
                                   list_widget,
                                   XmNorientation, XmVERTICAL,
                                   XmNspacing, 1,
                                  NULL);

   XtasPathSet->PATHLIST_ROW = row_widget;

   XtVaGetValues(row_widget,XmNforeground,&tas_state_color,
                              XmNbackground,&tas_backg_color,NULL );

   r_pixmap = XalGetPixmap( row_widget, 
                           XTAS_REDG_MAP, 
                           tas_state_color, tas_backg_color );
   f_pixmap = XalGetPixmap( row_widget, 
                           XTAS_FEDG_MAP,
                           tas_state_color, tas_backg_color );

   head = XtasCreatePathTabIndex( XtasPathSet );
   path = XtasPathSet->CUR_PATH_WIN->DATA;

   if (XtasPathSet->CUR_PATH_WIN->PREV)
       size = XtasPathSet->CUR_PATH_WIN->PREV->SIZE;
   else
       size = XtasPathSet->CUR_PATH_WIN->SIZE;
   c = ((XtasPathSet->CUR_PATH_WIN->INDEX - 1) * size) + 1;

   while ( head != NULL )
   {
      Widget sub_form;

      n = 0;
      XtSetArg( args[n], XmNborderWidth, 1 ); n++;
      XtSetArg( args[n], XmNborderColor, tas_backg_color ); n++;
   //   XtSetArg( args[n], XmNwidth,                 690             ); n++;
      sub_form = XtCreateManagedWidget( "XtasSubForm",
                                         xmFormWidgetClass,
                                         row_widget,
                                         args, n );
      //XmAddTabGroup( sub_form );
   
      if ( (r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP) )
      {
         text = ( ( path->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? XmStringCreateSimple("rising") : XmStringCreateSimple("falling"); 
         label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                               xmLabelWidgetClass,
                                               sub_form,
                                               XmNtopAttachment,  XmATTACH_FORM,
                                               XmNlabelString,    text,
                                               XmNleftAttachment, XmATTACH_FORM,
                                               XmNleftOffset,     7,
                                               XmNwidth,          40,
                                               XmNheight,         25,
                                              NULL);
          XmStringFree( text );
      }
      else
      {
          v_pixmap = ( (path->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? r_pixmap : f_pixmap;
         label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                               xmLabelWidgetClass,
                                               sub_form,
                                               XmNtopAttachment,  XmATTACH_FORM,
                                               XmNlabelType,      XmPIXMAP,
                                               XmNlabelPixmap,    v_pixmap,
                                               XmNleftAttachment, XmATTACH_FORM,
                                               XmNleftOffset,     7,
                                               XmNwidth,          40,
                                               XmNheight,         25,
                                              NULL);
      }

      n = 0;
      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNleftWidget,            label_widget    ); n++;
      XtSetArg( args[n], XmNrightOffset,           2               ); n++;
      XtSetArg( args[n], XmNwidth,                 150             ); n++;
      XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
      XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
      XtSetArg( args[n], XmNeditable,              False           ); n++;
      text_from = XtCreateManagedWidget( "XtasUserField",
                                           xmTextWidgetClass,
                                           sub_form,
                                           args, n);
/*      XtAddCallback( text_from,
                     XmNactivateCallback,
                     XtasPathsDetailCallback,
                     (XtPointer)XtasPathSet );*/
      XtAddCallback( text_from,
                     XmNfocusCallback,
                     XtasPathsFocusCallback,
                    (XtPointer)head->DATA ); 
      XtAddCallback( text_from,
                     XmNlosingFocusCallback,
                     XtasLosingPathsFocusCallback,
                    (XtPointer)head->DATA ); 
      ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG, value,path->NODE->ROOT);
      XtAddCallback(text_from,
                     XmNfocusCallback,
                     XtasSignalFocusCallback,
                    (XtPointer)strdup (value)); 
      XmTextSetString( text_from, value );
      if ( (r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP) || 
           ( ( path->TYPE & TTV_FIND_HZ ) == TTV_FIND_HZ ) )
      {
         if(( path->TYPE & TTV_FIND_HZ ) == TTV_FIND_HZ )
         text = XmStringCreateSimple("highZ");
         else
         text = ( ( path->ROOT->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? XmStringCreateSimple("rising") : XmStringCreateSimple("falling"); 
         label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                             xmLabelWidgetClass,
                                             sub_form,
                                             XmNlabelString,    text,
                                             XmNleftAttachment, XmATTACH_WIDGET,
                                             XmNleftWidget,     text_from,
                                             XmNleftOffset,     9,
                                             XmNwidth,          40,
                                             XmNheight,         25,
                                            NULL);
         XmStringFree( text );
      }
      else
      {
         v_pixmap = ( ( path->ROOT->TYPE & TTV_NODE_UP ) == TTV_NODE_UP) ? r_pixmap : f_pixmap;
         label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                             xmLabelWidgetClass,
                                             sub_form,
                                             XmNlabelType,      XmPIXMAP,
                                             XmNlabelPixmap,    v_pixmap,
                                             XmNleftAttachment, XmATTACH_WIDGET,
                                             XmNleftWidget,     text_from,
                                             XmNleftOffset,     9,
                                             XmNwidth,          40,
                                             XmNheight,         25,
                                            NULL);
      }
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNleftWidget,            label_widget    ); n++;
      XtSetArg( args[n], XmNrightOffset,           2               ); n++;
      XtSetArg( args[n], XmNwidth,                 150             ); n++; 
      XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
      XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
      XtSetArg( args[n], XmNeditable,              False           ); n++;
      text_to = XtCreateManagedWidget( "XtasUserField",
                                           xmTextWidgetClass,
                                           sub_form,
                                           args, n );
/*      XtAddCallback( text_to,
                     XmNactivateCallback,
                     XtasPathsDetailCallback,
                     (XtPointer)XtasPathSet );*/
      XtAddCallback( text_to,
                     XmNfocusCallback,
                     XtasPathsFocusCallback,
                     (XtPointer)head->DATA ); 
      XtAddCallback( text_to,
                     XmNlosingFocusCallback,
                     XtasLosingPathsFocusCallback,
                    (XtPointer)head->DATA ); 
      ttv_getsigname( XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG, value, path->ROOT->ROOT);
      XtAddCallback( text_to,
                     XmNfocusCallback,
                     XtasSignalFocusCallback,
                     (XtPointer)strdup (value)); 
      XmTextSetString( text_to,value);
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNleftWidget,            text_to      ); n++;
      XtSetArg( args[n], XmNleftOffset,            10              ); n++;
      XtSetArg( args[n], XmNrightOffset,           2               ); n++;
      XtSetArg( args[n], XmNwidth,                 100             ); n++;
      XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
      XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
      XtSetArg( args[n], XmNeditable,              False           ); n++;
      text_slope = XtCreateManagedWidget( "XtasUserField",
                                           xmTextWidgetClass,
                                           sub_form, args, n);
/*      XtAddCallback( text_slope,
                     XmNactivateCallback,
                     XtasPathsDetailCallback,
                     (XtPointer)XtasPathSet );*/
      XtAddCallback( text_slope,
                     XmNfocusCallback,
                     XtasPathsFocusCallback,
                     (XtPointer)head->DATA );
      XtAddCallback( text_slope,
                     XmNlosingFocusCallback,
                     XtasLosingPathsFocusCallback,
                    (XtPointer)head->DATA ); 
      XtAddCallback( text_slope,
                     XmNfocusCallback,
                     XtasSignalFocusCallback,
                     (XtPointer)NULL); 
      sprintf( value, "%.1f pS" , path->SLOPE/TTV_UNIT );  
      XmTextSetString( text_slope, value );  
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNleftWidget,            text_slope      ); n++;
      
      XtSetArg( args[n], XmNleftOffset,            2               ); n++;
      XtSetArg( args[n], XmNrightOffset,           2               ); n++;
      XtSetArg( args[n], XmNwidth,                 100             ); n++;
      XtSetArg( args[n], XmNshadowThickness,       1               ); n++;
      XtSetArg( args[n], XmNcursorPositionVisible, False           ); n++;
      XtSetArg( args[n], XmNeditable,              False           ); n++;
      text_time = XtCreateManagedWidget( "XtasUserField",
                                           xmTextWidgetClass,
                                           sub_form,
                                           args, n);
/*      XtAddCallback( text_time,
                     XmNactivateCallback,
                     XtasPathsDetailCallback,
                    (XtPointer)XtasPathSet );*/
      XtAddCallback( text_time,
                     XmNfocusCallback,
                     XtasPathsFocusCallback,
                     (XtPointer)head->DATA );
      XtAddCallback( text_time,
                     XmNlosingFocusCallback,
                     XtasLosingPathsFocusCallback,
                    (XtPointer)head->DATA ); 
      XtAddCallback( text_time,
                     XmNfocusCallback,
                     XtasSignalFocusCallback,
                     (XtPointer)NULL); 
      sprintf( value, "%.1fpS" , (path->DELAY+path->DATADELAY)/ TTV_UNIT);  
      XmTextSetString( text_time, value );
  
      /*===== New Widget =====*/   

      n = 0;
      XtSetArg( args[n], XmNtopAttachment,  XmATTACH_OPPOSITE_WIDGET    ); n++;
      XtSetArg( args[n], XmNtopWidget,      text_time                   ); n++;
      XtSetArg( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET   ); n++;
      XtSetArg( args[n], XmNbottomWidget,    text_time                  ); n++;
      XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET             ); n++;
      XtSetArg( args[n], XmNleftWidget,     text_time                   ); n++;
      label_widget = XtCreateManagedWidget( "XtasLabel",
                                             xmLabelWidgetClass,
                                             sub_form,
                                             args, n );
      sprintf( value, "%i" , c );  
      XtasSetLabelString( label_widget, value);
      
      
      /*===== Widget used to display false path result =====*/
      n = 0;
      XtSetArg( args[n], XmNtopOffset,      3               ); n++;
      XtSetArg( args[n], XmNleftAttachment, XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNleftWidget,     label_widget      ); n++;
      XtSetArg( args[n], XmNleftOffset,     2      ); n++;
      XtSetArg( args[n], XmNlabelType,      XmPIXMAP     ); n++;
      XtSetArg( args[n], XmNheight,    20      ); n++;
      XtSetArg( args[n], XmNwidth,     20      ); n++;
      label_widget = XtCreateManagedWidget( "",
                                             xmLabelWidgetClass,
                                             sub_form,
                                             args, n );
      
             
      
      XtAddCallback(text_from,
                     XmNfocusCallback,
                     XtasGetFocusParentCallback,
                    (XtPointer)label_widget); 
      XtAddCallback(text_to,
                     XmNfocusCallback,
                     XtasGetFocusParentCallback,
                    (XtPointer)label_widget); 
      XtAddCallback(text_slope,
                     XmNfocusCallback,
                     XtasGetFocusParentCallback,
                    (XtPointer)label_widget); 
      XtAddCallback(text_time,
                     XmNfocusCallback,
                     XtasGetFocusParentCallback,
                    (XtPointer)label_widget); 

      c++;
      head = head->NEXT;
      path = path->NEXT;
   }
   c--; 
   if ((c > limit) || ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_SBS)==XTAS_SBS))
       XtVaSetValues (list_widget, XmNheight, offset + limit*32, NULL);
   else
       XtVaSetValues (list_widget, XmNheight, offset + c*32, NULL);


   XtManageChild( row_widget );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFillPathList2                                              */
/*                                                                           */
/* New display of the paths list                                             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillPathList2( XtasPathSet)
XtasPathListSetStruct  *XtasPathSet;
{
 XmString       text;
 char           value[1024];
 chain_list    *head;
 chain_list    *headsav;
 ttvpath_list  *path;
 int            c;
 char           text_line[2048];
 char          *first_signame;
 char          *last_signame;
 char          *tp;
 char          *slope;
 char           type_front1[1024];
 char           type_front2[1024];

 head = XtasCreatePathTabIndex( XtasPathSet );
 headsav = head ;
 path = XtasPathSet->CUR_PATH_WIN->DATA;

 XmListDeleteAllItems(XtasPathSet->PATHLIST_SCROLL) ;
 
 c = 1;

 while ( head != NULL )
 {
     /* type du 1er front */
     if(( path->NODE->TYPE & TTV_NODE_UP ) == TTV_NODE_UP)
         sprintf(type_front1,"R ") ;
     else
         sprintf(type_front1,"F ") ;

     /* type du 2e  front */
     if (( path->TYPE & TTV_FIND_HZ ) == TTV_FIND_HZ)
         sprintf(type_front2,"Z ") ;
     else if(( path->ROOT->TYPE & TTV_NODE_UP ) == TTV_NODE_UP)
         sprintf(type_front2,"R ") ;
     else
         sprintf(type_front2,"F ") ;

     /* first signal */
     ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG, value,path->NODE->ROOT);
     first_signame = XtasPlaceString(value,XtasCarLength1) ;

     /* last  signal */
     ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG, value,path->ROOT->ROOT);
     last_signame = XtasPlaceString(value,XtasCarLength2) ;

     /* Slope */
     slope = XtasPlaceNbStringTTVUnit(path->SLOPE,TTV_UNIT) ;
          
     /* Tp    */
     tp    = XtasPlaceNbStringTTVUnit(path->DELAY+path->DATADELAY, TTV_UNIT) ;
          
     /* indice du chemin = c */
     
     sprintf(text_line,"%s   %s %s %s %spS  %spS   %d", first_signame, type_front1, type_front2,
             last_signame, slope, tp ,c) ;

     mbkfree(first_signame) ;
     mbkfree(last_signame) ;
     mbkfree(slope); 
     mbkfree(tp); 

     text = XmStringCreateSimple (text_line) ;
     XmListAddItemUnselected (XtasPathSet->PATHLIST_SCROLL, text, c) ;
     XmStringFree (text) ;
     c++;
     head = head->NEXT;
     path = path->NEXT;
 }

 /* Call back to define on scroll list */
 XtAddCallback (XtasPathSet->PATHLIST_SCROLL,
                XmNbrowseSelectionCallback,
                XtasGet2InfoCallback,
                (XtPointer)XtasPathSet);
 
 /* Call back to display signal details */ 
 XtAddCallback (XtasPathSet->PATHLIST_SCROLL,
                XmNbrowseSelectionCallback,
                XtasPathsFocus2Callback,
                (XtPointer)headsav);
/*
 XtAddCallback( XtasPathSet->PATHLIST_SCROLL,
                XmNdefaultActionCallback,
                XtasPathsDetailCallback,
                (XtPointer)XtasPathSet);
*/
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathSelectCallback(widget, client_data , call_data )
Widget widget;
XtPointer client_data ;
XtPointer call_data;
{
XtasPathListSetStruct *XtasPathSet = (XtasPathListSetStruct *)client_data ;
XmListCallbackStruct *cbs = (XmListCallbackStruct *)call_data ;
int position, n;
chain_list  *sig_list;
char         text_info[1024];
char         text_info2[1024];
ttvsig_list *head;

XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
XalForceUpdate( XtasPathSet->TOP_LEVEL );

XTAS_PATHSSELECTEDWIDGET = NULL;

sig_list = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN->DATA;
position = cbs->item_position;

for ( n = 1 ; n < position ; n++ )
 {
   sig_list = sig_list->NEXT; 
 }

XtasPathSet->PARAM->CUR_SIG = sig_list; 
head = sig_list->DATA;

if ( XtasTtvGetPathList( XtasPathSet ) != NULL )
 {
  if ((XtasPathSet->PARAM->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
     sprintf( text_info,  "Current Input Signal: %s",
     ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                    text_info2, head));
  else
      sprintf( text_info, "Current Output Signal: %s",
      ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                     text_info2, head));

  XtasSetLabelString( XtasPathSet->INFO_CUR_SIG, text_info);
  if (XTAS_DISPLAY_NEW)
      XtasFillPathList2 (XtasPathSet) ;
  else
      XtasFillPathList( XtasPathSet );
 }
 else 
 {
   if ((XtasPathSet->PARAM->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
     sprintf( text_info,  "Current Input Signal: %s",
     ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                    text_info2, head));
  else
      sprintf( text_info, "Current Output Signal: %s",
      ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                     text_info2, head));
  XalDrawMessage( XtasWarningWidget , XTAS_NPATWAR ) ;
 }

 XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                        XtasPathTreatEndCallback                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatEndCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;
   XtasChainJmpList  *head; 
   
   head = XtasPathSet->CUR_PATH_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if (head->NEXT != NULL )
   {
      while( head->NEXT != NULL )
      {
         head                      = head->NEXT;
         XtasPathSet->CUR_PATH_WIN = head;
      }
/*      if (XTAS_DISPLAY_NEW)
          XtasFillPathList2 (XtasPathSet) ;
      else*/
          XtasFillPathList( XtasPathSet );
   }
   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                     XtasPathTreatFastFwdCallback                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatFastFwdCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   XtasChainJmpList  *head = XtasPathSet->CUR_PATH_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->NEXT != NULL )
   {
      XtasPathSet->CUR_PATH_WIN = head->NEXT;
      head = head->NEXT;
   }

   if ( head->NEXT != NULL )
   {
     XtasPathSet->CUR_PATH_WIN = head->NEXT;
   }
/*   if (XTAS_DISPLAY_NEW)
       XtasFillPathList2 (XtasPathSet) ;
   else*/
       XtasFillPathList( XtasPathSet );

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatMiddleCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   XtasChainJmpList  *head = XtasPathSet->CUR_PATH_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->NEXT != NULL )
   {
      XtasPathSet->CUR_PATH_WIN = head->NEXT;
/*      if (XTAS_DISPLAY_NEW)
          XtasFillPathList2 (XtasPathSet) ;
      else */
          XtasFillPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatNextCallback( widget, client_data , call_data )
Widget widget;
XtPointer client_data;
XtPointer call_data;
{
   XtasPathListSetStruct *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   XtasChainJmpList  *head = XtasPathSet->CUR_PATH_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->NEXT != NULL )
   {
      XtasPathSet->CUR_PATH_WIN = head->NEXT;
/*      if (XTAS_DISPLAY_NEW)
          XtasFillPathList2 (XtasPathSet) ;
      else*/
          XtasFillPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatFastRewCallback( widget, client_data, call_data )
Widget                  widget;
XtPointer client_data ;
XtPointer                 call_data;
{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   XtasChainJmpList  *head = XtasPathSet->CUR_PATH_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->PREV != NULL )
   {
      head = head->PREV;  
      if ( head->PREV != NULL )
      {
         head = head->PREV;  
         XtasPathSet->CUR_PATH_WIN = head;
/*         if (XTAS_DISPLAY_NEW)
             XtasFillPathList2 (XtasPathSet) ;
         else*/
             XtasFillPathList( XtasPathSet );
      }
      else 
      {
         XtasPathSet->CUR_PATH_WIN = head;
/*         if (XTAS_DISPLAY_NEW)
             XtasFillPathList2 (XtasPathSet) ;
         else*/
             XtasFillPathList( XtasPathSet );
      }
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatTopCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   XtasChainJmpList  *head = XtasPathSet->CUR_PATH_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );
   if ( head->PREV != NULL )
   {
      while ( head->PREV != NULL )
      {
         head                      = head->PREV;
         XtasPathSet->CUR_PATH_WIN = head;
      }
/*      if (XTAS_DISPLAY_NEW)
          XtasFillPathList2 (XtasPathSet) ;
      else*/
          XtasFillPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPathTreatPrevCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   XtasChainJmpList  *head = XtasPathSet->CUR_PATH_WIN;; 


   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->PREV != NULL )
   {
      XtasPathSet->CUR_PATH_WIN = head->PREV;
/*      if (XTAS_DISPLAY_NEW)
          XtasFillPathList2 (XtasPathSet) ;
      else*/
          XtasFillPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillSigPathList( XtasPathSet )

 XtasPathListSetStruct  *XtasPathSet;

{
   int         position = 0;
   chain_list *head;
   XmString    motif_string;
   char        buffer[1024];


   XmListDeleteAllItems( XtasPathSet->SIGNAL_LIST_SET->SIGLIST_SCROLL );

   if ( XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN == NULL )
   {
      return;
   }
   head = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN->DATA;

   if ( head == NULL )
   {
 
 /*   XalDrawMessage( Xtaswarning_widget[type], XTAS_NSIGWAR ); */

   }
   else
   {  
      while ( position < XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN->SIZE )
      {
           /*------------------------------------------------*/
          /*    traitement a modifier  pour plus d'infos ?  */
         /*------------------------------------------------*/
     
         ttv_getsigname( XtasPathSet->SIGNAL_LIST_SET->SIGNAL->TOP_FIG,
                         buffer, head->DATA);
         motif_string = XmStringCreateLtoR( buffer, XmSTRING_DEFAULT_CHARSET ); 
         XmListAddItemUnselected( XtasPathSet->SIGNAL_LIST_SET->SIGLIST_SCROLL,
                                  motif_string, ++position );
         XmStringFree( motif_string );
         head = head->NEXT;
      }
   }   
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatEndCallback( widget, client_data, call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;
 
   head = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   while ( head->NEXT != NULL )
   {
      head                                      = head->NEXT;
      XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head;
   }
   XtasFillSigPathList( XtasPathSet );

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatFastFwdCallback( widget, client_data, call_data )
 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;
{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;
 
   head = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->NEXT != NULL )
   {
      head = head->NEXT;
   }

   if ( head->NEXT != NULL )
   {
      head = head->NEXT;
   }

   XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head;

   XtasFillSigPathList( XtasPathSet );

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatNextCallback( widget, client_data , call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;
 
   head = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->NEXT != NULL )
   {
      XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head->NEXT;
      XtasFillSigPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatMiddleCallback( widget, client_data , call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;
 
   head = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN;; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->NEXT != NULL )
   {
      XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head->NEXT;
      XtasFillSigPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatPrevCallback( widget, client_data  , call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;

   head  = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->PREV != NULL )
   {
      XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head->PREV;
      XtasFillSigPathList( XtasPathSet );
   }

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatFastRewCallback( widget, client_data  , call_data )

 Widget                  widget;
 XtPointer client_data ;
 XtPointer                 call_data;

{
 XtasPathListSetStruct  *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;

   head  = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   if ( head->PREV != NULL )
   {
      head = head->PREV;
   }
   if ( head->PREV != NULL )
   {
      head = head->PREV;
   }

   XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head;

   XtasFillSigPathList( XtasPathSet );

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSigPathTreatTopCallback( widget, client_data  , call_data )

 Widget widget;
 XtPointer client_data;
 XtPointer call_data;

{
   XtasPathListSetStruct *XtasPathSet = (XtasPathListSetStruct *)client_data ;

   static XtasChainJmpList  *head;

   head  = XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN; 

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_SIGINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, WAIT ); 
   XalForceUpdate( XtasPathSet->TOP_LEVEL );

   while ( head->PREV != NULL )
   {
      head                                      = head->PREV;
      XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = head;
   }

   XtasFillSigPathList( XtasPathSet );

   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
   XalSetCursor( XtasPathSet->TOP_LEVEL, NORMAL ); 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsButtonsCallback                                       */
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
void XtasPathsButtonsCallback( widget, client_data , call_data )

 Widget             widget;
 XtPointer          client_data ;
 XtPointer            call_data;

{
 XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;
 XtasOptionsButtonsTreat( widtab->wrkwidget, &widtab->butwidget,
                          XtasPathsButtonMenus,  XtasPathsButtonOptions );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreatePathsMenus                                           */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the paths   window.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreatePathsMenus( parent , tas_winfos)

 Widget             parent;
 XtasWindowStruct  *tas_winfos;

{
   Widget   menu_bar;
   Arg      args[20];
   int      n;
   
   for(n = 0 ; XtasPathsMenuFile[n].label != NULL ; n++)
     XtasPathsMenuFile[n].callback_data    = (XtPointer)tas_winfos;
   for(n = 0 ; XtasPathsMenuView[n].label != NULL ; n++)
     XtasPathsMenuFile[n].callback_data    = (XtPointer)tas_winfos;
   for(n = 0 ; XtasPathsMenuTools[n].label != NULL ; n++)
     XtasPathsMenuTools[n].callback_data   = (XtPointer)tas_winfos;
   for(n = 0 ; XtasPathsMenuOptions[n].label != NULL ; n++)
     XtasPathsMenuOptions[n].callback_data = (XtPointer)tas_winfos;

     /*-------------------*/
    /* Create a menu_bar */
   /*-------------------*/

   n = 0;
   menu_bar = XmCreateMenuBar( parent, "XtasMenuBar", args, n );

   XalBuildMenus( menu_bar,         /* The Menu Bar widget id               */
                   XtasPathsMenu );  /* The main menu XalMenus structure     */

  return( menu_bar );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathColumnsTitleMoveWithHSBCallback                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasPathColumnsTitleMoveWithHSBCallback (hsb, label, call_data)
    Widget      hsb;
    XtPointer   label;
    XtPointer   call_data;
{
    int         loffset ;
    int         value;

    if (XTAS_DISPLAY_NEW == 0)
        loffset = XTAS_PATH_FROM_LOFFSET_G;
    else
        loffset = XTAS_PATH_FROM_LOFFSET_T;

    XtVaGetValues (hsb, XmNvalue, &value, NULL);

    XtVaSetValues (label, XmNleftOffset, (loffset - value), NULL);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreatePathsList                                            */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .params : The get paths parameters.                            */
/*            .paths_list : The paths list description.                      */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation of the signals window objects.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreatePathsList( parent, XtasPathSet, tas_winfos )

 Widget                  parent;
 XtasPathListSetStruct  *XtasPathSet;
 XtasWindowStruct       *tas_winfos;

{
   Widget        label_widget; 
   Widget        FROM_label_widget; 
   Widget        list_widget; 
   Widget        form_t;
   Widget        form, form1, form2;
   Widget        dbase_form;
   Widget        pan_widget;
   Widget        form_widget;
   Widget        buttons_form;
   Widget        button;
   Widget        frame;
   Widget        separator1;
   Widget        separator2;
   Widget        tmp;
   Pixmap        pixmap;
   XmString      text;
   Pixel         tas_state_color;
   Pixel         tas_backg_color;
   Arg           args[20];
   int           n;
   char          text_info[1024];
   ttvsig_list  *head;

   Widget        scrollW;
 
     /*-------------------------*/
    /* my var                  */
   /*-------------------------*/
   char          value[1024];
   int           lengthname1 ;
   int           lengthname2 ;
   chain_list   *headchain;
   ttvpath_list *path ;
   /* for paned windows */
   Widget        paned_window ;  
   Widget        form1_window ;  
   Widget        form2_window ;  


      /*-----------------------/---------------------------------------*/
     /* Create a Paned Window to split the Main window into many rows */
    /*-----------------------/---------------------------------------*/
   /*===== New Widget =====*/   
   form_widget = XtVaCreateWidget( "XtasMainPan",
                                    xmFormWidgetClass,
                                    parent,
                                    XmNtopAttachment,    XmATTACH_FORM,
                                    XmNbottomAttachment, XmATTACH_POSITION,
                                    XmNbottomPosition,   99,
                                    XmNleftAttachment,   XmATTACH_POSITION,
                                    XmNleftPosition,     1,
                                    XmNrightAttachment,  XmATTACH_POSITION,
                                    XmNrightPosition,    99,
                                   NULL);

      /*-----------------------/------------------------*/
     /* Create a form to manage the first row children */
    /*-----------------------/------------------------*/
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM  ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM  ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM  ); n++;
   buttons_form = XtCreateManagedWidget( "XtasMainButtons", 
                                          xmFormWidgetClass,
                                          form_widget,
                                          args, n );

   for ( n=0; XtasPathsButtonMenus[n].pix_file != -1 ; n++ )
     XtasPathsButtonMenus[n].callback_data = (XtPointer)tas_winfos ;

   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNnumColumns,  1            ); n++;
   XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
   tas_winfos->butwidget = XalButtonMenus( buttons_form, 
                                           XtasPathsButtonMenus, 
                                           args, n,
                                           40, 40 );
   XtasAddDummyButton( tas_winfos->butwidget );
   XtManageChild( tas_winfos->butwidget );
   //XmAddTabGroup( tas_winfos->butwidget );
   
    /*-------- separator ---*/
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,       buttons_form    ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNleftOffset,      3               ); n++;
   XtSetArg( args[n], XmNrightOffset,     3               ); n++;
   separator1 = XtCreateManagedWidget( "XtasMainButtons",
                                        xmSeparatorWidgetClass,
                                        form_widget,
                                        args, n );
   
      /*-----------------------/--*/
     /* Create the message area  */
    /*-----------------------/--*/
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNtopOffset,        2             ); n++;
   XtSetArg( args[n], XmNleftOffset,       2             ); n++;
   XtSetArg( args[n], XmNrightOffset,      2             ); n++;
   form = XtCreateManagedWidget( "XtasShellSubForm",
                                  xmFormWidgetClass,
                                  form_widget,
                                  args, n );
   
   label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass, form,
                                            XmNtopAttachment,   XmATTACH_FORM,
                                            XmNleftAttachment,  XmATTACH_FORM,
                                            XmNleftOffset,      2,
                                           NULL);
   XtasSetLabelString( label_widget, "Informations Area" );
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
   XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ); n++;
   XtSetArg( args[n], XmNrightPosition,    48                ); n++;
   frame = XtCreateManagedWidget( "XtasFrame",
                                   xmFrameWidgetClass,
                                   form,
                                   args, n );

   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNheight, 20 ); n++;
   XtasPathSet->INFO_USER = XtCreateManagedWidget( "XtasUserInfo",
                                                    xmLabelWidgetClass,
                                                    frame,
                                                    args, n);
   XtasSetLabelString( XtasPathSet->INFO_USER, XTAS_NULINFO );
  
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,        frame                    ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM            ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ); n++; 
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ); n++;
   XtSetArg( args[n], XmNleftWidget,       frame                    ); n++;
   frame = XtCreateManagedWidget( "XtasFrame",
                                   xmFrameWidgetClass,
                                   form, args, n );

   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNheight, 20 ); n++;
   XtasPathSet->INFO_HELP = XtCreateManagedWidget( "XtashelpInfo",
                                                    xmLabelWidgetClass,
                                                    frame,
                                                    args, n);
   XtasSetLabelString( XtasPathSet->INFO_HELP, XTAS_NULINFO );


      /*----------------------*/
     /* Create a separator   */
    /*----------------------*/
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
   XtSetArg( args[n], XmNbottomWidget,     form            ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNleftOffset,       3               ); n++;
   XtSetArg( args[n], XmNrightOffset,      3               ); n++;
   separator2 = XtCreateManagedWidget( "separator2",
                                        xmSeparatorWidgetClass,
                                        form_widget,
                                        args, n );
   
      /*-----------------------/------------------*/
     /* Paned Window pour les deux scrolled list */
    /*-----------------------/------------------*/
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,        separator1      ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
   XtSetArg( args[n], XmNbottomWidget,     separator2      ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM  ); n++;
   pan_widget = XtCreateManagedWidget( "XtasShellSubForm",
                                        xmPanedWindowWidgetClass,
                                        form_widget,
                                        args, n );
   
      /*-----------------------/------------------------*/
     /* Create a form to manage the next pan  children */
    /*-----------------------/------------------------*/
   /*===== New Widget =====*/   
   n = 0;
   form = XtCreateManagedWidget( "XtasShellSubForm",
                                  xmFormWidgetClass,
                                  pan_widget,
                                  args, n );
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
   label_widget = XtCreateManagedWidget( "XtasSubTitles",
                                          xmLabelGadgetClass,
                                          form,
                                          args, n );
   XtasSetLabelString( label_widget, "Search Parameters");
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET   ); n++;
   XtSetArg( args[n], XmNtopWidget,       label_widget      ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM     ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_POSITION ); n++;
   XtSetArg( args[n], XmNrightPosition,   20                ); n++;
   frame = XtCreateManagedWidget( "XtasFrame",
                                   xmFrameWidgetClass,
                                   form,
                                   args, n );
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
   label_widget = XtCreateManagedWidget( "XtasFrame",
                                          xmLabelWidgetClass,
                                          frame,
                                          args, n );
   if ( (XtasPathSet->PARAM->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL )
   {
      XtasSetLabelString( label_widget, "Search from Start");
   }
   else
   {
      XtasSetLabelString( label_widget, "Search from End");
   }
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,        frame                    ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
   XtSetArg( args[n], XmNbottomWidget,     frame                    ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ); n++;
   XtSetArg( args[n], XmNleftWidget,       frame                    ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION        ); n++;
   XtSetArg( args[n], XmNrightPosition,    40                       ); n++;
   frame = XtCreateManagedWidget( "XtasFrame",
                                   xmFrameWidgetClass,
                                   form,
                                   args, n );

   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
   label_widget = XtCreateManagedWidget( "XtasFrame",
                                          xmLabelWidgetClass,
                                          frame,
                                          args, n );
   if (( XtasPathSet->PARAM->SCAN_TYPE & TTV_FIND_MAX ) == TTV_FIND_MAX )
   {
     XtasSetLabelString( label_widget, "Max");
   }
   else
   {
     XtasSetLabelString( label_widget, "Min");
   }
  
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,        frame                    ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
   XtSetArg( args[n], XmNbottomWidget,     frame                    ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ); n++;
   XtSetArg( args[n], XmNleftWidget,       frame                    ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ); n++;
   frame = XtCreateManagedWidget( "XtasFrame",
                                   xmFrameWidgetClass,
                                   form,
                                   args, n );
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
   label_widget = XtCreateManagedWidget( "XtasFrame",
                                          xmLabelWidgetClass,
                                          frame,
                                          args, n );

   if(XtasPathSet->PARAM->DELAY_MIN == TTV_DELAY_MIN)
    {
     if(XtasPathSet->PARAM->DELAY_MAX == TTV_DELAY_MAX)
       sprintf(text_info, "Time Bounds: None ");
     else
       sprintf(text_info, "Time Bounds: delay < %.0f",
               XtasPathSet->PARAM->DELAY_MAX/TTV_UNIT);
    }
   else if(XtasPathSet->PARAM->DELAY_MAX == TTV_DELAY_MAX)
    {
     sprintf(text_info, "Time Bounds: %.0f < delay",
               XtasPathSet->PARAM->DELAY_MIN/TTV_UNIT);
    }
   else
    {
      sprintf(  text_info, 
               "Time Bounds: %.0f < delay < %.0f",
                XtasPathSet->PARAM->DELAY_MIN/TTV_UNIT, XtasPathSet->PARAM->DELAY_MAX/TTV_UNIT);
    }
   XtasSetLabelString( label_widget, text_info);

   if ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_SBS) == XTAS_SBS )
   {
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNtopWidget,       frame           ); n++;
      XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
      frame = XtCreateManagedWidget( "XtasFrame",
                                      xmFrameWidgetClass,
                                      form,
                                      args, n );

      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ); n++;
      label_widget = XtCreateManagedWidget( "XtasFrame",
                                             xmLabelWidgetClass,
                                             frame,
                                             args, n );
      head = XtasPathSet->PARAM->CUR_SIG->DATA;
      if ((XtasPathSet->PARAM->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      {
         sprintf( text_info, "Current Input Signal: ") ;
         ttv_getsigname( XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                         text_info+strlen( text_info ), head );
      }
      else
      {
          sprintf( text_info, "Current Output Signal: ");
          ttv_getsigname( XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                          text_info+strlen( text_info ), head) ;
      }
      XtasSetLabelString( label_widget, text_info);
      XtasPathSet->INFO_CUR_SIG = label_widget;
      
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNborderWidth,     0               ); n++;
      XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNtopWidget,       frame        ); n++;
      XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
      frame = XtCreateManagedWidget( "XtasShellSubForm",
                                      xmFrameWidgetClass,
                                      form,
                                      args, n );
      
      /*===== New Widget =====*/   
      n = 0;
      form_t = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmFormWidgetClass,
                                       frame,
                                       args, n );
       /*----------------------*/
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNwidth,            60            ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonGadgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatEndCallback,
                     (XtPointer)XtasPathSet);
      XtasSetLabelString( button, " ->| " );
    
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatFastFwdCallback,
                     (XtPointer)XtasPathSet);
      XtasSetLabelString( button, " ->-> " );
    
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatNextCallback,
                     (XtPointer)XtasPathSet);
      XtasSetLabelString( button, " -> " );

      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatMiddleCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " ->|<- " );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatPrevCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " <-" );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatFastRewCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " <-<- " );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasSigPathTreatTopCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " |<- " );
   
       /*------ titre ---------*/
      /*===== New Widget =====*/   
      label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                              xmLabelWidgetClass, form_t,
                                              XmNtopAttachment,    XmATTACH_FORM,
                                              XmNbottomAttachment, XmATTACH_FORM,
                                              XmNleftAttachment,   XmATTACH_FORM,
                                              XmNtopOffset,        2,
                                              XmNrightOffset,      2,
                                              XmNleftOffset,       2,
                                             NULL);
   
      if ((XtasPathSet->PARAM->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
      {
         XtasSetLabelString( label_widget, "Input Signals List" );
      }
      else
      {
         XtasSetLabelString( label_widget, "Output Signals List" );
      }

      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNleftWidget,       label_widget    ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            100             ); n++;
      XtasPathSet->SIGNAL_LIST_SET->INFO_PAGE 
              = XtCreateManagedWidget( "XtasShellSubForm",
                                        xmLabelWidgetClass,
                                        form_t,
                                        args, n );
      XtasSetLabelString( XtasPathSet->SIGNAL_LIST_SET->INFO_PAGE, XTAS_NULINFO);
      
      /*===== New Widget =====*/   
      n=0;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNtopWidget,        frame           ); n++;
      XtSetArg( args[n], XmNtopOffset,        2               ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNscrollingPolicy,  XmSTATIC        ); n++;
      XtSetArg( args[n], XmNheight,           90              ); n++;
      list_widget = XmCreateScrolledList( form,
                                         "XtasUserField",
                                          args, n );
      XtManageChild( list_widget );
      XtAddCallback( list_widget,
                     XmNdefaultActionCallback,
                     XtasSigPathSelectCallback,
                     (XtPointer)XtasPathSet );
      XtAddCallback( list_widget,
                     XmNbrowseSelectionCallback,
                     XtasSigPathSelectCallback,
                     (XtPointer)XtasPathSet );
      //XmAddTabGroup( list_widget );
      XtasPathSet->SIGNAL_LIST_SET->SIGLIST_SCROLL = list_widget;
      XtasFillSigPathList( XtasPathSet );
      

    /*--------- deuxieme form/ pour les chemins -------------------------------*/
   /*===== New Widget =====*/   
   n = 0;
   form = XtCreateManagedWidget( "XtasShellSubForm",
                                  xmFormWidgetClass,
                                  pan_widget,
                                  args, n );
   
   n = 0;
   XtSetArg( args[n], XmNborderWidth,     0             ); n++;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
   }
   else
   {
   n = 0;
   XtSetArg( args[n], XmNborderWidth,     0               ); n++;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,       frame           ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
   }

   XtSetArg( args[n], XmNheight,          30              ); n++;
   XtSetArg( args[n], XmNalignment,       XmALIGNMENT_BEGINNING); n++;
   label_widget = XtCreateManagedWidget( "XtasShellSubForm",
                                   xmLabelWidgetClass,
                                   form,
                                   args, n );
   XtasSetLabelString( label_widget, "Path Information" );

   n = 0;
   XtSetArg( args[n], XmNborderWidth,     0                 ); n++;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET   ); n++;
   XtSetArg( args[n], XmNtopWidget,       label_widget      ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM     ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM     ); n++;
   frame = XtCreateManagedWidget( "XtasShellSubForm",
                                   xmFrameWidgetClass,
                                   form,
                                   args, n );   
   n = 0;
   form = XtCreateManagedWidget( "XtasPathInfosForm",
                                   xmFormWidgetClass,
                                   frame,
                                   args, n );   
   
/*----------------------------------------------------------------------------------------*/   
   n = 0;
   XtSetArg (args[n], XmNbottomAttachment,  XmATTACH_FORM   ); n++;
   XtSetArg (args[n], XmNleftAttachment,    XmATTACH_FORM   ); n++;
   XtSetArg (args[n], XmNleftOffset,        10              ); n++;
   XtSetArg (args[n], XmNrightAttachment,   XmATTACH_FORM   ); n++;
   form1 = XtCreateManagedWidget( "XtasPathInfosForm", xmFormWidgetClass, form, args, n );

   /* Information "Data Lag" */
   n = 0;
   XtSetArg ( args[n], XmNbottomAttachment,  XmATTACH_FORM   ); n++;
   XtSetArg ( args[n], XmNbottomOffset,      6               ); n++;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_FORM   ); n++;
   XtSetArg ( args[n], XmNheight,            25              ); n++;
   XtSetArg ( args[n], XmNwidth,             100             ); n++;
   label_widget = XtCreateManagedWidget( "XtasShellSubForm", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "Data Lag:" );
   
   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNtopWidget,        label_widget                ); n++;
   XtSetArg ( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNbottomWidget,     label_widget                ); n++;
   XtSetArg ( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
   XtSetArg ( args[n], XmNleftWidget,       label_widget                ); n++;
   XtSetArg ( args[n], XmNleftOffset,       2                           ); n++;
   XtSetArg ( args[n], XmNrightAttachment,  XmATTACH_POSITION           ); n++;
   XtSetArg ( args[n], XmNrightPosition,    49                          ); n++;
   XtSetArg ( args[n], XmNalignment,        XmALIGNMENT_BEGINNING       ); n++;
   label_widget = XtCreateManagedWidget( "XtasPathInfosDataLagLabel", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "" );
   
   /* Information "Latch" */
   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNtopWidget,        label_widget                ); n++;
   XtSetArg ( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNbottomWidget,     label_widget                ); n++;
   XtSetArg ( args[n], XmNrightAttachment,  XmATTACH_FORM               ); n++;
   XtSetArg ( args[n], XmNleftAttachment,   XmATTACH_POSITION           ); n++;
   XtSetArg ( args[n], XmNleftPosition,     50                          ); n++;
   XtSetArg ( args[n], XmNalignment,        XmALIGNMENT_BEGINNING       ); n++;
   label_widget = XtCreateManagedWidget( "XtasPathInfosLatchLabel", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, " " );


   n = 0;
   XtSetArg (args[n], XmNbottomAttachment,  XmATTACH_WIDGET ); n++;
   XtSetArg (args[n], XmNbottomWidget,      form1           ); n++;
   XtSetArg (args[n], XmNleftAttachment,    XmATTACH_FORM   ); n++;
   XtSetArg (args[n], XmNleftOffset,        10              ); n++;
   XtSetArg (args[n], XmNrightAttachment,   XmATTACH_FORM   ); n++;
   form1 = XtCreateManagedWidget( "XtasPathInfosForm", xmFormWidgetClass, form, args, n );

   /* Information "Starting Slope" */
   n = 0;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_FORM      ); n++;
   XtSetArg ( args[n], XmNheight,            25                 ); n++;
   XtSetArg ( args[n], XmNwidth,             100                ); n++;
   label_widget = XtCreateManagedWidget( "XtasShellSubForm", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "Starting Slope:" );

   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNtopWidget,         label_widget                ); n++;
   XtSetArg ( args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNbottomWidget,      label_widget                ); n++;
   XtSetArg ( args[n], XmNrightAttachment,   XmATTACH_POSITION           ); n++;
   XtSetArg ( args[n], XmNrightPosition,     49                          ); n++;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_WIDGET             ); n++;
   XtSetArg ( args[n], XmNleftWidget,        label_widget                ); n++;
   XtSetArg ( args[n], XmNleftOffset,        2                           ); n++;
   XtSetArg ( args[n], XmNalignment,        XmALIGNMENT_BEGINNING       ); n++;
   label_widget = XtCreateManagedWidget( "XtasPathInfosSSlopeLabel",
                                   xmLabelWidgetClass,
                                   form1,
                                   args, n );
   XtasSetLabelString( label_widget, "" );

   /* Information "Starting Time" */
   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET   ); n++;
   XtSetArg ( args[n], XmNtopWidget,         label_widget               ); n++;
   XtSetArg ( args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET   ); n++;
   XtSetArg ( args[n], XmNbottomWidget,      label_widget               ); n++;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_POSITION          ); n++;
   XtSetArg ( args[n], XmNleftPosition,      50                         ); n++;
   XtSetArg ( args[n], XmNwidth,             100                        ); n++;
   label_widget = XtCreateManagedWidget( "XtasShellSubForm", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "Starting Time:" );
   
   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNtopWidget,         label_widget                ); n++;
   XtSetArg ( args[n], XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET    ); n++;
   XtSetArg ( args[n], XmNbottomWidget,      label_widget                ); n++;
   XtSetArg ( args[n], XmNrightAttachment,   XmATTACH_FORM               ); n++;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_WIDGET             ); n++;
   XtSetArg ( args[n], XmNleftWidget,        label_widget                ); n++;
   XtSetArg ( args[n], XmNleftOffset,        2                           ); n++;
   XtSetArg ( args[n], XmNalignment,        XmALIGNMENT_BEGINNING       ); n++;
   label_widget = XtCreateManagedWidget( "XtasPathInfosSTimeLabel", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "" );

   n = 0;
   XtSetArg (args[n], XmNbottomAttachment,  XmATTACH_WIDGET ); n++;
   XtSetArg (args[n], XmNbottomWidget,      form1           ); n++;
   XtSetArg (args[n], XmNleftAttachment,    XmATTACH_FORM   ); n++;
   XtSetArg (args[n], XmNleftOffset,        10              ); n++;
   XtSetArg (args[n], XmNrightAttachment,   XmATTACH_FORM   ); n++;
   form1 = XtCreateManagedWidget( "XtasPathInfosForm", xmFormWidgetClass, form, args, n );

    /* information "From" */
   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,     XmATTACH_FORM   ); n++;
   XtSetArg ( args[n], XmNtopOffset,         6               ); n++;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_FORM   ); n++;
   XtSetArg ( args[n], XmNheight,            25              ); n++;
   XtSetArg ( args[n], XmNwidth,             100             ); n++;
   label_widget = XtCreateManagedWidget( "XtasShellSubForm", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "From:" );


   XtVaGetValues(  label_widget,
                   XmNforeground,
                  &tas_state_color,
                   XmNbackground,
                  &tas_backg_color,
                 NULL );

   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,    XmATTACH_FORM               ); n++;
   XtSetArg ( args[n], XmNrightAttachment,  XmATTACH_POSITION           ); n++;
   XtSetArg ( args[n], XmNrightPosition,    49                          ); n++;
   XtSetArg ( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
   XtSetArg ( args[n], XmNleftWidget,       label_widget                ); n++;
   XtSetArg ( args[n], XmNleftOffset,       2                           ); n++;
   XtSetArg ( args[n], XmNeditable,         False                       ); n++;
   XtSetArg ( args[n], XmNeditMode,         XmMULTI_LINE_EDIT           ); n++;
   XtSetArg ( args[n], XmNwidth,            300                         ); n++;
   XtSetArg ( args[n], XmNresizeHeight,     True                        ); n++;
   XtSetArg ( args[n], XmNwordWrap,         True                        ); n++;
   XtSetArg ( args[n], XmNvalue,            " "                         ); n++;
   XtSetArg ( args[n], XmNcursorPositionVisible,    False               ); n++;
   XtSetArg ( args[n], XmNbackground,       tas_backg_color             ); n++;
   XtSetArg ( args[n], XmNbottomShadowColor,tas_backg_color             ); n++;
   XtSetArg ( args[n], XmNbottomShadowPixmap,XmUNSPECIFIED_PIXMAP       ); n++;
   XtSetArg ( args[n], XmNtopShadowColor,   tas_backg_color             ); n++;
   XtSetArg ( args[n], XmNtopShadowPixmap,   XmUNSPECIFIED_PIXMAP       ); n++;
   label_widget = XtCreateManagedWidget( "XtasPathInfosFromLabel", xmTextWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "" );
   
    /* information "To" */
   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,     XmATTACH_FORM      ); n++;
   XtSetArg ( args[n], XmNtopOffset,         6                  ); n++;
   XtSetArg ( args[n], XmNleftAttachment,    XmATTACH_POSITION  ); n++;
   XtSetArg ( args[n], XmNleftPosition,      50                 ); n++;
   XtSetArg ( args[n], XmNheight,            25                 ); n++;
   XtSetArg ( args[n], XmNwidth,             100                ); n++;
   label_widget = XtCreateManagedWidget( "XtasShellSubForm", xmLabelWidgetClass, form1, args, n );
   XtasSetLabelString( label_widget, "To:" );

   

   n = 0;
   XtSetArg ( args[n], XmNtopAttachment,    XmATTACH_FORM               ); n++;
   XtSetArg ( args[n], XmNrightAttachment,  XmATTACH_FORM               ); n++;
   XtSetArg ( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
   XtSetArg ( args[n], XmNleftWidget,       label_widget                ); n++;
   XtSetArg ( args[n], XmNleftOffset,       2                           ); n++;
   XtSetArg ( args[n], XmNeditable,         False                       ); n++;
   XtSetArg ( args[n], XmNeditMode,         XmMULTI_LINE_EDIT           ); n++;
   XtSetArg ( args[n], XmNwidth,            300                         ); n++;
   XtSetArg ( args[n], XmNresizeHeight,     True                        ); n++;
   XtSetArg ( args[n], XmNwordWrap,         True                        ); n++;
   XtSetArg ( args[n], XmNvalue,            "  "                        ); n++;
   XtSetArg ( args[n], XmNcursorPositionVisible,    False               ); n++;
   XtSetArg ( args[n], XmNbackground,       tas_backg_color             ); n++;
   XtSetArg ( args[n], XmNbottomShadowColor,tas_backg_color             ); n++;
   XtSetArg ( args[n], XmNbottomShadowPixmap,XmUNSPECIFIED_PIXMAP       ); n++;
   XtSetArg ( args[n], XmNtopShadowColor,   tas_backg_color             ); n++;
   XtSetArg ( args[n], XmNtopShadowPixmap,   XmUNSPECIFIED_PIXMAP       ); n++;
   label_widget = XtCreateManagedWidget( "XtasPathInfosToLabel", xmTextWidgetClass, form1, args, n );
   

   
   
   n = 0;
   form = XtCreateManagedWidget( "XtasShellSubForm",
                                  xmFormWidgetClass,
                                  pan_widget,
                                  args, n );
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNborderWidth,     0             ); n++;
   XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
   frame = XtCreateManagedWidget( "XtasShellSubForm",
                                   xmFrameWidgetClass,
                                   form,
                                   args, n );
   /*===== New Widget =====*/   
   n = 0;
   form_t = XtCreateManagedWidget( "XtasShellSubForm",
                                    xmFormWidgetClass,
                                    frame,
                                    args, n );
  
//   if (((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_SBS) == XTAS_SBS) ||
//       ( XtasPathSet->PARAM->CRITIC == 'P'  ))
//   {
     if (!XTAS_DISPLAY_NEW) {
       /*----------------------*/
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
      XtSetArg( args[n], XmNwidth,            60            ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatEndCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " ->| " );
      
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatFastFwdCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " ->-> " );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatNextCallback,
                     (XtPointer)XtasPathSet);
      XtasSetLabelString( button, " -> " );
   
      /*===== New Widget =====*/   
/*      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatMiddleCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " ->|<- " );*/
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatPrevCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " <- " );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatFastRewCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " <-<- " );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            60              ); n++;
      button = XtCreateManagedWidget( "XtasShellSubForm",
                                       xmPushButtonWidgetClass,
                                       form_t,
                                       args, n );
      XtAddCallback( button,
                     XmNactivateCallback,
                     XtasPathTreatTopCallback,
                     (XtPointer)XtasPathSet );
      XtasSetLabelString( button, " |<- " );
   
      /*===== New Widget =====*/   
      n = 0;
      XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET ); n++;
      XtSetArg( args[n], XmNrightWidget,      button          ); n++;
      XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
      XtSetArg( args[n], XmNwidth,            100             ); n++;
      label_widget = XtCreateManagedWidget( "XtasShellSubForm",
                                             xmLabelWidgetClass,
                                             form_t,
                                             args, n );
      XtasSetLabelString( label_widget, "" );
     }      
      /*--------------------- titre ---------------------------------------*/
      
      if((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_SBS) == XTAS_SBS) {
          if ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS)
            text = XmStringCreateSimple( "Paths List" );
          else /*access*/
            text = XmStringCreateSimple( "Access List" );
      }
      else if (XtasPathSet->PARAM->CRITIC == 'P')
       text = XmStringCreateSimple( "Parallel Paths List" );
//   }
   else if ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_CRITIC) == XTAS_CRITIC)
   {
      if ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS)
        text = XmStringCreateSimple( "Critic Path List" );
      else /* access */
        text = XmStringCreateSimple( "Critic Access List" );
   }
   else
   {
      if ((XtasPathSet->PARAM->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS)
        text = XmStringCreateSimple( "All Paths" );
      else /* access */
        text = XmStringCreateSimple( "All Access" );
   }
       
   
   /*===== New Widget =====*/   
   label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass,
                                            form_t,
                                            XmNlabelString,      text,
                                            XmNtopAttachment,    XmATTACH_FORM,
                                            XmNbottomAttachment, XmATTACH_FORM,
                                            XmNleftAttachment,   XmATTACH_FORM,
                                            XmNtopOffset,        2,
                                            XmNrightOffset,      2,
                                            XmNleftOffset,       2,
                                           NULL);
   XmStringFree( text );   
   
   /*===== New Widget =====*/   
   n = 0;
   XtSetArg( args[n], XmNborderWidth,      0               ); n++;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
   XtSetArg( args[n], XmNtopWidget,        frame           ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
   XtSetArg( args[n], XmNrightOffset,      2               ); n++;
   dbase_form = XtCreateManagedWidget( "XtasTitleForm",
                                        xmFormWidgetClass,
                                        form,
                                        args, n );
   
   /* longueur des noms des signaux */
   headchain = XtasCreatePathTabIndex( XtasPathSet );
   path = XtasPathSet->CUR_PATH_WIN->DATA;

   XtasCarLength1 = XTAS_MIN_CARLENGTH;
   XtasCarLength2 = XTAS_MIN_CARLENGTH;
   while ( headchain != NULL ) {
      lengthname1 = strlen(ttv_getsigname(XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                           value,path->NODE->ROOT)); /* longueur du signal source */
      lengthname2 = strlen(ttv_getsigname( XtasPathSet->PARAM->ROOT_SIG_EDGE->TOP_FIG,
                           value, path->ROOT->ROOT)); /* longueur du signal destinataire */
       if(lengthname1 >= XtasCarLength1)
           XtasCarLength1 = lengthname1;
       if(lengthname2 >= XtasCarLength2)
           XtasCarLength2 = lengthname2;
      path = path->NEXT;
      headchain = headchain->NEXT;
   }
   if(XtasCarLength1 >= XTAS_MAX_CARLENGTH)
       XtasCarLength1 = XTAS_MAX_CARLENGTH;
   if(XtasCarLength2 >= XTAS_MAX_CARLENGTH)
       XtasCarLength2 = XTAS_MAX_CARLENGTH;

   /*===== New Widget =====*/   
   if(!XTAS_DISPLAY_NEW) 
   {

   text = XmStringCreateSimple( "From" );
   FROM_label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                           xmLabelWidgetClass,
                                           dbase_form,
                                           XmNlabelString,    text,
                                           XmNtopAttachment,  XmATTACH_FORM,
                                           XmNleftAttachment, XmATTACH_FORM,
                                           XmNwidth,          150,
                                           XmNtopOffset,      2,
                                           XmNleftOffset,     XTAS_PATH_FROM_LOFFSET_G,
                                           XmNheight,         25,
                                          NULL);
   XmStringFree( text );

   /*===== New Widget =====*/   
   text = XmStringCreateSimple( "To" );

   label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass,
                                            dbase_form,
                                            XmNlabelString,    text,
                                            XmNtopAttachment,  XmATTACH_FORM,
                                            XmNleftAttachment, XmATTACH_WIDGET,
                                            XmNleftWidget,     FROM_label_widget,
                                            XmNwidth,          150,
                                            XmNtopOffset,      2,
                                            XmNleftOffset,     49,
                                            XmNheight,         25,
                                           NULL);
   XmStringFree( text );
   
   /*===== New Widget =====*/   
   text = XmStringCreateSimple( "Output Slope" );
   label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass,
                                            dbase_form,
                                            XmNlabelString,    text,
                                            XmNtopAttachment,  XmATTACH_FORM,
                                            XmNleftAttachment, XmATTACH_WIDGET,
                                            XmNleftWidget,     label_widget,
                                            XmNwidth,          100,
                                            XmNtopOffset,      2,
                                            XmNleftOffset,     10,
                                            XmNheight,         25,
                                           NULL);
   XmStringFree( text );
   
   /*===== New Widget =====*/   
   pixmap = XalGetPixmap( label_widget, 
                         XTAS_TIME_MAP,
                         tas_state_color,
                         tas_backg_color );
   
   if ( pixmap == XmUNSPECIFIED_PIXMAP )
   {
      text = XmStringCreateSimple( "Time" );
      label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                              xmLabelWidgetClass,
                                              dbase_form,
                                              XmNlabelString,    text,
                                              XmNtopAttachment,  XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_WIDGET,
                                              XmNleftWidget,     label_widget,
                                              XmNwidth,          100,
                                              XmNtopOffset,      2,
                                              XmNrightOffset,    2,
                                              XmNleftOffset,     2,
                                              XmNheight,         25,
                                             NULL);

       XmStringFree( text );
   } 
   else
   {
      label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass, dbase_form,
                                            XmNlabelType,       XmPIXMAP,
                                            XmNlabelPixmap,     pixmap,
                                            XmNtopAttachment,  XmATTACH_FORM,
                                            XmNleftAttachment, XmATTACH_WIDGET,
                                            XmNleftWidget,     label_widget,
                                            XmNwidth,           100,
                                            XmNtopOffset,       2,
                                            XmNrightOffset,     2,
                                            XmNleftOffset,      2,
                                            XmNheight,          25,
                                           NULL);
   }
   /*===== New Widget =====*/   
    n=0;
    XtSetArg( args[n], XmNtopAttachment,            XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget,                label_widget    ); n++;
    XtSetArg( args[n], XmNbottomAttachment,         XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,           XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,          XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNscrollBarPlacement,       XmBOTTOM_RIGHT  ); n++;
    XtSetArg( args[n], XmNscrollBarDisplayPolicy,   XmAS_NEEDED     ); n++;
    XtSetArg( args[n], XmNscrollingPolicy,          XmAUTOMATIC     ); n++;
    list_widget = XmCreateScrolledWindow(  dbase_form,
                                          "XtasScrollWindow",
                                           args, n );
    XtVaGetValues (list_widget, XmNhorizontalScrollBar, &tmp, NULL);
    XtAddCallback (tmp, XmNvalueChangedCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNdragCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNincrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNdecrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNpageIncrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNpageDecrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNtoTopCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNtoBottomCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtManageChild( list_widget );
    XtasPathSet->PATHLIST_SCROLL = list_widget;
    
    XtasFillPathList( XtasPathSet );
   }
   else
   {
   /*===== New Widget =====*/   
   text = XmStringCreateSimple( "From" );
   FROM_label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                           xmLabelWidgetClass,
                                           dbase_form,
                                           XmNlabelString,    text,
                                           XmNtopAttachment,  XmATTACH_FORM,
                                           XmNleftAttachment, XmATTACH_FORM,
                                           XmNwidth,          XtasCarLength1*7,
                                           XmNtopOffset,      2,
                                           XmNrightOffset,    2,
                                           XmNleftOffset,     XTAS_PATH_FROM_LOFFSET_T,
                                           XmNheight,         25,
                                          NULL);
   XmStringFree( text );

   /*===== New Widget =====*/   
   text = XmStringCreateSimple( "To" );

   label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass,
                                            dbase_form,
                                            XmNlabelString,    text,
                                            XmNtopAttachment,  XmATTACH_FORM,
                                            XmNleftAttachment, XmATTACH_WIDGET,
                                            XmNleftWidget,     FROM_label_widget,
                                            XmNwidth,          XtasCarLength2*7,
                                            XmNtopOffset,      2,
                                            XmNrightOffset,    2,
                                            XmNleftOffset,     60,
                                            XmNheight,         25,
                                           NULL);
   XmStringFree( text );
   
   /*===== New Widget =====*/   
   text = XmStringCreateSimple( "SLOPE" );
   label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass,
                                            dbase_form,
                                            XmNlabelString,    text,
                                            XmNtopAttachment,  XmATTACH_FORM,
                                            XmNleftAttachment, XmATTACH_WIDGET,
                                            XmNleftWidget,     label_widget,
                                            XmNwidth,           80,
                                            XmNtopOffset,      2,
                                            XmNrightOffset,    2,
                                            XmNleftOffset,     20,
                                            XmNheight,         25,
                                           NULL);
   XmStringFree( text );
   
   /*===== New Widget =====*/   
   XtVaGetValues(  label_widget,
                   XmNforeground,
                  &tas_state_color,
                   XmNbackground,
                  &tas_backg_color,
                 NULL );
   pixmap = XalGetPixmap( label_widget, 
                         XTAS_TIME_MAP,
                         tas_state_color,
                         tas_backg_color );
   
   if ( pixmap == XmUNSPECIFIED_PIXMAP )
   {
      text = XmStringCreateSimple( "Time" );
      label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                              xmLabelWidgetClass,
                                              dbase_form,
                                              XmNlabelString,    text,
                                              XmNtopAttachment,  XmATTACH_FORM,
                                              XmNleftAttachment, XmATTACH_WIDGET,
                                              XmNleftWidget,     label_widget,
                                              XmNwidth,           80,
                                              XmNtopOffset,      2,
                                              XmNrightOffset,    2,
                                              XmNleftOffset,     2,
                                              XmNheight,         25,
                                             NULL);

       XmStringFree( text );
   } 
   else
   {
      label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                            xmLabelWidgetClass, dbase_form,
                                            XmNlabelType,       XmPIXMAP,
                                            XmNlabelPixmap,     pixmap,
                                            XmNtopAttachment,  XmATTACH_FORM,
                                            XmNleftAttachment, XmATTACH_WIDGET,
                                            XmNleftWidget,     label_widget,
                                            XmNwidth,           80,
                                            XmNtopOffset,       2,
                                            XmNrightOffset,     2,
                                            XmNleftOffset,      2,
                                            XmNheight,          25,
                                           NULL);
   }
    /* creation of a paned windows which will contain 2 infos */
    n=0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget    ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNheight,           250             ); n++;
    paned_window = XtCreateManagedWidget( "XtasPanedWindow",
                                           xmPanedWindowWidgetClass,
                                           dbase_form,
                                           args, n );

    form1_window = XtVaCreateManagedWidget (       "XtasSubForm",
                                                    xmFormWidgetClass,
                                                    paned_window,
                                                    NULL) ; 

    n=0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNscrollingPolicy,  XmSTATIC        ); n++;
    XtSetArg( args[n], XmNheight,           170             ); n++;
    list_widget = XmCreateScrolledList(  form1_window,
                                        "XtasTextualDisplay",
                                         args, n );

    scrollW = XtParent (list_widget); /* on recupere la ScrolledWindow associee a la ScrolledList */
    XtVaGetValues (scrollW, XmNhorizontalScrollBar, &tmp, NULL);
    XtAddCallback (tmp, XmNvalueChangedCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNdragCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNincrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNdecrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNpageIncrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNpageDecrementCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNtoTopCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
    XtAddCallback (tmp, XmNtoBottomCallback, XtasPathColumnsTitleMoveWithHSBCallback, FROM_label_widget);
     
    XtManageChild( list_widget );
    XtasPathSet->PATHLIST_SCROLL = list_widget;
    
    XtasFillPathList2( XtasPathSet );

    /* form2_window contient les deux subforms */
    /* pour afficher les infos des signaux     */
    form2_window = XtVaCreateManagedWidget (       "XtasSubForm",
                                                    xmFormWidgetClass,
                                                    paned_window,
                                                    XmNtopAttachment,    XmATTACH_WIDGET,
                                                    XmNtopWidget    ,    form1_window ,
                                                    NULL) ; 
    XtasPathSet->PATHLIST_2INFO = form2_window ;
   }
   XtManageChild( form_widget );
   return( form_widget );
}

   
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsList                                                  */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .params : The paths request parameters.                        */
/*            .paths_list : The list of paths to draw.                       */
/*                                                                           */
/* OUT ARGS : The shell widget id.                                           */
/*                                                                           */
/* OBJECT   : The main routine of Paths   List Main window command           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
XtasWindowStruct *XtasPathsList( parent, params )
Widget                      parent;
XtasSearchPathParamStruct  *params;
{
   Widget                         main_window;
   Arg                            args[20];
   int                            n;
   int                            i;
   XtasPathListSetStruct         *XtasPathSet;
   XtasWindowStruct              *tas_winfos = NULL;
   char                           text_title[128];
   ttvpath_list                  *path;

     /*-- initialisation de la structure de controle d'affichage -------------*/
    /*    a ecrire par la suite sous forme d'une macro                       */
   /*-----------------------------------------------------------------------*/

   if (XTAS_PATHSSELECTEDWIDGET != NULL) {
       XtasPathsDeselect (XTAS_PATHSSELECTEDWIDGET);
       XTAS_PATHSSELECTEDWIDGET = NULL;
   }

   XtasPathSet      = XtasNewPathListSet( params ); 

   if ( XtasPathSet->PARAM->LOOK_PATH != NULL )
   /*------------ chemins paralleles ----------------*/
   {
      XtasPathSet->PARAM->CRITIC = 'P'; 
      if ( XtasTtvGetParaList( XtasPathSet ) == NULL )
      {
         XtasFreePathListSet( XtasPathSet );
         return( NULL );
      }
      n = 0;
      XtSetArg( args[n], XmNwidth, 690 ); n++;
      sprintf (text_title, XTAS_NAME": Parallel Paths Window - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
      XtSetArg( args[n], XmNtitle, text_title ); n++;
      for(i = 0 ; XtasPathsMenuHelp[i].label != NULL ; i++){
        XtasPathsMenuHelp[i].callback_data = (XtPointer) ((long)XtasPathsMenuHelp[i].callback_data & (~XTAS_SRC_PATHS));
        XtasPathsMenuHelp[i].callback_data = (XtPointer) ((long)XtasPathsMenuHelp[i].callback_data | XTAS_SRC_PARA);
      }
      
   }
   /*------------ chemins a delay determine ---------*/     
   else
   {
      if ( (params->REQUEST_TYPE & XTAS_SBS) == XTAS_SBS )
      {
          if (/*((params->REQUEST_TYPE & XTAS_ACCESS) == XTAS_ACCESS) ||*/
                  ((params->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL))
              XtasPathSet->SIGNAL_LIST_SET->SIGNAL->TYPE &= ~TTV_SIG_Q;
         if ( XtasTtvGetSigList( XtasPathSet->SIGNAL_LIST_SET ) == NULL )
         {
            XtasFreePathListSet( XtasPathSet );
            return( NULL );
         }
         else
         {
            if ( XtasCleanTtvSigList( XtasPathSet ) == 0 ) 
            {
               return( NULL );
            }
         }
         /*===== New Widget =====*/   
      }
      /*------------ chemins max/min --------------------*/
      else
      {
         if ( XtasTtvGetPathList( XtasPathSet ) == NULL )
         {
            return( NULL );
         }
      }

      n = 0;
      XtSetArg( args[n], XmNwidth, 690); n++;
      sprintf (text_title, XTAS_NAME": Paths Window - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
      XtSetArg( args[n], XmNtitle, text_title ); n++;
   }

   tas_winfos           = (XtasWindowStruct *)mbkalloc( sizeof(XtasWindowStruct) );
   tas_winfos->userdata = XtasPathSet; 
   XTAS_PATH_SESSION = addchain(XTAS_PATH_SESSION,tas_winfos) ;

   tas_winfos->topwidget = XtVaCreatePopupShell( NULL,
                                                  topLevelShellWidgetClass, 
					                              parent,
                                                  XmNallowShellResize, False,
                                                  XmNdeleteResponse,   XmDESTROY,
                                                 NULL );

   HelpFather = tas_winfos->topwidget ;
   XtSetValues( tas_winfos->topwidget, args, n );

   XtasPathSet->TOP_LEVEL = tas_winfos->topwidget;

     /*------------------------------------------------*/
    /* Create the Alliance applications standard Icon */
   /*------------------------------------------------*/
   XalCustomIcon( tas_winfos->topwidget, XTAS_PATH_MAP );

   n = 0;
   main_window = XmCreateMainWindow(  tas_winfos->topwidget,
                                     "XtasMain",
                                      args, n );
   XtManageChild( main_window );

     /*------------------------------------------------------------------*/
    /* Create the menu bar now, so it can be in the motif default areas */
   /*------------------------------------------------------------------*/
   tas_winfos->menwidget = XtasCreatePathsMenus( main_window , tas_winfos);
   XtManageChild( tas_winfos->menwidget );

   tas_winfos->wrkwidget = XtasCreatePathsList( main_window, XtasPathSet, tas_winfos );
   XtManageChild( tas_winfos->wrkwidget );

     /*----------------------------------------*/
    /* Creates a Motif Error and Warning Boxs */
   /*----------------------------------------*/
   tas_winfos->errwidget = XalCreateErrorBox(   tas_winfos->wrkwidget );
   tas_winfos->warwidget = XalCreateWarningBox( tas_winfos->wrkwidget );

   XmMainWindowSetAreas( main_window,
                         tas_winfos->menwidget,
                        (Widget)0,
                        (Widget)0,
                        (Widget)0,
                         tas_winfos->wrkwidget );

   XtPopup( tas_winfos->topwidget, XtGrabNone );
   return ( tas_winfos );
}

