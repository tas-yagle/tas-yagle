/****************************************************************************/
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_simu_path.h                                            */
/*                                                                          */
/*    Author(s) : Caroline BLED                       Date : 02/25/2003     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/




extern  char    XTAS_SIMU_RES;
extern  Widget  XtasSimuParamWidget;
extern  char    XTAS_SIMU_NEW_NETLIST;

typedef struct  xtas_simu_param
{
    /* Techno File */
    char    *tec            ;

    /* Tool */
    char     tool           ;
    char    *spicestr       ;
    char    *spice_out      ;
    char    *spice_stdout   ;

    /* Data Extraction */
    char     use_print      ;
    char     use_meas       ;
/*    char    *meas_cmd       ;
    char    *extract_rule   ; */

    /* Conditions */
    double   vdd            ;
    double   temp           ;
    double   trans_time     ;
    char    *spice_options  ;  

    /* Transient */
    double   trans_step     ;

    /* Input/Output Constraints */
    double   input_start    ;
    double   input_slope    ;
    double   out_capa_val   ;

    /* Thresholds */
    double   vth            ;
    double   vth_high       ;
    double   vth_low        ;
}
xtas_simu_param_struct;

extern  xtas_simu_param_struct  *XtasSimuParams;



extern  void    XtasSimuPathCallback            __P((
                                                     Widget ,
                                                     XtPointer ,
                                                     XtPointer
                                                   )) ;

extern  void    XtasDestroySimuParams           __P((void));



