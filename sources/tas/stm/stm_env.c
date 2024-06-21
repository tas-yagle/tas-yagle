/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_env.c                                                   */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

chain_list *STM_CELLS_CHAIN     = NULL;
chain_list *STM_TEMPLATES_CHAIN = NULL;
chain_list *STM_TEMPLATE_NAME   = NULL;

ht         *STM_CELLS_HT        = NULL;
ht         *STM_TEMPLATES_HT    = NULL;

char       *STM_IN              = "stm";
char       *STM_OUT             = "stm";

float       STM_DEF_SLEW        = (float)200.0;
float       STM_DEF_LOAD        = (float)0.0;
float       STM_DEFAULT_VT      = (float)0.2;
float       STM_DEFAULT_VTN     = (float)0.2;
float       STM_DEFAULT_VTP     = (float)0.2;
float       STM_DEFAULT_VFD     = (float)0.0;
float       STM_DEFAULT_VFU     = (float)1.0;
float       STM_DEFAULT_TEMP    = (float)25;
float       STM_DEFAULT_SMINR   = (float)(0.2);
float       STM_DEFAULT_SMAXR   = (float)(0.8);
float       STM_DEFAULT_SMINF   = (float)(0.2);
float       STM_DEFAULT_SMAXF   = (float)(0.8);
float       STM_DEFAULT_RLIN    = (float)1000.0;
float       STM_DEFAULT_VSAT    = (float)1.0;
float      *STM_AXIS_SLOPEIN    = NULL ;
float      *STM_AXIS_CAPAOUT    = NULL ;
int         STM_AXIS_NSLOPE     = 0 ;
int         STM_AXIS_NCAPA      = 0 ;
int         STM_BUFSIZE         = 1024;
int         STM_DEBUG           = 0;
//int         STM_SHARE_MODEL     = 0;
int         STM_SOLVER_PILOAD   = 0;
int         STM_IMAX_FOR_PILOAD = 0;
float       STM_OVERSHOOT       = 0.01;

char       *STM_TECHNONAME      = "unknown";

unsigned long int STM_CACHE_SIZE=0; /* default  = 0 */

//int         STM_USE_MSC       = NO;


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void stmenv () 
{
    const char *str, *ptend ;
    
    stm_init();
    
/*
    str = avt_gethashvar ("STM_TRACE_MODE");
    if (str)
        if (!strcmp (str, "yes"))
            STM_DEBUG = 1;
    str = V_BOOL_TAB[__STM_SHARE_MODEL].VALUE;
    if (str)
        if (!strcmp (str, "yes"))
            STM_SHARE_MODEL = 1;
            */

    str = avt_gethashvar ("STM_SOLVER_PILOAD");
    if (str)
        if (!strcmp (str, "yes"))
            STM_SOLVER_PILOAD = 1;
   
    str = avt_gethashvar ("STM_IMAX_FOR_PILOAD" );
    if (str)
        if (!strcmp (str, "yes"))
            STM_IMAX_FOR_PILOAD = 1;
    str = getenv("STM_OVERSHOOT") ;
    if (str){
        STM_OVERSHOOT = atof(str) / 100.0 ;
    }

    str = V_STR_TAB[__STM_CACHESIZE].VALUE;
    if( str ) {
        STM_CACHE_SIZE = strtol( str, &ptend, 10 );
        if( *ptend != '\0' ) {
            if( strcasecmp( ptend, "kb" )==0 )
                STM_CACHE_SIZE = STM_CACHE_SIZE * 1024;
            else if( strcasecmp( ptend, "mb" )==0 )
                STM_CACHE_SIZE = STM_CACHE_SIZE * 1048576;
            else if( strcasecmp( ptend, "gb" )==0 )
                STM_CACHE_SIZE = STM_CACHE_SIZE * 1073741824;
            else 
                avt_errmsg (STM_ERRMSG,"037", AVT_FATAL);
        }
    }

    /*
    str = V_BOOL_TAB[__STM_USE_MSC].VALUE;
    if (str) {
        if (!strcmp (str, "no"))
            STM_USE_MSC = NO;
        else
        if (!strcmp (str, "yes"))
            STM_USE_MSC = YES;
    }
            */

    str = getenv( "STM_PWL_DEBUG_ALLOC" );
    if( str ) {
        if (!strcmp (str, "no"))
            STM_PWL_DEBUG_ALLOC = NO;
        else
        if (!strcmp (str, "yes"))
            STM_PWL_DEBUG_ALLOC = YES;
    }
}
    
/****************************************************************************/

void stm_exit () 
{
    
    chain_list *pt;
    
    for (pt = STM_TEMPLATES_CHAIN; pt; pt = pt->NEXT)
        stm_modtbl_destroytemplate ((timing_ttable*)pt->DATA);
    if (STM_TEMPLATES_CHAIN)
        freechain (STM_TEMPLATES_CHAIN);
    STM_TEMPLATES_CHAIN = NULL;

    /*for (pt = STM_TEMPLATE_NAME; pt; pt = pt->NEXT)
        mbkfree((char*)pt->DATA);*/
    if (STM_TEMPLATE_NAME)
        freechain (STM_TEMPLATE_NAME);
    STM_TEMPLATE_NAME = NULL;

    if (STM_TEMPLATES_HT) {
        delht (STM_TEMPLATES_HT);
        STM_TEMPLATES_HT = NULL;
    }
    
/*    for (pt = STM_CELLS_CHAIN; pt; pt = pt->NEXT) 
        stm_freecell ((char*)pt->DATA);
    if (STM_CELLS_CHAIN)
        freechain (STM_CELLS_CHAIN);
    STM_CELLS_CHAIN = NULL;
    
    if (STM_CELLS_HT) {
        delht (STM_CELLS_HT);
        STM_CELLS_HT = NULL;
    }*/
}

/****************************************************************************/

void stm_init () 
{
    STM_DEF_SLEW = elpGeneral[elpSLOPE];
    if((elpGeneral[elpGSLTHR] > 0.0) && (elpGeneral[elpGSHTHR] > 0.0)){
        STM_DEFAULT_SMINR   = elpGeneral[elpGSLTHR];
        STM_DEFAULT_SMAXR   = elpGeneral[elpGSHTHR];
        STM_DEFAULT_SMINF   = elpGeneral[elpGSLTHR];
        STM_DEFAULT_SMAXF   = elpGeneral[elpGSHTHR];
    }else{
        STM_DEFAULT_SMINR   = SIM_VTH_LOW;
        STM_DEFAULT_SMAXR   = SIM_VTH_HIGH;
        STM_DEFAULT_SMINF   = SIM_VTH_LOW;
        STM_DEFAULT_SMAXF   = SIM_VTH_HIGH;
    }
    STM_DEFAULT_VFU     = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;    
    STM_DEFAULT_VFD     = (float)0.0;    
    STM_DEFAULT_VTN     = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE * 0.2;
    STM_DEFAULT_VTP     = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE * 0.2;
    STM_DEFAULT_VT      = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE * 0.2;
    if(elpGeneral[elpTEMP] > ELPMINTEMP)
        STM_DEFAULT_TEMP    = elpGeneral[elpTEMP];
    else
        STM_DEFAULT_TEMP    = V_FLOAT_TAB[__SIM_TEMP].VALUE;
}
