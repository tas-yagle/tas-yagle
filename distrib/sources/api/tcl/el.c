/*	$LAAS: el.c,v 1.2 2003/07/08 15:47:17 mallet Exp $ */

/* 
 * Copyright (c) 2001 LAAS/CNRS                       --  Sun Oct 14 2001
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
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "eltclsh.h"

/*
 * elTclGets ------------------------------------------------------------
 *
 * Implements the el::gets function (wrapper around el_gets)
 */

int
elTclGets(ClientData data, Tcl_Interp *interp,
	  int objc, Tcl_Obj *const objv[])
{
   ElTclInterpInfo *iinfo = data;
   const char *line;
   HistEvent ev;
   int length;
   int code = TCL_OK;

   if (objc != 2) {
      Tcl_WrongNumArgs(interp, 1, objv, "prompt");
      return TCL_ERROR;
   }

   /* set static prompt */
   Tcl_IncrRefCount(objv[1]);
   iinfo->promptString = objv[1];

   /* switch history context */
   el_set(iinfo->el, EL_HIST, history, iinfo->askaHistory);

   /* read one line */
   line = el_gets(iinfo->el, &length);
   if (line == NULL) {
      Tcl_SetResult(interp, "no input", TCL_STATIC);
      code = TCL_ERROR;
   } else {
      Tcl_SetObjResult(interp, Tcl_NewStringObj(line, length-1));
      history(iinfo->askaHistory, &ev, H_ENTER, line);
   }

   /* set editline back to normal state */
   el_set(iinfo->el, EL_HIST, history, iinfo->history);
   iinfo->promptString = NULL;
   Tcl_DecrRefCount(objv[1]);
   return code;
}


/*
 * elTclGetc ------------------------------------------------------------
 *
 * Implements the el::getc function (wrapper around el_getc)
 */

int
elTclGetc(ClientData data, Tcl_Interp *interp,
	  int objc, Tcl_Obj *const objv[])
{
   ElTclInterpInfo *iinfo = data;
   char c;

   if (objc != 1) {
      Tcl_WrongNumArgs(interp, 1, objv, NULL);
      return TCL_ERROR;
   }

   /* read one char */
   if (el_getc(iinfo->el, &c) < 0) {
      Tcl_SetResult(interp, "no input", TCL_STATIC);
      return TCL_ERROR;
   }

   Tcl_SetObjResult(interp, Tcl_NewStringObj(&c, 1));
   return TCL_OK;
}


/*
 * elTclHistory ---------------------------------------------------------
 *
 * Add an event to the history
 */

int
elTclHistory(ClientData data, Tcl_Interp *interp,
	     int objc, Tcl_Obj *const objv[])
{
   ElTclInterpInfo *iinfo = data;
   char *string;
   HistEvent ev;

   if (objc != 3) {
      Tcl_WrongNumArgs(interp, 1, objv, "add string");
      return TCL_ERROR;
   }

   if (strcmp(Tcl_GetStringFromObj(objv[1], NULL), "add")) {
      Tcl_WrongNumArgs(interp, 1, objv, "add string");
      return TCL_ERROR;
   }
   
   string = Tcl_GetStringFromObj(objv[2], NULL);
   if (string == NULL) {
      Tcl_WrongNumArgs(interp, 1, objv, "add string");
      return TCL_ERROR;
   }

   history(iinfo->history, &ev, H_ENTER, string);
   return TCL_OK;
}


/*
 * elTclEventLoop -------------------------------------------------------
 *
 * editline read_char function: loop on Tcl_DoOneEvent until there's some
 * input to process.
 */

int
elTclEventLoop(EditLine *el, char *c)
{
   ElTclInterpInfo *iinfo;
   el_get(el, EL_CLIENTDATA, &iinfo);
   
   /* process Tcl events until there's some input available */
   while (iinfo->preReadSz == 0 && !feof(stdin))
      Tcl_DoOneEvent(0);

   if (iinfo->preReadSz == 0 && feof(stdin)) {
      /* eof and no pending character: leave */
      c[0] = '0';
      return -1;
   }
       
   c[0] = iinfo->preRead[0];
   if (iinfo->preReadSz-- > 0)
      memmove(iinfo->preRead, iinfo->preRead+1, iinfo->preReadSz);

   return 1;
}


/*
 * elTclRead ------------------------------------------------------------
 *
 * Non blocking read: let Tcl process background events if no char is
 * available for reading. Also let signals (i.e. tcl script associated
 * with signals) be processed in the middle of nowhere.
 * This function works in combination with the elTclEventLoop function.
 */

void
elTclRead(ClientData data, int mask)
{
   ElTclInterpInfo *iinfo = data;
   int nRead;

   if (iinfo->preReadSz >= ELTCL_RDSZ) return /* buffer is full */;

   /* we know that there is at least one character (since we're
    * called from a select() but there might be more than one. Thus we
    * try to compute the actual number and we keep on the safe side in
    * the case it is not possible to get that number */ 

#ifdef FIONREAD
   if (ioctl(0, FIONREAD, &nRead)) {
      nRead = 1;
   }
#else
   {
      struct termios attrs;

      nRead = ELTCL_RDSZ;
      if (!tcgetattr(0, &attrs)) {
	 if (nRead < attrs.c_cc[VMIN]) nRead = 1;
      }
   }
#endif

   if (iinfo->preReadSz + nRead >= ELTCL_RDSZ)
      nRead = ELTCL_RDSZ - iinfo->preReadSz;

   /* read that number of bytes */
   nRead = read(0, iinfo->preRead + iinfo->preReadSz, nRead);
   if (nRead > 0) iinfo->preReadSz += nRead;
}


/*
 * elTclGetWindowSize ---------------------------------------------------
 *
 * Compute current terminal size (comes from libedit). Return 0 on
 * success.
 */

int
elTclGetWindowSize(int fd, int *lins, int *cols)
{
#ifdef TIOCGWINSZ
   {
      struct winsize ws;
      if (ioctl(fd, TIOCGWINSZ, &ws) != -1) {
	 if (ws.ws_col && cols)
	    *cols = ws.ws_col;
	 if (ws.ws_row && lins)
	    *lins = ws.ws_row;
      }
      return 0;
   }
#endif
#ifdef TIOCGSIZE
   {
      struct ttysize ts;
      if (ioctl(fd, TIOCGSIZE, &ts) != -1) {
	 if (ts.ts_cols && cols)
	    *cols = ts.ts_cols;
	 if (ts.ts_lines && lins)
	    *lins = ts.ts_lines;
      }
      return 0;
   }
#endif

   return -1;
}
