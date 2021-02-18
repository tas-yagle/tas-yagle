
#define _GNU_SOURCE 1
#include <dlfcn.h>

#include <alloca.h>

#include "sk.h"

#include MCC_H
#include "mcc_mod_psp_interface.h"

char *psp_op_param_label[ PSP_OP_PARAM_NB ] = { "ids", "vth", "weff" } ;
SK_MODEL *dyn_psp_1020 = NULL ;
SK_MODEL *dyn_psp_1021 = NULL ;

int addpspmodelparam( pspmodelparam *param, sk_unint index, sk_real value ) {

  if( param->n >= param->max )
    return 0;

  param->index[ param->n ] = index ;
  param->value[ param->n ] = value ;
  (param->n)++ ;

  return 1 ;
}

sk_unint getindexparam( trs *ptr, char *name, int paramtype )
{
  sk_unint      i ;
  sk_unint      base ;
  sk_unint      idx ;
  ptype_list   *ptl ;
  ht           *hash ;
  long          n ;
 
  idx = PSP_UNDEF ;

  /* to be optimized with an hash table : name -> index */

  if( ( paramtype & PSP_FIND_MPARAM ) == PSP_FIND_MPARAM ) {
  
    ptl = getptype( ptr->mccmodel->USER, PSPHASHMODELPARAM );
    if( ptl ) {
      hash = (ht*)ptl->DATA ;
    }
    else {
    
      hash = addht( 128 );
      
      for( i=0 ; i < ptr->model->n_model_params ; i++ ) 
        addhtitem( hash, 
                   namealloc( (char*) ptr->model->pp_params[i]->p_name ),
                   i 
                 );

      ptr->mccmodel->USER = addptype( ptr->mccmodel->USER, 
                                      PSPHASHMODELPARAM, 
                                      hash 
                                    );
    }
 
    n = gethtitem( hash, name );
    if( n == EMPTYHT || n == DELETEHT )
      idx = PSP_UNDEF ;
    else
      idx = (sk_unint)n ;
  }
  
  if( ( paramtype & PSP_FIND_IPARAM ) == PSP_FIND_IPARAM ) {

    ptl = getptype( ptr->mccmodel->USER, PSPHASHINSTANCEPARAM );
    if( ptl ) {
      hash = (ht*)ptl->DATA ;
    }
    else {
    
      hash = addht( 8 );
      
      base = ptr->model->n_model_params ;
      for( i=0 ; i < ptr->model->n_inst_params ; i++ ) 
        addhtitem( hash, 
                   namealloc( (char*) ptr->model->pp_params[i+base]->p_name ), 
                   i+base 
                 );

      ptr->mccmodel->USER = addptype( ptr->mccmodel->USER, 
                                      PSPHASHINSTANCEPARAM, 
                                      hash 
                                    );
    }
 
    n = gethtitem( hash, name );
    if( n == EMPTYHT || n == DELETEHT )
      idx = PSP_UNDEF ;
    else
      idx = (sk_unint)n ;
  }
   
  return idx ;
}

void loadmodelparameter( trs *ptr, pspmodelparam *param )
{
  mcc_paramlist *mccparam ;
  sk_unint       i ;
  int            r ;
 
  for( mccparam = ptr->mccmodel->PARAM ; mccparam ; mccparam = mccparam->NEXT ) {
 
    i = getindexparam( ptr, mccparam->NAME, PSP_FIND_MPARAM );
    
    if( i != PSP_UNDEF ) {
      r = addpspmodelparam( param, i, mccparam->VALUE );
      if( !r )
        printf( "too many model parameters ! can't set param %s\n", mccparam->NAME );
    }
    else {
      printf( "model parameter %s isn't supported in psp model\n", mccparam->NAME );
    }
  }
}

void loadinstanceparameter( trs             *ptr,
                            double           L,
                            double           W,
                            elp_lotrs_param *lotrsparam,
                            pspmodelparam   *param 
                          )
{
  char     *label[20] ;
  double    value[20] ;
  int       n ;
  int       j ;
  sk_unint  i ;
  int       r ;

  n=0 ;
  
  label[n  ] = "L" ;
  value[n++] = L ;
  label[n  ] = "W" ;
  value[n++] = W ;

  if( lotrsparam ) {
    label[n  ] = "SA" ;
    value[n++] = lotrsparam->PARAM[elpSA] ;
    label[n  ] = "SB" ;
    value[n++] = lotrsparam->PARAM[elpSB] ;
    label[n  ] = "MULT" ;
    value[n++] = lotrsparam->PARAM[elpM] ;
  }

  for( j=0 ; j<n ; j++ ) {

    i = getindexparam( ptr, namealloc(label[j]), PSP_FIND_IPARAM );

    if( i != PSP_UNDEF ) {
      r = addpspmodelparam( param, i, value[j] );
      if( !r ) 
        printf( "too many instance parameters ! can't set %s\n", label[j] );
    }
    else {
      printf( "instance parameter %s isn't supported in psp model\n", label[j] );
    }
  }
}

void tuneparam( trs *trs, psptunedparam *tuned, pspmodelparam *mparam, pspmodelparam *iparam )
{
  int      i ;
  sk_unint j ;
  sk_unint idx ;
  char     *name ;

  if( tuned ) { /* to be optimized */
  
    for( i=0 ; i<tuned->n ; i++ ) {

      name = namealloc( tuned->param[i] ) ;
    
      idx = getindexparam( trs, name, PSP_FIND_MPARAM );
      
      if( idx != PSP_UNDEF ) { /* model parameter */
      
        for( j = 0 ; j<mparam->n ; j++ ) {
        
          if( mparam->index[j] == idx ) {
            mparam->value[j] = tuned->value[i] ;
            break ;
          }
        }
        
        if( j >= mparam->n ) 
          addpspmodelparam( mparam, idx, tuned->value[i] );
      }
      else { /* instance parameter */
    
        idx = getindexparam( trs, name, PSP_FIND_IPARAM );
        
        if( idx != PSP_UNDEF ) {

          for( j = 0 ; j<iparam->n ; j++ ) {
          
            if( iparam->index[j] == idx ) {
              iparam->value[j] = tuned->value[i] ;
              break ;
            }
          }
          
          if( j >= iparam->n ) 
            addpspmodelparam( iparam, idx, tuned->value[i] );
        }
        else {
          printf( "unknown tuned parameter %s\n", name );
        }
      }
    }
  }
}

void freepspparam( pspmodelparam *param )
{
  mbkfree( param->index );
  mbkfree( param->value );
  mbkfree( param );
}

pspmodelparam* allocpspparam( int size )
{
  pspmodelparam *mparam ;

  mparam = (pspmodelparam*)mbkalloc( sizeof( pspmodelparam ) );
  mparam->max   = size ;
  mparam->n     = 0 ;
  mparam->index = (sk_unint*)mbkalloc( sizeof( sk_unint ) * mparam->max );
  mparam->value = (sk_real*) mbkalloc( sizeof( sk_real )  * mparam->max );

  return mparam ;
}

pspmodelparam* dupmodelparam( pspmodelparam *p )
{
  pspmodelparam *n ;
  
  n = allocpspparam( p->max );
  n->n = p->n ;
  memcpy( n->index, p->index, sizeof( sk_unint ) * p->max );
  memcpy( n->value, p->value, sizeof( sk_real  ) * p->max );

  return n ;
}

void loadmodel( trs             *ptr,
                double           L,
                double           W,
                psptunedparam   *tuned,
                elp_lotrs_param *lotrsparam
              )
{
  SK_ERROR       code ;
  pspmodelparam  *mparam ;
  pspmodelparam  *iparam ;
  pspmodelparam  *tmparam ;
  pspmodelparam  *tiparam ;
  ptype_list     *ptl ;

  ptl = getptype( ptr->mccmodel->USER, PSPCACHEMODEL );
  if( !ptl ) {
    mparam = allocpspparam( ptr->model->n_model_params );
    loadmodelparameter( ptr, mparam );
    if( V_BOOL_TAB[ __AVT_USE_CACHE_PSP ].VALUE )
      ptr->mccmodel->USER = addptype( ptr->mccmodel->USER, 
                                      PSPCACHEMODEL, 
                                      mparam 
                                    );
  }
  else
    mparam = (pspmodelparam*)ptl->DATA ;

  ptl = getptype( ptr->mccmodel->USER, PSPCACHEINSTANCE );
  if( !ptl ) {
    iparam = allocpspparam( ptr->model->n_inst_params );
    loadinstanceparameter( ptr, L, W, lotrsparam, iparam );
    if( V_BOOL_TAB[ __AVT_USE_CACHE_PSP ].VALUE )
      ptr->mccmodel->USER = addptype( ptr->mccmodel->USER, 
                                      PSPCACHEINSTANCE, 
                                      iparam 
                                    );
  }
  else
    iparam = (pspmodelparam*)ptl->DATA ;

  tmparam = dupmodelparam( mparam );
  tiparam = dupmodelparam( iparam );

  tuneparam( ptr, tuned, tmparam, tiparam );
    
  code = ptr->model->p_set_model_params( ptr->mdata,
                                         tmparam->index,
                                         tmparam->value,
                                         tmparam->n,
                                         NULL
                                       );
  switch( code ) {
  case SK_ERR_NONE : 
    break ;
  case SK_ERR_UNKNOWN_PARAMETER :
    printf( "there is at least one unknown model parameter\n" );
    break ;
  case SK_ERR_PARAMETER_NOT_WRITEABLE :
    printf( "there is at least one unwriteable model parameter\n" );
    break ;
  default :
    printf( "unknown error\n");
  }

  code = ptr->model->p_set_inst_params( ptr->idata,
                                        tiparam->index,
                                        tiparam->value,
                                        tiparam->n,
                                        NULL
                                      );
  switch( code ) {
  case SK_ERR_NONE : 
    break ;
  case SK_ERR_UNKNOWN_PARAMETER :
    printf( "there is at least one unknown instance parameter\n" );
    break ;
  case SK_ERR_PARAMETER_NOT_WRITEABLE :
    printf( "there is at least one unwriteable instance parameter\n" );
    break ;
  default :
    printf( "unknown error\n");
  }

  freepspparam( tmparam );
  freepspparam( tiparam );

  if( ! V_BOOL_TAB[ __AVT_USE_CACHE_PSP ].VALUE ) {
    freepspparam( mparam );
    freepspparam( iparam );
  }
}

int initialize( trs             *ptr,
                mcc_modellist   *mccmodel, 
                elp_lotrs_param *lotrsparam,
                double           L,
                double           W,
                double           temp,
                psptunedparam   *tuned 
              )
{
  SK_ERROR    code ;
  int         i ;
  ptype_list *ptl ;
  cachemodel *cache ;

  ptr->mdata = NULL ;
  ptr->idata = NULL ;
  
  if( mccmodel->MODELTYPE == MCC_MPSP )
    ptr->model = dyn_psp_1020 ;
  else
    ptr->model = dyn_psp_1021 ;

  ptr->mccmodel = mccmodel ;

  ptr->simdata.temperature = temp ;
  ptr->simdata.frequency   = (sk_real)1.0 ;
  ptr->simdata.inst_scale  = (sk_real)1.0 ;
 
  if( V_BOOL_TAB[ __AVT_USE_CACHE_PSP ].VALUE && !tuned ) {
    ptl = getptype( mccmodel->USER, PSPCACHETRS ) ;
    if( ptl ) {
      cache = (cachemodel*)ptl->DATA ;
      ptr->mdata     = cache->mdata ;
      ptr->idata     = cache->idata ;
      ptr->variables = cache->variables ;
      ptr->cleanmidata = 0 ;
    }
  }

  if( !ptr->mdata ) {
    ptr->mdata = malloc( ptr->model->model_data_size );
    code = ptr->model->p_init_model( ptr->mdata, SK_DT_GENERIC_N );
    switch( code ) {
    case SK_ERR_NONE :
      break ;
    case SK_ERR_UNKNOWN_TYPE_STRING :
      printf( "  ->unknown type string\n\n" );
      return 0 ;
      break ;
    default :
      printf( "  ->unknown error\n\n" );
      return 0 ;
      break ;
    }
    
    ptr->idata = malloc( ptr->model->inst_data_size );

    code = ptr->model->p_init_inst( ptr->mdata, 
                                    ptr->idata, 
                                    &(ptr->variables),
                                    "piou-piou",
                                    NULL,
                                    NULL
                                  );
    switch( code ) {
    case SK_ERR_NONE :
      break ;
    default :
      printf( "  ->unknown error\n\n" );
      return 0 ;
      break ;
    }
    loadmodel( ptr, L, W, tuned, lotrsparam );

    ptr->model->p_eval_int_params( ptr->mdata, 
                                   ptr->idata, 
                                   &(ptr->simdata), 
                                   SK_IC_GEOM_SCALING | SK_IC_TEMP_SCALING | SK_IC_MULT_SCALING 
                                 );
    if( V_BOOL_TAB[ __AVT_USE_CACHE_PSP ].VALUE && !tuned ) {
      ptr->cleanmidata = 0 ;
      cache = (cachemodel*)mbkalloc( sizeof( cachemodel ) );
      cache->mdata     = ptr->mdata ;
      cache->idata     = ptr->idata ;
      cache->variables = ptr->variables ;
      mccmodel->USER = addptype( mccmodel->USER, PSPCACHETRS, cache );
    }
    else
      ptr->cleanmidata = 1 ;
  }

  for( i=0 ; i<PSP_OP_PARAM_NB ; i++ )
    ptr->tabid[i] = PSP_UNDEF ;
      
  return 1 ;
}

void terminate( trs *ptr )
{
  if( ptr->cleanmidata ) {
    free( ptr->mdata );
    free( ptr->idata );
  }
}

sk_unint get_id_param( trs *ptr, int param )
{
  sk_unint      base ;
  sk_unint      i ;

  if( ptr->tabid[ param ] == PSP_UNDEF ) {
    base = ptr->model->n_model_params + ptr->model->n_inst_params ;
    for( i = 0 ; i < ptr->model->n_op_info ; i++ ) {
      if( !strcasecmp( ptr->model->pp_params[i+base]->p_name, 
                       psp_op_param_label[ param ] ) )
        break ;
    }
    if( i < ptr->model->n_op_info )
      ptr->tabid[ param ] = i ;
    else {
      printf( "can't find operating point #%d\n", param );
      exit(1);
    }
  }

  return ptr->tabid[ param ] ;
}

void set_polarization( trs *ptr, double vgs, double vds, double vbs )
{
  const SK_NODE *node[2] ;
  sk_unint       i ;
  int            j ;
  double         v[2];
  double         vg, vd, vs, vb ;
  int            flag ;

  vs = 0.0 ;
  vg = vgs + vs ;
  vd = vds + vs ;
  vb = vbs + vs ;
  
  for( i = 0 ; i < ptr->model->n_ev ; i++) {
  
    node[0] = ptr->variables->p_ev[i].p_ev_descrip->p_pos_node ;
    node[1] = ptr->variables->p_ev[i].p_ev_descrip->p_neg_node ;
    for( j=0 ; j<2 ; j++ ) {
      switch( node[j]->number ) {
      case 0 :  v[j] = (sk_real)vd ; break ;
      case 1 :  v[j] = (sk_real)vg ; break ;
      case 2 :  v[j] = (sk_real)vs ; break ;
      default : v[j] = (sk_real)vb ; break ;
      }
    }
    ptr->variables->p_ev[i].value = v[0]-v[1] ;
  }
  
  flag = SK_EC_CURRENTS | SK_EC_CHARGES | SK_EC_OP_INFO | SK_EC_CAPACITANCES ;
  ptr->model->p_eval_model( ptr->mdata, ptr->idata, &( ptr->simdata ), flag );

}

int psp_loaddynamiclibrary( void )
{
  void *lib ;
  char libname[]="libnxp_models.so" ;
  Dl_info m ;

  printf( "Opening dynamic library %s.\n", libname );
  lib = dlopen( libname, RTLD_NOW );
  if( !lib ) {
    printf( "can't load dynamic library %s.\n", libname );
    return 0 ;
  }

  dyn_psp_1020 = dlsym( lib, "psp_1020" );
  dyn_psp_1021 = dlsym( lib, "psp_1021" );
  if( !dyn_psp_1020 || !dyn_psp_1021 ) {
    printf( "can't find dynamic function !\n" );
    return 0 ;
  }
  
  if( dladdr( dyn_psp_1020->p_init_model, &m ) != -1 )
    printf( "  -> %s\n", m.dli_fname );
  else
    printf( "  -> can't get information about the loaded library.\n" );
   
  return 1 ;
}

void mcc_clean_psp_interface( mcc_modellist *mccmodel, int check )
{
  ptype_list *ptl ;
  cachemodel *cache ;

  ptl = getptype( mccmodel->USER, PSPCACHEINSTANCE );
  if( ptl ) {
    if( check )
      printf( "warning : non empty instance cache.\n" );
    freepspparam( (pspmodelparam*)ptl->DATA );
    mccmodel->USER = delptype( mccmodel->USER, PSPCACHEINSTANCE );
  }

  ptl = getptype( mccmodel->USER, PSPCACHETRS );
  if( ptl ) {
    if( check )
      printf( "warning : non empty model cache.\n" );
    cache = (cachemodel*)ptl->DATA ;
    mbkfree( cache->mdata );
    mbkfree( cache->idata );
    mbkfree( cache );
    mccmodel->USER = delptype( mccmodel->USER, PSPCACHETRS );
  }
}
