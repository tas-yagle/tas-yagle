/****************************************************************************/
/*                                                                          */
/* file    : vhd_drive.c                                                    */
/* date    : Oct  1999                                                      */
/* version  : v202                                                          */
/* author(s): LESTER A.P.P                                                  */
/*          : PINTO A.                                                      */
/* description : Synopsys VHDL driver                                       */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>

#include AVT_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BEG_H
#include BEF_H
#include BHL_H
#include BGL_H
#include BVL_H

#include "bgl_type.h"
#include "bgl_util.h"
#include "bgl_drive.h"

/* Global variables  */

static char         *TIME_UNIT;

int      BGL_DRIVE_PRIMITIVES   = FALSE;
int      BGL_ONLY_PRIMITIVES    = FALSE;


static int           BGL_VECT_ASS   = 0;
static int           BGL_CPT_IND    = 0;

static char         *CLOCKSTR;

/****************************************************************************/
/*{{{                    Static function prototypes                         */
/****************************************************************************/
static void writelist(FILE *fp, chain_list *ptchain, int indent, int posEdge, int negEdge);
static void print_value(FILE *fp,chain_list *abl, char type, int time, int,int,char **buffer, int *bufsize);
//static void          begen_treatment          (FILE *fp, befig_list *figure);

static void print_withselect(FILE *fp, biabl_list *biabl, char *name, int vectsize, char **buffer, int *bufsize);
static void print_affected(FILE *fp, char indent, char type, int time, int,int,char *name, char *vname, loop_list *loop,long);
static void          print_loop               (FILE *fp, char indent,char *head,int np,int *cpt,loop_list *loop);
static void          print_endLoop            (FILE *fp,char indent,char *head,int np,int cpt, int nbsame,loop_list *loop);
static void          print_loopList           (FILE *fp,char indent,char *head,int np,int *cpt, int nbsame,loop_list *loop);
static void print_signal(FILE *fp, char *name, char type, int left, int right);
static chain_list * print_process_declaration(FILE *fp, biabl_list *biabl, char *name, int np, char *head, char **buffer,ptype_list *clock);
static char *bgl_bebusname(char *name);
static char *bgl_beregname(char *name);
static void print_interassign(FILE *fp, char *dest, char *src, int mode);


static void print_special(long flags, char *buf)
{
  strcpy(buf,"");
  if (flags & (BEH_CND_WEAK|BEH_CND_STRONG|BEH_CND_PULL))
  {
    char *str="weak";
    if (flags & BEH_CND_STRONG) str="strong";
    else if (flags & BEH_CND_PULL) str="pull";
    sprintf(buf,"(%s1, %s0) ", str, str);
  }
}
/*}}}************************************************************************/
/*{{{                    Utilities                                          */
/****************************************************************************/
/*{{{                    writelist()                                        */
/*                                                                          */
/*  ecrit dans un fichier une chaine list                                   */
/****************************************************************************/
static void
writelist(FILE *fp, chain_list *ptchain, int indent, int posEdge, int negEdge)
{
  char          *name;
  char           indentstr[80];
  char buf[1024];
  int            hpos       = indent;
  int            first      = TRUE;
  int            i;
  chain_list    *namelist   = NULL;
  chain_list    *chainx;

  hpos = indent;
  for (i = 0; i < indent; i ++)
    indentstr[i] = ' ';
  indentstr[i] = '\0';

  if (posEdge)
  {
    name = bgl_getRadical(CLOCKSTR,NULL);
    hpos += strlen(name);
    if (!negEdge)
    {
      fprintf(fp, "posedge ");
      hpos += 8;
    }
    fprintf(fp, "%s", name);
    first = FALSE;
  }
  else if (negEdge)
  {
    name = bgl_getRadical(CLOCKSTR,NULL);
    hpos += strlen(name) + 8;
    fprintf(fp, "negedge %s", name);
    first = FALSE;
  }
  else
  {
    // pre-treatment to delete twice word
    // and clock in case of posedge or negedge
    for (; ptchain; ptchain = delchain(ptchain,ptchain))
    {
      chain_list    *tail;

      if ( isBitStr(ptchain->DATA) 
           /*|| ( (posEdge || negEdge) && ptchain->DATA == CLOCKSTR)*/)
        continue;
      beg_get_vectname(ptchain->DATA,&name,NULL,NULL,BEG_SEARCH_ABL);
      tail = NULL;
      for (chainx = namelist; chainx; chainx = chainx->NEXT)
      {
        tail = chainx;
        if ( name == chainx->DATA )
          break;
      }
      if (!chainx)
        if (tail)
          tail->NEXT = addchain(NULL,name);
        else
          namelist = addchain(NULL,name);
      else
        continue;
    }

    for (chainx = namelist; chainx; chainx = delchain(chainx,chainx))
    {
      if (!first)
        fprintf(fp, " or ");
      if (hpos > 60)
      {
        fprintf(fp, "\n%s", indentstr);
        hpos = indent;
      }
      name = chainx->DATA;
      hpos += strlen(name) + 4 ;
      fprintf(fp,"%s",bgl_vectorize_sub(name, buf));
      first = FALSE;
    }
  }
}

/*}}}************************************************************************/
/*{{{                    print_time()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_time_int(FILE *fp,int time,int timer, int timef, int timehz)
{
  if (timef<=0) timef=timer;
  if (timer<=0) timer=timef;
  if (timer==0 && timef==0) timef=timer=time;
                  
  if (timer)
  {
    if (timehz<0)
    {
      if (timer==timef)
        fprintf(fp,"#%u ",timer);
      else
        fprintf(fp,"#(%u, %u) ",timer, timef);
    }
    else
    {
      fprintf(fp,"#(%u, %u, %u) ",timer, timef, timehz);
    }
  }
}

static void print_time(FILE *fp,int time,int timer, int timef)
{
  print_time_int(fp,time,timer,timef,-1);
}

/*}}}************************************************************************/
/*{{{                    isone()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int isone(char *txt)
{
  return !strcmp(txt,"1'b1");
}


/*}}}************************************************************************/
/*{{{                    bebus_suffix()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *bgl_bebusname(char *name)
{
  char buf[256], *c;
  
  if ((c=strchr(name, ' '))!=NULL && *(c+1)!='\0')
  {
    *c='\0';
    sprintf(buf, "%s_bebus %s",name,c+1);
    *c=' ';
  }
  else
    sprintf(buf, "%s_bebus", name);
  return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                    bereg_suffix()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *bgl_beregname(char *name)
{
  char buf[256], *c;
  
  if ((c=strchr(name, ' '))!=NULL && *(c+1)!='\0')
  {
    *c='\0';
    sprintf(buf, "%s_bereg %s",name,c+1);
    *c=' ';
  }
  else
    sprintf(buf, "%s_bereg", name);
  return namealloc(buf);
}

/*}}}************************************************************************/
/*{{{                    Module declaration                                 */
/****************************************************************************/
/*{{{                    module_declaration_vect()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void module_declaration_vect(FILE *fp,bevectpor_list *vpor,int cpt)
{
  if (vpor)
  {
    module_declaration_vect(fp,vpor->NEXT,cpt+1);
    fprintf(fp,"%s",bgl_vlogname(vpor->NAME));
    if (cpt)
      fprintf(fp,", ");
  }
}

/*}}}************************************************************************/
/*{{{                    module_declaration_sing()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void module_declaration_sing(FILE *fp,bepor_list *por,int vpor)
{
  if (por)
  {
    int             left, right;
    char           *name;
    struct bepor   *porx;

    if (vpor)
      fprintf(fp,", ");
    por = (bepor_list *)reverse((chain_list *)por);
    for (porx = por; porx; porx = porx->NEXT)
    {
      porx = bgl_vectnam(porx, &left, &right, &name, 0);
      fprintf(fp, "%s", bgl_vlogname(name));
      mbkfree(name);
      if (porx->NEXT)
        fprintf(fp,", ");
    }
    por = (bepor_list *)reverse((chain_list *)por);
  }
}

/*}}}************************************************************************/
/*{{{                    module_declaration()                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
module_declaration(FILE *fp, befig_list *figure)
{
  iprint(fp,'i',"module %s", bgl_vlogname(figure->NAME));

  if (figure->BEPOR || figure->BEVECTPOR)
  {
    fprintf(fp," (");
    module_declaration_vect(fp,figure->BEVECTPOR,0);
    module_declaration_sing(fp,figure->BEPOR,(int)(long)figure->BEVECTPOR);
    fprintf(fp,")");
  }

  fprintf(fp, ";\n\n");
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    print_process_declaration()                        */
/*                                                                          */
/* Create the sensitivity list                                              */
/****************************************************************************/
static chain_list *
print_process_declaration(FILE *fp, biabl_list *biabl, char *name,
                          int np, char *head, char **buffer, ptype_list *clock)
{
  chain_list    *loopchain          = NULL;
  chain_list    *chainx;
  loop_list     *loop;
  biabl_list    *biablx;
  chain_list    *temp_expr;
  int            isFlipFlop         = FALSE;
  int            isEdgeCondition    = FALSE;
  int            posEdge            = 0;
  int            negEdge            = 0;
  char *clk;

  iprint(fp,'i',"always @ (");
  for (biablx = biabl; biablx; biablx = biablx->NEXT)
  {
    if (beh_isloop(biablx))
    {
      for (loop = beh_getloop(biablx); loop; loop = loop->NEXT)
      {
        for (chainx = loopchain; chainx; chainx = chainx->NEXT)
          if (chainx->DATA == loop->VARNAME)
            break;
        if (!chainx)
          loopchain = addchain(loopchain,loop->VARNAME);
      }
    }
    if (biablx->VALABL == NULL)// || biablx->CNDABL == NULL)
      beh_toolbug(20, "print_process_declaration", name, 0);
/*    if ( biablx->CNDABL &&
         (isEdgeCondition = bef_search_stable(biablx->CNDABL,&CLOCKSTR)) > 0)
    {
      if (bef_search_notck(biablx->CNDABL,CLOCKSTR) > 0)
        negEdge = 1;
      else
        posEdge = 1;
      isFlipFlop = TRUE;
    }*/
  }
  if (clock==NULL || clock->DATA==NULL)
  {
    chain_list *list;
    temp_expr = createExpr(OR);
    if (!isFlipFlop)
      for (biablx = biabl; biablx; biablx = biablx->NEXT)
      {
        if (clock==NULL || (clock->DATA==NULL && clock->TYPE==1 && bef_search_stable(biablx->CNDABL,&clk))<=0)
         {
           if (biablx->CNDABL)
              addQExpr(temp_expr, copyExpr(biablx->CNDABL));
           addQExpr(temp_expr, copyExpr(biablx->VALABL));
         }
      }
    else
      for (biablx = biabl; biablx; biablx = biablx->NEXT)
        if (biablx->CNDABL)
        {
          if (clock==NULL || (clock->DATA==NULL && clock->TYPE==1 && bef_search_stable(biablx->CNDABL,&clk))<=0)
            addQExpr(temp_expr, copyExpr(biablx->CNDABL));
        }
    
    list=supportChain_listExpr(temp_expr);
    /*if (clock!=NULL && clock->DATA==NULL)
    {
      chain_list *temp=NULL;
      while (list!=NULL)
      {
        if (bgl_getRadical((char *)list->DATA,NULL)==list->DATA)
          temp=addchain(temp, list->DATA);
        list=delchain(list, list);
      }
      list=reverse(temp);
    }*/
    writelist(fp, list, 10, posEdge, negEdge);
    freeExpr(temp_expr);
  }
  else
  {
    CLOCKSTR=clock->DATA;
    writelist(fp, addchain(NULL, clock->DATA), 10, clock->TYPE==1?1:0, clock->TYPE==0?1:0);
  }
  fprintf(fp, ")\n");
  
  return loopchain;

  //
  np        = 0;
  head      = NULL;
  buffer    = NULL;
}

/*}}}************************************************************************/
/*{{{                    Process driving                                    */
/****************************************************************************/
static void print_procBegin(FILE *fp, chain_list *loopchain)
{
  chain_list    *chainx;
  
  if (loopchain)
  {
    iprint(fp,'i',"begin : LOOP_NB_%d\n",BGL_CPT_IND ++);
    iprint(fp,'0',"integer ");
    for (chainx = loopchain; chainx; chainx = delchain(chainx,chainx))
      if (chainx != loopchain)
        fprintf(fp,", %s",(char*)chainx->DATA);
      else
        fprintf(fp,"%s",(char*)chainx->DATA);
    fprintf(fp,";\n\n");
  }
  else
    iprint(fp,'i',"begin\n");
}

static void print_procEnd(FILE *fp)
{
  iprint(fp,'-',"end\n");
}

static void print_procIf(FILE *fp, char *buffer)
{
  if (buffer[0]=='@')
  {
   // iprint(fp,'+',"%s\n",buffer);
   iprint(fp,'+',"");
  }
  else
    iprint(fp,'+',"if (%s)\n",bgl_printabl(buffer));
}

static void print_procElsif(FILE *fp, char *buffer)
{
  iprint(fp,'+',"else if (%s)\n",bgl_printabl(buffer));
}

static int print_procElse(FILE *fp)
{
  iprint(fp,'+',"else\n");
  
  return 1;
}

static int print_procEndif(FILE *fp, char ind)
{
  iprint(fp,ind,NULL);

  return 1;
  
  fp        = NULL;
  ind       = 0;
}

static int print_edge(char *buffer, char *ckname, char ckstate,
                      char *signame)
{
  sprintf(buffer,"@ (%sedge %s)",(ckstate=='1')?"pos":"neg",ckname);
  
  return 0;
  signame   = NULL;
}
 

/*}}}************************************************************************/
/*{{{                    Ports driving                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

static void port_begin(FILE *fp)
{
  fp        = NULL;
}

static void port_end(FILE *fp)
{
  fprintf(fp,";\n\n");
}

/*}}}************************************************************************/
/*{{{                    print_endLoop()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_endLoop(FILE *fp,char indent,char *head,int np,int cpt,
                          int nbsame,loop_list *loop)
{
  if (loop)
  {
    if (nbsame <= 0)
      iprint(fp,indent,"end\n");
    print_endLoop(fp,indent,head,np,0,nbsame-1,loop->NEXT);
  }

  cpt       = 0;
}

/*}}}************************************************************************/
/*{{{                    print_loopList()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_loopList(FILE *fp,char indent,char *head,int np,int *cpt,
                           int nbsame,loop_list *loop)
{
  if (loop)
  {
    if (nbsame <= 0)
      print_loop(fp,indent,head,np,cpt,loop);
    print_loopList(fp,indent,head,np,cpt,nbsame - 1,loop->NEXT);
  }
}

/*}}}************************************************************************/
/*{{{                    print_loop()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_loop(FILE *fp,char indent,char *head,int np,int *cpt,
                       loop_list *loop)
{
  int        left, right, up;
  char      *name;

  name  = loop->VARNAME;
  left  = loop->LEFT;
  right = loop->RIGHT;
  up    = (left < right) ? 1 : 0;
  
  iprint(fp,'0',"for (%s = %d; %s %c= %d; %s = %s %c 1)\n",
         name,left,name,(up)?'<':'>',right,name,name,(up)?'+':'-');
  iprint(fp,indent,"begin\n");

  cpt   = NULL;
  head  = NULL;
  np    = 0;
}

/*}}}************************************************************************/
/*{{{                    print_withselect()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_withselect(FILE *fp, biabl_list *biabl, char *name,
                 int vectsize, char **buffer, int *bufsize)
{
  int                cpt     = 1;
  char               buf[4096];

  // sensibility list
  print_process_declaration(fp,biabl,name,cpt,"BUS",buffer,NULL);

  // process
  iprint(fp,'i',"begin\n");

  *buffer[0]    = '\0';
  *buffer       = bgl_abl2str(biabl->CNDABL,*buffer,bufsize);
  
  iprint(fp,'+',"case (%s)\n",bgl_printabl(*buffer));

  for (; biabl; biabl = biabl->NEXT, cpt ++)
  {
    if (biabl->VALABL == NULL)
      beh_toolbug(20,"print_withselect",name,0);

    if (biabl->FLAG&BEH_CND_SEL_OTHERS)
      iprint  (fp,'0',"default : ");
    else if (biabl->FLAG&BEH_CND_SELECT)
    {
      char  *bitcnd;

      bitcnd    = bgl_getBitStr(beh_getcndvectfbiabl(biabl),buf);
      iprint(fp,'0',"%s : ",bitcnd);
      mbkfree(bitcnd);
    }
    else
      beh_toolbug(19, "print_withselect", name, 0);

    fprintf(fp,"%s = ",name);
    print_time(fp,biabl->TIME, 0, 0);
    *buffer[0]  = '\0';
    *buffer     = bgl_abl2str(biabl->VALABL, *buffer, bufsize);
    fprintf(fp,"%s;\n",bgl_printabl(*buffer));

    if ( !biabl->NEXT )
      if ( !(biabl->FLAG&BEH_CND_SEL_OTHERS) )
      {
        char      *tmp;

        iprint (fp,'0',"default : ");
        fprintf(fp,"%s = ",name);
        tmp     = bgl_getBitStr(genHZBitStr(vectsize),buf);
        fprintf(fp,"%s;\n",tmp);
        mbkfree(tmp);
      }
  }
  iprint(fp,'<',"endcase\n");
  iprint(fp,'<',"end\n");
}

/*}}}************************************************************************/
/*{{{                    print_value()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_value(FILE *fp,chain_list *abl, char type, int time, int timer, int timef,
            char **buffer, int *bufsize)
{
  *buffer[0]    = '\0';

  switch (type)
  {
    case 'R' :
         print_time(fp,time,timer,timef);  
    case 'W':
    default:
         break;
  }

  if (BGL_VECT_ASS)
  {
    abl         = beg_compactAbl(copyExpr(abl));
    *buffer     = bgl_abl2str(abl, *buffer, bufsize);
    freeExpr(abl);
  }
  else
    *buffer     = bgl_abl2str(abl, *buffer, bufsize);
  fprintf(fp,"%s",bgl_printabl(*buffer));
  if (type!='w')
    fprintf(fp,";\n");

  time          = 0;
}

/*}}}************************************************************************/
/*{{{                    print_affected()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_affected(FILE *fp, char indent, char type, int time, int timer, int timef,
               char *name, char *vname, loop_list *loop, long specialflags)
{
  char buf[128];
  char *sign="=";
  if (indent)
    iprint(fp,indent,"");
  switch (type)
  {
    case 'W':
    case 'w':
         fprintf(fp,"assign ");
         print_special(specialflags, buf);
         fprintf(fp,buf);
         if (type=='w')
           print_time_int(fp,time,timer,timef,0);
         else
           print_time(fp,time,timer,timef);
         break;
    case 'R':
         if (V_BOOL_TAB[__MGL_NONBLOKING_ASSIGN].VALUE)
           sign="<=";
    default:
         break;
  }
  
  if (vname == name)
    fprintf(fp,"%s %s ",bgl_vectorize(name), sign);
  else if (loop && loop->INDEX)
    fprintf(fp,"%s[%s] %s ",name,loop->INDEX, sign);
  else
    fprintf(fp,"%s %s ",vname, sign);
}

/*}}}************************************************************************/
/*{{{                    print_signal()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_signal(FILE *fp, char *name, char type, int left, int right)
{
  switch (type)
  {
    case 'P' :
    case 'R' :
    case 'b' :
         iprint(fp,'0',"reg ");
         break;
    case 'B' :
    case 'r' :
    case 'c' :
    case 'W' :
         iprint(fp,'0',"wire ");
         break;
    default : 
         iprint(fp,'0',"linkage ");
  }
  if (type == 'B')
    name = bgl_bebusname(name);
  else if (type == 'b')
    name = bgl_bebusname(name);
  else if (type == 'P')
    name = bgl_beregname(name);

  if (left != -1)
    fprintf(fp,"[%d:%d] %s;\n",left,right,bgl_vlogname(name));
  else
    fprintf(fp,"%s;\n",bgl_vlogname(name));
}

/*}}}************************************************************************/
/*{{{                    print_por()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_por(FILE *fp, char *name, char dir, int left, int right)
{
  char           *mode;

  mode      = NULL;

  switch (dir)
  {
    case 'I':
         mode = "input";  break;
    case 'O': case 'Z':
         mode = "output"; break;
    case 'B': case 'T':
         mode = "inout";  break;
    default:
         mode = "input";  break;
  }

  if (left != -1)
    iprint(fp,'0',"%-6s [%d:%d] %s", mode, left, right, bgl_vlogname(name));
  else
    iprint(fp,'0',"%-6s %s", mode, bgl_vlogname(name));
}

/*}}}************************************************************************/
/*{{{                    print_interassign()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_interassign(FILE *fp, char *dest, char *src, int mode)
{
  char buf0[1024];
  char buf1[1024];
  if (mode)
    iprint(fp,'0',"assign %s = %s;\n",bgl_vectorize_sub(dest,buf0),bgl_vectorize_sub(src,buf1));
  else
    iprint(fp,'0',"assign %s = %s;\n", dest, src);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
///*{{{                    Traitement de la begen                             */
///****************************************************************************/
///*{{{                    begen_treatment()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//static void
//begen_treatment(FILE *fp, befig_list *figure)
//{
//  begen_list     *gen;
//
//  gen       = figure->BEGEN;
//
//  iprint(fp,'i',NULL);
//  if (gen)
//  {
//    iprint(fp,'+',"GENERIC (\n");
//    
//    for (gen = figure->BEGEN; gen->NEXT; gen = gen->NEXT)
//      iprint(fp,'0',"CONSTANT  %s : %s := %ld ;\t-- %s\n",
//             gen->NAME,gen->TYPE,*((long *)gen->VALUE),gen->NAME);
//    
//    iprint(fp,'-',"CONSTANT  %s : %s := %ld  \t-- %s\n",
//           gen->NAME,gen->TYPE,*((long *)gen->VALUE),gen->NAME);
//    iprint(fp,'-',"); \n");
//  }
//}
//
///*}}}************************************************************************/
///*}}}************************************************************************/
/*{{{                    Traitement des messages                            */
/****************************************************************************/
/*{{{                    bgl_initDriver()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void bgl_initDriver(bef_driver *driver, FILE *fp, befig_list *fig, ht *porht,
                    char **buffer, int *bufsize)
{
  bef_driverSet(driver,fp,fig,buffer,bufsize,
                print_time,
                print_endLoop,
                print_loopList,
                print_loop,
                print_value,
                print_affected,
                print_signal,
                print_interassign,
                isone);
  bef_driverSetAbl(driver,
                   bgl_abl2strbool);
  bef_driverSetProcess(driver,
                       print_withselect,
                       print_process_declaration,
                       print_procBegin,
                       print_procEnd,
                       print_procIf,
                       print_procElsif,
                       print_procElse,
                       print_procEndif,
                       print_edge);
  bef_driverSetStr(driver,
                   bgl_vlogname,
                   beh_buildvlgvectname,
                   bgl_vectorize_sub,
                   bgl_getBitStr,
                   bgl_bebusname,
                   bgl_beregname);
  bef_driverSetPor(driver,
                   print_por,
                   port_end,
                   port_begin,
                   porht);
  driver->VLG=1;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Function de drive                                  */
/****************************************************************************/
/*{{{                    vlogsavebefig()                                    */
/*                                                                          */
/* description : print out a text file containing a data-flow VHDL          */
/*     description                                                          */
/****************************************************************************/
void
vlogsavebefig(befig_list *pthedbefig, unsigned int trace_mode)
{
  char          *suffix, *str;
  char          *buffer;
  int            bufsize;
  FILE          *fp;
  ht            *vlgportht;
  bef_driver     driver;

  BGL_VECT_ASS  = (int)(long)getenv("DRI_VECT_ASS");

  if (!pthedbefig)
    beh_toolbug(10, "vlogsavebefig", NULL, 0);

  bufsize       = 2048;
  buffer        = mbkalloc(bufsize);
  buffer[0]     = '\0';
  
  suffix        = V_STR_TAB[__BGL_FILE_SUFFIX].VALUE;
  if (!suffix)
    suffix      = namealloc("v");
  fp            = mbkfopen(pthedbefig->NAME,suffix,WRITE_TEXT);
  if (!fp)
  {
    beh_error(107, NULL);
    EXIT(1);
  }

  vlgportht                 = addht(40);
  // imported
  bgl_initDriver(&driver,fp,pthedbefig,vlgportht,&buffer,&bufsize);
  str           = getenv("BGL_DRIVE_PRIMITIVES");
  if (str != NULL)
    if ((str[0]=='y') || (str[0]=='Y'))
      BGL_DRIVE_PRIMITIVES  = 1;
    else
      BGL_DRIVE_PRIMITIVES  = 0;
  else
    BGL_DRIVE_PRIMITIVES    = 0;

  //BGL_LOOP_CPT              = 0;

  if ((trace_mode & BGL_TRACE) != 0)
    (void) fprintf(stdout, "Saving '%s' in a verilog file (%s)\n",
                   pthedbefig->NAME, suffix);

  sprintf(buffer,"Verilog data flow description generated from `%s`\n",
          pthedbefig->NAME);
  (void) avt_printExecInfo(fp, "/*", buffer, "*/");
  
  TIME_UNIT     = bef_gettimeunit(pthedbefig->TIME_UNIT);
  fprintf(fp, "\n");
  fprintf(fp, "`timescale 1 %s/1 %s\n\n", TIME_UNIT, TIME_UNIT);
  
  
  module_declaration(fp, pthedbefig);

  bef_drive_porDeclaration(&driver);
  bef_drive_sigDeclaration(&driver);

  delht(vlgportht);
  vlgportht     = NULL;

  bef_drive_aux(&driver);
  bef_drive_reg(&driver);
  bef_drive_bux(&driver);
  bef_drive_bus(&driver);
  bef_drive_out(&driver);
  
  delht(vlgportht);
  vlgportht     = NULL;

  (void) fprintf(fp, "\nendmodule\n");
  (void) fclose(fp);

  if (V_BOOL_TAB[__API_DRIVE_CORRESP].VALUE)
    bvl_drivecorresp(pthedbefig->NAME, pthedbefig, 1);

  bgl_freenames();
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
