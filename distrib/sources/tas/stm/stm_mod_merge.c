/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit: STM Version 1.00                                             */
/*    Fichier: stm_mod_merge.c                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s): Gilles Augustins                                           */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/


/**************************************************/
/* define to get the old version of stm_mod_merge */
//#define OLD_VERS
/**************************************************/

#ifndef OLD_VERS

/****************************************************************************/
/*{{{                    stm_mod_mergec()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_mergec(char *name,
                             timing_model *dtmod,
                             timing_model *ckmod,
                             float constr, float load, float ckload)
{
  timing_model  *mrgmod = NULL,
                 null_struct,
                *dtmod_tbl = &null_struct,
                *ckmod_tbl = &null_struct;
  int            delckmod = 0,
                 deldtmod = 0;

  null_struct.UTYPE             = STM_MOD_MODTBL;
  null_struct.UMODEL.TABLE      = NULL;
  
  if (dtmod)
    switch (dtmod->UTYPE)
    {
      case STM_MOD_MODTBL:
           dtmod_tbl            = dtmod;
           break;
      case STM_MOD_MODSCM: case STM_MOD_MODPLN: case STM_MOD_MODFCT:
           dtmod_tbl            = stm_mod_duplicate (NULL, dtmod);
           deldtmod             = 1;
    }
  
  if (ckmod)
    switch (ckmod->UTYPE)
    {
      case STM_MOD_MODTBL:
           ckmod_tbl            = ckmod;
           break;
      case STM_MOD_MODSCM: case STM_MOD_MODPLN: case STM_MOD_MODFCT:
           ckmod_tbl            = stm_mod_duplicate (NULL, ckmod);
           delckmod             = 1;
    }
  
  if (ckmod_tbl->UMODEL.TABLE || dtmod_tbl->UMODEL.TABLE)
  {
    mrgmod                      = stm_mod_create(name);
    mrgmod->UTYPE               = STM_MOD_MODTBL;
    stm_mod_defaultstm2tbl_delay(dtmod_tbl,~STM_NOTYPE,STM_NOTYPE,-1.0,load);
    stm_mod_defaultstm2tbl_delay(ckmod_tbl,~STM_NOTYPE,STM_NOTYPE,-1.0,ckload);
    mrgmod->UMODEL.TABLE        = stm_modtbl_mergec(dtmod_tbl->UMODEL.TABLE,
                                                    ckmod_tbl->UMODEL.TABLE,
                                                    constr, load, ckload);
    if (deldtmod)
      stm_mod_destroy(dtmod_tbl);
    if (delckmod)
      stm_mod_destroy(ckmod_tbl);
  }
  
  return mrgmod;
}

#else
/*}}}************************************************************************/
/*{{{                    old                                                */
timing_model *stm_mod_mergec (char *name, timing_model *dtmod, timing_model *ckmod, float constr, float load, float ckload)
{
    timing_model *mrgmod = NULL,
                 *dtmod_tbl,
                 *ckmod_tbl;
    long          a_type;
    long          b_type;
             

    if (dtmod)
        switch (dtmod->UTYPE) {
            case STM_MOD_MODTBL:
                a_type = STM_MOD_MODTBL;
                break;
            case STM_MOD_MODSCM:
                a_type = STM_MOD_MODSCM;
                break;
            case STM_MOD_MODPLN:
                a_type = STM_MOD_MODPLN;
                break;
            case STM_MOD_MODNULL:
            case STM_MOD_MODFCT:
                a_type = STM_MOD_MODNULL;
                break;
        }
     else
        a_type = STM_MOD_MODNULL;

    if (ckmod)
        switch (ckmod->UTYPE) {
            case STM_MOD_MODTBL:
                b_type = STM_MOD_MODTBL;
                break;
            case STM_MOD_MODSCM:
                b_type = STM_MOD_MODSCM;
                break;
            case STM_MOD_MODPLN:
                b_type = STM_MOD_MODPLN;
                break;
            case STM_MOD_MODNULL:
            case STM_MOD_MODFCT:
                b_type = STM_MOD_MODNULL;
                break;
        }
     else
        b_type = STM_MOD_MODNULL;

    switch (a_type) {
        case STM_MOD_MODTBL:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod->UMODEL.TABLE, ckmod->UMODEL.TABLE, constr, load, ckload);
                    break;
                case STM_MOD_MODSCM:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultscm2tbl_delay (ckmod_tbl, -1.0, ckload);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod->UMODEL.TABLE, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODPLN:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultpln2tbl_delay (ckmod_tbl);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod->UMODEL.TABLE, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODNULL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod->UMODEL.TABLE, NULL, constr, load, ckload);
                    break;
            }
            break;
        case STM_MOD_MODSCM:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, ckmod->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    break;
                case STM_MOD_MODSCM:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                    stm_mod_defaultscm2tbl_delay (ckmod_tbl, -1.0, ckload);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODPLN:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                    stm_mod_defaultpln2tbl_delay (ckmod_tbl);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODNULL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, NULL, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    break;
            }
            break;
        case STM_MOD_MODPLN:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, ckmod->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    break;
                case STM_MOD_MODSCM:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                    stm_mod_defaultscm2tbl_delay (ckmod_tbl, -1.0, ckload);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODPLN:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                    stm_mod_defaultpln2tbl_delay (ckmod_tbl);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODNULL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                    stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (dtmod_tbl->UMODEL.TABLE, NULL, constr, load, ckload);
                    stm_mod_destroy (dtmod_tbl);
                    break;
            }
            break;
        case STM_MOD_MODNULL:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (NULL, ckmod->UMODEL.TABLE, constr, load, ckload);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    break;
                case STM_MOD_MODSCM:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultscm2tbl_delay (ckmod_tbl, -1.0, ckload);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (NULL, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODPLN:
                    mrgmod = stm_mod_create (name);
                    mrgmod->UTYPE = STM_MOD_MODTBL;
                    ckmod_tbl = stm_mod_duplicate (NULL, ckmod);
                    stm_mod_defaultpln2tbl_delay (ckmod_tbl);
                    mrgmod->UMODEL.TABLE = stm_modtbl_mergec (NULL, ckmod_tbl->UMODEL.TABLE, constr, load, ckload);
                    stm_mod_destroy (ckmod_tbl);
                    break;
                case STM_MOD_MODNULL:
                    mrgmod = NULL;
                    break;
            }
    }

    return mrgmod;
}
#endif
/*}}}************************************************************************/
///*{{{                  stm_mod_mergecd()                                    */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//timing_model *stm_mod_mergecd (char *name, timing_model *dtmod, timing_model *dsmod, timing_model *cstrmod, float constr, float load)
//{
//  timing_model *mrgmod = NULL,
//               *dtmod_tbl,
//               *cstrmod_tbl;
//  long          a_type;
//  long          b_type;
//
//
//  if (dtmod)
//    switch (dtmod->UTYPE) {
//      case STM_MOD_MODTBL:
//           a_type = STM_MOD_MODTBL;
//           break;
//      case STM_MOD_MODSCM:
//           a_type = STM_MOD_MODSCM;
//           break;
//      case STM_MOD_MODPLN:
//           a_type = STM_MOD_MODPLN;
//           break;
//      case STM_MOD_MODNULL:
//      case STM_MOD_MODFCT:
//           a_type = STM_MOD_MODNULL;
//           break;
//    }
//  else
//    a_type = STM_MOD_MODNULL;
//
//  if (cstrmod)
//    switch (cstrmod->UTYPE) {
//      case STM_MOD_MODTBL:
//           b_type = STM_MOD_MODTBL;
//           break;
//      case STM_MOD_MODSCM:
//           b_type = STM_MOD_MODSCM;
//           break;
//      case STM_MOD_MODPLN:
//           b_type = STM_MOD_MODPLN;
//           break;
//      case STM_MOD_MODNULL:
//      case STM_MOD_MODFCT:
//           b_type = STM_MOD_MODNULL;
//           break;
//    }
//  else
//    b_type = STM_MOD_MODNULL;
//
//  switch (a_type) {
//    case STM_MOD_MODTBL:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
//                break;
//         }
//         break;
//    case STM_MOD_MODSCM:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//         }
//         break;
//    case STM_MOD_MODPLN:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//         }
//         break;
//    case STM_MOD_MODNULL:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (NULL, NULL, cstrmod->UMODEL.TABLE, constr, load);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = NULL;
//                break;
//         }
//  }
//
//  return mrgmod;
//}

/*}}}************************************************************************/
/*{{{                    old                                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_mergecd (char *name, timing_model *dtmod, timing_model *dsmod, timing_model *cstrmod, float constr, float load)
{
  timing_model *mrgmod = NULL,
               *dtmod_tbl,
               *cstrmod_tbl;
  long          a_type;
  long          b_type;


  if (dtmod)
    switch (dtmod->UTYPE) {
      case STM_MOD_MODTBL:
           a_type = STM_MOD_MODTBL;
           break;
      case STM_MOD_MODSCM:
           a_type = STM_MOD_MODSCM;
           break;
      case STM_MOD_MODPLN:
           a_type = STM_MOD_MODPLN;
           break;
      case STM_MOD_MODNULL:
      case STM_MOD_MODFCT:
           a_type = STM_MOD_MODNULL;
           break;
    }
  else
    a_type = STM_MOD_MODNULL;

  if (cstrmod)
    switch (cstrmod->UTYPE) {
      case STM_MOD_MODTBL:
           b_type = STM_MOD_MODTBL;
           break;
      case STM_MOD_MODSCM:
           b_type = STM_MOD_MODSCM;
           break;
      case STM_MOD_MODPLN:
           b_type = STM_MOD_MODPLN;
           break;
      case STM_MOD_MODNULL:
      case STM_MOD_MODFCT:
           b_type = STM_MOD_MODNULL;
           break;
    }
  else
    b_type = STM_MOD_MODNULL;

  switch (a_type) {
    case STM_MOD_MODTBL:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
                break;
         }
         break;
    case STM_MOD_MODSCM:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
         }
         break;
    case STM_MOD_MODPLN:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
         }
         break;
    case STM_MOD_MODNULL:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (NULL, NULL, cstrmod->UMODEL.TABLE, constr, load);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecd (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = NULL;
                break;
         }
  }

  return mrgmod;
}

/*}}}************************************************************************/
///*{{{                  stm_mod_mergecc()                                    */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//timing_model *stm_mod_mergecc (char *name, timing_model *dtmod, timing_model *dsmod, timing_model *cstrmod, float constr, float load)
//{
//  timing_model *mrgmod = NULL,
//               *dtmod_tbl,
//               *cstrmod_tbl;
//  long          a_type;
//  long          b_type;
//
//
//  if (dtmod)
//    switch (dtmod->UTYPE) {
//      case STM_MOD_MODTBL:
//           a_type = STM_MOD_MODTBL;
//           break;
//      case STM_MOD_MODSCM:
//           a_type = STM_MOD_MODSCM;
//           break;
//      case STM_MOD_MODPLN:
//           a_type = STM_MOD_MODPLN;
//           break;
//      case STM_MOD_MODNULL:
//      case STM_MOD_MODFCT:
//           a_type = STM_MOD_MODNULL;
//           break;
//    }
//  else
//    a_type = STM_MOD_MODNULL;
//
//  if (cstrmod)
//    switch (cstrmod->UTYPE) {
//      case STM_MOD_MODTBL:
//           b_type = STM_MOD_MODTBL;
//           break;
//      case STM_MOD_MODSCM:
//           b_type = STM_MOD_MODSCM;
//           break;
//      case STM_MOD_MODPLN:
//           b_type = STM_MOD_MODPLN;
//           break;
//      case STM_MOD_MODNULL:
//      case STM_MOD_MODFCT:
//           b_type = STM_MOD_MODNULL;
//           break;
//    }
//  else
//    b_type = STM_MOD_MODNULL;
//
//  switch (a_type) {
//    case STM_MOD_MODTBL:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
//                break;
//         }
//         break;
//    case STM_MOD_MODSCM:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultscm2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//         }
//         break;
//    case STM_MOD_MODPLN:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
//                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
//                stm_mod_destroy (dtmod_tbl);
//                break;
//         }
//         break;
//    case STM_MOD_MODNULL:
//         switch (b_type) {
//           case STM_MOD_MODTBL:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (NULL, NULL, cstrmod->UMODEL.TABLE, constr, load);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                break;
//           case STM_MOD_MODSCM:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultscm2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODPLN:
//                mrgmod = stm_mod_create (name);
//                mrgmod->UTYPE = STM_MOD_MODTBL;
//                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
//                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
//                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
//                stm_mod_destroy (cstrmod_tbl);
//                break;
//           case STM_MOD_MODNULL:
//                mrgmod = NULL;
//                break;
//         }
//  }
//
//  return mrgmod;
//}
//
///*}}}************************************************************************/
/*{{{                    old                                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_mergecc (char *name, timing_model *dtmod, timing_model *dsmod, timing_model *cstrmod, float constr, float load)
{
  timing_model *mrgmod = NULL,
               *dtmod_tbl,
               *cstrmod_tbl;
  long          a_type;
  long          b_type;


  if (dtmod)
    switch (dtmod->UTYPE) {
      case STM_MOD_MODTBL:
           a_type = STM_MOD_MODTBL;
           break;
      case STM_MOD_MODSCM:
           a_type = STM_MOD_MODSCM;
           break;
      case STM_MOD_MODPLN:
           a_type = STM_MOD_MODPLN;
           break;
      case STM_MOD_MODNULL:
      case STM_MOD_MODFCT:
           a_type = STM_MOD_MODNULL;
           break;
    }
  else
    a_type = STM_MOD_MODNULL;

  if (cstrmod)
    switch (cstrmod->UTYPE) {
      case STM_MOD_MODTBL:
           b_type = STM_MOD_MODTBL;
           break;
      case STM_MOD_MODSCM:
           b_type = STM_MOD_MODSCM;
           break;
      case STM_MOD_MODPLN:
           b_type = STM_MOD_MODPLN;
           break;
      case STM_MOD_MODNULL:
      case STM_MOD_MODFCT:
           b_type = STM_MOD_MODNULL;
           break;
    }
  else
    b_type = STM_MOD_MODNULL;

  switch (a_type) {
    case STM_MOD_MODTBL:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
                break;
         }
         break;
    case STM_MOD_MODSCM:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultscm2tbl_delay (dtmod_tbl, -1.0, load);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
         }
         break;
    case STM_MOD_MODPLN:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (dtmod_tbl);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                dtmod_tbl = stm_mod_duplicate (NULL, dtmod);
                stm_mod_defaultpln2tbl_delay (dtmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (dtmod_tbl->UMODEL.TABLE, dsmod->UMODEL.TABLE, NULL, constr, load);
                stm_mod_destroy (dtmod_tbl);
                break;
         }
         break;
    case STM_MOD_MODNULL:
         switch (b_type) {
           case STM_MOD_MODTBL:
                mrgmod = stm_mod_create (name);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (NULL, NULL, cstrmod->UMODEL.TABLE, constr, load);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                break;
           case STM_MOD_MODSCM:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultscm2tbl_delay (cstrmod_tbl, -1.0, -1.0);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODPLN:
                mrgmod = stm_mod_create (name);
                mrgmod->UTYPE = STM_MOD_MODTBL;
                cstrmod_tbl = stm_mod_duplicate (NULL, cstrmod);
                stm_mod_defaultpln2tbl_delay (cstrmod_tbl);
                mrgmod->UMODEL.TABLE = stm_modtbl_mergecc (NULL, NULL, cstrmod_tbl->UMODEL.TABLE, constr, load);
                stm_mod_destroy (cstrmod_tbl);
                break;
           case STM_MOD_MODNULL:
                mrgmod = NULL;
                break;
         }
  }

  return mrgmod;
}
#ifndef OLD_VERS
/*}}}************************************************************************/
/*{{{                    stm_mod_merge()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
timing_model *stm_mod_merge(char *name,
                            timing_model *modA,
                            timing_model *modB,
                            float delay, float load, float slew)
{
  timing_model  *modAB = NULL,
                 null_struct,
                *modA_tbl = &null_struct,
                *modB_tbl = &null_struct;
  int            delmodB = 0,
                 delmodA = 0;

  null_struct.UTYPE             = STM_MOD_MODTBL;
  null_struct.UMODEL.TABLE      = NULL;

  if (modA)
    switch (modA->UTYPE)
    {
      case STM_MOD_MODTBL:
           modA_tbl            = modA;
           break;
      case STM_MOD_MODSCM: case STM_MOD_MODPLN: case STM_MOD_MODFCT:
           modA_tbl            = stm_mod_duplicate (NULL, modA);
           delmodA             = 1;
    }
  
  if (modB)
    switch (modB->UTYPE)
    {
      case STM_MOD_MODTBL:
           modB_tbl            = modB;
           break;
      case STM_MOD_MODSCM: case STM_MOD_MODPLN: case STM_MOD_MODFCT:
           modB_tbl            = stm_mod_duplicate (NULL, modB);
           delmodB             = 1;
    }
  
  if (modA_tbl->UMODEL.TABLE || modB_tbl->UMODEL.TABLE)
  {
    modAB                   = stm_mod_create(name);
    modAB->UTYPE            = STM_MOD_MODTBL;
    stm_mod_defaultstm2tbl_delay(modA_tbl,~STM_NOTYPE,STM_NOTYPE, -1.0,load);
    stm_mod_defaultstm2tbl_delay(modB_tbl,STM_NOTYPE,~STM_NOTYPE,slew, -1.0);
    modAB->UMODEL.TABLE     = stm_modtbl_merge(modA_tbl->UMODEL.TABLE,
                                               modB_tbl->UMODEL.TABLE,
                                               delay, load, slew);
    if (modB_tbl->UMODEL.TABLE)
      stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
    else
      stm_mod_update (modAB, modA->VTH, modA->VDD, modA->VT, STM_UNKNOWN);
    if (delmodA)
      stm_mod_destroy(modA_tbl);
    if (delmodB)
      stm_mod_destroy(modB_tbl);
  }

  return modAB;
}

#else
/*}}}************************************************************************/
/*{{{                    old                                                */
timing_model *stm_mod_merge (char *name, timing_model *modA, timing_model *modB, float delay, float load, float slew)
{
    timing_model *modAB = NULL;
    timing_model *modA_tbl;
    timing_model *modB_tbl;
    long          a_type;
    long          b_type;

    if (modA)
        switch (modA->UTYPE) {
            case STM_MOD_MODTBL:
                a_type = STM_MOD_MODTBL;
                break;
            case STM_MOD_MODSCM:
                a_type = STM_MOD_MODSCM;
                break;
            case STM_MOD_MODPLN:
                a_type = STM_MOD_MODPLN;
                break;
            case STM_MOD_MODNULL:
            case STM_MOD_MODFCT:
                a_type = STM_MOD_MODNULL;
                break;
        }
     else
        a_type = STM_MOD_MODNULL;

    if (modB)
        switch (modB->UTYPE) {
            case STM_MOD_MODTBL:
                b_type = STM_MOD_MODTBL;
                break;
            case STM_MOD_MODSCM:
                b_type = STM_MOD_MODSCM;
                break;
            case STM_MOD_MODPLN:
                b_type = STM_MOD_MODPLN;
                break;
            case STM_MOD_MODNULL:
            case STM_MOD_MODFCT:
                b_type = STM_MOD_MODNULL;
                break;
        }
     else
        b_type = STM_MOD_MODNULL;


    switch (a_type) {
        case STM_MOD_MODTBL:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA->UMODEL.TABLE, modB->UMODEL.TABLE, delay, load, slew);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    break;
                case STM_MOD_MODSCM:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultscm2tbl_delay (modB_tbl, slew, -1.0);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA->UMODEL.TABLE, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODPLN:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultpln2tbl_delay (modB_tbl);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA->UMODEL.TABLE, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODNULL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modA->VTH, modA->VDD, modA->VT, STM_UNKNOWN);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA->UMODEL.TABLE, NULL, delay, load, slew);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    break;
            }
            break;
        case STM_MOD_MODSCM:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    stm_mod_defaultscm2tbl_delay (modA_tbl, -1.0, load);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, modB->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
                    break;
                case STM_MOD_MODSCM:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultscm2tbl_delay (modA_tbl, -1.0, load);
                    stm_mod_defaultscm2tbl_delay (modB_tbl, slew, -1.0);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODPLN:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultscm2tbl_delay (modA_tbl, -1.0, load);
                    stm_mod_defaultpln2tbl_delay (modB_tbl);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODNULL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modA->VTH, modA->VDD, modA->VT, STM_UNKNOWN);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    stm_mod_defaultscm2tbl_delay (modA_tbl, -1.0, load);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, NULL, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
            }
            break;
        case STM_MOD_MODPLN:
            switch (b_type) {
                case STM_MOD_MODTBL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    stm_mod_defaultpln2tbl_delay (modA_tbl);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, modB->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
                    break;
                case STM_MOD_MODSCM:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultpln2tbl_delay (modA_tbl);
                    stm_mod_defaultscm2tbl_delay (modB_tbl, slew, -1.0);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODPLN:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultpln2tbl_delay (modA_tbl);
                    stm_mod_defaultpln2tbl_delay (modB_tbl);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODNULL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modA->VTH, modA->VDD, modA->VT, STM_UNKNOWN);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modA_tbl = stm_mod_duplicate (NULL, modA);
                    stm_mod_defaultpln2tbl_delay (modA_tbl);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (modA_tbl->UMODEL.TABLE, NULL, delay, load, slew);
                    stm_mod_destroy (modA_tbl);
            }
            break;
        case STM_MOD_MODNULL:
            switch (b_type) {
                case STM_MOD_MODSCM:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultscm2tbl_delay (modB_tbl, slew, -1.0);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (NULL, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODTBL:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (NULL, modB->UMODEL.TABLE, delay, load, slew);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    break;
                case STM_MOD_MODPLN:
                    modAB = stm_mod_create (name);
                    stm_mod_update (modAB, modB->VTH, modB->VDD, modB->VT, modB->VF);
                    modAB->UTYPE = STM_MOD_MODTBL;
                    modB_tbl = stm_mod_duplicate (NULL, modB);
                    stm_mod_defaultpln2tbl_delay (modB_tbl);
                    modAB->UMODEL.TABLE = stm_modtbl_merge (NULL, modB_tbl->UMODEL.TABLE, delay, load, slew);
                    stm_mod_destroy (modB_tbl);
                    break;
                case STM_MOD_MODNULL:
                    modAB = NULL;
                    break;
            }
    }

    return modAB;
}
/*}}}************************************************************************/
#endif
/****************************************************************************/

timing_model *stm_mod_multidelaymerge (timing_model *smodelA, timing_model *dmodelA, float capa, timing_model *dmodelB)
{
    timing_model *dmodel = stm_mod_create (NULL);
    stm_mod_update (dmodel, dmodelB->VTH, dmodelB->VDD, dmodelB->VT, dmodelB->VF);
    dmodel->UMODEL.TABLE = stm_modtbl_multidelaymerge (smodelA->UMODEL.TABLE, dmodelA->UMODEL.TABLE, capa, dmodelB->UMODEL.TABLE);
    dmodel->UTYPE = STM_MOD_MODTBL;

    return dmodel;
}

/****************************************************************************/

timing_model *stm_mod_multislewmerge (timing_model *smodelA, float capa, timing_model *smodelB)
{
    timing_model *dmodel = stm_mod_create (NULL);
    stm_mod_update (dmodel, smodelB->VTH, smodelB->VDD, smodelB->VT, smodelB->VF);
    dmodel->UMODEL.TABLE = stm_modtbl_multislewmerge (smodelA->UMODEL.TABLE, capa, smodelB->UMODEL.TABLE);
    dmodel->UTYPE = STM_MOD_MODTBL;

    return dmodel;
}

/****************************************************************************/

timing_model *stm_mod_multidelaymerge_n (chain_list *smodels, chain_list *dmodels)
{
    chain_list   *stabs = NULL;
    chain_list   *dtabs = NULL;
    chain_list   *ch;
    timing_model *dmodel = stm_mod_create (NULL);

    for (ch = smodels; ch; ch = ch->NEXT)
        stabs = addchain (stabs, ((timing_model*)smodels->DATA)->UMODEL.TABLE);
    stabs = reverse (stabs);

    for (ch = smodels; ch; ch = ch->NEXT)
        dtabs = addchain (dtabs, ((timing_model*)dmodels->DATA)->UMODEL.TABLE);
    dtabs = reverse (dtabs);

    dmodel->UMODEL.TABLE = stm_modtbl_multidelaymerge_n (stabs, dtabs);
    dmodel->UTYPE = STM_MOD_MODTBL;

    freechain (dtabs);
    freechain (stabs);

    return dmodel;
}

/****************************************************************************/

timing_model *stm_mod_multislewmerge_n (chain_list *smodels)
{
    chain_list   *stabs = NULL;
    chain_list   *ch;
    timing_model *smodel = stm_mod_create (NULL);

    for (ch = smodels; ch; ch = ch->NEXT)
        stabs = addchain (stabs, ((timing_model*)smodels->DATA)->UMODEL.TABLE);
    stabs = reverse (stabs);

    smodel->UMODEL.TABLE = stm_modtbl_multislewmerge_n (stabs);
    smodel->UTYPE = STM_MOD_MODTBL;

    freechain (stabs);

    return smodel;
}
