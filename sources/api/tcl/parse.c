/*	$LAAS: parse.c,v 1.3 2003/07/08 17:07:55 mallet Exp $ */

/* 
 * Copyright (c) 1997 Sun Microsystems, Inc.
 * Copyright (c) 1998 by Scriptics Corporation.
 * Copyright (C) 2001 LAAS/CNRS 
 *
 * This file contains procedures that  parse Tcl scripts. They do so in a
 * general-purpose fashion that can be  used for many different purposes,
 * including compilation, direct execution, code analysis, etc.
 *
 * These procedures are part  of the standard Tcl distribution.  However,
 * they have been slightly modified to  let them parse incomplete command
 * lines and for the completion mechanism.
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

#include <ctype.h>
#include <string.h>
#include <tcl.h>
#include "eltclsh.h"
#include "parse.h"

/*
 * The following table provides parsing information about each possible
 * 8-bit character.  The table is designed to be referenced with either
 * signed or unsigned characters, so it has 384 entries.  The first 128
 * entries correspond to negative character values, the next 256 correspond
 * to positive character values.  The last 128 entries are identical to the
 * first 128.  The table is always indexed with a 128-byte offset (the 128th
 * entry corresponds to a character value of 0).
 *
 * The macro CHAR_TYPE is used to index into the table and return
 * information about its character argument.  The following return
 * values are defined.
 *
 * TYPE_NORMAL -	All characters that don't have special significance
 *			to the Tcl parser.
 * TYPE_SPACE -		The character is a whitespace character other
 *			than newline.
 * TYPE_COMMAND_END -	Character is newline or semicolon.
 * TYPE_SUBS -		Character begins a substitution or has other
 *			special meaning in ParseTokens: backslash, dollar
 *			sign, open bracket, or null.
 * TYPE_QUOTE -		Character is a double quote.
 * TYPE_CLOSE_PAREN -	Character is a right parenthesis.
 * TYPE_CLOSE_BRACK -	Character is a right square bracket.
 * TYPE_BRACE -		Character is a curly brace (either left or right).
 */

#define TYPE_NORMAL		0
#define TYPE_SPACE		0x1
#define TYPE_COMMAND_END	0x2
#define TYPE_SUBS		0x4
#define TYPE_QUOTE		0x8
#define TYPE_CLOSE_PAREN	0x10
#define TYPE_CLOSE_BRACK	0x20
#define TYPE_BRACE		0x40

#define CHAR_TYPE(c) (typeTable+128)[(int)(c)]

static char typeTable[] = {
    /*
     * Negative character values, from -128 to -1:
     */

    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,

    /*
     * Positive character values, from 0-127:
     */

    TYPE_SUBS,        TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_SPACE,       TYPE_COMMAND_END, TYPE_SPACE,
    TYPE_SPACE,       TYPE_SPACE,       TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_SPACE,       TYPE_NORMAL,      TYPE_QUOTE,       TYPE_NORMAL,
    TYPE_SUBS,        TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_CLOSE_PAREN, TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_COMMAND_END,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_SUBS,
    TYPE_SUBS,        TYPE_CLOSE_BRACK, TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_BRACE,
    TYPE_NORMAL,      TYPE_BRACE,       TYPE_NORMAL,      TYPE_NORMAL,

    /*
     * Large unsigned character values, from 128-255:
     */

    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
    TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,      TYPE_NORMAL,
};

/*
 * Prototypes for local procedures defined in this file:
 */

static int	parseTokens(char *src, int mask, ElTclParse *parsePtr);
static void	expandTokenArray(ElTclParse *parsePtr);
static int	parseVarName(char *string, int numBytes,
			ElTclParse *parsePtr);
static int	parseBraces(char *string, int numBytes,
			ElTclParse *parsePtr, char ** termPtr);
static int	parseQuotedString(char *string, int numBytes,
			ElTclParse * parsePtr, char **termPtr);

/*
 *----------------------------------------------------------------------
 *
 * elTclParseCommand --
 *
 *	Given a  string, this procedure  parses the  first Tcl command in
 *	the string  and  returns information  about the  structure of the
 *	command.
 *
 * Results:
 *	The return value is TCL_OK if the command was parsed successfully
 *	and TCL_ERROR otherwise.   On  a successful  return, parsePtr  is
 *	filled  in with     information   about the   command that    was
 *	parsed. Note that unlike with the similar  Tcl function, an error
 *	is *not* returned if the script in an incomplete command.
 *
 * Side effects:
 *	If there is insufficient space in parsePtr to hold all the
 *	information about the command, then additional space is
 *	malloc-ed.  If the procedure returns TCL_OK then the caller must
 *	eventually invoke Tcl_FreeParse to release any additional space
 *	that was allocated.
 *
 *----------------------------------------------------------------------
 */

int
elTclParseCommand(string, numBytes, nested, parsePtr)
    char *string;		/* First character of string containing
				 * one or more Tcl commands.  The string
				 * must be in writable memory and must
				 * have one additional byte of space at
				 * string[length] where we can
				 * temporarily store a 0 sentinel
				 * character. */
    int numBytes;		/* Total number of bytes in string.  If < 0,
				 * the script consists of all bytes up to 
				 * the first null character. */
    int nested;			/* Non-zero means this is a nested command:
				 * close bracket should be considered
				 * a command terminator. If zero, then close
				 * bracket has no special meaning. */
    register ElTclParse *parsePtr;
    				/* Structure to fill in with information
				 * about the parsed command; any previous
				 * information in the structure is
				 * ignored. */
{
    register char *src;		/* Points to current character
				 * in the command. */
    int type;			/* Result returned by CHAR_TYPE(*src). */
    ElTclToken *tokenPtr;	/* Pointer to token being filled in. */
    int wordIndex;		/* Index of word token for current word. */
    char utfBytes[TCL_UTF_MAX];	/* Holds result of backslash substitution. */
    int terminators;		/* CHAR_TYPE bits that indicate the end
				 * of a command. */
    char *termPtr;		/* Set by Tcl_ParseBraces/QuotedString to
				 * point to char after terminating one. */
    int length, savedChar;


    if (numBytes < 0) {
	numBytes = (string? strlen(string) : 0);
    }

    parsePtr->commentStart = NULL;
    parsePtr->commentSize = 0;
    parsePtr->commandStart = NULL;
    parsePtr->commandSize = 0;
    parsePtr->numWords = 0;
    parsePtr->tokenPtr = parsePtr->staticTokens;
    parsePtr->numTokens = 0;
    parsePtr->tokensAvailable = NUM_STATIC_TOKENS;
    parsePtr->string = string;
    parsePtr->end = string + numBytes;
    parsePtr->term = parsePtr->end;
    parsePtr->incomplete = 0;
    parsePtr->errorType = TCL_PARSE_SUCCESS;

    if (nested != 0) {
       terminators = TYPE_COMMAND_END | TYPE_CLOSE_BRACK;
    } else {
       terminators = TYPE_COMMAND_END;
    }

    /*
     * Temporarily overwrite the character just after the end of the
     * string with a 0 byte.  This acts as a sentinel and reduces the
     * number of places where we have to check for the end of the
     * input string.  The original value of the byte is restored at
     * the end of the parse.
     */

    savedChar = string[numBytes];
    if (savedChar != 0) {
	string[numBytes] = 0;
    }

    /*
     * Parse any leading space and comments before the first word of the
     * command.
     */

    src = string;
    while (1) {
	while ((CHAR_TYPE(*src) == TYPE_SPACE) || (*src == '\n')) {
	    src++;
	}
	if ((*src == '\\') && (src[1] == '\n')) {
	    /*
	     * Skip backslash-newline sequence: it should be treated
	     * just like white space.
	     */

	    if ((src + 2) == parsePtr->end) {
		parsePtr->incomplete = 1;
	    }
	    src += 2;
	    continue;
	}
	if (*src != '#') {
	    break;
	}
	if (parsePtr->commentStart == NULL) {
	    parsePtr->commentStart = src;
	}
	while (1) {
	    if (src == parsePtr->end) {
		if (nested) {
		    parsePtr->incomplete = nested;
		}
		parsePtr->commentSize = src - parsePtr->commentStart;
		break;
	    } else if (*src == '\\') {
		if ((src[1] == '\n') && ((src + 2) == parsePtr->end)) {
		    parsePtr->incomplete = 1;
		}
		Tcl_UtfBackslash(src, &length, utfBytes);
		src += length;
	    } else if (*src == '\n') {
		src++;
		parsePtr->commentSize = src - parsePtr->commentStart;
		break;
	    } else {
		src++;
	    }
	}
    }

    /*
     * The following loop parses the words of the command, one word
     * in each iteration through the loop.
     */

    parsePtr->commandStart = src;
    while (1) {
	/*
	 * Create the token for the word.
	 */

	if (parsePtr->numTokens == parsePtr->tokensAvailable) {
	    expandTokenArray(parsePtr);
	}
	wordIndex = parsePtr->numTokens;
	tokenPtr = &parsePtr->tokenPtr[wordIndex];
	tokenPtr->type = TCL_TOKEN_WORD;

	/*
	 * Skip white space before the word. Also skip a backslash-newline
	 * sequence: it should be treated just like white space.
	 */

	while (1) {
	    type = CHAR_TYPE(*src);
	    if (type == TYPE_SPACE) {
		src++;
		continue;
	    } else if ((*src == '\\') && (src[1] == '\n')) {
		if ((src + 2) == parsePtr->end) {
		    parsePtr->incomplete = 1;
		}
		Tcl_UtfBackslash(src, &length, utfBytes);
		src += length;
		continue;
	    }
	    break;
	}
	if ((type & terminators) != 0) {
	    parsePtr->term = src;
	    src++;
	    break;
	}
	if (src == parsePtr->end) {
	    break;
	}
	tokenPtr->start = src;
	parsePtr->numTokens++;
	parsePtr->numWords++;

	/*
	 * At this point the word can have one of three forms: something
	 * enclosed in quotes, something enclosed in braces, or an
	 * unquoted word (anything else).
	 */

	if (*src == '"') {
	    if (parseQuotedString(src, (parsePtr->end - src),
				  parsePtr, &termPtr) != TCL_OK) {
		goto error;
	    }
	    src = termPtr;
	} else if (*src == '{') {
	    if (parseBraces(src, (parsePtr->end - src),
			    parsePtr, &termPtr) != TCL_OK) {
		goto error;
	    }
	    src = termPtr;
	} else {
	    /*
	     * This is an unquoted word.  Call ParseTokens and let it do
	     * all of the work.
	     */

	    if (parseTokens(src, TYPE_SPACE|terminators, 
		    parsePtr) != TCL_OK) {
		goto error;
	    }
	    src = parsePtr->term;
	}

	/*
	 * Finish filling in the token for the word and check for the
	 * special case of a word consisting of a single range of
	 * literal text.
	 */

	tokenPtr = &parsePtr->tokenPtr[wordIndex];
	tokenPtr->size = src - tokenPtr->start;
	tokenPtr->numComponents = parsePtr->numTokens - (wordIndex + 1);
	if ((tokenPtr->numComponents == 1)
		&& (tokenPtr[1].type == TCL_TOKEN_TEXT)) {
	    tokenPtr->type = TCL_TOKEN_SIMPLE_WORD;
	}

	/*
	 * Do two additional checks: (a) make sure we're really at the
	 * end of a word (there might have been garbage left after a
	 * quoted or braced word), and (b) check for the end of the
	 * command.
	 */

	type = CHAR_TYPE(*src);
	if (type == TYPE_SPACE) {
	    src++;
	    continue;
	} else {
	    /*
	     * Backslash-newline (and any following white space) must be
	     * treated as if it were a space character.
	     */

	    if ((*src == '\\') && (src[1] == '\n')) {
		if ((src + 2) == parsePtr->end) {
		    parsePtr->incomplete = 1;
		}
		Tcl_UtfBackslash(src, &length, utfBytes);
		src += length;
		continue;
	    }
	}

	if ((type & terminators) != 0) {
	    parsePtr->term = src;
	    src++;
	    break;
	}
	if (src == parsePtr->end) {
	    break;
	}
	if (src[-1] == '"') { 
	    parsePtr->errorType = TCL_PARSE_QUOTE_EXTRA;
	} else {
	    parsePtr->errorType = TCL_PARSE_BRACE_EXTRA;
	}
	break;
    }

    parsePtr->commandSize = src - parsePtr->commandStart;
    if (savedChar != 0) {
	string[numBytes] = (char) savedChar;
    }
    return TCL_OK;

    error:
    if (savedChar != 0) {
	string[numBytes] = (char) savedChar;
    }
    if (parsePtr->commandStart == NULL) {
	parsePtr->commandStart = string;
    }
    parsePtr->commandSize = parsePtr->term - parsePtr->commandStart;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * ParseTokens --
 *
 *	This procedure forms the heart of the Tcl parser.  It parses one
 *	or more tokens from a string, up to a termination point
 *	specified by the caller.  This procedure is used to parse
 *	unquoted command words (those not in quotes or braces), words in
 *	quotes, and array indices for variables.
 *
 * Results:
 *	Tokens are added to parsePtr and parsePtr->term is filled in
 *	with the address of the character that terminated the parse (the
 *	first one whose CHAR_TYPE matched mask or the character at
 *	parsePtr->end).  The return value is TCL_OK if the parse
 *	completed successfully and TCL_ERROR otherwise.
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
parseTokens(src, mask, parsePtr)
    register char *src;		/* First character to parse. */
    int mask;			/* Specifies when to stop parsing.  The
				 * parse stops at the first unquoted
				 * character whose CHAR_TYPE contains
				 * any of the bits in mask. */
    ElTclParse *parsePtr;	/* Information about parse in progress.
				 * Updated with additional tokens and
				 * termination information. */
{
    int type, originalTokens, varToken;
    char utfBytes[TCL_UTF_MAX];
    ElTclToken *tokenPtr;
    ElTclParse nested;

    /*
     * Each iteration through the following loop adds one token of
     * type TCL_TOKEN_TEXT, TCL_TOKEN_BS, TCL_TOKEN_COMMAND, or
     * TCL_TOKEN_VARIABLE to parsePtr.  For TCL_TOKEN_VARIABLE tokens,
     * additional tokens are added for the parsed variable name.
     */

    originalTokens = parsePtr->numTokens;
    while (1) {
	if (parsePtr->numTokens == parsePtr->tokensAvailable) {
	    expandTokenArray(parsePtr);
	}
	tokenPtr = &parsePtr->tokenPtr[parsePtr->numTokens];
	tokenPtr->start = src;
	tokenPtr->numComponents = 0;

	type = CHAR_TYPE(*src);
	if (type & mask) {
	    break;
	}

	if ((type & TYPE_SUBS) == 0) {
	    /*
	     * This is a simple range of characters.  Scan to find the end
	     * of the range.
	     */

	    while (1) {
		src++;
		if (CHAR_TYPE(*src) & (mask | TYPE_SUBS)) {
		    break;
		}
	    }
	    tokenPtr->type = TCL_TOKEN_TEXT;
	    tokenPtr->size = src - tokenPtr->start;
	    parsePtr->numTokens++;
	} else if (*src == '$') {
	    /*
	     * This is a variable reference.  Call ParseVarName to do
	     * all the dirty work of parsing the name.
	     */

	    varToken = parsePtr->numTokens;
	    if (parseVarName(src, parsePtr->end - src, parsePtr) != TCL_OK) {
	       return TCL_ERROR;
	    }
	    src += parsePtr->tokenPtr[varToken].size;
	} else if (*src == '[') {
	    /*
	     * Command substitution.  Call Tcl_ParseCommand recursively
	     * (and repeatedly) to parse the nested command(s), then
	     * throw away the parse information.
	     */

	    src++;
	    while (1) {
		if (elTclParseCommand(src, parsePtr->end - src,
				      1, &nested) != TCL_OK) {
		    parsePtr->errorType = nested.errorType;
		    parsePtr->term = nested.term;
		    parsePtr->incomplete = nested.incomplete;
		    return TCL_ERROR;
		}

		src = nested.commandStart + nested.commandSize;
		if (nested.tokenPtr != nested.staticTokens) {
		    ckfree((char *) nested.tokenPtr);
		}
		if ((*nested.term == ']') && !nested.incomplete) {
		    break;
		}
		if (src == parsePtr->end) {
		    parsePtr->errorType = TCL_PARSE_MISSING_BRACKET;
		    parsePtr->incomplete = 1;
		    break;
		}
	    }
	    tokenPtr->type = TCL_TOKEN_COMMAND;
	    tokenPtr->size = src - tokenPtr->start;
	    parsePtr->numTokens++;
	} else if (*src == '\\') {
	    /*
	     * Backslash substitution.
	     */

	    if (src[1] == '\n') {
		if ((src + 2) == parsePtr->end) {
		    parsePtr->incomplete = 1;
		}

		/*
		 * Note: backslash-newline is special in that it is
		 * treated the same as a space character would be.  This
		 * means that it could terminate the token.
		 */

		if (mask & TYPE_SPACE) {
		    break;
		}
	    }
	    tokenPtr->type = TCL_TOKEN_BS;
	    Tcl_UtfBackslash(src, &tokenPtr->size, utfBytes);
	    parsePtr->numTokens++;
	    src += tokenPtr->size;
	} else if (*src == 0) {
	    /*
	     * We encountered a null character.  If it is the null
	     * character at the end of the string, then return.
	     * Otherwise generate a text token for the single
	     * character.
	     */

	    if (src == parsePtr->end) {
		break;
	    }
	    tokenPtr->type = TCL_TOKEN_TEXT;
	    tokenPtr->size = 1;
	    parsePtr->numTokens++;
	    src++;
	} else {
	   elTclFreeParse(parsePtr);
	   return TCL_ERROR;
	}
    }
    if (parsePtr->numTokens == originalTokens) {
	/*
	 * There was nothing in this range of text.  Add an empty token
	 * for the empty range, so that there is always at least one
	 * token added.
	 */

	tokenPtr->type = TCL_TOKEN_TEXT;
	tokenPtr->size = 0;
	parsePtr->numTokens++;
    }
    parsePtr->term = src;
    return TCL_OK;
}

/*
 *----------------------------------------------------------------------
 *
 * elTclFreeParse --
 *
 *	This procedure is invoked to free any dynamic storage that may
 *	have been allocated by a previous call to Tcl_ParseCommand.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	If there is any dynamically allocated memory in *parsePtr,
 *	it is freed.
 *
 *----------------------------------------------------------------------
 */

void
elTclFreeParse(parsePtr)
    ElTclParse *parsePtr;	/* Structure that was filled in by a
				 * previous call to Tcl_ParseCommand. */
{
    if (parsePtr->tokenPtr != parsePtr->staticTokens) {
	ckfree((char *) parsePtr->tokenPtr);
	parsePtr->tokenPtr = parsePtr->staticTokens;
    }
}

/*
 *----------------------------------------------------------------------
 *
 * ExpandTokenArray --
 *
 *	This procedure is invoked when the current space for tokens in
 *	a Tcl_Parse structure fills up; it allocates memory to grow the
 *	token array
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	Memory is allocated for a new larger token array; the memory
 *	for the old array is freed, if it had been dynamically allocated.
 *
 *----------------------------------------------------------------------
 */

static void
expandTokenArray(parsePtr)
    ElTclParse *parsePtr;	/* Parse structure whose token space
				 * has overflowed. */
{
    int newCount;
    ElTclToken *newPtr;

    newCount = parsePtr->tokensAvailable*2;
    newPtr = (ElTclToken *) ckalloc((unsigned)(newCount*sizeof(ElTclToken)));
    memcpy((VOID *) newPtr, (VOID *) parsePtr->tokenPtr,
	    (size_t) (parsePtr->tokensAvailable * sizeof(ElTclToken)));
    if (parsePtr->tokenPtr != parsePtr->staticTokens) {
	ckfree((char *) parsePtr->tokenPtr);
    }
    parsePtr->tokenPtr = newPtr;
    parsePtr->tokensAvailable = newCount;
}

/*
 *----------------------------------------------------------------------
 *
 * ParseVarName --
 *
 *	Given a string starting with a $ sign, parse off a variable
 *	name and return information about the parse.
 *
 * Results:
 *	The return value is TCL_OK if the command was parsed successfully
 *	and  TCL_ERROR otherwise.  On a  successful  return, tokenPtr and
 *	numTokens fields of parsePtr are filled in with information about
 *	the variable name that was parsed.  The "size" field of the first
 *	new token gives the  total number of  bytes in the variable name.
 *	Other fields in parsePtr are undefined.
 *
 * Side effects:
 *	If there is insufficient space in parsePtr to hold all the
 *	information about the command, then additional space is
 *	malloc-ed.  If the procedure returns TCL_OK then the caller must
 *	eventually invoke Tcl_FreeParse to release any additional space
 *	that was allocated.
 *
 *----------------------------------------------------------------------
 */

static int
parseVarName(string, numBytes, parsePtr)
    char *string;		/* String containing variable name.  First
				 * character must be "$". */
    int numBytes;		/* Total number of bytes in string.  If < 0,
				 * the string consists of all bytes up to the
				 * first null character. */
    ElTclParse *parsePtr;	/* Structure to fill in with information
				 * about the variable name. */
{
    ElTclToken *tokenPtr;
    char *end, *src;
    unsigned char c;
    int varIndex, offset;
    Tcl_UniChar ch;
    unsigned array;

    if (numBytes >= 0) {
	end = string + numBytes;
    } else {
	end = string + strlen(string);
    }

    /*
     * Generate one token for the variable, an additional token for the
     * name, plus any number of additional tokens for the index, if
     * there is one.
     */

    src = string;
    if ((parsePtr->numTokens + 2) > parsePtr->tokensAvailable) {
	expandTokenArray(parsePtr);
    }
    tokenPtr = &parsePtr->tokenPtr[parsePtr->numTokens];
    tokenPtr->type = TCL_TOKEN_VARIABLE;
    tokenPtr->start = src;
    varIndex = parsePtr->numTokens;
    parsePtr->numTokens++;
    tokenPtr++;
    src++;
    if (src >= end) {
	goto justADollarSign;
    }
    tokenPtr->type = TCL_TOKEN_TEXT;
    tokenPtr->start = src;
    tokenPtr->numComponents = 0;

    /*
     * The name of the variable can have three forms:
     * 1. The $ sign is followed by an open curly brace.  Then 
     *    the variable name is everything up to the next close
     *    curly brace, and the variable is a scalar variable.
     * 2. The $ sign is not followed by an open curly brace.  Then
     *    the variable name is everything up to the next
     *    character that isn't a letter, digit, or underscore.
     *    :: sequences are also considered part of the variable
     *    name, in order to support namespaces. If the following
     *    character is an open parenthesis, then the information
     *    between parentheses is the array element name.
     * 3. The $ sign is followed by something that isn't a letter,
     *    digit, or underscore:  in this case, there is no variable
     *    name and the token is just "$".
     */

    if (*src == '{') {
	src++;
	tokenPtr->type = TCL_TOKEN_TEXT;
	tokenPtr->start = src;
	tokenPtr->numComponents = 0;
	while (1) {
	    if (src == end) {
		parsePtr->errorType = TCL_PARSE_MISSING_VAR_BRACE;
		parsePtr->incomplete = 1;
		break;
	    }
	    if (*src == '}') {
		break;
	    }
	    src++;
	}
	tokenPtr->size = src - tokenPtr->start;
	tokenPtr[-1].size = src - tokenPtr[-1].start;
	parsePtr->numTokens++;
	if (src < end) src++;
    } else {
	tokenPtr->type = TCL_TOKEN_TEXT;
	tokenPtr->start = src;
	tokenPtr->numComponents = 0;
	while (src != end) {
	    offset = Tcl_UtfToUniChar(src, &ch);
	    c = (unsigned char)(ch);
	    if (isalnum(c) || (c == '_')) { /* INTL: ISO only, UCHAR. */
		src += offset;
		continue;
	    }
	    if ((c == ':') && (((src+1) != end) && (src[1] == ':'))) {
		src += 2;
		while ((src != end) && (*src == ':')) {
		    src += 1;
		}
		continue;
	    }
	    break;
	}

	/*
	 * Support for empty array names here.
	 */
	array = ((src != end) && (*src == '('));
	tokenPtr->size = src - tokenPtr->start;
	if (tokenPtr->size == 0 && !array) {
	    goto justADollarSign;
	}
	parsePtr->numTokens++;
	if (array) {
	    /*
	     * This is a reference to an array element.  Call
	     * ParseTokens recursively to parse the element name,
	     * since it could contain any number of substitutions.
	     */

	    if (parseTokens(src+1, TYPE_CLOSE_PAREN, parsePtr)
		    != TCL_OK) {
		goto error;
	    }
	    if ((parsePtr->term == end) || (*parsePtr->term != ')')) { 
		parsePtr->errorType = TCL_PARSE_MISSING_PAREN;
		parsePtr->incomplete = 1;
		src = parsePtr->term;
	    } else
	       src = parsePtr->term + 1;
	}
    }
    tokenPtr = &parsePtr->tokenPtr[varIndex];
    tokenPtr->size = src - tokenPtr->start;
    tokenPtr->numComponents = parsePtr->numTokens - (varIndex + 1);
    return TCL_OK;

    /*
     * The dollar sign isn't followed by a variable name.
     * replace the TCL_TOKEN_VARIABLE token with a
     * TCL_TOKEN_TEXT token for the dollar sign.
     */

    justADollarSign:
    tokenPtr = &parsePtr->tokenPtr[varIndex];
    tokenPtr->type = TCL_TOKEN_TEXT;
    tokenPtr->size = 1;
    tokenPtr->numComponents = 0;
    return TCL_OK;

    error:
    elTclFreeParse(parsePtr);
    return TCL_ERROR;
}


/*
 *----------------------------------------------------------------------
 *
 * ParseBraces --
 *
 *	Given a string in braces such as a Tcl command argument or a string
 *	value in a Tcl expression, this procedure parses the string and
 *	returns information about the parse.
 *
 * Results:
 *	The return value is TCL_OK if  the string was parsed successfully
 *	and  TCL_ERROR otherwise. On a   successful return, tokenPtr  and
 *	numTokens fields of parsePtr are filled in with information about
 *	the  string that   was   parsed. Other   fields in parsePtr   are
 *	undefined. termPtr is set to  point to  the character just  after
 *	the last one in the braced string.
 *
 * Side effects:
 *	If there is insufficient space in parsePtr to hold all the
 *	information about the command, then additional space is
 *	malloc-ed. If the procedure returns TCL_OK then the caller must
 *	eventually invoke Tcl_FreeParse to release any additional space
 *	that was allocated.
 *
 *----------------------------------------------------------------------
 */

static int
parseBraces(string, numBytes, parsePtr, termPtr)
    char *string;		/* String containing the string in braces.
				 * The first character must be '{'. */
    int numBytes;		/* Total number of bytes in string. If < 0,
				 * the string consists of all bytes up to
				 * the first null character. */
    register ElTclParse *parsePtr;
    				/* Structure to fill in with information
				 * about the string. */
    char **termPtr;		/* If non-NULL, points to word in which to
				 * store a pointer to the character just
				 * after the terminating '}' if the parse
				 * was successful. */

{
    char utfBytes[TCL_UTF_MAX];	/* For result of backslash substitution. */
    ElTclToken *tokenPtr;
    register char *src, *end;
    int startIndex, level, length;

    if ((numBytes >= 0) || (string == NULL)) {
	end = string + numBytes;
    } else {
	end = string + strlen(string);
    }
    
    src = string+1;
    startIndex = parsePtr->numTokens;

    if (parsePtr->numTokens == parsePtr->tokensAvailable) {
	expandTokenArray(parsePtr);
    }
    tokenPtr = &parsePtr->tokenPtr[startIndex];
    tokenPtr->type = TCL_TOKEN_TEXT;
    tokenPtr->start = src;
    tokenPtr->numComponents = 0;
    level = 1;
    while (1) {
	while (CHAR_TYPE(*src) == TYPE_NORMAL) {
	    src++;
	}
	if (*src == '}') {
	    level--;
	    if (level == 0) {
		break;
	    }
	    src++;
	} else if (*src == '{') {
	    level++;
	    src++;
	} else if (*src == '\\') {
	    Tcl_UtfBackslash(src, &length, utfBytes);
	    if (src[1] == '\n') {
		/*
		 * A backslash-newline sequence must be collapsed, even
		 * inside braces, so we have to split the word into
		 * multiple tokens so that the backslash-newline can be
		 * represented explicitly.
		 */
		
		if ((src + 2) == end) {
		    parsePtr->incomplete = 1;
		}
		tokenPtr->size = (src - tokenPtr->start);
		if (tokenPtr->size != 0) {
		    parsePtr->numTokens++;
		}
		if ((parsePtr->numTokens+1) >= parsePtr->tokensAvailable) {
		    expandTokenArray(parsePtr);
		}
		tokenPtr = &parsePtr->tokenPtr[parsePtr->numTokens];
		tokenPtr->type = TCL_TOKEN_BS;
		tokenPtr->start = src;
		tokenPtr->size = length;
		tokenPtr->numComponents = 0;
		parsePtr->numTokens++;
		
		src += length;
		tokenPtr++;
		tokenPtr->type = TCL_TOKEN_TEXT;
		tokenPtr->start = src;
		tokenPtr->numComponents = 0;
	    } else {
		src += length;
	    }
	} else if (src == end) {
	    parsePtr->errorType = TCL_PARSE_MISSING_BRACE;
	    parsePtr->incomplete = 1;
	    break;
	} else {
	    src++;
	}
    }

    /*
     * Decide if we need to finish emitting a partially-finished token.
     * There are 3 cases:
     *     {abc \newline xyz} or {xyz}	- finish emitting "xyz" token
     *     {abc \newline}		- don't emit token after \newline
     *     {}				- finish emitting zero-sized token
     * The last case ensures that there is a token (even if empty) that
     * describes the braced string.
     */
    
    if ((src != tokenPtr->start)
	    || (parsePtr->numTokens == startIndex)) {
	tokenPtr->size = (src - tokenPtr->start);
	parsePtr->numTokens++;
    }
    if (termPtr != NULL) {
	*termPtr = src+1;
    }
    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * ParseQuotedString --
 *
 *	Given a double-quoted string such as a quoted Tcl command argument
 *	or a quoted value in a Tcl expression, this procedure parses the
 *	string and returns information about the parse.
 *
 * Results:
 *	The return value is TCL_OK  if the string was parsed successfully
 *	and TCL_ERROR otherwise.  On a  successful return, tokenPtr   and
 *	numTokens fields of parsePtr are filled in with information about
 *	the    string that  was  parsed. Other    fields in  parsePtr are
 *	undefined. termPtr is  set to point to  the  character just after
 *	the quoted string's terminating close-quote.
 *
 * Side effects:
 *	If there is insufficient space in parsePtr to hold all the
 *	information about the command, then additional space is
 *	malloc-ed. If the procedure returns TCL_OK then the caller must
 *	eventually invoke Tcl_FreeParse to release any additional space
 *	that was allocated.
 *
 *----------------------------------------------------------------------
 */

static int
parseQuotedString(string, numBytes, parsePtr, termPtr)
    char *string;		/* String containing the quoted string. 
				 * The first character must be '"'. */
    int numBytes;		/* Total number of bytes in string. If < 0,
				 * the string consists of all bytes up to
				 * the first null character. */
    register ElTclParse *parsePtr;
    				/* Structure to fill in with information
				 * about the string. */
    char **termPtr;		/* If non-NULL, points to word in which to
				 * store a pointer to the character just
				 * after the quoted string's terminating
				 * close-quote if the parse succeeds. */
{
    char *end;
    
    if ((numBytes >= 0) || (string == NULL)) {
	end = string + numBytes;
    } else {
	end = string + strlen(string);
    }
    
    if (parseTokens(string+1, TYPE_QUOTE, parsePtr) != TCL_OK) {
	goto error;
    }
    if (*parsePtr->term != '"') {
	parsePtr->errorType = TCL_PARSE_MISSING_QUOTE;
	parsePtr->incomplete = 1;
	if (termPtr != NULL) {
	   *termPtr = parsePtr->term;
	}
    } else {
       if (termPtr != NULL) {
	  *termPtr = (parsePtr->term + 1);
       }
    }
    return TCL_OK;

    error:
    elTclFreeParse(parsePtr);
    return TCL_ERROR;
}
