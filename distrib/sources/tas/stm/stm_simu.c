/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_noise.c                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Grégoire Avot                                             */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

stm_simu_pi_load_parameter stm_simu_param_standard = {
  /* DEFAULT_TIME_STEP      */      10.0 ,       //  10 FS
  /* MIN_TIME_STEP          */       1.0 ,       //   1 FS
  /* MAX_TIME_STEP          */     100.0 ,       // 100 Fs
  /* MAX_DELTA_CURRENT      */       0.000100 ,  //  100uA
  /* MIN_DELTA_CURRENT      */       0.000001 ,  //    1uA
  /* MAX_DELTA_VOLTAGE      */       0.001000 ,  // 1000uV
  /* MIN_DELTA_VOLTAGE      */       0.000010 ,  //   10uV
  /* MAX_ITER               */  100000 ,
  /* VARIABLE_STEP          */       1 ,
  /* STAT_ITER              */       0 ,
  /* STAT_CHANGE_STEP       */       0 ,
  /* STAT_MIN_TIME_STEP     */     0.0 ,
  /* STAT_MAX_TIME_STEP     */     0.0 ,
  /* STAT_MIN_DELTA_VOLTAGE */     0.0 ,
  /* STAT_MAX_DELTA_VOLTAGE */     0.0 ,
  /* STAT_MIN_DELTA_CURRENT */     0.0 ,
  /* STAT_MAX_DELTA_CURRENT */     0.0
};

/******************************************************************************\

\******************************************************************************/

/******************************************************************************\

   Convention générateur : 
   transition UD : le courant décharge la sortie, il est négatif.
   transition DU : le courant charge la sortie, il est positif

\******************************************************************************/

char stm_simu_mcc_ids( param_mcc_ids *param, float vgs, float vds, float *ids )
{
  float a    = param->A;
  float b    = param->B;
  float vdd  = param->VDD;
  float rnt  = param->RNT;
  float rns  = param->RNS;
  float vt   = param->VT;
  char  neg  = param->NEG;
  float imax = param->IMAX;
  float kres;
  float k;
  float x;

  float isat ;
  float usat ;
  float rlin ;
  float ures ;
  float ires ;
  float rsat ;

  /* Initialisation */
  if( param->k == 0.0 ) {
  
    k    = imax * rnt / (vdd-vt );
    
    x    = ( imax * rnt - vdd*rnt/rns )/( 1.0 - rnt/rns );
    kres = x/(vdd-vt);
    
    param->k    = k;
    param->kres = kres;
  }
  else {
    k    = param->k;
    kres = param->kres;
  }

  /* Calcul */
  usat = k*(vgs-vt);
  isat = a*(vgs-vt)*(vgs-vt) / ( 1.0 + b*(vgs-vt) );
  rlin = usat/isat;

  ures = kres*( vgs-vt );
  ires = ures/rlin;
  rsat = (vdd-ures) / (isat-ires);

  if( vds > ures ) 
    *ids = isat - (vdd-vds)/rsat;
  else
    *ids = vds/rlin;

  if( neg )
    *ids = -(*ids);

  return 1;
}

/******************************************************************************\

\******************************************************************************/

char stm_simu_tanh( stm_simu_tanh_param *param, float t, float *v )
{
  *v = stm_get_v( t, param->VT, param->VI, param->VF, param->F );
  return 1;
}

/******************************************************************************\

Unités :
--------
   Courant      Ampère
   Tension      Volt
   Capacité     Femto-seconde
   Temps        Femto-seconde

Paramètres :
------------
   fn_is        Pointeur sur fonction de calcul du courant dans la branche.
   fn_ve        Pointeur sur fonction de calcul de la tension d'entrée.
   dat_is       Données utilisées par fn_is.
   dat_ve       Données utilisées par fn_ve.
   r, c1, c2    Charge de la branche.
   vi           Tension initiale de la sortie.
   vth          Tension de sortie à laquelle la simulation s'arrète.
   ts, fs       Pointeurs vers le temps et le front simulé lorsque la sortie
                vaut vth. NULL autorisé.
   param        Configuration du simulateur. NULL autorisé.

Notes :
-------

   Le front calculé correspond à la pente du signal, en V/fs. Il ne correspond
   PAS au front défini par Amjad. Le passage de l'un à l'autre se fait avec la
   relation 3-12-b page III/15 de la thèse.

\******************************************************************************/

char stm_simu_pi_load_ts( char (*fn_is)( void *dat_is, 
                                         float ve, 
                                         float vs, 
                                         float *is 
                                       ),
                          char (*fn_ve)( void *dat_ve, float t, float *v ),
                          void  *dat_is,
                          void  *dat_ve,
                          float r,
                          float c1,
                          float c2,
                          float vi,
                          float vth,
                          float *ts,
                          float *fs,
                          stm_simu_pi_load_parameter *param,
                          char *plotcurvename
                        )
{
  double  vx1, vx2, vx3, vx4, vx5;
  double  a, b;
  double  ti, tj;
  double  dt;
  double  v1i, v1j ;
  double  v2i, v2j ;
  float   ii, ij ;
  double  di;
  double  dv;
  int     needmodifyconst;
  int     i;
  char    ret=1;
  float   ve ;
  char    encore;
  double  t0;
  FILE    *ptf=NULL ;

  /* Various initialisation */

  if( plotcurvename ) {
    ptf = mbkfopen( plotcurvename, "dat", WRITE_TEXT );
    fprintf( ptf, "#time vin vout igate\n" );
  }

  if( !param ) param = &stm_simu_param_standard ;

  param->STAT_ITER = 0.0 ;
  param->STAT_CHANGE_STEP = 0.0;
  param->STAT_MIN_TIME_STEP     =  1.0e9;
  param->STAT_MAX_TIME_STEP     = -1.0e9;
  param->STAT_MIN_DELTA_VOLTAGE =  1.0e9;
  param->STAT_MAX_DELTA_VOLTAGE = -1.0e9;
  param->STAT_MIN_DELTA_CURRENT =  1.0e9;
  param->STAT_MAX_DELTA_CURRENT = -1.0e9;

  ti   = 0.0 ;
  ii   = 0.0 ;
  dt   = param->DEFAULT_TIME_STEP ;
  b    = r * c2;
  v1i  = vi ;
  v2i  = vi ;
  needmodifyconst = 1;
  t0   = 0.0 ;

  do
  {
    tj = ti ;
    ij = ii ;
    v1j = v1i ;
    v2j = v2i ;

    /* Variable step */
    do
    {
      
      if( needmodifyconst )
      {
        param->STAT_CHANGE_STEP++;
        a = b + dt;
        vx1 = 1.0/( c1 / dt + c2 / a );
        vx2 = c1 / dt;
        vx3 = ( c2 - b * c2 / a ) / dt;
        vx4 = dt / a ;
        vx5 = b / a ;
        needmodifyconst = 0;
      }

      /* Le corp du simulateur */
      ti  = tj + dt ;
      if( !fn_ve( dat_ve, ti-t0, &ve ) ) {
        ret=0;
        break;
      }
      
      for( i=3;i>0;i-- ) {
        if( !fn_is( dat_is, ve, v1i, &ii ) ) {
          ret=0;
          break;
        }
        v1i = vx1 * ( ii + vx2 * v1j + vx3 * v2j ) ;
      }
      if( !ret ) 
        break;

      v2i = vx4 * v1i + vx5 * v2j ;
      /* Fin du corp */
    
      if( param->VARIABLE_STEP ) {
        /* Vérification du step */
        di = ii - ij ;
        if( di < 0.0 )
          di = -di ;
       
        dv = v1i - v1j ;
        if( dv < 0 )
          dv = -dv ;

        if( ( dv > param->MAX_DELTA_VOLTAGE ||
              di > param->MAX_DELTA_CURRENT    ) && dt > param->MIN_TIME_STEP )
        {
          dt = dt / 2.0 ;
          needmodifyconst=1;
        }
      }
    }
    while( needmodifyconst );

    if( !ret )
      break;

    if( param->VARIABLE_STEP ) {
      /* Vérification du step */
      if( ( di < param->MIN_DELTA_CURRENT && 
            dv < param->MIN_DELTA_VOLTAGE    ) && dt < param->MAX_TIME_STEP )
      {
        dt = dt * 1.2 ;
        needmodifyconst=1;
      }
    }

    di = ii - ij;
    if( dv < param->STAT_MIN_DELTA_VOLTAGE ) param->STAT_MIN_DELTA_VOLTAGE = dv;
    if( dv > param->STAT_MAX_DELTA_VOLTAGE ) param->STAT_MAX_DELTA_VOLTAGE = dv;
    if( dt < param->STAT_MIN_TIME_STEP ) param->STAT_MIN_TIME_STEP = dt;
    if( dt > param->STAT_MAX_TIME_STEP ) param->STAT_MAX_TIME_STEP = dt;
    if( di < param->STAT_MIN_DELTA_CURRENT ) param->STAT_MIN_DELTA_CURRENT = di;
    if( di > param->STAT_MAX_DELTA_CURRENT ) param->STAT_MAX_DELTA_CURRENT = di;

    param->STAT_ITER++;

    // test de sortie

    if( ptf )
      fprintf( ptf, "%g %g %g %g\n", ti, ve, v1i, ii );

    encore = 1;
    if( param->STAT_ITER > param->MAX_ITER ) 
      encore = 0 ;
    if( ( v1i > vth && v1j < vth ) || ( v1i < vth && v1j > vth ) )
      encore = 0 ;
  }
  while( encore );
  
  if( !ret || param->STAT_ITER >= param->MAX_ITER ) {
    if(ts) *ts = 0.0;
    if(fs) *fs = 0.0;
    ret=0;
  }
  else {
    if(ts) *ts = tj+(vth-v1j)/(v1i-v1j)*(ti-tj);
    if(fs) *fs = fabs(v1i-v1j)/dt;
    ret=1;
  }

  if( ptf )
    fclose( ptf );
  return ret;
}
