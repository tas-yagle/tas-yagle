#include AVT_H
#include GEN_H
#include INF_H
#include YAG_H
#include MSL_H
#define API_USE_REAL_TYPES
#include "gen_API_netlist.h"
#include "gen_API.h"



lofig_list *gns_GetBlackboxNetlist(char *name)
{
  if (external_getlofig==NULL)
    return getlofig(name, 'A');
  return external_getlofig(namealloc(name));
}

ALL_FOR_GNS *gns_RunGNS(lofig_list *lf, char *celldir, char *libname)
{
  mbkContext *curctx;
  ALL_FOR_GNS *old_run=LATEST_GNS_RUN, *new_run;
  chain_list *rec;
  mbk_chrono mc;
  char buf[128];
  inffig_list *ifl;
  
  avt_fprintf(stdout,"¤1____ Start of GNS sub execution on '%s' ____¤.\n",lf->NAME);
  curctx=mbkCreateContext();

  transfert_needed_lofigs(lf, curctx);

/*  lf=rduplofig(lf);
  lofigchain(lf);*/
  addhtitem(curctx->HT_LOFIG, lf->NAME, (long)lf);
  
  mbkSwitchContext(curctx);

  if (lf->LOINS!=NULL)
    {
      avt_error("gnsapi", -1, AVT_INFO, "Flattening Netlist...");
      fflush(stderr);
      mbk_StartChrono(&mc);
      flatOutsideList(lf);
      mbk_StopChrono(&mc);
      avt_fprintf(stderr, " (¤3%s¤.)\n", mbk_GetUserChrono(&mc, buf));
    }
  ifl=getinffig(lf->NAME);
  
  yagFindSupplies(ifl, lf, TRUE);
  yagFindInternalSupplies(ifl, lf, FALSE);
  yagTestTransistors(lf, FALSE);

  rec=genius_main(lf, celldir, libname, NULL);

  // demarquer les trans //
  mbkSwitchContext(curctx);

  avt_fprintf(stdout,"¤1____ End of GNS sub execution for '%s' ____¤.\n",lf->NAME);
  new_run=LATEST_GNS_RUN;
  LATEST_GNS_RUN=old_run;
  return new_run;
}

void gns_DestroyGNSRun(ALL_FOR_GNS *afg)
{
  
  // free des lofig gns
  //delht(afg->HIER_HT_LOFIG);
  //DeleteHeap(&afg->corresp_heap);
  //DeleteHeap(&afg->subinst_heap);
  // delht(afg->ALL_INSTANCES);
  // freechain(afg->RETURN);
  // mbkfree(afg);
}


static chain_list *AFG=NULL;

void gns_EnterGNSContext(ALL_FOR_GNS *afg, char *instance)
{
  subinst_t *sins;
  sins=gen_get_hier_instance(afg, instance);
  AFG=addchain(AFG, LATEST_GNS_RUN);
  LATEST_GNS_RUN=afg;
  in_genius_context_of(afg, sins);
}

void gns_ExitGNSContext()
{
  chain_list *cl;
  if (AFG==NULL)
    fprintf(stderr,"%s: gns_ExitGNSContext: Not in a genius context\n", gen_info());
  else
    {
      out_genius_context_of();
      LATEST_GNS_RUN=(ALL_FOR_GNS *)AFG->DATA;
      cl=AFG;
      AFG=AFG->NEXT;
      cl->NEXT=NULL;
      freechain(cl);
    }
}

chain_list *gns_GetGNSTopLevels(ALL_FOR_GNS *afg)
{
  chain_list *cl=NULL;
  subinst_t *sins;
  for (sins=afg->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT)
    {
      cl=addchain(cl, sins->INSNAME);
    }
  return cl;
}

lofig_list *gns_CutNetlist(ALL_FOR_GNS *afg)
{
  mbk_set_cut_info("bbox", FCL_INSLIST_PTYPE, FCL_TRANSLIST_PTYPE);
  return mbk_modify_hierarchy(afg->GLOBAL_LOFIG, afg->RETURN, 1);
}
