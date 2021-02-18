#include <stdio.h>
#include AVT_H
#include MUT_H
#include SPE_H
#include MSL_H
#include MLU_H
#include MCC_H
#include SIM_H

lofig_list *avt_GetNetlist(char *name)
{
    lofig_list *fig = getloadedlofig(name);
    float max_global_supply;
    
    if (!fig) {
        avt_errmsg(AVT_API_ERRMSG, "001", AVT_WARNING, name);
//        fprintf (stdout, "ERROR: '%s' is not among the loaded netlists\n", name);
        return NULL;
    }

    if (V_FLOAT_TAB[__SIM_POWER_SUPPLY].SET==0 && mbk_GetPowerSupply (fig, &max_global_supply)) 
        MCC_VDDmax = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE = elpGeneral[elpGVDDMAX] = max_global_supply;
    
    return fig;
}

void avt_DriveNetlist(lofig_list *lf, char *filename, char *format)
{
  FILE *f;
  int error=0;

  if (lf==NULL) return;

  if (strcasecmp(format,"spice")==0)
    {
      if ((f=mbkfopen(filename,NULL,WRITE_TEXT))!=NULL)
        {
          spicesavelofiginfile(lf, f, 0);
          fclose(f);
        }
      else error=1;
    }
  else if (strcasecmp(format,"verilog")==0)
    {
      if ((f=mbkfopen(filename,NULL,WRITE_TEXT))!=NULL)
        {
          vlogsavelofig(lf, f);
          fclose(f);
        }
      else error=1;
    }
  else if (strcasecmp(format,"vhdl")==0)
    {
      if ((f=mbkfopen(filename,NULL,WRITE_TEXT))!=NULL)
        {
          vhdlsavelofig(lf, f);
          fclose(f);
        }
      else error=1;
    }
   else if (strcasecmp(format,"spef")==0)
    {
      if ((f=mbkfopen(filename,NULL,WRITE_TEXT))!=NULL)
        {
          spef_drive(lf, f);
          fclose(f);
        }
      else error=1;
    }
  else
    avt_errmsg(AVT_API_ERRMSG, "002", AVT_ERROR, format, filename);
//    avt_error("avtapi", 2, AVT_ERROR, "unknown file format '%s' for '%s'\n", format, filename);

  if (error)
    avt_errmsg(AVT_API_ERRMSG, "003", AVT_ERROR, filename);
//    avt_error("avtapi", 2, AVT_ERROR, "could not create file '%s'\n", filename);
}

void avt_DriveSignalInfo( lofig_list *lf, char *nameregex, char *filename )
{
  losig_list  *losig ;
  chain_list  *chain ;
  FILE        *ptf ;
  num_list    *node ;
  ptype_list  *ptl ;
  chain_list  *chlocon ;
  locon_list  *locon ;
  chain_list  *chnodename ;
  char        *name ;
  lowire_list *wire ;
  char         s1[256], s2[256] ;
  lonode_list *ptnode ;
  chain_list  *chctc ;
  loctc_list  *ctc ;
  char         needsepar='n';

  if( lf == NULL )
    return ;

  ptf = mbkfopen( filename, NULL, WRITE_TEXT );
  if( !ptf ) {
    avt_errmsg(AVT_API_ERRMSG, "003", AVT_ERROR, filename);
    //avt_error("avtapi", 2, AVT_ERROR, "could not create file '%s'\n", filename);
    return ;
  }
  
  for( losig = lf->LOSIG ; losig ; losig = losig->NEXT ) {
  
    for( chain = losig->NAMECHAIN ; chain ; chain = chain->NEXT ) {
      if( mbk_TestREGEX( (char*)chain->DATA, nameregex ) ) 
        break ;
    }

    if( chain ) {

      if( needsepar == 'y' ) {
        fprintf( ptf, "\n-------------------------------------------------\n\n" );
      }
      needsepar = 'y' ;
      
      fprintf( ptf, "Signal %s\n\n", getsigname( losig ) );
      
      ptl = getptype( losig->USER, LOFIGCHAIN );
      if( ptl ) {
        fprintf( ptf, "Connector list :\n\n" );
        for( chlocon = (chain_list*)ptl->DATA ; chlocon ; chlocon = chlocon->NEXT ) {
          locon = (locon_list*)chlocon->DATA ;
          switch( locon->TYPE ) {
          case 'I' :
            fprintf( ptf, "instance %s", ((loins_list*)locon->ROOT)->INSNAME );
            break ;
          case 'E' :
            fprintf( ptf, "external" );
            break ;
          case 'T' :
            fprintf( ptf, "transistor %s", ((lotrs_list*)locon->ROOT)->TRNAME );
            break ;
          default :
            fprintf( ptf, "unknown" );
          }
          fprintf( ptf, " -> %s\n", locon->NAME );
         
          ptl = getptype( locon->USER, PNODENAME );
          if( ptl )
            chnodename = (chain_list*)ptl->DATA ;
          else
            chnodename = NULL ;
            
          for( node = locon->PNODE ; node ; node = node->NEXT ) {
            if( chnodename ) 
              name = (char*)chnodename->DATA ;
            else
              name = "";
            fprintf( ptf, "  %ld %s\n", node->DATA, name );
            if( chnodename ) 
              chnodename = chnodename->NEXT ;
          }
        }
        fprintf( ptf, "\n" );
      }
      else {
        fprintf( ptf, "No connector on this signal\n\n" );
      }

      if( losig->PRCN ) {

        for( wire = losig->PRCN->PWIRE ; wire ; wire = wire->NEXT ) {
          ptnode = getlonode( losig, wire->NODE1 );
          if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) )
            sprintf( s1, "%4ld*", ptnode->INDEX );
          else 
            sprintf( s1, "%4ld ", ptnode->INDEX );
          ptnode = getlonode( losig, wire->NODE2 );
          if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) )
            sprintf( s2, "%4ld*", ptnode->INDEX );
          else
            sprintf( s2, "%4ld ", ptnode->INDEX );
          fprintf( ptf, "w %s %s %g\n", s1, s2, wire->RESI );
        }
        fprintf( ptf, "\n" );
        
        for( chctc = losig->PRCN->PCTC ; chctc ; chctc = chctc->NEXT ) {
          ctc = (loctc_list*)chctc->DATA ;
          ptnode = getlonode( losig, rcn_ctcnode( ctc, losig ) );
          if( RCN_GETFLAG( ptnode->FLAG, RCN_FLAG_LOCON ) )
            sprintf( s1, "%4ld*", ptnode->INDEX );
          else 
            sprintf( s1, "%4ld ", ptnode->INDEX );
          fprintf( ptf, "c %s %g\n", s1, ctc->CAPA );
        }
      }
    }
  }

  fclose( ptf );
}

void avt_RemoveCapacitances(lofig_list *lf, char *nameregex)
{
  losig_list *ls;
  chain_list *cl;

  if (lf==NULL) return;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (ls->PRCN==NULL) continue;
      ls->PRCN->RCCAPA=0;
      if (ls->PRCN->PCTC==NULL) continue;

      for (cl=ls->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
        if (mbk_TestREGEX((char *)cl->DATA, nameregex)) break;

      if (cl!=NULL)
        {
          while( ls->PRCN->PCTC )
            delloctc( (loctc_list*)ls->PRCN->PCTC->DATA );

          rcn_mergectclosig(ls);
        }        
    }
}

void avt_RemoveResistances(lofig_list *lf, char *nameregex)
{
  losig_list *ls;
  chain_list *cl;
  loctc_list *lctc;
  ptype_list *pt;
  num_list *ln;

  if (lf==NULL) return;

  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (ls->PRCN==NULL) continue;
      if (ls->PRCN->PWIRE==NULL) { rcn_mergectclosig(ls); continue; }

      for (cl=ls->NAMECHAIN; cl!=NULL; cl=cl->NEXT)
        if (mbk_TestREGEX((char *)cl->DATA, nameregex)) break;

      if (cl!=NULL)
        {
          while( ls->PRCN->PWIRE )
            dellowire(ls, ls->PRCN->PWIRE->NODE1, ls->PRCN->PWIRE->NODE2);

          for (cl=ls->PRCN->PCTC; cl!=NULL; cl=cl->NEXT)
            {
              lctc=(loctc_list *)cl->DATA;
              if (lctc->SIG1==ls)
                lctc->NODE1=1;
              else
                lctc->NODE2=1;
            }
          
          if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL) cl=(chain_list *)pt->DATA;
          else cl=NULL;
          
          while (cl!=NULL)
            {
              for (ln=((locon_list *)cl->DATA)->PNODE; ln!=NULL; ln=ln->NEXT)
                ln->DATA=1;
              cl=cl->NEXT;
            }
          
          ls->PRCN->NBNODE=2;
          rcn_mergectclosig(ls);
        }
    }
}

void avt_FlattenNetlist(lofig_list *lf, char *level)
{
  eqt_ctx *mc_ctx=NULL;
  if (lf==NULL) return;
  flatten_parameters (lf, NULL, NULL , 0, 0, 0, 0, 0, 0, &mc_ctx);
  if (mc_ctx!=NULL) eqt_term(mc_ctx);

  if (strcasecmp(level,"transistors")==0 || strcasecmp(level,"trs")==0)
    rflattenlofig (lf, YES, NO);
  else if (strcasecmp(level,"catalog")==0 || strcasecmp(level,"catal")==0)
    rflattenlofig (lf, YES, YES);
  else if (strcasecmp(level,"blackbox")==0 || strcasecmp(level,"bbox")==0)
    flatOutsideList (lf);
  else
    flattenlofig (lf, level, YES);
}

void avt_ViewNetlist(char *name)
{
  lofig_list *lf;
  if ((lf=getloadedlofig(name))!=NULL)
    viewlofig(lf);
}

void avt_ViewSignal(char *name, char *signal)
{
  lofig_list *lf;
  losig_list *ls;
  if ((lf=getloadedlofig(name))!=NULL)
     if ((ls=mbk_quickly_getlosigbyname(lf, signal))!=NULL)
       {
         rcn_flush_signal(lf, ls );
         rcn_refresh_signal( lf, ls );
         viewlosig(ls);
       }
}

void avt_RemoveNetlist(char *name)
{
  lofig_list *lf;
  if ((lf=getloadedlofig(name))!=NULL)
    unlocklofig(lf);
  dellofig(name);
}

void avt_SetBlackBoxes(chain_list *cl)
{
  SetBlackList(cl);
}

typedef struct 
{
  int tot_trans;
  int tot_capa;
  int tot_resi;
  int tot_sig;
  int tot_inst;
} stat_result;

static chain_list *disphier(lofig_list *lf, char *figname, char *pad, int count, stat_result *tot, int maxdepth, int depth, char *insnamestr)
{
  ht *tab;
  loins_list *li;
  losig_list *ls;
  int cnti;
  stat_result sr, lsr;
  char *model;
  char *buf;
  chain_list *lst=NULL, *cl;
  loctc_list *lcl;
  chain_list *inslist;
  char *newinsnamestr;
  chain_list *ptchain;
  int len;


  sr.tot_trans=sr.tot_capa=sr.tot_resi=sr.tot_sig=sr.tot_inst=0;

  if (lf!=NULL)
    {
      tab=addht(10);
      while (1)
        {
          for (li=lf->LOINS; li!=NULL && gethtitem(tab,li->FIGNAME)!=EMPTYHT; li=li->NEXT) ;
          if (li!=NULL)
            {
              addhtitem(tab, li->FIGNAME, 1);
              model=li->FIGNAME;
              cnti=0;
              len = 0;
              inslist = NULL;
              while (li!=NULL)
                {
                  if (li->FIGNAME==model)
                  {
                    inslist = addchain(inslist, li->INSNAME);
                    len += strlen(li->INSNAME);
                    cnti++;
                  }
                  li=li->NEXT;
                }
              inslist = reverse(inslist);
              newinsnamestr = mbkalloc(len + cnti + 1);
              newinsnamestr[0] = 0;
              for (ptchain = inslist; ptchain; ptchain = ptchain->NEXT)
                {
                  strcat(newinsnamestr, (char *)ptchain->DATA);
                  strcat(newinsnamestr, ",");
                }
              newinsnamestr[len+cnti-1] = 0;
              freechain(inslist);
              strcat(pad,"| ");
              lst=append(disphier(getloadedlofig(model), model, pad, cnti, tot, maxdepth, depth+1, newinsnamestr), lst);
              mbkfree(newinsnamestr);
              sr.tot_trans+=tot->tot_trans*cnti;
              sr.tot_capa+=tot->tot_capa*cnti;
              sr.tot_resi+=tot->tot_resi*cnti;
              sr.tot_sig+=tot->tot_sig*cnti;
              sr.tot_inst+=tot->tot_inst*cnti;
              pad[strlen(pad)-2]='\0';
            }
          else break;
        }
      delht(tab);
      sr.tot_trans+=(lsr.tot_trans=countchain((chain_list *)lf->LOTRS));
      sr.tot_sig+=(lsr.tot_sig=countchain((chain_list *)lf->LOSIG));
      sr.tot_inst+=(lsr.tot_inst=countchain((chain_list *)lf->LOINS));
  
      lsr.tot_resi=lsr.tot_capa=0;
      for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
        {
          if (ls->PRCN!=NULL)
            {
              lsr.tot_resi+=countchain((chain_list *)ls->PRCN->PWIRE);
              sr.tot_resi+=countchain((chain_list *)ls->PRCN->PWIRE);
              if (ls->PRCN->PCTC!=NULL)
                {
                  for (cl=ls->PRCN->PCTC; cl!=NULL; cl=cl->NEXT)
                    {
                      lcl=(loctc_list *)cl->DATA;
                      if (lcl->SIG1<lcl->SIG2)
                        {
                          lsr.tot_capa++;
                          sr.tot_capa++;
                        }
                    }
                }
            }
        }
    }
  else
    {
      lsr.tot_trans=lsr.tot_capa=lsr.tot_resi=lsr.tot_sig=lsr.tot_inst=0;
    }
  
  memcpy(tot, &sr, sizeof(stat_result));
  if (depth+1<=maxdepth)
    {
      if (insnamestr)
        {
          buf = mbkalloc(strlen(insnamestr) + 1024);
          sprintf(buf,"%s(%d) %s  (%s)   global(%d trs, %d ins, %d resi, %d capa, %d sig)   local(%d trs, %d ins, %d resi, %d capa, %d sig)",pad,count,figname, insnamestr, sr.tot_trans, sr.tot_inst, sr.tot_resi, sr.tot_capa, sr.tot_sig, lsr.tot_trans, lsr.tot_inst, lsr.tot_resi, lsr.tot_capa, lsr.tot_sig);
        }
      else
        {
          buf = mbkalloc(1024);
          sprintf(buf,"%s(%d) %s     global(%d trs, %d ins, %d resi, %d capa, %d sig)   local(%d trs, %d ins, %d resi, %d capa, %d sig)",pad,count,figname, sr.tot_trans, sr.tot_inst, sr.tot_resi, sr.tot_capa, sr.tot_sig, lsr.tot_trans, lsr.tot_inst, lsr.tot_resi, lsr.tot_capa, lsr.tot_sig);
        }
      lst=append(addchain(NULL, strdup(buf)), lst);
      mbkfree(buf);
    }

  return lst;
}

void avt_DisplayNetlistHierarchy(FILE *f, char *netlistname, int maxdepth)
{
  chain_list *lst;
  lofig_list *lf;
  stat_result sr;
  char pad[1024];

  if ((lf=avt_GetNetlist(netlistname))==NULL) return;
  
  strcpy(pad,"");
  if (maxdepth<=0) maxdepth=10000000;
  lst=disphier(lf, lf->NAME, pad, 1, &sr, maxdepth, 0, NULL);
  while (lst!=NULL)
    {
      fprintf(f, "%s\n", (char *)lst->DATA);
      free(lst->DATA);
      lst=delchain(lst, lst);
    }
}

#define COL_RESI_NAME 0
#define COL_RESI_VAL 1
#define COL_RESI_NODE 2

void avt_DisplayResistivePath(FILE *f, lofig_list *lf, char *connector1, char *connector2)
{
  ptype_list *pt;
  chain_list *phinterf=NULL, *realinterf=NULL, *cl, *ch;
  num_list *nm;
  locon_list *lc;
  int i, done;
  char *con[2], *net[2]={NULL, NULL};
  long num[2]={-1, -1}, cnt;
  losig_list *sig;
  lowire_list *lw;

  con[0]=mbk_decodeanyvector(connector1);
  con[1]=mbk_decodeanyvector(connector2);
  if ((pt=getptype(lf->USER, PH_INTERF))!=NULL) phinterf=(chain_list *)pt->DATA;
  if ((pt=getptype(lf->USER, PH_REAL_INTERF))!=NULL) realinterf=(chain_list *)pt->DATA;

  if (countchain(phinterf)==countchain(realinterf))
    {
      for (i=0; i<2; i++)
        {
          for (cl=realinterf, ch=phinterf; cl!=NULL && cl->DATA!=con[i]; cl=cl->NEXT, ch=ch->NEXT) ;
          if (cl!=NULL) 
            {
              net[i]=ch->DATA;
              for (cl=phinterf, cnt=0; cl!=ch; cl=cl->NEXT)
                if (cl->DATA==net[i]) cnt++;

              for (lc=lf->LOCON; lc!=NULL && lc->NAME!=net[i]; lc=lc->NEXT) ;
              
              if (lc!=NULL)
                {
                  for (nm=lc->PNODE; nm!=NULL && cnt>0; nm=nm->NEXT, cnt--) ;
                  if (nm!=NULL)
                    num[i]=nm->DATA;
                  else
                    num[i]=-1;
                  sig=lc->SIG;
                }
            }
          else 
            avt_errmsg(AVT_API_ERRMSG, "004", AVT_ERROR, con[i]);
        }


      if (net[0]!=NULL && num[0]==-1)
         avt_errmsg(AVT_API_ERRMSG, "005", AVT_ERROR, con[0]);
      if (net[1]!=NULL && num[1]==-1) 
         avt_errmsg(AVT_API_ERRMSG, "005", AVT_ERROR, con[1]);

      done=0;
      if (net[0]==net[1] && num[0]!=-1 && num[1]!=-1)
        {          
          long a, nb;
          char buf[512];
          rcn_flush_signal(lf, sig );
          rcn_refresh_signal( lf, sig );
          cl=rcn_get_a_r_way(sig, num[0], num[1]);
          if (cl!=NULL)
            {
              Board *b;
              b=Board_CreateBoard();
              Board_SetSize(b, COL_RESI_NAME, 20, 'l');
              Board_SetSize(b, COL_RESI_VAL, 10, 'r');
              Board_SetSize(b, COL_RESI_NODE, 10, 'l');
              Board_NewLine(b);
              Board_SetValue(b, COL_RESI_NAME, "Resistor Name");
              Board_SetValue(b, COL_RESI_VAL, "Value");
              Board_SetValue(b, COL_RESI_NODE, "Node");
              Board_NewSeparation(b);
              Board_NewLine(b);
              sprintf(buf,"[%ld] %s", num[0], con[0]);
              Board_SetValue(b, COL_RESI_NODE,  buf);

              a=num[0];
              while (cl!=NULL)
                {
                  lw=(lowire_list *)cl->DATA;
                  if (lw->NODE1!=a) nb=lw->NODE1; else nb=lw->NODE2;

                  Board_NewLine(b);

                  if ((pt=getptype(lw->USER, RESINAME))!=NULL)
                    Board_SetValue(b, COL_RESI_NAME,  (char *)pt->DATA);

                  if (cl->NEXT==NULL)
                    sprintf(buf,"[%ld] %s", num[1], con[1]);
                  else
                    sprintf(buf,"[%ld]", nb);

                  Board_SetValue(b, COL_RESI_NODE,  buf);
                  sprintf(buf,"%.1f", lw->RESI);
                  Board_SetValue(b, COL_RESI_VAL,  buf);
                  a=nb;
                  cl=delchain(cl, cl);
                }
              Board_NewSeparation(b);
              avt_fprintf(f, "      *** Resistive path beetween connector '%s' and '%s'\n\n", con[0], con[1]);
              
              Board_Display(f, b, "");
              Board_FreeBoard(b);
              
              avt_fprintf(f, "\n");            
              done=1;
            }
        }

      if (!done)
        fprintf(f, "      *** No resistive path found beetween connector '%s' and '%s'\n", con[0], con[1]);
    }
}
