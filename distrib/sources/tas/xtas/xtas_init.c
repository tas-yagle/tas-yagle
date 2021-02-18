/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_init.c                                                 */
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
XtAppContext		XtasApplicationContext;
char           XTAS_SWITCH_COLOR_MAP = 'N';

char           *WORK_LIB_SAVE = NULL;
char           *spi_sfx = "spi";
char           *vhd_sfx = "vhd";
char           *vlg_sfx = "vlg";

Widget			XtasTopLevel;
Widget			XtasMainWindow;
Widget			XtasDeskMainForm;
Widget			XtasFrontWidget;

XalMessageWidgetStruct  *XtasErrorWidget;
XalMessageWidgetStruct  *XtasWarningWidget;			
XalMessageWidgetStruct  *XtasTraceWidget;			
XalMessageWidgetStruct  *XtasNotYetWidget;			

XtasMainInfoStruct      *XtasMainParam ;

static String XtasFallBacks[] = {
 "*background                           : Grey",
 "*pathMode                             : XmPATH_MODE_RELATIVE",
 "xtas.*.menuBar*background                  : gray64",
 "xtas.*.main*XtasInfosBox.background        : Grey",
 "xtas.*.main*XtasLogoForm*background        : gray64",
 "xtas.*.main*XtasMainTitles*background      : gray64",
 "xtas.*.main*XtasMainTitles*foreground      : Maroon",
 "xtas.*.XalExit.foreground                  : Red",
 "xtas.*.XalErrorBox.foreground              : Red",
 "xtas.*.XalWarningBox.foreground            : DarkOliveGreen",
 "xtas.*.XalOnVersion.foreground             : Blue",
 "xtas.*.XtasMainState.foreground            : DarkOliveGreen",
 "xtas.*.XtasStateForm.borderColor           : DarkOliveGreen",
 "xtas.*.XtasUserField.foreground            : royal blue",
 "xtas.*.XtasUserField.background            : PeachPuff",
 "xtas.*.XtasDrawingArea.background          : Black",
 "xtas.*.XtasHelpField.foreground            : royal blue",
 "xtas.*.XtasHelpField.background            : PeachPuff",
 "xtas.*XtasTextualDisplay*background       : PeachPuff",
 "xtas.*XtasTextualDisplay*foreground       : royal blue",
 "xtas.*XtasTextualDisplay*listSizePolicy   : XmCONSTANT",
 "xtas.*.XtasRequesterBox.Text.background        : PeachPuff",
 "xtas.*.XtasRequesterBox.FilterText.background  : PeachPuff",
 /*
 "xtas.x        : 100",
 "xtas.y        : 300",   
 "xtas.minWidth : 540", 
 "xtas.maxWidth : 660", 
 "xtas.minHeight: 180",  
 "xtas.maxHeight: 335",        
 */
 "xtas.title    : "XTAS_NAME" Main Desk Window",
 "xtas.iconName : "XTAS_NAME,
 "xtas.*.main.*.fontList: -*-helvetica-bold-r-normal--14-*",
 "xtas.*fontList: -adobe-helvetica-bold-r-normal--*-120-*-*-*-*-iso8859-*",
 "xtas.*.XtasTextualDisplay.*.fontList: -*-fixed-medium-r-normal-*-13-*-*-*-*-*-iso8859-*",
 "xtas.*.main.*.XtasShellSubForm.*.fontList: -*-helvetica-bold-r-normal--12-*",
 "xtas.*.XalMessageScrolled.*.fontList: -*-fixed-medium-r-normal-*-14-*-*-*-*-*-iso8859-*",
      NULL };

#ifdef AVERTEC_LICENSE
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasAvtlicence                                                 */
/*                                                                           */
/* IN  ARGS : None                                                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Call to avt licence server                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasAvtlicence()
{
 if(avt_givetoken("HITAS_LICENSE_SERVER","xtas") != AVT_VALID_TOKEN)
  XtasExit(1) ;
 XtAppAddTimeOut( XtasApplicationContext,60000,XtasAvtlicence,NULL);
}
#endif

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPresent                                                    */
/*                                                                           */
/* IN  ARGS : None                                                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Call to alliancebanner                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPresent()
{

#ifdef AVERTEC_LICENSE
 if(avt_givetoken("HITAS_LICENSE_SERVER","xtas") != AVT_VALID_TOKEN)
  XtasExit(1) ;
#endif

avt_banner(XTAS_NAME,XTAS_ABOUT,"1998");
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasInitMainParam                                              */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Front Page push button.          */
/*            In this fuction, we build the main window widgets.             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
XtasMainInfoStruct *XtasInitMainParam()
{
 XtasMainInfoStruct  *param ;
 
 param = (XtasMainInfoStruct *)mbkalloc(sizeof(XtasMainInfoStruct)) ;
 param->ttvfig = NULL ;
 param->stbfig = NULL ;
 param->befig = NULL ;
 return(param) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFrontCallback                                              */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Front Page push button.          */
/*            In this fuction, we build the main window widgets.             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasInitializeWindows()
{
Widget XtasDeskMenuWidget, version_form;
Arg    args[20];
int    n;

XtasInitializeSignalsWindow();

XtasDeskMainForm = XtasCreateDesk( XtasDeskMainForm );
version_form = XtNameToWidget (XtasDeskMainForm, "*XtasVersionForm");
HelpFather = XtasDeskMainForm ;
XtasDeskMenuWidget = XtasCreateMainMenus( XtasMainWindow );
XtManageChild( XtasDeskMenuWidget );

/*---------------------------------*/
/* Use the Motif default areas set */
/*---------------------------------*/
XmMainWindowSetAreas( XtasMainWindow,
                      XtasDeskMenuWidget, 
                      (Widget) NULL,
                      (Widget) NULL,
                      (Widget) NULL,
                      XtasDeskMainForm );

/*------------------------------------------------------------------*/
/* Creates a Motif Error and Warning Boxs and AllianceFrontPage     */
/*------------------------------------------------------------------*/
XtasErrorWidget   = XalCreateErrorBox(   XtasDeskMainForm );
XtasWarningWidget = XalCreateWarningBox( XtasDeskMainForm );  
XtasNotYetWidget = XalCreateWarningBox( XtasDeskMainForm );  
XtasTraceWidget = XalCreateTraceBox( XtasDeskMainForm );  
XalFrontPage( version_form,      
            NULL,                  
            AVERTEC_VERSION,      
            PATCH_NUM,      
            "XTAS",             
            XTAS_ABOUT);             

n = 0; 
XtSetArg( args[n], XmNallowShellResize, False ); n++;
XtSetValues( XtasTopLevel, args, n );

XalForceUpdate( XtasTopLevel );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasEnv                                                        */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasEnv( void )
{
    char *c;
#ifdef AVERTEC
    avtenv() ;
#endif

    mbkenv() ; /* sinon => traitement normal */
    /* forcage de SCALE_X: la variable d'environnement ne sert plus */
    if(V_INT_TAB[__MBK_SCALE_X].VALUE == NULL)
            SCALE_X = 10000 ;
    else
     {
      if(SCALE_X < 1000)
         SCALE_X = 1000 ;
     }

    cnsenv() ;

    yagenv(tas_yaginit) ;
    fclenv() ;

    elpenv() ;
    mccenv() ;

    tlcenv() ;
    rcnenv();
    rcx_env();
    stb_env();
    stb_ctk_env() ;

    tas_setenv();
    tmaenv(0);

    libenv() ;
    tlfenv() ;
    cbhenv() ;
    ttvenv() ;

    Xyaggetenv();

    if ((c=V_STR_TAB[__MBK_SPI_SUFFIX].VALUE)!=NULL) spi_sfx=c;
    if ((c=V_STR_TAB[__MVL_FILE_SUFFIX].VALUE)!=NULL) vhd_sfx=c;
    if ((c=V_STR_TAB[__MGL_FILE_SUFFIX].VALUE)!=NULL) vlg_sfx=c;

        XTAS_DISPLAY_NEW = V_BOOL_TAB[__XTAS_TEXT_DISPLAY].VALUE;

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasAppInit                                                    */
/*                                                                           */
/* IN  ARGS : None                                                           */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Initialization of graphics variables.                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasAppInit( void )
{

char avertec_version[128];

/*------------------------------------*/
/*   Set MBK environnement variables  */
/*------------------------------------*/
XtasEnv() ;

XalBitmaps = Xtas_bitmaps ;

XtasPresent();

sprintf (avertec_version,"%g%s", AVT_VERSION, PATCH_NUM) ;

/*--------------------------------------------------*/
/* Prepare the message for the Help On Version box  */
/*--------------------------------------------------*/
XtasOnVersion = (char *)mbkalloc( strlen(XTAS_ABOUT) + strlen( avertec_version ) + 2 );
strcpy( XtasOnVersion, XTAS_ABOUT );
strcat( XtasOnVersion, "\n" );
strcat( XtasOnVersion, avertec_version );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasInitializeRessources                                       */
/*                                                                           */
/* IN  ARGS : .You can give some X resources in your argv. They will be      */
/*            passed to the X server in the XtVaAppInitialize call.          */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : initialise the main widget                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasInitializeRessources( argc, argv )
int  *argc;
char *argv[];
{
int n;
Arg args[10];
Colormap  XtasColorMap;
Display   *ADisplay;
Screen    *AScreen;

MBK_EXIT_KILL = 'Y';

XtasAppInit( );

XtasTopLevel = XtVaAppInitialize( &XtasApplicationContext, 
				  "Xtas", 
				  NULL, 
				  0, 
				  argc, 
				  argv, 
				  XtasFallBacks, 
				  NULL );

n = 0;
/*
XtSetArg( args[n], XmNx        , 100 ); n++;
XtSetArg( args[n], XmNy        , 100 ); n++;
XtSetArg( args[n], XmNwidth    , 660 ); n++;
XtSetArg( args[n], XmNheight   , 320 ); n++;
*/

ADisplay = XtDisplay(XtasTopLevel);
AScreen = XtScreen(XtasTopLevel);
XtasColorMap = DefaultColormapOfScreen(AScreen);

if (XTAS_SWITCH_COLOR_MAP == 'Y') {
    XtasColorMap = XCopyColormapAndFree(ADisplay, XtasColorMap);
    XInstallColormap(ADisplay, XtasColorMap);
    XtSetArg( args[n], XmNcolormap, XtasColorMap); n++;
}

XtSetValues( XtasTopLevel, args, n );

XalSetIcon( XtasTopLevel ); 

XtasMainWindow = XtVaCreateManagedWidget( "XtasMainWindow",
					xmMainWindowWidgetClass,
					XtasTopLevel, 
					NULL 
					);

XtasDeskMainForm = XtVaCreateManagedWidget( "XtasDeskMainForm", 
					xmFormWidgetClass, 
					XtasMainWindow, 
					XmNfractionBase,
					200,         
					NULL );

XtasMainParam = XtasInitMainParam() ;

XtasInitializeWindows();

XtasSetXyagleQuitFOnction() ;
XyagCreateMainWindow(XtasTopLevel,XtasApplicationContext,&XtasColorMap) ;

XtRealizeWidget( XtasTopLevel );

#ifdef AVERTEC_LICENSE
 XtasAvtlicence() ;
#endif

XtasInitFileErr() ;
}
