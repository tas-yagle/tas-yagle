/* ###--------------------------------------------------------------### */
/* file		: bvl_bspec.c						*/
/* date		: Jan 18 1993						*/
/* version	: v106							*/
/* authors	: Pirouz BAZARGAN SABET, VUONG Huu Nghia		*/
/* content	: contains all specific functions used to build		*/
/*		  behaviour data structures :				*/
/*		  bvl_addgen   , bvl_tobin , bvl_cpyabllst,		*/
/*		  bvl_cpyablstr, bvl_crtabl, bvl_select			*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include <string.h>
#include MUT_H
#include LOG_H
#include BEH_H

#include "bvl_util.h"
#include "bvl_byacc.h"
#include "bvl_bspec.h"
#include "bvl_bedef.h"

/* ###--------------------------------------------------------------### */
/* function	: bvl_addgen						*/
/* description	: create one or more BEGEN structures			*/
/*		  For a scalar a BEGEN is created at the head of	*/
/*		  existing BEGEN list.					*/
/*		  For an array (including arraies of one element) a set	*/
/*		  of BEGENs are created in a sorted list. BEGEN related	*/
/*		  to the index i of the array is named `name(i)`. The	*/
/*		  head of the list represents the right bound of the	*/
/*		  array. This list is then chained to the head of	*/
/*		  existing BEGEN list.					*/
/* called func.	: beh_addbegen, namealloc				*/
/* ###--------------------------------------------------------------### */

struct begen *
bvl_addgen (struct begen *lastgeneric, chain_list *nat_lst, chain_list *nam_lst, char *type, short left, short right)

    /* pointer on the last begen structure	*/
    /* generic's value list			*/
    /* generic's name list			*/
    /* generic's type			*/
    /* array's left bound (= -1 if scalar)	*/
    /* array's right bound (= -1 if scalar)	*/

{
  char          extname[100];
  char         *name;
  struct begen *ptgen;
  struct chain *ptauxnam;
  struct chain *ptauxnat;
  int           i;
  int           inc = 1;

  ptgen    = lastgeneric;
  ptauxnam = nam_lst;
  ptauxnat = nat_lst;

  if ((left == -1) && (right == -1))
    if ((ptauxnat != NULL) && (ptauxnat->NEXT == NULL))
      while (ptauxnam != NULL)
        {
        name     = namealloc((char *)ptauxnam->DATA);
        ptgen    = beh_addbegen (ptgen,name,type,(void *)ptauxnat->DATA);
        ptauxnam = ptauxnam->NEXT;
        }
    else
      bvl_error(75,NULL);
  else
    {
    if (left >= right)
      inc = -1;
    while (ptauxnam != NULL)
      {
      for (i=left ; i!=(right+inc) ; i+=inc)
        {
        sprintf (extname,"%s %d",(char *)ptauxnam->DATA,i);
        name = namealloc(extname);
        if (ptauxnat != NULL)
          {
          ptgen    = beh_addbegen (ptgen,name,type,(void *)ptauxnat->DATA);
	  ptauxnat = ptauxnat->NEXT;
          }
        else
          bvl_error(75,NULL);
        }
      if (ptauxnat != NULL)
        bvl_error (75,NULL);
      ptauxnat = nat_lst;
      ptauxnam = ptauxnam->NEXT;
      }
    }
  return (ptgen);
}

/* ###--------------------------------------------------------------### */
/* function	: bvl_cpyabllst						*/
/* description	: duplicate bvl_abllst structure 			*/
/* called func.	: addchain, reverse, copyExpr				*/
/* ###--------------------------------------------------------------### */

chain_list *
bvl_cpyabllst (chain_list *abllst)
{
  struct chain *ptabllst = NULL;

  while (abllst != NULL)
    {
    ptabllst = addchain (ptabllst,copyExpr((struct chain *)abllst->DATA));
    abllst    = abllst->NEXT;
    }
  ptabllst = reverse (ptabllst);
  return (ptabllst);
  }

/* ###--------------------------------------------------------------### */
/* function	: bvl_cpyablstr						*/
/* description	: duplicate bvlexpr structure 			*/
/* called func.	: mbkalloc <mbk>, bvl_cpyabllst				*/
/* ###--------------------------------------------------------------### */

bvlexpr 
bvl_cpyablstr (bvlexpr ablstr)
{
  bvlexpr ptablstr;

  ptablstr.IDENT    = NULL;
  ptablstr.WIDTH    = ablstr.WIDTH;
  ptablstr.LIST_ABL = bvl_cpyabllst (ablstr.LIST_ABL);
  return (ptablstr);
}

/* ###--------------------------------------------------------------### */
/* function     : bvl_crtabl                                            */
/* description  : combine at most two ABLs and build a new one          */
/*    		  The following operations can be performed :		*/
/*		    CONC    perform concatenation			*/
/*		    NOPI    initialize a structure for a signal (scalar	*/
/*		            or array)					*/
/*		    NOPS    initialize a structure for a literal	*/
/*		    NE      create a structure with an ABL representing	*/
/*		            the 'non equality' of two expressions	*/
/*		    EQ      create a structure with an ABL representing	*/
/*		            the 'equality' of two expressions		*/
/*		    NOT     perform logical not of an expression	*/
/*		    AND     perform logical and  between two expressions*/
/*		    OR      perform logical or   between two expressions*/
/*		    NAND    perform logical nand between two expressions*/
/*		    NOR     perform logical nor  between two expressions*/
/*		    XOR     perform logical xor  between two expressions*/
/*		    ANDM    perform logical and  between two expressions*/
/*		            (the second expression is a scalar)		*/
/* called func. : createAtom , createExpr, addQExpr , beh_toolbug,	*/
/*		  bvl_error , addchain   , freechain			*/
/* ###--------------------------------------------------------------### */

bvlexpr 
bvl_crtabl (short oper, bvlexpr expr1, bvlexpr expr2, int left, int right)
{
  char            name[256];
  char           *name2;
  struct chain   *ptabl1;
  struct chain   *ptabl2;
  struct chain   *ptaux1;
  struct chain   *ptaux2;
  bvlexpr      result;
  char            lcl_buffer[256];
  short           inc;
  short           i;
  char            true_flag_un   = 0;
  char            true_flag_zero = 0;
  struct chain   *abl_un         = createAtom("'1'");
  struct chain   *abl_zero       = createAtom("'0'");

  result.IDENT    = NULL;
  result.TIME     = 0   ;
  result.LIST_ABL = NULL;
  result.WIDTH    = 0   ;

  switch (oper)
  {
    case CONC :
      if ((expr1.LIST_ABL == NULL) || (expr2.LIST_ABL == NULL))
        beh_toolbug (4,"bvl_crtabl",NULL,0);
      else
      {
        if (expr1.LIST_ABL == expr2.LIST_ABL)
          beh_toolbug (16,"bvl_crtabl",NULL,0);
        else
        {
          ptaux2 = expr2.LIST_ABL;
          while (ptaux2->NEXT != NULL)
            ptaux2 = ptaux2->NEXT;

          ptaux2->NEXT = expr1.LIST_ABL;

          result.LIST_ABL = expr2.LIST_ABL;
          result.WIDTH    = expr1.WIDTH + expr2.WIDTH;

          expr1.LIST_ABL  = NULL;
          expr2.LIST_ABL  = NULL;
        }
      }
      break;

    case NOPI :
      if ( expr1.IDENT == NULL )
        beh_toolbug (2,"bvl_crtabl",NULL,0);
      else
      {
        if ((left == -1) && (right == -1))
        {
          result.LIST_ABL = addchain(result.LIST_ABL,createAtom(expr1.IDENT));
          result.WIDTH    = 1;
        }
        else
        {
          if (left <= right)
          {
            inc = 1;
            result.WIDTH = right - left + 1;
          }
          else
          {
            inc = -1;
            result.WIDTH = left - right + 1;
          }

          for (i=left ; i!=(right+inc) ; i+=inc)
          {
            sprintf (name,"%s %i",expr1.IDENT,i);
            name2           = namealloc (name);
            result.LIST_ABL = addchain (result.LIST_ABL,createAtom(name2));
          }
        }
        expr1.IDENT = NULL;
      }
      break;

    case NOPS :
      if ( expr1.IDENT == NULL )
        beh_toolbug (2,"bvl_crtabl",NULL,0);
      else
      {
        bvl_tobin (lcl_buffer,expr1.IDENT,-1,-1);
        if ((left == -1) && (right == -1))
        {
          left = 0;
          right = strlen (lcl_buffer) - 1;
        }

        for (i=left ; i<=right ; i++)
        {
          switch ( lcl_buffer[i] )
          {
            case '0' :
              result.LIST_ABL = addchain (result.LIST_ABL,createAtom("'0'"));
              break;
            case '1' :
              result.LIST_ABL = addchain (result.LIST_ABL,createAtom("'1'"));
              break;
            case 'z' :
              result.LIST_ABL = addchain (result.LIST_ABL,createAtom("'Z'"));
              break;
            case 'u' :
              result.LIST_ABL = addchain (result.LIST_ABL,createAtom("'U'"));
              break;
              /*-----------        Beware Not VHDL        -------------*/
            case 'd' :
              result.LIST_ABL = addchain (result.LIST_ABL,createAtom("'D'"));
              break;
            default  :
              beh_toolbug (15,"bvl_crtabl",NULL,expr1.IDENT[i]);
          }
        }
        result.WIDTH = right - left + 1;
      }
      break;

    case STABLE :
      if (expr1.LIST_ABL == NULL)
        beh_toolbug (3,"bvl_crtabl",NULL,0);
      else
      {
        ptaux1  = expr1.LIST_ABL;
        while (ptaux1 != NULL)
        {
          ptabl1  = createExpr (STABLE);
          addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
          ptaux1->DATA = ptabl1;
          ptaux1  = ptaux1->NEXT;
        }

        result.LIST_ABL = expr1.LIST_ABL;
        result.WIDTH    = expr1.WIDTH;

        expr1.LIST_ABL  = NULL;
      }
      break;

    case NOT :
      if (expr1.LIST_ABL == NULL)
        beh_toolbug (3,"bvl_crtabl",NULL,0);
      else
      {
        ptaux1  = expr1.LIST_ABL;
        while (ptaux1 != NULL)
        {
          ptabl1  = createExpr (NOT);
          addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
          ptaux1->DATA = ptabl1;
          ptaux1  = ptaux1->NEXT;
        }

        result.LIST_ABL = expr1.LIST_ABL;
        result.WIDTH    = expr1.WIDTH;

        expr1.LIST_ABL  = NULL;
      }
      break;

    case EQ :
      if ((expr1.LIST_ABL == NULL) || (expr2.LIST_ABL == NULL))
        beh_toolbug (4,"bvl_crtabl",NULL,0);
      else
      {
        if (expr1.WIDTH != expr2.WIDTH)
        {
          bvl_error (38,NULL);
          ptabl2 = createAtom ("'1'");

          ptaux1 = expr1.LIST_ABL;
          while (ptaux1 != NULL)
          {
            freeExpr (ptaux1->DATA);
            ptaux1 = ptaux1->NEXT;
          }
          ptaux2 = expr2.LIST_ABL;
          while (ptaux2 != NULL)
          {
            freeExpr (ptaux2->DATA);
            ptaux2 = ptaux2->NEXT;
          }
        }
        else
        {
          ptaux1 = expr1.LIST_ABL;
          ptaux2 = expr2.LIST_ABL;
          /* If expr2 = '1' then return expr1 */
          while(ptaux2 != NULL)
          {
            true_flag_un = 1;
            if(!equalExpr(ptaux2->DATA,abl_un))
            {
              /* One abl not abl_un */
              true_flag_un = 0;
              break;
            }
            ptaux2 = ptaux2->NEXT;  
          }
          if(true_flag_un == 1)
          {
            ptabl1 = (struct chain *)ptaux1->DATA;
            ptaux1 = ptaux1->NEXT;
            if(ptaux1 != NULL)
            {
              ptabl2 = createExpr(AND);
              addQExpr(ptabl2,ptabl1);
              ptabl1 = ptabl2;
            }
            while(ptaux1 != NULL)
            {
              addQExpr(ptabl1,(struct chain *)ptaux1->DATA);
              ptaux1 = ptaux1->NEXT;
            }
            result.LIST_ABL = addchain(result.LIST_ABL,ptabl1);
            result.WIDTH    = 1;
            expr1.LIST_ABL  = NULL;
            expr2.LIST_ABL  = NULL;
          }
          /* If expr2 = '0' then return NOT(expr1) */
          ptaux2 = expr2.LIST_ABL;
          true_flag_zero = 0;
          while(ptaux2 != NULL)
          {
            true_flag_zero = 1;
            if(!equalExpr(ptaux2->DATA,abl_zero))
            {
              /* One abl not abl_zero */
              true_flag_zero = 0;
              break;
            }
            ptaux2 = ptaux2->NEXT;  
          }
          if(true_flag_zero == 1)
          {
            while(ptaux1 != NULL)
            {
              ptabl1 = createExpr(NOT);
              addQExpr(ptabl1,(struct chain *)ptaux1->DATA);
              ptaux1 = ptaux1->NEXT;
              if(ptaux1 != NULL)
              {
                ptabl2 = createExpr(AND);
                addQExpr(ptabl2,ptabl1);
                ptabl1 = ptabl2;
              }
              while(ptaux1 != NULL)
              {
                ptabl2 = createExpr(NOT);
                addQExpr(ptabl2,(struct chain *)ptaux1->DATA);
                addQExpr(ptabl1,ptabl2);
                ptaux1 = ptaux1->NEXT;
              }
              /* --ptabl1 = createExpr(NOT);
                 addQExpr(ptabl1,(struct chain *)ptaux1->DATA);
                 ptaux1->DATA = ptabl1;
                 ptaux1       = ptaux1->NEXT;
                 --- */
            }
            result.LIST_ABL = addchain(result.LIST_ABL,ptabl1);
            result.WIDTH    = 1;
            expr1.LIST_ABL  = NULL;
            expr2.LIST_ABL  = NULL;
          }


          ptaux2 = expr2.LIST_ABL;
          if((true_flag_zero == 0) && (true_flag_un == 0))
          {
            ptabl1 = createExpr (XOR);
            addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
            addQExpr (ptabl1, (struct chain *)ptaux2->DATA);

            ptaux1 = ptaux1->NEXT;
            ptaux2 = ptaux2->NEXT;

            while (ptaux1 != NULL)
            {
              ptabl2 = createExpr (OR);
              addQExpr (ptabl2,ptabl1);

              ptabl1 = createExpr (XOR);
              addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
              addQExpr (ptabl1, (struct chain *)ptaux2->DATA);

              addQExpr (ptabl2, ptabl1);
              ptabl1 = ptabl2;

              ptaux1 = ptaux1->NEXT;
              ptaux2 = ptaux2->NEXT;
            }
            ptabl2 = createExpr (NOT);
            addQExpr (ptabl2, ptabl1);

            result.LIST_ABL = addchain (result.LIST_ABL,ptabl2);
            result.WIDTH    = 1;
            freechain (expr1.LIST_ABL);
            freechain (expr2.LIST_ABL);
            expr1.LIST_ABL = NULL;
            expr2.LIST_ABL = NULL;
          }
        }
      }
      break;

    case NE :

      if ((expr1.LIST_ABL == NULL) || (expr2.LIST_ABL == NULL))
        beh_toolbug (4,"bvl_crtabl",NULL,0);
      else
      {
        if (expr1.WIDTH != expr2.WIDTH)
        {
          bvl_error(38,NULL);
          ptabl1 = createAtom ("'1'");

          ptaux1 = expr1.LIST_ABL;
          while (ptaux1 != NULL)
          {
            freeExpr (ptaux1->DATA);
            ptaux1 = ptaux1->NEXT;
          }
          ptaux2 = expr2.LIST_ABL;
          while (ptaux2 != NULL)
          {
            freeExpr (ptaux2->DATA);
            ptaux2 = ptaux2->NEXT;
          }
        }
        else
        {
          ptaux1 = expr1.LIST_ABL;
          ptaux2 = expr2.LIST_ABL;

          /* If expr2 = '0' then return expr1 */
          while(ptaux2 != NULL)
          {
            true_flag_zero = 1;
            if(!equalExpr(ptaux2->DATA,abl_zero))
            {
              /* One abl not abl_zero */
              true_flag_zero = 0;
              break;
            }
            ptaux2 = ptaux2->NEXT;  
          }
          if(true_flag_zero == 1)
          {
            result.LIST_ABL = expr1.LIST_ABL;
            result.WIDTH    = 1;
            expr1.LIST_ABL  = NULL;
            expr2.LIST_ABL  = NULL;
          }
          /* If expr2 = '1' then return NOT(expr1) */
          ptaux2 = expr2.LIST_ABL;
          true_flag_un = 0;
          while(ptaux2 != NULL)
          {
            true_flag_un = 1;
            if(!equalExpr(ptaux2->DATA,abl_un))
            {
              /* One abl not abl_un */
              true_flag_un = 0;
              break;
            }
            ptaux2 = ptaux2->NEXT;  
          }
          if(true_flag_un == 1)
          {
            while(ptaux1 != NULL)
            {
              ptabl1 = createExpr(NOT);
              addQExpr(ptabl1,(struct chain *)ptaux1->DATA);
              ptaux1->DATA = ptabl1;
              ptaux1       = ptaux1->NEXT;
            }
            result.LIST_ABL = expr1.LIST_ABL;
            result.WIDTH    = 1;
            expr1.LIST_ABL  = NULL;
            expr2.LIST_ABL  = NULL;
          }


          ptaux2 = expr2.LIST_ABL;
          if((true_flag_zero == 0) && (true_flag_un == 0))
          {
            ptabl1 = createExpr (XOR);
            addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
            addQExpr (ptabl1, (struct chain *)ptaux2->DATA);

            ptaux1 = ptaux1->NEXT;
            ptaux2 = ptaux2->NEXT;

            for (i=2 ; i<=expr1.WIDTH ; i++)
            {
              ptabl2 = createExpr (OR);
              addQExpr (ptabl2, ptabl1);

              ptabl1 = createExpr (XOR);
              addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
              addQExpr (ptabl1, (struct chain *)ptaux2->DATA);

              addQExpr (ptabl2, ptabl1);
              ptabl1 = ptabl2;

              ptaux1 = ptaux1->NEXT;
              ptaux2 = ptaux2->NEXT;
            }
            result.LIST_ABL = addchain(result.LIST_ABL,ptabl1);
            result.WIDTH    = 1;
            freechain (expr1.LIST_ABL);
            freechain (expr2.LIST_ABL);
            expr1.LIST_ABL = NULL;
            expr2.LIST_ABL = NULL;
          }
        }
      }
      break;

    case AND  :
    case NAND :
    case OR   :
    case NOR  :
    case XOR  :

      if (expr1.LIST_ABL == NULL)
      {
        if (expr2.LIST_ABL == NULL)
          beh_toolbug (4,"bvl_crtabl",NULL,0);
        else
        {
          result.LIST_ABL = expr2.LIST_ABL;
          result.WIDTH    = expr2.WIDTH;
          expr2.LIST_ABL  = NULL;
        }
      }
      else
      {
        if (expr2.LIST_ABL == NULL)
        {
          result.LIST_ABL = expr1.LIST_ABL;
          result.WIDTH    = expr1.WIDTH;
          expr1.LIST_ABL  = NULL;
        }
        else
        {
          if (expr1.LIST_ABL == expr2.LIST_ABL)
            beh_toolbug (16,"bvl_crtabl",NULL,0);
          else
          {
            if (expr1.WIDTH != expr2.WIDTH)
            {
              bvl_error(38,NULL);
            }
            else
            {
              ptaux1 = expr1.LIST_ABL;
              ptaux2 = expr2.LIST_ABL;

              for (i=1 ; i<=expr1.WIDTH ; i++)
              {
                ptabl1 = createExpr (oper);
                addQExpr (ptabl1, (struct chain *)ptaux1->DATA);
                addQExpr (ptabl1, (struct chain *)ptaux2->DATA);

                ptaux1->DATA = (void *)ptabl1;

                ptaux1       = ptaux1->NEXT;
                ptaux2       = ptaux2->NEXT;
              }
            }
            result.LIST_ABL = expr1.LIST_ABL;
            result.WIDTH    = expr1.WIDTH;
            freechain (expr2.LIST_ABL);
            expr1.LIST_ABL  = NULL;
            expr2.LIST_ABL  = NULL;
          }
        }
      }
      break;

    case ANDM :
      if ((expr1.LIST_ABL == NULL) || (expr2.LIST_ABL == NULL))
        beh_toolbug (4,"bvl_crtabl",NULL,0);
      else
      {
        if (expr2.WIDTH != 1)
        {
          bvl_error( 38,NULL);
        }
        else
        {
          ptaux1 = expr1.LIST_ABL;
          ptaux2 = expr2.LIST_ABL;
          while (ptaux1 != NULL)
          {
            ptabl1 = createExpr (AND);
            addQExpr (ptabl1,          (struct chain *)ptaux1->DATA);
            addQExpr (ptabl1, copyExpr((struct chain *)ptaux2->DATA));
            ptaux1->DATA = (void *)ptabl1;

            ptaux1       = ptaux1->NEXT;
          }
        }
        result.LIST_ABL = expr1.LIST_ABL;
        result.WIDTH    = expr1.WIDTH;
        ptaux2 = expr2.LIST_ABL;
        while (ptaux2 != NULL)
        {
          freeExpr (ptaux2->DATA);
          ptaux2 = ptaux2->NEXT;
        }
        freechain (expr2.LIST_ABL);
        expr2.LIST_ABL  = NULL;
        expr1.LIST_ABL  = NULL;
      }
      break;

    default :
      beh_toolbug (1,"bvl_crtabl",NULL,0);
  }

  /* may be cleaner */
  freeExpr(abl_un);
  freeExpr(abl_zero);

  return (result);
}

/* ###--------------------------------------------------------------### */
/* function	: bvl_select						*/
/* description	: create an abl representing the choice in a selected	*/
/*		  signal assignment and perform unicity verification	*/
/*		  using BDDs.						*/
/* called func.	: bvl_tobin   , bddToAbl     , addInputCct      ,	*/
/*		  substPhyExpr, initializeCct, createNodeTermBdd,	*/
/*		  bvl_error   , notBdd       , applyBinBdd      ,	*/
/*		  addchain    , namealloc				*/
/* ###--------------------------------------------------------------### */

void 
bvl_select (bvlexpr *result, chain_list *ptstr, pNode *ptbdd, bvlexpr ptablstr)
{
  char             binstr[256];
  int              i;
  struct chain    *ptauxabl;
  pNode            ptbddres;
  pNode            ptbddnew;
  pNode            ptbddtmp;
  pNode            ptbddaux;
  char             nomvar[10];
  struct chain    *ptnewabl;
  struct chain    *ptnewabl2;
  static int       oth_flg=0;
  static int       last_width=0;
  static pCircuit  pC = NULL;

  result->LIST_ABL = NULL;
  result->IDENT    = NULL;

  ptbddtmp = BDD_zero;

  if (pC == NULL)
    pC = initializeCct ("-select-",200,0);

  if (*ptbdd == NULL)
  {
    if (last_width < ptablstr.WIDTH)
    {
      for (; last_width<ptablstr.WIDTH ; last_width++)
      {
        sprintf (nomvar,"(%d)",last_width);
        createNodeTermBdd (addInputCct(pC,nomvar));
      }
    }
    *ptbdd = BDD_zero;
    oth_flg = 0;
  }

  while (ptstr != NULL)
  {
    bvl_tobin (binstr, (char *)ptstr->DATA,-1,-1);
    if (oth_flg != 0)
    {
      bvl_error(30,NULL);
    }

    if (strcmp("others", (char *)ptstr->DATA))
    {
      ptbddres = BDD_one;

      if (strlen (binstr) != (size_t)ptablstr.WIDTH)
      {
        bvl_error(38,NULL);
      }
      for (i=0 ; binstr[i]!='\0' ; i++)
      {
        ptbddaux = createNodeTermBdd (i+2);
        if (binstr[i] == '0')
          ptbddaux = notBdd (ptbddaux);
        ptbddres = applyBinBdd (AND, ptbddaux, ptbddres);
      }
      ptbddnew = applyBinBdd (OR,*ptbdd,ptbddres);

      if (*ptbdd == ptbddnew)
      {
        bvl_error(28,NULL);
      }

      *ptbdd = ptbddnew;
    }
    else
    {
      oth_flg   = 1;
      ptbddres = notBdd (*ptbdd);
      *ptbdd   = BDD_one;
    }
    ptbddtmp = applyBinBdd (OR,ptbddtmp,ptbddres);
    ptstr = ptstr->NEXT;
  }

  ptnewabl = bddToAbl (ptbddtmp, pC->pNameI);

  ptauxabl = ptablstr.LIST_ABL;
  i         = ptablstr.WIDTH - 1;
  while (ptauxabl != NULL)
  {
    sprintf (nomvar,"(%i)",i);
    /*--------
      substPhyExpr (ptnewabl,namealloc(nomvar),(struct chain *)ptauxabl->DATA);
      ---------*/
    ptnewabl2 = substExpr (ptnewabl,namealloc(nomvar),
                           (struct chain *)ptauxabl->DATA);
    freeExpr (ptnewabl);
    ptnewabl = ptnewabl2;

    i--;
    ptauxabl = ptauxabl->NEXT;
  }
  result->LIST_ABL = addchain (NULL, ptnewabl);
  result->WIDTH    = 1;

}

/* ###--------------------------------------------------------------### */
/* function	: bvl_tobin						*/
/* description	: transform a StringLit, BitStringLit or CharacterLit	*/
/*		  in a string of '0' and '1's				*/
/* ###--------------------------------------------------------------### */

int 
bvl_tobin (char *trg, char *src, int left, int right)
{
  char base;
  int  indx;
  int  j = 0;
  int  errflg = 0;
  char lcl_trg[256];

  lcl_trg[0] = '\0';

  if (src == NULL)
  {
    strcpy (trg,"0");
  }
  else
  {
    if (!strcmp (src,"others"))
    {
      strcpy(trg,src);
    }
    else
    {
      if ((src[0] != '\'') && (src[0] != '"') && (src[0] != '%'))
      {
        base = src[0];
        indx = 2;
      }
      else
      {
        base = 'B';
        indx = 1;
      }

      switch (base)
      {
        case 'B' :
        case 'b' :
          while ((lcl_trg[j] = src[indx]) != '\0')
          {
            switch (src[indx])
            {
              case '0':
              case '1':
              case 'z':
              case 'u':
              case 'd':				/* Beware Not VHDL	*/
                j++; break;

              case '%' :
              case '"' :
              case '\'':
              case '_' :
                break;

              default :
                errflg = 1; bvl_error(73,src);
            }
            indx++;
          }
          break;

        case 'O' :
        case 'o' :
          while (src[indx] != '\0')
          {
            j += 3;
            switch (src[indx])
            {
              case '0' :
                strcat (lcl_trg,"000"); break;
              case '1' :
                strcat (lcl_trg,"001"); break;
              case '2' :
                strcat (lcl_trg,"010"); break;
              case '3' :
                strcat (lcl_trg,"011"); break;
              case '4' :
                strcat (lcl_trg,"100"); break;
              case '5' :
                strcat (lcl_trg,"101"); break;
              case '6' :
                strcat (lcl_trg,"110"); break;
              case '7' :
                strcat (lcl_trg,"111"); break;
              case '"' :
              case '%' :
              case '_' :
                j -= 3; break;
              default :
                j -= 3; errflg = 1; bvl_error(73,src);
            }
            indx++;
          }
          break;

        case 'X' :
        case 'x' :
          while (src[indx] != '\0')
          {
            j += 4;
            switch (src[indx])
            {
              case '0' :
                strcat (lcl_trg,"0000"); break;
              case '1' :
                strcat (lcl_trg,"0001"); break;
              case '2' :
                strcat (lcl_trg,"0010"); break;
              case '3' :
                strcat (lcl_trg,"0011"); break;
              case '4' :
                strcat (lcl_trg,"0100"); break;
              case '5' :
                strcat (lcl_trg,"0101"); break;
              case '6' :
                strcat (lcl_trg,"0110"); break;
              case '7' :
                strcat (lcl_trg,"0111"); break;
              case '8' :
                strcat (lcl_trg,"1000"); break;
              case '9' :
                strcat (lcl_trg,"1001"); break;
              case 'a' :
              case 'A' :
                strcat (lcl_trg,"1010"); break;
              case 'b' :
              case 'B' :
                strcat (lcl_trg,"1011"); break;
              case 'c' :
              case 'C' :
                strcat (lcl_trg,"1100"); break;
              case 'd' :
              case 'D' :
                strcat (lcl_trg,"1101"); break;
              case 'e' :
              case 'E' :
                strcat (lcl_trg,"1110"); break;
              case 'f' :
              case 'F' :
                strcat (lcl_trg,"1111"); break;
              case '%' :
              case '"' :
              case '_' :
                j -= 4; break;
              default :
                j -= 4; errflg = 1; bvl_error(73,src);
            }
            indx++;
          }
          break;

        default :
          beh_toolbug (17,"bvl_tobin",NULL,base);
      }

      if ((j == 0) || (j <= right))
      {
        trg[0] = '0';
        trg[1] = '\0';
      }
      else
      {
        if (left != -1)
        {
          strcpy (trg, &lcl_trg[left]);
          trg[right - left + 1] = '\0';
        }
        else
          strcpy (trg, lcl_trg);
      }
    }
  }

  return (errflg);
}
