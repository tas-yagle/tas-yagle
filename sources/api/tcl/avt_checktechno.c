#include STM_H
#include TTV_H
#include MLU_H
#include MUT_H
#include INF_H
#include EFG_H
#include TAS_H
#include TRC_H
//#include YAG_H
#include MCC_H

#include AVT_H
#include "../../tas/mcc/mcc_mod_util.h"

void avt_CheckTechno(char *label, char *tn, char *tp)
{
  hitas_tcl_pt=(hitas_tcl_pt_t) hitas_tcl;
  mcc_CheckTechno(label, tn, tp);
}
