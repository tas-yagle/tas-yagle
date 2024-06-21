/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_stb.c                                                  */
/*                                                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"
#include "xtas_stb_menu.h"
#include "xtas_stb_button.h"
#include "xtas_stb_pbar.h"

#define XTAS_STB_LABEL_WIDTH1  236
#define XTAS_STB_LABEL_HEIGHT  30
#define XTAS_STB_LABEL_OFFSET  8
#define XTAS_STB_LABEL_WIDTH2  279 /* New display type */
#define XTAS_STB_MAXSIG_LENGTH 77

#define XTAS_STB_SIGNAME_LOFFSET    6

#define XTAS_STB_CACHE_UNIT_KB 'K'
#define XTAS_STB_CACHE_UNIT_MB 'M'
#define XTAS_STB_CACHE_UNIT_GB 'G'

Widget      XTAS_STB_LISTWIDGET = NULL;
Widget      STBMARGIN ;
Widget      STBRCSIG ;
Widget      STBRELCTKSIG ;
Widget      STBRELCTKFILTER ;
Widget      CTK_MARGIN ;
Widget      STB_MIN_SLOPE_CHANGE ;
Widget      STB_MAX_IT_NB ;
Widget      STBUSECACHE ;
Widget      STBCACHESIZE ;
Widget      NBERROR ;
//char        STBTYPE ;
//char        STBMONOPHASE ;
//char        STBLEVEL ;
//char        STBCONT ;
//char        STBFILE ;
//char        STBCTK ;
long        MaxSig ;
static      Widget XtasStbAnalysisWidget = NULL ;
chain_list *XTAS_STB_SESSION             = NULL ;
char       *XtasStbSigName               = NULL ;
static char *tas_current_signal          = NULL ;
static int  XtasStbLabelWidth ;
static chain_list *XtasStbCurrentError   = NULL ;
static stbfig_list *XtasStbCurrentFig    = NULL ;



char *CACHE_UNIT[] = { "Kb", "Mb", "Gb"};
char  XTAS_STB_CTK_CACHE_UNIT;
char  XTAS_STB_PARSE_STO = 'N';

Widget      FILE_TTX_TOGGLE, FILE_DTX_TOGGLE, CTK_TOGGLE, STO_TOGGLE;
Widget      CTK_TYPE_ROW_WIDGET, CTK_SAV_ROW_WIDGET, CTK_RES_ROW_WIDGET, CTK_CAP_ROW_WIDGET, CTK_CACHE_ROW_WIDGET  ;
Widget      CTK_TYPE_FRAME_WIDGET, CTK_SAV_FRAME_WIDGET, CTK_RES_FRAME_WIDGET, CTK_CAP_FRAME_WIDGET, CTK_CACHE_FRAME_WIDGET;
Widget      CTK_PARAM_LABEL_WIDGET;
Widget      ANALYSIS_FORM, INFO_FORM,CTK_FORM, USECACHE_FORM, CACHE_UNIT_LIST;
Widget      CTK_TOGGLE_OBS;
Widget      REPORT_CTK_FORM, REPORT_DELAY_MIN, REPORT_SLOPE_MIN, REPORT_NOISE_MIN, REPORT_CTK_MIN;


extern  long            STB_CTK_MINSLOPECHANGE;
extern  long            STB_CTK_MARGIN;
extern  int             STB_CTK_MAXLASTITER;

extern  char        RCX_0C2C;

extern  long  CTK_REPORT_DELTA_DELAY_MIN; 
extern  long  CTK_REPORT_DELTA_SLOPE_MIN; 
extern  float CTK_REPORT_CTK_MIN        ;
extern  long  CTK_REPORT_NOISE_MIN      ; 

//extern void rcx_env_error( char *msg );

xtas_stb_param_struct *XtasStbParam = NULL;

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasSaveStbResults (file)
FILE    *file;
{
    int         nlist, nboxes, i, j, k;
    WidgetList  list_children, textboxes;
    char        *signame, *type, *setup, *hold;
    char         nbspace[1024] ; /* between signame & sigtype */
    XmStringTable   items;
    char        *buf, *p, *p1, *p2;
    
    fprintf (file, "\n\n");
    fprintf (file, "%5s %11s %11s %11s    %s\n","Rank","Setup","Hold","Type","Signal Name");
    fprintf (file,"#---- ----------- ----------- -----------    ------------------------------\n");
    
    if (XTAS_DISPLAY_NEW) {
        XtVaGetValues (XTAS_STB_LISTWIDGET, XmNitemCount, &nlist, XmNitems, &items, NULL);        
        for (i=0 ; i<nlist ; i++) {
            XmStringGetLtoR (items[i], XmSTRING_DEFAULT_CHARSET, &buf);
            /* recherche du signame */
            p = strchr (buf, ' '); /* l'espace marque la fin du signame */
            signame  = (char *)mbkalloc (p-buf+1);
            j = 0;
            while ((buf+j) != p) {
                *(signame+j) = *(buf+j);
                j++;
            }
            *(signame+j) = '\0'; 
            p++;
            /* recherche du type */
            p1 = strchr (p, '[');
            p2 = strchr (p1, ']');
            type = (char *)mbkalloc (p2-p1+2); /* +2 car on garde les crochets */
            for (p=p1, j=0 ; p<=p2 ; p++, j++)
                *(type+j) = *p;
            *(type+j) = '\0';
            /* recherche du hold */
            p = buf+strlen(buf);
            if (*(p-1) != 'S') { /* il n'y a pas de hold */
                hold = (char *)mbkalloc (2);
                strcpy (hold, " ");
            }
            else { 
                p2 = strrchr (buf, ' ');
                for (p1=p2-1 ; *(p1-1) != ' ' ; p1--);
                j = 0;
                hold = (char *)mbkalloc (p2-p1+1);
                while (*(p1+j)!='\0') {
                    *(hold+j) = *(p1+j);
                    j++;
                }
                *(hold+j) = '\0';
            }
            /* recherche du setup */
            for (p=p1-1 ; *p==' ' ; p--);
            if (*p != 'S') { /* on est tombe sur la fin du type -> il n'y a pas de setup*/
                setup = (char *)mbkalloc (2);
                strcpy (setup, " ");
            }
            else {
                p2 = p-2;
                for (p1=p2-1 ; *(p1-1) != ' ' ; p1--);
                j = 0;
                setup = (char *)mbkalloc (p-p1+2);
                while ((p1+j)!=(p+1)) {
                    *(setup+j) = *(p1+j);
                    j++;
                }
                *(setup+j) = '\0';
            }
            fprintf (file, "%5d %11s %11s %11s    %s\n", i+1, setup, hold, type, signame);
            free (buf);
            mbkfree (signame);
            mbkfree (type);
            mbkfree (setup);
            mbkfree (hold);
        }
    }
    else { 
        XtVaGetValues (XTAS_STB_LISTWIDGET, XmNnumChildren, &nlist, XmNchildren, &list_children, NULL);
        for (i=0 ; i<nlist ; i++) {
            XtVaGetValues (list_children[i], XmNnumChildren, &nboxes, XmNchildren, &textboxes, NULL); 
            signame = XmTextGetString (textboxes[0]);    
            type = XmTextGetString (textboxes[1]);    
            setup = XmTextGetString (textboxes[2]);    
            hold = XmTextGetString (textboxes[3]);    
            fprintf (file, "%5d %11s %11s %11s    %s\n", i+1, setup, hold, type, signame);
            free (signame);
            free (type);
            free (setup);
            free (hold);
        }
    }
    fprintf (file,"#---- ----------- ----------- -----------    ------------------------------\n");
    fprintf (file, "%5s %11s %11s %11s    %s\n","Rank","Setup","Hold","Type","Signal Name");
    fprintf (file, "\n\n");
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasStbSaveCallback (widget, client_data, call_data)
Widget      widget;
XtPointer   client_data;
XtPointer   call_data;
{

    if (XTAS_STB_LISTWIDGET == NULL) {/* no error */
        XalDrawMessage(((XtasWindowStruct*)client_data)->warwidget, XTAS_NOSTBERR);
    }

    else {
        XtasSelectFileCallback( widget, (XtPointer)XTAS_STBA_MAP, call_data ) ;
    }
}

void    XtasStbSaveSelectFileOk (void)
{
    FILE *file ;
    
    if(XtasDataFileName != NULL) {
        file = fopen(XtasDataFileName,"a") ;
        if(file != NULL){
            XtasSaveStbResults (file) ;
            fclose(file) ;
        }
        else {
            XalDrawMessage( XtasErrorWidget, XTAS_NOPENFL);
        }
    }
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasRemoveStbSession(window)
XtasWindowStruct *window  ;
{
 chain_list *head = XTAS_STB_SESSION ;
 XtasWindowStruct *data ;

 XtasDestroyDebugWidget() ;
 if(XtasNoiseParamWidget) {
    XtDestroyWidget (XtasNoiseParamWidget);
    XtasNoiseParamWidget = NULL;
 }
 
 if(head != NULL)
   {
    for( ; head != NULL ; head = head->NEXT )
      {
       data = head->DATA ;
       if((window == NULL) || (window == data))
         {
          XtDestroyWidget( data->topwidget ) ;
          mbkfree(data) ;
          if(window == data)
           break  ;
         }
      }
   }
 if(window == NULL)
   {
    freechain( XTAS_STB_SESSION ) ;
    XTAS_STB_SESSION = NULL ;
   }
 else if(head != NULL)
      XTAS_STB_SESSION = delchain(XTAS_STB_SESSION,head) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDispInfoCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .top : The window toplevel widget id.                          */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Get current SigName in the scroll list (only for new display)  */
/*            And display its information in a widget at bottom of window    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDispInfoCallback( widget, client_data , call_data )
Widget widget ;
XtPointer client_data  ;
XtPointer call_data ;
{
    XmListCallbackStruct    *cbs ;
    chain_list              *chain ;
    ttvsig_list             *ptsig  ;
    int                      i ;
    char                     buf[1024] ;
    Arg                      args[20] ;
    int                      n ;
    static Widget            info_form ;
    static chain_list*       last_error ;

    if(!XtasStbCurrentError)
        return ;

    cbs = (XmListCallbackStruct*)call_data ;
    i    = 1 ;
    chain = XtasStbCurrentError ;

    while (chain && (cbs) && (i < cbs->item_position)) {
        chain = chain->NEXT ;
        i++ ;
    }
    if(tas_current_signal != NULL) {
       mbkfree( tas_current_signal) ;
       tas_current_signal = NULL ;
    }

        
    ptsig = (ttvsig_list *)chain->DATA ;
    if(ptsig && (XtasStbCurrentFig)) {
      ttv_getsigname(XtasStbCurrentFig->FIG, buf, ptsig) ;
      tas_current_signal = strdup ( buf ) ;
      XtasStbSigName = tas_current_signal ;

      if (info_form && (last_error == XtasStbCurrentError))
          XtDestroyWidget ( info_form ) ;
      n = 0  ;
      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM ) ; n++ ;
      XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM ) ; n++ ;
      XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM ) ; n++ ;
      XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM ) ; n++ ;
      info_form = XtCreateManagedWidget( "XtasSubForm",
                                         xmFormWidgetClass,
                                         (Widget)client_data, 
                                         args, n ) ;
      XtasFillInfosForm( info_form, tas_current_signal ) ;
      last_error = XtasStbCurrentError ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbCloseCallback                                           */
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
void XtasStbCloseCallback( widget, client_data , call_data )
Widget widget ;
XtPointer client_data  ;
XtPointer call_data ;
{
 XtasWindowStruct *top = (XtasWindowStruct *)client_data  ;

 XtasRemoveStbSession(top) ;
 if (XtasNoiseParamWidget) {
    XtDestroyWidget (XtasNoiseParamWidget);
    XtasNoiseParamWidget = NULL;
 }

 if (XtasNoiseResultsWidget) {
    XtDestroyWidget (XtasNoiseResultsWidget);
    XtasNoiseResultsWidget = NULL;
 }

 /* supprime pour avoir acces aux info sur les crosstalk 
  * dans la fenetre Detailed Path sans avoir a garder 
  * la fenetere de Stability Resulte ouverte */
// if(XtasMainParam->stbfig != NULL) {
//    stb_ctk_clean(XtasMainParam->stbfig); 
// 	stb_delstbfig(XtasMainParam->stbfig) ;
// 	XtasMainParam->stbfig = NULL  ;
// }
 TTV_MAX_SIG = MaxSig  ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbButtonsCallback                                         */
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
void XtasStbButtonsCallback( widget, client_data, call_data )
Widget           widget ;
XtPointer client_data ;
XtPointer call_data ;
{
 XtasWindowStruct *widtab = (XtasWindowStruct *)client_data  ;
 XtasOptionsButtonsTreat( widtab->wrkwidget, &widtab->butwidget,
                         XtasStbButtonMenus,  XtasStbButtonOptions ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbGetPathCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStbGetPathCallback( widget, client_data, call_data )
Widget widget ;
XtPointer client_data ;
XtPointer call_data ;
{
    short int mask=0x0, i;
    ttvsig_list *ptsig;
    stbnode *ptnode;
    ptype_list *ptype;
    long hold;
    
    
    if(tas_current_signal != NULL)
    {
        XtasSetOrderByStartOrEnd( 10, 1, "*" ) ;
        XtasSetOrderByStartOrEnd( 20, 1, tas_current_signal ) ;

        ptsig = ttv_getsig(XtasMainParam->ttvfig, tas_current_signal);
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

        if(hold < XtasMargin)
            mask |= XTAS_HOLD; 
    }
    XtasMainToolsCallback( widget, (XtPointer)(&mask), call_data ) ;
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbGetPathCmdCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStbGetPathCmdCallback( widget, client_data, call_data )
Widget widget ;
XtPointer client_data ;
XtPointer call_data ;
{
    short int mask = XTAS_REG, i;
    ttvsig_list *ptsig;
    chain_list  *ch, *chain;
    char buf[256], cmd_list[1024];
    stbnode *ptnode;
    ptype_list *ptype;
    long hold;
    
    if(tas_current_signal != NULL)
    {
        ptsig = ttv_getsig(XtasMainParam->ttvfig, tas_current_signal);
        if(ptsig) {
            if((ptsig->TYPE & TTV_SIG_L) != TTV_SIG_L)
                XalDrawMessage(XtasErrorWidget, "No command found: \n the selected signal is not a register signal");
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
        XalDrawMessage(XtasErrorWidget, XTAS_NSIGERR);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbDetailCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .widtab : Table of important widgets for the current window.   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the View command                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStbDetailCallback( widget, client_data, call_data )
Widget widget ;
XtPointer client_data ;
XtPointer call_data ;
{
 XtasWindowStruct *widtab = (XtasWindowStruct *)client_data  ;

 if( tas_current_signal != NULL )
   {
    XalSetCursor( widtab->topwidget, WAIT ) ;
    XalForceUpdate(  widtab->topwidget ) ;
    XtasSignalsDetail( widtab->wrkwidget, tas_current_signal ) ;
    XalSetCursor( widtab->topwidget, NORMAL ) ;
   }
 else
   {
    XalDrawMessage( widtab->errwidget, XTAS_NSIGERR ) ;
   }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateStbMenus                                             */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the detail  window.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateStbMenus( parent , tas_winfos )
Widget parent ;
XtasWindowStruct *tas_winfos ;
{
Widget menu_bar ;
Arg    args[20] ;
int    n ;

for(n = 0  ; XtasStbMenuFile[n].label != NULL  ; n++)
  XtasStbMenuFile[n].callback_data    = (XtPointer) tas_winfos ;
for(n = 0  ; XtasStbMenuView[n].label != NULL  ; n++)
  XtasStbMenuView[n].callback_data    = (XtPointer) tas_winfos ;
for(n = 0  ; XtasStbMenuTools[n].label != NULL  ; n++)
  XtasStbMenuTools[n].callback_data    = (XtPointer) tas_winfos ;
for(n = 0  ; XtasStbMenuOptions[n].label != NULL  ; n++)
  XtasStbMenuOptions[n].callback_data = (XtPointer) tas_winfos ;

n = 0  ;
menu_bar = XmCreateMenuBar( parent, "XtasMenuBar", args, n ) ;
XalBuildMenus( menu_bar, XtasStbMenu ) ;
return( menu_bar ) ;
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbColumnsTitleMoveWithHSBCallback                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasStbColumnsTitleMoveWithHSBCallback (hsb, label, call_data)
    Widget      hsb;
    XtPointer   label;
    XtPointer   call_data;
{
    int         loffset = XTAS_STB_SIGNAME_LOFFSET;
    int         value;

    XtVaGetValues (hsb, XmNvalue, &value, NULL);

    XtVaSetValues (label, XmNleftOffset, (loffset - value), NULL);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbFocusCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .signal : The signal name, NULL if not initialized.            */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStbFocusCallback( widget, client_data, call_data )
Widget  widget ;
XtPointer client_data ;
XtPointer call_data ;
{
 if(tas_current_signal != NULL)
   XtFree( tas_current_signal) ;
 tas_current_signal = XmTextGetString( widget ) ;
 XtVaSetValues (widget, XmNcursorPositionVisible, True, NULL);
 XtasStbSigName = tas_current_signal  ; //pour interfacage avec debug
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasLosingStbFocusCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .signal : The signal name, NULL if not initialized.            */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the focus in and out.                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasLosingStbFocusCallback( widget, signal, call_data )
Widget  widget ;
XtPointer signal ;
XtPointer call_data ;
{
 XtVaSetValues( widget, XmNcursorPositionVisible, False ,NULL ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateStbScrollList2                                       */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite (new Display )   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCreateStbScrollList2(scform, stbfig,margin,nberror,errorw,setupw,holdw, SIGNAME_label_widget)
Widget scform  ;
stbfig_list *stbfig  ;
long margin  ;
int nberror  ;
Widget errorw  ;
Widget setupw  ;
Widget holdw  ;
Widget SIGNAME_label_widget;
{
 Widget       scroll  ;
 Widget       list_form ;
 Widget       info_form ;
 Arg          args[20] ;
 int          n ;
 ttvsig_list *ptsig  ;
 ptype_list   *ptype ;
 stbnode     *ptnode  ;
 chain_list  *chainerror  ;
 chain_list  *chain  ;
 XmString     motif_string ;
 char         buffer[2048] ;
 long         minsetup  ;
 long         minhold  ;
 long         setup  ;
 long         hold  ;
 int          errnumb ;
 int          position = 0  ;
 int          i  ;
 char         nbspace[1024] ; /* between signame & sigtype */
 char         sigtype[16] ;
 char         signame[1024] ;
 char        *setuptime ;
 char        *holdtime ;
 int          scroll_index_pos ;
 Widget       scrollW, tmp_widget;

 chainerror = stb_geterrorlist(stbfig,margin,nberror,&minsetup,&minhold,&errnumb) ;
 XtasStbCurrentError = chainerror ;
 XtasStbCurrentFig = stbfig ;

 sprintf(buffer,"Error Number: %d",errnumb) ;
 XtasSetLabelString( errorw, buffer) ;

 if(minsetup != STB_NO_TIME)
   sprintf(buffer,"Min Setup: %.1f",minsetup/TTV_UNIT) ;
 else
   sprintf(buffer,"Min Setup: ") ;

 XtasSetLabelString( setupw, buffer) ;

 if(minhold != STB_NO_TIME)
   sprintf(buffer,"Min Hold: %.1f",minhold/TTV_UNIT) ;
 else
   sprintf(buffer,"Min Hold: ") ;

 XtasSetLabelString( holdw, buffer) ;

 n = 0  ;
 list_form = XtCreateManagedWidget( "XtasSubForm",
                                    xmFormWidgetClass,
                                    scform,
                                    args, n ) ;
 n = 0  ;
 XtSetArg( args[n], XmNheight, 130) ; n++ ;
 info_form = XtCreateManagedWidget( "XtasSubForm",
                                    xmFormWidgetClass,
                                    scform,
                                    args, n ) ;

 if(chainerror == NULL)
  {
   XtasStbCurrentError = chainerror ;
   n=0 ;
   XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNvisibleItemCount,       15                   ) ; n++ ;
   scroll = XmCreateScrolledList( list_form, "XtasUserField", args , n ) ;
   XtManageChild( scroll) ;

   sprintf(buffer,"no error") ;
   motif_string = XmStringCreateLtoR(buffer, XmSTRING_DEFAULT_CHARSET ) ;

   XmListAddItemUnselected( scroll, motif_string, ++position ) ;
   XmStringFree( motif_string ) ;
   XTAS_STB_LISTWIDGET = NULL;
   return  ;
  }
   
 n=0 ;
 XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNscrollingPolicy,        XmSTATIC             ) ; n++ ;
 XtSetArg( args[n], XmNheight,                 250                  ) ; n++ ;
 scroll = XmCreateScrolledList( list_form, "XtasTextualDisplay", args , n ) ;
 XmListDeleteAllItems( scroll ) ;
 XtManageChild( scroll) ;

 scroll_index_pos = 1 ;
 for(chain = chainerror  ; chain != NULL  ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;

   if((ptype = getptype(ptsig->USER,STB_SETUP)) != NULL)
     setup = (long)ptype->DATA ;

   if((ptype = getptype(ptsig->USER,STB_HOLD)) != NULL)
     hold = (long)ptype->DATA ;

   if(ptype == NULL)
    {
     setup = STB_NO_TIME ;
     hold  = STB_NO_TIME;
     for(i = 0  ; i < 2  ; i++)
      {
       ptnode = stb_getstbnode(ptsig->NODE+i) ;
       if(ptnode != NULL)
        {
         if(ptnode->SETUP < setup)
           setup = ptnode->SETUP  ;
         if(ptnode->HOLD < hold)
           hold = ptnode->HOLD  ;
        }
      }
    }

   ttv_getsigname(stbfig->FIG, signame, ptsig) ;

   if((ptsig->TYPE & TTV_SIG_CQ) == TTV_SIG_CQ)
    sprintf(sigtype,"[CON CMD]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CT) == TTV_SIG_CT)
    sprintf(sigtype,"[CON TST]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ)
    sprintf(sigtype,"[CON  HZ]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CB) == TTV_SIG_CB)
    sprintf(sigtype,"[CON I/O]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CO) == TTV_SIG_CO)
    sprintf(sigtype,"[CON OUT]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CI) == TTV_SIG_CI)
    sprintf(sigtype,"[CON  IN]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CX) == TTV_SIG_CX)
    sprintf(sigtype,"[CON  ??]") ;
   else
   if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
    sprintf(sigtype,"[CON  ??]") ;
   else
   if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
    sprintf( sigtype, "[CMD]") ;
   else
   if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
    sprintf(sigtype,"[LATCH]") ;
   else
   if((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
    sprintf(sigtype,"[FLIP/FLOP]") ;
   else
   if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
    sprintf(sigtype,"[LATCH]") ;
   else
   if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
    sprintf(sigtype,"[PRE]") ;
   else
   if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
    sprintf(sigtype,"[BREAK]") ;
   else
    sprintf(sigtype,"[INT]") ;
   
    if(setup != STB_NO_TIME)
      setuptime = XtasPlaceNbStringTTVUnit( setup, TTV_UNIT ) ;
    else
      setuptime = XtasPlaceNbString( 0 ) ;
    
    if(hold != STB_NO_TIME)
      holdtime = XtasPlaceNbStringTTVUnit( hold, TTV_UNIT ) ;
    else
      holdtime = XtasPlaceNbString( 0 ) ;

   for ( i = 0 ; i < (XTAS_STB_MAXSIG_LENGTH - (int)strlen(signame)); i++ ) {
       nbspace[i] = ' ' ;
   }
   nbspace[i] = '\0' ;

   sprintf(buffer, "%s %s %11s    %s pS    %s pS", 
           signame,nbspace,sigtype,setuptime,holdtime) ;
   mbkfree(setuptime) ;
   mbkfree(holdtime) ;
   motif_string = XmStringCreateSimple (buffer) ;
   XmListAddItemUnselected (scroll, motif_string, scroll_index_pos) ;
   XmStringFree ( motif_string ) ;
   scroll_index_pos++  ;
  }

 XTAS_STB_LISTWIDGET = scroll;
 stb_delerrorlist(chainerror) ;

 /* Call back to get info */
 XtAddCallback (scroll,
                XmNbrowseSelectionCallback,
                XtasDispInfoCallback,
               (XtPointer)info_form);

 scrollW = XtParent (scroll);
 XtVaGetValues (scrollW, XmNhorizontalScrollBar, &tmp_widget, NULL);
 XtAddCallback (tmp_widget, XmNvalueChangedCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNdragCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNincrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNdecrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNpageIncrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNpageDecrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNtoTopCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNtoBottomCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateStbScrollList                                        */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCreateStbScrollList(scform, stbfig,margin,nberror,errorw,setupw,holdw, SIGNAME_label_widget)
Widget scform  ;
Widget SIGNAME_label_widget ;
stbfig_list *stbfig  ;
long margin  ;
int nberror  ;
Widget errorw  ;
Widget setupw  ;
Widget holdw  ;
{
 Widget       row_widget  ;
 Widget       scroll  ;
 Widget       sub_form  ;
 Widget       tmp_widget  ;
 Arg          args[20] ;
 int          n ;
 ttvsig_list *ptsig  ;
 ptype_list   *ptype ;
 stbnode     *ptnode  ;
 chain_list  *chainerror  ;
 chain_list  *chain  ;
 XmString     motif_string ;
 char         buffer[2048] ;
 long         minsetup  ;
 long         minhold  ;
 long         setup  ;
 long         hold  ;
 int          errnumb ;
 int          position = 0  ;
 int          i  ;

 chainerror = stb_geterrorlist(stbfig,margin,nberror,&minsetup,&minhold,&errnumb) ;

 sprintf(buffer,"Error Number: %d",errnumb) ;
 XtasSetLabelString( errorw, buffer) ;

 if(minsetup != STB_NO_TIME)
   sprintf(buffer,"Min Setup: %.1f",minsetup/TTV_UNIT) ;
 else
   sprintf(buffer,"Min Setup: ") ;

 XtasSetLabelString( setupw, buffer) ;

 if(minhold != STB_NO_TIME)
   sprintf(buffer,"Min Hold: %.1f",minhold/TTV_UNIT) ;
 else
   sprintf(buffer,"Min Hold: ") ;

 XtasSetLabelString( holdw, buffer) ;

 if(chainerror == NULL)
  {
   n=0 ;
   XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
   XtSetArg( args[n], XmNvisibleItemCount,       15                   ) ; n++ ;
   scroll = XmCreateScrolledList( scform, "XtasUserField", args , n ) ;
   XtManageChild( scroll) ;

   sprintf(buffer,"no error") ;
   motif_string = XmStringCreateLtoR(buffer, XmSTRING_DEFAULT_CHARSET ) ;

   XmListAddItemUnselected( scroll, motif_string, ++position ) ;
   XmStringFree( motif_string ) ;
   XTAS_STB_LISTWIDGET = NULL;
   return  ;
  }
   
 n=0 ;
 XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
 XtSetArg( args[n], XmNscrollBarPlacement,     XmBOTTOM_RIGHT  ); n++;
 XtSetArg( args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED     ); n++;
 XtSetArg( args[n], XmNscrollingPolicy,        XmAUTOMATIC     ); n++;
 XtSetArg( args[n], XmNheight,                 250                  ) ; n++ ;
 scroll = XmCreateScrolledWindow( scform, "XtasScrollWindow", args , n ) ;
 XtVaGetValues (scroll, XmNhorizontalScrollBar, &tmp_widget, NULL);
 XtAddCallback (tmp_widget, XmNvalueChangedCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNdragCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNincrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNdecrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNpageIncrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNpageDecrementCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNtoTopCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtAddCallback (tmp_widget, XmNtoBottomCallback, XtasStbColumnsTitleMoveWithHSBCallback, SIGNAME_label_widget);
 XtManageChild( scroll) ;

 row_widget = XtVaCreateWidget( "XtasStbRow",
                                xmRowColumnWidgetClass,
                                scroll,
                                XmNorientation, XmVERTICAL,
                                NULL) ;

 for(chain = chainerror  ; chain != NULL  ; chain = chain->NEXT)
  {
   ptsig = (ttvsig_list *)chain->DATA ;

   if((ptype = getptype(ptsig->USER,STB_SETUP)) != NULL)
     setup = (long)ptype->DATA ;

   if((ptype = getptype(ptsig->USER,STB_HOLD)) != NULL)
     hold = (long)ptype->DATA ;

   if(ptype == NULL)
    {
     setup = STB_NO_TIME ;
     hold  = STB_NO_TIME;
     for(i = 0  ; i < 2  ; i++)
      {
       ptnode = stb_getstbnode(ptsig->NODE+i) ;
       if(ptnode != NULL)
        {
         if(ptnode->SETUP < setup)
           setup = ptnode->SETUP  ;
         if(ptnode->HOLD < hold)
           hold = ptnode->HOLD  ;
        }
      }
    }

   n = 0  ;
   sub_form = XtCreateManagedWidget( "XtasSubForm",
                                      xmFormWidgetClass,
                                      row_widget,
                                      args, n ) ;
   n = 0 ;
   XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM ) ; n++ ;
   XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM ) ; n++ ;
   XtSetArg( args[n], XmNrightOffset,           2               ) ; n++ ;
   XtSetArg( args[n], XmNwidth,                 400             ) ; n++ ;
   XtSetArg( args[n], XmNrecomputeSize,         False           ) ; n++ ;
   XtSetArg( args[n], XmNshadowThickness,       1               ) ; n++ ;
   XtSetArg( args[n], XmNcursorPositionVisible, False           ) ; n++ ;
   XtSetArg( args[n], XmNeditable,              False           ) ; n++ ;
   tmp_widget = XtCreateManagedWidget( "XtasUserField",
                                        xmTextWidgetClass,
                                        sub_form,
                                        args, n) ;

   XtAddCallback( tmp_widget,
                  XmNfocusCallback,
                  XtasStbFocusCallback,
                  tmp_widget ) ;
   XtAddCallback( tmp_widget,
                  XmNlosingFocusCallback,
                  XtasLosingStbFocusCallback,
                  tmp_widget ) ;

   ttv_getsigname(stbfig->FIG, buffer, ptsig) ;
   XmTextSetString( tmp_widget, buffer ) ;

   if((ptsig->TYPE & TTV_SIG_CQ) == TTV_SIG_CQ)
    sprintf(buffer,"[CON CMD]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CT) == TTV_SIG_CT)
    sprintf(buffer,"[CON TST]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CZ) == TTV_SIG_CZ)
    sprintf(buffer,"[CON  HZ]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CB) == TTV_SIG_CB)
    sprintf(buffer,"[CON I/O]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CO) == TTV_SIG_CO)
    sprintf(buffer,"[CON OUT]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CI) == TTV_SIG_CI)
    sprintf(buffer,"[CON  IN]") ;
   else
   if((ptsig->TYPE & TTV_SIG_CX) == TTV_SIG_CX)
    sprintf(buffer,"[CON  ??]") ;
   else
   if((ptsig->TYPE & TTV_SIG_C) == TTV_SIG_C)
    sprintf(buffer,"[CON  ??]") ;
   else
   if((ptsig->TYPE & TTV_SIG_Q) == TTV_SIG_Q)
    sprintf( buffer, "[CMD]") ;
   else
   if((ptsig->TYPE & TTV_SIG_LL) == TTV_SIG_LL)
    sprintf(buffer,"[LATCH]") ;
   else
   if((ptsig->TYPE & TTV_SIG_LF) == TTV_SIG_LF)
    sprintf(buffer,"[FLIP/FLOP]") ;
   else
   if((ptsig->TYPE & TTV_SIG_L) == TTV_SIG_L)
    sprintf(buffer,"[LATCH]") ;
   else
   if((ptsig->TYPE & TTV_SIG_R) == TTV_SIG_R)
    sprintf(buffer,"[PRE]") ;
   else
   if((ptsig->TYPE & TTV_SIG_B) == TTV_SIG_B)
    sprintf(buffer,"[BREAK]") ;
   else
    sprintf(buffer,"[INT]") ;
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,            tmp_widget      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,           2               ) ; n++ ;
    XtSetArg( args[n], XmNwidth,                 100             ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,         False           ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,       1               ) ; n++ ;
    XtSetArg( args[n], XmNcursorPositionVisible, False           ) ; n++ ;
    XtSetArg( args[n], XmNeditable,              False           ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasUserField",
                                         xmTextWidgetClass,
                                         sub_form,
                                         args, n ) ;
    XmTextSetString( tmp_widget,buffer) ;
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,            tmp_widget      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,            2               ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,           2               ) ; n++ ;
    XtSetArg( args[n], XmNwidth,                 100             ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,         False           ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,       1               ) ; n++ ;
    XtSetArg( args[n], XmNcursorPositionVisible, False           ) ; n++ ;
    XtSetArg( args[n], XmNeditable,              False           ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasUserField",
                                         xmTextWidgetClass,
                                         sub_form, args, n) ;
    if(setup != STB_NO_TIME)
      sprintf(buffer,"%.1f pS" , setup/TTV_UNIT) ;
    else
      sprintf(buffer," ") ;
    XmTextSetString( tmp_widget, buffer ) ;
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,            tmp_widget      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,            2               ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,           2               ) ; n++ ;
    XtSetArg( args[n], XmNwidth,                 100             ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,         False           ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,       1               ) ; n++ ;
    XtSetArg( args[n], XmNcursorPositionVisible, False           ) ; n++ ;
    XtSetArg( args[n], XmNeditable,              False           ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasUserField",
                                         xmTextWidgetClass,
                                         sub_form,
                                         args, n) ;
    if(hold != STB_NO_TIME)
      sprintf(buffer,"%.1f pS" , hold/TTV_UNIT) ;
    else
      sprintf(buffer," ") ;
    XmTextSetString( tmp_widget, buffer ) ;
    
  }

 stb_delerrorlist(chainerror) ;
 XTAS_STB_LISTWIDGET = row_widget;
 XtManageChild( row_widget ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateStbList                                              */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation de la liste des erreurs de stabilite                  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateStbList( parent, tas_winfos, stbfig,margin,nberror)
Widget             parent ;
XtasWindowStruct  *tas_winfos ;
stbfig_list *stbfig  ;
long margin  ;
int nberror  ;
{
 Widget             form_widget, form, dbase_form, pan_widget ;
 Widget             label_widget  ;
 Widget             frame  ;
 Widget             errframe  ;
 Widget             nberrorwidget  ;
 Widget             setupwidget  ;
 Widget             holdwidget  ;
 Widget             buttons_form  ;
 Widget             separator1, separator2 ;
 Widget             SIGNAME_label_widget ;
 XmString           text ;
 Arg                args[20] ;
 char               buffer[1024]  ;
 int                n ;
 int                label_signame_width ;

 if(XTAS_DISPLAY_NEW) {
        XtasStbLabelWidth = XTAS_STB_LABEL_WIDTH2 ;
         
     label_signame_width = 540 ;
 }
 
 else {
        XtasStbLabelWidth = XTAS_STB_LABEL_WIDTH1 ;
     label_signame_width = 400 ;
 }
     

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
                                 NULL) ;
 HelpFather = form_widget ;

n = 0 ;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ) ; n++ ;
buttons_form = XtCreateManagedWidget( "XtasStbResultsButtons", xmFormWidgetClass, form_widget, args, n ) ;
for ( n=0 ; XtasStbButtonMenus[n].pix_file != -1  ; n++ )
  XtasStbButtonMenus[n].callback_data = (XtPointer) tas_winfos ;
 n = 0 ;
XtSetArg( args[n], XmNnumColumns,      1             ) ; n++ ;
XtSetArg( args[n], XmNorientation,     XmHORIZONTAL  ) ; n++ ;
tas_winfos->butwidget = XalButtonMenus( buttons_form, XtasStbButtonMenus, args,
                                        n, 40, 40 ) ;
XtasAddDummyButton( tas_winfos->butwidget ) ;
XtManageChild( tas_winfos->butwidget ) ;

/*---------------------------------------------------------------------------*/
/* creation d'un separateur                                                  */
/*---------------------------------------------------------------------------*/
n = 0 ;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM             ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM             ) ; n++ ;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET           ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,       buttons_form              ) ; n++ ;
separator1 = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, form_widget, args, n ) ;

n = 0 ;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM             ) ; n++ ;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET           ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,       separator1                ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM             ) ; n++ ;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form_widget, args, n ) ;

/* toto label */
n = 0 ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM            ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM            ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       XTAS_STB_LABEL_OFFSET    ) ; n++ ;
XtSetArg( args[n], XmNheight,           XTAS_STB_LABEL_HEIGHT    ) ; n++ ;
XtSetArg( args[n], XmNwidth,            XtasStbLabelWidth        ) ; n++ ;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
sprintf(buffer," ") ;
XtasSetLabelString( label_widget, buffer) ;

/* required setup label */
n = 0 ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM            ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ) ; n++ ;
XtSetArg( args[n], XmNleftWidget,       frame                    ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       XTAS_STB_LABEL_OFFSET    ) ; n++ ;
XtSetArg( args[n], XmNheight,           XTAS_STB_LABEL_HEIGHT    ) ; n++ ;
XtSetArg( args[n], XmNwidth,            XtasStbLabelWidth        ) ; n++ ;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
sprintf(buffer,"Required Setup: %.0f",stbfig->SETUP/TTV_UNIT) ;
XtasSetLabelString( label_widget, buffer) ;

/* required hold label */
n = 0 ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM            ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ) ; n++ ;
XtSetArg( args[n], XmNleftWidget,       frame                    ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       XTAS_STB_LABEL_OFFSET    ) ; n++ ;
XtSetArg( args[n], XmNheight,           XTAS_STB_LABEL_HEIGHT    ) ; n++ ;
XtSetArg( args[n], XmNwidth,            XtasStbLabelWidth        ) ; n++ ;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
sprintf(buffer,"Required Hold: %.0f",stbfig->HOLD/TTV_UNIT) ;
XtasSetLabelString( label_widget, buffer) ;

/* nb error label */
n = 0 ;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM             ) ; n++ ;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET           ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,       frame                     ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
errframe = frame  ;
XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       XTAS_STB_LABEL_OFFSET    ) ; n++ ;
XtSetArg( args[n], XmNheight,           XTAS_STB_LABEL_HEIGHT    ) ; n++ ;
XtSetArg( args[n], XmNwidth,            XtasStbLabelWidth        ) ; n++ ;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
nberrorwidget = label_widget  ;

/* max setup label */
n = 0 ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,        frame                    ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ) ; n++ ;
XtSetArg( args[n], XmNleftWidget,       frame                    ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       XTAS_STB_LABEL_OFFSET    ) ; n++ ;
XtSetArg( args[n], XmNheight,           XTAS_STB_LABEL_HEIGHT    ) ; n++ ;
XtSetArg( args[n], XmNwidth,            XtasStbLabelWidth        ) ; n++ ;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
setupwidget = label_widget  ;

/* max hold label */
n = 0 ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,        frame                    ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ) ; n++ ;
XtSetArg( args[n], XmNleftWidget,       frame                    ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
XtSetArg( args[n], XmNrecomputeSize,    False                    ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       XTAS_STB_LABEL_OFFSET    ) ; n++ ;
XtSetArg( args[n], XmNheight,           XTAS_STB_LABEL_HEIGHT    ) ; n++ ;
XtSetArg( args[n], XmNwidth,            XtasStbLabelWidth        ) ; n++ ;
label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, frame, args, n ) ;
holdwidget = label_widget  ;

/**/
n = 0 ;
XtSetArg( args[n], XmNborderWidth,      0               ) ; n++ ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,        errframe        ) ; n++ ;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
XtSetArg( args[n], XmNrightOffset,      2               ) ; n++ ;
dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n ) ;

text = XmStringCreateSimple( "Signal Name" ) ;
SIGNAME_label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                         xmLabelWidgetClass,
                                         dbase_form,
                                         XmNlabelString,    text,
                                         XmNtopAttachment,  XmATTACH_FORM,
                                         XmNleftAttachment, XmATTACH_FORM,
                                         XmNwidth,          label_signame_width,
                                         XmNtopOffset,      2,
                                         XmNrightOffset,    2,
                                         XmNleftOffset,     XTAS_STB_SIGNAME_LOFFSET,
                                         XmNheight,         25,
                                         NULL) ;
XmStringFree( text ) ;

text = XmStringCreateSimple( "Signal Type" ) ;

label_widget = XtVaCreateManagedWidget( "XtasMainTitles",
                                         xmLabelWidgetClass,
                                         dbase_form,
                                         XmNlabelString,    text,
                                         XmNtopAttachment,  XmATTACH_FORM,
                                         XmNleftAttachment, XmATTACH_WIDGET,
                                         XmNleftWidget,     SIGNAME_label_widget,
                                         XmNwidth,          100,
                                         XmNtopOffset,      2,
                                         XmNrightOffset,    2,
                                         XmNleftOffset,     2,
                                         XmNheight,         25,
                                         NULL) ;
XmStringFree( text ) ;

text = XmStringCreateSimple( "Setup Time" ) ;

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
                                         NULL) ;
XmStringFree( text ) ;

text = XmStringCreateSimple( "Hold Time" ) ;

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
                                         NULL) ;
XmStringFree( text ) ;

n = 0 ;
XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM    ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM    ) ; n++ ;
XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET  ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,       form             ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,      3                ) ; n++ ;
XtSetArg( args[n], XmNrightOffset,     3                ) ; n++ ;
separator1 = XtCreateManagedWidget( "separator12", xmSeparatorWidgetClass, form_widget, args, n ) ;

n = 0 ;
XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ) ; n++ ;
XtSetArg( args[n], XmNtopOffset,              2                 ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,             2                 ) ; n++ ;
XtSetArg( args[n], XmNrightOffset,            2                 ) ; n++ ;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form_widget, args, n ) ;

/* begin of old display */
label_widget = XtVaCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, form,
                                         XmNtopAttachment,   XmATTACH_FORM,
                                         XmNleftAttachment,  XmATTACH_FORM,
                                         XmNleftOffset,      2,
                                         NULL) ;
XtasSetLabelString( label_widget, "Informations Area" ) ;

n = 0 ;
XtSetArg( args[n], XmNtopAttachment,         XmATTACH_WIDGET   ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,             label_widget      ) ; n++ ;
XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ) ; n++ ;
XtSetArg( args[n], XmNrightPosition,         48                ) ; n++ ;
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
n = 0 ;
XtSetArg( args[n], XmNheight,                 20                       ) ; n++ ;
tas_winfos->meswidget = XtCreateManagedWidget( "XtasUserInfo", xmLabelWidgetClass, frame, args, n) ;
XtasSetLabelString( tas_winfos->meswidget, XTAS_NULINFO ) ;


n = 0 ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,        frame                    ) ; n++ ;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
XtSetArg( args[n], XmNleftWidget,       frame           ) ; n++ ; 
frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n ) ;
n = 0 ;
XtSetArg( args[n], XmNheight,           20              ) ; n++ ;
tas_winfos->hlpwidget = XtCreateManagedWidget( "XtasHelpInfo", xmLabelWidgetClass, frame, args, n) ;
XtasSetLabelString( tas_winfos->hlpwidget, XTAS_NULINFO ) ;

n = 0 ;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET            ) ; n++ ;
XtSetArg( args[n], XmNbottomWidget,     form    ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM              ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
XtSetArg( args[n], XmNleftOffset,       3                          ) ; n++ ;
XtSetArg( args[n], XmNrightOffset,      3                          ) ; n++ ;
separator2 = XtCreateManagedWidget( "separator2", xmSeparatorWidgetClass, form_widget, args, n ) ;

n = 0 ;
XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET            ) ; n++ ;
XtSetArg( args[n], XmNbottomWidget,     separator2    ) ; n++ ;
XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET            ) ; n++ ;
XtSetArg( args[n], XmNtopWidget,        separator1    ) ; n++ ;
XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM              ) ; n++ ;
XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM            ) ; n++ ;
pan_widget = XtCreateManagedWidget( "XtasPanWindow", xmPanedWindowWidgetClass, form_widget, args, n ) ;

if (XTAS_DISPLAY_NEW == 0)
{
n = 0 ;
form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, pan_widget, args, n ) ;

XtasCreateStbScrollList(form, stbfig,margin,nberror, nberrorwidget,setupwidget,holdwidget, SIGNAME_label_widget) ;
}
else
{
 XtasCreateStbScrollList2(pan_widget, stbfig,margin,nberror, nberrorwidget,setupwidget,holdwidget, SIGNAME_label_widget) ; 
}

XtManageChild( form_widget ) ;
return(form_widget) ;
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDisplayStbResult                                           */
/*                                                                           */
/* IN  ARGS :                                                                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Display the result of an Stb analysis                          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDisplayStbResult(stbfig)
stbfig_list *stbfig  ;
{
 char *text  ;
 XtasWindowStruct *tas_winfos ;
 Widget main_window ;
 Arg    args[20] ;
 long margin  ;
 int nberror  ;
 int    n ;
 char   text_title[128];

/** *** **                    ** *** **/
 
 text = XmTextGetString(STBMARGIN) ;
 if (text) {
    margin = atol(text)*TTV_UNIT ;
    XtFree (text);
 }
 
 XtasMargin = margin  ; /* pour interfacage avec debug */
 
 text = XmTextGetString(NBERROR) ;
 if (text) {
    nberror = atoi(text) ;
    if(nberror <= 0)
        nberror = 100  ;
    XtFree (text);
 }

 tas_winfos = (XtasWindowStruct *) mbkalloc( sizeof(XtasWindowStruct) ) ;
 XTAS_STB_SESSION = addchain(XTAS_STB_SESSION,tas_winfos) ;
 tas_winfos->topwidget = XtVaCreatePopupShell( NULL, topLevelShellWidgetClass, 
                                               XtasTopLevel,
                                               XmNwidth,            720,
                                               XmNallowShellResize, False,
                                               XmNdeleteResponse,   XmDESTROY,
                                               NULL ) ;
 n = 0 ;
 sprintf (text_title, XTAS_NAME": Stability Analysis Results - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
// XtSetArg( args[n], XmNtitle , XTAS_NAME": Stability Analysis Results") ;n++ ;
 XtSetArg( args[n], XmNtitle, text_title) ;n++ ;
 if(XTAS_DISPLAY_NEW)  { 
         XtSetArg( args[n], XmNwidth, 850) ; n++ ;
 }
 XtSetValues( tas_winfos->topwidget, args, n ) ;

 XalCustomIcon( tas_winfos->topwidget, XTAS_STBA_MAP ) ;
 main_window = XmCreateMainWindow( tas_winfos->topwidget, "XtasMain", args, n ) ;
 XtManageChild( main_window ) ;
 tas_winfos->menwidget = XtasCreateStbMenus( main_window ,tas_winfos ) ;
 XtManageChild( tas_winfos->menwidget ) ;
 tas_winfos->wrkwidget = XtasCreateStbList( main_window, tas_winfos, stbfig,margin,nberror) ;
 tas_winfos->errwidget = XalCreateErrorBox(   tas_winfos->wrkwidget ) ;
 tas_winfos->warwidget = XalCreateWarningBox( tas_winfos->wrkwidget ) ;
 
 XmMainWindowSetAreas( main_window,
                      tas_winfos->menwidget,
                      (Widget) NULL,
                      (Widget) NULL,
                      (Widget) NULL,
                      tas_winfos->wrkwidget ) ;
					  

 XtPopup( tas_winfos->topwidget, XtGrabNone ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbGetParamValue                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int    XtasStbGetParamValue (void)
{
    char    *text, *ptend;
    long     value;
    float    val;
    
    text = XmTextGetString(CTK_MARGIN) ;
    if( text ) {
        value = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(value < 0)) {
            XtFree (text);
            XtManageChild (XtasStbAnalysisWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Aggression Margin.\nMust be a positive integer." );
            return 1;
        }
        XtFree (text);
        XtasStbParam->ctk_margin = value ;
    }

    text = XmTextGetString(STB_MIN_SLOPE_CHANGE) ;
    if( text ) {
        value = strtol( text, &ptend, 10);
        if((*ptend != '\0')||(value < 0)) {
            XtFree (text);
            XtManageChild (XtasStbAnalysisWidget);
            XalDrawMessage(XtasErrorWidget, "Bad value for Min Slope Change.\nMust be a positive integer." );
            return 1;
        }
        XtFree (text);
        XtasStbParam->min_slope_change = value ;
    }

    text = XmTextGetString(STB_MAX_IT_NB) ;
    if( text ) {
        value = strtol( text, &ptend, 10 );
        if((*ptend != '\0')||(value < 0)||(strchr(text,'.'))) {
            XtFree (text);
            XtManageChild (XtasStbAnalysisWidget);
            XalDrawMessage (XtasErrorWidget, "Bad value for Max Iterations Number.\nMust be a positive integer." );
            return 1;
        }
        XtFree (text);
        XtasStbParam->max_last_iter = (int)value ;
    }

    if (XtasStbParam->cache_size > (float) 0.0) {
        text = XmTextGetString(STBCACHESIZE) ;
        if( text ) {
            val = strtod( text, &ptend);
            if((*ptend != '\0')||(val <= 0)) {
                XtFree (text);
                XtManageChild (XtasStbAnalysisWidget);
                XalDrawMessage (XtasErrorWidget, "Bad value for Cache Size.\nMust be a positive floating number." );
                return 1;
            }
            XtFree (text);
            XtasStbParam->cache_size = val;
        }


//        switch(XTAS_STB_CTK_CACHE_UNIT) {
//            case   XTAS_STB_CACHE_UNIT_KB  :
//                RCN_CACHE_SIZE =  (val * 1024)/(long)1;
//                break;
//            case   XTAS_STB_CACHE_UNIT_MB  :
//                XtasStbParam->cache_size =  (val * 1048576)/(long)1;
//                break;
//            case   XTAS_STB_CACHE_UNIT_GB  :
//                RCN_CACHE_SIZE =  (val * 1073741824)/(long)1;
//                break;
//        }
    }

    if((XtasStbParam->ctk_var & STB_CTK_REPORT) == STB_CTK_REPORT) { 
        text = XmTextGetString(REPORT_DELAY_MIN) ;
        if( text ) {
            value = strtol( text, &ptend, 10 );
            if( *ptend != '\0'|| value<0  ) {
                XtFree (text);
                XtManageChild (XtasStbAnalysisWidget);
                XalDrawMessage(XtasErrorWidget, 
                        "Bad value for minimum delta delay.\nMust be a positive integer."
                            );
                return 1;
            }
            XtFree (text);
            XtasStbParam->ctk_report_delay_min = value;
        }

        text = XmTextGetString(REPORT_SLOPE_MIN) ;
        if( text ) {
            value = strtol( text, &ptend, 10 );
            if( *ptend != '\0' || value<0 ) {
                XtFree (text);
                XtManageChild (XtasStbAnalysisWidget);
                XalDrawMessage(XtasErrorWidget, 
                        "Bad value for minimum delta slope.\nMust be a positive integer."
                             );
                return 1;
            }
            XtFree (text);
            XtasStbParam->ctk_report_slope_min = value;
        }

        text = XmTextGetString(REPORT_NOISE_MIN) ;
        if( text ) {
            value = strtol( text, &ptend, 10 );
            if( *ptend != '\0' || value<0 ) {
                XtFree (text);
                XtManageChild (XtasStbAnalysisWidget);
                XalDrawMessage( XtasErrorWidget , 
                        "Bad value for minimum noise.\nMust be a positive integer."
                             );
                return 1;
            }
            XtFree (text);
            XtasStbParam->ctk_report_noise_min = value;
        }

        text = XmTextGetString(REPORT_CTK_MIN) ;
        if( text ) {
            val = strtod( text, &ptend );
            if( *ptend != '\0' || val<0.0 || val>100.0 ) {
                XtFree (text);
                XtManageChild (XtasStbAnalysisWidget);
                XalDrawMessage( XtasErrorWidget, 
                        "Bad value for minimum crosstalk.\nMust be a floating number between 0 and 100."
                             );
                return 1;
            }
            XtFree (text);
            XtasStbParam->ctk_report_ctk_min = val;
        }
    }
    
    return 0;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbUpdateVariables                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasStbUpdateVariables (void)
{
//    STB_SILENT          = 'N';
    STB_ANALYSIS_VAR    = XtasStbParam->analysis_var;
    STB_GRAPH_VAR       = XtasStbParam->graph_var   ;
    STB_MODE_VAR        = XtasStbParam->mode_var    ;
    STB_CTK_VAR         = XtasStbParam->ctk_var     ;
    STB_OUT             = XtasStbParam->out         ;
    STB_REPORT          = XtasStbParam->report      ;
    RCX_CTK_MODEL       = XtasStbParam->rcx_ctk_model    ;
    RCX_CTK_NOISE       = XtasStbParam->rcx_ctk_noise    ;
    RCX_CTK_SLOPE_NOISE = XtasStbParam->rcx_ctk_slope_noise    ;
    RCX_CTK_SLOPE_DELAY = XtasStbParam->rcx_ctk_slope_delay    ;
    STB_CTK_MARGIN      = XtasStbParam->ctk_margin * TTV_UNIT  ;
    STB_CTK_MINSLOPECHANGE      = XtasStbParam->min_slope_change * TTV_UNIT   ;
    STB_CTK_MAXLASTITER         = XtasStbParam->max_last_iter       ;
    CTK_REPORT_DELTA_DELAY_MIN  = XtasStbParam->ctk_report_delay_min;
    CTK_REPORT_DELTA_SLOPE_MIN  = XtasStbParam->ctk_report_slope_min;
    CTK_REPORT_CTK_MIN          = XtasStbParam->ctk_report_ctk_min  ;
    CTK_REPORT_NOISE_MIN        = XtasStbParam->ctk_report_noise_min; 
    RCN_CACHE_SIZE              = (XtasStbParam->cache_size * 1048576) /(long) 1;

    /* on force le report du fichier ctx */
    STB_CTK_VAR |= STB_CTX_REPORT;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbAnalysis                                                */
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
void XtasStbAnalysis(void)
{
    char    buf[1024];

    if (!XtasStbGetParamValue ()) {
        XtasStbUpdateVariables ();
        
        XtasPasqua() ;
        if(sigsetjmp( XtasMyEnv , 1 ) == 0)
        {
            XalSetCursor( XtasTopLevel, WAIT ) ;
            XalForceUpdate( XtasTopLevel ) ;
            if(XtasMainParam->stbfig != NULL)
            {
                XtasRemoveStbSession(NULL) ;
                stb_ctk_clean(XtasMainParam->stbfig);
                stb_delstbfig(XtasMainParam->stbfig);
                XtasMainParam->stbfig = NULL  ;
                XTAS_CTX_LOADED = XTAS_NOT_LOADED;
            }

            if (XTAS_STB_PARSE_STO == 'Y') {
                sprintf (buf, "%s.sto", XtasMainParam->ttvfig->INFO->FIGNAME);
                XtasMainParam->stbfig = stb_parseSTO (XtasMainParam->ttvfig, buf);
            }
            else {
                MaxSig = TTV_MAX_SIG  ; 
                TTV_MAX_SIG = TTV_ALLOC_MAX ;
                if((STB_CTK_VAR & STB_CTK) == STB_CTK)
                    ctk_setprogressbar(XtasStbInProgress);
                ttv_init_stm(XtasMainParam->ttvfig);
                XtasMainParam->stbfig = stb_analysis(XtasMainParam->ttvfig, STB_ANALYSIS_VAR, STB_GRAPH_VAR, STB_MODE_VAR, STB_CTK_VAR) ;
                if((STB_CTK_VAR & STB_CTK) == STB_CTK) {
                    XtasDestroyStbInProgress();
                    XTAS_CTX_LOADED = XTAS_FROM_ANALYSIS; 
                }
            }

            if(XtasMainParam->stbfig != NULL)
            {
                XtasDisplayStbResult(XtasMainParam->stbfig) ;
            }
        }
        else
            return  ;


        XtasGetWarningMess() ;
        XtasFirePasqua() ;
        XalSetCursor( XtasTopLevel, NORMAL ) ;
        if (XtasStbAnalysisWidget) {
            XtDestroyWidget (XtasStbAnalysisWidget);
            XtasStbAnalysisWidget = NULL;
        }
    }
}

/******************************************************************************/

void XtasStabilityOkCallback( widget, client_data, call_data )
Widget  widget ;
XtPointer client_data ;
XtPointer call_data ;
{

    XalSetCursor( XtasTopLevel, WAIT ) ;
    XalForceUpdate( XtasTopLevel ) ;
    XtasStbAnalysis() ;
/*    XalLeaveLimitedLoop();*/
    
}

/******************************************************************************/

void XtasCancelStabilityCallback( widget, client_data, call_data )
Widget  widget ;
XtPointer client_data ;
XtPointer call_data ;
{
  //XtVaSetValues(CTK_TOGGLE, XmNset, False, NULL);
  //XtSetSensitive(FILE_TTX_TOGGLE, True);
  //XtSetSensitive(CTK_FORM, False);
/*  XalLeaveLimitedLoop() ;*/
}

/******************************************************************************/

void XtasStbGlobAnalysisCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if (state->set)
    XtasStbParam->analysis_var = STB_GLOB_ANALYSIS;
 else
    XtasStbParam->analysis_var = STB_DET_ANALYSIS;
}

/******************************************************************************/

void XtasStbDetAnalysisCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if (state->set)
    XtasStbParam->analysis_var = STB_DET_ANALYSIS;
 else
    XtasStbParam->analysis_var = STB_GLOB_ANALYSIS;
}

/******************************************************************************/

void XtasAnalysisWorstCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if (state->set)
    XtasStbParam->mode_var |= STB_STABILITY_WORST;
 else
    XtasStbParam->mode_var &= ~STB_STABILITY_WORST; 
}

/******************************************************************************/

void XtasAnalysisBestCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if (state->set)
    XtasStbParam->mode_var |= STB_STABILITY_BEST;
 else
    XtasStbParam->mode_var &= ~STB_STABILITY_BEST;    

}

/******************************************************************************/

void XtasAnalysisAllLevelCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;
 
 if (state->set)
     XtasStbParam->mode_var &= ~STB_STABILITY_LAST  ;
 else
     XtasStbParam->mode_var = STB_STABILITY_LAST ;
}

/******************************************************************************/

void XtasAnalysisTopLevelCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if (state->set)
     XtasStbParam->mode_var = STB_STABILITY_LAST;
 else
     XtasStbParam->mode_var = STB_STABILITY_LAST;
}

/******************************************************************************/

void XtasAnalysisSetupCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
   XtasStbParam->mode_var |= STB_STABILITY_SETUP ;
 else
   XtasStbParam->mode_var &= ~(STB_STABILITY_SETUP) ;
}

/******************************************************************************/

void XtasAnalysisHoldCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
   XtasStbParam->mode_var |= STB_STABILITY_HOLD ;
 else
   XtasStbParam->mode_var &= ~(STB_STABILITY_HOLD) ;
}

/******************************************************************************/

void XtasAnalysisMonophaseFfCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
  {
   XtasStbParam->mode_var |= STB_STABILITY_FF  ;
  }
 else
 {
  XtasStbParam->mode_var &= ~(STB_STABILITY_FF) ;
 }
}

/******************************************************************************/

void XtasDriveStoCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
   XtasStbParam->out = 'Y'  ;
 else
   XtasStbParam->out = 'N'  ;
}

/******************************************************************************/

void XtasDriveStrCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) 
   XtasStbParam->report = 'Y'  ;
 else
   XtasStbParam->report = 'N'  ;
}

/******************************************************************************/

void XtasAnalysisMonophaseLtCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
  {
   XtasStbParam->mode_var |= STB_STABILITY_LT  ;
  }
 else
 {
  XtasStbParam->mode_var &= ~(STB_STABILITY_LT) ;
 }
}

/******************************************************************************/

void XtasAnalysisMonophaseErCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;
 
 if( state->set )
  {
   XtasStbParam->mode_var |= STB_STABILITY_ER  ;
  }
 else
 {
  XtasStbParam->mode_var &= ~(STB_STABILITY_ER) ;
 }
}


/******************************************************************************/

void XtasAnalysisTtxCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 XtasStbParam->graph_var = ( state->set ) ? STB_RED_GRAPH : STB_DET_GRAPH  ;
}

/******************************************************************************/

void XtasAnalysisDtxCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 XtasStbParam->graph_var = ( state->set ) ? STB_DET_GRAPH : STB_RED_GRAPH  ;
}

/******************************************************************************/

void XtasAnalysisCtkCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) {
     XtasStbParam->ctk_var |= STB_CTK ;
     XtSetSensitive(FILE_TTX_TOGGLE, False);
     XtVaSetValues(FILE_TTX_TOGGLE, XmNset, False, NULL);
     XtVaSetValues(FILE_DTX_TOGGLE, XmNset, True, NULL);
     XtSetSensitive(CTK_FORM, True);
     XtasStbParam->graph_var = STB_DET_GRAPH;
 }
 else {
     XtasStbParam->ctk_var &= ~STB_CTK ;
     XtasStbParam->ctk_var |= STB_CTK_NOT ;
     XtSetSensitive(FILE_TTX_TOGGLE, True);
     XtSetSensitive(CTK_FORM, False);
 }
 
}

/******************************************************************************/

void XtasParseSTOCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) {
     XTAS_STB_PARSE_STO = 'Y';
     XtSetSensitive(ANALYSIS_FORM, False);
     XtSetSensitive(INFO_FORM, False);
     XtSetSensitive(CTK_FORM, False);
     XtVaSetValues(CTK_TOGGLE, XmNset, False, NULL);
     XtSetSensitive(FILE_TTX_TOGGLE, True);
 }
 else {
     XTAS_STB_PARSE_STO = 'N';
     XtSetSensitive(ANALYSIS_FORM, True);
     XtSetSensitive(INFO_FORM, True);
 }
 
}

/******************************************************************************/

void XtasAnalysisCtkBestObsCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
     XtasStbParam->ctk_var |= STB_CTK_OBSERVABLE;
 else
     XtasStbParam->ctk_var &= ~STB_CTK_OBSERVABLE;
}


/******************************************************************************/

void XtasAnalysisCtkBestCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) {
     XtasStbParam->ctk_var &= ~STB_CTK_WORST;
     XtSetSensitive(CTK_TOGGLE_OBS, True);
 }
 else {
     XtasStbParam->ctk_var |= STB_CTK_WORST;
     XtSetSensitive(CTK_TOGGLE_OBS, False);
 }
}

/******************************************************************************/

void XtasAnalysisCtkWorstCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set )
     XtasStbParam->ctk_var |= STB_CTK_WORST;
 else
     XtasStbParam->ctk_var &= ~STB_CTK_WORST;

}


/******************************************************************************/

void XtasAnalysisCtkReportCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) {
     XtasStbParam->ctk_var |= STB_CTK_REPORT ;
     XtSetSensitive(REPORT_CTK_FORM, True);
 }
 
 else {
     XtasStbParam->ctk_var &= ~STB_CTK_REPORT ;
     XtSetSensitive(REPORT_CTK_FORM, False);
 }     
 
     
 
}
/******************************************************************************/

void XtasAnalysisCtkUseCacheCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if( state->set ) {
     XtSetSensitive(USECACHE_FORM, True);
     XtasStbParam->cache_size = (float)10.0;
 } 
 else {
     XtSetSensitive(USECACHE_FORM, False);
     XtasStbParam->cache_size = (float)0.0;
 }     
 
     
 
}
/******************************************************************************/

void XtasAnalysisCtkModeleCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;

 if (state->set) {
    switch((char)(long)client_data) {
        case RCX_MILLER_0C2C    : XtasStbParam->rcx_ctk_model = RCX_MILLER_0C2C; break;
        case RCX_MILLER_NOMINAL : XtasStbParam->rcx_ctk_model = RCX_MILLER_NOMINAL; break;
        case RCX_MILLER_NC3C    : XtasStbParam->rcx_ctk_model = RCX_MILLER_NC3C; break;
    }
 }

}
/******************************************************************************/

void XtasAnalysisCtkNoiseCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;
 if (state->set) {
    switch((char)(long)client_data) {
        case RCX_NOISE_NEVER      : XtasStbParam->rcx_ctk_noise = RCX_NOISE_NEVER; break;
        case RCX_NOISE_FINE       : XtasStbParam->rcx_ctk_noise = RCX_NOISE_FINE; break;
    }
 }

}
 
/******************************************************************************/

void XtasAnalysisCtkSlopeNoiseCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;
 if (state->set) {
    switch((char)(long)client_data) {
        case RCX_SLOPE_NOMINAL   : XtasStbParam->rcx_ctk_slope_noise = RCX_SLOPE_NOMINAL; break;
        case RCX_SLOPE_CTK       : XtasStbParam->rcx_ctk_slope_noise = RCX_SLOPE_CTK; break;
        case RCX_SLOPE_REAL      : XtasStbParam->rcx_ctk_slope_noise = RCX_SLOPE_REAL; break;
    }
 }

}

/******************************************************************************/

void XtasAnalysisCtkSlopeDelayCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs  ;
 if (state->set) {
    switch((char)(long)client_data) {
        case RCX_SLOPE_DELAY_ENHANCED  : XtasStbParam->rcx_ctk_slope_delay = RCX_SLOPE_DELAY_ENHANCED; break;
        case RCX_SLOPE_CTK       : XtasStbParam->rcx_ctk_slope_delay = RCX_SLOPE_CTK; break;
    }
 }

}

/******************************************************************************/
#if XTAS_MOTIF_VERSION >= 20100
void XtasAnalysisCtkCacheUnitCallback( widget, client_data, cbs )
Widget  widget ;
XtPointer client_data ;
XtPointer cbs  ;
{
    char *text;
    
    
    XmComboBoxCallbackStruct *cb = (XmComboBoxCallbackStruct *) cbs;
    
    text = (char *) XmStringUnparse (cb->item_or_text,
                                            XmFONTLIST_DEFAULT_TAG,
                                            XmCHARSET_TEXT, XmCHARSET_TEXT,
                                            NULL, 0, XmOUTPUT_ALL);
    if(!strcmp(text,"Kb"))
        XTAS_STB_CTK_CACHE_UNIT = XTAS_STB_CACHE_UNIT_KB;
    else if(!strcmp(text,"Mb"))
        XTAS_STB_CTK_CACHE_UNIT = XTAS_STB_CACHE_UNIT_MB;
    else        XTAS_STB_CTK_CACHE_UNIT = XTAS_STB_CACHE_UNIT_GB;

    //mbkfree(text);
}
#endif

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasStbParamInit                                               */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasStbParamInit (void)
{
    if (!XtasStbParam) {
        XtasStbParam = (xtas_stb_param_struct*) mbkalloc (sizeof (xtas_stb_param_struct));
    }

    XtasStbParam->report        = STB_REPORT    ;
    XtasStbParam->out           = STB_OUT       ;
    XtasStbParam->analysis_var  = STB_ANALYSIS_VAR  ;
    XtasStbParam->graph_var     = (((STB_GRAPH_VAR==STB_DET_GRAPH) || (STB_CTK_VAR & STB_CTK)==STB_CTK) || (XTAS_FORMAT_LOADED==XTAS_DTX)) ?  STB_DET_GRAPH:STB_RED_GRAPH;
    XtasStbParam->mode_var      = STB_MODE_VAR  ;
    XtasStbParam->ctk_var       = STB_CTK_VAR   ;
    XtasStbParam->rcx_ctk_model = RCX_CTK_MODEL ;
    XtasStbParam->rcx_ctk_noise = RCX_CTK_NOISE ;
    XtasStbParam->rcx_ctk_slope_noise = RCX_CTK_SLOPE_NOISE ;
    XtasStbParam->rcx_ctk_slope_delay = RCX_CTK_SLOPE_DELAY ;
    XtasStbParam->ctk_margin    = STB_CTK_MARGIN / TTV_UNIT   ;
    XtasStbParam->min_slope_change      = STB_CTK_MINSLOPECHANGE / TTV_UNIT   ;
    XtasStbParam->max_last_iter         = STB_CTK_MAXLASTITER       ;
    XtasStbParam->ctk_report_delay_min  = CTK_REPORT_DELTA_DELAY_MIN;
    XtasStbParam->ctk_report_slope_min  = CTK_REPORT_DELTA_SLOPE_MIN;
    XtasStbParam->ctk_report_ctk_min    = CTK_REPORT_CTK_MIN        ;
    XtasStbParam->ctk_report_noise_min  = CTK_REPORT_NOISE_MIN      ;
    XtasStbParam->cache_size            = RCN_CACHE_SIZE / 1048576.0; /* conversion en MBytes */

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
void XtasStbAnalysisCallback( widget, client_data, call_data )
Widget  widget ;
XtPointer client_data ;
XtPointer call_data ;
{
  int      n ;
  Arg      args[20];
  XmString text ;
  Widget   form, form_t, dbase_form, ebase_form, fbase_form  ;
  Widget   row_widget, row2, frame_widget, label_widget, toggle, tmp_widget ;
  Widget   gbase_form, hbase_form, option_form, button;
//  Widget   unit_list;
//  int count_unit = XtNumber (CACHE_UNIT);
//  int i;
//  XmStringTable str_list;
  Atom     WM_DELETE_WINDOW;
  char     buf[128];
  Pixmap   pixmap;
  Pixel    fg, bg;
  FILE    *stoin;

  if(XtasStbAnalysisWidget && /*(V_BOOL_TAB[__STB_DET_GRAPH].VALUE) &&*/
     (((XtasStbParam->graph_var == STB_DET_GRAPH) && (XTAS_FORMAT_LOADED == XTAS_TTX)) ||
      ((XtasStbParam->graph_var == STB_RED_GRAPH) && (XTAS_FORMAT_LOADED == XTAS_DTX)))) {
      if (XTAS_FORMAT_LOADED == XTAS_DTX)       XtasStbParam->graph_var = STB_DET_GRAPH;
      else if (XTAS_FORMAT_LOADED == XTAS_TTX)  XtasStbParam->graph_var = STB_RED_GRAPH;
      XtDestroyWidget(XtasStbAnalysisWidget);
      mbkfree (XtasStbParam);
      XtasStbParam = NULL;
      XtasStbAnalysisWidget = NULL;
  }

 if( XtasAppNotReady() ) return  ;


 if( !XtasStbAnalysisWidget )
   {
    XtasStbParamInit ();
    
    n = 0 ;
    XtSetArg( args[n], XmNtitle, XTAS_NAME" : Stability Parameterization" ) ; n++ ;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ) ; n++ ;
    XtSetArg( args[n], XmNwidth, 850) ; n++ ;
    XtasStbAnalysisWidget = XmCreatePromptDialog(XtasDeskMainForm, "XtasInfosBox",args, n) ;
	HelpFather = XtasStbAnalysisWidget ;
    XtUnmanageChild(XmSelectionBoxGetChild(XtasStbAnalysisWidget,XmDIALOG_TEXT)) ;
    XtUnmanageChild(XmSelectionBoxGetChild(XtasStbAnalysisWidget,XmDIALOG_PROMPT_LABEL)) ;
    XtAddCallback( XtasStbAnalysisWidget, XmNokCallback, XtasStabilityOkCallback, (XtPointer)XtasStbAnalysisWidget ) ;
    XtAddCallback( XtasStbAnalysisWidget, XmNcancelCallback, XtasCancelStabilityCallback, (XtPointer)XtasStbAnalysisWidget ) ;
    XtAddCallback( XtasStbAnalysisWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_STABPARAM)) ;
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(XtParent(XtasStbAnalysisWidget), WM_DELETE_WINDOW, XtasCancelStabilityCallback, (XtPointer )XtasStbAnalysisWidget);
    //XmAddTabGroup( XtasStbAnalysisWidget ) ;

    n = 0 ;
    form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, XtasStbAnalysisWidget, args, n ) ;
 
    text = XmStringCreateSimple( "Load Switching Windows" ) ;
    STO_TOGGLE = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, form,
                                      XmNset,           False,
                                      XmNheight,        20,
                                      XmNlabelString,   text,
                                      XmNrightAttachment,   XmATTACH_FORM,
                                      XmNleftAttachment,    XmATTACH_FORM,
                                      XmNleftOffset,        330,
                                      XmNrightOffset,       330,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( STO_TOGGLE, XmNvalueChangedCallback, XtasParseSTOCallback, NULL) ;
   
    
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        STO_TOGGLE      ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    ANALYSIS_FORM = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, form, args, n ) ;


    /****** CTK ******/
//    n = 0 ;
//    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
//    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
//    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
//    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n ) ;
//    form_t = dbase_form ;
//    n = 0 ;
//    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ; 
//
    text = XmStringCreateSimple( "Crosstalk Analysis" ) ;
    CTK_TOGGLE = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, ANALYSIS_FORM,
                                      XmNset,           ((XtasStbParam->ctk_var & STB_CTK) == STB_CTK) ? True:False,
                                      XmNheight,        20,
                                      XmNlabelString,   text,
                                      XmNleftAttachment,   XmATTACH_FORM,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( CTK_TOGGLE, XmNvalueChangedCallback, XtasAnalysisCtkCallback, NULL) ;
   


    /*------------------------------- Crosstalk Parameterization -----------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        CTK_TOGGLE        ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ANALYSIS_FORM, args, n ) ;
    CTK_FORM = dbase_form ;
    n = 0 ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ; 
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,   2                     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple( "Crosstalk Parameterization" ) ;
    n = 0  ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,         frame_widget      ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ) ; n++ ;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n ) ;

    /*------------------------------------------ Crosstalk Analysis Type -------------------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, fbase_form, args, n ) ;

    text = XmStringCreateSimple( "Crosstalk Analysis Type" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       2                 ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           43     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNheight,           39     ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       frame_widget,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      15,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;
    //XmAddTabGroup( row_widget ) ;

    

    text = XmStringCreateSimple( "Remove Non-Aggression" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->ctk_var & STB_CTK_WORST) == STB_CTK_WORST) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkWorstCallback, NULL) ;

    text = XmStringCreateSimple( "Detect Aggression" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->ctk_var & STB_CTK_WORST) == STB_CTK_WORST) ? False:True,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkBestCallback, NULL ) ;

    text = XmStringCreateSimple( "Observable Only" ) ;
    CTK_TOGGLE_OBS = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, ebase_form,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasStbParam->ctk_var & STB_CTK_OBSERVABLE) == STB_CTK_OBSERVABLE) ? True:False,
                                      XmNtopAttachment, XmATTACH_WIDGET,
                                      XmNtopWidget,     row_widget,
                                      XmNleftAttachment,XmATTACH_FORM,
                                      XmNleftOffset,    30,
                                      XmNsensitive,     True,
                                      XmNlabelString,   text,
                                      XmNheight,        20,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkBestObsCallback, NULL) ;


    /*--------------------- Stop conditions ---------------------------------------------*/
//    n = 0 ;
//    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
//    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
//    XtSetArg( args[n], XmNtopWidget,        ebase_form        ) ; n++ ;
//    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
//    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
//    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, fbase_form, args, n ) ;

    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        CTK_TOGGLE_OBS    ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        45                ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       2                 ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           43                ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;

    
    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, frame_widget,
                                           XmNtopAttachment,   XmATTACH_FORM,
                                           XmNbottomAttachment, XmATTACH_FORM,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNentryAlignment,  XmALIGNMENT_CENTER,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNmarginHeight,    0,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           NULL) ;
    
    text = XmStringCreateSimple( "No More Aggressions" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, row_widget, args, n ) ;
    XmStringFree( text ) ;
    text = XmStringCreateSimple( "Stop Conditions" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, row_widget, args, n ) ;
    XmStringFree( text ) ;

  
    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       frame_widget,
                                           XmNbottomAttachment, XmATTACH_FORM,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      5,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           NULL) ;
  //XmAddTabGroup( row_widget ) ;
    /*------------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    gbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, row_widget, args, n ) ;


    text = XmStringCreateSimple( "Min Slope Change" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,     False              ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            140                ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, gbase_form, args, n ) ;
    XmStringFree( text ) ;  
        
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        label_widget             ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,     label_widget             ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       label_widget             ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            30                       ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1                        ) ; n++ ;

    STB_MIN_SLOPE_CHANGE = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, gbase_form , args, n) ;
    sprintf (buf, "%ld", XtasStbParam->min_slope_change);
    XmTextSetString(STB_MIN_SLOPE_CHANGE , buf ) ;

    text = XmStringCreateSimple( "ps" ) ;
    
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,    False             ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       STB_MIN_SLOPE_CHANGE       ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            30                ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, gbase_form, args, n ) ;
    XmStringFree( text ) ; 

    /*------------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    gbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, row_widget, args, n ) ;

    text = XmStringCreateSimple( "Max Iterations Number" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,     False            ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            140               ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, gbase_form, args, n ) ;
    XmStringFree( text ) ;  
        
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        label_widget             ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,     label_widget             ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET          ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       label_widget             ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            30                       ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1                        ) ; n++ ;

    STB_MAX_IT_NB = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, gbase_form, args, n) ;
    sprintf (buf, "%d", XtasStbParam->max_last_iter);
    XmTextSetString( STB_MAX_IT_NB, buf ) ;       

    /*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    45   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, fbase_form, args, n ) ;

 /*----------------------- Options -------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ) ; n++ ;
    option_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, fbase_form, args, n ) ;

    text = XmStringCreateSimple( "Options" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           43                ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, option_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNheight,           39     ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    /*------------------------------------------------------------------------------------------*/


    text = XmStringCreateSimple( "Generate a report file (.ctk)" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, option_form,
                                      XmNtopAttachment,     XmATTACH_WIDGET,
                                      XmNtopWidget,         frame_widget,
                                      XmNleftAttachment,    XmATTACH_FORM,
                                      XmNleftOffset,        5,
                                      XmNset,               ((XtasStbParam->ctk_var & STB_CTK_REPORT) == STB_CTK_REPORT) ? True:False,
                                      XmNlabelString,       text,
                                      NULL) ;
    XmStringFree( text ) ;    
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkReportCallback, NULL) ;
    

    /*------------------------------------------------------------------------------------------*/
    REPORT_CTK_FORM = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, option_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       toggle,
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      5,
                                           XmNrightAttachment, XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNsensitive,       ((XtasStbParam->ctk_var & STB_CTK_REPORT) == STB_CTK_REPORT) ? True:False,
                                           XmNisAligned,       True,
                                           NULL) ;
    /*------------------------------------------------------------------------------------------*/

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, REPORT_CTK_FORM, args, n ) ;

    text = XmStringCreateSimple( "Minimum delta delay:" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text                ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,    False               ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       10                  ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                  ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            135                  ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;  
        
    n = 0 ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, label_widget); n++;
    XtSetArg( args[n], XmNwidth,            45                       ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1                        ) ; n++ ;
    REPORT_DELAY_MIN = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args, n) ;
    sprintf (buf, "%ld", XtasStbParam->ctk_report_delay_min);
    XmTextSetString( REPORT_DELAY_MIN, buf) ;       
    
    text = XmStringCreateSimple("ps");
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg (args[n], XmNtopOffset,        5               ); n++;
    XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg (args[n], XmNleftWidget,       REPORT_DELAY_MIN); n++;
    XtSetArg (args[n], XmNleftOffset,       5               ); n++;
    XtSetArg (args[n], XmNlabelString,      text            ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;
   
    
    /*------------------------------------------------------------------------------------------*/

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, REPORT_CTK_FORM, args, n ) ;

    text = XmStringCreateSimple( "Minimum delta slope:" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text                ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,    False               ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       10                  ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                  ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            135                  ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;  
        
    n = 0 ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, label_widget); n++;
    XtSetArg( args[n], XmNwidth,            45                       ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1                        ) ; n++ ;
    REPORT_SLOPE_MIN = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args, n) ;
    sprintf (buf, "%ld", XtasStbParam->ctk_report_slope_min);
    XmTextSetString( REPORT_SLOPE_MIN, buf ) ;       
    
    text = XmStringCreateSimple("ps");
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg (args[n], XmNtopOffset,        5   ); n++;
    XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg (args[n], XmNleftWidget,       REPORT_SLOPE_MIN    ); n++;
    XtSetArg (args[n], XmNleftOffset,       5              ); n++;
    XtSetArg (args[n], XmNlabelString,      text            ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;
   
    /*------------------------------------------------------------------------------------------*/

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, REPORT_CTK_FORM, args, n ) ;

    text = XmStringCreateSimple( "Minimum noise:" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text                ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,    False               ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       10                  ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                  ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            135                  ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;  
        
    n = 0 ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, label_widget); n++;
    XtSetArg( args[n], XmNwidth,            45                       ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1                        ) ; n++ ;
    REPORT_NOISE_MIN = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args, n) ;
    sprintf (buf, "%ld", XtasStbParam->ctk_report_noise_min);
    XmTextSetString( REPORT_NOISE_MIN, buf) ;       
    
    text = XmStringCreateSimple("mV");
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg (args[n], XmNtopOffset,        5   ); n++;
    XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg (args[n], XmNleftWidget,       REPORT_NOISE_MIN    ); n++;
    XtSetArg (args[n], XmNleftOffset,       5              ); n++;
    XtSetArg (args[n], XmNlabelString,      text            ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;
   
    /*------------------------------------------------------------------------------------------*/

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, REPORT_CTK_FORM, args, n ) ;

    text = XmStringCreateSimple( "Minimum crosstalk:" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text                ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,    False               ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       10                  ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                  ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            135                  ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;  
        
    n = 0 ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET   ); n++;
    XtSetArg (args[n], XmNleftWidget, label_widget          ); n++;
    XtSetArg( args[n], XmNwidth,            45              ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1               ) ; n++ ;
    REPORT_CTK_MIN = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args, n) ;
    sprintf (buf, "%.2f", XtasStbParam->ctk_report_ctk_min);
    XmTextSetString( REPORT_CTK_MIN, buf) ;       
    
    text = XmStringCreateSimple("%");
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg (args[n], XmNtopOffset,        5   ); n++;
    XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg (args[n], XmNleftWidget,       REPORT_CTK_MIN    ); n++;
    XtSetArg (args[n], XmNleftOffset,       5              ); n++;
    XtSetArg (args[n], XmNlabelString,      text            ); n++;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, tmp_widget, args, n ) ;
    XmStringFree( text ) ;

    /*------------------------------------------------------------------------------------------*/


    text = XmStringCreateSimple( "Use Cache" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, option_form,
                                      XmNtopAttachment, XmATTACH_WIDGET,
                                      XmNtopWidget,     REPORT_CTK_FORM,
                                      XmNtopOffset,     20,
                                      XmNleftAttachment,XmATTACH_FORM,
                                      XmNleftOffset,    5,
                                      XmNset,           XtasStbParam->cache_size > (float)0.0 ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;    
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkUseCacheCallback, NULL ) ;
    

    /*------------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        toggle            ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       5                 ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ) ; n++ ;
    USECACHE_FORM = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, option_form, args, n ) ;

    text = XmStringCreateSimple( "Cache Size:" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text                ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,    False               ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        1                   ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       10                  ) ; n++ ;
    XtSetArg( args[n], XmNheight,           30                  ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            80                  ) ; n++ ;
    XtSetArg( args[n], XmNalignment,        XmALIGNMENT_BEGINNING) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, USECACHE_FORM, args, n ) ;
    XmStringFree( text ) ;  
        

    n = 0 ;
    XtSetArg (args[n], XmNleftAttachment, XmATTACH_WIDGET); n++;
    XtSetArg (args[n], XmNleftWidget, label_widget); n++;
    XtSetArg( args[n], XmNwidth,            70                       ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,  1                        ) ; n++ ;

    STBCACHESIZE = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, USECACHE_FORM, args, n) ;
    if (XtasStbParam->cache_size < (float)1)
        sprintf (buf, "%.4f", XtasStbParam->cache_size);
    else if (XtasStbParam->cache_size < (float)10)
        sprintf (buf, "%.2f", XtasStbParam->cache_size);
    else
        sprintf (buf, "%.0f", XtasStbParam->cache_size);

    XmTextSetString( STBCACHESIZE, buf) ;       

//#if XTAS_MOTIF_VERSION >= 20100
//    /* create the List items */
//    str_list = (XmStringTable) XtMalloc (count_unit * sizeof (XmString *));
//    
//    for (i = 0; i < count_unit; i++)
//        str_list[i] = XmStringCreateLocalized (CACHE_UNIT[i]);
//    
//    /* create the combobox */
//    n = 0;
//    XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
//    XtSetArg (args[n], XmNleftWidget,       STBCACHESIZE    ); n++;
//    XtSetArg (args[n], XmNleftOffset,       10              ); n++;
//    XtSetArg( args[n], XmNwidth,            60              ); n++;
//    XtSetArg (args[n], XmNitems,            str_list        ); n++;
//    XtSetArg (args[n], XmNitemCount,        count_unit      ); n++;
//    XtSetArg (args[n], XmNpositionMode,     XmONE_BASED     ); n++;
//    XtSetArg (args[n], XmNselectedPosition, 2               ); n++;
//    CACHE_UNIT_LIST= XmCreateDropDownList (USECACHE_FORM, "XtasList", args, n);
//    //XtAddCallback (list_w, XmNdefaultActionCallback, sel_callback, NULL);
//    
//    
//    for (i = 0; i < count_unit; i++)
//        XmStringFree (str_list[i]);
//    XtFree ((XtPointer) str_list);  
//
//    XtAddCallback (CACHE_UNIT_LIST, XmNselectionCallback, XtasAnalysisCtkCacheUnitCallback, NULL);
//    XTAS_STB_CTK_CACHE_UNIT = XTAS_STB_CACHE_UNIT_MB;
//
//    XtManageChild (CACHE_UNIT_LIST);   
//#else
    text = XmStringCreateSimple("Mb");
    n = 0;
    XtSetArg (args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg (args[n], XmNtopOffset,        5   ); n++;
    XtSetArg (args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg (args[n], XmNleftWidget,       STBCACHESIZE    ); n++;
    XtSetArg (args[n], XmNleftOffset,       5              ); n++;
    XtSetArg (args[n], XmNlabelString,      text            ); n++;
    CACHE_UNIT_LIST = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, USECACHE_FORM, args, n ) ;
    XmStringFree( text ) ;
//#endif    

    XtSetSensitive(CTK_FORM, ((XtasStbParam->ctk_var & STB_CTK) == STB_CTK) ? True:False);
    
/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    45   ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNrightWidget,       option_form      ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    option_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, fbase_form, args, n ) ;
    
 /*-----------------------Prise en compte des capacites de couplage -------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNrightWidget,      option_form        ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, fbase_form, args, n ) ;

    text = XmStringCreateSimple( "Crosstalk Model" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           43                ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNheight,           39     ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    /*------------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg ( args[n], XmNtopWidget,       frame_widget      ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
//    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ) ; n++ ;
    hbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple( "Capacitance For Delays" ) ;
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNleftOffset,      25   ); n++;
    XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNrightOffset,     25   ); n++;
    XtSetArg ( args[n], XmNlabelString,     text); n++;
    label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, hbase_form, args, n);
    XmStringFree (text);

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, hbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       label_widget,   
                                           XmNtopOffset,       5,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      40,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;

    text = XmStringCreateSimple( "  0C  1C  2C" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_model == RCX_MILLER_0C2C) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkModeleCallback, (XtPointer)RCX_MILLER_0C2C) ;

    text = XmStringCreateSimple( "  0C   to  2C" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_model == RCX_MILLER_NOMINAL) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkModeleCallback, (XtPointer)RCX_MILLER_NOMINAL) ;

    text = XmStringCreateSimple( "-1C   to  3C" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_model == RCX_MILLER_NC3C) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkModeleCallback, (XtPointer)RCX_MILLER_NC3C) ;

/*--------------------------------- horizontal separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       5              ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      5              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        row_widget      ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    ) ; n++ ;
    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, hbase_form, args, n ) ;


 /*----------------------- Noise model -------------------------------*/ 
    text = XmStringCreateSimple( "Noise For Delays" ) ;
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg ( args[n], XmNtopWidget,       gbase_form      ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNlabelString,     text); n++;
    label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, hbase_form, args, n);
    XmStringFree (text);

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, hbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       label_widget,   
                                           XmNtopOffset,       5,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      40,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;

    text = XmStringCreateSimple( "Never" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_noise == RCX_NOISE_NEVER) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkNoiseCallback, (XtPointer)RCX_NOISE_NEVER) ;

    text = XmStringCreateSimple( "Fine" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_noise == RCX_NOISE_FINE) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkNoiseCallback, (XtPointer)RCX_NOISE_FINE) ;

/*--------------------------------- horizontal separator ----------------------------------------*/
//    n = 0 ;
//    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
//    XtSetArg( args[n], XmNleftOffset,       5              ) ; n++ ;
//    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
//    XtSetArg( args[n], XmNrightOffset,      5              ) ; n++ ;
//    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
//    XtSetArg( args[n], XmNtopWidget,        row_widget      ) ; n++ ;
//    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
//    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    ) ; n++ ;
//    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, hbase_form, args, n ) ;

    /*------------------------------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        hbase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       5               ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      5               ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    gbase_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, ebase_form, args, n);

    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, gbase_form, args, n ) ;

    text = XmStringCreateSimple ("Aggression Margin");
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg ( args[n], XmNtopWidget,       label_widget      ); n++;
    XtSetArg ( args[n], XmNtopOffset,       2                 ); n++;
    XtSetArg ( args[n], XmNbottomAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNbottomOffset,      10   ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNleftOffset,      80   ); n++;
    XtSetArg ( args[n], XmNlabelString,     text            ); n++;
    XtSetArg ( args[n], XmNheight,          30              ); n++;
    label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, gbase_form, args, n);
    XmStringFree (text);

    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg ( args[n], XmNtopWidget,       label_widget   ); n++;
    XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg ( args[n], XmNbottomWidget,    label_widget   ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET     ); n++;
    XtSetArg ( args[n], XmNleftWidget,      label_widget        ); n++;
    XtSetArg ( args[n], XmNleftOffset,      10        ); n++;
//    XtSetArg ( args[n], XmNheight,          30                  ); n++;
    XtSetArg ( args[n], XmNwidth,           45                  ); n++;
    CTK_MARGIN = XtCreateManagedWidget ("XtasUserField", xmTextWidgetClass, gbase_form, args, n);
    sprintf (buf, "%ld", XtasStbParam->ctk_margin);
    XmTextSetString (CTK_MARGIN, buf);
    
    text = XmStringCreateSimple ("ps");
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg ( args[n], XmNtopWidget,       CTK_MARGIN   ); n++;
    XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET   ); n++;
    XtSetArg ( args[n], XmNbottomWidget,    CTK_MARGIN   ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET ); n++;
    XtSetArg ( args[n], XmNleftWidget,      CTK_MARGIN      ); n++;
    XtSetArg ( args[n], XmNleftOffset,      10 ); n++;
    XtSetArg ( args[n], XmNlabelString,     text            ); n++;
//    XtSetArg ( args[n], XmNheight,          30              ); n++;
    label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, gbase_form, args, n);
    XmStringFree (text);

 /*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       hbase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        10              ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,     gbase_form      ) ; n++ ;
  //  XtSetArg( args[n], XmNbottomOffset,     5               ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n ) ;
    
 /*--------------------------------- Slope for Noise ----------------------------------------*/
    
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        frame_widget      ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       gbase_form        ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET     ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,     gbase_form     ) ; n++ ;
    hbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple( "Slope For Noise" ) ;
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNlabelString,     text); n++;
    label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, hbase_form, args, n);
    XmStringFree (text);

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, hbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       label_widget,   
                                           XmNtopOffset,       5,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      40,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;

    text = XmStringCreateSimple( "Nominal" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_slope_noise == RCX_SLOPE_NOMINAL) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkSlopeNoiseCallback, (XtPointer)RCX_SLOPE_NOMINAL) ;

    text = XmStringCreateSimple( "CTK" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_slope_noise == RCX_SLOPE_CTK) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkSlopeNoiseCallback, (XtPointer)RCX_SLOPE_CTK) ;

    text = XmStringCreateSimple( "Real" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_slope_noise == RCX_SLOPE_REAL) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkSlopeNoiseCallback, (XtPointer)RCX_SLOPE_REAL) ;
    
    
/*--------------------------------- horizontal separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       5              ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      5              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        row_widget      ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    ) ; n++ ;
    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, hbase_form, args, n ) ;
    
/*--------------------------------- Slope for Delays ----------------------------------------*/
    text = XmStringCreateSimple( "Slope For Delays" ) ;
    n = 0;
    XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg ( args[n], XmNtopWidget,       gbase_form ); n++;
    XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
    XtSetArg ( args[n], XmNlabelString,     text); n++;
    label_widget = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, hbase_form, args, n);
    XmStringFree (text);

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, hbase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       label_widget,   
                                           XmNtopOffset,       5,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      40,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;

    text = XmStringCreateSimple( "CTK" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_slope_delay == RCX_SLOPE_CTK) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkSlopeDelayCallback, (XtPointer)RCX_SLOPE_CTK) ;

    text = XmStringCreateSimple( "Enhanced" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->rcx_ctk_slope_delay == RCX_SLOPE_DELAY_ENHANCED) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisCtkSlopeDelayCallback, (XtPointer)RCX_SLOPE_DELAY_ENHANCED) ;
    
/*-------------------------------  1 ere sub form -------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        ANALYSIS_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n ) ;

    form_t = dbase_form ;
    n = 0 ; 
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;
    
/*------------------------------------------- Stability Type -------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ) ; n++ ;
    XtSetArg( args[n], XmNrightPosition,    20                ) ; n++ ; 
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;

    text = XmStringCreateSimple( "Analysis Type" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,   2    ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       frame_widget,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      20,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;

    text = XmStringCreateSimple( "Best Case" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_BEST) == STB_STABILITY_BEST) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisBestCallback, NULL) ;

    text = XmStringCreateSimple( "Worst Case" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_WORST) == STB_STABILITY_WORST) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisWorstCallback, NULL ) ;
/*--------------------------------- horizontal separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       15              ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNrightOffset,      15              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        row_widget      ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5               ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    ) ; n++ ;
    gbase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n ) ;


    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                                           XmNtopAttachment,   XmATTACH_WIDGET,
                                           XmNtopWidget,       gbase_form,   
                                           XmNleftAttachment,  XmATTACH_FORM,
                                           XmNleftOffset,      20,
                                           XmNrightAttachment,  XmATTACH_FORM,
                                           XmNbottomAttachment,  XmATTACH_FORM,
                                           XmNpacking,         XmPACK_COLUMN,
                                           XmNnumColumns,      1,
                                           XmNorientation,     XmVERTICAL,
                                           XmNisAligned,       True,
                                           XmNradioBehavior,   True,
                                           XmNradioAlwaysOne,  True,
                                           NULL) ;

    text = XmStringCreateSimple( "Mono-Interval" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->analysis_var == STB_GLOB_ANALYSIS) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasStbGlobAnalysisCallback, NULL) ;

    text = XmStringCreateSimple( "Multi-Interval" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->analysis_var == STB_DET_ANALYSIS) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasStbDetAnalysisCallback, NULL ) ;
/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    25   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n ) ;

 /*------------------------------------------ Monophase Latch -------------------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;

    text = XmStringCreateSimple( "Monophase Latch" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            160 ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
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
                                           NULL) ;
    //XmAddTabGroup( row_widget ) ;

    text = XmStringCreateSimple( "Flip Flop" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_FF) == STB_STABILITY_FF) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisMonophaseFfCallback, NULL ) ;

    text = XmStringCreateSimple( "Transparent" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_LT) == STB_STABILITY_LT) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisMonophaseLtCallback, NULL ) ;
    text = XmStringCreateSimple( "Error" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_ER) == STB_STABILITY_ER) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisMonophaseErCallback, NULL ) ;


/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    25   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n ) ;

 /*------------------------------------------ level -------------------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;

    text = XmStringCreateSimple( "Level" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            120 ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
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
                                           NULL) ;
    //XmAddTabGroup( row_widget ) ;

    text = XmStringCreateSimple( "All Levels" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_LAST) == STB_STABILITY_LAST) ? False:True,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisAllLevelCallback, NULL ) ;

    text = XmStringCreateSimple( "Top Level" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,          ((XtasStbParam->mode_var & STB_STABILITY_LAST) == STB_STABILITY_LAST) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisTopLevelCallback, NULL ) ;


/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    25   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n ) ;

 /*------------------------------------------ Contraintes -------------------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;

    text = XmStringCreateSimple( "Error Type" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            118 ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
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
                                           XmNradioBehavior,   False,
                                           XmNradioAlwaysOne,  False,
                                           NULL) ;
    //XmAddTabGroup( row_widget ) ;

    text = XmStringCreateSimple( "Setup" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_SETUP) == STB_STABILITY_SETUP) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisSetupCallback, NULL ) ;

    text = XmStringCreateSimple( "Hold" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           ((XtasStbParam->mode_var & STB_STABILITY_HOLD) == STB_STABILITY_HOLD) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasAnalysisHoldCallback, NULL ) ;

/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    25   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n ) ;

 /*------------------------------------------ Report -------------------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;

    text = XmStringCreateSimple( "Error Reports" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            128 ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
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
                                           XmNradioBehavior,   False,
                                           XmNradioAlwaysOne,  False,
                                           NULL) ;
    //XmAddTabGroup( row_widget ) ;

    text = XmStringCreateSimple( "sto" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           (XtasStbParam->out == 'Y') ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDriveStoCallback, NULL ) ;

    text = XmStringCreateSimple( "str" ) ;
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmN_OF_MANY,
                                      XmNset,           (XtasStbParam->report == 'Y') ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasDriveStrCallback, NULL ) ;


/*--------------------------------- vertical separator ----------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form      ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,    25   ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset, 5   ) ; n++ ;
    XtSetArg( args[n], XmNorientation,      XmVERTICAL      ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, dbase_form, args, n ) ;

 /*------------------------------------------ File Type -------------------------------------------*/ 
    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       ebase_form        ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ) ; n++ ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ;

    text = XmStringCreateSimple( "File Type" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
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
                                           NULL) ;
    //XmAddTabGroup( row_widget ) ;

    text = XmStringCreateSimple( "ttx" ) ;
    FILE_TTX_TOGGLE = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->graph_var == STB_RED_GRAPH) ? True:False,
                                      XmNsensitive,     ((STB_CTK_VAR & STB_CTK) == STB_CTK) ? False:True,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( FILE_TTX_TOGGLE, XmNvalueChangedCallback, XtasAnalysisTtxCallback, NULL ) ;

    text = XmStringCreateSimple( "dtx" ) ;
    FILE_DTX_TOGGLE = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                                      XmNindicatorType, XmONE_OF_MANY,
                                      XmNset,           (XtasStbParam->graph_var == STB_DET_GRAPH) ? True:False,
                                      XmNlabelString,   text,
                                      NULL) ;
    XmStringFree( text ) ;
    XtAddCallback( FILE_DTX_TOGGLE, XmNvalueChangedCallback, XtasAnalysisDtxCallback, NULL ) ;

    /*------------------------------------------------------------------------------------------*/
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        form_t            ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        5                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_POSITION ) ; n++ ;
    XtSetArg( args[n], XmNrightPosition,    70                ) ; n++ ;
//    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n ) ;
    form_t = dbase_form ;
    n = 0 ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n ) ; 
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,   2                     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple( "Error Report" ) ;
    n = 0  ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,         frame_widget      ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ) ; n++ ;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple( "Error Margin" ) ;
    n = 0 ;
    XtSetArg( args[n], XmNlabelString,      text              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,     False              ) ; n++ ;
    XtSetArg( args[n], XmNtopOffset,        7                 ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       30     ) ; n++ ;
    XtSetArg( args[n], XmNheight,           35                ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            120                ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n ) ;
    XmStringFree( text ) ;

    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,             label_widget     ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,          label_widget     ) ; n++ ;
    XtSetArg( args[n], XmNbottomOffset,          2     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            90               ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,       2                 ) ; n++ ;

    STBMARGIN = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n) ;
    //XmAddTabGroup( STBMARGIN ) ;
    XmTextSetString( STBMARGIN, "0" ) ;

    text = XmStringCreateSimple( "Number of Error" ) ;
    n = 0 ;

    XtSetArg( args[n], XmNlabelString,      text              ) ; n++ ;
    XtSetArg( args[n], XmNrecomputeSize,     False              ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_OPPOSITE_WIDGET     ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,             STBMARGIN     ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,          STBMARGIN     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,            STBMARGIN      ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,            50      ) ; n++ ;
//    XtSetArg( args[n], XmNheight,           30                ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            120               ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n ) ;
    XmStringFree( text ) ;


    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,      XmATTACH_OPPOSITE_WIDGET  ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,          label_widget  ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment,   XmATTACH_OPPOSITE_WIDGET ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,       label_widget     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ) ; n++ ;
    XtSetArg( args[n], XmNwidth,            90               ) ; n++ ;
    XtSetArg( args[n], XmNshadowThickness,       2                 ) ; n++ ;

    NBERROR = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args, n) ;
    //XmAddTabGroup( NBERROR ) ;
    XmTextSetString( NBERROR ,"100" ) ;
/****---------****/
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,        form_t            ) ; n++ ;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNbottomWidget,     form_t            ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNleftWidget,       form_t            ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,       5                 ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    INFO_FORM = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n ) ;
    form_t = INFO_FORM ;
    n = 0 ;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, INFO_FORM, args, n ) ; 
    n = 0 ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNleftOffset,   2                     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ) ; n++ ;
    frame_widget = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple( "Information File" ) ;
    n = 0  ;
    XtSetArg( args[n], XmNlabelString,      text ) ; n++ ;
    label_widget = XtCreateManagedWidget( "XtasMainTitles", xmLabelWidgetClass, frame_widget, args, n ) ;
    XmStringFree( text ) ;

    n = 0 ;
    XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ) ; n++ ;
    XtSetArg( args[n], XmNtopWidget,         frame_widget      ) ; n++ ;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ) ; n++ ;
    XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ) ; n++ ;
    fbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n ) ;

    text = XmStringCreateSimple("Choose the file to use:");
    n = 0;
    XtSetArg( args[n], XmNlabelString,      text              ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
    XtSetArg( args[n], XmNheight,           35                ); n++;
    XtSetArg( args[n], XmNwidth,            180                ); n++;
    label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, fbase_form, args, n );
    XmStringFree( text );
    
/*    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNtopOffset,             2                 ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         90               ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
    XtSetArg( args[n], XmNleftOffset,            30                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, fbase_form, args,n);
*/    
    XtVaGetValues(fbase_form, XmNforeground, &fg, XmNbackground, &bg, NULL);
    pixmap = XalGetPixmap(fbase_form, 
                         XTAS_OPEN_MAP, fg, bg);
    
    button = XtVaCreateManagedWidget("dbutton", xmDrawnButtonWidgetClass, fbase_form,
                                     XmNlabelType,         XmPIXMAP,
                                     XmNlabelPixmap,       pixmap,
                                     XmNwidth,             40,
                               //      XmNheight,            60,
                                     XmNpushButtonEnabled, True,
                                     XmNsensitive, True,
      		                         XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
      		                         XmNtopWidget,	       label_widget,
      		                         XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
      		                         XmNbottomWidget,      label_widget,
                                     XmNbottomOffset,      2,
                                     XmNshadowType,        XmSHADOW_ETCHED_OUT,
                                     XmNleftAttachment,    XmATTACH_WIDGET,
                                     XmNleftWidget,        label_widget,
                                     XmNshadowThickness,   2,
                                     NULL);
    
    XtAddCallback(button, XmNactivateCallback, XtasSelectInfFileCallback, (XtPointer)tmp_widget);

    if((stoin = mbkfopen(XtasMainParam->ttvfig->INFO->FIGNAME,"sto",READ_TEXT))) {
        fclose(stoin);
        XtVaSetValues (STO_TOGGLE, XmNset, True, NULL);
        XtSetSensitive (STO_TOGGLE, True);
        XtVaSetValues (CTK_TOGGLE, XmNset, False, NULL);
        XtSetSensitive (CTK_FORM, False);
        XtSetSensitive (ANALYSIS_FORM, False);
        XtSetSensitive (INFO_FORM, False);
        XTAS_STB_PARSE_STO = 'Y';
        XtSetSensitive(FILE_TTX_TOGGLE, True);
    }
    else {
        XtVaSetValues (STO_TOGGLE, XmNset, False, NULL);
        XtSetSensitive (STO_TOGGLE, False);
        XtSetSensitive (ANALYSIS_FORM, True);
        XtSetSensitive (INFO_FORM, True);
        XTAS_STB_PARSE_STO = 'N';
    }
    
   }

  XtManageChild( XtasStbAnalysisWidget ) ;
/*  XalLimitedLoop(XtasStbAnalysisWidget); */
}
