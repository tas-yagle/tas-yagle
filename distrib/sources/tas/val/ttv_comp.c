/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TTVDIFF Version 1                                           */
/*    Fichier : ttv_comp.c                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "ttvdiff.h"
#define     DIFF_MISS   ((long) 0x00000001 )
#define     DIFF_EXTRA  ((long) 0x00000002 )
#define     DIFF_CHANGE ((long) 0x00000004 )
#define     DIFF_PATH   ((long) 0x00000008 )
#define     DIFF_LINE   ((long) 0x00000010 )


int          HTSIZE;
ht          *HTSIG12;
ht          *HTSIG21;
ht          *HTSIG1NAME;
ht          *HTSIG2NAME;
chain_list  *SIGLIST1;
chain_list  *SIGLIST2;

static int   DIFF_NB = 0;

#define TCAPA     0
#define TSLEW     1
#define TLINERC   2
#define TPATH     3
#define TOTHERS   4
#define TSHA      5
#define TCOMBI    6
#define TLINEGATE 7
#define TLAST     8

#define LOWLIM  (499-250)
#define HIGHLIM (501+250)
#define RANGE   10

static int errcnt[TLAST][1000];

const struct {
  char *header;
  int num;
} HH[]=
  {
    {"ErrRanges", -1},
    {"Capa", TCAPA},
    {"Slope", TSLEW},
    {"Gate", TLINEGATE},
    {"RC", TLINERC},
    {"Path", TPATH},
    {"S/H/A", TSHA},
    {"Comb", TCOMBI},
    {"Others", TOTHERS}
  };

#define TMEMISS   0
#define TMEEXTRA  1
#define TMEMISMATCH 2
#define TMESIGNAL 0
#define TMELINE   1
#define TMERC     2
#define TMEMODEL  3
#define TMENETNAME 4
#define TMEOTHERS  5
#define TMESHA     6
#define TMELAST   7

static int errcntME[TMELAST][3];

const struct {
  char *header;
  int num;
} MEHH[]=
  {
    {"M/E", -1},
    {"Signal", TMESIGNAL},
    {"Gate", TMELINE},
    {"S/H/A", TMESHA},
    {"RC", TMERC},
    {"Models", TMEMODEL},
    {"Netname", TMENETNAME},
    {"Others", TMEOTHERS}
  };

/****************************************************************************/
/*{{{                    Static functions                                   */
/****************************************************************************/
/*{{{                    print()                                            */
/*                                                                          */
/* count difference                                                         */
/****************************************************************************/
static void print(FILE *fp, char *text, ... )
{
  va_list    arg;

  DIFF_NB ++;

  va_start(arg,text);
  vfprintf(fp,text,arg);
  va_end(arg);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Extern functions                                   */
/****************************************************************************/
/*{{{                    diff_lookinfodiff()                                */
/* parametres :                                                             */
/*                                                                          */
/****************************************************************************/
void diff_lookinfodiff(file,info1,info2)
FILE            *file;
ttvinfo_list    *info1;
ttvinfo_list    *info2;
{
  if (info1->TOOLNAME!=info2->TOOLNAME)
    print(file,"\t->different tool %s for %s and %s for %s\n",
          info1->TOOLNAME,info1->FIGNAME,info2->TOOLNAME,info2->FIGNAME);
  else
    fprintf(file,"tool : %s\n",info1->TOOLNAME);
  if (info1->TOOLVERSION!=info2->TOOLVERSION)
    print(file,"\t->different tool version %s for %s and %s for %s\n",
          info1->TOOLVERSION,info1->FIGNAME,info2->TOOLVERSION,info2->FIGNAME);
  else
    fprintf(file,"tool version : %s\n",info1->TOOLVERSION);
  if (info1->TECHNONAME!=info2->TECHNONAME)
    print(file,"\t->different techno %s for %s and %s for %s\n",
          info1->TECHNONAME,info1->FIGNAME,info2->TECHNONAME,info2->FIGNAME);
  else
    fprintf(file,"techno : %s\n",info1->TECHNONAME);
  if (info1->TECHNOVERSION!=info2->TECHNOVERSION)
    print(file,"\t->different techno version %s for %s and %s for %s\n",
          info1->TECHNOVERSION,info1->FIGNAME,info2->TECHNOVERSION,info2->FIGNAME);
  else
    fprintf(file,"techno version : %s\n",info1->TECHNOVERSION);
  if (info1->TTVYEAR!=info2->TTVYEAR)
    print(file,"\t->different year %d for %s and %d for %s\n",
          info1->TTVYEAR,info1->FIGNAME,info2->TTVYEAR,info2->FIGNAME);
  else
    fprintf(file,"year : %d\n",info1->TTVYEAR);
  if (info1->TTVMONTH!=info2->TTVMONTH)
    print(file,"\t->different month %d for %s and %d for %s\n",
          info1->TTVMONTH,info1->FIGNAME,info2->TTVMONTH,info2->FIGNAME);
  else
    fprintf(file,"month : %d\n",info1->TTVMONTH);
  if (info1->TTVDAY!=info2->TTVDAY)
    print(file,"\t->different day %d for %s and %d for %s\n",
          info1->TTVDAY,info1->FIGNAME,info2->TTVDAY,info2->FIGNAME);
  else
    fprintf(file,"day : %d\n",info1->TTVDAY);
  if (info1->TTVHOUR!=info2->TTVHOUR)
    print(file,"\t->different hour %d for %s and %d for %s\n",
          info1->TTVHOUR,info1->FIGNAME,info2->TTVHOUR,info2->FIGNAME);
  else
    fprintf(file,"hour : %d\n",info1->TTVHOUR);
  if (info1->TTVMIN!=info2->TTVMIN)
    print(file,"\t->different min %d for %s and %d for %s\n",
          info1->TTVMIN,info1->FIGNAME,info2->TTVMIN,info2->FIGNAME);
  else
    fprintf(file,"min : %d\n",info1->TTVMIN);
  if (info1->TTVSEC!=info2->TTVSEC)
    print(file,"\t->different sec %d for %s and %d for %s\n",
          info1->TTVSEC,info1->FIGNAME,info2->TTVSEC,info2->FIGNAME);
  else
    fprintf(file,"sec : %d\n",info1->TTVSEC);
  if (info1->SLOPE!=info2->SLOPE)
    print(file,"\t->different slope %ld for %s and %ld for %s\n",
          info1->SLOPE,info1->FIGNAME,info2->SLOPE,info2->FIGNAME);
  else
    fprintf(file,"slope : %ld\n",info1->SLOPE);
  if (info1->CAPAOUT!=info2->CAPAOUT)
    print(file,"\t->different capaout %.2f for %s and %.2f for %s\n",
          info1->CAPAOUT,info1->FIGNAME,info2->CAPAOUT,info2->FIGNAME);
  else
    fprintf(file,"capaout : %.2f\n",info1->CAPAOUT);
}

/*}}}************************************************************************/
/*{{{                    diff_htloadnamesig()                               */
/* parametres :                                                             */
/* ttffig : fig de base                                                     */
/* siglist : liste des pointeurs de signaux                                 */
/*                                                                          */
/* cree une table de hash et la remplit avec les pointeurs sur les signaux  */
/* en les reperant par leurs noms                                           */
/****************************************************************************/
ht  *diff_htloadnamesig(ttvfig,siglist)
ttvfig_list     *ttvfig;
chain_list      *siglist;
{
  chain_list    *chainx;
  char          *ptname;
  ttvsig_list   *ptsig;
  char          buf[1024];
  ht            *htab;

  for (chainx= siglist, HTSIZE=0; chainx; chainx=chainx->NEXT, HTSIZE++);
  htab = addht(HTSIZE);

  for (chainx= siglist; chainx; chainx= chainx->NEXT)
  {
    ptsig = chainx->DATA;

    ptname = namealloc(ttv_getsigname(ttvfig, buf, ptsig));
    addhtitem(htab, ptname, (long)ptsig);
  }

  return (htab);
}

/*}}}************************************************************************/
/*{{{                    diff_printdiffsig()                                */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* ptname : nom du signal                                                   */
/* ttvfig1 : fig source                                                     */
/* ttvfig2 : fig a comparer                                                 */
/*                                                                          */
/* affiche les differences de signaux                                       */
/****************************************************************************/
void diff_printdiffsig(file,ptname,ttvfig2,type,mask)
FILE        *file;
char        *ptname;
ttvfig_list *ttvfig2;
long        type;
long        mask;
{
  if ((mask&DIFF_OPT_NSIG) == 0)
  {
    if (type == DIFF_MISS)
    {
      if ((mask&DIFF_OPT_IMISS) == 0)
      {
        print(file,"missing signal %s from %s\n",
              ptname,ttvfig2->INFO->FIGNAME);
        errcntME[TMESIGNAL][TMEMISS]++;
      }
    }
    else if (type == DIFF_EXTRA)
      if ( (mask&DIFF_OPT_IEXTRA) == 0 )
      {
        print(file,"extra signal %s not found in %s\n",
              ptname,ttvfig2->INFO->FIGNAME);
        errcntME[TMESIGNAL][TMEEXTRA]++;
      }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_printdiffsigtype()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
diff_printdiffsigtype(FILE *file, char *name, long type)
{
  if (type == DIFF_MISS)
  {
    print(file,"net: %s typemismatch\n",name);
    errcntME[TMESIGNAL][TMEMISMATCH]++;
  }
}

/*}}}************************************************************************/
/*{{{                    diff_printdiffcapa()                               */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* ptname : nom du signal                                                   */
/* capa1 : capacite source                                                  */
/* capa2 : capacite obtenue                                                 */
/*                                                                          */
/* affiche les differences de capa                                          */
/****************************************************************************/
void diff_printdiffcapa(file,ptname,capa1,capa2,type)
FILE        *file;
char        *ptname;
float       capa1;
float       capa2;
long        type;
{
  float         diff;
  long          diffpercent, pos;
  char          sign;

  diff = capa2-capa1;
  sign = (diff<0.0)?'-':'+';
  diff = fabs(diff);
  diffpercent = labs(((capa1<capa2)?(long)((float)diff*100.0/(float)capa2):(long)((float)diff*100.0/(float)capa1)));

  if ( ( (diff >= (float)DELTA)
         && ((type&(DIFF_OPT_DELTA|DIFF_OPT_DELTAC)) == DIFF_OPT_DELTA ) )
       || ( (diff >= (float)DELTAC)
            &&((type&(DIFF_OPT_DELTAC)) ==DIFF_OPT_DELTAC))
       || ( (diffpercent >= DELTA)
            && ((type&(DIFF_OPT_DELTAP|DIFF_OPT_DELTACP)) == DIFF_OPT_DELTAP ) )
       || ( (diffpercent >= DELTAC)
            &&((type&(DIFF_OPT_DELTACP)) ==DIFF_OPT_DELTACP))
       || ( ((type&(DIFF_OPT_DELTA|DIFF_OPT_DELTAC| DIFF_OPT_DELTAP|
                    DIFF_OPT_DELTACP)) == 0 )))
    {
      print(file,"change capacitance of signal %s %c%.2ffF ( %c%ld%% )\n",
            ptname,sign,diff,sign,diffpercent);
      
      if (sign=='-')
        pos=499-(diffpercent/RANGE);
      else
        pos=501+(diffpercent/RANGE);
      if (pos<LOWLIM || pos>HIGHLIM) pos=0;
      errcnt[TCAPA][pos]++;
    }
}

/*}}}************************************************************************/
/*{{{                    diff_sigcapa()                                     */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* siglist1 : liste des signaux de la figure 1                              */
/* htsig12 : hashtable de correspondance des signaux                        */
/*                                                                          */
/* recherche les differences de capa                                        */
/****************************************************************************/
void diff_sigcapa(file,ttvfig1,siglist1,htsig12,type)
FILE        *file;
ttvfig_list *ttvfig1;
chain_list  *siglist1;
ht          *htsig12;
long        type;
{
  chain_list    *chainx;
  ttvsig_list   *ptsig1;
  ttvsig_list   *ptsig2;
  char          *ptname;
  char          buf[1024];

  for (chainx= siglist1; chainx; chainx= chainx->NEXT)
  {
    ptsig1 = chainx->DATA;
    ptsig2 = (ttvsig_list *)gethtitem(htsig12,ptsig1);
    ptname = namealloc(ttv_getsigname(ttvfig1, buf, ptsig1));
    if ((long)ptsig2 != EMPTYHT)
      if (ptsig1->CAPA!=ptsig2->CAPA)
        diff_printdiffcapa(file,ptname,ptsig1->CAPA,ptsig2->CAPA,type);
  }
}

/*}}}************************************************************************/
/*{{{                    diff_printdiffnetname()                            */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* netname1 : netname     source                                            */
/* netname2 : netname     obtenue                                           */
/* ttvfig1 : fig source                                                     */
/* ttvfig2 : fig a comparer                                                 */
/*                                                                          */
/* affiche les differences de netname                                       */
/****************************************************************************/
void diff_printdiffnetname(file,netname1,ttvfig1,netname2,ttvfig2)
FILE        *file;
char        *netname1;
ttvfig_list *ttvfig1;
char        *netname2;
ttvfig_list *ttvfig2;
{
  print(file,"change netname %s in %s, %s in %s\n",
        netname1,ttvfig1->INFO->FIGNAME,netname2,ttvfig2->INFO->FIGNAME);
  errcntME[TMENETNAME][TMEMISMATCH]++;
}

/*}}}************************************************************************/
/*{{{                    diff_signetname()                                  */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* siglist1 : liste des signaux de la figure 1                              */
/* htsig12 : hashtable de correspondance des signaux                        */
/*                                                                          */
/* recherche les differences de netname                                     */
/****************************************************************************/
void diff_signetname(file,ttvfig1,ttvfig2,siglist1,htsig12)
FILE        *file;
ttvfig_list *ttvfig1;
ttvfig_list *ttvfig2;
chain_list  *siglist1;
ht          *htsig12;
{
  chain_list    *chainx;
  ttvsig_list   *ptsig1;
  ttvsig_list   *ptsig2;
  char          *ptname1;
  char          *ptname2;
  char          buf1[1024];
  char          buf2[1024];

  for (chainx= siglist1; chainx; chainx= chainx->NEXT)
  {
    ptsig1 = chainx->DATA;
    ptsig2 = (ttvsig_list *)gethtitem(htsig12,ptsig1);
    if ((long)ptsig2 != EMPTYHT)
    {
      ptname1 = ttv_getnetname(ttvfig1, buf1, ptsig1);
      ptname2 = ttv_getnetname(ttvfig2, buf2, ptsig2);
      if (strcmp(ptname1,ptname2))
        diff_printdiffnetname(file,ptname1,ttvfig1,ptname2,ttvfig2);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_printdiffmodel()                              */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* model1 : model source                                                    */
/* model2 : model obtenue                                                   */
/* ttvfig1 : fig source                                                     */
/* ttvfig2 : fig a comparer                                                 */
/*                                                                          */
/* affiche les differences de model                                         */
/****************************************************************************/
void diff_printdiffmodel(file,model1,ttvfig1,model2,ttvfig2,modeltype)
FILE        *file;
char        *model1;
ttvfig_list *ttvfig1;
char        *model2;
ttvfig_list *ttvfig2;
char        *modeltype;
{
  print(file,"change %s %s in %s, %s in %s\n", modeltype,
        model1,ttvfig1->INFO->FIGNAME,model2,ttvfig2->INFO->FIGNAME);
  errcntME[TMEMODEL][TMEMISMATCH]++;
}

/*}}}************************************************************************/
/*{{{                    diff_htsig12()                                     */
/* parametres :                                                             */
/* siglist  : liste des signaux de la figure                                */
/* htsigname : hashtable de correspondance signaux noms                     */
/* ttvfig1 : fig source                                                     */
/* ttvfig2 : fig a comparer                                                 */
/* file : fichier de sortie                                                 */
/*                                                                          */
/* cree une table de hash qui contient les correspondances entres sig1 et   */
/* sig2                                                                     */
/* si on repere des signaux qui ne sont que dans une des ttvfig on le       */
/* signale                                                                  */
/*                                                                          */
/* Aug,10 2004, type comparison added                                       */
/*                                                                          */
/****************************************************************************/
ht *diff_htsig12(file,siglist,htsigname,ttvfig1,ttvfig2,type,mask)
FILE            *file;
chain_list      *siglist;
ht              *htsigname;
ttvfig_list     *ttvfig1;
ttvfig_list     *ttvfig2;
long            type;
long            mask;
{
  ht            *htsig;
  chain_list    *chainx;
  char          *ptname2;
  ttvsig_list   *ptsig1;
  ttvsig_list   *ptsig2;
  char          buf[1024];

  htsig = addht(HTSIZE);

  for (chainx= siglist; chainx; chainx= chainx->NEXT)
  {
    ptsig2      = chainx->DATA;
    ptname2     = namealloc(ttv_getsigname(ttvfig2, buf, ptsig2));
    ptsig1      = (ttvsig_list *)gethtitem(htsigname,ptname2);
    if ((long)ptsig1 != EMPTYHT)
      addhtitem(htsig, ptsig1, (long)ptsig2);
#if 0
    else if( /* (mask&DIFF_OPT_DTX) == DIFF_OPT_DTX && potentially necessary
              */
             (ptsig1 = ttv_getsigbyhash(ttvfig1,ptname2)) != NULL)
    {
      addhtitem(htsig, ptsig1, (long)ptsig2);
      if(siglist == SIGLIST1)
      {
        SIGLIST2 = addchain(SIGLIST2,ptsig1) ;
        addhtitem(HTSIG2NAME, ptname2, (long)ptsig1);
      }
      else
      {
        SIGLIST1 = addchain(SIGLIST1,ptsig1) ;
        addhtitem(HTSIG1NAME, ptname2, (long)ptsig1);
      }
    }
    else
      diff_printdiffsig(file,ptname2,ttvfig1,type,mask);
    
    if (ptsig1)
#else
    else
      diff_printdiffsig(file,ptname2,ttvfig1,type,mask);
    
    if ((long)ptsig1 != EMPTYHT)
#endif
      if ((ptsig1->TYPE&TTV_SIG_TYPE) != (ptsig2->TYPE&TTV_SIG_TYPE))
        diff_printdiffsigtype(file,ptname2,type);
        
  }
  return htsig;
}

/*}}}************************************************************************/
/*{{{                    diff_geteventtype()                                */
/* parametres :                                                             */
/* node : noeud                                                             */
/*                                                                          */
/* renvoit le type de l'evenement                                           */
/****************************************************************************/
char diff_geteventtype(node)
ttvevent_list   *node;
{
  char      nodetype ='X';

  if ( ((node->TYPE)&TTV_NODE_UP  ) == TTV_NODE_UP)
    nodetype = TTV_UP;
  if ( ((node->TYPE)&TTV_NODE_DOWN) == TTV_NODE_DOWN)
    nodetype = TTV_DOWN;
  return nodetype;
}

/*}}}************************************************************************/
/*{{{                    diff_printlinepath()                               */
/* parametres :                                                             */
/* file : fichier de sortie                                                 */
/* line : line source                                                       */
/* val1 : valeur source                                                     */
/* val2 : valeur trouvee                                                    */
/* valtype : valeur min ou max                                              */
/* miss : line miss, extra ou differente                                    */
/*                                                                          */
/* affiche les differences entre les lines ou l'absence de line             */
/* correspondantes selon le delta                                           */
/****************************************************************************/
void diff_printlinepath(file,ttvfig,pathorline,val1,val2,valtype,miss,type)
FILE            *file;
ttvfig_list     *ttvfig;
void            *pathorline;
long            val1;
long            val2;
char            *valtype;
long            miss;
long            type;
{
  long          diff, pos;
  long          diffpercent;
  char          sign;
  char          *ptnamein;
  char          *ptnameout;
  char          buf1[1024];
  char          buf2[1024];
  char          tein;
  char          teout;

  if ((miss&DIFF_LINE) == DIFF_LINE)
  {
    ttvline_list *pathorlinex = (ttvline_list*)pathorline;
    ptnameout   = ttv_getsigname(ttvfig, buf1,pathorlinex->ROOT->ROOT);
    ptnamein    = ttv_getsigname(ttvfig, buf2,pathorlinex->NODE->ROOT);
    teout       = diff_geteventtype(pathorlinex->ROOT);
    tein        = diff_geteventtype(pathorlinex->NODE);
  }
  else if ((miss&DIFF_PATH) == DIFF_PATH)
  {
    ttvpath_list *pathorlinex = (ttvpath_list*)pathorline;
    ptnameout   = ttv_getsigname(ttvfig, buf1,pathorlinex->ROOT->ROOT);
    ptnamein    = ttv_getsigname(ttvfig, buf2,pathorlinex->NODE->ROOT);
    teout       = diff_geteventtype(pathorlinex->ROOT);
    tein        = diff_geteventtype(pathorlinex->NODE);
  }
  else printf("error ");

  if ((miss&DIFF_CHANGE) == DIFF_CHANGE)
  {
    if (val2 == TTV_NOTIME || val2 == TTV_NOSLOPE)
      print(file,"change %s from %c %s:%ldps to %c %s:NOTIME\n",
            valtype,tein,ptnamein,val1/TTV_UNIT,teout,ptnameout);
    else if (val1 == TTV_NOTIME || val1 == TTV_NOSLOPE)
      print(file,"change %s from %c %s:NOTIME to %c %s:%ldps\n",
            valtype,tein,ptnamein,teout,ptnameout,val2/TTV_UNIT);
    else
    {
      diff      = val2-val1;
      sign      = (diff<0)?'-':'+';
      diff      = labs(diff);
      if (val1&&val2)
        diffpercent = labs((val1<val2)?(long)((float)diff*100.0/(float)val2):(long)((float)diff*100.0/(float)val1));
      else diffpercent = 100;
      if ( ( (diff >= DELTA*TTV_UNIT)
             && ((type&DIFF_OPT_DMASK) == DIFF_OPT_DELTA))
           || ( (diff >= DELTAD*TTV_UNIT) && (valtype[4] == 'd')
                && ((type&DIFF_OPT_DELTAD) == DIFF_OPT_DELTAD) )
           || ( (diff >= DELTAS*TTV_UNIT) && (valtype[4] == 's')
                && ((type&DIFF_OPT_DELTAS) == DIFF_OPT_DELTAS) )
           || ( (diffpercent >= DELTA*TTV_UNIT) &&
                ((type&DIFF_OPT_DMASKP) == DIFF_OPT_DELTAP))
           || ( (diffpercent >= DELTAD*TTV_UNIT) && (valtype[4] == 'd')
                && ((type&DIFF_OPT_DELTADP) == DIFF_OPT_DELTADP) )
           || ( (diffpercent >= DELTAS*TTV_UNIT) && (valtype[4] == 's')
                && ((type&DIFF_OPT_DELTASP) == DIFF_OPT_DELTASP) )
           || ( (valtype[4] == 'd')
                && ((type&(DIFF_OPT_DELTA|DIFF_OPT_DELTAD| DIFF_OPT_DELTAP|
                           DIFF_OPT_DELTADP)) == 0))
           || ( (valtype[4] == 's')
                && ((type&(DIFF_OPT_DELTA|DIFF_OPT_DELTAP|
                           DIFF_OPT_DELTAS|DIFF_OPT_DELTASP)) == 0)))
        {
          print(file,"change %s %c%.1fps ( %.1f -> %.1f : %c%ld%% ) from %c %s to %c %s\n",
                valtype,sign,diff/TTV_UNIT,val1/TTV_UNIT,val2/TTV_UNIT,sign,diffpercent,tein,ptnamein,teout,ptnameout);
          
          if (sign=='-')
            pos=499-(diffpercent/RANGE);
          else
            pos=501+(diffpercent/RANGE);

          if (pos<LOWLIM || pos>HIGHLIM) pos=0;
          if (strstr(valtype,"setup")!=NULL || strstr(valtype,"hold")!=NULL || strstr(valtype,"access")!=NULL)
            errcnt[TSHA][pos]++;
          else if (strstr(valtype,"slope")!=NULL)
            errcnt[TSLEW][pos]++;
          else if (strstr(valtype," rc")!=NULL)
            errcnt[TLINERC][pos]++;
          else if (strstr(valtype," gate")!=NULL)
            errcnt[TLINEGATE][pos]++;
          else if (strstr(valtype,"model")!=NULL)
            errcnt[TOTHERS][pos]++;
          else if (strstr(valtype,"delay")!=NULL) {
                  if (strstr(valtype,"gate")!=NULL)
                    errcnt[TLINEGATE][pos]++;
                  else
                    errcnt[TPATH][pos]++;
          }
          else
            errcnt[TOTHERS][pos]++;
        }
    }
  }
  else if ((miss&DIFF_MISS) == DIFF_MISS)
  {
    print(file,"missing %s from %c %s to %c %s\n",
          valtype,tein,ptnamein,teout,ptnameout);
    if (strstr(valtype," rc")!=NULL)
       errcntME[TMERC][TMEMISS]++;
    else
    {
      if (strstr(valtype,"setup")!=NULL || strstr(valtype,"hold")!=NULL || strstr(valtype,"access")!=NULL)
        errcntME[TMESHA][TMEMISS]++;
      else
        errcntME[TMELINE][TMEMISS]++;
    }
  }
  else if ((miss&DIFF_EXTRA) == DIFF_EXTRA)
  {
    print(file,"extra %s from %c %s to %c %s\n",
          valtype,tein,ptnamein,teout,ptnameout);
    if (strstr(valtype," rc")!=NULL)
       errcntME[TMERC][TMEEXTRA]++;
    else
    {
      if (strstr(valtype,"setup")!=NULL || strstr(valtype,"hold")!=NULL || strstr(valtype,"access")!=NULL)
        errcntME[TMESHA][TMEEXTRA]++;
      else
        errcntME[TMELINE][TMEEXTRA]++;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_getline2()                                    */
/* parametres :                                                             */
/* htsig12 : hashtable de correspondance des signaux                        */
/* line1 : line source                                                      */
/* numnode : numero de node ( 0 ou 1 )                                      */
/*                                                                          */
/* recupere la line correspondant a la line ttvfig1 dans la ttvfig2         */
/****************************************************************************/
ttvline_list *diff_getline2(htsig12,ttvfig1,ttvfig2,ttvins2,line1,numnode,mode)
ht           *htsig12;
ttvfig_list  *ttvfig1;
ttvfig_list  *ttvfig2;
ttvfig_list  *ttvins2;
ttvline_list *line1;
int          numnode;
char         mode;
{
  ttvline_list  *line2;
  ttvsig_list   *nsig2;
  ttvsig_list   *rsig2;
  ttvevent_list node2, *cmd, *cmd2;
  char          tein1;
  char          teout1;
  char          tein2;
  char          teout2;
  long          type = TTV_LINE_A|TTV_LINE_U|TTV_LINE_O|
    TTV_LINE_S|TTV_LINE_R|TTV_LINE_RC|TTV_LINE_PR|
    TTV_LINE_EV|TTV_LINE_HZ;

  tein1  = diff_geteventtype(line1->NODE);
  teout1 = diff_geteventtype(line1->ROOT);
  rsig2  = (ttvsig_list *)gethtitem(htsig12,line1->ROOT->ROOT);
  node2  = rsig2->NODE[numnode];
  nsig2  = (ttvsig_list *)gethtitem(htsig12,line1->NODE->ROOT);

  cmd=ttv_getlinecmd(ttvfig1, line1, TTV_LINE_CMDMAX);
  cmd2=NULL;
  if (cmd!=NULL)
  {
     int dir=0;
     ttvsig_list *tvs;
     if (cmd->TYPE & TTV_NODE_UP) dir=1;
     tvs=(ttvsig_list *)gethtitem(htsig12,cmd->ROOT);
     if ((long)tvs!=EMPTYHT)
        cmd2=&tvs->NODE[dir];
  }
          
  if(mode == 'd'){
    ttv_expfigsig(ttvfig2,rsig2,0,ttvfig2->INFO->LEVEL,TTV_STS_DTX, TTV_FILE_DTX);
    if ((long)nsig2 != EMPTYHT)
      ttv_expfigsig(ttvfig2,nsig2,0,ttvfig2->INFO->LEVEL,TTV_STS_DTX, TTV_FILE_DTX);
  }else{
    ttv_expfigsig(ttvfig2,rsig2,0,ttvfig2->INFO->LEVEL,TTV_STS_TTX, TTV_FILE_TTX);
    if ((long)nsig2 != EMPTYHT)
      ttv_expfigsig(ttvfig2,nsig2,0,ttvfig2->INFO->LEVEL,TTV_STS_TTX, TTV_FILE_TTX);
  }

  for (line2=((mode=='d')?node2.INLINE:node2.INPATH);line2;line2=line2->NEXT)
  {
    if(ttvins2 != NULL)
      if((line2->FIG != ttvins2) &&
         (ttvfig1->INSNAME == ttvfig2->INSNAME))
        continue ;
    if (line2->FIG->INFO->LEVEL!=line1->FIG->INFO->LEVEL) continue;
    tein2  = diff_geteventtype(line2->NODE);
    teout2 = diff_geteventtype(line2->ROOT);
    cmd=ttv_getlinecmd(ttvfig2, line2, TTV_LINE_CMDMAX);

    if ( (tein1 == tein2)&&(teout1 == teout2)&&
         (line2->NODE->ROOT == nsig2)&&
         ( ((line1->TYPE)&type) == ((line2->TYPE)&type)) &&
         cmd==cmd2
         )
    {
      return line2;
    }
  }
  return NULL;
}

/*}}}************************************************************************/
/*{{{                    diff_printdiffmodeltime()                          */
/* parametres :                                                             */
/*                                                                          */
/* print les diff de modele                                                 */
/****************************************************************************/
void diff_printdiffmodeltime(file,daccess1,daccess2,saccess1,saccess2,slew1
                             ,slew2,type)
FILE    *file;
long    daccess1;
long    daccess2;
long    saccess1;
long    saccess2;
long    slew1;
long    slew2;
char    type;
{
  long diff;
  char sign;
  long diffpercent, pos;
  int cas;
  

  print(file,"change model ");

  switch (type)
  {
    case 'A' :
         fprintf(file,"access ");
         cas=TSHA;
         break;
    case 'O' :
         fprintf(file,"hold ");
         cas=TSHA;
         break;
    case 'S' :
         fprintf(file,"set-up ");
         cas=TSHA;
         break;
    default :
         fprintf(file,"combi "); 
         cas=TCOMBI;
         break;
  }
  if (daccess1!=daccess2)
  {
    diff = daccess2-daccess1;
    sign = (diff<0.0)?'-':'+';
    diff = labs(diff);
    diffpercent = labs((daccess1<daccess2) ? (long)((float)diff*100.0/(float)daccess2)
                       : (long)((float)diff*100.0/(float)daccess1));
    fprintf(file,"delay %c%ldps (%c%ld%%)\n",sign,diff,sign,diffpercent);
    if (sign=='-')
      pos=499-(diffpercent/RANGE);
    else
      pos=501+(diffpercent/RANGE);
    if (pos<LOWLIM || pos>HIGHLIM) pos=0;
    errcnt[cas][pos]++;
  }
  else if (saccess2!=saccess1)
  {
    diff = saccess2-saccess1;
    sign = (diff<0.0)?'-':'+';
    diff = labs(diff);
    diffpercent = labs((saccess1<saccess2) ? (long)((float)diff*100.0/(float)saccess2)
                       : (long)((float)diff*100.0/(float)saccess1));
    fprintf(file,"slew %c%ldps (%c%ld%%)\n",sign,diff,sign,diffpercent);
    if (sign=='-')
      pos=499-(diffpercent/RANGE);
    else
      pos=501+(diffpercent/RANGE);
    if (pos<LOWLIM || pos>HIGHLIM) pos=0;
    errcnt[TSLEW][pos]++;
  }
  else if (slew2!=slew1)
  {
    diff = slew2-slew1;
    sign = (diff<0.0)?'-':'+';
    diff = labs(diff);
    diffpercent = labs((slew1<slew2)?(long)((float)diff*100.0/(float)slew2):(long)((float)diff*100.0/(float)slew1));
    fprintf(file,"slew %c%ldps (%c%ld%%)\n",sign,diff,sign,diffpercent);
    if (sign=='-')
      pos=499-(diffpercent/RANGE);
    else
      pos=501+(diffpercent/RANGE);
    if (pos<LOWLIM || pos>HIGHLIM) pos=0;
    errcnt[TSLEW][pos]++;
  }

}

/*}}}************************************************************************/
/*{{{                    diff_lookmodelaccess()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void diff_lookmodelaccess(FILE *file, int modkf, int modc,
                                 float modkft[DIFF_MAX_MODNB],
                                 float  modct[DIFF_MAX_MODNB],
                                 timing_model *md1, timing_model *md2,
                                 ttvfig_list  *fig1, ttvfig_list  *fig2,
                                 char type, int mode)
{
  int        j, k;
  float      capa1, slope1, capa2, slope2;
  long       daccess1,daccess2,saccess1,saccess2;
  char       tmodel;

  if ((tmodel = md1->UTYPE) != md2->UTYPE)
    print(file,"model difference between on %s\n",md1->NAME);

  for ( j = 0; j < ((modkf) ? modkf : 1); j ++)
  {
    for (k = 0; k < ((modc) ? modc : 1); k ++)
    {
      capa1    = (modc)  ?  modct[k] : fig1->INFO->CAPAOUT;
      slope1   = (modkf) ? modkft[j] : fig1->INFO->SLOPE;
      capa2    = (modc)  ?  modct[k] : fig2->INFO->CAPAOUT;
      slope2   = (modkf) ? modkft[j] : fig2->INFO->SLOPE;

	  // delay difference
      daccess1 = stm_mod_delay(md1,capa1,slope1, NULL, "unknown");
      daccess2 = stm_mod_delay(md2,capa2,slope2, NULL, "unknown");
      if (daccess1 != daccess2 && (mode < 2 || tmodel != STM_MOD_MODTBL))
        diff_printdiffmodeltime(file,daccess1,daccess2,0,0,0,0,type);
	  
	  // slew difference
      saccess1 = stm_mod_slew (md1,capa1,slope1, NULL, NULL, "unknown");
      saccess2 = stm_mod_slew (md2,capa2,slope2, NULL, NULL, "unknown");
      if (saccess1 != saccess2 && (mode > 2 || tmodel != STM_MOD_MODTBL))
        diff_printdiffmodeltime(file,0,0,saccess1,saccess2,0,0,type);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_lookmodelconstraint()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void diff_lookmodelconstraint(FILE *file, int modk, int modf,
                                     float modkt[DIFF_MAX_MODNB],
                                     float modft[DIFF_MAX_MODNB],
                                     timing_model *md1, timing_model *md2,
                                     ttvfig_list  *fig1, ttvfig_list  *fig2,
                                     char type)
{
  int        j, k;
  float      slope11, slope12, slope21, slope22;
  long       slew1,slew2;

  for (j = 0; j < ((modf) ? modf : 1) ; j ++)
  {
    for (k = 0; k < ((modk) ? modk : 1) ; k ++)
    {
      slope11  = (modf) ? modft[k] : fig1->INFO->SLOPE;
      slope12  = (modk) ? modkt[k] : fig1->INFO->SLOPE;
      slope21  = (modf) ? modft[k] : fig2->INFO->SLOPE;
      slope22  = (modk) ? modkt[k] : fig2->INFO->SLOPE;

      slew1    = stm_mod_constraint(md1,slope11,slope12);
      slew2    = stm_mod_constraint(md2,slope21,slope22);
      if (slew1!=slew2)
        diff_printdiffmodeltime(file,0,0,0,0,slew1,slew2,type);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_lookmodel()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void diff_lookmodel(file,ttvfig1,md_1,ttvfig2,md_2,mask,path)
FILE            *file;
ttvfig_list     *ttvfig1;
void            *md_1;
ttvfig_list     *ttvfig2;
void            *md_2;
long            mask;
long            path;
{
  timing_model  *md1, *md2;
  ttvline_list  *line1, *line2;
  int           i;
  char          type;

  line1 = (ttvline_list*)md_1;
  line2 = (ttvline_list*)md_2;

  for (i = 0; i < 4; i ++)
  {
    if (path == DIFF_PATH)
    {
      switch (i)
      {
        case 0 :
             md1 = ((ttvpath_list*)md_1)->MD;
             md2 = ((ttvpath_list*)md_2)->MD;
             break;
        case 1 :
             md1 = ((ttvpath_list*)md_1)->MF;
             md2 = ((ttvpath_list*)md_2)->MF;
             i  += 2;
             break;
        default :
             break;
      }
    }
    else
      switch (i)
      {
        case 0 :
             md1 = stm_getmodel(ttvfig1->INFO->FIGNAME,line1->MDMAX);
             md2 = stm_getmodel(ttvfig2->INFO->FIGNAME,line2->MDMAX);
             break;
        case 1 :
             md1 = stm_getmodel(ttvfig1->INFO->FIGNAME,line1->MDMIN);
             md2 = stm_getmodel(ttvfig2->INFO->FIGNAME,line2->MDMIN);
             break;
        case 2 :
             md1 = stm_getmodel(ttvfig1->INFO->FIGNAME,line1->MFMAX);
             md2 = stm_getmodel(ttvfig2->INFO->FIGNAME,line2->MFMAX);
             break;
        default :
             md1 = stm_getmodel(ttvfig1->INFO->FIGNAME,line1->MFMIN);
             md2 = stm_getmodel(ttvfig2->INFO->FIGNAME,line2->MFMIN);
             break;
      }
    if ((mask&DIFF_OPT_MODF) == DIFF_OPT_MODF)
    {
      if ((mask&DIFF_OPT_MODC) == DIFF_OPT_MODC)
      {
        if ((mask&DIFF_OPT_MODK) == DIFF_OPT_MODK)
        { /* f c k defined */
          if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
            diff_lookmodelaccess(file,NBMODK,NBMODC,MODK,MODC,md1,md2,0,0,type,i);
          else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                    || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
            diff_lookmodelconstraint(file,NBMODK,NBMODF,MODK,MODF,md1,md2,0,0,type);
          else
            diff_lookmodelaccess(file,NBMODF,NBMODC,MODF,MODC,md1,md2,0,0,'U',i);
        }
        else
        { /* f c not k defined */
          if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
            diff_lookmodelaccess(file,0,NBMODC,0,MODC,md1,md2,ttvfig1,ttvfig2,type,i);
          else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                    || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
            diff_lookmodelconstraint(file,0,NBMODF,0,MODF,md1,md2,ttvfig1,ttvfig2,type);
          else
            diff_lookmodelaccess(file,NBMODF,NBMODC,MODF,MODC,md1,md2,0,0,'U',i);
        }
      }
      else if ((mask&DIFF_OPT_MODK) == DIFF_OPT_MODK)
      { /* f not c k defined */
        if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
          diff_lookmodelaccess(file,NBMODK,0,MODK,0,md1,md2,ttvfig1,ttvfig2,type,i);
        else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                  || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
          diff_lookmodelconstraint(file,NBMODK,NBMODF,MODK,MODF,md1,md2,0,0,type);
        else
          diff_lookmodelaccess(file,0,NBMODC,0,MODC,md1,md2,ttvfig1,ttvfig2,'U',i);
      }
      else
      { /* f not c not k defined */
        if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
          diff_lookmodelaccess(file,0,0,0,0,md1,md2,ttvfig1,ttvfig2,type,i);
        else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                  || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
          diff_lookmodelconstraint(file,0,NBMODF,0,MODF,md1,md2,ttvfig1,ttvfig2,type);
        else
          diff_lookmodelaccess(file,NBMODK,0,MODK,0,md1,md2,ttvfig1,ttvfig2,'U',i);
      }
    }
    else if ((mask&DIFF_OPT_MODC) == DIFF_OPT_MODC)
    {
      if ((mask&DIFF_OPT_MODK) == DIFF_OPT_MODK)
      { /* not f c k defined */
        if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
          diff_lookmodelaccess(file,NBMODK,NBMODC,MODK,MODC,md1,md2,0,0,type,i);
        else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                  || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
          diff_lookmodelconstraint(file,NBMODK,0,MODK,0,md1,md2,ttvfig1,ttvfig2,type);
        else
          diff_lookmodelaccess(file,0,NBMODC,0,MODC,md1,md2,ttvfig1,ttvfig2,'U',i);
      }
      else
      { /* not f c not k defined */
        if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
          diff_lookmodelaccess(file,0,NBMODC,0,MODC,md1,md2,ttvfig1,ttvfig2,type,i);
        else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                  || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
          diff_lookmodelconstraint(file,0,NBMODC,0,MODC,md1,md2,ttvfig1,ttvfig2,type);
        else
          diff_lookmodelaccess(file,0,NBMODC,0,MODC,md1,md2,ttvfig1,ttvfig2,'U',i);
      }
    }
    else if ((mask&DIFF_OPT_MODK) == DIFF_OPT_MODK)
    { /* not f not c k defined */
      if ((type=(line1->TYPE) == 'A')&&(line2->TYPE == 'A'))
        diff_lookmodelaccess(file,NBMODK,0,MODK,0,md1,md2,ttvfig1,ttvfig2,type,i);
      else if ( ((type=(line1->TYPE) == 'O')&&(line2->TYPE == 'O'))
                || ((type=(line1->TYPE) == 'S')&&(line2->TYPE == 'S')) )
        diff_lookmodelconstraint(file,NBMODK,0,MODK,0,md1,md2,ttvfig1,ttvfig2,type);
      else
        diff_lookmodelaccess(file,0,0,0,0,md1,md2,ttvfig1,ttvfig2,'U',i);
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_lookline()                                    */
/* parametres :                                                             */
/* siglist1 : liste des signaux                                             */
/* htsig12 : hashtable des correspondance des pointeurs de signaux          */
/* file : fichier de sortie                                                 */
/* what : type de recherche extra ou miss                                   */
/*                                                                          */
/* cherche les differences entre deux ttvfig au niveau des lines            */
/****************************************************************************/
void diff_lookline(file,ttvfig1,ttvfig2,ttvins1,ttvins2,siglist1,
                   htsig12,what,mask,type)
FILE        *file;
ttvfig_list *ttvfig1;
ttvfig_list *ttvfig2;
ttvfig_list *ttvins1;
ttvfig_list *ttvins2;
chain_list  *siglist1;
ht          *htsig12;
short       what;
long        mask;
char        type;

{
  chain_list    *chainx;
  ttvsig_list   *ptsig1, *ptsig2;
  ttvevent_list  node1;
  ttvline_list  *line1, *line2;
  int            i;
  long           delaymax1, delaymax2, delaymin1, delaymin2;
  long           slopemax1, slopemax2, slopemin1, slopemin2;
  char           delaytype[16], slopetype[16], extratype[16], modeltype[16];

  for (chainx= siglist1; chainx; chainx= chainx->NEXT)
  {
    ptsig1 = chainx->DATA;
    if(type == 'd'){
      ttv_expfigsig(ttvfig1,ptsig1,0,ttvfig1->INFO->LEVEL,TTV_STS_DTX, TTV_FILE_DTX);
    }else{
      ttv_expfigsig(ttvfig1,ptsig1,0,ttvfig1->INFO->LEVEL,TTV_STS_TTX, TTV_FILE_TTX);
    }
    ptsig2 = (ttvsig_list *)gethtitem(htsig12,ptsig1);
    if ((long)ptsig2 != EMPTYHT)
    {
      if(type == 'd'){
        ttv_expfigsig(ttvfig2,ptsig2,0,ttvfig2->INFO->LEVEL,TTV_STS_DTX, TTV_FILE_DTX);
      }else{
        ttv_expfigsig(ttvfig2,ptsig2,0,ttvfig2->INFO->LEVEL,TTV_STS_TTX, TTV_FILE_TTX);
      }
      for (i = 0; i < 2; i ++)
      {
        node1 = ptsig1->NODE[i];
        for (line1 = ((type=='d')?node1.INLINE:node1.INPATH); line1; line1 = line1->NEXT)
        {
          /* mode mask */
          if ( (ttv_testmask(ttvfig1,line1->NODE->ROOT,TTV_NAME_IN) == 0)
               && (TTV_NAME_IN != NULL))
            continue ;
          /* mode hierarchique */
          if (ttvins1 != NULL)
            if ( ( (line1->FIG != ttvins1)
                   && (ttvfig1->INSNAME == ttvfig2->INSNAME))
                 || (gethtitem(htsig12,line1->ROOT->ROOT) == EMPTYHT)
                 || (gethtitem(htsig12,line1->NODE->ROOT) == EMPTYHT))
              continue ;
          /* mode hierarchique et dtx */
          //if (mask&(DIFF_OPT_HIER|DIFF_OPT_DTX)) == (DIFF_OPT_HIER|DIFF_OPT_DTX))

          if ((line2 = diff_getline2(htsig12,ttvfig1,ttvfig2,ttvins2,line1,i,type))!=NULL)
          {
            /* test si l'on est en ignore change ou en mode miss */
            if ( (what == DIFF_MISS) && ((mask&DIFF_OPT_ICHANGE) == 0) )
            {
              /* diff de delais */
              if ((mask&DIFF_OPT_NDELAY) == 0)
              {
                delaymax1 = line1->VALMAX;
                delaymax2 = line2->VALMAX;
                delaymin1 = line1->VALMIN;
                delaymin2 = line2->VALMIN;

                if (delaymax1!=delaymax2)
                {
                  if ((line1->TYPE & TTV_LINE_RC)==TTV_LINE_RC)
                    strcpy(delaytype,"max_delay rc");
                  else if ((line1->TYPE & TTV_LINE_U)==TTV_LINE_U)
                    strcpy(delaytype,"max_delay setup");
                  else if ((line1->TYPE & TTV_LINE_O)==TTV_LINE_O)
                    strcpy(delaytype,"max_delay hold");
                  else if ((line1->TYPE & TTV_LINE_A)==TTV_LINE_A)
                    strcpy(delaytype,"max_delay access");
                  else
                    strcpy(delaytype,"max_delay gate");
                  diff_printlinepath(file,ttvfig1,line1,delaymax1,delaymax2,
                                     delaytype,DIFF_CHANGE|DIFF_LINE,mask);
                }
                if (delaymin1!=delaymin2)
                {
                  if ((line1->TYPE & TTV_LINE_RC)==TTV_LINE_RC)
                    strcpy(delaytype,"min_delay rc");
                  else if ((line1->TYPE & TTV_LINE_U)==TTV_LINE_U)
                    strcpy(delaytype,"min_delay setup");
                  else if ((line1->TYPE & TTV_LINE_O)==TTV_LINE_O)
                    strcpy(delaytype,"min_delay hold");
                  else if ((line1->TYPE & TTV_LINE_A)==TTV_LINE_A)
                    strcpy(delaytype,"min_delay access");
                  else
                    strcpy(delaytype,"min_delay gate");
                  diff_printlinepath(file,ttvfig1,line1,delaymin1,delaymin2,
                                     delaytype,DIFF_CHANGE|DIFF_LINE,mask);
                }
                /* diff de modele de delais */
                if ((mask&DIFF_OPT_MODEL) == DIFF_OPT_MODEL)
                {
                  if (line1->MDMAX)
                    if (strcmp(line1->MDMAX,line2->MDMAX))
                    {
                      strcpy(modeltype,"delay model max");
                      diff_printdiffmodel(file,line1->MDMAX,ttvfig1,
                                          line2->MDMAX,ttvfig2, modeltype);
                    }
                  if (line1->MDMIN)
                    if (strcmp(line1->MDMIN,line2->MDMIN))
                    {
                      strcpy(modeltype,"delay model min");
                      diff_printdiffmodel(file,line1->MDMIN,ttvfig1,
                                          line2->MDMIN,ttvfig2, modeltype);
                    }
                  /* diff de modele  */
                  diff_lookmodel(file,ttvfig1,line1,ttvfig2,line2,mask,DIFF_LINE);
                }
              }
              /* diff de front */
              if ((mask&DIFF_OPT_NSLOPE) == 0)
              {
                slopemax1 = line1->FMAX;
                slopemax2 = line2->FMAX;
                slopemin1 = line1->FMIN;
                slopemin2 = line2->FMIN;

                if (slopemax1!=slopemax2)
                {
                  if ((line1->TYPE & TTV_LINE_RC)==TTV_LINE_RC)
                    strcpy(slopetype,"max_slope rc");
                  else if ((line1->TYPE & TTV_LINE_A)==TTV_LINE_A)
                    strcpy(slopetype,"max_slope access");
                  else
                    strcpy(slopetype,"max_slope gate");
                  diff_printlinepath(file,ttvfig1,line1,slopemax1,slopemax2,
                                     slopetype,DIFF_CHANGE|DIFF_LINE,mask);
                }
                if  (slopemin1!=slopemin2)
                {
                  if ((line1->TYPE & TTV_LINE_RC)==TTV_LINE_RC)
                    strcpy(slopetype,"min_slope rc");
                  else if ((line1->TYPE & TTV_LINE_A)==TTV_LINE_A)
                    strcpy(slopetype,"min_slope access");
                  else
                    strcpy(slopetype,"min_slope gate");
                  diff_printlinepath(file,ttvfig1,line1,slopemin1,slopemin2,
                                     slopetype,DIFF_CHANGE|DIFF_LINE,mask);
                }
                /* diff de modele de front */
                if ((mask&DIFF_OPT_MODEL) == DIFF_OPT_MODEL)
                {
                  if (line1->MFMAX)
                    if (strcmp(line1->MFMAX,line2->MFMAX))
                    {
                      strcpy(modeltype,"front model max");
                      diff_printdiffmodel(file,line1->MFMAX,ttvfig1,
                                          line2->MFMAX,ttvfig2, modeltype);
                    }
                  if (line1->MFMIN)
                    if (strcmp(line1->MFMIN,line2->MFMIN))
                    {
                      strcpy(modeltype,"front model min");
                      diff_printdiffmodel(file,line1->MFMIN,ttvfig1,
                                          line2->MFMIN,ttvfig2, modeltype);
                    }
                  /* diff de modele  */
                  diff_lookmodel(file,ttvfig1,line1,ttvfig2,line2,mask,DIFF_LINE);
                }
              }
            }
          }
          /* miss ou extra detecte */
          else if ( ( ( !(mask & DIFF_OPT_IMISS))&&(what == DIFF_MISS))
                    || ( ( !(mask & DIFF_OPT_IEXTRA))&&(what == DIFF_EXTRA))
                  )
          {
            if ((line1->TYPE & TTV_LINE_RC)==TTV_LINE_RC)
              strcpy(extratype,"line rc");
            else if ((line1->TYPE & TTV_LINE_U)==TTV_LINE_U)
              strcpy(extratype,"line gate setup");
            else if ((line1->TYPE & TTV_LINE_O)==TTV_LINE_O)
              strcpy(extratype,"line gate hold");
            else if ((line1->TYPE & TTV_LINE_A)==TTV_LINE_A)
              strcpy(extratype,"line gate access");
            else
              strcpy(extratype,"line gate");
            diff_printlinepath(file,ttvfig1,line1,0,0,extratype,what|DIFF_LINE,0);
          }
        }
      }
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_getpath2()                                    */
/* parametres :                                                             */
/* htsig12 : hashtable de correspondances des pointeurs de signaux          */
/* path1 : chemin source                                                    */
/* type : type de chemin min ou max                                         */
/*                                                                          */
/* recupere le path correspondant au path de ttvfig1 dans la ttvfig2        */
/****************************************************************************/
ttvpath_list *diff_getpath2(htsig12,path1,path2,type)
ht              *htsig12;
ttvpath_list    *path1;
ttvpath_list    *path2;
long            type;
{
  ttvsig_list  *nsig2, *rsig2, *cmd2;
  char          tein1;
  char          teout1;
  char          tein2;
  char          teout2;
  ttvpath_list *path;

  type = 0; // added to prevent warning, unused

  tein1     = diff_geteventtype(path1->NODE);
  teout1    = diff_geteventtype(path1->ROOT);
  rsig2     = (ttvsig_list *)gethtitem(htsig12,path1->ROOT->ROOT);
  nsig2     = (ttvsig_list *)gethtitem(htsig12,path1->NODE->ROOT);
  if (path1->CMD)
    cmd2    = (ttvsig_list *)gethtitem(htsig12,path1->CMD->ROOT);
  else
    cmd2    = NULL;

  for (path = path2; path; path = path->NEXT)
  {
    tein2  = diff_geteventtype(path->NODE);
    teout2 = diff_geteventtype(path->ROOT);
    if (path->NODE->ROOT == nsig2 &&
        path->ROOT->ROOT == rsig2 &&
        tein1 == tein2 &&
        teout1 == teout2 &&
        (path->TYPE&TTV_FIND_HZ) == (path1->TYPE&TTV_FIND_HZ) &&
        ((path->CMD && path->CMD->ROOT == cmd2) || !(path->CMD || cmd2)))
      return path;
  }

  return NULL;
}

/*}}}************************************************************************/
/*{{{                    ttv_testValidPath()                                */
/*                                                                          */
/* test si on doit traiter le signal                                        */
/****************************************************************************/
int ttv_testValidSig(sig)
ttvsig_list     *sig;
{
  long       mask;

  mask = TTV_SIG_C|TTV_SIG_Q|TTV_SIG_L|TTV_SIG_R|TTV_SIG_B;

  return (sig->TYPE&mask);
}

/*}}}************************************************************************/
/*{{{                    diff_lookpath()                                    */
/* parametres :                                                             */
/* siglist1 : liste des signaux                                             */
/* htsig12 : hashtable des correspondance des pointeurs de signaux          */
/* file : fichier de sortie                                                 */
/* type : type de chemin min ou max                                         */
/* what : type de recherche extra ou miss                                   */
/*                                                                          */
/* cherche les differences entre deux ttvfig au niveau des paths            */
/****************************************************************************/
void diff_lookpath(file,ttvfig1,ttvfig2,ttvins1,ttvins2,siglist1,htsig12,
                   type,what,mask)
FILE        *file;
ttvfig_list *ttvfig1 ;
ttvfig_list *ttvfig2;
ttvfig_list *ttvins1;
ttvfig_list *ttvins2;
chain_list  *siglist1;
ht          *htsig12;
long        type;
short       what;
long        mask;
{
  chain_list    *chainx;
  ttvsig_list   *ptsig1;
  ttvsig_list   *ptsig2;
  ttvpath_list  *path1;
  ttvpath_list  *path2;
  ttvpath_list  *path2diff;
  char          delaytype[16];
  char          slopetype[16];
  char          extratype[16];
  long          delay1;
  long          delay2;
  long          slope2;
  long          slope1;

  if (type == TTV_FIND_MIN)
    strcpy(delaytype,"min");
  else if (type == TTV_FIND_MAX)
    strcpy(delaytype,"max");
  strcpy(extratype,delaytype);
  strcpy(slopetype,delaytype);
  strcat(delaytype,"_delay");
  strcat(slopetype,"_slope");
  strcat(extratype,"_path");

  if ((mask&DIFF_OPT_MOD)!=0) type = type| TTV_FIND_CARAC;

  for (chainx= siglist1; chainx; chainx= chainx->NEXT)
  {
    ptsig1 = chainx->DATA;
    if (ttv_testValidSig(ptsig1))
    {
      path1 = ttv_getpath(ttvfig1,ttvins1,ptsig1,TTV_NAME_IN,
                          TTV_SEUIL_MAX,TTV_SEUIL_MIN,type);
      ptsig2 = (ttvsig_list *)gethtitem(htsig12,ptsig1);
      if ((long)ptsig2 != EMPTYHT)
      {
        path2 = ttv_getpath(ttvfig2,ttvins2,ptsig2,TTV_NAME_IN,
                            TTV_SEUIL_MAX,TTV_SEUIL_MIN,type);
        for (;path1;path1=path1->NEXT)
        {
          if ((path2diff = diff_getpath2(htsig12,path1,path2,type))!=NULL)
          {
            if ((what == DIFF_MISS)&&((mask&DIFF_OPT_ICHANGE) == 0))
            {
              if ((mask&DIFF_OPT_NDELAY) == 0)
              {
                delay1 = path1->DELAY;
                delay2 = path2diff->DELAY;
                if (delay1!=delay2)
                  diff_printlinepath(file,ttvfig1,path1,delay1,delay2,delaytype,
                                     DIFF_CHANGE|DIFF_PATH,mask);
              }
              if ((mask&DIFF_OPT_NSLOPE) == 0)
              {
                slope1 = path1->SLOPE;
                slope2 = path2diff->SLOPE;
                if (slope1!=slope2)
                  diff_printlinepath(file,ttvfig1,path1,slope1,slope2,slopetype,
                                     DIFF_CHANGE|DIFF_PATH,mask);
              }
              if ((mask&DIFF_OPT_MOD)!=0)
                diff_lookmodel(file,ttvfig1,path1,ttvfig2,path2diff,mask,DIFF_PATH);
            }
          }
          else if ( (((mask&DIFF_OPT_IMISS) == 0)&&(what == DIFF_MISS)) ||
                    (((mask&DIFF_OPT_IEXTRA) == 0)&&(what == DIFF_EXTRA)) )
            diff_printlinepath(file,ttvfig1,path1,0,0,extratype,what|DIFF_PATH,0);
        }
        ttv_freepathlist(path2);
        ttv_freepathlist(path1);
      }
    }
  }
}

/*}}}************************************************************************/
/*{{{                    diff_ttvfigdiff()                                  */
/* parametres :                                                             */
/* ttvfig1 : fig source                                                     */
/* ttvfig2 : fig a comparer                                                 */
/* type :                                                                   */
/* file : fichier de sortie                                                 */
/*                                                                          */
/* compare deux ttvfig et ecrit un rapport                                  */
/****************************************************************************/
void diff_ttvfigdiff(file,ttvfig1,ttvfig2,type)
FILE        *file;
ttvfig_list *ttvfig1;
ttvfig_list *ttvfig2;
long        type;
{
  char      head[1024];
  ttvfig_list     *ttvins1 = NULL ;
  ttvfig_list     *ttvins2 = NULL ;

  if ((type&DIFF_OPT_DTX) == DIFF_OPT_DTX)
  {
    ttv_parsttvfig(ttvfig1,TTV_STS_DTX,TTV_FILE_DTX) ;
    ttv_parsttvfig(ttvfig2,TTV_STS_DTX,TTV_FILE_DTX) ;
  }
  else
  {
    ttv_parsttvfig(ttvfig1,TTV_STS_TTX,TTV_FILE_TTX) ;
    ttv_parsttvfig(ttvfig2,TTV_STS_TTX,TTV_FILE_TTX) ;
  }

  if((type & DIFF_OPT_HIER) != DIFF_OPT_HIER)
  {
    ttvins1 = ttvfig1 ;
    ttvins2 = ttvfig2 ;
  }
  else
  {
    ttvins1 = NULL ;
    ttvins2 = NULL ;
  }

  SIGLIST1 = ttv_getsigbytype(ttvfig1, ttvins1, TTV_SIG_TYPE, TTV_NAME_OUT);
  SIGLIST2 = ttv_getsigbytype(ttvfig2, ttvins2, TTV_SIG_TYPE, TTV_NAME_OUT);

  strcpy(head,"***** comparison of ");
  strcat(head,ttvfig1->INFO->FIGNAME);
  strcat(head," and ");
  strcat(head,ttvfig2->INFO->FIGNAME);

  if ((type & DIFF_OPT_INFO) == DIFF_OPT_INFO)
  {
    fprintf(file,"%s info level *****\n", head);
    diff_lookinfodiff(file,ttvfig1->INFO,ttvfig2->INFO);
    fprintf(file,"\n");
  }

  if ((type&DIFF_OPT_NSIG) == 0)
    fprintf(file,"%s signal level *****\n", head);
  HTSIG1NAME = diff_htloadnamesig(ttvfig1,SIGLIST1);
  HTSIG2NAME = diff_htloadnamesig(ttvfig2,SIGLIST2);
  HTSIG12 = diff_htsig12(file,SIGLIST2,HTSIG1NAME,ttvfig1,ttvfig2,
                         0,type);
  HTSIG21 = diff_htsig12(file,SIGLIST1,HTSIG2NAME,ttvfig2,ttvfig1,
                         DIFF_EXTRA,type);
  delht(HTSIG12) ;
  HTSIG12 = diff_htsig12(file,SIGLIST2,HTSIG1NAME,ttvfig1,ttvfig2,
                         DIFF_MISS,type);
  delht(HTSIG1NAME);
  delht(HTSIG2NAME);

  if ((type&DIFF_OPT_NSIG) == 0)
    fprintf(file,"\n");

  if ((type & DIFF_OPT_NETNAME) == DIFF_OPT_NETNAME)
  {
    fprintf(file,"%s netname level *****\n", head);
    diff_signetname(file,ttvfig1,ttvfig2,SIGLIST1,HTSIG12);
    fprintf(file,"\n");
  }

  if ((type & DIFF_OPT_NCAPA) == 0)
  {
    fprintf(file,"%s capacitance level *****\n", head);
    diff_sigcapa(file,ttvfig1,SIGLIST1,HTSIG12,type);
    fprintf(file,"\n");
  }

  if ((type & DIFF_OPT_DTX) == DIFF_OPT_DTX)
  {
    fprintf(file,"%s line level *****\n", head);
    diff_lookline(file,ttvfig1,ttvfig2,ttvins1,ttvins2,SIGLIST1,
                  HTSIG12,DIFF_MISS,type,'d');
    diff_lookline(file,ttvfig2,ttvfig1,ttvins2,ttvins1,SIGLIST2,
                  HTSIG21,DIFF_EXTRA,type,'d');
    fprintf(file,"\n");
  }

  if ((type & DIFF_OPT_NTTX) == 0)
  {
    fprintf(file,"%s path level *****\n", head);
    diff_lookpath(file,ttvfig1,ttvfig2,ttvins1,ttvins2,SIGLIST1,
                  HTSIG12,TTV_FIND_MAX,DIFF_MISS,type);
    diff_lookpath(file,ttvfig1,ttvfig2,ttvins1,ttvins2,SIGLIST1,
                  HTSIG12,TTV_FIND_MIN,DIFF_MISS,type);
    diff_lookpath(file,ttvfig2,ttvfig1,ttvins2,ttvins1,SIGLIST2,
                  HTSIG21,TTV_FIND_MAX,DIFF_EXTRA,type);
    diff_lookpath(file,ttvfig2,ttvfig1,ttvins2,ttvins1,SIGLIST2,
                  HTSIG21,TTV_FIND_MIN,DIFF_EXTRA,type);
    diff_lookline(file,ttvfig1,ttvfig2,ttvins1,ttvins2,SIGLIST1,
                  HTSIG12,DIFF_MISS,type,'t');
    diff_lookline(file,ttvfig2,ttvfig1,ttvins2,ttvins1,SIGLIST2,
                  HTSIG21,DIFF_EXTRA,type,'t');
    fprintf(file,"\n");
  }

  freechain(SIGLIST1);
  freechain(SIGLIST2);
  delht(HTSIG12);
  delht(HTSIG21);
}

void init_disp_report()
{
  int i, j;
  for (j=0; j<TLAST; j++)
    for (i=0;i<1000;i++) errcnt[j][i]=0;
}

void disp(FILE *f, char *section, int err)
{
  unsigned int cnt=0, i;
  for (i=0; i<sizeof(HH)/sizeof(*HH); i++)
    if (HH[i].num>=0) cnt+=errcnt[HH[i].num][err];

  if (cnt>0)
    {
      if (HH[0].num<0)
        fprintf(f, "%-15s", section);

      for (i=0; i<sizeof(HH)/sizeof(*HH); i++)
        if (HH[i].num>=0) fprintf(f, "%9d ", errcnt[HH[i].num][err]);
      fprintf(f, "\n");
    }
  
}

void dispME(FILE *f, char *section, int err)
{
  unsigned int cnt=0, i;
  for (i=0; i<sizeof(MEHH)/sizeof(*MEHH); i++)
    if (MEHH[i].num>=0) cnt+=errcntME[MEHH[i].num][err];

  if (cnt>0)
    {
      if (MEHH[0].num<0)
        fprintf(f, "%-15s", section);

      for (i=0; i<sizeof(MEHH)/sizeof(*MEHH); i++)
        if (MEHH[i].num>=0) fprintf(f, "%9d ", errcntME[MEHH[i].num][err]);
      fprintf(f, "\n");
    }
}

void disp_report(FILE *f)
{
  unsigned int i, j;
  char temp[50];

  fprintf(f,"\n***** Error range report *****\n\n");

  for (i=0; i<sizeof(HH)/sizeof(*HH); i++)
    if (HH[i].num<0)
      fprintf(f,"%-15s", HH[i].header);
    else
      fprintf(f,"%-10s", HH[i].header);

  fprintf(f,"\n");
  for (i=0; i<sizeof(HH)/sizeof(*HH); i++)
    if (HH[i].num<0)
      fprintf(f,"%-15s", "______________");
    else
      fprintf(f,"%-10s", "_________");
  fprintf(f,"\n");

  for (j=LOWLIM; j<=HIGHLIM; j++)
    {
      if (j<500)
        sprintf(temp,"%+04d%% %+04d%%", ((j-499)-1)*RANGE+1, (j-499)*RANGE);
      else if (j>500)
        sprintf(temp,"%+04d%% %+04d%%", (j-501)*RANGE, ((j-501)+1)*RANGE-1);
      disp(f, temp, j);
    }
  strcpy(temp,"Out Of Range");
  disp(f, temp, 0);

  fprintf(f,"\n***** Missing/Extra report *****\n\n");

  for (i=0; i<sizeof(MEHH)/sizeof(*MEHH); i++)
    if (MEHH[i].num<0)
      fprintf(f,"%-15s", MEHH[i].header);
    else
      fprintf(f,"%-10s", MEHH[i].header);

  fprintf(f,"\n");
  for (i=0; i<sizeof(MEHH)/sizeof(*MEHH); i++)
    if (MEHH[i].num<0)
      fprintf(f,"%-15s", "______________");
    else
      fprintf(f,"%-10s", "_________");
  fprintf(f,"\n");

  for (j=0; j<=2; j++)
    {
      if (j==0) strcpy(temp,"Missing");
      else if (j==1) strcpy(temp,"Extra");
      else if (j==2) strcpy(temp,"Mismatch");
      else strcpy(temp,"?");
      dispME(f, temp, j);
    }

}

/*}}}************************************************************************/
/*{{{                    diff_runttvdiff()                                  */
/* parametres :                                                             */
/* ttvfig1 : fig source                                                     */
/****************************************************************************/
int diff_runttvdiff(file,ttvfig1,ttvfig2,maskopt)
FILE        *file;
ttvfig_list *ttvfig1;
ttvfig_list *ttvfig2;
long        maskopt;
{
  chain_list    *chainx;
  ttvfig_list   *ptfig;
  ttvfig_list   *ptfig1;
  ttvinfo_list  *ptinfo;
  chain_list    *ttvfiglist1 = NULL;
  chain_list    *ttvfiglist2 = NULL;
  ht            *htfig;
  ht            *htinfo;
  long           mask = maskopt;

  init_disp_report();

  if ((maskopt&(DIFF_OPT_HIER|DIFF_OPT_DTX)) == (DIFF_OPT_HIER|DIFF_OPT_DTX))
  {
    ttvfiglist1 = ttv_getttvfiglist(ttvfig1);
    ttvfiglist1 = reverse(ttvfiglist1);
    ttvfiglist2 = ttv_getttvfiglist(ttvfig2);
    ttvfiglist2 = reverse(ttvfiglist2);
    htfig  = addht(128);
    htinfo = addht(128);
    TTV_MAX_SIG = TTV_ALLOC_MAX ;

    for (chainx=ttvfiglist1;chainx;chainx=chainx->NEXT)
    {
      ptfig = chainx->DATA;
      ptinfo = ptfig->INFO;

      if (gethtitem(htfig,ptfig->INSNAME) == EMPTYHT)
        if (gethtitem(htinfo,ptinfo) == EMPTYHT)
        {
          addhtitem(htfig,ptfig->INSNAME,(long)ptfig);
          addhtitem(htinfo,ptinfo,0);
        }
    }

    for (chainx=ttvfiglist2;chainx;chainx=chainx->NEXT)
    {
      ptfig = chainx->DATA;
      ptinfo = ptfig->INFO;

      if ((ptfig1=(ttvfig_list*)gethtitem(htfig,ptfig->INSNAME)) == (ttvfig_list*)EMPTYHT)
      {
        if (ptinfo->FIGNAME == ttvfig2->INFO->FIGNAME)
        {
          mask = (maskopt&DIFF_OPT_NHIER)|DIFF_OPT_NTTX;
          diff_ttvfigdiff(file,ttvfig1,ttvfig2,mask) ;
        }
      }
      else
      {
        mask = (maskopt&DIFF_OPT_NHIER)|DIFF_OPT_NTTX;
        diff_ttvfigdiff(file,(ttvfig_list*)ptfig1,ptfig,mask);
        addhtitem(htinfo,ptinfo,0);
      }
    }

    if ((maskopt&DIFF_OPT_NTTX) == 0)
    {
      mask = (maskopt&DIFF_OPT_NDTX)|DIFF_OPT_NCAPA|DIFF_OPT_NSIG;
      diff_ttvfigdiff(file,ttvfig1,ttvfig2,mask) ;
    }

    freechain(ttvfiglist1);
    freechain(ttvfiglist2);
    delht(htfig);
    delht(htinfo);
  }
  else
    diff_ttvfigdiff(file,ttvfig1,ttvfig2,maskopt) ;


  disp_report(file);
  disp_report(stdout);

  return DIFF_NB ;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
