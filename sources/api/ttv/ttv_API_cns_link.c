#include MUT_H
#include MLO_H
#include MLU_H
#include CNS_H
#include STM_H
#include TTV_H

#include "ttv_API_util.h"

static chain_list *findtr(branch_list *br, long typemask, char *namein, chain_list *cl)
{
  chain_list *ch;
  link_list *lk;
  while (br!=NULL)
    {
      for (lk=br->LINK; lk!=NULL; lk=lk->NEXT)
        {
          if ((lk->TYPE & CNS_EXT) != CNS_EXT && 
              (lk->ULINK.LOTRS->TYPE & typemask)==typemask &&
              lk->ULINK.LOTRS->GRID->SIG->NAMECHAIN->DATA==namein &&
              lk->ULINK.LOTRS->TRNAME!=NULL
              ) 
            {
              for (ch=cl; ch!=NULL; ch=ch->NEXT)
                if (ch->DATA==lk->ULINK.LOTRS) break;
              if (ch==NULL) cl=addchain(cl, lk->ULINK.LOTRS);
            }
        }
      br=br->NEXT;
    }
  return cl;
}

static chain_list *ttv_cnsgettransistor(cnsfig_list *cf, char *namein, char dirin, char *nameout, char dirout, int hz)
{
  cone_list *cn;
  long transistortypemask;
  branch_list *brtosearch;
  chain_list *cl;

  if (tolower(dirin)!='u' && tolower(dirin)!='d')
    {
      avt_errmsg(TTV_API_ERRMSG, "001", AVT_ERROR, dirin);
/*      avt_error("ttvapi", 4, AVT_ERR, "invalid direction for input signal '%c', function call ignored.\n", dirin);
      avt_error("ttvapi", 4, AVT_INFO, "a direction should be 'u' or 'd'.\n");*/
      return NULL;
    }
  if (tolower(dirout)!='u' && tolower(dirout)!='d')
    {
      avt_errmsg(TTV_API_ERRMSG, "001", AVT_ERROR, dirout);
/*      avt_error("ttvapi", 4, AVT_ERR, "invalid direction for output signal '%c', function call ignored.\n", dirout);
      avt_error("ttvapi", 4, AVT_INFO, "a direction should be 'u' or 'd'.\n");*/
      return NULL;
    }

  if ((cn=getcone(cf, 0, nameout))==NULL) return NULL;
  
  if (!hz)
    {
      // transistor passant
      if (dirin=='u') transistortypemask=TRANSN; else transistortypemask=TRANSP;
    }
  else
    {
      // transistor bloquant
      if (dirin=='u') transistortypemask=TRANSP; else transistortypemask=TRANSN;
    }

  if (dirout=='u') brtosearch=cn->BRVDD; else brtosearch=cn->BRVSS;

  cl=findtr(cn->BREXT, transistortypemask, namein, NULL);
  return findtr(brtosearch, transistortypemask, namein, cl);
}

chain_list *__ttv_GetCrossedTransistors(ttvfig_list *tvf, char *namein, char dirin, char *nameout, char dirout, int hzflag)
{
  cnsfig_list *cf;
  chain_list *cl;

  cf=ttvutil_cnsload(tvf->INFO->FIGNAME, getloadedinffig(tvf->INFO->FIGNAME));

  if (cf==NULL) return NULL;

  cl=ttv_cnsgettransistor(cf, namealloc(namein), dirin, namealloc(nameout), dirout, hzflag);
  if (cl==NULL)
    {
      namein=mbk_devect(namein, "[", "]");
      nameout=mbk_devect(nameout, "[", "]");
      cl=ttv_cnsgettransistor(cf, namein, dirin, nameout, dirout, hzflag);
    }
  return cl;
}

chain_list *ttv_GetCrossedTransistorNames(ttvfig_list *tvf, char *namein, char dirin, char *nameout, char dirout, int hzflag)
{
  chain_list *cl, *ch;
  cl=__ttv_GetCrossedTransistors(tvf, namein, dirin, nameout, dirout, hzflag);
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      ch->DATA=((lotrs_list *)ch->DATA)->TRNAME;
    }
  return cl;
}
