/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : YAGLE  v1.n                                                 */
/*    Fichier : info.c                                                      */
/*                                                                          */
/*    (c) copyright 1991 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Auteur(s) : Marc LAURENTIN                        le : 06/09/1991     */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include MUT_H
#include INF_H
#include AVT_H
#include "inf_grab.h"

//#define ONEINF

static void infCleanRegistry(inffig_list *ifl, int location);
void infrestart( FILE *input_file ) ;
	
extern FILE    *infin;
extern int      inflineno;

extern char     INF_LANG;

int             infParseLine, inf_ignorename=0;
static int INFDISPLAYLOAD=0;
int             infparse();


static int STUCK_SECTION=INF_DEFAULT_LOCATION|INF_LOADED_LOCATION;

char *infFile, *infname;

int inf_StuckSection(int val)
{
  int old=STUCK_SECTION;
  STUCK_SECTION=val;
  return old;
}

int inf_DisplayLoad(int val)
{
  int old=INFDISPLAYLOAD;
  INFDISPLAYLOAD=val;
  return old;
}
/****************************************************************************
 *                         fonction infRead();                              *
 ****************************************************************************/

/*-------------------------------*
  | lit le fichier <filename.inf> |
  *------------------------------*/
static ht *INF_HT=NULL;
inffig_list *INF_FIG;

int inf_getinffig_state(inffig_list *ifl)
{
  return ifl->changed;
}

void inf_resetinffig_state(inffig_list *ifl)
{
  ifl->changed=0;
}

inffig_list *getloadedinffig(char *name)
{
  long l;

  name=namealloc(name);

  if (INF_HT==NULL) INF_HT=addht(16);
  if ((l=gethtitem(INF_HT, name))==EMPTYHT) return NULL;
  return (inffig_list *)l;
}

inffig_list *addinffig(char *name)
{
  inffig_list *ifig;

  name=namealloc(name);

  if ((ifig=getloadedinffig(name))!=NULL)
    delinffig(name);
    
  ifig=(inffig_list *)mbkalloc(sizeof(inffig_list));
  ifig->NAME=name;
  ifig->USER=NULL;
  ifig->changed=0;
  addhtitem(INF_HT, name, (long)ifig);
  
  memset(&ifig->DEFAULT, 0, sizeof(INF_INFOSET));
  memset(&ifig->LOADED, 0, sizeof(INF_INFOSET));
  return ifig;
}

void inf_renameinffig(char *orig, char *newname)
{
  inffig_list *ifl;

  if ((ifl=getloadedinffig(orig))!=NULL)
  {
    delhtitem(INF_HT, ifl->NAME);    
    ifl->NAME=namealloc(newname);
    delinffig(ifl->NAME);
    addhtitem(INF_HT, ifl->NAME, (long)ifl);
  }
}

void infclone(char *name, inffig_list *ifig)
{
  name=namealloc(name);
  
  if (ifig==NULL) return;

  addhtitem(INF_HT, name, (long)ifig);
}

void delinffig(char *name)
{
  inffig_list *ifig;
  chain_list *all, *cl;
  
  name=namealloc(name);

  if ((ifig=getloadedinffig(name))!=NULL)
    {
      // pour etre sure d'eliminer tous les clones pointant
      // sur la meme inffig
      all=GetAllHTKeys(INF_HT);
      for (cl=all; cl!=NULL; cl=cl->NEXT)
       {
        if (gethtitem(INF_HT, cl->DATA)==(long)ifig)
          delhtitem(INF_HT, cl->DATA);
       }
      freechain(all);
      // ---------

      infClean(ifig, INF_LOADED_LOCATION);
	  if (ifig->LOADED.REGISTRY!=NULL) delht(ifig->LOADED.REGISTRY);
      infClean(ifig, INF_DEFAULT_LOCATION);
	  if (ifig->DEFAULT.REGISTRY!=NULL) delht(ifig->DEFAULT.REGISTRY);
      freeptype(ifig->USER);
      mbkfree(ifig);
//      delhtitem(INF_HT, name);
    }
}

inffig_list *getinffig(char *name)
{
  inffig_list *ifl;
  ptype_list *pt=NULL;

  if ((ifl=getloadedinffig(name))==NULL 
      || (pt=getptype(ifl->USER, INF_IS_DIRTY))!=NULL
      || getptype(ifl->USER,INF_LOADED_FROM_DISK)==NULL
      )
    {
      ifl=infRead(name, 1);
  //    if (pt!=NULL && ifl!=NULL) ifl->USER=delptype(ifl->USER, INF_IS_DIRTY);
    }

  return ifl;
}

static chain_list *Hidden=NULL;

static int countwild(char *val)
{
  int i, cnt=0;
  for (i=0;val[i]!='\0';i++) if (val[i]!='*') cnt++;
  return cnt;
}
static int infsortnames(const void *a0, const void *b0)
{
  char *a=*(char **)a0, *b=*(char **)b0;
  int ca, cb;
  ca=countwild(a);
  cb=countwild(b);
  if (strcmp(a,"default")==0)
  {
    if (a==b) return 0;
    return 1;
  }
  if (strcmp(b,"default")==0)
  {
    if (a==b) return 0;
    return -1;
  }
  if (ca>cb) return -1;
  if (ca<cb) return 1;
  return strcmp(a, b);
}
        
chain_list *inf_SortEntries(chain_list *cl)
{
  int nb, i;
  char **tab;
  chain_list *ch;
  nb=countchain(cl);
  tab=mbkalloc(sizeof(char *)*nb);
  for (i=0, ch=cl;i<nb; ch=ch->NEXT, i++) tab[i]=(char *)ch->DATA;

  qsort(tab, nb, sizeof(char *), infsortnames);

  for (i=0, ch=cl;i<nb; ch=ch->NEXT, i++) ch->DATA=tab[i];
  mbkfree(tab);
  return cl;
}

void inf_PushAndHideInf(char *name)
{
  inffig_list *ifl;

  if ((ifl=getloadedinffig(name))!=NULL)
    {
      delhtitem(INF_HT, ifl->NAME);
      Hidden=addchain(Hidden, ifl);
    }
  else
    Hidden=addchain(Hidden, NULL);
}

void inf_PopInf()
{
  inffig_list *ifl;

  if (Hidden!=NULL)
    {
      ifl=(inffig_list *)Hidden->DATA;
      if (ifl!=NULL) addhtitem(INF_HT, ifl->NAME, (long)ifl);
      Hidden=delchain(Hidden, Hidden);
    }
}

void inf_Dirty(char *name)
{
  chain_list *cl, *all;
  inffig_list *ifl;
  if (name==NULL)
    all=GetAllHTElems(INF_HT);
  else
    {
      if ((ifl=getloadedinffig(name))!=NULL) 
        all=addchain(NULL, ifl);
      else
        return;
    }
  for (cl=all; cl!=NULL; cl=cl->NEXT)
    {
      ifl=(inffig_list *)cl->DATA;
      infClean(ifl, INF_LOADED_LOCATION);  
      if (getptype(ifl->USER, INF_IS_DIRTY)==NULL) 
        ifl->USER=addptype(ifl->USER, INF_IS_DIRTY, NULL);
    }
  freechain(all);
}

extern int infdebug;

inffig_list *_infRead(char *figname, char *filename, char silent)
{
  static int vierge = 1 ;
  char buf[1024];

  INF_LANG = 'E';
  sprintf (buf, "%s", filename);
  infFile = buf;

  //  if (vierge == 0) infclean();
  infin = mbkfopen(filename, NULL, READ_TEXT);
  if (infin != NULL) 
    {
      infParseLine = 1;
      if (vierge == 0) infrestart(infin) ;
      vierge = 0 ;
      INF_ERROR = 0;
      INF_FIG=NULL;
      infname=namealloc(figname);
//      infdebug=1;
      infparse();
      fclose(infin) ;
      if (INFDISPLAYLOAD) avt_error("inf", -1, AVT_INFO, "loading '%s' ... %s\n", filename, INF_ERROR?"failed":"ok");
      if ((silent & INF_DONTLOG)==0 && AVTLOGFILE!=NULL)
      {
#if 0
        fprintf(AVTLOGFILE, ">>>> STATE OF INF FOR FIGURE '%s' AFTER READING FILE '%s'\n\n", INF_FIG==NULL||inf_ignorename?infname:INF_FIG->NAME, filename); 
        if (INF_FIG!=NULL) infDrive(INF_FIG, NULL, INF_LOADED_LOCATION|INF_DEFAULT_LOCATION, AVTLOGFILE);
        fprintf(AVTLOGFILE, "\n<<<<\n\n"); 
#else
        avt_log(LOGCONFIG, 1, "Loading information file '%s' for figure '%s'\n", filename, INF_FIG==NULL||inf_ignorename?infname:INF_FIG->NAME); 
                
        infin = mbkfopen(filename, NULL, READ_TEXT);
        if (infin != NULL) 
        {
          mbk_dumpfile(infin, AVTLOGFILE, 9);
          fclose(infin);
        }
        
        avt_log(LOGCONFIG, 1, " -- Current information taken from inf file --\n");         
        inf_set_print_mode(1);
        infDrive (INF_FIG, "dummy", INF_LOADED_LOCATION, AVTLOGFILE);
        inf_set_print_mode(0);
        avt_log(LOGCONFIG, 1, " --                End                      --\n");         

#endif
      }
      return INF_FIG;//getloadedinffig(namealloc(filename));
    }
  else
    if (!(silent & 1))
      avt_errmsg(INF_ERRMSG, "012", AVT_WARNING, filename);
     // avt_error("inf", 1, AVT_WAR, "can not open '%s' information file\n", filename);
  return NULL;
}

static chain_list *inf_split(char *varname)
{
  char *str;
  char *l, *tmp;
  char buf[1024];
  chain_list *cl=NULL;

  str=avt_gethashvar(varname);
  if (str!=NULL)
    {
      strcpy(buf,str);
      l=strtok_r(buf, ",", &tmp);	
      do
        {
          cl=addchain(cl, mbkstrdup(l));
          l=strtok_r(NULL, ",", &tmp);
        } while (l!=NULL);
    }
  return reverse(cl);
}

static void inf_freechain(chain_list *cl)
{
  chain_list *ch;
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    mbkfree(ch->DATA);
  freechain(cl);
}


char *inf_GetDefault_AVT_INF_Value()
{
  if (mbk_GetREGEXMODE()=='C')
    return "$\\.spice\\.inf,$\\.sdc\\.inf,$\\.inf";
  else
    return "$.spice.inf,$.sdc.inf,$.inf";
}

inffig_list *infRead(char *name, char silent)
{
  chain_list *regx, *files, *cl;
  inffig_list *ifl;

  regx=inf_split("AVT_INF");
  if (regx==NULL) return getloadedinffig(namealloc(name));

  files=BuildFileList(name, regx, 0); 
  inf_freechain(regx);
 

  if (!(files!=NULL && files->NEXT==NULL && strcasecmp(files->DATA, "no")==0))
    {
      for (cl=files; cl!=NULL; cl=cl->NEXT)
        {
          ifl=_infRead(name, (char *)cl->DATA, silent);
        }
    }
  inf_freechain(files);
  return getloadedinffig(namealloc(name));
}


/*
static void double_list_clean(chain_list **ch)
{
  chain_list *ptchain;
  for (ptchain = *ch; ptchain; ptchain = ptchain->NEXT) {
    freechain((chain_list *)ptchain->DATA);
  } 
  freechain(*ch);
  *ch=NULL;
}
*/
static void ptype_list_clean(chain_list **ch)
{
  chain_list *ptchain;
  for (ptchain = *ch; ptchain; ptchain = ptchain->NEXT) {
    freeptype((ptype_list *)ptchain->DATA);
  } 
  freechain(*ch);
  *ch=NULL;
}

void infClean(inffig_list *ifl, int location)
{
  INF_INFOSET *is;
  
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    is=&ifl->LOADED;
  
  ptype_list_clean(&is->INF_FALSEPATH);
  ptype_list_clean(&is->INF_FALSESLACK);

  infCleanRegistry(ifl, location);
}

static chain_list *merge_chain(chain_list *first, chain_list *second, int checkdup)
{
  chain_list *res, *cl;

  res=NULL;
  for (cl=first; cl!=NULL; cl=cl->NEXT) res=addchain(res, cl->DATA);
  while (second!=NULL)
    {
      if (checkdup)
        {
          for (cl=first; cl!=NULL; cl=cl->NEXT)
            if (cl->DATA==second->DATA) break;
        }
      else 
        cl=NULL;
      if (cl==NULL)
        res=addchain(res, second->DATA);
      second=second->NEXT;
    }
  return reverse(res);
}
/*
static chain_list *merge_ptype(ptype_list *first, ptype_list *second, int checkdup)
{
  chain_list *res;
  ptype_list *cl;

  res=NULL;
  for (cl=first; cl!=NULL; cl=cl->NEXT) res=addchain(res, cl);
  while (second!=NULL)
    {
      if (checkdup)
        {
          for (cl=first; cl!=NULL; cl=cl->NEXT)
            if (cl->DATA==second->DATA) break;
        }
      else 
        cl=NULL;
      if (cl==NULL)
        res=addchain(res, second);
      second=second->NEXT;
    }
  return reverse(res);
}
*/
static chain_list *merge_siglist(list_list *first, list_list *second, int checkdup)
{
  list_list *icl;
  chain_list *res;

  res=NULL; 
  for (icl=first; icl!=NULL; icl=icl->NEXT) res=addchain(res, icl);
  
  while (second!=NULL)
    {
      if (checkdup)
        {
          for (icl=first; icl!=NULL; icl=icl->NEXT)
            if(icl->DATA==second->DATA && icl->TYPE==second->TYPE) break;
        }
      else icl=NULL;
          
      if (icl==NULL)
        res=addchain(res, second);
      second=second->NEXT; 
    }
  return reverse(res);
}

chain_list *infGetInfo(inffig_list *myfig, int type)
{
  if (myfig==NULL) return NULL;
  switch(type)
    {
    case INF_FALSEPATH_INFO : return merge_chain(myfig->LOADED.INF_FALSEPATH, myfig->DEFAULT.INF_FALSEPATH, 0);
    case INF_SIGLIST_INFO : return merge_siglist(myfig->LOADED.INF_SIGLIST, myfig->DEFAULT.INF_SIGLIST, 1);
    default :
      return NULL;
      }
}

int infHasInfo(inffig_list *myfig, int type)
{
  if (myfig==NULL) return 0;
  switch(type)
    {
    case INF_FALSEPATH_INFO : return myfig->LOADED.INF_FALSEPATH!=NULL || myfig->DEFAULT.INF_FALSEPATH!=NULL;
    case INF_SIGLIST_INFO : return myfig->LOADED.INF_SIGLIST!=NULL || myfig->DEFAULT.INF_SIGLIST!=NULL;
    default :
      return 0;
      }
}


/***********************************************************************
 *                         fonction inf_GetList();                     *
 **********************************************************************/
list_list *inf_GetList (inffig_list *myfig, char *name, long type)
{
  list_list *list;
  chain_list *cl, *maincl;

  maincl=infGetInfo(myfig, INF_SIGLIST_INFO);

  for (cl=maincl; cl!=NULL; cl=cl->NEXT)
    {
      list=(list_list *)cl->DATA;
      if (list->DATA==name && list->TYPE==type) return list;
    }

  freechain(maincl);
  return NULL;
}


/***********************************************************************
 *                         fonction infAddList();                      *
 **********************************************************************/
list_list *infAddList(inffig_list *ifl, int location, char *data,long level,char prg, void *user, char *where)
{
  list_list *list, *endlist = NULL, *indefault;
  short flag = 0;
  INF_INFOSET *addlist;

  if (location==INF_LOADED_LOCATION)
    {
      for(indefault= ifl->DEFAULT.INF_SIGLIST ; indefault != NULL ; indefault = indefault->NEXT)
        if(indefault->DATA==data && indefault->TYPE==level) break;
      
      addlist=&ifl->LOADED;
    }
  else
    {
      addlist=&ifl->DEFAULT;
      indefault=NULL;
    }

  for(list = addlist->INF_SIGLIST ; list != NULL ; list = list->NEXT)
    {
      if((list->DATA == data) && (list->TYPE == level))
        flag = 1 ;

      endlist = list ;
    }

  if((flag != 0) && (prg != 't'))
    {
        avt_errmsg(INF_ERRMSG, "013", AVT_WARNING, where==NULL?"":where, data);
        // avt_error("inf", 2, AVT_WAR, "%sinformation on signal %s already read -- ignored\n",where==NULL?"":where, data);
      return NULL;
    }

  if (indefault!=NULL)
    {
        avt_errmsg(INF_ERRMSG, "014", AVT_WARNING, where==NULL?"":where, data);
        // avt_error("inf", 3, AVT_WAR, "%sinformation on signal '%s' has already been set elsewhere -- overriding with inf values\n",where==NULL?"":where, data);
    }

  list = mbkalloc(sizeof(struct list)) ;
  list->DATA = data ;
  list->TYPE = level ;
  list->USER = user ;

  list->NEXT = addlist->INF_SIGLIST;
  addlist->INF_SIGLIST=list;
  ifl->changed|=1;

  return list;
}

list_list *infaddll(list_list *head, void *data, long level, ptype_list *user)
{
  list_list *list, *last;
  list = mbkalloc(sizeof(list_list)) ;
  list->DATA = data ;
  list->TYPE = level ;
  list->USER = user ;
  list->NEXT = NULL;
  
  for (last=head; last!=NULL && last->NEXT!=NULL; last=last->NEXT) ;
  if (last!=NULL)
    {
      last->NEXT=list;
      return head;
    }

  return list;
}

void inffreell(list_list *ptlist)
{
  list_list *ptnextlist;
  while (ptlist!=NULL)
    {
      ptnextlist = ptlist->NEXT;
      mbkfree(ptlist);
      ptlist = ptnextlist;
    }
}

static void inffreedatavalue(InfValue *iv)
{
  if (iv->datatype=='s' && iv->VAL.string!=NULL) mbkfree(iv->VAL.string);
  else if (iv->datatype=='p')
    {
      if (strcasecmp(iv->pointertype, INF_SPECIN)==0)
        inf_stb_parse_spec_free((inf_stb_p_s *)iv->VAL.pointer);
      else if (strcasecmp(iv->pointertype, INF_ASSOCLIST)==0)
        {
          chain_list *cl;
          for (cl=(chain_list *)iv->VAL.pointer; cl!=NULL; cl=cl->NEXT)
            mbkfree(cl->DATA);
          freechain((chain_list *)iv->VAL.pointer);
        }
      else if (strcasecmp(iv->pointertype, INF_MISCDATA)==0)
        {
          mbkfree(iv->VAL.pointer);
        }
    }
}

static void inffreevalue(InfValue *iv)
{
  inffreedatavalue(iv);
  mbkfree(iv);
}

static InfValue *infcreateentrydata(char *type, char datatype)
{
  InfValue *iv;

  iv=(InfValue *)mbkalloc(sizeof(InfValue));
  iv->infotype=type; //sensitive_namealloc(type);
  iv->datatype=datatype;
  iv->pointertype="?";
  iv->match = 0;
  return iv;
}

static InfValue *infgetentrydata(chain_list *cl, char *type)
{
  while (cl!=NULL && ((InfValue *)cl->DATA)->infotype!=type) cl=cl->NEXT;
  if (cl==NULL) return NULL;
  return (InfValue *)cl->DATA;
}

static chain_list *infgetvals(INF_INFOSET *is, char *key)
{
  long l;
  if (is->REGISTRY==NULL) return NULL; 
  if ((l=gethtitem(is->REGISTRY, key))!=EMPTYHT) return (chain_list *)l;
  return NULL;
}

static InfValue *infgetorcreatekey(INF_INFOSET *is, char *key, char *type, char datatype)
{
  InfValue *iv;
  chain_list *vals;

  key=namealloc(key);
  vals=infgetvals(is, key);
  if ((iv=infgetentrydata(vals, type))==NULL)
    {
      if (is->REGISTRY==NULL) is->REGISTRY=addht(16);
      iv=infcreateentrydata(type, datatype);
      if (datatype=='s') iv->VAL.string=NULL;
      else if (datatype=='p') iv->VAL.pointer=NULL;
      vals=addchain(vals, iv);
      addhtitem(is->REGISTRY, key, (long)vals);
    }
  return iv;
}

static InfValue *infgetkey(INF_INFOSET *is, char *key, char *type)
{
  InfValue *iv;
  chain_list *vals;

  key=namealloc(key);
  vals=infgetvals(is, key);
  iv=infgetentrydata(vals, type);

  return iv;
}

inf_assoc *inf_createassoc()
{
  inf_assoc *assoc;
  assoc=(inf_assoc *)mbkalloc(sizeof(inf_assoc));
  return assoc;
}
inf_miscdata *inf_createmiscdata()
{
  inf_miscdata *assoc;
  assoc=(inf_miscdata *)mbkalloc(sizeof(inf_miscdata));
  return assoc;
}

void inf_AddString(inffig_list *ifl, int location, char *key, char *type, char *val, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  int sayit=0;
  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
      if ((iv=infgetkey(&ifl->DEFAULT, key, type))!=NULL)
        {
          if (iv->VAL.string==NULL || val==NULL)
            {
              if (val!=iv->VAL.string) sayit=1;
            }
          else if (strcmp(iv->VAL.string, val)!=0) sayit=1;
          if (sayit)
            avt_errmsg(INF_ERRMSG, "015", AVT_WARNING, where==NULL?"":where, type, key, iv->VAL.string, val);
            // avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set to '%s' elsewhere -- overriding with inf value '%s'\n",where==NULL?"":where, type, key, iv->VAL.string, val);
        }
    }

  iv=infgetorcreatekey(is, key, type, 's');
  inffreedatavalue(iv);
  if (val!=NULL) iv->VAL.string=mbkstrdup(val);
  else iv->VAL.string=NULL;
  ifl->changed|=1;
}

void inf_AddPointer(inffig_list *ifl, int location, char *key, char *type, char *pointertype, void *val, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
      if (infgetkey(&ifl->DEFAULT, key, type)!=NULL)
        avt_errmsg(INF_ERRMSG, "016", AVT_WARNING, where==NULL?"":where, type, key);
        // avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set elsewhere -- overriding with inf values\n",where==NULL?"":where, type, key);
    }

  iv=infgetorcreatekey(is, key, type, 'p');
  iv->pointertype=pointertype;
//  inffreedatavalue(iv);
//  if (val!=NULL) iv->VAL.string=mbkstrdup(val);
  iv->VAL.pointer=val;
  ifl->changed|=1;
}

void inf_AddMiscData(inffig_list *ifl, int location, char *key, char *type, char *val0, char *val1, long lval, double dval, double dval1, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  inf_miscdata *assoc;

  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
      if (infgetkey(&ifl->DEFAULT, key, type)!=NULL)
        avt_errmsg(INF_ERRMSG, "016", AVT_WARNING, where==NULL?"":where, type, key);
        // avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set elsewhere -- overriding with inf values\n",where==NULL?"":where, type, key);
    }

  iv=infgetorcreatekey(is, key, type, 'p');
  iv->pointertype=INF_MISCDATA;
  if (iv->VAL.pointer==NULL)
    {
      assoc=inf_createmiscdata();
      iv->VAL.pointer=assoc;
    }
  else
    assoc=iv->VAL.pointer;

  assoc->orig=val0;
  assoc->dest=val1;
  assoc->lval=lval;
  assoc->dval=dval;
  assoc->dval1=dval1;
  ifl->changed|=1;
}

void inf_AddDouble(inffig_list *ifl, int location, char *key, char *type, double val, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
/*      if ((iv=infgetkey(&ifl->DEFAULT, key, type))!=NULL && fabs(iv->VAL.dvalue-val)>1e-13)
        avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set to %g elsewhere -- overriding with inf value %g\n",where==NULL?"":where, type, key, iv->VAL.dvalue, val);*/
    }
  iv=infgetorcreatekey(is, key, type, 'd');
  iv->VAL.dvalue=val;
  where=NULL;
  ifl->changed|=1;
}

void inf_AddInt(inffig_list *ifl, int location, char *key, char *type, int val, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
      if ((iv=infgetkey(&ifl->DEFAULT, key, type))!=NULL && iv->VAL.ivalue!=val)
        avt_errmsg(INF_ERRMSG, "017", AVT_WARNING, where==NULL?"":where, type, key, iv->VAL.ivalue, val);
        // avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set to %d elsewhere -- overriding with inf value %d\n",where==NULL?"":where, type, key, iv->VAL.ivalue, val);
    }
  iv=infgetorcreatekey(is, key, type, 'i');
  iv->VAL.ivalue=val;
  ifl->changed|=1;
}

int inf_GetString(inffig_list *ifl, char *key, char *type, char **val)
{
  InfValue *iv=NULL;
  if (ifl==NULL) return 0;
  type=sensitive_namealloc(type);
  if ((STUCK_SECTION & INF_LOADED_LOCATION)!=0) iv=infgetkey(&ifl->LOADED, key, type);
  if (iv==NULL || iv->datatype!='s')
    {
      if ((STUCK_SECTION & INF_DEFAULT_LOCATION)!=0) iv=infgetkey(&ifl->DEFAULT, key, type);
      if (iv==NULL || iv->datatype!='s')
        return 0;
    }
  *val=iv->VAL.string;
  return 1;
}

int inf_GetPointer(inffig_list *ifl, char *key, char *type, void **val)
{
  InfValue *iv=NULL;
  if (ifl==NULL) return 0;
  type=sensitive_namealloc(type);
  if ((STUCK_SECTION & INF_LOADED_LOCATION)!=0) iv=infgetkey(&ifl->LOADED, key, type);
  if (iv==NULL || iv->datatype!='p')
    {
      if ((STUCK_SECTION & INF_DEFAULT_LOCATION)!=0) iv=infgetkey(&ifl->DEFAULT, key, type);
      if (iv==NULL || iv->datatype!='p')
        return 0;
    }
  *val=iv->VAL.pointer;
  return 1;
}

int inf_GetDouble(inffig_list *ifl, char *key, char *type, double *val)
{
  InfValue *iv=NULL;
  if (ifl==NULL) return 0;
  type=sensitive_namealloc(type);
  if ((STUCK_SECTION & INF_LOADED_LOCATION)!=0) iv=infgetkey(&ifl->LOADED, key, type);
  if (iv==NULL || iv->datatype!='d')
    {
      if ((STUCK_SECTION & INF_DEFAULT_LOCATION)!=0) iv=infgetkey(&ifl->DEFAULT, key, type);
      if (iv==NULL || iv->datatype!='d')
        return 0;
    }
  *val=iv->VAL.dvalue;
  return 1;
}

int inf_GetInt(inffig_list *ifl, char *key, char *type, int *val)
{
  InfValue *iv=NULL;
  if (ifl==NULL) return 0;
  type=sensitive_namealloc(type);
  if ((STUCK_SECTION & INF_LOADED_LOCATION)!=0) iv=infgetkey(&ifl->LOADED, key, type);
  if (iv==NULL || iv->datatype!='i')
    {
      if ((STUCK_SECTION & INF_DEFAULT_LOCATION)!=0) iv=infgetkey(&ifl->DEFAULT, key, type);
      if (iv==NULL || iv->datatype!='i')
        return 0; 
    }
  *val=iv->VAL.ivalue;
  return 1;
}

chain_list *inf_GetEntriesByType(inffig_list *ifl, char *type, char *val)
{
  chain_list *cl, *all, *ret=NULL;
  InfValue *iv;
  ht *tempht;

  if (ifl==NULL) return NULL;

  type=sensitive_namealloc(type);
  tempht=addht(16);

  if ((STUCK_SECTION & INF_LOADED_LOCATION)!=0 && ifl->LOADED.REGISTRY!=NULL)
    {
      all=GetAllHTKeys(ifl->LOADED.REGISTRY);
      for (cl=all; cl!=NULL; cl=cl->NEXT)
        {
          iv=infgetkey(&ifl->LOADED, (char*)cl->DATA, type);
          if (type==NULL || (iv!=NULL && (val==INF_ANY_VALUES || (iv->datatype=='s' && strcmp(iv->VAL.string, val)==0) || (iv->datatype=='i' && iv->VAL.ivalue==(int)(long)val))))
            {
              ret=addchain(ret, (char*)cl->DATA);
              addhtitem(tempht, (char*)cl->DATA, 0);
            }
        }      
      freechain(all);
    }

  if ((STUCK_SECTION & INF_DEFAULT_LOCATION)!=0 && ifl->DEFAULT.REGISTRY!=NULL)
    {
      all=GetAllHTKeys(ifl->DEFAULT.REGISTRY);
      for (cl=all; cl!=NULL; cl=cl->NEXT)
        {
          if (gethtitem(tempht, (char*)cl->DATA)==EMPTYHT)
            {
              iv=infgetkey(&ifl->DEFAULT, (char*)cl->DATA, type);
              if (type==NULL || (iv!=NULL && (val==INF_ANY_VALUES || (iv->datatype=='s' && strcmp(iv->VAL.string, val)==0) || (iv->datatype=='i' && iv->VAL.ivalue==(int)(long)val))))
                {
                  ret=addchain(ret, (char*)cl->DATA);
                }
            }
        }      
      freechain(all);
    }

  delht(tempht);
  return inf_SortEntries(ret);
}

chain_list *inf_GetValuesByType(inffig_list *ifl, char *type)
{
  chain_list *cl, *all, *ret=NULL;
  InfValue *iv;
  ht *tempht;
  NameAllocator na;
  char *nn;

  if (ifl==NULL || type==NULL) return NULL;
  type=sensitive_namealloc(type);
  tempht=addht(16);
  CreateNameAllocator(127, &na, 'n');
  if ((STUCK_SECTION & INF_LOADED_LOCATION)!=0 && ifl->LOADED.REGISTRY!=NULL)
    {
      all=GetAllHTKeys(ifl->LOADED.REGISTRY);
      for (cl=all; cl!=NULL; cl=cl->NEXT)
        {
          iv=infgetkey(&ifl->LOADED, (char*)cl->DATA, type);
          if (iv!=NULL && iv->datatype=='s')
            {
              nn=NameAlloc(&na, iv->VAL.string);
              if (gethtitem(tempht, nn)==EMPTYHT)
                {
                  ret=addchain(ret, iv->VAL.string);
                  addhtitem(tempht, nn, 0);
                }
              addhtitem(tempht, (char*)cl->DATA, 0);
            }
        }      
      freechain(all);
    }

  if ((STUCK_SECTION & INF_DEFAULT_LOCATION)!=0 && ifl->DEFAULT.REGISTRY!=NULL)
    {
      all=GetAllHTKeys(ifl->DEFAULT.REGISTRY);
      for (cl=all; cl!=NULL; cl=cl->NEXT)
        {
          if (gethtitem(tempht, (char*)cl->DATA)==EMPTYHT)
            {
              iv=infgetkey(&ifl->DEFAULT, (char*)cl->DATA, type);
              if (iv!=NULL && iv->datatype=='s')
                {
                 nn=NameAlloc(&na, iv->VAL.string);
                 if (gethtitem(tempht, nn)==EMPTYHT)
                   {
                     ret=addchain(ret, iv->VAL.string);
                     addhtitem(tempht, nn, 0);
                   }
                }
            }      
        }
      freechain(all);
    }
  DeleteNameAllocator(&na);
  delht(tempht);
  return ret;
}

chain_list *inf_GetLocatedEntriesByType(inffig_list *ifl, int location, char *type, char *val)
{
  chain_list *cl, *all, *ret=NULL;
  InfValue *iv;
  INF_INFOSET *is;
  
  if (ifl==NULL) return NULL;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    is=&ifl->LOADED;

  if (is->REGISTRY!=NULL)
    {
      all=GetAllHTKeys(is->REGISTRY);
      for (cl=all; cl!=NULL; cl=cl->NEXT)
        {
          iv=infgetkey(&ifl->LOADED, (char*)cl->DATA, type);
          if (type==NULL || (iv!=NULL && (val==NULL || (iv->datatype=='s' && strcmp(iv->VAL.string, val)==0) || (iv->datatype=='i' && iv->VAL.ivalue==(int)(long)val))))
            {
              ret=addchain(ret, (char*)cl->DATA);
            }
        }      
      freechain(all);
    }
  return ret;
}

void inf_RemoveKey(inffig_list *ifl, int location, char *key, char *type)
{
  chain_list *ret=NULL, *ch, *prec;
  INF_INFOSET *is;

  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    is=&ifl->LOADED;

  if (is->REGISTRY==NULL) return;
  
  ret=infgetvals(is, key);  
  if (type==NULL)
    {
      for (ch=ret; ch!=NULL; ch=ch->NEXT) inffreevalue((InfValue *)ch->DATA);
      freechain(ret);
      delhtitem(is->REGISTRY, key);
    }
  else
    {
      prec=NULL;
      for (ch=ret; ch!=NULL; prec=ch, ch=ch->NEXT)
        if (strcasecmp(((InfValue *)ch->DATA)->infotype, type)==0) break;

      if (ch!=NULL)
        {
          inffreevalue((InfValue *)ch->DATA);
          if (prec!=NULL) prec->NEXT=ch->NEXT;
          else addhtitem(is->REGISTRY, key, (long)ch->NEXT);
          ch->NEXT=NULL;
          freechain(ch);
        }
    }
  ifl->changed|=1;
}

static void infCleanRegistry(inffig_list *ifl, int location)
{
  chain_list *cl, *all;
  INF_INFOSET *is;

  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    is=&ifl->LOADED;

  if (is->REGISTRY==NULL) return;

  all=GetAllHTKeys(is->REGISTRY);
  for (cl=all; cl!=NULL; cl=cl->NEXT)
    {
      inf_RemoveKey(ifl, location, (char*)cl->DATA, NULL);
    }  
  freechain(all);
  ifl->changed|=1;
}

static char *__getrel(char val)
{
  if (val==INF_STB_BEFORE) return "before";
  if (val==INF_STB_AFTER) return "after";
  return "?";
}

static char *__getstab(char val)
{
  if (val==INF_STB_STABLE) return "stable";
  if (val==INF_STB_UNSTABLE) return "unstable";
  return "?";
}

static char *__getedge(char val)
{
  if (val==INF_STB_RISING) return "rising";
  if (val==INF_STB_FALLING) return "falling";
  if (val==INF_STB_SLOPEALL) return "(all)";
  return "?";
}

/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int
getVerificationLevel(char *token)
{
  // I
  if (!strcmp(token,INF_MUXU)
      || !strcmp(token,INF_MUXD)
      || !strcmp(token,INF_CMPU)
      || !strcmp(token,INF_CMPD)
      || !strcmp(token,INF_INPUTS)
      || !strcmp(token,INF_PIN_RISING_SLEW)
      || !strcmp(token,INF_PIN_FALLING_SLEW)
      || !strcmp(token,INF_PIN_LOW_VOLTAGE)
      || !strcmp(token,INF_PIN_HIGH_VOLTAGE)
      || !strcmp(token,INF_SLOPEIN)
      || !strcmp(token,INF_CAPAOUT)
      || !strcmp(token,INF_CAPAOUT_LW)
      || !strcmp(token,INF_CAPAOUT_L)
      || !strcmp(token,INF_OUTPUT_CAPACITANCE)
      || !strcmp(token,INF_ASYNCHRON)
      || !strcmp(token,INF_CONNECTOR_DIRECTION)
      || !strcmp(token,INF_SPECIN)
      || !strcmp(token,INF_SPECOUT))
    return 1;
  // II
  if (!strcmp(token,INF_STOP)
      || !strcmp(token,INF_DIROUT)
      || !strcmp(token,INF_CKLATCH)
      || !strcmp(token,INF_BREAK)
      || !strcmp(token,INF_RC)
      || !strcmp(token,INF_CLOCK_TYPE)
      || !strcmp(token,INF_MIN_RISE_TIME)
      || !strcmp(token,INF_MAX_RISE_TIME)
      || !strcmp(token,INF_MIN_FALL_TIME)
      || !strcmp(token,INF_MAX_FALL_TIME)
      || !strcmp(token,INF_CLOCK_PERIOD))
    return 2;
  // III
  if (!strcmp(token,INF_DLATCH)
      || !strcmp(token,INF_NOTLATCH)
      || !strcmp(token,INF_KEEP_TRISTATE_BEHAVIOUR)
      || !strcmp(token,INF_PRECHARGE)
      || !strcmp(token,INF_MODELLOOP)
      || !strcmp(token,INF_STRICT_SETUP)
      || !strcmp(token,INF_STUCK)
      || !strcmp(token,INF_BYPASS)
      || !strcmp(token,INF_INTER)
      //|| token,INF_LL_PATHSIGS)
      || !strcmp(token,INF_PATHIN)
      || !strcmp(token,INF_PATHOUT)
      || !strncmp(token,INF_PATHDELAYMARGINPREFIX,strlen(INF_PATHDELAYMARGINPREFIX))
      || !strcmp(token,INF_NORISING)
      || !strcmp(token,INF_NOFALLING)
      || !strcmp(token,INF_NOCHECK)
      || !strcmp(token,INF_CROSSTALKMUXU)
      || !strcmp(token,INF_CROSSTALKMUXD))
    return 3;
  // ??
  if (!strcmp(token,INF_IGNORE_INSTANCE)
      || !strcmp(token,INF_IGNORE_TRANSISTOR)
      || !strcmp(token,INF_IGNORE_CAPACITANCE)
      || !strcmp(token,INF_IGNORE_RESISTANCE)
      || !strcmp(token,INF_IGNORE_PARASITICS)
      || !strcmp(token,INF_IGNORE_NAMES)
      || !strcmp(token,INF_VERIF_STATE)
      || !strcmp(token,INF_ASYNC_CLOCK_GROUP)
      || !strcmp(token,INF_ASYNC_CLOCK_GROUP_PERIOD)
      || !strcmp(token,INF_EQUIV_CLOCK_GROUP)
      || !strcmp(token,INF_PREFERED_CLOCK))
    return -1;
  else
    return -1;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#define INF_VERIFICATION_LEVEL_MAX 50
#define INF_VERIFICATION_DONE 10

static int
getNFillMinVerificationLevel(chain_list *ch, int b)
{
  if (ch)
  {
    int a, min;
    
    a = getVerificationLevel(((InfValue *)ch->DATA)->infotype);
    /*
    if (a != -1)
      printf("        OK  \"%s\"\n",((InfValue *)ch->DATA)->infotype);
    else
      printf(">>>> FAILED \"%s\"\n",((InfValue *)ch->DATA)->infotype);
    */
    if ((a == -1 && b >= 0) || a > b)
      min = getNFillMinVerificationLevel(ch->NEXT,b);
    else
      min = getNFillMinVerificationLevel(ch->NEXT,a);
    ((InfValue *)ch->DATA)->match =  min;
    
    return min;
  }
  else
    if (b == INF_VERIFICATION_LEVEL_MAX)
      return -1;
    else
      return b;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
markChecked(chain_list *ch, int level)
{
  if (ch)
  {
    ((InfValue *)ch->DATA)->match = INF_VERIFICATION_DONE+level;
    markChecked(ch->NEXT, level);
  }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *
checkKeys(FILE * f, char *key,INF_INFOSET *is, int level)
{
  chain_list *ret;
  int match;
  
  ret = infgetvals(is, namealloc(key));
  match = ((InfValue *)ret->DATA)->match;
  
  if (match == 0)
    match = getNFillMinVerificationLevel(ret,INF_VERIFICATION_LEVEL_MAX);
  
  if ((match < INF_VERIFICATION_DONE && match == level) || (match >= INF_VERIFICATION_DONE && match!=INF_VERIFICATION_DONE-level))
    return key;
  
  f=NULL; // supress warning
  return NULL;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
markKeyChecked(char *key,INF_INFOSET *is, int level)
{
  chain_list *ret;
  
  ret = infgetvals(is, namealloc(key));
  
  markChecked(ret,level);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
checkSections(FILE *f, inffig_list *ifl, char *token, int level, chain_list *data, ht *sig_ht)
{
  static ht *th = NULL;
  chain_list *keys, *c1, *cx;
  int done;
  char *st;

  if (ifl==NULL)
  {
    if (th!=NULL) delht(th);
    th = NULL;
    return;
  }

  if (!th)
    th = addht(50);
  
  if (level == 2 && (!strcmp(token,INF_MUXU)
                     || !strcmp(token,INF_MUXD)
                     || !strcmp(token,INF_CMPU)
                     || !strcmp(token,INF_CMPD)
                     || !strcmp(token,INF_DISABLE_GATE_DELAY)))
    {
      if (!strcmp(token,INF_DISABLE_GATE_DELAY))
        keys=grab_inf_assoc(ifl, INF_DISABLE_GATE_DELAY);
      else
        keys = grab_inf_Mutex(ifl);
      
      for (c1 = keys; c1; c1 = c1->NEXT)
        {
          done = 0;
          st = (char*)gethtitem(th,c1->DATA);
          if (st == (char*)EMPTYHT)
            {
              if (gethtitem(sig_ht, c1->DATA)==EMPTYHT)
                {            
                  for (cx = data; cx; cx = cx->NEXT)
                    {
                      /*st = (char*)gethtitem(th,cx->DATA);
                        if (st != (char*)EMPTYHT && strchr((char *)c1->DATA,'*')==NULL)
                        continue;*/
                      if (mbk_TestREGEX(cx->DATA,c1->DATA) || mbk_TestREGEX(mbk_vect(cx->DATA,'[',']'),c1->DATA))
                        {
                          done = 1;
                          break;
                        }
                    }
                }
              else
                done=1;
              if (!done)
                avt_errmsg(INF_ERRMSG, "023", AVT_WARNING, (char *)c1->DATA,level==1?"connector":level==2?"net":"cone");
//                avt_error("inf",-1,AVT_ERROR,"name '¤2%s¤.' doesn't match any signal in circuit\n",(char *)c1->DATA);
              addhtitem(th,c1->DATA,done);
            }
        }
      
      freechain(keys);
    }
  if (level == 3 && (!strcmp(token,INF_CROSSTALKMUXU)
                     || !strcmp(token,INF_CROSSTALKMUXD)))
    {
      keys = grab_inf_CrosstalkMutex(ifl);
      
      for (c1 = keys; c1; c1 = c1->NEXT)
        {
          done = 0;
          st = (char*)gethtitem(th,c1->DATA);
          if (st == (char*)EMPTYHT)
            {
              if (gethtitem(sig_ht, c1->DATA)==EMPTYHT)
                {
                  for (cx = data; cx; cx = cx->NEXT)
                    {
                      if (mbk_TestREGEX(cx->DATA,c1->DATA) || mbk_TestREGEX(mbk_vect(cx->DATA,'[',']'),c1->DATA))
                        {
                          done = 1;
                          break;
                        }
                    }
                }
              else
                done=1;
              if (!done)
                avt_errmsg(INF_ERRMSG, "023", AVT_WARNING, (char *)c1->DATA,level==1?"connector":level==2?"net":"cone");
//                avt_error("inf",-1,AVT_ERROR,"%name '¤2%s¤.' doesn't match any signal in circuit\n",(char *)c1->DATA);
              addhtitem(th,c1->DATA,done);
            }
        }
      freechain(keys);
    }
  f=NULL; // supress warning
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/* level       composition of data                                          */
/*    I     chain_list of pin name                                          */
/*   II        ''      of signal name                                       */
/*  III        ''      of cones name                                        */
/*                                                                          */
/****************************************************************************/
void 
inf_CheckRegistry(FILE *f, inffig_list *ifl, int level, chain_list *data)
{
  chain_list *cl, *cx, *all;
  INF_INFOSET *is;
  char *key, *str;
  int done;
  static int init = 0;
  ht *sig_ht;

  switch (level)
  {
    case 1: avt_log(LOGCONFIG, 0, "Check level I (connectors)"); break;
    case 2: avt_log(LOGCONFIG, 0, "Check level II (nets)"); break;
    case 3: avt_log(LOGCONFIG, 0, "Check level III (cones)"); break;
  }
  if (!V_BOOL_TAB[__INF_CONF_CHECK].VALUE)
  {
    avt_log(LOGCONFIG, 0, " disabled\n");
    return;
  }
  else
    avt_log(LOGCONFIG, 0, "\n");

  
  if (ifl==NULL)
    return;

  if (0==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    is=&ifl->LOADED;

  if (is->REGISTRY==NULL)
    return;

  sig_ht=addht(1000);
  for (cx = data; cx; cx = cx->NEXT) addhtitem(sig_ht, cx->DATA, 0);

  all = GetAllHTKeys(is->REGISTRY);
  
  if (!init)
    for (cl=all; cl!=NULL; cl=cl->NEXT)
      if ((key = (char*)cl->DATA) && strcasecmp(key,"default"))
      {
        if (key[0] != '+')
       /*   checkSections(f,ifl,key,level,data);
        else */
          checkKeys(f,key,is,level);
      }
  
  for (cl=all; cl!=NULL; cl=cl->NEXT)
  {
    done = 0;
    key = (char*)cl->DATA;
    if (strcasecmp(key,"default"))
    {
      if (key[0] == '+')
        checkSections(f,ifl,key,level,data,sig_ht);
      else if (checkKeys(f,key,is,level))
      {  
        if (gethtitem(sig_ht, key)!=EMPTYHT)
          done=1;
        else {
          for (cx = data; cx; cx = cx->NEXT)
            {
              if (mbk_TestREGEX(cx->DATA,key) || mbk_TestREGEX(mbk_vect(cx->DATA,'[',']'),key))
                {
                  done = 1;
                  markKeyChecked(key,is,level);
                  break;
                }
            }
        }
        if (!done)
          avt_errmsg(INF_ERRMSG, "023", AVT_WARNING, key,level==1?"connector":level==2?"net":"cone");
//          avt_error("inf",-1,AVT_ERROR,"name '¤2%s¤.' doesn't match any signal in circuit\n",key);
      }
    }
  }

  if (level==2) checkSections(f,ifl,INF_DISABLE_GATE_DELAY,level,data,sig_ht);

  freechain(all);
  delht(sig_ht);
  // reinitialisation au dernier level
  if (level==3) checkSections(NULL, NULL, NULL, 0, NULL, NULL);
  return;
}

/*}}}************************************************************************/

void inf_DumpRegistry (FILE *f, inffig_list *ifl, int location)
{
  chain_list *cl, *all, *ret, *ch;
  INF_INFOSET *is;
  InfValue *iv;
  char *type;

  if (ifl==NULL) return;

  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    is=&ifl->LOADED;

  if (is->REGISTRY==NULL) return;

  all=GetAllHTKeys(is->REGISTRY);
  for (cl=all; cl!=NULL; cl=cl->NEXT)
    {
      fprintf(f, "key: \"%s\"\n",(char*)cl->DATA);
      ret=infgetvals(is, namealloc((char*)cl->DATA));
      for (ch=ret; ch!=NULL; ch=ch->NEXT)
        {
          iv=(InfValue *)ch->DATA;
          switch(iv->datatype)
            {
            case 'i' : type="int"; break;
            case 'd' : type="real"; break;
            case 's' : type="string"; break;
            case 'p' : type="pointer"; break;
            default : type="unknown";
            }
          fprintf(f, "    item: \"%s\", type:%s, val: ",iv->infotype,type);
          switch(iv->datatype)
            {
            case 'i' : fprintf(f, "%d\n", iv->VAL.ivalue); break;
            case 'd' : fprintf(f, "%g\n", iv->VAL.dvalue); break;
            case 's' : fprintf(f, "\"%s\"\n", iv->VAL.string==NULL?"NULL":iv->VAL.string); break;
            case 'p' : 
              {
                // STB SPEC structure
                if (strcasecmp(iv->pointertype,INF_SPECIN)==0)
                  {
                    inf_stb_p_s *isps;
                    inf_stb_p_s_stab *stab;
                    isps=(inf_stb_p_s *)iv->VAL.pointer;
                    fprintf(f, "\n");
                    while (isps!=NULL)
                      {
                        fprintf(f, "        %s from \"%s\" %s\n", __getedge(isps->DATAEDGE)/*,isps->SPECTYPE*/, isps->CKNAME!=NULL?isps->CKNAME:"null", __getedge(isps->CKEDGE));
                        for (stab=isps->STABILITY; stab!=NULL; stab=stab->NEXT)
                          {
                            if (stab->CKNAME==NULL)
                              {
                                chain_list *cl;
                                fprintf(f,"          %s ", __getstab(stab->STABILITY));
                                for (cl=stab->TIME_LIST; cl!=NULL; cl=cl->NEXT)
                                  fprintf(f," %g", *(double *)cl->DATA);
                                fprintf(f,"\n");
                              }
                            else
                              {
                                fprintf(f, "          %s %g %s \"%s\" %s\n", __getstab(stab->STABILITY), stab->DELAY, __getrel(stab->RELATIVITY), stab->CKNAME!=NULL?stab->CKNAME:"null", __getedge(stab->CKEDGE));
                              }
                          }
                        isps=isps->NEXT;
                      }

                  }
                // INF association list
                else if (strcasecmp(iv->pointertype,INF_ASSOCLIST)==0)
                  {
                    chain_list *cl;
                    inf_assoc *assoc;
                    fprintf(f, "\n");
                    for (cl=(chain_list *)iv->VAL.pointer; cl!=NULL; cl=cl->NEXT)
                      {
                        assoc=(inf_assoc *)cl->DATA;
                        fprintf(f, "        \"%s\" \"%s\" lval:%ld dval:%g\n", assoc->orig!=NULL?assoc->orig:"NULL", assoc->dest!=NULL?assoc->dest:"NULL", assoc->lval, assoc->dval);
                      }
                  }
                else if (strcasecmp(iv->pointertype,INF_LISTOFLIST)==0)
                  {
                    chain_list *cl, *ch;
                    fprintf(f, "\n");
                    for (cl=(chain_list *)iv->VAL.pointer; cl!=NULL; cl=cl->NEXT)
                      {
                        fprintf(f, "        {");
                        for (ch=(chain_list *)cl->DATA; ch!=NULL; ch=ch->NEXT)
                          fprintf(f, " \"%s\"", (char *)ch->DATA);
                        fprintf(f, " }\n");
                      }
                  }
                // INF misc data
                else if (strcasecmp(iv->pointertype,INF_MISCDATA)==0)
                  {
                    inf_miscdata *assoc;
                    fprintf(f, "\n");
                    assoc=(inf_miscdata *)iv->VAL.pointer;
                    fprintf(f, "        \"%s\" \"%s\" lval:%ld dval:%g dval1:%g\n", assoc->orig!=NULL?assoc->orig:"NULL", assoc->dest!=NULL?assoc->dest:"NULL", assoc->lval, assoc->dval, assoc->dval1);
                  }
                else
                  fprintf(f, "%p (%s)\n", iv->VAL.pointer, iv->pointertype);
                break;
              }
            default : fprintf(f, "?\n");
            }
        }
    }  
  freechain(all);

}

void inf_AddAssociation(inffig_list *ifl, int location, char *key, char *type, char *val0, char *val1, long lval, double dval, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  chain_list *old;
  inf_assoc *assoc;

  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
      if (infgetkey(&ifl->DEFAULT, key, type)!=NULL)
        avt_errmsg(INF_ERRMSG, "016", AVT_WARNING, where==NULL?"":where, type, key);
        // avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set elsewhere -- overriding with inf values\n",where==NULL?"":where, type, key);
    }

  assoc=inf_createassoc();
  assoc->orig=val0;
  assoc->dest=val1;
  assoc->lval=lval;
  assoc->dval=dval;
  iv=infgetorcreatekey(is, key, type, 'p');
  iv->pointertype=sensitive_namealloc(INF_ASSOCLIST);
  old=iv->VAL.pointer;
  old=append(old, addchain(NULL, assoc));
  iv->VAL.pointer=old;
  ifl->changed|=1;
}

void inf_AddList(inffig_list *ifl, int location, char *key, char *type, chain_list *val, char *where)
{
  InfValue *iv;
  INF_INFOSET *is;
  chain_list *old;

  if (ifl==NULL) return;
  type=sensitive_namealloc(type);
  if (location==INF_DEFAULT_LOCATION)
    is=&ifl->DEFAULT;
  else
    {
      is=&ifl->LOADED;
      if (infgetkey(&ifl->DEFAULT, key, type)!=NULL)
        avt_errmsg(INF_ERRMSG, "016", AVT_WARNING, where==NULL?"":where, type, key);
        // avt_error("inf", 3, AVT_WAR, "%sinformation '%s' for '%s' has already been set elsewhere -- overriding with inf values\n",where==NULL?"":where, type, key);
    }

  iv=infgetorcreatekey(is, key, type, 'p');
  iv->pointertype=sensitive_namealloc(INF_LISTOFLIST);
  old=iv->VAL.pointer;
  old=append(old, addchain(NULL, val));
  iv->VAL.pointer=old;
  ifl->changed|=1;
}

static list_list *inf_addsimplell(list_list *ll, inffig_list *ifl, char *infotype, char *val, long lltype)
{
  chain_list *lst, *cl;
  lst=inf_GetEntriesByType(ifl, infotype, val);
  for (cl=lst; cl!=NULL; cl=cl->NEXT)
  {
    ll=infaddll(ll, (char *)cl->DATA, lltype, NULL);
  }
  freechain(lst);
  return ll;
}


list_list *inf_create_INFSIGLIST(inffig_list *ifl)     
{
  list_list *tl, *ll;
  chain_list *lst, *cl;
  int val;
  long type;

  if (ifl==NULL) return NULL;

  ll=NULL;
  for (tl=ifl->LOADED.INF_SIGLIST; tl!=NULL; tl=tl->NEXT)
  {
    ll=infaddll(ll, tl->DATA, tl->TYPE, tl->USER);
  }

  lst=inf_GetEntriesByType(ifl, INF_STUCK, INF_ANY_VALUES);
  for (cl=lst; cl!=NULL; cl=cl->NEXT)
  {
    inf_GetInt(ifl, (char *)cl->DATA, INF_STUCK, &val);
    if (val==0) type=INF_LL_STUCKVSS;
    else type=INF_LL_STUCKVDD;
    ll=infaddll(ll, inf_reVectName((char *)cl->DATA), type, NULL);
  }
  freechain(lst);

  ll=inf_addsimplell(ll, ifl, INF_PRECHARGE, INF_YES, INF_LL_PRECHARGE);
  ll=inf_addsimplell(ll, ifl, INF_PRECHARGE, INF_NO, INF_LL_NOTPRECHARGE);

  ll=inf_addsimplell(ll, ifl, INF_ASYNCHRON, INF_ANY_VALUES, INF_LL_ASYNCHRON);

  ll=inf_addsimplell(ll, ifl, INF_BYPASS, INF_ALL, INF_LL_BYPASS);
  ll=inf_addsimplell(ll, ifl, INF_BYPASS, INF_IN, INF_LL_BYPASSIN);
  ll=inf_addsimplell(ll, ifl, INF_BYPASS, INF_OUT, INF_LL_BYPASSOUT);
  ll=inf_addsimplell(ll, ifl, INF_BYPASS, INF_ONLYEND, INF_LL_ONLYEND);

  ll=inf_addsimplell(ll, ifl, INF_INTER, INF_ANY_VALUES, INF_LL_INTER);

  ll=inf_addsimplell(ll, ifl, INF_BREAK, INF_ANY_VALUES, INF_LL_BREAK);
  ll=inf_addsimplell(ll, ifl, INF_RC, INF_YES, INF_LL_RC);
  ll=inf_addsimplell(ll, ifl, INF_RC, INF_NO, INF_LL_NORC);
  ll=inf_addsimplell(ll, ifl, INF_NORISING, INF_ANY_VALUES, INF_LL_NORISING);
  ll=inf_addsimplell(ll, ifl, INF_NOFALLING, INF_ANY_VALUES, INF_LL_NOFALLING);
  ll=inf_addsimplell(ll, ifl, INF_FLIPFLOP, INF_ANY_VALUES, INF_LL_FLIPFLOP);
  return ll;
}

int inf_code_marksig(char *string)
{
    int result = 0;
    char *c, *tok;
    char buf[1024];

    strcpy(buf, string);
    
    tok = strtok_r(buf, " ,+", &c);
    while (tok!=NULL) {
        if (!strcasecmp("Latch",tok) || !strcasecmp("net_Latch",tok)) result |= INF_NET_LATCH;
        else if (!strcasecmp("MemSym",tok) || !strcasecmp("net_MemSym",tok)) result |= INF_NET_MEMSYM;
        else if (!strcasecmp("RS",tok) || !strcasecmp("net_RS",tok)) result |= INF_NET_RS;
        else if (!strcasecmp("FlipFlop",tok) || !strcasecmp("net_FlipFlop",tok)) result |= INF_NET_FLIPFLOP;
        else if (!strcasecmp("Master",tok) || !strcasecmp("net_Master",tok)) result |= INF_NET_MASTER;
        else if (!strcasecmp("Slave",tok) || !strcasecmp("net_Slave",tok)) result |= INF_NET_SLAVE;
        else if (!strcasecmp("Blocker",tok) || !strcasecmp("net_Blocker",tok)) result |= INF_NET_BLOCKER;
        else if (!strcasecmp("NoFalseBranch",tok) || !strcasecmp("net_NoFalseBranch",tok)) result |= INF_NET_NOFALSEBRANCH;
        else if (!strcasecmp("Vdd",tok) || !strcasecmp("net_Vdd",tok)) result |= INF_NET_VDD;
        else if (!strcasecmp("Vss",tok) || !strcasecmp("net_Vss",tok)) result |= INF_NET_VSS;
        else avt_errmsg(INF_ERRMSG, "027", AVT_ERROR, tok);
        tok = strtok_r(NULL, " ,+", &c);
    }
    return result;
}

int inf_code_markRS(char *string)
{
    if (!strcasecmp(string,"illegal")) return INF_RS_ILLEGAL;
    if (!strcasecmp(string,"legal")) return INF_RS_LEGAL;
    if (!strcasecmp(string,"mark_only")) return INF_RS_MARKONLY;
    avt_errmsg(INF_ERRMSG, "027", AVT_ERROR, string);
    return 0;
}

int inf_code_marktrans(char *string)
{
    int result = 0;
    char *c, *tok;
    char buf[1024];

    strcpy(buf, string);
    
    tok = strtok_r(buf, " ,+", &c);
    while (tok!=NULL) {
        if (!strcasecmp("Bleeder",tok) || !strcasecmp("trans_Bleeder",tok)) result |= INF_TRANS_BLEEDER;
        else if (!strcasecmp("Feedback",tok) || !strcasecmp("trans_Feedback",tok)) result |= INF_TRANS_FEEDBACK;
        else if (!strcasecmp("Command",tok) || !strcasecmp("trans_Command",tok)) result |= INF_TRANS_COMMAND;
        else if (!strcasecmp("NonFunctional",tok) || !strcasecmp("trans_NonFunctional",tok)) result |= INF_TRANS_NOT_FUNCTIONAL;
        else if (!strcasecmp("Blocker",tok) || !strcasecmp("trans_Blocker",tok)) result |= INF_TRANS_BLOCKER;
        else if (!strcasecmp("Short",tok) || !strcasecmp("trans_Short",tok)) result |= INF_TRANS_SHORT;
        else if (!strcasecmp("Unused",tok) || !strcasecmp("trans_Unused",tok)) result |= INF_TRANS_UNUSED;
        else avt_errmsg(INF_ERRMSG, "027", AVT_ERROR, tok);
        tok = strtok_r(NULL, " ,+", &c);
    }
    return result;
}

void inf_buildmatchrule (inffig_list * ifl, char *section, mbk_match_rules * mr, int canbenonnameallocated)
{
    chain_list *cl;
    mbk_CreateREGEX (mr, CASE_SENSITIVE, canbenonnameallocated);
    if (ifl == NULL)
        return;
    if (inf_GetPointer (ifl, section, "", (void **)&cl)) {
        while (cl != NULL) {
            mbk_AddREGEX (mr, ((inf_assoc *) cl->DATA)->orig);
            cl = cl->NEXT;
        }
    } 
    else 
    {
      cl=inf_GetEntriesByType(ifl, section, INF_ANY_VALUES);
      while (cl != NULL) {
         mbk_AddREGEX (mr, (char *)cl->DATA);
         cl = cl->NEXT;
      }
    }
}




