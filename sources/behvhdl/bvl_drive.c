#include <stdio.h>
#include <stdlib.h>
#include BEH_H
#include BHL_H
#include BVL_H

void 
vhdlsavebefig(befig_list *ptbefig, unsigned int trace_mode)
{
//    fprintf(stderr, "BVL : !* Attention *! vhdlsavebefig() devient savebefig()\n");
//    fprintf(stderr, "BVL : !* Attention *! il faut modifier les SOURCES\n");
    vbesavebefig(ptbefig, trace_mode);
    return;
}
