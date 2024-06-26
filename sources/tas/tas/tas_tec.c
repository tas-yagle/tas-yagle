/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Version 5                                               */
/*    Fichier : tas_tec.c                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Amjad HAJJAR et Payam KIANI                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* calcul des parametres TAS.                                               */
/****************************************************************************/

#include "tas.h"


dt     *TAB_R         ,
       *TAB_A         ,
       *TAB_B         ,
       *TAB_RT        ,
       *TAB_RS        ,
       *TAB_VT        ,
       *TAB_VT0       ,
       *TAB_deg       ,
       *TAB_VDDmax    ,
       *TAB_SEUIL     ,
       *TAB_RDD       ,
       *TAB_KDD       ,
       *TAB_RDF       ,
       *TAB_RUD       ,
       *TAB_KUD       ,
       *TAB_RUF       ,
       *TAB_KG        ,
       *TAB_Q         ,
       *TAB_K         ,
       *TAB_KT        ,
       *TAB_KRT       ,
       *TAB_IFB       ,
       *TAB_CGP       ,
       *TAB_CGPO      ,
       *TAB_CGPOC     ,
       *TAB_CGD       ,
       *TAB_CGD0      ,
       *TAB_CGD1      ,
       *TAB_CGD2      ,
       *TAB_CGDC      ,
       *TAB_CGDC0     ,
       *TAB_CGDC1     ,
       *TAB_CGDC2     ,
       *TAB_CGSI      ,
       *TAB_CGSIC     ,
       *TAB_CDS       ,
       *TAB_MULU0     ,
       *TAB_DELTAVT0  ,
       *TAB_DL        ,
       *TAB_DW        ,
       *TAB_LMLT      ,
       *TAB_WMLT      ,
       *TAB_RACCS     ,
       *TAB_RACCD     ;

it     *TAB_INDEX=NULL ;


/****************************************************************************/
/*                            fonction tas_TechnoParameters()               */
/* fonction faisant appel au fichier techno et calculant les parametres     */
/* technos de TAS.                                                          */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) tecname: nom complet du fichier techno a charger.                  */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    La fonction renvoie -1 en cas de probleme, 0 sinon.                   */
/****************************************************************************/
int tas_TechnoParameters(void)
{
 long front;
 int valeur = 0;

 if(elpLoadOnceElp()) /* si probleme dans la lecture techno */
     valeur = 1;

 stm_init();

 if(TAS_CONTEXT->FRONT_NOT_SHRINKED == TAS_NOFRONT){
     front = mbk_long_round(STM_DEF_SLEW*((float)TTV_UNIT));
     TAS_CONTEXT->FRONT_NOT_SHRINKED = front;
 }else{
     front = TAS_CONTEXT->FRONT_NOT_SHRINKED;
 }
 
TAS_CONTEXT->FRONT_CON = front;

STM_DEF_SLEW = (TAS_CONTEXT->FRONT_CON)/TTV_UNIT ;
 
if (TAB_INDEX==NULL)
{
  TAB_INDEX     = addit(4) ;
  TAB_R         = adddt(2) ;
  TAB_A         = adddt(2) ;
  TAB_B         = adddt(2) ;
  TAB_RT        = adddt(2) ;
  TAB_RS        = adddt(2) ;
  TAB_VT        = adddt(2) ;
  TAB_VT0       = adddt(2) ;
  TAB_deg       = adddt(2) ;
  TAB_VDDmax    = adddt(2) ;
  TAB_SEUIL     = adddt(2) ;
  TAB_RDD       = adddt(2) ;
  TAB_KDD       = adddt(2) ;
  TAB_RDF       = adddt(2) ;
  TAB_RUD       = adddt(2) ;
  TAB_KUD       = adddt(2) ;
  TAB_RUF       = adddt(2) ;
  TAB_KG        = adddt(2) ;
  TAB_Q         = adddt(2) ;
  TAB_K         = adddt(2) ;
  TAB_KT        = adddt(2) ;
  TAB_KRT       = adddt(2) ;
  TAB_IFB       = adddt(2) ;
  TAB_CGP       = adddt(2) ;
  TAB_CGPO      = adddt(2) ;
  TAB_CGPOC     = adddt(2) ;
  TAB_CGD       = adddt(2) ;
  TAB_CGD0      = adddt(2) ;
  TAB_CGD1      = adddt(2) ;
  TAB_CGD2      = adddt(2) ;
  TAB_CGDC      = adddt(2) ;
  TAB_CGDC0     = adddt(2) ;
  TAB_CGDC1     = adddt(2) ;
  TAB_CGDC2     = adddt(2) ;
  TAB_CGSI      = adddt(2) ;
  TAB_CGSIC     = adddt(2) ;
  TAB_CDS       = adddt(2) ;
  TAB_MULU0     = adddt(2) ;
  TAB_DELTAVT0  = adddt(2) ;
  TAB_DL        = adddt(2) ;
  TAB_DW        = adddt(2) ;
  TAB_LMLT      = adddt(2) ;
  TAB_WMLT      = adddt(2) ;
  TAB_RACCS     = adddt(2) ;
  TAB_RACCD     = adddt(2) ;
}

return(valeur) ;
}

/****************************************************************************/
/*                            fonction tas_getparam()                       */
/* recherche un parametre en fonction du transistor                         */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/****************************************************************************/
double tas_getparam(lotrs,type,param)
lotrs_list *lotrs ;
int type ;
int param ;
{
 elpmodel_list *model ;
 float alim;

 if(cns_getlotrsalim(lotrs, TAS_PATH_TYPE, &alim))
     model = elpGetModel(lotrs,alim,elpTYPICAL) ; // typical case if a there is a specific voltage...
 else
     model = elpGetModel(lotrs,elpGeneral[elpGVDDMAX],type) ;

 if(getititem(TAB_INDEX,model->elpModelIndex) == EMPTYHT)
     tas_calparam(model) ; 
   

 switch(param) 
  {
   case TP_R         : return(TAB_R->data[model->elpModelIndex]) ;
   case TP_A         : return elp_scale_a (lotrs,model) ;
   case TP_B         : return(TAB_B->data[model->elpModelIndex]) ;
   case TP_RT        : return(TAB_RT->data[model->elpModelIndex]) ;
   case TP_RS        : return(TAB_RS->data[model->elpModelIndex]) ;
   case TP_VT        : return elp_scale_vth ( lotrs, model );
   case TP_VT0       : return(TAB_VT0->data[model->elpModelIndex]) ;
   case TP_deg       : return(TAB_deg->data[model->elpModelIndex]) ;
   case TP_VDDmax    : return(TAB_VDDmax->data[model->elpModelIndex]) ;
   case TP_SEUIL     : return(TAB_SEUIL->data[model->elpModelIndex]) ;
   case TP_RDD       : return(TAB_RDD->data[model->elpModelIndex]) ;
   case TP_KDD       : return(TAB_KDD->data[model->elpModelIndex]) ;
   case TP_RDF       : return(TAB_RDF->data[model->elpModelIndex]) ;
   case TP_RUD       : return(TAB_RUD->data[model->elpModelIndex]) ;
   case TP_KUD       : return(TAB_KUD->data[model->elpModelIndex]) ;
   case TP_RUF       : return(TAB_RUF->data[model->elpModelIndex]) ;
   case TP_KG        : return(TAB_KG->data[model->elpModelIndex]) ;
   case TP_Q         : return(TAB_Q->data[model->elpModelIndex]) ;
   case TP_K         : return(TAB_K->data[model->elpModelIndex]) ;
   case TP_KT        : return(TAB_KT->data[model->elpModelIndex]) ;
   case TP_KRT       : return(TAB_KRT->data[model->elpModelIndex]) ;
   case TP_IFB       : return(TAB_IFB->data[model->elpModelIndex]) ;
   case TP_CGP       : return(TAB_CGP->data[model->elpModelIndex]) ;
   case TP_CGPO      : return(TAB_CGPO->data[model->elpModelIndex]) ;
   case TP_CGPOC     : return(TAB_CGPOC->data[model->elpModelIndex]) ;
   case TP_CGD       : return(TAB_CGD->data[model->elpModelIndex]) ;
   case TP_CGD0      : return(TAB_CGD0->data[model->elpModelIndex]) ;
   case TP_CGD1      : return(TAB_CGD1->data[model->elpModelIndex]) ;
   case TP_CGD2      : return(TAB_CGD2->data[model->elpModelIndex]) ;
   case TP_CGDC      : return(TAB_CGDC->data[model->elpModelIndex]) ;
   case TP_CGDC0     : return(TAB_CGDC0->data[model->elpModelIndex]) ;
   case TP_CGDC1     : return(TAB_CGDC1->data[model->elpModelIndex]) ;
   case TP_CGDC2     : return(TAB_CGDC2->data[model->elpModelIndex]) ;
   case TP_CGSI      : return(TAB_CGSI->data[model->elpModelIndex]) ;
   case TP_CGSIC     : return(TAB_CGSIC->data[model->elpModelIndex]) ;
   case TP_CDS       : return(TAB_CDS->data[model->elpModelIndex]) ;
   case TP_MULU0     : return(TAB_MULU0->data[model->elpModelIndex]) ;
   case TP_DELTAVT0  : return(TAB_DELTAVT0->data[model->elpModelIndex]) ;
   case TP_DL        : return(TAB_DL->data[model->elpModelIndex]) ;
   case TP_DW        : return(TAB_DW->data[model->elpModelIndex]) ;
   case TP_LMLT      : return(TAB_LMLT->data[model->elpModelIndex]) ;
   case TP_WMLT      : return(TAB_WMLT->data[model->elpModelIndex]) ;
   case TP_RACCS     : return(TAB_RACCS->data[model->elpModelIndex]) ;
   case TP_RACCD     : return(TAB_RACCD->data[model->elpModelIndex]) ;
  }
return(0);
}

/****************************************************************************/
/*                            fonction tas_calparam()                       */
/* calcul les nouveaux parametre pour un index donne                        */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/****************************************************************************/
int tas_calparam(model) 
elpmodel_list *model ;
{
 int index = model->elpModelIndex ;
 double vt,a,b,rt,vmax,vdeg,seuil,
        ifb,k,q,kg,ruf,kud,
        rud,rdf,kdd,rdd,r,kt,rno,rpo;
 double vt1, var1, var2, b1;

 setititem(TAB_INDEX,index,(long)0) ;
 setdtitem(TAB_VT,index,model->elpModel[elpVT]) ;
 setdtitem(TAB_VT0,index,model->elpModel[elpVT0]) ;
 vt=getdtitem(TAB_VT,index);
 setdtitem(TAB_A,index,model->elpModel[elpA]) ;
 a=getdtitem(TAB_A,index);
 setdtitem(TAB_B,index,model->elpModel[elpB]) ;
 b=getdtitem(TAB_B,index);
 setdtitem(TAB_RS,index,model->elpModel[elpRS]) ;
 setdtitem(TAB_RT,index,model->elpModel[elpRT]) ;
 rt=getdtitem(TAB_RT,index);
 setdtitem(TAB_VDDmax,index,model->elpVoltage[elpVDDMAX]) ;
 vmax=getdtitem(TAB_VDDmax,index);
 setdtitem(TAB_deg,index,model->elpVoltage[elpVDEG]) ;
 vdeg=getdtitem(TAB_deg,index);
 setdtitem(TAB_SEUIL,index,model->elpVoltage[elpVDDMAX]*elpGeneral[elpGDTHR]) ;
 seuil=getdtitem(TAB_SEUIL,index);
 setdtitem(TAB_CGP,index,model->elpCapa[elpCGP]) ;
 setdtitem(TAB_CGPO,index,model->elpCapa[elpCGPO]) ;
 setdtitem(TAB_CGPOC,index,model->elpCapa[elpCGPOC]) ;
 setdtitem(TAB_CGD,index,model->elpCapa[elpCGD]) ;
 setdtitem(TAB_CGD0,index,model->elpCapa[elpCGD0]) ;
 setdtitem(TAB_CGD1,index,model->elpCapa[elpCGD1]) ;
 setdtitem(TAB_CGD2,index,model->elpCapa[elpCGD2]) ;
 setdtitem(TAB_CGDC,index,model->elpCapa[elpCGDC]) ;
 setdtitem(TAB_CGDC0,index,model->elpCapa[elpCGDC0]) ;
 setdtitem(TAB_CGDC1,index,model->elpCapa[elpCGDC1]) ;
 setdtitem(TAB_CGDC2,index,model->elpCapa[elpCGDC2]) ;
 setdtitem(TAB_CGSI,index,model->elpCapa[elpCGSI]) ;
 setdtitem(TAB_CGSIC,index,model->elpCapa[elpCGSIC]) ;
 setdtitem(TAB_CDS,index,model->elpCapa[elpCDS]) ;
 setdtitem(TAB_DL,index,model->elpShrink[elpDL]) ;
 setdtitem(TAB_DW,index,model->elpShrink[elpDW]) ;
 setdtitem(TAB_LMLT,index,model->elpShrink[elpLMLT]) ;
 setdtitem(TAB_WMLT,index,model->elpShrink[elpWMLT]) ;
 setdtitem(TAB_KT,index,model->elpModel[elpKT]) ;
 setdtitem(TAB_KRT,index,model->elpModel[elpKRT]) ;
 setdtitem(TAB_MULU0,index,model->elpModel[elpMULU0]) ;
 setdtitem(TAB_DELTAVT0,index,model->elpModel[elpDELVT0]) ;
 setdtitem(TAB_RACCS,index,model->elpRacc[elpRACCS]);
 setdtitem(TAB_RACCD,index,model->elpRacc[elpRACCD]);
 kt=getdtitem(TAB_KT,index);


 ifb = 250.0*vmax/rt ;
 setdtitem(TAB_IFB,index,ifb) ;

 r = (1.0+b*0.90*(vmax-vt)) / (a*0.90*(vmax-vt)) ; 
 setdtitem(TAB_R,index,r) ;

 if(model->elpTransType==elpNMOS)
   rdd = (vmax-seuil)*(b+(1.0/(vmax-vt)))/(a*(vmax-vt)) ;
 else if(model->elpTransType==elpPMOS){
   if(kt > 0.0)
       vt1 = vt;
   else
       vt1 = (vt + vdeg)/2.0;
   b1 = b*(vmax-vt1);
   rdd = vmax*(kt+1.0) / (vmax*(1.0-kt)-vt1-vt1);
   rdd += b1 * log((2*(vmax-vt1))/(vmax*(1.0-kt)-vt1-vt1));
   rdd /= (a*(vmax-vt1));
   rdd *= (kt+1.0);
 }
 //setdtitem(TAB_RDD,index,rdd) ;

 if(model->elpTransType==elpPMOS)
   rud = seuil * (b+(1.0/(vmax-vt)))/(a*(vmax-vt)) ;
 else if(model->elpTransType==elpNMOS){
   if(kt > 0.0)
       vt1 = vt;
   else
       vt1 = (vmax + vt - vdeg)/2.0;
   b1=b*(vmax-vt1);
   rud = vmax*(kt+1.0) / (vmax*(1.0-kt)-vt1-vt1);
   rud += b1 * log((2*(vmax-vt1))/(vmax*(1.0-kt)-vt1-vt1));
   rud /= (a*(vmax-vt1)) ;
   rud *= (kt+1.0);
 }
 //setdtitem(TAB_RUD,index,rud) ;

 if(model->elpTransType==elpPMOS){
   ruf = rt ;
   rdf = rdd ;
 }
 else if(model->elpTransType==elpNMOS){
   rdf = rt ;
   ruf = rud ;
 }
 setdtitem(TAB_RUF,index,ruf) ;
 setdtitem(TAB_RDF,index,rdf) ;

 if(model->elpTransType==elpPMOS){
   if(kt > 0.0)
       vt1 = vt;
   else
       vt1 = (vt + 2*vdeg)/3.0 ;
   b1 = /*1.0+*/b*(vmax-vt1);
   //kdd = rdd*((vmax*(1.0-kt)-2*vt1)*(1.0+b1)) / (rt*(2*(vmax-vt1) + b1*(vmax*(1.0-kt)-2*vt1))) ;
   rpo = (rt*(2*(vmax-vt1) + b1*(vmax*(1.0-kt)-2*vt1))) / ((vmax*(1.0-kt)-2*vt1)*(1.0+b1)) ;
/* pour revenir comme avant mettre en commentaire ce qui suit: */
   rdd = (rdd + rpo) / 2.0; /* a mettre en commentaire */
   //kdd = 1.0;
   rpo = rdd * 1.0; /* a mettre en commentaire */
   rdd = rdd * 1.3; /* a mettre en commentaire */
   kdd = rdd / rpo;
 }
 else if(model->elpTransType==elpNMOS){
   b1=b*(vmax-vt);
   kdd = b1/(b1-1.0);
   kdd *= log((2*b1)/(b1+1.0));
   kdd -= 0.5*log(3.0-(4.0*vt/vmax)) ;
 }
 setdtitem(TAB_RDD,index,rdd) ;
 setdtitem(TAB_KDD,index,kdd) ;

 q = a*(vmax-vt)*(vmax-vt)/(1.0+b*(vmax-vt));
 setdtitem(TAB_Q,index,q) ;
 
 k = a/(1+b);
 setdtitem(TAB_K,index,k) ;


 if(model->elpTransType==elpPMOS){
   b1=b*(vmax-vt);
   kud = b1/(b1-1.0);
   kud *= log((2*b1)/(b1+1.0));
   kud -= 0.5*log(3.0-(4.0*vt/vmax)) ;
 }
 else if(model->elpTransType==elpNMOS){
   if(kt > 0.0)
       vt1 = vt;
   else
       vt1 = (vt + 2*(vmax-vdeg))/3.0 ;
   b1 = /*1.0+*/b*(vmax-vt1);
   //kud = rud*((vmax*(1.0-kt)-2*vt1)*(1.0+b1)) / (rt*(2*(vmax-vt1) + b1*(vmax*(1.0-kt)-2*vt1))) ;
   rno = (rt*(2*(vmax-vt1) + b1*(vmax*(1.0-kt)-2*vt1))) / ((vmax*(1.0-kt)-2*vt1)*(1.0+b1)) ;
/* pour revenir comme avant mettre en commentaire ce qui suit: */
   rud = (rud + rno) / 2.0; /* a mettre en commentaire */
   //kud = 1.0;
   rno = rud * 1.0; /* a mettre en commentaire */
   rud = rud * 1.3; /* a mettre en commentaire */
   kud = rud / rno;
 }
 setdtitem(TAB_RUD,index,rud) ;
 setdtitem(TAB_KUD,index,kud) ;


 if(model->elpTransType==elpNMOS){
   vt1 = (vt + 2*(vmax - vdeg)) / 3.0;
   var1 = 0.5*vmax / (vmax-vt1);
   var2 = (0.5*vmax - vt1) / (vmax - vt1) ;
   var1 = 0.5*log( (1.0+var1)/(1.0-var1) );
   var2 = 0.5*log( (1.0+var2)/(1.0-var2) );
   kg = var1 - var2;
 }
 else if(model->elpTransType==elpPMOS){
   vt1 = (vt + 2*vdeg) / 3.0;
   var1 = 0.5*vmax / (vmax-vt1);
   var2 = (0.5*vmax - vt1)/(vmax - vt1) ;
   var1 = 0.5*log( (1.0+var1)/(1.0-var1) );
   var2 = 0.5*log( (1.0+var2)/(1.0-var2) );
   kg = var1 - var2;
 }
 setdtitem(TAB_KG,index,kg) ;

return 0;
}

