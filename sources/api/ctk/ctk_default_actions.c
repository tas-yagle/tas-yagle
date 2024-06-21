
#include <stdio.h>
#include AVT_H
#include "api_communication.h"
#include GEN_H
#include STB_H
//#include "gen_API.h"
#include "api_common_structures.h"
#include "ctk_api_local.h"
#include "ctk_API.h"

void ctk_default_action_0c2c()
{
  ctk_struct *cs=ctk_CreateContext();

  SET_CTK_CONTEXT(cs);

  if (ctk_WorstCase())
    {
      ctk_SetInternalCrosstalkToGround(CTK_STRUCT, 2.0);
      ctk_SetExternalCrosstalkRatio(CTK_STRUCT, 2.0);
    }
  else
    {
      ctk_SetInternalCrosstalkToGround(CTK_STRUCT, 0.0);
      ctk_SetExternalCrosstalkRatio(CTK_STRUCT, 0.0);
    }
}

void ctk_default_action_miller()
{
  chain_list *cl, *ch;
  losig_list *lsv;
  ctk_struct *cs=ctk_CreateContext();

  SET_CTK_CONTEXT(cs);

  cl=ctk_GetAllVictims();
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      lsv=(losig_list *)ch->DATA;
      // recup ags actifs
      // calc capa miller selon type de delai
      // ajout de l'info dans la ctk struct
    }
  freechain(cl);
}
