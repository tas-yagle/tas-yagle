/*------------------------------------------------------------\
|                                                             |
| Tool    :                     Scg                           |
|                                                             |
| File    :                   scgmain.c                       |
|                                                             |
| Date    :                   28.04.98                        |
|                                                             |
| Authors :               Picault  Stephane                   |
|                         Miramond   Benoit                   |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

#include MUT_H
#include SLB_H
#include MLO_H
#include CGV_H

int CGV_MAKE_CELLS = 1;

cgvfig_list *
getcgvfig(char *FileName, long Type, char *filename)
{
    char *str = NULL;
        
            CGV_MAKE_CELLS = V_BOOL_TAB[__CGV_MAKE_CELLS].VALUE;
    
    if (Type & CGV_FROM_CNS) return getcgvfig_from_cnsfig(FileName, filename);
    else
      if (Type & CGV_FROM_LOFIG) return getcgvfig_from_lofig(FileName,filename);
    
    return NULL;
}
