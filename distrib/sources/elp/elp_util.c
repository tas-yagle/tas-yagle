/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : ELP Verison 1                                               */
/*    Fichier : elp_util.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1995 Laboratoire MASI equipe CAO & VLSI            */
/*    Tous droits reserves                                                  */
/*    Support : e-mail cao-vlsi@masi.ibp.fr                                 */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                                                                          */
/****************************************************************************/
/* caracterisation electrique des netlists en fonction des parametres elp   */
/****************************************************************************/

#include <stdlib.h>
#include <string.h>
#include AVT_H
#include ELP_H

//int    ELP_CAPA_LEVEL = ELP_CAPA_LEVEL1;
//double ELP_MULU0_MARGIN = -1.0;
//double ELP_DELVT0_MARGIN = -1.0;
//double ELP_SA_MARGIN = -1.0;
//double ELP_SB_MARGIN = -1.0;
//double ELP_SD_MARGIN = -1.0;
double ELP_NF_MARGIN = -1.0;
double ELP_M_MARGIN = -1.0;
double ELP_NRS_MARGIN = -1.0;
double ELP_NRD_MARGIN = -1.0;
extern FILE *elpin                                                         ;
int    ELPLINE                                                             ;
//int    ELP_DRV_FILE = 0;
int    ELP_GENPARAM = 0 ; 
int    ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE ; 
char  *elpoldtechnofilename = NULL;
int    elpres_load;
char   elpTechnoName[elpSTRINGSIZE]                                        ;
char   elpTechnoFile[elpSTRINGSIZE]                                        ;
char   elpGenTechnoFile[elpSTRINGSIZE]                                     ; // auto generated techno
double elpTechnoVersion                                                    ;
char   elpEsimName[elpSTRINGSIZE]                                          ;
double elpGeneral[elpGENERALNUM] = {0.0,ELPINITTEMP,200.0,ELPMINVOLTAGE,ELPMINVOLTAGE,ELPMINVOLTAGE,ELPINITTHR,ELPINITTHR,ELPINITTHR} ;
elpmodel_list *ELP_MODEL_LIST = NULL                                       ;
elptree_list *ELP_HEAD_TREE = NULL                                         ;
char   elpLang                             = elpDEFLANG                    ;
extern int elpparse() ;
int ELP_CAPA_DIFF = 0 ;
int ELP_CALC_ONLY_CAPA = 0;
void (*elpCalcPAFct)(lotrs_list*,char*,int,int,double,elp_lotrs_param*,double*,double*,double*,double*);

char *elpTabTechnoName[] = { "No techno", "Bsim3v3", "Bsim4", "Psp", "Mos2", "MM9" };

/****************************************************************************\
 FUNCTION : elpSameSD_sig      
\****************************************************************************/
int elpSameSD_sig ( lotrs_list *lotrs )
{
  int same_SD = 0;
  int vddsource=0, vsssource=0, vdddrain=0, vssdrain=0 ;

  if ( mbk_LosigIsVDD(lotrs->DRAIN->SIG) ) vdddrain=1;
  if ( mbk_LosigIsVSS(lotrs->DRAIN->SIG) ) vssdrain=1;
  if ( mbk_LosigIsVDD(lotrs->SOURCE->SIG) ) vddsource=1;
  if ( mbk_LosigIsVSS(lotrs->SOURCE->SIG) ) vsssource=1;
  
  if ( (lotrs->DRAIN->SIG == lotrs->SOURCE->SIG) ||
       ( vdddrain && vddsource ) ||
       ( vssdrain && vsssource )
       )
    same_SD = 1;
  return same_SD;
}

/****************************************************************************\
 FUNCTION : elpGetVddFromCorner
\****************************************************************************/
double elpGetVddFromCorner ( lotrs_list *lotrs, int corner )
{
 double vdd;
float alim;

 if(cns_getlotrsalim(lotrs, 'M', &alim)){
   vdd =  (double)alim;
 }else{
   switch ( corner ) {
     case elpBEST : vdd = elpGeneral[elpGVDDBEST];
                    break;
     case elpWORST: vdd = elpGeneral[elpGVDDWORST];
                    break;
     default      : vdd = elpGeneral[elpGVDDMAX];
                    break;
   }
 }
 return vdd;
}

/*****************************************************************************/
/*                        function elpDouble2Int                             */
/*****************************************************************************/
elpFCT int elpDouble2Int ( value )
double value;
{
 return (int)(value+0.5);
}

/*****************************************************************************/
/*                        function elpDouble2Long                            */
/*****************************************************************************/
elpFCT long elpDouble2Long (value)
double value;
{
 return (long)(value+0.5);
}

/*****************************************************************************/
/*                        function elpShrinkSize()                           */
/*****************************************************************************/
elpFCT long elpShrinkSize(size,delta,mlt)
long size ;
double delta ;
double mlt ;
{

 if((size == 0) || (size == ELPMAXLONG))
   return(size) ;
 delta = delta * (double)SCALE_X ;
 size = elpDouble2Long(mlt*(double)size + delta) ;
 return(size) ;
}

/*****************************************************************************/
/*                        function elpAddTree()                              */
/*****************************************************************************/

void elp_get_key(lotrs_list *lt, chain_list **longkey)
{
  ptype_list *pt;
  *longkey=NULL;
  if (lt!=NULL && (pt=getptype(lt->USER, MCC_COMPUTED_KEY))!=NULL)
    *longkey=(chain_list *)pt->DATA;
}

int elp_is_same_paramcontext(chain_list *longkey0, chain_list *longkey1)
{
  while (longkey0!=NULL && longkey1!=NULL && mbk_cmpdouble(*(float *)&longkey0->DATA,*(float *)&longkey1->DATA, EQT_PRECISION)==0)
  {
    longkey0=longkey0->NEXT;
    longkey1=longkey1->NEXT;
  }
  if (longkey0!=NULL || longkey1!=NULL) return 0;
  return 1;
}

elpFCT elptree_list *elpAddTree(head,model,level)
elptree_list *head ;
elpmodel_list *model ;
int level ;
{
 elptree_list *tree ;
 long mulu0,delvt0,sa,sb,sd,nf,val;
 long sc,sca,scb,scc;

 tree = (elptree_list *)mbkalloc(sizeof(elptree_list)) ;
 tree->LEVEL = level ;
 tree->DOWN = NULL ;
 tree->NEXT = head ;

 switch(level)
   {
    case elpNameLevel   : tree->DATA1 = (long)model->elpModelName ;
                          tree->DATA2 = (long)model->elpModelNameAlias ;
                          break ;
    case elpLengthLevel : 
                          tree->DATA1 = model->elpRange[elpLMIN] ;
                          tree->DATA2 = model->elpRange[elpLMAX] ;
                          break ;
    case elpWidthLevel  : 
                          tree->DATA1 = model->elpRange[elpWMIN] ;
                          tree->DATA2 = model->elpRange[elpWMAX] ;
                          break ;
    case elpVDDLevel    : tree->DATA1 = elpDouble2Long(model->elpVoltage[elpVDDMAX]*SCALE_X ) ;
                          tree->DATA2 = (long)NULL ;
                          break ;
    case elpMULU0Level  : if ( V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE < 0.0 ) {
                            tree->DATA1 = elpDouble2Long(model->elpModel[elpMULU0]*ELPPRECISION) ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            mulu0 = elpDouble2Long(model->elpModel[elpMULU0]*ELPPRECISION) ;
                            if ( mulu0 >= 0 ) {
                              tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE) * mulu0);
                              tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE) * mulu0);
                            }
                            else {
                              tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE) * mulu0);
                              tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE) * mulu0);
                            }
                          }
                          break ;
    case elpDELVT0Level : if ( V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE < 0.0 ) {
                            tree->DATA1 = elpDouble2Long (model->elpModel[elpDELVT0]*ELPPRECISION);
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            delvt0 = elpDouble2Long (model->elpModel[elpDELVT0]*ELPPRECISION);
                            if ( delvt0 >= 0 ) {
                              tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE) * delvt0);
                              tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE) * delvt0);
                            }
                            else {
                              tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE) * delvt0);
                              tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE) * delvt0);
                            }
                          }
                          break ;
    case elpSALevel     : if ( model->elpModel[elpSA] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSA]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              sa = elpDouble2Long (model->elpModel[elpSA]*ELPPRECISION2);
                              if ( sa >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE) * sa);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE) * sa);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE) * sa);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE) * sa);
                              }
                            }
                          }
                          break ;
    case elpSBLevel     : if (  model->elpModel[elpSB] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSB]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              sb = elpDouble2Long (model->elpModel[elpSB]*ELPPRECISION2);
                              if ( sb >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE) * sb);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE) * sb);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE) * sb);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE) * sb);
                              }
                            }
                          }
                          break ;
    case elpSDLevel     : if (  model->elpModel[elpSD] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSD]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              sd = elpDouble2Long (model->elpModel[elpSD]*ELPPRECISION2);
                              if ( sd >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE) * sd);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE) * sd);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE) * sd);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE) * sd);
                              }
                            }
                          }
                          break ;
    case elpNFLevel     : if (  model->elpModel[elpNF] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( ELP_NF_MARGIN < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpNF]);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              nf = elpDouble2Long (model->elpModel[elpNF]);
                              if ( nf >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - ELP_NF_MARGIN) * nf);
                                tree->DATA2 = elpDouble2Long((1.0 + ELP_NF_MARGIN) * nf);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + ELP_NF_MARGIN) * nf);
                                tree->DATA2 = elpDouble2Long((1.0 - ELP_NF_MARGIN) * nf);
                              }
                            }
                          }
                          break ;
    case elpMLevel      : if (  model->elpModel[elpM] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( ELP_M_MARGIN < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpM]);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              val = elpDouble2Long (model->elpModel[elpM]);
                              if ( val >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - ELP_M_MARGIN) * val);
                                tree->DATA2 = elpDouble2Long((1.0 + ELP_M_MARGIN) * val);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + ELP_M_MARGIN) * val);
                                tree->DATA2 = elpDouble2Long((1.0 - ELP_M_MARGIN) * val);
                              }
                            }
                          }
                          break ;
    case elpNRSLevel      : if (  model->elpModel[elpNRS] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( ELP_M_MARGIN < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpNRS]*ELPPRECISION);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              val = elpDouble2Long (model->elpModel[elpNRS]*ELPPRECISION);
                              if ( val >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - ELP_NRS_MARGIN) * val);
                                tree->DATA2 = elpDouble2Long((1.0 + ELP_NRS_MARGIN) * val);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + ELP_NRS_MARGIN) * val);
                                tree->DATA2 = elpDouble2Long((1.0 - ELP_NRS_MARGIN) * val);
                              }
                            }
                          }
                          break ;
    case elpNRDLevel      : if (  model->elpModel[elpNRD] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( ELP_M_MARGIN < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpNRD]*ELPPRECISION);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              val = elpDouble2Long (model->elpModel[elpNRD]*ELPPRECISION);
                              if ( val >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - ELP_NRD_MARGIN) * val);
                                tree->DATA2 = elpDouble2Long((1.0 + ELP_NRD_MARGIN) * val);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + ELP_NRD_MARGIN) * val);
                                tree->DATA2 = elpDouble2Long((1.0 - ELP_NRD_MARGIN) * val);
                              }
                            }
                          }
                          break ;
    case elpSCLevel     : if ( model->elpModel[elpSC] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSC]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              sc = elpDouble2Long (model->elpModel[elpSC]*ELPPRECISION2);
                              if ( sc >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE) * sc);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE) * sc);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE) * sc);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE) * sc);
                              }
                            }
                          }
                          break ;
    case elpSCALevel    : if ( model->elpModel[elpSCA] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSCA]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              sca = elpDouble2Long (model->elpModel[elpSCA]*ELPPRECISION2);
                              if ( sca >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE) * sca);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE) * sca);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE) * sca);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE) * sca);
                              }
                            }
                          }
                          break ;
    case elpSCBLevel    : if ( model->elpModel[elpSCB] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSCB]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              scb = elpDouble2Long (model->elpModel[elpSCB]*ELPPRECISION2);
                              if ( scb >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE) * scb);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE) * scb);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE) * scb);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE) * scb);
                              }
                            }
                          }
                          break ;
    case elpSCCLevel    : if ( model->elpModel[elpSCC] < ELPMINVALUE ) {
                            tree->DATA1 = (long)NULL ;
                            tree->DATA2 = (long)NULL ;
                          }
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE < 0.0 ) {
                              tree->DATA1 = elpDouble2Long (model->elpModel[elpSCC]*ELPPRECISION2);
                              tree->DATA2 = (long)NULL ;
                            }
                            else {
                              sca = elpDouble2Long (model->elpModel[elpSCC]*ELPPRECISION2);
                              if ( sca >= 0 ) {
                                tree->DATA1 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE) * scc);
                                tree->DATA2 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE) * scc);
                              }
                              else {
                                tree->DATA1 = elpDouble2Long((1.0 + V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE) * scc);
                                tree->DATA2 = elpDouble2Long((1.0 - V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE) * scc);
                              }
                            }
                          }
                          break ;
    case elpVBULKLevel  : tree->DATA1 =  elpDouble2Long(model->elpVoltage[elpVBULK]*SCALE_X ) ;
                          tree->DATA2 = (long)NULL ;
                          break ;
    case elpParamContextLevel :
                          tree->DATA1 = (long)model->longkey;
                          tree->DATA2 = (long)NULL ;
                          break;
    case elpCaseLevel   : tree->DATA1 = (long)model->elpTransCase ;
                          tree->DATA2 = (long)NULL ;
                          break ;
   }
 
 return tree ;
}

/*****************************************************************************/
/*                        function elpCmpTreeModel()                         */
/*****************************************************************************/
elpFCT int elpCmpTreeModel(tree,model,level)
elptree_list *tree ;
elpmodel_list *model ;
int level ;
{
 int res = 0 ;
 long mulu0,delvt0,sa,sb,sd,nf,val,sc,sca,scb,scc;

 switch(level) 
   {
    case elpNameLevel   : if(tree->DATA1 == (long)model->elpModelName)
                            res = 1 ;
                          break ;
    case elpLengthLevel : 
                          if((tree->DATA1 == model->elpRange[elpLMIN]) &&
                             (tree->DATA2 == model->elpRange[elpLMAX]))
                            res = 1 ;
                          break ;
    case elpWidthLevel  : 
                          if((tree->DATA1 == model->elpRange[elpWMIN]) &&
                             (tree->DATA2 == model->elpRange[elpWMAX]))
                            res = 1 ;
                          break ;
    case elpVDDLevel    : if(tree->DATA1 == elpDouble2Long (model->elpVoltage[elpVDDMAX]*SCALE_X )) 
                            res = 1 ;
                          break ;
    case elpMULU0Level  : if ( V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE < 0.0 ) {
                            if (tree->DATA1 == elpDouble2Long (model->elpModel[elpMULU0]*ELPPRECISION))
                              res = 1;
                          }
                          else {
                            mulu0 = elpDouble2Long (model->elpModel[elpMULU0]*ELPPRECISION);
                            if ( mulu0 >= tree->DATA1 && mulu0 <= tree->DATA2 )
                              res = 1;
                          }
                          break ;
    case elpDELVT0Level : if ( V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE < 0.0 ) {
                            if (tree->DATA1 == elpDouble2Long (model->elpModel[elpDELVT0]*ELPPRECISION))
                              res = 1;
                          }
                          else {
                            delvt0 = elpDouble2Long (model->elpModel[elpDELVT0]*ELPPRECISION);
                            if ( delvt0 >= tree->DATA1 && delvt0 <= tree->DATA2 )
                              res = 1;
                          }
                          break ;
    case elpSALevel     : if ( model->elpModel[elpSA] < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSA]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sa = elpDouble2Long (model->elpModel[elpSA]*ELPPRECISION2);
                              if ( sa >= tree->DATA1 && sa <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSBLevel     : if ( model->elpModel[elpSB] < ELPMINVALUE )
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSB]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sb = elpDouble2Long (model->elpModel[elpSB]*ELPPRECISION2);
                              if ( sb >= tree->DATA1 && sb <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSDLevel     : if ( model->elpModel[elpSD] < ELPMINVALUE )
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSD]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sd = elpDouble2Long (model->elpModel[elpSD]*ELPPRECISION2);
                              if ( sd >= tree->DATA1 && sd <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpNFLevel     : if ( model->elpModel[elpNF] < ELPMINVALUE )
                            res = 1;
                          else {
                            if ( ELP_NF_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpNF]))
                                res = 1;
                            }
                            else {
                              nf = elpDouble2Long (model->elpModel[elpNF]);
                              if ( nf >= tree->DATA1 && nf <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpMLevel      : if ( model->elpModel[elpM] < ELPMINVALUE )
                            res = 1;
                          else {
                            if ( ELP_M_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpM]))
                                res = 1;
                            }
                            else {
                              val = elpDouble2Long (model->elpModel[elpM]);
                              if ( val >= tree->DATA1 && val <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpNRSLevel    : if ( model->elpModel[elpNRS] < ELPMINVALUE )
                            res = 1;
                          else {
                            if ( ELP_NRS_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpNRS]*ELPPRECISION))
                                res = 1;
                            }
                            else {
                              val = elpDouble2Long (model->elpModel[elpNRS]*ELPPRECISION);
                              if ( val >= tree->DATA1 && val <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpNRDLevel    : if ( model->elpModel[elpNRD] < ELPMINVALUE )
                            res = 1;
                          else {
                            if ( ELP_NRD_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpNRD]*ELPPRECISION))
                                res = 1;
                            }
                            else {
                              val = elpDouble2Long (model->elpModel[elpNRD]*ELPPRECISION);
                              if ( val >= tree->DATA1 && val <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpVBULKLevel  : if(tree->DATA1 == elpDouble2Long (model->elpVoltage[elpVBULK]*SCALE_X)) 
                            res = 1 ;
                          break ;
    case elpParamContextLevel :
                          if(elp_is_same_paramcontext((chain_list *)tree->DATA1,model->longkey))        
                            res=1;
                          break;
    case elpCaseLevel   : if(tree->DATA1 == (long)model->elpTransCase)
                            res = 1 ;
                          break ;
    case elpSCLevel     : if ( model->elpModel[elpSC] < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSC]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sc = elpDouble2Long (model->elpModel[elpSC]*ELPPRECISION2);
                              if ( sc >= tree->DATA1 && sc <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSCALevel    : if ( model->elpModel[elpSCA] < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSCA]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sca = elpDouble2Long (model->elpModel[elpSCA]*ELPPRECISION2);
                              if ( sca >= tree->DATA1 && sca <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSCBLevel    : if ( model->elpModel[elpSCB] < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSCB]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              scb = elpDouble2Long (model->elpModel[elpSCB]*ELPPRECISION2);
                              if ( scb >= tree->DATA1 && scb <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSCCLevel    : if ( model->elpModel[elpSCC] < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (model->elpModel[elpSCC]*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              scc = elpDouble2Long (model->elpModel[elpSCC]*ELPPRECISION2);
                              if ( scc >= tree->DATA1 && scc <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
   }

 return(res) ;
}

/*****************************************************************************/
/*                        function elpDiffTree()                             */
/*****************************************************************************/
elpFCT int elpDiffTree(model,tree,level)
elpmodel_list *model ;
elptree_list *tree ;
int level ;
{

 switch(level) 
   {
    case elpNameLevel   : return 0 ;
    case elpLengthLevel : 
                         if(model->elpRange[elpLMAX] > tree->DATA2)
                            return 1 ;
                         else
                            return 0 ;
    case elpWidthLevel  : 
                         if(model->elpRange[elpWMAX] > tree->DATA2)
                            return 1 ;
                         else
                            return 0 ;
    case elpVDDLevel    : return 0  ;
    case elpMULU0Level  : return 0  ;
    case elpDELVT0Level : return 0  ;
    case elpSALevel     : return 0  ;
    case elpSBLevel     : return 0  ;
    case elpSDLevel     : return 0  ;
    case elpNFLevel     : return 0  ;
    case elpMLevel      : return 0  ;
    case elpNRSLevel    : return 0  ;
    case elpNRDLevel    : return 0  ;
    case elpVBULKLevel  : return 0  ;
    case elpParamContextLevel : return 0  ;
    case elpCaseLevel   : return 0  ;
    case elpSCLevel     : return 0 ;
    case elpSCALevel    : return 0 ;
    case elpSCBLevel    : return 0 ;
    case elpSCCLevel    : return 0 ;
   }

 return 0 ;
}

/*****************************************************************************/
/*                        function elpBuiltTreeModel()                       */
/*****************************************************************************/
elpFCT elptree_list *elpBuiltTreeModel(head,model,level)
elptree_list *head ;
elpmodel_list *model ;
int level ;
{
 elptree_list *tree ;
 elptree_list *pt ;

 for(tree = head ; tree != NULL ; tree = tree->NEXT)
  {
   if(elpCmpTreeModel(tree,model,level) == 1)
     break ; 
  }

 if(tree == NULL)
  {
   if(head == NULL)
    {
     head = elpAddTree(head,model,level) ;
     tree = head ;
    }
   else
    {
     tree = head ;
     pt = tree ;
     while(tree != NULL)
      {
       if(elpDiffTree(model,tree,level) == 1)
        {
         pt = tree ;
         tree = tree->NEXT ;
         continue ;
        }
       else
        {
         if(tree == head)
          {
           head = elpAddTree(head,model,level) ;
           tree = head ;
          }
         else
          {
           pt->NEXT = elpAddTree(pt->NEXT,model,level) ;
           tree = pt->NEXT ;
          }
         break ;
        }
      }
     if(tree == NULL)
       {
        pt->NEXT = elpAddTree(pt->NEXT,model,level) ;
        tree = pt->NEXT ;
       }
    }
  }
 else
  {
   if(level == elpCaseLevel)
    return(head) ;
  }

  if(level != elpCaseLevel)
     tree->DOWN = elpBuiltTreeModel(tree->DOWN,model,level+1) ;
  else
     tree->DOWN = model ;

  return(head) ;
}

/*****************************************************************************/
/*                        function elpaddmodel()                             */
/*****************************************************************************/
elpFCT elpmodel_list *elpAddModel(name,name_alias,type,index,lmin,lmax,
                                  wmin,wmax,dl,dw,ml,mw,vdd,trcase, trtechno,
                                  mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,longkey)
char *name ;
chain_list *name_alias;
int type ;
int index ;
long lmin ;
long lmax ;
long wmin ;
long wmax ;
double dl ;
double dw ;
double ml ;
double mw ;
double vdd ;
int trcase ;
int trtechno ;
double mulu0 ;
double delvt0;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc;
double sca;
double scb;
double scc;
chain_list *longkey;
{
 elpmodel_list *model ;
 int i ;
 static int warn_displayed=0 ;

 if( nf >=2 && (wmin != wmax || lmin != lmax ) ) {
   if( ! warn_displayed ) {
     warn_displayed = 1 ;
     fprintf( stderr, "warning : setting avtCharacForEachSize to a value different than yes for transistor with nf parameter greater than 1 can lead to unpredictable results !\n" );
   }
 }
 
 model = (elpmodel_list *)mbkalloc(sizeof(elpmodel_list)) ;

 if(ELP_MODEL_LIST == NULL)
   model->elpModelIndex = 0 ;
 else
   model->elpModelIndex = ELP_MODEL_LIST->elpModelIndex + 1 ;

 model->NEXT = ELP_MODEL_LIST ;
 ELP_MODEL_LIST = model ;

 model->elpTransType = type ;
 model->elpTransModel = elpMOS ;
 model->elpTransIndex = index ;
 model->elpVoltage[elpVDDMAX] = vdd ;
 model->elpVoltage[elpVBULK] = vbulk;
 model->elpTransCase = trcase ;
 model->elpTransTechno = trtechno ;
 model->elpModelNameAlias = name_alias;
 
 if(name != NULL) {
   model->elpModelName = namealloc(name) ;
   if (type == elpNMOS &&  !mbk_istransn(model->elpModelName)) TNMOS = addchain(TNMOS, model->elpModelName);
   else if (type == elpPMOS &&  !mbk_istransp(model->elpModelName)) TPMOS = addchain(TPMOS, model->elpModelName);
 }
 else
     {
      if(type == elpNMOS)
       {
        model->elpModelName = namealloc("TN") ;
        if (!mbk_istransn(model->elpModelName)) TNMOS = addchain(TNMOS, model->elpModelName);
       }
      else
       {
        model->elpModelName = namealloc("TP") ;
        if (!mbk_istransp(model->elpModelName)) TPMOS = addchain(TPMOS, model->elpModelName);
       }
     }

 /* Initialisation des parametres du modele */

 model->elpShrink[elpLMLT] = ml ;
 model->elpShrink[elpWMLT] = mw ;
 model->elpShrink[elpDL]   = dl ;
 model->elpShrink[elpDW]   = dw ;
 model->elpShrink[elpDLC]   = dl ;
 model->elpShrink[elpDWC]   = dw ;
 model->elpShrink[elpDWCJ]  = dw ;
 
 model->elpRange[elpLMIN]  = lmin ;
 model->elpRange[elpLMAX]  = lmax ;
 model->elpRange[elpWMIN]  = wmin ;
 model->elpRange[elpWMAX]  = wmax ;

 model->elpTemp = ELPINITTEMP ;
 
 for (i = 0 ; i < elpMODELNUM ; i++)
     model->elpModel[i] = 0.0 ;
 model->elpModel[elpMULU0] = mulu0;
 model->elpModel[elpDELVT0] = delvt0;
 model->elpModel[elpSA] = sa;
 model->elpModel[elpSB] = sb;
 model->elpModel[elpSD] = sd;
 model->elpModel[elpNF] = nf;
 model->elpModel[elpM] = m;
 model->elpModel[elpNRS] = nrs;
 model->elpModel[elpNRD] = nrd;
 model->elpModel[elpSC] = sc;
 model->elpModel[elpSCA] = sca;
 model->elpModel[elpSCB] = scb;
 model->elpModel[elpSCC] = scc;
 model->longkey=dupchainlst(longkey);
         
 model->elpVoltage[elpVDEG] = ELPMINVOLTAGE ;
 model->elpVoltage[elpVTI] = ELPMINVOLTAGE ;
 
 for (i = 0 ; i < elpCAPANUM ; i++)
     model->elpCapa[i] = 0.0 ;

 for (i = 0 ; i < elpRSSNUM ; i++)
     model->elpRss[i] = 0.0 ;

 ELP_HEAD_TREE = elpBuiltTreeModel(ELP_HEAD_TREE,model,elpNameLevel) ;

 return model ;
}

/*****************************************************************************/
/*                        function elpFreeTreeModel()                        */
/*****************************************************************************/
elpFCT void elpFreeTreeModel(head,level)
elptree_list *head ;
int level ;
{
 elptree_list *tree ;

 while(head != NULL)
  {
   tree = head->NEXT ;
   if(level != elpCaseLevel)
     elpFreeTreeModel(head->DOWN,level + 1) ;
   mbkfree(head) ;
   head = tree ;
  }
}


/*****************************************************************************/
/*                        function elpPrintData()                            */
/*****************************************************************************/
elpFCT void elpPrintData(elptree_list *tree, int level)
{
 chain_list *chain,*chain_alias;
 ptype_list *pt;
 
 switch(level)
   {
	case elpNameLevel   : fprintf(stdout, "Model Name : %s\n", (char *)tree->DATA1) ;
                          chain_alias = (chain_list*)tree->DATA2;
                          if (chain_alias) 
                           {
                            fprintf (stdout,"Alias ");
                            for (chain = chain_alias ; chain ; chain = chain->NEXT)
                                fprintf(stdout,": %s ",(char*)chain->DATA);
                            fprintf (stdout,"\n");
                           }
							 break ;
    case elpLengthLevel : fprintf(stdout, "LMIN = %ld\n", tree->DATA1) ; 
						  fprintf(stdout, "LMAX = %ld\n", tree->DATA2) ; 
							 break ;
    case elpWidthLevel  : fprintf(stdout, "WMIN = %ld\n", tree->DATA1) ; 
						  fprintf(stdout, "WMAX = %ld\n", tree->DATA2) ; 
							 break ;
	case elpVDDLevel    : fprintf(stdout, "VDDMAX = %ld\n", tree->DATA1) ;  
						     break ;
	case elpMULU0Level  : fprintf(stdout, "MULU0 = %ld\n", tree->DATA1) ;  
						     break ;
	case elpDELVT0Level : fprintf(stdout, "DELVT0 = %ld\n", tree->DATA1) ;  
						     break ;
	case elpSALevel     : fprintf(stdout, "SA = %ld\n", tree->DATA1) ;  
						     break ;
	case elpSBLevel     : fprintf(stdout, "SB = %ld\n", tree->DATA1) ;  
						     break ;
	case elpSDLevel     : fprintf(stdout, "SD = %ld\n", tree->DATA1) ;  
						     break ;
	case elpNFLevel     : fprintf(stdout, "NF = %ld\n", tree->DATA1) ;  
						     break ;
	case elpVBULKLevel  : fprintf(stdout, "VBULK = %ld\n", tree->DATA1) ;  
			 			     break ;
        case elpParamContextLevel :
                          fprintf(stdout, "ParamContext :");
                          for (pt=(ptype_list *)tree->DATA1; pt!=NULL; pt=pt->NEXT)
                             {
                               if ((long)pt==tree->DATA1)
                                  fprintf(stdout," %s (fastkey=%ld)", (char *)pt->DATA, pt->TYPE);
                               else
                                  fprintf(stdout," %s=%g", (char *)pt->DATA, *(float *)&pt->TYPE);
                             }
                          fprintf(stdout,"\n");
                          break;
                          
	case elpCaseLevel   : fprintf(stdout, "Case value : %ld\n", tree->DATA1) ;  
						     break ;
	case elpSCLevel     : fprintf(stdout, "SC = %ld\n", tree->DATA1) ;  
						     break ;
	case elpSCALevel    : fprintf(stdout, "SCA = %ld\n", tree->DATA1) ;  
						     break ;
	case elpSCBLevel    : fprintf(stdout, "SCB = %ld\n", tree->DATA1) ;  
						     break ;
	case elpSCCLevel    : fprintf(stdout, "SCC = %ld\n", tree->DATA1) ;  
						     break ;
   }
	
}


/*****************************************************************************/
/*                        function elpPrintTreeModel()                       */
/*****************************************************************************/
elpFCT void elpPrintTreeModel(head,level)
elptree_list *head ;
int level ;
{
 elptree_list *tree ;

 if (!head) {
	 fprintf(stdout, "Empty tree!!!\n") ;
     return ;
 }
 
 for (tree = head ; tree ; tree = tree->NEXT) {
      elpPrintData(tree, level) ;
	  if(level != elpCaseLevel)
         elpPrintTreeModel(tree->DOWN, level + 1) ;
 }
}

/*****************************************************************************/
/*                        function elpFreemodel()                            */
/*****************************************************************************/
elpFCT void elpFreeModel()
{
 elpmodel_list *model ;

 while(ELP_MODEL_LIST != NULL)
  {
   model = ELP_MODEL_LIST ;
   ELP_MODEL_LIST = ELP_MODEL_LIST->NEXT ;
   freechain (model->elpModelNameAlias) ;
   freechain(model->longkey);
   mbkfree(model) ;
  }

 ELP_MODEL_LIST = NULL ;
 elpFreeTreeModel(ELP_HEAD_TREE,elpNameLevel) ;
 ELP_HEAD_TREE = NULL ;

 elpGeneral[elpACM] = 0.0 ;
 elpGeneral[elpTEMP] = ELPINITTEMP ;
 elpGeneral[elpSLOPE] = 200.0 ;
 elpGeneral[elpGVDDMAX] = ELPMINVOLTAGE ;
 elpGeneral[elpGVDDBEST] = ELPMINVOLTAGE ;
 elpGeneral[elpGVDDWORST] = ELPMINVOLTAGE ;
 elpGeneral[elpGDTHR] = ELPINITTHR ;
 elpGeneral[elpGSHTHR] = ELPINITTHR ;
 elpGeneral[elpGSLTHR] = ELPINITTHR ;
}

/*****************************************************************************/
/*                        function elpCmpTreeLotrs()                         */
/*****************************************************************************/
elpFCT int elpCmpTreeLotrs (tree,lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,
                            nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,level,longkey)
elptree_list *tree ;
lotrs_list *lotrs ;
double vdd ;
int transcase ;
double mulu0;
double delvt0;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc;
double sca;
double scb;
double scc;
int level ;
chain_list *longkey;
{
 int res = 0 ;
 char *name ;
 chain_list *chain;

 switch(level) 
   {
    case elpNameLevel   : name = getlotrsmodel(lotrs) ;
                          if((tree->DATA1 == (long)NULL) ||
                             (tree->DATA1 == (long)name))
                            res = 1 ;
                          if (res == 0) 
                           {
                            for (chain = (chain_list*)tree->DATA2 ; chain ;
                                    chain = chain->NEXT)
                                if ((char*)chain->DATA == name)
                                    res = 1 ;
                           }
                          break ;
    case elpLengthLevel :  if((lotrs->LENGTH >= tree->DATA1) && 
                             ((lotrs->LENGTH <= tree->DATA2) ||
                              ((lotrs->LENGTH == tree->DATA2) &&
                               (tree->DATA1 == tree->DATA2))))
                           {
                            if(lotrs->LENGTH > tree->DATA2)
                              res = 2 ;
                            else
                              res = 1 ;
                           }
                          break ;
    case elpWidthLevel  : if((lotrs->WIDTH >= tree->DATA1) &&
                             ((lotrs->WIDTH <= tree->DATA2) ||
                              ((lotrs->WIDTH == tree->DATA2) &&
                               (tree->DATA1 == tree->DATA2))))
                           {
                            if(lotrs->WIDTH > tree->DATA2)
                              res = 2 ;
                            else
                              res = 1 ;
                           }
                          break ;
    case elpVDDLevel    : if(tree->DATA1 == elpDouble2Long(vdd*SCALE_X)) 
                           res = 1 ;
                          else if(tree->DATA1 < 0) {
                              if (elpDouble2Long (vdd*SCALE_X ) == 
                                  elpDouble2Long(elpGeneral[elpGVDDMAX]*SCALE_X))
                                  res = 1 ;
                          }
                          break ;
    case elpMULU0Level  : if ( V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE < 0.0 ) {
                            if (tree->DATA1 == elpDouble2Long (mulu0*ELPPRECISION))
                              res = 1;
                          }
                          else {
                            mulu0 = elpDouble2Long (mulu0*ELPPRECISION);
                            if ( mulu0 >= tree->DATA1 && mulu0 <= tree->DATA2 )
                              res = 1;
                          }
                          break ;
    case elpDELVT0Level : if ( V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE < 0.0 ) {
                            if (tree->DATA1 == elpDouble2Long (delvt0*ELPPRECISION))
                              res = 1;
                          }
                          else {
                            delvt0 = elpDouble2Long (delvt0*ELPPRECISION);
                            if ( delvt0 >= tree->DATA1 && delvt0 <= tree->DATA2 )
                              res = 1;
                          }
                          break ;
    case elpSALevel     : if ( sa < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (sa*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sa = elpDouble2Long (sa*ELPPRECISION2);
                              if ( sa >= tree->DATA1 && sa <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSBLevel     : if ( sb < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (sb*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sb = elpDouble2Long (sb*ELPPRECISION2);
                              if ( sb >= tree->DATA1 && sb <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSDLevel     : if ( sd < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (sd*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sd = elpDouble2Long (sd*ELPPRECISION2);
                              if ( sd >= tree->DATA1 && sd <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpNFLevel     : if ( nf < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( ELP_NF_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (nf))
                                res = 1;
                            }
                            else {
                              nf = elpDouble2Long (nf);
                              if ( nf >= tree->DATA1 && nf <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpMLevel     : if ( ELP_M_MARGIN < 0.0 ) {
                            if (tree->DATA1 == elpDouble2Long (m))
                              res = 1;
                          }
                          else {
                            m = elpDouble2Long (m);
                            if ( m >= tree->DATA1 && m <= tree->DATA2 )
                              res = 1;
                          }
                          break ;
    case elpNRSLevel    : if ( nrs < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( ELP_NRS_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (nrs*ELPPRECISION))
                                res = 1;
                            }
                            else {
                              nrs = elpDouble2Long (nrs*ELPPRECISION);
                              if ( nrs >= tree->DATA1 && nrs <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpNRDLevel    : if ( nrd < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( ELP_NRD_MARGIN < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (nrd*ELPPRECISION))
                                res = 1;
                            }
                            else {
                              nrd = elpDouble2Long (nrd*ELPPRECISION);
                              if ( nrd >= tree->DATA1 && nrd <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpVBULKLevel  : if(tree->DATA1 == elpDouble2Long (vbulk*SCALE_X)) 
                           res = 1 ;
                          else if(tree->DATA1 < ELPMINVBULK ) {
                              if (MLO_IS_TRANSP(lotrs->TYPE) && (elpDouble2Long (vbulk*SCALE_X )) == 
                                  elpDouble2Long(elpGeneral[elpGVDDMAX]*SCALE_X))
                                  res = 1 ;
                              if (MLO_IS_TRANSN(lotrs->TYPE) && (elpDouble2Long (vbulk*SCALE_X ) == 0))
                                  res = 1 ;
                          }
                          break ;
    case elpParamContextLevel  : if(elp_is_same_paramcontext((chain_list *)tree->DATA1,longkey))
                                  res = 1 ;
                                 break ;                         
    case elpCaseLevel   : if((tree->DATA1 == (long)transcase) || 
                             //(tree->DATA1 == elpTYPICAL) || 
                             (transcase == elpTYPICAL))
                           res = 1 ;
                          break ;
    case elpSCLevel     : if ( sc < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (sc*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sc = elpDouble2Long (sc*ELPPRECISION2);
                              if ( sc >= tree->DATA1 && sc <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSCALevel    : if ( sca < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (sca*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              sca = elpDouble2Long (sca*ELPPRECISION2);
                              if ( sca >= tree->DATA1 && sca <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSCBLevel    : if ( scb < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (scb*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              scb = elpDouble2Long (scb*ELPPRECISION2);
                              if ( scb >= tree->DATA1 && scb <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
    case elpSCCLevel    : if ( scc < ELPMINVALUE ) 
                            res = 1;
                          else {
                            if ( V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE < 0.0 ) {
                              if (tree->DATA1 == elpDouble2Long (scc*ELPPRECISION2))
                                res = 1;
                            }
                            else {
                              scc = elpDouble2Long (scc*ELPPRECISION2);
                              if ( scc >= tree->DATA1 && scc <= tree->DATA2 )
                                res = 1;
                            }
                          }
                          break ;
   }

 return res ;
}

/*****************************************************************************\
                          function elpDupModel
\*****************************************************************************/
elpFCT elpmodel_list *elpDupModel (lotrs_list *lotrs,elpmodel_list *model,int type,double vdd,int transcase,
                                   double mulu0,double delvt0,double sa,double sb,double sd,double nf,double m,
                                   double nrs,double nrd,double vbulk,double sc,double sca,double scb,double scc)
/*lotrs_list *lotrs;
elpmodel_list *model;
int type;
double vdd;
int transcase;
double mulu0;
double delvt0;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc;
double sca;
double scb;
double scc;*/
{
 elpmodel_list *nmodel ;

 nmodel = elpAddModel(getlotrsmodel(lotrs),model->elpModelNameAlias,type,elpNOINDEX,
                      lotrs->LENGTH,
                      lotrs->LENGTH,
                      lotrs->WIDTH,
                      lotrs->WIDTH,
                      model->elpShrink[elpDL],
                      model->elpShrink[elpDW],
                      model->elpShrink[elpLMLT],
                      model->elpShrink[elpWMLT],
                      vdd, transcase, 
                      model->elpTransTechno,
                      mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,model->longkey
                      ) ;
 nmodel->elpTransModel = model->elpTransModel ;
 nmodel->elpTemp = model->elpTemp ;
 nmodel->elpModel[elpVT] = model->elpModel[elpVT] ;
 nmodel->elpModel[elpVT0] = model->elpModel[elpVT0] ;
 nmodel->elpModel[elpKT] = model->elpModel[elpKT] ;
 nmodel->elpModel[elpKS] = model->elpModel[elpKS] ;
 nmodel->elpModel[elpKR] = model->elpModel[elpKR] ;
 nmodel->elpModel[elpMULU0] = model->elpModel[elpMULU0] ;
 nmodel->elpModel[elpDELVT0] = model->elpModel[elpDELVT0] ;
 nmodel->elpModel[elpSA] = model->elpModel[elpSA] ;
 nmodel->elpModel[elpSB] = model->elpModel[elpSB] ;
 nmodel->elpModel[elpSD] = model->elpModel[elpSD] ;
 nmodel->elpModel[elpNF] = model->elpModel[elpNF] ;
 nmodel->elpModel[elpM] = model->elpModel[elpM] ;
 nmodel->elpModel[elpNRS] = model->elpModel[elpNRS] ;
 nmodel->elpModel[elpNRD] = model->elpModel[elpNRD] ;
 nmodel->elpModel[elpA] = model->elpModel[elpA] ;
 nmodel->elpModel[elpB] = model->elpModel[elpB] ;
 nmodel->elpModel[elpRT] = model->elpModel[elpRT] ;
 nmodel->elpModel[elpKRT] = model->elpModel[elpKRT] ;
 nmodel->elpModel[elpRS] = model->elpModel[elpRS] ;
 nmodel->elpModel[elpKRS] = model->elpModel[elpKRS] ;
 nmodel->elpModel[elpSC] = model->elpModel[elpSC] ;
 nmodel->elpModel[elpSCA] = model->elpModel[elpSCA] ;
 nmodel->elpModel[elpSCB] = model->elpModel[elpSCB] ;
 nmodel->elpModel[elpSCC] = model->elpModel[elpSCC] ;
 nmodel->elpVoltage[elpVDEG] = model->elpVoltage[elpVDEG] ;
 nmodel->elpVoltage[elpVTI] = model->elpVoltage[elpVTI] ;
 nmodel->elpCapa[elpCGS] = model->elpCapa[elpCGS] ;
 nmodel->elpCapa[elpCGS0] = model->elpCapa[elpCGS0] ;
 nmodel->elpCapa[elpCGSU] = model->elpCapa[elpCGSU] ;
 nmodel->elpCapa[elpCGSU0] = model->elpCapa[elpCGSU0] ;
 nmodel->elpCapa[elpCGSUMIN] = model->elpCapa[elpCGSUMIN] ;
 nmodel->elpCapa[elpCGSUMAX] = model->elpCapa[elpCGSUMAX] ;
 nmodel->elpCapa[elpCGSD] = model->elpCapa[elpCGSD] ;
 nmodel->elpCapa[elpCGSD0] = model->elpCapa[elpCGSD0] ;
 nmodel->elpCapa[elpCGSDMIN] = model->elpCapa[elpCGSDMIN] ;
 nmodel->elpCapa[elpCGSDMAX] = model->elpCapa[elpCGSDMAX] ;
 nmodel->elpCapa[elpCGP] = model->elpCapa[elpCGP] ;
 nmodel->elpCapa[elpCGPUMIN] = model->elpCapa[elpCGPUMIN] ;
 nmodel->elpCapa[elpCGPUMAX] = model->elpCapa[elpCGPUMAX] ;
 nmodel->elpCapa[elpCGPDMIN] = model->elpCapa[elpCGPDMIN] ;
 nmodel->elpCapa[elpCGPDMAX] = model->elpCapa[elpCGPDMAX] ;
 nmodel->elpCapa[elpCGPO] = model->elpCapa[elpCGPO];
 nmodel->elpCapa[elpCGPOC] = model->elpCapa[elpCGPOC];
 nmodel->elpCapa[elpCGD] = model->elpCapa[elpCGD] ;
 nmodel->elpCapa[elpCGD0] = model->elpCapa[elpCGD0] ;
 nmodel->elpCapa[elpCGD1] = model->elpCapa[elpCGD1] ;
 nmodel->elpCapa[elpCGD2] = model->elpCapa[elpCGD2] ;
 nmodel->elpCapa[elpCGDC] = model->elpCapa[elpCGDC] ;
 nmodel->elpCapa[elpCGDC0] = model->elpCapa[elpCGDC0] ;
 nmodel->elpCapa[elpCGDC1] = model->elpCapa[elpCGDC1] ;
 nmodel->elpCapa[elpCGDC2] = model->elpCapa[elpCGDC2] ;
 nmodel->elpCapa[elpCGSI] = model->elpCapa[elpCGSI] ;
 nmodel->elpCapa[elpCGSIC] = model->elpCapa[elpCGSIC] ;
 nmodel->elpCapa[elpCDS] = model->elpCapa[elpCDS] ;
 nmodel->elpCapa[elpCDSU] = model->elpCapa[elpCDSU] ;
 nmodel->elpCapa[elpCDSD] = model->elpCapa[elpCDSD] ;
 nmodel->elpCapa[elpCDP] = model->elpCapa[elpCDP] ;
 nmodel->elpCapa[elpCDPU] = model->elpCapa[elpCDPU] ;
 nmodel->elpCapa[elpCDPD] = model->elpCapa[elpCDPD] ;
 nmodel->elpCapa[elpCDW] = model->elpCapa[elpCDW] ;
 nmodel->elpCapa[elpCDWU] = model->elpCapa[elpCDWU] ;
 nmodel->elpCapa[elpCDWD] = model->elpCapa[elpCDWD] ;
 nmodel->elpCapa[elpCSS] = model->elpCapa[elpCSS] ;
 nmodel->elpCapa[elpCSSU] = model->elpCapa[elpCSSU] ;
 nmodel->elpCapa[elpCSSD] = model->elpCapa[elpCSSD] ;
 nmodel->elpCapa[elpCSP] = model->elpCapa[elpCSP] ;
 nmodel->elpCapa[elpCSPU] = model->elpCapa[elpCSPU] ;
 nmodel->elpCapa[elpCSPD] = model->elpCapa[elpCSPD] ;
 nmodel->elpCapa[elpCSW] = model->elpCapa[elpCSW] ;
 nmodel->elpCapa[elpCSWU] = model->elpCapa[elpCSWU] ;
 nmodel->elpCapa[elpCSWD] = model->elpCapa[elpCSWD] ;
 nmodel->elpRacc[elpRACCS] = model->elpRacc[elpRACCS] ;
 nmodel->elpRacc[elpRACCD] = model->elpRacc[elpRACCD] ;

 return nmodel;
}

/*****************************************************************************\
                        function elpDelModel2Lotrs()                       
\*****************************************************************************/
elpFCT void elpDelModel2Lotrs (lotrs_list *lotrs)
{
  if ( lotrs ) {
    if ( getptype (lotrs->USER, ELP_LOTRS_MODEL ) != NULL ) 
      lotrs->USER = delptype (lotrs->USER, ELP_LOTRS_MODEL);
  }
}

/*****************************************************************************\
                        function elpSetModel2Lotrs()                       
\*****************************************************************************/
elpFCT int elpSetModel2Lotrs (elpmodel_list *model, lotrs_list *lotrs)
{
  int set=0;

  if ( model && lotrs ) {
    if ( !getptype (lotrs->USER, ELP_LOTRS_MODEL ) ) {
      lotrs->USER = addptype (lotrs->USER, ELP_LOTRS_MODEL, model);
      set=1;
    }
  }
  return set;
}
                              
/*****************************************************************************\
                        function elpGetModelFromLotrs()                       
\*****************************************************************************/
elpFCT elpmodel_list *elpGetModelFromLotrs (lotrs_list *lotrs)
{
  elpmodel_list *model=NULL;
  ptype_list *ptype;

  if ( lotrs ) {
    if ( (ptype = getptype (lotrs->USER, ELP_LOTRS_MODEL )) ) 
      model = (elpmodel_list*)ptype->DATA;
  }
  return model;
}

/*****************************************************************************/
/*                        function elpGetModelList()                         */
/*****************************************************************************/
elpFCT elpmodel_list *elpGetModelList (lotrs_list *lotrs,double vdd,int transcase,
                                       double mulu0,double delvt0,double sa,double sb,double sd,double nf,
                                       double m,double nrs,double nrd,double vbulk,double sc,double sca,double scb,double scc)
/*lotrs_list *lotrs ;
double vdd ;
int transcase ;
double mulu0;
double delvt0;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc;
double sca;
double scb;
double scc;*/
{
 elpmodel_list *model = NULL ;
 char *modelname;
 int type = (MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS) ;

 modelname = getlotrsmodel (lotrs);
 // first : try to get the same model name...
 for(model = ELP_MODEL_LIST ; model != NULL ; model = model->NEXT)
   {
    if((modelname == model->elpModelName) && 
       (elpDouble2Long(vdd*SCALE_X ) == 
        elpDouble2Long(model->elpVoltage[elpVDDMAX]*SCALE_X ) ) &&
       (elp_is_valcomprise (elpDouble2Long(mulu0*ELPPRECISION) , 
        elpDouble2Long(model->elpModel[elpMULU0]*ELPPRECISION),V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE )) &&
       (elp_is_valcomprise(elpDouble2Long(delvt0*ELPPRECISION) ,
        elpDouble2Long(model->elpModel[elpDELVT0]*ELPPRECISION),V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sa*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSA]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sb*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSB]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sd*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSD]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(nf) ,
        elpDouble2Long(model->elpModel[elpNF]),ELP_NF_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(m) ,
        elpDouble2Long(model->elpModel[elpM]),ELP_M_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(nrs*ELPPRECISION) ,
        elpDouble2Long(model->elpModel[elpNRS]*ELPPRECISION),ELP_NRS_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(nrd*ELPPRECISION) ,
        elpDouble2Long(model->elpModel[elpNRD]*ELPPRECISION),ELP_NRD_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(sc*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSC]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sca*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSCA]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(scb*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSCB]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(scc*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSCC]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE) ) &&
       (elpDouble2Long(vbulk*SCALE_X ) == 
        elpDouble2Long(model->elpVoltage[elpVBULK]*SCALE_X ) ) &&
        ((transcase == model->elpTransCase) || (model->elpTransCase == elpTYPICAL)))
     {
//      if (!mbk_isdioden(getlotrsmodel(lotrs)) && !mbk_isdiodep(getlotrsmodel(lotrs)))
       {
        if(lotrs->TRNAME != NULL)
          avt_errmsg(ELP_ERRMSG, "007", AVT_ERROR, lotrs->TRNAME, model->elpModelName);
//          elpError(1007,lotrs->TRNAME,model->elpModelName) ;
        else
          avt_errmsg(ELP_ERRMSG, "007", AVT_ERROR, modelname, model->elpModelName);
//          elpError(1007,modelname,model->elpModelName) ;
       }
      elpDupModel (lotrs,model,type,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc);
      break ;
     }
   }
 if ( !model )
 for(model = ELP_MODEL_LIST ; model != NULL ; model = model->NEXT)
   {
    if((type == model->elpTransType) && 
       (elpDouble2Long(vdd*SCALE_X ) == 
        elpDouble2Long(model->elpVoltage[elpVDDMAX]*SCALE_X ) ) &&
       (elp_is_valcomprise (elpDouble2Long(mulu0*ELPPRECISION) , 
        elpDouble2Long(model->elpModel[elpMULU0]*ELPPRECISION),V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE )) &&
       (elp_is_valcomprise(elpDouble2Long(delvt0*ELPPRECISION) ,
        elpDouble2Long(model->elpModel[elpDELVT0]*ELPPRECISION),V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sa*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSA]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sb*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSB]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sd*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSD]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sc*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSC]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SC_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(sca*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSCA]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SCA_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(scb*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSCB]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SCB_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(scc*ELPPRECISION2) ,
        elpDouble2Long(model->elpModel[elpSCC]*ELPPRECISION2),V_FLOAT_TAB[__ELP_SCC_MARGIN].VALUE) ) &&
       (elp_is_valcomprise(elpDouble2Long(nf) ,
        elpDouble2Long(model->elpModel[elpNF]),ELP_NF_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(m) ,
        elpDouble2Long(model->elpModel[elpM]),ELP_M_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(nrs*ELPPRECISION) ,
        elpDouble2Long(model->elpModel[elpNRS]*ELPPRECISION),ELP_NRS_MARGIN) ) &&
       (elp_is_valcomprise(elpDouble2Long(nrd*ELPPRECISION) ,
        elpDouble2Long(model->elpModel[elpNRD]*ELPPRECISION),ELP_NRD_MARGIN) ) &&
       (elpDouble2Long(vbulk*SCALE_X ) == 
        elpDouble2Long(model->elpVoltage[elpVBULK]*SCALE_X ) ) &&
        ((transcase == model->elpTransCase) || (model->elpTransCase == elpTYPICAL)))
         {
//          if (!mbk_isdioden(getlotrsmodel(lotrs)) && !mbk_isdiodep(getlotrsmodel(lotrs)))
           {
            if(lotrs->TRNAME != NULL)
              avt_errmsg(ELP_ERRMSG, "007", AVT_ERROR, lotrs->TRNAME, model->elpModelName);
              //elpError(1007,lotrs->TRNAME,model->elpModelName) ;
            else
              avt_errmsg(ELP_ERRMSG, "007", AVT_ERROR, modelname, model->elpModelName);
              //elpError(1007,modelname,model->elpModelName) ;
           }
          elpDupModel (lotrs,model,type,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc);
          break ;
         }
   }

 elpSetModel2Lotrs ( model, lotrs );

 return model ;
}

/*****************************************************************************/
/*                        function elpGetModelTree()                         */
/*****************************************************************************/
static elpFCT elpmodel_list *elpGetModelTree_int(head,lotrs,vdd,transcase,
                                      mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,level,longkey)
elptree_list *head ;
lotrs_list *lotrs ;
double vdd ;
int transcase ;
double mulu0;
double delvt0;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc;
double sca;
double scb;
double scc;
int level ;
chain_list *longkey;
{
 elptree_list *tree ;
 elpmodel_list *resmodel ;
 int ret;
 
 for(tree = head ; tree != NULL ; tree = tree->NEXT)
  {
          
   if((ret=elpCmpTreeLotrs(tree,lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,level,longkey)) == 2)
     {
      tree = NULL ;
      break ;
     }
   if(ret == 1)
    {
     if(level != elpCaseLevel)
      {
       resmodel = elpGetModelTree_int(tree->DOWN,lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,level+1,longkey) ;
       if((resmodel != NULL) || (level == elpNameLevel))
          return(resmodel) ;
      }
     else
       return(tree->DOWN) ;
    }
  }
 return(NULL) ;
}

elpFCT elpmodel_list *elpGetModelTree(head,lotrs,vdd,transcase,
                                      mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,level)
elptree_list *head ;
lotrs_list *lotrs ;
double vdd ;
int transcase ;
double mulu0;
double delvt0;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc ;
double sca ;
double scb ;
double scc ;
int level ;
{
   chain_list *longkey;
   elpmodel_list *em;
   elp_get_key(lotrs, &longkey);
   em=elpGetModelTree_int(head,lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,level,longkey);
   return em;
}


void fill_elp_lotrs_param( elp_lotrs_param *pt,
                           double mulu0,
                           double delvt0,
                           double sa,
                           double sb,
                           double sd,
                           double nf,
                           double m,
                           double nrs,
                           double nrd,
                           double vbulk,
                           double sc,
                           double sca,
                           double scb,
                           double scc,
                           lotrs_list *lt
                          )
{
   chain_list *longkey;
   int i;
   pt->PARAM[elpMULU0]  = mulu0;
   pt->PARAM[elpDELVT0] = delvt0;
   pt->PARAM[elpSA]     = sa;
   pt->PARAM[elpSB]     = sb;
   pt->PARAM[elpSD]     = sd;
   pt->PARAM[elpNF]     = nf;
   pt->PARAM[elpM]      = m;
   pt->PARAM[elpNRS]    = nrs;
   pt->PARAM[elpNRD]    = nrd;
   pt->PARAM[elpSC]     = sc;
   pt->PARAM[elpSCA]    = sca;
   pt->PARAM[elpSCB]    = scb;
   pt->PARAM[elpSCC]    = scc;
   pt->VBULK            = vbulk;
   pt->ISVBSSET         = 0;
   elp_get_key(lt, &longkey);
   pt->longkey=dupchainlst(longkey);
   for (i = 0 ; i < __MCC_LAST_SAVED ; i++)
    pt->MCC_SAVED[i] = ELPINITVALUE;

}

/*****************************************************************************/
/*                        function elpSearchOrGenModel                       */
/*****************************************************************************/
elpFCT elpmodel_list *elpSearchOrGenModel (lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc)
lotrs_list *lotrs ;
double vdd ;
int transcase ;
double mulu0 ; 
double delvt0 ;
double sa;
double sb;
double sd;
double nf;
double m;
double nrs;
double nrd;
double vbulk;
double sc;
double sca;
double scb;
double scc;
{
 elpmodel_list *model = NULL;
 int lotrstype ;
 elp_lotrs_param ptlotrs_param;
#ifdef DELAY_DEBUG_STAT
 static int cnt=0;
#endif
 
 model = elpGetModelTree(ELP_HEAD_TREE,lotrs,vdd,transcase, 
                         mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,elpNameLevel) ;

 if((model == NULL) && (ELP_LOAD_FILE_TYPE || ELP_GENPARAM ))
  {
   lotrstype = MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS ;
  
   fill_elp_lotrs_param( &ptlotrs_param, mulu0, delvt0, sa, sb, sd, nf, m ,nrs, nrd, vbulk, sc, sca, scb, scc, lotrs );

   elpGenParam(getlotrsmodel(lotrs), lotrstype,(double)lotrs->LENGTH/(double)(SCALE_X),
                (double)lotrs->WIDTH/(double)(SCALE_X),vdd,lotrs,transcase,&ptlotrs_param) ;

   freechain(ptlotrs_param.longkey);
   model = elpGetModelTree (ELP_HEAD_TREE,lotrs,vdd,transcase,
                            mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc,elpNameLevel) ;
#ifdef DELAY_DEBUG_STAT
   cnt++;
   if ((cnt % 100)==0)
     fprintf(stdout,"\relp: %05d\r", cnt); fflush(stdout);
#endif
   if(model == NULL) {
       if((model = elpGetModelList(lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc)) == NULL)
         {
          if(lotrs->TRNAME != NULL)
           {
            avt_errmsg(ELP_ERRMSG, "009", AVT_ERROR, lotrs->TRNAME);
            //elpError(1009,lotrs->TRNAME) ;
            EXIT(1);
           }
          else
           {
            avt_errmsg(ELP_ERRMSG, "009", AVT_ERROR, getlotrsmodel(lotrs));
            //elpError(1009,getlotrsmodel(lotrs)) ;
            EXIT(1);
           }
         }
   }
  }
 else if(model == NULL)
  {
   if((model = elpGetModelList(lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc)) == NULL)
     {
      if(lotrs->TRNAME != NULL)
       {
        avt_errmsg(ELP_ERRMSG, "009", AVT_ERROR, lotrs->TRNAME);
        //elpError(1009,lotrs->TRNAME) ;
        EXIT(1);
       }
      else
       {
         avt_errmsg(ELP_ERRMSG, "009", AVT_ERROR, getlotrsmodel(lotrs));
        //elpError(1009,getlotrsmodel(lotrs)) ;
        EXIT(1);
       }
     }
  }

 return model;
}


/*****************************************************************************/
/*                        function elpGetModelWithLotrsParams ()             */
/*****************************************************************************/
elpFCT elpmodel_list *elpGetModelWithLotrsParams(lotrs,lotrsparams,vdd,transcase)
lotrs_list *lotrs ;
double vdd ;
int transcase ;
elp_lotrs_param *lotrsparams;
{
 elpmodel_list *model ;
 double mulu0,delvt0,sa,sb,sd,nf,vbulk,sc,sca,scb,scc;
 double m,nrs,nrd;

 mulu0  = lotrsparams->PARAM[elpMULU0];
 delvt0 = lotrsparams->PARAM[elpDELVT0];
 sa = lotrsparams->PARAM[elpSA];
 sb = lotrsparams->PARAM[elpSB];
 sd = lotrsparams->PARAM[elpSD];
 sc = lotrsparams->PARAM[elpSC];
 sca = lotrsparams->PARAM[elpSCA];
 scb = lotrsparams->PARAM[elpSCB];
 scc = lotrsparams->PARAM[elpSCC];
 nf = lotrsparams->PARAM[elpNF];
 m = lotrsparams->PARAM[elpM];
 nrs = lotrsparams->PARAM[elpNRS];
 nrd = lotrsparams->PARAM[elpNRD];
 vbulk = lotrsparams->VBULK;

 model = elpSearchOrGenModel (lotrs,vdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc);

 return model ;
}

/*****************************************************************************/
/*                        function elpGetVdd4UnusedLotrs()                   */
/*****************************************************************************/
elpFCT double elpGetVdd4UnusedLotrs(lotrs_list *lotrs)
{
  double vdd=-1.0;
  float vbulk;

  if (MLO_IS_TRANSP(lotrs->TYPE) ) {
    if ( lotrs->BULK && lotrs->BULK->SIG && 
        getlosigalim(lotrs->BULK->SIG, &vbulk) ) 
      vdd = vbulk;
  }
  if ( vdd <= 0.0 )
    vdd = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;

  return vdd;
}

/*****************************************************************************/
/*                        function elpGetModel()                             */
/*****************************************************************************/
elpFCT elpmodel_list *elpGetModel(lotrs,vdd,transcase)
lotrs_list *lotrs ;
double vdd ;
int transcase ;
{
 elpmodel_list *model ;
 double mulu0,delvt0,vbulk,sa,sb,sd,nf,sc,sca,scb,scc;
 double m,nrs,nrd;
 char *trsmodel;
 double newvdd;
 float alim;

 if ( (model = elpGetModelFromLotrs (lotrs)) )
   return model;

 trsmodel = getlotrsmodel(lotrs);
/*
 if (mbk_isdioden(trsmodel) || mbk_isdiodep(trsmodel))
   // don t try to get diode model, get only the equivalent transistor from type of diode
   return elpGetModelType(lotrs->TYPE);
*/
 elp_lotrs_param_get (lotrs,&mulu0,&delvt0,&sa,&sb,&sd,&nf,&m,&nrs,&nrd,&vbulk,&sc,&sca,&scb,&scc);

 if ( transcase != elpTYPICAL ) {
   newvdd = elpGetVddFromCorner (lotrs, transcase);
   if (MLO_IS_TRANSP(lotrs->TYPE) )
     vbulk = newvdd;  // because bulk voltage was also updated to newvdd
 }
 else
   newvdd = vdd;

 if ( getptype (lotrs->USER, CNS_UNUSED) && !cns_getlotrsalim(lotrs, 'M', &alim)) {
  newvdd = elpGetVdd4UnusedLotrs(lotrs);
  if (MLO_IS_TRANSP(lotrs->TYPE) )
    vbulk = newvdd;  // to keep vbs=0.0
  else 
    vbulk = 0.0; 
 }
 else if (MLO_IS_TRANSP(lotrs->TYPE) ) {
   if ( newvdd > vbulk && vbulk > 0 )
     newvdd = vbulk;
 }

 model = elpSearchOrGenModel (lotrs,newvdd,transcase,mulu0,delvt0,sa,sb,sd,nf,m,nrs,nrd,vbulk,sc,sca,scb,scc);

 elpSetModel2Lotrs (model, lotrs);

 return model ;
}

/*****************************************************************************/
/*                        function elpGetParamModel()                        */
/*****************************************************************************/
elpFCT elpmodel_list *elpGetParamModel(name,l,w,type,vdd,transcase,params)
char *name ;
double l ;
double w ;
char type ;
double vdd ;
int transcase ;
elp_lotrs_param *params;
{
 lotrs_list lotrs ;
 elpmodel_list *em;
 
 lotrs.TRNAME = NULL;
 lotrs.LENGTH = elpDouble2Long (l * SCALE_X ) ;
 lotrs.WIDTH =  elpDouble2Long (w * SCALE_X ) ;
 lotrs.TYPE = type ;
 lotrs.USER = NULL;
 lotrs.BULK = NULL;
 lotrs.GRID = NULL;
 lotrs.SOURCE = NULL;
 lotrs.DRAIN= NULL;
 addlotrsmodel(&lotrs,name) ;

 if (params->SUBCKTNAME!=NULL)
   lotrs.USER =addptype(lotrs.USER, TRANS_FIGURE, params->SUBCKTNAME);
 em=elpGetModelWithLotrsParams(&lotrs,params,vdd,transcase);
 freeptype(lotrs.USER);
 return em ;
}

/*****************************************************************************/
/*                        function elpGetModelType()                         */
/*****************************************************************************/
elpFCT elpmodel_list *elpGetModelType(typet)
char typet ;
{
 elpmodel_list *model ;
 int type = (MLO_IS_TRANSN(typet) ? elpNMOS : elpPMOS) ;

 for(model = ELP_MODEL_LIST ; model != NULL ; model = model->NEXT)
   {
    if(type == model->elpTransType)
     break ;
   }
 if((model == NULL) && (ELP_LOAD_FILE_TYPE || ELP_GENPARAM ))
   {
    elpGenParam(NULL,type,elpBADLW,elpBADLW,ELPMINVOLTAGE,NULL,elpTYPICAL,NULL) ;
    model = ELP_MODEL_LIST ;
   }

 return(model) ;
}

/*****************************************************************************/
/*                        function elpGetModelIndex()                        */
/*****************************************************************************/
elpFCT elpmodel_list *elpGetModelIndex(index)
int index ;
{
 elpmodel_list *model ;

 for(model = ELP_MODEL_LIST ; model != NULL ; model = model->NEXT)
   {
    if(index == model->elpModelIndex)
     break ;
   }

 return(model) ;
}

/*****************************************************************************/
/*                        function elpenv()                                  */
/* recuperation du nom du fichier elp a partir de la variable:               */
/* ELP_TECHNO_NAME.                                                          */
/*                                                                           */
/* Parametres en entree:                                                     */
/* --------------------                                                      */
/*    Aucun!                                                                 */
/*                                                                           */
/* Parametre en sortie:                                                      */
/* -------------------                                                       */
/*    Aucun!                                                                 */
/*****************************************************************************/
elpFCT int elpenv()
{
 char *str ;
 float val;
 static int doneonce=0;

 if (!doneonce)
 {
   elpTechnoName[0] = '\0';
   elpGenTechnoFile[0] = '\0';
   if (elpoldtechnofilename) mbkfree(elpoldtechnofilename);
   elpoldtechnofilename = NULL;
   elpres_load = 1;
   doneonce=1;
 }
 
 str = V_STR_TAB[__ELP_GEN_TECHNO_NAME].VALUE ;
 if(str != NULL)
   strcpy(elpGenTechnoFile,str) ;
 else 
   strcpy(elpGenTechnoFile,"techno.elp") ;

 str = V_STR_TAB[__ELP_LOAD_FILE].VALUE ;

 /*
 if ( (str = V_INT_TAB[__ELP_CAPA_LEVEL].VALUE) ) {
   level = atoi ( str );
   switch ( level ) {
     case 0 : ELP_CAPA_LEVEL = ELP_CAPA_LEVEL0;
              break;
     case 1 : ELP_CAPA_LEVEL = ELP_CAPA_LEVEL1;
              break;
     case 2 : ELP_CAPA_LEVEL = ELP_CAPA_LEVEL2;
              break;
     default: ELP_CAPA_LEVEL = ELP_CAPA_LEVEL1;
              break;
   }
 }
 else
   ELP_CAPA_LEVEL = ELP_CAPA_LEVEL1;
   */

 /*
 str = V_BOOL_TAB[__ELP_DRV_FILE].VALUE ;
 if((str != NULL) && (strcmp(str,"yes") == 0))
   {
    ELP_DRV_FILE = 1 ;
   }
 else
   {
    ELP_DRV_FILE = 0 ;
   }
   */
 str = V_STR_TAB[__ELP_TECHNO_NAME].VALUE ;
 if(str != NULL) 
  {
   strcpy(elpTechnoFile,str) ;
   str = V_STR_TAB[__ELP_LOAD_FILE].VALUE ;
   if((str != NULL) && (strcmp(str,"yes") != 0))
    {
     if( !strcmp(str,"no") )
      {
       strcpy(elpTechnoFile,elpGenTechnoFile) ;
       ELP_LOAD_FILE_TYPE = ELP_DONTLOAD_FILE ;
      }
     else if( !strcmp(str,"default") )
      {
       strcpy(elpTechnoFile,elpGenTechnoFile) ;
       ELP_LOAD_FILE_TYPE = ELP_DEFAULTLOAD_FILE ;
      }
     else
      {
       ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE ;
      }
    }
   else
    {
     ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE ;
    }
  }
 else 
  {
   str = getenv("AVT_TOOLS_DIR") ;
   if(str != NULL)
    {
     sprintf(elpTechnoFile,"%s%s",str,TECHNOLOGY) ;
    }
   else if((str = getenv("AVERTEC_TOP")) != NULL)
    {
     sprintf(elpTechnoFile,"%s%s",str,TECHNOLOGY) ;
    }
   else
    {
     sprintf(elpTechnoFile,"AvtTools%s",TECHNOLOGY) ;
    }
    str = V_STR_TAB[__ELP_LOAD_FILE].VALUE ;
    if((str != NULL) && (strcmp(str,"yes") != 0))
     {
      if( !strcmp(str,"no") )
       {
        strcpy(elpTechnoFile,elpGenTechnoFile) ;
        ELP_LOAD_FILE_TYPE = ELP_DONTLOAD_FILE ;
       }
      else if( !strcmp(str,"default") )
       {
        strcpy(elpTechnoFile,elpGenTechnoFile) ;
        ELP_LOAD_FILE_TYPE = ELP_DEFAULTLOAD_FILE ;
       }
      else
       {
        ELP_LOAD_FILE_TYPE = ELP_LOADELP_FILE ;
       }
     }
    else
     {
      strcpy(elpTechnoFile,elpGenTechnoFile) ;
      ELP_LOAD_FILE_TYPE = ELP_DONTLOAD_FILE ;
     }
 }
       ELP_GENPARAM = V_BOOL_TAB[__ELP_GEN_PARAM].VALUE ;
    ELP_CAPA_DIFF = V_BOOL_TAB[__ELP_DEDUCE_DIFFSIZE].VALUE ;
 /*
 str = V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE ;
 if ( str )
   {
    val = atof (str);
    if ( val >= 0.0 && val <= 1.0)
      ELP_MULU0_MARGIN = val;
   }
 STR = V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE ;
 if ( str )
   {
    val = atof (str);
    if ( val >= 0.0 && val <= 1.0)
      ELP_DELVT0_MARGIN = val;
   }
 str = V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE ;
 if ( str )
   {
    val = atof (str);
    if ( val >= 0.0 && val <= 1.0)
      ELP_SA_MARGIN = val;
   }
 str = V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE ;
 if ( str )
   {
    val = atof (str);
    if ( val >= 0.0 && val <= 1.0)
      ELP_SB_MARGIN = val;
   }
 str = V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE ;
 if ( str )
   {
    val = atof (str);
    if ( val >= 0.0 && val <= 1.0)
      ELP_SD_MARGIN = val;
   }
   */
 V_FLOAT_TAB[__ELP_SA_MARGIN].VALUE = -1.0;
 V_FLOAT_TAB[__ELP_SB_MARGIN].VALUE = -1.0;
 V_FLOAT_TAB[__ELP_SD_MARGIN].VALUE = -1.0;
 V_FLOAT_TAB[__ELP_MULU0_MARGIN].VALUE = -1.0;
 V_FLOAT_TAB[__ELP_DELVT0_MARGIN].VALUE = -1.0;

 str = getenv("ELP_NF_MARGIN") ;
 if ( str )
   {
    val = atof (str);
    if ( val >= 0.0 && val <= 1.0)
      ELP_NF_MARGIN = val;
   }

 return 1 ;
}

/*****************************************************************************/
/*                        function VerifModel()                              */
/*                                                                           */
/* Parametres en entree:                                                     */
/* --------------------                                                      */
/*    model de transistor                                                    */
/*                                                                           */
/* Parametre en sortie:                                                      */
/* -------------------                                                       */
/*    verifie le model                                                       */
/*****************************************************************************/
elpFCT int elpVerifModel(model)
elpmodel_list *model ;
{
   int i ;
  
   for( i=0 ; i<elpSHRINKNUM ; i++ ) 
     if( !isfinite(model->elpShrink[i]) ) //elpError(1004,elpTechnoFile) ;
        avt_errmsg(ELP_ERRMSG, "004", AVT_ERROR, elpTechnoFile);
   for( i=0 ; i<elpMODELNUM ; i++ )
     if( !isfinite(model->elpModel[i]) ) //elpError(1004,elpTechnoFile) ;
        avt_errmsg(ELP_ERRMSG, "004", AVT_ERROR, elpTechnoFile);
   for( i=0 ; i<elpVOLTNUM ; i++ )
     if( !isfinite(model->elpVoltage[i]) ) //elpError(1004,elpTechnoFile) ;
        avt_errmsg(ELP_ERRMSG, "004", AVT_ERROR, elpTechnoFile);
   for( i=0 ; i<elpCAPANUM ; i++ )
     if( !isfinite(model->elpCapa[i]) ) //elpError(1004,elpTechnoFile) ;
        avt_errmsg(ELP_ERRMSG, "004", AVT_ERROR, elpTechnoFile);
   for( i=0 ; i<elpRSSNUM ; i++ )
     if( !isfinite(model->elpRss[i]) ) //elpError(1004,elpTechnoFile) ;
        avt_errmsg(ELP_ERRMSG, "004", AVT_ERROR, elpTechnoFile);
   for( i=0 ; i<elpRACCNUM ; i++ )
     if( !isfinite(model->elpRacc[i]) ) //elpError(1004,elpTechnoFile) ;
        avt_errmsg(ELP_ERRMSG, "004", AVT_ERROR, elpTechnoFile);
   
   if( 
   ((model->elpTransModel == elpMOS) && (
   (model->elpModel[elpVT]  == 0.0 ) ||
   (model->elpModel[elpVT0] == 0.0 ) ||
   (model->elpModel[elpA]   == 0.0 ) ||
   (model->elpModel[elpB]   == 0.0 ) ||
   (model->elpModel[elpRT]  == 0.0 ) ||
   ((model->elpVoltage[elpVDDMAX] < 0.0 )&&(elpGeneral[elpGVDDMAX] < 0.0)) ||
   (model->elpCapa[elpCGS]  == 0.0 ) ||
   (model->elpCapa[elpCGSU]  == 0.0 ) ||
   (model->elpCapa[elpCGSUMIN]  == 0.0 ) ||
   (model->elpCapa[elpCGSUMAX]  == 0.0 ) ||
   (model->elpCapa[elpCGSD]  == 0.0 ) ||
   (model->elpCapa[elpCGSDMIN]  == 0.0 ) ||
   (model->elpCapa[elpCGSDMAX]  == 0.0 ) ||
   (model->elpCapa[elpCGP]  == 0.0 ) ||
   (model->elpCapa[elpCGPUMIN]  == 0.0 ) ||
   (model->elpCapa[elpCGPUMAX]  == 0.0 ) ||
   (model->elpCapa[elpCGPDMIN]  == 0.0 ) ||
   (model->elpCapa[elpCGPDMAX]  == 0.0 ) ||
   (elpEsimName[0]              == '\0') ||
   (model->elpTransTechno       == 0.0 ) ||
   ((model->elpTemp         < ELPMINTEMP ) && (elpGeneral[elpTEMP] < ELPMINTEMP))))
   )
   if (model->elpTransModel == elpMOS) {
        if (model->elpModel[elpVT]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "VT", model->elpModel[elpVT]);
            return 1004 ; 
        }
        if (model->elpModel[elpVT0]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "VT0", model->elpModel[elpVT0]);
            return 1004 ; 
        }
        if (model->elpModel[elpA]   == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "A", model->elpModel[elpA]);
            return 1004 ; 
        }
        if (model->elpModel[elpB]   == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "B", model->elpModel[elpB]);
            return 1004 ; 
        }
        if (model->elpModel[elpRT]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "RT", model->elpModel[elpRT]);
            return 1004 ; 
        }
        if ((model->elpVoltage[elpVDDMAX] < 0.0 ) && (elpGeneral[elpGVDDMAX] < 0.0)) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "VDDMAX", model->elpModel[elpVDDMAX]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGS]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGS", model->elpModel[elpCGS]);
            return 1004 ; 
        }
        if ((model->elpCapa[elpCGSUMIN]  == 0.0 ) &&
            (model->elpCapa[elpCGSUMAX]  == 0.0 ) &&
            (model->elpCapa[elpCGSDMIN]  == 0.0 ) &&
            (model->elpCapa[elpCGSDMAX]  == 0.0 )) {
            model->elpCapa[elpCGSUMIN] = model->elpCapa[elpCGSU];
            model->elpCapa[elpCGSUMAX] = model->elpCapa[elpCGSU];
            model->elpCapa[elpCGSDMIN] = model->elpCapa[elpCGSD];
            model->elpCapa[elpCGSDMAX] = model->elpCapa[elpCGSD];
        }            
        if (model->elpCapa[elpCGSU]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGSU", model->elpModel[elpCGSU]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGSUMIN]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGSUMIN", model->elpModel[elpCGSUMIN]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGSUMAX]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGSUMAX", model->elpModel[elpCGSUMAX]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGSDMIN]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGSDMIN", model->elpModel[elpCGSDMIN]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGSDMAX]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGSDMAX", model->elpModel[elpCGSDMAX]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGSD]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGSD", model->elpModel[elpCGSD]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGP]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGP", model->elpModel[elpCGP]);
            return 1004 ; 
        }
        if ((model->elpCapa[elpCGPUMIN]  == 0.0 ) &&
            (model->elpCapa[elpCGPUMAX]  == 0.0 ) &&
            (model->elpCapa[elpCGPDMIN]  == 0.0 ) &&
            (model->elpCapa[elpCGPDMAX]  == 0.0 )) {
            model->elpCapa[elpCGPUMIN] = model->elpCapa[elpCGP];
            model->elpCapa[elpCGPUMAX] = model->elpCapa[elpCGP];
            model->elpCapa[elpCGPDMIN] = model->elpCapa[elpCGP];
            model->elpCapa[elpCGPDMAX] = model->elpCapa[elpCGP];
        }
        if (model->elpCapa[elpCGPUMIN]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGPUMIN", model->elpModel[elpCGPUMIN]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGPUMAX]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGPUMAX", model->elpModel[elpCGPUMAX]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGPDMIN]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGPDMIN", model->elpModel[elpCGPDMIN]);
            return 1004 ; 
        }
        if (model->elpCapa[elpCGPDMAX]  == 0.0 ) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "CGPDMAX", model->elpModel[elpCGPDMAX]);
            return 1004 ; 
        }
        if (elpEsimName[0]          == '\0') {
            return 1004 ; 
        }
        if (model->elpTransTechno    == 0.0 ) {
            fprintf (stderr, "model %s: Techno = 0\n", model->elpModelName);
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "Techno", model->elpTransTechno);
            return 1004 ; 
        }
        if ((model->elpTemp         < ELPMINTEMP ) && (elpGeneral[elpTEMP] < ELPMINTEMP)) {
            avt_errmsg(ELP_ERRMSG, "010", AVT_ERROR, elpTechnoFile, model->elpModelName, "Temperature", model->elpTemp);
            return 1004 ; 
        }
   }

    if(model->elpTemp < ELPMINTEMP)
      model->elpTemp = elpGeneral[elpTEMP] ;

    if(elpGeneral[elpTEMP] < ELPMINTEMP)
      elpGeneral[elpTEMP] = model->elpTemp ;

    if(elpGeneral[elpGVDDMAX] < 0.0)
      elpGeneral[elpGVDDMAX] =  model->elpVoltage[elpVDDMAX] ;

    if(elpGeneral[elpGVDDBEST] < 0.0)
      elpGeneral[elpGVDDBEST] =  model->elpVoltage[elpVDDMAX] ;

    if(elpGeneral[elpGVDDWORST] < 0.0)
      elpGeneral[elpGVDDWORST] =  model->elpVoltage[elpVDDMAX] ;

    if(model->elpVoltage[elpVDDMAX] < 0.0)
      model->elpVoltage[elpVDDMAX] = elpGeneral[elpGVDDMAX] ;

    if (model->elpVoltage[elpVBULK] < ELPMINVBULK ) {
      if ( model->elpTransType == elpPMOS )
        model->elpVoltage[elpVBULK] = model->elpVoltage[elpVDDMAX];
      else
        model->elpVoltage[elpVBULK] = 0;
    }

    if(elpGeneral[elpGDTHR] < 0.0) 
      elpGeneral[elpGDTHR] = 0.5 ;

    if((model->elpTransType == elpPMOS) && (model->elpVoltage[elpVDEG] < 0.0))
      model->elpVoltage[elpVDEG] = 1.5*model->elpModel[elpVT] ;
    if((model->elpTransType == elpPMOS) && (model->elpVoltage[elpVTI] < 0.0))
      model->elpVoltage[elpVTI] = 1.5*model->elpModel[elpVT] ;

    if((model->elpTransType == elpNMOS) && (model->elpVoltage[elpVDEG] < 0.0))
      model->elpVoltage[elpVDEG] = model->elpVoltage[elpVDDMAX] - 1.5*model->elpModel[elpVT] ;
    if((model->elpTransType == elpNMOS) && (model->elpVoltage[elpVTI] < 0.0))
      model->elpVoltage[elpVTI] = model->elpVoltage[elpVDDMAX] - 1.5*model->elpModel[elpVT] ;

 return(0) ;
}

/*****************************************************************************/
/*                        function LoadElp()                                 */
/* chargement du fichier techno sous forme structure ELP. En cas d'erreur la */
/* fonction affiche un message et renvoie le numero d'erreur. Sinon le code  */
/* de retour est 0.                                                          */
/*                                                                           */
/* Parametres en entree:                                                     */
/* --------------------                                                      */
/*    Aucun!                                                                 */
/*                                                                           */
/* Parametre en sortie:                                                      */
/* -------------------                                                       */
/*    La fonction renvoie 0 si le chargement s'est bien effectue, le numero  */
/*    d'erreur sinon.                                                        */
/*****************************************************************************/
elpFCT short elpLoadElp()
{
 elpmodel_list *model ;
 int res ;
/* ouverture du fichier techno */

elpFreeModel() ;

if ( ELP_LOAD_FILE_TYPE == ELP_DONTLOAD_FILE )
  elpin = NULL;
else
  elpin = fopen(elpTechnoFile,"r") ;

ELPLINE = 1 ;

/* appel au parser pour chargement */
if ( elpin )
if(elpparse() != 0)
    {
      avt_errmsg(ELP_ERRMSG, "002", AVT_ERROR, elpTechnoFile);
//    elpError(1002,elpTechnoFile) ; /* si erreur chergement => erreur 1002 */
    return 1002 ; /* arret de la procedure elp */
    }

/* fermeture du fichier techno */
if ( elpin )
if( fclose(elpin) != 0)
    {
      avt_errmsg(ELP_ERRMSG, "003", AVT_ERROR, elpTechnoFile);
     //elpError(1003,elpTechnoFile) ; /* si fermeture ko => erreur 1003 */
    }

/*-------------------------------------------------------------------*/
/* Si le fichier elp n existe pas, ni le fichier techno (spice)      */
/* alors retourne 1                                                  */
/*-------------------------------------------------------------------*/
if ( !ELP_MODEL_LIST ) {
  if (elpSetDefaultParam!=NULL) elpSetDefaultParam ();
  return 1;
}

/*-------------------------------------------------------------------*/
/* verification des parametres obligatoires et calcul des parametres */
/* optionnels.                                                       */
/*-------------------------------------------------------------------*/
 model = ELP_MODEL_LIST ;
 while(model){
   res = elpVerifModel(model) ;
   if(res != 0)
     return(res) ;
   model = model -> NEXT ;
 }

return 0 ; /* bon chargement */
}

/*****************************************************************************/
/*                        function DriveElp()                                */
/* sauvegarde le fichier elp                                                 */
/*                                                                           */
/* Parametres en entree:                                                     */
/* --------------------                                                      */
/*    Nom du fichier elp                                                     */
/*                                                                           */
/* Parametre en sortie:                                                      */
/* -------------------                                                       */
/*    La fonction renvoie 0 si le chargement s'est bien effectue, le numero  */
/*    d'erreur sinon.                                                        */
/*****************************************************************************/
elpFCT void elpDriveElp(char *filename)
{
 FILE *file ;
 elpmodel_list *model ;
      
 sprintf(filename, "%s%s",filename,".elp") ; 
 file = fopen(filename,"w") ;
 if(file == NULL) 
  {
   fprintf(stderr, "elp error: can't open file %s to drive!!!\n",filename) ;
   return ;
  }
 
 model = ELP_MODEL_LIST ;

 while(model){
   elpVerifModel(model) ;
   model = model -> NEXT ;
 }

 model = ELP_MODEL_LIST ;

 fprintf(file, "#TAS PARAMETER FILE\n\n") ;
 fprintf(file, "Technologie: %s   Version: %g\n\n", elpTechnoName, elpTechnoVersion) ;
 fprintf(file, "#General parameters\n\n") ;
 fprintf(file, "#Reference Simulator\n")  ;
 fprintf(file, "ESIM  = %s\n", elpEsimName) ;
 fprintf(file, "#simulation parameters\n") ;
 if(elpGeneral[elpTEMP] > ELPMINTEMP)
   fprintf(file, "TEMP   = %g\n", elpGeneral[elpTEMP]) ;
 if(elpGeneral[elpGVDDMAX] > 0)
   fprintf(file, "VDDmax = %g\n", elpGeneral[elpGVDDMAX]) ;
 if(elpGeneral[elpGVDDBEST] > 0)
   fprintf(file, "VDDBest = %g\n", elpGeneral[elpGVDDBEST]) ;
 if(elpGeneral[elpGVDDWORST] > 0)
   fprintf(file, "VDDWorst = %g\n", elpGeneral[elpGVDDWORST]) ;
 if(elpGeneral[elpGDTHR] > 0)
   fprintf(file, "DTHR   = %g\n", elpGeneral[elpGDTHR]) ;
 if(elpGeneral[elpGSHTHR] > 0)
   fprintf(file, "SHTHR  = %g\n", elpGeneral[elpGSHTHR]) ;
 if(elpGeneral[elpGSLTHR] > 0)
   fprintf(file, "SLTHR  = %g\n", elpGeneral[elpGSLTHR]) ;
 fprintf(file, "SLOPE  = %g\n", elpGeneral[elpSLOPE]) ;
 fprintf(file, "ACM    = %g\n\n\n", elpGeneral[elpACM]) ;
 
  while (model) {
    elpDriveOneModel (file,model);
    model = model->NEXT ;
  }
  fclose(file) ;
}

/****************************************************************************/
/*                           fonction LotrsCapaDrain()                      */
/* calcule la capacite de drain d'un transistor MBK passe en parametre.     */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: transistor logique MBK dont on veut calculer la capacite de */
/*              drain.                                                      */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    la fonction renvoie la capcite du drain du transistor en picoF.       */
/****************************************************************************/
elpFCT float elpLotrsCapaDrain(lotrs,capatype,transcase)
struct lotrs *lotrs ;
int capatype;
int transcase;
{
double w,wcj,vdd ;
double cds,cdp,cdw;
double cdsval,cdpval,cdwval;
long xd_s,pd_s;
elpmodel_list *model;
double ad, pd, wj, ab, lg, ls, cgp ;
static char displayed=0 ;

vdd = elpGetVddFromCorner ( lotrs, transcase );
model = elpGetModel(lotrs,vdd,transcase) ;
if ( !model ) return 0.0;

/* si les parametres CDxx ou sont manquants => erreur 1005 */
if(model->elpTransModel == elpMOS)
  {
   if((model->elpCapa[elpCDS] == 0.0) &&
      (model->elpCapa[elpCDP] == 0.0) &&
      (model->elpCapa[elpCDW] == 0.0))
       {
         if( !displayed ) 
          avt_errmsg(ELP_ERRMSG, "005", AVT_ERROR, elpTechnoFile);
          // elpError(1005,"diffusion") ;
         displayed = 1 ;
         return 0 ;
       }
    }

if((lotrs->PD == 0) && (lotrs->XD == 0) && (lotrs->WIDTH == 0))
    return(0.0) ;

if(ELP_CAPA_DIFF == 1)
  {
   if((lotrs->XD < (long)0) || (lotrs->PD < (long)0))
    {
     lotrs->XD = model->elpShrink[elpWMLT]*SCALE_X ;
     lotrs->PD = elpGetShrinkedWidth(lotrs,model);
    }
  }

if(lotrs->WIDTH == 0)
   wcj = w = ((double)lotrs->PD / (double)4.0) ;
else {
   w = (double)elpGetShrinkedWidth(lotrs,model);
   wcj = elpShrinkSize(lotrs->WIDTH,model->elpShrink[elpDWCJ],1.0) ;
}

switch ( capatype ) {
  case ELP_CAPA_UP:
  case ELP_CAPA_UP_MIN:
  case ELP_CAPA_UP_MAX: cds = model->elpCapa[elpCDSU];
                        cdp = model->elpCapa[elpCDPU];
                        cdw = model->elpCapa[elpCDWU];
                        cgp = model->elpCapa[elpCGP];
                        break;
  case ELP_CAPA_DN:
  case ELP_CAPA_DN_MIN:
  case ELP_CAPA_DN_MAX: cds = model->elpCapa[elpCDSD];
                        cdp = model->elpCapa[elpCDPD];
                        cdw = model->elpCapa[elpCDWD];
                        cgp = model->elpCapa[elpCGP];
                        break;
  default             : cds = model->elpCapa[elpCDS];
                        cdp = model->elpCapa[elpCDP];
                        cdw = model->elpCapa[elpCDW];
                        cgp = model->elpCapa[elpCGP];
                        break;
}
/*---------------------------------------------------------------------------*/
/* Methode de calcul des capacites: si le perimetre est manquant, la contri- */
/* bution en perimetre sera remplacee par la contribution en largeur. Sinon, */
/* seul le perimetre sera pris en compte.                                    */
/*---------------------------------------------------------------------------*/
if(model->elpTransModel == elpMOS)
{
  elpLotrsGetShrinkDim(lotrs,NULL,NULL,NULL,&xd_s,NULL,&pd_s,NULL,NULL, transcase);

  if( model->elpTransTechno == elpPSP ) {
  
    ad = (double)xd_s*w / (double)(SCALE_X*SCALE_X) ;
    pd = pd_s / (double)SCALE_X ;
    wj = wcj / (double)SCALE_X ;
    
    switch( model->elpSWJUNCAP ) {
    case 0 :
      ab = 0.0 ;
      ls = 0.0 ;
      lg = 0.0 ;
      break ;
    case 1 :
      ab = getlotrsparam( lotrs, MBK_ABDRAIN, NULL, NULL );
      ls = getlotrsparam( lotrs, MBK_LSDRAIN, NULL, NULL );
      lg = getlotrsparam( lotrs, MBK_LGDRAIN, NULL, NULL );
      break ;
    case 2 :
      ab = ad ;
      ls = pd ;
      lg = wj ;
      break ;
    case 3 :
      ab = ad ;
      ls = pd-wj ;
      lg = wj ;
      break ;
    }
    cdsval = ab*cds ; 
    cdpval = ls*cdp ;
    cdwval = lg*cdw + cgp*w ; 
  }
  else {
    if ( model->elpTransTechno == elpBSIM3V3 &&  pd_s < wcj ) {
      cdsval = ((double)xd_s*w / (double)(SCALE_X*SCALE_X)) * cds ;
      cdpval = (pd_s / (double)SCALE_X ) * ( cdw + cdp - model->elpCapa[elpCGP] ) ;
      cdwval = (wcj / (double)SCALE_X) * model->elpCapa[elpCGP] ;
    }
    else {
      cdsval = ((double)xd_s*w / (double)(SCALE_X*SCALE_X)) * cds ;
      cdpval = ((double)pd_s / (double)SCALE_X ) * cdp ;
      cdwval = (wcj / (double)SCALE_X ) * cdw ;
    }
  }

  return (float)(cdsval+cdpval+cdwval);
}
else
{
  return(0.0) ;
}

}


/****************************************************************************/
/*                           fonction LotrsCapaSource()                     */
/* calcule la capacite de source d'un transistor MBK passe en parametre.    */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: transistor logique MBK dont on veut calculer la capacite de */
/*              source.                                                     */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    la fonction renvoie la capcite de la source du transistor en picoF.   */
/****************************************************************************/
elpFCT float elpLotrsCapaSource(lotrs,capatype,transcase)
struct lotrs *lotrs ;
int capatype;
int transcase;
{
double w,wcj,vdd;
double css,csp,csw;
long xs_s,ps_s;
double cssval,cspval,cswval;
elpmodel_list *model;
double as, ps, wj, ab, lg, ls, cgp ;
static char displayed=0;

vdd = elpGetVddFromCorner ( lotrs, transcase );
model = elpGetModel(lotrs,vdd,transcase) ;
if ( !model ) return 0.0;

/* si les parametres CDxx ou CSxx sont manquants => erreur 1005 */
if(model->elpTransModel == elpMOS)
  {
   if((model->elpCapa[elpCSS] == 0.0) &&
      (model->elpCapa[elpCSP] == 0.0) &&
	  (model->elpCapa[elpCSW] == 0.0))
       {
         if( !displayed ) 
           avt_errmsg(ELP_ERRMSG, "002", AVT_ERROR, elpTechnoFile);
   //        elpError(1005,"diffusion") ;
         displayed = 1 ;
         return 0 ;
       }
    }

if((lotrs->PS == 0) && (lotrs->XS == 0) && (lotrs->WIDTH == 0))
    return(0.0) ;

if(ELP_CAPA_DIFF == 1)
  {
   if((lotrs->XS<0) || (lotrs->PS<0))
    {
     lotrs->XS = model->elpShrink[elpWMLT]*SCALE_X ;
     lotrs->PS = elpGetShrinkedWidth(lotrs,model);
    }
  }

if(lotrs->WIDTH == 0)
   wcj = w = ((double)lotrs->PS / (double)4.0) ;
else {
   w = (double)elpGetShrinkedWidth(lotrs,model);
   wcj = elpShrinkSize(lotrs->WIDTH,model->elpShrink[elpDWCJ],1.0) ;
}

switch ( capatype ) {
  case ELP_CAPA_UP:
  case ELP_CAPA_UP_MIN:
  case ELP_CAPA_UP_MAX: css = model->elpCapa[elpCSSU];
                        csp = model->elpCapa[elpCSPU];
                        csw = model->elpCapa[elpCSWU];
                        cgp = model->elpCapa[elpCGP];
                        break;
  case ELP_CAPA_DN:
  case ELP_CAPA_DN_MIN:
  case ELP_CAPA_DN_MAX: css = model->elpCapa[elpCSSD];
                        csp = model->elpCapa[elpCSPD];
                        csw = model->elpCapa[elpCSWD];
                        cgp = model->elpCapa[elpCGP];
                        break;
  default             : css = model->elpCapa[elpCSS];
                        csp = model->elpCapa[elpCSP];
                        csw = model->elpCapa[elpCSW];
                        cgp = model->elpCapa[elpCGP];
                        break;
}
/*---------------------------------------------------------------------------*/
/* Methode de calcul des capacites: si le perimetre est manquant, la contri- */
/* bution en perimetre sera remplacee par la contribution en largeur. Sinon, */
/* seul le perimetre sera pris en compte.                                    */
/*---------------------------------------------------------------------------*/
if(model->elpTransModel == elpMOS)
{
  elpLotrsGetShrinkDim(lotrs,NULL,NULL,&xs_s,NULL,&ps_s,NULL,NULL,NULL, transcase);
    
  if( model->elpTransTechno == elpPSP ) {

    as = (double)xs_s*w / (double)(SCALE_X*SCALE_X) ;
    ps = ps_s / (double)SCALE_X ;
    wj = wcj / (double)SCALE_X ;
    
    switch( model->elpSWJUNCAP ) {
    case 0 :
      ab = 0.0 ;
      ls = 0.0 ;
      lg = 0.0 ;
      break ;
    case 1 :
      ab = getlotrsparam( lotrs, MBK_ABSOURCE, NULL, NULL );
      ls = getlotrsparam( lotrs, MBK_LSSOURCE, NULL, NULL );
      lg = getlotrsparam( lotrs, MBK_LGSOURCE, NULL, NULL );
      break ;
    case 2 :
      ab = as ;
      ls = ps ;
      lg = wj ;
      break ;
    case 3 :
      ab = as ;
      ls = ps-wj ;
      lg = wj ;
      break ;
    }
    cssval = ab*css ; 
    cspval = ls*csp ;
    cswval = lg*csw + cgp*w ;
  }
  else {
  
    if ( model->elpTransTechno == elpBSIM3V3  && ps_s < wcj ) {
      cssval = ((double)xs_s*(double)w / (double)(SCALE_X*SCALE_X)) * css ;
      cspval = (ps_s / (double)SCALE_X ) * ( csw + csp - model->elpCapa[elpCGP] ) ;
      cswval = (wcj / (double)SCALE_X) * model->elpCapa[elpCGP] ;
    }
    else {
      cssval = ((double)xs_s*(double)w / (double)(SCALE_X*SCALE_X)) * css ;
      cspval = ((double)ps_s / (double)SCALE_X ) * csp ;
      cswval = (wcj / (double)SCALE_X ) * csw ;
    }
  }
  return cssval + cspval + cswval ;
}
else
{
  return(0.0) ;
}

}

/****************************************************************************/
/*                           fonction LotrsCapaGrid()                       */
/* calcule la capacite de grille d'un transistor MBK passe en parametre.    */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: transistor logique MBK dont on veut calculer la capacite de */
/*              grille.                                                     */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    la fonction renvoie la capcite de la grille du transistor en picoF.   */
/****************************************************************************/
elpFCT float elpLotrsCapaGrid(lotrs,transcase)
struct lotrs *lotrs ;
int transcase;

{
 long l,w ;
 elpmodel_list *model;
 double vdd,cgs;
 int same_SD;

 vdd = elpGetVddFromCorner ( lotrs, transcase );
 model = elpGetModel(lotrs,vdd,transcase) ;
 if ( !model ) return 0.0;

 same_SD = elpSameSD_sig (lotrs);

l = elpShrinkSize(elpGetShrinkedLength(lotrs,model),-model->elpShrink[elpDL],1.0) ;
l = elpShrinkSize(l,model->elpShrink[elpDLC],1.0) ;
w = elpShrinkSize(elpGetShrinkedWidth(lotrs,model),-model->elpShrink[elpDW],1.0) ;
w = elpShrinkSize(w,model->elpShrink[elpDWC],1.0) ;

cgs = (same_SD == 0) ? model->elpCapa[elpCGS] : model->elpCapa[elpCGS0];

/* les parametres CGxx sont obligatoires et ne sont donc pas manquant */

if(model->elpTransModel == elpMOS)
{
return(float)(
/* contribution en surface   */
((double)w*(double)l / (double)(SCALE_X*SCALE_X))
*cgs +
/* contribution en perimetre */
(2.0*(double)w / (double)SCALE_X)*model->elpCapa[elpCGP]) ;
}
else
{
return(0.0) ;
}

}

/****************************************************************************/
/*                           fonction LotrsInCapa ()                        */
/* calcule la capacite d'entree  d'un transistor MBK passe en parametre.    */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: transistor logique MBK dont on veut calculer la capacite de */
/*              grille.                                                     */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    la fonction renvoie la capcite de la grille du transistor en picoF.   */
/****************************************************************************/
elpFCT float elpLotrsInCapa( lofig_list *lofig,
                             lotrs_list *lotrs, 
                             int capatype, 
                             int transcase,
                             int correct
                           )
{
  long l,w ;
  double cgs=0.0, cgp=0.0,vdd;
  double cc, cm, c ;
  elpmodel_list *model;
  int same_SD=0;
 
  vdd = elpGetVddFromCorner ( lotrs, transcase );
  model = elpGetModel(lotrs,vdd,transcase) ;
  if ( !model || model->elpTransModel != elpMOS ) return 0.0;

  l = elpShrinkSize(elpGetShrinkedLength(lotrs,model),-model->elpShrink[elpDL],1.0) ;
  l = elpShrinkSize(l,model->elpShrink[elpDLC],1.0) ;
  w = elpShrinkSize(elpGetShrinkedWidth(lotrs,model),-model->elpShrink[elpDW],1.0) ;
  w = elpShrinkSize(w,model->elpShrink[elpDWC],1.0) ;

  same_SD = elpSameSD_sig ( lotrs );

  switch ( capatype ) {
    case ELP_CAPA_UP     : if (!same_SD) 
                             cgs = model->elpCapa[elpCGSU];
                           else
                             cgs = model->elpCapa[elpCGSU0];
                           cgp = model->elpCapa[elpCGPUMIN]; 
                           break;
    case ELP_CAPA_UPF    : if (!same_SD) 
                             cgs = model->elpCapa[elpCGSUF];
                           else
                             cgs = model->elpCapa[elpCGSU0];
                           cgp = model->elpCapa[elpCGPUMIN]; 
                           break;
    case ELP_CAPA_UP_MIN : cgs = model->elpCapa[elpCGSUMIN];
                           cgp = model->elpCapa[elpCGPUMIN];
                           break;
    case ELP_CAPA_UP_MAX : cgs = model->elpCapa[elpCGSUMAX];
                           cgp = model->elpCapa[elpCGPUMAX];
                           break;
    case ELP_CAPA_DN     : if (!same_SD) 
                             cgs = model->elpCapa[elpCGSD];
                           else
                             cgs = model->elpCapa[elpCGSD0];
                           cgp = model->elpCapa[elpCGPDMIN];
                           break;
    case ELP_CAPA_DNF    : if (!same_SD) 
                             cgs = model->elpCapa[elpCGSDF];
                           else
                             cgs = model->elpCapa[elpCGSD0];
                           cgp = model->elpCapa[elpCGPDMIN];
                           break;
    case ELP_CAPA_DN_MIN : cgs = model->elpCapa[elpCGSDMIN];
                           cgp = model->elpCapa[elpCGPDMIN];
                           break;
    case ELP_CAPA_DN_MAX : cgs = model->elpCapa[elpCGSDMAX];
                           cgp = model->elpCapa[elpCGPDMAX];
                           break;
    case ELP_CAPA_TYPICAL: if (!same_SD)
                             cgs = model->elpCapa[elpCGS];
                           else
                             cgs = model->elpCapa[elpCGS0];
                           cgp = model->elpCapa[elpCGP];
                           break;
  }

  c = ((double)w*(double)l / (double)(SCALE_X*SCALE_X)) * cgs ;
    
  cc = ((double)w) / ((double)SCALE_X) * cgp ;
  
  /* Source side */
  if( correct 
      && !mbk_LosigIsVSS(lotrs->SOURCE->SIG) 
      && !mbk_LosigIsVDD(lotrs->SOURCE->SIG) )
    cm = elpGetCapaSig( lofig, lotrs->SOURCE->SIG, ELP_CAPA_TYPICAL ) - cc ;
  else
    cm = -1.0 ;

  if( cm > 0.0 )
    c = c + cc*cm/(cc+cm);
  else
    c = c + cc ;

  /* Drain Side */
  if( correct 
      && !mbk_LosigIsVSS(lotrs->DRAIN->SIG)
      && !mbk_LosigIsVSS(lotrs->DRAIN->SIG) )
    cm = elpGetCapaSig( lofig, lotrs->DRAIN->SIG, ELP_CAPA_TYPICAL ) - cc ;
  else
    cm = -1.0 ;

  if( cm > 0.0 )
    c = c + cc*cm/(cc+cm);
  else
    c = c + cc ;
  
  return (float)c ;
}

/****************************************************************************/
/*                    fonction LofigCapaDiff()                              */
/* calcule les capas de diffusion des transistors de la figure lofig        */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lofig: figure logique MBK dont dans laquelle il faut ajouter les   */
/*              capas de diffusion.                                         */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    Aucun!                                                                */
/****************************************************************************/
elpFCT void elpLofigCapaDiff(lofig,transcase)
struct lofig*lofig ;
int transcase;

{
lotrs_list *lotrs ;
float       capadrain;
float       capasource;

for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
 {
/*  if (mbk_isdioden(getlotrsmodel(lotrs)) || mbk_isdiodep(getlotrsmodel(lotrs)))
    continue;*/
  if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL0 )
   {
    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_TYPICAL ) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_TYPICAL ) ;
   }
  else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL1 )
   {
    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_TYPICAL ) ; 
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_TYPICAL ) ;

    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_UP,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_UP) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_UP) ;
    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_DN,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_DN) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_DN) ;
   }
  else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 )
   {
    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_TYPICAL,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_TYPICAL,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_TYPICAL ) ; 
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_TYPICAL ) ;

    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_UP,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_UP,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_UP) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_UP) ;
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_UP_MIN) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_UP_MIN) ;
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_UP_MAX) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_UP_MAX) ;

    capasource = elpLotrsCapaSource(lotrs,ELP_CAPA_DN,transcase);
    capadrain  = elpLotrsCapaDrain(lotrs,ELP_CAPA_DN,transcase);
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_DN) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_DN) ;
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_DN_MIN) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_DN_MIN) ;
    elpAddCapaSig( lofig, lotrs->SOURCE->SIG, capasource, ELP_CAPA_DN_MAX) ;
    elpAddCapaSig( lofig, lotrs->DRAIN->SIG, capadrain, ELP_CAPA_DN_MAX) ;
   }
 }

return ;
}

/****************************************************************************\
 *
 * FUNC :  elpLofigAddCapas
 *
 * Annotate the lofig with the grid and diffusion capacitance
 *
\****************************************************************************/
void elpLofigAddCapas ( lofig_list *lofig, int transcase )
{
  ptype_list *ptype;
  char *techno = namealloc ( elpTechnoFile );

  ptype = getptype ( lofig->USER, ELP_LOFIG_CAPAS_ID);
  if ( !ptype || ( ptype && ( (char*)ptype->DATA != techno  ) )) {
    elpFreeCapaLofig ( lofig );
    elpLofigCapaGrid ( lofig, transcase );
    elpLofigCapaDiff ( lofig, transcase );
    if ( ptype )
      ptype->DATA = techno;
    else 
      lofig->USER = addptype ( lofig->USER, ELP_LOFIG_CAPAS_ID, techno);
  }
}

/****************************************************************************/
/*                    fonction LofigCapaGrid()                              */
/* calcule les capas de grille.                                             */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lofig: figure logique MBK dont dans laquelle il faut ajouter les   */
/*              capas de grille.                                            */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    Aucun!                                                                */
/****************************************************************************/
elpFCT void elpLofigCapaGrid(lofig,transcase)
struct lofig *lofig ;
int transcase;

{
lotrs_list *lotrs  ;
char *trsmodel;

for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
{
  trsmodel = getlotrsmodel(lotrs);
/*  if (mbk_isdioden(trsmodel) || mbk_isdiodep(trsmodel))
    continue;
*/
  if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL0 )
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,transcase,0), ELP_CAPA_TYPICAL);
  else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL1 )
   {
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,transcase,0), ELP_CAPA_TYPICAL);

    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,transcase,0), ELP_CAPA_UP);
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,transcase,0), ELP_CAPA_DN);
   }
  else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 )
   {
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,transcase,0), ELP_CAPA_TYPICAL);

    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP,transcase,0), ELP_CAPA_UP);
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN,transcase,0), ELP_CAPA_DN);
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP_MIN,transcase,0), ELP_CAPA_UP_MIN);
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_UP_MAX,transcase,0), ELP_CAPA_UP_MAX);
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN_MIN,transcase,0), ELP_CAPA_DN_MIN);
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_DN_MAX,transcase,0), ELP_CAPA_DN_MAX);
   }
  else
   {
    elpAddCapaSig( lofig, lotrs->GRID->SIG, elpLotrsInCapa(lofig,lotrs,ELP_CAPA_TYPICAL,transcase,0), ELP_CAPA_TYPICAL);
   }
}
return ;
}

/****************************************************************************/
/*                        fonction elpLotrsResiCanal()                      */
/* Calcule la résistance en petit signaux du transistor passé en parametre. */
/* Hypothèse : le transistor est bien sûr supposé être en régime linéaire.  */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: Transistor dont on veut obtenir la résistance.              */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    La résistance en ohms.                                                */
/****************************************************************************/

elpFCT double elpLotrsResiCanal(lotrs,transcase)
struct lotrs *lotrs;
int transcase;
{
  double          a, b, c ;
  double          w, l ;
  double          r,vdd ;
  elpmodel_list *model;
 
  vdd = elpGetVddFromCorner ( lotrs, transcase );
  model = elpGetModel(lotrs,vdd,transcase) ;
  if ( !model ) return 0.0;

  a = model->elpRss[elpRSSL] ;
  b = model->elpRss[elpRSSC] ;
  c = model->elpRss[elpRSSW] ;

  w = ((float)elpGetShrinkedWidth(lotrs,model))/((float)SCALE_X)*1e-6 ;
  l = ((float)elpGetShrinkedLength(lotrs,model))/((float)SCALE_X)*1e-6 ;

  if( a == 0.0 || b == 0.0 || c == 0.0 )
    r = model->elpModel[elpRT]*l/w;     /* Modèle approché utilisant MCC     */
  else
    r = ( a*l + b ) / ( c*w + 1.0 ) ;   /* Modèle fin                        */

  return r ;
}


/****************************************************************************/

double elpScm2Thr (double f, double smin, double smax, double Vt, double Vfinal, double Vdd, int type)
{
   double v0, v1;
   double x0, x1;
   double ff;

   if (smin < 0 && smax < 0) {
      ff = f;
   }
   else
   if (type == elpRISE) {
      v0 = smin * Vfinal;
      v1 = smax * Vfinal;
      if(v0 >= Vt){
          x0 = (atanh ((v0 - Vt) / (Vfinal - Vt)));
          x1 = (atanh ((v1 - Vt) / (Vfinal - Vt)));
      }else if(v1 >= Vt){
          x0 = (v0 - Vt) / (Vfinal - Vt);
          x1 = (atanh ((v1 - Vt) / (Vfinal - Vt)));
      }else{
          x0 = (v0 - Vt) / (Vfinal - Vt);
          x1 = (v1 - Vt) / (Vfinal - Vt);
      }
      ff = f * (x1 - x0);
   } 
   else
   if (type == elpFALL) {
      v1 = smax * (Vdd - Vfinal) + Vfinal;
      v0 = smin * (Vdd - Vfinal) + Vfinal;
      if(v1 < Vdd - Vt){
          x1 = (atanh ((v1 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt))));
          x0 = (atanh ((v0 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt))));
      }else if(v0 < Vdd - Vt){
          x1 = (v1 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt));
          x0 = (atanh ((v0 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt))));
      }else{
          x1 = (v1 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt));
          x0 = (v0 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt));
      }
      ff = f * (x0 - x1);
   }
   else {
      fprintf (stderr, "elp error : wrong transistion type\n");
      ff = f;
   }

   return ff;
}

/****************************************************************************/

double elpThr2Scm (double f, double smin, double smax, double Vt, double Vfinal, double Vdd, int type)
{
   double v0, v1;
   double x0, x1;
   double ff;

   if (smin < 0 && smax < 0) {
      ff = f;
   }
   else
   if (type == elpRISE) {
      v0 = smin * Vfinal;
      v1 = smax * Vfinal;
      if(v0 >= Vt){
          x0 = (atanh ((v0 - Vt) / (Vfinal - Vt)));
          x1 = (atanh ((v1 - Vt) / (Vfinal - Vt)));
      }else if(v1 >= Vt){
          x0 = (v0 - Vt) / (Vfinal - Vt);
          x1 = (atanh ((v1 - Vt) / (Vfinal - Vt)));
      }else{
          x0 = (v0 - Vt) / (Vfinal - Vt);
          x1 = (v1 - Vt) / (Vfinal - Vt);
      }
      ff = f / (x1 - x0);
   } 
   else
   if (type == elpFALL) {
      v1 = smax * (Vdd - Vfinal) + Vfinal;
      v0 = smin * (Vdd - Vfinal) + Vfinal;
      if(v1 < Vdd - Vt){
          x1 = (atanh ((v1 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt))));
          x0 = (atanh ((v0 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt))));
      }else if(v0 < Vdd - Vt){
          x1 = (v1 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt));
          x0 = (atanh ((v0 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt))));
      }else{
          x1 = (v1 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt));
          x0 = (v0 - (Vdd - Vt)) / (Vfinal - (Vdd - Vt));
      }
      ff = f / (x0 - x1);
   }
   else {
      fprintf (stderr, "elp error : wrong transistion type\n");
      ff = f;
   }

   return ff;
}

/****************************************************************************/
void (*elpSetDefaultParam) ( void ) ;

/****************************************************************************/
void elpdefaultfct2 ( void )
{
 return;
}

/****************************************************************************/
void elpSetFct2 (void (*f)( void ))
{
    if (f)
        elpSetDefaultParam = f;
    else
        elpSetDefaultParam = elpdefaultfct2;
}


/****************************************************************************/
void (*elpGenParam)(char* tname, int type, double l, double w, double vdd,
                    lotrs_list *lotrs, int transcase, elp_lotrs_param *ptlotrs_param) ;

/****************************************************************************/
void elpdefault (char* tname, int type, double l, double w, double vdd,
                 lotrs_list *lotrs, int transcase, elp_lotrs_param *ptlotrs_param)
{
#ifndef __ALL__WARNING__
  tname	= NULL;
  type	= 0;
  l		= 0.0;
  w		= 0.0;
  vdd	= 0.0;
  lotrs = NULL;
  transcase = elpTYPICAL;
  ptlotrs_param = NULL;
#endif

    return;
}

/****************************************************************************/
void elpSetFct(void (*f)(char* p0, int p1, double p2, double p3, double p4,
                         lotrs_list *, int, elp_lotrs_param *ptlotrs_param))
{
    if (f)
        elpGenParam = f;
    else
        elpGenParam = elpdefault;
}

/****************************************************************************/
double (*elpCalcIleak)(char* tname, int type, double l, double w,
                       lotrs_list *lotrs, int transcase, 
                       double vgs, double vds, double vbs,
                       double AD, double PD, double AS, double PS,
                       double *BLeak, double *DLeak, double *SLeak,
                       elp_lotrs_param *ptlotrs_param) ;

/****************************************************************************/
double elpLeakDfltFct (char* tname, int type, double l, double w, 
                       lotrs_list *lotrs, int transcase, 
                       double vgs, double vds, double vbs,
                       double AD, double PD, double AS, double PS,
                       double *BLeak, double *DLeak, double *SLeak,
                       elp_lotrs_param *ptlotrs_param)
{
#ifndef __ALL__WARNING__
  tname	= NULL;
  type	= 0;
  l		= 0.0;
  w		= 0.0;
  vgs	= 0.0;
  vds	= 0.0;
  vbs	= 0.0;
  AD 	= 0.0;
  PD 	= 0.0;
  AS 	= 0.0;
  PS 	= 0.0;
  BLeak = NULL;
  DLeak = NULL;
  SLeak = NULL;
  lotrs = NULL;
  transcase = elpTYPICAL;
  ptlotrs_param = NULL;
#endif

    return 0.0;
}

/****************************************************************************/
void elpSetLeakFct(double (*f)(char* p0, int p1, double p2, double p3, 
                         lotrs_list *, int, double p4, double p5, double p6,
                         double p7, double p8, double p9, double p10,
                         double *t0, double *t1, double *t2,
                         elp_lotrs_param *ptlotrs_param))
{
    if (f)
        elpCalcIleak = f;
    else
        elpCalcIleak = elpLeakDfltFct;
}
/****************************************************************************/
extern void elpSetCalcPAFct( void(*f)(lotrs_list*,char*,int,int,double,elp_lotrs_param*,double*,double*,double*,double*) )
{
  elpCalcPAFct = f ;
}

/****************************************************************************/
elpFCT int elpLoadOnceElp ()
{

    if (!elpoldtechnofilename || 
        (strcasecmp(elpoldtechnofilename,elpTechnoFile))) {
        elpres_load = elpLoadElp();
        if (elpoldtechnofilename) mbkfree(elpoldtechnofilename);
        elpoldtechnofilename = mbkstrdup(elpTechnoFile);
    }
    return elpres_load;
}

/****************************************************************************/
elpFCT float elpGetCapaFromLocon (locon,capatype,transcase)
    locon_list *locon;
    int capatype;
    int transcase;
{
    lotrs_list *lotrs;
    float       capa = 0.0;

    if ((!locon) || (locon->TYPE != 'T'))
        return 0.0;
    else {
        ELP_CALC_ONLY_CAPA = 1;
        lotrs = (lotrs_list*)locon->ROOT;
        elpLoadOnceElp();
        if (lotrs->DRAIN == locon)
            capa = elpLotrsCapaDrain (lotrs,capatype,transcase);
        else if (lotrs->SOURCE == locon)
            capa = elpLotrsCapaSource (lotrs,capatype,transcase);
        else if (lotrs->GRID == locon)
            capa = elpLotrsInCapa(NULL,lotrs,capatype,transcase,0);
        else
            capa = 0.0;
        ELP_CALC_ONLY_CAPA = 0;
    }
    return capa;
}

/******************************************************************************\
elpGetCapaSig : Renvoie la capacité mémorisée sur un signal, ou 0.0 si rien n'a
                été mémorisé.
\******************************************************************************/

elpFCT float elpGetCapaSig ( lofig, losig, type )
    lofig_list *lofig;
    losig_list *losig;
    int         type;
{
  ptype_list     *ptl;
  float           capa;
  struct elpcapa *capas;
  int             pos;


  ptl = getptype( lofig->USER, ELP_LOFIG_TABCAPA );
  if( !ptl )
    capa = ELP_NOCAPA;
  else {
    capas = (struct elpcapa*)(ptl->DATA);
    pos   = losig->INDEX - 1;

    if( pos < 0 || pos >= capas->NBSIG ) {
      fflush( stdout );
      fprintf( stderr, "\nelp error : bad index for signal %s\n", 
                       getsigname( losig ) 
             );
      capa = ELP_NOCAPA;
    }
    else {
      switch ( type ) {
        case ELP_CAPA_TYPICAL : capa = capas->TAB[pos];
                                break;
        case ELP_CAPA_UP      : capa = capas->TAB_UP[pos];
                                break;
        case ELP_CAPA_UP_MIN  : capa = capas->TAB_UP_MIN[pos];
                                break;
        case ELP_CAPA_UP_MAX  : capa = capas->TAB_UP_MAX[pos];
                                break;
        case ELP_CAPA_DN      : capa = capas->TAB_DN[pos];
                                break;
        case ELP_CAPA_DN_MIN  : capa = capas->TAB_DN_MIN[pos];
                                break;
        case ELP_CAPA_DN_MAX  : capa = capas->TAB_DN_MAX[pos];
                                break;
      }
    }
  }

  if( capa == ELP_NOCAPA )
    return 0.0;

  return capa;
}


/******************************************************************************\
elpGetTotalCapaSig : Renvoie toutes les capacités sur un signal.
\******************************************************************************/

elpFCT float elpGetTotalCapaSig( lofig, losig, type )
    lofig_list *lofig;
    losig_list *losig;
    int         type;
{
  float capa=0.0, c ;
  
  c = rcn_getcapa( lofig, losig );
  capa = capa + c;
  c = elpGetCapaSig( lofig, losig, type );
  capa = capa + c;

  return capa;
}

/******************************************************************************\
elpSetCapaSig : Mémorise une capacité sur un signal.
\******************************************************************************/

elpFCT void elpSetCapaSig ( lofig, losig, capa, type )
    lofig_list *lofig;
    losig_list *losig;
    float       capa;
    int         type;
{
  ptype_list     *ptl;
  struct elpcapa *capas;
  int             n;
  int             pos;
  
  ptl = getptype( lofig->USER, ELP_LOFIG_TABCAPA );
  
  if( !ptl ) {
  
    lofig->USER = addptype( lofig->USER, ELP_LOFIG_TABCAPA, NULL );
    ptl = lofig->USER;
    
    capas = (struct elpcapa*)mbkalloc( sizeof( struct elpcapa ) );
    capas->NBSIG      = getnumberoflosig( lofig );
    capas->TAB        = NULL;
    capas->TAB_UP     = NULL;
    capas->TAB_DN     = NULL;
    capas->TAB_UP_MIN = NULL;
    capas->TAB_UP_MAX = NULL;
    capas->TAB_DN_MIN = NULL;
    capas->TAB_DN_MAX = NULL;

    if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL0 ) 
     {
      capas->TAB        = mbkalloc( sizeof( float ) * capas->NBSIG );

      for( n=0 ; n<capas->NBSIG ; n++ ) 
        capas->TAB[n]        = ELP_NOCAPA;
     }
    else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL1 )
     {
      capas->TAB        = mbkalloc( sizeof( float ) * capas->NBSIG );

      capas->TAB_UP     = mbkalloc( sizeof( float ) * capas->NBSIG );
      capas->TAB_DN     = mbkalloc( sizeof( float ) * capas->NBSIG );

      for( n=0 ; n<capas->NBSIG ; n++ ) 
       {
        capas->TAB[n]        = ELP_NOCAPA;

        capas->TAB_UP[n]     = ELP_NOCAPA;
        capas->TAB_DN[n]     = ELP_NOCAPA;
       }
     }
    else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 )
     {
      capas->TAB        = mbkalloc( sizeof( float ) * capas->NBSIG );

      capas->TAB_UP     = mbkalloc( sizeof( float ) * capas->NBSIG );
      capas->TAB_DN     = mbkalloc( sizeof( float ) * capas->NBSIG );
      capas->TAB_UP_MIN = mbkalloc( sizeof( float ) * capas->NBSIG );
      capas->TAB_UP_MAX = mbkalloc( sizeof( float ) * capas->NBSIG );
      capas->TAB_DN_MIN = mbkalloc( sizeof( float ) * capas->NBSIG );
      capas->TAB_DN_MAX = mbkalloc( sizeof( float ) * capas->NBSIG );

      for( n=0 ; n<capas->NBSIG ; n++ ) 
       {
        capas->TAB[n]        = ELP_NOCAPA;

        capas->TAB_UP[n]     = ELP_NOCAPA;
        capas->TAB_DN[n]     = ELP_NOCAPA;
        capas->TAB_UP_MIN[n] = ELP_NOCAPA;
        capas->TAB_UP_MAX[n] = ELP_NOCAPA;
        capas->TAB_DN_MIN[n] = ELP_NOCAPA;
        capas->TAB_DN_MAX[n] = ELP_NOCAPA;
       }
     }
    else 
     {
      capas->TAB        = mbkalloc( sizeof( float ) * capas->NBSIG );

      for( n=0 ; n<capas->NBSIG ; n++ ) 
        capas->TAB[n]        = ELP_NOCAPA;
     }

    ptl->DATA = capas;
  }
  else
    capas = (struct elpcapa*)ptl->DATA;

  pos = losig->INDEX - 1;
  if( pos < 0 || pos >= capas->NBSIG ) {
    fflush( stdout );
    fprintf( stderr, "\nelp error : bad index for signal %s\n", 
                     getsigname( losig ) 
           );
  }
  else {
    switch ( type ) {
      case ELP_CAPA_TYPICAL : capas->TAB[pos] = capa;
                              break;
      case ELP_CAPA_UP      : capas->TAB_UP[pos] = capa;
                              break;
      case ELP_CAPA_UP_MIN  : capas->TAB_UP_MIN[pos] = capa;
                              break;
      case ELP_CAPA_UP_MAX  : capas->TAB_UP_MAX[pos] = capa;
                              break;
      case ELP_CAPA_DN      : capas->TAB_DN[pos] = capa;
                              break;
      case ELP_CAPA_DN_MIN  : capas->TAB_DN_MIN[pos] = capa;
                              break;
      case ELP_CAPA_DN_MAX  : capas->TAB_DN_MAX[pos] = capa;
                              break;
    }
  }
}

/******************************************************************************\
elpIsCapaSig : Renvoie 1 si on a déjà mémorisé une capacité sur le signal.
\******************************************************************************/

elpFCT char elpIsCapaSig( lofig, losig, type )
    lofig_list *lofig;
    losig_list *losig;
    int         type;
{
  ptype_list     *ptl;
  struct elpcapa *capas;
  int             pos,res;

  ptl = getptype( lofig->USER, ELP_LOFIG_TABCAPA );
  if( !ptl )
    return 0;

  capas = (struct elpcapa*)(ptl->DATA);
  pos   = losig->INDEX - 1;

  if( pos < 0 || pos >= capas->NBSIG ) {
    fflush( stdout );
    fprintf( stderr, "\nelp error : bad index for signal %s\n", 
                     getsigname( losig ) 
           );
    return 0;
  }
  
  res = 1;
  switch ( type ) {
    case ELP_CAPA_TYPICAL : if( !capas->TAB || capas->TAB[pos] == ELP_NOCAPA )
                              res = 0;
                            break;
    case ELP_CAPA_UP      : if( !capas->TAB_UP || capas->TAB_UP[pos] == ELP_NOCAPA )
                              res = 0;
                            break;
    case ELP_CAPA_UP_MIN  : if (!capas->TAB_UP_MIN || capas->TAB_UP_MIN[pos] == ELP_NOCAPA)
                              res = 0;
                            break;
    case ELP_CAPA_UP_MAX  : if (!capas->TAB_UP_MAX || capas->TAB_UP_MAX[pos] == ELP_NOCAPA)
                              res = 0;
                            break;
    case ELP_CAPA_DN      : if(!capas->TAB_DN ||  capas->TAB_DN[pos] == ELP_NOCAPA )
                              res = 0;
                            break;
    case ELP_CAPA_DN_MIN  : if (!capas->TAB_DN_MIN || capas->TAB_DN_MIN[pos] == ELP_NOCAPA)
                              res = 0;
                            break;
    case ELP_CAPA_DN_MAX  : if (!capas->TAB_DN_MAX || capas->TAB_DN_MAX[pos] == ELP_NOCAPA)
                              res = 0;
                            break;
  }
  return res;
}

/******************************************************************************\
elpAddCapaSig : Ajoute une capacité sur un signal dans la capacité mémorisée.
\******************************************************************************/

elpFCT float elpAddCapaSig( lofig, losig, capa, type )
    lofig_list *lofig;
    losig_list *losig;
    float       capa;
    int         type;
{
  float newcapa;

  newcapa = capa + elpGetCapaSig( lofig, losig, type );
  elpSetCapaSig( lofig, losig, newcapa, type );

  return newcapa;
}

/******************************************************************************\
elpFreeCapaLofig : Efface toutes les capacités de la lofig
\******************************************************************************/
elpFCT void elpFreeCapaLofig( lofig )
    lofig_list *lofig;
{
  ptype_list     *ptl;
  struct elpcapa *capas;

  ptl = getptype( lofig->USER, ELP_LOFIG_TABCAPA );
  if( !ptl ) return;
  capas = (struct elpcapa*)(ptl->DATA);

  if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL0 )
    mbkfree( capas->TAB );
  else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL1 )
   {
    mbkfree( capas->TAB );

    mbkfree( capas->TAB_UP);
    mbkfree( capas->TAB_DN);
   }
  else if ( V_INT_TAB[__ELP_CAPA_LEVEL].VALUE == ELP_CAPA_LEVEL2 )
   {
    mbkfree( capas->TAB );

    mbkfree( capas->TAB_UP);
    mbkfree( capas->TAB_DN);
    mbkfree( capas->TAB_UP_MIN );
    mbkfree( capas->TAB_UP_MAX );
    mbkfree( capas->TAB_DN_MIN );
    mbkfree( capas->TAB_DN_MAX );
   }
  else
    mbkfree( capas->TAB );

  mbkfree( capas );

  lofig->USER = delptype( lofig->USER, ELP_LOFIG_TABCAPA );
}

/****************************************************************************/
/*                        fonction LofigUnShrink()                          */
/*                                                                          */
/* Repositionne les vrais tailles des transistors                           */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lofig: figure logique MBK dont dans laquelle il faut modifier les  */
/*              dimensions des transistors.                                 */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    Aucun!                                                                */
/****************************************************************************/
elpFCT void elpLofigUnShrink(lofig,transcase)
struct lofig *lofig ;
int transcase;

{
lotrs_list *lotrs  ;

for(lotrs = lofig->LOTRS ; lotrs != NULL ; lotrs = lotrs->NEXT)
elpLotrsUnShrink(lotrs,transcase) ;

return ;
}

/****************************************************************************/
/*                           fonction LotrsUnShrink()                       */
/* modifie les dimensions du transistor en fonction des parametres de       */
/* shrink et du parametre ACM.                                              */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: transistor logique MBK dont on veut modifier les dimensions */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    Aucun!                                                                */
/****************************************************************************/
elpFCT void elpLotrsUnShrink(lotrs,transcase)
struct lotrs *lotrs ;
int transcase;

{
double deltal , vdd,
       deltaw ;
elpmodel_list *model;

vdd = elpGetVddFromCorner ( lotrs, transcase );
model = elpGetModel(lotrs,vdd,transcase) ;
if ( !model ) return;

if((lotrs->WIDTH == 0) || (lotrs->LENGTH == 0))
 return ;

     if(model->elpTransModel == elpMOS)
       {
        deltal = model->elpShrink[elpDL] * (double)SCALE_X ;
        deltaw = model->elpShrink[elpDW] * (double)SCALE_X ;
       }
lotrs->LENGTH = elpDouble2Long((((double)lotrs->LENGTH - deltal) / model->elpShrink[elpLMLT])) ;
lotrs->WIDTH = elpDouble2Long((((double)lotrs->WIDTH - deltaw) / model->elpShrink[elpWMLT]));
lotrs->XS = elpDouble2Long(((double)lotrs->XS / (model->elpShrink[elpWMLT] + deltaw/(double)lotrs->WIDTH))) ;
lotrs->XD = elpDouble2Long(((double)lotrs->XD / (model->elpShrink[elpWMLT] + deltaw/(double)lotrs->WIDTH))) ;

if(elpGeneral[elpACM] == 1.0)
    {
     lotrs->XS = lotrs->XD = elpDouble2Long((model->elpShrink[elpWMLT] * (double)SCALE_X)) ;
     lotrs->PS = lotrs->PD = lotrs->WIDTH ;
    }

return ;
}

//-------------------------------------------------------------------------
elpFCT void elpLotrsGetUnShrinkDim( lotrs, length, width, length_s, width_s, transcase )
struct lotrs *lotrs ;
long length, width, *length_s, *width_s ;
int transcase;
{
double deltal ,vdd,
       deltaw ;
elpmodel_list *model;

vdd = elpGetVddFromCorner ( lotrs, transcase );
model = elpGetModel(lotrs,vdd,transcase) ;
if ( !model ) return;

if((lotrs->WIDTH == 0) || (lotrs->LENGTH == 0))
 return ;

if(model->elpTransModel == elpMOS)
{
  deltal = model->elpShrink[elpDL] * (double)SCALE_X ;
  deltaw = model->elpShrink[elpDW] * (double)SCALE_X ;
}

if( length_s )
        *length_s = elpDouble2Long((((double)length - deltal) / model->elpShrink[elpLMLT])) ;
if( width_s )
        *width_s = elpDouble2Long((((double)width - deltaw) / model->elpShrink[elpWMLT]));

return ;

}

/****************************************************************************/
/*                           fonction LotrsGetShrinkDim()                    */
/* modifie les dimensions du transistor en fonction des parametres de       */
/* shrink et du parametre ACM.                                              */
/*                                                                          */
/* Parametres en entree:                                                    */
/* --------------------                                                     */
/*    1) lotrs: transistor logique MBK dont on veut modifier les dimensions */
/*                                                                          */
/* Parametre en sortie:                                                     */
/* -------------------                                                      */
/*    Aucun!                                                                */
/****************************************************************************/
elpFCT void elpLotrsGetShrinkDim (lotrs,length_s,width_s,xs_s,xd_s,
                                  ps_s,pd_s,ptlactive,ptwactive,transcase)
    struct lotrs *lotrs ;
    long         *length_s;
    long         *width_s;
    long         *xs_s;
    long         *xd_s;
    long         *ps_s;
    long         *pd_s;
    long         *ptlactive;
    long         *ptwactive;
    int           transcase;
{
    long         l_s,w_s;
    long         lactive,wactive;
    double deltal, deltaw, dla, dwa,vdd;
    
    double          as=0.0, ad=0.0, ps=0.0, pd=0.0, xs=0.0, xd=0.0 ;
    double          mulu0, delvt0, sa, sb, sd, nf, m ,nrs, nrd, vbulk, sc, sca, scb, scc ;
    int             ptlotrs_param_filled ;
    elp_lotrs_param ptlotrs_param;
   
    elpmodel_list *model;

    vdd = elpGetVddFromCorner ( lotrs, transcase );
    model = elpGetModel(lotrs,vdd,transcase) ;
    if ( !model ) return;

    if((lotrs->WIDTH == 0) || (lotrs->LENGTH == 0)){
        if (xs_s != NULL)
            *xs_s = elpDouble2Long( (double)lotrs->XS) ;
        if (xd_s != NULL)
            *xd_s = elpDouble2Long( (double)lotrs->XD) ;
        if (ps_s != NULL)
            *ps_s = elpDouble2Long( (double)lotrs->PS) ;
        if (pd_s != NULL)
            *pd_s = elpDouble2Long( (double)lotrs->PD) ;
    }else{
        if(model->elpTransModel == elpMOS) {
            deltal = model->elpShrink[elpDL] * (double)SCALE_X ;
            deltaw = model->elpShrink[elpDW] * (double)SCALE_X ;
            dla = model->elpShrink[elpDLC] * (double)SCALE_X ;
            dwa = model->elpShrink[elpDWC] * (double)SCALE_X ;
        }
        l_s = elpDouble2Long((model->elpShrink[elpLMLT]*(double)lotrs->LENGTH + deltal)) ; 
        w_s = elpDouble2Long((model->elpShrink[elpWMLT]*(double)lotrs->WIDTH + deltaw)) ;
        lactive = elpDouble2Long((model->elpShrink[elpLMLT]*(double)lotrs->LENGTH + dla)) ; 
        wactive = elpDouble2Long((model->elpShrink[elpWMLT]*(double)lotrs->WIDTH + dwa)) ;
        if (length_s != NULL)
            *length_s = l_s;
        if (width_s != NULL)
            *width_s = w_s;
        if (ptlactive != NULL)
            *ptlactive = lactive;
        if (ptwactive != NULL)
            *ptwactive = wactive;

        ptlotrs_param_filled = 0 ;

        /******************************************************************************************************/
        if (xs_s != NULL) {
            if( lotrs->XS<0 ) {
              if( !ptlotrs_param_filled ) {   
                elp_lotrs_param_get (lotrs,&mulu0,&delvt0,&sa,&sb,&sd,&nf,&m,&nrs,&nrd,&vbulk,&sc,&sca,&scb,&scc);
                fill_elp_lotrs_param( &ptlotrs_param, mulu0, delvt0, sa, sb, sd, nf, m ,nrs, nrd, vbulk, sc, sca, scb, scc, NULL );
                ptlotrs_param_filled = 1 ;
              }
              elpCalcPAFct( lotrs,
                            getlotrsmodel( lotrs ),
                            MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS, 
                            transcase,
                            vdd,
                            &ptlotrs_param,
                            &as, 
                            NULL, 
                            NULL, 
                            NULL 
                          );
              xs = as * SCALE_X * SCALE_X * 1.0e12 / lotrs->WIDTH ;
            }
            else 
              xs = lotrs->XS ;
            *xs_s = elpDouble2Long(((model->elpShrink[elpWMLT] - deltaw/(double)w_s) * xs)) ;
        }

        /******************************************************************************************************/
        if (xd_s != NULL) {
            if( lotrs->XD<0) {
              if( !ptlotrs_param_filled ) {   
                elp_lotrs_param_get (lotrs,&mulu0,&delvt0,&sa,&sb,&sd,&nf,&m,&nrs,&nrd,&vbulk,&sc,&sca,&scb,&scc);
                fill_elp_lotrs_param( &ptlotrs_param, mulu0, delvt0, sa, sb, sd, nf, m ,nrs, nrd, vbulk, sc, sca, scb, scc, NULL );
                ptlotrs_param_filled = 1 ;
              }
              elpCalcPAFct( lotrs,
                            getlotrsmodel( lotrs ),
                            MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS, 
                            transcase,
                            vdd,
                            &ptlotrs_param,
                            NULL, 
                            &ad, 
                            NULL, 
                            NULL 
                          );
              xd = ad * SCALE_X * SCALE_X * 1.0e12 / lotrs->WIDTH ;
            }
            else 
              xd = lotrs->XD ;
            *xd_s = elpDouble2Long(((model->elpShrink[elpWMLT] - deltaw/(double)w_s) * xd)) ;
        }
        
        /******************************************************************************************************/
        if (ps_s != NULL) {
            if( lotrs->PS<0 ) {
              if( !ptlotrs_param_filled ) {   
                elp_lotrs_param_get (lotrs,&mulu0,&delvt0,&sa,&sb,&sd,&nf,&m,&nrs,&nrd,&vbulk,&sc,&sca,&scb,&scc);
                fill_elp_lotrs_param( &ptlotrs_param, mulu0, delvt0, sa, sb, sd, nf, m ,nrs, nrd, vbulk, sc, sca, scb, scc, NULL );
                ptlotrs_param_filled = 1 ;
              }

              elpCalcPAFct( lotrs,
                            getlotrsmodel( lotrs ),
                            MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS, 
                            transcase,
                            vdd,
                            &ptlotrs_param,
                            NULL, 
                            NULL, 
                            &ps, 
                            NULL 
                          );
              /*
              Ps et pd sont les dimensions efficace. Dans les champs PS et PD du
              lotrs, ce sont les dimensions telles qu'elles apparaissent dans le 
              fichier Spice.
              Le passage de l'un à l'autre s'effectue à l'aide le la relation 
              de la doc bsim4v30.pdf p 115.
              */

              ps = ps * SCALE_X * 1.0e6 + lotrs->WIDTH ;
            }
            else 
              ps = lotrs->PS<0?0:lotrs->PS ;
            *ps_s = ps ;
        }

        /******************************************************************************************************/
        if (pd_s != NULL) {
            if( lotrs->PD<0 ) {
              if( !ptlotrs_param_filled ) {   
                elp_lotrs_param_get (lotrs,&mulu0,&delvt0,&sa,&sb,&sd,&nf,&m,&nrs,&nrd,&vbulk,&sc,&sca,&scb,&scc);
                vdd = elpGetVddFromCorner ( lotrs, transcase );
                fill_elp_lotrs_param( &ptlotrs_param, mulu0, delvt0, sa, sb, sd, nf, m ,nrs, nrd, vbulk, sc, sca, scb, scc, NULL );
                ptlotrs_param_filled = 1 ;
              }
              elpCalcPAFct( lotrs,
                            getlotrsmodel( lotrs ),
                            MLO_IS_TRANSN(lotrs->TYPE) ? elpNMOS : elpPMOS, 
                            transcase,
                            vdd,
                            &ptlotrs_param,
                            NULL, 
                            NULL, 
                            NULL, 
                            &pd 
                          );
              /*
              Ps et pd sont les dimensions efficace. Dans les champs PS et PD du
              lotrs, ce sont les dimensions telles qu'elles apparaissent dans le 
              fichier Spice.
              Le passage de l'un à l'autre s'effectue à l'aide le la relation 
              de la doc bsim4v30.pdf p 115.
              */

              pd = pd * SCALE_X * 1.0e6 + lotrs->WIDTH ;
            }
            else 
              pd = lotrs->PD<0?0:lotrs->PD;
            *pd_s = pd;
        }

        if(elpGeneral[elpACM] == 1.0) {
            if (xs_s != NULL)
                *xs_s = elpDouble2Long((model->elpShrink[elpWMLT] * (double)SCALE_X)) ;
            if (xd_s != NULL)
                *xd_s = elpDouble2Long((model->elpShrink[elpWMLT] * (double)SCALE_X)) ;
            if (ps_s != NULL)
                *ps_s = w_s ;
            if (pd_s != NULL)
                *pd_s = w_s ;
        }
    }

return ;
}

/****************************************************************************/
/*                           fonction elpGetDeltaShrinkWidth                */
/****************************************************************************/
elpFCT void elpGetDeltaShrinkWidth (lotrs,ptdeff,ptdactive,ptwmlt,transcase)
    struct lotrs *lotrs ;
    long         *ptdeff;
    long         *ptdactive;
    long         *ptwmlt;
    int           transcase;
{
    long         deltaw=0, dwa=0;
    double vdd;
    elpmodel_list *model;

    vdd = elpGetVddFromCorner ( lotrs, transcase );
    model = elpGetModel(lotrs,vdd,transcase) ;
    if ( !model ) return;

    if((lotrs->WIDTH == 0) || (lotrs->LENGTH == 0)){
        if (ptdeff != NULL)
            *ptdeff = 0 ;
        if (ptdactive != NULL)
            *ptdactive = 0 ;
        if (ptwmlt != NULL)
            *ptwmlt = 0 ;
    }else{
        if(model->elpTransModel == elpMOS) {
            deltaw = elpDouble2Long(model->elpShrink[elpDW] * (double)SCALE_X) ;
            dwa = elpDouble2Long(model->elpShrink[elpDWC] * (double)SCALE_X) ;
        }
        if (ptdeff != NULL)
            *ptdeff = deltaw;
        if (ptdactive != NULL)
            *ptdactive = dwa;
        if (ptwmlt != NULL)
            *ptwmlt = elpDouble2Long(model->elpShrink[elpWMLT]);
    }

return ;
}

// Func : elpGetShrinkedLength
// Obtient la longueur shrinkee d'un transistor

long elpGetShrinkedLength (lotrs_list *lotrs, elpmodel_list *model)
{
    double deltal;
    long length_s = 0;

    if (!lotrs || !model) return 0;
    if (lotrs->LENGTH == 0) return 0;
    if(model->elpTransModel == elpMOS)
        deltal = model->elpShrink[elpDL] * (double)SCALE_X ;
    length_s = elpDouble2Long((model->elpShrink[elpLMLT]*(double)lotrs->LENGTH + deltal)) ;

    return length_s;
}

// Func : elpGetShrinkedWidth 
// Obtient la largueur shrinkee d'un transistor

long elpGetShrinkedWidth (lotrs_list *lotrs, elpmodel_list *model)
{
    double deltaw;
    long width_s = 0;

    if (!lotrs || !model) return 0;
    if (lotrs->WIDTH == 0) return 0;
    if(model->elpTransModel == elpMOS)
        deltaw = model->elpShrink[elpDW] * (double)SCALE_X ;
    width_s = elpDouble2Long((model->elpShrink[elpWMLT]*(double)lotrs->WIDTH + deltaw)) ;
    return width_s;
}

/****************************************************************************\
 FUNCTION : elp_lotrs_param_alloc
\****************************************************************************/
elp_lotrs_param *elp_lotrs_param_alloc ( void )
{
  elp_lotrs_param *ptlotrs_param;
  int i;

  ptlotrs_param = (elp_lotrs_param*) mbkalloc (sizeof (elp_lotrs_param));

  for (i = 0 ; i < elpPARAMNUM ; i++)
    ptlotrs_param->PARAM[i] = ELPINITVALUE;
  for (i = 0 ; i < __MCC_LAST_SAVED ; i++)
    ptlotrs_param->MCC_SAVED[i] = ELPINITVALUE;

  ptlotrs_param->VBULK  = ELPINITVBULK;
  ptlotrs_param->ISVBSSET = 0;
  ptlotrs_param->longkey = NULL;
  return ptlotrs_param;
}

/******************************************************************************\
 FUNCTION : elp_lotrs_param_free
\******************************************************************************/
void elp_lotrs_param_free (elp_lotrs_param *ptlotrs_param)
{
  if (ptlotrs_param)
  {
    freechain(ptlotrs_param->longkey);
    mbkfree (ptlotrs_param);
  }
}

/******************************************************************************\
 FUNCTION : elp_lotrs_param_create

 Cree une structure d'instance avec des parametres specifiques si le transistor
 en comporte
\******************************************************************************/
elp_lotrs_param *elp_lotrs_param_create (lotrs_list *lotrs)
{
  elp_lotrs_param *ptlotrs_param = NULL;
  float delvt0, mulu0,val;
  float sa, sb, sd, nf, vbulk;
  float sc, sca, scb, scc;
  int status;

  ptlotrs_param = elp_lotrs_param_alloc ();

  mulu0 = getlotrsparam (lotrs,MBK_MULU0,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpMULU0] = mulu0;
  else
    ptlotrs_param->PARAM[elpMULU0] = 1.0;

  delvt0 = getlotrsparam (lotrs,MBK_DELVT0,NULL,&status);
  if ( status == 1 ) {
    ptlotrs_param->PARAM[elpDELVT0] = delvt0;
  }
  else
    ptlotrs_param->PARAM[elpDELVT0] = 0.0;

  sa = getlotrsparam (lotrs,MBK_SA,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSA] = sa;

  sb = getlotrsparam (lotrs,MBK_SB,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSB] = sb;

  sd = getlotrsparam (lotrs,MBK_SD,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSD] = sd;

  sc = getlotrsparam (lotrs,MBK_SC,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSC] = sc;

  sca = getlotrsparam (lotrs,MBK_SCA,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSCA] = sca;

  scb = getlotrsparam (lotrs,MBK_SCB,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSCB] = scb;

  scc = getlotrsparam (lotrs,MBK_SCC,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpSCC] = scc;

  nf = getlotrsparam (lotrs,MBK_NF,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpNF] = nf;
  else {
    nf = getlotrsparam (lotrs,MBK_NFING,NULL,&status);
    if ( status == 1 ) 
      ptlotrs_param->PARAM[elpNF] = nf;
  }

  val = getlotrsparam (lotrs,MBK_M,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpM] = val;
  else
    val = getlotrsparam (lotrs,MBK_MULT,NULL,&status);
    if( status == 1 )
      ptlotrs_param->PARAM[elpM] = val;
    else
      ptlotrs_param->PARAM[elpM] = 1.0;

  val = getlotrsparam (lotrs,MBK_NRS,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpNRS] = val;

  val = getlotrsparam (lotrs,MBK_NRD,NULL,&status);
  if ( status == 1 ) 
    ptlotrs_param->PARAM[elpNRD] = val;

  if ( lotrs->BULK && lotrs->BULK->SIG && getlosigalim(lotrs->BULK->SIG, &vbulk) ) 
    ptlotrs_param->VBULK = vbulk;

  ptlotrs_param->ISVBSSET=0;
  ptlotrs_param->longkey=NULL;
  return ptlotrs_param;
}

/****************************************************************************\
 FUNCTION : elp_lotrs_param_dup  
\****************************************************************************/
elp_lotrs_param *elp_lotrs_param_dup ( elp_lotrs_param *src )
{
  elp_lotrs_param *dest = NULL ;

  if ( src ) {
    dest = (elp_lotrs_param*) mbkalloc (sizeof (elp_lotrs_param));
    memcpy(dest, src, sizeof(elp_lotrs_param));
/*    dest->PARAM[elpMULU0]  = src->PARAM[elpMULU0];
    dest->PARAM[elpDELVT0] = src->PARAM[elpDELVT0];
    dest->PARAM[elpSA] = src->PARAM[elpSA];
    dest->PARAM[elpSB] = src->PARAM[elpSB];
    dest->PARAM[elpSD] = src->PARAM[elpSD];
    dest->PARAM[elpSC] = src->PARAM[elpSC];
    dest->PARAM[elpSCA] = src->PARAM[elpSCA];
    dest->PARAM[elpSCB] = src->PARAM[elpSCB];
    dest->PARAM[elpSCC] = src->PARAM[elpSCC];
    dest->PARAM[elpNF] = src->PARAM[elpNF];
    dest->PARAM[elpM] = src->PARAM[elpM];
    dest->PARAM[elpNRS] = src->PARAM[elpNRS];
    dest->PARAM[elpNRD] = src->PARAM[elpNRD];
    dest->VBULK  = src->VBULK;
    dest->ISVBSSET = src->ISVBSSET ;
    dest->VBS      = src->VBS ;*/
    dest->longkey  = dupchainlst(src->longkey) ;
    
  }
  return dest;
}

/******************************************************************************\
 FUNCTION : elp_lotrs_param_get   

\******************************************************************************/
void elp_lotrs_param_get (lotrs_list *lotrs, 
                          double *ptmulu0,
                          double *ptdelvt0,
                          double *ptsa,
                          double *ptsb,
                          double *ptsd,
                          double *ptnf,
                          double *ptm,
                          double *ptnrs,
                          double *ptnrd,
                          double *ptvbulk,
                          double *ptsc,
                          double *ptsca,
                          double *ptscb,
                          double *ptscc
                         )
{
  double delvt0, mulu0;
  double sa, sb, sd,nf;
  double sca, scb, scc, sc ;
  double val;
  float vbulk;
  int status;

  if ( ptdelvt0 ) {
    delvt0 = getlotrsparam (lotrs,MBK_DELVT0,NULL,&status);
    if ( status == 1 ) 
      *ptdelvt0 = delvt0;
    else
      *ptdelvt0 = 0.0;
  }
  if ( ptmulu0 ) {
    mulu0 = getlotrsparam (lotrs,MBK_MULU0,NULL,&status);
    if ( status == 1 ) 
      *ptmulu0 = mulu0;
    else
      *ptmulu0 = 1.0;
  }
  if ( ptsa ) {
    sa = getlotrsparam (lotrs,MBK_SA,NULL,&status);
    if ( status == 1 ) 
      *ptsa = sa;
    else
      *ptsa = ELPINITVALUE;
  }
  if ( ptsb ) {
    sb = getlotrsparam (lotrs,MBK_SB,NULL,&status);
    if ( status == 1 ) 
      *ptsb = sb;
    else
      *ptsb = ELPINITVALUE;
  }
  if ( ptsd ) {
    sd = getlotrsparam (lotrs,MBK_SD,NULL,&status);
    if ( status == 1 ) 
      *ptsd = sd;
    else
      *ptsd = ELPINITVALUE;
  }
  if ( ptnf ) {
    nf = getlotrsparam (lotrs,MBK_NF,NULL,&status);
    if ( status == 1 ) 
      *ptnf = nf;
    else {
      nf = getlotrsparam (lotrs,MBK_NFING,NULL,&status);
      if ( status == 1 ) 
        *ptnf = nf;
      else
        *ptnf = 1.0;
    }
  }
  if ( ptm ) {
    val = getlotrsparam (lotrs,MBK_M,NULL,&status);
    if ( status == 1 ) 
      *ptm = val;
    else {
      val = getlotrsparam (lotrs,MBK_MULT,NULL,&status);
      if ( status == 1 )
        *ptm = val ;
      else
        *ptm = 1.0;
    }
  }
  if ( ptnrs ) {
    val = getlotrsparam (lotrs,MBK_NRS,NULL,&status);
    if ( status == 1 ) 
      *ptnrs = val;
    else
      *ptnrs = 0.0;
  }
  if ( ptnrd ) {
    val = getlotrsparam (lotrs,MBK_NRD,NULL,&status);
    if ( status == 1 ) 
      *ptnrd = val;
    else
      *ptnrd = 0.0;
  }
  if ( ptvbulk ) {
    if ( lotrs->BULK && lotrs->BULK->SIG && getlosigalim(lotrs->BULK->SIG, &vbulk) )
      *ptvbulk = vbulk;
    else {
      if ( MLO_IS_TRANSN(lotrs->TYPE ) )
        *ptvbulk = 0.0;
      else
        *ptvbulk = elpGeneral[elpGVDDMAX];
    }
  }
  if ( ptsc ) {
    sc = getlotrsparam (lotrs,MBK_SC,NULL,&status);
    if ( status == 1 ) 
      *ptsc = sc;
    else
      *ptsc = ELPINITVALUE;
  }
  if ( ptsca ) {
    sca = getlotrsparam (lotrs,MBK_SCA,NULL,&status);
    if ( status == 1 ) 
      *ptsca = sca;
    else
      *ptsca = ELPINITVALUE;
  }
  if ( ptscb ) {
    scb = getlotrsparam (lotrs,MBK_SCB,NULL,&status);
    if ( status == 1 ) 
      *ptscb = scb;
    else
      *ptscb = ELPINITVALUE;
  }
  if ( ptscc ) {
    scc = getlotrsparam (lotrs,MBK_SCC,NULL,&status);
    if ( status == 1 ) 
      *ptscc = scc;
    else
      *ptscc = ELPINITVALUE;
  }
}

/****************************************************************************\
 FUNCTION : elp_is_valcomprise
 Renvoie 1 si la valeur est comprise dans l'intervalle definit 
 par la precision definit entre 0 et 1
 Si la precision est < 0.0 , il faut une stricte egalitee
\****************************************************************************/
int elp_is_valcomprise ( long val1, long val2, double precision )
{
  int res = 0;
  long min,max;
  long abs_val1;
  long abs_val2;
  
  if ( precision < 0.0 ) {
    if ( val1 == val2 )
      res = 1;
  }
  else {
    abs_val1 = abs (val1);
    abs_val2 = abs (val2);
    min = elpDouble2Long (abs_val2 * (1.0 - precision));
    max = elpDouble2Long (abs_val2 * (1.0 + precision));
    if ( abs_val1 >= min && abs_val1 <= max )
      res = 1;
  }
  return res;
}

/****************************************************************************\
 FUNCTION : elp_scale_vth
\****************************************************************************/
double elp_scale_vth ( lotrs_list *lotrs, elpmodel_list *model )
{
 double delvt0;
 double vt,res,delta;

 if ( lotrs && model ) {
   vt = model->elpModel[elpVT];
   res = vt;
   elp_lotrs_param_get (lotrs,NULL,&delvt0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   if ( elpDouble2Long (SCALE_X*SCALE_X*delvt0) != 
        elpDouble2Long (SCALE_X*SCALE_X*model->elpModel[elpDELVT0]) ) {
     delta = delvt0 - model->elpModel[elpDELVT0];
     res = MLO_IS_TRANSN(lotrs->TYPE) ? vt + delta : vt - delta; 
   }
 }
 return res;
}

/****************************************************************************\
 FUNCTION : elp_scale_a  
\****************************************************************************/
double elp_scale_a ( lotrs_list *lotrs, elpmodel_list *model )
{
 double mulu0;
 double a,res,rap;

 if ( lotrs && model ) {
   a = model->elpModel[elpA];
   res = a;
   elp_lotrs_param_get (lotrs,&mulu0,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL);
   if ( elpDouble2Long (SCALE_X*SCALE_X*mulu0) != 
        elpDouble2Long (SCALE_X*SCALE_X*model->elpModel[elpMULU0]) ) {
     rap = mulu0 / model->elpModel[elpMULU0] ;
     res = a*rap;
   }
 }
 return res;
}


/****************************************************************************\
 FUNCTION : elpDriveModel
\****************************************************************************/
void elpDriveModel (char *filename, elpmodel_list *model)
{
 FILE *file ;
      
 sprintf(filename, "%s%s",filename,".elp") ; 
 file = fopen(filename,"a") ;
 if(file == NULL) {
   fprintf(stderr, "elp error: can't open file %s to drive!!!\n",filename) ;
   return ;
 }

 elpVerifModel(model) ;
 elpDriveOneModel (file, model);

 if (fclose(file) != 0)
   avt_errmsg(ELP_ERRMSG, "003", AVT_ERROR, filename);
//   elpError(1003,filename) ;
}

/****************************************************************************\
 FUNCTION : elpDriveModel
\****************************************************************************/
void elpDriveOneModel (FILE *file, elpmodel_list *model)
{
 if ( !model ) return;
 if(model->elpTransIndex == elpNOINDEX)
   fprintf(file, "BEGIN %s\n\n",model->elpModelName) ;
 else
   fprintf(file, "BEGIN %s [%d]\n\n",model->elpModelName, model->elpTransIndex) ;
 fprintf(file, "#transistor identification\n") ;
 fprintf(file, "TECHNO = %d\n", model->elpTransTechno );
 if (model->elpTransType == elpNMOS)
   fprintf(file, "TYPE = NMOS\n\n") ;
 else if (model->elpTransType == elpPMOS)
   fprintf(file, "TYPE = PMOS\n\n") ;
 else
   fprintf(file, "TYPE = NMOS\n\n") ;

 if (model->elpTransCase == elpTYPICAL) {
 	fprintf(file, "#model for best, typical or worst case\n") ;
     fprintf(file, "CASE = TYPICAL\n\n") ;
 }
 if(model->elpTransCase != elpTYPICAL)
  {
   fprintf(file, "#model for best, typical or worst case\n") ;
   fprintf(file, "CASE = %s\n\n",(model->elpTransCase == elpWORST) ? "WORST" : "BEST") ;
  }
 
 fprintf(file, "#voltage and temperature characteristics\n") ;
 fprintf(file, "VDDmax = %g\n", model->elpVoltage[elpVDDMAX]) ;
 fprintf(file, "VDEG   = %g\n", model->elpVoltage[elpVDEG]) ;
 fprintf(file, "VTI    = %g\n", model->elpVoltage[elpVTI]) ;
 fprintf(file, "VBULK  = %g\n", model->elpVoltage[elpVBULK]) ;
 fprintf(file, "TEMP   = %g\n\n", model->elpTemp) ;

 
 if((model->elpRange[elpLMIN] != (long)0) ||
    (model->elpRange[elpLMAX] != (long)ELPMAXLONG) ||
    (model->elpRange[elpWMIN] != (long)0) ||
    (model->elpRange[elpWMAX] != (long)ELPMAXLONG))
     fprintf(file, "#dimension range (micron)\n") ;

 if(model->elpRange[elpLMIN] != (long)0)
 fprintf(file, "LMIN = %g\n",(double)model->elpRange[elpLMIN]/(double)SCALE_X) ;    
 if(model->elpRange[elpLMAX] != (long)ELPMAXLONG)
 fprintf(file, "LMAX = %g\n", (double)model->elpRange[elpLMAX]/(double)SCALE_X) ;    
 if(model->elpRange[elpWMIN] != (long)0)
 fprintf(file, "WMIN = %g\n", (double)model->elpRange[elpWMIN]/(double)SCALE_X) ;    
 if(model->elpRange[elpWMAX] != (long)ELPMAXLONG)
 fprintf(file, "WMAX = %g\n\n", (double)model->elpRange[elpWMAX]/(double)SCALE_X) ;    

 fprintf(file, "#shrink parameters (micron)\n") ;
 fprintf(file, "DL   = %g\n", model->elpShrink[elpDL]) ;    
 fprintf(file, "DW   = %g\n", model->elpShrink[elpDW]) ;    
 fprintf(file, "DLC  = %g\n", model->elpShrink[elpDLC]) ;    
 fprintf(file, "DWC  = %g\n", model->elpShrink[elpDWC]) ;    
 fprintf(file, "DWCJ = %g\n\n", model->elpShrink[elpDWCJ]) ;    

 fprintf(file, "#mult factor on dimension\n") ;
 fprintf(file, "LMLT = %g\n", model->elpShrink[elpLMLT]) ;    
 fprintf(file, "WMLT = %g\n\n", model->elpShrink[elpWMLT]) ;    

 fprintf(file, "#transistor characteristics\n") ;
 fprintf(file, "VT = %g\n", model->elpModel[elpVT]) ;    
 fprintf(file, "VT0 = %g\n", model->elpModel[elpVT0]) ;    
 fprintf(file, "KT = %g\n", model->elpModel[elpKT]) ;    
 fprintf(file, "A  = %g\n", model->elpModel[elpA]) ;    
 fprintf(file, "B  = %g\n", model->elpModel[elpB]) ;    
 fprintf(file, "RT  = %g\n", model->elpModel[elpRT]) ;    
 fprintf(file, "KRT = %g\n", model->elpModel[elpKRT]) ;    
 fprintf(file, "RS  = %g\n", model->elpModel[elpRS]) ;    
 fprintf(file, "KRS = %g\n", model->elpModel[elpKRS]) ;    
 fprintf(file, "KS = %g\n", model->elpModel[elpKS]) ;    
 fprintf(file, "KR = %g\n", model->elpModel[elpKR]) ;    
 if ( model->elpModel[elpSA] > ELPMINVALUE )
   fprintf(file, "SA = %g\n", model->elpModel[elpSA]);
 if ( model->elpModel[elpSB] > ELPMINVALUE )
   fprintf(file, "SB = %g\n", model->elpModel[elpSB]);
 if ( model->elpModel[elpSD] > ELPMINVALUE )
   fprintf(file, "SD = %g\n", model->elpModel[elpSD]);
 if ( model->elpModel[elpSC] > ELPMINVALUE )
   fprintf(file, "SC = %g\n", model->elpModel[elpSC]);
 if ( model->elpModel[elpSCA] > ELPMINVALUE )
   fprintf(file, "SCA = %g\n", model->elpModel[elpSCA]);
 if ( model->elpModel[elpSCB] > ELPMINVALUE )
   fprintf(file, "SCB = %g\n", model->elpModel[elpSCB]);
 if ( model->elpModel[elpSCC] > ELPMINVALUE )
   fprintf(file, "SCC = %g\n", model->elpModel[elpSCC]);
 if ( model->elpModel[elpNF] > ELPMINVALUE )
   fprintf(file, "NF = %g\n", model->elpModel[elpNF]);
 if ( model->elpModel[elpNRS] > ELPMINVALUE )
   fprintf(file, "NRS = %g\n", model->elpModel[elpNRS]);
 if ( model->elpModel[elpNRD] > ELPMINVALUE )
   fprintf(file, "NRD = %g\n", model->elpModel[elpNRD]);
 fprintf(file, "M      = %g\n", model->elpModel[elpM]) ;    
 fprintf(file, "MULU0  = %g\n", model->elpModel[elpMULU0]) ;    
 fprintf(file, "DELVT0 = %g\n\n", model->elpModel[elpDELVT0]) ;    

 fprintf(file, "#dynamic capacitance: grid capacitance (in pF/u and pF/u2)\n") ;
 fprintf(file, "CGS     = %g\n", model->elpCapa[elpCGS]) ;    
 fprintf(file, "CGS0    = %g\n", model->elpCapa[elpCGS0]) ;    
 fprintf(file, "CGSU    = %g\n", model->elpCapa[elpCGSU]) ;    
 fprintf(file, "CGSUF   = %g\n", model->elpCapa[elpCGSUF]) ;    
 fprintf(file, "CGSU0   = %g\n", model->elpCapa[elpCGSU0]) ;    
 fprintf(file, "CGSUMIN = %g\n", model->elpCapa[elpCGSUMIN]) ;
 fprintf(file, "CGSUMAX = %g\n", model->elpCapa[elpCGSUMAX]) ;
 fprintf(file, "CGSD    = %g\n", model->elpCapa[elpCGSD]) ;    
 fprintf(file, "CGSDF   = %g\n", model->elpCapa[elpCGSDF]) ;    
 fprintf(file, "CGSD0   = %g\n", model->elpCapa[elpCGSD0]) ;    
 fprintf(file, "CGSDMIN = %g\n", model->elpCapa[elpCGSDMIN]) ;    
 fprintf(file, "CGSDMAX = %g\n", model->elpCapa[elpCGSDMAX]) ;    
 fprintf(file, "CGP     = %g\n", model->elpCapa[elpCGP]) ;    
 fprintf(file, "CGPUMIN = %g\n", model->elpCapa[elpCGPUMIN]) ;    
 fprintf(file, "CGPUMAX = %g\n", model->elpCapa[elpCGPUMAX]) ;    
 fprintf(file, "CGPDMIN = %g\n", model->elpCapa[elpCGPDMIN]) ;    
 fprintf(file, "CGPDMAX = %g\n", model->elpCapa[elpCGPDMAX]) ;    
 fprintf(file, "CGD     = %g\n", model->elpCapa[elpCGD]) ;    
 fprintf(file, "CGD0    = %g\n", model->elpCapa[elpCGD0]) ;    
 fprintf(file, "CGD1    = %g\n", model->elpCapa[elpCGD1]) ;    
 fprintf(file, "CGD2    = %g\n", model->elpCapa[elpCGD2]) ;    
 fprintf(file, "CGDC    = %g\n", model->elpCapa[elpCGDC]) ;    
 fprintf(file, "CGDC0   = %g\n", model->elpCapa[elpCGDC0]) ;    
 fprintf(file, "CGDC1   = %g\n", model->elpCapa[elpCGDC1]) ;    
 fprintf(file, "CGDC2   = %g\n", model->elpCapa[elpCGDC2]) ;    
 fprintf(file, "CGSI    = %g\n", model->elpCapa[elpCGSI]) ;    
 fprintf(file, "CGSIC   = %g\n\n", model->elpCapa[elpCGSIC]) ;    
 
 fprintf(file, "#dynamic capacitance: drain capacitance (in pF/u and pF/u2)\n") ;
 fprintf(file, "CDS  = %g\n", model->elpCapa[elpCDS]) ;    
 fprintf(file, "CDSU = %g\n", model->elpCapa[elpCDSU]) ;    
 fprintf(file, "CDSD = %g\n", model->elpCapa[elpCDSD]) ;    
 fprintf(file, "CDP  = %g\n", model->elpCapa[elpCDP]) ;    
 fprintf(file, "CDPU = %g\n", model->elpCapa[elpCDPU]) ;    
 fprintf(file, "CDPD = %g\n", model->elpCapa[elpCDPD]) ;    
 fprintf(file, "CDW  = %g\n", model->elpCapa[elpCDW]) ;    
 fprintf(file, "CDWU = %g\n", model->elpCapa[elpCDWU]) ;    
 fprintf(file, "CDWD = %g\n", model->elpCapa[elpCDWD]) ;    

 fprintf(file, "#dynamic capacitance: source capacitance (in pF/u and pF/u2)\n") ;
 fprintf(file, "CSS  = %g\n", model->elpCapa[elpCSS]) ;    
 fprintf(file, "CSSU = %g\n", model->elpCapa[elpCSSU]) ;    
 fprintf(file, "CSSD = %g\n", model->elpCapa[elpCSSD]) ;    
 fprintf(file, "CSP  = %g\n", model->elpCapa[elpCSP]) ;    
 fprintf(file, "CSPU = %g\n", model->elpCapa[elpCSPU]) ;    
 fprintf(file, "CSPD = %g\n", model->elpCapa[elpCSPD]) ;    
 fprintf(file, "CSW  = %g\n", model->elpCapa[elpCSW]) ;    
 fprintf(file, "CSWU = %g\n", model->elpCapa[elpCSWU]) ;    
 fprintf(file, "CSWD = %g\n\n", model->elpCapa[elpCSWD]) ;    

 fprintf(file, "#access resistance (in ohm)\n");
 fprintf(file, "RACCS = %g\n", model->elpRacc[elpRACCS]) ;
 fprintf(file, "RACCD = %g\n", model->elpRacc[elpRACCD]) ;

 if(model->elpTransIndex == elpNOINDEX)
   fprintf(file, "END %s\n\n",model->elpModelName) ;
 else
   fprintf(file, "END %s [%d]\n\n",model->elpModelName, model->elpTransIndex) ;
}

/*******************************************************************************
* function elp_getlotrsalim                                                    *
*******************************************************************************/
double elpCalcIleakage( lotrs_list *lotrs,
                        int lotrstype, 
                        int transcase, 
                        double vgs, double vds, double vbs,
                        double *BLeak, double *DLeak, double *SLeak
                        ) 
{
  double vdd;
  long xs_s,ps_s,xd_s,pd_s;
  double AD,PD,AS,PS;
  double Ileak=0.0,w;
  elpmodel_list *model ;
  elp_lotrs_param *ptlotrs_param;

  vdd = elpGetVddFromCorner ( lotrs, transcase );
  model = elpGetModel(lotrs,vdd,transcase) ;
  if (!model) return 0.0;

  elpLotrsGetShrinkDim(lotrs,NULL,NULL,
                       &xs_s,&xd_s,&ps_s,&pd_s,
                       NULL,NULL, transcase);

  ptlotrs_param = elp_lotrs_param_create (lotrs);

  if(lotrs->WIDTH == 0)
     w = ((double)lotrs->PS / (double)4.0) ;
  else 
     w = (double)elpGetShrinkedWidth(lotrs,model);

  AD = (double)xd_s*w / (double)(SCALE_X*SCALE_X);
  AS = (double)xs_s*w / (double)(SCALE_X*SCALE_X);
  PD = pd_s / (double)SCALE_X;
  PS = ps_s / (double)SCALE_X;

  Ileak = elpCalcIleak(getlotrsmodel(lotrs), lotrstype, 
                       (double)lotrs->LENGTH/(double)(SCALE_X),
                       (double)lotrs->WIDTH/(double)(SCALE_X),
                       lotrs, transcase, 
                       vgs, vds, vbs,
                       AD,PD,AS,PS,
                       BLeak,DLeak,SLeak,
                       ptlotrs_param) ;

  elp_lotrs_param_free (ptlotrs_param);
  
  return Ileak;
}

int elpHack_GetNodeLocon( locon_list *locon )
{
  int node ;

  if( !locon->SIG->PRCN )
    addlorcnet( locon->SIG );

  node = -1 ;
  if( locon->PNODE )
    node = locon->PNODE->DATA ;
  
  if( node < 0 ) {
    if( locon->SIG->PRCN->PWIRE ) 
      node = locon->SIG->PRCN->PWIRE->NODE1 ;
  }

  if( node < 0 ) {
    if( locon->SIG->PRCN->PCTC ) 
      node = rcn_ctcnode( (loctc_list*)(locon->SIG->PRCN->PCTC->DATA), locon->SIG );
  }
  
  if( node < 0 ) {
    node = 1 ;
    setloconnode( locon, node );
  }

  return node ;
}

void elpHack_AddCapa( lotrs_list *lotrs, losig_list *vss, char where, float capa )
{
  locon_list *locon ;
  int         node ;

  switch( where ) {
  case 'g' :
    locon = lotrs->GRID ;
    break ;
  case 's' :
    locon = lotrs->SOURCE ;
    break ;
  case 'd' :
    locon = lotrs->DRAIN ;
    break ;
  }

  if( locon->SIG == vss )
    return ;

  node = elpHack_GetNodeLocon( locon );

  addloctc( locon->SIG, node, vss, 1, capa*V_FLOAT_TAB[__AVT_HACK_COEF_CGND].VALUE );
}

void elpHack_AddCtcCapa( locon_list *locon1, locon_list *locon2, float capa )
{
  int         node1 ;
  int         node2 ;

  if( !locon1->SIG->PRCN )
    addlorcnet( locon1->SIG );

  node1 = elpHack_GetNodeLocon( locon1 );
  node2 = elpHack_GetNodeLocon( locon2 );

  addloctc( locon1->SIG, node1, locon2->SIG, node2, capa*V_FLOAT_TAB[__AVT_HACK_COEF_CCTK].VALUE );
}

void elpHackNetlistResi( lofig_list *lofig )
{
  losig_list *losig ;
  locon_list *locon ;
  chain_list *chain ;
  ptype_list *ptl ;
  loctc_list *loctc ;
  
  for( losig = lofig->LOSIG ; losig ; losig = losig->NEXT ) {
  
    if( losig->PRCN ) {
    
      freetable( losig );
     
      /* remove all resistance */
      while( losig->PRCN->PWIRE )
        dellowire( losig, losig->PRCN->PWIRE->NODE1, losig->PRCN->PWIRE->NODE2 );
      
      /* set all locon to node 1 */
      ptl = getptype( losig->USER, LOFIGCHAIN );
      if( ptl ) {
        for( chain = (chain_list*)ptl->DATA ; chain ; chain = chain->NEXT ) {
          locon = (locon_list*)chain->DATA ;
          while( locon->PNODE ) 
            delloconnode( locon, locon->PNODE->DATA );
          freenum( locon->PNODE ); 
          setloconnode( locon, 1 );
        }
      }

      /* set all capacitance node to 1 */
      for( chain = losig->PRCN->PCTC ; chain ; chain = chain->NEXT ) {
        loctc = (loctc_list*)chain->DATA ;
        rcn_ctcnode_set( loctc, losig, 1 );
      }
    }
  }
}

void elpHackNetlistCapa( lofig_list *hackedlofig, int whatjob )
{
  losig_list    *sig_vss ;
  losig_list    *sig_vss_backup ;
  losig_list    *sig_vdd ;
  losig_list    *sig_vdd_backup ;
  losig_list    *sig_supply ;
  losig_list    *losig ;
  lotrs_list    *lotrs ;
  float          vdd ;
  elpmodel_list *elpmodel ;
  long           la ;
  long           wa ;
  float          cs ;
  float          cd ;
  float          cgi ;
  float          cdi ;
  float          csi ;
  float          cgs ;
  float          cgd ;
  ptype_list    *ptl ;

  sig_vss = NULL ;
  sig_vdd = NULL ;
  sig_vss_backup = NULL ;
  sig_vdd_backup = NULL ;

  for( losig = hackedlofig->LOSIG ; losig ; losig = losig->NEXT ) {
    if( mbk_LosigIsVSS( losig ) ) {
      if( losig->PRCN && losig->PRCN->PCTC )
        sig_vss = losig ;
      else {
        ptl = getptype( losig->USER, LOFIGCHAIN );
        if( ptl && ptl->DATA )
          sig_vss_backup = losig ;
      }
    }
    if( mbk_LosigIsVDD( losig ) ) {
      if( losig->PRCN && losig->PRCN->PCTC )
        sig_vdd = losig ;
      else {
        ptl = getptype( losig->USER, LOFIGCHAIN );
        if( ptl && ptl->DATA )
          sig_vdd_backup = losig ;
      }
    }
  }

  if( !sig_vss )
    sig_vss = sig_vss_backup ;
  if( !sig_vdd )
    sig_vdd = sig_vdd_backup ;
    
  if( !sig_vss || !sig_vdd ) {
    printf( "can't find supply signal !\n" );
    exit(0);
  }
  
  for( lotrs = hackedlofig->LOTRS ; lotrs ; lotrs = lotrs->NEXT ) {

    if( MLO_IS_TRANSN( lotrs->TYPE ) )
      sig_supply = sig_vss ;
    else
      sig_supply = sig_vdd ;
      
    vdd = elpGetVddFromCorner( lotrs, elpTYPICAL ) ;
    elpmodel = elpGetModel( lotrs, vdd, elpTYPICAL ) ;

    cs = elpLotrsCapaSource( lotrs, ELP_CAPA_TYPICAL, elpTYPICAL );
    cd = elpLotrsCapaDrain( lotrs, ELP_CAPA_TYPICAL, elpTYPICAL );

    la = elpShrinkSize( elpGetShrinkedLength( lotrs, elpmodel ), -elpmodel->elpShrink[elpDL], 1.0 ) ;
    la = elpShrinkSize( la, elpmodel->elpShrink[elpDLC], 1.0 ) ;
    wa = elpShrinkSize( elpGetShrinkedWidth( lotrs, elpmodel ), -elpmodel->elpShrink[elpDW], 1.0 ) ;
    wa = elpShrinkSize( wa, elpmodel->elpShrink[elpDWC], 1.0 ) ;
    
    cdi = elpmodel->elpCapa[ elpCGD ]  * ((double)wa) * ((double)la) / ((double)(SCALE_X*SCALE_X)) ;
    csi = elpmodel->elpCapa[ elpCGSI ] * ((double)wa) * ((double)la) / ((double)(SCALE_X*SCALE_X)) ;
    cgi = elpmodel->elpCapa[ elpCGS ]  * ((double)wa) * ((double)la) / ((double)(SCALE_X*SCALE_X)) ;
    cgs = elpmodel->elpCapa[ elpCGP ]  * ((double)wa) / ((double)SCALE_X) ;
    cgd = elpmodel->elpCapa[ elpCGP ]  * ((double)wa) / ((double)SCALE_X) ;
    
    elpHack_AddCapa( lotrs, sig_supply, 's', cs-cgs  );
    elpHack_AddCapa( lotrs, sig_supply, 'd', cd-cgd  );

    elpHack_AddCapa( lotrs, sig_supply, 'g', cgi );
   
    if( whatjob == ELP_HACK_FIX_CAPA_TO_GND ) {

      elpHack_AddCapa( lotrs, sig_supply, 's', cgs+csi );
      elpHack_AddCapa( lotrs, sig_supply, 'g', cgs+csi );
    
      elpHack_AddCapa( lotrs, sig_supply, 'd', cgd+cdi );
      elpHack_AddCapa( lotrs, sig_supply, 'g', cgd+cdi );
    }
    else {
    
      if( lotrs->GRID != lotrs->DRAIN )
        elpHack_AddCtcCapa( lotrs->GRID, lotrs->DRAIN,  cgd+cdi );
        
      if( lotrs->GRID != lotrs->SOURCE )
        elpHack_AddCtcCapa( lotrs->GRID, lotrs->SOURCE, cgs+csi );
    }
  }
}

