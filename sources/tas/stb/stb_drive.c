/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_drive.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#include STB_H
#include AVT_H

#include "stb_util.h"
#include "stb_error.h"
#include "stb_transfer.h"
#include "stb_drive.h"
#include "stb_directives.h"

/*****************************************************************************
* static function declarations                                               *
*****************************************************************************/

static void stb_writesig __P((FILE *ptfile, stbfig_list *ptstbfig, ttvsig_list *ptttvsig, int format));
static void stb_writeintervals __P((FILE *ptfile, stbpair_list *ptheadstbpair, int format));
static void stb_writeintervals_split(FILE *ptfile, stbfig_list *sb, stbnode *node, int hz, char phase);

/*****************************************************************************
*                           fonction stb_savestbfig()                        *
*****************************************************************************/



static void stb_driveconfig(FILE *ptfile, inffig_list *ifl)
{
  int i;

  fprintf(ptfile, "Conditions\nBegin\n");
  for (i=0; i<(signed)sto_cfg_size; i++)
    {
      if (sto_cfg[i].type=='b' && V_BOOL_TAB[sto_cfg[i].idx].SET)
        fprintf(ptfile, "  %s = \"%s\";\n", V_BOOL_TAB[sto_cfg[i].idx].VAR, V_BOOL_TAB[sto_cfg[i].idx].VALUE?"yes":"no");
      else if (sto_cfg[i].type=='s' && V_STR_TAB[sto_cfg[i].idx].SET && V_STR_TAB[sto_cfg[i].idx].VALUE!=NULL)
        fprintf(ptfile, "  %s = \"%s\";\n", V_STR_TAB[sto_cfg[i].idx].VAR, V_STR_TAB[sto_cfg[i].idx].VALUE);
      else if (sto_cfg[i].type=='i' && V_INT_TAB[sto_cfg[i].idx].SET)
        fprintf(ptfile, "  %s = \"%d\";\n", V_INT_TAB[sto_cfg[i].idx].VAR, V_INT_TAB[sto_cfg[i].idx].VALUE);
    }
  fprintf(ptfile, "  key_fp_bp = \"%x\";\n", stb_getfalsepathkey(ifl));
  fprintf(ptfile, "  key_mc = \"%x\";\n", stb_getmulticyclekey(ifl));
  fprintf(ptfile, "  key_d = \"%x\";\n", stb_getdirectivekey(ifl));
  fprintf(ptfile, "  key_dm = \"%x\";\n", stb_getdelaymarginkey(ifl));
  fprintf(ptfile, "  key_nc = \"%x\";\n", stb_getnocheckkey(ifl));
  fprintf(ptfile, "  key_nf_nr = \"%x\";\n", stb_getlistsectionkey(ifl, INF_NORISING)*3+stb_getlistsectionkey(ifl, INF_NOFALLING));
  fprintf(ptfile, "  key_fs = \"%x\";\n", stb_getfalseslackkey(ifl));
  fprintf(ptfile, "  key_proba = \"%x\";\n",stb_getswitchingprobakey(ifl));
  fprintf(ptfile, "  key_uncertainty = \"%x\";\n", stb_getclockuncertaintykey(ifl));

  fprintf(ptfile, "End;\n\n");
}

static void
stb_writesetuphold(FILE *ptfile, ttvsig_list *ptttvsig, char *signame)
{
   stbnode *nodeu, *noded;
   noded=stb_getstbnode(ptttvsig->NODE);
   nodeu=stb_getstbnode(ptttvsig->NODE+1);
   if (nodeu->SETUP!=STB_NO_TIME || nodeu->HOLD!=STB_NO_TIME
       || noded->SETUP!=STB_NO_TIME || noded->HOLD!=STB_NO_TIME)
   {
     fprintf(ptfile, "  \"%s\" :", signame); 
     fprintf(ptfile, " ( %s %s ) ", noded->SETUP==STB_NO_TIME?"-":stb_drvtime(noded->SETUP), noded->HOLD==STB_NO_TIME?"-":stb_drvtime(noded->HOLD)); 
     fprintf(ptfile, " ( %s %s );\n", nodeu->SETUP==STB_NO_TIME?"-":stb_drvtime(nodeu->SETUP), nodeu->HOLD==STB_NO_TIME?"-":stb_drvtime(nodeu->HOLD)); 
   }
}

static void
stb_writeflags(FILE *ptfile, ttvsig_list *ptttvsig, char *signame)
{
   stbnode *nodeu, *noded;
   noded=stb_getstbnode(ptttvsig->NODE);
   nodeu=stb_getstbnode(ptttvsig->NODE+1);
   if ((nodeu->FLAG & STB_NODE_STABCORRECT)!=0
       || (noded->FLAG & STB_NODE_STABCORRECT)!=0)
   {
     fprintf(ptfile, "  \"%s\" :", signame); 
     fprintf(ptfile, " \"%s\"", (noded->FLAG & STB_NODE_STABCORRECT)!=0?"S":""); 
     fprintf(ptfile, " \"%s\";\n", (nodeu->FLAG & STB_NODE_STABCORRECT)!=0?"S":""); 
   }
}

int
stb_savestbfig(ptstbfig, mode, format, suffix)
    stbfig_list    *ptstbfig;
    int             mode, format, suffix;
{
    FILE           *stbout;
    ttvsig_list    *ptttvsig;
    ttvevent_list  *ptevent;
    stbnode        *ptstbnode;
    stbck          *ptstbck;
    chain_list     *ptchain;
    ptype_list *pt;
    char            namebuf[1024];
    char            suffixbuf[4];
    char            state;
    int             conditioned = FALSE;
    int             internals = FALSE;
    int             memories = FALSE, invert, ideal, virt;
    stb_propagated_clock_to_clock *spctc;
    ttv_directive *sd;
    inffig_list *ifl;

    if (suffix == STB_SUFFIX_STO) {
        strcpy(suffixbuf, "sto");
    }
    else if (suffix == STB_SUFFIX_STB) {
        strcpy(suffixbuf, "stb");
    }
    else {
        stb_error(ERR_UNKNOWN_SUFFIX, NULL, 0, STB_FATAL);
    }

    if ((stbout = mbkfopen(ptstbfig->FIG->INFO->FIGNAME, suffixbuf, WRITE_TEXT)) == NULL) {
	    stb_error(ERR_CANNOT_OPEN, NULL, 0, STB_FATAL);
    }

    /* General Header */

    avt_printExecInfo(stbout, "#","","");

    fprintf(stbout, "NAME      \"%s\";\n", ptstbfig->FIG->INFO->FIGNAME);
    fprintf(stbout, "SETUPTIME %s;\n", stb_drvtime(ptstbfig->SETUP));
    fprintf(stbout, "HOLDTIME  %s;\n", stb_drvtime(ptstbfig->HOLD));
    fprintf(stbout, "\n");


    stb_driveconfig(stbout, getloadedinffig(ptstbfig->FIG->INFO->FIGNAME));

    /* Clock Connectors */

    if (ptstbfig->CLOCK != NULL) {
        fprintf(stbout, "CLOCK CONNECTORS\n");
        fprintf(stbout, "BEGIN\n");

        for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
            ptttvsig = (ttvsig_list *)ptchain->DATA;
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
            ptstbnode = stb_getstbnode(ptttvsig->NODE);
            if ((pt=getptype(ptttvsig->USER, STB_IS_CLOCK))!=NULL)
              spctc=(stb_propagated_clock_to_clock *)pt->DATA;
            else
              spctc=NULL;

            if (spctc!=NULL) ptstbck=&spctc->original_waveform;
            else ptstbck = ptstbnode->CK;
            
            if (getptype(ptttvsig->USER, STB_INVERTED_CLOCK)) invert=1;
            else invert=0;
            if (getptype(ptttvsig->USER, STB_IDEAL_CLOCK)) ideal=1;
            else ideal=0;
            if (getptype(ptttvsig->USER, STB_VIRTUAL_CLOCK)) virt=1;
            else virt=0;
                    
            fprintf(stbout, " %s%s%s \"%s\":\n", virt?"VIRTUAL ":"",ideal?"IDEAL ":"", invert?"! ":"", namebuf);
            fprintf(stbout, "    %s   (%s:%s);\n", !invert?"UP":"DOWN", stb_drvtime(ptstbck->SUPMIN), stb_drvtime(ptstbck->SUPMAX));
            fprintf(stbout, "    %s (%s:%s);\n", !invert?"DOWN":"UP", stb_drvtime(ptstbck->SDNMIN), stb_drvtime(ptstbck->SDNMAX));
            fprintf(stbout, "    PERIOD %s;\n", stb_drvtime(ptstbck->PERIOD));

            if (spctc!=NULL && spctc->master!=NULL)
            {
              fprintf(stbout, "      CLOCK \"%s\" %s %s;\n", spctc->master, (spctc->edges & 2)!=0?"UP":"DOWN", (spctc->edges & 1)!=0?"UP":"DOWN");
            }
        }
        
        fprintf(stbout, "END;\n");
        fprintf(stbout, "\n");
    }
    
   fprintf(stbout, "ClockLatencies\nBegin\n");
   for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
      long a, b, c, d;
      ptttvsig = (ttvsig_list *)ptchain->DATA;
      if ((pt=getptype(ptttvsig->USER, STB_IS_CLOCK))!=NULL)
      {
        spctc=(stb_propagated_clock_to_clock *)pt->DATA;
        if (getptype(ptttvsig->USER, STB_INVERTED_CLOCK))
        {
          a=spctc->latencies.SDNMIN; b=spctc->latencies.SDNMAX; c=spctc->latencies.SUPMIN; d=spctc->latencies.SUPMAX;
        }
        else
        {
          a=spctc->latencies.SUPMIN; b=spctc->latencies.SUPMAX; c=spctc->latencies.SDNMIN; d=spctc->latencies.SDNMAX;
        }
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        fprintf(stbout, "    \"%s\" : %s %s", namebuf, stb_drvtime(a), stb_drvtime(b));
        fprintf(stbout, " %s %s;\n", stb_drvtime(c), stb_drvtime(d));
      }
   }
   fprintf(stbout, "End;\n\n");

    
    
    /* equivalent clock group */
    if ((pt=getptype(ptstbfig->USER, STB_EQUIVALENT))!=NULL)
    {
      int num;
      chain_list *cl;
      fprintf(stbout, "Equivalent Clock Groups\n");
      fprintf(stbout, "Begin\n");
      for (ptchain =(chain_list *)pt->DATA, num=1; ptchain!=NULL; ptchain=ptchain->NEXT, num++)
      {
         fprintf(stbout, "    Group%d:", num);
         for (cl=ptchain->DATA; cl!=NULL; cl=cl->NEXT)
         {
            ptttvsig=(ttvsig_list *)cl->DATA;
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
            fprintf(stbout, " \"%s\"%s", namebuf, cl->NEXT!=NULL?",":";");
         }
         fprintf(stbout, "\n");
      }
      fprintf(stbout, "End;\n\n");
    }

    /* asynchronous clock group */
    if ((pt=getptype(ptstbfig->USER, STB_DOMAIN))!=NULL)
    {
      int num;
      chain_list *cl;
      fprintf(stbout, "Asynchronous Clock Groups\n");
      fprintf(stbout, "Begin\n");
      for (ptchain =(chain_list *)pt->DATA, num=1; ptchain!=NULL; ptchain=ptchain->NEXT, num++)
      {
         fprintf(stbout, "    Group%d:", num);
         for (cl=ptchain->DATA; cl!=NULL; cl=cl->NEXT)
         {
            ptttvsig=(ttvsig_list *)cl->DATA;
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
            fprintf(stbout, " \"%s\"%s", namebuf, cl->NEXT!=NULL?",":";");
         }
         fprintf(stbout, "\n");
      }
      fprintf(stbout, "End;\n\n");
    }

    if ((ifl=getloadedinffig(ptstbfig->FIG->INFO->FIGNAME))!=NULL)
    {
      infDriveStbSpecSection (stbout, ifl, INF_SPECIN, "Specify Input Connectors");
      infDriveStbSpecSection (stbout, ifl, INF_SPECOUT, "Verify Output Connectors");
    }
    
    /* Conditioned Command States */

    if (ptstbfig->COMMAND != NULL) {

        for (ptchain = ptstbfig->COMMAND; ptchain; ptchain = ptchain->NEXT) {
            ptttvsig = (ttvsig_list *)ptchain->DATA;
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
            ptstbnode = stb_getstbnode(ptttvsig->NODE);
            ptstbck = ptstbnode->CK;
            state = ptstbck->VERIF & (STB_UP|STB_DN);
            if (state != 0) {
                if (conditioned == FALSE) {
                    conditioned = TRUE;
                    fprintf(stbout, "CONDITIONED COMMAND STATES\n");
                    fprintf(stbout, "BEGIN\n");
                }
                if (state == STB_UP) fprintf(stbout, "  \"%s\": UP;\n", namebuf);
                else fprintf(stbout, "  \"%s\": DOWN;\n", namebuf);
            }
        }
        
        if (conditioned == TRUE) {
            fprintf(stbout, "END;\n");
            fprintf(stbout, "\n");
        }
    }

    /* Stability of Input Connectors */

    fprintf(stbout, "INPUT CONNECTORS STABILITY\n");
    fprintf(stbout, "BEGIN\n");

    for (ptchain = ptstbfig->CONNECTOR; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if ((ptttvsig->TYPE & TTV_SIG_CI) != TTV_SIG_CI || (ptttvsig->TYPE & TTV_SIG_CB) == TTV_SIG_CB) continue;
        if (stb_getstbnode(ptttvsig->NODE)->CK != NULL) {
         if (stb_getstbnode(ptttvsig->NODE)->CK->TYPE == STB_TYPE_CLOCK)
         continue;
        }
        stb_writesig(stbout, ptstbfig, ptttvsig, format);
    }

    fprintf(stbout, "END;\n");
    fprintf(stbout, "\n");

    /* Stability of Output Connectors */

    fprintf(stbout, "OUTPUT CONNECTORS STABILITY\n");
    fprintf(stbout, "BEGIN\n");

    for (ptchain = ptstbfig->CONNECTOR; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if ((ptttvsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO) continue;
        stb_writesig(stbout, ptstbfig, ptttvsig, format);
    }

    fprintf(stbout, "END;\n");
    fprintf(stbout, "\n");

    /* Stability of Memory nodes */

    if (mode == STB_DRIVE_INTERNALS) {

        for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
            ptevent = (ttvevent_list *)ptchain->DATA;
            ptttvsig = ptevent->ROOT;
            if ((ptttvsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE) continue;
            if ((ptttvsig->TYPE & TTV_SIG_L) != TTV_SIG_L) continue;
            if (memories == FALSE) {
                memories = TRUE;
                fprintf(stbout, "MEMORY NODES STABILITY\n");
                fprintf(stbout, "BEGIN\n");
            }    
            ptttvsig->TYPE |= TTV_SIG_MARQUE;
            stb_writesig(stbout, ptstbfig, ptttvsig, format);
        }

        for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
            ptevent = (ttvevent_list *)ptchain->DATA;
            ptttvsig = ptevent->ROOT;
            ptttvsig->TYPE &= ~(TTV_SIG_MARQUE);
        }

        if (memories == TRUE) {
            fprintf(stbout, "END;\n");
            fprintf(stbout, "\n");
        }
    }

    /* Stability of Internal nodes */

    if (mode == STB_DRIVE_INTERNALS) {

        for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
            ptevent = (ttvevent_list *)ptchain->DATA;
            ptttvsig = ptevent->ROOT;
            if ((ptttvsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE) continue;
            if ((ptttvsig->TYPE & TTV_SIG_C) == TTV_SIG_C) continue;
            if ((ptttvsig->TYPE & TTV_SIG_L) == TTV_SIG_L) continue;
            if (internals == FALSE) {
                internals = TRUE;
                fprintf(stbout, "INTERNAL NODES STABILITY\n");
                fprintf(stbout, "BEGIN\n");
            }    
            ptttvsig->TYPE |= TTV_SIG_MARQUE;
            stb_writesig(stbout, ptstbfig, ptttvsig, format);
        }

        for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
            ptevent = (ttvevent_list *)ptchain->DATA;
            ptttvsig = ptevent->ROOT;
            ptttvsig->TYPE &= ~(TTV_SIG_MARQUE);
        }

        if (internals == TRUE) {
            fprintf(stbout, "END;\n");
            fprintf(stbout, "\n");
        }
    }

    fprintf(stbout, "SetupHold\n");
    fprintf(stbout, "Begin\n");

    fprintf(stbout, "# <signal name> : ( <fallsetup> <fallhold> ) ( <risesetup> <risehold> ) ;\n"); 
    for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
            ptevent = (ttvevent_list *)ptchain->DATA;
            ptttvsig = ptevent->ROOT;
            if ((ptttvsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE) continue;
            ptttvsig->TYPE |= TTV_SIG_MARQUE;
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
            stb_writesetuphold(stbout, ptttvsig, namebuf);
        }
    fprintf(stbout, "End;\n");
    fprintf(stbout, "\n");


    fprintf(stbout, "StabFlags\n");
    fprintf(stbout, "Begin\n");
    for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
            ptevent = (ttvevent_list *)ptchain->DATA;
            ptttvsig = ptevent->ROOT;
            if ((ptttvsig->TYPE & TTV_SIG_MARQUE) != TTV_SIG_MARQUE) continue;
            ptttvsig->TYPE &= ~TTV_SIG_MARQUE;
            ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
            stb_writeflags(stbout, ptttvsig, namebuf);
    }
    fprintf(stbout, "End;\n");
    fprintf(stbout, "\n");

    for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) {
        ptevent = (ttvevent_list *)ptchain->DATA;
        ptttvsig = ptevent->ROOT;
        ptttvsig->TYPE &= ~TTV_SIG_MARQUE;
    }

/*    fprintf (stbout, "Directives\nBegin\n");
    for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) 
      {
        ptevent = (ttvevent_list *)ptchain->DATA;
        ptttvsig = ptevent->ROOT;
        if ((ptttvsig->TYPE & TTV_SIG_MARQUE) == TTV_SIG_MARQUE) continue;
        ptttvsig->TYPE |= TTV_SIG_MARQUE;
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        if ((sd=stb_get_directive(ptttvsig))!=NULL)
          {
            while (sd!=NULL)
              {
                if (sd->filter) fprintf (stbout, "  FILTER: "); else fprintf (stbout, "  CHECK: ");
                
                if (sd->target1_dir & INF_DIRECTIVE_CLOCK) fprintf (stbout, "CLOCK ");
                fprintf (stbout, "\"%s\" ", namebuf);
                
                if (sd->target1_dir & INF_DIRECTIVE_UP) fprintf(stbout, "Up ");
                else if (sd->target1_dir & INF_DIRECTIVE_DOWN) fprintf(stbout, "Down ");
                else if (sd->target1_dir & INF_DIRECTIVE_RISING) fprintf(stbout, "Rising ");
                else if (sd->target1_dir & INF_DIRECTIVE_FALLING) fprintf(stbout, "Falling ");
                
                if (sd->operation & INF_DIRECTIVE_BEFORE) fprintf(stbout, "Before ");
                else if (sd->operation & INF_DIRECTIVE_AFTER) fprintf(stbout, "After ");
                else fprintf(stbout, "With ");
                
                if (sd->target2!=NULL)
                  {
                    char namebuf2[1024];
                    ttv_getsigname(ptstbfig->FIG, namebuf2, sd->target2);
                    if (sd->target2_dir & INF_DIRECTIVE_CLOCK) fprintf(stbout, "Clock ");
                    fprintf(stbout, "\"%s\" ", namebuf2);
                    if (sd->target2_dir & INF_DIRECTIVE_UP) fprintf(stbout, "Up ");
                    else if (sd->target2_dir & INF_DIRECTIVE_DOWN) fprintf(stbout, "Down ");
                    else if (sd->target2_dir & INF_DIRECTIVE_RISING) fprintf(stbout, "Rising ");
                    else if (sd->target2_dir & INF_DIRECTIVE_FALLING) fprintf(stbout, "Falling ");
                    
                    if (sd->margin!=0) fprintf(stbout, "Margin %.1f", sd->margin/TTV_UNIT);
                  }
                fprintf(stbout,";\n");
                sd=sd->next;
              }
          }
      }
     
    for (ptchain = ptstbfig->NODE; ptchain; ptchain = ptchain->NEXT) 
      {
        ptevent = (ttvevent_list *)ptchain->DATA;
        ptttvsig = ptevent->ROOT;
        ptttvsig->TYPE &= ~TTV_SIG_MARQUE;
      }
    fprintf(stbout, "End;\n");
    fprintf(stbout, "\n");
*/
    
    if (fclose(stbout)) {
	    stb_error(ERR_CANNOT_CLOSE, NULL, 0, STB_FATAL);
    }

    return 0; 
}

/*****************************************************************************
*                           fonction stb_writesig()                          *
*****************************************************************************/
static void
stb_writesig(ptfile, ptstbfig, ptttvsig, format)
    FILE           *ptfile;
    stbfig_list    *ptstbfig;
    ttvsig_list    *ptttvsig;
    int             format;
{
    stbnode        *ptstbnode_up;
    stbnode        *ptstbnode_dn;
    char            namebuf[1024];
    char            phasebuf[1024];
    char            active_edge;
    int             i;

    ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
    ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
    ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
    for (i = 0; i < ptstbfig->PHASENUMBER; i++) {
        /* add source phase */
        strcpy(phasebuf, " FROM \"");
        if (stb_getclock(ptstbfig, i, phasebuf+7, &active_edge, NULL) == NULL) {
            if (i == 0) {/* combinatorial */
                phasebuf[0] = 0;
            }
            else
            {
               stb_error(ERR_UNKNOWN_CLOCK, namebuf, i, STB_NONFATAL/*STB_FATAL*/);
               continue;
            }
        }
        else {
            strcat(phasebuf, "\"");
            if (active_edge == STB_SLOPE_DN) {
                strcat(phasebuf, " FALLING");
            }
            else strcat(phasebuf, " RISING");
        }

        /* write intervals for hz precharge/evaluate */
        if (ptstbnode_dn != NULL) {
            if (ptstbnode_dn->STBHZ != NULL) {
                if (ptstbnode_dn->STBHZ[i] != NULL) {
                    if (ptstbnode_dn->CK->TYPE == STB_TYPE_PRECHARGE) {
                        fprintf(ptfile, "  \"%s\" FALLING%s WITHOUT PRECHARGE:\n", namebuf, phasebuf);
                        stb_writeintervals(ptfile, ptstbnode_dn->STBHZ[i], format);
                        stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_dn, 1, i);
                    }
                    else if (ptstbnode_dn->CK->TYPE == STB_TYPE_EVAL) {
                        fprintf(ptfile, "  \"%s\" FALLING%s WITHOUT EVALUATE:\n", namebuf, phasebuf);
                        stb_writeintervals(ptfile, ptstbnode_dn->STBHZ[i], format);
                        stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_dn, 1, i);
                    }
                }
            }
        }
        if (ptstbnode_up != NULL) {
            if (ptstbnode_up->STBHZ != NULL) {
                if (ptstbnode_up->STBHZ[i] != NULL) {
                    if (ptstbnode_up->CK->TYPE == STB_TYPE_PRECHARGE) {
                        fprintf(ptfile, "  \"%s\" RISING%s WITHOUT PRECHARGE:\n", namebuf, phasebuf);
                        stb_writeintervals(ptfile, ptstbnode_up->STBHZ[i], format);
                        stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_up, 1, i);
                    }
                    else if (ptstbnode_up->CK->TYPE == STB_TYPE_EVAL) {
                        fprintf(ptfile, "  \"%s\" RISING%s WITHOUT EVALUATE:\n", namebuf, phasebuf);
                        stb_writeintervals(ptfile, ptstbnode_up->STBHZ[i], format);
                        stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_up, 1, i);
                    }
                }
            }
        }
     
        /* write intervals for the current phase */
        if (ptstbnode_up != NULL && ptstbnode_dn != NULL) {
            if (stb_compstbpairlist(ptstbnode_dn->STBTAB[i], ptstbnode_up->STBTAB[i]) == TRUE) {
                if (ptstbnode_dn->STBTAB[i] != NULL) {
                    fprintf(ptfile, "  \"%s\"%s:\n", namebuf, phasebuf);
                    stb_writeintervals(ptfile, ptstbnode_dn->STBTAB[i], format);
                    stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_dn, 0, i);
                }
            }
            else {
                if (ptstbnode_dn->STBTAB[i] != NULL) {
                    fprintf(ptfile, "  \"%s\" FALLING%s:\n", namebuf, phasebuf);
                    stb_writeintervals(ptfile, ptstbnode_dn->STBTAB[i], format);
                    stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_dn, 0, i);
                }
                if (ptstbnode_up->STBTAB[i] != NULL) {
                    fprintf(ptfile, "  \"%s\" RISING%s:\n", namebuf, phasebuf);
                    stb_writeintervals(ptfile, ptstbnode_up->STBTAB[i], format);
                    stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_up, 0, i);
                }
            }
        }
        else if (ptstbnode_up != NULL) {
            if (ptstbnode_up->STBTAB[i] != NULL) {
                fprintf(ptfile, "  \"%s\" RISING%s:\n", namebuf, phasebuf);
                stb_writeintervals(ptfile, ptstbnode_up->STBTAB[i], format);
                stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_up, 0, i);
            }
        }
        else if (ptstbnode_dn != NULL) {
            if (ptstbnode_dn->STBTAB[i] != NULL) {
                fprintf(ptfile, "  \"%s\" FALLING%s:\n", namebuf, phasebuf);
                stb_writeintervals(ptfile, ptstbnode_dn->STBTAB[i], format);
                stb_writeintervals_split(ptfile, ptstbfig, ptstbnode_dn, 0, i);
            }
        }
    }
}

/*****************************************************************************
*                           fonction stb_writeintervals()                    *
*****************************************************************************/

static void
stb_writeintervals_split(FILE *ptfile, stbfig_list *sb, stbnode *node, int hz, char phase)
{
  stbpair_list   *ptstbpair, *pair;
  stbpair_list *STBTAB[256];
  stbpair_list *STBHZ[256]; 
  int i;
  stbck *ck;
  char buf[1024];
  
  if (node->CK==NULL || node->CK->NEXT==NULL) return;
  
  for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
    {
      if (ck->CKINDEX==phase)
        {
          for (i=0; i<node->NBINDEX; i++)
            {
              STBHZ[i]=NULL, STBTAB[i]=NULL;
            }
          stb_transferstbline (sb, node->EVENT, node , STBTAB, STBHZ, 0, 1, NULL, ck, 1, (node->FLAG & STB_NODE_STABCORRECT)!=0?STB_TRANSFERT_CORRECTION:0) ;
          pair = stb_globalstbtab (!hz?STBTAB:STBHZ, node->NBINDEX);
          stb_freestbtabpair(STBHZ, node->NBINDEX);
          stb_freestbtabpair(STBTAB, node->NBINDEX);
          
          if (pair!=NULL)
            {
              if (ck->CMD==NULL) fprintf(ptfile, "#no cmd:");
              else fprintf(ptfile, "#cmd %s (%c):", ttv_getsigname(sb->FIG,buf,ck->CMD->ROOT), (ck->CMD->TYPE & TTV_NODE_UP)?'R':'F');
              for (ptstbpair = pair; ptstbpair; ptstbpair = ptstbpair->NEXT)
                fprintf(ptfile, " %s-%s", stb_drvtime(ptstbpair->D), stb_drvtime(ptstbpair->U));
              fprintf(ptfile, "\n");
            }
          stb_freestbpair(pair);
        }
    }
}
static void
stb_writeintervals(ptfile, ptheadstbpair, format)
    FILE           *ptfile;
    stbpair_list   *ptheadstbpair;
    int             format;
{
    stbpair_list   *ptstbpair;

    if (format == STB_SEPARATE_INTERVALS) {
        for (ptstbpair = ptheadstbpair; ptstbpair; ptstbpair = ptstbpair->NEXT) {
            fprintf(ptfile, "    UNSTABLE %s;\n", stb_drvtime(ptstbpair->D));
            fprintf(ptfile, "    STABLE   %s;\n", stb_drvtime(ptstbpair->U));
        }
    }
    else {
        fprintf(ptfile, "    UNSTABLE");
        for (ptstbpair = ptheadstbpair; ptstbpair; ptstbpair = ptstbpair->NEXT) {
            fprintf(ptfile, " %s%s", stb_drvtime(ptstbpair->D), ptstbpair->NEXT?" ":"");
        }
        fprintf(ptfile, ";\n");
        fprintf(ptfile, "    STABLE  ");
        for (ptstbpair = ptheadstbpair; ptstbpair; ptstbpair = ptstbpair->NEXT) {
            fprintf(ptfile, " %s%s", stb_drvtime(ptstbpair->U), ptstbpair->NEXT?" ":"");
        }
        fprintf(ptfile, ";\n");
    }
}

/*****************************************************************************
*                           fonction stb_drvsigerr()                         *
*****************************************************************************/
void stb_drvsigerr(file,ptstbfig,ptsig)
    FILE *file ;
    stbfig_list *ptstbfig;
    ttvsig_list *ptsig ;
{
    stbdebug_list *debug ;
    stbdebug_list *deb ;
    char namebuf[1024], buf[1024];

   debug = stb_debugstberror(ptstbfig,ptsig,(long)0, 0);

   for(deb = debug ; deb ; deb = deb->NEXT) {
     if (deb->START_CMD_EVENT!=NULL)
       sprintf(buf, " CMD '%s' (%c)", ttv_getsigname(ptstbfig->FIG,namebuf,deb->START_CMD_EVENT->ROOT), (deb->START_CMD_EVENT->TYPE & TTV_NODE_UP)?'R':'F');
     else
       strcpy(buf,"");
      fprintf(file, "\tERROR FROM '%s'%s:",ttv_getsigname(ptstbfig->FIG,namebuf,deb->SIG1), buf);
      if (deb->SETUP != STB_NO_TIME) {
          fprintf(file, " SETUP=%s", stb_drvtime(deb->SETUP));
      }
      if (deb->HOLD != STB_NO_TIME) {
          fprintf(file, " HOLD=%s", stb_drvtime(deb->HOLD));
      }
      fprintf(file, "\n");
   }
   stb_freestbdebuglist(debug) ;
}

/*****************************************************************************
*                           fonction stb_drvreport()                         *
*****************************************************************************/
int stb_getdatalag(ttvfig_list *ttvfig, ttvevent_list *latch, double *time)
{
 stbfig_list *stbfig ;
 long datamin ;
 long datamax=0, dmax ;
 char phase ;
 stbck *ck;
 stbnode *node;

 *time=-1;
 if((((latch->ROOT->TYPE & TTV_SIG_LL) !=  TTV_SIG_LL) &&
     ((latch->ROOT->TYPE & TTV_SIG_R) !=  TTV_SIG_R))) 
    return(0) ;

 if (ttv_testsigflag(latch->ROOT, TTV_SIG_FLAGS_DONTCROSS))
   return 0;
  
 if((stbfig = stb_getstbfig(ttvfig)) == NULL)
   return(0) ;

 stb_geteventphase(stbfig,latch,&phase,NULL,NULL,1,1) ;

 if(phase == STB_NO_INDEX)
 {
   *time=-2;
   return(1) ;
 }

 node = stb_getstbnode (latch);
 if (node!=NULL)
 {
   for (ck=node->CK; ck!=NULL; ck=ck->NEXT)
   {
     if (stb_IsClockCK(node->EVENT, ck)) continue;
     stb_getstbdelta(stbfig, latch, &datamin, &dmax, ck->CMD);
     if (dmax>datamax) datamax=dmax;
   }
 }
 *time=(datamax/TTV_UNIT)*1e-12;
 if(datamax <= 0)
   {
    *time=-1;
    return(0) ;
   }

 return(1) ;
}

void stb_drvreport(stbfig,file,type,slope,name,setup,hold,ck,lag,node)
    stbfig_list *stbfig ;
    FILE *file ;
    char *type ;
    char *slope ;
    char *name ;
    long setup ;
    long hold ;
    stbck *ck;
    double lag;
    ttvevent_list *node;
    
{
   char upmin[32];
   char upmax[32];
   char dnmin[32];
   char dnmax[32];
   char nameclock[1024];
   char namecmd[1024];
   char edge ;
   char *up, *down; 
   int invert;
   
   if ((setup != STB_NO_TIME) || (hold != STB_NO_TIME)) {
       fprintf(file, "%s '%s' %s:", type, name, slope);
       if (setup != STB_NO_TIME) {
           fprintf(file, " SETUP=%s", stb_drvtime(setup));
       }
       if (hold != STB_NO_TIME) {
           fprintf(file, " HOLD=%s", stb_drvtime(hold));
       }
       if (lag!=-3)
       {
         if (lag==-2)
           fprintf(file, " TRANSPARENT=ALWAYS");
         else if (lag!=-1)
           fprintf(file, " TRANSPARENT=%s", stb_drvtime(mbk_long_round(lag*TTV_UNIT*1e12)));
         else 
           fprintf(file, " NOT-TRANSPARENT");
       }
       
       fprintf(file, "\n");
       for(; ck ; ck = ck->NEXT) {
         if (stb_IsClockCK(node, ck)) continue;
         if (ck->CMD!=NULL && stbfig!=NULL && !stb_cmd_can_generate_event(stbfig, node, ck->CMD)) continue;
           if(ck->CKINDEX != STB_NO_INDEX) {
              if(stb_getclock(stbfig,ck->CKINDEX,nameclock,&edge, ck) == NULL)
                *nameclock='\0' ;
           }
           else *nameclock='\0' ;
           if(ck->CMD != NULL) {
               ttv_getsigname(stbfig->FIG,namecmd,ck->CMD->ROOT) ;
           }
           else *namecmd='\0' ;
           if (ck->SUPMIN == STB_NO_TIME) strcpy(upmin, "NO_TIME");
           else sprintf(upmin, "%s", stb_drvtime(ck->SUPMIN));
           if (ck->SUPMAX == STB_NO_TIME) strcpy(upmax, "NO_TIME");
           else sprintf(upmax, "%s", stb_drvtime(ck->SUPMAX));
           if (ck->SDNMIN == STB_NO_TIME) strcpy(dnmin, "NO_TIME");
           else sprintf(dnmin, "%s", stb_drvtime(ck->SDNMIN));
           if (ck->SDNMAX == STB_NO_TIME) strcpy(dnmax, "NO_TIME");
           else sprintf(dnmax, "%s", stb_drvtime(ck->SDNMAX));
           fprintf(file, "\tCLOCK: ");
           if(*nameclock != '\0')
           {
             if (STB_OPEN_LATCH_PHASE == 'N')
               fprintf(file, "%s(%s) ",nameclock,(edge == STB_SLOPE_DN)?"DOWN":"UP");
             else
               fprintf(file, "%s(%s) ",nameclock,(edge != STB_SLOPE_DN)?"DOWN":"UP");
           }
           up="UP", down="DN"; 
           if(*namecmd != '\0')
           {
             stb_getgoodclock_and_status(stbfig, ck, ck->CMD, node, &invert);
                     
             if(invert)
             {
               up="DN"; down="UP";
             }
             fprintf(file, "COMMAND : %s(%s) ",namecmd, (ck->CMD->TYPE & TTV_NODE_UP)==TTV_NODE_UP?"DOWN":"UP");
           }
           fprintf(file, "%sMIN=%s, %sMAX=%s, %sMIN=%s, %sMAX=%s\n",up, upmin,up, upmax,down,dnmin,down,dnmax);
       }
   }
}

/*****************************************************************************
*                           fonction stb_report()                            *
*****************************************************************************/

void
stb_report(stbfig_list   *ptstbfig, char *filename)
{
    FILE           *stbrep;
    ttvsig_list    *ptttvsig;
    stbnode        *ptstbnode_up;
    stbnode        *ptstbnode_dn;
    chain_list     *ptchain;
    char            namebuf[1024];
    double lag;
    
    stbrep = mbkfopen(filename==NULL?ptstbfig->FIG->INFO->FIGNAME:filename, filename==NULL?"str":NULL, WRITE_TEXT);
    if (stbrep== NULL) {
	    stb_error(ERR_CANNOT_OPEN, NULL, 0, STB_FATAL);
    }

    sprintf(namebuf,"Stability report : %s.str\n",filename==NULL?ptstbfig->FIG->INFO->FIGNAME:filename);
    avt_printExecInfo(stbrep, "#", namebuf, "");

    fprintf(stbrep, "NUMBER OF PHASES: %d\n", (int)ptstbfig->PHASENUMBER);
    for (ptchain = ptstbfig->CLOCK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ptstbnode_up = stb_getstbnode(ptttvsig->NODE);
        ptstbnode_dn = stb_getstbnode(ptttvsig->NODE+1);
        fprintf(stbrep, "CLOCK %s :", ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig));
        if (ptstbnode_up->CK->CKINDEX != STB_NO_INDEX) 
           fprintf(stbrep, " FALLING(%d)", ptstbnode_up->CK->CKINDEX) ;
        if (ptstbnode_dn->CK->CKINDEX != STB_NO_INDEX) 
           fprintf(stbrep, " RISING(%d)", ptstbnode_dn->CK->CKINDEX) ;
        fprintf(stbrep, "\n") ;
    }
     
    fprintf(stbrep, "STABLE: ");
    if (ptstbfig->STABILITYFLAG == STB_STABLE) {
        fprintf(stbrep, "YES\n\n");
    }
    else fprintf(stbrep, "NO\n\n");

    lag=-3;
    for (ptchain = ptstbfig->CONNECTOR; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        if ((ptttvsig->TYPE & TTV_SIG_CO) != TTV_SIG_CO) continue;
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
        stb_drvreport(ptstbfig,stbrep, "OUTPUT", "FALLING", namebuf, ptstbnode_dn->SETUP, ptstbnode_dn->HOLD, NULL, lag, ptttvsig->NODE);
        ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
        stb_drvreport(ptstbfig,stbrep, "OUTPUT", "RISING", namebuf, ptstbnode_up->SETUP, ptstbnode_up->HOLD, NULL, lag, ptttvsig->NODE+1);
        if((ptstbnode_up->SETUP <= (long)0) || (ptstbnode_up->HOLD <= (long)0)||
           (ptstbnode_dn->SETUP <= (long)0) || (ptstbnode_dn->HOLD <= (long)0))
          stb_drvsigerr(stbrep,ptstbfig,ptttvsig) ;
    }

    lag=-3;
    for (ptchain = ptstbfig->MEMORY; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
        stb_getdatalag(ptstbfig->FIG, ptttvsig->NODE, &lag);
        stb_drvreport(ptstbfig,stbrep, "MEMORY", "FALLING", namebuf, ptstbnode_dn->SETUP, ptstbnode_dn->HOLD, ptstbnode_dn->CK, lag, ptttvsig->NODE);
        ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
        stb_getdatalag(ptstbfig->FIG, ptttvsig->NODE+1, &lag);
        stb_drvreport(ptstbfig,stbrep, "MEMORY", "RISING", namebuf, ptstbnode_up->SETUP, ptstbnode_up->HOLD, ptstbnode_up->CK, lag, ptttvsig->NODE+1);
        if((ptstbnode_up->SETUP <= (long)0) || (ptstbnode_up->HOLD <= (long)0)||
           (ptstbnode_dn->SETUP <= (long)0) || (ptstbnode_dn->HOLD <= (long)0))
          stb_drvsigerr(stbrep,ptstbfig,ptttvsig) ;
    }

    lag=-3;
    for (ptchain = ptstbfig->COMMAND; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
        stb_drvreport(ptstbfig,stbrep, "COMMAND", "FALLING", namebuf, ptstbnode_dn->SETUP, ptstbnode_dn->HOLD, ptstbnode_dn->CK, lag, ptttvsig->NODE);
        ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
        stb_drvreport(ptstbfig,stbrep, "COMMAND", "RISING", namebuf, ptstbnode_up->SETUP, ptstbnode_up->HOLD, ptstbnode_up->CK, lag, ptttvsig->NODE+1);
        if((ptstbnode_up->SETUP <= (long)0) || (ptstbnode_up->HOLD <= (long)0)||
           (ptstbnode_dn->SETUP <= (long)0) || (ptstbnode_dn->HOLD <= (long)0))
          stb_drvsigerr(stbrep,ptstbfig,ptttvsig) ;
    }

    lag=-3;
    for (ptchain = ptstbfig->PRECHARGE; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
        stb_getdatalag(ptstbfig->FIG, ptttvsig->NODE, &lag);
        stb_drvreport(ptstbfig,stbrep, "PRECHARGE", "FALLING", namebuf, ptstbnode_dn->SETUP, ptstbnode_dn->HOLD, ptstbnode_dn->CK, lag, ptttvsig->NODE);
        ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
        stb_getdatalag(ptstbfig->FIG, ptttvsig->NODE+1, &lag);
        stb_drvreport(ptstbfig,stbrep, "PRECHARGE",  "RISING", namebuf, ptstbnode_up->SETUP, ptstbnode_up->HOLD, ptstbnode_up->CK, lag, ptttvsig->NODE+1);
        if((ptstbnode_up->SETUP <= (long)0) || (ptstbnode_up->HOLD <= (long)0)||
           (ptstbnode_dn->SETUP <= (long)0) || (ptstbnode_dn->HOLD <= (long)0))
          stb_drvsigerr(stbrep,ptstbfig,ptttvsig) ;
    }

    lag=-3;
    for (ptchain = ptstbfig->BREAK; ptchain; ptchain = ptchain->NEXT) {
        ptttvsig = (ttvsig_list *)ptchain->DATA;
        ttv_getsigname(ptstbfig->FIG, namebuf, ptttvsig);
        ptstbnode_dn = stb_getstbnode(ptttvsig->NODE);
        stb_drvreport(ptstbfig,stbrep, "BREAK", "FALLING", namebuf, ptstbnode_dn->SETUP, ptstbnode_dn->HOLD, ptstbnode_dn->CK, lag, ptttvsig->NODE);
        ptstbnode_up = stb_getstbnode(ptttvsig->NODE+1);
        stb_drvreport(ptstbfig,stbrep, "BREAK",  "RISING", namebuf, ptstbnode_up->SETUP, ptstbnode_up->HOLD, ptstbnode_up->CK, lag, ptttvsig->NODE+1);
        if((ptstbnode_up->SETUP <= (long)0) || (ptstbnode_up->HOLD <= (long)0)||
           (ptstbnode_dn->SETUP <= (long)0) || (ptstbnode_dn->HOLD <= (long)0))
          stb_drvsigerr(stbrep,ptstbfig,ptttvsig) ;
    }

    if (fclose(stbrep)) {
	    stb_error(ERR_CANNOT_CLOSE, NULL, 0, STB_FATAL);
    }
}

char* stb_drvtime( long time )
{
  static int  n=0;
  static char buffer[4][64];

  n++;
  if( n==4 )
    n=0;
  sprintf( buffer[n], "%.1f", (double)time / TTV_UNIT );

  return buffer[n] ;
}
