#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BVL_H

char            BVL_ERRFLG = 0;        /* if = 1 no structure is made  */
int             BVL_AUXMOD;        /* simplify internal sig (= 1)  */
int             BVL_CHECK;         /* activate coherency checks */
befig_list     *BVL_HEDFIG = NULL;

struct befig   *
vhdlloadbefig(pt_befig, figname, trace_mode)
    struct befig   *pt_befig;
    char           *figname;
    unsigned int    trace_mode;
{
    return (vbeloadbefig(pt_befig, figname, trace_mode));
}
