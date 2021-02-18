#include <stdio.h>
#include AVT_H
#include MCC_H

#define API_USE_REAL_TYPES
#include "../../api/tcl/avt_API.h"
#undef API_USE_REAL_TYPES

typedef struct ams_context_st
{
  char  *OUTNAME;
  char  *SPINAME;
  char  *FIGNAME;
} ams_context;

typedef struct powinfo
{
  double slope;
  double vss, vdd;
} powinfo;

ams_context *AMS_CONTEXT = NULL;

#define POWVSS   1
#define POWVDD   2
#define POWSLOPE 3
#define D2A_TYPE 1
#define A2D_TYPE 2
#define POWINFO_PTYPE 0xfab50421

extern char *vlg_name(char *);

/****************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ams_DriveProbeCommandFile(lofig_list *lf, char *TOP, FILE *f)
{
  loins_list *li, *li0;
  locon_list *lc;
  char buf[2000];

  fprintf(f,"*.probe tran v\n");

  return;

  for (li=lf->LOINS; li!=NULL && strstr(li->INSNAME,"_yagcore")==NULL; li=li->NEXT)
    ;

  if (li!=NULL)
  {
    // digital interface
    fprintf(f,"* top interface\n");
    for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
      fprintf(f,".probe tran v(%s)\n", vlg_name(lc->NAME));

    fprintf(f,"* digital core\n");
    for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
    {
      sprintf(buf,"%s.%s.%s", TOP, li->INSNAME, lc->NAME);
      fprintf(f,".probe tran v(%s)\n", buf);
    }

    fprintf(f,"* analog\n");
    for (li0=lf->LOINS; li0!=NULL; li0=li0->NEXT)
      if (li0!=li)
        for (lc=li0->LOCON; lc!=NULL; lc=lc->NEXT)
        {
          sprintf(buf,"%s.%s.%s", TOP, li0->INSNAME, lc->NAME);
          fprintf(f,".probe tran v(%s)\n", buf);
        }
  }
}

/*}}}************************************************************************/
/*{{{                    initcontext()                                      */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void initcontext()
{
  if (AMS_CONTEXT)
    return;
  else
  {
    AMS_CONTEXT = mbkalloc(sizeof(ams_context));
    memset(AMS_CONTEXT,0,sizeof(ams_context));
  }
}

/*}}}************************************************************************/
/*{{{                    ams_SetContext()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int
ams_SetContext(char *outname, char *spiname, char *figname)
{
  initcontext();
  if (outname)
    AMS_CONTEXT->OUTNAME = outname;
  if (spiname)
    AMS_CONTEXT->SPINAME = spiname;
  if (figname)
    AMS_CONTEXT->FIGNAME = figname;

  return 0;
}

/*}}}************************************************************************/
/*{{{                    ams_LoadMainFile()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ams_LoadMainFile(char *filename, char *format)
{
  ams_SetContext(NULL,namealloc(filename),NULL);
  avt_LoadFile(filename,format);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ams_SetPowerSupply(lofig_list *lf, char *name, double vss, double vdd)
{
  char *con, *ins;
  ptype_list *pt;
  loins_list *li;
  locon_list *lc;
  powinfo *pi;

  if ((con=strchr(name,':'))!=NULL) { *con='\0'; con++; con=namealloc(con); }
  ins=namealloc(name);
  if ((li=mbk_quickly_getloinsbyname(lf, ins))==NULL) exit(5);
  if (con!=NULL)
  {
    for (lc=li->LOCON; lc!=NULL && lc->NAME!=con; lc=lc->NEXT) ;
    if (lc==NULL) exit(6);
    if ((pt=getptype(lc->USER, POWINFO_PTYPE))==NULL)
      pt=lc->USER=addptype(lc->USER, POWINFO_PTYPE, mbkalloc(sizeof(powinfo)));
  }
  else
    if ((pt=getptype(li->USER, POWINFO_PTYPE))==NULL)
      pt=li->USER=addptype(li->USER, POWINFO_PTYPE, mbkalloc(sizeof(powinfo)));

  pi=(powinfo *)pt->DATA;
  pi->slope=0;
  pi->vss=vss;
  pi->vdd=vdd;
}


/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static double ams_GetPowerInfo(locon_list *con, int item)
{
  ptype_list *pt;
  if ((pt=getptype(con->USER, POWINFO_PTYPE))==NULL)
  {
    if (con->TYPE=='I')
      pt=getptype(((loins_list *)con->ROOT)->USER, POWINFO_PTYPE);
    else
      pt=NULL;
  }

  switch(item)
  {
    case POWVSS:
         if (pt==NULL) return 0;
         return ((powinfo *)pt->DATA)->vss;
    case POWVDD:
         if (pt==NULL) return V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
         return ((powinfo *)pt->DATA)->vdd;
    case POWSLOPE:
         if (pt==NULL) return SIM_SLOP*1e-12;
         if (((powinfo *)pt->DATA)->slope==0) return SIM_SLOP*1e-12;
         return ((powinfo *)pt->DATA)->slope;
  }
  return 0;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static char *addmodel(FILE *f, ht *models, double vss, double vdd, double slope, int type)
{
  char buf[1024], *c;
  long l;
  if (type==D2A_TYPE)
  {
    sprintf(buf,"D2A__%.2f__%.2f__%.2f",vss, vdd, slope*1e12);
    c=buf; while ((c=strchr(c,'.'))!=NULL) *c='_';
    if ((l=gethtitem(models, c=namealloc(buf)))==EMPTYHT)
    {
      fprintf(f, ".model %s d2a mode=std_logic vlo=%g vhi=%g trise=%g tfall=%g\n", buf, vss, vdd, slope, slope);
      addhtitem(models, c, 0);
    }
  }
  else
  {
    sprintf(buf,"A2D__%.2f",(vss+vdd)*SIM_VTH);
    c=buf; while ((c=strchr(c,'.'))!=NULL) *c='_';
    if ((l=gethtitem(models, c=namealloc(buf)))==EMPTYHT)
    {
      fprintf(f, ".model %s a2d mode=std_logic vth1=%g vth2=%g\n", buf, (vss+vdd)*SIM_VTH, (vss+vdd)*SIM_VTH);
      addhtitem(models, c, 0);
    }
  }
  return c;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void hname(char *buf, char *dest)
{
  if (buf[0]=='\\')
  {
    char *c;
    if ((c=strchr(buf,' '))!=NULL) *c='\0';
    sprintf(dest,"\\%s\\\\", buf);
  }
  else
    strcpy(dest, buf);
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline int cmpd(double v1, double v2)
{
  if (fabs(v1-v2)<1e-20) return 1;
  return 0;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ams_DriveWaveCommandFile(lofig_list *lf, char *TOP, char *name)
{
  //loins_list *li, *li0;
  //locon_list *lc, *lci;
  //losig_list *ls;
  char /*buf[2000],*/ a[128], b[128]/*, c[128], d[128]*/;
  FILE *f;

  f=fopen(name,"wt");

  hname(vlg_name("test"), a);
  hname(vlg_name(TOP), b);

  fprintf(f,"\nvcd add -r :%s:%s:*\n",a,b);

  /*
     fprintf(f,"\nview structure nets\n");


     for (li=lf->LOINS; li!=NULL && strstr(li->INSNAME,"_yagcore")==NULL; li=li->NEXT) ;

     if (li!=NULL)
     {
  // digital interface
  fprintf(f,"# top interface\n");
  for (lc=lf->LOCON; lc!=NULL; lc=lc->NEXT)
  {
  hname(vlg_name(lc->NAME), c);
  fprintf(f,"add wave :%s:%s\n", a, c);
  }

  fprintf(f,"# digital core\n");
  for (lc=li->LOCON; lc!=NULL; lc=lc->NEXT)
  {
  hname(vlg_name(lc->NAME), c);
  hname(vlg_name(li->INSNAME), d);
  sprintf(buf,":%s:%s:%s:%s", a, b, d, c);
  fprintf(f,"add wave %s\n", buf);
  }

  fprintf(f,"# analog\n");
  for (li0=lf->LOINS; li0!=NULL; li0=li0->NEXT)
  {
  if (li0!=li)
  {
  for (lc=li0->LOCON; lc!=NULL; lc=lc->NEXT)
  {
  hname(vlg_name(lc->NAME), c);
  hname(vlg_name(li0->INSNAME), d);
  sprintf(buf,":%s:%s:%s:%s", a, b, d, c);
  fprintf(f,"add wave %s\n", buf);
  }
  }
  }      
  }
   */
  fclose(f);
  //unused
  lf = NULL;
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ams_CreateConverters(lofig_list *lf, char *TOP, FILE *f)
{
  loins_list *li;
  int nbi, nbo, nbio;
  chain_list *cl;
  ptype_list *pt;
  locon_list *lc;//, *lci;
  losig_list *ls;
  double powvss, powvdd, powslope;
  ht *models;
  char buf[2000], *digi, *modelname, *modelnameA, *iname;
  int t1, t2;
  chain_list *allconverters=NULL;

  models=addht(100);

  for (li=lf->LOINS; li!=NULL && (digi=strstr(li->INSNAME,"_yagcore"))==NULL; li=li->NEXT) ;

  if (li!=NULL)
  {
    nbi=nbo=nbio=0;
    *digi='\0';
    sprintf(buf,"%s_yagroot", li->INSNAME);
    *digi='_';
    digi=namealloc(buf);
    digi="root";
    for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
    {
      if (mbk_LosigIsVDD(ls) || mbk_LosigIsVSS(ls)) continue;

      if ((pt=getptype(ls->USER, LOFIGCHAIN))!=NULL)
      {
        for (cl=(chain_list *)pt->DATA; cl!=NULL; cl=cl->NEXT)
        {
          lc=(locon_list *)cl->DATA;
          powvss=ams_GetPowerInfo(lc, POWVSS);
          powvdd=ams_GetPowerInfo(lc, POWVDD);
          powslope=ams_GetPowerInfo(lc, POWSLOPE);

          // par defaut?
          //                  if (cmpd(powvss,0) && cmpd(powvdd,V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE) && cmpd(powslope,SIM_SLOP*1e-12)) continue;

          if (lc->TYPE=='I')
          {
            iname=((loins_list *)lc->ROOT)->INSNAME;
            if (lc->ROOT==li) // digital
              t1=A2D_TYPE, t2=D2A_TYPE;
            else
              t2=A2D_TYPE, t1=D2A_TYPE;

            if (lc->DIRECTION==IN) 
            {
              modelname=addmodel(f, models, powvss, powvdd, powslope, t1);
              sprintf(buf, ".hook %s.%s.%s %s.%s.%s.%s mod=%s", TOP, digi, getsigname(lc->SIG), TOP, digi, iname, lc->NAME, modelname);
              allconverters=addchain(allconverters, mbkstrdup(buf));
            }
            else if (lc->DIRECTION==TRANSCV || lc->DIRECTION==UNKNOWN)
            {
              modelnameA=addmodel(f, models, powvss, powvdd, powslope, D2A_TYPE);
              modelname=addmodel(f, models, powvss, powvdd, powslope, A2D_TYPE);
              sprintf(buf, ".hook %s.%s.%s %s.%s.%s.%s bidir mod=%s mod=%s", TOP, digi, getsigname(lc->SIG), TOP, digi, iname, lc->NAME, modelname, modelnameA);
              allconverters=addchain(allconverters, mbkstrdup(buf));
            }
            else
            {
              modelnameA=addmodel(f, models, powvss, powvdd, powslope, t2);
              sprintf(buf, ".hook %s.%s.%s %s.%s.%s.%s mod=%s", TOP, digi, getsigname(lc->SIG), TOP, digi, iname, lc->NAME, modelnameA);
              allconverters=addchain(allconverters, mbkstrdup(buf));
            }
          }
        }
      } // ptype
    }
  }
  delht(models);
  if (allconverters!=NULL) 
  {
    allconverters=reverse(allconverters);
    fprintf(f,"\n* specific hook association\n");
    for (cl=allconverters; cl!=NULL; cl=cl->NEXT)
    {
      fprintf(f, "%s\n", (char *)cl->DATA);
      mbkfree(cl->DATA);
    }
    freechain(allconverters);
  }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static void ams_AddAlims(lofig_list *lf, FILE *f)
{
  losig_list *ls;
  float val;
  int cnt=1;
  chain_list *allglob=NULL, *cl;

  // alim "locales"
  for (ls=lf->LOSIG; ls!=NULL; ls=ls->NEXT)
  {
    if (mbk_LosigIsVDD(ls))
    {
      if (!getlosigalim (ls,&val)) val=V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE;
      fprintf(f,"vavt%d %s 0 dc %gv\n", cnt++, spinamenodedetail(ls, SPI_NONODES, 0), val);
    }
    else if (mbk_LosigIsVSS(ls) && strcmp(getsigname(ls),"0")!=0)
    {
      if (!getlosigalim (ls,&val)) val=0;
      fprintf(f,"vavt%d %s 0 dc %gv\n", cnt++, spinamenodedetail(ls, SPI_NONODES, 0), val);
    }
    else
      if (strcmp(getsigname(ls),"0")!=0 && getlosigalim (ls,&val)) 
        fprintf(f,"vavt%d %s 0 dc %gv\n", cnt++, spinamenodedetail(ls, SPI_NONODES, 0), val);


    if (mbk_LosigIsGlobal(ls) && strcmp(getsigname(ls),"0")!=0) allglob=addchain(allglob, ls);
  }

  if (allglob!=NULL)
  {
    fprintf(f,"\n.global");
    for (cl=allglob; cl!=NULL; cl=cl->NEXT)
      fprintf(f," %s",spinamenodedetail((losig_list *)cl->DATA, SPI_NONODES, 0));        
    fprintf(f,"\n");
    freechain(allglob);
  }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void printOption(FILE *f, char *options)
{
  char  *ptr_str, *ptr_sh;
  char   buf[1024];

  sprintf(buf,"%s",options);
  fprintf(f, "\n");
  ptr_sh    = buf;
  while ((ptr_str = strtok(ptr_sh," ")))
  {
    fprintf(f, ".option %s\n", ptr_str);
    ptr_sh  = NULL;
  }
}

/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void printTime(FILE *f, double time_ns, int end)
{
  if (time_ns > 0.99e9)
    fprintf(f,"%g",time_ns*1e-9);
  else if (time_ns > 0.99e6)
    fprintf(f,"%gm",time_ns*1e-6);
  else if (time_ns > 0.99e3)
    fprintf(f,"%gu",time_ns*1e-3);
  else if (time_ns > 0.99)
    fprintf(f,"%gn",time_ns);
  else if (time_ns > 0.99e-3)
    fprintf(f,"%gp",time_ns*1e3);
  else
    fprintf(f,"%gf",time_ns*1e6);

  if (end)
    fprintf(f,"s\n");
  else
    fprintf(f,"s ");
}


/*}}}************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void ams_DriveCommandFile(lofig_list *origlf, lofig_list *lf, char *filename)
{
  FILE *f;
  char *env;


  lofigchain(lf);

  f=fopen(filename,"wt");

  fprintf(f, "* command file\n");
  if (SIM_SPICE_OPTIONS!=NULL)
    printOption(f,SIM_SPICE_OPTIONS);
  //fprintf(f, "\n.option %s\n", SIM_SPICE_OPTIONS);
  fprintf(f, ".option limprobe=10000\n");

  fprintf(f,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
  fprintf(f,"\n.temp %g\n", MCC_TEMP);
  if ((env=SIM_TECHFILE)==NULL || avt_is_default_technoname(env))
    env=MCC_MODELFILE;
  if (env!=NULL)
    fprintf(f,".include \"%s\"\n", env);

  // hooks
  fprintf(f,"\n* default hook models\n");
  fprintf(f,".model a2d_default a2d mode=std_logic vth1=%g vth2=%g\n",V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE*SIM_VTH,V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE*SIM_VTH);
  fprintf(f,".model d2a_default d2a mode=std_logic vlo=%g vhi=%g trise=%g tfall=%g\n", 0.0, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, SIM_SLOP*1e-12, SIM_SLOP*1e-12);
  fprintf(f,".defhook d2a_default a2d_default\n");

  /*  fprintf(f,"\n* specific hook models\n");
      ams_CreateConverters(lf, "top", f);
   */
  // alims
  fprintf(f,"\n* power supplies\n");
  ams_AddAlims(origlf, f);

  // simu
  //fprintf(f,"\n.tran %gns %gns\n", V_FLOAT_TAB[__SIM_TRAN_STEP ].VALUE*1e9, V_FLOAT_TAB[__SIM_TIME ].VALUE*1e9);
  fprintf(f,"\n.tran ");
  printTime(f,V_FLOAT_TAB[__SIM_TRAN_STEP ].VALUE*1e9,0);
  printTime(f,V_FLOAT_TAB[__SIM_TIME ].VALUE*1e9,1);
  //ams_DriveProbeCommandFile(lf, "top", f);

  //fprintf(f,"*.probe tran v\n");
  fclose(f);
}

/*}}}************************************************************************/
/*{{{                    ams_DriveADvanceMSDeck()                           */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void
ams_DumpADvanceMSDeck(char *tb)
{
  loins_list    *li;
  lofig_list    *inslf;
  ht            *fight;
  FILE          *fbb;
  FILE          *fcom;
  FILE          *fcmd;
  char          *env;
  lofig_list    *origlf;
  lofig_list    *lf; 
  char          *bbfile = "ams_blackbox_file.v";

  origlf    = avt_GetNetlist(AMS_CONTEXT->FIGNAME);
  lf        = avt_GetNetlist(AMS_CONTEXT->OUTNAME);

//  printf ("test %s from %s -> %s\n",
//          AMS_CONTEXT->FIGNAME,AMS_CONTEXT->SPINAME,AMS_CONTEXT->OUTNAME);
  fight = addht(10);

  if (!origlf || !lf)
  {
    printf("Missed some file\n");
    return;
  }
  lofigchain(lf);

  fcmd  = fopen("ams_command_file.cmd","wt");
  fcom  = fopen("ams_compile","wt");
  fbb   = fopen(bbfile,"wt");

  //
  // command file
  // 
  fprintf(fcmd, "* command file\n");
  if (SIM_SPICE_OPTIONS!=NULL)
    printOption(fcmd,SIM_SPICE_OPTIONS);

  fprintf(fcmd, ".option limprobe=10000\n");

  fprintf(fcmd,".option tnom=%g\n", V_FLOAT_TAB[__SIM_TNOM].VALUE);
  fprintf(fcmd,"\n.temp %g\n", MCC_TEMP);
  if ((env=SIM_TECHFILE)==NULL || avt_is_default_technoname(env))
    env=MCC_MODELFILE;
  if (env!=NULL)
    fprintf(fcmd,".include \"%s\"\n", env);

  if (AMS_CONTEXT->SPINAME)
    fprintf(fcmd,".include \"%s\"\n", AMS_CONTEXT->SPINAME);

  // hooks
  fprintf(fcmd,"\n* default hook models\n");
  fprintf(fcmd,".model a2d_default a2d mode=std_logic vth1=%g vth2=%g\n",V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE*SIM_VTH,V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE*SIM_VTH);
  fprintf(fcmd,".model d2a_default d2a mode=std_logic vlo=%g vhi=%g trise=%g tfall=%g\n", 0.0, V_FLOAT_TAB[__SIM_POWER_SUPPLY].VALUE, SIM_SLOP*1e-12, SIM_SLOP*1e-12);
  fprintf(fcmd,".defhook d2a_default a2d_default\n");

  // alims
  fprintf(fcmd,"\n* power supplies\n");
  ams_AddAlims(origlf, fcmd);

  // simu
  fprintf(fcmd,"\n.tran ");
  printTime(fcmd,V_FLOAT_TAB[__SIM_TRAN_STEP ].VALUE*1e9,0);
  printTime(fcmd,V_FLOAT_TAB[__SIM_TIME ].VALUE*1e9,1);

  //
  // compile script
  // 
  fprintf(fcom,"#!/bin/csh -f\n");
  fprintf(fcom,"set SPICE_LIB=\"ams_spice_lib\"\n");
  fprintf(fcom,"set VERILOG_LIB=\"ams_verilog_lib\"\n");
  fprintf(fcom,"#clean\n");
  fprintf(fcom,"/usr/bin/rm -rf $SPICE_LIB $VERILOG_LIB WORK\n");
  fprintf(fcom,"\n");
  fprintf(fcom,"#verilog compilation\n");
  fprintf(fcom,"valib $VERILOG_LIB\n");
  fprintf(fcom,"\n");
  fprintf(fcom,"valog %s.v -ms\n",lf->NAME);
  fprintf(fcom,"valog %s -ms\n",bbfile);
  fprintf(fcom,"valog %s -ms\n",tb);
  fprintf(fcom,"\n");
  fprintf(fcom,"# spice compilation\n");
  fprintf(fcom,"valib $SPICE_LIB\n");

  for (li=lf->LOINS; li!=NULL; li=li->NEXT)
    if (gethtitem(fight, li->FIGNAME)==EMPTYHT)
      if ((inslf=getloadedlofig(li->FIGNAME))!=NULL)
      {
        addhtitem(fight, li->FIGNAME, 0);
        if (AMS_CONTEXT->SPINAME)
          fprintf(fcom,"vaspi -f -srclib $VERILOG_LIB %s %s@%s\n",
                  li->FIGNAME,li->FIGNAME,AMS_CONTEXT->SPINAME);
        else
          fprintf(fcom,"vaspi -f -srclib $VERILOG_LIB %s %s@<to complete>\n",
                  li->FIGNAME,li->FIGNAME);
        vlogsavelofig(inslf, fbb);
      }

  fprintf(fcom,"\n");
  fprintf(fcom,"vamap WORK $VERILOG_LIB\n");

  delht(fight);
  fclose(fcom);
  fclose(fbb);
  fclose(fcmd);
}

/*}}}************************************************************************/
