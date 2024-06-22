
#include "ttv.h"

#define TTV_PROPAGATE_IF_DIFFERENT 1
//#define TTV_PROPAGATE_IF_DIFFERENT 2
#define SAMESLOPELIMIT 0.01

long TTV_PROPAGATE_FORCED_SLOPE=TTV_NOTIME;
float TTV_PROPAGATE_FORCED_CAPA=-1;
static int TTV_STOP_AFTER_GATE_1=0, useheap=0;

typedef struct propagate_cache
{
  struct propagate_cache *next;
  long slope;
  float capa;
  long res_slope, res_delay;
} propagate_cache;

static HeapAlloc propagate_heap;

static inline int ttv_get_propagate_cache_delay_slope(ttvline_list *tl, long slope, float capa, long *res_slope, long *res_delay)
{
  ptype_list *pt;
  propagate_cache *pc;
  if (useheap)
  {
    if ((pt=getptype(tl->USER, TTV_NODE_PROPINFO_CACHE))==NULL) return 0;
    pc=(propagate_cache *)pt->DATA;
    while (pc!=NULL && (pc->slope!=slope || pc->capa!=capa)) pc=pc->next;
    if (pc!=NULL)
    {
      *res_slope=pc->res_slope;
      *res_delay=pc->res_delay;
      return 1;
    }
  }
  return 0;
}

static inline void ttv_add_propagate_cache_delay_slope(ttvline_list *tl, long slope, float capa, long res_slope, long res_delay)
{
  propagate_cache *npc;
  if (useheap)
  {
   ptype_list *pt;
   if ((pt=getptype(tl->USER, TTV_NODE_PROPINFO_CACHE))==NULL)
       pt=tl->USER=addptype(tl->USER, TTV_NODE_PROPINFO_CACHE, NULL);

    npc=(propagate_cache *)AddHeapItem(&propagate_heap);
    npc->slope=slope;
    npc->capa=capa;
    npc->res_slope=res_slope;
    npc->res_delay=res_delay;
    npc->next=(propagate_cache *)pt->DATA;
    pt->DATA=npc;
  }
}


void ttv_experim_setstopaftergate1(int val)
{
  TTV_STOP_AFTER_GATE_1=val;
}

ttvline_list *ttv_CreateContactLine(ttvevent_list *node, ttvevent_list *root)
{
  ttvline_list *tvl;
  tvl=(ttvline_list *)mbkalloc(sizeof(ttvline_list));
  tvl->USER=NULL;
  tvl->NODE=node;
  tvl->ROOT=root;
  tvl->FIG=NULL;
  return tvl;
}

static void initprop(ttvpropinfo_list *prop, long slope)
{
  prop->CTK.PWL=NULL;
  prop->CTK.SLEW=slope;
  prop->CTK.DELAY=0;
  prop->CTK.R=-1;
  if (prop->REF!=NULL)
    {
      prop->REF->PWL=NULL;
      prop->REF->SLEW=slope;
      prop->REF->DELAY=0;
      prop->REF->R=-1;
    }
}

static ttvpropinfo_list *createpropinfo(ttvevent_list *a, int usectk)
{
  ttvpropinfo_list *prop;
  prop=(ttvpropinfo_list *)mbkalloc(sizeof(ttvpropinfo_list)); //<= par heap
  prop->CTK.PWL=NULL;
  prop->CTK.SLEW=TTV_NOSLOPE;
  prop->CTK.DELAY=TTV_NOTIME;
  prop->CTK.R=-1;
  prop->CTK.C1=-1;
  prop->CTK.C2=-1;
  prop->CTK.RDRIVER=-1;
  prop->CTK.CAPA=-1;
  if (usectk)
    {
      prop->REF=(ttv_onepropinfo *)mbkalloc(sizeof(ttv_onepropinfo));
      prop->REF->PWL=NULL;
      prop->REF->SLEW=TTV_NOSLOPE;
      prop->REF->DELAY=TTV_NOTIME;
      prop->REF->R=-1;
      prop->REF->C1=-1;
      prop->REF->C2=-1;
      prop->REF->RDRIVER=-1;
      prop->REF->CAPA=-1;
    }
  else
    prop->REF=NULL;
  a->USER=addptype(a->USER, TTV_NODE_PROPINFO, prop);
  return prop;
}

ttvpropinfo_list *ttv_get_prop_info(ttvevent_list *node)
{
  ptype_list *pt;
  if ((pt=getptype(node->USER, TTV_NODE_PROPINFO))!=NULL)
    return (ttvpropinfo_list *)pt->DATA;
  return NULL;
}

ttvpropinfo_list *givepropinfo(ttvevent_list *a, int usectk)
{
  ttvpropinfo_list *prop;
  
  if ((prop=ttv_get_prop_info(a))!=NULL)
    return prop;

  return createpropinfo(a, usectk);
}

static void cleanpropinfo(ttvevent_list *a)
{
  ptype_list *pt;
  ttvpropinfo_list *prop;

  if ((pt=getptype(a->USER, TTV_NODE_PROPINFO))!=NULL)
    {
      prop=(ttvpropinfo_list *)pt->DATA;
      if (prop->REF!=NULL)
        {
          mbkfree(prop->REF);
        }
      mbkfree(prop);
      a->USER=delptype(a->USER, TTV_NODE_PROPINFO);
    }
}

static int ttv_sameslope(long a, long b)
{
  long diff;
  if (a==TTV_NOSLOPE || b==TTV_NOSLOPE) return 0;
  if (a!=b) return 0;
/*  diff=abs(a-b);
  if (((float)diff)/b>SAMESLOPELIMIT || ((float)diff)/a>SAMESLOPELIMIT) return 0;
  */
  return 1;
}

void  ttv_RecomputeLineDelayAndSlope(ttvfig_list *tvf, ttvline_list *line, ttvpropinfo_list *input0, ttvpropinfo_list *output0, float capa, long type, int doref)
{
  timing_model *modelmin, *modelmax ;
  float vtmin=0, vtmax=0 ;
  float vfmin=0, vfmax=0 ;
  float vfrcmin=0, vfrcmax=0 ;
  float vddmin=0, vddmax=0 ;
  float vthmin=0, vthmax=0 ;
  long *dmin1 = NULL, *fmin1 = NULL, *dmax1 = NULL, *fmax1 = NULL;
  stm_pwl **fmin1_pwl = NULL, **fmax1_pwl = NULL;
  double *rmin=NULL, *c1min=NULL, *c2min=NULL, vsatmin=-1, rlinmin=-1 ;
  double *rmax=NULL, *c1max=NULL, *c2max=NULL, vsatmax=-1, rlinmax=-1 ;
  double r=0, c1=0, c2=0;
  lofig_list *lofig ;
  locon_list *locon ;
  stm_pwl *pwlmin=NULL, *pwlmax=NULL;
  long smin=0, smax=0;
  ttvline_list *previous_line=input0->PREVIOUS_LINE;
  ttv_onepropinfo *input, *output;
  float *nrjmin = NULL,*nrjmax = NULL;
  

  if (doref)
    {
      input=input0->REF, output=output0->REF;
    }
  else
    {
      input=&input0->CTK, output=&output0->CTK;
    }

  if (type & TTV_FIND_MIN)
    {
      pwlmin=input->PWL;
      smin=input->SLEW;
      dmin1=&output->DELAY;
      fmin1=&output->SLEW;
      if (0/*V_BOOL_TAB[__STM_USE_MSC].VALUE*/)
        fmin1_pwl=&output->PWL;
      else
        fmin1_pwl=NULL;
      rmin=&r;
      c1min=&c1;
      c2min=&c2;
      *rmin = input->RDRIVER;
      *c1min = 1000.0*input->CAPA;
      nrjmin = &output->ENERGY;
    }
  else
    {
      pwlmax=input->PWL;
      smax=input->SLEW;
      dmax1=&output->DELAY;
      fmax1=&output->SLEW;
      if (0/*V_BOOL_TAB[__STM_USE_MSC].VALUE*/)
        fmax1_pwl=&output->PWL;
      else
        fmax1_pwl=NULL;
      rmax=&r;
      c1max=&c1;
      c2max=&c2;
      *rmax = input->RDRIVER;
      *c1max = 1000.0*input->CAPA;
      nrjmax = &output->ENERGY;
    }
  
  modelmax = NULL;
  modelmin = NULL;
            
  if((line->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
    {
      if (type & TTV_FIND_MIN)
        {
          if (previous_line) 
            modelmin = stm_getmodel(previous_line->FIG->INFO->FIGNAME,previous_line->MDMIN) ;
          vtmin = stm_mod_vt(modelmin);
          vfmin = stm_mod_vf(modelmin);
          vthmin = stm_mod_vth(modelmin);
          vddmin = stm_mod_vdd(modelmin);
          if (input->R>=0)
            {
              vsatmin = stm_mod_vsat(modelmin);
              rlinmin = stm_mod_rlin(modelmin);
            }
        }
      else
        {
          if (previous_line)
            modelmax = stm_getmodel(previous_line->FIG->INFO->FIGNAME,previous_line->MDMAX) ;
          vtmax = stm_mod_vt(modelmax);
          vfmax = stm_mod_vf(modelmax);
          vthmax = stm_mod_vth(modelmax);
          vddmax = stm_mod_vdd(modelmax);
          if (input->R>=0)
            {
              vsatmax = stm_mod_vsat(modelmax);
              rlinmax = stm_mod_rlin(modelmax);
            }
        }
      if (modelmax==NULL && modelmin==NULL && (line->NODE->ROOT->TYPE & TTV_SIG_C)!=0)
      {
        float low, high;
        ttv_get_signal_swing(tvf, line->NODE->ROOT, &low, &high);
        vddmax=high-low;
        vthmax=vddmax/2;
        vfmax=vddmax;
        vtmax=0.2*vddmax;
      }
      
      r=input->R; c1=input->C1; c2=input->C2;
      
      if((line->ROOT->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
        {
          vfrcmin = vddmin;
          vfrcmax = vddmax;
        }
      else
        {
          vfrcmin = 0.0;
          vfrcmax = 0.0;
        }
     
      
      ttv_calclinedelayslope(line,smin,smax,pwlmin,pwlmax,vtmin,vtmax,vfrcmin,vfrcmax,vddmin,vddmax,vthmin,vthmax,capa,dmin1,fmin1,fmin1_pwl,dmax1,fmax1,fmax1_pwl,0,vsatmin,rlinmin,rmin,c1min,c2min,vsatmax,rlinmax,rmax,c1max,c2max,NULL,NULL);

      output->R=-1;
      output->RDRIVER  = input->RDRIVER;
      output->CAPA     = input->CAPA;
      output->ENERGY   = 0.0;
  }
 else
  {
   if (type & TTV_FIND_MIN)
     {
       modelmin = stm_getmodel(line->FIG->INFO->FIGNAME,line->MDMIN) ;
       vtmin = stm_mod_vt(modelmin);
       vfmin = stm_mod_vf(modelmin);
       vthmin = stm_mod_vth(modelmin);
       vddmin = stm_mod_vdd(modelmin);
     }
   else
     {
       modelmax = stm_getmodel(line->FIG->INFO->FIGNAME,line->MDMAX) ;
       vtmax = stm_mod_vt(modelmax);
       vfmax = stm_mod_vf(modelmax);
       vthmax = stm_mod_vth(modelmax);
       vddmax = stm_mod_vdd(modelmax);
     }

   lofig = ttv_getrcxlofig( line->ROOT->ROOT->ROOT ) ;
   if( lofig ) 
     {
       locon = rcx_gethtrcxcon( NULL, lofig, line->ROOT->ROOT->NAME ) ;
       if( locon ) 
         {
           rcx_rcnload_reset( locon->SIG );
         }
     }

   ttv_calclinedelayslope(line,smin,smax,pwlmin,pwlmax,vtmin,vtmax,vfrcmin,vfrcmax,vddmin,vddmax,vthmin,vthmax,capa,dmin1,fmin1,fmin1_pwl,dmax1,fmax1,fmax1_pwl,0,vsatmin,rlinmin,rmin,c1min,c2min,vsatmax,rlinmax,rmax,c1max,c2max,nrjmin,nrjmax);

   output->R=r;
   output->C1=c1;
   output->C2=c2;
   if (type & TTV_FIND_MIN)
     {
       modelmin = stm_getmodel(line->FIG->INFO->FIGNAME,line->MDMIN) ;
       output->RDRIVER = stm_mod_rlin( modelmin );
     }
   else
     {
       modelmax = stm_getmodel(line->FIG->INFO->FIGNAME,line->MDMAX) ;
       output->RDRIVER = stm_mod_rlin( modelmax );
     }
  }

  if( r > 0.0 && c2 > 0.0 )
      output->CAPA = c1+c2 ;
  else
      output->CAPA = c1 ;
}

static int mustpropagate_tmalike(ttvline_list *prevline, chain_list *cur, chain_list *start)
{
  ttvline_list *curl, *nextl;
  if (cur==start || (cur==start->NEXT && (prevline->TYPE & TTV_LINE_RC) == TTV_LINE_RC)) return 1;
  curl=(ttvline_list *)cur->DATA;
  if (cur->NEXT==NULL && (curl->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) return 1;
  if (cur->NEXT!=NULL && cur->NEXT->NEXT==NULL)
    {
      nextl=(ttvline_list *)cur->NEXT->DATA;
      if ((nextl->TYPE & TTV_LINE_RC) == TTV_LINE_RC && (nextl->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0) return 1;
    }
  return 0;
}

static inline int ttvprop_islastgaterc(chain_list *cl)
{
  ttvline_list *curl, *nextl;
  if (cl->NEXT==NULL) return 1;
  if (cl->NEXT->NEXT!=NULL) return 0;
  nextl=(ttvline_list *)cl->NEXT->DATA;
  if ((nextl->TYPE & TTV_LINE_RC) != TTV_LINE_RC) return 0;
  return 1;
}

long ttv_RecomputeLineChain(ttvfig_list *tvf, ttvfig_list *tvi, chain_list *ordered_line_list, long inputSLEW, long *outputSLEW, float *energy, float capa, long type, long mode, int doref)
{
  chain_list *cl;
  ttvline_list *tl;
  ttvpropinfo_list *res;
  ttvevent_list *startnode;
  float thiscapa;
  ttvpropinfo_list *input=NULL;
  long totaldelay=0, totaldelayref=0, outdelay, oldslope, justlast=0, val;
  ttvline_list *prevline;
  stbfig_list *sf=NULL;
  ctk_exchange ctkinfo;
  int lastmode, trig=0;

  if ((type & TTV_FIND_ACCESS)==0) trig=1;
  
  *energy = 0.0;

  if (doref) sf=stb_getstbfig(tvf);

  if (ordered_line_list==NULL) { *outputSLEW=0; return 0; }

  if ((mode & TTV_PROPAGATE_IF_DIFFERENT)!=0) doref=0;

  prevline=NULL;
  for (cl=ordered_line_list; cl!=NULL; cl=cl->NEXT)
    {
      tl=(ttvline_list *)cl->DATA;
      if (input==NULL) 
        {
          input=givepropinfo(tl->NODE, doref);
          if ((mode & TTV_PROPAGATE_IF_DIFFERENT)!=0 && ttv_sameslope(input->CTK.SLEW, inputSLEW)) justlast=1;
          initprop(input, inputSLEW);
        }

      if (prevline!=NULL && prevline->FIG==NULL) prevline=NULL; // contact line
      input->PREVIOUS_LINE=prevline;

      res=givepropinfo(tl->ROOT, doref); //?

      if (tl->FIG!=NULL)
        {
          if (cl!=ordered_line_list && prevline==NULL) // pas d'access
            {
              ttv_getnodeslope(tvf,tvi,tl->ROOT,&oldslope,type);
              initprop(input, oldslope);
            }
          if((tl->ROOT->ROOT->TYPE & (TTV_SIG_N|TTV_SIG_C)) != 0
             && (cl->NEXT==NULL || (cl->NEXT->NEXT==NULL && (((ttvline_list *)cl->NEXT->DATA)->TYPE & TTV_LINE_RC)!=0))
                          )
          {
            if (capa>=0)
              thiscapa=capa;
            else
              thiscapa=ttv_get_signal_output_capacitance(tvf, tl->ROOT->ROOT);
          }
          else
            thiscapa=0;
          
          oldslope=res->CTK.SLEW;

          if (justlast==0 || (justlast==1 && ttvprop_islastgaterc(cl)))
          {
            if (doref && sf!=NULL)
             {
               lastmode=rcx_crosstalkactive(RCX_MILLER); //?
               if((tl->TYPE & TTV_LINE_RC) == TTV_LINE_RC)
                 stb_set_ctk_information(sf, tl->NODE, type, &ctkinfo);
               else
                 stb_set_ctk_information(sf, tl->ROOT, type, &ctkinfo);
             }
            if (TTV_STOP_AFTER_GATE_1 && !mustpropagate_tmalike(prevline, cl, ordered_line_list))
            {
               if ((type & TTV_FIND_MIN)!=0)
                 res->CTK.DELAY=tl->VALMIN, res->CTK.SLEW=tl->FMIN;
               else
                 res->CTK.DELAY=tl->VALMAX, res->CTK.SLEW=tl->FMAX;
            }
            else
            {
               if (ttv_get_propagate_cache_delay_slope(tl, input->CTK.SLEW, thiscapa, &res->CTK.SLEW, &res->CTK.DELAY)==0)
               {                       
                 ttv_RecomputeLineDelayAndSlope(tvf, tl, input, res, thiscapa, type, 0);
                 ttv_add_propagate_cache_delay_slope(tl, input->CTK.SLEW, thiscapa, res->CTK.SLEW, res->CTK.DELAY);
               }
            }
            if (doref && sf!=NULL)
             {
               stb_release_ctk_information(&ctkinfo);
               rcx_crosstalkactive(lastmode); // ?
             }
          }
          if (doref)
          {
            lastmode=rcx_crosstalkactive(RCX_NOCROSSTALK);
            if (TTV_STOP_AFTER_GATE_1 && !mustpropagate_tmalike(prevline, cl, ordered_line_list))
            {
               if ((type & TTV_FIND_MIN)!=0)
                 res->REF->DELAY=tl->VALMIN, res->REF->SLEW=tl->FMIN;
               else
                 res->REF->DELAY=tl->VALMAX, res->REF->SLEW=tl->FMAX;
            }
            else ttv_RecomputeLineDelayAndSlope(tvf, tl, input, res, thiscapa, type, 1);
            rcx_crosstalkactive(lastmode);
          }
          
          if ((mode & TTV_PROPAGATE_IF_DIFFERENT)!=0 && ttv_sameslope(oldslope, res->CTK.SLEW)) justlast=1;

          if ((tl->TYPE & TTV_LINE_A) != 0)
          {
            if ((val=stb_gettruncatedaccess(tl->ROOT, tl->NODE, (type & TTV_FIND_MAX)==TTV_FIND_MAX?1:0))!=TTV_NOTIME)
            {
                if (res->CTK.DELAY>val) res->CTK.DELAY=val;
                if (doref && res->REF->DELAY>val) res->REF->DELAY=val;
            }
          }

          totaldelay+=res->CTK.DELAY;
          if (doref) totaldelayref+=res->REF->DELAY;
          if (trig==0 && (tl->ROOT->ROOT->TYPE & (TTV_SIG_L|TTV_SIG_R))!=0) trig=1;
          if (trig)
            *energy += res->CTK.ENERGY;
        }

      input=res;
      prevline=tl;
    }

  outdelay=totaldelay;

  if ((mode & TTV_PROPAGATE_IF_DIFFERENT)==0)
  {
    if (0/*V_BOOL_TAB[__STM_USE_MSC].VALUE*/)
      {
        // clean pwl 
        for (cl=ordered_line_list; cl!=NULL; cl=cl->NEXT)
          {
            tl=(ttvline_list *)cl->DATA;
            input=givepropinfo(tl->NODE, doref);
            if (input->CTK.PWL!=NULL) stm_pwl_destroy(input->CTK.PWL);
            if (doref && input->REF->PWL!=NULL) stm_pwl_destroy(input->REF->PWL);
          }
        if (ordered_line_list!=NULL && ((ttvline_list *)ordered_line_list->DATA)->NODE!=tl->ROOT)
          {
            input=givepropinfo(tl->ROOT, doref);
            if (input->CTK.PWL!=NULL) stm_pwl_destroy(input->CTK.PWL);
            if (doref && input->REF->PWL!=NULL) stm_pwl_destroy(input->REF->PWL);
          }
      }

    if ((type & TTV_FIND_DUAL)==TTV_FIND_DUAL)
      {
        totaldelay=0;
        totaldelayref=0;
        tl=(ttvline_list *)ordered_line_list->DATA;
        input=givepropinfo(tl->NODE, doref);
        for (cl=ordered_line_list; cl!=NULL; cl=cl->NEXT)
          {
            tl=(ttvline_list *)cl->DATA;
            input->CTK.FIND_DELAY=totaldelay;          
            res=givepropinfo(tl->ROOT, doref);
            totaldelay+=res->CTK.DELAY;
            if (doref)
              {
                input->REF->FIND_DELAY=totaldelayref;
                totaldelayref+=res->REF->DELAY;
              }
            input=res;
          }
        res->CTK.FIND_DELAY=totaldelay;
        if (doref) res->REF->FIND_DELAY=totaldelayref;
      }
    else
      {
        tl=(ttvline_list *)ordered_line_list->DATA;
        startnode=tl->NODE;
        input=givepropinfo(tl->NODE, doref);

        for (cl=ordered_line_list; cl!=NULL; cl=cl->NEXT)
          {
            tl=(ttvline_list *)cl->DATA;
            input->CTK.FIND_DELAY=totaldelay;
            res=givepropinfo(tl->ROOT, doref);
            totaldelay-=res->CTK.DELAY;
            if (doref)
              {
                input->REF->FIND_DELAY=totaldelayref;
                totaldelayref-=res->REF->DELAY;
              }
            input=res;
          }
        if (startnode!=tl->ROOT)
          {
            res->CTK.FIND_DELAY=totaldelay; // 0 ?
            if (doref) res->REF->FIND_DELAY=totaldelayref;
          }
      }
  }
  *outputSLEW=res->CTK.SLEW;
  return outdelay;
}

void ttv_RecomputeCleanLineChain(chain_list *ordered_line_list)
{
  chain_list *cl;
  ttvline_list *tl;

  for (cl=ordered_line_list; cl!=NULL; cl=cl->NEXT)
    {
      tl=(ttvline_list *)cl->DATA;
      cleanpropinfo(tl->NODE);
      if (useheap) tl->USER=testanddelptype(tl->USER, TTV_NODE_PROPINFO_CACHE);
    }
  if (ordered_line_list!=NULL) cleanpropinfo(tl->ROOT);
}

void ttv_RecomputeFreeLineChain(chain_list *ordered_line_list)
{
  chain_list *cl;
  ttvline_list *tl;
  for (cl=ordered_line_list; cl!=NULL; cl=cl->NEXT)
    {
      tl=(ttvline_list *)cl->DATA;
      if (tl->FIG==NULL) mbkfree(tl);
    }
  freechain(ordered_line_list);
}

chain_list *ttv_BuildLineList(ttvfig_list *ttvfig, ttvevent_list *root, ttvevent_list *node, long type, int *doref)
{
 ttvevent_list *nodex ;
 ptype_list *ptype ;
 chain_list *line_list;

 nodex = node ;
 *doref=0;
 line_list=NULL;

 if((type & TTV_FIND_DUAL) != TTV_FIND_DUAL)
  {
    if((nodex->FIND->OUTLINE == NULL) && 
       ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
     {
       line_list=addchain(line_list, ttv_CreateContactLine(nodex, (ttvevent_list *)ptype->DATA));
       nodex = (ttvevent_list *)ptype->DATA ;
     }

   while(nodex->FIND->OUTLINE != NULL)
    {
      line_list=addchain(line_list, nodex->FIND->OUTLINE);
      if (*doref==0 && ttv_getlinedelay(nodex->FIND->OUTLINE)!=NULL) *doref=1;
      nodex = nodex->FIND->OUTLINE->ROOT ;
      if((nodex->FIND->OUTLINE == NULL) && 
         ((ptype = getptype(nodex->USER,TTV_NODE_LATCH)) != NULL))
        {
          line_list=addchain(line_list, ttv_CreateContactLine(nodex, (ttvevent_list *)ptype->DATA));
          nodex = (ttvevent_list *)ptype->DATA ;
        }
      if(nodex == root) break ;
    }
   line_list=reverse(line_list);
  }
 else
  {
   while(nodex->FIND->OUTLINE != NULL)
     {
       if (*doref==0 && ttv_getlinedelay(nodex->FIND->OUTLINE)!=NULL) *doref=1;
       line_list=addchain(line_list, nodex->FIND->OUTLINE);
       nodex = nodex->FIND->OUTLINE->NODE ;
       if(nodex == root) break ;
     }
  }

 return(line_list) ;
}

chain_list *ttv_DoPropagation_PRECISION_LEVEL1(ttvfig_list *tvf, ttvfig_list *tvi, ttvevent_list *node, ttvevent_list *root, long type)
{ 
  chain_list *line_list;
  long inslope, res;
  int doref;
  float energy;

  if ((type & TTV_FIND_PATH)==TTV_FIND_PATH) return NULL;

  line_list=ttv_BuildLineList(tvf, root, node, type, &doref);
  
  if (TTV_PROPAGATE_FORCED_SLOPE!=TTV_NOTIME)
    inslope=TTV_PROPAGATE_FORCED_SLOPE;
  else
  {
   if ((type & TTV_FIND_DUAL)!=TTV_FIND_DUAL)
     ttv_getnodeslope(tvf, tvi, node, &inslope, type);
   else
     ttv_getnodeslope(tvf, tvi, root, &inslope, type);
  }

  ttv_RecomputeLineChain(tvf, tvi, line_list, inslope, &res, &energy, TTV_PROPAGATE_FORCED_CAPA, type, 0, doref);
#if 0
  {              
    ttvpropinfo_list *input;
    long oslop;
    char *n1, *n2;
    double rat;
//    if ((type & TTV_FIND_DUAL)!=TTV_FIND_DUAL)
    input=givepropinfo(node, doref);
/*    else
      input=givepropinfo(root, doref);*/
    
    if ((type & TTV_FIND_DUAL)!=TTV_FIND_DUAL)
      n1=node->ROOT->NAME, n2=root->ROOT->NAME;
    else
      n1=root->ROOT->NAME, n2=node->ROOT->NAME;
    line_list=reverse(line_list);
    if ((type & TTV_FIND_MIN)!=0) oslop=((ttvline_list *)line_list->DATA)->FMIN;
    else oslop=((ttvline_list *)line_list->DATA)->FMAX;
    rat=((float)(input->CTK.FIND_DELAY-node->FIND->DELAY))*100.0/node->FIND->DELAY;
    avt_fprintf(stderr,"%s%s %s is:%ld od:%ld os:%ld -> od:%ld os:%ld (%+.1f%%)¤.\n",fabs(rat)<5?"":fabs(rat)<10?"¤4":"¤6", n1, n2, inslope,node->FIND->DELAY,oslop,input->CTK.FIND_DELAY,res,rat);
    line_list=reverse(line_list);
  }
#endif  
  return line_list;
}

void ttv_CleanPropagation_PRECISION_LEVEL1(chain_list *line_list)
{
  ttv_RecomputeCleanLineChain(line_list);
  ttv_RecomputeFreeLineChain(line_list);
}

long ttv_GET_FINDDELAY(ttvevent_list *node, int ref)
{
  ttvpropinfo_list *propinfo=NULL;

  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE!=0)
    propinfo = ttv_get_prop_info(node);

  if(propinfo){
    if (ref && propinfo->REF!=NULL)
      return(propinfo->REF->FIND_DELAY);
    return(propinfo->CTK.FIND_DELAY);
  }
  return node->FIND->DELAY;
}

long ttv_GET_LINE_DELAY(ttvline_list *line, long type, int ref)
{
  ttvpropinfo_list *propinfo=NULL;
  long val, ret;

  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE!=0)
    propinfo = ttv_get_prop_info(line->ROOT);

  if(propinfo){
    if (ref && propinfo->REF!=NULL)
      ret=propinfo->REF->DELAY;
    else
      ret=propinfo->CTK.DELAY;
  }
  else {
    if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
      ret=line->VALMAX;
    else
      ret=line->VALMIN;
  }
  if ((line->TYPE & TTV_LINE_A) != 0)
  {
    if ((val=stb_gettruncatedaccess(line->ROOT, line->NODE, (type & TTV_FIND_MAX)==TTV_FIND_MAX?1:0))!=TTV_NOTIME && ret>val) ret=val;
  }
  return ret;
}

long ttv_GET_LINE_SLEW(ttvline_list *line, long type, int ref)
{
  ttvpropinfo_list *propinfo=NULL;

  if (V_INT_TAB[__TTV_PRECISION_LEVEL].VALUE!=0)
    propinfo= ttv_get_prop_info(line->ROOT);

  if(propinfo){
    if (ref && propinfo->REF!=NULL)
      return(propinfo->REF->SLEW);
    return(propinfo->CTK.SLEW);
  }
  if ((type & TTV_FIND_MAX) == TTV_FIND_MAX)
    return(line->FMAX);
  else
    return(line->FMIN);
}

void ttv_DoCharacPropagation(ttvfig_list *tvf, ttvfig_list *tvi, ttvevent_list *node, ttvevent_list *root, long type, chain_list *slope_list, chain_list *capa_list, chain_list **resd, chain_list **ress, chain_list **rese, chain_list *inputlinelist)
{ 
  chain_list *line_list;
  long inslope, res, delay, resen;
  int doref=0;
  chain_list *slopes, *capas;
  float capa;

  *resd=NULL;
  *ress=NULL;
  *rese=NULL;
  if (inputlinelist) 
    line_list=inputlinelist;
  else
    line_list=ttv_BuildLineList(tvf, root, node, type, &doref);

  useheap=1;
  CreateHeap(sizeof(propagate_cache), 1000, &propagate_heap);
  
  for (slopes=slope_list; slopes!=NULL; slopes=slopes->NEXT)
  {
    inslope=(long)slopes->DATA;
    if (inslope==TTV_NOTIME)
    {
     if ((type & TTV_FIND_DUAL)!=TTV_FIND_DUAL)
       ttv_getnodeslope(tvf, tvi, node, &inslope, type);
     else
       ttv_getnodeslope(tvf, tvi, root, &inslope, type);
    }

    for (capas=capa_list; capas!=NULL; capas=capas->NEXT)
    {
      capa=*(float *)&capas->DATA;
      delay=ttv_RecomputeLineChain(tvf, tvi, line_list, inslope, &res, (float*)&resen, capa, type, TTV_PROPAGATE_IF_DIFFERENT, doref);
      *resd=addchain(*resd, (void *)delay);
      *ress=addchain(*ress, (void *)res);
      if(( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 1 )
       ||( V_INT_TAB[ __AVT_POWER_CALCULATION ].VALUE == 5 )){
          *rese=addchain(*rese, (void *)resen);
      }
    }
  }
  
  DeleteHeap(&propagate_heap);
  ttv_RecomputeCleanLineChain(line_list);
  ttv_RecomputeFreeLineChain(line_list);
  *resd=reverse(*resd);
  *ress=reverse(*ress);
  *rese=reverse(*rese);
  useheap=0;
}
