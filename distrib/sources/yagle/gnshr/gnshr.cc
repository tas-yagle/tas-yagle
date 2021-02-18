
extern "C" 
{
#include MCC_H
#include LIB_H
#include TLF_H
#include STM_H
#include TRC_H
#include INF_H
}

#define _t_STRING 1
#define _t_NUM 2
#define _t_TAG 3

#include "gnshr.h"

char *CELLDIR=NULL, *LIBNAME=NULL, *SUBFIGURE=NULL, *BBOXPREFIX="bbox";
char *inf=NULL, *ouf=NULL, *separ=NULL;
int TAI=0, BI=0, FL=0, REVI=0, DISP=0, LSTAT=0, DISPHIER=0, T2I=0, SCRAM=0;
int VLSTEP=-1;

oneopt options[]=
  {
/*    {"-fig=", _t_STRING, &SUBFIG, "Chooses a figure in main figure"},
    {"-na", _t_TAG, &NA, "Keep only one name per signal"},
    {"-bi", _t_TAG, &BI, "Flatten using BLACKBOX method (step 1)"},
    {"-f", _t_TAG, &FL, "Flatten using standard method (step 1)"},
    {"-revi", _t_TAG, &REVI, "Revert lofig interface"},
    {"-d", _t_TAG, &DISP, "Display lofig and instance interface"},
    {"-stat", _t_TAG, &LSTAT, "Display lofig stats"},
    {"-disphier", _t_TAG, &DISPHIER, "Display all the lofig hierarchy"},
*/
    {"-nodesepar=", _t_STRING, &separ, "Specify the node separator character"},
    {"-in=", _t_STRING, &inf, "Specify the input netlist format"},
    {"-out=", _t_STRING, &ouf, "Specify the output netlist format"},
    {"-fig=", _t_STRING, &SUBFIGURE, "Specify the netlist name inside the input file"},
    {"-tai", _t_TAG, &TAI, "Drive transistors as instance in final netlist"},
    {"-prefix=", _t_STRING, &BBOXPREFIX, "Specify the prefix to use when creating blackboxs"},
    {"-gnscelldir=", _t_STRING, &CELLDIR, "Sets the gns cell directory to load the rules from"},
    {"-gnslibraryname=", _t_STRING, &LIBNAME, "Sets the gns library name to read the configuration from"}
  };



void parseopt(char **argv, int argc, char **newargv, int *newargc)
{
  int i=1, sub=0, j, mode=0;

  *newargc=0;
  newargv[(*newargc)++]=argv[0];

  while (i<argc || mode==1)
    {
      if (mode==0)
        {
          sub=0;
          for (j=0; (unsigned)j<sizeof(options)/sizeof(*options); j++)
            {
              if (strncmp(argv[i], options[j].code, strlen(options[j].code))==0)
                {
                  mode=1;
                  if (argv[i][strlen(options[j].code)]!='\0' && argv[i][strlen(options[j].code)-1]=='=')
                    {
                      sub=strlen(options[j].code);
                      break;
                    }
                  else
                    if (argv[i][strlen(options[j].code)]=='\0') break;
                }
            }
          if ((unsigned)j>=sizeof(options)/sizeof(*options))
            {
              newargv[(*newargc)++]=argv[i];
            }
          if (sub==0) i++;
        }
      else
        {
          switch(options[j].mode)
            {
            case _t_STRING : *(char **)options[j].var=strdup(&argv[i][sub]); i++; break;
            case _t_NUM: *(int *)options[j].var=atoi(&argv[i][sub]); i++; break;
            case _t_TAG: *(int *)options[j].var=1; break;
            default: i++; break;
            }
          mode=0;
        }
    }
}

void dispopt()
{
  int j;
  char ligne[2048];
  for (j=0; (unsigned)j<sizeof(options)/sizeof(*options); j++)
    {
      sprintf(ligne,"%s",options[j].code);
      switch(options[j].mode)
        {
        case _t_STRING : strcat(ligne," <string>"); break;
        case _t_NUM: strcat(ligne," <integer>"); break;
        default:  break;
        }
      printf("    %-20s %s\n",ligne,options[j].desc);
    }
}


void updateinstancemodel(chain_list *lflist, char *orig, char *dest)
{
  loins_list *li;
  
  while (lflist!=NULL)
    {
      for (li=((lofig_list *)lflist->DATA)->LOINS; li!=NULL; li=li->NEXT)
        if (li->FIGNAME==orig) li->FIGNAME=dest;
      lflist=lflist->NEXT;
    }
}
// **************************
// * taken from spi_drive.c *
// **************************
static char *spi_gettransname(char type)
{
  chain_list *chain ;
 
  if(MLO_IS_TRANSN(type))
    chain = TNMOS ;
  else if (MLO_IS_TRANSP(type))
    chain = TPMOS ;
  else
    chain = TNMOS ;
   
  return((char *)chain->DATA) ;
}

static char *modelname(lotrs_list *tr)
{
  char *subcktname=NULL, *modelname;
  ptype_list *ptype;

  if(tr->MODINDEX < 0)
    return spi_gettransname(tr->TYPE);

  modelname=getlotrsmodel(tr);

  if ( SPI_TRANSISTOR_AS_INSTANCE )
    {
      if (( ptype = getptype ( tr->USER, TRANS_FIGURE )))
        subcktname = (char*)ptype->DATA;
      else 
        subcktname = mcc_getsubckt( MCC_MODELFILE, modelname, 
                                    MLO_IS_TRANSN(tr->TYPE)?MCC_NMOS:MCC_PMOS,
                                    MCC_TYPICAL, ((float)tr->LENGTH/SCALE_X)*1.0e-6,
                                    ((float)tr->WIDTH/SCALE_X)*1.0e-6);
    }

  if (subcktname==NULL) return modelname;
  return subcktname;
}
// ***************************
ht *getransistormodels(chain_list *figures)
{
  ht *done;
  lotrs_list *tr;
  char *trmodel;

  done=addht(10);
  while (figures)
    {
      for (tr=((lofig_list *)figures->DATA)->LOTRS; tr!=NULL; tr=tr->NEXT)
        {
          trmodel=modelname(tr);
          if (gethtitem(done, trmodel)==EMPTYHT)
            {
              addhtitem(done, trmodel, 0);
            }
        }
      figures=figures->NEXT;
    }
  return done;
}

chain_list *striplist(chain_list *figures)
{
  chain_list *cl=NULL, *ch;
  ht *done;

  ch=figures;
  done=getransistormodels(figures);
  while (ch)
    {
      if (gethtitem(done, ((lofig_list *)ch->DATA)->NAME)==EMPTYHT)
        cl=addchain(cl, ch->DATA);
      ch=ch->NEXT;
    }
  freechain(figures);
  delht(done);
  return reverse(cl);
}




int main(int argc, char *argv[])
{

  char *av[1000];
  int ac, i, k;
  lofig_list *mainlf, *lf, *bbox;
  chain_list *rec;
  bbox_list *bboxs, *curbbox;
  mbk_chrono mc;
  char buf[128];
  chain_list *subcktlist;
  FILE *f;
  int topmode=0;
  inffig_list *ifl;
  
  parseopt(argv,argc,av,&ac);

  if (TAI)
    putenv ("MBK_SPICE_DRIVER_FLAGS=DriveInstanceParameters,IgnoreGlobalParameters");
    
  if (separ!=NULL)
    {
      char temp[1000];
      sprintf(temp,"MBK_SPI_COMPOSE_NAME_SEPAR=%c", separ[0]);
      putenv (temp);
    }

  avtenv();
  
  mbkenv();               /* MBK environment */

  avt_banner ("gnshr", "GNS based Hierarchy Rebuilder", "2004");
  
  if (ac <3)
    {
      printf ("USAGE: %s [options] <input figure> <output figure> [<blackbox name>...]\n", av[0]);
      dispopt();
      return 1;
    }
 
  cnsenv();               /* CNS environment */
  yagenv(NULL);           /* YAG environment */
  fclenv();               /* FCL environment */
  rcnenv();
  rcx_env();
  
#ifdef AVERTEC_LICENSE
  if(!avt_givetoken("YAGLE_LICENSE_SERVER", YAG_CONTEXT->YAGLE_TOOLNAME)) EXIT(1);
#endif
    
  elpenv();
  mccenv();
  stmenv();
  elpLoadOnceElp();
    
  if (inf!=NULL)
    {
      if (strlen(inf)>3) avt_error("gnshr", 1, AVT_ERR, "input format '%s' is too long\n", inf);
      else strcpy(IN_LO, inf);
    }

  if (ouf!=NULL)
    {
      if (strlen(ouf)>3) avt_error("gnshr", 2, AVT_ERR, "output format '%s' is too long\n", ouf);
      else strcpy(OUT_LO, ouf);
    }

  avt_error("gnshr", -1, AVT_INFO, "input format: ¤+%s¤.   output format : ¤+%s¤.\n", IN_LO, OUT_LO);

  // sets scale_x so the parameters in meter fit in LONG*SCALE_X
  SCALE_X=1000;

#ifdef DELAY_DEBUG_STAT
  mbk_debugstat("",1);
#endif

  avt_error("gnshr", -1, AVT_INFO,"loading input netlist : '¤3%s¤.'\n",av[1]);
  mainlf=lf=getlofig(av[1],'A');

#ifdef DELAY_DEBUG_STAT
  mbk_debugstat("",0);
#endif

  if (SUBFIGURE!=NULL)
    {
      if ((mainlf=lf=getloadedlofig(SUBFIGURE))!=NULL)
        avt_error("gnshr", -1, AVT_INFO, "using sub-figure '¤3%s¤.'\n",SUBFIGURE);
      else
        {
          avt_error("gnshr", 3, AVT_ERR, "Could not find sub-figure '¤3%s¤.'\n",SUBFIGURE);
          return 1;
        }
      if (ac<4) { av[3]=lf->NAME; ac=4; topmode=1; }
    }
  else
    if (ac<4) { av[3]=av[1]; ac=4; topmode=1; }

  YAG_CONTEXT->YAGLE_ERR_FILE = mbkfopen(lf->NAME, "rep", WRITE_TEXT); //stdout;


  bboxs=NULL;

  for (i=3; i<ac; i++)
    {
      if ((bbox=getloadedlofig(av[i]))!=NULL)
          {
            avt_error("gnshr", -1, AVT_INFO, "¤~     ¤. Blackbox '¤+%s¤.'\n",av[i]);
            if (bbox->LOINS!=NULL)
              {
                avt_error("gnshr", -1, AVT_INFO, "flattening Netlist...");
                fflush(stderr);
                mbk_StartChrono(&mc);
                flatOutsideList(bbox);
                mbk_StopChrono(&mc);
                avt_fprintf(stderr, " (¤3%s¤.)\n", mbk_GetUserChrono(&mc, buf));
              }
            
            curbbox=(bbox_list *)mbkalloc(sizeof(bbox_list));
            curbbox->next=bboxs;
            bboxs=curbbox;
            curbbox->lf=bbox;
            
            ifl=getinffig(bbox->NAME);
            
            yagFindSupplies(ifl, bbox, TRUE);
            yagFindInternalSupplies(ifl, bbox, FALSE);
            yagTestTransistors(bbox, FALSE);
            
            rec=genius_main(bbox, CELLDIR, LIBNAME, NULL);
            
            curbbox->gnsrun=LATEST_GNS_RUN;
            curbbox->kept_instances=rec;
            
            k=countchain(rec);
            avt_error("gnshr", -1, AVT_INFO, "%d instance%s kept at top level\n", k, k>1?"s":"");
#ifdef DELAY_DEBUG_STAT
            mbk_debugstat("",0);
#endif
          }
      else
        {
          avt_error("gnshr", 1, AVT_WAR, "could not find blackbox figure named '%s'\n",av[i]);
        }
    }

  mbk_set_cut_info(BBOXPREFIX, FCL_INSLIST_PTYPE, FCL_TRANSLIST_PTYPE);

  subcktlist=NULL;
  mbk_build_hierarchy_list(mainlf, NULL, &subcktlist);

  for (curbbox=bboxs; curbbox!=NULL; curbbox=curbbox->next)
    {
      if (curbbox->kept_instances!=NULL)
        {
          lf=mbk_modify_hierarchy(curbbox->lf, curbbox->kept_instances, 1);
          updateinstancemodel(subcktlist, curbbox->lf->NAME, lf->NAME);
#ifdef DELAY_DEBUG_STAT
          mbk_debugstat("modif+update:",0);
#endif
        }
    }

  freechain(subcktlist);

  if (topmode) { mainlf=lf; }

  mainlf->NAME=namealloc(av[2]);

  subcktlist=NULL;
  mbk_build_hierarchy_list(mainlf, NULL, &subcktlist);

  subcktlist=striplist(subcktlist);

  avt_error("gnshr", -1, AVT_INFO, "driving output netlist : '¤3%s¤.'\n",av[2]);
  if ((f=mbkfopen(av[2], OUT_LO, WRITE_TEXT))==NULL)
    {
      avt_error("gnshr", 4, AVT_ERR, "could not open output file for netlist '¤3%s¤.'\n",av[2]);
      return 2;
    }

  if (TAI)
    {
      avt_error("gnshr", -1, AVT_INFO, "transistors are changed to instances of transistor\n");
      SPI_TRANSISTOR_AS_INSTANCE=1;
      
    }

  spiceprintdate( mainlf->NAME, f);
  spicesavelofigsinfile( subcktlist, f);

  fclose(f);
  freechain(subcktlist);

  fclose(YAG_CONTEXT->YAGLE_ERR_FILE);
#ifdef DELAY_DEBUG_STAT
  mbk_debugstat("drive:",0);
#endif
  return 0;
}
