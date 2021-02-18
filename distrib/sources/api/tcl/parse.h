/*	$LAAS: parse.h,v 1.2 2003/07/08 15:47:17 mallet Exp $ */

/* 
 * Copyright (C) 2001 LAAS/CNRS 
 * All rights reserved.
 *
 * The  following  data  structures  and declarations   are for  the  new
 * customized  Tcl  parser. They   are almost the    same  as in  the Tcl
 * distribution but are duplicated here to prevent  future changes in the
 * undocumented part. Furthermore, these structure  did not exist in  Tcl
 * 8.0.
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
#ifndef H_PARSE
#define H_PARSE

/*
 * For each word of a command, and for each piece of a word such as a
 * variable reference, one of the following structures is created to
 * describe the token.
 */

typedef struct ElTclToken {
    int type;			/* Type of token, such as TCL_TOKEN_WORD;
				 * see below for valid types. */
    char *start;		/* First character in token. */
    int size;			/* Number of bytes in token. */
    int numComponents;		/* If this token is composed of other
				 * tokens, this field tells how many of
				 * them there are (including components of
				 * components, etc.).  The component tokens
				 * immediately follow this one. */
} ElTclToken;

/*
 * Type values defined for Tcl_Token structures.  These values are
 * defined as mask bits so that it's easy to check for collections of
 * types.
 *
 * ELTCL_TOKEN_WORD -		The token describes one word of a command,
 *				from the first non-blank character of
 *				the word (which may be " or {) up to but
 *				not including the space, semicolon, or
 *				bracket that terminates the word. 
 *				NumComponents counts the total number of
 *				sub-tokens that make up the word.  This
 *				includes, for example, sub-tokens of
 *				TCL_TOKEN_VARIABLE tokens.
 * ELTCL_TOKEN_SIMPLE_WORD -	This token is just like TCL_TOKEN_WORD
 *				except that the word is guaranteed to
 *				consist of a single TCL_TOKEN_TEXT
 *				sub-token.
 * ELTCL_TOKEN_TEXT -		The token describes a range of literal
 *				text that is part of a word. 
 *				NumComponents is always 0.
 * ELTCL_TOKEN_BS -		The token describes a backslash sequence
 *				that must be collapsed.	 NumComponents
 *				is always 0.
 * ELTCL_TOKEN_COMMAND -	The token describes a command whose result
 *				must be substituted into the word.  The
 *				token includes the enclosing brackets. 
 *				NumComponents is always 0.
 * ELTCL_TOKEN_VARIABLE -	The token describes a variable
 *				substitution, including the dollar sign,
 *				variable name, and array index (if there
 *				is one) up through the right
 *				parentheses.  NumComponents tells how
 *				many additional tokens follow to
 *				represent the variable name.  The first
 *				token will be a TCL_TOKEN_TEXT token
 *				that describes the variable name.  If
 *				the variable is an array reference then
 *				there will be one or more additional
 *				tokens, of type TCL_TOKEN_TEXT,
 *				TCL_TOKEN_BS, TCL_TOKEN_COMMAND, and
 *				TCL_TOKEN_VARIABLE, that describe the
 *				array index; numComponents counts the
 *				total number of nested tokens that make
 *				up the variable reference, including
 *				sub-tokens of TCL_TOKEN_VARIABLE tokens.
 * ELTCL_TOKEN_SUB_EXPR -	The token describes one subexpression of a
 *				expression, from the first non-blank
 *				character of the subexpression up to but not
 *				including the space, brace, or bracket
 *				that terminates the subexpression. 
 *				NumComponents counts the total number of
 *				following subtokens that make up the
 *				subexpression; this includes all subtokens
 *				for any nested TCL_TOKEN_SUB_EXPR tokens.
 *				For example, a numeric value used as a
 *				primitive operand is described by a
 *				TCL_TOKEN_SUB_EXPR token followed by a
 *				TCL_TOKEN_TEXT token. A binary subexpression
 *				is described by a TCL_TOKEN_SUB_EXPR token
 *				followed by the	TCL_TOKEN_OPERATOR token
 *				for the operator, then TCL_TOKEN_SUB_EXPR
 *				tokens for the left then the right operands.
 * ELTCL_TOKEN_OPERATOR -	The token describes one expression operator.
 *				An operator might be the name of a math
 *				function such as "abs". A TCL_TOKEN_OPERATOR
 *				token is always preceeded by one
 *				TCL_TOKEN_SUB_EXPR token for the operator's
 *				subexpression, and is followed by zero or
 *				more TCL_TOKEN_SUB_EXPR tokens for the
 *				operator's operands. NumComponents is
 *				always 0.
 */

#define ELTCL_TOKEN_WORD	1
#define ELTCL_TOKEN_SIMPLE_WORD	2
#define ELTCL_TOKEN_TEXT	4
#define ELTCL_TOKEN_BS		8
#define ELTCL_TOKEN_COMMAND	16
#define ELTCL_TOKEN_VARIABLE	32
#define ELTCL_TOKEN_SUB_EXPR	64
#define ELTCL_TOKEN_OPERATOR	128

/*
 * Parsing error types.  On any parsing error, one of these values
 * will be stored in the error field of the Tcl_Parse structure
 * defined below.
 */

#define ELTCL_PARSE_SUCCESS		0
#define ELTCL_PARSE_QUOTE_EXTRA		1
#define ELTCL_PARSE_BRACE_EXTRA		2
#define ELTCL_PARSE_MISSING_BRACE	3
#define ELTCL_PARSE_MISSING_BRACKET	4
#define ELTCL_PARSE_MISSING_PAREN	5
#define ELTCL_PARSE_MISSING_QUOTE	6
#define ELTCL_PARSE_MISSING_VAR_BRACE	7
#define ELTCL_PARSE_SYNTAX		8
#define ELTCL_PARSE_BAD_NUMBER		9

/*
 * A structure of the following type is filled in by Tcl_ParseCommand.
 * It describes a single command parsed from an input string.
 */

#define ELPARSE_NUM_STATIC_TOKENS 20

typedef struct ElTclParse {
    char *commentStart;		/* Pointer to # that begins the first of
				 * one or more comments preceding the
				 * command. */
    int commentSize;		/* Number of bytes in comments (up through
				 * newline character that terminates the
				 * last comment).  If there were no
				 * comments, this field is 0. */
    char *commandStart;		/* First character in first word of command. */
    int commandSize;		/* Number of bytes in command, including
				 * first character of first word, up
				 * through the terminating newline,
				 * close bracket, or semicolon. */
    int numWords;		/* Total number of words in command.  May
				 * be 0. */
    ElTclToken *tokenPtr;	/* Pointer to first token representing
				 * the words of the command.  Initially
				 * points to staticTokens, but may change
				 * to point to malloc-ed space if command
				 * exceeds space in staticTokens. */
    int numTokens;		/* Total number of tokens in command. */
    int tokensAvailable;	/* Total number of tokens available at
				 * *tokenPtr. */
    int errorType;		/* One of the parsing error types defined
				 * above. */

    /*
     * The fields below are intended only for the private use of the
     * parser.	They should not be used by procedures that invoke
     * Tcl_ParseCommand.
     */

    char *string;		/* The original command string passed to
				 * Tcl_ParseCommand. */
    char *end;			/* Points to the character just after the
				 * last one in the command string. */
    char *term;			/* Points to character in string that
				 * terminated most recent token.  Filled in
				 * by ParseTokens.  If an error occurs,
				 * points to beginning of region where the
				 * error occurred (e.g. the open brace if
				 * the close brace is missing). */
    int incomplete;		/* This field is set to 1 by Tcl_ParseCommand
				 * if the command appears to be incomplete.
				 * This information is used by
				 * Tcl_CommandComplete. */
    ElTclToken staticTokens[ELPARSE_NUM_STATIC_TOKENS];
				/* Initial space for tokens for command.
				 * This space should be large enough to
				 * accommodate most commands; dynamic
				 * space is allocated for very large
				 * commands that don't fit here. */
} ElTclParse;

#endif /* H_PARSE */
