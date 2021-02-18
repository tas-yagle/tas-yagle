#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include AVT_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BGL_H
#include "bgl_type.h"
#include "bgl_parse.h"
#include "bgl_error.h"
#include "bgl_util.h"

int             BGL_CASE_SENSITIVE;
ht             *BGL_CASEHT = NULL;

int             BGL_USE_LIBRARY = 0;

int             BGL_AUXMOD = 1;

char *vlogfindbefig(char *name)
{
  char                 *suffix     ;
  suffix = V_STR_TAB[__BGL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = namealloc("v");

  return filepath(name, suffix);
}

/* ###--------------------------------------------------------------### */
/* function     : vlogloadbefig                                         */
/* ###--------------------------------------------------------------### */

befig_list *
vlogloadbefig(befig_list *pt_lastbefig, char *figname, unsigned int trace_mode)
{
    bgl_bcompcontext *ptcontext;
    void           *ptparser;

    befig_list     *ptbefig;
    char           *str, *suffix;
    unsigned int    check_mode;
    char            buffer[1024];
    FILE           *fp;

    bgl_bcompdebug = 0;

    str = getenv("BGL_CASE_SENSITIVE");
    if (str != NULL) {
        if ((str[0]=='y') || (str[0]=='Y')) {
            BGL_CASE_SENSITIVE = 1;
        }
        else BGL_CASE_SENSITIVE = 0; 
    }
    else BGL_CASE_SENSITIVE = 0; 

    suffix = V_STR_TAB[__BGL_FILE_SUFFIX].VALUE;
    if (suffix == NULL) suffix = namealloc("v");

    str = getenv("BGL_USE_LIBRARY");
    if (str != NULL) {
        if ((str[0]=='y') || (str[0]=='Y')) {
            BGL_USE_LIBRARY = 1;
        }
        else BGL_USE_LIBRARY = 0; 
    }
    else BGL_USE_LIBRARY = 0; 

    /* Open file */
    fp = mbkfopen(figname, suffix, READ_TEXT);
    if (fp == NULL) {
        if (suffix == NULL) sprintf(buffer, "%s", figname);
        else sprintf(buffer, "%s.%s", figname, suffix);
        avt_errmsg(BGL_ERRMSG,"033",AVT_FATAL,buffer);
//        fprintf(stderr, "\n*** beh error *** file does not exist : %s\n", buffer);
//        EXIT(1);
    }

    /* Initialization of some variables */
    ptparser = bgl_initparser(fp);
    ptcontext = bgl_getcontext(ptparser);
    strcpy(ptcontext->FILENAME, figname);
    ptcontext->LINENUM = 1;
    ptcontext->BEFIG = pt_lastbefig;
    ptcontext->TOPFIG = NULL;
    ptcontext->ERRFLG = 0;

    /* ###------------------------------------------------------### */
    /*    call the compiler on the current file                     */
    /*      - print a message if the trace mode is actif            */
    /*      - add internal signals to the primary input list if     */
    /*        "keep auxiliary" mode is actif                        */
    /* ###------------------------------------------------------### */

    if ((trace_mode & BGL_TRACE) != 0)
        beh_message(3, ptcontext->FILENAME);

    if ((trace_mode & BGL_KEEPAUX) != 0)
        BGL_AUXMOD = 1;
    else
        BGL_AUXMOD = 0;

    if (bgl_bcompparse(ptparser) != 0) {
//        fprintf(stderr, "\n*** bgl error *** abnormal parsing for : %s\n", ptcontext->FILENAME);
        bgl_bcompclean(ptcontext);
        bgl_delparser(ptparser);
        fclose(fp);
//        EXIT(1);
        avt_errmsg(BGL_ERRMSG,"034",AVT_FATAL,ptcontext->FILENAME);
    }

    ptbefig = ptcontext->TOPFIG;
    bgl_delparser(ptparser);
    fclose(fp);

    /* ###------------------------------------------------------### */
    /*    check the consistency of the compiled description         */
    /* ###------------------------------------------------------### */
    for (;ptbefig!=NULL;ptbefig=ptbefig->NEXT) {
        if ((trace_mode & BGL_CHECKEMPTY) != 0) {
            check_mode = BEH_CHK_EMPTY;
            ptbefig->FLAG = BEH_ARC_C;
        }
        else {
            check_mode = BEH_CHK_DRIVERS;
            ptbefig->FLAG = BEH_ARC_VHDL;
        }
        if (ptbefig->ERRFLG == 0) {
            ptbefig->ERRFLG = beh_chkbefig(ptbefig, check_mode);
        }
    }

    ptbefig = ptcontext->TOPFIG;

    return (ptbefig);
}
