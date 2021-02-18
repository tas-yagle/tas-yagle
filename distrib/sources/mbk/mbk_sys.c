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
 * Purpose : system dependant functions
 * Date    : 06/03/92
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 */

#ident "$Id: mbk_sys.c,v 1.69 2008/09/22 14:34:15 anthony Exp $"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

// zinaps: to compute the memory consumed
/*
#ifdef Linux
#include <asm/page.h>
#endif
*/

#ifdef Solaris
#define _STRUCTURED_PROC 1
#include <sys/procfs.h>
#endif

#include AVT_H
#include MUT_H
#include "mbk_sys.h"

#ifdef MALLOC_HISTOGRAM
#undef mbkalloc
#undef mbkfree
#undef mbkrealloc
#undef malloc
#undef free
#undef realloc
#endif

#ifdef Darwin
#include <sys/sysctl.h>
#include <mach/task.h>
#include <mach/mach_init.h>
#include <mach/shared_memory_server.h>
#endif


#define VFORKMAXTRY 100

static char filename[BUFSIZ];
char MBKFOPEN_NAME[BUFSIZ];
char MBKFOPEN_FILTER;
autoackchld_list *HEAD_AUTOACKCHLD=NULL;
autoackchld_list *HEAD_AUTOACKCHLD_FREE=NULL;
int STAT_AUTOACKCHLD;

it* MBK_TAB_FILE_ACCES=NULL;

long mbkalloc_stat = 0; /* statistics on maximun allocated memory */

void *mbkalloc(nbytes)
size_t nbytes;
{
void 	*pt;

	mbkalloc_stat += nbytes;
	if (!(pt = malloc(nbytes))) {
        avt_errmsg (MBK_ERRMSG, "045", AVT_FATAL,(unsigned long)nbytes,mbkprocessmemoryusage()/(unsigned long)1024);
  	}
	return pt;
}

void *mbkrealloc(pt, nbytes)
void 	*pt;
unsigned int nbytes;
{

	mbkalloc_stat += nbytes;
	if (!(pt = realloc(pt, nbytes))) {
        avt_errmsg (MBK_ERRMSG, "046", AVT_FATAL);
  	}
	return pt;
}

void mbkfree(ptr)
void *ptr;
{
#ifdef lint
	(void)free((char *)ptr);
#else
	(void)free(ptr); /* lint makes a mistake here : expects (char *) */
#endif
}

/* file opening :
   defines the strategy used for searching and opening file in the
   mbk environement. */
FILE* mbkfopen( name, extension, mode )
char *name;
char *extension;
char *mode;
{
  return mbkfopen_ext( name, extension, mode, 0, 1 );
}

FILE* mbkfopen_ext( name, extension, mode, access, allowcompress )
char *name;
char *extension;
char *mode;
char  access;
char  allowcompress;
{
  FILE *ptf;
  int   ln, le, i, j;

  // Check name contain extension
  if( extension ) {
    ln = strlen( name );
    le = strlen( extension );
    for( i=ln-1, j=le-1 ; i>=0 && j>=0 && name[i] == extension[j] ; i--, j-- );
    if( name[i] == '.' )
      extension = NULL;
  }
  
  ptf = mbkfopentrace( name, extension, mode, access, allowcompress );

  avt_log( LOGFILEACCESS, 1,
             "open( \"%s\", \"%s\", \"%s\" ) -> %s [%s]\n",
             name,
             extension ? extension : "",
             mode,
             ptf ? MBKFOPEN_NAME : "FAILED",
             mbk_getfileacces( ptf ) == MBK_FILE_STD ? "STD" : "LFS"
           );
  return( ptf );
}

void mbkfopen_infos( in, filename, name, extension, mode ) 
FILE *in;
char *filename;
char *name;
char *extension;
char *mode;
{
  avt_log( LOGFILEACCESS, 2,
           "%s : %s, (%s,%s,%s)\n",
           in ? "ok" : "FAILED",
           filename,
           name,
           extension ? extension : "",
           mode
         );
}

/*******************************************************************************

Fonctions d'accès aux fichiers en 32 ou en 64 bits dans un environnement 32
bits.

Informations :

En écriture, le mode 64 est toujours utilisé.

En lecture, si la taille du fichier est inférieure à MBK_LIMIT_USELARGEFILE, 
le mode 32 bits est utilisé, sinon, c'est le mode 64.
Pour savoir en quel mode a été ouvert un fichier, il faut utiliser 
mbk_getfileacces, qui renvoit MBK_FILE_STD ou MBK_FILE_LFS.
Lorsque le programme est compilé en 64 bits, le mode MBK_FILE_STD est toujours
utilisé puisque les fonctions de base accèdent en mode natif à des fichiers
en 64bits.
MBK_OFFSET_STD et MBK_OFFSET_LFS sont les types qui permettent de stocker les
offsets, selon le mode renvoyer par mbk_getfileacces(). L'objet MBK_OFFSET_STD
est stockable dans un long.

Fonctions de positionnement dans un fichier.

mbkftell() et mbkfseek() permettent d'appeller les fonctions fseek et ftell sans
se soucier du mode d'acces au fichier. Les offsets étant de taille variable, ils
sont passés par pointeur. Le type MBK_OFFSET_MAX est un type qui permet de
stocker n'importe quelle taille d'offset. 

*******************************************************************************/

void mbk_setfileacces( FILE *file, char acces )
{
  if( !file ) return;
  
  if( ! MBK_TAB_FILE_ACCES )
    MBK_TAB_FILE_ACCES = addit( 10 );

  setititem( MBK_TAB_FILE_ACCES, fileno( file ), (long)acces );
}

char mbk_getfileacces( FILE *file )
{
  if( !file || !MBK_TAB_FILE_ACCES ) return 0;
  return (char)getititem( MBK_TAB_FILE_ACCES, fileno( file ) );
}

// Renvoie 1 en cas de succès, 0 en cas d'echec.
int mbkftell( FILE *file, MBK_OFFSET_MAX *offset )
{
  int r=1;
  #if defined(_LARGEFILE64_SOURCE ) && !defined(_LP64)
  if( mbk_getfileacces( file ) == MBK_FILE_STD ) {
    *((MBK_OFFSET_STD*)offset) = ftell( file );
    if( *((MBK_OFFSET_STD*)offset) == (MBK_OFFSET_STD)-1 )
      r = 0;
  }
  else {
    *((MBK_OFFSET_LFS*)offset) = ftello64( file );
    if( *((MBK_OFFSET_LFS*)offset) == (MBK_OFFSET_LFS)-1 )
      r = 0;
  }
  #else
  *((MBK_OFFSET_STD*)offset) = ftell( file );
    if( *((MBK_OFFSET_STD*)offset) == (MBK_OFFSET_STD)-1 )
      r = 0;
  #endif

  return r;
}

// Renvoie 1 en cas de succès, 0 en cas d'echec.
int mbkfseek( FILE *file, MBK_OFFSET_MAX *offset, int whence )
{
  int r;
  #if defined(_LARGEFILE64_SOURCE ) && !defined(_LP64)
  if( mbk_getfileacces( file ) == MBK_FILE_STD ) 
    r = fseek( file, *((MBK_OFFSET_STD*)offset), whence );
  else
    r = fseeko64( file, *((MBK_OFFSET_LFS*)offset), whence );
  #else
  r = fseek( file, *((MBK_OFFSET_STD*)offset), whence );
  #endif

  if( r<0 )
    return 0;
  return 1;
}

long long unsigned mbk_getfilesize( char *name )
{
  #if defined(_LARGEFILE64_SOURCE ) && !defined(_LP64)
  struct stat64 infos;
  int r;

  r=stat64( name, &infos );
  if( r==-1) return 0;
  return (long long unsigned) infos.st_size;
  #else
  struct stat infos;
  int r;

  r=stat( name, &infos );
  if( r==-1) {
    if( errno == EOVERFLOW ) {
      avt_errmsg (MBK_ERRMSG, "047", AVT_FATAL, name);
    }
    return 0;
  }
  return (long long unsigned) infos.st_size;
  #endif
}

FILE *mbksysfopen( char *name, char *mode, char access )
{
  FILE *file;
  
  if( strchr(mode,'w')!=NULL || strchr(mode,'a')!=NULL) {
    #if defined(_LARGEFILE64_SOURCE ) && !defined(_LP64)
    switch( access ) {
    case MBK_FILE_LFS :
    case 0:
      file = fopen64( name, mode );
      mbk_setfileacces( file, MBK_FILE_LFS );
      break;
    case MBK_FILE_STD :
      file = fopen( name, mode );
      mbk_setfileacces( file, MBK_FILE_STD );
      break;
    default:
      avt_errmsg (MBK_ERRMSG, "048", AVT_FATAL);
    }
    #else
    file = fopen( name, mode );
    mbk_setfileacces( file, MBK_FILE_STD );
    #endif
  }
  else {
    #if defined(_LARGEFILE64_SOURCE ) && !defined(_LP64)
    if( access == 0 ) {
      if( mbk_getfilesize( filename ) > MBK_LIMIT_USELARGEFILE ) {
        file = fopen64( name, mode );
        mbk_setfileacces( file, MBK_FILE_LFS );
      }
      else {
        file = fopen( name, mode );
        mbk_setfileacces( file, MBK_FILE_STD );
      }
    }
    else {
      if( access == MBK_FILE_LFS ) {
        file = fopen64( name, mode );
        mbk_setfileacces( file, MBK_FILE_LFS );
      }
      else {
        file = fopen( name, mode );
        mbk_setfileacces( file, MBK_FILE_STD );
      }
    }
    #else
    file = fopen( name, mode );
    mbk_setfileacces( file, MBK_FILE_STD );
    #endif
  }

  access=0;
  return file;
}

/******************************************************************************/

// Ouvre un fichier. Le nom UNIX de ce fichier est stocké dans la variable 
// global MBKFOPEN_NAME.

FILE *mbkfopentrace(name, extension, mode, access, allowcompress)
char *name, *extension, *mode;
char access, allowcompress ;
{
FILE *in;
FILE *infilter;
int i;
char suffix[512], suffixfilter[512] ;
char filenamefilter[BUFSIZE];
char *prefixlist[64];
char local ;

  if (!CATA_LIB || !WORK_LIB)
    mbkenv(); /* not done yet */

  if( *mode != 'r' && *mode !='w' && *mode !='a') {
    fflush( stdout );
    fprintf( stderr,
             "Fatal error in mbkfopentrace() : unknown mode %s\n", 
             mode 
           );
    EXIT(1);
  }

  if( *mode == 'w' || *mode == 'a' || mode[1] == 'l' )
    local = 1 ;
  else
    local = 0 ;
  
  if (*mode == 'a') allowcompress=0;

  // Le tableau prefixlist contient la liste des répertoires où acceder les
  // fichiers.
 
  if( name[0]=='/' ) {
    prefixlist[0]="";
    prefixlist[1]=NULL;
  }
  else {
    prefixlist[0] = WORK_LIB;
    if( local == 1 ) {
      prefixlist[1] = NULL;
    }
    else {
      i=0;
      while (CATA_LIB[i]) {
        prefixlist[i+1] = CATA_LIB[i];
        i++;
      }
      prefixlist[i+1] = NULL;
    }
  }

  // Accès aux fichiers
  i=0;
  while( prefixlist[i] ) {
  
    if( extension )
      sprintf( suffix, ".%s", extension );
    else
      suffix[0]='\0';

    if( ( ( OUT_FILTER && *mode == 'w' ) || ( IN_FILTER && *mode == 'r' ))
        && FILTER_SFX && allowcompress )
      sprintf( suffixfilter, "%s", FILTER_SFX );
    else
      suffixfilter[0]='\0';

    if (strcmp(prefixlist[i],"")!=0)
    {
      sprintf( filename, "%s/%s%s", prefixlist[i], name, suffix );
      sprintf( filenamefilter, "%s/%s%s%s", prefixlist[i], 
                                            name, 
                                            suffix, 
                                            suffixfilter 
             );
    }
    else
    {
      sprintf( filename, "%s%s", name, suffix );
      sprintf( filenamefilter, "%s%s%s", name, 
                                            suffix, 
                                            suffixfilter 
             );
    }

    if (V_STR_TAB[__AVT_EXCLUDE_COMPRESSION].VALUE!=NULL && *mode=='w')
    {
      char buf[1024];
      char *c, *tok;
      strcpy(buf, V_STR_TAB[__AVT_EXCLUDE_COMPRESSION].VALUE);
      tok=strtok_r(buf, " ", &c);
      while (tok!=NULL)
      {
        if (mbk_TestREGEX(filename, tok))
        {
          suffixfilter[0]='\0';
          break;
        }
        tok=strtok_r(NULL, " ", &c);
      }
    }

    // Stratégies différentes selon le mode d'ouverture :
    // Ecriture : le format preféré est le compressé, l'autre est effacé
    // Lecture : le format préféré est le non compressé, l'autre est ignoré

    if( *mode == 'r' ) {
   
      in = mbksysfopen( filename, "r", access );
      mbkfopen_infos( in, filename, name, extension, mode );

      if( suffixfilter[0] ) {
        infilter = mbkpopen( filenamefilter, IN_FILTER, 'r' );
        mbkfopen_infos( infilter, filenamefilter, name, extension, mode );
      }
      else
        infilter = NULL;

      if( in && infilter ) {
        avt_errmsg (MBK_ERRMSG, "049", AVT_WARNING, filename, filenamefilter);
        fclose( infilter );
        infilter = NULL;
      }
    }
    else if ( *mode == 'w' || *mode == 'a') {

      if( suffixfilter[0] ) {
        infilter = mbkpopen( filenamefilter, OUT_FILTER, 'w' );
        mbkfopen_infos( infilter, filenamefilter, name, extension, mode );
        in = mbksysfopen( filename, "r", access );
        mbkfopen_infos( in, filename, name, extension, mode );
      }
      else {
        infilter = NULL;
        in = mbksysfopen( filename, *mode == 'a'?"a":"w", access );
        mbkfopen_infos( in, filename, name, extension, mode );
      }

      if( in && infilter ) {
        avt_errmsg (MBK_ERRMSG, "050", AVT_WARNING, filenamefilter, filename);
        fclose( in );
        unlink( filename );
        in = NULL;
      }
    } 
    
    if( infilter ) {
      strcpy( MBKFOPEN_NAME, filenamefilter );
      MBKFOPEN_FILTER = YES;
      return( infilter );
    }

    if( in ) {
      strcpy( MBKFOPEN_NAME, filename );
      MBKFOPEN_FILTER = NO;
      return( in );
    }

    i++;
  }
  return( NULL );
}

/* unlink :
   ensures that only files in the working library may be erased. */
int mbkunlink(name, extension)
char *name, *extension;
{
	if (!CATA_LIB || !WORK_LIB)
		mbkenv(); /* not done yet */

	if (extension) /* if extension is null, no dot is required */
		(void)sprintf(filename, "%s/%s.%s", WORK_LIB, name, extension);
	else
		(void)sprintf(filename, "%s/%s", WORK_LIB, name);

	return unlink(filename);
}
	
/* filepath :
   find the complete path of file from mbkfopen point of view. */
char *filepath(name, extension)
char *name, *extension;
{
FILE *in;
int i ,ln ,le ,j ;

  if (!CATA_LIB || !WORK_LIB)
    mbkenv(); /* not done yet */

  if( extension ) {
    ln = strlen( name );
    le = strlen( extension );
    for( i=ln-1, j=le-1 ; i>=0 && j>=0 && name[i] == extension[j] ; i--, j-- );
    if( name[i] == '.' )
      extension = NULL;
  }


  if (*name == '/') {
    if( extension ) sprintf(filename, "%s.%s", name, extension);
    else sprintf(filename, "%s", name);

    if ((in = mbksysfopen(filename, READ_TEXT, 0))) {
      fclose(in);
      return filename;
    }

    if( FILTER_SFX && IN_FILTER ) {
      if( extension ) sprintf(filename, "%s.%s%s", name, extension, FILTER_SFX);
      else sprintf(filename, "%s%s", name, FILTER_SFX);
      
      if ((in = mbksysfopen(filename, READ_TEXT, 0))) {
        fclose(in);
        return filename;
      }
    }   
    return NULL;
  }
      
  if (extension) /* if extension is null, no dot is required */
    (void)sprintf(filename, "%s/%s.%s", WORK_LIB, name, extension );
  else
    (void)sprintf(filename, "%s/%s", WORK_LIB, name );
  
  if ((in = mbksysfopen(filename, READ_TEXT, 0))) {
    (void)fclose(in);
    return filename;
  }

  if( FILTER_SFX && IN_FILTER )
  {
    if (extension) /* if extension is null, no dot is required */
      (void)sprintf(filename, "%s/%s.%s%s", WORK_LIB, name, extension, FILTER_SFX );
    else
      (void)sprintf(filename, "%s/%s%s", WORK_LIB, name, FILTER_SFX );
    
    if ((in = mbksysfopen(filename, READ_TEXT, 0))) {
      (void)fclose(in);
      return filename;
    }
  }


  i = 0;
  while (CATA_LIB[i]) {
    if (extension) /* if extension is null, no dot is required */
      (void)sprintf(filename, "%s/%s.%s", CATA_LIB[i], name, extension );
    else
      (void)sprintf(filename, "%s/%s", CATA_LIB[i], name );
    
    if ((in = mbksysfopen(filename, READ_TEXT, 0))) {
      (void)fclose(in);
      return filename;
    }
  
    if( FILTER_SFX && IN_FILTER )
    {
      if (extension) /* if extension is null, no dot is required */
        (void)sprintf(filename, "%s/%s.%s%s", CATA_LIB[i], name, extension, FILTER_SFX );
      else
        (void)sprintf(filename, "%s/%s%s", CATA_LIB[i], name, FILTER_SFX );
      
      if ((in = mbksysfopen(filename, READ_TEXT, 0))) {
        (void)fclose(in);
        return filename;
      }
    }
    i++;
  }
  return NULL;
}

#if 0
/* process state :
   this gives some information on allocated memory, time spend, and so on. */
void mbkps()
{
#include <sys/time.h>
#include <sys/resource.h>
struct rusage rusage;
static int pagesize;
extern long mbkalloc_stat;
static int times;

	if (!times)
		pagesize = getpagesize() / 1024;
	(void)getrusage(RUSAGE_SELF, &rusage);
	(void)fprintf(stdout, "mbk stats          : %d call\n", ++times);
	(void)fprintf(stdout, "user     time      : %ld sec\n",
									rusage.ru_utime.tv_sec);
	(void)fprintf(stdout, "system   time      : %d sec\n",
									rusage.ru_stime.tv_usec);
	(void)fprintf(stdout, "real     size      : %ld k\n",
									rusage.ru_maxrss * pagesize);
	(void)fprintf(stdout, "mbkalloc peak size : %ld k\n",
									mbkalloc_stat / 1024);
	(void)fprintf(stdout, "mallinfo size      : %ld k\n",
									mallinfo().arena / 1024);
}
#endif

/* mbkpopen : open a pipe with a filter, return a FILE*
 * Gregoire.Avot
 */

FILE* mbkpopen( char *nom, char *filter, char mode )
{
  int ptf[2];
  static int fic;
  int pid;
  FILE *file;
  char **argv;
  int i;
  int errfile ;

  if( mode == 'w' )
    fic = open( nom,
                O_WRONLY|O_CREAT|O_TRUNC,
                S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH
              );
  else
    fic = open( nom, O_RDONLY );

  if( fic == -1 )
    return( NULL );

  if( pipe( ptf ) == -1 )
  {
    fflush( stdout );
    fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" )\n", nom );
    perror( "pipe() ");
    EXIT(1);
  }
  
  argv = decompfilter( filter );

  i=1 ;
  do {
    pid = vfork();
    if( pid == -1 ) {
      perror( "vfork() " );
      if( i<VFORKMAXTRY ) {
        fflush( stdout );
        printf( "vfork() #%d attempt failed. wait a few moment...\n", i );
        sleep( 6 );
      }
    }
    i++;
  }
  while( pid == -1 && i <= VFORKMAXTRY );

  if( pid == -1 )
  {
    fflush( stdout );
    fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" )\n", nom );
    printf( "Memory usage : %ldKb\n", mbkprocessmemoryusage()/1024 );
    EXIT(1);
  }
  
  if( pid == 0 )
  {
    /* fils */
    fflush( stdout );

    if( argv == NULL )
    {
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
      fprintf( stderr, "Bad filter.\n" );
      EXIT(1);
    }

    if( mode == 'w' )
    {
      if( dup2( ptf[0], 0 ) == -1 )
      {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "dup2( ptf[0], 0 ) ");
        EXIT(1);
      }
          
      if( dup2( fic, 1 ) == -1 )
      {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "dup2( fic, 1 ) ");
        EXIT(1);
      }
    }
    else
    {
      if( dup2( ptf[1], 1 ) == -1 )
      {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "dup2( ptf[1], 1 ) ");
        EXIT(1);
      }
          
      if( dup2( fic, 0 ) == -1 )
      {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "dup2( fic, 0 ) ");
        EXIT(1);
      }
    }

    if( close( fic ) == -1 )
    {
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
      perror( "close( fic ) ");
      EXIT(1);
    }
    
    if( close( ptf[1] ) == -1 )
    {
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
      perror( "close( ptf[1] ) ");
      EXIT(1);
    }

    if( close( ptf[0] ) == -1 )
    {
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
      perror( "close( ptf[0] ) ");
      EXIT(1);
    }
   
    if( V_BOOL_TAB[ __MBK_FILTER_MASK_ERROR ].VALUE ) {
      errfile = open( "/dev/null", 0 );
      if( errfile < 0 ) {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "open(\"/dev/null\",0) " );
        EXIT(1);
      }
      if( dup2( errfile, 2 ) == -1 )
      {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "dup2( errfile, 2 ) ");
        EXIT(1);
      }
      if( close( errfile ) == -1 )
      {
        fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
        perror( "close( errfile ) ");
        EXIT(1);
      }
    }
    
    if( execvp( argv[0], &(argv[1]) ) == -1 )
    {
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) child\n", nom );
      perror( "execvp " );
      EXIT(1);
    }
  }

  for( i=0 ; argv[i] ; i++ )
    mbkfree( argv[i] );
  mbkfree( argv );

  mbksetautoackchld( pid );
  close( fic );

  if( mode == 'w' )
  {  
    if( close( ptf[0] ) == -1 )
    {
      fflush( stdout );
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) parent\n", nom );
      perror( "close( ptf[0] ) ");
      EXIT(1);
    }
    
    file = fdopen( ptf[1], "w" );
    if( file == NULL )
    {
      fflush( stdout );
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) parent\n", nom );
      perror( "fdopen( ptf[1], \"w\" ) ");
      EXIT(1);
    }
  }
  else
  {  
    if( close( ptf[1] ) == -1 )
    {
      fflush( stdout );
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) parent\n", nom );
      perror( "close( ptf[1] ) ");
      EXIT(1);
    }
    
    file = fdopen( ptf[0], "r" );
    if( file == NULL )
    {
      fflush( stdout );
      fprintf( stderr, "*** Fatal error in mbkpopen( \"%s\" ) parent\n", nom );
      perror( "fdopen( ptf[0], \"r\" ) ");
      EXIT(1);
    }
  }

  return( file );
}

char **decompfilter( char *filter )
{
  int         pos;
  int         i;
  char       *pt;
  char       *fullpath;
  char       *argv0;
  char       *argvn;
  chain_list *head;
  chain_list *scan;
  int         nbarg;
  char      **ret;

  /* remove trailing space */
  for( pos = 0 ; filter[pos] == ' ' && filter[pos] != 0  ; pos++ );
  if( filter[pos] == 0 )
    return( NULL );

  /* The full path to binairie */
  for( i = pos ; filter[i] != ' ' && filter[i] != 0 ; i++ );
  fullpath = (char*)mbkalloc( sizeof(char) * ( i-pos+1 ) );
  strncpy( fullpath, filter+pos, i-pos );
  fullpath[i-pos] = 0;
  pos = i;

  /* The binairie himself */
  pt = strrchr( fullpath, '/' );
  argv0 = (char*)mbkalloc( sizeof( char ) * (strlen( fullpath ) + 1) );
  if( pt == NULL )
    strcpy( argv0, fullpath );
  else
    strcpy( argv0, pt+sizeof(char) );

  /* The arguments */
  head = NULL;
  nbarg=2;

  do
  {
    for( ; filter[pos] == ' ' && filter[pos] != 0  ; pos++ );
    if( filter[pos] == 0 )
      break;

    for( i = pos ; filter[i] != ' ' && filter[i] != 0 ; i++ );
    argvn = (char*)mbkalloc( sizeof( char ) * ( i-pos+1 ) );
    strncpy( argvn, filter+pos, i-pos );
    argvn[i-pos] = 0;
    pos = i;
    head = addchain( head, argvn );
    nbarg++;
  }
  while( 1 );

  head = reverse( head );

  ret = (char**)mbkalloc( sizeof(char*) * (nbarg+1) );

  ret[0] = fullpath;
  ret[1] = argv0;

  for( scan = head, pos=2 ; scan ; scan = scan->NEXT, pos++ )
    ret[pos] = scan->DATA;
  ret[pos] = NULL;

  freechain( head );

  return( ret );
}

void mbkackchld( int sig )
{
  int               status;
  autoackchld_list *scanauto, *prevauto, *nextauto;
  
  /* Le handlert d'interruption SIGCHLD. Il doit avoir été configuré pour ne
   * pas être interrompu par lui même. */
  
  if( sig != SIGCHLD )
  {
    fflush( stdout );
    fprintf( stderr,
             "*** mbk warning : call of mbkackchld with signal %d\n",
             (int)sig
           );
    return;
  }
 
  for( scanauto = HEAD_AUTOACKCHLD, prevauto=NULL ;
       scanauto ;
       scanauto = nextauto
     )
  {
    nextauto = scanauto->NEXT;
    if( waitpid(scanauto->PID, &status, WNOHANG) == scanauto->PID )
    {
      if( prevauto )
        prevauto->NEXT = scanauto->NEXT;
      else
        HEAD_AUTOACKCHLD = scanauto->NEXT;
      mbkfreeautoackchld( scanauto );
    }
    else prevauto = scanauto ;
  }
}

autoackchld_list* mbkgetautoackchld( void )
{
  autoackchld_list *new;

  if( ! HEAD_AUTOACKCHLD_FREE )
  {
    fflush( stdout );
    fprintf( stderr, 
             "\n*** mbk ERROR : Not enought free slot in mbkgetautoackchld().\n"
           );
    EXIT(1);
  }

  new = HEAD_AUTOACKCHLD_FREE ;
  HEAD_AUTOACKCHLD_FREE = HEAD_AUTOACKCHLD_FREE->NEXT ;

  STAT_AUTOACKCHLD++;
  return( new );
}

void mbkfreeautoackchld( autoackchld_list *bloc )
{
  bloc->NEXT = HEAD_AUTOACKCHLD_FREE ;
  HEAD_AUTOACKCHLD_FREE = bloc ;
  STAT_AUTOACKCHLD--;
}

void mbkinitautoackchld( void )
{
  autoackchld_list *bloc;
  int n;

  bloc=(autoackchld_list*)mbkalloc( sizeof( autoackchld_list ) * ENDCHLD_MAX );
  n = AUTOACKCHLD_MAX - 1;
  bloc[n].NEXT = NULL ;
  while( n > 0 )
  {
    n-- ;
    bloc[n].NEXT = &( bloc[n+1] );
  }
  HEAD_AUTOACKCHLD_FREE = &(bloc[0]);
  STAT_AUTOACKCHLD=0;
}

void mbksetautoackchld( int pid )
{
  autoackchld_list *new;
  sigset_t  set;
  int       status;

  /* Fonction utilisateur : on met dans la liste HEAD_AUTOACKCHLD un numéro de
   * pid. Dès qu'un processus fils se termine ave ce pid, il sera 
   * automatiquement supprimé. */

  /* Dans le déroulement normal du programme, on touche ici à des structures
   * qui sont modifiées par le handler SIGCHLD. On évite de mettre le bazard en
   * blindant le morceau de code. */

  sigemptyset( &set );
  sigaddset( &set, SIGCHLD );
  sigprocmask( SIG_BLOCK, &set, NULL );

  if( waitpid( pid, &status, WNOHANG ) == 0 ) /* pas deja termine */
  {
    /* On ne voudra jamais savoir ce qu'est devenu le processus fils pid. On le
     * met dans la liste des processus à acquiter automatiquement */
    new         = mbkgetautoackchld();
    new->PID    = pid;
    new->NEXT   = HEAD_AUTOACKCHLD ;
    HEAD_AUTOACKCHLD = new ;
  }
  sigprocmask( SIG_UNBLOCK, &set, NULL );
}

unsigned long mbkprocessmemoryusage()
{
  #ifdef Linux
  char temp[100];
  pid_t pid;
  FILE *f;
  int cnt;
  unsigned long size;
  pid=getpid();
  sprintf(temp,"/proc/%d/stat",pid);
  if ((f=fopen(temp,"rt"))==NULL) return (unsigned long)sbrk(0);
  for (cnt=1;cnt<=23;cnt++) fscanf(f,"%s",temp);
  size=strtoul(temp,NULL,10);
  fclose(f);
  return size;
  #endif

  #ifdef Solaris
  unsigned long total;
  char   file[255];
  int    f;
 
  sprintf( file, "/proc/%d/as", (int)getpid());
  f = open( file, O_RDONLY );
  if( f < 0 ) return (unsigned long)sbrk(0);

  total = (unsigned long )lseek( f, 0, SEEK_END ); 
  close( f );

  return total;
  #endif

  #ifdef Darwin
  struct task_basic_info t_info;
  mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
  task_t task = MACH_PORT_NULL;

  if (task_for_pid(current_task(), getpid(), &task) == KERN_SUCCESS) {
    task_info(task, TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count);
    return (unsigned long)(t_info.virtual_size - SHARED_TEXT_REGION_SIZE - SHARED_DATA_REGION_SIZE);
  }
  #endif
  return 0;
}

char *mbkIsLofigExt(char *filename, char *ext)
{
  char *c;
  c=&filename[strlen(filename)-1];
  while (c!=filename && *c!='.') c--;
  if (c==filename) return NULL;
  if (strcmp(c+1,ext)==0) return c;
  if (FILTER_SFX!=NULL)
    {
      char temp[20];
      c--;
      while (c!=filename && *c!='.') c--;
      if (c==filename) return NULL;
      sprintf(temp,".%s%s",ext,FILTER_SFX);
      if (strcmp(c,temp)==0) return c;
    }
  return 0;
}

static int done=0;
static char *ext[]={"","","","al","spi","hns","fdn","vhd","vlg","v","vst"};

static void setext()
{
  char *c;
  if (!done)
    {
      char *SPI_SFX="spi", *VHD_SFX="vhd", *VLG_SFX="vlg";
      if ((c=V_STR_TAB[__MBK_SPI_SUFFIX].VALUE)!=NULL) SPI_SFX=c;
      if ((c=V_STR_TAB[__MVL_FILE_SUFFIX].VALUE)!=NULL) VHD_SFX=c;
      if ((c=V_STR_TAB[__MGL_FILE_SUFFIX].VALUE)!=NULL) VLG_SFX=c;
      ext[0]=SPI_SFX;
      ext[1]=VHD_SFX;
      ext[2]=VLG_SFX;
      done=1;
    }
}

char *mbkFileIsLofig(char *filename)
{
  unsigned int i;
  char *c;
  setext();
  for (i=0;i<sizeof(ext)/sizeof(*ext); i++)
    if ((c=mbkIsLofigExt(filename, ext[i]))!=NULL) return c;
  return NULL;
}


/****************************************************************************/
/*{{{                    Pipe functions                                     */
/*                                                                          */
/* author : Antony PinTo (anto)                                             */
/*                                                                          */
/* 8 january 2004 : anto creation                                           */
/*                                                                          */
/****************************************************************************/
/*{{{                    mbkpprint()                                        */
/*                                                                          */
/* write into a pipe                                                        */
/*                                                                          */
/****************************************************************************/
int mbkpprint(int fildes, void *data, int size)
{
  ssize_t    writen, remaining;

  remaining     = (ssize_t)size;
  writen        = 0;
  
  while (remaining)
  {
    writen     += write(fildes,(char*)data+writen,remaining);
    if (writen < 0)
      break;
    remaining   = size - writen;
  }
  
  return (writen == (ssize_t)size);
}

/*}}}************************************************************************/
/*{{{                    mbkpscan()                                         */
/*                                                                          */
/* read a pipe                                                              */
/*                                                                          */
/****************************************************************************/
int mbkpscan(int fildes, void *data, int size)
{
  ssize_t    readen, remaining, this_time;

  remaining     = (ssize_t)size;
  readen        = 0;
  
  while (remaining)
  {
    this_time   = read(fildes,(char*)data+readen,remaining);
    readen     += this_time;
    if (readen < 0)
      break;
    remaining   = size - readen;
  }
  
  return (readen == (ssize_t)size);
}

/*}}}************************************************************************/
/*{{{                    mbkpcreate()                                       */
/*                                                                          */
/* forks and create the communication pipes                                 */
/*                                                                          */
/* return the result of fork()                                              */
/*                                                                          */
/****************************************************************************/
pid_t mbkpcreate(int *file_des_write, int *file_des_read)
{
  pid_t      son_id;
  int        server_to_client[2];
  int        client_to_server[2];
  
  pipe(server_to_client);
  pipe(client_to_server);
  
  switch ((son_id = fork()))
  {
    case -1 :
         *file_des_write    = 0;
         *file_des_read     = 0;
         break;
    case 0 : // son is the client
         *file_des_write    = client_to_server[1];
         *file_des_read     = server_to_client[0];
         break;
    default : // father is the server
         *file_des_write    = server_to_client[1];
         *file_des_read     = client_to_server[0];
         break;
  }
  
  return son_id;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    Test if come from gdb                              */
/****************************************************************************/
/*{{{                    isGdb()                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline
int isGdb(char *name)
{
  return !strncmp("gdb",name,3);
}

/*}}}************************************************************************/
/*{{{                    mbk_whoAmI()                                       */
/*                                                                          */
/* fill the buffer name with the process name                               */
/*                                                                          */
/****************************************************************************/
#ifdef Linux
char *mbk_whoAmI(pid_t pd, char *name)
{
  char       buf[1024], *p;
  int        fd;
  int        len;
  char      *res;
  char      *q;

  res       = (char*)-1;
  
  sprintf(buf,"/proc/%d/stat",(int)pd);
  
  if ((fd = open(buf,O_RDONLY)) >= 0)
  {
    len         = read(fd, buf, sizeof(buf)-1);
    buf[len]    = '\0';
    p           = buf;
    p           = strchr(p,'(')+1;
    q           = strrchr(p,')');
    len         = q-p;
    if (len >= (signed)sizeof(name))
      len       = sizeof(name)-1;
    memcpy(name,p,len);
    name[len]   = 0;
    res         = name;
    close(fd);
  }
  return res;
}
#endif

#ifdef Solaris
char *mbk_whoAmI(pid_t pd, char *name)
{
  char       buf[1024], *p;
  int        fd;
  int        len;
  char      *res;

  psinfo_t   curproc;
  
  res       = (char*)-1;
  
  sprintf(buf,"/proc/%d/psinfo",(int)pd);
  
  if ((fd = open(buf,O_RDONLY)) >= 0)
  {
    if ((read(fd,&curproc,sizeof(psinfo_t))) == sizeof(psinfo_t))
    {
      p         = curproc.pr_fname;
      len       = strlen(p);
      memcpy(name,p,len);
      name[len] = 0;
      res       = name;
    }
    close(fd);
  }
  return res;
}
#endif

#ifdef Darwin
char *mbk_whoAmI(pid_t pd, char *name)
{
    int mib[4];
    size_t len = sizeof(struct kinfo_proc);
    struct kinfo_proc kp;
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = pd;
    if (sysctl(mib, 4, &kp, &len, NULL, 0) == 0) {
        strcpy(name, kp.kp_proc.p_comm);
    }
    else strcpy(name, "unknown");
    
    return name;
}
#endif

#ifdef Win32
char *mbk_whoAmI(pid_t pd, char *name)
{
  /* TODO */
  strcpy(name, "unknown");
  
  return name;
}
#endif

/*}}}************************************************************************/
/*{{{                    mbk_whoAmILong()                                   */
/*                                                                          */
/* fill the buffer name with the process name                               */
/*                                                                          */
/****************************************************************************/
#ifdef Linux
char *mbk_whoAmILong(pid_t pd, char *name)
{
  char           buf[1024];
  int            fd;
  int            len, done;
  char          *res;
  int            i;
  char           buf2[1024];
  res       = (char*)-1;
  
  sprintf(buf,"/proc/%d/exe",(int)pd);
  len           = readlink(buf,buf2,sizeof(buf));
  buf2[len]     = '\0';
    
  sprintf(buf,"/proc/%d/cmdline",(int)pd);
  if ((fd = open(buf,O_RDONLY)) >= 0)
  {
    done        = 0;
    len         = read(fd, buf, sizeof(buf)-1);
    for (i = 0; i < len; i ++)
      if (buf[i] == '\0')
      {
        if (!done)
          done  = i;
        buf[i]  = ' ';
      }
    buf[len]    = '\0';
    close(fd);
  }
  sprintf(name,"%s %s",buf2,buf + done + 1);
  
  return res;
}
#endif

#ifdef Solaris
char *mbk_whoAmILong(pid_t pd, char *name)
{
  char           buf[1024];
  int            fd;
  int            len, done;
  char          *res;
  char          *p, *env, *tt, *strtok;
  psinfo_t       curproc;
  struct stat    st;
  
  res       = (char*)-1;
  
  sprintf(buf,"/proc/%d/psinfo",(int)pd);
  if ((fd = open(buf,O_RDONLY)) >= 0)
  {
    done        = 0;
    if ((read(fd,&curproc,sizeof(psinfo_t))) == sizeof(psinfo_t))
    {
      p                 = curproc.pr_psargs;
      if (p[0] != '/')
        if ((env = getenv("PATH")))
          while (!done)
          {
            strtok      = strtok_r(env,":",&tt);
            env         = NULL;
            sprintf(name,"%s/%s",strtok,curproc.pr_fname);
            if (!stat(name,&st) && (st.st_mode & (S_IROTH | S_IXOTH)))
            {
              sprintf(name,"%s/%s",strtok,curproc.pr_psargs);
              done      = 1;
            }

            if (tt)
              tt[-1]    = ':';
            else
              done      = 2;
          }

      if (done != 1)
      {
        len             = strlen(p);
        memcpy(name,p,len);
        name[len]       = 0;
      }
    }
    res                 = name;
    close(fd);
  }
  
  return res;
}
#endif

#ifdef Darwin
char *mbk_whoAmILong(pid_t pd, char *name)
{
    int mib[4];
    size_t len = sizeof(struct kinfo_proc);
    struct kinfo_proc kp;
    
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = pd;
    if (sysctl(mib, 4, &kp, &len, NULL, 0) == 0) {
        strcpy(name, kp.kp_proc.p_comm);
    }
    else strcpy(name, "unknown");
    
    return name;
}
#endif

#ifdef Win32
char *mbk_whoAmILong(pid_t pd, char *name)
{
  /* TODO */
  strcpy(name, "unknown");
  
  return name;
}
#endif
/*}}}************************************************************************/
/*}}}************************************************************************/

chain_list *BuildFileList(char *name, chain_list *regx, int once)
{
  DIR            *dp;
  struct dirent  *ep;
  char buf[1024], *regxname, buf0[1024];
  char str[3];
  int i;
  chain_list *cl = NULL, *ch, *oncelist=NULL;
/*  regex_t regular_expression;
  regmatch_t pmatch[1];*/
  char **catal;
  char *mycatal[]={"", NULL};
  struct stat st;
  char save_sensi;

  save_sensi=CASE_SENSITIVE;
  CASE_SENSITIVE='Y';

  strcpy(str," ");
  while (regx!=NULL)
    {
      regxname=(char *)regx->DATA;
      if (name!=NULL)
        {
          strcpy(buf,"");
          for (i=0; regxname[i]!='\0'; i++)
            {
              str[0]=regxname[i];
              if (regxname[i]=='$') strcat(buf, name);
              else strcat(buf, str);
            }
        }
      else
        strcpy(buf, regxname);
      
      if ((regxname=strrchr(buf, '/'))==NULL)
        {          
          catal=CATA_LIB;
          regxname=buf;
        }
      else 
        {
          catal=mycatal;
          *regxname='\0';
          mycatal[0]=buf;
          regxname++;
        }
      
      for (i=0; catal[i]!=NULL; i++)
        {
          dp = opendir(catal[i]);
          if (dp != NULL) 
            {              
              while ((ep = readdir(dp)) != NULL)
                {
                  if (mbk_TestREGEX(ep->d_name, regxname))
                    {
                      if (once)
                        {
                          for (ch=oncelist; ch!=NULL && strcmp((char *)ch->DATA, ep->d_name)!=0; ch=ch->NEXT) ;
                          if (ch!=NULL) continue;
                        }
                      sprintf(buf0, "%s/%s",catal[i],ep->d_name);
                      
                      if (stat(buf0, &st)!=ENOENT && !S_ISDIR(st.st_mode))
                        {
                          for (ch=cl; ch!=NULL && strcmp((char *)ch->DATA, buf0)!=0; ch=ch->NEXT);
                          if (ch==NULL)
                            {
                              cl=addchain(cl, mbkstrdup(buf0));
                              if (once) oncelist=addchain(oncelist, mbkstrdup(ep->d_name));
                            }
                        }
                    }
                }
              closedir(dp);
            }
        }
      regx=regx->NEXT;
    }

  CASE_SENSITIVE=save_sensi;

  for (ch=oncelist; ch!=NULL; ch=ch->NEXT)
    mbkfree(ch->DATA);
  freechain(oncelist);

  return reverse(cl);
}

#undef mbk_DisplayProgress

void mbk_DisplayProgress(long *TM, char *mes, int cur, int tot, char mode)
{
  long tmp;

  if (!isatty (fileno(stdout))) return;
  if (mode=='e' || *TM!=(tmp=time(NULL)))
  {                
    *TM=tmp;
    if (mode=='%')
      fprintf(stdout, "\r%s[%3.2f%%]", mes, (cur*100.0)/tot);
    else
      fprintf(stdout, "\r%s[%07d/%07d]", mes, cur, tot);
    if (mode=='e') fprintf(stdout, "\n");
    fflush(stdout);
  }
}

void mbk_sem_init(mbk_sem_t *sm, int value)
{
  sm->nb=value;
  pthread_mutex_init(&sm->mutex, NULL);
  pthread_cond_init(&sm->cond, NULL);
}

void mbk_sem_destroy(mbk_sem_t *sm)
{
  pthread_mutex_destroy(&sm->mutex);
  pthread_cond_destroy(&sm->cond);
}

void mbk_sem_wait(mbk_sem_t *sm)
{
  int rc;
  pthread_mutex_lock(&sm->mutex);
  while (sm->nb<=0) pthread_cond_wait(&sm->cond, &sm->mutex);
  sm->nb--;
  pthread_mutex_unlock(&sm->mutex);
}

void mbk_sem_post(mbk_sem_t *sm)
{
  pthread_mutex_lock(&sm->mutex);
  sm->nb++;
  pthread_mutex_unlock(&sm->mutex);
  pthread_cond_signal(&sm->cond); 
}
