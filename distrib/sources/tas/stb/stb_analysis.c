/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_analysis.c                                              */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>

#include MUT_H
#include STB_H
#include RCN_H
#include TRC_H
#include TTV_H

extern int TAS_PVT_COUNT;

#include "stb_util.h"
#include "stb_error.h"
#include "stb_parse.h"
#include "stb_init.h"
#include "stb_relaxation.h"
#include "stb_relaxation_correction.h"
#include "stb_transfer.h"
#include "stb_analysis.h"
#include "stb_ctk.h"
#include "stb_drive.h"
#include "stb_falseslack.h"

/*****************************************************************************
*                           fonction stb_compstberr                          *
*****************************************************************************/
int stb_compstberr(chain1,chain2)
chain_list **chain1  ;
chain_list **chain2  ;
{
 ttvsig_list *ptsig1  ;
 ttvsig_list *ptsig2  ;
 ptype_list *ptype ;
 stbnode *ptnode  ;
 int i  ;
 long max1 = STB_NO_TIME  ;
 long max2 = STB_NO_TIME  ;
 long setup = STB_NO_TIME ;
 long hold = STB_NO_TIME ;

 ptsig1 = (*chain1)->DATA  ;
 
 if((ptype = getptype(ptsig1->USER,STB_SETUP)) != NULL)
   setup = (long)ptype->DATA ;
 if((ptype = getptype(ptsig1->USER,STB_HOLD)) != NULL)
   hold = (long)ptype->DATA ;

 if(ptype == NULL)
  for(i = 0  ; i < 2  ; i++)
   {
    if((ptnode = stb_getstbnode(ptsig1->NODE+i)) == NULL)
      continue  ;
    if(ptnode->SETUP < setup)
      setup = ptnode->SETUP  ;
    if(ptnode->HOLD < hold)
      hold = ptnode->HOLD  ;
   }

 if(setup < max1) 
   max1 = setup ;

 if(hold < max1) 
   max1 = hold ;

 setup = STB_NO_TIME ;
 hold = STB_NO_TIME ;

 ptsig2 = (*chain2)->DATA  ;

 if((ptype = getptype(ptsig2->USER,STB_SETUP)) != NULL)
   setup = (long)ptype->DATA ;
 if((ptype = getptype(ptsig2->USER,STB_HOLD)) != NULL)
   hold = (long)ptype->DATA ;

 if(ptype == NULL)
  for(i = 0  ; i < 2  ; i++)
   {
    if((ptnode = stb_getstbnode(ptsig2->NODE+i)) == NULL)
      continue  ;
    if(ptnode->SETUP < setup)
      setup = ptnode->SETUP  ;
    if(ptnode->HOLD < hold)
      hold = ptnode->HOLD  ;
   }

 if(setup < max2) 
   max2 = setup ;

 if(hold < max2) 
   max2 = hold ;

 if (max1==max2)
  {
   // pour garantir un ordre quand les setup/hold sont egaux
   return strcmp(ptsig1->NAME, ptsig2->NAME);
  }

 return((int)(max1-max2)) ;
}

/*****************************************************************************
*                           fonction stb_geterrorlist                        *
*****************************************************************************/
chain_list *stb_geterrorlist(stbfig,margin,nberror,minsetup,minhold,errnumb)
stbfig_list *stbfig  ;
long margin  ;
int nberror  ;
long *minsetup  ;
long *minhold  ;
int *errnumb ;
{
 ttvsig_list *ptsig  ;
 ttvsig_list *ptsigx  ;
 stbnode *ptnode  ;
 chain_list *chainerr = NULL  ;
 chain_list *chain  ;
 chain_list *chainx  ;
 chain_list **chaintab  ;
 chain_list *chainsig  ;
 chain_list *chainsav  ;
 chain_list *ptchain  ;
 stbdebug_list *debug ;
 stbdebug_list *ptdebug ;
 ptype_list *ptype ;
 long setup ;
 long hold ;
 long sigsetup ;
 long sighold ;
 long noerrminsetup  ;
 long noerrminhold  ;
 int i  ;
 int nb = 0  ;
 int nberr ;
 char flag  ;

 *minsetup = STB_NO_TIME  ;
 *minhold = STB_NO_TIME  ;
 noerrminsetup = STB_NO_TIME  ;
 noerrminhold = STB_NO_TIME  ;
 
 chainsig = ttv_getsigbytype(stbfig->FIG,NULL,TTV_SIG_TYPEALL/*C|
                                              TTV_SIG_Q|
                                              TTV_SIG_L|
                                              TTV_SIG_B|
                                              TTV_SIG_R*/,NULL) ;

 for(chain = chainsig  ; chain != NULL  ; chain = chain->NEXT)
   {
    ptsig = (ttvsig_list *)chain->DATA  ;
    flag = 'N'  ;
    sigsetup = STB_NO_TIME ;
    sighold = STB_NO_TIME ;
    for(i = 0  ; i < 2  ; i++)
     {
      if((ptnode = stb_getstbnode(ptsig->NODE+i)) == NULL)
        continue  ;

      if((ptnode->SETUP < *minsetup) && (ptnode->SETUP != STB_NO_TIME))
        *minsetup = ptnode->SETUP  ;

      if((ptnode->HOLD < *minhold) && (ptnode->HOLD != STB_NO_TIME))
        *minhold = ptnode->HOLD  ;

      if((ptnode->SETUP < sigsetup) && (ptnode->SETUP != STB_NO_TIME))
         sigsetup = ptnode->SETUP ;

      if((ptnode->HOLD < sighold) && (ptnode->HOLD != STB_NO_TIME))
         sighold = ptnode->HOLD ;


      if((ptnode->SETUP < margin) ||
         (ptnode->HOLD < margin))
         flag = 'Y'  ;
     }
    if(flag =='Y')
     {
      chainerr = addchain(chainerr,ptsig) ;
      nb++  ;
     }
    else
     {
      if((sigsetup < noerrminsetup) && (sigsetup != STB_NO_TIME))
         noerrminsetup = sigsetup ;

      if((sighold < noerrminhold) && (sighold != STB_NO_TIME))
         noerrminhold = sighold ;
     }
   }

 freechain(chainsig) ;

 if(chainerr == NULL)
  {
   *errnumb = 0 ;
   return(NULL) ;
  }

 chaintab = (chain_list**)mbkalloc(nb * sizeof(chain_list *)) ;

 chain = chainerr  ;

 for(i = 0  ; i < nb  ; i ++)
  {
   *(chaintab + i) = chain  ;
   chain = chain->NEXT  ;
  }

 qsort(chaintab,nb,sizeof(chain_list *),
       (int (*)(const void*,const void*))stb_compstberr)   ;

 chainerr = *chaintab  ;
 chain = chainerr  ;

 for(i = 1  ; i < nb  ; i ++)
  {
   chain->NEXT = *(chaintab + i) ;
   chain = chain->NEXT  ;
  }

 chain->NEXT = NULL  ;

 mbkfree(chaintab) ;

 chainsig = chainerr ;
 chainerr = NULL ;
 nb = 0 ;
 nberr = 0 ;
 *minsetup = STB_NO_TIME  ;
 *minhold = STB_NO_TIME  ;

 chain = chainsig  ;
 chainsav = NULL ;

 while(chain != NULL)
   {
    ptsig = (ttvsig_list *)chain->DATA  ;

    if((ptype = getptype(ptsig->USER,STB_SETUP)) != NULL)
      setup = (long)ptype->DATA ;
    if((ptype = getptype(ptsig->USER,STB_HOLD)) != NULL)
      hold = (long)ptype->DATA ;

     if(ptype != NULL)
      {
       chain = chain->NEXT ;
       if((nberr < nberror) && ((setup < margin) || (hold < margin)))
         {
          chainerr = addchain(chainerr,ptsig) ;
          nberr++ ;
         }
       else
         {
          ptsig->USER = delptype(ptsig->USER,STB_SETUP) ;
          ptsig->USER = delptype(ptsig->USER,STB_HOLD) ;
         }
       continue ;
      }

    flag = 'N'  ;
    setup = STB_NO_TIME ;
    hold = STB_NO_TIME ;
    sigsetup = STB_NO_TIME ;
    sighold = STB_NO_TIME ;

    for(i = 0  ; i < 2  ; i++)
     {
      if((ptnode = stb_getstbnode(ptsig->NODE+i)) == NULL)
        continue  ;

      if((ptnode->SETUP < sigsetup) && (ptnode->SETUP != STB_NO_TIME))
         sigsetup = ptnode->SETUP ;

      if((ptnode->HOLD < sighold) && (ptnode->HOLD != STB_NO_TIME))
         sighold = ptnode->HOLD ;

      if((ptnode->SETUP < margin) ||
         (ptnode->HOLD < margin))
         flag = 'Y'  ;
     }

    if(flag =='Y')
     {
      if(nberr < nberror)
       {
        debug = stb_debugstberror(stbfig,ptsig,STB_NO_TIME, 0) ;
        if(debug == NULL)
         {
          chain = chain->NEXT ;
          continue ;
         }
       }
      else 
       {
        hold = sighold ;
        setup = sigsetup ;
        debug = NULL ;
        if((sigsetup < noerrminsetup) && (sigsetup != STB_NO_TIME))
          noerrminsetup = sigsetup  ;

        if((sighold < noerrminhold) && (sighold != STB_NO_TIME))
          noerrminhold = sighold  ;
       }

      for(ptdebug = debug ; ptdebug ; ptdebug = ptdebug->NEXT)
        {
         if((ptdebug->SETUP < *minsetup) && (ptdebug->SETUP != STB_NO_TIME))
           *minsetup = ptdebug->SETUP  ;
         if((ptdebug->HOLD < *minhold) && (ptdebug->HOLD != STB_NO_TIME))
           *minhold = ptdebug->HOLD  ;

         if((ptdebug->SETUP < setup) && (ptdebug->SETUP != STB_NO_TIME))
           setup = ptdebug->SETUP  ;

         if((ptdebug->HOLD < hold) && (ptdebug->HOLD != STB_NO_TIME))
           hold = ptdebug->HOLD  ;
        }

       if(debug != NULL)
        {
         if((sighold != hold) || (sigsetup != setup))
          {
           ptsig->USER = addptype(ptsig->USER,STB_SETUP,(void *)setup) ;
           ptsig->USER = addptype(ptsig->USER,STB_HOLD,(void *)hold) ;
          }
         stb_freestbdebuglist(debug) ;
        }

       if((setup < margin) ||
          (hold < margin))
         {
          nb++  ;
         }

       if((chain->NEXT != NULL) && ((sighold != hold) || (sigsetup != setup)) 
          && (nberr < nberror))
        {
         for(chainx = chain ; chainx->NEXT != NULL ; chainx = chainx->NEXT)
           {
            ptsigx = (ttvsig_list *)chainx->NEXT->DATA  ;
  
            sigsetup = STB_NO_TIME ;
            sighold = STB_NO_TIME ;
          
            if((ptype = getptype(ptsigx->USER,STB_SETUP)) != NULL)
               sigsetup = (long)ptype->DATA ;

            if((ptype = getptype(ptsigx->USER,STB_HOLD)) != NULL)
               sighold = (long)ptype->DATA ;

            if(ptype == NULL)
            for(i = 0  ; i < 2  ; i++)
             {
              if((ptnode = stb_getstbnode(ptsigx->NODE+i)) == NULL)
                continue  ;  ;

              if((ptnode->SETUP < sigsetup) && (ptnode->SETUP != STB_NO_TIME))
                 sigsetup = ptnode->SETUP ;
      
              if((ptnode->HOLD < sighold) && (ptnode->HOLD != STB_NO_TIME))
                 sighold = ptnode->HOLD ;

              if((ptnode->SETUP < margin) ||
                 (ptnode->HOLD < margin))
                 flag = 'Y'  ;
             }

            if(((sigsetup < sighold) ? sigsetup : sighold) > 
               ((setup < hold) ? setup : hold))
               break ;
           }
         if(chainx == chain)
           {
            if((setup < margin) ||
               (hold < margin))
             {
              chainerr = addchain(chainerr,ptsig) ;
              nberr++ ;
             }
           }
         else
           {
            if(chainsav == NULL)
             {
              ptchain = chain ;
              chain = chain->NEXT ;
              ptchain->NEXT = chainx->NEXT ;
              chainx->NEXT = ptchain ;
             }
            else
             {
              ptchain = chain ;
              chainsav->NEXT = chain->NEXT ;
              chain = chain->NEXT ;
              ptchain->NEXT = chainx->NEXT ;
              chainx->NEXT = ptchain ;
             }
            continue ;
           }
        }
       else if(nberr < nberror)
        {
         if((setup < margin) ||
            (hold < margin))
           {
            chainerr = addchain(chainerr,ptsig) ;
            nberr++ ;
           }
        }
      }
    chainsav = chain ;
    chain = chain->NEXT ;
   }

 *errnumb = nb ;
 nb = nberr ;

 if(*minsetup > noerrminsetup)
   *minsetup = noerrminsetup ;

 if(*minhold > noerrminhold)
   *minhold = noerrminhold ;

 freechain(chainsig) ;

 if(chainerr == NULL)
   return(NULL) ;

 chaintab = (chain_list**)mbkalloc(nb * sizeof(chain_list *)) ;

 chain = chainerr  ;

 for(i = 0  ; i < nb  ; i ++)
  {
   *(chaintab + i) = chain  ;
   chain = chain->NEXT  ;
  }

 qsort(chaintab,nb,sizeof(chain_list *),
       (int (*)(const void*,const void*))stb_compstberr)   ;

 chainerr = *chaintab  ;
 chain = chainerr  ;

 for(i = 1  ; i < nb  ; i ++)
  {
   chain->NEXT = *(chaintab + i) ;
   chain = chain->NEXT  ;
  }

 chain->NEXT = NULL  ;

 mbkfree(chaintab) ;

 for(i = 0 , chain = chainerr  ; i < (nberror - 1) && chain != NULL  ;
     chain = chain->NEXT , i++) ;

 if(chain != NULL)
  {
   freechain(chain->NEXT) ;
   chain->NEXT = NULL  ;
  }

 return(chainerr) ;
}

/*****************************************************************************
*                           fonction stb_delerrorlist                        *
*****************************************************************************/
void stb_delerrorlist(chainerr)
chain_list *chainerr ;
{
 ttvsig_list *ptsig  ;
 chain_list  *chain  ;

 for(chain = chainerr ; chain != NULL ; chain = chain->NEXT)
   {
    ptsig = (ttvsig_list *)chain->DATA ;

    if(getptype(ptsig->USER,STB_SETUP) != NULL)
      ptsig->USER = delptype(ptsig->USER,STB_SETUP) ;

    if(getptype(ptsig->USER,STB_HOLD) != NULL)
      ptsig->USER = delptype(ptsig->USER,STB_HOLD) ;
   }
}

/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
stb_testSTB(ttvfig_list *ttvfig)
{
  stbfig_list *stbfig ;

  stbfig                = stb_addstbfig(ttvfig) ;
  stbfig->ANALYSIS      = 0;
  stbfig->CTKMODE       = 0;
  stbfig->GRAPH         = 0;
  stbfig->CONNECTOR     = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_C, NULL) ;
  stbfig->COMMAND       = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_Q, NULL) ;
  stbfig->MEMORY        = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_L, NULL) ;
  stbfig->PRECHARGE     = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_R, NULL) ;
  stbfig->BREAK         = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_B, NULL) ;
  stbfig->STABILITYMODE = 0;

  stb_init(stbfig) ;
  stb_loadstbfig(stbfig) ;
  stb_savestbfig(stbfig,STB_DRIVE_INTERNALS,0,STB_SUFFIX_STO);
}

static void stb_update_ttv_clocks(ttvfig_list *tvf, stbfig_list *stbfig)
{
  chain_list *cl;
  inffig_list *ifl=NULL;
  int updated=0;
  ttvsig_list *tvs;
  char buf[1024];

  for (cl=stbfig->CLOCK; cl!=NULL; cl=cl->NEXT)
  {
    tvs=(ttvsig_list *)cl->DATA;
    if (getptype(tvs->USER, TTV_SIG_CLOCK)==NULL)
    {
      updated=1;
      if (ifl==NULL && (ifl=getloadedinffig(tvf->INFO->FIGNAME))==NULL)
        ifl=addinffig(tvf->INFO->FIGNAME);
      ttv_getsigname(tvf, buf, tvs);
      inf_AddInt(ifl, INF_LOADED_LOCATION, buf, INF_CLOCK_TYPE, 0, NULL);
    }
  }

  if (updated) ttv_getinffile(tvf);
}

static int stb_checkconfigsto(inffig_list *origifl, inffig_list *stoifl)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  char buf[1024], *var;
  int set, i;

  if (inf_GetPointer (stoifl, INF_CONDITIONS, "", (void **)&list)) 
    {
      for (chainx = list; chainx; chainx = chainx->NEXT) 
        {
          assoc = (inf_assoc *) chainx->DATA;
          if (strcasecmp(assoc->orig, "key_fp_bp")==0)
            {
              sprintf(buf, "%x", stb_getfalsepathkey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "Falsepath or Bypass");
            }
          else if (strcasecmp(assoc->orig, "key_mc")==0)
            {
              sprintf(buf, "%x", stb_getmulticyclekey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "Multicycle");
            } 
          else if (strcasecmp(assoc->orig, "key_d")==0)
            {
              sprintf(buf, "%x", stb_getdirectivekey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "Directive");
            }
          else if (strcasecmp(assoc->orig, "key_dm")==0)
            {
              sprintf(buf, "%x", stb_getdelaymarginkey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "DelayMargin");
            }
          else if (strcasecmp(assoc->orig, "key_nc")==0)
            {
              sprintf(buf, "%x", stb_getnocheckkey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "NoCheck");
            }
          else if (strcasecmp(assoc->orig, "key_nf_nr")==0)
            {
              sprintf(buf, "%x", stb_getlistsectionkey(origifl, INF_NORISING)*3+stb_getlistsectionkey(origifl, INF_NOFALLING));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "NoRising or NoFalling");
            }
           else if (strcasecmp(assoc->orig, "key_fs")==0)
            {
              sprintf(buf, "%x", stb_getfalseslackkey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "FalseSlack");
            }
           else if (strcasecmp(assoc->orig, "key_proba")==0)
            {
              sprintf(buf, "%x", stb_getswitchingprobakey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "SwitchingProbability");
            }
           else if (strcasecmp(assoc->orig, "stb_getclockuncertaintykey")==0)
            {
              sprintf(buf, "%x", stb_getclockuncertaintykey(origifl));
              if (strcmp(buf, assoc->dest)!=0)
                avt_errmsg(STB_ERRMSG, "039", AVT_ERROR, "ClockUncertainty");
            }
        }
      for (i=0; i<sto_cfg_size; i++)
        {
          if (sto_cfg[i].type=='b')
            { 
              set=V_BOOL_TAB[sto_cfg[i].idx].SET;
              var=V_BOOL_TAB[sto_cfg[i].idx].VAR;
              sprintf(buf,"%s",V_BOOL_TAB[sto_cfg[i].idx].VALUE?"yes":"no");
            }
          else if (sto_cfg[i].type=='s')
            {
              if (V_STR_TAB[sto_cfg[i].idx].SET && V_STR_TAB[sto_cfg[i].idx].VALUE!=NULL) set=1; else set=0;
              var=V_STR_TAB[sto_cfg[i].idx].VAR;
              sprintf(buf,"%s",V_STR_TAB[sto_cfg[i].idx].VALUE!=NULL?V_STR_TAB[sto_cfg[i].idx].VALUE:"NULL");
            }
          else if (sto_cfg[i].type=='i')
            {
              set=V_INT_TAB[sto_cfg[i].idx].SET;
              var=V_INT_TAB[sto_cfg[i].idx].VAR;
              sprintf(buf,"%d",V_INT_TAB[sto_cfg[i].idx].VALUE);
            }

          for (chainx = list; chainx; chainx = chainx->NEXT) 
            {
              assoc = (inf_assoc *) chainx->DATA;
              if (strcasecmp(assoc->orig, var)==0) break;
            }

          if ((chainx!=NULL && !set) || (chainx==NULL && set) || (set && strcasecmp(assoc->dest, buf)!=0))
            avt_errmsg(STB_ERRMSG, "040", AVT_ERROR, var, !set?"<not set>":buf);
        }  
    }

  return 0;
}
                

static void stb_checksetfalseaccess(stbfig_list *stbfig)
{
  ttvsig_list *tvs;
  chain_list *cl;
  for (cl=stbfig->CLOCK; cl!=NULL; cl=cl->NEXT)
  {
    tvs=(ttvsig_list *)cl->DATA;
    if(ttv_canbeinfalsepath(tvs->NODE, 'i') || ttv_canbeinfalsepath(tvs->NODE+1, 'i'))
     {
       stbfig->FLAGS|=STB_HAS_FALSE_ACCESS;
       break;
     }
  }
}

stbfig_list *
stb_parseSTO(ttvfig_list *ttvfig, char *filename)
{
  stbfig_list *stbfig ;
  chain_list *chain ;
  chain_list *chainsig ;
  stb_parse *stab;
  inffig_list *ifl, *origifl;
  ttvsig_list *tvs;
  stbnode *node;
  double val;
  char namebuf[1024];
  int flag, res;

  if ((stbfig=stb_getstbfig(ttvfig))!=NULL) stb_delstbfig(stbfig);

  ttv_disablecache(ttvfig) ;
  
  stbfig                = stb_addstbfig(ttvfig) ;
  stbfig->ANALYSIS      = STB_ANALYSIS_VAR;
  stbfig->CTKMODE       = STB_CTK_VAR;
  if ((ttv_getloadedfigtypes(ttvfig) & TTV_FILE_DTX)==TTV_FILE_DTX)     
    stbfig->GRAPH         = STB_DET_GRAPH;
  else
    stbfig->GRAPH         = STB_RED_GRAPH;
  stbfig->CONNECTOR     = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_C, NULL) ;
  stbfig->COMMAND       = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_Q, NULL) ;
  stbfig->MEMORY        = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_L, NULL) ;
  stbfig->PRECHARGE     = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_R, NULL) ;
  stbfig->BREAK         = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_B, NULL) ;
  stbfig->STABILITYMODE = STB_MODE_VAR;

  ttv_getinffile(ttvfig);
  origifl=getloadedinffig(ttvfig->INFO->FIGNAME);
  
  inf_PushAndHideInf(ttvfig->INFO->FIGNAME);

  stb_init(stbfig) ;
  ifl=_infRead(ttvfig->INFO->FIGNAME, filename, INF_DONTLOG);

  stb_checkconfigsto(origifl, ifl);
        
  stab = stb_convert_inf_to_stb_1(stbfig) ;

  stb_setfalseslack(stbfig, origifl);
  
  stbfig->NODE=reverse(stbfig->NODE);
  for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
    stb_init_backward (stbfig, (ttvevent_list *)chain->DATA);
  stbfig->NODE=reverse(stbfig->NODE);
  
  for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
    {
     stb_initstbnode(stbfig,(ttvevent_list *)chain->DATA) ;
/*     if(stbfig->GRAPH == STB_DET_GRAPH)
       stb_initckpath(stbfig,(ttvevent_list *)chain->DATA) ;*/
    }
 if(stbfig->GRAPH == STB_DET_GRAPH)
   stb_propagate_signal(stbfig, stbfig->CLOCK);

  stbfig->FLAGS|=STB_CLOCK_INFO_READY;
  stb_convert_inf_to_stb_2(stbfig, stab) ;
  stbfig->STABILITYFLAG = STB_STABLE ;

  stb_computedelaymargin(stbfig);

  chainsig = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_TYPEALL/*C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B*/, NULL) ;
  for(chain = chainsig; chain ; chain = chain->NEXT)
  {
      tvs=(ttvsig_list*)chain->DATA;
      ttv_getsigname(stbfig->FIG, namebuf, tvs);

      node=stb_getstbnode(tvs->NODE);
      if (inf_GetDouble(ifl, namebuf, INF_FALL_HOLD, &val)) node->HOLD=val*1e12*TTV_UNIT;
      if (inf_GetDouble(ifl, namebuf, INF_FALL_SETUP, &val)) node->SETUP=val*1e12*TTV_UNIT;
      if((node->HOLD!=STB_NO_TIME && node->HOLD <= 0) || (node->SETUP!=STB_NO_TIME && node->SETUP <= 0))
         stbfig->STABILITYFLAG = STB_UNSTABLE;
      if ((res=inf_GetInt(ifl, namebuf, INF_STB_NODE_FLAGS, &flag))!=0)
      {
        if ((flag & (INF_NODE_FLAG_STABCORRECT<<16))!=0)
        {
          stb_assign_paths(stbfig, node->EVENT);
          node->FLAG|=STB_NODE_STABCORRECT;
        }
      }

      
      node=stb_getstbnode(tvs->NODE+1);
      if (inf_GetDouble(ifl, namebuf, INF_RISE_HOLD, &val)) node->HOLD=val*1e12*TTV_UNIT;
      if (inf_GetDouble(ifl, namebuf, INF_RISE_SETUP, &val)) node->SETUP=val*1e12*TTV_UNIT;
      if((node->HOLD!=STB_NO_TIME && node->HOLD <= 0) || (node->SETUP!=STB_NO_TIME && node->SETUP <= 0))
         stbfig->STABILITYFLAG = STB_UNSTABLE;
      if (res)
      {
        if ((flag & INF_NODE_FLAG_STABCORRECT)!=0)
        {
          stb_assign_paths(stbfig, node->EVENT);
          node->FLAG|=STB_NODE_STABCORRECT;
        }
      }

  }
  freechain(chainsig);

  delinffig(ttvfig->INFO->FIGNAME);
  inf_PopInf();
  cleanStartForNode_HT();
  stb_update_ttv_clocks(ttvfig, stbfig);
  stb_checksetfalseaccess(stbfig);
  return stbfig;
}

static void stb_final_update_setup_and_hold(stbfig_list *stbfig)
{
  chain_list *chainsig, *chain;
  ttvsig_list *tvs;
  stbdebug_list *sdl;
  stbnode *node;
 
  stbfig->STABILITYFLAG = STB_STABLE;
  chainsig = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_TYPEALL/*C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B*/, NULL) ;
  for(chain = chainsig; chain ; chain = chain->NEXT)
  {
      tvs=(ttvsig_list*)chain->DATA;
      sdl = stb_debugstberror(stbfig, tvs, 0, STB_UPDATE_SETUP_HOLD);
      stb_freestbdebuglist(sdl);
      node=stb_getstbnode(tvs->NODE);
      if((node->HOLD!=STB_NO_TIME && node->HOLD <= 0) || (node->SETUP!=STB_NO_TIME && node->SETUP <= 0))
         stbfig->STABILITYFLAG = STB_UNSTABLE;
      node=stb_getstbnode(tvs->NODE+1);
      if((node->HOLD!=STB_NO_TIME && node->HOLD <= 0) || (node->SETUP!=STB_NO_TIME && node->SETUP <= 0))
         stbfig->STABILITYFLAG = STB_UNSTABLE;
  }
  freechain(chainsig);
}

static int stb_driveconfigforssta(char *filename)
{
   char *varlist[]=
   {
     "stbMonoPhase",
     "stbCreateEquivGroups",
     "stbSyncEdges",
     "stbStabilityCorrection",
     "stbEnableCommandCheck",
     "stbWorstCaseAnalysis",
     "stbTopLevelPath",
     "stbSetupOnly",
     "stbHoldOnly",
     "stbDetailedAnalysis",
     "stbPrechargeDataHeuristic",
     "avtTransparentPrecharge",
     "avtMaxPathPeriodDepth"
   };
   FILE *f;
   unsigned int i;
   if ((f=fopen(filename,"w"))!=NULL)
    {
       for (i=0; i<sizeof(varlist)/sizeof(*varlist); i++)
         fprintf(f, "avt_config %s \"%s\"\n", varlist[i], avt_gethashvar_sub(varlist[i],1)); 
       fclose(f);
    }
   return f!=NULL;
}
/*}}}************************************************************************/
/*****************************************************************************
*                           fonction stb_analysis                            *
*****************************************************************************/
stbfig_list *stb_analysis(ttvfig,analysemode,graphmode,stabilitymode,ctkmode)
ttvfig_list *ttvfig ;
char analysemode ;
char graphmode ;
char stabilitymode ;
char ctkmode ;
{
 stbfig_list *stbfig ;
 chain_list *chain ;
 long i ;
 int oldprecisionlevel;
 inffig_list *ifl;
 
#ifdef AVERTEC_LICENSE
 if(avt_givetoken("HITAS_LICENSE_SERVER", STB_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

 oldprecisionlevel=ttv_SetPrecisionLevel(0);

 if ((stbfig=stb_getstbfig(ttvfig))!=NULL) stb_delstbfig(stbfig);
 
 ttv_getinffile(ttvfig);

 if (MBK_SOCK!=-1 && (ifl=getloadedinffig(ttvfig->INFO->FIGNAME))!=NULL)
 {
   char filename[1024];
   sprintf(filename,"%s.ssta.stb.inf",ttvfig->INFO->FIGNAME);
   infDrive(ifl, filename, INF_LOADED_LOCATION, NULL);
   mbk_comm_send_text_file(filename);
   sprintf(filename,"%s.ssta.stb.tcl",ttvfig->INFO->FIGNAME);
   if (stb_driveconfigforssta(filename))
   {
     mbk_comm_send_text_file(filename);
   }
 }
 
 stbfig = stb_addstbfig(ttvfig) ;
 stbfig->ANALYSIS = analysemode ;
 stbfig->CTKMODE = ctkmode ;
 stbfig->GRAPH = graphmode ;
 stbfig->CONNECTOR = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_C, NULL) ;
 stbfig->COMMAND = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_Q, NULL) ;
 stbfig->MEMORY = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_L, NULL) ;
 stbfig->PRECHARGE = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_R, NULL) ;
 stbfig->BREAK = ttv_getsigbytype(stbfig->FIG, NULL, TTV_SIG_B, NULL) ;
 stbfig->STABILITYMODE = stabilitymode ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat (NULL, 1);
#endif

 stb_init(stbfig) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb init:", 0);
#endif

 stb_loadstbfig(stbfig) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb load:", 0);
#endif

 stbfig->NODE=reverse(stbfig->NODE);
 for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
   stb_init_backward (stbfig, (ttvevent_list *)chain->DATA);
 stbfig->NODE=reverse(stbfig->NODE);
  
 for(chain = stbfig->NODE; chain ; chain = chain->NEXT)
   {
    if(stbfig->GRAPH == STB_DET_GRAPH)
        i = TTV_FIND_LINE;
    else
        i = TTV_FIND_PATH;
        
    if((stbfig->CTKMODE & STB_PROPAGATE) == STB_PROPAGATE) {
       ttv_calcnodedelayslope(stbfig->FIG,stbfig->FIG->INFO->LEVEL,
                              (ttvevent_list *)chain->DATA,
                              i|TTV_FIND_MIN|TTV_FIND_MAX,TTV_MODE_LINE) ;
    }
    stb_initstbnode(stbfig,(ttvevent_list *)chain->DATA) ;
/*    if(stbfig->GRAPH == STB_DET_GRAPH)
      stb_initckpath(stbfig,(ttvevent_list *)chain->DATA) ;*/
   }

 if(stbfig->GRAPH == STB_DET_GRAPH)
   stb_propagate_signal(stbfig, stbfig->CLOCK);

 stbfig->FLAGS|=STB_CLOCK_INFO_READY;
 stb_checksetfalseaccess(stbfig);

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb initnode, initckpath:", 0);
#endif

 if((stbfig->CTKMODE & STB_CTK) == STB_CTK) {
    i = stb_ctk(stbfig) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb ctk:", 0);
#endif
 }
 
 i = stb_relaxation(stbfig) ;

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb relaxation:", 0);
#endif
 
#ifdef AVERTEC_LICENSE
 if(avt_givetoken("HITAS_LICENSE_SERVER", STB_TOOLNAME)!=AVT_VALID_TOKEN) EXIT(1);
#endif

 if (V_BOOL_TAB[__STB_SETUP_HOLD_UPDATE].VALUE)
   stb_final_update_setup_and_hold(stbfig);
   
#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb update setup/hold:", 0);
#endif
 
 if (MBK_SOCK!=-1 && (stbfig->CTKMODE & STB_CTK) == STB_CTK)
   {
     ttv_sendttvdelays(stbfig->FIG, 1, TAS_PVT_COUNT);
#ifdef DELAY_DEBUG_STAT
     mbk_debugstat ("ssta: send ctk delays:", 0);
#endif
   }
 stb_computedelaymargin(stbfig);

#ifdef DELAY_DEBUG_STAT
 mbk_debugstat ("stb delay margin:", 0);
#endif
 
 if (stbfig != NULL) {
     if(STB_OUT == 'Y')
         stb_savestbfig(stbfig, STB_DRIVE_INTERNALS, STB_SEPARATE_INTERVALS, STB_SUFFIX_STO);
     if(STB_REPORT == 'Y')
         stb_report(stbfig, NULL);
#ifdef DELAY_DEBUG_STAT
     mbk_debugstat ("stb reports:", 0);
#endif
 }

 ttv_SetPrecisionLevel(oldprecisionlevel);
 
 ttv_freefigdelay(stbfig->NODE) ;

 // pour les delay sur les connecteurs d'entree
 ttv_getinffile(ttvfig);
 //--
 stm_pwl_finish();
 return(stbfig) ;
}
