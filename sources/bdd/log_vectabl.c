/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : VECTABL Version 1.00                                        */
/*    Fichier : log_vectabl.c                                               */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include MUT_H
#include LOG_H
#include AVT_H

/*}}}************************************************************************/
/*{{{                                                                       */
/*                       functions declarations                             */
/*                                                                          */
/****************************************************************************/
char            *replicateBitStr    ( char          *bitstr     ,
                                      int            n          );
chain_list      *concatAbl          ( chain_list    *expr1      ,
                                      chain_list    *expr2      );
chain_list      *replicateAbl       ( chain_list    *expr       ,
                                      int            n          );
chain_list      *getAblAtPos        ( chain_list    *expr       ,
                                      int            pos        );
int              getVectAblRange    ( chain_list    *expr       ,
                                      int           *left       ,
                                      int           *right      );
char            *getVectAblVarName  ( chain_list    *expr       );
int              verifyVectAbl      ( chain_list    *expr       );
int              getAtomSize        ( chain_list    *expr       );
chain_list      *getAblAtIndex      ( chain_list    *expr       ,
                                      int            left       ,
                                      int            right      ,
                                      int            index      );
chain_list      *createBitStr       ( char          *bitstr     );
int              isBitStr           ( char          *bitstr     );
void             freeBitStrTable    (                           );
char            *makeBitStr         ( char          *bistr      );
char            *invertBitStr       ( char          *bitstr     );
chain_list *downCat(chain_list *expr);

ht              *LOG_BITSTRTABLE = NULL;

char *genBitStr(int n,char car);

/****************************************************************************/
/*{{{                    concatAbl()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *concatAbl(chain_list *expr1, chain_list *expr2)
{
  chain_list    *resExpr;
  chain_list    *chainx;
  int            vectSize;

  vectSize = verifyVectAbl(expr1) + verifyVectAbl(expr2);

  if (!ATOM(expr1) && OPER(expr1)==CAT)
  {
    resExpr = expr1;
    for (chainx=resExpr->NEXT; chainx->NEXT; chainx=chainx->NEXT)
      ;
    if (!ATOM(expr2) && OPER(expr2)==CAT)
    {
      freechain(CAR(expr2));
      expr2 = delchain(expr2,expr2);
    }
    else
      expr2 = addchain(NULL,expr2);
//    addQExpr(resExpr,expr2);
    chainx->NEXT = expr2;
  }
  else if (!ATOM(expr2) && OPER(expr2)==CAT)
  {

    resExpr     = expr2;
    addHExpr(resExpr,expr1);
  }
//  else if (OPER(expr1)==REPLICATE)
//  else if (OPER(expr2)==REPLICATE)
  else if (!ATOM(expr1) && !ATOM(expr2)
           && OPER(expr1) == OPER(expr2)
           && lengthExpr(expr1) == lengthExpr(expr2))
  {
    resExpr = createExpr(OPER(expr1));
    while ((expr1 = CDR(expr1)) && (expr2 = CDR(expr2)))
      addQExpr(resExpr,concatAbl(CAR(expr1),CAR(expr2)));
  }
  else
  {
    resExpr = createExpr(CAT);
    addHExpr(resExpr,expr2);
    addHExpr(resExpr,expr1);
  }

  if (CDAR(resExpr))
    VECT_S(resExpr) = vectSize;
  else
    CDAR(resExpr) = addchain(NULL,(void *)(long)vectSize);

  return resExpr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/* reduce a concatenation by grouping same atom radical                     */
/* return the way of the reduce operation 0 for upto, 1 for downto          */
/****************************************************************************/
/*int simplifyCat(chain_list *expr)
{
  chain_list    *tmp    = expr;
  
  if (ATOM(expr))
    return -1;
  else if (OPER(expr) == CAT)
  {
    while (tmp)
      
    
    
  }
  else
    while (tmp = CDR(tmp))
      return simplifyCat(CAR(expr));
}
*/

/*}}}************************************************************************/
/*{{{                    replicateAbl()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *replicateAbl(chain_list *expr, int n)
{
  chain_list    *resExpr;
  int            vectSize;
  int            replicateTimes;

  resExpr = expr;
  if (!ATOM(resExpr) && OPER(resExpr)==REPLICATE)
  {
    replicateTimes  = (int)(VECT_S(resExpr) * n);
    vectSize = verifyVectAbl(resExpr) * n;
    VECT_S(resExpr) = vectSize;
    REP_NB(resExpr) = replicateTimes;
  }
  else if (ATOM(resExpr) && isBitStr(resExpr->DATA))
  {
    resExpr->DATA = replicateBitStr(expr->DATA,n);
  }
  else
  {
    resExpr = createExpr(REPLICATE);
    CDR(resExpr) = addchain(NULL,expr);
    replicateTimes = n;
    vectSize = verifyVectAbl(expr) * n;
    CDAR(resExpr) = addchain(NULL,(void *)(long)n);
    CDAR(resExpr) = addchain(CDAR(resExpr),(void *)(long)vectSize);
  }

  return resExpr;
}

/*}}}************************************************************************/
/*{{{                    getAblAtPos()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *getAblAtPos(chain_list *expr, int pos)
{
  chain_list    *resExpr    = NULL;
  chain_list    *subExpr;
  chain_list    *exprx;
  char           buf[255];
  chain_list    *chainx;
  int            count      = 0;
  int            countNext  = 0;
  int            bit;
  int            vectSize;
  int            left;

  vectSize = verifyVectAbl(expr);
  
  if (vectSize > pos)
  {
    if (ATOM(expr))
    {
      if (getAtomSize(expr) == 1)
        resExpr = addchain(NULL,expr->DATA);
      else if (isBitStr(expr->DATA))
      {
        sprintf(buf,"'%c'",((char*)expr->DATA)[pos+1]);
        resExpr = addchain(NULL,namealloc(buf));
      }
      else
      {
        bit = getVectAblRange(expr,&left,NULL);
        bit = left + (bit * pos);
        sprintf(buf,"%s %d",getVectAblVarName(expr),bit);
        resExpr = addchain(NULL,namealloc(buf));
      }
    }
    else if (vectSize == 1)
      resExpr = copyExpr(expr);
    else if (OPER(expr) == CAT)
    {
      for (chainx = expr->NEXT; chainx; chainx = chainx->NEXT)
      {
        countNext = count + verifyVectAbl(CAR(chainx));
        if (pos<countNext)
          break;
        else
          count = countNext;
      }
      resExpr   = getAblAtPos(CAR(chainx),pos-count);
    }
    else if (OPER(expr) == REPLICATE)
    {
      count = VECT_S(expr)/REP_NB(expr);
      if (count==1)
        resExpr = getAblAtPos(CADR(expr),0);
      else
        resExpr = getAblAtPos(CADR(expr),pos%count);
    }
    else
    {
      resExpr       = createExpr(OPER(expr));
      exprx         = resExpr;
      for (chainx = expr->NEXT; chainx; chainx = chainx->NEXT)
      {
        subExpr     = getAblAtPos(CAR(chainx),pos);
        exprx->NEXT = addchain(NULL,subExpr);
        exprx       = exprx->NEXT;
      }
    }
  }
  else
    resExpr = NULL;
  
  return resExpr;
}

/*}}}************************************************************************/
/*{{{                    downCat()                                          */
/*                                                                          */
/* move concatenation to leaf                                               */
/****************************************************************************/
chain_list *downCat(chain_list *expr)
{
  if (expr)
  {
    if (OPER(expr) == CAT)
    {
      chain_list    *abl    = expr;
      chain_list    *res    = CADR(expr);
    
      while ((abl = CDR(abl)))
        if (isBitStr(VALUE_ATOM(CAR(abl))))
          return expr;
      abl   = CDR(expr);
      while ((abl = CDR(abl)))
        res = concatAbl(res,CAR(abl));
      return res;
    }
    else
      return expr;
  }
  else
    return expr;
}

/*}}}************************************************************************/
/*{{{                    getVectAblRange()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int getVectAblRange(chain_list *expr, int *left, int *right)
{
  int        res;
  long       pos1;
  long       pos2;
  int        leFt;
  int        riGht;

  if (ATOM(expr))
  {
    if ((pos2 = (long)strchr(expr->DATA,':')))
    {
      pos1  = (long)strchr(expr->DATA,' ');
      leFt  = atoi((char *)(pos1+1));
      riGht = atoi((char *)(pos2+1));
      if (left)  *left  = leFt;
      if (right) *right = riGht;
      res = (leFt>riGht)?-1:1;
    }
    else res = 0;
  }
  else res = 0;

  return res;
}

/*}}}************************************************************************/
/*{{{                    repToCatVectAbl()                                  */
/*                                                                          */
/* convert a replicate abl onto a cat abl                                   */
/*                                                                          */
/****************************************************************************/
void repToCatVectAbl(chain_list *expr)
{
  int            i;
  int            nb;
  chain_list    *tmpExpr;
  chain_list    *cpyExpr;
  chain_list    *resExpr;
  
  if (OPER(expr) == REPLICATE)
  {
    nb = REP_NB(expr);
    resExpr = addchain(NULL,NULL);
    CAR(resExpr)  = CAR(expr);
    OPER(resExpr) = CAT;
    CDR(CDAR(resExpr)) = delchain(CDR(CDAR(resExpr)),CDR(CDAR(resExpr))); 
    for (i = 0; i < nb; i ++)
    {
      cpyExpr = copyExpr(expr);
      for (tmpExpr = cpyExpr; CDR(tmpExpr); tmpExpr = CDR(tmpExpr))
        ;
      CDR(tmpExpr) = CDR(resExpr);
      CDR(resExpr) = CDR(cpyExpr);
      CDR(cpyExpr) = NULL;
      freeExpr(cpyExpr);
    }
    CDR(expr) = CDR(resExpr);
    delchain(resExpr,resExpr);
  }
}

/*}}}************************************************************************/
/*{{{                    changeAblAtRange()                                 */
/*                                                                          */
/* substitute abl on range by a new abl                                     */
/* mode 1 only subsitute undefined abl on range                             */
/*                                                                          */
/* expr : 0     left     right    n                                         */
/*        +-------+--------+------+                                         */
/*        |xxxxxxx| added  |xxxxxx|                                         */
/*        +-------+--------+------+                                         */
/* mode 1 |xxxxxxx|"UUUUUU"|xxxxxx|                                         */
/* mode 2 |xxxxxxx|existing|xxxxxx|                                         */
/*        +-------+--------+------+                                         */
/*        |    ^     expr         |                                         */
/*        +---/|\-----------------+                                         */
/*             |                                                            */
/*            pos (current position)                                        */
/*                                                                          */
/****************************************************************************/
int changeAblAtRange(chain_list *expr,int left,int right,chain_list *added,
                     int mode)
{
  int            vsize;
  int            i;
  char           buf1[255];
  char           buf2[255];
  char          *buft;
  chain_list    *tmpExpr;
  int            al,ar; // atome caracterisation
  char          *an;
  chain_list    *nl;
  chain_list    *nr;
  
  // only mode 1
  if ((vsize = verifyVectAbl(expr)) >= right)
  {
    int            tmp;
    chain_list    *abl;

    if (ATOM(expr))
    {
      switch (mode)
      {
        case 1 : 
             if (isBitStr(VALUE_ATOM(expr))) 
             {
               for (i = left; i <= right; i ++)
                 if ( (VALUE_ATOM(expr))[i+1] != 'u')
                   return 0;
             }
             else
               return 0;
        case 2 : 
             if (isBitStr(VALUE_ATOM(expr)))
             {
               sprintf(buf1,"%s",VALUE_ATOM(expr)); // before
               buf1[0]       = (left == 1) ? '\'' : '"';
               buf1[left+1]  = (left == 1) ? '\'' : '"';
               buf1[left+2]  = '\0';
               nl            = (left == 0) ? NULL : createBitStr(buf1);
               
               sprintf(buf2,"%s",VALUE_ATOM(expr)); // after
               buf2[vsize+1] = (right == vsize-1) ? '\'' : '"';
               buft          = (char*)((long)&buf2 + right + 1);
               buft[0]       = (right == vsize-1) ? '\'' : '"';
               nr            = (right == vsize-1) ? NULL : createBitStr(buft);
             }
             else
             {
               getVectAtomRange(expr,&an,&al,&ar);
               if (al<ar) // good range direction
               {
                 nl     = createVectAtom(an, al, al+left-1);
                 nr     = createVectAtom(an, al+right+1, ar);
               }
               else
               {
                 nl     = createVectAtom(an, al, al-left+1);
                 nr     = createVectAtom(an, al-right-1, ar);
               }
             }
      }
      tmpExpr    = (nl) ? concatAbl(nl,added) : added;
      tmpExpr    = (nr) ? concatAbl(tmpExpr,nr) : tmpExpr;
//      tmpExpr    = downCat(tmpExpr);
      CAR(expr) = CAR(tmpExpr);
      CDR(expr) = CDR(tmpExpr);
      delchain(tmpExpr,tmpExpr);
      return 2;
    }
    else switch (OPER(expr))
    {
      case CAT :
           abl = expr;
           abl = CDR(abl);
           for (i = 0; abl ;abl = CDR(abl)) 
           {
             tmp = verifyVectAbl(CAR(abl));
             if (i <= left && right < tmp+i)
             {
               if (changeAblAtRange(CAR(abl),left-i,right-i,added,mode))
               {
                 chain_list     *delExpr = NULL;
                 chain_list     *abl2;
                 
                 if (OPER(CAR(abl)) == CAT)
                 {
                   delExpr      = CAR(abl);
                   tmpExpr      = CDR(abl);
                   CAR(abl)     = CAR(CDR(delExpr));
                   CDR(abl)     = CDR(CDR(delExpr));
                   while (CDR(abl))
                     abl        = CDR(abl);
                   CDR(abl)     = tmpExpr;
                   CDR(delExpr) = NULL;
                   freeExpr(delExpr);
                 }
                 abl2           = downCat(expr);
                 CDR(expr)      = CDR(abl2);
                 CAR(expr)      = CAR(abl2);
                 return 1;
               }
             }
             else
               i += tmp;
           }
           return 0;
      case REPLICATE :
           repToCatVectAbl(expr);
           return changeAblAtRange(expr,left,right,added,mode);
      default :
           avt_errmsg(LOG_ERRMSG,"000",AVT_ERROR,"157");
//           printf("pas traite changeAblAtRange() def\n");
//           return 0;
    }
    return 1;
  }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    createVectAtomName()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *createVectAtomName(char *name,int left,int right)
{
  char       buf[255];

  if (left != right)
    sprintf(buf,"%s %d:%d",name,left,right);
  else
    sprintf(buf,"%s %d",name,left);
  return namealloc(buf);  
}

/*}}}************************************************************************/
/*{{{                    createVectAtom()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *createVectAtom(char *name,int left,int right)
{
  return createAtom(createVectAtomName(name,left,right));
}

/*}}}************************************************************************/
/*{{{                    getVectAtomRange()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void getVectAtomRange(chain_list *expr,char **name,int *left, int*right)
{
  char       buf[255];
  char       tmp[16];
  int        i          = 0;
  int        j          = 0;
  int        done       = 0;

  sprintf(buf,"%s",(char *)expr->DATA);
  while (buf[i] != ' ') i++;
  buf[i] = '\0';
  *name = namealloc(buf);
  i++;
  while (buf[i] != '\0')
  {
    if (buf[i] == ':')
    {
      tmp[j] = '\0';
      *left = atoi(tmp);
      done = 1;
      j = 0;
    }
    else
      tmp[j++] = buf[i];
    i ++;
  }
  tmp[j] = '\0';
  *right = atoi(tmp);
  if (!done)
    *left = atoi(tmp);
}

/*}}}************************************************************************/
/*{{{                    getAblAtIndex()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *getAblAtIndex(chain_list *expr,int left,int right,int index)
{
  int        pos;
  
  pos = left - index;
  if (left<right)
    pos = 0 - pos;
    
  return getAblAtPos(expr,pos);
}

/*}}}************************************************************************/
/*{{{                    getVectAblVarName()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *getVectAblVarName(chain_list *expr)
{
  char      *res;
  char       buf[255];
  int        i          = 0;

  if (ATOM(expr))
  {
    sprintf(buf,"%s",(char *)expr->DATA);
    while (buf[i]!=' ' && buf[i] != '\0')
      i++;
    buf[i] = '\0';
    res = namealloc(buf);
  }
  else res = NULL;

  return res;
}

/*}}}************************************************************************/
/*{{{                    initBitStrTable()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void initBitStrTable()
{
  LOG_BITSTRTABLE = addht(200);
  addhtitem(LOG_BITSTRTABLE,namealloc("'0'"),1);
  addhtitem(LOG_BITSTRTABLE,namealloc("'1'"),1);
  addhtitem(LOG_BITSTRTABLE,namealloc("'d'"),1);
  addhtitem(LOG_BITSTRTABLE,namealloc("'u'"),1);
  addhtitem(LOG_BITSTRTABLE,namealloc("'z'"),1);
}

/*}}}************************************************************************/
/*{{{                    verifyVectAbl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int verifyVectAbl(chain_list *expr)
{
  int            res    = 0;
  chain_list    *chainx;
  int            vectSize;

  if (ATOM(expr))
    res = getAtomSize(expr);
  else if (CDAR(expr) && VECT_S(expr))
    res = (int)VECT_S(expr);
  else
  {
    for (chainx=expr->NEXT;chainx;chainx=chainx->NEXT)
    {
      vectSize = verifyVectAbl(CAR(chainx));
      if (!res)                 res = vectSize;
      else if (vectSize!=res)
        return 0;
    }
    if (CDAR(expr))
      VECT_S(expr) = res;
    else
      CDAR(expr) = addchain(NULL,(void *)(long)res);
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    getAtomSize()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int getAtomSize(chain_list *expr)
{
  int        res;
  long       pos1;
  long       pos2;

  if (isBitStr(expr->DATA))
    res = strlen(expr->DATA) - 2;
  else if ((pos2 = (long)strchr(expr->DATA,':')))
  {
    pos1 = (long)strchr(expr->DATA,' ');
    if (pos1 == 0)
      res = 1;
    else
      res = abs(atoi((char *)(pos1+1)) - atoi((char *)(pos2+1))) + 1;
  }
  else res = 1;

  return res;
}

/*}}}************************************************************************/
/*{{{                    invertVectAbl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void invertVectAbl(chain_list *expr)
{
  chain_list    *chainx;
  
  if (ATOM(expr)) invertAtom(expr);
  else switch (OPER(expr))
  {
    case CAT : 
      CDR(expr) = reverse(CDR(expr)); 
    default :
      for (chainx=CDR(expr);chainx;chainx=chainx->NEXT)
        invertVectAbl(CAR(chainx));
  }
}

/*}}}************************************************************************/
/*{{{                    invertAtom()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void invertAtom(chain_list *expr)
{
  int            left;
  int            right;
  char          *pos1;
  char          *pos2;
  char          *name;
  char           buf[255];

  if (ATOM(expr))
  {
    name = mbkstrdup(expr->DATA);
    if (isBitStr(expr->DATA))
    {
      expr->DATA = invertBitStr(expr->DATA);
    }
    else if ((pos2 = strchr(name,':')))
    {
      pos1 = strchr(name,' ');
      *pos1 = '\0';
      left = atoi(pos1+1);
      right= atoi(pos2+1);
      sprintf(buf,"%s %d:%d",name,right,left);
      expr->DATA = namealloc(buf);
    }
    mbkfree(name);
  }
}

/*}}}************************************************************************/
/*{{{                    invertBitStr()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *invertBitStr(char *bitstr)
{
  char       buf[1024];
  char       tmp[1024];
  int        i, j;

  sprintf(tmp,"%s",bitstr);
  for (i=strlen(tmp)-1,j=0;i>=0;i--,j++)
    buf[j] = tmp[i];
  buf[++j] = '\0';
  
  return makeBitStr(buf);
}

/*}}}************************************************************************/
/*{{{                    createBitStr()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *createBitStr(char *bitstr)
{
  if (*bitstr == '\'' || *bitstr == '"')
    return createAtom(makeBitStr(bitstr));
  else
  {
    char     buf[1024];

    if (*(bitstr+1) != '\0')
      sprintf(buf,"\"%s\"",bitstr);
    else
      sprintf(buf,"'%s'",bitstr);
    return createAtom(makeBitStr(buf));
  }
}

/*}}}************************************************************************/
/*{{{                    makeBitStr()                                       */
/*                                                                          */
/* fait d'une chaine de caractere une bitstring                             */
/****************************************************************************/
char *makeBitStr(char *bitstr)
{
  char      *res;

  res = namealloc(bitstr);
  if (!LOG_BITSTRTABLE)
    initBitStrTable();
  addhtitem(LOG_BITSTRTABLE,res,1);

  return res;
} 

/*}}}************************************************************************/
/*{{{                    genOneBitStr()                                     */
/*                                                                          */
/* generate a bitstring of one (n lenght)                                   */
/****************************************************************************/
char *genOneBitStr(int n)
{
  return genBitStr(n,'1');
}

/*}}}************************************************************************/
/*{{{                    genZeroBitStr()                                    */
/*                                                                          */
/* generate a bitstring of zero (n lenght)                                  */
/****************************************************************************/
char *genZeroBitStr(int n)
{
  return genBitStr(n,'0');
}

/*}}}************************************************************************/
/*{{{                    genHZBitStr()                                      */
/*                                                                          */
/* generate a bitstring of one (n lenght)                                   */
/****************************************************************************/
char *genHZBitStr(int n)
{
  return genBitStr(n,'z');
}

/*}}}************************************************************************/
/*{{{                    genBitStr()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *genBitStr(int n,char car)
{
  char       buf[255];
  int        i;
  
  buf[0]   = (n == 1) ? '\'' : '"';
  for (i = 1; i <= n; )
    buf[i++] = car;
  buf[i++] = (n == 1) ? '\'' : '"';
  buf[i]   = '\0';
  
  return makeBitStr(buf);
}

/*}}}************************************************************************/
/*{{{                    replicateBitStr()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *replicateBitStr(char *bitstr, int n)
{
  char       buf[511];
  int        i, j, len, k;

  k = 0;
  buf[k++] = '"'; 
  len = strlen(bitstr) - 2;
  for (j = 0; j < n; j ++)
    for (i = 1; i <= len; i ++)
      buf [k++] = bitstr[i];
  
  buf[k++] = '"'; 
  buf[k++] = '\0';
  
  return makeBitStr(buf);
}

/*}}}************************************************************************/
/*{{{                    fprintHZBitStr()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void fprintHZBitStr(FILE *file,char *bitstr)
{
  unsigned int   i;
  unsigned int   n = strlen(bitstr) - 2;

  fprintf(file,"%c",(n == 1) ? '\'' : '"');
  for (i = 0; i < n; i ++)
    fprintf(file,"Z");
  fprintf(file,"%c",(n == 1) ? '\'' : '"');
}

/*}}}************************************************************************/
/*{{{                    genUBitStr()                                       */
/*                                                                          */
/* generate a bitstring of one (n lenght)                                   */
/****************************************************************************/
char *genUBitStr(int n)
{
  return genBitStr(n,'u');
}

/*}}}************************************************************************/
/*{{{                    getBitStr()                                        */
/*                                                                          */
/* create an easy to drive copy of the bitstring                            */
/*                                                                          */
/****************************************************************************/
char *getBitStr(char *str)
{
  char       buf[255];
  int        i, n, j;

  n = strlen(str);
  
  for (i = 0, j = 0; i <= n; i ++)
  {
    switch (str[i])
    {
      case 'u' : buf[j++] = 'U';      break;
      case 'z' : buf[j++] = 'Z';      break;
      case 'd' : buf[j++] = 'D';      break;
      default  : buf[j++] = str[i];   break;
    }
  }
  return mbkstrdup(buf);
}

/*}}}************************************************************************/
/*{{{                    fprintUBitStr()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void fprintUBitStr(FILE *file,char *bitstr)
{
  int            i;
  int            n;

  n = strlen(bitstr)-2;
  
  if (n==1)
    fprintf(file,"'");
  else 
    fprintf(file,"\"");
  for (i = 0; i < n; i++)
    fprintf(file,"U");
  if (n==1)
    fprintf(file,"'");
  else 
    fprintf(file,"\"");
}

/*}}}************************************************************************/
/*{{{                    notBitStr()                                        */
/*                                                                          */
/* invert   a bitstring                                                     */
/****************************************************************************/
char *notBitStr(char *bitstr)
{
  char       buf[255];
  int        i;

  for (i = 0; bitstr[i-1] != '\0'; i ++)
    if (bitstr[i] == '0') 
      buf[i] = '1';
    else if (bitstr[i] == '1')
      buf[i] = '0';
    else
      buf[i] = bitstr[i];

  return makeBitStr(buf);
}

/*}}}************************************************************************/
/*{{{                    isBitStr()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int isBitStr(char *bitstr)
{
  if (!LOG_BITSTRTABLE)
    initBitStrTable();
  return
    (gethtitem(LOG_BITSTRTABLE,bitstr) == 1);
}

/*}}}************************************************************************/
/*{{{                    freeBitStrTable()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void  freeBitStrTable()
{
  delht(LOG_BITSTRTABLE);
}
