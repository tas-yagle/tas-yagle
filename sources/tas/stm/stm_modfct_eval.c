/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modfct_eval.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"
#include GEN_H

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

ExecutionContext *STM_EC = NULL;

ExecutionContext *Get_stm_C_context ()
{
    lib_entry *le;
    char path[200], lib[200], *str;
    if (STM_EC != NULL)
        return STM_EC;

    APIInit ();
    STM_EC = APINewExecutionContext ();

    str = V_STR_TAB[__GENIUS_LIB_PATH].VALUE;
    if (str == NULL)
        strcpy (lib, "cells");
    else
        strcpy (lib, str);

    sprintf (path, "%s/%s/", WORK_LIB, lib);

    str = V_STR_TAB[__GENIUS_LIB_NAME].VALUE;
    if (str == NULL)
        sprintf (lib, "%sLIBRARY", path);
    else
        sprintf (lib, "%s%s", path, str);

    le = APIReadLibrary (lib, path, NULL);
    if (ReadAllCorVHDL (STM_EC, le, NULL)) {
        printf ("stmfct could not read rule files\n");
        EXIT (3);
    }
    // call user init function
    {
      APICallFunc cf;
      t_arg *ret;
      cf.NAME="GnsInit";
      cf.ARGS=NULL;
      if (APIExecAPICallFunc(STM_EC, &cf, &ret, 1)==0)
         APIFreeTARG(ret);
    } 

    APIFreeLibrary (le);
    return STM_EC;
}

/*
   ht *ALL_FCT_FILES=NULL;

   ExecutionContext *Get_stm_C_context(char *file)
   {
   long l;
   ExecutionContext *ec;
   FILE *f;
   char *name;
   tree_list *tr;

   if (ALL_FCT_FILES==NULL) ALL_FCT_FILES=addht(10);

   name=sensitive_namealloc(file);
   if ((l=gethtitem(ALL_FCT_FILES, name))!=EMPTYHT) return (ExecutionContext *)l;

   if (!(f = mbkfopen (name, NULL, READ_TEXT))) {
   fprintf (stderr, "Cannot open file '%s'\n", name);
   return NULL;
   }

   APIInit ();
   ec = APINewExecutionContext ();
   tr = APIParseFile (f, name, ec);
   APIVerify_C_Functions (ec);
   if (APICheckCFunctions (ec)) { fclose(f); return NULL;}
   addhtitem(ALL_FCT_FILES, name, (long)ec);
   fclose(f);
   return ec;
   }

   void Free_stm_C_context()
   {
   chain_list *cl, *ch;

   cl=GetAllHTElems(ALL_FCT_FILES);

   for (ch=cl; ch!=NULL; ch=ch->NEXT)
   APIFreeExecutionContext ((ExecutionContext *)ch->DATA);

   freechain(cl);

   delht(ALL_FCT_FILES);
   ALL_FCT_FILES=NULL;
   }
 */

static double __STM_DELAY__, __STM_INPUT_SLOPE__, __STM_COMMAND_SLOPE__;
static double __STM_SLOPE__, __STM_OUTPUT_LOAD__;
static timing_function *__STM_ARC__=NULL;


timing_function *stm_get_current_arc()
{
    return __STM_ARC__;
}

void stm_set_current_arc(timing_function *arc)
{
    __STM_ARC__=arc;
}

void stm_set_computed_delay (double value)
{
    __STM_DELAY__ = value;
}

void stm_set_computed_slope (double value)
{
    __STM_SLOPE__ = value;
}

double stm_get_computed_delay ()
{
    return __STM_DELAY__;
}

double stm_get_computed_slope ()
{
    return __STM_SLOPE__;
}

double stm_get_output_load ()
{
    return __STM_OUTPUT_LOAD__;
}

double stm_get_input_slope ()
{
    return __STM_INPUT_SLOPE__;
}

double stm_get_command_slope ()
{
    return __STM_COMMAND_SLOPE__;
}

void stm_set_output_load (double val)
{
    __STM_OUTPUT_LOAD__ = val;
}

void stm_set_input_slope (double val)
{
    __STM_INPUT_SLOPE__ = val;
}

void stm_set_command_slope (double val)
{
    __STM_COMMAND_SLOPE__ = val;
}

void stm_call_simulation()
{
    timing_function *tf;
    HierLofigInfo *hli;
    t_arg *ta;
    ArcInfo *ai;
    if ((tf=stm_get_current_arc())==NULL)
    {
        avt_errmsg (STM_ERRMSG, "042", AVT_ERROR);
        return;
    }
    hli=gethierlofiginfo(CUR_HIER_LOFIG);
    ai=getarcinfo(hli, tf->LOCALNAME);
    if (ai->SIM==NULL)
    {
        avt_errmsg (STM_ERRMSG, "043", AVT_ERROR, CUR_HIER_LOFIG, tf->LOCALNAME);
        return;
    }
    if (APIExecAPICallFunc (Get_stm_C_context (), ai->SIM, &ta, 0)) EXIT (1);
    APIFreeTARG (ta);
}

void stm_call_simulation_env()
{
    timing_function *tf;
    HierLofigInfo *hli;
    t_arg *ta;
    ArcInfo *ai;
    if ((tf=stm_get_current_arc())==NULL)
    {
        avt_errmsg (STM_ERRMSG, "044", AVT_ERROR);
        return;
    }
    hli=gethierlofiginfo(CUR_HIER_LOFIG);
    ai=getarcinfo(hli, tf->LOCALNAME);
    if (ai->ENV==NULL)
    {
        avt_errmsg (STM_ERRMSG, "045", AVT_ERROR, CUR_HIER_LOFIG, tf->LOCALNAME);
        return;
    }
    if (APIExecAPICallFunc (Get_stm_C_context (), ai->ENV, &ta, 0)) EXIT (1);
    APIFreeTARG (ta);
}

void stm_call_ctk_env()
{
    timing_function *tf;
    HierLofigInfo *hli;
    t_arg *ta;
    ArcInfo *ai;
    if ((tf=stm_get_current_arc())==NULL)
    {
        avt_errmsg (STM_ERRMSG, "046", AVT_ERROR);
        return;
    }
    hli=gethierlofiginfo(CUR_HIER_LOFIG);
    ai=getarcinfo(hli, tf->LOCALNAME);
    if (ai->CTK_ENV==NULL)
    {
        avt_errmsg (STM_ERRMSG, "047", AVT_ERROR, tf->LOCALNAME);
        return;
    }
    if (APIExecAPICallFunc (Get_stm_C_context (), ai->CTK_ENV, &ta, 0)) EXIT (1);
    APIFreeTARG (ta);
}

/****************************************************************************/
/* function stm_modfct_slew()                                               */
/****************************************************************************/

float stm_modfct_slew (timing_function * fct, float slew, float load)
{
    subinst_t *sins;

    if (!fct || !fct->FCT)
        return 0.0;

    //printf ("stmfct SLEW recompute request instance = '%s' slope = %g load = %g\n", fct->INS, slew, load);
    if (LATEST_GNS_RUN==NULL)
    {
      avt_errmsg (STM_ERRMSG, "048", AVT_ERROR);
      return 1e-12;
    }
    sins = gen_get_hier_instance (LATEST_GNS_RUN, fct->INS);
    if (!sins) {
        avt_errmsg (STM_ERRMSG, "049", AVT_ERROR, fct->INS);
        return 0.0;
    }

    in_genius_context_of (LATEST_GNS_RUN, sins);
    APICallApiInitFunctions();
    stm_set_input_slope (slew * 1e-12);
    stm_set_output_load (load * 1e-15);
    stm_set_current_arc(fct);
    stm_call_func (Get_stm_C_context (), fct);
    stm_set_current_arc(NULL);
    APICallApiTerminateFunctions();
    out_genius_context_of ();
/*
    if ( stm_get_computed_slope ()>10000e-12)
      {
        printf("big slope returned : %g\n",stm_get_computed_slope());
        EXIT(89);
      }
*/
    return stm_get_computed_slope () * 1e12;
}

/****************************************************************************/
/* function stm_call_func ()                                                */
/****************************************************************************/

void stm_call_func (ExecutionContext * ec, timing_function * fct)
{
    t_arg *ta;
    if (APIExecAPICallFunc (ec, fct->FCT, &ta, 0))
        EXIT (1);
    APIFreeTARG (ta);
}

/****************************************************************************/
/* function stm_modfct_delay ()                                             */
/****************************************************************************/

float stm_modfct_delay (timing_function * fct, float slew, float load)
{
    subinst_t *sins;

    if (!fct || !fct->FCT)
        return 0.0;

    //printf ("stmfct DELAY recompute request instance = '%s' slope = %g load = %g\n", fct->INS, slew, load);
    if (LATEST_GNS_RUN==NULL)
    {
      avt_errmsg (STM_ERRMSG, "048", AVT_ERROR);
      return 0;
    }
    sins = gen_get_hier_instance (LATEST_GNS_RUN, fct->INS);
    if (!sins) {
        avt_errmsg (STM_ERRMSG, "049", AVT_ERROR, fct->INS);
        return 0.0;
    }

    in_genius_context_of (LATEST_GNS_RUN, sins);
    APICallApiInitFunctions();
    stm_set_input_slope (slew * 1e-12);
    stm_set_output_load (load * 1e-15);
    stm_set_current_arc(fct);
    stm_call_func (Get_stm_C_context (), fct);
    stm_set_current_arc(NULL); // pour etre sur de voir une erreur hors context
    APICallApiTerminateFunctions();
    out_genius_context_of ();
/*
    if ( stm_get_computed_delay ()>10000e-12) 
      {
        printf("big delay returned : %g\n",stm_get_computed_delay());
        EXIT(88);
      }
*/
    return stm_get_computed_delay () * 1e12;
}

/****************************************************************************/
/* function stm_modfct_constraint()                                         */
/****************************************************************************/

float stm_modfct_constraint (timing_function * fct, float inslew, float ckslew)
{
    subinst_t *sins;

    if (!fct || !fct->FCT)
        return 0.0;

//    printf ("stmfct CONSTRAINT recompute request instance = '%s' inslope = %g comslope = %g\n", fct->INS, inslew, ckslew);
    if (LATEST_GNS_RUN==NULL)
    {
      avt_error("stm", 1, AVT_ERR, "Gns informations not loaded, can't compute constraint\n");
      return 0;
    }
    sins = gen_get_hier_instance (LATEST_GNS_RUN, fct->INS);
    if (!sins) {
        avt_errmsg (STM_ERRMSG, "049", AVT_ERROR, fct->INS);
        return 0.0;
    }

    in_genius_context_of (LATEST_GNS_RUN, sins);
    APICallApiInitFunctions();
    stm_set_input_slope (inslew * 1e-12);
    stm_set_command_slope (ckslew * 1e-15);
    stm_set_current_arc(fct);
    stm_call_func (Get_stm_C_context (), fct);
    stm_set_current_arc(NULL); // pour etre sur de voir une erreur hors context
    APICallApiTerminateFunctions();
    out_genius_context_of ();

    return stm_get_computed_delay () * 1e12;
}
