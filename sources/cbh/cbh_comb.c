/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_comb.c                                                  */
/*                                                                          */
/*    (c) copyright 2001 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Anthony LESTER                                          */
/*                                                                          */
/****************************************************************************/

#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include CBH_H

/****************************************************************************/
/*                cbh_getcombfunc                                           */
/****************************************************************************/
cbhcomb *cbh_getcombfunc(befig_list *ptcellbefig, cbhseq *ptcbhseq,
                         char *name)
{
  beout_list *ptbeout   = NULL;
  bebus_list *ptbebus   = NULL;
  cbhcomb    *ptcbhcomb = NULL;

  if (ptcellbefig->ERRFLG != 0)
    return NULL;

  if (ptcellbefig->CIRCUI == NULL)
  {
    beh_depend(ptcellbefig);
    if (ptcellbefig->BEAUX != NULL && cbh_suppressaux(ptcellbefig) != 0)
      return NULL;
    beh_indexbdd(ptcellbefig);
  }

  if ( ptcellbefig->BEBUX != NULL
       || (ptcellbefig->BEREG != NULL && ptcbhseq == NULL))
    return NULL;

  for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT)
    if (ptbeout->NAME == name)
      break;
  if (ptbeout != NULL)
  {
    ptcbhcomb               = cbh_newcomb();
    /* Handle case of latch output pins */
    if (ptcbhseq != NULL)
    {
      if (name == ptcbhseq->PIN)
        ptcbhcomb->FUNCTION = createAtom(ptcbhseq->NAME);
      if (name == ptcbhseq->NEGPIN)
        ptcbhcomb->FUNCTION = createAtom(ptcbhseq->NEGNAME);
    }
    ptcbhcomb->NAME         = name;
    if (ptcbhcomb->FUNCTION == NULL)
      ptcbhcomb->FUNCTION   = copyExpr(ptbeout->ABL);
  }
  else
    for (ptbebus = ptcellbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT)
      if (ptbebus->NAME == name)
        break;

  if (ptbebus != NULL)
  {
    ptcbhcomb               = cbh_newcomb();
    ptcbhcomb->NAME         = name;
    cbh_calchzfunc(ptcellbefig->CIRCUI, ptbebus->BIABL, ptcbhcomb);
    /* Handle case of latch output pins */
    if (ptcbhseq != NULL)
    {
      if (name == ptcbhseq->HZPIN)
      {
        freeExpr(ptcbhcomb->FUNCTION);
        ptcbhcomb->FUNCTION = createAtom(ptcbhseq->NAME);
      }
      if (name == ptcbhseq->HZNEGPIN)
      {
        freeExpr(ptcbhcomb->FUNCTION);
        ptcbhcomb->FUNCTION = createAtom(ptcbhseq->NEGNAME);
      }
    }
  }

  return ptcbhcomb;
}

