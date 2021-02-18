/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : BEHVECT Version 1.00                                        */
/*    Fichier : behvect.c                                                   */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/****************************************************************************/
/*{{{                    structures' constructors                           */
/****************************************************************************/
/*{{{                    beh_addbevectaux()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bevectaux_list *beh_addbevectaux(bevectaux_list *last, char *name,
                                 chain_list *abl, int left, int right,long flags)
{
  bevectaux_list    *ptaux;

  name              = namealloc(name);

  ptaux             = mbkalloc(sizeof(bevectaux_list));
  ptaux->NAME       = name;
  ptaux->TIME       = 0;
  ptaux->TIMEVAR       = NULL;
  ptaux->ABL        = abl;
  ptaux->NEXT       = last;
  ptaux->LEFT       = left;
  ptaux->RIGHT      = right;
  ptaux->FLAGS      = flags;

  return (ptaux);
}

/*}}}************************************************************************/
/*{{{                    beh_addbevectout()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bevectout_list *beh_addbevectout(bevectout_list *last, char *name,
                                 chain_list *abl, int left, int right,long flags)
{
  bevectout_list    *ptout;

  name              = namealloc(name);

  ptout             = mbkalloc(sizeof(bevectout_list));
  ptout->NAME       = name;
  ptout->TIME       = 0;
  ptout->TIMEVAR       = NULL;
  ptout->ABL        = abl;
  ptout->NEXT       = last;
  ptout->LEFT       = left;
  ptout->RIGHT      = right;
  ptout->FLAGS      = flags;

  return (ptout);
}

/*}}}************************************************************************/
/*{{{                    beh_addbevectpor()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bevectpor_list *beh_addbevectpor(bevectpor_list *last, char *name,
                                 char dir, char type, int left, int right)
{
  bevectpor_list    *ptpor;

  name              = namealloc(name);

  ptpor             = mbkalloc(sizeof(bevectpor_list));
  ptpor->NAME       = name;
  ptpor->DIRECTION  = dir;
  ptpor->TYPE       = type;
  ptpor->NEXT       = last;
  ptpor->LEFT       = left;
  ptpor->RIGHT      = right;

  return (ptpor);
}

/*}}}************************************************************************/
/*{{{                    beh_addbevectbux()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bevectbux_list *beh_addbevectbux(bevectbux_list *last, char *name,
                                 vectbiabl_list *vbiabl,
                                 int left, int right, char type, char flags)
{
  bevectbux_list    *ptbux;

  name              = namealloc(name);

  ptbux             = mbkalloc(sizeof(bevectbux_list));
  ptbux->NAME       = name;
  ptbux->VECTBIABL  = vbiabl;
  ptbux->NEXT       = last;
  ptbux->LEFT       = left;
  ptbux->TYPE       = type;
  ptbux->FLAGS      = flags;
  ptbux->RIGHT      = right;

  return (ptbux);
}

/*}}}************************************************************************/
/*{{{                    beh_addbevectbus()                                 */
/*                                                                          */
/*                                                                          */
/***************************************************************************/
bevectbus_list *beh_addbevectbus(bevectbus_list *last, char *name,
                                 vectbiabl_list *vbiabl,
                                 int left, int right, char type, char flags)
{
  bevectbus_list    *ptbus;

  name              = namealloc(name);

  ptbus             = mbkalloc(sizeof(bevectbus_list));
  ptbus->NAME       = name;
  ptbus->VECTBIABL  = vbiabl;
  ptbus->NEXT       = last;
  ptbus->LEFT       = left;
  ptbus->TYPE       = type;
  ptbus->FLAGS      = flags;
  ptbus->RIGHT      = right;

  return (ptbus);
}

/*}}}************************************************************************/
/*{{{                    beh_addbevectreg()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bevectreg_list *beh_addbevectreg(bevectreg_list *last, char *name,
                                 vectbiabl_list *vbiabl,
                                 int left, int right, char flags)
{
  bevectreg_list    *ptreg;

  name              = namealloc(name);

  ptreg             = mbkalloc(sizeof(bevectreg_list));
  ptreg->NAME       = name;
  ptreg->VECTBIABL  = vbiabl;
  ptreg->NEXT       = last;
  ptreg->LEFT       = left;
  ptreg->FLAGS      = flags;
  ptreg->RIGHT      = right;

  return (ptreg);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    structures' deleters                               */
/****************************************************************************/
/*{{{                    beh_delbevectaux()                                 */
/*                                                                          */
/* mode : recursive delete or not (Y or N)                                  */
/*                                                                          */
/****************************************************************************/
bevectaux_list *beh_delbevectaux(bevectaux_list *listvaux,
                                 bevectaux_list *vaux, char mode)
{
  bevectaux_list     head;
  bevectaux_list    *last;

  if (listvaux && vaux)
  {
    head.NEXT       = listvaux;
    last            = &head;
    while (last && last->NEXT != vaux)
      last          = last->NEXT;

    if (last)
    {
      if (mode == 'N')
      {
        if (vaux->ABL)
          beh_warning(315,vaux->NAME,NULL);
          //(stderr,"[BEH_WAR] bevectaux `%s` not empty\n",vaux->NAME);
      }
      else
        freeExpr(vaux->ABL);
      last->NEXT    = vaux->NEXT;
      mbkfree(vaux);
    }
    listvaux        = head.NEXT;
  }

  return listvaux;
}

/*}}}************************************************************************/
/*{{{                    beh_delbevectout()                                 */
/*                                                                          */
/* mode : recursive delete or not (Y or N)                                  */
/*                                                                          */
/****************************************************************************/
bevectout_list *beh_delbevectout(bevectout_list *listvout,
                                 bevectout_list *vout, char mode)
{
  bevectout_list     head;
  bevectout_list    *last;

  if (listvout && vout)
  {
    head.NEXT       = listvout;
    last            = &head;
    while (last && last->NEXT != vout)
      last          = last->NEXT;

    if (last)
    {
      if (mode == 'N')
      {
        if (vout->ABL)
          beh_warning(316,vout->NAME,NULL);
          //(stderr,"[BEH_WAR] bevectout `%s` not empty\n",vout->NAME);
      }
      else
        freeExpr(vout->ABL);
      last->NEXT    = vout->NEXT;
      mbkfree(vout);
    }
    listvout        = head.NEXT;
  }
  return listvout;
}

/*}}}************************************************************************/
/*{{{                    beh_delbevectpor()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
bevectpor_list *beh_delbevectpor(bevectpor_list *listvpor,
                                 bevectpor_list *vpor)
{
  bevectpor_list   head;
  bevectpor_list  *last;

  if (listvpor && vpor)
  {
    head.NEXT       = listvpor;
    last            = &head;
    while (last && (last->NEXT != vpor))
      last          = last->NEXT;

    if (last)
    {
      last->NEXT    = vpor->NEXT;
      mbkfree(vpor);
    }
    listvpor        = head.NEXT;
  }
  return listvpor;
}

/*}}}************************************************************************/
/*{{{                    beh_delbevectbux()                                 */
/*                                                                          */
/* mode : recursive delete or not (Y or N)                                  */
/*                                                                          */
/****************************************************************************/
bevectbux_list *beh_delbevectbux(bevectbux_list *listvbux,
                                 bevectbux_list *vbux, char mode)
{
  bevectbux_list   head;
  bevectbux_list  *last;

  if (listvbux && vbux)
  {
    head.NEXT       = listvbux;
    last            = &head;
    while (last && (last->NEXT != vbux))
      last          = last->NEXT;
    if (last)
    {
      if (mode == 'N')
      {
        if (vbux->VECTBIABL)
          beh_warning(317,vbux->NAME,NULL);
          //(stderr,"[BEH_WAR] bevectbux `%s` not empty\n",vbux->NAME);
      }
      else
        beh_frevectbiabl(vbux->VECTBIABL);
      last->NEXT    = vbux->NEXT;
      mbkfree (vbux);
    }
    listvbux        = head.NEXT;
  }

  return listvbux;
}

/*}}}************************************************************************/
/*{{{                    beh_delbevectbus()                                 */
/*                                                                          */
/* mode : recursive delete or not (Y or N)                                  */
/*                                                                          */
/****************************************************************************/
bevectbus_list *beh_delbevectbus(bevectbus_list *listvbus,
                                 bevectbus_list *vbus, char mode)
{
  bevectbus_list   head;
  bevectbus_list  *last;

  if (listvbus && vbus)
  {
    head.NEXT       = listvbus;
    last            = &head;
    while (last && (last->NEXT != vbus))
      last          = last->NEXT;
    if (last)
    {
      if (mode == 'N')
      {
        if (vbus->VECTBIABL)
          beh_warning(318,vbus->NAME,NULL);
          //(stderr,"[BEH_WAR] bevectbus `%s` not empty\n",vbus->NAME);
      }
      else
        beh_frevectbiabl(vbus->VECTBIABL);
      last->NEXT    = vbus->NEXT;
      mbkfree (vbus);
    }
    listvbus        = head.NEXT;
  }

  return listvbus;
}

/*}}}************************************************************************/
/*{{{                    beh_delbevectreg()                                 */
/*                                                                          */
/* mode : recursive delete or not (Y or N)                                  */
/*                                                                          */
/****************************************************************************/
bevectreg_list *beh_delbevectreg(bevectreg_list *listvreg,
                                 bevectreg_list *vreg, char mode)
{
  bevectreg_list   head;
  bevectreg_list  *last;

  if (listvreg && vreg)
  {
    head.NEXT       = listvreg;
    last            = &head;
    while (last && (last->NEXT != vreg))
      last          = last->NEXT;
    if (last)
    {
      if (mode == 'N')
      {
        if (vreg->VECTBIABL)
          beh_warning(319,vreg->NAME,NULL);
          //(stderr,"[BEH_WAR] bevectreg `%s` not empty\n",vreg->NAME);
      }
      else
        beh_frevectbiabl(vreg->VECTBIABL);
      last->NEXT    = vreg->NEXT;
      mbkfree (vreg);
    }
    listvreg        = head.NEXT;
  }

  return listvreg;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    structures' destructors                            */
/****************************************************************************/
/*{{{                    beh_frebevectaux()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frebevectaux(bevectaux_list *listvaux)
{
  bevectaux_list  *vaux;

  while (listvaux)
  {
    freeExpr (listvaux->ABL);
    vaux        = listvaux;
    listvaux    = listvaux->NEXT;
    mbkfree (vaux);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_frebevectout()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frebevectout(bevectout_list *listvout)
{
  bevectout_list  *vout;

  while (listvout)
  {
    freeExpr (listvout->ABL);
    vout        = listvout;
    listvout    = listvout->NEXT;
    mbkfree (vout);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_frebevectpor()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frebevectpor(bevectpor_list *listvpor)
{
  bevectpor_list  *vpor;

  while (listvpor)
  {
    vpor        = listvpor;
    listvpor    = listvpor->NEXT;
    mbkfree (vpor);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_frebevectbux()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frebevectbux(bevectbux_list *listvbux)
{
  bevectbux_list  *vbux;

  while (listvbux)
  {
    beh_frevectbiabl(listvbux->VECTBIABL);
    vbux        = listvbux;
    listvbux    = listvbux->NEXT;
    mbkfree (vbux);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_frebevectbus()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frebevectbus(bevectbus_list *listvbus)
{
  bevectbus_list  *vbus;

  while (listvbus)
  {
    beh_frevectbiabl(listvbus->VECTBIABL);
    vbus        = listvbus;
    listvbus    = listvbus->NEXT;
    mbkfree (vbus);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_frebevectreg()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frebevectreg(bevectreg_list *listvreg)
{
  bevectreg_list  *vreg;

  while (listvreg)
  {
    beh_frevectbiabl(listvreg->VECTBIABL);
    vreg        = listvreg;
    listvreg    = listvreg->NEXT;
    mbkfree (vreg);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    vectbiabl operations                               */
/****************************************************************************/
/*{{{                    beh_addvectbiabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
vectbiabl_list *beh_addvectbiabl(vectbiabl_list *last,
                                 int left, int right, biabl_list *biabl)
{
  vectbiabl_list    *vbiabl;

  vbiabl        = mbkalloc(sizeof(vectbiabl_list));

  vbiabl->NEXT  = last;
  vbiabl->RIGHT = right;
  vbiabl->LEFT  = left;
  vbiabl->BIABL = biabl;

  return vbiabl;
}

/*}}}************************************************************************/
/*{{{                    beh_copybiabl()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
biabl_list *beh_copybiabl(biabl_list *biabl)
{
  biabl_list    *new;

  new       = beh_addbiabl(NULL,biabl->LABEL,biabl->CNDABL,biabl->VALABL);
  new->USER = biabl->USER;
  new->TIME = biabl->TIME;
  new->TIMEVAR = biabl->TIMEVAR;
  new->FLAG = biabl->FLAG;

  return new;
}

/*}}}************************************************************************/
/*{{{                    beh_delvectbiabl()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
vectbiabl_list *beh_delvectbiabl(listvbiabl,vbiabl,mode)
vectbiabl_list  *listvbiabl;
vectbiabl_list  *vbiabl;
char             mode;
{
  vectbiabl_list     head;
  vectbiabl_list    *last;

  if ((listvbiabl)&&(vbiabl))
  {
    head.NEXT       = listvbiabl;
    last            = &head;
    while ((last)&&(last->NEXT!=vbiabl))
      last          = last->NEXT;
    if (last)
    {
      if (mode == 'N')
      {
        if (vbiabl->BIABL)
          beh_warning(320,NULL,NULL);
          //(stderr,"[BEH_WAR] vectbiabl not empty\n");
      }
      else
        beh_frebiabl(vbiabl->BIABL);
      last->NEXT    = vbiabl->NEXT;
      mbkfree(vbiabl);
    }
    listvbiabl      = head.NEXT;
  }
  return listvbiabl;
}

/*}}}************************************************************************/
/*{{{                    beh_invertvectbiabl()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_invertvectbiabl(vectbiabl_list *vbiabl)
{
  if (vbiabl)
  {
    int      tmp;

    beh_invertvectbiabl(vbiabl->NEXT);
    tmp             = vbiabl->LEFT;
    vbiabl->LEFT    = vbiabl->RIGHT;
    vbiabl->RIGHT   = tmp;
    beh_invertbiabl(vbiabl->BIABL);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_invertbiabl()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_invertbiabl(biabl_list *biabl)
{
  if (biabl)
  {
    invertVectAbl(biabl->VALABL);
    beh_invertbiabl(biabl->NEXT);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_frevectbiabl()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_frevectbiabl(vectbiabl_list *listvbiabl)
{
  vectbiabl_list    *vbiabl;

  while (listvbiabl)
  {
    beh_frebiabl(listvbiabl->BIABL);

    vbiabl          = listvbiabl;
    listvbiabl      = listvbiabl->NEXT;
    mbkfree (vbiabl);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_addcndvect2biabl()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_addcndvect2biabl(biabl_list *biabl,char *cndVect)
{
  beh_delcndvectfbiabl(biabl);
  biabl->FLAG      |= BEH_CND_SELECT;
  biabl->USER       = addptype(biabl->USER,BEH_TYPE_CND,namealloc(cndVect));
}

/*}}}************************************************************************/
/*{{{                    beh_addcnddefault2biabl()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_addcnddefault2biabl(biabl_list *biabl)
{
  biabl->FLAG      |= BEH_CND_SEL_OTHERS;
}

/*}}}************************************************************************/
/*{{{                    beh_getcndvectfbiabl()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *beh_getcndvectfbiabl(biabl_list *biabl)
{
  ptype_list    *ptype;

  ptype     = getptype(biabl->USER,BEH_TYPE_CND);
  if (ptype)
    return ptype->DATA;
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_delcndvectfbiabl()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_delcndvectfbiabl(biabl_list *biabl)
{
  if (beh_getcndvectfbiabl(biabl))
    biabl->USER     = delptype(biabl->USER,BEH_TYPE_CND);
}

/*}}}************************************************************************/
/*{{{                    beh_simplifybiabl()                                */
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beh_simplifybiabl(biabl_list *biabl)
{
  int            size;
  int            oper;
  biabl_list    *biablx;
  chain_list    *ablx;
  chain_list    *abl;
  chain_list    *tab[128];
  char           cond[128][128];
  char           buf[128];
  int            i, j, k, cpt;

  if (biabl->FLAG&BEH_CND_SELECT)
    return 1;
  if ((biabl->FLAG&BEH_CND_CMPLX) || (biabl->FLAG&BEH_CND_PRECEDE))
    return 0;

  size      = verifyVectAbl(biabl->CNDABL);
  for (biablx = biabl; biablx; biablx = biablx->NEXT)
    if (size != verifyVectAbl(biablx->CNDABL))
    {
      biabl->FLAG  |= BEH_CND_CMPLX;
      return 0;
    }
  for (j = 0, biablx = biabl; biablx; biablx = biablx->NEXT, j ++)
  {
    ablx    = /*copyExpr*/(biablx->CNDABL);
    i       = 0;
    k       = 0;
    if (ATOM(biablx->CNDABL))
    {
      biabl->FLAG  |= BEH_CND_CMPLX;
      return 0;
    }
    oper    = OPER(biablx->CNDABL);
    ablx    = ablx->NEXT;
    switch (oper)
    {
      case REPLICATE :
           abl  = ablx->DATA;
           size = verifyVectAbl(abl);
           if (!ATOM(abl) && OPER(abl) == NOT)
           {
             abl = notExpr(abl);
             for (cpt = 0; cpt < size; cpt ++)
               cond[j][k++] = '0';
           }
           else
             for (cpt = 0; cpt < size; cpt ++)
               cond[j][k++] = '1';

           if (j)
           {
             if (!equalExpr(abl,tab[i]))
             {
               biabl->FLAG |= BEH_CND_CMPLX;
               return 0;
             }
           }
           else
             tab[i] = abl;
           for (i = 1; i < REP_NB(biablx->CNDABL); i ++)
           {
             if (!j)
               tab[i] = tab[0];
             for (cpt = 0; cpt < size; cpt ++)
               cond[j][k++] = cond[j][cpt];
           }
           break;
      case CAT :
           for ( ; ablx; ablx = ablx->NEXT, i ++)
           {
             abl    = ablx->DATA;
             size   = verifyVectAbl(abl);
             if (!ATOM(abl) && OPER(abl) == NOT)
             {
               abl = notExpr(abl);
               for (cpt = 0; cpt < size; cpt ++)
                 cond[j][k++] = '0';
             }
             else
               for (cpt = 0; cpt < size; cpt ++)
                 cond[j][k++] = '1';
             if (j)
             {
               if (!equalExpr(abl,tab[i]))
               {
                 biabl->FLAG |= BEH_CND_CMPLX;
                 return 0;
               }
             }
             else
               tab[i] = abl;
           }
           break;
      default :
           biabl->FLAG |= BEH_CND_CMPLX;
           return 0;
    }
    cond[j][k] = '\0';
  }

  for (j = 0, biablx = biabl; biablx; biablx = biablx->NEXT, j ++)
  {
    sprintf(buf,"\"%s\"",cond[j]);
    beh_addcndvect2biabl(biablx,namealloc(buf));
    if (j==0)
      for (ablx = biablx->CNDABL->NEXT, i = 0; ablx; ablx = ablx->NEXT)
        ablx->DATA = tab[i++];
    else
      biablx->CNDABL = NULL;
  }

  return 1;
}

/*}}}************************************************************************/
/*{{{                    beh_addothersseltobiabl()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beh_addothersseltobiabl(biabl_list *biabl,chain_list *abl)
{
  if (biabl->FLAG&BEH_CND_SELECT)
  {
    while (biabl->NEXT)
      biabl             = biabl->NEXT;
    biabl->NEXT         = beh_addbiabl(NULL,biabl->LABEL,NULL,abl);
    biabl->NEXT->FLAG   = BEH_CND_SEL_OTHERS;

    return 1;
  }
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    beh_buildvhdvectname()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *beh_buildvhdvectname(char *name, int left, int right)
{
  char       buf[255];

  if (left>right)
    sprintf(buf,"%s (%d DOWNTO %d)",name,left,right);
  else if (left<right)
    sprintf(buf,"%s (%d TO %d)",name,left,right);
  else
    sprintf(buf,"%s (%d)",name,left);

  return mbkstrdup(buf);
}

/*}}}************************************************************************/
/*{{{                    beh_buildvlgvectname()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *beh_buildvlgvectname(char *name, int left, int right)
{
  char       buf[255];
  char *addspace="";
  int i;

  i=strlen(name);
  if (i>0 && name[i-1]!=' ') addspace=" ";

  if (left == right)
    sprintf(buf,"%s%s[%d]",name,addspace,right);
  else
    sprintf(buf,"{%s%s[%d:%d]}",name,addspace,left,right);

  return mbkstrdup(buf);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Loop operation                                     */
/****************************************************************************/
/*{{{                    beh_newloop()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
loop_list *beh_newloop(loop_list *next,char *varname,char *index,
                       int begin,int end)
{
  loop_list *loop;

  loop          = mbkalloc(sizeof(loop_list));
  loop->NEXT    = next;
  loop->VARNAME = namealloc(varname);
  loop->INDEX   = namealloc(index);
  loop->LEFT    = begin;
  loop->RIGHT   = end;

  return loop;
}

/*}}}************************************************************************/
/*{{{                    delloop()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static loop_list *delloop(loop_list *head,loop_list *loop)
{
  if (!head || !loop)
    ;
  else if (head == loop)
  {
    head    = head->NEXT;
    mbkfree(loop);
  }
  else
  {
    loop_list   *loopx;

    for (loopx = head; loopx->NEXT ; loopx = loopx->NEXT)
      if (loopx->NEXT == loop)
      {
        loopx->NEXT = loop->NEXT;
        mbkfree(loop);
        break;
      }
  }

  return head;
}

/*}}}************************************************************************/
/*{{{                    freloop()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static loop_list *freloop(loop_list *loop)
{
  while (loop)
    loop    = delloop(loop,loop);

  return NULL;
}

/*}}}************************************************************************/
/*{{{                    beh_isSameLoop()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beh_isSameLoop(loop_list *loop1, loop_list* loop2)
{
  if (loop1 && loop2 && loop1->VARNAME == loop2->VARNAME &&
      loop1->LEFT == loop2->LEFT && loop1->RIGHT == loop2->RIGHT)
    return 1 + beh_isSameLoop(loop1->NEXT,loop2->NEXT);
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    beh_isloop()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int beh_isloop(biabl_list *biabl)
{
  if (!biabl) return 0;
  return biabl->FLAG&BEH_CND_LOOP;
}

/*}}}************************************************************************/
/*{{{                    beh_addloop()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *beh_addloop( biabl_list *biabl, char *index, char *varname, int begin,
 int end)
{
  loop_list *loop;

  loop     = beh_getloop(biabl);
  index    = namealloc(index);
  if (loop)
  {
    loop_list *loopx;

    for (loopx = loop; loopx; loopx = loopx->NEXT)
    {
      if (index)
        loopx->INDEX = index;
      else if (loopx->INDEX)
        index        = loopx->INDEX;
    }
  }
  loop          = beh_newloop(loop,varname,index,begin,end);
  biabl->FLAG  |= BEH_CND_LOOP;
  biabl->LOOP   = loop;

  return varname;
}

/*}}}************************************************************************/
/*{{{                    beh_getloop()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
loop_list *beh_getloop(biabl_list *biabl)
{
  return biabl->LOOP;
}

/*}}}************************************************************************/
/*{{{                    beh_delloop()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_delloop(biabl_list *biabl)
{
  biabl->LOOP    = freloop(biabl->LOOP);
}

/*}}}************************************************************************/
/*}}}***********************************************************************/
