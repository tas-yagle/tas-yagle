/*--------------------------------------------------------------------------*/
/*      fichier.c où se trouve les fonctions ajoutées au code pour          */
/*                                                                          */
/*      @     @@   @@  @  @ @  @ @@@    @@@@@  @@  @@@  @     @@@           */
/*      @    @  @ @  @ @ @  @  @ @  @     @   @  @ @  @ @    @              */
/*      @    @  @ @  @ @@   @  @ @@@      @   @@@@ @@@@ @    @@@@           */
/*      @    @  @ @  @ @ @  @  @ @        @   @  @ @  @ @    @              */
/*      @@@@  @@   @@  @  @  @@  @        @   @  @ @@@  @@@@  @@@           */
/*--------------------------------------------------------------------------*/



#include "tas.h"


#define CARAC_PREC   2

/*--------------------------------------------------------------------------*/
/*caracmode()  renvoie le mode de caracterisation pour un cone et son input */
/*             FCARAC: caractérisation par une liste de fronts              */
/*             CCARAC: caractérisation par une liste de capas               */
/*             LES2:   caractérisation par 2 listes                         */
/*             mode=0: pas de caractérisation(cone interne)                 */
/*--------------------------------------------------------------------------*/
int caracmode(cone_list *cone, edge_list *input, double **ccarac, int *nccarac, double **fcarac, int *nfcarac)
{
    ptype_list *ptype ;
    locon_list *locon ;
    cone_list  *incone;
    double     *ccaracio;
    int         mode,
                nccaracio,
                condf = 0,
                condc = 0;

	if((cone->TYPE & TAS_CONE_CONNECT) != TAS_CONE_CONNECT){
		if((input->TYPE & CNS_EXT)==CNS_EXT) {
			
			ptype = getptype (input->UEDGE.LOCON->USER, TAS_CON_CARAC) ;
			if (input->UEDGE.LOCON->DIRECTION == CNS_I) {
				*fcarac = ((caraccon_list*)ptype->DATA)->SLOPEIN ;
				*nfcarac = ((caraccon_list*)ptype->DATA)->NSLOPE ;
				if(*nfcarac)
        			condf=1;
				else
					printf("WARNING: no characterization for pin %s\n", input->UEDGE.LOCON->NAME);
                    							
			} else if (input->UEDGE.LOCON->DIRECTION == CNS_B || input->UEDGE.LOCON->DIRECTION==CNS_T) {
				ptype = getptype (input->UEDGE.LOCON->USER, CNS_EXT) ;
				incone = (cone_list*)ptype->DATA ;
				ptype = getptype (input->UEDGE.LOCON->USER, TAS_CON_CARAC) ;
				ccaracio = ((caraccon_list*)ptype->DATA)->CAPAOUT ;
				nccaracio = ((caraccon_list*)ptype->DATA)->NCAPA ;
                if (input->UEDGE.LOCON->DIRECTION == CNS_B){
    				*nfcarac = nccaracio ;
	    			*fcarac = autocarac (incone, nccaracio, ccaracio) ;
                }else if(input->UEDGE.LOCON->DIRECTION==CNS_T){
				    *fcarac = ((caraccon_list*)ptype->DATA)->SLOPEIN ;
				    *nfcarac = ((caraccon_list*)ptype->DATA)->NSLOPE ;
                }
				if(*nfcarac)
        			condf=1;
				else
					printf("WARNING: no characterization for pin %s\n", locon->NAME);
			}
		}
		if((input->TYPE & CNS_CONE)==CNS_CONE){
    		if((input->UEDGE.CONE->TYPE & CNS_EXT)==CNS_EXT) {
				//ptype = getptype (input->UEDGE.CONE->USER, CNS_EXT) ;
				locon = cns_get_one_cone_external_connector(input->UEDGE.CONE);
				ptype = getptype (locon->USER, TAS_CON_CARAC) ;
				if (locon->DIRECTION == CNS_I) {
					*fcarac = ((caraccon_list*)ptype->DATA)->SLOPEIN ;
					*nfcarac = ((caraccon_list*)ptype->DATA)->NSLOPE ;
					if(*nfcarac)
        				condf=1;
					else
						printf("WARNING: no characterization for pin %s\n", locon->NAME);
				} else if (locon->DIRECTION == CNS_B || locon->DIRECTION==CNS_T) {
					ccaracio = ((caraccon_list*)ptype->DATA)->CAPAOUT ;
					nccaracio = ((caraccon_list*)ptype->DATA)->NCAPA ;
                    if (locon->DIRECTION == CNS_B){
    					*nfcarac = nccaracio ;
    					*fcarac = autocarac (input->UEDGE.CONE, nccaracio, ccaracio) ;
                    }else if(locon->DIRECTION==CNS_T){
    					*fcarac = ((caraccon_list*)ptype->DATA)->SLOPEIN ;
	    				*nfcarac = ((caraccon_list*)ptype->DATA)->NSLOPE ;
                    }
					if(*nfcarac)
        				condf=1;
					else
						printf("WARNING: no characterization for pin %s\n", locon->NAME);
				}
			}
        }
    }

	if(cone->OUTCONE) {
        if((cone->TYPE & CNS_EXT)==CNS_EXT){
            if((cone->TYPE & TAS_CONE_CONNECT) != TAS_CONE_CONNECT) {
//                ptype = getptype (cone->USER, CNS_EXT) ;
			    locon = cns_get_one_cone_external_connector(cone);
			    if(locon->DIRECTION!=CNS_I){
		    	    ptype = getptype (locon->USER, TAS_CON_CARAC) ;
		    	    *ccarac = ((caraccon_list*)ptype->DATA)->CAPAOUT ;
			        *nccarac = ((caraccon_list*)ptype->DATA)->NCAPA ;
		    		    if(*nccarac)
                		    condc=1;
				        else
					        printf("WARNING: no characterization for pin %s\n", locon->NAME);
			    }
		    }
        }
    }
    if(condf && condc) {
        mode=(FCARAC | CCARAC);
    } else if(condf) {
        mode=(FCARAC);
    } else if(condc) {
        mode=(CCARAC);
    } else 
        mode=(0);
    
    return(mode);
}

/*----------------------------------------------------------------------------*/
/*                    frontbis()                                              */
/*                                                                            */
/* generation de front pour la caracterisation sans .inf                      */
/*----------------------------------------------------------------------------*/
long frontbis(cone, fcarac, ccarac, fll, fhh)
cone_list *cone;
long       fcarac;
double     ccarac;
long      *fll;
long      *fhh;
{
    char         ext='n',
                 exl='n',
                 vdd='n',
                 vss='n' ;
    branch_list *pathext ;
    branch_list *pathvdd ;
    branch_list *pathvss ;
    front_list  *slope ;
    locon_list  *locon ;
    ptype_list *ptype ;
    chain_list *chain ;

/*---------------------------------------------------------------------------*/
/* a ce niveau, les branches de chaque cone sont classees dans l'ordre       */
/* decroissant et par type : EXT, VDD et VSS. Dans un premier temps on       */
/* la branche la plus resistive pour chaque type (premiere branche de chaque */
/* liste).                                                                   */
/*---------------------------------------------------------------------------*/

if(cone->BREXT != NULL)
   if((cone->BREXT->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
      {
       ext = 'o' ;
       for(pathext = cone->BREXT ; pathext != NULL ; pathext = pathext->NEXT)
         if((pathext->LINK->NEXT != NULL) &&
            ((pathext->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL))
          {
           exl = 'o' ;
           break ;
          }
       }
if(cone->BRVDD != NULL)
   if((cone->BRVDD->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
        vdd = 'o' ;
if(cone->BRVSS != NULL)
   if((cone->BRVSS->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
        vss = 'o' ;

if((cone->TYPE & CNS_EXT) == CNS_EXT)
 {
  if((vdd == 'n') && (vss == 'n') && (exl == 'n'))
   {
    locon = cns_get_one_cone_external_connector(cone);
    ptype = getptype(locon->SIG->USER,LOFIGCHAIN) ;
    chain = NULL ;
    if(ptype != NULL)
      {
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
          {
           if((((locon_list *)chain->DATA)->TYPE == INTERNAL) &&
              ((((locon_list *)chain->DATA)->DIRECTION != CNS_I) &&
               (((locon_list *)chain->DATA)->DIRECTION != CNS_X)))
               break ;
          }
      }
    if(chain == NULL)
      {
       if(locon->DIRECTION == CNS_T)
        {
         locon->DIRECTION = CNS_I ;
        }
       else if((locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_O))
        {
         locon->DIRECTION = CNS_X ;
        }
      }
   }
 }
        
/*---------------------------------------------------------------------------*/
/* pour calculer FUP il faut prendre la branche la plus resistive entre      */
/* une externe et une VDD. Idem pour FDOWN, mais avec une branche VSS.       */
/* C'est la fonction decision() qui choisit la branche.                      */
/*---------------------------------------------------------------------------*/
tlc_setmaxfactor() ;

slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MAX)->DATA ;

if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0)
 {
  *fhh = TAS_NOFRONT ;
  *fll = TAS_NOFRONT ;
 }
else
 {
  if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
    *fhh = valfupfdownbis(decision(ext,vdd,cone->BREXT,cone->BRVDD),fcarac,ccarac) ;
  else
    *fhh = TAS_NOFRONT ;

  if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
    *fll = valfupfdownbis(decision(ext,vss,cone->BREXT,cone->BRVSS),fcarac,ccarac) ;
  else
    *fll = TAS_NOFRONT ;
 }

tlc_setminfactor() ;

/* calcul du front min */
if(TAS_PATH_TYPE == 'm')
{
if(cone->BREXT == NULL) pathext = NULL ;
else
 {
  for(pathext = cone->BREXT ; pathext->NEXT != NULL ; pathext = pathext->NEXT)
   {
      if((pathext->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        break ;
   }
  if((pathext->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
    pathext = NULL ;
 }

if(cone->BRVDD == NULL) pathvdd = NULL ;
else
 {
  for(pathvdd = cone->BRVDD ; pathvdd->NEXT != NULL ; pathvdd = pathvdd->NEXT)
   {
      if((pathvdd->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        break ;
   }
  if((pathvdd->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
    pathvdd = NULL ;
 }

if(cone->BRVSS == NULL) pathvss = NULL ;
else
 {
  for(pathvss = cone->BRVSS ; pathvss->NEXT != NULL ; pathvss = pathvss->NEXT)
   {
      if((pathvss->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
        break ;
   }
  if((pathvss->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
    pathvss = NULL ;
 }

slope = (front_list *)getptype(cone->USER,TAS_SLOPE_MIN)->DATA ;

if((cone->TYPE & (CNS_VDD | CNS_VSS)) != 0)
 {
  *fhh = TAS_NOFRONT ;
  *fll = TAS_NOFRONT ;
 }
else if(pathext == NULL) 
 {
  if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
    *fhh = valfupfdownbis(pathvdd,fcarac,ccarac) ;
  else
    *fhh = TAS_NOFRONT ;
  if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
    *fll = valfupfdownbis(pathvss,fcarac,ccarac) ;
  else
    *fll = TAS_NOFRONT ;
 }
else
 {
  if(pathvdd == NULL) 
   {
    if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
      *fhh = valfupfdownbis(pathext,fcarac,ccarac) ;
    else
      *fhh = TAS_NOFRONT ;
   }
  else 
   {
    if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
    *fhh = valfupfdownbis((((long)getptype(pathext->USER,TAS_RESIST)->DATA > 
        (long)getptype(pathvdd->USER,TAS_RESIST)->DATA) ? pathext : pathvdd),fcarac,ccarac) ;
    else
      *fhh = TAS_NOFRONT ;
   }
  if(pathvss == NULL)
    {
     if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
       *fll = valfupfdownbis(pathext,fcarac,ccarac) ;
     else
       *fll = TAS_NOFRONT ;
    }
  else 
   {
     if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
   *fll = valfupfdownbis((((long)getptype(pathext->USER,TAS_RESIST)->DATA > 
       (long)getptype(pathvss->USER,TAS_RESIST)->DATA) ? pathext : pathvss),fcarac,ccarac) ;
     else
       *fll = TAS_NOFRONT ;
   }
 }
}

    avt_log (LOGTAS, 4, "       front: ") ;
    if(((cone->BREXT)==NULL && (cone->BRVDD)==NULL && (cone->BRVSS)==NULL)) 
        avt_log (LOGTAS, 4, "no path\n") ;
    else
        {
        avt_log (LOGTAS, 4, "types of pathes are ") ;
        if(cone->BREXT != NULL) avt_log (LOGTAS, 4, "extern ") ;
        if(cone->BRVDD != NULL) avt_log (LOGTAS, 4, "VDD ") ;
        if(cone->BRVSS != NULL) avt_log (LOGTAS, 4, "VSS") ;
        avt_log (LOGTAS, 4, "\n") ;
        }

return(0) ;
}

/*----------------------------------------------------------------------------*/
/*                    tas_getslewparamsduo()                                  */
/*                                                                            */
/* extraction des parametres de front pour fll et fhh                         */
/* srcil: somme des rc intrinseques pour fll                                  */
/* srl  : somme des resistances pour fll                                      */
/* fll = srcil + srl * ccarac                                                 */
/* srcih: somme des rc intrinseques pour fhh                                  */
/* srh  : somme des resistances pour fhh                                      */
/* fhh = srcih + srh * ccarac                                                 */
/*----------------------------------------------------------------------------*/
long tas_getslewparamsduo(cone, srcil, srl, srcih, srh)
cone_list *cone;
double    *srcil;
double    *srl;
double    *srcih;
double    *srh;
{
    char         ext='n',
                 exl='n',
                 vdd='n',
                 vss='n' ;
    branch_list *pathext ;
    branch_list *pathvdd ;
    branch_list *pathvss ;
    locon_list  *locon ;
    ptype_list *ptype ;
    chain_list *chain ;

    /*---------------------------------------------------------------------------*/
    /* a ce niveau, les branches de chaque cone sont classees dans l'ordre       */
    /* decroissant et par type : EXT, VDD et VSS. Dans un premier temps on       */
    /* la branche la plus resistive pour chaque type (premiere branche de chaque */
    /* liste).                                                                   */
    /*---------------------------------------------------------------------------*/

   if(cone->BREXT != NULL)
   if((cone->BREXT->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
      {
       ext = 'o' ;
       for(pathext = cone->BREXT ; pathext != NULL ; pathext = pathext->NEXT)
         if((pathext->LINK->NEXT != NULL) &&
            ((pathext->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL))
          {
           exl = 'o' ;
           break ;
          }
       }
    if(cone->BRVDD != NULL)
        if((cone->BRVDD->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
            vdd = 'o' ;
    if(cone->BRVSS != NULL)
        if((cone->BRVSS->TYPE & CNS_NOT_FUNCTIONAL) != CNS_NOT_FUNCTIONAL)
            vss = 'o' ;

if((cone->TYPE & CNS_EXT) == CNS_EXT)
 {
  if((vdd == 'n') && (vss == 'n') && (exl == 'n'))
   {
    locon = cns_get_one_cone_external_connector(cone);
    ptype = getptype(locon->SIG->USER,LOFIGCHAIN) ;
    chain = NULL ;
    if(ptype != NULL)
      {
       for(chain = (chain_list *)ptype->DATA ; chain != NULL ; chain = chain->NEXT)
          {
           if((((locon_list *)chain->DATA)->TYPE == INTERNAL) &&
              ((((locon_list *)chain->DATA)->DIRECTION != CNS_I) &&
               (((locon_list *)chain->DATA)->DIRECTION != CNS_X)))
               break ;
          }
      }
    if(chain == NULL)
      {
       if(locon->DIRECTION == CNS_T)
        {
         locon->DIRECTION = CNS_I ;
        }
       else if((locon->DIRECTION == CNS_Z) || (locon->DIRECTION == CNS_O))
        {
         locon->DIRECTION = CNS_X ;
        }
      }
   }
 }
        
    /*---------------------------------------------------------------------------*/
    /* pour calculer FUP il faut prendre la branche la plus resistive entre      */
    /* une externe et une VDD. Idem pour FDOWN, mais avec une branche VSS.       */
    /* C'est la fonction decision() qui choisit la branche.                      */
    /*---------------------------------------------------------------------------*/


    if((cone->TYPE & (CNS_VDD | CNS_VSS)) == 0){
        if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
            tas_getslewparams(decision(ext,vdd,cone->BREXT,cone->BRVDD),srcih,srh,'U') ;

        if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
            tas_getslewparams(decision(ext,vss,cone->BREXT,cone->BRVSS),srcil,srl,'D') ;
    }

    /* calcul du front min */
    if(TAS_PATH_TYPE == 'm'){
        if(cone->BREXT == NULL) 
            pathext = NULL ;
        else{
            for(pathext = cone->BREXT ; pathext->NEXT != NULL ; pathext = pathext->NEXT){
                if((pathext->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
                    break ;
            }
            if((pathext->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
                pathext = NULL ;
        }
        if(cone->BRVDD == NULL) 
            pathvdd = NULL ;
        else{
            for(pathvdd = cone->BRVDD ; pathvdd->NEXT != NULL ; pathvdd = pathvdd->NEXT){
                if((pathvdd->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
                    break ;
            }
            if((pathvdd->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
                pathvdd = NULL ;
        }
        if(cone->BRVSS == NULL) 
            pathvss = NULL ;
        else{
            for(pathvss = cone->BRVSS ; pathvss->NEXT != NULL ; pathvss = pathvss->NEXT){
                if((pathvss->NEXT->TYPE & CNS_NOT_FUNCTIONAL) == CNS_NOT_FUNCTIONAL)
                    break ;
            }
            if((pathvss->TYPE & CNS_NOT_FUNCTIONAL)== CNS_NOT_FUNCTIONAL)
                pathvss = NULL ;
        }


        if((cone->TYPE & (CNS_VDD | CNS_VSS)) == 0){
            if(pathext == NULL){
                if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
                    tas_getslewparams(pathvdd,srcih,srh,'U') ;
                if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
                    tas_getslewparams(pathvss,srcil,srl,'D') ;
            }else{
                if(pathvdd == NULL){
                    if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
                        tas_getslewparams(pathext,srcih,srh,'U') ;
                }else{
                    if((cone->TYPE & TAS_NORISING) != TAS_NORISING)
                        tas_getslewparams((((long)getptype(pathext->USER,TAS_RESIST)->DATA > 
                                         (long)getptype(pathvdd->USER,TAS_RESIST)->DATA) ? pathext : pathvdd),srcih,srh,'U') ;
                }
                if(pathvss == NULL){
                    if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
                        tas_getslewparams(pathext,srcil,srl,'D') ;
                }else{
                    if((cone->TYPE & TAS_NOFALLING) != TAS_NOFALLING)
                        tas_getslewparams((((long)getptype(pathext->USER,TAS_RESIST)->DATA > 
                                         (long)getptype(pathvss->USER,TAS_RESIST)->DATA) ? pathext : pathvss),srcil,srl,'D') ;
                }
            }
        }
    }
    return(0) ;
}

/*----------------------------------------------------------------------------*/
/*                    valfupfdownbis()                                        */
/*                                                                            */
/* renvoie le front pour frontbis()                                           */
/*----------------------------------------------------------------------------*/
long valfupfdownbis(path, fcarac, ccarac)
branch_list *path;
long         fcarac;
double       ccarac;
{
    link_list *link;
    long       slope = 0;
    double     res,
               capa  = 0.0,
               restrans = 0.0;
    int        passe = 0;
    int        mode  = 0;
/*----------------------------------------------------------------------------*/
/* si il n'y a pas de maillon, il n'y a pas de front=>TAS_NOFRONT.            */
/* si il n`y a qu'un seul maillon et que c'est un connecteur => TAS_CONTEXT->FRONT_CON.    */
/* sinon on calcule le front du a la branche.                                 */
/*----------------------------------------------------------------------------*/
if(path == NULL) /* aucun maillon */
    {
    avt_log (LOGTAS, 4, "       valfupfdown: no slope\n" ) ;
    return(TAS_NOFRONT) ; 
    }
link = (link_list *)path->LINK ;

if(link->NEXT == NULL) /* si un seul maillon sur la branche */
    {
    if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) /* si connecteur => front connecteur */
        {
        avt_log (LOGTAS, 4, "       valfupfdown: value of slope is %ld", (long)fcarac) ;
        avt_log (LOGTAS, 4, " -connector slope-\n") ;
        if(fcarac)
            return((long)fcarac) ;
        else
            return TAS_CONTEXT->FRONT_CON;
       
        }
    }

while(link->NEXT != NULL) /* il existe des maillons transistors */
    {
        if (!passe)
    {
        capa+=ccarac;
        passe=1;
    }
        
    res = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R);

    restrans = (res*(double)TAS_GETLENGTH(link) /
                          (double)TAS_GETWIDTH(link)) ;

    if(slope != 0)
    restrans = (restrans * 0.95) ;

    if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
    restrans = (restrans * 0.5) ; 

    if(slope == 0)
    {
            capa += (tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                         tas_getcapabl(link)) ;
    
    }
    else
            capa += (0.95*(tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                            tas_getcapabl(link))) ;

    slope += (long)(restrans*capa*tlc_getcapafactor()) ;
    link = link->NEXT ;
    mode=0;
    }

if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0) /* si dernier maillon PAS connecteur */
    {
    if (!passe)
    {
        capa+=ccarac;
        passe=1;
    }

    res = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R);

    restrans = (res*(double)TAS_GETLENGTH(link) /
                          (double)TAS_GETWIDTH(link)) ;

    if(slope != 0)
    restrans = (restrans * 0.95) ;

    if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
    restrans = (restrans * 0.5) ;

    if(slope==0L)
    {
            capa += (tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                          tas_getcapabl(link)) ;
    }
    else
            capa += (0.95*(tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + 
                                            tas_getcapabl(link))) ;

    slope += (long)(restrans*capa*tlc_getcapafactor()) ;
    }
/* si dernier maillon connecteur, alors pas touche! */
        
slope = (long)(slope  / 1000) ;

if(slope == 0) /* front nul => erreur: plus tard cause des problemes ! */
 {
  tas_error(1,NULL,TAS_WARNING) ;
  slope = 1 ;
 }

avt_log (LOGTAS, 3, "       valfupfdown: value of slope is %ld\n",slope) ;

return(slope) ;
}
/*----------------------------------------------------------------------------*/
/*                    tas_getslewparams()                                     */
/*                                                                            */
/* extraction des parametres srci et sr pour tas_getslewparamsduo()           */
/*----------------------------------------------------------------------------*/
void tas_getslewparams(path, srci, sr, sens)
branch_list *path;
double      *srci;
double      *sr;
char         sens;
{
    link_list *link;
    long      slopei   = 0;
    double    sres     = 0.0, 
              res, 
              capa     = 0.0, 
              restrans = 0.0;
   
    if(path == NULL) /* aucun maillon */
        return ; 
    link = (link_list *)path->LINK ;
    
    if(link->NEXT == NULL) /* si un seul maillon sur la branche */
        if((link->TYPE & (CNS_IN | CNS_INOUT)) != 0) /* si connecteur => front connecteur */
             slopei = tas_get_pinslew(link->ULINK.LOCON, sens) * 1000;
   
    while(link->NEXT != NULL){ /* il existe des maillons transistors */
      
        res = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R);
   
        restrans = (res*(double)TAS_GETLENGTH(link) / (double)TAS_GETWIDTH(link)) ;
   
        if(sres != 0.0)
            restrans = (restrans * 0.95) ;
   
        if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
            restrans = (restrans * 0.5) ; 
   
        if(sres == 0.0)
            capa += (/*link->CAPA +*/ TAS_GETCAPARA(link) + tas_getcapabl(link)) ;
        else
            capa += (0.95*(tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + tas_getcapabl(link))) ;
   
        slopei += (long)(restrans*capa*tlc_getcapafactor()) ;
        sres += restrans;
        link = link->NEXT ;
    }
   
    if((link->TYPE & (CNS_IN | CNS_INOUT)) == 0){ /* si dernier maillon PAS connecteur */
       
        res = tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_R);
   
        restrans = (res*(double)TAS_GETLENGTH(link) /
                             (double)TAS_GETWIDTH(link)) ;
   
        if(sres != 0.0)
            restrans = (restrans * 0.95) ;
   
        if((link->TYPE & CNS_SWITCH) == CNS_SWITCH)
            restrans = (restrans * 0.5) ;
   
        if(sres == 0.0)
            capa += (/*link->CAPA +*/ TAS_GETCAPARA(link) + tas_getcapabl(link)) ;
        else
            capa += (0.95*(tas_getcapalink(NULL, link,TAS_UNKNOWN_EVENT) + TAS_GETCAPARA(link) + tas_getcapabl(link))) ;
   
        slopei += (long)(restrans*capa*tlc_getcapafactor()) ;
        sres += restrans;    
    }
    /* si dernier maillon connecteur, alors pas touche! */
           
    *srci = (double)(slopei  / 1000) ;
    *sr   = sres    / 1000.0 ;
}

/*----------------------------------------------------------------------------*/
/*                    tas_DupTab()                                            */
/*----------------------------------------------------------------------------*/
double *tas_DupTab (double *tab, int n)
{
	int i ;
	double *duptab = (double*)mbkalloc (n * sizeof (double)) ;

	for (i = 0 ; i < n ; i++)
		duptab[i] = tab[i] ;

	return duptab ;
}

/*----------------------------------------------------------------------------*/
/*                    tas_SetSlopeList()                                      */
/*----------------------------------------------------------------------------*/
void tas_SetSlopeList (locon_list *locon, inf_carac *defaultslope, ht *htslope)
{
	inf_carac     *caracslope = NULL ;
	ptype_list    *ptype ;
	caraccon_list *carac ;
	
	ptype = getptype (locon->USER, TAS_CON_CARAC) ; 
	carac = ptype ?  (caraccon_list*)ptype->DATA : NULL ;
	if (!carac)
		return ;
	
	caracslope = (inf_carac*)gethtitem (htslope, locon->NAME) ;
	if ((long)caracslope == EMPTYHT)
		caracslope = defaultslope ;

	if (!caracslope) { 
		carac->NSLOPE = TAS_CONTEXT->CARAC_VALUES * 2 + 1 ;
		carac->SLOPEIN = tas_DynamicSlopes ((tas_get_pinslew(locon, 'U') + tas_get_pinslew(locon, 'D'))/2.0/TTV_UNIT , TAS_CONTEXT->CARAC_VALUES) ;
	} else {
		carac->NSLOPE =  caracslope->NVALUES ;
		carac->SLOPEIN = tas_DupTab (caracslope->VALUES, carac->NSLOPE) ;
	}
}

/*----------------------------------------------------------------------------*/
/*                    tas_SetCapaList()                                       */
/*----------------------------------------------------------------------------*/
void tas_SetCapaList (locon_list *locon, inf_carac *defaultcapa, ht *htcapa)
{
	inf_carac     *caraccapa = NULL ;
	ptype_list    *ptype ;
	caraccon_list *carac ;
	cone_list     *cone ;
	link_list     *link ;
	double         lwvss, lwvdd, lwext, lw ;
	double         rmoy;
	int            i ;

	ptype = getptype (locon->USER, TAS_CON_CARAC) ; 
	carac = ptype ?  (caraccon_list*)ptype->DATA : NULL ;
	if (!carac)
		return ;

	caraccapa  = (inf_carac*)gethtitem (htcapa, locon->NAME) ;
	if ((long)caraccapa == EMPTYHT)
		caraccapa = defaultcapa ;
	
	if (!caraccapa) {
	    rmoy = (carac->RUPMAX + carac->RDOWNMAX + carac->RUPMIN + carac->RDOWNMIN) / 4 ;
		carac->CAPAOUT = tas_DynamicCapas (rmoy, TAS_CONTEXT->CARAC_VALUES) ;
		carac->NCAPA = TAS_CONTEXT->CARAC_VALUES * 2 + 1 ;
		return ;
	}

	carac->NCAPA = caraccapa->NVALUES ;
	carac->CAPAOUT = tas_DupTab (caraccapa->VALUES, carac->NCAPA) ;
	if (caraccapa->LW > 0) {
		ptype = getptype (locon->USER, CNS_EXT) ;
		cone = ptype ? (cone_list*)ptype->DATA : NULL ;
		if (cone) {
			lwvdd = 0 ; 
			lwvss = 0 ; 
			lwext = 0 ; 
			if (cone->BRVDD) {
				for (link = cone->BRVDD->LINK ; link ; link = link->NEXT){
                    if(((link->TYPE & CNS_IN) != CNS_IN) && ((link->TYPE & CNS_INOUT) != CNS_INOUT)) {
                        lwvdd += ((double)TAS_GETLENGTH(link) 
                                 - tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_DL)*1000) 
                                 / ((double)TAS_GETCLINK(link)->WIDTH 
                                 - tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_DW)*1000) ;
                    }
                }
            }
			if (cone->BRVSS) {
				for (link = cone->BRVSS->LINK ; link ; link = link->NEXT){
                    if(((link->TYPE & CNS_IN) != CNS_IN) && ((link->TYPE & CNS_INOUT) != CNS_INOUT)) {
                        lwvss += ((double)TAS_GETLENGTH(link) 
                                 - tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_DL)*1000) 
                                 / ((double)TAS_GETCLINK(link)->WIDTH 
                                 - tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_DW)*1000) ;
                    }
                }
            }
			if (cone->BREXT) {
				for (link = cone->BRVSS->LINK ; link ; link = link->NEXT){
                    if(((link->TYPE & CNS_IN) != CNS_IN) && ((link->TYPE & CNS_INOUT) != CNS_INOUT)) {
                        lwext += ((double)TAS_GETLENGTH(link) 
                                 - tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_DL)*1000) 
                                 / ((double)TAS_GETCLINK(link)->WIDTH 
                                 - tas_getparam(link->ULINK.LOTRS,TAS_CASE,TP_DW)*1000) ;
                    }
                }
            }
			lw = lwvdd > lwvss ? lwvdd : lwvss ;
			lw = lw > lwext ? lw : lwext ; 
			
			for (i = 0 ; i < carac->NCAPA ; i++)
				carac->CAPAOUT[i] *= caraccapa->LW / lw ;
		}
	} 
}

/*----------------------------------------------------------------------------*/
/*                    tas_AffectCaracCon()                                    */
/*----------------------------------------------------------------------------*/

inf_carac     *inftocarac(chain_list *ch, float factor)
{
  inf_carac     *stmcaracslope;
  int i;
  stmcaracslope = (inf_carac*)mbkalloc (sizeof (inf_carac));
  stmcaracslope->VALUES = (double*)mbkalloc (countchain(ch) * sizeof (double)) ;
  for (i = 0 ; ch!=NULL ; i++, ch=ch->NEXT)
    stmcaracslope->VALUES[i] = ((inf_assoc *)ch->DATA)->dval*factor;
  stmcaracslope->NVALUES = i;
  return stmcaracslope;
}

void tas_AffectCaracCon (cnsfig_list *cnsfig)
{
  ht         *htslope ;
  ht         *htcapa ;
  inf_carac  *defaultslope ;
  inf_carac  *defaultcapa ;
  locon_list *locon ;
  locon_list *tabcon[2] ;
  int i ;
  inffig_list *ifl;
  chain_list *cl, *list0, *ch;
  char *name;
  double l, lw;

  htslope = addht (1000) ;
  htcapa  = addht (1000) ;
  
  ifl=getloadedinffig(cnsfig->NAME);
  
  list0=inf_GetEntriesByType(ifl, INF_SLOPEIN, INF_ANY_VALUES);
  for (cl=list0; cl!=NULL; cl=cl->NEXT) 
    {
      name=(char *)cl->DATA;
      inf_GetPointer(ifl, name, INF_SLOPEIN, (void **)&ch);
      addhtitem (htslope, name, (long)inftocarac(ch, 1e12));
    }
  freechain(list0);

  list0=inf_GetEntriesByType(ifl, INF_CAPAOUT, INF_ANY_VALUES);
  for (cl=list0; cl!=NULL; cl=cl->NEXT) 
    {
      name=(char *)cl->DATA;
      inf_GetPointer(ifl, name, INF_CAPAOUT, (void **)&ch);
      defaultcapa=inftocarac(ch, 1e15);
      if (inf_GetDouble(ifl, name, INF_CAPAOUT_LW, &lw))
        {
          inf_GetDouble(ifl, name, INF_CAPAOUT_L, &l);
          defaultcapa->LW=lw;
          defaultcapa->L=l;
        }
      else
        defaultcapa->LW=-1;
      addhtitem (htcapa, name, (long)defaultcapa);
    }
  freechain(list0);

  defaultslope = (inf_carac*)gethtitem (htslope, namealloc ("default")) ;
  if ((long)defaultslope == EMPTYHT)
    defaultslope = NULL ;
  
  defaultcapa = (inf_carac*)gethtitem (htcapa, namealloc ("default")) ;
  if ((long)defaultcapa == EMPTYHT)
    defaultcapa = NULL ;
  
  tabcon[0] = cnsfig->LOCON ;
  tabcon[1] = cnsfig->INTCON ;
  
  for (i = 0 ; i < 2 ; i++ )
    for (locon = tabcon[i]  ; locon ; locon = locon->NEXT) {
      
      if (locon->DIRECTION == CNS_I) 
        tas_SetSlopeList (locon, defaultslope, htslope) ;
      
      if (locon->DIRECTION == CNS_T || locon->DIRECTION == CNS_B) {
        tas_SetSlopeList (locon, defaultslope, htslope) ;
        tas_SetCapaList (locon, defaultcapa, htcapa) ;
      }
      
      if (locon->DIRECTION == CNS_O) 
        tas_SetCapaList (locon, defaultcapa, htcapa) ;
      
      if (locon->DIRECTION == CNS_Z) 
        tas_SetCapaList (locon, defaultcapa, htcapa) ;
    }
		
#if 0	
       //for (inflist = INF_SIGLIST ; inflist ; inflist = inflist->NEXT) {
    for (cl=maincl; cl!=NULL; cl=cl->NEXT) {
        inflist=(list_list *)cl->DATA;
		if (inflist->TYPE == INF_LL_CAPAOUT) {
            mbkfree(((inf_carac*)inflist->USER)->VALUES);
			mbkfree(inflist->USER) ;	
        }
		if (inflist->TYPE == INF_LL_SLOPEIN) {
            mbkfree(((inf_carac*)inflist->USER)->VALUES);
			mbkfree(inflist->USER) ;	
        }
	}
#endif	
    cl=GetAllHTElems(htslope);
    cl=append(cl, GetAllHTElems(htcapa));
    ch=cl;
    while (cl!=NULL)
      {
        mbkfree(((inf_carac*)cl->DATA)->VALUES);
        mbkfree(cl->DATA);
        cl=cl->NEXT;
      }
    freechain(ch);

    delht (htslope) ;
    delht (htcapa) ;
}

/*----------------------------------------------------------------------------*/
/*                    autocarac()                                             */
/*----------------------------------------------------------------------------*/
double *autocarac (cone_list *cone, int nccaracio, double *ccaracio) 
{
int i;
long fll,fhh;
double *fcaracio;
fcaracio=(double*)mbkalloc(nccaracio * sizeof(double));
for(i = 0; i < nccaracio; i++){
	frontbis(cone,TAS_CONTEXT->FRONT_CON, ccaracio[i]-(tas_get_cone_output_capacitance(cone)/*TAS_CONTEXT->TAS_CAPAOUT*/*1000), &fll, &fhh);
	fcaracio[i]=((fhh+fll)/2);
}
return fcaracio;
}

/*----------------------------------------------------------------------------*/
/*                    round()                                                 */
/*----------------------------------------------------------------------------*/
double pround (double d, int precision)
{
	long   l ;
	int    i = 0 ;
	double dp ;
	int    cs ;

        if( d == 0.0 )
          return d ;

	dp = d < 0 ? -d : d ;

	cs = (int)pow ((double)10, (double)precision) ;
	if (dp >= cs) {
		l = (long)dp ;
		while (l / cs) {
			l /= 10 ;
			i++ ;
		}
		while (i--) 
			l *= 10 ;
		if (d < 0)
			return -(double)l ;
		else
			return (double)l ;
	} else {
		cs /= 10 ; 
		while (dp < cs) {
			dp *= 10 ;
			i++ ;
		}
		l = (long)dp ;
		if (l == LONG_MAX)
			fprintf (stderr, "[WARNING] : round overflow\n") ;
		dp = (double)l ;
		while (i--)
			dp /= 10 ;
		if (d < 0)
			return -dp ;
		else
			return dp ;
	}
}

/*----------------------------------------------------------------------------*/
/*                    tas_DynamicSlopes()                                     */
/*----------------------------------------------------------------------------*/
double *tas_DynamicSlopes (long typslope, int n)
{
	int i ;
	double *slopes ;

	slopes = (double*)mbkalloc ((2 * n + 1) * sizeof (double)) ;
	for (i = 0 ; i < 2 * n + 1 ; i++)
		slopes[i] = pround (typslope / pow ((double)((pow(n-5,2)+12)/10), (double)(n - i)), CARAC_PREC) ;

	return slopes ;
}

/*----------------------------------------------------------------------------*/
/*                    tas_DynamicCapas()                                      */
/*----------------------------------------------------------------------------*/
double *tas_DynamicCapas (double rmoy, int n)
{
	int i ;
	double *capas ;
    double *slopes ;

	slopes = tas_DynamicSlopes (TAS_CONTEXT->FRONT_CON, TAS_CONTEXT->CARAC_VALUES) ;

	capas = (double*)mbkalloc ((2 * n + 1) * sizeof (double)) ;
    if(rmoy < 0.1) rmoy = 1;
	for (i = 0 ; i < 2 * TAS_CONTEXT->CARAC_VALUES + 1 ; i++)
		capas[i] = pround (slopes[i] / rmoy, CARAC_PREC) * 1000 ;

    mbkfree(slopes);

	return capas ;
}

/*----------------------------------------------------------------------------*/
/*                    tas_Compute_Delay()                                     */
/*----------------------------------------------------------------------------*/
void tas_Compute_Delay(cone_list *cone0, edge_list *input0, long fcarac, double ccarac, double ctyp, delay_list **delay0, delay_list **delay1, double *capa)
{
    static int tas_counter_model=0;
    tas_counter_model++;
    if(((input0->TYPE & (CNS_FEEDBACK|TAS_IN_MEMSYM))==0) ||
       ((input0->TYPE & TAS_IN_NOTMEMSYM) == TAS_IN_NOTMEMSYM)) {
        if ((input0->TYPE &CNS_CONE)==CNS_CONE) {
            if(TAS_PATH_TYPE == 'm') {
                *delay0 = (delay_list *)getptype(input0->USER,TAS_DELAY_MIN)->DATA ;
                *delay1 = (delay_list *)getptype(input0->USER,TAS_DELAY_MAX)->DATA ;
            } else
                *delay0 = (delay_list *)getptype(input0->USER,TAS_DELAY_MAX)->DATA ;
            if((cone0->TECTYPE & CNS_DUAL_CMOS) == CNS_DUAL_CMOS)
    			*delay0=val_dual(cone0,(cone_list *)input0->UEDGE.CONE, *delay0,fcarac,ccarac-ctyp);
            else
    			*delay0=val_non_dual(cone0,(cone_list *)input0->UEDGE.CONE, *delay0,fcarac,ccarac-ctyp);
        } else {
            if ((input0->TYPE & CNS_EXT)==CNS_EXT) {
                if(TAS_PATH_TYPE == 'm') {
                    *delay0 = (delay_list *)getptype(input0->USER,TAS_DELAY_MIN)->DATA ;
                    *delay1 = (delay_list *)getptype(input0->USER,TAS_DELAY_MAX)->DATA ;
                } else
                    *delay0 = (delay_list *)getptype(input0->USER,TAS_DELAY_MAX)->DATA ;
    			*delay0=val_con(cone0,(locon_list *)input0->UEDGE.LOCON, *delay0,fcarac,ccarac-ctyp); 
            }
        }
    }
    if(cone0->BRVDD) 
        *capa = (tas_getcapalink(NULL, cone0->BRVDD->LINK,TAS_UNKNOWN_EVENT) + ccarac-ctyp)* tlc_getcapafactor() ;
    else if(cone0->BRVSS) 
        *capa = (tas_getcapalink(NULL, cone0->BRVSS->LINK,TAS_UNKNOWN_EVENT) + ccarac-ctyp)* tlc_getcapafactor() ;
    else if(cone0->BREXT) 
        *capa = (tas_getcapalink(NULL, cone0->BREXT->LINK,TAS_UNKNOWN_EVENT) + ccarac-ctyp)* tlc_getcapafactor() ;
} 

