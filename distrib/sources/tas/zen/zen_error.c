/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : zen                                                         */
/*    Fichier : zen_error.c                                                 */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Stephane Picault                                        */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include ZEN_H

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_error(Error,String)
int          Error;
char        *String;
{
  fprintf(stderr,"[ZEN_ERR]:");
  switch (Error)
  {
    case 0 : fprintf(stderr,"%s",String)                               ;break;
    case 1 : fprintf(stderr,"must have a file with a description")     ;break;
    case 2 : fprintf(stderr,"must have a path file")                   ;break;
    case 3 : fprintf(stderr,"not enough arguments")                    ;break;
    default: fprintf(stderr,"Unknown Error");   
  }
  fprintf(stderr,"\n");
  EXIT(0);
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_warning(Warning,String)
int     Warning;
char   *String;
{
  fprintf(stderr,"[ZEN_WAR]:");
  
  switch (Warning)
  {
    case 0 : fprintf(stderr,"%s",String)                               ;break;
    case 1 : fprintf(stderr,"Path File %s don't load ... ",String)     ;break;
    case 2 : fprintf(stderr,"-%s- unknown loading -file.inf-",String)  ;break;
    default: fprintf(stderr,"Unknown");
  }
  fprintf(stderr,"\n");
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_trace(Trace,String,File,Line)
int     Trace;
char   *String;
char   *File;
long    Line;
{
  fprintf(stdout,"[Zen_Tra]: %s:%ld: ",File,Line);
  
  switch (Trace)
  {
    case 0 : fprintf(stdout,"%s",String)                               ;break;
    case 1 : fprintf(stdout,"loading path file : -%s-",String)         ;break;
    default: fprintf(stdout,"Unknown");
  }  
  fprintf(stdout,"\n");
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_test(test,string,file,line)
int     test;
char   *string;
char   *file;
long    line;
{
  fprintf(stdout,"[Zen_Tst]: %s:%ld: ",file,line);

  switch (test)
  {
    case 0 : fprintf(stdout,"%s",string)                               ;break;
    default: fprintf(stdout,"Unknown");

  }
  fprintf(stdout,"\n");
}
