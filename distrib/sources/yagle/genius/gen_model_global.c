/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_global.c                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 09/02/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include MSL_H
#include FCL_H
#include API_H
#include AVT_H
#include "gen_env.h"
#include "gen_display.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_model_GENIUS.h"
#include "gen_model_FCL.h"
#include "gen_model_global.h"
#include "gen_symmetric.h"

static lofig_list *FIG=NULL;                 /*current lofig in construction*/

int isignoredlocon(char *name, chain_list *unu);
extern char *modelradical(char *name);

int ispragmacomment(char *txt, const char *code)
{
  unsigned int i;
  for (i=2;i<strlen(txt) && txt[i]==' ';i++) ;
  if (i>=strlen(txt)) return 0;
  if (strncmp(&txt[i],"pragma",6)!=0) return 0;
  i+=6;
  for (;i<strlen(txt) && txt[i]==' ';i++) ;
  if (i>=strlen(txt)) return 0;
  if (strncmp(&txt[i],code,strlen(code))!=0) return 0;
  return 1;
}


/****************************************************************************/
/*        for all structure field in file, attach it to its own figure      */
/****************************************************************************/
//extern ht *architomodel;

static void markforcematch(lofig_list *lf, mbk_match_rules *mr)
{
  losig_list *ls;
  loins_list *li;
  lotrs_list *tr;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    if (mbk_CheckREGEX(mr, getsigname(ls)))      
      ls->USER=addptype(ls->USER, GNS_FORCE_MATCH, NULL);

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    if (mbk_CheckREGEX(mr, li->INSNAME))      
      li->USER=addptype(li->USER, GNS_FORCE_MATCH, NULL);

  for (tr=lf->LOTRS; tr!=NULL; tr=tr->NEXT)
    if (mbk_CheckREGEX(mr, tr->TRNAME))      
      tr->USER=addptype(tr->USER, GNS_FORCE_MATCH, NULL);
}

extern void Get_All_Loins(tree_list *tree, chain_list *fcl)
{
  chain_list *p;
  model_list *model;
  char *modelname;

  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 20);
    //fprintf(stderr,"Get_All_Loins: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
    /*node */
  case GEN_TOKEN_NODE:
    Get_All_Loins(tree->DATA,fcl);
    break;
    /*search description of model*/
  case ';':
    Get_All_Loins(tree->NEXT,fcl);
    Get_All_Loins(tree->NEXT->NEXT,fcl);
    break;
  case GEN_TOKEN_ACTION:
    break;
    /*ignore some branches*/
  case GEN_TOKEN_ENTITY:  
    modelname=getname(tree->NEXT); 
    model=getmodel(modelname);                               
    if (!model) {
      avt_errmsg(GNS_ERRMSG, "045", AVT_FATAL, modelname);
      //fprintf(stderr,"Get_All_Loins: no model %s found\n",modelname);
      EXIT(1);
    }
    /*    else
          printf("no act for %s\n",modelname);*/
    break;
    /*beginnig of real treatment*/
  case GEN_TOKEN_ARCHITECTURE:
    modelname=getname_of(tree->NEXT); 
    model=getmodel(modelname);
    if (!model) {
      avt_errmsg(GNS_ERRMSG, "045", AVT_FATAL, modelname);
      //fprintf(stderr,"Get_All_Loins: no model %s found\n",modelname);
      EXIT(1);
    }

    FIG=model->LOFIG;                                       /*init var glob.*/ 
    /* Is it a model for FCL-->standard Alliance  or  for Genius-->generic*/
    for (p=fcl;p;p=p->NEXT) if (p->DATA==modelname) break;
    if (p) {
      Env_Alliance(FIG);
      /*no need to get signals now by Get_Signals_Alliance(), if you put an*/
      /*unused signal in lofig, FCL will not recognized the rule. So we */
      /*put it when it's used in loins due to Get_Losig_Alliance() */
      model->GENERIC=Get_Loins_Alliance(tree->NEXT->NEXT->NEXT);
    }
    else 
      {
        misc_stuffs *ms;
        ms=(misc_stuffs *)tree->NEXT->DATA->NEXT->NEXT->NEXT->NEXT->DATA->DATA;
        Env_Generic(FIG);
        Get_Signals_Generic(tree->NEXT->NEXT);
        model->GENERIC=Get_Loins_Generic(tree->NEXT->NEXT->NEXT, tree->NEXT);
        if (ms!=NULL)
          {
            verif_instance_flags(ms->EXCLUDES, FILE_NAME(tree), "exclude");
            verif_instance_flags(ms->WITHOUTS, FILE_NAME(tree), "non existant");
 
            if (ms->FORCEMATCH!=NULL)
            {
              ptype_list *pt;
              mbk_match_rules mr;
              mbk_CreateREGEX(&mr, CASE_SENSITIVE, 1);
              for (pt=ms->FORCEMATCH; pt!=NULL; pt=pt->NEXT)
                mbk_AddREGEX(&mr, (char *)pt->DATA);
              markforcematch(FIG, &mr);
              mbk_FreeREGEX(&mr);
            }
          }
        if (CheckModelInstances(model)) Inc_Error();
      }


    break;
  default:
    Error_Tree("Get_All_Loins",tree);
    EXIT(1);
  }
}

/* ---------------- */

/* ---------------- */

void verifySyms(lofig_list *lf)
{
  ptype_list *syms;
  chain_list *ch;
  locon_list *lc;
  int total, vecto;
  char temp[200], *r;
  SymInfoItem *sii;

  syms=getptype(lf->USER,GEN_SYM_INFO_PTYPE);
  if (syms==NULL) //EXIT(2); // internal error
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 21);

  for (ch=(chain_list *)syms->DATA;ch!=NULL;ch=ch->NEXT)
    {
      gen_printf(1,"for %s, Syms :",lf->NAME);
      total=0; vecto=0;
      for (sii=(SymInfoItem *)ch->DATA;sii!=NULL;sii=sii->NEXT)
        {
          for (lc=lf->LOCON;lc!=NULL && vectorradical(lc->NAME)!=sii->ConnectorName;lc=lc->NEXT) ;
          if (lc==NULL)
            {
              sprintf(temp,"%s",sii->ConnectorName);
              if ((r=strchr(temp,'('))!=0)
                {
                  vecto++;
                  *r='\0';
                  sii->ConnectorName=namealloc(temp);
                  sii->ConnectorRadical=vectorradical(sii->ConnectorName);
                  sii->FLAGS=1;
                  for (lc=lf->LOCON;lc!=NULL && strcmp(vectorradical(lc->NAME),sii->ConnectorRadical)!=0;lc=lc->NEXT) ;         
                }
            }
          if (lc==NULL) 
            {
              avt_errmsg(GNS_ERRMSG, "046", AVT_FATAL, sii->ConnectorName);
              //fprintf(stderr,"verifySyms: unknown connector (%s) declared in symmetric connector list\n",sii->ConnectorName);
              EXIT(1);
            }
          if (sii->FLAGS==0)
            gen_printf(1," %s",lc->NAME);
          else
            gen_printf(1," %s(i)",lc->NAME);
          total++;
        }
      gen_printf(1,"\n");
      if (vecto>0 && total!=vecto)
        {
          avt_errmsg(GNS_ERRMSG, "047", AVT_FATAL);
          //fprintf(stderr,"verifySyms: mix of vector and bit\n");
          EXIT(1);
        }
    }
}

void verifyCoupled(lofig_list *lf)
{
  ptype_list *syms;
  chain_list *ch;
  locon_list *lc;
  int total, vecto;
  char temp[200], *r;
  SymInfoItem *sii;

  syms=getptype(lf->USER,GEN_COUPLED_INFO_PTYPE);
  if (syms==NULL) //EXIT(2); // internal error
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 22);

  for (ch=(chain_list *)syms->DATA;ch!=NULL;ch=ch->NEXT)
    {
      gen_printf(1,"for %s, Coupled :",lf->NAME);
      total=0; vecto=0;
      for (sii=(SymInfoItem *)ch->DATA;sii!=NULL;sii=sii->NEXT)
        {
          for (lc=lf->LOCON;lc!=NULL && vectorradical(lc->NAME)!=sii->ConnectorName;lc=lc->NEXT) ;
          if (lc==NULL)
            {
              sprintf(temp,"%s",sii->ConnectorName);
              if ((r=strchr(temp,'('))!=0)
                {
                  vecto++;
                  *r='\0';
                  sii->ConnectorName=namealloc(temp);
                  sii->ConnectorRadical=vectorradical(sii->ConnectorName);
                  sii->FLAGS=1;
                  for (lc=lf->LOCON;lc!=NULL && strcmp(vectorradical(lc->NAME),sii->ConnectorRadical)!=0;lc=lc->NEXT) ;         
                }
            }
          if (lc==NULL) 
            {
              avt_errmsg(GNS_ERRMSG, "048", AVT_FATAL, sii->ConnectorName);
              //fprintf(stderr,"verifyCoupled: unknown connector (%s) declared in coupled connector list\n",sii->ConnectorName);
              EXIT(1);
            }
          if (sii->FLAGS==0)
            gen_printf(1," %s",lc->NAME);
          else
            gen_printf(1," %s(i)",lc->NAME);
          total++;
        }
      gen_printf(1,"\n");
      /* if (vecto>0 && total!=vecto)
         {
         fprintf(stderr,"verifyCoupled: experimental mode used : 'mix of bit and vector in coupling'\n");
         }
      */
    }
}

static void markunusedlocons(lofig_list *lf, chain_list *unu)
{
  locon_list *lc;
  for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
    if (isignoredlocon(lc->NAME, unu))
      {
        lc->SIG->USER=addptype(lc->SIG->USER, GEN_UNUSED_FLAG_PTYPE, NULL);
      }
}

int gns_isunusedsig(losig_list *ls)
{
  if (getptype(ls->USER, GEN_UNUSED_FLAG_PTYPE)) return 1;
  return 0;
}
/****************************************************************************/
/*                  build all interfaces in file                            */
/****************************************************************************/
extern void Get_All_Lofig(tree_list *tree,chain_list *fcl)
{
  chain_list *p;
  char *modelname;                                         
  chain_list *variables;
  tree_list *symtree;

  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 23);
    //    fprintf(stderr,"Get_All_Lofig: NULL pointer\n");
    EXIT(1);
  }
  switch (TOKEN(tree)) {
    /*node */
  case GEN_TOKEN_NODE:
    Get_All_Lofig(tree->DATA,fcl);
    break;
  case ';':
    Get_All_Lofig(tree->NEXT,fcl);
    Get_All_Lofig(tree->NEXT->NEXT,fcl);       
    break;
    /*ignore some branches*/
  case GEN_TOKEN_ARCHITECTURE: case GEN_TOKEN_ACTION:
    break;
    /*begining of real treatment*/
  case GEN_TOKEN_ENTITY:
    {
      chain_list *cl;
      lib_entry *le, *mainle=NULL;

      modelname=getname(tree->NEXT); 
      for (le=GENIUS_PRIORITY;le!=NULL;le=le->NEXT) 
        {
          if (le->entity==modelradical(modelname)) mainle=le;
          if (le->entity==modelname) break;
        }

      // le is NULL for the "other architectures"
      if (le==NULL || le->format==0)
        {
          // GENIUS/FCL vhdl
          symtree=tree->NEXT->NEXT->NEXT->NEXT; // zinaps was here
          //-----
          FIG=addlofig(modelname);                              /*init var glob.*/

          /* Is it a model for fcl-->standard Alliance  or  for Genius-->generic*/
          for (p=fcl;p;p=p->NEXT) if (p->DATA==modelname) break;
          if (p) {
            Env_Alliance(FIG);
            Get_Port_Alliance(tree->NEXT->NEXT->NEXT);
            variables=NULL;
          }
          else {
            Env_Generic(FIG);
            Get_Port_Generic(tree->NEXT->NEXT->NEXT);
            variables=Get_Generic(tree->NEXT->NEXT,NULL);
          }   
          addmodel(FIG,variables,NULL,mainle==NULL||mainle->model==NULL?modelradical(modelname):mainle->model, mainle->match);
          // zinaps ---
          cl=(chain_list *)((tree_list *)symtree->DATA)->DATA;
          FIG->USER=addptype(FIG->USER, GEN_SYM_INFO_PTYPE, cl->DATA); // double chain_list, symmetric connectors
          FIG->USER=addptype(FIG->USER, GEN_COUPLED_INFO_PTYPE, cl->NEXT->DATA); // double chain_list, coupled connectors
          verifySyms(FIG);
          verifyCoupled(FIG);
          markunusedlocons(FIG, (chain_list *)cl->NEXT->NEXT->DATA);


        }
      // ---
    }
    break;
  default:
    Error_Tree("Get_All_Lofig",tree);
    EXIT(1);
  }
}


/****************************************************************************/
/*build the model data structure with the syntaxic tree from the parsed file*/
/****************************************************************************/
#define PRAGMA_NUMBER 8
static chain_list *pragmas[PRAGMA_NUMBER];

//symm, *coupled;

SymInfoItem *chain_to_sii(chain_list *cl)
{
  SymInfoItem *sii=NULL;
  chain_list *ch;

  for (ch=cl; ch!=NULL; ch=ch->NEXT)
    {
      sii=AddSymInfoItem(sii);
      sii->ConnectorName=(char *)ch->DATA;
      sii->ConnectorRadical=vectorradical((char *)ch->DATA);
      sii->ConnectorIndex=vectorindex((char *)ch->DATA);
      sii->FLAGS=0;
    }
  return sii;
}

const char *known_pragmas[]={"symmetric","coupled","exclude_at_end","without","unused","exclude","stop_at_power_supplies", "forcematch"};

int SymmetricExtension(char *tmp, int process)
{
  chain_list *ch=NULL;
  char line[1024];
  char *i,*i0;
  unsigned int j;
  int   handled = 0;

  strcpy(line,tmp);

  if (PRAGMA_NUMBER!=sizeof(known_pragmas)/sizeof(*known_pragmas)) exit(67);

  if (!process)
    {
      for (j=0; j<sizeof(known_pragmas)/sizeof(*known_pragmas); j++)
        if (ispragmacomment(line, known_pragmas[j])) return 1;
      return 0;
    }

  for (j=0; j<sizeof(known_pragmas)/sizeof(*known_pragmas); j++)
    if (ispragmacomment(line, known_pragmas[j]))
      {
        if ((i=strchr(line,'\n'))!=NULL) *i='\0';
        i=strstr(line,known_pragmas[j])+strlen(known_pragmas[j]);
 
        do
          {
            while (*i==' ' && *i!='\0') i++;
            if (*i!='\0') 
              {
                i0=strchr(i,' ');
                if (i0==NULL)        
                  ch=addchain(ch,namealloc(i));
                else {
                  *i0='\0';
                  ch=addchain(ch,namealloc(i));
                  i=i0+1;
                }
              }
          } while (*i!='\0' && i0!=NULL);
        if (j<=1)
          {
            pragmas[j]=addchain(pragmas[j],chain_to_sii(ch));
            freechain(ch);
          }
        else
          {
            pragmas[j]=append(ch, pragmas[j]);
          }
        break;
      }
  return 0;
}

tree_list *bin_tree_locon(locon_list *lc)
{
  tree_list *temp,*tempnext;
  locon_list *next;
  
  if (lc==NULL) return NULL;
  next=lc->NEXT;
  if ((GEN_OPTIONS_PACK & GEN_STRICT)==0)
    {
      while (next!=NULL && vectorradical(next->NAME)==vectorradical(lc->NAME)) next=next->NEXT;
    }
  tempnext=bin_tree_locon(next);

  temp=PutAto_Tree(GEN_TOKEN_IDENT,"spice",0,(void *)lc->NAME);
  temp=PutUni_Tree(GEN_TOKEN_LINKAGE,"spice",0,temp);
  temp=PutUni_Tree(GEN_TOKEN_SIGNAL,"spice",0,temp);
  
  if (tempnext==NULL) return temp;
    
  return PutBin_Tree(',',"spice",0,tempnext,temp);
}

tree_list *build_entity_tree(lofig_list *lf)
{
  tree_list *t;

  t=bin_tree_locon(lf->LOCON);
  t=PutUni_Tree(GEN_TOKEN_PORT,"spice",0,t);
  t=PutQua_Tree(
                GEN_TOKEN_ENTITY,
                "spice",
                0,
                PutAto_Tree(GEN_TOKEN_IDENT,"spice",0,lf->NAME), // nom
                PutAto_Tree(GEN_TOKEN_NOP,"spice",0,0), // generic
                t, // ports
                PutAto_Tree(GEN_TOKEN_PRAGMA_SYM,"spice",0,NULL) // chain of chain?
                );
  return t;
}

#if 0
static char *newname(char *orig, chain_list *defines)
{
  chain_list *cl;
  api_define_type *adt;
  for (cl=defines; cl!=NULL; cl=cl->NEXT)
    {
      adt=(api_define_type *)cl->DATA;
      if (strcasecmp(adt->ORIG, orig)==0) return namealloc(adt->DEST);
    }
  return orig;
}

static void GNSPostprocess(lofig_list *lf, chain_list *defines)
{
  losig_list *ls;
  loins_list *li;
  lotrs_list *lt;
  chain_list *cl;

  if (defines==NULL) return;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      for (cl=ls->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
        cl->DATA=newname((char *)cl->DATA, defines);
    }
  
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      for (cl=ls->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
        cl->DATA=newname((char *)cl->DATA, defines);
    }
  
  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      li->INSNAME=newname(li->INSNAME, defines);
      li->FIGNAME=newname(li->FIGNAME, defines);
    }

  for (lt=lf->LOTRS; lt!=NULL; lt=lt->NEXT)
    {
      lt->TRNAME=newname(lt->TRNAME, defines);
    }

  lf->NAME=newname(lf->NAME, defines);
}
#endif

static tree_list *spice_to_geniushr(lofig_list *lf)
{
  chain_list *cl, *p, *fd;
  lotrs_list *tr;
  loins_list *li;
  lofig_list *mlf;
  locon_list *lc, *lcm;
  long flags;
  tree_list *tree, *stree;
  char *file="spice";
  int lineno=0;
  ptype_list *ps;
  losig_list *ls;
  generic_map_info *gmi;

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      li->LOCON=(locon_list *)reverse((chain_list *)li->LOCON);
    }
  for (tr=lf->LOTRS; tr!=NULL; tr=tr->NEXT)
    {
      cl=addchain(NULL, tr->BULK->SIG);
      cl=addchain(cl, tr->DRAIN->SIG);
      cl=addchain(cl, tr->SOURCE->SIG);
      cl=addchain(cl, tr->GRID->SIG);
      mlf=getloadedlofig(getlotrsmodel(tr));
      if (mlf==NULL)
        avt_errmsg(GNS_ERRMSG, "168", AVT_FATAL, getlotrsmodel(tr));
      li=addloins(lf, tr->TRNAME, mlf, cl);

      freechain(cl);
      gmi=(generic_map_info *)mbkalloc(sizeof(generic_map_info));
      gmi->left=transistor_params[0]; // l
      gmi->type='v';
      gmi->right.value=((double)tr->LENGTH/SCALE_X)*1e-6;
      gmi->FILE="spice";
      gmi->LINE=0;
      cl=addchain(NULL, gmi);
      gmi=(generic_map_info *)mbkalloc(sizeof(generic_map_info));
      gmi->left=transistor_params[1]; // w
      gmi->type='v';
      gmi->right.value=((double)tr->WIDTH/SCALE_X)*1e-6;
      gmi->FILE="spice";
      gmi->LINE=0;
      cl=addchain(cl, gmi);
      li->USER=addptype(li->USER, GEN_GENERIC_MAP_PTYPE, cl);
    }
  
  while (lf->LOTRS!=NULL) dellotrs(lf, lf->LOTRS);

  lofigchain(lf);
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      ls->USER=addptype(ls->USER, GEN_REAL_RANGE_PTYPE, NULL);
      freelorcnet(ls);
    }
  
  tree=NULL;
  for (lc=lf->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      ps=getptype(lc->SIG->USER,GENSIGCHAIN);  
      if (!ps) ps=lc->SIG->USER=addptype(lc->SIG->USER,GENSIGCHAIN,NULL);
      ps->DATA=addgensigchain(ps->DATA,PUT_ATO(GEN_TOKEN_DIGIT,-1),lc, PUT_ATO(GEN_TOKEN_DIGIT,-1));      
    }

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      mlf=getloadedlofig(li->FIGNAME);
      for (lc=li->LOCON, lcm=mlf->LOCON; lc!=NULL && lcm!=NULL; lc=lc->NEXT, lcm=lcm->NEXT)
        {
          if (gns_isunusedsig(lcm->SIG))
            if (!gns_isunusedsig(lc->SIG))
              {
                lc->SIG->USER=addptype(lc->SIG->USER, GEN_UNUSED_FLAG_PTYPE, NULL);
              }
          
          //          for (p=ms->STOP_POWER; p!=NULL && p->DATA!=getsigname(lc->SIG); p=p->NEXT) ;
          if ((fd=getchain(pragmas[6], getsigname(lc->SIG)))!=NULL)
            {
              if (getptype(lc->SIG->USER, GEN_STOP_POWER_FLAG_PTYPE)==NULL)
                lc->SIG->USER=addptype(lc->SIG->USER, GEN_STOP_POWER_FLAG_PTYPE, NULL);
              fd->DATA=NULL;
            }
          
          lc->USER=addptype(lc->USER,GENCONCHAIN,NULL);
          lc->USER->DATA=addgenconchain(NULL, 
                                        PutAto_Tree(GEN_TOKEN_DIGIT,"spice",0,(void *)-1),
                                        lc->SIG,
                                        PutAto_Tree(GEN_TOKEN_DIGIT,"spice",0,(void *)-1)
                                        );
          ps=getptype(lc->SIG->USER,GENSIGCHAIN);  
          if (!ps) ps=lc->SIG->USER=addptype(lc->SIG->USER,GENSIGCHAIN,NULL);
          ps->DATA=addgensigchain(ps->DATA,PUT_ATO(GEN_TOKEN_DIGIT,-1),lc, PUT_ATO(GEN_TOKEN_DIGIT,-1));

        }
      
      flags=0;
      if ((fd=getchain(pragmas[5], li->INSNAME))!=NULL) flags|=LOINS_IS_EXCLUDE, fd->DATA=NULL;
      if ((fd=getchain(pragmas[3], li->INSNAME))!=NULL) flags|=LOINS_IS_WITHOUT, fd->DATA=NULL;
      if ((fd=getchain(pragmas[2], li->INSNAME))!=NULL) flags|=LOINS_IS_EXCLUDE_AT_END, fd->DATA=NULL;
      
      li->USER=addptype(li->USER, GENIUS_LOINS_FLAGS_PTYPE, (void *)flags);
//      printf("%s %s %ld\n",li->FIGNAME, li->INSNAME, flags);
      /*stree=PUT_TRI(GEN_TOKEN_MAP,
                    PUT_QUA(GEN_TOKEN_OF,
                            PUT_ATO(GEN_TOKEN_IDENT,li->INSNAME),
                            PUT_ATO(GEN_TOKEN_IDENT,li->FIGNAME),
                            PUT_ATO(GEN_TOKEN_VOIDTOKEN,NULL),
                            PUT_ATO(GEN_TOKEN_VOIDTOKEN,NULL)),
                    NULL,
                    PUT_ATO(GEN_TOKEN_GENERIC,NULL)
                    );*/
      
      if (getptype(li->USER, GEN_GENERIC_MAP_PTYPE)==NULL)
        li->USER=addptype(li->USER, GEN_GENERIC_MAP_PTYPE, NULL/*((tree_list *)stree->NEXT->NEXT->NEXT->DATA)->DATA*/); // (<model_var>,<circuit_expr>)+ single chain list
      stree=PUT_ATO(GEN_TOKEN_LOINS,li);
      li->USER=addptype(li->USER,GENERIC_PTYPE,Duplicate_Tree(stree));
      if (tree!=NULL)
        tree=PUT_BIN(';',stree,tree);
      else
        tree=stree;



    }
  return tree;
}

void LoadSpiceFCLFiles(lib_entry *le)
{
  lofig_list *lf;
  FILE *f;
  locon_list *lc;
  int i;
  tree_list *tree;
  chain_list *cl;

  for (;le!=NULL;le=le->NEXT)
    {
      if (le->format!=0)
        {
          // FCL
          char *oldconfig;
          mbkContext *ctx;

          oldconfig=V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE;
          V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE="All";

          if ((f = APIFindFile(le))!=NULL) fclose(f);
      
          ctx=mbkCreateContext();
          mbkSwitchContext(ctx);

          for (i=0; i<PRAGMA_NUMBER; i++)
            pragmas[i]=NULL;
          //          coupled=symm=NULL;
          gen_printf(0, "Opening spice model file %s....\n",le->name);
          fclExtendSpice();
          mslAddExtension(SymmetricExtension);
      
          spiceloadlofig(NULL, le->name, 'A');

          mslRmvExtension(SymmetricExtension);
          fclRmvSpiceExtensions();

          V_STR_TAB[__MBK_SPICE_KEEP_NAMES].VALUE=oldconfig;

          if (le->subfigurename!=NULL)
            {
              if ((FIG=getloadedlofig(namealloc(le->subfigurename)))==NULL)
                {
                  avt_errmsg(GNS_ERRMSG, "049", AVT_FATAL, le->subfigurename, le->name);
                  EXIT(1);
                }
            }
          else
            FIG=HEAD_LOFIG;

          if (FIG->LOINS!=NULL && le->format!=2)
            {
              char old_separ=SEPAR;
              SEPAR='_';
              gen_printf(0, "Flattening spice description of '%s'....\n",FIG->NAME);
              rflattenlofig(FIG, YES, NO);
              SEPAR=old_separ;
            }

          while (HEAD_LOFIG!=FIG || HEAD_LOFIG->NEXT!=NULL)
            {
              if (HEAD_LOFIG!=FIG) dellofig(HEAD_LOFIG->NAME);
              else dellofig(HEAD_LOFIG->NEXT->NAME);
            }

          mbkFreeAndSwitchContext(ctx);

          if (HT_LOFIG==NULL) HT_LOFIG=addht(128);
          addhtitem(HT_LOFIG, FIG->NAME, (long)FIG);
          FIG->NEXT=HEAD_LOFIG;
          HEAD_LOFIG=FIG;

          // check the lofig
          if (!mbk_checklofig(FIG))
            {
              EXIT(1);
            }

          Env_Alliance(FIG);
          if (le->format!=2 && !(FIG->LOINS==NULL && FIG->LOTRS==NULL) && (FIG->LOTRS==NULL || FIG->LOINS!=NULL))
            {
              avt_errmsg(GNS_ERRMSG, "050", AVT_FATAL, le->name);
              //fprintf(stderr, "Spice file %s should be a flat transistor netlist\n",le->name);
              EXIT(1);
            }
          if (le->format==2) tree=spice_to_geniushr(FIG);
          else tree=NULL;

          for (lc=FIG->LOCON;lc!=NULL;lc=lc->NEXT)
            {
              lc->USER=addptype(lc->USER,GENCONCHAIN,NULL);
              lc->USER->DATA=addgenconchain(NULL, 
                                            PutAto_Tree(GEN_TOKEN_DIGIT,"spice",0,(void *)-1),
                                            lc->SIG,
                                            PutAto_Tree(GEN_TOKEN_DIGIT,"spice",0,(void *)-1)
                                            );
            }
      
          FIG->LOCON=(locon_list *)reverse((chain_list *)FIG->LOCON);

          le->entity=FIG->NAME;
          addmodel(FIG,NULL,tree,le->model==NULL?FIG->NAME:le->model, le->match);

          
          if (pragmas[7]!=NULL)
            {
              mbk_match_rules mr;
              mbk_CreateREGEX(&mr, CASE_SENSITIVE, 1);
              for (cl=pragmas[7]; cl!=NULL; cl=cl->NEXT)
                mbk_AddREGEX(&mr, (char *)cl->DATA);
              markforcematch(FIG, &mr);
              mbk_FreeREGEX(&mr);
            }

          FIG->USER=addptype(FIG->USER, GEN_SYM_INFO_PTYPE, pragmas[0]); // double chain_list, symmetric connectors
          FIG->USER=addptype(FIG->USER, GEN_COUPLED_INFO_PTYPE, pragmas[1]); // double chain_list, coupled connectors
          verifySyms(FIG);
          verifyCoupled(FIG);
          markunusedlocons(FIG, pragmas[4]);

          for (i=2; i<PRAGMA_NUMBER; i++)
          {
            if (i!=7) verif_instance_flags_chainlist(pragmas[i], le->name, (char *)known_pragmas[i]);
            freechain(pragmas[i]);
          }

          le->tree=build_entity_tree(FIG);
        }
    }
}

void Build_All_Transistor_Models()
{
  chain_list *cl;
  // building all transistors
  for (cl=TNMOS;cl!=NULL;cl=cl->NEXT)
    {
      Build_Transistor((char *)cl->DATA);
    }
  for (cl=TPMOS;cl!=NULL;cl=cl->NEXT)
    {
      Build_Transistor((char *)cl->DATA);
    }
}

void Build_All_Models(tree_list *tree,chain_list *fcl)
{
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 24);
    //    fprintf(stderr,"Build_All_Models: NULL pointer\n");
    EXIT(1);
  }

  Get_All_Lofig(tree,fcl);
  Get_All_Loins(tree,fcl);
  
  if (Get_Error())
    {
      avt_errmsg(GNS_ERRMSG, "060", AVT_FATAL);
      //fprintf(stderr,"other errors follow...\n");
      EXIT(3);
    }
}



int isignoredlocon(char *name, chain_list *unu)
{
  chain_list *ch;
  SymInfoItem *sii;

  for (ch=unu;ch!=NULL;ch=ch->NEXT)
    {
      for (sii=(SymInfoItem *)ch->DATA;sii!=NULL;sii=sii->NEXT)
        {
          if (vectorradical(name)==sii->ConnectorName) return 1;
        }
    }
  return 0;
}

