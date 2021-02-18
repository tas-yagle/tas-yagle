/****************************************************************************/
/*                                                                          */
/*                          AVERTEC S.A. 2002                               */
/*                                                                          */
/*    Produit : cnsdiff                                                     */
/*    Fichier : cnsdiff_util.c                                              */
/*    Auteur(s) : Romaric Thevenent                                         */
/*                                                                          */
/****************************************************************************/

#include CNSDIFF_H

/****************************************************************************/
/*                    function printlocondiffheader()                       */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison des LOCON                         */
/****************************************************************************/

void printlocondiffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#           COMPARAISON DES CONNECTEURS EXTERNES           #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#           COMPARAISON DES CONNECTEURS EXTERNES           #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                  function cnsdiff_cnsfiglocon_diff()                     */
/*                                                                          */
/* comparaison des LOCON de deux figures CNS                                */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int cnsdiff_cnsfiglocon_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  locon_list   *chain;
  locon_list   *ptlocon;
  cnsfig_list  *minsize;
  cnsfig_list  *maxsize;
  ht           *htmin;
  ht           *htmax;
  int          index1=0;
  int          index2=0;
  int          deltaindex=0;
  int          count=0;

  
  printlocondiffheader(file,maskopt);
  
  htmin=addht(128);
  htmax=addht(128);
  
  for(chain=cnsfig1->LOCON;chain;chain=chain->NEXT) {
          index1++;
  }
  for(chain=cnsfig2->LOCON;chain;chain=chain->NEXT) {
          index2++;
  }
  
  if (index2 == index1) {
    minsize= cnsfig1;
    maxsize = cnsfig2;
  } // if (index2 == index1)
  else {
    if ( index1 > index2) {
      minsize= cnsfig2;
      maxsize = cnsfig1;
      deltaindex = index1 - index2;
    } // if ( index1 > index2)
    else {
      minsize= cnsfig1;
      maxsize = cnsfig2;
      deltaindex = index2 - index1;
    } // else 
    
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de connecteurs differents (%d). entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de connecteurs differents (%d). entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  } // else (index2 != index1)
  
  for (chain=minsize->LOCON;chain;chain=chain->NEXT) {
    if (gethtitem(htmin,chain->NAME)==EMPTYHT)
          addhtitem(htmin,chain->NAME,(long)chain);
  }
  
  for (chain=maxsize->LOCON;chain;chain=chain->NEXT) {
    if (gethtitem(htmax,chain->NAME)==EMPTYHT)
          addhtitem(htmax,chain->NAME,(long)chain);

    if (((long)ptlocon=gethtitem(htmin,chain->NAME))==EMPTYHT) {
      count++;
      if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
        (void)fprintf(file,"*** ERROR: Connecteur %s de %s manquant dans %s \n",
                     chain->NAME,maxsize->NAME,minsize->NAME) ; 
      }  // if (maskopt&DIFF_OPT_NOOUT)
      if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
        (void)fprintf(stdout,"*** ERROR: Connecteur %s de %s manquant dans %s \n",
                     chain->NAME,maxsize->NAME,minsize->NAME) ;  
       } // if (maskopt&DIFF_OPT_VERBOSE)
    }
    else {
      if (chain->SIG != NULL && ptlocon->SIG != NULL) { 
        if (chain->SIG->INDEX != ptlocon->SIG->INDEX) {
          count++;
          if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
            (void)fprintf(file,"*** ERROR: Connecteur %s - champs SIG different\n",chain->NAME) ; 
          if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
            (void)fprintf(stdout,"*** ERROR: Connecteur %s - champs SIG different\n",chain->NAME) ;  
        } // test de LOCON->SIG
      }

/*      
      if (chain->ROOT != ptlocon->ROOT) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Connecteur %s - champs ROOT different\n",chain->NAME) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: Connecteur %s - champs ROOT different\n",chain->NAME) ;  
      } // test de LOCON->ROOT
*/

      if (chain->DIRECTION != ptlocon->DIRECTION) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Connecteur %s - champs DIRECTION different\n",chain->NAME) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: Connecteur %s - champs DIRECTION different\n",chain->NAME) ;  
      } // test de LOCON->DIRECTION
      
      if (chain->TYPE != ptlocon->TYPE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Connecteur %s - champs TYPE different\n",chain->NAME) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: Connecteur %s - champs TYPE different\n",chain->NAME) ;  
      } // test de LOCON->TYPE
      
/*
      if (chain->USER != ptlocon->USER) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Connecteur %s - champs USER different\n",chain->NAME) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: Connecteur %s - champs USER different\n",chain->NAME) ;  
      } // test de LOCON->USER
*/

    } // fin des tests
  }
  
  for (chain=minsize->LOCON;chain;chain=chain->NEXT) {
    if (gethtitem(htmax,chain->NAME)==EMPTYHT) {
      count++;
      if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
        (void)fprintf(file,"*** ERROR: Connecteur %s de %s manquant dans %s \n",
                     chain->NAME,minsize->NAME,maxsize->NAME) ; 
      }  // if (maskopt&DIFF_OPT_NOOUT)
      if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
        (void)fprintf(stdout,"*** ERROR: Connecteur %s de %s manquant dans %s \n",
                     chain->NAME,minsize->NAME,maxsize->NAME) ;  
       } // if (maskopt&DIFF_OPT_VERBOSE)
    } // pas de else car les comparaisons ont deja ete faites
  }

  delht(htmin);
  delht(htmax);
  
  return count;
}

/****************************************************************************/
/*                    function printlotrsdiffheader()                       */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison des LOTRS                         */
/****************************************************************************/

void printlotrsdiffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                COMPARAISON DES TRANSISTORS               #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                COMPARAISON DES TRANSISTORS               #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                  function cnsdiff_cnsfiglotrs_diff()                     */
/*                                                                          */
/* comparaison des LOTRS de deux figures CNS                                */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int cnsdiff_cnsfiglotrs_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  
  lotrs_list   *chain;
  lotrs_list   *ptlotrs;
  cnsfig_list  *minsize;
  cnsfig_list  *maxsize;
  ht           *htmin;
  ht           *htmax;
  int          index1=0;
  int          index2=0;
  int          deltaindex=0;
  int          count=0;
  ptype_list   *ptype ;
  int          lotrsindex=0 ;
  
  printlotrsdiffheader(file,maskopt);
  
  htmin=addht(128);
  htmax=addht(128);
  
  for(chain=cnsfig1->LOTRS;chain;chain=chain->NEXT) {
          index1++;
  }
  for(chain=cnsfig2->LOTRS;chain;chain=chain->NEXT) {
          index2++;
  }
  
  if (index2 == index1) {
    minsize= cnsfig1;
    maxsize = cnsfig2;
  } // if (index2 == index1)
  else {
    if ( index1 > index2) {
      minsize= cnsfig2;
      maxsize = cnsfig1;
      deltaindex = index1 - index2;
    } // if ( index1 > index2)
    else {
      minsize= cnsfig1;
      maxsize = cnsfig2;
      deltaindex = index2 - index1;
    } // else 
    
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de transistors differents (%d) entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de transistors differents (%d) entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  } // else (index2 != index1)
  
  for (chain=minsize->LOTRS;chain;chain=chain->NEXT) {
    ptype =  getptype(chain->USER,CNS_INDEX);
    lotrsindex = *(int *)ptype->DATA;
    if (gethtitem(htmin,(void *)lotrsindex)==EMPTYHT)
          addhtitem(htmin,(void *)lotrsindex,(long)chain);
  }
        
  for (chain=maxsize->LOTRS;chain;chain=chain->NEXT) {
    ptype =  getptype(chain->USER,CNS_INDEX);
    lotrsindex = *(int *)ptype->DATA;
   if (gethtitem(htmax,(void *)lotrsindex)==EMPTYHT)
          addhtitem(htmax,(void *)lotrsindex,(long)chain);

    if (((long)ptlotrs=gethtitem(htmin,(void *)lotrsindex))==EMPTYHT) {
      count++;
      if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
        (void)fprintf(file,"*** ERROR: transistors %d non trouvé\n",
                     lotrsindex) ; 
      }  // if (maskopt&DIFF_OPT_NOOUT)
      if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
        (void)fprintf(stdout,"*** ERROR:transistors %d non trouvé\n",
                     lotrsindex) ; 
      } // if (maskopt&DIFF_OPT_VERBOSE)
    }
    else { 
    
      if (chain->DRAIN != ptlotrs->DRAIN) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs DRAIN different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs DRAIN different\n",(long)chain) ;  
      } // test de LOTRS->DRAIN
      
      if (chain->GRID != ptlotrs->GRID) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs GRID different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs GRID different\n",(long)chain) ;  
      } // test de LOTRS->GRID
            
      if (chain->SOURCE != ptlotrs->SOURCE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs SOURCE different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs SOURCE different\n",(long)chain) ;  
      } // test de LOTRS->SOURCE
      
      if (chain->X != ptlotrs->X) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs X different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs X different\n",(long)chain) ;  
      } // test de LOTRS->X
            
      if (chain->Y != ptlotrs->Y) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs Y different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs Y different\n",(long)chain) ;  
      } // test de LOTRS->Y
    
      if (chain->WIDTH != ptlotrs->WIDTH) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs WIDTH different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs WIDTH different\n",(long)chain) ;  
      } // test de LOTRS->WIDTH
            
      if (chain->LENGTH != ptlotrs->LENGTH) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs LENGTH different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs LENGTH different\n",(long)chain) ;  
      } // test de LOTRS->LENGTH
     
      if (chain->PS != ptlotrs->PS) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs PS different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs PS different\n",(long)chain) ;  
      } // test de LOTRS->PS
            
      if (chain->PD != ptlotrs->PD) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs PD different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs PD different\n",(long)chain) ;  
      } // test de LOTRS->PD
     
      if (chain->XS != ptlotrs->XS) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs XS different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs XS different\n",(long)chain) ;  
      } // test de LOTRS->XS
            
      if (chain->XD != ptlotrs->XD) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs XD different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs XD different\n",(long)chain) ;  
      } // test de LOTRS->XD
            
      if (chain->TYPE != ptlotrs->TYPE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs TYPE different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs TYPE different\n",(long)chain) ;  
      } // test de LOTRS->TYPE
            
/*
      if (chain->USER != ptlotrs->USER) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)
          (void)fprintf(file,"*** ERROR: Transistor (pt:%ld) - champs USER different\n",(long)chain) ; 
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
          (void)fprintf(stdout,"*** ERROR: transistor (pt:%ld) - champs USER different\n",(long)chain) ;  
      } // test de LOTRS->USER
*/

    }// fin des tests
  }

  for (chain=minsize->LOTRS;chain;chain=chain->NEXT) {
    if (gethtitem(htmax,chain)==EMPTYHT)
      count++;
  }
   
  delht(htmin);
  delht(htmax);
  
  return count;      
}

/****************************************************************************/
/*                    function printconediffheader()                        */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison des CONE                          */
/****************************************************************************/

void printconediffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                   COMPARAISON DES CONES                  #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                   COMPARAISON DES CONES                  #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                     function loc_coneloop_incone()                       */
/*                                                                          */
/* comparaison du champ INCONE de CNSFIG->CONE                              */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int loc_coneloop_incone(file,maskopt,chain,ptcone)
  FILE         *file;
  long         maskopt;
  cone_list    *chain;
  cone_list    *ptcone;
 {
  int          count=0;
  edge_list    *incone;
  edge_list    *ptincone;
  ht           *htchain;
  ht           *htptcone;
  int          htchainindex=0;
  int          htptconeindex=0;
  
  htchain=addht(128);
  htptcone=addht(128);

   for (incone=chain->INCONE;incone;incone=incone->NEXT) {
    if (gethtitem(htchain,incone->UEDGE.CONE->NAME)==EMPTYHT) {
          addhtitem(htchain,incone->UEDGE.CONE->NAME,(long)incone);
          htchainindex++;
    }
  }

  for (incone=ptcone->INCONE;incone;incone=incone->NEXT) {
    if (gethtitem(htptcone,incone->UEDGE.CONE->NAME)==EMPTYHT) {
          addhtitem(htptcone,incone->UEDGE.CONE->NAME,(long)incone);
          htptconeindex++;
    }

    if (((long)ptincone=gethtitem(htptcone,incone->UEDGE.CONE->NAME))==EMPTYHT) {
      count++;
      if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
        (void)fprintf(file,"*** ERROR: Cone %s - INCONE %ld manquant. \n",
                      chain->NAME,(long)incone->UEDGE.CONE->NAME) ; 
      }  // if (maskopt&DIFF_OPT_NOOUT)
      if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
        (void)fprintf(stdout,"*** ERROR: Cone %s - INCONE %ld manquant. \n",
                      chain->NAME,(long)incone->UEDGE.CONE->NAME) ; 
       } // if (maskopt&DIFF_OPT_VERBOSE)
    }
    else {
      if (incone->TYPE != ptincone->TYPE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - INCONE (%ld) de TYPE different. \n",
                        chain->NAME,(long)incone->UEDGE.PTR) ; 
        }  // if (maskopt&DIFF_OPT_NOOUT)
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - INCONE (%ld) TYPE different. \n",
                        chain->NAME,(long)incone->UEDGE.PTR) ; 
         } // if (maskopt&DIFF_OPT_VERBOSE)
      } // test du UEDGE->TYPE
    
    }
    
  }
  
  if (htchainindex > htptconeindex) {
    for (incone=chain->INCONE;incone;incone=incone->NEXT) {
            
      if (((long)ptincone=gethtitem(htchain,incone->UEDGE.CONE->NAME))==EMPTYHT) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - INCONE %ld manquant. \n",
                        chain->NAME,(long)incone->UEDGE.CONE->NAME) ; 
        }  // if (maskopt&DIFF_OPT_NOOUT)
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - INCONE %ld manquant. \n",
                        chain->NAME,(long)incone->UEDGE.CONE->NAME) ; 
         } // if (maskopt&DIFF_OPT_VERBOSE)
      }
      else {
        if (incone->TYPE != ptincone->TYPE) {
          count++;
          if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
            (void)fprintf(file,"*** ERROR: Cone %s - INCONE (%ld) de TYPE different. \n",
                          chain->NAME,(long)incone->UEDGE.CONE->NAME) ; 
          }  // if (maskopt&DIFF_OPT_NOOUT)
          if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
            (void)fprintf(stdout,"*** ERROR: Cone %s - INCONE (%ld) TYPE different. \n",
                          chain->NAME,(long)incone->UEDGE.CONE->NAME) ; 
           } // if (maskopt&DIFF_OPT_VERBOSE)
        } // test du UEDGE->TYPE
      
      }
    }
  } // (htchainindex > htptconeindex)

  delht(htchain);
  delht(htptcone);

  return count;
 }

/****************************************************************************/
/*                     function loc_coneloop_outcone()                      */
/*                                                                          */
/* comparaison du champ OUTCONE de CNSFIG->CONE                             */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int loc_coneloop_outcone(file,maskopt,chain,ptcone)
  FILE         *file;
  long         maskopt;
  cone_list    *chain;
  cone_list    *ptcone;
 {
  int          count=0;
  edge_list    *outcone;
  edge_list    *ptoutcone;
  ht           *htchain;
  ht           *htptcone;
  int          htchainindex=0;
  int          htptconeindex=0;
  
  htchain=addht(128);
  htptcone=addht(128);

   for (outcone=chain->OUTCONE;outcone;outcone=outcone->NEXT) {
    if (gethtitem(htchain,outcone->UEDGE.CONE->NAME)==EMPTYHT) {
          addhtitem(htchain,outcone->UEDGE.CONE->NAME,(long)outcone);
          htchainindex++;
    }
  }

  for (outcone=ptcone->OUTCONE;outcone;outcone=outcone->NEXT) {
    if (gethtitem(htptcone,outcone->UEDGE.CONE->NAME)==EMPTYHT) {
          addhtitem(htptcone,outcone->UEDGE.CONE->NAME,(long)outcone);
          htptconeindex++;
    }

    if (((long)ptoutcone=gethtitem(htptcone,outcone->UEDGE.CONE->NAME))==EMPTYHT) {
      count++;
      if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
        (void)fprintf(file,"*** ERROR: Cone %s - OUTCONE %ld manquant. \n",
                      chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
      }  // if (maskopt&DIFF_OPT_NOOUT)
      if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
        (void)fprintf(stdout,"*** ERROR: Cone %s - OUTCONE %ld manquant. \n",
                      chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
       } // if (maskopt&DIFF_OPT_VERBOSE)
    }
    else {
      if (outcone->TYPE != ptoutcone->TYPE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - OUTCONE (%ld) de TYPE different. \n",
                        chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
        }  // if (maskopt&DIFF_OPT_NOOUT)
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - OUTCONE (%ld) TYPE different. \n",
                        chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
         } // if (maskopt&DIFF_OPT_VERBOSE)
      } // test du UEDGE->TYPE
    
    }
    
  }
  
  if (htchainindex > htptconeindex) {
    for (outcone=chain->OUTCONE;outcone;outcone=outcone->NEXT) {
            
      if (((long)ptoutcone=gethtitem(htchain,outcone->UEDGE.CONE->NAME))==EMPTYHT) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - OUTCONE %ld manquant. \n",
                        chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
        }  // if (maskopt&DIFF_OPT_NOOUT)
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - OUTCONE %ld manquant. \n",
                        chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
         } // if (maskopt&DIFF_OPT_VERBOSE)
      }
      else {
        if (outcone->TYPE != ptoutcone->TYPE) {
          count++;
          if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
            (void)fprintf(file,"*** ERROR: Cone %s - OUTCONE (%ld) de TYPE different. \n",
                          chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
          }  // if (maskopt&DIFF_OPT_NOOUT)
          if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
            (void)fprintf(stdout,"*** ERROR: Cone %s - OUTCONE (%ld) TYPE different. \n",
                          chain->NAME,(long)outcone->UEDGE.CONE->NAME) ; 
           } // if (maskopt&DIFF_OPT_VERBOSE)
        } // test du UEDGE->TYPE
      
      }
    }
  } // (htchainindex > htptconeindex)

  delht(htchain);
  delht(htptcone);

  return count;
 }

/****************************************************************************/
/*                     function loc_countlink_diff()                        */
/*                                                                          */
/* compte le nombre des elements dans une LINK_LISTE                        */
/* si >0 link_liste de chain > link liste de ptcone                         */
/* si <0 link_liste de chain < link liste de ptcone                         */
/* si =0 link_liste de chain = link liste de ptcone                         */
/****************************************************************************/

int loc_countlink_diff(loop_chain,loop_ptcone)
  branch_list   *loop_chain;
  branch_list   *loop_ptcone;
{
  int           indexchain=0;
  int           indexptcone=0;

  while (loop_chain) {
    indexchain ++;
    loop_chain = loop_chain->NEXT;
  }
  while (loop_ptcone) {
    indexptcone ++;
    loop_ptcone= loop_ptcone->NEXT;
  }

  return indexchain - indexptcone ;
  
}

/****************************************************************************/
/*                        function loc_link_diff()                          */
/*                                                                          */
/* comparaison des LINK dans une BRANCH_LIST de CNSFIG->CONE                */
/* retourne le nombre de differences                                        */
/****************************************************************************/
int loc_link_diff(loop_chain,loop_ptcone)
  branch_list  *loop_chain;
  branch_list  *loop_ptcone;
{
  link_list    *link_chain;
  link_list    *link_ptcone;
  cone_list    *cone_chain;
  cone_list    *cone_ptcone;
  ptype_list   *ptype_chain;
  ptype_list   *ptype_ptcone;
  int          count =0;
  int          match =0;
  int          indexptcone=0;
  int          indexchain=0;
  
  for (link_chain=loop_chain->LINK;link_chain;link_chain=link_chain->NEXT)
    indexchain++;
  
  for (link_ptcone=loop_ptcone->LINK;link_ptcone;link_ptcone=link_ptcone->NEXT) 
    indexptcone++;

  if (indexchain != indexptcone)
    return 1;  // nombre de noeuds differents
  
  link_ptcone=loop_ptcone->LINK;
  for (link_chain=loop_chain->LINK;link_chain;link_chain=link_chain->NEXT) {
    if(link_ptcone->TYPE != link_chain->TYPE)
      continue;

    if( (link_ptcone->TYPE == CNS_IN) || (link_ptcone->TYPE == CNS_INOUT) ) {
      if ( (link_chain->ULINK.LOCON->NAME) == (link_ptcone->ULINK.LOCON->NAME) ) {
        match++;
      } // if ( (link_chain->ULINK.LOCON->NAME) == ... )
    }
    else {
      ptype_chain = getptype(link_chain->ULINK.LOTRS->USER,CNS_DRIVINGCONE);
      ptype_ptcone = getptype(link_ptcone->ULINK.LOTRS->USER,CNS_DRIVINGCONE);
      cone_chain  = (cone_list *)ptype_chain->DATA;
      cone_ptcone = (cone_list *)ptype_ptcone->DATA;
      if (cone_chain->NAME == cone_ptcone->NAME) {
        match++;
      } // if (cone_chain->NAME == cone_ptcone->NAME)
    }
      
    if (match == 0) {
      count++; // rien n'a matcher 
    }
    else {
      match=0;
    }
    link_ptcone=link_ptcone->NEXT;
  }
  
  return count;
}

/****************************************************************************/
/*                     function oc_coneloop_branch()                       */
/*                                                                          */
/* comparaison des BRANCH_LIST de CNSFIG->CONE                              */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int loc_coneloop_branch(file,maskopt,cone_name,branch_chain,branch_ptcone)
  FILE         *file;
  long         maskopt;
  char         *cone_name;
  branch_list  *branch_chain;
  branch_list  *branch_ptcone;
 {
  int          match=0;
  int          count=0;
  branch_list  *loop;
  branch_list  *loop_chain;
  branch_list  *loop_ptcone;
  ht           *htmatch;
  int          chainindex=0;
  int          ptconeindex=0;
  int          link_diff=0;
  
  htmatch=addht(128);

  for (loop=branch_chain; loop; loop=loop->NEXT) {
      chainindex++;
  }
  
  for (loop=branch_ptcone; loop; loop=loop->NEXT) {
      ptconeindex++;
  }
  
  if (chainindex != ptconeindex) {
    count++;
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de branches de %s différents.\n",
                     cone_name) ;
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de branches de %s différents.\n",
                     cone_name) ;
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  
    return count;
  }
  else {
    for (loop_chain=branch_chain;loop_chain;loop_chain=loop_chain->NEXT) {
      for (loop_ptcone=branch_ptcone;loop_ptcone;loop_ptcone=loop_ptcone->NEXT) {
        if(match ==0) {      
          if (loop_chain->TYPE != loop_ptcone->TYPE)
            continue;

          if ((link_diff=loc_link_diff(loop_chain,loop_ptcone)) == 0) {
            if (gethtitem(htmatch,loop_ptcone)==EMPTYHT) {
              addhtitem(htmatch,loop_ptcone,(long)loop_ptcone); 
              match++;
            }
          }
        } //if(match ==0) sinon on a deja matcher le loop_chain
      } // for (loop_ptcone)
      
      if (match==0) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: branche de %s differente entre les deux fichiers.\n",cone_name) ;
        }  // if (maskopt&DIFF_OPT_NOOUT)
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
         (void)fprintf(stdout,"*** ERROR: branche de %s differente entre les deux fichiers.\n",cone_name) ;
        } // if (maskopt&DIFF_OPT_VERBOSE)
      } //if (match==0)
      else
        match=0;
      
    } //  for (loop_chain)
  }  // else

  delht(htmatch);
  return count;
 }

/****************************************************************************/
/*                     function loc_coneloop_cells()                        */
/*                                                                          */
/* comparaison du champ CELLS de CNSFIG->CONE                               */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int loc_coneloop_cells(file,maskopt,cone_name,chaincells,ptconecells)
  FILE         *file;
  long         maskopt;
  char         *cone_name;
  cell_list    *chaincells;
  cell_list    *ptconecells;
 {
  int          count=0;
  int          chainindex=0;
  int          ptconeindex=0;
  cell_list    *loop;
  
  //on verifie simplement le nombre de cells, chaque cell sont verifiee ailleurs.

  for (loop=chaincells; loop; loop=loop->NEXT) {
      chainindex++;
  }
  
  for (loop=ptconecells; loop; loop=loop->NEXT) {
      ptconeindex++;
  }

  if (chainindex != ptconeindex) {
    count++;
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de CELLS de %s différents.\n",
                     cone_name) ;
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de CELLS de %s différents.\n",
                     cone_name) ;
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)

  }

  return count;
 }

/****************************************************************************/
/*                  function cnsdiff_cnsfigcone_diff()                      */
/*                                                                          */
/* comparaison des CONE de deux figures CNS                                 */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int cnsdiff_cnsfigcone_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  
  cone_list    *chain;
  cone_list    *ptcone;
  cnsfig_list  *minsize;
  cnsfig_list  *maxsize;
  ht           *htmin;
  ht           *htmax;
  int          index1=0;
  int          index2=0;
  int          deltaindex=0;
  int          count=0;

  
  printconediffheader(file,maskopt) ;
  
  htmin=addht(128);
  htmax=addht(128);
  
  for(chain=cnsfig1->CONE;chain;chain=chain->NEXT) {
          index1++;
  }
  for(chain=cnsfig2->CONE;chain;chain=chain->NEXT) {
          index2++;
  }
  
  if (index2 == index1) {
    minsize= cnsfig1;
    maxsize = cnsfig2;
  } // if (index2 == index1)
  else {
    if ( index1 > index2) {
      minsize= cnsfig2;
      maxsize = cnsfig1;
      deltaindex = index1 - index2;
    } // if ( index1 > index2)
    else {
      minsize= cnsfig1;
      maxsize = cnsfig2;
      deltaindex = index2 - index1;
    } // else 
    
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de cones differents (%d) entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de cones differents (%d) entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  } // else (index2 != index1)
  
  for (chain=minsize->CONE;chain;chain=chain->NEXT) {
    if (gethtitem(htmin,chain->NAME)==EMPTYHT)
          addhtitem(htmin,chain->NAME,(long)chain);
  }

  for (chain=maxsize->CONE;chain;chain=chain->NEXT) {
    if (gethtitem(htmax,chain->NAME)==EMPTYHT)
          addhtitem(htmax,chain->NAME,(long)chain);

    if (((long)ptcone=gethtitem(htmin,chain->NAME))==EMPTYHT) {
      count++;
    }
    else { 
            
      if (chain->INDEX != ptcone->INDEX) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs INDEX different entre %s (%ld) et %s (%ld)\n",
                        chain->NAME,maxsize->NAME,chain->INDEX,minsize->NAME,ptcone->INDEX) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs INDEX different entre %s (%ld) et %s (%ld)\n",
                        chain->NAME,maxsize->NAME,chain->INDEX,minsize->NAME,ptcone->INDEX) ; } 
      } // test de CONE->INDEX
              
      if (chain->TYPE != ptcone->TYPE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs TYPE different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->TYPE,minsize->NAME,ptcone->TYPE) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs TYPE different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->TYPE,minsize->NAME,ptcone->TYPE) ; }
      } // test de CONE->TYPE
      
      if (chain->TECTYPE != ptcone->TECTYPE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs TECTYPE different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->TECTYPE,minsize->NAME,ptcone->TECTYPE) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs TECTYPE different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->TECTYPE,minsize->NAME,ptcone->TECTYPE) ; }
      } // test de CONE->TECTYPE
      
      if (chain->XM != ptcone->XM) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs XM different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->XM,minsize->NAME,ptcone->XM) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs XM different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->XM,minsize->NAME,ptcone->XM) ; }
      } // test de CONE->XM
            
      if (chain->Xm != ptcone->Xm) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs Xm different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->Xm,minsize->NAME,ptcone->Xm) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs Xm different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->Xm,minsize->NAME,ptcone->Xm) ; }
      } // test de CONE->Xm
         
      if (chain->YM != ptcone->YM) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs YM different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->YM,minsize->NAME,ptcone->YM) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs YM different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->YM,minsize->NAME,ptcone->YM) ; }
      } // test de CONE->YM
            
      if (chain->Ym != ptcone->Ym) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs Ym different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->Ym,minsize->NAME,ptcone->Ym) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs Ym different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,chain->Ym,minsize->NAME,ptcone->Ym) ; }
      } // test de CONE->Ym
    
/*      
      if (chain->USER != ptcone->USER) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs USER different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,(long)chain->USER,minsize->NAME,(long)ptcone->USER) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs USER different entre %s (%ld) et %s (%ld)\n\n",
                        chain->NAME,maxsize->NAME,(long)chain->USER,minsize->NAME,(long)ptcone->USER) ; }
      } // test de CONE->USER
*/
      
     count += loc_coneloop_incone(file,maskopt,chain,ptcone) ;
     count += loc_coneloop_outcone(file,maskopt,chain,ptcone) ;
     count += loc_coneloop_branch(file,maskopt,chain->NAME,chain->BREXT,ptcone->BREXT) ;
     count += loc_coneloop_branch(file,maskopt,chain->NAME,chain->BRVDD,ptcone->BRVDD) ;
     count += loc_coneloop_branch(file,maskopt,chain->NAME,chain->BRVSS,ptcone->BRVSS) ;
     count += loc_coneloop_branch(file,maskopt,chain->NAME,chain->BRGND,ptcone->BRGND) ;
     count += loc_coneloop_cells(file,maskopt,chain->NAME,chain->CELLS,ptcone->CELLS) ;
      
    }// fin des tests
  }

  for (chain=minsize->CONE;chain;chain=chain->NEXT) {
    if (gethtitem(htmax,chain->NAME)==EMPTYHT)
      count++;
  }
   
  delht(htmin);
  delht(htmax);
  
  return count;    
}

/****************************************************************************/
/*                    function printcelldiffheader()                        */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison des CELL                          */
/****************************************************************************/

void printcelldiffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                   COMPARAISON DES CELLS                  #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                   COMPARAISON DES CELLS                  #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                    function loc_match_conescell()                        */
/*                                                                          */
/*                                                                          */
/* retourne 1 si les CELLS sont identiques et 0 sinon                       */
/****************************************************************************/

int loc_match_conescell(coneref,conediff,file,maskopt)
  chain_list    *coneref;
  chain_list    *conediff;
  FILE         *file;
  long         maskopt;
{
  int          match=0;
  ht           *htmatchcone;

  htmatchcone=addht(128);
        
        for (coneref=coneref;coneref;coneref=coneref->NEXT) {
          for (conediff=conediff;conediff;conediff=conediff->NEXT) {
            if ( ((cone_list *)coneref->DATA)->NAME == ((cone_list *)conediff->DATA)->NAME ) {
              if (gethtitem(htmatchcone,conediff)==EMPTYHT) {
                addhtitem(htmatchcone,conediff,(long)conediff); 
                match++;
              }
            }
            
          } // for (loop_max=maxcell)
          if (match == 0) {
            if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
              (void)fprintf(file,"*** ERROR: Cone %s manquant dans une cellule\n",
                     ((cone_list *)coneref->DATA)->NAME) ; 
              (void)fprintf(file,"\n") ; 
            }  // if (maskopt&DIFF_OPT_NOOUT)
            if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
              (void)fprintf(stdout,"*** ERROR: Cone %s manquant dans une cellule\n",
                     ((cone_list *)coneref->DATA)->NAME) ; 
              (void)fprintf(stdout,"\n") ; 
            } // if (maskopt&DIFF_OPT_VERBOSE)
            return 0; // un cone est manquant
          }
        } // for (loop_min=mincell)

        delht(htmatchcone); // on libere pour la liste de cone de la prochaine cellule        

  return 1;
}

/****************************************************************************/
/*                  function cnsdiff_cnsfigcell_diff()                      */
/*                                                                          */
/* comparaison des CELL de deux figures CNS                                 */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int cnsdiff_cnsfigcell_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  cell_list    *mincell;
  cell_list    *maxcell;
  chain_list   *loop_min;
  chain_list   *loop_max;
  cnsfig_list  *minsize;
  cnsfig_list  *maxsize;
  ht           *htmatchcell;
  int          index1=0;
  int          index2=0;
  int          indexconemin=0;
  int          indexconemax=0;
  int          deltaindex=0;
  int          match=0;
  int          count=0;
  
  htmatchcell=addht(128);
  
  printcelldiffheader(file,maskopt) ;
  
  for(mincell=cnsfig1->CELL;mincell;mincell=mincell->NEXT) {
          index1++;
  }
  for(mincell=cnsfig2->CELL;mincell;mincell=mincell->NEXT) {
          index2++;
  }
  
  if (index2 == index1) {
    minsize= cnsfig1;
    maxsize = cnsfig2;
  } // if (index2 == index1)
  else {
    count++;
    if ( index1 > index2) {
      minsize= cnsfig2;
      maxsize = cnsfig1;
      deltaindex = index1 - index2;
    } // if ( index1 > index2)
    else {
      minsize= cnsfig1;
      maxsize = cnsfig2;
      deltaindex = index2 - index1;
    } // else 
    
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de cells differents (%d) entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de cells differents (%d) entre %s(%d) et %s(%d) \n",
                     deltaindex,cnsfig1->NAME,index1,cnsfig2->NAME,index2) ; 
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  } // else (index2 != index1)
 
  
  if (index2 == index1) {
          
    for (mincell=minsize->CELL;mincell;mincell=mincell->NEXT) {
      
      for (loop_min=mincell->CONES;loop_min;loop_min=loop_min->NEXT) {
        indexconemin++;
      }

      for (maxcell=maxsize->CELL;maxcell;maxcell=maxcell->NEXT) {
           
        if (gethtitem(htmatchcell,maxcell)==EMPTYHT) {
      
          if (mincell->TYPE != maxcell->TYPE)
            continue;
        
          for (loop_max=maxcell->CONES;loop_max;loop_max=loop_max->NEXT) {
            indexconemax++;
          }
        
          if (indexconemin != indexconemax)
            continue;
        
          if ( (match=loc_match_conescell(loop_min,loop_max,file,maskopt)) ==0)
            continue;
        
          addhtitem(htmatchcell,maxcell,(long)maxcell); 
        } // if (gethtitem(htmatchcell,maxcell)==EMPTYHT)
        indexconemax=0;
      } // for minsize
      indexconemin=0;
    } // for maxsize
  }//  if (index2 == index1)

  delht(htmatchcell);
  
  return count;
}

/****************************************************************************/
/*                    function printlofigdiffheader()                       */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison des LOFIG                         */
/****************************************************************************/

void printlofigdiffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                   COMPARAISON DES LOFIG                  #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                   COMPARAISON DES LOFIG                  #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                  function cnsdiff_cnsfiglofig_diff()                     */
/*                                                                          */
/* comparaison des LOFIG de deux figures CNS                                */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int cnsdiff_cnsfiglofig_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  lofig_list    *chain;
  lofig_list    *ptlofig;
  cnsfig_list  *minsize;
  cnsfig_list  *maxsize;
  ht           *htmin;
  ht           *htmax;
  int          index1=0;
  int          index2=0;
  int          deltaindex=0;
  int          count=0;
  
  printlofigdiffheader(file,maskopt) ;
  
  htmin=addht(128);
  htmax=addht(128);
  
  for(chain=cnsfig1->LOFIG;chain;chain=chain->NEXT) {
          index1++;
  }
  for(chain=cnsfig2->LOFIG;chain;chain=chain->NEXT) {
          index2++;
  }
  
  if (index2 == index1) {
    minsize= cnsfig1;
    maxsize = cnsfig2;
  } // if (index2 == index1)
  else {
    count++;
    if ( index1 > index2) {
      minsize= cnsfig2;
      maxsize = cnsfig1;
      deltaindex = index1 - index2;
    } // if ( index1 > index2)
    else {
      minsize= cnsfig1;
      maxsize = cnsfig2;
      deltaindex = index2 - index1;
    } // else 
    
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de lofigs differents (%d) entre %s et %s \n",
                     deltaindex,cnsfig1->NAME,cnsfig2->NAME) ; 
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de lofigs differents (%d) entre %s et %s \n",
                     deltaindex,cnsfig1->NAME,cnsfig2->NAME) ; 
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  } // else (index2 != index1)
  
  
  for (chain=minsize->LOFIG;chain;chain=chain->NEXT) {
      if (gethtitem(htmin,chain->NAME)==EMPTYHT)
        addhtitem(htmin,chain->NAME,(long)chain);
  }

  for (chain=maxsize->LOFIG;chain;chain=chain->NEXT) {
      if (gethtitem(htmax,chain->NAME)==EMPTYHT)
        addhtitem(htmax,chain->NAME,(long)chain);

    if (((long)ptlofig=gethtitem(htmin,chain->NAME))==EMPTYHT) {
      count++;
    }
    else {
      if (chain->MODE != ptlofig->MODE) {
        count++;
        if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
          (void)fprintf(file,"*** ERROR: Cone %s - champs MODE different entre %s (%c) et %s (%c)\n\n",
                        chain->NAME,maxsize->NAME,chain->MODE,minsize->NAME,ptlofig->MODE) ; }
        if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
          (void)fprintf(stdout,"*** ERROR: Cone %s - champs MODE different entre %s (%c) et %s (%c)\n\n",
                        chain->NAME,maxsize->NAME,chain->MODE,minsize->NAME,ptlofig->MODE) ; }
      } // test de CONE->MODE
 
    }
  }

  delht(htmin);
  delht(htmax);
    
  return count;

}

/****************************************************************************/
/*                    function printbefigdiffheader()                       */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison des LOFIG                         */
/****************************************************************************/

void printbefigdiffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                   COMPARAISON DES BEFIG                  #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                   COMPARAISON DES BEFIG                  #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                  function cnsdiff_cnsfigbefig_diff()                     */
/*                                                                          */
/* comparaison des BEFIG de deux figures CNS                                */
/* retourne le nombre de differences                                        */
/****************************************************************************/

int cnsdiff_cnsfigbefig_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  befig_list    *chain;
  befig_list    *ptbefig;
  cnsfig_list  *minsize;
  cnsfig_list  *maxsize;
  ht           *htmin;
  ht           *htmax;
  int          index1=0;
  int          index2=0;
  int          deltaindex=0;
  int          count=0;
  
  printbefigdiffheader(file,maskopt) ;
  
  htmin=addht(128);
  htmax=addht(128);
  
  for(chain=cnsfig1->BEFIG;chain;chain=chain->NEXT) {
          index1++;
  }
  for(chain=cnsfig2->BEFIG;chain;chain=chain->NEXT) {
          index2++;
  }
  
  if (index2 == index1) {
    minsize= cnsfig1;
    maxsize = cnsfig2;
  } // if (index2 == index1)
  else {
    count++;
    if ( index1 > index2) {
      minsize= cnsfig2;
      maxsize = cnsfig1;
      deltaindex = index1 - index2;
    } // if ( index1 > index2)
    else {
      minsize= cnsfig1;
      maxsize = cnsfig2;
      deltaindex = index2 - index1;
    } // else 
    
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
       (void)fprintf(file,"*** ERROR: nombre de befigs differents (%d) entre %s et %s \n",
                     deltaindex,cnsfig1->NAME,cnsfig2->NAME) ; 
       (void)fprintf(file,"\n") ; 
    }  // if (maskopt&DIFF_OPT_NOOUT)
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
       (void)fprintf(stdout,"*** ERROR: nombre de befigs differents (%d) entre %s et %s \n",
                     deltaindex,cnsfig1->NAME,cnsfig2->NAME) ; 
       (void)fprintf(stdout,"\n") ; 
    } // if (maskopt&DIFF_OPT_VERBOSE)
  } // else (index2 != index1)
  
  
  for (chain=minsize->BEFIG;chain;chain=chain->NEXT) {
      if (gethtitem(htmin,chain->NAME)==EMPTYHT)
        addhtitem(htmin,chain->NAME,(long)chain);
  }

  for (chain=maxsize->BEFIG;chain;chain=chain->NEXT) {
      if (gethtitem(htmax,chain->NAME)==EMPTYHT)
        addhtitem(htmax,chain->NAME,(long)chain);

    if (((long)ptbefig=gethtitem(htmin,chain->NAME))==EMPTYHT) {
      count++;
    }
    else { 
    }
  }

  delht(htmin);
  delht(htmax);
  
  return count;
}

/****************************************************************************/
/*                    function printuserdiffheader()                        */
/*                                                                          */
/*                                                                          */
/* affiche une entete pour la comparaison du champs USER                    */
/****************************************************************************/

void printuserdiffheader(file,maskopt)
  FILE         *file;
  long         maskopt;
{
  if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT) {
    (void)fprintf(file,"\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"#                 COMPARAISON DU CHAMPS USER               #\n") ; 
    (void)fprintf(file,"#                                                          #\n") ; 
    (void)fprintf(file,"############################################################\n") ; 
    (void)fprintf(file,"\n") ; 
  }  // if (maskopt&DIFF_OPT_NOOUT)
  if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE) {
    (void)fprintf(stdout,"\n") ; 
    (void)fprintf(stdout,"############################################################\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"#                 COMPARAISON DU CHAMPS USER               #\n") ; 
    (void)fprintf(stdout,"#                                                          #\n") ; 
    (void)fprintf(stdout,"############################################################\n") ;
    (void)fprintf(stdout,"\n") ; 
  }  // if (maskopt&DIFF_OPT_VERBOSE)
 }

/****************************************************************************/
/*                   function cnsdiff_cnsfiguser_diff()                     */
/*                                                                          */
/* comparaison du USER de deux figures CNS                                  */
/* retourne 1 si il y a une difference, 0 sinon                             */
/****************************************************************************/

int cnsdiff_cnsfiguser_diff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  printuserdiffheader(file,maskopt) ;

  if (cnsfig1->USER != cnsfig2->USER) {
          
    if ((maskopt&DIFF_OPT_NOOUT)!=DIFF_OPT_NOOUT)    
      (void)fprintf(file,"*** ERROR: CnsFig->USER \n") ;       
    if ((maskopt&DIFF_OPT_VERBOSE)==DIFF_OPT_VERBOSE)
      (void)fprintf(stdout,"*** ERROR: CnsFig->USER \n") ;   
    
    return 1;  
  }
  return 0;
}        
/****************************************************************************/
/*                     function cnsdiff_makefigdiff()                       */
/*                                                                          */
/* comparaison de deux figures CNS                                          */
/* retourne le nombre de difference entre les deux figures                  */
/****************************************************************************/
int cnsdiff_makefigdiff(file,cnsfig1,cnsfig2,maskopt)
  FILE         *file;
  cnsfig_list  *cnsfig1;
  cnsfig_list  *cnsfig2;
  long         maskopt;
{
  int          countingerror=0;
  
  // CNSFIGURE->LOCON
  countingerror += cnsdiff_cnsfiglocon_diff(file,cnsfig1,cnsfig2,maskopt);
  // CNSFIGURE->LOTRS
  //countingerror += cnsdiff_cnsfiglotrs_diff(file,cnsfig1,cnsfig2,maskopt);
  // CNSFIGURE->CONE
  //voir pour le parcours des champs des branches.
  countingerror += cnsdiff_cnsfigcone_diff(file,cnsfig1,cnsfig2,maskopt);
  // CNSFIGURE->CELL
  countingerror += cnsdiff_cnsfigcell_diff(file,cnsfig1,cnsfig2,maskopt);
  // CNSFIGURE->LOFIG
  //countingerror += cnsdiff_cnsfiglofig_diff(file,cnsfig1,cnsfig2,maskopt);
  // CNSFIGURE->BEFIG
  //countingerror += cnsdiff_cnsfigbefig_diff(file,cnsfig1,cnsfig2,maskopt);
  // CNSFIGURE->USER
  countingerror += cnsdiff_cnsfiguser_diff(file,cnsfig1,cnsfig2,maskopt);

  return countingerror;
}
