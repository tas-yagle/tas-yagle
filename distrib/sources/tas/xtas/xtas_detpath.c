/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_detpath.c                                              */
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
#include "xtas.h"
#include "xtas_detpath_menu.h"
#include "xtas_detpath_button.h"

#define  XTAS_DETAIL_FROM_LOFFSET_G   8
#define  XTAS_DETAIL_TP_LOFFSET_T   6

extern      char XTAS_SIMU_RES;
/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/
static char                    *tas_current_signal = NULL ;
static XtasDetailPathSetStruct *xtas_current_detail = NULL ;
chain_list                     *XTAS_DETPATH_SESSION = NULL ;
//static char                     DisableInfo ;
XtasWindowStruct *XtasDetailwidtab = NULL;
Widget      XTAS_DETPATHSELECTEDWIDGET = NULL;

extern char                     STBCTK;
/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailColumnsTitleMoveWithHSBCallback                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasDetailColumnsTitleMoveWithHSBCallback (hsb, label, call_data)
    Widget      hsb;
    XtPointer   label;
    XtPointer   call_data;
{
    int         loffset;
    int         value;

    if (XTAS_DISPLAY_NEW == 0)
        loffset = XTAS_DETAIL_FROM_LOFFSET_G;
    else 
        loffset = XTAS_DETAIL_TP_LOFFSET_T;
        
    XtVaGetValues (hsb, XmNvalue, &value, NULL);

    XtVaSetValues (label, XmNleftOffset, (loffset - value), NULL);
}

/*---------------------------------------------------------------------------*/
void XtasRemoveDetailPathSession(window)
    XtasWindowStruct *window ;
{
    chain_list *head = XTAS_DETPATH_SESSION;
    XtasWindowStruct *data;
    XtasDetailPathSetStruct *detail_set;
    chain_list *chain ;

    if(head != NULL)
    {
        for(; head != NULL; head = head->NEXT )
        {
            data = head->DATA;
            if((window == NULL) || (window == data))
            {
                detail_set = data->userdata;
                if(detail_set->PARENT != NULL)
                {
                    for(chain = detail_set->PARENT->DETAIL_SESSION ; chain != NULL;
                            chain = chain->NEXT )
                    {
                        if(chain->DATA == window) {
                            chain->DATA = NULL ;
                            XtasDestroySimuParams ();
                        }
                    }
                }
                XtasFreeDetailPathSetStruct(detail_set);
                XtDestroyWidget( data->topwidget );
                mbkfree(data);
                if(window == data)
                    break ;
            }
        }
        if(window == NULL)
        {
            freechain( XTAS_DETPATH_SESSION );
            XTAS_DETPATH_SESSION = NULL;
        }
        else if(head != NULL)
            XTAS_DETPATH_SESSION = delchain(XTAS_DETPATH_SESSION,head) ;
        if (XtasVisualPathWidget) {
            XtDestroyWidget (XtasVisualPathWidget);
            XtasVisualPathWidget = NULL;
        }
        if (XtasSimuParamWidget) {
            XtDestroyWidget (XtasSimuParamWidget);
            XtasSimuParamWidget = NULL;
        }            
    }
    XTAS_DETPATHSELECTEDWIDGET = NULL;
}

/*---------------------------------------------------------------------------*/
void XtasSaveDetail(file,detailset,infos)
    FILE *file ;
    XtasDetailPathSetStruct *detailset ;
    long infos;
{
    chain_list *head = (chain_list *)detailset->CUR_DETAIL->DATA ;
    ttvcritic_list * critic ;
    ttvcritic_list *auxcritic ;
    char type  ;
    long delay ;
    long otherdelay ;
    char slope1 ;
    char slope2 ;
    char *name1 ;
    char *name2 ;
    char *typedelay ;
    long slope ;
    long otherslope ;
    long cumul = 0 ;
    long othercumul = 0 ;
    int sizetype ;
    int sizedelay ;
    int sizeotherdelay ;
    int sizeslope ;
    int sizeotherslope ;
    int sizecumul ;
    int sizeothercumul ;
    int sizename1 ;
    int sizename2 ;
    int i ;

    if(detailset->PARENT != NULL)
    {
        if(detailset->PARENT->PARAM->CRITIC == 'Y')
            type = 'm' ;
        else if(detailset->PARENT->PARAM->CRITIC == 'P')
            type = 'p' ;
        else
            type = 'd' ;
    }
    else
        type = 'l' ;

    XtasCalcSize(XtasMainParam->ttvfig,
            &sizetype,&sizedelay,&sizeotherdelay,&sizecumul,&sizeothercumul,&sizeslope,&sizeotherslope,
            &sizename1,&sizename2,type,head,&delay,&otherdelay,infos,
            detailset->CUR_DETAIL->SIZE) ;

    XtasPrintFirstEnd(file,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,
            sizename1,sizename2,infos) ;
    XtasPrintLinesepar(file,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,
            sizename1,sizename2) ;

    i = 1 ;

    while(head != NULL)
    {
        critic = (ttvcritic_list *)head->DATA ;
        while(critic->NEXT != NULL)
        {
            auxcritic = critic->NEXT ;
            if((auxcritic->TYPE & TTV_FIND_RC) == TTV_FIND_RC)
                typedelay = "# RC" ;
            else
                typedelay = "# GT" ;
            if(critic->SNODE == TTV_UP)
                slope1 = 'U' ;
            else
                slope1 = 'D' ;
            name1 = critic->NAME ;
            if(((auxcritic->TYPE & TTV_FIND_HZ) == TTV_FIND_HZ) &&
                    (auxcritic->NEXT == NULL))
                slope2 = 'Z' ;
            else if(auxcritic->SNODE == TTV_UP)
                slope2 = 'U' ;
            else
                slope2 = 'D' ;
            name2 = auxcritic->NAME ;
            if (infos == XTAS_SIMU) {
                slope = auxcritic->REFSLOPE ;
                delay = auxcritic->REFDELAY+critic->DATADELAY;
                cumul += auxcritic->REFDELAY+critic->DATADELAY ;
                otherslope = auxcritic->SIMSLOPE ;
                otherdelay = auxcritic->SIMDELAY ;
                othercumul += auxcritic->SIMDELAY ;
            }
            else if (infos == XTAS_CTK) {
                slope = auxcritic->REFSLOPE ;
                delay = auxcritic->REFDELAY+critic->DATADELAY ;
                cumul += auxcritic->REFDELAY+critic->DATADELAY ;
                otherslope = auxcritic->SLOPE ;
                otherdelay = auxcritic->DELAY+critic->DATADELAY ;
                othercumul += auxcritic->DELAY+critic->DATADELAY ;
            }
            else {
                slope = auxcritic->SLOPE ;
                delay = auxcritic->DELAY+critic->DATADELAY ;
                cumul += auxcritic->DELAY+critic->DATADELAY ;
                otherslope = 0;
                othercumul = 0;
                otherdelay = 0;
            }
            XtasPrintLine(file,typedelay,sizetype,delay,sizedelay,otherdelay,sizeotherdelay,cumul,sizecumul,othercumul,sizeothercumul,
                    slope,sizeslope,otherslope,sizeotherslope,name1,sizename1,slope1,slope2,
                    name2,sizename2) ;
            critic = critic->NEXT ;
        }
        i++ ;
        if(i > detailset->CUR_DETAIL->SIZE)
            break ;
        head = head->NEXT ;
    }

    XtasPrintLinesepar(file,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,
            sizename1,sizename2) ;
    XtasPrintFirstEnd(file,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,
            sizename1,sizename2,infos) ;
    fprintf(file,"\n\n") ;
}

/*---------------------------------------------------------------------------*/

void XtasDetailSaveCallback( widget, client_data, call_data )
    Widget  widget;
    XtPointer  client_data;
    XtPointer call_data;
{
    XtasDetailwidtab = (XtasWindowStruct *)client_data ;

    XtasSelectFileCallback( widget, (XtPointer)XTAS_DETA_MAP, call_data ) ;
}

void XtasDetailSaveSelectFileOk (void)
{
    FILE *file;

    if(XtasDataFileName != NULL)
    {
        file = fopen(XtasDataFileName,"a") ;
        if(file != NULL)
        {
            XtasSaveDetail(file,(XtasDetailPathSetStruct *)XtasDetailwidtab->userdata,XtasDetailwidtab->userinfos) ;
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
/* FUNCTION : XtasDetailCloseCallback                                        */
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
void XtasDetailCloseCallback( widget, client_data , call_data )
    Widget widget;
    XtPointer client_data ;
    XtPointer call_data;
{
    XtasWindowStruct *top = (XtasWindowStruct *)client_data ;

    XtasRemoveDetailPathSession(top) ;
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailCloseCallback                                        */
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
//void XtasDetailSimuResCloseCallback( widget, client_data , call_data )
//    Widget widget;
//    XtPointer client_data ;
//    XtPointer call_data;
//{
//    XtasWindowStruct *top = (XtasWindowStruct *)client_data ;
////    XtDestroyWidget (top->topwidget);
//    XtUnmanageChild (top->topwidget);
//}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailButtonsCallback                                      */
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
void XtasDetailButtonsCallback( widget, client_data, call_data )
    Widget           widget;
    XtPointer client_data;
    XtPointer call_data;
{
    XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;
    if (((XtasDetailPathSetStruct*)((XtasWindowStruct*)client_data)->userdata)->PARENT == NULL)
        XtasOptionsButtonsTreat( widtab->wrkwidget, &widtab->butwidget,
                XtasDetailButtonMenusDelay,  XtasDetailButtonOptionsDelay );
    else if (XTAS_SIMU_RES == 'Y')
        XtasOptionsButtonsTreat( widtab->wrkwidget, &widtab->butwidget,
                XtasDetailButtonMenusSimu,  XtasDetailButtonOptionsSimu );
    else
        XtasOptionsButtonsTreat( widtab->wrkwidget, &widtab->butwidget,
                XtasDetailButtonMenus,  XtasDetailButtonOptions );
}

/*---------------------------------------------------------------------------*/
void XtasSetDetailInfo(detailset,critic)
    XtasDetailPathSetStruct *detailset ;
    ttvcritic_list * critic ;
{
    long delay = 0, refdelay = 0;
    char value[1024] ;

    if((critic != NULL) && (detailset != NULL))
    {
        if ( critic->SNODE == TTV_UP )
            XtasSetLabelString( detailset->NODE_RF, "(Rise)");
        else    
            XtasSetLabelString( detailset->NODE_RF, "(Fall)");
        XmTextSetString( detailset->NODE_NAME, critic->NAME );

        while( critic->NEXT != NULL )
        {
            delay += critic->DELAY+critic->DATADELAY;
            refdelay += critic->REFDELAY+critic->DATADELAY;
            critic = critic->NEXT;
        }
        delay += critic->DELAY+critic->DATADELAY;
        if ( critic->SNODE == TTV_UP )
            XtasSetLabelString( detailset->ROOT_RF, "(Rise)");
        else
            XtasSetLabelString( detailset->ROOT_RF, "(Fall)");
        XmTextSetString( detailset->ROOT_NAME, critic->NAME);
        sprintf( value, "Total Delay : %.1f pS", delay/TTV_UNIT );
        XtasSetLabelString( detailset->TOTAL_DELAY, value );
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSelectDelayCallback                                        */
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
void XtasSelectDelayCallback( widget, client_data, call_data )
    Widget  widget;
    XtPointer client_data;
    XtPointer call_data;
{
    if(xtas_current_detail != NULL)
        XtasSetDetailInfo(xtas_current_detail,
                (ttvcritic_list *)((chain_list *)client_data)->DATA) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailFocusCallback                                        */
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
void XtasDetailFocusCallback( widget, client_data, call_data )
    Widget  widget;
    XtPointer client_data;
    XtPointer call_data;
{
    if(tas_current_signal != NULL)
    {
        XtFree( tas_current_signal) ;
    }
    tas_current_signal = NULL ;
    xtas_current_detail = (XtasDetailPathSetStruct *)client_data ;
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasPathsDeselect                                              */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDetPathDeselect( widget )
Widget widget;
{
    Pixel   fg, bg;
    Widget  parent;

    parent = XtParent (widget);
    XtVaGetValues(parent, XmNbackground,&bg,NULL );
    XtVaSetValues(widget, XmNcursorPositionVisible, False, XmNborderColor, bg, NULL) ;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailFocusNameCallback                                    */
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
void XtasDetailFocusNameCallback( widget, client_data, call_data )
    Widget  widget;
    XtPointer client_data;
    XtPointer call_data;
{
    Pixel   fg, bg;

    if(tas_current_signal != NULL)
    {
        XtFree( tas_current_signal) ;
    }

    if (XTAS_DETPATHSELECTEDWIDGET != NULL) 
        XtasDetPathDeselect (XTAS_DETPATHSELECTEDWIDGET);
    
    XTAS_DETPATHSELECTEDWIDGET = widget;
    XtVaGetValues(XtParent(XTAS_DETPATHSELECTEDWIDGET), XmNforeground, &fg,NULL );
    XtVaSetValues( XTAS_DETPATHSELECTEDWIDGET, XmNcursorPositionVisible, True, XmNborderColor, fg, NULL );
    tas_current_signal = XmTextGetString( XTAS_DETPATHSELECTEDWIDGET );
    xtas_current_detail = (XtasDetailPathSetStruct *)client_data ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasLosingDetailFocusCallback                                  */
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
void XtasLosingDetailFocusCallback( widget, signal, call_data )
    Widget  widget;
    XtPointer signal;
    XtPointer call_data;
{ 
    XtVaSetValues( widget, XmNcursorPositionVisible, False ,NULL );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailDetailCallback                                       */
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
void XtasDetailDetailCallback( widget, client_data, call_data )
    Widget widget;
    XtPointer client_data;
    XtPointer call_data;
{
    XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;

    if( tas_current_signal != NULL )
    {
        XalSetCursor( widtab->topwidget, WAIT ); 
        XalForceUpdate(  widtab->topwidget );
        XtasSignalsDetail( widtab->wrkwidget, tas_current_signal );
        XalSetCursor( widtab->topwidget, NORMAL ); 
    }
    else
    {
        XalDrawMessage( widtab->errwidget, XTAS_NSIGERR );
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCtkInfoCallback                                            */
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
void XtasCtkInfoCallback( widget, client_data, call_data )
    Widget widget;
    XtPointer client_data;
    XtPointer call_data;
{   
    chain_list      *head;
    ttvcritic_list  *critic;
    ttvsig_list     *ptsig;
    Widget           info_user;
    XtasWindowStruct *widtab = (XtasWindowStruct *)client_data ;

    info_user = ((XtasDetailPathSetStruct *)((XtasWindowStruct *)client_data)->userdata)->INFO_USER;
    if( tas_current_signal && xtas_current_detail )
    {
        if((XTAS_CTX_LOADED & XTAS_FROM_ANALYSIS) != XTAS_FROM_ANALYSIS)
            XalDrawMessage( widtab->errwidget, 
                    "Crosstalk analysis must have been run from XTas before");
        else if(XtasMainParam->stbfig == NULL)
            XalDrawMessage( widtab->errwidget, 
                    "Crosstalk Informations display impossible :\nno stbfig in memory.");
        else {        
            XalSetCursor( widtab->topwidget, WAIT ); 
            XtasSetLabelString( info_user, "Looking for Crosstalk Informations ...");
            XalForceUpdate(  widtab->topwidget );
            head = xtas_current_detail->CUR_DETAIL->DATA;
            critic = head->DATA;
            while(strcmp(critic->NAME,tas_current_signal))
                critic = critic->NEXT;
            ptsig = ttv_getsig (XtasMainParam->ttvfig, tas_current_signal);
            if(ptsig) {
                if(critic->SNODE == TTV_UP)
                    XtasDisplayCtkInfo( widtab->wrkwidget, &ptsig->NODE[1], widtab->errwidget);
                else
                    XtasDisplayCtkInfo( widtab->wrkwidget, &ptsig->NODE[0], widtab->errwidget);
            }
            XalSetCursor( widtab->topwidget, NORMAL ); 
            XtasSetLabelString( info_user, " ");
            XalForceUpdate(  widtab->topwidget );
        }
    }
    else
    {
        XalDrawMessage( widtab->errwidget, XTAS_NSIGERR );
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNextDetailCallback( widget, client_data  , call_data )
    Widget                  widget;
    XtPointer               client_data ;
    XtPointer               call_data;
{
    XtasDetailPathSetStruct *detail_set = (XtasDetailPathSetStruct *)client_data ;

    if(detail_set->CUR_DETAIL->NEXT != NULL)
    {
        detail_set->CUR_DETAIL = detail_set->CUR_DETAIL->NEXT ;
        if (XTAS_DISPLAY_NEW)
            XtasFillDelayList ( detail_set ) ;
        else
            XtasFillDetailPathList(detail_set) ;
        XtasSetDetailInfo(detail_set,(ttvcritic_list *)((chain_list *)detail_set->CUR_DETAIL->DATA)->DATA) ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasPrevDetailCallback( widget, client_data  , call_data )
    Widget                  widget;
    XtPointer               client_data ;
    XtPointer               call_data;
{  
    XtasDetailPathSetStruct *detail_set = (XtasDetailPathSetStruct *)client_data ;

    if(detail_set->CUR_DETAIL->PREV != NULL) 
    {
        detail_set->CUR_DETAIL = detail_set->CUR_DETAIL->PREV ;
        if (XTAS_DISPLAY_NEW)
            XtasFillDelayList ( detail_set ) ;
        else
            XtasFillDetailPathList(detail_set) ;
        XtasSetDetailInfo(detail_set,(ttvcritic_list *)((chain_list *)detail_set->CUR_DETAIL->DATA)->DATA) ;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetSigNameCallback                                         */
/*                                                                           */
/* OBJECT   : Get current signame if click on the scrolled list              */
/*            Display info at the bottom                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void XtasGetSigNameCallback(w, client_data, call_data)
    Widget     w;
    XtPointer  client_data;
    XtPointer  call_data;
{
    XmListCallbackStruct    *cbs ;
    chain_list              *head;
    ttvcritic_list          *critic;
    XtasDetailPathSetStruct *current_detail = NULL ;
    char                     buf[1024] ;
    int                      i, size ;
    Widget                   info_form ;
    int                      n ;
    Arg                      args[20] ;
    static Widget            last_info_widget ;
    static XtasDetailPathSetStruct* last_detail ;

    cbs = (XmListCallbackStruct*)call_data ;
    current_detail =  (XtasDetailPathSetStruct *)client_data ;
    head = current_detail->CUR_DETAIL->DATA;
    /* update "From" and "To" field in "getdelay" window */
    xtas_current_detail = current_detail ; 
    XTAS_DISABLE_INFO = 'N' ; /* active le bouton info */

    size = 0 ;
    i    = 1 ;

    while((head != NULL) && (size++ < current_detail->CUR_DETAIL->SIZE))
    {
        critic = head->DATA;
        while (critic && (cbs) && (i < cbs->item_position)) {
            critic = critic->NEXT ;
            i++ ;
        }
        if((i >= cbs->item_position) && critic){ /* if signal founded in list */
            break ;
        }
        else
            head = head->NEXT ;
    }

    if((tas_current_signal != NULL) && ( last_detail == current_detail )){
        mbkfree( tas_current_signal) ;
        tas_current_signal = NULL ;
    }

    sprintf(buf, "%s", critic->NAME) ;
    tas_current_signal =  strdup ( buf ) ;
    if ( tas_current_signal && current_detail->INFO_PAGE ) 
    {
        if (( last_info_widget ) && ( last_detail == current_detail ))
            XtDestroyWidget ( last_info_widget ) ;
        n=0;
        XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNscrollingPolicy,        XmSTATIC             ); n++;
        info_form   = XtCreateManagedWidget( "XtasSubForm",
                xmFormWidgetClass,
                current_detail->INFO_PAGE,
                args, n );
        last_info_widget = info_form ;
        XtasFillInfosForm( info_form, tas_current_signal ) ;
        last_detail = current_detail ;
    }
    if ( head )
        XtasSelectDelayCallback( w, (XtPointer)head, call_data ) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* New display of the path details                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillDetailPathList2(detail_set, label_widget)
    XtasDetailPathSetStruct *detail_set;
    Widget                   label_widget;
{
    int     size;
    chain_list *head;
    ttvcritic_list *critic;
    long total = (long)0 ;
    long totalnew = (long)0 ;

    char       text_line[2048] ;
    char       *tp, *tpnew, *cartotal, *cartotalnew, *capa, *slope, *slopenew ;
    char       type_rc_gate[1024] ;
    char       type_front[1024] ;
    char       signame[1024] ;
    int        i ;
    XmString   text ;
    Widget     list_widget, tmp ;
    int        n ;
    Arg        args[20] ;


    n=0;
    XtSetArg( args[n], XmNtopAttachment,          XmATTACH_FORM        ); n++;
    XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
    XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
    XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
    list_widget = XmCreateScrolledList ( detail_set->SCROLL_DETAIL, "XtasTextualDisplay", args, n );
    XmListDeleteAllItems (list_widget) ; 
    XtManageChild( list_widget );
    head = detail_set->CUR_DETAIL->DATA;
    size = 0 ;

    i = 1 ;

    while((head != NULL) && (size++ < detail_set->CUR_DETAIL->SIZE))
    {
        critic = head->DATA;
        while ( critic != NULL )
        {
            if ((detail_set->PARENT != NULL)&&(XTAS_SIMU_RES == 'Y')) {
                totalnew += critic->SIMDELAY ;
                total += critic->REFDELAY+critic->DATADELAY ;
            }
            else if ((detail_set->PARENT != NULL)&&((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED)) {
                totalnew += critic->DELAY+critic->DATADELAY ;
                total += critic->REFDELAY+critic->DATADELAY ;
            }
            else
                total += critic->DELAY+critic->DATADELAY ;

            /*- type : RC or GATE -*/
            if(critic != head->DATA) {
                if((critic->TYPE & TTV_FIND_RC) != TTV_FIND_RC)
                    sprintf(type_rc_gate,"GT"); 
                else
                    sprintf(type_rc_gate,"RC");
            }
            else
                sprintf(type_rc_gate,"  ");

            /*- Capacite -*/
            capa = XtasPlaceFloatString(critic->CAPA/1000.0) ; /* pF */

            /*- Tp -*/
            if ((detail_set->PARENT != NULL)&&(XTAS_SIMU_RES == 'Y')) {
                tpnew = XtasPlaceNbStringTTVUnit(critic->SIMDELAY,TTV_UNIT) ;
                tp = XtasPlaceNbStringTTVUnit(critic->REFDELAY+critic->DATADELAY,TTV_UNIT) ;
            }
            else if ((detail_set->PARENT != NULL)&&((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED)) {
                tpnew = XtasPlaceNbStringTTVUnit(critic->DELAY+critic->DATADELAY,TTV_UNIT) ;
                tp = XtasPlaceNbStringTTVUnit(critic->REFDELAY+critic->DATADELAY,TTV_UNIT) ;
            }
            else
                tp = XtasPlaceNbStringTTVUnit(critic->DELAY+critic->DATADELAY,TTV_UNIT) ;

            /*- Tp total -*/
            if ((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                cartotalnew = XtasPlaceNbStringTTVUnit(totalnew,TTV_UNIT) ;
                cartotal = XtasPlaceNbStringTTVUnit(total,TTV_UNIT) ;
            }
            else
                cartotal = XtasPlaceNbStringTTVUnit(total,TTV_UNIT) ;


            /*- Slope -*/
            if ((detail_set->PARENT != NULL)&&(XTAS_SIMU_RES == 'Y')) {
                slopenew = XtasPlaceNbStringTTVUnit(critic->SIMSLOPE,TTV_UNIT) ;
                slope = XtasPlaceNbStringTTVUnit(critic->REFSLOPE,TTV_UNIT) ;
            }
            else if ((detail_set->PARENT != NULL)&&((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED)) {
                slopenew = XtasPlaceNbStringTTVUnit(critic->SLOPE,TTV_UNIT) ;
                slope = XtasPlaceNbStringTTVUnit(critic->REFSLOPE,TTV_UNIT) ;
            }
            else
                slope = XtasPlaceNbStringTTVUnit(critic->SLOPE,TTV_UNIT) ;

            /*- Type de front -*/
            if(critic->SNODE == TTV_UP)
                sprintf(type_front,"R ") ;
            else
                sprintf(type_front,"F ") ;

            /*- Type de front -*/
            sprintf(signame,"%s",critic->NAME );

            if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED ) || (XTAS_SIMU_RES == 'Y')))
                sprintf(text_line,"%s %s pS %s pS %s pS %s pS %s pS %s pS %s pF   %s  %s", 
                        type_rc_gate, tp, tpnew, cartotal, cartotalnew, slope, slopenew, capa, type_front, signame);
            else
                sprintf(text_line,"%s %s pS %s pS %s pS %s pF   %s  %s", 
                        type_rc_gate, tp, cartotal, slope, capa, type_front, signame);
            mbkfree(capa) ;
            mbkfree(tp) ;
            mbkfree(cartotal) ;
            mbkfree(slope) ;

            text = XmStringCreateSimple (text_line) ;
            XmListAddItemUnselected ( list_widget, text, i) ;
            XmStringFree (text) ;

            i++ ;
            critic = critic->NEXT;
        }
        head = head->NEXT ;
    }

    XtAddCallback (list_widget,
            XmNbrowseSelectionCallback,
            XtasGetSigNameCallback,
            (XtPointer)detail_set);

    XtAddCallback (list_widget,
            XmNdefaultActionCallback,
            XtasDetailDetailCallback,
            (XtPointer)detail_set);
    
    XtVaGetValues (list_widget, XmNhorizontalScrollBar, &tmp, NULL);
    XtAddCallback (tmp, XmNvalueChangedCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNdragCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNincrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNdecrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNpageIncrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNpageDecrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNtoTopCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
    XtAddCallback (tmp, XmNtoBottomCallback, XtasDetailColumnsTitleMoveWithHSBCallback, label_widget);
}

void XtasTestCallBack(widget, client_data, call_data)
    Widget widget;
    XtPointer client_data;
    XtPointer call_data;
{

    XalDrawMessage(XtasErrorWidget, "Attention");
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFillDetailPathList(detail_set)
    XtasDetailPathSetStruct *detail_set;
{
    int     n;
    int     size;
    XmString   text;
    Arg    args[20];
    char *value;
    Pixmap r_pixmap, f_pixmap, v_pixmap, gt_pixmap, rc_pixmap; 
    Widget  row_widget;
    Widget  label_widget;
    Widget  tmp_widget;
    chain_list *head;
    ttvcritic_list *critic;
    Pixel tas_state_color,tas_backg_color ;
    long total = (long)0 ;
    long totalnew = (long)0 ;


    if (detail_set->ROW_DETAIL != NULL ) {
        if(XTAS_SIMU_RES == 'N')
            XtDestroyWidget( detail_set->ROW_DETAIL );
    }

    row_widget = XtVaCreateWidget( "XtasDetailRow",xmRowColumnWidgetClass, detail_set->SCROLL_DETAIL,
            XmNorientation, XmVERTICAL,
            XmNspacing, 2,
            NULL);

    detail_set->ROW_DETAIL = row_widget;
    XtVaGetValues( row_widget, XmNforeground, &tas_state_color, XmNbackground, &tas_backg_color, NULL );
    r_pixmap = XalGetPixmap( row_widget, XTAS_REDG_MAP, tas_state_color, tas_backg_color );
    f_pixmap = XalGetPixmap( row_widget, XTAS_FEDG_MAP, tas_state_color, tas_backg_color );

    rc_pixmap = XalGetPixmap( row_widget, XTAS_RCLN_MAP, tas_state_color, tas_backg_color );
    gt_pixmap = XalGetPixmap( row_widget, XTAS_GATE_MAP, tas_state_color, tas_backg_color );

    value = (char *) mbkalloc ( 15*sizeof( char ) );

    head = detail_set->CUR_DETAIL->DATA;
    size = 0 ;
    while((head != NULL) && (size++ < detail_set->CUR_DETAIL->SIZE))
    {
        critic = head->DATA;
        if ((detail_set->PARENT != NULL)&&((XTAS_CTX_LOADED & XTAS_LOADED)== XTAS_LOADED)) {
            totalnew =  critic->DELAY;
            total = critic->REFDELAY;
        }
        else
            total = critic->DELAY;
        if (critic != NULL )
            while ( critic->NEXT != NULL )
            {
                Widget sub_form;

                if ((detail_set->PARENT != NULL)&&(XTAS_SIMU_RES == 'Y')) {
                    totalnew += critic->NEXT->SIMDELAY ;
                    total += critic->NEXT->REFDELAY+critic->DATADELAY ;
                }
                else if ((detail_set->PARENT != NULL)&&((XTAS_CTX_LOADED & XTAS_LOADED)== XTAS_LOADED)) {
                    totalnew += critic->NEXT->DELAY+critic->DATADELAY ;
                    total += critic->NEXT->REFDELAY+critic->DATADELAY ;
                }
    
                else
                    total += critic->NEXT->DELAY+critic->DATADELAY ;

                n = 0;
                sub_form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, row_widget, args, n );

                n = 0;
                XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ); n++;
                XtSetArg( args[n], XmNwidth,                 150               ); n++;
                XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                XtSetArg( args[n], XmNeditable,              False             ); n++;
                XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                XmTextSetString( tmp_widget, critic->NAME );

                XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusNameCallback, (XtPointer)detail_set );
                XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );
                //XmAddTabGroup( tmp_widget );

                if ( (r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP) )
                {
                    text = ( critic->SNODE == TTV_UP ) ? XmStringCreateSimple("rising") : XmStringCreateSimple("falling");
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNtopOffset,       2,
                            XmNlabelString,     text,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      tmp_widget,
                            XmNleftOffset,      7,
                            XmNwidth,           40,
                            XmNheight,          25,
                            NULL);
                    XmStringFree( text );
                }
                else

                {
                    v_pixmap = ( critic->SNODE == TTV_UP ) ? r_pixmap : f_pixmap;
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNtopOffset,       2,
                            XmNlabelType,       XmPIXMAP,
                            XmNlabelPixmap,     v_pixmap,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      tmp_widget,
                            XmNleftOffset,      7,
                            XmNwidth,           40,
                            XmNheight,          25,
                            NULL);
                }


                if ( (rc_pixmap == XmUNSPECIFIED_PIXMAP) || (gt_pixmap == XmUNSPECIFIED_PIXMAP) )
                {
                    if((critic->NEXT->TYPE & TTV_FIND_RC) != TTV_FIND_RC)
                        XmStringCreateSimple("GT"); 
                    else
                        XmStringCreateSimple("RC");
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNtopOffset,       2,
                            XmNlabelString,     text,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      label_widget,
                            XmNleftOffset,      7,
                            XmNwidth,           40,
                            XmNheight,          25,
                            NULL);
                    XmStringFree( text );
                }
                else

                {
                    if((critic->NEXT->TYPE & TTV_FIND_RC) != TTV_FIND_RC)
                        v_pixmap = gt_pixmap ;
                    else
                        v_pixmap = rc_pixmap ;
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                            //        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmDrawnButtonWidgetClass, sub_form,
                    XmNtopAttachment,   XmATTACH_FORM,
                    // XmNpushButtonEnabled, True,
                    XmNtopOffset,       2,
                    XmNlabelType,       XmPIXMAP,
                    XmNlabelPixmap,     v_pixmap,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNleftOffset,      7,
                    XmNwidth,           40,
                    XmNheight,          25,
                    NULL);
  //        XtAddCallback(label_widget, XmNactivateCallback, XtasTestCallBack, (XtPointer)NULL);
                  }


                  if ( (r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP) || 
                      ( ( ( critic->NEXT->TYPE & TTV_FIND_HZ ) == TTV_FIND_HZ ) && ( critic->NEXT->NEXT == NULL ) ))
                  {
                  if ( ( ( critic->NEXT->TYPE & TTV_FIND_HZ ) == TTV_FIND_HZ ) 
                      && ( critic->NEXT->NEXT == NULL ) )
                      text = XmStringCreateSimple("highZ");
                  else
                      text = ( critic->NEXT->SNODE == TTV_UP ) ? XmStringCreateSimple("rising") : XmStringCreateSimple("falling");


                  label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                          XmNtopAttachment,   XmATTACH_FORM,
                          XmNtopOffset,       2,
                          XmNlabelString,     text,
                          XmNleftAttachment,  XmATTACH_WIDGET,
                          XmNleftWidget,      label_widget,
                          XmNleftOffset,      9,
                          XmNwidth,           40,
                          XmNheight,          25,
                          NULL);
                  XmStringFree( text );
                  }
                  else
                  {

                      v_pixmap = ( critic->NEXT->SNODE == TTV_UP ) ? r_pixmap : f_pixmap;


                      label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                              XmNtopAttachment,   XmATTACH_FORM,
                              XmNtopOffset,       2,
                              XmNlabelType,       XmPIXMAP,
                              XmNlabelPixmap,     v_pixmap,
                              XmNleftAttachment,  XmATTACH_WIDGET,
                              XmNleftWidget,      label_widget,
                              XmNleftOffset,      9,
                              XmNwidth,           40,
                              XmNheight,          25,
                              NULL);
                  }

                  n = 0;
                  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                  XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
                  XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                  XtSetArg( args[n], XmNwidth,                 150               ); n++;
                  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                  XtSetArg( args[n], XmNeditable,              False             ); n++;
                  XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                  XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);
                  XmTextSetString( tmp_widget, critic->NEXT->NAME );

                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusNameCallback, (XtPointer)detail_set );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                  XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );
                  //XmAddTabGroup( tmp_widget );

                  n = 0;
                  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                  XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                  XtSetArg( args[n], XmNleftOffset,            10                ); n++;
                  XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                  XtSetArg( args[n], XmNwidth,                 100               ); n++;
                  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                  XtSetArg( args[n], XmNeditable,              False             ); n++;
                  XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                  XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                  sprintf( value, "%g pF", critic->NEXT->CAPA/1000.0 );
                  XmTextSetString( tmp_widget, value );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                  XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );

                  n = 0;
                  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                  XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                  XtSetArg( args[n], XmNleftOffset,            10                ); n++;
                  XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                  XtSetArg( args[n], XmNwidth,                 100               ); n++;
                  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                  XtSetArg( args[n], XmNeditable,              False             ); n++;
                  XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                  XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                  if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y')))
                      sprintf( value, "%.1f pS", critic->NEXT->REFSLOPE/TTV_UNIT );
                  else
                      sprintf( value, "%.1f pS", critic->NEXT->SLOPE/TTV_UNIT );

                  XmTextSetString( tmp_widget, value );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                  XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );

                  if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                      n = 0;
                      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                      XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                      XtSetArg( args[n], XmNleftOffset,            2                 ); n++;
                      XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                      XtSetArg( args[n], XmNwidth,                 100               ); n++;
                      XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                      XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                      XtSetArg( args[n], XmNeditable,              False             ); n++;
                      XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                      XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                      tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                      if (XTAS_SIMU_RES == 'Y')
                        sprintf( value, "%.1f pS", critic->NEXT->SIMSLOPE/TTV_UNIT );
                      else 
                        sprintf( value, "%.1f pS", critic->NEXT->SLOPE/TTV_UNIT );
                      XmTextSetString( tmp_widget, value );
                      XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                      XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                      XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );
                  }

                  n = 0;
                  XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                  XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                  XtSetArg( args[n], XmNleftOffset,            10                 ); n++;
                  XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                  XtSetArg( args[n], XmNwidth,                 100               ); n++;
                  XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                  XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                  XtSetArg( args[n], XmNeditable,              False             ); n++;
                  XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                  XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                  tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                  if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y')))
                      sprintf( value, "%.1f pS", (critic->NEXT->REFDELAY+critic->DATADELAY)/TTV_UNIT );
                  else
                      sprintf( value, "%.1f pS", (critic->NEXT->DELAY+critic->DATADELAY)/TTV_UNIT );

                  XmTextSetString( tmp_widget, value );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                  XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                  XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );

                  if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                      n = 0;
                      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                      XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                      XtSetArg( args[n], XmNleftOffset,            2                 ); n++;
                      XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                      XtSetArg( args[n], XmNwidth,                 100               ); n++;
                      XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                      XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                      XtSetArg( args[n], XmNeditable,              False             ); n++;
                      XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                      XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                      tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);
                      
                      if (XTAS_SIMU_RES == 'Y')
                          sprintf( value, "%.1f pS", critic->NEXT->SIMDELAY/TTV_UNIT );
                      else 
                          sprintf( value, "%.1f pS", (critic->NEXT->DELAY+critic->DATADELAY)/TTV_UNIT );
                      XmTextSetString( tmp_widget, value );
                      XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                      XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                      XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );
                  }

                  if(detail_set->PARENT != NULL)
                  {
                      n = 0;
                      XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                      XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                      XtSetArg( args[n], XmNleftOffset,            10                 ); n++;
                      XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                      XtSetArg( args[n], XmNwidth,                 100               ); n++;
                      XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                      XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                      XtSetArg( args[n], XmNeditable,              False             ); n++;
                      XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                      XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                      tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                      sprintf( value, "%.1f pS", total/TTV_UNIT);

                      XmTextSetString( tmp_widget, value );
                      XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                      XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                      XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );

                      if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                          n = 0;
                          XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
                          XtSetArg( args[n], XmNleftWidget,            tmp_widget        ); n++;
                          XtSetArg( args[n], XmNleftOffset,            2                 ); n++;
                          XtSetArg( args[n], XmNrightOffset,           2                 ); n++;
                          XtSetArg( args[n], XmNwidth,                 100               ); n++;
                          XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
                          XtSetArg( args[n], XmNcursorPositionVisible, False             ); n++;
                          XtSetArg( args[n], XmNeditable,              False             ); n++;
                          XtSetArg( args[n], XmNborderWidth,           1                 ); n++;
                          XtSetArg( args[n], XmNborderColor,           tas_backg_color   ); n++;
                         tmp_widget = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, sub_form, args, n);

                          sprintf( value, "%.1f pS", totalnew/TTV_UNIT );

                          XmTextSetString( tmp_widget, value );
                          XtAddCallback( tmp_widget, XmNfocusCallback, XtasDetailFocusCallback, (XtPointer)detail_set );
                          XtAddCallback( tmp_widget, XmNfocusCallback, XtasSelectDelayCallback, (XtPointer)head );
                          XtAddCallback( tmp_widget, XmNlosingFocusCallback, XtasLosingDetailFocusCallback, (XtPointer)0 );
                      }
                  }
                  critic = critic->NEXT;
            }
        head = head->NEXT ;
    }
    XtManageChild( row_widget );
    mbkfree( value );

}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateDetailMenus                                           */
/*                                                                           */
/* IN  ARGS : .parent : Parent widget for the Menu Bar.                      */
/*                                                                           */
/* OUT ARGS : .The value of the Menu Bar widget.                             */
/*                                                                           */
/* OBJECT   : Creates the menu bar of the detail  window.                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateDetailMenus( parent , tas_winfos )
    Widget parent;
    XtasWindowStruct *tas_winfos;
{
    Widget menu_bar;
    Arg    args[20];
    int    n;

    if (((XtasDetailPathSetStruct*)tas_winfos->userdata)->PARENT == NULL) { /*delay window */
        for(n = 0 ; XtasDetailMenuFile[n].label != NULL ; n++)
            XtasDetailMenuFile[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuViewDelay[n].label != NULL ; n++)
            XtasDetailMenuViewDelay[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuOptions[n].label != NULL ; n++)
            XtasDetailMenuOptions[n].callback_data = (XtPointer) tas_winfos;
    }
    else if (XTAS_SIMU_RES == 'Y'){
        for(n = 0 ; XtasDetailMenuFile[n].label != NULL ; n++)
            XtasDetailMenuFile[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuViewSimu[n].label != NULL ; n++)
            XtasDetailMenuViewSimu[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuOptions[n].label != NULL ; n++)
            XtasDetailMenuOptions[n].callback_data = (XtPointer) tas_winfos;
    }
    else {
        for(n = 0 ; XtasDetailMenuFile[n].label != NULL ; n++)
            XtasDetailMenuFile[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuView[n].label != NULL ; n++)
            XtasDetailMenuView[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuTools[n].label != NULL ; n++)
            XtasDetailMenuTools[n].callback_data    = (XtPointer) tas_winfos;
        for(n = 0 ; XtasDetailMenuOptions[n].label != NULL ; n++)
            XtasDetailMenuOptions[n].callback_data = (XtPointer) tas_winfos;
    }
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
    if (((XtasDetailPathSetStruct*)tas_winfos->userdata)->PARENT == NULL)
        XalBuildMenus( menu_bar,             /* The Menu Bar widget id               */
                XtasDetailMenuDelay );    /* The main menu XalMenus structure     */
    else if (XTAS_SIMU_RES == 'Y')
        XalBuildMenus( menu_bar,             /* The Menu Bar widget id               */
                XtasDetailMenuSimu );    /* The main menu XalMenus structure     */
        
    else if ((XTAS_CTX_LOADED & XTAS_LOADED) == 'Y')
        XalBuildMenus( menu_bar,             /* The Menu Bar widget id               */
                XtasDetailMenuCtk );    /* The main menu XalMenus structure     */
    else
        XalBuildMenus( menu_bar,             /* The Menu Bar widget id               */
                XtasDetailMenu  );    /* The main menu XalMenus structure     */

    return( menu_bar );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCreateDetailList                                           */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget id.                                */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Creation of the signals window objects.                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasCreateDetailList( parent, tas_winfos, detail_set, item_count )
    Widget             parent;
    XtasWindowStruct  *tas_winfos;
    XtasDetailPathSetStruct *detail_set;
    int                item_count;
{
    Widget             label_widget, list_widget, paned_w, info_w;
    Widget             form_widget, form, sub_form, dbase_form, pan_widget;
    Widget             buttons_form, frame;
    Widget             separator1, separator2;
    Widget             button;
    Widget             tmp;
    Widget             THE_label_widget;
    XmString           text;
    Arg                args[20];
    int                n;
    int                c;
    char              *value;
    ttvcritic_list    *critic;
    chain_list        *head;
    long               delay = (long)0, newdelay = (long)0;
    Pixel              tas_state_color,tas_backg_color ;
    Pixmap             pixmap;
    int                scrollheight_newdisp;
    XtasSearchPathParamStruct    *params;     


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

    /*------------------------------------------------*/
    /* Create a form to manage the first row children */
    /*------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,   XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM ); n++;
    buttons_form = XtCreateManagedWidget( "XtasMainButtons", xmFormWidgetClass, form_widget, args, n );

    if (((XtasDetailPathSetStruct*)tas_winfos->userdata)->PARENT == NULL)
        for ( n=0; XtasDetailButtonMenusDelay[n].pix_file != -1 ; n++ ) 
            XtasDetailButtonMenusDelay[n].callback_data = (XtPointer) tas_winfos;
    else if (XTAS_SIMU_RES == 'Y')
        for ( n=0; XtasDetailButtonMenusSimu[n].pix_file != -1 ; n++ )
            XtasDetailButtonMenusSimu[n].callback_data = (XtPointer) tas_winfos;
    else
        for ( n=0; XtasDetailButtonMenus[n].pix_file != -1 ; n++ )
            XtasDetailButtonMenus[n].callback_data = (XtPointer) tas_winfos;



    n = 0;
    XtSetArg( args[n], XmNnumColumns,      1             ); n++;
    XtSetArg( args[n], XmNorientation,     XmHORIZONTAL  ); n++;

    if (((XtasDetailPathSetStruct*)tas_winfos->userdata)->PARENT == NULL)
        tas_winfos->butwidget = XalButtonMenus( buttons_form, XtasDetailButtonMenusDelay, args, n, 40, 40 );
    else if (XTAS_SIMU_RES == 'Y')
        tas_winfos->butwidget = XalButtonMenus( buttons_form, XtasDetailButtonMenusSimu, args, n, 40, 40 );
    else
        tas_winfos->butwidget = XalButtonMenus( buttons_form, XtasDetailButtonMenus, args, n, 40, 40 );
    XtasAddDummyButton( tas_winfos->butwidget );
    XtManageChild( tas_winfos->butwidget );
    //XmAddTabGroup( tas_winfos->butwidget );

    /*---------------------------------------------------------------------------*/
    /* creation d'un separateur                                                  */
    /*---------------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM         ); n++;
    XtSetArg( args[n], XmNleftAttachment,  XmATTACH_FORM         ); n++;
    XtSetArg( args[n], XmNtopAttachment,   XmATTACH_WIDGET       ); n++;
    XtSetArg( args[n], XmNtopWidget,       buttons_form ); n++;
    separator1 = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, form_widget, args, n );


    /*---------------------------------------------------------------------------*/
    /* Create the message area                                                   */
    /*---------------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,            0                    ); n++;
    XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
    XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
    XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
    XtSetArg( args[n], XmNbottomOffset,           2                 ); n++;
    XtSetArg( args[n], XmNleftOffset,             2                 ); n++;
    XtSetArg( args[n], XmNrightOffset,            2                 ); n++;
    form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form_widget, args, n );

    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, form,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNleftOffset,      2,
            NULL);
    XtasSetLabelString( label_widget, "Informations Area" );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,             label_widget      ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
    XtSetArg( args[n], XmNrightPosition,         48                ); n++;
    frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
    n = 0;
    XtSetArg( args[n], XmNheight,                 20                       ); n++;
    label_widget = XtCreateManagedWidget( "XtasUserInfo", xmLabelWidgetClass, frame, args, n);
    XtasSetLabelString( label_widget, XTAS_NULINFO );
    detail_set->INFO_USER = label_widget;

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame                    ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++; 
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNleftWidget,       frame           ); n++;
    frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
    n = 0;
    XtSetArg( args[n], XmNheight,           20              ); n++;
    label_widget = XtCreateManagedWidget( "XtashelpInfo", xmLabelWidgetClass, frame, args, n);
    XtasSetLabelString( label_widget, XTAS_NULINFO );
    detail_set->INFO_HELP = label_widget;

    /*---------------------------------------------------------------------------*/
    /* creation d'un separateur                                                  */
    /*---------------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,     form       ); n++;
    separator2 = XtCreateManagedWidget( "XtasSeparator", xmSeparatorWidgetClass, form_widget, args, n );

    /*------------------------------------------------------------------------------------------------*/
    /* Creation de la paned window                                                                    */
    /*------------------------------------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget,        separator1      ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNbottomWidget,     separator2      ); n++;
    form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, form_widget , args, n );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
    pan_widget = XtCreateManagedWidget( "XtasShellSubForm", xmPanedWindowWidgetClass, form , args, n );

    /*---------------------------------------------------------------------------------------------*/
    /*  affichage d'informations generales sur le chemin detaille en cours                         */
    /*---------------------------------------------------------------------------------------------*/

    head = (chain_list *) detail_set->CUR_DETAIL->DATA;
    critic = (ttvcritic_list *) head->DATA;


    n = 0;
    form = XtCreateManagedWidget( "XtasShellSubForm", xmFormWidgetClass, pan_widget, args, n );

    value = (char *) mbkalloc (1024*sizeof(char));

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form, args, n );
    n = 0;
    sub_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame, args, n);

    text = XmStringCreateSimple ("From: ");
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,  XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNtopOffset,      6             ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNlabelString,    text          ); n++;
    XtSetArg( args[n], XmNwidth,          40            ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
    XmStringFree (text);
    
    XtVaGetValues( label_widget, XmNforeground, &tas_state_color, XmNbackground, &tas_backg_color, NULL );
    
    if (critic!=NULL)
    {
        if ( critic->SNODE == TTV_UP )
            text = XmStringCreateSimple ("(Rise)");
        else
            text = XmStringCreateSimple ("(Fall)");
    }
    n = 0;
    XtSetArg( args[n], XmNwidth,            40                          ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget                ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNbottomWidget,     label_widget                ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,       label_widget                ); n++;
    XtSetArg( args[n], XmNlabelString,      text                        ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );  
    detail_set->NODE_RF = label_widget;
    XmStringFree (text);
    n = 0;
    XtSetArg( args[n], XmNwidth,            800                         ); n++;
    XtSetArg( args[n], XmNeditable,         False                       ); n++;
    XtSetArg( args[n], XmNeditMode,         XmMULTI_LINE_EDIT           ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM               ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,       label_widget                ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM               ); n++;
    XtSetArg( args[n], XmNrightOffset,      5                           ); n++;
    XtSetArg( args[n], XmNresizeHeight,     True                        ); n++;
    XtSetArg( args[n], XmNresizeWidth,      True                        ); n++;
    XtSetArg( args[n], XmNwordWrap,         True                        ); n++;
    XtSetArg( args[n], XmNvalue,            critic->NAME                ); n++;
    XtSetArg( args[n], XmNcursorPositionVisible,  False                 ); n++;
    XtSetArg( args[n], XmNbackground,       tas_backg_color             ); n++;
    XtSetArg( args[n], XmNtopShadowColor,   tas_backg_color             ); n++;
    XtSetArg( args[n], XmNtopShadowPixmap,  XmUNSPECIFIED_PIXMAP        ); n++;
    XtSetArg( args[n], XmNbottomShadowColor,tas_backg_color             ); n++;
    XtSetArg( args[n], XmNbottomShadowPixmap,XmUNSPECIFIED_PIXMAP       ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmTextWidgetClass, sub_form, args, n );  
    detail_set->NODE_NAME = label_widget;


    if (critic!=NULL)
    {
        if (XTAS_SIMU_RES != 'Y')
            delay = critic->DELAY+critic->DATADELAY;
        while( critic->NEXT != NULL )
        {
            delay += critic->NEXT->DELAY+critic->DATADELAY;
            if (XTAS_SIMU_RES == 'Y')
                newdelay += critic->NEXT->SIMDELAY;
            else
                newdelay += critic->NEXT->DELAY+critic->DATADELAY;
            critic = critic->NEXT;
        }
    }

    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget, frame ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    frame = XtCreateManagedWidget( "XtasLabel", xmFrameWidgetClass, form, args, n );
    n = 0;
    sub_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame, args, n);
    
    text = XmStringCreateSimple ("To: ");   
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNtopOffset,        6             ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNlabelString,      text          ); n++;
    XtSetArg( args[n], XmNwidth,            40            ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
    XmStringFree (text);

    if (critic!=NULL)
    {
        if ( critic->SNODE == TTV_UP )
            text = XmStringCreateSimple ("(Rise)");    
        else
            text = XmStringCreateSimple ("(Fall)");    
    }
    n = 0;
    XtSetArg( args[n], XmNwidth,            40                          ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
    XtSetArg( args[n], XmNtopWidget,        label_widget                ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,       label_widget                ); n++;
    XtSetArg( args[n], XmNlabelString,      text                        ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );  
    detail_set->ROOT_RF = label_widget;
    XmStringFree (text);
    n = 0;
    XtSetArg( args[n], XmNwidth,            800                         ); n++;
    XtSetArg( args[n], XmNeditable,         False                       ); n++;
    XtSetArg( args[n], XmNeditMode,         XmMULTI_LINE_EDIT           ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM               ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET             ); n++;
    XtSetArg( args[n], XmNleftWidget,       label_widget                ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM               ); n++;
    XtSetArg( args[n], XmNrightOffset,      5                           ); n++;
    XtSetArg( args[n], XmNresizeHeight,     True                        ); n++;
    XtSetArg( args[n], XmNresizeWidth,      True                        ); n++;
    XtSetArg( args[n], XmNwordWrap,         True                        ); n++;
    XtSetArg( args[n], XmNvalue,            critic->NAME                ); n++;
    XtSetArg( args[n], XmNcursorPositionVisible,  False                 ); n++;
    XtSetArg( args[n], XmNbackground,       tas_backg_color             ); n++;
    XtSetArg( args[n], XmNtopShadowColor,   tas_backg_color             ); n++;
    XtSetArg( args[n], XmNtopShadowPixmap,  XmUNSPECIFIED_PIXMAP        ); n++;
    XtSetArg( args[n], XmNbottomShadowColor,tas_backg_color             ); n++;
    XtSetArg( args[n], XmNbottomShadowPixmap,XmUNSPECIFIED_PIXMAP       ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmTextWidgetClass, sub_form, args, n );  
    detail_set->ROOT_NAME = label_widget;

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,  XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget,  frame ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    frame = XtCreateManagedWidget( "XtasLabel", xmFrameWidgetClass, form, args, n );
    n = 0;
    sub_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame, args, n);
    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
    detail_set->TOTAL_DELAY = label_widget;
    if ((XTAS_SIMU_RES == 'Y') || ((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED))
        sprintf( value, "Total Delay: %.1f pS", newdelay/TTV_UNIT );
    else
        sprintf( value, "Total Delay: %.1f pS", delay/TTV_UNIT );
    XtasSetLabelString( label_widget, value );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,  XmATTACH_WIDGET ); n++;
    XtSetArg( args[n], XmNtopWidget,  frame ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNrightAttachment, XmATTACH_FORM ); n++;
    frame = XtCreateManagedWidget( "XtasLabel", xmFrameWidgetClass, form, args, n );
    n = 0;
    sub_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame, args, n);
    critic = (ttvcritic_list *)head->DATA;
    n = 0;
    XtSetArg( args[n], XmNtopAttachment, XmATTACH_FORM ); n++;
    XtSetArg( args[n], XmNleftAttachment, XmATTACH_FORM ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, sub_form, args, n );
    sprintf( value, "Starting Time: %.1f pS", (critic->DELAY+critic->DATADELAY)/TTV_UNIT );
    XtasSetLabelString( label_widget, value );

    mbkfree( value );


    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    frame = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, form,  args, n);
    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, frame, args, n);

    /*--------------------- recherche de plusieurs details de chemins ----------------------*/
    if ( head->NEXT != NULL )
    {
        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,   XmATTACH_FORM     ); n++;
        button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, dbase_form,  args, n);
        XtasSetLabelString( button, "Next");
        XtAddCallback( button, XmNactivateCallback, XtasNextDetailCallback, 
                (XtPointer)detail_set );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,     XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment,  XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,   XmATTACH_WIDGET     ); n++;
        XtSetArg( args[n], XmNrightWidget,       button     ); n++;
        button = XtCreateManagedWidget( "XtasButton", xmPushButtonWidgetClass, dbase_form,  args, n);
        XtasSetLabelString( button, "Prev");
        XtAddCallback( button, XmNactivateCallback, XtasPrevDetailCallback, 
                (XtPointer)detail_set );
    }

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabel", xmLabelWidgetClass, dbase_form,  args, n);
    if(detail_set->PARENT == NULL)
        XtasSetLabelString( label_widget, "Delay List");
    else {
        params = detail_set->PARENT->PARAM;
        if (params->CRITIC == 'P') {
            if ((params->SCAN_TYPE & TTV_FIND_MAX) == TTV_FIND_MAX){
                if (XTAS_SIMU_RES == 'Y')
                    XtasSetLabelString( label_widget, "Simulated Parallel Max Path Detail List");
                else
                    XtasSetLabelString( label_widget, "Parallel Max Path Detail List");
            }
            else {
                if (XTAS_SIMU_RES == 'Y')
                    XtasSetLabelString( label_widget, "Simulated Parallel Min Path Detail List");
                else
                    XtasSetLabelString( label_widget, "Parallel Min Path Detail List");
            }
        }
        else if ((params->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS) {
            if ((params->REQUEST_TYPE & XTAS_CRITIC) == XTAS_CRITIC) {
                if ((params->SCAN_TYPE & TTV_FIND_MAX) == TTV_FIND_MAX) {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Critic Max Path Detail List");
                    else
                        XtasSetLabelString( label_widget, "Critic Max Path Detail List");
                }
                else {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Critic Min Path Detail List");
                    else
                        XtasSetLabelString( label_widget, "Critic Min Path Detail List");
                }
            }
            else {
                if ((params->SCAN_TYPE & TTV_FIND_MAX) == TTV_FIND_MAX) {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Max Path Detail List");
                    else
                        XtasSetLabelString( label_widget, "Max Path Detail List");
                }
                else {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Min Path Detail List");
                    else
                        XtasSetLabelString( label_widget, "Min Path Detail List");
                }
            }
        }
        else {
            if ((params->REQUEST_TYPE & XTAS_CRITIC) == XTAS_CRITIC) {
                if ((params->SCAN_TYPE & TTV_FIND_MAX) == TTV_FIND_MAX) {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Critic Max Access Detail List");
                    else
                        XtasSetLabelString( label_widget, "Critic Max Access Detail List");
                }
                else {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Critic Min Access Detail List");
                    else
                        XtasSetLabelString( label_widget, "Critic Min Access Detail List");
                }
            }
            else {
                if ((params->SCAN_TYPE & TTV_FIND_MAX) == TTV_FIND_MAX) {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Max Access Detail List");
                    else
                        XtasSetLabelString( label_widget, "Max Access Detail List");
                }
                else {
                    if (XTAS_SIMU_RES == 'Y')
                        XtasSetLabelString( label_widget, "Simulated Min Access Detail List");
                    else
                        XtasSetLabelString( label_widget, "Min Access Detail List");
                }
            }
        }
    }
    
    if(XTAS_DISPLAY_NEW)
    {
        n = 0;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n );
        if(detail_set->PARENT != NULL)
        {
            if(XTAS_SIMU_RES == 'Y')
                text = XmStringCreateSimple( "TAS Tp" );
            else
                text = XmStringCreateSimple( "Tp" );

            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_FORM,
                    XmNalignment,       XmALIGNMENT_END,
                    XmNwidth,           100,
                    XmNtopOffset,       2,
                    XmNrightOffset,     2,
                    XmNleftOffset,      XTAS_DETAIL_TP_LOFFSET_T,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );
            THE_label_widget = label_widget;

            if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                if(XTAS_SIMU_RES == 'Y')
                    text = XmStringCreateSimple( "SPICE Tp" );
                else
                    text = XmStringCreateSimple( "Tp CTK" );

                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelString,     text,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNalignment,       XmALIGNMENT_END,
                        XmNwidth,           77,
                        XmNtopOffset,       2,
                        XmNrightOffset,     2,
                        XmNleftOffset,      6,
                        XmNheight,          25,
                        NULL);
                XmStringFree( text );
            }

            if(XTAS_SIMU_RES == 'Y')
                text = XmStringCreateSimple( "TAS Total" );
            else
                text = XmStringCreateSimple( "Total" );
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNalignment,       XmALIGNMENT_END,
                    XmNwidth,           77,
                    XmNtopOffset,       2,
                    XmNrightOffset,     2,
                    XmNleftOffset,      6,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );
            if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                if (XTAS_SIMU_RES == 'Y')
                    text = XmStringCreateSimple( "SPICE Total" );
                else 
                    text = XmStringCreateSimple( "Total CTK" );
                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelString,     text,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNalignment,       XmALIGNMENT_END,
                        XmNwidth,           77,
                        XmNtopOffset,       2,
                        XmNrightOffset,     2,
                        XmNleftOffset,      6,
                        XmNheight,          25,
                        NULL);
                XmStringFree( text );
            }

            if(XTAS_SIMU_RES == 'Y')
                text = XmStringCreateSimple( "TAS Slope" );
            else
                text = XmStringCreateSimple( "Slope" );
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNalignment,       XmALIGNMENT_END,
                    XmNwidth,           83,
                    XmNtopOffset,       2,
                    XmNrightOffset,     2,
                    XmNleftOffset,      2,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );
            if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                if (XTAS_SIMU_RES == 'Y')
                    text = XmStringCreateSimple( "SPICE Slope" );
                else 
                    text = XmStringCreateSimple( "Slope CTK" );
                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelString,     text,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNalignment,       XmALIGNMENT_END,
                        XmNwidth,           81,
                        XmNtopOffset,       2,
                        XmNrightOffset,     2,
                        XmNleftOffset,      2,
                        XmNheight,          25,
                        NULL);
                XmStringFree( text );
            }
            text = XmStringCreateSimple( "Capa" );
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNalignment,       XmALIGNMENT_END,
                    XmNwidth,            80,
                    XmNtopOffset,       2,
                    XmNrightOffset,     2,
                    XmNleftOffset,      2,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );

            text = XmStringCreateSimple( "Signal" );
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNalignment,       XmALIGNMENT_END,
                    XmNwidth,           100,
                    XmNtopOffset,       2,
                    XmNrightOffset,     2,
                    XmNleftOffset,      40,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );
            scrollheight_newdisp = 300 ;
        }
        else 
        {
            scrollheight_newdisp = 200 ; /* if detailpath is call by getdelay */
        }
        n=0;
        XtSetArg( args[n], XmNtopAttachment,          XmATTACH_WIDGET      ); n++;
        XtSetArg( args[n], XmNtopWidget,              label_widget         ); n++;
        XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNheight,           300             ); n++;
        paned_w = XtCreateManagedWidget( "XtasPanedWidget", 
                xmPanedWindowWidgetClass,
                dbase_form,
                args, n );
        /* list_widget will contain the scroll list of detail path */
        list_widget = XtVaCreateManagedWidget (       "XtasSubForm",
                xmFormWidgetClass,
                paned_w,
                XmNheight, scrollheight_newdisp, 
                NULL) ;
        detail_set->SCROLL_DETAIL = list_widget;

        /* list_widget will display info path */
        info_w = XtVaCreateManagedWidget (       "XtasSubForm",
                xmFormWidgetClass,
                paned_w,
                XmNtopAttachment,    XmATTACH_WIDGET,
                XmNtopWidget    ,    list_widget,
                NULL) ;
        detail_set->INFO_PAGE = info_w ;

        if (detail_set->PARENT == NULL)
            XtasFillDelayList ( detail_set ) ;
        else
            XtasFillDetailPathList2( detail_set, THE_label_widget ); 
    }
    else /* old display format */
    {
        n = 0;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n );

        text = XmStringCreateSimple( "From" );

        THE_label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNwidth,           150,
                XmNtopOffset,       2,
                XmNleftOffset,      XTAS_DETAIL_FROM_LOFFSET_G,
                XmNheight,          25,
                NULL);
        XmStringFree( text );
        text = XmStringCreateSimple( "Type" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_WIDGET,
                XmNleftWidget,      THE_label_widget,
                XmNwidth,           136,
                XmNtopOffset,       2,
                XmNleftOffset,      7,
                XmNheight,          25,
                NULL);
        XmStringFree( text );
        text = XmStringCreateSimple( "To" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_WIDGET,
                XmNleftWidget,      label_widget,
                XmNwidth,           150,
                XmNtopOffset,       2,
                XmNleftOffset,      2,
                XmNheight,          25,
                NULL);
        XmStringFree( text );

        text = XmStringCreateSimple( "Capa" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_WIDGET,
                XmNleftWidget,      label_widget,
                XmNwidth,           100,
                XmNtopOffset,       2,
                XmNleftOffset,      12,
                XmNheight,          25,
                NULL);
        XmStringFree( text );

        if((detail_set->PARENT != NULL)&&(XTAS_SIMU_RES == 'Y'))
            text = XmStringCreateSimple( "TAS Slope" );
        else if((detail_set->PARENT != NULL)&&((XTAS_CTX_LOADED & XTAS_LOADED)==XTAS_LOADED))
            text = XmStringCreateSimple( "Ref. Slope" );
        else
            text = XmStringCreateSimple( "Output Slope" );

        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_WIDGET,
                XmNleftWidget,      label_widget,
                XmNwidth,           100,
                XmNtopOffset,       2,
                XmNleftOffset,      12,
                XmNheight,          25,
                NULL);
        XmStringFree( text );


        if ((detail_set->PARENT != NULL) && (((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED) || (XTAS_SIMU_RES == 'Y'))) {
            if (XTAS_SIMU_RES == 'Y')
                text = XmStringCreateSimple( "SPICE Slope" );
            else
                text = XmStringCreateSimple( "Slope Crosstalk" );

            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNwidth,           100,
                    XmNtopOffset,       2,
                    XmNleftOffset,      2,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );

        }

        pixmap = XalGetPixmap( label_widget, XTAS_TIME_MAP, tas_state_color, tas_backg_color );

        if ( pixmap == XmUNSPECIFIED_PIXMAP )
        {
            text = XmStringCreateSimple( "Time" );
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNwidth,           100,
                    XmNtopOffset,       2,
                    XmNleftOffset,      12,
                    XmNheight,          25,
                    NULL);

            XmStringFree( text );
        }
        else {
            if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y')) ){
                if (XTAS_SIMU_RES == 'Y')
                    text = XmStringCreateSimple( "TAS " );
                else
                    text = XmStringCreateSimple( "Ref. " );
                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelString,     text,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNalignment,       XmALIGNMENT_END,
                        XmNwidth,           47,
                        XmNtopOffset,       2,
                        XmNleftOffset,      12,
                        XmNheight,          25,
                        NULL);
                XmStringFree( text );
                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelType,       XmPIXMAP,
                        XmNlabelPixmap,     pixmap,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNalignment,       XmALIGNMENT_BEGINNING,
                        XmNwidth,           53,
                        XmNtopOffset,       2,
                        XmNleftOffset,      0,
                        XmNheight,          25,
                        NULL);
            }
            else
                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelType,       XmPIXMAP,
                        XmNlabelPixmap,     pixmap,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNwidth,           100,
                        XmNtopOffset,       2,
                        XmNleftOffset,      12,
                        XmNheight,          25,
                        NULL);
        }

        if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
            if ( pixmap == XmUNSPECIFIED_PIXMAP )
            {
                if (XTAS_SIMU_RES == 'Y')
                    text = XmStringCreateSimple( "SPICE Time" );
                else
                    text = XmStringCreateSimple( "Crosstalk Time" );

                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelString,     text,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNwidth,           100,
                        XmNtopOffset,       2,
                        XmNleftOffset,      4,
                        XmNheight,          25,
                        NULL);
                XmStringFree( text );
            }

            else{
                if (XTAS_SIMU_RES == 'Y') {
                    text = XmStringCreateSimple( "SPICE" );
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                            XmNlabelString,     text,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      label_widget,
                            XmNalignment,       XmALIGNMENT_END,
                            XmNwidth,           50,
                            XmNtopOffset,       2,
                            XmNleftOffset,      0,
                            XmNheight,          25,
                            NULL);
                    XmStringFree( text );
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                            XmNlabelType,       XmPIXMAP,
                            XmNlabelPixmap,     pixmap,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      label_widget,
                            XmNalignment,       XmALIGNMENT_BEGINNING,
                            XmNwidth,           50,
                            XmNtopOffset,       2,
                            XmNleftOffset,      2,
                            XmNheight,          25,
                            NULL);

                }
                else {
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                            XmNlabelType,       XmPIXMAP,
                            XmNlabelPixmap,     pixmap,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      label_widget,
                            XmNalignment,       XmALIGNMENT_END,
                            XmNwidth,           36,
                            XmNtopOffset,       2,
                            XmNleftOffset,      4,
                            XmNheight,          25,
                            NULL);
                    text = XmStringCreateSimple( "Crosstalk" );
                    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                            XmNlabelString,     text,
                            XmNtopAttachment,   XmATTACH_FORM,
                            XmNleftAttachment,  XmATTACH_WIDGET,
                            XmNleftWidget,      label_widget,
                            XmNalignment,       XmALIGNMENT_BEGINNING,
                            XmNwidth,           64,
                            XmNtopOffset,       2,
                            XmNleftOffset,      0,
                            XmNheight,          25,
                            NULL);
                    XmStringFree( text );
                }

            }
        }


        if(detail_set->PARENT != NULL)
        {
            if(XTAS_SIMU_RES == 'Y') 
                text = XmStringCreateSimple( "TAS Total" );
            else if((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)
                text = XmStringCreateSimple( "Ref. Total" );
            else
                text = XmStringCreateSimple( "Total" );
            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                    XmNlabelString,     text,
                    XmNtopAttachment,   XmATTACH_FORM,
                    XmNleftAttachment,  XmATTACH_WIDGET,
                    XmNleftWidget,      label_widget,
                    XmNwidth,           100,
                    XmNtopOffset,       2,
                    XmNleftOffset,      10,
                    XmNheight,          25,
                    NULL);
            XmStringFree( text );
            if((detail_set->PARENT != NULL)&&(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y'))) {
                if(XTAS_SIMU_RES == 'Y')
                    text = XmStringCreateSimple( "SPICE Total" );
                else
                    text = XmStringCreateSimple( "Total Crosstalk" );
                label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
                        XmNlabelString,     text,
                        XmNtopAttachment,   XmATTACH_FORM,
                        XmNleftAttachment,  XmATTACH_WIDGET,
                        XmNleftWidget,      label_widget,
                        XmNwidth,           100,
                        XmNtopOffset,       2,
                        XmNleftOffset,      0,
                        XmNheight,          25,
                        NULL);
                XmStringFree( text );
            }
        }

        n=0;
        XtSetArg( args[n], XmNtopAttachment,          XmATTACH_WIDGET      ); n++;
        XtSetArg( args[n], XmNtopWidget,              label_widget         ); n++;
        XtSetArg( args[n], XmNbottomAttachment,       XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNleftAttachment,         XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNrightAttachment,        XmATTACH_FORM        ); n++;
        XtSetArg( args[n], XmNscrollBarPlacement,     XmBOTTOM_RIGHT  ); n++;
        XtSetArg( args[n], XmNscrollBarDisplayPolicy, XmAS_NEEDED     ); n++;
        XtSetArg( args[n], XmNscrollingPolicy,        XmAUTOMATIC     ); n++;
        if (detail_set->PARENT == NULL)
            item_count = detail_set->CUR_DETAIL->SIZE;
        if (item_count < 10) {
            XtSetArg( args[n], XmNheight,                 8+(item_count*33)  ); n++;
        }
        else {
            XtSetArg( args[n], XmNheight,                 8+(10*33)          ); n++;
        }
        list_widget = XmCreateScrolledWindow( dbase_form, "XtasScrollWindow", args, n );
        XtManageChild( list_widget );
        XtVaGetValues (list_widget, XmNhorizontalScrollBar, &tmp, NULL);
        XtAddCallback (tmp, XmNvalueChangedCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNdragCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNincrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNdecrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNpageIncrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNpageDecrementCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNtoTopCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
        XtAddCallback (tmp, XmNtoBottomCallback, XtasDetailColumnsTitleMoveWithHSBCallback, THE_label_widget);
 
        detail_set->SCROLL_DETAIL = list_widget;

        XtasFillDetailPathList( detail_set );
    }

    XtManageChild( form_widget );

    return( form_widget );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDetailList                                                 */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .path_detail : The path detail description.                    */
/*                                                                           */
/* OUT ARGS : The shell widget id.                                           */
/*                                                                           */
/* OBJECT   : The main routine of Detail List Main window command            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
Widget XtasPathDetailList( parent, path_set )
    Widget             parent;
    XtasPathListSetStruct	  *path_set;
{
    Widget main_window;
    Arg    args[20];
    int    n;
    int    c;
    int    width1 ;
    int    width2 ;
    ttvcritic_list *detail_list, *critic_sim ;
    chain_list     *head = NULL;
    XtasDetailPathSetStruct *detail_set,*detail_set2;
    static XtasWindowStruct *tas_winfos;
    char    text_title[128];
//    XtasChainJmpList *cur_detail_save, *detail_list_save;
    
    
   if (XTAS_DETPATHSELECTEDWIDGET != NULL) {
       XtasDetPathDeselect (XTAS_DETPATHSELECTEDWIDGET);
       XTAS_DETPATHSELECTEDWIDGET = NULL;
   }

    if((detail_set = XtasTtvGetDelayList(path_set)) == NULL)
        return(NULL) ;
    
    if(XTAS_SIMU_RES == 'Y') {
//        cur_detail_save = detail_set->CUR_DETAIL;
//        detail_list_save = detail_set->DETAIL_LIST;
        tas_winfos = (XtasWindowStruct *)path_set->DETAIL_SESSION->DATA;   
        detail_set2 = (XtasDetailPathSetStruct *) tas_winfos->userdata;
//        detail_set->CUR_DETAIL = detail_set2->CUR_DETAIL;
//        detail_set->DETAIL_LIST = detail_set2->DETAIL_LIST;

        head = (chain_list *) detail_set2->CUR_DETAIL->DATA;
        critic_sim = (ttvcritic_list *)head->DATA;

        head = (chain_list *) detail_set->CUR_DETAIL->DATA;
        detail_list = (ttvcritic_list *) head->DATA;

        while (detail_list && critic_sim) {
            detail_list->SIMSLOPE = critic_sim->SIMSLOPE;
            detail_list->SIMDELAY = critic_sim->SIMDELAY;
            critic_sim = critic_sim->NEXT;
            detail_list = detail_list->NEXT;
        }


        XtasRemoveDetailPathSession (tas_winfos);
    }
    
    tas_current_signal = NULL;

    tas_winfos = (XtasWindowStruct *) mbkalloc( sizeof(XtasWindowStruct) );

    head = (chain_list *)detail_set->CUR_DETAIL->DATA ;
    detail_list = head->DATA;

    if ( path_set->PARAM->CRITIC == 'D' )
        c = path_set->PARAM->PATH_ITEM ;
    else {
        c = 0;
        while( detail_list!=NULL)
        {
            detail_list = detail_list->NEXT;
            c++;
        } 
        c = c-1; /* pour correspondre au nombre effectif de lignes */
    }

    if( !XTAS_DISPLAY_NEW )
    {
        if(((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)||(XTAS_SIMU_RES == 'Y')) {
            if (c<10) width1 = 1205;
            else width1 = 1225;
        }
        else {
            if (c<10) width1 = 915 ;
            else width1 = 935 ;
        }
        if (c<10) width2 = 810 ;
        else width2 = 830 ;
    }
    else
    {
        width1 = 850 ;
        width2 = 650 ;
    }

    if(path_set->PARAM->CRITIC != 'D')
        tas_winfos->topwidget = XtVaCreatePopupShell( NULL, topLevelShellWidgetClass, parent,
                XmNwidth,            width1,
                XmNallowShellResize, False,
                XmNdeleteResponse,   XmDESTROY,
                NULL );
    else
        tas_winfos->topwidget = XtVaCreatePopupShell( NULL, topLevelShellWidgetClass, parent,
                XmNwidth,            width2,
                XmNallowShellResize, False,
                XmNdeleteResponse,   XmDESTROY,
                NULL );

    tas_winfos->userdata = detail_set;

    if(path_set->PARAM->CRITIC != 'D')
    {
        detail_set->PARENT = path_set;
        path_set->DETAIL_SESSION = addchain(path_set->DETAIL_SESSION,tas_winfos) ;
    }
    XTAS_DETPATH_SESSION = addchain(XTAS_DETPATH_SESSION,tas_winfos) ;
    
    n = 0;

    if ( path_set->PARAM->CRITIC == 'D' )
    {
        sprintf (text_title, XTAS_NAME": Delay Window - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
    }
    else 
    {
        sprintf (text_title, XTAS_NAME": Path Detail Window - %s", XtasMainParam->ttvfig->INFO->FIGNAME);
    }
    XtSetArg( args[n], XmNtitle , text_title); n++;
    XtSetValues( tas_winfos->topwidget, args, n );

    detail_set->TOP_LEVEL = tas_winfos->topwidget;

    /*------------------------------------------------*/
    /* Create the Alliance applications standard Icon */
    /*------------------------------------------------*/
    if ( path_set->PARAM->CRITIC == 'D' )
        XalCustomIcon( tas_winfos->topwidget, XTAS_EXEC_MAP );
    else
        XalCustomIcon( tas_winfos->topwidget, XTAS_DETA_MAP );

    n = 0;
    main_window = XmCreateMainWindow( tas_winfos->topwidget, "XtasMain", args, n );
    XtManageChild( main_window );

    tas_winfos->menwidget = XtasCreateDetailMenus( main_window ,tas_winfos );
    XtManageChild( tas_winfos->menwidget );

    tas_winfos->wrkwidget  =  XtasCreateDetailList( main_window, tas_winfos, detail_set, c ); 
    XtManageChild( tas_winfos->wrkwidget );
    if((path_set->PARAM->CRITIC != 'D')&&(XTAS_SIMU_RES == 'Y')) {
        tas_winfos->userinfos = XTAS_SIMU;
//        detail_set->CUR_DETAIL = cur_detail_save;
//        detail_set->DETAIL_LIST = detail_list_save;
        XTAS_SIMU_RES = 'N';
    }
    else if ((path_set->PARAM->CRITIC != 'D')&&((XTAS_CTX_LOADED & XTAS_LOADED) == XTAS_LOADED)) {
        tas_winfos->userinfos = XTAS_CTK;
    }

    /*----------------------------------------*/
    /* Creates a Motif Error and Warning Boxs */
    /*----------------------------------------*/
    tas_winfos->errwidget = XalCreateErrorBox(   tas_winfos->wrkwidget );
    tas_winfos->warwidget = XalCreateWarningBox( tas_winfos->wrkwidget );

    XmMainWindowSetAreas( main_window,
            tas_winfos->menwidget,
            (Widget) NULL,
            (Widget) NULL,
            (Widget) NULL,
            tas_winfos->wrkwidget );

    XtPopup( tas_winfos->topwidget, XtGrabNone );

    return ( tas_winfos->topwidget ); 
}

