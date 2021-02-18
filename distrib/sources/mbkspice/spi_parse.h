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

#ifndef SPI_PARSEH
#define SPI_PARSEH

#define SPIEQUI 1664

//#define RESINAME 300703
#define TOPEXTNODE 031003
#define TOPEXTNAME 111003

#define TAILLENOM 131072
#define TAILLENOEUD 16
#define TAILLEINST 1024
#define TAILLEDIODE 1024

struct s_noeud;
struct s_circuit;
struct s_spifile;

#include "spi2spef.h"

extern eqt_ctx *GLOBAL_CTX;
extern int SPI_MODEL_CORNER;

typedef struct s_circuit
{
  lotrs_list 	        *TRANS;
  struct s_diode	*DIODE;
  lowire_list	        *RESI;
  loctc_list	        *CAPA;
  struct s_inst		*INST;
  struct s_valim        *VALIM;
  char			*NOM;
  struct s_noeud        *NOEUDLIBRE ;
  struct s_inst         *INSTLIBRE ;
  struct s_diode        *DIODELIBRE  ;
  chain_list		*CINTERF; 
  chain_list		*FREE;
//  tableint		*INT_NOEUD;
//  thash                 *NOM_NOEUD;
  NameAllocator 	HASHGNAME;
  thash			*HASHCAPA;
  thash			*HASHRESI;
  thash			*HASHVALIM;
  thash			*HASHINST;
  thash			*HASHTRAN;
  thash			*HASHDIODE;
  long                   TAILLE;
  char                  *ALLOUENOM;
  int                    RESTENOM;
  ptype_list            *INTERF_DIR; // zinaps
  eqt_ctx               *CTX;
  optparam_list         *PARAM;
  optparam_list         *LOCAL_PARAMS;
  int                    HASMODEL;
  char *FAILNAME;
  ht *ALL_DIODE_MODELS, *ALL_CAPA_MODELS, *ALL_RESI_MODELS;
  ht *ALL_TRANSISTOR_MODELS;
  ht *ALL_INSTANCE_MODELS;
//zinaps : again?!!?
  AdvancedNameAllocator *ana;
  AdvancedTableAllocator *ata;
  int nbnodes;
  AdvancedBlockAllocator *aba;

  HeapAlloc INSPARAMLIST; // zinaps

  HeapAlloc ha_equi;
  __equi_ *all_equi;
  struct s_spifile *sf;
  int pass;
  int nbequi;
  int TOTAL_LINE_COUNTER;
  FILE *of;
  mbk_match_rules IGNORE_RESISTANCE;
  mbk_match_rules IGNORE_CAPACITANCE;
  mbk_match_rules IGNORE_DIODE;
  mbk_match_rules IGNORE_TRANSISTOR;
  mbk_match_rules IGNORE_INSTANCE;
  mbk_match_rules IGNORE_NAMES;
  ht *SUBCIRCUITS;
  chain_list *UNSOLVED_VCARDS;
} circuit;

typedef struct s_icname
{
  struct s_icname       *SUIV;
  char                  *NOM;
  char                  *INSTANCE;
} icname ;

typedef struct
{
  char *SNOM;
  icname *ICNAME;
  union 
  {
    chain_list *DIODE;
    chain_list **SHORTCIRCUIT;
  } U;
  __names_ *names;
} s_noeud_sup_info;

typedef struct s_noeud
{
  struct s_noeud	*SUIV;
  int index;
//  char			*NOM;
//  char			*SNOM;
//  icname		*ICNAME;
//  int			 SIGNAL;
  int			 RCN;
//  int			 SPICE;
//  chain_list            *DIODE ;
//zinaps:
  struct __equi_ *signal;
  
//  __names_ *names;
  s_noeud_sup_info *SUPINFO;
} noeud;

typedef struct s_trans
{
  struct s_trans	*SUIV;
  struct s_noeud	*DRAIN;
  struct s_noeud	*SOURCE;
  struct s_noeud	*GRILLE;
  struct s_noeud	*SUBST;
  char			*TYPE;
  char			*NOM;
  float			 L;
  float			 W;
  float			 AS;
  float			 AD;
  float			 PS;
  float			 PD;
  float			 X;
  float			 Y;
} trans;

typedef struct insparam {
  struct insparam   *NEXT;
  char              *NAME;
  float              VALUE;
  char              *EXPR;
} insparam_list;

typedef struct s_inst
{
  struct s_inst		*SUIV;
  char			*NOM;
  char			*MODELE;
  chain_list		*IINTERF;
  insparam_list *PARAM;
  int			        X;
  int			        Y;
  int                   Tx;
  int                   Ty;
  short                 R;
  short                 A;
} inst;

typedef struct s_diode
{
  struct s_diode        *SUIV;
  struct s_noeud        *N1;
  struct s_noeud        *N2;
  char                  *NOM;
  char                  *TYPE;
  float                 P;
  float                 A;
  float			        X;
  float			        Y;
  optparam_list         *PARAM;
} diode;

typedef struct s_valim
{
  struct s_valim        *SUIV;
  float                  TENSION;
  struct s_noeud        *N1;
  struct s_noeud        *N2;
  char                  *NOM;
  char                  *EXPR;
} valim;

typedef struct s_interf
{
   struct s_interf      *SUIV;
   char                 *NOM;
   chain_list           *GINTERF;        /* DATA field : char* nom des locons */
   chain_list           *DUPCON;
   chain_list           *CORRESPINTERFNAME;
} ginterf;

typedef struct s_allinterf
{
  ginterf   *teteinterf;
  ht *h;
} allinterf;

#define LONG_LIGNE 20480

typedef struct s_spiline
{
  chain_list    *decomp;
  char          *decompalloc;
  chain_list    *chaindecomp;
  char          *ptdecompalloc;
  int            decompfree;
  int            linenum;
  char           file_line[LONG_LIGNE];
} spiline ;

typedef struct s_spifile
{
  FILE          *df;
  FILE          *savedf;
  int            encrypted;
  int            encryptedlines;
  int            numlines;
  int            linetablesize;
  spiline       *lines;
  char           file_line[LONG_LIGNE];
  int            linenum;
  int		     msl_line;
  char          *filename;
  int            parsefirstline;
} spifile ;

#define PARSEFIRSTLINE_NO  ((int)0)
#define PARSEFIRSTLINE_YES ((int)1)
#define PARSEFIRSTLINE_INC ((int)2 )

extern char SPI_PARSE_FIRSTLINE ;

typedef struct spi_load_global
{
  chain_list *ptcir_stack;
  int blackboxed;
} spi_load_global;

/* Fonctions d'allocation mémoire pour un circuit */

void*		spiciralloue  __P(( circuit*, int ));
void		liberecircuit __P(( circuit* ));

/* Fonctions pour parser le fichier au format Spice */

void spi_set_model_corner ( int corner );

circuit*	lirecircuit     __P(( chain_list**, allinterf* , lofig_list* , int *, spi_load_global *));
chain_list*	lireligne       __P(( spifile* , circuit*));
chain_list* replaceligne( spifile *df , circuit *ptcir , char *EXTERNAL_LINE);
chain_list*	decompligne     __P(( spifile* , int, char *));
noeud*		ajoutenoeud     __P(( circuit*, char*, int ));
void		nomenoeud       __P(( circuit*, noeud*, char*, spifile* ));
char*		spicenamealloc  __P(( circuit*, char* ));
float		spicefloat      __P(( char*, int* ));
chain_list*     recupereinterf  __P(( char* ));
void            loconinterf     __P(( lofig_list*, chain_list* ));
allinterf*        traiteinclude   __P(( allinterf*, char* , lofig_list*));
char*		retireextention __P(( char *nom ));
lofig_list*	recuperemodele  __P(( lofig_list*, char*, chain_list*, chain_list* ));
void    	checkinstmodel  __P(( circuit*, inst*, ginterf* ));
allinterf*	constinstmodel  __P(( circuit*, inst*, allinterf* ));
allinterf*	constinterf     __P(( circuit*, allinterf* ));
allinterf*	spiceloading    __P(( lofig_list*, char*, char*, char, allinterf* ));
allinterf*        spi_resolvecircuit (lofig_list *ptfig, circuit *ptcir, allinterf *teteinterf, char mode);
spifile*        spifileopen     __P(( char*, char*, int ));
void            spifileclose    (spifile *pt);
void            freedecompligne  __P(( spifile*, int ));
char *          allocdecompligne __P(( spifile*, int, int ));
void            movedecompligne  __P(( spifile* ));
int             spi_canresolve   __P(( circuit*, allinterf* ));
char*           spi_getextention __P(( char* ));
char*           spi_getbasename __P(( char* ));
allinterf*        spi_addlofiginterf __P(( char *, chain_list *, allinterf* ));
allinterf*        spi_initpreloadedlib __P(( allinterf *));
void            spi_savepreloadedlib __P(( allinterf *teteinterf ));
chain_list*     spi_expandvector __P(( circuit *, char * ));

/* Fonctions pour traiter les parametres */

insparam_list  *spi_addinsparam  __P((circuit *, insparam_list *, char *, float, char *));
void            spi_freeinsparamlist  __P((circuit *, insparam_list *));
/* Fonctions pour convertir la vue spice en vue Alliance */

allinterf*	constequi      __P(( circuit*, allinterf* ));
allinterf*	constlofig     __P(( circuit*, lofig_list*, allinterf*, char));
void		triecapa       __P(( circuit* ));
long          	float2long     __P(( float ));
ptype_list*     constphinterf  __P(( ptype_list*, ginterf*, int realinterftag));
char*           spi_devect     __P(( char* ));

int             nodenameisequi __P(( char*, char* ));
void            stopchainsepar __P(( char* ));
void            CheckNodeSupInfo (circuit *ptcir, noeud *n);
int             spi_decodvectorconfig      __P(( char *env ));
#endif
