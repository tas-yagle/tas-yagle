#define API_USE_REAL_TYPES

#include GEN_H
#include SIM_H
#include TRC_H
#include STM_H
#include TTV_H

#include "../genius/gen_API.h"
#include "sim_API.h"


static double sim_tas_thr2scm (double fthr, char dir)
{
  return fthr;
  if (dir=='d')
    {
      return elpThr2Scm (fthr, elpGeneral[elpGSLTHR],
                         elpGeneral[elpGSHTHR],
                         elpGetModelType (TRANSN)->elpModel[elpVT],
                         0.0, elpGeneral[elpGVDDMAX], elpFALL);
    }
  else
    {
      return elpThr2Scm (fthr, elpGeneral[elpGSLTHR],
                         elpGeneral[elpGSHTHR],
                         elpGetModelType (TRANSP)->elpModel[elpVT],
                         elpGeneral[elpGVDDMAX], elpGeneral[elpGVDDMAX], elpRISE);

    }
}

static void disp_func(char *inname, float intime, float inslope, char *outname, float outtime, float outslope, void *data)
{
  fprintf((FILE *)data, "%s (%g ps, slope=%g) -> %s (%g ps) => %g ps, slope=%g ps\n", 
          inname, intime*1e12, inslope*1e12, outname, outtime*1e12, (outtime-intime)*1e12, outslope*1e12);
}

void sim_DumpDelayDetail(sim_model *model, char *name1, char *name2, char *transition, FILE *f)
{
  sim_dump_delay (model, name1, name2, transition, disp_func, f);
}

static ht *slopeht, *delayht;
static char __dir;
static double fastest;

static void grab_func(char *inname, float intime, float inslope, char *outname, float outtime, float outslope, void *data)
{
  double delay;
  outname=namealloc(outname);
//  fprintf(stdout, "%s(%p) %g %g=>%g\n", outname, outname, outtime-intime, outslope*1e12, sim_tas_thr2scm(outslope*1e12, __dir));
  outslope=sim_tas_thr2scm(outslope*1e12, __dir);
  if ((delay=outtime)<fastest) fastest=delay;
  
  addhtitem(delayht, outname, (long)((outtime-intime)*1e15));
  addhtitem(slopeht, outname, (long)((outslope)*1e3));
}


void grab_split_pnodes(lofig_list *lf, char *name, num_list **internal, num_list **external, losig_list **modelsig)
{
  losig_list *ls;
  ptype_list *p;
  locon_list *lc;
  chain_list *cl;
  num_list *num;

  for (ls=lf->LOSIG; ls!=NULL && gen_losigname(ls)!=name; ls=ls->NEXT) ;
  if (ls==NULL) 
    {
      avt_errmsg(SIM_API_ERRMSG, "001", AVT_FATAL, name);
//      avt_error("sim", -1, AVT_ERR, "could not find signal '%s' in netlist\n", name);
      EXIT(1);
    }

  *modelsig=ls;
  *internal=NULL; *external=NULL;

  for (cl=(chain_list *)getptype(ls->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='E')
        for (num=lc->PNODE; num!=NULL; num=num->NEXT) *external=addnum(*external, num->DATA);
      else
        for (num=lc->PNODE; num!=NULL; num=num->NEXT) *internal=addnum(*internal, num->DATA);

    }
}

static inline void create_groups_if_needed(losig_list *ls)
{
  locon_list *lc;
  chain_list *cl, *ch;
  char *thisins;
  int countgrp=0;
  chain_list *internallocons;
  subinst_t *sins;

  if (rcx_has_rc_timings(ls)) return;
  thisins=gns_GetCurrentInstance();
  for (cl=(chain_list *)getptype(ls->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='I' && ((loins_list *)lc->ROOT)->INSNAME==thisins)
        {
          char buf[255];
          rcx_set_connector_origin_group(lc, 0);
          mbk_set_rc_external(lc);
//          printf("add orig %s, sig=%s\n",mbk_putconname(lc, buf), getsigname(lc->SIG));
        }
      else
        {
          char buf[255];

          if (lc->TYPE=='I' && (sins=gen_get_hier_instance(LATEST_GNS_RUN, ((loins_list *)lc->ROOT)->INSNAME))!=NULL)
            {
              internallocons=GrabAllConnectorsThruCorresp(lc->NAME, lc->SIG, sins->CRT, NULL);
              for (ch=internallocons; ch!=NULL; ch=ch->NEXT)
                {
                  rcx_set_connector_destination_group((locon_list *)ch->DATA, countgrp);
//                  printf("add internal dest %s, sig=%s, grp=%d\n",mbk_putconname((locon_list *)ch->DATA, buf), getsigname(lc->SIG), countgrp);
                }
              freechain(internallocons);
            }
          rcx_set_connector_destination_group(lc, countgrp);
//          printf("add dest %s, sig=%s, grp=%d\n",mbk_putconname(lc, buf), getsigname(lc->SIG), countgrp);
          countgrp++;
        }
    }
  rcx_create_signal_rc_timings(ls);
}

static inline void spread(losig_list *circuitsig, losig_list *modelsig, ht *delays, ht *slopes, float delta, float input_slope, char dir)
{
  locon_list *lc, *lc0;
  chain_list *cl, *ch;
  char *thisins;
  int countgrp=1;
  num_list *extnode;
  char *name;
  long l;
  float delay, slope;
  locon_list *myorig=NULL;
  chain_list *allmylocons;
  subinst_t *sins;

  thisins=gns_GetCurrentInstance();

  for (cl=(chain_list *)getptype(circuitsig->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='I' && ((loins_list *)lc->ROOT)->INSNAME==thisins) { myorig=lc; break; }
    }

  if (myorig==NULL) exit(4444);

  for (cl=(chain_list *)getptype(circuitsig->USER, LOFIGCHAIN)->DATA; cl!=NULL; cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      if (lc->TYPE=='I' && ((loins_list *)lc->ROOT)->INSNAME==thisins) continue;
      allmylocons=addchain(NULL, lc);

      if (lc->TYPE=='I' && (sins=gen_get_hier_instance(LATEST_GNS_RUN, ((loins_list *)lc->ROOT)->INSNAME))!=NULL)
        {
          allmylocons=GrabAllConnectorsThruCorresp(lc->NAME, lc->SIG, sins->CRT, allmylocons);
        }

      for (ch=allmylocons; ch!=NULL; ch=ch->NEXT)
        {
          for (extnode=((locon_list *)ch->DATA)->PNODE; extnode!=NULL; extnode=extnode->NEXT)
            {
              name=namealloc(spinamenode( modelsig, extnode->DATA ));
              //         printf("try %s(%p) (#%d)", name,name, extnode->DATA);
              if ((l=gethtitem(delays, name))!=EMPTYHT)
                delay=((float)l)*1e-3;
              else
                {
                  avt_errmsg(SIM_API_ERRMSG, "002", AVT_ERROR, name);
//                  avt_error("sim", -1, AVT_ERR, "could not find delay of node '%s'\n", name);
                  delay=0;
                }
              if ((l=gethtitem(slopes, name))!=EMPTYHT)
                slope=((float)l)*1e-3;
              else
                {
                  avt_errmsg(SIM_API_ERRMSG, "003", AVT_ERROR, name);
//                  avt_error("sim", -1, AVT_ERR, "could not find delay of node '%s'\n", name);
                  slope=0;
                }
              
              rcx_add_rc_timing(myorig, lc, input_slope, delay-delta, slope, dir);
/*              {
                char buf[256], buf1[256];
                printf("spread %s -> %s, sig=%s, slope=%g\n",mbk_putconname(myorig, buf), mbk_putconname(lc, buf1), getsigname(lc->SIG), input_slope);
              }*/
              //          printf(" delay=%g slope=%g\n", delay-delta, slope);
            }
        } 
      freechain(allmylocons);
    }  
}


double sim_SpreadRC(sim_model *sc, char *input, char *output, char *dir, double input_slope, double delta_delay)
{
  lofig_list *lf;
  losig_list *signal_in_circuit, *signal_in_model;
  char *noutput;
  num_list *internal, *external, *num;

  lf=sc->FIG;
  signal_in_circuit=gns_GetCorrespondingSignal(output);

//  printf("delta=%g, corresp %p %s\n",delta_delay,signal_in_circuit,signal_in_circuit?gen_losigname(signal_in_circuit):"?");
  if (signal_in_circuit==NULL)
    {
      avt_errmsg(SIM_API_ERRMSG, "001", AVT_ERROR, output);
//      avt_error("sim", -1, AVT_ERR, "sim_SpreadRC: could not find corresponding signal of '%s' in circuit\n", output);
      return delta_delay;
    }

  slopeht=addht(4096);
  delayht=addht(4096);

  __dir=tolower(dir[1]);
  input_slope=sim_tas_thr2scm(input_slope*1e12, __dir);

  noutput=namealloc(gen_makesignalname(output));

//  printf("add inputslope=%g %c\n",input_slope, __dir);
  fastest=1000000;
  sim_dump_delay (sc, input, output, dir, grab_func, NULL);
  if (fastest<delta_delay) { delta_delay=fastest;/* printf("new delta=%g\n", delta_delay);*/ }
  
  delta_delay*=1e12;
  grab_split_pnodes(lf, noutput, &internal, &external, &signal_in_model);
/*  
  printf("INTER\n");
  for (num=internal; num!=NULL; num=num->NEXT) printf("%d ", num->DATA);
  printf("\nEXTER\n");
  for (num=external; num!=NULL; num=num->NEXT) printf("%d ", num->DATA);
  printf("\n");
*/
  create_groups_if_needed(signal_in_circuit);
  
  spread(signal_in_circuit, signal_in_model, delayht, slopeht, delta_delay, input_slope, tolower(dir[1]));

  freenum(internal);
  freenum(external);
  delht(delayht);
  delht(slopeht);

  return delta_delay*1e-12;
}
