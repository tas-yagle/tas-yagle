/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtbl_modif.c                                          */
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

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void stm_modtbl_scale_and_add_val (timing_table *table, float val, float factor)
{
    int i, j;

    if (!table)
        return;
    
    if(table->NY && table->NX){
        for (j = 0; j < table->NY; j++)
            for (i = 0; i < table->NX; i++)
                table->SET2D[i][j] = table->SET2D[i][j]*factor+val;
    }else if(table->NX){
        for (i = 0; i < table->NX; i++)
            table->SET1D[i] = table->SET1D[i]*factor+val;
    }else{
        table->CST = table->CST*factor+val;
    }
}

void stm_modtbl_addtab (timing_table *table, float *valtab, char   type)
{
    int nld, nsd, i, j;

    if (!table)
        return;
    
    nld = stm_modtbl_getnload (table);
    nsd = stm_modtbl_getnslew (table);

    if (type == STM_LOAD) {
        if (nsd && nld) {
            if (table->XTYPE == type)
                for (j = 0; j < table->NY; j++)
                    for (i = 0; i < table->NX; i++)
                        table->SET2D[i][j] += valtab[i];
            else
            if (table->YTYPE == type)
                for (i = 0; i < table->NX; i++)
                    for (j = 0; j < table->NY; j++)
                        table->SET2D[i][j] += valtab[j];
        }
        else
        if (!nsd && nld) {
            if (table->XTYPE == STM_LOAD)
                for (i = 0; i < table->NX; i++)
                    table->SET1D[i] += valtab[i];
            else
            if (table->YTYPE == STM_LOAD)
                for (i = 0; i < table->NY; i++)
                    table->SET1D[i] += valtab[i];
        }
    }
    else
    if (type == STM_INPUT_SLEW || type == STM_CLOCK_SLEW) {
        if (nsd && nld) {
            if (table->XTYPE == type)
                for (j = 0; j < table->NY; j++)
                    for (i = 0; i < table->NX; i++)
                        table->SET2D[i][j] += valtab[i];
            else
            if (table->YTYPE == type)
                for (i = 0; i < table->NX; i++)
                    for (j = 0; j < table->NY; j++)
                        table->SET2D[i][j] += valtab[j];
        }
        else
        if (nsd && !nld) {
            if (table->XTYPE == type)
                for (i = 0; i < table->NX; i++)
                    table->SET1D[i] += valtab[i];
            else
            if (table->YTYPE == type)
                for (i = 0; i < table->NY; i++)
                    table->SET1D[i] += valtab[i];
        }
    }
}

/****************************************************************************/

timing_table *stm_modtbl_Creduce (timing_table *table, float slew, float ckslew, char redmode)
{
    timing_table *rcstrtab = NULL;
    int           nsd,           /* nb input slew delaytable */ 
                  ncsd;          /* nb clock slew delaytable */ 
    int           i;
    float         inslew, constraint;

    if (!table)
        return NULL;
    
    nsd  = stm_modtbl_getnslew (table);
    ncsd = stm_modtbl_getnckslew (table);

    if (redmode == STM_COPY) 
        rcstrtab = stm_modtbl_duplicate (table);

    if (redmode == STM_CLOCKSLEWFIX_RED) { 
        if (nsd && ncsd) {
            rcstrtab = stm_modtbl_create (nsd, 0, STM_INPUT_SLEW, STM_NOTYPE);
            stm_modtbl_Ximportslewaxis (rcstrtab, table);
            for (i = 0; i < nsd; i++) {
                inslew = stm_modtbl_getslewaxisval (table, i);
                constraint = stm_modtbl_constraint (table, inslew, ckslew);
                stm_modtbl_set1Dval (rcstrtab, i, constraint);
            }
        }
        if (nsd && !ncsd) {
            rcstrtab = stm_modtbl_create (nsd, 0, STM_INPUT_SLEW, STM_NOTYPE);
            stm_modtbl_Ximportslewaxis (rcstrtab, table);
            for (i = 0; i < nsd; i++) {
                inslew = stm_modtbl_getslewaxisval (table, i);
                constraint = stm_modtbl_constraint (table, inslew, STM_DONTCARE);
                stm_modtbl_set1Dval (rcstrtab, i, constraint);
            }
        }
        if (!nsd && ncsd) {
            rcstrtab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            constraint = stm_modtbl_constraint (table, STM_DONTCARE, ckslew);
            stm_modtbl_setconst (rcstrtab, constraint);
        }
        if (!nsd && !ncsd) {
            rcstrtab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            constraint = stm_modtbl_constraint (table, STM_DONTCARE, STM_DONTCARE);
            stm_modtbl_setconst (rcstrtab, constraint);
        }
    }
    
    if (redmode == STM_SLEWFIX_RED) {
        if (nsd && ncsd) {
            rcstrtab = stm_modtbl_create (ncsd, 0, STM_CLOCK_SLEW, STM_NOTYPE);
            stm_modtbl_Ximportckslewaxis (rcstrtab, table);
            for (i = 0; i < ncsd; i++) {
                ckslew = stm_modtbl_getckslewaxisval (table, i);
                constraint = stm_modtbl_constraint (table, slew, ckslew);
                stm_modtbl_set1Dval (rcstrtab, i, constraint);
            }
        }
        if (nsd && !ncsd) {
            rcstrtab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            constraint = stm_modtbl_constraint (table, slew, STM_DONTCARE);
            stm_modtbl_setconst (rcstrtab, constraint);
        }
        if (!nsd && ncsd) {
            rcstrtab = stm_modtbl_create (ncsd, 0, STM_CLOCK_SLEW, STM_NOTYPE);
            stm_modtbl_Ximportckslewaxis (rcstrtab, table);
            for (i = 0; i < ncsd; i++) {
                ckslew = stm_modtbl_getckslewaxisval (table, i);
                constraint = stm_modtbl_constraint (table, STM_DONTCARE, ckslew);
                stm_modtbl_set1Dval (rcstrtab, i, constraint);
            }
        }
        if (!nsd && !ncsd) {
            rcstrtab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            constraint = stm_modtbl_constraint (table, STM_DONTCARE, STM_DONTCARE);
            stm_modtbl_setconst (rcstrtab, constraint);
        }
    }

    return rcstrtab;
}

/****************************************************************************/

timing_table *stm_modtbl_reduce (timing_table *table, float slew, float load, char redmode)
{
    timing_table *rdelaytab = NULL;
    int           nsd,           /* nb load delaytable       */ 
                  nld;          /* nb input slew delaytable */ 
    int           i;
    float         inslew, outload, delay;

    if (!table)
        return NULL;
    
    nld  = stm_modtbl_getnload (table);
    nsd  = stm_modtbl_getnslew (table);

    if (redmode == STM_COPY) 
        rdelaytab = stm_modtbl_duplicate (table);

    
    if (redmode == STM_SLEWFIX_RED) {
        if (nsd && nld) {
            rdelaytab = stm_modtbl_create (nld, 0, STM_LOAD, STM_NOTYPE);
            stm_modtbl_Ximportloadaxis (rdelaytab, table);
            for (i = 0; i < nld; i++) {
                outload = stm_modtbl_getloadaxisval (table, i);
                delay = stm_modtbl_delay (table, outload, slew);
                stm_modtbl_set1Dval (rdelaytab, i, delay);
            }
        }
        if (nsd && !nld) {
            rdelaytab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            delay = stm_modtbl_delay (table, STM_DONTCARE, slew);
            stm_modtbl_setconst (rdelaytab, delay);
        }
        if (!nsd && nld) {
            rdelaytab = stm_modtbl_create (nld, 0, STM_LOAD, STM_NOTYPE);
            stm_modtbl_Ximportloadaxis (rdelaytab, table);
            for (i = 0; i < nld; i++) {
                outload = stm_modtbl_getloadaxisval (table, i);
                delay = stm_modtbl_delay (table, outload, STM_DONTCARE);
                stm_modtbl_set1Dval (rdelaytab, i, delay);
            }
        }
        if (!nsd && !nld) {
            rdelaytab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            delay = stm_modtbl_delay (table, STM_DONTCARE, STM_DONTCARE);
            stm_modtbl_setconst (rdelaytab, delay);
        }
    }
    
    if (redmode == STM_LOADFIX_RED) {
        if (nsd && nld) {
            rdelaytab = stm_modtbl_create (nsd, 0, STM_INPUT_SLEW, STM_NOTYPE);
            stm_modtbl_Ximportslewaxis (rdelaytab, table);
            for (i = 0; i < nsd; i++) {
                inslew = stm_modtbl_getslewaxisval (table, i);
                delay = stm_modtbl_delay (table, load, inslew);
                stm_modtbl_set1Dval (rdelaytab, i, delay);
            }
        }
        if (nsd && !nld) {
            rdelaytab = stm_modtbl_create (nsd, 0, STM_INPUT_SLEW, STM_NOTYPE);
            stm_modtbl_Ximportslewaxis (rdelaytab, table);
            for (i = 0; i < nsd; i++) {
                inslew = stm_modtbl_getslewaxisval (table, i);
                delay = stm_modtbl_delay (table, STM_DONTCARE, inslew);
                stm_modtbl_set1Dval (rdelaytab, i, delay);
            }
        }
        if (!nsd && nld) {
            rdelaytab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            delay = stm_modtbl_delay (table, load, STM_DONTCARE);
            stm_modtbl_setconst (rdelaytab, delay);
        }
        if (!nsd && !nld) {
            rdelaytab = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE);
            delay = stm_modtbl_delay (table, STM_DONTCARE, STM_DONTCARE);
            stm_modtbl_setconst (rdelaytab, delay);
        }
    }

    return rdelaytab;
}

/****************************************************************************/

timing_table *stm_modtbl_neg (timing_table *table)
{
    int i, j;
    timing_table *n_table;

    if (!table)
       return NULL;
    
    n_table = stm_modtbl_duplicate (table);

    if (table->SET2D) 
        for (i = 0; i < table->NX; i++)
            for (j = 0; j < table->NY; j++)
                n_table->SET2D[i][j] = - table->SET2D[i][j];
    
    if (table->SET1D) 
        for (i = 0; i < table->NX; i++)
            n_table->SET1D[i] = - table->SET1D[i];

    if (!stm_modtbl_isnull (table->CST)) 
        n_table->CST = - table->CST;

    return n_table;
}

/****************************************************************************/

void stm_modtbl_shift (timing_table *table, float load)
{
   int ns, nl;
   int i, j;
   float outload, inslew, val;
   timing_table *duptable;

   duptable = stm_modtbl_duplicate (table);
   
   ns = stm_modtbl_getnslew (duptable);
   nl = stm_modtbl_getnload (duptable);


   if (ns && nl) {
      for (i = 0; i < ns; i++) {
         inslew = stm_modtbl_getslewaxisval (duptable, i);
         for (j = 0; j < nl; j++) {
            outload = stm_modtbl_getloadaxisval (duptable, j);
            val = stm_modtbl_delay (duptable, outload + load, inslew);
            stm_modtbl_set2Dval (table, i, j, val);
         }
      }
   }
   else
   if (!ns && nl) {
      for (i = 0; i < nl; i++) {
         outload = stm_modtbl_getloadaxisval (duptable, i);
         val = stm_modtbl_delay (duptable, outload + load, STM_DONTCARE);
         stm_modtbl_set1Dval (table, i, val);
      }
   }
   stm_modtbl_destroy(duptable);
}

/****************************************************************************/

void stm_modtbl_shrinkslewaxis (timing_table *table, double thmin, double thmax, int type)
{
   int i;
   double vdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
   double vtn = STM_DEFAULT_VTN;
   double vtp = STM_DEFAULT_VTP;
   
   return; /* edge normalisation */
   if (table->XTYPE == STM_INPUT_SLEW || table->XTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NX; i++) {
         if (type == elpRISE) {
            table->XRANGE[i] = elpThr2Scm (table->XRANGE[i], thmin, thmax, vtn, vdd, vdd, type);
         }
         else
         if (type == elpFALL) {
            table->XRANGE[i] = elpThr2Scm (table->XRANGE[i], thmin, thmax, vtp, 0.0, vdd, type);
         }
      }
   }

   if (table->YTYPE == STM_INPUT_SLEW || table->YTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NY; i++) {
         if (type == elpRISE) {
            table->YRANGE[i] = elpThr2Scm (table->YRANGE[i], thmin, thmax, vtn, vdd, vdd, type);
         }
         else
         if (type == elpFALL) {
            table->YRANGE[i] = elpThr2Scm (table->YRANGE[i], thmin, thmax, vtp, 0.0, vdd, type);
         }
      }
   }
}

/****************************************************************************/

void stm_modtbl_shrinkslewaxis_with_rate (timing_table *table, double rate)
{
   int i;
   
   if (table->XTYPE == STM_INPUT_SLEW || table->XTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NX; i++) {
         table->XRANGE[i] = table->XRANGE[i] * rate;
      }
   }

   if (table->YTYPE == STM_INPUT_SLEW || table->YTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NY; i++) {
         table->YRANGE[i] = table->YRANGE[i] * rate;
      }
   }
}

/****************************************************************************/

void stm_modtbl_shrinkslewaxis_scm2thr (timing_table *table, double thmin, double thmax, int type, double vt, double vdd)
{
   int i;
   
   if (table->XTYPE == STM_INPUT_SLEW || table->XTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NX; i++) {
         if (type == elpRISE) {
            table->XRANGE[i] = elpScm2Thr (table->XRANGE[i], thmin, thmax, vt, vdd, vdd, type);
         }
         else
         if (type == elpFALL) {
            table->XRANGE[i] = elpScm2Thr (table->XRANGE[i], thmin, thmax, vt, 0.0, vdd, type);
         }
      }
   }

   if (table->YTYPE == STM_INPUT_SLEW || table->YTYPE == STM_CLOCK_SLEW) {
      for (i = 0; i < table->NY; i++) {
         if (type == elpRISE) {
            table->YRANGE[i] = elpScm2Thr (table->YRANGE[i], thmin, thmax, vt, vdd, vdd, type);
         }
         else
         if (type == elpFALL) {
            table->YRANGE[i] = elpScm2Thr (table->YRANGE[i], thmin, thmax, vt, 0.0, vdd, type);
         }
      }
   }
}

/****************************************************************************/

void stm_modtbl_shrinkslewdata (timing_table *table, double thmin, double thmax, int type)
{
   int i, j;
   double vdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
   double vtn = STM_DEFAULT_VTN;
   double vtp = STM_DEFAULT_VTP;

   return; /* edge normalisation */
   if (!stm_modtbl_isnull (table->CST)) {
      if (type == elpRISE) { 
         stm_modtbl_setconst (table, elpThr2Scm (table->CST, thmin, thmax, vtn, vdd, vdd, type));
      }
      else
      if (type == elpFALL) {
         stm_modtbl_setconst (table, elpThr2Scm (table->CST, thmin, thmax, vtp, 0, vdd, type));
      }
   }
   else
   if (table->SET1D) {
      for (i = 0; i < table->NX; i++) {
         if (type == elpRISE) {
            table->SET1D[i] = elpThr2Scm (table->SET1D[i], thmin, thmax, vtn, vdd, vdd, type);
         }
         else
         if (type == elpFALL) {
            table->SET1D[i] = elpThr2Scm (table->SET1D[i], thmin, thmax, vtp, 0, vdd, type);
         }
      }
   }
   else
   if (table->SET2D) {
      for (i = 0; i < table->NX; i++) {
         for (j = 0; j < table->NY; j++) {
            if (type == elpRISE) {
               table->SET2D[i][j] = elpThr2Scm (table->SET2D[i][j], thmin, thmax, vtn, vdd, vdd, type);
            }
            else
            if (type == elpFALL) {
               table->SET2D[i][j] = elpThr2Scm (table->SET2D[i][j], thmin, thmax, vtp, 0, vdd, type);
            }
         }
      }
   }
}

/****************************************************************************/

void stm_modtbl_shrinkslewdata_with_rate (timing_table *table, double rate)
{
   int i, j;

   if (!stm_modtbl_isnull (table->CST)) {
      stm_modtbl_setconst (table, table->CST * rate);
   }
   else
   if (table->SET1D) {
      for (i = 0; i < table->NX; i++) {
         table->SET1D[i] = table->SET1D[i] * rate;
      }
   }
   else
   if (table->SET2D) {
      for (i = 0; i < table->NX; i++) {
         for (j = 0; j < table->NY; j++) {
            table->SET2D[i][j] = table->SET2D[i][j] * rate;
         }
      }
   }
}

/****************************************************************************/

void stm_modtbl_shrinkslewdata_scm2thr (timing_table *table, double thmin, double thmax, int type, double vt, double vdd, double vf)
{
   int i, j;

   if (!stm_modtbl_isnull (table->CST)) {
     stm_modtbl_setconst (table, elpScm2Thr (table->CST, thmin, thmax, vt, vf, vdd, type));
   }
   else
   if (table->SET1D) {
      for (i = 0; i < table->NX; i++) {
        table->SET1D[i] = elpScm2Thr (table->SET1D[i], thmin, thmax, vt, vf, vdd, type);
      }
   }
   else
   if (table->SET2D) {
      for (i = 0; i < table->NX; i++) {
         for (j = 0; j < table->NY; j++) {
           table->SET2D[i][j] = elpScm2Thr (table->SET2D[i][j], thmin, thmax, vt, vf, vdd, type);
         }
      }
   }
}

/****************************************************************************/

void stm_modtbl_Ximportckslewaxis (timing_table *dsttable, timing_table *srctable)
{
    int i;
    if (dsttable->XRANGE) {
        if (srctable->XTYPE == STM_CLOCK_SLEW){ 
            for (i = 0; i < srctable->NX; i++)
                dsttable->XRANGE[i] = srctable->XRANGE[i];
        }
        if (srctable->YTYPE == STM_CLOCK_SLEW){
            for (i = 0; i < srctable->NY; i++)
                dsttable->XRANGE[i] = srctable->YRANGE[i];
        }
    }
}

/****************************************************************************/

void stm_modtbl_Yimportckslewaxis (timing_table *dsttable, timing_table *srctable)
{
    int i;
    if (dsttable->YRANGE) {
        if (srctable->XTYPE == STM_CLOCK_SLEW) 
            for (i = 0; i < srctable->NX; i++)
                dsttable->YRANGE[i] = srctable->XRANGE[i];
        if (srctable->YTYPE == STM_CLOCK_SLEW) 
            for (i = 0; i < srctable->NY; i++)
                dsttable->YRANGE[i] = srctable->YRANGE[i];
    }
}

/****************************************************************************/

void stm_modtbl_Ximportslewaxis (timing_table *dsttable, timing_table *srctable)
{
    int i;
    if (dsttable->XRANGE) {
        if (srctable->XTYPE == STM_INPUT_SLEW){ 
            for (i = 0; i < srctable->NX; i++)
                dsttable->XRANGE[i] = srctable->XRANGE[i];
        }
        if (srctable->YTYPE == STM_INPUT_SLEW){
            for (i = 0; i < srctable->NY; i++)
                dsttable->XRANGE[i] = srctable->YRANGE[i];
        }
    }
}

/****************************************************************************/

void stm_modtbl_Yimportslewaxis (timing_table *dsttable, timing_table *srctable)
{
    int i;
    if (dsttable->YRANGE) {
        if (srctable->XTYPE == STM_INPUT_SLEW){ 
            for (i = 0; i < srctable->NX; i++)
                dsttable->YRANGE[i] = srctable->XRANGE[i];
        }
        if (srctable->YTYPE == STM_INPUT_SLEW){
            for (i = 0; i < srctable->NY; i++)
                dsttable->YRANGE[i] = srctable->YRANGE[i];
        }
    }
}

/****************************************************************************/

void stm_modtbl_Ximportloadaxis (timing_table *dsttable, timing_table *srctable)
{
    int i;
    if (dsttable->XRANGE) {
        if (srctable->XTYPE == STM_LOAD) 
            for (i = 0; i < srctable->NX; i++)
                dsttable->XRANGE[i] = srctable->XRANGE[i];
        if (srctable->YTYPE == STM_LOAD) 
            for (i = 0; i < srctable->NY; i++)
                dsttable->XRANGE[i] = srctable->YRANGE[i];
    }
}

/****************************************************************************/

void stm_modtbl_Yimportloadaxis (timing_table *dsttable, timing_table *srctable)
{
    int i;
    if (dsttable->YRANGE) {
        if (srctable->XTYPE == STM_LOAD) 
            for (i = 0; i < srctable->NX; i++)
                dsttable->YRANGE[i] = srctable->XRANGE[i];
        if (srctable->YTYPE == STM_LOAD) 
            for (i = 0; i < srctable->NY; i++)
                dsttable->YRANGE[i] = srctable->YRANGE[i];
    }
}
