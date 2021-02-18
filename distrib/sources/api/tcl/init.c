/*	$LAAS: init.c,v 1.11 2005/10/04 07:51:49 matthieu Exp $ */

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

#include "eltclsh.h"

char *avt_gettcldistpath ();

#if 0
#if !HAVE_STRLCAT
extern size_t strlcat(char *, const char *, size_t);
#endif
#if !HAVE_STRLCPY
extern size_t strlcpy(char *, const char *, size_t);
#endif
#endif

/* private functions */
static const char *	elTclPrompt(EditLine *el);


/*
 * elTclAppInit ---------------------------------------------------------
 *
 * Application-specific initialization: start libedit, source standard
 * procedures and create commands.
 */

int
elTclAppInit(ElTclInterpInfo *iinfo)
{
   Tcl_Channel inChannel;
   Tcl_Obj *obj;
   HistEvent ev;


   /* initialize the Tcl interpreter */
   if (Tcl_Init(iinfo->interp) == TCL_ERROR) {
      Tcl_SetResult(iinfo->interp, "cannot initialize tcl", TCL_STATIC);
      return TCL_ERROR;
   }

   if (!strcmp(Tcl_GetVar(iinfo->interp, "tcl_interactive", TCL_GLOBAL_ONLY),"0")) return TCL_OK;

   /* initialize InterpInfo structure */
   if (elTclGetWindowSize(fileno(stdout), NULL, &iinfo->windowSize) < 0) {
      iinfo->windowSize = 80;
   }
   iinfo->completionQueryItems = 100;
   iinfo->prompt1Name = Tcl_NewStringObj("el::prompt1", -1);
   Tcl_IncrRefCount(iinfo->prompt1Name);
   iinfo->prompt2Name = Tcl_NewStringObj("el::prompt2", -1);
   Tcl_IncrRefCount(iinfo->prompt2Name);
   iinfo->matchesName = Tcl_NewStringObj("el::matches", -1);
   Tcl_IncrRefCount(iinfo->matchesName);
   iinfo->promptString = NULL;
   iinfo->preReadSz = 0;

   if (elTclHandlersInit(iinfo) != TCL_OK) {
      fputs("warning: signal facility not created\n", stdout);
   }

   /* some useful commands */
   Tcl_CreateObjCommand(iinfo->interp, "exit", elTclExit, iinfo, NULL);
   Tcl_CreateObjCommand(iinfo->interp, "el::gets", elTclGets, iinfo, NULL);
   Tcl_CreateObjCommand(iinfo->interp, "el::getc", elTclGetc, iinfo, NULL);
   Tcl_CreateObjCommand(iinfo->interp, "el::history",
			elTclHistory, iinfo, NULL);
   Tcl_CreateObjCommand(iinfo->interp, "el::parse",
			elTclBreakCommandLine, iinfo, NULL);

#if 0
   Tcl_CreateObjCommand(iinfo->interp, "wrappedputs", rtclshWrappedPutsCmd, 0,
			NULL);
#endif


   /* and variables */
   Tcl_SetVar(iinfo->interp, "tcl_rcFileName", ".eltclshrc", TCL_GLOBAL_ONLY);

   obj = Tcl_NewStringObj("el::queryItems", -1);
   if (Tcl_LinkVar(iinfo->interp,
		   Tcl_GetStringFromObj(obj, NULL),
		   (char *)&iinfo->completionQueryItems,
		   TCL_LINK_INT) != TCL_OK) {
      return TCL_ERROR;
   }
   Tcl_PkgProvide(iinfo->interp, "el::base", "1.0");

   /* initialize libedit */
   iinfo->el = el_init(iinfo->argv0, stdin, stdout, stderr);
   if (iinfo->el == NULL) {
      Tcl_SetResult(iinfo->interp, "cannot initialize libedit", TCL_STATIC);
      return TCL_ERROR;
   }

   iinfo->history = history_init();
   history(iinfo->history, &ev, H_SETSIZE, 800);

   iinfo->askaHistory = history_init();
   history(iinfo->askaHistory, &ev, H_SETSIZE, 100);

   el_set(iinfo->el, EL_CLIENTDATA, iinfo);
   el_set(iinfo->el, EL_HIST, history, iinfo->history);
   el_set(iinfo->el, EL_EDITOR, "emacs");
   el_set(iinfo->el, EL_PROMPT, elTclPrompt);

   el_set(iinfo->el, EL_ADDFN,
	  "eltcl-complete", "Context sensitive argument completion",
	  elTclCompletion);
   el_set(iinfo->el, EL_BIND, "^I", "eltcl-complete", NULL);

   el_source(iinfo->el, NULL);
   el_get(iinfo->el, EL_EDITMODE, &iinfo->editmode);

   /* set up the non-blocking read stuff */
   inChannel = Tcl_GetStdChannel(TCL_STDIN);
   if (inChannel) {
      Tcl_CreateChannelHandler(inChannel, TCL_READABLE, elTclRead, iinfo);
      el_set(iinfo->el, EL_GETCFN, elTclEventLoop);
   }

   /* configure standard path for packages */
   obj = Tcl_NewListObj(0, NULL);
   Tcl_ListObjAppendElement(iinfo->interp, obj,
			    Tcl_NewStringObj(avt_gettcldistpath(), -1));
   Tcl_SetVar(iinfo->interp,
	      "eltcl_pkgPath", Tcl_GetString(obj), TCL_GLOBAL_ONLY);

   return TCL_OK;
}


/*
 * elTclPrompt ----------------------------------------------------------
 *
 * Compute prompt, base on current context
 */

static const char *
elTclPrompt(EditLine *el)
{
   ElTclInterpInfo *iinfo;
   Tcl_Obj *promptCmdPtr;
   static char buf[32];
   const char *prompt;
   int code;

   /* get context */
   el_get(el, EL_CLIENTDATA, &iinfo);

   /* see if a static prompt is defined */
   if (iinfo->promptString != NULL) {
      return Tcl_GetStringFromObj(iinfo->promptString, NULL);
   }

   /* compute prompt */
   promptCmdPtr = 
      Tcl_ObjGetVar2(iinfo->interp,
		     (iinfo->gotPartial?
		      iinfo->prompt2Name : iinfo->prompt1Name),
		     (Tcl_Obj *) NULL, TCL_GLOBAL_ONLY);
   if (promptCmdPtr == NULL) {
     defaultPrompt:
      if (!iinfo->gotPartial) {
	 strncpy(buf, iinfo->argv0, sizeof(buf)-4);
	 strncat(buf, " > ", sizeof(buf));
	 prompt = buf;
      } else
	 prompt = "» ";

   } else {
      code = Tcl_EvalObj(iinfo->interp, promptCmdPtr);

      if (code != TCL_OK) {
	 Tcl_Channel inChannel, outChannel, errChannel;

	 inChannel = Tcl_GetStdChannel(TCL_STDIN);
	 outChannel = Tcl_GetStdChannel(TCL_STDOUT);
	 errChannel = Tcl_GetStdChannel(TCL_STDERR);
	 if (errChannel) {
#if TCL_MAJOR_VERSION >= 8 && TCL_MINOR_VERSION >= 4
	    const char *bytes;
#else
	    char *bytes;
#endif /* TCL_VERSION */
	    bytes = Tcl_GetStringResult(iinfo->interp);
	    Tcl_Write(errChannel, bytes, strlen(bytes));
	    Tcl_Write(errChannel, "\n", 1);
	 }
	 Tcl_AddErrorInfo(iinfo->interp,
			  "\n    (script that generates prompt)");
	 goto defaultPrompt;
      }
	    
      prompt = Tcl_GetStringResult(iinfo->interp);
   }

   return prompt;
}
