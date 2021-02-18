#include <stdarg.h>
#include <stdio.h>
#include MUT_H
#include MLO_H
#include AVT_H
#include API_H

#include "gen_model_utils.h"
//#include "gen_search_utils.h"
#include "gen_search_recursive.h"

#include "gen_env.h"

char *GENIUS_TO_SPY=NULL, *GENIUS_SPY;

void gen_printf(int level, char *format, ...)
{
  va_list va;
  if (GOTO_MODE) return;
  if (level<0 || (GEN_DEBUG_LEVEL>level && (GENIUS_SPY==NULL || GENIUS_TO_SPY==NULL || GENIUS_TO_SPY==GENIUS_SPY)))
    {
      va_start(va,format);
      avt_trace_va(level,GENIUS_OUTPUT,format,va);
    }
}
