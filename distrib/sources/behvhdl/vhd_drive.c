/****************************************************************************/
/*                                                                          */
/* file     : vhd_drive.c                                                   */
/* date     : Oct  1999                                                     */
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
#include BVL_H
#include "bvl_utype.h"
#include "bvl_utdef.h"
#include "bvl_util.h"

/* Global variables  */

static char         *TIME_UNIT;

static int           BVL_VECT_ASS   = 0;
static int           BVL_LOOP_IND[] = {0,0,0,0,0,0,0,0,0,0,
                                       0,0,0,0,0,0,0,0,0,0,
                                       0,0,0,0,0,0,0,0,0,0};
static int           BVL_CPT_IND    = 0;

/****************************************************************************/
/*{{{                    Static function prototypes                         */
/****************************************************************************/
static void          writelist                (FILE *fp, chain_list *ptchain, int         indent);
static void          begen_treatment          (FILE *fp, befig_list *figure);

static void print_msg(FILE *fp, befig_list *figure, char **buffer, int *bufsize);
//static void print_value(FILE *fp,chain_list *abl, int time, char **buffer, int *bufsize);
static void print_withselect(FILE *fp, biabl_list *biabl, char *name, int vectsize, char **buffer, int *bufsize);
static void          print_time               (FILE *fp, int time,int,int);
static char         *vhd_bebusname            (char *name);
//static void          print_withselect         (FILE *fp, biabl_list *biabl, char *name, int vectsize);
//static void          print_value              (FILE *fp, chain_list *abl, int time);

//static void          print_affected           (FILE *fp, char indent,char *name,char *vname,loop_list *loop);
static void          print_loop               (FILE *fp, char indent,char *head,int np,int *cpt,loop_list *loop);
static void          print_endLoop            (FILE *fp,char indent,char *head,int np,int cpt, int nbsame,loop_list *loop);
static void          print_loopList           (FILE *fp,char indent,char *head,int np,int *cpt, int nbsame,loop_list *loop);
static void          print_interassign        (FILE *fp, char *dest, char *src, int mode);
//static void          print_signal             (FILE *fp, char *name, int left, int right);

static chain_list   *print_process_declaration(FILE *fp, biabl_list *biabl, char *name, int np, char *head, char **buffer,ptype_list*clock);

/*}}}************************************************************************/
/*{{{                    Utilities                                          */
/****************************************************************************/
/*{{{                    settimeunit()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void settimeunit (unsigned char time_unit_char)
{
  TIME_UNIT = bef_gettimeunit(time_unit_char);
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
  int            hpos;
  int            first  = TRUE;
  int            i;
  chain_list    *namelist = NULL;
  chain_list    *chainx;
  chain_list    *tail;

  hpos              = indent;
  for (i=0; i<indent; i++)
    indentstr[i]    = ' ';
  indentstr[i]      = '\0';

  // pre-traitement de la liste pour supprimmer les doublons
  for (; ptchain; ptchain = ptchain->NEXT)
  {
    if ( isBitStr(ptchain->DATA) )
      continue;
    beg_get_vectname(ptchain->DATA,&name,NULL,NULL,BEG_SEARCH_ABL);
    tail            = NULL;
    for (chainx=namelist;chainx;chainx=chainx->NEXT)
    {
      tail          = chainx;
      if (name == chainx->DATA)
        break;
    }
    if (!chainx)
      if (tail)
        tail->NEXT  = addchain(NULL,name);
      else
        namelist    = addchain(NULL,name);
    else
      continue;
  }

  for (chainx = namelist; chainx; chainx = chainx->NEXT)
  {
    if (!first)
      fprintf(fp, ", ");
    if (hpos > 60)
    {
      fprintf(fp, "\n%s", indentstr);
      hpos      = indent;
    }
    name        = chainx->DATA;
    hpos       += strlen(name) + 2;
    fprintf(fp,"%s",bvl_vhdlname(name));
    first       = FALSE;
  }
  freechain(namelist);
}

/*}}}************************************************************************/
/*{{{                    print_time()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_time(FILE *fp,int time,int timer, int timef)
{
  if (time)
    fprintf(fp," AFTER %u %s",time,TIME_UNIT);
}

/*}}}************************************************************************/
/*{{{                    isone()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int isone(char *txt)
{
  return !strcmp(txt,"'1'");
}

/*}}}************************************************************************/
/*{{{                    vhd_bebusname()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *vhd_bebusname(char *name)
{
  return name;
}

/*}}}************************************************************************/
/*{{{                    vhd_beregname()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *vhd_beregname(char *name)
{
  return name;
}

/*}}}************************************************************************/
/*{{{                    print_process_declaration()                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static chain_list *print_process_declaration(FILE *fp, biabl_list *biabl,
                                             char *name, int np, char *head,
                                             char **buffer,ptype_list*clock)
{
  biabl_list    *biablx;
  chain_list    *temp_expr;
  chain_list    *temp_support;

  iprint(fp,'i',"%s%d: PROCESS (",head,np);
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
  sprintf(*buffer,"%d",np);
  temp_support = supportChain_listExpr(temp_expr);
  writelist(fp, temp_support, 14 + strlen(*buffer));
  fprintf(fp,")\n");
  freechain(temp_support);
  freeExpr(temp_expr);

  return NULL;
}

/*}}}************************************************************************/
/*{{{                    Process driving                                    */
/****************************************************************************/
static void print_procBegin(FILE *fp, chain_list *loopchain)
{
  loopchain     = NULL;
  iprint(fp,'i',"BEGIN\n");
}

static void print_procEnd(FILE *fp)
{
  iprint(fp,'-',"END PROCESS;\n");
}

static void print_procIf(FILE *fp, char *buffer)
{
  iprint(fp,'+',"IF %s THEN\n",bvl_printabl(buffer));
}

static void print_procElsif(FILE *fp, char *buffer)
{
  iprint(fp,'+',"ELSIF %s THEN\n",bvl_printabl(buffer));
}

static int print_procElse(FILE *fp)
{
  iprint(fp,'+',"ELSE\n");
  
  return 1;
}

static int print_procEndif(FILE *fp, char ind)
{
  iprint(fp,ind,"END IF;\n");

  return 1;
}

static int print_edge(char *buffer, char *ckname, char ckstate,
                       char *signame)
{
  sprintf(buffer,"(%s = '%c' and %s'EVENT)",ckname,ckstate,signame);
  
  return 1;
}

/*}}}************************************************************************/
/*{{{                    Ports driving                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

static void port_begin(FILE *fp)
{
  iprint(fp,'+',"PORT (\n");
}

static void port_end(FILE *fp)
{
  fprintf(fp,"\n");
  iprint(fp,'<',");\n");
}

/*}}}************************************************************************/
/*{{{                    print_endLoop()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_endLoop(FILE *fp,char indent,char *head,int np,int cpt,
                          int nbsame,loop_list *loop)
{
  int        ind;
  
  if (loop)
  {
    if (nbsame <= 0)
    {
      ind       = BVL_LOOP_IND[-- BVL_CPT_IND];
      iprint(fp,indent,"END LOOP %s%dLOOP%d;\n", head, np, ind);
    }
    cpt --;
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
  BVL_LOOP_IND[BVL_CPT_IND ++]  = *cpt;
  *cpt  = *cpt + 1;
}

/*}}}************************************************************************/
/*{{{                    print_withselect()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_withselect(FILE *fp, biabl_list *biabl, char *name, int vectsize,
                      char **buffer, int *bufsize)
{
  char               buf[4096];
  int                cpt     = 1;

  *buffer[0] = '\0';
  *buffer    = bvl_abl2str(biabl->CNDABL,*buffer,bufsize);
  iprint(fp,'i',"WITH %s SELECT\n",bvl_printabl(*buffer));
  iprint(fp,'+',"%s <=\n",name);

  for (;biabl;biabl=biabl->NEXT, cpt ++)
  {
    if (biabl->VALABL == NULL)
      beh_toolbug(20,"print_withselect",name,0);

    *buffer[0] = '\0';
    *buffer    = bvl_abl2str(biabl->VALABL,*buffer,bufsize);
    iprint   (fp,0,"%s",bvl_printabl(*buffer));
    print_time(fp,biabl->TIME,0,0);
    if (biabl->FLAG&BEH_CND_SEL_OTHERS)
      fprintf(fp," WHEN OTHERS");
    else if (biabl->FLAG&BEH_CND_SELECT)
      fprintf(fp," WHEN %s",vhd_getBitStr(beh_getcndvectfbiabl(biabl),buf));
    else beh_toolbug(19, "print_withselect", name, 0);

    if (biabl->NEXT)
      fprintf(fp,",\n");
    else if (!(biabl->FLAG&BEH_CND_SEL_OTHERS))
    {
      fprintf(fp,",\n");
      iprint(fp,0,"");
      fprintHZBitStr(fp,genHZBitStr(vectsize));
      fprintf(fp," WHEN OTHERS;\n");
    }
    else
      fprintf(fp,";\n");
  }
}

/*}}}************************************************************************/
/*{{{                    print_value()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_value(FILE *fp, chain_list *abl, char type, int time, int timer, int timef,
                 char **buffer, int *bufsize)
{
  if (!abl)
    fprintf(fp,"?;\n");
  else
  {
    *buffer[0]  = '\0';

    if (BVL_VECT_ASS)
    {
      abl       = beg_compactAbl(copyExpr(abl));
      *buffer   = bvl_abl2str(abl,*buffer,bufsize);
      freeExpr(abl);
    }
    else
      *buffer   = bvl_abl2str(abl,*buffer,bufsize);
    fprintf(fp,"%s",bvl_printabl(*buffer));
    print_time(fp,time,0,0);
    fprintf(fp,";\n");
  }

  type          = 0;
}

/*}}}************************************************************************/
/*{{{                    print_affected()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_affected(FILE *fp, char indent, char type, int time,int timer, int timef,
                           char *name, char *vname, loop_list *loop,long specialflags)
{
  if (vname == name)
    iprint(fp,indent,"%s <= ",bvl_vectorize(name));
  else if (loop && loop->INDEX)
    iprint(fp,indent,"%s (%s) <= ",name,loop->INDEX);
  else
    iprint(fp,indent,"%s <= ",vname);

  type      = 0;
  time      = 0;
}

static char *bvl_vectorize_sub(char *name, char *buf)
{
  strcpy(buf, bvl_vectorize(name));
  return buf;                
}

/*}}}************************************************************************/
/*{{{                    print_signal()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_signal(FILE *fp, char *name, char type, int left, int right)
{
  char  *way;
  
  switch (type)
  {
    case 'P' :
    case 'R' :
    case 'r' :
    case 'W' :
         iprint(fp,'d',"SIGNAL %s : ",bvl_vhdlname(name));
         if (left != -1)
         {
           if (left >= right)
             way       = "DOWNTO";
           else
             way       = "TO";

           fprintf(fp,"STD_LOGIC_VECTOR (%d %s %d);\n",left,way,right);
         }
         else
           fprintf(fp,"STD_LOGIC;\n");
    case 'B':
    default:
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    print_por()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_por(FILE *fp, char *name, char dir, int left, int right)
{
  char          *way, *mode;
  
  switch (dir)
  {
    case 'I' :
         mode = "in";
         break;
    case 'O' : case 'Z' :
         mode = "out";
         break;
    case 'B' : case 'T' :
         mode = "inout";
         break;
    default:
         mode = "linkage";
         break;
  }
  iprint(fp,'d',"%s : %-5s ",bvl_vhdlname(name),mode);
  
  if (left != -1)
  {
    if (left >= right)
      way       = "DOWNTO";
    else
      way       = "TO";

    fprintf(fp,"STD_LOGIC_VECTOR (%d %s %d)",left,way,right);
  }
  else
    fprintf(fp,"STD_LOGIC");
}

/*}}}************************************************************************/
/*{{{                    print_por()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_interassign(FILE *fp, char *dest, char *src, int mode)
{
  return;
  fp = NULL;
  dest = NULL;
  src = NULL;
  mode = 0;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitement de la begen                             */
/****************************************************************************/
/*{{{                    begen_treatment()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void
begen_treatment(FILE *fp, befig_list *figure)
{
  begen_list     *gen;

  gen       = figure->BEGEN;

  iprint(fp,'i',NULL);
  if (gen)
  {
    iprint(fp,'+',"GENERIC (\n");
    
    for (gen = figure->BEGEN; gen->NEXT; gen = gen->NEXT)
      iprint(fp,'0',"CONSTANT  %s : %s := %ld ;\t-- %s\n",
             gen->NAME,gen->TYPE,*((long *)gen->VALUE),gen->NAME);
    
    iprint(fp,'-',"CONSTANT  %s : %s := %ld  \t-- %s\n",
           gen->NAME,gen->TYPE,*((long *)gen->VALUE),gen->NAME);
    iprint(fp,'-',"); \n");
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Traitement des messages                            */
/****************************************************************************/
/*{{{                    print_msg()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_msg(FILE *fp, befig_list *figure, char **buffer, int *bufsize)
{
//  int            bufsize = 512;
//  char          *buffer;
  bemsg_list    *msg;        /* current BEMSG pointer       */
  char          *clock;
  chain_list    *ptcondabl;

//  buffer = mbkalloc(bufsize);

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

    *buffer[0] = '\0';
    if ((isedge = bef_search_stable(msg->ABL,&clock)))
    {
      char   ckstate, *ckname;
      int    attrib;

      ckname        =  bvl_vhdlname(clock);
      ckstate       = (bef_search_notck(msg->ABL,clock) > 0) ? '0' : '1';
      attrib        = bef_get_stable_attribute(msg->ABL,&clock);
      if (attrib)
        sprintf(*buffer,"%s = '%c' and not %s'STABLE(%d %s)",
                ckname,ckstate,ckname,attrib,TIME_UNIT);
      else
        sprintf(*buffer,"%s = '%c' and %s'EVENT",ckname,ckstate,ckname);

      ptcondabl     = bef_strip_ckedge(msg->ABL,clock);
      sndckcnd      = ( ! ( ATOM(ptcondabl) &&
                        ! ( strcmp (VALUE_ATOM (ptcondabl), "'1'") &&
                            strcmp (VALUE_ATOM (ptcondabl), "'0'"))));
      if (sndckcnd)
        (void) fprintf(fp,"%s and ",bvl_printabl(*buffer));
      else
        (void) fprintf(fp,"%s",bvl_printabl(*buffer));
    }

    if (sndckcnd)
    {
      abl_red       = bef_preprocess_abl(ptcondabl);
      freeExpr(ptcondabl);
    }
    if (!isedge)
      abl_red       = msg->ABL;
    if (abl_red)
    {
      *buffer[0]     = '\0';
      *buffer        = bvl_abl2strbool(abl_red,*buffer,bufsize);
      (void) fprintf(fp,"%s",bvl_printabl(*buffer));
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
//  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    vhd_initDriver()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vhd_initDriver(bef_driver *driver, FILE *fp, befig_list *fig, ht *porht,
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
                   bvl_abl2strbool);
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
                   bvl_vhdlname,
                   beh_buildvhdvectname,
                   bvl_vectorize_sub,
                   vhd_getBitStr,
                   vhd_bebusname,
                   vhd_beregname);
  bef_driverSetPor(driver,
                   print_por,
                   port_end,
                   port_begin,
                   porht);
  driver->VLG=0;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Function de drive                                  */
/****************************************************************************/
/*{{{                    vhdsavebefig()                                     */
/*                                                                          */
/* description : print out a text file containing a data-flow VHDL          */
/*     description                                                          */
/****************************************************************************/
void
vhdsavebefig(befig_list *pthedbefig, unsigned int trace_mode)
{
  char          *suffix, *str;
  char          *buffer;
  int            bufsize;
  FILE          *fp;
  ht            *vhdportht;
  bef_driver     driver;

  BVL_VECT_ASS  = (int)(long)getenv("DRI_VECT_ASS");

  if (pthedbefig == NULL)
    beh_toolbug(10, "vhdsavebefig", NULL, 0);

  suffix = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = namealloc("vhd");

  /* ###------------------------------------------------------### */
  /*   Opening result file                                        */
  /* ###------------------------------------------------------### */

  if (!(fp = mbkfopen(bvl_vhdlname(pthedbefig->NAME),suffix,WRITE_TEXT)))
  {
    beh_error(107, NULL);
    EXIT(1);
  }

  vhdportht     = addht(40);
  
  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,pthedbefig,vhdportht,&buffer,&bufsize);

  if ((trace_mode & BVL_TRACE) != 0)
    (void) fprintf(stdout, "Saving '%s' in a vhdl file (vhd)\n",
                   pthedbefig->NAME);

  sprintf(buffer,"VHDL data flow description generated from `%s`\n",
          pthedbefig->NAME);
  avt_printExecInfo(fp, "--", buffer, "");
  
  (void) fprintf(fp, "library IEEE;\nuse IEEE.std_logic_1164.all;\n\n");

  /* ###------------------------------------------------------### */
  /*   Entity declaration                                         */
  /* ###------------------------------------------------------### */

  iprint(fp,'i',"-- Entity Declaration\n\n");
  iprint(fp,'i',"ENTITY %s IS\n",bvl_vhdlname(pthedbefig->NAME));

  /* ###------------------------------------------------------### */
  /*  begen declaration                                           */
  /* ###------------------------------------------------------### */

  begen_treatment(fp, pthedbefig);

  /* ###------------------------------------------------------### */
  /*   Port declaration                                           */
  /* ###------------------------------------------------------### */

  bef_drive_porDeclaration(&driver);

  iprint(fp,'i',"END %s;\n\n", bvl_vhdlname(pthedbefig->NAME));

  /* ###------------------------------------------------------### */
  /*   Architecture declaration                                   */
  /* ###------------------------------------------------------### */

  iprint(fp,'i',"-- Architecture Declaration\n\n");
  iprint(fp,'i',"ARCHITECTURE RTL OF %s IS\n",bvl_vhdlname(pthedbefig->NAME));

  /* ###------------------------------------------------------### */
  /*   Signals declaration                                        */
  /* ###------------------------------------------------------### */

  bef_drive_sigDeclaration(&driver);

  delht(vhdportht);
  vhdportht     = NULL;

  (void) fprintf(fp, "\nBEGIN\n");

  TIME_UNIT     = bef_gettimeunit(pthedbefig->TIME_UNIT);

  /* ###------------------------------------------------------### */
  /*   Print out a concurrent assert statement for each BEMSG     */
  /* ###------------------------------------------------------### */

  print_msg(fp,pthedbefig,&buffer,&bufsize);

  bef_drive_aux(&driver);
  bef_drive_reg(&driver);
  bef_drive_bux(&driver);
  bef_drive_bus(&driver);
  bef_drive_out(&driver);

  (void) fprintf(fp, "\nEND;\n");
  (void) fclose(fp);

  if (V_BOOL_TAB[__API_DRIVE_CORRESP].VALUE)
    bvl_drivecorresp(pthedbefig->NAME, pthedbefig, 0);

  bvl_freenames();
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Obsolete                                           */
/****************************************************************************/
/*{{{                    print_one_bebus()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_one_bebus(FILE *fp, bebus_list *bus, int *numprocess)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);
  
  bef_print_one_beobj(&driver,bus,numprocess,BEG_TYPE_SINGBUS);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_one_bebux()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_one_bebux(FILE *fp, bebux_list *bux, int *numprocess)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);

  bef_print_one_beobj(&driver,bux,numprocess,BEG_TYPE_SINGBUX);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_one_bereg()                                  */
/*                                                                          */
/* exploit priority encoding                                                */
/****************************************************************************/
void print_one_bereg(FILE *fp, bereg_list *reg, int *numprocess)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);
  
  bef_print_one_beobj(&driver,reg,numprocess,BEG_TYPE_SINGREG);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_one_beaux()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_one_beaux(FILE *fp, beaux_list *aux)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);
  
  bef_print_one_beobj(&driver,  aux, NULL, BEG_TYPE_SINGAUX);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_one_bevectaux()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_one_bevectaux(FILE *fp, bevectaux_list *vaux)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);
  
  bef_print_one_beobj(&driver, vaux, NULL, BEG_TYPE_VECTAUX);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_one_beout()                                  */
/*                                                                          */
/* affectation des signaux beout                                            */
/****************************************************************************/
void print_one_beout(FILE *fp, beout_list *out)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);

  bef_print_one_beobj(&driver,  out, NULL, BEG_TYPE_SINGOUT);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*{{{                    print_one_bevectout()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void print_one_bevectout(FILE *fp, bevectout_list *vout)
{
  bef_driver     driver;
  char          *buffer;
  int            bufsize;

  bufsize       = 512;
  buffer        = mbkalloc(bufsize);
  vhd_initDriver(&driver,fp,NULL,NULL,&buffer,&bufsize);

  bef_print_one_beobj(&driver, vout, NULL, BEG_TYPE_VECTOUT);
  mbkfree(buffer);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
