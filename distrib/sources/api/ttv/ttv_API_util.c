/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : ttv_API_util.c                                              */
/*                                                                          */
/*    © copyright 2004 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

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

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    isSameNode()                                       */
/*                                                                          */
/* Compare 2 ttv path                                                       */
/*                                                                          */
/****************************************************************************/
static int
isSameNode(ttvcritic_list *x, ttvcritic_list *y)
{
  return (!strcmp(x->NAME,y->NAME) &&
          (x->SLOPE == y->SLOPE) &&
          (x->DELAY == y->DELAY) &&
          (x->TYPE == y->TYPE) &&
          (x->SNODE == y->SNODE));
}

/*}}}************************************************************************/
/*{{{                    pathIsInside()                                     */
/*                                                                          */
/* detect if a pth is included onto a longer path                           */
/*                                                                          */
/****************************************************************************/
static int 
pathIsInside(ttvpath_list* sp,ttvpath_list *lp)
{
  chain_list        *x, *y;
  ttvcritic_list    *sc, *lc;
  int                res;
  
  if (sp && lp)
  {
    res             = 1;
    
    x               = ttv_GetPathDetail(sp);
    y               = ttv_GetPathDetail(lp);

    sc              = x->DATA;
    for ( ; y; y = y->NEXT)
    {
      lc            = y->DATA;
      if (!strcmp(lc->NAME,sc->NAME))
        break;
    }
    
    if (y)
      for ( ; x && y && res; x = x->NEXT, y = y->NEXT)
      {
        res           = isSameNode(x->DATA,y->DATA);
        //if (!res) printf("differences\n");
      }

    if (!y && x)
      return 0;
    else
      return res;
  }   
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    ttv_RemoveIncludedSmallerPath()                    */
/*                                                                          */
/* remove included path                                                     */
/*                                                                          */
/****************************************************************************/
chain_list *
ttv_RemoveIncludedSmallerPath(chain_list *lpl, chain_list *spl)
{
  chain_list    *x, *y, hd, *prev;
  int            res;
  
  hd.NEXT           = spl;
  prev              = &hd;
  
  if (lpl && spl)
    for (x = prev->NEXT; x; )
    {
      //res           = 0;
      for (y = lpl; y; y = y->NEXT)
        if ((res = pathIsInside(x->DATA,y->DATA)))
          break;
      if (res)
      {
        x           = delchain(x,x);
        prev->NEXT  = x;
      }
      else
      {
        x           = x->NEXT;
        prev        = prev->NEXT;
      }
    }
  
  return hd.NEXT;
}

/*}}}************************************************************************/

cnsfig_list *ttvutil_cnsload(char *figname, inffig_list *ifl)
{
  cnsfig_list *cf;
  char *where;
  if ((cf=getloadedcnsfig(figname))==NULL)
    {
      cnsenv();
      where=filepath (figname,"cns");
      if (where!=NULL)
        {
          avt_log(LOGFILEACCESS, 0, "Loading Cone netlist \"%s\"\n", where);
          //avt_error("library", -1, AVT_INFO, "loading CNS '¤2%s¤.'\n", where);
          cf=getcnsfig(figname, NULL);
          cns_addmultivoltage(ifl, cf);
        }
    }
  return cf;
}

char ttv_getUorD(char dir)
{
  if (tolower(dir) == 'u' || tolower(dir) == 'r') return 'u';
  else if (tolower(dir) == 'd' || tolower(dir) == 'f') return 'd';
  return dir;
}

int ttv_DirectionToIndex(char dir)
{
    if (tolower(dir) == 'u' || tolower(dir) == 'r') return TTVAPI_UP;
    else if (tolower(dir) == 'd' || tolower(dir) == 'f') return TTVAPI_DN;
    else if (dir == '?') return TTVAPI_EITHER;
    return TTVAPI_NONE;
}

void ttv_DirectionStringToIndices(char *dir, int *a, int *b)
{
    *a = ttv_DirectionToIndex(dir[0]);
    *b = ttv_DirectionToIndex(dir[1]);
}

int ttv_DirectionStringToIndicesHZ(char *dir)
{
  if (dir[1]!='\0')
  {
    if (tolower(dir[2])=='z') return TTV_API_ONLY_HZ;
    else if (tolower(dir[2])=='/') return TTV_API_ONLY_NOTHZ;
  }
  return 0;
}


