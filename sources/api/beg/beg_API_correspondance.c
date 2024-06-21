/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : beg_API_correspondance.c                                    */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/*    Warning : this is an included file, it can't be compiled alone        */
/*                                                                          */
/****************************************************************************/

#include AVT_H
#include "beg_API_correspondance.h"

char *BEGAPI_NO_CORRESP = "<nocorresp>";


static void updatecorresp(ht *h);
static void begAddCorrespInHt(ht *htb, void *mbv, char *key, char *name, int mode, int index);

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    begInitCorrespondance()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begInitCorrespondance(void)
{
  mbv_setNoCorrepondance(BEGAPI_NO_CORRESP);
}

/*}}}************************************************************************/
/*{{{                    begGetNamesRad()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *begGetNamesRad(befig_list *befig)
{
  bepor_list     *por;
  bevectpor_list *vpor;
  berin_list     *rin;
  chain_list     *names;

  names     = NULL;
  for (rin = befig->BERIN; rin; rin = rin->NEXT)
    names   = addchain(names, rin->NAME);

  for (vpor = befig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    if (vpor->DIRECTION == 'O' || vpor->DIRECTION == 'Z')
      names = addchain(names, vpor->NAME);

  for (por = befig->BEPOR; por; por = por->NEXT)
    if (por->DIRECTION == 'O' || por->DIRECTION == 'Z')
      names   = addchain(names, por->NAME);

  return names;
}

/*}}}************************************************************************/
/*{{{                    begGetNames()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
chain_list *begGetNames(befig_list *befig)
{
  bepor_list     *por;
  bevectpor_list *vpor;
  berin_list     *rin;
  chain_list     *names;
  char            buf[1024];
  ht *tempht;

  names     = NULL;
  tempht=addht(40);
  for (rin = befig->BERIN; rin; rin = rin->NEXT) {
    addhtitem(tempht, rin->NAME, 0);
    if (rin->LEFT == -1)
      names = addchain(names,rin->NAME);
    else
    {
      sprintf(buf, "%s %d:%d",rin->NAME,rin->LEFT,rin->RIGHT);
      names = addchain(names,namealloc(buf));
    }
  }

  for (vpor = befig->BEVECTPOR; vpor; vpor = vpor->NEXT)
    if (gethtitem(tempht, vpor->NAME)==EMPTYHT)
    {
      sprintf(buf,"%s %d:%d",vpor->NAME,vpor->LEFT,vpor->RIGHT);
      names = addchain(names,namealloc(buf));
    }
  
  for (por = befig->BEPOR; por; por = por->NEXT)
    if (gethtitem(tempht, por->NAME)==EMPTYHT)
      names = addchain(names,por->NAME);

  delht(tempht);
  return names;
}

/*}}}************************************************************************/
/*{{{                    begUpdateChanges()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begUpdateChanges(ht *org, ht *dest, chain_list *before_change,
                      chain_list *after_change)
{
  mbv_changeRef(org,dest,before_change,after_change);
}

/*}}}************************************************************************/
///*{{{                    begWriteCorrespondanceList()                       */
///*                                                                          */
///* TODO : update                                                            */
///****************************************************************************/
//void begWriteCorrespondanceList(char *name)
//{
//  bepor_list        *ptbepor;
//  bevectpor_list    *ptbevpor;
//  berin_list        *ptberin;
//  befig_list        *ptbefig;
//  ht                *corresp_ht = NULL;
//  char              *corresp;
//  int                i;
//  char               buf[1024];
//  FILE              *fp;
//
//  fp            = mbkfopen(name, "cor", WRITE_TEXT);
//
//  ptbefig       = beg_get_befig(BEG_NOVERIF);
//  corresp_ht    = begGetCorrespHT(ptbefig);
//
//  for (ptberin = ptbefig->BERIN; ptberin; ptberin = ptberin->NEXT)
//    if (ptberin->LEFT == -1)
//      if (corresp_ht &&
//          (corresp = (char *)gethtitem(corresp_ht,ptberin->NAME)) != (char *)EMPTYHT)
//        fprintf (fp,"%s : %s\n",bvl_vectorize(ptberin->NAME),corresp);
//      else
//        fprintf (fp,"%s : KO\n",bvl_vectorize(ptberin->NAME));
//    else
//      for (i = ptberin->LEFT; i <= ptberin->RIGHT; i++)
//      {
//        sprintf (buf, "%s %d", ptberin->NAME, i);
//        if (corresp_ht &&
//            (corresp = (char*)gethtitem(corresp_ht, namealloc (buf))) != (char *)EMPTYHT)
//          fprintf (fp,"%s : %s\n",bvl_vectorize(mbkstrdup(buf)),corresp);
//        else
//          fprintf (fp,"%s : KO\n",bvl_vectorize(mbkstrdup(buf)));
//      }
//
//  for (ptbevpor = ptbefig->BEVECTPOR; ptbevpor; ptbevpor = ptbevpor->NEXT)
//    if (ptbevpor->DIRECTION == 'O' || ptbevpor->DIRECTION == 'T')
//      for (i = ptbevpor->LEFT; i <= ptbevpor->RIGHT; i++)
//      {
//        sprintf (buf, "%s %d", ptbevpor->NAME, i);
//        if (corresp_ht &&
//            (corresp = (char*)gethtitem(corresp_ht,namealloc(buf))) != (char*)EMPTYHT)
//          fprintf (fp, "%s : %s\n", bvl_vectorize (strdup (buf)), corresp);
//        else
//          fprintf (fp, "%s : KO\n", bvl_vectorize (strdup (buf)));
//      }
//
//  for (ptbepor = ptbefig->BEPOR; ptbepor; ptbepor = ptbepor->NEXT)
//    if (ptbepor->DIRECTION == 'O' || ptbepor->DIRECTION == 'T')
//    {
//      if (corresp_ht &&
//          (corresp = (char*)gethtitem(corresp_ht,ptbepor->NAME)) != (char*)EMPTYHT)
//        fprintf (fp, "%s : %s\n", bvl_vectorize (ptbepor->NAME), corresp);
//      else
//        fprintf (fp, "%s : KO\n", bvl_vectorize (ptbepor->NAME));
//    }
//
//  fclose (fp);
//}
//
///*}}}************************************************************************/
/*{{{                    begGetCorrespHTCreate()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/*}}}************************************************************************/
/*{{{                    updatecorresp()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void updatecorresp(ht *h)
{
  loins_list    *li;
  locon_list    *lc;

  mbv_prefix(h,CUR_CORRESP_TABLE->GENIUS_INSNAME);

  li    = gen_findinstance(GENIUS_GLOBAL_LOFIG,
                           CUR_CORRESP_TABLE->GENIUS_INSNAME);
  if (!li)
    fprintf(stderr,"could not find blackbox instance '%s' in figure '%s'\n",
            CUR_CORRESP_TABLE->GENIUS_INSNAME, GENIUS_GLOBAL_LOFIG->NAME);
  else
    for (lc = li->LOCON; lc != NULL; lc = lc->NEXT)
      mbv_addCorresp(h,lc->NAME,gen_losigname(lc->SIG));
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begUpdateCorresp(ht *h)
{
  updatecorresp(h);
}

/*}}}************************************************************************/
/*{{{                    begAddCorrespInHt()                                */
/*                                                                          */
/* mbv is NULL if unknown                                                   */
/*                                                                          */
/****************************************************************************/
static void begAddCorrespInHt(ht *htb, void *mbv, char *key, char *name,
                              int mode, int index)
{
  losig_list    *ls;
  char          *corresp;

  ls        = gns_GetCorrespondingSignal(name);

  if ((!mode && ls) || (mode && !gen_wasunused()))
    corresp = (char*)ls->NAMECHAIN->DATA;
  else
    corresp = BEGAPI_NO_CORRESP;

  if (mbv)
    mbv_addAtIndex(mbv,corresp,index);
  else
    mbv_addCorresp(htb,key,corresp);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begAddCorrespName(ht *htb, char *key, char *name)
{
  mbv_addCorresp(htb,key,name);
}

/*}}}************************************************************************/
/*{{{                    begAddCorresp()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begAddCorresp(char *curbefig, char *name)
{
  befig_list    *befig;
  ht            *tb;
  int            left, right, t;
  char           shortname[1024], buf[1024], mbvname[1024];
  
//  befig     = beg_get_befig(BEG_NOVERIF);
  tb        = begGetCorrespHTCreate(curbefig/*befig->NAME*/);
  t         = sscanf(name,"%[^(](%d:%d)",shortname,&left,&right);
  
  switch (t)
  {
    case 1:
         begAddCorrespInHt(tb,NULL,name,name,0,0);
         break;
    case 2:
         sprintf(buf,"%s %d",shortname,left);
         begAddCorrespInHt(tb,NULL,buf,name,0,left);
         break;
    case 3:
         if (left > right)
         {
           t        = right;
           right    = left;
           left     = t;
         }
         for (t = left; t <= right; t ++)
         {
           sprintf(mbvname,"%s %d",shortname,t);
           sprintf(buf,"%s(%d)",shortname,t);
           begAddCorrespInHt(tb,NULL,mbvname,buf,0,t);
         }
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    begTransfertCorresp()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begTransfertCorresp(char *dest, char *source, int update)
{
  ptype_list    *p;
  chain_list    *cl, *ch = NULL;
  ht            *newht, *oright;

  if ((oright=begGetCorrespHT(source))!=NULL)
    {
      newht     = mbv_duplicateht(oright);
      if (update)
        updatecorresp(newht);
      begGetCorrespHTRelink(dest, newht);
    }
/*
  if ((p = getptype(source->USER,BEG_CORRESP_PTYPE)))
  {
    for (cl = (chain_list *)p->DATA; cl; cl = cl->NEXT)
    {
      newht     = dupht((ht *)cl->DATA);
      ch        = addchain(ch,newht);
      if (update)
        updatecorresp(newht);
    }
    dest->USER  = addptype(dest->USER,BEG_CORRESP_PTYPE,ch);
  }
*/
}

/*}}}************************************************************************/
/*{{{                    begTraceAndCorresp()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void begTraceAndCorresp(ht *corresp_ht, char *name, int left, int right,
                        int trace, int corresp)
{
  if (trace || corresp)
  {
    void        *mbv;
    char         vecname[1024], buf[1024];
    int          i;

    name        = namealloc(name);

    if (left > 0)
      mbv       = mbv_initName(corresp_ht,name,left,right);
    else
      mbv       = NULL;

    if (left > right)
    {
      i         = left;
      left      = right;
      right     = i;
    }

    for (i = left; i <= right; i++)
    {
      sprintf(vecname,"%s%c%d)",name,(left < 0)?'\0':'(',i);
      if (trace)
        fprintf(stdout,"    port %s <%s>\n",name,
                gns_GetSignalName(gns_GetCorrespondingSignal(vecname)));
      if (corresp)
      {
        sprintf(buf,"%s%c%d",name,(left < 0)?'\0':' ',i);
        begAddCorrespInHt(corresp_ht,mbv,buf,vecname,1,i);
      }
    }
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
