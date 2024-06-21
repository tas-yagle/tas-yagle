#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include MSL_H
#include AVT_H
#include INF_H
#include ELP_H
#include CNS_H
#include YAG_H
#include TRC_H
#include STM_H
#include TTV_H
#include MCC_H
#include FCL_H

#define API_USE_REAL_TYPES
#include "yagle_API.h"
#include "../../api/ams/ams_API.h"

void yagle (char *figname)
{
    mbkenv();
    cnsenv();
    yagenv(NULL);
    fclenv();
    rcnenv();
    rcx_env();

    elpenv();
    mccenv();
    stmenv();
    elpLoadOnceElp();

    cbhenv();

    YAG_CONTEXT->YAG_FIGNAME = namealloc (figname);
    YAG_CONTEXT->YAG_FILENAME = NULL;
    
    yagle_main ();

    ams_SetContext(getenv("YAGLE_OUT_NAME"),NULL,namealloc(figname));
    
    if(YAG_CONTEXT){
        mbkfree (YAG_CONTEXT);
        YAG_CONTEXT = NULL;
    }
}
