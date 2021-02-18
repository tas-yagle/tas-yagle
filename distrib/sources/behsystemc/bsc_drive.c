/****************************************************************************/
/*                                                                          */
/* file         : bsc_drive.c                                               */
/* date         : March 2003                                                */
/* version      : v100                                                      */
/* author(s)    : PINTO A.                                                  */
/*                                                                          */
/* description  : SystemC HDL driver                                        */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <math.h>

#include MUT_H
#include LOG_H
#include BEH_H
#include BEG_H
#include BHL_H
#include BSC_H
#include "bsc_utype.h"
#include "bsc_utdef.h"
#include "bsc_util.h"

#define  BSC_INDENT          "  "
#define  BSC_ASS_PREFIX      "assign_"
#define  BSC_REG_PREFIX      "write_"
#define  BSC_BUS_PREFIX      "process_"

/* Global variables  */

static char       *CLOCKSTR;
static int         CKTYPE;
static FILE       *BSC_DOTH_FILE;
static FILE       *BSC_DOTCPP_FILE;

static int         BSC_MAXNAME;
static int         BSC_VECT_ASS = 0;

/****************************************************************************/
/*{{{                    Static function prototypes                         */
/****************************************************************************/
static void        writelist                (FILE *fp, chain_list *ptchain, int         indent);
static void        port_declaration         (FILE *fp, befig_list *figure);
static void        print_signals_bereg      (FILE *fp, befig_list *figure);
static void        print_signals_bevectreg  (FILE *fp, bevectreg_list *vreg);
static void        print_signals_bebux      (FILE *fp, befig_list *figure);
static void        print_signals_bevectbux  (FILE *fp, bevectbux_list *vbux);
static void        print_signals_beaux      (FILE *fp, befig_list *figure);
static void        print_signals_bevectaux  (FILE *fp, bevectaux_list *vaux);
static void        print_aux                (FILE *dotcpp, FILE *doth, befig_list *figure);
static void        print_out                (FILE *dotcpp, FILE *doth, befig_list *figure);
static void        bux_process              (FILE *dotcpp, FILE *doth, befig_list *figure);
static void        bus_process              (FILE *dotcpp, FILE *doth, befig_list *figure);
static void        reg_process              (FILE *dotcpp, FILE *doth, befig_list *figure);
//static void        begen_treatment          (FILE *fp, befig_list *figure);

static void        print_one_beobj          (FILE *dotcpp, FILE *doth, void *be, int *np,long type);
static void        printTime                (FILE *fp, int time);
static void        print_withselect         (FILE *dotcpp, biabl_list *biabl, char *name, int vectsize);
static void        print_process            (FILE *dotcpp, FILE *doth, biabl_list *biabl, char *name, char *vname, int vsize, int *np, char *head, int type);
static void        print_biabl              (FILE *dotcpp, FILE *doth, biabl_list *biabl, char *name, char *vname, int vsize, int *np, char *head, int type);
static void        print_value              (FILE *fp, chain_list *abl, int time);
static void        print_affected           (FILE *fp, char indent,char *name,char *vname,loop_list *loop);
static void        print_loop               (FILE *fp, char indent,char *head,int np,int *cpt,loop_list *loop);
static void        print_endLoop            (FILE *fp,char indent,char *head,int np,int cpt, int nbsame,loop_list *loop);
static void        print_loopList           (FILE *fp,char indent,char *head,int np,int *cpt, int nbsame,loop_list *loop);

static void        print_bevectpor          (FILE *fp, bevectpor_list *vpor,int cpt);
static void        print_bepor              (FILE *fp, bepor_list *por);
static void        print_process_declaration(FILE *fp, biabl_list *biabl, chain_list *abl, char *name, char *head, char *buffer);
static int         get_stable_attribute     (chain_list *ptabl);
static void        print_declaration        (FILE *fp, befig_list *fig);

static void print_dec(FILE *fp,char mode,int left,int right,char *name);

static void print_function_bevectaux(FILE *fp, bevectaux_list *vaux);
static void print_function_beaux(FILE *fp, befig_list *figure);
static void print_function_bevectbux(FILE *fp, bevectbux_list *vbux);
static void print_function_bebux(FILE *fp, befig_list *figure);
static void print_function_bevectbus(FILE *fp, bevectbus_list *vbus);
static void print_function_bebus(FILE *fp, befig_list *figure);
static void print_function_bevectreg(FILE *fp, bevectreg_list *vreg);
static void print_function_bereg(FILE *fp, befig_list *figure);
static void print_function_bevectout(FILE *fp, bevectout_list *vout);
static void print_function_beout(FILE *fp, befig_list *figure);
static void print_functiondeclaration(FILE *fp, befig_list *fig);

static void bsc_getHead(long type, char head[16]);

/*}}}************************************************************************/
/*{{{                    Utilities                                          */
/****************************************************************************/
/*{{{                    print_dec()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_dec(FILE *fp,char mode,int left,int right,char *name)
{
  switch (mode)
  {
    case 'I':
         iprint(fp,'0',"sc_in");
         break;
    case 'O': case 'Z':
         iprint(fp,'0',"sc_out");
         break;
    case 'B': case 'T':
         iprint(fp,'0',"sc_inout");
         break;
    case 'S':
         iprint(fp,'0',"sc_signal");
         break;
    default:
         /*beh_error(69, por->NAME);*/
         iprint(fp,'0',"sc_linkage");
         break;
  }
  if (left != -1)
    fprintf(fp,"<sc_lv<%d> > %s;\n",abs(left - right) + 1,bsc_name(name));
  else
    fprintf(fp,"<sc_logic> %s;\n",bsc_name(name));
}

/*}}}************************************************************************/
/*{{{                    writelist()                                        */
/*                                                                          */
/*  ecrit dans un fichier une chaine list                                   */
/****************************************************************************/
static void writelist(FILE *fp, chain_list *ptchain, int indent)
{
  char          *name;
  char           indentstr[80];
//  int            hpos;
//  int            first  = TRUE;
  int            i;
  chain_list    *namelist = NULL;
  chain_list    *chainx;
  chain_list    *tail;

//  hpos = indent;
  for (i = 0; i < indent; i++)
    indentstr[i] = ' ';
  indentstr[i] = '\0';

  // pre-traitement de la liste pour supprimmer les doublons
  for (; ptchain; ptchain = ptchain->NEXT)
  {
    if ( isBitStr(ptchain->DATA) )
      continue;
    beg_get_vectname(ptchain->DATA,&name,NULL,NULL,BEG_SEARCH_ABL);
    tail = NULL;
    for (chainx=namelist;chainx;chainx=chainx->NEXT)
    {
      tail = chainx;
      if (name==chainx->DATA) break;
    }
    if (!chainx)
      if (tail) tail->NEXT = addchain(NULL,name);
      else namelist = addchain(NULL,name);
    else continue;
  }

  if (namelist)
    fprintf(fp,"sensitive");
  for (chainx = namelist; chainx; chainx = chainx->NEXT)
  {
//    if (!first)
//      fprintf(fp, ", ");
//    if (hpos > 60)
//    {
//      fprintf(fp, "\n%s", indentstr);
//      hpos = indent;
//    }
    name = chainx->DATA;
//    hpos += strlen(name) + 2;
    fprintf(fp," << %s",bsc_name(name));
//    first = FALSE;
  }
  freechain(namelist);
}

/*}}}************************************************************************/
/*{{{                    printInfo()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void printInfo(FILE *fp, time_t *ck, char *text)
{
  fprintf(fp,"// SystemC data flow description generated from `%s`\n", text);
  fprintf(fp,"//     date : %s\n", ctime(ck));
}

/*}}}************************************************************************/
/*{{{                    printTime()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void printTime(FILE *fp,int time)
{
  if (time)
    fprintf(fp," AFTER %u %s",time,bhl_getTimeUnit());
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void bsc_getHead(long type, char head[16])
{
  switch (type)
  {
    case BEG_TYPE_SINGAUX : case BEG_TYPE_VECTAUX : case BEG_TYPE_SINGOUT :
    case BEG_TYPE_VECTOUT :
         sprintf(head,BSC_ASS_PREFIX);
         break;
    case BEG_TYPE_VECTBUX : case BEG_TYPE_SINGBUX : case BEG_TYPE_VECTBUS :
    case BEG_TYPE_SINGBUS :
         sprintf(head,BSC_BUS_PREFIX);
         break;
    case BEG_TYPE_VECTREG : case BEG_TYPE_SINGREG :
         sprintf(head,BSC_REG_PREFIX);
         break;
    default :
         beh_toolbug(19,"print_one_beobj type",NULL,0);
  }
}

/*}}}************************************************************************/
/*{{{                    print_one_beobj()                                  */
/*                                                                          */
/* print a process for a single signal                                      */
/*                                                                          */
/****************************************************************************/
void print_one_beobj(FILE  *dotcpp, FILE *doth, void *be, int *np,long type)
{
  chain_list        *abl;
  biabl_list        *biabl;
  vectbiabl_list    *vbiabl;
  char              *vname, *name, head[16], *buffer;
  long               time, singorvect;
  int                left, right, bufsize = 512, vectsize;

  buffer = mbkalloc(bufsize);
  bhl_getinfo(be,type,&abl,&time,&name,&left,&right,&vbiabl,&biabl);
  bsc_getHead(type,head);
  
  singorvect = type &  BEG_MASK_TYPE;
  type       = type & ~BEG_MASK_TYPE;

  switch (singorvect)
  {
    case BEG_MASK_VECT :
         switch (type)
         {
           case BEG_MASK_REG : case BEG_MASK_BUS : case BEG_MASK_BUX :
                name      = bsc_name(name);
                for (; vbiabl; vbiabl = vbiabl->NEXT)
                {
//                  iprint(dotcpp,'0',"\n");
//                  iprint(doth  ,'0',"\n");
                  left      = vbiabl->LEFT;
                  right     = vbiabl->RIGHT;
                  vname     = beh_buildvhdvectname(name,left,right);
                  vectsize  = abs(left - right) + 1;
                  biabl     = vbiabl->BIABL;
                  print_biabl(dotcpp,doth,biabl,name,vname,vectsize,np,head,type);
                  mbkfree(vname);
                }
                break;
           case BEG_MASK_OUT : case BEG_MASK_AUX :
                if (!abl)
                  beh_error(40, vname);
                name        = bsc_name(name);
                print_process_declaration(doth,NULL,abl,name,head,buffer);
                vname       = beh_buildvhdvectname(name,left,right);
                print_affected(dotcpp,'0',NULL,vname,NULL);
                print_value   (dotcpp,abl,time);
                mbkfree(vname);
                break;
           default : ;
         }
         break;
    case BEG_MASK_SING :
         switch (type)
         {
           case BEG_MASK_REG : case BEG_MASK_BUS : case BEG_MASK_BUX :
//                iprint(dotcpp,'0',"\n");
//                iprint(doth  ,'0',"\n");
                print_biabl(dotcpp,doth,biabl,name,name,1,np,head,type);
                break;
           case BEG_MASK_OUT : case BEG_MASK_AUX :
                if (!abl)
                  beh_error(40, name);
                print_process_declaration(doth,NULL,abl,name,head,buffer);
                print_affected(dotcpp,'0',name,name,NULL);
                print_value   (dotcpp,abl,time);
                break;
           default : ;
         }
         break;
    default : ;
  }
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_senslist()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_process_declaration(FILE *fp, biabl_list *biabl, chain_list *abl,
                               char *name, char *head, char *buffer)
{
  biabl_list    *biablx;
  chain_list    *temp_expr;
  chain_list    *temp_support;

  iprint(fp,'0',"SC_THREAD(%s%s)\n",head,name);
  
  if (biabl)
  {
    temp_expr = createExpr(OR);
    for(biablx = biabl; biablx; biablx = biablx->NEXT)
    {
      if (biablx->CNDABL)
        addQExpr(temp_expr,copyExpr(biablx->CNDABL));
      else
        beh_toolbug(19,"print_process_declaration",name,0);
      if (biablx->VALABL)
        addQExpr(temp_expr,copyExpr(biablx->VALABL));
      else
        beh_toolbug(20,"print_process_declaration",name,0);
    }
  }
  else if (abl)
    temp_expr = copyExpr(abl);
  else
    beh_toolbug(20,"print_process_declaration",name,0);
  
  temp_support = supportChain_listExpr(temp_expr);
  iprint(fp,'0',"");
  writelist(fp, temp_support, 14 + strlen(buffer));
  fprintf(fp,";\n");
  freechain(temp_support);
  freeExpr(temp_expr);
}

/*}}}************************************************************************/
/*{{{                    print_process()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_process(FILE *dotcpp, FILE *doth, biabl_list *biabl, char *name, char *vname,
                   int vsize, int *np, char *head, int type)
{
  char          *buffer;
  int            bufsize = 512;

  biabl_list    *biablx;
  chain_list    *ptcondabl          = NULL;
  chain_list    *abl_red            = NULL;
  loop_list     *loop               = NULL;
  loop_list     *lastloop           = NULL;
  char          *oldckcnd           = NULL;
  int            done_endif;
  int            done_else          = 0;
  int            isEdgeCondition    = 0;
  int            sameloop           = 0;
  int            cpt                = 0;
  int            sameckcnd          = 0;
  int            sndckcnd           = 0;

  buffer = mbkalloc(bufsize);

  // sensibility list
  print_process_declaration(doth,biabl,NULL,name,head,buffer);

  // process
  iprint(dotcpp,'i',"BEGIN\n");

  done_endif  = 1;
  for (biablx = biabl; biablx; biablx = biablx->NEXT)
  {
    if (!done_endif && !(biablx->FLAG&BEH_CND_PRECEDE)
        && !(biablx->FLAG&BEH_CND_LAST))
    {
      iprint(dotcpp,0,"END IF;\n");
      done_endif    = 1;
    }

    loop      = beh_getloop(biablx);
    sameloop  = beh_isSameLoop(loop,lastloop);
    print_endLoop(dotcpp,'<',head,*np,cpt,sameloop,lastloop);
    print_loopList(dotcpp,'+',head,*np,&cpt,sameloop,loop);
    // definition de la condition
    // case of register
    if ( (type == BEG_MASK_REG)
         && (isEdgeCondition = search_stable(biablx->CNDABL,&CLOCKSTR)) )
    {
      char   ckstate = (CKTYPE = search_notck(biablx->CNDABL,CLOCKSTR) > 0)?'0':'1';
      char  *ckname  = bsc_vectorize(CLOCKSTR);

      sprintf(buffer,"(%s = '%c' and %s'EVENT)",ckname,ckstate,ckname);
      sameckcnd = oldckcnd && !strcmp(oldckcnd,buffer);
      mbkfree(oldckcnd);
      oldckcnd = mbkstrdup(buffer);
    }
    else
    // default
    {
      abl_red = preprocess_abl(biablx->CNDABL);
      buffer[0] = '\0';
      buffer = bsc_abl2strbool(abl_red,buffer,&bufsize);
      freeExpr(abl_red);
      isEdgeCondition = 0;
    }

    // ecriture de la condition
    // the first
    if (biablx == biabl)
      iprint(dotcpp,'+',"IF %s THEN\n",bsc_printabl(buffer));
    // the last pour le else
    else if ( ( ! biablx->NEXT || ! (biablx->NEXT->FLAG & BEH_CND_PRECEDE) )
              && ! strcmp(buffer,"'1'"))
    {
      iprint(dotcpp,'+',"ELSE\n");
      done_endif    = 1;
      done_else     = TRUE;
    }
    // precedence
    else if ( ! (biablx->FLAG & BEH_CND_PRECEDE))
    {
      // case of bus and not the last
      if ( (type != BEG_MASK_REG) && !done_else)
      {
        iprint(dotcpp,'+',"ELSE\n");
        done_endif    = 1;
        print_affected(dotcpp,'-',name,vname,loop);
        fprintHZBitStr(dotcpp,genHZBitStr(vsize));
        fprintf(dotcpp,";\n");
      }
      if (! sameckcnd)
      {
        if (!done_endif)
        {
          iprint(dotcpp,'0',"END IF;\n");
          done_endif    = 1;
        }
        iprint(dotcpp,'+',"IF %s THEN\n",bsc_printabl(buffer));
      }
      else
        iprint(dotcpp,'+',NULL);
      done_else = FALSE;
    }
    else if (biablx->FLAG & BEH_CND_LAST)
    {
      iprint(dotcpp,'+',"ELSE\n");
      done_else = TRUE;
    }
    else
      iprint(dotcpp,'+',"ELSIF %s THEN\n",bsc_printabl(buffer));

    // supplementary condition in case of register
    if (isEdgeCondition)
    {
      ptcondabl = strip_ckedge(biablx->CNDABL,CLOCKSTR,CKTYPE);
      sndckcnd  = ( ! ( ATOM(ptcondabl)
                        && ! ( strcmp (VALUE_ATOM (ptcondabl), "'1'")
                               && strcmp (VALUE_ATOM (ptcondabl), "'0'"))));
      if (sndckcnd)
      {
        abl_red = preprocess_abl(ptcondabl);
        buffer[0] = '\0';
        buffer = bsc_abl2strbool(abl_red, buffer, &bufsize);
        freeExpr(abl_red);
        iprint(dotcpp,'+',"IF %s THEN\n",bsc_printabl(buffer));
      }
      freeExpr(ptcondabl);
    }

    // value
    if (biablx->VALABL)
    {
      print_affected(dotcpp,'-',name,vname,loop);
      print_value   (dotcpp,biablx->VALABL,biablx->TIME);
    }
    else
      beh_toolbug(20, "print_process",name, 0);

    done_endif  = 0;

    if (sndckcnd)
    {
      sndckcnd = 0;
      iprint(dotcpp,'-',"END IF;\n");
    }
    lastloop = loop;
  }

  // la fin du process
  // ELSE for buffer
  if ( (type != BEG_MASK_REG) && !done_else)
  {
    iprint(dotcpp,'+',"ELSE\n");
    print_affected(dotcpp,'-',name,vname,loop);
    fprintHZBitStr(dotcpp,genHZBitStr(vsize));
    fprintf(dotcpp,";\n");
  }
  if (!done_endif)
    iprint(dotcpp,'-',"END IF;\n");
  print_endLoop(dotcpp,'-',head,*np,cpt,0,loop);
  iprint(dotcpp,'-',"END PROCESS;\n");
  (*np) ++;

  mbkfree(oldckcnd);
  mbkfree(buffer);
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
      iprint(fp,indent,"END LOOP %s%dLOOP%d;\n", head, np, --cpt);
    print_endLoop(fp,indent,head,np,cpt,nbsame-1,loop->NEXT);
  }
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
  int        left, right;
  char      *name;

  name  = loop->VARNAME;
  left  = loop->LEFT;
  right = loop->RIGHT;
  iprint(fp, indent, "%s%dLOOP%d : FOR %s IN %d %s %d LOOP\n",
         head, np, *cpt, name, left, (left > right)?"DOWNTO":"TO", right);
  *cpt  = *cpt + 1;
}

/*}}}************************************************************************/
/*{{{                    print_withselect()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_withselect(FILE *dotcpp, biabl_list *biabl, char *name,int vectsize)
{
  int                cpt     = 1;
  char              *buffer;
  int                bufsize = 512;
  char              *tmp;

  buffer    = mbkalloc(bufsize);
  buffer[0] = '\0';
  buffer    = bsc_abl2str(biabl->CNDABL,buffer,&bufsize);
  iprint(dotcpp,'i',"WITH %s SELECT\n",bsc_printabl(buffer));
  iprint(dotcpp,'+',"%s <=\n",name);

  for (;biabl;biabl=biabl->NEXT, cpt ++)
  {
    if (biabl->VALABL == NULL)
      beh_toolbug(20,"print_withselect",name,0);

    buffer[0] = '\0';
    buffer    = bsc_abl2str(biabl->VALABL, buffer, &bufsize);
    iprint   (dotcpp,0,"%s",bsc_printabl(buffer));
    printTime(dotcpp,biabl->TIME);
    if (biabl->FLAG&BEH_CND_SEL_OTHERS)
      fprintf(dotcpp," WHEN OTHERS");
    else if (biabl->FLAG&BEH_CND_SELECT)
    {
      tmp   = bsc_getBitStr(beh_getcndvectfbiabl(biabl));
      fprintf(dotcpp," WHEN %s",tmp);
      mbkfree(tmp);
    }
    else beh_toolbug(19, "print_withselect", name, 0);

    if (biabl->NEXT)
      fprintf(dotcpp,",\n");
    else if (!(biabl->FLAG&BEH_CND_SEL_OTHERS))
    {
      fprintf(dotcpp,",\n");
      iprint(dotcpp,0,"");
      fprintHZBitStr(dotcpp,genHZBitStr(vectsize));
      fprintf(dotcpp," WHEN OTHERS;\n");
    }
    else
      fprintf(dotcpp,";\n");
  }
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_biabl()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_biabl(FILE *dotcpp, FILE *doth, biabl_list *biabl, char *name, char *vname,
                 int vsize, int *np, char *head, int type)
{
  if ( ! biabl->CNDABL)
    beh_toolbug(19,"print_biabl",name,0);
  if ( ! biabl->VALABL)
    beh_toolbug(20,"print_biabl",name,0);
  // case of  with ... select
  if ((type != BEG_MASK_REG) && (beh_simplifybiabl(biabl)))
    print_withselect(dotcpp,biabl,vname,vsize);
  // case of process
  else
    print_process   (dotcpp,doth,biabl,name,vname,vsize,np,head,type);
}

/*}}}************************************************************************/
/*{{{                    print_value()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_value(FILE *fp,chain_list *abl, int time)
{
  char           *buffer;
  int             bufsize = 512;

  buffer = mbkalloc(bufsize);
  buffer[0] = '\0';

  if (BSC_VECT_ASS)
  {
    abl    = beg_compactAbl(copyExpr(abl));
    buffer = bsc_abl2str(abl, buffer, &bufsize);
    freeExpr(abl);
  }
  else
    buffer = bsc_abl2str(abl, buffer, &bufsize);
  fprintf(fp,"%s",bsc_printabl(buffer));
  printTime(fp,time);
  fprintf(fp,";\n");

  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_affected()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_affected(FILE *fp,char indent,char *name,char *vname,
                    loop_list *loop)
{
  if (vname == name)
    iprint(fp,indent,"%s <= ",bsc_vectorize(name));
  else if (loop && loop->INDEX)
    iprint(fp,indent,"%s (%s) <= ",name,loop->INDEX);
  else
    iprint(fp,indent,"%s <= ",vname);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Declaration des ports                              */
/****************************************************************************/
/*{{{                    ports variables                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static ht *bscportht = NULL;

/*}}}************************************************************************/
/*{{{                    print_one_bepor()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static bepor_list *
print_one_bepor(FILE * fp, bepor_list *por)
{
  int            left, right;
  char          *name;

  por   = bsc_vectnam(por, &left, &right, &name, 0);
  if (bscportht != NULL)
    addhtitem(bscportht, namealloc(name), 0);

  print_dec(fp,por->DIRECTION,left,right,name);

  mbkfree(name);
  return por;
}

/*}}}************************************************************************/
/*{{{                    print_one_bevectpor()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_one_bevectpor(FILE *fp, bevectpor_list *vpor)
{
  int            left, right;
  char          *name;

  left  = vpor->LEFT ;
  right = vpor->RIGHT;
  name  = vpor->NAME ;
  print_dec(fp,vpor->DIRECTION,left,right,name);
}

/*}}}************************************************************************/
/*{{{                    print_bevectpor()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_bevectpor(FILE *fp, bevectpor_list *vpor,int cpt)
{
  if (vpor)
  {
    if (BSC_MAXNAME < (int)strlen(vpor->NAME))
      BSC_MAXNAME = (int)strlen(vpor->NAME);
    print_bevectpor(fp,vpor->NEXT,cpt + 1);
    print_one_bevectpor(fp,vpor);
  }
}

/*}}}************************************************************************/
/*{{{                    print_bepor()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_bepor(FILE *fp, bepor_list *por)
{
  if (por)
  {
    struct bepor    *porx;

    por = (bepor_list *)reverse((chain_list *)por);

    for (porx = por; porx; porx = (print_one_bepor(fp, porx))->NEXT)
      ;

    por = (bepor_list *)reverse((chain_list *)por);
  }
}

/*}}}************************************************************************/
/*{{{                    port_declaration()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void port_declaration(FILE *fp, befig_list *figure)
{
  BSC_MAXNAME   = 0;

  // scalar port declaration
  print_bevectpor(fp,figure->BEVECTPOR,figure->BEPOR ? 1 : 0);
  // classical port declaration
  print_bepor    (fp,figure->BEPOR);
  iprint         (fp,'0',"\n");
  BSC_MAXNAME   = 0;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Declaration des signaux                            */
/****************************************************************************/
/*{{{                    print_declaration()                                */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_declaration(FILE *fp, befig_list *fig)
{
  print_signals_bevectreg (fp, fig->BEVECTREG);
  print_signals_bereg     (fp, fig);
  print_signals_bevectbux (fp, fig->BEVECTBUX);
  print_signals_bebux     (fp, fig);
  print_signals_bevectaux (fp, fig->BEVECTAUX);
  print_signals_beaux     (fp, fig);
  iprint(fp,'0',"\n");
}

/*}}}************************************************************************/
/*{{{                    print_signals_bereg()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_signals_bereg(FILE *fp, befig_list *figure)
{
  int             left, right;
  bereg_list     *reg;
  char           *name = NULL;

  figure->BEREG = (bereg_list *) reverse((chain_list *) figure->BEREG);

  for (reg = figure->BEREG; reg; reg = reg->NEXT)
  {
    reg = bsc_vectnam(reg, &left, &right, &name, 2);
    if (bscportht != NULL && gethtitem(bscportht, namealloc(name)) != EMPTYHT)
      continue;
    print_dec(fp,'S',left,right,name);

    mbkfree(name);
  }
  figure->BEREG = (bereg_list *) reverse((chain_list *)figure->BEREG);
}

/*}}}************************************************************************/
/*{{{                    print_signals_bevectreg()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_signals_bevectreg(FILE *fp, bevectreg_list *vreg)
{
  if (vreg)
  {
    int                left, right;
    char              *name = NULL;

    print_signals_bevectreg(fp,vreg->NEXT);
    left  = vreg->LEFT;
    right = vreg->RIGHT;
    name  = vreg->NAME;

    if ( bscportht && (gethtitem(bscportht,namealloc(name))!=EMPTYHT))
      return;
    print_dec(fp,'S',left,right,name);
  }
}
/*}}}************************************************************************/
/*{{{                    print_signals_bebux()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_signals_bebux(FILE *fp, befig_list *figure)
{
  int             left, right;
  struct bebux   *ptbebux;
  char           *name = NULL;

  figure->BEBUX = (struct bebux *)reverse((chain_list *)figure->BEBUX);

  for (ptbebux = figure->BEBUX; ptbebux; ptbebux = ptbebux->NEXT)
  {
    ptbebux = bsc_vectnam(ptbebux, &left, &right, &name, 1);
    print_dec(fp,'S',left,right,name);
    mbkfree(name);
  }
  figure->BEBUX = (struct bebux *)reverse((chain_list *)figure->BEBUX);
}

/*}}}************************************************************************/
/*{{{                    print_signals_bevectbux()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_signals_bevectbux(FILE *fp, bevectbux_list *vbux)
{
  if (vbux)
  {
    int                left, right;
    char              *name = NULL;

    print_signals_bevectbux(fp,vbux->NEXT);
    left  = vbux->LEFT;
    right = vbux->RIGHT;
    name  = vbux->NAME;
    print_dec(fp,'S',left,right,name);
  }
}

/*}}}************************************************************************/
/*{{{                    print_signals_beaux()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
print_signals_beaux(FILE *fp, befig_list *figure)
{
  int             left, right;
  beaux_list     *ptbeaux;
  char           *name = NULL;

  figure->BEAUX = (beaux_list *)reverse((chain_list *)figure->BEAUX);

  for (ptbeaux = figure->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT)
  {
    ptbeaux = bsc_vectnam(ptbeaux, &left, &right, &name, 3);
    print_dec(fp,'S',left,right,name);
    mbkfree(name);
  }
  figure->BEAUX = (beaux_list *)reverse((chain_list *)figure->BEAUX);
}

/*}}}************************************************************************/
/*{{{                    print_signals_bevectaux()                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_signals_bevectaux(FILE *fp, bevectaux_list *vaux)
{
  if (vaux)
  {
    int                left, right;
    char              *name = NULL;

    print_signals_bevectaux(fp,vaux->NEXT);

    left  = vaux->LEFT;
    right = vaux->RIGHT;
    name  = vaux->NAME;
    print_dec(fp,'S',left,right,name);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Declaration des fonctions                          */
/****************************************************************************/
/*{{{                    print_functiondeclaration()                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_functiondeclaration(FILE *fp, befig_list *fig)
{
  print_function_bevectout (fp, fig->BEVECTOUT);
  print_function_beout     (fp, fig);
  print_function_bevectbus (fp, fig->BEVECTBUS);
  print_function_bebus     (fp, fig);
  print_function_bevectreg (fp, fig->BEVECTREG);
  print_function_bereg     (fp, fig);
  print_function_bevectbux (fp, fig->BEVECTBUX);
  print_function_bebux     (fp, fig);
  print_function_bevectaux (fp, fig->BEVECTAUX);
  print_function_beaux     (fp, fig);
  iprint(fp,'0',"\n");
}

/*}}}************************************************************************/
/*{{{                    print_function_beout()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_beout(FILE *fp, befig_list *figure)
{
  int             left, right;
  beout_list     *out;
  char           *name = NULL;

  figure->BEOUT = (beout_list *) reverse((chain_list *) figure->BEOUT);

  for (out = figure->BEOUT; out; out = out->NEXT)
  {
    out = bsc_vectnam(out, &left, &right, &name, 2);
    if (bscportht != NULL && gethtitem(bscportht, namealloc(name)) != EMPTYHT)
      continue;
    iprint(fp,'0',"void %s%s();\n",BSC_ASS_PREFIX,name);

    mbkfree(name);
  }
  figure->BEOUT = (beout_list *) reverse((chain_list *)figure->BEOUT);
}

/*}}}************************************************************************/
/*{{{                    print_function_bevectout()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bevectout(FILE *fp, bevectout_list *vout)
{
  if (vout)
  {
    print_function_bevectout(fp,vout->NEXT);
    if ( bscportht && (gethtitem(bscportht,vout->NAME)!=EMPTYHT))
      return;
    iprint(fp,'0',"void %s%s();\n",BSC_ASS_PREFIX,vout->NAME);
  }
}
/*}}}************************************************************************/
/*{{{                    print_function_bereg()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bereg(FILE *fp, befig_list *figure)
{
  int             left, right;
  bereg_list     *reg;
  char           *name = NULL;

  figure->BEREG = (bereg_list *) reverse((chain_list *) figure->BEREG);

  for (reg = figure->BEREG; reg; reg = reg->NEXT)
  {
    reg = bsc_vectnam(reg, &left, &right, &name, 2);
    if (bscportht != NULL && gethtitem(bscportht, namealloc(name)) != EMPTYHT)
      continue;
    iprint(fp,'0',"void %s%s();\n",BSC_REG_PREFIX,name);

    mbkfree(name);
  }
  figure->BEREG = (bereg_list *) reverse((chain_list *)figure->BEREG);
}

/*}}}************************************************************************/
/*{{{                    print_function_bevectreg()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bevectreg(FILE *fp, bevectreg_list *vreg)
{
  if (vreg)
  {
    print_function_bevectreg(fp,vreg->NEXT);
    if ( bscportht && (gethtitem(bscportht,vreg->NAME)!=EMPTYHT))
      return;
    iprint(fp,'0',"void %s%s();\n",BSC_REG_PREFIX,vreg->NAME);
  }
}
/*}}}************************************************************************/
/*{{{                    print_function_bebus()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bebus(FILE *fp, befig_list *figure)
{
  int             left, right;
  struct bebus   *ptbebus;
  char           *name = NULL;

  figure->BEBUS = (struct bebus *)reverse((chain_list *)figure->BEBUS);

  for (ptbebus = figure->BEBUS; ptbebus; ptbebus = ptbebus->NEXT)
  {
    ptbebus = bsc_vectnam(ptbebus, &left, &right, &name, 1);
    iprint(fp,'0',"void %s%s();\n",BSC_REG_PREFIX,name);
    mbkfree(name);
  }
  figure->BEBUS = (struct bebus *)reverse((chain_list *)figure->BEBUS);
}

/*}}}************************************************************************/
/*{{{                    print_function_bevectbus()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bevectbus(FILE *fp, bevectbus_list *vbus)
{
  if (vbus)
  {
    print_function_bevectbus(fp,vbus->NEXT);
    iprint(fp,'0',"void %s%s();\n",BSC_REG_PREFIX,vbus->NAME);
  }
}

/*}}}************************************************************************/
/*{{{                    print_function_bebux()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bebux(FILE *fp, befig_list *figure)
{
  int             left, right;
  struct bebux   *ptbebux;
  char           *name = NULL;

  figure->BEBUX = (struct bebux *)reverse((chain_list *)figure->BEBUX);

  for (ptbebux = figure->BEBUX; ptbebux; ptbebux = ptbebux->NEXT)
  {
    ptbebux = bsc_vectnam(ptbebux, &left, &right, &name, 1);
    iprint(fp,'0',"void %s%s();\n",BSC_REG_PREFIX,name);
    mbkfree(name);
  }
  figure->BEBUX = (struct bebux *)reverse((chain_list *)figure->BEBUX);
}

/*}}}************************************************************************/
/*{{{                    print_function_bevectbux()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bevectbux(FILE *fp, bevectbux_list *vbux)
{
  if (vbux)
  {
    print_function_bevectbux(fp,vbux->NEXT);
    iprint(fp,'0',"void %s%s();\n",BSC_REG_PREFIX,vbux->NAME);
  }
}

/*}}}************************************************************************/
/*{{{                    print_function_beaux()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_beaux(FILE *fp, befig_list *figure)
{
  int             left, right;
  beaux_list     *ptbeaux;
  char           *name = NULL;

  figure->BEAUX = (beaux_list *)reverse((chain_list *)figure->BEAUX);

  for (ptbeaux = figure->BEAUX; ptbeaux; ptbeaux = ptbeaux->NEXT)
  {
    ptbeaux = bsc_vectnam(ptbeaux, &left, &right, &name, 3);
    iprint(fp,'0',"void %s%s();\n",BSC_ASS_PREFIX,name);
    mbkfree(name);
  }
  figure->BEAUX = (beaux_list *)reverse((chain_list *)figure->BEAUX);
}

/*}}}************************************************************************/
/*{{{                    print_function_bevectaux()                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_function_bevectaux(FILE *fp, bevectaux_list *vaux)
{
  if (vaux)
  {
    print_function_bevectaux(fp,vaux->NEXT);
    iprint(fp,'0',"void %s%s();\n",BSC_ASS_PREFIX,vaux->NAME);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitements des signaux simples                    */
/****************************************************************************/
/*{{{                    print_aux()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_aux(FILE *dotcpp,FILE *doth, befig_list *figure)
{
  beaux_list        *aux  = figure->BEAUX;
  bevectaux_list    *vaux = figure->BEVECTAUX;

  if (aux || vaux)
    iprint(dotcpp,'i',"\n");

  for (; aux; aux = aux->NEXT)
    print_one_beobj(dotcpp, doth, aux, NULL, BEG_TYPE_SINGAUX);
  for (; vaux; vaux = vaux->NEXT)
    print_one_beobj(dotcpp, doth, vaux, NULL, BEG_TYPE_VECTAUX);
}

/*}}}************************************************************************/
/*{{{                    print_out()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_out(FILE *dotcpp, FILE *doth, befig_list *figure)
{
  bevectout_list    *vout = figure->BEVECTOUT;
  beout_list        *out  = figure->BEOUT;

  if (out || vout)
    iprint(dotcpp,'i',"\n");

  for (; out; out  = out->NEXT)
    print_one_beobj(dotcpp,doth,  out, NULL, BEG_TYPE_SINGOUT);
  for (; vout; vout = vout->NEXT)
    print_one_beobj(dotcpp,doth, vout, NULL, BEG_TYPE_VECTOUT);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitement des bus                                 */
/****************************************************************************/
/*{{{                    bux_process()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
bux_process(FILE *dotcpp, FILE *doth, befig_list *figure)
{
  bebux_list     *bux;
  bevectbux_list *vbux;
  int             numprocess = 0;

  for (bux = figure->BEBUX; bux; bux = bux->NEXT)
    print_one_beobj(dotcpp,doth,bux,&numprocess,BEG_TYPE_SINGBUX);

  numprocess = 0;
  for(vbux=figure->BEVECTBUX;vbux;vbux=vbux->NEXT)
    print_one_beobj(dotcpp,doth,vbux,&numprocess,BEG_TYPE_VECTBUX);
}

/*}}}************************************************************************/
/*{{{                    bus_process()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
bus_process(FILE *dotcpp, FILE *doth, befig_list *figure)
{
  bebus_list     *bus;
  bevectbus_list *vbus;
  int             numprocess = 0;

  for (bus = figure->BEBUS; bus; bus = bus->NEXT)
    print_one_beobj(dotcpp,doth,bus,&numprocess,BEG_TYPE_SINGBUS);

  numprocess = 0;
  for(vbus = figure->BEVECTBUS;vbus;vbus=vbus->NEXT)
    print_one_beobj(dotcpp,doth,vbus,&numprocess,BEG_TYPE_VECTBUS);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitement des registres                           */
/****************************************************************************/
/*{{{                    get_stable_attribute()                             */
/*                                                                          */
/* recupere l'attribut de STABLE                                            */
/****************************************************************************/
static int get_stable_attribute(chain_list *ptabl)
{
  int            num    = 0;

  if (ATOM(ptabl))
    return 0;
  if (OPER(ptabl) == STABLE)
  {
    CLOCKSTR = VALUE_ATOM(CADR(ptabl));
    if (ATTRIB(ptabl))
      num = VALUE_ATTRIB(ptabl);
  }
  else
    while ((ptabl = CDR(ptabl)))
      if ((num = get_stable_attribute(CAR(ptabl))))
        return num;

  return num;
}

/*}}}************************************************************************/
/*{{{                    reg_process()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
reg_process(FILE *dotcpp, FILE *doth, befig_list *figure)
{
  bereg_list     *reg;
  bevectreg_list *vreg;
  int             numprocess    = 0;

  for (reg = figure->BEREG; reg; reg = reg->NEXT)
    print_one_beobj(dotcpp,doth,reg,&numprocess,BEG_TYPE_SINGREG);

  numprocess = 0;
  for(vreg=figure->BEVECTREG;vreg;vreg=vreg->NEXT)
    print_one_beobj(dotcpp,doth,vreg,&numprocess,BEG_TYPE_VECTREG);
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
//  begen_list     *ptbegen;
//
//  ptbegen = figure->BEGEN;
//
//  iprint(fp,'i',NULL);
//  if (ptbegen != NULL)
//  {
//    iprint(fp,'+',"GENERIC (\n");
//    for (ptbegen = figure->BEGEN; ptbegen->NEXT; ptbegen = ptbegen->NEXT)
//      iprint(fp,'0',"CONSTANT  %s : %s := %ld ;\t-- %s\n",
//             ptbegen->NAME,ptbegen->TYPE,*((long *)ptbegen->VALUE),
//             ptbegen->NAME);
//    iprint(fp,'-',"CONSTANT  %s : %s := %ld  \t-- %s\n",
//           ptbegen->NAME,ptbegen->TYPE,*((long *)ptbegen->VALUE),
//           ptbegen->NAME);
//    iprint(fp,'-',"); \n");
//  }
//}
//
///*}}}************************************************************************/
///*}}}************************************************************************/
/*{{{                    Traitement des messages                            */
/****************************************************************************/
/*{{{                    print_msg()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_msg(FILE *fp, befig_list *figure)
{
  int             bufsize = 512;
  char           *buffer;
  bemsg_list     *msg;        /* current BEMSG pointer       */

  chain_list    *ptcondabl;

  buffer = mbkalloc(bufsize);

  for (iprint(fp,'i',"\n"), msg = figure->BEMSG; msg; msg = msg->NEXT)
  {
    int          sndckcnd;
    int          isedge;
    chain_list  *abl_red;

    iprint(fp,'i',NULL);
    sndckcnd    = 0;
    abl_red     = NULL;

    if (msg->LABEL)
      iprint(fp,'+',"%s : ASSERT (",msg->LABEL);
    else
      iprint(fp,'+',"ASSERT (");

    buffer[0] = '\0';
    if ((isedge = search_stable(msg->ABL,&CLOCKSTR)))
    {
      char   ckstate    = (CKTYPE = search_notck(msg->ABL,CLOCKSTR) > 0) ? '0' : '1';
      char  *ckname     =  bsc_name(CLOCKSTR);
      int    attrib;

      attrib = get_stable_attribute(msg->ABL);
      if (attrib)
        sprintf(buffer,"%s = '%c' and not %s'STABLE(%d %s)",
                ckname,ckstate,ckname,attrib,bhl_getTimeUnit());
      else
        sprintf(buffer,"%s = '%c' and %s'EVENT",ckname,ckstate,ckname);

      ptcondabl = strip_ckedge(msg->ABL,CLOCKSTR,CKTYPE);
      sndckcnd  = ( ! ( ATOM(ptcondabl)
                        &&
                        ! ( strcmp (VALUE_ATOM (ptcondabl), "'1'")
                            &&
                            strcmp (VALUE_ATOM (ptcondabl), "'0'"))));
      if (sndckcnd)
        (void) fprintf(fp,"%s and ",bsc_printabl(buffer));
      else
        (void) fprintf(fp,"%s",bsc_printabl(buffer));
    }

    if (sndckcnd)
    {
      abl_red = preprocess_abl(ptcondabl);
      freeExpr(ptcondabl);
    }
    if (!isedge)
      abl_red = msg->ABL;
    if (abl_red)
    {
      buffer[0] = '\0';
      buffer = bsc_abl2strbool(abl_red, buffer, &bufsize);
      (void) fprintf(fp,"%s",bsc_printabl(buffer));
      if (sndckcnd)
        freeExpr(abl_red);
    }
    (void) fprintf(fp,")\n");

    if (msg->MESSAGE)
      iprint(fp,'+',"REPORT %s\n",msg->MESSAGE);
    if (msg->LEVEL == 'W')
      iprint(fp,'-',"SEVERITY WARNING;\n");
    else
      iprint(fp,'-',"SEVERITY ERROR;\n");
  }
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Function de drive                                  */
/****************************************************************************/
/*{{{                    bscsavebefig()                                     */
/*                                                                          */
/* description : print out a text file containing a data-flow VHDL          */
/*     description                                                          */
/****************************************************************************/
void bscsavebefig(befig_list *befig, unsigned int trace_mode)
{
  char           *figname;
  FILE           *dotcpp, *doth;
  time_t          clock;

  BSC_VECT_ASS  = (int)getenv("DRI_VECT_ASS");

  if (befig == NULL)
    beh_toolbug(10, "bscsavebefig", NULL, 0);
  //   Opening result file
  figname   = befig->NAME;
  if ( !( ( doth     = mbkfopen(figname,"h"  ,WRITE_TEXT))
          && (dotcpp = mbkfopen(figname,"cpp",WRITE_TEXT))))
  {
    beh_error(107, NULL);
    EXIT(1);
  }
  else
  {
    BSC_DOTH_FILE   = doth;
    BSC_DOTCPP_FILE = dotcpp;
  }

  time(&clock);
  // SystemC header file
  if ((trace_mode & BSC_TRACE))
    fprintf(stdout,"Saving '%s' in SystemC header file (h)\n",figname);
  printInfo(doth  ,&clock,figname);

  //   Entity declaration
  iprint(doth,'i',"\n\n");
  iprint(doth,'i',"SC_MODULE(%s)\n",bsc_name(figname));
  iprint(doth,'i',"{\n");

  //  begen declaration
  // begen_treatment(fp, befig);

  //   Port declaration
  bscportht = addht(40);

  port_declaration (doth, befig);

  //   Signals declaration
  print_declaration(doth, befig);

  print_functiondeclaration(doth,befig);

  delht(bscportht);
  bscportht = NULL;

  iprint(doth,'0',"SC_CTOR(%s)\n",bsc_name(figname));
  iprint(doth,'+',"{\n");


  // SystemC file
  if ((trace_mode & BSC_TRACE))
    fprintf(stdout,"Saving '%s' in SystemC file (cpp)\n",figname);
  printInfo(dotcpp,&clock,figname);
  fprintf(dotcpp,"#include <systemc.h>\n");
  fprintf(dotcpp,"#include \"%s.h\"\n\n",figname);

  bhl_setTimeUnit(gettimeunit(befig->TIME_UNIT));
  //   Print out a concurrent assert statement for each BEMSG
  print_msg       (dotcpp, befig);

  //   Print out a concurrent signal assignment for each BEAUX
  print_aux       (dotcpp, doth, befig);

  //   Print out a block statement  with one guarded concurrent
  //   signal assignment for each BIABL of each BEREG
  reg_process     (dotcpp, doth, befig);

  //   Print out a block statement  with one guarded concurrent
  //   signal assignment for each BIABL of each BEBUX
  bux_process     (dotcpp, doth, befig);

  // Print out a block statement  with one guarded concurrent
  // signal assignment for each BIABL of each BEBUS
  bus_process     (dotcpp, doth, befig);

  //   Print out a concurrent signal assignment for each BEOUT
  print_out       (dotcpp, doth, befig);

  iprint(doth,'<',"}\n");
  iprint(doth,'<',"};\n");
  (void) fclose (doth);
  (void) fclose (dotcpp);
  bsc_freenames();
}

/*}}}************************************************************************/
/*}}}************************************************************************/
///*{{{                    Obsolete                                           */
///****************************************************************************/
///*{{{                    print_one_bebus()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void print_one_bebus(FILE *fp, bebus_list *bus, int *numprocess)
//{
//  print_one_beobj(fp,bus,numprocess,BEG_TYPE_SINGBUS);
//}
//
///*}}}************************************************************************/
///*{{{                    print_one_bebux()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void print_one_bebux(FILE *fp, bebux_list *bux, int *numprocess)
//{
//  print_one_beobj(fp,bux,numprocess,BEG_TYPE_SINGBUX);
//}
//
///*}}}************************************************************************/
///*{{{                    print_one_bereg()                                  */
///*                                                                          */
///* exploit priority encoding                                                */
///****************************************************************************/
//void print_one_bereg(FILE *fp, bereg_list *reg, int *numprocess)
//{
//    print_one_beobj(fp,reg,numprocess,BEG_TYPE_SINGREG);
//}
//
///*}}}************************************************************************/
///*{{{                    print_one_beaux()                                  */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void print_one_beaux(FILE *fp, beaux_list *aux)
//{
//  print_one_beobj(fp,  aux, NULL, BEG_TYPE_SINGAUX);
//}
//
///*}}}************************************************************************/
///*{{{                    print_one_bevectaux()                              */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void print_one_bevectaux(FILE *fp, bevectaux_list *vaux)
//{
//  print_one_beobj(fp, vaux, NULL, BEG_TYPE_VECTAUX);
//}
//
///*}}}************************************************************************/
///*{{{                    print_one_beout()                                  */
///*                                                                          */
///* affectation des signaux beout                                            */
///****************************************************************************/
//void print_one_beout(FILE *fp, beout_list *out)
//{
//  print_one_beobj(fp,  out, NULL, BEG_TYPE_SINGOUT);
//}
//
///*}}}************************************************************************/
///*{{{                    print_one_bevectout()                              */
///*                                                                          */
///*                                                                          */
///****************************************************************************/
//void print_one_bevectout(FILE *fp, bevectout_list *vout)
//{
//  print_one_beobj(fp, vout, NULL, BEG_TYPE_VECTOUT);
//}
//
///*}}}************************************************************************/
///*}}}************************************************************************/
