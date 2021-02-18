/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_drislo.c                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* driver des fronts                                                        */
/****************************************************************************/

#include "tas.h"

/****************************************************************************/
/*                            fonction tas_drislo()                         */ 
/*                    pour driver les fronts de signaux                     */
/****************************************************************************/
int tas_drislo(cnsfig,ttvfig)
cnsfig_list *cnsfig ;
ttvfig_list *ttvfig ;
{
cone_list *cone ;
edge_list *incone ;
locon_list *locon ;
FILE      *file ;
time_t    counter;
char      buffer[1024];

if(cnsfig != NULL)
  file = mbkfopen(cnsfig->NAME,"slo",WRITE_TEXT) ;
else
  file = mbkfopen(ttvfig->INFO->FIGNAME,"slo",WRITE_TEXT) ;

if(file == NULL)
    {
    char straux[100] ;

    if(cnsfig != NULL)
      sprintf(straux,"%s.slo",cnsfig->NAME) ;
    else
      sprintf(straux,"%s.slo",ttvfig->INFO->FIGNAME) ;
    tas_error(8,straux,TAS_WARNING) ;

    return(0) ;
    }

/* entete du fichier ".slo" */
sprintf(buffer,"List of slopes : %s.slo\n",cnsfig->NAME);
avt_printExecInfo(file, "#", buffer, "");

if(cnsfig != NULL)
 {
  for(locon = cnsfig->LOCON ; locon != NULL ; locon = locon->NEXT)
    {
      ptype_list *user = getptype(locon->USER,TAS_SLOPE_MAX) ;
      ptype_list *usermin = getptype(locon->USER,TAS_SLOPE_MIN) ;

      if(user != NULL)
          {
          front_list *slope = (front_list *)user->DATA ;
    
          fprintf(file,"locon (%s)\t: FUPMAX = ",locon->NAME) ;
          if(slope->FUP == TAS_NOFRONT) fprintf(file,"NO SLOPE") ;
          else fprintf(file,"%ld",slope->FUP) ;
          fprintf(file,"\tFDOWNMAX = ") ;
          if(slope->FDOWN == TAS_NOFRONT) fprintf(file,"NO SLOPE ;\n") ;
          else fprintf(file,"%ld ;\n",slope->FDOWN) ;
          }
      if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
        {
         if(usermin != NULL)
            {
            front_list *slope = (front_list *)usermin->DATA ;
  
            fprintf(file,"locon (%s)\t: FUPMIN = ",locon->NAME) ;
            if(slope->FUP == TAS_NOFRONT) fprintf(file,"NO SLOPE") ;
            else fprintf(file,"%ld",slope->FUP) ;
            fprintf(file,"\tFDOWNMIN = ") ;
            if(slope->FDOWN == TAS_NOFRONT) fprintf(file,"NO SLOPE ;\n") ;
            else fprintf(file,"%ld ;\n",slope->FDOWN) ;
          }
        }
    }
  for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
    {
      ptype_list *user = getptype(cone->USER,TAS_SLOPE_MAX) ;
      ptype_list *usermin = getptype(cone->USER,TAS_SLOPE_MIN) ;
 
      if(user != NULL)
          {
          front_list *slope = (front_list *)user->DATA ;
    
          fprintf(file,"cone (%s)\t: FUPMAX = ",cone->NAME) ;
          if(slope->FUP == TAS_NOFRONT) fprintf(file,"NO SLOPE") ;
          else fprintf(file,"%ld",slope->FUP) ;
          fprintf(file,"\tFDOWNMAX = ") ;
          if(slope->FDOWN == TAS_NOFRONT) fprintf(file,"NO SLOPE ;\n") ;
          else fprintf(file,"%ld ;\n",slope->FDOWN) ;
          }
      if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
        {
         if(usermin != NULL)
            {
            front_list *slope = (front_list *)usermin->DATA ;
  
            fprintf(file,"cone (%s)\t: FUPMIN = ",cone->NAME) ;
            if(slope->FUP == TAS_NOFRONT) fprintf(file,"NO SLOPE") ;
            else fprintf(file,"%ld",slope->FUP) ;
            fprintf(file,"\tFDOWNMIN = ") ;
            if(slope->FDOWN == TAS_NOFRONT) fprintf(file,"NO SLOPE ;\n") ;
            else fprintf(file,"%ld ;\n",slope->FDOWN) ;
          }
        }
     for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
      {
       user = getptype(incone->USER,TAS_SLOPE_MAX) ;
       usermin = getptype(incone->USER,TAS_SLOPE_MIN) ;
       if(user != NULL)
          {
          front_list *slope = (front_list *)user->DATA ;
   
          fprintf(file,"cone (%s)\t input (%s)\t: FUPMAX = ",cone->NAME,
          ((incone->TYPE & CNS_EXT) == CNS_EXT) ? incone->UEDGE.LOCON->NAME : 
                                                  incone->UEDGE.CONE->NAME) ;
          if(slope->FUP == TAS_NOFRONT) fprintf(file,"NO SLOPE") ;
          else fprintf(file,"%ld",slope->FUP) ;
          fprintf(file,"\tFDOWNMAX = ") ;
          if(slope->FDOWN == TAS_NOFRONT) fprintf(file,"NO SLOPE ;\n") ;
          else fprintf(file,"%ld ;\n",slope->FDOWN) ;
          }
      if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
        {
         if(usermin != NULL)
           {
            front_list *slope = (front_list *)usermin->DATA ;
 
            fprintf(file,"cone (%s)\t input (%s)\t : FUPMIN = ",cone->NAME,
            ((incone->TYPE & CNS_EXT) == CNS_EXT) ? incone->UEDGE.LOCON->NAME : 
                                                  incone->UEDGE.CONE->NAME) ;
            if(slope->FUP == TAS_NOFRONT) fprintf(file,"NO SLOPE") ;
            else fprintf(file,"%ld",slope->FUP) ;
            fprintf(file,"\tFDOWNMIN = ") ;
            if(slope->FDOWN == TAS_NOFRONT) fprintf(file,"NO SLOPE ;\n") ;
            else fprintf(file,"%ld ;\n",slope->FDOWN) ;
          }
        }
      }
    }
 }
else if(ttvfig != NULL)
 {
  ttvsig_list *ptsig ;
  chain_list *chain ;
  chain_list *chainx ;
  long i ;

  chain = NULL ;

  for(i = 0 ; i < ttvfig->NBCONSIG ; i++)
    chain = addchain(chain,(void*)(*(ttvfig->CONSIG + i))) ;
  
  for(i = 0 ; i < ttvfig->NBNCSIG ; i++)
    chain = addchain(chain,(void*)(*(ttvfig->NCSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBILCMDSIG ; i++)
   if(((*(ttvfig->ILCMDSIG + i))->ROOT == ttvfig) && 
      (((*(ttvfig->ILCMDSIG + i))->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0))
      chain = addchain(chain,(void*)(*(ttvfig->ILCMDSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBELCMDSIG ; i++)
   if(((*(ttvfig->ELCMDSIG + i))->ROOT == ttvfig) && 
      (((*(ttvfig->ELCMDSIG + i))->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0))
      chain = addchain(chain,(void*)(*(ttvfig->ELCMDSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBILATCHSIG ; i++)
   if(((*(ttvfig->ILATCHSIG + i))->ROOT == ttvfig) && 
      (((*(ttvfig->ILATCHSIG + i))->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0))
      chain = addchain(chain,(void*)(*(ttvfig->ILATCHSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBELATCHSIG ; i++)
   if(((*(ttvfig->ELATCHSIG + i))->ROOT == ttvfig) && 
      (((*(ttvfig->ELATCHSIG + i))->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0))
      chain = addchain(chain,(void*)(*(ttvfig->ELATCHSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBIPRESIG ; i++)
   if(((*(ttvfig->IPRESIG + i))->ROOT == ttvfig) && 
      (((*(ttvfig->IPRESIG + i))->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0))
      chain = addchain(chain,(void*)(*(ttvfig->IPRESIG + i))) ;

  for(i = 0 ; i < ttvfig->NBEPRESIG ; i++)
   if(((*(ttvfig->EPRESIG + i))->ROOT == ttvfig) && 
      (((*(ttvfig->EPRESIG + i))->TYPE & (TTV_SIG_N | TTV_SIG_C)) == 0))
      chain = addchain(chain,(void*)(*(ttvfig->EPRESIG + i))) ;

  for(i = 0 ; i < ttvfig->NBINTSIG ; i++)
   if((*(ttvfig->INTSIG + i))->ROOT == ttvfig)
      chain = addchain(chain,(void*)(*(ttvfig->INTSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBEXTSIG ; i++)
   if((*(ttvfig->EXTSIG + i))->ROOT == ttvfig)
      chain = addchain(chain,(void*)(*(ttvfig->EXTSIG + i))) ;

  for(i = 0 ; i < ttvfig->NBESIG ; i++)
   if((*(ttvfig->ESIG + i))->ROOT == ttvfig)
      chain = addchain(chain,(void*)(*(ttvfig->ESIG + i))) ;

   if(ttvfig->NBISIG != 0)
       {
        long j ;
        long nbfree ;
        long nbend ;
        ttvsbloc_list *ptsbloc = ttvfig->ISIG ;
        nbfree = (TTV_MAX_SBLOC - (ttvfig->NBISIG % TTV_MAX_SBLOC))
                  % TTV_MAX_SBLOC ;
        nbend = ttvfig->NBISIG + nbfree ;
        for(i = nbfree ; i < nbend ; i++)
         {
          j = i % TTV_MAX_SBLOC ;
          if((j == 0) && (i != 0)) ptsbloc = ptsbloc->NEXT ;
          chain = addchain(chain,(void*)(ptsbloc->SIG + j)) ;
         }
       }

  for(chainx = chain ; chainx != NULL ; chainx = chainx->NEXT)
   {
    ptsig = (ttvsig_list *)chainx->DATA ;
    if(TAS_CONTEXT->TAS_FIND_MIN == 'Y')
      {
      }
   }
  freechain(chain) ;
 }

if(fclose(file) != 0)
    {
    char straux[100] ;

    if(cnsfig != NULL)
      sprintf(straux,"%s.slo",cnsfig->NAME) ;
    else
      sprintf(straux,"%s.slo",ttvfig->INFO->FIGNAME) ;

    tas_error(10,straux,TAS_WARNING) ;
    return(0) ;
    }

return(1) ;
}

/****************************************************************************/
/*                            fonction tas_drivrcfile()                     */
/*                    driver les delais rc des portes                       */
/****************************************************************************/
int tas_drivrcfile(cnsfig)
cnsfig_list *cnsfig ;
{
edge_list *incone ;
locon_list *locon ;
cone_list *cone ;
delay_list *delay ;
delay_list *delaymin ;
delay_list *delaymax ;
ptype_list *ptype ;
long timemax ;
long timemin ;
char *name1,*name2 ;
char *type1,*type2 ;
time_t    counter;
char buffer[1024];
char flag ;

FILE      *file ;
#ifdef USEOLDTEMP

if(cnsfig != NULL)
  file = mbkfopen(TAS_CONTEXT->TAS_RC_FILE,"rc",WRITE_TEXT) ;
else
  return(0) ;

if(file == NULL)
    {
    char straux[100] ;

    sprintf(straux,"%s.rc",TAS_CONTEXT->TAS_RC_FILE) ;
    tas_error(8,straux,TAS_WARNING) ;

    return(0) ;
    }

time(&counter);

sprintf(buffer,"List of interconnect delays : %s.rc\n",cnsfig->NAME);
avt_printExecInfo(file, "#", buffer, "");

fprintf(file,"\n\n") ;

 for(cone = cnsfig->CONE ; cone != NULL ; cone = cone->NEXT)
  {
   name2 = cone->NAME ;
   type2= "GATE" ;
   for(incone = cone->INCONE ; incone != NULL ; incone = incone->NEXT)
    {
     if((incone->TYPE & (CNS_FEEDBACK | CNS_VDD | CNS_VSS |
           CNS_BLEEDER | TAS_PATH_INOUT | TAS_EXT_IN | TAS_EXT_OUT)) != 0)
       continue ;
     delay = NULL ;
     delaymax = NULL ;
     delaymin = NULL ;
     if((incone->TYPE & CNS_EXT) == CNS_EXT)
      {
       name1 = incone->UEDGE.LOCON->NAME ;
       if(incone->UEDGE.LOCON->DIRECTION == CNS_I)
        type1 = "CONNECTOR" ;
       else
        type1 = "GATE" ;
      }
     else
      {
       if((incone->UEDGE.CONE->TYPE & CNS_EXT) == CNS_EXT)
        {
         chain_list *cl;
         type1="CONNECTOR" ;
         cl=cns_get_cone_external_connectors(incone->UEDGE.CONE);
//         ptype = getptype(incone->UEDGE.CONE->USER,CNS_EXT) ;
         if(cl!=NULL/*ptype != NULL*/)
           if(((locon_list *)cl->DATA)->DIRECTION != CNS_I)
            type1 = "GATE" ;
        }
       else
         type1="GATE" ;
       name1 = incone->UEDGE.CONE->NAME ;
      }
     flag = 'N' ;
     timemax = TAS_NOTIME ;
     timemin = TAS_NOTIME ;
     if((ptype = getptype(incone->USER,TAS_DELAY_MIN)) != NULL)
      {
       delay = (delay_list *)ptype->DATA ;
       delaymin = delay ;
       if(((delay->RCLL != TAS_NOTIME) && ((delay->TPLH != TAS_NOTIME) || 
                                           (delay->TPLL != TAS_NOTIME) ||
                          ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))) || 
          ((delay->RCHH != TAS_NOTIME) && ((delay->TPHL != TAS_NOTIME) || 
                                           (delay->TPHH != TAS_NOTIME) ||
                          ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))))
         {
          if(delay->RCLL != TAS_NOTIME) 
             timemin = delay->RCLL ;
          else
             timemin = delay->RCHH ;
          flag = 'Y' ;
         }
      }
     if((ptype = getptype(incone->USER,TAS_DELAY_MAX)) != NULL)
      {
       delay = (delay_list *)ptype->DATA ;
       delaymax = delay ;
       if(((delay->RCLL != TAS_NOTIME) && ((delay->TPLH != TAS_NOTIME) || 
                                           (delay->TPLL != TAS_NOTIME) ||
                       ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))) || 
          ((delay->RCHH != TAS_NOTIME) && ((delay->TPHL != TAS_NOTIME) || 
                                           (delay->TPHH != TAS_NOTIME) ||
                       ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))))
         {
          if(delay->RCLL != TAS_NOTIME) 
             timemax = delay->RCLL ;
          else
             timemax = delay->RCHH ;
          flag = 'Y' ;
         }
      }
     if(flag == 'Y')
      {
       fprintf(file,"%s %s %s %s ",type1,name1,type2,name2) ;
       if(timemin != TAS_NOTIME)
         fprintf(file,"TRCMIN=%ld ",timemin) ;
       if(timemax != TAS_NOTIME)
         fprintf(file,"TRCMAX=%ld ",timemax) ;
      }

     if((delaymin != NULL) && ((incone->TYPE & CNS_COMMAND) != CNS_COMMAND))
        {
         if((flag == 'N') &&
            ((delaymin->TPLH != TAS_NOTIME) || (delaymin->TPLL != TAS_NOTIME) ||
             (delaymin->TPHL != TAS_NOTIME) || (delaymin->TPHH != TAS_NOTIME)))
          {
           fprintf(file,"%s %s %s %s ",type1,name1,type2,name2) ;
           flag = 'Y' ;
          }
         if((delaymin->SHH != TAS_NOS) && (delaymin->TPHH != TAS_NOTIME))
           fprintf(file,"SUUMIN=%ld ",delaymin->SHH) ;
         if((delaymin->SLH != TAS_NOS) && (delaymin->TPLH != TAS_NOTIME))
           fprintf(file,"SDUMIN=%ld ",delaymin->SLH) ;
         if((delaymin->SHL != TAS_NOS) && (delaymin->TPHL != TAS_NOTIME))
           fprintf(file,"SUDMIN=%ld ",delaymin->SHL) ;
         if((delaymin->SLL != TAS_NOS) && (delaymin->TPLL != TAS_NOTIME))
           fprintf(file,"SDDMIN=%ld ",delaymin->SLL) ;
         if((delaymin->RHH != TAS_NORES) && (delaymin->TPHH != TAS_NOTIME))
           fprintf(file,"RUUMIN=%ld ",delaymin->RHH) ;
         if((delaymin->RLH != TAS_NORES) && (delaymin->TPLH != TAS_NOTIME))
           fprintf(file,"RDUMIN=%ld ",delaymin->RLH) ;
         if((delaymin->RHL != TAS_NORES) && (delaymin->TPHL != TAS_NOTIME))
           fprintf(file,"RUDMIN=%ld ",delaymin->RHL) ;
         if((delaymin->RLL != TAS_NORES) && (delaymin->TPLL != TAS_NOTIME))
           fprintf(file,"RDDMIN=%ld ",delaymin->RLL) ;
        }

     if((delay != NULL) && ((incone->TYPE & CNS_COMMAND) != CNS_COMMAND))
        {
         if((flag == 'N') &&
            ((delay->TPLH != TAS_NOTIME) || (delay->TPLL != TAS_NOTIME) ||
             (delay->TPHL != TAS_NOTIME) || (delay->TPHH != TAS_NOTIME)))
          {
           fprintf(file,"%s %s %s %s ",type1,name1,type2,name2) ;
           flag = 'Y' ;
          }
         if((delay->SHH != TAS_NOS) && (delay->TPHH != TAS_NOTIME))
           fprintf(file,"SUUMAX=%ld ",delay->SHH) ;
         if((delay->SLH != TAS_NOS) && (delay->TPLH != TAS_NOTIME))
           fprintf(file,"SDUMAX=%ld ",delay->SLH) ;
         if((delay->SHL != TAS_NOS) && (delay->TPHL != TAS_NOTIME))
           fprintf(file,"SUDMAX=%ld ",delay->SHL) ;
         if((delay->SLL != TAS_NOS) && (delay->TPLL != TAS_NOTIME))
           fprintf(file,"SDDMAX=%ld ",delay->SLL) ;
         if((delay->RHH != TAS_NORES) && (delay->TPHH != TAS_NOTIME))
           fprintf(file,"RUUMAX=%ld ",delay->RHH) ;
         if((delay->RLH != TAS_NORES) && (delay->TPLH != TAS_NOTIME))
           fprintf(file,"RDUMAX=%ld ",delay->RLH) ;
         if((delay->RHL != TAS_NORES) && (delay->TPHL != TAS_NOTIME))
           fprintf(file,"RUDMAX=%ld ",delay->RHL) ;
         if((delay->RLL != TAS_NORES) && (delay->TPLL != TAS_NOTIME))
           fprintf(file,"RDDMAX=%ld ",delay->RLL) ;
        }

     if(flag == 'Y')
      fprintf(file,";\n") ;

     timemax = TAS_NOTIME ;
     timemin = TAS_NOTIME ;
     flag = 'N' ;
     if(((ptype = getptype(incone->USER,TAS_DELAY_MINT)) != NULL) &&
        (delaymin != NULL))
      {
       delay = (delay_list *)ptype->DATA ;
       delaymin = delay ;
       if(((delay->RCLL != TAS_NOTIME) && ((delaymin->TPLH != TAS_NOTIME) ||
                                           (delaymin->TPLL != TAS_NOTIME) ||
                          ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))) ||
          ((delay->RCHH != TAS_NOTIME) && ((delaymin->TPHL != TAS_NOTIME) ||
                                           (delaymin->TPHH != TAS_NOTIME) ||
                          ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))))
         {
          if(delay->RCLL != TAS_NOTIME)
             timemin = delay->RCLL ;
          else
             timemin = delay->RCHH ;
          flag = 'Y' ;
         }
      }
     if(((ptype = getptype(incone->USER,TAS_DELAY_MAXT)) != NULL) &&
        (delaymax != NULL))
      {
       delay = (delay_list *)ptype->DATA ;
       if(((delay->RCLL != TAS_NOTIME) && ((delaymax->TPLH != TAS_NOTIME) ||
                                           (delaymax->TPLL != TAS_NOTIME) ||
                          ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))) ||
          ((delay->RCHH != TAS_NOTIME) && ((delaymax->TPHL != TAS_NOTIME) ||
                                           (delaymax->TPHH != TAS_NOTIME) ||
                          ((incone->TYPE & CNS_COMMAND) == CNS_COMMAND))))
         {
          if(delay->RCLL != TAS_NOTIME)
             timemax = delay->RCLL ;
          else
             timemax = delay->RCHH ;
          flag = 'Y' ;
         }
      }
     if(flag == 'Y') 
      {
       fprintf(file,"CONNECTOR %s %s %s ",name1,type2,name2) ;
       if(timemin != TAS_NOTIME)
         fprintf(file,"TRCMIN=%ld ",timemin) ;
       if(timemax != TAS_NOTIME)
         fprintf(file,"TRCMAX=%ld ",timemax) ;
       fprintf(file,";\n") ;
      }
    }
  }

 for(locon = cnsfig->LOCON ; locon != NULL ; locon = locon->NEXT)
  {
   name2 = locon->NAME ;
   type2= "CONNECTOR" ;
   type1="GATE" ;
   name1 = locon->NAME ;
   flag = 'N' ;
   timemax = TAS_NOTIME ;
   timemin = TAS_NOTIME ;
   if((ptype = getptype(locon->USER,TAS_DELAY_MIN)) != NULL)
    {
     delay = (delay_list *)ptype->DATA ;
     if((delay->RCLL != TAS_NOTIME) || (delay->RCHH != TAS_NOTIME))
       {
        if(delay->RCLL != TAS_NOTIME) 
           timemin = delay->RCLL ;
        else
           timemin = delay->RCHH ;
        flag = 'Y' ;
       }
    }
   if((ptype = getptype(locon->USER,TAS_DELAY_MAX)) != NULL)
    {
     delay = (delay_list *)ptype->DATA ;
     if((delay->RCLL != TAS_NOTIME) || (delay->RCHH != TAS_NOTIME))
       {
        if(delay->RCLL != TAS_NOTIME) 
           timemax = delay->RCLL ;
        else
           timemax = delay->RCHH ;
        flag = 'Y' ;
       }
    }
   if(flag == 'Y') 
    {
     fprintf(file,"%s %s %s %s ",type1,name1,type2,name2) ;
     if(timemin != TAS_NOTIME)
       fprintf(file,"TRCMIN=%ld ",timemin) ;
     if(timemax != TAS_NOTIME)
       fprintf(file,"TRCMAX=%ld ",timemax) ;
     fprintf(file,";\n") ;
    }
  }

if(fclose(file) != 0)
    {
    char straux[100] ;

    sprintf(straux,"%s.rc",TAS_CONTEXT->TAS_RC_FILE) ;
    tas_error(10,straux,TAS_WARNING) ;
    return(0) ;
    }
#endif
 return(1) ;
}
