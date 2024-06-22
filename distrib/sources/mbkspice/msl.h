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
#ifndef MSLH
#define MSLH

#include EQT_H

#define SPI_INSPARAM_PTYPE 141200
#define SPI_TRSPARAM_PTYPE 151200

#define RESINAME 300703
#define MSL_CAPANAME 300704

#define TPARAM_L  1
#define TPARAM_W  2
#define TPARAM_AS 3
#define TPARAM_AD 4
#define TPARAM_PS 5
#define TPARAM_PD 6

typedef struct trsparam {
  struct trsparam   *NEXT;
  char              *NAME;
  int                FIELD;
} trsparam_list;

#define RPARAM_R   1
#define RPARAM_TC1 2
#define RPARAM_TC2 3

typedef struct resparam {
  struct trsparam   *NEXT;
  char              *NAME;
  int                FIELD;
} resparam_list;

extern trsparam_list  *spi_addtrsparam  __P((trsparam_list *, char *, int));
extern void            spi_freetrsparamlist  __P((trsparam_list *));
extern resparam_list  *spi_addresparam  __P((resparam_list *, char *, int));
extern void            spi_freeresparamlist  __P((resparam_list *));

extern int          msl_line;        /* current line number */
extern int          msl_figloaded;   /* TRUE if inside a SUBCKT */
extern char        *msl_figname;     /* name of main SUBCKT */
extern char        *msl_subckt;      /* name of current SUBCKT */
extern lofig_list  *msl_ptfig;       /* last lofig parsed */

extern char         SPI_PARSE_ALLNAME; /* parse all transistor names */
extern char         SPI_IGNORE_BULK;   /* ignore bulk connections */

void mslAddExtension __P(( int(*)() )); /* add function called during parsing */
void mslRmvExtension __P((          )); /* remove this function */

void mslAddCompletion __P(( int(*)() )); /* add function called after parsing */
void mslRmvCompletion __P((          )); /* remove this function */

void spi_addpreloadedlib __P(( char *insname, chain_list *loconlist ));

void parsespice       __P(( char* ));
void spicesavelofigsinfile( chain_list *cl, FILE *df);
void spicesavelofiginfile( lofig_list *ptfig, FILE *df, int driveincludes);

void spiceprintdate( char *circuit, FILE *df );
void spi_vect         __P(( char* ));

extern int SPI_DRIVE_SPEF_MODE;

/*** Fonctions utilisateurs pour configurer le driver ***/
extern void spi_setfuncinode( void (*fn)( FILE*, lofig_list*, void* ), void *data );
extern num_list* spi_getinode( locon_list *locon );
extern void spi_setinode( locon_list *locon, num_list *head );
extern void spi_clearinode( locon_list *locon );

eqt_ctx *spi_init_eqt ();
extern float SPI_SCALE_CAPAFACTOR;
extern float SPI_SCALE_RESIFACTOR;
extern float SPI_SCALE_TRANSFACTOR;
extern float SPI_SCALE_DIODEFACTOR;

#ifndef TRUE
#define TRUE (1==1)	/* c'est ben vrai	*/
#endif

#ifndef FALSE
#define FALSE (1==0)	/* tout faux		*/
#endif

extern int SPI_MODEL_CORNER;
void spi_set_model_corner ( int corner );

#define SPI_NONODES (-1l)
char*  spinamenode          __P(( losig_list*, long ));
char*  spinamenodedetail     __P(( losig_list*, long, char ));
void   spi_print            __P((FILE *, ...));

/* Capacite minimum acceptable par Spice en pf */
extern float CAPAMINI ;
/* Resistance minimum acceptable par Spice en ohm */
extern float RESIMINI ;

extern int SPI_TRANSISTOR_AS_INSTANCE;
extern int SPI_LOWRESISTANCE_AS_TENSION;

extern char* spi_makename __P(( char* ));
extern char* SPI_SUFFIX;
extern char SPI_INS_SEPAR ;
extern char SPI_REPLACE_INS_SEPAR ;
extern void spi_env ();


#endif
