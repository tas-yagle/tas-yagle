#include MUT_H
#include MLO_H
#include MLU_H
#include RCN_H
#include MSL_H
#include AVT_H
#include INF_H
#include ELP_H
#include CNS_H
#include YAG_H
#include TLC_H
#include TRC_H
#include STM_H
#include TTV_H
#include MCC_H
#include SIM_H
#include TAS_H
#include FCL_H
#include BEH_H
#include CBH_H
#include TUT_H
#include LIB_H
#include TLF_H
#include STB_H

#define API_USE_REAL_TYPES
#include "tas_API.h"

//-----------------------------------------------------------------

ttvfig_list *hitas_sub (char *figname, char cnsannotatelofig, char loadcnsfig )
{
    ttvfig_list *ttvfig;

    mbkenv ();
    cnsenv ();
    yagenv (tas_yaginit);
    fclenv ();
    elpenv ();
    mccenv ();
    tlcenv ();
    rcnenv ();
    rcx_env ();
    stb_env ();
    stb_ctk_env ();
    tas_setenv ();
    tas_version ();
    libenv ();
    tlfenv ();
    cbhenv ();
    ttvenv ();

    TAS_CONTEXT->TAS_FILENAME           = namealloc (figname);
    TAS_CONTEXT->TAS_CNS_ANNOTATE_LOFIG = cnsannotatelofig ;
    
    if (loadcnsfig=='Y' || getenv("TASREADCNS")!=NULL)
    {
       if (getloadedlofig(figname)!=NULL)
         dellofig(figname);
       TAS_CONTEXT->TAS_CNS_LOAD='Y';
    }

    { // configuration check level I
      lofig_list *lf;
      locon_list *lc;
      chain_list *data;
      inffig_list *ifl;

      data = NULL;

      lf = getloadedlofig(TAS_CONTEXT->TAS_FILENAME);
      ifl = getloadedinffig(TAS_CONTEXT->TAS_FILENAME);
      if (lf)
      {
        for (lc = lf->LOCON; lc; lc = lc->NEXT)
          data = addchain(data,lc->NAME);

        if (ifl)
        {
          inf_CheckRegistry(stdout,ifl,1,data);
        }
      }
      freechain(data);
    }


    
    TAS_CONTEXT->TAS_FILEIN = TAS_CONTEXT->TAS_FILENAME;
    YAG_CONTEXT->YAG_FIGNAME = TAS_CONTEXT->TAS_FILEIN;
    if (!TAS_CONTEXT->TAS_FILEOUT)
        TAS_CONTEXT->TAS_FILEOUT = TAS_CONTEXT->TAS_FILEIN;

    if ((ttvfig=ttv_gethtmodel(TAS_CONTEXT->TAS_FILEOUT))!=NULL)
      {
        ttv_freettvfig(ttvfig);
      }

    if(TAS_CONTEXT->TAS_SILENTMODE == 'Y')
        tas_CloseTerm() ;

    ttvfig = tas_main ();

    if(TAS_CONTEXT){
        mbkfree (TAS_CONTEXT);
        TAS_CONTEXT = NULL;
    }

    ttv_getinffile(ttvfig);

    return ttvfig;
}

ttvfig_list *tas (char *figname)
{
    return hitas_sub (figname, 'N', 0 );
}

//-----------------------------------------------------------------

void tas_command_line (char *args)
{
    int argc, i, j, k;
    char **argv;
    char buf[1024];

    i = 0;
    argc = 1;
    if (args[0] != '\0')
        do 
            if ((args[i] == ' ') || (args[i] == '\0')) argc++;
        while (args[i++] != '\0');
    argv = (char**)malloc (argc * sizeof (char*));
    argv[0] = strdup ("hitas");

    i = 0;
    j = 1;
    k = 0;
    do {
        if ((args[i] == ' ') || (args[i] == '\0')) {
            buf[k] = '\0';
            argv[j++] = strdup (buf);
            k = 0;
            if (args[i] == '\0') break;
        }
        else {
            if( args[i] != '{' && args[i] != '}' )
                buf[k++] = args[i];
        }
    } while (args[i++] != '\0');

    /* debug */
    /*
    fprintf (stdout, "******* argc = %d\n", argc);
    for (i = 0; i < argc; i++) 
        fprintf (stdout, "%s ", argv[i]);
    fprintf (stdout, "\n");
    */
                
    
    tas_top_main (argc, argv);
}

int hitas_pvt_count()
{
  return TAS_PVT_COUNT;
}

#if 0
//-----------------------------------------------------------------

lofig_list *tas_extract_SPICE_path (ttvcritic_list *critic)
{
    lofig_list *lofig;
    cnsfig_list *cnsfig;

    mbkenv ();
    cnsenv ();
    yagenv (tas_yaginit);
    fclenv ();
    elpenv ();
    mccenv ();
    tlcenv ();
    rcnenv ();
    rcx_env ();
    stb_env ();
    stb_ctk_env ();
    tas_setenv ();

    cnsfig = getcnsfig (critic->FIG->INFO->FIGNAME, NULL);
    lofig = getloadedlofig (critic->FIG->INFO->FIGNAME);
    return tas_extract_spice_path (lofig, cnsfig, NULL, critic);
}

//-----------------------------------------------------------------

sim_model *tas_extract_SPICE_patterns (ttvcritic_list *critic, lofig_list *figext)
{
    lofig_list *lofig;
    cnsfig_list *cnsfig;

    cnsfig = getcnsfig (critic->FIG->INFO->FIGNAME, NULL);
    lofig = getloadedlofig (critic->FIG->INFO->FIGNAME);
    return tas_extract_spice_patterns (lofig, cnsfig, figext, SIM_MAX);
}

#endif
