/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_tas.c                                                  */
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
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

#define  TMA_PS     'P'
#define  TMA_NS     'N'
#define  TMA_FF     'F'
#define  TMA_PF     'P'

#define  SCM_INT    'S'
#define  SCM_CPL    's'
#define  LUT_INT    'L'
#define  LUT_CPL    'l'
#define  PLY_INT    'P'
#define  PLY_CPL    'p'
#define  NO_MODEL   'n'

#define  PCH        'a'
#define  F          'b'
#define  COUT       'c'
#define  CTC        'd'
#define  OPC        'e'
#define  SLOPE      'f'
#define  SWC        'g'
#define  NM         'h'
#define  Y          'i'
#define  Q          'j'
#define  STR        'k'

#define  MOD_R      'R'
#define  MOD_W      'W'
#define  MOD_T      'T'

#define  XTAS_SHARE_CMD          0
#define  XTAS_FF_DETECT          1
#define  XTAS_SLAVE_FF           2
#define  XTAS_LATCH_LIB          3
#define  XTAS_AUTO_LATCH_LOOP    4
#define  XTAS_RM_PARA_TRANS      5
#define  XTAS__S_CONV            6
#define  XTAS_SIMPLE_OR          7
#define  XTAS_TRANS_RECOG        8
#define  XTAS_ONLY_TRANS_RECOG   9
#define  XTAS_HIER_RECOG        10
#define  XTAS_ONLY_HIER_RECOG   11
#define  XTAS_FUNC_ANA          12
#define  XTAS_HZ_NODES          13

Widget              xtas_tas_file_select=NULL;

extern  void        Xyagavt_gethashvar(void);
static  char        TOOL = NO_TOOL;
static  Pixel       tas_state_color, tas_backg_color;
char               *tas_format_type[] = {"Spice", "VHDL", "Verilog"};
char               *tas_format_sfx[] = {"spi","vhd","vlg"};
char               *tma_format_type[] = {"Spice","VHDL","Verilog","General Perfmodule (.ttx)", "Detailed Perfmodule (.dtx)"};
char               *tma_format_sfx[] = {"spi","ttx","dtx"};
//char                tma_beh_mode = 'A';
Widget              xtas_winput_netlist_file;
Widget              xtas_techno_form;
Widget              xtas_winput_techno_file;
Widget              xtas_winput_filein_file;
Widget              xtas_winput_readarea_file;
Widget              xtas_woutput_sfx;
Widget              xtas_woutput_name;
Widget              xtas_wopt_mrg;
Widget              xtas_woutput_file_name;
static Widget tas_text_w[5]; /* 0=cout, 1=opc, 2=slope, 3=swc, 4=p */
static Widget tas_form_w[5]; /* 0=cout, 1=opc, 2=slope, 3=swc, 4=p */
static Widget tas_form;
static char *tas_opt_output[] = { 
                                 "General Perfmodule (.ttx)",
                                 "Detailed Perfmodule (.dtx)",
                                 "Disassembled File (.cns)",
                                 "Slopes File (.slo)",
                                 "Silent Mode (.tou .ter)",
                                };
static char *tas_opt_inter[]   = { 
                               /*"Ignore Interconnect Resistance",
                               "Break Loop In The Interconnect",
                               "Save Interconnect in RC net Form"*/
                               "calculates RC delays",
                               "merge RC and gate delays",
                               "breaks loops in RC networks"
                               };
static char *tas_opt_carac[]    = { 
                               "scm models of interface delays",
                               "scm models of all delays",
                               "lookup-tables of interface delays",
                               "lookup-tables of all delays",
                               };


static  Widget  session_widget = NULL;
static  Widget  session_options_widget = NULL;


xtas_tas_param_struct *tas_tmppar = NULL;
xtas_tma_param_struct *tma_tmppar = NULL;

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaInit                                                    */
/*                                                                           */
/* IN  ARGS : .fd : The file descriptor.                                     */
/*            .on : Indicator of a blocking read or not.                     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int XtasTmaInit()
{
//    strcpy(tma_tmppar->inpath,""); 
//    
//    if(WORK_LIB)
//        strcpy(tma_tmppar->outpath, WORK_LIB);
//    else
//        strcpy(tma_tmppar->outpath,"");
//
//    if(TMA_CONTEXT->TMA_NAMEIN)
//        strcpy(tma_tmppar->namein,TMA_CONTEXT->TMA_NAMEIN);
//    else
//        strcpy(tma_tmppar->namein,"");
//    
//    if(TMA_CONTEXT->TMA_LIBRARY)
//        strcpy(tma_tmppar->nameout,TMA_CONTEXT->TMA_LIBRARY); 
//    else
//        strcpy(tma_tmppar->nameout,""); 
//    
//    if (!strcmp(IN_LO,"ttx")) {
//        strcpy(tma_tmppar->in,"ttx");
//        strcpy(tma_tmppar->ext,"ttx");
//    }
//    else if(!strcmp(IN_LO,"dtx")) {
//        strcpy(tma_tmppar->in,"dtx");
//        strcpy(tma_tmppar->ext,"dtx");
//    }
//    else if (!strcmp(IN_LO,"vhd")) {
//        strcpy(tma_tmppar->in,"vhd");
//        strcpy(tma_tmppar->ext,vhd_sfx);
//    }
//    else if(!strcmp(IN_LO,"vlg") || !strcmp(IN_LO,"v")) {
//        strcpy(tma_tmppar->in,"vlg");
//        strcpy(tma_tmppar->ext,vlg_sfx);
//    }
//    else {
//        strcpy(tma_tmppar->in,"spi");
//        strcpy(tma_tmppar->ext,spi_sfx);
//    }
//        
//
//    tma_tmppar->ttxin    = TMA_CONTEXT->TMA_TTXIN; 
//    if(tma_tmppar->ttxin == 'Y') {
//        strcpy(tma_tmppar->in, "ttx");
//        strcpy(tma_tmppar->ext,"ttx");
//    }
//    tma_tmppar->dtxin    = TMA_CONTEXT->TMA_DTXIN;
//    if(tma_tmppar->dtxin == 'Y') {
//        if(tma_tmppar->ttxin == 'Y') {
//            XalDrawMessage(XtasWarningWidget,XTAS_CONFERR);
//            return (1);
//        }
//        else {
//            strcpy(tma_tmppar->in, "dtx");
//            strcpy(tma_tmppar->ext,"dtx");
//        }
//    }
//    
//
//    tma_tmppar->out      = TMA_CONTEXT->TMA_OUTPUT;
//    tma_tmppar->otock    = TMA_CONTEXT->TMA_AUTOCLOCK; 
//    switch(TMA_CONTEXT->TMA_MODE) {
//        case 'T' :
//        case 'R' :
//        case 'W' : tma_tmppar->mod = TMA_CONTEXT->TMA_MODE;
//                   break;
//        default  : tma_tmppar->mod = 'T';
//                   break;
//    }
////    strcpy(tma_tmppar->fin,"");
//    
//    if(TMA_CONTEXT->TMA_MARGIN > 0)
//        sprintf(tma_tmppar->mrg,"%ld",TMA_CONTEXT->TMA_MARGIN);
//    else
//        strcpy(tma_tmppar->mrg, "");
//    
//    tma_tmppar->v        = TMA_CONTEXT->TMA_VERBOSE;
//    tma_tmppar->i        = 'N';
//    
//    if(TMA_CONTEXT->TMA_TUNIT == namealloc("ns"))
//        tma_tmppar->tu       = TMA_NS;
//    else
//        tma_tmppar->tu       = TMA_PS;
//        
//    if(TMA_CONTEXT->TMA_CUNIT == namealloc("pf"))
//        tma_tmppar->cu       = TMA_PF;
//    else
//        tma_tmppar->cu       = TMA_FF;
////    strcpy(tma_tmppar->ra,"");
//    return (0);
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasInit                                                    */
/*                                                                           */
/* IN  ARGS : NONE                                                           */ 
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTasInit()
{
    strcpy(tas_tmppar->inpath,"");

    if (elpTechnoFile)
        strcpy(tas_tmppar->tec,elpTechnoFile);
    else
        strcpy(tas_tmppar->tec,"");

    if (TAS_CONTEXT->TAS_FILENAME)
        strcpy(tas_tmppar->infile,TAS_CONTEXT->TAS_FILENAME);
    else
        strcpy(tas_tmppar->infile,"");
    
    if(TAS_CONTEXT->TAS_FILEOUT)    
        strcpy(tas_tmppar->outfile,TAS_CONTEXT->TAS_FILEOUT);
    else
        strcpy(tas_tmppar->outfile,"");
   
    if(WORK_LIB)
        strcpy(tas_tmppar->outpath,WORK_LIB); 
    else
        strcpy(tas_tmppar->outpath,""); 
    tas_tmppar->i        = 'N'; 
    
    tas_tmppar->pch      = TAS_CONTEXT->TAS_TREATPRECH; 
    tas_tmppar->f        = TAS_CONTEXT->TAS_FACTORISE;
    
    if (TAS_CONTEXT->TAS_CAPAOUT > 0) {
        tas_tmppar->use_cout = 'Y';
        sprintf(tas_tmppar->cout, "%.2f", TAS_CONTEXT->TAS_CAPAOUT);
    }
    else {
        tas_tmppar->use_cout = 'N';
        strcpy(tas_tmppar->cout,"");
    }
    
    if (TAS_CONTEXT->TAS_CAPARAPREC > 0) {
        tas_tmppar->use_opc  = 'Y';
        sprintf(tas_tmppar->opc, "%.2f", (TAS_CONTEXT->TAS_CAPARAPREC*100));
    }
    else {
        tas_tmppar->use_opc = 'N';
        strcpy(tas_tmppar->opc,"");
    }
    
    if ((TAS_CONTEXT->FRONT_CON != TAS_NOFRONT) && (TAS_CONTEXT->FRONT_CON > 0)) {
        tas_tmppar->use_slope = 'Y';
        sprintf(tas_tmppar->slope, "%.2f",TAS_CONTEXT->FRONT_CON);
    }
    else {
        tas_tmppar->use_slope = 'N';
        strcpy(tas_tmppar->slope,"");
    }
    
    if(TAS_CONTEXT->TAS_CAPASWITCH > 0) {
        tas_tmppar->use_swc = 'Y';
        sprintf(tas_tmppar->swc, "%.2f", TAS_CONTEXT->TAS_CAPASWITCH*100);
    }
    else {
        tas_tmppar->use_swc = 'N';
        strcpy(tas_tmppar->swc,"");
    }
    
//    tas_tmppar->nm       = 'N';
    tas_tmppar->y        = TAS_CONTEXT->TAS_IGNBLACKB;
    tas_tmppar->q        = TAS_CONTEXT->TAS_FLATCELLS;

    tas_tmppar->mg   = TAS_CONTEXT->TAS_MERGERCN;
    tas_tmppar->rc   = TAS_CONTEXT->TAS_CALCRCN;
    tas_tmppar->bk   = TAS_CONTEXT->TAS_BREAKLOOP;

    if(TAS_CONTEXT->TAS_CARAC == 'Y') {
        switch (TAS_CONTEXT->TAS_CARACMODE) {
            case TAS_LUT_INT    : tas_tmppar->carac = LUT_INT;
                                  break;
            case TAS_LUT_CPL    : tas_tmppar->carac = LUT_CPL;
                                  break;
            case TAS_SCM_INT    : tas_tmppar->carac = SCM_INT;
                                  break;
            case TAS_SCM_CPL    : tas_tmppar->carac = SCM_CPL;
                                  break;
            case TAS_PLY_INT    : tas_tmppar->carac = PLY_INT;
                                  break;
            case TAS_PLY_CPL    : tas_tmppar->carac = PLY_CPL;
                                  break;
            default             : tas_tmppar->carac = NO_MODEL;
                                  break;
        }
    }
    else
        tas_tmppar->carac = NO_MODEL;
    
    if (YAG_CONTEXT->YAG_ONLY_FCL == TRUE)                                  
        tas_tmppar->xfcl = 'Y';
    else 
        tas_tmppar->xfcl = 'N';
    if (YAG_CONTEXT->YAG_FCL_DETECT == TRUE)                                  
        tas_tmppar->fcl  = 'Y';
    else
        tas_tmppar->fcl  = 'N';
    
    
    if (YAG_CONTEXT->YAG_ONLY_GENIUS == TRUE)                                  
        tas_tmppar->xg = 'Y';
    else 
        tas_tmppar->xg = 'N';
    if (YAG_CONTEXT->YAG_GENIUS == TRUE)                                  
        tas_tmppar->gns  = 'Y';
    else
        tas_tmppar->gns  = 'N';
         
    if (YAG_CONTEXT->YAG_DETECT_LATCHES == FALSE)
        tas_tmppar->nl   = 'Y';
    else
        tas_tmppar->nl   = 'N';

    if (YAG_CONTEXT->YAG_CELL_SHARE == TRUE)
        tas_tmppar->cl   = 'Y';
    else
        tas_tmppar->cl   = 'N';
    
    if (YAG_CONTEXT->YAG_MAKE_CELLS == TRUE)
        tas_tmppar->fl   = 'Y';
    else
        tas_tmppar->fl   = 'N';

    if (YAG_CONTEXT->YAG_REMOVE_PARA == TRUE)    
        tas_tmppar->rpt  = 'Y';
    else
        tas_tmppar->rpt  = 'N';

    if (YAG_CONTEXT->YAG_LOOP_ANALYSIS == TRUE)
        tas_tmppar->la   = 'Y';
    else
        tas_tmppar->la   = 'N';
    
    if (YAG_CONTEXT->YAG_HELP_S == TRUE)
        tas_tmppar->o    = 'Y';
    else
        tas_tmppar->o    = 'N';
    
    if (YAG_CONTEXT->YAG_PROP_HZ == TRUE)
        tas_tmppar->z    = 'Y';
    else
        tas_tmppar->z    = 'N';
    
    if (YAG_CONTEXT->YAG_ORIENT == TRUE)
        tas_tmppar->b    = 'Y';    
    else
        tas_tmppar->b    = 'N';    
   
    if (YAG_CONTEXT->YAG_USE_FCF == TRUE) 
        sprintf(tas_tmppar->p,"%d",YAG_CONTEXT->YAG_DEPTH);
    else    
        strcpy(tas_tmppar->p,"");

    if (TAS_CONTEXT->TAS_INT_END == 'Y')
        tas_tmppar->ttx = 'N';
    else
        tas_tmppar->ttx = 'Y';
    
    tas_tmppar->dtx = TAS_CONTEXT->TAS_PERFINT;
    tas_tmppar->c = TAS_CONTEXT->TAS_CNS_FILE;
    tas_tmppar->e = TAS_CONTEXT->TAS_SLOFILE;
    tas_tmppar->s = TAS_CONTEXT->TAS_SILENTMODE;
    tas_tmppar->hr = TAS_CONTEXT->TAS_HIER;
    tas_tmppar->min = TAS_CONTEXT->TAS_FIND_MIN;
    tas_tmppar->stb = TAS_CONTEXT->TAS_STABILITY;

    if (!strcmp(IN_LO,"vhd")) {
        strcpy(tas_tmppar->in,"vhd");
        strcpy(tas_tmppar->ext,vhd_sfx);
    }
    else if(!strcmp(IN_LO,"vlg") || !strcmp(IN_LO,"v")) {
        strcpy(tas_tmppar->in,"vlg");
        strcpy(tas_tmppar->ext,vlg_sfx);
    }
    else {
        strcpy(tas_tmppar->in,"spi");
        strcpy(tas_tmppar->ext,spi_sfx);
    }


}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCleanStates                                                */
/*                                                                           */
/* IN  ARGS : .fd : The file descriptor.                                     */
/*            .on : Indicator of a blocking read or not.                     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   :                                                                */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCleanStates()
{
Arg args[10], argx[10];

XtSetArg( args[0], XmNforeground, tas_state_color );
XtSetArg( argx[0], XmNbackground, tas_backg_color );

}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasGeneralOptionsCallback                                  */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTasGeneralOptionsCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    switch((char)(long)type) {
        case 'T': tas_tmppar->hr  = ( state->set ) ? 'N':'Y'; break;
        case 'H': tas_tmppar->hr  = ( state->set ) ? 'Y':'N'; break;
        case 'L': tas_tmppar->min = ( state->set ) ? 'N':'Y'; break;
        case 'S': tas_tmppar->min = ( state->set ) ? 'Y':'N'; break;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOutputFileFormatCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOutputFileFormatCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if ( state->set )
    {
        switch((int)(long)type) {
            case 0: tas_tmppar->ttx = 'Y'; break;
            case 1: tas_tmppar->dtx = 'Y'; break;
            case 2: tas_tmppar->c   = 'Y'; break;
            case 3: tas_tmppar->e   = 'Y'; break;
            case 4: tas_tmppar->s   = 'Y'; break;
            default: break;
        }
    }
    else {
        switch((int)(long)type) {
            case 0: tas_tmppar->ttx = 'N'; break;
            case 1: tas_tmppar->dtx = 'N'; break;
            case 2: tas_tmppar->c   = 'N'; break;
            case 3: tas_tmppar->e   = 'N'; break;
            case 4: tas_tmppar->s   = 'N'; break;
            default: break;
        }
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaUnitCallback                                            */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTmaUnitCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    switch((char)(long)type) {
        case 's': tma_tmppar->tu  = ( state->set ) ? TMA_PS:TMA_NS; break;
        case 'S': tma_tmppar->tu  = ( state->set ) ? TMA_NS:TMA_PS; break;
        case 'f': tma_tmppar->cu  = ( state->set ) ? TMA_PF:TMA_FF; break;
        case 'F': tma_tmppar->cu  = ( state->set ) ? TMA_FF:TMA_PF; break;
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNetTypeCallback                                            */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNetTypeCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    Widget  dialog_widget;
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if ( state->set )
    {
     if (TOOL == TAS_TOOL) {
         switch((int)(long)type) {
             case 0 :  strcpy( tas_tmppar->in, "spi");
                       strcpy( tas_tmppar->ext, spi_sfx);
                       break;
             case 1 :  strcpy( tas_tmppar->in, "vhd");
                       strcpy( tas_tmppar->ext, vhd_sfx);
                       break;
             case 2 :  strcpy( tas_tmppar->in, "vlg");
                       strcpy( tas_tmppar->ext, vlg_sfx);
                       break;
         }
     }
     else {
         dialog_widget = widget;
         while (dialog_widget && (XmIsMessageBox (dialog_widget) == 0))
             dialog_widget = XtParent (dialog_widget);
         
         switch((int)(long)type) {
             case 0 :   tma_tmppar->ttxin = 'N';
                        tma_tmppar->dtxin = 'N';
                        strcpy( tma_tmppar->in, "spi");
                        strcpy( tma_tmppar->ext, spi_sfx);
                        XtSetSensitive(tas_form,True);
                        XtSetSensitive(XmSelectionBoxGetChild(dialog_widget, XmDIALOG_CANCEL_BUTTON),True);
                        break;
             case 1 :   tma_tmppar->ttxin = 'N';
                        tma_tmppar->dtxin = 'N';
                        strcpy( tma_tmppar->in, "vhd");
                        strcpy( tma_tmppar->ext, vhd_sfx);
                        XtSetSensitive(tas_form,True);
                        XtSetSensitive(XmSelectionBoxGetChild(dialog_widget, XmDIALOG_CANCEL_BUTTON),True);
                        break;
             case 2 :   tma_tmppar->ttxin = 'N';
                        tma_tmppar->dtxin = 'N';
                        strcpy( tma_tmppar->in, "vlg");
                        strcpy( tma_tmppar->ext, vlg_sfx);
                        XtSetSensitive(tas_form,True);
                        XtSetSensitive(XmSelectionBoxGetChild(dialog_widget, XmDIALOG_CANCEL_BUTTON),True);
                        break;
             case 3 :   tma_tmppar->ttxin = 'Y';
                        tma_tmppar->dtxin = 'N';
                        strcpy( tma_tmppar->in, "ttx");
                        strcpy( tma_tmppar->ext,"ttx");
                        XtSetSensitive(tas_form,False);
                        XtSetSensitive(XmSelectionBoxGetChild(dialog_widget, XmDIALOG_CANCEL_BUTTON),False);
                        break;
             case 4 :   tma_tmppar->ttxin = 'N';
                        tma_tmppar->dtxin = 'Y';
                        strcpy( tma_tmppar->in, "dtx");
                        strcpy( tma_tmppar->ext,"dtx");
                        XtSetSensitive(tas_form,False);
                        XtSetSensitive(XmSelectionBoxGetChild(dialog_widget, XmDIALOG_CANCEL_BUTTON),False);
                        break;
         }
     }
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetFileNameFromFilePath                                    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char *XtasGetFileNameFromFilePath( char *filepath )
{
    char *start;
    
    if((start = strrchr(filepath, '/')) != NULL) {
        *start='\0';
        return start+1;
    }
    else return filepath;

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSessionFileOkCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .file_type : Filetype we are working on.                       */
/*            .file_struct : Informations on the file widget.                */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/Open command when the OK    */
/*            button has been pushed.                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSessionFileOkCallback( widget, file_type, cbs )
Widget widget;
XtPointer file_type;
XtPointer cbs;
{
    XmFileSelectionBoxCallbackStruct * file_struct = (XmFileSelectionBoxCallbackStruct *)cbs ;
    char *filepath, *filename;
    char *p;
    int type;
    /*-----------------------------------------------------------------------*/
    /* Get the filename selected by the user. Don't forget to check out that */
    /* it isn't a NULL string.                                               */
    /*-----------------------------------------------------------------------*/
    XmStringGetLtoR( file_struct->value, XmSTRING_DEFAULT_CHARSET, &filepath );
    type = *((int *) file_type);
    
    switch(type) {
        case XTAS_NETLIST_FILE  :   filename = XtasGetFileNameFromFilePath(filepath);
                                    if(*filepath != '/') {
                                        if(TOOL == TAS_TOOL)
                                            strcpy(tas_tmppar->inpath, ".");
                                        else
                                            strcpy(tma_tmppar->inpath, ".");
                                    }
                                    else {
                                        if(TOOL == TAS_TOOL)
                                            strcpy(tas_tmppar->inpath, filepath);
                                        else
                                            strcpy(tma_tmppar->inpath, filepath);
                                    }
                                    XmTextSetString(xtas_winput_netlist_file, filename);
                                    XmTextSetCursorPosition(xtas_winput_netlist_file, (XmTextPosition) strlen(filename));
                                    XmTextSetTopCharacter(xtas_winput_netlist_file, (XmTextPosition) 0);
                                    p = strrchr(filename,'.');
                                    if(p)
                                        *p = '\0';
                                //    XmTextSetString( xtas_woutput_file_name, filename);
                                    if (TOOL == TAS_TOOL) /*|| ((TOOL == TMA_TOOL) && (!strcmp(tma_tmppar->in,"spi")))*/
                                        strcpy(tas_tmppar->infile,filename);
                                                                   
                                    else {
                                        strcpy(tma_tmppar->namein, filename);
                                        strcpy(tma_tmppar->nameout, filename);
                                        XmTextSetString(xtas_woutput_name, filename);
                                        XmTextSetCursorPosition(xtas_woutput_name, (XmTextPosition) strlen(filename));
                                        XmTextSetTopCharacter(xtas_woutput_name, (XmTextPosition) 0);
                                    }
                                    strcpy(tas_tmppar->outfile, filename);
                                    XmTextSetString(xtas_woutput_file_name, filename);
                                    XmTextSetCursorPosition(xtas_woutput_file_name, (XmTextPosition) strlen(filename));
                                    XmTextSetTopCharacter(xtas_woutput_file_name, (XmTextPosition) 0);

                                    break;

       case XTAS_TAS_OUTPUT_FILE:   filename = XtasGetFileNameFromFilePath(filepath);
                                    if(*filepath != '/') 
                                        strcpy(tas_tmppar->outpath, ".");
                                    else 
                                        strcpy(tas_tmppar->outpath, filepath);
                                    strcpy(tas_tmppar->outfile, filename); 
                                    XmTextSetString(xtas_woutput_file_name, filename);
                                    XmTextSetCursorPosition(xtas_woutput_file_name, (XmTextPosition) strlen(filename));
                                    XmTextSetTopCharacter(xtas_woutput_file_name, (XmTextPosition) 0);
                                    break;

       case XTAS_TMA_OUTPUT_FILE:   filename = XtasGetFileNameFromFilePath(filepath);
                                    if(*filepath != '/') 
                                        strcpy(tma_tmppar->outpath, ".");
                                    else 
                                        strcpy(tma_tmppar->outpath, filepath);
                                    strcpy(tma_tmppar->nameout, filename); 
                                    XmTextSetString(xtas_woutput_name, filename);
                                    XmTextSetCursorPosition(xtas_woutput_name, (XmTextPosition) strlen(filename));
                                    XmTextSetTopCharacter(xtas_woutput_name, (XmTextPosition) 0);
                                    break;                          
                                    
       case XTAS_TECHNO_FILE   :    strcpy(tas_tmppar->tec, filepath);
                                    XmTextSetString(xtas_winput_techno_file, filepath);
                                    XmTextSetCursorPosition(xtas_winput_techno_file, (XmTextPosition) strlen(filepath));
                                    XmTextSetTopCharacter(xtas_winput_techno_file, (XmTextPosition) 0);
                                    break;
                                    
//        case XTAS_FILEIN_FILE   :   strcpy(tma_tmppar->fin, filepath);
//                                    XmTextSetString(xtas_winput_filein_file, filepath);
//                                    XmTextSetCursorPosition(xtas_winput_filein_file, (XmTextPosition) strlen(filepath));
//                                    XmTextSetTopCharacter(xtas_winput_filein_file, (XmTextPosition) 0);
//                                    break;
//
//        case XTAS_READAREA_FILE :   strcpy(tma_tmppar->ra, filepath);
//                                    XmTextSetString(xtas_winput_readarea_file, filepath);
//                                    XmTextSetCursorPosition(xtas_winput_readarea_file, (XmTextPosition) strlen(filepath));
//                                    XmTextSetTopCharacter(xtas_winput_readarea_file, (XmTextPosition) 0);
//                                    break;


                                    
    }
    XtUnmanageChild( widget );

  //  XalLeaveLimitedLoop();
}

void XtasTasCancelCallback(widget, call_data, client_data)
Widget  widget;
XtPointer call_data;
XtPointer client_data;
{
    XtUnmanageChild(widget);
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSessionFileCallback                                        */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .file_type : Indicates which file to read.                     */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New Session command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSessionFileCallback( widget, file_type, call_data )
Widget  widget;
XtPointer file_type;
XtPointer call_data;
{
Atom      WM_DELETE_WINDOW;
char      title[64];
char     *etoile = "*.";
char      file_mask[20];
Arg       args[10];
int       n;
XmString  Filter;
static    int type;

type = (int)(long)file_type;

strcpy(file_mask, etoile);

//XalLeaveLimitedLoop();

switch ((int)(long)file_type) {
    case XTAS_NETLIST_FILE  :   strcpy(title,"Netlist File");
                                if(TOOL == TAS_TOOL)
                                    strcat(file_mask, tas_tmppar->ext);
                                else
                                    strcat(file_mask, tma_tmppar->ext);
                                break;
    case XTAS_TAS_OUTPUT_FILE  :strcpy(title,"TAS Output File");
                                strcpy(file_mask, "*");
                                break;
    case XTAS_TECHNO_FILE   :   strcpy(title,"Techno File"); 
                                strcat(file_mask, "elp");
                                break;
//    case XTAS_CONTEXT->TAS_FILEIN_FILE   :   strcpy(title,"Library File");
//                                strcpy(file_mask, "*");
//                                break;
//    case XTAS_READAREA_FILE :   strcpy(title,"Area File");
//                                strcpy(file_mask, "*");
//                                break;
    case XTAS_TMA_OUTPUT_FILE:  strcpy(title,"TMA Output File");
                                strcpy(file_mask, "*");
                                break;
}
    
//n = 0;
//XtSetArg( args[n], XmNtitle, title); n++;

if (!xtas_tas_file_select) {
    n = 0;
    XtSetArg ( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
    xtas_tas_file_select = XmCreateFileSelectionDialog( widget, "XtasFileSelect", args, n );
	XtUnmanageChild( XmSelectionBoxGetChild(xtas_tas_file_select, XmDIALOG_HELP_BUTTON ));
    XtAddCallback( xtas_tas_file_select, XmNokCallback, XtasSessionFileOkCallback, (XtPointer)(&type) );
    XtAddCallback( xtas_tas_file_select, XmNcancelCallback, XtasTasCancelCallback, (XtPointer)xtas_tas_file_select );

    

/*--------------------------------------------------*/
/* Add a callback for the WM_DELETE_WINDOW protocol */
/*--------------------------------------------------*/

    WM_DELETE_WINDOW = XmInternAtom( XtDisplay(widget), "WM_DELETE_WINDOW", False );
    XmAddWMProtocolCallback( XtParent(xtas_tas_file_select), WM_DELETE_WINDOW,
                             XtasCancelCallback, xtas_tas_file_select);
}

/* #NA 30-10-93 */
//XtVaSetValues( xtas_tas_file_select,
//               XtVaTypedArg, XmNdirMask, XmRString, file_mask,
//               strlen(file_mask) + 1, NULL );

Filter    = XmStringCreateSimple( file_mask );

n = 0;
XtSetArg (args[n], XmNpattern, Filter ); n++;
XtSetValues( xtas_tas_file_select, args, n);
XmStringFree(Filter);

n = 0;
XtSetArg (args[n], XmNtitle, title    ); n++;
XtSetValues( XtParent(xtas_tas_file_select), args, n);
XtManageChild( xtas_tas_file_select );
//XalLimitedLoop( xtas_tas_file_select); 

}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptTypeCallback                                            */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .cbs : Toggle informations.                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptTypeCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

switch ((int)(long)type)
    {
    case 0 : tas_tmppar->rc   = ( state->set ) ? 'Y' : 'N' ; break;
    case 1 : tas_tmppar->mg   = ( state->set ) ? 'Y' : 'N' ; break;
    case 2 : tas_tmppar->bk   = ( state->set ) ? 'Y' : 'N' ; break;
    }

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCalculationCallback                                        */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCalculationCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;
    if(state->set) {
        switch((char)(long)type) {
            case PCH    :   tas_tmppar->pch      = 'Y'; 
                            break;
            case F      :   tas_tmppar->f        = 'Y';
                            break;
            case COUT   :   XtSetSensitive(tas_form_w[0], True);
                            tas_tmppar->use_cout  = 'Y'; 
                            break;
            case OPC    :   XtSetSensitive(tas_form_w[1], True);
                            tas_tmppar->use_opc   = 'Y'; 
                            break;
            case SLOPE  :   XtSetSensitive(tas_form_w[2], True);
                            tas_tmppar->use_slope = 'Y'; 
                            break;
            case SWC    :   XtSetSensitive(tas_form_w[3], True);
                            tas_tmppar->use_swc   = 'Y'; 
                            break;
//            case NM     :   tas_tmppar->nm       = 'Y';
//                            break;
            case Y      :   tas_tmppar->y        = 'Y';
                            break;
            case Q      :   tas_tmppar->q        = 'Y';
                            break;
            case STR    :   tas_tmppar->stb      = 'Y';
                            break;
        }
    }
    else {
        switch((char)(long)type) {
            case PCH    :   tas_tmppar->pch      = 'N'; 
                            break;
            case F      :   tas_tmppar->f        = 'N';
                            break;
            case COUT   :   XtSetSensitive(tas_form_w[0], False);
                            tas_tmppar->use_cout  = 'N'; 
                            break;
            case OPC    :   XtSetSensitive(tas_form_w[1], False);
                            tas_tmppar->use_opc   = 'N'; 
                            break;
            case SLOPE  :   XtSetSensitive(tas_form_w[2], False);
                            tas_tmppar->use_slope = 'N'; 
                            break;
            case SWC    :   XtSetSensitive(tas_form_w[3], False);
                            tas_tmppar->use_swc   = 'N'; 
                            break;
//            case NM     :   tas_tmppar->nm       = 'N';
//                            break;
            case Y      :   tas_tmppar->y        = 'N';
                            break;
            case Q      :   tas_tmppar->q        = 'N';
                            break;
            case STR    :   tas_tmppar->stb      = 'N';
                            break;
        }
    
    }
        
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCharacterizationCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .cbs : Toggle informations.                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCharacterizationCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{

switch ((int)(long)type)
    {
    case 0 : tas_tmppar->carac = SCM_INT; break;
    case 1 : tas_tmppar->carac = SCM_CPL; break;
    case 2 : tas_tmppar->carac = LUT_INT; break;
    case 3 : tas_tmppar->carac = LUT_CPL; break;
    case 4 : tas_tmppar->carac = NO_MODEL;break;
    }

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasRegistersCallback                                       */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .cbs : Toggle informations.                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTasRegistersCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

switch ((int)(long)type)
    {
    case XTAS_SHARE_CMD         : tas_tmppar->cl   = ( state->set ) ? 'Y' : 'N'  ; break;
    case XTAS_FF_DETECT         : tas_tmppar->fl   = ( state->set ) ? 'Y' : 'N'  ; break;
    case XTAS_SLAVE_FF          : tas_tmppar->ls   = ( state->set ) ? 'Y' : 'N'  ; break;
    case XTAS_LATCH_LIB         : tas_tmppar->nl   = ( state->set ) ? 'N' : 'Y'  ; break;
    case XTAS_AUTO_LATCH_LOOP   : tas_tmppar->la   = ( state->set ) ? 'Y' : 'N'  ; break;
    }

}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasPatternRecogCallback                                    */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .cbs : Toggle informations.                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTasPatternRecogCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{ 
//    Widget toggleTR, toggleOTR, toggleHR, toggleOHR;
//    Boolean bTR, bOTR, bHR, bOHR;
    
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

if(state->set) {
    switch ((int)(long)type) {
        case XTAS_TRANS_RECOG       : tas_tmppar->fcl = 'Y'; 
                                      
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyTransRecog"); 
                                      //toggleOHR = XtNameToWidget(XtParent(widget), "*XtasOnlyHierRecog");
                                      //toggleHR = XtNameToWidget(XtParent(widget), "*XtasHierRecog");
                                      
                                      //XtSetSensitive(toggleOTR,False);
                                      //XtVaSetValues(toggleOTR, XmNset, False, NULL);
                                      //tas_tmppar->xfcl = 'N';
                                      
                                      //XtSetSensitive(toggleOHR,False);                   
                                      //XtVaSetValues(toggleOHR, XmNset, False, NULL);
                                      //tas_tmppar->xg = 'N';
                                      
                                      //XtSetSensitive(toggleHR,True);
   
                                      break;
                                      
                                      
        case XTAS_ONLY_TRANS_RECOG  : tas_tmppar->xfcl = 'Y'; 
                                      
                                      //toggleTR = XtNameToWidget(XtParent(widget), "*XtasTransRecog");
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyHierRecog");
                                      //toggleHR = XtNameToWidget(XtParent(widget), "*XtasHierRecog");
                                      
                                      //XtSetSensitive(toggleTR,False); 
                                      //XtVaSetValues(toggleTR, XmNset, False, NULL);
                                      //tas_tmppar->fcl = 'N';
                                      
                                      //XtSetSensitive(toggleOTR,False);
                                      //XtVaSetValues(toggleOTR, XmNset, False, NULL);
                                      //tas_tmppar->xg = 'N';
                                      
                                      //XtSetSensitive(toggleHR,True); 
                                      
                                      break;
                                      
                                      
        case XTAS_HIER_RECOG        : tas_tmppar->gns = 'Y'; 
                                      
                                      //toggleTR = XtNameToWidget(XtParent(widget), "*XtasTransRecog");
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyTransRecog");
                                      //toggleOHR = XtNameToWidget(XtParent(widget), "*XtasOnlyHierRecog");
                                      
                                      //XtVaGetValues(toggleOTR, XmNset, &bOTR, NULL);
                                      //if(!bOTR)
                                      //  XtSetSensitive(toggleTR,True);                   
                                      
                                      //XtVaGetValues(toggleTR, XmNset, &bTR, NULL);
                                      //if(!bTR)
                                      //  XtSetSensitive(toggleOTR,True);

                                      //XtSetSensitive(toggleOHR,False);  
                                      //XtVaSetValues(toggleOHR, XmNset, False, NULL);
                                      //tas_tmppar->xg = 'N';
                                      
                                      break;

                                      
        case XTAS_ONLY_HIER_RECOG   : tas_tmppar->xg = 'Y'; 

                                      //toggleTR = XtNameToWidget(XtParent(widget), "*XtasTransRecog"); 
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyTransRecog"); 
                                      //toggleHR = XtNameToWidget(XtParent(widget), "*XtasHierRecog");
                                     
                                      //XtSetSensitive(toggleTR,False);                  
                                      //XtVaSetValues(toggleTR, XmNset, False, NULL);
                                      //tas_tmppar->fcl = 'N';
                                      
                                      //XtSetSensitive(toggleOTR,False);                  
                                      //XtVaSetValues(toggleOTR, XmNset, False, NULL);
                                      //tas_tmppar->xfcl = 'N';
                                     
                                      //XtSetSensitive(toggleHR,False);
                                      //XtVaSetValues(toggleHR, XmNset, False, NULL);
                                      //tas_tmppar->gns = 'N';
                                     
                                      break;
    }
}
else {
    switch ((int)(long)type) {
        case XTAS_TRANS_RECOG       : tas_tmppar->fcl = 'N'; 
                                      
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyTransRecog"); 
                                      //toggleOHR = XtNameToWidget(XtParent(widget), "*XtasOnlyHierRecog");
                                      //toggleHR = XtNameToWidget(XtParent(widget), "*XtasHierRecog");
                                    
                                      //XtSetSensitive(toggleOTR,True);
                                     
                                      //XtVaGetValues( toggleHR, XmNset, &bHR, NULL);
                                      //if(!bHR)
                                      //  XtSetSensitive(toggleOHR,True);                   
                                      
   
                                      break;
                                      
                                      
        case XTAS_ONLY_TRANS_RECOG  : tas_tmppar->xfcl = 'N'; 
                                      
                                      //toggleTR = XtNameToWidget(XtParent(widget), "*XtasTransRecog");
                                      //toggleOHR = XtNameToWidget(XtParent(widget), "*XtasOnlyHierRecog");
                                      //toggleHR = XtNameToWidget(XtParent(widget), "*XtasHierRecog");
                                    
                                      //XtSetSensitive(toggleTR,True); 
                                     
                                      //XtVaGetValues( toggleHR, XmNset, &bHR, NULL);
                                      //if(!bHR)
                                      //  XtSetSensitive(toggleOHR,True);
                                      
                                      
                                      break;
                                      
                                      
        case XTAS_HIER_RECOG        : tas_tmppar->gns = 'N'; 
                                     
                                      
                                      //toggleOHR = XtNameToWidget(XtParent(widget), "*XtasOnlyHierRecog");
                                      //toggleTR = XtNameToWidget(XtParent(widget), "*XtasTransRecog");
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyTransRecog");
                                     
                                      //XtVaGetValues(toggleTR, XmNset, &bTR, NULL);
                                      //XtVaGetValues(toggleOTR, XmNset, &bOTR, NULL);
                                     
                                      //if(!bTR && !bOTR)
                                      //  XtSetSensitive(toggleOHR,True);  
                                      
                                      break;

                                      
        case XTAS_ONLY_HIER_RECOG   : tas_tmppar->xg = 'N'; 

                                      //toggleTR = XtNameToWidget(XtParent(widget), "*XtasTransRecog"); 
                                      //toggleOTR = XtNameToWidget(XtParent(widget), "*XtasOnlyTransRecog"); 
                                      //toggleHR = XtNameToWidget(XtParent(widget), "*XtasHierRecog");
                                      
                                      //XtSetSensitive(toggleTR,True);                  
                                      
                                      //XtSetSensitive(toggleOTR,True);                  
                                     
                                      //XtSetSensitive(toggleHR,True);
                                     
                                      break;
    }

}
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasTransistorsCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .cbs : Toggle informations.                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTasTransistorsCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

switch ((int)(long)type)
    {
    case XTAS_RM_PARA_TRANS : tas_tmppar->rpt   = ( state->set ) ? 'Y' : 'N'  ; break;
    case XTAS__S_CONV       : tas_tmppar->o     = ( state->set ) ? 'Y' : 'N'  ; break;
    case XTAS_SIMPLE_OR     : tas_tmppar->b     = ( state->set ) ? 'Y' : 'N'  ; break;
    }

}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasFuncAnaCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .cbs : Toggle informations.                                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Net type toggle's is pressed.       */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTasFuncAnaCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    Widget form;
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

if(state->set) {
    switch ((int)(long)type){
        case  XTAS_FUNC_ANA     : form = XtNameToWidget(XtParent(widget),"*XtasFuncAnaForm");
                                  XtSetSensitive(form, True);
                                  form = XtNameToWidget(XtParent(widget),"*XtasFuncAnaHZ");
                                  XtSetSensitive(form, True);
                                  break;
                                  
        case  XTAS_HZ_NODES     : tas_tmppar->z     = 'Y' ; 
                                  break;
    }
}

else {
    switch ((int)(long)type){
        case  XTAS_FUNC_ANA     : form = XtNameToWidget(XtParent(widget),"*XtasFuncAnaForm");
                                  XtSetSensitive(form, False);
                                  form = XtNameToWidget(XtParent(widget),"*XtasFuncAnaHZ");
                                  XtSetSensitive(form, False);
                                  XtVaSetValues(form, XmNset, False, NULL);
                                  tas_tmppar->z     = 'N' ; 
                                  strcpy(tas_tmppar->p, "0");
                                  break;
                                  
        case  XTAS_HZ_NODES     : tas_tmppar->z     = 'N'  ; 
                                  break;
    }

}

}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSessionOptionsOkCallback                                   */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .parent : The parent widget id.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New Session Options command */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSessionOptionsOkCallback( widget, parent, call_data )
Widget  widget;
XtPointer  parent;
XtPointer call_data;
{

    Widget toggle;
    Boolean b;
/*-------------------------------------------------------*/
/* Copy the the temporary structure into the real one    */
/*-------------------------------------------------------*/

/*
tas_params->help_s = tas_tmppar->help_s;
tas_params->pch    = tas_tmppar->pch;
*/
//strncpy(tas_tmppar->swc   , XmTextGetString( tas_w_opt_elect[0] ), 32);
//strncpy(tas_tmppar->opc   , XmTextGetString( tas_w_opt_elect[1] ), 32);
//strncpy(tas_tmppar->p     , XmTextGetString( tas_w_opt_elect[2] ), 32);
//strncpy(tas_tmppar->cout  , XmTextGetString( tas_w_opt_elect[3] ), 32);
if (tas_tmppar->use_cout  == 'Y')
    strncpy(tas_tmppar->cout , XmTextGetString( tas_text_w[0] ), 32);
else
    strcpy(tas_tmppar->cout,"");

if (tas_tmppar->use_opc  == 'Y')
    strncpy(tas_tmppar->opc  , XmTextGetString( tas_text_w[1] ), 32);
else
    strcpy(tas_tmppar->opc,"");

if (tas_tmppar->use_slope  == 'Y')
    strncpy(tas_tmppar->slope, XmTextGetString( tas_text_w[2] ), 32);
else
    strcpy(tas_tmppar->slope,"");

if (tas_tmppar->use_swc  == 'Y')
    strncpy(tas_tmppar->swc  , XmTextGetString( tas_text_w[3] ), 32);
else
    strcpy(tas_tmppar->swc,"");

toggle = XtNameToWidget(widget,"*XtasUseFuncAna");
XtVaGetValues(toggle, XmNset, &b, NULL);
if (b)
    strncpy(tas_tmppar->p    , XmTextGetString( tas_text_w[4] ), 32);
else
    strcpy(tas_tmppar->p,"");


//XalLeaveLimitedLoop();
}



/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSessionOptionsCallback                                    */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .parent : The parent widget id.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New Session command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSessionOptionsCallback( widget, parent, call_data )
Widget  widget;
XtPointer  parent;
XtPointer call_data;
{
Arg      args[10];
int      n, type;
Widget   frame, frame2, frame3, row_widget, row_widget2, tmp_widget, label_widget;
Widget   separator, form, dbase_form, ebase_form, fbase_form, sub_form;
//char     outfiles[20], interc_options[10];
XmString text;
Atom     WM_DELETE_WINDOW;

Widget toggle;

/*-------------------------------------------------------*/
/* Copy the real parameters into the temporary structure */
/*-------------------------------------------------------*/

    XtManageChild ((Widget)parent);
if (!session_options_widget) {
    n = 0;
    XtSetArg( args[n], XmNtitle, XTAS_NAME": Timing Analysis Advanced Options..." ); n++;
    XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
    session_options_widget = XmCreatePromptDialog(widget, "XtasInfosBox", args, n);
    XtUnmanageChild( XmSelectionBoxGetChild(session_options_widget, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(session_options_widget, XmDIALOG_PROMPT_LABEL));
    XtAddCallback( session_options_widget, XmNokCallback, XtasSessionOptionsOkCallback, (XtPointer)session_options_widget );
    XtAddCallback( session_options_widget, XmNcancelCallback, XtasCancelCallback, (XtPointer)session_options_widget );
    WM_DELETE_WINDOW = XmInternAtom(XtDisplay(widget), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(XtParent(session_options_widget), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )session_options_widget);
	HelpFather = session_options_widget ;
    XtAddCallback( session_options_widget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_TASADV)) ;
    
    //XmAddTabGroup( session_options_widget );

    n = 0;
    form = XtCreateManagedWidget( "XtasDeskMainForm", xmFormWidgetClass, session_options_widget, args, n );


    /*----------------------------------------------------------------------*/
    /*       Frame  Calculation 					        */
    /*----------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    frame = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );

    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, frame, args, n );

    text = XmStringCreateSimple( "Calculation" );
    tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopOffset,       2,
            XmNrightOffset,     2,
            XmNleftOffset,      2,
            NULL);
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, dbase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       tmp_widget,
            XmNtopOffset,       10,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNleftOffset,      5,
            XmNbottomAttachment,XmATTACH_FORM,
            XmNbottomOffset,    5,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         5,
            NULL);
    row_widget2 = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, dbase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       tmp_widget,
            XmNtopOffset,       10,
            XmNbottomAttachment,XmATTACH_FORM,
            XmNbottomOffset,    5,
            XmNleftAttachment,  XmATTACH_WIDGET,
            XmNleftWidget,      row_widget,
            XmNrightAttachment, XmATTACH_FORM,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         5,
            NULL);
    //XmAddTabGroup( row_widget );
    /*--------------------------------------------------------------------------------------------*/
    text = XmStringCreateSimple( "precharged signals" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->pch == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)PCH );

    /*--------------------------------------------------------------------------------------------*/
    text = XmStringCreateSimple( "path factorisation" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->f == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)F );

    /*--------------------------------------------------------------------------------------------*/
    n = 0;
    sub_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, row_widget, args, n);

    text = XmStringCreateSimple( "output terminal charge" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM, 
            XmNindicatorType,   XmN_OF_MANY,
            XmNset,             tas_tmppar->use_cout == 'Y' ? True:False,
            XmNlabelString,     text,
            XmNalignment,       XmALIGNMENT_BEGINNING,
            XmNwidth,           160,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)COUT );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );  n++;
    XtSetArg( args[n], XmNleftWidget,       toggle          );  n++;
    XtSetArg( args[n], XmNsensitive,        tas_tmppar->use_cout == 'Y' ? True:False );  n++;
    tas_form_w[0] = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, sub_form, args, n);

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    // XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNwidth,                 55                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tas_text_w[0] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tas_form_w[0], args, n);
    XmTextSetString(tas_text_w[0],tas_tmppar->cout);

    text = XmStringCreateSimple( "pF" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            tas_text_w[0]     ); n++;
    XtSetArg( args[n], XmNlabelString,           text              ); n++;
    XtSetArg( args[n], XmNalignment,             XmALIGNMENT_BEGINNING); n++;
    XtSetArg( args[n], XmNwidth,                 25                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, tas_form_w[0], args, n);
    /*--------------------------------------------------------------------------------------------*/
    n = 0;
    sub_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, row_widget, args, n);

    text = XmStringCreateSimple( "out of path capacitance factor" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM, 
            XmNindicatorType,   XmN_OF_MANY,
            XmNset,             tas_tmppar->use_opc == 'Y' ? True:False,
            XmNlabelString,     text,
            XmNalignment,       XmALIGNMENT_BEGINNING,
            XmNwidth,           210,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)OPC );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );  n++;
    XtSetArg( args[n], XmNleftWidget,       toggle          );  n++;
    XtSetArg( args[n], XmNsensitive,        tas_tmppar->use_opc == 'Y' ? True:False );  n++;
    tas_form_w[1] = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, sub_form, args, n);

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNwidth,                 55                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tas_text_w[1] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tas_form_w[1], args, n);
    XmTextSetString(tas_text_w[1],tas_tmppar->opc);

    text = XmStringCreateSimple( "%" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            tas_text_w[1]     ); n++;
    XtSetArg( args[n], XmNlabelString,           text              ); n++;
    XtSetArg( args[n], XmNalignment,             XmALIGNMENT_BEGINNING); n++;
    XtSetArg( args[n], XmNwidth,                 40                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, tas_form_w[1], args, n);

    /*--------------------------------------------------------------------------------------------*/
    text = XmStringCreateSimple( "use stability analysis" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->stb == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)STR );


    /*--------------------------------------------------------------------------------------------*/
    n = 0;
    sub_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, row_widget2, args, n);

    text = XmStringCreateSimple( "connectors input slope" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM, 
            XmNindicatorType,   XmN_OF_MANY,
            XmNset,             tas_tmppar->use_slope == 'Y' ? True:False,
            XmNlabelString,     text,
            XmNalignment,       XmALIGNMENT_BEGINNING,
            XmNwidth,           160,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)SLOPE );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );  n++;
    XtSetArg( args[n], XmNleftWidget,       toggle          );  n++;
    XtSetArg( args[n], XmNsensitive,        tas_tmppar->use_slope == 'Y' ? True:False );  n++;
    tas_form_w[2] = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, sub_form, args, n);

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNwidth,                 55                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tas_text_w[2] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tas_form_w[2], args, n);
    XmTextSetString(tas_text_w[2],tas_tmppar->slope);

    text = XmStringCreateSimple( "ps" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            tas_text_w[2]     ); n++;
    XtSetArg( args[n], XmNlabelString,           text              ); n++;
    XtSetArg( args[n], XmNalignment,             XmALIGNMENT_BEGINNING); n++;
    XtSetArg( args[n], XmNwidth,                 25                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, tas_form_w[2], args, n);

    /*--------------------------------------------------------------------------------------------*/
    n = 0;
    sub_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, row_widget2, args, n);

    text = XmStringCreateSimple( "input pass-transistor capacitance factor" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM, 
            XmNindicatorType,   XmN_OF_MANY,
            XmNset,             tas_tmppar->use_swc == 'Y' ? True:False,
            XmNlabelString,     text,
            XmNalignment,       XmALIGNMENT_BEGINNING,
            XmNwidth,           270,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)SWC );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   );  n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );  n++;
    XtSetArg( args[n], XmNleftWidget,       toggle          );  n++;
    XtSetArg( args[n], XmNsensitive,        tas_tmppar->use_swc == 'Y' ? True:False );  n++;
    tas_form_w[3] = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, sub_form, args, n);

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM   ); n++;
    XtSetArg( args[n], XmNwidth,                 55                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    tas_text_w[3] = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tas_form_w[3], args, n);
    XmTextSetString(tas_text_w[3],tas_tmppar->swc);

    text = XmStringCreateSimple( "%" );
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,            tas_text_w[3]     ); n++;
    XtSetArg( args[n], XmNlabelString,           text              ); n++;
    XtSetArg( args[n], XmNalignment,             XmALIGNMENT_BEGINNING); n++;
    XtSetArg( args[n], XmNwidth,                 30                ); n++;
    XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
    label_widget = XtCreateManagedWidget( "XtasLabels", xmLabelWidgetClass, tas_form_w[3], args, n);


    /*--------------------------------------------------------------------------------------------*/
//    text = XmStringCreateSimple( "remove look-up tables" );
//    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//            XmNindicatorType, XmN_OF_MANY,
//            XmNset,           False,
//            XmNlabelString,   text,
//            NULL);
//    XmStringFree( text );
//    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)NM );
//
    /*--------------------------------------------------------------------------------------------*/
    text = XmStringCreateSimple( "ignore black boxes" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget2,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->y == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)Y );

    /*--------------------------------------------------------------------------------------------*/
    text = XmStringCreateSimple( "flat cells" );
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget2,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->q == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasCalculationCallback, (XtPointer)Q );


    //for ( type=0; type < XtNumber(tas_opt_output); type++)
    //    {
    //    Widget toggle;
    //
    //    text = XmStringCreateSimple( tas_opt_output[type] );
    //    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
    //                                      XmNindicatorType, XmN_OF_MANY,
    //                                      XmNset,           False,
    //                                      XmNlabelString,   text,
    //                                      NULL);
    //    XmStringFree( text );
    //
    //    if ( outfiles[type] == 'Y' )
    //        {
    //        n = 0;
    //        XtSetArg( args[n], XmNset, True ); n++;
    //        XtSetValues( toggle, args, n );
    //        }
    //
    //    if ( outfiles[type] == 'N' )
    //        {
    //        n = 0;
    //        XtSetArg( args[n], XmNset, False ); n++; 
    //        XtSetValues( toggle, args, n );
    //        }
    // 
    //    if ( outfiles[type] == 'X' )
    //        {
    //        n = 0;
    //        XtSetArg( args[n], XmNsensitive, False ); n++; 
    //        XtSetValues( toggle, args, n );
    //        }
    //    XtAddCallback( toggle, XmNvalueChangedCallback, XtasOutFilesCallback, (XtPointer)type );
    //    }


    /*---------------------  end Calculation -------------------------------------*/

    /*------------------------------------------------------------------*/
    /*                    Frame Interconnect Options        	        */
    /*------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       frame             ); n++;
    XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
    XtSetArg( args[n], XmNbottomWidget,     frame             ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
    frame2=dbase_form;
    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

    text = XmStringCreateSimple( "Interconnect Options" );
    tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopOffset,       2,
            XmNrightOffset,     2,
            XmNleftOffset,      2,
            NULL);
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, dbase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       tmp_widget,
            XmNtopOffset,       25,
            XmNleftOffset,      10,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         15,
            NULL);
    //XmAddTabGroup( row_widget );

    for ( type=0; type < (int)(long)XtNumber(tas_opt_inter); type++)
    {
        Widget toggle;

        text = XmStringCreateSimple( tas_opt_inter[type] );
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType, XmN_OF_MANY,
                XmNlabelString,   text,
                NULL);
        XmStringFree( text );
        
        switch (type ) {
            case 0  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->rc == 'Y' ? True:False, NULL); break;
            case 1  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->mg == 'Y' ? True:False, NULL); break;
            case 2  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->bk == 'Y' ? True:False, NULL); break;
        }
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasOptTypeCallback, (XtPointer)type );
    }


    /*------------------------------------------------------------------*/
    /*                    Characterizaton Options        	            */
    /*------------------------------------------------------------------*/
    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    //XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );
    frame3 = dbase_form;

    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

    text = XmStringCreateSimple( "Characterization" );
    tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopOffset,       2,
            XmNrightOffset,     2,
            XmNleftOffset,      2,
            NULL);
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, dbase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       tmp_widget,
            XmNtopOffset,       70,
            XmNleftOffset,      5,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNrightOffset,     10,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNradioBehavior,   True,
            XmNradioAlwaysOne,  True,
            XmNspacing,         60,
            NULL);
    //XmAddTabGroup( row_widget );

    for ( type=0; type < (int)(long)XtNumber(tas_opt_carac); type++)
    {

        text = XmStringCreateSimple( tas_opt_carac[type] );
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType, XmONE_OF_MANY,
                XmNlabelString, text,
                NULL);
        XmStringFree( text );
        switch(type) {
            case 0  :   XtVaSetValues (toggle, XmNset, tas_tmppar->carac == SCM_INT ? True:False, NULL);break;
            case 1  :   XtVaSetValues (toggle, XmNset, tas_tmppar->carac == SCM_CPL ? True:False, NULL);break;
            case 2  :   XtVaSetValues (toggle, XmNset, tas_tmppar->carac == LUT_INT ? True:False, NULL);break;
            case 3  :   XtVaSetValues (toggle, XmNset, tas_tmppar->carac == LUT_CPL ? True:False, NULL);break;
        }
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasCharacterizationCallback, (XtPointer)type );

    }


    /*------------------------------------------------------------------*/
    /*                    Abstraction Options            	            */
    /*------------------------------------------------------------------*/

    n = 0;
    XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
    XtSetArg( args[n], XmNleftWidget,       frame3            ); n++;
    XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    dbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, form, args, n );

    n = 0;
    dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );

    text = XmStringCreateSimple( "Abstraction" );
    tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, dbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNtopOffset,       2,
            XmNrightOffset,     2,
            XmNleftOffset,      2,
            NULL);
    XmStringFree( text );

    row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, dbase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       tmp_widget,
            XmNleftOffset,      5,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM,
            XmNbottomOffset,    5,
            XmNrightAttachment, XmATTACH_FORM,
            XmNrightOffset,     5,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      2,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         5,
            NULL);
    //XmAddTabGroup( row_widget );

    /******* 1st frame: Latch / flipflop ********/
    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, row_widget, args, n);

    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       50                ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,      50                ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( " LATCH / FLIPFLOP " );
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, fbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            NULL);
    XmStringFree( text );

    fbase_form = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       fbase_form,
            XmNtopOffset,       10,
            XmNleftOffset,      10,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         5,
            NULL);

    text = XmStringCreateSimple("latch share command");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->cl == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasRegistersCallback, (XtPointer)XTAS_SHARE_CMD);

    text = XmStringCreateSimple("flipflop detection");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->fl == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasRegistersCallback, (XtPointer)XTAS_FF_DETECT);

    text = XmStringCreateSimple("slave as flipflop");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->ls == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasRegistersCallback, (XtPointer)XTAS_SLAVE_FF);

    text = XmStringCreateSimple( " Latch Detection " );
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, ebase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopOffset,       10,
            XmNtopWidget,       fbase_form,
            XmNleftAttachment,  XmATTACH_FORM,
            NULL);
    XmStringFree( text );


    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET );n++; 
    XtSetArg( args[n], XmNtopWidget,        fbase_form      );n++; 
    XtSetArg( args[n], XmNtopOffset,        20              );n++; 
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );n++; 
    XtSetArg( args[n], XmNleftWidget,       label_widget    );n++; 
    XtSetArg( args[n], XmNleftOffset,       5               );n++; 
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   );n++; 
    XtSetArg( args[n], XmNrightOffset,      20              );n++; 
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    );n++; 
    separator = XtCreateManagedWidget("XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n);

    fbase_form = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       separator,
            XmNtopOffset,       5,
            XmNleftOffset,      30,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM,
            XmNbottomOffset,    10,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         5,
            NULL);

    text = XmStringCreateSimple("use built-in library");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->nl == 'N' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    if(TOOL == TMA_TOOL){
       XtVaSetValues(toggle, XmNset, True, NULL);
        tas_tmppar->nl = 'Y';
    }
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasRegistersCallback, (XtPointer)XTAS_LATCH_LIB);

    text = XmStringCreateSimple("use automatic loop analysis");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->la == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    if(TOOL == TMA_TOOL) {
        XtVaSetValues(toggle, XmNset, True, NULL);
        tas_tmppar->la = 'Y';
    }
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasRegistersCallback, (XtPointer)XTAS_AUTO_LATCH_LOOP);


    /******* 2nd frame: Pattern recognition ********/
    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, row_widget, args, n);

    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       50                ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightOffset,      50                ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( " PATTERN RECOGNITION " );
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, fbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            NULL);
    XmStringFree( text );

    fbase_form = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       fbase_form,
            XmNtopOffset,       30,
            XmNleftOffset,      10,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         10,
            NULL);

    text = XmStringCreateSimple("transistor netlist recognition");
    toggle = XtVaCreateManagedWidget( "XtasTransRecog", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->fcl == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasPatternRecogCallback, (XtPointer)XTAS_TRANS_RECOG);

    text = XmStringCreateSimple("stops after transistor netlist recognition");
    toggle = XtVaCreateManagedWidget( "XtasOnlyTransRecog", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->xfcl == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasPatternRecogCallback, (XtPointer)XTAS_ONLY_TRANS_RECOG);

    text = XmStringCreateSimple("hierachical recognition");
    toggle = XtVaCreateManagedWidget( "XtasHierRecog", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->gns == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasPatternRecogCallback, (XtPointer)XTAS_HIER_RECOG);

    text = XmStringCreateSimple("stops after hierachical recognition");
    toggle = XtVaCreateManagedWidget( "XtasOnlyHierRecog", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->xg == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasPatternRecogCallback, (XtPointer)XTAS_ONLY_HIER_RECOG);

    /******* 3rd frame: TRANSISTORS ********/
    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, row_widget, args, n);

    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       50                ); n++;
    XtSetArg( args[n], XmNrightOffset,      50                ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( " TRANSISTORS " );
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, fbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            NULL);
    XmStringFree( text );

    text = XmStringCreateSimple("removes parallel transistors");
    toggle = XtVaCreateManagedWidget( "XtasTransRecog", xmToggleButtonWidgetClass, ebase_form,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopWidget,     fbase_form,
            XmNtopOffset,     30,
            XmNleftAttachment,XmATTACH_FORM,
            XmNleftOffset,    10,
            XmNrightAttachment,XmATTACH_FORM,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->rpt == 'Y' ? True:False,
            XmNlabelString,   text,
            XmNalignment,     XmALIGNMENT_BEGINNING,
            XmNheight,        20,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasTransistorsCallback, (XtPointer)XTAS_RM_PARA_TRANS);

    text = XmStringCreateSimple( " Orientation " );
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, ebase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       toggle,
            XmNtopOffset,       25,
            XmNleftAttachment,  XmATTACH_FORM,
            NULL);
    XmStringFree( text );


    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET );n++; 
    XtSetArg( args[n], XmNtopWidget,        toggle          );n++; 
    XtSetArg( args[n], XmNtopOffset,        35              );n++; 
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );n++; 
    XtSetArg( args[n], XmNleftWidget,       label_widget    );n++; 
    XtSetArg( args[n], XmNleftOffset,       5               );n++; 
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   );n++; 
    XtSetArg( args[n], XmNrightOffset,      10              );n++; 
    XtSetArg( args[n], XmNorientation,      XmHORIZONTAL    );n++; 
    separator = XtCreateManagedWidget("XtasSeparator", xmSeparatorWidgetClass, ebase_form, args, n);

    fbase_form = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
            XmNtopAttachment,   XmATTACH_WIDGET,
            XmNtopWidget,       separator,
            XmNtopOffset,       10,
            XmNleftOffset,      30,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            XmNbottomAttachment,XmATTACH_FORM,
            XmNpacking,         XmPACK_COLUMN,
            XmNnumColumns,      1,
            XmNorientation,     XmVERTICAL,
            XmNisAligned,       True,
            XmNspacing,         10,
            NULL);

    text = XmStringCreateSimple("take the \"_s\" convention in account");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->o == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasTransistorsCallback, (XtPointer)XTAS__S_CONV);

    text = XmStringCreateSimple("use simple transistor orientation heuristic ");
    toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->b == 'Y' ? True:False,
            XmNlabelString,   text,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasTransistorsCallback, (XtPointer)XTAS_SIMPLE_OR);

    /******* 4th frame: Functional Analysis ********/
    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasFrame", xmFrameWidgetClass, row_widget, args, n);

    n = 0;
    ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
    XtSetArg( args[n], XmNleftOffset,       50                ); n++;
    XtSetArg( args[n], XmNrightOffset,      50                ); n++;
    XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
    fbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, ebase_form, args, n );

    text = XmStringCreateSimple( " FUNCTIONAL ANALYSIS " );
    label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, fbase_form,
            XmNlabelString,     text,
            XmNtopAttachment,   XmATTACH_FORM,
            XmNleftAttachment,  XmATTACH_FORM,
            XmNrightAttachment, XmATTACH_FORM,
            NULL);
    XmStringFree( text );


    text = XmStringCreateSimple("use functional analysis");
    toggle = XtVaCreateManagedWidget( "XtasUseFuncAna", xmToggleButtonWidgetClass, ebase_form,
            XmNleftAttachment, XmATTACH_FORM,
            XmNleftOffset,     10,
            XmNrightAttachment,XmATTACH_FORM,
            XmNtopAttachment, XmATTACH_WIDGET,
            XmNtopOffset,     50,
            XmNtopWidget,     fbase_form,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           (strlen(tas_tmppar->p) > 0) ? True:False,
            XmNlabelString,   text,
            XmNalignment,     XmALIGNMENT_BEGINNING,
            XmNheight,        20,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasFuncAnaCallback, (XtPointer)XTAS_FUNC_ANA);

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET     ); n++;   
    XtSetArg( args[n], XmNtopWidget,        toggle              ); n++;   
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ); n++;   
    XtSetArg( args[n], XmNleftOffset,       30                  ); n++;   
    XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM       ); n++;   
    XtSetArg( args[n], XmNsensitive,        (strlen(tas_tmppar->p) > 0) ? True:False ); n++;   
    fbase_form = XtCreateManagedWidget( "XtasFuncAnaForm", xmFormWidgetClass, ebase_form,args, n);

    text = XmStringCreateSimple("depth");
    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM       ); n++;   
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM       ); n++;   
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM       ); n++;   
    XtSetArg( args[n], XmNlabelString,      text                ); n++;   
    label_widget = XtCreateManagedWidget("xtasLabels", xmLabelWidgetClass, fbase_form, args, n);
    XmStringFree(text);

    n = 0;
    XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM       ); n++;   
    XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET     ); n++;   
    XtSetArg( args[n], XmNleftWidget,       label_widget        ); n++;   
    XtSetArg( args[n], XmNleftOffset,       10                  ); n++;   
    XtSetArg( args[n], XmNwidth,            30                  ); n++;   
    XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM       ); n++;   
    tas_text_w[4] = XtCreateManagedWidget("XtasUserField", xmTextWidgetClass, fbase_form, args, n);
    XmTextSetString(tas_text_w[4], tas_tmppar->p);


    text = XmStringCreateSimple("exploits high impedance nodes");
    toggle = XtVaCreateManagedWidget( "XtasFuncAnaHZ", xmToggleButtonWidgetClass, ebase_form,
            XmNtopAttachment,     XmATTACH_WIDGET,
            XmNtopWidget,         fbase_form,
            XmNtopOffset,         15,
            XmNleftAttachment,    XmATTACH_FORM,
            XmNleftOffset,        10,
            XmNrightAttachment,   XmATTACH_FORM,
            //XmNbottomAttachment,  XmATTACH_FORM,
            XmNindicatorType, XmN_OF_MANY,
            XmNset,           tas_tmppar->z == 'Y' ? True:False,
            XmNsensitive,     (strlen(tas_tmppar->p) > 0) ? True:False,
            XmNlabelString,   text,
            XmNalignment,     XmALIGNMENT_BEGINNING,
            XmNheight,        20,
            NULL);
    XmStringFree( text );
    XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasFuncAnaCallback, (XtPointer)XTAS_HZ_NODES);


}
XtManageChild( session_options_widget );
//XalLimitedLoop( session_options_widget );

}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTasOptions                                                 */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char *XtasTasOptions( char *elpTechnoFile_save)
{
    char *str=NULL, *str2, *new_work_lib = NULL, **new_cata_lib = NULL;
    int  i,j;


    if (strlen(tas_tmppar->tec) != 0){
        strcpy(elpTechnoFile, tas_tmppar->tec);
        //ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE;
    }else{
        strcpy(elpTechnoFile, elpTechnoFile_save); 
    }
        
    if((TOOL == TAS_TOOL) || ((TOOL == TMA_TOOL) && ((!strcmp(tma_tmppar->in,"spi")) || (!strcmp(tma_tmppar->in,"vhd")) || (!strcmp(tma_tmppar->in,"vlg"))))) {
       // tas_init();
        tas_version();
        
        
        if ((TOOL == TMA_TOOL) && ((!strcmp(tma_tmppar->in,"spi")) || (!strcmp(tma_tmppar->in,"vhd")) || (!strcmp(tma_tmppar->in,"vlg")))) {
            TAS_CONTEXT->TAS_FILENAME = namealloc(tma_tmppar->namein);
            strcpy(IN_LO, tma_tmppar->in);
            if(strlen(tma_tmppar->outpath) != 0) {
                new_work_lib = (char *)mbkalloc((unsigned)(strlen(tma_tmppar->outpath)+1)*sizeof(char));
                strcpy(new_work_lib,tma_tmppar->outpath);
            }
            else {
                new_work_lib = (char *)mbkalloc((unsigned)2*sizeof(char));
                strcpy(new_work_lib,".");
            }
            WORK_LIB_SAVE = WORK_LIB;
            WORK_LIB = new_work_lib;
            if (strlen(tas_tmppar->outfile) != 0) TAS_CONTEXT->TAS_FILEOUT=namealloc(tas_tmppar->outfile);
            else    TAS_CONTEXT->TAS_FILEOUT  = TAS_CONTEXT->TAS_FILENAME;
        }
        else {
            for(i=0 ; ((CATA_LIB[i] != NULL) && (strcmp(CATA_LIB[i],tas_tmppar->inpath) != 0)) ; i++);
            if(CATA_LIB[i] == NULL){ 
                new_cata_lib = (char **)mbkalloc((unsigned int)(i+2) * sizeof(char *));
                for(j=0; j<i ; j++) {
                    new_cata_lib[j] = (char *)mbkalloc((strlen(CATA_LIB[j])+1) * sizeof(char));
                    strcpy(new_cata_lib[j],CATA_LIB[j]);
                }
                str2 = (char *)mbkalloc((strlen(tas_tmppar->inpath)+1) * sizeof(char));
                strcpy(str2, tas_tmppar->inpath);
                new_cata_lib[i] = str2;
                new_cata_lib[i+1] = NULL;
                CATA_LIB = new_cata_lib;
            }

            TAS_CONTEXT->TAS_FILENAME = namealloc(tas_tmppar->infile);
            strcpy(IN_LO, tas_tmppar->in);
            if(strlen(tas_tmppar->outpath) != 0) {
                new_work_lib = (char *)mbkalloc((unsigned)(strlen(tas_tmppar->outpath)+1)*sizeof(char));
                strcpy(new_work_lib,tas_tmppar->outpath);
            }
            else {
                new_work_lib = (char *)mbkalloc((unsigned)2*sizeof(char));
                strcpy(new_work_lib,".");
            }
            WORK_LIB_SAVE = WORK_LIB;
            WORK_LIB = new_work_lib;
            if (strlen(tas_tmppar->outfile) != 0) TAS_CONTEXT->TAS_FILEOUT=namealloc(tas_tmppar->outfile);
            else    TAS_CONTEXT->TAS_FILEOUT  = TAS_CONTEXT->TAS_FILENAME;
        }
        
        if(tas_tmppar->i == 'Y')
            avt_sethashvar("avtReadInformationFile", inf_GetDefault_AVT_INF_Value());            

        if (tas_tmppar->min  == 'Y')  TAS_CONTEXT->TAS_FIND_MIN='Y';  
        else                          TAS_CONTEXT->TAS_FIND_MIN='N';  
        
        if (tas_tmppar->hr   == 'Y') {
            TAS_CONTEXT->TAS_HIER='Y'; 
            TAS_CONTEXT->TAS_TOOLNAME="hitas";
        }
        else {
            TAS_CONTEXT->TAS_HIER='N'; 
            TAS_CONTEXT->TAS_TOOLNAME="tas";
        }
        
        if (tas_tmppar->dtx  == 'Y') { 
            TAS_CONTEXT->TAS_PERFINT='Y';
            if(tas_tmppar->ttx == 'N')
                TAS_CONTEXT->TAS_INT_END = 'Y';
            else
                TAS_CONTEXT->TAS_INT_END = 'N';
        }       
        else    TAS_CONTEXT->TAS_PERFINT = 'N';
        
        if (tas_tmppar->c    == 'Y') TAS_CONTEXT->TAS_CNS_FILE='Y';
        else                         TAS_CONTEXT->TAS_CNS_FILE='N';
        
        if (tas_tmppar->e    == 'Y') TAS_CONTEXT->TAS_SLOFILE='Y';
        else                         TAS_CONTEXT->TAS_SLOFILE='N';
        
        if (tas_tmppar->s    == 'Y') TAS_CONTEXT->TAS_SILENTMODE='Y';
        else                         TAS_CONTEXT->TAS_SILENTMODE='N';
        
        if (tas_tmppar->pch  == 'Y')  TAS_CONTEXT->TAS_TREATPRECH = 'Y';
        else                          TAS_CONTEXT->TAS_TREATPRECH = 'N';
        
        if (tas_tmppar->f    == 'Y')  TAS_CONTEXT->TAS_FACTORISE = 'Y';
        else                          TAS_CONTEXT->TAS_FACTORISE = 'N';
        
        if (tas_tmppar->stb  == 'Y')  TAS_CONTEXT->TAS_STABILITY = 'Y';
        else                          TAS_CONTEXT->TAS_STABILITY = 'N';
        
        if (strlen(tas_tmppar->cout) != 0) { 
            TAS_CONTEXT->TAS_CAPAOUT = atof(tas_tmppar->cout);
            if((TAS_CONTEXT->TAS_CAPAOUT < 0) || ((TAS_CONTEXT->TAS_CAPAOUT == 0.0) && (!strcmp(tas_tmppar->cout,"0")))) {
                str = (char *)mbkalloc(sizeof(char)*1024);
                strcpy(str,"Bad value for Output terminal charge");
                return str;
            }
        }
        else
            TAS_CONTEXT->TAS_CAPAOUT = 0.0;

        if (strlen(tas_tmppar->opc) != 0) {
            TAS_CONTEXT->TAS_CAPARAPREC = (double)atoi(tas_tmppar->opc)/100.0;
            if(((TAS_CONTEXT->TAS_CAPARAPREC == 0.0) && (strcmp(tas_tmppar->opc,"0"))) || (TAS_CONTEXT->TAS_CAPARAPREC < 0.0)) {
                str = (char *)mbkalloc(sizeof(char)*1024);
                strcpy(str,"Bad value for out of path capacitance factor");
                return str;
            }           
        }
        else
            TAS_CONTEXT->TAS_CAPARAPREC = 0.0;
        
        if (strlen(tas_tmppar->slope) != 0) {
            TAS_CONTEXT->FRONT_CON = atof(tas_tmppar->slope);
            TAS_CONTEXT->FRONT_NOT_SHRINKED = atof(tas_tmppar->slope);
            if(((TAS_CONTEXT->FRONT_CON == 0.0) && (strcmp(tas_tmppar->slope,"0"))) || (TAS_CONTEXT->FRONT_CON < (long)0)) {
                str = (char *)mbkalloc(sizeof(char)*1024);
                strcpy(str,"Bad value for connector input slope");
                return str;
            }    
        }
        else 
            TAS_CONTEXT->FRONT_CON = TAS_NOFRONT;

        if (strlen(tas_tmppar->swc) != 0) {
            TAS_CONTEXT->TAS_CAPASWITCH = (double)atoi(tas_tmppar->swc)/100.0;
            if(((TAS_CONTEXT->TAS_CAPASWITCH == 0.0) && (strcmp(tas_tmppar->swc,"0"))) || (TAS_CONTEXT->TAS_CAPASWITCH < 0.0)) {
                str = (char *)mbkalloc(sizeof(char)*1024);
                strcpy(str,"Bad value for input pass-transistor factor");
                return str;
            }
        }
        else
            TAS_CONTEXT->TAS_CAPASWITCH = 0.0;
//        if (tas_tmppar->nm   == 'Y') TAS_CONTEXT->TAS_SUPSTMMODEL = 'Y';

        if (tas_tmppar->y    == 'Y') TAS_CONTEXT->TAS_IGNBLACKB   = 'Y';
        else                         TAS_CONTEXT->TAS_IGNBLACKB   = 'N';
     
        if (tas_tmppar->q    == 'Y') TAS_CONTEXT->TAS_FLATCELLS   = 'Y';
        else                         TAS_CONTEXT->TAS_FLATCELLS   = 'N';
        
        if (tas_tmppar->mg   == 'Y') TAS_CONTEXT->TAS_MERGERCN    = 'Y';
        else                         TAS_CONTEXT->TAS_MERGERCN    = 'N';
        
        if (tas_tmppar->rc   == 'Y') TAS_CONTEXT->TAS_CALCRCN     = 'Y';
        else                         TAS_CONTEXT->TAS_CALCRCN     = 'N';
        
        if (tas_tmppar->bk   == 'Y') TAS_CONTEXT->TAS_BREAKLOOP   = 'Y';
        else                         TAS_CONTEXT->TAS_BREAKLOOP   = 'N';
        
        switch(tas_tmppar->carac) {
            case SCM_INT :  TAS_CONTEXT->TAS_CARAC = 'Y';
                            TAS_CONTEXT->TAS_CARACMODE = TAS_SCM_INT;
                            break;
            case SCM_CPL :  TAS_CONTEXT->TAS_CARAC = 'Y';
                            TAS_CONTEXT->TAS_CARACMODE = TAS_SCM_CPL;
                            break;
            case LUT_INT :  TAS_CONTEXT->TAS_CARAC = 'Y';
                            TAS_CONTEXT->TAS_CARACMODE = TAS_LUT_INT;
                            break;
            case LUT_CPL :  TAS_CONTEXT->TAS_CARAC = 'Y';
                            TAS_CONTEXT->TAS_CARACMODE = TAS_LUT_CPL;
                            break;
            case NO_MODEL:  TAS_CONTEXT->TAS_CARAC = 'N';
                            TAS_CONTEXT->TAS_CARACMODE = 0;
                            break;
        }
        
        /*** options de Yagle ***/
        if ((tas_tmppar->fcl  == 'Y') || (tas_tmppar->xfcl == 'Y')) YAG_CONTEXT->YAG_FCL_DETECT = TRUE;
        else                                                        YAG_CONTEXT->YAG_FCL_DETECT = FALSE;
        
        if (tas_tmppar->xfcl == 'Y')  YAG_CONTEXT->YAG_ONLY_FCL = TRUE;
        else                          YAG_CONTEXT->YAG_ONLY_FCL = FALSE;
        
        if (tas_tmppar->nl   == 'Y')   YAG_CONTEXT->YAG_DETECT_LATCHES = FALSE;
        else                           YAG_CONTEXT->YAG_DETECT_LATCHES = TRUE;
        
        if (tas_tmppar->cl   == 'Y')   YAG_CONTEXT->YAG_CELL_SHARE = TRUE;
        else                           YAG_CONTEXT->YAG_CELL_SHARE = FALSE; 
        
        if (tas_tmppar->fl   == 'Y')   YAG_CONTEXT->YAG_MAKE_CELLS = TRUE;
        else                           YAG_CONTEXT->YAG_MAKE_CELLS = FALSE;
        
        if ((tas_tmppar->gns  == 'Y') || (tas_tmppar->xg   == 'Y'))  YAG_CONTEXT->YAG_GENIUS = TRUE;
        else                                                         YAG_CONTEXT->YAG_GENIUS = FALSE;
        
        if (tas_tmppar->xg   == 'Y')  YAG_CONTEXT->YAG_ONLY_GENIUS = TRUE;
        else                          YAG_CONTEXT->YAG_ONLY_GENIUS = FALSE;
        
        if (tas_tmppar->rpt  == 'Y')   YAG_CONTEXT->YAG_REMOVE_PARA = TRUE;
        else                           YAG_CONTEXT->YAG_REMOVE_PARA = FALSE;
        
        if (tas_tmppar->la   == 'Y')   YAG_CONTEXT->YAG_LOOP_ANALYSIS = TRUE;
        else                           YAG_CONTEXT->YAG_LOOP_ANALYSIS = FALSE;
        
        if (tas_tmppar->o    == 'Y')   YAG_CONTEXT->YAG_HELP_S = TRUE;
        else                           YAG_CONTEXT->YAG_HELP_S = FALSE;
        
        if (tas_tmppar->z    == 'Y')   YAG_CONTEXT->YAG_PROP_HZ = TRUE;
        else                           YAG_CONTEXT->YAG_PROP_HZ = FALSE;
        
        if (tas_tmppar->b    == 'Y')   YAG_CONTEXT->YAG_ORIENT = TRUE;
        else                           YAG_CONTEXT->YAG_ORIENT = FALSE;
        
        if (strlen(tas_tmppar->p) != 0) {
            YAG_CONTEXT->YAG_DEPTH = atoi(tas_tmppar->p);
            YAG_CONTEXT->YAG_USE_FCF = TRUE ;

            if((YAG_CONTEXT->YAG_DEPTH == 0) && (strcmp(tas_tmppar->p,"0") != 0)) {
                str = (char *)mbkalloc(sizeof(char)*1024);
                strcpy(str,"Bad value for depth level of functionnal analysis");
                return str;
            }

            if(YAG_CONTEXT->YAG_DEPTH == 0)
                YAG_CONTEXT->YAG_USE_FCF = FALSE ;
        }
        else {
            YAG_CONTEXT->YAG_USE_FCF = FALSE;
            YAG_CONTEXT->YAG_DEPTH = 0;
        }

     }
    
//    if(TOOL == TMA_TOOL) {
//        for(i=0 ; ((CATA_LIB[i] != NULL) && (strcmp(CATA_LIB[i],tma_tmppar->inpath) != 0)) ; i++);
//        if(CATA_LIB[i] == NULL){ 
//            new_cata_lib = (char **)mbkalloc((unsigned int)(i+2) * sizeof(char *));
//            for(j=0; j<i ; j++) {
//                new_cata_lib[j] = (char *)mbkalloc((strlen(CATA_LIB[j])+1) * sizeof(char));
//                strcpy(new_cata_lib[j],CATA_LIB[j]);
//            }
//            str2 = (char *)mbkalloc((strlen(tma_tmppar->inpath)+1) * sizeof(char));
//            strcpy(str2, tma_tmppar->inpath);
//            new_cata_lib[i] = str2;
//            new_cata_lib[i+1] = NULL;
//            CATA_LIB = new_cata_lib;
//        }
//
//        TMA_CONTEXT->TMA_NAMEIN = namealloc(tma_tmppar->namein);
//      
//        if(strlen(tma_tmppar->outpath) != 0) {
//            new_work_lib = (char *)mbkalloc((unsigned)(strlen(tma_tmppar->outpath)+1)*sizeof(char));
//            strcpy(new_work_lib,tma_tmppar->outpath);
//        }
//        else {
//            new_work_lib = (char *)mbkalloc((unsigned)2*sizeof(char));
//            strcpy(new_work_lib,".");
//        }
//        WORK_LIB_SAVE = WORK_LIB;
//        WORK_LIB = new_work_lib;
//        if(strlen(tma_tmppar->nameout) != 0)
//            TMA_CONTEXT->TMA_LIBRARY = namealloc(tma_tmppar->nameout);
//        
//        TMA_CONTEXT->TMA_OUTPUT = tma_tmppar->out;
//
//        TMA_CONTEXT->TMA_MODE = tma_tmppar->mod;
////        if(strlen(tma_tmppar->fin) != 0)
////            TMA_CONTEXT->TMA_FILEIN = namealloc(tma_tmppar->fin);
////
////        if(strlen(tma_tmppar->ra) != 0)
////            TMA_CONTEXT->TMA_READFILE = namealloc(tma_tmppar->ra);
//
//        if(tma_tmppar->tu == TMA_PS)
//            TMA_CONTEXT->TMA_TUNIT = namealloc("ps");
//        else
//            TMA_CONTEXT->TMA_TUNIT = namealloc("ns");
//    
//        if(tma_tmppar->cu == TMA_PF)
//            TMA_CONTEXT->TMA_CUNIT = namealloc("pf");
//        else
//            TMA_CONTEXT->TMA_CUNIT = namealloc("ff");
//    
//        if(strlen(tma_tmppar->mrg) != 0)
//            TMA_CONTEXT->TMA_MARGIN = atoi (tma_tmppar->mrg);
//        else
//            TMA_CONTEXT->TMA_MARGIN = 0;
//
//        if(tma_tmppar->dtxin == 'Y') TMA_CONTEXT->TMA_DTXIN = 'Y';
//        else                        TMA_CONTEXT->TMA_DTXIN = 'N';
//        if(tma_tmppar->ttxin == 'Y') TMA_CONTEXT->TMA_TTXIN = 'Y';
//        else                        TMA_CONTEXT->TMA_TTXIN = 'N';
//       
//        if(tma_tmppar->otock == 'Y') TMA_CONTEXT->TMA_AUTOCLOCK = 'Y';
//        else                        TMA_CONTEXT->TMA_AUTOCLOCK = 'N';
//
//        if(tma_tmppar->i == 'Y')
//            avt_sethashvar("avtReadInformationFile", inf_GetDefault_AVT_INF_Value());            
//
//        if(tma_tmppar->v == 'Y')     TMA_CONTEXT->TMA_VERBOSE = 'Y';
//        else                        TMA_CONTEXT->TMA_VERBOSE = 'N';
//    }
    
    return NULL;


    
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFreeAllCells                                               */
/*                                                                           */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void    XtasFreeAllCells (ttvfig)
    ttvfig_list     *ttvfig;
{
    chain_list      *ins;

    if (ttvfig->INS) {
        for (ins = ttvfig->INS ; ins ; ins = ins->NEXT)
            XtasFreeAllCells ((ttvfig_list *)ins->DATA);
    }
    if (((ttvfig->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK) &&
                       (stm_getmemcell(ttvfig->INFO->FIGNAME)))
        stm_freecell(ttvfig->INFO->FIGNAME);

}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSessionTreat                                               */
/*                                                                           */
/* IN  ARGS : ( None )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Call for loading and treating a new session.                   */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSessionTreat( )
{
    int i;
    ttvfig_list *ptfig, *ptfignext;
    char *mess, in_lo_save[4], **cata_lib_save, elpTechnoFile_save[elpSTRINGSIZE];
    cnsfig_list    *ptcnsfig;
    char   text_title[128]; 
    
    XtasSetLabelString( XtasDeskMessageField, XTAS_NULINFO );
    if(TOOL == TAS_TOOL)
        XtasSetLabelString( XtasDeskMessageField, "TAS tool is running..." );
    else
        XtasSetLabelString( XtasDeskMessageField, "TMA tool is running..." );


    XtasCleanStates();
    XalSetCursor( XtasTopLevel, WAIT );  
    XalForceUpdate( XtasTopLevel );
 
    cata_lib_save = CATA_LIB;
    strcpy(elpTechnoFile_save,elpTechnoFile) ;
    strcpy(in_lo_save, IN_LO);
    mess = XtasTasOptions(elpTechnoFile_save);
    if (!mess) {
        XtasPasqua();
        if (sigsetjmp( XtasMyEnv , 1 ) == 0)
        {   
            inf_Dirty(NULL); // all inffig will be reloaded from disk

            if (TOOL == TMA_TOOL) {
            /*
                if(((!strcmp(IN_LO,"spi")) || (!strcmp(IN_LO,"vhd")) || (!strcmp(IN_LO,"vlg"))) && XtasMainParam->ttvfig) {
                    ptfig = XtasMainParam->ttvfig;
                    while(ptfig) {
                        ptfignext = ptfig->NEXT;
                        XtasFreeAllCells (ptfig);      
                        if (getloadedlofig(ptfig->INFO->FIGNAME)) {
                            dellofig (ptfig->INFO->FIGNAME);
                        }
                        for (ptcnsfig = CNS_HEADCNSFIG; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
                            if (strcmp(ptcnsfig->NAME, ptfig->INFO->FIGNAME) == 0) break;
                        }
                        if (ptcnsfig) {
                            CNS_HEADCNSFIG = delcnsfig (CNS_HEADCNSFIG, ptcnsfig);
                            ptcnsfig = NULL;
                        }
                        ptfig = ptfignext;
                    }
                    ttv_freeall();
                }
                ptfig = tma_core();
                if (ptfig) {
                    if (XtasMainParam->stbfig != NULL) {
                        stb_ctk_clean(XtasMainParam->stbfig);
                        stb_delstbfig(XtasMainParam->stbfig) ;
                        XtasMainParam->stbfig= NULL;
                        XTAS_CTX_LOADED = XTAS_NOT_LOADED;
                    }
                    XtasMainParam->ttvfig = ptfig;
                    XtasRemovePathSession(NULL);
                    XtasRemoveDetailPathSession(NULL) ;
                    XtasRemoveStbSession(NULL) ;
                    XtasSigsRemove();
                    XTAS_SIMU_NEW_NETLIST = 'Y';
                    XtasSetLabelString(XtasDeskMessageField,"New DataBase Successfully Loaded");
                    sprintf(text_title,"Xtas Main Window - %s",
                          XtasMainParam->ttvfig->INFO->FIGNAME);
                    XtVaSetValues (XtasTopLevel, XmNtitle, text_title, NULL);
                }
                stm_exit();
                if(XtasMainParam->ttvfig != NULL)
                    XtasGetOutMess();
            */
            }
            else {
                if(XtasMainParam->ttvfig) {
                    ptfig = XtasMainParam->ttvfig;
                    while(ptfig) {
                        ptfignext = ptfig->NEXT;
//                        if(((ptfig->STATUS & TTV_STS_LOCK) != TTV_STS_LOCK) &&
//                                (stm_getcell(ptfig->INFO->FIGNAME)))
//                            stm_freecell(ptfig->INFO->FIGNAME);
                        XtasFreeAllCells (ptfig);      
                        if (getloadedlofig(ptfig->INFO->FIGNAME))
                            dellofig (ptfig->INFO->FIGNAME);
                        for (ptcnsfig = CNS_HEADCNSFIG; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
                            if (strcmp(ptcnsfig->NAME, ptfig->INFO->FIGNAME) == 0) break;
                        }
                        if (ptcnsfig)
                            CNS_HEADCNSFIG = delcnsfig (CNS_HEADCNSFIG, ptcnsfig);
                        ptfig = ptfignext;
                    }
                    ttv_freeall();
                } 
                if(TAS_CONTEXT->TAS_SILENTMODE == 'Y') {
                    tas_CloseTerm();
                    ptfig = tas_main();
                    tas_RestoreTerm();
                    if (ptfig) {
                        if (XtasMainParam->stbfig != NULL) {
                            stb_ctk_clean(XtasMainParam->stbfig);
                            stb_delstbfig(XtasMainParam->stbfig) ;
                            XtasMainParam->stbfig= NULL;
                        }
                        XTAS_CTX_LOADED = XTAS_NOT_LOADED;
                        if (TAS_CONTEXT->TAS_INT_END == 'Y') 
                            XTAS_FORMAT_LOADED = XTAS_DTX;
                        else
                            XTAS_FORMAT_LOADED = XTAS_TTX;
                        XtasMainParam->ttvfig = ptfig;
                        XtasRemovePathSession(NULL);
                        XtasRemoveDetailPathSession(NULL) ;
                        XtasRemoveStbSession(NULL) ;
                        XtasSigsRemove();
                        XTAS_SIMU_NEW_NETLIST = 'Y';
                        XtasSetLabelString(XtasDeskMessageField,"New DataBase Successfully Loaded");
                        sprintf(text_title,"Xtas Main Window - %s",
                                XtasMainParam->ttvfig->INFO->FIGNAME);
                        XalForceUpdate (XtasMainWindow);
                        XtVaSetValues (XtasTopLevel, XmNtitle, text_title, NULL);
                    }
                }
                else {
                    ptfig = tas_main();
                    if(ptfig != NULL)
                        XtasGetOutMess();
                    if (ptfig) {
                        if (XtasMainParam->stbfig != NULL) {
                            stb_ctk_clean(XtasMainParam->stbfig);
                            stb_delstbfig(XtasMainParam->stbfig) ;
                            XtasMainParam->stbfig= NULL;
                        }
                        XTAS_CTX_LOADED = XTAS_NOT_LOADED;
                        if (TAS_CONTEXT->TAS_INT_END == 'Y') 
                            XTAS_FORMAT_LOADED = XTAS_DTX;
                        else
                            XTAS_FORMAT_LOADED = XTAS_TTX;
                        XtasMainParam->ttvfig = ptfig;
                        XtasRemovePathSession(NULL);
                        XtasRemoveDetailPathSession(NULL) ;
                        XtasRemoveStbSession(NULL) ;
                        XtasSigsRemove();
                        XTAS_SIMU_NEW_NETLIST = 'Y';
                        XtasSetLabelString(XtasDeskMessageField,"New DataBase Successfully Loaded");
                        sprintf(text_title,"Xtas Main Window - %s",
                                XtasMainParam->ttvfig->INFO->FIGNAME);
                        XalForceUpdate (XtasMainWindow);
                        XtVaSetValues (XtasTopLevel, XmNtitle, text_title, NULL);
                    }
                }
                   
            }
            
        }
        XalSetCursor( XtasTopLevel, NORMAL ); 
        XtasGetWarningMess() ;
        XtasFirePasqua() ;

    }
    else
        XalDrawMessage(XtasWarningWidget, mess);
    if (ptfig) {
        XtasSetLabelString(XtasDeskMessageField,"New DataBase Successfully Loaded");
    }
    if (CATA_LIB != cata_lib_save) {
        for (i=0; CATA_LIB[i]; i++)
            mbkfree(CATA_LIB[i]);
        mbkfree(CATA_LIB);
        CATA_LIB = cata_lib_save;
    }
    strcpy(elpTechnoFile, elpTechnoFile_save);
    strcpy(IN_LO, in_lo_save);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSessionOkCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .parent : The parent widget id.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New Session command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSessionOkCallback( widget, parent, call_data )
Widget  widget;
XtPointer  parent;
XtPointer call_data;
{


    char *tmp,*p;
    
    tmp = XmTextGetString( xtas_winput_netlist_file );
    p = strrchr(tmp,'.');
    if(p)
        *p = '\0';


    if(TOOL == TAS_TOOL) {
        if(strlen(tas_tmppar->inpath) == 0)
            strcpy(tas_tmppar->inpath, ".");
        strcpy(tas_tmppar->infile, tmp) ;
    }
    else {
        if(strlen(tma_tmppar->inpath) == 0)
            strcpy(tma_tmppar->inpath, ".");
        strcpy(tma_tmppar->namein, tmp) ;
    }
    
    if ( (strlen(tmp) == 0) || (*tmp == ' ') ) {
        XtManageChild( parent );
        XalDrawMessage( XtasErrorWidget, XTAS_NFILERR );
    }

    else {

        strcpy(tas_tmppar->tec, XmTextGetString(xtas_winput_techno_file));
       
        if((TOOL == TAS_TOOL) || ((TOOL == TMA_TOOL) && ((!strcmp(tma_tmppar->in,"spi")) || (!strcmp(tma_tmppar->in,"vhd")) || (!strcmp(tma_tmppar->in,"vlg"))))) {
            if(strlen(tas_tmppar->outpath) == 0)
                strcpy(tas_tmppar->outpath,".");
            strcpy(tas_tmppar->outfile, XmTextGetString(xtas_woutput_file_name));
            if(strlen(tas_tmppar->outfile) == 0) 
                strcpy(tas_tmppar->outfile,tas_tmppar->infile);
        }
    
       
        if (TOOL == TMA_TOOL){
            if(strlen(tma_tmppar->outpath) == 0)
                strcpy(tma_tmppar->outpath,".");
            strcpy(tma_tmppar->nameout, XmTextGetString(xtas_woutput_name));
            if(strlen(tma_tmppar->nameout) == 0) 
             strcpy(tma_tmppar->nameout,tma_tmppar->namein);
//            if(strlen(tma_tmppar->ra) > 0)
//                strcpy(tma_tmppar->ra, XmTextGetString(xtas_winput_readarea_file));
//            if(strlen(tma_tmppar->fin) > 0)
//                strcpy(tma_tmppar->fin, XmTextGetString(xtas_winput_filein_file));
        }        
        

        XalSetCursor( XtasTopLevel, WAIT ); 
        XtUnmanageChild( parent );
        XalForceUpdate( XtasTopLevel );

    //    XtasSigsRemove();
    //    XalForceUpdate( XtasTopLevel );

    //   XalSetCursor( XtasTopLevel, NORMAL ); 
    //   XalForceUpdate( XtasTopLevel );
    
        XtasSessionTreat();
    }
    if (tmp)
        XtFree (tmp);
        
//    XalLeaveLimitedLoop();
    
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasReadInfoCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasReadInfoCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) {
        if(TOOL == TMA_TOOL)
            tma_tmppar->i = 'Y';
        tas_tmppar->i = 'Y';
    }
    else{
        if(TOOL == TMA_TOOL)
            tma_tmppar->i = 'N';
        tas_tmppar->i = 'N';
    }

        
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaVerbCallback                                             */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTmaVerbCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) 
        tma_tmppar->v = 'Y';
    else
        tma_tmppar->v = 'N';
    
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaNacCallback                                             */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTmaNacCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) 
        tma_tmppar->otock = 'Y';
    else
        tma_tmppar->otock = 'N';
    
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaModeCallback                                            */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTmaModeCallback( widget, type, cbs )
Widget widget;
XtPointer type;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    switch((char)(long)type) {
       case 'F'     : if(state->set ) {
                          tma_tmppar->mod = 'R';
                          //tma_beh_mode = 'R';
                      }
//                      else {
//                          tma_tmppar->mod = 'W';
//                          //tma_beh_mode = 'W';
//                      }
                      break;
//       case 'f'     : if(state->set ) {
//                          tma_tmppar->mod = 'R';
//                          //tma_beh_mode = 'R';
//                      }
//                      else {
//                          tma_tmppar->mod = 'W';
//                          //tma_beh_mode = 'W';
//                      }
                      break;
       case 'A'     : if(state->set ) {
                          tma_tmppar->mod = 'W';
                          //tma_beh_mode = 'W';
                      }
                      else {
                          tma_tmppar->mod = 'R';
                          //tma_beh_mode = 'R';
                      }
                      break;
                     
    }
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaOptGenerateBehCallback                                  */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTmaOptGenerateBehCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) {
        tma_tmppar->mod = 'W';
        XtSetSensitive((Widget)client_data, True);
    }
    else {
        tma_tmppar->mod = 'T';
        XtSetSensitive((Widget)client_data, False);
    }
        
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOutputLibCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOutputLibCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    /*
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) {
        tma_tmppar->out |= TMA_OUT_LIB;
        XtSetSensitive((Widget)client_data, True);
    }
    else {
        tma_tmppar->out &= ~(TMA_OUT_LIB);
        if ((tma_tmppar->out == 0) || (tma_tmppar->out == TMA_OUT_TTX))
            XtSetSensitive((Widget)client_data, False);
    }
    */
        
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOutputTlf3Callback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOutputTlf3Callback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    /*
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) {
        tma_tmppar->out |= TMA_OUT_TLF3;
        XtSetSensitive((Widget)client_data, True);
    }
    else {
        tma_tmppar->out &= ~(TMA_OUT_TLF3);
        if ((tma_tmppar->out == 0) || (tma_tmppar->out == TMA_OUT_TTX))
            XtSetSensitive((Widget)client_data, False);
    }
    */
        
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOutputTlf4Callback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOutputTlf4Callback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    /*
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) {
        tma_tmppar->out |= TMA_OUT_TLF4;
        XtSetSensitive((Widget)client_data, True);
    }
    else {
        tma_tmppar->out &= ~(TMA_OUT_TLF4);
        if ((tma_tmppar->out == 0) || (tma_tmppar->out == TMA_OUT_TTX))
            XtSetSensitive((Widget)client_data, False);
    }
        
    */
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOutputTtxCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOutputTtxCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    /*
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) {
        tma_tmppar->out |= TMA_OUT_TTX;
        XtSetSensitive((Widget)client_data, True);
    }
    else {
        tma_tmppar->out &= ~(TMA_OUT_TTX);
        XtSetSensitive((Widget)client_data, False);
    }
    */
        
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasTmaMarginCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasTmaMarginCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) 
        XtSetSensitive((Widget)client_data, True);
    else {
        strcpy(tma_tmppar->mrg,"");
        XtSetSensitive((Widget)client_data, False);
    }
        
}
/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasFileInCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasFileInCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) 
        XtSetSensitive((Widget)client_data, True);
    else
        XtSetSensitive((Widget)client_data, False);
        
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasReadAreaCallback                                           */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .type : The selected type.                                     */
/*            .state : Toggle informations.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function when Output File toggle's is pressed.    */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasReadAreaCallback( widget, client_data, cbs )
Widget widget;
XtPointer client_data;
XtPointer cbs;
{
    XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

    if(state->set) 
        XtSetSensitive((Widget)client_data, True);
    else
        XtSetSensitive((Widget)client_data, False);
        
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNewSessionCallback                                         */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the File/New Session command         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNewSessionCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
    Arg      args[10];
    int      n, type;
    Widget   row_widget, tmp_widget, label_widget, form, dbase_form, ebase_form, fbase_form, sub_form, button, button2;
    Widget   frame, frame2, frame3, toggle, toggle2, title, separator;
    XmString text, cancel_text, adv_text;
    Pixmap   pixmap;
    Pixel    fg, bg;
    Atom     WM_DELETE_WINDOW;


    if(session_widget && (TOOL != (char)(long) client_data)) {
        XtDestroyWidget(session_widget);
        if( session_options_widget )
            XtDestroyWidget(session_options_widget);
        if( xtas_tas_file_select ) 
            XtDestroyWidget(xtas_tas_file_select);
            
        session_widget = NULL;
        session_options_widget = NULL;
        xtas_tas_file_select = NULL;
    }

    if (!session_widget) {




        /***********  fenetre  **********/
        n = 0;
        if((char)(long) client_data == TAS_TOOL) {
            if (!tas_tmppar)
                tas_tmppar = (xtas_tas_param_struct*)mbkalloc(sizeof(xtas_tas_param_struct));
            TOOL = TAS_TOOL;
            XtSetArg( args[n], XmNtitle, XTAS_NAME": Timing Anaysis Parameterization" ); n++; 
        }
        else if((char)(long) client_data == TMA_TOOL) {
            if (!tma_tmppar)
                tma_tmppar = (xtas_tma_param_struct*)mbkalloc(sizeof(xtas_tma_param_struct));
            if(XtasTmaInit() == 1)
                return;
            TOOL = TMA_TOOL;
            XtSetArg( args[n], XmNtitle, XTAS_NAME": Timing Model Abstraction Parameterization" ); n++; 
        }
        
        if (!tas_tmppar)
            tas_tmppar = (xtas_tas_param_struct*)mbkalloc(sizeof(xtas_tas_param_struct));

        XtasTasInit();
        
        adv_text = XmStringCreateSimple( "Advanced Options" );
        cancel_text = XmStringCreateSimple( "Cancel" );
        XtSetArg( args[n], XmNwidth, 805 ); n++; 
        XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++; 
        XtSetArg( args[n], XmNcancelLabelString, adv_text            ); n++; 
        session_widget = XmCreateMessageDialog(XtasDeskMainForm, "XtasInfosBox", args, n);
        XtUnmanageChild(XmSelectionBoxGetChild(session_widget, XmDIALOG_TEXT));
        XtUnmanageChild(XmSelectionBoxGetChild(session_widget, XmDIALOG_PROMPT_LABEL));
        XtAddCallback( session_widget, XmNokCallback, XtasSessionOkCallback, (XtPointer)session_widget );
        XtAddCallback( session_widget, XmNcancelCallback, XtasSessionOptionsCallback, (XtPointer)session_widget );
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasDeskMainForm), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(session_widget), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )session_widget);
        //XmAddTabGroup( session_widget );
        HelpFather = session_widget;
        if (TOOL==TAS_TOOL)
            XtAddCallback( session_widget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_TASPARAM)) ;
        else if (TOOL==TMA_TOOL) {
            XtAddCallback( session_widget, XmNhelpCallback, XtasHelpCallback, (XtPointer)(XTAS_HELP_MAIN|XTAS_SRC_TMAPARAM)) ;
            XtSetSensitive (XmSelectionBoxGetChild (session_widget, XmDIALOG_CANCEL_BUTTON), False);
        }
        XmStringFree( adv_text );

        n = 0;
        XtSetArg(args[n], XmNlabelString, cancel_text); n++;
        button = XtCreateManagedWidget ("XtasButton", xmPushButtonWidgetClass, session_widget, args, n);
        XtAddCallback (button, XmNactivateCallback, XtasCancelCallback, (XtPointer)session_widget); 
        XmStringFree( cancel_text );

        n = 0;
        form = XtCreateManagedWidget( "XtasNewSessionForm", xmFormWidgetClass, session_widget, args, n );
        
        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        dbase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, form, args, n );
 
        /***********  part2 :  Input Netlist Format  **********/
        n = 0;
        XtSetArg( args[n], XmNborderWidth,      0                       ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM           ); n++;
        XtSetArg( args[n], XmNtopOffset,        5                       ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM           ); n++;
        sub_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, dbase_form, args, n );
        frame = sub_form;

        n = 0;
        sub_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
    
        text = XmStringCreateSimple( "Input Netlist Format" );
        tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNleftOffset,      10,
                XmNrightAttachment, XmATTACH_FORM,
                XmNrightOffset,     10,
                XmNtopOffset,       2,
                NULL);
        XmStringFree( text );

        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, sub_form,
                XmNtopAttachment,   XmATTACH_WIDGET,
                XmNtopWidget,       tmp_widget,
                XmNtopOffset,       10,
                XmNleftOffset,      10,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNbottomAttachment,XmATTACH_FORM,
                XmNbottomOffset,    15,
                XmNpacking,         XmPACK_COLUMN,
                XmNnumColumns,      1,
                XmNorientation,     XmVERTICAL,
                XmNisAligned,       True,
                XmNradioBehavior,   True,
                XmNradioAlwaysOne,  True,
                NULL);
        
        if (TOOL == TAS_TOOL) {
            XtVaSetValues(row_widget, XmNleftOffset, 35, NULL);
            for ( type=0; type < (int)(long)XtNumber(tas_format_type); type++)
            {

                text   = XmStringCreateSimple( tas_format_type[type] );
                toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNspacing,       5,
                        XmNlabelString,   text,
                        NULL);
                XmStringFree( text );

                switch(type) {
                    case 0  :   if(!strcmp(tas_tmppar->in,"spi"))
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                    case 1  :   if(!strcmp(tas_tmppar->in,"vhd"))
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                    case 2  :   if(!strcmp(tas_tmppar->in,"vlg"))
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                }
                XtAddCallback( toggle, XmNvalueChangedCallback, XtasNetTypeCallback, (XtPointer)type );
            }
        }
        else {
            for ( type=0; type < (int)(long)XtNumber(tma_format_type); type++)
            {

                text   = XmStringCreateSimple( tma_format_type[type] );
                toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                        XmNindicatorType, XmONE_OF_MANY,
                        XmNspacing,       2,
                        XmNlabelString,   text,
                        NULL);
                XmStringFree( text );
                switch(type) {
                    case 0  :   if(!strcmp(tma_tmppar->in,"spi"))
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                    case 1  :   if(!strcmp(tma_tmppar->in,"vhd"))
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                    case 2  :   if(!strcmp(tma_tmppar->in,"vlg"))
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                    case 3  :   if(tma_tmppar->ttxin == 'Y')
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                    case 4  :   if(tma_tmppar->dtxin == 'Y')
                                    XtVaSetValues(toggle, XmNset, True, NULL);
                                else
                                    XtVaSetValues(toggle, XmNset, False, NULL);
                                break;
                }

                
                XtAddCallback( toggle, XmNvalueChangedCallback, XtasNetTypeCallback, (XtPointer)type );
            }
        }

       
        /***********  part1 : Input Files  **********/
        n = 0;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNleftWidget,       frame             ); n++;
        XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET ); n++;
        XtSetArg( args[n], XmNbottomWidget,     frame             ); n++;
        sub_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, dbase_form, args, n );
        frame2 = sub_form;
        
        n = 0;
        sub_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
        
        text = XmStringCreateSimple( "Input Files" );
        tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                                              XmNlabelString,     text,
                                              XmNtopAttachment,   XmATTACH_FORM,
                                              XmNleftAttachment,  XmATTACH_FORM,
                                              XmNrightAttachment, XmATTACH_FORM,
                                              XmNtopOffset,       2,
                                              XmNrightOffset,     2,
                                              XmNleftOffset,      2,
                                              NULL);
        XmStringFree( text );
        
        /***********  subpart1 : Netlist File  **********/
        n = 0;
        XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
        XtSetArg( args[n], XmNtopWidget,        tmp_widget        ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
        
        text = XmStringCreateSimple("Netlist File");
        n = 0;
        XtSetArg( args[n], XmNlabelString,      text              ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
        XtSetArg( args[n], XmNheight,           23                ); n++;
        XtSetArg( args[n], XmNwidth,            90                ); n++;
        label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, ebase_form, args, n );
        XmStringFree( text );
        
        n = 0;
        XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,             2                 ); n++;
        XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
        XtSetArg( args[n], XmNrightPosition,         90               ); n++;
        XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
        XtSetArg( args[n], XmNleftOffset,            30                ); n++;
        XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
        xtas_winput_netlist_file = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, ebase_form, args,n);
        
        XtVaGetValues(form, XmNforeground, &fg, XmNbackground, &bg, NULL);
        pixmap = XalGetPixmap(ebase_form, 
                             XTAS_OPEN_MAP, fg, bg);
        
        button = XtVaCreateManagedWidget("dbutton", xmDrawnButtonWidgetClass, ebase_form,
                                         XmNlabelType,         XmPIXMAP,
                                         XmNlabelPixmap,       pixmap,
                                         XmNwidth,             40,
                                         XmNheight,            35,
                                         XmNpushButtonEnabled, True,
                                         XmNsensitive, True,
          		                         XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
          		                         XmNtopWidget,	       xtas_winput_netlist_file,
          		                         XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
          		                         XmNbottomWidget,      xtas_winput_netlist_file,
                                         XmNshadowType,        XmSHADOW_ETCHED_OUT,
                                         XmNleftAttachment,    XmATTACH_WIDGET,
                                         XmNleftWidget,        xtas_winput_netlist_file,
                                         XmNshadowThickness,   2,
                                         NULL);
        
        XtAddCallback(button, XmNactivateCallback, XtasSessionFileCallback, (XtPointer)XTAS_NETLIST_FILE);
        
        
        /***********  subpart2 : Techno File  **********/
        n = 0;
        XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
        XtSetArg( args[n], XmNtopWidget,        ebase_form        ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        xtas_techno_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
        
        text = XmStringCreateSimple("Techno File");
        n = 0;
        XtSetArg( args[n], XmNlabelString,      text              ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
        XtSetArg( args[n], XmNheight,           23                ); n++;
        XtSetArg( args[n], XmNwidth,            90                ); n++;
        label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, xtas_techno_form, args, n );
        XmStringFree( text );
        
        n = 0;
        XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,             2                 ); n++;
        XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
        XtSetArg( args[n], XmNrightPosition,         90               ); n++;
        XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
        XtSetArg( args[n], XmNleftOffset,            30                ); n++;
        XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
        xtas_winput_techno_file = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, xtas_techno_form, args,n);
        XmTextSetString(xtas_winput_techno_file,tas_tmppar->tec);
        
        XtVaGetValues(form, XmNforeground, &fg, XmNbackground, &bg, NULL);
        pixmap = XalGetPixmap(ebase_form, 
                             XTAS_OPEN_MAP, fg, bg);
        
        button = XtVaCreateManagedWidget("dbutton", xmDrawnButtonWidgetClass, xtas_techno_form,
                                         XmNlabelType,         XmPIXMAP,
                                         XmNlabelPixmap,       pixmap,
                                         XmNwidth,             40,
                                         XmNheight,            35,
                                         XmNpushButtonEnabled, True,
                                         XmNsensitive, True,
          		                         XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
          		                         XmNtopWidget,	       xtas_winput_techno_file,
          		                         XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
          		                         XmNbottomWidget,      xtas_winput_techno_file,
                                         XmNshadowType,        XmSHADOW_ETCHED_OUT,
                                         XmNleftAttachment,    XmATTACH_WIDGET,
                                         XmNleftWidget,        xtas_winput_techno_file,
                                         XmNshadowThickness,   2,
                                         NULL);
        
        XtAddCallback(button, XmNactivateCallback, XtasSessionFileCallback, (XtPointer)XTAS_TECHNO_FILE);
            
        /***********  subpart3 : Info File  **********/
        
        text   = XmStringCreateSimple("Read Info File" );
        toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
                                            XmNtopAttachment,     XmATTACH_WIDGET,
                                            XmNtopWidget,         xtas_techno_form,
                                            XmNtopOffset,         2,
                                            XmNleftAttachment,    XmATTACH_FORM,
                                            XmNleftOffset,        5,
                                            XmNindicatorType,     XmN_OF_MANY,
                                            XmNset,               False,
                                            XmNlabelString,       text,
                                            XmNheight,            20,
                                            NULL);
//        if(TOOL == TAS_TOOL)
            XtVaSetValues( toggle, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, 5, NULL);
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasReadInfoCallback, (XtPointer)NULL );
        
        
//        if(TOOL == TMA_TOOL) {
//            /***********  subpart4 : Filein File  **********/
//            
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNtopWidget,        toggle            ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
//        
//            text   = XmStringCreateSimple("Library File" );
//            toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, ebase_form,
//                                              XmNindicatorType, XmN_OF_MANY,
//                                              XmNset,           False,
//                                              XmNspacing,       10,
//                                              XmNlabelString,   text,
//                                              NULL);
//            XmStringFree( text );
//        
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopWidget,        ebase_form        ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,       toggle            ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
//            
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
//            XtSetArg( args[n], XmNrightPosition,         85                ); n++;
//            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
//            XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
//            xtas_winput_filein_file = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args,n);
//            
//            XtVaGetValues(form, XmNforeground, &fg, XmNbackground, &bg, NULL);
//            pixmap = XalGetPixmap(ebase_form, 
//                                 XTAS_OPEN_MAP, fg, bg);
//        
//            button = XtVaCreateManagedWidget("dbutton", xmDrawnButtonWidgetClass, tmp_widget,
//                                             XmNlabelType,         XmPIXMAP,
//                                             XmNlabelPixmap,       pixmap,
//                                             XmNwidth,             40,
//                                             XmNheight,            35,
//                                             XmNpushButtonEnabled, True,
//          			                         XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
//          			                         XmNtopWidget,	       xtas_winput_filein_file,
//          			                         XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
//          			                         XmNbottomWidget,      xtas_winput_filein_file,
//                                             XmNshadowType,        XmSHADOW_ETCHED_OUT,
//                                             XmNleftAttachment,    XmATTACH_WIDGET,
//                                             XmNleftWidget,        xtas_winput_filein_file,
//                                             XmNshadowThickness,   2,
//                                             NULL);
//        
//            XtAddCallback(button, XmNactivateCallback, XtasSessionFileCallback, (XtPointer)XTAS_CONTEXT->TAS_FILEIN_FILE);
//            
//            XtSetSensitive(tmp_widget, False);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasFileInCallback, (XtPointer)tmp_widget);
//        
//            /***********  subpart5 : ReadArea File  **********/
//            
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNtopWidget,        ebase_form        ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
//        
//            text   = XmStringCreateSimple("Read Area From File" );
//            toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, ebase_form,
//                                              XmNindicatorType, XmN_OF_MANY,
//                                              XmNset,           False,
//                                              XmNspacing,       10,
//                                              XmNlabelString,   text,
//                                              NULL);
//            XmStringFree( text );
//        
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopWidget,        ebase_form        ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,       toggle            ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
//            
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
//            XtSetArg( args[n], XmNrightPosition,         85                ); n++;
//            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
//            XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
//            xtas_winput_readarea_file = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args,n);
//            
//            XtVaGetValues(form, XmNforeground, &fg, XmNbackground, &bg, NULL);
//            pixmap = XalGetPixmap(ebase_form, 
//                                 XTAS_OPEN_MAP, fg, bg);
//        
//            button = XtVaCreateManagedWidget("dbutton", xmDrawnButtonWidgetClass, tmp_widget,
//                                             XmNlabelType,         XmPIXMAP,
//                                             XmNlabelPixmap,       pixmap,
//                                             XmNwidth,             40,
//                                             XmNheight,            35,
//                                             XmNpushButtonEnabled, True,
//          			                         XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
//          			                         XmNtopWidget,	       xtas_winput_readarea_file,
//          			                         XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
//          			                         XmNbottomWidget,      xtas_winput_readarea_file,
//                                             XmNshadowType,        XmSHADOW_ETCHED_OUT,
//                                             XmNleftAttachment,    XmATTACH_WIDGET,
//                                             XmNleftWidget,        xtas_winput_readarea_file,
//                                             XmNshadowThickness,   2,
//                                             NULL);
//        
//            XtAddCallback(button, XmNactivateCallback, XtasSessionFileCallback, (XtPointer)XTAS_READAREA_FILE);
//            
//            XtSetSensitive(tmp_widget, False);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasFileInCallback, (XtPointer)tmp_widget );
//        
//        }
        

        /***********  part3 : Output Files  **********/

//        if(TOOL == TMA_TOOL) {
//            n = 0;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_POSITION ); n++;
//            XtSetArg( args[n], XmNbottomPosition,   66                ); n++;
//            sub_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, dbase_form, args, n );
//            frame3 = sub_form;
//
//            n = 0;
//            sub_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
//
//            text = XmStringCreateSimple( " TMA Output Files" );
//            tmp_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
//                    XmNlabelString,     text,
//                    XmNtopAttachment,   XmATTACH_FORM,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNtopOffset,       2,
//                    XmNrightOffset,     2,
//                    XmNleftOffset,      2,
//                    NULL);
//            XmStringFree( text );
//
//        /***********  subpart3.1 : ttx  **********/
//            text   = XmStringCreateSimple("ttx" );
//            toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
//                                              XmNindicatorType, XmN_OF_MANY,
//                                              XmNleftAttachment,XmATTACH_FORM,
//                                              XmNleftOffset,    5,
//                                              XmNtopAttachment, XmATTACH_WIDGET,
//                                              XmNtopWidget,     tmp_widget,
//                                              XmNtopOffset,     30,
//                                              XmNset,           ((tma_tmppar->out & TMA_OUT_TTX) == TMA_OUT_TTX) ? True:False,
//                                              XmNlabelString,   text,
//                                              NULL);
//            XmStringFree( text );
//        
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasOutputTtxCallback, (XtPointer)tmp_widget);
//        
//        /***********  subpart3.2 : lib  **********/
//            text   = XmStringCreateSimple("lib" );
//            toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
//                                              XmNtopAttachment,     XmATTACH_WIDGET,
//                                              XmNtopWidget,         toggle,
//                                              XmNtopOffset,         29,
//                                              XmNleftAttachment,    XmATTACH_FORM,
//                                              XmNleftOffset,        5,
//                                              XmNindicatorType,     XmN_OF_MANY,
//                                              XmNset,               ((tma_tmppar->out & TMA_OUT_LIB) == TMA_OUT_LIB) ? True:False,
//                                              XmNlabelString,       text,
//                                              NULL);
//            XmStringFree( text );
//
//        /***********  subpart3.3 : tlf3  **********/
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNtopWidget,        toggle            ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
//        
//            text   = XmStringCreateSimple("tlf3" );
//            toggle2 = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, ebase_form,
//                                              XmNtopAttachment, XmATTACH_FORM,
//                                              XmNtopOffset,     4,
//                                              XmNindicatorType, XmN_OF_MANY,
//                                              XmNset,           ((tma_tmppar->out & TMA_OUT_TLF3) == TMA_OUT_TLF3) ? True:False,
//                                              XmNlabelString,   text,
//                                              NULL);
//            XmStringFree( text );
//        
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftOffset,       10                ); n++;
//            XtSetArg( args[n], XmNleftWidget,       toggle2           ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
//            
//
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
////            XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
////            XtSetArg( args[n], XmNrightPosition,         85                ); n++;
//            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,            label_widget      ); n++;
//            XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
//            xtas_woutput_name = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args,n);
//            XmTextSetString( xtas_woutput_name, tma_tmppar->nameout);
//
//            XtVaGetValues(form, XmNforeground, &fg, XmNbackground, &bg, NULL);
//            pixmap = XalGetPixmap(ebase_form, 
//                                 XTAS_OPEN_MAP, fg, bg);
//        
//            button = XtVaCreateManagedWidget("dbutton", xmDrawnButtonWidgetClass, tmp_widget,
//                                             XmNlabelType,         XmPIXMAP,
//                                             XmNlabelPixmap,       pixmap,
//                                             XmNwidth,             40,
//                                             XmNheight,            35,
//                                             XmNpushButtonEnabled, True,
//          			                         XmNtopAttachment,     XmATTACH_OPPOSITE_WIDGET,
//          			                         XmNtopWidget,	       xtas_woutput_name,
//          			                         XmNbottomAttachment,  XmATTACH_OPPOSITE_WIDGET,
//          			                         XmNbottomWidget,      xtas_woutput_name,
//                                             XmNshadowType,        XmSHADOW_ETCHED_OUT,
//                                             XmNleftAttachment,    XmATTACH_WIDGET,
//                                             XmNleftWidget,        xtas_woutput_name,
////                                             XmNrightAttachment,   XmATTACH_FORM,
////                                             XmNrightOffset,       5,
//                                             XmNshadowThickness,   2,
//                                             NULL);
//        
//            XtAddCallback(button, XmNactivateCallback, XtasSessionFileCallback, (XtPointer)XTAS_TMA_OUTPUT_FILE );
//           
//            if ((tma_tmppar->out == 0) || ((tma_tmppar->out & TMA_OUT_TTX) == TMA_OUT_TTX))
//                XtSetSensitive(tmp_widget, False);
//            else
//                XtSetSensitive(tmp_widget, True);
//            XtAddCallback( toggle, XmNvalueChangedCallback,  XtasOutputLibCallback, (XtPointer)tmp_widget);
//            XtAddCallback( toggle2, XmNvalueChangedCallback, XtasOutputTlf3Callback, (XtPointer)tmp_widget);
//
//            
//            /***********  subpart3.4 : tlf4  **********/
//            text   = XmStringCreateSimple("tlf4" );
//            toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, sub_form,
//                                              XmNtopAttachment,     XmATTACH_WIDGET,
//                                              XmNtopWidget,         ebase_form,
//                                              XmNtopOffset,         5,
//                                              XmNleftAttachment,    XmATTACH_FORM,
//                                              XmNleftOffset,        5,
//                                              XmNindicatorType,     XmN_OF_MANY,
//                                              XmNset,               ((tma_tmppar->out & TMA_OUT_TLF4) == TMA_OUT_TLF4) ? True:False,
//                                              XmNlabelString,       text,
//                                              NULL);
//            XmStringFree( text );
//            XtAddCallback( toggle, XmNvalueChangedCallback,  XtasOutputTlf4Callback, (XtPointer)tmp_widget);
//
//
//        /***********  part4 : Tma options  **********/
//
//            n = 0;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopWidget,        frame             ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_POSITION ); n++;
//            XtSetArg( args[n], XmNbottomPosition,   66                ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_OPPOSITE_WIDGET ); n++;
//            XtSetArg( args[n], XmNrightWidget,      frame2            ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,       frame3            ); n++;
//            XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
//            sub_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, dbase_form, args, n );
//
////            XtVaSetValues(frame3, XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET, 
////                                  XmNbottomWidget, sub_form, NULL);
//            
//            frame3 = sub_form;
//
//            n = 0;
//            sub_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );
//
//            text = XmStringCreateSimple( "TMA Options" );
//            title = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
//                    XmNlabelString,     text,
//                    XmNtopAttachment,   XmATTACH_FORM,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNtopOffset,       2,
//                    XmNrightOffset,     2,
//                    XmNleftOffset,      2,
//                    NULL);
//            XmStringFree( text );
//
//            n = 0;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopWidget,        title             ); n++;
//            XtSetArg( args[n], XmNtopOffset,        10                ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
//            fbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFormWidgetClass, sub_form, args, n );
//            
//            
//            /***********  subpart4.1 : behavioural description  **********/
//            text = XmStringCreateSimple ("Generate a behavioral description");
//            toggle2 = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
//                    XmNtopAttachment,      XmATTACH_WIDGET,
//                    XmNtopWidget,          label_widget,
//                    XmNleftAttachment,     XmATTACH_FORM,
//                    XmNleftOffset,         5,
//                    XmNrightAttachment,    XmATTACH_FORM,
//                    XmNindicatorType,      XmN_OF_MANY,
//                    XmNset,                tma_tmppar->mod == 'T' ? False:True,
//                    XmNlabelString,        text,
//                    XmNalignment,          XmALIGNMENT_BEGINNING,
//                    NULL);
//            XmStringFree(text);
//
//
//            row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, fbase_form,
//                    XmNtopAttachment,   XmATTACH_WIDGET,
//                    XmNtopWidget,       toggle2,
//                    XmNtopOffset,       0,
//                    XmNleftOffset,      15,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNpacking,         XmPACK_COLUMN,
//                    XmNnumColumns,      1,
//                    XmNorientation,     XmVERTICAL,
//                    XmNisAligned,       True,
//                    XmNradioBehavior,   True,
//                    XmNradioAlwaysOne,  True,
//                    XmNsensitive,       tma_tmppar->mod == 'T' ? False:True,
//                    NULL);
//
//
//            XtAddCallback( toggle2, XmNvalueChangedCallback, XtasTmaOptGenerateBehCallback, (XtPointer)row_widget) ;
//            text = XmStringCreateSimple ("Automatically");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//                    XmNindicatorType,  XmONE_OF_MANY,
//                    XmNset,            ((tma_tmppar->mod == 'W') || (tma_tmppar->mod == 'T')) ? True:False,
//                    XmNlabelString,    text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaModeCallback, (XtPointer)'A') ;
//
//
//            text = XmStringCreateSimple ("From Behavioral File");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//                    XmNindicatorType,  XmONE_OF_MANY,
//                    XmNset,            tma_tmppar->mod == 'R' ? True:False,
//                    XmNlabelString,    text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaModeCallback, (XtPointer)'F') ;
//
////            text = XmStringCreateSimple ("From Verilog File");
////            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
////                    XmNindicatorType,  XmONE_OF_MANY,
////                    XmNset,            False,
////                    XmNlabelString,    text,
////                    NULL);
////            XmStringFree(text);
////            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaModeCallback, (XtPointer)'f') ;
////
//
//            /***********  subpart4.2 : margin  **********/
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNtopWidget,        row_widget        ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, fbase_form, args, n );
//        
//            text   = XmStringCreateSimple("Margin" );
//            toggle = XtVaCreateManagedWidget( "XtasSubTitles", xmToggleButtonWidgetClass, ebase_form,
//                                              XmNtopAttachment, XmATTACH_FORM,
//                                              XmNtopOffset,     4,
//                                              XmNindicatorType, XmN_OF_MANY,
//                                              XmNset,           strlen(tma_tmppar->mrg) > 0 ? True:False,
//                                              XmNlabelString,   text,
//                                              NULL);
//            XmStringFree( text );
//        
//            n = 0;
//            XtSetArg( args[n], XmNshadowThickness,  0                 ); n++;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftOffset,       10                ); n++;
//            XtSetArg( args[n], XmNleftWidget,       toggle            ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            tmp_widget = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, ebase_form, args, n );
//            
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,         XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
////            XtSetArg( args[n], XmNrightAttachment,       XmATTACH_POSITION ); n++;
////            XtSetArg( args[n], XmNrightPosition,         85                ); n++;
//            XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM   ); n++;
//            XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
//            xtas_wopt_mrg = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, tmp_widget, args,n);
//            XmTextSetString( xtas_wopt_mrg, tma_tmppar->mrg);
//
//            text = XmStringCreateSimple("ps");
//            n = 0;
//            XtSetArg( args[n], XmNlabelString,      text              ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNtopOffset,        4     ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,       xtas_wopt_mrg     ); n++;
////            XtSetArg( args[n], XmNtopOffset,        8                 ); n++;
////            XtSetArg( args[n], XmNheight,           23                ); n++;
////            XtSetArg( args[n], XmNwidth,            90                ); n++;
//            label_widget = XtCreateManagedWidget( "XtasSubTitles", xmLabelGadgetClass, tmp_widget, args, n );
//            XmStringFree( text );
//        
//            if(strlen(tma_tmppar->mrg) > 0)
//                XtSetSensitive(tmp_widget, True);
//            else 
//                XtSetSensitive(tmp_widget, False);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaMarginCallback, (XtPointer)tmp_widget);
//        
//
//            /***********  subpart4.3 : no automatic detection of clocks  **********/
//            
//            text = XmStringCreateSimple ("Automatic Detection of clocks");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
//                    XmNtopAttachment,      XmATTACH_WIDGET,
//                    XmNtopOffset,          5,
//                    XmNtopWidget,          ebase_form,
//                    XmNleftAttachment,     XmATTACH_FORM,
//                    XmNleftOffset,         5,
//                    XmNindicatorType,      XmN_OF_MANY,
//                    XmNset,                tma_tmppar->otock == 'Y' ? True:False,
//                    XmNlabelString,        text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaNacCallback, (XtPointer)NULL);
//
//            /***********  subpart4.4 : verbose  **********/
//            
//            text = XmStringCreateSimple ("Verbose");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, fbase_form,
//                    XmNtopAttachment,      XmATTACH_WIDGET,
//                    XmNtopWidget,          toggle,
//                    XmNtopOffset,          5,
//                    XmNleftAttachment,     XmATTACH_FORM,
//                    XmNleftOffset,         5,
//                    XmNindicatorType,      XmN_OF_MANY,
//                    XmNset,                tma_tmppar->v == 'Y' ? True:False,
//                    XmNlabelString,        text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaVerbCallback, (XtPointer)NULL);
//
//            /***************************** vertical separator *******************************/
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET );n++; 
//            XtSetArg( args[n], XmNtopWidget,        title           );n++; 
//            XtSetArg( args[n], XmNtopOffset,        15              );n++; 
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );n++; 
//            XtSetArg( args[n], XmNleftWidget,       fbase_form      );n++; 
//            XtSetArg( args[n], XmNleftOffset,       5              );n++; 
//            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   );n++; 
//            XtSetArg( args[n], XmNbottomOffset,     10              );n++; 
//            XtSetArg( args[n], XmNorientation,      XmVERTICAL      );n++; 
//            separator = XtCreateManagedWidget("XtasSeparator", xmSeparatorWidgetClass, sub_form, args, n);
//
//            n = 0;
//            XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNtopWidget,        title             ); n++;
//            XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
//            XtSetArg( args[n], XmNleftWidget,       separator         ); n++;
//            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
//            fbase_form = XtCreateManagedWidget( "XtasTitleFrame", xmFormWidgetClass, sub_form, args, n );
//            
//
//            /***********  subpart4.5 : Units  **********/
//            text = XmStringCreateSimple( "Units" );
//            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, fbase_form,
//                    XmNlabelString,     text,
//                    XmNtopAttachment,   XmATTACH_FORM,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNtopOffset,       2,
//                    XmNrightOffset,     2,
//                    XmNleftOffset,      2,
//                    NULL);
//            XmStringFree( text );
//            title = label_widget;
//
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
//            XtSetArg( args[n], XmNtopWidget,        label_widget    ); n++;
//            XtSetArg( args[n], XmNtopOffset,        15              ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM   ); n++;
//            ebase_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, fbase_form, args, n);
//
//            n = 0;
//            text = XmStringCreateSimple( "Time unit" );
//            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, ebase_form,
//                    XmNlabelString,     text,
//                    XmNtopAttachment,   XmATTACH_FORM,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNtopOffset,       2,
//                    XmNrightOffset,     2,
//                    XmNleftOffset,      2,
//                    NULL);
//            XmStringFree( text );
//
//            row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
//                    XmNtopAttachment,   XmATTACH_WIDGET,
//                    XmNtopWidget,       label_widget,
//                    XmNtopOffset,       15,
//                    XmNleftOffset,      30,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNpacking,         XmPACK_COLUMN,
//                    XmNnumColumns,      2,
//                    XmNorientation,     XmVERTICAL,
//                    XmNspacing,         35,
//                    XmNisAligned,       True,
//                    XmNradioBehavior,True,
//                    XmNradioAlwaysOne,True,
//                    NULL);
//
//            text = XmStringCreateSimple ("ns");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//                    XmNindicatorType,  XmONE_OF_MANY,
//                    XmNset,            tma_tmppar->tu == TMA_NS ? True:False,
//                    XmNlabelString,    text,
//                    NULL);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaUnitCallback, (XtPointer)'S') ;
//            XmStringFree(text);
//
//            text = XmStringCreateSimple ("ps");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//                    XmNindicatorType,  XmONE_OF_MANY,
//                    XmNset,            tma_tmppar->tu == TMA_PS ? True:False,
//                    XmNlabelString,    text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaUnitCallback, (XtPointer)'s') ;
//
//
//            n = 0;
//            XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
//            XtSetArg( args[n], XmNtopWidget,        ebase_form     ); n++;
//            XtSetArg( args[n], XmNtopOffset,        25             ); n++;
//            XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM  ); n++;
//            XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM  ); n++;
//            XtSetArg( args[n], XmNbottomOffset,     10             ); n++;
//            XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM  ); n++;
//            ebase_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, fbase_form, args, n);
//
//            n = 0;
//            text = XmStringCreateSimple( "Capacitance unit" );
//            label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, ebase_form,
//                    XmNlabelString,     text,
//                    XmNtopAttachment,   XmATTACH_FORM,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNtopOffset,       2,
//                    XmNrightOffset,     2,
//                    XmNleftOffset,      2,
//                    NULL);
//            XmStringFree( text );
//
//            row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
//                    XmNtopAttachment,   XmATTACH_WIDGET,
//                    XmNtopWidget,       label_widget,
//                    XmNtopOffset,       15,
//                    XmNleftOffset,      35,
//                    XmNleftAttachment,  XmATTACH_FORM,
//                    XmNrightAttachment, XmATTACH_FORM,
//                    XmNpacking,         XmPACK_COLUMN,
//                    XmNnumColumns,      2,
//                    XmNorientation,     XmVERTICAL,
//                    XmNspacing,         35,
//                    XmNisAligned,       True,
//                    XmNradioBehavior,True,
//                    XmNradioAlwaysOne,True,
//                    NULL);
//
//            text = XmStringCreateSimple ("pF");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//                    XmNindicatorType,  XmONE_OF_MANY,
//                    XmNset,            tma_tmppar->cu == TMA_PF ? True:False,
//                    XmNlabelString,    text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaUnitCallback, (XtPointer)'f') ;
//
//            text = XmStringCreateSimple ("fF");
//            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
//                    XmNindicatorType,  XmONE_OF_MANY,
//                    XmNset,            tma_tmppar->cu == TMA_FF ? True:False,
//                    XmNlabelString,    text,
//                    NULL);
//            XmStringFree(text);
//            XtAddCallback( toggle, XmNvalueChangedCallback, XtasTmaUnitCallback, (XtPointer)'F') ;
//        }

        /***********  part5 : Tas options  **********/
        n = 0;
        XtSetArg ( args[n], XmNtopAttachment,   XmATTACH_WIDGET   ); n++;
        if (TOOL == TMA_TOOL) {
            XtSetArg ( args[n], XmNtopWidget,       frame3            ); n++;}
        else {
            XtSetArg ( args[n], XmNtopWidget,       frame            ); n++;}
            
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        
        tas_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, dbase_form, args, n );
        if ((TOOL==TMA_TOOL)&&((tma_tmppar->ttxin=='Y')||(tma_tmppar->dtxin=='Y')))
            XtSetSensitive (tas_form, False);
        else 
            XtSetSensitive (tas_form, True);
        
        n = 0;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
        sub_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, tas_form, args, n );
        frame = sub_form;

        n = 0;
        sub_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );

        text = XmStringCreateSimple( " TAS Output Files" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNtopOffset,       2,
                XmNrightOffset,     2,
                XmNleftOffset,      2,
                NULL);
        XmStringFree( text );

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNtopWidget,        label_widget      ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM     ); n++;
        ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );

        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNtopOffset,       15,
                XmNleftOffset,      5,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNbottomAttachment, XmATTACH_FORM,
                XmNpacking,         XmPACK_COLUMN,
                XmNnumColumns,      1,
                XmNorientation,     XmVERTICAL,
                XmNisAligned,       True,
                NULL);

        for(type=0; type < (int)(long)XtNumber(tas_opt_output); type++)
        {
            Widget toggle;

            text = XmStringCreateSimple (tas_opt_output[type]);
            toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                    XmNindicatorType,  XmN_OF_MANY,
                    XmNlabelString,    text,
                    NULL);
            XmStringFree(text);
            XtAddCallback( toggle, XmNvalueChangedCallback, XtasOutputFileFormatCallback, (XtPointer)type );

            switch(type) {
                case 0  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->ttx == 'Y' ? True:False, NULL); break;
                case 1  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->dtx == 'Y' ? True:False, NULL); break;
                case 2  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->c   == 'Y' ? True:False, NULL); break;
                case 3  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->e   == 'Y' ? True:False, NULL); break;
                case 4  :   XtVaSetValues ( toggle, XmNset, tas_tmppar->s   == 'Y' ? True:False, NULL); break;
            }
        }

        n = 0;
        XtSetArg(args[n], XmNleftAttachment,    XmATTACH_WIDGET ); n++;
        XtSetArg(args[n], XmNleftWidget,        ebase_form      ); n++;
        XtSetArg(args[n], XmNrightAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg(args[n], XmNtopAttachment,     XmATTACH_WIDGET ); n++;
        XtSetArg(args[n], XmNtopWidget,         label_widget    ); n++;
        ebase_form = XtCreateManagedWidget( "XtasTitleForm", xmFormWidgetClass, sub_form, args, n );

        button2 = XtVaCreateManagedWidget("Choose the file name", xmPushButtonWidgetClass, ebase_form,
                XmNtopAttachment,      XmATTACH_FORM,
                XmNtopOffset,          45,
                XmNleftAttachment,     XmATTACH_FORM,
                XmNrightAttachment,    XmATTACH_FORM,
                XmNrightOffset,        5,
                NULL);

        /*-------------------------------------------------*/
        /* if this button is selected, execute the program */
        /*-------------------------------------------------*/

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,         XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNtopWidget,             button2           ); n++;
        XtSetArg( args[n], XmNtopOffset,             5                 ); n++;
        XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightOffset,            5                 ); n++;
        XtSetArg( args[n], XmNleftAttachment,        XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNrightAttachment,       XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNbottomAttachment,      XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNshadowThickness,       1                 ); n++;
        xtas_woutput_file_name = XtCreateManagedWidget( "XtasUserField", xmTextWidgetClass, ebase_form, args,n);

        XtAddCallback(button2, XmNactivateCallback, XtasSessionFileCallback,  (XtPointer)XTAS_TAS_OUTPUT_FILE);

        n = 0;
        XtSetArg( args[n], XmNborderWidth,      0                 ); n++;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM     ); n++;
        XtSetArg( args[n], XmNtopOffset,        5                 ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET   ); n++;
        XtSetArg( args[n], XmNleftOffset,       5                 ); n++;
        XtSetArg( args[n], XmNleftWidget,       frame             ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_OPPOSITE_WIDGET     ); n++;
        XtSetArg( args[n], XmNbottomWidget,     frame             ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM     ); n++;
        sub_form = XtCreateManagedWidget( "XtasTitleFrame", xmFrameWidgetClass, tas_form, args, n );

        n = 0;
        sub_form = XtCreateManagedWidget( "XtasForm", xmFormWidgetClass, sub_form, args, n );


        text = XmStringCreateSimple( "TAS General Options" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, sub_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNtopOffset,       2,
                XmNrightOffset,     2,
                XmNleftOffset,      2,
                NULL);
        XmStringFree( text );
        title = label_widget;

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET ); n++;
        XtSetArg( args[n], XmNtopWidget,        label_widget    ); n++;
        XtSetArg( args[n], XmNtopOffset,        25              ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM   ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   ); n++;
        ebase_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, sub_form, args, n);

        n = 0;
        text = XmStringCreateSimple( "Analysis Type" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, ebase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNtopOffset,       2,
                XmNrightOffset,     2,
                XmNleftOffset,      22,
                NULL);
        XmStringFree( text );

        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                XmNtopAttachment,   XmATTACH_WIDGET,
                XmNtopWidget,       label_widget,
                XmNtopOffset,       15,
                XmNleftOffset,      10,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNpacking,         XmPACK_COLUMN,
                XmNnumColumns,      1,
                XmNorientation,     XmVERTICAL,
                XmNisAligned,       True,
                XmNradioBehavior,True,
                XmNradioAlwaysOne,True,
                NULL);

        text = XmStringCreateSimple ("Transistor Level");
        toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType,  XmONE_OF_MANY,
                XmNset,            tas_tmppar->hr == 'N' ? True:False,
                XmNlabelString,    text,
                NULL);
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasGeneralOptionsCallback, (XtPointer)'T') ;
        XmStringFree(text);

        text = XmStringCreateSimple ("Hierarchical");
        toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType,  XmONE_OF_MANY,
                XmNset,            tas_tmppar->hr == 'Y' ? True:False,
                XmNlabelString,    text,
                NULL);
        XmStringFree(text);
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasGeneralOptionsCallback, (XtPointer)'H') ;
        tas_tmppar->hr = 'N';

        /***************************** vertical separator *******************************/
        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET );n++; 
        XtSetArg( args[n], XmNtopWidget,        title           );n++; 
        XtSetArg( args[n], XmNtopOffset,        25              );n++; 
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET );n++; 
        XtSetArg( args[n], XmNleftWidget,       ebase_form      );n++; 
        XtSetArg( args[n], XmNleftOffset,       20              );n++; 
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM   );n++; 
        XtSetArg( args[n], XmNbottomOffset,     10              );n++; 
        XtSetArg( args[n], XmNorientation,      XmVERTICAL      );n++; 
        separator = XtCreateManagedWidget("XtasSeparator", xmSeparatorWidgetClass, sub_form, args, n);

        n = 0;
        XtSetArg( args[n], XmNtopAttachment,    XmATTACH_WIDGET); n++;
        XtSetArg( args[n], XmNtopWidget,        title          ); n++;
        XtSetArg( args[n], XmNtopOffset,        25             ); n++;
        XtSetArg( args[n], XmNleftAttachment,   XmATTACH_WIDGET); n++;
        XtSetArg( args[n], XmNleftWidget,       separator      ); n++;
        XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM  ); n++;
        XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM  ); n++;
        ebase_form = XtCreateManagedWidget("XtasForms", xmFormWidgetClass, sub_form, args, n);

        n = 0;
        text = XmStringCreateSimple( "Critical Paths" );
        label_widget = XtVaCreateManagedWidget( "XtasMainTitles",xmLabelWidgetClass, ebase_form,
                XmNlabelString,     text,
                XmNtopAttachment,   XmATTACH_FORM,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNtopOffset,       2,
                XmNrightOffset,     2,
                XmNleftOffset,      2,
                NULL);
        XmStringFree( text );

        row_widget = XtVaCreateManagedWidget( "XtasRow", xmRowColumnWidgetClass, ebase_form,
                XmNtopAttachment,   XmATTACH_WIDGET,
                XmNtopWidget,       label_widget,
                XmNtopOffset,       15,
                XmNleftOffset,      10,
                XmNleftAttachment,  XmATTACH_FORM,
                XmNrightAttachment, XmATTACH_FORM,
                XmNpacking,         XmPACK_COLUMN,
                XmNnumColumns,      1,
                XmNorientation,     XmVERTICAL,
                XmNisAligned,       True,
                XmNradioBehavior,True,
                XmNradioAlwaysOne,True,
                NULL);

        text = XmStringCreateSimple ("Long Paths Only");
        toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType,  XmONE_OF_MANY,
                XmNset,            tas_tmppar->min == 'N' ? True:False,
                XmNlabelString,    text,
                NULL);
        XmStringFree(text);
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasGeneralOptionsCallback, (XtPointer)'L') ;

        text = XmStringCreateSimple ("Long Paths and Short Paths");
        toggle = XtVaCreateManagedWidget("XtasSubTitles", xmToggleButtonWidgetClass, row_widget,
                XmNindicatorType,  XmONE_OF_MANY,
                XmNset,            tas_tmppar->min == 'Y' ? True:False,
                XmNlabelString,    text,
                NULL);
        XmStringFree(text);
        XtAddCallback( toggle, XmNvalueChangedCallback, XtasTasGeneralOptionsCallback, (XtPointer)'S') ;
    }
    XtManageChild( session_widget );
//    XalLimitedLoop( session_widget );
}

