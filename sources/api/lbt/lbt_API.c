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
#include TUT_H
#include LIB_H
#include TLF_H
#include BEH_H

#define API_USE_REAL_TYPES
#include "lbt_API.h"

char *lib_CanonicPinName (char *name)
{
    return lib_get_tg_pinname(name);
}

void lib_DriveHeader (TimingFigure *fig, FILE* file, char* libname)
{
#ifdef AVERTEC_LICENSE
   if (avt_givetoken ("HITAS_LICENSE_SERVER", "tma")!=AVT_VALID_TOKEN)
      EXIT (1);
#endif
     if (fig) {
         if (libname)
            lib_driveheader(file, libname, fig->INFO);
         else
            lib_driveheader(file, fig->INFO->FIGNAME, fig->INFO);
     }
}

void lib_drivefile (List *fig_list, List *befig_list, char* file, char* delaytype)
{
#ifdef AVERTEC_LICENSE
   if (avt_givetoken ("HITAS_LICENSE_SERVER", "tma")!=AVT_VALID_TOKEN)
      EXIT (1);
#endif
    if( !fig_list )
        return ;
    if(strcmp(delaytype,"max") == 0 )
        V_STR_AFFECT(V_STR_TAB[__TMA_DRIVEDELAY].VALUE,"max");
    else if(strcmp(delaytype,"min") == 0 )
        V_STR_AFFECT(V_STR_TAB[__TMA_DRIVEDELAY].VALUE,"min");
    else if(strcmp(delaytype,"both") == 0 )
        V_STR_AFFECT(V_STR_TAB[__TMA_DRIVEDELAY].VALUE,"both");
    else
        V_STR_AFFECT(V_STR_TAB[__TMA_DRIVEDELAY].VALUE,"max");

    initializeBdd (0); 
    lib_drive (fig_list, befig_list, file, namealloc ("_avt_bkbox")); 
    destroyBdd (1);
}

void lib_DriveFile (List *fig_list, List *befig_list, char* file, char* delaytype)
{
    lib_drivefile (fig_list, befig_list, file, delaytype);
}

void tlf_DriveFile  (List *fig_list, List *befig_list, char* file, char* format)
{
#ifdef AVERTEC_LICENSE
   if (avt_givetoken ("HITAS_LICENSE_SERVER", "tma")!=AVT_VALID_TOKEN)
      EXIT (1);
#endif
    if (!strcasecmp (format, "tlf3")) 
        tlf3_drive (fig_list, befig_list, file, namealloc ("_avt_bkbox"));
    if (!strcasecmp (format, "tlf4")) 
        tlf4_drive (fig_list, befig_list, file, namealloc ("_avt_bkbox"));
}
