/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_common.c                                               */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Nizar ABDALLAH                      Date : 01/04/1993     */
/*                                                                          */
/*    Modified by : Stephane PICAULT                  Date : 30/07/1998     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/


/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"
#include "bitmaps.h"

/*--------------------------------------------------------------------------*/
/*                             WORK VARIABLES                               */
/*--------------------------------------------------------------------------*/

static        Widget          *Xtas_pt_buttons;
static struct XalButtonsItem  *Xtas_final_menus;

XalBitmapsStruct Xtas_bitmaps[] = {
{"xtas_al2spice",xtas_al2spice_bits,xtas_al2spice_width,xtas_al2spice_height},
{"xtas_all_paths",xtas_all_paths_bits,xtas_all_paths_width,xtas_all_paths_height},
{"xtas_all_sig",xtas_all_sig_bits,xtas_all_sig_width,xtas_all_sig_height},
{"xtas_break",xtas_break_bits,xtas_break_width,xtas_break_height},
{"xtas_capa",xtas_capa_bits,xtas_capa_width,xtas_capa_height},
{"xtas_close",xtas_close_bits,xtas_close_width,xtas_close_height},
{"xtas_command",xtas_command_bits,xtas_command_width,xtas_command_height},
{"xtas_connectors",xtas_connectors_bits,xtas_connectors_width,xtas_connectors_height},
{"xtas_deb",xtas_deb_bits,xtas_deb_width,xtas_deb_height},
{"xtas_detail",xtas_detail_bits,xtas_detail_width,xtas_detail_height},
{"xtas_dummy",xtas_dummy_bits,xtas_dummy_width,xtas_dummy_height},
{"xtas_fpath",xtas_false_path_bits,xtas_false_path_width,xtas_false_path_height},
{"xtas_f_edge",xtas_f_edge_bits,xtas_f_edge_width,xtas_f_edge_height},
{"xtas_gate",xtas_gate_bits,xtas_gate_width,xtas_gate_height},
{"xtas_h_resistance",xtas_h_resistance_bits,xtas_h_resistance_width,xtas_h_resistance_height},
{"xtas_l_resistance",xtas_l_resistance_bits,xtas_l_resistance_width,xtas_l_resistance_height},
{"xtas_info",xtas_info_bits,xtas_info_width,xtas_info_height},
{"xtas_main_win",xtas_main_win_bits,xtas_main_win_width,xtas_main_win_height},
{"xtas_memo",xtas_memo_bits,xtas_memo_width,xtas_memo_height},
{"xtas_open",xtas_open_bits,xtas_open_width,xtas_open_height},
{"xtas_paths",xtas_paths_bits,xtas_paths_width,xtas_paths_height},
{"xtas_prech",xtas_prech_bits,xtas_prech_width,xtas_prech_height},
{"xtas_rc",xtas_rc_bits,xtas_rc_width,xtas_rc_height},
{"xtas_r_edge",xtas_r_edge_bits,xtas_r_edge_width,xtas_r_edge_height},
{"xtas_save",xtas_save_bits,xtas_save_width,xtas_save_height},
{"xtas_time",xtas_time_bits,xtas_time_width,xtas_time_height},
{"xtas_time",xtas_time_bits,xtas_time_width,xtas_time_height},
{"xtas_stb",xtas_stb_bits,xtas_stb_width,xtas_stb_height} ,
{"xtas_false",xtas_falsemap_bits,xtas_falsemap_width,xtas_falsemap_height},
{"xtas_true",xtas_truemap_bits,xtas_truemap_width,xtas_truemap_height} ,
{"xtas_tas",xtas_tas_bits,xtas_tas_width,xtas_tas_height} ,
{"xtas_tma",xtas_tma_bits,xtas_tma_width,xtas_tma_height} ,
{"xtas_xya",xtas_xya_bits,xtas_xya_width,xtas_xya_height} ,
{"xtas_vis",xtas_visual_bits,xtas_visual_width,xtas_visual_height} ,
{"xtas_cmd",xtas_cmd_bits,xtas_cmd_width,xtas_cmd_height} ,
{"xtas_simu",xtas_simu_bits,xtas_simu_width,xtas_simu_height} ,
{"xtas_ctki",xtas_ctk_info_bits,xtas_ctk_info_width,xtas_ctk_info_height} ,
{"xtas_noise",xtas_noise_bits,xtas_noise_width,xtas_noise_height} ,
{"xtas_scores",xtas_scores_bits,xtas_scores_width,xtas_scores_height}} ;

/*---------------------------------------------------------------------------*/
/*                               FUNCTIONS                                   */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
int XtasSizeLong(l)
long l ;
{
 char buf[1024] ;

 sprintf(buf,"%ld",l) ;
 return(strlen(buf)) ;
}

int XtasSizeFloatUnit(l)
long l ;
{
 char buf[1024] ;

 sprintf(buf,"%.1f",l/TTV_UNIT) ;
 return(strlen(buf)) ;
}

/*---------------------------------------------------------------------------*/
void XtasPrintType(number,type,buf,maxsize)
long number ;
char *type ;
char *buf ;
int maxsize ;
{
 int len ;
 char *pt ;
 
 pt = buf ;

 len = XtasSizeLong(number) ;
 len = maxsize - len - strlen(type) - 3 ;

 while(len--)
  {
   *pt = ' ' ;
   pt++ ;
  }

 sprintf(pt,"%ld : %s",number,type) ;
}

/*---------------------------------------------------------------------------*/

void XtasCalcSize(ttvfig,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,sizename1,sizename2,type,chain,delay,otherdelay,infos,w)
ttvfig_list *ttvfig ;
int *sizetype ;
int *sizedelay ;
int *sizeotherdelay ;
int *sizecumul ;
int *sizeothercumul ;
int *sizeslope ;
int *sizeotherslope ;
int *sizename1 ;
int *sizename2 ;
char type ;
chain_list *chain ;
long *delay ;
long *otherdelay ;
long infos ;
long w ;
{
 char buf[1024] ;
 ttvcritic_list *critic ;
 ttvpath_list *path ;
 int auxtype ;
 int auxdelay ;
 int auxotherdelay ;
 int auxslope ;
 int auxotherslope ;
 int auxcumul ;
 int auxothercumul ;
 int auxname1 ;
 int auxname2 ;
 int first ;
 long i ;

 *sizetype = 4 ;

 if((type == 'd') || (type == 'm') || (type == 'p')) {
     if (infos == XTAS_SIMU) {
        *sizedelay = 6 ;
        *sizeslope = 9 ;
        *sizecumul = 9 ;
        auxcumul = 9 ;        
        *sizeotherdelay = 8 ;
        *sizeotherslope = 11 ;
        *sizeothercumul = 11 ;        
        auxothercumul = 11 ;        
     }
     else if (infos == XTAS_CTK) {
        *sizedelay = 6 ;
        *sizeslope = 9 ;
        *sizecumul = 9 ;
        auxcumul = 9 ;        
        *sizeotherdelay = 6 ;
        *sizeotherslope = 9 ;
        *sizeothercumul = 9 ;     
        auxothercumul = 9 ;        
     }
     else {
        *sizedelay = 5 ;
        *sizeslope = 5 ;
        *sizecumul = 5 ;
        auxcumul = 5 ;        
        *sizeotherdelay = 0 ;
        *sizeotherslope = 0 ;
        *sizeothercumul = 0 ;
        auxothercumul = 0 ;        
        *otherdelay = 0 ;        
     }
 }
 else {
    *sizedelay = 5 ;
    *sizeslope = 5 ;
    *sizecumul = 5 ;
    auxcumul = 5 ;
    if (sizeotherdelay)
        *sizeotherdelay = 0 ;
    if (sizeotherslope)   
        *sizeotherslope = 0 ;
    if (sizeothercumul)
        *sizeothercumul = 0 ;
        auxothercumul = 0 ; 
    if (otherdelay)
        *otherdelay = 0 ;        
 }
 *sizename1 = 5 ;
 *sizename2 = 5 ;



 if((type == 'd') || (type == 'm') || (type == 'p') || (type == 'l'))
  {
   first = 0 ;
   *delay = (long)0 ;
   i = 1 ;
   while(chain != NULL)
    {
     critic = (ttvcritic_list *)chain->DATA ;
     for(; critic != NULL ; critic = critic->NEXT)
       {
        if (infos == XTAS_SIMU) {
            *delay += critic->REFDELAY ;
            auxdelay = XtasSizeLong(critic->REFDELAY) ;
            if(auxdelay > *sizedelay)
             *sizedelay = auxdelay ;
            auxslope = XtasSizeLong(critic->REFSLOPE) ;
            if(auxslope > *sizeslope)
             *sizeslope = auxslope ;
            *otherdelay += critic->SIMDELAY ;
            auxotherdelay = XtasSizeLong(critic->SIMDELAY) ;
            if(auxotherdelay > *sizeotherdelay)
             *sizeotherdelay = auxotherdelay ;
            auxotherslope = XtasSizeLong(critic->SIMSLOPE) ;
            if(auxotherslope > *sizeotherslope)
             *sizeotherslope = auxotherslope ;
        }
        else if (infos == XTAS_CTK){
            *delay += critic->REFDELAY ;
            auxdelay = XtasSizeLong(critic->REFDELAY) ;
            if(auxdelay > *sizedelay)
             *sizedelay = auxdelay ;
            auxslope = XtasSizeLong(critic->REFSLOPE) ;
            if(auxslope > *sizeslope)
             *sizeslope = auxslope ;
            *otherdelay += critic->DELAY ;
            auxotherdelay = XtasSizeLong(critic->DELAY) ;
            if(auxotherdelay > *sizeotherdelay)
             *sizeotherdelay = auxotherdelay ;
            auxotherslope = XtasSizeLong(critic->SLOPE) ;
            if(auxotherslope > *sizeotherslope)
             *sizeotherslope = auxotherslope ;
        }
        else {
            *delay += critic->DELAY ;
            auxdelay = XtasSizeLong(critic->DELAY) ;
            if(auxdelay > *sizedelay)
             *sizedelay = auxdelay ;
            auxslope = XtasSizeLong(critic->SLOPE) ;
            if(auxslope > *sizeslope)
             *sizeslope = auxslope ;
        }
        if(critic->NEXT != NULL)
         {
          auxname1 = strlen(critic->NAME) ;
          if(auxname1 > *sizename1)
           *sizename1 = auxname1 ;
         }
        if(first != 0)
         {
          auxname2 = strlen(critic->NAME) ;
          if(auxname2 > *sizename2)
           *sizename2 = auxname2 ;
         }
        first = 1 ;
       }
     *sizecumul = XtasSizeLong(*delay) ;
     if(*sizecumul < auxcumul) *sizecumul = auxcumul ;
     if (*otherdelay > 0)
        *sizeothercumul = XtasSizeLong(*otherdelay) ;
     if(*sizeothercumul < auxothercumul) *sizeothercumul = auxothercumul ;
     *sizetype = 4 ;
     if(type == 'l')
      {
       i++ ;
       *sizecumul = 0 ;
       if(i > w ) 
        break ;
       chain = chain->NEXT ;
      }
     else
       break ;
    }
  }
 else if(type == 'T')
  {
   i = 1 ;
   for(; chain != NULL ; chain = chain->NEXT)
    {
     critic = (ttvcritic_list *)chain->DATA ;
     auxdelay = XtasSizeLong(critic->NEXT->DELAY) ;
     if(auxdelay > *sizedelay)
      *sizedelay = auxdelay ;
     auxslope = XtasSizeLong(critic->NEXT->SLOPE) ;
     if(auxslope > *sizeslope)
      *sizeslope = auxslope ;
     auxname1 = strlen(critic->NAME) ;
     if(auxname1 > *sizename1)
      *sizename1 = auxname1 ;
     auxname2 = strlen(critic->NEXT->NAME) ;
     if(auxname2 > *sizename2)
      *sizename2 = auxname2 ;
     auxtype = XtasSizeLong(i) + 5 ;
     if(auxtype > *sizetype)
      *sizetype = auxtype ;
     i++ ;
     if(i > w ) 
      break ;
    }
   *sizecumul = 0 ;
  }
 else
  {
   i = 1 ;
   for(; chain != NULL ; chain = chain->NEXT)
    {
     path = (ttvpath_list *)chain ;

     auxdelay = XtasSizeLong(path->DELAY) ;
     if(auxdelay > *sizedelay)
      *sizedelay = auxdelay ;
     auxslope = XtasSizeLong(path->SLOPE) ;
     if(auxslope > *sizeslope)
      *sizeslope = auxslope ;
     auxname1 = strlen(ttv_getsigname(ttvfig,buf,path->NODE->ROOT)) ;
     if(auxname1 > *sizename1)
      *sizename1 = auxname1 ;
     auxname2 = strlen(ttv_getsigname(ttvfig,buf,path->ROOT->ROOT)) ;
     if(auxname2 > *sizename2)
      *sizename2 = auxname2 ;
     auxtype = XtasSizeLong(i) + 8 ;
     if(auxtype > *sizetype)
      *sizetype = auxtype ;
     i++ ;
     if(i > w ) 
      break ;
    }
   *sizecumul = 0 ;
  }
}

/*---------------------------------------------------------------------------*/

void XtasPrintFirstEnd(file,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,sizename1,sizename2,infos)
FILE *file ;
int sizetype ;
int sizedelay ;
int sizeotherdelay ;
int sizecumul ;
int sizeothercumul ;
int sizeslope ;
int sizeotherslope ;
int sizename1 ;
int sizename2 ;
long infos ;
{
 char buf[2048] ;
 char *pt ;
 int len ;

 pt = buf ;

 if(sizetype > 0)
  {
   len = sizetype ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizedelay > 0)
  {
   if (infos == XTAS_SIMU) {
    len = sizedelay - 6 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"TAS Tp") ;
    pt += 6 ;
   }
   else if (infos == XTAS_CTK) {
    len = sizedelay - 6 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Ref Tp") ;
    pt += 6 ;
   }
   else {       
    len = sizedelay - 2 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Tp") ;
    pt += 2 ;
   }
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizeotherdelay > 0)
  {
   if (infos == XTAS_SIMU) {
    len = sizeotherdelay - 8 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"SPICE Tp") ;
    pt += 8 ;
   }
   else {
    len = sizeotherdelay - 6 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Tp CTK") ;
    pt += 6 ;
   }
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizecumul > 0)
  {
   if (infos == XTAS_SIMU) {
    len = sizecumul - 9 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"TAS Total") ;
    pt += 9 ;
   }
   else if (infos == XTAS_CTK) {
    len = sizecumul - 9 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Ref Total") ;
    pt += 9 ;
   }
   else {
    len = sizecumul - 5 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Total") ;
    pt += 5 ;
   }
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizeothercumul > 0)
  {
   if (infos == XTAS_SIMU) {
    len = sizeothercumul - 11 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"SPICE Total") ;
    pt += 11 ;
   }
   else {
    len = sizeothercumul - 9 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Total CTK") ;       
    pt += 9 ;
   }
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizeslope > 0)
  {
   if (infos == XTAS_SIMU) {
    len = sizeslope - 9 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"TAS Slope") ;
    pt += 9 ;
   }
   else if (infos == XTAS_CTK) {
    len = sizeslope - 9 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Ref Slope") ;
    pt += 9 ;
   }
   else {
    len = sizeslope - 5 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Slope") ;
    pt += 5 ;
   }
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizeotherslope > 0)
  {
   if (infos == XTAS_SIMU) {
    len = sizeotherslope - 11 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"SPICE Slope") ;
    pt += 11;
   }
   else {
    len = sizeotherslope - 9 ;
    while(len--)
     {
      *pt = ' ' ;
      pt++ ;
     }
    strcpy(pt,"Slope CTK") ;
    pt += 9 ;
   }
   *pt = ' ' ;
   pt++ ;
  }
 
 if(sizename1 > 0)
  {
   len = sizename1 - 4 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   strcpy(pt,"From    ") ;
   pt += 8 ;
  }

 if(sizename2 > 0)
  {
   strcpy(pt,"To") ;
   pt += 2 ;
   len = sizename2 - 2 ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
  }

 *pt = '\0' ;

 fprintf(file,"%s\n",buf) ;
}

/*---------------------------------------------------------------------------*/

void XtasPrintLinesepar(file,sizetype,sizedelay,sizeotherdelay,sizecumul,sizeothercumul,sizeslope,sizeotherslope,sizename1,sizename2)
FILE *file ;
int sizetype ;
int sizedelay ;
int sizeotherdelay ;
int sizecumul ;
int sizeothercumul ;
int sizeslope ;
int sizeotherslope ;
int sizename1 ;
int sizename2 ;
{
 char buf[2048] ;
 char *pt ;
 int len ;

 fprintf(file,"#") ;
 pt = buf ;

 if(sizetype > 0)
  {
   len = sizetype - 1 ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizedelay > 0)
  {
   len = sizedelay ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizeotherdelay > 0)
  {
   len = sizeotherdelay ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizecumul > 0)
  {
   len = sizecumul ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizeothercumul > 0)
  {
   len = sizeothercumul ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizeslope > 0)
  {
   len = sizeslope ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizeotherslope > 0)
  {
   len = sizeotherslope ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
  }

 if(sizename1 > 0)
  {
   len = sizename1 ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
   *pt = ' ' ;
   pt++ ;
   strcpy(pt,"-- ") ;
   pt+= 3 ;
  }

 if(sizename2 > 0)
  {
   len = sizename2 ;
   while(len--)
    {
     *pt = '-' ;
     pt++ ;
    }
  }

 *pt = '\0' ;

 fprintf(file,"%s\n",buf) ;
}

/*---------------------------------------------------------------------------*/

void XtasPrintLine(file,typedelay,sizetype,delay,sizedelay,otherdelay,sizeotherdelay,cumul,sizecumul,othercumul,sizeothercumul,
                   slope,sizeslope,otherslope,sizeotherslope,name1,sizename1,slope1,slope2,
                   name2,sizename2)
FILE *file ;
char *typedelay ;
int sizetype ;
long delay ;
int sizedelay ;
long otherdelay ;
int sizeotherdelay ;
long cumul ;
int sizecumul ;
long othercumul ;
int sizeothercumul ;
long slope ;
int sizeslope ;
long otherslope ;
int sizeotherslope ;
char *name1 ;
int sizename1 ;
char slope1 ;
char slope2 ;
char *name2 ;
int sizename2 ;
{
 char buf[2048] ;
 char *pt ;
 int len ;

 if(sizetype > 0)
  {
   fprintf(file,"%s ",typedelay) ;
  }

 pt = buf ;

 if(sizedelay > 0)
  {
   len = XtasSizeLong(delay) ;
   len = sizedelay - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",delay/TTV_UNIT) ;
   pt += (XtasSizeFloatUnit(delay) + 1) ;
  }
 
 if(sizeotherdelay > 0)
  {
   len = XtasSizeLong(otherdelay) ;
   len = sizeotherdelay - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",otherdelay/TTV_UNIT) ;
   pt += (XtasSizeFloatUnit(otherdelay) + 1) ;
  }
 
 if(sizecumul > 0)
  {
   len = XtasSizeLong(cumul) ;
   len = sizecumul - len ;
   while(len--)
    {
    *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",cumul/TTV_UNIT) ;
   pt += (XtasSizeFloatUnit(cumul) + 1) ;
  }

 if(sizeothercumul > 0)
  {
   len = XtasSizeLong(othercumul) ;
   len = sizeothercumul - len ;
   while(len--)
    {
    *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",othercumul/TTV_UNIT) ;
   pt += (XtasSizeFloatUnit(othercumul) + 1) ;
  }

 if(sizeslope > 0)
  {
   len = XtasSizeLong(slope) ;
   len = sizeslope - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",slope/TTV_UNIT) ;
   pt += (XtasSizeFloatUnit(slope) + 1) ;
  }

 if(sizeotherslope > 0)
  {
   len = XtasSizeLong(otherslope) ;
   len = sizeotherslope - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%.1f ",otherslope/TTV_UNIT) ;
   pt += (XtasSizeFloatUnit(otherslope) + 1) ;
  }

 if(sizename1 > 0)
  {
   len = strlen(name1) ;
   len = sizename1 - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
   sprintf(pt,"%s %c%c ",name1,slope1,slope2) ;
   pt = pt + strlen(name1) + 4 ;
  }

 if(sizename2 > 0)
  {
   sprintf(pt,"%s",name2) ;
   len = strlen(name2) ;
   pt += len ;
   len = sizename2 - len ;
   while(len--)
    {
     *pt = ' ' ;
     pt++ ;
    }
  }

 *pt = '\0' ;

 fprintf(file,"%s\n",buf) ;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasGetMask                                                    */
/*                                                                           */
/* IN  ARGS : mask_widget : mask widget                                      */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : this function is called to build a mask chain from a text      */
/*            widget.                                                        */
/*                                                                           */
/*---------------------------------------------------------------------------*/
chain_list *XtasGetMask( mask_widget )
Widget   mask_widget;
{
static chain_list  *mask;
char        *prompt;
char        *p;

prompt = XmTextGetString( mask_widget ) ;

if (strlen( prompt ) == 0)
 {
  return( NULL );
 } 

mask = addchain(NULL,NULL) ;

if ( (p = strrchr(prompt, ' ')) == NULL)
 {
  mask->DATA = namealloc( prompt );
 }
else
 {
  *p = '\0';
  p++;
  mask->DATA = namealloc( p );
  while ((p = strrchr(prompt, ' ')) != NULL)
   {  
    *p = '\0';
     p++;
     mask = addchain( mask, (char *)namealloc( p ));
   }
  mask = addchain( mask, (char *)namealloc( prompt ));
 }
   XtFree( prompt );
   return( mask );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSuppresStringSpace                                         */
/*                                                                           */
/* IN  ARGS : s : string                                                     */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : this function is called to supress the blank char in the begin */
/*            and the end of s.                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
char* XtasSuppresStringSpace( s )
char* s;
{
 int i,j;

 while (!strncmp(s," ",1))
  {
   strcpy(s,s+1);
  }

 for ( i=strlen(s); i>=0; i-- )
   {
    if (s[i]==' ')
     {
      for ( j = i; j<(int)strlen(s); j++ )
         {
          s[j]=s[j+1];
         }
      }
   }
  return s;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasSetLabelString                                             */
/*                                                                           */
/* IN  ARGS : label_widget : label widget                                    */
/*            text : text to set                                             */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : this function is called to set a string in a label widget.     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasSetLabelString( label_widget , text )

 Widget   label_widget;
 char    *text;

{
   XmString   label;
   Arg        args[1];
   int        n;

   n = 0;
   label = XmStringCreateSimple( text ); 
   XtSetArg( args[n], XmNlabelString, label ); n++;
   XtSetValues( label_widget, args, n);
   XmStringFree( label );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDestroyAllFig                                              */
/*                                                                           */
/* IN  ARGS : ( none )                                                       */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Remove all figure                                              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDestroyAllFig()
{
 ttvfig_list *ttvfig ;

 for(ttvfig = TTV_LIST_TTVFIG ; ttvfig != NULL ; ttvfig = ttvfig->NEXT)
   if(ttvfig == XtasMainParam->ttvfig) 
     break ;
 if(ttvfig == NULL)
  {
   XtasRemovePathSession(NULL);
   XtasRemoveDetailPathSession(NULL) ;
   XtasRemoveStbSession(NULL) ;
   XtasSigsRemove();
   XtasMainParam->ttvfig = NULL ;
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasMainPopupCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Widget to popup.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This function is called from other sub-windows in order to     */
/*            make the main window appears at the top of the screen or       */
/*            the client_data != NULL                                        */
/*---------------------------------------------------------------------------*/
void XtasMainPopupCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
 XtVaSetValues( XtasTopLevel, XmNiconic, False, NULL );
 XtPopup( XtasTopLevel, XtGrabNone );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasReloadInfCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : Reload the INF file from any Xtas window.                      */
/*---------------------------------------------------------------------------*/
void XtasReloadInfCallback( widget, client_data, call_data )
Widget  widget;
XtPointer client_data;
XtPointer call_data;
{
    if (XtasMainParam->ttvfig == NULL) {
        XalDrawMessage( XtasErrorWidget, XTAS_NODBERR );
        return;
    }
    else {
        XtasPasqua();
        if(sigsetjmp( XtasMyEnv , 1 ) == 0)
        {
            
            XtasSetLabelString(XtasDeskMessageField, " ");
            XalForceUpdate( XtasTopLevel );
            XalSetCursor( XtasTopLevel, WAIT );
            inf_Dirty(XtasMainParam->ttvfig->INFO->FIGNAME);
            infRead(XtasMainParam->ttvfig->INFO->FIGNAME,'E') ;
            ttv_getinffile (XtasMainParam->ttvfig);
            tas_update_mcctemp (getloadedinffig(XtasMainParam->ttvfig->INFO->FIGNAME)); /*mise a jour de la temperature pour une eventuelle simu */
            XtasGetWarningMess() ;
            XtasFirePasqua();
            XalSetCursor( XtasTopLevel, NORMAL );
            XtasSetLabelString(XtasDeskMessageField, "INF file Reloaded");
            XalForceUpdate( XtasTopLevel );
        }
        else {
            XtasGetWarningMess() ;
            XtasFirePasqua();
            XtasSetLabelString(XtasDeskMessageField, "INF file Not Reloaded");
            XalForceUpdate( XtasTopLevel );
        }
    }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasAppNotReady                                                */
/*                                                                           */
/* IN  ARGS : ( None )                                                       */
/*                                                                           */
/* OUT ARGS :  1: Not Ready, 0: Ready.                                       */
/*                                                                           */
/* OBJECT   : Changes the current state of the current session.              */
/*                                                                           */
/*---------------------------------------------------------------------------*/
int XtasAppNotReady()
{
 if(XtasMainParam->ttvfig == NULL)
   {
    XalDrawMessage( XtasErrorWidget, XTAS_NODBERR );
    return( 1 );
   }
 return( 0 );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDestroyWidgetCallback                                      */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for all destroy commands. All it does is */
/*            to destroy the widget.                                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDestroyWidgetCallback( widget, root_widget, call_data )

 Widget  widget;
 XtPointer root_widget;
 XtPointer call_data;

{
   XtDestroyWidget( root_widget );
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasCancelCallback                                             */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for all Cancel commands. All it does is  */
/*            to unmanage the widget to be canceled.                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasCancelCallback( widget, root_widget, call_data )

 Widget  widget;
 XtPointer  root_widget;
 XtPointer call_data;

{
   XtUnmanageChild( root_widget );
//   XalLeaveLimitedLoop();
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasDummyCallback                                              */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is just a dummy function in order to do nothing.          */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasDummyCallback( widget, client_data, call_data )

 Widget    widget;
 XtPointer   client_data;
 XtPointer   call_data;
{
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasNotYetCallback                                             */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .client_data : Not significant.                                */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : This is to display a message for functions under development.  */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasNotYetCallback( widget, client_data, call_data )

 Widget  widget;
 XtPointer client_data ;
 XtPointer call_data;

{
 XalDrawMessage( XtasNotYetWidget , XTAS_NYETWAR );
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasAddDummyButton                                             */
/*                                                                           */
/* IN  ARGS : .widget : Parent widget id.                                    */
/*                                                                           */
/* OUT ARGS : ( none )                                                       */
/*                                                                           */
/* OBJECT   : Adds a dummy Button in order to have at least one.             */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasAddDummyButton( widget )
Widget widget;
{
   GC          gc;
   Pixel       fg;
   Pixel       bg;

   XtVaGetValues( widget, XmNforeground, &fg, XmNbackground, &bg, NULL );
   gc = XCreateGC(XtDisplay(widget), RootWindowOfScreen(XtScreen(widget)), 0, 0);
   XSetForeground(XtDisplay(widget), gc, fg);
   XSetBackground(XtDisplay(widget), gc, bg);
   XFreeGC(XtDisplay(widget),gc) ; 

   XtVaCreateManagedWidget( "dbutton",
                             xmDrawnButtonWidgetClass,
                             widget,
                             XmNwidth,             40,
                             XmNheight,            40,
                             XmNpushButtonEnabled, False,
                             XmNsensitive,         False,
                             XmNshadowThickness,   0,
                            NULL);
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasButtonsChangedCallback                                     */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .new_state : Pointer to the specific "selected" field.         */
/*            .call_data : The toggle callback structure.                    */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Buttons command when     */
/*            a Button is selected or unselected.                            */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasButtonsChangedCallback( widget, new_state, cbs )
Widget widget;
XtPointer new_state;
XtPointer cbs;
{
 Boolean *res = (Boolean *)new_state ;

 XmToggleButtonCallbackStruct *state = (XmToggleButtonCallbackStruct *) cbs ;

 *res = state->set;
}


/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasButtonsOkCallback                                          */
/*                                                                           */
/* IN  ARGS : .widget : The widget on which the event occurs.                */
/*            .options : User New choices.                                   */
/*            .call_data : Not significant.                                  */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : The callback function for the Options/Buttons command when     */
/*            the OK Button is selected.                                     */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasButtonsOkCallback( widget, client_data, call_data )

Widget widget;
XtPointer client_data;
XtPointer call_data;

{
   int    n, i;
   Arg    args[10];
   Widget parent;
   struct  XalButtonsItem *options = (struct XalButtonsItem *)client_data ;

   for ( i=0;
         Xtas_final_menus[i].pix_file != -1 ;
         i++ )
   {
      Xtas_final_menus[i].selected = options[i].selected;
   }

   parent = XtParent( *Xtas_pt_buttons );
   XtDestroyWidget( *Xtas_pt_buttons );

   n = 0;
   XtSetArg( args[n], XmNnumColumns,  1            ); n++;
   XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
   *Xtas_pt_buttons = XalButtonMenus( parent,
                                     Xtas_final_menus,
                                     args, n,
                                     40, 40 );
   XtasAddDummyButton( *Xtas_pt_buttons );
   XtManageChild( *Xtas_pt_buttons );
   XmAddTabGroup( *Xtas_pt_buttons );

//   XalLeaveLimitedLoop();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/* FUNCTION : XtasOptionsButtonsTreat                                        */
/*                                                                           */
/* IN  ARGS : .parent : The parent widget.                                   */
/*            .pt_buttons_widget : Pointer to the Button's Bar.              */
/*            .button_menus : The Menu's table.                              */
/*            .button_options : The Button's name list.                      */
/*                                                                           */
/* OUT ARGS : ( void )                                                       */
/*                                                                           */
/* OBJECT   : A function to customize the button's bar menu.                 */
/*                                                                           */
/*---------------------------------------------------------------------------*/
void XtasOptionsButtonsTreat( parent, pt_buttons_widget, button_menus, button_options )

 Widget                  parent;
 Widget                 *pt_buttons_widget;
 struct XalButtonsItem   button_menus[];
 struct XalButtonsItem   button_options[];
{
   int        n;
   int        i;
   int        height;
   Arg        args[10];
   XmString   text;
   Widget     prompt; 
   Widget     scroll; 
   Widget     form; 
   Widget     main_row; 
   Widget     sub_row; 
   Widget     buttons; 
   Widget     toggle;
   Atom       WM_DELETE_WINDOW;


   Xtas_pt_buttons  = pt_buttons_widget;
   Xtas_final_menus = button_menus;

   /*= PromptDialog prompt =*/
   n = 0;
   XtSetArg( args[n], XmNtitle,  XTAS_NAME": Button's Configuration" ); n++;
   XtSetArg( args[n], XmNdialogStyle, XmDIALOG_APPLICATION_MODAL ); n++;
   prompt = XmCreatePromptDialog(  parent, 
                                  "XtasInfosBox", 
                                   args, n );
   XtUnmanageChild(XmSelectionBoxGetChild( prompt, XmDIALOG_TEXT));
   XtUnmanageChild(XmSelectionBoxGetChild( prompt, XmDIALOG_PROMPT_LABEL));
   XtAddCallback(  prompt, 
                   XmNokCallback, 
                   XtasButtonsOkCallback, 
                  (XtPointer)button_options );
   XtAddCallback(  prompt, 
                   XmNcancelCallback, 
                   XtasCancelCallback, 
                  (XtPointer)prompt );

   WM_DELETE_WINDOW = XmInternAtom(XtDisplay(parent), "WM_DELETE_WINDOW", False);
   XmAddWMProtocolCallback(XtParent(prompt), WM_DELETE_WINDOW, XtasCancelCallback, (XtPointer )prompt);

   /*= Form form =*/
   n = 0;
   form = XmCreateForm(  prompt,
                        "XtasButtonsForm",
                         args, n);
   XtManageChild( form );

   /*= ScrolledWindow scroll =*/
   for ( i=0, height=0;
         button_options[i].pix_file != -1 ;
         i++, height +=45 );
   {
      height += 10;
   }
   n = 0;
   XtSetArg( args[n], XmNtopAttachment,    XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNbottomAttachment, XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNleftAttachment,   XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNrightAttachment,  XmATTACH_FORM ); n++;
   XtSetArg( args[n], XmNscrollingPolicy,  XmAUTOMATIC   ); n++;
   XtSetArg( args[n], XmNheight,           height        ); n++;
   XtSetArg( args[n], XmNwidth,            250           ); n++;
   scroll = XmCreateScrolledWindow(  form, 
                                    "XtasButtonsScroll",
                                     args, n );
   XtManageChild( scroll );

   /*= Widget Row : main_row =*/ 
   n = 0;
   XtSetArg( args[n], XmNorientation, XmHORIZONTAL ); n++;
   main_row = XtCreateManagedWidget( "XtasButtonsRow",
                                        xmRowColumnWidgetClass,
                                        scroll,
                                        args, n );
   /*= Widget Column : buttons =*/ 
   for ( i=0; 
         button_options[i].pix_file != -1 ;
         i++ ) 
   {
      button_options[i].selected = True;
   }

   n = 0;
   XtSetArg( args[n], XmNorientation,  XmVERTICAL ); n++;
   buttons = XalButtonMenus( main_row, button_options, args, n, 40, 40 );
   XtManageChild( buttons );

   /*= Widget Column : sub_row =*/ 
   n = 0;
   XtSetArg( args[n], XmNorientation, XmVERTICAL ); n++;
   XtSetArg( args[n], XmNspacing,     21         ); n++;
   sub_row = XtCreateManagedWidget( "XtasButtonsRow",
                                       xmRowColumnWidgetClass,
                                       main_row,
                                      args, n );
   /*= Widget ToggleButton : toggle =*/ 
   for ( i=0; button_menus[i].pix_file != -1 ; i++ )
   {
      n = 0;
      text = XmStringCreateSimple( button_options[i].callback_data );
      XtSetArg( args[n], XmNlabelString,   text); n++;
      XtSetArg( args[n], XmNindicatorType, XmN_OF_MANY); n++;
      XtSetArg( args[n], XmNset,           False); n++;
      toggle = XtCreateManagedWidget( "XtasButtonsToggles",
                                       xmToggleButtonWidgetClass,
                                       sub_row,
                                       args, n );
      XmStringFree( text );

      if ( button_menus[i].selected == True )
      {
         n = 0;
         XtSetArg( args[n], XmNset, True ); n++;
         XtSetValues( toggle, args, n );
      }
      else
      {
         button_options[i].selected = False;
      }

      XtAddCallback(  toggle,
                      XmNvalueChangedCallback, 
                      XtasButtonsChangedCallback,
                     &button_options[i].selected );
   }
   XtManageChild( prompt );
//   XalLimitedLoop( prompt );
}

