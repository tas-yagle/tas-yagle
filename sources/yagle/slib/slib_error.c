#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "slib_error.h"

#include SLB_H


void slib_error(int type, int line, char *string)
{
    switch(type) {
        case ERR_NO_EVAL :
            printf("SLIB ERROR : Unable to evaluate expression at line %d .\n",line);
            printf("\tNo value for \"%s\" , make sure every function and variable are correctly defined before being used.\n",string);    
            break;
        case ERR_NO_REF_HEIGHT :
            printf("SLIB ERROR : Required AND_HEIGHT is missing.\n");
            printf("\tPlease specify AND_HEIGHT in the .slib file.\n");
            break;
        case ERR_NO_SYMBOL :
            printf("SLIB ERROR : No symbol \"%s\" previously defined at line %d .\n",string,line);
            printf("\tMake sure every symbol were defined before being used as a sub-symbol.\n");
            break;
        case ERR_NO_GRID :
            printf("SLIB ERROR : Required set_route_grid() is missing or cannot be evaluated at line %d .\n",line);
            printf("\tPlease specify the gird with a value power of 2.\n");
            break;
        case ERR_PIN_DIR :
            printf("SLIB ERROR : Invalide pin direction at line %d .\n",line);
            printf("\t%s is not a valide pin direction.\n",string);
            break;
        }
}
