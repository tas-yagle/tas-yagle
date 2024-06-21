/*	$LAAS: signal.c,v 1.4 2003/08/13 11:47:50 mallet Exp $ */

/* 
 * Copyright (c) 2001 LAAS/CNRS                       --  Wed Oct 10 2001
 * All rights reserved.                                    Anthony Mallet
 *
 * Redistribution and use  in source  and binary  forms,  with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   1. Redistributions of  source  code must retain the  above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice,  this list of  conditions and the following disclaimer in
 *      the  documentation  and/or  other   materials provided  with  the
 *      distribution.
 *
 * THIS  SOFTWARE IS PROVIDED BY  THE  COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND  ANY  EXPRESS OR IMPLIED  WARRANTIES,  INCLUDING,  BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR  PURPOSE ARE DISCLAIMED. IN  NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR      CONTRIBUTORS  BE LIABLE FOR   ANY    DIRECT, INDIRECT,
 * INCIDENTAL,  SPECIAL,  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF  SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN  CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE   OF THIS SOFTWARE, EVEN   IF ADVISED OF   THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

#include "eltclsh.h"

/* should be large enough */
#define ELTCL_MAXSIG 64

/* these two macros correspond to SIG_IGN and SIG_DFL and are used to
 * distinguish between a valid Tcl script and the particular behavior
 * they represent */
#define ELTCL_SIGIGN ((void *)-1)
#define ELTCL_SIGDFL ((void *)0)

/* signal names */
const char *signalNames[ELTCL_MAXSIG];

/* Context definition for signal handler's execution: it's an array
 * indexed by signal number. */
typedef struct ElTclSignalContext {
   Tcl_Obj *script;		/* tcl script, or one of ELTCL_SIGDFL or
				 * ELTCL_SIGIGN macros */
   ElTclInterpInfo *iinfo;	/* tcl interpreter in which to execute
				 * script */
   Tcl_AsyncHandler asyncH;	/* acync handler for the interpreter */

   struct ElTclSignalContext *next;
				/* in a multi-threaded context, this
				 * points to the next handler for this
				 * signal (possibly in another
				 * interpreter). This does not apply for
				 VxWorks since there is at most one
				 handler per task. */
} ElTclSignalContext;

/* Since there is no obvious way to make that context non-global, this
 * array is TaskVarAdd'ed under VxWorks. */ 
static ElTclSignalContext *signalContext[ELTCL_MAXSIG];

/* VxWorks doesn't have SIGWINCH, so we don't have to bother with
 * taskVars... */
#ifdef SIGWINCH
static ElTclSignalContext *sigWinchContext;
#endif


/* local prototypes */
static int			elTclSignal(ClientData data,
					Tcl_Interp *interp, int objc,
					Tcl_Obj *const objv[]);
static ElTclSignalContext *	getSignalContext(int signum, 
					ElTclInterpInfo *iinfo);
static ElTclSignalContext *	createSignalContext(int signum,
					ElTclInterpInfo *iinfo);
static int			asyncSignalHandler(ClientData data,
					Tcl_Interp *interp, int code);
static void			signalHandler(int signal);
static void			initSigNames(const char **signalNames);



/*
 * elTclHandlersInit ----------------------------------------------------
 *
 * Intialize signal handlers: create the list of signal names and the
 * signal procedure.
 */

int
elTclHandlersInit(ElTclInterpInfo *iinfo)
{
#ifdef SIGWINCH
   /* we must keep track of that one (for libedit and completion engine) */

   sigset_t set, oset;
   ElTclSignalContext *ctx = malloc(sizeof(*ctx));

   if (ctx != NULL) {
      sigemptyset(&set);
      sigaddset(&set, SIGWINCH);
      sigprocmask(SIG_BLOCK, &set, &oset);

      ctx->iinfo = iinfo;
      ctx->next = sigWinchContext;
      /* XXX this might not be atomic */
      sigWinchContext = ctx;

      signal(SIGWINCH, signalHandler);

      sigprocmask(SIG_SETMASK, &oset, NULL);
   }
#endif

#ifdef VXWORKS
#  error TaskVarAdd signalContext
#endif

   initSigNames(signalNames);
   Tcl_CreateObjCommand(iinfo->interp, "signal", elTclSignal, iinfo, NULL);
   return TCL_OK;
}


/*
 * elTclHandlersExit ----------------------------------------------------
 *
 * Free signal stuff
 */

void
elTclHandlersExit(ElTclInterpInfo *iinfo)
{
   ElTclSignalContext *ctx, *swp, *prev;
   sigset_t set, oset;
   int i;

   /* block all signals */
   sigfillset(&set);
   sigprocmask(SIG_BLOCK, &set, &oset);

   /* XXX must be protected... */
#ifdef SIGWINCH
   prev = NULL;
   for(ctx = sigWinchContext; ctx != NULL; ) {
      if (ctx->iinfo == iinfo) {
	 swp = ctx;
	 ctx = ctx->next;

	 if (prev == NULL)
	    sigWinchContext = ctx;
	 else 
	    prev->next = ctx;

	 free(swp);
      } else {
	 prev = ctx;
	 ctx = ctx->next;
      }
   }
#endif

   for(i=0; i<ELTCL_MAXSIG; i++) {
      prev = NULL;
      for(ctx = signalContext[i]; ctx != NULL; ) {
	 /* delete the handlers that are for us */ 
	 if (ctx->iinfo == iinfo) {
	    swp = ctx;
	    ctx = ctx->next;

	    if (prev == NULL)
	       signalContext[i] = ctx;
	    else 
	       prev->next = ctx;

	    if (swp->script != ELTCL_SIGDFL &&
		swp->script != ELTCL_SIGIGN) {
	       Tcl_DecrRefCount(swp->script);
	       Tcl_AsyncDelete(swp->asyncH);
	    }
	    free(swp);
	 } else {
	    prev = ctx;
	    ctx = ctx->next;
	 }
      }
   }

   /* unblock */
   sigprocmask(SIG_SETMASK, &oset, NULL);
}


/*
 * elTclSignal ----------------------------------------------------------
 *
 * Implements the "signal" procedure.
 */

static int
elTclSignal(ClientData data, Tcl_Interp *interp, int objc,
	    Tcl_Obj *const objv[])
{
   ElTclInterpInfo *iinfo = data;
   ElTclSignalContext *ctx;
   sigset_t set, oset;
   int i, signum;
   char *action;

   if (objc < 2 || objc > 3) {
      Tcl_WrongNumArgs(interp, 1, objv,
		       "signal ?script|-ignore|-default|-block|-unblock?");
      return TCL_ERROR;
   }

   if (objc == 2 &&
       !strcmp(Tcl_GetStringFromObj(objv[1], NULL), "names")) {
      /* [signal names] */
      Tcl_DString dstring;

      Tcl_DStringInit(&dstring);
      for(i=0; i<ELTCL_MAXSIG; i++) if (signalNames[i] != NULL) {
	 Tcl_DStringAppendElement(&dstring, signalNames[i]);
      }
      Tcl_DStringResult(interp, &dstring);
      return TCL_OK;
   }

   /* objv[1] must be a signal name */
   signum = -1;
   for(i=0; i<ELTCL_MAXSIG; i++) if (signalNames[i] != NULL)
      if (!strcmp(Tcl_GetStringFromObj(objv[1], NULL), signalNames[i])) {
	 signum = i;
	 break;
      }

   if (signum < 0) {
      /* or an integer */
      if (Tcl_GetIntFromObj(interp, objv[1], &signum) == TCL_ERROR)
	 return TCL_ERROR;
   }
   
   /* prepare the interpreter result so that this command returns the
    * previous action for that signal */
   ctx = getSignalContext(signum, iinfo);
   if (ctx == NULL || ctx->script == ELTCL_SIGDFL) {
      Tcl_SetResult(interp, "-default", TCL_STATIC);
   } else if (ctx->script == ELTCL_SIGIGN) {
      Tcl_SetResult(interp, "-ignore", TCL_STATIC);
   } else {
      Tcl_SetObjResult(interp, ctx->script);
   }

   /* if no action given, return current script associated with 
    * signal */
   if (objc == 2) { return TCL_OK; }

   /* get the given action */
   action = Tcl_GetStringFromObj(objv[2], NULL);

   /* check if signal should be reset to default */
   if (!strcmp(action, "-default")) {
      /* special case of SIGWINCH, which we must keep processing */
#ifdef SIGWINCH
      if (signum != SIGWINCH)
#endif
	 if (signal(signum, SIG_DFL) == (void *)-1) goto error;

      if (ctx == NULL) return TCL_OK;
     
      if (ctx->script != ELTCL_SIGDFL && ctx->script != ELTCL_SIGIGN) {
	 Tcl_DecrRefCount(ctx->script);
	 Tcl_AsyncDelete(ctx->asyncH);
      }
      ctx->script = ELTCL_SIGDFL;
      return TCL_OK;
   }

   /* check if signal should be ignored */
   if (!strcmp(action, "-ignore")) {
      if (ctx == NULL) {
	 ctx = createSignalContext(signum, iinfo);
	 if (ctx == NULL) goto error;
      }
      /* special case of SIGWINCH, which we must keep processing */
#ifdef SIGWINCH
      if (signum != SIGWINCH)
#endif
	 if (signal(signum, SIG_IGN) == (void *)-1) goto error;

      if (ctx->script != ELTCL_SIGDFL && ctx->script != ELTCL_SIGIGN) {
	 Tcl_DecrRefCount(ctx->script);
	 Tcl_AsyncDelete(ctx->asyncH);
      }
      ctx->script = ELTCL_SIGIGN;
      return TCL_OK;
   }

   /* check if signal should be (un)blocked */
   if (!strcmp(action, "-block") || !strcmp(action, "-unblock")) {
      Tcl_DString dstring;
      int code;

      sigemptyset(&set);
      sigemptyset(&oset);
      sigaddset(&set, signum);

      if (!strcmp(action, "-block"))
	 code = sigprocmask(SIG_BLOCK, &set, &oset);
      else
	 code = sigprocmask(SIG_UNBLOCK, &set, &oset);

      if (code) goto error;

      /* return the previous mask */
      Tcl_DStringInit(&dstring);
      for(i=0; i<ELTCL_MAXSIG; i++) if (signalNames[i] != NULL) {
	 if (sigismember(&oset, i))
	    Tcl_DStringAppendElement(&dstring, signalNames[i]);
      }
      Tcl_DStringResult(interp, &dstring);
      return TCL_OK;
   }

   /* a script was given: create async handler and register signal */

   if (ctx == NULL) {
      ctx = createSignalContext(signum, iinfo);
      if (ctx == NULL) goto error;
   }

   /* block signal while installing handler */
   sigemptyset(&set);
   sigaddset(&set, signum);
   if (sigprocmask(SIG_BLOCK, &set, &oset)) goto error;

#ifdef SIGWINCH
   if (signum != SIGWINCH)
#endif
      if (signal(signum, signalHandler) == (void *)-1) {
	 sigprocmask(SIG_SETMASK, &oset, NULL);
	 goto error;
      }

   if (ctx->script != ELTCL_SIGDFL && ctx->script != ELTCL_SIGIGN) {
      Tcl_DecrRefCount(ctx->script);
      Tcl_AsyncDelete(ctx->asyncH);
   }

   ctx->script = objv[2];
   Tcl_IncrRefCount(ctx->script);
   ctx->asyncH = Tcl_AsyncCreate(asyncSignalHandler, ctx);

   sigprocmask(SIG_SETMASK, &oset, NULL);
   return TCL_OK;

  error:
   Tcl_SetResult(interp, (char *)Tcl_ErrnoMsg(errno), TCL_VOLATILE);
   Tcl_SetErrno(errno);
   Tcl_PosixError(interp);
   return TCL_ERROR;
}


/*
 * getSignalContext -----------------------------------------------------
 *
 * Return the signal context for the given signal and interpreter, or one
 * of the ELTCL_SIGIGN or ELTCL_SIGDFL macros.
 */

static ElTclSignalContext *
getSignalContext(int signum, ElTclInterpInfo *iinfo)
{
   ElTclSignalContext *ctx;

   for(ctx = signalContext[signum]; ctx != NULL; ctx=ctx->next)
      if (ctx->iinfo == iinfo) return ctx;

   return NULL;
}


/*
 * createSignalContext --------------------------------------------------
 *
 * Create the signal context for the given signal and interpreter.
 */

static ElTclSignalContext *
createSignalContext(int signum, ElTclInterpInfo *iinfo)
{
   ElTclSignalContext *ctx;

   for(ctx = signalContext[signum]; ctx != NULL; ctx=ctx->next)
      if (ctx->iinfo == iinfo) return ctx;

   ctx = malloc(sizeof(*ctx));
   if (ctx == NULL) return NULL;

   ctx->script = ELTCL_SIGDFL;
   ctx->iinfo = iinfo;
   ctx->next = signalContext[signum];

   /* XXX this might not be atomic */
   signalContext[signum] = ctx;

   return ctx;
}


/*
 * asyncSignalHandler ---------------------------------------------------
 *
 * Tcl asynchronous signal handler. The ClientData is a pointer to the
 * eltcl signal context struture.
 */

static int
asyncSignalHandler(ClientData data, Tcl_Interp *interp, int code)
{
   ElTclSignalContext *ctx = data;
   Tcl_Obj *result, *errorInfo, *errorCode;

   if (ctx->script == ELTCL_SIGDFL || ctx->script == ELTCL_SIGIGN) {
      fputs("Warning: wrong signal delivered for Tcl\n", stdout);
      return code;
   }

   /* save interpreter state */
   result = Tcl_GetObjResult(ctx->iinfo->interp);
   if (result != NULL)  Tcl_IncrRefCount(result);
   errorInfo = Tcl_GetVar2Ex(ctx->iinfo->interp, "errorInfo", NULL,
			     TCL_GLOBAL_ONLY);
   if (errorInfo != NULL) Tcl_IncrRefCount(errorInfo);
   errorCode = Tcl_GetVar2Ex(ctx->iinfo->interp, "errorCode", NULL,
			     TCL_GLOBAL_ONLY);
   if (errorCode != NULL) Tcl_IncrRefCount(errorCode);

   /* eval script */
   if (Tcl_EvalObjEx(ctx->iinfo->interp,
		     ctx->script, TCL_EVAL_GLOBAL) != TCL_OK)
      Tcl_BackgroundError(ctx->iinfo->interp);


   /* restore interpreter state */
   if (errorInfo != NULL) {
      Tcl_SetVar2Ex(ctx->iinfo->interp, "errorInfo", NULL, errorInfo,
		    TCL_GLOBAL_ONLY);
      Tcl_DecrRefCount(errorInfo);
   }
   if (errorCode != NULL) {
      Tcl_SetVar2Ex(ctx->iinfo->interp, "errorCode", NULL, errorCode,
		    TCL_GLOBAL_ONLY);
      Tcl_DecrRefCount(errorCode);
   }
   if (result != NULL) {
      Tcl_SetObjResult(ctx->iinfo->interp, result);
      Tcl_DecrRefCount(result);
   }

   return code;
}


/*
 * signalHandler --------------------------------------------------------
 *
 * Simply mark the corresponding Tcl async handlers.
 */

static void
signalHandler(int signum)
{
   ElTclSignalContext *ctx;

#ifdef SIGWINCH
   for(ctx = sigWinchContext; ctx != NULL; ctx=ctx->next) {
      el_resize(ctx->iinfo->el);
      elTclGetWindowSize(1, NULL, &ctx->iinfo->windowSize);
   }
#endif

   for(ctx = signalContext[signum]; ctx != NULL; ctx=ctx->next)
      if (ctx->script != ELTCL_SIGDFL && ctx->script != ELTCL_SIGIGN)
	 Tcl_AsyncMark(ctx->asyncH);

#ifdef __svr4__ /* solaris 2 */
   signal(signum, signalHandler);
#endif
}


/*
 * initSigNames ---------------------------------------------------------
 *
 * Fill signalNames array with existing signals
 */

static void
initSigNames(const char **signalNames)
{
#ifdef SIGHUP
   signalNames[SIGHUP] = "HUP";
#endif
#ifdef SIGINT
   signalNames[SIGINT] = "INT";
#endif
#ifdef SIGQUIT
   signalNames[SIGQUIT] = "QUIT";
#endif
#ifdef SIGILL
   signalNames[SIGILL] = "ILL";
#endif
#ifdef SIGTRAP
   signalNames[SIGTRAP] = "TRAP";
#endif
#ifdef SIGABRT
   signalNames[SIGABRT] = "ABRT";
#endif
#ifdef SIGEMT
   signalNames[SIGEMT] = "EMT";
#endif
#ifdef SIGFPE
   signalNames[SIGFPE] = "FPE";
#endif
#ifdef SIGKILL
   signalNames[SIGKILL] = "KILL";
#endif
#ifdef SIGBUS
   signalNames[SIGBUS] = "BUS";
#endif
#ifdef SIGSEGV
   signalNames[SIGSEGV] = "SEGV";
#endif
#ifdef SIGSYS
   signalNames[SIGSYS] = "SYS";
#endif
#ifdef SIGPIPE
   signalNames[SIGPIPE] = "PIPE";
#endif
#ifdef SIGALRM
   signalNames[SIGALRM] = "ALRM";
#endif
#ifdef SIGTERM
   signalNames[SIGTERM] = "TERM";
#endif
#ifdef SIGURG
   signalNames[SIGURG] = "URG";
#endif
#ifdef SIGSTOP
   signalNames[SIGSTOP] = "STOP";
#endif
#ifdef SIGTSTP
   signalNames[SIGTSTP] = "TSTP";
#endif
#ifdef SIGCONT
   signalNames[SIGCONT] = "CONT";
#endif
#ifdef SIGCHLD
   signalNames[SIGCHLD] = "CHLD";
#endif
#ifdef SIGTTIN
   signalNames[SIGTTIN] = "TTIN";
#endif
#ifdef SIGTTOU
   signalNames[SIGTTOU] = "TTOU";
#endif
#ifdef SIGIO
   signalNames[SIGIO] = "IO";
#endif
#ifdef SIGXCPU
   signalNames[SIGXCPU] = "XCPU";
#endif
#ifdef SIGXFSZ
   signalNames[SIGXFSZ] = "XFSZ";
#endif
#ifdef SIGVTALRM
   signalNames[SIGVTALRM] = "VTALRM";
#endif
#ifdef SIGPROF
   signalNames[SIGPROF] = "PROF";
#endif
#ifdef SIGWINCH
   signalNames[SIGWINCH] = "WINCH";
#endif
#ifdef SIGINFO
   signalNames[SIGINFO] = "INFO";
#endif
#ifdef SIGUSR1
   signalNames[SIGUSR1] = "USR1";
#endif
#ifdef SIGUSR2
   signalNames[SIGUSR2] = "USR2";
#endif
#ifdef SIGPWR
   signalNames[SIGPWR] = "PWR";
#endif
#ifdef SIGPOLL
   signalNames[SIGPOLL] = "POLL";
#endif
#ifdef SIGSTP
   signalNames[SIGSTP] = "STP";
#endif
#ifdef SIGWAITING
   signalNames[SIGWAITING] = "WAITING";
#endif
#ifdef SIGLWP
   signalNames[SIGLWP] = "LWP";
#endif
#ifdef SIGFREEZE
   signalNames[SIGFREEZE] = "FREEZE";
#endif
#ifdef SIGTHAW
   signalNames[SIGTHAW] = "THAW";
#endif
#ifdef SIGCANCEL
   signalNames[SIGCANCEL] = "CANCEL";
#endif
}
