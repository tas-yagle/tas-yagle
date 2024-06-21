#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include AVT_H
#include MUT_H
#include BEH_H

char           *BEH_OUT = NULL;
char           *BEH_IN = NULL;

void          vbesavebefig (befig_list *ptbefig, unsigned int trace_mode);
void          vhdsavebefig (befig_list *ptbefig, unsigned int trace_mode);
void          vlogsavebefig (befig_list *ptbefig, unsigned int trace_mode);
struct befig *vbeloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
struct befig *vhdloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
struct befig *vlogloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
char *vlogfindbefig(char *name);
char *vhdfindbefig(char *name);

void 
savebefig(befig_list *ptbefig, unsigned int trace_mode)
{
    if (BEH_OUT == NULL) {
        BEH_OUT = V_STR_TAB[__BEH_OUT_FORMAT].VALUE;
    }

    if (BEH_OUT == NULL || strcmp(BEH_OUT, "vhd") == 0) {
        vhdsavebefig(ptbefig, trace_mode);
    }
    else if (strcmp(BEH_OUT, "vbe") == 0) {
        vbesavebefig(ptbefig, trace_mode);
    }
    else if (strcmp(BEH_OUT, "vlg") == 0) {
        vlogsavebefig(ptbefig, trace_mode);
    }
    else {
      avt_errmsg(BEF_ERRMSG,"000",AVT_ERROR,BEH_OUT);
      //        fprintf(stderr, "BHL : Behavior out format %s is not a legal format!\n", BEH_OUT);
      //        fprintf(stderr, "      Legal format are 'vhd' and 'vlg'\n");
      vlogsavebefig(ptbefig, trace_mode);
    }
}


char *findbefig(char *name)
{
  if (BEH_IN == NULL) {
    BEH_IN = V_STR_TAB[__BEH_IN_FORMAT].VALUE;
  }
  
  if (BEH_IN == NULL || strcmp(BEH_IN, "vhd") == 0) {
    return vhdfindbefig(name);
  }
  else if (strcmp(BEH_IN, "vbe") == 0) {
    return vhdfindbefig(name);
  }
  else if (strcmp(BEH_IN, "vlg") == 0) {
    return vlogfindbefig(name);
  }

  return NULL;
}

struct befig *
loadbefig(pt_befig, figname, trace_mode)
    struct befig   *pt_befig;
    char           *figname;
    unsigned int    trace_mode;
{
    if (BEH_IN == NULL) {
        BEH_IN = V_STR_TAB[__BEH_IN_FORMAT].VALUE;
    }

    if (BEH_IN == NULL || strcmp(BEH_IN, "vhd") == 0) {
        return (vhdloadbefig(pt_befig, figname, trace_mode));
    }
    else if (strcmp(BEH_IN, "vbe") == 0) {
        return (vbeloadbefig(pt_befig, figname, trace_mode));
    }
    else if (strcmp(BEH_IN, "vlg") == 0) {
        return (vlogloadbefig(pt_befig, figname, trace_mode));
    }
    else {
      avt_errmsg(BEF_ERRMSG,"001",AVT_ERROR,BEH_IN);
//        fprintf(stderr, "BVL : BEH_IN_FORMAT = %s is not a legal format !\n", BEH_IN);
//        EXIT(1);
    }
    return NULL;
}

char *
bef_getsuffix()
{
    char *str;

    if (BEH_IN == NULL) {
        BEH_IN = V_STR_TAB[__BEH_IN_FORMAT].VALUE;
    }

    if (BEH_IN == NULL || strcmp(BEH_IN, "vhd") == 0) {
        str = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
        if (str != NULL) return namealloc(str);
        else return namealloc("vhd");
    }
    else if (strcmp(BEH_IN, "vbe") == 0) {
        str = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
        if (str != NULL) return namealloc(str);
        else return namealloc("vbe");
    }
    else if (strcmp(BEH_IN, "vlg") == 0) {
        str = V_STR_TAB[__BGL_FILE_SUFFIX].VALUE;
        if (str != NULL) return namealloc(str);
        else return namealloc("v");
    }
    else {
      avt_errmsg(BEF_ERRMSG,"001",AVT_ERROR,BEH_IN);
//        fprintf(stderr, "BVL : BEH_IN_FORMAT = %s is not a legal format !\n", BEH_IN);
//        EXIT(1);
    }
    return NULL;
}

