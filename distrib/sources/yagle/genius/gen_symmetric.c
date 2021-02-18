
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include FCL_H
#include MLU_H
#include AVT_H
#include API_H
#include "gen_env.h"
#include "gen_execute_VHDL.h"
#include "gen_model_utils.h"
#include "gen_model_transistor.h"
#include "gen_corresp.h"
#include "gen_search_utils.h"
#include "gen_symmetric.h"
#include "gen_optimizations.h"
#include "gen_display.h"

chain_list *PENDING_LOFIG_CHAIN=NULL;
HeapAlloc swap_heap;
ht *radindexht;
char **radtable;
int curradindex;

void SwapHeap_Manage(int mode)
{
  if (mode==0)
    {
      radindexht=addht(1500);
      radtable=(char **)mbkalloc(sizeof(char *)*10000);
      curradindex=1; // 1 to avoid pnode=0
      CreateHeap(sizeof(undoswap), 0, &swap_heap);
    }
  else
    {
      DeleteHeap(&swap_heap);
      mbkfree(radtable);
      delht(radindexht);
    }
}

int getradindex(char *name)
{
  long res;
  res=gethtitem(radindexht, name);
  if (res!=EMPTYHT) return (int)res;
  radtable[curradindex++]=name;
  addhtitem(radindexht, name, (long)(curradindex-1));
  return curradindex-1;
}

inline char *fastradical(locon_list *lc)
{
  Pnode2Radical *r;
  r=(Pnode2Radical *)&lc->PNODE;
  if (r->radindex<1 || r->radindex>=curradindex) {/*printf("%s",0x01);*/EXIT(44);}
  return radtable[r->radindex];
}

inline int fastindex(locon_list *lc)
{
  Pnode2Radical *r;
  r=(Pnode2Radical *)&lc->PNODE;
  return r->index;
}


void *AddSwap(locon_list *lc, locon_list *sym)
{
  undoswap *temp;
  temp=(undoswap *)AddHeapItem(&swap_heap);
  temp->lc=lc;
  temp->sym=sym;
  return temp;
}

void UndoAndDeleteSwap(chain_list *cl)
{
  undoswap *temp=(undoswap *)cl->DATA;
  SwapLoconWithSym(temp->lc, temp->sym);
  DelHeapItem(&swap_heap, temp);
}

void JustFreeSwap(void *item)
{
  DelHeapItem(&swap_heap, item);
}

void UpdatePendingLofigChain(locon_list *lc, locon_list *sym)
{
  exchange_pending_con(lc, sym);
}

void PushPendingLofigChain(chain_list *cl)
{
  PENDING_LOFIG_CHAIN=addchain(PENDING_LOFIG_CHAIN, cl);
  while (cl!=NULL)
    {
      add_pending_con((locon_list *)cl->DATA, cl);
      cl=cl->NEXT;
    }
}

void PopPendingLofigChain()
{
  chain_list *last=PENDING_LOFIG_CHAIN, *cl;
  for (cl=(chain_list *)last->DATA; cl!=NULL; cl=cl->NEXT)
    remove_pending_con((locon_list *)cl->DATA);
  PENDING_LOFIG_CHAIN=PENDING_LOFIG_CHAIN->NEXT;
  last->NEXT=NULL;
  freechain(last);
}


SymInfoItem *FindVectorRadicalSymInChain(SymInfoItem *sii, locon_list *lc)
{
  while (sii!=NULL && sii->ConnectorRadical!=vectorradical(lc->NAME)) sii=sii->NEXT;
  return sii;
}

locon_list *FindALoconPlusIndexInChain(SymInfoItem *ch, locon_list *lc_list, int bit_wanted)
{
  locon_list *sym;

  for (sym=lc_list;sym!=NULL && !(fastradical(sym)==ch->ConnectorRadical && fastindex(sym)==bit_wanted);sym=sym->NEXT) ;

  return sym;
}

SymInfoItem *AreYouMySymmetric(SymInfoItem *ch, locon_list *you/*, locon_list *me*/)
{
  char *temp_name;
  SymInfoItem *ch1;

  temp_name=fastradical(you); 
  
  for (ch1=ch;ch1!=NULL && ch1->ConnectorRadical!=temp_name;ch1=ch1->NEXT) ;

  return ch1;
}

// zinaps : a commenter
void SwapLoconWithSym(locon_list *syms_locon, locon_list *circuit_con)
{
  losig_list *tmpsig;
  ptype_list *lfc,*user,*top,*top1;
  mark_list *mark0, *mark1;
  ptype_list *model_sig_last,*model_sig_last1;
  int setfixed0=0,setfixed1=0;

#ifdef ZINAPS_DUMP
  if (GEN_DEBUG_LEVEL>3)
    gen_printf(3,"symmetry: for %s Swapping connector signals %s(%s) %s(%s)\n",((loins_list *)circuit_con->ROOT)->INSNAME,syms_locon->NAME,syms_locon->SIG->NAMECHAIN->DATA,circuit_con->NAME,circuit_con->SIG->NAMECHAIN->DATA);
#endif  

  UpdatePendingLofigChain(circuit_con, syms_locon);

  // it's another than myself, we need to swap the fields
  tmpsig=syms_locon->SIG; syms_locon->SIG=circuit_con->SIG; circuit_con->SIG=tmpsig;
  user=syms_locon->USER; syms_locon->USER=circuit_con->USER; circuit_con->USER=user;

  // les LOFIGCHAINs doivent etre updatées

  fastswaplofigchain(syms_locon->SIG, circuit_con, syms_locon, 0);
  fastswaplofigchain(circuit_con->SIG, syms_locon, circuit_con, 0);

  // fixed tag update
  if ((lfc=getptype(syms_locon->USER,GEN_FIXED_PTYPE))!=NULL) setfixed0=1;
  if ((lfc=getptype(circuit_con->USER,GEN_FIXED_PTYPE))!=NULL) setfixed1=1;
  if (setfixed0 || setfixed1) EXIT(110);
  if ((setfixed0 && !setfixed1) || (!setfixed0 && setfixed1))
    {
      if (setfixed0)
        {
          syms_locon->USER=delptype(syms_locon->USER, GEN_FIXED_PTYPE);
          circuit_con->USER=addptype(circuit_con->USER, GEN_FIXED_PTYPE, circuit_con);
        }
      else
        {
          circuit_con->USER=delptype(circuit_con->USER, GEN_FIXED_PTYPE);
          syms_locon->USER=addptype(syms_locon->USER, GEN_FIXED_PTYPE, syms_locon);
        }
    }

  // marks update
  mark0=NULL; 
  if ((lfc=getptype(syms_locon->USER,GEN_LOCON_MARK_PTYPE))!=NULL)
    {
      mark0=(mark_list *)lfc->DATA;
    }

  mark1=NULL;
  if ((lfc=getptype(circuit_con->USER,GEN_LOCON_MARK_PTYPE))!=NULL)
    {
      mark1=(mark_list *)lfc->DATA;
    }
  //  gen_printf(5,"<%p %p>",mark0, mark1);
  if (mark0 && mark1)
    {
      int type;
      top=getptype(circuit_con->SIG->USER,GEN_VISITED_PTYPE);
      top1=getptype(syms_locon->SIG->USER,GEN_VISITED_PTYPE);
      //      gen_printf(5,"(%s %s)",mark0->LOSIG->NAMECHAIN->DATA, mark1->LOSIG->NAMECHAIN->DATA);
      if (mark0->COUNT>1) EXIT(55);
      if (mark1->COUNT>1) EXIT(56);
      for (model_sig_last=(ptype_list*)top->DATA; model_sig_last && model_sig_last->DATA!=mark1; model_sig_last=model_sig_last->NEXT) ;
      if (model_sig_last==NULL) EXIT(35);

      for (model_sig_last1=(ptype_list*)top1->DATA; model_sig_last1 && model_sig_last1->DATA!=mark0; model_sig_last1=model_sig_last1->NEXT) ;
      if (model_sig_last1==NULL) EXIT(36);

      // inversement des mark dans les LOSIGs du circuit
      model_sig_last->DATA=mark0;
      model_sig_last1->DATA=mark1;
      type=model_sig_last->TYPE; model_sig_last->TYPE=model_sig_last1->TYPE;model_sig_last1->TYPE=type;

      // remise en place des marks dans les LOCONs
      top=getptype(circuit_con->USER, GEN_LOCON_MARK_PTYPE);
      top1=getptype(syms_locon->USER, GEN_LOCON_MARK_PTYPE);
      if (top->DATA==mark0) EXIT(54);
      top->DATA=mark0;
      top1->DATA=mark1;

      type=mark0->VISITED; mark0->VISITED=mark1->VISITED; mark1->VISITED=type;
      tmpsig=mark0->LOSIG; mark0->LOSIG=mark1->LOSIG; mark1->LOSIG=tmpsig;
    }
  else if (mark0 || mark1) EXIT(123);

  //    gen_printf(5,"\n");
}

locon_list *FindConWithSym(chain_list *locons, locon_list *me)
{
  locon_list *lc;
  SymInfoItem *ch;
  chain_list *cl;
  SymInfoItem *syms;
  char *tmp_name;
  int v_me;

  tmp_name=fastradical(me);
  for (cl=locons;cl!=NULL;cl=cl->NEXT)
    {
      lc=(locon_list *)cl->DATA;
      syms=GetSymmetricChain(lc);

      if (syms==NULL) 
        {
        avt_errmsg(GNS_ERRMSG, "115", AVT_FATAL, ((loins_list *)lc->ROOT)->INSNAME,lc->NAME);
//          fprintf(stderr,"connector %s.%s is in coupled list but has no symetric\n",((loins_list *)lc->ROOT)->INSNAME,lc->NAME);
          EXIT(2);
        }

      if (syms->FLAGS!=1)
        {
          for (ch=syms;ch!=NULL && ch->ConnectorRadical!=tmp_name;ch=ch->NEXT) ;
        }
      else
        {
          if ((v_me=fastindex(me))==-1) return NULL;

          for (ch=syms;ch!=NULL;ch=ch->NEXT) 
            {
              if (ch->ConnectorRadical==tmp_name && fastindex(lc)==v_me) break;
            }
        }
      if (ch!=NULL) return lc;
    }
        avt_errmsg(GNS_ERRMSG, "116", AVT_FATAL, lc->NAME);
//  fprintf(stderr,"no symmetry found for connector %s in coupled connector list (1)\n",lc->NAME);
  EXIT(2);
  return NULL; // pour la forme
}

static int  COUPLED_VECTOR_MODE;

chain_list *GrabConnectors(loins_list *li, SymInfoItem *syms, int index)
{
  SymInfoItem *cl;
  chain_list *found=NULL;
  locon_list *lc;
  int hasone=0/*, hasnone=0*/;
  /*
  // new
  for (cl=syms;cl!=NULL; cl=cl->NEXT)
  if (cl->FLAGS==1) hasone=1;
  else if (cl->FLAGS==0) hasnone=1;
  */
  if (hasone)
    { 
      COUPLED_VECTOR_MODE=1;
      for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
        {
          for (cl=syms;cl!=NULL; cl=cl->NEXT)
            if (cl->FLAGS!=1 && cl->ConnectorRadical==lc->NAME) break;
            else if (cl->ConnectorRadical==fastradical(lc)/* && index==fastindex(lc)*/) break;

          if (cl!=NULL)
            found=addchain(found, lc);
        }
    }
  else
    {
      COUPLED_VECTOR_MODE=0;
      //
      if (syms->FLAGS!=1)
        {
          if (index==-1)
            {
              for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
                {
                  for (cl=syms;cl!=NULL && cl->ConnectorRadical!=lc->NAME;cl=cl->NEXT) ; // ne marche pas pour les vecteurs
                  if (cl!=NULL)
                    found=addchain(found, lc);
                }
            }
          else
            {
              //          printf("grab:");
              for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
                {
                  for (cl=syms;cl!=NULL && !(cl->ConnectorRadical==fastradical(lc) && index==fastindex(lc));cl=cl->NEXT) ; // ne marche pas pour les vecteurs
                  if (cl!=NULL)
                    {
                      found=addchain(found, lc);
                      //                  printf(" '%s'",lc->NAME);
                    }
                }
              //          printf("\n");
            }
        }
      else
        {
        avt_errmsg(GNS_ERRMSG, "117", AVT_FATAL);
//          fprintf(stderr,"coupling won't work with vectors ... yet...\n");
          EXIT(2);
        }
    }
  return found;
}

chain_list *GrabVectorConnectors(loins_list *li, SymInfoItem *syms, char *radical, int index)
{
  SymInfoItem *cl;
  chain_list *found=NULL;
  locon_list *lc;
  /*
    if (syms->FLAGS==1)
    {
  */
  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      for (cl=syms; cl!=NULL && !(cl->ConnectorRadical==fastradical(lc) && fastindex(lc)==index); cl=cl->NEXT) ; // ne marche pas pour les vecteurs
      if (cl!=NULL)
        found=addchain(found, lc);
    }
  /*    }
        else
        {
        fprintf(stderr,"implicit coupling won't work with bits\n");
        EXIT(2);
        }*/
  return found;
  radical = NULL;
}

// check if all the connectors are not fixed
int CheckConnectorFreedom(chain_list *cl) 
{
  for (;cl!=NULL;cl=cl->NEXT)
    if (isfixed((locon_list *)cl->DATA)) return 1;
  return 0;
}

chain_list *BuildCorrespondance(chain_list *me, chain_list *you)
{
  chain_list *cl=NULL;
  locon_list *found, *lc;
  chain_list *cp, *testyou=you;
  for (;me!=NULL && testyou!=NULL;me=me->NEXT,testyou=testyou->NEXT)
    {
      lc=(locon_list *)me->DATA;
      found=FindConWithSym(you, lc);
      if (found==NULL)
        {
          // zinaps : la fonction FindConWithSym fait le exit
          // cette erreur ne sert donc a rien, mais je la laisse quant meme
        avt_errmsg(GNS_ERRMSG, "118", AVT_FATAL, lc->NAME);
//          fprintf(stderr,"can not find coupled connector for '%s'\n",lc->NAME);
          EXIT(2);
        }
      cp=addchain(addchain(NULL,found), lc);
      cl=addchain(cl,cp);
    }
  if (testyou!=NULL)
    {
        avt_errmsg(GNS_ERRMSG, "116", AVT_FATAL, ((locon_list *)testyou->DATA)->NAME);
//      fprintf(stderr,"no symmetry found for connector %s in coupled connector list (2)\n",((locon_list *)testyou->DATA)->NAME);
      EXIT(2);
    }
  return cl;
}

void free_double_chain(chain_list *con_cl)
{
  chain_list *ch;
  for (ch=con_cl;ch!=NULL;ch=ch->NEXT)
    {
      freechain((chain_list *)ch->DATA);
    }
  freechain(con_cl);
}

locon_list *FindLoconWithIndex(locon_list *lc, char *radical, int index)
{

  while (lc!=NULL && !(fastindex(lc)==index && fastradical(lc)==radical))
    lc=lc->NEXT;
  return lc;
}

int CheckForCrossSymmetry(locon_list *lc, SymInfoItem *syms, char *v0, int index0, char *v1, int index1, chain_list **con_cl)
{
  SymInfoItem *ch;

  if (syms->FLAGS==1 && index1!=-1 && index0!=index1)
    {
      locon_list *pa, *pb;
      if (v0!=v1)
        {
          gen_printf(3,"symmetry : cross symmetry detected %s(%d) - %s(%d)\n", v0, index1, v1, index0);
          if ((pa=FindLoconWithIndex(lc, v0, index1))==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "119", AVT_FATAL, v0, index1, ((loins_list *)lc->ROOT)->INSNAME);
//              fprintf(stderr,"Could not find connector '%s(%d)' for instance '%s'\n", v0, index1, ((loins_list *)lc->ROOT)->INSNAME);
              EXIT(1);
            }
          if ((pb=FindLoconWithIndex(lc, v1, index0))==NULL)
            {
              avt_errmsg(GNS_ERRMSG, "119", AVT_FATAL, v1, index0, ((loins_list *)lc->ROOT)->INSNAME);
//              fprintf(stderr,"Could not find connector '%s(%d)' for instance '%s'\n", v1, index0, ((loins_list *)lc->ROOT)->INSNAME);
              EXIT(1);
            }

          if (isfixed(pa) || isfixed(pb)) 
            {
              return 0;
            }
        
          *con_cl=addchain(*con_cl, addchain(addchain(NULL,pb), pa));
        }

      // implicit coupling for the other signal in symmetric list
      for (ch=syms;ch!=NULL;ch=ch->NEXT)
        {
          if (strcmp(ch->ConnectorRadical, v0)!=0 && 
              strcmp(ch->ConnectorRadical, v1)!=0)
            {
              gen_printf(3,"symmetry : implicit coupling %s(%d) - %s(%d)\n", ch->ConnectorRadical, index0, ch->ConnectorRadical, index1);
              if ((pa=FindLoconWithIndex(lc, ch->ConnectorRadical, index1))==NULL)
                {
              avt_errmsg(GNS_ERRMSG, "119", AVT_FATAL, ch->ConnectorRadical, index1, ((loins_list *)lc->ROOT)->INSNAME);
//                  fprintf(stderr,"Could not find connector '%s(%d)' for instance '%s'\n", ch->ConnectorRadical, index1, ((loins_list *)lc->ROOT)->INSNAME);
                  EXIT(1);
                }
              if ((pb=FindLoconWithIndex(lc, ch->ConnectorRadical, index0))==NULL)
                {
              avt_errmsg(GNS_ERRMSG, "119", AVT_FATAL, ch->ConnectorRadical, index0, ((loins_list *)lc->ROOT)->INSNAME);
//                  fprintf(stderr,"Could not find connector '%s(%d)' for instance '%s'\n", ch->ConnectorRadical, index0, ((loins_list *)lc->ROOT)->INSNAME);
                  EXIT(1);
                }
              
              if (isfixed(pa) || isfixed(pb)) 
                {
                  return 0;
                }
              
              *con_cl=addchain(*con_cl, addchain(addchain(NULL,pb), pa));
            }
        }
    }
  return 1;
}
#if 0
static void bit_swapping(chain_list *bi_list, char *radi, int index0, int index1)
{
  chain_list *cl;
  chain_list *a0=NULL, *a1=NULL, *b0=NULL, *b1=NULL;
  void *temp;
  locon_list *one, *two;

  for (cl=bi_list;cl!=NULL;cl=cl->NEXT)
    {
      one=(locon_list *)((chain_list *)cl->DATA)->DATA;
      two=(locon_list *)((chain_list *)cl->DATA)->NEXT->DATA;
      if (a0==NULL)
        {
          if (fastradical(one)==radi && fastindex(one)==index0)
            {
              a0=(chain_list *)cl->DATA; a1=((chain_list *)cl->DATA)->NEXT;
            }
        }
      if (b0==NULL)
        {
          if (fastradical(one)==radi && fastindex(one)==index1)
            {
              b0=(chain_list *)cl->DATA; b1=((chain_list *)cl->DATA)->NEXT;
            }
        }
    }
  if (a0==NULL || b0==NULL) 
    {
      avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 191);
//      avt_error("gns", 1, AVT_ERR, "Internal error\n");
      EXIT(44);
    }
  temp=a0->DATA; a0->DATA=b0->DATA; b0->DATA=temp;
  //  temp=a1->DATA; a1->DATA=b1->DATA; b1->DATA=temp;
}
#endif

int TryToSwapWithAConnectorWithTheSameName(locon_list **circuit_c, locon_list *model_con, SymInfoItem *syms, int bit_wanted)
{
  chain_list *con_cl, *cl;
  SymInfoItem *ch;
  locon_list *sym, *one, *two;
  locon_list *circuit_con=*circuit_c;
  char *tmp_name;
  int force_swap=0;

  tmp_name=/*vectorradical(*/model_con->NAME/*)*/;

  ch=FindVectorRadicalSymInChain(syms, model_con);

  if (ch==NULL) return 0;

  if (!(fastradical(circuit_con)==model_con->NAME && fastindex(circuit_con)==bit_wanted))
    {
      chain_list *circuit_list, *sym_list;
      SymInfoItem *circuit_list_p, *sym_list_p;
      // search for the symmetric locon

      gen_printf(3,"symmetry (A): '%s' found instead of '%s(%d)'\n",circuit_con->NAME,model_con->NAME, bit_wanted);

      if (isfixed(circuit_con)) 
        {
          gen_printf(3,"*** fixed *** %s.%s\n",((loins_list *)circuit_con->ROOT)->INSNAME,circuit_con->NAME);
          return 0;
        } // can't be moved

      sym=FindALoconPlusIndexInChain(ch, ((loins_list *)circuit_con->ROOT)->LOCON, bit_wanted);

      if (sym==NULL) 
        {
          // not an error
          // it simply means that the instance won't match because its
          // paramaters don't match
          gen_printf(3,"connector '%s(%d)' not found\n",model_con->NAME,bit_wanted);
          return 0;
        }
      // it's fixed, we can do nothing about the swap

      if (isfixed(sym)) 
        {
          gen_printf(3,"*** fixed2 *** %s.%s",((loins_list *)sym->ROOT)->INSNAME,sym->NAME);
          return 0;
        }
      
      circuit_list_p=GetCoupledChain(circuit_con);
      sym_list_p=GetCoupledChain(sym);
      if (!((circuit_list_p==NULL && sym_list_p==NULL) || (sym_list_p!=NULL && circuit_list_p!=NULL)))
        {
              avt_errmsg(GNS_ERRMSG, "120", AVT_FATAL, circuit_con->NAME,sym->NAME);
//          fprintf(stderr,"while swapping %s and %s, one of the connector did not have coupled connector list while the other has\n",circuit_con->NAME,sym->NAME);
          EXIT(2);
        }

      if (circuit_list_p!=NULL)// && (fastindex(circuit_con)==-1 || (fastindex(circuit_con)!=-1 && fastradical(circuit_con)!=fastradical(sym))))
        {
          // there are coupled connectors
          circuit_list=GrabConnectors((loins_list *)circuit_con->ROOT, circuit_list_p, fastindex(circuit_con));
          sym_list=GrabConnectors((loins_list *)circuit_con->ROOT, sym_list_p, fastindex(sym));

          if (CheckConnectorFreedom(circuit_list) || CheckConnectorFreedom(sym_list))
            {
              freechain(circuit_list);
              freechain(sym_list);
              return 0; // at list one the connectors can't be moved
            }
          con_cl=BuildCorrespondance(circuit_list, sym_list);
          /*          if (COUPLED_VECTOR_MODE==1)
                      {
                      bit_swapping(con_cl, fastradical(circuit_con), fastindex(circuit_con), fastindex(sym));
                      force_swap=1;
                      }*/
          freechain(circuit_list);
          freechain(sym_list);
        }
      else
        {
          // we already know that the connectors can be freely moved
          con_cl=addchain(NULL, addchain(addchain(NULL,sym), circuit_con));
        }


      if (force_swap || CheckForCrossSymmetry(((loins_list *)circuit_con->ROOT)->LOCON, GetSymmetricChain(circuit_con), fastradical(circuit_con), fastindex(circuit_con), fastradical(sym), bit_wanted, &con_cl)!=0)
        {

          for (cl=con_cl;cl!=NULL;cl=cl->NEXT)
            {
              one=(locon_list *)((chain_list *)cl->DATA)->DATA;
              two=(locon_list *)((chain_list *)cl->DATA)->NEXT->DATA;
              SwapLoconWithSym(two, one);
              addswap(two, one);
            }
         
          free_double_chain(con_cl);
         
          *circuit_c=sym;
        }
      else
        {
          free_double_chain(con_cl);
          return 0;
        }

      /*      setfixed(sym);
              printf("0. marked fixed %s.%s(%s)\n",((loins_list *)sym->ROOT)->INSNAME,sym->NAME,sym->SIG->NAMECHAIN->DATA);  */
    }
  
  return 1;
}

// return the next unfixed symmetric connector
// last==NULL => begin of the search
locon_list *GetNextSymmetric(locon_list *me, locon_list *last)
{
  locon_list *syms_locon;
  SymInfoItem *syms, *symsrun;

  syms=GetSymmetricChain(me);

  if (syms==NULL) return NULL;
  if (last==NULL) last=((loins_list *)me->ROOT)->LOCON; // me->NEXT? pour etre + rapide ???
  else last=last->NEXT;

  for (syms_locon=last;syms_locon!=NULL;syms_locon=syms_locon->NEXT)
    {
      // we look in this locon is a symmetric
      char *temp_name;
      int v_you, v_me;
      
      if (syms_locon!=me)
        {
          temp_name=fastradical(syms_locon);
        
          if (syms->FLAGS!=1)
            {
              for (symsrun=syms;symsrun!=NULL;symsrun=symsrun->NEXT)
                {
                  if (symsrun->ConnectorRadical==temp_name && !isfixed(syms_locon))
                    {
                      gen_printf(3,"symmetry (B): slidingnext, choosing %s(%s) for %s\n",syms_locon->NAME,syms_locon->SIG->NAMECHAIN->DATA,me->NAME);
                      return syms_locon;   
                    }
                }
            }
          else
            {
              if ((v_me=fastindex(me))==-1) return NULL;
            
              for (symsrun=syms;symsrun!=NULL;symsrun=symsrun->NEXT)
                {
                  if ((v_you=fastindex(syms_locon))!=-1 && v_me==v_you)
                    {
                      if (symsrun->ConnectorRadical==temp_name && !isfixed(syms_locon))
                        {
                          gen_printf(3,"symmetry (Bv): slidingnext, choosing %s(%s) for %s\n",syms_locon->NAME,syms_locon->SIG->NAMECHAIN->DATA,me->NAME);
                          return syms_locon;
                        }
                    }
                }
            }
        }
      
    }
  return NULL;
}
// return the next unfixed symmetric connector
// last==NULL => begin of the search
locon_list *GetNextSymmetric2(locon_list *me, locon_list *last)
{
  locon_list *syms_locon;
  SymInfoItem *syms, *symsrun;

  syms=GetSymmetricChain(me);

  if (syms==NULL) return NULL;
  if (last==NULL) last=me->NEXT;// pour etre + rapide ???
  else last=last->NEXT;

  for (syms_locon=last;syms_locon!=NULL;syms_locon=syms_locon->NEXT)
    {
      // we look in this locon is a symmetric
      char *temp_name;
      int v_you, v_me;

      temp_name=fastradical(syms_locon);
      
      if (syms->FLAGS!=1)
        {
          for (symsrun=syms;symsrun!=NULL;symsrun=symsrun->NEXT)
            {
              if (symsrun->ConnectorRadical==temp_name && !isfixed(syms_locon))
                {
                  gen_printf(3,"symmetry (B): slidingnext, choosing %s(%s) for %s\n",syms_locon->NAME,syms_locon->SIG->NAMECHAIN->DATA,me->NAME);
                  return syms_locon;   
                }
            }
        }
      else
        {
          if ((v_me=fastindex(me))==-1) return NULL;
        
          for (symsrun=syms;symsrun!=NULL;symsrun=symsrun->NEXT)
            {
              if ((v_you=fastindex(syms_locon))!=-1 && v_me==v_you)
                {
                  if (symsrun->ConnectorRadical==temp_name && !isfixed(syms_locon))
                    {
                      gen_printf(3,"symmetry (B): slidingnext, choosing %s(%s) for %s\n",syms_locon->NAME,syms_locon->SIG->NAMECHAIN->DATA,me->NAME);
                      return syms_locon;
                    }
                }
            }
        }
    } 
  return NULL;
}
 
/*
  Echange deux connections si cela est possible
  Prend en compte les couplages
*/
int TryToSwapConnectors(locon_list *circuit_con, locon_list *sym)
{
  chain_list *ch, *con_cl;
  locon_list *one, *two;

  chain_list *circuit_list, *sym_list;
  SymInfoItem *circuit_list_p, *sym_list_p;
  
  circuit_list_p=GetCoupledChain(circuit_con);
  sym_list_p=GetCoupledChain(sym);
  if (!((circuit_list_p==NULL && sym_list_p==NULL) || (sym_list_p!=NULL && circuit_list_p!=NULL)))
    {
              avt_errmsg(GNS_ERRMSG, "120", AVT_FATAL, circuit_con->NAME,sym->NAME);
//      fprintf(stderr,"while swapping %s and %s, one of the connector did not have coupled connector list while the other has\n",circuit_con->NAME,sym->NAME);
      EXIT(2);
    }
  
  if (circuit_list_p!=NULL)
    {
      // there are coupled connectors
      circuit_list=GrabConnectors((loins_list *)circuit_con->ROOT, circuit_list_p, fastindex(circuit_con));
      sym_list=GrabConnectors((loins_list *)circuit_con->ROOT, sym_list_p, fastindex(sym));
      
      if (CheckConnectorFreedom(circuit_list) || CheckConnectorFreedom(sym_list))
        {
          freechain(circuit_list);
          freechain(sym_list);
          return 0; // at list one the connectors can't be moved
        }

      con_cl=BuildCorrespondance(circuit_list, sym_list);
      // were commented ---
      freechain(circuit_list);
      freechain(sym_list);
      // ------------------
    }
  else
    {
      // we already know that the connectors can be freely moved
      con_cl=addchain(NULL, addchain(addchain(NULL,sym), circuit_con));
    }

  if (CheckForCrossSymmetry(((loins_list *)circuit_con->ROOT)->LOCON, GetSymmetricChain(circuit_con), fastradical(circuit_con), fastindex(circuit_con), fastradical(sym), fastindex(sym), &con_cl)!=0)
    {
            
      for (ch=con_cl;ch!=NULL;ch=ch->NEXT)
        {
          one=(locon_list *)((chain_list *)ch->DATA)->DATA;
          two=(locon_list *)((chain_list *)ch->DATA)->NEXT->DATA;
          SwapLoconWithSym(two, one);
          addswap(two, one);
          // they won't be moved anymore, except if Backward() is called
        }
    
      free_double_chain(con_cl);
    }
  else
    {
      free_double_chain(con_cl);
      return 0;
    }

  /*  setfixed(circuit_con);
      printf("1. marked fixed %s.%s(%s)\n",((loins_list *)circuit_con->ROOT)->INSNAME,circuit_con->NAME,circuit_con->SIG->NAMECHAIN->DATA);  
  */
  return 1;
}

int FindMarkContaining(locon_list * lc, losig_list *ls)
{
  mark_list *mark;
  ptype_list *head;
   
  head=getptype(lc->SIG->USER,GEN_MARK_PTYPE);
  if (!head) return 0;
   
  for (mark=(mark_list*)head->DATA; mark; mark=mark->NEXT) 
    {
      if (mark->LOSIG==ls) break;
    }

  if (mark!=NULL && mark->COUNT==1) return 1;
  return 0;
}

int TryToSwapWithAConnectorWithTheSameSignalAndWithOneMark(locon_list *circuit_con, losig_list *ls, mark_list **newmark)
{
  chain_list *con_cl, *cl;
  locon_list *sym, *one, *two;
  SymInfoItem *syms, *ch;
  chain_list *circuit_list, *sym_list;
  SymInfoItem *circuit_list_p, *sym_list_p;
  mark_list *cmark, *cmark1;

  //  if (isfixed(circuit_con)) { printf("*** fixed *** %s.%s\n",((loins_list *)circuit_con->ROOT)->INSNAME,circuit_con->NAME);} // can't be moved
  //  syms=getptype(circuit_con->USER,GEN_SYM_INFO_PTYPE);

  syms=GetSymmetricChain(circuit_con);
  if (syms==NULL) return 0;

  for (sym=((loins_list *)circuit_con->ROOT)->LOCON;sym!=NULL;sym=sym->NEXT)
    {
      if (sym!=circuit_con)
        {
          ch=AreYouMySymmetric(syms, sym/*, circuit_con*/);

          if (ch!=NULL && sym->SIG==ls && !isfixed(sym))
            {
              if ((cmark=getloconmark(sym))!=NULL && cmark->COUNT==1) break;
            }
        }
    }

  if (sym==NULL) return 0;

  cmark1=getloconmark(circuit_con);

  if (sym!=NULL)
    gen_printf(3,"Symmetry (C): found candidate %s %c\n",sym->NAME,isfixed(sym)?'F':'-');
  /*
    if ((cmark=getloconmark(sym))!=NULL)
    gen_printf(3,"mark_cnt_sym= %d\n",cmark->COUNT);
  
    if ((cmark1=getloconmark(circuit_con))!=NULL)
    gen_printf(3,"mark_cnt_circuit_con= %d\n",cmark1->COUNT);
  */
  circuit_list_p=GetCoupledChain(circuit_con);
  sym_list_p=GetCoupledChain(sym);
  if (!((circuit_list_p==NULL && sym_list_p==NULL) || (sym_list_p!=NULL && circuit_list_p!=NULL)))
    {
              avt_errmsg(GNS_ERRMSG, "120", AVT_FATAL, circuit_con->NAME,sym->NAME);
      fprintf(stderr,"while swapping %s and %s, one of the connector did not have coupled connector list when the other has\n",circuit_con->NAME,sym->NAME);
      EXIT(2);
    }

  if (circuit_list_p!=NULL)
    {
      // there are coupled connectors
      circuit_list=GrabConnectors((loins_list *)circuit_con->ROOT, circuit_list_p, fastindex(circuit_con));
      sym_list=GrabConnectors((loins_list *)circuit_con->ROOT, sym_list_p, fastindex(sym));
      
      if (CheckConnectorFreedom(circuit_list) || CheckConnectorFreedom(sym_list))
        {
          freechain(circuit_list);
          freechain(sym_list);
          return 0; // at list one the connectors can't be moved
        }
      con_cl=BuildCorrespondance(circuit_list, sym_list);
      freechain(circuit_list);
      freechain(sym_list);
    }
  else
    {
      // we already know that the connectors can be freely moved
      con_cl=addchain(NULL, addchain(addchain(NULL,sym), circuit_con));
    }

  if (CheckForCrossSymmetry(((loins_list *)circuit_con->ROOT)->LOCON, syms, fastradical(circuit_con), fastindex(circuit_con), fastradical(sym), fastindex(sym), &con_cl)!=0)
    {
      for (cl=con_cl;cl!=NULL;cl=cl->NEXT)
        {
          one=(locon_list *)((chain_list *)cl->DATA)->DATA;
          if (isfixed(one)) {free_double_chain(con_cl); gen_printf(3,"no possible symmetry: '%s' is not free\n",ccname(one)); return 0;}
        
          two=(locon_list *)((chain_list *)cl->DATA)->NEXT->DATA;
          if (isfixed(one)) {free_double_chain(con_cl); gen_printf(3,"no possible symmetry: '%s' is not free\n",ccname(two)); return 0;}
        }
      
      for (cl=con_cl;cl!=NULL;cl=cl->NEXT)
        {
          one=(locon_list *)((chain_list *)cl->DATA)->DATA;
          two=(locon_list *)((chain_list *)cl->DATA)->NEXT->DATA;
          SwapLoconWithSym(two, one);
          addswap(two, one);
        }
      
      free_double_chain(con_cl);
    }
  else
    {
      free_double_chain(con_cl);
      return 0;
    }

  setfixed(circuit_con);

  gen_printf(4,"2. now fixed %s.%s(%s)\n",((loins_list *)circuit_con->ROOT)->INSNAME,circuit_con->NAME,circuit_con->SIG->NAMECHAIN->DATA);  
  *newmark=cmark1; ///*cmark; //*/getloconmark(sym);
  return 1;
}

/*
  Si un connecteur i0.A est connecte' sur un mauvais signal S0
  relie' a i1.D, s'il existe un connecteur i1.B ayant deja marque'
  le bon signal S1, on essaie d'echanger les connections de i1.B et i1.D
  s'ils sont sym
*/
int TryToSwapWithAConnectorWithTheMarkedSignal(locon_list *circuit_con, mark_list *failedmark, locon_list **last, SymInfoItem *syms)
{
  locon_list *lc;
  SymInfoItem *ch;
  //  char *temp_name;

  if (failedmark==NULL) return 0;

  do 
    {
      for (lc=(*last)->NEXT;lc!=NULL && lc->SIG!=failedmark->LOSIG; lc=lc->NEXT) ;
      if (lc!=NULL)
        {
          // we've found a locon connected to this signal,
          // now we checked if it's a sym of the locon
          *last=lc;
          if (!isfixed(lc))
            {
            
              ch=AreYouMySymmetric(syms, lc/*, circuit_con*/);

              if (ch!=NULL)
                {
                  // yes there's one, we exchange the connector signals
                  if (TryToSwapConnectors(circuit_con, lc)) return 1;
                }
            } else gen_printf(4,"-fixed %s-",lc->NAME);
        }
    } while (lc!=NULL);
  return 0;
}


/*
  Gestion des informations de symmetry et de couplage
  pour chaque modele
  Une table de hash permet de retrouver la liste des signaux
  en couplage et symmetrique a partir de:
  <nom de modele>.<nom de connecteur>
  Un systeme de cache est cense' ameliorer les performances :)
*/


ht *scht;
chain_list *myallocs;

typedef struct biinfo
{
  SymInfoItem *syms, *coupl;
  int index;
} biinfo;

#define CASHS 4
#define MAX_SYMLIST 1024

biinfo *ALL_SYM_INFO[MAX_SYMLIST];
int CUR_SYM_INDEX;

struct localcash
{
  int count;
  char *model, *con;
  biinfo *info;
} entry[CASHS];

void InitSymmetricAndCoupledInfoMecanism()
{
  int i;
  scht=addht(1024);
  for (i=0;i<CASHS;i++) {entry[i].model=NULL;entry[i].count=0;}
  myallocs=NULL;
  CUR_SYM_INDEX=1;
}

void RemoveSymmetricAndCoupledInfoMecanism()
{
  chain_list *ch;
  delht(scht);
  for (ch=myallocs;ch!=NULL;ch=ch->NEXT)
    {
      mbkfree(ch->DATA);
    }
  freechain(myallocs);
}

static char *getkey(char *a, char *b)
{
  int i;
  char r[1000];
  strcpy(r,a);
  for (i=0;r[i]!='\0' && r[i]!='_'; i++) ;
  if (r[i]=='_' && r[i+1]>='0' && r[i+1]<='9') r[i]='\0'; // 'toto_5_4' is model 'toto'
  strcat(r,".");strcat(r,b);
  return namealloc(r);
}

void addcash(char *model, char *con, biinfo *bf)
{
  int min=0, i;
  static int turn=0;
  for (i=1;i<CASHS;i++) if (entry[i].count<entry[min].count) {min=i;}
  turn++;
  if (turn>1000)
    {
      turn=0;
      for (i=0;i<CASHS;i++) entry[i].count=10;
    }
  entry[min].count=0;
  entry[min].model=model;
  entry[min].con=con;
  entry[min].info=bf;
}

biinfo *getinfo(char *model, char *con)
{
  int i;
  char *name;
  long item, key;

  for (i=0;i<CASHS;i++)
    if (entry[i].model==model && entry[i].con==con) {entry[i].count++;return entry[i].info;}
  
  name=getkey(model, con);
  key=((long)name>>2);
  item=gethtitem(scht, (void *)key);
  if (item!=EMPTYHT) {addcash(model, con, (biinfo *)item); return (biinfo *)item;}

  return NULL;
}

biinfo *getorcreateinfo(char *model, char *con)
{
  biinfo *bf;
  char *name;
  long key;
  if ((bf=getinfo(model,con))!=NULL) { return bf; }
  bf=(biinfo *)mbkalloc(sizeof(biinfo));
  bf->syms=NULL;
  bf->coupl=NULL;
  bf->index=CUR_SYM_INDEX;
  ALL_SYM_INFO[CUR_SYM_INDEX]=bf;
  CUR_SYM_INDEX++;
  if (CUR_SYM_INDEX==MAX_SYMLIST)
    {
            
              avt_errmsg(GNS_ERRMSG, "121", AVT_FATAL, MAX_SYMLIST);
//      fprintf(stderr,"INTERNAL LIMITATION: too much symmetric informations.\n\tActual limit is %d.\n", MAX_SYMLIST);
      EXIT(1);
    }
  myallocs=addchain(myallocs, bf);
  name=getkey(model, con);
  key=((long)name>>2);
  addhtitem(scht, (void *)key, (long)bf);
  return bf;
}

void addSymmetricInfo(char *figname, char *conname, SymInfoItem *syms)
{
  biinfo *bf;
  bf=getorcreateinfo(figname, vectorradical(conname));
  if (bf->syms!=NULL) return; //EXIT(112);
  bf->syms=syms;
}

void addCoupledInfo(char *figname, char *conname, SymInfoItem *coupl)
{
  biinfo *bf;
  bf=getorcreateinfo(figname, vectorradical(conname));
  if (bf->coupl!=NULL) return; //EXIT(112);
  bf->coupl=coupl;
}

SymInfoItem *GetSymmetricChain(locon_list *lc)
{
  if (lc->TYPE=='T' || lc->FLAGS==0) return NULL;
  return ALL_SYM_INFO[lc->FLAGS]->syms;
}

SymInfoItem *OldFashionGetSymmetricChain(locon_list *lc)
{
  biinfo *bf;
  bf=getinfo(((loins_list *)lc->ROOT)->FIGNAME, vectorradical(lc->NAME));
  if (bf==NULL) return NULL;
  return bf->syms;
}

void SetSymmetricChainIndexInLoconFlags(locon_list *lc)
{
  biinfo *bf;
  if (lc->TYPE=='T') { lc->FLAGS=0; return; }
  bf=getinfo(((loins_list *)lc->ROOT)->FIGNAME, vectorradical(lc->NAME));
  if (bf==NULL) { lc->FLAGS=0; return; }
  lc->FLAGS=bf->index;
}
SymInfoItem *GetCoupledChain(locon_list *lc)
{
  if (lc->TYPE=='T'|| lc->FLAGS==0) return NULL;
  return ALL_SYM_INFO[lc->FLAGS]->coupl;
}

void AddSymsOfLofig(lofig_list *lf)
{
  ptype_list *syms;
  chain_list *ch;
  locon_list *lc;
  SymInfoItem *sii, *mysyms;

  // retreive instance model
  syms=getptype(lf->USER,GEN_SYM_INFO_PTYPE);
  if (syms==NULL) return;

  for (lc=lf->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      mysyms=NULL;
      // connector has sims?
      for (ch=(chain_list *)syms->DATA;ch!=NULL;ch=ch->NEXT)
        {
          sii=FindVectorRadicalSymInChain((SymInfoItem *)ch->DATA, lc);
          if (sii!=NULL)
            {
              // found, we add the sims
              if (mysyms!=NULL) 
                {
              avt_errmsg(GNS_ERRMSG, "122", AVT_FATAL);
//                  fprintf(stderr,"AddSymsOfLofig: same signal in different symmetry list\n");
                  EXIT(1);
                }

              mysyms=(SymInfoItem *)ch->DATA;
            }
        }
      if (mysyms!=NULL)
        {
          addSymmetricInfo(lf->NAME, lc->NAME, mysyms);
        }
    }
}

void AddSymsFlagInLoinsLocon(loins_list *li)
{
  locon_list *lc;
  
  for (lc=li->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      SetSymmetricChainIndexInLoconFlags(lc);
    }
}

/****************************************************************************/
/* Add a list of signal names to each connector having coupled connectors   */
/****************************************************************************/
void AddCoupledOfLofig(lofig_list *lf)
{
  ptype_list *syms;
  chain_list *ch;
  locon_list *lc;
  SymInfoItem *sii, *mysyms;

  // retreive instance model
  syms=getptype(lf->USER,GEN_COUPLED_INFO_PTYPE);
  if (syms==NULL) return;

  for (lc=lf->LOCON;lc!=NULL;lc=lc->NEXT)
    {
      mysyms=NULL;
      // connector has sims?
      for (ch=(chain_list *)syms->DATA;ch!=NULL;ch=ch->NEXT)
        {
          for (sii=(SymInfoItem *)ch->DATA;sii!=NULL && sii->ConnectorName!=vectorradical(lc->NAME);sii=sii->NEXT) ;
          if (sii!=NULL)
            {
              // found, we add the sims
              if (mysyms!=NULL) 
                {
              avt_errmsg(GNS_ERRMSG, "123", AVT_FATAL);
//                  fprintf(stderr,"AddCoupledOfLofig: same signal in different coupled list\n");
                  EXIT(1);
                }
            
              mysyms=(SymInfoItem *)ch->DATA;
            }
        }
      if (mysyms!=NULL)
        {
          addCoupledInfo(lf->NAME, lc->NAME, mysyms);
        }
    }
}

void AddRadicalInfoInPNODE(loins_list *li)
{
  locon_list *lc;
  Pnode2Radical *r;

  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      r=(Pnode2Radical *)&lc->PNODE;
      r->radindex=getradindex(vectorradical(lc->NAME));
      r->index=vectorindex(lc->NAME);
    }
}

void AddRadicalInfoInLOCON(locon_list *lc, int radindex, int index)
{
  Pnode2Radical *r;

  r=(Pnode2Radical *)&lc->PNODE;
  r->radindex=radindex;
  r->index=index;
}

void ComputeWeightsForLoinsConnectors(lofig_list *lf)
{
  long sp=50+curradindex+100, cc, coupl=1, p;
  int index;
  loins_list *li;
  locon_list *lc, *lc0;
  SymInfoItem *si;
  chain_list *circuit_list, *cl;
  ptype_list *p0;
  char *temp0;
  if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
    printf("compute weight on %s\n",lf->NAME);
  
  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    {   
      p=sp;
      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT) lc->FLAGS=0;

      if ((p0=getptype (li->USER, GEN_ORIG_LOINS_PTYPE))==NULL)
        {
          if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
            printf(" (1)%s *%s* %s\n",li->INSNAME,li->FIGNAME, li->FIGNAME);
          AddRadicalInfoInPNODE(li);
          AddSymsFlagInLoinsLocon(li);
        }
      else
        {
          temp0=li->FIGNAME;
          li->FIGNAME=((loins_list *)p0->DATA)->FIGNAME;
          if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
            printf(" (2)%s *%s* %s\n",li->INSNAME,li->FIGNAME, ((loins_list *)p0->DATA)->FIGNAME);
          AddRadicalInfoInPNODE(li);
          AddSymsFlagInLoinsLocon(li);
          li->FIGNAME=temp0;
        }
      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          si=GetSymmetricChain(lc);
          if (si!=NULL) 
            {
              for (index=1;index<CUR_SYM_INDEX && ALL_SYM_INFO[index]->syms!=si; index++) ;
              if (index>=CUR_SYM_INDEX) EXIT(124);
              cc=50+index;
            }
          else cc=p++;

          lc->USER=addptype(lc->USER, FCL_WEIGHT_PTYPE, (void *)cc);
          if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
            printf("  w: %s.%s %ld\n",li->INSNAME,lc->NAME,cc);

          if (getptype(lc->USER, FCL_COUPLING_PTYPE)!=NULL) continue;

          if (si!=NULL)
            {
              circuit_list=NULL;
              if (si->FLAGS==1)
                {
                  circuit_list=GrabVectorConnectors(li, si, fastradical(lc), fastindex(lc));
                }
              else
                if ((si=GetCoupledChain(lc))!=NULL)
                  {
                    circuit_list=GrabConnectors(li, si, fastindex(lc));
                  }

              if (circuit_list!=NULL)
                {
                  for (cl=circuit_list; cl!=NULL; cl=cl->NEXT)
                    {
                      lc0=(locon_list *)cl->DATA;

                      if (getptype(lc->USER, FCL_COUPLING_PTYPE)==NULL)
                        {
                          lc0->USER=addptype(lc0->USER, FCL_COUPLING_PTYPE, (void *)coupl);
                          if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
                            printf("  cpl: %s %ld\n",lc0->NAME,coupl);
                        }
                    }
                  freechain(circuit_list);
                  coupl++;
                }
            }
        }

      for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT) 
        {
          lc->FLAGS=(short)(long)getptype(lc->USER, FCL_WEIGHT_PTYPE)->DATA;
          lc->USER=delptype(lc->USER, FCL_WEIGHT_PTYPE);
          lc->PNODE=NULL;
        }
    }

}

static int compare_locon(const void *a, const void *b)
{
  locon_list *a0=*(locon_list **)a, *b0=*(locon_list **)b;
  if (fastindex(a0)<fastindex(b0)) return -1;
  else if (fastindex(a0)>fastindex(b0)) return 1;
  EXIT(4); // a enlever
  return 0;
}

static int compare_losig(const void *a, const void *b)
{
  losig_list *a0=*(losig_list **)a, *b0=*(losig_list **)b;
  int ia, ib;
  ia=vectorindex((char *)a0->NAMECHAIN->DATA);
  ib=vectorindex((char *)b0->NAMECHAIN->DATA);
  if (ia<ib) return -1;
  if (ia>ib) return 1;
  return 0;
}

void TrytoArrangeSymmetricConnectors(loins_list *li)
{
  locon_list *lc, *next, *end_lc, *run;
  int nb, i;
  SymInfoItem *si;
  locon_list **lcs;
  losig_list **lss;
  char *sigradical;

  for (lc=li->LOCON; lc!=NULL; lc=next)
    {
      next=lc->NEXT;
      if ((si=GetSymmetricChain(lc))==NULL) continue;
      for (nb=1, end_lc=lc, run=lc->NEXT; 
           run!=NULL && fastradical(lc)==fastradical(run)
             && GetSymmetricChain(run)==si; nb++, end_lc=run, run=run->NEXT) ;
      next=run;
      if (nb==1) continue;
      lcs=(locon_list **)mbkalloc(nb*sizeof(locon_list *));
      lss=(losig_list **)mbkalloc(nb*sizeof(losig_list *));
      i=0;
      sigradical=vectorradical((char *)lc->SIG->NAMECHAIN->DATA);
      do
        {
          lcs[i]=lc;
          lss[i]=lc->SIG;
          if (vectorindex((char *)lc->SIG->NAMECHAIN->DATA)==-1
              || vectorradical((char *)lc->SIG->NAMECHAIN->DATA)!=sigradical
              )
            {
              break;
            }
          i++; lc=lc->NEXT;
        } while (lc!=next);
      
      if (lc==next)
        {
          qsort(lcs, nb, sizeof(locon_list *), compare_locon);
          qsort(lss, nb, sizeof(losig_list *), compare_losig);
          
          for (i=1; i<nb && vectorindex((char *)lss[i]->NAMECHAIN->DATA)==vectorindex((char *)lss[i-1]->NAMECHAIN->DATA)+1; i++) ;
          if (i==nb)
            {
              for (i=0; i<nb; i++)
                {
                  lcs[i]->SIG=lss[i];
              //    printf("con %s -> sig %s\n", lcs[i]->NAME, (char *)lss[i]->NAMECHAIN->DATA);
                }
             // printf("---\n");
            }
        }
      mbkfree(lcs); mbkfree(lss);
    }

}

typedef struct symentry
{
  struct symentry *next;
  int nb;
  locon_list **tab;
} symentry;

typedef struct
{
  locon_list *lc;
  ptype_list *user;
  losig_list *ls;
} MatElem;

typedef struct
{
  int width, height;
  int choice;
  MatElem *table;  
} SymMat;

chain_list *BuildMatrices(chain_list *groups)
{
  chain_list *cl, *ch;
  int w, h, i;
  symentry *se, *se0;
  SymMat *sm;

  for (cl=groups, ch=NULL; cl!=NULL; cl=cl->NEXT)
    {
      se=(symentry *)cl->DATA;
      w=se->nb;
      for (se0=se, h=0; se0!=NULL && se0->nb==w; se0=se0->next, h++) ;
      if (se0!=NULL) EXIT(6);
      sm=(SymMat *)mbkalloc(sizeof(SymMat));
      sm->width=w; sm->height=h;
      sm->table=(MatElem *)mbkalloc(sizeof(MatElem)*w*h);
      ch=addchain(ch, sm);

      for (se0=se, h=0; se0!=NULL; se0=se0->next, h++)
        {
          for (i=0; i<sm->width; i++)
            {
              sm->table[h*sm->width+i].lc=se0->tab[i];
              sm->table[h*sm->width+i].ls=se0->tab[i]->SIG;
              sm->table[h*sm->width+i].user=se0->tab[i]->USER;
            }
          mbkfree(se0->tab);
        }
      
      for (se0=se; se0!=NULL; se0=se)
        {
          se=se0->next;
          mbkfree(se0);
        }
    }
  freechain(groups);
  return ch;
}


chain_list *GetLoinsSymmetryTables(loins_list *li)
{
  long l;
  int index, count/*, cc=100*/;
  locon_list *lc, *lc0;
  SymInfoItem *si;
  chain_list *circuit_list, *cl;
  char *temp0;
  ht *groups;
  symentry *group_list, *se;

  groups=addht(10);

  
  //  printf("(1) *%s* %s\n",li->INSNAME, li->FIGNAME);
  temp0=li->FIGNAME;
  li->FIGNAME=modelradical(li->FIGNAME);
  AddRadicalInfoInPNODE(li);
  AddSymsFlagInLoinsLocon(li);
  li->FIGNAME=temp0;

  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      if (fastindex(lc)!=-1 && getptype(lc->USER, FCL_WEIGHT_PTYPE)==NULL)
        {
          si=GetSymmetricChain(lc);
          if (si!=NULL) 
            {
              for (index=1;index<CUR_SYM_INDEX && ALL_SYM_INFO[index]->syms!=si; index++) ;
              if (index>=CUR_SYM_INDEX) EXIT(124);
              
              if ((l=gethtitem(groups, (void *)(long)index))!=EMPTYHT)
                group_list=(symentry *)l;
              else
                group_list=NULL;

              circuit_list=GrabVectorConnectors(li, si, fastradical(lc), fastindex(lc));
              /*              if (si->FLAGS==1)
                              {
                              circuit_list=GrabVectorConnectors(li, si, fastradical(lc), fastindex(lc));
                              }
                              else
                              if ((si=GetCoupledChain(lc))!=NULL)
                              {
                              circuit_list=GrabConnectors(li, si, fastindex(lc));
                              }
                              else
                              circuit_list=addchain(NULL, lc);
              */
              for (cl=circuit_list, count=0; cl!=NULL; cl=cl->NEXT, count++) ;
              se=(symentry *)mbkalloc(sizeof(symentry));
              se->nb=count;
              se->tab=mbkalloc(sizeof(locon_list *)*count);
              for (cl=circuit_list, count=0; cl!=NULL; cl=cl->NEXT, count++)
                {
                  lc0=(locon_list *)cl->DATA;
                  lc0->USER=addptype(lc0->USER, FCL_WEIGHT_PTYPE, NULL);
                  se->tab[count]=lc0;
                }
              
              se->next=group_list;
              group_list=se;
              addhtitem(groups, (void *)(long)index, (long)group_list);
              freechain(circuit_list);
            }
        }
    }

  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT) 
    {
      if (getptype(lc->USER, FCL_WEIGHT_PTYPE)!=NULL)
        lc->USER=delptype(lc->USER, FCL_WEIGHT_PTYPE);
      lc->PNODE=NULL;
    }

  //  for (lc=li->LOCON, cc=1; lc!=NULL; lc=lc->NEXT, cc++) lc->FLAGS=cc;

  cl=GetAllHTElems(groups);
  delht(groups);
  return cl;
}

static int SORT_INDEX=0;
#define _D_LIMITS 100000

void disp_mat(int w, int h, MatElem *tab)
{
  int i, j;
  printf("-- start -- w=%d -- h=%d --\n",w,h);
  for (j=0;j<_D_LIMITS && j<h;j++)
    {
      for (i=0;i<_D_LIMITS && i<w;i++)
        {
          printf("%10s (%10s)",tab[j*w+i].lc->NAME,(char *)tab[j*w+i].lc->SIG->NAMECHAIN->DATA);
        }
      printf("\n");
    }
  printf("-- end --\n");
}

static int advanced_compare(char *a, char *b)
{
  char *ai=a, *bi=b;
  char *aend, *bend;
  int av, bv;
  
  while (*ai!='\0' && *bi!='\0')
    {
      if (*ai>='0' && *ai<='9' && *bi>='0' && *bi<='9')
        {
          av=strtol(ai, &aend, 10);
          bv=strtol(bi, &bend, 10);
          if (av<bv) return -1;
          else if (av>bv) return 1;
          ai=aend;
          bi=bend;
        }
      else
        if (*ai<*bi) return -1;
        else if (*ai>*bi) return 1;
        else ai++, bi++;
    }
  return strcmp(ai,bi);
}

//#define BRUTE_FORCE
//#define BETTER

static int compare_mat_line(const void *a, const void *b)
{
  MatElem *a0=(MatElem *)a, *b0=(MatElem *)b;
  locon_list *lca, *lcb;
#ifdef BRUTE_FORCE
  lca=a0[SORT_INDEX].lc;
  lcb=b0[SORT_INDEX].lc;
  //  printf("%s - %s\n",(char *)lca->SIG->NAMECHAIN->DATA,(char *)lcb->SIG->NAMECHAIN->DATA);
  if (strcmp((char *)lca->SIG->NAMECHAIN->DATA, (char *)lcb->SIG->NAMECHAIN->DATA)<0) return -1;
  if (strcmp((char *)lca->SIG->NAMECHAIN->DATA, (char *)lcb->SIG->NAMECHAIN->DATA)>0) return 1;  
  return 0;
#else
#ifdef BETTER
  char *a_rad, *b_rad;
  int a_index, b_index;
  lca=a0[SORT_INDEX].lc;
  lcb=b0[SORT_INDEX].lc;
  a_rad=vectorradical((char *)lca->SIG->NAMECHAIN->DATA);
  b_rad=vectorradical((char *)lcb->SIG->NAMECHAIN->DATA);
  if (strcmp(a_rad, b_rad)<0) return -1;
  else if (strcmp(a_rad, b_rad)>0) return 1;
  a_index=vectorindex((char *)lca->SIG->NAMECHAIN->DATA);
  b_index=vectorindex((char *)lcb->SIG->NAMECHAIN->DATA);
  if (a_index<b_index) return -1;
  else if (a_index>b_index) return 1;
  return 0;
#else
  lca=a0[SORT_INDEX].lc;
  lcb=b0[SORT_INDEX].lc;
  return advanced_compare((char *)lca->SIG->NAMECHAIN->DATA, (char *)lcb->SIG->NAMECHAIN->DATA);
#endif
#endif
}
#if 0
static void swap_locon(locon_list *syms_locon, locon_list *circuit_con)
{
  losig_list *tmpsig;
  ptype_list *user;

#ifdef ZINAPS_DUMP
  //  if (GEN_DEBUG_LEVEL>3)
  //    gen_printf(-1,"sort: for %s Swapping connector %s(%s) %s(%s)\n",((loins_list *)circuit_con->ROOT)->INSNAME,syms_locon->NAME,syms_locon->SIG->NAMECHAIN->DATA,circuit_con->NAME,circuit_con->SIG->NAMECHAIN->DATA);
#endif  

  tmpsig=syms_locon->SIG; syms_locon->SIG=circuit_con->SIG; circuit_con->SIG=tmpsig;
  user=syms_locon->USER; syms_locon->USER=circuit_con->USER; circuit_con->USER=user;
  
  // les LOFIGCHAINs doivent etre updatées

  fastswaplofigchain(syms_locon->SIG, circuit_con, syms_locon, 0);
  fastswaplofigchain(circuit_con->SIG, syms_locon, circuit_con, 0);

}
#endif

void ArrangeInstanceConnectors(loins_list *li)
{
  chain_list *matrices, *cl;
  MatElem *duptable;
  locon_list *orig;
  SymMat *sm;
  int i,j;
  
  matrices=BuildMatrices(GetLoinsSymmetryTables(li));
  
  for (cl=matrices; cl!=NULL; cl=cl->NEXT)
    {
      sm=(SymMat *)cl->DATA;
      duptable=(MatElem *)mbkalloc(sm->width*sm->height*sizeof(MatElem));
      memcpy(duptable, sm->table, sm->width*sm->height*sizeof(MatElem));
      
      SORT_INDEX=0; // devrait passer par un appel de fonction pour etre determine

      if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
        disp_mat(sm->width, sm->height, duptable);

      qsort(duptable, sm->height, sm->width*sizeof(MatElem), compare_mat_line);
      
      if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
        disp_mat(sm->width, sm->height, duptable);

      for (j=0; j<sm->height; j++)
        {
          for (i=0; i<sm->width; i++)
            {
              orig=sm->table[j*sm->width+i].lc;
              if (orig!=duptable[j*sm->width+i].lc
                  && sm->table[j*sm->width+i].ls!=duptable[j*sm->width+i].ls
                  )
                {
                  //              dest=duptable[j*sm->width+i].lc;
                  
                  orig->SIG=duptable[j*sm->width+i].ls;
                  orig->USER=duptable[j*sm->width+i].user;
                  if ((GEN_OPTIONS_PACK & GEN_DEBUG_REMAPPING)!=0)
                    printf("%s -> %s\n",orig->NAME,(char*) orig->SIG->NAMECHAIN->DATA);
                  fastswaplofigchain(orig->SIG, duptable[j*sm->width+i].lc, orig, 1);
                }
            }
        }

      mbkfree(duptable);
      mbkfree(sm->table);
mbkfree(sm);
    }

  freechain(matrices);
}
