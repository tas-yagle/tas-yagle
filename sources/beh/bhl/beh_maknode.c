
/* ###--------------------------------------------------------------### */
/* 									*/
/* file		: beh_maknode.c						*/
/* date		: Mar  23 1995						*/
/* version	: v109							*/
/* authors	: Laurent VUILLEMIN					*/
/* description	: high level function					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include "bhl_lib.h"

/* ###--------------------------------------------------------------### */
/* function     : beh_maknode						*/
/* description  : create node for each signal register or bus		*/ 
/* called func. : beh_makquad beh_makbdd beh_makderiv beh_makgex	*/
/* ###--------------------------------------------------------------### */

void beh_maknode (pt_befig, node_flag, trace)

struct befig *pt_befig ;
char          node_flag;
char          trace    ;

  {
	/* ###------------------------------------------------------### */
	/*   setting missing flags :					*/
	/*     - derivatives need quads, Bdd and list of variables	*/
	/*     - if two flags are set, quads are needed			*/
	/* ###------------------------------------------------------### */

  if ((node_flag & BEH_NODE_DERIVATE) != 0)
    node_flag |= BEH_NODE_BDD | BEH_NODE_VARLIST | BEH_NODE_QUAD;

  if ((node_flag & BEH_NODE_VARLIST) != 0)
    {
    if (((node_flag & BEH_NODE_BDD)   || (node_flag & BEH_NODE_GEX) ||
         (node_flag & BEH_NODE_USER)) != 0)
      {
      node_flag |= BEH_NODE_QUAD ;
      }
    }
  else
    {
    if ((node_flag & BEH_NODE_USER) != 0)
      {
      if (((node_flag & BEH_NODE_BDD) || (node_flag & BEH_NODE_GEX)) != 0)
        {
        node_flag |= BEH_NODE_QUAD ;
        }
      else
        {
        if (((node_flag & BEH_NODE_BDD) && (node_flag & BEH_NODE_GEX)) != 0)
          {
          node_flag |= BEH_NODE_QUAD ;
          }
        }
      }
    }

	/* ###------------------------------------------------------### */
	/*    call the appropriate function two build the requested	*/
	/* structures							*/
	/* ###------------------------------------------------------### */

	/* ###------------------------------------------------------### */
	/*   build quads to support further structures			*/
	/* ###------------------------------------------------------### */

  if ((node_flag & BEH_NODE_QUAD) != 0)
    {
    if (trace != 0)
      beh_message (14, "QUAD");
    beh_makquad (pt_befig);
    }

	/* ###------------------------------------------------------### */
	/*   build general expressions					*/
	/* ###------------------------------------------------------### */

  if ((node_flag & BEH_NODE_GEX) != 0)
    {
    if (trace != 0)
      beh_message (14, "GEX");
    beh_makgex (pt_befig);
    }

	/* ###------------------------------------------------------### */
	/*   build binary decision diagrams				*/
	/* ###------------------------------------------------------### */

  if ((node_flag & BEH_NODE_BDD) != 0)
    {
    if (trace != 0)
      beh_message (14, "BDD");
    beh_makbdd (pt_befig);
    }

	/* ###------------------------------------------------------### */
	/*   build list of variables					*/
	/* ###------------------------------------------------------### */

  if ((node_flag & BEH_NODE_VARLIST) != 0)
    {
    if (trace != 0)
      beh_message (14, "list of variables");
    beh_makvarlist (pt_befig);
    }

	/* ###------------------------------------------------------### */
	/*   build derivatives of binary expressions (list of Bdds)	*/
	/* ###------------------------------------------------------### */

  if ((node_flag & BEH_NODE_DERIVATE) != 0)
    {
    if (trace != 0)
      beh_message (14, "derivatives");
    beh_makderiv (pt_befig);
    }

  }
