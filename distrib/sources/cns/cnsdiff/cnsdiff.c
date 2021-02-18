/****************************************************************************/
/*                                                                          */
/*                          AVERTEC S.A. 2002                               */
/*                                                                          */
/*    Produit : cnsdiff                                                     */
/*    Fichier : cnsdiff.c                                                   */
/*    Auteur(s) : Romaric Thevenent                                         */
/*                                                                          */
/****************************************************************************/

#include CNSDIFF_H

char            *NAMEFIG1 = NULL ;
char            *NAMEFIG2 = NULL ;
char            *FILEOUT  = NULL ;
FILE            *file ;

/****************************************************************************/
/*                       function cnsdiff_rename()                          */
/*                                                                          */
/* renomme les fichiers et les cnsfigs.                                     */
/****************************************************************************/
void cnsdiff_rename(ptcnsfig,extension)
  cnsfig_list  *ptcnsfig;
  char         *extension;
{
  char         *newname;
  
  strcpy(newname,ptcnsfig->NAME);
  strcat(newname,"_");
  strcat(newname,extension);

  ptcnsfig->NAME = namealloc(newname);
}

/****************************************************************************/
/*                       function cnsdiff_error()                           */
/*                                                                          */
/* affiche le menu d'aide                                                   */
/****************************************************************************/
void cnsdiff_error(error_code,name)
long   error_code;
char   *name;
{
  if ( error_code != CNS_ERROR_HELP)
    (void)fprintf(stderr, "*** cnsdiff error ***\n"); 
  
  if ( error_code == CNS_ERROR_USAGE || error_code == CNS_ERROR_HELP ) {
    (void)fprintf(stdout,"\n");
    (void)fprintf(stdout,"usage : cnsdiff filename1 filename2 [-v -nf]\n");
    (void)fprintf(stdout,"filename1       : file .cns to be compared without extention\n");
    (void)fprintf(stdout,"filename1       : file .cns compared without extention\n");
    (void)fprintf(stdout,"-v              : verbose mode\n");
    (void)fprintf(stdout,"-nf             : no output file \"reportfile.cnsdiff\"\n");
  }
  else if ( error_code == CNS_ERROR_NOFILE) {
    (void)fprintf(stderr,"File is missing. See Usage. \n");  
    cnsdiff_error(CNS_ERROR_HELP,NULL);
  }
  else if ( error_code == CNS_ERROR_CNSFIG) {
    (void)fprintf(stderr,"Missing or invalide CnsFigure from %s \n",name);
  }
  else if ( error_code == CNS_ERROR_OUTPUT) {
    (void)fprintf(stderr, "Invalid output direction \n");
    }

  EXIT(0) ;
} // cnsdiff_error


/****************************************************************************/
/*                        function cnsdiff_getopt()                            */
/*                                                                          */
/* renvoie le masque correspondant aux options                              */
/****************************************************************************/
long cnsdiff_getopt(argc,argv)
int argc;
char *argv[];
{
  int      i ;
  long     maskopt = 0;
  
  for(i = 1 ; i != argc ; i++) {
     if (argv[i][0] != '-') {
       if (NAMEFIG1 == NULL)
         NAMEFIG1 = namealloc(argv[i]) ;
       else 
         NAMEFIG2 = namealloc(argv[i]) ;
     } // if (argv[i] ...)
     else {
       if(strcmp(argv[i],"-h") == 0) {
         maskopt |= DIFF_OPT_HELP ;
       } // if ( -h )
        else if (strcmp(argv[i],"-v") == 0) {
         maskopt |= DIFF_OPT_VERBOSE ;
       } // else if ( -v )
       else if (strcmp(argv[i],"-nf") == 0) {
         maskopt |= DIFF_OPT_NOOUT ;
       } // else if ( -nf )
       else if (strcmp(argv[i],"-error=first") == 0) {
         maskopt |= DIFF_OPT_FIRST ;
       } // else if ( -error=first )
       else if (strcmp(argv[i],"-error=all") == 0) {
         maskopt |= DIFF_OPT_ALL ;
       } // else if ( -error=all )
       else if (strcmp(argv[i],"-out=") == 0) {
         maskopt |= DIFF_OPT_OUTFILE ;
       } // else if ( )
                       
     } // else (argv[i] ...)
  } // for (argc ..)
    
  return maskopt;
} // cnsdiff_getopt

/****************************************************************************/
/*                        function main()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int main(argc,argv)
  int argc;
  char *argv[];
{
  int          countingerror = 0;
  long         maskopt = 0;
  char         *FILEOUTNAME = "reportfile";
  cnsfig_list  *cnsfig1 ;
  cnsfig_list  *cnsfig2 ;

#ifdef AVERTEC
  avtenv() ;
#endif

  mbkenv();               /* MBK environment */
  cnsenv();               /* CNS environment */

  if (((maskopt = cnsdiff_getopt(argc,argv))&DIFF_OPT_HELP)==DIFF_OPT_HELP)
  {
    cnsdiff_error(CNS_ERROR_HELP,NULL);
    EXIT(0);
  }

#ifdef AVERTEC_LICENSE
  if(avt_givetoken("AVT_LICENSE_SERVER","cnsdiff") != AVT_VALID_TOKEN )
    EXIT(1) ;
#endif
  
  if ((maskopt&(DIFF_OPT_TEST))==0)
  {
    avt_banner("CnsDiff","cns file compare","2002");
  }

  if((NAMEFIG1 == NULL) || (NAMEFIG2 == NULL)) 
    cnsdiff_error(CNS_ERROR_NOFILE,NULL) ;
  
  if((cnsfig1 = getcnsfig(NAMEFIG1, NULL)) == NULL)
    cnsdiff_error(CNS_ERROR_CNSFIG,NAMEFIG1);

  if((cnsfig2 = loadcnsfig(NAMEFIG2, NULL)) == NULL)
    cnsdiff_error(CNS_ERROR_CNSFIG,NAMEFIG2);
  
  file = mbkfopen(FILEOUTNAME,"cnsdiff",WRITE_TEXT) ;
  
  if(file == NULL)
    cnsdiff_error(CNS_ERROR_OUTPUT,NULL);

  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT){
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                  CNSDIFF VALIDATION REPORT               #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"          FIRST  FILE NAME : %s\n",NAMEFIG1) ;
    (void)fprintf(file,"          ------ FIRST  CNSFIG NAME : %s\n",cnsfig1->NAME) ; 
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"          SECOND FILE NAME : %s\n",NAMEFIG2) ; 
    (void)fprintf(file,"          ------ SECOND CNSFIG NAME : %s\n",cnsfig2->NAME) ; 
    (void)fprintf(file,"\n") ; 
  }
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE){
    (void)fprintf(file,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                  CNSDIFF VALIDATION REPORT               #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"          FIRST  FILE NAME : %s\n",NAMEFIG1) ;
    (void)fprintf(stdout,"          ------ FIRST  CNSFIG NAME : %s\n",cnsfig1->NAME) ; 
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"          SECOND FILE NAME : %s\n",NAMEFIG2) ; 
    (void)fprintf(stdout,"          ------ SECOND CNSFIG NAME : %s\n",cnsfig2->NAME) ; 
    (void)fprintf(stdout,"\n") ;
  }
  
  #ifdef AVERTEC_LICENSE
    if(avt_givetoken("AVT_LICENSE_SERVER","cnsdiff") != AVT_VALID_TOKEN )
      EXIT(1) ;
  #endif
  countingerror = cnsdiff_makefigdiff(file,cnsfig1,cnsfig2,maskopt);
  
  fflush(file) ;
  freecnsfig(cnsfig1);
  freecnsfig(cnsfig2);
    
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"*** number of error : %d \n",countingerror);
    if ( countingerror == 0) 
      (void)fprintf(file,"*** VALIDATION SUCCESFULL\n");
    else 
      (void)fprintf(file,"*** VALIDATION FAILED\n");
  }
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"*** number of error : %d \n",countingerror);
    if ( countingerror == 0) 
      (void)fprintf(stdout,"*** VALIDATION SUCCESFULL\n");
    else 
      (void)fprintf(stdout,"*** VALIDATION FAILED\n");
  }
 
  EXIT(countingerror) ; 
  return countingerror;
} // main
