/********************************************/
/* lib_global.c :                           */
/* fichier de fonctions en  C appelees dans */
/* le fichier YACC.                         */
/********************************************/

/******************************************************************************/
/* INCLUDE                                                                    */
/******************************************************************************/
#include "lib_global.h"


/******************************************************************************/
/* GLOBALS                                                                    */
/******************************************************************************/

/******************************************************************************/
/* FONCTIONS                                                                  */
/******************************************************************************/
/*----------------------------------------------------------------------------*/
/* LIB_TREAT_GROUP_HEAD :                                                     */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_treat_group_head(char *ident, chain_list *param)
{
    long ctxt;
    chain_list *ch;
    
    lib_push_context(lib_give_context(ident));
    if((ctxt=lib_get_context()) != NO_ACTION_TYPE) {
        if(ctxt == LIB_TYPE) {
            lib_setenv();
            lib_init();
            if(LIB_TRACE_MODE == 'N') {
//                avt_error("library", -1, AVT_INFO, "loading LIB   '¤2%s¤.'\n", (char*)param->DATA);
/*                fprintf(stdout,"\nLOADING LIBRARY %s LIB... ", (char*)param->DATA);
                fflush( stdout );*/
            }
        }
        else if(ctxt == CELL_TYPE) {
            if((LIB_TRACE_MODE == 'V') || (LIB_TRACE_MODE == 'A'))
                printf("\t ---> processing cell %s\n", (char *)(param->DATA));
        }
        lib_pushgroup(ident, param);
    }
    else {
        for(ch = param ; ch ; ch=ch->NEXT) 
            mbkfree((char *)(ch->DATA));
        freechain(param);
    }                 
    mbkfree((char *)ident);
}
/*----------------------------------------------------------------------------*/
/* LIB_TREAT_GROUP_END :                                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_treat_group_end(void) 
{
    long ctxt;
    lofig_list  *plofig;
    lib_group *p;
    
    if((ctxt=lib_get_context()) != NO_ACTION_TYPE) {
        if(ctxt == LIB_TYPE) {
            lib_tim_store_default_wire_load();
            if((LIB_TRACE_MODE == 'V') || (LIB_TRACE_MODE == 'A'))
                printf("\t<--- done\n");
/*            else
                fprintf(stdout,"DONE!\n");
*/
            p=LIB_CURRENT_GP;
            
            lib_popgroup();
            lib_del_group(p);
            cbh_delcct();
            if(LIB_TIME_U){
                mbkfree((lib_scale_unit *)LIB_TIME_U);
                LIB_TIME_U = NULL;
            }
            if(LIB_RES_U){
                mbkfree((lib_scale_unit *)LIB_RES_U);
                LIB_RES_U = NULL;
            }
            if(LIB_CAPA_U){ 
                mbkfree((lib_scale_unit *)LIB_CAPA_U);
                LIB_CAPA_U = NULL;
            }
            if(libparserpowerptype){
                freeptype(libparserpowerptype);
                libparserpowerptype = NULL;
            }
        }
        else if(ctxt == CELL_TYPE) {
            if((plofig=lib_func_treat_cell(LIB_CURRENT_GP)))
                lib_tim_treat_cell(LIB_CURRENT_GP, plofig);
            p=LIB_CURRENT_GP;
            lib_popgroup();
            lib_del_group(p);
        }
        else if(ctxt == LU_TEMPL_TYPE) {
            lib_tim_store_template();
            p=LIB_CURRENT_GP;
            lib_popgroup();
            lib_del_group(p);
        }
        else if(ctxt == WIRE_LOAD_TYPE) {
            lib_tim_store_wire_load();
            p=LIB_CURRENT_GP;
            lib_popgroup();
            lib_del_group(p);
        }
        else if(ctxt == WIRE_LOAD_TABLE_TYPE) {
            lib_tim_store_wire_load_table();
            p=LIB_CURRENT_GP;
            lib_popgroup();
            lib_del_group(p);
        }
        else if(ctxt == POWER_SUPPLY_TYPE) {
            lib_tim_treat_power_supply();
            p=LIB_CURRENT_GP;
            lib_popgroup();
            lib_del_group(p);
        }
        else
            lib_popgroup();
    }
    
    lib_pop_context();
}

/*----------------------------------------------------------------------------*/
/* LIB_TREAT_S_ATTR :                                                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_treat_s_attr(char *ident, char *attr) 
{

    if(lib_get_context() != NO_ACTION_TYPE) 
        lib_add_s_attr(ident, attr);
    else
        mbkfree((char *)attr);
    mbkfree((char *)ident);
}
    
/*----------------------------------------------------------------------------*/
/* LIB_TREAT_C_ATTR :                                                         */
/*                                                                            */
/*----------------------------------------------------------------------------*/
void    lib_treat_c_attr(char *ident, chain_list *attr) 
{
    chain_list *ch;

    if(lib_get_context() != NO_ACTION_TYPE) 
        lib_add_c_attr(ident, attr);
    else {
        for(ch=attr ; ch ; ch=ch->NEXT)
            mbkfree((char *)(ch->DATA));
        freechain(attr);
    }
    mbkfree((char *)ident);
}
   
