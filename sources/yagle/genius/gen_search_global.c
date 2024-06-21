/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_global.c                                         */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 19/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <tcl.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include FCL_H
#include AVT_H
#include API_H
#include GEN_H
#include "gen_env.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_corresp.h"
#include "gen_lofig.h"
#include "gen_search_utils.h"
#include "gen_search_iterative.h"
#include "gen_search_global.h"
#include "gen_symmetric.h"
#include "gen_display.h"
#include "gen_globals.h"

extern void printchainstatus();
void gen_set_pnode_in_loins(chain_list *list);

/****************************************************************************/
/*               sort by dependancies the models and dump results           */
/****************************************************************************/
// zinaps

static int reject_model(lofig_list *lf, char *model)
{
  int cnt=0, without=0;
  loins_list *li;
  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {
      if (li->FIGNAME==model) 
        {
          cnt++;
          if (((long)getptype(li->USER, GENIUS_LOINS_FLAGS_PTYPE)->DATA & LOINS_IS_WITHOUT)!=0)
            without=1;
        }
    }
  if (cnt==1 && without) return 1;
  return 0;
}

static void Sort_Search(lofig_list *circuit, model_list *model, chain_list* namechain)
{
  model_list *model_bis;
  int select=0,count,selecthassyms=1,hassyms=0, count0, nosearch=0;
  char *select_model=NULL,*name_ins;
  chain_list *modelchain, *Ins_list, *q;
  lofig_list *lofig;
  foundins_list *foundins;
  ptype_list *p;
  time_t          lstart = 0;
  time_t          lend = 0;
  struct rusage   lEND;
  struct rusage   lSTART;  
  loins_list *li;
  corresp_t *onetable;
  ht *unique;
  chain_list *ch;

  if (!circuit || !model) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 40);
//    fprintf(stderr,"genFindInstances: NULL pointer\n");
    EXIT(1);
  }

  /*do only those one which are not already searched*/
  if (Check_Model(modelradical(model->NAME))) return ;
  lofig=model->LOFIG;


  /*search instance in model with the lowest occurence*/
  for (modelchain=lofig->MODELCHAIN; nosearch==0 && modelchain; modelchain=modelchain->NEXT) {
    name_ins=modelchain->DATA;
    if (mbk_istransn(name_ins) || mbk_istransp(name_ins)) continue;
    else {
      /* search at first time dependancies*/
      //           printf("=> %s\n",name_ins);
      if (!Check_Model(name_ins)) 
        Sort_Search(circuit,getmodel(name_ins),namechain);
      count=Count_Model(name_ins);
    }   
    /* if no instance found EXIT */
    if (count==0)
      {
//        PutCheck_Model(model->NAME);
        if (model->NAME!=modelradical(model->NAME)) 
          {
            gen_printf(0,"\n--- %s (%s) ---\n", modelradical(model->NAME),strchr(model->NAME,' ')+1);
            gen_printf(0,"no unit %s left\n",name_ins);

            gen_printf(0,"%s (%s):  ", modelradical(model->NAME),strchr(model->NAME,' ')+1);
          }  
        else 
          {
            gen_printf(0,"\n--- %s ---\n",model->NAME);
            gen_printf(0,"no unit %s left\n",name_ins);

            gen_printf(0,"%s:  ",model->NAME);
          }   
        gen_printf(0,"not found\n");
             
        nosearch=1;
//        return;
      }
    else
      {
        /*clean last search on this object*/
        foundins=getfoundins(name_ins);
             
        if (getptype(getmodel(foundins->LOINS->FIGNAME)->LOFIG->USER,GEN_SYM_INFO_PTYPE)->DATA!=NULL)
          hassyms=1;
        else 
          hassyms=0;
        // if (hassyms) printf("%s has syms\n",foundins->LOINS->FIGNAME);
        if (foundins->NOT_MATCH || foundins->FLAG) 
          { 
            for ( ; foundins; foundins=foundins->NEXT) 
              {
                foundins->FLAG=0;
                foundins->SIZE=0;
                foundins->NOT_MATCH=0;
                foundins->AFTER=NULL;
                foundins->BEFORE=NULL;
                foundins->LOWER=NULL;
                if (foundins->HIGHER) 
                  {
                    p=getptype(foundins->LOINS->USER,GEN_FOUNDINS_PTYPE);
                    if (!p) 
                      {
                        avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 41);
                        //fprintf(stderr,"genFindInstances: NULL pointer\n");
                        EXIT(1);
                      }
                    p->DATA=foundins;
                    foundins->HIGHER=NULL;
                  }
                foundins->VISITED=0;
                foundins->LOCKED=0;
              }
          }
        /*select best model --> less occurences*/
        if (!reject_model(lofig, name_ins))
          {
            if (selecthassyms && !hassyms)
              {
                // loins with not syms have more priority
                select=count;
                select_model=name_ins;
                selecthassyms=hassyms;
              }
            else
              if (selecthassyms==hassyms && (select==0 || count<select))
                {
                  select=count;
                  select_model=name_ins;
                }   
          }
      }   
  }   
      
  current_variable_values=NULL;
  current_variable_name=NULL;

  if (!nosearch)
    {
      genius_chrono(&lSTART,&lstart);
      
      if (model->NAME==modelradical(model->NAME)) 
        gen_printf(0,"\n--- %s ---\n",model->NAME);
      else 
        gen_printf(0,"\n--- %s (%s) ---\n",modelradical(model->NAME),strchr(model->NAME,' ')+1);
      
      gen_printf(0,"%d possible occurences\n",select);
      
      if (select_model==NULL)
        {
          avt_errmsg(GNS_ERRMSG, "079", AVT_FATAL);
/*          fprintf(stderr,"Could not find instance in model to start search with\n");
          fprintf(stderr,"There should be at least one real instance in the model\n");*/
          EXIT(1);
        }
      
      gen_printf(0,"select model: %s\n",select_model);
      
      if (model->NAME!=modelradical(model->NAME))
        avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "%s" AVT_RESET " (%s):  ", modelradical(model->NAME),strchr(model->NAME,' ')+1);
      else
        avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "%s" AVT_RESET ":  ",model->NAME);
      fflush(GENIUS_OUTPUT);
      
      count=genFindInstances(circuit, model, select_model); // zinaps
      
    }

  count0=0;
  ch=NULL;
  unique=addht(100);

  if (model->NAME==modelradical(model->NAME))
    {
      Ins_list=GrabExistingLoins(circuit, model->NAME, model->REGEXP);
      for (q=Ins_list; q; q=q->NEXT) 
        {
          li=(loins_list *)q->DATA;
//          if (q==Ins_list)
            {
              CheckLoinsLofigInconsistancy(li, model->LOFIG);
            }
          count++;
          count0++;
          CleanLoconFlags(li);
          AddRadicalInfoInPNODE(li);
          add_foundins(model, q->DATA,NULL);
          GetLastAddedFoundins()->SAVEDFLAG=1; // instance from circuit
          add_loins_in_all_loins_found(li->INSNAME, model, onetable=create_blackbox_corresp_table(li));
          if (gethtitem(unique, onetable->GENIUS_FIGNAME)==EMPTYHT)
            {
              ch=addchain(ch, onetable);
              addhtitem(unique, onetable->GENIUS_FIGNAME, 0);
            }
        }
      if (Ins_list) 
        {
          chain_list *cl;
          for (q=ch; q!=NULL; q=q->NEXT)
            {
              onetable=(corresp_t *)q->DATA;
              for (cl=ALL_KIND_OF_MODELS; cl!=NULL && cl->DATA!=onetable->GENIUS_FIGNAME; cl=cl->NEXT) ;
              if (cl==NULL) 
                { 
                  setglobalvariables(model->LOFIG, NULL, NULL, NULL, NULL, NULL);
                  gen_build_netlist(onetable->GENIUS_FIGNAME, onetable);
                  ALL_KIND_OF_MODELS=addchain(ALL_KIND_OF_MODELS, onetable->GENIUS_FIGNAME);
                  setglobalvariables(NULL, NULL, NULL, NULL, NULL, NULL);
                }
            }
          freechain(Ins_list);
        }
    }

  freechain(ch);
  delht(unique);

  if (count || count0) 
    {
      genius_chrono(&lEND,&lend);

      if (nosearch)
        {
          if (model->NAME!=modelradical(model->NAME))
            avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "%s" AVT_RESET " (%s):  ", modelradical(model->NAME),strchr(model->NAME,' ')+1);
          else
            avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "%s" AVT_RESET ":  ",model->NAME);
          fflush(GENIUS_OUTPUT);
        }

      if (count0==0)
        avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "" AVT_CYAN "%d" AVT_RESET " found",count); //gen_printf(-1,"%d found\n",count);
      else
        avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "" AVT_CYAN "%d" AVT_RESET " (" AVT_CYAN "%d" AVT_RESET "" AVT_YELLOW "+%d" AVT_RESET ") found",count,count-count0,count0);
      if (current_variable_values==NULL) avt_fprintf(GENIUS_OUTPUT,"  (" AVT_CYAN "%s" AVT_RESET ")\n",genius_getutime(&lSTART,&lEND));
      else
        {
          chain_list *cl;
          avt_fprintf(GENIUS_OUTPUT,"   (" AVT_MAGENTA "%s" AVT_RESET " = ",current_variable_name);
          for (cl=current_variable_values;cl!=NULL;cl=cl->NEXT)
            {
              avt_fprintf(GENIUS_OUTPUT,"" AVT_YELLOW "%ld" AVT_RESET "",(long)cl->DATA);
              if (cl->NEXT!=NULL) avt_fprintf(GENIUS_OUTPUT,", ");
            }
          avt_fprintf(GENIUS_OUTPUT,")  (" AVT_CYAN "%s" AVT_RESET ")\n",genius_getutime(&lSTART,&lEND));
        }
    }
  else 
    if (!nosearch) avt_fprintf(GENIUS_OUTPUT,"" AVT_RED "not found\n",count); //gen_printf(-1,"not found\n");

  freechain(current_variable_values);
   
  AddCount_Model(model->NAME,count);

#ifdef ZINAPS_DUMP
  // zinaps
  if (GEN_DEBUG_LEVEL>1)
    if (count) displaymodels();
#endif
  /*do other architecture*/

  if (modelradical(model->NAME)==model->NAME) {
    for ( ; namechain; namechain=namechain->NEXT) {
      //        printf("-> %s\n",namechain->DATA);
      if (namechain->DATA!=model->NAME && modelradical(namechain->DATA)==model->NAME) 
        {
          model_bis=getmodel(namechain->DATA);
          Sort_Search(circuit,model_bis,NULL);
        }
    }   
    UpdateNewInstanceLofigChain();
    PutCheck_Model(model->NAME);/*entire scope on netlist*/
  }
}

/****************************************************************************/
/*           Return a list of instances to extract from netlist             */
/* models to extract are given by parameters genius and fcl                 */
/* do action associated with the model if recognized in netlist             */
/****************************************************************************/

static model_list *_model_=NULL;

void geniusExecuteAction(loins_list *loins)
{
  ptype_list *var;
  chain_list *cl;
  corresp_t *table;
  char *temp, *c;

  if (_model_==NULL) return;
 
  var=addptype(NULL, (long)modelradical(_model_->NAME), namealloc("model"));
  var=addptype(var, (long)loins->INSNAME, namealloc("instance"));
//  var=addptype(var, (long)_model_->NAME, namealloc("long_model"));
  if (modelradical(_model_->NAME)!=_model_->NAME)
    var=addptype(var, (long)(strchr(_model_->NAME,' ')+1), namealloc("archi"));
  else
    var=addptype(var, (long)_model_->NAME, namealloc("archi"));
  

  setglobalvariables(_model_->LOFIG, NULL, (chain_list *)getptype(loins->USER, FCL_TRANSLIST_PTYPE)->DATA, var, (chain_list *)getptype(loins->USER, FCL_TRANSLIST_PTYPE)->DATA, NULL);
  GEN_USER=NULL;
  table=gen_build_corresp_table(_model_->LOFIG, NULL, (chain_list *)getptype(loins->USER, FCL_TRANSLIST_PTYPE)->DATA);
  table->VAR=NULL;
  temp=modelradical(_model_->NAME);
  table->GENIUS_FIGNAME=completename(temp, _model_->LOFIG->NAME, (char *)var->TYPE);
  if ((c=strchr(temp,' '))!=NULL)
    {
      table->ARCHISTART=c+1-temp;
      table->ARCHILENGTH=strlen(temp)-table->ARCHISTART;
    }
     else
       table->ARCHISTART=table->ARCHILENGTH=0;

  add_loins_in_all_loins_found(loins->INSNAME, _model_, table);
  
  for (cl=ALL_KIND_OF_MODELS; cl!=NULL && cl->DATA!=table->GENIUS_FIGNAME; cl=cl->NEXT) ;
  if (cl==NULL) 
    { 
      gen_build_netlist(table->GENIUS_FIGNAME, table);
      ALL_KIND_OF_MODELS=addchain(ALL_KIND_OF_MODELS, table->GENIUS_FIGNAME);
    }
  
  freeptype(var);
  loins->USER = (ptype_list *)append((chain_list *)loins->USER, (chain_list *)GEN_USER);
  setglobalvariables(NULL, NULL, NULL, NULL, NULL, NULL);

  //  GenPostOp(loins);
}


void MakeGeniusModelGlobal()
{
  lofig_list *dup;
  chain_list *cl;
  for (cl=ALL_KIND_OF_MODELS; cl!=NULL; cl=cl->NEXT) 
    {
      dup=getloadedlofig((char *)cl->DATA);
      if (dup==NULL || (dup!=NULL && getptype(dup->USER,GENIUS_GHOST_PTYPE)!=NULL))
        {
          if (dup!=NULL) dellofig(dup->NAME);
          dup=rduplofig(gen_getlofig((char *)cl->DATA));
          lofigchain(dup);
          //          dup->LOCON=(locon_list *)reverse((chain_list *)dup->LOCON);
          dup->NEXT=HEAD_LOFIG;
          dup->USER=addptype(dup->USER, GENIUS_GHOST_PTYPE, (void *)0);
          HEAD_LOFIG=dup;
          addhtitem(HT_LOFIG, dup->NAME, (long)dup);
        }
    }
  freechain(ALL_KIND_OF_MODELS);
}



void ExecuteActions()
{
  all_loins_heap_struct *r;
  ptype_list *var;
  corresp_t *table;
  t_arg *ret;
  chain_list *t_arglist;
  tree_list *act;
  char *prev=NULL, *modname;
  int hastcl;
  char buf[1024];

  ALL_LOINS_FOUND=(all_loins_heap_struct *)reverse((chain_list *)ALL_LOINS_FOUND);

  for (r=ALL_LOINS_FOUND; r!=NULL; r=r->NEXT)
    {
      if (r->model->C)
        {
          act=APIGetCFunction(genius_ec, r->model->C, 'n');
          hastcl=api_has_tcl_func(r->model->C);

          table=(corresp_t *)r->CorrespondanceTable;

          if (act)
          {
          t_arglist=NULL;
          for (var=table->VAR; var!=NULL; var=var->NEXT)
            t_arglist=APIAddIntTARG(t_arglist, sensitive_namealloc((char *)var->DATA), (int)var->TYPE);

          var=table->VAR;

          t_arglist=APIAddPointerTARG(t_arglist, sensitive_namealloc("model"), "char", 1, table->GENIUS_FIGNAME);

          var=addptype(var, (long)table->GENIUS_FIGNAME/*modelradical(r->model->NAME)*/, namealloc("model"));
          t_arglist=APIAddPointerTARG(t_arglist, sensitive_namealloc("instance"), "char", 1, r->instance_name);
          var=addptype(var, (long)r->instance_name, namealloc("instance"));
          if ((modname=modelradical(r->model->NAME))!=r->model->NAME)
            {
              t_arglist=APIAddPointerTARG(t_arglist, sensitive_namealloc("archi"), "char", 1, strchr(r->model->NAME,' ')+1);
              var=addptype(var, (long)(strchr(r->model->NAME,' ')+1), namealloc("archi"));
            }
          else
            {
              t_arglist=APIAddPointerTARG(t_arglist, sensitive_namealloc("archi"), "char", 1, r->model->NAME);
              var=addptype(var, (long)r->model->NAME, namealloc("archi"));
            }
          }
          else
          {
            char buf1[256];
            strcpy(buf,"");
            for (var=table->VAR; var!=NULL; var=var->NEXT)
            {
              sprintf(buf1,"set %s %d\n", (char *)var->DATA, (int)var->TYPE); strcat(buf, buf1);
            }

          var=table->VAR;

          sprintf(buf1,"set %s %s\n", "model", table->GENIUS_FIGNAME); strcat(buf, buf1);
          var=addptype(var, (long)table->GENIUS_FIGNAME/*modelradical(r->model->NAME)*/, namealloc("model"));
          
          sprintf(buf1,"set %s %s\n", "instance", r->instance_name); strcat(buf, buf1);
          var=addptype(var, (long)r->instance_name, namealloc("instance"));

          if ((modname=modelradical(r->model->NAME))!=r->model->NAME)
            {
              sprintf(buf1,"set %s %s\n", "archi", strchr(r->model->NAME,' ')+1); strcat(buf, buf1);
              var=addptype(var, (long)(strchr(r->model->NAME,' ')+1), namealloc("archi"));
            }
          else
            {
              sprintf(buf1,"set %s %s\n", "archi", r->model->NAME); strcat(buf, buf1);
              var=addptype(var, (long)r->model->NAME, namealloc("archi"));
            }
            sprintf(buf1,"if {[catch {%s} msg options]==1} {\n"
                         "  set gns_tcl_error [dict get $options -errorinfo]\n"
                         "  0\n"
                         "}\n"
                        , r->model->C);
            
            strcat(buf, buf1);
          }
          CUR_HIER_LOFIG=gen_getlofig(table->GENIUS_FIGNAME);
          CUR_CORRESP_TABLE=table;

          GEN_USER=NULL;
          setglobalvariables(r->model->LOFIG, NULL, NULL, var, NULL, NULL);

          if (!(GEN_OPTIONS_PACK & GEN_NODEFAULT_ACTIONS))
            {
              APICallApiInitFunctions();
            }

          if (modname!=prev)
            {
              avt_fprintf(GENIUS_OUTPUT, "" AVT_BOLD "~~~ %s" AVT_RESET "\n", modname);
              fflush(stdout);
              prev=modname;
            }

          if (act)
            APIExecute_C(act, t_arglist, &ret, genius_ec, !(GEN_OPTIONS_PACK & GEN_ENABLE_CORE));
          else if (hastcl)
          {
            if (Tcl_EvalEx((Tcl_Interp *)TCL_INTERPRETER, buf, -1, TCL_EVAL_GLOBAL)!=TCL_OK)
                {
                  const char *res;
                  res=Tcl_GetVar((Tcl_Interp *)TCL_INTERPRETER, "gns_tcl_error", TCL_GLOBAL_ONLY);
                  avt_errmsg(GNS_ERRMSG, "167", AVT_ERROR, r->model->C, /*Tcl_GetStringResult((Tcl_Interp *)TCL_INTERPRETER)*/res!=NULL?res:"?");
                }
          }
          
          if (act && ret!=NULL)
            APIFreeTARG(ret);

          if (!(GEN_OPTIONS_PACK & GEN_NODEFAULT_ACTIONS))
            {
              APICallApiTerminateFunctions();
            }

          setglobalvariables(NULL, NULL, NULL, NULL, NULL, NULL);
          var->NEXT->NEXT->NEXT=NULL;
          freeptype(var);
        }
    }
  avt_fprintf(GENIUS_OUTPUT, "\n");
}



extern   void dispst0();
extern chain_list *FindInstances(netlist,genius_models,fcl_models)

     lofig_list *netlist;
     chain_list *genius_models,*fcl_models;  /*list of names of models*/
{
  chain_list *p,*ret,*Ins_list,*q,*ch;
  lofig_list *lofig;
  losig_list *sig;
  int count, count0,count1;
  model_list *model;
  char *name;
  time_t          start = 0;
  time_t          end = 0;
  struct rusage   END;
  struct rusage   START; 
  time_t          lstart = 0;
  time_t          lend = 0;
  struct rusage   lEND;
  struct rusage   lSTART; 
  lotrs_list *lt;
  loins_list *li;
  corresp_t *onetable;
  ht *unique;
  
  if (!netlist) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 42);
    //fprintf(stderr,"FindInstances: NULL pointer\n");
    EXIT(1);
  }   

  GENIUS_GLOBAL_LOFIG=netlist;
  ALL_KIND_OF_MODELS=NULL;

  mbk_debug_losig("bef remove paral", netlist);
  mbk_removeparallel(netlist, 0);

  ClearLOFIGLoconFlags(netlist);

  InitSymmetricAndCoupledInfoMecanism();
  avt_fprintf(GENIUS_OUTPUT,"\n" AVT_BOLD "_________________________" AVT_RESET "" AVT_BLACK "" AVT_REV "  FCL   " AVT_RESET "\n");
  genius_chrono(&START,&start);
  /* search first base model within only transistors*/
  for (p=fcl_models;p;p=p->NEXT) {
    genius_chrono(&lSTART,&lstart);
    name=(char*)p->DATA;
    model=getmodel(name);  
    _model_=model;
    lofig=model->LOFIG;
  mbk_debug_losig(name, netlist);

    if (name!=modelradical(name)) /*only another architecture, do nothing*/;
    else add_foundmodel(NULL,model);

    if (name!=modelradical(name)) 
      avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "%s" AVT_RESET " (%s):  ",modelradical(name),strchr(name,' ')+1);
    else 
      avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "%s" AVT_RESET ":  ",name);
    fflush(GENIUS_OUTPUT);

    count=0;

    if (lofig!=NULL && (lofig->LOTRS!=NULL || lofig->LOINS!=NULL))
      {
        SwitchLOFIGContext(); // => genius
          
        if (lofig->LOINS!=NULL)
          flattenlofig(lofig,NULL,'Y');
          
        SwitchLOFIGContext(); // => normal
          
        // remove the .transitor at the end of the transistor name
        for (lt=lofig->LOTRS;lt!=NULL;lt=lt->NEXT)
          lt->TRNAME=gen_canonize_trname (lt->TRNAME);
          
        // Modif Gilles 08/10/02: flatten before FCL call
        for (sig = lofig->LOSIG; sig; sig = sig->NEXT)
          for (ch = sig->NAMECHAIN; ch; ch = ch->NEXT)
            if (!strchr ((char*)ch->DATA, '.')) {
              sig->NAMECHAIN = addchain (sig->NAMECHAIN, ch->DATA);
              freechain (sig->NAMECHAIN->NEXT);
              sig->NAMECHAIN->NEXT = NULL;
              break;
            }
        // End modif Gilles
        
        // ---- unused blackbox
        ch=GrabBlackboxAsUnused(name, lofig, netlist);
        count1=countchain(ch);

        // ---- normal fcl
        if (IsModelFullyConnected(lofig))
          Ins_list=fclFindInstances(netlist, lofig);
        else
          Ins_list=NULL;
        Ins_list=append(ch, Ins_list);
          
        AddSymsOfLofig(lofig);
        AddCoupledOfLofig(lofig);
        for (q=Ins_list; q; q=q->NEXT) { 
          count++;
          AddSymsFlagInLoinsLocon((loins_list *)q->DATA);
          AddRadicalInfoInPNODE((loins_list *)q->DATA);
          add_foundins(model, q->DATA,NULL);
        }
         
        if (Ins_list) freechain(Ins_list);
      }

    Ins_list=GrabExistingLoins(netlist, model->NAME, model->REGEXP);
    ch=NULL;
    unique=addht(100);
    for (q=Ins_list, count0=0; q; q=q->NEXT) 
      {
        li=(loins_list *)q->DATA;
//        if (q==Ins_list)
          {
            CheckLoinsLofigInconsistancy(li, model->LOFIG);
          }
        count++;
        count0++;
        CleanLoconFlags(li);
        AddRadicalInfoInPNODE(li);
        add_foundins(model, q->DATA,NULL);
        GetLastAddedFoundins()->SAVEDFLAG=1; // instance from circuit
        add_loins_in_all_loins_found(li->INSNAME, model, onetable=create_blackbox_corresp_table(li));
        if (gethtitem(unique, onetable->GENIUS_FIGNAME)==EMPTYHT)
          {
            ch=addchain(ch, onetable);
            addhtitem(unique, onetable->GENIUS_FIGNAME, 0);
          }
      }
    AddCount_Model(model->NAME,count);
    PutCheck_Model(model->NAME);/*entire scope on netlist*/
    if (Ins_list) 
      {
        chain_list *cl;
        for (q=ch; q!=NULL; q=q->NEXT)
          {
            onetable=(corresp_t *)q->DATA;
            for (cl=ALL_KIND_OF_MODELS; cl!=NULL && cl->DATA!=onetable->GENIUS_FIGNAME; cl=cl->NEXT) ;
            if (cl==NULL) 
              { 
                setglobalvariables(model->LOFIG, NULL, NULL, NULL, NULL, NULL);
                gen_build_netlist(onetable->GENIUS_FIGNAME, onetable);
                ALL_KIND_OF_MODELS=addchain(ALL_KIND_OF_MODELS, onetable->GENIUS_FIGNAME);
                setglobalvariables(NULL, NULL, NULL, NULL, NULL, NULL);
              }
          }
        freechain(Ins_list);
      }
    freechain(ch);
    delht(unique);

    genius_chrono(&lEND,&lend);
    if (count0==0 && count1==0)
      {
        if (count) avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "" AVT_CYAN "%d" AVT_RESET " found  (" AVT_CYAN "%s" AVT_RESET ")\n",count,genius_getutime(&lSTART,&lEND));
        else avt_fprintf(GENIUS_OUTPUT,"" AVT_RED "not found\n",count);
      }
    else
      {
        avt_fprintf(GENIUS_OUTPUT,"" AVT_BOLD "" AVT_CYAN "%d" AVT_RESET " (" AVT_CYAN "%d" AVT_RESET "" AVT_YELLOW "+%d" AVT_RESET "" AVT_WHITE "+%d" AVT_RESET ") found  (" AVT_CYAN "%s" AVT_RESET ")\n",count,count-count0-count1,count0,count1,genius_getutime(&lSTART,&lEND));
      }
  }

  fclCleanShareMarks(netlist);
  _model_=NULL;

  avt_fprintf(GENIUS_OUTPUT,"Total execution time for FCL: ");
  genius_chrono(&END,&end);
  genius_print_time(&START,&END,start,end);

  avt_fprintf(GENIUS_OUTPUT,"\n" AVT_BOLD "_________________________" AVT_RESET "" AVT_BLACK "" AVT_REV " GENIUS " AVT_RESET "\n");
  genius_chrono(&START,&start);
  /* genius_models is in reversed order so it will in good ordering */
  /*creating a file for instances storing and also sort by priority model*/


  if (V_STR_TAB[__GEN_SPY].VALUE!=NULL)
    GENIUS_SPY=namealloc(V_STR_TAB[__GEN_SPY].VALUE);
  else
    GENIUS_SPY=NULL;

  for (p=genius_models; p; p=p->NEXT) {
    name=(char*)p->DATA;
    model=getmodel(name);  
    if (name!=modelradical(name)) /*only another architecture, do nothing*/;
    else 
      {
        add_foundmodel(NULL,model);
        AddSymsOfLofig(model->LOFIG);
        AddCoupledOfLofig(model->LOFIG);
        setALIMSignalType(model->LOFIG);
      }
  }
   
  for (sig=netlist->LOSIG; sig!=NULL; sig=sig->NEXT)
    sig->FLAGS=0;


  /* genius search */
  for (p=genius_models; p; p=p->NEXT) {
    name=(char*)p->DATA;
    model=getmodel(name);  
  mbk_debug_losig(name, netlist);
    Sort_Search(netlist,model,p->NEXT);
  }
  mbk_debug_losig("after", netlist);

  //   avt_fprintf(GENIUS_OUTPUT,"Executing C actions...\n");
  /*result is a list of instances found*/
  genius_chrono(&lSTART,&lstart);
  avt_fprintf(GENIUS_OUTPUT,"Updating correspondance tables... "); fflush(GENIUS_OUTPUT);
  UpdateTOPInstancesCorrespondanceTable();
//  dispst0();
  genius_chrono(&lEND,&lend);
  avt_fprintf(GENIUS_OUTPUT,"(" AVT_CYAN "%s" AVT_RESET ")\n",genius_getutime(&lSTART,&lEND));
  avt_fprintf(GENIUS_OUTPUT,"Executing actions :\n"); fflush(GENIUS_OUTPUT);

  LATEST_GNS_RUN->HIER_HT_LOFIG=GEN_HT_LOFIG;
  LATEST_GNS_RUN->HIER_HEAD_LOFIG=GEN_HEAD_LOFIG;
  memcpy(&LATEST_GNS_RUN->corresp_heap, &corresp_heap, sizeof(HeapAlloc));
  memcpy(&LATEST_GNS_RUN->subinst_heap, &subinst_heap, sizeof(HeapAlloc));

  reset_existing_loins_pnodes();
  
  ExecuteActions();

  ret=Build_loins_list();


  /* free all in results (also loins which hasn't a C behaviour*/
  free_foundmodel(1);

  mbk_restoreparallel(netlist);

  gen_set_pnode_in_loins(ret);

  CleanMainLofig(netlist);

  MakeGeniusModelGlobal();

  avt_fprintf(GENIUS_OUTPUT,"Total execution time for hierarchical recognition: ");
  genius_chrono(&END,&end);
  genius_print_time(&START,&END,start,end);
  RemoveSymmetricAndCoupledInfoMecanism();
  avt_fprintf(GENIUS_OUTPUT,"\n" AVT_BOLD "_________________________" AVT_RESET "" AVT_BLACK "" AVT_REV "  END   " AVT_RESET "\n");

  return ret;
}

void gen_set_pnode_in_loins(chain_list *list)
{
  chain_list *cl, *ch, *ch0;
  loins_list *li;
  locon_list *lc, *lc0;
  int dir;
  num_list *nl;
  subinst_t *sins;
  ht *tempht;
  long l;

  tempht=addht(100000);
  for (sins=LATEST_GNS_RUN->TOP_LEVEL_SUBINST; sins!=NULL; sins=sins->NEXT) 
    addhtitem(tempht, sins->INSNAME, (long)sins);
  
  for (cl=list; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;
      li->USER=addptype(li->USER, GENIUS_LOINS_FLAGS_PTYPE, 0);
    }

  for (cl=list; cl!=NULL; cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;

      if ((l=gethtitem(tempht, li->INSNAME))==EMPTYHT)
        {
          avt_errmsg(GNS_ERRMSG, "080", AVT_FATAL, li->INSNAME);
          //avt_error("gns", 5, AVT_ERR, "could not find instance '%s' in GNS toplevel instance\n", li->INSNAME);
          EXIT(1);
        }
      sins=(subinst_t *)l;
      if (sins->FLAGS & LOINS_IS_BLACKBOX) continue;

      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          if (lc->SIG!=NULL)
            {
              if (lc->PNODE==NULL && !mbk_LosigIsVSS(lc->SIG) && !mbk_LosigIsVDD(lc->SIG))
                {
                  dir=mbk_rc_are_internal(li, lc);
                  
                  if (dir==0) 
                    {
                      // RC are external
                      ch0=ch=GrabAllConnectorsThruCorresp(lc->NAME, lc->SIG, sins->CRT, NULL);
                      while (ch!=NULL)
                        {
                          lc0=(locon_list *)ch->DATA;
                          if (lc0->PNODE!=NULL)
                            {
                              for (nl=lc0->PNODE; nl!=NULL; nl=nl->NEXT)
                                lc->PNODE=addnum(lc->PNODE, nl->DATA);
                        }
                          ch=ch->NEXT;
                        }
                      freechain(ch0);
                    }
                  else
                    {
                      // RC are internal
                      ch=(chain_list *)getptype(lc->SIG->USER, LOFIGCHAIN)->DATA;
                      while (ch!=NULL)
                        {
                          lc0=(locon_list *)ch->DATA;
                          if (lc0->PNODE!=NULL && !(lc0->TYPE=='I' && getptype(((loins_list *)lc0->ROOT)->USER,GENIUS_LOINS_FLAGS_PTYPE)!=NULL))
                            {
                              for (nl=lc0->PNODE; nl!=NULL; nl=nl->NEXT)
                                lc->PNODE=addnum(lc->PNODE, nl->DATA);
                            }
                          ch=ch->NEXT;
                        }
                    }
                }
            }
          
        }
    }
  delht(tempht);
}
