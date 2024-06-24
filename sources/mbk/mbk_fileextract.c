/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : mbk_fileextract.c                                           */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include <stdlib.h>
#include "mbk_fileextract.h"

static        int  mfe_error();

static        void newlabel(label_table **table);
static        void freelabel(label_table *label, int nbmc);
static        void newrule(extract_rules **ruleslist);
static        void freerule(extract_rules *rule);
static        void allocref(label_ref **ref);
static        int  affectlabel(label_ref *ref, char *label, double value, int match, int nbmc);
static        void duplabel(label_ref **new, label_table *tbl);
static        void freeref(label_ref **ref);
static        int  decodeline(char *buf, extract_rules *rules, label_ref *ref, int usemc);

static int  __getline(FILE *fd, char *buf);
static int  istoken(char *buf, char *token, int *offset);
static int  isvalue(char *buf, double *value, int *offset);
static int  istext(char *buf, char text[], int *offset);
static int  matchline(char *head, extract_rules *rule, label_ref *ref, int usemc);
static int  matchtoken(char *head, char *token, int *offset, char *label, double *value, char *isval);
static int  matchtableline(char *head, extract_rules *rule);

static        int  domatchvarval(label_ref *ref, extract_rules *rule);

static        int  MFE_DEBUG_MODE = 0;

#define GO2ERR(x)   do { ret = (x); goto endfunc; } while(0)

#if 0 
/*{{{                                                                       */
/*{{{                    Memory tracing                                     */
/****************************************************************************/
/*{{{                                                                       */
static void *alloc[2048][2];
static int  nballoc;

void initmemory()
{
  nballoc = 0;
  memset(alloc,0,2048 * 2 * sizeof(void *));
}

void print_alloue()
{
  int        i;
  long       res = 0;

  printf("There is %d blocs allocated\n",nballoc);
  for (i = 0; i < 2048; i ++)
    if (alloc[i][0])
      res   += (long) alloc[i][1];
  printf("It makes %ld bytes\n",res);
}

int closememory()
{
  if (nballoc != 0)
  {
    printf("All memory allocated hasn't been freed\n");
    print_alloue();
  }
  return nballoc;
}

void *alloue(int size)
{
  void      *res;
  int        i;

  if (nballoc > 2048)
  {
    printf("error\n");
    exit(1);
  }
  res   = mbkalloc(size);
  for (i = 0; i < 2048; i ++)
    if (!alloc[i][0])
    {
      alloc[i][0]   = res;
      alloc[i][1]   = (void *)size;
      break;
    }
  nballoc ++;

  return res;
}

void desalloue(void *ptr)
{
  int        i;

  for (i = 0; i < 2048; i ++)
    if (alloc[i][0] == ptr)
    {
      mbkfree(alloc[i][0]);
      alloc[i][0]   = NULL;
      alloc[i][1]   = NULL;
      break;
    }
  nballoc --;
}

char *copystr(char *str)
{
  void      *res;
  int        i;

  if (nballoc > 2048)
  {
    printf("error\n");
    exit(1);
  }
  res   = mbkstrdup(str);
  for (i = 0; i < 2048; i ++)
    if (!alloc[i][0])
    {
      alloc[i][0]   = res;
      alloc[i][1]   = (void *)strlen(str);
      break;
    }
  nballoc ++;

  return res;
}

/*}}}************************************************************************/
/*}}}************************************************************************/

static        void print_error(FILE *fd, int code);
static        void print_rule(extract_rules *rule);
static        void ruletostr(char *buf, extract_rules *rule);

/*{{{                tests functions                                        */
/*{{{                    print_error()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_error(FILE *fd, int code)
{
  fprintf(fd,"[RUL_ERR] ");
  switch (code)
  {
    case RULE_TYPE_ERR :
         fprintf(fd,"unknow type\n");
         break;
    case RULE_BEGE_ERR :
         fprintf(fd,"unable to decode begin and end\n");
         break;
    case RULE_TOOT_ERR :
         fprintf(fd,"too much token (superior to 1024)\n");
         break;
    case RULE_RULE_ERR :
         fprintf(fd,"rule problem\n");
         break;
  }
}

/*}}}************************************************************************/
/*{{{                    print_rule()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void print_rule(extract_rules *rule)
{
  int        i;

  if (rule->TYPE == RULE_TYPE_LINE)
    printf("type      : line\n");
  else if (rule->TYPE == RULE_TYPE_TABLE)
    printf("type      : table\n");
//  printf("begin     : %s\n",(rule->BEGIN) ? rule->BEGIN : "NULL");
//  printf("end       : %s\n",(rule->END)   ? rule->END : "NULL");
  printf("begin     : %s\n",rule->BEGIN);
  printf("end       : %s\n",rule->END);
  printf("nbtoken   : %d\n",rule->NBTOKEN);
  for (i = 0; i < rule->NBTOKEN; i ++)
    printf("token %3d : %s\n",i,rule->TOKEN[i]);
  printf("next      : %p\n",rule->NEXT);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testrule(int trace)
{
  extract_rules *rules_list = NULL, *rx, *tmp;
  int            res, i, nberr;
  char          *ruletab[] =
  {
    "line:/begin/end0/:# %r %t %y %u %f",
    "line://end1/:& %r %t %y %u %f"     ,
    "line:/beginend2/:* %r %t %y %u %f" ,
    "line:begin/end3/:^%r %t %y %u %f" ,
    "line:/begin/end4:u %r %t %y %u %f" ,
    "table:/begin/end5/: okri %v kflk %t" ,
    "array:/begin/end6/:%r %t %y %u %f"
  };
  int            restab[] =
  {
    0               , RULE_TYPE_LINE    , 1, 1 , 6,
    0               , RULE_TYPE_LINE    , 1, 1 , 6,
    RULE_BEGE_ERR   , 0                 , 0, 0 , 0,
    RULE_BEGE_ERR   , 0                 , 0, 0 , 0,
    RULE_BEGE_ERR   , 0                 , 0, 0 , 0,
    0               , RULE_TYPE_TABLE   , 1, 1 , 3,
    RULE_TYPE_ERR   , 0                 , 0, 0 , 0,
  };

  initmemory();
  nberr  = 0;

  if (trace) printf("-**** %s readrules ****-\n",__func__);
  for (i = 0; i < (int)(sizeof(ruletab) / sizeof(char *)); i ++)
  {
    if (trace) printf("-**************-\n");
    if (trace) printf("    %d, %s\n",i,ruletab[i]);
    if (trace) printf("-**************-\n");
    if ((res = mfe_addrule(&rules_list,ruletab[i])))
    {
      if (trace) print_error(stderr,res);
    }
    else
    {
      if (trace) print_rule(rules_list);
    }
    if (!i)
      tmp   = rules_list;
    if (res != restab[i*5]) nberr ++;
    if (!res)
    {
      if (rules_list->TYPE != restab[i*5 + 1]) nberr ++;
      if (rules_list->BEGIN && !restab[i*5 + 2]) nberr ++;
      if (!rules_list->BEGIN && restab[i*5 + 2]) nberr ++;
      if (rules_list->END && !restab[i*5 + 3]) nberr ++;
      if (!rules_list->END && restab[i*5 + 3]) nberr ++;
      if (rules_list->NBTOKEN != restab[i*5 + 4]) nberr ++;
    }
  }
  mfe_delrule(&rules_list,tmp);
  if (trace) printf("-**** %s del ****-\n",__func__);
  for (rx = rules_list; rx; rx = rx->NEXT)
  {
    if (trace) print_rule(rx);
  }
  mfe_freerules(&rules_list);

  if (trace) printf("-**** %s free ****-\n",__func__);
  if (closememory()) nberr ++;

  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testlabels(int trace)
{
  char          *labtab[] =
  {
    "sig1",
    "sig2",
    "sig3",
    "sig4",
    "sig5",
    "sig6",
    "sig7",
    "sig8",
    "sig9"
  };
  int            i;

  return 0;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testisvalue(int trace)
{
  double         res1[] = {
    0.0e0,
    1.5e10,
    -6.5e-1,
    -1855.59,
    582563258
  }, value;
  char          *test1[] = {
    "0.0e0",
    "1.5e10",
    "-6.5e-1",
    "-1855.59",
    "582563258"
  }, buf[2048];
  unsigned int   i;
  int            nberr = 0, offset, set;

  if (trace) printf("-**** %s 1 ****-\n",__func__);
  for (i = 0; i < sizeof(res1)/sizeof(double); i ++)
  {
    offset  = 0;
    if (isvalue(test1[i],&value,&offset))
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    error code\n");
      nberr++;
    }
    if (value != res1[i])
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    obtained : %g\n",value);
      nberr++;
    }
    if (offset != (int)(5+i))
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    offset : %d\n",offset);
      nberr++;
    }
  }

  if (trace) printf("-**** %s 2 ****-\n",__func__);
  buf[0]    = '\0';
  for (i = 0; i < sizeof(res1)/sizeof(double); i ++)
  {
    strcat(buf,test1[i]);
    strcat(buf," ");
  }

  offset    = 0;
  set       = 0;
  for (i = 0; i < sizeof(res1)/sizeof(double); i ++)
  {
    set     += 5 + i;
    if (isvalue(buf,&value,&offset))
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    error code\n");
      nberr++;
    }
    if (value != res1[i])
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    obtained : %g\n",value);
      nberr++;
    }
    if (offset != set)
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    offset : %d\n",offset);
      nberr++;
    }
    set ++;
  }

  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testistoken(int trace)
{
  int            nberr = 0, offset;

  if (trace) printf("-**** %s ****-\n",__func__);
  offset     = 0;
  if (istoken("test du token","test",&offset)) nberr++;
  if (istoken("test du token","du",&offset)) nberr++;
  if (istoken("test du token","token",&offset)) nberr++;

  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testistext(int trace)
{
  char          *test1[] = {
    "test15",
    "89test2",
    "test4+_4"
  }, buf[2048], test2[2048];
  int            res[] = {
    1,
    0,
    1
  };
  unsigned int   i;
  int            nberr = 0, offset, set;

  if (trace) printf("-**** %s 1 ****-\n",__func__);
  for (i = 0; i < sizeof(res)/sizeof(double); i ++)
  {
    offset  = 0;
    if (istext(test1[i],buf,&offset) && res[i])
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    error code\n");
      nberr++;
    }
    if (strcmp(test1[i],buf))
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    obtained : %s\n",buf);
      nberr++;
    }
    if (offset != (int)(6+i))
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    offset : %d\n",offset);
      nberr++;
    }
  }

  if (trace) printf("-**** %s 2 ****-\n",__func__);
  test2[0]    = '\0';
  for (i = 0; i < sizeof(res)/sizeof(double); i ++)
  {
    strcat(test2,test1[i]);
    strcat(test2," ");
  }

  offset    = 0;
  set       = 0;
  for (i = 0; i < sizeof(res)/sizeof(double); i ++)
  {
    set     += 6 + i;
    if (istext(test2,buf,&offset) && res[i])
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    error code\n");
      nberr++;
    }
    if (strcmp(buf,test1[i]))
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    obtained : %s\n",buf);
      nberr++;
    }
    if (offset != set)
    {
      if (trace) printf("    tested : %s\n",test1[i]);
      if (trace) printf("    offset : %d\n",offset);
      nberr++;
    }
    set ++;
  }

  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void filetitan(FILE * fd)
{
  fprintf(fd,
"Measure results from Transient analysis:\n"
" ==============================================================================\n"
"\n"
" --- Nominal Run, Temperature =   -30.000 (Deg C)\n"
"\n"
" Measure Statement |      Result      |   Trigger Point  |   Target Point\n"
" ------------------------------------------------------------------------------\n"
" D_RISE            |   2.24824269E-09 |   9.50000000E-10 |   3.19824269E-09\n"
" D_FALL            |   2.37524146E-09 |   1.95000000E-09 |   4.32524146E-09\n");
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void filehsp(FILE * fd)
{
  fprintf(fd,
 "$DATA1 SOURCE='HSPICE' VERSION='2002.2' \n"
 ".TITLE ' '\n"
 " d_rise d_fall temper alter#  \n"
 "  5.369e-09  5.669e-09  110.0000    1.0000\n"
          );
}

/*}}}************************************************************************/
/*{{{                    filehsp2()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void filehsp2(FILE * fd)
{
  fprintf(fd,
          "$DATA1 SOURCE='HSPICE' VERSION='U-2003.03'\n"
          ".TITLE '*e'\n"
          "sim_slope_sig129 sim_delay_sig131___sig129 sim_slope_sig131\n"
          "sim_delay_sig218___sig131 sim_slope_sig218 sim_delay_sig147___sig218\n"
          "sim_slope_sig147 sim_delay_sig216___sig147 sim_slope_sig216\n"
          "sim_delay_sig151___sig216 sim_slope_sig151 sim_delay_sig214___sig151 \n"
          "sim_slope_sig214 sim_delay_sig159___sig214 sim_slope_sig159 \n"
          "sim_delay_tran18_grid___sig159 sim_slope_tran7_drain \n"
          "sim_delay_tran7_drain___tran18_grid sim_slope_tran18_grid \n"
          "sim_delay_tran7_grid___tran7_drain sim_slope_tran15_source \n"
          "sim_delay_tran15_source___tran7_grid sim_slope_tran7_grid \n"
          "sim_delay_sig196___tran15_source sim_slope_sig196 sim_delay_sig202___sig196\n"
          "sim_slope_sig202 sim_delay_sig203___sig202 sim_delay_sig203___sig131 temper\n"
          "alter#\n"
          "5.208e-09 -1.923e-10  1.311e-08  1.070e-08  5.642e-10  3.349e-10  2.148e-09\n"
          "1.496e-09  5.225e-10  3.084e-10  2.141e-09  1.716e-09  5.709e-10  3.261e-10\n"
          "2.201e-09  1.793e-09  1.306e-09  6.667e-13  1.306e-09  5.070e-10  4.767e-09\n"
          "1.853e-12  4.768e-09  3.402e-09  9.945e-10  5.941e-10  3.596e-10  3.751e-10\n"
          "2.156e-08  125.0000    1.0000\n"
         );
}
  
/*}}}************************************************************************/
/*{{{                    testfile()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testfile(int trace)
{
  double         value;
  int            nberr, j, i;
  char          *rule[] = {
    "line:/EXTRACT INFORMATION/1****/:* %l = %v targ= %0 trig= %0",
    "line:///:%l | %v | %0 | %0",
    "table:/.TITLE ' '//:%l %v"
  };
  char          *filename[] = {
    "main.eldo.chi",
    "sim_save_titan.tr.measure",
    "main.hsp.mt0"
  };
  char          *label[2] = {
      "d_rise"        , "d_fall"
  };
  double         res[][2] = {
    { 5.3665e-09      , 5.6704e-09 },
    { 2.24824269E-09  , 2.37524146E-09 },
    { 5.369e-9        , 5.669e-09 }
  };
  int            nblabel = (int)(sizeof(label)/sizeof(char*));
  int            nbtest = (int)(sizeof(rule)/sizeof(char*));
  extract_rules *rules  = NULL;
  label_ref     *labels = NULL;

  nberr     = 0;
  initmemory();

  for (i = 0; i < nbtest; i ++)
  {
    if (trace) printf("-**** %s %s ****-\n",__func__,filename[i]);

    mfe_addrule(&rules,rule[i]);
    
    for (j = 0; j < nblabel; j ++)
      mfe_addlabel(&labels,label[j],0);

    mfe_fileextractlabel(filename[i],rules,labels,0);

    for (j = 0; j < nblabel; j ++)
      if (!mfe_labelvalue(labels,label[j],&value,-1))
      {
        if (trace) printf("    %s = %g\n",label[j],value);
        if (value - res[i][j] > 1e-12 || value - res[i][j] < -1e-12) nberr++;
      }
      else
        nberr ++;


    mfe_freelabels(&labels);
    mfe_freerules(&rules);

  }
  if (closememory()) nberr ++;
  
  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                    testfile2()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testfile2(int trace)
{
  double         value;
  int            nberr, j;
  char          *rule = "table:/.TITLE '*e'//:%l %v";
  char          *filename = "main.hsp.mt01";
  char          *label[29] = {
    "sim_slope_sig129", "sim_delay_sig131___sig129", "sim_slope_sig131", 
    "sim_delay_sig218___sig131", "sim_slope_sig218", "sim_delay_sig147___sig218", 
    "sim_slope_sig147", "sim_delay_sig216___sig147", "sim_slope_sig216", 
    "sim_delay_sig151___sig216", "sim_slope_sig151", "sim_delay_sig214___sig151", 
    "sim_slope_sig214", "sim_delay_sig159___sig214", "sim_slope_sig159", 
    "sim_delay_tran18_grid___sig159", "sim_slope_tran7_drain", 
    "sim_delay_tran7_drain___tran18_grid", "sim_slope_tran18_grid", 
    "sim_delay_tran7_grid___tran7_drain", "sim_slope_tran15_source", 
    "sim_delay_tran15_source___tran7_grid", "sim_slope_tran7_grid", 
    "sim_delay_sig196___tran15_source", "sim_slope_sig196", "sim_delay_sig202___sig196", 
    "sim_slope_sig202", "sim_delay_sig203___sig202", "sim_delay_sig203___sig131"
  };
  double         res[29] = { 
    5.208e-09 , -1.923e-10  , 1.311e-08  , 1.070e-08  , 5.642e-10  , 3.349e-10  , 2.148e-09  , 
    1.496e-09  , 5.225e-10  , 3.084e-10  , 2.141e-09  , 1.716e-09  , 5.709e-10  , 3.261e-10  , 
    2.201e-09  , 1.793e-09  , 1.306e-09  , 6.667e-13  , 1.306e-09  , 5.070e-10  , 4.767e-09  , 
    1.853e-12  , 4.768e-09  , 3.402e-09  , 9.945e-10  , 5.941e-10  , 3.596e-10  , 3.751e-10  , 
    2.156e-08
  };
  int            nblabel = (int)(sizeof(label)/sizeof(char*));
  extract_rules *rules  = NULL;
  label_ref     *labels = NULL;
  FILE          *tmpfile;

  tmpfile = fopen(filename ,"w");
  filehsp2(tmpfile);
  fclose(tmpfile);
    
  nberr     = 0;
  initmemory();

  if (trace) printf("-**** %s %s ****-\n",__func__,filename);

  mfe_addrule(&rules,rule);

  for (j = 0; j < nblabel; j ++)
  {
    //if (trace) printf("-**** adding label %s in database\n",label[j]);
    mfe_addlabel(&labels,label[j],0);
  }

  mfe_fileextractlabel(filename,rules,labels,0);

  for (j = 0; j < nblabel; j ++)
    if (!mfe_labelvalue(labels,label[j],&value,-1))
    {
      if (trace) printf("    %s = %g\n",label[j],value);
      if (value - res[j] > 1e-12 || value - res[j] < -1e-12) nberr++;
    }
    else
      nberr ++;


  mfe_freelabels(&labels);
  mfe_freerules(&rules);

  if (closememory()) nberr ++;
  
  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testglobal(int trace)
{
  double         value;
  FILE          *tmpfile;
  char           buf[2][26];
  int            nberr, i, j;
  extract_rules *rules  = NULL;
  char          *type[] = {
    "titan",
    "hsp" };
  void         (*func[])(FILE*) = {
    filetitan,
    filehsp };
  char          *rule[] = {
    "line:///:%l | %v | %0 | %0",
    "table:/.TITLE ' '//:%l %v" };
  label_ref     *labels = NULL;
  char          *label[] = {
    "d_rise",
    "d_fall"
  };
  double         res[][2] = {
    { 2.24824269E-09  , 2.37524146E-09 },
    { 5.369e-9        , 5.669e-09}
  };

  nberr     = 0;
  initmemory();

  if (trace) printf("-**** %s ****-\n",__func__);

  /* 1 - titan     */
  /* 2 - hsp       */
  for (i = 0; i < (int)(sizeof(buf)/(26*sizeof(char))); i ++)
  {
    sprintf(buf[i],"%stestfileXXXXXX",type[i]);
    mktemp(buf[i]);
    if (trace) printf("-**** %s generating testfile %s ****-\n",__func__,buf[i]);
    tmpfile = fopen(buf[i],"w");
    func[i](tmpfile);
    fclose(tmpfile);
  }

  for (i = 0; i < 2; i ++)
  {
    if (trace) printf("-**** %s starting test %s ****-\n",__func__,buf[i]);
    mfe_addrule(&rules,rule[i]);
    for (j = 0; j < 2; j ++)
      mfe_addlabel(&labels,label[j],0);

    mfe_fileextractlabel(buf[i],rules,labels,0);

    for (j = 0; j < 2; j ++)
      if (!mfe_labelvalue(labels,label[j],&value,-1))
      {
        if (trace) printf("    %s = %g\n",label[j],value);
        if (value - res[i][j] > 1e-19 || value - res[i][j] < -1e-19) nberr++;
      }
      else
        nberr ++;


    mfe_freelabels(&labels);
    mfe_freerules(&rules);

//    unlink(buf[i]);
  }
  if (closememory()) nberr ++;
  
  if (nberr) printf("%s %d errors\n",__func__,nberr);

  return nberr;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int testduplication(int trace)
{
  extract_rules *rules = NULL, *duprules, *rx;
  label_ref     *ref = NULL, *dupref;
  char          *rule[] = {
    "line:/EXTRACT INFORMATION/1****/:* %l = %v targ= %0 trig= %0",
    "line:///:%l | %v | %0 | %0",
    "table:/.TITLE ' '//:%l %v"
  },            *label[] = {
      "d_rise"        , "d_fall", "test_2" 
  };
  int            nblabel = (int)(sizeof(label)/sizeof(char*));
  int            nbrule = (int)(sizeof(rule)/sizeof(char*));

  int            i, nberr = 0;
  double         value;
  char           buf[2048];
  
  if (trace) printf("-**** %s ****-\n",__func__);
  
  initmemory();
  
  for (i = 0; i < nbrule; i++)
    mfe_addrule(&rules,rule[i]);
  for (i = 0; i < nblabel; i ++)
    mfe_addlabel(&ref,label[i],0);
  
  if (trace) printf("-**** %s affect ref ****-\n",__func__);
  for (i = 0; i < nblabel; i ++)
  {
    if (affectlabel(ref,min_namealloc(label[i]),(double)i,LABL_FOUND,0)) nberr ++;
    if (mfe_labelvalue(ref,min_namealloc(label[i]),&value,-1)) nberr ++;
    if (trace) printf("    %s = %g\n",label[i],value);
  }
 
  if (trace) printf("-**** %s affiche rules ****-\n",__func__);
  for (i = nbrule, rx = rules; i > 0; i --, rx = rx->NEXT)
  {
    ruletostr(buf,rx);
    if (trace) printf("    %s\n",buf);
    if (trace) printf("    %s\n",rule[i-1]);
    if (strcmp(rule[i-1],buf)) nberr ++; 
  }
  
  if (trace) printf("-**** %s duplication ****-\n",__func__);
  mfe_duprules(&duprules,rules);
  mfe_duplabels(&dupref,ref);
  
  if (trace) printf("-**** %s free ****-\n",__func__);
  mfe_freerules(&rules);
  mfe_freelabels(&ref);

  if (trace) printf("-**** %s affiche duprules ****-\n",__func__);
  for (i = nbrule, rx = duprules; i > 0; i --, rx = rx->NEXT)
  {
    ruletostr(buf,rx);
    if (trace) printf("    %s\n",buf);
    if (trace) printf("    %s\n",rule[i-1]);
    if (strcmp(rule[i-1],buf)) nberr ++; 
  }
  
  for (i = 0; i < nblabel; i ++)
  {
    if (mfe_labelvalue(dupref,label[i],&value,-1) != LABL_NOTFOUND)
      nberr ++;
    if (trace) printf("    %s = %g\n",label[i],value);
  }
  
  mfe_freerules(&duprules);
  mfe_freelabels(&dupref);

  if (closememory()) nberr ++;
  if (nberr) printf("%s %d errors\n",__func__,nberr);
  return nberr;
}

/*}}}************************************************************************/
/*{{{                    main()                                             */
/*                                                                          */
/* test function for mbk_fileextract                                        */
/*                                                                          */
/* />gcc -o main -L$AVT_DISTRIB_DIR/lib -Xlinker -z -Xlinker allextract \
   -lMut325 -lstdc++ -lnsl -ldl -lm -Xlinker -z -Xlinker defaultextract     */
/* />main                                                                   */
/*                                                                          */
/****************************************************************************/
int main(int argc, char *argv[])
{
  int        trace;
  int        nberr = 0;

  mbkenv();

  trace = (argc > 1);

  nberr += testrule(trace);
  nberr += testisvalue(trace);
  nberr += testistoken(trace);
  nberr += testistext(trace);
  nberr += testglobal(trace);
  nberr += testfile(trace);
  nberr += testfile2(trace);
  nberr += testduplication(trace);

  if (nberr) printf("total : %d errors\n",nberr);
  else printf("ok : %d error\n",nberr);

  return 0;
}

/*}}}************************************************************************/
/*{{{                    ruletostr()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void ruletostr(char *buf, extract_rules *rule)
{
  int        i;
  
  buf[0]    = '\0';

  if (rule->TYPE == RULE_TYPE_LINE)
    strcat(buf,"line:/");
  else if (rule->TYPE == RULE_TYPE_TABLE)
    strcat(buf,"table:/");

  strcat(buf,rule->BEGIN);
  strcat(buf,"/");
  strcat(buf,rule->END);
  strcat(buf,"/:");
  for (i = 0; i < rule->NBTOKEN; i ++)
  {
    if (i)
      strcat(buf," ");
    strcat(buf,rule->TOKEN[i]);
  }
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*}}}************************************************************************/
#else
/*{{{                                                                       */
#define alloue(size)    mbkalloc(size)
#define desalloue(ptr)  mbkfree(ptr)
#define copystr(str)    mbkstrdup(str)
#define initmemory()    
#define closememory()   0
/*}}}************************************************************************/
#endif
/****************************************************************************/
/*{{{                    Static functions                                   */
/****************************************************************************/
/*{{{                    mfe_error()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int mfe_error()
{
  fprintf(stderr,"[MFE_ERR]\n");
  EXIT(0);
  return 0;
}

/*}}}************************************************************************/
/*{{{                    newrule()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void newrule(extract_rules **ruleslist)
{
  extract_rules *neo;

  neo           = alloue(sizeof(extract_rules));
  memset(neo,0,sizeof(extract_rules));
  neo->NEXT     = *ruleslist;
  *ruleslist    = neo;
}

/*}}}************************************************************************/
/*{{{                    freerule()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void freerule(extract_rules *rule)
{
  if (rule)
  {
    int        i;

    if (rule->BEGIN)
      desalloue(rule->BEGIN);
    if (rule->END)
      desalloue(rule->END);
    for (i = 0; i < rule->NBTOKEN; i ++)
      desalloue(rule->TOKEN[i]);
    if (rule->TOKEN)
      desalloue(rule->TOKEN);
    desalloue(rule);
  }
}

/*}}}************************************************************************/
/*{{{                    newlabel()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void newlabel(label_table **table)
{
  label_table   *neo;

  neo           = alloue(sizeof(label_table));
  memset(neo,0,sizeof(label_table));
  neo->NEXT     = *table;
  *table        = neo;
}

/*}}}************************************************************************/
/*{{{                    freelabel()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void freelabel(label_table *label, int cleanmc)
{
  if (label) {
    if( cleanmc )
      mbkfree( label->VALUE.MC.TABLE );
    desalloue(label);
  }
}

/*}}}************************************************************************/
/*{{{                    allocref()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void allocref(label_ref **ref)
{
  *ref              = alloue(sizeof(label_ref));
  memset(*ref,0,sizeof(label_ref));
  (*ref)->REFTBL    = addht(50);
}

/*}}}************************************************************************/
/*{{{                    freeref()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void freeref(label_ref **ref)
{
  delht((*ref)->REFTBL);
  desalloue(*ref);
  *ref      = NULL;
}

/*}}}************************************************************************/
/*{{{                    affectlabel()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int affectlabel(label_ref *ref, char *label, double value, int match, int usemc)
{
  label_table   *lbl;
  int n;

  if ((lbl= (label_table*)gethtitem(ref->REFTBL,label)) == (void*)EMPTYHT)
    return LABL_ULBL_ERR;
  else
  {
    if (MFE_DEBUG_MODE)
      printf("[MFE_MES] storing %g for %s\n",value,label);
    if( usemc > 0 ) {
      n = lbl->VALUE.MC.NBMC ;
      if( n < lbl->VALUE.MC.MAXMC ) {
        lbl->VALUE.MC.TABLE[n] = value ;
        lbl->VALUE.MC.NBMC++ ;
      }
    }
    else
      lbl->VALUE.NUMBER = value ;
    lbl->FLAG   = match;

    return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    duplabel()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void duplabel(label_ref **new, label_table *tbl)
{
  if (tbl)
  {
    duplabel(new, tbl->NEXT);
    mfe_addlabel(new,tbl->LABEL,0);
  }
}

/*}}}************************************************************************/
/*{{{                    __getline()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int __getline(FILE *fd, char *buf)
{
  int        len;
  
  if (fgets(buf,MFE_CHAR_IN_LINE,fd))
    if ((len = strlen(buf)) != MFE_CHAR_IN_LINE - 1)
    {
      buf[len-1]    = '\0';
      len--;
      while (len>=0)
      {
        if (buf[len]<' ') buf[len]=' ';
        len--;
      }
      return 0;
    }
    else
      return MFE_LONGLINE_ERR;
  else
    return 1;
}

/*}}}************************************************************************/
/*{{{                    istext()                                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int istext(char *buf, char text[], int *offset)
{
  int        i = 0;

  while (buf[*offset] == ' ')
    if (buf[*offset] == '\0')
      return RULE_MATC_ERR;
    else
      (*offset) ++;

  if (isalpha((int)buf[*offset]))
  {
    while (buf[*offset] != ' ' && buf[*offset] != '=' && buf[*offset] != '\0')
    {
      if (text)
        text[i++]   = buf[(*offset)++];
    }
    if (text)
      text[i]       = '\0';

    return 0;
  }
  else
    return RULE_MATC_ERR;

}

/*}}}************************************************************************/
/*{{{                    istoken()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int istoken(char *buf, char *token, int *offset)
{
  int        i = 0;

  while (buf[*offset] == ' ')
    if (buf[*offset] == '\0')
      return RULE_MATC_ERR;
    else
      (*offset) ++;

  while (buf[*offset] != ' ' && buf[*offset] != '\0')
    if (token[i]=='\0') break;
    else
     if (token[i++] != buf[(*offset)++])
       return RULE_MATC_ERR;

  return 0;
}

/*}}}************************************************************************/
/*{{{                    isvalue()                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int isvalue(char *buf, double *value, int *offset)
{
  char       *endptr;
  double      valtmp;

  valtmp    = strtod(buf + (*offset)*sizeof(char), &endptr);
  if (*offset == endptr - buf)
  {
    (*offset) ++;
    return RULE_MATC_ERR;
  }
  *offset   = endptr - buf;
  if (value)
    *value  = valtmp;

  if (isnan(valtmp))
    return RULE_MATC_ERR;
  else
    return 0;
}

/*}}}************************************************************************/
/*{{{                    matchtoken()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int matchtoken(char *head, char *token, int *offset,
                             char *label, double *value, char *isval)
{
  char          *tail;
  int            res, len;

  if (token[0] == '%')
    switch (token[1])
    {
      case 'l' :
           res  = istext(head,label,offset);
           if (isval)
             isval[0]   = label[0];
           len = strlen(token);
           if (len > 2)
           {
             tail = label + strlen(label) - (len-2);
             if (!strcmp(tail, token+2))
               tail[0] = 0;
             else
               res  = RULE_UTOK_ERR;
           }
           if (MFE_DEBUG_MODE)
             printf("[MFE_MES] %s match token %s\n",label,token);
           break;
      case 'i' :
           res  = istext(head,NULL,offset);
           break;
      case 'v' :
           res  = isvalue(head,value,offset);
           if (MFE_DEBUG_MODE)
             printf("[MFE_MES] %g match token %s\n",*value,token);
           if (isval)
             isval[0]   = -1;
           break;
      case '0' :
           res  = isvalue(head,NULL,offset);
           break;
      default :
           res  = RULE_UTOK_ERR;
           break;
    }
  else
    res         = istoken(head,token,offset);

  return res;
}

/*}}}************************************************************************/
/*{{{                    matchline()                                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int matchline(char *head, extract_rules *rule, label_ref *ref, int usemc)
{
  double         value;
  char           label[1024], *name;
  int            i, offset;

  label[0]  = '\0';
  value     = 0.0;

  for (i = 0, offset = 0; i < rule->NBTOKEN; i ++)
    if (matchtoken(head,rule->TOKEN[i],&offset,label,&value,0))
      break;

  if (label[0] != '\0' && i == rule->NBTOKEN && (name = min_namefind(label)))
  {
    affectlabel(ref,name,value,LABL_FOUND,usemc);
    return 0;
  }
  else
    return RULE_FTOK_ERR;
}

/*}}}************************************************************************/
/*{{{                    matchtableline()                                   */
/*                                                                          */
/* simple revision                                                          */
/****************************************************************************/
static int matchtableline(char *head, extract_rules *rule)
{
  char          *token, label[1024];
  unsigned char  is /*what is*/;
  int            i, offset, res;
  double         value;

  token     = rule->TOKEN[rule->LINE];
  
  if (!rule->LBLLIST)
    rule->LBLLIST  = alloue(500 * sizeof(label_list));

  offset    = 0;
  for (i = 0; head[offset] != '\0'; i ++)
  {
    is       = '\0';
    if ((res = matchtoken(head,token,&offset,label,&value,&is)))
    {
      if (!rule->LINE)
        rule->NBELT1ST = rule->NBELT;
      (rule->LINE) ++;
      rule ->NBELT = 0;

      if (rule->LINE > rule->NBTOKEN-1)
      {
        rule->LINE      = 0;
        rule ->NBELT    = 0;
        rule ->NBELT1ST = 0;
        
//        printf("[MFE_WAR] Be carefull, maybe the file hasn't been generated by unix systems\n");
        
        return 1;
      }
      
      return matchtableline(head,rule);
    }
    else
    {
      switch (is)
      {
        case -1 :
             rule->LBLLIST[rule->NBELT].VALUE   = value;
        case '\0' :
             break;
        default :
             rule->LBLLIST[rule->NBELT].LABEL   = min_namefind(label);
      }
      (rule->NBELT) ++;
    }
      
    while (head[offset] == ' ')
      offset ++;
  }

  return 0;
}

/*}}}************************************************************************/
/*{{{                    domatchvarval()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int domatchvarval(label_ref *ref, extract_rules *rule)
{
  int        i;

  for (i = 0; i < rule->NBELT; i ++)
    if (rule->LBLLIST[i].LABEL)
      affectlabel(ref,rule->LBLLIST[i].LABEL,
                  rule->LBLLIST[i].VALUE,LABL_FOUND,-1);

  return 0;
}

/*}}}************************************************************************/
/*{{{                    decodeline()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static int decodeline(char *buf, extract_rules *rules, label_ref *ref, int usemc)
{
  extract_rules *rx;

  for (rx = rules; rx; rx = rx->NEXT)
  {
    if (rx->ACTIVATED != RULE_UNACTIVATED)
      if (rx->END[0] && strstr(buf,rx->END))
        rx->ACTIVATED = RULE_UNACTIVATED;
      else switch (rx->TYPE)
      {
        case RULE_TYPE_LINE :
             matchline(buf,rx,ref,usemc);
             break;
        case RULE_TYPE_TABLE :
             if (matchtableline(buf,rx))
             {
               rx->NBELT        = 0;
             }
             else if (rx->LINE == rx->NBTOKEN-1 && rx->NBELT == rx->NBELT1ST)
             {
               domatchvarval(ref,rx);
               rx->NBELT    = 0;
             }
             break;
        default :
             return RULE_TYPE_ERR;
      }
    else
      if (strstr(buf,rx->BEGIN))
        rx->ACTIVATED = RULE_ACTIVATED;
  }
  return 0;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Extern Functions                                   */
/****************************************************************************/
/*{{{                    mfe_addrule()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mfe_addrule(extract_rules **ruleslist, char *rule)
{
  int            i, nbtoken, ret = 0;
  char          *token[1024], *next, *begend[2], line[2048];
  extract_rules *res;

  // momentatly disable NEXT field
  if (!(*ruleslist))
    newrule(ruleslist);
  
  res           = *ruleslist;
  sprintf(line,"%s",rule);
  next          = line;

  /* find type */
  if (next[0] == 't' && next[1] == 'a' && next[2] == 'b' && next[3] == 'l' &&
      next[4] == 'e' && next[5] == ':')
  {
    next        = next + 6 * sizeof(char);
    res->TYPE   = RULE_TYPE_TABLE;
  }
  else if (next[0] == 'l' && next[1] == 'i' && next[2] == 'n' &&
           next[3] == 'e' && next[4] == ':')
  {
    next        = next + 5 * sizeof(char);
    res->TYPE   = RULE_TYPE_LINE;
  }
  else
    GO2ERR(RULE_TYPE_ERR);

  if (!ret)
  {
    /* find begin and end */
    for (i = 0; i < 3; i ++)
    {
      if (next[0] != '/')
        GO2ERR(RULE_BEGE_ERR);
      next[0]   = '\0';
      next      = next + sizeof(char);
      if (i >= 2)
        continue;
      begend[i] = next;
      if (!(next = strchr(next,'/')))
        GO2ERR(RULE_BEGE_ERR);
    }
    res->BEGIN  = copystr(begend[0]);
    res->END    = copystr(begend[1]);
    if (next[0] == ':')
      next      = next + sizeof(char);
    else
      GO2ERR(RULE_BEGE_ERR);

    /* find token(s) */
    nbtoken     = 0;
    if (res->TYPE == RULE_TYPE_LINE)
      for (i = 0, nbtoken = 0; next[i] != '\0'; i ++)
      {
        int      j = i;

        while (next[j] == ' ')
          next[j++] = '\0';
        if (j != i || i == 0)
        {
          i                 = j;
          token[nbtoken++]  = next + i * sizeof(char);
          if (nbtoken > 1023)
            GO2ERR(RULE_TOOT_ERR);
        }
      }
    else if (res->TYPE == RULE_TYPE_TABLE)
      for (i = 0, nbtoken = 0; next[i] != '\0'; i ++)
      {
        int        j = i;

        while (next[j] == ' ')
          j ++;
        if (next[j] == '%' || i == 0)
        {
          i                 = j;
          if (i)
            next[i-1]       = '\0';
          token[nbtoken++]  = next + i * sizeof(char);
          if (nbtoken > 1023)
            GO2ERR(RULE_TOOT_ERR);
        }
      }

    res->TOKEN      = alloue(nbtoken * sizeof(char *));
    res->NBTOKEN    = nbtoken;
    for (i = 0; i < nbtoken; i ++)
      res->TOKEN[i] = copystr(token[i]);
  }
endfunc :
  if (ret)
    mfe_delrule(ruleslist,*ruleslist);
  return ret;
}

/*}}}************************************************************************/
/*{{{                    mfe_delrule()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mfe_delrule(extract_rules **ruleslist, extract_rules *rule)
{
  extract_rules *rx, head;

  head.NEXT     = *ruleslist;
  for (rx = &head; rx->NEXT; rx = rx->NEXT)
    if (rx->NEXT == rule)
    {
      rx->NEXT      = rx->NEXT->NEXT;
      freerule(rule);
      *ruleslist    = head.NEXT;

      return 0;
    }

  return 1;
}

/*}}}************************************************************************/
/*{{{                    mfe_freerules()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mfe_freerules(extract_rules **ruleslist)
{
  if (*ruleslist)
  {
    mfe_freerules(&((*ruleslist)->NEXT));
    freerule(*ruleslist);
    *ruleslist  = NULL;
  }
}

/*}}}************************************************************************/
/*{{{                    mfe_duprules()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mfe_duprules(extract_rules **newrules, extract_rules *rules)
{
  if (rules)
  {
    int          i;
    
    mfe_duprules(newrules,rules->NEXT);
    newrule(newrules);
    (*newrules)->TYPE         = rules->TYPE;
    (*newrules)->ACTIVATED    = rules->ACTIVATED;
    (*newrules)->NBTOKEN      = rules->NBTOKEN;
    (*newrules)->TOKEN        = alloue(rules->NBTOKEN * sizeof(char *));
    for (i = 0; i < rules->NBTOKEN; i ++)
      (*newrules)->TOKEN[i] = copystr(rules->TOKEN[i]);
    (*newrules)->BEGIN        = copystr(rules->BEGIN);
    (*newrules)->END          = copystr(rules->END);
  }
  else
    *newrules   = NULL;
}

/*}}}************************************************************************/
/*{{{                    mfe_addlabel()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mfe_addlabel(label_ref **ref, char *label, int nbmc)
{
  long           res;

  label     = min_namealloc(label);
  if (!*ref)
    allocref(ref);
  res     = gethtitem((*ref)->REFTBL,label);
  if (res != EMPTYHT)
    return LABL_LEXI_WAR;
  else
  {
    label_table **tbl;

    tbl             = &((*ref)->LABTBL);
    newlabel(tbl);
    (*tbl)->LABEL   = label;
    (*tbl)->FLAG    = LABL_NOTFOUND;
    if( nbmc > 0 ) {
      (*tbl)->VALUE.MC.TABLE = (double*)mbkalloc( sizeof(double)*nbmc );
      (*tbl)->VALUE.MC.NBMC  = 0 ;
      (*tbl)->VALUE.MC.MAXMC = nbmc ;
    }
    addhtitem((*ref)->REFTBL,label,(long)*tbl);

    return 0;
  }
}

/*}}}************************************************************************/
/*{{{                    mfe_dellabel()                                     */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mfe_dellabel(label_ref **ref, char *label)
{
  label_table   *tbl, *tx, head;

  tbl   = (label_table *)gethtitem((*ref)->REFTBL,label);
  if (tbl == (label_table*)EMPTYHT)
    return LABL_ULBL_ERR;
  else
  {
    head.NEXT   = (*ref)->LABTBL;
    for (tx = &head; tx->NEXT; tx = tx->NEXT)
      if (tx->NEXT == tbl)
      {
        delhtitem((*ref)->REFTBL,label);
        freelabel(tbl,0);
        if (!head.NEXT)
          freeref(ref);
        else
          (*ref)->LABTBL    = head.NEXT;

        return 0;
      }

    return LABL_PLBL_ERR;
  }
}

/*}}}************************************************************************/
/*{{{                    mfe_freelabels()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mfe_freelabels(label_ref **ref, int cleanmc)
{
  if (*ref)
  {
    label_table   *tx, *txx;

    for (tx = (*ref)->LABTBL; tx; )
    {
      txx       = tx->NEXT;
      freelabel(tx, cleanmc);
      tx        = txx;
    }
    freeref(ref);
  }
}

/*}}}************************************************************************/
/*{{{                    mfe_duplabels()                                    */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mfe_duplabels(label_ref **table, label_ref *ref)
{
  allocref(table);
  duplabel(table,ref->LABTBL);
}

/*}}}************************************************************************/
/*{{{                    mfe_fileextractlabel()                             */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mfe_fileextractlabel(char *filename, extract_rules *rules, label_ref *ref, int nbmc)
{
  FILE          *fd;
  extract_rules *rx;
  char          *buf;
  int            usemc;

  if (!rules || !ref)
    return 1;
  else
  {
    if( nbmc > 0 )
      usemc = 1 ;
    else
      usemc = 0 ;
      
    /* init rules */
    for (rx = rules; rx; rx = rx->NEXT)
      if (!rx->BEGIN[0])
        rx->ACTIVATED = 1;
    if ((fd = mbkfopen(filename,NULL,"r")))
    {
      if (getenv("MFE_DEBUG"))
        MFE_DEBUG_MODE = 1;
      buf   = alloue(MFE_LINE_SIZE);
      /* read lines */
      while (!feof(fd) && __getline(fd,buf)!=1)
        if (decodeline(buf,rules,ref,usemc))
          break;

      /* free rules buffer if needed */
      for (rx = rules; rx; rx = rx->NEXT)
        if (rx->LBLLIST)
        {
          desalloue(rx->LBLLIST);
          rx->LBLLIST   = NULL;
        }

      desalloue(buf);
      fclose(fd);

      return 0;
    }
    else
      return 1;
  }
}

/*}}}************************************************************************/
/*{{{                    mfe_labelvalue()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int mfe_labelvalue(label_ref *ref, char *label, double *value, int idmc)
{
  long       res;

  label     = min_namealloc(label);
  if (ref && ref->REFTBL)
    if ((res = gethtitem(ref->REFTBL,label)) == EMPTYHT)
    {
      if (MFE_DEBUG_MODE)
        printf("[MFE_MES] %s doesn't match anything\n",label);

      return LABL_ULBL_ERR;
    }
    else
    {
      label_table   *table = (void *)res;

      if (table->FLAG == LABL_NOTFOUND)
        return LABL_NOTFOUND;
      else
      {
        if (value) {
          if( idmc >= 0 ) 
            *value = table->VALUE.MC.TABLE[ idmc ];
          else
            *value  = table->VALUE.NUMBER;
        }
        if (MFE_DEBUG_MODE)
          printf("[MFE_MES] %s match, result is: %g\n",label,*value);
        
        return LABL_FOUND; /* 0 */
      }
    }
  else
    return LABL_UTBL_ERR;
}

/*}}}************************************************************************/
/*{{{                    mfe_cleanLabel()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void mfe_cleanLabel(label_ref *ref)
{
  label_table   *tx;

  if (ref)
    for (tx = ref->LABTBL; tx; tx = tx->NEXT)
    {
      tx->FLAG    = LABL_NOTFOUND;
    }
}

/*}}}************************************************************************/
/*}}}************************************************************************/

int mfe_readmeasure(char *filename, void *mferules, char *rule, char *label, double *value)
{
  int err;
  extract_rules *crules  = (extract_rules *)mferules;
  label_ref     *labels = NULL;

  if (rule!=NULL) mfe_addrule(&crules,rule);
    
  mfe_addlabel(&labels,label, 0);

  mfe_fileextractlabel(filename,crules,labels,0);

  err=mfe_labelvalue(labels,label,value,-1);
          
  mfe_freelabels(&labels,0);
  if (rule!=NULL) mfe_freerules(&crules);
  
  return err;
}

static int cutword(char *buf, char **tab, int max)
{
  char *r, *res;
  int i;

  i=0;
  res=strtok_r(buf, " ", &r);
  while (res!=NULL && i<max)
    {
      tab[i]=res;
      res=strtok_r(NULL, " ", &r);
      i++;
    }
  return i;
}
void mfe_read_montecarlo_ltspice(char *filename, label_ref *ref)
{
  FILE *f;
  char *buf;
  char *tab[5];
  int i, il, lastidx;
  char *label=NULL;

  if ((f=fopen(filename,"r"))!=NULL)
    {
      buf   = alloue(MFE_LINE_SIZE);
      while (!feof(f) && __getline(f, buf)!=1)
        {
          i=cutword(buf, tab, 5);
          if (i==2 && strcasecmp(tab[0],"Measurement:")==0)
            {
              label=min_namefind(tab[1]);
              lastidx=0;
            } else if (i==0 || (i==4 && label!=NULL && strcasecmp(tab[1],label)==0 && strcasecmp(tab[2],"from")==0 && strcasecmp(tab[3],"to")==0))
              {
              } 
          else if (i==4)
            {
              il=atoi(tab[0]);
              if (il!=0)
                {
                  for (i=lastidx+1; i<il; i++)
                    affectlabel(ref,label,-1,LABL_FOUND,1); 
                  affectlabel(ref,label,atof(tab[1]),LABL_FOUND,1); 
                  lastidx=il;
                }
              else label=NULL;
            }
          else
            label=NULL;
        }
      desalloue(buf);
      fclose(f);
    }
}

