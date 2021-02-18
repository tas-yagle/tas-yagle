/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : mbk_vector.c                                                */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include <string.h>
#include "mbk_vector.h"
#include AVT_H

static int                       MBV_ELT_SIZE       = 0;
static AdvancedNameAllocator    *MBV_NAME_ALLOCATOR = NULL; 
static char                     *MBV_NO_CORRESP     = 0;

static inline void       addAtIndex(ht *tbl, char *key, char *corresp, int index);
static inline mbv_tab   *newmbv_tab(ht *tbl, char *key, int left, int right);
static inline long       getElt(mbv_tab *table, int index);
static inline void       addElt(mbv_tab *table, int index, long elt);
static inline void       addName(mbv_tab *table, int index, char *corresp, int nameallocate);
static inline char      *transName(mbv_tab *table, int index, char *buf);
static inline char      *getCorresp(ht *tbl, mbv_tab *table, int index, char *name, char *buf, int del);
static inline void       addInHt(ht *tbl, char *key, mbv_tab *table);
static inline void       resize(mbv_tab *table, int index, int dep);
static inline void       reverseTbl(mbv_tab *table);

mbv_tab *mbv_resize(ht *tbl, char *key, int left, int right);

/****************************************************************************/
/*{{{                    Static                                             */
/****************************************************************************/
/*{{{                    addElt()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void addElt(mbv_tab *table, int index, long elt)
{
  table->CPT ++;
  
  if (MBV_NAME_ALLOCATOR)
    table->TABLE.NUM[index]    = (int)elt;
  else
    table->TABLE.NOM[index]  = (char*)elt;
}

/*}}}************************************************************************/
/*{{{                    addName()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void addName(mbv_tab *table, int index, char *corresp,
                           int nameallocate)
{
  table->CPT ++;
  if (MBV_NAME_ALLOCATOR)
    table->TABLE.NUM[index]    =   AdvancedNameAlloc(MBV_NAME_ALLOCATOR,corresp) + 1;
  else if (nameallocate)
    table->TABLE.NOM[index]  = namealloc(corresp);
  else
    table->TABLE.NOM[index]  = corresp;
}

/*}}}************************************************************************/
/*{{{                    getElt()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline long getElt(mbv_tab *table, int index)
{
  if (MBV_NAME_ALLOCATOR)
    return (long)table->TABLE.NUM[index];
  else
    return (long)table->TABLE.NOM[index];
}

/*}}}************************************************************************/
/*{{{                    resize()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void resize(mbv_tab *table, int index, int dep)
{
  int        size, olds;
  void     *str;
  
  if (table->RIGHT < index)
  {
    olds                = table->RIGHT - table->LEFT + 1;
    table->RIGHT        = index + dep;
    size                = table->RIGHT - table->LEFT + 1;
    str                 = mbkalloc(MBV_ELT_SIZE*size);
    //memset(str + olds,0,MBV_ELT_SIZE*(size - olds));
    memset(str,0,MBV_ELT_SIZE*size);
    memcpy(str,table->TABLE.TAB,MBV_ELT_SIZE*olds);
    mbkfree(table->TABLE.TAB);
    table->TABLE.TAB        = str;
  }
  else if (index < table->LEFT)
  {
    olds                = table->RIGHT - table->LEFT + 1;
    table->LEFT         = (index > dep) ? index - dep : 0;
    size                = table->RIGHT - table->LEFT + 1;
    str                 = mbkalloc(MBV_ELT_SIZE*size);
    memset(str,0,MBV_ELT_SIZE*(size - olds));
    //memcpy(str + size - olds,table->TABLE.TAB,MBV_ELT_SIZE*olds);
    memcpy(str + (size - olds)*MBV_ELT_SIZE,table->TABLE.TAB,MBV_ELT_SIZE*olds);
    mbkfree(table->TABLE.TAB);
    table->TABLE.TAB        = str;
  }
}

/*}}}************************************************************************/
/*{{{                    reverseTbl()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void reverseTbl(mbv_tab *table)
{
  long       elt;
  int        size, i;
  
  size             = table->RIGHT - table->LEFT + 1;
  size            /= 2;
  for (i = 0; i < size; i ++)
  {
    elt            = getElt(table,i);
    addElt(table,i,getElt(table,(size-1) - i));
    addElt(table,(size-1) - i,elt);
  }
}

/*}}}************************************************************************/
/*{{{                    addInHt()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void addInHt(ht *tbl, char *key, mbv_tab *table)
{
  addhtitem(tbl,key,(long)table);
}

static inline int addInHtIfNew(ht *tbl, char *key, mbv_tab *table)
{
  if (gethtitem(tbl,key)!=EMPTYHT) return 1;
  addhtitem(tbl,key,(long)table);
  return 0;
}

/*}}}************************************************************************/
/*{{{                    newmbv_tab()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline mbv_tab *newmbv_tab(ht *tbl, char *key, int left, int right)
{
  int        size;
  mbv_tab   *table;

  if (!MBV_ELT_SIZE)
    MBV_ELT_SIZE    = sizeof(char*);
      
  size              = right - left + 1;
  table             = mbkalloc(sizeof(mbv_tab));
  table->LEFT       = left;
  table->RIGHT      = right;
  table->CPT        = 0;
  table->TABLE.TAB      = mbkalloc(MBV_ELT_SIZE * size);
  
  memset(table->TABLE.TAB,0,MBV_ELT_SIZE * size);
 
  if (tbl && key)
    addInHt(tbl,key,table);

  return table;
}

/*}}}************************************************************************/
/*{{{                    dupmbv_tab()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline mbv_tab *dupmbv_tab(mbv_tab *table, char *prefix)
{
  mbv_tab   *res;
  int        i, size;
  char       buf[1024], buf2[1024], *corresp;

  res       = newmbv_tab(NULL,NULL,table->LEFT,table->RIGHT);
  size      = table->RIGHT - table->LEFT + 1;
  if (!prefix)
    for (i = 0; i < size; i++)
      addElt(res,i,getElt(table,i));
  else
    for (i = 0; i < size; i++)
      if ((corresp = transName(table,i,buf)) == MBV_NO_CORRESP ||
          !corresp)
        addName(res,i,MBV_NO_CORRESP,1);
      else
      {
        sprintf(buf2,"%s%c%s",prefix,SEPAR,corresp);
        addName(res,i,buf2,1);
      }

  return res;
}

/*}}}************************************************************************/
/*{{{                    addAtIndex()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline void addAtIndex(ht *tbl, char *key, char *corresp, int index)
{
  mbv_tab   *table;
  
  if (!(table = mbv_get(tbl,key)))
    table       = newmbv_tab(tbl,key,index,index);
  
  mbv_addAtIndex(table,corresp,index);
}

/*}}}************************************************************************/
/*{{{                    transName()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline char *transName(mbv_tab *table, int index, char *buf)
{
  if (MBV_NAME_ALLOCATOR)
  {
    int      name;
    
    if ((name = table->TABLE.NUM[index]))
    {
      AdvancedNameAllocName(MBV_NAME_ALLOCATOR,name-1,buf);

      return buf;
    }
    else
      return NULL;
  }
  else
  {
    char    *name;

    if ((name = table->TABLE.NOM[index]))
    {
      buf[0]      = '\0';
      
      return name;
    }
    else
      return NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    getCorresp()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline char *getCorresp(ht *tbl, mbv_tab *table, int index, char *key,
                               char *buf, int del)
{
  char      *res;
  
  if (index == -1)
  {
    res     = transName(table,0,buf);
    if (del)
      mbv_free(tbl,key);
  }
  else
  {
    res     = transName(table,index - table->LEFT,buf);
    if (del)
    {
      table->CPT --;
      if (!table->CPT)
        mbv_free(tbl,key);
    }
  }

  return res;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Extern                                             */
/****************************************************************************/
/*{{{                    mbv_prefix()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mbv_prefix(ht *tbl, char *prefix)
{
  htitem        *hi;
  long           val;
  int            i;
  
  for (i = 0; i < tbl->length; i ++)
  {
    hi              =& tbl->pElem[i];
    val             = hi->value;
    if (val != EMPTYHT && val != DELETEHT)
      hi->value     = (long)dupmbv_tab((mbv_tab*)val,prefix);
  }
}

/*}}}************************************************************************/
/*{{{                    mbv_setNoCorrepondance()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mbv_setNoCorrepondance(char *noCorrespString)
{
  MBV_NO_CORRESP    = noCorrespString;
}

/*}}}************************************************************************/
///*{{{                    mbv_changeRef()                                    */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void mbv_changeRef(ht *oht, ht *nht, chain_list *old, chain_list *new)
//{
//  int        nleft, nright, oleft, oright, nm, om;
//  int        size, k, ol, or, nl, nr;
//  long       elt;
//  char       nname[1024], oname[1024], buf[1024], *nkey, *okey;
//  mbv_tab   *otable, *ntable;
//
//  
//  for (; old && new; old = delchain(old,old), new = delchain(new,new))
//  {
//    nm              = sscanf(new->DATA,"%s%d:%d",nname,&nleft,&nright);
//    om              = sscanf(old->DATA,"%s%d:%d",oname,&oleft,&oright);
//    
//    ol              = (oleft <= oright) ? oleft  : oright;
//    or              = (oleft <= oright) ? oright : oleft;
//    nl              = (nleft <= nright) ? nleft  : nright;
//    nr              = (nleft <= nright) ? nright : nleft;
//                 
//    nkey            = namealloc(nname);
//    okey            = namealloc(oname);
//    
//    if (nm != om)
//    {
//      //&& (otable = mbv_get(oht,okey)))
//      printf("[BVL_WAR] botched name changing\b");
//      mbkexit(0);
//      /*
//      if ((ntable = mbv_get(nht,nkey )))
//        mbv_addAtIndex(ntable,transName(otable,0,buf),nleft);
//      else
//      {
//        otable->LEFT   = nleft;
//        otable->RIGHT  = nleft;
//        addInHt(nht,nkey ,otable);
//        delhtitem(oht,old->DATA);
//      }
//      */
//    }
//    else
//    {
//      if ((otable = mbv_get(oht,okey )))
//      {
//        switch (nm)
//        {
//          case 3 :
//               ntable           = mbv_get(nht,nkey);
//               if (!ntable && oleft == nleft && oright == nright)
//                 addInHt(nht,nkey ,otable);
//               else if (!ntable && oleft-oright == nleft-nright)
//               {
//                 addInHt(nht,nkey ,otable);
//                 otable->LEFT    = nl;
//                 otable->RIGHT   = nr;
//               }
//               else if (!ntable && oleft-oright == nright-nleft)
//               {
//                 addInHt(nht,nkey ,otable);
//                 otable->LEFT    = nl;
//                 otable->RIGHT   = nr;
//                 size           = or - ol + 1;
//                 size          /= 2;
//                 for (k = 0; k < size; k ++)
//                 {
//                   elt          = getElt(otable,k);
//                   addElt(otable,k,getElt(otable,(size-1) - k));
//                   addElt(otable,(size-1) - k,elt);
//                 }
//               }
//               else
//               {
//                 if (!(ntable = mbv_initName(nht,nkey ,nl,nr)))
//                   if (!(ntable = mbv_resize(nht,nkey ,nl,nr)))
//                     return;
//                 
//                 size           = or - ol + 1;
//
//                 for (k = 0; k < size; k ++)
//                   if (nleft <= nright && oleft <= oright)
//                     addElt(ntable,k + (nl-ntable->LEFT),getElt(otable,k));
//                   else
//                     addElt(ntable,(nr-ntable->RIGHT) - k,getElt(otable,k));
//                   
//                 mbkfree(otable->TABLE);
//                 mbkfree(otable);
//               }
//               break;
//          case 1 :
//               addInHt(nht,nkey ,otable);
//               break;
//          default :
//               fprintf(stderr,"[MBV_ERR] Bad number of argument to data\n");
//        }
//        delhtitem(oht,old->DATA);
//      }
//      else
//        fprintf(stderr,"[MBV_ERR] Bad old name\n");
//      
//    }
//  }
//}
//


ht *mbv_duplicateht(ht *source)
{
  chain_list *cl, *ch;
  char *key;
  ht *newht;

  newht=addht(source->count);

  cl=ch=GetAllHTKeys(source);
  while (cl!=NULL)
    {
      key=(char *)cl->DATA;
      addInHt(newht, key, 
              dupmbv_tab(mbv_get(source,key), NULL));
      cl=cl->NEXT;
    }
  freechain(ch);
  return newht;
}


///*}}}************************************************************************/
/*{{{                    mbv_changeRef()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

void mbv_changeRef(ht *oht, ht *nht, chain_list *old, chain_list *new)
{
  int        nleft, nright, oleft, oright, nm, om;
  int        size, k = 0, ol, or, nl, nr, shift;
  char       nname[1024], oname[1024], buf[1024], *nkey, *okey;
  mbv_tab   *otable, *ntable;
  long       elt;

  
  for (; old && new; old = delchain(old,old), new = delchain(new,new))
  {
    nm              = sscanf(new->DATA,"%s%d:%d",nname,&nleft,&nright);
    om              = sscanf(old->DATA,"%s%d:%d",oname,&oleft,&oright);
    
    nkey            = namealloc(nname);
    okey            = namealloc(oname);
    otable          = mbv_get(oht,okey);
    ntable          = mbv_get(nht,nkey);
    
    delhtitem(oht,okey);
    
    if (!otable)
    {
      if (strncasecmp(okey,"ext@",4)!=0)
        avt_error("mbv", 1, AVT_WAR,"could not find correspondance for name '%s'\n",oname);
    }
    else if (nm != om)
      {
        if (ntable)
          {
            mbv_addAtIndex(ntable,transName(otable,0,buf),nleft);
            mbkfree(otable->TABLE.TAB);
            mbkfree(otable);
          }
        else
          {
            ntable              = otable;
            ntable->LEFT        = nleft;
            ntable->RIGHT       = nleft;
            addInHt(nht,nkey ,ntable);
          }
      }
    else if (nm == 1)
      {
        if (addInHtIfNew(nht,nkey,otable))
          {
            mbkfree(otable->TABLE.TAB);
            mbkfree(otable);
          }
      }
    else if (nm == 2)
    {
      fprintf(stderr,"[MBV_WAR] contact support@avertec.com\n");
      addInHt(nht,nkey,otable);
    }
    else if (nm == 3)
    {
      ol                    = (oleft <= oright) ? oleft  : oright;
      or                    = (oleft <= oright) ? oright : oleft;
      nl                    = (nleft <= nright) ? nleft  : nright;
      nr                    = (nleft <= nright) ? nright : nleft;
                 
      if (or - ol != nr - nl)
        fprintf(stderr,"[MBV_ERR] Botched name\n");
      else
      {
      //  ntable              = mbv_get(nht,nkey);

        if (!ntable && oleft == nleft && oright == nright)
          ntable            = otable;
        else if (!ntable && oleft-oright == nleft-nright)
        {
          ntable            = otable;
          ntable->LEFT      = nl;
          ntable->RIGHT     = nr;
        }
        else if (!ntable && oleft-oright == nright-nleft)
        {
          ntable            = otable;
          ntable->LEFT      = nl;
          ntable->RIGHT     = nr;
          reverseTbl(ntable);
        }
        else
        {
          // if needed the correspondence table is resized
          // or created
          if (!(ntable && ntable->LEFT <= nl && ntable->RIGHT >= nr)
              && !(ntable = mbv_initName(nht,nkey ,nl,nr))
              && !(ntable = mbv_resize(nht,nkey ,nl,nr)) )
            return;
/*
          size              = or - otable->LEFT + 1;
          shift             = nl - ntable->LEFT;
*/
          for (k = ol; k <= or/* size*/; k ++)
            if ((elt = getElt(otable,k-otable->LEFT)))
              addElt(ntable,nl+(k-ol)-ntable->LEFT /*k+shift*/,elt);

          mbkfree(otable->TABLE.TAB);
          mbkfree(otable);
        }
        addInHt(nht,nkey ,ntable);
      }
    }
    else
      fprintf(stderr,"[MBV_ERR] Bad number of argument to data\n");
  }
}

/*}}}************************************************************************/
/*{{{                    mbv_useAdvancedNameAllocator()                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mbv_useAdvancedNameAllocator()
{
  if (!MBV_ELT_SIZE)
  {
    MBV_ELT_SIZE        = sizeof(int);
    MBV_NAME_ALLOCATOR  = CreateAdvancedNameAllocator(CASE_SENSITIVE);
  }  
}

/*}}}************************************************************************/
/*{{{                    mbv_addAtIndex()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mbv_addAtIndex(mbv_tab *table, char *corresp, int index)
{
  resize(table,index,16);
  if (index < 0)
    index               = 0;
  else
    index               = index - table->LEFT;

  if (corresp)
    addName(table,index,corresp,0);
}

/*}}}************************************************************************/
/*{{{                    mbv_resize()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
mbv_tab *mbv_resize(ht *tbl, char *key, int left, int right)
{
  mbv_tab   *table;
  int        tmp;

  if (left > right)
  {
    tmp     = left;
    left    = right;
    right   = tmp;
  }
  
  if (!(table = mbv_get(tbl,key)))
    return NULL;
  else if (right < table->LEFT)
  { 
    resize(table,left,0);
    
    return table;
  }
  else if (table->RIGHT < left)
  {
    resize(table,right,0);

    return table;
  }
  else
  {
    for (tmp = left; tmp <= right; tmp ++)
      if (tmp > table->RIGHT || table->LEFT > tmp)
        continue;
      else if (getElt(table,tmp - table->LEFT))
      {
        avt_error("mbv", 1, AVT_ERR, "Conflict on %s(%d to %d) to merge with %s(%d to %d), cannot resize\n", key, left, right, key, table->LEFT, table->RIGHT);
//        fprintf(stderr,"[MBV_ERR] Conflict can not resize\n");
        return NULL;
      }
    resize(table,left,0);
    resize(table,right,0);

    return table;
  }
}

/*}}}************************************************************************/
/*{{{                    mbv_initName()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
mbv_tab *mbv_initName(ht *tbl, char *key, int left, int right)
{
  mbv_tab   *table;
  int        tmp;

  if (left > right)
  {
    tmp     = left;
    left    = right;
    right   = tmp;
  }
  
  if (!(table = mbv_get(tbl,key)))
    return newmbv_tab(tbl,key,left,right);
  else
    return NULL;
}

/*}}}************************************************************************/
/*{{{                    mbv_addCorresp()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mbv_addCorresp(ht *tbl, char *key, char *corresp)
{
  char       buf[1024], test;
  int        index, res;

  switch (sscanf(key,"%s %d%c",buf,&index,&test))
  {
    case 1 :
         index  = -1;
    case 2 :
         key    = namealloc(buf);
         addAtIndex(tbl,key,corresp,index);
         res    = 1;
         break;
    default :
         res    = 0;
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    mbv_get()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
mbv_tab *mbv_get(ht *tbl, char *key)
{
  mbv_tab   *table;
  
  if ((table=(mbv_tab*)gethtitem(tbl,key)) == (mbv_tab*)EMPTYHT ||
      table == (mbv_tab*)DELETEHT)
    return NULL;
  else
    return table;
}

ht *mbv_buildglobalht(chain_list *htl)
{
  ht *h=addht(10000), *j;
  chain_list *entries;
  while (htl!=NULL)
  {
    j=(ht *)htl->DATA;
    entries=GetAllHTKeys(j);
    while (entries!=NULL)
    {
      addhtitem(h, entries->DATA, gethtitem(j, entries->DATA));
      entries=delchain(entries, entries);
    }
    htl=htl->NEXT;
  }
  return h;
}
/*}}}************************************************************************/
///*{{{                    mbv_getFromVect()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//mbv_tab *mbv_getFromVect(ht *tbl, char *key)
//{
//  mbv_tab   *table;
//  char       rad[1024];
//  
//  sscanf(key,"%s",rad);
//  if ((table = (mbv_tab*)gethtitem(tbl,namealloc(rad))) == (mbv_tab*)EMPTYHT)
//    return NULL;
//  else
//    return table;
//}
//
///*}}}************************************************************************/
///*{{{                    mbv_addFromVect()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void mbv_addFromVect(ht *tbl, char *key, mbv_tab *data)
//{
//  char       rad[1024];
//  
//  sscanf(key,"%s",rad);
//  addhtitem(tbl,namealloc(rad),(long)data);
//}
//
///*}}}************************************************************************/
///*{{{                    mbv_del()                                          */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void mbv_del(ht *tbl, char *key)
//{
//  mbv_tab   *table;
//  char       buf[1024], test, *res;
//  int        index;
//
//  switch (sscanf(key,"%s %d%c",buf,&index,&test))
//  {
//    case 1 :
//         index      = -1;
//    case 2 :
//         if ((table = mbv_get(tbl,buf)))
//         {
//           if (index == -1)
//             res    = table->TABLE[0];
//           else
//             res    = table->TABLE[index - table->LEFT];
//         }
//         else
//           res      = NULL;
//         break;
//    default :
//         res    = NULL;
//  }
//}
//
///*}}}************************************************************************/
/*{{{                    mbv_free()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mbv_free(ht *tbl, char *key)
{
  mbv_tab   *table;
  
  if ((table = mbv_get(tbl,key)))
  {
    mbkfree(table->TABLE.TAB);
    mbkfree(table);
  }
  delhtitem(tbl,key);
}

void mbv_freeht(ht *source)
{
  chain_list *cl, *ch;
  char *key;

  cl=ch=GetAllHTKeys(source);
  while (cl!=NULL)
    {
      key=(char *)cl->DATA;
      mbv_free(source, key);
      cl=cl->NEXT;
    }
  freechain(ch);
  delht(source);
}

/*}}}************************************************************************/
/*{{{                    mbv_getCorresp()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *mbv_getCorresp(ht *tbl, char *key, char *buf)
{
  mbv_tab   *table;
  char       name[1024], test, *res;
  int        index;

  switch (sscanf(key,"%s %d%c",name,&index,&test))
  {
    case 1 :
         index      = -1;
    case 2 :
         key        = namealloc(name);
         if ((table = mbv_get(tbl,key )))
           res      = getCorresp(tbl,table,index,key,buf,0);
         else
           res      = NULL;
         break;
    default :
         res        = NULL;
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    mbv_getCorrespAndDel()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *mbv_getCorrespAndDel(ht *tbl, char *key, char *buf)
{
  mbv_tab   *table;
  char       name[1024], test, *res;
  int        index;

  switch (sscanf(key,"%s %d%c",name,&index,&test))
  {
    case 1 :
         index      = -1;
    case 2 :
         key       = namealloc(name);
         if ((table = mbv_get(tbl,key)))
           res      = getCorresp(tbl,table,index,key,buf,1);
         else
           res      = NULL;
         break;
    default :
         res        = NULL;
  }

  return res;
}

/*}}}************************************************************************/
/*{{{                    mbv_dumpCorresp()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mbv_dumpCorresp(ht *table)
{
  htitem        *hi;
  long           val;
  int            i, j, size;
  mbv_tab       *tab;
  char           buf[1024], *name;
  
  for (i = 0; i < table->length; i ++)
  {
    hi              = &(table->pElem[i]);
    val             = hi->value;
    if (val != EMPTYHT && val != DELETEHT)
    {
      tab   = (mbv_tab*)val;
      size  = tab->RIGHT - tab->LEFT + 1;
      for (j = 0; j < size; j ++)
      {
        name        = transName(tab,j,buf);
        name        = name ? name : "nothing";
        printf("%s(%d) -> %s\n", (char*)hi->key,j+tab->LEFT,name);
      }
    }
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
#if 0
/*{{{                    Test functions                                     */
/****************************************************************************/
#include <stdlib.h>

/****************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/* void mbv_changeRef(ht *orig, ht *dest, chain_list *old, chain_list *new) */
/*                                                                          */
/****************************************************************************/
void testchangeref()
{
#define TSIZE   16
  ht            *old[TSIZE], *new[TSIZE], *top;
  //int            mem[]  = {1024,3145728};
  //int            test   = 0;
  int            i, j;
  char           buf[TSIZE], *corresp[TSIZE], *oname[TSIZE], *nname;
  chain_list    *och, *nch;
 
  
  top               = addht(15);
  for (j = 0; j < TSIZE; j ++)
  {
    new[j]          = addht(15);
    mbv_initName(new[j],namealloc("mem"),0,TSIZE-1);
    
    sprintf(buf,"mem_%d",j);
    oname[j]        = mbkstrdup(buf);
    
    for (i = 0; i < TSIZE; i ++)
    {
      old[i]        = addht(15);
      sprintf(buf,"cell_%d_mem",i+j*TSIZE);
      corresp[i]    = mbkstrdup(buf);

      mbv_addCorresp(old[i],oname[j],corresp[i]);

      //printf("drive %d : ",i);
      //mbv_dumpCorresp(old[i]);
    }
    for (i = 0; i < TSIZE; i ++)
    {
      och           = addchain(NULL,oname[j]);

      sprintf(buf,"mem %d",i);
      nname         = mbkstrdup(buf);
      nch           = addchain(NULL,nname);

      mbv_changeRef(old[i],new[j],och,nch);

      mbkfree(nname);
    }
    
    mbkfree(oname[j]);
    
    
    sprintf(buf,"mem %d:%d",0,TSIZE -1);
    oname[0]        = mbkstrdup(buf);
    och             = addchain(NULL,oname[0]);
    
    if (j%2)
      sprintf(buf,"mem %d:%d",(j+1)*TSIZE -1,j*TSIZE);
    else
      sprintf(buf,"mem %d:%d",j*TSIZE,(j+1)*TSIZE -1);
    nname           = mbkstrdup(buf);
    nch             = addchain(NULL,nname);

    mbv_changeRef(new[j],top,och,nch);
    
    //mbv_dumpCorresp(new[j]);
  }
  printf("top ---\n");
  mbv_dumpCorresp(top);
/*
  for (i = 0; i < TSIZE; i ++)
  {
    mbkfree(corresp[i]);
    delht(old[i]);
  }
  */
}

/*}}}************************************************************************/
/*{{{                    main()                                             */
/*                                                                          */
/* />gcc -o main -L$AVT_DISTRIB_DIR/lib -Xlinker -z -Xlinker allextract \
   -lMut325 -lstdc++ -lnsl -ldl -lm -Xlinker -z -Xlinker defaultextract     */
/* />main                                                                   */
/****************************************************************************/
int main(int argc, char *argv[])
{
  char  *testbench [] =
  {
    "sig0"              ,"testsig0"             ,
    "sig 0"             ,"testsig(0)"           ,
    "sig(0)"            ,"testsig(0)"           ,
    "sig 10"            ,"testsig(10)"          ,
    "sig 58"            ,"testsig(58)"          ,
    "sig 15"            ,"testsig(15)"          ,
    "sig 13"            ,"testsig(13)"          ,
    "sig 45"            ,"testsig(45)"          ,
    "sig1 58"           ,"testsig1(58)"         ,
    "sig1 98"           ,"testsig1(98)"         ,
    "sig1 10"           ,"testsig1(10)"         ,
  };
//  chain_list    *old, *new;
  char          *res, buf[1024];
  int            i, nbbench, nberr = 0;
  ht*            tbl    = addht(50);
  
  mbkenv();
 
//  mbv_useAdvancedNameAllocator();
  tbl       = addht(50);
  nbbench   = (int)(sizeof(testbench)/sizeof(char*))/2;
  for (i = 0; i < nbbench; i ++ )
    mbv_addCorresp(tbl,testbench[i*2],testbench[i*2+1]);

//  mbv_prefix(tbl,"prefix");
  for (i = 0; i < nbbench; i ++ )
  {
    res     = mbv_getCorresp(tbl,testbench[i*2],buf);
    if (!res)
    {
      nberr ++;
      printf("[MBV_ERR] not found corresp for : %s\n",testbench[i*2]);
    }
    else if (strcmp(res,testbench[i*2+1]))
    {
      nberr ++;
      printf("[MBV_ERR] expected : %s obtained : %s\n",testbench[i*2+1],res);
    }
  }
  printf("[MBV_TEST] %d/%d errors\n",nberr,nbbench);

  printf("--- testing changeref ---\n\n");
  testchangeref();
  printf("\n--- done ---\n");
  
  
  return EXIT_SUCCESS;
  argc  = 0;
  argv  = NULL;
}

/*}}}************************************************************************/
#endif
