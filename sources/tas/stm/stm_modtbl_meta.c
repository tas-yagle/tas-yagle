/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_meta.c                                           */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                Antony Pinto                                              */
/*                                                                          */
/* Log :                                                                    */
/*       16-04-03 : added conversion from fct (..anto..)                    */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
/*{{{                    stm_mod_destroy_model()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_mod_destroy_model(timing_model *stm)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         stm_modfct_destroy(stm->UMODEL.FUNCTION);
         break;
    case STM_MOD_MODSCM :
         stm_modscm_destroy(stm->UMODEL.SCM);
         break;
    case STM_MOD_MODPLN :
         stm_modpln_destroy(stm->UMODEL.POLYNOM);
         break;
    case STM_MOD_MODTBL : 
         stm_modtbl_destroy(stm->UMODEL.TABLE);
         break;
    default :
         break;
  }
}

/*}}}************************************************************************/

/*{{{                    stm_modtbl_morph2_delay_fstm()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_delay_fstm(timing_model *stm, double *s_axis, int ns,
                                  double *l_axis, int nl, double slew,
                                  double load, double ci0)
{
  if (stm)
  {
    if (ns && nl)
      stm_modtbl_morph2_delay2D_fstm(stm,s_axis,ns,l_axis,nl,ci0);
    else if (!ns && nl)
      stm_modtbl_morph2_delay1Dslewfix_fstm(stm,l_axis,nl,slew,ci0);
    else if (ns && !nl)
      stm_modtbl_morph2_delay1Dloadfix_fstm(stm,s_axis,ns,load + ci0);
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_morph2_slew_fstm()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_slew_fstm(timing_model *stm, double *s_axis, int ns,
                                 double *l_axis, int nl, double slew,
                                 double load, double ci0)
{
  if (stm)
  {
    if (ns && nl)
      stm_modtbl_morph2_slew2D_fstm(stm,s_axis,ns,l_axis,nl,ci0);
    else if (!ns && nl)
      stm_modtbl_morph2_slew1Dslewfix_fstm(stm,l_axis,nl,slew,ci0);
    else if (ns && !nl)
      stm_modtbl_morph2_slew1Dloadfix_fstm(stm,s_axis,ns,load + ci0);
  }
}

/*}}}************************************************************************/

/*{{{                    stm_modtbl_morph2_delay2D_fstm()                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_delay2D_fstm(timing_model *stm,
                                    double *s_axis, int ns,
                                    double *l_axis, int nl,
                                    double ci0)
{
  timing_table  *tbl;

  tbl   = stm_modtbl_create_delay2D_fstm(stm,s_axis,ns,l_axis,nl,ci0);
  if (tbl)
  {
    stm_mod_destroy_model(stm);
    stm->UTYPE          = STM_MOD_MODTBL;
    stm->UMODEL.TABLE   = tbl;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_morph2_slew2D_fstm()                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_slew2D_fstm(timing_model *stm,
                                   double *s_axis, int ns, 
                                   double *l_axis, int nl,
                                   double ci0)
{
  timing_table  *tbl;

  tbl   = stm_modtbl_create_slew2D_fstm(stm,s_axis,ns,l_axis,nl,ci0);
  if (tbl)
  {
    stm_mod_destroy_model(stm);
    stm->UTYPE          = STM_MOD_MODTBL;
    stm->UMODEL.TABLE   = tbl;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_morph2_delay1Dslewfix_fstm()            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_delay1Dslewfix_fstm(timing_model *stm,
                                           double *l_axis, int nl,
                                           double slew, double ci0)
{
  timing_table  *tbl;

  tbl   = stm_modtbl_create_delay1Dslewfix_fstm(stm,l_axis,nl,slew,ci0);
  if (tbl)
  {
    stm_mod_destroy_model(stm);
    stm->UTYPE          = STM_MOD_MODTBL;
    stm->UMODEL.TABLE   = tbl;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_morph2_delay1Dloadfix_fstm()            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_delay1Dloadfix_fstm(timing_model *stm,
                                           double *s_axis, int ns,
                                           double load)
{
  timing_table  *tbl;

  tbl   = stm_modtbl_create_delay1Dloadfix_fstm(stm,s_axis,ns,load);
  if (tbl)
  {
    stm_mod_destroy_model(stm);
    stm->UTYPE          = STM_MOD_MODTBL;
    stm->UMODEL.TABLE   = tbl;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_morph2_slew1Dslewfix_fstm()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_slew1Dslewfix_fstm(timing_model *stm,
                                          double *l_axis, int nl,
                                          double slew, double ci0)
{
  timing_table  *tbl;

  tbl   = stm_modtbl_create_slew1Dslewfix_fstm(stm,l_axis,nl,slew,ci0);
  if (tbl)
  {
    stm_mod_destroy_model(stm);
    stm->UTYPE          = STM_MOD_MODTBL;
    stm->UMODEL.TABLE   = tbl;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_morph2_slew1Dloadfix_ffct()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_modtbl_morph2_slew1Dloadfix_fstm(timing_model *stm,
                                          double *s_axis, int ns,
                                          double load)
{
  timing_table  *tbl;

  tbl   = stm_modtbl_create_slew1Dloadfix_fstm(stm,s_axis,ns,load);
  if (tbl)
  {
    stm_mod_destroy_model(stm);
    stm->UTYPE          = STM_MOD_MODTBL;
    stm->UMODEL.TABLE   = tbl;
  }
}

/*}}}************************************************************************/

/*{{{                    stm_modtbl_create_delay2D_fstm()                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_delay2D_fstm(timing_model *stm,
                                             double *s_axis, int ns,
                                             double *l_axis, int nl,
                                             double ci0)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         return stm_modtbl_create_delay2D_ffct(stm->UMODEL.FUNCTION,
                                               s_axis,ns,
                                               l_axis,nl,ci0);
    case STM_MOD_MODSCM :
         return stm_modtbl_create_delay2D_fscm(stm->UMODEL.SCM,
                                               s_axis,ns,
                                               l_axis,nl,ci0);
    case STM_MOD_MODPLN :
         return stm_modtbl_create_delay2D_fpln(stm->UMODEL.POLYNOM,
                                               s_axis,ns,
                                               l_axis,nl,ci0);
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_slew2D_fstm()                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_slew2D_fstm(timing_model *stm,
                                            double *s_axis, int ns, 
                                            double *l_axis, int nl,
                                            double ci0)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         return stm_modtbl_create_slew2D_ffct(stm->UMODEL.FUNCTION,
                                               s_axis,ns,
                                               l_axis,nl,ci0);
    case STM_MOD_MODSCM :
         return stm_modtbl_create_slew2D_fscm(stm->UMODEL.SCM,
                                               s_axis,ns,
                                               l_axis,nl,ci0);
    case STM_MOD_MODPLN :
         return stm_modtbl_create_slew2D_fpln(stm->UMODEL.POLYNOM,
                                               s_axis,ns,
                                               l_axis,nl,ci0);
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_delay1Dslewfix_fstm()            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_delay1Dslewfix_fstm(timing_model *stm,
                                                    double *l_axis, int nl,
                                                    double slew, double ci0)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         return stm_modtbl_create_delay1Dslewfix_ffct(stm->UMODEL.FUNCTION,
                                                     l_axis,nl,slew,ci0);
    case STM_MOD_MODSCM :
         return stm_modtbl_create_delay1Dslewfix_fscm(stm->UMODEL.SCM,
                                                     l_axis,nl,slew,ci0);
    case STM_MOD_MODPLN :
         return stm_modtbl_create_delay1Dslewfix_fpln(stm->UMODEL.POLYNOM,
                                                     l_axis,nl,slew,ci0);
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_delay1Dloadfix_fstm()            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_delay1Dloadfix_fstm(timing_model *stm,
                                                    double *s_axis, int ns,
                                                    double load)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         return stm_modtbl_create_delay1Dloadfix_ffct(stm->UMODEL.FUNCTION,
                                                      s_axis,ns,load);
    case STM_MOD_MODSCM :
         return stm_modtbl_create_delay1Dloadfix_fscm(stm->UMODEL.SCM,
                                                      s_axis,ns,load);
    case STM_MOD_MODPLN :
         return stm_modtbl_create_delay1Dloadfix_fpln(stm->UMODEL.POLYNOM,
                                                      s_axis,ns,load);
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_slew1Dslewfix_fstm()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_slew1Dslewfix_fstm(timing_model *stm,
                                                   double *l_axis, int nl,
                                                   double slew, double ci0)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         return stm_modtbl_create_slew1Dslewfix_ffct(stm->UMODEL.FUNCTION,
                                                     l_axis,nl,slew,ci0);
    case STM_MOD_MODSCM :
         return stm_modtbl_create_slew1Dslewfix_fscm(stm->UMODEL.SCM,
                                                     l_axis,nl,slew,ci0);
    case STM_MOD_MODPLN :
         return stm_modtbl_create_slew1Dslewfix_fpln(stm->UMODEL.POLYNOM,
                                                     l_axis,nl,slew,ci0);
    default :
         return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_slew1Dloadfix_ffct()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_slew1Dloadfix_fstm(timing_model *stm,
                                                   double *s_axis, int ns,
                                                   double load)
{
  switch (stm->UTYPE)
  {
    case STM_MOD_MODFCT :
         return stm_modtbl_create_slew1Dloadfix_ffct(stm->UMODEL.FUNCTION,
                                                     s_axis,ns,load);
    case STM_MOD_MODSCM :
         return stm_modtbl_create_slew1Dloadfix_fscm(stm->UMODEL.SCM,
                                                     s_axis,ns,load);
    case STM_MOD_MODPLN :
         return stm_modtbl_create_slew1Dloadfix_fpln(stm->UMODEL.POLYNOM,
                                                     s_axis,ns,load);
    default :
         return NULL;
  }
}

/*}}}************************************************************************/

/*{{{                    stm_modtbl_create_delay2D_ffct()                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_delay2D_ffct(timing_function *fct,
                                             double *s_axis, int ns,
                                             double *l_axis, int nl,
                                             double ci0)
{
  timing_table  *tbl;
  int            x, y;
  double         delay;

  tbl = stm_modtbl_create(ns, nl, STM_INPUT_SLEW, STM_LOAD);

  /* axis */
  if (nl)
    for (x = 0; x < ns; x++)
      stm_modtbl_setXrangeval(tbl, x, s_axis[x]);
  if (ns)
    for (y = 0; y < nl; y++)
      stm_modtbl_setYrangeval(tbl, y, l_axis[y]);

  /* values */
  if (nl && ns)
    for (x = 0; x < ns; x++)
      for (y = 0; y < nl; y++)
      {
        delay   = stm_modfct_delay(fct, s_axis[x], l_axis[y] + ci0);
        stm_modtbl_set2Dval(tbl, x, y, delay);
      }
  
  return tbl;
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_slew2D_ffct()                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_slew2D_ffct(timing_function *fct,
                                            double *s_axis, int ns, 
                                            double *l_axis, int nl,
                                            double ci0)
{
  timing_table  *tbl;
  int            x, y;
  double         slope;

  tbl = stm_modtbl_create(ns, nl, STM_INPUT_SLEW, STM_LOAD);

  /* axis */
  if (ns)
    for (x = 0; x < ns; x++)
      stm_modtbl_setXrangeval(tbl, x, s_axis[x]);
  if (nl)
    for (y = 0; y < nl; y++)
      stm_modtbl_setYrangeval(tbl, y, l_axis[y]);

  /* values */
  if (nl && ns)
    for (x = 0; x < ns; x++)
      for (y = 0; y < nl; y++)
      {
        slope   = stm_modfct_slew(fct, s_axis[x], l_axis[y] + ci0);
        stm_modtbl_set2Dval(tbl, x, y, slope);
      }

  return tbl;
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_delay1Dslewfix_ffct()            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_delay1Dslewfix_ffct(timing_function *fct,
                                                    double *l_axis, int nl,
                                                    double slew, double ci0)
{
  timing_table  *tbl;
  int            x;
  double         delay;

  tbl = stm_modtbl_create(nl, 0, STM_LOAD, STM_NOTYPE);

  /* axis */
  if (nl)
    for (x = 0; x < nl; x++)
      stm_modtbl_setXrangeval(tbl, x, l_axis[x]);

  /* values */
  if (nl)
    for (x = 0; x < nl; x++)
    {
      delay = stm_modfct_delay(fct, slew, l_axis[x] + ci0);
      stm_modtbl_set1Dval(tbl, x, delay);
    }

  return tbl;
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_delay1Dloadfix_ffct()            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_delay1Dloadfix_ffct(timing_function *fct,
                                                    double *s_axis, int ns,
                                                    double load)
{
  timing_table  *tbl;
  int            x;
  double         delay;

  tbl = stm_modtbl_create(ns, 0, STM_INPUT_SLEW, STM_NOTYPE);

  /* axis */
  if (ns)
    for (x = 0; x < ns; x++)
      stm_modtbl_setXrangeval(tbl, x, s_axis[x]);

  /* values */
  if (ns)
    for (x = 0; x < ns; x++)
    {
      delay = stm_modfct_delay(fct, s_axis[x], load);
      stm_modtbl_set1Dval(tbl, x, delay);
    }

  return tbl;
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_slew1Dslewfix_ffct()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_slew1Dslewfix_ffct(timing_function *fct,
                                                   double *l_axis, int nl,
                                                   double slew, double ci0)
{
  timing_table  *tbl;
  int            x;
  double         slope;

  tbl = stm_modtbl_create(nl, 0, STM_LOAD, STM_NOTYPE);

  /* axis */
  if (nl)
    for (x = 0; x < nl; x++)
      stm_modtbl_setXrangeval(tbl, x, l_axis[x]);

  /* values */
  if (nl)
    for (x = 0; x < nl; x++)
    {
      slope = stm_modfct_slew(fct, slew, l_axis[x] + ci0);
      stm_modtbl_set1Dval(tbl, x, slope);
    }

  return tbl;
}

/*}}}************************************************************************/
/*{{{                    stm_modtbl_create_slew1Dloadfix_ffct()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_table *stm_modtbl_create_slew1Dloadfix_ffct(timing_function *fct,
                                                   double *s_axis, int ns,
                                                   double load)
{
  timing_table  *tbl;
  int            x;
  double         slope;

  tbl = stm_modtbl_create(ns, 0, STM_INPUT_SLEW, STM_NOTYPE);

  /* axis */
  if (ns)
    for (x = 0; x < ns; x ++)
      stm_modtbl_setXrangeval(tbl, x, s_axis[x]);

  /* values */
  if (ns)
    for (x = 0; x < ns; x ++)
    {
      slope = stm_modfct_slew(fct, s_axis[x], load);
      stm_modtbl_set1Dval(tbl, x, slope);
    }

  return tbl;
}

/*}}}************************************************************************/

timing_table *stm_modtbl_create_delay2D_fscm (timing_scm *scm, double *s_axis, int ns, double *l_axis, int nl, double ci0)
{
    int x, y;
    timing_table *tbl;

    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_delay (scm, STM_NOVALUE, NULL, STM_NOVALUE));
        return tbl;
    }
        
    tbl = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);

    /* axis */
    if (nl)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);
    if (ns)
        for (y = 0; y < nl; y++)
            stm_modtbl_setYrangeval (tbl, y, l_axis[y]);

    /* values */
    if (nl && ns)
        for (x = 0; x < ns; x++)
            for (y = 0; y < nl; y++)
                stm_modtbl_set2Dval (tbl, x, y, stm_modscm_delay (scm, s_axis[x], NULL, l_axis[y] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_slew2D_fscm (timing_scm *scm, double *s_axis, int ns, double *l_axis, int nl, double ci0)
{
    int x, y;
    timing_table *tbl;

    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_slew (scm, STM_NOVALUE, NULL, NULL, STM_NOVALUE));
        return tbl;
    }
    
    tbl = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);
    
    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);
    if (nl)
        for (y = 0; y < nl; y++)
            stm_modtbl_setYrangeval (tbl, y, l_axis[y]);

    /* values */
    if (nl && ns)
        for (x = 0; x < ns; x++)
            for (y = 0; y < nl; y++)
                stm_modtbl_set2Dval (tbl, x, y, stm_modscm_slew (scm, s_axis[x], NULL, NULL, l_axis[y] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_delay1Dslewfix_fscm (timing_scm *scm, double *l_axis, int nl, double slew, double ci0)
{
    int x;
    timing_table *tbl;

    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_delay (scm, STM_NOVALUE, NULL, STM_NOVALUE));
        return tbl;
    }
    
    tbl = stm_modtbl_create (nl, 0, STM_LOAD, STM_NOTYPE);

    /* axis */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_setXrangeval (tbl, x, l_axis[x]);

    /* values */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modscm_delay (scm, slew, NULL, l_axis[x] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_delay1Dloadfix_fscm (timing_scm *scm, double *s_axis, int ns, double load)
{
    int x;
    timing_table *tbl;
    
    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_delay (scm, STM_NOVALUE, NULL, STM_NOVALUE));
        return tbl;
    }
    
   tbl = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);

    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);

    /* values */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modscm_delay (scm, s_axis[x], NULL, load));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_slew1Dslewfix_fscm (timing_scm *scm, double *l_axis, int nl, double slew, double ci0)
{
    int x;
    timing_table *tbl;

    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_slew (scm, STM_NOVALUE, NULL, NULL, STM_NOVALUE));
        return tbl;
    }
    
    tbl = stm_modtbl_create (nl, 0, STM_LOAD, STM_NOTYPE);

    /* axis */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_setXrangeval (tbl, x, l_axis[x]);

    /* values */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modscm_slew (scm, slew, NULL, NULL, l_axis[x] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_slew1Dloadfix_fscm (timing_scm *scm, double *s_axis, int ns, double load)
{
    int x;
    timing_table *tbl;

    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_slew (scm, STM_NOVALUE, NULL, NULL, STM_NOVALUE));
        return tbl;
    }
    
    tbl = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);
    
    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);

    /* values */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modscm_slew (scm, s_axis[x], NULL, NULL, load));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_delay2D_fpln (timing_polynom *pln, double *s_axis, int ns, double *l_axis, int nl, double ci0)
{
    int x, y;
    timing_table *tbl;

/*    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_delay (scm, STM_NOVALUE, STM_NOVALUE));
        return tbl;
    }*/
        
    tbl = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);

    /* axis */
    if (nl)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);
    if (ns)
        for (y = 0; y < nl; y++)
            stm_modtbl_setYrangeval (tbl, y, l_axis[y]);

    /* values */
    if (nl && ns)
        for (x = 0; x < ns; x++)
            for (y = 0; y < nl; y++)
                stm_modtbl_set2Dval (tbl, x, y, stm_modpln_delay (pln, s_axis[x], l_axis[y] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_slew2D_fpln (timing_polynom *pln, double *s_axis, int ns, double *l_axis, int nl, double ci0)
{
    int x, y;
    timing_table *tbl;

/*    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_slew (scm, STM_NOVALUE, STM_NOVALUE));
        return tbl;
    }*/
    
    tbl = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);
    
    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);
    if (nl)
        for (y = 0; y < nl; y++)
            stm_modtbl_setYrangeval (tbl, y, l_axis[y]);

    /* values */
    if (nl && ns)
        for (x = 0; x < ns; x++)
            for (y = 0; y < nl; y++)
                stm_modtbl_set2Dval (tbl, x, y, stm_modpln_slew (pln, s_axis[x], l_axis[y] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_delay1Dslewfix_fpln (timing_polynom *pln, double *l_axis, int nl, double slew, double ci0)
{
    int x;
    timing_table *tbl;

/*    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_delay (scm, STM_NOVALUE, STM_NOVALUE));
        return tbl;
    }*/
    
    tbl = stm_modtbl_create (nl, 0, STM_LOAD, STM_NOTYPE);

    /* axis */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_setXrangeval (tbl, x, l_axis[x]);

    /* values */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modpln_delay (pln, slew, l_axis[x] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_delay1Dloadfix_fpln (timing_polynom *pln, double *s_axis, int ns, double load)
{
    int x;
    timing_table *tbl;
    
/*    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_delay (scm, STM_NOVALUE, STM_NOVALUE));
        return tbl;
    }*/
    
   tbl = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);

    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);

    /* values */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modpln_delay (pln, s_axis[x], load));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_slew1Dslewfix_fpln (timing_polynom *pln, double *l_axis, int nl, double slew, double ci0)
{
    int x;
    timing_table *tbl;

/*    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_slew (scm, STM_NOVALUE, STM_NOVALUE));
        return tbl;
    }*/
    
    tbl = stm_modtbl_create (nl, 0, STM_LOAD, STM_NOTYPE);

    /* axis */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_setXrangeval (tbl, x, l_axis[x]);

    /* values */
    if (nl)
        for (x = 0; x < nl; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modpln_slew (pln, slew, l_axis[x] + ci0));

    return tbl;
}

/****************************************************************************/

timing_table *stm_modtbl_create_slew1Dloadfix_fpln (timing_polynom *pln, double *s_axis, int ns, double load)
{
    int x;
    timing_table *tbl;

/*    if (scm->TYPE == STM_MODSCM_CST) {
        tbl = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
        stm_modtbl_setconst (tbl, stm_modscm_slew (scm, STM_NOVALUE, STM_NOVALUE));
        return tbl;
    }*/
    
    tbl = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);
    
    /* axis */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_setXrangeval (tbl, x, s_axis[x]);

    /* values */
    if (ns)
        for (x = 0; x < ns; x++)
            stm_modtbl_set1Dval (tbl, x, stm_modpln_slew (pln, s_axis[x], load));

    return tbl;
}

/****************************************************************************/

int stm_modtbl_is2D (timing_table *t)
{
    return t->SET2D && !t->SET1D && stm_modtbl_isnull (t->CST);
}

/****************************************************************************/

int stm_modtbl_is1D (timing_table *t)
{
    return !t->SET2D && t->SET1D && stm_modtbl_isnull (t->CST);
}

/****************************************************************************/

int stm_modtbl_isconst (timing_table *t)
{
    return !t->SET2D && !t->SET1D && !stm_modtbl_isnull (t->CST);
}

/****************************************************************************/

void stm_modtbl_fitonmax (timing_table **tab_A, timing_table **tab_B)
{
    int i, j;
    timing_table *newtab;

    if (stm_modtbl_is2D (*tab_A) && stm_modtbl_is1D (*tab_B)) {

        newtab = stm_modtbl_duplicate (*tab_A);
        for (i = 0; i < (*tab_A)->NX; i++)
            for (j = 0; j < (*tab_A)->NY; j++)
                if ((*tab_A)->XTYPE == (*tab_B)->XTYPE)
                    newtab->SET2D[i][j] = (*tab_B)->SET1D[i];
                else
                    newtab->SET2D[i][j] = (*tab_B)->SET1D[j];
        stm_modtbl_destroy (*tab_B);
        *tab_B = newtab;
    }
    else
    if (stm_modtbl_is2D (*tab_A) && stm_modtbl_isconst (*tab_B)) {
        newtab = stm_modtbl_duplicate (*tab_A);
        for (i = 0; i < (*tab_A)->NX; i++)
            for (j = 0; j < (*tab_A)->NY; j++)
                newtab->SET2D[i][j] = (*tab_B)->CST;
        stm_modtbl_destroy (*tab_B);
        *tab_B = newtab;
    }
    else
    if (stm_modtbl_is1D (*tab_A) && stm_modtbl_isconst (*tab_B)) {
        newtab = stm_modtbl_duplicate (*tab_A);
        for (i = 0; i < (*tab_A)->NX; i++)
            newtab->SET1D[i] = (*tab_B)->CST;
        stm_modtbl_destroy (*tab_B);
        *tab_B = newtab;
    }
    else
    if (stm_modtbl_is2D (*tab_B) && stm_modtbl_is1D (*tab_A))
        stm_modtbl_fitonmax (tab_B, tab_A);
    else
    if (stm_modtbl_is2D (*tab_B) && stm_modtbl_isconst (*tab_A))
        stm_modtbl_fitonmax (tab_B, tab_A);
    else
    if (stm_modtbl_is1D (*tab_B) && stm_modtbl_isconst (*tab_A))
        stm_modtbl_fitonmax (tab_B, tab_A);
}

