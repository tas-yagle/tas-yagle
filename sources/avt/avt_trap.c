/****************************************************************************/
/*                                                                          */
/*                      Chaine de verification                              */
/*                                                                          */
/*    Produit : AVT Version 1                                               */
/*    Fichier : avt_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1998-1999 AVERTEC                                       */
/*    Tous droits reserves                                                  */
/*    Support : e-mail support@avertec.com                                  */
/*                                                                          */
/*    Auteur(s) : AUGUSTINS Gilles                                          */
/*                                                                          */
/****************************************************************************/

#include "avt_headers.h"

chain_list *SEGV_MESSAGE = NULL;
chain_list *FPE_MESSAGE = NULL;
chain_list *INT_MESSAGE = NULL;

chain_list *SEGV_EXIT = NULL;
chain_list *FPE_EXIT = NULL;
chain_list *INT_EXIT = NULL;

chain_list *SEGV_CODE = NULL;
chain_list *FPE_CODE = NULL;
chain_list *INT_CODE = NULL;

typedef void (*t_pfunc)(int); 

t_pfunc orig_SEGV, orig_FPE, orig_BUS;
int trap_SEGV_count=0, trap_FPE_count=0;

static void
avt_DumpStack(void)
{
#ifdef Linux
    void *array[40];
    size_t size;
    char **strings;
    size_t i;

    size = backtrace (array, 40);
    strings = backtrace_symbols (array, size);

    printf ("Obtained %zd stack frames.\n", size);

    for (i = 0; i < size; i++) {
        printf ("%s\n", strings[i]);
    }

    free (strings);
#endif
#ifdef Solaris
    char buf[32];
    
    sprintf(buf, "/bin/pstack %ld", (long)getpid()); 
    system(buf);
#endif
}

/* ------------------------------------------------------------------------ */

char *GetMessage (chain_list *mlist)
{
    return (char*)mlist->DATA;
}

/* ------------------------------------------------------------------------ */

t_pfunc GetExit (chain_list *mlist)
{
    return (t_pfunc)mlist->DATA;
}

/* ------------------------------------------------------------------------ */
int GetCode (chain_list *mlist)
{
    return (int)(long)mlist->DATA;
}

/* ------------------------------------------------------------------------ */

void avt_Handler (int sig)
{
    char *segv_mes;
    char *fpe_mes;
    char *int_mes;
    int   force_core = 0;

#ifdef AVT_MORE_INFOS
    force_core = 1;
#endif

    if (!SEGV_MESSAGE) 
      {
        segv_mes = "Internal error, please contact Avertec support\n";
      }
    else
        segv_mes = GetMessage (SEGV_MESSAGE);
    
    if (!FPE_MESSAGE) 
        fpe_mes = "Internal error, please contact Avertec support\n";
    else
        fpe_mes = GetMessage (FPE_MESSAGE);

    if (!INT_MESSAGE) 
        int_mes = "Program aborted\n";
    else
        int_mes = GetMessage (INT_MESSAGE);

    // Reset colors
    if (AVT_COL)
        fprintf (stdout, "\x1B[0m");

    switch (sig) {
        case SIGBUS:
            avt_errmsg(AVT_ERRMSG,"027",-1,segv_mes);
            fflush (stderr);
            if (V_BOOL_TAB[__AVT_TRACE_FATAL].VALUE == 1)
                avt_DumpStack();
            if (V_BOOL_TAB[__AVT_ENABLE_CORE].VALUE == 1 || force_core)
                abort();
            else if (SEGV_EXIT)
                GetExit (SEGV_EXIT) (GetCode (SEGV_CODE));
            else {
                exit (1);
            }
            break;
        case SIGSEGV:
            avt_errmsg(AVT_ERRMSG,"027",-1,segv_mes);
            fflush (stderr);
            if (V_BOOL_TAB[__AVT_TRACE_FATAL].VALUE == 1)
                avt_DumpStack();
            if (V_BOOL_TAB[__AVT_ENABLE_CORE].VALUE == 1 || force_core)
                abort();
            else if (SEGV_EXIT)
                GetExit (SEGV_EXIT) (GetCode (SEGV_CODE));
            else {
                exit (1);
            }
            break;
        case SIGFPE:
            avt_errmsg(AVT_ERRMSG,"027",-1,fpe_mes);
            fflush (stderr);
            if (V_BOOL_TAB[__AVT_TRACE_FATAL].VALUE == 1)
                avt_DumpStack();
            if (V_BOOL_TAB[__AVT_ENABLE_CORE].VALUE == 1 || force_core)
                abort();
            if (FPE_EXIT)
                GetExit (FPE_EXIT) (GetCode (FPE_CODE));
            else
                exit (1);
            break;
        case SIGINT:
            avt_errmsg(AVT_ERRMSG,"027",-1,int_mes);
            fflush (stderr);
            if (INT_EXIT)
                GetExit (INT_EXIT) (GetCode (INT_CODE));
            else
                exit (1);
            break;
    }
}
            
/* ------------------------------------------------------------------------ */

void avt_TrapSegV ()
{
  if (trap_SEGV_count==0)
    {
      orig_SEGV = signal (SIGSEGV, avt_Handler);
      
#ifndef MACOS
      orig_BUS = signal (SIGBUS, avt_Handler);
#endif
    }
    trap_SEGV_count++;
}

void avt_UnTrapSegV ()
{
    trap_SEGV_count--;
    if (trap_SEGV_count==0)
      {
	signal (SIGSEGV, orig_SEGV);
#ifndef MACOS
	signal (SIGBUS, orig_BUS);
#endif
      }
}
void avt_TrapFPE ()
{
  if (trap_FPE_count==0)
    orig_FPE =  signal (SIGFPE, avt_Handler);
  trap_FPE_count++;
}

void avt_UnTrapFPE ()
{
  trap_FPE_count--;
   if (trap_FPE_count==0)
      signal (SIGSEGV, orig_FPE);
}

void avt_TrapKill ()
{
    signal (SIGINT, avt_Handler);
}

/* ------------------------------------------------------------------------ */

void avt_PushSegVExit (void (*fexit)(int), int code)
{
    SEGV_EXIT = addchain (SEGV_EXIT, fexit);
    SEGV_CODE = addchain (SEGV_CODE, (void*)(long)code);
}

void avt_PopSegVExit ()
{
    if (SEGV_EXIT)
        SEGV_EXIT = delchain (SEGV_EXIT, SEGV_EXIT);
    if (SEGV_CODE)
        SEGV_CODE = delchain (SEGV_CODE, SEGV_CODE);
}

/* ------------------------------------------------------------------------ */

void avt_PushFPEExit (void (*fexit)(int), int code)
{
    FPE_EXIT = addchain (FPE_EXIT, fexit);
    FPE_CODE = addchain (FPE_CODE, (void*)(long)code);
}

void avt_PopFPEExit ()
{
    if (FPE_EXIT)
        FPE_EXIT = delchain (FPE_EXIT, FPE_EXIT);
    if (FPE_CODE)
        FPE_CODE = delchain (FPE_CODE, FPE_CODE);
}

/* ------------------------------------------------------------------------ */

void avt_PushKillExit (void (*fexit)(int), int code)
{
    INT_EXIT = addchain (INT_EXIT, fexit);
    INT_CODE = addchain (INT_CODE, (void*)(long)code);
}

void avt_PopKillExit ()
{
    if (INT_EXIT)
        INT_EXIT = delchain (INT_EXIT, INT_EXIT);
    if (INT_CODE)
        INT_CODE = delchain (INT_CODE, INT_CODE);
}

/* ------------------------------------------------------------------------ */

void avt_PushSegVMessage (const char *message)
{
    SEGV_MESSAGE = addchain (SEGV_MESSAGE, message);
}

void avt_PopSegVMessage ()
{
    if (SEGV_MESSAGE)
        SEGV_MESSAGE = delchain (SEGV_MESSAGE, SEGV_MESSAGE);
}

/* ------------------------------------------------------------------------ */

void avt_PushFPEMessage (const char *message)
{
    FPE_MESSAGE = addchain (FPE_MESSAGE, message);
}
    
void avt_PopFPEMessage ()
{
    if (FPE_MESSAGE)
        FPE_MESSAGE = delchain (FPE_MESSAGE, FPE_MESSAGE);
}

/* ------------------------------------------------------------------------ */
    
void avt_PushKillMessage (const char *message)
{
    INT_MESSAGE = addchain (INT_MESSAGE, message);
}
    
void avt_PopKillMessage ()
{
    if (INT_MESSAGE)
        INT_MESSAGE = delchain (INT_MESSAGE, INT_MESSAGE);
}
/* ------------------------------------------------------------------------ */

    
