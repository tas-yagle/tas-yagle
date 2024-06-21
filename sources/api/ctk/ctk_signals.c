#include <stdio.h>
#include <ctype.h>
#include AVT_H
#include "api_communication.h"
#include GEN_H
#include STB_H
#include RCN_H
#include TRC_H
#include "api_common_structures.h"
#define API_USE_REAL_TYPES
#include "gen_API.h"
#include "ctk_api_local.h"
#include "ctk_API.h"

chain_list *ctk_GetAllVictims()
{
  chain_list *cl=NULL;
  losig_list *ls, *cls;
  
  for (ls=CTK_NETLIST->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      cls=gns_GetCorrespondingSignal(gns_GetSignalName(ls));
      if (cls!=NULL) cl=addchain(cl, cls);
    }
  return cl;
}

double ctk_GetTotalGroundCapa(losig_list *ls)
{
  int code;
  float capa;
  if (CTK_NETLIST==NULL)
    {
      avt_errmsg(CTK_API_ERRMSG, "004", AVT_WARNING, gen_info());
//      fprintf(stderr,"%s: The CTK_NETLIST is not set, function ignored", gen_info());
      return 0.0;
    }
  code=rcxi_getcapa(GENIUS_GLOBAL_LOFIG, ls, RCXI_UP, 0.0, RCXI_GND, &capa);
  if (code==0)
    {
      avt_errmsg(CTK_API_ERRMSG, "005", AVT_ERROR, gen_info());
//      fprintf(stderr,"%s: Error occured when calling function", gen_info());
    }
  return capa;
}

double ctk_GetTotalResi(losig_list *ls)
{
  int code;
  float resi;
  if (CTK_NETLIST==NULL)
    {
      avt_errmsg(CTK_API_ERRMSG, "004", AVT_WARNING, gen_info());
      //fprintf(stderr,"%s: The CTK_NETLIST is not set, function ignored", gen_info());
      return 0.0;
    }
  code=rcxi_getresi(GENIUS_GLOBAL_LOFIG, ls, RCXI_ALLWIRE, &resi);
  if (code==0)
    {
      //fprintf(stderr,"%s: Error occured when calling function", gen_info());
      avt_errmsg(CTK_API_ERRMSG, "005", AVT_ERROR, gen_info());
    }
  return resi;
}

double ctk_GetTotalCrosstalkCapa(losig_list *ls, char type, char dir, double slope)
{
  int code, tp=RCXI_MAX;
  float capa;
  char dir0=RCXI_UP;
  
  if (CTK_NETLIST==NULL)
    {
      avt_errmsg(CTK_API_ERRMSG, "004", AVT_WARNING, gen_info());
      //fprintf(stderr,"%s: The CTK_NETLIST is not set, function ignored\n", gen_info());
      return 0.0;
    }
  switch(tolower(dir))
    {
    case 'u' : dir0 = RCXI_UP; break; 
    case 'd' : dir0 = RCXI_DW; break;
    default : //fprintf(stderr,"%s: Unknown direction '%c' using 'u'\n", gen_info(), dir);
      avt_errmsg(CTK_API_ERRMSG, "006", AVT_ERROR, gen_info(), dir);
    }
  switch(tolower(type))
    {
    case 'w' : tp = RCXI_MAX; break; 
    case 'b' : tp = RCXI_MIN; break;
    default : //fprintf(stderr,"%s: Unknown delay type '%c' using 'w'\n", gen_info(), type);
      avt_errmsg(CTK_API_ERRMSG, "007", AVT_ERROR, gen_info(), type);
    }
  code=rcxi_getcapa(GENIUS_GLOBAL_LOFIG, ls, dir0, slope, RCXI_CTK | tp, &capa);
  if (code==0)
    {
      //fprintf(stderr,"%s: Error occured when calling function", gen_info());
      avt_errmsg(CTK_API_ERRMSG, "005", AVT_ERROR, gen_info());
    }
  return capa;
}

double ctk_GetCouplingFactor(losig_list *ls, char type, char dir, double slope)
{
  double ctk_capa, gnd_capa;
  ctk_capa=ctk_GetTotalCrosstalkCapa(ls, type, dir, slope);
  gnd_capa=ctk_GetTotalGroundCapa(ls);
  if (ctk_capa+gnd_capa<1e-30) return 1.0;
  return ctk_capa/(ctk_capa+gnd_capa);
}

double ctk_GetMillerCapa(losig_list *lsv, losig_list *lsa, char type, char dir, double slope)
{
  int code, tp=RCXI_MAX, tp0=0;
  float capa;
  char dir0=RCXI_UP;
  
  if (CTK_NETLIST==NULL)
    {
      avt_errmsg(CTK_API_ERRMSG, "004", AVT_WARNING, gen_info());
      //fprintf(stderr,"%s: The CTK_NETLIST is not set, function ignored\n", gen_info());
      return 0.0;
    }
/*
  if (STB_COM!=NULL)
    tp0=RCXI_WITHSTB;
*/
  switch(tolower(dir))
    {
    case 'u' : dir0 = RCXI_UP; break; 
    case 'd' : dir0 = RCXI_DW; break;
    default : // fprintf(stderr,"%s: Unknown direction '%c' using 'u'\n", gen_info(), dir);
      avt_errmsg(CTK_API_ERRMSG, "006", AVT_ERROR, gen_info(), dir);
    }
  switch(tolower(type))
    {
    case 'w' : tp = RCXI_MAX; break; 
    case 'b' : tp = RCXI_MIN; break;
    default : // fprintf(stderr,"%s: Unknown delay type '%c' using 'w'\n", gen_info(), type);
      avt_errmsg(CTK_API_ERRMSG, "007", AVT_ERROR, gen_info(), type);
    }
  code=rcxi_getmiller(GENIUS_GLOBAL_LOFIG, lsv, lsa, dir0, slope, tp0 | tp, &capa);
  if (code==0)
    {
      avt_errmsg(CTK_API_ERRMSG, "005", AVT_ERROR, gen_info());
      //fprintf(stderr,"%s: Error occured when calling function", gen_info());
    }
  return capa;
}
#if 0
void ctk_GetPILoad(losig_list *lsv, losig_list *lsa, char type, char dir, double slope)
{
  int code, tp=RCXI_MAX, tp0=0;
  float capa;
  char dir0=RCXI_UP;
  
  if (CTK_NETLIST==NULL)
    {
      fprintf(stderr,"%s: The CTK_NETLIST is not set, function ignored\n", gen_info());
      return 0.0;
    }

  if (STB_COM!=NULL)
    tp0=RCXI_WITHSTB;

  switch(tolower(dir))
    {
    case 'u' : dir0 = RCXI_UP; break; 
    case 'd' : dir0 = RCXI_DW; break;
    default : fprintf(stderr,"%s: Unknown direction '%c' using 'u'\n", gen_info(), dir);
    }
  switch(tolower(type))
    {
    case 'w' : tp = RCXI_MAX; break; 
    case 'b' : tp = RCXI_MIN; break;
    default : fprintf(stderr,"%s: Unknown delay type '%c' using 'w'\n", gen_info(), type);
    }
  code=rcxi_getmiller(GENIUS_GLOBAL_LOFIG, lsv, lsa, dir0, slope, tp0 | tp, &capa);
  if (code==0)
    {
      fprintf(stderr,"%s: Error occured when calling function", gen_info());
    }
  return capa;
}
#endif
