/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
/* ###--------------------------------------------------------------### */
/*									*/
/* file		: mvl_drive.c						*/
/* date		: Sep 24 1993						*/
/* author	: VUONG Huu Nghia					*/
/* description	: This file contains a MBK-->VHDL driver :		*/
/* functions    : vhdlsavelofig()					*/
/*									*/
/* ###--------------------------------------------------------------### */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include AVT_H
#include MUT_H
#include MLO_H
#include "mvl_utype.h"
#include "mvl_utdef.h"
#include "mvl_drive.h"

char *VST_OUT = NULL;

/* ###--------------------------------------------------------------### */
/* function	: vhdlsavelofig						*/
/* description	: print out a texte file containing a structural VHDL	*/
/*		  description						*/
/* called func.	: mvl_vhdlname   , getsigname, mvl_message, mvl_error  ,*/
/*		  mvl_toolbug, getptype, reverse, mvl_abl2str		*/
/*									*/
/* ###--------------------------------------------------------------### */
void vhdlsavelofig (lofig_list *ptfig, FILE *outputfile)
  {
  extern char  *getsigname ();
  struct loins *ptins        = NULL;
  struct locon *ptcon        = NULL;
  struct locon *tmp_ptcon    = NULL;
  struct locon *ptscan       = NULL;
  struct losig *ptsig        = NULL;
  struct chain *ptmodel      = NULL;
  struct chain *ptchain      = NULL;
  struct chain *sig_list     = NULL;
  FILE	       *ptfile       ;
  ht           *rangeht      ;
  time_t        clock        ;
  char 	       *mode         = NULL;
  char         *name         ;
  char         *sig_name     ;
  char         *root_name    ;
  char          typestr[20]  ;
  char         *str          ;
  int           left         ;
  int           right        ;
  int           i            ;
  int           cur_index    ;
  int           match        ;
  char          new_name[200];
  char         *suffix;

  suffix = V_STR_TAB[__MVL_FILE_SUFFIX].VALUE;
  if (suffix == NULL) suffix = OUT_LO;

  if (ptfig == NULL)
    mvl_toolbug (10, "mvl_decomp", NULL, 0);
  
  if (VST_OUT == NULL) 
    {
    VST_OUT = getenv("VST_OUT_FORMAT");
    }

  rangeht = addht(100);

	/* ###------------------------------------------------------### */
	/*    Opening result file					*/
	/* ###------------------------------------------------------### */

  if (VST_OUT != NULL && strcmp(VST_OUT, "vst") == 0)
    strcpy(typestr, "BIT");
  else
    strcpy(typestr, "STD_LOGIC");

  if (outputfile!=NULL) ptfile=outputfile;
  else 
  {
    if ((ptfile = mbkfopen (ptfig->NAME, suffix, "w")) == NULL)
      {
      mvl_error (107, NULL);
      EXIT (1);
      }
  }

  time (&clock);
  fprintf (ptfile,"-- VHDL structural description generated from `%s`\n",
           ptfig->NAME);
  fprintf (ptfile,"--\t\tdate : %s\n",ctime(&clock));
  if (VST_OUT == NULL || (VST_OUT != NULL && strcmp(VST_OUT, "vst") != 0))
    {
    fprintf(ptfile, "library IEEE;\nuse IEEE.std_logic_1164.all;\n\n");
    }

	/* ###------------------------------------------------------### */
	/*    Entity declaration					*/
	/* ###------------------------------------------------------### */

  fprintf (ptfile,"-- Entity Declaration\n\n");
  fprintf (ptfile,"ENTITY %s IS\n", mvl_vhdlname (ptfig->NAME));

	/* ###------------------------------------------------------### */
	/*    Port declaration						*/
	/* ###------------------------------------------------------### */

  if (ptfig->LOCON != NULL)
    {
    fprintf (ptfile,"  PORT (\n");
    ptfig->LOCON = (struct locon *) reverse ((struct chain *)ptfig->LOCON);
    ptcon        = ptfig->LOCON;
    while (ptcon != NULL)
      {
      switch (ptcon->DIRECTION)
        {
        case 'O':
        case 'Z':
          mode = namealloc ("out");
          break;
        case 'B':
        case 'T':
          mode = namealloc ("inout");
          break;
        case 'X':
          mode = namealloc ("linkage");
          break;
        case 'I':
        default : // ..anto..
          mode = namealloc ("in");
          break;
        }
      ptcon = (struct locon *) mvl_vectnam (ptcon,&left,&right,&name,1);
      if (left != -1)
        {
        fprintf (ptfile,"  %s : %s %s_VECTOR (%d %s %d)",
                      mvl_vhdlname (name), mode, typestr, left,
                      (left >= right)? "DOWNTO":"TO",right);
        addhtitem(rangeht, mvl_vhdlname(name), abs(left-right));
        }
      else
        {
        fprintf (ptfile,"  %s : %s %s",mvl_vhdlname(name),mode, typestr);
        }

      if (ptcon->NEXT != NULL)
        fprintf (ptfile,";\t-- %s\n",name);
      else
        fprintf (ptfile,"\t-- %s\n",name);

      ptcon = ptcon->NEXT;
      mbkfree (name);
      }
    fprintf (ptfile, "  );\n");
    }
  fprintf (ptfile,"END %s;\n\n",mvl_vhdlname(ptfig->NAME));
  ptfig->LOCON = (struct locon *) reverse ((struct chain *)ptfig->LOCON);

	/* ###------------------------------------------------------### */
	/*    Architecture declaration					*/
	/* ###------------------------------------------------------### */

  fprintf (ptfile,"-- Architecture Declaration\n\n");
  fprintf (ptfile,"ARCHITECTURE STRUCTURAL OF %s IS\n",
                  mvl_vhdlname(ptfig->NAME));

	/* ###------------------------------------------------------### */
	/*    Component declaration : first make a list of models, then	*/
	/* for each model print out a "component declaration"		*/
	/* ###------------------------------------------------------### */

  ptmodel = NULL;
  for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT)
    {
    for (ptchain = ptmodel ; ptchain ; ptchain = ptchain->NEXT)
      {
      if (((struct loins *)ptchain->DATA)->FIGNAME == ptins->FIGNAME)
        break;
      }
    if (ptchain == NULL)
      ptmodel = addchain(ptmodel,ptins);
    }

  for (ptchain = ptmodel ; ptchain ; ptchain = ptchain->NEXT)
    {
    ptins = (struct loins *)ptchain->DATA;
    fprintf (ptfile,"  COMPONENT %s\n    PORT (\n",
                    mvl_vhdlname(ptins->FIGNAME));

    ptins->LOCON = (struct locon *) reverse ((struct chain *)ptins->LOCON);
    ptcon = ptins->LOCON;
    while (ptcon != NULL)
      {
      switch (ptcon->DIRECTION)
        {
        case 'O':
        case 'Z':
          mode = namealloc ("out");
          break;
        case 'B':
        case 'T':
          mode = namealloc ("inout");
          break;
        case 'X':
          mode = namealloc ("linkage");
          break;
        case 'I':
        default : // ..anto..
          mode = namealloc ("in");
          break;
        }
      ptcon = (struct locon *) mvl_vectnam (ptcon,&left,&right,&name,1);
      if (left != -1)
        {
        fprintf (ptfile,"    %s : %s %s_VECTOR(%d %s %d)",
                     mvl_vhdlname(name), mode, typestr, left,
                     (left >= right)?"DOWNTO":"TO",right);
        }
      else
        fprintf(ptfile,"    %s : %s %s", mvl_vhdlname(ptcon->NAME),mode, typestr);

      if (ptcon->NEXT != NULL)
         fprintf(ptfile, ";\n");
      else
         fprintf(ptfile, "\n");

      ptcon = ptcon->NEXT;
      mbkfree(name);
      }
    ptins->LOCON = (struct locon *) reverse ((chain_list *)ptins->LOCON);
    fprintf (ptfile, "    );\n  END COMPONENT;\n\n");
    }

  freechain (ptmodel);

	/* ###------------------------------------------------------### */
	/*    Signal declaration					*/
	/* ###------------------------------------------------------### */

    ptsig = ptfig->LOSIG;
    while (ptsig != NULL)
      {
      if (ptsig->TYPE == 'I')
        {
        ptsig = (struct losig *) mvl_vectnam (ptsig,&left,&right,&name,0);
        if (left != -1)
          {
          fprintf (ptfile,"  SIGNAL %s : %s_VECTOR(%d %s %d);\n",
                       mvl_vhdlname(name), typestr, left,
                       (left >= right)?"DOWNTO":"TO",right);
          addhtitem(rangeht, mvl_vhdlname(name), abs(left-right));
          }
        else
          {
          fprintf (ptfile,"  SIGNAL %s : %s;\n",
                   mvl_vhdlname(getsigname(ptsig)),typestr);
          }
        mbkfree(name);
        }
      ptsig = ptsig->NEXT;       
      }

	/* ###------------------------------------------------------### */
	/*    Description block						*/
	/* ###------------------------------------------------------### */

  fprintf (ptfile,"\nBEGIN\n\n");
  ptfig->LOINS = (struct loins *) reverse ((struct chain *)ptfig->LOINS);

  for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT)
    {
	/* ###------------------------------------------------------### */
        /*   Instantiation of a model					*/
	/* ###------------------------------------------------------### */

    ptins->LOCON = (struct locon *) reverse ((struct chain *)ptins->LOCON);
    fprintf (ptfile,"  %s : %s\n    PORT MAP (\n",
             mvl_vhdlname(ptins->INSNAME),mvl_vhdlname(ptins->FIGNAME));
    ptcon = ptins->LOCON; 
    while (ptcon != NULL)
      {
      tmp_ptcon = (struct locon *) mvl_vectnam (ptcon,&left,&right,&name,1);
      tmp_ptcon = ptcon;

      if (left != -1)
        {
	/* ###------------------------------------------------------### */
        /*    The connected signals are bused				*/
	/* ###------------------------------------------------------### */

        root_name = NULL;
        match = 1;
        sig_list = NULL;
        for (i=abs(left-right) ; i>=0 ; i--)
          {
          if (left <= right) cur_index = right - i;
          else cur_index = right + i;
          ptsig = ptcon->SIG;
          if (ptsig->TYPE == 'I')
            {
	    /* ###-------------------------------------------------### */
            /*    The signal is internal			       */
	    /* ###-------------------------------------------------### */

            if (root_name == NULL) root_name = vectorradical(getsigname(ptsig));
            else if (root_name != vectorradical(getsigname(ptsig))) match = 0;
            if (vectorindex(getsigname(ptsig)) != cur_index) match = 0;

            mvl_name(getsigname(ptsig), new_name);
            sig_name = namealloc (new_name);
            sig_list = addchain  (sig_list, sig_name);
            }
          else
            {
	    /* ###-------------------------------------------------### */
            /*    The signal is external                               */
	    /* ###-------------------------------------------------### */

            for (ptscan = ptfig->LOCON ; ptscan ; ptscan = ptscan->NEXT)
	          {
	          if (ptscan->SIG == ptsig) break;
	          }
            if (ptscan == NULL)
              {
	          printf ("\n*** mbk error *** no external connector  \n");
	          printf ("     driving vhdl file %s\n", ptfig->NAME);
	          }
            else
              {
              if (root_name == NULL) root_name = vectorradical(ptscan->NAME);
              else if (root_name != vectorradical(ptscan->NAME)) match = 0;
              if (vectorindex(ptscan->NAME) != cur_index) match = 0;

              mvl_name (ptscan->NAME,new_name);
              sig_name = namealloc (new_name);
              sig_list = addchain  (sig_list, sig_name);
              }
            }
          if (i > 0) 
          {
            ptcon = ptcon->NEXT;
            if (ptcon==NULL)
            {
                match=0;
                break;
            }
          }
          }

        if (match && gethtitem(rangeht,mvl_vhdlname(root_name)) == abs(left-right))
          {
          fprintf (ptfile,"    %s => %s",mvl_vhdlname(name), mvl_vhdlname(root_name));
          }
        else
          {
          ptcon = tmp_ptcon;
          sig_list = reverse(sig_list);
          while (sig_list != NULL)
            {
            mvl_name(ptcon->NAME, new_name);
            fprintf (ptfile,"    %s => %s",new_name, (char *)sig_list->DATA);
            sig_list = sig_list->NEXT;
            if (sig_list != NULL)
              {
              fprintf (ptfile,",\n");
              ptcon = ptcon->NEXT;
              }
            }
          }
        freechain(sig_list); //zinaps: 22/4/2003
        }
      else
        {
	/* ###------------------------------------------------------### */
        /*    The connected signals is simple				*/
	/* ###------------------------------------------------------### */

        ptsig = ptcon->SIG;
        if (ptsig->TYPE == 'I')
          {
          mvl_name (getsigname(ptsig), new_name);
          fprintf (ptfile, "    %s => %s", mvl_vhdlname(name), new_name);
          }
        else
          {
          for (ptscan = ptfig->LOCON ; ptscan ; ptscan = ptscan->NEXT)
	    {
	    if (ptscan->SIG == ptsig) 
              break;
	    }
          if (ptscan == NULL)
            {
	        printf ("\n*** mbk error *** no external connector  \n");
	        printf ("     driving vhdl file %s\n", ptfig->NAME);
	        }
          else
            {
	        mvl_name (ptscan->NAME,new_name);
	        fprintf(ptfile, "    %s => %s", mvl_vhdlname(name), new_name);
	        }
          }
        }
      if (ptcon->NEXT != NULL)
        fprintf (ptfile, ",\n");
      ptcon = ptcon->NEXT;
      mbkfree(name);
      }
    fprintf(ptfile, "\n    );\n");
    ptins->LOCON = (struct locon *) reverse ((struct chain *)ptins->LOCON);
    }
  ptfig->LOINS = (struct loins *) reverse ((chain_list *)ptfig->LOINS);
  fprintf (ptfile, "\nEND STRUCTURAL;\n");

  if (V_BOOL_TAB[__MVL_CONFIGURE].VALUE)
    {
    fprintf (ptfile, "\nCONFIGURATION cfg_%s OF %s IS\n", ptfig->NAME, ptfig->NAME);
    fprintf (ptfile, "  FOR STRUCTURAL\n");
    fprintf (ptfile, "  END FOR;\n");
    fprintf (ptfile, "END cfg_%s;\n", ptfig->NAME);
    }
  if (outputfile==NULL) fclose  (ptfile);
  delht(rangeht);
  }
