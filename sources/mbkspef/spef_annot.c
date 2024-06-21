/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spef_annot.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include AVT_H
#include "spef_annot.h"
#include "spef_util.h"
#include "spef_actions.h"

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

lofig_list *Lofig ;
int         Line ;
int         Mode ;
char        SPEF_PRELOAD = 'N';
int         SPEF_LOFIG_CACHE = 0, SPEF_IN_CACHE=0;
int         nbloadelem=0;
int         pool = -1;
int SPEF_PRESERVE_RC, SPEF_CARDS;
/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern void  spefrestart();
extern int   spefparse() ;
extern FILE *spefin ;
FILE        *file;
extern void  spef_dnetrestart();
extern int   spef_dnetparse() ;
extern FILE *spef_dnetin ;
extern chain_list *ctclist;
static char vierge = 1 ;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/
void spef_setlofigfile( lofig_list *lofig, FILE *file )
{
  ptype_list    *ptl;
  ptl = getptype( lofig->USER, SPEF_FILE );
  if( ptl )
      ptl->DATA = file;
  else
      lofig->USER = addptype( lofig->USER, SPEF_FILE, file );
}

FILE *spef_getlofigfile( lofig_list *lofig )
{
  ptype_list    *ptl;
  ptl = getptype( lofig->USER, SPEF_FILE );
  if( ptl )
      return (FILE*) ptl->DATA;
  return NULL;
}
void spef_Indexation(FILE *file, lofig_list *lofig)
{
  char buf[1024];
  int size = 1024;
  MBK_OFFSET_MAX index;
  char *bufindex, *name;
  losig_list *ptsig;

  spef_setlofigfile( lofig, file );

  for(;;){
    if(!mbkftell( file, &index )) {
      avt_errmsg(SPE_ERRMSG, "005", AVT_FATAL);
/*  
      fflush( stdout );
      fprintf( stderr, "spef error : can't get current file position.\n" );*/
      EXIT(1);
    }
    if(fgets(buf,size,file) == NULL){
        break;
    }else{
        if(strncasecmp(buf, "*D_NET", 6)==0){
            bufindex = strrchr(buf+1,'*');
            if(bufindex && namemaptable){
                bufindex++;
                *strchr(bufindex,' ') = '\0';
                name = (char*)getititem(namemaptable, atoi(bufindex));
            }else{
                bufindex = strchr(buf,' ');
                if (bufindex!=NULL)
                {
                  *bufindex= '\0';
                  bufindex++;
                }
                name = namealloc(bufindex);              
            }
            if ((ptsig = spef_getlosigbyname(name))!=NULL)
              mbk_losig_setfilepos( lofig, ptsig, file, &index, SPEF_INDEX, SPEF_HEAP );
        }
    }
  }
}
/*****************************************************************/
void spef_cache_free( lofig_list *lofig )
{
    spef_remove_htctc();
    if(portmap_ht)
        delht(portmap_ht);
    if(namemaptable)
        delit(namemaptable);
    spef_destroy_loins_htable();
    spef_destroy_losig_htable();
    fclose(spef_getlofigfile( lofig ));
    SPEF_LOFIG_CACHE = 0;
}
/*****************************************************************/
unsigned long int spef_cache_parse( lofig_list *lofig, losig_list *losig )
{
    MBK_OFFSET_MAX offset;
    static char vierge = 1 ;
    char buf[1024];
    nbloadelem=0;

    SPEF_IN_CACHE=1;

    SPEF_PRELOAD = 'N';
    sprintf(buf,"%s.%s", lofig->NAME, IN_PARASITICS);
	Lofig = lofig;
    Line = 1;
    spef_dnetin = spef_getlofigfile( lofig );
    spef_ParsedFile=lofig->NAME;
    if(vierge == 0)
        spef_dnetrestart(spef_dnetin) ;
    vierge = 0 ;
    if(mbk_losig_getfilepos( lofig, losig, spef_dnetin, &offset, SPEF_INDEX )){
        if( !mbkfseek(spef_dnetin, &offset, SEEK_SET)) {
          avt_errmsg(SPE_ERRMSG, "006", AVT_FATAL);      
/*          fflush( stdout );
         fprintf( stderr, "spef error : can't set file position.\n" );*/
          EXIT(1);
        }
        spef_dnetparse();
    }

    SPEF_IN_CACHE=0;

    return nbloadelem;
}
static void spef_checkenv()
{
  char *env;
  int SPEF_KEEPM, SPEF_KEEPX;
  mbk_options_pack_struct spf_device_opack[] = {
        {"Transistor", KEEP__TRANSISTOR},
        {"Resistance", KEEP__RESISTANCE},
        {"Instance", KEEP__INSTANCE},
        {"Capacitance", KEEP__CAPA},
        {"Diode", KEEP__DIODE},
        {"None", KEEP__NONE},
        {"All", KEEP__ALL}
  };

  SPEF_PRESERVE_RC=0;

      SPEF_KEEPM=V_BOOL_TAB[__ANNOT_KEEP_M].VALUE;
      SPEF_KEEPX=V_BOOL_TAB[__ANNOT_KEEP_X].VALUE;
      SPEF_PRESERVE_RC=V_BOOL_TAB[__DSPF_PRESERVE_RC].VALUE;

  SPEF_CARDS =
       mbk_ReadFlags (__ANNOT_KEEP_CARDS, spf_device_opack,
                      sizeof (spf_device_opack) / sizeof (*spf_device_opack), 1, 0);
   
  if (SPEF_CARDS!=0)
  {
    if (SPEF_CARDS & KEEP__NONE)
      SPEF_CARDS = 0;

    if (SPEF_CARDS & KEEP__TRANSISTOR) SPEF_KEEPM=1; else SPEF_KEEPM=0;
    if (SPEF_CARDS & KEEP__INSTANCE) SPEF_KEEPX=1; else SPEF_KEEPX=0;
  }
  else
  {
     if (SPEF_KEEPM) SPEF_CARDS|=KEEP__TRANSISTOR;
     if (SPEF_KEEPX) SPEF_CARDS|=KEEP__INSTANCE;
  }

}

void spef_Annotate(ptlofig)
     lofig_list *ptlofig;
{
    MBK_OFFSET_MAX debut;
    char *fp;
    char buf[1024];
    
    spef_groundlosig = NULL;
    SPEF_PRELOAD = 'N';

    spef_checkenv();

    if(pool == -1)
	  pool = rcn_getnewpoolcache();

    if( SPEF_LOFIG_CACHE == 1 )
      {
        avt_errmsg(SPE_ERRMSG, "007", AVT_WARNING, ptlofig->NAME);      
//        printf("only one spef cache for now\n'%s' have no cache\n",ptlofig->NAME);
        return;
      }
    if ((spefin = mbkfopen(ptlofig->NAME, IN_PARASITICS, READ_TEXT))!=NULL){
        if( MBKFOPEN_FILTER==NO ){
            if( !mbkftell( spefin, &debut ) ) {
                avt_errmsg(SPE_ERRMSG, "005", AVT_ERROR);      
/*                fflush( stdout );
                fprintf( stderr, "spef error : can't get current file position.\n" );*/
                EXIT(1);
            }
        }
        Lofig = ptlofig;
        Line = 1;
        if ((fp=filepath(ptlofig->NAME, IN_PARASITICS))!=NULL)        
          strcpy(buf,fp);
        else
          strcpy(buf,ptlofig->NAME);

        spef_ParsedFile=buf;
			
        if(vierge == 0)
            spefrestart(spefin) ;
        vierge = 0 ;
        spefparse();

        if (!spef_error())
          {
            //        spef_ht_stats();
            
            if( SPEF_PRELOAD == 'N' ) {
              spef_destroy_loins_htable();
              spef_destroy_losig_htable();
            }
            
            if( SPEF_PRELOAD == 'Y' ) {
              if( !mbkfseek( spefin, &debut, SEEK_SET ) ) {
                avt_errmsg(SPE_ERRMSG, "006", AVT_ERROR);      
/*                fflush( stdout );
                fprintf( stderr, "spef error : can't set file position.\n" );*/
                EXIT(1);
              }
              spef_Indexation( spefin, ptlofig );
            }
          }
        if( SPEF_PRELOAD == 'N' ) 
          fclose(spefin);
        mbk_commit_errors(spef_ParsedFile);
    }
}

void parsespef(char *filename)
{
  char buf[1024]; 
  char *fp;
  MBK_OFFSET_MAX debut;

  spef_groundlosig = NULL;
  SPEF_PRELOAD = 'N';

  spef_checkenv();

  if(pool == -1)
	  pool = rcn_getnewpoolcache();

  if( SPEF_LOFIG_CACHE == 1 )
   {
     avt_errmsg(SPE_ERRMSG, "007", AVT_WARNING, filename);      
     //printf("only one spef cache for now\n'%s' have no cache\n",filename);
     return;
   }

  if ((spefin = mbkfopen(filename, NULL, READ_TEXT))!=NULL){
    if( MBKFOPEN_FILTER==NO ){
       if( !mbkftell( spefin, &debut ) ) {
         avt_errmsg(SPE_ERRMSG, "005", AVT_ERROR);      
         /*fflush( stdout );
         fprintf( stderr, "spef error : can't get current file position.\n" );*/
         EXIT(1);
       }
    }

    Lofig = NULL;
    Line = 1;
 
    if ((fp=filepath(filename, NULL))!=NULL)        
      strcpy(buf,filepath(filename, NULL));
    else
      strcpy(buf,filename);
 
    spef_ParsedFile=buf;
  	
    if(vierge == 0)
        spefrestart(spefin) ;
    vierge = 0 ;
    spefparse();
 
    if (!spef_error())
      {
        if( SPEF_PRELOAD == 'N' ) {
          spef_destroy_loins_htable();
          spef_destroy_losig_htable();
        }
        if( SPEF_PRELOAD == 'Y' ) {
          if( !mbkfseek( spefin, &debut, SEEK_SET ) ) {
            avt_errmsg(SPE_ERRMSG, "006", AVT_ERROR);      
/*            fflush( stdout );
            fprintf( stderr, "spef error : can't set file position.\n" );*/
            EXIT(1);
          }
          spef_Indexation( spefin, Lofig );
        }
      }
    if( SPEF_PRELOAD == 'N' ) 
      fclose(spefin);
    mbk_commit_errors(spef_ParsedFile);
  }
  else
  {
    avt_errmsg(SPE_ERRMSG, "008", AVT_FATAL, filename);      
/*    fflush( stdout );
    fprintf( stderr, "spef error : can't open file '%s'\n", filename );
*/
    EXIT(1);
  }
}
