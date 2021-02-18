/*****************************************************************************/
/*                                                                           */
/*                      Chaine de CAO & VLSI   AVERTEC                       */
/*                                                                           */
/*    Produit : SIM Version 1.00                                             */
/*    Fichier : sim_parse_titan_ppr.c                                        */
/*                                                                           */
/*    (c) copyright 2000 AVERTEC                                             */
/*    Tous droits reserves                                                   */
/*                                                                           */
/*    Auteur(s) : Grégoire Avot                                              */
/*                                                                           */
/*****************************************************************************/

#include SIM_H
#include "sim.h"
#define SIMBUFSIZE 10240

void sim_parse_eldo_chi(char *filename, char *argv[], int nbx, int nby, double **tab, double limit, double simstep)
{
  FILE    *file ;
  char     buffer[SIMBUFSIZE] ;
  char     prevline[SIMBUFSIZE] ;
  char     titleline[SIMBUFSIZE] ;
  char *ret;
  int i, lasty, y, debug;
  double *cur_data, tmval;
  char cur_name[1024], tm[1024], val[1024];
  char      *found;
  int l;

  file = fopen( filename, "r" );
  if( !file ) {
    avt_errmsg( SIM_ERRMSG, "001", AVT_FATAL, filename);
    //fprintf( stderr, "\nSIM error : can't open file %s\n", filename );
    EXIT(1);
  }
 
  found = avt_gethashvar ("SIM_DEBUG_MODE");
  if (found != NULL && !strcasecmp (found,"yes"))
    debug = 1;
  else
    debug = 0;
  

  found=mbkalloc(nbx);
  for (i=0; i<nbx; i++) found[i]=0;

  cur_data=mbkalloc(sizeof(double)*nby);
  strcpy(prevline,"");

  while (fgets( buffer, SIMBUFSIZE, file )!=NULL)
    {
      if (strcmp(buffer,"X \n")==0)
        {
          for (i=0; i<nby; i++) cur_data[i]=0;
          lasty=0;

          // recherche du nom
          strcpy( titleline, prevline );
          ret=strstr(prevline, "("); 

          if (ret!=NULL ) {
            if( sscanf(ret, "(%s", tm)==1) {
              strcpy(cur_name,tm);
              if ((ret=strrchr(cur_name,')'))!=NULL) *ret='\0';
            }
          }

          // contruction du tableau de valeurs
          while ((ret=fgets( buffer, SIMBUFSIZE, file ))!=NULL && strcmp(buffer,"Y\n")!=0)
            {
              if (sscanf(buffer
, "%s %s", tm, val)==2)
                {
                  tmval=atof(tm);
                  y=(int)((tmval/simstep)+0.5);
                  while (lasty<=y && lasty<nby)
                    cur_data[lasty++]=atof(val);
                }
            }
          if (lasty>0)
            {
              while (lasty<nby)
                cur_data[lasty]=cur_data[lasty-1];
            }
          // recherche d'un eventuel nom a la fin du tableau
          if (ret!=NULL && fgets( buffer, SIMBUFSIZE, file )!=NULL)
            {
              if (sscanf(buffer, "1: V(%s", tm)==1) {
                strcpy(cur_name,tm);
                if ((ret=strrchr(cur_name,')'))!=NULL) *ret='\0';
              }
              if (sscanf(buffer, "1: I(%s", tm)==1) {
                strcpy(cur_name,tm);
                if ((ret=strrchr(cur_name,')'))!=NULL) *ret='\0';
              }
              if (sscanf(buffer, "1: %s", tm)==1) strcpy(cur_name,tm);
            }

          // recherche de l'entree de la variable
          for (i=0; i<nbx && strcasecmp(argv[i], cur_name)!=0; i++) ;
          if (i<nbx)
            {
              found[i]=1;
              memcpy(tab[i], cur_data, nby*sizeof(double));
            }
          else {
            l = strlen( titleline );
            for( i=0 ; i<l ; i++ ) 
              titleline[i] = tolower(titleline[i]);
            for (i=0; i<nbx ; i++ ) {
              strcpy( buffer, argv[i] );
              l=strlen(buffer);
              for(y=0;y<l;y++)
                buffer[y] = tolower(buffer[y]);
              if( strstr( titleline, buffer ) )
                break ;
            }
            if( i<nbx ) {
              found[i]=1;
              memcpy(tab[i], cur_data, nby*sizeof(double));
            }
          }
        }
      strcpy(prevline, buffer);
    }

  if (debug)
    {
      char buf[10];
      for (i=0; i<nbx; i++) printf("%10s ", argv[i]);
      printf("\n");
      for (y=0;y<nby;y++)
        {
          for (i=0; i<nbx; i++)
            {
              if (found[i]==0) printf("%8s   ","/");
              else { sprintf(buf, "%2.4f", tab[i][y]); printf("%8s   ", buf);}
            }
          printf("\n");
        }
    }

  for (i=0, y=0; i<nbx; i++)
    {
      if (found[i]==1) y++;
      else if (debug) fprintf( stderr, "SIM error : labels '%s' not found\n", argv[i]);
    }

  if (y!=nbx)
    {
      fprintf( stderr, 
               "\nSIM error : %d labels found vs %d labels requested.\n",
               y,
               nbx
               );
      EXIT(1);
    }

  mbkfree(found);
  mbkfree(cur_data);

  sim_fclose (file,filename);

  printf( "file parsed\n" );   
}

