#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <alloca.h>
#include <time.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#include MUT_H
#include MLO_H
#include RCN_H
#include MLU_H
#include AVT_H
#include API_H
#define AVTWIG_AVOID_CONFLICT
#define API_USE_REAL_TYPES
#include "mbk_API.h"

#ifdef Solaris
extern char *ctime_r(const time_t *clock, char *buf, int buflen);
#endif

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* function DriveNetlist (API purpose)                                        */
/*                                                                            */
/*----------------------------------------------------------------------------*/

//_____________________________________________________
// Interface for qsort
//_____________________________________________________

static int vcomp (const void *p1, const void *p2)
{
    int index1, index2;

    index1 = *(long*)p1;
    index2 = *(long*)p2;

    if (index1 > index2) return 1;
    if (index1 < index2) return -1;
    return 0;
}

/*----------------------------------------------------------------------------*/
                                                    
void mbk_Sort (int *index_array, void **value_array, int nbelem)
{
    int i;
    long *sort_array = (long*)malloc (nbelem * 2 * sizeof (long));

    for (i = 0; i < nbelem; i++) {
        sort_array[2 * i] = (long)index_array[i];
        sort_array[2 * i + 1] = (long)value_array[i];
    }

    qsort (sort_array, nbelem, sizeof (long) * 2, vcomp); 
    
    for (i = 0; i < nbelem; i++) {
        index_array[i] = (int)sort_array[2 * i];
        value_array[i] = (void*)sort_array[2 * i + 1];
    }

    free (sort_array);
}

/*----------------------------------------------------------------------------*/

void mbk_FreeList(chain_list *lst)
{
  freechain(lst);
}

/*----------------------------------------------------------------------------*/

int mbk_EndofList(chain_list *lst)
{
  return lst==NULL;
}

/*----------------------------------------------------------------------------*/

chain_list *mbk_AddListItem (chain_list *lst, void *item)
{
  return addchain ((chain_list*)lst, item);
}
/*----------------------------------------------------------------------------*/

chain_list *mbk_AppendList(chain_list *lst1, chain_list *lst2)
{
  return append (lst1, lst2);
}

/*----------------------------------------------------------------------------*/

void *mbk_GetListItem(chain_list *lst)
{
  return lst->DATA;
}

/*----------------------------------------------------------------------------*/

chain_list *mbk_GetListNext(chain_list *lst)
{
  return lst->NEXT;
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Hash Table API                                                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/

ht *mbk_NewHashTable (int size)
{
    return addht (size);
}

/*----------------------------------------------------------------------------*/

long mbk_AddHashItem (ht *htable, void *key, long value) 
{
    return addhtitem ((ht*)htable, key, (long)value);
}

void mbk_AddStringHashItem (ht *htable, char *key, char *value) 
{
  addhtitem ((ht*)htable, sensitive_namealloc(key), (long)strdup(value));
}

/*----------------------------------------------------------------------------*/

long mbk_DelHashItem (ht *htable, void *key)
{
    return delhtitem ((ht*)htable, key);
}

/*----------------------------------------------------------------------------*/

long mbk_GetHashItem (ht *htable, void *key)
{
    return gethtitem (htable, key);
}

char *mbk_GetStringHashItem (ht *htable, char *key)
{
  long res;
  res=gethtitem (htable, sensitive_namealloc(key));
  if (res!=EMPTYHT) return (char *)res;
  return "?";
}

/*----------------------------------------------------------------------------*/

void mbk_FreeHashTable (ht *htable)
{
    delht ((ht*)htable);
}
void mbk_FreeStringHashTable (ht *htable)
{
  chain_list *cl;
  cl=GetAllHTElems(htable);
  while (cl!=NULL)
  {
    free(cl->DATA);
    cl=delchain(cl,cl);
  }
  delht ((ht*)htable);
}
chain_list *mbk_GetStringHashTableKeys (ht *htable)
{
  return GetAllHTKeys(htable);
}

/*----------------------------------------------------------------------------*/

int mbk_IsEmptyHashItem (long value)
{
    return value == EMPTYHT;
}

/*----------------------------------------------------------------------------*/

lofig_list *mbk_GetNetlist (char *figname)
{
    static int env = 0;
    if (!env) {
        avtenv ();
        mbkenv ();
        env = 1;
    }
    return getlofig (figname, 'A');
}

FILE *myfopen(char *name, const char *mode)
{
  FILE *f;
  if ((f=mbksysfopen(name,(char*)mode,0))==NULL)
    {
      if (strchr(mode,'r')!=NULL)
        avt_errmsg(AVT_ERRMSG, "045", AVT_ERROR, name);
      else
        avt_errmsg(AVT_ERRMSG, "046", AVT_ERROR, name);
    }
  return f;
}

int myfputs (const char *s, FILE *stream)
{
  if (stream!=NULL)
    return fputs (s, stream);
  return 1;
}

int myfclose(FILE *f)
{
  if (f!=NULL)
    return fclose (f);
  return 0;
}

void myfflush(FILE *f)
{
  if (f!=NULL)
    return fflush (f);
}

/*----------------------------------------------------------------------------*/

int decodejob( char *numrun, int *jobs, int *parajobs )
{
  int   n ;
  char *pt ;
  
  *jobs     = -1 ;
  *parajobs = -1 ;

  n = strtol( numrun, &pt, 10 );
  if( *pt == '\0' ) {
    *jobs     = n ;
    *parajobs = 1 ;
  }
  else {
    if( *pt == ':' ) {
      *jobs = n ;
      n = strtol( pt+1, &pt, 10 );
      if( *pt == '\0' ) {
        *parajobs = n ;
      }
    }
  }
  
  if( *parajobs <= 0 || *jobs <= 0 ) {
    avt_errmsg(AVT_ERRMSG, "050", AVT_ERROR, numrun);
    return 0 ;
  }

  return 1 ;
}

#define STAT_EXEC_FREE 1
#define STAT_EXEC_WAIT 2
#define STAT_EXEC_RUN  3

typedef struct {
  int   id ;
  int   state ;
  char *buffer ;
  int size;
  FILE *myfile;
} execslot ;


void runjob( char *script, char **argv )
{
  int pid ;

  pid = vfork() ;
  if( pid < 0 ) {
    perror( "vfork() " );
    avt_errmsg(AVT_ERRMSG, "056", AVT_FATAL );
  } 
  else {
    if( pid ) {
      mbksetautoackchld( pid );
    }
    else {
      execvp( script, argv );
      perror( "execvp() " );
      avt_errmsg(AVT_ERRMSG, "056", AVT_FATAL );
    }
  }
}

FILE* openstatdatafile( char *resfile, int incremental )
{
  FILE     *file ;
  char     *point ;
  char     *lastpoint ;
  char     *name ;
  char     *ext ;
  char     *testfile ;
  int       n ;
  
  if (incremental==1)
    return mbkfopen( resfile, NULL, "al" );
  else if (incremental==-1)
    return mbkfopen( resfile, NULL, "wl" );
                    
  file = mbkfopen( resfile, NULL, "rl" );
  if( file ) 
    fclose( file );
  
  if( file ) {
  
    n = strlen( resfile );
    name = alloca( sizeof(char)*(n+1) );
    ext  = alloca( sizeof(char)*(n+1) );
    testfile = alloca( sizeof(char)*(n+10) );

    lastpoint = NULL ;
    point = resfile ;
    
    while( ( point = strchr( point, '.' ) ) ) {
      lastpoint = point ;
      point = point + 1 ; 
    }
    
    if( lastpoint ) {
      strncpy( name, resfile, lastpoint-resfile );
      name[lastpoint-resfile]='\0';
      strcpy( ext, lastpoint+1 );
    }
    else {
      strcpy( name, resfile );
      ext = NULL ;
    }
    
    n=0 ;
    
    do {

      n++ ;
      
      if( ext )
        sprintf( testfile, "%s_%03d.%s", name, n, ext );
      else
        sprintf( testfile, "%s_%03d", name, n );

      file = mbkfopen( testfile, NULL, "rl" );
      if( file )
        fclose( file );
    }
    while( file );
    file = mbkfopen( testfile, NULL, "wl" );
    avt_errmsg( AVT_ERRMSG, "057", AVT_WARNING, resfile, testfile );
  }
  else {
    file = mbkfopen( resfile, NULL, "wl" );
  }

  return file ;
}

static void filltime(char *buf)
{
  time_t             clock;

  strcpy(buf,"");
  time(&clock);

#ifdef Solaris
  ctime_r(&clock,buf,52);
#else
  ctime_r(&clock,buf);
#endif
  if (strlen(buf)>0)
    buf[strlen(buf)-1]    = '\0';
}


int handle_slave_data(FILE *file, mbk_comdata *comdata, execslot *es, chain_list **mutex, char *resdir)
{
  char *res=NULL, *data=NULL;
  int ok=0;
  unsigned int i, lg;    
  const char *cmd[]= 
    {  
      "info:",
      "ask:pwd",
      "ask:getsetflag:",
      "ask:unsetflag:",
      "ask:createfile:",
      "ask:closefile",
      "ask:writefile:",
      "ask:writefile32:",
      "ask:resdir",
    };

  for (i=0; i<sizeof(cmd)/sizeof(*cmd); i++)
    {
      if (strncmp(es->buffer,cmd[i], strlen(cmd[i]))==0)
        {
          data=&es->buffer[strlen(cmd[i])];
          break;
        }
    }

  if (data!=NULL)
    {
      switch(i)
        {
        case 0: // info:
          avt_log(-1, 0, "  Job %d: %s\n",es->id, data);
          break;
        case 1: // ask:pwd
          res=mbkalloc(1024);
          getcwd(res, 1024);
          break;
        case 2: // ask:getsetflag:
          data=sensitive_namealloc(data);
          if (getchain(*mutex, data)==NULL)
            *mutex=addchain(*mutex, data), ok=1;
          res=mbkstrdup(ok?"ok":"ko");
          break;
        case 3: // ask:unsetflag:
          data=sensitive_namealloc(data);
          if (getchain(*mutex, data)!=NULL)
            *mutex=delchaindata(*mutex, data), ok=1;
          res=mbkstrdup(ok?"ok":"ko");
          break;
        case 4: // ask:createfile:
          es->myfile=mbkfopen(data, NULL, "w");
          res=mbkstrdup(es->myfile!=NULL?"ok":"ko");
          break;
        case 5: // ask:closefile:
          if (es->myfile!=NULL)
            ok=(fclose(es->myfile)==0);
          else
            ok=0;
          res=mbkstrdup(ok?"ok":"ko");
          break;
        case 6: // ask:writefile:
//          ok=(fputs(data, es->myfile)>=0);   
          lg=es->size-(data-es->buffer)-1; // -1 pour le 0 final
//          printf("%d %d %d %d\n",strlen(data), lg, es->size, data-es->buffer);
          if (es->myfile!=NULL)
            ok=(fwrite(data, sizeof(char), lg, es->myfile)==lg);
          else
            ok=0;
          res=mbkstrdup(ok?"ok":"ko");
          break;
        case 7: // ask:writefile32:
          lg=es->size-(data-es->buffer);
//          printf("%d %d %d %d\n",strlen(data), lg, es->size, data-es->buffer);
          if ((lg % sizeof(int))!=0)
            avt_errmsg( AVT_ERRMSG, "061", AVT_FATAL);
          lg/=4;
          if (es->myfile!=NULL)
            ok=(fwrite(data, sizeof(int), lg, es->myfile)==lg);
          else
            ok=0;
          res=mbkstrdup(ok?"ok":"ko");
          break;
        case 8: // ask:resdir
          if (resdir)
            res=strdup(resdir);
          else
            res=strdup("NULL");
          break;
        default:
          avt_errmsg( AVT_ERRMSG, "059", AVT_FATAL, data, es->id );
        }
//      printf("%s\n",es->buffer);
      if (res!=NULL)
        {
          mbk_commputdata( comdata, es->id, res, strlen(res)+1);
          mbkfree(res);
        }
    }
  else
    {
      fprintf( file, "-- begin %d --\n", es->id );
      fprintf( file, "%s\n", es->buffer );
      fprintf( file, "-- end %d --\n", es->id );
      fflush( file );
    }
  return 0;
}

void runStatHiTas_sub( char *numrun, char *script, char *tool, char *resfile, int incremental, char *resdir)
{
  int       jobs ;
  int       parajobs ;
  execslot *execs ;
  char     *argv[6] ;
  char      bufid[10] ;
  char      bufsubid[10] ;
  int       i ;
  int       id ;
  int       done ;
  int       launch ;
  FILE     *file ;
  char      hostname[256] ;
  char      connection[1024] ;
  mbk_comdata *comdata ;
  int         type ;
  char buf[256];
  chain_list *mutex=NULL;

  if( ! decodejob( numrun, &jobs, &parajobs ) )
    return ;

  file = openstatdatafile( resfile, incremental);

//  printf( "running %d jobs, %d simultaneous jobs\n", jobs, parajobs );
  avt_log(-1, 0, "Running %d jobs, %d simultaneous jobs\n", jobs, parajobs );


  if (resdir!=NULL && strcmp(resdir,"NULL")!=0)
    {
      int res;
      res=mkdir(resdir, 0777);
      if (res!=0 && errno!=EEXIST)
        avt_errmsg( AVT_ERRMSG, "062", AVT_FATAL, resdir);
    }

  comdata = mbk_activatecommaster( parajobs );
  if( !comdata )
    exit(0);

  execs = alloca( sizeof( execslot ) * parajobs );
    for( i=0 ; i<parajobs ; i++ ) {
    execs[i].state = STAT_EXEC_FREE ;
  }

  gethostname( hostname, 256 );

  argv[0] = script ;
  argv[1] = tool ;
  argv[2] = bufid ;
  argv[3] = bufsubid ;
  argv[4] = connection ;
  argv[5] = NULL ;
  
  done   = 0 ;
  launch = 0 ;

  while( done < jobs ) {

    for( i=0 ; i < parajobs ; i++ ) {
    
      if( launch < jobs ) {
        if( execs[i].state == STAT_EXEC_FREE ) {
        
          launch++ ;
          execs[i].id    = launch ;
          execs[i].state = STAT_EXEC_WAIT ;
          sprintf( bufid, "%d", launch );
          sprintf( bufsubid, "%d", i+1 );
          sprintf( connection, "%s:%d:%d", hostname, comdata->port, launch );

//          printf( "starting job %d : %d\n", launch, i+1 );
          avt_log(-1, 0, "Starting job %d : %d\n", launch, i+1 );
          runjob( script, argv );
          
        }
      }
    }

    if( done < jobs ) {
      id = mbk_comwaiteventfromnet( comdata );

      if( id >= 0  ) {

        avt_log( LOGMBKCOM, 1, "an event is available for %d\n", id );

        for( i=0 ; i<parajobs ; i++ ) {
          if( execs[i].state == STAT_EXEC_RUN ||
              execs[i].state == STAT_EXEC_WAIT   ) {
            if( execs[i].id == id )
              break ;
          }
        }
        
        if( i >= parajobs )
          avt_errmsg( AVT_ERRMSG, "051", AVT_FATAL, id );
        
        type = mbk_commgetdata( comdata, execs[i].id, &(execs[i].buffer), &execs[i].size );

        if( type == MBK_COM_BEGIN ) {
          if( execs[i].state != STAT_EXEC_WAIT )
            avt_errmsg( AVT_ERRMSG, "052", AVT_FATAL, execs[i].id );
          execs[i].state = STAT_EXEC_RUN ;
          filltime(buf);
          avt_log(-1, 0, "  Job %d started [%s]\n", execs[i].id, buf ) ;
        }
        
        if( type == MBK_COM_DATA ) {
          avt_log( LOGMBKCOM, 1, "data received from job %d\n", execs[i].id );
          if( execs[i].state != STAT_EXEC_RUN )
            avt_errmsg( AVT_ERRMSG, "053", AVT_FATAL, execs[i].id );

          handle_slave_data(file, comdata, &execs[i], &mutex, resdir);

          mbkfree( execs[i].buffer );
          execs[i].buffer = NULL ;
        }

        if( type == MBK_COM_END ) {
          if( execs[i].state != STAT_EXEC_RUN )
            avt_errmsg( AVT_ERRMSG, "054", AVT_FATAL, execs[i].id );
//          printf( "  job %d completed\n", execs[i].id );
          filltime(buf);
          avt_log(-1, 0, "  Job %d completed [%s]\n", execs[i].id, buf ) ;
          execs[i].state = STAT_EXEC_FREE ;
          done++ ;
        }

        if( type == MBK_COM_ABORT ) {
          filltime(buf);
          if( V_BOOL_TAB[ __TAS_STAT_HITAS_BREAK_ON_ERROR ].VALUE )
            avt_errmsg( AVT_ERRMSG, "055", AVT_FATAL, execs[i].id, buf );
          else
            avt_errmsg( AVT_ERRMSG, "055", AVT_WARNING, execs[i].id, buf );
          execs[i].state = STAT_EXEC_FREE ;
          done++ ;
        }
      }
    }
  }

  fclose( file );
  mbk_terminatecommaster( comdata );
}

void avt_McInfo( char *msg )
{
  if (MBK_SOCK!=-1)
  {
    char buf[2048];
    sprintf(buf,"info:%s", msg);
    mbk_sendmsg( MBK_COM_DATA, buf, strlen( buf )+1 );
  }
}

void avt_McPostData( char *msg )
{
  if (MBK_SOCK!=-1)
  {
    mbk_sendmsg( MBK_COM_DATA, msg, strlen( msg )+1 );
    mbk_sendmsg( MBK_COM_END, NULL, 0 );
  }
}

char *avt_McAsk( char *msg )
{
  if (MBK_SOCK!=-1)
  {
    char buf[2048], *res;
    sprintf(buf,"ask:%s", msg);
    res=mbk_commtalk(buf, -1);
    return res;
  }
  return NULL;
}

int avt_McIsSlave()
{
  return MBK_SOCK!=-1;
}


quicklist *mbk_QuickListCreate()
{
  quicklist *ql=mbkalloc(sizeof(quicklist));
  ql->head=ql->end=NULL;
  ql->nb=0;
  return ql;
}

void mbk_QuickListAppend(quicklist *ql, char *item)
{
  if (ql->end==NULL)
    ql->end=ql->head=addchain(ql->head, strdup(item));
  else
   {
     ql->end->NEXT=addchain(NULL, strdup(item));
     ql->end=ql->end->NEXT;
   }
  ql->nb++;
}

StringList *mbk_QuickListToTCL(quicklist *ql)
{
  chain_list *temp=ql->head;
  mbkfree(ql);
  return temp;
}

int mbk_QuickListLength(quicklist *ql)
{
  return ql->nb;
}

void mbk_QuickListAppendDouble(quicklist *ql, double value)
{
  chain_list *new;
  if (ql->end==NULL)
    new=ql->end=ql->head=addchain(ql->head, NULL);
  else
   {
     new=ql->end->NEXT=addchain(NULL, NULL);
     ql->end=ql->end->NEXT;
   }
  *(float *)&new->DATA=value;
  ql->nb++;
}

static int cmpfloat(const void *a0, const void *b0)
{
  float *a=(float *)a0, *b=(float *)b0;
  if (*a<*b) return -1;
  if (*a>*b) return 1;
  return 0;
}

chain_list *mbk_QuickListComputeMeanVarMedian(quicklist *ql)
{
  double mean=0, var=0, median=0, val;
  float *tab;
  chain_list *cl;
  int i;

  if (ql->nb>0)
  {
   for (cl=ql->head; cl!=NULL; cl=cl->NEXT)
     mean=mean+*(float *)&cl->DATA;
   mean/=ql->nb;
   for (cl=ql->head; cl!=NULL; cl=cl->NEXT)
   {
     val=*(float *)&cl->DATA;
     var=var+(val-mean)*(val-mean);
   }
   var=var/ql->nb;
   tab=(float *)mbkalloc(sizeof(float)*ql->nb);
   for (cl=ql->head, i=0; cl!=NULL; cl=cl->NEXT, i++)
     tab[i]=*(float *)&cl->DATA;

   qsort(tab, ql->nb, sizeof(float), cmpfloat);

   median=tab[ql->nb/2];
   mbkfree(tab);
  }
  cl=addchain(NULL, NULL);
  *(float *)&cl->DATA=median;
  cl=addchain(cl, NULL);
  *(float *)&cl->DATA=var;
  cl=addchain(cl, NULL);
  *(float *)&cl->DATA=mean;
  return cl;
}

void mbk_QuickListDoubleFree(quicklist *ql)
{
  freechain(ql->head);
  mbkfree(ql);
}

