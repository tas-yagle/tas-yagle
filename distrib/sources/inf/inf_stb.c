/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : inf_stb.c                                                   */
/*                                                                          */
/*    © copyright 2004 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include MUT_H
#include "inf_stb.h"

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    inf_stb_parse_spec_alloc()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
inf_stb_p_s *
inf_stb_parse_spec_alloc(inf_stb_p_s *next)
{
  inf_stb_p_s   *res;

  res   = mbkalloc(sizeof(inf_stb_p_s));
  res->NEXT         = next;
  res->STABILITY    = NULL;
  res->CKNAME       = NULL;
  res->CKEDGE       = -1;
  res->SPECTYPE     = -1;
  res->DATAEDGE     = -1;
  res->HZOPTION     = -1;
  return res;
}

/*}}}************************************************************************/
/*{{{                    inf_stb_parse_spec_stab_alloc()                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
inf_stb_p_s_stab *
inf_stb_parse_spec_stab_alloc(inf_stb_p_s_stab *next)
{
  inf_stb_p_s_stab  *res;
  
  res   = mbkalloc(sizeof(inf_stb_p_s_stab));
  res->NEXT         = next;
  res->TIME_LIST    = NULL;
  res->CKNAME       = NULL;
  res->CKEDGE       = -1;
  res->RELATIVITY   = -1;
  res->DELAY        = -1;
  return res;
}

/*}}}************************************************************************/
/*{{{                    inf_stb_parse_spec_free()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
inf_stb_parse_spec_free(inf_stb_p_s *pt)
{
  while (pt)
    pt  = inf_stb_parse_spec_del(pt);
}

/*}}}************************************************************************/
/*{{{                    inf_stb_parse_spec_stab_free()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
inf_stb_parse_spec_stab_free(inf_stb_p_s_stab *pt)
{
  while (pt)
    pt  = inf_stb_parse_spec_stab_del(pt);
}

/*}}}************************************************************************/
/*{{{                    inf_stb_parse_spec_del()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
inf_stb_p_s *
inf_stb_parse_spec_del(inf_stb_p_s *pt)
{
  inf_stb_p_s   *next;
  
  next      = pt->NEXT;
  inf_stb_parse_spec_stab_free(pt->STABILITY);
  mbkfree(pt);

  return next;
}

/*}}}************************************************************************/
/*{{{                    inf_stb_parse_spec_stab_del()                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
inf_stb_p_s_stab *
inf_stb_parse_spec_stab_del(inf_stb_p_s_stab *pt)
{
  inf_stb_p_s_stab  *next;
  chain_list *cl;
  next  = pt->NEXT;
  for (cl=pt->TIME_LIST; cl!=NULL; cl=cl->NEXT) mbkfree(cl->DATA);
  freechain(pt->TIME_LIST);
  mbkfree(pt);

  return next;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
inf_stb_p_s * inf_stb_spec_exists(inf_stb_p_s *pt, inf_stb_p_s *list)
{
  while (list!=NULL)
    {
      if (list->CKNAME==pt->CKNAME
          && list->CKEDGE==pt->CKEDGE
          && list->SPECTYPE==pt->SPECTYPE
          && list->DATAEDGE==pt->DATAEDGE
          && list->HZOPTION==pt->HZOPTION)
        return list;
      list=list->NEXT;
    }

  return NULL;
}
