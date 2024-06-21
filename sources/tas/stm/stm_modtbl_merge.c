/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modtb_merge.c                                           */
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

timing_table *stm_modtbl_mergec (timing_table *dttable, timing_table *cktable, float cstr, float dtload, float ckload)
{
    timing_table *delaytabAB = NULL;
    timing_table *delaytabA;
    timing_table *delaytabB;
    int           nldA,           /* nb load delaytable A */ 
                  nsdA,           /* nb slew delaytable A */ 
                  nsdB,           /* nb load delaytable B */ 
                  nldB;           /* nb slew delaytable B */ 
    int           i, j;
    float         inslewA, inslewB;
    float         delayA, delayB;
    float         loadA = dtload;
    float         loadB = ckload;
    
    delaytabA = dttable; 
    delaytabB = cktable; 

    nldA = stm_modtbl_getnload (delaytabA);
    nsdA = stm_modtbl_getnslew (delaytabA);
    nldB = stm_modtbl_getnload (delaytabB);
    nsdB = stm_modtbl_getnslew (delaytabB);

    if (nsdA && nsdB) {
        delaytabAB = stm_modtbl_create (nsdA, nsdB, STM_INPUT_SLEW, STM_CLOCK_SLEW);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        stm_modtbl_Yimportslewaxis (delaytabAB, delaytabB);
        for (i = 0; i < nsdA; i++) {
            inslewA = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, loadA, inslewA);
            for (j = 0; j < nsdB; j++) {
                inslewB = stm_modtbl_getslewaxisval (delaytabB, j);
                delayB = stm_modtbl_delay (delaytabB, loadB, inslewB);
                stm_modtbl_set2Dval (delaytabAB, i, j, delayA + cstr + delayB);
            }
        }
    }

    if (nsdA && !nsdB) {
        delaytabAB = stm_modtbl_create (nsdA, 0, STM_INPUT_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        delayB = stm_modtbl_delay (delaytabB, loadB, STM_DONTCARE);
        for (i = 0; i < nsdA; i++) {
            inslewA = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, loadA, inslewA);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + cstr + delayB);
        }
    }

    if (!nsdA && nsdB) {
        delaytabAB = stm_modtbl_create (nsdB, 0, STM_CLOCK_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportloadaxis (delaytabAB, delaytabB);
        delayA = stm_modtbl_delay (delaytabA, loadA, STM_DONTCARE);
        for (i = 0; i < nsdB; i++) {
            inslewB = stm_modtbl_getslewaxisval (delaytabB, i);
            delayB  = stm_modtbl_delay (delaytabB, loadB, inslewB);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + cstr + delayB);
        }
    }

    if (!nsdA && !nsdB) {
        delaytabAB = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE) ;
        delayA = stm_modtbl_delay (delaytabA, loadA, STM_DONTCARE) ;
        delayB = stm_modtbl_delay (delaytabB, loadB, STM_DONTCARE) ;
        stm_modtbl_setconst (delaytabAB, delayA + cstr + delayB) ;
    }

    if (!delaytabAB) 
        avt_errmsg (STM_ERRMSG, "029", AVT_ERROR);

    return delaytabAB;
}

/****************************************************************************/
timing_table *stm_modtbl_mergecd (timing_table *dttable, timing_table *dstable, timing_table *cstrtable, float cstr, float dtload)
{
    timing_table *delaytabAB = NULL;
    timing_table *delaytabA;
    timing_table *slewtabA;
    timing_table *delaytabB;
    int           nldA,           /* nb load delaytable A */ 
                  nsdA,           /* nb slew delaytable A */ 
                  nsdB,           /* nb load delaytable B */ 
                  nldB;           /* nb slew delaytable B */ 
    int           i, j;
    float         inslewA, ckslewB, slewA;
    float         delayA, delayB;
    float         loadA = dtload;
    
    delaytabA = dttable; 
    slewtabA = dstable; 
    delaytabB = cstrtable; 

    nldA = stm_modtbl_getnload (delaytabA);
    nsdA = stm_modtbl_getnslew (delaytabA);
    nldB = stm_modtbl_getnload (delaytabB);
    nsdB = stm_modtbl_getnckslew (delaytabB);

    if (cstrtable && cstr) {
        avt_errmsg (STM_ERRMSG, "041", AVT_ERROR);
        return NULL;
    }

    if (nsdA && nsdB) {
        delaytabAB = stm_modtbl_create (nsdA, nsdB, STM_INPUT_SLEW, STM_CLOCK_SLEW);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        stm_modtbl_Yimportckslewaxis (delaytabAB, delaytabB);
        for (i = 0; i < nsdA; i++) {
            inslewA = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, loadA, inslewA);
            slewA = stm_modtbl_slew (slewtabA, loadA, inslewA);
            for (j = 0; j < nsdB; j++) {
                ckslewB = stm_modtbl_getckslewaxisval (delaytabB, j);
                delayB = stm_modtbl_constraint (delaytabB, slewA, ckslewB);
                stm_modtbl_set2Dval (delaytabAB, i, j, delayA + cstr + delayB);
            }
        }
    }

    if (nsdA && !nsdB) {
        delaytabAB = stm_modtbl_create (nsdA, 0, STM_INPUT_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        for (i = 0; i < nsdA; i++) {
            inslewA = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, loadA, inslewA);
            slewA = stm_modtbl_slew (slewtabA, loadA, inslewA);
            delayB = stm_modtbl_constraint (delaytabB, slewA, STM_DONTCARE);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + cstr + delayB);
        }
    }

    if (!nsdA && nsdB) {
        delaytabAB = stm_modtbl_create (nsdB, 0, STM_CLOCK_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportckslewaxis (delaytabAB, delaytabB);
        delayA = stm_modtbl_delay (delaytabA, loadA, STM_DONTCARE);
        slewA = stm_modtbl_slew (slewtabA, loadA, STM_DONTCARE);
        for (i = 0; i < nsdB; i++) {
            ckslewB = stm_modtbl_getckslewaxisval (delaytabB, i);
            delayB  = stm_modtbl_constraint (delaytabB, slewA, ckslewB);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + cstr + delayB);
        }
    }

    if (!nsdA && !nsdB) {
        delaytabAB = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE) ;
        delayA = stm_modtbl_delay (delaytabA, loadA, STM_DONTCARE) ;
        slewA = stm_modtbl_slew (slewtabA, loadA, STM_DONTCARE);
        delayB = stm_modtbl_constraint (delaytabB, slewA, STM_DONTCARE) ;
        stm_modtbl_setconst (delaytabAB, delayA + cstr + delayB) ;
    }

    if (!delaytabAB) 
        avt_errmsg (STM_ERRMSG, "029", AVT_ERROR);

    return delaytabAB;
}

/****************************************************************************/

timing_table *stm_modtbl_mergecc (timing_table *dttable, timing_table *dstable, timing_table *cstrtable, float cstr, float dtload)
{
    timing_table *delaytabAB = NULL;
    timing_table *delaytabA;
    timing_table *slewtabA;
    timing_table *delaytabB;
    int           nldA,           /* nb load delaytable A */ 
                  nsdA,           /* nb slew delaytable A */ 
                  nsdB,           /* nb load delaytable B */ 
                  nldB;           /* nb slew delaytable B */ 
    int           i, j;
    float         inslewA, inslewB, slewA;
    float         delayA, delayB;
    float         loadA = dtload;
    
    delaytabA = dttable; 
    slewtabA = dstable; 
    delaytabB = cstrtable; 

    nldA = stm_modtbl_getnload (delaytabA);
    nsdA = stm_modtbl_getnslew (delaytabA);
    nldB = stm_modtbl_getnload (delaytabB);
    nsdB = stm_modtbl_getnslew (delaytabB);

    if (cstrtable && cstr) {
        avt_errmsg (STM_ERRMSG, "041", AVT_ERROR);
        return NULL;
    }

    if (nsdA && nsdB) {
        delaytabAB = stm_modtbl_create (nsdB, nsdA, STM_INPUT_SLEW, STM_CLOCK_SLEW);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabB);
        stm_modtbl_Yimportslewaxis (delaytabAB, delaytabA);
        for (i = 0; i < nsdA; i++) {
            inslewA = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, loadA, inslewA);
            slewA = stm_modtbl_slew (slewtabA, loadA, inslewA);
            for (j = 0; j < nsdB; j++) {
                inslewB = stm_modtbl_getslewaxisval (delaytabB, j);
                delayB = stm_modtbl_constraint (delaytabB, inslewB, slewA);
                stm_modtbl_set2Dval (delaytabAB, j, i, delayA + cstr + delayB);
            }
        }
    }

    if (nsdA && !nsdB) {
        delaytabAB = stm_modtbl_create (nsdA, 0, STM_CLOCK_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        for (i = 0; i < nsdA; i++) {
            inslewA = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, loadA, inslewA);
            slewA = stm_modtbl_slew (slewtabA, loadA, inslewA);
            delayB = stm_modtbl_constraint (delaytabB, STM_DONTCARE, slewA);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + cstr + delayB);
        }
    }

    if (!nsdA && nsdB) {
        delaytabAB = stm_modtbl_create (nsdB, 0, STM_INPUT_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabB);
        delayA = stm_modtbl_delay (delaytabA, loadA, STM_DONTCARE);
        slewA = stm_modtbl_slew (slewtabA, loadA, STM_DONTCARE);
        for (i = 0; i < nsdB; i++) {
            inslewB = stm_modtbl_getslewaxisval (delaytabB, i);
            delayB  = stm_modtbl_constraint (delaytabB, inslewB, slewA);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + cstr + delayB);
        }
    }

    if (!nsdA && !nsdB) {
        delaytabAB = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE) ;
        delayA = stm_modtbl_delay (delaytabA, loadA, STM_DONTCARE) ;
        slewA = stm_modtbl_slew (slewtabA, loadA, STM_DONTCARE);
        delayB = stm_modtbl_constraint (delaytabB, STM_DONTCARE, slewA) ;
        stm_modtbl_setconst (delaytabAB, delayA + cstr + delayB) ;
    }

    if (!delaytabAB) 
        avt_errmsg (STM_ERRMSG, "029", AVT_ERROR);

    return delaytabAB;
}

/****************************************************************************/

timing_table *stm_modtbl_merge (timing_table *delaytabA, timing_table *delaytabB, float delay, float load, float slew)
{
    timing_table *delaytabAB;
    int           nldA,           /* nb load delaytable A */ 
                  nsdA,           /* nb slew delaytable A */ 
                  nsdB,           /* nb load delaytable B */ 
                  nldB;           /* nb slew delaytable B */ 
    int           i, j;
    float         inslew, outload;
    float         delayA, delayB;

    nldA = stm_modtbl_getnload (delaytabA);
    nsdA = stm_modtbl_getnslew (delaytabA);
    nldB = stm_modtbl_getnload (delaytabB);
    nsdB = stm_modtbl_getnslew (delaytabB);

    if (nsdA && nldB) {
        delaytabAB = stm_modtbl_create (nsdA, nldB, STM_INPUT_SLEW, STM_LOAD);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        stm_modtbl_Yimportloadaxis (delaytabAB, delaytabB);
        for (i = 0; i < nsdA; i++) {
            inslew = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, load, inslew);
            for (j = 0; j < nldB; j++) {
                outload = stm_modtbl_getloadaxisval (delaytabB, j);
                delayB = stm_modtbl_delay (delaytabB, outload, slew);
                stm_modtbl_set2Dval (delaytabAB, i, j, delayA + delay + delayB);
            }
        }
    }

    if (nsdA && !nldB) {
        delaytabAB = stm_modtbl_create (nsdA, 0, STM_INPUT_SLEW, STM_NOTYPE);
        stm_modtbl_Ximportslewaxis (delaytabAB, delaytabA);
        delayB = stm_modtbl_delay (delaytabB, STM_DONTCARE, slew);
        for (i = 0; i < nsdA; i++) {
            inslew = stm_modtbl_getslewaxisval (delaytabA, i);
            delayA = stm_modtbl_delay (delaytabA, load, inslew);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + delay + delayB);
        }
    }

    if (!nsdA && nldB) {
        delaytabAB = stm_modtbl_create (nldB, 0, STM_LOAD, STM_NOTYPE);
        stm_modtbl_Ximportloadaxis (delaytabAB, delaytabB);
        delayA = stm_modtbl_delay (delaytabA, load, STM_DONTCARE);
        for (i = 0; i < nldB; i++) {
            outload = stm_modtbl_getloadaxisval (delaytabB, i);
            delayB  = stm_modtbl_delay (delaytabB, outload, slew);
            stm_modtbl_set1Dval (delaytabAB, i, delayA + delay + delayB);
        }
    }

    if (!nsdA && !nldB) {
        delaytabAB = stm_modtbl_create (0, 0, STM_NOTYPE, STM_NOTYPE) ;
        delayA = stm_modtbl_delay (delaytabA, load, STM_DONTCARE) ;
        delayB = stm_modtbl_delay (delaytabB, STM_DONTCARE, slew) ;
        stm_modtbl_setconst (delaytabAB, delayA + delay + delayB) ;
    }

    if (!delaytabAB) 
        avt_errmsg (STM_ERRMSG, "029", AVT_ERROR);

    return delaytabAB;
}

/****************************************************************************/

timing_table *stm_modtbl_multidelaymerge (timing_table *stableA, 
                                       timing_table *dtableA,
                                       float         capa,
                                       timing_table *dtableB)
{
   timing_table *dtable = NULL;
   int           nl, ns;
   int           i, j;
   float         delay = 0, slew, newslew, load;

   ns = stm_modtbl_getnslew (dtableA);
   nl = stm_modtbl_getnload (dtableB);
    
   if (ns && nl) {
      dtable = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);
      stm_modtbl_Ximportslewaxis (dtable, dtableA);
      stm_modtbl_Yimportloadaxis (dtable, dtableB);
      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (dtableA, i);
         for (j = 0; j < nl; j++) {
            load = stm_modtbl_getloadaxisval (dtableB, j);
            newslew = slew;
            delay = stm_modtbl_delay (dtableA, capa, slew);
            newslew = stm_modtbl_slew (stableA, capa, newslew);
            delay += stm_modtbl_delay (dtableB, load, slew);
            stm_modtbl_set2Dval (dtable, i, j, delay);
         }
      }
   }
   else
   if (ns && !nl) {
      dtable = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);
      stm_modtbl_Ximportslewaxis (dtable, dtableA);
      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (dtableA, i);
         delay = stm_modtbl_delay (dtableA, capa, slew);
         slew = stm_modtbl_slew (stableA, capa, slew);
         delay = stm_modtbl_delay (dtableB, STM_DONTCARE, slew);
         stm_modtbl_set1Dval (dtable, i, delay);
      }
   }
   
   return dtable;
}

/****************************************************************************/

timing_table *stm_modtbl_multidelaymerge_n (chain_list *stables, chain_list *dtables)
{
   timing_table *dtable = NULL, *dtable_org, *dtable_end;
   int           nl, ns;
   int           i, j;
   chain_list   *dch, *sch;
   float         delay = 0, slew, newslew, load;

   dtable_org = (timing_table*)dtables->DATA;
   ns = stm_modtbl_getnslew (dtable_org);

   for (dch = dtables; dch->NEXT; dch = dch->NEXT);
   dtable_end = (timing_table*)dch->DATA;
   nl = stm_modtbl_getnload (dtable_end);
    
   if (ns && nl) {
      dtable = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);
      stm_modtbl_Ximportslewaxis (dtable, dtable_org);
      stm_modtbl_Yimportloadaxis (dtable, dtable_end);
      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (dtable_org, i);
         for (j = 0; j < nl; j++) {
            load = stm_modtbl_getloadaxisval (dtable_end, j);
            newslew = slew;
            delay = 0;
            for (dch = dtables, sch = stables; dch->NEXT; dch = dch->NEXT, sch = sch->NEXT) {
               delay += stm_modtbl_delay ((timing_table*)dch->DATA, STM_DONTCARE, slew);
               newslew = stm_modtbl_slew ((timing_table*)sch->DATA, STM_DONTCARE, newslew);
            }
            delay += stm_modtbl_delay ((timing_table*)dch->DATA, load, slew);
            newslew = stm_modtbl_slew ((timing_table*)sch->DATA, load, newslew);
            stm_modtbl_set2Dval (dtable, i, j, delay);
         }
      }
   }
   else
   if (ns && !nl) {
      dtable = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);
      stm_modtbl_Ximportslewaxis (dtable, dtable_org);
      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (dtable_org, i);
         delay = 0;
         for (dch = dtables, sch = stables; dch; dch = dch->NEXT, sch = sch->NEXT) {
            delay += stm_modtbl_delay ((timing_table*)dch->DATA, STM_DONTCARE, slew);
            slew = stm_modtbl_slew ((timing_table*)sch->DATA, STM_DONTCARE, slew);
         }
         stm_modtbl_set1Dval (dtable, i, delay);
      }
   }
   
   return dtable;
}

/****************************************************************************/

timing_table *stm_modtbl_multislewmerge (timing_table *stableA, float capa, timing_table *stableB)
{
   timing_table *stable = NULL;
   int           nl, ns;
   int           i, j;
   float         slew, load, newslew;

   ns  = stm_modtbl_getnslew (stableA);
   nl  = stm_modtbl_getnload (stableB);
    
   if (ns && nl) {
      stable = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);
      stm_modtbl_Ximportslewaxis (stable, stableA);
      stm_modtbl_Yimportloadaxis (stable, stableB);

      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (stableA, i);
         for (j = 0; j < nl; j++) {
            load = stm_modtbl_getloadaxisval (stableB, j);
            newslew = stm_modtbl_slew (stableA, capa, slew);
            newslew = stm_modtbl_slew (stableB, load, newslew);
            stm_modtbl_set2Dval (stable, i, j, newslew);
         }
      }
   }
   else
   if (ns && !nl) {
      stable = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);
      stm_modtbl_Ximportslewaxis (stable, stableA);

      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (stableA, i);
         slew = stm_modtbl_slew (stableA, capa, slew);
         slew = stm_modtbl_slew (stableB, capa, slew);
         stm_modtbl_set1Dval (stable, i, slew);
      }
   }

   return stable;
}

/****************************************************************************/

timing_table *stm_modtbl_multislewmerge_n (chain_list *stables)
{
   timing_table *stable = NULL, *stable_org, *stable_end;
   int           nl, ns;
   int           i, j;
   float         slew, load, newslew;
   chain_list   *sch;

   stable_org = (timing_table*)stables->DATA;
   ns  = stm_modtbl_getnslew (stable_org);

   for (sch = stables; sch->NEXT; sch = sch->NEXT);
   stable_end = (timing_table*)sch->DATA;
   nl  = stm_modtbl_getnload (stable_end);
    
   if (ns && nl) {
      stable = stm_modtbl_create (ns, nl, STM_INPUT_SLEW, STM_LOAD);
      stm_modtbl_Ximportslewaxis (stable, stable_org);
      stm_modtbl_Yimportloadaxis (stable, stable_end);

      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (stable_org, i);
         for (j = 0; j < nl; j++) {
            load = stm_modtbl_getloadaxisval (stable_end, j);
            newslew = slew;
            for (sch = stables; sch->NEXT; sch = sch->NEXT)
               newslew = stm_modtbl_slew ((timing_table*)sch->DATA, STM_DONTCARE, newslew);
            newslew = stm_modtbl_slew ((timing_table*)sch->DATA, load, newslew);
            stm_modtbl_set2Dval (stable, i, j, newslew);
         }
      }
   }
   else
   if (ns && !nl) {
      stable = stm_modtbl_create (ns, 0, STM_INPUT_SLEW, STM_NOTYPE);
      stm_modtbl_Ximportslewaxis (stable, stable_org);

      for (i = 0; i < ns; i++) {
         slew = stm_modtbl_getslewaxisval (stable_org, i);
         for (sch = stables; sch; sch = sch->NEXT) 
            slew = stm_modtbl_slew ((timing_table*)sch->DATA, STM_DONTCARE, slew);
         stm_modtbl_set1Dval (stable, i, slew);
      }
   }

   return stable;
}

/****************************************************************************/

int stm_modtbl_sametablesize (timing_table *tableA, timing_table *tableB)
{
   if (tableA && !tableB)
      return 0;

   if (!tableA && tableB)
      return 0;

   if (!tableA && !tableB)
      return 1;

   if (tableA->SET1D && tableB->SET1D)
      return 1;

   if (tableA->SET2D && tableB->SET2D)
      return 1;

   return 0;
}


