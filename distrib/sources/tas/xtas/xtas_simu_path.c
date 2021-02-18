/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_simu_path.c                                            */
/*                                                                          */
/*    Author(s) : Caroline BLED                       Date : 02/25/2003     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#include "xtas.h"
#include GEN_H

char    XTAS_SIMU_RES = 'N';
Widget  XtasSimuParamWidget = NULL;
xtas_simu_param_struct  *XtasSimuParams = NULL;
char    XTAS_SIMU_NEW_NETLIST = 'N';
Widget XtasSimuMessageWidget = NULL ;

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasSimuCancelCallback                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuCancelCallback (widget, client_data, call_data)
    Widget      widget;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XtUnmanageChild ((Widget) client_data);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasSimuSetParamString                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuSetParamString (dest, text)
    char    **dest;
    char    *text;
{
    if ((*dest) == NULL) 
        *dest = (char*) mbkalloc (sizeof (char) * (strlen(text)+1));
    else 
        *dest = (char*) mbkrealloc (*dest, sizeof (char) * (strlen(text)+1));
    
    strcpy (*dest, text);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasSimuFreeParams                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuFreeParams (void)
{
    mbkfree (XtasSimuParams->tec);
    mbkfree (XtasSimuParams);
    XtasSimuParams = NULL;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasDestroySimuParams                            */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasDestroySimuParams (void)
{
    if (XtasSimuParamWidget) {
        XtDestroyWidget (XtasSimuParamWidget);
        XtasSimuParamWidget = NULL;
    }
    if (XtasSimuParams)
        XtasSimuFreeParams ();
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                    XtasSimuUpdateMessageWindow                           */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasSimuUpdateMessageWindow (parent, message, display)
Widget   parent;
char    *message;
int      display;
{
    static      Widget XtasSimuMessageLabel  = NULL ; 
    XmString    text, textLabel;
    int         n;
    Arg         args[10];
    Widget      form;
    
    textLabel = XmStringCreateSimple ("");
    if (message)
        textLabel = XmStringCreateSimple (message);

    if (!XtasSimuMessageWidget) {
        text = XmStringCreateSimple( "Simulation in progress..." ) ;
        n = 0;
        XtSetArg( args[n], XmNtitle, XTAS_NAME": Simulation " ) ; n++ ;
        XtSetArg( args[n], XmNmessageString, text ) ; n++ ;
        XtSetArg( args[n], XmNwidth, 300) ; n++ ;
        XtSetArg( args[n], XmNnoResize, True) ; n++ ;
        XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL) ; n++ ;
        XtasSimuMessageWidget = XmCreateWorkingDialog (parent, "XtasInfosBox",args, n) ;
	    HelpFather = XtasSimuMessageWidget ;
        XtUnmanageChild (XtNameToWidget (XtasSimuMessageWidget,"Help"));
        XtUnmanageChild (XtNameToWidget (XtasSimuMessageWidget,"Cancel"));
        XtUnmanageChild (XtNameToWidget (XtasSimuMessageWidget,"OK"));
        XtUnmanageChild (XmMessageBoxGetChild (XtasSimuMessageWidget,XmDIALOG_SEPARATOR));
        //XmAddTabGroup( XtasStbInProgressWidget ) ;
        XmStringFree(text);
        
        n = 0 ;
        XtSetArg( args[n], XmNborderWidth,      0                 ) ; n++ ;
        form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, XtasSimuMessageWidget, args, n ) ;
  
        n = 0 ;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ) ; n++ ;
        XtSetArg( args[n], XmNalignment,        XmALIGNMENT_CENTER) ; n++ ;
        XtSetArg( args[n], XmNlabelString,      textLabel     ) ; n++ ;
        XtasSimuMessageLabel = XtCreateManagedWidget( "XtasMainTitles", xmLabelGadgetClass, form, args, n ) ;

    }
    else {
        if (message) {
            XtVaSetValues(XtasSimuMessageLabel, XmNlabelString, textLabel, NULL);
//            XtVaSetValues(XtasSimuMessageWidget, XmNwidth, 300, NULL);
        }
    }
    
    if (display) {
        XtManageChild( XtasSimuMessageWidget ) ;
        XalForceUpdate(XtasSimuMessageWidget) ;
        sleep (1);
    }
    XmStringFree(textLabel);

}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                    XtasSimuDestroyMessageWindow                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasSimuDestroyMessageWindow ()
{

    if(XtasSimuMessageWidget) {
        XtUnmanageChild (XtasSimuMessageWidget);
        XmUpdateDisplay (XtParent(XtasSimuMessageWidget));
        XtDestroyWidget(XtasSimuMessageWidget) ;
    }
    XtasSimuMessageWidget = NULL;
}
    
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasSimuPath                                     */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasSimuPath (tas_winfos)
    XtasWindowStruct    *tas_winfos;
{
    XtasDetailPathSetStruct *detail_set;
    chain_list              *head;
    ttvcritic_list          *critic;
//    static char             *prevfigname = NULL;
    static lofig_list       *lofig = NULL;
    static cnsfig_list      *cnsfig = NULL;
    char                    buf[512];
    int cns_exist = 0;
    int gns_exist = 0;
    int spi_exist = 0;
    int res_simu = 0;
    chain_list *chcritic;

    detail_set = (XtasDetailPathSetStruct *) tas_winfos->userdata;
    head = detail_set->CUR_DETAIL->DATA;
    critic = head->DATA;

    XtasPasqua();
    if (sigsetjmp( XtasMyEnv , 1 ) == 0)
    {
          XtasSimuUpdateMessageWindow (detail_set->TOP_LEVEL, " ", 0);
          if (!lofig || (XTAS_SIMU_NEW_NETLIST == 'Y')) {
          if (filepath (XtasMainParam->ttvfig->INFO->FIGNAME,"cns"))
            cns_exist = 1;
          if ((filepath (XtasMainParam->ttvfig->INFO->FIGNAME,"gns")))
            gns_exist = 1;
          if ((filepath (XtasMainParam->ttvfig->INFO->FIGNAME,IN_LO)))
            spi_exist = 1;
          if (cns_exist) {
            XtasSimuUpdateMessageWindow (NULL, "Loading cns figure", 1);
          if((cnsfig =getloadedcnsfig(XtasMainParam->ttvfig->INFO->FIGNAME)) == NULL)
           {
            cnsfig = getcnsfig(XtasMainParam->ttvfig->INFO->FIGNAME, NULL);
            cns_addmultivoltage(getloadedinffig(cnsfig->NAME),cnsfig) ;
           }
            lofig = getloadedlofig (XtasMainParam->ttvfig->INFO->FIGNAME);
          }
          else {
            if ( spi_exist ) {
              lofig = getlofig(XtasMainParam->ttvfig->INFO->FIGNAME,'A');
              lofigchain (lofig);
            }
          }
          if ( !lofig ) {
              sprintf (buf, "Can't get description of figure %s.\nSimulation aborted.",XtasMainParam->ttvfig->INFO->FIGNAME);
              XalDrawMessage (tas_winfos->errwidget, buf);
            return;
          }
          // if a gns file exist, create corresponding table
          if ( gns_exist ) {
            LoadDynamicLibraries (NULL);
            LATEST_GNS_RUN = gnsParseCorrespondanceTables(XtasMainParam->ttvfig->INFO->FIGNAME);
            UpdateTransistorsForYagle(LATEST_GNS_RUN,TRUE);
          }
        }

//        prevfigname = XtasMainParam->ttvfig->INFO->FIGNAME;
        XTAS_SIMU_NEW_NETLIST = 'N';

        XtasSetLabelString( detail_set->INFO_HELP, XTAS_NULINFO );
        XtasSetLabelString( detail_set->INFO_HELP, "Simulation is running..." );


//        XtasCleanStates();
//        XalSetCursor( detail_set->TOP_LEVEL, WAIT );  
//        XalForceUpdate( detail_set->TOP_LEVEL );

        tas_simu_set_progression (XtasSimuUpdateMessageWindow);

        chcritic = addchain( NULL, critic );
        if((detail_set->PARENT->PARAM->SCAN_TYPE & TTV_FIND_MAX ) == TTV_FIND_MAX)
            res_simu = tas_simu_netlist(XtasMainParam->ttvfig, lofig, cnsfig, NULL, chcritic, SIM_MAX, 1, NULL, NULL, NULL, 0 , 0 );
        else
            res_simu = tas_simu_netlist(XtasMainParam->ttvfig, lofig, cnsfig, NULL, chcritic, SIM_MAX, 1, NULL, NULL, NULL, 0 , 0 );
        // on appelle toujours avec SIM_MAX car TAS ne gere pas les bons delay min
        // pour certain design
        freechain( chcritic );

    }    
    XtasSimuDestroyMessageWindow ();
    XtasGetWarningMess() ;
    XtasFirePasqua() ;

    
    if ( !res_simu ) {
      XtasSetLabelString( detail_set->INFO_HELP, XTAS_NULINFO );
      XtasSetLabelString( detail_set->INFO_HELP, "Simulation failed" );
      XTAS_SIMU_RES = 'N';
    }
    else {
      XtasSetLabelString( detail_set->INFO_HELP, XTAS_NULINFO );
      XtasSetLabelString( detail_set->INFO_HELP, "Simulation is finished" );
      XTAS_SIMU_RES = 'Y';
    }

    XalSetCursor( detail_set->TOP_LEVEL, NORMAL ); 
    XalForceUpdate( detail_set->TOP_LEVEL );

    if ( (res_simu) )
      XtasPathDetailList( detail_set->PARENT->TOP_LEVEL, 
                          detail_set->PARENT );
}


/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuUpdateVariables                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuUpdateVariables (void)
{
    char    pt[1024], buf[1024];
    
    /* Techno File */
    SIM_TECHFILE            = sensitive_namealloc (XtasSimuParams->tec);
    avt_sethashvar ("simTechnologyName", SIM_TECHFILE);
    
    /* Tool */
    V_INT_TAB[__SIM_TOOL].VALUE  = XtasSimuParams->tool;
    SIM_SPICESTRING         = sensitive_namealloc (XtasSimuParams->spicestr);
    avt_sethashvar ("avtSpiceString", SIM_SPICESTRING);
    SIM_SPICEOUT            = sensitive_namealloc (XtasSimuParams->spice_out);
    avt_sethashvar ("avtSpiceOutFile", SIM_SPICEOUT);
    SIM_SPICESTDOUT         = sensitive_namealloc (XtasSimuParams->spice_stdout);
    avt_sethashvar ("avtSpiceStdoutFile", SIM_SPICESTDOUT);
    V_BOOL_TAB[__SIM_USE_PRINT].VALUE           = XtasSimuParams->use_print==SIM_NO?0:1;
    V_BOOL_TAB[__SIM_USE_MEAS].VALUE           = XtasSimuParams->use_meas==SIM_NO?0:1;
   
    /* Conditions */
    V_FLOAT_TAB[__SIM_TIME].VALUE                = XtasSimuParams->trans_time*1e-9;
    V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE              = XtasSimuParams->vdd;
    sprintf (buf, "%f", V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE);
    avt_sethashvar ("simPowerSupply", buf);
    V_FLOAT_TAB[__SIM_TEMP].VALUE = XtasSimuParams->temp;
    sprintf (buf, "%f", V_FLOAT_TAB[__SIM_TEMP].VALUE);
    avt_sethashvar ("simTemperature", buf);
    SIM_SPICE_OPTIONS       = sensitive_namealloc (XtasSimuParams->spice_options);
    avt_sethashvar ("simSpiceOptions", SIM_SPICE_OPTIONS);
    
    /* Transient */
    V_FLOAT_TAB[__SIM_TRAN_STEP].VALUE           = XtasSimuParams->trans_step * 1e-12 ;
    
    /* Input/Output Constraints */
    SIM_INPUT_START         = XtasSimuParams->input_start * (1.0e-12);
    sprintf (buf, "%f", SIM_INPUT_START);
    avt_sethashvar ("simInputStartTime", buf);
    SIM_SLOP                = XtasSimuParams->input_slope;
    sprintf (buf, "%fe-12", SIM_SLOP);
    avt_sethashvar ("simSlope", buf);
    SIM_OUT_CAPA_VAL        = XtasSimuParams->out_capa_val;
    sprintf (buf, "%f", SIM_OUT_CAPA_VAL);
    avt_sethashvar ("simOutCapaValue", buf);
    
    /* Thresholds */
    SIM_VTH                 = XtasSimuParams->vth / 100.0;
    sprintf (buf, "%f", SIM_VTH);
    avt_sethashvar ("simVth", buf);
    SIM_VTH_HIGH            = XtasSimuParams->vth_high / 100.0;
    sprintf (buf, "%f", SIM_VTH_HIGH);
    avt_sethashvar ("simVthHigh", buf);
    SIM_VTH_LOW             = XtasSimuParams->vth_low / 100.0;
    sprintf (buf, "%f", SIM_VTH_LOW);
    avt_sethashvar ("simVthLow", buf);

}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuGetParamsValues                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int    XtasSimuGetParamsValues (void)
{
    char    *text, *ptend;
    double  value;
    Widget  text_w;


    /***** Get Technology File *****/
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuTechnoFileText");
    text = XmTextGetString (text_w);
    if (text) {
        XtasSimuSetParamString (&(XtasSimuParams->tec), text);
        XtFree (text);
    }

    /***** Get Command Line *****/
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuCmdLineText");
    text = XmTextGetString (text_w);
    if (text) {
        XtasSimuSetParamString (&(XtasSimuParams->spicestr), text);
        XtFree (text);
    }

    /***** Get output Format *****/
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuOutFormatText");
    text = XmTextGetString (text_w);
    if (text) {
        XtasSimuSetParamString (&(XtasSimuParams->spice_out), text);
        XtFree (text);
    }

    /***** Get stdout Format *****/
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuStdoutFormatText");
    text = XmTextGetString (text_w);
    if (text) {
        XtasSimuSetParamString (&(XtasSimuParams->spice_stdout), text);
        XtFree (text);
    }
    
    /***** Get Transient Values *****/
    /*  Get Time Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuTimeText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for Time.\nIt must be a positive floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->trans_time = value;
        XtFree (text);
    }

    /*  Get Step Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuStepText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value <= 0)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for Step.\nIt must be a positive floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->trans_step = value;
        XtFree (text);
    }

    /***** Get Constraints Values *****/
    /*  Get Input Start Time Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuInputStartText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for Input Start Time.\nIt must be a positive floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->input_start = value;
        XtFree (text);
    }

    /*  Get Input Slope Time Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuInputSlopeText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for Input Slope Time.\nIt must be a positive floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->input_slope = value;
        XtFree (text);
    }

    /*  Get Out Capacitance Value Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuOutCapaValText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for Output Capacitance Value.\nIt must be a positive floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->out_capa_val = value;
        XtFree (text);
    }

    /***** Get General Values *****/
    /*  Get VDD Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuVddText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0')) {
            XalDrawMessage (XtasErrorWidget, "Bad value for VDD.\nIt must be a floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->vdd = value;
        XtFree (text);
    }
    
    /*  Get VTH Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuVthText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0) || (value > 100)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for VTH.\nIt must be a floating number\nbetween 0 and 100.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->vth = value;
        XtFree (text);
    }
   
    /*  Get VTH high Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuVthhText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0) || (value > 100)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for VTH high.\nIt must be a floating number\nbetween 0 and 100.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->vth_high = value;
        XtFree (text);
    }

    /*  Get VTH low Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuVthlText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0') || (value < 0) || (value > 100)) {
            XalDrawMessage (XtasErrorWidget, "Bad value for VTH low.\nIt must be a floating number\nbetween 0 and 100.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->vth_low = value;
        XtFree (text);
    }
   
    
    /*  Get temperature Value */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuTempText");
    text = XmTextGetString (text_w);
    if (text) {
        value = strtod (text, &ptend);
        if((*ptend != '\0')) {
            XalDrawMessage (XtasErrorWidget, "Bad value for Temperature.\nMust a floating number.");
            XtFree (text);
            return 1;
        }
        XtasSimuParams->temp = value;
        XtFree (text);
    }
    
    /*  Get Simulator's options string */
    text_w = XtNameToWidget (XtasSimuParamWidget, "*XtasSimuOptionsText");
    text = XmTextGetString (text_w);
    if (text) {
        XtasSimuSetParamString (&(XtasSimuParams->spice_options), text);
        XtFree (text);
    }
    
    return  0;
    
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuPathOkCallback                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuPathOkCallback (widget, client_data, call_data)
    Widget      widget      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{

    if (XtasSimuGetParamsValues () == 0) {
        XtUnmanageChild (XtasSimuParamWidget);
        XalSetCursor( ((XtasWindowStruct*)client_data)->topwidget, WAIT );  
        XalForceUpdate (((XtasWindowStruct*)client_data)->topwidget);
        XtasSimuUpdateVariables ();
        XtasSimuPath ((XtasWindowStruct *)client_data);
    }
    else
        XtManageChild (XtasSimuParamWidget);

}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuAdvancedParamCallback                       */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuAdvancedParamCallback (widget, client_data, call_data)
    Widget      widget      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{



}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuPathParamsToolCallback                      */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuPathParamsToolCallback (widget, tool, call_data)
    Widget      widget;
    XtPointer   tool;
    XtPointer   call_data;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data;

    if (state->set) {
        XtasSimuParams->tool = (int)(long)tool;
    }
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuPathUsePrintCallback                        */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuPathUsePrintCallback (widget, client_data, call_data)
    Widget      widget;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data;

    if (state->set) 
        XtasSimuParams->use_print = SIM_YES;
    else
        XtasSimuParams->use_print = SIM_NO;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuPathUseMeasCallback                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuPathUseMeasCallback (widget, client_data, call_data)
    Widget      widget;
    XtPointer   client_data;
    XtPointer   call_data;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) call_data;

    if (state->set) 
        XtasSimuParams->use_meas = SIM_YES;
    else
        XtasSimuParams->use_meas = SIM_NO;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuSelectTechnoFileOkCallback                  */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuSelectTechnoFileOkCallback (widget, text_w, call_data)
    Widget      widget;
    XtPointer   text_w;
    XtPointer   call_data;
{
    XmFileSelectionBoxCallbackStruct *file_struct = (XmFileSelectionBoxCallbackStruct *) call_data ;
    char    *filepath   ;


    XmStringGetLtoR (file_struct->value, XmSTRING_DEFAULT_CHARSET, &filepath);
    XmTextSetString ((Widget) text_w, filepath);
    XmTextSetCursorPosition ((Widget) text_w, (XmTextPosition) strlen(filepath));
    XmTextSetTopCharacter ((Widget) text_w, (XmTextPosition) 0);

    XtUnmanageChild (widget);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                      XtasSimuSelectTechnoFileCallback                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuSelectTechnoFileCallback (widget, text_w, call_data)
    Widget      widget;
    XtPointer   text_w;     /* the TextWidget to print file path when it is selected */
    XtPointer   call_data;
{
    Arg             args[10]            ;
    int             n                   ;
    Atom            WM_DELETE_WINDOW    ;
    static Widget   file_select = NULL  ;

//    XalLeaveLimitedLoop ();


    if (!file_select) {
        n = 0;
        XtSetArg ( args[n], XmNtitle,       XTAS_NAME": Simulation Technology File"    ); n++;
        XtSetArg ( args[n], XmNminWidth,    330                                         ); n++;
        XtSetArg ( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL                  ); n++;

        file_select = XmCreateFileSelectionDialog (XtasSimuParamWidget, "XtasFileSelect", args, n);
        XtUnmanageChild (XmSelectionBoxGetChild (file_select, XmDIALOG_HELP_BUTTON));
        XtAddCallback (file_select, XmNokCallback, XtasSimuSelectTechnoFileOkCallback, (XtPointer) text_w);
        XtAddCallback (file_select, XmNcancelCallback, XtasCancelCallback, (XtPointer) file_select);


        WM_DELETE_WINDOW = XmInternAtom (XtDisplay (widget), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback (XtParent (file_select), WM_DELETE_WINDOW, 
                XtasCancelCallback, (XtPointer) file_select);
    }

    XtManageChild (file_select);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasSimuParamsInit                               */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuParamsInit  (void)
{
/*    char    *env; */

    XtasSimuParams = (xtas_simu_param_struct*) mbkalloc (sizeof (xtas_simu_param_struct));
    
    /* Techno File */
    XtasSimuParams->tec = NULL;
    if (SIM_TECHFILE) 
        XtasSimuSetParamString (&(XtasSimuParams->tec), SIM_TECHFILE);
    else
        XtasSimuParams->tec = NULL;

    /* Tool */
    XtasSimuParams->tool = V_INT_TAB[__SIM_TOOL].VALUE;
    XtasSimuParams->spicestr = NULL;
    if (SIM_SPICESTRING)
        XtasSimuSetParamString (&(XtasSimuParams->spicestr), SIM_SPICESTRING);
    else
        XtasSimuParams->spicestr = NULL;
    
    XtasSimuParams->spice_out = NULL;
    if (SIM_SPICEOUT)
        XtasSimuSetParamString (&(XtasSimuParams->spice_out), SIM_SPICEOUT);
    else
        XtasSimuParams->spice_out = NULL;
    
    XtasSimuParams->spice_stdout = NULL;
    if (SIM_SPICESTDOUT)
        XtasSimuSetParamString (&(XtasSimuParams->spice_stdout), SIM_SPICESTDOUT);
    else
        XtasSimuParams->spice_stdout = NULL;

    XtasSimuParams->use_print       = V_BOOL_TAB[__SIM_USE_PRINT].VALUE     ;
    XtasSimuParams->use_meas        = V_BOOL_TAB[__SIM_USE_MEAS].VALUE    ;
     
    /* Conditions */
    XtasSimuParams->trans_time      = V_FLOAT_TAB[ __SIM_TIME ].VALUE * 1e9          ;
    XtasSimuParams->vdd             = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE        ;
    XtasSimuParams->temp            = V_FLOAT_TAB[__SIM_TEMP].VALUE      ;
    XtasSimuParams->spice_options   = NULL              ;
    if (SIM_SPICE_OPTIONS)
        XtasSimuSetParamString (&(XtasSimuParams->spice_options), SIM_SPICE_OPTIONS);
    else 
        XtasSimuParams->spice_options = NULL            ;
    
    /* Transient */
    XtasSimuParams->trans_step      = V_FLOAT_TAB[__SIM_TRAN_STEP].VALUE*1e12;
    
    /* Input/Output Constraints */
    XtasSimuParams->input_start     = SIM_INPUT_START * (1.0e+12);
    XtasSimuParams->input_slope     = SIM_SLOP          ;
    if (SIM_OUT_CAPA_VAL > 0)
        XtasSimuParams->out_capa_val= SIM_OUT_CAPA_VAL  ;
    else
        XtasSimuParams->out_capa_val= 0.0               ;

    /* Thresholds */
    if (SIM_VTH != ELPINITTHR)
        XtasSimuParams->vth         = SIM_VTH * 100     ;
    else
        XtasSimuParams->vth         = 0.5 * 100         ;
    if (SIM_VTH_HIGH != ELPINITTHR)
        XtasSimuParams->vth_high    = SIM_VTH_HIGH * 100;
    else
        XtasSimuParams->vth_high    = 0.8 * 100         ;
    if (SIM_VTH_LOW != ELPINITTHR)
        XtasSimuParams->vth_low     = SIM_VTH_LOW * 100 ;
    else
        XtasSimuParams->vth_low     = 0.2 * 100         ;

    /* Data Extraction */
/*    if ((env = V_STR_TAB[__SIM_MEAS_CMD].VALUE) != NULL)
        XtasSimuSetParamString (&(XtasSimuParams->meas_cmd), env);
    else
        XtasSimuParams->meas_cmd = NULL;
    if ((env = V_STR_TAB[__SIM_EXTRACT_RULE].VALUE) != NULL)
        XtasSimuSetParamString (&(XtasSimuParams->extract_rule), env);
    else
        XtasSimuParams->extract_rule = NULL;*/
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                         XtasSimuPathCallback                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void    XtasSimuPathCallback (widget, client_data, call_data)
    Widget      widget      ;
    XtPointer   client_data ;
    XtPointer   call_data   ;
{
    /* Variables for motif */
    Arg         args[20];
    int         n;
    Widget      frame, frame_top;
    Widget      win_form, frame_form, form;
    Widget      label_w, text_w, row_w, toggle, button;
    XmString    motif_str;
    Pixmap      pixmap, open_pixmap, f_pixmap, r_pixmap;
    Pixel       fg, bg;
    Atom        WM_DELETE_WINDOW;
    char        buf[1024];

    /* Variables for treatment */
    XtasWindowStruct        *tas_winfos = (XtasWindowStruct *)client_data;
    XtasDetailPathSetStruct *detail_set = (XtasDetailPathSetStruct *)tas_winfos->userdata;
    chain_list              *head       = detail_set->CUR_DETAIL->DATA;
    ttvcritic_list          *critic = head->DATA;

    if (!XtasSimuParamWidget) {
        
        XtasSimuParamsInit ();

        XtasSimuParams->input_slope = tas_get_input_slope ( critic );
        
        n = 0;
        XtSetArg ( args[n], XmNtitle,       XTAS_NAME": Simulation Parameterization" ); n++;
        XtSetArg ( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
        XtSetArg ( args[n], XmNwidth,    570); n++;
        XtSetArg ( args[n], XmNmaxWidth,    570); n++;
        XtasSimuParamWidget = XmCreatePromptDialog (XtasDeskMainForm, "XtasParamBox", args, n);
        XtUnmanageChild (XmSelectionBoxGetChild (XtasSimuParamWidget, XmDIALOG_TEXT));
        XtUnmanageChild (XmSelectionBoxGetChild (XtasSimuParamWidget, XmDIALOG_PROMPT_LABEL));
        XtAddCallback (XtasSimuParamWidget, XmNokCallback, XtasSimuPathOkCallback, (XtPointer)client_data);
        XtAddCallback (XtasSimuParamWidget, XmNcancelCallback, XtasSimuCancelCallback, (XtPointer)XtasSimuParamWidget);
        XtAddCallback (XtasSimuParamWidget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_SRC_SIMUPARAM|XTAS_HELP_MAIN));
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasSimuParamWidget), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer)XtasSimuParamWidget);
        
        
        n = 0;
        win_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, XtasSimuParamWidget, args, n);

        XtVaGetValues (win_form, XmNforeground, &fg, XmNbackground, &bg, NULL);
        r_pixmap        = XalGetPixmap (win_form, XTAS_REDG_MAP, fg, bg);
        f_pixmap        = XalGetPixmap (win_form, XTAS_FEDG_MAP, fg, bg);
        open_pixmap     = XalGetPixmap (win_form, XTAS_OPEN_MAP, fg, bg);


        /***** Frame: Simulated Path *****/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
        frame_top = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);

        motif_str = XmStringCreateSimple("Simulated Path");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else   
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif        
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_top, args, n);
        XmStringFree (motif_str);

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD ); n++;
#else   
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD ); n++;
#endif
        frame_form = XtCreateManagedWidget ("XtasSubForm", xmFormWidgetClass, frame_top, args, n);


        motif_str = XmStringCreateSimple ("From:");
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNleftOffset,          5               ); n++;
        XtSetArg ( args[n], XmNheight,              30              ); n++;
        XtSetArg ( args[n], XmNwidth,               40              ); n++;
        XtSetArg ( args[n], XmNlabelString,         motif_str       ); n++;
        XtSetArg ( args[n], XmNalignment,           XmALIGNMENT_END ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_form, args, n);        
        XmStringFree (motif_str);
        
        if ((r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP))
        {
            motif_str = ((critic->SNODE & TTV_NODE_UP) == TTV_NODE_UP) ? XmStringCreateSimple("(Rising)") : XmStringCreateSimple("(Falling)"); 
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
            XtSetArg ( args[n], XmNleftWidget,      label_w                     ); n++;
            XtSetArg ( args[n], XmNwidth,           40                          ); n++;
            XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
            label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_form, args, n);
            XmStringFree (motif_str);
        }
        else
        {
            pixmap = ((critic->SNODE & TTV_NODE_UP) == TTV_NODE_UP) ? r_pixmap : f_pixmap;
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
            XtSetArg ( args[n], XmNleftWidget,      label_w                     ); n++;
            XtSetArg ( args[n], XmNwidth,           40                          ); n++;
            XtSetArg ( args[n], XmNlabelType,       XmPIXMAP                    ); n++;
            XtSetArg ( args[n], XmNlabelPixmap,     pixmap                      ); n++;
            label_w = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, frame_form, args, n);
        }

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightOffset,     2                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNleftWidget,      label_w                     ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           490                         ); n++;
        XtSetArg ( args[n], XmNvalue,           critic->NAME                ); n++;
        XtSetArg ( args[n], XmNeditable,        False                       ); n++;
        XtSetArg ( args[n], XmNeditMode,        XmMULTI_LINE_EDIT           ); n++;
        XtSetArg ( args[n], XmNresizeHeight,    True                        ); n++;
        XtSetArg ( args[n], XmNwordWrap,        True                        ); n++;
        XtSetArg ( args[n], XmNbackground,      bg                          ); n++;
        XtSetArg ( args[n], XmNbottomShadowColor,       bg                  ); n++;
        XtSetArg ( args[n], XmNbottomShadowPixmap,XmUNSPECIFIED_PIXMAP      ); n++;
        XtSetArg ( args[n], XmNtopShadowColor,          bg                  ); n++;
        XtSetArg ( args[n], XmNtopShadowPixmap, XmUNSPECIFIED_PIXMAP        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible,   False               ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmTextWidgetClass, frame_form, args, n);

        motif_str = XmStringCreateSimple ("To:");
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w             ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                   ); n++;
        XtSetArg ( args[n], XmNheight,          30                  ); n++;
        XtSetArg ( args[n], XmNwidth,           40                  ); n++;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END     ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_form, args, n);
        XmStringFree (motif_str);

        /* recherche du point d'arrivee */
        while (critic->NEXT !=NULL)
            critic=critic->NEXT;

        if ((r_pixmap == XmUNSPECIFIED_PIXMAP) || (f_pixmap == XmUNSPECIFIED_PIXMAP))
        {
            motif_str = ((critic->SNODE & TTV_NODE_UP) == TTV_NODE_UP) ? XmStringCreateSimple("(Rising)") : XmStringCreateSimple("(Falling)"); 
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
            XtSetArg ( args[n], XmNleftWidget,      label_w                     ); n++;
            XtSetArg ( args[n], XmNwidth,           40                          ); n++;
            XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
            label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_form, args, n);
            XmStringFree (motif_str);
        }
        else
        {
            pixmap = ((critic->SNODE & TTV_NODE_UP) == TTV_NODE_UP) ? r_pixmap : f_pixmap;
            n = 0;
            XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
            XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
            XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;
            XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
            XtSetArg ( args[n], XmNleftWidget,      label_w                     ); n++;
            XtSetArg ( args[n], XmNwidth,           40                          ); n++;
            XtSetArg ( args[n], XmNlabelType,       XmPIXMAP                    ); n++;
            XtSetArg ( args[n], XmNlabelPixmap,     pixmap                      ); n++;
            label_w = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, frame_form, args, n);
        }

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightOffset,     2                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNleftWidget,      label_w                     ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           490                         ); n++;
        XtSetArg ( args[n], XmNvalue,           critic->NAME                ); n++;
        XtSetArg ( args[n], XmNeditable,        False                       ); n++;
        XtSetArg ( args[n], XmNeditMode,        XmMULTI_LINE_EDIT           ); n++;
        XtSetArg ( args[n], XmNresizeHeight,    True                        ); n++;
        XtSetArg ( args[n], XmNwordWrap,        True                        ); n++;
        XtSetArg ( args[n], XmNbackground,      bg                          ); n++;
        XtSetArg ( args[n], XmNbottomShadowColor,       bg                  ); n++;
        XtSetArg ( args[n], XmNbottomShadowPixmap,XmUNSPECIFIED_PIXMAP      ); n++;
        XtSetArg ( args[n], XmNtopShadowColor,          bg                  ); n++;
        XtSetArg ( args[n], XmNtopShadowPixmap, XmUNSPECIFIED_PIXMAP        ); n++;
        XtSetArg ( args[n], XmNcursorPositionVisible, False                 ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmTextWidgetClass, frame_form, args, n);


        /***** Frame: Technology Name *****/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNtopWidget,       frame_top                   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNleftWidget,      frame_top                   ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNrightWidget,     frame_top                   ); n++;
        frame_top = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);

        motif_str = XmStringCreateSimple ("Technology File");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_top, args, n);
        XmStringFree (motif_str);

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD  ); n++;
#endif
        frame_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, frame_top, args, n);

        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                   ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNbottomOffset,        5                   ); n++;
        XtSetArg ( args[n], XmNheight,              35                  ); n++;
        XtSetArg ( args[n], XmNwidth,               40                  ); n++;
        XtSetArg ( args[n], XmNlabelType,           XmPIXMAP            ); n++;
        XtSetArg ( args[n], XmNlabelPixmap,         open_pixmap         ); n++;
        XtSetArg ( args[n], XmNpushButtonEnabled,   True                ); n++;
        XtSetArg ( args[n], XmNshadowType,          XmSHADOW_ETCHED_OUT ); n++;
        XtSetArg ( args[n], XmNshadowThickness,     2                   ); n++;
        button = XtCreateManagedWidget ("dbutton", xmDrawnButtonWidgetClass, frame_form, args, n);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,           button                      ); n++;
        XtSetArg ( args[n], XmNtopOffset,           2                           ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        button                      ); n++;        
        XtSetArg ( args[n], XmNbottomOffset,        2                           ); n++;        
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,         button                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                           ); n++;
        XtSetArg ( args[n], XmNwidth,               375                         ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuTechnoFileText", xmTextWidgetClass, frame_form, args, n);
        XmTextSetString (text_w, XtasSimuParams->tec);
        XtAddCallback(button, XmNactivateCallback, XtasSimuSelectTechnoFileCallback, (XtPointer)text_w);

        
        motif_str = XmStringCreateSimple ("Technology File:");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       text_w                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    text_w                      ); n++;    
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame_form, args, n);
        XmStringFree (motif_str);
       

        /***** Frame : Simulation Tool *****/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNtopWidget,       frame_top                   ); n++;
        XtSetArg ( args[n], XmNtopOffset,       5                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        frame = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);

        motif_str = XmStringCreateSimple ("Simulation Tool");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame, args, n);
        XmStringFree (motif_str);
        

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD  ); n++;
#endif
        frame_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, frame, args, n);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNpacking,         XmPACK_COLUMN           ); n++;
        XtSetArg ( args[n], XmNorientation,     XmVERTICAL              ); n++;
        XtSetArg ( args[n], XmNnumColumns,      2                       ); n++;
        XtSetArg ( args[n], XmNradioBehavior,   True                    ); n++;
        XtSetArg ( args[n], XmNradioAlwaysOne,  True                    ); n++;
        XtSetArg ( args[n], XmNisAligned,       True                    ); n++;
        XtSetArg ( args[n], XmNmarginHeight,    5                       ); n++;
        XtSetArg ( args[n], XmNmarginWidth,     60                      ); n++;
        XtSetArg ( args[n], XmNspacing,         25                      ); n++;
//        XtSetArg ( args[n], XmNadjustLast,      False                   ); n++;
        row_w = XtCreateManagedWidget ("XtasRow", xmRowColumnWidgetClass, frame_form, args, n);

        motif_str = XmStringCreateSimple ("Eldo");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNset,             XtasSimuParams->tool == SIM_TOOL_ELDO ? True:False ); n++;
        toggle = XtCreateManagedWidget ("XtasToggle", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree ( motif_str);
        XtAddCallback (toggle, XmNvalueChangedCallback, XtasSimuPathParamsToolCallback, (XtPointer)SIM_TOOL_ELDO);

        motif_str = XmStringCreateSimple ("NgSpice");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNset,             XtasSimuParams->tool == SIM_TOOL_NGSPICE ? True:False); n++;
        toggle = XtCreateManagedWidget ("XtasToggle", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree ( motif_str);
        XtAddCallback (toggle, XmNvalueChangedCallback, XtasSimuPathParamsToolCallback, (XtPointer)SIM_TOOL_NGSPICE);

        motif_str = XmStringCreateSimple ("LtSpice");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNset,             XtasSimuParams->tool == SIM_TOOL_LTSPICE ? True:False); n++;
        toggle = XtCreateManagedWidget ("XtasToggle", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree ( motif_str);
        XtAddCallback (toggle, XmNvalueChangedCallback, XtasSimuPathParamsToolCallback, (XtPointer)SIM_TOOL_LTSPICE);

        motif_str = XmStringCreateSimple ("MSpice");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNset,             XtasSimuParams->tool == SIM_TOOL_MSPICE ? True:False); n++;
        toggle = XtCreateManagedWidget ("XtasToggle", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree ( motif_str);
        XtAddCallback (toggle, XmNvalueChangedCallback, XtasSimuPathParamsToolCallback, (XtPointer)SIM_TOOL_MSPICE);

        motif_str = XmStringCreateSimple ("HSpice");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNset,             XtasSimuParams->tool == SIM_TOOL_HSPICE ? True:False); n++;
        toggle = XtCreateManagedWidget ("XtasToggle", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree ( motif_str);
        XtAddCallback (toggle, XmNvalueChangedCallback, XtasSimuPathParamsToolCallback, (XtPointer)SIM_TOOL_HSPICE);

        motif_str = XmStringCreateSimple ("Other");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNset,             ((XtasSimuParams->tool == SIM_TOOL_TITAN)||(XtasSimuParams->tool == SIM_TOOL_TITAN)) ? True:False); n++;
        toggle = XtCreateManagedWidget ("XtasToggle", xmToggleButtonWidgetClass, row_w, args, n);
        XmStringFree ( motif_str);
        XtAddCallback (toggle, XmNvalueChangedCallback, XtasSimuPathParamsToolCallback, (XtPointer)V_INT_TAB[__SIM_TOOL].VALUE);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           row_w               ); n++;
        XtSetArg ( args[n], XmNtopOffset,           5                   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_FORM       ); n++;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, frame_form, args, n);

        motif_str = XmStringCreateSimple ("Command line:");
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNleftOffset,          5                   ); n++;
        XtSetArg ( args[n], XmNlabelString,         motif_str           ); n++;
        XtSetArg ( args[n], XmNheight,              30                  ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n); 
        XmStringFree (motif_str);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w             ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        label_w             ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNleftWidget,          label_w             ); n++;
        XtSetArg ( args[n], XmNleftOffset,          10                  ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                   ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuCmdLineText", xmTextWidgetClass, form, args, n); 
        XmTextSetString (text_w, XtasSimuParams->spicestr);
        
        motif_str = XmStringCreateSimple ("Output File Format:");
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w             ); n++;
        XtSetArg ( args[n], XmNtopOffset,           5                   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNleftOffset,          5                   ); n++;
        XtSetArg ( args[n], XmNlabelString,         motif_str           ); n++;
        XtSetArg ( args[n], XmNheight,              30                  ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n); 
        XmStringFree (motif_str);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w             ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        label_w             ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNleftWidget,          label_w             ); n++;
        XtSetArg ( args[n], XmNleftOffset,          10                  ); n++;
        XtSetArg ( args[n], XmNwidth,               50                  ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuOutFormatText", xmTextWidgetClass, form, args, n); 
        XmTextSetString (text_w, XtasSimuParams->spice_out);
        
        motif_str = XmStringCreateSimple ("Stdout Redirection File Format:");
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w             ); n++;
        XtSetArg ( args[n], XmNtopOffset,           5                   ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM       ); n++;
        XtSetArg ( args[n], XmNleftOffset,          5                   ); n++;
        XtSetArg ( args[n], XmNlabelString,         motif_str           ); n++;
        XtSetArg ( args[n], XmNheight,              30                  ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n); 
        XmStringFree (motif_str);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w             ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        label_w             ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_WIDGET     ); n++;
        XtSetArg ( args[n], XmNleftWidget,          label_w             ); n++;
        XtSetArg ( args[n], XmNleftOffset,          10                  ); n++;
        XtSetArg ( args[n], XmNwidth,               50                  ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuStdoutFormatText", xmTextWidgetClass, form, args, n); 
        XmTextSetString (text_w, XtasSimuParams->spice_stdout);
       
        motif_str = XmStringCreateSimple( "Use Print" ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, form,
                                          XmNset,           (XtasSimuParams->use_print == SIM_YES) ? True:False,
                                          XmNtopAttachment, XmATTACH_WIDGET,
                                          XmNtopWidget,     label_w,
                                          XmNtopOffset,     5,
                                          XmNleftAttachment,XmATTACH_FORM,
                                          XmNleftOffset,    5,
                                          XmNheight,        20,
                                          XmNlabelString,   motif_str,
                                          NULL) ;
        XmStringFree( motif_str ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasSimuPathUsePrintCallback, NULL) ;
       
        motif_str = XmStringCreateSimple( "Use Measure" ) ;
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, form,
                                          XmNset,           (XtasSimuParams->use_meas == SIM_YES) ? True:False,
                                          XmNtopAttachment, XmATTACH_WIDGET,
                                          XmNtopWidget,     toggle,
                                          XmNtopOffset,     5,
                                          XmNleftAttachment,XmATTACH_FORM,
                                          XmNleftOffset,    5,
                                          XmNbottomAttachment,XmATTACH_FORM,
                                          XmNbottomOffset,  5,
                                          XmNheight,        20,
                                          XmNlabelString,   motif_str,
                                          NULL) ;
        XmStringFree( motif_str ) ;
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasSimuPathUseMeasCallback, NULL) ;
       

        /***** Frame: Input/Output Constraints *****/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNtopWidget,           frame                       ); n++;
        XtSetArg ( args[n], XmNtopOffset,           5                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNleftWidget,          frame                       ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNrightWidget,         frame                       ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_FORM               ); n++;
        frame = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);


        motif_str = XmStringCreateSimple ("Input/Output Constraints");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame, args, n);
        XmStringFree (motif_str);
        
        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD  ); n++;
#endif
        XtSetArg ( args[n], XmNpacking,         XmPACK_COLUMN           ); n++;
        XtSetArg ( args[n], XmNorientation,     XmVERTICAL              ); n++;
        XtSetArg ( args[n], XmNnumColumns,      1                       ); n++;
        XtSetArg ( args[n], XmNisAligned,       True                    ); n++;
        XtSetArg ( args[n], XmNmarginHeight,    5                       ); n++;
        XtSetArg ( args[n], XmNmarginWidth,     17                      ); n++;
        XtSetArg ( args[n], XmNspacing,         5                       ); n++;
        row_w = XtCreateManagedWidget ("XtasRow", xmRowColumnWidgetClass, frame, args, n);

        /* FORM: Input Start Time */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("ps");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           3*7                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuInputStartText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->input_start);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("Input Start Time =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       text_w                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    text_w                      ); n++;    
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        /* FORM: Input Slope Time */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("ps");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           3*7                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuInputSlopeText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->input_slope);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("Input Slope Time =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       text_w                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    text_w                      ); n++;    
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        /* FORM: Input Slope Time */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("fF");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           3*7                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuOutCapaValText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->out_capa_val);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("Output Capacitance Value =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       text_w                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    text_w                      ); n++;    
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        

        /***** Frame : Step *****/
        n = 0;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNleftWidget,      frame                       ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    frame                       ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        frame = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);

        motif_str = XmStringCreateSimple ("Step");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame, args, n);
        XmStringFree (motif_str);
        

        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD  ); n++;
#endif
        XtSetArg ( args[n], XmNpacking,         XmPACK_COLUMN           ); n++;
        XtSetArg ( args[n], XmNorientation,     XmVERTICAL              ); n++;
        XtSetArg ( args[n], XmNnumColumns,      1                       ); n++;
        XtSetArg ( args[n], XmNisAligned,       True                    ); n++;
        XtSetArg ( args[n], XmNmarginHeight,    5                       ); n++;
        XtSetArg ( args[n], XmNmarginWidth,     25                      ); n++;
        XtSetArg ( args[n], XmNspacing,         5                       ); n++;
        row_w = XtCreateManagedWidget ("XtasRow", xmRowColumnWidgetClass, frame, args, n);



        /* FORM: Step */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);
        
        motif_str = XmStringCreateSimple ("ps");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str              ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM          ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM          ); n++;
        XtSetArg ( args[n], XmNrightOffset,     24                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING  ); n++;
        XtSetArg ( args[n], XmNheight,          30                     ); n++;
        XtSetArg ( args[n], XmNwidth,           3*7                    ); n++; /* 7 = largeur moyenne d'un caractere */
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        label_w                     ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,         label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                           ); n++;
        XtSetArg ( args[n], XmNwidth,               70                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuStepText", xmTextWidgetClass, form, args, n);
        sprintf (buf , "%.3f", XtasSimuParams->trans_step);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("Step =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,         motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,           text_w                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        text_w                      ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,         text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNalignment,           XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);

        
         /***** Frame: Thresholds *****/
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNrightWidget,         frame                       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNleftWidget,          frame                       ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        frame                       ); n++;
        XtSetArg ( args[n], XmNbottomOffset,        5                           ); n++;
        frame = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);

        motif_str = XmStringCreateSimple ("Thresholds");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame, args, n);
        XmStringFree (motif_str);
        
        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD  ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD  ); n++;
#endif
        XtSetArg ( args[n], XmNpacking,         XmPACK_COLUMN           ); n++;
        XtSetArg ( args[n], XmNorientation,     XmVERTICAL              ); n++;
        XtSetArg ( args[n], XmNnumColumns,      1                       ); n++;
        XtSetArg ( args[n], XmNisAligned,       True                    ); n++;
        XtSetArg ( args[n], XmNmarginHeight,    10                      ); n++;
        XtSetArg ( args[n], XmNmarginWidth,     17                      ); n++;
        XtSetArg ( args[n], XmNspacing,         5                       ); n++;
        row_w = XtCreateManagedWidget ("XtasRow", xmRowColumnWidgetClass, frame, args, n);

        
        /* FORM: VTH */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("%");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightOffset,     28                      ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           15                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuVthText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->vth);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("VTH =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;    
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        /* FORM: VTHh */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("%");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightOffset,     28                      ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           15                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuVthhText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->vth_high);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("VTH high =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++; 
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        /* FORM: VTHl */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("%");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightOffset,     28                      ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           15                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuVthlText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->vth_low);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("VTH low =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;     
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        

        
        /***** Frame: Conditions *****/
        n = 0;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNrightWidget,         frame                       ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNleftWidget,          frame                       ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        frame                       ); n++;
        XtSetArg ( args[n], XmNbottomOffset,        5                           ); n++;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNtopWidget,           frame_top                   ); n++;
        XtSetArg ( args[n], XmNtopOffset,           5                           ); n++;
        frame = XtCreateManagedWidget ("XtasFrame", xmFrameWidgetClass, win_form, args, n);

        motif_str = XmStringCreateSimple ("Conditions");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str           ); n++;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_TITLE_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_TITLE_CHILD ); n++;
#endif
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, frame, args, n);
        XmStringFree (motif_str);
        
        /* FORM: Time */
        n = 0;
#if XTAS_MOTIF_VERSION >= 20000
        XtSetArg ( args[n], XmNframeChildType,  XmFRAME_WORKAREA_CHILD ); n++;
#else
        XtSetArg ( args[n], XmNchildType,       XmFRAME_WORKAREA_CHILD ); n++;
#endif
        frame_form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, frame, args, n);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNpacking,         XmPACK_COLUMN           ); n++;
        XtSetArg ( args[n], XmNorientation,     XmVERTICAL              ); n++;
        XtSetArg ( args[n], XmNnumColumns,      1                       ); n++;
        XtSetArg ( args[n], XmNisAligned,       True                    ); n++;
        XtSetArg ( args[n], XmNmarginHeight,    10                      ); n++;
        XtSetArg ( args[n], XmNmarginWidth,     5                       ); n++;
        XtSetArg ( args[n], XmNspacing,         5                       ); n++;
        row_w = XtCreateManagedWidget ("XtasRow", xmRowColumnWidgetClass, frame_form, args, n);

        /* FORM: Time */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);
        
        motif_str = XmStringCreateSimple ("ns");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str              ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM          ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM          ); n++;
        XtSetArg ( args[n], XmNrightOffset,     24                     ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING  ); n++;
        XtSetArg ( args[n], XmNheight,          30                     ); n++;
        XtSetArg ( args[n], XmNwidth,           3*7                    ); n++; /* 7 = largeur moyenne d'un caractere */
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,           label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        label_w                     ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,         label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                           ); n++;
        XtSetArg ( args[n], XmNwidth,               60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuTimeText", xmTextWidgetClass, form, args, n);
        sprintf (buf , "%.2f", XtasSimuParams->trans_time);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("Time =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,         motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,       XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,           text_w                      ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,    XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,        text_w                      ); n++;
        XtSetArg ( args[n], XmNrightAttachment,     XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,         text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,         5                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,      XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNalignment,           XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        /* FORM: VDD */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("V");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightOffset,     30                      ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           15                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuVddText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->vdd);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("VDD =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;    
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);

        /* FORM: TEMP */
        n = 0;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, row_w, args, n);

        motif_str = XmStringCreateSimple ("°C");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM           ); n++;
        XtSetArg ( args[n], XmNrightOffset,     30                      ); n++;
        XtSetArg ( args[n], XmNheight,          30                      ); n++;
        XtSetArg ( args[n], XmNwidth,           15                      ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING   ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);
        
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++;        
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     label_w                     ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNwidth,           60                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuTempText", xmTextWidgetClass, form, args, n);
        sprintf (buf, "%.2f", XtasSimuParams->temp);
        XmTextSetString (text_w, buf);

        motif_str = XmStringCreateSimple ("Temperature =");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_OPPOSITE_WIDGET    ); n++;
        XtSetArg ( args[n], XmNbottomWidget,    label_w                     ); n++; 
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNrightWidget,     text_w                      ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_END             ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);

        /* FORM: OPTIONS1 */
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET ); n++;
        XtSetArg ( args[n], XmNtopWidget,       row_w           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNbottomAttachment,XmATTACH_FORM   ); n++;
        XtSetArg ( args[n], XmNbottomOffset,    5               ); n++;
        form = XtCreateManagedWidget ("XtasForm", xmFormWidgetClass, frame_form, args, n);       
       
        motif_str = XmStringCreateSimple ("Simulator's options:");
        n = 0;
        XtSetArg ( args[n], XmNlabelString,     motif_str                   ); n++;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNalignment,       XmALIGNMENT_BEGINNING       ); n++;
        label_w = XtCreateManagedWidget ("XtasLabels", xmLabelWidgetClass, form, args, n);
        XmStringFree (motif_str);

        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET             ); n++;
        XtSetArg ( args[n], XmNtopWidget,       label_w                     ); n++;
        XtSetArg ( args[n], XmNrightAttachment, XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNrightOffset,     5                           ); n++;
        XtSetArg ( args[n], XmNleftAttachment,  XmATTACH_FORM               ); n++;
        XtSetArg ( args[n], XmNleftOffset,      5                           ); n++;
        XtSetArg ( args[n], XmNheight,          30                          ); n++;
        text_w = XtCreateManagedWidget ("XtasSimuOptionsText", xmTextWidgetClass, form, args, n);
        XmTextSetString (text_w, XtasSimuParams->spice_options);
          
    }

    XtManageChild (XtasSimuParamWidget);
//    XalLimitedLoop (XtasSimuParamWidget);
    

    
}


