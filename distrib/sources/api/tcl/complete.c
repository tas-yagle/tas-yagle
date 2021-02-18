/*	"$LAAS: complete.c,v 1.3 2003/09/03 10:04:54 mallet Exp $" */

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
#include <math.h>

#include "eltclsh.h"

static int	appendToken(Tcl_DString *dstring, ElTclToken **token,
			char *line);
static char *	tokenTypeStr(int type);


/*
 * elTclCompletion ------------------------------------------------------
 *
 * Builtin command-line completion procedure
 */

unsigned char
elTclCompletion(EditLine *el, int ch)
{
   ElTclInterpInfo *iinfo;
   const LineInfo *linfo;
   Tcl_Obj *cmd[2], *buffer, *arg, **matches, *cmdLine;
   Tcl_Channel outChannel;
   int i,j,k, start, end, count, length0, length1, max, oldLen;
   char *string0, *string1, *old;
   int again, ncols, nitems;
   char c;

   /* get context */
   el_get(el, EL_CLIENTDATA, &iinfo);
   linfo = el_line(el);

   /* compute current command line: it is the concatenation of the
    * current command (any incomplete lines entered so far) plus the
    * current editline buffer */
   cmd[1] = Tcl_DuplicateObj(iinfo->command);
   cmdLine = Tcl_NewStringObj(linfo->buffer, linfo->cursor - linfo->buffer);
   Tcl_AppendObjToObj(cmd[1], cmdLine);
   Tcl_IncrRefCount(cmd[1]);
   Tcl_IncrRefCount(cmdLine);

   /* call the procedure that generates completion matches */
   cmd[0] = iinfo->matchesName;
   if (Tcl_EvalObjv(iinfo->interp,
		    2, cmd, TCL_EVAL_GLOBAL|TCL_EVAL_DIRECT) != TCL_OK) {

      outChannel = Tcl_GetStdChannel(TCL_STDOUT);
      fputc('\a', stdout);
      if (outChannel) {
	 buffer = Tcl_GetObjResult(iinfo->interp);
	 Tcl_Write(outChannel, "\n", 1);
	 Tcl_WriteObj(outChannel, buffer);
	 Tcl_Write(outChannel, "\n", 1);
	 Tcl_DecrRefCount(cmd[1]);
      }
      el_beep(el);
      return CC_REDISPLAY;
   }

   Tcl_DecrRefCount(cmd[1]);

   /* get result and display */
   buffer = Tcl_GetObjResult(iinfo->interp);
   Tcl_ListObjGetElements(iinfo->interp, buffer, &count, &matches);
   Tcl_GetIntFromObj(iinfo->interp, matches[0], &start);
   Tcl_GetIntFromObj(iinfo->interp, matches[1], &end);

   if (count == 2) {
      /* no match */
      Tcl_DecrRefCount(cmdLine);
      return CC_ERROR;
   }

   el_deletestr(el, linfo->cursor - linfo->buffer - start);

   if (count == 3) {
      /* unique match */
      Tcl_ListObjIndex(iinfo->interp, matches[2], 0, &arg);
      el_insertstr(el, Tcl_GetStringFromObj(arg, NULL));
      Tcl_ListObjIndex(iinfo->interp, matches[2], 1, &arg);
      el_insertstr(el, Tcl_GetStringFromObj(arg, NULL));
      Tcl_DecrRefCount(cmdLine);
      return CC_REFRESH;
   }

   if (count-2 > iinfo->completionQueryItems) {
      /* ask user */
      printf("\nDisplay all %d possibilit%s? [y/n] ",
	     count-2, count>3?"ies":"y");
      fflush(stdout);

      if (el_getc(iinfo->el, &c) <= 0) { 
	 fputc('\n', stdout);
	 Tcl_DecrRefCount(cmdLine);
	 return CC_REDISPLAY;
      }

      if (c != 'y' && c != 'Y') {
	 fputc(c, stdout);
	 fputc('\n', stdout);
	 Tcl_DecrRefCount(cmdLine);
	 return CC_REDISPLAY;
      }

      fputc(c, stdout);
   }

   /* find the smallest common part in every matches */
   Tcl_ListObjIndex(iinfo->interp, matches[2], 0, &cmd[0]);
   string0 = Tcl_GetStringFromObj(cmd[0], &length0);
   end = -1;
   do {
      end++;
      again = 1;
      if (end >= length0) { end--; break; }
 
      for(i=3; i<count; i++) {
	 Tcl_ListObjIndex(iinfo->interp, matches[i], 0, &cmd[1]);
	 string1 = Tcl_GetStringFromObj(cmd[1], &length1);
	 if (end >= length1 || string0[end] != string1[end]) {
	    again = 0;
	    end--;
	    break;
	 }
      }
   } while (again);

   old = Tcl_GetStringFromObj(cmdLine, &oldLen);
   if (end+start >= oldLen) {
      c = string0[end+1];
      string0[end+1] = 0;
      el_insertstr(el, string0);
      string0[end+1] = c;
   } else {
      el_insertstr(el, old + start);
   }
   Tcl_DecrRefCount(cmdLine);

   /* find the biggest match (for multicol display) */
   max = 0;
   for(i=2; i<count; i++) {
      Tcl_ListObjIndex(iinfo->interp, matches[i], 0, &arg);
      string0 = Tcl_GetStringFromObj(arg, &length0);

      Tcl_ListObjIndex(iinfo->interp, matches[i], 2, &arg);
      string0 = Tcl_GetStringFromObj(arg, &length1);
      if (length0+length1 > max) max = length0+length1;
   }
   max++;

   ncols = iinfo->windowSize / max;
   nitems = ceil((double)(count-2)/ncols);

   /* and display all matches */
   fputc('\n', stdout);
   for(i=0; i<nitems; i++) {
      for(j=0; j<ncols; j++) {
	 start = j*nitems+i + 2;
	 if (start >= count) break;

	 Tcl_ListObjIndex(iinfo->interp, matches[start], 0, &arg);
	 string0 = Tcl_GetStringFromObj(arg, &length0);
	 fputs(string0, stdout);

	 Tcl_ListObjIndex(iinfo->interp, matches[start], 2, &arg);
	 string1 = Tcl_GetStringFromObj(arg, &length1);
	 fputs(string1, stdout);
	 
	 for(k=length0+length1; k<max; k++) fputc(' ', stdout);
      }
      fputc('\n', stdout);
   }

   return CC_REDISPLAY;
}


/*
 * elTclBreakCommandLine ------------------------------------------------
 *
 * Parse the given string and return a list of parsed tokens.
 */

int
elTclBreakCommandLine(ClientData data, Tcl_Interp *interp,
		      int objc, Tcl_Obj *const objv[])
{
   ElTclToken *token;
   ElTclParse parse;
   Tcl_Obj *cmd, *num;
   Tcl_DString result;
   char *line, *start, c;
   int length, i;

   if (objc != 2) {
      Tcl_WrongNumArgs(interp, 1, objv, "string");
      return TCL_ERROR;
   }
    
   cmd = Tcl_DuplicateObj(objv[1]);
   start = line = Tcl_GetStringFromObj(cmd, &length);

   /* break current line up into its components */
   do {
      do {
	 if (elTclParseCommand(line, -1, 0, &parse) != TCL_OK) {
	    return TCL_ERROR;
	 }

	 /* parse the line until we get to the last portion */;
	 line = parse.commandStart + parse.commandSize;
      } while (*line != 0)

      /* parse the line until we get to the most nested sub command */;
      line = parse.tokenPtr[parse.numTokens-1].start + 1;
   } while (parse.tokenPtr[parse.numTokens-1].type == ELTCL_TOKEN_COMMAND
	    && parse.incomplete);

   /* build result */
   Tcl_DStringInit(&result);
   
   c = parse.commandStart[parse.commandSize];
   parse.commandStart[parse.commandSize] = 0;
   Tcl_DStringAppendElement(&result, parse.commandStart);
   parse.commandStart[parse.commandSize] = c;

   num = Tcl_NewIntObj(parse.commandStart - start);
   Tcl_DStringAppendElement(&result, Tcl_GetStringFromObj(num, NULL));
   num = Tcl_NewIntObj(parse.commandStart + parse.commandSize - start - 1);
   Tcl_DStringAppendElement(&result, Tcl_GetStringFromObj(num, NULL));

   Tcl_DStringStartSublist(&result);
   for(i=0, token=parse.tokenPtr;i<parse.numWords; i++) {
      appendToken(&result, &token, start);
   }
   Tcl_DStringEndSublist(&result);

   Tcl_DStringResult(interp, &result);
   return TCL_OK;
}


/*
 * appendToken ----------------------------------------------------------
 *
 * Build a list containing a description of the 'token' and append it
 * to the dstring.
 */

static int
appendToken(Tcl_DString *dstring, ElTclToken **token, char *line)
{
   char c;
   int i, n;
   Tcl_Obj *num;

   Tcl_DStringStartSublist(dstring);
   Tcl_DStringAppendElement(dstring, tokenTypeStr((*token)->type));

   c = (*token)->start[(*token)->size];
   (*token)->start[(*token)->size] = 0;
   Tcl_DStringAppendElement(dstring, (*token)->start);
   (*token)->start[(*token)->size] = c;

   num = Tcl_NewIntObj((*token)->start - line);
   Tcl_DStringAppendElement(dstring, Tcl_GetStringFromObj(num, NULL));
   num = Tcl_NewIntObj((*token)->start + (*token)->size - line - 1);
   Tcl_DStringAppendElement(dstring, Tcl_GetStringFromObj(num, NULL));

   Tcl_DStringStartSublist(dstring);
   n = (*token)->numComponents;
   for(i=0,(*token)++; i<n; i+=appendToken(dstring, token, line));
   Tcl_DStringEndSublist(dstring);

   Tcl_DStringEndSublist(dstring);

   return n+1;
}


/*
 * tokenTypeStr ---------------------------------------------------------
 *
 * Return a static string describing the token 'type'
 */

static char *
tokenTypeStr(int type)
{
   switch(type) {
      case TCL_TOKEN_WORD: return "word";
      case TCL_TOKEN_SIMPLE_WORD: return "simple-word";
      case TCL_TOKEN_TEXT: return "text";
      case TCL_TOKEN_BS: return "bs";
      case TCL_TOKEN_COMMAND: return "command";
      case TCL_TOKEN_VARIABLE: return "variable";
      case TCL_TOKEN_SUB_EXPR: return "sub expr";
      case TCL_TOKEN_OPERATOR: return "operator";
   }

   return "undefined";
}
