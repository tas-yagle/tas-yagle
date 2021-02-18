/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTV Verison 1                                               */
/*    Fichier : ttv_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1995-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* fontion d'utilite general                                                */
/****************************************************************************/
#include "ttv.h"
#include "ttv_directives.h"

chain_list *TTV_NAME_IN = NULL ;
chain_list *TTV_NAME_OUT = NULL ;
char TTV_LANG = TTV_LANG_E ;

struct 
{
  char *gatename;
  char *sign;
  int nbinput;
} ttv_mapping_table[] = 
  {
    { "inv", "u-1_d+1", 1},
    { "nd2", "u-2_u-1_d+1+2", 2},
    { "nd3", "u-3_u-2_u-1_d+1+2+3", 3},
    { "nd4", "u-4_u-3_u-2_u-1_d+1+2+3+4", 4},
    { "nd5", "u-5_u-4_u-3_u-2_u-1_d+1+2+3+4+5", 5},
    { "nd6", "u-6_u-5_u-4_u-3_u-2_u-1_d+1+2+3+4+5+6", 6},
    { "nd7", "u-7_u-6_u-5_u-4_u-3_u-2_u-1_d+1+2+3+4+5+6+7", 7},
    { "nd8", "u-8_u-7_u-6_u-5_u-4_u-3_u-2_u-1_d+1+2+3+4+5+6+7+8", 8},
    { "nr2", "u-2-1_d+1_d+2", 2},
    { "nr3", "u-3-2-1_d+1_d+2_d+3", 3},
    { "nr4", "u-4-3-2-1_d+1_d+2_d+3_d+4", 4},
    { "nr5", "u-5-4-3-2-1_d+1_d+2_d+3_d+4_d+5", 5},
    { "nr6", "u-6-5-4-3-2-1_d+1_d+2_d+3_d+4_d+5_d+6", 6},
    { "nr7", "u-7-6-5-4-3-2-1_d+1_d+2_d+3_d+4_d+5_d+6_d+7", 7},
    { "nr8", "u-8-7-6-5-4-3-2-1_d+1_d+2_d+3_d+4_d+5_d+6_d+7_d+8", 8},
    { "oai21", "u-3-1_u-2_d+1+2_d+2+3", 3}, // ao22
    { "ao22", "u-4-1_u-3-2_d+1+2_d+3+4", 4}, // nmx2
    { "ao22", "u-4-3_u-2-1_d+1+4_d+2+3", 4}, // nmx2
    { "ao22", "u-4-3_u-4-2_u-3-1_u-2-1_d+1+4_d+2+3", 4},       //xr2 
  };

int ttv_getgateinfo(int num, char **name, int *nbinput)
{
  num--;
  if (num>=0 && num<(signed)(sizeof(ttv_mapping_table)/sizeof(*ttv_mapping_table)))
    {
      *name=ttv_mapping_table[num].gatename;
      *nbinput=ttv_mapping_table[num].nbinput;
      return 1;
    }
  else
  {
    switch(num+1)
    {
      case TTV_SIG_GATE_MEMSYM:
        *name="MemSym", *nbinput=0; return 1;
      case TTV_SIG_GATE_TRISTATE:
        *name="<Tg>", *nbinput=0; return 1;
      case TTV_SIG_GATE_CONFLICT:
        *name="<Cg>", *nbinput=0; return 1;
      case TTV_SIG_GATE_TRISTATE_CONFLICT:
        *name="<Tg|Cg>", *nbinput=0; return 1;
      case TTV_SIG_GATE_DUAL_CMOS:
        *name="<Dg>", *nbinput=0; return 1;
      case TTV_SIG_GATE_NAND_RS:
        *name="RS/nd2", *nbinput=2; return 1;
      case TTV_SIG_GATE_NOR_RS:
        *name="RS/nr2", *nbinput=2; return 1;
      case TTV_SIG_GATE_RS:
        *name="RS", *nbinput=0; return 1;
      case TTV_SIG_GATE_LATCH:
        *name="Latch", *nbinput=0; return 1;
      case TTV_SIG_GATE_FLIPFLOP:
        *name="FlipFlop", *nbinput=0; return 1;
    }
  }
  return 0;
}

int ttv_findgateinfo(char *sign)
{
  int i;
  for (i=0;i<(signed)(sizeof(ttv_mapping_table)/sizeof(*ttv_mapping_table))
           && mbk_casestrcmp(ttv_mapping_table[i].sign, sign)!=0; i++);

  if (i>=(signed)(sizeof(ttv_mapping_table)/sizeof(*ttv_mapping_table))) return 0;
  return i+1;
}

/*****************************************************************************/
/*                        function ttv_getstmmodelname                       */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/* node : evenement source                                                   */
/* node : evenement destination                                              */
/* type : type de modele                                                     */
/* n : numero de modele                                                      */
/*                                                                           */
/* donne un nom de model                                                     */
/*****************************************************************************/
char *ttv_getstmmodelname(ttvfig,line,type,minmax)
ttvfig_list *ttvfig ;
ttvline_list *line ;
int type ;
char minmax ;
{
 char buf[4096] ;
 ttvevent_list *node = line->NODE ;
 ttvevent_list *root = line->ROOT ;
 char *pt ;
 char *pti ;
 char *pto ;
 char frin ;
 char frout ;

 pt = buf ;
 pti = ttv_getsigname(ttvfig,pt,node->ROOT) ;
 pt += strlen(buf) ;
 *pt = '\0' ;
 pt++ ;
 pto = ttv_getsigname(ttvfig,pt,root->ROOT) ;
   
 if((node->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
  frin = 'U' ;
 else
  frin = 'D' ;
 ttv_getsigname(ttvfig,pt,root->ROOT) ;

 if((root->TYPE & TTV_NODE_UP) == TTV_NODE_UP)
  frout = 'U' ;
 else
  frout = 'D' ;

 if((line->TYPE & TTV_LINE_U) == TTV_LINE_U)
    type |= STM_SETUP ;
 else if((line->TYPE & TTV_LINE_O) == TTV_LINE_O)
    type |= STM_HOLD ;
 else if((line->TYPE & TTV_LINE_A) == TTV_LINE_A)
    type |= STM_ACCESS ;

 pt = stm_generate_name (ttvfig->INFO->FIGNAME,pti,frin,pto,frout,type,minmax);
 
 return(pt) ;
}

/*****************************************************************************/
/*                        function ttv_revect()                              */
/* parametres :                                                              */
/* name : nom a vectoriser                                                   */
/*                                                                           */
/* revectorise le nom passe en parametre s'il contient des blancs ils seront */
/* remplaces par des '[]'.                                                   */
/* renvoie :                                                                 */
/* le pointeur sur le nom vectoriser                                         */
/*****************************************************************************/
char *ttv_revect(name)
char *name ;
{
 char  namex[512] ;
 short i = strlen(name) ;
 short j = i - 1 ;
 char vect = 'N' ;

 strcpy(namex,name) ;

 do
   {
    while((--i != -1) && (namex[i] != ' '))
      if(namex[i] == SEPAR)
        j = i-1 ;

    if(i != -1)
       {
        short k ;

        vect = 'Y' ;
        namex[i] = '[' ;

        for(k = strlen(namex) ; k != j ; k--)
        namex[k+1] = namex[k] ;

        namex[j+1] = ']' ;
        j = i - 1 ;
       }
    }
 while(i != -1) ;

 if (vect == 'N')
  return(name) ;
 else
  return(namealloc(namex)) ;
}

/*****************************************************************************/
/*                        function ttv_devect()                              */
/* parametres :                                                              */
/* name : nom a devectoriser                                                 */
/*                                                                           */
/* devectorise un nom                                                        */
/* renvoie :                                                                 */
/* le pointeur sur le nom devectoriser                                       */
/*****************************************************************************/
char *ttv_devect(name)
char *name ;
{
 char namex[512] ;
 short i = strlen(name) ;
 char vect = 'N' ;

 strcpy(namex,name) ;

 while(i-- != 0)
    {
     short j ;

     if(namex[i] == ']')
       {
        vect = 'Y' ;
        j = i ;
        while((--i != 0) && (namex[i] != '[') &&
              ((isdigit((int)namex[i]) != 0) || (namex[i] == '*'))) ;

        if(namex[i] == '[')
           {
            while(namex[j] != '\0')
            namex[j] = namex[++j] ;

            namex[i] = ' ' ;
           }
       }
    }

 if (vect == 'N')
  return(name) ;
 else
  return(namealloc(namex)) ;
}

long ttv_testsigflag(ttvsig_list *tvs, long mask)
{
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SIG_FLAGS))!=NULL)
    return ((long)pt->DATA) & mask;
  return 0;
}

void ttv_setsigflag(ttvsig_list *tvs, long mask)
{
  ptype_list *pt;
  if (mask!=0)
  {
    if ((pt=getptype(tvs->USER, TTV_SIG_FLAGS))==NULL)
      pt=tvs->USER=addptype(tvs->USER, TTV_SIG_FLAGS, 0);  
    pt->DATA=(void *)(((long)pt->DATA) | mask);
  }
}

void ttv_resetsigflag(ttvsig_list *tvs, long mask)
{
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SIG_FLAGS))==NULL) return;
  pt->DATA=(void *)(((long)pt->DATA) & ~mask);
  if (pt->DATA==(void *)0) tvs->USER=delptype(tvs->USER, TTV_SIG_FLAGS);
}
void ttv_setsigflaggate(ttvsig_list *tvs, char num)
{
  long newnum=num<<24;
  ttv_resetsigflag(tvs, 0xff000000);
  ttv_setsigflag(tvs, newnum);
}

/*****************************************************************************/
/*                        function ttv_testmask()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ptsig : signal comparer                                                   */
/* mask : liste de mask                                                      */
/*                                                                           */
/* renvoie :                                                                 */
/* 1 si le nom signal correspond a un des masques                            */
/* 0 sinon                                                                   */
/*****************************************************************************/
int ttv_testmask(ttvfig,ptsig,mask)
ttvfig_list *ttvfig ;
ttvsig_list *ptsig ;
chain_list *mask ;
{
 chain_list *chainx ;
 char bufname[1024], *name ;

 if(mask == NULL) return(0) ;
 if(strcmp((char*)mask->DATA,"*") == 0) return(1) ;  

 name=ttv_getsigname_sub(ttvfig,bufname,ptsig,1) ;

 for(chainx = mask ; chainx != NULL ; chainx = chainx->NEXT)
  {
   if(mbk_TestREGEX(name,(char*)chainx->DATA)) return(1) ;
  }

 return(0) ;
}
/*****************************************************************************/
/*                        function ttv_testnetnamemask()                            */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* ptsig : signal comparer                                                   */
/* mask : liste de mask                                                      */
/*                                                                           */
/* renvoie :                                                                 */
/* 1 si le nom signal correspond a un des masques                            */
/* 0 sinon                                                                   */
/*****************************************************************************/
int ttv_testnetnamemask(ttvfig,ptsig,mask)
ttvfig_list *ttvfig ;
ttvsig_list *ptsig ;
chain_list *mask ;
{
 chain_list *chainx ;
 char bufname[1024], *name ;

 if(mask == NULL) return(0) ;
 if(strcmp((char*)mask->DATA,"*") == 0) return(1) ;  

 name=ttv_getnetname_sub(ttvfig,bufname,ptsig,1) ;

 for(chainx = mask ; chainx != NULL ; chainx = chainx->NEXT)
  {
   if(mbk_TestREGEX(name,(char*)chainx->DATA)) return(1) ;
  }

 name=ttv_getsigname_sub(ttvfig,bufname,ptsig,1) ;

 for(chainx = mask ; chainx != NULL ; chainx = chainx->NEXT)
  {
   if(mbk_TestREGEX(name,(char*)chainx->DATA)) return(1) ;
  }

 return(0) ;
}

/*****************************************************************************/
/*                        function ttv_setttvlevel()                         */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/*                                                                           */
/* fixe le niveau hierarchique d'une ttvfig                                  */
/*****************************************************************************/
void ttv_setttvlevel(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chain ;
 long level = 0 ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   if(((ttvfig_list*)chain->DATA)->INFO->LEVEL > level) 
       level = ((ttvfig_list*)chain->DATA)->INFO->LEVEL ;
 ttvfig->INFO->LEVEL = level + (long)1 ;
}

/*****************************************************************************/
/*                        function ttv_setsigttvfiglevel()                   */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/*                                                                           */
/* fixe le niveau hierarchique d'une ttvfig                                  */
/*****************************************************************************/
void ttv_setsigttvfiglevel(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chain ;
 chain_list *chainsig ;

 chainsig = ttv_getsigbytype(ttvfig,ttvfig,TTV_SIG_C|TTV_SIG_N|TTV_SIG_L|
                                           TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B|
                                           TTV_SIG_I|TTV_SIG_S,NULL) ;

 for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
  {
   ttv_setsiglevel((ttvsig_list *)chain->DATA,ttvfig->INFO->LEVEL) ;
  }

 freechain(chainsig) ;

}


/*****************************************************************************/
/*                        function ttv_setttvdate()                          */
/* parametres :                                                              */
/* ttvfig : ttvfig courante                                                  */
/* type : type d'heure local ou gmt                                          */
/*                                                                           */
/* fixe la date et l'heure de la ttvfig dans le fichier pour comparer les    */
/* versions des fichiers                                                     */
/*****************************************************************************/
void ttv_setttvdate(ttvfig,type)
ttvfig_list *ttvfig ;
long type ;
{
 struct tm *tm ;
 time_t datesys ;
 int yearsys ;

 datesys = time(&datesys) ;

  if(type == TTV_DATE_LOCAL)
   tm = localtime(&datesys) ;
 else if(type == TTV_DATE_GMT)
   tm = gmtime(&datesys) ;

 yearsys = tm->tm_year ;

 if(((yearsys >= (long)95) && (yearsys <= (long)99)) || 
    ((yearsys >= (long)100) && (yearsys < (long)200)))
  yearsys = yearsys + (long)1900 ;
 else if((yearsys >= (long)0) && (yearsys < (long)95))
  yearsys = yearsys + (long)2000 ;

 ttvfig->INFO->TTVYEAR = yearsys ;
 ttvfig->INFO->TTVMONTH = tm->tm_mon +1 ;
 ttvfig->INFO->TTVDAY = tm->tm_mday ;
 ttvfig->INFO->TTVHOUR = tm->tm_hour ;
 ttvfig->INFO->TTVMIN = tm->tm_min ;
 ttvfig->INFO->TTVSEC = tm->tm_sec ;
}

/*****************************************************************************/
/*                        function ttv_checkfigname()                        */
/* parametres :                                                              */
/* name : nom de la ttvfig                                                   */
/*                                                                           */
/* verifie le nom d'une ttvfig                                               */
/*****************************************************************************/
char *ttv_checkfigname(name)
char *name ;
{
 char *pt ;
 char buf[1024] ;

 if(name == NULL)
   return(NULL) ;

 if(SEPAR == '_')
   return(name);

 if((pt = strchr(name,(int)SEPAR)) == NULL)
  return(namealloc(name)) ;

 strcpy(buf,name) ;
 pt = buf ;

 while((pt = strchr(pt,(int)SEPAR)) != NULL)
  {
   *pt = '_' ;
  }

/*
 ttv_error(52,name,TTV_WARNING) ;
*/

 return(namealloc(buf)) ;
}

/*****************************************************************************/
/*                        function ttv_checkdate()                           */
/* parametres :                                                              */
/* info1 : info premiere ttvfig                                              */
/* info2 : info deuxieme ttvfig                                              */
/*                                                                           */
/* compare deux ttvfig et indique la plus recente                            */
/* renvoie :                                                                 */
/* -1 si ttvfig1 est moins recente que ttvfig2                               */ 
/*  1 si ttvfig1 est plus recente que ttvfig2                                */ 
/*  0 sinon                                                                  */ 
/*****************************************************************************/
int ttv_checkdate(info1,info2)
ttvinfo_list *info1 ;
ttvinfo_list *info2 ;
{
 if(info1->TTVYEAR < info2->TTVYEAR) return(-1) ;
 if(info1->TTVYEAR > info2->TTVYEAR) return(1) ;
 if(info1->TTVMONTH < info2->TTVMONTH) return(-1) ;
 if(info1->TTVMONTH > info2->TTVMONTH) return(1) ;
 if(info1->TTVDAY < info2->TTVDAY) return(-1) ;
 if(info1->TTVDAY > info2->TTVDAY) return(1) ;
 if(info1->TTVHOUR < info2->TTVHOUR) return(-1) ;
 if(info1->TTVHOUR > info2->TTVHOUR) return(1) ;
 if(info1->TTVMIN < info2->TTVMIN) return(-1) ;
 if(info1->TTVMIN > info2->TTVMIN) return(1) ;
 if(info1->TTVSEC < info2->TTVSEC) return(-1) ;
 if(info1->TTVSEC > info2->TTVSEC) return(1) ;
 return(0) ;
}

/*****************************************************************************/
/*                        function ttv_checktechno()                         */
/* parametres :                                                              */
/* info1 : info premiere ttvfig                                              */
/* info2 : info deuxieme ttvfig                                              */
/*                                                                           */
/* compare les technologies de deux ttvfig                                   */
/* renvoie :                                                                 */
/*  0 si la technologie et la version sont identiques.                       */ 
/*  1 si la version de la technologie est differente                         */ 
/*  2 si la technologie est differente                                       */ 
/*****************************************************************************/
int ttv_checktechno(info1,info2)
ttvinfo_list *info1 ;
ttvinfo_list *info2 ;
{
 if((info1->TECHNONAME != info2->TECHNONAME) && 
    (strcmp(info1->TECHNONAME,"unknown") != 0) && 
    (strcmp(info2->TECHNONAME,"unknown") != 0))
   return(2) ;
 if((info1->TECHNOVERSION != info2->TECHNOVERSION) &&
    (strcmp(info1->TECHNONAME,"unknown") != 0) && 
    (strcmp(info2->TECHNONAME,"unknown") != 0))
   return(1) ;
 return(0) ;
}

/*****************************************************************************/
/*                        function ttv_checktool()                           */
/* parametres :                                                              */
/* info1 : info premiere ttvfig                                              */
/* info2 : info deuxieme ttvfig                                              */
/*                                                                           */
/* compare les outils qui ont generes de deux ttvfig                         */
/* renvoie :                                                                 */
/*  0 si l'outil et la version sont identiques.                              */ 
/*  1 si la version de l'outil est differente                                */ 
/*  2 si l'outil est different                                               */ 
/*****************************************************************************/
int ttv_checktool(info1,info2)
ttvinfo_list *info1 ;
ttvinfo_list *info2 ;
{
 if(info1->TOOLNAME != info2->TOOLNAME)
   return(2) ;
 if(info1->TOOLVERSION != info2->TOOLVERSION)
   return(1) ;
 return(0) ;
}

/*****************************************************************************/
/*                        function ttv_checkfigins()                         */
/* parametres :                                                              */
/* ttvfigp : ttvfig pere                                                     */
/* ttvfigf : ttvfig fils                                                     */
/*                                                                           */
/* verifie que les informations entre un pere et un fils sont concordantes   */
/*****************************************************************************/
void ttv_checkfigins(ttvfigp,ttvfigf)
ttvfig_list *ttvfigp ;
ttvfig_list *ttvfigf ;
{
 char message[256] ;

 if(ttv_checkdate(ttvfigp->INFO,ttvfigf->INFO) < 0)
  {
   if(TTV_LANG == TTV_LANG_E)
     sprintf(message,"%s must be generated after %s",
             ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   else
     sprintf(message,"%s doit etre genere apres %s",
             ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   ttv_error(41,message,TTV_WARNING) ;
  }
 if(ttv_checktechno(ttvfigp->INFO,ttvfigf->INFO) == 1)
  {
   if(TTV_LANG == TTV_LANG_E)
     sprintf(message,"%s and %s",ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   else
     sprintf(message,"%s et %s",ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   ttv_error(42,message,TTV_WARNING) ;
  }
 if(ttv_checktechno(ttvfigp->INFO,ttvfigf->INFO) == 2)
  {
   if(TTV_LANG == TTV_LANG_E)
     sprintf(message,"%s and %s",ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   else
     sprintf(message,"%s et %s",ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   ttv_error(43,message,TTV_WARNING) ;
  }
 if(ttv_checktool(ttvfigp->INFO,ttvfigf->INFO) == 1)
  {
   if(TTV_LANG == TTV_LANG_E)
     sprintf(message,"%s and %s",ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   else
     sprintf(message,"%s et %s",ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   ttv_error(45,message,TTV_WARNING) ;
  }
 if(ttvfigp->INFO->LEVEL <= ttvfigf->INFO->LEVEL)
  {
   if(TTV_LANG == TTV_LANG_E)
     sprintf(message,"%s level must be upper than %s",
             ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   else
     sprintf(message,"%s doit etre superieur au niveau de %s",
                      ttvfigp->INFO->FIGNAME,ttvfigf->INFO->FIGNAME) ;
   ttv_error(46,message,TTV_ERROR) ;
  }
}

/*****************************************************************************/
/*                        function ttv_checkallinstool()                     */
/* parametres :                                                              */
/* ttvfig : ttvfig pere                                                      */
/*                                                                           */
/* verifie que les fils sont generes avec la meme version d'outil            */
/*****************************************************************************/
void ttv_checkallinstool(ttvfig)
ttvfig_list *ttvfig ;
{
 ttvfig_list *ttvfig1 ;
 ttvfig_list *ttvfig2 ;
 chain_list *chain ;
 chain_list *chainx ;
 char message[256] ;

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
  {
   ttvfig1 = (ttvfig_list *)chain->DATA ;
   for(chainx = ttvfig->INS ; chainx != NULL ; chainx = chainx->NEXT)
    {
     ttvfig2 = (ttvfig_list *)chainx->DATA ;
     if(ttvfig1 != ttvfig2)
      {
       if(ttv_checktool(ttvfig1->INFO,ttvfig2->INFO) == 1)
        {
         if(TTV_LANG == TTV_LANG_E)
           sprintf(message,"%s and %s",ttvfig1->INFO->FIGNAME,ttvfig2->INFO->FIGNAME) ;
         else
           sprintf(message,"%s et %s",ttvfig1->INFO->FIGNAME,ttvfig2->INFO->FIGNAME) ;
         ttv_error(45,message,TTV_WARNING) ;
        }
      }
    }
  }
   
}

/*****************************************************************************/
/*                        function ttv_setcachesize()                        */
/* parametres :                                                              */
/* sizes : taille du cache de chemin en MB                                   */
/* sizel : taille du cache de signal en MB                                   */
/*                                                                           */
/* fixe la taille des caches de signaux et de chemins                        */
/*****************************************************************************/
void ttv_setcachesize(sizes,sizel)
double sizes ;
double sizel ;
{
 long nblinebloc ;
 long nbsigbloc ;

 if(sizel <= (double)0.0) 
   nbsigbloc = LONG_MAX;
 else
   nbsigbloc = (long)(((double)sizes / ((double)sizeof(ttvsbloc_list))) *
                     (((double)1024.0) * ((double)1024.0))) ;
 
 if(sizes <= (double)0.0)
   nblinebloc = LONG_MAX;
 else
   nblinebloc = (long)(((double)sizel / ((double)sizeof(ttvlbloc_list))) *
                     (((double)1024.0) * ((double)1024.0))) ;

 if(nbsigbloc >= TTV_NUMB_SIG)
   {
    TTV_MAX_SIG = nbsigbloc ;
   }
 else
  {
   while(nbsigbloc < TTV_NUMB_SIG)
     {
      ttvsbloc_list *ptsbloc ;
     
      if(TTV_FREE_SBLOC != NULL)
        {
         ptsbloc = TTV_FREE_SBLOC ;
         TTV_FREE_SBLOC = TTV_FREE_SBLOC->NEXT ;
         mbkfree(ptsbloc) ;
         TTV_NUMB_SIG-- ;
        }
      else
       {
        if(ttv_getsbloclist(TTV_LIST_TTVFIG) == 0)
         {
          break ;
         }
       }
     }

   if(TTV_NUMB_SIG < nbsigbloc)
     TTV_MAX_SIG = nbsigbloc ;
   else
     TTV_MAX_SIG = TTV_NUMB_SIG ;
  }

 if(nblinebloc >= TTV_NUMB_LINE)
   {
    TTV_MAX_LINE = nblinebloc ;
   }
 else
  {
   while(nblinebloc < TTV_NUMB_LINE)
     {
      ttvlbloc_list *ptlbloc ;
    
      if(TTV_FREE_LBLOC != NULL)
        {
         ptlbloc = TTV_FREE_LBLOC ;
         TTV_FREE_LBLOC = TTV_FREE_LBLOC->NEXT ;
         mbkfree(ptlbloc) ;
         TTV_NUMB_LINE-- ;
        }
      else
       {
        if(ttv_getlbloclist(TTV_LIST_TTVFIG,0l) == 0)
         {
          break ;
         }
       }
     }
 
   if(TTV_NUMB_LINE < nblinebloc)
     TTV_MAX_LINE = nblinebloc ;
   else
     TTV_MAX_LINE = TTV_NUMB_LINE ;
  }
}

void ttv_disablecache(ttvfig_list *ttvfig)
{
  chain_list *chain;
  ttv_setcachesize(-1,-1);
  if (ttvfig!=NULL && getptype(ttvfig->USER, TTV_FIG_FULL_MEMORY)==NULL)
  {
    chain=ttv_levelise(ttvfig,ttvfig->INFO->LEVEL,ttv_getloadedfigtypes(ttvfig));
    freechain(chain);
    ttvfig->USER=addptype(ttvfig->USER, TTV_FIG_FULL_MEMORY, NULL);
  }
}

/*****************************************************************************/
/*                        function ttv_getsigcachesize()                     */
/* parametres :                                                              */
/*                                                                           */
/* renvoie la taille en mega octet du cache des signaux                      */
/*****************************************************************************/
double ttv_getsigcachesize(void)
{
 return(((((double)TTV_MAX_SIG * (double)sizeof(ttvsbloc_list))) /
       (((double)1024.0) * ((double)1024.0)))) ;
}

/*****************************************************************************/
/*                        function ttv_getlinecachesize()                    */
/* parametres :                                                              */
/*                                                                           */
/* renvoie la taille en mega octet du cache des lines                        */
/*****************************************************************************/
double ttv_getlinecachesize(void)
{
 return(((((double)TTV_MAX_LINE * (double)sizeof(ttvlbloc_list))) /
       (((double)1024.0) * ((double)1024.0)))) ;
}

/*****************************************************************************/
/*                        function ttv_getsigmemorysize()                     */
/* parametres :                                                              */
/*                                                                           */
/* renvoie la taille en mega octet de la memoire des signaux                 */
/*****************************************************************************/
double ttv_getsigmemorysize(void)
{
 return((((double)(TTV_NUMB_SIG * sizeof(ttvsbloc_list))) /
       (((double)1024.0) * ((double)1024.0)))) ;
}

/*****************************************************************************/
/*                        function ttv_getlinememorysize()                   */
/* parametres :                                                              */
/*                                                                           */
/* renvoie la taille en mega octet de la memoire des lines                   */
/*****************************************************************************/
double ttv_getlinememorysize(void)
{
 return((((double)(TTV_NUMB_LINE * sizeof(ttvlbloc_list))) /
       (((double)1024.0) * ((double)1024.0)))) ;
}

/*****************************************************************************/
/*                        function ttv_getclocklist()                        */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* renvoie la list des clocks de la ttvfig                                   */
/*****************************************************************************/
chain_list *ttv_getclocklist(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chainsig = NULL ;
 ptype_list *list ;

 if((list = getptype(ttvfig->USER,TTV_FIG_INFLIST)) != NULL)
    list = (ptype_list *)list->DATA ;
 else
    list = NULL ;

 for(; list != NULL ; list = list->NEXT)
  {
   if(list->TYPE == INF_LL_CLOCK)
     chainsig = addchain(chainsig,((ptype_list*)list->DATA)->DATA) ;
  }

 return(chainsig) ;
}

chain_list *ttv_getasyncsiglist(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chainsig = NULL, *cl ;
 ptype_list *list ;

 cl = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C,NULL) ;
 
 while (cl!=NULL)
 {
   if (getptype(((ttvsig_list *)cl->DATA)->USER, TTV_SIG_ASYNCHRON)!=NULL)
     chainsig=addchain(chainsig, cl->DATA);
   cl=delchain(cl,cl);
 }
 return(chainsig) ;
}

/*****************************************************************************/
/*                        function ttv_getclocksiglist()                     */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* renvoie la list des sig clocks de la ttvfig                               */
/*****************************************************************************/
chain_list *ttv_getclocksiglist(ttvfig)
ttvfig_list *ttvfig ;
{
 chain_list *chain = NULL ;
 ptype_list *list ;

 if (ttvfig!=NULL) {
   if((list = getptype(ttvfig->USER,TTV_FIG_INFLIST)) != NULL)
      list = (ptype_list *)list->DATA ;
   else
      list = NULL ;

   for(; list != NULL ; list = list->NEXT)
    {
     if(list->TYPE == INF_LL_CLOCK)
      {
       chain = addchain(chain,(void *)((ptype_list*)list->DATA)->TYPE) ;
      }
    }
 }
 return chain;
}

/*****************************************************************************/
/*                        function ttv_markinf()                             */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* renvoie la taille en mega octet de la memoire des lines                   */
/*****************************************************************************/
ptype_list *ttv_markinf(ttvfig,hlist,ifl,mode)
ttvfig_list *ttvfig ;
ptype_list *hlist ;
inffig_list *ifl;
char mode ;
{
 chain_list *chainsig ;
 chain_list *chain ;
 chain_list *chainx, *cl, *multi ;
 chain_list *chainn = NULL ;
 ptype_list *list, *pti, *pto ;
 long typesig ;
 double rise, fall, c;
 ttvsig_list *tvs;
 char buf[1024];
 
 for(list = hlist ; list != NULL ; list = list->NEXT)
  {
   chain = addchain(NULL,((ptype_list*)list->DATA)->DATA) ;
   if(list->TYPE == INF_LL_BYPASSOUT)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|
                                 TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B,chain) ;
     typesig = TTV_SIG_BYPASSOUT ;
    }
   else if(list->TYPE == INF_LL_BYPASSIN)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|
                                 TTV_SIG_R|TTV_SIG_B,chain) ;
     typesig = TTV_SIG_BYPASSIN ;
    }
   else if(list->TYPE == INF_LL_BYPASS)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|
                                 TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B,chain) ;
     typesig = TTV_SIG_BYPASSOUT | TTV_SIG_BYPASSIN ;
    }
   else if(list->TYPE == INF_LL_NORISING)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|
                                 TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B,chain) ;
     typesig = TTV_NODE_BYPASSOUT | TTV_NODE_BYPASSIN ;
    }
   else if(list->TYPE == INF_LL_NOFALLING)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|
                                 TTV_SIG_R|TTV_SIG_Q|TTV_SIG_B,chain) ;
     typesig = TTV_NODE_BYPASSOUT | TTV_NODE_BYPASSIN ;
    }
   else if(list->TYPE == INF_LL_ONLYEND)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C,chain) ;
     if(chainn == NULL)
       chainn = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_N,NULL) ;
     typesig = TTV_NODE_ONLYEND ;
    }
   else if(list->TYPE == INF_LL_ASYNCHRON)
    {
     chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C,chain) ;
    }
   else
    {
     freechain(chain);
     continue ;
    }
   freechain(chain) ;
   for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
    {
     if(list->TYPE == INF_LL_NOFALLING)
      {
       if(mode == 'M')
         ((ttvsig_list *)chain->DATA)->NODE[0].TYPE |= typesig ;
       else
         ((ttvsig_list *)chain->DATA)->NODE[0].TYPE &= ~(typesig) ;
      }
     else if(list->TYPE == INF_LL_NORISING)
      {
       if(mode == 'M')
         ((ttvsig_list *)chain->DATA)->NODE[1].TYPE |= typesig ;
       else
         ((ttvsig_list *)chain->DATA)->NODE[1].TYPE &= ~(typesig) ;
      }
     else if(list->TYPE == INF_LL_ONLYEND)
      {
       if((((ttvsig_list *)chain->DATA)->TYPE & TTV_SIG_CT) == TTV_SIG_CT)
       {
        ((ttvsig_list *)chain->DATA)->NODE[0].TYPE |= typesig ;
        ((ttvsig_list *)chain->DATA)->NODE[1].TYPE |= typesig ;
        for(chainx = chainn ; chainx != NULL ; chainx = chainx->NEXT)
         {
          if(((ttvsig_list *)chain->DATA)->NETNAME == 
             ((ttvsig_list *)chainx->DATA)->NETNAME)
            {
             if(mode == 'M')
              {
               ((ttvsig_list *)chainx->DATA)->NODE[0].TYPE |= typesig ;
               ((ttvsig_list *)chainx->DATA)->NODE[1].TYPE |= typesig ;
              }
             else
              {
               ((ttvsig_list *)chainx->DATA)->NODE[0].TYPE &= ~(typesig) ;
               ((ttvsig_list *)chainx->DATA)->NODE[1].TYPE &= ~(typesig) ;
              }
            }
         }
       }
      }
     else if(list->TYPE == INF_LL_ASYNCHRON)
       {
         if(mode == 'M')
	   ((ttvsig_list *)chain->DATA)->USER = 
             addptype (((ttvsig_list *)chain->DATA)->USER, TTV_SIG_ASYNCHRON, 0) ;
         else if(getptype (((ttvsig_list *)chain->DATA)->USER, TTV_SIG_ASYNCHRON))
	   ((ttvsig_list *)chain->DATA)->USER = 
             delptype (((ttvsig_list *)chain->DATA)->USER, TTV_SIG_ASYNCHRON) ;
       }
     else
       {
        if(mode == 'M')
          ((ttvsig_list *)chain->DATA)->TYPE |= typesig ;
        else
          ((ttvsig_list *)chain->DATA)->TYPE &= ~(typesig) ;
       }
    }
   freechain(chainsig) ;
  }

 if (mode!='M')
 {
    // nettoyage
    chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_TYPEALL,NULL) ;
    for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
     {
       tvs=(ttvsig_list *)chain->DATA;
       // INF_PIN_RISING_SLEW
       ttv_delnodedelay(&tvs->NODE[0]);
       ttv_delnodedelay(&tvs->NODE[1]);
       
       // INF_CLOCK_TYPE
       if(getptype (tvs->USER, TTV_SIG_CLOCK))
         tvs->USER = delptype (tvs->USER, TTV_SIG_CLOCK) ;

       // INF_DONTCROSS
       ttv_resetsigflag(tvs, TTV_SIG_FLAGS_DONTCROSS);
       ttv_resetsigflag(tvs, TTV_SIG_FLAGS_TRANSPARENT);
       ttv_resetsigflag(tvs, TTV_SIG_FLAGS_ISDATA);

       tvs->NODE->TYPE&=~(TTV_NODE_FALSEIN|TTV_NODE_FALSEOUT|TTV_NODE_FALSEPATHNODE);
       (tvs->NODE+1)->TYPE&=~(TTV_NODE_FALSEIN|TTV_NODE_FALSEOUT|TTV_NODE_FALSEPATHNODE);

       // INF_MULTICYCLE_PATH
       if((pti=getptype (tvs->USER, TTV_SIG_MULTICYCLE_IN))!=NULL)
        {
          freechain((chain_list *)pti->DATA);
          tvs->USER = delptype (tvs->USER, TTV_SIG_MULTICYCLE_IN);
        }
       if((pto=getptype (tvs->USER, TTV_SIG_MULTICYCLE_OUT))!=NULL)
        {
          for (chainx=(chain_list *)pto->DATA; chainx!=NULL; chainx=chainx->NEXT)
            mbkfree(chainx->DATA);
          freechain((chain_list *)pto->DATA);
          tvs->USER = delptype (tvs->USER, TTV_SIG_MULTICYCLE_OUT) ;
        }         
       tvs->USER=testanddelptype(tvs->USER, TTV_SIG_OUTPUT_CAPACITANCE);
       tvs->USER=testanddelptype(tvs->USER, TTV_SIG_STRICT_SETUP);

       tvs->USER=testanddelptype(tvs->USER, TTV_SIG_DIRECTIVES);
       
     }
    freechain(chainsig) ;
    if ((pto=getptype(ttvfig->USER, TTV_FIG_DIRECTIVES_HEAP))!=NULL)
    {
      DeleteHeap((HeapAlloc *)pto->DATA);
      mbkfree(pto->DATA);
    }

    ttv_unmark_signals_corresponding_path_margin_regex(ttvfig);
 }
 else
   {
     HeapAlloc *ha;
     ha=(HeapAlloc *)mbkalloc(sizeof(HeapAlloc));
     CreateHeap(sizeof(ttv_directive), 4096, ha);

     ttvfig->USER=addptype(ttvfig->USER, TTV_FIG_DIRECTIVES_HEAP, ha);
     ttv_setdirectives(ttvfig, ifl, ha);

     ttv_mark_signals_with_corresponding_path_margin_regex(ttvfig);
             
     chainx=inf_GetEntriesByType(ifl, INF_OUTPUT_CAPACITANCE, INF_ANY_VALUES);
  
     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
      {
        ht *realcon=addht(10);
        chain=addchain(NULL, (char *)cl->DATA);
        chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_N,chain) ;
        freechain(chain);
        inf_GetDouble(ifl, (char *)cl->DATA, INF_OUTPUT_CAPACITANCE, &c);
        for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
        {
          tvs=(ttvsig_list *)chain->DATA;
          if ((tvs->TYPE & TTV_SIG_CO)==TTV_SIG_CO
              || (tvs->TYPE & TTV_SIG_CZ)==TTV_SIG_CZ
              || (tvs->TYPE & TTV_SIG_CB)==TTV_SIG_CB
              || (tvs->TYPE & TTV_SIG_CT)==TTV_SIG_CT)
            addhtitem(realcon, tvs->NETNAME, (long)tvs);
        }

        for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
          {
            tvs=(ttvsig_list *)chain->DATA;
            if (gethtitem(realcon, tvs->NETNAME)!=EMPTYHT && (pto=getptype(tvs->USER, TTV_SIG_OUTPUT_CAPACITANCE))==NULL)
            {
              pto=tvs->USER=addptype(tvs->USER, TTV_SIG_OUTPUT_CAPACITANCE, NULL);
              *(float *)&pto->DATA=(float)c;
            }
          }

        delht(realcon);
        freechain(chainsig) ;
      }
     freechain(chainx);

     chainx=inf_GetEntriesByType(ifl, INF_PIN_RISING_SLEW, INF_ANY_VALUES);
     chain=inf_GetEntriesByType(ifl, INF_PIN_FALLING_SLEW, INF_ANY_VALUES);
     chainx=inf_SortEntries(inf_mergeclist (chainx, chain));

     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
       {
         chain=addchain(NULL, (char *)cl->DATA);
         if(strcmp((char*)cl->DATA,"default") == 0)  
           chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C,NULL) ;
         else
           chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C,chain) ;
         freechain(chain);
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {
             tvs=(ttvsig_list *)chain->DATA;
             rise=fall=-1;
             inf_GetDouble(ifl, (char*)cl->DATA, INF_PIN_RISING_SLEW, &rise);
             if (rise>0) rise=mbk_long_round(rise*1e12*TTV_UNIT);
             inf_GetDouble(ifl, (char*)cl->DATA, INF_PIN_FALLING_SLEW, &fall);
             if (fall>0) fall=mbk_long_round(fall*1e12*TTV_UNIT);

             if(rise>0 && !ttv_getnodedelay(&tvs->NODE[1]))
               {
                 ttv_addnodedelay(&tvs->NODE[1],TTV_NOTIME,TTV_NOTIME, rise, rise);
               }
             if(fall>0 && !ttv_getnodedelay(&tvs->NODE[0]))
               {
                 ttv_addnodedelay(&tvs->NODE[0],TTV_NOTIME,TTV_NOTIME, fall, fall);
               }
           }
         freechain(chainsig) ;
       }
     freechain(chainx);

     chainx=inf_GetEntriesByType(ifl, INF_CLOCK_TYPE, INF_ANY_VALUES);
     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
       {
         chain=addchain(NULL, (char *)cl->DATA);
         chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B/*|TTV_SIG_Q*/,chain) ;
         freechain(chain);
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {
             tvs=(ttvsig_list *)chain->DATA;
             if (!getptype (tvs->USER, TTV_SIG_CLOCK))
             {
               tvs->USER = addptype (tvs->USER, TTV_SIG_CLOCK, NULL) ;
              hlist=addptype(hlist, INF_LL_CLOCK, addptype(NULL, (long)tvs, namealloc(ttv_getsigname(ttvfig, buf, tvs))));
             }
           }
         freechain(chainsig);
       }
     freechain(chainx);

     chainx=inf_GetEntriesByType(ifl, INF_DONTCROSS, INF_ANY_VALUES);
     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
       {
         chain=addchain(NULL, (char *)cl->DATA);
         chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_L|TTV_SIG_R,chain) ;
         freechain(chain);
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {
             tvs=(ttvsig_list *)chain->DATA;
             ttv_setsigflag(tvs, TTV_SIG_FLAGS_DONTCROSS);
           }
         freechain(chainsig);
       }
     freechain(chainx);

     chainx=inf_GetEntriesByType(ifl, INF_TRANSPARENT, INF_ANY_VALUES);
     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
       {
         chain=addchain(NULL, (char *)cl->DATA);
         chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_L|TTV_SIG_R,chain) ;
         freechain(chain);
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {
             tvs=(ttvsig_list *)chain->DATA;
             ttv_setsigflag(tvs, TTV_SIG_FLAGS_TRANSPARENT);
           }
         freechain(chainsig);
       }
     freechain(chainx);

     chainx=inf_GetEntriesByType(ifl, INF_STRICT_SETUP, INF_ANY_VALUES);
     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
       {
         chain=addchain(NULL, (char *)cl->DATA);
         chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_L,chain) ;
         freechain(chain);
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {
             tvs=(ttvsig_list *)chain->DATA;
             if (getptype(tvs->USER, TTV_SIG_STRICT_SETUP)==NULL)
               tvs->USER=addptype(tvs->USER, TTV_SIG_STRICT_SETUP, 0);
           }
         freechain(chainsig);
       }
     freechain(chainx);

     // ZINAPS: test temporaire, creer une section inf propre
     chainx=inf_GetEntriesByType(ifl, INF_CHARAC_DATA, INF_ANY_VALUES);
     if (chainx!=NULL)
     {
       // desabling ttv cache
       ttv_disablecache(ttvfig);
     }
     for (cl=chainx; cl!=NULL; cl=cl->NEXT)
       {
         chain=addchain(NULL, (char *)cl->DATA);
         chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_TYPEALL,chain) ;
         freechain(chain);
         for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
           {
             tvs=(ttvsig_list *)chain->DATA;
             ttv_setsigflag(tvs, TTV_SIG_FLAGS_ISDATA);
           }
         freechain(chainsig);
       }
     freechain(chainx);

     if (inf_GetPointer(ifl, INF_MULTICYCLE_PATH, "", (void **)&multi))
       {
         inf_assoc *assoc;
         chain_list *chainsigin;
         ttvsig_list *tvsi, *tvso;
         ptype_list *pti, *pto;
         int rulenum, anyi;
         ttv_MultiCycleInfo *tmi;
         multi=reverse(multi);
         for (cl=multi, rulenum=0; cl!=NULL; rulenum++, cl=cl->NEXT)
           {         
             assoc=(inf_assoc *)cl->DATA;
             if (strcmp((char *)assoc->orig, "*")==0) anyi=1;
             else anyi=0;

             if (!anyi)
               {
                 chain=addchain(NULL, (char *)assoc->orig);
                 chainsigin = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R,chain) ;
                 freechain(chain);
               }
             else 
               chainsigin=(chain_list *)1;
           
             if (anyi || chainsigin!=NULL)
               {
                 chain=addchain(NULL, (char *)assoc->dest);
                 chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C|TTV_SIG_L|TTV_SIG_B|TTV_SIG_R,chain) ;
                 chainsig=append(ttv_getmatchingdirectivenodes(ttvfig,chain, 1), chainsig);
                 freechain(chain);
             
                 if (chainsig!=NULL)
                   {

                     for (chain=chainsigin; chain!=NULL; chain=chain->NEXT)
                       {
                         if (!anyi)
                           {
                             tvsi=(ttvsig_list *)chain->DATA;
                         
                             if ((pti=getptype(tvsi->USER, TTV_SIG_MULTICYCLE_IN))==NULL)
                               pti=tvsi->USER = addptype (tvsi->USER, TTV_SIG_MULTICYCLE_IN, NULL) ;
                             pti->DATA=addchain((chain_list *)pti->DATA, (void *)(long)rulenum);
                           }

                         for (chainn=chainsig; chainn!=NULL; chainn=chainn->NEXT)
                           {
                             tvso=(ttvsig_list *)chainn->DATA;

                             if ((pto=getptype(tvso->USER, TTV_SIG_MULTICYCLE_OUT))==NULL)
                               pto=tvso->USER = addptype (tvso->USER, TTV_SIG_MULTICYCLE_OUT, NULL) ;
                             tmi=(ttv_MultiCycleInfo *)mbkalloc(sizeof(ttv_MultiCycleInfo));
                             
                             tmi->Flags=assoc->lval;
                             if (anyi) tmi->Flags|=TTV_MULTICYCLE_ANYINPUT;
                             tmi->RuleNumber=rulenum;
                             tmi->Multiplier=assoc->dval;
                             pto->DATA=addchain((chain_list *)pto->DATA, tmi);
                           }
                         if (anyi) break;
                       }
                     freechain(chainsig);
                   }
                 if (!anyi)
                   freechain(chainsigin);
               }
           }
         multi=reverse(multi);
       }
 }
 return hlist;
}

void ttv_post_traitment(ttvfig_list *ttvfig)
{
 chain_list *chainsig ;
 chain_list *chain ;
 chain_list *chainx, *cl, *done ;
 long typesig ;
 ttvsig_list *tvs;
 char *dir;
 inffig_list *ifl;
 
 ifl=getinffig(ttvfig->INFO->FIGNAME) ;
 
 done=NULL;
 chainx=inf_GetEntriesByType(ifl, INF_CONNECTOR_DIRECTION, INF_ANY_VALUES);
 for (cl=chainx; cl!=NULL; cl=cl->NEXT)
   {
     dir="";
     inf_GetString(ifl, (char *)cl->DATA, INF_CONNECTOR_DIRECTION, &dir);
     switch(dir[0])
     {
       case 'I' : typesig=TTV_SIG_CI; break;
       case 'O' : typesig=TTV_SIG_CO; break;
       case 'Z' : typesig=TTV_SIG_CZ; break;
       case 'B' : typesig=TTV_SIG_CB; break;
       case 'T' : typesig=TTV_SIG_CT; break;
       case 'X' : typesig=TTV_SIG_CX; break;
       default  : typesig=0; break;
     }
     if (typesig!=0)
     {
       chain=addchain(NULL, (char *)cl->DATA);
       chainsig = ttv_getsigbytype_and_netname(ttvfig,NULL,TTV_SIG_C,chain) ;
       freechain(chain);         
       for(chain = chainsig ; chain != NULL ; chain = chain->NEXT)
         {
           tvs=(ttvsig_list *)chain->DATA;
           if (!getptype(tvs->USER, TTV_SIG_DONE_PTYPE))
           {
             tvs->TYPE &= ~(TTV_SIG_CI|TTV_SIG_CO|TTV_SIG_CZ|TTV_SIG_CB|TTV_SIG_CT|TTV_SIG_CX);
             tvs->TYPE |= typesig;
             tvs->USER=addptype(tvs->USER, TTV_SIG_DONE_PTYPE, NULL);
             done=addchain(done, tvs);
           }
         }
       freechain(chainsig);
     }
   }
 while (done!=NULL)
 {
   tvs=(ttvsig_list *)done->DATA;
   tvs->USER=testanddelptype(tvs->USER, TTV_SIG_DONE_PTYPE);
   done=delchain(done, done);
 }
 freechain(chainx);
}      

/*****************************************************************************/
/*                        function ttv_getinffile()                          */
/* parametres :                                                              */
/* ttvfig : figure                                                           */
/*                                                                           */
/* renvoie la taille en mega octet de la memoire des lines                   */
/*****************************************************************************/
void ttv_getinffile(ttvfig)
ttvfig_list *ttvfig ;
{
 list_list *list, *ilist ;
 chain_list *chain ;
 ptype_list *ptype ;
 ptype_list *ptypeclock ;
 ptype_list *ptypelast ;
 ptype_list *ptypeinf = NULL ;
 ptype_list *ptypedatauser = NULL ;
 char *sigin ;
 char *sigout ;
 char *clock ;
 long slopein ;
 long slopeout ;
 long slope ;
 inffig_list *ifl;
 chain_list *maincl;
 ht *nametosig;
 NameAllocator NA;

 ifl=getinffig(ttvfig->INFO->FIGNAME) ;
 ttv_addaxis(ifl, ttvfig);

 if((ptypeinf = getptype(ttvfig->USER,TTV_FIG_INFLIST)) != NULL)
   {
    ptypeinf->DATA=ttv_markinf(ttvfig,(ptype_list *)ptypeinf->DATA,ifl,'D') ;
    for (ptype=(ptype_list *)ptypeinf->DATA; ptype!=NULL; ptype=ptype->NEXT)
      freeptype(ptype->DATA);
    freeptype((ptype_list *)ptypeinf->DATA) ;
    ttvfig->USER = delptype(ttvfig->USER,TTV_FIG_INFLIST) ;
    ptypeinf = NULL ;
   }

 ilist=inf_create_INFSIGLIST(ifl);
 for (list=ilist; list!=NULL; list=list->NEXT)
  {
   ptypedatauser = addptype(NULL, (long)list->USER, list->DATA);
   ptypeinf = addptype(ptypeinf,list->TYPE, ptypedatauser) ;
  }
 inffreell(ilist);

 ptypeinf=ttv_markinf(ttvfig,ptypeinf,ifl,'M') ;

 ttvfig->USER = addptype(ttvfig->USER,TTV_FIG_INFLIST,ptypeinf) ;

 ttv_freefalsepath(ttvfig);

 nametosig=ttv_buildquickaccessht_forfalsepath(ttvfig, &NA);

 maincl=infGetInfo(ifl, INF_FALSEPATH_INFO);
 if (maincl) ttv_disablecache(ttvfig);
 for(chain = maincl ; chain != NULL ; chain = chain->NEXT)
  {
   ptype = (ptype_list *)chain->DATA ;
   ptypeclock = NULL ;
   ptypelast = NULL ;
   if(ptype != NULL)
    {
     if(ptype->NEXT != NULL)
      {
       sigin = mbk_index_regex((char *)ptype->DATA) ;
       if(ptype->TYPE == INF_UP)
         slopein = TTV_NODE_UP ;
       else if(ptype->TYPE == INF_DOWN)
         slopein = TTV_NODE_DOWN ;
       else if(ptype->TYPE == INF_UPDOWN)
         slopein = TTV_NODE_DOWN | TTV_NODE_UP ;
       for(; ptype != NULL ; ptype = ptype->NEXT)
        {
         ptypelast = ptypeclock ;
         ptypeclock = ptype ;
        }
       if(ptypeclock->TYPE != INF_CK)
        {
         ptypelast = ptypeclock ;
         clock = NULL ;
        }
       else
        {
         clock = (char *)ptypeclock->DATA ;
        }
       sigout = mbk_index_regex((char *)ptypelast->DATA) ;
       if(ptypelast->TYPE == INF_UP)
         slopeout = TTV_NODE_UP ;
       else if(ptypelast->TYPE == INF_DOWN)
         slopeout = TTV_NODE_DOWN ;
       else if(ptypelast->TYPE == INF_UPDOWN)
         slopeout = TTV_NODE_DOWN | TTV_NODE_UP ;
       if(((ptype_list *)chain->DATA)->NEXT != ptypelast)
        {
         ptypeclock = NULL ;
         for(ptype = ((ptype_list *)chain->DATA)->NEXT ; ptype != ptypelast ;
             ptype = ptype->NEXT)
          {
           if(ptype->TYPE == INF_UP)
             slope = TTV_NODE_UP ;
           else if(ptype->TYPE == INF_DOWN)
             slope = TTV_NODE_DOWN ;
           else if(ptype->TYPE == INF_UPDOWN)
             slope = TTV_NODE_DOWN | TTV_NODE_UP ;
           else if(ptype->TYPE == INF_NOTHING)
             slope=0;
           ptypeclock = addptype(ptypeclock,slope,mbk_index_regex(ptype->DATA)) ;
          }
         ptypeclock = (ptype_list *)reverse((chain_list *)ptypeclock) ;
        }
       else
        { 
         ptypeclock = NULL ;
        }
       ttv_addfalsepath(ttvfig,sigin,slopein,sigout,slopeout,clock,ptypeclock, nametosig, &NA) ;
      }
    }
  }
 freechain(maincl);
 ttv_freeequickaccessht_forfalsepath(nametosig, &NA);
 DeleteNameAllocator(&NA);

 for(chain = ttvfig->INS ; chain != NULL ; chain = chain->NEXT)
   {
    ttv_getinffile((ttvfig_list *)chain->DATA) ;
   }
}


/*****************************************************************************/
/*                        function ttv_addaxis()                             */
/*****************************************************************************/
void ttv_addaxis(inffig_list *ifl, ttvfig_list *ttvfig)
{
  stm_carac     *stmcaracslope = NULL ;
  stm_carac     *stmcaraccapa = NULL ;
  float *s_axis;
  long  slope;
  int i,ns;
  chain_list *cl, *maincl, *list0, *ch;
  char *name;
  double rise, fall;
  ht *htslope_axis = NULL;
  ht *htcapa_axis  = NULL;
  ptype_list *ptype;
  
  ptype = getptype(ttvfig->USER, TTV_FIG_SAXIS);
  if(ptype){
      htslope_axis = (ht*)ptype->DATA;
      cl=GetAllHTElems(htslope_axis);
      while (cl!=NULL){
          mbkfree(((stm_carac*)cl->DATA)->VALUES);
          mbkfree(cl->DATA);
          cl = delchain(cl, cl);
      }
      delht(htslope_axis);
      ttvfig->USER=delptype(ttvfig->USER, TTV_FIG_SAXIS);
  }
  ptype = getptype(ttvfig->USER, TTV_FIG_CAXIS);
  if(ptype){
      htcapa_axis = (ht*)ptype->DATA;
      cl=GetAllHTElems(htcapa_axis);
      while (cl!=NULL){
          mbkfree(((stm_carac*)cl->DATA)->VALUES);
          mbkfree(cl->DATA);
          cl = delchain(cl, cl);
      }
      delht(htcapa_axis);
      ttvfig->USER=delptype(ttvfig->USER, TTV_FIG_CAXIS);
  }
  htslope_axis = NULL;
  htcapa_axis  = NULL;

  list0=inf_GetEntriesByType(ifl, INF_SLOPEIN, INF_ANY_VALUES);
  if (list0) htslope_axis = addht (1000) ;
  for (cl=list0; cl!=NULL; cl=cl->NEXT) 
    {
      name=(char *)cl->DATA;
      inf_GetPointer(ifl, name, INF_SLOPEIN, (void **)&ch);
      stmcaracslope = (stm_carac*)mbkalloc (sizeof (stm_carac));
      stmcaracslope->VALUES = (float*)mbkalloc (countchain(ch) * sizeof (float)) ;
      for (i = 0 ; ch!=NULL ; i++, ch=ch->NEXT)
        stmcaracslope->VALUES[i] = (float)(((inf_assoc *)ch->DATA)->dval*1e12);
      stmcaracslope->NVALUES = i;
      addhtitem (htslope_axis, name, (long)stmcaracslope) ;
    }
  freechain(list0);

  list0=inf_GetEntriesByType(ifl, INF_CAPAOUT, INF_ANY_VALUES);
  if (list0) htcapa_axis = addht (1000) ;
  for (cl=list0; cl!=NULL; cl=cl->NEXT) 
    {
      name=(char *)cl->DATA;
      inf_GetPointer(ifl, name, INF_CAPAOUT, (void **)&ch);
      stmcaraccapa = (stm_carac*)mbkalloc (sizeof (stm_carac));
      stmcaraccapa->VALUES = (float*)mbkalloc (countchain(ch) * sizeof (float)) ;
      for (i = 0 ; ch!=NULL ; i++, ch=ch->NEXT)
        stmcaraccapa->VALUES[i] = ((inf_assoc *)ch->DATA)->dval*1e15;
      stmcaraccapa->NVALUES = i;
      addhtitem (htcapa_axis, name, (long)stmcaraccapa) ;
    }
  freechain(list0);

  maincl=inf_GetEntriesByType(ifl, INF_PIN_RISING_SLEW, INF_ANY_VALUES);
  ch=inf_GetEntriesByType(ifl, INF_PIN_FALLING_SLEW, INF_ANY_VALUES);
  maincl=inf_SortEntries(inf_mergeclist (maincl, ch));
  if (maincl && !htslope_axis) htslope_axis = addht (1000) ;

  for (cl=maincl; cl!=NULL; cl=cl->NEXT) {
    slope = gethtitem(htslope_axis, (char *)cl->DATA);
    if ((slope == EMPTYHT) || (slope == DELETEHT)) {
      ns = (STM_DEF_AXIS_BASE*2+1);
      rise=fall=-1;
      inf_GetDouble(ifl, (char*)cl->DATA, INF_PIN_RISING_SLEW, &rise);
      inf_GetDouble(ifl, (char*)cl->DATA, INF_PIN_FALLING_SLEW, &fall);
      if (rise<0) rise=fall;
      if (fall<0) fall=rise;

      s_axis = stm_dyna_slews (STM_DEF_AXIS_BASE, mbk_long_round(((rise + fall)/2.0)*1e12));
      stmcaracslope = (stm_carac*)mbkalloc (sizeof (stm_carac));
      stmcaracslope->VALUES = (float*)mbkalloc (ns * sizeof (float)) ;
      for (i = 0 ; i < ns ; i++)
        stmcaracslope->VALUES[i] = (float)(s_axis[i]);
      stmcaracslope->NVALUES = ns;
      addhtitem (htslope_axis, cl->DATA, (long)stmcaracslope) ;
      mbkfree(s_axis);
    }
  }
  freechain(maincl);
  if(htslope_axis){
      ttvfig->USER = addptype(ttvfig->USER, TTV_FIG_SAXIS, htslope_axis);
  }
  if(htcapa_axis){
      ttvfig->USER = addptype(ttvfig->USER, TTV_FIG_CAXIS, htcapa_axis);
  }
}

void ttv_init_stm(ttvfig_list *ttvfig)
{
    if(!ttvfig) return;
    STM_DEF_SLEW = ttvfig->INFO->SLOPE/TTV_UNIT;
    if((ttvfig->INFO->STHLOW > 0.0) && (ttvfig->INFO->STHHIGH > 0.0)){
        STM_DEFAULT_SMINR   = ttvfig->INFO->STHLOW;
        STM_DEFAULT_SMAXR   = ttvfig->INFO->STHHIGH;
        STM_DEFAULT_SMINF   = ttvfig->INFO->STHLOW;
        STM_DEFAULT_SMAXF   = ttvfig->INFO->STHHIGH;
    }
    STM_DEFAULT_VFU     = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;    
    STM_DEFAULT_VFD     = (float)0.0;    
    STM_DEFAULT_VTN     = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE * 0.2;
    STM_DEFAULT_VTP     = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE * 0.2;
    STM_DEFAULT_VT      = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE * 0.2;
    STM_DEFAULT_TEMP    = ttvfig->INFO->TEMP;
}

float ttv_get_signal_output_capacitance(ttvfig_list *tvf, ttvsig_list *tvs)
{
 ptype_list *pt; 
 if (tvf==NULL || tvs==NULL) return 0;
 if (TTV_PROPAGATE_FORCED_CAPA>=0) return TTV_PROPAGATE_FORCED_CAPA;
 if ((pt=getptype(tvs->USER, TTV_SIG_OUTPUT_CAPACITANCE))!=NULL)
   return (*(float *)&pt->DATA)*1e15; 
 if ((pt=getptype(tvs->USER, TTV_SIG_UTD_OUTPUT_CAPA))!=NULL)
   return *(float *)&pt->DATA; 
 return tvf->INFO->CAPAOUT;
}

int ttv_get_signal_swing(ttvfig_list *tvf, ttvsig_list *tvs, float *low, float *high)
{
 ptype_list *pt; 
 ttv_swing_info *tsi;
 *low=*high=-1;
 if (tvf==NULL || tvs==NULL) return 1;
 if ((pt=getptype(tvs->USER, TTV_SIG_SWING))!=NULL)
 {
   tsi=(ttv_swing_info *)pt->DATA;
   *low=tsi->low;
   *high=tsi->high;
   return 0;
 }
 *low=0;
 *high=tvf->INFO->VDD;
 return 1;
}

void ttv_set_signal_swing(ttvsig_list *tvs, float low, float high)
{
  ttv_swing_info *tsi;
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SIG_SWING))!=NULL)
    tsi=(ttv_swing_info *)pt->DATA;
  else
  {
    tsi=mbkalloc(sizeof(ttv_swing_info));
    tvs->USER=addptype(tvs->USER, TTV_SIG_SWING, tsi);
  }
 tsi->low=low; 
 tsi->high=high; 
}

void ttv_getmargin(inffig_list *myfig, ttvsig_list *tvs, char *infregex, char *infsection, float *factor, long *delta)
{
  inf_miscdata *imd;
  *factor=1;
  *delta=0;
  if (inf_GetPointer(myfig, infregex, infsection, (void **)&imd))
  {
    if ((imd->lval & INF_MARGIN_ON_ALL)!=0
     || ((imd->lval & INF_MARGIN_ON_LATCH)!=0 && (tvs->TYPE & TTV_SIG_L)==TTV_SIG_L)
     || ((imd->lval & INF_MARGIN_ON_BREAK)!=0 && (tvs->TYPE & TTV_SIG_B)==TTV_SIG_B)
     || ((imd->lval & INF_MARGIN_ON_CONNECTOR)!=0 && (tvs->TYPE & TTV_SIG_C)==TTV_SIG_C)
     || ((imd->lval & INF_MARGIN_ON_PRECHARGE)!=0 && (tvs->TYPE & TTV_SIG_R)==TTV_SIG_R)
     || ((imd->lval & INF_MARGIN_ON_CMD)!=0 && (tvs->TYPE & TTV_SIG_Q)==TTV_SIG_Q)
     )
    {
      *factor=imd->dval;
      *delta=mbk_long_round(imd->dval1*1e12*TTV_UNIT);
    }
  }
}

chain_list *ttv_getallmarginregex(inffig_list *myfig)
{
  ht *tempht;
  chain_list *list, *cl;
  char *run[]=
    {
      INF_PATHDELAYMARGINMIN, INF_PATHDELAYMARGINMAX,
      INF_PATHDELAYMARGINCLOCK, INF_PATHDELAYMARGINDATA,
      INF_PATHDELAYMARGINRISE, INF_PATHDELAYMARGINFALL
    };
  int i, j, k;
  char section[128];

  tempht=addht(128);
  for (i=0; i<2; i++)
    for (j=2; j<4; j++)
      for (k=4; k<6; k++)
        {
          sprintf(section,INF_PATHDELAYMARGINPREFIX"|%s,%s,%s",run[i], run[j], run[k]);
          list=inf_GetEntriesByType(myfig,section,INF_ANY_VALUES);
          for (cl=list; cl!=NULL; cl=cl->NEXT) addhtitem(tempht, cl->DATA, 0);
          freechain(list);
        }

  list=inf_SortEntries(GetAllHTKeys(tempht));
  delht(tempht);

  return list;
} 

#define R_EMPTYHT  1000000000
#define R_DELETEHT 1000000001

static long ttv_changevalue(long val, int revert)
{
  if (!revert)
  {
    if (val==EMPTYHT) return R_EMPTYHT;
    else if (val==DELETEHT) return R_DELETEHT;
    return val;
  }
  else
  {
    if (val==R_EMPTYHT) return EMPTYHT;
    else if (val==R_DELETEHT) return DELETEHT;
    return val;
  }
}

ht *ttv_buildclockdetailht(NameAllocator *na, ttvfig_list *tvf, ttvpath_list *clockp, long findmode)
{
  long type = (clockp->TYPE | findmode) & ~TTV_FIND_DUAL;
  ttvcritic_list *detail, *d;
  char buf[2048];
  ht *cht;
  chain_list *cl;

  cht=addht(128);
  if (clockp->CRITIC==NULL)
    {
      if ((clockp->TYPE & TTV_FIND_ALL)==0)
        {
          detail = ttv_getcritic(tvf, clockp->FIG, clockp->ROOT, clockp->NODE, clockp->LATCH, clockp->CMDLATCH, type);
        } 
      else
        {
          cl = ttv_getcriticpara(tvf, clockp->FIG, clockp->ROOT, clockp->NODE, clockp->DELAY, type, NULL, clockp->TTV_MORE_SEARCH_OPTIONS);
          if (cl!=NULL) detail=(ttvcritic_list *)cl->DATA;
          else detail=NULL;
        }
    }
  else
    detail=clockp->CRITIC;

  for (d=detail; d!=NULL && d->NEXT!=NULL; d=d->NEXT)
    {
      sprintf(buf, "%s/%c->%s/%c", d->NAME, d->SNODE==TTV_UP?'u':'d', d->NEXT->NAME, d->NEXT->SNODE==TTV_UP?'u':'d');
      addhtitem(cht, NameAlloc(na, buf), ttv_changevalue(d->NEXT->DELAY, 0));
    }

  if (clockp->CRITIC==NULL)
    {
      if ((clockp->TYPE & TTV_FIND_ALL)==0)
        ttv_freecriticlist(detail);
      else
        ttv_freecriticpara(cl);
    }
  return cht;
}

void ttv_checkpathoverlap(NameAllocator *na, ttvfig_list *tvf, ttvpath_list *datap, ht *cht, long findmode, long *overlapclock, long *overlapdata)
{
  long type = (datap->TYPE | findmode) & ~TTV_FIND_DUAL;
  long l;
  ttvcritic_list *detail, *d;
  char buf[2048];
  chain_list *cl;

  *overlapclock=0;
  *overlapdata=0;

  if (cht==NULL) return;

  if (datap->CRITIC==NULL)
    {
      if ((datap->TYPE & TTV_FIND_ALL)==0)
        {
          detail = ttv_getcritic(tvf, datap->FIG, datap->ROOT, datap->NODE, datap->LATCH, datap->CMDLATCH, type);
        }
      else
        {
          cl = ttv_getcriticpara(tvf, datap->FIG, datap->ROOT, datap->NODE, datap->DELAY, type, NULL, datap->TTV_MORE_SEARCH_OPTIONS);
          if (cl!=NULL) detail=(ttvcritic_list *)cl->DATA;
          else detail=NULL;
        }
    }
  else detail=datap->CRITIC;
  for (d=detail; d!=NULL && d->NEXT!=NULL; d=d->NEXT)
    {
      sprintf(buf, "%s/%c->%s/%c", d->NAME, d->SNODE==TTV_UP?'u':'d', d->NEXT->NAME, d->NEXT->SNODE==TTV_UP?'u':'d');
      if ((l=gethtitem(cht, NameAlloc(na, buf)))!=EMPTYHT)
      {
        *overlapdata+=d->NEXT->DELAY;
        *overlapclock+=ttv_changevalue(l, 1);
      }
    }
  
  if (datap->CRITIC==NULL)
  {
    if ((datap->TYPE & TTV_FIND_ALL)==0)
      ttv_freecriticlist(detail);
    else
      ttv_freecriticpara(cl);
  }
}

int ttv_mark_signals_with_corresponding_path_margin_regex(ttvfig_list *tvf)
{
  chain_list *list, *cl, *chain, *chainsig;
  inffig_list *ifl;
  ttvsig_list *tvs;

  if ((ifl=getloadedinffig(tvf->INFO->FIGNAME))==NULL) return 0;
  list=ttv_getallmarginregex(ifl);

  if (list!=NULL)
  {
     // desabling ttv cache
     ttv_disablecache(tvf);
/*     chain=ttv_levelise(tvf,tvf->INFO->LEVEL,ttv_getloadedfigtypes(tvf));
     freechain(chain);*/
  }

  for (cl=list; cl!=NULL; cl=cl->NEXT)
    {
      chain=addchain(NULL, cl->DATA);
      
      chainsig = ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,chain);
      freechain(chain);
          
      for (chain=chainsig; chain!=NULL; chain=delchain(chain, chain))
        {
          tvs=(ttvsig_list *)chain->DATA;
          if (getptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX)==NULL)
            tvs->USER=addptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX, cl->DATA);
        }
    }
  freechain(list);
  return 1;
}

void ttv_unmark_signals_corresponding_path_margin_regex(ttvfig_list *tvf)
{
  chain_list *chain, *chainsig;
  inffig_list *ifl;
  ttvsig_list *tvs;

  if ((ifl=getloadedinffig(tvf->INFO->FIGNAME))!=NULL)
    {
      chain=addchain(NULL, "*");
      
      chainsig = ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,chain);
      freechain(chain);
      
      for (chain=chainsig; chain!=NULL; chain=delchain(chain, chain))
        {
          tvs=(ttvsig_list *)chain->DATA;
          tvs->USER=testanddelptype(tvs->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX);
        }
    }
}

int ttv_get_path_margins_info(ttvfig_list *tvf, ttvpath_list *path, float *factor, long *delta)
{
  int trans;
  char *infregex, *pathtype;
  inffig_list *ifl;
  ttvsig_list *tvs;
  ptype_list *pt;
  char section[1024];
  char *clockordata;

  *factor=1; *delta=0;

  if ((ifl=getloadedinffig(tvf->INFO->FIGNAME))==NULL) return 0;

  tvs=path->ROOT->ROOT;
  if ((pt=getptype(path->ROOT->ROOT->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX))==NULL) return 0;

  infregex=(char *)pt->DATA;

  if (getptype(path->NODE->ROOT->USER, TTV_SIG_CLOCK)!=NULL) clockordata=INF_PATHDELAYMARGINCLOCK; else clockordata=INF_PATHDELAYMARGINDATA;
  if ((path->TYPE & TTV_FIND_MIN)!=0) pathtype=INF_PATHDELAYMARGINMIN; else pathtype=INF_PATHDELAYMARGINMAX;
  if ((path->ROOT->TYPE & TTV_NODE_UP)!=0) trans=1; else trans=0;

  sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s,%s,%s", pathtype, clockordata, trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  ttv_getmargin(ifl, tvs, infregex, section, factor, delta);
  
  return 1;
}

int ttv_get_path_margins(ttvfig_list *tvf, ttvpath_list *datapath, ttvpath_list *clockpath, long *margedata, long *margeclock, long *overlapc, long *overlapd, int clockisdata, int dataisclock)
{
  int trans;
  char *infregex, *pathtype;
  inffig_list *ifl;
  float factor_data, factor_clock, newdatapathdelay, dmclock;
  long delta_data, delta_clock;
  long findmode;
  NameAllocator na;
  ttvsig_list *tvs;
  long CPD, DPD;
  ptype_list *pt;
  char section[1024];
  ht *cht;

  *margedata=*margeclock=*overlapc=*overlapd=0;

  if (datapath==NULL) return 0;
  if ((ifl=getloadedinffig(tvf->INFO->FIGNAME))==NULL) return 0;

  findmode=TTV_FIND_LINE;

  tvs=datapath->ROOT->ROOT;
  if ((pt=getptype(datapath->ROOT->ROOT->USER, TTV_SIG_PATH_DELAY_MARGINS_REGEX))==NULL) return 0;

  infregex=(char *)pt->DATA;

  if ((datapath->ROOT->TYPE & TTV_NODE_UP)!=0) trans=1; else trans=0;

  if (clockpath!=NULL)
    {
      if ((clockpath->TYPE & TTV_FIND_MIN)!=0) pathtype=INF_PATHDELAYMARGINMIN;
      else pathtype=INF_PATHDELAYMARGINMAX;
      if (!clockisdata)
        sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINCLOCK",""%s", pathtype, (clockpath->NODE->TYPE & TTV_NODE_UP)==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);
      else
        sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINDATA",""%s", pathtype, (clockpath->NODE->TYPE & TTV_NODE_UP)==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);
//      ttv_getmargin(ifl, tvs, infregex, section, &factor_clock, &delta_clock);
      ttv_getmargin(ifl, clockpath->ROOT->ROOT, infregex, section, &factor_clock, &delta_clock);
    }
  else factor_clock=1, delta_clock=0;

  if ((datapath->TYPE & TTV_FIND_MIN)!=0) pathtype=INF_PATHDELAYMARGINMIN;
  else pathtype=INF_PATHDELAYMARGINMAX;
  if ((datapath->ROOT->TYPE & TTV_NODE_UP)!=0) trans=1; else trans=0;

  if (!dataisclock)
    sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINDATA",%s", pathtype, trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);
  else
    sprintf(section,INF_PATHDELAYMARGINPREFIX"|""%s"","INF_PATHDELAYMARGINCLOCK",%s", pathtype, trans==0?INF_PATHDELAYMARGINFALL:INF_PATHDELAYMARGINRISE);

  ttv_getmargin(ifl, tvs, infregex, section, &factor_data, &delta_data);

  if (factor_clock==1 && factor_data==1 && delta_clock==0 && delta_data==0) return 1;

  if (clockpath==NULL) dmclock=delta_clock, cht=NULL;
  else
    {
      CPD=clockpath->DELAY;
      newdatapathdelay=clockpath->DELAY*factor_clock+delta_clock;
      CreateNameAllocator(23, &na, CASE_SENSITIVE);
      cht=ttv_buildclockdetailht(&na, tvf, clockpath, findmode);
      dmclock=newdatapathdelay-clockpath->DELAY;
    }

  if (datapath==NULL) dmclock=delta_clock, *overlapc=*overlapd=0;
  else
    {
      newdatapathdelay=datapath->DELAY*factor_data+delta_data;
      ttv_checkpathoverlap(&na, tvf, datapath, cht, findmode, overlapc, overlapd);
      newdatapathdelay-=*overlapd*factor_data-*overlapd;
      DPD=datapath->DELAY;
      *margedata=mbk_long_round(newdatapathdelay-datapath->DELAY);
    }

  *margeclock=mbk_long_round(dmclock-*overlapc*factor_clock+*overlapc);

  if (cht!=NULL) { delht(cht); DeleteNameAllocator(&na); }
  
  return 1;
}

void ttv_checkinfchange(ttvfig_list *tvf)
{
 inffig_list *ifl;
 
 if ((ifl=getinffig(tvf->INFO->FIGNAME))!=NULL)
 {
   if (inf_getinffig_state(ifl))
   {
     ttv_getinffile(tvf); 
     inf_resetinffig_state(ifl);
   }
 }
}

chain_list *ttv_getoutputlines(ttvevent_list *node, long type)
{
  ptype_list *ptype;
  if((type & TTV_FIND_PATH) == TTV_FIND_PATH)
    ptype = getptype(node->USER,TTV_NODE_DUALPATH) ;
  else
    ptype = getptype(node->USER,TTV_NODE_DUALLINE) ;

  if(ptype == NULL)
    return(NULL) ;

  return (chain_list *)ptype->DATA ;
}

ttvevent_list *ttv_opposite_event(ttvevent_list *cmd_ev)
{
  if (cmd_ev==NULL) return NULL;
  if (cmd_ev->TYPE & TTV_NODE_UP) cmd_ev=cmd_ev->ROOT->NODE; else cmd_ev=cmd_ev->ROOT->NODE+1;
  return cmd_ev;
}

ttvpath_list *ttv_mergepathlists(ttvpath_list *pathx, int nbx, ttvpath_list *path, int nb, int max, long type, int *nbtot)
{
  ttvpath_list *pathr=NULL, *pathxx, *pathh=NULL;
  int nbe;
  // les deux liste sont inversées, ont garde les max elem de la fin
  nbe=0;
  *nbtot=0;
  while(pathx != NULL || path != NULL)
   {
    if(max!=TTV_MAX_PATHNB && nbe < nbx+nb-max)
      {
       nbe++ ;
       if(path==NULL || (pathx!=NULL && (((path->DELAY > pathx->DELAY) &&
          ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
          ((path->DELAY < pathx->DELAY) &&
          ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))))
          {
           pathxx = pathx ;
           pathx = pathx->NEXT ;
           pathxx->NEXT = NULL ;
           ttv_freepathlist(pathxx) ;
          }
        else
          {
           pathxx = path ;
           path = path->NEXT ;
           pathxx->NEXT = NULL ;
           ttv_freepathlist(pathxx) ;
          }
      }
    else
      {
       if((path!=NULL) && ((pathx == NULL) || (((path->DELAY < pathx->DELAY) &&
          ((type & TTV_FIND_MAX) == TTV_FIND_MAX)) ||
          ((path->DELAY > pathx->DELAY) &&
          ((type & TTV_FIND_MIN) == TTV_FIND_MIN)))))
          {
           pathxx = pathx ;
           pathx = path ;
           path = pathxx ;
          }

       (*nbtot)++;
       if(pathr == NULL)
         {
          pathr = pathx ;
          pathh = pathr ;
          pathx = pathx->NEXT ;
          pathr->NEXT = NULL ;
         }
       else
         {
          pathr->NEXT = pathx ;
          pathxx = pathx->NEXT ;
          pathx->NEXT = NULL ;
          pathr = pathx ;
          pathx = pathxx ;
         }
      }
   }
  return(pathh);
}

static int ttv_marked_for(ttvevent_list *evi, ttvpath_list *pth, int multiplecmd)
{
  ptype_list *pt;
  chain_list *cl;
  ttvpath_list *pth0;

  if ((pt=getptype(evi->USER, TTV_NODE_TMP_MARKED))!=NULL)
  {
    for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
    {
      pth0=(ttvpath_list *)cl->DATA;
      if (pth0->ROOT==pth->ROOT && pth0->NODE==pth->NODE &&
          pth0->LATCH==pth->LATCH && (!multiplecmd || pth0->CMDLATCH==pth->CMDLATCH) &&
          (pth0->TYPE & TTV_FIND_HZ)==(pth->TYPE & TTV_FIND_HZ) &&
          pth0->CMD==pth->CMD) return 1;
    }
  }
  return 0;
}

static void ttv_mark_for(ttvevent_list *evi, ttvpath_list *pth)
{
  ptype_list *pt;
  if ((pt=getptype(evi->USER, TTV_NODE_TMP_MARKED))==NULL)
  {
     pt=evi->USER=addptype(evi->USER, TTV_NODE_TMP_MARKED, NULL);
  }
  pt->DATA=addchain((chain_list *)pt->DATA, pth);
}

ttvpath_list *ttv_keep_critic_paths(ttvpath_list *pth, int *nb, long type, int multiplecmd)
{
  ttvpath_list *path, *next;
  chain_list *cl=NULL, *ch;
  ptype_list *pt;
  int hz;
  ttvevent_list *mid, *clk, *start;
   
  pth=ttv_classpath(pth,type & ~TTV_FIND_NOTCLASS);

  for (path=pth; path!=NULL; path=next)
  {
    next=path->NEXT;
    if ((path->TYPE & TTV_FIND_HZ)==TTV_FIND_HZ) hz=1;
    else hz=0;
    if ((type & TTV_FIND_DUAL)==0)
    { 
      if (path->LATCH!=NULL) mid=path->LATCH, clk=path->NODE, start=path->ROOT;
      else mid=path->NODE, clk=NULL, start=path->ROOT;
    }
    else
    {
      if (path->LATCH!=NULL) mid=path->LATCH, clk=path->ROOT, start=path->NODE;
      else mid=path->ROOT, clk=NULL, start=path->NODE;
    }
    if (!ttv_marked_for(mid, path, multiplecmd))
    {
      cl=addchain(cl, path);
      ttv_mark_for(mid, path);
    }
    else
    {
       path->NEXT=NULL;
       ttv_freepathlist(path);
       (*nb)--;
    }
  }

  path=NULL;
  for (ch=cl; ch!=NULL; ch=ch->NEXT)
  {
     pth=(ttvpath_list *)ch->DATA;
     pth->NEXT=path;
     if ((type & TTV_FIND_DUAL)==0)
     {
       if (pth->LATCH!=NULL) mid=pth->LATCH; else mid=pth->NODE;
     }
     else
     {
       if (pth->LATCH!=NULL) mid=pth->LATCH; else mid=pth->ROOT;
     }

     if ((pt=getptype(mid->USER, TTV_NODE_TMP_MARKED))!=NULL)
     {
       freechain(pt->DATA);
       mid->USER=testanddelptype(mid->USER, TTV_NODE_TMP_MARKED);
     }

     path=pth;
  }
  
  freechain(cl);

  path=(ttvpath_list *)reverse((chain_list *)path);

  return path;
}

void ttv_set_UTD_slope(ttvevent_list *tve, long slope)
{
  ptype_list *pt;
  if ((pt=getptype(tve->USER, TTV_NODE_UTD_INPUT_SLOPE))==NULL)
    pt=tve->USER=addptype(tve->USER, TTV_NODE_UTD_INPUT_SLOPE, 0);

  pt->DATA=(void *)slope;
}

void ttv_set_UTD_outputcapa(ttvsig_list *tvs, float capa)
{
  ptype_list *pt;
  if ((pt=getptype(tvs->USER, TTV_SIG_UTD_OUTPUT_CAPA))==NULL)
    pt=tvs->USER=addptype(tvs->USER, TTV_SIG_UTD_OUTPUT_CAPA, 0);

  *((float *)&pt->DATA)=capa;
}

void ttv_update_slope_and_output_capa(ttvfig_list *tf)
{
  chain_list *allcon;
  ttvsig_list *tvs;
  allcon=ttv_getsigbytype_and_netname(tf,NULL,TTV_SIG_C,NULL) ;
  while (allcon!=NULL)
  {
    tvs=(ttvsig_list *)allcon->DATA;
    ttv_set_UTD_slope(&tvs->NODE[0], ttv_getnodeslew(&tvs->NODE[0], TTV_FIND_MAX));
    ttv_set_UTD_slope(&tvs->NODE[1], ttv_getnodeslew(&tvs->NODE[1], TTV_FIND_MAX));
    ttv_set_UTD_outputcapa(tvs, ttv_get_signal_output_capacitance(tf, tvs));
    allcon=delchain(allcon, allcon);
  }
}

int ttv_check_update_slope_and_output_capa(ttvfig_list *tf)
{
  chain_list *allcon;
  ptype_list *pt;
  ttvsig_list *tvs;
  float capa;
  int diff=0;
  long slope;
  allcon=ttv_getsigbytype_and_netname(tf,NULL,TTV_SIG_C,NULL) ;
  while (allcon!=NULL)
  {
    tvs=(ttvsig_list *)allcon->DATA;
    if ((pt=getptype(tvs->NODE[0].USER, TTV_NODE_UTD_INPUT_SLOPE))==NULL) slope=tvs->ROOT->INFO->SLOPE;
    else slope=(long)pt->DATA;
    if (slope!=ttv_getnodeslew(&tvs->NODE[0], TTV_FIND_MAX)) diff=1;
    if ((pt=getptype(tvs->NODE[1].USER, TTV_NODE_UTD_INPUT_SLOPE))==NULL) slope=tvs->ROOT->INFO->SLOPE;
    else slope=(long)pt->DATA;
    if (slope!=ttv_getnodeslew(&tvs->NODE[1], TTV_FIND_MAX)) diff=1;
    if ((pt=getptype(tvs->USER, TTV_SIG_UTD_OUTPUT_CAPA))==NULL) capa=-1;
    else capa=*(float *)&pt->DATA;
    if (fabs(capa-ttv_get_signal_output_capacitance(tf, tvs))>1e-10) diff=1;
    allcon=delchain(allcon, allcon);
    if (diff) break;
  }
  freechain(allcon);
  return diff;
}

int ttv_isgateoutput(ttvfig_list *tvf, ttvsig_list *tvs,long type)
{
  int i;
  ttvline_list *tl;

  for (i=0; i<1; i++)
  {
          
   if((type & TTV_FILE_DTX)==TTV_FILE_DTX)
     tl=tvs->NODE[i].INLINE;
   else
     tl=tvs->NODE[i].INPATH;

   while (tl!=NULL)
   {
     if ((tl->TYPE & TTV_LINE_RC)!=0) return 0;
     else if ((tl->TYPE & TTV_LINE_CONT)==0) return 1;

     tl=tl->NEXT;
   }
  }
  return 1;
}

int ttv_has_strict_setup(ttvevent_list *tve)
{
  if ((tve->ROOT->TYPE & TTV_SIG_LL)==TTV_SIG_LL
      && getptype(tve->ROOT->USER, TTV_SIG_STRICT_SETUP)!=NULL) return 1;
  return 0;
}


ht *ttv_buildquickaccessht(ttvfig_list *tvf, NameAllocator *NA)
{
  chain_list *chainsig;
  ht *h;
  char buf[1024], *nname;
  ttvsig_list *tvs;

  CreateNameAllocator(10000, NA, CASE_SENSITIVE);

  chainsig = ttv_getsigbytype(tvf,NULL,TTV_SIG_TYPEALL,NULL) ;

  h=addht(100000);
  while (chainsig!=NULL)
    {
      tvs=(ttvsig_list *)chainsig->DATA;

      if (ttv_isgateoutput(tvf,tvs,TTV_FILE_DTX))
      {
        ttv_getnetname(tvf, buf, tvs);
        nname=NameAlloc(NA, buf);
        addhtitem(h, nname, (long)tvs);
      }
      chainsig=delchain(chainsig, chainsig);
    }
  return h;
}

chain_list *ttv_get_signals_from_netname(ttvfig_list *tvf, ht **h, char *netname)
{
  chain_list *chainsig;
  char buf[1024], *nname;
  ttvsig_list *tvs;
  long l;
 
  if (*h==NULL)
  {
    chainsig = ttv_getsigbytype(tvf,NULL,TTV_SIG_TYPEALL,NULL) ;
    *h=addht(10000);
    while (chainsig!=NULL)
      {
        tvs=(ttvsig_list *)chainsig->DATA;
        l=gethtitem(*h, tvs->NETNAME);
        if (l==EMPTYHT) l=(long)NULL;
        addhtitem(*h, tvs->NETNAME, (long)addchain((chain_list *)l, tvs));
        chainsig=delchain(chainsig, chainsig);
      }
  }
  l=gethtitem(*h, netname);
  if (l==EMPTYHT)
  {
    if (mbk_isregex_name(netname))
    {
      chain_list *tmp;
      nname=namealloc(netname);
      chainsig=addchain(NULL, nname);
      tmp=ttv_getsigbytype_and_netname(tvf,NULL,TTV_SIG_TYPEALL,chainsig) ;
      freechain(chainsig);
      addhtitem(*h, nname, (long)tmp);
      return tmp;
    }
    else
     return NULL;
  }
  return (chain_list *)l;
}
void ttv_free_signals_from_netname(ht *h)
{
  chain_list *chainsig;
 
  if (h!=NULL)
  {
    chainsig = GetAllHTElems(h);
    while (chainsig!=NULL)
      {
        freechain((chain_list *)chainsig->DATA);
        chainsig=delchain(chainsig, chainsig);
      }
    delht(h);
  }
}

