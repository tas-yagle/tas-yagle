/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : beh_gnsConflictingName.c                                    */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "beh_gnsConflictingName.h"

//#define BEH_AVT_SUPPORT(num) (stderr,"[BEH_ERR] %d please contact Avertec support\n",(num))
#define BEH_UNDO_NAME (long)(-9)
        
static ht   *HTSIGREF;

/****************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    addInHt()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void addInHt(ht *signalht, char *ref, char *name)
{
  chain_list    *c;

  c         = (chain_list*)gethtitem(signalht,ref);
  if (c == (chain_list*)EMPTYHT || c == (chain_list*)DELETEHT)
    c       = NULL;

  addhtitem(signalht,ref,(long)addchain(c,name));
}

/*}}}************************************************************************/
/*{{{                    renameSig()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *renameSig(char *name)
{
  char          *res;

  res           = (char*)gethtitem(HTSIGREF,name);
  if (res == (char*)EMPTYHT || res == (char*)DELETEHT)
    res         = name;

  return res;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/****************************************************************************/
/*{{{                    beh_createName()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_createName(ht *signalht, char *(*func)(char *,chain_list*))
{
  htitem        *hi;
  chain_list    *listname;
  char          *oldname;
  int           i;

  if (signalht)
    for (i = 0; i < signalht->length; i ++)
    {
      hi            =& signalht->pElem[i];
      listname      = (chain_list*)hi->value;
      if (listname != (chain_list*)EMPTYHT)
      {
        oldname     = (char*)hi->key;
        if (func)
          hi->value = (long)func(oldname,listname);
        else
          hi->value = (long)(listname->DATA);
        
        freechain(listname);
      }
    }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_dumpHTName(ht *nameht, int mode)
{
  htitem        *hi;
  chain_list    *listname, *chainx;
  char          *oldname;
  int           i;

  if (nameht)
    for (i = 0; i < nameht->length; i ++)
    {
      hi            =& nameht->pElem[i];
      listname      = (chain_list*)hi->value;
      if (listname != (chain_list*)EMPTYHT)
      {
        oldname     = (char*)hi->key;
        
        if (mode)
        {
          beh_warning(312,oldname,(char*)listname->DATA);
          //(stderr,"%20s -> %s\n",oldname,(char*)listname->DATA);
          for (chainx = listname->NEXT; chainx; chainx = chainx->NEXT)
            beh_warning(313," ",(char*)chainx->DATA);
            //(stderr,"%23s %s\n"," ",(char*)chainx->DATA);
        }
        else
          beh_warning(312,oldname,(char*)listname);
          //(stderr,"%20s -> %s\n",oldname,(char*)listname);
      }
    }
}

/*}}}************************************************************************/
/*{{{                    beh_changeNameInCnsBefigFromSignalHt()             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_changeNameInside(ht *signalht, befig_list *fig)
{
  if (signalht)
  {
    bepor_list  *por;
    
    if (fig->BEVECTPOR)
    {
      char buf[16];

      sprintf(buf,"%d",__LINE__);
      beh_toolbug(199,buf,NULL,0);
      //BEH_AVT_SUPPORT(__LINE__);
      //EXIT(0);
    }
    // discard top level interface renaming
    for (por = fig->BEPOR; por; por = por->NEXT)
      addhtitem(signalht,por->NAME,(long)por->NAME);
    
    HTSIGREF        = signalht;
    beh_namewrap(fig,renameSig);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_changeNameInterface()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_changeNameInterface(ht *signalht, befig_list *fig)
{
  ptype_list        *hlcor, *ptx, *lcor, *nlcor;
  bepor_list        *por;
  berin_list        *rin;
  bevectpor_list    *vpor;
  char              *oldname, *newname, buf1[1024], buf2[1024];
  ht                *porht, *rinht;
  void              *tmp;
  int                n, o, i;

  if (signalht)
  {
    porht           = addht(40);
    rinht           = addht(40);

    for (rin = fig->BERIN; rin; rin = rin->NEXT)
      addhtitem(rinht,rin->NAME,(long)rin);

    nlcor           = NULL;
    if ((hlcor = getptype(fig->USER,BEG_RENAME_LIST)))
    {
      lcor          = hlcor->DATA;
      for (ptx = lcor; ptx; ptx = ptx->NEXT)
      {
        newname     = (char*)gethtitem(signalht,ptx->DATA);
        oldname     = (char*)ptx->TYPE;
        n           = sscanf(newname,"%[^ ] %d",buf1,&i);
        o           = sscanf(oldname,"%[^ ] %d",buf2,&i);
        if (o == 1)
          addhtitem(porht,oldname,(long)newname);
        else if (o == n)
          addhtitem(porht,namealloc(buf2),(long)namealloc(buf1));
        else
          addptype(nlcor,(long)oldname,newname);
      }
      freeptype(lcor);
      hlcor->DATA   = nlcor;
      
    }
    
    for (por = fig->BEPOR; por; por = por->NEXT)
      if ((newname = (char*)gethtitem(porht,por->NAME)) != (char*)EMPTYHT)
      {
        rin         = (berin_list*)gethtitem(rinht,por->NAME);
        if (rin == (berin_list *)EMPTYHT)
          rin       = NULL;
        beh_renameport(fig,por,rin,newname);
      }

    for (vpor = fig->BEVECTPOR; vpor; vpor = vpor->NEXT)
      if ((newname = (char*)gethtitem(porht,vpor->NAME)) != (char*)EMPTYHT)
      {
        rin         = (berin_list*)gethtitem(rinht,vpor->NAME);
        if (rin == (berin_list *)EMPTYHT)
          rin       = NULL;
        beh_renamevectport(fig,vpor,rin,newname,0);
      }
      else
      {
        if (vpor->DIRECTION != 'I')
          for (ptx = nlcor; ptx; ptx = ptx->NEXT)
          {
            sscanf((char*)ptx->TYPE,"%[^ ] %d",buf2,&i);
            if (namealloc(buf2) == vpor->NAME)
            {
              tmp       = ptx->DATA;
              ptx->DATA = (void*)ptx->TYPE;
              ptx->TYPE = (long)tmp;
            }
          }
      }

    delht(porht);
    delht(rinht);
  }
}

/*}}}************************************************************************/
/*{{{                    beh_addRefName()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void beh_addRefName(ht **signalht, befig_list *fig)
{
  ptype_list    *pt;

  if ((pt = getptype(fig->USER,BEG_RENAME_LIST)))
  {
    if (!*signalht)
      *signalht     = addht(40);
    for (pt = pt->DATA; pt; pt = pt->NEXT)
      addInHt(*signalht,(char*)pt->DATA,(char*)pt->TYPE);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
