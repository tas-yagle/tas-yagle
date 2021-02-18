#include AVT_H
#include <stdlib.h>

ptype_list *AVTERR=NULL;
ptype_list *AVTLIB=NULL;
ptype_list *AVTNUM=NULL;

char *AVT_BINARY_NAME="";
int   AVT_MAXNBERRMSG = 3 ;
static int TOTAL_WARNINGS=0, TOTAL_ERRORS=0;

extern struct msgtab errmsg_tab[];

typedef struct {
  char *msg ;
  int   num ;
} avterrnum ;

FILE *AVTLOGFILE=NULL ;
int   AVTLOGINDENT=0;
int   AVTLOGNAMELENGTH=0;

libloginfo LIBLOGINFO[] = {
  /*  0 */ { "MCC",        "MCC", 0 },
  /*  1 */ { "TRC",        "TRC", 0 },
  /*  2 */ { "YAG",        "YAG", 0 },
  /*  3 */ { "MBK",        "MBK", 0 },
  /*  4 */ { "FILEACCESS", "FAC", 0 },
  /*  5 */ { "MBKCACHE",   "MCH", 0 },
  /*  6 */ { "RCN",        "RCN", 0 },
  /*  7 */ { "EFG",        "EFG", 0 },
  /*  8 */ { "GSP",        "GSP", 0 },
  /*  9 */ { "LOG",        "LOG", 1 },
  /* 10 */ { "TLF",        "TLF", 0 },
  /* 11 */ { "LIB",        "LIB", 0 },
  /* 12 */ { "ZEN",        "ZEN", 0 },
  /* 13 */ { "STATPRS",    "PRS", 0 },
  /* 14 */ { "ERROR",      "ERR", 0 },
  /* 15 */ { "TMA",        "TMA", 0 },
  /* 16 */ { "CTK",        "CTK", 0 },
  /* 17 */ { "TAS",        "TAS", 0 },
  /* 18 */ { "STM",        "STM", 0 },
  /* 19 */ { "FALSEPATH",  "FPT", 0 },
  /* 20 */ { "GNS",        "GNS", 0 },
  /* 21 */ { "BEG",        "BEG", 0 },
  /* 22 */ { "CONFIG",     "CNF", 0 },
  /* 23 */ { "STABILITY",  "STB", 0 },
  /* 24 */ { "DEBUG",      "DBG", 0 },
  /* 25 */ { "COM",        "COM", 0 },
  /* 26 */ { "SPI",        "SPI", 0 }
} ;

liblogpool LIBLOGPOOL[] = {
  { "mos_charac",    { LOGMCC, -1 } },
  { "spicedeck",     { LOGEFG, LOGGSP, -1 } },
  { "files",         { LOGMBKCACHE, LOGFILEACCESS, -1 } },
  { "disassembly",   { LOGYAG, -1 } },
  { "rc",            { LOGRCN, LOGTRC, -1 } },
  { "timing_abs",    { LOGTLF, LOGLIB, -1 } },
  { "error",         { LOGERROR, -1 } },
  { "stat",          { LOGSTATPRS, LOGFALSEPATH, -1 } },
  { "delay_calc",    { LOGTAS, LOGSTM, -1 } },
  { "gns",           { LOGGNS, LOGBEG, -1 } },
  { "config",        { LOGCONFIG, -1 } },
  { "stability",     { LOGSTABILITY, -1 } },
  { "spice",         { LOGSPI, -1 } },
  { 0x0, { -1 } }
} ;

void avt_initerrmsg (char *tool)
{
    unsigned int i=0;
    unsigned int j=0;
    unsigned int k;
    avterrnum *ptnum;
    
    if (tool)
        AVT_BINARY_NAME = tool;

    if (AVTERR || AVTLIB || AVTNUM) return ;
    
    while (errmsg_tab[i].type) {
    if( getptype(AVTERR,(long)(errmsg_tab[i].type)) == NULL )
        AVTERR = addptype(AVTERR,(long)(errmsg_tab[i].type),(void *)(errmsg_tab[i].data));
    if( getptype(AVTLIB,(long)(errmsg_tab[i].type)) == NULL )
        AVTLIB = addptype(AVTLIB,(long)(errmsg_tab[i].type),(void *)(errmsg_tab[i].lib));
    if( getptype(AVTNUM,(long)(errmsg_tab[i].type)) == NULL ) {
        for( j=0 ; errmsg_tab[i].data[j] ; j++ );
        if( j ) {
          k=j/2;
          ptnum = mbkalloc( sizeof( avterrnum ) * (k+1) );
          for( j=0 ; j<k ; j++ ) {
            ptnum[j].msg = errmsg_tab[i].data[2*j] ;
            ptnum[j].num = 0 ;
          }
          ptnum[k].msg=NULL;
          ptnum[k].num=0;
        }
        else
          ptnum = NULL ;
        AVTNUM = addptype(AVTNUM,(long)(errmsg_tab[i].type),ptnum );
    }
    i++;
    }
}

char *avt_geterrmsg (long lib,char *num)
{
    unsigned int i=0;
    ptype_list *pt=NULL;
    char **errtab=NULL;
    
    if(! AVTERR)
        avt_initerrmsg(AVT_BINARY_NAME);
    
    if ( ( pt=getptype(AVTERR,(long)lib) ) ) {
        errtab=(char **)(pt->DATA);
        while (errtab[i]) {
            if (!strcmp (num, errtab[i])) return errtab[i + 1];
            i += 2;
        } 
    } 
    
    fprintf(stderr,"[Unknown Error Number]:\n'%02ld-%s' is not a valid error number\n",lib,num);
    return NULL;
}

char *avt_geterrlib (long lib)
{
    ptype_list *pt=NULL;
    char *libname=NULL;
    
    if(! AVTLIB)
        avt_initerrmsg(AVT_BINARY_NAME);
    
    if ( ( pt=getptype(AVTLIB,(long)lib) ) )
        if ( (libname=(char *)(pt->DATA))!=NULL ) return libname;
    
    fprintf(stderr,"[Unknown Library Number]:\n'%02ld' is not a valid library number\n",lib);
    return NULL;
}

int avt_getprintederrmsg( long lib, char *num )
{
  ptype_list  *ptlnum ;
  int          i ;
  avterrnum   *ptnum ;
  int          n=0 ;

  ptlnum = getptype( AVTNUM, lib ) ;

  if( ptlnum ) {
    
    if( ptlnum->DATA ) {

      ptnum = (avterrnum*)ptlnum->DATA ;
      for( i=0 ; ptnum[i].msg && strcmp( ptnum[i].msg, num ) ; i++ );
      if( ptnum[i].msg ) {
        ptnum[i].num++;
        n = ptnum[i].num ;
      }
    }
  }

  return n ;
}

static int message_filter_logged=0;

static void print_color(FILE *outstream, int type)
{
  switch( type ) {
      case AVT_ERROR   : avt_fprintf(outstream,"¤6")    ; break ;
      case AVT_WARNING : avt_fprintf(outstream,"¤4") ; break ;
      case AVT_FATAL   : avt_fprintf(outstream,"¤6")    ; break ;
      default          : avt_fprintf(outstream,"¤7")  ; break ;
  }
}

static int encrypted_mode = 0;

void avt_set_encrypted_mode(int mode)
{
    encrypted_mode = mode;
}

void avt_errmsg (long lib, char *num, int type, ...)
{
    va_list va;
    char *msg=NULL;
    char *libname=NULL;
    char *buff;
    char resbuf[4096];
    char filtercheck[16];
    int  n;
    char *filter;
    FILE *outstream;

    libname = avt_geterrlib(lib);
    
    if (!message_filter_logged && V_STR_TAB[__AVT_WARN_FILTER].VALUE!=NULL)
    {
       message_filter_logged++;
       avt_errmsg( AVT_ERRMSG, "047", AVT_WARNING, V_STR_TAB[__AVT_WARN_FILTER].VALUE);
    }
    
    if (type == AVT_WARNING && (filter = V_STR_TAB[__AVT_WARN_FILTER].VALUE) != NULL && libname != NULL)
      {
        sprintf(filtercheck, "%s-%s", libname, num);
        if (strstr(filter, filtercheck)) return;
      }

    va_start(va,type);
    
    outstream=stderr;
    switch( type ) 
      {
      case AVT_ERROR: 
          if  (!strcasecmp (V_STR_TAB[__AVT_ERROR_POLICY].VALUE , "strict"))    
              buff="Error", TOTAL_ERRORS++;
          else 
              buff="Warning", TOTAL_WARNINGS++, outstream=stdout; 
          break;
      case AVT_WARNING: buff="Warning"; TOTAL_WARNINGS++, outstream=stdout; break;
      case AVT_FATAL: buff="Error"; TOTAL_ERRORS++; break;
      default: buff="Fatal Internal Error";        
      }
            
    if( (msg=avt_geterrmsg(lib,num)) ) {
        if( libname ) {
            if (encrypted_mode) {
                sprintf(resbuf, "In encrypted data\n");
            }
            else vsprintf(resbuf,msg,va);
            if( avt_islog( 1, LOGERROR ) ) {
              n = avt_getprintederrmsg( lib, num );
              avt_log( LOGERROR, 1, "[%s %s-%s] #%d: %s",buff,libname,num,n,resbuf);
              if( n < AVT_MAXNBERRMSG ) {
                print_color(outstream, type);
                avt_fprintf(outstream,"[%s %s-%s]: ",buff,libname,num);
                avt_fprintf(outstream,"%s",resbuf);
                avt_fprintf(outstream,"¤.");
              }
              else {
                if( n == AVT_MAXNBERRMSG ) {
                  print_color(outstream, type);
                  avt_fprintf(outstream,"[%s %s-%s]: more errors of that kind reported in log file\n",buff,libname,num);
                  avt_fprintf(outstream,"¤.");
                }
              }
            }
            else {
              print_color(outstream, type);
              avt_fprintf(outstream,"[%s %s-%s]: ",buff,libname,num);
              avt_fprintf(outstream,"%s",resbuf);
              avt_fprintf(outstream,"¤.");
              fflush( outstream );
           }
       } 
    }
    va_end(va);

    if  ( type == AVT_FATAL || type == AVT_INTERNAL )
           EXIT(1); 

    if  ( type == AVT_ERROR && !strcasecmp (V_STR_TAB[__AVT_ERROR_POLICY].VALUE , "strict") )
           EXIT(1); 

}

int avt_islog ( int level, int lib )
{
  int res=0;

  if (LIBLOGINFO[lib].LEVEL >= level )
    res=1;
  return res;
}

void avt_setlibloglevel (int lib, int level)
{
  LIBLOGINFO[lib].LEVEL = level ;
}

int avt_getlibloglevel (int lib)
{
  return LIBLOGINFO[lib].LEVEL ;
}

char* avt_getliblogshortname( int lib )
{
  return LIBLOGINFO[lib].SHORTNAME ;
}

char* avt_getliblogname( int lib )
{
  return LIBLOGINFO[lib].NAME ;
}

int avt_getliblogmax()
{
  return LOGMAX ;
}

void avt_logenterfunction( int lib, int level,char *fnname )
{
  char buf[1024];
  
  sprintf( buf, "entering function %s\n", fnname );
  avt_log( lib, level, buf );
  AVTLOGINDENT++;
}

void avt_logexitfunction( int lib, int level)
{
  AVTLOGINDENT--;
  if( AVTLOGINDENT < 0 )
    AVTLOGINDENT = 0 ;
  avt_log( lib, level, "return\n" );
  return ;
}

void avt_log( int lib, int level, char *fmt, ... )
{
  va_list arg ;
  char    buf2[4096] ;
  char    bufi[4096] ;
  char    buflibname[256] ;
  char    bufprintf[10] ;
  int     i ;
  char    todumpfile   = 0, *c, *tok, *cc ;
  char    todumpstdout = 0 ;
  static  int newline = 1 ;
  
  if (lib>=0 && !avt_islog(level,lib) ) return;
  if (encrypted_mode) return;

  if( level==0 ) {
    todumpstdout = 1 ;
    if( AVTLOGFILE )
      todumpfile = 1 ;
  }
  else {
    if( AVTLOGFILE )
      todumpfile   = 1 ;
    else
      todumpstdout = 1 ;
  }
 
  if (lib>=0)
    sprintf( buflibname, "[%s]", avt_getliblogshortname(lib) );
  else
    sprintf( buflibname, "[INFO]");

  for( i=0 ; i<AVTLOGINDENT && i<60 ; i++ )
    bufi[i]=' ';
  bufi[i]='\0';

  va_start( arg, fmt );
  vsnprintf( buf2, 4096, fmt, arg );
  va_end ( arg );
 
  tok=buf2;
  if ((c=strchr(tok, '\n'))!=NULL) *c='\0', cc="\n";
  else cc="";
  
  while (tok!=NULL) {
    if( todumpfile ) {
      if( !newline )
        fprintf( AVTLOGFILE, "%s%s", tok, cc );
      else
        fprintf( AVTLOGFILE, "%s %s%s%s", buflibname, bufi, tok, cc );
      fflush( AVTLOGFILE );
    }

    if( todumpstdout ) {
      if( (level==0 && lib>=0) || !newline)
        fprintf( stdout, "%s%s", tok, cc );
      else
        fprintf( stdout, "%s %s%s%s", buflibname, bufi, tok, cc );
      fflush( stdout );
    }

    if( c!=NULL) { 
      newline = 1, tok=c+1 ;
      if (*tok!='\0') {
        if ((c=strchr(tok, '\n'))!=NULL) *c='\0', cc="\n";
        else cc="";
      } else {
        tok=NULL;
      }
    }
    else newline = 0, tok=NULL ;
  }
}

void avt_initlog()
{
  int   i ;
  char *env ;
  char *ptend ;
  long  level ;
  char *errmsg ;
  int   defaultlevel=0;
  int   l;
  char  varname[1024];
  char  varlevel[1024] ;
 
  env = V_STR_TAB[__AVT_GLOBALLOGFILE].VALUE ;
  if( env ) {
      if( !AVTLOGFILE ) 
          AVTLOGFILE = mbkfopen(env,NULL,"w");
  }

  /* hidden variable */
  env = getenv("AVT_GLOBALLOGLEVEL");
  if( env ) {
defaultlevel = atoi( env );
  }
  /* end of hidden variable */
  

  for( i=0 ; i < avt_getliblogmax() ; i++ ) {
 
    avt_setlibloglevel( i, defaultlevel );

    l = strlen( avt_getliblogshortname( i ) );
    if( l > AVTLOGNAMELENGTH )
      AVTLOGNAMELENGTH = l ;
  }

  env = V_STR_TAB[__AVT_LOGLEVEL].VALUE;
  
  if( env ) {
  
    do {
   
      while( *env && *env==' ' ) *env++;
     
      if( *env ) {
        i=0;
        while( *env && *env != ' ' && *env != ':' ) {
          varname[i]=*env ;
          env++ ;
          i++ ;
        }
        varname[i]='\0';
        
        if( *env==':' ) {
        
          env++;
          
          i=0;
          while( *env && *env != ' ' ) {
            varlevel[i]=*env ;
            env++ ;
            i++ ;
          }
          varlevel[i]='\0';
          
          errmsg = NULL ;
      
          level = strtol( varlevel, &ptend, 10 );
        
          if( *ptend!='\0' ) {
            avt_errmsg( AVT_ERRMSG, "037", AVT_ERROR, varname, varlevel );
            level = 1;
          }
          else {
            if( level < 0 || level > 9 ) {
              avt_errmsg( AVT_ERRMSG, "038", AVT_WARNING, varname, varlevel );
              level = 1;
            }
          }
        }
        else
          level = 1 ;
        
        for( i=0 ; i < avt_getliblogmax() ; i++ ) {
          if( strcmp( varname, avt_getliblogname(i) ) == 0 ) {
            avt_setlibloglevel( i, level );
            break ;
          }
        }

        if( i >= avt_getliblogmax() ) {
        
          for( i=0 ; LIBLOGPOOL[i].NAME ; i++ ) {
            if( strcmp( varname, LIBLOGPOOL[i].NAME ) == 0 ) {

              for( l=0 ; LIBLOGPOOL[i].LIBLIST[l] >= 0 ; l++ ) 
                avt_setlibloglevel( LIBLOGPOOL[i].LIBLIST[l], level );
             
              break ;
            }
          }
          
          if( !LIBLOGPOOL[i].NAME ) 
            avt_errmsg( AVT_ERRMSG, "039", AVT_WARNING, varname );
        }
      }
    }
    while( *env );
  }
  
}

void avt_PrintErrorSummary()
{
  if (TOTAL_ERRORS!=0)
     avt_log(LOGERROR, 0, "** TOTAL ERRORS: %d\n", TOTAL_ERRORS);

  if (TOTAL_WARNINGS!=0)
     avt_log(LOGERROR, 0, "** TOTAL WARNINGS: %d\n", TOTAL_WARNINGS);
}
