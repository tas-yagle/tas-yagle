/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_env.c                                                   */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include AVT_H
#include MUT_H
#include MLO_H
#include API_H
#include "gen_model_utils.h"
#include "gen_env.h"

#ifndef LIB_NAME
#define LIB_NAME "LIBRARY"
#endif

#ifndef LIB_PATH
#define LIB_PATH "cells"        
#endif

#ifndef DEST_PATH
#define DEST_PATH "."        
#endif


char       *GENIUS_LIB_NAME;              /* file with all models names */
char       *GENIUS_LIB_PATH;              /* directory */
tree_list  *GENIUS_TREE      = NULL;      /* syntaxical result of model files */
lib_entry  *GENIUS_PRIORITY = NULL;      /* syntaxical result of library file*/
FILE       *GENIUS_OUTPUT    = NULL;          /* file for verbose disassembly */
int GEN_DEBUG_LEVEL = 0;
char *unit_M, *unit_UM, *unit_PM, *unit_NM, *unit_MM;
char *FAKE_TRAN_NAME, *FAKE_INS_NAME;

char *transistor_params[NB_TRAN_PARAM];
int GEN_OPTIONS_PACK=0;

static mbk_options_pack_struct gen_opack[]=
  {
    {"NoDefaultActions", GEN_NODEFAULT_ACTIONS},
    {"EnableCore", GEN_ENABLE_CORE},
    {"NoGns", GEN_NO_GNS},
    {"VerboseGns", GEN_VERBOSE_GNS},
    {"NoOrdering", GEN_NO_ORDERING},
    {"Strict", GEN_STRICT},
    {"DebugRemapping", GEN_DEBUG_REMAPPING}
  };

ExecutionContext *genius_ec;

/****************************************************************************/
/*               Set variables with the environment                         */
/****************************************************************************/
extern void genius_env(lofig_list* circuit, char *cellib, char *libname)
{
    char    *str = NULL;


    transistor_params[0]=namealloc("l");
    transistor_params[1]=namealloc("w");

    FAKE_TRAN_NAME=namealloc("fake_transistor");
    FAKE_INS_NAME=namealloc("fake_instance");

    GEN_OPTIONS_PACK=mbk_ReadFlags(__GENIUS_FLAGS, gen_opack, sizeof(gen_opack)/sizeof(*gen_opack), 1, 0);

/*get the directory where model files are*/
    if (cellib!=NULL)
      str=cellib;
    else
      str = V_STR_TAB[__GENIUS_LIB_PATH].VALUE;

    if (str == NULL) {
        /* WORK_LIB is a char* defined in MUT_H */
        GENIUS_LIB_PATH = mbkalloc(strlen(WORK_LIB)+strlen("/"LIB_PATH"/")+1);
        strcpy(GENIUS_LIB_PATH, WORK_LIB);
        strcat(GENIUS_LIB_PATH, "/"LIB_PATH"/");
    }
    else {
        GENIUS_LIB_PATH = mbkalloc(strlen(str)+strlen("/")+1);
        strcpy(GENIUS_LIB_PATH, str);
        strcat(GENIUS_LIB_PATH, "/");
    }        

   

/*get the file where are written all model files and priority*/
    if (libname!=NULL)
      str =libname;
    else
      str = V_STR_TAB[__GENIUS_LIB_NAME].VALUE;

    if (str == NULL) {/* library file is in cells directory */
      GENIUS_LIB_NAME = mbkalloc(strlen(GENIUS_LIB_PATH)+strlen(LIB_NAME)+1);
      strcpy(GENIUS_LIB_NAME,GENIUS_LIB_PATH);
      strcat(GENIUS_LIB_NAME,LIB_NAME);
    }
    else {
      if (str[0]!='/')
        {
          GENIUS_LIB_NAME = mbkalloc(strlen(GENIUS_LIB_PATH)+strlen(str)+1);
          strcpy(GENIUS_LIB_NAME, GENIUS_LIB_PATH);
          strcat(GENIUS_LIB_NAME, str);
        }
      else
        GENIUS_LIB_NAME= mbkstrdup(str);
    }        

/*get the trace mode*/    
    str = V_STR_TAB[__GEN_DEBUG_MODE].VALUE;
    if (str!=NULL)
    { 
      if (strcmp(str,"stderr")==0) GENIUS_OUTPUT=stderr;
      else if (strcmp(str,"stdout")==0) GENIUS_OUTPUT=stdout;
      else
      {
        if (strcmp(str,"null")==0)
        {
          GENIUS_OUTPUT = fopen("/dev/null","w");
          if (GENIUS_OUTPUT==NULL) avt_errmsg(GNS_ERRMSG, "025", AVT_FATAL);
        }
        else
        {
          GENIUS_OUTPUT = mbkfopen(str, NULL, WRITE_TEXT);
          if (!GENIUS_OUTPUT) avt_errmsg(GNS_ERRMSG, "024", AVT_FATAL, circuit->NAME);
        }
        if (GENIUS_OUTPUT==NULL) GENIUS_OUTPUT=stdout;
      }
    }
    else GENIUS_OUTPUT = stdout;

    GEN_DEBUG_LEVEL=V_INT_TAB[__GEN_DEBUG_LEVEL].VALUE;

}

void clean_gen_env()
{
  mbkfree(GENIUS_LIB_PATH);
  mbkfree(GENIUS_LIB_NAME);
}

