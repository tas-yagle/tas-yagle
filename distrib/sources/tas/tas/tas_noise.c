#include "tas.h"

float tas_rsslistbranch( branch_list* );
float tas_rssbranch( branch_list* );

void tas_rssfig( cnsfig_list *cnsfig )
{
  cone_list     *cone;
  edge_list     *incone;
  long           type[2];
  int            nbtype=2;
  int            n;
  delay_list    *delay;
  char           flag;
  float          rup, rdw;
  ptype_list    *ptl;
  noise_scr     *model_scr;

  type[0] = TAS_DELAY_MAX;
  type[1] = TAS_DELAY_MIN;
  
  for( cone = cnsfig->CONE ; cone ; cone = cone->NEXT ) {
  
    flag='N';

    for( incone = cone->INCONE ; incone ; incone = incone->NEXT ) {

      for( n=0 ; n<nbtype ; n++ ) {
      
        ptl = getptype( incone->USER, type[n] );
        
        if( !ptl ) continue;
        
        delay = (delay_list*)ptl->DATA;
        
        if( delay->TMLH || delay->TMHL || delay->TMLL || delay->TMHH ) {
        
          if( flag=='N' ) {
            flag='Y';
            tas_rssgate( cone, &rdw, &rup );
          }
          
          if( delay->TMHL && 
              stm_noise_getmodeltype( delay->TMHL ) == STM_NOISE_SCR 
            ) {
            model_scr = stm_noise_getmodel_scr( delay->TMHL );
            stm_noise_scr_update_resi( model_scr, rup );
          }
          
          if( delay->TMLH && 
              stm_noise_getmodeltype( delay->TMLH ) == STM_NOISE_SCR 
            ) {
            model_scr = stm_noise_getmodel_scr( delay->TMLH );
            stm_noise_scr_update_resi( model_scr, rdw );
          }
          
          if( delay->TMLL && 
              stm_noise_getmodeltype( delay->TMLL ) == STM_NOISE_SCR
            ) {
            model_scr = stm_noise_getmodel_scr( delay->TMLL );
            stm_noise_scr_update_resi( model_scr, rup );
          }
          
          if( delay->TMHH && 
              stm_noise_getmodeltype( delay->TMHH ) == STM_NOISE_SCR 
            ) {
            model_scr = stm_noise_getmodel_scr( delay->TMHH );
            stm_noise_scr_update_resi( model_scr, rdw );
          }
        }
      }
    }
  }
}

/* Calcule la résistance petits signaux d'un cône */
void tas_rssgate( cone_list *cone, float *rssdown, float *rssup )
{
  float        rssvdd=-1.0;
  float        rssvss=-1.0;
  float        rssext=-1.0;
  float        rssgnd=-1.0;
  
  if( rssdown ) {
    *rssdown = 0.0;

    if( rssvss < 0.0 ) 
      rssvss = tas_rsslistbranch( cone->BRVSS );
    *rssdown = rssvss ;
    
    if( rssgnd < 0.0 ) 
      rssgnd = tas_rsslistbranch( cone->BRGND );
    if( rssgnd > *rssdown )
      *rssdown = rssgnd ;

    if( rssext < 0.0 ) 
      rssext = tas_rsslistbranch( cone->BREXT );
    if( rssext > *rssdown )
      *rssdown = rssext ;
  }

  if( rssup ) {
    *rssup = 0.0;

    if( rssvdd < 0.0 ) 
      rssvdd = tas_rsslistbranch( cone->BRVDD );
    *rssup = rssvdd;

    if( rssext < 0.0 ) 
      rssext = tas_rsslistbranch( cone->BREXT );
    if( rssext > *rssup )
      *rssup = rssext ;
  }
}

/* Renvoie la résistance maximum d'une liste de branches */
float tas_rsslistbranch( branch_list *head ) 
{
  branch_list *branch ;
  float        r, rmax ;
  
  rmax = 0.0;
  for( branch = head ; branch ; branch = branch->NEXT ) {
    r = tas_get_rlin_br( branch->LINK );
    if( r > rmax )
      rmax = r;
  }

  return rmax ;
}

/* Renvoie la résistance d'une branche
   NE PAS UTILISER : NE COMPTE PAS LA TAILLE DES TRANSISTORS */
float tas_rssbranch( branch_list *branch )
{
  link_list *link;
  lotrs_list *lotrs;
  float r;
  
  r=0.0;
  
  for( link = branch->LINK ; link ; link = link->NEXT ) {

    if( ( link->TYPE & CNS_EXT ) != CNS_EXT ) {
      lotrs = link->ULINK.LOTRS;
      r = r + elpLotrsResiCanal( lotrs, TAS_CASE );
    }
  }

  return r;
}
