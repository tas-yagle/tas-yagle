#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include MUT_H
#include SLB_H
#include CGV_H

chain_list *box_in_path, *net_in_path;
static int AUTOMARK_REVERSE=0, lastcut=1;

void search(cgvbox_list *depart, int d, int depth, int dir, markobject mark)
{
  cgvcon_list *c, *con0;
  chain_list *cl;
  if (d!=0 && ((depth>=0 && d>depth) || (depart->misc.count!=-1 && depart->misc.count<=d))) return;
  if (IsCgvBoxTransparence(depart)) return;
  if (depart->misc.count==-1/* && !IsCgvBoxTransparence(depart)*/)
    {
//      printf("B:%s\n",depart->NAME);
      if (mark!=NULL) mark(depart->USER);
      depart->misc.count=d;
      for (cl=box_in_path; cl!=NULL && cl->DATA!=depart; cl=cl->NEXT) ;
      if (cl==NULL) box_in_path=addchain(box_in_path, depart);
    }
  if ((dir & CGV_SEL_PREC_LOGIC) != 0) c=depart->CON_IN; else c=depart->CON_OUT;
  if (AUTOMARK_REVERSE)
    {
      if ((dir & CGV_SEL_PREC_LOGIC) != 0) con0=depart->CON_OUT; else con0=depart->CON_IN;
      while (con0!=NULL)
        {
          for (cl=net_in_path; cl!=NULL && cl->DATA!=con0->NET; cl=cl->NEXT) ;
          if (cl==NULL)
            {
              net_in_path=addchain(net_in_path, con0->NET);
              if (mark!=NULL && con0->NET->USER!=NULL) mark(con0->NET->USER);
            }
          con0=con0->NEXT;
        }

    }
  while (c!=NULL)
    {
      for (cl=net_in_path; cl!=NULL && cl->DATA!=c->NET; cl=cl->NEXT) ;
      if (cl==NULL)
        {
          net_in_path=addchain(net_in_path, c->NET);
          if (mark!=NULL && c->NET->USER!=NULL) mark(c->NET->USER);
        }
      for (cl = c->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
        {
          con0 = (cgvcon_list *) cl->DATA;
          if (con0!=c && !IsCgvConExternal(con0) &&
              (((dir & CGV_SEL_PREC_LOGIC)!=0 && IsCgvConOut(con0))
               || ((dir & CGV_SEL_SUCC_LOGIC)!=0 && IsCgvConIn(con0)))
              ) 
            {
              search((cgvbox_list *)con0->ROOT, d+1, depth, dir, mark);
            }
          if (con0!=c && IsCgvConExternal(con0) && mark!=NULL) 
            {
              mark(con0->USER);
//              printf("EC:%s\n",con0->NAME);
            }
        }
      c=c->NEXT;
    }
}

void cgv_hilite(cgvfig_list *Figure, cgvbox_list *box, int mode, int depth, markobject mark)
{
  cgvbox_list *b;
//  printf("depth=%d mode=%d p=%d n=%d\n",depth,mode,CGV_SEL_PREC_LOGIC,CGV_SEL_SUCC_LOGIC);
  box_in_path=net_in_path=NULL;
  for (b = Figure->BOX; b != NULL; b = b->NEXT) b->misc.count=-1;

  if ((mode & CGV_SEL_PREC_LOGIC)!=0)
    search(box, 0, depth, CGV_SEL_PREC_LOGIC, mark);
  if ((mode & CGV_SEL_SUCC_LOGIC)!=0)
    search(box, 0, depth, CGV_SEL_SUCC_LOGIC, mark);
  freechain(box_in_path);
  freechain(net_in_path);
}

void cgv_hilite_net(cgvfig_list *Figure, cgvcon_list *c, int mode, int depth, markobject mark)
{
//  cgvbox_list *b;
  chain_list *cl;
  cgvcon_list *con0;

  if (mark!=NULL && c->NET->USER) mark(c->NET->USER);
  net_in_path=addchain(net_in_path, c->NET);
  mark(c->USER);
  for (cl = c->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
    {
      con0 = (cgvcon_list *) cl->DATA;
      if (!IsCgvConExternal(con0))
        {
          cgv_hilite(Figure, (cgvbox_list *)con0->ROOT, mode, depth, mark);
        }
    }

}

static int reset=1;

void cgv_extract(cgvfig_list *Figure, cgvbox_list *box, int mode, int depth)
{
  cgvbox_list *b;
//  printf("extract: depth=%d mode=%d p=%d n=%d\n",depth,mode,CGV_SEL_PREC_LOGIC,CGV_SEL_SUCC_LOGIC);
  if (reset)
    {
      box_in_path=net_in_path=NULL;
      for (b = Figure->BOX; b != NULL; b = b->NEXT) b->misc.count=-1;
    }

  AUTOMARK_REVERSE=1;
  if ((mode & CGV_SEL_PREC_LOGIC)!=0)
    {
      search(box, 0, depth, CGV_SEL_PREC_LOGIC, NULL);
//      if ((mode & CGV_SEL_SUCC_LOGIC)==0) search(box, 0, 0, CGV_SEL_SUCC_LOGIC, NULL);
    }
  if ((mode & CGV_SEL_SUCC_LOGIC)!=0)
    {
      search(box, 0, depth, CGV_SEL_SUCC_LOGIC, NULL);
//      if ((mode & CGV_SEL_PREC_LOGIC)==0) search(box, 0, 0, CGV_SEL_PREC_LOGIC, NULL);
    }
  AUTOMARK_REVERSE=0;
  
}

void cgv_extract_net(cgvfig_list *Figure, cgvcon_list *c, int mode, int depth)
{
  cgvbox_list *b;
  chain_list *cl;
  cgvcon_list *con0;

  box_in_path=net_in_path=NULL;
  for (b = Figure->BOX; b != NULL; b = b->NEXT) b->misc.count=-1;


  reset=0;
  for (cl = c->NET->CON_NET; cl!=NULL; cl=cl->NEXT) 
    {
      con0 = (cgvcon_list *) cl->DATA;
      if (!IsCgvConExternal(con0))
        {
          cgv_extract(Figure, (cgvbox_list *)con0->ROOT, mode, depth);
        }
    }
  reset=1;
}


cgvcon_list *dupcgvcon(cgvcon_list *lst)
{
  cgvcon_list *b;
  if (lst==NULL) return NULL;
  b=(cgvcon_list *)mbkalloc(sizeof(cgvcon_list));
  memcpy(b, lst, sizeof(cgvcon_list));
  b->USER=lst;
  b->NEXT=dupcgvcon(lst->NEXT);
  return b;
}
cgvcon_list *duponecon(cgvcon_list *lst)
{
  cgvcon_list *b;
  b=(cgvcon_list *)mbkalloc(sizeof(cgvcon_list));
  memcpy(b, lst, sizeof(cgvcon_list));
  b->USER=lst;
  b->NEXT=NULL;
  return b;
}

cgvfig_list *finish_extract(cgvfig_list *parent)
{
  cgvfig_list *ncgv;
  chain_list *cl, *ch, *pred, *next;
  cgvbox_list *b, *nb;
  cgvnet_list *n, *nn;
  cgvcon_list *c, *c0, *unselcon;
  char name[1024];
  int done, state;

  ncgv=(cgvfig_list *)mbkalloc(sizeof(cgvfig_list));
  memcpy(ncgv, parent, sizeof(cgvfig_list));

  sprintf(name,"%s+", ncgv->NAME);
  ncgv->NAME=namealloc(name);

  ncgv->CON_IN=NULL; ncgv->NUMBER_IN=0;
  ncgv->CON_OUT=NULL; ncgv->NUMBER_OUT=0;
  ncgv->data0=ncgv->data1=ncgv->data2=ncgv->data_ZOOM=NULL;
  ncgv->NET=NULL;
  ncgv->BOX=NULL;
  for (cl=box_in_path; cl!=NULL; cl=cl->NEXT)
    {
      b=(cgvbox_list *)cl->DATA;
      nb=(cgvbox_list *)mbkalloc(sizeof(cgvbox_list));
      memcpy(nb, b, sizeof(cgvbox_list));
      nb->CON_IN=dupcgvcon(b->CON_IN);
      for (c0=nb->CON_IN; c0!=NULL; c0=c0->NEXT)
        {
          for (ch=net_in_path; ch!=NULL && ch->DATA!=c0->NET; ch=ch->NEXT) ;
          if (ch==NULL)
            {
              c0->NET=addcgvnet(ncgv);
              c0->ROOT=nb;
              c0->USER=NULL;
              c0->NET->CON_NET=addchain(c0->NET->CON_NET, c0);
            }
        }
      nb->CON_OUT=dupcgvcon(b->CON_OUT);
      for (c0=nb->CON_OUT; c0!=NULL; c0=c0->NEXT)
        {
          for (ch=net_in_path; ch!=NULL && ch->DATA!=c0->NET; ch=ch->NEXT) ;
          if (ch==NULL)
            {
              c0->NET=addcgvnet(ncgv);
              c0->ROOT=nb;
              c0->USER=NULL;
              c0->NET->CON_NET=addchain(c0->NET->CON_NET, c0);
            }
        }
      nb->FLAGS=0;
      nb->USER=NULL;
      b->tmp=nb;
      nb->NEXT=ncgv->BOX;
      ncgv->BOX=nb;
/*      sprintf(name,"%s cut #x", nb->NAME);
      nb->NAME=namealloc(name);*/
//      printf("-box-> %s \n",nb->NAME);
    }

  for (cl=net_in_path; cl!=NULL; cl=cl->NEXT)
    {
      n=(cgvnet_list *)cl->DATA;
      nn=(cgvnet_list *)mbkalloc(sizeof(cgvnet_list));
      memcpy(nn, n, sizeof(cgvnet_list));
      n->UNDER=nn;
      nn->CON_NET=dupchainlst(n->CON_NET);
      nn->WIRE = NULL;
      nn->FLAGS = 0;
      nn->NUMBER_IN = 0;
      nn->NUMBER_OUT = 0;
      nn->CANAL = 0;
      nn->USER = NULL;

      nn->NEXT=ncgv->NET;
      ncgv->NET=nn;
//      printf("--- new net\n");
      done=state=0;

      for (ch=nn->CON_NET; ch!=NULL; ch=ch->NEXT)
        {
          c=(cgvcon_list *)ch->DATA;
          if (IsCgvConExternal(c))
            {
              if (IsCgvConIn(c)) state|=CGV_CON_OUT;
              else state|=CGV_CON_IN;
            }
          else
            {
              b=(cgvbox_list *)c->ROOT;
              if (!IsCgvBoxTransparence(b) && b->misc.count!=-1)
                state|=c->TYPE;
            }
        }

      for (ch=nn->CON_NET, pred=NULL; ch!=NULL; ch=next)
        {
          next=ch->NEXT;          
          c=(cgvcon_list *)ch->DATA;
          if (IsCgvConExternal(c))
            {
              c0=duponecon(c);
              c0->NET=nn;
              ch->DATA=c0;
              c0->ROOT=ncgv;
              c0->USER=NULL;
              c0->FLAGS=0;
              if (IsCgvConIn(c)) 
                {
                  c0->TYPE=CGV_CON_IN;
                  c0->NEXT=ncgv->CON_IN; ncgv->CON_IN=c0;
                  ncgv->NUMBER_IN++;
                }
              else
                {
                  c0->TYPE=CGV_CON_OUT;
                  c0->NEXT=ncgv->CON_OUT; ncgv->CON_OUT=c0;
                  ncgv->NUMBER_OUT++;
                }
              SetCgvConExternal(c0);
              if (IsCgvConFake(c)) SetCgvConFake(c0);
//              printf("external %s to %s\n",c->NAME,c0->NAME);
            }
          else
            {
              b=(cgvbox_list *)c->ROOT;
              if (IsCgvBoxTransparence(b) || (b->misc.count==-1 && done))
                {
                  if (pred==NULL) 
                    {
                      nn->CON_NET=ch->NEXT;
                    }
                  else pred->NEXT=ch->NEXT;
                  ch->NEXT=NULL;
                  freechain(ch);                  
//                  printf("transparence %s.%s removed\n",b->NAME,c->NAME);
                  continue;
                }
              else
                {
                  if (b->misc.count==-1)
                    {
                      
                      c0=duponecon(c);
                      c0->NET=nn;
                      ch->DATA=c0;
                      c0->FLAGS=0;                      
                      c0->ROOT=ncgv;
                      c0->USER=NULL;

                      if ((IsCgvConIn(c0) && (state & CGV_CON_OUT)==0)
                          || (IsCgvConOut(c0) && (state & CGV_CON_IN)==0))
                        {
                          if (IsCgvConOut(c0)) 
                            {
                              c0->TYPE=CGV_CON_OUT;
                              c0->NEXT=ncgv->CON_OUT; ncgv->CON_OUT=c0;
                              ncgv->NUMBER_OUT++;
                            }
                          else
                            {
                              c0->TYPE=CGV_CON_IN;
                              c0->NEXT=ncgv->CON_IN; ncgv->CON_IN=c0;
                              ncgv->NUMBER_IN++;
                            }                                                    
                        }
                      else
                        {
                          if (IsCgvConIn(c0)) 
                            {
                              c0->TYPE=CGV_CON_OUT;
                              c0->NEXT=ncgv->CON_OUT; ncgv->CON_OUT=c0;
                              ncgv->NUMBER_OUT++;
                            }
                          else
                            {
                              c0->TYPE=CGV_CON_IN;
                              c0->NEXT=ncgv->CON_IN; ncgv->CON_IN=c0;
                              ncgv->NUMBER_IN++;
                            }
                        }
                      SetCgvConFake(c0);
                      SetCgvConExternal(c0);
                      c0->X_REL = 0;
                      sprintf(name,"%s.%s",b->NAME,c0->NAME);
                      unselcon=c0;
                      c0->NAME=namealloc(name);
//                      printf("external tagged box %s.%s %s\n",b->NAME,c->NAME,c0->TYPE==CGV_CON_IN?"in":"out");
                      done++;
                    }
                  else
                    {
//                      printf("internal box %s.%s %s\n",b->NAME,c->NAME,c0->TYPE==CGV_CON_IN?"in":"out");
                      if (IsCgvConIn(c)) state|=1;
                      else state|=2;

                      if (IsCgvConIn(c)) c0=b->tmp->CON_IN;
                      else c0=b->tmp->CON_OUT;

                      while (c0!=NULL && c0->USER!=c) c0=c0->NEXT;
                      if (c0==NULL) 
              {
                avt_errmsg(CGV_ERRMSG, "002", AVT_FATAL, 3);
                //fprintf(stderr,"internal: connector  %s.%s not found\n",b->NAME,c->NAME);
                EXIT(1);
              }
                      
                      c0->NET=nn;
                      ch->DATA=c0;
                      c0->ROOT=b->tmp;
                      c0->USER=NULL;
                    }
                } 
            }
          pred=ch;
        }   
    }
  for (b=ncgv->BOX; b!=NULL; b=b->NEXT)
    {
      b->misc.UNDER=NULL;
    }

  freechain(box_in_path);
  freechain(net_in_path);
  addcgvfile(ncgv->NAME, ncgv->SOURCE_TYPE, lastcut++, NULL, ncgv, parent->SOURCE, NULL, 0);
  CGV_WAS_PRESENT=0;
  buildcgvfig(ncgv);
  strcpy(getcgvfileext(ncgv), getcgvfileext(parent));
  return ncgv;
}

static char *vectname(char *name)
{
  char temp[1024];
  int i;
  if (name==NULL) return NULL;
  i=vectorindex(name);
  if (i==-1) return name;
  sprintf(temp,"%s[%d]",vectorradical(name),i);
  return namealloc(temp);
}

void complete_extract_list(cgvfig_list *cgvf, cgv_interaction *itr, int complete, markobject mark)
{
  cgv_interaction *it;
  cgvbox_list *b;
  cgvnet_list *n;
  cgvcon_list *c;
  chain_list *cl;
  ht *netht, *boxht;

 box_in_path=net_in_path=NULL;
 for (b = cgvf->BOX; b != NULL; b = b->NEXT) b->misc.count=-1;

 AUTOMARK_REVERSE=1;

#if 1
  boxht=addht(12000);

  for (b=cgvf->BOX; b!=NULL; b=b->NEXT)
    {
      b->misc.count=0;
      if (b->NAME!=NULL)
        {
          addhtitem(boxht, b->NAME, (long)b);
          addhtitem(boxht, vectname(b->NAME), (long)b);
        }
    }

  for (it=itr; it!=NULL; it=it->NEXT)
    {
      if (it->object_type==CGV_BOX_TYPE && (b=(cgvbox_list *)gethtitem(netht, it->name))!=(cgvbox_list *)EMPTYHT)
        {
          search(b, 0, 0, CGV_SEL_SUCC_LOGIC, mark); //<-fonc
        }
    }
  delht(boxht);

  netht=addht(12000);

  for (n=cgvf->NET; n!=NULL; n=n->NEXT)
    {
      if (n->NAME!=NULL)
        {
          addhtitem(netht, n->NAME, (long)n);
          addhtitem(netht, vectname(n->NAME), (long)n);
        }
    }

  for (it=itr; it!=NULL; it=it->NEXT)
    {
      if (it->object_type==CGV_NET_TYPE && (n=(cgvnet_list *)gethtitem(netht, it->name))!=(cgvnet_list *)EMPTYHT)
//          (n->NAME!=NULL && (it->name==n->NAME || it->name==vectname(n->NAME))))
        {
          for (cl = n->CON_NET; cl!=NULL; cl=cl->NEXT) 
            {
              c = (cgvcon_list *) cl->DATA;
              if (!IsCgvConExternal(c))
                {
                  b=(cgvbox_list *)c->ROOT;
                  b->misc.count++;
                }
            }
          if (mark!=NULL && n->USER!=NULL) mark(n->USER);
          for (cl=net_in_path; cl!=NULL && cl->DATA!=n; cl=cl->NEXT) ;
          if (cl==NULL) net_in_path=addchain(net_in_path, n);
          // |_ fonc
        }
    }

  if (complete)
    {
      for (b=cgvf->BOX; b!=NULL; b=b->NEXT)
        {
          if (b->misc.count>=2)
            {
              for (cl=box_in_path; cl!=NULL && cl->DATA!=b; cl=cl->NEXT) ;
              if (cl==NULL && !IsCgvBoxTransparence(b)) 
                {
                  box_in_path=addchain(box_in_path, b);
                  if (mark!=NULL) mark(b->USER);
                }
            }
          else 
            b->misc.count=-1;
        }
    }

  delht(netht);
#else

  for (b=cgvf->BOX; b!=NULL; b=b->NEXT)
    {
      b->misc.count=0;
      for (it=itr; it!=NULL; it=it->NEXT)
        if (it->object_type==CGV_BOX_TYPE && (it->name==b->NAME  || it->name==vectname(b->NAME)))
          {
            search(b, 0, 0, CGV_SEL_SUCC_LOGIC, mark); //<-fonc
          }
    }

  for (n=cgvf->NET; n!=NULL; n=n->NEXT)
    {
      for (it=itr; it!=NULL; it=it->NEXT)
        {
          if (it->object_type==CGV_NET_TYPE && 
              (n->NAME!=NULL && (it->name==n->NAME || it->name==vectname(n->NAME))))
            {
              for (cl = n->CON_NET; cl!=NULL; cl=cl->NEXT) 
                {
                  c = (cgvcon_list *) cl->DATA;
                  if (!IsCgvConExternal(c))
                    {
                      b=(cgvbox_list *)c->ROOT;
                      b->misc.count++;
                    }
                }
              if (mark!=NULL && n->USER!=NULL) mark(n->USER);
              for (cl=net_in_path; cl!=NULL && cl->DATA!=n; cl=cl->NEXT) ;
              if (cl==NULL) net_in_path=addchain(net_in_path, n);
              // |_ fonc
              break;
            }
        }
    }

  if (complete)
    {
      for (b=cgvf->BOX; b!=NULL; b=b->NEXT)
        {
          if (b->misc.count>=2)
            {
              for (cl=box_in_path; cl!=NULL && cl->DATA!=b; cl=cl->NEXT) ;
              if (cl==NULL && !IsCgvBoxTransparence(b)) 
                {
                  box_in_path=addchain(box_in_path, b);
                  if (mark!=NULL) mark(b->USER);
                }
            }
          else 
            b->misc.count=-1;
        }
    }

#endif

  AUTOMARK_REVERSE=0;
}

library *CGV_LIB=NULL;

void cgv_SetDefaultLibrary(library *l)
{
  CGV_LIB=l;
}

static ht *corresp=NULL;

void parsecorresp(char *name)
{
  FILE *f;
  int i, nbcon,k;
  char *names[1000];
  char temp[1000], temp0[1000];

  char *gatename, *conname, *gatename0;
  if ((f=fopen(name,"rt"))==NULL) 
   { 
     avt_errmsg(CGV_ERRMSG, "004", AVT_ERROR, name);
     // fprintf(stderr,"could not open file '%s'\n", name);
     return; 
   }
  if (corresp==NULL) corresp=addht(100);

  i=fscanf(f,"%s",temp);
  
  while (i>0) 
    {
      gatename=namealloc(temp);
//      printf("->%s",gatename);
      i=fscanf(f,"%d",&nbcon);
      for (k=1;k<=nbcon;k++) 
        {
          if (fscanf(f,"%s",temp)<=0) 
            {
              fprintf(stderr,"could not parse file '%s'\n", name);
              fclose(f); 
              return; 
            }
//          printf(" %s",temp);
          names[k]=namealloc(temp);
        }
//      printf("\n");
      i=fscanf(f,"%s",temp);
      while (i>=0)
        {
          if (temp[0]!='*') break;
          if (fscanf(f," %s",temp)<=0) 
            {
              avt_errmsg(CGV_ERRMSG, "005", AVT_ERROR, name);
              //fprintf(stderr,"could not parse file '%s'\n", name);
              fclose(f);
              return; 
            }
          addhtitem(corresp,gatename0=namealloc(temp), (long)gatename);
//          printf("-+>%s",gatename0);
          for (k=1;k<=nbcon;k++) 
            {
              if (fscanf(f,"%s",temp0)<=0) 
                {
                  avt_errmsg(CGV_ERRMSG, "005", AVT_ERROR, name);
                  // fprintf(stderr,"could not parse file '%s'\n", name);
                  fclose(f);
                  return; 
                }
              conname=namealloc(temp0);
//              printf(" %s",temp0);
              sprintf(temp0,"%s.%s",gatename0, conname);
              addhtitem(corresp,namealloc(temp0),(long)names[k]);
            }
//          printf("\n");
          i=fscanf(f,"%s",temp);
        }
    }

  fclose(f);
}

char *getcorrespgate(char *name)
{
  long t;
  if ((t=gethtitem(corresp, name))==EMPTYHT) return NULL;
  else return (char *)t;
}

char *getcorrespgatepin(char *name, char *con)
{
  char temp[1000];
  long t;
  sprintf(temp,"%s.%s",name,con);
  if ((t=gethtitem(corresp, namealloc(temp)))==EMPTYHT) return NULL;
  else return (char *)t;
}
