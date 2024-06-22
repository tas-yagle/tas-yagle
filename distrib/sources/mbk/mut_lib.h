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
 * Date    : 31/08/93
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 * $Id: mut_lib.h,v 1.184 2009/06/02 10:50:30 fabrice Exp $
 */

#ifndef _MUT_H_
#define _MUT_H_

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <semaphore.h>

//#include <dmalloc.h>

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

/* If set, disable heap allocation for addchain */
//#define NOHEAPALLOC
//#define NOHEAPALLOCFORHEAP

//#define ENABLE_STATS
//#define MALLOC_HISTOGRAM

/* namealloc hash table size: the 1230th prime */
//#define HASHVAL     10007
#define HASHVAL     100003

//#define USE_DEFAULT_RANDR

/* hash tables management defines */
#define EMPTYHT     -1
#define DELETEHT    -2


/* read-write text-files opening codes: */
#define READ_TEXT   "r" /* read  access */
#define WRITE_TEXT  "w" /* write access */

/* chain buffer size */
#define BUFSIZE     1024
#define SIGSIZE     1024

/* USER field codes  */
#define LOFIGCHAIN  4592
#define LOFIGCHAIN_LOCAL  4593
#define PLACEABOX   5749
#define VTIPARSER   9716
#define CHECKFEED   6996
#define UNFLATLOFIG 1992
#define UNFLATRC    1984
#define PTSIGSIZE   8111
#define MSL_XY_PTYPE 20040519
#define MBK_SIGNALNAME_NODE  0xfab90511
#define MBK_VCARD_NODES  0xfab90512

#define BBOX_AS_UNUSED 0xfab90528

/* Maximum number of simultaneous terminated process */
#define ENDCHLD_MAX     1024
/* Maximum number of autoacknowledge process ( should be the same as
 *  END_CHLD_MAX ) */
#define AUTOACKCHLD_MAX 1024

/* concatenate names while flattening or copying up */
#define YES         'Y'
#define NO          'N'

/* endianess of vectors */
#define MBK_BIG_ENDIAN 1
#define MBK_LITTLE_ENDIAN 1

/* supply voltage scale */
#define SCALE_ALIM (float)10000

/*******************************************************************************
* macro definitions                                                            *
*******************************************************************************/
#define isvdd(name)	wildstrstr(name, VDD)
#define isvss(name)	wildstrstr(name, VSS)
#define isglobalvdd(name)	wildstrstr(name, GLOBAL_VDD)
#define isglobalvss(name)	wildstrstr(name, GLOBAL_VSS)
#define funcin()     if (MBK_DEBUG_ON)                                \
                        HEAD_MBKDEBUG = addptype(HEAD_MBKDEBUG,       \
                                                 __LINE__,            \
                                                 (void *)__FILE__)
#define funcout()    if (MBK_DEBUG_ON && HEAD_MBKDEBUG)               \
                        HEAD_MBKDEBUG = delptype(HEAD_MBKDEBUG,       \
                                                 HEAD_MBKDEBUG->TYPE)

#ifdef MAXLONG
#undef MAXLONG
#define MAXLONG (long) (((unsigned long)((long) - 1)) >> 1)
#endif

#define ROT( x, n ) ((x)<<(n) | (x)>>(8*sizeof(unsigned int)-(n)))

/*******************************************************************************
* service structures types                                                     *
*******************************************************************************/

typedef struct xy_struct 
{
    int X;
    int Y;
    int Tx;
    int Ty;
    short R;
    short A;
} xy_struct;

typedef struct point                           /* point list                  */
{
struct point *NEXT;                            /* next point                  */
long         INDEX;                            /* index (unique)              */
long         X,Y;                              /* coordinates                 */
}
point_list;

typedef struct chain                           /* pointer list, lisp lists    */
{
struct chain *NEXT;                            /* next pointer                */
void         *DATA;                            /* generic pointer             */
}
chain_list;

typedef struct ptype                           /* typed pointer list          */
{
struct ptype *NEXT;                            /* next pointer                */
long         TYPE;                             /* type                        */
void         *DATA;                            /* generic pointer             */
}
ptype_list;

typedef struct num                             /* integer list                */
{
struct num *NEXT;                              /* next integer                */
long       DATA;                               /* integer value               */
}
num_list;

typedef struct endchld                         /* list of ended process       */
{
  struct endchld *NEXT;
  int             PID;
  int             STATUS;
} endchld_list;

typedef struct autoackchld                     /* list of process auto-ack    */
{
  struct autoackchld *NEXT;
  int                 PID;
} autoackchld_list;

/******************************************************************************
* hash table structures types                                                 *
******************************************************************************/
typedef struct htitem {                        /* hash table element          */
	void *key;                                  /* pointer for access          */
	long value;                                 /* associated value            */
} htitem;

typedef struct htable {                        /* hash table itself           */
	long length;                                /* table length                */
	htitem *pElem;                              /* pointer to table start      */
	long count;                                 /* number of inserted elements */
} ht;

typedef struct ititem {
        int length;                  
        int size;                  
        long *data;                  
} it;

typedef struct vtitem {
        int length;                  
        int size;                  
        void **data;                  
} voidt;

typedef struct dtitem {
        int length;                  
        int size;                  
        double *data;                  
} dt;

struct lofig;

/*******************************************************************************
* mbk externals for the global variables defined in mbk_util.c                 *
*******************************************************************************/
extern chain_list *HEAD_CHAIN;                 /* chain list buffer head      */
extern chain_list *HEAD_BLOCK_CHAIN;
extern ptype_list *HEAD_PTYPE;                 /* ptype list buffer head      */
extern num_list   *HEAD_NUM;                   /* num list buffer head        */
extern char       TRACE_MODE;                  /* trace if Y                  */
extern char       DEBUG_MODE;                  /* debug if Y                  */
extern char       FAST_MODE;                   /* no consistency control if Y */
extern char       *WORK_LIB;                   /* working directory           */
extern char       **CATA_LIB;                  /* read only directory         */
extern char       *CATAL;                      /* catalog file name           */
extern char       SEPAR;                       /* char used for concatenation */
extern char       IN_LO[];                     /* logical input format        */
/* par Fabrice le 7/2/2002 */
extern char       IN_PARASITICS[];             /* parasitic input format      */
extern int        MBK_LOAD_PARA;         /* load parasitics enabler     */
/* -----       le 8/2/2002 */
extern int        MBK_DEVECT;               /* devect signal name enabler  */
/* ----- */
extern char       IN_PH[];                     /* physical input format       */
extern char       OUT_LO[];                    /* logical output format       */
extern char       OUT_PH[];                    /* physical output format      */
extern long       SCALE_X;                     /* distance scale parameter    */
extern char       PARSER_INFO[];               /* version number, and so on   */
extern char       *VDD;                        /* user name for power high    */
extern char       *VSS;                        /* user name for power ground  */
extern char       *GLOBAL_VDD;                 /* user name for power high    */
extern char       *GLOBAL_VSS;                 /* user name for power ground  */
extern long       VDD_VSS_THRESHOLD;           /* Threshold between VDD & VSS */
extern char       *MBK_BBOX_NAME;          /* blackbox list file name     */
extern char       MBK_NO_TRNAME;               /* do not save transistor name */
extern ptype_list *HEAD_MBKDEBUG;              /* function debug list         */
extern char       MBK_DEBUG_ON;                /* mode for debug on/off       */
extern char       *IN_FILTER;
extern char       *OUT_FILTER;
extern char       *FILTER_SFX;
extern char       MBKFOPEN_NAME[];             /* dernier fichier ouvert      */
extern char       MBKFOPEN_FILTER;
extern endchld_list *HEAD_ENDCHLD;             /* list of terminated process  */
extern endchld_list *HEAD_ENDCHLD_FREE;
extern int        STAT_ENDCHLD;
extern autoackchld_list *HEAD_AUTOACKCHLD;     /* list of terminated process  */
extern autoackchld_list *HEAD_AUTOACKCHLD_FREE;
extern int        STAT_AUTOACKCHLD;
extern char       MBK_DUP_PNAME_FF;
extern char SPI_VECTOR[1024];
extern char CASE_SENSITIVE;
extern char *MBK_BULK_NAME,*MBK_GRID_NAME,*MBK_DRAIN_NAME,*MBK_SOURCE_NAME;
extern FILE *MBKLOGFILE;

extern chain_list *TNMOS, *TPMOS, *CRYPTMOS;
extern chain_list *DNMOS, *DPMOS;
extern chain_list *JFETN, *JFETP;

extern char **MBK_NRS;
extern char **MBK_NRD;
extern char **MBK_MULU0;
extern char **MBK_DELVT0;
extern char **MBK_SA;
extern char **MBK_SB;
extern char **MBK_SD;
extern char **MBK_SC;
extern char **MBK_SCA;
extern char **MBK_SCB;
extern char **MBK_SCC;
extern char **MBK_NF;
extern char **MBK_NFING;
extern char **MBK_M;
extern char **MBK_MULT;
extern char **MBK_GEOMOD;
extern char **MBK_ABSOURCE;
extern char **MBK_LSSOURCE;
extern char **MBK_LGSOURCE;
extern char **MBK_ABDRAIN;
extern char **MBK_LSDRAIN;
extern char **MBK_LGDRAIN;

extern char FLATTEN_KEEP_ALL_NAMES;

extern void (*MBK_EXIT_FUNCTION)(int);           /* Added by Ludovic Jacomme */
extern char MBK_EXIT_KILL;                    /* Added by Ludovic Jacomme */

#define mbkexit(x) {                                    \
  if(MBK_EXIT_KILL == 'Y') kill( getpid(), SIGTERM ) ;  \
  else if ( MBK_EXIT_FUNCTION != NULL )                 \
    {                                                   \
      (*MBK_EXIT_FUNCTION)( x );                        \
    }                                                   \
  else exit(x) ;                                        \
                                                        \
  while(1);                                             \
}

#define EXIT(x) {                                    \
  if(MBK_EXIT_KILL == 'Y') kill( getpid(), SIGTERM ) ;  \
  else if ( MBK_EXIT_FUNCTION != NULL )                 \
    {                                                   \
      (*MBK_EXIT_FUNCTION)( x );                        \
    }                                                   \
  else exit(x) ;                                        \
                                                        \
  while(1);                                             \
}

// extern void mbkexit __P(( int ExitValue ));   /* new exit handler system  */

extern char              MBK_VECTOR_OPEN[256];
extern char              MBK_VECTOR_CLOSE[256];
extern char              MBK_VECTOR_SINGLE[256];

/*******************************************************************************
* externals for mbk_util.c functions                                           *
*******************************************************************************/

  extern           void  mbkenv __P(());
  extern           void  readlibfile __P((char *type,void (*fonc)(char *),int reload));
  extern          char * nameindex __P((char *name, long index));
  extern      num_list * addnum __P((num_list *ptnum, long data));
  extern      num_list * getnum __P((num_list *ptnum, long data));
  extern           void  freenum __P((num_list *pt));
  extern       num_list* dupnumlst __P(( num_list *head ));

  extern    chain_list * addchain __P((chain_list *pthead, void *ptdata));
  extern int countchain(chain_list *doubl);
  extern chain_list *getchain(chain_list *ptchain, void *data);
  extern    chain_list * delchain __P((chain_list *pthead, chain_list *ptdel));
  extern    chain_list * subchain __P((chain_list *chainref, chain_list *chaindel));
  extern    chain_list * delchaindata __P((chain_list *pthead, void *ptdeldata));
  extern    chain_list * dupchainlst __P((chain_list *chain_ptr));
  extern    ptype_list * dupptypelst __P((ptype_list *ptype_ptr));
  extern           void  freechain __P((chain_list *pt));
  extern           void testchain();
  extern    ptype_list * addptype __P((ptype_list *pthead, long type, void *ptdata));
  extern    ptype_list * getptype __P((ptype_list *pthead, long type));
  extern    ptype_list * delptype __P((ptype_list *pthead, long type));
  extern    ptype_list * testanddelptype __P((ptype_list *pthead, long type));
  extern           void  freeptype __P((ptype_list *pt));
  extern    chain_list * append __P((chain_list *pt1, chain_list *pt2));
  extern          char * namealloc __P((char *inputname));
  extern          char * sensitive_namealloc(char *name);
  extern          char * min_namealloc __P((char *inputname));
  extern          char * namefind __P((char *inputname));
  extern          char * min_namefind __P((char *inputname));
  extern          char * mbkstrdup __P((char *s));
int mbk_casestrcmp(char *orig, char *dest);
  extern           void  downstr __P((char *s, char *t));
  extern           void  upstr __P((char *s, char *t));
  extern          char * beginstr __P((char *s, char *find, char separ));
  extern          char * endstr __P((char *s, char *find));
  extern          int    wildstrstr __P((char *s, char *find));
  extern          char * vectorize __P((char *radical, long index));
  extern          char * vectorradical __P((char *name));
  extern            int  vectorindex __P((char *name));
  char *mbk_VectorRadical(char *name, char *openb, char *closeb);
  int mbk_VectorIndex(char *name, char *openb, char *closeb);
  extern          char * concatname __P((char *name1, char *name2));
  extern          void leftunconcatname __P((char *name, char **left, char **right));
  extern          void rightunconcatname __P((char *name, char **left, char **right));
  extern    chain_list * reverse __P((chain_list *head));
  extern            int  naturalstrcmp __P((char *s, char *t));
  extern            ht * addht __P((unsigned long len));
  extern       ht* controlled_dupht __P(( ht *orig, long (*func)( long value, void *user_data ), void *user_data ));
  extern            ht * dupht(ht *orig);
  extern           long  gethtitem __P((ht *pTable, void *key));
  extern           long  addhtitem __P((ht *pTable, void *key, long value));
long controlled_addhtitem(ht *pTable, void *key, long (*func)(int newone, long old_value, void *user_data), void *user_data);
  extern           long  sethtitem __P((ht *pTable, void *key, long value));
  extern           long  delhtitem __P((ht *pTable, void *key));
  extern           void  scanhtkey __P((ht *pTable, int first, void **nextkey, long *nextitem ));
  extern           void  viewht __P((ht *pTable, char *(*pout)()));
  extern           void  delht __P((ht *pTable));
void resetht(ht *pTable);
  extern            it * addit __P((unsigned int length));
  extern           long  getititem __P((it *table, int item));
  extern           int   addititem __P((it *table, long data));
  extern           void  setititem __P((it *table, int item, long data));
  extern           void  delititem __P((it *table, int item));
  extern           void  delit __P((it *table));
  extern            voidt * addvt __P((unsigned int length));
  extern           void *getvtitem __P((voidt *table, int item));
  extern           int   addvtitem __P((voidt *table, void *data));
  extern           void  setvtitem __P((voidt *table, int item, void *data));
  extern           void  delvtitem __P((voidt *table, int item));
  extern           void  delvt __P((voidt *table));
  extern            dt * adddt __P((unsigned int length));
  extern          double getdtitem __P((dt *table, int item));
  extern           int   adddtitem __P((dt *table, double data));
  extern           void  setdtitem __P((dt *table, int item, double data));
  extern           void  deldtitem __P((dt *table, int item));
  extern           void  deldt __P((dt *table));
  extern            int  incatalog __P((char *figname));
  extern            int  inlibcatalog __P((char *figname));
  extern            void addcatalog __P((char *figname));
  extern            int  incataloggds __P((char *figname));
  extern            int  incatalogfeed __P((char *figname));
  extern            int  incatalogdelete __P((char *figname));
  chain_list *getcataloglist();
  void setcataloglist(chain_list *cl);
  extern           void  alliancebanner __P((char *tool, char *tv, char *comment, char *date, char *av));
  extern           void  alliancebanner_with_authors __P((char *tool, char *tv, char *comment, char *date, char *av, char *authors));
  extern           void  alliancebanner_with_contrib __P((char *tool, char *tv, char *comment, char *date, char *av, char *authors, char *contrib));
  extern           int   mbkwaitpid __P(( int, int, int* ));
  extern           void mbkinitendchld __P((void));
  extern           endchld_list* mbkgetendchld __P((void));
  extern           void mbkfreeendchld __P((endchld_list*));
  extern           void mbksetautoackchld __P(( int ));
  extern           void mbkinitautoackchld __P(( void ));
  extern           void mbkfreeautoackchld __P(( autoackchld_list* ));
  extern           autoackchld_list* mbkgetautoackchld __P(( void ));
  extern           int    isknowntrsparam __P(( void *));
  extern           char** getknowntrsparam __P(( char *));
  extern           char* mbk_getvssname __P((void));
  extern           char* mbk_getvddname __P((void));

/*******************************************************************************
* externals for mbk_sys.c functions                                            *
*******************************************************************************/
  extern          void * mbkalloc __P((size_t nbytes));
  extern          void * mbkrealloc __P((void *pt, unsigned int nbytes));
  extern           void  mbkfree __P((void *ptr));
  extern          FILE * mbkfopen __P((char *name, char *extension, char *mode));
  extern          FILE * mbkfopen_ext __P((char *name, char *extension, char *mode, char access, char allowcompress ));
  extern            int  mbkunlink __P((char *name, char *extension));
  extern          char * filepath __P((char *name, char *extension));
  extern          char** decompfilter( char* );
  extern          FILE*  mbkpopen __P(( char*, char*, char ));
  extern           void  mbkps();
  extern           void  mbkackchld __P(( int ));
  extern   unsigned long mbkprocessmemoryusage();
  
/*******************************************************************************
* externals for mbk_crypt.c functions                                          *
*******************************************************************************/
typedef struct {
    int pipefd;
    FILE *fp;
    int *ptlineno;
} decrypt_files;

void *mbk_ascii_decrypt_thread(void *ptfiles);
int mbk_ascii_encrypt(char *inname, char *outname);

/*******************************************************************************
* externals for mbk_fake.c  or parsers drivers functions                       *
*******************************************************************************/
extern void vtiloadphfig();
extern void vtiloadlofig();
extern void alcloadphfig();
extern void alcloadlofig();
extern void spiceloadlofig();
extern void vhdlloadlofig();
extern void verilogloadlofig();
extern void hiloloadlofig();
extern void edifloadlofig();
extern void vtisavephfig();
extern void vtisavelofig();
extern void alcsavephfig();
extern void alcsavelofig();
extern void spicesavelofig();
extern void vhdlsavelofig(struct lofig *, FILE *);
extern void hilosavelofig();
extern void edifsavelofig();
extern void vlogsavelofig(struct lofig *, FILE *);
extern void mgnloadphfig();
extern void mgnsavephfig();

void parseverilognetlist(char *libname);
void parsevhdlnetlist(char *libname);

/* par Fabrice le 7/2/2002 */
extern void spf_Annotate();
/* ------- */ 
/* par Olivier le 27/6/2002 */
extern void spef_Annotate();
/* ------- */ 


typedef struct 
{
  void *HEAD;
  int size, granularity;
  chain_list *blocks_to_free;
#ifdef ENABLE_STATS
  long __SIZE__;
#endif
} HeapAlloc;

void CreateHeap(int size, int granularity, HeapAlloc *myheap);
void DeleteHeap(HeapAlloc *myheap);
void *AddHeapItem(HeapAlloc *myheap);
void DelHeapItem(HeapAlloc *myheap, void *item);
size_t getsizeofHeapAlloc( HeapAlloc *heap );

/*************** CACHE ****************/

typedef struct mbkcachelist {
  struct mbkcachelist  *NEXT;
  struct mbkcachelist  *PREV;
  void                 *DATA;
  int                   LOCKED;
} mbkcachelist;

typedef struct {
  mbkcachelist         *FIRST;
  mbkcachelist         *LAST;
  ht                   *INFOS;
  char                (*FN_ISACTIVE)( void *root, void *elem );
  unsigned long int   (*FN_LOAD    )( void *root, void *elem );
  unsigned long int   (*FN_RELEASE )( void *root, void *elem );
  unsigned long int     CACHESIZE;
  unsigned long int     CURSIZE;
  unsigned int          MAXELEM;
  unsigned int          CURELEM;
  HeapAlloc             HEAPCACHELIST;
} mbkcache;

/* Définition du cache */
extern mbkcache* mbk_cache_create( char (*isactive)( void *root, void *elem ), unsigned long int (*load)( void *root, void *elem ), unsigned long int (*release)( void *root, void *elem ), unsigned long int cachesize);
extern void mbk_cache_delete( mbkcache *cache, void *root );

/* Opération sur un élément du cache */
extern void mbk_cache_refresh( mbkcache *cache, void *root, void *elem );
extern void mbk_cache_release( mbkcache *cache, void *root, void *elem );

/* Mécanisme de vérouillage d'un élément du cache. L'élément doit avoir été 
chargé en mémoire avant d'être vérrouillé. On peut faire autant d'appels à 
mbk_cache_lock() pour un seul élément, et pour le dévérouiller il faudra faire
autant de mbk_cache_unlock(). */
extern void mbk_cache_lock( mbkcache *cache, void *elem );
extern void mbk_cache_unlock( mbkcache *cache, void *elem );
extern char mbk_cache_islock( mbkcache *cache, void *elem );

/* Change directement la taille du cache. Libération automatique des éléments si
la capacité maximale est atteinte ou dépassée */
void mbk_cache_update_size( mbkcache *cache, void *root, long int size );
chain_list* mbk_cache_list_content( mbkcache *cache );

/* Limite le nombre d'éléments présents dans le cache à un instant donné */
void mbk_cache_set_limit_element( mbkcache *cache, void *root, unsigned int nbelem );

/* Mécanisme de cache des fichiers ouverts */
extern int   MBK_MAX_CACHE ;
extern void  mbk_cache_clear_file( int id );
extern FILE* mbk_cache_get_file( int id );
extern int   mbk_cache_set_file( FILE *fd, char *filename, char *extension );

/*****************************************************************************\
POINTEURS SUR LES FONCTIONS UTILISATEUR.
FN_ISACTIVE :           Doit renvoyer 1 si le cache est actif pour l'élément, 
                        ou 0 sinon.
FN_GETINFOS :           L'utilisateur doit mémoriser pour chaque élément 
FN_SETINFOS             present dans le cache un pointeur différent de NULL.
FN_LOAD :               Chargement d'un élément. Renvoie le pointeur sur l'élément.
FN_RELEASE :            Libération d'un élément.
FN_GETSIZE:             Doit renvoyer la taille de l'élément présent en 
                        mémoire.
\*****************************************************************************/

// indiquent si un nom de fichier correspond a une lofig
// prend en compte le FILTER_SFX
// retourne un pointeur sur le '.' entre le radical et le suffixe
char *mbkFileIsLofig(char *filename);
char *mbkIsLofigExt(char *filename, char *ext);

/*********** File access ************/
#define MBK_FILE_STD  ((char)32)
#define MBK_FILE_LFS  ((char)64)

#define MBK_OFFSET_STD long int

#if defined(_LARGEFILE64_SOURCE ) && !defined(_LP64)
  #define MBK_OFFSET_LFS  off64_t
  #define MBK_OFFSET_MAX  off64_t
#else
  #define MBK_OFFSET_LFS  long int
  #define MBK_OFFSET_MAX  long int
#endif

#define MBK_FILESIZE_TYPE long long unsigned
#define MBK_LIMIT_USELARGEFILE ((MBK_FILESIZE_TYPE)2147483647) /* 2Gb-1 */
//#define MBK_LIMIT_USELARGEFILE ((MBK_FILESIZE_TYPE)31457280) /* 30Mb */

void mbk_setfileacces( FILE *file, char acces );
extern char mbk_getfileacces( FILE *file );
long long unsigned mbk_getfilesize( char *name );
int mbkftell( FILE *file, MBK_OFFSET_MAX *offset );
int mbkfseek( FILE *file, MBK_OFFSET_MAX *offset, int whence );
FILE *mbksysfopen( char *name, char *mode, char );

typedef struct st_mbk_tableint
{
  int                     index;
  int                     data;
  struct st_mbk_tableint     *down;  
} mbk_tableint;


chain_list *GetAllHTElems(ht *pTable);
chain_list *GetAllHTKeys(ht *pTable);
chain_list *GetAllHTElems_sub(ht *pTable, int sort);

typedef struct _advanced_block_allocator_elem
{
  struct _advanced_block_allocator_elem *next;
  char *tab;
  int size;
  int free;
#ifdef ENABLE_STATS
  long __SIZE__;
#endif
} _advanced_block_allocator_elem;


typedef struct
{
  _advanced_block_allocator_elem *head, *search;
  int minsize;
  int align;
#ifdef ENABLE_STATS
  long __SIZE__;
#endif
} AdvancedBlockAllocator;


typedef struct
{
  char **blocs;
  int maxblocs;
  int nbblockelemmask;
  int elemsize;
  short nbblockelempow;
  char init;
#ifdef ENABLE_STATS
  long __SIZE__;
#endif
} AdvancedTableAllocator;

typedef struct
{
  chain_list **NAME_HASHTABLE;
  int HASHVAL0;
  char SENSITIVE;
  int namealloc_primes_index;
  AdvancedBlockAllocator *aba;
#ifdef ENABLE_STATS
  long __SIZE__, __NBNAMES__;
#endif
} NameAllocator;

typedef struct _advanced_namealloc_piece
{
  struct _advanced_namealloc_piece *brother, *next, *prev;
  int index;
  char *piece;
} _advanced_namealloc_piece;


typedef struct
{
  _advanced_namealloc_piece *start;

} _advanced_namealloc;

typedef struct
{
  int sensitive;
  int curindex;
  chain_list *allhtables;
  NameAllocator na;
  AdvancedBlockAllocator *aba;
  AdvancedTableAllocator *ata;
  _advanced_namealloc tab[256];
#ifdef UNUN
  mbk_tableint *numbers;
#else
  ht *numbers;
#endif
} AdvancedNameAllocator;

void CreateNameAllocator(int size, NameAllocator *na, char _case);
void DeleteNameAllocator(NameAllocator *na);
char *NameAlloc(NameAllocator *na, char *inputname);
void NameAllocStat(NameAllocator *na);
char *NameAllocFind(NameAllocator *na, char *inputname);

AdvancedBlockAllocator *CreateAdvancedBlockAllocator(int minsize, int align);
void *AdvancedBlockAlloc(AdvancedBlockAllocator *aba, int size);
void FreeAdvancedBlockAllocator(AdvancedBlockAllocator *aba);

AdvancedNameAllocator *CreateAdvancedNameAllocator(int _case);
int AdvancedNameAlloc(AdvancedNameAllocator *ana, char *name);
int AdvancedNameAllocated(AdvancedNameAllocator *ana, char *name);
int AdvancedNameAllocEx(AdvancedNameAllocator *ana, char *name, int *exists);
int AdvancedNameAllocName(AdvancedNameAllocator *ana, int index, char *buf);
void FreeAdvancedNameAllocator(AdvancedNameAllocator *ana);
void AdvancedNameAllocStat(AdvancedNameAllocator *ana);

AdvancedTableAllocator *CreateAdvancedTableAllocator(int basesize, int elemsize);
void *GetAdvancedTableElem(AdvancedTableAllocator *ata, int index);
void FreeAdvancedTableAllocator(AdvancedTableAllocator *ata);

/******************************************************************************\
includes for mbk_math
\******************************************************************************/

// La définition des pentes y = A.x + B

typedef struct {
  double        X0 ;    // Absisse de début (jusqu'à la pente suivante).
  double        A ;     // Pente.
  double        B ;     // Valeur à l'origine.
} mbk_pwl_data ;

typedef struct {
  mbk_pwl_data *DATA;     // Tableau des pente.
  int           N;        // Nombre de pentes.
  double        X1;       // Abscisse finale.
} mbk_pwl ;

// Modèles de construction

#define MBK_PWL_DEFAULT ((char)'d')
#define MBK_PWL_SURFACE ((char)'f')

typedef struct {
  double        DELTAYMAX;      // Valeur max entre la fn et l'approximation.
  double        DELTAXMIN;      // Ecart minimum entre 2 absisses de pentes.
  double        SDERIVE;        // Ecart pour calculer la dérivée de la fn.
} mbk_pwl_param_default ;

typedef struct {
  double        EPSILON;        // part relative maximum de la surface initiale.
  double        DELTAXMIN;      // Ecart minimum entre 2 absisses de pentes.
  double        SDERIVE;        // Ecart pour calculer la dérivée de la fn.
} mbk_pwl_param_surface ;

typedef union {
  mbk_pwl_param_default   DEFAULT ;
  mbk_pwl_param_surface   SURFACE ;
} mbk_pwl_model_param ;

typedef struct mbk_pwl_param_point {
  struct mbk_pwl_param_point *NEXT;
  double                      X;
} mbk_pwl_param_point_list ;


typedef struct {
  char                       MODEL ;
  mbk_pwl_model_param        PARAM;
  mbk_pwl_param_point_list  *POINTS;
} mbk_pwl_param ;

// Configuration de la transformée
mbk_pwl_param* mbk_pwl_get_surface_param( char (*fn)(void*, double, double*),
                                          void *data,
                                          double x0,
                                          double x1
                                        );
mbk_pwl_param* mbk_pwl_get_default_param( char (*fn)(void*, double, double*),
                                          void *data,
                                          double x0,
                                          double x1
                                        );
void mbk_pwl_add_param_point( mbk_pwl_param *param, double x );
void mbk_pwl_free_param( mbk_pwl_param *param );

// Création de la transformée
mbk_pwl* mbk_pwl_create( char (*fn)(void*, double, double*),
                         void *data,
                         double x0,
                         double x1,
                         mbk_pwl_param *param
                       );
void mbk_pwl_free_pwl( mbk_pwl *pt );
mbk_pwl* mbk_pwl_alloc_pwl( int n );
mbk_pwl* mbk_create_pwl_according_tanh( char (*fn)(void*, float, float*), 
                                        void *data,
                                        float v1,
                                        float v2
                                      );
// Utilisation de la transformée
mbk_pwl* mbk_pwl_duplicate( mbk_pwl *pwl );

int mbk_qsort_dbl_cmp( double *x1, double *x2 );

// Code de retour de mbk_pwl_get_value() et mbk_pwl_get_value_bytabindex().
#define MBK_PWL_ERROR ((int)0)  // Impossible de déterminer de résultat.
#define MBK_PWL_FOUND ((int)1)  // Un résultat a été trouvé.
#define MBK_PWL_EXTPL ((int)2)  // Un résultat a été trouvé, mais il a fallu
                                // extrapoler le 1° ou le dernier interval.

int mbk_pwl_get_value( mbk_pwl *pwl, double x, double *y );
int mbk_pwl_get_value_bytabindex ( mbk_pwl *pwl, double x, double *y, int *tabindex );
char mbk_pwl_get_inv_value( mbk_pwl *pwl, double y, double *x );
void mbk_pwl_gnuplot( mbk_pwl *lines, double tstart, double tstop, char *fname );


// Structure plus pratique à utiliser pour calculer des transformées
// de Laplace

typedef struct {
  double T;
  double A;
} mbk_laplace_data ;

typedef struct {
  int               N;
  int               SIZE;
  double            B;
  void             *DATA;
} mbk_laplace ;

mbk_laplace* mbk_pwl_to_laplace( mbk_pwl *lines, int size );
void mbk_free_laplace( mbk_laplace *pt );

int mbk_dichotomie( int(*fn)(void*, double, double*), int (*fnd)(void*, double, double*), void *data, double *x0, double *x1, char mode, int *itmax, double epsilonx, double epsilony, double *x );

// Paramètre 'mode' de la fonction mbk_dichotomie().
#define MBK_DICHO_EQUAL ((char)'e')
#define MBK_DICHO_PROP  ((char)'p')
#define MBK_DICHO_DERIV ((char)'d')
#define MBK_DICHO_NR    ((char)'n')

// Codes de retour de la fonction mbk_dichotomie().
#define MBK_DICHO_OK    ((int)1)
#define MBK_DICHO_ERRFN ((int)2)
#define MBK_DICHO_MAXIT ((int)3)
#define MBK_DICHO_ERR   ((int)4)

// Convertion de tableaux de valeur
void mbk_make_tab_equi( double *tabinx, double *tabiny, int nbin, double *tabout, int nbout );

// Fonction d'approximation de nombre

int mbk_float_approx_interval( float x, float precis ) ;
void mbk_float_approx_limits( int n, float precis, float *ll, float *lh ) ;
float mbk_float_approx_middle( float x, float precis ) ;
long mbk_long_round(double val);
int mbk_cmpdouble(double a, double b, double precision);
double mbk_rounddouble(double a, double precision);
double mbk_compute_curve_error( int na, double *xa, double *ya, int nb, double *xb, double *yb );

typedef struct htitem_v2
{                        /* hash table element          */
  struct htitem_v2 *next;
  void *key;                                  /* pointer for access          */
  long value;                                 /* associated value            */
} htitem_v2;

typedef struct
{                        /* hash table itself           */
  htitem_v2 **ITEM_HASHTABLE;
  int primes_index, size, rehashlimit;
  HeapAlloc ha;
} ht_v2;

ht_v2 *addht_v2(long len);
void delht_v2(ht_v2 *h);
long gethtitem_v2(ht_v2 *h, void *key);
long addhtitem_v2(ht_v2 *h, void *key, long value);
long sethtitem_v2(ht_v2 *h, void *key, long value);
void scanhtkey_v2( ht_v2 *h, int first, void **nextkey, long *nextitem );
ht_v2 *dupht_v2( ht_v2 *orig );
long controlled_addhtitem_v2(ht_v2 *h, void *key, long (*func)(int newone, long old_value, void *user_data), void *user_data );
long delhtitem_v2(ht_v2 *h, void *key);
chain_list* GetAllHTKeys_v2( ht_v2 *h );
chain_list *GetAllHTElems_sub_v2( ht_v2 *pTable, int sort);
ht_v2* controlled_dupht_v2( ht_v2 *orig, long (*func)( long value, void *user_data ), void *user_data );
void resetht_v2(ht_v2 *h);

void set_ht_v2_rehashlimit(ht_v2 *h, int value);
void ht_v2Stat(ht_v2 *h);
size_t getsizeofht_v2( ht_v2 *ht );

void mbk_debugstat(char *label, int reset);

/****************************************************************************/
/*{{{                    mbk_fileextract functions                          */
/*                                                                          */
/* mfe_fileextractlabel is a function which get the value associated to     */
/* a variable in a file by matching a rule                                  */
/*                                                                          */
/* a rule can be defined as follow :                                        */
/* "type:/beginmark/endmark/:tokenmatching"                                 */
/* type can be either line or table                                         */
/* tokenmatching contains specials token :                                  */
/* %l for matching a variable name                                          */
/* %v for matching a variable value                                         */
/* %0 for matching a value to be ignored                                    */
/* %i for matching a name to be ignored                                     */
/* exemple of rule for extract data on a .chi file :                        */
/* "line:/EXTRACT INFORMATION/1**** /:* %l = %v targ= %0 trig= %0           */
/*                                                                          */
/****************************************************************************/
#ifndef MBK_FILEEXTRACT_H

int  mfe_addrule(void **ruleslist, char *rule);
int  mfe_delrule(void **ruleslist, void *rule);
void mfe_freerules(void **ruleslist);
void mfe_duprules(void **ruleslist, void *rule);
/* be carefull label must be nameallocated */
int  mfe_addlabel(void **table, char *label, int nbmc);
int  mfe_dellabel(void **table, char *label);
void mfe_freelabels(void **table, int cleanmc);
void mfe_duplabels(void **table, void *ref);

int  mfe_fileextractlabel(char *filename, void *rules, void *ref, int nbmc);

int  mfe_labelvalue(void *ref, char *label, double *value, int nbmc);
void mfe_cleanLabel(void *ref);

int mfe_readmeasure(char *filename, void *mferules, char *rule, char *label, double *value);
void mfe_read_montecarlo_ltspice(char *filename, void *ref);

#endif

/*}}}************************************************************************/
/*{{{                    mbk_vector functions                               */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#ifndef MBK_VECTOR_H

void     mbv_addAtIndex (void *table, char *corresp, int index);

void    *mbv_initName   (ht *tbl, char *key, int left, int right);
int      mbv_addCorresp (ht *tbl, char *key, char *corresp);
void    *mbv_get        (ht *tbl, char *key);
void     mbv_free       (ht *tbl, char *key);
void mbv_freeht(ht *source);
char    *mbv_getCorresp (ht *tbl, char *key, char *buf);
char    *mbv_getCorrespAndDel(ht *tbl, char *key, char *buf);
void     mbv_prefix     (ht *tbl, char *prefix);
void     mbv_changeRef  (ht *orig, ht *dest, chain_list *orc, chain_list *dec);
void     mbv_useAdvancedNameAllocator(void);
void     mbv_dumpCorresp(ht *table);
void     mbv_setNoCorrepondance(char *noCorrespString);
ht *mbv_duplicateht(ht *source);
ht *mbv_buildglobalht(chain_list *htl);

#endif
/*}}}************************************************************************/

  extern int mbkpprint __P((int fildes, void *data, int size));
  extern int mbkpscan __P((int fildes, void *data, int size));
  extern pid_t mbkpcreate __P((int *file_des_write, int *file_des_read));

#ifdef MALLOC_HISTOGRAM
void malloc_hook_stats(char *filename, int mode);
void reset_malloc_hook();
void *my_malloc_hook(char *file, int line, size_t size);
void my_free_hook(void *block);
void *my_realloc_hook(char *file, int line, void *block, size_t size);
#define mbkalloc(x) my_malloc_hook(__FILE__, __LINE__, x)
#define mbkfree(x) my_free_hook(x)
#define mbkrealloc(x,y) my_realloc_hook(__FILE__, __LINE__, x, y)
/*#define malloc(x) my_malloc_hook(x)
#define free(x) my_free_hook(x)
#define realloc(x,y) my_realloc_hook(x,y)*/
#endif

/* devectorise dans un buffer
   mbk_devect_sub("toto<5>", "<[", ">]", buffer)
   => vecteur supporte:  toto<?> et toto[?]
 */
char *mbk_devect_sub(char *name, char *leftb, char *rightb, char *sepb, char *buf, int testchar(int));
/* devectorise et renvoie un nom namealloc'er */
char *mbk_devect(char *name, char *leftb, char *rightb);
char *mbk_decodevector(char *name);
char *mbk_decodeanyvector(char *name);

/* vectorise dans un buffer */
char *mbk_vect_sub(char *name, char leftb, char rightb, char *buf);
/* vectorise et renvoie un nom namealloc'er */
char *mbk_vect(char *name, char leftb, char rightb);

/* recupere un numero dans une chaine en se basant sur une regle
   eg.  "titi145rf", "titi?rf" => index=145
*/ 
int mbk_FindNumber(char *source, char *rule, int *index);

// fill the name with the process name pointed by pd
extern char *mbk_whoAmI(pid_t pd, char *name);
extern char *mbk_whoAmILong(pid_t pd, char *name);


typedef struct
{
  char *name;
  int mask;
} mbk_options_pack_struct;

int mbk_ReadFlags(int varnum, mbk_options_pack_struct *gen_opack, int nbopt, int warn, int initval);

typedef struct
{
  struct rusage start_t;
  time_t start_rt;
  struct rusage end_t;
  time_t end_rt;
} mbk_chrono;

char *mbk_GetUserChrono(mbk_chrono *mc, char *temp);
void mbk_StartChrono(mbk_chrono *mc);
void mbk_StopChrono(mbk_chrono *mc);
char *mbk_GetRealChrono(mbk_chrono *mc, char *temp);


typedef struct mbk_matrix_elem
{
  struct mbk_matrix_elem *UP, *DOWN, *LEFT, *RIGHT;
  int x, y;
  double value;
} mbk_matrix_elem;

typedef struct
{
  int NZ;
  mbk_matrix_elem *first_elem, *last_elem, *tracker;
} elem_index_tab;

typedef struct mbk_matrix
{
  int nbx, nby;
  elem_index_tab *line, *col;
  mbk_matrix_elem *tracker0, *tracker1;
  HeapAlloc mbk_matrix_elem_heap;
  int *resolve_line_order, *resolve_unk_order;
} mbk_matrix;

typedef int (*matrix_traverse_func)(int x, int y, double *value, void *data);

void mbk_FreeMatrix(mbk_matrix *mm);
mbk_matrix *mbk_CreateMatrix(int nbx, int nby);
void mbk_DisplayMatrix(char *filename,mbk_matrix *mm);
double mbk_GetMatrixValue(mbk_matrix *mm, int x, int y);
void mbk_SetMatrixValue(mbk_matrix *mm, int x, int y, double val);

int mbk_CreateLUMatrix(mbk_matrix *u, mbk_matrix *l, mbk_matrix *sol);
mbk_matrix *mbk_MatrixMultiplyMatrix(mbk_matrix *a, mbk_matrix *b);
void mbkMatrixSwapCols(mbk_matrix *u, int x0, int x1);
void mbkMatrixSwapLines(mbk_matrix *u, int y0, int y1);
mbk_matrix *mbk_MatrixSubstractMatrix(mbk_matrix *a, mbk_matrix *b);
mbk_matrix *mbk_MatrixDuplicateMatrix(mbk_matrix *a);
double *mbk_MatrixSolveUsingArray(mbk_matrix *a, double *solval, double *unkval);
mbk_matrix *mbk_MatrixSolve(mbk_matrix *a, mbk_matrix *sol);
int mbk_MatrixReduce(mbk_matrix *a, int limit_index);

void mbk_MatrixAddHalfLinkedElem(mbk_matrix *mm, int x, int y, double value);
void mbk_MatrixFinishElemLink(mbk_matrix *mm);

int mbk_MatrixTraverse(mbk_matrix *a, matrix_traverse_func mtf, void *data);

chain_list *BuildFileList(char *name, chain_list *regx, int once);

#ifdef Solaris
extern char *strtok_r(char *s1, const char *s2, char **lasts);
#endif

typedef struct
{
  chain_list *reg_list;
  chain_list *name_list;
  ht *QUICKFIND;
  int casef;
  char cbna;
} mbk_match_rules;

void mbk_CreateREGEX(mbk_match_rules *mr, char casef, int canbenonnameallocated);
void mbk_FreeREGEX(mbk_match_rules *mr);
void mbk_AddREGEX(mbk_match_rules *mr, char *name);
int mbk_CheckREGEX(mbk_match_rules *mr, char *name);
int mbk_TestREGEX(char *testname, char *refname);
char *mbk_REGEXJokerName(char *name, char *oldname, char *newname);
char mbk_GetREGEXMODE();
int mbk_isregex_name(char *name);
char *mbk_get_reverse_index_regex(char *refname);
char *mbk_index_regex(char *r);
        
void mbk_dumpfile(FILE *infile, FILE *outfile, int level);

mbk_tableint*	creatmbk_tableint ();
int		setmbk_tableint ( mbk_tableint *table, int value, int data );
void            removembk_tableint ( mbk_tableint*, int );
int		tstmbk_tableint ( mbk_tableint *table, int value );
void		freembk_tableint ( mbk_tableint *table );
int             mbk_scanint ( mbk_tableint *table, int n );

// -- tableaux
//
#define MAXCOL 64

typedef struct BoardColumn
{
  char *value;
} BoardColumn;

typedef struct
{
  int size;
  char align;
  char sep;
} boardproperties;

typedef struct Board
{
  boardproperties prop[MAXCOL];
  chain_list *list;
} Board;

Board *Board_CreateBoard();
void Board_SetSize(Board *B, int col, int size, char align);
BoardColumn *Board_NewLine(Board *B);
void Board_SetValue(Board *B, int col, char *val);
void Board_Display(FILE *f, Board *B, char *LP);
void Board_FreeBoard(Board *B);
void Board_DisplaySeparation(FILE *f, Board *B, char *LP);
void Board_NewSeparation(Board *B);
void Board_SetSep(Board *B, int col);
void Board_Display_sub(FILE *f, int lib, int lev, Board *B, char *LP);

typedef struct {
  char *filename;
  unsigned long int NB_RESI ;
  unsigned long int NB_CAPA ;
  unsigned long int NB_LOTRS ;
  unsigned long int NB_NET ;
  unsigned long int NB_INSTANCE ;
  unsigned long int NB_DIODE ;
} mbk_parse_error ;

extern mbk_parse_error MBK_PARSE_ERROR ;
extern chain_list *MBK_ALL_PARSE_ERROR;

void mbk_commit_errors(char *filename);
void mbk_reset_errors();
int namealloc_ok();

#ifdef DELAY_DEBUG_STAT
void mbk_DisplayProgress(long *TM, char *mes, int cur, int tot, char mode);
#else
#define mbk_DisplayProgress(a,b,c,d,e)
#endif

typedef struct
{
  int nb;
  pthread_cond_t cond;
  pthread_mutex_t mutex;
} mbk_sem_t;

void mbk_sem_init(mbk_sem_t *sm0, int value);
void mbk_sem_destroy(mbk_sem_t *sm);
void mbk_sem_wait(mbk_sem_t *sm);
void mbk_sem_post(mbk_sem_t *sm);

char *mbk_montecarlo_key(char *model, char *subcktname, int type, char location);

/*****************************************************************************/
/* the socket to the master process. set to -1 if not connected */
extern int MBK_SOCK ;

#define MBK_COM_NONE  ' ' 
#define MBK_COM_BEGIN 'B'
#define MBK_COM_DATA  'D'
#define MBK_COM_END   'E'
#define MBK_COM_ABORT 'A'

#define COM_HEADER_LENGTH 3

#define MBK_COM_NOEVENT -99

typedef struct {
  int           fd ;
  int           state ;
  int           part ;
  int           current ;
  int           total ;
  int           id ;
  unsigned int  key ;
  unsigned int  crc ;
  int           event ;
  unsigned int  header[ COM_HEADER_LENGTH  ] ;  /* type - length - crc */
  char         *buffer ;
  int           type ;
  int           size;
} comslot ;

typedef struct {
  int        n ;
  comslot   *tabslot ;
  int        sock ;
  int        port ;
} mbk_comdata ;

int  mbk_activatecomslave( char *connexion ) ;
void mbk_sendmsg( unsigned int type, char *data, int length ) ;

mbk_comdata* mbk_activatecommaster( int n ) ;
int          mbk_comwaiteventfromnet( mbk_comdata* ) ;
int          mbk_commgetdata( mbk_comdata*, int, char**, int *size ) ;
void         mbk_terminatecommaster( mbk_comdata* ) ;
int          mbk_comslavegivetoken( char*, char* );
void         mbk_comcheck( unsigned int keylofig, unsigned int keycnsfig, unsigned int keyttvfig );
void mbk_commputdata( mbk_comdata *comdata, int id, char *data, int l ); 
void mbk_comrecvslavemessage( void *buffer, int l );
char *mbk_commtalk( char *msg, int size );
int mbk_comm_send_text_file(char *name);

#ifdef USE_DEFAULT_RANDR
#define MBK_RAND_MAX RAND_MAX
#else
#define MBK_RAND_MAX (unsigned int)0xffffffff
#endif

unsigned int mbk_get_a_seed();
unsigned int mbk_rand_r(unsigned int *mt);
unsigned int mbk_get_montecarlo_global_seed_init_saved();

#ifdef Linux
  #define uint32_t u_int32_t
  #define uint8_t  u_int8_t
#endif

typedef struct
{
  chain_list *head, *end;
  int nb;
} quicklist;

int mbk_sumchar(char *name);
void mbk_substenvvariable(char *ref, char *res);
int mbk_charcmp(char orig, char dest);

#endif /* !MUTL */
