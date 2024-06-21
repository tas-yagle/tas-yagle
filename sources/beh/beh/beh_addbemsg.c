
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_addbemsg.c					*/
/* date		: Nov  6 1995						*/
/* version	: v109							*/
/* authors	: Pirouz BAZARGAN SABET					*/
/* content	: low-level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include MUT_H
#include LOG_H
#include BEH_H

/* ###--------------------------------------------------------------### */
/* function	: beh_addbemsg						*/
/* description	: create BEMSG structure at the top the list		*/
/* called func.	: mbkalloc, namealloc					*/
/* ###--------------------------------------------------------------### */

bemsg_list *beh_addbemsg (lastbemsg,label,level,message,abl_expr,bdd_expr)

bemsg_list *lastbemsg;	/* pointer on the last bemsg structure	*/
char         *label;		/* assert's label			*/
char          level;		/* severity level (E or W)		*/
char         *message;		/* reported message			*/
struct chain *abl_expr;		/* assert's condition (ABL)		*/
struct node  *bdd_expr;		/* assert's condition (BDD)		*/

  {

  struct bemsg *ptmsg;

  label          = namealloc (label);

  ptmsg          = (struct bemsg *) mbkalloc (sizeof(struct bemsg));
  ptmsg->LABEL   = label;
  ptmsg->LEVEL   = level;
  ptmsg->MESSAGE = message;
  ptmsg->ABL     = abl_expr;
  ptmsg->NODE    = bdd_expr;
  ptmsg->NEXT    = lastbemsg;

  return (ptmsg);
  }
