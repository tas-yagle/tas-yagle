#include MUT_H
#include MLO_H
#include STM_H
#include TTV_H
#include TAS_H
#include GEN_H
#include CNS_H
#include YAG_H

#define API_USE_REAL_TYPES
#include "ttv_API_LOCAL.h"
#include "ttv_API.h"
#include "ttv_API_util.h"

extern void ttvapi_setprefix(char *val);
extern void ttv_DisplayCompletePathDetail(FILE *f, int num, ttvpath_list *tp, chain_list *detail);

static int LoadFilesForSimIfNeeded(char *figname, lofig_list **lf, cnsfig_list **cf)
{
  char *where;

  *lf=NULL;
  *cf=ttvutil_cnsload(figname, getloadedinffig(figname));

  if (*cf) *lf = getloadedlofig (figname);
  if (*lf==NULL && filepath (figname,IN_LO))
    {
      *lf = getlofig(figname,'A');
    }

  if (!*cf) fprintf (stderr, "ERR ttv_DriveSpiceDeck: no CNS file\n");
  if (!*lf) return 1;
        
  if ((where=filepath (figname,"gns"))!=NULL)
    {
      if (LATEST_GNS_RUN==NULL || LATEST_GNS_RUN->GLOBAL_LOFIG!=*lf)
        {
          printf("Loading '%s'\n",where);
          LoadDynamicLibraries (NULL);
          LATEST_GNS_RUN = gnsParseCorrespondanceTables(figname);
          UpdateTransistorsForYagle(LATEST_GNS_RUN,TRUE);
        }
    }
  return 0;
}


static ptype_list *ttv_simdetectloop(ttvcritic_list *tc)
{
  ttvcritic_list *start=tc, *end=tc->NEXT, *check, *prev=NULL, *newtc;
  ptype_list *split_tc=NULL;
  while (end!=NULL)
    {      
      for (check=start; check!=end && strcmp(check->NAME, end->NAME)!=0; check=check->NEXT) ;
      if (check!=end)
        {
          // creation d'une sous critic list
          check=prev->NEXT;
          prev->NEXT=NULL;
          newtc=ttv_dupcritic(start);
          prev->NEXT=check;
          split_tc=addptype(split_tc, (long)start, newtc);
          start=prev;
        }
      prev=end;
      end=end->NEXT;
    }  

  if (start!=tc)
    {
      // creation de la derniere sous critic list
      newtc=ttv_dupcritic(start);
      split_tc=addptype(split_tc, (long)start, newtc);
    }
  return (ptype_list *)reverse((chain_list *)split_tc);
}



int ttv_SimulatePath_sub(ttvfig_list *tvf, ttvpath_list *tp, char *mode, int mc)
{
  lofig_list *lf;
  cnsfig_list *cf;
  int minmax=SIM_MAX, res_simu, cnt, plot;
  ttvcritic_list *tc, *cktc, *gototc;
  chain_list *criticlist, *detail ;
  ptype_list *split_tc, *pt;
  double slope;
  ttvevent_list *savcmd;

  detail=ttv_GetPathDetail(tp);
  if (detail==NULL) return 0;
  freechain(detail);

  tc=tp->CRITIC;  

  if (strstr (mode, "plot") != NULL) plot=1; else plot=0;

  if (strstr (mode, "force")==NULL)
    {
      for (cktc=tc; cktc!=NULL && (cktc->SIMDELAY==TTV_NOTIME || cktc->SIMSLOPE==TTV_NOTIME); cktc=cktc->NEXT) ;
      if (cktc!=NULL) return 0;
    }
  if (tc->NEXT==NULL)
    {
      GSP_FOUND_SOLUTION_NUMBER=1;      
      tc->SIMDELAY=tc->DELAY;
      tc->SIMSLOPE=tc->SLOPE;
      return 0;
    }

  mbkenv ();
  cnsenv ();
  yagenv (tas_yaginit);
  simenv();

  if (LoadFilesForSimIfNeeded(tvf->INFO->FIGNAME, &lf, &cf)) return 0;

  if((tc->TYPE & TTV_FIND_MAX)==TTV_FIND_MAX) minmax=SIM_MAX; else minmax=SIM_MIN;

  if ((split_tc=ttv_simdetectloop(tc))==NULL)
    {
      tas_setenv();
      
      TAS_CONTEXT->FRONT_CON = tas_get_input_slope(tc);
            
      criticlist = addchain( NULL, tp );
      if (strstr (mode, "no_simu")!=NULL)
        res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 0, NULL, NULL, NULL, mc , plot );
      else
        res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 1, NULL, NULL, NULL, mc , plot );
      freechain( criticlist );
      
      tas_restore();
    }
  else
    {
      char buf[512];
      res_simu=1;
      slope=tas_get_input_slope(tc);
      GSP_LOOP_MODE=1;
      EFG_SET_FIG_NAME=buf;
      for (pt=split_tc, cnt=1; pt!=NULL && res_simu; pt=pt->NEXT, cnt++)
        {
          sprintf(buf,"%s__ext__P%d", lf->NAME, cnt);
          if (res_simu)
            {
              tas_setenv();
              
              TAS_CONTEXT->FRONT_CON = slope;
              
              cktc=tp->CRITIC;
              savcmd=tp->CMD;
              tp->CRITIC=(ttvcritic_list *)pt->DATA;
              if (pt->NEXT!=NULL) tp->CMD=NULL;
              criticlist = addchain( NULL, tp );
              if (strstr (mode, "no_simu")!=NULL)
                res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 0, NULL, NULL, NULL, mc , plot );
              else
                res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 1, NULL, NULL, NULL, mc , plot );
              freechain( criticlist );
              tp->CMD=savcmd;
              tp->CRITIC=cktc;
              
              tas_restore();
            }
          if (res_simu)
            {
              cktc=(ttvcritic_list *)pt->TYPE;
              gototc=(ttvcritic_list *)pt->DATA;
              if (pt!=split_tc)
              {
                cktc=cktc->NEXT;
                gototc=gototc->NEXT;
              }
              for (tc=gototc; tc!=NULL; tc=tc->NEXT, cktc=cktc->NEXT)
                {
                  slope=tc->SIMSLOPE/TTV_UNIT;
                  cktc->SIMSLOPE=tc->SIMSLOPE;
                  cktc->SIMDELAY=tc->SIMDELAY;
                }
            }
          ttv_freecriticlist((ttvcritic_list *)pt->DATA);
        }
      EFG_SET_FIG_NAME=NULL;
      freeptype(split_tc);
      GSP_LOOP_MODE=0;
    }
  if (res_simu==1) return 0;
  return 1;
}

int ttv_DriveSpiceDeck(ttvfig_list *tvf, ttvpath_list *tp, char *filename)
{
  lofig_list *lf;
  cnsfig_list *cf;
  int minmax=SIM_MAX, res_simu;
  FILE *f;
  ttvcritic_list *tc;
  chain_list *criticlist, *detail;

  detail=ttv_GetPathDetail(tp);
  if (detail==NULL) return 0;

  mbkenv ();
  cnsenv ();
  yagenv (tas_yaginit);
  simenv();

  if (LoadFilesForSimIfNeeded(tvf->INFO->FIGNAME, &lf, &cf)) return 0;

  tas_setenv();


  tc=tp->CRITIC;

  TAS_CONTEXT->FRONT_CON = tas_get_input_slope(tc);

  if((tc->TYPE & TTV_FIND_MAX)==TTV_FIND_MAX) minmax=SIM_MAX; else minmax=SIM_MIN;

  if ((f=fopen(filename,"wt"))==NULL) 
    {
      avt_errmsg(TTV_API_ERRMSG, "036", AVT_ERROR, filename);
//      avt_error("ttvapi", 501, AVT_ERR, "could not open file '%s'\n",filename);
      return 1;
    }

  avt_printExecInfo(f, "*", "Spice deck", "");
  ttvapi_setprefix("* ");
  ttv_DisplayCompletePathDetail(f, 0, NULL, detail);
  ttvapi_setprefix("");

  criticlist = addchain( NULL, tp );
  res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 0, filename, f, NULL, 0, 0);
  freechain( criticlist );

  if (res_simu!=1)
    {
      fprintf(f, "\n* Errors occurred while extracting spicedeck\n");
      fprintf(f, "* most likely it's a false path that could not be activated\n");
      fprintf(f, "* after its behaviour analysis\n");
    }

  fclose(f);

  tas_restore();

  if (res_simu==1) return 0;
  return 1;
}

int ttv_DriveSetupHoldSpiceDeck( ttvfig_list *tvf, chain_list *datapath, chain_list *ckpathlist, char *filename )
{
  lofig_list     *lf;
  cnsfig_list    *cf;
  int            minmax = SIM_MAX, 
                 res_simu;
  FILE           *f;
  chain_list     *criticlist ;
  chain_list     *detdata, *detck ;
  chain_list     *chainckpath ;
  
  if( !datapath || !ckpathlist ) 
    return 0;

  mbkenv ();
  cnsenv ();
  yagenv( tas_yaginit );
  simenv( 0 );

  if( LoadFilesForSimIfNeeded( tvf->INFO->FIGNAME, &lf, &cf) ) 
    return 0;

  tas_setenv();

  detdata = ttv_GetPathDetail( (ttvpath_list *)datapath->DATA );
  TAS_CONTEXT->FRONT_CON = tas_get_input_slope( (ttvcritic_list*)detdata->DATA );

  f = fopen( filename, "wt" ) ;
  
  if( !f ) {
    avt_errmsg(TTV_API_ERRMSG, "036", AVT_ERROR, filename);
//    avt_error("ttvapi", 501, AVT_ERR, "could not open file '%s'\n",filename);
    return 1;
  }

  avt_printExecInfo(f, "*", "Spice deck", "");

  ttvapi_setprefix( "* " );
  ttv_DisplayCompletePathDetail( f, 0, NULL, detdata );
  for( chainckpath = ckpathlist ; chainckpath ; chainckpath = chainckpath->NEXT ) {
    detck = ttv_GetPathDetail( (ttvpath_list *)chainckpath->DATA );
    ttv_DisplayCompletePathDetail( f, 0, NULL, detck );
    freechain(detck);
  }
  ttvapi_setprefix( "" );

//  tcck   = (ttvcritic_list*)(ttv_GetPathDetail( (ttvpath_list *)ckpathlist->DATA ))->DATA ;

  criticlist = addchain( NULL, datapath->DATA );
  for( chainckpath = ckpathlist ; chainckpath ; chainckpath = chainckpath->NEXT ) {
    detck=ttv_GetPathDetail( (ttvpath_list *)chainckpath->DATA ) ;
    criticlist = addchain( criticlist, chainckpath->DATA );
    freechain(detck);
  }
  criticlist = reverse( criticlist );

  res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 0, filename, f, NULL, 0, 0);
  freechain( criticlist );

  if (res_simu!=1)
    {
      fprintf(f, "\n* Errors occurred while extracting spicedeck\n");
      fprintf(f, "* most likely it's a false path that could not be activated\n");
      fprintf(f, "* after its behaviour analysis\n");
    }

  fclose(f);

  freechain(detdata);
  tas_restore();

  if (res_simu==1) return 0;
  return 1;
}


chain_list *ttv_SimulateCharacPathTables(ttvfig_list *tvf, ttvpath_list *tp, chain_list *slopes, chain_list *capas, int maxsim)
{
  lofig_list *lf;
  cnsfig_list *cf;
  int minmax=SIM_MAX, res_simu, cnt, sim=1;
  ttvcritic_list *tc, *cktc, *gototc, *reachtc;
  chain_list *criticlist, *detail ;
  ptype_list *split_tc, *pt;
  double slope;
  ttvevent_list *savcmd;
  tas_spice_charac_struct tscs;
  chain_list *final_res;
  chain_list *cl, *ch, *ch0;
          
  API_TEST_TOKEN_SUB(TMA_API,"tma")
        
  tscs.maxthreads=maxsim>1?maxsim:1;
  
  detail=ttv_GetPathDetail(tp);
  if (detail==NULL) return NULL;
  freechain(detail);

  tc=tp->CRITIC;  
 
  if (tc->NEXT==NULL)
    {
      GSP_FOUND_SOLUTION_NUMBER=1;
      final_res=NULL;
      for (ch=slopes; ch!=NULL; ch=ch->NEXT)
      {
        ch0=NULL;
        for (cl=capas; cl!=NULL; cl=cl->NEXT)
        {
          ch0=addchain(ch0, ttv_duppath(tp));
          tc=((ttvpath_list *)ch0->DATA)->CRITIC;
          tc->SIMDELAY=0;
          tc->SIMSLOPE=(*(float *)&ch->DATA)*1e12/TTV_UNIT;
        }
        final_res=addchain(final_res, ch0);
      }
      return final_res;
    }

  mbkenv ();
  cnsenv ();
  yagenv (tas_yaginit);
  simenv();

  if (LoadFilesForSimIfNeeded(tvf->INFO->FIGNAME, &lf, &cf)) return 0;

  if((tc->TYPE & TTV_FIND_MAX)==TTV_FIND_MAX) minmax=SIM_MAX; else minmax=SIM_MIN;

  if ((split_tc=ttv_simdetectloop(tc))==NULL)
    {
      tas_setenv();
      
      TAS_CONTEXT->FRONT_CON = tas_get_input_slope(tc);
      tscs.slopes=slopes;
      tscs.capas=capas;
      criticlist = addchain( NULL, tp );
      res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 1, NULL, NULL, &tscs, 0 , 0 );
      freechain( criticlist );
      
      tas_restore();
      return tscs.result;
    }
  else
    {
      char buf[512];
      chain_list zerocapa;
      chain_list *trslopes, *reschcapa, *resch;
      
      zerocapa.NEXT=NULL;
      *(float *)&zerocapa.DATA=0;
      
      final_res=NULL;

      for (ch=slopes; ch!=NULL; ch=ch->NEXT)
      {
        ch0=NULL;
        for (cl=capas; cl!=NULL; cl=cl->NEXT)
          ch0=addchain(ch0, ttv_duppath(tp));
        final_res=addchain(final_res, ch0);
      }
      
      res_simu=1;
      slope=tas_get_input_slope(tc);
      GSP_LOOP_MODE=1;
      EFG_SET_FIG_NAME=buf;
      trslopes=dupchainlst(slopes);
      for (pt=split_tc, cnt=1; pt!=NULL && res_simu; pt=pt->NEXT, cnt++)
        {
          sprintf(buf,"%s__ext__P%d", lf->NAME, cnt);
          tscs.slopes=trslopes;
          if (pt->NEXT!=NULL)
            tscs.capas=&zerocapa;
          else
            tscs.capas=capas;

          if (res_simu)
            {
              tas_setenv();
              
              TAS_CONTEXT->FRONT_CON = slope;
              
              cktc=tp->CRITIC;
              savcmd=tp->CMD;
              tp->CRITIC=(ttvcritic_list *)pt->DATA;
              if (pt->NEXT!=NULL) tp->CMD=NULL;
              criticlist = addchain( NULL, tp );
              res_simu = tas_simu_netlist(tvf, lf, cf, NULL, criticlist, minmax, 1, NULL, NULL, &tscs, 0 , 0 );
              freechain( criticlist );
              tp->CMD=savcmd;
              tp->CRITIC=cktc;
              
              tas_restore();
            }
          freechain(trslopes);
          if (res_simu)
            {
              trslopes=NULL;
              for (ch=final_res, resch=tscs.result; ch!=NULL; ch=ch->NEXT, resch=resch->NEXT)
                {
                  reschcapa=(chain_list *)resch->DATA;
                  for (cl=(chain_list *)ch->DATA; cl!=NULL; cl=cl->NEXT)
                  {
                     for (gototc=tp->CRITIC, reachtc=((ttvpath_list *)cl->DATA)->CRITIC; gototc!=(ttvcritic_list *)pt->TYPE; gototc=gototc->NEXT, reachtc=reachtc->NEXT) ;
                     gototc=((ttvpath_list *)reschcapa->DATA)->CRITIC;
                     if (pt!=split_tc)
                     {
                       reachtc=reachtc->NEXT;
                       gototc=gototc->NEXT;
                     }
                     for (tc=gototc; tc!=NULL; tc=tc->NEXT, reachtc=reachtc->NEXT)
                     {
                        slope=_LONG_TO_DOUBLE(tc->SIMSLOPE);
                        reachtc->SIMSLOPE=tc->SIMSLOPE;
                        reachtc->SIMDELAY=tc->SIMDELAY;
                     }
                     if (pt->NEXT!=NULL)
                     {
                       if (cl==ch->DATA)
                       {
                         trslopes=addchain(trslopes, NULL);
                         *(float *)&trslopes->DATA=slope;
                       }
                     }
                     else
                     {
                       reschcapa=reschcapa->NEXT;
                     }
                  }
                  ttv_FreePathList((chain_list *)resch->DATA);
                  freechain((chain_list *)resch->DATA);
                }
              freechain(tscs.result);
            }
          ttv_freecriticlist((ttvcritic_list *)pt->DATA);
        }
      EFG_SET_FIG_NAME=NULL;
      freeptype(split_tc);
      GSP_LOOP_MODE=0;
    }

  return final_res;
}

void ttv_Simulate_AddDelayToVT(ttvfig_list *tvf, ttvpath_list *pth, ttvevent_list *latch)
{
  float vt=-1;
  long find=0;
  if (tvf!=NULL)
    {
      if ((ttv_getloadedfigtypes(tvf) & TTV_FILE_DTX)==TTV_FILE_DTX)
        find=TTV_FIND_LINE;
      else
        find=TTV_FIND_PATH;

      if ((pth->ROOT->TYPE & TTV_SIG_Q)!=0)
        vt=ttv_getline_vt(tvf, NULL, latch, ttv_opposite_event(pth->ROOT), (pth->TYPE & (TTV_FIND_MAX|TTV_FIND_MIN))|find);
      
      if (vt>0)
        {
          pth->USER=testanddelptype(pth->USER, TTV_PATH_END_VT);
          pth->USER=addptype(pth->USER, TTV_PATH_END_VT, NULL);
          *(float *)&pth->USER->DATA=vt;
        }
    }
}

int ttv_Simulate_FoundSolutions()
{
  return GSP_FOUND_SOLUTION_NUMBER;
}
