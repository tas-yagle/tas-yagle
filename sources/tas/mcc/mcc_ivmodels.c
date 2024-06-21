#include MCC_H
#include "mcc_mod_spice.h"
#include "mcc_ivmodels.h"
#include "mcc_genspi.h"

#define MCC_RINF ((float)1.0e20)
#define MCC_DEFAULT_ARNS ((float)0.9)


char mcc_trans_is_reverse( mcc_trans_mcc *trsmcc, float ids )
{
  if( MLO_IS_TRANSN( trsmcc->TYPE ) && ids < 0.0 ) return 1 ;
  if( MLO_IS_TRANSP( trsmcc->TYPE ) && ids > 0.0 ) return 1 ;
  return 0 ;
}

/******************************************************************************\
Paramètres pour le transistor MCC
Si le signal losigdrain est spécifié, prend en compte les résistances d'accès
sur le drain et la source.
\******************************************************************************/

mcc_trans_mcc* mcc_create_trans_mcc( lotrs_list *lotrs, float vdd, int transcase, losig_list *losigdrain, float vg, float vb )
{
  mcc_trans_mcc   *trs;
  elpmodel_list   *model;
  double           imax;
  
  trs = (mcc_trans_mcc*)mbkalloc( sizeof( mcc_trans_mcc ) );
  model  = elpGetModel( lotrs, vdd, transcase);

  elpLotrsGetShrinkDim( lotrs, &trs->TRLENGTH,
                               &trs->TRWIDTH,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               NULL,
                               transcase
                      );
  trs->TYPE   = lotrs->TYPE;
  trs->VDD    = vdd ;
  trs->A      = model->elpModel[elpA];
  trs->B      = model->elpModel[elpB];
  trs->RNT    = model->elpModel[elpRT];
  trs->RNS    = model->elpModel[elpRS];
  trs->VLIN   = vdd * model->elpModel[elpKRT] ;
  trs->VSAT   = vdd * model->elpModel[elpKRS] ;
  trs->VT     = model->elpModel[elpVT];
  trs->KVT    = model->elpModel[elpKT];
  trs->KS     = model->elpModel[elpKS];
  trs->KR     = model->elpModel[elpKR];

  if( losigdrain ) {
    if( lotrs->DRAIN->SIG == losigdrain ) {
      trs->RD     = model->elpRacc[elpRACCD];
      trs->RS     = model->elpRacc[elpRACCS];
    }
    else {
      trs->RS     = model->elpRacc[elpRACCD];
      trs->RD     = model->elpRacc[elpRACCS];
    }
  }
  else {
    trs->RD     = 0.0 ;
    trs->RS     = 0.0 ;
  }

  if( trs->RNS < 1.0 ) {
    imax = trs->A * ( trs->VDD - trs->VT ) * ( trs->VDD - trs->VT ) / 
           ( 1.0 + trs->B * ( trs->VDD - trs->VT ) );
    trs->RNS = ( trs->VDD - trs->RNT * MCC_DEFAULT_ARNS * imax ) /
               ( imax * ( 1.0 - MCC_DEFAULT_ARNS ) ) ;
  }

  trs->VG = vg ;
  trs->VB = vb ;

  return trs;
}

void mcc_delete_trans_mcc( mcc_trans_mcc *trs )
{
  mbkfree( trs );
}

/******************************************************************************\
Caractéristiques transistor MCC
\******************************************************************************/

char mcc_mcc_characteristic( mcc_trans_mcc *trs,
                             float          vgs,
                             float          vbs,
                             mcc_model     *model,
                             int           *nb,
                             int            max
                           )
{
  float          kres ;
  float          x ;
  float          isat ;
  float          vdd ;
  float          a ;
  float          b ;
  float          rnt ;
  float          rns ;
  float          vt ;
  float          kr ;
  float          ks ;
  float          imax ;
  float          wl ;
  float          ires ;
  float          ures ;
  float          rlin ;
  float          plin ;
  float          rsat ;
  float          psat ;
  float          csat ;
  float          vlin ;
  float          ilin ;
  float          vsat ;
  float          di ;
  float          dv ;
  float          a0, b0 ;
  float          a1, b1 ;
  float          a2, b2 ;
  float          a3, b3 ;
  float          a4, b4 ;
  float          iit ;
  float          im ;
  float          ures2 ;
  float          ires2 ;
  float          iit2 ;
  float          im2 ;
  float          rsatmin ;

  if( !mcc_mcc_vth( trs, vbs, trs->VDD, &vt ) )
    return 0;

  vdd  = trs->VDD;
  a    = trs->A;
  b    = trs->B;
  rnt  = trs->RNT;
  rns  = trs->RNS;
  kr   = trs->KR;
  ks   = trs->KS;
  vlin = ( vgs - vt ) / ( vdd - vt ) * trs->VLIN;
  vsat = ( vgs - vt ) / ( vdd - vt ) * trs->VSAT;

  if( rns == 0.0 ) return 0;
  if( vgs < vt ) {
    model[0].VMAX = vdd ;
    model[0].A    = 1.0/MCC_RINF ;
    model[0].B    = 0.0 ;
    *nb           = 1 ;
    return 1;
  }
  
  wl   = ((float)trs->TRWIDTH) / ((float)trs->TRLENGTH);
 
  a    = a * wl;
  rnt  = rnt / wl;
  rns  = rns / wl;

  imax    = a*(vdd-vt)*(vdd-vt)/(1.0+b*(vdd-vt));
  isat    = a*(vgs-vt)*(vgs-vt) / ( 1.0 + b*(vgs-vt) );
  rsat    = ( ks*rns*(vdd-vt) + rns*(1.0-ks)*(vgs-vt) ) / (vgs-vt) ;
  rsatmin = vdd/isat ;

  ures = -1.0 ; 
  if( rsat > rsatmin ) {

    x    = ( imax * rnt - vdd*rnt/rns )/( 1.0 - rnt/rns );
    kres = x/(vdd-vt);
    ures = kr * kres * (vgs-vt) + kres*(1.0-kr)*(vdd-vt) ;

    if( ures < 1.1*vlin )
      ures = 1.1*vlin ;
  }
  
  if( ures > 0.0 ) {
    ires    = isat - (vdd-ures)/rsat ;
    rlin    = ures / ires ;

    if( ures > vlin ) {
      ilin = vlin / rlin;
      plin =  1.0 / rlin;

      isat = isat - ( vdd - vsat ) / rsat;
      psat = 1.0 / rsat;
      csat = isat - vsat * psat;

      // Version avec 5 segments
      {
        di   = isat - ilin;
        dv   = vsat - vlin;

        a0   = di / dv;
        b0   = ilin - a0 * vlin;
      
        iit  = a0 * ures + b0 ;
        im   = ( ires + iit ) / 2.0 ;
      
        a1   = ( im - ilin ) / ( ures - vlin ) ;
        b1   = ilin - a1 * vlin ;
      
        a2   = ( isat - im ) / ( vsat - ures ) ;
        b2   = isat - a2 * vsat ;
      
        ures2 = ( csat - b1 ) / ( a1 - psat );
        ires2 = psat * ures2 + csat ;
      
        iit2 = ( a2 * ures2 ) + b2 ;
        im2  = ( ires2 - iit2 ) / 2.0 + iit2 ;
      
        a3   = ( im2 - im ) / ( ures2 - ures ) ;
        b3   = im2 - a3 * ures2 ;
      
        a4   = ( isat - im2 ) / ( vsat - ures2 ) ;
        b4   = isat - a4 * vsat ;

        if( max < 5 ) 
          return 0;
        
        model[0].VMAX = vlin  ;
        model[0].A    = plin  ;
        model[0].B    = 0.0   ;
        model[1].VMAX = ures  ;
        model[1].A    = a1    ;
        model[1].B    = b1    ;
        model[2].VMAX = ures2 ;
        model[2].A    = a3    ;
        model[2].B    = b3    ;
        model[3].VMAX = vsat  ;
        model[3].A    = a4    ;
        model[3].B    = b4    ;
        model[4].VMAX = vdd   ;
        model[4].A    = psat  ;
        model[4].B    = csat  ;
        *nb = 5 ;
      }
    }
    else {
      model[0].VMAX = ures     ;
      model[0].A    = 1.0/rlin ;
      model[0].B    = 0.0      ;
      model[1].VMAX = vdd      ;
      model[1].A    = 1.0/rsat ;
      model[1].B    = isat - vdd/rsat ;
      *nb = 2 ;
    }
  }
  else {
  
    model[0].VMAX = vdd ;
    model[0].A    = 1.0/rsatmin ;
    model[0].B    = 0.0 ;
    *nb           = 1 ;
    
  }

  return 1;
}

char mcc_mcc_vth( mcc_trans_mcc *trs,
                  float vbs,
                  float vds,
                  float *vth
                )
{
  vds=0.0; // unused parameter
  *vth = trs->VT + trs->KVT * vbs ;
  return 1;
}

char mcc_mcc_ids( mcc_trans_mcc *trs, 
                  float vs, 
                  float vd, 
                  float *ids 
                )
{
  mcc_model model[5];
  int n, i ;
  float vgs, vbs, vds ;
  float x ;
  char reverse ;
  float vt ;
  float wl ;

  if( ( MLO_IS_TRANSN( trs->TYPE ) && vs > vd ) ||
      ( MLO_IS_TRANSP( trs->TYPE ) && vs < vd )    ) {
    /* transistor is degraded */
    x = vs ;
    vs = vd ;
    vd = x ;
    reverse = 1 ;
  }
  else
    reverse = 0 ;
  
  vgs = trs->VG-vs ;
  vbs = trs->VB-vs ;
  vds = vd-vs ;

  if( MLO_IS_TRANSP(trs->TYPE) ) {
    vgs = -vgs ;
    vds = -vds ;
  }

  if( MLO_IS_TRANSN(trs->TYPE) ) {
    vbs = -vbs ;
  }
  
  if( !mcc_mcc_vth( trs, vbs, trs->VDD, &vt ) ) {
    *ids = 0.0 ;
    return 0;
  }

  if( vds > 0.999 * trs->VDD && vds < 1.001 * trs->VDD && vgs > vt ) {
    wl   = ((float)trs->TRWIDTH) / ((float)trs->TRLENGTH);
    *ids = wl * trs->A*(vgs-vt)*(vgs-vt) / ( 1.0 + trs->B*(vgs-vt) );
  }
  else {
    if( !mcc_mcc_characteristic( trs, vgs, vbs, model, &n, 5 ) ) {
      *ids = 0.0;
      return 0;
    }

    for( i = 0 ; ( i < n ) && ( model[i].VMAX < vds ) ; i++ );
    
    if( i >= n ) // si on a un vds > vdd, on prend le dernier segment de droite.
      i = n-1 ;
    
    *ids = model[i].A * vds + model[i].B ;
  }

  if( MLO_IS_TRANSP(trs->TYPE) )
    *ids = -(*ids);

  if( reverse )
    *ids = -(*ids);
   
  return 1;
}

/* La tension initiale dans *vds est la tension maximum de la branche */
char mcc_mcc_vds( mcc_trans_mcc *trs, 
                  float vs, 
                  float ids, 
                  float *vds 
                )
{
  mcc_model  model[5] ;
  int        n, i ;
  float      imax ;
  float      vgs ;
  float      vbs ;
  int        found ;
  float      vt ;
  float      wl ;

  if( mcc_trans_is_reverse( trs, ids ) == 1 ) {
    avt_errmsg(MCC_ERRMSG, "003", AVT_ERROR);
    return 0;
  }
  
  vgs = trs->VG - vs ;
  vbs = trs->VB - vs ;
  
  if( MLO_IS_TRANSP(trs->TYPE) ) {
    vgs = -vgs ;
    ids = -ids ;
  }

  if( MLO_IS_TRANSN(trs->TYPE) ) {
    vbs = -vbs ;
  }
 
  vgs = vgs - trs->RS*ids ;
  vbs = vbs + trs->RS*ids ;

  found = 0 ;

  if( vgs > 0.999*trs->VDD && vgs < 1.001*trs->VDD ) {
    if( !mcc_mcc_vth( trs, vbs, trs->VDD, &vt ) ) {
      *vds = 0.0 ;
      return 0;
    }
    wl   = ((float)trs->TRWIDTH) / ((float)trs->TRLENGTH);
    imax = wl * trs->A*(vgs-vt)*(vgs-vt) / ( 1.0 + trs->B*(vgs-vt) );
    if( ids > 0.999*imax && ids < 1.001*imax ) {
      *vds = trs->VDD ;
      found = 1 ;
    }
  }
 
  if( found == 0 ) {
    if( !mcc_mcc_characteristic( trs, vgs, vbs, model, &n, 5 ) ) {
      *vds = 0.0;
      return 0;
    }

    for( i = 0 ; i < n ; i++ ) {
      imax = model[i].VMAX * model[i].A + model[i].B ;
      if( ids < imax )
        break ;
    }
    
    if( i >= n ) // si on a un vds > vdd, on prend le dernier segment de droite.
      i = n-1 ;

    *vds = ( ids - model[i].B ) / model[i].A ;

    *vds = *vds + ( trs->RS + trs->RD ) * ids ;
  }
    
  if( MLO_IS_TRANSP(trs->TYPE) ) {
    *vds = -(*vds);
  }

  return 1;
}

void mcc_mcc_plot( mcc_trans_mcc *trs, float vdmax, float vs )
{
  FILE *file;
  int   n;
  int   nmax=50;
  float v;
  float i;
  
  file = mbkfopen( "trsvbs","dat", WRITE_TEXT );

  for( n=0 ; n<=nmax ; n++ ) {
    v = n*vdmax/nmax;
    if( mcc_mcc_ids( trs, vs, v, &i ) )
    {
      if( MLO_IS_TRANSN(trs->TYPE) )
        fprintf( file, "%g %g\n", v, i );
      else
        fprintf( file, "%g %g\n", vdmax-v, i );
    }
        
  }

  fclose( file );
}

/******************************************************************************\
Paramètres pour le transistor BSIM3
Si le signal losigdrain est spécifié, prend en compte les résistances d'accès
sur le drain et la source.
\******************************************************************************/

mcc_trans_spice* mcc_create_trans_spice( lotrs_list *lotrs, float vdd, losig_list *losigdrain, float vg, float vb )
{
  mcc_trans_spice* trs;
  elp_lotrs_param* lotrsparam=NULL;
  double rd, rs ;

  trs           = (mcc_trans_spice*)mbkalloc( sizeof( mcc_trans_spice ) ) ;
  
  trs->MODELFILE  = MCC_MODELFILE ;
  trs->MODELTYPE  = mcc_getmodeltype (MCC_MODELFILE) ;
  trs->TRANSNAME  = getlotrsmodel( lotrs ) ;
  trs->TRANSTYPE  = MLO_IS_TRANSN(lotrs->TYPE) ? MCC_NMOS : MCC_PMOS ;
  trs->TRANSCASE  = MLO_IS_TRANSN(lotrs->TYPE) ? MCC_NCASE : MCC_PCASE;
  trs->TRLENGTH   = ((float)lotrs->LENGTH) * 1.0e-6 / ((float)SCALE_X);
  trs->TRWIDTH    = ((float)lotrs->WIDTH)  * 1.0e-6 / ((float)SCALE_X);
  if (lotrs->XD<0) trs->AD=0;
  else
    trs->AD         = (((float)lotrs->XD) * ((float)lotrs->WIDTH) ) * 1.0e-12 /
                    ( ((float)SCALE_X) * ((float)SCALE_X) );
  if (lotrs->PD<0) trs->PD=0;
  else
    trs->PD         = (((float)lotrs->PD) ) * 1.0e-6 / ((float)SCALE_X);
  if (lotrs->XS<0) trs->AS=0;
  else 
    trs->AS         = (((float)lotrs->XS) * ((float)lotrs->WIDTH) ) * 1.0e-12 /
                    ( ((float)SCALE_X) * ((float)SCALE_X) );
  if (lotrs->PS<0) trs->PS=0;
  else
    trs->PS         = (((float)lotrs->PS) ) * 1.0e-6 / ((float)SCALE_X);
  trs->TEMP       = MCC_TEMP ;
  trs->VDD        = vdd ;
  if ( (lotrsparam = elp_lotrs_param_create (lotrs)) ) {
    if ( lotrsparam->VBULK < ELPMINVBULK )
      lotrsparam->VBULK = (trs->TRANSTYPE == MCC_NMOS) ? 0.0 : vdd;
  }
  trs->PARAM      = lotrsparam;
  trs->LOTRS      = lotrs ; // necessaire pour récupérer le subckt...
 
  mcc_update_technoparams( trs->TRANSNAME, trs->TRANSTYPE == MCC_NMOS ? MCC_TRANS_N : MCC_TRANS_P, trs->TRLENGTH*1e6, trs->TRWIDTH*1e6, trs->LOTRS, trs->TRANSCASE );
  mcc_calcRACCESS( trs->MODELFILE, trs->TRANSNAME, trs->TRANSTYPE, trs->TRANSCASE, trs->TRLENGTH, trs->TRWIDTH, trs->PARAM, &rs, &rd );

  if( losigdrain ) {
    if( lotrs->DRAIN->SIG == losigdrain ) {
      trs->RD = rd ;
      trs->RS = rs ;
    }
    else {
      trs->RS = rd ;
      trs->RD = rs ;
    }
  }
  else {
    trs->RD = 0.0 ;
    trs->RS = 0.0 ;
  }
 
  trs->VG = vg ;
  trs->VB = vb ;
  return trs ;
}

void mcc_delete_trans_spice( mcc_trans_spice *trs )
{
  elp_lotrs_param_free (trs->PARAM);
  mbkfree( trs );
}

/******************************************************************************\
Caractéristiques transistor BSIM3
\******************************************************************************/

char mcc_spice_vds_list( chain_list *trs,
                         float vs,
                         float ids,
                         float *vds
                       )
{
  /* to be done : taking into account all transistor in the list */
  mcc_trans_spice *spitrs ;
  float            vgs, vbs ;
  char             ret ;

  spitrs = (mcc_trans_spice*)trs->DATA ;
  if (spitrs->LOTRS) MCC_CURRENT_LOTRS=spitrs->LOTRS;
  vgs = spitrs->VG - vs ;
  vbs = spitrs->VB - vs ;
  ret = mcc_spice_vds( spitrs, vgs, vbs, ids, vds );
  return ret ;
}

char mcc_spice_ids_list( chain_list *trs,
                         float vs,
                         float vd,
                         float *ids
                       )
{
  chain_list      *chain ;
  float            vgs, vbs, vds ;
  mcc_trans_spice *spitrs ;
  float            i ;
  char             ret ;

  *ids = 0.0 ;
  for( chain = trs ; chain ; chain = chain->NEXT ) {
  
    spitrs = (mcc_trans_spice*)chain->DATA ;

    vgs = spitrs->VG - vs ;
    vbs = spitrs->VB - vs ;
    vds = vd - vs ;

    ret = mcc_spice_ids_pol( spitrs, vgs, vbs, vds, &i );
    if( !ret )
      return 0 ;
    
    *ids = *ids + i ;
  }

  return 1 ;
}

char mcc_spice_ids( mcc_trans_spice *trs, 
                    float vgs, 
                    float vbs, 
                    float vds, 
                    float *ids 
                  )
{
  *ids = mcc_calcIDS( trs->MODELFILE,
                      trs->TRANSNAME,
                      trs->TRANSTYPE,
                      trs->TRANSCASE,
                      vbs,
                      vgs,
                      vds,
                      trs->TRLENGTH,
                      trs->TRWIDTH,
                      trs->TEMP,
                      trs->PARAM
                    );
  return 1;
}

/* La tension initiale dans *vds est la tension maximum de la branche */
char mcc_spice_vds( mcc_trans_spice *trs, 
                    float vgs, 
                    float vbs, 
                    float ids, 
                    float *vds 
                  )
{
  float vdsmax ;
  float vdsmin ;
  float i ;
  float idsmax ;
  float idsmin ;
  float v ;
  int   n=50 ;

  vgs = vgs - trs->RS * ids ;
  vbs = vbs - trs->RS * ids ;
  
  if( ids > 0.0 ) {
    vdsmin = 0.0 ;
    idsmin = 0.0 ;
    vdsmax =  *vds ;
    if( !mcc_spice_ids( trs, vgs, vbs, vdsmax, &idsmax ) ) {
      *vds = 0.0;
      return 0;
    }
    if( idsmax < ids ) {
      *vds = ids*vdsmax/idsmax ;
      return 1;
    }
  }
  else {
    vdsmin = *vds ;
    if( !mcc_spice_ids( trs, vgs, vbs, vdsmin, &idsmin ) ) {
      *vds = 0.0;
      return 0;
    }
    vdsmax = 0.0 ;
    idsmax = 0.0 ;
    if( ids < idsmin ) {
      *vds = ids*vdsmin/idsmin ;
      return 1;
    }
  }

  do
  {
    n--;

    v = (vdsmax-vdsmin)/2.0 + vdsmin;
    
    if( ! mcc_spice_ids( trs, vgs, vbs, v, &i ) ) {
      *vds = 0.0;
      return 0;
    }
    if( i > ids ) {
      vdsmax = v ;
      idsmax = i ;
    }
    else {
      vdsmin = v ;
      idsmin = i ;
    }
  }
  while( vdsmax-vdsmin > 1.0e-3 && n );

  if( ! n ) {
    *vds = 0.0;
    return 0;
  }

  *vds = (ids-idsmin)*(vdsmax-vdsmin)/(idsmax-idsmin) + vdsmin ;

  *vds = *vds + (trs->RD+trs->RS)*ids ;
  return 1;
}

char mcc_spice_vth( mcc_trans_spice *trs,
                    float vbs,
                    float vds,
                    float *vth
                  )
{
  *vth = mcc_calcVTH( trs->MODELFILE,
                      trs->TRANSNAME,
                      trs->TRANSTYPE,
                      trs->TRANSCASE,
                      trs->TRLENGTH,
                      trs->TRWIDTH,
                      trs->TEMP,
                      vbs,
                      vds,
                      trs->PARAM,
                      MCC_NO_LOG
                    );
  return 1;
}

char mcc_spice_trace_vth( mcc_trans_spice *trs, 
                          float vbsmin, 
                          float vbsmax, 
                          float vds,
                          char  *filename
                        )
{
  float step;
  float vbs;
  float vth;
  int   i=200;
  FILE  *file;

  file = mbkfopen( filename, "plot", "w" );
  
  step = (vbsmax-vbsmin)/((float)i);
  for( ; i ; i-- ) {
    vbs = vbsmax-step*i;
    if( mcc_spice_vth( trs, vbs, vds, &vth ) )
      fprintf( file, "%g %g\n", vbs, vth );
  }

  fclose( file );

  return 1;
}

void mcc_spice_plot( mcc_trans_spice *trs, float vdsmax, float vgs, float vbs )
{
  FILE *file;
  int   n;
  int   nmax=50;
  float v;
  float i;
  
  file = mbkfopen( "trsvbs","dat", WRITE_TEXT );

  for( n=0 ; n<=nmax ; n++ ) {
    v = n*vdsmax/nmax;
    if( mcc_spice_ids( trs, vgs, vbs, v, &i ) )
      fprintf( file, "%g %g\n", v, i );
        
  }

  fclose( file );
}

/******************************************************************************\
Polarise correctement les transistors.
\******************************************************************************/
char mcc_spice_ids_pol( mcc_trans_spice *trs, 
                        float vgs, 
                        float vbs, 
                        float vds, 
                        float *ids 
                      )
{
  char ineg ;
  float vdsr, vgsr, vbsr;

  if( trs->TRANSTYPE == MCC_NMOS ) {
    if( vds < 0.0 ) {
      // pour un tn, vds doit etre positif
      vdsr = -vds ;
      vgsr = vgs - vds ;
      vbsr = vbs - vds ;
      ineg = 1;
    }
    else {
      vdsr = vds ;
      vgsr = vgs ;
      vbsr = vbs ;
      ineg = 0;
    }
  }
  else {
    if( vds > 0.0 ) {
      // pour un tp, vds doit etre negatif
      vdsr = -vds ;
      vgsr = vgs - vds ;
      vbsr = vbs - vds ;
      ineg = 1;
    }
    else {
      vdsr = vds ;
      vgsr = vgs ;
      vbsr = vbs ;
      ineg = 0;
    }
  }
  
  mcc_update_technoparams( trs->TRANSNAME, trs->TRANSTYPE == MCC_NMOS ? MCC_TRANS_N : MCC_TRANS_P, trs->TRLENGTH*1e6, trs->TRWIDTH*1e6, trs->LOTRS, trs->TRANSCASE );
  *ids = mcc_calcIDS( trs->MODELFILE,
                      trs->TRANSNAME,
                      trs->TRANSTYPE,
                      trs->TRANSCASE,
                      vbsr,
                      vgsr,
                      vdsr,
                      trs->TRLENGTH,
                      trs->TRWIDTH,
                      trs->TEMP,
                      trs->PARAM
                    );

  if( ineg == 1 )
    *ids = -(*ids);

  return 1;
}

char mcc_mcc_ids_list( chain_list *headtrs,
                       float vs,
                       float vd,
                       float *ids
                     )
{
  float       i ;
  char        r ;
  chain_list *chain ;
  
  *ids=0.0;
  
  for( chain = headtrs ; chain ; chain = chain->NEXT ) {
  
    r = mcc_mcc_ids( (mcc_trans_mcc*)chain->DATA, vs, vd, &i );
    if( !r )
      return 0 ;

    *ids = *ids + i ;
  }
  return 1 ;
}

void mcc_mcc_vds_list_compute_raccess( chain_list *headtrs, float *rs, float *rd )
{
  chain_list *chain ;
  mcc_trans_mcc *trs ;

  /* Computing acces resistances. This is an approximative approach : we
     consider all access resistance in parallel. */
  *rs = 0.0 ;
  *rd = 0.0 ;
  
  for( chain = headtrs ; chain ; chain = chain->NEXT ) {
  
    trs = (mcc_trans_mcc*)chain->DATA ;
    
    if( trs->RS > 0.0 )
      *rs = *rs + 1.0/trs->RS ;

    if( trs->RD > 0.0 )
      *rd = *rd + 1.0/trs->RD ;
  }

  if( *rs > 0.0 )
    *rs = 1.0 / *rs ;
    
  if( *rd > 0.0 )
    *rd = 1.0 / *rd ;
  
}

char mcc_mcc_vds_list_compute_sum_current( chain_list *headtrs, float vs, float vd, float *i )
{
  chain_list    *chain ;
  mcc_trans_mcc *trs ;
  float          itrs ;
  char           r ;

  *i = 0.0 ;
  for( chain = headtrs ; chain ; chain = chain->NEXT ) {

    trs = (mcc_trans_mcc*)chain->DATA ;
    r = mcc_mcc_ids( trs, vs, vd, &itrs );
    if( !r )
      return 0 ;
    *i = *i + itrs ;
  }

  return 1 ;
}

/* estimate the vds of the group of parallel transistor. if all transistor are
   off, return exactly 0.0 */
char mcc_mcc_vds_list_estim_vds( chain_list *headtrs, float vs, float ids, float *vds )
{
  float          itrs ;
  chain_list    *chain ;
  mcc_trans_mcc *trs ;
  char           r ;
  
  /* find a non degraded transistor to get an estimation of the current */
  // itrs = ids / ((float)countchain( headtrs )) ;
  itrs = ids ; // to find the boundary faster in the mcc_mcc_vdd_list()
  for( chain = headtrs ; chain ; chain = chain->NEXT ) {
    trs = (mcc_trans_mcc*)chain->DATA ;
    if( mcc_trans_is_reverse( trs, itrs ) == 0 ) {
      r = mcc_mcc_vds( trs, vs, itrs, vds ) ;
      if( r==0 )
        return 0 ;
      if( fabs(*vds) < 10.0 )
        return 1 ;
    }
  }

  /* find an estimation of the current among all of the degraded transistor */

  /* Since non degraded is blocked, all current go throw the degraded transistor */
  itrs = ids ;
  for( chain = headtrs ; chain ; chain = chain->NEXT ) {
    trs = (mcc_trans_mcc*)chain->DATA ;
    if( mcc_trans_is_reverse( trs, itrs ) == 1 ) {
      if( MLO_IS_TRANSN( trs->TYPE ) )
        vs = 0.0 ;
      else
        vs = trs->VDD ;
      r = mcc_mcc_vds( trs, vs, -itrs, vds );
      if( r==0 )
        return 0 ;
      *vds = -*vds ;
      if( fabs(*vds) < 10.0 )
        return 1 ;
    }
  }

  *vds=10000.0;
  return 1 ;
}

char mcc_mcc_vds_list( chain_list *headtrs,
                       float vs,
                       float ids, 
                       float *vds 
                     )
{
  mcc_trans_mcc *trs ;
  float          i ;
  float          rs, rd ;
  char           r ;
  char           onemoretime ;
  double         vsr, vdr, vdrmin, vdrmax ;
  double         imax, imin ;
  double         a ;
  double         abs_ids ;
  double         lim_ids ;
  double         abs_imax ;
  float          vd ;
  float          vdse ;
  char           computed ;
  static int     debug=0 ;
  int            iter ;
  fn_sum_info    info ;

  debug++ ;
  computed = 0 ;
  
  if( !headtrs->NEXT ) {
    trs = (mcc_trans_mcc*)headtrs->DATA ;
    if( mcc_trans_is_reverse( trs, ids ) == 0 ) {
      r = mcc_mcc_vds( trs, vs, ids, vds );
      if( !r ) 
        return 0;
      computed = 1 ;
    }
  }
  
  if( computed == 0 ) {
    /* approximation tant que mcc_mcc_ids ne prend pas en compte rs/rd */
    mcc_mcc_vds_list_compute_raccess( headtrs, &rs, &rd );
    vsr = vs + rs * ids ;
    
    /* calcule une estimation de l'encadrement du vds recherché */

    r = mcc_mcc_vds_list_estim_vds( headtrs, vs, ids, &vdse );
    if( !r ) {
      vdse = *vds ;
      return 0 ;
    }

    if( vdse == 0.0 ) {
      return 1 ;
    }
   
    /*
    for( chain = headtrs ; chain ; chain = chain->NEXT ) {
      trs = (mcc_trans_mcc*)chain->DATA ;
      if( chain == headtrs || trs->VDD < vdd )
        vdd = trs->VDD ;
    }
    */
  
    if( fabs( vdse ) > 1.2 * fabs(*vds) )
      vdse = 1.2 * *vds ;

    if( vdse == 0.0 ) {
      return 1 ;
    }

    vdrmax = vsr + vdse ;

    /* détermine le imax correspondant à cette tension, et on corrige si la valeur
       imax est inférieur à ids */
    imin   = FLT_MAX ;
    vdrmin = FLT_MAX ;
   
    iter = 100 ; 
    abs_ids = fabs(ids) ;
    lim_ids = 1.05 * abs_ids ;

    do {
      r = mcc_mcc_vds_list_compute_sum_current( headtrs, vsr, vdrmax, &i );
      if( !r )
        return 0 ;
      imax = i ;
      
      abs_imax = fabs(imax) ;

      if( abs_imax < lim_ids ) {

        if( abs_imax < abs_ids ) {
          imin = imax ;
          vdrmin = vdrmax ;
        }
        vdrmax = vdrmax + ( vdrmax - vsr ) ;
        onemoretime = 1 ;
      }
      else {
        onemoretime = 0 ;
      }

      iter-- ;
    }
    while( onemoretime && iter );
  
    if( !iter ) {
      *vds = 1.0e10 ;
      return 1 ;
    }

    /* borne inférieur */
    if( vdrmin == FLT_MAX ) {
      vdrmin = vsr ;
      imin   = 0.0 ;
    }

    /* résolution par dichotomie */

    iter = 100 ;

    if( vdrmin > vdrmax ) {
      a = vdrmin ;
      vdrmin = vdrmax ;
      vdrmax = a ;
    
      a = imin ;
      imin = imax ;
      imax = a ;
    }
    info.HEADTRS = headtrs ;
    info.VSR     = vsr ;
    info.VDRMIN  = vdrmin ;
    info.VDRMAX  = vdrmax ;
    info.IMIN    = imin ;
    info.IMAX    = imax ;
    info.IDS     = ids ;
    
    r= mbk_dichotomie( (int(*)(void*, double, double*))fn_sum_current,
                       NULL,
                       &info,
                       &vdrmin,
                       &vdrmax,
                       MBK_DICHO_NR,
                       &iter,
                       (vdrmax-vdrmin)*1e-3,
                       fabs(ids*1e-3),
                       &vdr 
                     );
   
    switch( r ) {
    case MBK_DICHO_OK :
      vd = vdr + rd * ids ;
      *vds = vd - vs ;
      break ;
    case MBK_DICHO_ERRFN :
    case MBK_DICHO_ERR :
      avt_log( LOGMCC, 2, "mcc_mcc_vds_list() : all transistors are blocked\n");
      return 0 ;
      break ;
    case MBK_DICHO_MAXIT :
      avt_log( LOGMCC, 2, "mcc_mcc_vds_list() : maximum iteration reached\n");
      *vds = 1.0e10 ;
      break ;
    }
    
  }
 
  return 1 ;
}

int fn_sum_current( fn_sum_info *info, double vdr, double *i )
{
  int    r=1 ;
  float  di ;

  if( vdr == info->VDRMIN )
    *i = info->IMIN ;
  else {
    if( vdr == info->VDRMAX )
      *i = info->IMAX ;
    else {
      r = mcc_mcc_vds_list_compute_sum_current( info->HEADTRS, 
                                                info->VSR, 
                                                vdr, 
                                                &di 
                                              );
      *i = di ;
    }
  }

  if( r )
    *i = *i - info->IDS ;
 
  return r ;
}

void plot_current_list( chain_list *headtrs,
                        float vs,
                        float vds,
                        char  *name 
                      )
{
  float step ;
  float avds ;
  FILE *ptf ;
  float vd ;
  char r ;
  float i;

  ptf = mbkfopen( name, NULL, "w" );
  if( !ptf) {
    printf( "can't open file\n" ) ;
    return ;
  }

  vd = ((int)(vs*1000.0))/1000.0 ;
  if( vds > 0.0 )
    step = 0.001 ;
  else
    step = -0.001 ;

  avds = fabs( vds );
  for( ; fabs(vd-vs) <= avds ; vd = vd+step ) {
    r = mcc_mcc_vds_list_compute_sum_current( headtrs, vs, vd, &i );
    if( r ) 
      fprintf( ptf, "%g %g\n", vd, i );
    else
      fprintf( ptf, "%g %g\n", vd, 0.0 );
  }

  fclose( ptf );
}
