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

/* 
 * Purpose : verilog driver, mainly used to enable simulating with Cadence Verilog.
 * Version : 002
 * Date    : Jul 1995
 * Author  : Aldo Mozzi <aldo@protec.it>
 * Modified by Mourad Aberbour in Sept. 1997 
 */

#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include AVT_H
#include MUT_H
#include MLO_H

static char     name_buffer[255];
static char     vec_name_buffer[255];

/******************************************************************
* function vlg_name: gets a name and returns a legal verilog name *
* in name_buffer. Characteres not allowed in a valid verilog name *
* are replaced by "_"<ascii code in hex>"_".                      *
* If the name is indexed the indexing syntax is translated:       *
*     <name><space><number> --> <name>[<number>]                  *
******************************************************************/

static int
isVerilogKeyword(char *str)
{
  static char *keywords[] = { "and", "nand", "or", "nor", "xor", "xnor", 
    "buf", "bufif0", "bufif1", "not", "notif0", "notif1",
    "pulldown", "pullup", "nmos", "rnmos", "pmos", "rpmos", "cmos", "rcmos",
    "tran", "rtran", "tranif0", "rtranif0", "tranif1", "rtranif1"};
  int i;
  
  for (i = sizeof(keywords)/sizeof(char*)-1; i >= 0; i-- )
    if (!strcmp(keywords[i],str))
      return 1;

  return 0;
}

char *
vlg_name(name)
    char           *name;
{
    char           *scan_new;
    int             i, indexed;
    int             badname = 0;
    char           *s;
    char            c;

    scan_new = name_buffer;
    scan_new[0] = '\0';
    if (V_BOOL_TAB[__VERILOG_KEEP_NAMES].VALUE) {
        i = 0;
        while ((c = name[i++]) != '\0') {
            if (c == ' ') break;
            if (!(isalpha((int)c) || c == '_' || ((i != 1) && (isdigit((int)c) || c == '$')))) {
                badname = 1;
                break;
            }
        }
        if (isVerilogKeyword(name))
          badname = 1;
        if (badname) {
            strcpy(scan_new, "\\");
            strcat(scan_new, vectorradical(name));
            strcat(scan_new, " ");
        }
        else {
            strcpy(scan_new, vectorradical(name));
        }

        if ((s = strchr(name, ' ')) != NULL) {
            strcat(scan_new, "[");
            strcat(scan_new, s+1);
            strcat(scan_new, "]");
        }
    }
    else {
      if (sscanf(name,"%d",&i) == 1)
        sprintf(name_buffer,"noname%d",i);
      else
      {
        i = 0;
        indexed = 0;
        while (*name) {
            if (i != 0 && *name == ' ') {
                *scan_new++ = '[';
                indexed = 1;
                name++;
            }
            else {
                if ( isalpha((int) *name) || *name == '_'
                    || ((i != 0 && isdigit((int) *name)) || *name == '$')) {
                    *scan_new = *name++;
                    if (isupper((int) *scan_new)) {
                        *scan_new = tolower((int) *scan_new);
                    }
                    scan_new++;
                }
                else {
                    *scan_new++ = '_';
                    name++;
                }
            }
            ++i;
        }
        if (indexed != 0) *scan_new++ = ']';
        *scan_new = '\0';
      }
    }
    return (name_buffer);
}


/***************************************************************************
* function vlg_busname: gets a pointer to a list of connectors and returns *
* the next connector name in *name (that points to vec_buffer), the left   *
* and right indices of the vector (-1 if signal is a scalar) and the       *
* pointer to the next vector as the return value. Performs vectorization   *
* of locon names, that are always scalar.                                  *
***************************************************************************/
locon_list     *
vlg_busname(pt_list, left, right, name)
    locon_list     *pt_list;
    int            *left, *right;
    char          **name;
{
    char           *blank_space;
    locon_list     *ptcon;
    char            END = 0;

    ptcon = pt_list;
    *left = *right = -1;
    strcpy(vec_name_buffer, ptcon->NAME);
    blank_space = strchr(vec_name_buffer, ' ');
    if (blank_space != NULL) {
        *right = atoi(blank_space);
        *left = *right;
        *blank_space = '\0';

        while (END != 1) {
            if (ptcon->NEXT != NULL) {
                blank_space = strchr(ptcon->NEXT->NAME, ' ');
                if (blank_space != NULL) {
                    *blank_space = '\0';
                    if (!strcmp(vec_name_buffer, ptcon->NEXT->NAME)) {
                        *right = atoi(blank_space + 1);
                        ptcon = ptcon->NEXT;
                    }
                    else END = 1;
                    *blank_space = ' ';
                }
                else END = 1;
            }
            else END = 1;
        }
    }
    *name = vec_name_buffer;

    return (ptcon->NEXT);
}

losig_list     *
vlg_intbusname(pt_list, left, right, name)
    losig_list     *pt_list;
    int            *left, *right;
    char          **name;
{
    char           *blank_space;
    losig_list     *ptsig;
    char            END = 0;

    ptsig = pt_list;
    *left = *right = -1;
    strcpy(vec_name_buffer, getsigname(ptsig));
    blank_space = strchr(vec_name_buffer, ' ');
    if (blank_space != NULL) {
        *right = atoi(blank_space);
        *left = *right;
        *blank_space = '\0';

        while (END != 1) {
            if (ptsig->NEXT != NULL) {
                blank_space = strchr(getsigname(ptsig->NEXT), ' ');
                if (blank_space != NULL) {
                    *blank_space = '\0';
                    if (!strcmp(vec_name_buffer, getsigname(ptsig->NEXT))) {
                        *right = atoi(blank_space + 1);
                        ptsig = ptsig->NEXT;
                    }
                    else END = 1;
                    *blank_space = ' ';
                }
                else END = 1;
            }
            else END = 1;
        }
    }
    *name = vec_name_buffer;

    return (ptsig->NEXT);
}


/*******************************************************************************
* function vlogsavelofig                                                       *
*******************************************************************************/

void
vlogsavelofig(lofig_list *ptfig, FILE *outputfile)
{
    loins_list     *ptins = NULL;
    locon_list     *ptcon = NULL;
    locon_list     *tmp_ptcon = NULL;
    locon_list     *ptscan = NULL;
    losig_list     *ptsig = NULL;
    losig_list     *ptnextsig = NULL;
    lotrs_list     *pttrs;
    FILE           *ptfile;
    int             flag, left, right, i, first;
    char           *name;
    char           *sig_name;
    char            direc_flag;
    chain_list     *sigchain = NULL;
    chain_list     *namechain = NULL;
    chain_list     *ptchain;
    char           *suffix;
    int             index=0, increment;
    char            tmp_name[20];
    char            buffer[2048];

    suffix = V_STR_TAB[__MGL_FILE_SUFFIX].VALUE;
    if (suffix == NULL) suffix = namealloc("v");

    if (outputfile!=NULL) ptfile = outputfile;
    else {
        if ((ptfile = mbkfopen(ptfig->NAME, suffix, WRITE_TEXT)) == NULL) {
            avt_errmsg(MGL_ERRMSG, "010", AVT_ERROR, ptfig->NAME);
            return;
        }
    }

    sprintf(buffer,"Verilog structural description generated from `%s`\n", ptfig->NAME);
    avt_printExecInfo(ptfile, "/*", buffer, "*/");
    fprintf(ptfile, "\n");
    fprintf(ptfile, "`timescale 1 ps/1 ps\n\n");


/*********************
* Module declaration *
*********************/
    fprintf(ptfile, "// Module Declaration.\n\n");
    fprintf(ptfile, "module %s (\n", vlg_name(ptfig->NAME));

    if (ptfig->LOCON != NULL) {
        ptfig->LOCON = (struct locon *) reverse((chain_list *) ptfig->LOCON);
        /*list inputs without indices first */
        ptcon = ptfig->LOCON;
        flag = 0;
        while (ptcon != NULL) {
            ptcon = vlg_busname(ptcon, &left, &right, &name);
            if (flag == 4) {
                fprintf(ptfile, ",\n");
                flag = 0;
            }
            if (flag != 0)
                fprintf(ptfile, ", ");
            fprintf(ptfile, "%s", vlg_name(name));
            ++flag;
        }
        fprintf(ptfile, ");\n\n\n");

        fprintf(ptfile, "// Declaration of external signals.\n\n");
        /*now list inputs and outputs with indices */
        ptcon = ptfig->LOCON;
        while (ptcon != NULL) {
            direc_flag = ptcon->DIRECTION;
            ptcon = vlg_busname(ptcon, &left, &right, &name);
            if (direc_flag == IN) {
                if (left != -1) {
                    fprintf(ptfile, "input [%d:%d] %s;\n", left, right, vlg_name(name));
                }
                else {
                    fprintf(ptfile, "input %s;\n", vlg_name(name));
                }
            }
            if (direc_flag == OUT || direc_flag == TRISTATE) {
                if (left != -1) {
                    fprintf(ptfile, "output [%d:%d] %s;\n", left, right, vlg_name(name));
                }
                else {
                    fprintf(ptfile, "output %s;\n", vlg_name(name));
                }
            }
            if (direc_flag == INOUT || direc_flag == TRANSCV || direc_flag == UNKNOWN) {
                if (left != -1) {
                    fprintf(ptfile, "inout [%d:%d] %s;\n", left, right, vlg_name(name));
                }
                else {
                    fprintf(ptfile, "inout %s;\n", vlg_name(name));
                }
            }
        }
    }
    fprintf(ptfile, "\n\n");

    ptfig->LOCON = (struct locon *) reverse((chain_list *) ptfig->LOCON);

/***********************************************************
* End of module declarations, now declare internal signals *
***********************************************************/

    fprintf(ptfile, "// Declaration of internal signals.\n\n");
    ptsig = ptfig->LOSIG;
    while (ptsig != NULL) {
        if ( ! (mbk_LosigIsGlobal(ptsig) && (mbk_LosigIsVDD(ptsig) || mbk_LosigIsVSS(ptsig)))
             && (ptsig->TYPE == 'I')) {
            ptnextsig = vlg_intbusname(ptsig, &left, &right, &name);
            if (left != -1) {
                fprintf(ptfile, "wire [%d:%d] %s;\n", left, right, vlg_name(name));
            }
            else fprintf(ptfile, "wire %s;\n", vlg_name(getsigname(ptsig)));
            ptsig = ptnextsig;
            continue;
        }
        else if (mbk_LosigIsVDD(ptsig) && (ptsig->TYPE == 'I')) {
            fprintf(ptfile, "supply1 %s;\n", vlg_name(getsigname(ptsig)));
        }
        else if (mbk_LosigIsVSS(ptsig) && (ptsig->TYPE == 'I')) {
            fprintf(ptfile, "supply0 %s;\n", vlg_name(getsigname(ptsig)));
        }
        ptsig = ptsig->NEXT;
    }

/*********************************************************
* End of signal declarations, now comes the real netlist *
*********************************************************/

    if (ptfig->LOTRS) {
        fprintf(ptfile, "\n\n// Declaration of transistors.\n\n");
        ptfig->LOTRS = (lotrs_list *)reverse((chain_list *) ptfig->LOTRS);
        for (pttrs = ptfig->LOTRS; pttrs; pttrs = pttrs->NEXT) {
            fprintf(ptfile, "%s ", mbk_istransp(getlotrsmodel(pttrs))?"tranif0":"tranif1");
            fprintf(ptfile, "%s (", vlg_name(pttrs->TRNAME));
            sigchain = addchain(sigchain, pttrs->GRID->SIG);
            sigchain = addchain(sigchain, pttrs->SOURCE->SIG);
            sigchain = addchain(sigchain, pttrs->DRAIN->SIG);
            for (ptchain = sigchain; ptchain; ptchain = ptchain->NEXT) {
                ptsig = (losig_list *)ptchain->DATA;
                if (ptsig->TYPE == 'I') {
                    fprintf(ptfile, "%s", vlg_name(getsigname(ptsig)));
                }
                else {
                    for (ptscan = ptfig->LOCON; ptscan; ptscan = ptscan->NEXT) {
                        if (ptscan->SIG == ptsig) break;
                    }
                    if (ptscan == NULL) avt_errmsg(MGL_ERRMSG, "009", AVT_ERROR, vlg_name(getsigname(ptsig)), ptfig->NAME);
                    else fprintf(ptfile, "%s", vlg_name(ptscan->NAME));
                }
                if (ptchain->NEXT) fprintf(ptfile, ", ");
            }
            freechain(sigchain);
            sigchain = NULL;
            fprintf(ptfile, ");\n");
        }
        ptfig->LOTRS = (lotrs_list *)reverse((chain_list *) ptfig->LOTRS);
    }

    fprintf(ptfile, "\n\n// Declaration of instances.\n\n");
    ptfig->LOINS = (loins_list *)reverse((chain_list *)ptfig->LOINS);
    for (ptins = ptfig->LOINS; ptins; ptins = ptins->NEXT) {
        ptins->LOCON = (locon_list *)reverse((chain_list *)ptins->LOCON);
        /* name the module name and the instance name */
        fprintf(ptfile, "%s ", vlg_name(ptins->FIGNAME));
        fprintf(ptfile, "%s (\n  ", vlg_name(ptins->INSNAME));
        ptcon = ptins->LOCON;
        while (ptcon != NULL) {
            tmp_ptcon = vlg_busname(ptcon, &left, &right, &name);
            fprintf(ptfile, ".%s(", vlg_name(name));
            if (left != -1) {
                /* The connected signals are bused */
                if (left < right) increment=1;
                else increment = -1;
                for (i = left; i != right+increment; i+=increment) {
                    if (vectorindex(ptcon->NAME)==i) {
                    ptsig = ptcon->SIG;
                    if (ptsig->TYPE == 'I') {
                        /* The signal is internal */
                        sig_name = namealloc(vlg_name(getsigname(ptsig)));
                        namechain = addchain(namechain, (char *)sig_name);
                    }
                    else {
                        /* The signal is external */
                        for (ptscan = ptfig->LOCON; ptscan; ptscan = ptscan->NEXT) {
                            if (ptscan->SIG == ptsig) break;
                        }
                        if (ptscan == NULL) avt_errmsg(MGL_ERRMSG, "009", AVT_ERROR, vlg_name(getsigname(ptsig)), ptfig->NAME);
                        else {
                            sig_name = namealloc(vlg_name(ptscan->NAME));
                            namechain = addchain(namechain, (char *)sig_name);
                        }
                    }
                    if (i != right)
                        ptcon = ptcon->NEXT;
                    }
                    else {
                        sprintf(tmp_name,"avt_noname_%d",index++);
                        namechain = addchain(namechain, (char *)tmp_name);
                    }
                }
                first = 1;
                namechain = reverse(namechain);
                for (ptchain = namechain; ptchain; ptchain = ptchain->NEXT) {
                    if (first != 1) {
                        fprintf(ptfile, ", %s", (char *)ptchain->DATA);
                    }
                    else {
                        fprintf(ptfile, "{%s", (char *)ptchain->DATA);
                        first = 0;
                    }
                }
                fprintf(ptfile, "}");
                freechain(namechain);
                namechain = NULL;
            }
            else {
                /* The connected signal is simple */
                ptsig = ptcon->SIG;
                if (ptsig->TYPE == 'I') {
                    fprintf(ptfile, "%s", vlg_name(getsigname(ptsig)));
                }
                else {
                    for (ptscan = ptfig->LOCON; ptscan; ptscan = ptscan->NEXT) {
                        if (ptscan->SIG == ptsig) break;
                    }
                    if (ptscan == NULL) avt_errmsg(MGL_ERRMSG, "009", AVT_ERROR, vlg_name(getsigname(ptsig)), ptfig->NAME);
                    else fprintf(ptfile, "%s", vlg_name(ptscan->NAME));
                }
            }
            if (ptcon->NEXT != NULL)
                fprintf(ptfile, "),\n  ");
            ptcon = ptcon->NEXT;
        }
        fprintf(ptfile, "));\n");
        ptins->LOCON = (locon_list *)reverse((chain_list *) ptins->LOCON);
    }
    ptfig->LOINS = (loins_list *)reverse((chain_list *) ptfig->LOINS);
    fprintf(ptfile, "endmodule\n");
    if (outputfile==NULL) fclose(ptfile);
}
