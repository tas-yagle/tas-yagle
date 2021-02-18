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
 * Purpose : constantes, externs, and data-structures
 * Date    : 05/08/93
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 * $Id: mph_lib.h,v 1.2 2003/06/30 08:07:09 fabrice Exp $
 */

#ifndef _MPH_H_
#define _MPH_H_

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

/* physical connector orientation  */
#define NORTH       'N' 
#define EAST        'E' 
#define SOUTH       'S' 
#define WEST        'W' 

/* physical segment type */
#define LEFT         'L'
#define UP           'U'
#define RIGHT        'R'
#define DOWN         'D'

/* physical symetry type  */
#define NOSYM        0  /* vti : standart  */
#define ROT_P        1  /* vti : Rot90cc   */
#define SYMXY        2  /* vti : Rot180    */
#define ROT_M        3  /* vti : Rot90c    */
#define SYM_X        4  /* vti : mx(x=-x)  */
#define SY_RM        5  /* vti : mxRot90cc */
#define SYM_Y        6  /* vti : my(y=-y)  */
#define SY_RP        7  /* vti : myRot90cc */

/* physical contact type  */
#define CONT_POLY     0
#define CONT_DIF_N    1
#define CONT_DIF_P    2
#define CONT_BODY_N   3
#define CONT_BODY_P   4
#define CONT_VIA      5
#define CONT_VIA2     6
#define CONT_VIA3     7
#define CONT_VIA4     8
#define CONT_VIA5     9
#define CONT_VIA6     10
#define CONT_VIA7     11
#define CONT_VIA8     12
#define CONT_POLY2    13

/* physical transistor flood fill macros */
#define C_X_N        14
#define C_X_P        15

/* turn via */
#define CONT_TURN1   16
#define CONT_TURN2   17
#define CONT_TURN3   18
#define CONT_TURN4   19
#define CONT_TURN5   20
#define CONT_TURN6   21
#define CONT_TURN7   22
#define CONT_TURN8   23
#define CONT_TURN9   24

#define LAST_CONTACT  24

/* physical layer code  */
#define NWELL        0
#define PWELL        1
#define NTIE         2
#define PTIE         3
#define NDIF         4
#define PDIF         5
#define NTRANS       6
#define PTRANS       7
#define POLY         8
#define TPOLY        9
#define POLY2        10
#define TPOLY2       11
#define ALU1         12
#define TALU1        13
#define ALU2         14
#define TALU2        15
#define ALU3         16
#define TALU3        17
#define ALU4         18
#define TALU4        19
#define ALU5         20
#define TALU5        21
#define ALU6         22
#define TALU6        23
#define ALU7         24
#define TALU7        25
#define ALU8         26
#define TALU8        27
#define ALU9         28
#define TALU9        29
/* connector segment layer code */
#define CALU1        30
#define CALU2        31
#define CALU3        32
#define CALU4        33
#define CALU5        34
#define CALU6        35
#define CALU7        36
#define CALU8        37
#define CALU9        38

#define LAST_LAYER   38

/*******************************************************************************
* layout structures types                                                      *
*******************************************************************************/
typedef struct phfig                           /* physical figure             */
{
struct phfig *NEXT;                            /* next figure                 */
struct chain *MODELCHAIN;                      /* list of models              */
struct phseg *PHSEG;                           /* segment list head           */
struct phvia *PHVIA;                           /* contact list head           */
struct phref *PHREF;                           /* reference list head         */
struct phins *PHINS;                           /* instance list head          */
struct phcon *PHCON;                           /* connector list head         */
char         *NAME;                            /* figure name (unique)        */
char         MODE;                             /* if 'A' all , else perif     */
long         XAB1,YAB1,XAB2,YAB2;              /* abutment box coordinates    */
ptype_list *USER;                            /* Application specific        */
}
phfig_list;

typedef struct phcon                           /* physical connector          */
{
struct phcon *NEXT;                            /* next connector              */
char         *NAME;                            /* connector name              */
long         INDEX;                            /* connector index(unique)     */
long         XCON,YCON;                        /* connector coordinates       */
long         WIDTH;                            /* connector width             */
char         ORIENT;                           /* possible values: N E S W    */
char         LAYER;                            /* physical layer              */
ptype_list *USER;                            /* Application specific        */
}
phcon_list;

typedef struct phseg                           /* physical (symbolic) segment */
{
struct phseg    *NEXT;                         /* next segment                */
char            *NAME;                         /* segment name                */
long             X1,Y1,X2,Y2;                  /* segment coordinates         */
long             WIDTH;                        /* segment width               */
char             TYPE;                         /* possibles values: L U R D   */
char             LAYER;                        /* physical layer              */
ptype_list    *USER;                         /* Application specific        */
}
phseg_list;

typedef struct phvia                           /* physical contact            */
{
struct phvia    *NEXT;                         /* next contact                */
char            *NAME;                         /* via name                    */
char             TYPE;                         /* contact type: see define    */
long             XVIA,YVIA;                    /* coordinates                 */
long             DX,DY;                        /* size of big vias            */
ptype_list    *USER;                         /* Application specific        */
}
phvia_list;

typedef struct phref                           /* physical reference          */
{
struct phref    *NEXT;                         /* next reference              */
char            *FIGNAME;                      /* reference type              */
char            *NAME;                         /* reference name (unique)     */
long             XREF,YREF;                    /* coordinates                 */
ptype_list    *USER;                         /* Application specific        */
}
phref_list;

typedef struct phins                           /* physical instance           */
{
struct phins    *NEXT;                         /* next instance               */
char            *INSNAME;                      /* instance name (unique)      */
char            *FIGNAME;                      /* model name                  */
long             XINS,YINS;                    /* coordinates                 */
char             TRANSF;                       /* possible values: cf define  */
ptype_list    *USER;                         /* Application specific        */
}
phins_list;

/*******************************************************************************
* externals for physical view                                                  *
*******************************************************************************/
extern phfig_list *HEAD_PHFIG;                 /* physical figure list head   */
  extern    phfig_list * addphfig __P((char *figname));
  extern    phins_list * addphins __P((phfig_list *ptfig, char *figname, char *insname, char sym, long x, long y));
  extern    phvia_list * addphvia __P((phfig_list *ptfig, char viatype, long x, long y, long dx, long dy, char *name));
  extern    phref_list * addphref __P((phfig_list *ptfig, char *type, char *name, long x, long y));
  extern    phseg_list * addphseg __P((phfig_list *ptfig, char layer, long width, long x1, long y1, long x2, long y2, char *nodename));
  extern    phcon_list * addphcon __P((phfig_list *ptfig, char orient, char *conname, long x, long y, char layer, long width));
  extern           void  defab __P((phfig_list *ptfig, long x1, long y1, long x2, long y2));
  extern    phfig_list * getphfig __P((char *figname, char mode));
  extern    phins_list * getphins __P((phfig_list *ptfig, char *insname));
  extern    phcon_list * getphcon __P((phfig_list *ptfig, char *conname, long index));
  extern    phref_list * getphref __P((phfig_list *ptfig, char *refname));
  extern            int  delphcon __P((phfig_list *ptfig, phcon_list *ptdelcon));
  extern            int  delphins __P((phfig_list *ptfig, char *insname));
  extern            int  delphfig __P((char *name));
  extern            int  delphvia __P((phfig_list *ptfig, phvia_list *ptvia));
  extern            int  delphseg __P((phfig_list *ptfig, phseg_list *ptseg));
  extern            int  delphref __P((phfig_list *ptfig, phref_list *ptref));
  extern           void  xyflat __P((long *xout, long *yout, long x, long y, long xins, long yins, long x1, long y1, long x2, long y2, char trsf));
  extern           void  viewph __P(());
  
#endif /* !MPH */

