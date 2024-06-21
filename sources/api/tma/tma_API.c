#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include MSL_H
#include AVT_H
#include INF_H
#include ELP_H
#include CNS_H
#include YAG_H
#include TLC_H
#include TRC_H
#include STM_H
#include TTV_H
#include MCC_H
#include SIM_H
#include TAS_H
#include FCL_H
#include BEH_H
#include BHL_H
#include BEF_H
#include BVL_H
#include CBH_H
#include TUT_H
#include LIB_H
#include TLF_H
#include STB_H
#include TMA_H

#define API_USE_REAL_TYPES
#include "tma_API.h"
#include "../ttv/ttv_API_LOCAL.h"
#include "../ttv/ttv_API.h"
//#include "tma_API_blackbox.h"

/**********************************************************/

BehavioralFigure *beh_getbefig (char *name)
{
    befig_list *befig = NULL;
    char *beh_name = namealloc (name);
    FILE *behfile;

    BEH_IN = V_STR_TAB[__BEH_IN_FORMAT].VALUE;

    /* read from yagle context */
    if (YAG_CONTEXT) {
        for (befig = YAG_CONTEXT->YAG_BEFIG_LIST; befig; befig = befig->NEXT) {
            if (befig->NAME == beh_name) return befig;
        }
    }

    /* read from file */
    if ((behfile = mbkfopen (beh_name, BEH_IN, READ_TEXT))) { 
        fclose (behfile);
        BEH_IN = NULL;
        if ((befig = loadbefig (NULL, beh_name, BVL_KEEPAUX))) return befig;
    }

    avt_errmsg(TMA_API_ERRMSG, "001", AVT_WARNING, beh_name);
//    fprintf (stderr, "Warning: Unable to get behavioral figure for  '%s'\n", beh_name);
    return NULL;
}

/**********************************************************/

TimingFigure *tma_abstract_old (TimingFigure *fig, BehavioralFigure *befig)
{
    ttvfig_list *blackbox;

    if (!fig) return NULL;
    
    tlfenv ();
    libenv ();
    tmaenv (ttv_getloadedfigtypes(fig) & (TTV_FILE_DTX|TTV_FILE_TTX|TTV_FILE_CTX));
    stmenv () ;

#ifdef AVERTEC_LICENSE
   if (avt_givetoken ("HITAS_LICENSE_SERVER", "tma")!=AVT_VALID_TOKEN)
      EXIT (1);
#endif

    tma_DetectClocksFromBeh (fig, befig);
    ttv_init_stm(fig);
    blackbox = tma_CreateBlackBox (fig, namealloc ("avt_bkbox"));
    //blackbox = tma_APIBlackBox (fig, namealloc ("avt_bkbox"));
    tma_AddInsertDelays (blackbox, fig);
    tma_UpdateSetReset (blackbox, befig);

    return blackbox;
}

TimingFigure *tma_DuplicateInterface (TimingFigure *fig, char *newname, chain_list *filter)
{
    char *figname, *blackboxname;
    ttvfig_list *blackbox;
    char buf[4096];

    if (!fig) return NULL;
    
    tlfenv ();
    libenv ();
    tmaenv (ttv_getloadedfigtypes(fig) & (TTV_FILE_DTX|TTV_FILE_TTX));
    stmenv () ;

#ifdef AVERTEC_LICENSE
   if (avt_givetoken ("HITAS_LICENSE_SERVER", "tma")!=AVT_VALID_TOKEN)
      EXIT (1);
#endif
    figname = fig->INFO->FIGNAME;
//    sprintf (buf, "%s_blackbox", figname);
    blackboxname = namealloc (newname);

    blackbox = tma_DupTtvFigHeader (blackboxname, fig);
    stm_addcell (blackboxname);
    tma_DupConnectorList (blackbox, fig, filter);

    return blackbox;
}
    
chain_list *tma_GetConnectorAxis(ttvfig_list *tvf, char *type, char *name)
{
  chain_list *cl, *res=NULL;
  float value;
  long slope, sloperef;
  ttvsig_list *tvs;
  tvs=ttv_getsig(tvf, mbk_vect(mbk_decodeanyvector(name), '[', ']'));
  if (tvs==NULL || ((tvs->TYPE & TTV_SIG_C)==0 && getptype(tvs->USER, TTV_SIG_CLOCK)==NULL))
  {
    res=addchain(res, NULL);
    *(float *)&res->DATA=-1;
  }
  else if (strcasecmp(type,"slope")==0)
  {
    res=ttv_getconnectoraxis(tvf, 1, namealloc(name));
    value=-1;
    if (getptype(tvs->USER, TTV_SIG_CLOCK)!=NULL && (tvs->TYPE & TTV_SIG_C)==0)
    {
       slope=ttv_getnodeslope(tvf,NULL,&tvs->NODE[0],&sloperef,TTV_FIND_MAX);
       value=slope*1e-12/TTV_UNIT;
    }

    for (cl=res; cl!=NULL; cl=cl->NEXT)
      if ((long)cl->DATA==TTV_NOTIME)
        *(float *)&cl->DATA=value;
      else
        *(float *)&cl->DATA=(((long)cl->DATA)/TTV_UNIT)*1e-12;
  }
  else if (strcasecmp(type,"capacitance")==0)
  {
    res=ttv_getconnectoraxis(tvf, 0, namealloc(name));
    for (cl=res; cl!=NULL; cl=cl->NEXT)
      if (*(float *)&cl->DATA>=0)
        *(float *)&cl->DATA=(*(float *)&cl->DATA)*1e-15;
  }
  return res;
}

void tma_SetMaxCapacitance(ttvfig_list *bbox, char *name, double value)
{
  chain_list *cl;
  ttvsig_list *tvs;
  ptype_list *pt;

  if ((cl=ttv_GetMatchingSignal(bbox, name, "connector"))==NULL)
     avt_errmsg(TMA_API_ERRMSG, "002", AVT_WARNING, name);
  
  while (cl!=NULL)
   {
     tvs=(ttvsig_list *)cl->DATA;
     if ((pt=getptype(tvs->USER, LIB_MAX_CAPACITANCE))==NULL)
       pt=tvs->USER=addptype(tvs->USER, LIB_MAX_CAPACITANCE, NULL);
     *(float *)&pt->DATA=value;
     cl=delchain(cl,cl);
   }
}

void tma_SetGeneratedClockInfo(ttvfig_list *bbox, char *name, char *string)
{
  chain_list *cl;
  ttvsig_list *tvs;
  ptype_list *pt;

  if ((cl=ttv_GetMatchingSignal(bbox, name, "connector latch precharge breakpoint"))==NULL)
     avt_errmsg(TMA_API_ERRMSG, "002", AVT_WARNING, name);
  
  while (cl!=NULL)
   {
     tvs=(ttvsig_list *)cl->DATA;
     if ((pt=getptype(tvs->USER, LIB_GENERATED_CLOCK_INFO))==NULL)
       pt=tvs->USER=addptype(tvs->USER, LIB_GENERATED_CLOCK_INFO, NULL);
     else
       mbkfree((char *)pt->DATA);
     pt->DATA=mbkstrdup(string);
     cl=delchain(cl,cl);
   }
}


void tma_TransfertSignalInformation(ttvsig_list *src, ttvsig_list *dst)
{
  float low, high;
  if (ttv_get_signal_swing(src->ROOT, src, &low, &high)==0)
    ttv_set_signal_swing(dst, low, high);    
}

void tma_SetEnergyInformation(ttvfig_list *bbox, char *name, char dir, char *modelname)
{
  chain_list *cl;
  ttvsig_list *tvs;
  ttvevent_list *tve;
  ptype_list *pt;
  long type;
  int num=0;

  if ((cl=ttv_GetMatchingSignal(bbox, name, "connector"))==NULL)
     avt_errmsg(TMA_API_ERRMSG, "002", AVT_WARNING, name);
  
  if (dir=='u') num=1;
  else if (dir=='d') num=0;
  else avt_errmsg(TMA_API_ERRMSG, "003", AVT_FATAL, dir);


  while (cl!=NULL)
   {
     tvs=(ttvsig_list *)cl->DATA;
     tve=&tvs->NODE[num];
     if ((pt=getptype(tve->USER, LIB_CONNECTOR_ENERGY_MODEL_NAME))==NULL)
       pt=tve->USER=addptype(tve->USER, LIB_CONNECTOR_ENERGY_MODEL_NAME, NULL);
     pt->DATA=namealloc(modelname);
     cl=delchain(cl,cl);
   }
}

