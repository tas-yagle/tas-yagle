#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <setjmp.h>
#include <limits.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>

extern "C"
{
#include AVT_H
  
#include MUT_H
#include MLO_H
#include MLU_H
#include MSL_H

#include ELP_H

#include YAG_H
#include FCL_H
#include GEN_H

lofig_list *yagCutLofig(lofig_list *ptlofig, chain_list *instances, lofig_list **ptpttopfig, int copytrans);

}

typedef struct
{
  char *code;
  int mode;
  void *var;
  char *desc;
} oneopt;

typedef struct bbox_list
{
  struct bbox_list *next;
  lofig_list *lf;
  ALL_FOR_GNS *gnsrun;
  chain_list *kept_instances;
} bbox_list;

extern char *BBOXPREFIX;

