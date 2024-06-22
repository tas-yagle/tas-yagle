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

/*******************************************************************************
 *                                                                              *
 *  Tool        : Spice parser / driver v 7.00                                  *
 *  Author(s)   : Gregoire AVOT                                                 *
 *  Updates     : March, 18th 1998                                              *
 *                                                                              *
 *******************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <alloca.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <float.h>
#include <pthread.h>

#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include EQT_H
#include MCC_H
#include MSL_H
#include SIM_H
#include AVT_H
#include "spi_int.h"
#include "spi_hash.h"
#include "spi_parse.h"

// #define HANDLE_XY

#define SPICE_UNSOLVED_VCARDS 0xfab60919
#define SPICE_MULTI_CONNECTOR_NAMES 0xfab70222

#define TOPCIRINFNAME "xxoo$TOPCIR$ooxx"

int SPI_MODEL_CORNER = MCC_TYPICAL;
static ptype_list *UNRESOLVED_EXP = NULL;
static char *SIGNAME = NULL;

static char SPI_COM_CHAR='$';

/* Fonctions dynamiques */
static chain_list *ext_handler_list = NULL;
static chain_list *cmp_handler_list = NULL;
static int ext_handlers ();
static int cmp_handlers ();

/* Variables globales MSL */
lofig_list *msl_ptfig;
int msl_figloaded;
char *msl_figname;
char *msl_subckt;
int msl_line;

char SPI_SEPAR;
char *SPI_NETNAME;
char *SPI_SUFFIX;
char SPI_ONE_NODE_NORC;
char SPI_MERGE;
char SPI_VERBOSE;
char SPI_CHECK_NAME;
char SPI_AUTO_LOAD;
char SPI_PARSE_FIRSTLINE;
char SPI_CREATE_TOP_FIGURE;
char SPI_IGNORE_MODELS;
char SPI_IGNORE_DIODES;
char SPI_IGNORE_CAPA;
float SPI_MIN_CAPA;
char SPI_IGNORE_RESI;
char SPI_IGNORE_VOLTAGE;
char SPI_ZEROVOLT_RESI;
char SPI_MERGE_DIODES;
char SPI_JFET_RESI;
char SPI_INSTANCE_MULTINODE;
float SPI_MAX_RESI;
float SPI_MIN_RESI;
char SPI_NORCCAPA_TOGND;
char SPI_NO_SIG_PREFIX;
float SPI_SCALE_CAPAFACTOR = 1.0;
float SPI_SCALE_RESIFACTOR = 1.0;
float SPI_SCALE_TRANSFACTOR = 1.0;
float SPI_SCALE_DIODEFACTOR = 1.0;
static char SPI_LIBRARY = 0;
char SPI_IGNORE_BULK = 'N';
char SPI_USE_PRELOADED_LIB;
char SPI_DEBUGPARSE;
static long SPI_RC_MEMORY_LIMIT = 0;
static double SPI_GLOBAL_TEMP, SPI_GLOBAL_SCALE;
int SPI_TOLERANCE = 0;
int SPI_MONTE_CARLO_ACTIVE=0;

/* static variables */

static chain_list *SPI_GLOBAL_NODES;
static chain_list *SPI_PARSED_LOFIG;
static chain_list *SPI_UNRESOLVED_CIRCUITS = NULL;
ptype_list *SPI_LIBNAME = NULL;
static chain_list *CMODEL=NULL, *RMODEL=NULL;
static circuit *SPI_TOPCIR = NULL;

ginterf *SPI_PRELOADED_LIB = NULL;

eqt_ctx *GLOBAL_CTX = NULL;

extern eqt_param *MBK_GLOBALPARAMS;
extern chain_list *MBK_GLOBALFUNC;

#define KEEP__TRANSISTOR  0x1
#define KEEP__RESISTANCE  0x2
#define KEEP__INSTANCE  0x4
#define KEEP__NONE  0x8
#define KEEP__ALLSIGNALS  0x10
#define KEEP__CAPA  0x20
#define KEEP__DIODE  0x40
#define KEEP__ALL  (KEEP__CAPA|KEEP__TRANSISTOR|KEEP__RESISTANCE|KEEP__INSTANCE|KEEP__ALLSIGNALS|KEEP__DIODE|0x1000)

#define SF_KEEP_BBOX    0x1
#define SF_TRANSFERTTOP 0x2
#define SF_EXPLICITINSTANCENAMES 0x4

int SPI_DRIVE_SPEF_MODE = 0;
int SPICE_KEEP_NAMES, SPICE_KEEP_CARDS, SPICE_SF;


static char *_v2_spi_devect (char *nom, char *nouv);

static double spi_eval (spifile * df, eqt_ctx * ctx, char *ptexp, char **ptparamexpr, int failifrandom, int checkneg);
static char *spi_stripquotes (char *ptexp);
static void spi_displayunresolved (char *failname, chain_list * cirlist);
static void display_decompligne (spifile * df, int i);
static void log_decompligne (spifile * df, int i, int loglevel);
static circuit *spi_create_circuit (char *name);
static int spi_empty_circuit (circuit * ptcir);
static void spi_create_top_interface (lofig_list * ptfig);

static int get_equi_separ_pos (char *name);

typedef struct
{
  struct
  {
    union
    {
      losig_list *sig;
      noeud *node;
    } u;
    char *name, *nodename;
    long rcn;
  } s[2];
  float value;
  char *expr;
} unsolved_vcard_type;
/******************************************************************************\
  strip_space
\******************************************************************************/
char *strip_space (char *name)
{
    char *stripped;
    int i, l, j = 0;

    stripped = (char *)mbkalloc (l = strlen (name) + 1);
    for (i = 0; i < l - 1; i++)
        if (name[i] != ' ')
            stripped[j++] = name[i];
    stripped[j] = '\0';

    return stripped;
}

/******************************************************************************\
  spi_set_model_corner
\******************************************************************************/
void spi_set_model_corner (int corner)
{
    SPI_MODEL_CORNER = corner;
}

/******************************************************************************/

/******************************************************************************/

eqt_ctx *spi_init_eqt ()
{
    eqt_ctx *ctx = eqt_init (50);
    eqt_add_spice_extension(ctx);
    return ctx;
}

/******************************************************************************/

/*
#define SPI_VAL_LIMIT 1e-3
#define SPI_RESCALE_UNIT 1e-6

float spi_rescale_if_too_big(float val, float scale, float scalepow)
{
  if (val!=0 && val*pow(scale, scalepow)>pow(SPI_VAL_LIMIT, scalepow)) return val*pow(SPI_RESCALE_UNIT, scalepow);
  return val;
}
*/

static chain_list *spi_findfigure (chain_list * parsedlofig, char *nom)
{
    chain_list *cl;
    for (cl = parsedlofig; cl != NULL && mbk_casestrcmp (((lofig_list *) cl->DATA)->NAME, nom) != 0; cl = cl->NEXT);
    return cl;
}

static void spi_updatelofiginfo (lofig_list * topfig, chain_list * parsedlofig, chain_list * globalnames)
{
    losig_list *ls;
    chain_list *findname;
    char *sname;
    inffig_list *ifl;
    double val;
    float val0;
    ht *globalalim;
    long l;
    chain_list *cl, *ch;
    lofig_list *start;
    lofiginfo *lfif;
    globalalim = addht (10);

    if (topfig != NULL) {
        for (ls = topfig->LOSIG; ls != NULL; ls = ls->NEXT) {
            for (cl=ls->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
            {
              sname = (char *)cl->DATA;
              for (findname = globalnames; findname != NULL && findname->DATA != sname; findname = findname->NEXT);
              if (findname != NULL) {
                  ls->ALIMFLAGS |= MBK_HAS_GLOBAL_NODE_FLAG;
                  if (getlosigalim (ls, &val0))
                      addhtitem (globalalim, sname, (long)ls);
              }
            }
        }
    }
    for (cl = parsedlofig; cl != NULL; cl = cl->NEXT)
        ((lofig_list *) cl->DATA)->FLAGS0 = 0;
    for (cl = parsedlofig; cl != NULL; cl = cl->NEXT) {
        start = (lofig_list *) cl->DATA;

        if (start->FLAGS0)
            continue;

        if (start != topfig) {
            if (globalnames != NULL) {
                for (ls = start->LOSIG; ls != NULL; ls = ls->NEXT) {
                    for (ch=ls->NAMECHAIN; ch!=NULL; ch=ch->NEXT)
                    {
                      sname = (char *)ch->DATA;
                      for (findname = globalnames; findname != NULL && findname->DATA != sname; findname = findname->NEXT);
                      if (findname != NULL) {
                          ls->ALIMFLAGS |= MBK_HAS_GLOBAL_NODE_FLAG;
                          if ((l = gethtitem (globalalim, sname)) != EMPTYHT)
                              duplosigalim ((losig_list *) l, ls);
                      }
                    }
                }
            }

            if (SPI_GLOBAL_TEMP != -DBL_MAX) {
                if ((ifl = getloadedinffig (start->NAME)) == NULL)
                    ifl = addinffig (start->NAME);
                if (inf_GetDouble (ifl, INF_OPERATING_CONDITION, INF_TEMPERATURE, &val) == 0) {
                    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, INF_OPERATING_CONDITION, INF_TEMPERATURE, SPI_GLOBAL_TEMP,
                                   NULL);
                }
            }

            if (SPI_GLOBAL_SCALE != 1) {
                lfif = mbk_getlofiginfo (start, 1);
                lfif->scale = SPI_GLOBAL_SCALE;
            }
        }
        start->FLAGS0 = 1;
    }
    delht (globalalim);
}

static void create_corresponding_hier_name(char *buf, char *dest)
{
  while (*buf!='\0')
  {
    if (*buf=='.')
    {
      *dest=SEPAR;
      if ((SPICE_KEEP_CARDS & KEEP__INSTANCE)==0 && tolower(*(buf+1))=='x') buf++;
    }
    else *dest=*buf;
    dest++;
    buf++;
  }
  *dest='\0';
}

static char *sub_getconnectorname(lofig_list *lf, locon_list *lc, long node)
{
  chain_list *phinterf=NULL, *realinterf=NULL, *cl, *ch;
  ptype_list *pt;
  num_list *nm;
  int cnt, cnt0;

  if ((pt=getptype(lf->USER, PH_INTERF))!=NULL) phinterf=(chain_list *)pt->DATA;
  if ((pt=getptype(lf->USER, PH_REAL_INTERF))!=NULL) realinterf=(chain_list *)pt->DATA;
  
  for (nm=lc->PNODE, cnt=1; nm && nm->DATA!=node; cnt++, nm=nm->NEXT) ;
  if (nm!=NULL)
    {
      for (cl=realinterf, ch=phinterf, cnt0=0; cl && ch; ch=ch->NEXT, cl=cl->NEXT) 
        {
          if (ch->DATA==lc->NAME)
            {
              cnt0++;
              if (cnt0==cnt) break;
            }
        }

      if (cnt0==cnt && cl!=NULL) return (char *)cl->DATA;
    }
  return NULL;
}

static void spitransfertvcardnodes(locon_list *inst, locon_list *fig)
{
  ptype_list *pt;
  num_list *ninst, *nfig;
  chain_list *cl;
  vcardnodeinfo *vci;
  if ((pt=getptype(inst->SIG->USER, MBK_VCARD_NODES))!=NULL)
  {
    cl=(chain_list *)pt->DATA;
    while (cl!=NULL)
    {
      vci=(vcardnodeinfo *)cl->DATA;
      for (ninst=inst->PNODE, nfig=fig->PNODE; ninst!=NULL && nfig!=NULL && ninst->DATA!=vci->rcn; ninst=ninst->NEXT, nfig=nfig->NEXT) ;
      if (ninst!=NULL && nfig!=NULL)
        mbk_addvcardnode(fig->SIG, sub_getconnectorname((lofig_list *)fig->ROOT, fig, nfig->DATA), vci->value, nfig->DATA);
      cl=cl->NEXT;
    }
  }
}

static void spi_put_topfig_info_in_instance_fig (lofig_list * topfig)
{
 loins_list *li;
 locon_list *lc, *lci;
 lofig_list *lfi;
 float val, val1;
 ptype_list *pt, *pttop, *p;
 char buf[1024], *c;
 losig_list *ls;
 chain_list *cl;
 unsolved_vcard_type *uvc;
 float minus_value, value;
 int has_value, done=0, found;
 inffig_list *ifl;

 pttop=getptype(topfig->USER, SPICE_UNSOLVED_VCARDS);

 for (li = topfig->LOINS; li != NULL; li = li->NEXT) {
     lfi = getloadedlofig (li->FIGNAME);
     if (lfi != NULL) {
         for (lci = li->LOCON, lc = lfi->LOCON; lc != NULL && lci != NULL; lc = lc->NEXT, lci = lci->NEXT) {
             if (strcmp(getsigname(lci->SIG),"0")==0)
             {
                mbk_SetLosigVSS(lc->SIG);
                spitransfertvcardnodes(lci, lc);
             }
             else
             {
               if ((found=getlosigalim (lci->SIG, &val))!=0 && getlosigalim (lc->SIG, &val1) && fabs (val - val1) > 1e-3) {
                 avt_errmsg (SPI_ERRMSG, "068", AVT_ERROR, getsigname (lci->SIG), val, val1);
               }
               else
                 duplosigalim (lci->SIG, lc->SIG);
               if (found) spitransfertvcardnodes(lci, lc);
             }
         }
         
         if (pttop!=NULL)
         {
            int i0, i1;
            for (cl=(chain_list *)pttop->DATA; cl!=NULL; cl=cl->NEXT)
              {
                uvc=(unsolved_vcard_type *)cl->DATA;
                if (uvc!=NULL)
                {
                  if (uvc->s[0].name!=NULL && uvc->s[1].name==NULL) i0=0, i1=1;
                  else if (uvc->s[0].name==NULL && uvc->s[1].name!=NULL) i0=1, i1=0;
                  else i0=i1=0;

                  if (i0!=i1)
                  {
                    strcpy(buf, (SPICE_KEEP_CARDS & KEEP__INSTANCE)?uvc->s[i0].name:uvc->s[i0].name+1);
                    c=strchr(buf,'.');
                    *c='\0';
                    c++;
                    c=namealloc(c);
                    if (li->INSNAME==namealloc(buf))
                      for (ls=lfi->LOSIG; ls!=NULL && getsigname(ls)!=c; ls=ls->NEXT) ;
                    else
                      ls=NULL;
                    
                    has_value = getlosigalim (uvc->s[i1].u.sig, &minus_value);
                    if (mbk_LosigIsVSS (uvc->s[i1].u.sig) || has_value) 
                    {
                      if (!has_value) minus_value = 0.0;
                      value=uvc->value + minus_value;
                      if (ls!=NULL)
                      {
                        if (getlosigalim (ls, &val) && fabs (val - value) > 1e-3) {
                          avt_errmsg (SPI_ERRMSG, "068", AVT_ERROR, getsigname (ls), val, value);
                        }
                        else
                          addlosigalim (ls, value, uvc->expr);
                        if ((p=getptype(ls->USER, MBK_SIGNALNAME_NODE))!=NULL)
                          mbk_addvcardnode(ls, uvc->s[0].name, value, (long)p->DATA);
                      }
                      else
                      {
                        if ((ifl = getloadedinffig (li->FIGNAME)) == NULL)
                          ifl = addinffig (li->FIGNAME);
                        create_corresponding_hier_name((SPICE_KEEP_CARDS & KEEP__INSTANCE)?c:c+1, buf);
                        inf_AddDouble(ifl, INF_LOADED_LOCATION, mbk_decodeanyvector(buf), INF_POWER, value, NULL);
                      }
                      mbkfree(uvc);
                      cl->DATA=NULL;
                      done = 0;
                      
                    }
                  }
                }
              }
         }
         
         if ((pt=getptype(lfi->USER, SPICE_UNSOLVED_VCARDS))!=NULL)
         {
           while (!done) 
           {
             done = 1;
             for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
               {
                 uvc=(unsolved_vcard_type *)cl->DATA;
                 if (uvc!=NULL && uvc->s[0].name==NULL && uvc->s[1].name==NULL)
                 {
                   has_value = getlosigalim (uvc->s[1].u.sig, &minus_value);
                   if (mbk_LosigIsVSS (uvc->s[1].u.sig) || has_value) 
                   {
                       if (!has_value) minus_value = 0.0;
                       value=uvc->value + minus_value;
                       if (getlosigalim (uvc->s[0].u.sig, &val) && fabs (val - value) > 1e-3) {
                            avt_errmsg (SPI_ERRMSG, "068", AVT_ERROR, getsigname (uvc->s[0].u.sig), val, value);
                       }
                       else
                         addlosigalim (uvc->s[0].u.sig, value, uvc->expr);
                       mbk_addvcardnode(uvc->s[0].u.sig, uvc->s[0].nodename, value, uvc->s[0].rcn);
                       mbkfree(uvc);
                       cl->DATA=NULL;
                       done = 0;
                   }
                   else if (mbk_LosigIsVSS (uvc->s[0].u.sig) || has_value) 
                   {
                       if (!has_value) minus_value = 0.0;
                       value=uvc->value + minus_value;
                       if (getlosigalim (uvc->s[1].u.sig, &val) && fabs (val - value) > 1e-3) {
                            avt_errmsg (SPI_ERRMSG, "068", AVT_ERROR, getsigname (uvc->s[1].u.sig), val, value);
                       }
                       else
                        addlosigalim (uvc->s[1].u.sig, value, uvc->expr);
                       mbk_addvcardnode(uvc->s[1].u.sig, uvc->s[1].nodename, value, uvc->s[1].rcn);
                       mbkfree(uvc);
                       cl->DATA=NULL;
                       done = 0;
                   }
                 }
               }
           }
           
           for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
             {
               uvc=(unsolved_vcard_type *)cl->DATA;
               if (uvc!=NULL)
               {
                 // unsolved message
                 avt_errmsg (SPI_ERRMSG, "071", AVT_ERROR, 
                            uvc->s[0].name?uvc->s[0].name:getsigname (uvc->s[0].u.sig),
                            uvc->s[1].name?uvc->s[1].name:getsigname (uvc->s[1].u.sig),
                            uvc->value, li->FIGNAME);
                 mbkfree(uvc);
               }
             }

           freechain((chain_list *)pt->DATA);
           lfi->USER=delptype(lfi->USER, SPICE_UNSOLVED_VCARDS);
         }

     }
 }

 if (pttop!=NULL)
 {
   for (cl=(chain_list *)pttop->DATA; cl!=NULL; cl=cl->NEXT)
   {
     uvc=(unsolved_vcard_type *)cl->DATA;
     if (uvc!=NULL)
     {
       // unsolved message
       avt_errmsg (SPI_ERRMSG, "072", AVT_ERROR, 
                  uvc->s[0].name?uvc->s[0].name:getsigname (uvc->s[0].u.sig),
                  uvc->s[1].name?uvc->s[1].name:getsigname (uvc->s[1].u.sig),
                  uvc->value, topfig->NAME);
       mbkfree(uvc);
     }
   }
   freechain((chain_list *)pttop->DATA);
   topfig->USER=delptype(topfig->USER, SPICE_UNSOLVED_VCARDS);
 }

}

void spi_post_traitment_connector_direction (lofig_list * lf)
{
    chain_list *chainx, *cl;
    char typesig;
    char *dir;
    inffig_list *ifl;
    locon_list *lc;

    ifl = getloadedinffig (lf->NAME);

    for (lc = lf->LOCON; lc != NULL; lc = lc->NEXT)
        lc->FLAGS = 0;

    chainx = inf_GetEntriesByType (ifl, INF_CONNECTOR_DIRECTION, INF_ANY_VALUES);
    for (cl = chainx; cl != NULL; cl = cl->NEXT) {
        dir = "";
        inf_GetString (ifl, (char *)cl->DATA, INF_CONNECTOR_DIRECTION, &dir);
        switch (dir[0]) {
            case 'I': typesig = IN; break;
            case 'O': typesig = OUT; break;
            case 'Z': typesig = TRANSCV; break;
            case 'B': typesig = INOUT; break;
            case 'T': typesig = TRISTATE; break;
            case 'X': typesig = UNKNOWN; break;
            default: typesig = 0; break;
        }
        if (typesig != 0) {
            for (lc = lf->LOCON; lc != NULL; lc = lc->NEXT) {
                if (lc->FLAGS == 0 && mbk_TestREGEX (lc->NAME, (char *)cl->DATA))
                    lc->DIRECTION = typesig, lc->FLAGS = 1;
            }
        }
    }
    freechain (chainx);
}


/******************************************************************************/

void parsespice (name)
char *name;
{
    SPI_LIBRARY = 1;
    spiceloadlofig (NULL, name, 'A');
    SPI_LIBRARY = 0;
}

void spiceloadlofig (ptfig, name, mode)
lofig_list *ptfig;
char *name;
char mode;
{
    char *env;
    allinterf ALLINTERF_STRUCT, *ALLINTERF = &ALLINTERF_STRUCT;
    chain_list *ptchain;
    chain_list *cl, *ch;
    circuit *ptcir;
    char *spicename;
    char *extention;
    lofig_list *topfig, *lastlofig;
    ptype_list *ptl;
    ptype_list *exp;            // expression a evaluer a la fin (pour les param globaux)
    double valeur;
    mbk_options_pack_struct spice_opack[] = {
        {"KeepBBOXContent", SF_KEEP_BBOX},
        {"TransfertTopLevelVcards", SF_TRANSFERTTOP},
        {"ExplicitInstanceNames", SF_EXPLICITINSTANCENAMES}
    };
    mbk_options_pack_struct spice_device_opack[] = {
        {"Transistor", KEEP__TRANSISTOR},
        {"Resistance", KEEP__RESISTANCE},
        {"Instance", KEEP__INSTANCE},
        {"Capacitance", KEEP__CAPA},
        {"Diode", KEEP__DIODE},
        {"AllNodes", KEEP__ALLSIGNALS},
        {"None", KEEP__NONE},
        {"All", KEEP__ALL}
    };

    SPI_UNRESOLVED_CIRCUITS = NULL;
    SPI_LIBNAME = NULL;
    SPI_TOPCIR = NULL;

    /* Comportement par defaut */

    SPI_SEPAR = '.';
    SPI_NETNAME = "NET";
    SPI_MERGE = 1;
    SPI_VERBOSE = 0;
    SPI_ONE_NODE_NORC = 0;
    SPI_CHECK_NAME = 0;
    SPI_AUTO_LOAD = 0;
    SPI_PARSE_FIRSTLINE = PARSEFIRSTLINE_NO;
    SPI_CREATE_TOP_FIGURE = 1;
    SPI_IGNORE_MODELS = 0;
    SPI_IGNORE_DIODES = 0;
    SPI_IGNORE_CAPA = 0;
    SPI_MIN_CAPA = 0.0;
    SPI_IGNORE_RESI = 0;
    SPI_IGNORE_VOLTAGE = 0;
    SPI_ZEROVOLT_RESI = 0;
    SPI_MERGE_DIODES = 0;
    SPI_JFET_RESI = 0;
    SPI_INSTANCE_MULTINODE = 1;
    SPI_NO_SIG_PREFIX = 1;
    SPI_MIN_RESI = 0.0;
    SPI_MAX_RESI = 0.0;
    SPI_NORCCAPA_TOGND = 0;
    SPI_SCALE_CAPAFACTOR = 1.0;
    SPI_SCALE_RESIFACTOR = 1.0;
    SPI_SCALE_TRANSFACTOR = 1.0;
    SPI_SCALE_DIODEFACTOR = 1.0;
    SPI_USE_PRELOADED_LIB = 'Y';
    SPI_DEBUGPARSE = 'N';
    SPI_GLOBAL_TEMP = -DBL_MAX;
    SPI_GLOBAL_SCALE = 1;
    SPI_GLOBAL_NODES = NULL;
    SPI_TOLERANCE = 0;
    SPI_PARSED_LOFIG = NULL;
    SPI_LIBNAME=NULL;

    if (V_INT_TAB[__SIM_TOOLMODEL].VALUE==SIM_TOOLMODEL_ELDO)
       SPI_COM_CHAR='!';
    else
       SPI_COM_CHAR='$';

    SPI_RC_MEMORY_LIMIT = V_INT_TAB[__SPI_RC_MEMORY_LIMIT].VALUE;

    SPI_DRIVE_SPEF_MODE = V_BOOL_TAB[__SPI_DRIVE_PARASITICS].VALUE;

    env = V_STR_TAB[__MBK_SPI_TOLERANCE].VALUE;

    if (env) {
        if (!strcmp (env, "low")) SPI_TOLERANCE = 0;
        if (!strcmp (env, "medium")) SPI_TOLERANCE = 1;
        if (!strcmp (env, "high")) SPI_TOLERANCE = 2;
    }

    SPI_NO_SIG_PREFIX = V_BOOL_TAB[__MBK_SPI_NO_SIG_PREFIX].VALUE;

    env = V_STR_TAB[__MBK_SPI_PARSE_FIRSTLINE].VALUE;

    if (env) {
        if (strcmp (env, "yes") == 0) SPI_PARSE_FIRSTLINE = PARSEFIRSTLINE_YES;
        if (strcmp (env, "no") == 0) SPI_PARSE_FIRSTLINE = PARSEFIRSTLINE_NO;
        if (strcmp (env, "include") == 0) SPI_PARSE_FIRSTLINE = PARSEFIRSTLINE_INC;
    }

    SPI_CREATE_TOP_FIGURE = V_BOOL_TAB[__MBK_SPI_CREATE_TOP_FIGURE].VALUE;

    SPI_NETNAME = V_STR_TAB[__MBK_SPI_NETNAME].VALUE;

    env = V_STR_TAB[__MBK_SPI_SUFFIX].VALUE;

    if (env) SPI_SUFFIX = env;
    else SPI_SUFFIX = IN_LO;

    SPI_IGNORE_BULK = V_BOOL_TAB[__MBK_SPI_NOBULK].VALUE?'Y':'N';

    env = V_STR_TAB[__MBK_SPI_SEPAR].VALUE;

    if (env) {
        switch (strlen (env)) {
        case 0:
            SPI_SEPAR = 0;
            SPI_MERGE = 0;
            break;
        case 1:
            SPI_SEPAR = *env;
            break;
        default:
            avt_errmsg (SPI_ERRMSG, "007", AVT_FATAL);
            break;
        }
    }

    env = getenv ("MBK_SPI_CHECK_NAME");
    if (env)
        if (strcmp (env, "yes") == 0) SPI_CHECK_NAME = 1;

    SPI_AUTO_LOAD = V_BOOL_TAB[__MBK_SPI_AUTO_LOAD].VALUE;

    SPI_IGNORE_MODELS = V_BOOL_TAB[__MBK_SPI_IGNORE_MODELS].VALUE;

    SPI_IGNORE_DIODES = V_BOOL_TAB[__MBK_SPI_IGNORE_DIODES].VALUE;

    SPI_IGNORE_CAPA = V_BOOL_TAB[__MBK_SPI_IGNORE_CAPA].VALUE;

    SPI_MIN_CAPA = V_FLOAT_TAB[__MBK_SPI_MIN_CAPA].VALUE;

    SPI_IGNORE_RESI = V_BOOL_TAB[__MBK_SPI_IGNORE_RESI].VALUE;

    SPI_MAX_RESI = V_FLOAT_TAB[__MBK_SPI_MAX_RESI].VALUE;

    SPI_MIN_RESI = V_FLOAT_TAB[__MBK_SPI_MIN_RESI].VALUE;

    SPI_IGNORE_VOLTAGE = V_BOOL_TAB[__MBK_SPI_IGNORE_VOLTAGE].VALUE;

    SPI_ZEROVOLT_RESI = V_BOOL_TAB[__MBK_SPI_ZEROVOLT_RESI].VALUE;

    SPI_MERGE_DIODES = V_BOOL_TAB[__MBK_SPI_MERGE_DIODES].VALUE;

    SPI_JFET_RESI = V_BOOL_TAB[__MBK_SPI_JFET_RESI].VALUE;

    SPI_INSTANCE_MULTINODE = V_BOOL_TAB[__MBK_SPI_INSTANCE_MULTINODE].VALUE;

    env = getenv ("MBK_SPI_NORCCAPA_TOGND");
    if (env)
        if (strcmp (env, "yes") == 0) SPI_NORCCAPA_TOGND = 1;

    SPI_SCALE_CAPAFACTOR = V_FLOAT_TAB[__MBK_SPI_SCALE_CAPAFACTOR].VALUE;

    SPI_SCALE_RESIFACTOR = V_FLOAT_TAB[__MBK_SPI_SCALE_RESIFACTOR].VALUE;

    SPI_SCALE_TRANSFACTOR = V_FLOAT_TAB[__MBK_SPI_SCALE_TRANSFACTOR].VALUE;

    SPI_SCALE_DIODEFACTOR = V_FLOAT_TAB[__MBK_SPI_SCALE_DIODEFACTOR].VALUE;

    /* Variables d'environnement cachees */

    /* MBK_SPI_MERGE. Si positionnee, on n'effectue pas
     * de regroupement de locon sur les noms toto, toto.1, toto.2, toto.3...
     */

    SPI_MERGE = V_BOOL_TAB[__MBK_SPI_MERGE].VALUE;

    env = getenv ("MBK_SPI_VERBOSE");
    if (env)
        if (strcmp (env, "yes") == 0) SPI_VERBOSE = 1;

    SPI_ONE_NODE_NORC = V_BOOL_TAB[__MBK_SPI_ONE_NODE_NORC].VALUE;

    env = getenv ("SPI_USE_PRELOADED_LIB");
    if (env) {
        if (strcmp (env, "yes") == 0) SPI_USE_PRELOADED_LIB = 'Y';
        else SPI_USE_PRELOADED_LIB = 'N';
    }

    env = getenv ("SPI_DEBUGPARSE");

    if (env) {
        if (strcmp (env, "yes") == 0) SPI_DEBUGPARSE = 'Y';
        else SPI_DEBUGPARSE = 'N';
    }

    SPICE_SF = mbk_ReadFlags (__MBK_SPICE_DRIVER_FLAGS, spice_opack, sizeof (spice_opack) / sizeof (*spice_opack), 0, SF_TRANSFERTTOP);

    env = getenv ("SPI_TRANSFERT_TOPFIG_INFO");
    if (env && strcasecmp (env, "yes") == 0) SPICE_SF |= SF_TRANSFERTTOP;

    SPICE_KEEP_NAMES =
        mbk_ReadFlags (__MBK_SPICE_KEEP_NAMES, spice_device_opack,
                       sizeof (spice_device_opack) / sizeof (*spice_device_opack), 1, KEEP__TRANSISTOR);
    if (SPICE_KEEP_NAMES & KEEP__NONE)
        SPICE_KEEP_NAMES = 0;

    SPICE_KEEP_CARDS =
        mbk_ReadFlags (__MBK_SPICE_KEEP_CARDS, spice_device_opack,
                       sizeof (spice_device_opack) / sizeof (*spice_device_opack), 1, 0);
    if (SPICE_KEEP_CARDS & KEEP__NONE) SPICE_KEEP_CARDS = 0;

    if (SPI_VERBOSE) {
        printf ("Parser Spice compile le %s a %s\n", __DATE__, __TIME__);
        printf ("Revision     : %s\n", "$Revision: 1.448 $");
        printf ("Date         : %s\n", "$Date: 2010/06/03 16:02:21 $");
        printf ("Transistor N : %s\n", (char *)TNMOS->DATA);
        printf ("Transistor P : %s\n", (char *)TPMOS->DATA);
        printf ("Separateur   : '%c'\n", SPI_SEPAR);
        printf ("Nom de noeud : %s\n", SPI_NETNAME);
        printf ("Regroupement : %s\n", SPI_MERGE ? "Oui" : "Non");
        printf ("Noms         : %s\n", SPI_CHECK_NAME ? "Oui" : "Non");
        printf ("Pre-load     : %c\n", SPI_USE_PRELOADED_LIB);
        printf ("Ne cree pas les vues RC sur les reseaux ne contenant qu'un seul noeud : %s\n", SPI_ONE_NODE_NORC ? "Oui" : "Non");
    }

    if (mode != 'A') {
        avt_errmsg (SPI_ERRMSG, "008", AVT_FATAL, mode);
    }

    spicename = (char *)mbkalloc (sizeof (char) * (strlen (name) + 1));
    sprintf (spicename, "%s", name);

    msl_figname = name;
    msl_ptfig = NULL;

    ALLINTERF = spi_initpreloadedlib (ALLINTERF);
    lastlofig = HEAD_LOFIG;

    GLOBAL_CTX = spi_init_eqt ();    // with .PARAM

    eqt_addvar (GLOBAL_CTX, namealloc ("temper"), V_FLOAT_TAB[__SIM_TEMP].VALUE);

    if (SPI_CREATE_TOP_FIGURE)
        SPI_TOPCIR = spi_create_circuit (spicename);

    if (SPI_LIBRARY == 0) {
        ALLINTERF = spiceloading (ptfig, spicename, SPI_SUFFIX, mode, ALLINTERF);
    }
    else {
        extention = spi_getextention (spicename);
        ALLINTERF = spiceloading (ptfig, spicename, extention, mode, ALLINTERF);
    }

    if (SPI_UNRESOLVED_CIRCUITS && SPI_DRIVE_SPEF_MODE)
        fprintf (stderr, "spice parser: unresolved circuits found, spef generation disabled for them\n");

    /* Try to solve the rest using ICNAME field */
    for (ptchain = SPI_UNRESOLVED_CIRCUITS; ptchain; ptchain = ptchain->NEXT) {
        ptcir = (circuit *) ptchain->DATA;
        ALLINTERF = spi_resolvecircuit (ptfig, ptcir, ALLINTERF, mode);
        liberecircuit (ptcir);
    }

    /* resolve created top circuit */
    if (SPI_CREATE_TOP_FIGURE && !spi_empty_circuit (SPI_TOPCIR)) {
        if (spi_findfigure (SPI_PARSED_LOFIG, SPI_TOPCIR->NOM)) {
            // gestion d'un eventuel conflit de nom
            char buf[1024];
            sprintf (buf, "top_%s", SPI_TOPCIR->NOM);
            SPI_TOPCIR->NOM = (char *)spiciralloue (SPI_TOPCIR, sizeof (char) * (strlen (buf) + 1));
            strcpy (SPI_TOPCIR->NOM, buf);
        }
        if (ptfig == NULL)
            ptfig = addlofig (SPI_TOPCIR->NOM);
        ALLINTERF = spi_resolvecircuit (ptfig, SPI_TOPCIR, ALLINTERF, mode);
        if (namealloc (SPI_TOPCIR->NOM) == ptfig->NAME)
            topfig = ptfig;
        else
            topfig = getloadedlofig (SPI_TOPCIR->NOM);
        spi_create_top_interface (topfig);
        ptl = getptype (ptfig->USER, PH_INTERF);
        if (ptl) {
            freechain ((chain_list *) ptl->DATA);
            ptfig->USER = delptype (ptfig->USER, PH_INTERF);
        }
        ptl = getptype (ptfig->USER, PH_REAL_INTERF);
        if (ptl) {
            freechain ((chain_list *) ptl->DATA);
            ptfig->USER = delptype (ptfig->USER, PH_REAL_INTERF);
        }

    }
    else
        topfig = NULL;

    if (topfig != NULL)
        SPI_PARSED_LOFIG = addchain (SPI_PARSED_LOFIG, topfig);

    if (SPI_TOPCIR != NULL)
    {
       inf_renameinffig(TOPCIRINFNAME, SPI_TOPCIR->NOM);
       liberecircuit (SPI_TOPCIR);
    }
    SPI_TOPCIR = NULL;

    spi_updatelofiginfo (topfig, SPI_PARSED_LOFIG, SPI_GLOBAL_NODES);
    if (topfig != NULL && (SPICE_SF & SF_TRANSFERTTOP) != 0)
        spi_put_topfig_info_in_instance_fig (topfig);
    spi_savepreloadedlib (ALLINTERF);

    mbkfree (spicename);
    freeptype(SPI_LIBNAME);
    

    while (1) {
        int ok;

        ok = 0;

        for (exp = UNRESOLVED_EXP; exp; exp = exp->NEXT) {
            if (exp->DATA == NULL)
                continue;

            valeur = spi_eval (NULL, GLOBAL_CTX, (char *)(exp->DATA), NULL, 0, 0);
            if (eqt_resistrue (GLOBAL_CTX)) {
                eqt_addvar (GLOBAL_CTX, (char *)exp->TYPE, valeur);
                exp->DATA = NULL;
                ok = 1;
            }
        }
        if (ok == 0)
            break;
    }
    for (exp = UNRESOLVED_EXP; exp; exp = exp->NEXT) {

        if (exp->DATA != NULL) {
            fprintf (stderr, "[SPI ERR] Can't evaluate");
            cl = eqt_GetVariables (GLOBAL_CTX, (char *)exp->DATA, 0);
            for (ch = cl; ch != NULL; ch = ch->NEXT) {
                fprintf (stderr, "%s '%s'", ch == cl ? "" : ch->NEXT == NULL ? " and" : ",", (char *)ch->DATA);
//                eqt_addvar (GLOBAL_CTX, (char *)ch->DATA, 0);
                fprintf (stderr, ", assuming 0");
            }
            freechain (cl);
            fprintf (stderr, " for '%s=%s'\n", (char *)exp->TYPE, (char *)exp->DATA);
            valeur = eqt_eval (GLOBAL_CTX, namealloc((char *)exp->DATA), EQTFAST);
            eqt_addvar (GLOBAL_CTX, (char *)exp->TYPE, valeur);
        }
    }

    if (UNRESOLVED_EXP) {
        freeptype (UNRESOLVED_EXP);
        UNRESOLVED_EXP = NULL;
    }

    if (MBK_GLOBALPARAMS) {
        eqt_import_vars (GLOBAL_CTX, MBK_GLOBALPARAMS);
        eqt_free_param (MBK_GLOBALPARAMS);
    }
    MBK_GLOBALPARAMS = eqt_export_vars (GLOBAL_CTX);
    MBK_GLOBALFUNC = eqt_export_func(MBK_GLOBALFUNC, GLOBAL_CTX);
    eqt_term (GLOBAL_CTX);
    freechain (SPI_GLOBAL_NODES);
    freechain (SPI_PARSED_LOFIG);

    mbk_commit_errors (name);
}

/******************************************************************************/

spifile *spifileopen (name, extension, parsefirstline)
char *name;
char *extension;
char parsefirstline;
{
    spifile *pt;
    int i;

    pt = mbkalloc (sizeof (spifile));
    pt->df = mbkfopen (name, extension, READ_TEXT);
    if (!pt->df)
        pt->df = mbkfopen (name, NULL, READ_TEXT);
    pt->savedf = NULL;
    pt->encryptedlines = 0;
    pt->encrypted = 0;
    pt->numlines = 0;
    pt->linetablesize = 4;
    pt->lines = (spiline *) mbkalloc (pt->linetablesize * sizeof (spiline));
    for (i = 0; i < pt->linetablesize; i++) {
        pt->lines[i].decomp = NULL;
        pt->lines[i].decompalloc = NULL;
        pt->lines[i].chaindecomp = NULL;
        pt->lines[i].ptdecompalloc = NULL;
        pt->lines[i].decompfree = 0;
        pt->lines[i].linenum = 0;
    }
    pt->filename = name;
    pt->linenum = 0;
    pt->msl_line = -1;
    *pt->file_line = '\0';
    pt->parsefirstline = parsefirstline;


    if (!pt->df) 
        avt_errmsg (SPI_ERRMSG, "009", AVT_FATAL, name);

    avt_set_encrypted_mode(0);
    return (pt);
}

/******************************************************************************/

void spifileclose (pt)
spifile *pt;
{
    int i;

    fclose (pt->df);
    /* zinaps : 12/5/2003, on peut arreter le parse au milieu du fichier => spi2spef.c
       if( pt->decomp1 || pt->decomp2 )
       {
       fflush( stdout );
       fprintf( stderr, "%s\n", SPIMSG(0) );
       fprintf( stderr, "%s.\n", SPIMSG(10) );
       EXIT(1);
       }
     */
    for (i = 0; i < pt->linetablesize; i++) {
        freedecompligne (pt, i);
        if (pt->lines[i].decompalloc != NULL)
            mbkfree (pt->lines[i].decompalloc);
    }
    mbkfree (pt->lines);
    mbkfree (pt);
    avt_set_encrypted_mode(0);
}

/******************************************************************************/

allinterf *spiceloading (lofig_list * ptfig, char *name, char *extension, char mode, allinterf * ALLINTERF)
{
    circuit *ptcir;
    spifile *df;
    chain_list *ptunresolved, *ptdelete;
    int changes;
    int activate;
    chain_list *all_cirs_in_file = NULL;
    chain_list *fifodf, *scandf;
    spi_load_global globalinfo;

    df = spifileopen (name, extension, (SPI_PARSE_FIRSTLINE == PARSEFIRSTLINE_YES) ? 1 : 0);
    fifodf = addchain (NULL, df);

    if (SPI_LIBNAME != NULL)
        activate = 0;
    else
        activate = 1;
    globalinfo.blackboxed = 0;
    globalinfo.ptcir_stack = NULL;

    while ((ptcir = lirecircuit (&fifodf, ALLINTERF, ptfig, &activate, &globalinfo))) {
        if (spi_canresolve (ptcir, ALLINTERF) == FALSE) {
            SPI_UNRESOLVED_CIRCUITS = addchain (SPI_UNRESOLVED_CIRCUITS, ptcir);
            continue;
        }
        else {
            ALLINTERF = spi_resolvecircuit (ptfig, ptcir, ALLINTERF, mode);
            if (SPI_DRIVE_SPEF_MODE)
                all_cirs_in_file = addchain (all_cirs_in_file, ptcir);
            else
                liberecircuit (ptcir);
        }

        /* Resolve as much as possible */
        ptunresolved = SPI_UNRESOLVED_CIRCUITS;
        changes = FALSE;

        while ((ptunresolved || changes) && (SPI_UNRESOLVED_CIRCUITS != NULL)) {
            /* back to the beginning */
            if (ptunresolved == NULL) {
                ptunresolved = SPI_UNRESOLVED_CIRCUITS;
                changes = FALSE;
            }

            ptcir = (circuit *) ptunresolved->DATA;
            if (spi_canresolve (ptcir, ALLINTERF) == FALSE) {
                ptunresolved = ptunresolved->NEXT;
                continue;
            }
            ALLINTERF = spi_resolvecircuit (ptfig, ptcir, ALLINTERF, mode);
            if (SPI_DRIVE_SPEF_MODE)
                all_cirs_in_file = addchain (all_cirs_in_file, ptcir);
            else
                liberecircuit (ptcir);

            ptdelete = ptunresolved;
            ptunresolved = ptunresolved->NEXT;
            SPI_UNRESOLVED_CIRCUITS = delchain (SPI_UNRESOLVED_CIRCUITS, ptdelete);
            changes = TRUE;
        }
    }

    // --------- zinaps: 27/9/2004 --------------
    // dans le cas ou les .model sont en fin de fichier hors de subckt
    /* Resolve as much as possible */
    ptunresolved = SPI_UNRESOLVED_CIRCUITS;
    changes = FALSE;

    while ((ptunresolved || changes) && (SPI_UNRESOLVED_CIRCUITS != NULL)) {
        /* back to the beginning */
        if (ptunresolved == NULL) {
            ptunresolved = SPI_UNRESOLVED_CIRCUITS;
            changes = FALSE;
        }

        ptcir = (circuit *) ptunresolved->DATA;
        if (spi_canresolve (ptcir, ALLINTERF) == FALSE) {
            ptunresolved = ptunresolved->NEXT;
            continue;
        }
        ALLINTERF = spi_resolvecircuit (ptfig, ptcir, ALLINTERF, mode);
        if (SPI_DRIVE_SPEF_MODE)
            all_cirs_in_file = addchain (all_cirs_in_file, ptcir);
        else
            liberecircuit (ptcir);

        ptdelete = ptunresolved;
        ptunresolved = ptunresolved->NEXT;
        SPI_UNRESOLVED_CIRCUITS = delchain (SPI_UNRESOLVED_CIRCUITS, ptdelete);
        changes = TRUE;
    }
    // -----------------------------------------

    if (cmp_handler_list) {
        msl_figloaded = FALSE;
        msl_subckt = NULL;
        cmp_handlers ();
    }

    for (scandf = fifodf; scandf; scandf = scandf->NEXT) {
        spifileclose ((spifile *) scandf->DATA);
    }
    freechain (fifodf);

    if (SPI_DRIVE_SPEF_MODE) {
        chain_list *cl;
        SpiceToSpef (name, extension, all_cirs_in_file, SPI_RC_MEMORY_LIMIT);
        // liberation...
        for (cl = all_cirs_in_file; cl != NULL; cl = cl->NEXT) {
            liberecircuit ((circuit *) cl->DATA);
        }
        freechain (all_cirs_in_file);
    }

    return ALLINTERF;
}

/******************************************************************************/

allinterf *spi_resolvecircuit (lofig_list * ptfig, circuit * ptcir, allinterf * ALLINTERF, char mode)
{
    lofig_list *newfig;
    locon_list *scanlocon;

//    mbk_debugstat(NULL,1);
//    fprintf(stderr, "all: %s",ptcir->NOM); fflush(stderr);
    ALLINTERF = constequi (ptcir, ALLINTERF);
    ALLINTERF = constinterf (ptcir, ALLINTERF);

    if (ptfig && strcasecmp (ptcir->NOM, ptfig->NAME) == 0) {
        /* top figure */
        int saveSPICE_KEEP_NAMES=SPICE_KEEP_NAMES;
        SPICE_KEEP_NAMES|=KEEP__ALLSIGNALS;
        ALLINTERF = constlofig (ptcir, ptfig, ALLINTERF, mode);
        msl_ptfig = ptfig;
        ptfig->USER = constphinterf (ptfig->USER, ALLINTERF->teteinterf, 1);
        /* change name of top circuit to avoid collision */
        if (SPI_TOPCIR != NULL && ptcir != SPI_TOPCIR) {
            SPI_TOPCIR->NOM = (char *)spiciralloue (SPI_TOPCIR, sizeof (char) * (strlen (ptfig->NAME) + 5));
            strcpy (SPI_TOPCIR->NOM, "top_");
            strcat (SPI_TOPCIR->NOM, ptfig->NAME);
        }
        SPICE_KEEP_NAMES=saveSPICE_KEEP_NAMES;
    }
    else {
        /* any other figure */
        newfig = getloadedlofig (ptcir->NOM);
        if (!newfig) {
            newfig = addlofig (ptcir->NOM);
            if (SPI_LIBRARY == 1)
                locklofig (newfig);
            SPI_PARSED_LOFIG = addchain (SPI_PARSED_LOFIG, newfig);
        }
        else {
            /* existing figure : delete all except interface */
            losig_list *scanlosig, *nextlosig;
            while (newfig->LOTRS)
                dellotrs (newfig, newfig->LOTRS);
            while (newfig->LOINS)
                delloins (newfig, newfig->LOINS->INSNAME);
            lofigchain (newfig);
            for (scanlosig = newfig->LOSIG; scanlosig; scanlosig = nextlosig) {
                nextlosig = scanlosig->NEXT;
                if (scanlosig->TYPE == INTERNAL)
                    dellosig (newfig, scanlosig->INDEX);
            }
            for (scanlocon = newfig->LOCON; scanlocon; scanlocon = scanlocon->NEXT) {
                if (scanlocon->SIG->PRCN)
                    freelorcnet (scanlocon->SIG);
            }
        }

        ALLINTERF = constlofig (ptcir, newfig, ALLINTERF, mode);
        msl_ptfig = newfig;
        newfig->USER = constphinterf (newfig->USER, ALLINTERF->teteinterf, 1);
        SPI_PARSED_LOFIG = addchain (SPI_PARSED_LOFIG, newfig);
    }

    if (SPI_VERBOSE)
        printf ("Taille memoire occupee par le subckt %s : %ld\n", ptcir->NOM, ptcir->TAILLE);

    return ALLINTERF;
}

/******************************************************************************/

ptype_list *constphinterf (ptype_list * pt, ginterf * teteinterf, int realinterftag)
{
    typedef struct {
        char *nom;
        char *realname;
    } tempstuct;
    chain_list *tete_ph_interf = NULL, *realinterf = NULL;
    chain_list *scan, *next;
    char *nomdevec;
    tempstuct *tabnom;
    int n, in, jn;
    ptype_list *ptl;

    tete_ph_interf = NULL;

    if ((ptl = getptype (pt, PH_INTERF)) != NULL) {
        freechain ((chain_list *) ptl->DATA);
        pt = delptype (pt, PH_INTERF);
    }
    if ((ptl = getptype (pt, PH_REAL_INTERF)) != NULL) {
        freechain ((chain_list *) ptl->DATA);
        pt = delptype (pt, PH_REAL_INTERF);
    }

    for (scan = teteinterf->GINTERF, n = 0; scan; scan = scan->NEXT, n++);

    tabnom = (tempstuct *) mbkalloc (sizeof (tempstuct) * n);

    if (n > 0) {
        for (scan = teteinterf->GINTERF, in = 0; scan; scan = scan->NEXT, in++) {
            nomdevec = spi_devect ((char *)(scan->DATA));
            tabnom[in].nom = nomdevec;
        }
        if (realinterftag) {
          for (scan = teteinterf->CORRESPINTERFNAME, in = 0; scan; scan = scan->NEXT, in++) {
              tabnom[in].realname = (char *)scan->DATA;
          }
        }

        for (scan = teteinterf->DUPCON, in = 0; scan; scan = scan->NEXT, in++) {
            if (scan->DATA) {
                for (next = scan->NEXT, jn = in + 1; next != scan->DATA; next = next->NEXT, jn++);
                tabnom[jn].nom = NULL;
            }
        }

        for (in = 0; in < n; in++) {
            if (tabnom[in].nom) {
                tete_ph_interf = addchain (tete_ph_interf, tabnom[in].nom);
                if (realinterftag && teteinterf->CORRESPINTERFNAME)
                    realinterf = addchain (realinterf, tabnom[in].realname);
            }
        }
    }

    mbkfree (tabnom);

    if (realinterftag && teteinterf->CORRESPINTERFNAME)
        pt = addptype (pt, PH_REAL_INTERF, reverse (realinterf));
    return (addptype (pt, PH_INTERF, reverse (tete_ph_interf)));
}

/******************************************************************************/

void checkinstmodel (ptcir, instance, interf)
circuit *ptcir;
inst *instance;
ginterf *interf;
{
    chain_list *chaini;
    chain_list *chainm;
    noeud *ptnoeud;
    icname *pticname;
    char nom[1024];
    char orderdspf = 'u';
    char orderpower = 'u';
    chain_list *newhead;
    chain_list *prev;
    chain_list *chain;
    char *env;
    char findvss, findvdd;

    orderdspf = V_BOOL_TAB[__SPI_PIN_DSPF_ORDER].VALUE?'y':'n';
    orderpower = V_BOOL_TAB[__SPI_ORDER_PIN_POWER].VALUE?'y':'n';

    for (chaini = instance->IINTERF, chainm = interf->GINTERF;
         chaini != NULL && chainm != NULL; chaini = chaini->NEXT, chainm = chainm->NEXT);

    if ((chaini == NULL) && (chainm == NULL)) {
        for (chaini = instance->IINTERF, chainm = interf->GINTERF;
             chaini != NULL && chainm != NULL; chaini = chaini->NEXT, chainm = chainm->NEXT) {
            ptnoeud = (noeud *) chaini->DATA;
            if (ptnoeud->SUPINFO == NULL || ptnoeud->SUPINFO->ICNAME == NULL)
                continue;
            for (pticname = ptnoeud->SUPINFO->ICNAME; pticname; pticname = pticname->SUIV) {
                if (strcasecmp (pticname->INSTANCE, instance->NOM) == 0)
                    break;
            }
            if (pticname == NULL)
                continue;

            if (strlen ((char *)chainm->DATA) == 0) {
                strcpy (nom, pticname->NOM);

                if (SPI_MERGE)
                    stopchainsepar (nom);

                chainm->DATA = mbkstrdup (nom);
            }
        }
    }

    if (orderdspf == 'y') {

        newhead = NULL;

        /* Changement de l'ordre d'instantiation.

           exemple : 
           dans le fichier, on a :

           .subckt VSS VDD A B inv
           ...
           .ends

           x1 X Y Z T inv
           *|I X inv B
           *|I Y inv A

           en mémoire, on a :

           GINTERF = VSS VDD A   B    <- c'est l'ordre à respecter.
           IINTERF = X   Y   Z   T    <- ordre d'instanciation 

           les icname nous indiquent : 

           X doit être connecté sur B
           Y doit être connecté sur A

           Crée une liste chainée des noeuds dont les informations icname correspondent
           à celle du modèle. Si aucun ne correspond, un NULL est inséré dans la chaine.

           newhead vaudra : null null Y X
         */

        for (chainm = interf->GINTERF; chainm; chainm = chainm->NEXT) {

            prev = NULL;

            for (chaini = instance->IINTERF; chaini; chaini = chaini->NEXT) {

                ptnoeud = (noeud *) chaini->DATA;

                if (ptnoeud->SUPINFO && ptnoeud->SUPINFO->ICNAME) {

                    for (pticname = ptnoeud->SUPINFO->ICNAME; pticname; pticname = pticname->SUIV) {

                        if (strcasecmp (pticname->INSTANCE, instance->NOM) == 0 &&
                            strcasecmp (pticname->NOM, (char *)chainm->DATA) == 0)
                            break;
                    }

                    if (pticname != NULL)
                        break;
                }

                prev = chaini;
            }

            if (chaini) {
                newhead = addchain (newhead, ptnoeud);
                if (prev)
                    prev->NEXT = chaini->NEXT;
                else
                    instance->IINTERF = instance->IINTERF->NEXT;
                chaini->NEXT = NULL;
                freechain (chaini);
            }
            else
                newhead = addchain (newhead, NULL);
        }

        newhead = reverse (newhead);

        /* Une seconde passe est effectuée pour remplir les éléments qu'on a pas eu 
           durant la boucle précédente, en conservant l'ordre.

           newhead vaudra : Z T Y X.
         */

        chaini = instance->IINTERF;
        for (chainm = newhead; chainm; chainm = chainm->NEXT) {
            if (!chainm->DATA) {
                chainm->DATA = chaini->DATA;
                chaini = chaini->NEXT;
            }
        }

        freechain (instance->IINTERF);
        instance->IINTERF = newhead;
    }

    if (orderpower == 'y') {

        newhead = NULL;

        for (chainm = interf->GINTERF, chaini = instance->IINTERF; chainm; chainm = chainm->NEXT, chaini = chaini->NEXT) {

            prev = NULL;

            if (isvdd ((char *)chainm->DATA) && isglobalvdd ((char *)chainm->DATA))
                findvdd = 'y';
            else
                findvdd = 'n';

            if (isvss ((char *)chainm->DATA) && isglobalvss ((char *)chainm->DATA))
                findvss = 'y';
            else
                findvss = 'n';

            if (findvdd == 'n' && findvss == 'n') {

                newhead = addchain (newhead, (noeud *) chaini->DATA);

            }
            else {

                for (chain = instance->IINTERF; chain; chain = chain->NEXT) {

                    ptnoeud = (noeud *) chain->DATA;

                    if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom) == 0) {

                        if ((findvdd == 'y' && isvdd (nom)) || (findvss == 'y' && isvss (nom))) {
                            newhead = addchain (newhead, ptnoeud);
                            break;
                        }
                    }
                }

                if (!chain)

                    newhead = addchain (newhead, (noeud *) chaini->DATA);
            }
        }

        newhead = reverse (newhead);
        freechain (instance->IINTERF);
        instance->IINTERF = newhead;
    }
}

/******************************************************************************/

allinterf *constinstmodel (ptcir, instance, ALLINTERF)
circuit *ptcir;
inst *instance;
allinterf *ALLINTERF;
{
    ginterf *ninterf;
    noeud *ptnoeud;
    chain_list *chain;
    char nom[1024];
    char buf[1024];
    icname *pticname;
    char findpower = 'u';
    char thereisvdd = 'n';
    char thereisvss = 'n';
    char *env;

    findpower = V_BOOL_TAB[__SPI_DSPF_BUILD_POWER].VALUE?'y':'n';

    for (chain = instance->IINTERF; chain; chain = chain->NEXT) {
        ptnoeud = (noeud *) chain->DATA;
        if (ptnoeud->SUPINFO != NULL) {
            for (pticname = ptnoeud->SUPINFO->ICNAME; pticname; pticname = pticname->SUIV) {
                if (strcasecmp (pticname->INSTANCE, instance->NOM) == 0)
                    break;
            }
            if (pticname != NULL)
                break;
        }
    }

    if (chain == NULL)
        return (NULL);

    ninterf = (ginterf *) mbkalloc (sizeof (ginterf));
    ninterf->NOM = mbkstrdup (instance->MODELE);

    ninterf->SUIV = ALLINTERF->teteinterf;
    ALLINTERF->teteinterf = ninterf;

    addhtitem (ALLINTERF->h, namealloc (ninterf->NOM), (long)ninterf);

    ninterf->GINTERF = NULL;
    ninterf->DUPCON = NULL;
    ninterf->CORRESPINTERFNAME = NULL;

    if (findpower == 'y') {

        for (chain = instance->IINTERF; chain; chain = chain->NEXT) {
            ptnoeud = (noeud *) chain->DATA;
            if (ptnoeud->SUPINFO != NULL) {
                for (pticname = ptnoeud->SUPINFO->ICNAME; pticname; pticname = pticname->SUIV) {
                    if (strcasecmp (pticname->INSTANCE, instance->NOM) == 0)
                        break;
                }
            }
            else
                pticname = NULL;
            if (pticname && pticname->NOM) {
                if (isvdd (pticname->NOM) || isglobalvdd (pticname->NOM))
                    thereisvdd = 'y';
                if (isvss (pticname->NOM) || isglobalvss (pticname->NOM))
                    thereisvss = 'y';
            }
        }
    }

    for (chain = instance->IINTERF; chain; chain = chain->NEXT) {
        ptnoeud = (noeud *) chain->DATA;
        if (ptnoeud->SUPINFO != NULL) {
            for (pticname = ptnoeud->SUPINFO->ICNAME; pticname; pticname = pticname->SUIV) {
                if (strcasecmp (pticname->INSTANCE, instance->NOM) == 0)
                    break;
            }
        }
        else
            pticname = NULL;
        if (pticname != NULL)
            strcpy (nom, pticname->NOM);
        else {
            strcpy (nom, "/0");

            if (findpower == 'y') {
                if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, buf) == 0) {
                    if (thereisvdd == 'n') {
                        if (isvdd (buf) || isglobalvdd (buf))
                            strcpy (nom, mbk_getvddname ());
                    }
                    if (thereisvss == 'n') {
                        if (isvss (buf) || isglobalvss (buf))
                            strcpy (nom, mbk_getvssname ());
                    }
                }
            }
        }

        if (SPI_MERGE)
            stopchainsepar (nom);

        ninterf->GINTERF = addchain (ninterf->GINTERF, namealloc (nom)
            );
    }

    ninterf->GINTERF = reverse (ninterf->GINTERF);

    return (ALLINTERF);
}

/******************************************************************************/

allinterf *constinterf (ptcir, ALLINTERF)
circuit *ptcir;
allinterf *ALLINTERF;
{
    ginterf *ninterf;
    char **tabnom;
    int i;
    int nb;
    noeud *ptnoeud;
    noeud *scannoeud;
    char nom[1024], nom0[1024];
    chain_list *scaninterf;
    chain_list *scandup;


    ninterf = (ginterf *) mbkalloc (sizeof (ginterf));
    ninterf->NOM = namealloc (ptcir->NOM);
    ninterf->SUIV = ALLINTERF->teteinterf;
    ALLINTERF->teteinterf = ninterf;

    addhtitem (ALLINTERF->h, namealloc (ninterf->NOM), (long)ninterf);

    /* On cree pour chaque signal de l'interface une chain_list contenant tous
     * les noms possibles pour un signal. Pour chaque signal, le nom retenu en
     * priorite est le premier nommé sur l'interface */

    /* Calcul du nombre d'elements */
    nb = 0;
    for (scaninterf = ptcir->CINTERF; scaninterf; scaninterf = scaninterf->NEXT) {
        ptnoeud = (noeud *) scaninterf->DATA;

        if (ptnoeud->signal->b.SIGNAL > nb)
            nb = ptnoeud->signal->b.SIGNAL;
    }

    /* Creation du tableau */
    tabnom = (char **)mbkalloc (sizeof (char *) * nb);
    for (i = 0; i < nb; i++)
        tabnom[i] = NULL;

    /* Remplissage du tableau */
    /* On regarde d'abord l'interface */
    for (scaninterf = ptcir->CINTERF; scaninterf; scaninterf = scaninterf->NEXT) {
        ptnoeud = (noeud *) scaninterf->DATA;

        if (!tabnom[ptnoeud->signal->b.SIGNAL - 1]) {
            if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom) == 0 && !tabnom[ptnoeud->signal->b.SIGNAL - 1]) {
                tabnom[ptnoeud->signal->b.SIGNAL - 1] = namealloc (nom);
            }
        }
    }
    /* Puis les noeuds constituant les signaux */

    for (scaninterf = ptcir->CINTERF; scaninterf; scaninterf = scaninterf->NEXT) {
        ptnoeud = (noeud *) scaninterf->DATA;
        if (!tabnom[ptnoeud->signal->b.SIGNAL - 1]) {
            int i;
            for (i = 0; i < ptcir->nbnodes; i++) {
                scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
                if (scannoeud->index < 0)
                    continue;
                if (scannoeud->signal->b.SIGNAL == ptnoeud->signal->b.SIGNAL
                    && AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom) == 0) {
                    /* Il est possible que le nom de signal soit deja utilise : 
                     * on le verifie*/
                    for (i = 0; i < nb; i++)
                        if (strcmp (tabnom[i], nom) == 0)
                            break;

                    if (i == nb) {
                        tabnom[ptnoeud->signal->b.SIGNAL - 1] = namealloc (nom);
                        break;
                    }
                }
            }
        }
    }

    /* finallement on cree un nom de connecteurs */
    i = 0;
    for (scaninterf = ptcir->CINTERF; scaninterf; scaninterf = scaninterf->NEXT) {
        ptnoeud = (noeud *) scaninterf->DATA;
        if (tabnom[ptnoeud->signal->b.SIGNAL - 1] == NULL) {
            if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom0) != 0)
                sprintf (nom, "SPICE_C_%s", nom0);
            else
                sprintf (nom, "SPICE_C_%d", i++);
            tabnom[ptnoeud->signal->b.SIGNAL - 1] = spicenamealloc (ptcir, nom);
        }
    }

    /* On cree la chainlist de interf. */

    ninterf->GINTERF = NULL;
    ninterf->DUPCON = NULL;
    ninterf->CORRESPINTERFNAME = NULL;

    for (scaninterf = ptcir->CINTERF; scaninterf; scaninterf = scaninterf->NEXT) {
        ptnoeud = (noeud *) scaninterf->DATA;
        strcpy (nom, tabnom[ptnoeud->signal->b.SIGNAL - 1]);

        if (SPI_MERGE)
            stopchainsepar (nom);

        ninterf->GINTERF = addchain (ninterf->GINTERF, namealloc (nom)
            );
        AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom0);
        ninterf->CORRESPINTERFNAME = addchain (ninterf->CORRESPINTERFNAME, spi_devect (nom0));
    }

    ninterf->GINTERF = reverse (ninterf->GINTERF);
    ninterf->CORRESPINTERFNAME = reverse (ninterf->CORRESPINTERFNAME);
    /*
       La liste DUPCON sert à repérer les noeuds du subckt qui apparaissent en 
       plusieurs fois sur l'interface du subckt. On crée une liste chainée jumelle de
       GINTERF, dont les champs DATA valent NULL, sauf dans le cas où un doublon est
       détecté. Dans ce cas le champ DATA pointe vers le chain_list correspondant.
       ex :
       subckt toto 1 2 1 3 1 4
       avec
       1 et 3 : signal "toto"
       2      :   -    "titi"
       4      :   -    "tutu"

       on aura :

       GINTERF "toto" "titi" "toto" "toto" "toto" "tutu"
       DUPCON:   X    NULL     X     NULL    X    NULL
       \------------/ \-----------/
     */
    {
        chain_list *lst, *sc2;
        for (scaninterf = ptcir->CINTERF, lst = NULL; scaninterf; scaninterf = scaninterf->NEXT) {
            ninterf->DUPCON = addchain (ninterf->DUPCON, NULL);
            ptnoeud = (noeud *) scaninterf->DATA;
            lst = addchain (lst, ptnoeud->SUPINFO);
            ptnoeud->SUPINFO = NULL;
        }

        lst = reverse (lst);

        for (scaninterf = ptcir->CINTERF, scandup = ninterf->DUPCON;
             scaninterf; scaninterf = scaninterf->NEXT, scandup = scandup->NEXT) {
            ptnoeud = (noeud *) scaninterf->DATA;
            if (ptnoeud->SUPINFO != NULL)
                ((chain_list *) ptnoeud->SUPINFO)->DATA = scandup;
            ptnoeud->SUPINFO = (s_noeud_sup_info *) scandup;
        }

        for (scaninterf = ptcir->CINTERF, sc2 = lst; scaninterf; sc2 = sc2->NEXT, scaninterf = scaninterf->NEXT) {
            ptnoeud = (noeud *) scaninterf->DATA;
            ptnoeud->SUPINFO = (s_noeud_sup_info *) sc2->DATA;
        }
        freechain (lst);

    }
    mbkfree (tabnom);

    return (ALLINTERF);
}

/******************************************************************************/
static chain_list *morecut (char *line, char *buf)
{
    chain_list *cl = NULL;
    char *c, *temp;
    strcpy (buf, line);

    c = strtok_r (buf, " (),", &temp);
    while (c != NULL) {
        cl = addchain (cl, c);
        c = strtok_r (NULL, " (),", &temp);
    }
    return reverse (cl);
}

static double eval_and_check (char *expr, spifile * df, int checkneg)
{
    double val;
    val = spi_eval (df, GLOBAL_CTX, expr, NULL, 0, checkneg);
    if (eqt_resistrue (GLOBAL_CTX))
        return val;
    else
        avt_errmsg(SPI_ERRMSG, "052", AVT_WARNING, df->filename, df->linenum, expr);
    return 0;
}

static int spi_parse_pulse (circuit * ptcir, char *aname, chain_list * ligne)
{
    // ( v0 v1 t0 tr tf uptime period )
    float period, pulsew, tr, tf, t0, v0, v1, dr, df, rt, ft;
    float VTH, VTH_H, VTH_L;
    char buf[1024];
    inffig_list *ifl;
    spifile *sdf = ptcir->sf;

    return 0;
    
    if (ligne == NULL || countchain (ligne = morecut ((char *)ligne->DATA, buf)) != 7) {
        avt_errmsg(SPI_ERRMSG, "053", AVT_WARNING, sdf->filename, sdf->msl_line);
        freechain (ligne);
        return 1;
    }
    if (SIM_VTH == -1)
        VTH = 0.5;
    else
        VTH = SIM_VTH;
    if (SIM_VTH_HIGH == -1)
        VTH_H = 0.8;
    else
        VTH_H = SIM_VTH_HIGH;
    if (SIM_VTH_LOW == -1)
        VTH_L = 0.2;
    else
        VTH_L = SIM_VTH_LOW;

    aname = namealloc (aname);
    v0 = eval_and_check ((char *)ligne->DATA, sdf, 0);
    v1 = eval_and_check ((char *)ligne->NEXT->DATA, sdf, 0);
    t0 = eval_and_check ((char *)ligne->NEXT->NEXT->DATA, sdf, 1);
    tr = eval_and_check ((char *)ligne->NEXT->NEXT->NEXT->DATA, sdf, 1);
    tf = eval_and_check ((char *)ligne->NEXT->NEXT->NEXT->NEXT->DATA, sdf, 1);
    pulsew = eval_and_check ((char *)ligne->NEXT->NEXT->NEXT->NEXT->NEXT->DATA, sdf, 1);
    period = eval_and_check ((char *)ligne->NEXT->NEXT->NEXT->NEXT->NEXT->NEXT->DATA, sdf, 1);
    df = VTH * tf;
    dr = VTH * tr;
    if (v1 > v0) {
        rt = t0 + dr;
        ft = t0 + tr + pulsew + df;
    }
    else {
        ft = t0 + df;
        rt = t0 + tf + pulsew + dr;
    }

//  printf("%s time: R=%g F=%g, PER=%g, slope: R=%g F=%g\n", aname, rt, ft, period, tr*(VTH_H-VTH_L), tf*(VTH_H-VTH_L));
    if ((ifl = getloadedinffig (ptcir->NOM)) == NULL)
        ifl = addinffig (ptcir==SPI_TOPCIR?TOPCIRINFNAME:ptcir->NOM);

    sprintf (buf, "%s:%d: ", sdf->filename, sdf->msl_line);

    inf_AddInt (ifl, INF_DEFAULT_LOCATION, aname, INF_CLOCK_TYPE, 0, buf);
    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_CLOCK_PERIOD, period, buf);
    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MIN_RISE_TIME, rt, buf);
    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MAX_RISE_TIME, rt, buf);
    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MIN_FALL_TIME, ft, buf);
    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MAX_FALL_TIME, ft, buf);

    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_PIN_RISING_SLEW, tr * (VTH_H - VTH_L), buf);
    inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_PIN_FALLING_SLEW, tf * (VTH_H - VTH_L), buf);

    freechain (ligne);
    return 0;
}

static int spi_parse_pwl (circuit * ptcir, char *aname, chain_list * ligne)
{
    // ( v0 v1 t0 tr tf uptime period )
    float period;
    float VTH, VTH_H, VTH_L;
    char buf[4096];
    double tabx[1024], taby[1024];
    double vth, vtl, vtm, shift = 0, slopeu = -1, sloped = -1;
    double risetime = -1, falltime = -1;
    sim_slopes slopes[1024];
    char *id;
    int i, j;
    spifile *sdf = ptcir->sf;
    chain_list *saveligne;
    inffig_list *ifl;

    aname = namealloc (aname);

    saveligne = ligne = morecut ((char *)ligne->DATA, buf);

    if (ligne == NULL) {
        avt_errmsg(SPI_ERRMSG, "049", AVT_WARNING, sdf->filename, sdf->msl_line);
        return 1;
    }

    if (SIM_VTH == -1)
        VTH = 0.5;
    else
        VTH = SIM_VTH;
    if (SIM_VTH_HIGH == -1)
        VTH_H = 0.8;
    else
        VTH_H = SIM_VTH_HIGH;
    if (SIM_VTH_LOW == -1)
        VTH_L = 0.2;
    else
        VTH_L = SIM_VTH_LOW;

    vtm = VTH * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    vth = VTH_H * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
    vtl = VTH_L * V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;

    i = 0;
    while (ligne != NULL) {
        if (ligne->NEXT != NULL && strcmp (ligne->NEXT->DATA, "=") == 0) {
            id = ligne->DATA;
            ligne = ligne->NEXT;
            if (ligne->NEXT != NULL) {
                if (strcasecmp (id, "shift") == 0)
                    shift = eval_and_check ((char *)ligne->DATA, sdf, 0);
                ligne = ligne->NEXT;
            }
        }
        else {
            if (ligne->NEXT != NULL) {
                tabx[i] = eval_and_check ((char *)ligne->DATA, sdf, 1);
                taby[i] = eval_and_check ((char *)ligne->NEXT->DATA, sdf, 1);
                if (i > 0 && tabx[i] < tabx[i - 1]) {
                    avt_errmsg(SPI_ERRMSG, "049", AVT_WARNING, sdf->filename, sdf->msl_line);
                    freechain (saveligne);
                    return 1;
                }
                i++;
                ligne = ligne->NEXT;
            }
            ligne = ligne->NEXT;
        }
    }

    j = sim_getallslopes (tabx, taby, i, vtm, vtl, vth, slopes);

    if (j > 0) {
        if ((ifl = getloadedinffig (ptcir->NOM)) == NULL)
            ifl = addinffig (ptcir==SPI_TOPCIR?TOPCIRINFNAME:ptcir->NOM);
    }
    else
        ifl = NULL;

    sprintf (buf, "%s:%d: ", sdf->filename, sdf->msl_line);

    for (i = 0; i < j; i++) {
        if (slopes[i].direction == 'u') {
            if (slopeu == -1) {
                slopeu = slopes[i].time_high - slopes[i].time_low;
                risetime = slopes[i].time + shift;
//              printf("rise transition at %g on %s = %g\n", risetime, aname, slopeu);
                inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_PIN_RISING_SLEW, slopeu, buf);
            }
            else if (fabs (slopeu - (slopes[i].time_high - slopes[i].time_low)) >= 1e-12) {
                avt_errmsg(SPI_ERRMSG, "050", AVT_WARNING, sdf->filename, sdf->msl_line, i + 1);
            }
        }
        else {
            if (sloped == -1) {
                sloped = slopes[i].time_low - slopes[i].time_high;
                falltime = slopes[i].time + shift;
//              printf("fall transition at %g on %s = %g\n", falltime, aname, sloped);
                inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_PIN_FALLING_SLEW, sloped, buf);
            }
            else if (fabs (sloped - (slopes[i].time_low - slopes[i].time_high)) >= 1e-12) {
                avt_errmsg(SPI_ERRMSG, "050", AVT_WARNING, sdf->filename, sdf->msl_line, i + 1);
            }
        }
    }

    if (j > 2) {
        period = slopes[2].time - slopes[0].time;
        inf_AddInt (ifl, INF_DEFAULT_LOCATION, aname, INF_CLOCK_TYPE, 0, buf);
        inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_CLOCK_PERIOD, period, buf);
        inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MIN_RISE_TIME, risetime, buf);
        inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MAX_RISE_TIME, risetime, buf);
        inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MIN_FALL_TIME, falltime, buf);
        inf_AddDouble (ifl, INF_DEFAULT_LOCATION, aname, INF_MAX_FALL_TIME, falltime, buf);
//      printf("%s time: R=%g F=%g, PER=%g\n", aname, risetime, falltime, period);
    }

    if (j == 0) avt_errmsg(SPI_ERRMSG, "056", AVT_WARNING, sdf->filename, sdf->msl_line, aname);
    freechain (saveligne);
    return 0;
}

int spi_parse_voltage (circuit * ptcir, chain_list * ligne, spifile * df)
{
    noeud *a, *b;
    char *vname, *a_name, *b_name, *expr=NULL;
    chain_list *elem, *ch;
    valim *ptvalim, *scanvalim;
    float valeur;
    int ret;

    if (*((char *)(ligne->DATA) + 1) == 0)
        avt_errmsg (SPI_ERRMSG, "036", AVT_WARNING, df->filename, df->linenum);

    vname = (char *)(ligne->DATA) + 1;

    elem = ligne->NEXT;            /* first node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    a_name = (char *)(elem->DATA);

    elem = elem->NEXT;            /* second node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    b_name = (char *)(elem->DATA);

    elem = elem->NEXT;            /* V value */
    if (elem)
      {
        int ispwl=0, ispwl2=0;
        if (strcasecmp ((char *)elem->DATA, "DC") == 0)
          elem = elem->NEXT;
        if (!elem)
          avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
        if (strcmp ((char *)(elem->DATA), "=") == 0)
          elem = elem->NEXT;
        if (!elem)
          avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
        if (strncasecmp ((char *)(elem->DATA), "pwl", 3) == 0) ispwl=1;
        if (elem->NEXT!=NULL)
        {
          if (strncasecmp ((char *)(elem->NEXT->DATA), "pwl", 3) == 0) ispwl2=1;
        }
        if (ispwl || ispwl2) {
          if (ispwl2) elem=elem->NEXT;
          if (elem->NEXT == NULL) {
            ch = addchain (NULL, &((char *)elem->DATA)[3]);
            ret = spi_parse_pwl (ptcir, a_name, ch);
            freechain (ch);
            return ret;
          }
          return spi_parse_pwl (ptcir, a_name, elem->NEXT);    // was 'return (0)'
        }
        if (strncasecmp ((char *)(elem->DATA), "pulse", 5) == 0) {
          if (elem->NEXT == NULL) {
            ch = addchain (NULL, &((char *)elem->DATA)[5]);
            ret = spi_parse_pulse (ptcir, a_name, ch);
            freechain (ch);
            return ret;
          }
          return spi_parse_pulse (ptcir, a_name, elem->NEXT);
        }
        valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)(elem->DATA), NULL, 0, 0);
        if (!eqt_resistrue (GLOBAL_CTX))
          {
            expr=mbkstrdup((char *)elem->DATA);
            //        return 0;
          }
      }
    else
      valeur=0;

    if (strchr(a_name,'.')==0) a = ajoutenoeud (ptcir, a_name, 0); else a=NULL;
    if (strchr(b_name,'.')==0) b = ajoutenoeud (ptcir, b_name, 0); else b=NULL;

    if (a==NULL || b==NULL)
    {
      unsolved_vcard_type *uvc;
      uvc=(unsolved_vcard_type *)mbkalloc(sizeof(unsolved_vcard_type));
      uvc->s[0].name=uvc->s[1].name=NULL;
      uvc->s[0].u.node=a; uvc->s[1].u.node=b;
      uvc->s[0].u.node=a; uvc->s[1].u.node=b;
      uvc->value=valeur;
      uvc->expr=sensitive_namealloc(expr);
      uvc->s[0].nodename=namealloc(a_name);
      uvc->s[1].nodename=namealloc(b_name);
      if (a==NULL) uvc->s[0].name=namealloc(a_name); //namealloc((SPICE_KEEP_CARDS & KEEP__INSTANCE)?a_name:a_name+1);
      if (b==NULL) uvc->s[1].name=namealloc(b_name); //namealloc((SPICE_KEEP_CARDS & KEEP__INSTANCE)?b_name:b_name+1);
      ptcir->UNSOLVED_VCARDS=addchain(ptcir->UNSOLVED_VCARDS, uvc);
      return 0;
    }
    
    
    if (SPI_ZEROVOLT_RESI && ((SPI_IGNORE_RESI || SPI_DRIVE_SPEF_MODE) && (valeur > -1e-15 && valeur < 1e-15) && expr==NULL)) {
        _spispef_mergenodes (ptcir, a, b);
        a->signal->nbelem++;
        return 0;
    }

    if (SPI_IGNORE_VOLTAGE) {
        return 0;
    }

    ptvalim = (valim *) spiciralloue (ptcir, sizeof (valim));

    if ((SPICE_KEEP_NAMES & KEEP__ALL) == KEEP__ALL) {
        ptvalim->NOM = spicenamealloc (ptcir, vname);
        if (SPI_CHECK_NAME) {
            scanvalim = getthashelem (ptvalim->NOM, ptcir->HASHVALIM, NULL);
            if (scanvalim) {
                avt_errmsg (SPI_ERRMSG, "037", AVT_FATAL, df->filename, df->linenum, ptvalim->NOM);
            }
            else
                addthashelem (ptvalim->NOM, ptvalim, ptcir->HASHVALIM);
        }
    }
    else
        ptvalim->NOM = NULL;
    ptvalim->SUIV = ptcir->VALIM;
    ptcir->VALIM = ptvalim;
    ptvalim->N1 = a;
    ptvalim->N2 = b;
    ptvalim->EXPR=sensitive_namealloc(expr);

    ptvalim->TENSION = valeur;

    return 1;
}

/******************************************************************************/

int spi_parse_capa (circuit * ptcir, chain_list * ligne, spifile * df)
{
    float cnom = 0.0;
    char *capaname;
    noeud *a, *b, *ptnoeud;
    chain_list *elem, *elem1, *elem2;
    loctc_list *ptcapa, *scancapa;
    char *paramexpr;
    float valeur;
    char valuecfound;

    if (*((char *)(ligne->DATA) + 1) == 0 && (SPICE_KEEP_CARDS & KEEP__CAPA) == 0)
        avt_errmsg (SPI_ERRMSG, "038", AVT_WARNING, df->filename, df->linenum); 

    if ((SPICE_KEEP_CARDS & KEEP__CAPA) != 0)
        capaname = (char *)(ligne->DATA);
    else
        capaname = (char *)(ligne->DATA) + 1;

    if (mbk_CheckREGEX (&ptcir->IGNORE_CAPACITANCE, capaname)) {
        avt_log (LOGSTATPRS, 2, "%s:%d: capacitor '%s' ignored in circuit '%s'\n", df->filename, df->linenum, capaname,
                 ptcir->NOM);
        MBK_PARSE_ERROR.NB_CAPA++;
        return 1;
    }

    elem = ligne->NEXT;            /* first node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum); 
    a = ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 3);

    elem = elem->NEXT;            /* second node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum); 
    b = ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 1);

    if (SPI_IGNORE_CAPA || SPI_DRIVE_SPEF_MODE) {
        if (a->signal != b->signal) {
            a->signal->nbelem++;
            b->signal->nbelem++;
        }
        return 0;
    }

    ptcapa = heaploctc ();
    ptcapa->SIG1 = (losig_list *) ptcir->CAPA;
    ptcir->CAPA = ptcapa;

    ptcapa->NODE1 = (long)a;
    ptcapa->NODE2 = (long)b;
    ptcapa->USER = NULL;

    if (elem->NEXT == NULL)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    valuecfound = 'n';
    if (elem->NEXT->NEXT == NULL || strcmp (elem->NEXT->NEXT->DATA, "=") != 0) {
        elem = elem->NEXT;        /* C value */
        valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem->DATA, &paramexpr, ptcir!=NULL?1:0, 1);
        if (paramexpr)
            addloctcparam (ptcapa, "val", valeur, paramexpr);
        ptcapa->CAPA = valeur * SPI_SCALE_CAPAFACTOR * 1e12;    /* Les valeurs dans Alliance sont en pif */
        valuecfound = 'y';
    }

    /* Le reste des parametres est de la forme xxx = yyy */

    while ((elem = elem->NEXT)) {

        elem1 = elem->NEXT;
        elem2 = (elem1 ? elem1->NEXT : NULL);

        if (elem && elem1 && elem2 && (!strcmp ((char *)(elem1->DATA), "="))) {

            if (strcasecmp ((char *)elem->DATA, "POLY") == 0) {
                ptcir->CAPA = (loctc_list *) ptcapa->SIG1;
                ptcapa->USER = (ptype_list *) ptcapa->SIG2;
                freeloctc (ptcapa);
                return 1;
            }

            valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem2->DATA, &paramexpr, ptcir!=NULL?1:0, 0);

            if (strcasecmp ((char *)elem->DATA, "C") == 0 ||
                strcasecmp ((char *)elem->DATA, "VAL") == 0 || strcasecmp ((char *)elem->DATA, "VALUE") == 0) {
                cnom = valeur;
                if (paramexpr)
                    addloctcparam (ptcapa, "val", valeur, paramexpr);
            }
            else
                addloctcparam (ptcapa, (char *)elem->DATA, valeur, paramexpr);

            elem = elem->NEXT->NEXT;
        }
        else {
            switch (SPI_TOLERANCE) {
            case 0:
                avt_errmsg (SPI_ERRMSG, "064", AVT_FATAL, df->filename, df->linenum);
                break;
            case 1:
                avt_errmsg (SPI_ERRMSG, "064", AVT_WARNING, df->filename, df->linenum);
                break;
            case 2:
                break;
            }
            break;
        }
    }
    ptcapa->SIG2 = (losig_list *) ptcapa->USER;
    ptcapa->USER = NULL;        // le USER sert a stocker le nom !!!!
    if (valuecfound == 'n')
        ptcapa->CAPA = cnom * SPI_SCALE_CAPAFACTOR * 1e12;

    if (ptcapa->NODE1 == ptcapa->NODE2 || (SPI_MIN_CAPA > 0.0 && ptcapa->CAPA < SPI_MIN_CAPA)) {
        ptcir->CAPA = (loctc_list *) ptcapa->SIG1;
        ptcapa->USER = (ptype_list *) ptcapa->SIG2;
        freeloctc (ptcapa);
    }
    else {
        if ((SPICE_KEEP_NAMES & KEEP__CAPA) == KEEP__CAPA) {
            ptcapa->USER = (ptype_list *) spicenamealloc (ptcir, capaname);
            if (SPI_CHECK_NAME) {
                scancapa = getthashelem ((char *)ptcapa->USER, ptcir->HASHCAPA, NULL);
                if (scancapa) {
                    avt_errmsg (SPI_ERRMSG, "039", AVT_FATAL, df->filename, df->linenum);
                }
                else
                    addthashelem ((char *)ptcapa->USER, ptcapa, ptcir->HASHCAPA);
            }
        }
        else
            ptcapa->USER = NULL;
    }

    return 1;
}

/******************************************************************************/
void spi_rename_instance_model (circuit * ptcir, chain_list *list)
{
    inst *ptinst;
    long l;
    chain_list *cl;
    //lofig_list *lf;
    circuit * ptcirsub;
    
    ht *newht = NULL;
    if (ptcir != NULL) {
        newht = addht (100);
        for (ptinst = ptcir->INST; ptinst != NULL; ptinst = ptinst->SUIV) {
          if (ptcir->SUBCIRCUITS!=NULL && (l = gethtitem (ptcir->SUBCIRCUITS, namealloc(ptinst->MODELE))) != EMPTYHT)
          {
            ptinst->MODELE = (char *)l;
          }
          else
          {
            for (cl=list; cl!=NULL; cl=cl->NEXT)
            {
              ptcirsub=(circuit *)cl->DATA;
              if (ptcirsub->SUBCIRCUITS!=NULL && (l = gethtitem (ptcirsub->SUBCIRCUITS, namealloc(ptinst->MODELE))) != EMPTYHT) {
                ptinst->MODELE = (char *)l;
                break;
              }
            }
          }
          addhtitem (newht, ptinst->MODELE, 0);
        }
        
        delht (ptcir->ALL_INSTANCE_MODELS);
        ptcir->ALL_INSTANCE_MODELS = newht;
    }
}

int spi_parse_instance (circuit * ptcir, chain_list * ligne, spifile * df, float com_x, float com_y,
                        int Tx, int Ty, int R, int A)
{
    inst *ptinst, *scaninst;
    int i, nbelem;
    chain_list *elem, *elem1, *elem2, *ptchain, *namelist;
    noeud *ptnoeud;
    float valeur;
    char *paramexpr;

    if (ptcir->INSTLIBRE == NULL) {
        ptcir->INSTLIBRE = (inst *) spiciralloue (ptcir, sizeof (inst) * TAILLEINST);
        ptinst = ptcir->INSTLIBRE;
        for (i = 1; i < TAILLEINST; i++) {
            ptinst->SUIV = ptinst + 1;
            ptinst++;
        }
        ptinst->SUIV = NULL;
    }

    ptinst = ptcir->INSTLIBRE;
    ptcir->INSTLIBRE = ptcir->INSTLIBRE->SUIV;

    ptinst->IINTERF = NULL;
    ptinst->PARAM = NULL;

    if (*(((char *)(ligne->DATA)) + 1) == 0 && (SPICE_KEEP_CARDS & KEEP__INSTANCE) == 0)
        avt_errmsg (SPI_ERRMSG, "040", AVT_FATAL, df->filename, df->linenum);

    if (SPICE_KEEP_CARDS & KEEP__INSTANCE)
        ptinst->NOM = spicenamealloc (ptcir, (char *)(ligne->DATA));
    else
        ptinst->NOM = spicenamealloc (ptcir, ((char *)(ligne->DATA)) + 1);

    if (mbk_CheckREGEX (&ptcir->IGNORE_INSTANCE, ptinst->NOM)) {
        avt_log (LOGSTATPRS, 2, "%s:%d: instance '%s' ignored in circuit '%s'\n", df->filename, df->linenum,
                 ptinst->NOM, ptcir->NOM);
        MBK_PARSE_ERROR.NB_INSTANCE++;
        return 1;
    }

    if (SPI_CHECK_NAME) {
        scaninst = getthashelem (ptinst->NOM, ptcir->HASHINST, NULL);
        if (scaninst)
            avt_errmsg (SPI_ERRMSG, "041", AVT_FATAL, df->filename, df->linenum, ptinst->NOM);
        addthashelem (ptinst->NOM, ptinst, ptcir->HASHINST);
    }

    ptinst->SUIV = ptcir->INST;
    ptcir->INST = ptinst;

    for (nbelem = 0, elem = ligne->NEXT; elem; elem = elem->NEXT, nbelem++);
    if (!nbelem) 
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    
    for (elem = ligne->NEXT; elem->NEXT; elem = elem->NEXT) {
        /* Check for parameters */
        if (elem->NEXT->NEXT && strcmp ((char *)elem->NEXT->NEXT->DATA, "=") == 0) {
            break;
        }
        if ((namelist = spi_expandvector (ptcir, (char *)(elem->DATA)))) {
            for (ptchain = namelist; ptchain; ptchain = ptchain->NEXT) {
                ptnoeud = ajoutenoeud (ptcir, (char *)(ptchain->DATA), 0);
                ptinst->IINTERF = addchain (ptinst->IINTERF, ptnoeud);
            }
            freechain (namelist);
        }
        else {
            ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
            ptinst->IINTERF = addchain (ptinst->IINTERF, ptnoeud);
        }
    }
    ptinst->IINTERF = reverse (ptinst->IINTERF);

    ptinst->MODELE = spicenamealloc (ptcir, (char *)elem->DATA);
    addhtitem (ptcir->ALL_INSTANCE_MODELS, ptinst->MODELE, 0);

    /* Check for parameters */
    for (elem = elem->NEXT; elem; elem = elem->NEXT) {
        if (elem->NEXT && !strcmp ((char *)elem->NEXT->DATA, "=")) {
            elem1 = elem->NEXT;
            elem2 = (elem1 ? elem1->NEXT : NULL);
            if (!elem2)
                avt_errmsg (SPI_ERRMSG, "061", AVT_FATAL, df->filename, df->linenum);

            valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem2->DATA, &paramexpr, ptcir!=NULL?1:0,0);

            ptinst->PARAM = spi_addinsparam (ptcir, ptinst->PARAM, (char *)elem->DATA, valeur, paramexpr);
            elem = elem2;
        }
        else 
            avt_errmsg (SPI_ERRMSG, "011", AVT_FATAL, df->filename, df->linenum, (char *)(elem->DATA));
    }
    if (ptinst->PARAM)
        ptinst->PARAM = (insparam_list *) reverse ((chain_list *) ptinst->PARAM);

    // XY coordinates

    ptinst->X = (int)com_x;
    ptinst->Y = (int)com_y;
    ptinst->Tx = Tx;
    ptinst->Ty = Ty;
    ptinst->R = R;
    ptinst->A = A;

    return 1;
}

/******************************************************************************/
static void spi_short_circuit (circuit * ptcir, noeud * a, noeud * b)
{
    chain_list *la = NULL, *lb = NULL, **common = NULL;
    if (a == b)
        return;
    CheckNodeSupInfo (ptcir, a);
    CheckNodeSupInfo (ptcir, b);

    if (a->SUPINFO->U.SHORTCIRCUIT != NULL)
        common = a->SUPINFO->U.SHORTCIRCUIT, la = *common;
    else
        la = addchain (NULL, a);

    if (b->SUPINFO->U.SHORTCIRCUIT != NULL) {
        lb = *b->SUPINFO->U.SHORTCIRCUIT;
        if (la == lb)
            return;
        if (common != NULL)
            mbkfree (b->SUPINFO->U.SHORTCIRCUIT);
        else
            common = b->SUPINFO->U.SHORTCIRCUIT;
    }
    else
        lb = addchain (NULL, b);

    if (common == NULL)
        common = (chain_list **) mbkalloc (sizeof (chain_list *));

    la = append (la, lb);
    *common = la;
    while (la != NULL)
        ((noeud *) la->DATA)->SUPINFO->U.SHORTCIRCUIT = common, la = la->NEXT;
}


/******************************************************************************/
int spi_parse_induc (circuit * ptcir, chain_list * ligne, spifile * df)
{
    noeud *a, *b;
    chain_list *elem;
    
    elem = ligne->NEXT;            /* first node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    a = ajoutenoeud (ptcir, (char *)(elem->DATA), 1);

    elem = elem->NEXT;            /* second node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    b = ajoutenoeud (ptcir, (char *)(elem->DATA), 1);

    if (a == b)
        return 1;

    _spispef_mergenodes (ptcir, a, b);
    spi_short_circuit (ptcir, a, b);
    a->signal->nbelem++;
    return 0;
}

int spi_parse_wire (circuit * ptcir, chain_list * ligne, spifile * df)
{
    float rnom = 0.0, tc1 = 0.0, tc2 = 0.0;
    lowire_list *ptresi, *scanresi;
    char *resiname, *param;
    noeud *a, *b, *ptnoeud;
    float valeur;
    char *paramexpr, *rnomexpr = NULL, *tc1expr = NULL, *tc2expr = NULL;
    chain_list *elem, *elem1, *elem2;
    char foundrvalue;

    if (*((char *)(ligne->DATA) + 1) == 0 && (SPICE_KEEP_CARDS & KEEP__RESISTANCE) == 0)
        avt_errmsg (SPI_ERRMSG, "036", AVT_WARNING, df->filename, df->linenum);

    if ((SPICE_KEEP_CARDS & KEEP__RESISTANCE) == 0)
        resiname = (char *)(ligne->DATA) + 1;
    else
        resiname = (char *)(ligne->DATA);
    if (mbk_CheckREGEX (&ptcir->IGNORE_RESISTANCE, resiname)) {
        avt_log (LOGSTATPRS, 2, "%s:%d: resistor '%s' ignored in circuit '%s'\n", df->filename, df->linenum, resiname,
                 ptcir->NOM);
        MBK_PARSE_ERROR.NB_RESI++;
        return 1;
    }

    elem = ligne->NEXT;            /* first node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    a = ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 3);

    elem = elem->NEXT;            /* second node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    b = ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 3);

    if (a == b)
        return 1;

    if (SPI_IGNORE_RESI || SPI_DRIVE_SPEF_MODE) {
        _spispef_mergenodes (ptcir, a, b);
        a->signal->nbelem++;
        return 0;
    }

    ptresi = heaplowire ();

    ptresi->NODE1 = (long)a;
    ptresi->NODE2 = (long)b;

    if (elem->NEXT == NULL)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);

    elem=elem->NEXT;
    
    foundrvalue = 'n';
    if (elem->NEXT == NULL || strcmp (elem->NEXT->DATA, "=") != 0) {
//        elem = elem->NEXT;        /* R value */
        valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem->DATA, &paramexpr, ptcir!=NULL?1:0,1);
        ptresi->RESI = valeur * SPI_SCALE_RESIFACTOR;
        ptresi->CAPA = 0.0;
        if (paramexpr)
          addlowireparam (ptresi, "r", valeur, paramexpr);
        foundrvalue = 'y';
        elem = elem->NEXT;
     
        if (elem!=NULL && (elem->NEXT == NULL || strcmp (elem->NEXT->DATA, "=") != 0)) {
          valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem->DATA, &paramexpr, ptcir!=NULL?1:0,1);
          addlowireparam (ptresi, "tc1", valeur, paramexpr);
          elem = elem->NEXT;

          if (elem!=NULL && (elem->NEXT == NULL || strcmp (elem->NEXT->DATA, "=") != 0)) {
            valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem->DATA, &paramexpr, ptcir!=NULL?1:0,1);
            addlowireparam (ptresi, "tc2", valeur, paramexpr);
            elem = elem->NEXT;
          }
        }
    }

    /* Le reste des parametres est de la forme xxx = yyy */

    while (elem) {

        elem1 = elem->NEXT;
        elem2 = (elem1 ? elem1->NEXT : NULL);

        if (elem && elem1 && elem2 && (!strcmp ((char *)(elem1->DATA), "="))) {

            valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem2->DATA, &paramexpr, ptcir!=NULL?1:0,0);
            param = (char *)elem->DATA;
            if (strcasecmp(param,"r")==0) rnom=valeur;

            addlowireparam (ptresi, param, valeur, paramexpr);

            elem = elem->NEXT->NEXT;
        }
        else {
            switch (SPI_TOLERANCE) {
            case 0:
                avt_errmsg (SPI_ERRMSG, "065", AVT_FATAL, df->filename, df->linenum);
                break;
            case 1:
                avt_errmsg (SPI_ERRMSG, "065", AVT_WARNING, df->filename, df->linenum);
                break;
            case 2:
                break;
            }
            break;
        }
        elem = elem->NEXT;
    }
    if (foundrvalue == 'n')
        ptresi->RESI = rnom * SPI_SCALE_RESIFACTOR;
    ptresi->CAPA = 0.0;

    if (SPI_MAX_RESI > 0.0 && ptresi->RESI > SPI_MAX_RESI) {
        freelowire (ptresi);
        return 1;
    }
    if (SPI_MIN_RESI > 0.0 && ptresi->RESI < SPI_MIN_RESI) {
        _spispef_mergenodes (ptcir, a, b);
        spi_short_circuit (ptcir, a, b);
        a->signal->nbelem++;
        freelowire (ptresi);
        return 1;
    }

    if ((SPICE_KEEP_NAMES & KEEP__RESISTANCE) == 0)
        ptresi->USER = addptype (ptresi->USER, RESINAME, spicenamealloc (ptcir, ((char *)(ligne->DATA) + 1)));
    else
        ptresi->USER = addptype (ptresi->USER, RESINAME, namealloc (resiname));

    if (SPI_CHECK_NAME) {
        scanresi = getthashelem ((char *)ptresi->USER->DATA, ptcir->HASHRESI, NULL);
        if (scanresi) {
            avt_errmsg (SPI_ERRMSG, "037", AVT_FATAL, df->filename, df->linenum);
        }
        else
            addthashelem ((char *)ptresi->USER->DATA, ptresi, ptcir->HASHRESI);
    }

    ptresi->NEXT = ptcir->RESI;
    ptcir->RESI = ptresi;

    _spispef_mergenodes (ptcir, a, b);
    a->signal->nbelem++;

    return 1;
}

/******************************************************************************/
int spi_parse_connect (circuit * ptcir, chain_list * ligne, spifile * df)
{
    lowire_list *ptresi;
    noeud *a, *b;
    chain_list *elem;

    elem = ligne->NEXT;            /* first node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    a = ajoutenoeud (ptcir, (char *)(elem->DATA), 1);

    elem = elem->NEXT;            /* second node */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, df->filename, df->linenum);
    b = ajoutenoeud (ptcir, (char *)(elem->DATA), 1);

    if (SPI_IGNORE_RESI || SPI_DRIVE_SPEF_MODE) {
        _spispef_mergenodes (ptcir, a, b);
        a->signal->nbelem++;
        return 0;
    }

    if (a == b)
        return 1;

    ptresi = heaplowire ();

    ptresi->NODE1 = (long)a;
    ptresi->NODE2 = (long)b;

    ptresi->RESI = 0;
    ptresi->CAPA = 0.0;

    if (elem->NEXT!=NULL)
      avt_errmsg (SPI_ERRMSG, "014", AVT_FATAL, df->filename, df->linenum);

    ptresi->NEXT = ptcir->RESI;
    ptcir->RESI = ptresi;

    _spispef_mergenodes (ptcir, a, b);
    a->signal->nbelem++;

    return 1;
}

/******************************************************************************/

int spi_parse_transistor (circuit * ptcir, chain_list * ligne, spifile * df, float com_x, float com_y)
{
    char *tmp_name;
    char *ins;
    chain_list *elem, *elem1, *elem2;
    noeud *ptnoeud;
    char *paramexpr;
    lotrs_list *pttrans, *scantrans;
    int extranodes = 0;

    pttrans = (lotrs_list *) mbkalloc (sizeof (lotrs_list));
    pttrans->TRNAME = NULL;
    pttrans->USER = NULL;

    if (*((char *)(ligne->DATA) + 1) == 0 && (SPICE_KEEP_CARDS & KEEP__TRANSISTOR)==0)
        avt_errmsg (SPI_ERRMSG, "025", AVT_WARNING, df->filename, df->linenum);

    if (SPICE_KEEP_CARDS & KEEP__TRANSISTOR)
        tmp_name = (char *)(ligne->DATA);
    else
        tmp_name = (char *)(ligne->DATA) + 1;

    if (SPI_DRIVE_SPEF_MODE || (SPICE_KEEP_NAMES & KEEP__TRANSISTOR) != 0) {
        pttrans->TRNAME = ins = namealloc (tmp_name);

        if (SPI_CHECK_NAME) {
            scantrans = getthashelem (pttrans->TRNAME, ptcir->HASHTRAN, NULL);
            if (scantrans) 
                avt_errmsg (SPI_ERRMSG, "026", AVT_FATAL, df->filename, df->linenum, pttrans->TRNAME);
            else
                addthashelem (pttrans->TRNAME, pttrans, ptcir->HASHTRAN);
        }
    }

    if (mbk_CheckREGEX (&ptcir->IGNORE_TRANSISTOR, tmp_name)) {
        avt_log (LOGSTATPRS, 2, "%s:%d: transistor '%s' ignored in circuit '%s'\n", df->filename, df->linenum,
                 tmp_name, ptcir->NOM);
        MBK_PARSE_ERROR.NB_LOTRS++;
        return 1;
    }

    pttrans->NEXT = ptcir->TRANS;
    ptcir->TRANS = pttrans;

    elem = ligne->NEXT;            /* drain */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "027", AVT_FATAL, df->filename, df->linenum);
    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    pttrans->DRAIN = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
    if (SPI_DRIVE_SPEF_MODE)
        _spispef_setnodename (ptcir, ptnoeud, ins, "d" /*MBK_DRAIN_NAME */ );

    elem = elem->NEXT;            /* grille */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "028", AVT_FATAL, df->filename, df->linenum);
    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    pttrans->GRID /*LLE*/ = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
    if (SPI_DRIVE_SPEF_MODE)
        _spispef_setnodename (ptcir, ptnoeud, ins, "g" /*MBK_GRID_NAME */ );

    elem = elem->NEXT;            /* source */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "029", AVT_FATAL, df->filename, df->linenum);
    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    pttrans->SOURCE = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
    if (SPI_DRIVE_SPEF_MODE)
        _spispef_setnodename (ptcir, ptnoeud, ins, "s" /*MBK_SOURCE_NAME */ );

    elem = elem->NEXT;            /* bulk ou type du transistor */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "030", AVT_FATAL, df->filename, df->linenum);

    pttrans->BULK = NULL;

    tmp_name = namealloc ((char *)elem->DATA);
    if (mbk_istransn (tmp_name)) {
        addlotrsmodel (pttrans, tmp_name);
        addhtitem (ptcir->ALL_TRANSISTOR_MODELS, tmp_name, 0);
        pttrans->TYPE = mbk_gettranstype (tmp_name);
    }
    else if (mbk_istransp (tmp_name)) {
        addlotrsmodel (pttrans, tmp_name);
        addhtitem (ptcir->ALL_TRANSISTOR_MODELS, tmp_name, 0);
        pttrans->TYPE = mbk_gettranstype (tmp_name);
    }
    else {
        if (SPI_IGNORE_BULK == 'N') {
            ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
            pttrans->BULK = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
            if (SPI_DRIVE_SPEF_MODE)
                _spispef_setnodename (ptcir, ptnoeud, ins, "b");
        }

        for (elem = elem->NEXT; elem; elem = elem->NEXT) {
            tmp_name = namealloc ((char *)elem->DATA);
            if (mbk_istransn (tmp_name)) {
                pttrans->TYPE = mbk_gettranstype (tmp_name);
                break;
            }
            else if (mbk_istransp (tmp_name)) {
                pttrans->TYPE = mbk_gettranstype (tmp_name);
                break;
            }
            else {
                extranodes++;
                if (elem->NEXT == NULL || extranodes > 3
                    || (elem->NEXT->NEXT && !strcmp ((char *)(elem->NEXT->NEXT->DATA), "="))) {
                    pttrans->TYPE = 0;
                    break;
                }
            }
        }
        addlotrsmodel (pttrans, tmp_name);
        addhtitem (ptcir->ALL_TRANSISTOR_MODELS, tmp_name, 0);
    }

    /* Le reste des parametres est de la forme xxx = yyy */

    if (!elem) 
        avt_errmsg (SPI_ERRMSG, "033", AVT_FATAL, df->filename, df->linenum);

    while ((elem = elem->NEXT)) {

        elem1 = elem->NEXT;
        if (!elem1 || strcmp ((char *)(elem1->DATA), "=")) {
            continue;
        }

        elem2 = (elem1 ? elem1->NEXT : NULL);
        if (!elem2)
            avt_errmsg (SPI_ERRMSG, "033", AVT_FATAL, df->filename, df->linenum);

        paramexpr = namealloc (spi_stripquotes (((char *)elem2->DATA)));
        addlotrsparam (pttrans, (char *)elem->DATA, 0.0, paramexpr);

        elem = elem->NEXT->NEXT;
    }

    pttrans->X = com_x;
    pttrans->Y = com_y;
    pttrans->PS = -1;
    pttrans->PD = -1;
    pttrans->XS = -1;
    pttrans->XD = -1;
    pttrans->WIDTH = 0;
    pttrans->LENGTH = 0;

    return 1;
}

/******************************************************************************/

int spi_parse_jfet (circuit * ptcir, chain_list * ligne, spifile * df, float com_x, float com_y)
{
    char *tmp_name;
    char *ins;
    chain_list *elem, *elem1, *elem2;
    noeud *ptnoeud;
    float valeur;
    char *paramexpr;
    lotrs_list *pttrans, *scantrans;
    lowire_list *ptresi, *scanresi;

    if (SPI_JFET_RESI) {
        ptresi = heaplowire ();
    }
    else {
        pttrans = (lotrs_list *) mbkalloc (sizeof (lotrs_list));
        pttrans->TRNAME = NULL;
        pttrans->USER = NULL;
    }

    if (*((char *)(ligne->DATA) + 1) == 0 && (SPICE_KEEP_CARDS & KEEP__RESISTANCE) == 0)
        avt_errmsg (SPI_ERRMSG, "025", AVT_WARNING, df->filename, df->linenum);

    if (SPI_DRIVE_SPEF_MODE || (SPICE_KEEP_NAMES & (KEEP__TRANSISTOR | KEEP__RESISTANCE)) != 0) {
        if (SPI_JFET_RESI) {
            if ((SPICE_KEEP_NAMES & KEEP__RESISTANCE) == 0)
                ptresi->USER = addptype (ptresi->USER, RESINAME, spicenamealloc (ptcir, ((char *)(ligne->DATA) + 1)));
            else {
                if ((SPICE_KEEP_CARDS & KEEP__RESISTANCE) == 0)    // YES
                    ptresi->USER = addptype (ptresi->USER, RESINAME, namealloc (((char *)(ligne->DATA) + 1)));
                else            // KEEP_R
                    ptresi->USER = addptype (ptresi->USER, RESINAME, namealloc ((char *)ligne->DATA));
            }

            if (SPI_CHECK_NAME) {
                scanresi = getthashelem ((char *)ptresi->USER->DATA, ptcir->HASHRESI, NULL);
                if (scanresi) 
                    avt_errmsg (SPI_ERRMSG, "037", AVT_FATAL, df->filename, df->linenum, (char *)ptresi->USER->DATA);
                else
                    addthashelem ((char *)ptresi->USER->DATA, ptresi, ptcir->HASHRESI);
            }
        }
        else {
            pttrans->TRNAME = ins = namealloc ((char *)(ligne->DATA) + 1);
            if (SPI_CHECK_NAME) {
                scantrans = getthashelem (pttrans->TRNAME, ptcir->HASHTRAN, NULL);
                if (scantrans) 
                    avt_errmsg (SPI_ERRMSG, "026", AVT_FATAL, df->filename, df->linenum);
                else
                    addthashelem (pttrans->TRNAME, pttrans, ptcir->HASHTRAN);
            }
        }
    }

    if (SPI_JFET_RESI) {
        ptresi->NEXT = ptcir->RESI;
        ptcir->RESI = ptresi;
    }
    else {
        pttrans->NEXT = ptcir->TRANS;
        ptcir->TRANS = pttrans;
    }

    elem = ligne->NEXT;            /* drain */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "027", AVT_FATAL, df->filename, df->linenum);
    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    if (SPI_JFET_RESI)
        ptresi->NODE1 = (long)ptnoeud;
    else {
        pttrans->DRAIN = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
        if (SPI_DRIVE_SPEF_MODE)
            _spispef_setnodename (ptcir, ptnoeud, ins, "d");
    }

    elem = elem->NEXT;            /* grille */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "028", AVT_FATAL, df->filename, df->linenum);
    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    if (!SPI_JFET_RESI) {
        pttrans->GRID = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
        if (SPI_DRIVE_SPEF_MODE)
            _spispef_setnodename (ptcir, ptnoeud, ins, "g");
    }

    elem = elem->NEXT;            /* source */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "029", AVT_FATAL, df->filename, df->linenum);
    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    if (SPI_JFET_RESI)
        ptresi->NODE2 = (long)ptnoeud;
    else {
        pttrans->SOURCE = (locon_list *) ptnoeud;    // [!] ce n'est pas un locon
        if (SPI_DRIVE_SPEF_MODE)
            _spispef_setnodename (ptcir, ptnoeud, ins, "s");
    }

    if (SPI_JFET_RESI) {
        ptresi->RESI = 0.0;
        ptresi->CAPA = 0.0;
        return 1;
    }

    elem = elem->NEXT;            /* type du transistor */
    if (!elem)
        avt_errmsg (SPI_ERRMSG, "030", AVT_FATAL, df->filename, df->linenum);

    pttrans->BULK = NULL;

    tmp_name = namealloc ((char *)elem->DATA);
    if (mbk_isjfetn (tmp_name)) {
        pttrans->TYPE = mbk_getjfettype (tmp_name);
    }
    else if (mbk_isjfetp (tmp_name)) {
        pttrans->TYPE = mbk_getjfettype (tmp_name);
    }
    else {
        pttrans->TYPE = 0;
    }
    addlotrsmodel (pttrans, tmp_name);

    /* Le reste des parametres est de la forme xxx = yyy */

    while ((elem = elem->NEXT)) {

        elem1 = elem->NEXT;
        elem2 = (elem1 ? elem1->NEXT : NULL);

        if (!elem2)
            avt_errmsg (SPI_ERRMSG, "033", AVT_FATAL, df->filename, df->linenum);

        if (strcmp ((char *)(elem1->DATA), "="))
            avt_errmsg (SPI_ERRMSG, "033", AVT_FATAL, df->filename, df->linenum);

        valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)elem2->DATA, &paramexpr, ptcir!=NULL?1:0,0);

        addlotrsparam (pttrans, (char *)elem->DATA, valeur, paramexpr);

        elem = elem->NEXT->NEXT;
    }
    pttrans->X = com_x;
    pttrans->Y = com_y;

    pttrans->PS = -1;
    pttrans->PD = -1;
    pttrans->XS = -1;
    pttrans->XD = -1;

    pttrans->WIDTH = 0;
    pttrans->LENGTH = 0;

    return 1;
}

/******************************************************************************/

int spi_parse_diode (circuit * ptcir, chain_list * ligne, spifile * df, float com_x, float com_y)
{
    chain_list *elem, *elem1, *elem2;
    diode *ptdiode, *scandiode;
    noeud *ptnoeud;
    float value;
    char *paramexpr, *tmp_name, *diodename;
    int i;

    if (SPI_IGNORE_DIODES) {
        return 0;
    }

    if (ptcir->DIODELIBRE == NULL) {
        ptcir->DIODELIBRE = (diode *) spiciralloue (ptcir, sizeof (diode) * TAILLEDIODE);
        ptdiode = ptcir->DIODELIBRE;
        for (i = 1; i < TAILLEDIODE; i++) {
            ptdiode->SUIV = ptdiode + 1;
            ptdiode++;
        }
        ptdiode->SUIV = NULL;
    }

    if ((SPICE_KEEP_NAMES & KEEP__DIODE) == KEEP__DIODE) {
        if (SPICE_KEEP_CARDS & KEEP__DIODE)
          diodename = spicenamealloc (ptcir, (char *)(ligne->DATA));
        else
          diodename = spicenamealloc (ptcir, (char *)(ligne->DATA) + 1);

        if (SPI_CHECK_NAME) {
            scandiode = getthashelem (diodename, ptcir->HASHDIODE, NULL);

            if (scandiode)
                avt_errmsg (SPI_ERRMSG, "058", AVT_FATAL, df->filename, df->linenum, diodename);

            addthashelem (diodename, ptdiode, ptcir->HASHDIODE);
        }
    }
    else
        diodename = NULL;

    if (mbk_CheckREGEX (&ptcir->IGNORE_DIODE, diodename)) {
        avt_log (LOGSTATPRS, 2, "%s:%d: diode '%s' ignored in circuit '%s'\n", df->filename, df->linenum, diodename,
                 ptcir->NOM);
        MBK_PARSE_ERROR.NB_DIODE++;
        return 1;
    }
    ptdiode = ptcir->DIODELIBRE;
    ptcir->DIODELIBRE = ptcir->DIODELIBRE->SUIV;

    ptdiode->P = 0.0;
    ptdiode->A = 0.0;
    ptdiode->PARAM = NULL;

    if (*((char *)(ligne->DATA) + 1) == 0 && (SPICE_KEEP_CARDS & KEEP__DIODE)==0)
        avt_errmsg (SPI_ERRMSG, "057", AVT_WARNING, df->filename, df->linenum);

    ptdiode->NOM = diodename;

    ptdiode->SUIV = ptcir->DIODE;
    ptcir->DIODE = ptdiode;

    elem = ligne->NEXT;

    if (!elem) 
        avt_errmsg (SPI_ERRMSG, "059", AVT_FATAL, df->filename, df->linenum);

    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    ptdiode->N1 = ptnoeud;

    elem = elem->NEXT;

    if (!elem)
        avt_errmsg (SPI_ERRMSG, "059", AVT_FATAL, df->filename, df->linenum);

    ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
    ptdiode->N2 = ptnoeud;

    elem = elem->NEXT;

    if (!elem) 
        avt_errmsg (SPI_ERRMSG, "059", AVT_FATAL, df->filename, df->linenum);

    tmp_name=namealloc((char *)elem->DATA);
    ptdiode->TYPE = tmp_name;

    if (ptcir->ALL_DIODE_MODELS==NULL) ptcir->ALL_DIODE_MODELS=addht(5);
    addhtitem (ptcir->ALL_DIODE_MODELS, tmp_name, 0);

    if (elem->NEXT && elem->NEXT->NEXT && strcmp ((char*)elem->NEXT->NEXT->DATA, "=")) {
        elem = elem->NEXT;
        value = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)(elem->DATA), &paramexpr, ptcir!=NULL?1:0,0);
        ptdiode->PARAM = addoptparam (ptdiode->PARAM, "diode_area", value, paramexpr);
        ptdiode->A = value * SPI_SCALE_DIODEFACTOR * SPI_SCALE_DIODEFACTOR;

        elem = elem->NEXT;
        value = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)(elem->DATA), &paramexpr, ptcir!=NULL?1:0,0);
        ptdiode->PARAM = addoptparam (ptdiode->PARAM, "diode_pj", value, paramexpr);
        ptdiode->P = value * SPI_SCALE_DIODEFACTOR;
    }

    /* Le reste des parametres est de la forme xxx = yyy */

    while ((elem = elem->NEXT)) {
        elem1 = elem->NEXT;
        elem2 = (elem1 ? elem1->NEXT : NULL);

        if (!elem2)
            avt_errmsg (SPI_ERRMSG, "061", AVT_FATAL, df->filename, df->linenum);

        if (strcmp ((char *)(elem1->DATA), "="))
            avt_errmsg (SPI_ERRMSG, "061", AVT_FATAL, df->filename, df->linenum);

        value = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)(elem2->DATA), &paramexpr, ptcir!=NULL?1:0,0);

        if (strcasecmp ((char *)(elem->DATA), "AREA") == 0) {
            ptdiode->PARAM = addoptparam (ptdiode->PARAM, "diode_area", value, paramexpr);
            ptdiode->A = value * SPI_SCALE_DIODEFACTOR * SPI_SCALE_DIODEFACTOR;
        }
        else if ((strcasecmp ((char *)(elem->DATA), "PJ") == 0) || (strcasecmp ((char *)(elem->DATA), "PERI") == 0)) {
            ptdiode->PARAM = addoptparam (ptdiode->PARAM, "diode_pj", value, paramexpr);
            ptdiode->P = value * SPI_SCALE_DIODEFACTOR;
        }
        elem = elem->NEXT->NEXT;
    }
    ptdiode->X = com_x;
    ptdiode->Y = com_y;

    return 1;
}

/******************************************************************************/

int spi_parse_comment (circuit * ptcir, spiline * linedata, spifile * df, int process)
{
    int traitecomment;
    chain_list *elem;
    char *cinsname, *cmodelname, *insname;
    icname *pticname;
    chain_list *sc1, *sc2;
    int i;
    noeud *ptnoeud;
    chain_list *ligne;

    ligne = linedata->decomp;

    traitecomment = FALSE;

    elem = ligne->NEXT;

    if ((ptcir != NULL || !process) && elem) {
        if (*((char *)(elem->DATA)) == '|') {
            if (strcasecmp ((char *)(elem->DATA), "|I") == 0) {
                traitecomment = TRUE;
                elem = elem->NEXT;
                if (elem && process) {
                    cinsname = (char *)(elem->DATA);
                    ptnoeud = ajoutenoeud (ptcir, cinsname, 0x2);
                    elem = elem->NEXT;
                    if (elem) {
                        insname = (char *)(elem->DATA);
                        elem = elem->NEXT;
                        if (elem) {
                            cmodelname = (char *)(elem->DATA);

                            CheckNodeSupInfo (ptcir, ptnoeud);

                            pticname = (icname *) spiciralloue (ptcir, sizeof (icname));
                            pticname->SUIV = ptnoeud->SUPINFO->ICNAME;
                            ptnoeud->SUPINFO->ICNAME = pticname;
                            pticname->NOM = spicenamealloc (ptcir, cmodelname);
                            pticname->INSTANCE = spicenamealloc (ptcir, insname);
                        }
                    }
                }
            }
            else if (strcasecmp ((char *)(elem->DATA), "|NET") == 0) {
                traitecomment = TRUE;
                elem = elem->NEXT;
                if (elem && process) {
                    SIGNAME = spicenamealloc (ptcir, (char *)(elem->DATA));
                }
            }
            else if (strcasecmp ((char *)(elem->DATA), "|CONDIR") == 0) {
                traitecomment = TRUE;
                if (process) {
                    do {
                        elem = elem->NEXT;
                        if (elem && elem->NEXT) {
                            int dir = -1;
                            char *c = (char *)elem->NEXT->DATA;
                            if (strcasecmp (c, "in") == 0)
                                dir = IN;
                            else if (strcasecmp (c, "out") == 0)
                                dir = OUT;
                            else if (strcasecmp (c, "inout") == 0)
                                dir = INOUT;
                            else if (strcasecmp (c, "transcv") == 0)
                                dir = TRANSCV;
                            else if (strcasecmp (c, "tristate") == 0)
                                dir = TRISTATE;
                            if (dir != -1)
                                ptcir->INTERF_DIR =
                                    addptype (ptcir->INTERF_DIR, (long)dir,
                                              namealloc (spicenamealloc (ptcir, (char *)elem->DATA)));
                            elem = elem->NEXT;
                        }
                    } while (elem != NULL);
                }
            }
            else if (strcasecmp ((char *)(elem->DATA), "|UNCONNECTED_NET") == 0) {
               if (elem->NEXT!=NULL) {
                  ptnoeud = ajoutenoeud (ptcir, (char *)(elem->NEXT->DATA), 0);
               }
            }
        }
        else if (strcasecmp ((char *)(elem->DATA), SPI_NETNAME) == 0) {
            traitecomment = TRUE;
            if (process) {
                elem = elem->NEXT;
                sc1 = elem;
                sc2 = NULL;

                for (; elem && strcasecmp ((char *)(elem->DATA), "=") != 0; elem = elem->NEXT) {
                    for (i = 0; ((char *)(elem->DATA))[i] >= '0' && ((char *)(elem->DATA))[i] <= '9'; i++);

                    if (((char *)(elem->DATA))[i] == 0) {
                        ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
                        sc2 = addchain (sc2, ptnoeud);
                    }
                    else
                        break;
                }

                if ((elem) && (elem != sc1)) {
                    elem = elem->NEXT;

                    if (elem) {
                        for (sc1 = sc2; sc1; sc1 = sc1->NEXT)
                            nomenoeud (ptcir, (noeud *) (sc1->DATA), (char *)(elem->DATA), df);
                    }
                    freechain (sc2);
                }
            }
        }
        else if (strcasecmp ((char *)(elem->DATA), ".PININFO") == 0) {
            traitecomment = TRUE;
            if (process) {
                do {
                    elem = elem->NEXT;
                    if (elem) {
                        int dir = -1;
                        char *c = strrchr(elem->DATA,':');
                        if (c == NULL) continue;
                        c++;
                        if (strcasecmp (c, "I") == 0) dir = IN;
                        else if (strcasecmp (c, "O") == 0) dir = OUT;
                        else if (strcasecmp (c, "B") == 0) dir = INOUT;
                        if (dir != -1) {
                            *(c - 1) = 0;
                            ptcir->INTERF_DIR = addptype (ptcir->INTERF_DIR, (long)dir, namealloc (spicenamealloc (ptcir, (char *)elem->DATA)));
                        }
                    }
                } while (elem != NULL);
            }
        }
    }

    if (ext_handler_list && !traitecomment) {
        msl_figloaded = (ptcir ? TRUE : FALSE);
        msl_subckt = (ptcir ? ptcir->NOM : NULL);
        msl_line = df->linenum;
        traitecomment = ext_handlers (linedata->file_line, process);
    }
    return traitecomment;
}

/******************************************************************************/
void spi_parse_func_sub(int i, chain_list * ligne, spifile * df)
{
  chain_list *cl;
  char buf[1024];
  cl=morecut(ligne->DATA, buf);
  if (eqt_adddotfunc(GLOBAL_CTX, (char *)cl->DATA, cl->NEXT, spi_stripquotes(i==2?ligne->NEXT->DATA:ligne->NEXT->NEXT->DATA))==0)
     avt_errmsg (SPI_ERRMSG, "074", AVT_ERROR, df->filename, df->linenum, (char *)cl->DATA);
  freechain(cl);
}

void spi_parse_func (circuit * ptcir, chain_list * ligne, spifile * df)
{
  int i;
  if ((i=countchain(ligne))==2 || (i==3 && strcmp((char *)ligne->NEXT->DATA,"=")==0))
  {
    spi_parse_func_sub(i, ligne, df);
  }
  else
    avt_errmsg (SPI_ERRMSG, "075", AVT_ERROR, df->filename, df->linenum);
  ptcir = NULL;                // avoid warning
}

int spi_parse_param_or_dist_montecarlo (circuit * ptcir, chain_list * ligne, spifile * df, int isdist)
{
  chain_list *elem, *elem1, *elem2;
  char *paramname;
  ptype_list **pt, *check;
  
  if (isdist) pt=&MBK_MONTE_DIST;
  else pt=&MBK_MONTE_PARAMS;

  elem = ligne;
  while (elem->NEXT) {
    elem1 = elem->NEXT;
    elem2 = (elem1 ? elem1->NEXT : NULL);
    
    if (!elem2)
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
    if (strcmp ((char *)(elem2->DATA), "="))
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
    
    paramname = namealloc((char *)elem1->DATA);
    elem = elem2->NEXT;

    if (elem) {
      for (check=*pt; check!=NULL && check->TYPE!=(long)paramname; check=check->NEXT) ;
      if (check!=NULL)
        {
          mbkfree(check->DATA);
          check->DATA=mbkstrdup(spi_stripquotes((char *)elem->DATA));
        }
      else
        *pt=addptype(*pt, (long)paramname, mbkstrdup(spi_stripquotes((char *)elem->DATA)));
    }
  }
  freechain(ligne);
  return 1;
}

int spi_parse_param (circuit * ptcir, chain_list *ligne, eqt_ctx * ctx, spifile * df, optparam_list ** ptptoptparam, chain_list **linestart)
{
    chain_list *elem, *elem1, *elem2;
    double valeur;
    char *paramname;
    char *paramexpr;
    //chain_list *cl, *ch;

    *linestart = NULL;

    elem = ligne;
    while (elem->NEXT) {
        elem1 = elem->NEXT;
        elem2 = (elem1 ? elem1->NEXT : NULL);

        if (!elem2)
            avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
        if (strcmp ((char *)(elem2->DATA), "="))
            avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);

        paramname = (char *)elem1->DATA;
        elem = elem2->NEXT;

        if (elem) {
            if (strchr(paramname, '(')!=NULL)
              spi_parse_func_sub(3, elem1, df);
            else
            {
              valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)(elem->DATA), &paramexpr, ptcir!=NULL?1:0,0);
              if (!eqt_resistrue (GLOBAL_CTX)) {
                  if (ptptoptparam == NULL) {
                      UNRESOLVED_EXP = addptype (UNRESOLVED_EXP, (long)namealloc (paramname), paramexpr);
                  }
                  else
                      *ptptoptparam = addoptparam (*ptptoptparam, paramname, valeur, paramexpr);
              }
              else
              {
                  if (!finite(valeur))
                     avt_errmsg (SPI_ERRMSG, "076", AVT_ERROR, df->filename, df->linenum, paramname, paramexpr?paramexpr:"?", " : returned NaN or Inf");
                  eqt_addvar (ctx, namealloc (paramname), valeur);
              }
            }
        }
        else
            avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
    }
    freechain(ligne);
    return 1;
    ptcir=NULL;
}

/******************************************************************************/
static int warn_simtoolmodel=0;

int spi_parse_model (circuit * ptcir, chain_list * ligne, spifile * df)
{
  chain_list *elem, *elem1, *elem2;
  double valeur;
  char *paramname;
  char *paramexpr;
  char *modname;
  int modtype;
  int modeltype;
  char *typestr;
  mcc_modellist *ptmodel = NULL;
  mcc_technolist *pttechno;

  if (SPI_IGNORE_MODELS)
    return 0;

  if (!warn_simtoolmodel)
  {
    if (!V_INT_TAB[__SIM_TOOLMODEL].SET)
      {
        avt_set_encrypted_mode(0);
        avt_errmsg (SPI_ERRMSG, "077", AVT_WARNING, V_INT_TAB[__SIM_TOOLMODEL].ENUMSTR_FUNC(V_INT_TAB[__SIM_TOOLMODEL].VALUE));
        avt_set_encrypted_mode(df->encrypted);
      }
    warn_simtoolmodel++;
  }

  elem = ligne;
  elem = elem->NEXT;
  if (!elem)
    avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);
  modname = namealloc ((char *)elem->DATA);
  elem = elem->NEXT;
  if (!elem)
    avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);

  typestr = (char *)elem->DATA;
  if (strncasecmp (typestr, "nmos", 4) == 0) {
    modtype = MCC_NMOS;
    if (!mbk_istransn (modname))
      TNMOS = addchain (TNMOS, modname);
      if (df->encrypted) CRYPTMOS = addchain (CRYPTMOS, modname);
  }
  else if (strncasecmp (typestr, "pmos", 4) == 0) {
    modtype = MCC_PMOS;
    if (!mbk_istransp (modname))
      TPMOS = addchain (TPMOS, modname);
      if (df->encrypted) CRYPTMOS = addchain (CRYPTMOS, modname);
  }
  //else if (strcasecmp(typestr, "d") == 0) {
  else if (tolower(typestr[0])=='d') {
    if (!mbk_isdioden (modname) && !mbk_isdiodep (modname))
      DNMOS = addchain (DNMOS, modname);
    modtype = MCC_DIODE;
  }
#if 0
  else if (tolower(typestr[0])=='r') {
    if (!getchain(RMODEL, modname))
      RMODEL = addchain (RMODEL, modname);
    modtype = MCC_MODEL_RESI;
  }
  else if (tolower(typestr[0])=='c') {
    if (!getchain(CMODEL, modname))
      CMODEL = addchain (CMODEL, modname);
    modtype = MCC_MODEL_CAPA;
  }
#endif  
   else if (strcasecmp (typestr, "njf") == 0) {
    if (!mbk_isjfetn (modname))
      JFETN = addchain (JFETN, modname);
    modtype = MCC_UNKNOWN;
  }
  else if (strcasecmp (typestr, "pjf") == 0) {
    if (!mbk_isjfetp (modname))
      JFETP = addchain (JFETP, modname);
    modtype = MCC_UNKNOWN;
  }
  else
    modtype = MCC_UNKNOWN;

  if (modtype == MCC_UNKNOWN)
    return 0;
  if ((pttechno = mcc_gettechno (MCC_MODELFILE)) == NULL) {
    pttechno = mcc_addtechno (MCC_MODELFILE);
  }
  if (ptcir != NULL) {
    ptmodel = mcc_addmodel (MCC_MODELFILE, modname, namealloc (ptcir->NOM), modtype, SPI_MODEL_CORNER);
    ptcir->HASMODEL = 1;
  }
  else
    ptmodel = mcc_addmodel (MCC_MODELFILE, modname, NULL, modtype, SPI_MODEL_CORNER);

  while (elem && elem->NEXT) {
    elem1 = elem->NEXT;
    elem2 = (elem1 ? elem1->NEXT : NULL);

    if (!elem2)
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
    if (strcmp ((char *)(elem2->DATA), "="))
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);

    paramname = (char *)elem1->DATA;
    ptmodel->origparams=addchain(ptmodel->origparams, namealloc(paramname));
    elem = elem2->NEXT;

    if (!strcasecmp(paramname, "VERSION") && !strcmp((char *)(elem->DATA),"4.4.0")) {
      mcc_setparam (ptmodel, paramname, 4.4);
    }
    else if (elem) {
      valeur = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)(elem->DATA), &paramexpr, ptcir!=NULL?1:0,0);
      mcc_setparam (ptmodel, paramname, valeur);
      if (!eqt_resistrue (GLOBAL_CTX)) {
        mcc_paramlist *pt = mcc_chrparam (ptmodel, paramname);
        pt->EXPR = mcc_addexp (pt->EXPR, paramname, paramexpr, valeur);
      }
    }
    else
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
  }

  if (ptmodel) {
    if( typestr && strlen( typestr ) > 4 )
      modeltype = mcc_get_modeltype (ptmodel,typestr+4);
    else
      modeltype = mcc_get_modeltype (ptmodel,NULL);
    ptmodel->MODELTYPE = modeltype;
  }
  return 1;
}


int spi_parse_model_montecarlo (circuit * ptcir, chain_list * ligne, spifile * df)
{
  chain_list *elem, *elem1, *elem2;
  char *paramname;
  char *modname;
  int modtype;
  char *typestr, *subcktname=NULL, *mckey, where;
  ptype_list *mcparam, *pt;
  long l;

  if (SPI_IGNORE_MODELS)
    return 0;

  if (!warn_simtoolmodel)
  {
    if (!V_STR_TAB[__SIM_TOOLMODEL].SET)
      avt_errmsg (SPI_ERRMSG, "077", AVT_WARNING, V_INT_TAB[__SIM_TOOLMODEL].ENUMSTR_FUNC(V_INT_TAB[__SIM_TOOLMODEL].VALUE));
    warn_simtoolmodel++;
  }

  elem = ligne;
  elem = elem->NEXT;
  if (!elem)
    avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);
  modname = namealloc ((char *)elem->DATA);
  elem = elem->NEXT;
  if (!elem)
    avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);

  if (!strcasecmp((char *)elem->DATA, "SUBCKT")) {
    elem = elem->NEXT;
    if (!elem)
      avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);
    if (strcmp ((char *)(elem->DATA), "="))
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
    elem = elem->NEXT;
    if (!elem)
      avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);
    subcktname=namealloc((char *)elem->DATA);
    elem = elem->NEXT;
    if (!elem)
      avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);
  }

  typestr = (char *)elem->DATA;
  if (strncasecmp (typestr, "nmos", 4) == 0) {
    modtype = MCC_NMOS;
  }
  else if (strncasecmp (typestr, "pmos", 4) == 0) {
    modtype = MCC_PMOS;
  }
  //else if (strcasecmp(typestr, "d") == 0) {
  else if (tolower(typestr[0])=='d') {
    modtype = MCC_DIODE;
  }
  else if (strcasecmp (typestr, "njf") == 0) {
    modtype = MCC_UNKNOWN;
  }
  else if (strcasecmp (typestr, "pjf") == 0) {
    modtype = MCC_UNKNOWN;
  }
  else if (strcasecmp (typestr, "sub_mod") == 0 || strcasecmp (typestr, "subckt_model") == 0) {
    modtype = 555;
    if (subcktname!=NULL)
      {
        char buf[256];
        sprintf(buf, "%s::%s", subcktname, modname);
        modname = namealloc (buf);
        subcktname=NULL;
      }    
  } else
    modtype = MCC_UNKNOWN;
    
  if (modtype == MCC_UNKNOWN)
    return 0;

  elem = elem->NEXT;
  if (!elem)
    avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);

  if (!strcasecmp((char *)elem->DATA, "GLOBAL")) {
    where='G';
  } else if (!strcasecmp((char *)elem->DATA, "LOCAL")) {
    where='L';
  } else {
    avt_errmsg (SPI_ERRMSG, "067", AVT_FATAL, df->filename, df->linenum);
  }

  if (MBK_MONTE_MODEL_PARAMS==NULL)
    MBK_MONTE_MODEL_PARAMS=addht(10);

  mckey=mbk_montecarlo_key(modname, subcktname, modtype, where); 
  if ((l=gethtitem(MBK_MONTE_MODEL_PARAMS, mckey))!=EMPTYHT) mcparam=(ptype_list *)l;
  else mcparam=NULL;

  while (elem && elem->NEXT) {
    elem1 = elem->NEXT;
    elem2 = (elem1 ? elem1->NEXT : NULL);

    if (!elem2)
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
    if (strcmp ((char *)(elem2->DATA), "="))
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);

    paramname = namealloc((char *)elem1->DATA);
    elem = elem2->NEXT;

    if (elem) {
      for (pt=mcparam; pt!=NULL && pt->TYPE!=(long)paramname; pt=pt->NEXT) ;
      if (pt!=NULL)
        {
          pt->DATA=namealloc(spi_stripquotes((char *)elem->DATA));
        }
      else
        mcparam=addptype(mcparam, (long)paramname, namealloc(spi_stripquotes((char *)elem->DATA)));
    }
    else
      avt_errmsg (SPI_ERRMSG, "066", AVT_FATAL, df->filename, df->linenum);
  }

  addhtitem(MBK_MONTE_MODEL_PARAMS, mckey, (long)mcparam);
  return 1;
}


/******************************************************************************/
static void spi_include_file(char *name, chain_list **fifodf, spifile **df)
{
  char *ptname, *tofree, *pt;
  char buf[2048];

  mbk_substenvvariable(name, buf);
  tofree=ptname=mbkstrdup(buf);
  if (*ptname == '\'')
    ptname++;
  if (ptname[strlen (ptname) - 1] == '\'')
    ptname[strlen (ptname) - 1] = 0;
  ptname = strip_space (ptname);
  pt = sensitive_namealloc (ptname);
  *df = spifileopen (pt, NULL,
                    (SPI_PARSE_FIRSTLINE == PARSEFIRSTLINE_YES
                     || SPI_PARSE_FIRSTLINE == PARSEFIRSTLINE_INC) ? 1 : 0);
  *fifodf = addchain (*fifodf, *df);
  mbkfree (ptname);
  mbkfree (tofree);
}

void spi_parse_library (circuit * ptcir, chain_list * ligne, spifile ** df, int *ptactivate, chain_list **fifodf)
{
    chain_list *elem1, *elem2;
    char *ptname;
    char buf[2048];

    elem1 = ligne->NEXT;

    if (!elem1)
        avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, (*df)->filename, (*df)->msl_line);

    elem2 = elem1->NEXT;
    if (elem2 && *ptactivate == 1) {
        /*if (elem2->NEXT)
            avt_errmsg (SPI_ERRMSG, "014", AVT_FATAL, df->filename, df->linenum);*/
        if (!strcasecmp((char *)elem1->DATA, "key") && !strcasecmp((char *)elem2->DATA, "=")) {
            if (countchain(elem2->NEXT) != 3) avt_errmsg (SPI_ERRMSG, "013", AVT_FATAL, (*df)->filename, (*df)->msl_line);
            elem1 = elem2->NEXT->NEXT;
            elem2 = elem1->NEXT;
        }
        ptname = (char *)elem1->DATA;
        if (*ptname == '\'')
            ptname++;
        if (ptname[strlen (ptname) - 1] == '\'')
            ptname[strlen (ptname) - 1] = 0;
        SPI_LIBNAME = addptype(SPI_LIBNAME, (long)*df, mbkstrdup((char *)elem2->DATA));
        spi_include_file(ptname, fifodf, df);
        *ptactivate = 0;
/*        ALLINTERF = spiceloading (topfig, ptname, NULL, 'A', ALLINTERF);
        SPI_LIBNAME = NULL;*/
    }
    else {
        if (SPI_LIBNAME != NULL && strcasecmp ((char *)SPI_LIBNAME->DATA, (char *)elem1->DATA) == 0)
            *ptactivate = 1;
    }
}

/******************************************************************************/

void spi_start_decrypt(spifile *df)
{
    int pipefd[2];
    pthread_t thread;
    FILE *pipefp;
    decrypt_files *ptfiles;

    if (df->savedf == NULL) {
        ptfiles = mbkalloc(sizeof(decrypt_files));
        ptfiles->fp = df->df;
        ptfiles->ptlineno = &(df->msl_line);
        if (pipe(pipefd) == -1) {
            avt_errmsg(SPI_ERRMSG, "079", AVT_FATAL, df->filename, df->msl_line);
        }
        ptfiles->pipefd = pipefd[1];
        if (pthread_create(&thread, NULL, mbk_ascii_decrypt_thread, ptfiles) != 0) {
            avt_errmsg(SPI_ERRMSG, "079", AVT_FATAL, df->filename, df->msl_line);
        }
        df->savedf = df->df;
        if ((pipefp = fdopen(pipefd[0], "r")) == NULL) {
            avt_errmsg(SPI_ERRMSG, "079", AVT_FATAL, df->filename, df->msl_line);
        }
        df->df = pipefp;
    }
}

void spi_end_decrypt(spifile *df)
{
    if (df->savedf != NULL) {
        fclose(df->df);
        df->df = df->savedf;
        df->savedf = NULL;
    }
}

/******************************************************************************/
/* On récupère les paramètres non standards : $X, $Y                          */
/******************************************************************************/

int spi_get_xy (chain_list * ligne, float *com_x, float *com_y, int *Tx, int *Ty, int *R, int *A)
{
    float valeur;
    chain_list *sc1, *sc2, *prev, *next;
    eqt_ctx *ctx = NULL;

    *com_x = LONG_MIN;
    *com_y = LONG_MIN;
    *Tx = *Ty = *R = *A = 0;

    for (sc1 = ligne; sc1; sc1 = sc1->NEXT) {
        if (!sc1->DATA)
            continue;
        if (strcasecmp ((char *)(sc1->DATA), "$X") == 0 && sc1->NEXT && sc1->NEXT->NEXT) {
            if (!sc1->NEXT->DATA)
                continue;
            if (strcmp ((char *)(sc1->NEXT->DATA), "="))
                continue;
            if (!ctx)
                ctx = spi_init_eqt ();
            if (!sc1->NEXT->NEXT->DATA)
                continue;
            valeur = eqt_eval (ctx, namealloc((char *)(sc1->NEXT->NEXT->DATA)), EQTFAST);
            if (!eqt_resistrue (ctx))
                continue;
            *com_x = valeur;
        }
        else if (strcasecmp ((char *)(sc1->DATA), "$Y") == 0 && sc1->NEXT && sc1->NEXT->NEXT) {
            if (!sc1->NEXT->DATA)
                continue;
            if (strcmp ((char *)(sc1->NEXT->DATA), "="))
                continue;
            if (!ctx)
                ctx = spi_init_eqt ();
            if (!sc1->NEXT->NEXT->DATA)
                continue;
            valeur = eqt_eval (ctx, namealloc((char *)(sc1->NEXT->NEXT->DATA)), EQTFAST);
            if (!eqt_resistrue (ctx))
                continue;
            *com_y = valeur;
        }
        else if (strcasecmp ((char *)(sc1->DATA), "$T") == 0
                 && sc1->NEXT
                 && sc1->NEXT->NEXT
                 && sc1->NEXT->NEXT->NEXT && sc1->NEXT->NEXT->NEXT->NEXT && sc1->NEXT->NEXT->NEXT->NEXT->NEXT) {
            if (!sc1->NEXT->DATA)
                continue;
            if (strcmp ((char *)(sc1->NEXT->DATA), "="))
                continue;
            *Tx = atoi ((char *)sc1->NEXT->NEXT->DATA);
            *Ty = atoi ((char *)sc1->NEXT->NEXT->NEXT->DATA);
            *R = atoi ((char *)sc1->NEXT->NEXT->NEXT->NEXT->DATA);
            *A = atoi ((char *)sc1->NEXT->NEXT->NEXT->NEXT->NEXT->DATA);
        }
    }

    if (ctx != NULL)
        eqt_term (ctx);

    /* free comment between $ and the end of the line */
    prev = NULL;
    for (sc1 = ligne; sc1; sc1 = next) {
        if (!sc1->DATA) {
            next = sc1->NEXT;
            sc1->NEXT = NULL;
            freechain (sc1);
            if (prev)
                prev->NEXT = next;
        }
        else {
            if (*((char *)(sc1->DATA)) == SPI_COM_CHAR) {
                for (sc2 = sc1->NEXT; sc2 && sc2->DATA; sc2 = sc2->NEXT);
                if (sc2) {
                    next = sc2->NEXT;
                    sc2->NEXT = NULL;
                }
                else
                    next = NULL;
                freechain (sc1);
                if (prev)
                    prev->NEXT = next;
            }
            else {
                next = sc1->NEXT;
                prev = sc1;
            }
        }
    }

    /* free end of line */

    if (!prev) {
        return 0;
    }
    return 1;
}

/******************************************************************************/

circuit *spi_parse_subckt (chain_list ** line0, spifile * df, int *ptblackboxed, chain_list * ptcir_stack)
{
    circuit *ptcir;
    chain_list *elem, *prevelem, *ptchain, *namelist, *ligne=*line0;
    noeud *ptnoeud;
    inffig_list *ifl;
    char cirname[1024];

    ptcir = (circuit *) mbkalloc (sizeof (circuit));
    ptcir->TRANS = NULL;
    ptcir->DIODE = NULL;
    ptcir->RESI = NULL;
    ptcir->CAPA = NULL;
    ptcir->INST = NULL;
    ptcir->NOM = NULL;
    ptcir->CINTERF = NULL;
    ptcir->FREE = NULL;
    ptcir->VALIM = NULL;
    ptcir->NOEUDLIBRE = NULL;
    ptcir->INSTLIBRE = NULL;
    ptcir->DIODELIBRE = NULL;
    ptcir->CTX = NULL;
    ptcir->PARAM = NULL;
    ptcir->LOCAL_PARAMS = NULL;
    ptcir->HASMODEL = 0;
    ptcir->FAILNAME = NULL;
    CreateNameAllocator (100, &ptcir->HASHGNAME, CASE_SENSITIVE);    // = creatthash ();
    CreateHeap (sizeof (insparam_list), 8096, &ptcir->INSPARAMLIST);
    ptcir->HASHCAPA = creatthash ();
    ptcir->HASHRESI = creatthash ();
    ptcir->HASHVALIM = creatthash ();
    ptcir->HASHINST = creatthash ();
    ptcir->HASHTRAN = creatthash ();
    ptcir->HASHDIODE = creatthash ();
    ptcir->TAILLE = sizeof (circuit);
    ptcir->RESTENOM = 0;
    ptcir->INTERF_DIR = NULL;
    ptcir->ALL_TRANSISTOR_MODELS = addht (5);
    ptcir->ALL_INSTANCE_MODELS = addht (15);
    ptcir->ALL_DIODE_MODELS=NULL;
    ptcir->ALL_CAPA_MODELS=NULL;
    ptcir->ALL_RESI_MODELS=NULL;
    ptcir->ana = CreateAdvancedNameAllocator (CASE_SENSITIVE);
    ptcir->ata = CreateAdvancedTableAllocator (100, sizeof (noeud));
    ptcir->nbnodes = 0;
    ptcir->aba = CreateAdvancedBlockAllocator (10000, 'y');
    ptcir->sf = df;
    ptcir->all_equi = NULL;
    ptcir->pass = 0;
    ptcir->TOTAL_LINE_COUNTER = 0;
    ptcir->SUBCIRCUITS = NULL;
    ptcir->UNSOLVED_VCARDS=NULL;

    CreateHeap (sizeof (__equi_), 1000, &ptcir->ha_equi);

    elem = ligne->NEXT;

    if (!elem)
        avt_errmsg (SPI_ERRMSG, "016", AVT_FATAL, df->filename, df->linenum);

    strcpy (cirname, "");
    if (ptcir_stack != NULL) {
        circuit *lastcir;
        strcat (cirname, ((circuit *) ptcir_stack->DATA)->NOM);
        strcat (cirname, "::");
        strcat (cirname, (char *)elem->DATA);
        lastcir = (circuit *) ptcir_stack->DATA;
        addhtitem (lastcir->SUBCIRCUITS, namealloc(spicenamealloc (lastcir, (char *)elem->DATA)),
                   (long)namealloc(spicenamealloc (lastcir, cirname)));
    }
    else
        strcpy (cirname, (char *)elem->DATA);

    ptcir->NOM = (char *)spiciralloue (ptcir, sizeof (char) * (strlen (cirname) + 1));
    strcpy (ptcir->NOM, cirname);

    if ((SPICE_SF & SF_KEEP_BBOX) == 0 && IsInBlackList (ptcir->NOM))
        *ptblackboxed = 1;

    if ((ifl = getloadedinffig (ptcir->NOM)) != NULL) {
        infClean (ifl, INF_DEFAULT_LOCATION);
    }

    inf_buildmatchrule (ifl, INF_IGNORE_RESISTANCE, &ptcir->IGNORE_RESISTANCE, 1);
    inf_buildmatchrule (ifl, INF_IGNORE_CAPACITANCE, &ptcir->IGNORE_CAPACITANCE, 1);
    inf_buildmatchrule (ifl, INF_IGNORE_DIODE, &ptcir->IGNORE_DIODE, 1);
    inf_buildmatchrule (ifl, INF_IGNORE_TRANSISTOR, &ptcir->IGNORE_TRANSISTOR, 1);
    inf_buildmatchrule (ifl, INF_IGNORE_INSTANCE, &ptcir->IGNORE_INSTANCE, 1);
    inf_buildmatchrule (ifl, INF_IGNORE_NAMES, &ptcir->IGNORE_NAMES, 1);

    /* Pour verifier qu'on a pas plusieurs fois le meme noeud, on
     * met un 1 dans le champs RCN */
    prevelem = elem;
    for (elem = elem->NEXT; elem; elem = elem->NEXT) {
        if (elem->NEXT != NULL && strcmp ((char *)elem->NEXT->DATA, "=") == 0) {
            elem = prevelem;
            break;
        }
        if (strcasecmp ((char *)elem->DATA, "PARAM:") == 0)
            break;
        if ((namelist = spi_expandvector (ptcir, (char *)(elem->DATA))) != NULL) {
            for (ptchain = namelist; ptchain; ptchain = ptchain->NEXT) {
                ptnoeud = ajoutenoeud (ptcir, (char *)(ptchain->DATA), 0);
                _spispef_setnodename (ptcir, ptnoeud, NULL, namealloc ((char *)(ptchain->DATA)));
                ptnoeud->RCN = 1;
                ptcir->CINTERF = addchain (ptcir->CINTERF, ptnoeud);
            }
            freechain (namelist);
        }
        else {
            ptnoeud = ajoutenoeud (ptcir, (char *)(elem->DATA), 0);
            _spispef_setnodename (ptcir, ptnoeud, NULL, namealloc ((char *)(elem->DATA)));

            ptnoeud->RCN = 1;

            ptcir->CINTERF = addchain (ptcir->CINTERF, ptnoeud);
        }
        prevelem = elem;
    }

    if (elem) {
        ptcir->CTX = spi_init_eqt ();
        for (prevelem=ligne; prevelem->NEXT!=elem; prevelem=prevelem->NEXT);
        prevelem->NEXT=NULL;
        freechain(ligne);
        spi_parse_param (ptcir, elem, ptcir->CTX, df, &(ptcir->PARAM), line0);
        ptcir->PARAM = (optparam_list *) reverse ((chain_list *) ptcir->PARAM);
    }

    ptcir->CINTERF = reverse (ptcir->CINTERF);

    return ptcir;

}

/******************************************************************************/

void spi_parse_ends (circuit * ptcir, lofig_list * topfig, chain_list * ligne, allinterf * ALLINTERF, char *complet,
                     spifile * df)
{
    lofig_list *modfig;
    locon_list *ptlocon;
    chain_list *namelist;
    inst *scaninst;
    ginterf *scaninterf;
    char buf[1024];
    char *checkpart;
    long l;

    if (!ptcir)
        avt_errmsg (SPI_ERRMSG, "022", AVT_FATAL, df->filename, df->linenum);
    if (!ligne->NEXT) {
    }
    else {
        if ((checkpart = strrchr (ptcir->NOM, ':')) == NULL || *(checkpart - 1) != ':')
            checkpart = ptcir->NOM;
        else
            checkpart++;
        if (strcasecmp ((char *)(ligne->NEXT->DATA), checkpart) != 0)
            avt_errmsg (SPI_ERRMSG, "024", AVT_FATAL, df->filename, df->linenum);

        if (ligne->NEXT->NEXT)
            avt_errmsg (SPI_ERRMSG, "014", AVT_FATAL, df->filename, df->linenum);
    }

    *complet = 1;
    /* Vérification de l'existence des modèles pour les instances */

    for (scaninst = ptcir->INST; scaninst; scaninst = scaninst->SUIV) {
        if ((l = gethtitem (ALLINTERF->h, namealloc (scaninst->MODELE))) == EMPTYHT)
            scaninterf = NULL;
        else
            scaninterf = (ginterf *) l;

        if (!scaninterf && (modfig = getloadedlofig (scaninst->MODELE)) != NULL) {
            namelist = NULL;
            for (ptlocon = modfig->LOCON; ptlocon; ptlocon = ptlocon->NEXT) {
                namelist = addchain (namelist, ptlocon->NAME);
            }
            ALLINTERF = spi_addlofiginterf (modfig->NAME, namelist, ALLINTERF);
            scaninterf = ALLINTERF->teteinterf;
        }

        if (!scaninterf && SPI_AUTO_LOAD) {
            // On cherche un fichier toto.spi qui correspond à l'instance toto
            sprintf (buf, "%s.%s", scaninst->MODELE, SPI_SUFFIX);
            ALLINTERF = traiteinclude (ALLINTERF, buf, topfig);
        }
    }
}

/******************************************************************************/

static circuit *spi_create_circuit (char *name)
{
    circuit *ptcir;
    inffig_list *ifl;

    ptcir = (circuit *) mbkalloc (sizeof (circuit));
    ptcir->TRANS = NULL;
    ptcir->DIODE = NULL;
    ptcir->RESI = NULL;
    ptcir->CAPA = NULL;
    ptcir->INST = NULL;
    ptcir->NOM = NULL;
    ptcir->CINTERF = NULL;
    ptcir->FREE = NULL;
    ptcir->VALIM = NULL;
    ptcir->NOEUDLIBRE = NULL;
    ptcir->INSTLIBRE = NULL;
    ptcir->DIODELIBRE = NULL;
    ptcir->CTX = NULL;
    ptcir->PARAM = NULL;
    ptcir->LOCAL_PARAMS = NULL;
    ptcir->HASMODEL = 0;
    ptcir->FAILNAME = NULL;
    CreateNameAllocator (100, &ptcir->HASHGNAME, CASE_SENSITIVE);    // = creatthash ();
    CreateHeap (sizeof (insparam_list), 8096, &ptcir->INSPARAMLIST);
    ptcir->HASHCAPA = creatthash ();
    ptcir->HASHRESI = creatthash ();
    ptcir->HASHVALIM = creatthash ();
    ptcir->HASHINST = creatthash ();
    ptcir->HASHTRAN = creatthash ();
    ptcir->HASHDIODE = creatthash ();
    ptcir->TAILLE = sizeof (circuit);
    ptcir->RESTENOM = 0;
    ptcir->INTERF_DIR = NULL;
    ptcir->ALL_TRANSISTOR_MODELS = addht (5);
    ptcir->ALL_INSTANCE_MODELS = addht (15);
    ptcir->ALL_DIODE_MODELS=NULL;
    ptcir->ALL_CAPA_MODELS=NULL;
    ptcir->ALL_RESI_MODELS=NULL;
    ptcir->ana = CreateAdvancedNameAllocator (CASE_SENSITIVE);
    ptcir->ata = CreateAdvancedTableAllocator (100000, sizeof (noeud));
    ptcir->nbnodes = 0;
    ptcir->aba = CreateAdvancedBlockAllocator (10000, 'y');
    ptcir->sf = NULL;
    ptcir->all_equi = NULL;
    ptcir->pass = 0;
    ptcir->TOTAL_LINE_COUNTER = 0;
    ptcir->SUBCIRCUITS = NULL;
    ptcir->UNSOLVED_VCARDS=NULL;

    CreateHeap (sizeof (__equi_), 1000, &ptcir->ha_equi);

    name = spi_getbasename (name);
    ptcir->NOM = (char *)spiciralloue (ptcir, sizeof (char) * (strlen (name) + 1));
    strcpy (ptcir->NOM, name);

    if ((ifl = getloadedinffig (ptcir->NOM)) != NULL) {
        infClean (ifl, INF_DEFAULT_LOCATION);
    }

    inf_buildmatchrule (ifl, INF_IGNORE_RESISTANCE, &ptcir->IGNORE_RESISTANCE,1);
    inf_buildmatchrule (ifl, INF_IGNORE_CAPACITANCE, &ptcir->IGNORE_CAPACITANCE,1);
    inf_buildmatchrule (ifl, INF_IGNORE_DIODE, &ptcir->IGNORE_DIODE,1);
    inf_buildmatchrule (ifl, INF_IGNORE_TRANSISTOR, &ptcir->IGNORE_TRANSISTOR,1);
    inf_buildmatchrule (ifl, INF_IGNORE_INSTANCE, &ptcir->IGNORE_INSTANCE,1);
    inf_buildmatchrule (ifl, INF_IGNORE_NAMES, &ptcir->IGNORE_NAMES,1);


    return ptcir;

}

/******************************************************************************/

static int spi_empty_circuit (circuit * ptcir)
{
    if (ptcir == NULL)
        return 1;
    if (!ptcir->TRANS && !ptcir->DIODE && !ptcir->RESI && !ptcir->CAPA && !ptcir->INST && !ptcir->VALIM) {
        return 1;
    }
    return 0;
}

/******************************************************************************/

static void spi_create_top_interface (lofig_list * ptfig)
{
    losig_list *ptlosig;
    locon_list *ptnewlocon;
    ptype_list *ptuser;
    char *name;

    if (ptfig == NULL)
        return;
    for (ptlosig = ptfig->LOSIG; ptlosig; ptlosig = ptlosig->NEXT) {
        ptlosig->TYPE = EXTERNAL;
        if ((ptuser = getptype (ptlosig->USER, TOPEXTNAME)) != NULL) {
            name = (char *)ptuser->DATA;
            ptlosig->USER = delptype (ptlosig->USER, TOPEXTNAME);
        }
        else
            name = ptlosig->NAMECHAIN->DATA;
        ptnewlocon = addlocon (ptfig, name, ptlosig, UNKNOWN);
//        freechain (ptlosig->NAMECHAIN);
        if (getchain(ptlosig->NAMECHAIN, name)==NULL)
          ptlosig->NAMECHAIN = addchain (ptlosig->NAMECHAIN, name);
        if (ptlosig->PRCN) {
            if ((ptuser = getptype (ptlosig->USER, TOPEXTNODE)) != NULL) {
                setloconnode (ptnewlocon, (int)(long)ptuser->DATA);
                ptlosig->USER = delptype (ptlosig->USER, TOPEXTNODE);
            }
            else
                setloconnode (ptnewlocon, 1);
            ptnewlocon->USER = addptype (ptnewlocon->USER, PNODENAME, addchain (NULL, name));
        }
    }
    lofigchain (ptfig);
    spi_post_traitment_connector_direction (ptfig);
    mbk_set_transistor_instance_connector_orientation_info (ptfig);
}

/******************************************************************************/

#ifdef ENABLE_STATS
extern long ___intcnt;
extern long ____nbequi;
extern long ____nbequiadd, ____nbequisup;

void spi_stat (circuit * ptcir, spifile * df, long *df_temp, long *df_time)
{
    long df_temp0;

    if (++*df_temp >= 1024) {
        if ((df_temp0 = time (NULL)) != *df_time) {
            char temp[1024];
            *df_time = df_temp0;
            *df_temp = 0;

            if (avt_terminal (stdout)) {
                long mem_aba = 0, mem_ata = 0, mem_ana = 0;
                long mem_ana_na = 0, mem_ana_na_names = 0, mem_ana_aba = 0, mem_ana_ata = 0;
                long mem_num = 0, mem_equi = 0, mem_tot = 0, mem_equi_nb = 0;
                if (ptcir != NULL) {
                    mem_aba = ptcir->aba->__SIZE__ / (1024 * 1024);
                    mem_ata = ptcir->ata->__SIZE__ / (1024 * 1024);
                    mem_ana_na = ptcir->ana->na.__SIZE__ / (1024 * 1024);
                    mem_ana_na_names = ptcir->ana->na.__NBNAMES__;
                    mem_ana_aba = ptcir->ana->aba->__SIZE__ / (1024 * 1024);
                    mem_ana_ata = ptcir->ana->ata->__SIZE__ / (1024 * 1024);
                    mem_num = ___intcnt / (1024 * 1024);
                    mem_equi = ptcir->ha_equi.__SIZE__ / (1024 * 1024);
                    mem_tot = mem_aba + mem_ata + mem_ana_na + mem_ana_aba + mem_ana_ata + mem_equi;
                    mem_equi_nb = ____nbequi;
                }

                sprintf (temp,
                         "[¤+%08d¤.] aba:%ld ata:%ld [na:%ld|¤1%ld¤. aba:%ld ata:%ld num:%ld] eq:%ld|¤1%ld¤. ~%ldmo",
                         df->linenum, mem_aba, mem_ata, mem_ana_na, mem_ana_na_names, mem_ana_aba, mem_ana_ata,
                         mem_num, mem_equi, mem_equi_nb, mem_tot);
                avt_fprintf (stdout, temp);
                fflush (stdout);
                avt_back_fprintf (stdout, strlen (temp) - 4 - 4 - 4);
            }
        }
        *df_temp = 0;
    }
}

#endif
int spi_parse_options (circuit * ptcir, chain_list * ligne, spifile * df)
{
    double val;
    char *paramexpr;
    char buf[1024];
    inffig_list *ifl;

    if (ptcir != NULL && (ifl = getloadedinffig (ptcir->NOM)) == NULL)
        ifl = addinffig (ptcir==SPI_TOPCIR?TOPCIRINFNAME:ptcir->NOM);
    else
        ifl = NULL;

    do {
        if (ligne == NULL || ligne->NEXT == NULL || ligne->NEXT->NEXT == NULL
            || strcmp ((char *)ligne->NEXT->DATA, "=") != 0) {
            // .option <item>
            if (ligne != NULL)
                ligne = ligne->NEXT;
        }
        else {
            // .option <item>=<expr>
            if (strcasecmp (ligne->DATA, "temp") == 0) {
                // set temperature in INF
                val = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)ligne->NEXT->NEXT->DATA, &paramexpr, ptcir!=NULL?1:0,0);
                if (paramexpr == NULL) {
                    sprintf (buf, "%s:%d: ", df->filename, df->linenum);
                    if (ifl != NULL) {
                        inf_AddDouble (ifl, INF_DEFAULT_LOCATION, INF_OPERATING_CONDITION, INF_TEMPERATURE, val, buf);
                    }
                    SPI_GLOBAL_TEMP = val;
                    eqt_addvar (GLOBAL_CTX, namealloc ("temper"), SPI_GLOBAL_TEMP);
                }
                else avt_errmsg(SPI_ERRMSG, "052", AVT_WARNING, df->filename, df->linenum, paramexpr);
            }
            else if (strcasecmp (ligne->DATA, "scale") == 0) {
                // set scale
                val = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)ligne->NEXT->NEXT->DATA, &paramexpr, ptcir!=NULL?1:0,0);
                if (paramexpr == NULL) {
                    SPI_GLOBAL_SCALE = val;
                }
                else avt_errmsg(SPI_ERRMSG, "052", AVT_WARNING, df->filename, df->linenum, paramexpr);
            }
            else if (strcasecmp (ligne->DATA, "tnom") == 0) {
                val = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)ligne->NEXT->NEXT->DATA, &paramexpr, ptcir!=NULL?1:0,0);
                if (paramexpr == NULL) {
                    if (V_FLOAT_TAB[__SIM_TNOM].SET!=1)
                    {
                       V_FLOAT_TAB[__SIM_TNOM].VALUE=val;
                       V_FLOAT_TAB[__SIM_TNOM].SET=1;
                    }
                }
                else avt_errmsg(SPI_ERRMSG, "052", AVT_WARNING, df->filename, df->linenum, paramexpr);
            }
            else if (strcasecmp (ligne->DATA, "spice") == 0) {
                if (!V_FLOAT_TAB[__SIM_TNOM].SET)
                {
                   V_FLOAT_TAB[__SIM_TNOM].VALUE=27;
                   V_FLOAT_TAB[__SIM_TNOM].SET=2;
                }
            }
            ligne = ligne->NEXT->NEXT->NEXT;
        }
    } while (ligne != NULL);
    return 0;
}

void spi_parse_global (circuit * ptcir, chain_list * ligne)
{
    while (ligne != NULL) {
        SPI_GLOBAL_NODES = addchain (SPI_GLOBAL_NODES, namealloc (ligne->DATA));
        ligne = ligne->NEXT;
    }
    ptcir = NULL;                // avoid warning
}


int spi_parse_temp (circuit * ptcir, chain_list * ligne, spifile * df)
{
    char *paramexpr;
    double val;
    inffig_list *ifl;
    char buf[1024];

    if (ligne == NULL) {
        avt_errmsg(SPI_ERRMSG, "054", AVT_WARNING, df->filename, df->linenum, ".TEMP");
        return 1;
    }
    if (strcmp (ligne->DATA, "=") == 0)
        ligne = ligne->NEXT;
    if (ligne == NULL) {
        avt_errmsg(SPI_ERRMSG, "054", AVT_WARNING, df->filename, df->linenum, ".TEMP");
        return 1;
    }
    if (ligne->NEXT != NULL) {
        avt_errmsg(SPI_ERRMSG, "055", AVT_WARNING, df->filename, df->linenum, ".TEMP");
    }

    if (ptcir != NULL) {
        char *iname=(ptcir==SPI_TOPCIR)?TOPCIRINFNAME:ptcir->NOM;
        if ((ifl = getloadedinffig (iname)) == NULL)
            ifl = addinffig (iname);
    }
    else
        ifl = NULL;

    val = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)ligne->DATA, &paramexpr, ptcir!=NULL?1:0,0);
    if (eqt_resistrue(GLOBAL_CTX)) {
        sprintf (buf, "%s:%d: ", df->filename, df->linenum);
//      printf("temp=%g\n",val);
        if (ifl != NULL) {
            inf_AddDouble (ifl, INF_DEFAULT_LOCATION, INF_OPERATING_CONDITION, INF_TEMPERATURE, val, buf);
        }
        SPI_GLOBAL_TEMP = val;
        eqt_addvar (GLOBAL_CTX, namealloc ("temper"), SPI_GLOBAL_TEMP);
        return 0;
    }

    avt_errmsg(SPI_ERRMSG, "052", AVT_WARNING, df->filename, df->linenum, spi_stripquotes((char *)ligne->DATA));
    ptcir = NULL;                // avoid warning
    return 1;
}

int spi_parse_dotopt (circuit * ptcir, chain_list * ligne, spifile * df, char *param, double *val)
{
    char *paramexpr;

    if (ligne == NULL) {
        avt_errmsg(SPI_ERRMSG, "054", AVT_WARNING, df->filename, df->linenum, param);
        return 1;
    }
    if (strcmp (ligne->DATA, "=") == 0)
        ligne = ligne->NEXT;
    if (ligne == NULL) {
        avt_errmsg(SPI_ERRMSG, "054", AVT_WARNING, df->filename, df->linenum, param);
        return 1;
    }
    if (ligne->NEXT != NULL) {
        avt_errmsg(SPI_ERRMSG, "055", AVT_WARNING, df->filename, df->linenum, param);
    }

    *val = spi_eval (ptcir?ptcir->sf:NULL, GLOBAL_CTX, (char *)ligne->DATA, &paramexpr, ptcir!=NULL?1:0,0);
    if (paramexpr == NULL)
        return 1;
    else
        avt_errmsg(SPI_ERRMSG, "052", AVT_WARNING, df->filename, df->linenum, (char *)ligne->DATA);
    ptcir = NULL;                // avoid warning
    return 0;
}

int spi_parse_scale (circuit * ptcir, chain_list * ligne, spifile * df)
{
    int ret;
    double val;
    if ((ret = spi_parse_dotopt (ptcir, ligne, df, ".scale", &val)))
        SPI_GLOBAL_SCALE = val;
    return ret;
}

/******************************************************************************/

circuit *lirecircuit (fifodf, ALLINTERF, topfig, ptactivate, globalinfo)
chain_list **fifodf;
allinterf *ALLINTERF;
lofig_list *topfig;
int *ptactivate;
spi_load_global *globalinfo;
{
    char complet;
    chain_list *ligne;
    circuit *ptcir;
    int usetopcir;
    float com_x, com_y;
    int Tx, Ty, R, A;
    spifile *df;
    chain_list *elem;
    char *ptname;
//    int         blackboxed;
//    chain_list *ptcir_stack=NULL;

#ifdef ENABLE_STATS
    long df_time = time (NULL);
    long df_temp = 0;
#endif

    complet = 0;
    if (globalinfo->ptcir_stack != NULL) {
        ptcir = (circuit *) globalinfo->ptcir_stack->DATA;
        globalinfo->ptcir_stack = delchain (globalinfo->ptcir_stack, globalinfo->ptcir_stack);
    }
    else
        ptcir = NULL;

    if (*fifodf == NULL)
        return NULL;

    df = (spifile *) ((*fifodf)->DATA);
    avt_set_encrypted_mode(df->encrypted);

    while (!complet) {
        if (!(ligne = lireligne (df, ptcir))) {
            spifileclose (df);
            elem = (*fifodf);
            *fifodf = (*fifodf)->NEXT;
            elem->NEXT = NULL;
            freechain (elem);
            if ((*fifodf)) {
                df = (spifile *) ((*fifodf)->DATA);
                avt_set_encrypted_mode(df->encrypted);
                if (SPI_LIBNAME!=NULL && SPI_LIBNAME->TYPE==(long)df)
                {
                   mbkfree(SPI_LIBNAME->DATA);
                   SPI_LIBNAME=delptype(SPI_LIBNAME, SPI_LIBNAME->TYPE);
                   *ptactivate =1;
                }
                continue;
            }
            else
                break;            /* fin de fichier */
        }
        if (!spi_get_xy (ligne, &com_x, &com_y, &Tx, &Ty, &R, &A))
            ligne = NULL;

#ifdef ENABLE_STATS
        spi_stat (ptcir, df, &df_temp, &df_time);
#endif
        if (!ptcir && SPI_CREATE_TOP_FIGURE && SPI_TOPCIR)
            usetopcir = 1;
        else
            usetopcir = 0;

        if (ligne) {
            if (*ptactivate == 1 && globalinfo->blackboxed == 0) {
                log_decompligne (df, 0, 2);
                if (df->encrypted) df->encryptedlines++;
            }

            if (strcasecmp ((char *)ligne->DATA, ".LIBRARY") == 0 || strcasecmp ((char *)ligne->DATA, ".LIB") == 0) {
                //spi_parse_library (ptcir, topfig, ligne, ALLINTERF, df, ptactivate);
                spi_parse_library (ptcir, ligne, &df, ptactivate, fifodf);
            }
            else if (*ptactivate == 0) {
                freechain (ligne);
                continue;
            }
            else if (!V_BOOL_TAB[__MBK_SPI_IGNORE_CRYPT].VALUE && strncasecmp ((char *)(ligne->DATA), ".PROT", 5)==0) {
                df->encryptedlines = 0;
                avt_set_encrypted_mode(1);
                df->encrypted = 1;
            }
            else if (!V_BOOL_TAB[__MBK_SPI_IGNORE_CRYPT].VALUE && strncasecmp ((char *)(ligne->DATA), ".UNPROT", 7)==0) {
                avt_set_encrypted_mode(0);
                df->encrypted = 0;
                if (df->encryptedlines == 1) {
                    avt_errmsg (SPI_ERRMSG, "078", AVT_WARNING, df->filename, df->linenum);
                }
            }
            else if (strcasecmp ((char *)(ligne->DATA), ".MCPARAM")==0)
              {
                SPI_MONTE_CARLO_ACTIVE=1;
              }
            else if (strcasecmp ((char *)(ligne->DATA), ".ENDMCPARAM")==0)
              {
                SPI_MONTE_CARLO_ACTIVE=0;
              }
            else if (SPI_MONTE_CARLO_ACTIVE)
              {
                if (strcasecmp ((char *)ligne->DATA, ".MODEL") == 0) {
                  if (V_BOOL_TAB[__AVT_ENABLE_STAT].VALUE) spi_parse_model_montecarlo (ptcir, ligne, df);
                }
                else if (*((char *)(ligne->DATA)) == '*') {
                  if (V_BOOL_TAB[__AVT_ENABLE_STAT].VALUE) spi_parse_comment (ptcir, &(df->lines[0]), df, TRUE);
                }
                else if (strcasecmp ((char *)ligne->DATA, ".PARAM") == 0) {
                  if (V_BOOL_TAB[__AVT_ENABLE_STAT].VALUE) spi_parse_param_or_dist_montecarlo(ptcir, ligne, df, 0);
                }
                else if (strcasecmp ((char *)ligne->DATA, ".DIST") == 0) {
                  if (V_BOOL_TAB[__AVT_ENABLE_STAT].VALUE) spi_parse_param_or_dist_montecarlo(ptcir, ligne, df, 1);
                }
                else {
                  avt_log(LOGSPI, 1, "Ignored in .MCPARAM section: ");
                  log_decompligne (df, 0, 1);
                  avt_log(LOGSPI, 1, "\t(file '%s', line %d)\n", df->filename, df->linenum);
                  if (df->encrypted) df->encryptedlines--;
                }
              }
            else if (strcasecmp ((char *)(ligne->DATA), ".ENDS") == 0 || strcasecmp ((char *)(ligne->DATA), ".EOM") == 0) {
                if (globalinfo->blackboxed > 0)
                    globalinfo->blackboxed--;
                if (globalinfo->blackboxed == 0)
                    spi_parse_ends (ptcir, topfig, ligne, ALLINTERF, &complet, df);
                if (globalinfo->blackboxed) {
                    log_decompligne (df, 0, 2);
                    if (df->encrypted) df->encryptedlines++;
                }
            }
            else if (strcasecmp ((char *)ligne->DATA, ".SUBCKT") == 0 || strcasecmp ((char *)ligne->DATA, ".MACRO") == 0) {
                if (globalinfo->blackboxed > 0)
                    globalinfo->blackboxed++;
                else {
                    if (ptcir != NULL) {
                        if (ptcir->SUBCIRCUITS == NULL)
                            ptcir->SUBCIRCUITS = addht (10);
                        globalinfo->ptcir_stack = addchain (globalinfo->ptcir_stack, ptcir);
                    }
                    ptcir = spi_parse_subckt (&ligne, df, &globalinfo->blackboxed, globalinfo->ptcir_stack);
                    usetopcir = 0;
                }
            }
            else if (globalinfo->blackboxed > 0) {
                freechain (ligne);
                continue;
            }
            else if (strcasecmp ((char *)ligne->DATA, ".ENDL") == 0) {
                *ptactivate = 0;
            }
            else if (strcasecmp ((char *)ligne->DATA, ".INCLUDE") == 0 || strcasecmp ((char *)ligne->DATA, ".INC") == 0
                     || strcasecmp ((char *)ligne->DATA, ".INCL") == 0) {
                if (!ligne->NEXT)
                    avt_errmsg (SPI_ERRMSG, "069", AVT_FATAL, df->filename, df->linenum);
                elem = ligne->NEXT;
                ptname = (char *)elem->DATA;
                spi_include_file(ptname, fifodf, &df);
            }
            else if (strcasecmp ((char *)ligne->DATA, ".PARAM") == 0) {
                if (!ptcir)
                    spi_parse_param (ptcir,ligne, GLOBAL_CTX, df, NULL,&ligne);
                else {
                    if (ptcir->CTX == NULL)
                        ptcir->CTX = spi_init_eqt ();
                    spi_parse_param (ptcir,ligne, ptcir->CTX, df, &(ptcir->LOCAL_PARAMS),&ligne);
                }
            }
            else if (strcasecmp ((char *)ligne->DATA, ".MODEL") == 0) {
                spi_parse_model (ptcir, ligne, df);
            }
            else if (*((char *)(ligne->DATA)) == '*') {
                spi_parse_comment (ptcir, &(df->lines[0]), df, TRUE);
            }
            else if (strchr ("Mm", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_transistor (ptcir, ligne, df, com_x, com_y);
            }
            else if (strchr ("Jj", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_jfet (ptcir, ligne, df, com_x, com_y);
            }
            else if (strchr ("Dd", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_diode (ptcir, ligne, df, com_x, com_y);
            }
            else if (strcasecmp ((char *)ligne->DATA, ".CONNECT") == 0 && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_connect (ptcir, ligne, df);
            }
            else if (strchr ("Rr", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_wire (ptcir, ligne, df);
            }
            else if (strchr ("Ll", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_induc (ptcir, ligne, df);
            }
            else if (strchr ("Cc", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_capa (ptcir, ligne, df);
            }
            else if (strchr ("Vv", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_voltage (ptcir, ligne, df);
            }
            else if (strchr ("Xx", *((char *)ligne->DATA)) && (ptcir || usetopcir)) {
                if (usetopcir) {
                    ptcir = SPI_TOPCIR;
                    ptcir->sf = df;
                }
                spi_parse_instance (ptcir, ligne, df, com_x, com_y, Tx, Ty, R, A);
            }
            else if (*(char *)ligne->DATA == '.') {
                if (strcasecmp ((char *)ligne->DATA, ".option") == 0
                    || strcasecmp ((char *)ligne->DATA, ".options") == 0) {
                    if (usetopcir) {
                        ptcir = SPI_TOPCIR;
                        ptcir->sf = df;
                    }
                    spi_parse_options (ptcir, ligne->NEXT, df);
                }
                else if (strcasecmp ((char *)ligne->DATA, ".temp") == 0) {
                    if (usetopcir) {
                        ptcir = SPI_TOPCIR;
                        ptcir->sf = df;
                    }
                    spi_parse_temp (ptcir, ligne->NEXT, df);
                }
                else if (strcasecmp ((char *)ligne->DATA, ".scale") == 0) {
                    if (usetopcir) {
                        ptcir = SPI_TOPCIR;
                        ptcir->sf = df;
                    }
                    spi_parse_scale (ptcir, ligne->NEXT, df);
                }
                else if (strcasecmp ((char *)ligne->DATA, ".global") == 0) {
                    if (usetopcir) {
                        ptcir = SPI_TOPCIR;
                        ptcir->sf = df;
                    }
                    spi_parse_global (ptcir, ligne->NEXT);
                }
                else if (strcasecmp ((char *)ligne->DATA, ".func") == 0) {
                    if (usetopcir) {
                        ptcir = SPI_TOPCIR;
                        ptcir->sf = df;
                    }
                    spi_parse_func (ptcir, ligne->NEXT, df);
                }

            }
            else {
                avt_log(LOGSPI, 1, "Ignored: ");
                log_decompligne (df, 0, 1);
                avt_log(LOGSPI, 1, "\t(file '%s', line %d)\n", df->filename, df->linenum);
                if (df->encrypted) df->encryptedlines--;
            }
            freechain (ligne);
        }
        if (usetopcir)
            ptcir = NULL;
    }

#ifdef ENABLE_STATS
    fprintf (stdout, "circuit '%s' :\n", ptcir != NULL ? ptcir->NOM : "?");
    fflush (stdout);
    df_temp = 100000;
    df_time = 100000;
    spi_stat (ptcir, df, &df_temp, &df_time);
    fprintf (stdout, "\n");
    fflush (stdout);
#endif

    spi_rename_instance_model (ptcir, globalinfo->ptcir_stack);    // pour subckt dans un subckt

    if (ptcir != NULL && SPI_GLOBAL_TEMP != -DBL_MAX) {
        inffig_list *ifl;
        double val;
        char *iname=(ptcir==SPI_TOPCIR)?TOPCIRINFNAME:ptcir->NOM;
        if (ptcir != NULL && (ifl = getloadedinffig (iname)) == NULL)
            ifl = addinffig (iname);
        if (inf_GetDouble (ifl, INF_OPERATING_CONDITION, INF_TEMPERATURE, &val) == 0) {
            inf_AddDouble (ifl, INF_DEFAULT_LOCATION, INF_OPERATING_CONDITION, INF_TEMPERATURE, SPI_GLOBAL_TEMP, NULL);
        }
    }

    return ptcir;
}

/******************************************************************************/

allinterf *traiteinclude (ALLINTERF, nom, topfig)
allinterf *ALLINTERF;
char *nom;
lofig_list *topfig;
{
    char *pt;

    pt = sensitive_namealloc (nom);
    if (gethtitem (ALLINTERF->h, pt) != EMPTYHT) 
        avt_errmsg (SPI_ERRMSG, "042", AVT_FATAL, nom);

    ALLINTERF = spiceloading (topfig, nom, NULL, 'A', ALLINTERF);

    return (ALLINTERF);
}

/******************************************************************************/
ht *linksameconnectors (chain_list * ginterf_v, chain_list * iinterf_v)
{
    chain_list *lst, *sc2, *sc1;
    ht *h;
    long l;

    h = addht (64);

    for (sc1 = ginterf_v, sc2 = iinterf_v; sc1 && sc2; sc1 = sc1->NEXT, sc2 = sc2->NEXT) {
        
        if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE && (l = gethtitem (h, sc1->DATA)) != EMPTYHT)
            lst = (chain_list *) l;
        else
            lst = NULL;
        lst = addchain (lst, sc2->DATA);
        addhtitem (h, sc1->DATA, (long)lst);
    }

    return h;
}

// herez
chain_list *linksameequiconnectors (circuit * ptcir, chain_list * ginterf_v)
{
    chain_list *lst, *cl, *sc1, *ch, *all;
    char nom0[1024], *rad;
    noeud *n;
    ht *h;
    long l;
    typedef struct bi {
        chain_list *head, *end;
    } bi;
    HeapAlloc ha;
    bi *mybi, abi, *bi2;

    h = addht (10000);
    CreateHeap (sizeof (bi), 10000, &ha);

    for (sc1 = ginterf_v; sc1; sc1 = sc1->NEXT) {
        n = (noeud *) sc1->DATA;
        AdvancedNameAllocName (ptcir->ana, n->index, nom0);
        nom0[get_equi_separ_pos (nom0)] = '\0';
        rad = namealloc (nom0);

        if ((l = gethtitem (h, rad)) != EMPTYHT)
            mybi = (bi *) l;
        else {
            mybi = AddHeapItem (&ha);
            mybi->head = NULL;
            mybi->end = NULL;
        }
        mybi->head = addchain (mybi->head, n);
        if (mybi->end == NULL)
            mybi->end = mybi->head;
        addhtitem (h, rad, (long)mybi);
    }

    lst = GetAllHTElems (h);
    for (cl = lst; cl != NULL; cl = cl->NEXT) {
        mybi = (bi *) cl->DATA;
        for (ch = mybi->head; ch != NULL; ch = ch->NEXT) {
            n = (noeud *) ch->DATA;
            n->signal->items = (__item_ *) addchain ((chain_list *) n->signal->items, mybi);
        }
    }

    all = NULL;
    for (cl = lst; cl != NULL; cl = cl->NEXT) {
        mybi = (bi *) cl->DATA;
        if (mybi->head != NULL) {
            abi.head = mybi->head;
            abi.end = mybi->end;
            mybi->head = NULL;
            for (ch = abi.head; ch != NULL; ch = ch->NEXT) {
                n = (noeud *) ch->DATA;
                if (n->signal->items != NULL) {
                    for (sc1 = (chain_list *) n->signal->items; sc1 != NULL; sc1 = sc1->NEXT) {
                        bi2 = (bi *) sc1->DATA;
                        abi.end->NEXT = bi2->head;
                    }
                    freechain ((chain_list *) n->signal->items);
                    n->signal->items = NULL;
                }
            }
            all = addchain (all, abi.head);
        }
    }

    freechain (lst);


    DeleteHeap (&ha);
    delht (h);
    return all;
}

allinterf *constequi (ptcir, ALLINTERF)
circuit *ptcir;
allinterf *ALLINTERF;
{
    inst *scaninst;
    chain_list *sc1, *sc2, *sc3, *sc4, *sc5, *nodelist;
    ptype_list *pt, *pt2;
    int idsig = 0;                /* Warning a la compil */
    int signal;
    ptype_list *headsig;
    int nb;
    int t;
    ptype_list *spt;
    noeud *scannoeud;
    lowire_list *scanresi;
    valim *scanvalim;
    ginterf *sci;
    char *vdd;
    char *vss;
    int vuevss;
    int vuevdd;
    int cirvss;
    int cirvdd;
    chain_list *scanchain;
    noeud *node1;
    noeud *node2;
    tableint *htab;
    //ht            *htinst;
    ht *htcon;
    int i;
    char *ins;
    char nom0[1024], nom1[1024];
    __equi_ *e;
    noeud *n;
    tableint *signbnode = NULL;
    int l1, l2;
    chain_list *last;
    int nbresi, curresi;
    int maxnode;
    static char dspflinkexternal = 'u';
    char *env;
    char *dspfname;
    long l;

    signal = 1;
    headsig = NULL;
    htab = creattableint ();
    signbnode = creattableint ();

    for (e = ptcir->all_equi; e != NULL; e = e->next) {
        headsig = addptype (headsig, signal, NULL);
        settableint (htab, signal, headsig);
        settableint (signbnode, signal, (void *)(long)e->b.nbnodes);
        for (n = e->nodes; n != NULL; n = n->SUIV) {
            n->signal->b.SIGNAL = signal;
            headsig->DATA = addchain (headsig->DATA, n);
        }
        signal++;
    }

    /*    traite les instances                        */

    if (ptcir->INST) {
        for (scaninst = ptcir->INST; scaninst; scaninst = scaninst->SUIV) {
            sci = (ginterf *) gethtitem (ALLINTERF->h, namealloc (scaninst->MODELE));

            if (sci == (ginterf *) EMPTYHT) {
                ALLINTERF = constinstmodel (ptcir, scaninst, ALLINTERF);
                if (ALLINTERF == NULL)
                    sci = NULL;
                else
                    sci = ALLINTERF->teteinterf;
            }
            else {
                checkinstmodel (ptcir, scaninst, sci);
            }

            if (!sci) {
                spi_displayunresolved (scaninst->MODELE, SPI_UNRESOLVED_CIRCUITS);
                avt_errmsg (SPI_ERRMSG, "043", AVT_FATAL, scaninst->MODELE);
            }

            if (SPI_DRIVE_SPEF_MODE)
                ins = namealloc (scaninst->NOM);

            if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE) htcon = linksameconnectors (sci->GINTERF, scaninst->IINTERF);

            for (sc1 = sci->GINTERF, sc2 = scaninst->IINTERF; sc1 && sc2; sc1 = sc1->NEXT, sc2 = sc2->NEXT) {
                /* On verifie qu'on a une nouvelle equipotentielle */

                if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE)
                {
                if ((l = gethtitem (htcon, sc1->DATA)) == 1)
                    continue;
                nodelist = reverse ((chain_list *) l);
                addhtitem (htcon, sc1->DATA, 1);
                sc3 = sc1;
                }
                else
                {
                nodelist = addchain(NULL, sc2->DATA);
                sc3 = sc2;
                }


                nb = 0;            /* nombre d'equipotentielle deja existant sur l'interface */

                maxnode = -1;

                for (sc4 = nodelist; sc4 != NULL; sc4 = sc4->NEXT) {
                    if (((noeud *) (sc4->DATA))->signal->b.SIGNAL != 0) {
                        nb++;
                        l1 = ((noeud *) (sc4->DATA))->signal->b.SIGNAL;
                        l2 = (int)(long)tsttableint (signbnode, l1);
                        if (l2 > maxnode) {
                            maxnode = l2;
                            idsig = l1;
                        }
                    }
                }

                switch (nb) {
                case 0:        /* creation d'une nouvelle equi */

                    headsig = addptype (headsig, signal, NULL);
                    settableint (htab, signal, headsig);

                    for (sc4 = nodelist; sc4 != NULL; sc4 = sc4->NEXT) {
                        ((noeud *) (sc4->DATA))->signal->b.SIGNAL = signal;
                        headsig->DATA = addchain (headsig->DATA, sc4->DATA);
                        settableint (signbnode, signal, (void *)(((long)tsttableint (signbnode, signal)) + 1));
                        if (SPI_DRIVE_SPEF_MODE)
                            _spispef_setnodename (ptcir, (noeud *) (sc4->DATA), ins, namealloc (sc3->DATA));
                    }

                    signal++;

                    break;

                case 1:        /* On reutilise l'equi existante */

                    // pt = getptype( headsig, idsig );
                    pt = (ptype_list *) tsttableint (htab, idsig);

                    for (sc4 = nodelist; sc4 != NULL; sc4 = sc4->NEXT) {
                        if (!((noeud *) (sc4->DATA))->signal->b.SIGNAL) {
                            ((noeud *) (sc4->DATA))->signal->b.SIGNAL = idsig;
                            pt->DATA = addchain (pt->DATA, sc4->DATA);
                            settableint (signbnode, pt->TYPE, (void *)(((long)tsttableint (signbnode, pt->TYPE)) + 1));
                        }
                        if (SPI_DRIVE_SPEF_MODE)
                            _spispef_setnodename (ptcir, (noeud *) (sc4->DATA), ins, namealloc (sc3->DATA));
                    }

                    break;

                default:        /* Il y a plus de deux equi existante : il faut
                                   les regrouper en une seule */
                    // pt = getptype( headsig, idsig );
                    pt = (ptype_list *) tsttableint (htab, idsig);
                    l1 = (int)(long)tsttableint (signbnode, pt->TYPE);


                    for (sc4 = nodelist; sc4 != NULL; sc4 = sc4->NEXT) {
                        if (SPI_DRIVE_SPEF_MODE)
                            _spispef_setnodename (ptcir, (noeud *) (sc4->DATA), ins, namealloc (sc3->DATA));

                        /* Pas de signal sur le noeud sc4 */
                        if (!((noeud *) (sc4->DATA))->signal->b.SIGNAL) {
                            ((noeud *) (sc4->DATA))->signal->b.SIGNAL = idsig;
                            pt->DATA = addchain (pt->DATA, sc4->DATA);
                            l1++;
                        }
                        else
                            /* Si on se trouve sur un autre signal */
                        if (((noeud *) (sc4->DATA))->signal->b.SIGNAL != idsig) {
                            /* t : index signal source */
                            t = ((noeud *) (sc4->DATA))->signal->b.SIGNAL;
                            pt2 = (ptype_list *) tsttableint (htab, t);
                            l1 = l1 + (int)(long)tsttableint (signbnode, pt2->TYPE);

                            /* on change ne numero de signal de tous les noeuds du signal 
                             * source */
                            for (sc5 = pt2->DATA; sc5; sc5 = sc5->NEXT)
                                ((noeud *) (sc5->DATA))->signal->b.SIGNAL = idsig;

                            /* on ajoute la chain_list source au debut de la chain_list
                             * destination */
                            for (sc5 = (chain_list *) (pt2->DATA); sc5->NEXT; sc5 = sc5->NEXT);
                            sc5->NEXT = (chain_list *) pt->DATA;
                            pt->DATA = pt2->DATA;

                            /* On libere le signal source */
                            removetableint (htab, t);
                            pt2->DATA = NULL;
                        }
                        /* Cas où on est sur le meme signal : on ne fait rien */
                    }
                    settableint (signbnode, pt->TYPE, (void *)(long)l1);

                    break;
                }
                freechain (nodelist);
            }

            if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE) delht (htcon);

            if (sc1 || sc2) 
                avt_errmsg (SPI_ERRMSG, "044", AVT_FATAL, ptcir->NOM, scaninst->NOM, scaninst->MODELE);
        }

    }

    for (scanresi = ptcir->RESI, nbresi = 0; scanresi; scanresi = scanresi->NEXT, nbresi++);

    for (scanresi = ptcir->RESI, curresi = 0; scanresi; scanresi = scanresi->NEXT, curresi++) {
        if (((noeud *) scanresi->NODE1)->signal->b.SIGNAL == 0 && ((noeud *) scanresi->NODE2)->signal->b.SIGNAL == 0) {
            headsig = addptype (headsig, signal, NULL);
            settableint (htab, signal, headsig);
            ((noeud *) scanresi->NODE1)->signal->b.SIGNAL = signal;
            ((noeud *) scanresi->NODE2)->signal->b.SIGNAL = signal;
            headsig->DATA = addchain (headsig->DATA, (void *)scanresi->NODE1);
            headsig->DATA = addchain (headsig->DATA, (void *)scanresi->NODE2);
            settableint (signbnode, signal, (void *)(long)2);
            signal++;
        }
        else if (((noeud *) scanresi->NODE1)->signal->b.SIGNAL != 0 &&
                 ((noeud *) scanresi->NODE2)->signal->b.SIGNAL == 0) {
            pt = (ptype_list *) tsttableint (htab, ((noeud *) scanresi->NODE1)->signal->b.SIGNAL);
            pt->DATA = addchain (pt->DATA, (void *)scanresi->NODE2);
            ((noeud *) scanresi->NODE2)->signal->b.SIGNAL = pt->TYPE;
            settableint (signbnode, pt->TYPE, (void *)(((long)tsttableint (signbnode, pt->TYPE)) + 1));
        }
        else if (((noeud *) scanresi->NODE1)->signal->b.SIGNAL == 0 &&
                 ((noeud *) scanresi->NODE2)->signal->b.SIGNAL != 0) {
            pt = (ptype_list *) tsttableint (htab, ((noeud *) scanresi->NODE2)->signal->b.SIGNAL);
            pt->DATA = addchain (pt->DATA, (void *)scanresi->NODE1);
            ((noeud *) scanresi->NODE1)->signal->b.SIGNAL = pt->TYPE;
            settableint (signbnode, pt->TYPE, (void *)(((long)tsttableint (signbnode, pt->TYPE)) + 1));
        }
        else if (((noeud *) scanresi->NODE1)->signal->b.SIGNAL != ((noeud *) scanresi->NODE2)->signal->b.SIGNAL) {
            l1 = (int)(long)tsttableint (signbnode, ((noeud *) scanresi->NODE1)->signal->b.SIGNAL);
            pt = (ptype_list *) tsttableint (htab, ((noeud *) scanresi->NODE1)->signal->b.SIGNAL);

            l2 = (int)(long)tsttableint (signbnode, ((noeud *) scanresi->NODE2)->signal->b.SIGNAL);
            pt2 = (ptype_list *) tsttableint (htab, ((noeud *) scanresi->NODE2)->signal->b.SIGNAL);

            if (l1 < l2) {
                for (sc1 = pt->DATA; sc1; sc1 = sc1->NEXT) {
                    ((noeud *) (sc1->DATA))->signal->b.SIGNAL = pt2->TYPE;
                    last = sc1;
                }
                last->NEXT = pt2->DATA;
                pt2->DATA = pt->DATA;
                pt->DATA = NULL;
                settableint (signbnode, pt2->TYPE, (void *)(long)(l1 + l2));
                settableint (signbnode, pt->TYPE, (void *)(long)0);
            }
            else {
                for (sc1 = pt2->DATA; sc1; sc1 = sc1->NEXT) {
                    ((noeud *) (sc1->DATA))->signal->b.SIGNAL = pt->TYPE;
                    last = sc1;
                }
                last->NEXT = pt->DATA;
                pt->DATA = pt2->DATA;
                pt2->DATA = NULL;
                settableint (signbnode, pt->TYPE, (void *)(long)(l1 + l2));
                settableint (signbnode, pt2->TYPE, (void *)(long)0);
            }
        }
    }

    for (scanvalim = ptcir->VALIM; scanvalim; scanvalim = scanvalim->SUIV) {
        if (!SPI_ZEROVOLT_RESI || scanvalim->TENSION != 0.0)
            continue;
        if (scanvalim->N1->signal->b.SIGNAL == 0 && scanvalim->N2->signal->b.SIGNAL == 0) {
            headsig = addptype (headsig, signal, NULL);
            settableint (htab, signal, headsig);
            scanvalim->N1->signal->b.SIGNAL = signal;
            scanvalim->N2->signal->b.SIGNAL = signal;
            headsig->DATA = addchain (headsig->DATA, scanvalim->N1);
            headsig->DATA = addchain (headsig->DATA, scanvalim->N2);
            settableint (signbnode, signal, (void *)(long)2);
            signal++;
        }
        else if (scanvalim->N1->signal->b.SIGNAL != 0 && scanvalim->N2->signal->b.SIGNAL == 0) {
            pt = (ptype_list *) tsttableint (htab, scanvalim->N1->signal->b.SIGNAL);
            pt->DATA = addchain (pt->DATA, scanvalim->N2);
            scanvalim->N2->signal->b.SIGNAL = pt->TYPE;
            settableint (signbnode, pt->TYPE, (void *)(((long)tsttableint (signbnode, pt->TYPE)) + 1));
        }
        else if (scanvalim->N1->signal->b.SIGNAL == 0 && scanvalim->N2->signal->b.SIGNAL != 0) {
            pt = (ptype_list *) tsttableint (htab, scanvalim->N2->signal->b.SIGNAL);
            pt->DATA = addchain (pt->DATA, scanvalim->N1);
            scanvalim->N1->signal->b.SIGNAL = pt->TYPE;
            settableint (signbnode, pt->TYPE, (void *)(((long)tsttableint (signbnode, pt->TYPE)) + 1));
        }
        else if (scanvalim->N1->signal->b.SIGNAL != scanvalim->N2->signal->b.SIGNAL) {
            pt = (ptype_list *) tsttableint (htab, scanvalim->N1->signal->b.SIGNAL);
            pt2 = (ptype_list *) tsttableint (htab, scanvalim->N2->signal->b.SIGNAL);

            l1 = (int)(long)tsttableint (signbnode, scanvalim->N1->signal->b.SIGNAL);
            pt = (ptype_list *) tsttableint (htab, scanvalim->N1->signal->b.SIGNAL);

            l2 = (int)(long)tsttableint (signbnode, scanvalim->N2->signal->b.SIGNAL);
            pt2 = (ptype_list *) tsttableint (htab, scanvalim->N2->signal->b.SIGNAL);

            if (l1 < l2) {
                for (sc1 = pt->DATA; sc1; sc1 = sc1->NEXT) {
                    ((noeud *) (sc1->DATA))->signal->b.SIGNAL = pt2->TYPE;
                    last = sc1;
                }
                last->NEXT = pt2->DATA;
                pt2->DATA = pt->DATA;
                pt->DATA = NULL;
                settableint (signbnode, pt2->TYPE, (void *)(long)(l1 + l2));
                settableint (signbnode, pt->TYPE, (void *)(long)0);
                /* On peut l'enlever ? */
                removetableint (htab, pt->TYPE);
                headsig = delptype (headsig, pt->TYPE);
            }
            else {
                for (sc1 = pt2->DATA; sc1; sc1 = sc1->NEXT) {
                    ((noeud *) (sc1->DATA))->signal->b.SIGNAL = pt->TYPE;
                    last = sc1;
                }
                last->NEXT = pt->DATA;
                pt->DATA = pt2->DATA;
                pt2->DATA = NULL;
                settableint (signbnode, pt->TYPE, (void *)(long)(l1 + l2));
                settableint (signbnode, pt2->TYPE, (void *)(long)0);
                /* On peut l'enlever ? */
                removetableint (htab, pt2->TYPE);
                headsig = delptype (headsig, pt2->TYPE);
            }


        }
    }

    if (SPI_MERGE == 1) {
        chain_list *lst, *grp;

        for (sc1 = ptcir->CINTERF, lst = NULL; sc1; sc1 = sc1->NEXT) {
            node1 = (noeud *) sc1->DATA;
/*          lst=addchain(lst, node1->signal->items);
*/
            node1->signal->items = NULL;
        }

//      lst=reverse(lst);

        grp = linksameequiconnectors (ptcir, ptcir->CINTERF);

        while (grp != NULL) {
            sc1 = (chain_list *) grp->DATA;
            node1 = (noeud *) (sc1->DATA);

            for (sc2 = sc1->NEXT; sc2; sc2 = sc2->NEXT) {
                node2 = (noeud *) (sc2->DATA);

                if (node1->signal->b.SIGNAL == 0 && node2->signal->b.SIGNAL == 0) {
                    headsig = addptype (headsig, signal, NULL);
                    settableint (htab, signal, headsig);
                    node1->signal->b.SIGNAL = signal;
                    node2->signal->b.SIGNAL = signal;
                    headsig->DATA = addchain (headsig->DATA, node1);
                    headsig->DATA = addchain (headsig->DATA, node2);
                    signal++;
                }
                else if (node1->signal->b.SIGNAL != 0 && node2->signal->b.SIGNAL == 0) {
                    pt = (ptype_list *) tsttableint (htab, node1->signal->b.SIGNAL);
                    pt->DATA = addchain (pt->DATA, node2);
                    node2->signal->b.SIGNAL = node1->signal->b.SIGNAL;
                }
                else if (node1->signal->b.SIGNAL == 0 && node2->signal->b.SIGNAL != 0) {
                    pt = (ptype_list *) tsttableint (htab, node2->signal->b.SIGNAL);
                    pt->DATA = addchain (pt->DATA, node1);
                    node1->signal->b.SIGNAL = node2->signal->b.SIGNAL;
                }
                else if (node1->signal->b.SIGNAL != node2->signal->b.SIGNAL) {
                    pt = (ptype_list *) tsttableint (htab, node1->signal->b.SIGNAL);
                    pt2 = (ptype_list *) tsttableint (htab, node2->signal->b.SIGNAL);

                    for (sc3 = pt2->DATA; sc3; sc3 = sc3->NEXT)
                        ((noeud *) (sc3->DATA))->signal->b.SIGNAL = node1->signal->b.SIGNAL;
                    for (sc3 = pt->DATA; sc3->NEXT; sc3 = sc3->NEXT);
                    sc3->NEXT = pt2->DATA;
                    removetableint (htab, pt2->TYPE);
                    headsig = delptype (headsig, pt2->TYPE);
                }
                else {
                    /* rien a faire : node1->SIGNAL == node2->SIGNAL */
                }
            }
            freechain ((chain_list *) grp->DATA);
            grp = delchain (grp, grp);
        }
/*
      for( sc1 = ptcir->CINTERF, sc2=lst ; sc1 ; sc2=sc2->NEXT, sc1 = sc1->NEXT )
        {
          node1 = (noeud*)sc1->DATA;
          node1->signal->items=(__item_ *)sc2->DATA;
        }
      freechain(lst);
*/
    }

    /*    On s'occupe des noeuds libres                   */

    for (i = 0; i < ptcir->nbnodes; i++) {
        scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
        if (scannoeud->index < 0)
            continue;
        if (scannoeud->signal->b.SIGNAL == 0) {
            headsig = addptype (headsig, signal, addchain (NULL, scannoeud));
            settableint (htab, signal, headsig);
            scannoeud->signal->b.SIGNAL = signal++;

        }
    }

    /*    Relie les noeuds sur l'interface aux signaux de meme nom */

    dspflinkexternal = V_BOOL_TAB[__SPI_DSPF_LINK_EXTERNAL].VALUE?'y':'n';

    if (dspflinkexternal == 'y') {

        for (sc1 = ptcir->CINTERF; sc1; sc1 = sc1->NEXT) {

            node1 = (noeud *) (sc1->DATA);

            pt = (ptype_list *) tsttableint (htab, node1->signal->b.SIGNAL);

            if (((chain_list *) pt->DATA)->NEXT == NULL) {    /* noeud isolé */

                /* recherche son nom */
                AdvancedNameAllocName (ptcir->ana, node1->index, nom0);
                dspfname = _v2_spi_devect (nom0, nom1);

                /* recherche parmis headsig un signal qui a le meme SNOM */

                for (pt2 = headsig; pt2; pt2 = pt2->NEXT) {

                    for (sc2 = pt2->DATA; sc2; sc2 = sc2->NEXT) {
                        node2 = (noeud *) sc2->DATA;
                        if (node2->SUPINFO) {
                            if (node2->SUPINFO->SNOM)
                                break;
                        }
                    }

                    if (sc2 && strcasecmp (node2->SUPINFO->SNOM, dspfname) == 0)
                        break;
                }

                if (pt2) {

                    node1->signal->b.SIGNAL = node2->signal->b.SIGNAL;
                    pt2->DATA = addchain (pt2->DATA, node1);
                    removetableint (htab, pt->TYPE);
                    headsig = delptype (headsig, pt->TYPE);

                }
            }
        }
    }

    /*    Remplie le champ RCN de chaque noeud                */

    for (spt = headsig; spt; spt = spt->NEXT) {
        sc1 = (chain_list *) (spt->DATA);

        if (!sc1)
            continue;

        if (!(SPI_IGNORE_RESI || SPI_DRIVE_SPEF_MODE) && (sc1->NEXT || SPI_ONE_NODE_NORC == 0)) {
            nb = 1;
            for (; sc1; sc1 = sc1->NEXT)
                ((noeud *) (sc1->DATA))->RCN = nb++;
        }
        else {
            for (; sc1; sc1 = sc1->NEXT)
                ((noeud *) (sc1->DATA))->RCN = 0;
        }
    }

    /* short circuits : removed resitors */

    for (i = 0; i < ptcir->nbnodes; i++) {
        scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
        if (scannoeud->index < 0)
            continue;
        if (scannoeud->SUPINFO != NULL && scannoeud->SUPINFO->U.SHORTCIRCUIT != NULL) {
            chain_list **common;
            common = scannoeud->SUPINFO->U.SHORTCIRCUIT;
            nb = scannoeud->RCN;
            for (sc1 = *common; sc1 != NULL; sc1 = sc1->NEXT) {
                ((noeud *) sc1->DATA)->RCN = nb;
                ((noeud *) sc1->DATA)->SUPINFO->U.SHORTCIRCUIT = NULL;
            }

            freechain (*common);
            mbkfree (common);
        }
    }


    /* Renumérote les noeud doublons sur l'interface des instances */

    for (scaninst = ptcir->INST; scaninst; scaninst = scaninst->SUIV) {
        sci = (ginterf *) gethtitem (ALLINTERF->h, namealloc (scaninst->MODELE));
        if (sci != (ginterf *) EMPTYHT && sci->DUPCON) {

            for (sc1 = sci->DUPCON, sc2 = scaninst->IINTERF; sc1 && sc2; sc1 = sc1->NEXT, sc2 = sc2->NEXT) {
                if (sc1->DATA) {
                    for (sc3 = sc1->NEXT, sc4 = sc2->NEXT; sc3 != sc1->DATA; sc3 = sc3->NEXT, sc4 = sc4->NEXT);
                    ((noeud *) sc4->DATA)->RCN = ((noeud *) sc2->DATA)->RCN;
                }
            }
        }
    }

    /*    Tous les noeuds d'un signal VSS ou VDD ont le nom VSS ou VDD    */

#if 0
    vss = spicenamealloc (ptcir, VSS);
    vdd = spicenamealloc (ptcir, VDD);
    cirvss = 0;
    cirvdd = 0;

    for (spt = headsig; spt; spt = spt->NEXT) {
        vuevss = vuevdd = 0;
        

        for (sc1 = spt->DATA; sc1; sc1 = sc1->NEXT) {
            AdvancedNameAllocName (ptcir->ana, ((noeud *) (sc1->DATA))->index, nom0);
            if (strcmp (nom0, vss) == 0)
                vuevss = 1;

            if (strcmp (nom0, vdd) == 0)
                vuevdd = 1;
        }

        if (vuevss && cirvss) 
            avt_errmsg (SPI_ERRMSG, "046", AVT_FATAL, ptcir->NOM);

        if (vuevss) {
            for (sc1 = spt->DATA; sc1; sc1 = sc1->NEXT)
                ((noeud *) (sc1->DATA))->index = AdvancedNameAlloc (ptcir->ana, vss);
            cirvss = 1;
        }

        if (vuevdd && cirvdd) 
            avt_errmsg (SPI_ERRMSG, "047", AVT_FATAL, ptcir->NOM);

        if (vuevdd) {
            for (sc1 = spt->DATA; sc1; sc1 = sc1->NEXT)
                ((noeud *) (sc1->DATA))->index = AdvancedNameAlloc (ptcir->ana, vdd);
            cirvdd = 1;
        }
    }
#endif
    /*
       if( !cirvss || !cirvdd )
       {
       fflush( stdout );
       fprintf( stderr,
       "*** mbkspi error *** : No vdd or vss nodes in %s\n",
       ptcir->NOM
       );
       EXIT( 1 );
       }
     */

    /* On renumerote les signaux en partant de 1, c'est plus propre.      */
    idsig = 1;
    for (spt = headsig; spt; spt = spt->NEXT) {
        if (spt->DATA) {
            for (scanchain = (chain_list *) (spt->DATA); scanchain; scanchain = scanchain->NEXT) {
                scannoeud = (noeud *) (scanchain->DATA);
                scannoeud->signal->b.SIGNAL = idsig;
            }
            idsig++;
        }
    }

    /*    libere la liste des noeuds par signal               */

    for (spt = headsig; spt; spt = spt->NEXT)
        if (spt->DATA)
            freechain ((chain_list *) (spt->DATA));
    freeptype (headsig);
    freetableint (htab);

    if (signbnode)
        freetableint (signbnode);

    return (ALLINTERF);
}

/******************************************************************************/
     /* Renvoie 1 si les deux noms sont sur la meme equipotentielle */

static int get_equi_separ_pos (char *name)
{
    int l, pos;
    pos = l = strlen (name) - 1;
    if (V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE)
    {
      return pos+1;
    }

    while (l >= 0 && isdigit ((int)name[l]))
        l--;
    if (l < 0)
        return pos + 1;
    if (name[l] == SPI_SEPAR)
        return l;
    return pos + 1;
}

/******************************************************************************/
// zinaps
static locon_list *createlocon (lotrs_list * pttrs, char *name, losig_list * ptsig)
{
    locon_list *ptcon;
    ptcon = (locon_list *) mbkalloc (sizeof (locon_list));
    ptcon->NAME = name;
    ptcon->SIG = ptsig;
    ptcon->ROOT = (void *)pttrs;
    ptcon->TYPE = 'T';
    ptcon->DIRECTION = 'I';
    ptcon->USER = NULL;
    ptcon->PNODE = NULL;
    return ptcon;
}

static char *_v2_getsigname (char *n0, char *n1)
{
    chain_list *ptscan, *cl;
    char *ptchar;
    char *result = NULL;
    register int nseparmin = 1000;
    
    if (n0 == NULL)
        return n1;

    if (strcmp(n0,"0")==0) return n0;
    else if (strcmp(n1,"0")==0) return n1;

    cl = addchain (NULL, n0);
    cl = addchain (cl, n1);

    for (ptscan = cl; ptscan; ptscan = ptscan->NEXT) {
        register int nsepar = 0;
        if (ptscan->DATA != NULL) {
            for (ptchar = (char *)ptscan->DATA; *ptchar != '\0'; ptchar++)
                if (*ptchar == SEPAR)
                    nsepar++;
            if (nsepar < nseparmin) {
                nseparmin = nsepar;
                result = (char *)ptscan->DATA;
            }
            else if (nsepar == nseparmin) {
                if (strlen ((char *)ptscan->DATA) < strlen (result)) {
                    result = (char *)ptscan->DATA;
                }
            }
        }
    }
    freechain (cl);
    return result;
}

static char *getBestInterfaceName (char *n0, char *n1)
{
    chain_list *ptchain;
    int l0, l1;

    if (n0 == NULL)
        return n1;
    if (n1 == NULL)
        return n0;
    n0 = namealloc (n0);
    n1 = namealloc (n1);
    for (ptchain = SPI_GLOBAL_NODES; ptchain; ptchain = ptchain->NEXT) {
        if ((char *)ptchain->DATA == n0)
            return n0;
        if ((char *)ptchain->DATA == n1)
            return n1;
    }
    l0=strlen (n0);
    l1=strlen (n1);
    if (isdigit ((int)n0[l0 - 1]) && !isdigit ((int)n1[l1 - 1]))
        return n1;
    if (isdigit ((int)n1[l1 - 1]) && !isdigit ((int)n0[l0 - 1]))
        return n0;
    if (l1 < l0)
        return n1;
    return n0;
}


allinterf *constlofig (ptcir, ptfig, ALLINTERF, mode)
circuit *ptcir;
lofig_list *ptfig;
allinterf *ALLINTERF;
char mode;
{
    noeud *scannoeud;
    noeud *ptnoeud;
    chain_list *sc1, *sc2, *sc3, *sc4;
    locon_list *scanlocon;
    losig_list *ptsig;
    losig_list *sigvss;
    int nbsig;
    int i;
    losig_list **tabsig;
    num_list *interf;
    chain_list *connec_sig;
    chain_list *connec_noeud;
    chain_list *connec_nom;
//  chain_list    *connec_con;
    lotrs_list *scantrans;
    diode *scandiode;
    inst *scaninst;
    char *nom;
    int idxcon;
    lotrs_list *pttrs;
    locon_list *scancon;
    loctc_list *scancapa, *ptctc;
    loctc_list *nextcapa;
    loins_list *ptinst;
    lowire_list *scanresi;
    lowire_list *ptresi;
    lowire_list *nextresi;
    valim *scanvalim;
    ginterf *sci;
    lofig_list *modele;
    lofig_list *tetemodele;
    ptype_list *ptnodename;
    char buf[4096];
    int *destsig;
    char *locksig;
    int maxsig;
    losig_list *scanlosig;
    //ht            *htinst;
    ht *htmodel;
    insparam_list *ptparam;
    int done;
    // zinaps
    struct s_noeud *DRAIN;
    struct s_noeud *SOURCE;
    struct s_noeud *GRILLE;
    struct s_noeud *SUBST;
    num_list *verifpnode;
    char nom0[1024];
    char nom1[1024];
    char *best;
    __equi_ *e;
    noeud *n;
    ptype_list *ptuser;
    noeud **extnamebysig;
    char buf0[1024], buf1[1024];
    char *ptbuf0, *ptbuf1;
    optparam_list *ptopt;
    mcc_explist *ptmccexp = NULL;
//    xy_struct *xy;
    inffig_list *ifl;
    ht *htcon;
    long l;
    chain_list *nodelist;
    float val;
    unsolved_vcard_type *uvc;
    chain_list *unsolved_vcard;
    losig_list *plus_signal;
    losig_list *minus_signal;

    tetemodele = NULL;

    /* if circuit contains a .MODEL then local .PARAM expressions 
     * are put aside exclusively for MCC to avoid costly recalculation
     * during flatten otherwise they are put with the normal default
     * parameters */
#if 0
    if (!ptcir->HASMODEL) {
        ptcir->PARAM =
            (optparam_list *) append ((chain_list *) ptcir->PARAM, reverse ((chain_list *) ptcir->LOCAL_PARAMS));
        ptcir->LOCAL_PARAMS = NULL;
    }
    if (ptcir->CTX)
        ptfig->USER = addptype (ptfig->USER, PARAM_CONTEXT, eqt_export_vars (ptcir->CTX));
    if (ptcir->PARAM)
        ptfig->USER = addptype (ptfig->USER, OPT_PARAMS, ptcir->PARAM);

    if (ptcir->HASMODEL) {
        for (ptopt = ptcir->LOCAL_PARAMS; ptopt; ptopt = ptopt->NEXT) {
            if (ptopt->TAG == '$') {
                ptmccexp = mcc_addexp (ptmccexp, ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR, 0.0);
            }
        }
        if (ptmccexp) {
            ptfig->USER = addptype (ptfig->USER, MCC_MODEL_EXPR, ptmccexp);
        }
    }
#endif
    if (ptcir->HASMODEL) {
        for (ptopt = ptcir->LOCAL_PARAMS; ptopt; ptopt = ptopt->NEXT) {
            if (ptopt->TAG == '$') {
                ptmccexp = mcc_addexp (ptmccexp, ptopt->UNAME.SPECIAL, ptopt->UDATA.EXPR, 0.0);
            }
        }
        if (ptmccexp) {
            ptfig->USER = addptype (ptfig->USER, MCC_MODEL_EXPR, ptmccexp);
        }
    }
    ptcir->PARAM = (optparam_list *) append ((chain_list *) ptcir->PARAM, reverse ((chain_list *) ptcir->LOCAL_PARAMS));
    ptcir->LOCAL_PARAMS = NULL;
    if (ptcir->CTX)
        ptfig->USER = addptype (ptfig->USER, PARAM_CONTEXT, eqt_export_vars (ptcir->CTX));
    if (ptcir->PARAM)
        ptfig->USER = addptype (ptfig->USER, OPT_PARAMS, ptcir->PARAM);

    /* Création du tableau des signaux */

    nbsig = 0;
    for (i = 0; i < ptcir->nbnodes; i++) {
        scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
        if (scannoeud->index < 0)
            continue;
        scannoeud->signal->tag = 0;
        if (scannoeud->signal->b.SIGNAL > nbsig)
            nbsig = scannoeud->signal->b.SIGNAL;
    }

    for (scanlosig = ptfig->LOSIG; scanlosig; scanlosig = scanlosig->NEXT) {
        if (scanlosig->INDEX > nbsig)
            nbsig = scanlosig->INDEX;
    }

    tabsig = (losig_list **) mbkalloc (sizeof (losig_list *) * nbsig);
    extnamebysig = (noeud **) mbkalloc (sizeof (noeud *) * nbsig);
    for (i = 0; i < nbsig; i++) {
        tabsig[i] = NULL;
    }

    // Renumérote les signaux pour les faire correspondre à ceux de la lofig
    // qui existent déjà.

    {
        // Le tableau destsig contient la correspondance entre l'ancien index de 
        // signal et le nouveau. Le tableau locksig permet de savoir rapidement
        // si un index de signal est disponible.

        destsig = (int *)mbkalloc (sizeof (int) * (nbsig + 1));
        locksig = (char *)mbkalloc (sizeof (char) * (nbsig + 1));

        for (i = 1; i <= nbsig; i++) {
            locksig[i] = 0;
            destsig[i] = 0;
        }


        // Trouve les signaux qui doivent être renuméroté.

        sci = ALLINTERF->teteinterf;
        for (sc1 = ptcir->CINTERF, sc2 = sci->GINTERF; sc1 && sc2; sc1 = sc1->NEXT, sc2 = sc2->NEXT) {
            for (scanlocon = ptfig->LOCON; scanlocon; scanlocon = scanlocon->NEXT)
                if (strcasecmp (scanlocon->NAME, _v2_spi_devect ((char *)(sc2->DATA), buf)) == 0)
                    break;

            if (scanlocon) {
                ptnoeud = (noeud *) (sc1->DATA);

                if (destsig[ptnoeud->signal->b.SIGNAL] != 0 &&
                    destsig[ptnoeud->signal->b.SIGNAL] != scanlocon->SIG->INDEX) {
                    fflush (stdout);
                    fprintf (stderr, "fatal error : can't complete model %s.\n", ptcir->NOM);
                    EXIT (1);
                }

                destsig[ptnoeud->signal->b.SIGNAL] = scanlocon->SIG->INDEX;
                locksig[scanlocon->SIG->INDEX] = 1;

            }
        }

        // Renumérote les autres

        maxsig = 1;
        for (i = 1; i <= nbsig; i++) {
            if (destsig[i] == 0) {
                while (locksig[maxsig])
                    maxsig++;
                locksig[maxsig] = 1;
                destsig[i] = maxsig;
            }
        }

        // Renumérote les noeuds

        for (i = 0; i < ptcir->nbnodes; i++) {
            scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
            if (scannoeud->index < 0 || scannoeud->signal->tag != 0)
                continue;
            scannoeud->signal->tag = 1;
            scannoeud->signal->b.SIGNAL = destsig[scannoeud->signal->b.SIGNAL];
        }
        mbkfree (locksig);
        locksig = NULL;
        mbkfree (destsig);
        destsig = NULL;
    }

    // On ajoute les signaux qui sont déjà dans la lofig : l'interface


    for (scanlocon = ptfig->LOCON; scanlocon; scanlocon = scanlocon->NEXT)
        tabsig[scanlocon->SIG->INDEX - 1] = scanlocon->SIG;


    /* Création des signaux */

    if (!ptfig->BKSIG.TAB) {
/*    if (nbsig > 4096)
      setsigsize( ptfig, 4096);
    else {*/
        if (nbsig > 0)
            setsigsize (ptfig, nbsig > 32 ? nbsig : 32);
//    }
    }

    for (e = ptcir->all_equi; e != NULL; e = e->next) {
        if (!(ptsig = tabsig[e->b.SIGNAL - 1])) {
            ptsig = addlosig (ptfig, e->b.SIGNAL, NULL, INTERNAL);
            tabsig[e->b.SIGNAL - 1] = ptsig;
        }

        if (SPI_CREATE_TOP_FIGURE && (ptcir == SPI_TOPCIR)) {
            char *best = NULL, *prevbest = NULL;
            char buf[1024];
            int bestindex = 1;

            if ((ptuser = getptype (ptsig->USER, TOPEXTNAME)) != NULL)
                prevbest = ptuser->DATA;
            if ((ptuser = getptype (ptsig->USER, TOPEXTNODE)) != NULL)
                bestindex = (int)(long)ptuser->DATA;
            for (n = e->nodes; n != NULL; n = n->SUIV) {
                if (AdvancedNameAllocName (ptcir->ana, n->index, nom0) == 0) {
                    best = getBestInterfaceName (prevbest, _v2_spi_devect (nom0, buf));
                    if (best != prevbest) {
                        bestindex = n->RCN;
                        prevbest = best;
                    }
                }
            }
            if (best != NULL) {
                if ((ptuser = getptype (ptsig->USER, TOPEXTNAME)) != NULL) {
                    ptuser->DATA = namealloc (best);
                }
                else
                    ptsig->USER = addptype (ptsig->USER, TOPEXTNAME, namealloc (best));
            }
            if ((ptuser = getptype (ptsig->USER, TOPEXTNODE)) != NULL) {
                ptuser->DATA = (void *)(long)bestindex;
            }
            else
                ptsig->USER = addptype (ptsig->USER, TOPEXTNODE, (void *)(long)bestindex);
        }

        l=0;
        if ((SPICE_KEEP_NAMES & KEEP__ALLSIGNALS) == KEEP__ALLSIGNALS) {
            chain_list *temp=NULL, *cl, *ch;
            char *name;
            for (n = e->nodes; n != NULL; n = n->SUIV) {
                if (AdvancedNameAllocName (ptcir->ana, n->index, nom0) == 0) {
                    for (sc1 = ptsig->NAMECHAIN; sc1; sc1 = sc1->NEXT)
                        if (strcmp (sc1->DATA, nom0) == 0)
                            break;

                    if (!sc1
                        && (mbk_CheckREGEX (&ptcir->IGNORE_NAMES, nom0) == 0
                            || (ptsig->NAMECHAIN == NULL && n->SUIV == NULL)))
                        ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, spi_devect (nom0)), temp=addchain(temp, (void *)(long)n->RCN);
                }
                else if (strcmp(nom0,"0")==0)
                {
                  ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, spi_devect (nom0)), temp=addchain(temp, (void *)(long)n->RCN);
                }

                if (n->SUPINFO != NULL && n->SUPINFO->SNOM) {
                    for (sc1 = ptsig->NAMECHAIN; sc1; sc1 = sc1->NEXT)
                        if (strcmp (sc1->DATA, n->SUPINFO->SNOM) == 0)
                            break;

                    if (!sc1
                        && (mbk_CheckREGEX (&ptcir->IGNORE_NAMES, n->SUPINFO->SNOM) == 0 || ptsig->NAMECHAIN == NULL))
                        ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, namealloc (n->SUPINFO->SNOM)), temp=addchain(temp, (void *)(long)n->RCN);
                }
            }
            name=getsigname(ptsig);
            for (cl=ptsig->NAMECHAIN, ch=temp; ch!=NULL && cl!=NULL && cl->DATA!=name; cl=cl->NEXT, ch=ch->NEXT) ;
            if (ch!=NULL) l=(long)ch->DATA;
            freechain(temp);
        }
        else {
            char *best = NULL;
            chain_list *cl;
            char temp[1024];
            char buf[1024];

            for (cl = ptsig->NAMECHAIN; cl != NULL; cl = cl->NEXT) {
                if (mbk_CheckREGEX (&ptcir->IGNORE_NAMES, (char *)cl->DATA) == 0)
                    best = _v2_getsigname (best, cl->DATA);
            }
            for (n = e->nodes; n != NULL; n = n->SUIV) {
                if (AdvancedNameAllocName (ptcir->ana, n->index, nom0) == 0) {
                    if (mbk_CheckREGEX (&ptcir->IGNORE_NAMES, nom0) == 0 || (best == NULL && n->SUIV == NULL)) {
                        best = _v2_getsigname (best, _v2_spi_devect (nom0, buf));
                        if (best != NULL && best!=temp) {
                            strcpy (temp, best);
                            best = temp;
                            l=n->RCN;
                        }
                    }
                }
                else if (strcmp(nom0,"0")==0)
                {
                  strcpy(temp, nom0);
                  best=temp;
                  l=n->RCN;
                  break;
                }
            }

            if (best != NULL) {
                freechain (ptsig->NAMECHAIN);
                ptsig->NAMECHAIN = addchain (NULL, namealloc (best));
            }

            for (n = e->nodes; n != NULL; n = n->SUIV) {
                if (n->SUPINFO != NULL && n->SUPINFO->SNOM) {
/*                    for (sc1 = ptsig->NAMECHAIN; sc1; sc1 = sc1->NEXT)
                        if (strcmp (sc1->DATA, n->SUPINFO->SNOM) == 0)
                            break;

                    if (!sc1)*/
                     freechain(ptsig->NAMECHAIN);
                     ptsig->NAMECHAIN = addchain (NULL, namealloc (n->SUPINFO->SNOM));
                     l=n->RCN;
                }
            }
        }
        if (l!=0) ptsig->USER=addptype(ptsig->USER, MBK_SIGNALNAME_NODE, (void *)l);
    }


    /* Traitement du nom des connecteurs externes. Comme les noms sont gérés par
       AdvancedNameAllocName(), on ne conserve pas le pointeur vers le bon nom mais
       le pointeur vers le noeud */

    for (i = 0; i < nbsig; i++)
        extnamebysig[i] = NULL;

    for (sc1 = ptcir->CINTERF; sc1; sc1 = sc1->NEXT) {
        ptnoeud = (noeud *) (sc1->DATA);
        ptsig = tabsig[ptnoeud->signal->b.SIGNAL - 1];
        if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom0) == 0) {
            if (extnamebysig[ptsig->INDEX - 1]) {
                AdvancedNameAllocName (ptcir->ana, extnamebysig[ptsig->INDEX - 1]->index, nom1);
                ptbuf0 = _v2_spi_devect (nom0, buf0);
                ptbuf1 = _v2_spi_devect (nom1, buf1);
                best = _v2_getsigname (ptbuf0, ptbuf1);
                if (best == ptbuf0) {
                    extnamebysig[ptsig->INDEX - 1] = ptnoeud;
                }
                else
                {
                   if ((ptuser=getptype(ptsig->USER, SPICE_MULTI_CONNECTOR_NAMES))==NULL)
                     ptuser=ptsig->USER=addptype(ptsig->USER, SPICE_MULTI_CONNECTOR_NAMES, NULL);
                   ptuser->DATA=addchain((chain_list *)ptuser->DATA, namealloc(nom0));
                }
            }
            else
                extnamebysig[ptsig->INDEX - 1] = ptnoeud;
        }
    }

    for (i = 0; i < nbsig; i++) {
        ptnoeud = extnamebysig[i];
        if (ptnoeud != NULL) {
            ptsig = tabsig[ptnoeud->signal->b.SIGNAL - 1];
            nodelist = ptsig->NAMECHAIN;
            ptsig->NAMECHAIN = NULL;

            AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom0);
            ptbuf0 = _v2_spi_devect (nom0, buf0);
            ptbuf1 = namealloc (ptbuf0);

            if ((SPICE_KEEP_NAMES & KEEP__ALLSIGNALS) == KEEP__ALLSIGNALS) {
                for (connec_nom = nodelist; connec_nom != NULL; connec_nom = connec_nom->NEXT)
                    if (connec_nom->DATA != ptbuf1)
                        ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, connec_nom->DATA);
            }
            freechain (nodelist);
            ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, ptbuf1);
        }
    }

    mbkfree (extnamebysig);

    for (i = 0; i < ptcir->nbnodes; i++) {
        scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
        if (scannoeud->index < 0)
            continue;
        ptsig = tabsig[scannoeud->signal->b.SIGNAL /*SIGNAL*/ - 1];


        if (AdvancedNameAllocName (ptcir->ana, scannoeud->index, nom0) != 0 && ptsig->NAMECHAIN == NULL) {
            // Modif pour retrouver les même noms que Xcalibre
            if (SPI_NO_SIG_PREFIX)
                sprintf (buf, "%s", nom0);
            else
                sprintf (buf, "n%s", nom0);
            nom = namealloc (buf);

            ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, nom);
        }
    }

    /* On construit les vues RCN et/ou CTC */

    if ((SPICE_KEEP_NAMES & KEEP__ALL) == KEEP__ALL)
        triecapa (ptcir);

    for (scancapa = ptcir->CAPA; scancapa; scancapa = nextcapa) {
        nextcapa = (loctc_list *) scancapa->SIG1;

        /* ne pas prendre en compte du tout une capacité dont les deux extrémitées sont sur le 
           meme signal */
        if (tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1] ==
            tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1])
            continue;

        if (!tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1]->PRCN)
            addlorcnet (tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1]);

        if (!tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1]->PRCN)
            addlorcnet (tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1]);


        rcn_addcapa (tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1], scancapa->CAPA);
        rcn_addcapa (tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1], scancapa->CAPA);
    }

    for (scanresi = ptcir->RESI; scanresi; scanresi = nextresi) {
        nextresi = scanresi->NEXT;
        if (!tabsig[((noeud *) scanresi->NODE1)->signal->b.SIGNAL - 1]->PRCN)
            addlorcnet (tabsig[((noeud *) scanresi->NODE1)->signal->b.SIGNAL - 1]);

        ptresi = addlowire (tabsig[((noeud *) scanresi->NODE1)->signal->b.SIGNAL - 1],
                            0,
                            scanresi->RESI,
                            scanresi->CAPA, ((noeud *) scanresi->NODE1)->RCN, ((noeud *) scanresi->NODE2)->RCN);
        if ((SPICE_KEEP_NAMES & KEEP__RESISTANCE) == 0)
            scanresi->USER = testanddelptype (scanresi->USER, RESINAME);
        if (ptresi)
            ptresi->USER = scanresi->USER;
        scanresi->USER = NULL;
        freelowire (scanresi);
    }

    for (scanvalim = ptcir->VALIM; scanvalim; scanvalim = scanvalim->SUIV) {
        if (SPI_ZEROVOLT_RESI && (scanvalim->TENSION < 1e-15 && scanvalim->TENSION > -1e-15)) {
            if (!tabsig[scanvalim->N1->signal->b.SIGNAL - 1]->PRCN)
                addlorcnet (tabsig[scanvalim->N1->signal->b.SIGNAL - 1]);

            addlowire (tabsig[scanvalim->N1->signal->b.SIGNAL - 1],
                       0, 0.0, 0.0, scanvalim->N1->RCN, scanvalim->N2->RCN);
        }
    }

    if ((ptcir->RESI != NULL) || (SPI_NORCCAPA_TOGND == 0))
        for (scancapa = ptcir->CAPA; scancapa; scancapa = nextcapa) {
            nextcapa = (loctc_list *) scancapa->SIG1;
            if (!tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1]->PRCN)
                addlorcnet (tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1]);

            if (!tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1]->PRCN)
                addlorcnet (tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1]);

            if (((scancapa->USER != NULL) || (SPICE_KEEP_NAMES & KEEP__ALL) != KEEP__ALL) &&
                scancapa->NODE1 != scancapa->NODE2) {
                ptctc = addloctc (tabsig[((noeud *) scancapa->NODE1)->signal->b.SIGNAL - 1],
                                  ((noeud *) scancapa->NODE1)->RCN ?
                                  ((noeud *) scancapa->NODE1)->RCN : 1,
                                  tabsig[((noeud *) scancapa->NODE2)->signal->b.SIGNAL - 1],
                                  ((noeud *) scancapa->NODE2)->RCN ?
                                  ((noeud *) scancapa->NODE2)->RCN : 1, scancapa->CAPA);
                if (ptctc != NULL)
                {
                    ptctc->USER = (ptype_list *) scancapa->SIG2;
                    if ((SPICE_KEEP_NAMES & KEEP__CAPA) == KEEP__CAPA && scancapa->USER!=NULL)
                       ptctc->USER = addptype (ptctc->USER, MSL_CAPANAME, namealloc((char *)scancapa->USER));
                }
                else {
                    if ((ptuser = getptype ((ptype_list *) scancapa->SIG2, OPT_PARAMS))) {
                        freeoptparams ((optparam_list *) ptuser->DATA);
                        scancapa->SIG2 = (losig_list *)delptype ((ptype_list *) scancapa->SIG2, OPT_PARAMS);
                    }
                }
                scancapa->SIG2 = NULL;
            }

            scancapa->USER = NULL;
            freeloctc (scancapa);
        }

    ptcir->RESI = NULL;
    ptcir->CAPA = NULL;
    sci = ALLINTERF->teteinterf;

    /* Connecteurs de l'interface */

    {                            // ZINAPSHERE
        locon_list **lc_tab;
        long maxidx = -1;

        for (scanlocon = ptfig->LOCON; scanlocon; scanlocon = scanlocon->NEXT) {
            if (maxidx < scanlocon->SIG->INDEX)
                maxidx = scanlocon->SIG->INDEX;
        }
        for (sc1 = ptcir->CINTERF; sc1; sc1 = sc1->NEXT) {
            ptnoeud = (noeud *) (sc1->DATA);
            if (maxidx < ptnoeud->signal->b.SIGNAL)
                maxidx = ptnoeud->signal->b.SIGNAL;
        }

        maxidx++;

        if (maxidx > 0)
            lc_tab = (locon_list **) mbkalloc (sizeof (locon_list *) * maxidx);

        for (idxcon = 0; idxcon < maxidx; idxcon++)
            lc_tab[idxcon] = NULL;
        for (scanlocon = ptfig->LOCON; scanlocon; scanlocon = scanlocon->NEXT) {
            if (scanlocon->SIG->INDEX >= 0)
                lc_tab[scanlocon->SIG->INDEX] = scanlocon;
        }

        idxcon = 1;
        for (sc1 = ptcir->CINTERF, sc2 = sci->GINTERF, sc4=sci->CORRESPINTERFNAME; sc1 && sc2; sc1 = sc1->NEXT, sc2 = sc2->NEXT, sc4=sc4->NEXT) {
            ptnoeud = (noeud *) (sc1->DATA);

            scanlocon = lc_tab[ptnoeud->signal->b.SIGNAL];

            if (!scanlocon || V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE) {
                void *old=scanlocon;
                ptsig = tabsig[ptnoeud->signal->b.SIGNAL - 1];
                ptsig->TYPE = EXTERNAL;

                if (V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE)
                  nom=namealloc((char *)sc4->DATA);
                else
                  nom = (char *)ptsig->NAMECHAIN->DATA;
                
                strcpy (buf, nom);

                if (SPI_MERGE == 1)
                    stopchainsepar (buf);

                if (strcasecmp (nom, buf) != 0)
                    nom = namealloc (buf);

                scanlocon = addlocon (ptfig, nom, ptsig, UNKNOWN);
                if (lc_tab[ptnoeud->signal->b.SIGNAL]==NULL) lc_tab[ptnoeud->signal->b.SIGNAL] = scanlocon;

                // Le nom de signal doit être le même que celui du locon externe.

                for (sc3 = ptsig->NAMECHAIN; sc3; sc3 = sc3->NEXT)
                    if (sc3->DATA == nom)
                        break;

                if (!sc3 && !old) {
                    freechain (ptsig->NAMECHAIN);
                    ptsig->NAMECHAIN = NULL;
                    ptsig->NAMECHAIN = addchain (ptsig->NAMECHAIN, nom);
                }
                if ((ptuser=getptype(ptsig->USER, SPICE_MULTI_CONNECTOR_NAMES))!=NULL && !old)
                {
                  chain_list *cl;
                  int size=0;

                  if ((SPICE_KEEP_NAMES & KEEP__ALLSIGNALS) == 0) 
                  {
                    for (cl=(chain_list *)ptuser->DATA; cl!=NULL; cl=cl->NEXT)
                       ptsig->NAMECHAIN=append(ptsig->NAMECHAIN, addchain(NULL, cl->DATA));
                  }

                  strcpy(buf,"");
                  for (cl=(chain_list *)ptuser->DATA; cl!=NULL && size<1000; cl=cl->NEXT)
                     {
                       strcat(buf, " ");
                       strcat(buf, (char *)cl->DATA);
                       size+=strlen((char *)cl->DATA);
                     }
                   if (cl!=NULL) strcat(buf," ...");
                   strcat(buf, " ");
                   //if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE) avt_errmsg (SPI_ERRMSG, "073", AVT_WARNING, ptcir->NOM, nom, buf);
                   freechain((chain_list *)ptuser->DATA);
                   ptsig->USER=delptype(ptsig->USER, SPICE_MULTI_CONNECTOR_NAMES);
                }

            }

            if (ptnoeud->RCN) {
                if (!scanlocon->SIG->PRCN)
                    addlorcnet (scanlocon->SIG);

                for (verifpnode = scanlocon->PNODE; verifpnode; verifpnode = verifpnode->NEXT)
                    if (verifpnode->DATA == ptnoeud->RCN)
                        break;

                if (!verifpnode) {

                    setloconnode (scanlocon, ptnoeud->RCN);
                    ptnodename = getptype (scanlocon->USER, PNODENAME);

                    if (!ptnodename) {
                        scanlocon->USER = addptype (scanlocon->USER, PNODENAME, NULL);
                        ptnodename = scanlocon->USER;
                    }

                    if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom0) == 0)
                        ptnodename->DATA =
                            addchain (((chain_list *) (ptnodename->DATA)), namealloc (nom0));
                    else
                        ptnodename->DATA = addchain (((chain_list *) (ptnodename->DATA)), NULL);
                }
            }
        }
        if (maxidx > 0)
            mbkfree (lc_tab);
    }

    if (sc1 || sc2) {
        fflush (stdout);
        fprintf (stderr, "sc1 ou sc2.\n");
        EXIT (-1);
    }

    for (sc1 = ptcir->CINTERF, sc2 = sci->GINTERF; sc1 && sc2; sc1 = sc1->NEXT, sc2 = sc2->NEXT) {
        ptnoeud = (noeud *) (sc1->DATA);
        sc2->DATA = tabsig[ptnoeud->signal->b.SIGNAL - 1]->NAMECHAIN->DATA;
    }

    done = 0;
    unsolved_vcard=NULL;
    while (!done) {
        done = 1;
        for (scanvalim = ptcir->VALIM; scanvalim; scanvalim = scanvalim->SUIV) {
            float minus_value, value, val;
            int has_value;

            if (SPI_ZEROVOLT_RESI && scanvalim->TENSION==0) continue;
            
            if (scanvalim->NOM == (char *)-1)
                continue;
            plus_signal = tabsig[scanvalim->N1->signal->b.SIGNAL - 1];
            minus_signal = tabsig[scanvalim->N2->signal->b.SIGNAL - 1];
            has_value = getlosigalim (minus_signal, &minus_value);
            if (mbk_LosigIsVSS (minus_signal) || has_value) {
                if (!has_value) minus_value = 0.0;
                value=scanvalim->TENSION + minus_value;
                AdvancedNameAllocName (ptcir->ana, scanvalim->N1->index, nom0);
                mbk_addvcardnode(plus_signal, namealloc(nom0), value, scanvalim->N1->RCN);
                if (getlosigalim (plus_signal, &val) && fabs (val - value) > 1e-3) {
                   avt_errmsg (SPI_ERRMSG, "070", AVT_WARNING, ptfig->NAME, getsigname (plus_signal), val, value);
                 }
                else addlosigalim (plus_signal, value, scanvalim->EXPR);
                scanvalim->NOM = (char *)-1;
                done = 0;
            }
            else if (mbk_LosigIsVSS (plus_signal) || has_value) {
                if (!has_value) minus_value = 0.0;
                value=scanvalim->TENSION + minus_value;
                AdvancedNameAllocName (ptcir->ana, scanvalim->N2->index, nom0);
                mbk_addvcardnode(minus_signal, namealloc(nom0), value, scanvalim->N2->RCN);
                if (getlosigalim (minus_signal, &val) && fabs (val - value) > 1e-3) {
                   avt_errmsg (SPI_ERRMSG, "070", AVT_WARNING, ptfig->NAME, getsigname (plus_signal), val, value);
                 }
                else addlosigalim (minus_signal, value, scanvalim->EXPR);
                scanvalim->NOM = (char *)-1;
                done = 0;
            }
        }
    }
    for (scanvalim = ptcir->VALIM; scanvalim; scanvalim = scanvalim->SUIV) 
     {
       if (SPI_ZEROVOLT_RESI && scanvalim->TENSION==0) continue;
       if (scanvalim->NOM == (char *)-1)
           continue;
       plus_signal = tabsig[scanvalim->N1->signal->b.SIGNAL - 1];
       minus_signal = tabsig[scanvalim->N2->signal->b.SIGNAL - 1];
       // unsolved vcard
       uvc=(unsolved_vcard_type *)mbkalloc(sizeof(unsolved_vcard_type));
       uvc->s[0].name=NULL;
       uvc->s[0].u.sig=plus_signal;
       uvc->s[0].rcn=scanvalim->N1->RCN;
       AdvancedNameAllocName (ptcir->ana, scanvalim->N1->index, nom0);
       uvc->s[0].nodename=namealloc(nom0);
       uvc->s[1].name=NULL;
       uvc->s[1].u.sig=minus_signal;
       uvc->s[1].rcn=scanvalim->N2->RCN;
       AdvancedNameAllocName (ptcir->ana, scanvalim->N2->index, nom0);
       uvc->s[1].nodename=namealloc(nom0);
       uvc->value=scanvalim->TENSION;
       uvc->expr=scanvalim->EXPR;
       unsolved_vcard=addchain(unsolved_vcard, uvc);
     }

    for (sc1=ptcir->UNSOLVED_VCARDS; sc1!=NULL; sc1=sc1->NEXT)
      {
        uvc=(unsolved_vcard_type *)sc1->DATA;
        if (uvc->s[0].name==NULL)
          uvc->s[0].u.sig=tabsig[uvc->s[0].u.node->signal->b.SIGNAL - 1], uvc->s[0].rcn=uvc->s[0].u.node->RCN;
        if (uvc->s[1].name==NULL)
          uvc->s[1].u.sig=tabsig[uvc->s[1].u.node->signal->b.SIGNAL - 1], uvc->s[1].rcn=uvc->s[1].u.node->RCN;
      }
    unsolved_vcard=append(unsolved_vcard, ptcir->UNSOLVED_VCARDS);
    ptcir->UNSOLVED_VCARDS=NULL;
    if (unsolved_vcard!=NULL)
      ptfig->USER=addptype(ptfig->USER, SPICE_UNSOLVED_VCARDS, unsolved_vcard);


    for (scanlocon = ptfig->LOCON; scanlocon; scanlocon = scanlocon->NEXT) {
        scanlocon->PNODE = (num_list *)reverse ((chain_list *) scanlocon->PNODE);

        ptnodename = getptype (scanlocon->USER, PNODENAME);
        if (ptnodename) {
            for (sc1 = (chain_list *) (ptnodename->DATA); sc1; sc1 = sc1->NEXT)
                if (sc1->DATA)
                    break;
            if (sc1)
                ptnodename->DATA = reverse (((chain_list *) (ptnodename->DATA)));
            else {
                freechain ((chain_list *) ptnodename->DATA);
                scanlocon->USER = delptype (scanlocon->USER, PNODENAME);
            }
        }
        if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE && scanlocon->SIG->NAMECHAIN->NEXT == NULL && scanlocon->SIG->NAMECHAIN->DATA != scanlocon->NAME)
            scanlocon->NAME = scanlocon->SIG->NAMECHAIN->DATA;
    }

    ptfig->LOCON = (locon_list *)reverse ((chain_list *) ptfig->LOCON);

    for (scandiode = ptcir->DIODE; scandiode; scandiode = scandiode->SUIV) {
        int isdiode;
        isdiode = mbk_isdioden (scandiode->TYPE) || mbk_isdiodep (scandiode->TYPE);
        if (!isdiode) 
        {
           avt_errmsg (SPI_ERRMSG, "060", AVT_FATAL, scandiode->NOM?scandiode->NOM:"noname", scandiode->TYPE);
        }
        if (MLO_IS_TRANSN (mbk_getdiodetype (scandiode->TYPE)))
            scannoeud = scandiode->N2;
        else
            scannoeud = scandiode->N1;

        CheckNodeSupInfo (ptcir, scannoeud);

        scannoeud->SUPINFO->U.DIODE = addchain (scannoeud->SUPINFO->U.DIODE, scandiode);
    }

    /* On cree les transistors */

    for (scantrans = ptcir->TRANS; scantrans; scantrans = scantrans->NEXT) {
        int status_a, status_p;
        char *tmp_name;
        int istrans, isdiode, isjfet;

        if (scantrans->TYPE == 0) {
            tmp_name = getlotrsmodel (scantrans);
            istrans = mbk_istransn (tmp_name) || mbk_istransp (tmp_name);
            isdiode = 0; //mbk_isdioden (tmp_name) || mbk_isdiodep (tmp_name);
            isjfet = mbk_isjfetn (tmp_name) || mbk_isjfetp (tmp_name);
            if (!istrans && !isdiode && !isjfet) {
                if (scantrans->TRNAME)
                    avt_errmsg (SPI_ERRMSG, "032", AVT_FATAL, scantrans->TRNAME, tmp_name);
                else
                    avt_errmsg (SPI_ERRMSG, "032", AVT_FATAL, "no name", tmp_name);
            }
            if (istrans)
                scantrans->TYPE = mbk_gettranstype (tmp_name);
            else if (isdiode)
                scantrans->TYPE = mbk_getdiodetype (tmp_name);
            else if (isjfet)
                scantrans->TYPE = mbk_getjfettype (tmp_name);
        }

        DRAIN = (struct s_noeud *)scantrans->DRAIN;
        SOURCE = (struct s_noeud *)scantrans->SOURCE;
        GRILLE = (struct s_noeud *)scantrans->GRID;
        SUBST = (struct s_noeud *)scantrans->BULK;

        /* Merge diodes into source or drain */
        for (sc1 = SOURCE->SUPINFO ? SOURCE->SUPINFO->U.DIODE : NULL; sc1; sc1 = sc1->NEXT)
            if (scantrans->TYPE == mbk_getdiodetype (((diode *) sc1->DATA)->TYPE))
                break;
        if ((sc1 != NULL) && SPI_MERGE_DIODES && (scantrans->PS <0) && (scantrans->XS <0)) {
            getlotrsparam (scantrans, "as", NULL, &status_a);
            getlotrsparam (scantrans, "ps", NULL, &status_p);
            scandiode = (diode *) sc1->DATA;
            if (status_a == 0 && status_p == 0 && scandiode->A != 0.0 && scandiode->P != 0.0) {
                scantrans->PS = float2long (scandiode->P * SCALE_X * 1e6);
                scantrans->XS =
                    (scantrans->WIDTH !=
                     0) ? float2long (scandiode->A * (SCALE_X * 1e6) * (SCALE_X * 1e6) / (float)scantrans->WIDTH) : 0;
                SOURCE->SUPINFO->U.DIODE = delchain (SOURCE->SUPINFO->U.DIODE, sc1);
                scandiode->TYPE = NULL;
            }
        }
        for (sc1 = DRAIN->SUPINFO ? DRAIN->SUPINFO->U.DIODE : NULL; sc1; sc1 = sc1->NEXT)
            if (scantrans->TYPE == mbk_getdiodetype (((diode *) sc1->DATA)->TYPE))
                break;
        if ((sc1 != NULL) && SPI_MERGE_DIODES && (scantrans->PD <0) && (scantrans->XD < 0)) {
            getlotrsparam (scantrans, "as", NULL, &status_a);
            getlotrsparam (scantrans, "ps", NULL, &status_p);
            scandiode = (diode *) sc1->DATA;
            if (status_a == 0 && status_p == 0 && scandiode->A != 0.0 && scandiode->P != 0.0) {
                scantrans->PD = float2long (scandiode->P * SCALE_X * 1e6);
                scantrans->XD =
                    (scantrans->WIDTH !=
                     0) ? float2long (scandiode->A * (SCALE_X * 1e6) * (SCALE_X * 1e6) / (float)scantrans->WIDTH) : 0;
                DRAIN->SUPINFO->U.DIODE = delchain (DRAIN->SUPINFO->U.DIODE, sc1);
                scandiode->TYPE = NULL;
            }
        }
        // zinaps : finishing the transistor build

        scantrans->GRID = createlocon (scantrans, MBK_GRID_NAME, tabsig[GRILLE->signal->b.SIGNAL - 1]);
        scantrans->SOURCE = createlocon (scantrans, MBK_SOURCE_NAME, tabsig[SOURCE->signal->b.SIGNAL - 1]);
        scantrans->DRAIN = createlocon (scantrans, MBK_DRAIN_NAME, tabsig[DRAIN->signal->b.SIGNAL - 1]);
        scantrans->BULK =
            (SUBST != NULL) ? createlocon (scantrans, MBK_BULK_NAME, tabsig[SUBST->signal->b.SIGNAL - 1]) : NULL;

        if (ptcir->HASMODEL)
            scantrans->USER = addptype (scantrans->USER, TRANS_FIGURE, ptfig->NAME);

        pttrs = scantrans;

        if (DRAIN->RCN) {
            if (!pttrs->DRAIN->SIG->PRCN)
                addlorcnet (pttrs->DRAIN->SIG);
            setloconnode (pttrs->DRAIN, DRAIN->RCN);
            if (AdvancedNameAllocName (ptcir->ana, DRAIN->index, nom0) == 0)
                pttrs->DRAIN->USER = addptype (pttrs->DRAIN->USER, PNODENAME, addchain (NULL, namealloc (nom0)
                                               )
                    );
        }

        if (GRILLE->RCN) {
            if (!pttrs->GRID->SIG->PRCN)
                addlorcnet (pttrs->GRID->SIG);
            setloconnode (pttrs->GRID, GRILLE->RCN);
            if (AdvancedNameAllocName (ptcir->ana, GRILLE->index, nom0) == 0)
                pttrs->GRID->USER = addptype (pttrs->GRID->USER, PNODENAME, addchain (NULL, namealloc (nom0)
                                              )
                    );
        }

        if (SOURCE->RCN) {
            if (!pttrs->SOURCE->SIG->PRCN)
                addlorcnet (pttrs->SOURCE->SIG);
            setloconnode (pttrs->SOURCE, SOURCE->RCN);
            if (AdvancedNameAllocName (ptcir->ana, SOURCE->index, nom0) == 0)
                pttrs->SOURCE->USER = addptype (pttrs->SOURCE->USER, PNODENAME, addchain (NULL, namealloc (nom0)
                                                )
                    );
        }

        if (SUBST) {
            if (SUBST->RCN) {
                if (!pttrs->BULK->SIG->PRCN)
                    addlorcnet (pttrs->BULK->SIG);
                setloconnode (pttrs->BULK, SUBST->RCN);
                if (AdvancedNameAllocName (ptcir->ana, SUBST->index, nom0) == 0)
                    pttrs->BULK->USER = addptype (pttrs->BULK->USER, PNODENAME, addchain (NULL, namealloc (nom0)
                                                  )
                        );
            }
        }
    }

    ptfig->LOTRS = ptcir->TRANS;
    ptcir->TRANS = NULL;        // par sécurité

    for (i = 0; i < ptcir->nbnodes; i++) {
        scannoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
        if (scannoeud->index < 0)
            continue;
        if (scannoeud->SUPINFO) {
            freechain (scannoeud->SUPINFO->U.DIODE);
            scannoeud->SUPINFO->U.DIODE = NULL;
        }
    }

    for (scandiode = ptcir->DIODE; scandiode; scandiode = scandiode->SUIV) {
        losig_list *sig1, *sig2;
        loctc_list *ptctc;

        if (scandiode->TYPE == NULL)
            continue;

        sig1 = tabsig[scandiode->N1->signal->b.SIGNAL - 1];
        sig2 = tabsig[scandiode->N2->signal->b.SIGNAL - 1];

        // need to keep diode name in equivalent ptctc
        if (ptcir->HASMODEL)
          scandiode->PARAM = addoptparam (scandiode->PARAM, "diode_subckt", 0.0, ptfig->NAME);
    
        scandiode->PARAM = addoptparam (scandiode->PARAM, "diode_model", 0.0, scandiode->TYPE);

        if (!sig1->PRCN)
            addlorcnet (sig1);

        if (!sig2->PRCN)
            addlorcnet (sig2);

        val = 0;
        if (scandiode->P != 0 || scandiode->A != 0) {
            if (ptcir->HASMODEL) MCC_CURRENT_SUBCKT=ptfig->NAME;
            if (mcc_getsubckt( MCC_MODELFILE, scandiode->TYPE, MCC_DIODE, MCC_TYPICAL, scandiode->A, scandiode->P)==NULL)
            {
              val = mcc_calcDioCapa (MCC_MODELFILE, scandiode->TYPE,
                                     MCC_DIODE, MCC_TYPICAL,
                                     0.0, MCC_VDDmax, MCC_TEMP,
                                     (scandiode->A > 0.0) ? scandiode->A : 1.0,
                                     (scandiode->P > 0.0) ? scandiode->P : 0.0);
              freeoptparams (scandiode->PARAM);
              scandiode->PARAM = NULL;
            }
            MCC_CURRENT_SUBCKT=NULL;
        }
        ptctc = addloctc (sig1,
                          (scandiode->N1->RCN) ? scandiode->N1->RCN : 1,
                          sig2, (scandiode->N2->RCN) ? scandiode->N2->RCN : 1, val * SPI_SCALE_CAPAFACTOR * 1.0e12);
        if (ptctc) {
            rcn_setCapaDiode (ptctc, 1);
            if (scandiode->PARAM)
                ptctc->USER = addptype (ptctc->USER, OPT_PARAMS, scandiode->PARAM);
            if ((SPICE_KEEP_NAMES & KEEP__DIODE) == KEEP__DIODE && scandiode->NOM!=NULL)
               ptctc->USER = addptype (ptctc->USER, MSL_CAPANAME, namealloc(scandiode->NOM));
               
        }
        scandiode->PARAM = NULL;
    }
    /* On cree les instances */

    if (ptcir->INST) {
        htmodel = addht (1000);

        for (scaninst = ptcir->INST; scaninst; scaninst = scaninst->SUIV) {
            connec_sig = NULL;
            connec_noeud = NULL;
            connec_nom = NULL;
//          connec_con   = NULL;

            sci = (ginterf *) gethtitem (ALLINTERF->h, namealloc (scaninst->MODELE));

            if (sci == (ginterf *) EMPTYHT) {
                ALLINTERF = constinstmodel (ptcir, scaninst, ALLINTERF);
                if (ALLINTERF == NULL)
                    sci = NULL;
                else
                    sci = ALLINTERF->teteinterf;
            }
            else {
                checkinstmodel (ptcir, scaninst, sci);
            }

            if (!sci) {
                fflush (stdout);
                fprintf (stderr, "Internal error : can't find model %s.\n", scaninst->MODELE);
                EXIT (1);
            }

            if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE) htcon = linksameconnectors (sci->GINTERF, scaninst->IINTERF);

            /* Constitue la liste des signaux */
            for (sc1 = !V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE?sci->GINTERF:scaninst->IINTERF; sc1; sc1 = sc1->NEXT) {
                num_list *nm = NULL;
                chain_list *cl = NULL;

                if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE)
                {
                if ((l = gethtitem (htcon, sc1->DATA)) == 1)
                    continue;
                nodelist = reverse ((chain_list *) l);
                addhtitem (htcon, sc1->DATA, 1);
                }
                else
                {
                  nodelist=addchain(NULL, sc1->DATA);
                }
                for (sc2 = nodelist; sc2 != NULL; sc2 = sc2->NEXT) {
                    nm = addnum (nm, ((noeud *) (sc2->DATA))->RCN);

                    AdvancedNameAllocName (ptcir->ana, ((noeud *) (sc2->DATA))->index, nom0);
                    cl = addchain (cl, namealloc (nom0));
                }
                sc2 = nodelist;
                connec_sig = addchain (connec_sig, tabsig[((noeud *) (sc2->DATA))->signal->b.SIGNAL - 1]
                    );
                connec_noeud = addchain (connec_noeud, nm);
                connec_nom = addchain (connec_nom, cl);

                freechain (nodelist);
            }

            if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE) delht (htcon);

            nom = namealloc (sci->NOM);

            modele = (lofig_list *) gethtitem (htmodel, nom);
            if (modele == (lofig_list *) EMPTYHT) {
                modele = tetemodele = recuperemodele (tetemodele, nom, sci->GINTERF, sci->CORRESPINTERFNAME);
                addhtitem (htmodel, nom, (long)modele);
            }

            ptinst = addloins (ptfig, "*", modele, connec_sig);
            if ((SPICE_SF & SF_EXPLICITINSTANCENAMES) == 0)
                ptinst->INSNAME = namealloc (scaninst->NOM);
            else {
                sprintf (buf, "%s@%s", sci->NOM, scaninst->NOM);
                ptinst->INSNAME = namealloc (buf);
            }
            if (scaninst->PARAM != NULL) {
                for (ptparam = scaninst->PARAM; ptparam; ptparam = ptparam->NEXT) {
                    addloinsparam (ptinst, ptparam->NAME, ptparam->VALUE, ptparam->EXPR);
                }
                spi_freeinsparamlist (ptcir, scaninst->PARAM);
                scaninst->PARAM = NULL;
            }

            ptinst->USER = constphinterf (ptinst->USER, sci, 0);

#ifdef HANDLE_XY
            // XY coordinates
            xy = (xy_struct *) mbkalloc (sizeof (struct xy_struct));
            xy->X = scaninst->X;
            xy->Y = scaninst->Y;
            xy->Tx = scaninst->Tx;
            xy->Ty = scaninst->Ty;
            xy->R = scaninst->R;
            xy->A = scaninst->A;
            ptinst->USER = addptype (ptinst->USER, MSL_XY_PTYPE, xy);
#endif
            for (scancon = ptinst->LOCON, sc1 = connec_noeud, sc2 = connec_nom;
                 scancon; scancon = scancon->NEXT, sc1 = sc1->NEXT, sc2 = sc2->NEXT) {
                interf = (num_list *) sc1->DATA;
                scancon->USER = addptype (scancon->USER, PNODENAME, NULL);
                ptnodename = scancon->USER;

                if (interf->DATA) {
                    if (!scancon->SIG->PRCN)
                        addlorcnet (scancon->SIG);

                    for (; interf; interf = interf->NEXT) {
                        for (verifpnode = scancon->PNODE; verifpnode; verifpnode = verifpnode->NEXT)
                            if (verifpnode->DATA == interf->DATA)
                                break;
                        if ((!verifpnode) || (SPI_INSTANCE_MULTINODE == 1))
                            setloconnode (scancon, interf->DATA);
                    }

                    sc3 = (chain_list *) sc2->DATA;
                    for (; sc3; sc3 = sc3->NEXT)
                        ptnodename->DATA = addchain (((chain_list *) (ptnodename->DATA)), sc3->DATA);

                }
            }

            freechain (connec_sig);
            for (sc1 = connec_noeud; sc1; sc1 = sc1->NEXT)
                freenum (sc1->DATA);
            freechain (connec_noeud);
            for (sc1 = connec_nom; sc1; sc1 = sc1->NEXT)
                freechain (sc1->DATA);
            freechain (connec_nom);

            for (scancon = ptinst->LOCON; scancon; scancon = scancon->NEXT) {
                ptnodename = getptype (scancon->USER, PNODENAME);
                for (sc1 = (chain_list *) (ptnodename->DATA); sc1; sc1 = sc1->NEXT)
                    if (sc1->DATA)
                        break;
                if (!sc1) {
                    freechain ((chain_list *) ptnodename->DATA);
                    scancon->USER = delptype (scancon->USER, PNODENAME);
                }
            }
        }

        delht (htmodel);
    }

    if (SPI_DRIVE_SPEF_MODE)
    {
      for (e=ptcir->all_equi; e!=NULL; e=e->next)
        {      
          e->a.nom=getsigname(tabsig[ e->b.SIGNAL - 1 ]);
        }
    }

    for (e=ptcir->all_equi; e!=NULL; e=e->next)
    {
      int vssc=0, vddc=0;
      scanlosig=tabsig[ e->b.SIGNAL - 1 ];
      for (n = e->nodes; n != NULL; n = n->SUIV) {
        AdvancedNameAllocName (ptcir->ana, n->index, nom0);
        if (isglobalvss(nom0) || (scanlosig->TYPE==EXTERNAL && isvss(nom0)))
        {
           vssc++;
//           mbk_addvcardnode(scanlosig, namealloc(nom0), 0, n->RCN);
        }
        if (isglobalvdd(nom0) || (scanlosig->TYPE==EXTERNAL && isvdd(nom0)))
        {
           vddc++;
//           mbk_addvcardnode(scanlosig, namealloc(nom0), V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, n->RCN);
        }
      }
      if (vssc>0 && vddc>0)
      {
        char bufvss[4096];
        char bufvdd[4096];
        strcpy(bufvss,"");
        strcpy(bufvdd,"");
        for (n = e->nodes; n != NULL; n = n->SUIV)
        {
          AdvancedNameAllocName (ptcir->ana, n->index, nom0);
          if (isglobalvss(nom0) || (scanlosig->TYPE==EXTERNAL && isvss(nom0))) {strcat(bufvss, " "); strcat(bufvss, nom0);}
          if (isglobalvdd(nom0) || (scanlosig->TYPE==EXTERNAL && isvdd(nom0))) {strcat(bufvdd, " "); strcat(bufvdd, nom0);}
        }
        avt_errmsg (SPI_ERRMSG, "045", AVT_FATAL, ptcir->NOM, bufvss, bufvdd);
      }
    }


        
    ptfig->MODE = mode;
    mbkfree (tabsig);
    freelomodel (tetemodele);

    ptfig->LOTRS = (lotrs_list *) reverse ((chain_list *) ptfig->LOTRS);

    if (SPI_NORCCAPA_TOGND == 1) {
        long idx = 0;
        sigvss = NULL;

        for (ptsig = ptfig->LOSIG; ptsig != NULL; ptsig = ptsig->NEXT) {
            if (ptsig->INDEX > idx)
                idx = ptsig->INDEX;
            if (ptsig->PRCN != NULL) {
                if (ptsig->PRCN->PWIRE != NULL) {
                    sigvss = NULL;
                    break;
                }
            }
            if (sigvss == NULL) {
                if (mbk_LosigIsVSS (ptsig))
                    sigvss = ptsig;
            }
        }

        if (sigvss == NULL && ptsig == NULL) {
            sigvss = addlosig (ptfig, idx + 1, addchain (NULL, mbk_getvssname ()), INTERNAL);
            addlorcnet (sigvss);
        }

        if (sigvss != NULL) {
            for (scanlocon = ptfig->LOCON; scanlocon != NULL; scanlocon = scanlocon->NEXT) {
                if ((mbk_LosigIsVSS (scanlocon->SIG)) || (mbk_LosigIsVDD (scanlocon->SIG)))
                    continue;

                ptsig = scanlocon->SIG;

                if (ptsig->PRCN != NULL) {
                    if ((ptsig->PRCN->PCTC == NULL) && (rcn_getcapa (ptfig, ptsig) > 0.0))
                        addloctc (ptsig, (long)1, sigvss, (long)0, rcn_getcapa (ptfig, ptsig));
                }
            }
        }
    }

    lofigchain (ptfig);

    // zinaps was here, direction des connecteurs
    for (ptnodename = ptcir->INTERF_DIR; ptnodename != NULL; ptnodename = ptnodename->NEXT) {
        for (scanlocon = ptfig->LOCON; scanlocon && scanlocon->NAME != spi_devect (ptnodename->DATA); scanlocon = scanlocon->NEXT);    // un joli n2 pas trop mechant
        if (scanlocon != NULL) {
            scanlocon->DIRECTION = ptnodename->TYPE;
        }
    }

    // marquage des signaux globaux
    for (ptsig = ptfig->LOSIG; ptsig != NULL; ptsig = ptsig->NEXT)
        if ((best = getsigname (ptsig)) != NULL && (isglobalvss (best) || isglobalvdd (best)))
            ptsig->ALIMFLAGS |= MBK_HAS_GLOBAL_NODE_FLAG;

    // Zinaps: coherence entre les formats mbk, retour a la normal

    ptfig->LOCON = (locon_list *) reverse ((chain_list *) ptfig->LOCON);
    // Zinaps: c'est tout

    spi_post_traitment_connector_direction (ptfig);
    mbk_set_transistor_instance_connector_orientation_info (ptfig);

    // drive du spicedeck
    if ((ifl = getloadedinffig (ptcir->NOM)) != NULL) {
        chain_list *cl, *cl0;
        int oldmode;

        oldmode = inf_StuckSection (INF_DEFAULT_LOCATION);
        cl = inf_GetEntriesByType (ifl, INF_PIN_RISING_SLEW, INF_ANY_VALUES);
        cl0 = inf_GetEntriesByType (ifl, INF_PIN_FALLING_SLEW, INF_ANY_VALUES);
        inf_StuckSection (oldmode);

        if (cl != NULL || cl0 != NULL) {
            sprintf (buf0, "%s.spice.inf", ptcir->NOM);
            infDrive (ifl, buf0, INF_DEFAULT_LOCATION, NULL);
            freechain (cl);
            freechain (cl0);
        }
    }
//  mbk_debugstat(" :",0);
    return (ALLINTERF);
}

/******************************************************************************/
char *allocdecompligne (df, size, i)
spifile *df;
int size;
int i;
{
    char *pt;

    if ((df->lines[i].decompalloc == NULL) || (size > df->lines[i].decompfree)) {
        if (df->lines[i].decompalloc != NULL) {
            df->lines[i].chaindecomp = addchain (df->lines[i].chaindecomp, df->lines[i].decompalloc);
        }
        df->lines[i].decompalloc = (char *)mbkalloc (sizeof (char) * LONG_LIGNE * 2);
        df->lines[i].decompfree = 2 * LONG_LIGNE;
        df->lines[i].ptdecompalloc = df->lines[i].decompalloc;
    }
    df->lines[i].decompfree -= size;
    pt = df->lines[i].ptdecompalloc;
    df->lines[i].ptdecompalloc += size;

    return (pt);
}

/******************************************************************************/
void freedecompligne (df, i)
spifile *df;
int i;
{
    chain_list *chain;
    int j;

    /* extend line buffer if i beyond table */
    if (i >= df->linetablesize) {
        df->linetablesize += 4;
        df->lines = (spiline *) mbkrealloc (df->lines, df->linetablesize * sizeof (spiline));
        for (j = df->linetablesize - 4; j < df->linetablesize; j++) {
            df->lines[j].decomp = NULL;
            df->lines[j].decompalloc = NULL;
            df->lines[j].chaindecomp = NULL;
            df->lines[j].ptdecompalloc = NULL;
            df->lines[j].decompfree = 0;
            df->lines[j].linenum = 0;
        }
    }
    /* clean the line buffer indexed by i */
    for (chain = df->lines[i].chaindecomp; chain; chain = chain->NEXT)
        mbkfree (chain->DATA);
    freechain (df->lines[i].chaindecomp);
    df->lines[i].chaindecomp = NULL;
    if (df->lines[i].decompalloc != NULL) {
        df->lines[i].decompfree = 2 * LONG_LIGNE;
        df->lines[i].ptdecompalloc = df->lines[i].decompalloc;
    }
}

/******************************************************************************/
void movedecompligne (df)
spifile *df;
{
    char *alloc;
    char *pt;
    int free;
    chain_list *chain;
    int i, lastline;

    alloc = df->lines[0].decompalloc;
    free = df->lines[0].decompfree;
    chain = df->lines[0].chaindecomp;
    pt = df->lines[0].decompalloc;

    lastline = df->numlines - 1;

    for (i = 0; i < lastline; i++) {
        df->lines[i].decomp = df->lines[i + 1].decomp;
        df->lines[i].decompfree = df->lines[i + 1].decompfree;
        df->lines[i].ptdecompalloc = df->lines[i + 1].ptdecompalloc;
        df->lines[i].chaindecomp = df->lines[i + 1].chaindecomp;
        df->lines[i].decompalloc = df->lines[i + 1].decompalloc;
        df->lines[i].linenum = df->lines[i + 1].linenum;
        strcpy (df->lines[i].file_line, df->lines[i + 1].file_line);
    }

    df->lines[lastline].decompfree = free;
    df->lines[lastline].decompalloc = alloc;
    df->lines[lastline].ptdecompalloc = pt;
    df->lines[lastline].chaindecomp = chain;
}

static void display_decompligne (spifile * df, int i)
{
    chain_list *ptchain;
    for (ptchain = df->lines[i].decomp; ptchain; ptchain = ptchain->NEXT) {
        printf ("%s ", (char *)ptchain->DATA);
    }
    printf ("\n");
}

static void log_decompligne (spifile * df, int i, int loglevel)
{
    chain_list *ptchain;
    for (ptchain = df->lines[i].decomp; ptchain; ptchain = ptchain->NEXT) {
        avt_log(LOGSPI, loglevel, "%s ", (char *)ptchain->DATA);
    }
    avt_log (LOGSPI, loglevel, "\n");
}

chain_list *lireligne (spifile * df, circuit * ptcir)
{
    chain_list *sc;
    chain_list *del;
    char *pt, testc[2]=" ";
    int lastline;

    testc[0]=SPI_COM_CHAR;

    if (df->numlines < 2) {
        freedecompligne (df, 0);
        df->lines[0].decomp = decompligne (df, 0, NULL);
        df->lines[0].linenum = df->msl_line+1;
        strcpy (df->lines[0].file_line, df->file_line);
        df->numlines = 1;
    }
    else {
        freedecompligne (df, 0);
        movedecompligne (df);
        df->numlines--;
    }

    if (df->lines[0].decomp == NULL)    /* empty file */
        return (NULL);

    /* comment lines have no continuation */
    if (strcmp (df->lines[0].decomp->DATA, "*") == 0 || strcmp (df->lines[0].decomp->DATA, testc) == 0) {
        df->linenum = df->lines[0].linenum;
        return (df->lines[0].decomp);
    }

    lastline = 1;
    while (TRUE) {
        freedecompligne (df, lastline);
        df->lines[lastline].decomp = decompligne (df, lastline, NULL);
        df->lines[lastline].linenum = df->msl_line+1;
        strcpy (df->lines[lastline].file_line, df->file_line);

        /* break of EOF */
        if (df->lines[lastline].decomp == NULL)
            break;

        /* if comment then save line and check next line for '+' */
        if (strcmp (df->lines[lastline].decomp->DATA, "*") == 0 || strcmp (df->lines[lastline].decomp->DATA, testc) == 0) {
            if (spi_parse_comment (ptcir, &(df->lines[lastline]), df, FALSE) == TRUE)
                break;
            else {
                freechain (df->lines[lastline].decomp);
                continue;
            }
        }

        if (strcmp (df->lines[lastline].decomp->DATA, "+") != 0)
            break;

        /* concatonate lines separated by '+' */

        del = df->lines[lastline].decomp;
        df->lines[lastline].decomp = df->lines[lastline].decomp->NEXT;
        del->NEXT = NULL;
        freechain (del);

        for (sc = df->lines[lastline].decomp; sc; sc = sc->NEXT) {
            pt = (char *)sc->DATA;
            pt = allocdecompligne (df, strlen (pt) + 1, 0);
            strcpy (pt, (char *)sc->DATA);
            sc->DATA = pt;
        }

        for (sc = df->lines[0].decomp; sc->NEXT; sc = sc->NEXT);
        del = addchain (NULL, NULL);
        del->NEXT = df->lines[lastline].decomp;
        sc->NEXT = del;
        lastline = 1;
    }

    df->numlines = lastline + 1;
    if (ptcir != NULL)
        ptcir->TOTAL_LINE_COUNTER++;
    df->linenum = df->lines[0].linenum;
    return (df->lines[0].decomp);
}

/******************************************************************************/
static int is_spice_function (char *name)
{
    if (!strcasecmp ("pwl", name)) {
        return 1;
    }
    return 0;
}

/******************************************************************************/
static int decompligne_skipspace(char *line, int i)
{
  int run=i;
  while (line[run]==' ' && line[run]!='\0') run++;
  return run;
}

chain_list *decompligne (spifile * df, int type, char *EXTERNAL_LINE)
{
    char mot[LONG_LIGNE];
    int lg;
    chain_list *decomp = NULL;
    char motencours;
    int inquotes = 0, inbackslash=0, old_inbackslash;
    int incomment = 0;
    char endquote;
    char beginquote;
    char *element;
    int i, j = 0;
    int expect_expression = 0;
    int model_context = 0;

    do {
        old_inbackslash=inbackslash;
        inbackslash=0;
        if (!EXTERNAL_LINE && !df->savedf)
            df->msl_line++;

        if (inquotes == 0 && old_inbackslash==0) {
            decomp = NULL;
            motencours = 0;
        }

        if (!EXTERNAL_LINE) {
            fgets (df->file_line, LONG_LIGNE, df->df);
            if (!df->parsefirstline && df->msl_line == 0)
                continue;
            if (!V_BOOL_TAB[__MBK_SPI_IGNORE_CRYPT].VALUE && strncasecmp (df->file_line, ".PROT", 5) == 0) {
                spi_start_decrypt(df);
            }
            else if (!V_BOOL_TAB[__MBK_SPI_IGNORE_CRYPT].VALUE && strncasecmp (df->file_line, ".UNPROT", 7) == 0) {
                spi_end_decrypt(df);
            }

                            
            if (feof (df->df))
                return (decomp ? reverse (decomp) : NULL);

            i=strlen(df->file_line)-1;
            if (i>1 && df->file_line[i-1]=='\\')
            {
               if (i>2 && df->file_line[i-2]=='\\') {inbackslash=2; df->file_line[i-2]='\0';i-=2;}
               else {inbackslash=1; df->file_line[i-1]='\0';i--;}
            }
            i--;
            if (inbackslash==2) while (i>0 && isspace ((int)df->file_line[i])) {df->file_line[i]='\0'; i--;}
        }
        else
            strcpy (df->file_line, EXTERNAL_LINE);

        lg = strlen (df->file_line);

        if (lg == LONG_LIGNE - 1)
            avt_errmsg (SPI_ERRMSG, "048", AVT_FATAL, df->filename, df->msl_line);

        /* Un cas particulier : si le premier caractere de la ligne est
         * un '+'. Sur la suite de la ligne, le '+' sera considere comme un
         * caractere standard. */

        i = 0;
        while (isspace ((int)df->file_line[i]))
            i++;

        if (df->file_line[i] == '+' && old_inbackslash==0) {
            if (inquotes == 0) {
                element = allocdecompligne (df, 2, type);
                element[0] = df->file_line[i];
                element[1] = 0;
                decomp = addchain (decomp, element);
            }
            else
                mot[j++] = ' ';
            i++;
        }

        /* Keep comment token at start of line */

        if ((df->file_line[i] == '*' || df->file_line[i] == SPI_COM_CHAR) && inquotes == 0 && inbackslash==0) {
            incomment = 1;
            element = allocdecompligne (df, 2, type);
            element[0] = df->file_line[i];
            element[1] = 0;
            decomp = addchain (decomp, element);
            i++;
        }

        /* On traite le reste de la ligne */

        for (; i < lg; i++) {
            if (j >= LONG_LIGNE)
                avt_errmsg (SPI_ERRMSG, "048", AVT_FATAL, df->filename, df->msl_line);
            if (inquotes > 0) {
                if (df->file_line[i] == endquote) {
                    inquotes--;
                }
                else if (df->file_line[i] == beginquote && !incomment) {
                    inquotes++;
                }
                if (df->file_line[i] != '\n')
                    mot[j++] = df->file_line[i];
            }
            else if (isalnum ((int)(unsigned char)df->file_line[i]) ||
                     strchr ("\\:/$<>[].%_-|!+@#", df->file_line[i]) ||
                     (df->file_line[i] == '*' && motencours && j != 0)) {
                if (!motencours) {
                    motencours = 1;
                    j = 0;
                }
                mot[j++] = df->file_line[i];
            }
            else if (strchr ("\'{", df->file_line[i])
                     || ((motencours && df->file_line[i]=='(' && !model_context)
                         || (expect_expression && (df->file_line[decompligne_skipspace(df->file_line, i)] == '(')))) {
                if (expect_expression) {                  
                  // cas des fonctions style 'toto ( expr)' avec un espace apres le nom
                  i=decompligne_skipspace(df->file_line, i);                  
                }
                switch (df->file_line[i]) {
                case '\'':
                    beginquote = 0;
                    endquote = '\'';
                    break;
                case '{':
                    beginquote = '{';
                    endquote = '}';
                    break;
                case '(':
                    beginquote = '(';
                    endquote = ')';
                    break;
                }
                if (!incomment)
                    inquotes = 1;
                if (!motencours) {
                    motencours = 1;
                    j = 0;
                }
                mot[j++] = df->file_line[i];
            }
            else {
                if (motencours) {
                    if (mot[0] == '/' && j == 1) {
                        motencours = 0;
                        expect_expression = 0;
                    }
                }
                if (motencours && (!inbackslash || df->file_line[i]!='\0')) {
                    mot[j++] = 0;
                    element = allocdecompligne (df, j, type);
                    memcpy (element, mot, j);
                    if (decomp == NULL) {
                        if (!strcasecmp(mot, ".model")) model_context = 1;
                    }
                    decomp = addchain (decomp, element);
                    motencours = 0;
                    expect_expression = 0;
                    if (is_spice_function (mot))
                        expect_expression = 1;
                }

                if (strchr ("=$", df->file_line[i])) {
                    element = allocdecompligne (df, 2, type);
                    element[0] = df->file_line[i];
                    element[1] = 0;
                    decomp = addchain (decomp, element);
                }
                if (df->file_line[i] == '=')
                    expect_expression = 1;
                if (df->file_line[i] == '*' || df->file_line[i] == SPI_COM_CHAR)
                    break;
            }
        }
    } while (!decomp || inquotes > 0 || inbackslash>0);

    return (reverse (decomp));
}

/******************************************************************************/

void liberecircuit (ptcir)
circuit *ptcir;
{
    chain_list *scanchain;
    inst *scaninst;

    DeleteHeap (&ptcir->ha_equi);
    FreeAdvancedNameAllocator (ptcir->ana);
    FreeAdvancedTableAllocator (ptcir->ata);
    FreeAdvancedBlockAllocator (ptcir->aba);

    for (scaninst = ptcir->INST; scaninst; scaninst = scaninst->SUIV)
        freechain (scaninst->IINTERF);

    for (scanchain = ptcir->FREE; scanchain; scanchain = scanchain->NEXT)
        mbkfree (scanchain->DATA);

    freechain (ptcir->FREE);
    freechain (ptcir->CINTERF);
    DeleteNameAllocator (&ptcir->HASHGNAME);
    DeleteHeap (&ptcir->INSPARAMLIST);

    freethash (ptcir->HASHCAPA);
    freethash (ptcir->HASHRESI);
    freethash (ptcir->HASHVALIM);
    freethash (ptcir->HASHINST);
    freethash (ptcir->HASHTRAN);
    freethash (ptcir->HASHDIODE);
    freeptype (ptcir->INTERF_DIR);
    if (ptcir->CTX != NULL)
        eqt_term (ptcir->CTX);
    delht (ptcir->ALL_TRANSISTOR_MODELS);
    delht (ptcir->ALL_INSTANCE_MODELS);
    if (ptcir->ALL_DIODE_MODELS!=NULL) delht(ptcir->ALL_DIODE_MODELS);
    if (ptcir->ALL_CAPA_MODELS!=NULL) delht(ptcir->ALL_CAPA_MODELS);
    if (ptcir->ALL_RESI_MODELS!=NULL) delht(ptcir->ALL_RESI_MODELS);
    
    mbk_FreeREGEX (&ptcir->IGNORE_RESISTANCE);
    mbk_FreeREGEX (&ptcir->IGNORE_CAPACITANCE);
    mbk_FreeREGEX (&ptcir->IGNORE_DIODE);
    mbk_FreeREGEX (&ptcir->IGNORE_TRANSISTOR);
    mbk_FreeREGEX (&ptcir->IGNORE_INSTANCE);
    mbk_FreeREGEX (&ptcir->IGNORE_NAMES);
    if (ptcir->SUBCIRCUITS != NULL)
        delht (ptcir->SUBCIRCUITS);
    mbkfree (ptcir);
}

/******************************************************************************/

void CheckNodeSupInfo (circuit * ptcir, noeud * n)
{
    if (n->SUPINFO == NULL) {
        s_noeud_sup_info *si;
        si = (s_noeud_sup_info *) AdvancedBlockAlloc (ptcir->aba, sizeof (s_noeud_sup_info));
        si->ICNAME = NULL;
        si->SNOM = NULL;
        si->U.DIODE = NULL;
        si->names = NULL;
        n->SUPINFO = si;
    }
}

noeud *ajoutenoeud (circuit * ptcir, char *index, int mark)
{
    int val;
    char *fin;
    noeud *sn;
    char *nom;
    int i, exists;

    val = -1;
    nom = NULL;


    val = strtol (index, &fin, 10);
/*
  if( *fin==0 && val==0 ) {
    index=mbk_getvssname();
    fin=index;
    val=-1;
  }
  else*/
    if (*fin != '\0')
        val = -1;

    i = AdvancedNameAllocEx (ptcir->ana, index, &exists);
    if (exists) {
        sn = (noeud *) GetAdvancedTableElem (ptcir->ata, i);
        _spispef_complete_node (ptcir, index, sn, mark & 1, 0);
        return sn;
    }

    sn = (noeud *) GetAdvancedTableElem (ptcir->ata, i);

    ptcir->nbnodes = i + 1;

    sn->index = i;
    sn->RCN = 0;
    sn->SUPINFO = NULL;

    if (SIGNAME != NULL) {
        if (mark & 2) {
         CheckNodeSupInfo (ptcir, sn);

         sn->SUPINFO->SNOM = spi_devect (SIGNAME);
        }
        SIGNAME = NULL;
    }

    _spispef_complete_node (ptcir, index, sn, mark & 1, 1);

    return (sn);
}


/******************************************************************************/

void nomenoeud (ptcir, ptnoeud, ptnom, df)
circuit *ptcir;
noeud *ptnoeud;
char *ptnom;
spifile *df;
{
    char nom0[1024];

    if (AdvancedNameAllocName (ptcir->ana, ptnoeud->index, nom0) == 0) 
        avt_errmsg (SPI_ERRMSG, "051", AVT_FATAL, df->filename, df->linenum, nom0);

    ptnoeud->index = AdvancedNameAlloc (ptcir->ana, ptnom);
    if (ptnoeud->index >= ptcir->nbnodes) {
        // it's not an existing node for now
        ptnoeud = (noeud *) GetAdvancedTableElem (ptcir->ata, ptnoeud->index);
        ptnoeud->index = -1;
    }
}

/******************************************************************************/

char *spicenamealloc (ptcir, ptnom)
circuit *ptcir;
char *ptnom;
{
#if 0
    char *elem;
    char tmp[LONG_LIGNE];
    int l;

    l = 0;
    for (elem = ptnom; *elem != '\0'; elem++) {
        /* on enleve pas le \\ dans les noms
           if(*elem != '\\') 
         */
        {
            if (CASE_SENSITIVE == 'Y' || CASE_SENSITIVE == 'P')
                tmp[l] = *elem;
            else
                tmp[l] = tolower ((int)*elem);
            l++;
        }
    }
    tmp[l] = '\0';

    elem = getthashelem (tmp, ptcir->HASHGNAME, NULL);
    if (elem)
        return (elem);

    l = sizeof (char) * (strlen (tmp) + 1);
    if (ptcir->RESTENOM < l) {
        ptcir->ALLOUENOM = (char *)spiciralloue (ptcir, TAILLENOM);
        ptcir->RESTENOM = TAILLENOM;
    }

    elem = strcpy (ptcir->ALLOUENOM + (TAILLENOM - ptcir->RESTENOM), tmp);
    ptcir->RESTENOM = ptcir->RESTENOM - l;
    addthashelem (elem, elem, ptcir->HASHGNAME);
    return (elem);
#else
    return NameAlloc (&ptcir->HASHGNAME, ptnom);
#endif
}

/******************************************************************************/

void *spiciralloue (cir, taille)
circuit *cir;
int taille;
{
    void *pt;

    if (!cir) {
        fflush (stdout);
        fprintf (stderr, "*** mbkspi internal error *** : spiciralloue()\n");
        EXIT (1);
    }

    pt = mbkalloc (taille);
    cir->FREE = addchain (cir->FREE, pt);
    cir->TAILLE = cir->TAILLE + taille;
    return (pt);
}

/******************************************************************************/

void triecapa (ptcir)
circuit *ptcir;
{
    lowire_list *scanresi;
    char *n1, *n2;
    loctc_list *c1, *c2;
    noeud *tmp;
    char *vss;
    char *vdd;
    char *resiname;
    int idx;

    vdd = spicenamealloc (ptcir, VDD);
    vss = spicenamealloc (ptcir, VSS);

    for (scanresi = ptcir->RESI; scanresi; scanresi = scanresi->NEXT) {
        resiname = (char *)getptype (scanresi->USER, RESINAME)->DATA;
        n1 = mbkalloc (sizeof (char) * (strlen (resiname) + 2));
        n2 = mbkalloc (sizeof (char) * (strlen (resiname) + 2));

        sprintf (n1, "%s1", resiname);
        sprintf (n2, "%s2", resiname);

        c1 = NULL;
        c2 = NULL;

        c1 = getthashelem (n1, ptcir->HASHCAPA, NULL);
        c2 = getthashelem (n2, ptcir->HASHCAPA, NULL);

        if (!c1 || !c2) {
            scanresi->CAPA = 0.0;
            mbkfree (n1);
            n1 = NULL;
            mbkfree (n2);
            n2 = NULL;
            continue;
        }

        idx = AdvancedNameAlloc (ptcir->ana, vss);
        if (idx >= ptcir->nbnodes) {
            // it's not an existing node for now
            tmp = (noeud *) GetAdvancedTableElem (ptcir->ata, idx);
            tmp->index = -1;
        }

        if (((noeud *) c1->NODE2)->index == idx) {
            tmp = (noeud *) c1->NODE2;
            c1->NODE2 = c1->NODE1;
            c1->NODE1 = (long)tmp;
        }
        else if (((noeud *) c1->NODE1)->index != idx) {
            scanresi->CAPA = 0.0;
            mbkfree (n1);
            n1 = NULL;
            mbkfree (n2);
            n2 = NULL;
            continue;
        }

        if (((noeud *) c2->NODE2)->index == idx) {
            tmp = (noeud *) c2->NODE2;
            c2->NODE2 = c2->NODE1;
            c2->NODE1 = (long)tmp;
        }
        else if (((noeud *) c2->NODE1)->index != idx) {
            scanresi->CAPA = 0.0;
            mbkfree (n1);
            n1 = NULL;
            mbkfree (n2);
            n2 = NULL;
            continue;
        }

        if (c1->CAPA != c2->CAPA) {
            scanresi->CAPA = 0.0;
            mbkfree (n1);
            n1 = NULL;
            mbkfree (n2);
            n2 = NULL;
            continue;
        }

        if ((c1->NODE2 == scanresi->NODE1 && c2->NODE2 == scanresi->NODE2) ||
            (c2->NODE2 == scanresi->NODE1 && c1->NODE2 == scanresi->NODE1)) {
            /* On ne prendra plus ces capacites en compte */

            c1->USER = NULL;
            c2->USER = NULL;

            scanresi->CAPA = c1->CAPA + c1->CAPA;
        }
        else {
            scanresi->CAPA = 0.0;
            mbkfree (n1);
            n1 = NULL;
            mbkfree (n2);
            n2 = NULL;
            continue;
        }

        mbkfree (n1);
        n1 = NULL;
        mbkfree (n2);
        n2 = NULL;

    }
}

/******************************************************************************/

long float2long (v)
float v;
{
    long i;
    float r;

    i = v;
    r = v - i;

    if (r >= 0.5)
        return (i + 1);
    return (i);
}

/******************************************************************************/

char *retireextention (nom)
char *nom;
{
    char *pt;
    int i;

    pt = mbkstrdup (nom);

    /* On retire l'extention du nom, c'est a dire qu'on arrete la chaine au
     * dernier point rencontré */
    /* >0 : On laisse quand meme 1 caractere si le nom commence par un point */
    for (i = strlen (pt) - 1; i > 0; i--)
        if (pt[i] == '.') {
            pt[i] = 0;
            break;
        }

    return (pt);
}

/******************************************************************************/

lofig_list *recuperemodele (lofig_list *tetemodel, char *nom, chain_list *interf, chain_list *realinterf)
{
    lofig_list *ptmod;
    chain_list *sc1;
    char *nomdevec;
    ht *conht;

    ptmod = addlomodel (tetemodel, nom);

    conht = addht (10000);

    for (sc1 = !V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE?interf:realinterf; sc1; sc1 = sc1->NEXT) {
        if (gethtitem (conht, sc1->DATA) != EMPTYHT)
            continue;
        addhtitem (conht, sc1->DATA, 1);
        nomdevec = spi_devect ((char *)(sc1->DATA));
        ptmod->LOCON = addlocon (ptmod, nomdevec, NULL, 'X');
    }

    delht (conht);

    return (ptmod);
}

static int ext_handlers (line, process)
char *line;
int process;
{
    chain_list *ptchain;
    int (*ext_handler) ();
    int handled = FALSE;

    for (ptchain = ext_handler_list; ptchain; ptchain = ptchain->NEXT) {
        ext_handler = ptchain->DATA;
        if (ext_handler (line, process) != 0)
            handled = TRUE;
    }
    return handled;
}

static int cmp_handlers ()
{
    chain_list *ptchain;
    int (*cmp_handler) ();

    for (ptchain = cmp_handler_list; ptchain; ptchain = ptchain->NEXT) {
        cmp_handler = ptchain->DATA;
        cmp_handler ();
    }
    return 0;
}

/******************************************************************************/

void mslAddExtension (mslExtHandler)
int (*mslExtHandler) ();
{
    ext_handler_list = addchain (ext_handler_list, mslExtHandler);
}

/******************************************************************************/

void mslRmvExtension (mslExtHandler)
int (*mslExtHandler) ();
{
    chain_list *ptchain, *ptnextchain;
    chain_list *ptprevchain = NULL;

    for (ptchain = ext_handler_list; ptchain; ptchain = ptnextchain) {
        ptnextchain = ptchain->NEXT;
        if (ptchain->DATA == mslExtHandler) {
            if (ptprevchain == NULL) {
                ext_handler_list = ptnextchain;
            }
            else
                ptprevchain->NEXT = ptnextchain;
            ptchain->NEXT = NULL;
            freechain (ptchain);
            break;
        }
        ptprevchain = ptchain;
    }
}

/******************************************************************************/

void mslAddCompletion (mslCmpHandler)
int (*mslCmpHandler) ();
{
    cmp_handler_list = addchain (cmp_handler_list, mslCmpHandler);
}

/******************************************************************************/

void mslRmvCompletion (mslCmpHandler)
int (*mslCmpHandler) ();
{
    chain_list *ptchain, *ptnextchain;
    chain_list *ptprevchain = NULL;

    for (ptchain = cmp_handler_list; ptchain; ptchain = ptnextchain) {
        ptnextchain = ptchain->NEXT;
        if (ptchain->DATA == mslCmpHandler) {
            if (ptprevchain == NULL) {
                cmp_handler_list = ptnextchain;
            }
            else
                ptprevchain->NEXT = ptnextchain;
            ptchain->NEXT = NULL;
            freechain (ptchain);
            break;
        }
        ptprevchain = ptchain;
    }
}

/******************************************************************************/

char *spi_devect (nom)
char *nom;
{
    char buf[4096];
    return namealloc (_v2_spi_devect (nom, buf));
}


/* string="<>,(),_,." -> positionne la reconnaissance des vecteurs
   string=null        -> par defaut

   retour : 1 ok
            0 ko
*/



static char *_v2_spi_devect (char *nom, char *nouv)
{
    return mbk_devect_sub (nom, MBK_VECTOR_OPEN, MBK_VECTOR_CLOSE, MBK_VECTOR_SINGLE, nouv, isdigit);
}

/******************************************************************************/
chain_list *spi_expandvector (ptcir, name)
circuit *ptcir;
char *name;
{
    char buf[1024];
    char tmp[1024];
    char separ[2];
    char *radical, *ptleft, *ptright;
    int left, right;
    char close;
    int inc, index;
    char open = 0;
    chain_list *namechain = NULL;

    close = name[strlen (name) - 1];

    if (close == '>')
        open = '<';
    else if (close == ']')
        open = '[';
    if (open == 0)
        return NULL;
    strcpy (buf, name);
    separ[0] = open;
    separ[1] = 0;
    radical = strtok (buf, separ);
    separ[0] = ':';
    if ((ptleft = strtok (NULL, separ)) == NULL)
        return NULL;
    separ[0] = close;
    if ((ptright = strtok (NULL, separ)) == NULL)
        return NULL;
    left = atoi (ptleft);
    right = atoi (ptright);
    inc = (right > left) ? -1 : +1;
    for (index = right; index != left + inc; index += inc) {
        sprintf (tmp, "%s%c%d%c", radical, open, index, close);
        namechain = addchain (namechain, spicenamealloc (ptcir, tmp));
    }
    return namechain;
}

/******************************************************************************/

void stopchainsepar (chaine)
char *chaine;
{
    int i;
    if (!V_BOOL_TAB[__AVT_ENBALE_MULTI_CON_ON_NET].VALUE)
    {
      /* On retire le nombre en fin de nom et le SPI_SEPAR */
      for (i = strlen (chaine) - 1; i >= 0; i--)
          if (!isdigit ((int)chaine[i]))
              break;
  
      if (i > 0)
          if (chaine[i] == SPI_SEPAR)
              chaine[i] = '\0';
    }
}

/******************************************************************************/

insparam_list *spi_addinsparam (circuit * cir, insparam_list * head_param, char *name, float value, char *expr)
{
    insparam_list *ptparam;

    ptparam = AddHeapItem (&cir->INSPARAMLIST);
    ptparam->NEXT = head_param;
    ptparam->NAME = namealloc (name);
    ptparam->VALUE = value;
    if (expr)
        ptparam->EXPR = namealloc (expr);
    else
        ptparam->EXPR = NULL;

    return ptparam;
}

/******************************************************************************/

void spi_freeinsparamlist (circuit * cir, insparam_list * head_param)
{
    insparam_list *ptparam;
    insparam_list *ptnextparam;

    for (ptparam = head_param; ptparam; ptparam = ptnextparam) {
        ptnextparam = ptparam->NEXT;
        DelHeapItem (&cir->INSPARAMLIST, ptparam);
    }
}

int spi_canresolve (ptcir, ALLINTERF)
circuit *ptcir;
allinterf *ALLINTERF;
{
    ginterf *ptinterf;
//    ht             *htinst=NULL;
    char *tmp_name;
    chain_list *cl, *ch;
/*
    if(ptcir->INST) {
        htinst = addht(1000) ;
        for( ptinterf = ALLINTERF-> ; ptinterf ; ptinterf = ptinterf->SUIV ) {
            addhtitem(htinst,namealloc(ptinterf->NOM),(long)ptinterf);
        }
    }
*/
    ptcir->FAILNAME = NULL;

    cl = GetAllHTKeys (ptcir->ALL_INSTANCE_MODELS);

    for (ch = cl; ch != NULL; ch = ch->NEXT) {
/*    for (ptins = ptcir->INST; ptins; ptins = ptins->SUIV) {
        ptcir->FAILNAME = NULL;*/
        ptinterf = (ginterf *) gethtitem (ALLINTERF->h, namealloc (ch->DATA));
        if (ptinterf == (ginterf *) EMPTYHT) {
            if (SPI_AUTO_LOAD) {
                avt_errmsg (SPI_ERRMSG, "043", AVT_FATAL, (char *)ch->DATA); 
            }
            else {
                ptcir->FAILNAME = namealloc ((char *)ch->DATA);    //namealloc(ptins->MODELE);
                break;
            }
        }
    }
    freechain (cl);

//    if( htinst ) delht( htinst );
    if (ch != NULL)
        return FALSE;

    cl = GetAllHTKeys (ptcir->ALL_TRANSISTOR_MODELS);

    for (ch = cl; ch != NULL; ch = ch->NEXT) {
        /*
           for (lt=ptcir->TRANS; lt!=NULL; lt=lt->NEXT)
           { */
        tmp_name = (char *)ch->DATA;    //getlotrsmodel(lt);
        if (!(mbk_istransn (tmp_name) || mbk_istransp (tmp_name))) {
            ptcir->FAILNAME = tmp_name;
            break;
        }
    }

    freechain (cl);
    if (ch != NULL) return FALSE;

    cl = GetAllHTKeys (ptcir->ALL_DIODE_MODELS);
    for (ch = cl; ch != NULL; ch = ch->NEXT) {
        tmp_name = (char *)ch->DATA;    //getlotrsmodel(lt);
        if (!(mbk_isdioden (tmp_name) || mbk_isdiodep (tmp_name))) {
            ptcir->FAILNAME = tmp_name;
            break;
        }
    }
    freechain (cl);
    if (ch != NULL) return FALSE;

    cl = GetAllHTKeys (ptcir->ALL_RESI_MODELS);
    for (ch = cl; ch != NULL; ch = ch->NEXT) {
        tmp_name = (char *)ch->DATA;
        if (!getchain(RMODEL, tmp_name)) {
            ptcir->FAILNAME = tmp_name;
            break;
        }
    }
    freechain (cl);
    if (ch != NULL) return FALSE;

    cl = GetAllHTKeys (ptcir->ALL_CAPA_MODELS);
    for (ch = cl; ch != NULL; ch = ch->NEXT) {
        tmp_name = (char *)ch->DATA;
        if (!getchain(CMODEL, tmp_name)) {
            ptcir->FAILNAME = tmp_name;
            break;
        }
    }
    freechain (cl);
    if (ch != NULL) return FALSE;

    return TRUE;
}

static circuit *spi_getcircuit (chain_list * cirlist, char *name)
{
    chain_list *ptchain;
    for (ptchain = cirlist; ptchain; ptchain = ptchain->NEXT) {
        if (namealloc (((circuit *) ptchain->DATA)->NOM) == name)
            break;
    }
    if (ptchain != NULL)
        return (circuit *) ptchain->DATA;
    else
        return NULL;
}

void spi_displayunresolved (char *failname, chain_list * cirlist)
{
    circuit *ptcir;
    chain_list *failchain = NULL;

    failname = namealloc (failname);
    while ((ptcir = spi_getcircuit (cirlist, failname)) != NULL) {
        if (getchain (failchain, failname))
            break;
        failchain = addchain (failchain, failname);
        fprintf (stderr, " %s depends on", failname);
        failname = ptcir->FAILNAME;
    }
    fprintf (stderr, " %s is undefined.\n", failname);
    freechain (failchain);
}

/******************************************************************************/

char *spi_stripquotes (char *ptexp)
{
    int numquotes;
    char *ptscan;

    if ((*ptexp == '\'' && *(ptexp + strlen (ptexp) - 1) == '\'')
        || (*ptexp == '{' && *(ptexp + strlen (ptexp) - 1) == '}')) {
        numquotes = 1;
        for (ptscan = ptexp + 1; *ptscan == '{'; ptscan++)
            numquotes++;
        *(ptexp + strlen (ptexp) - numquotes) = 0;
        return ptexp + numquotes;
    }

    return ptexp;
}

static double spi_eval (spifile * df, eqt_ctx * ctx, char *ptexp, char **ptparamexpr, int failifrandom, int checkneg)
{
    double valeur;
    char *exp;
    int save_param;

    exp = spi_stripquotes (ptexp);

    valeur = eqt_eval (ctx, namealloc(exp), EQTFAST);

    if (failifrandom && eqt_resisrandom(ctx)) eqt_set_failedres(ctx);

    if (!eqt_resistrue (ctx))
        valeur = 0.0;

    if (eqt_resistrue (ctx) && !eqt_var_involved (ctx))
    {
        save_param = 0;
        if (!finite(valeur))
          avt_errmsg(SPI_ERRMSG, "080", AVT_ERROR, df?df->filename:"?", df?df->linenum:0, exp," returned NaN or Inf");
        else if (checkneg && valeur<0)
          avt_errmsg(SPI_ERRMSG, "080", AVT_ERROR, df?df->filename:"?", df?df->linenum:0, exp," returned negative value");
    }
    else
        save_param = 1;

    if (ptparamexpr) {
        if (save_param)
            *ptparamexpr = namealloc (exp);
        else
            *ptparamexpr = NULL;
    }

    return valeur;
}

/******************************************************************************/

char *spi_getextention (name)
char *name;
{
    char *path;

    path = filepath (name, NULL);

    if (path)
        return NULL;

    path = filepath (name, SPI_SUFFIX);

    if (path)
        return SPI_SUFFIX;

    avt_errmsg (SPI_ERRMSG, "009", AVT_FATAL, name);

    return NULL;
}

/******************************************************************************/

char *spi_getbasename (name)
char *name;
{
    char *basename, *buffer;
    char *s;
    int len, lensuffix;

    buffer = mbkalloc (strlen (name) + 1);

    s = strrchr (name, '/');
    if (s != NULL && strlen (s + 1) != 0) {
        strcpy (buffer, s + 1);
    }
    else
        strcpy (buffer, name);

    len = strlen (buffer);
    lensuffix = strlen (SPI_SUFFIX);
    if (len > lensuffix) {
        s = buffer + len - lensuffix - 1;
        if (!strcmp (s + 1, SPI_SUFFIX) && *s == '.')
            *s = 0;
    }

    basename = namealloc (buffer);
    mbkfree (buffer);
    return basename;
}

/******************************************************************************\
Fonctions pour gérer le pre-load en spice

Attention : ne pas changer le SPI_USE_PRELOADED_LIB en cours de route !!!
\******************************************************************************/

/* 

Passer dans la chain_list les noms des locons dans l'ordre du fichier 
spice.
exemple

.subckt inv e s e.1 s.2 e.2 vdd vss

il faut passer : "e", "s", "e", "s", "e", "vdd", "vss"

On ne gère pas le cas des connecteurs identiques sur l'interface d'un subckt.
exemple non supporté :
.subckt inv 1 1 2 vdd vss

Si on veut le faire un jour, il jour gérer la chain_list DUPCON.
*/

allinterf *spi_addlofiginterf (char *insname, chain_list * loconlist, allinterf * ALLINTERF)
{
    ginterf *ptinterf;
    chain_list *chain;

    ptinterf = (ginterf *) mbkalloc (sizeof (ginterf));
    ptinterf->NOM = insname;
    ptinterf->SUIV = ALLINTERF->teteinterf;
    ptinterf->GINTERF = loconlist;
    ptinterf->DUPCON = NULL;

    for (chain = loconlist; chain; chain = chain->NEXT)
        ptinterf->DUPCON = addchain (ptinterf->DUPCON, NULL);

    ALLINTERF->teteinterf = ptinterf;
    addhtitem (ALLINTERF->h, namealloc (ptinterf->NOM), (long)ptinterf);
    return ALLINTERF;
}

void spi_addpreloadedlib (char *insname, chain_list * loconlist)
{
    ginterf *ptinterf;
    chain_list *chain;

    ptinterf = (ginterf *) mbkalloc (sizeof (ginterf));
    ptinterf->NOM = insname;
    ptinterf->SUIV = SPI_PRELOADED_LIB;
    SPI_PRELOADED_LIB = ptinterf;
    ptinterf->GINTERF = loconlist;
    ptinterf->DUPCON = NULL;

    for (chain = loconlist; chain; chain = chain->NEXT)
        ptinterf->DUPCON = addchain (ptinterf->DUPCON, NULL);
}

allinterf *spi_initpreloadedlib (allinterf * ai)
{
    ginterf *gi;
    ai->teteinterf = NULL;
    ai->h = addht (10000);
    if (SPI_USE_PRELOADED_LIB == 'Y') {
        for (gi = SPI_PRELOADED_LIB; gi != NULL; gi = gi->SUIV)
            addhtitem (ai->h, namealloc (gi->NOM), (long)gi);
        ai->teteinterf = SPI_PRELOADED_LIB;
    }
    return ai;
}

void spi_savepreloadedlib (allinterf * ALLINTERF)
{
    ginterf *effinterf;
    ginterf *scaninterf;

    if (SPI_USE_PRELOADED_LIB == 'Y')
        SPI_PRELOADED_LIB = ALLINTERF->teteinterf;
    else {
        effinterf = NULL;
        for (scaninterf = ALLINTERF->teteinterf; scaninterf; scaninterf = scaninterf->SUIV) {
            if (effinterf) {
                mbkfree (effinterf->NOM);

                freechain (effinterf->CORRESPINTERFNAME);
                freechain (effinterf->GINTERF);
                freechain (effinterf->DUPCON);
                mbkfree (effinterf);
            }
            effinterf = scaninterf;
        }
        if (effinterf) {
            mbkfree (effinterf->NOM);

            freechain (effinterf->CORRESPINTERFNAME);
            freechain (effinterf->GINTERF);
            mbkfree (effinterf);
        }
    }
    delht (ALLINTERF->h);
}

