/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* 
 * Purpose : services functions and global variables
 * Date    : 06/03/92
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 */

#ident  "$Id: mbk_util.c,v 1.198 2009/05/07 14:33:06 fabrice Exp $"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include AVT_H
#include MUT_H
#include "mbk_util.h"
#include "../avt/avt_init_funcs.h"

#ifdef MALLOC_HISTOGRAM
ht_v2 *MALLOC_HISTO=NULL;
int MALLOC_HISTO_mutex=0;
unsigned long MALLOC_HOOK_TOTAL_MEM=0, MALLOC_HOOK_TOTAL_CALL=0, MALLOC_HOOK_ERRORS=0, FREE_HOOK_TOTAL_CALL=0, REALLOC_HOOK_TOTAL_CALL=0, MALLOC_HOOK_TOTAL_MEM_FROM_START=0, MALLOC_HOOK_CONSO=0;
void setup_malloc_hook();
#endif

/* Normally defined in values.h, but not available on all systems */
#ifndef BITS
#define  BITS(type)  (8 * (int)sizeof(type))
#endif

/*******************************************************************************
 * global variables                                                             *
 *******************************************************************************/
chain_list *HEAD_CHAIN = NULL;                 /* chain buffer head           */
chain_list *HEAD_BLOCK_CHAIN = NULL; 
ptype_list *HEAD_PTYPE = NULL;                 /* ptype buffer head           */
num_list *HEAD_NUM = NULL;                     /* num   buffer head           */
char TRACE_MODE  = 'N';                        /* trace if 'Y'                */
char DEBUG_MODE  = 'N';                        /* debug if 'Y'                */
char FAST_MODE   = 'Y';                        /* no consistency check if 'Y' */
char SEPAR  = '.';                             /* char used in concatenation  */
char *WORK_LIB = NULL;                         /* working directory           */
char **CATA_LIB = NULL;                        /* read only directories       */
char *CATAL = NULL;                            /* catalog file                */
/* par Fabrice le 7/2/2002 */
char IN_PARASITICS[5] = "dspf";                /* parasitic input format      */
int MBK_LOAD_PARA = 0;                   /* load parasitics enabler     */
/* -----       le 8/2/2002 */
int MBK_DEVECT = 1;                         /* devect signal name enabler  */
/* ----- */
char IN_LO[5] = "spi";                         /* input logical format        */
char IN_PH[5] = "ap";                          /* input physical format       */
char OUT_LO[5] = "spi";                        /* output logical format       */
char OUT_PH[5] = "ap";                         /* output physical format      */
long SCALE_X = 10000;                          /* distance scale definition   */
char PARSER_INFO[100] = "nothing yet";         /* version number, and so on   */
char *VDD = NULL;                              /* user name for power high    */
char *VSS = NULL;                              /* user name for power ground  */
char *GLOBAL_VDD = NULL;                       /* user name for power high    */
char *GLOBAL_VSS = NULL;                       /* user name for power ground  */
long  VDD_VSS_THRESHOLD = 0;                   /* Threshold between VDD & VSS */
char *MBK_BBOX_NAME = NULL;                /* blackbox list file name     */
char *IN_FILTER = NULL ;
char *OUT_FILTER = NULL ;
char *FILTER_SFX = NULL ;
char *LIB_FILE = NULL ;
ht *LIB_CATAL = NULL ;

char **MBK_NRD = NULL;
char **MBK_NRS = NULL;
char **MBK_DELVT0 = NULL;
char **MBK_MULU0 = NULL;
char **MBK_SA = NULL;
char **MBK_SB = NULL;
char **MBK_SD = NULL;
char **MBK_SC = NULL;
char **MBK_SCA = NULL;
char **MBK_SCB = NULL;
char **MBK_SCC = NULL;
char **MBK_NF = NULL;
char **MBK_NFING = NULL;
char **MBK_M = NULL;
char **MBK_MULT = NULL;
char **MBK_GEOMOD = NULL;
char **MBK_ABSOURCE = NULL;
char **MBK_LSSOURCE = NULL;
char **MBK_LGSOURCE = NULL;
char **MBK_ABDRAIN  = NULL;
char **MBK_LSDRAIN  = NULL;
char **MBK_LGDRAIN  = NULL;

static int namealloc_primes[]={103, 5023, 10007, 50047, 100003, 507077, 1000099, 1500823, 2015177};
static int namealloc_primes_index=4;
#ifdef BASE_STAT
long ynmsize=0;
long i_nbchain=0, i_nbptype=0, i_nbnum=0;
#endif

/* table de hash de namealloc() et namefind() */
static chain_list **NAME_HASHTABLE=NULL;//[HASHVAL];
static char buffer[BUFSIZ];                    /* buffer for namealloc strcpy */
static char str[BUFSIZ];                       /* buffer for concatname       */
static char tolowertable[1 << BITS(char)];     /* number of chars             */
static char touppertable[1 << BITS(char)];     /* number of chars             */
char CASE_SENSITIVE = 'P';                     /* namealloc case sensistive   */
char MBK_DUP_PNAME_FF='Y';
char SPI_VECTOR[1024];
char FLATTEN_KEEP_ALL_NAMES=0;

/*
** Added by Ludovic Jacomme (The slave)
** in order to "trap" exit with Graal/Dreal etc ...
*/
void (*MBK_EXIT_FUNCTION)(int) = 0;
char MBK_EXIT_KILL = 'N' ;

char *MBK_BULK_NAME,*MBK_GRID_NAME,*MBK_DRAIN_NAME,*MBK_SOURCE_NAME;

chain_list *CRYPTMOS=NULL;
chain_list *TNMOS=NULL, *TPMOS=NULL;
chain_list *DNMOS=NULL, *DPMOS=NULL;
chain_list *JFETN=NULL, *JFETP=NULL;

/* Known optional parameters */
static char *KNOWN_TRSPARAM[] = {"nrd", "$nrd", "nrs", "$nrs", "mulu0", "$mulu0", "delvto", "$delvto", "sa", "$sa", "sb", "$sb", "sd", "$sd", "nf", "$nf", "nfing", "$nfing", "m", "$m", "geomod", "$geomod", "sc", "$sc", "sca", "$sca", "scb", "$scb", "scc", "$scc", "absource", "$absource", "lssource", "$lssource", "lgsource", "$lgsource", "abdrain", "$abdrain", "lsdrain", "$lsdrain", "lgdrain", "$lgdrain", "mult", "$mult" };
static int NUM_KNOWN_TRSPARAM = 44;

static NameAllocator SENSITIVE_NAMEALLOC_ALLOCATOR;

mbk_parse_error MBK_PARSE_ERROR = { NULL, 0ul, 0ul, 0ul, 0ul, 0, 0};
chain_list *MBK_ALL_PARSE_ERROR=NULL;

char              MBK_VECTOR_OPEN[256];
char              MBK_VECTOR_CLOSE[256];
char              MBK_VECTOR_SINGLE[256];

int namealloc_ok()
{
  return NAME_HASHTABLE!=NULL;
}
void mbk_commit_errors(char *filename)
{
   mbk_parse_error *mpr;
   mpr=(mbk_parse_error *)mbkalloc(sizeof(mbk_parse_error));
   memcpy(mpr, &MBK_PARSE_ERROR, sizeof(mbk_parse_error));
   mpr->filename=sensitive_namealloc(filename);
   MBK_ALL_PARSE_ERROR=addchain(MBK_ALL_PARSE_ERROR, mpr);
   MBK_PARSE_ERROR.NB_RESI=MBK_PARSE_ERROR.NB_CAPA=MBK_PARSE_ERROR.NB_LOTRS=MBK_PARSE_ERROR.NB_NET=MBK_PARSE_ERROR.NB_INSTANCE=MBK_PARSE_ERROR.NB_DIODE=0;
}

void mbk_reset_errors()
{
  chain_list *cl;
  for (cl=MBK_ALL_PARSE_ERROR; cl!=NULL; cl=cl->NEXT) mbkfree(cl->DATA);
  freechain(MBK_ALL_PARSE_ERROR);  
  MBK_ALL_PARSE_ERROR = NULL;
}

int isknowntrsparam(void *param)
{
  if ((char **)param >= (char **)KNOWN_TRSPARAM && (char **)param < (char **)KNOWN_TRSPARAM + NUM_KNOWN_TRSPARAM) {
    return 1;
  }
  return 0;
}

char **getknowntrsparam(char *name)
{
  int i;

  for (i = 0; i < NUM_KNOWN_TRSPARAM; i+=2) {
    if (!strcasecmp(name, KNOWN_TRSPARAM[i])) return ((char **)KNOWN_TRSPARAM + i);
  }
  return NULL;
}

static void dflhandler(int);

/*******************************************************************************
 * fonction handler()                                                           *
 *******************************************************************************/
static void dflhandler(int sig)
{
  MBK_EXIT_KILL = 'N';
  sig=0; /* avoir warning */
  EXIT(100);
}

/*******************************************************************************
 * fonction mbkwaitpid()                                                        *
 * Verifie parmis les processus fils termines si pid en fait partie. Si status  *
 * est non nul, le code de retour est place dans cette variable. si mode vaut   *
 * 1, cet appel est bloquant, sinon il est non bloquant.                        *
 *******************************************************************************/
int mbkwaitpid( int pid, int mode, int *status )
{
  while(1)
    {
      if( waitpid( pid, status, WNOHANG) != 0 )
        return( 1 );

      if( mode != 1 )
        return( 0 );

      sleep(1); /* le sleep est interruptible : on ne perd rien */
    }
  
  return( 0 );
}

int mbk_decodvectorconfig( char *env )
{
  char        buf[1024] ;
  char        onechar[2] ;
  static char separ = ',';
  char        *pt ;
  int          i ;
  char        vector_open[1024];
  char        vector_close[1024];
  char        vector_single[1024];
  
  MBK_DEVECT=1;
  if( !env || strcasecmp(env,"yes")==0) {
    strcpy( MBK_VECTOR_OPEN,   "<[" );
    strcpy( MBK_VECTOR_CLOSE,  ">]" );
    strcpy( MBK_VECTOR_SINGLE, "" );
    return 1 ;
  }
  else if (strcasecmp(env,"no")==0)
  {
    MBK_DEVECT=0;
    return 1;
  }

  pt = env ;
  *vector_open   = '\0' ;
  *vector_close  = '\0' ;
  *vector_single = '\0' ;

  
  while( *pt ) {

    for( i=0 ;
         *pt && *pt != separ ;
         buf[i]=*pt, i++, pt++
       );
    buf[i]='\0';
    if( *pt )
      pt++;

    switch( i ) {
   
    case 0 :
      break ;

    case 1 :
      strcat( vector_single, buf );
      break ;
      
    case 2 :
      onechar[1] = '\0' ;
      
      onechar[0] = buf[0] ;
      strcat( vector_open, onechar );
      
      onechar[0] = buf[1] ;
      strcat( vector_close, onechar );
      break ;
    
    default :
      return 0 ;
    }
  }

  strcpy( MBK_VECTOR_OPEN,   vector_open   ) ;
  strcpy( MBK_VECTOR_CLOSE,  vector_close  ) ;
  strcpy( MBK_VECTOR_SINGLE, vector_single ) ;

  return 1 ;
}

/*******************************************************************************
 * fonction mbkenv()                                                            *
 *******************************************************************************/
void mbkenv()
{
  char *str, *env;
  char          *pttok;
  long nchar;
  struct sigaction sgct;
  sigset_t ens;
  char          buf[1024];
  int           n ;
  static int protect_mbkenv=0;
  static char MBK_RAND_SEED[] =
    {
      0x62, 0x37, 0x34, 0x30, 0x30, 0x32, 0x31, 0x38, 0x61, 0x31, 0x37, 0x34,
      0x64, 0x34, 0x64, 0x36, 0x36, 0x65, 0x32, 0x35, 0x38, 0x30, 0x34, 0x63,
      0x31, 0x36, 0x32, 0x38, 0x34, 0x65, 0x37, 0x61 
    } ;
  
  
 if (!protect_mbkenv)
  {
#ifdef MALLOC_HISTOGRAM
    setup_malloc_hook();
#endif
  }
  read_lib(); /* read the contents of MBK_WORK_LIB and MBK_CATA_LIB */

  str = getenv("MBK_DEBUG_MODE");
  if (str)
    if (!strcmp(str,"yes"))
      DEBUG_MODE = 'Y';

  str = getenv("MBK_TRACE_MODE");
  if (str)
    if (!strcmp(str,"yes"))
      TRACE_MODE = 'Y';

      FAST_MODE = V_BOOL_TAB[__MBK_FAST_MODE].VALUE?'Y':'N';

    SCALE_X = (long)V_INT_TAB[__MBK_SCALE_X].VALUE;

  srand((unsigned int) MBK_RAND_SEED);

  str = V_STR_TAB[__MBK_IN_LO].VALUE;
  if (str) {
    if (!strcmp(str, "hns"))
      (void)strcpy(IN_LO, "hns");
    else if (!strcmp(str, "fne"))
      (void)strcpy(IN_LO, "fne");
    else if (!strcmp(str, "hdn"))
      (void)strcpy(IN_LO, "hdn");
    else if (!strcmp(str, "fdn"))
      (void)strcpy(IN_LO, "fdn");
    else if (!strcmp(str, "al" ))
      (void)strcpy(IN_LO, "al");
    else if (!strcmp(str, "alx"))
      (void)strcpy(IN_LO, "alx");
    else if (!strcmp(str, "spi"))
      (void)strcpy(IN_LO, "spi");
    else if (!strcmp(str, "cir"))
      (void)strcpy(IN_LO, "cir");
    else if (!strcmp(str, "sp"))
      (void)strcpy(IN_LO, "sp");
    else if (!strcmp(str, "edi"))
      (void)strcpy(IN_LO, "edi");
    else if (!strcmp(str, "vst"))
      (void)strcpy(IN_LO, "vst");
    else if (!strcmp(str, "vhd"))
      (void)strcpy(IN_LO, "vhd");
    else if (!strcmp(str, "v"))
      (void)strcpy(IN_LO, "v");
    else if (!strcmp(str, "vlg"))
      (void)strcpy(IN_LO, "vlg");
    else {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"netlist input format '%s' not supported\n",str);
      EXIT(1);
    }
  }

  str = V_STR_TAB[__MBK_OUT_LO].VALUE;
  if (str) {
    if (!strcmp(str, "hns"))
      (void)strcpy(OUT_LO, "hns");
    else if (!strcmp(str, "fne"))
      (void)strcpy(OUT_LO, "fne");
    else if (!strcmp(str, "hdn"))
      (void)strcpy(OUT_LO, "hdn");
    else if (!strcmp(str, "fdn"))
      (void)strcpy(OUT_LO, "fdn");
    else if (!strcmp(str, "al" ))
      (void)strcpy(OUT_LO, "al");
    else if (!strcmp(str, "alx"))
      (void)strcpy(OUT_LO, "alx");
    else if (!strcmp(str, "spi"))
      (void)strcpy(OUT_LO, "spi");
    else if (!strcmp(str, "cir"))
      (void)strcpy(OUT_LO, "cir");
    else if (!strcmp(str, "sp"))
      (void)strcpy(OUT_LO, "sp");
    else if (!strcmp(str, "edi"))
      (void)strcpy(OUT_LO, "edi");
    else if (!strcmp(str, "vst"))
      (void)strcpy(OUT_LO, "vst");
    else if (!strcmp(str, "vhd"))
      (void)strcpy(OUT_LO, "vhd");
    else if (!strcmp(str, "cct"))
      (void)strcpy(OUT_LO, "cct");
    else if (!strcmp(str, "vlg"))
      (void)strcpy(OUT_LO, "vlg");
    else if (!strcmp(str, "v"))
      (void)strcpy(OUT_LO, "v");
    else {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"netlist output format '%s' not supported\n",str);
      EXIT(1);
    }
  }

  /* par Fabrice le 7/2/2002 */
      MBK_LOAD_PARA=V_BOOL_TAB[__MBK_LOAD_PARASITICS].VALUE;

  str = V_STR_TAB[__MBK_IN_PARASITICS].VALUE;
  if (str) {
    if (!strcmp(str, "spf"))
      (void)strcpy(IN_PARASITICS, "spf");
    else if (!strcmp(str, "dspf"))
      (void)strcpy(IN_PARASITICS, "dspf");
    else if (!strcmp(str, "spef"))
      (void)strcpy(IN_PARASITICS, "spef");
    else {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"parasitic netlist input format '%s' not supported\n",str);
      EXIT(1);
    }
  }
  /* --------- le 8/2/2002 */
  str = getenv("MBK_DEVECT");
  if (str) {
    if (!strcmp(str, "yes"))
      MBK_DEVECT=1;
    else if (!strcmp(str, "no"))
      MBK_DEVECT=0;
    else {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"invalid value for MBK_DEVECT, should be yes or no\n");
      EXIT(1);
    }
  }
  /* ------------ */


  str = getenv("MBK_IN_PH");
  if (str) {
    if (!strcmp(str, "cp"))
      (void)strcpy(IN_PH, "cp");
    else if (!strcmp(str, "ap"))
      (void)strcpy(IN_PH, "ap");
    else if (!strcmp(str, "mg"))
      (void)strcpy(IN_PH, "mg");
    else {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"layout input format '%s' not supported\n",str);
      EXIT(1);
    }
  }

  str = getenv("MBK_OUT_PH");
  if (str) {
    if (!strcmp(str, "cp"))
      (void)strcpy(OUT_PH, "cp");
    else if (!strcmp(str, "ap"))
      (void)strcpy(OUT_PH, "ap");
    else if (!strcmp(str, "mg"))
      (void)strcpy(OUT_PH, "mg");
    else {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"layout output format '%s' not supported\n",str);
      EXIT(1);
    }
  }

   
  if ((str = V_STR_TAB[__MBK_CATAL_NAME].VALUE)) CATAL = str;
  else CATAL = "CATAL";
   
  if ((str = V_STR_TAB[__MBK_SEPAR].VALUE)) SEPAR = *str;
   
  if ((str = V_STR_TAB[__MBK_BLACKBOX_NAME].VALUE)) MBK_BBOX_NAME = str;
  else MBK_BBOX_NAME = "BLACKBOX";
   
  if((str = V_STR_TAB[__MBK_IN_FILTER].VALUE)) IN_FILTER = str;
  else IN_FILTER = NULL;
   
  if((str = V_STR_TAB[__MBK_OUT_FILTER].VALUE)) OUT_FILTER = str;
  else OUT_FILTER = NULL;

  if((str = V_STR_TAB[__MBK_FILTER_SFX].VALUE)) FILTER_SFX = str;
  else FILTER_SFX = NULL;
   
  if((str = V_STR_TAB[__MBK_LIB_FILE].VALUE)) LIB_FILE = str;
  else LIB_FILE = NULL;
   
  if (!protect_mbkenv)
  {
     if ((str = V_STR_TAB[__MBK_CASE_SENSITIVE].VALUE)) {
       if (!strcmp(str,"yes")) 
         CASE_SENSITIVE = 'Y';
       else if (!strcmp(str,"no")) 
         CASE_SENSITIVE = 'N';
       else if (!strcmp(str,"preserve")) 
         CASE_SENSITIVE = 'P';
     }
   
     /* EXIT shall produce a real exit if not trapped */
     signal(SIGTERM, dflhandler);
  }
  /* Initialize a table of lower case characters for the machine encoding */
  for (nchar = 0; (size_t)nchar < sizeof(tolowertable); nchar++)
    tolowertable[nchar] = isupper(nchar) ? tolower(nchar) : nchar;

  /* Initialize a table of upper case characters for the machine encoding */
  for (nchar = 0; (size_t)nchar < sizeof(touppertable); nchar++)
    touppertable[nchar] = islower(nchar) ? toupper(nchar) : nchar;

  if ((str = V_STR_TAB[__MBK_VDD].VALUE)) VDD = namealloc (str);
  else VDD = namealloc ("");
   
  if ((str = V_STR_TAB[__MBK_VSS].VALUE)) VSS = namealloc (str);
  else VSS = namealloc ("");

  if ((str = V_STR_TAB[__MBK_GLOBAL_VDD].VALUE)) GLOBAL_VDD = namealloc (str);
  else GLOBAL_VDD = namealloc ("");
   
  if ((str = V_STR_TAB[__MBK_GLOBAL_VSS].VALUE)) {
    sprintf(buf, "0:*%c0:%s", SEPAR, str);
    GLOBAL_VSS = namealloc (buf);
  }
  else {
    sprintf(buf, "0:*%c0", SEPAR);
    GLOBAL_VSS = namealloc (buf);
  }

  VDD_VSS_THRESHOLD = SCALE_ALIM*V_FLOAT_TAB[__MBK_VDD_VSS_THRESHOLD].VALUE;

  /* zinaps was here */
  /* prenameallocation of often used names */
  MBK_BULK_NAME = namealloc ("bulk");
  MBK_GRID_NAME = namealloc ("grid");
  MBK_DRAIN_NAME = namealloc ("drain");
  MBK_SOURCE_NAME = namealloc ("source");

  MBK_NRD = (char **)KNOWN_TRSPARAM;
  MBK_NRS = (char **)KNOWN_TRSPARAM+2;
  MBK_MULU0 = (char **)KNOWN_TRSPARAM+4;
  MBK_DELVT0 = (char **)KNOWN_TRSPARAM+6;
  MBK_SA = (char **)KNOWN_TRSPARAM+8;
  MBK_SB = (char **)KNOWN_TRSPARAM+10;
  MBK_SD = (char **)KNOWN_TRSPARAM+12;
  MBK_NF = (char **)KNOWN_TRSPARAM+14;
  MBK_NFING = (char **)KNOWN_TRSPARAM+16;
  MBK_M = (char **)KNOWN_TRSPARAM+18;
  MBK_GEOMOD = (char **)KNOWN_TRSPARAM+20;
  MBK_SC = (char **)KNOWN_TRSPARAM+22;
  MBK_SCA = (char **)KNOWN_TRSPARAM+24;
  MBK_SCB = (char **)KNOWN_TRSPARAM+26;
  MBK_SCC = (char **)KNOWN_TRSPARAM+28;
  MBK_ABSOURCE = (char **)KNOWN_TRSPARAM+30;
  MBK_LSSOURCE = (char **)KNOWN_TRSPARAM+32;
  MBK_LGSOURCE = (char **)KNOWN_TRSPARAM+34;
  MBK_ABDRAIN  = (char **)KNOWN_TRSPARAM+36;
  MBK_LSDRAIN  = (char **)KNOWN_TRSPARAM+38;
  MBK_LGDRAIN  = (char **)KNOWN_TRSPARAM+40;
  MBK_MULT = (char **)KNOWN_TRSPARAM+42;

 if (!protect_mbkenv)
  {
    avt_init_model(&TNMOS, V_STR_TAB[__MBK_SPI_TN].VALUE);
    avt_init_model(&TPMOS, V_STR_TAB[__MBK_SPI_TP].VALUE);
    avt_init_model(&DNMOS, V_STR_TAB[__MBK_SPI_DN].VALUE);
    avt_init_model(&DPMOS, V_STR_TAB[__MBK_SPI_DP].VALUE);
  }
      MBK_DUP_PNAME_FF=V_BOOL_TAB[__MBK_DUP_PNAME_FOR_FLAT].VALUE?'Y':'N';

  if((env = V_STR_TAB[__MBK_SPI_VECTOR].VALUE)) {
    if (strlen(env) < 1024) 
      strcpy (SPI_VECTOR, env);
  }
  else strcpy (SPI_VECTOR, "_");
   
    FLATTEN_KEEP_ALL_NAMES=V_BOOL_TAB[__MBK_KEEP_ALL_SIGNAL_NAMES].VALUE;
     
    if( V_INT_TAB[__MBK_MAX_CACHE_FILE].VALUE <= 0 ) {
      fflush( stdout );
      fprintf( stderr, "*** mbk error ***\n" );
      fprintf( stderr, "bad value for MBK_MAX_CACHE_FILE.\n" );
      EXIT(1);
    }
    MBK_MAX_CACHE = V_INT_TAB[__MBK_MAX_CACHE_FILE].VALUE ;
  
  if (!mbk_decodvectorconfig(V_STR_TAB[__MBK_INPUT_VECTOR].VALUE))
  {
    fprintf( stderr, "bad value for variable controlling input vectors\n" );
  }

  avt_initlog();

  avt_LogConfig();
  
  if (!protect_mbkenv)
  {

    mbkinitautoackchld();
    /* Sous Solaris, certaines fonctions de haut niveau (fgets...) sont 
     * interruptible par la reception d'un signal. Ceci n'a jamais ete pris
     * en compte auparavant : on conserve donc explicitement le comportement
     * "normal" qui est que ces fonctions ne sont normalement pas
     * interruptible. D'ou le SA_RESTART.
     */
  
#ifdef SunOS
    signal( SIGCHLD, mbkackchld );
#else
    sigemptyset( &ens );
    sgct.sa_handler  = mbkackchld;
    sgct.sa_mask     = ens;
    sgct.sa_flags    = SA_RESTART;
    sigaction( SIGCHLD, &sgct , NULL );
#endif
  
    CreateNameAllocator(1024, &SENSITIVE_NAMEALLOC_ALLOCATOR, 'y');
  }

  protect_mbkenv=1;

  AVT_FULLVERSION = namealloc(AVT_FULLVERSION);


}

/*******************************************************************************
 * fonction readlibfile()                                                       *
 *******************************************************************************/
void readlibfile(type,fonc,reload)
     char *type ;
     void (*fonc)(char *) ;
     int reload ;
{
  FILE *file ;
  char buftype[64] ;
  char buffile[1024] ;
  int nb ;
  int nb_input ;
  chain_list *chain ;
  char *pt ;
  static chain_list *typelist = NULL ;

  if(LIB_FILE == NULL)
    return ;

  for(chain = typelist ; chain != NULL ; chain = chain->NEXT)
    {
      if(strcmp((char *)chain->DATA,type) == 0)
        break ;
    }

  if(chain != NULL) 
    {
      if(reload == 0)
        return ;
    }
  else
    {
      pt = namealloc(type) ;
      typelist = addchain(typelist,pt) ;
    }

  file = mbkfopen(LIB_FILE,NULL,"r") ;

  if(file == NULL)
    {
      if(TRACE_MODE == 'Y')
        {
          fflush(stdout);
          (void)fprintf (stderr, "*** mbk warning ***\n");
          (void)fprintf (stderr, "can not open lib file %s\n",LIB_FILE);
        }
      return ;
    }

  nb = 0;

  while(!feof(file))
    {
      nb++;
      nb_input = fscanf(file, "%s %s\n", buffile, buftype);
      if(nb_input == 0) continue;
      if(nb_input != 2)
        {
          (void)fflush(stdout);
          (void)fprintf(stderr, "*** mbk error ***\n");
          (void)fprintf(stderr, "syntax error line %d in file %s\n",nb,LIB_FILE);
          fclose(file) ;
          EXIT(1) ;
        }
      if(strcmp(buftype,type) == 0)
        fonc(buffile) ;
    }
}

/*******************************************************************************
 * function nameindex()                                                         *
 * return a string that is the concatenation of the name argument, the mbk      *
 * separator, and an index                                                      *
 *******************************************************************************/
char *nameindex(name, index)
     char *name;
     long index;
{
  char str[1024];

  (void)sprintf(str,"%s%c%ld", name, SEPAR, index);
  return namealloc(str);
}

/*******************************************************************************
 * fonction addnum()                                                            *
 * num list specialized allocator to avoid too many mallocs                     *
 *******************************************************************************/
num_list *addnum(ptnum, data)
     num_list *ptnum;
     long data;
{
  num_list *pt;
  register int i;

  #ifdef NOHEAPALLOC
  HEAD_NUM = (num_list *)mbkalloc(sizeof(num_list));
  HEAD_NUM->NEXT = NULL ;
  #else
  if (HEAD_NUM == NULL) {
    HEAD_NUM = (num_list *)mbkalloc((1+BUFSIZE)*sizeof(num_list));
    #ifdef BASE_STAT
    i_nbnum+=(1+BUFSIZE);
    #endif
    pt = HEAD_NUM;
    for (i = 1; i < BUFSIZE; i++) {
      pt->NEXT = pt + 1;
      pt++;
    }
    pt->NEXT = NULL;
  }
  #endif

  pt = HEAD_NUM;
  HEAD_NUM = HEAD_NUM->NEXT;
  pt->NEXT = ptnum;
  pt->DATA = data;
#ifdef BASE_STAT
  i_nbnum--;
#endif
  return pt;
}

/*******************************************************************************
 * fonction getnum()                                                            *
 * get pointer to num list element from data                                    *
 *******************************************************************************/
num_list *getnum(ptnum, data)
     num_list *ptnum;
     long data;
{
  num_list *pt;

  for (pt = ptnum; pt; pt = pt->NEXT) {
    if (pt->DATA == data) return pt;
  }
  return NULL;
}

/*******************************************************************************
 * function freenum()                                                           *
 * gives back freed block to the num memory allocator                           *
 *******************************************************************************/
void freenum(pt)
     num_list *pt;
{
#ifdef BASE_STAT
  {
    num_list *n;
    for (n=pt; n!=NULL; n=n->NEXT)
      i_nbnum++;
  }
#endif
#ifdef NOHEAPALLOC
  {
    num_list *next,*scan;
    for( scan=pt ; scan ; scan=next ) {
      next = scan->NEXT;
      mbkfree( scan );
    }
  }
#else
  HEAD_NUM = (num_list *)append((chain_list *)pt, (chain_list *)HEAD_NUM);
#endif
}

num_list* dupnumlst( num_list *head )
{
  num_list *reversed = NULL ;

  for( ; head ; head = head->NEXT )
    reversed = addnum( reversed, head->DATA );
  reversed = (num_list*)reverse( (chain_list*)reversed );

  return reversed ;
}
/*******************************************************************************
 * function addchain()                                                          *
 * chain list specialized allocator to avoid too many mallocs                   *
 *******************************************************************************/
chain_list *addchain(pthead, ptdata)
     chain_list *pthead;
     void *ptdata;
{
  chain_list *pt;
  register int i;

#ifdef NOHEAPALLOC
  HEAD_CHAIN = (chain_list*)mbkalloc( sizeof( chain_list ) );
  HEAD_CHAIN->NEXT = NULL;
#else
  if (HEAD_CHAIN == NULL) {
    pt = (chain_list *)mbkalloc(BUFSIZE*sizeof(chain_list));
#ifdef BASE_STAT
    i_nbchain+=BUFSIZE;
#endif
    HEAD_CHAIN = pt;
    for (i = 1; i < BUFSIZE; i++) {
      pt->NEXT = pt + 1;
      pt++;
    }
    pt->NEXT = NULL;
  }
#endif

  pt = HEAD_CHAIN;
  HEAD_CHAIN = HEAD_CHAIN->NEXT;
  pt->NEXT = pthead;
  pt->DATA = ptdata;
#ifdef BASE_STAT
  i_nbchain--;
#endif
  return pt;
}

chain_list *getchain(chain_list *ptchain, void *data)
{
  while (ptchain!=NULL)
    {
      if (ptchain->DATA == data) return ptchain;
      ptchain=ptchain->NEXT;
    }
  return NULL;
}

/*******************************************************************************
 * function freechain()                                                         *
 * gives back freed block or blocks to the chain_list memory allocator          *
 *******************************************************************************/
void freechain(pt)
     chain_list *pt;
{
  chain_list *scan;
#ifdef BASE_STAT
  for( scan=pt ; scan ; scan=scan->NEXT) i_nbchain++;
#endif
#ifdef NOHEAPALLOC
  {
    chain_list *next;
    for( scan=pt ; scan ; scan=next ) {
      next = scan->NEXT;
      mbkfree( scan );
    }
  }
#else
  HEAD_CHAIN = append(pt, HEAD_CHAIN);
#endif
}

/*******************************************************************************
 * function delchain()                                                          *
 * delete a single element of a chain_list and gives it back to freechain       *
 *******************************************************************************/
chain_list *delchain(pthead, ptdel)
     chain_list *pthead;
     chain_list *ptdel;
{
  chain_list *pt;
  chain_list *ptsav = NULL; /* To make gcc -Wall silent */

  if (pthead == NULL || ptdel == NULL) {
    (void)fflush(stdout);
    (void)fprintf(stderr,"*** mbk error ***");
    (void)fprintf(stderr,"  delchain() impossible : pointer = NULL !\n");
    EXIT(1);
  }

  if (ptdel == pthead) {
    pt = pthead->NEXT;
    pthead->NEXT = NULL;
    freechain(pthead);
    return pt;
  } else {
    for (pt = pthead; pt; pt = pt->NEXT) {
      if (pt == ptdel)
        break;
      ptsav = pt;
    }
    if (pt != NULL) {
      ptsav->NEXT = pt->NEXT;
      ptdel->NEXT = NULL;
      freechain(ptdel);
      return pthead;
    } else
      return NULL;
  }
}

/*******************************************************************************
 * function subchain()                                                          *
 * copy all elements from chain refchain to a new chain, exept elements that    *
 * appear in delchain. refchain and delchain must have the same order.          *
 * neither refchain nor delchain are altered.                                   *
 *******************************************************************************/
chain_list* subchain( chain_list *refchain, chain_list *delchain )
{
  chain_list *chain;
  chain_list *retchain=NULL;

  for( chain = refchain ; chain ; chain = chain->NEXT ) {
    if( !delchain || chain->DATA != delchain->DATA ) {
      retchain = addchain( retchain, chain->DATA );
    }
    else {
      if( delchain )
        delchain = delchain->NEXT;
    }
  }

  return reverse( retchain );
}

/*
 * renvoir le nombre d'element dans une chain_list
 */

int countchain(chain_list *doubl)
{
  int count;
  for (count=0;doubl!=NULL;doubl=doubl->NEXT,count++) ;
  return count;
}

/*******************************************************************************
 * function delchaindata()                                                      *
 * delete a single element of a chain_list and gives it back to freechain       *
 *******************************************************************************/
chain_list *delchaindata(pthead, ptdeldata)
     chain_list *pthead;
     void       *ptdeldata;
{
  chain_list *pt;
  chain_list *ptsav = NULL; /* To make gcc -Wall silent */

  if (pthead == NULL || ptdeldata == NULL) {
    (void)fflush(stdout);
    (void)fprintf(stderr,"*** mbk error ***");
    (void)fprintf(stderr,"  delchaindata() impossible : pointer = NULL !\n");
    EXIT(1);
  }

  if (ptdeldata == pthead->DATA) {
    pt = pthead->NEXT;
    pthead->NEXT = NULL;
    freechain(pthead);
    return pt;
  } else {
    for (pt = pthead; pt; pt = pt->NEXT) {
      if (pt->DATA == ptdeldata)
        break;
      ptsav = pt;
    }
    if (pt != NULL) {
      ptsav->NEXT = pt->NEXT;
      pt->NEXT = NULL;
      freechain(pt);
      return pthead;
    }
  }
  return pthead;
}

/*##------------------------------------------------------------------##*/
/*  Function : dupptypelst()                                            */
/*  contents : duplicate a ptype list and return a pointer on the new   */
/*             structure.                                               */
/*  called func. : reverse(), mbkalloc(),                               */
/*##------------------------------------------------------------------##*/

ptype_list *dupptypelst(ptype_ptr)
     ptype_list *ptype_ptr;
{
  ptype_list *ptype_rpt = NULL;         /* Returned chain pointer       */

  while(ptype_ptr != NULL)
    {
      ptype_rpt = addptype(ptype_rpt,  ptype_ptr->TYPE, ptype_ptr->DATA);
      ptype_ptr           = ptype_ptr->NEXT;
    }
  ptype_rpt = (ptype_list *)reverse((chain_list *)ptype_rpt);
  return(ptype_rpt);
}

/*##------------------------------------------------------------------##*/
/*  Function : dupchainlst()                                            */
/*  contents : duplicate a chain list and return a pointer on the new   */
/*             structure.                                               */
/*  called func. : reverse(), mbkalloc(),                               */
/*##------------------------------------------------------------------##*/

chain_list *dupchainlst(chain_ptr)
     chain_list *chain_ptr;
{
  chain_list *chain_rpt = NULL;         /* Returned chain pointer       */

  while(chain_ptr != NULL)
    {
      chain_rpt = addchain(chain_rpt, chain_ptr->DATA);
      chain_ptr           = chain_ptr->NEXT;
    }
  chain_rpt = reverse(chain_rpt);
  return(chain_rpt);
}

/*******************************************************************************
 * function addptype()                                                          *
 *******************************************************************************/
ptype_list *addptype(pthead,type,ptdata)
     ptype_list *pthead;
     long type;
     void *ptdata;
{
  ptype_list *pt;
  register int i;

  if (HEAD_PTYPE == NULL) {
#ifdef NOHEAPALLOC
    pt = (ptype_list *)mbkalloc(sizeof(ptype_list));
    HEAD_PTYPE = pt;
#else
    pt = (ptype_list *)mbkalloc(BUFSIZE * sizeof(ptype_list));
#ifdef BASE_STAT
    i_nbptype+=BUFSIZE;
#endif
    HEAD_PTYPE = pt;
    for (i = 1; i < BUFSIZE; i++) {
      pt->NEXT = pt + 1;
      pt++;
    }
#endif
    pt->NEXT = NULL;
  }

  pt = HEAD_PTYPE;
  HEAD_PTYPE = HEAD_PTYPE->NEXT;
  pt->NEXT = pthead;
  pt->DATA = ptdata;
  pt->TYPE = type;
#ifdef BASE_STAT
  i_nbptype--;
#endif
  return pt;
}
  
/*******************************************************************************
 * function testanddelptype()                                                  *
 *******************************************************************************/
ptype_list *testanddelptype(pthead, type)
     ptype_list *pthead;
     long type;
{
  ptype_list *pt;
  ptype_list *ptsav = NULL; /* To make gcc -Wall silent */

  if (pthead == NULL) return NULL;

  if (pthead->TYPE == type) {
    pt = pthead->NEXT;
    pthead->NEXT = NULL;
    freeptype(pthead);
    return pt;
  } else {
    for (pt = pthead; pt; pt = pt->NEXT) {
      if (pt->TYPE == type)
        break;
      ptsav = pt;
    }
    if (pt != NULL) {
      ptsav->NEXT = pt->NEXT;
      pt->NEXT = NULL;
      freeptype(pt);
    }
    return pthead ;
  }
  
}
/*******************************************************************************
 * function delptype()                                                          *
 *******************************************************************************/
ptype_list *delptype(pthead, type)
     ptype_list *pthead;
     long type;
{
  ptype_list *pt;
  ptype_list *ptsav = NULL; /* To make gcc -Wall silent */

  if (pthead == NULL) {
    (void)fflush(stdout);
    (void)fprintf(stderr,"*** mbk error ***\n");
    (void)fprintf(stderr,"delptype() impossible : pthead = NULL !\n");
    EXIT(1);
  }

  if (pthead->TYPE == type) {
    pt = pthead->NEXT;
    pthead->NEXT = NULL;
    freeptype(pthead);
    return pt;
  } else {
    for (pt = pthead; pt; pt = pt->NEXT) {
      if (pt->TYPE == type)
        break;
      ptsav = pt;
    }
    if (pt != NULL) {
      ptsav->NEXT = pt->NEXT;
      pt->NEXT = NULL;
      freeptype(pt);
      return pthead;
    } else
#if DELAY_DEBUG_STAT
      avt_fprintf(stderr, "¤6delptype function called for non existing PTYPE¤.\n");
      exit(55);
#endif
      return NULL;
  }
}

/*******************************************************************************
 * function freeptype()                                                         *
 *******************************************************************************/
void freeptype(pt)
     ptype_list  *pt;
{
  ptype_list *scan;
#ifdef BASE_STAT
  for( scan = pt ; scan ; scan = scan->NEXT ) i_nbptype++;
#endif
#ifdef NOHEAPALLOC
  {
    ptype_list *next;
    for( scan = pt ; scan ; scan = next ) {
      next = scan->NEXT ;
      mbkfree( scan );
    }
  }
#else
  HEAD_PTYPE = (ptype_list *)append((chain_list *)pt,(chain_list *)HEAD_PTYPE);
#endif
}

/*******************************************************************************
 * function getptype()                                                          *
 *******************************************************************************/
ptype_list *getptype(pthead, type)
     ptype_list *pthead;
     long type;
{
  ptype_list  *pt;

  for (pt = pthead; pt; pt = pt->NEXT)
    if (pt->TYPE == type)
      return pt;
  return NULL;
}

/*******************************************************************************
 * function append()                                                            *
 *******************************************************************************/
chain_list *append(pt1, pt2)
     chain_list *pt1,*pt2;   
{
  chain_list *pt;

  if (pt1 == NULL)
    return pt2;
  else { 
    for (pt = pt1; pt->NEXT; pt = pt->NEXT);
    pt->NEXT = pt2; /* append the list 2 at the end of list 1 */
    return pt1;
  }
}

/*******************************************************************************
 * Dictonnary related functions                                                 *
 *******************************************************************************/

/* Random hash function due to Don. E. Knuth, The Stanford Graph Base.
 * Truly better than the previous one from my own experimentations. */
#define HASH_MULT 314159
#define HASH_PRIME 516595003

#if 0
#define HASH_FUNC(inputname, name, code)                                     \
   do {                                                                      \
      while (*inputname) {                                                   \
         if (CASE_SENSITIVE == 'N') *name = tolowertable[(int)*inputname++]; \
         else *name = *inputname++;                                          \
         code += (code ^ (code >> 1)) + HASH_MULT * (unsigned char) *name++; \
         while (code >= HASH_PRIME)                                          \
            code -= HASH_PRIME;                                              \
      }                                                                      \
      *name = '\0';                                                          \
      code %= HASHVAL;                                                       \
   } while (0)
#endif

static inline int SUB_HASH_FUNC(char *inputname, char *name, int code, char CASE_SENSITIVE)
{
  while (*inputname) {
    if (CASE_SENSITIVE == 'N') *name = tolowertable[(int)*inputname++];
    else *name = *inputname++;
    if (CASE_SENSITIVE != 'P') code += (code ^ (code >> 1)) + HASH_MULT * (unsigned char) *name++;
    else code += (code ^ (code >> 1)) + HASH_MULT * tolowertable[(int)*name++];
    while (code >= HASH_PRIME)
      code -= HASH_PRIME;
  }
  *name = '\0';
  return code;
}

static inline int HASH_FUNC(char *inputname, char *name, int code, int HASHVAL0)
{
  code = SUB_HASH_FUNC(inputname, name, code, CASE_SENSITIVE) % HASHVAL0;
  return code;
}

/*******************************************************************************
 * function namealloc()                                                         *
 *******************************************************************************/
void CreateNameAllocator(int size, NameAllocator *na, char _case)
{
  int i;
  if (size==0) size=5100; // next rehash @10007
  na->HASHVAL0=size;
  na->NAME_HASHTABLE=mbkalloc(sizeof(chain_list *)*size);
  for (i=0; i<size; i++) na->NAME_HASHTABLE[i]=NULL;

  if (tolower(_case)=='y') na->SENSITIVE='Y';
  else if (tolower(_case)=='p') na->SENSITIVE='P'; // case preserve
  else na->SENSITIVE='N';
  
  for (na->namealloc_primes_index=0;
       (unsigned)na->namealloc_primes_index<(sizeof(namealloc_primes)/sizeof(*namealloc_primes))-1
         && namealloc_primes[na->namealloc_primes_index+1]<=size; na->namealloc_primes_index++) ;

  na->aba=CreateAdvancedBlockAllocator(4000, 'n');
#ifdef ENABLE_STATS
  na->__SIZE__=sizeof(chain_list *)*size;
  na->__NBNAMES__=0;
#endif
}

void DeleteNameAllocator(NameAllocator *na)
{
  int i;
//  chain_list *cl;
  for (i=0; i<na->HASHVAL0; i++) 
    {
      /*      for (cl=na->NAME_HASHTABLE[i]; cl!=NULL; cl=cl->NEXT)
              mbkfree(cl->DATA);*/
      if (na->NAME_HASHTABLE[i]!=NULL) freechain(na->NAME_HASHTABLE[i]);
    }
  FreeAdvancedBlockAllocator(na->aba);
  mbkfree(na->NAME_HASHTABLE);
}

static void NameAlloc_rehash(NameAllocator *na)
{
  chain_list **nn, *cl;
  int i, code;
  char *name = buffer;

  //  fprintf(stdout,".rehashing name dico: %d -> %d...",na->HASHVAL0,namealloc_primes[na->namealloc_primes_index+1]); fflush(stdout);

  nn=mbkalloc(namealloc_primes[na->namealloc_primes_index+1]*sizeof(chain_list *));
  for (i=0;i<namealloc_primes[na->namealloc_primes_index+1];i++)
    nn[i]=NULL;

  for (i=0;i<na->HASHVAL0;i++)
    {
      for (cl=na->NAME_HASHTABLE[i]; cl!=NULL; cl=cl->NEXT)
        {
          code = HASH_FUNC((char *)cl->DATA, name, 0, namealloc_primes[na->namealloc_primes_index+1]);
          nn[code]=addchain(nn[code], cl->DATA);
        }
      freechain(na->NAME_HASHTABLE[i]);
    }
  mbkfree(na->NAME_HASHTABLE);
  na->NAME_HASHTABLE=nn;
  na->HASHVAL0=namealloc_primes[na->namealloc_primes_index+1];
  na->namealloc_primes_index++;
  //  fprintf(stdout,"done\n");
}

inline char *NameAlloc_sub(NameAllocator *na, char *inputname, int find)
{
  chain_list *pt;
  char buffer[BUFSIZ];
  char *name = buffer; /* ensure no modification of parameter string */
  int code = 0, depth;
  
  if (inputname == NULL)
    return NULL;
  
  /* Beware, that's a define, ... */
  code = SUB_HASH_FUNC(inputname, name, code, na->SENSITIVE) % na->HASHVAL0;
  
  for (pt = na->NAME_HASHTABLE[code], depth=0; pt; pt = pt->NEXT, depth++)
    {
      if (na->SENSITIVE != 'P')
        {
          if (!strcmp(buffer, (char *)pt->DATA))
            return (char *)pt->DATA;
        }
      else
        {
          if (!strcasecmp(buffer, (char *)pt->DATA))
            return (char *)pt->DATA;
        }
    }
  
  if (find) return NULL;

  name=(char *)AdvancedBlockAlloc(na->aba, (unsigned int)(strlen(buffer) + 1));

  //  name = (char *)mbkalloc((unsigned int)(strlen(buffer) + 1));
  strcpy(name, buffer);
  na->NAME_HASHTABLE[code] = addchain(na->NAME_HASHTABLE[code], name);
#ifdef ENABLE_STATS
  na->__NBNAMES__++;
  na->__SIZE__+=sizeof(chain_list *)+strlen(buffer)+1+4;
#endif
  if ((unsigned)na->namealloc_primes_index<(sizeof(namealloc_primes)/sizeof(*namealloc_primes))-1 && depth>32)
    NameAlloc_rehash(na);

  return name;
}

char *NameAlloc(NameAllocator *na, char *inputname)
{
  return NameAlloc_sub(na, inputname, 0);

}
char *NameAllocFind(NameAllocator *na, char *inputname)
{
  return NameAlloc_sub(na, inputname, 1);
}

void NameAllocStat(NameAllocator *na)
{
  int i, nb, max=0,moy=0, nb0=0;
  int maxstr=0, moystr=0, lg, cntstr=0;
  chain_list *cl;

  printf("--- Nameallocator stats:\n");
  for (i=0, nb=0;i<na->HASHVAL0;i++) if (na->NAME_HASHTABLE[i]==NULL) nb++;
  printf("  %d/%d entries unused\n",nb,na->HASHVAL0);
  for (i=0, max=0;i<na->HASHVAL0;i++)
    if (na->NAME_HASHTABLE[i]!=NULL) 
      {
        nb0++;
        for (nb=0, cl=na->NAME_HASHTABLE[i];cl!=NULL; cl=cl->NEXT, nb++) 
          {
            lg=strlen((char *)cl->DATA);
            if (lg>maxstr) maxstr=lg;
            moystr+=lg;
            cntstr++;
          }
        if (nb>max) max=nb;
        moy+=nb;
      }
  printf("  chain length: mean=%.1f max=%d\n",(float)moy/(float)nb0,max);
  printf("  string length: mean=%.1f max=%d\n",(float)moystr/(float)cntstr,maxstr);
}

char *sensitive_namealloc(char *name)
{
  return NameAlloc(&SENSITIVE_NAMEALLOC_ALLOCATOR, name);
}
  
char *min_namealloc(char *name)
{
  char buf[4096], *temp;

  downstr(name, buf);
  temp=namealloc(buf);
  return temp;
}

char *min_namefind(char *name)
{
  char buf[4096], *temp;

  downstr(name, buf);
  temp=namefind(buf);
  return temp;
}

static void namealloc_rehash()
{
  chain_list **nn, *cl;
  int i, code;
  char *name = buffer;

  //  fprintf(stdout,".rehashing name dico: %d -> %d...",namealloc_primes[namealloc_primes_index],namealloc_primes[namealloc_primes_index+1]); fflush(stdout);

  nn=mbkalloc(namealloc_primes[namealloc_primes_index+1]*sizeof(chain_list *));
  for (i=0;i<namealloc_primes[namealloc_primes_index+1];i++)
    nn[i]=NULL;

  for (i=0;i<namealloc_primes[namealloc_primes_index];i++)
    {
      for (cl=NAME_HASHTABLE[i]; cl!=NULL; cl=cl->NEXT)
        {
          code = HASH_FUNC((char *)cl->DATA, name, 0, namealloc_primes[namealloc_primes_index+1]);
          nn[code]=addchain(nn[code], cl->DATA);
        }
      freechain(NAME_HASHTABLE[i]);
    }
  mbkfree(NAME_HASHTABLE);
  NAME_HASHTABLE=nn;
  namealloc_primes_index++;

  //  fprintf(stdout,"done\n");
}



char *namealloc(inputname)
     char *inputname;
{
  chain_list *pt;
  char *name = buffer; /* ensure no modification of parameter string */
  int code = 0;
  int depth;
  
  if (inputname == NULL)
    return NULL;
  
  if (NAME_HASHTABLE==NULL)
    {
      int i;
      NAME_HASHTABLE=mbkalloc(namealloc_primes[namealloc_primes_index]*sizeof(chain_list *));
      for (i=0;i<namealloc_primes[namealloc_primes_index];i++)
        NAME_HASHTABLE[i]=NULL;
    }

  /* Beware, that's a define, ... */
  code = HASH_FUNC(inputname, name, 0, namealloc_primes[namealloc_primes_index]);

  for (pt = NAME_HASHTABLE[code], depth=0; pt; pt = pt->NEXT, depth++)
    {
      if (CASE_SENSITIVE != 'P')
        {
          if (!strcmp(buffer, (char *)pt->DATA))
            return (char *)pt->DATA;
        }
      else
        {
          if (!strcasecmp(buffer, (char *)pt->DATA))
            return (char *)pt->DATA;
        }
    }

  name = (char *)mbkalloc((unsigned int)(strlen(buffer) + 1));
#ifdef BASE_STAT
  ynmsize+=strlen(buffer) + 1+4+8;
#endif
  (void)strcpy(name, buffer);
  NAME_HASHTABLE[code] = addchain(NAME_HASHTABLE[code], (void *)name);

  if ((unsigned)namealloc_primes_index<(sizeof(namealloc_primes)/sizeof(*namealloc_primes))-1 && depth>32)
    namealloc_rehash();

  return name;
}

/*******************************************************************************
 * function namefind()                                                         *
 *******************************************************************************/
char *namefind(inputname)
     char *inputname;
{
  chain_list *pt;
  char *name = buffer; /* ensure no modification of parameter string */
  register int code = 0;

  if (inputname == NULL)
    return NULL;

  if (NAME_HASHTABLE==NULL) return NULL;

  code=HASH_FUNC(inputname, name, 0, namealloc_primes[namealloc_primes_index]);

  for (pt = NAME_HASHTABLE[code]; pt; pt = pt->NEXT)
    {
      if (CASE_SENSITIVE != 'P')
        {
          if (!strcmp(buffer, (char *)pt->DATA))
            return (char *)pt->DATA;
        }
      else
        {
          if (!strcasecmp(buffer, (char *)pt->DATA))
            return (char *)pt->DATA;
        }
    }

  return NULL ; 
}

/*******************************************************************************
 * function downstr()                                                           *
 *******************************************************************************/
void downstr(s, t)
     char *s, *t;
{
  for (; *s; s++, t++)
    *t = tolowertable[(int)*s];
  *t = *s;
}

/*******************************************************************************
 * function upstr()                                                             *
 *******************************************************************************/
void upstr(s, t)
     char *s, *t;
{
  for (; *s; s++, t++)
    *t = touppertable[(int)*s];
  *t = *s;
}

/*******************************************************************************
 * function endstr() : match occurence of find at the end of string             *
 *******************************************************************************/

char *endstr(s, find)
     char *s, *find ;
{
  int ls, lf, i ;

  if (!s || !find)
    return NULL;
   
  lf = strlen(find);
  ls = strlen(s);

  if (ls < lf)
    return NULL ;

  for (i = 0 ; i < lf ; i++)
    if ((find[lf - i - 1] != s[ls - i - 1]) 
        && (find[lf - i - 1] != s[ls - i - 1] + 32)
        && (find[lf - i - 1] + 32 != s[ls - i - 1]))
      return NULL ;
   
  return find ;
}

/*******************************************************************************
 * function beginstr() : match occurence of find  at the beginnig of string     *
 *******************************************************************************/

char *beginstr (s, find, separ)
     char *s, *find, separ ;
{
  char *t ;
  int lf, ls, i ;

  if (!s || !find)
    return NULL;
  if (separ) {
    t = s ;
    if ((s = strrchr(s, separ)) == NULL)
      s = t ;
    else
      s++; /* skip the separator itself */
  }
  if ((find[0]) != 0) {
    lf = strlen (find) ;
    ls = strlen (s) ;
    if (ls < lf)
      return NULL ;
    for (i = 0 ; i < lf ; i++)
      if ((s[i] != find[i]) && (s[i] + 32 != find[i]) && (s[i] != find[i] + 32)) 
        return NULL ;
  }
  return s;
}

/*******************************************************************************
 * function wildstrstr() : match multiple strings separated by ':'              *
 * enabling wildcard '*'                                                        *
 *******************************************************************************/

int wildstrstr (char *s, char *find)
{
  char  buff[1024], *buf = buff;
  int i, j, match;

  i = 0;
  while (find[i] != '\0') {

    j = 0;
    while (find[i] != '\0' && find[i] != ':') buf[j++] = find[i++];
    buf[j] = '\0';

    if (mbk_TestREGEX(s, buf)) return 1;
    if (find[i] == ':') i++;
  }

  return 0;
  
}

/*******************************************************************************
 * function vectorize : builds a legal mbk vector from a name and an index      *
 *******************************************************************************/
char *vectorize(radical, index)
     char *radical;
     long index;
{
  (void)sprintf(str,"%s %ld", radical, index);
  return namealloc(str);
}

/*******************************************************************************
 * function vectorradical : returns the radical of an mbk vector                *
 *******************************************************************************/
char *vectorradical(name)
     char *name;
{
  char *s;
  char t;
  if ((t=name[strlen(name)-1])<'0' || t>'9') return name;
  
  s = strchr(name, ' ');
  
  if (!s)
    return name;
  
  strcpy(str, name);
  s=&str[s-name];
  
  *s = '\0';
  return namealloc(str);
}

char *mbk_VectorRadical(char *name, char *openb, char *closeb)
{
  char *closebplace;
  char findchar;
  int l;
  char str[2048];

  if (openb==NULL || closeb==NULL) openb="[(<", closeb="])>";

  l=strlen(name)-1;

  if (l<=0) return name;
  
  if (isdigit((int)(unsigned char)name[l]))
  {
      findchar=' ';
  }
  else
   if ((closebplace=strchr(closeb, name[l]))!=NULL)
    {
      findchar=*(openb-(closebplace-closeb));
    }
   else
     return name;
  
  l--;
      
  while (l>0 && isdigit((int)(unsigned char)name[l])) l--;
  
  if (l==0 || name[l]!=findchar) return name;
  
  strncpy(str, name, l);
  str[l]='\0';

  return namealloc(str);
}

int mbk_VectorIndex(char *name, char *openb, char *closeb)
{
  char *closebplace;
  char findchar;
  int l;

  if (openb==NULL || closeb==NULL) openb="[(<", closeb="])>";

  l=strlen(name)-1;

  if (l<=0) return -1;
  
  if (isdigit((int)(unsigned char)name[l]))
  {
      findchar=' ';
  }
  else
   if ((closebplace=strchr(closeb, name[l]))!=NULL)
    {
      findchar=*(openb-(closebplace-closeb));
    }
   else return -1;

  l--;
  
  while (l>0 && isdigit((int)(unsigned char)name[l])) l--;
  
  if (l==0 || name[l]!=findchar) return -1;
  
  return strtol(&name[l+1], NULL, 10);
}


/*******************************************************************************
 * function vectorindex : returns the index of an mbk vector                    *
 *******************************************************************************/
int vectorindex(name)
     char *name;
{
  char *s = strchr(name, ' ');

  if (!s)
    return -1;

  return atoi(s);
}

/*******************************************************************************
 *  generic compaison function :                                                *
 *  unlike strcmp, ensure that 10 > 2 for vectorized strings.                   *
 *  first, check strings, then check numerical values as numbers, not strings.  *
 *******************************************************************************/
int naturalstrcmp(s, t)
     char *s, *t;
{
  char *spt, *tpt , *st = s, *tt = t;
  int u, ls, lt;

  spt = buffer, tpt = str;

  while ((!isspace((int)*st)) && *st)
    *spt++ = *st++;
  *spt = '\0';

  while ((!isspace((int)*tt)) && *tt)
    *tpt++ = *tt++;
  *tpt = '\0';

  if ((u = strcmp(buffer, str)) != 0)
    return u;

  if ((ls = strlen(s)) == (lt = strlen(t)))
    return strcmp(s, t);

  return ls - lt;
}

/*******************************************************************************
 * function concatname()                                                        *
 *******************************************************************************/
char *concatname(name1, name2)
     char *name1, *name2;
{
  (void)sprintf(str,"%s%c%s", name1, SEPAR, name2);
  return namealloc(str);
}

/*******************************************************************************
 * function leftunconcatname()                                                  *
 * name="AAA.BBB.CCC.DDD"                                                       *
 * gives :                                                                      *
 * left="AAA" or NULL                                                           *
 * right="BBB.CCC.DDD"                                                          *
 *******************************************************************************/
void leftunconcatname(name, left, right)
     char *name, **left, **right;
{
  int   i;

  // On recherche le premier séparateur.

  for( i=0;
       name[i] != SEPAR && name[i]!= 0 ;
       i++ )
    str[i] = name[i];
  str[i]=0;
 
  if( name[i] != SEPAR ) {
    // Il n'y a plus de séparateur : il ne reste donc que le right.
    *right = name;
    *left  =NULL;
  } else {
    *left  = namealloc( str );  
    *right = namealloc( name+i+1 );
  }
}

/*******************************************************************************
 * function rightunconcatname()                                                 *
 * name="AAA.BBB.CCC.DDD"                                                       *
 * gives :                                                                      *
 * left="AAA.BBB.CCC"                                                           *
 * right="DDD" or NULL                                                          *
 *******************************************************************************/
void rightunconcatname(name, left, right)
     char *name, **left, **right;
{
  int   i, m;

  // Positionne sur le dernier caractère;
  for( i=0, m=-1 ; name[i] ; i++)
    if( name[i] == SEPAR )
      m=i;

  if( m == -1 ) {
    // Il n'y a plus de séparateur : il ne reste donc que le left
    *right = NULL;
    *left  = name;
  } else {
    // strlcpy( str, name, m );   pas disponible sous Solaris 5.6
    for( i=0 ; i < m ; i++ ) 
      str[i] = name[i];
    str[i] = '\0';

    *right = namealloc( name+m+1 );
    *left  = namealloc( str );  
  }
}

/*******************************************************************************
 * mbkstrdup : since brain damaged system we aim at do not have it              *
 *******************************************************************************/
char *mbkstrdup(s)
     char *s;
{
  char *t;

  if (s == NULL)
    return NULL;
  
  t = (char *)mbkalloc((unsigned int)(strlen(s) + 1));
  return strcpy(t, s);
}

int mbk_casestrcmp(char *orig, char *dest)
{
  if (CASE_SENSITIVE=='Y') return strcmp(orig, dest);
  return strcasecmp(orig, dest);
}

int mbk_charcmp(char orig, char dest)
{
  if (CASE_SENSITIVE=='Y') return orig==dest;
  return tolower(orig)==tolower(dest);
}

/*******************************************************************************
 * function reverse                                                             *
 *******************************************************************************/
chain_list *reverse(head)
     chain_list *head;
{
  chain_list *p;
  chain_list *q = (chain_list *)NULL;

  if (!head)
    return NULL;
  while ((p = head->NEXT)) {
    head->NEXT = q;
    q = head;
    head = p;
  }
  head->NEXT = q;
  return head;
}

/*******************************************************************************
 * function pstrcmp                                                             *
 * used for qsort and bsearch use for catalog sorting and acessing              *
 *******************************************************************************/
static int pstrcmp(s, t)
     char **s, **t;
{
  return strcmp(*s, *t);
}

/*******************************************************************************
 * function incatalogfeed                                                       *
 * tests if a model is present in the catalog with the F attribut               *
 *******************************************************************************/
int incatalogfeed(figname)
     char *figname;
{
  static int size;
  static char **table;

  if (!size)
    loadcatalog(&table, &size, 'F');
  return size ?
    (int)((long)bsearch(&figname, table, size, sizeof(char *), pstrcmp)) : 0;
}

/*******************************************************************************
 * function incataloggds                                                        *
 * tests if a model is present in the catalog with the G attribut               *
 *******************************************************************************/
int incataloggds(figname)
     char *figname;
{
  static int size;
  static char **table;

  if (!size)
    loadcatalog(&table, &size, 'G');
  return size ?
    (int)((long)bsearch(&figname, table, size, sizeof(char *), pstrcmp)) : 0;
}

/*******************************************************************************
 * function inlibcatalog                                                        *
 * tests if a model is present in the lib catalog with the C attribut           *
 *******************************************************************************/
int inlibcatalog(figname)
     char *figname;
{
  if(LIB_CATAL != NULL) {
    if(gethtitem(LIB_CATAL,figname) != EMPTYHT)
      return(1) ;
  }

  return(0) ;
}

/*******************************************************************************
 * function incatalog                                                           *
 * tests if a model is present in the catalog with the C attribut               *
 *******************************************************************************/
int incatalog(figname)
     char *figname;
{
  static int size;
  static char **table;

  if(LIB_CATAL != NULL) {
    if(gethtitem(LIB_CATAL,figname) != EMPTYHT)
      return(1) ;
  }

  if (!size)
    loadcatalog(&table, &size, 'C');
  return size ?
    (int)((long)bsearch(&figname, table, size, sizeof(char *), pstrcmp)) : 0;
}

/*******************************************************************************
 * function incatalogdelete                                                     *
 * tests if a model is present in the catalog with the D attribut               *
 *******************************************************************************/
int incatalogdelete(figname)
     char *figname;
{
  static int size;
  static char **table;

  if (!size)
    loadcatalog(&table, &size, 'D');
  return size ?
    (int)((long)bsearch(&figname, table, size, sizeof(char *), pstrcmp)) : 0;
}

/*******************************************************************************
 * function addcatalog                                                          *
 * add a cell in the catalog                                                    *
 *******************************************************************************/
void addcatalog(figname)
     char *figname;
{
  if(LIB_CATAL == NULL)
    {
      LIB_CATAL = addht(100) ;
    }
  sethtitem(LIB_CATAL,figname,(long)1) ;
}

chain_list *getcataloglist()
{
  if(LIB_CATAL == NULL)
  {
    LIB_CATAL = addht(100) ;
  }
  return GetAllHTKeys(LIB_CATAL);
}

void setcataloglist(chain_list *cl)
{
  if(LIB_CATAL != NULL) 
  {
    delht(LIB_CATAL);
    LIB_CATAL = NULL;
  }
  while (cl!=NULL)
  {
    addcatalog(namealloc((char *)cl->DATA));
    cl=cl->NEXT;
  }
}

/*******************************************************************************
 * function loadcatalog                                                         *
 * read the catalog from disk checking the given type                           *
 *******************************************************************************/

static void loadcatalog(table, size, type)
     char ***table;
     int *size;
     char type;
{
  void *pt;
  char attrib;
  int nb;
  int nb_input; 
  int i = 0;
  chain_list *files = (chain_list *)NULL;
  /* Tables for quick cell search :
     The catalog file is read only once, and sorted for speed.
     The later calls to loadcatalog only return the approriate table. */
  static chain_list *cells[4];
  static char **tabs[4];
  static int sizes[4];
  static int read;
 
  if (!read) {
    read++;
    (void)sprintf(buffer, "%s/%s", WORK_LIB, CATAL ? CATAL : "CATAL");
    files = addchain(files, (void *)fopen(buffer, READ_TEXT));
    while (CATA_LIB[i]) {
      (void)sprintf(buffer, "%s/CATAL", CATA_LIB[i++]);
      files = addchain(files, (void *)fopen(buffer, READ_TEXT));
    }
    /* error message :
       To avoid malloc/free on file name, the error message is output
       relatively to its order. */
    files = reverse(files);
    for (i = 0, pt = (void *)files; pt;
         pt = (void *)((chain_list *)pt)->NEXT, i++) {
      if (!((chain_list *)pt)->DATA)
        continue;
      nb = 0;
      while (!feof((FILE *)((chain_list *)pt)->DATA)) {
        nb++; /* count lines */
        nb_input = fscanf((FILE *)((chain_list *)pt)->DATA, "%s %c\n",
                          buffer, &attrib);
        if (nb_input == 0) /* skip white lines */
          continue;
        if (nb_input != 2) {
          (void)fflush(stdout);
          (void)fprintf(stderr, "*** mbk error ***\n");
          (void)fprintf(stderr, "loadcatalog syntax error line %d ", nb);
          if (i == 0)
            (void)fprintf(stderr, "in file %s/%s\n", WORK_LIB, CATAL);
          else
            (void)fprintf(stderr, "in file %s/CATAL\n", CATA_LIB[i - 1]);
          EXIT(1);
        }   
        attrib = islower((int)attrib) ? (char)toupper(attrib) : attrib;
        switch (attrib) {
        case 'C' :
          cells[0] = addchain(cells[0], namealloc(buffer));
          sizes[0]++;
          break;
        case 'G' :
          cells[1] = addchain(cells[1], namealloc(buffer));
          sizes[1]++;
          break;
        case 'F' :
          cells[2] = addchain(cells[2], namealloc(buffer));
          sizes[2]++;
          break;
        case 'D' :
          cells[3] = addchain(cells[3], namealloc(buffer));
          sizes[3]++;
          break;
        default :
          (void)fflush(stdout);
          (void)fprintf(stderr, "*** mbk error ***\n");
          (void)fprintf(stderr, "loadcatalog syntax error line %d ",
                        nb);
          if (i == 0)
            (void)fprintf(stderr, "in file %s/%s\n", WORK_LIB, CATAL);
          else
            (void)fprintf(stderr, "in file %s/CATAL\n",
                          CATA_LIB[i - 1]);
          (void)fprintf(stderr,"unknown attribut %c\n", attrib);
          EXIT(1);
        }
      }
      (void)fclose((FILE *)((chain_list *)pt)->DATA);
    }
    freechain(files);
    for (nb = 0; nb < 4; nb++) {
      if (sizes[nb]) {
        tabs[nb] = (char **)mbkalloc(sizes[nb] * sizeof(char *));
        for (i = 0, pt = (void *)cells[nb]; pt;
             pt = (void *)((chain_list *)pt)->NEXT, i++)
          tabs[nb][i] = (char *)((chain_list *)pt)->DATA;
        qsort(tabs[nb], sizes[nb], sizeof(char *), pstrcmp);
        freechain(cells[nb]);
      }
    }
  }

  switch (type) {
  case 'C' :
    *table = tabs[0];
    *size = sizes[0];
    break;
  case 'G' :
    *table = tabs[1];
    *size = sizes[1];
    break;
  case 'F' :
    *table = tabs[2];
    *size = sizes[2];
    break;
  case 'D' :
    *table = tabs[3];
    *size = sizes[3];
    break;
  }
}

/*******************************************************************************
 * function read_lib()                                                          *
 * fills an array of char * in order to have a list of names as CATA_LIB        *
 *******************************************************************************/
static void read_lib()
{
  char *str, *s, *stc, *c;
  int argc = 0, i;

  if (WORK_LIB!=NULL) mbkfree(WORK_LIB);
  str = V_STR_TAB[__MBK_WORK_LIB].VALUE;
  if (str != NULL) {
    WORK_LIB =
      (char *)mbkalloc((unsigned int)(strlen(str) + 1) * sizeof(char));
    (void)strcpy(WORK_LIB, str);
  } else { /* no specific path is given */
    WORK_LIB = (char *)mbkalloc((unsigned int)2 * sizeof(char));
    (void)strcpy(WORK_LIB, ".");
  }

  if (CATA_LIB!=NULL)
  {
     i=0;     
     while (CATA_LIB[i]) { mbkfree(CATA_LIB[i]); i++; }
     mbkfree(CATA_LIB);
  }
 
  str = V_STR_TAB[__MBK_CATA_LIB].VALUE;
  if(str != NULL) {
    char *savs;
    savs= s = (char *)mbkalloc((unsigned int)(strlen(str) + 1) * sizeof(char));
    (void)strcpy(s, str);
    str = s;   /* let's not modify the environement values */
    stc = str; /* for counting purposes */
    while (1) {
      if ((c = strchr(stc, ':')) == NULL)
        break;
      argc++;
      stc = ++c;
    }

   
    CATA_LIB = (char **)mbkalloc((unsigned int)(argc + 2) * sizeof(char *));
    argc = 0;
    while (1) {
      if ((s = strchr(str, ':')) == NULL)
        break;
      *(s++) = '\0';
      CATA_LIB[argc++] = mbkstrdup(str); /* no allocation necessary */
      str = s;
    }
    if (s == NULL)
      CATA_LIB[argc++] = (str == NULL || *str == '\0') ? NULL : mbkstrdup(str);
    CATA_LIB[argc] = NULL;
    mbkfree(savs);
  } else { /* no specific path is given */
    CATA_LIB = (char **)mbkalloc((unsigned int)2 * sizeof(char *));
    CATA_LIB[0] = mbkstrdup(".");
    CATA_LIB[1] = NULL;
  }
}
/*******************************************************************************
 * Hash tables management functions, contributed to by Luc Burgun on 20/06/92   *
 *******************************************************************************/
/*******************************************************************************
 * new dilution function for the table accesses. G. Augustins 18.XII.2001       *
 *******************************************************************************/

unsigned long hash(p)
     void *p;
{
  unsigned long key ;
  unsigned long bit0 = (long)p & (long)0xff ;
  unsigned long bit1 = ((long)p >> 8) & (long)0xff ;
  unsigned long bit2 = ((long)p >> 16) & (long)0xff ;
  unsigned long bit3 = ((long)p >> 24) & (long)0xff ;
  unsigned long key0 ;
  unsigned long key1 ;
  unsigned long key2 ;
  unsigned long key3 ;

  key3 = bit2 ^ bit0 ;
  key2 = bit1 ^ bit3 ;
  key1 = bit3 ^ bit2 ;
  key0 = bit1 ^ bit2 ;

  key = (key3 << 24) | (key2 << 16) | (key1 << 8) | key0 ;

  return (key) ;
}

/*******************************************************************************
 * function addht, create a hash table                                          *
 *******************************************************************************/
ht *addht(len)
     unsigned long len;
{
  ht *pTable;
  htitem *pEl;
  unsigned int i;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return (ht*)addht_v2(len);

  if (len == 0) {
    fflush(stdout);
    (void)fprintf(stderr, "*** mbk error ***\n");
    (void)fprintf(stderr, "addht impossible : hash table size is '0'\n");
    EXIT(1);
  }
  pTable = (ht *)mbkalloc(sizeof(struct htable));
  pTable->length = len;
  pEl = (htitem *)mbkalloc(len * (sizeof(struct htitem)));
  pTable->pElem = pEl;
  for (i = 0; i < len; i++) {
    pEl[i].key = NULL;
    pEl[i].value = EMPTYHT;
  }
  pTable->count = 0;
  return pTable;
}

ht *dupht(ht *orig)
{
  ht *dest;
  //htitem *hi;
  int i;
  
  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return (ht*)dupht_v2((ht_v2*)orig);
  
  dest=(ht *)mbkalloc(sizeof(ht));
  memcpy(dest, orig, sizeof(ht));
  dest->pElem=(htitem *)mbkalloc(sizeof(htitem)*orig->length);
  for (i=0; i<orig->length; i++)
    memcpy(&dest->pElem[i], &orig->pElem[i], sizeof(htitem));
  //  memcpy(dest->pElem, orig->pElem, sizeof(htitem)*orig->length);
  return dest;
}
/*******************************************************************************
 * function delht, delete a hash table                                          *
 *******************************************************************************/
void delht(pTable)
     ht *pTable;
{
  htitem * pEl;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE ) {
    delht_v2((ht_v2*)pTable);
    return ;
  }
    
  if (pTable == NULL) return;
  pEl = pTable->pElem;
  mbkfree(pEl);
  mbkfree(pTable);
}

/*******************************************************************************
 * function gethtitem, get an element in a hash table                            *
 *******************************************************************************/
long gethtitem(pTable, key)
     ht *pTable;
     void *key;
{
  long co = 0;
  long indice = 0;
  htitem * pEl;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return gethtitem_v2( (ht_v2*)pTable, key );

  indice = hash(key) % pTable->length;
  do {
    if (co++ > HMAX_CALLS) {
      if ((pTable->count > (pTable->length) * 2 / 10) || (co >= pTable->length)) {
        reallocht(pTable);
        return  gethtitem(pTable, key);
      }
    }

    pEl = (pTable->pElem) + indice;
    if (pEl->value != EMPTYHT && pEl->value != DELETEHT) {
      if ((long) key == (long) pEl->key)
        return pEl->value;
    } else if (pEl->value == EMPTYHT)
      return EMPTYHT;
    indice = (indice + 1) % pTable->length;
  } while (1);
}

/*******************************************************************************
 * function addhtitem, get an element in a hash table                            *
 *******************************************************************************/
long addhtitem(pTable, key, value)
     ht *pTable;
     void *key;
     long value;
{
  int indice = 0;
  htitem *pEl, *FirstDEL=NULL;
  int co = 0;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return addhtitem_v2( (ht_v2*)pTable, key, value );

  if (value == EMPTYHT || value == DELETEHT) {
    fflush(stdout);
    (void)fprintf(stderr, "*** mbk error ***\n");
    (void)fprintf(stderr, "addhtitem impossible : value is EMPTYHT or DELETEHT\n");
    EXIT(1);
  }
  if (pTable->count++ > (pTable->length) * 8 / 10) {
    reallocht(pTable);
    return addhtitem(pTable, key, value);
  }

  indice = hash(key) % pTable->length;
  do {
    if (co++ > HMAX_CALLS) { 
      if (pTable->count > (pTable->length) * 2 / 10 || (co >= pTable->length)) {
        reallocht(pTable);
        return addhtitem(pTable, key, value);
      } else if(TRACE_MODE == 'Y') {
        fflush(stdout);
        (void)fprintf (stderr, "*** mbk warning ***\n");
        (void)fprintf (stderr, "reallocht undone : density is too low\n");
      }
    }
    pEl = (pTable->pElem) + indice;
    if (FirstDEL==NULL && pEl->value == DELETEHT) FirstDEL=pEl;
    else if (pEl->value == EMPTYHT) {
      if (FirstDEL!=NULL) pEl=FirstDEL;
      pEl->value = value;
      pEl->key = key;
      return value;
    } else if ((long) pEl->key == (long) key) {
      pTable->count--;
      pEl->value = value;
      return value;
    }
    indice = (indice + 1) % pTable->length;
  } while (1);
}

void resetht_v2(ht_v2 *h)
{
  int i;
  for (i=0; i<h->size; i++) h->ITEM_HASHTABLE[i]=NULL;
  DeleteHeap(&h->ha);
}

void resetht(ht *pTable)
{
  int i;
  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
     resetht_v2((ht_v2*)pTable);
  for (i=0; i<pTable->length; i++)
  {
    pTable->pElem[i].key = NULL;
    pTable->pElem[i].value = EMPTYHT;
  }
  pTable->count = 0;
}

ht* controlled_dupht( ht *orig, long (*func)( long value, void *user_data ), void *user_data )
{
  ht  *h ;
  int  i ;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return (ht*)controlled_dupht( orig, func, user_data );  

  h = dupht( orig ) ;
  for( i=0 ; i<h->length ; i++ )
    h->pElem[i].value = func( h->pElem[i].value, user_data );

  return h ;
}

ht_v2* controlled_dupht_v2( ht_v2 *orig, long (*func)( long value, void *user_data ), void *user_data )
{
  ht_v2     *h ;
  htitem_v2 *item ;
  int        i ;

  h = dupht_v2( orig ) ;
  for( i=0 ; i<h->size ; i++ ) {
    for( item = h->ITEM_HASHTABLE[i] ; item ; item = item->next ) {
      item->value = func( item->value, user_data );
    }
  }

  return h ;
}

long controlled_addhtitem(ht *pTable, void *key, long (*func)(int newone, long old_value, void *user_data), void *user_data)
{
  int indice = 0;
  htitem *pEl, *FirstDEL=NULL;
  int co = 0;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return controlled_addhtitem_v2( (ht_v2*)pTable, key, func, user_data );

  if (pTable->count++ > (pTable->length) * 8 / 10) {
    reallocht(pTable);
    return controlled_addhtitem(pTable, key, func, user_data);
  }
  
  indice = hash(key) % pTable->length;
  do {
    if (co++ > HMAX_CALLS) { 
      if (pTable->count > (pTable->length) * 2 / 10 || (co >= pTable->length)) {
        reallocht(pTable);
        return controlled_addhtitem(pTable, key, func, user_data);
      } else if(TRACE_MODE == 'Y') {
        fflush(stdout);
        (void)fprintf (stderr, "*** mbk warning ***\n");
        (void)fprintf (stderr, "reallocht undone : density is too low\n");
      }
    }
    pEl = (pTable->pElem) + indice;
    if (FirstDEL==NULL && pEl->value == DELETEHT) FirstDEL=pEl;
    else if (pEl->value == EMPTYHT) {
      if (FirstDEL!=NULL) pEl=FirstDEL;
      pEl->value = func(1, pEl->value, user_data);
      
      if (pEl->value == EMPTYHT || pEl->value == DELETEHT) {
        fflush(stdout);
        (void)fprintf(stderr, "*** mbk error ***\n");
        (void)fprintf(stderr, "controlled_addhtitem impossible : value is EMPTYHT or DELETEHT\n");
        EXIT(1);
      }      
      pEl->key = key;
      return pEl->value;
    } else if ((long) pEl->key == (long) key) {
      pTable->count--;
      pEl->value = func(0, pEl->value, user_data);
      if (pEl->value == EMPTYHT || pEl->value == DELETEHT) {
        fflush(stdout);
        (void)fprintf(stderr, "*** mbk error ***\n");
        (void)fprintf(stderr, "controlled_addhtitem impossible : value is EMPTYHT or DELETEHT\n");
        EXIT(1);
      }      
      return pEl->value;
    }
    indice = (indice + 1) % pTable->length;
  } while (1);
}

long sethtitem(pTable, key, value)
     ht *pTable;
     void *key;
     long value;
{
  int indice = 0;
  htitem *pEl, *FirstDEL=NULL;
  int co = 0;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return sethtitem_v2( (ht_v2*)pTable, key, value );

  if (value == EMPTYHT || value == DELETEHT) {
    fflush(stdout);
    (void)fprintf(stderr, "*** mbk error ***\n");
    (void)fprintf(stderr, "sethtitem impossible : value is EMPTYHT or DELETEHT\n");
    EXIT(1);
  }
  if (pTable->count++ > (pTable->length) * 8 / 10) {
    reallocht(pTable);
    return sethtitem(pTable, key, value);
  }

  indice = hash(key) % pTable->length;
  do {
    if (co++ > HMAX_CALLS) {
      if (pTable->count > (pTable->length) * 2 / 10 || (co >= pTable->length)) {
        reallocht(pTable);
        return sethtitem(pTable, key, value);
      } else if(TRACE_MODE == 'Y') {
        fflush(stdout);
        (void)fprintf (stderr, "*** mbk warning ***\n");
        (void)fprintf (stderr, "reallocht undone : density is too low\n");
      }
    }
    pEl = (pTable->pElem) + indice;
    if (FirstDEL==NULL && pEl->value == DELETEHT) FirstDEL=pEl;
    else if (pEl->value == EMPTYHT) {
      if (FirstDEL!=NULL) pEl=FirstDEL;
      pEl->value = value;
      pEl->key = key;
      return 0;
    } else if ((long) pEl->key == (long) key) {
      pTable->count--;
      pEl->value = value;
      return 1;
    }
    indice = (indice + 1) % pTable->length;
  } while (1);
}

/*******************************************************************************
 * function delhtitem, delete an element in a hash table                         *
 *******************************************************************************/
long delhtitem(pTable, key)
     ht *pTable;
     void *key;
{
  int indice = 0;
  htitem *pEl;
  int co = 0;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE )
    return delhtitem_v2( (ht_v2*)pTable, key );
  
  indice = hash(key) % pTable->length;
  do {
    if (co++ > HMAX_CALLS) {
      if (pTable->count > (pTable->length) * 2 / 10 || (co >= pTable->length)) {

        reallocht(pTable);
        return delhtitem(pTable, key);
      }
    }
    pEl = (pTable->pElem) + indice;
    if (pEl->value != EMPTYHT && pEl->value != DELETEHT) {
      if ((long) key == (long)pEl->key) {
        pTable->count--;
        pEl->value = DELETEHT;
        return pEl->value;
      }
    } else if (pEl->value == EMPTYHT)
      return EMPTYHT;
    indice = (indice + 1) % pTable->length;
  } while (1);
}

/*******************************************************************************
Scan all element of an hash table                                            
first must be set to 1 on the first call, then set to 0 for the following calls.
nextkey must contains the previous value returned (not applicable for the
first call).
end is reached when nextitem contain EMPTYHT.
 *******************************************************************************/

void scanhtkey(pTable, first, nextkey, nextitem)
     ht    *pTable;
     int    first;
     void **nextkey;
     long  *nextitem;
{
  long indice = 0;
  htitem * pEl;
  int co;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE ) {
    scanhtkey_v2( (ht_v2*)pTable, first, nextkey, nextitem );
    return ;
  }

  if( first )
    indice = 0;
  else {
    indice = hash(*nextkey) % pTable->length ;
    co=pTable->length+1;
    do {
      pEl = (pTable->pElem) + indice;
      if( pEl->key == *nextkey )
        break;
        
      indice++ ;
      if( indice == pTable->length )
        indice = 0;
      co--;
    } 
    while( co );

    if( !co ) {
      fflush( stdout );
      fprintf( stderr, "Internal error in scanhtitem().\n" );
      EXIT(1);
    }
    indice++;
  }
  
  while( indice < pTable->length ) {
    pEl = (pTable->pElem) + indice;
    if( pEl->value != EMPTYHT && pEl->value != DELETEHT ) {
      *nextkey  = pEl->key;
      *nextitem = pEl->value;
      return ;
    }
    indice++;
  }

  *nextkey  = NULL ;
  *nextitem = EMPTYHT;
}

/*******************************************************************************
 * display contents of an hash table                                            *
 *******************************************************************************/
void viewht(pTable, pout)
     ht *pTable;
     char *(*pout)();
{
  long i;
  htitem *pEl = pTable->pElem;

  (void)printf("================== viewht ================\n");
  (void)printf("length = %ld\t     count = %ld\n",
               pTable->length, pTable->count);
  (void)printf("==========================================\n");
  for (i = 0; i < pTable->length; i++) {
    if (pEl->value != EMPTYHT && pEl->value != DELETEHT) {
      printf("index  %ld\t", i);
      printf("key    %s\t", pout(pEl->key));
      printf("value  %ld \n", pEl->value);
    }
    pEl++;
  }
}

/*******************************************************************************
 * realloc space to adapt hash table size to number of entries                  *
 *******************************************************************************/
static void reallocht(pTable)
     ht *pTable;
{
  ht *tabBis;
  htitem *pEl;
  int i;
  double ratio;

  pEl = pTable->pElem;

  ratio = (double)pTable->count / (double)pTable->length;
  if (ratio > 0.8) ratio = 1;
  else if (ratio < 0.3) ratio = 0.3;
   
  tabBis = addht((unsigned long)((double)(pTable->length) * 5.0 * ratio)) ;
  for (i = 0; i < pTable->length; i++) {
    if (pEl->value != EMPTYHT && pEl->value != DELETEHT)
      addhtitem(tabBis, pEl->key, pEl->value);
    pEl++;
  }
  mbkfree(pTable->pElem);
  pTable->length = tabBis->length;
  pTable->pElem = tabBis->pElem;
  pTable->count = tabBis->count;
  mbkfree(tabBis);
}

/*******************************************************************************
 * function addvt, create a pointer table                                       *
 *******************************************************************************/
voidt *addvt(length)
     unsigned int length ;
{
  voidt *ptvt ;
  int i ;

  if(length <= 0)
    length = 10 ;

  ptvt  = (voidt *)mbkalloc(sizeof(voidt));
  ptvt->size = 0;
  ptvt->length = length;
  ptvt->data = mbkalloc(length * sizeof(void *));
  for(i = ptvt->size ; i < ptvt->length ; i++)
    (ptvt->data)[i] = (void *)EMPTYHT ;

  return ptvt;
}

/*******************************************************************************
 * function delvt, delete a pointer table                                       *
 *******************************************************************************/
void delvt(table)
     voidt *table ;
{
  if (table->data != NULL) mbkfree(table->data);
  mbkfree(table);
}

/*******************************************************************************
 * function addvtitem, add an element in a pointer table                        *
 *******************************************************************************/
int addvtitem(table,data)
     voidt *table;
     void *data ;
{
  int i ;

  if(table->size == table->length)
    {
      table->length = table->length * 2 ;
      table->data = mbkrealloc(table->data,table->length * sizeof(void *));
      for(i = table->size ; i < table->length ; i++)
        (table->data)[i] = (void *)EMPTYHT ;
    }
  (table->data)[(table->size)++] = data;
  return(table->size - 1) ;
}

/*******************************************************************************
 * function setvtitem, set an element in a pointer table                        *
 *******************************************************************************/
void setvtitem(table,item,data)
     voidt *table;
     int item ;
     void *data ;
{
  int i ;

  if(item >= table->length)
    {
      table->size = table->length ;
      table->length = item * 2 ;
      table->data = mbkrealloc(table->data,table->length * sizeof(void *));
      for(i = table->size ; i < table->length ; i++)
        (table->data)[i] = (void *)EMPTYHT ;
      table->size = item + 1 ;
    }
  (table->data)[item] = data;
}

/*******************************************************************************
 * function getvtitem, get an element in a pointer table                        *
 *******************************************************************************/
void *getvtitem(table,item)
     voidt *table;
     int item ;
{
  if(item >= table->length)
    return((void *)EMPTYHT) ;
  else
    return((table->data)[item]) ;
}

/*******************************************************************************
 * function delvtitem, del an element in a interger table                       *
 *******************************************************************************/
void delvtitem(table,item)
     voidt *table;
     int item ;
{
  if(item >= table->length)
    return ;
  else
    (table->data)[item] = (void *)DELETEHT ;
}

/*******************************************************************************
 * function addit, create a integer table                                       *
 *******************************************************************************/
it *addit(length)
     unsigned int length ;
{
  it *ptit ;
  int i ;

  if(length <= 0)
    length = 10 ;

  ptit  = (it *)mbkalloc(sizeof(it));
  ptit->size = 0;
  ptit->length = length;
  ptit->data = mbkalloc(length * sizeof(long));
  for(i = ptit->size ; i < ptit->length ; i++)
    (ptit->data)[i] = (long)EMPTYHT ;

  return ptit;
}

/*******************************************************************************
 * function delit, delete a integer table                                       *
 *******************************************************************************/
void delit(table)
     it *table ;
{
  if (table->data != NULL) mbkfree(table->data);
  mbkfree(table);
}

/*******************************************************************************
 * function addititem, add an element in a interger table                       *
 *******************************************************************************/
int addititem(table,data)
     it *table;
     long data ;
{
  int i ;

  if(table->size == table->length)
    {
      table->length = table->length * 2 ;
      table->data = mbkrealloc(table->data,table->length * sizeof(long));
      for(i = table->size ; i < table->length ; i++)
        (table->data)[i] = (long)EMPTYHT ;
    }
  (table->data)[(table->size)++] = data;
  return(table->size - 1) ;
}

/*******************************************************************************
 * function setititem, set an element in a interger table                       *
 *******************************************************************************/
void setititem(table,item,data)
     it *table;
     int item ;
     long data ;
{
  int i ;

  if(item >= table->length)
    {
      table->size = table->length ;
      table->length = item * 2 ;
      table->data = mbkrealloc(table->data,table->length * sizeof(long));
      for(i = table->size ; i < table->length ; i++)
        (table->data)[i] = (long)EMPTYHT ;
      table->size = item + 1 ;
    }
  (table->data)[item] = data;
}

/*******************************************************************************
 * function getititem, add an element in a interger table                       *
 *******************************************************************************/
long getititem(table,item)
     it *table;
     int item ;
{
  if(item >= table->length)
    return((long)EMPTYHT) ;
  else
    return((table->data)[item]) ;
}

/*******************************************************************************
 * function delititem, del an element in a interger table                       *
 *******************************************************************************/
void delititem(table,item)
     it *table;
     int item ;
{
  if(item >= table->length)
    return ;
  else
    (table->data)[item] = (long)DELETEHT ;
}

/*******************************************************************************
 * function adddt, create a integer table                                       *
 *******************************************************************************/
dt *adddt(length)
     unsigned int length ;
{
  dt *ptdt ;
  int i ;

  if(length <= 0)
    length = 10 ;

  ptdt  = (dt *)mbkalloc(sizeof(dt));
  ptdt->size = 0;
  ptdt->length = length;
  ptdt->data = (double *)mbkalloc(length * sizeof(double));
  for(i = ptdt->size ; i < ptdt->length ; i++)
    (ptdt->data)[i] = (double)EMPTYHT ;

  return ptdt;
}

/*******************************************************************************
 * function deldt, delete a integer table                                       *
 *******************************************************************************/
void deldt(table)
     dt *table ;
{
  if (table->data != NULL) mbkfree(table->data);
  mbkfree(table);
}

/*******************************************************************************
 * function adddtitem, add an element in a interger table                       *
 *******************************************************************************/
int adddtitem(table,data)
     dt *table;
     double data ;
{
  int i ;

  if(table->size == table->length)
    {
      table->length = table->length * 2 ;
      table->data = mbkrealloc(table->data,table->length * sizeof(double));
      for(i = table->size ; i < table->length ; i++)
        (table->data)[i] = (double)EMPTYHT ;
    }
  (table->data)[(table->size)++] = data;
  return(table->size - 1) ;
}

/*******************************************************************************
 * function setdtitem, set an element in a interger table                       *
 *******************************************************************************/
void setdtitem(table,item,data)
     dt *table;
     int item ;
     double data ;
{
  int i ;

  if(item >= table->length)
    {
      table->size = table->length ;
      table->length = item * 2 ;
      table->data = mbkrealloc(table->data,table->length * sizeof(double));
      for(i = table->size ; i < table->length ; i++)
        (table->data)[i] = (double)EMPTYHT ;
      table->size = item + 1 ;
    }
  (table->data)[item] = data;
}

/*******************************************************************************
 * function getdtitem, add an element in a interger table                       *
 *******************************************************************************/
double getdtitem(table,item)
     dt *table;
     int item ;
{
  if(item >= table->length)
    return((double)EMPTYHT) ;
  else
    return((table->data)[item]) ;
}

/*******************************************************************************
 * function deldtitem, del an element in a interger table                       *
 *******************************************************************************/
void deldtitem(table,item)
     dt *table;
     int item ;
{
  if(item >= table->length)
    return ;
  else
    (table->data)[item] = (double)DELETEHT ;
}

/*******************************************************************************
 * All that needed for a cute banner, by Frederic Petrot                        *
 * Used to be a standalone library                                              *
 *******************************************************************************/
#include <time.h>
#define WINDOW_SIZE 81
#define LINES 15
#define ASCENT 13
static char screen[LINES][WINDOW_SIZE];

static int indx(c)
     char c;
{
  return c >= '0' && c <= '9' ? (int)c - '0'
    : isupper((int)c) ? (int)10 + c - 'A'
    : islower((int)c) ? (int)11 + 'Z' - 'A' + c - 'a'
    : -1;
}

static void banner(s, police, nl)
     char *s;
     char *police[][62];
     int nl;
{
  int i, j, k, l, m;
  char *line;

  /* rince off :
     the buffer is filled with nul characteres. */
  for (j = 0; j < nl; j++)
    for (i = 0; i < WINDOW_SIZE; i++)
      screen[j][i] = '\0';
  /* first :
     filling the buffer with direct table output. */
  while (*s) {
    for (i = 0; i < nl; i++) {
      if ((j = indx(*s)) == -1) {
        fprintf(stderr,
                "alliancebanner: Error: Character out of [0-9A-Za-z] range\n");
        EXIT(1);
      }
      line = police[j][i];
      if (strlen(line) + strlen(screen[i]) >= WINDOW_SIZE) {
        fprintf(stderr,
                "alliancebanner: Error: Resulting size bigger than %d columns not allowed\n",
                WINDOW_SIZE - 1);
        EXIT(1);
      }
      strcat(screen[i], line);
      if (*(s + 1) != '\0')
        strcat(screen[i], " ");
    }
    s++;
  }
  for (m = l = -1, j = 0; j < nl; j++)
    for (i = 0; i < WINDOW_SIZE; i++)
      if (screen[j][i] == '@') {
        if (m == -1)
          m = j;
        l = j;
        break;
      }
  k = strlen(screen[0]);
  /* banner :
     output on stdout. */
  putc('\n', stdout);
  for (j = m; j <= l; j++) {
    for (i = 0; i < (WINDOW_SIZE - k) / 2; i++)
      putc(' ', stdout);
    for (i = 0; i < k; i++)
      putc(screen[j][i], stdout);
    putc('\n', stdout);
  }
}

static void cartouche(tool, tv, comment, date, av, authors, contrib)
     char *tool, *tv, *comment, *date, *av, *authors, *contrib;
{
  int i, j, k, l;
  static char *msg[6] = {
    "%s",
    "Alliance CAD System %s,\"%s %s",
    "Copyright (c) %s-%d,\"ASIM/LIP6/UPMC",
    "Author(s):\"%s",
    "Contributor(s):\"%s",
    "E-mail support:\"alliance-support@asim.lip6.fr"
  };
  int msgl[6];
  char *str;
  time_t timer;
  char day[4], month[4];
  int year, nday, hour, minute, second;

  (void)time(&timer);
  (void)sscanf(ctime(&timer), "%s %s %d %d:%d:%d %d",
               day, month, &nday, &hour, &minute, &second, &year);
  /* rince off :
     the buffer is filled with nul characteres. */
  for (j = 0; j < 12; j++)
    for (i = 0; i < WINDOW_SIZE; i++)
      screen[j][i] = '\0';
  i = strlen(tool);
  str = mbkstrdup(tool);
  for (k = 0; k < i; k++)
    str[k] = isupper((int)tool[k]) ? tolower(tool[k]) : tool[k];
  sprintf(screen[0], msg[0], comment);
  sprintf(screen[1], msg[1], av, str, tv);
  mbkfree(str);
  sprintf(screen[2], msg[2], date, year);
  if (authors != (char *)0 )
    sprintf(screen[3], msg[3],authors);
  else
    screen[3][0] = '\0';
  if (contrib != (char *)0 )
    sprintf(screen[4], msg[4],contrib);
  else
    screen[4][0] = '\0';

  strcat(screen[5], msg[5]);

  for (i = 1; i < 6; i++) {
    msgl[i] = strlen(screen[i]);
    j = j < msgl[i] ? msgl[i] : j;
  }
  for (i = 1; i < 6; i++)
    for (l = 0, k = 0; k < WINDOW_SIZE; k++) {
      if (screen[i][k] == '\0') {
        screen[i + 6][k + l] = '\0';
        break;
      }
      if (screen[i][k] == '"') { /* only once per line */
        for (; l <= j - msgl[i]; l++)
          screen[i + 6][k + l] = ' ';
        continue;
      }
      screen[i + 6][k + l] = screen[i][k];
    }
  /* cartouche :
     output on stdout. */
  i = strlen(comment);
  putc('\n', stdout);
  for (k = 0; k < (WINDOW_SIZE - i) / 2; k++)
    putc(' ', stdout);
  puts(screen[0]);
  putc('\n', stdout);
  for (i = 1; i < 6; i++) {
    if (screen[i][0]=='\0') continue;
    for (k = 0; k < (WINDOW_SIZE - j) / 2; k++)
      putc(' ', stdout);
    for (k = 0; k <= j; k++)
      if (screen[i + 6][k] != 0) /* not so nice, but */
        putc(screen[i + 6][k], stdout);
    putc('\n', stdout);
  }
  putc('\n', stdout);
}

void alliancebanner_with_contrib( tool, tv, comment, date, av, authors, contrib)
     char *tool, *tv, *comment, *date, *av, *authors, *contrib;
{
  banner(tool, Unknown_Bold_Normal_14, 15);
  cartouche(tool, tv, comment, date, av, authors,contrib);
}

void alliancebanner_with_authors( tool, tv, comment, date, av, authors)
     char *tool, *tv, *comment, *date, *av, *authors;
{
  alliancebanner_with_contrib( tool, tv, comment, date, av, authors, NULL );
}


void alliancebanner(tool, tv, comment, date, av)
     char *tool, *tv, *comment, *date, *av;
{
  alliancebanner_with_contrib( tool, tv, comment, date, av, NULL, NULL );
}

/******************
 * Contributed to by Ludovic Jacomme
 **************************/
ptype_list *HEAD_MBKDEBUG = NULL;
char        MBK_DEBUG_ON  = 0;

static void trapdebug()
{
  ptype_list *ScanDebug;

  for ( ScanDebug  = HEAD_MBKDEBUG;
        ScanDebug != (ptype_list *)NULL;
        ScanDebug  = ScanDebug->NEXT )
    {
      fprintf( stdout, "mbkdebug: file %s line %ld\n", 
               (char *)ScanDebug->DATA, ScanDebug->TYPE );
    }

  fflush( stdout );

  signal( SIGQUIT, trapdebug );
  signal( SIGSEGV, SIG_DFL      );
  signal( SIGBUS , SIG_DFL      );
  signal( SIGILL , SIG_DFL      );
}

void mbkdebug()
{
  signal( SIGSEGV, trapdebug );
  signal( SIGBUS,  trapdebug );
  signal( SIGILL,  trapdebug );
  signal( SIGQUIT, trapdebug );

  MBK_DEBUG_ON = 1;
}

/*
** Added by Ludovic Jacomme (The slave)
** in order to "trap" exit with Graal/Dreal etc ...
*/

/*
void mbkexit( ExitValue )

     int ExitValue;
{
  if(MBK_EXIT_KILL == 'Y') kill( getpid(), SIGTERM ) ;
  else if ( MBK_EXIT_FUNCTION != NULL )
    {
      (*MBK_EXIT_FUNCTION)( ExitValue );
    }
  else exit(ExitValue) ;

  while(1);
}*/


/* To compile with gcc under SunOS */

#ifdef SunOS

void *dlopen(path, mode)
     char *path; int mode;
{
}

void *dlsym(handle, symbol)
     void *handle; char *symbol;
{
}

char *dlerror()
{
}

int dlclose(handle)
     void *handle;
{
}

#endif

void CreateHeap(int size, int granularity, HeapAlloc *myheap)
{
  if (size<(signed)sizeof(void *)) size=sizeof(void *);
  myheap->size=size;
  myheap->blocks_to_free=NULL;
  if (granularity==0) granularity=BUFSIZE;
  myheap->granularity=granularity;
  myheap->HEAD=NULL;
#ifdef ENABLE_STATS
  myheap->__SIZE__=0;
#endif
}

void DeleteHeap(HeapAlloc *myheap)
{
#ifndef NOHEAPALLOCFORHEAP
  chain_list *cl;
  for (cl=myheap->blocks_to_free; cl!=NULL; cl=cl->NEXT)
    mbkfree(cl->DATA);
  freechain(myheap->blocks_to_free);
#endif
}

void *AddHeapItem(HeapAlloc *myheap)
{
#ifdef NOHEAPALLOCFORHEAP
  return mbkalloc(myheap->size);
#else
  char *pt;
  if (myheap->HEAD == NULL) {
    int i;
    pt = (char *)mbkalloc(myheap->granularity*myheap->size);
    myheap->blocks_to_free=addchain(myheap->blocks_to_free, pt);
    myheap->HEAD = pt;
    for (i = 1; i < myheap->granularity; i++) 
      {
	*(void **)pt = pt+myheap->size;
	pt+=myheap->size;
      }
    //    printf("<%d>",(pt-(char *)myheap->HEAD)/myheap->size);
    *(void **)pt = NULL;
#ifdef ENABLE_STATS
    myheap->__SIZE__+=sizeof(chain_list *)+myheap->granularity*myheap->size;
#endif
  }
  
  pt = myheap->HEAD;
  myheap->HEAD = *(void **)myheap->HEAD;
  return pt;
#endif
}

void DelHeapItem(HeapAlloc *myheap, void *item)
{
#ifdef NOHEAPALLOCFORHEAP
  mbkfree(item);
#else
  *(void **)item=myheap->HEAD;
  myheap->HEAD=item;
#endif
}

size_t getsizeofHeapAlloc( HeapAlloc *heap )
{
  size_t size = 0 ;
  chain_list *chain ;
  if( !heap )
    return 0 ;
  for( chain = heap->blocks_to_free ; chain ; chain = chain->NEXT ) 
    size = size + heap->size * heap->granularity + sizeof( chain_list );
  return size ;
}

typedef struct
{
  char *key;
  void *data;
} s2sort;

int s2sort_sortfunc(const void *a0, const void *b0)
{
  s2sort *a=(s2sort *)a0, *b=(s2sort *)b0;
  return strcmp(b->key, a->key);
}

chain_list *GetAllHTElems_sub(ht *pTable, int sort)
{
  chain_list *cl=NULL, *ch=NULL;
  int i, cnt;
  htitem *pEl;
  s2sort *s2c;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE ) 
    return GetAllHTElems_sub_v2( (ht_v2*)pTable, sort );
    
  if (!pTable) return cl;

  pEl = pTable->pElem;
  for (i = 0, cnt=0; i < pTable->length; i++) {
    if (pEl->value != EMPTYHT && pEl->value != DELETEHT) {
      cl=addchain(cl, (void *)pEl->value);
      if (sort) ch=addchain(ch, (void *)pEl->key);
      cnt++;
    }
    pEl++;
  }

  if (!sort) return cl;

  s2c=(s2sort *)mbkalloc(sizeof(s2sort)*cnt);
  for (i=0; i<cnt; i++, cl=delchain(cl, cl), ch=delchain(ch,ch))
  {
    s2c[i].key=(char *)ch->DATA;
    s2c[i].data=cl->DATA;
  }
  
  qsort(s2c, cnt, sizeof(s2sort), s2sort_sortfunc);

  for (i=0, cl=NULL; i<cnt; i++)
    cl=addchain(cl, s2c[i].data);
  
  mbkfree(s2c);

  return cl;
}

chain_list *GetAllHTElems_sub_v2( ht_v2 *h, int sort)
{
  chain_list *cl=NULL, *ch=NULL;
  int i, cnt;
  htitem_v2 *item;
  s2sort *s2c;
  
  
  if (!h) return cl;

  cnt=0;
  for( i=0 ; i<h->size ; i++ ) {
    for( item = h->ITEM_HASHTABLE[i] ; item ; item = item->next ) {
      cl = addchain( cl, (void*)item->value );
      if( sort ) ch=addchain( ch, (void*)item->key );
      cnt++;
    }
  }

  if (!sort) return cl;

  s2c=(s2sort *)mbkalloc(sizeof(s2sort)*cnt);
  for (i=0; i<cnt; i++, cl=delchain(cl, cl), ch=delchain(ch,ch))
  {
    s2c[i].key=(char *)ch->DATA;
    s2c[i].data=cl->DATA;
  }
  
  qsort(s2c, cnt, sizeof(s2sort), s2sort_sortfunc);

  for (i=0, cl=NULL; i<cnt; i++)
    cl=addchain(cl, s2c[i].data);
  
  mbkfree(s2c);

  return cl;
}

chain_list *GetAllHTElems(ht *pTable)
{
  return GetAllHTElems_sub(pTable, 0);
}

chain_list *GetAllHTKeys(ht *pTable)
{
  chain_list *cl=NULL;
  int i;
  htitem *pEl;

  if( V_BOOL_TAB[__AVT_USEHT_V2].VALUE ) 
    return GetAllHTKeys_v2( (ht_v2*)pTable );

  if (!pTable) return cl;

  pEl = pTable->pElem;
  for (i = 0; i < pTable->length; i++) {
    if (pEl->value != EMPTYHT && pEl->value != DELETEHT) {
      cl=addchain(cl, (void *)pEl->key);
    }
    pEl++;
  }
  return cl;
}

chain_list* GetAllHTKeys_v2( ht_v2 *h )
{
  int i;
  htitem_v2 *item ;
  chain_list *head ;

  head = NULL ;
  for( i=0 ; i<h->size ; i++ )
    for( item = h->ITEM_HASHTABLE[i] ; item ; item = item->next ) 
      head = addchain( head, item->key );
  return head ;
}

ht_v2 *addht_v2(long len)
{
  int i;
  ht_v2 *h;
  h=(ht_v2 *)mbkalloc(sizeof(ht_v2));
  if (len!=0)
    {
      h->size=(len+127)/128;
      h->primes_index=-1;
    }
  else
    {
      h->primes_index=0;
      h->size=namealloc_primes[0];
    }

  h->ITEM_HASHTABLE=(htitem_v2 **)mbkalloc(sizeof(htitem_v2 *)*h->size);
  for (i=0; i<h->size; i++) h->ITEM_HASHTABLE[i]=NULL;
  CreateHeap(sizeof(htitem_v2), 0, &h->ha);
  h->rehashlimit=128;
  return h;
}

void delht_v2(ht_v2 *h)
{
  DeleteHeap(&h->ha);
  mbkfree(h->ITEM_HASHTABLE);
  mbkfree(h);
}

static void ht_v2_rehash(ht_v2 *h)
{
  htitem_v2 **nn, *cl, *next;
  int i, index;
  unsigned int prm;

  if (h->primes_index==-1)
    prm=0;
  else
    prm=h->primes_index+1;
  while (prm<sizeof(namealloc_primes)/sizeof(*namealloc_primes) && namealloc_primes[prm]<h->size) prm++;
  h->primes_index=prm;
  if (prm>=sizeof(namealloc_primes)/sizeof(*namealloc_primes))
    {
      return;
    }

  //  fprintf(stdout,".rehashing ht: %d -> %d...",h->size,namealloc_primes[prm]); fflush(stdout);

  nn=(htitem_v2 **)mbkalloc(sizeof(htitem_v2 *)* namealloc_primes[prm]);
  for (i=0;i<namealloc_primes[prm];i++)
    nn[i]=NULL;

  for (i=0;i<h->size;i++)
    {
      for (cl=h->ITEM_HASHTABLE[i]; cl!=NULL; cl=next)
        {
          next=cl->next;
          index=hash(cl->key) % namealloc_primes[prm];
          cl->next=nn[index];
          nn[index]=cl;
        }
    }
  mbkfree(h->ITEM_HASHTABLE);
  h->ITEM_HASHTABLE=nn;
  h->size=namealloc_primes[prm];
  //  fprintf(stdout,"done\n");
}


long gethtitem_v2(ht_v2 *h, void *key)
{
  int index;
  htitem_v2 *head;

  index=hash(key) % h->size;
  head=h->ITEM_HASHTABLE[index];
  while (head!=NULL && head->key!=key) head=head->next;

  if (head==NULL) return EMPTYHT;
  return head->value;
}

ht_v2 *dupht_v2( ht_v2 *orig )
{
  ht_v2 *dup;
  void  *nextkey ;
  long   nextitem ;

  dup = addht_v2( orig->size );
  
  scanhtkey_v2( orig, 1, &nextkey, &nextitem );
  while( nextitem != EMPTYHT ) {
    addhtitem_v2( dup, nextkey, nextitem );
    scanhtkey_v2( orig, 0, &nextkey, &nextitem );
  }

  return dup ;
}

void scanhtkey_v2( ht_v2 *h, int first, void **nextkey, long *nextitem )
{
  int        i ;
  htitem_v2 *item ;
 
  i    = 0;
  item = NULL ;
  
  if( !first ) {
    i = hash( *nextkey ) % h->size ;
    
    for( item = h->ITEM_HASHTABLE[i] ; item->key != *nextkey ; item = item->next ) ;
    item = item->next ;
    
    if( item ) {
      *nextkey  = item->key ;
      *nextitem = item->value ;
    }
    else 
      i++ ;
  }

  if( !item ) {

    for( i=i ; i<h->size && !h->ITEM_HASHTABLE[i] ; i++ );
    
    if( i==h->size ) {
      *nextkey  = NULL ;
      *nextitem = EMPTYHT ;
    }
    else {
      *nextkey  = h->ITEM_HASHTABLE[i]->key ;
      *nextitem = h->ITEM_HASHTABLE[i]->value ;
    }
  }
}

void set_ht_v2_rehashlimit(ht_v2 *h, int value)
{
  h->rehashlimit=value;
}

long sethtitem_v2(ht_v2 *h, void *key, long value)
{
  return addhtitem_v2( h, key, value );
}

long controlled_addhtitem_v2(ht_v2 *h, void *key, long (*func)(int newone, long old_value, void *user_data), void *user_data ) 
{
  int index, depth=0;
  htitem_v2 *hi ;
  long value ;

  index=hash(key) % h->size;
  hi=h->ITEM_HASHTABLE[index];
  while(hi !=NULL && hi->key!=key) { hi=hi->next; depth++; }

  if (hi==NULL)
    {
      hi=(htitem_v2 *)AddHeapItem(&h->ha);
      hi->next=h->ITEM_HASHTABLE[index];
      h->ITEM_HASHTABLE[index]=hi;
      hi->key=key;
      hi->value=func( 1, EMPTYHT, user_data );
    }
  else {
    hi->value=func( 0, hi->value, user_data );
  }
 
  value = hi->value ;

  if (depth>h->rehashlimit && (h->primes_index==-1 || (unsigned)h->primes_index<(sizeof(namealloc_primes)/sizeof(*namealloc_primes))-1))
    ht_v2_rehash(h);

  return value ;
}

long addhtitem_v2(ht_v2 *h, void *key, long value)
{
  int index, depth=0;
  htitem_v2 *hi, *head;
  long replaced ;

  index=hash(key) % h->size;
  head=h->ITEM_HASHTABLE[index];
  while (head!=NULL && head->key!=key) { head=head->next; depth++; }

  if (head==NULL)
    {
      hi=(htitem_v2 *)AddHeapItem(&h->ha);
      hi->next=h->ITEM_HASHTABLE[index];
      h->ITEM_HASHTABLE[index]=hi;
      hi->key=key;
      hi->value=value;
      replaced=0;
    }
  else {
    head->value=value;
    replaced=1;
  }

  if (depth>h->rehashlimit && (h->primes_index==-1 || (unsigned)h->primes_index<(sizeof(namealloc_primes)/sizeof(*namealloc_primes))-1))
    ht_v2_rehash(h);

  return replaced ;
}

long delhtitem_v2(ht_v2 *h, void *key)
{
  int index;
  long value;
  htitem_v2 *head, *prev=NULL;

  index=hash(key) % h->size;
  head=h->ITEM_HASHTABLE[index];
  while (head!=NULL && head->key!=key) prev=head, head=head->next;

  if (head==NULL) return EMPTYHT;

  if (prev==NULL)
    h->ITEM_HASHTABLE[index]=head->next;
  else
    prev->next=head->next;

  value=head->value;
  DelHeapItem(&h->ha, head);
  return value;
}

size_t getsizeofht_v2( ht_v2 *ht )
{
  size_t size =0 ;
  if( !ht )
    return 0 ;
  size = sizeof( ht_v2 ) ;
  size = size + sizeof( htitem_v2* ) * ht->size ;
  size = size + getsizeofHeapAlloc( & ht->ha );
  return size ;
}

chain_list *GetAllHTElems_v2(ht_v2 *h)
{
  chain_list *cl=NULL;
  int i;
  htitem_v2 *he;

  for (i=0;i<h->size;i++)
    for (he=h->ITEM_HASHTABLE[i];he!=NULL; he=he->next) 
      cl=addchain(cl, (void *)he->value);

  return cl;
}

void ht_v2Stat(ht_v2 *h)
{
  int i, nb, max=0,moy=0, nb0=0;
  htitem_v2 *cl;

  if (h==NULL) return;
  printf("--- ht_v2 stats:\n");
  for (i=0, nb=0;i<h->size;i++) if (h->ITEM_HASHTABLE[i]==NULL) nb++;
  printf("  %d/%d entries unused\n",nb,h->size);
  for (i=0, max=0;i<h->size;i++)
    if (h->ITEM_HASHTABLE[i]!=NULL) 
      {
        nb0++;
        for (nb=0, cl=h->ITEM_HASHTABLE[i];cl!=NULL; cl=cl->next, nb++) ;
        if (nb>max) max=nb;
        moy+=nb;
      }
  printf("  chain length: mean=%.1f max=%d\n",(float)moy/(float)nb0,max);
}

#ifdef MALLOC_HISTOGRAM

#undef mbkalloc
#undef mbkfree
#undef mbkrealloc
#undef malloc
#undef free
#undef realloc

void malloc_hook_error()
{

}


void *my_malloc_hook(char *file, int line, size_t size)
{
  char *result;
  long val;
  if (!MALLOC_HISTO_mutex) return mbkalloc (size);
  MALLOC_HISTO_mutex=0;
  result = malloc (size+sizeof(long)*2);

  if (result==NULL)
    {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"fatal mbkalloc error : not enough memory\n");
      fprintf(stderr,"when trying to allocate %ld bytes, top= %ldKb\n",size,mbkprocessmemoryusage()/1024);
      EXIT(1);
    }
  MALLOC_HOOK_CONSO+=sizeof(long)*2;
  MALLOC_HOOK_TOTAL_MEM+=size;
  MALLOC_HOOK_TOTAL_MEM_FROM_START+=size;
  MALLOC_HOOK_TOTAL_CALL++;
  if ((val=gethtitem_v2(MALLOC_HISTO, (void *)size))==EMPTYHT) val=0;
  val++;
  addhtitem_v2(MALLOC_HISTO, (void *)size, val);

  //  printf ("malloc (%ld) returns %p\n", (unsigned long)size, result);
  *(long *)result=size;
  *(long *)(result+sizeof(long))=0x12345678;
  MALLOC_HISTO_mutex=1;
  return result+sizeof(long)*2;
}

void my_free_hook(void *block)
{
  char *result=block;
  unsigned long size;
  long val;
  if (!MALLOC_HISTO_mutex) { mbkfree(block); return ;}
  if (block==NULL) return;
  result-=sizeof(long)*2;
  if (*(long *)(result+sizeof(long))!=0x12345678) 
    {
      MALLOC_HOOK_ERRORS++;
      malloc_hook_error();
      free(block);
      printf("ERR-FR\n");
      return ;
    }
  else
    *(long *)(result+sizeof(long))=0;

  size=*(long *)result;
  MALLOC_HOOK_CONSO-=sizeof(long)*2;
  MALLOC_HOOK_TOTAL_MEM-=size;
  MALLOC_HOOK_TOTAL_MEM_FROM_START-=size;
  FREE_HOOK_TOTAL_CALL++;

  MALLOC_HISTO_mutex=0;
  if ((val=gethtitem_v2(MALLOC_HISTO, (void *)size))==EMPTYHT) MALLOC_HOOK_ERRORS++;
  else addhtitem_v2(MALLOC_HISTO, (void *)size, val-1);
  MALLOC_HISTO_mutex=1;
  free (result);
  //  printf ("free (%ld)\n", (unsigned long)size, result);
}

void *my_realloc_hook(char *file, int line, void *block, size_t size)
{
  char *result=block;
  unsigned long size0;
  long val;
  if (!MALLOC_HISTO_mutex) return mbkrealloc(block, size);
  result-=sizeof(long)*2;
  if (*(long *)(result+sizeof(long))!=0x12345678) 
    {
      MALLOC_HOOK_ERRORS++;
      printf("ERR-RE\n");
      malloc_hook_error();
      return realloc(block, size);
    }
  MALLOC_HISTO_mutex=0;
  size0=*(long *)result;
  MALLOC_HOOK_TOTAL_MEM-=size0;
  MALLOC_HOOK_TOTAL_MEM_FROM_START-=size0;  
  REALLOC_HOOK_TOTAL_CALL++;

  if ((val=gethtitem_v2(MALLOC_HISTO, (void *)size0))==EMPTYHT) MALLOC_HOOK_ERRORS++;
  else addhtitem_v2(MALLOC_HISTO, (void *)size0, val-1);

  result=realloc(result, size+sizeof(long)*2);
  if (result==NULL)
    {
      (void)fflush(stdout);
      (void)fprintf(stderr,"*** mbk error ***\n");
      (void)fprintf(stderr,"fatal mbkalloc error : not enough memory\n");
      fprintf(stderr,"when trying to allocate %ld bytes, top= %ldKb\n",size,mbkprocessmemoryusage()/1024);
      EXIT(1);
    }
  MALLOC_HOOK_TOTAL_MEM+=size;
  MALLOC_HOOK_TOTAL_MEM_FROM_START+=size;
  if ((val=gethtitem_v2(MALLOC_HISTO, (void *)size))==EMPTYHT) val=0;
  val++;
  addhtitem_v2(MALLOC_HISTO, (void *)size, val);

  *(long *)result=size;
  *(long *)(result+sizeof(long))=0x12345678;
  MALLOC_HISTO_mutex=1;
  return result+sizeof(long)*2;
}


void setup_malloc_hook()
{
  if (MALLOC_HISTO_mutex) return;
  MALLOC_HISTO=addht_v2(19);
  MALLOC_HISTO_mutex=1;
}

void reset_malloc_hook()
{
  if (!MALLOC_HISTO_mutex) return;
  MALLOC_HISTO_mutex=0;
  MALLOC_HOOK_TOTAL_MEM=0, MALLOC_HOOK_TOTAL_CALL=0, MALLOC_HOOK_ERRORS=0, FREE_HOOK_TOTAL_CALL=0, REALLOC_HOOK_TOTAL_CALL=0;
  delht_v2(MALLOC_HISTO);
  MALLOC_HISTO=addht_v2(190);
  MALLOC_HISTO_mutex=1;
}


typedef struct
{
  unsigned long size, nb, tot;
} mhtos;
static int mhtos_mode;
int mhtos_sort(const void *a, const void *b)
{
  mhtos *a0=(mhtos *)a, *b0=(mhtos *)b;
  if (mhtos_mode==0)
    {
      if (a0->size<b0->size) return -1;
      else if (a0->size>b0->size) return 1;
      return 0;
    }
  else
    {
      if (a0->tot<b0->tot) return -1;
      else if (a0->tot>b0->tot) return 1;
      return 0;
    }
}

void malloc_hook_stats(char *filename, int mode)
{
  int i, j, tot;
  htitem_v2 *cl;
  FILE *f;
  mhtos tab[1000];
  unsigned long all;

  mhtos_mode=mode;

  if (!MALLOC_HISTO_mutex) return;

  if (strcmp(filename,"stdout")==0) f=stdout;
  else f=fopen(filename,"wt");
  
  all=0;
  tot=0;
  fprintf(f,"--- malloc_hook_stats: REAL TOTAL=%ldk (+%ldk)\n", MALLOC_HOOK_TOTAL_MEM_FROM_START/1024, MALLOC_HOOK_CONSO/1024);
  fprintf(f," %10s %10s %10s\n","size","nb","total");
  fprintf(f,"--------------------------------------\n");
  for (i=0;i<MALLOC_HISTO->size;i++)
    for (cl=MALLOC_HISTO->ITEM_HASHTABLE[i];cl!=NULL; cl=cl->next) 
      if (cl->value!=0)
        {
          tab[tot].size=(unsigned long)cl->key;
          tab[tot].nb=(unsigned long)cl->value;
          tab[tot].tot=tab[tot].size*tab[tot].nb;
          all+=tab[tot].tot;
          tot++;
        }

  qsort(tab, tot, sizeof(mhtos), mhtos_sort);

  for (i=0;i<tot; i++)
    fprintf(f," %10ld %10ld %10ldk  %3.0f%%\n",tab[i].size,tab[i].nb,tab[i].tot/1024, tab[i].tot*100.0/all);

  fprintf(f,"--------------------------------------\n");
  fprintf(f," %ld errors, total mem =%ldk, #malloc=%ld, #free=%ld, #realloc=%ld\n",MALLOC_HOOK_ERRORS, MALLOC_HOOK_TOTAL_MEM/1024,MALLOC_HOOK_TOTAL_CALL,FREE_HOOK_TOTAL_CALL,REALLOC_HOOK_TOTAL_CALL);
  if (f!=stdout) fclose(f);
}
#endif
static void mbk_remove_1(char *buf)
{
  int i, j;
  i=0; j=0;
  while (buf[i]!='\0') 
  {
     if (buf[i]!=1) buf[j++]=buf[i];
     i++;
  }
  buf[j++]='\0';
}

char *mbk_devect_sub(char *name, char *leftb, char *rightb, char *sepb, char *buf, int testchar(int))
/* arguments :
   leftb  = string of opening delimiter.
   rightb = string of closing delimiter corresponding to leftb.
   sepb   = string of single character delimiter.
   testchar = an optional function to check contents of vecter.

   eg : leftb    = "([<"
        rightb   = ")]>"
        sebp     = "_."
        testchar = isdigit()
   traitement special des wildcards C commencant par %
*/
{
  char c;
  int l, i, j;
  char emptystyring[]="\0";
 
  if( !leftb )  leftb  = emptystyring ; 
  if( !rightb ) rightb = emptystyring ;
  if( !sepb )   sepb   = emptystyring ;

  strcpy(buf, name);
  if (!MBK_DEVECT || buf[0]=='\0') return buf;
  l=strlen(buf)-1;
  c=buf[l];
 
  for (i=0; rightb[i]!='\0' && leftb[i]!='\0' && c!=rightb[i]; i++);
  if (buf[0]=='%' && c==']' && (l==0 || buf[l-1]!='\\')) return buf;
  
  if (rightb[i]=='\0' || leftb[i]=='\0') {
    if (buf[0]=='%') {l--; buf[l]=1;}
    if (testchar) {
      for( i=l ; i && testchar((unsigned char)buf[i]) ; i-- );
      if( i && i!=l ) {
        for(j=0; sepb[j] && sepb[j]!=buf[i]; j++ );
        if(sepb[j] && !(buf[0]=='%' && i!=0 && buf[i-1]!='\\')) {
          buf[i]=' ';
          if (buf[0]=='%') { buf[i-1]=1; mbk_remove_1(buf); }
          return buf;
        }
      }
    }
    else {
      i=l ;
      while( i>0 ) {
        for( j=0; sepb[j] && sepb[j]!=buf[i] ; j++ );
        if( sepb[j]  && !(buf[0]=='%' && i!=0 && buf[i-1]!='\\')) {
          if( i!=l ) {
            buf[i]=' ';
            if (buf[0]=='%') { buf[i-1]=1; mbk_remove_1(buf); }
            return buf;
          }
          else
            break ;
        }
        i--;
      }
    }
  }
  else {
    buf[l]='\0';
    if (buf[0]=='%') { l--; buf[l]=1; }
    if( testchar ) {
      for( j=l-1 ; j && testchar((unsigned char)buf[j] ); j-- );
      if( j && buf[j]==leftb[i] && !(buf[0]=='%' && j!=0 && buf[j-1]!='\\')) {
        buf[j]=' ';
        if (buf[0]=='%') { buf[i-1]=1; mbk_remove_1(buf); }
        return buf ;
      }
    }
    else {
      while (l>0 && buf[l]!=leftb[i]) l--;
      if (l && !(buf[0]=='%' && buf[l-1]!='\\')) {
        buf[l]=' ';
        if (buf[0]=='%') { buf[l-1]=1; mbk_remove_1(buf); }
        return buf ;
      }
    }
  }
  return name;
}

char *mbk_decodevector(char *name)
{
  char res[4096];
  return namealloc(mbk_devect_sub(name, MBK_VECTOR_OPEN, MBK_VECTOR_CLOSE, MBK_VECTOR_SINGLE, res, isdigit));
}

char *mbk_decodeanyvector(char *name)
{
  char res[4096];
  return namealloc(mbk_devect_sub(name, MBK_VECTOR_OPEN, MBK_VECTOR_CLOSE, MBK_VECTOR_SINGLE, res, NULL));
}

char *mbk_devect(char *name, char *leftb, char *rightb)
{
  char res[4096];
  return namealloc(mbk_devect_sub(name, leftb, rightb, NULL, res, NULL));
}

char *mbk_vect_sub(char *name, char leftb, char rightb, char *buf)
{
  char *c;
  int i, j, done=0;

  i=j=0;
  while (name[i]!='\0')
  {
    if (name[i]==' ' && !done)
    {
      done=1;
      if (name[0]=='%' && leftb=='[') buf[j++]='\\';
      buf[j++]=leftb;
    }
    else
      buf[j++]=name[i];            
    i++;
  }
  if (done)
  {
    if (name[0]=='%' && rightb==']') buf[j++]='\\';
    buf[j++]=rightb;
  }
  buf[j++]='\0';
  return buf;
}

char *mbk_vect(char *name, char leftb, char rightb)
{
  char res[4096];
  return namealloc(mbk_vect_sub(name, leftb, rightb, res));
}


int mbk_ReadFlags(int varnum, mbk_options_pack_struct *gen_opack, int nbopt, int warn, int initval)
{
  char *str;
  int i;
  char *l;
  char buf[1024];
  int GEN_OPTIONS_PACK=initval;

  str=V_STR_TAB[varnum].VALUE;
  if (str!=NULL)
    {
      strcpy(buf,str);
      l=strtok(buf, ",");	
      while (l!=NULL)
        {
          for (i=0;i<nbopt; i++)
            if (strcasecmp(l, gen_opack[i].name)==0)
              {
                GEN_OPTIONS_PACK|=gen_opack[i].mask;
                break;
              }
            else if (l[0]=='!' && strcasecmp(&l[1], gen_opack[i].name)==0)
              {
                GEN_OPTIONS_PACK&=~gen_opack[i].mask;
                break;
              }
          if (i>=nbopt && warn)
            {
              fprintf(stderr,"warning: option '%s' is unknown\n", l);		
            }
          l=strtok(NULL, ",");
        }
    }
  return GEN_OPTIONS_PACK;
}

char* mbk_getvssname()
{
  static char  *ptvss=NULL ;
  char          buffer[BUFSIZE] ;
  int           i, l, p ;
  
  if( !ptvss ) {
  
    l = strlen( GLOBAL_VSS );
    
    /* Passe tous les caractères ':' ou '*' */
    for( i = 0 ; 
         i < l && ( GLOBAL_VSS[i] == ':' || GLOBAL_VSS[i] == '*' ) ;
         i++ 
         );
    
    if( i < l ) {
      /* retiens tous les caractères différents de ':' ou '*' */
      for( p = 0 ; 
           i < l && GLOBAL_VSS[i] != ':' && GLOBAL_VSS[i] != '*' ;
           i++, p++ 
           ) buffer[p] = GLOBAL_VSS[i] ;
    }

    buffer[p] = '\0' ;
    ptvss = namealloc( buffer );
  }
  
  return ptvss ;
}

char* mbk_getvddname()
{
  static char  *ptvdd=NULL ;
  char          buffer[BUFSIZE] ;
  int           i, l, p ;
  
  if( !ptvdd ) {
  
    l = strlen( GLOBAL_VDD );
    
    /* Passe tous les caractères ':' ou '*' */
    for( i = 0 ; 
         i < l && ( GLOBAL_VDD[i] == ':' || GLOBAL_VDD[i] == '*' ) ;
         i++ 
         );
    
    if( i < l ) {
      /* retiens tous les caractères différents de ':' ou '*' */
      for( p = 0 ; 
           i < l && GLOBAL_VDD[i] != ':' && GLOBAL_VDD[i] != '*' ;
           i++, p++ 
           ) buffer[p] = GLOBAL_VDD[i] ;
    }

    buffer[p] = '\0' ;
    ptvdd = namealloc( buffer );
  }
  
  return ptvdd ;
}

void mbk_dumpfile(FILE *infile, FILE *outfile, int level)
{
  char buf[32000];

  while (fgets(buf, 32000, infile)!=NULL)
  {
    avt_log(LOGCONFIG, level, "%s", buf);
  }
  avt_log(LOGCONFIG, level, "\n");
}

unsigned int mbk_get_a_seed()
{
  struct timeval tp;
  if (!gettimeofday(&tp, NULL))
    return tp.tv_sec+(tp.tv_usec<<21)+(getpid()<<15);
  else
    return time(NULL)+(getpid()<<15);
}


// algo Random Number generator 'Mersenne Twister'
// simplifie a mort par zinaps

#define MERS_N   1
#define MERS_M   1
#define MERS_R   31
#define MERS_U   11
#define MERS_S   7
#define MERS_T   15
#define MERS_L   18
#define MERS_A   0x9908B0DF
#define MERS_B   0x9D2C5680
#define MERS_C   0xEFC60000

#ifdef USE_DEFAULT_RANDR
unsigned int mbk_rand_r(unsigned int *mt)
{
 return rand_r(mt);
}
#else
unsigned int mbk_rand_r(unsigned int *mt) {
   // Generate 32 random bits
   unsigned int y;
//   int mti;

//   if (mti >= MERS_N) {
      // Generate MERS_N words at one time
      const unsigned int LOWER_MASK = (1LU << MERS_R) - 1;       // Lower MERS_R bits
      const unsigned int UPPER_MASK = 0xFFFFFFFF << MERS_R;      // Upper (32 - MERS_R) bits
      static const unsigned int mag01[2] = {0, MERS_A};

      int kk;
      for (kk=0; kk < MERS_N-MERS_M; kk++) {    
         y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
         mt[kk] = mt[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];}

      for (; kk < MERS_N-1; kk++) {    
         y = (mt[kk] & UPPER_MASK) | (mt[kk+1] & LOWER_MASK);
         mt[kk] = mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ mag01[y & 1];}      

      y = (mt[MERS_N-1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
      mt[MERS_N-1] = mt[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
//      mti = 0;
//   }

   y = mt[0];

#if 1
   // Tempering (May be omitted):
   y ^=  y >> MERS_U;
   y ^= (y << MERS_S) & MERS_B;
   y ^= (y << MERS_T) & MERS_C;
   y ^=  y >> MERS_L;
#endif

   return y;
}
#endif

int mbk_sumchar(char *name)
{
  int i, j;
  char c;
  if (name==NULL) return 0x12345;
  for (i=0, j=0; name[i]!='\0'; i++)
  {
    if (CASE_SENSITIVE!='Y') c=tolower(name[i]);
    else c=name[i];
    j+=c*(i+1);
  }
  return j;
}

void mbk_substenvvariable(char *ref, char *res)
{
  int i, j, k;
  char tc, *var;
  i=j=0;
  do
  {
    if (ref[i]=='$' && isalpha(ref[i+1]))
    {
      i++;
      k=i;
      while (ref[i]!='\0' && (isalnum(ref[i]) || ref[i]=='_')) i++;
      tc=ref[i];
      ref[i]='\0';
      var=getenv(&ref[k]);
      if (var==NULL)
        avt_errmsg (MBK_ERRMSG, "069", AVT_ERROR, &ref[k]);
      else
        for (k=0; var[k]!='\0'; k++) res[j++]=var[k];
      ref[i]=tc;
    }
    else
    {
      res[j++]=ref[i++];
    }
  } while (ref[i]!='\0');        
  res[j]='\0';
}
