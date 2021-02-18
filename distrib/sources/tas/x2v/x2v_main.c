/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : X2V Version 1                                               */
/*    Fichier : x2v_main.c                                                  */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "x2v_main.h"

short retkey(chaine)
char *chaine ;
{
 short i ;
 
 for(i = 0 ; (i<(signed)strlen(chaine)) && (chaine[i] != '=') ; i ++) ;
 
 return((i == (signed)strlen(chaine)) ? -1 : i) ;
}

void x2v_error()
{
 fprintf(stderr,"\nUsage : x2v -t -d -a -i -f -z -x=<min>:<max> -lm=<size> -tlf3 -tlf4 -lib -sdf \n") ;
 fprintf(stderr,"        -cout=<value (pF)> -flat -flt -gcf -fig=<\"name\"> -xin=<\"name\"> -xout=<\"name\"> filename\n") ;

 EXIT(1) ;
}

int main(argc,argv)
int argc;
char *argv[];
{
 /*FILE *file ;*/
 FILE *fileinf ;
 ttvfig_list *ttvfig ;
 lofig_list *lofig ;
 chain_list *chain ;
 chain_list *chainfig ;
 long type ;
 long typef ;
 long typeline = (long)0 ;
 char *namefig = NULL, *figin = NULL, *figout = NULL ;
 int eq ;
 int inf = 0 ;
 int i ;
 char typefile = 'T' ;
 char dtxttx = 'N' ;
 char fact = 'N' ;
 char tlf3 = 'N' ;
 char tlf4 = 'N' ;
 char lib = 'N' ;
 char sdf = 'N' ;
 char flt = 'N' ;
 char flat = 'N' ;
 char gcf = 'N' ;
 double limit = (double)100.0 ;
 char technoversion[64] ;
 
#ifdef AVERTEC
 avtenv() ;
#endif

 mbkenv() ;

 elpenv() ;

 for(i = 1 ; i != argc ; i++)
  {
   if(argv[i][0] != '-')
      {
       namefig = namealloc(argv[i]) ;
      }
   else if((eq=retkey(argv[i])) != -1)
      {
       argv[i][eq]='\0';
       if(strcmp(argv[i],"-cout") == 0)
         {
          STM_DEF_LOAD=atof((char *)(argv[i]+eq+1)) * 1000;
         }
	   else if(strcmp(argv[i],"-xin") == 0)
         {
          TTV_NAME_IN=addchain(TTV_NAME_IN,
                              (void *)namealloc((char *)(argv[i]+eq+1)));
         }
       else if(strcmp(argv[i],"-xout") == 0)
         {
          TTV_NAME_OUT=addchain(TTV_NAME_OUT,
                              (void *)namealloc((char *)(argv[i]+eq+1)));
         }
       else if(strcmp(argv[i],"-fig") == 0)
         {
          figin=(void *)namealloc((char *)(argv[i]+eq+1));
         }
       else if(strcmp(argv[i],"-out") == 0)
         {
          figout=(void *)namealloc((char *)(argv[i]+eq+1));
         }
       else if(strcmp(argv[i],"-lm") == 0)
         {
          if((limit = atof(argv[i]+eq+1)) <= 0)
            x2v_error() ;
          ttv_setcachesize((double)(limit * (double)0.1),
                           (double)(limit * (double)0.9)) ;
         }
       else if(strcmp(argv[i],"-x") == 0)
         {
          char pathopt[48] ;
          int j ;
 
          strcpy(pathopt,argv[i] + 3) ;

          for(j = 0 ; (j != (signed)strlen(pathopt)) && (pathopt[j] != ':') ; j++) ;
          if(j == (signed)strlen(pathopt))
           {
            TTV_SEUIL_MIN = atol(pathopt) ;
            TTV_SEUIL_MAX = TTV_DELAY_MAX ;
        
            if((TTV_SEUIL_MIN == 0) && (strcmp(pathopt,"0") != 0) &&
               (strcmp(pathopt,"00") != 0))
              x2v_error() ;
           }
          else 
           {
            char *aux = pathopt ;
        
            aux[j] = '\0' ;
        
            if(j != 0)
               {
                TTV_SEUIL_MIN = atol(aux) ;
        
                if((TTV_SEUIL_MIN == 0) && (strcmp(aux,"0") != 0) &&
                   (strcmp(aux,"00") != 0))
                  x2v_error() ;
               }
            else TTV_SEUIL_MIN = TTV_DELAY_MIN ;
        
            aux += j+1 ;
        
            if(aux[0] != '\0')
              {
               TTV_SEUIL_MAX = atol(aux) ;
        
               if((TTV_SEUIL_MAX == 0) && (strcmp(aux,"0") != 0) &&
                  (strcmp(aux,"00") != 0))
                 x2v_error() ;
              }
            else TTV_SEUIL_MAX = TTV_DELAY_MAX ;
           }
         }
       else  
         {
          argv[i][eq]='\0';
          x2v_error() ;
         }
       argv[i][eq]='\0';
      }
   else
      {
       if(strcmp(argv[i],"-t") == 0) typefile = 'D' ;
       else if(strcmp(argv[i],"-d") == 0) typeline = TTV_FIND_LINE ;
       else if(strcmp(argv[i],"-tlf3") == 0) tlf3 = 'Y' ;
       else if(strcmp(argv[i],"-tlf4") == 0) tlf4 = 'Y' ;
       else if(strcmp(argv[i],"-lib") == 0) lib = 'Y' ;
       else if(strcmp(argv[i],"-sdf") == 0) sdf = 'Y' ;
       else if(strcmp(argv[i],"-flt") == 0) flt = 'Y' ;
       else if(strcmp(argv[i],"-flat") == 0) flat = 'Y' ;
       else if(strcmp(argv[i],"-gcf") == 0) gcf = 'Y' ;
       else if(strcmp(argv[i],"-a") == 0) typefile = 'A' ;
       else if(strcmp(argv[i],"-i") == 0) { inf = 1 ; avt_sethashvar("avtReadInformationFile", inf_GetDefault_AVT_INF_Value()); }
       else if(strcmp(argv[i],"-f") == 0) fact = 'Y' ;
       else if(strcmp(argv[i],"-z") == 0) 
         {
          typeline = TTV_FIND_LINE ;
          dtxttx = 'Y' ;
         }
       else if(strcmp(argv[i],"-x") == 0)
         {
          TTV_SEUIL_MAX = TTV_DELAY_MAX ;
          TTV_SEUIL_MIN = TTV_DELAY_MIN ;
         }
       else x2v_error() ;
      }
  }

 if((TTV_NAME_IN == NULL) && (TTV_NAME_OUT != NULL))
  TTV_NAME_IN = addchain(NULL,(void *)namealloc("*")) ;
 if((TTV_NAME_IN != NULL) && (TTV_NAME_OUT == NULL))
  TTV_NAME_OUT = addchain(NULL,(void *)namealloc("*")) ;
 if((TTV_NAME_IN == NULL) && (TTV_NAME_OUT == NULL))
   TTV_PATH_SAVE = TTV_SAVE_ALLPATH ;
 else
   TTV_PATH_SAVE = TTV_SAVE_PATH ;

 if(namefig == NULL) 
     x2v_error() ;

#ifdef AVERTEC_LICENSE
if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
 EXIT(1) ;
#endif

 avt_banner("x2v","timing files translator","1998");

 tlfenv() ;
 libenv() ;
 cbhenv() ;
 ttvenv() ;

 if(lib == 'Y')
  {
   chainfig = lib_load(namefig) ;
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
    {
     ttvfig = (ttvfig_list *)chain->DATA ;
     ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
     if(typefile == 'D')
       ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL) ;
     stm_drivecell(ttvfig->INFO->FIGNAME) ;
    }
   freechain(chainfig) ;
   /* liberation des lofig, cbhcomb et cbhseq creees pour le classifier dans les parsers lib et tlf */
   cbh_freeclassifier();
   EXIT(0) ;
  }
 
 if(tlf3 == 'Y')
  {
   chainfig = tlf_load(namefig, 3) ;
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
    {
     ttvfig = (ttvfig_list *)chain->DATA ;
     ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
     if(typefile == 'D')
       ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL) ;
     stm_drivecell(ttvfig->INFO->FIGNAME) ;
    }
   freechain(chainfig) ;
   /* liberation des lofig, cbhcomb et cbhseq creees pour le classifier dans les parsers lib et tlf */
   cbh_freeclassifier();
   EXIT(0) ;
  }

 if(tlf4 == 'Y')
  {
   chainfig = tlf_load(namefig, 4) ;
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
    {
     ttvfig = (ttvfig_list *)chain->DATA ;
     ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
     if(typefile == 'D')
       ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL) ;
     stm_drivecell(ttvfig->INFO->FIGNAME) ;
    }
   freechain(chainfig) ;
   /* liberation des lofig, cbhcomb et cbhseq creees pour le classifier dans les parsers lib et tlf */
   cbh_freeclassifier();
   EXIT(0) ;
  }

 if(gcf == 'Y') 
  {
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   tlf_gcf2stb(namefig) ;
   EXIT(0) ;
  }

 if(sdf == 'Y') 
  {
   lofig = getlofig (namefig, 'A') ;
   if (figin != NULL)
     lofig = getloadedlofig (figin);
   if(flt == 'Y') 
     rflattenlofig(lofig, YES, YES);
   bck_sdfparse (namefig, lofig) ;
   if (figout != NULL)
     ttvfig = bck_bck2ttv(lofig, figout) ;
   else
     ttvfig = bck_bck2ttv(lofig, namefig) ;
   if(elpTechnoName[0] == '\0'){
       ttvfig->INFO->TECHNONAME = namealloc("unknown") ;
       sprintf(technoversion,"%.2f",0.0) ;
   }else{
       ttvfig->INFO->TECHNONAME = namealloc(elpTechnoName) ;
       sprintf(technoversion,"%.2f",elpTechnoVersion) ;
   }
   ttvfig->INFO->TECHNOVERSION = namealloc(technoversion) ;
   
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL) ;

   ttv_getinffile(ttvfig) ;

   if(dtxttx == 'Y')
    {
     if(fact == 'Y')
       ttv_detectinter(ttvfig,TTV_FIND_LINE) ;
     ttv_builtpath(ttvfig,TTV_FIND_HIER|TTV_FIND_LINE) ;
     ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
    }
   EXIT(0) ;
  }

 
 if((typeline & TTV_FIND_LINE) == TTV_FIND_LINE)
    typef = TTV_FILE_DTX ;
 else
    typef = TTV_FILE_TTX ;

   typef |= TTV_FILE_INF ;

 if((ttvfig = ttv_getttvfig(namefig,typef)) == NULL) 
   x2v_error() ;

 if(getinffig(namefig)==NULL && (inf == 1))
  {
   fprintf(stderr, "*** x2v warning ***\n");
   fprintf(stderr, "can not open the file %s.inf\n",namefig);
   inf = 0 ;
  }
 

 if(flat == 'Y')
  {
   stm_getcell(ttvfig->INFO->FIGNAME) ;
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   if(typefile == 'D')
     {
      ttv_parsttvfig(ttvfig,TTV_STS_DTX,TTV_FILE_DTX) ;
      ttv_flatttvfig(ttvfig,ttvfig->INS,TTV_FILE_DTX) ;
      ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL) ;
      stm_drivecell(ttvfig->INFO->FIGNAME) ;
     }
   if(typefile == 'T')
     {
      ttv_parsttvfig(ttvfig,TTV_STS_TTX,TTV_FILE_TTX) ;
      ttv_flatttvfig(ttvfig,ttvfig->INS,TTV_FILE_TTX) ;
      ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
      stm_drivecell(ttvfig->INFO->FIGNAME) ;
     } 
   if(typefile == 'A')
     {
      ttv_parsttvfig(ttvfig,TTV_STS_TTX,TTV_FILE_TTX) ;
      ttv_parsttvfig(ttvfig,TTV_STS_DTX,TTV_FILE_DTX) ;
      ttv_flatttvfig(ttvfig,ttvfig->INS,TTV_FILE_TTX|TTV_FILE_DTX) ;
      ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
      ttv_drittv(ttvfig,TTV_FILE_DTX,(long)0, NULL) ;
      stm_drivecell(ttvfig->INFO->FIGNAME) ;
     } 
  }
 
 if(dtxttx == 'Y')
  {
   stm_getcell(ttvfig->INFO->FIGNAME) ;
   ttv_parsttvfig(ttvfig,TTV_STS_DTX,TTV_FILE_DTX) ;
   if(ttvfig->INS != NULL)
    {
     fprintf(stderr, "*** x2v error ***\n");
     fprintf(stderr, "%s.ttx can not be obtained from a hierarchical dtx file\n",
                        ttvfig->INFO->FIGNAME);
     EXIT(1) ;

    }
   if(fact == 'Y')
     ttv_detectinter(ttvfig,TTV_FIND_LINE) ;
   ttv_builtpath(ttvfig,TTV_FIND_HIER|TTV_FIND_LINE) ;
   #ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
   #endif
   ttv_drittv(ttvfig,TTV_FILE_TTX,(long)0, NULL) ;
   stm_drivecell(ttvfig->INFO->FIGNAME) ;
  }
 
 if((typefile == 'T') || (typefile == 'A'))
   {
    type = TTV_FILE_TTV ;
    if((typeline & TTV_FIND_LINE) == TTV_FIND_LINE)
      ttv_parsttvfig(ttvfig,TTV_STS_DTX,TTV_FILE_DTX) ;
    else
      ttv_parsttvfig(ttvfig,TTV_STS_TTX,TTV_FILE_TTX) ;
#ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
#endif
    if(typefile == 'T')
     {
      if(ttv_existefile(ttvfig,TTV_FILE_DTX) != 0)
       {
        ttv_drittvold(ttvfig,type,typeline) ;
       }
      else
        ttv_drittvold(ttvfig,type,TTV_FIND_NOTSAVE) ;
     }
    else
      ttv_drittvold(ttvfig,type,typeline) ;
   }
 if((typefile == 'D') || (typefile == 'A'))
   {
    type = TTV_FILE_DTV ;
    ttv_parsttvfig(ttvfig,TTV_STS_DTX,TTV_FILE_DTX) ;
#ifdef AVERTEC_LICENSE
     if(avt_givetoken("HITAS_LICENSE_SERVER","x2v") != AVT_VALID_TOKEN)
       EXIT(1) ;
#endif
    ttv_drittvold(ttvfig,type,(long)0) ;
   }

 ttv_freeallttvfig(ttvfig) ;

 EXIT(0) ;
}
