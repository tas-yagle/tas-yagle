#include <stdio.h>
#include AVT_H
#include "api_communication.h"
#include GEN_H
#include STB_H
#include "api_common_structures.h"
//#include "gen_API.h"
#define API_USE_REAL_TYPES
#include "ctk_api_local.h"
#include "ctk_API.h"

lofig_list *CTK_NETLIST=NULL;
ctk_struct *CTK_STRUCT=NULL;

void SET_CTK_NETLIST(lofig_list *lf)
{
  CTK_NETLIST=lf;
}

lofig_list *GET_CTK_NETLIST()
{
  return CTK_NETLIST;
}

void SET_CTK_CONTEXT(ctk_struct *env)
{
  CTK_STRUCT=env;
}

ctk_struct *GET_CTK_CONTEXT()
{
  return CTK_STRUCT;
}

int ctk_WorstCase()
{
  int state;
  if (TTV_COM==NULL) return 0;
  if (TTV_COM(TTV_getdelaytype, &state)) return 0;
  if (state==TTV_getdelaytype_max) return 1;
  return 0;
}

int ctk_BestCase()
{
  int state;
  if (TTV_COM==NULL) return 0;
  if (TTV_COM(TTV_getdelaytype, &state)) return 0;
  if (state==TTV_getdelaytype_min) return 1;
  return 0;
}

int ctk_Active()
{
  if (STB_COM==NULL) return 0;
  return 1;
}

static int ctk_communication(int op, void *data)
{
  switch(op)
    {
    case CTK_rundefaultaction:
      // switch api_default_ctk_mthod
      ctk_default_action_0c2c();
      return 0;
    case CTK_getctkstruct:
      *(ctk_struct **)data=CTK_STRUCT;
      return 0;
    case CTK_getctknetlist:
      *(lofig_list **)data=CTK_NETLIST;
      return 0;
    }
  return 1;
}

void ctk_API_AtLoad_Initialize()// commentaire pour desactiver l'ajout de token
{
  CTK_API_COM=ctk_communication;
}

void ctk_API_Action_Terminate()// commentaire pour desactiver l'ajout de token
{
//  if (CTK_STRUCT!=NULL) // freectkstruct
  CTK_STRUCT=NULL;
}

ctk_struct *ctk_CreateContext()
{
  ctk_struct *ctks;
  ctks=(ctk_struct *)mbkalloc(sizeof(ctk_struct));
  ctks->victim_ctk_info=addht_v2(10000);
  ctks->crosstalk_to_ground=0;
  ctks->external_crosstalk_to_ground_ratio=1;
  return ctks;
}

void ctk_FreeContext(ctk_struct *cs)
{

  if (cs==CTK_STRUCT)
    {
      CTK_STRUCT=NULL;
    }
}

void ctk_SetCtkRatio(ctk_struct *cc, losig_list *lsv, losig_list *lsa, double ratio)
{
  ctk_PerVictimParam *cpvp;
  ctk_VictimVSAgressorParam *cvvsap;
  long l;
  if ((l=gethtitem_v2(cc->victim_ctk_info, lsv))==EMPTYHT)
    {
      cpvp=(ctk_PerVictimParam *)mbkalloc(sizeof(ctk_VictimVSAgressorParam));
      cpvp->agressor_ctk_info=addht_v2(100);
      addhtitem_v2(cc->victim_ctk_info, lsv, (long)cpvp);
    }
  else
    cpvp=(ctk_PerVictimParam *)l;

  if ((l=gethtitem_v2(cpvp->agressor_ctk_info, lsa))==EMPTYHT)
    {
      cvvsap=(ctk_VictimVSAgressorParam *)mbkalloc(sizeof(ctk_VictimVSAgressorParam));
      addhtitem_v2(cpvp->agressor_ctk_info, lsa, (long)cvvsap);
    }
  else
    cvvsap=(ctk_VictimVSAgressorParam *)l;

  cvvsap->ctk_ratio=ratio;
}

void ctk_SetInternalCrosstalkToGround(ctk_struct *cs, double ratio)
{
  cs->crosstalk_to_ground=1;
  cs->crosstalk_to_ground_ratio=ratio;
}

void ctk_SetExternalCrosstalkRatio(ctk_struct *cs, double ratio)
{
  cs->external_crosstalk_to_ground_ratio=ratio;
}
