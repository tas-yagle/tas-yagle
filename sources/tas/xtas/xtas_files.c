/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_files.c                                                */
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
/*                                                                          */
/*--------------------------------------------------------------------------*/
Widget XtasDataBaseRequester = 0;
Widget XtasInfFileRequester = 0;
char *XtasDataFileName = NULL ;


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSelectFileOkCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the OK File/New/OpenDataBase         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSelectFileOkCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{

XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *)call_data ;
char *filename;
char *newfile;
char *dir;

XalSetCursor( XtasDeskMainForm, WAIT );
XalForceUpdate( XtasTopLevel );

XmStringGetLtoR( cbs->value, XmSTRING_DEFAULT_CHARSET, &filename ); 
if (*filename != '/')
  { 
    if (XmStringGetLtoR(cbs->dir, XmSTRING_DEFAULT_CHARSET, &dir))
      {
        newfile = XtMalloc(strlen(dir) + 1 + strlen(filename) + 1);
        sprintf( newfile, "%s/%s", dir, filename );
        XtFree( filename );
        XtFree( dir );
        filename = newfile;
      }
  }
XtasDataFileName = filename ;

XtUnmanageChild( widget );
switch ((int)(long)client_data) {
    case XTAS_OPEN_MAP :
        XtasOpenDataBaseOk ();
        break;
    case XTAS_PATH_MAP :
        XtasPathsSelectFileOk ();
        break;
    case XTAS_DETA_MAP :
        XtasDetailSaveSelectFileOk ();
        break;
    case XTAS_STBA_MAP :
        XtasStbSaveSelectFileOk ();
        break ;
}
//XalLeaveLimitedLoop();
XalSetCursor( XtasDeskMainForm, NORMAL );
XalForceUpdate( XtasTopLevel );
if (  XtasDataBaseRequester )
    {
        XtDestroyWidget (XtasDataBaseRequester);
        XtasDataBaseRequester = NULL;
    }
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSelectFileCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File Select                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSelectFileCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
Atom      WM_DELETE_WINDOW;
XmString Filter;
char     file_mask[256];
Arg      args[10];
int             n;
char    *c;

switch ((int)(long)client_data) {
    case    XTAS_OPEN_MAP :
        if ((c=getenv("XTAS_FORMAT_FILTER")) !=NULL)
            strcpy (file_mask, c);
        else
            strcpy(file_mask, "*.[dt]tx");
        break;
    case    XTAS_PATH_MAP :
    case    XTAS_DETA_MAP :
    case    XTAS_STBA_MAP :
        strcpy(file_mask, "*.dat");
        break;
    default :
        strcpy(file_mask, "*");
        return;
}

if (!XtasDataBaseRequester) {
    n = 0;
    XtSetArg( args[n], XmNtitle, "Xtas Data Base Request" ); n++;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL ); n++;


    XtasDataBaseRequester = XmCreateFileSelectionDialog(
							XtParent(widget), 
							"XtasRequesterBox", 
							args, 
							n 
							);
	XtUnmanageChild( XmSelectionBoxGetChild(XtasDataBaseRequester, XmDIALOG_HELP_BUTTON ));
    XtAddCallback( XtasDataBaseRequester, XmNokCallback, XtasSelectFileOkCallback, client_data );
    XtAddCallback( XtasDataBaseRequester, XmNcancelCallback, XtasCancelCallback, (XtPointer)XtasDataBaseRequester );
	HelpFather = XtasDataBaseRequester ;
    

/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/
 
WM_DELETE_WINDOW = XmInternAtom( XtDisplay(XtParent(widget)), "WM_DELETE_WINDOW", False);
XmAddWMProtocolCallback(XtParent(XtasDataBaseRequester), WM_DELETE_WINDOW, XtasCancelCallback, XtasDataBaseRequester);
}

Filter    = XmStringCreateSimple( file_mask );
XtVaSetValues( XtasDataBaseRequester,XmNpattern, Filter, NULL);

if(XtasDataFileName != NULL)
  XtFree(XtasDataFileName) ;

XtasDataFileName = NULL ;

XtManageChild( XtasDataBaseRequester );
//XalLimitedLoop( XtasDataBaseRequester ); 

}

void    XtasOpenInfFile (filename, mode)
char * filename;
{
    XtasPasqua();
    if(sigsetjmp( XtasMyEnv , 1 ) == 0) {
        if (XtasMainParam->ttvfig) {
            inf_ignorename = 1;
            if (mode == 'R') 
                inf_Dirty (XtasMainParam->ttvfig->INFO->FIGNAME);
            _infRead (XtasMainParam->ttvfig->INFO->FIGNAME, filename, 0);
            ttv_getinffile (XtasMainParam->ttvfig);
            tas_update_mcctemp (getloadedinffig(XtasMainParam->ttvfig->INFO->FIGNAME)); /*mise a jour de la temperature pour une eventuelle simu */
            inf_ignorename = 0;
        }
        XtasGetWarningMess() ;
        XtasFirePasqua();
        if (mode == 'R')
            XtasSetLabelString(XtasDeskMessageField, "INF File Replaced");
        else
            XtasSetLabelString(XtasDeskMessageField, "INF File Completed");
        XalForceUpdate( XtasTopLevel );        
    }
    else  {
        XtasGetWarningMess() ;
        XtasFirePasqua();
        XtasSetLabelString(XtasDeskMessageField, "INF File Not Loaded");
        XalForceUpdate( XtasTopLevel );
    }
}

void    XtasSelectInfFileOkCallback (parent, client_data, call_data)
Widget  parent;
XtPointer   client_data;
XtPointer   call_data;
{
    XmFileSelectionBoxCallbackStruct *cbs = (XmFileSelectionBoxCallbackStruct *)call_data ;
    char *filename;
    char *newfile;
    char *dir;
    
    XalSetCursor( XtasTopLevel, WAIT );
    XalForceUpdate( XtasTopLevel );
    
    XmStringGetLtoR( cbs->value, XmSTRING_DEFAULT_CHARSET, &filename ); 
    if (strlen(filename)==0) {
        XalDrawMessage (XtasErrorWidget, "No Selected File");
        return;
    }
    if (*(filename+(strlen(filename)-1)) == '/')  {/* dernier caractere = separateur de repertoire */
        XalDrawMessage (XtasErrorWidget, "No Selected File");
        return;
    }
    if (*filename != '/') { 
        if (XmStringGetLtoR(cbs->dir, XmSTRING_DEFAULT_CHARSET, &dir)) {
            newfile = XtMalloc(strlen(dir) + 1 + strlen(filename) + 1);
            sprintf( newfile, "%s/%s", dir, filename );
            XtFree( filename );
            XtFree( dir );
            filename = newfile;
        }
    }
    XtUnmanageChild( XtasInfFileRequester );

    XtasOpenInfFile (filename, *(char*)client_data);
    
    mbkfree ((char*)client_data);
    XalSetCursor( XtasTopLevel, NORMAL );
    XalForceUpdate( XtasTopLevel );
//    if (  XtasInfFileRequester ) {
        XtDestroyWidget (XtasInfFileRequester);
        XtasInfFileRequester = NULL;
//    }
}
   


void    XtasCompleteCallback (parent, client_data, call_data)
Widget  parent;
XtPointer   client_data;
XtPointer   call_data;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data ;

    if (state->set) 
        *(char*)client_data = 'C';

}

void    XtasReplaceCallback (parent, client_data, call_data)
Widget  parent;
XtPointer   client_data;
XtPointer   call_data;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data ;

    if (state->set) 
        *(char*)client_data = 'R';
}

void XtasSelectInfFileCancelCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
    XtUnmanageChild (XtasInfFileRequester);
    XtDestroyWidget (XtasInfFileRequester);
    XtasInfFileRequester = NULL;
    mbkfree ((char*)client_data);
}

void XtasSelectInfFileCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
    Atom      WM_DELETE_WINDOW;
    XmString /*Filter*/ text;
    char     file_mask[256];
    Arg      args[10];
    int             n;
    static char     *c;
    Widget   row_column, toggle;


    if (XtasMainParam->ttvfig == NULL){
        XalDrawMessage( XtasErrorWidget, XTAS_NODBERR );
        return ;
    }
    
    if (!XtasInfFileRequester) {
        text = XmStringCreateSimple ("*.inf");
        n = 0;
        XtSetArg( args[n], XmNtitle, "Xtas Data Base Request" ); n++;
        XtSetArg( args[n], XmNpattern, text ); n++;
        XtSetArg( args[n], XmNdialogStyle, XmDIALOG_PRIMARY_APPLICATION_MODAL ); n++;
    
        XtasInfFileRequester = XmCreateFileSelectionDialog(
    							XtParent(widget), 
    							"XtasRequesterBox", 
    							args, 
    							n 
    							);
        c = (char*)mbkalloc(1);
        
    	XtUnmanageChild( XmSelectionBoxGetChild(XtasInfFileRequester, XmDIALOG_HELP_BUTTON ));
        XtAddCallback( XtasInfFileRequester, XmNokCallback, XtasSelectInfFileOkCallback, (XtPointer)c );
        XtAddCallback( XtasInfFileRequester, XmNcancelCallback, XtasSelectInfFileCancelCallback, (XtPointer)c );
    	HelpFather = XtasInfFileRequester ;
        XmStringFree (text); 
        
        row_column = XtVaCreateManagedWidget( "Row", xmRowColumnWidgetClass, XtasInfFileRequester,
                        XmNmarginWidth,         40,
                        XmNspacing,         40,
                        XmNpacking,         XmPACK_COLUMN,
                        XmNnumColumns,      1,
                        XmNorientation,     XmHORIZONTAL,
                        XmNisAligned,       True,
                        XmNradioBehavior,   True,
                        XmNradioAlwaysOne,  True,
                        NULL);
            
        text   = XmStringCreateSimple ("Complete");
        toggle = XtVaCreateManagedWidget( "toggle", xmToggleButtonWidgetClass, row_column,
                                XmNindicatorType, XmONE_OF_MANY,
                                XmNspacing,       5,
                                XmNlabelString,   text,
                                XmNset,           False,
                                NULL);
        XmStringFree( text );
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasCompleteCallback, (XtPointer)c );
            
        text   = XmStringCreateSimple ("Replace");
        toggle = XtVaCreateManagedWidget( "toggle", xmToggleButtonWidgetClass, row_column,
                                XmNindicatorType, XmONE_OF_MANY,
                                XmNspacing,       5,
                                XmNlabelString,   text,
                                XmNset,           True,
                                NULL);
        XmStringFree( text );
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasReplaceCallback, (XtPointer)c );
        *c = 'R';
        
    
        /*--------------------------------------------------*/
        /* Add a callback for the WM_DELETE_WINDOW protocol */
        /*--------------------------------------------------*/
     
        WM_DELETE_WINDOW = XmInternAtom( XtDisplay(XtParent(widget)), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasInfFileRequester), WM_DELETE_WINDOW, XtasCancelCallback, XtasInfFileRequester);
    }

    XtManageChild( XtasInfFileRequester );

}


