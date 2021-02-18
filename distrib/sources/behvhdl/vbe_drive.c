
/* ###--------------------------------------------------------------### */
/* file		: vbe_drive.c						                              */
/* date		: Nov  2 1995						                              */
/* version	: v111							                                 */
/* author	: VUONG H.N.						                              */
/* description	: This file contains VHDL drivers :			               */
/*		  vhdlsavebefig()					                                    */
/* ###--------------------------------------------------------------### */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include AVT_H
#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BVL_H
#include "bvl_utype.h"
#include "bvl_utdef.h"
#include "bvl_util.h"

static char *
gettimeunit (unsigned char time_unit)
{
  switch (time_unit)
  {
    case BEH_TU__FS:
      return(namealloc("fs"));
    case BEH_TU__PS:
      return(namealloc("ps"));
    case BEH_TU__NS:
      return(namealloc("ns"));
    case BEH_TU__US:
      return(namealloc("us"));
    case BEH_TU__MS:
      return(namealloc("ms"));
    default:
      beh_error (200, NULL);
      return(namealloc("ps"));
  }
}

/* ###--------------------------------------------------------------### */
/* function	: vbesavebefig						*/
/* description	: print out a text file containing a data-flow VHDL	*/
/*		  description						*/
/* called func.	: bvl_vhdlname, mbkalloc, beh_message, beh_error  ,	*/
/*		  beh_toolbug , getptype, reverse    , bvl_abl2str	*/
/*									*/
/* ###--------------------------------------------------------------### */

void 
vbesavebefig (befig_list *ptbefig, unsigned int trace_mode)
{
  char         *suffix;
  char         *mode;
  char         *type_mark;
  int           nrlabel = 0;
  int           buff_size = 100;
  char         *buffer;
  FILE         *fd;
  time_t        clock;
  int           left,right;
  char         *name;
  char         *bus;
  char         *time_unit;

  struct begen *ptgeneric = NULL;	/* current ptype pnt (generic)	*/
  struct bereg *ptbereg   = NULL;	/* current BEREG pointer	*/
  struct bemsg *ptbemsg   = NULL;	/* current BEMSG pointer	*/
  struct beout *ptbeout   = NULL;	/* current BEOUT pointer	*/
  struct bebus *ptbebus   = NULL;	/* current BEBUS pointer	*/
  struct beaux *ptbeaux   = NULL;	/* current BEAUX pointer	*/
  struct bebux *ptbebux   = NULL;	/* current BEBUX pointer	*/
  struct bepor *ptbepor   = NULL;	/* correctly ordered port list	*/
  struct biabl *ptbiabl   = NULL;	/* current BIABL pointer   	*/

  if (ptbefig == NULL)
    beh_toolbug (10,"vbe_decomp",NULL,0);

  buffer    = mbkalloc (buff_size);
  buffer[0] = '\0';

  suffix = V_STR_TAB[__BVL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = namealloc("vbe");

	/* ###------------------------------------------------------### */
	/*    Opening result file					*/
	/* ###------------------------------------------------------### */

  if ((fd = mbkfopen (ptbefig->NAME, suffix, WRITE_TEXT)) == NULL)
    {
    beh_error (107, NULL);
    EXIT (1);
    }

  if ((trace_mode & BVL_TRACE) != 0)
    beh_message (13, ptbefig->NAME);

  time (&clock);
  (void) fprintf (fd,"-- VHDL data flow description generated from `%s`\n",
                  ptbefig->NAME);
  (void) fprintf (fd, "--\t\tdate : %s\n\n", ctime(&clock));

	/* ###------------------------------------------------------### */
	/*    Entity declaration					*/
	/* ###------------------------------------------------------### */

  (void) fprintf (fd,"-- Entity Declaration\n\n");
  (void) fprintf (fd,"ENTITY %s IS\n",bvl_vhdlname(ptbefig->NAME));

	/* ###------------------------------------------------------### */
	/*    Generic declaration					*/
	/* ###------------------------------------------------------### */

  if ((ptgeneric = ptbefig->BEGEN) != NULL)
    {
    ptgeneric = (struct begen *) reverse ((chain_list *)ptgeneric);

    (void) fprintf (fd,"  GENERIC (\n");
    while (ptgeneric != NULL)
      {
      (void)fprintf (fd,"    CONSTANT %s : NATURAL := %ld",
                     bvl_vhdlname(ptgeneric->NAME),*((long *)ptgeneric->VALUE));
      if (ptgeneric->NEXT != NULL)
        (void)fprintf(fd,";\t-- %s\n",(char *)ptgeneric->NAME);
      else
        (void)fprintf(fd,"\t-- %s\n",(char *)ptgeneric->NAME);

      ptgeneric = ptgeneric->NEXT;
      }
    (void) fprintf (fd,"  );\n");
    }

	/* ###------------------------------------------------------### */
	/*    Port declaration						*/
	/* ###------------------------------------------------------### */

  ptbepor = ptbefig->BEPOR;
  if (ptbepor != NULL)
    {
    (void) fprintf (fd,"  PORT (\n");
    ptbefig->BEPOR = (struct bepor *)reverse( (chain_list *)ptbefig->BEPOR);

    ptbepor = ptbefig->BEPOR;
    while (ptbepor != NULL)
      {
      switch (ptbepor->DIRECTION)
        {
        case 'I':
          mode = namealloc("IN"); 
          break;
        case 'O':
        case 'Z':
          mode = namealloc("OUT"); 
          break;
        case 'B':
        case 'T':
          mode = namealloc("INOUT"); 
          break;
        default :
          beh_error (69, ptbepor->NAME);
        }
      ptbepor = (bepor_list *) bvl_vectnam (ptbepor,&left,&right,&name,0);

      if(left != -1)
        {
        switch (ptbepor->TYPE)
          {
          case 'B':
            type_mark = namealloc("BIT_VECTOR"); 
            bus = "";
            break;
          case 'W':
            type_mark = namealloc("WOR_VECTOR"); 
            bus = namealloc("BUS");
            break;
          case 'M':
            type_mark = namealloc("MUX_VECTOR"); 
            bus = namealloc("BUS");
            break;
          default :
            beh_error (68, ptbepor->NAME);
          }
        (void)fprintf(fd,"  %s : %s %s(%d %s %d) %s",bvl_vhdlname(name), mode,
                      type_mark, left, (left>=right)?"DOWNTO":"TO",right,bus);
        }
      else
        {
        switch (ptbepor->TYPE)
          {
          case 'B':
            type_mark = "BIT"; break;
          case 'W':
            type_mark = "WOR_BIT BUS"; break;
          case 'M':
            type_mark = "MUX_BIT BUS"; break;
          default :
            beh_error (68, ptbepor->NAME);
          }

        (void) fprintf (fd,"  %s : %s %s",bvl_vhdlname(name),
                                          mode,type_mark);
        }

      if (ptbepor->NEXT != NULL)
        (void) fprintf (fd,";\t-- %s\n",name);
      else
        (void) fprintf (fd,"\t-- %s\n  );\n",name);

      ptbepor = ptbepor->NEXT;
      }

    ptbefig->BEPOR = (struct bepor *)reverse( (chain_list *)ptbefig->BEPOR);
    }

  (void) fprintf (fd,"END %s;\n\n\n",bvl_vhdlname(ptbefig->NAME));

	/* ###------------------------------------------------------### */
	/*    Architecture declaration					*/
	/* ###------------------------------------------------------### */

  (void) fprintf (fd,"-- Architecture Declaration\n\n");
  (void) fprintf (fd,"ARCHITECTURE behaviour_data_flow OF %s IS\n",
                  bvl_vhdlname(ptbefig->NAME));

	/* ###------------------------------------------------------### */
        /*  Treatment of the BEREG list					*/
	/* ###------------------------------------------------------### */

  ptbefig->BEREG = (struct bereg *)reverse( (chain_list *)ptbefig->BEREG);
  ptbereg = ptbefig->BEREG;
  while (ptbereg != NULL)
    {
    ptbereg = (bereg_list *)bvl_vectnam(ptbereg,&left,&right,&name,2);
    if(left != -1)
      {
      (void)fprintf(fd,"  SIGNAL %s : REG_VECTOR(%d %s %d) REGISTER;\t-- %s\n",
                    bvl_vhdlname(name),left,(left>=right)?"DOWNTO":"TO",
                    right,name);
      }
    else
      {
      (void) fprintf (fd,"  SIGNAL %s : REG_BIT REGISTER;\t-- %s\n",
                    bvl_vhdlname(name),name);
      }
    ptbereg = ptbereg->NEXT;
    }
  ptbefig->BEREG = (struct bereg *)reverse( (chain_list *)ptbefig->BEREG);

	/* ###------------------------------------------------------### */
        /*  Treatment of the BEBUX list					*/
	/* ###------------------------------------------------------### */

  ptbefig->BEBUX = (struct bebux *)reverse( (chain_list *)ptbefig->BEBUX);
  ptbebux = ptbefig->BEBUX;
  while (ptbebux != NULL)
    {
    ptbebux = (bebux_list *)bvl_vectnam(ptbebux,&left,&right,&name,1);
    if(left != -1)
      {
      switch (ptbebux->TYPE)
        {
        case 'W':
          type_mark = namealloc("WOR_VECTOR"); 
          break;
        case 'M':
          type_mark = namealloc("MUX_VECTOR"); 
          break;
        }
      (void)fprintf(fd,"  SIGNAL %s : %s(%d %s %d) BUS;\t-- %s\n",
                    bvl_vhdlname(name),type_mark,left,(left>=right)?"DOWNTO":"TO",
                    right,name);
      }
    else
      {
      switch (ptbebux->TYPE)
        {
        case 'W':
          type_mark = namealloc("WOR_BIT"); 
          break;
        case 'M':
          type_mark = namealloc("MUX_BIT"); 
          break;
        }
      (void) fprintf (fd,"  SIGNAL %s : %s BUS;\t\t-- %s\n",bvl_vhdlname(name),
                      type_mark,name);
      }
    ptbebux = ptbebux->NEXT;
    }
  ptbefig->BEBUX = (struct bebux *)reverse( (chain_list *)ptbefig->BEBUX);

  ptbefig->BEAUX = (struct beaux *)reverse( (chain_list *)ptbefig->BEAUX);
  ptbeaux = ptbefig->BEAUX;
  while (ptbeaux != NULL)
    {
    ptbeaux = (beaux_list *)bvl_vectnam(ptbeaux,&left,&right,&name,3);
    if(left != -1)
      {
      (void)fprintf(fd,"  SIGNAL %s : BIT_VECTOR(%d %s %d);\t-- %s\n",
                    bvl_vhdlname(name),left,(left>=right)?"DOWNTO":"TO",
                    right,name);
      }
    else
      {
      (void) fprintf (fd,"  SIGNAL %s : BIT;\t\t-- %s\n",
                    bvl_vhdlname(name),name);
      }
    ptbeaux = ptbeaux->NEXT;
    }
  ptbefig->BEAUX = (struct beaux *)reverse( (chain_list *)ptbefig->BEAUX);

  (void) fprintf (fd,"\nBEGIN\n");

  time_unit = gettimeunit(ptbefig->TIME_UNIT);

	/* ###------------------------------------------------------### */
	/*    Print out a concurrent assert statement for each BEMSG	*/
	/* ###------------------------------------------------------### */

  ptbemsg = ptbefig->BEMSG;
  while (ptbemsg != NULL)
    {
    if (ptbemsg->LABEL != NULL)
      (void)fprintf(fd,"  %s :", ptbemsg->LABEL);

    buffer = bvl_abl2str (ptbemsg->ABL,buffer,&buff_size); 
    (void) fprintf (fd,"  ASSERT (%s = '1')\n", bvl_printabl(buffer));
    buffer[0] = '\0';

    if (ptbemsg->MESSAGE != NULL)
      (void) fprintf (fd,"    REPORT %s\n",ptbemsg->MESSAGE);

    if (ptbemsg->LEVEL == 'W')
      (void) fprintf (fd,"    SEVERITY WARNING;");
    else
      (void) fprintf (fd,"    SEVERITY ERROR;");

    (void) fprintf (fd,"\n\n");
    ptbemsg = ptbemsg->NEXT;
    }

	/* ###------------------------------------------------------### */
	/*    Print out a concurrent signal assignment for each BEAUX	*/
	/* ###------------------------------------------------------### */

  ptbeaux = ptbefig->BEAUX;
  while (ptbeaux != NULL)
    {
    if (ptbeaux->ABL != NULL)
      {
      buffer = bvl_abl2str (ptbeaux->ABL,buffer,&buff_size);
      if (ptbeaux->TIME == 0)
        (void) fprintf (fd,"  %s <= %s;\n",bvl_vectorize(ptbeaux->NAME),bvl_printabl(buffer));
      else
        (void) fprintf (fd,"  %s <= %s after %u %s;\n",bvl_vectorize(ptbeaux->NAME),bvl_printabl(buffer), ptbeaux->TIME, time_unit);
      buffer[0] = '\0';
      }
    else
      beh_error (40, ptbeaux->NAME);
    ptbeaux = ptbeaux->NEXT;
    }

	/* ###------------------------------------------------------### */
	/*    Print out a block statement  with one guarded concurrent	*/
	/* signal assignment for each BIABL of each BEREG		*/
	/* ###------------------------------------------------------### */

  ptbereg = ptbefig->BEREG;
  while (ptbereg != NULL)
    {
    ptbiabl = ptbereg->BIABL;
    while (ptbiabl != NULL)
      {
      (void) fprintf (fd,"  label%d : BLOCK ",nrlabel);
      if (ptbiabl->CNDABL  != NULL)
        {
        buffer = bvl_abl2str (ptbiabl->CNDABL,buffer,&buff_size);
        (void) fprintf (fd,"(%s = '1')\n",bvl_printabl(buffer));
        buffer[0] = '\0';
        }
      else
        beh_toolbug (19,"vbe_decomp",ptbereg->NAME,0);
  
      (void) fprintf (fd,"  BEGIN\n    %s <= GUARDED ",bvl_vectorize(ptbereg->NAME));
      if (ptbiabl->VALABL  != NULL)
        {
        buffer = bvl_abl2str (ptbiabl->VALABL,buffer,&buff_size);
        if (ptbiabl->TIME == 0)
          (void) fprintf (fd,"%s;\n",bvl_printabl(buffer));
        else
          (void) fprintf (fd,"%s after %u %s;\n",bvl_printabl(buffer), ptbiabl->TIME, time_unit);
        buffer[0] = '\0';
        }
      else
        beh_toolbug (20,"vbe_decomp",ptbereg->NAME,0);

      (void) fprintf ( fd,"  END BLOCK label%d;\n",nrlabel);
      ptbiabl = ptbiabl->NEXT;
      nrlabel++;
      }
  
    ptbereg = ptbereg->NEXT;
    }

	/* ###------------------------------------------------------### */
	/*    Print out a block statement  with one guarded concurrent	*/
	/* signal assignment for each BIABL of each BEBUX		*/
	/* ###------------------------------------------------------### */

  ptbebux = ptbefig->BEBUX;
  while (ptbebux != NULL)
    {
    ptbiabl = ptbebux->BIABL;
    while (ptbiabl != NULL)
      {
      (void) fprintf (fd,"  label%d : BLOCK (",nrlabel);
      if (ptbiabl->CNDABL != NULL)
        {
        buffer = bvl_abl2str (ptbiabl->CNDABL,buffer,&buff_size);
        (void) fprintf (fd,"%s = '1')\n",bvl_printabl(buffer));
        buffer[0] = '\0';
        }
      else
        beh_toolbug (19,"vbe_decomp",ptbebux->NAME,0);
  
      (void) fprintf (fd,"  BEGIN\n    %s <= GUARDED ",
                      bvl_vectorize(ptbebux->NAME));
      if (ptbiabl->VALABL != NULL)
        {
        buffer = bvl_abl2str (ptbiabl->VALABL,buffer,&buff_size);
        if (ptbiabl->TIME == 0)
          (void) fprintf (fd,"%s;\n",bvl_printabl(buffer));
        else
          (void) fprintf (fd,"%s after %u %s;\n",bvl_printabl(buffer), ptbiabl->TIME, time_unit);
        buffer[0] = '\0';
        }
      else
        beh_toolbug (20,"vbe_decomp",ptbebux->NAME,0);

      (void) fprintf (fd,"  END BLOCK label%d;\n",nrlabel);
      ptbiabl = ptbiabl->NEXT;
      nrlabel++;
      }
  
    ptbebux = ptbebux->NEXT;
    }

	/* ###------------------------------------------------------### */
	/*    Print out a block statement  with one guarded concurrent	*/
	/* signal assignment for each BIABL of each BEBUS		*/
	/* ###------------------------------------------------------### */

  ptbebus = ptbefig->BEBUS;
  while (ptbebus != NULL)
    {
    ptbiabl = ptbebus->BIABL;
    while (ptbiabl != NULL)
      {
      (void) fprintf (fd,"\tlabel%d : BLOCK (",nrlabel);
      if (ptbiabl->CNDABL != NULL)
        {
        buffer = bvl_abl2str (ptbiabl->CNDABL,buffer,&buff_size);
        (void) fprintf (fd,"%s = '1')\n",bvl_printabl(buffer));
        buffer[0] = '\0';
        }
      else
        beh_toolbug (19,"vbe_decomp",ptbebus->NAME,0);
  
      (void) fprintf (fd,"\tBEGIN\n\t%s <= GUARDED ",bvl_vectorize(ptbebus->NAME));
      if (ptbiabl->VALABL != NULL)
        {
        buffer = bvl_abl2str (ptbiabl->VALABL,buffer,&buff_size);
        if (ptbiabl->TIME == 0)
          (void) fprintf (fd,"%s;\n",bvl_printabl(buffer));
        else
          (void) fprintf (fd,"%s after %u %s;\n",bvl_printabl(buffer), ptbiabl->TIME, time_unit);
        buffer[0] = '\0';
        }
      else
        beh_toolbug (20,"vbe_decomp",ptbebus->NAME,0);

      (void) fprintf (fd,"\tEND BLOCK label%d;\n",nrlabel);
      ptbiabl = ptbiabl->NEXT;
      nrlabel++;
      }
  
    ptbebus = ptbebus->NEXT;
    }

	/* ###------------------------------------------------------### */
	/*    Print out a concurrent signal assignment for each BEOUT	*/
	/* ###------------------------------------------------------### */

  ptbeout = ptbefig->BEOUT;
  while (ptbeout != NULL)
    {
    if (ptbeout->ABL != NULL)
      {
      buffer = bvl_abl2str(ptbeout->ABL,buffer,&buff_size);
      if (ptbeout->TIME == 0)
        (void) fprintf (fd,"\n%s <= %s;\n",bvl_vectorize(ptbeout->NAME),bvl_printabl(buffer));
      else
        (void) fprintf (fd,"\n%s <= %s after %u %s;\n",bvl_vectorize(ptbeout->NAME),bvl_printabl(buffer), ptbeout->TIME, time_unit);
      buffer[0] = '\0';
      }
    else
      beh_error (40, ptbeout->NAME);

    ptbeout = ptbeout->NEXT;
    }
  
  (void) fprintf (fd,"END;\n");
  (void) fclose  (fd);
}
