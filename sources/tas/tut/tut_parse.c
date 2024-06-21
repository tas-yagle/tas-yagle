/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_parse.c                                                 */
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

#include TUT_H
#include "tut_parse.h"
/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

ht  *tut_tablasharea;

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern int   libareaparse() ;
extern FILE *libareain ;

extern int   tlfareaparse() ;
extern FILE *tlfareain ;

extern int   txtareaparse() ;
extern FILE *txtareain ;


/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

void tut_parse (char *filename, int version)
{
    if(version == 1){
        if ((libareain = mbkfopen (filename, NULL, "r"))) {
            tut_tablasharea=addht(1000); 
        	libareaparse () ;
        	fclose (libareain) ; 
        }
    }else if(version == 2){
        if ((tlfareain = mbkfopen (filename, NULL, "r"))) {
            tut_tablasharea=addht(1000); 
        	tlfareaparse () ;
        	fclose (tlfareain) ; 
        }
    }else if(version == 3){
        if ((txtareain = mbkfopen (filename, NULL, "r"))) {
            tut_tablasharea=addht(1000); 
        	txtareaparse () ;
        	fclose (txtareain) ; 
        }
    }    
}

/****************************************************************************/
/*     function  lib_unsuffix                                               */
/****************************************************************************/
char *lib_unsuffix (char *name, char* suffix)
{
	char buf[1024], *c ;
	int  ln, ls ;
	
    strcpy(buf, name);
    if ((c=strchr(buf,'\''))!=NULL)
    {
      *c='\0';
      return namealloc(buf);
    }
    if (!suffix || (c=strstr(buf, suffix))==NULL)
      return name ;

    *c='\0';
	return namealloc (buf) ;
}

