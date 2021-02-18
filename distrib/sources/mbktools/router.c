/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : ROUTER Version 1                                            */
/*    Fichier : router.c                                                    */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

#include "router.h"

char *ROU_FILEOUT= NULL ;
char ROU_CTC = 'N' ;
int ROU_MINCAPA = 100 ;
int ROU_MAXCAPA = 10000 ;
int ROU_MINRES = 100 ;
int ROU_MAXRES = 10000 ;
int ROU_MAXWIRELIST = 1000 ;

static short retkey(chaine)
char *chaine ;
{
 unsigned int    i ;

 for(i = 0 ; (i<strlen(chaine)) && (chaine[i] != '=') ; i ++) ;

 return ((i == strlen(chaine)) ? -1 : (short)i) ;
}

void route_error()
{
 fprintf(stderr,"\nrouter version %s\n",ROU_VERSION) ;
 fprintf(stderr,"usage : router [option] filename\n") ;
 fprintf(stderr,"\t-k : cross talk capacitance \n") ; 
 fprintf(stderr,"\t-spef : spef output else spice\n") ;
 fprintf(stderr,"\t-in=<in format> : input format \n") ;
 fprintf(stderr,"\t-wire=<nbwire> : limit de number wire list\n") ; 
 fprintf(stderr,"\t-cmin=<cmin> : node capacitance min\n") ; 
 fprintf(stderr,"\t-cmax=<cmax> : node capacitance max\n") ;
 fprintf(stderr,"\t-rmin=<cmin> : wire resistance min\n") ; 
 fprintf(stderr,"\t-rmax=<cmax> : wire resistance max\n") ; 
 fprintf(stderr,"\t-out=<\"fileout\"> : output filename\n") ;

 EXIT(1) ;
}

long max( long a, long b )
{
  if( a<b)
    return b;
  return a;
}

int route_rand(min,max)
int min ;
int max ;
{
 return((rand() % (max-min+1)) + min) ;
}

float route_capa()
{
 return((float)route_rand(ROU_MINCAPA,ROU_MAXCAPA)/100000.0) ;
}

float route_resis()
{
 return((float)route_rand(ROU_MINRES,ROU_MAXRES)/100.0) ;
}

long route_treelosig(losig,node,nbcon,loconlist)
losig_list *losig ;
long node ;
int nbcon ;
num_list **loconlist ;
{
 num_list *ptnum ;
 long nodex ;
 int nbwirelist ;
 int nbbranch ;
 int i ; 

 nbwirelist = route_rand(0,ROU_MAXWIRELIST) ;

 for(i = 0 ; i < nbwirelist - 1 ; i++)
  {
   addlowire(losig,0,route_resis(),route_capa(),node,node+1) ;
   node++ ;
  } 

 if(nbcon == 1)
   {
    ptnum = *loconlist ;
    *loconlist = (*loconlist)->NEXT ;
    ptnum->NEXT = NULL ;
    addlowire(losig,0,route_resis(),route_capa(),node,ptnum->DATA) ;
    freenum(ptnum) ;
    return(node) ;
   }
 else
   {
    nbbranch = route_rand(1,nbcon-1) ;
   }

 addlowire(losig,0,route_resis(),route_capa(),node,node+1) ;

 nodex = node +1;
 node = route_treelosig(losig,node+1,nbbranch,loconlist) ;

 addlowire(losig,0,route_resis(),route_capa(),nodex,node+1) ;

 node = route_treelosig(losig,node+1,nbcon-nbbranch,loconlist) ;

 return(node) ;
}

void route_losig(losig)
losig_list *losig ;
{
 locon_list *locon = NULL ;
 ptype_list *ptype ;
 chain_list *chain ;
 num_list *ptnum = NULL ;
 int nbcon ; 

 ptype = getptype(losig->USER,LOFIGCHAIN) ;

 if(losig->PRCN != NULL)
    freelorcnet(losig) ;

 addlorcnet(losig) ;


 for(chain = (chain_list *)ptype->DATA , nbcon = 0 ; chain != NULL ; 
     chain = chain->NEXT, nbcon++)
   {
    locon = (locon_list *)chain->DATA ;
    setloconnode(locon,nbcon+1) ;
    if(chain->NEXT != NULL)
      ptnum = addnum(ptnum,locon->PNODE->DATA) ;
   }
 
 if( locon && nbcon != 1)
   route_treelosig(losig,locon->PNODE->DATA,nbcon-1,&ptnum) ;
}

void route_lofig(lofig)
lofig_list *lofig ;
{
 losig_list *losig ;
 int cnt=0, cur;

 for(losig = lofig->LOSIG ; losig != NULL ; losig = losig->NEXT) cnt++;

 for(losig = lofig->LOSIG, cur=0 ; losig != NULL ; losig = losig->NEXT)
   {
    if(mbk_LosigIsVDD(losig) || mbk_LosigIsVSS(losig))
      continue ;
    if ((cur % 100)==0) {fprintf(stdout,"\rsig %d/%d", cur, cnt);fflush(stdout);}
    cur++;
    route_losig(losig) ;
   }
 fprintf(stdout,"\n");
}

void route_ctc(lofig)
lofig_list *lofig ;
{
 losig_list *losig ;
 losig_list **tabsig ;
 int nbsig ;
 int i ;
 int j ;
 int k ;
 int nbctc, totctc;

 for(losig = lofig->LOSIG , nbsig = 0 ; losig != NULL ; losig = losig->NEXT)
   {
    if(mbk_LosigIsVDD(losig) || mbk_LosigIsVSS(losig))
     {
      continue ;
     }
    nbsig++ ;
   }

 tabsig = (losig_list **)mbkalloc(nbsig * sizeof(losig_list*)) ;

 for(losig = lofig->LOSIG , nbsig = 0 ; losig != NULL ; losig = losig->NEXT)
   {
    if(mbk_LosigIsVDD(losig) || mbk_LosigIsVSS(losig))
     {
      continue ;
     }
    tabsig[nbsig] = losig ;
    nbsig++ ;
   }

 for(i = 0, totctc=0 ; i < nbsig ;  i++)
   {
    losig = tabsig[i] ;
    nbctc=0;
    for(j = 1 ; j < losig->PRCN->NBNODE ; j++)
      {
       if(route_rand(0,1))
         {
          k = route_rand(0,nbsig-1) ;    
          if(k != i )
            {
              addloctc( losig,
                        j,
                        tabsig[k],
                        route_rand(1,max(1,tabsig[k]->PRCN->NBNODE-1)),
                        route_capa()
                      ) ;
              totctc+=2;
              nbctc++;
            }
         }
      }
    if ((i % 100)==0)
      {
        fprintf(stdout,"\rsig: %d/%d, %.1f ctcs/sig",i+1, nbsig, ((float)totctc)/(float)(i+1));
        fflush(stdout);
      }
   }
 fprintf(stdout,"\n");
 mbkfree(tabsig) ;
}

int main(argc,argv)
int argc;
char *argv[];
{
 char *namefig = NULL ;
 char buf[1024] ;
 lofig_list *lofig ;
 int eq ;
 int i ;
 int spef = 'N' ;

#ifdef AVERTEC
 avtenv() ;
#endif

 mbkenv() ;

 strcpy(OUT_LO,"spi");

 for(i = 1 ; i != argc ; i++)
  {
   if(argv[i][0] != '-')
      {
       namefig = namealloc(argv[i]) ;
      }
   else if((eq=retkey(argv[i])) != -1)
      {
       argv[i][eq]='\0';
       if(strcmp(argv[i],"-out") == 0)
         {
          ROU_FILEOUT = namealloc((char *)(argv[i]+eq+1));
         }
       else if(strcmp(argv[i],"-wire") == 0)
         {
          ROU_MAXWIRELIST = atoi(argv[i]+eq+1) ;
         }
       else if(strcmp(argv[i],"-cmin") == 0)
         {
          ROU_MINCAPA = (int)(atof(argv[i]+eq+1) * 100.0) ;
         }
       else if(strcmp(argv[i],"-cmax") == 0)
         {
          ROU_MAXCAPA = (int)(atof(argv[i]+eq+1) * 100.0) ;
         }
       else if(strcmp(argv[i],"-rmin") == 0)
         {
          ROU_MINRES = (int)(atof(argv[i]+eq+1) * 100.0) ;
         }
       else if(strcmp(argv[i],"-rmax") == 0)
         {
          ROU_MAXRES = (int)(atof(argv[i]+eq+1) * 100.0) ;
         }
       else if(strcmp(argv[i],"-in") == 0)
         {
          strcpy(IN_LO,argv[i]+eq+1) ;
         }
       else
         {
          argv[i][eq]='\0';
          route_error() ;
         }
      }
   else
      {
       if(strcmp(argv[i],"-k") == 0) 
        {
         ROU_CTC = 'Y' ;
        }
       else if(strcmp(argv[i],"-spef") == 0) 
        {
         spef = 'Y' ;
        }
       else route_error() ;
      }
  }

 avt_banner("RouTer","Netlist Rc Router","1998");

 if(namefig == NULL)
   route_error() ;

 fprintf(stdout,".parsing '%s' ...",namefig); fflush(stdout);
 lofig = getlofig(namefig,'A') ;
 fprintf(stdout,"\n");
 if(ROU_FILEOUT != NULL)
    lofig->NAME = ROU_FILEOUT ;
 else
   {
    if((strcmp(IN_LO,"spi") == 0) && (spef == 'N'))
     {
      sprintf(buf,"%s_r",lofig->NAME) ;
      lofig->NAME = namealloc(buf) ;
     }
   }

 lofigchain(lofig) ;

 if(ROU_MINCAPA > ROU_MAXCAPA) ROU_MINCAPA = ROU_MAXCAPA ;
 if(ROU_MINRES > ROU_MAXRES) ROU_MINRES = ROU_MAXRES ;

 fprintf(stdout,".adding rc ...\n");
 route_lofig(lofig) ;

 if(ROU_CTC == 'Y') 
   {
     fprintf(stdout,".adding ctc ...\n");
     route_ctc(lofig) ;
   }
 if(spef == 'N')
   savelofig(lofig) ;
 else
   spef_drive(lofig, NULL) ;

 EXIT(0) ;
 return 0;
}
