
#include BCK_H
#include STM_H
#include TTV_H
#include MLU_H
#include MUT_H
#include INF_H
#include EFG_H
#include TAS_H
#include TRC_H
#include YAG_H
#include MCC_H
#include INF_H

#define API_USE_REAL_TYPES
#include "ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "ttv_API_util.h"

chain_list *ttv_GetLines(ttvfig_list *tvf, char *start, char *end, char *dir, char *linetype)
{
  
  long type;
  chain_list *startlist;
  chain_list *endlist, *cl, *outlist=NULL;
  ttvline_list *line;
  int a, b, idir, data=0, i;
  long val=0;
  char *c, *tok;
  char buf[1024];
  ttvsig_list *tvs;

  if (tvf==NULL) return NULL;
  
  ttv_disablecache(tvf); // desactivation du cache
  
  ttv_DirectionStringToIndices(dir, &a, &b);
  if (a == -1 || b == -1) {
    avt_errmsg(TTV_API_ERRMSG, "018", AVT_ERROR, dir);
/*    avt_error("ttvapi", 4, AVT_ERR, "invalid direction '%s', function call ignored\n", dir);
    avt_error("ttvapi", 4, AVT_INFO, "a direction should look like \"uu\" (start signal rising, end signal rising) or \"ud\" (start signal rising, end signal falling) for instance.\n");*/
    return NULL;
  }

  strcpy(buf, linetype);
  tok=strtok_r(buf, " ", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"access")==0) val|=TTV_LINE_A;
      else if (strcasecmp(tok,"setup")==0) val|=TTV_LINE_U;
      else if (strcasecmp(tok,"hold")==0) val|=TTV_LINE_O;
      else if (strcasecmp(tok,"data")==0) data=1;
      else if (strcasecmp(tok,"hz")==0) val|=TTV_LINE_HZ;
      else if (strcasecmp(tok,"rc")==0) val|=TTV_LINE_RC;
      else if (strcasecmp(tok,"prech")==0) val|=TTV_LINE_PR;
      else if (strcasecmp(tok,"eval")==0) val|=TTV_LINE_EV;
      else if (strcasecmp(tok,"all")==0) val=-1;
      else //avt_error("ttvapi", -1, AVT_ERR, "bad value '%s' in 'ttv_GetLines'\n",tok);
        avt_errmsg(TTV_API_ERRMSG, "028", AVT_ERROR, tok);

      tok=strtok_r(NULL, " ", &c);
    }
  
  val&=~TTV_LINE_CONT;
  if (val==0 && !data) val=-1;

  if (a == TTVAPI_UP) a=1;
  else if (a == TTVAPI_DN) a=0;
  else a=2;
  if (b == TTVAPI_UP) b=1;
  else if (b == TTVAPI_DN) b=0;
  else b=2;

  if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
    type = TTV_FIND_LINE;
  else
    type = TTV_FIND_PATH;
  
  cl=addchain(NULL, namealloc(start));
  startlist=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,cl) ;
  freechain(cl);
  for (cl=startlist; cl!=NULL; cl=cl->NEXT)
    ((ttvsig_list *)cl->DATA)->TYPE|=TTV_SIG_MARQUE;

  cl=addchain(NULL, namealloc(end));
  endlist=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,cl) ;
  freechain(cl);

  for (cl=endlist; cl!=NULL; cl=cl->NEXT)
    {
      tvs=(ttvsig_list *)cl->DATA;
      if((type & TTV_FIND_PATH) ==  TTV_FIND_PATH)
        {
          ttv_expfigsig(tvf,tvs,tvs->ROOT->INFO->LEVEL,tvf->INFO->LEVEL,
                        TTV_STS_CL_PJT, TTV_FILE_TTX);
        }
      else
        {
          ttv_expfigsig(tvf,tvs,tvs->ROOT->INFO->LEVEL,tvf->INFO->LEVEL,
                        TTV_STS_CLS_FED, TTV_FILE_DTX);
        }

      for (i=0; i<2; i++)
        {
          if (b==2 || i==b)
            {
              if ((type & TTV_FIND_PATH) ==  TTV_FIND_PATH)
                line=tvs->NODE[i].INPATH;
              else
                line=tvs->NODE[i].INLINE;


              while (line!=NULL)
                {
                  if(!((((line->TYPE & (TTV_LINE_D | TTV_LINE_T)) != 0) &&
                        (line->FIG != tvf)) ||
                       (((line->TYPE & (TTV_LINE_P | TTV_LINE_F)) != 0) &&
                        (line->FIG->INFO->LEVEL < tvs->ROOT->INFO->LEVEL))))
                    {
                      idir=(line->NODE->TYPE & TTV_NODE_UP)!=0?1:0;
                      if ((line->NODE->ROOT->TYPE & TTV_SIG_MARQUE)!=0
                          && (a==2 || idir==a)
                          && (val==-1 ||
                              (line->TYPE & val)!=0 ||
                              (data && (line->TYPE & TTV_LINE_CONT)==0))
                          )
                        outlist=addchain(outlist, line);
                    }
                  line=line->NEXT;
                }
            }
        }
    }
  
  for (cl=startlist; cl!=NULL; cl=cl->NEXT)
    ((ttvsig_list *)cl->DATA)->TYPE&=~TTV_SIG_MARQUE;
  
  freechain(startlist);
  freechain(endlist);
  return outlist;
}

double ttv_GetTimingLineFloatProperty(ttvline_list *tl, char *prop, char *sub_prop)
{
  long val=TTV_NOTIME;
  int fail=0;

  if (((tl->TYPE & TTV_LINE_U)==TTV_LINE_U
       || (tl->TYPE & TTV_LINE_O)==TTV_LINE_O) && strcasecmp(sub_prop,"min")==0)
    return ttv_GetTimingLineFloatProperty(tl, prop, "max");

  if (strcasecmp(prop,"delay")==0)
    {
      if (strcasecmp(sub_prop,"min")==0) val=ttv_getdelaymin(tl);
      else if (strcasecmp(sub_prop,"max")==0) val=ttv_getdelaymax(tl);
      else fail=1;
    }
  else if (strcasecmp(prop,"slope")==0)
    {
      if (strcasecmp(sub_prop,"min")==0) val=ttv_getslopemin(tl);
      else if (strcasecmp(sub_prop,"max")==0) val=ttv_getslopemax(tl);
      else fail=1;
    }
  else if (strcasecmp(prop,"ref_delay")==0)
    {
      if (strcasecmp(sub_prop,"min")==0) val=tl->VALMIN;
      else if (strcasecmp(sub_prop,"max")==0) val=tl->VALMAX;
      else fail=1;
    }
  else if (strcasecmp(prop,"ref_slope")==0)
    {
      if (strcasecmp(sub_prop,"min")==0) val=tl->FMIN;
      else if (strcasecmp(sub_prop,"max")==0) val=tl->FMAX;
      else fail=1;
    }

  if (fail) //printf("unrecognized propoerty '%s':'%s'\n", prop, sub_prop);
    avt_errmsg(TTV_API_ERRMSG, "029", AVT_ERROR, prop, sub_prop);
  if (val==TTV_NOTIME) return -1;
  return _LONG_TO_DOUBLE(val);
}

ttvevent_list *ttv_GetTimingLineEventProperty(ttvline_list *tl, char *prop)
{
  long val=TTV_NOTIME;
  int fail=0;
  if (strcasecmp(prop,"start")==0) return tl->NODE;
  else if (strcasecmp(prop,"end")==0) return tl->ROOT;
  else fail=1;

  if (fail)// printf("unrecognized property '%s'\n", prop);
     avt_errmsg(TTV_API_ERRMSG, "030", AVT_ERROR, prop);
  return NULL;
}

char *ttv_GetTimingLineStringProperty(ttvline_list *tl, char *prop)
{
  long val=TTV_NOTIME;
  int fail=0;
  if (strcasecmp(prop,"type")==0)
    {
      if ((tl->TYPE & TTV_LINE_A)==TTV_LINE_A) return "access";
      if ((tl->TYPE & TTV_LINE_U)==TTV_LINE_U) return "setup";
      if ((tl->TYPE & TTV_LINE_O)==TTV_LINE_O) return "hold";
      if ((tl->TYPE & TTV_LINE_HZ)==TTV_LINE_HZ) return "hz";
      if ((tl->TYPE & TTV_LINE_RC)==TTV_LINE_RC) return "rc";
      if ((tl->TYPE & TTV_LINE_PR)==TTV_LINE_PR) return "prech";
      if ((tl->TYPE & TTV_LINE_EV)==TTV_LINE_EV) return "eval";
      return "data";
    }
  else fail=1;

  if (fail) //printf("unrecognized property '%s'\n", prop);
     avt_errmsg(TTV_API_ERRMSG, "030", AVT_ERROR, prop);
  return "?";
}

static inline char *tostring(double val, char *buf)
{
  sprintf(buf, "%g", val);
  return strdup(buf);
}

Property *ttv_GetTimingLineProperty (TimingLine *tl, char *code)
{
    char buf[256];

    if (!tl) {
        return addptype (NULL, TYPE_CHAR, strdup ("error_null_timing_line"));
    }

    if (!strcasecmp (code, "TYPE")) {
        return addptype (NULL, TYPE_CHAR, strdup (ttv_GetTimingLineStringProperty(tl, "type")));
    }

    if (!strcasecmp (code, "START_EVENT"))
      return addptype (NULL, TYPE_TIMING_EVENT, ttv_GetTimingLineEventProperty(tl, "start"));

    if (!strcasecmp (code, "END_EVENT"))
      return addptype (NULL, TYPE_TIMING_EVENT, ttv_GetTimingLineEventProperty(tl, "end"));

    if (!strcasecmp (code, "DELAY_MAX"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "delay", "max"),buf));

    if (!strcasecmp (code, "REF_DELAY_MAX"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "ref_delay", "max"),buf));

    if (!strcasecmp (code, "SLOPE_MAX"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "slope", "max"),buf));

    if (!strcasecmp (code, "REF_SLOPE_MAX"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "ref_slope", "max"),buf));

    if (!strcasecmp (code, "DELAY_MIN"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "delay", "min"),buf));

    if (!strcasecmp (code, "REF_DELAY_MIN"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "ref_delay", "min"),buf));

    if (!strcasecmp (code, "SLOPE_MIN"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "slope", "min"),buf));

    if (!strcasecmp (code, "REF_SLOPE_MIN"))
      return addptype (NULL, TYPE_CHAR, tostring(ttv_GetTimingLineFloatProperty(tl, "ref_slope", "min"),buf));

    if (!strcasecmp (code, "COMMAND"))
      return addptype (NULL, TYPE_TIMING_EVENT, ttv_getlinecmd(ttv_GetTopTimingFigure(tl->FIG), tl, TTV_LINE_CMDMAX));

     avt_errmsg(TTV_API_ERRMSG, "030", AVT_ERROR, code);
    //fprintf (stderr, "error: unknown property %s\n", code);
    return NULL;
}

double ttv_ComputeLineDelay(ttvline_list *tl, double slope_in, double output_capa, char *delayslope, char *maxmin)
{
  float slope, val;
  char *model=NULL;
  int err=0;

  API_TEST_TOKEN_SUB(TMA_API,"tma")

  if (tl==NULL) return -1;

  if (strcasecmp(delayslope,"delay")==0)
    {
      if (strcasecmp(maxmin,"max")==0) model=tl->MDMAX;
      else if (strcasecmp(maxmin,"min")==0) model = tl->MDMIN ;
      else err=1;
    }
  else if (strcasecmp(delayslope,"slope")==0)
    {
      if (strcasecmp(maxmin,"max")==0) model=tl->MFMAX;
      else if (strcasecmp(maxmin,"min")==0) model = tl->MFMIN ;
      else err=1;
    }
  else err=2;
  
  if (!err)
    {
      if (model!=NULL)
        {
          if (strcasecmp(delayslope,"delay")==0)
            val = stm_getdelay (tl->FIG->INFO->FIGNAME, model, output_capa * 1e15, slope_in*1e12, NULL,tl->ROOT->ROOT->NAME);
          else
            val = stm_getslew (tl->FIG->INFO->FIGNAME, model, output_capa * 1e15, slope_in*1e12, NULL, NULL,tl->ROOT->ROOT->NAME);
          return val*1e-12;
        }
      else
        {
          if (strcasecmp(delayslope,"delay")==0)
            {
              if (strcasecmp(maxmin,"max")==0) val=_LONG_TO_DOUBLE(tl->VALMAX);
              else val=_LONG_TO_DOUBLE(tl->VALMIN);
            }
          else if (strcasecmp(delayslope,"slope")==0)
            {
              if (strcasecmp(maxmin,"max")==0) val=_LONG_TO_DOUBLE(tl->FMAX);
              else val = _LONG_TO_DOUBLE(tl->FMIN) ;
            }
          return val;
        }
    }
  else
    {
     avt_errmsg(TTV_API_ERRMSG, "031", AVT_ERROR, err==2?delayslope:maxmin);
//      fprintf (stderr, "error: unknown parameter value '%s'\n", err==2?delayslope:maxmin);
    }
  return -1;
}

void ttv_SetTimingLineDelay(ttvline_list *tl, char *prop, char *sub_prop, double value)
{
  long val=mbk_long_round(value*1e12*TTV_UNIT);
  int fail=1;

  if (strcasecmp(prop,"delay")==0)
    {
      if (strcasecmp(sub_prop,"min")==0 || strcasecmp(sub_prop,"all")==0)
        {
          tl->VALMIN=val;
          tl->MDMIN=NULL;
          fail=0;
        }
      if (strcasecmp(sub_prop,"max")==0 || strcasecmp(sub_prop,"all")==0)
        {
          tl->VALMAX=val;
          tl->MDMAX=NULL;
          fail=0;
        }
    }
  else if (strcasecmp(prop,"slope")==0)
    {
      if (val<=0)
        {
          avt_errmsg(TTV_API_ERRMSG, "032", AVT_ERROR, value);
//          printf("ttv_SetTimingLineDelay: negative slope given (%g), set to 1e-12\n", value);
          val=mbk_long_round(TTV_UNIT);
        }
      
      if (strcasecmp(sub_prop,"min")==0 || strcasecmp(sub_prop,"all")==0)
        {
          tl->FMIN=val;
          tl->MFMIN=NULL;
          fail=0;
        }
      if (strcasecmp(sub_prop,"max")==0 || strcasecmp(sub_prop,"all")==0)
        {
          tl->FMAX=val;
          tl->MFMAX=NULL;
          fail=0;
        }
    }

  if (fail) //printf("unrecognized property '%s':'%s'\n", prop, sub_prop);
    avt_errmsg(TTV_API_ERRMSG, "029", AVT_ERROR, prop, sub_prop);
}
