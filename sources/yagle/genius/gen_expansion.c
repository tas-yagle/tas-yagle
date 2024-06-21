

#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include FCL_H
#include MLU_H
#include API_H
#include "gen_env.h"
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_expansion.h"
#include "gen_display.h"

void MarkGensigAsGhost( loins_list *li)
{
  // desactivation of gensinchain for instances to expand
  locon_list *lc;
  ptype_list *gencon,*gensig;
  genconchain_list *gc;
  gensigchain_list *gs;
  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      gencon=getptype(lc->USER, GENCONCHAIN);
      if (gencon!=NULL)
    {
      gensig=getptype(lc->SIG->USER, GENSIGCHAIN);
      for (gc=(genconchain_list *)gencon->DATA;gc!=NULL;gc=gc->NEXT)
        {
          for (gs=(gensigchain_list *)gensig->DATA;gs!=NULL && gs->LOCON!=lc; gs=gs->NEXT) ;
          if (gs==NULL) avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 13); //EXIT(10); // internal error
          gs->ghost=1;
        }
    }
    }
}

loins_list *AddLoinsToModel(loins_list *li, ptype_list *env, char *toappend, int index)
{
  loins_list *newloins;
  char temp[200];
  locon_list *lc,*lc0;
  genconchain_list *gc;
  gensigchain_list *gs;
  int con_low_src,con_high_src;
  tree_list *con_low_tree, *con_high_tree,*con_tree, *sig_tree;
  ptype_list *p, *q;


  newloins=duploins(li);
  sprintf(temp,"%s%s",newloins->INSNAME,toappend);

  gen_printf(2," - %s: %s port map (",temp,newloins->FIGNAME);

  newloins->INSNAME=namealloc(temp);
  newloins->LOCON=duploconlst(li->LOCON);
  
  for (lc=li->LOCON, lc0=newloins->LOCON;lc!=NULL;lc=lc->NEXT, lc0=lc0->NEXT)
    {

      lc0->ROOT=newloins;

      lc->SIG->FLAGS&=~0x3; // disable quick match

      p=getptype(lc0->SIG->USER,GENSIGCHAIN);
      if (!p) 
    {
      lc0->SIG->USER=addptype(lc0->SIG->USER,GENSIGCHAIN,NULL);
      p=lc0->SIG->USER;
    }
  
      q=getptype(lc0->USER,GENCONCHAIN);
      if (!q) 
    {
      lc0->USER=addptype(lc0->USER,GENCONCHAIN,NULL);
      q=lc0->USER;
    }  

      gc=(genconchain_list *)getptype(lc->USER, GENCONCHAIN)->DATA;

      // FOR THE LOCON

      if (TOKEN(gc->LOSIG_EXP)==GEN_TOKEN_TO || TOKEN(gc->LOSIG_EXP)==GEN_TOKEN_DOWNTO) 
    {
      con_low_src=Eval_Exp_VHDL(gc->LOSIG_EXP->NEXT,env);
      con_high_src=Eval_Exp_VHDL(gc->LOSIG_EXP->NEXT->NEXT,env);

      if (con_low_src==UNDEF) con_low_tree=Duplicate_Tree(gc->LOSIG_EXP->NEXT);
      else con_low_tree=PutAto_Tree(GEN_TOKEN_DIGIT,FILE_NAME(gc->LOSIG_EXP),LINE(gc->LOSIG_EXP),(void *)(long)con_low_src);
      if (con_high_src==UNDEF) con_high_tree=Duplicate_Tree(gc->LOSIG_EXP->NEXT->NEXT);
      else con_high_tree=PutAto_Tree(GEN_TOKEN_DIGIT,FILE_NAME(gc->LOSIG_EXP),LINE(gc->LOSIG_EXP),(void *)(long)con_high_src);

      sig_tree=PutBin_Tree(TOKEN(gc->LOSIG_EXP),FILE_NAME(gc->LOSIG_EXP),LINE(gc->LOSIG_EXP),(void *)con_low_tree,(void *)con_high_tree);
      gen_printf(2,"%s(%d..%d)%c",(char *)lc->SIG->NAMECHAIN->DATA,con_low_src==UNDEF?-1:con_low_src,con_high_src==UNDEF?-1:con_high_src,lc->NEXT==NULL?' ':',');
    }
      else
    {
      con_low_src=Eval_Exp_VHDL(gc->LOSIG_EXP,env);
/*      if (con_low_src==UNDEF)
        con_tree=Duplicate_Tree(gc->LOSIG_EXP);*/

      if (con_low_src==UNDEF)
        sig_tree=Duplicate_Tree(gc->LOSIG_EXP);
      else
        sig_tree=PutAto_Tree(GEN_TOKEN_DIGIT,FILE_NAME(gc->LOCON_EXP),LINE(gc->LOCON_EXP),(void *)(long)con_low_src);
      if (con_low_src==UNDEF)
        gen_printf(2,"%s(?)%c",(char *)lc->SIG->NAMECHAIN->DATA,lc->NEXT==NULL?' ':',');
      else if (con_low_src==-1)
        gen_printf(2,"%s%c",(char *)lc->SIG->NAMECHAIN->DATA,lc->NEXT==NULL?' ':',');
      else
        gen_printf(2,"%s(%d)%c",(char *)lc->SIG->NAMECHAIN->DATA,con_low_src,lc->NEXT==NULL?' ':',');
    }

      q->DATA=addgenconchain((genconchain_list*)q->DATA, Duplicate_Tree(gc->LOCON_EXP), lc0->SIG, sig_tree); 

      gs=(gensigchain_list *)getptype(lc->USER, GENCONCHAIN)->DATA;

      p->DATA=addgensigchain((gensigchain_list*)p->DATA, Duplicate_Tree(sig_tree), lc0, Duplicate_Tree(gc->LOCON_EXP));
      ((gensigchain_list*)p->DATA)->ghost=2; // to hurry cleanup
    }

  gen_printf(2,")\n");

  q=getptype(li->USER,GENERIC_PTYPE);
//  sig_tree=(tree_list *)q->DATA;
  if (q==NULL) avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 14);
  // putting a fake tree here, i hope there will not be any problem
  sig_tree=(tree_list *)q->DATA;
  con_tree=PutAto_Tree(GEN_TOKEN_LOINS, FILE_NAME(sig_tree), LINE(sig_tree), (tree_list *)newloins);
  newloins->USER=addptype(newloins->USER,GENERIC_PTYPE,con_tree);
  q=getptype(li->USER,GEN_GENERIC_MAP_PTYPE);
  if (q==NULL) avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 15);
  newloins->USER=addptype(newloins->USER,GEN_GENERIC_MAP_PTYPE, q->DATA);
// loins flags
  q=getptype(li->USER,GENIUS_LOINS_FLAGS_PTYPE);
  newloins->USER=addptype(newloins->USER,GENIUS_LOINS_FLAGS_PTYPE, q->DATA);
  newloins->USER=addptype(newloins->USER,GEN_LOOP_INDEX_PTYPE, (void *)(long)index);
  newloins->USER=addptype(newloins->USER,GEN_ORIG_LOINS_PTYPE, li);
  return newloins;
}


void getallloins(tree_list* tree, chain_list **allloins)
{
  
  if (!tree) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 16);
    //fprintf(stderr,"choose_loins: NULL pointer\n");
    EXIT(1);
  }
  
   switch(TOKEN(tree)) {
   case GEN_TOKEN_NODE:
      getallloins(tree->DATA, allloins);
      break;
   case GEN_TOKEN_EXCLUDE:/*pointer on a loins in lofig*/
      getallloins(tree->NEXT, allloins);
      break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
      *allloins=addchain(*allloins,(loins_list*)tree->DATA);
      break;
   case GEN_TOKEN_IF:  /*at this state of development if aren't supported by searching*/
      avt_errmsg(GNS_ERRMSG, "029", AVT_FATAL, FILE_NAME(tree),LINE(tree));
/*      fprintf(stderr,
         "%s:%d: IF forbidden for GENIUS\n",FILE_NAME(tree),LINE(tree));*/
      EXIT(1);
      getallloins(tree->NEXT->NEXT, allloins);
      break;
   case GEN_TOKEN_FOR:
      getallloins(tree->NEXT->NEXT->NEXT->NEXT, allloins);
      break;
   case ';':
      getallloins(tree->NEXT->NEXT, allloins);
      getallloins(tree->NEXT, allloins);
      break;
   default:
      Error_Tree("getallloins",tree);
      EXIT(4);
   }
}

static struct 
{
  char *model; // model of instance assigning a value
  char *variable; // variable name
  int value; // value assigned by the instance
  chain_list *allvalues;
  chain_list *running;
  int nbdiff; // number of instances assigning a different value
} info[1000];
static int last,current,firsttime;

/* return the number of FOR to expand */
int CheckGenericValues(chain_list *instances, chain_list *forvars, ptype_list *env)
{

  foundins_list *foundins,*fil;
  chain_list *l,*cl,*cl1;
  ptype_list *p, *p1, *p2;
  loins_list *li,*li2;
  generic_map_info *gmi, *gmi0;
  int i,find,mustallbe,mustexist,nbunassigned,countFOR=0;

//  if (forvars==NULL) return 0;

  last=0;
  for (l=instances;l!=NULL;l=l->NEXT)
    {
      li=(loins_list *)l->DATA;
      if (mbk_istransn(li->FIGNAME) || mbk_istransp(li->FIGNAME)) continue;

      gen_printf(6,"- %s::%s\n",li->FIGNAME,li->INSNAME);

      p=getptype(li->USER,GEN_GENERIC_MAP_PTYPE);
      foundins=getfoundins(li->FIGNAME);
      
      for (cl1=p->DATA;cl1!=NULL;cl1=cl1->NEXT) // for each generic variable
    {
      gmi=(generic_map_info *)cl1->DATA;
      if (gmi->type!='n')
        {
          avt_errmsg(GNS_ERRMSG, "030", AVT_FATAL, li->INSNAME);
          //fprintf(stderr,"A variable name was expected for instance '%s', found a number\n",li->INSNAME);
          EXIT(1);
        }
      find=mustallbe=mustexist=0;
      for (i=0;i<last && info[i].variable!=gmi->right.name/*cl1->DATA*/;i++) ;
      // variable allready set ?
      if (i>=last)
        {
          // no, no check needed
          i=last;
          info[last].variable=gmi->right.name/*cl1->DATA*/;
          info[last].nbdiff=0;
          info[last].allvalues=NULL;
          info[last].running=NULL;
          info[last++].model=li->FIGNAME;
          // setting the variable
          find=1; // this an instance and set the variable value
        }
      else
        {
          if (info[i].model!=li->FIGNAME)
        mustexist=1; // at list one instance must match the variable value
        }

      gen_printf(6,"-- %s [%c]\n",gmi->right.name,find?'F':mustexist?'E':mustallbe?'A':'?');

      for (fil=foundins;fil!=NULL;fil=fil->NEXT)
        {
          li2=fil->LOINS;

          gen_printf(6,"--- %s\n",li2->INSNAME);

          // find the variable
          p1=NULL;
          for (cl=p->DATA,p2=fil->VAR;cl!=NULL;cl=cl->NEXT,p2=p2->NEXT)
        {
          gmi0=(generic_map_info *)cl->DATA;
          if (cl==cl1)
            {
              // looking for explicit assignation
              if (gmi0->left/*cl->NEXT->DATA*/!=NULL)
            {
              // searching for this generic in the circuit instance // explicit assignment
              for (p1=fil->VAR;p1!=NULL && p1->DATA!=gmi0->left;p1=p1->NEXT) ;
            }
              else p1=p2; // implicit assignment

              if (p1==NULL)
            {
              // generic not found
              avt_errmsg(GNS_ERRMSG, "031", AVT_FATAL, (char *)gmi0->left);
//              fprintf(stderr,"CheckGenericValues: Generic variable '%s' not define\n",(char *)gmi0->left/*cl->NEXT->DATA*/);
              EXIT(1);
            }
              break;
            }
        }
          if (p1==NULL) EXIT(88);

          if (find)
        info[i].value=p1->TYPE;
          else 
        if (mustexist==1)
          {
            if (p1->TYPE==info[i].value) {mustexist=2;/*break;*/}
          }
        else 
          if (mustallbe && p1->TYPE!=info[i].value)
            { 
              avt_errmsg(GNS_ERRMSG, "032", AVT_WARNING, (char *)p1->DATA,li->INSNAME,info[i].value,p1->TYPE);
              fprintf(stderr,"Warning: CheckGenericValues: Value of '%s' for instance '%s' must be %d, actually %ld\n",(char *)p1->DATA/*cl->NEXT->DATA*/,li->INSNAME,info[i].value,p1->TYPE);
            }
          if (info[i].value!=p1->TYPE)
        info[i].nbdiff++;

          for (cl=info[i].allvalues;cl!=NULL && (long)cl->DATA!=p1->TYPE;cl=cl->NEXT) ;
          if (cl==NULL) info[i].allvalues=addchain(info[i].allvalues, (void *)p1->TYPE);

        }
      if (mustexist==1)
        { 
          avt_errmsg(GNS_ERRMSG, "033", AVT_WARNING, li->FIGNAME,(char *)cl->NEXT->DATA,info[i].value);
          //fprintf(stderr,"Warning: CheckGenericValues: There should be at least one instance of model '%s' with %s=%d\n",li->FIGNAME,(char *)cl->NEXT->DATA,info[i].value);
        }
    }
    }

  for (i=0;i<last;i++) 
    {
      if (info[i].allvalues->NEXT!=NULL)
    {
      chain_list *c0, *c1, *bs;
      long val;
      for (c0=info[i].allvalues;c0->NEXT!=NULL;c0=c0->NEXT)
        {
          bs=c0;
          for (c1=c0->NEXT;c1!=NULL;c1=c1->NEXT)
        if ((long)c1->DATA<(long)bs->DATA) bs=c1;
          val=(long)bs->DATA; bs->DATA=c0->DATA; c0->DATA=(void *)val;
        }
    }
      // a little sort

      info[i].running=info[i].allvalues;
    }
  current=last-1;
  firsttime=1;

  // checking and assigning the know variable used in loops
  for (cl=forvars,nbunassigned=0;cl!=NULL;cl=cl->NEXT)
    {
      // find the variable
      for (i=0;i<last && cl->DATA!=info[i].variable;i++) ;
      if (i>=last)
    nbunassigned++;
      else
    {
      // found, set the value
      for (p=env;p!=NULL && p->DATA!=cl->DATA;p=p->NEXT) ;
      if (p==NULL)
        {
          avt_errmsg(GNS_ERRMSG, "031", AVT_FATAL, (char *)cl->NEXT->DATA);
//          fprintf(stderr,"CheckGenericValues: Generic variable '%s' not define\n",(char *)cl->NEXT->DATA);
          EXIT(1);
        }
      p->TYPE=info[i].value;

      gen_printf(2,"forced for loop: %s = %ld\n",(char *)p->DATA,p->TYPE);

      countFOR++;
    }
    }

  if (nbunassigned>1)
    {
      avt_errmsg(GNS_ERRMSG, "034", AVT_FATAL);
      //fprintf(stderr,"CheckGenericValues: More than 1 unknown generic variable\n");
      EXIT(1);
    }
  return countFOR;
}

int GenerateNextValueConfiguration(ptype_list *env, chain_list *forvars)
{
  int i=0, runup;
  chain_list *cl;
  ptype_list *p;

  if (!firsttime && current<0) return 0; // no more configurations

  for (p=env;p!=NULL;p=p->NEXT) 
    {
      for (i=0;i<last && p->DATA!=info[i].variable;i++) ;

      if (i>=last)
    {
      p->TYPE=UNDEF; // keep it?
    }
      else
    {
      
      for (cl=forvars;cl!=NULL && cl->DATA!=info[i].variable;cl=cl->NEXT) ;

      if (cl!=NULL)
        {
          // found, set the value
          p->TYPE=(long)info[i].running->DATA;
        }
      else
        {
          p->TYPE=UNDEF;
        }
    }
    }
  
  if (firsttime && current==-1) { firsttime=0; return 1; }

  do 
    {
      runup=0;
      for (cl=forvars;cl!=NULL && cl->DATA!=info[current].variable;cl=cl->NEXT) ;

      if (cl!=NULL) info[current].running=info[current].running->NEXT;
      if (cl==NULL || info[current].running==NULL) 
    {
      info[current].running=info[current].allvalues;
      current--;
      runup=1;
    }
    } while (current>=0 && runup);
  if (current>=0) current=last-1;
  firsttime=0;
  return 1;
}

void FreeConfigurationsData()
{
  int i;
  for (i=0;i<last;i++) freechain(info[i].allvalues);
}

/* retreives all the variables used in the FOR statements */
void search_FOR(tree_list* tree, chain_list **vars, int *launchexpansion)
{
   static int inloop=0;
   chain_list *bound_idents;

   if (!tree) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 17);
     //fprintf(stderr,"search_FOR: NULL pointer\n");
     EXIT(1);
   }
   
   switch(TOKEN(tree)) {
   case GEN_TOKEN_NODE:
      search_FOR(tree->DATA,vars,launchexpansion);
      break;
   case GEN_TOKEN_EXCLUDE:/*pointer on a loins in lofig*/
      search_FOR(tree->NEXT,vars,launchexpansion);
      break;
   case GEN_TOKEN_LOINS:
      break;
   case GEN_TOKEN_IF:
      search_FOR(tree->NEXT->NEXT,vars,launchexpansion);
      break;
   case GEN_TOKEN_FOR:
      inloop++;
      bound_idents=GetnameS_Exp_VHDL(tree->NEXT->NEXT->NEXT);
      if (!bound_idents) 
    {
      (*launchexpansion)++;
    }
      else
    {
      *vars=append(*vars,bound_idents);
    }
      search_FOR(tree->NEXT->NEXT->NEXT->NEXT,vars,launchexpansion);
      break;
   case ';':
      search_FOR(tree->NEXT,vars,launchexpansion);
      search_FOR(tree->NEXT->NEXT,vars,launchexpansion);
      break;
   default:
      Error_Tree("search_FOR",tree);
      EXIT(5);
   }
}



void ExpandFOR(tree_list* tree, lofig_list *circuit, ptype_list *env, chain_list **expansedloins, chain_list **originalloins, char* nameadd, int depart, int oldrange)
{
   int index_value,for_end;
   char *index_ident,*bound_ident;
   static int inloop=0;
   ptype_list *q;
   loins_list *ins_model;
   chain_list *cl;
   char nameadd0[50];
   
   if (!tree) {
     avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 18);
//     fprintf(stderr,"ExpandFOR: NULL pointer\n");
     EXIT(1);
   }
   
   switch(TOKEN(tree)) {
   case GEN_TOKEN_NODE:
      ExpandFOR(tree->DATA, circuit, env, expansedloins, originalloins, nameadd, depart, oldrange);
      break;
   case GEN_TOKEN_EXCLUDE:/*pointer on a loins in lofig*/
      ExpandFOR(tree->NEXT, circuit, env, expansedloins, originalloins, nameadd, depart, oldrange);
      break;
   case GEN_TOKEN_LOINS:/*pointer on a loins in lofig*/
      /*if we are in a loop, memorize*/
      if (inloop) 
    {
      ins_model=(loins_list*)tree->DATA;

      // add the loins to the list of original loins to be abble to put the lofig
      // back to its original form
      for (cl=*originalloins;cl!=NULL && cl->DATA!=ins_model; cl=cl->NEXT) ;
      if (cl==NULL)
          *originalloins=addchain(*originalloins, ins_model);

      // creating instance
      *expansedloins=addchain(*expansedloins, AddLoinsToModel(ins_model, env, nameadd, depart));
    }
      break;
   case GEN_TOKEN_IF:
      ExpandFOR(tree->NEXT->NEXT, circuit,env, expansedloins, originalloins, nameadd, depart, oldrange);
      break;
   case GEN_TOKEN_FOR:
      /*entity environment ==> 'n' */
      bound_ident=Getname_Exp_VHDL(tree->NEXT->NEXT->NEXT);      

      for_end=Eval_Exp_VHDL(tree->NEXT->NEXT->NEXT,env);
      if (for_end!=UNDEF)
    {
      // a loop to expand
      /*local environment */
      int range0;
      inloop++;
      index_value=Eval_Exp_VHDL(tree->NEXT->NEXT,env);
      index_ident=getname(tree->NEXT);
      env=addptype(env,index_value,index_ident);
      
          if (index_value>for_end)
            {
              // handleling the case of downto loop
              range0=for_end;
              for_end=index_value;
              index_value=range0;
            }

      gen_printf(2,"for %s in %d to %d {\n",(char *)env->DATA,index_value,for_end);

      /*iterations*/
      range0=for_end-index_value+1;
      for (env->TYPE=index_value; env->TYPE<=for_end; env->TYPE++)
        {
//          sprintf(nameadd0,"%s_%s%ld", nameadd, (char *)env->DATA, env->TYPE);
          sprintf(nameadd0,"%s_%ld", nameadd, env->TYPE);
          ExpandFOR(tree->NEXT->NEXT->NEXT->NEXT, circuit, env, expansedloins, originalloins, nameadd0/*env->TYPE*/, depart*oldrange+env->TYPE, range0);
        }
      /*environment*/
      q=env;
      env=env->NEXT;
      q->NEXT=NULL;/* break recursivity*/
      freeptype(q);
      inloop--;
    }
      else
    if (inloop)
      {
        avt_errmsg(GNS_ERRMSG, "035", AVT_FATAL, bound_ident);
//        fprintf(stderr,"ExpandFOR: Variable '%s' is not defined yet\n", bound_ident);
        EXIT(6);
      }
      break;
   case ';':
      ExpandFOR(tree->NEXT, circuit, env, expansedloins, originalloins, nameadd, depart, oldrange);
      ExpandFOR(tree->NEXT->NEXT, circuit, env, expansedloins, originalloins, nameadd, depart, oldrange);
      break;
   default:
      Error_Tree("ExpandFOR",tree);
      EXIT(7);
   }
}


void FinishExpansion(chain_list *originalloins)
{
  chain_list *cl;

  for (cl=originalloins; cl!=NULL; cl=cl->NEXT) 
    {
      MarkGensigAsGhost((loins_list *)cl->DATA);
      gen_printf(3,"%s marked ghost\n",((loins_list *)cl->DATA)->INSNAME);
    }
}


void CleanUpLofig(chain_list *expansedloins)
{
  chain_list *cl;
  chain_list *allsigs;
  loins_list *li;
  locon_list *lc, *lc0;
  losig_list *ls;
  ptype_list *pt;
  gensigchain_list *gs, *gsnext, *newgs;

  for (cl=expansedloins,allsigs=NULL; cl!=NULL;cl=cl->NEXT)
    {
      li=(loins_list *)cl->DATA;

      // grad the signals to modify
      for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      ls=lc->SIG;
      if (getptype(ls->USER,TO_DELETE_MARK_PTYPE)==NULL)
        {
          allsigs=addchain(allsigs,ls);
          ls->USER=addptype(ls->USER,TO_DELETE_MARK_PTYPE,0);
        }
      // destroy genconchain
      if ((pt=getptype(lc->USER,GENCONCHAIN))==NULL) 
        EXIT(2); // should never append
      freegenconchain((genconchain_list *)pt->DATA);
      freeptype(lc->USER); //=delptype(lc->USER, GENCONCHAIN);
    }

      // destroy locons
      for (lc=li->LOCON;lc!=NULL;lc=lc0)
    {
      lc0=lc->NEXT;
      mbkfree(lc);
    }
      // destroy loins
      pt=getptype(li->USER,GENERIC_PTYPE);
      Free_Tree((tree_list *)pt->DATA);
      freeptype(li->USER); //=delptype(li->USER,GENERIC_PTYPE);
      mbkfree(li);
    }
  
  // the expansed loins are cleaned, still the signal have some losigchain to clean
  for (cl=allsigs; cl!=NULL; cl=cl->NEXT)
    {
      ls=(losig_list *)cl->DATA;
      ls->USER=delptype(ls->USER,TO_DELETE_MARK_PTYPE);
      newgs=NULL; // new gensigchain_list
      if ((pt=getptype(ls->USER,GENSIGCHAIN))==NULL) EXIT(3); // should never append
      
      for (gs=(gensigchain_list *)pt->DATA; gs!=NULL; gs=gsnext)
    {
      gsnext=gs->NEXT;
      if (gs->ghost==2)
        {
          gs->NEXT=NULL;
          freegensigchain(gs);
        }
      else
        {
          if (gs->ghost==1)
        gs->ghost=0; // re-enable the gensigchain
          gs->NEXT=newgs;
          newgs=gs;
        }
    }

      pt->DATA=newgs;
    }
  // now the lofig sigs are cleaned
  freechain(allsigs);
}





void UndoExpansion(chain_list *expansedloins)
{
  // the function name isn't good but it does what we want
  // remove the expansed loins and put back the original loins
//  FinishExpansion(lf, expansedloins, originalloins);
  
  // now, we clean up the genconchain_list and the gensigchain_list
  CleanUpLofig(expansedloins);
}
