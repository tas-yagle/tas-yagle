/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_solver.c                                                */
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

#ifdef Solaris
#include <ieeefp.h>
#endif

/******************************************************************************\
Global definition
\******************************************************************************/


/******************************************************************************\
Insert a current in table
\******************************************************************************/

void stm_modiv_setis( timing_iv *iv, int ne, int ns, float is )
{
  iv->IS[ ne + ( ns * iv->NVE ) ] = is;
}

/******************************************************************************\
Get a current in table
\******************************************************************************/

float stm_modiv_getis( timing_iv *iv, int ne, int ns )
{
  return iv->IS[ ne + ( ns * iv->NVE ) ];
}

/******************************************************************************\
Create a new model
\******************************************************************************/

timing_iv* stm_modiv_create( int   nve, 
                             int   nvs, 
                             float vemax, 
                             float vsmax,
                         char (*fnct)( void *data, float ve, float vs, float* ),
                             void  *data
                           )
{
  timing_iv *iv;
  int        ne;
  int        ns;
  float      is;
  char       r;

  iv = stm_modiv_alloc( nve, nvs );

  for( ne=0 ; ne<nve ; ne++ )
    iv->VE[ne] = ((float)ne) * vemax / ((float)(nve-1));

  for( ns=0 ; ns<nvs ; ns++ )
    iv->VS[ns] = ((float)ns) * vsmax / ((float)(nvs-1));

  if( fnct ) {
    for( ne=0 ; ne<nve ; ne++ ) {
      for( ns=0 ; ns<nvs ; ns++ ) {
        r = fnct( data, iv->VE[ne], iv->VS[ns] , &is ) ;
        if( r ) {
          if( !finite( is ) )
            r = 0;
        }
        if( !r ) {
          stm_modiv_destroy( iv );
          return NULL;
        }
        stm_modiv_setis( iv, ne, ns, is );
      }
    }
  }

  return iv;
}

/******************************************************************************\
Duplicate a model
\******************************************************************************/

timing_iv* stm_modiv_duplicate( timing_iv *orig )
{
  timing_iv *iv;
  int        ne;
  int        ns;
  
  iv = stm_modiv_alloc( orig->NVE, orig->NVS );

  for( ne=0 ; ne<orig->NVE ; ne++ )
    iv->VE[ne] = orig->VE[ne];

  for( ns=0 ; ns<orig->NVS ; ns++ )
    iv->VS[ns] = orig->VS[ns];

  for( ne=0 ; ne<orig->NVE ; ne++ ) {
    for( ns=0 ; ns<orig->NVS ; ns++ ) {
      stm_modiv_setis( iv, ne, ns, stm_modiv_getis( orig, ne, ns ) );
    }
  }

  iv->IV_CONF.PCONF0    = orig->IV_CONF.PCONF0;
  iv->IV_CONF.PCONF1    = orig->IV_CONF.PCONF1;
  iv->IV_CONF.IRAP      = orig->IV_CONF.IRAP;
  iv->IV_CONF.CI        = orig->IV_CONF.CI;
  iv->IV_INPUT.VI       = orig->IV_INPUT.VI;
  iv->IV_INPUT.VF       = orig->IV_INPUT.VF;
  iv->IV_INPUT.VTH      = orig->IV_INPUT.VTH;
  iv->IV_INIT.VI        = orig->IV_INIT.VI;
  return iv;
}

/******************************************************************************\
Allocate a model
\******************************************************************************/

timing_iv* stm_modiv_alloc( int nve, int nvs )
{
  timing_iv *iv;
  int        ne;
  int        ns;

  iv = (timing_iv*)mbkalloc( sizeof( timing_iv ) );

  iv->NVE = nve;
  iv->NVS = nvs;
  
  iv->VE  = (float*)mbkalloc( sizeof( float ) * nve );
  iv->VS  = (float*)mbkalloc( sizeof( float ) * nvs );
  iv->IS  = (float*)mbkalloc( sizeof( float ) * nve * nve );

  iv->IV_CONF.PCONF0    = 0.0;
  iv->IV_CONF.PCONF1    = 0.0;
  iv->IV_CONF.IRAP      = 0.0;
  iv->IV_CONF.CI        = 0.0;
  iv->IV_INPUT.VI       = 0.0;
  iv->IV_INPUT.VF       = 0.0;
  iv->IV_INPUT.VTH      = 0.0;

  for( ne=0 ; ne<nve ; ne++ )
    iv->VE[ne] = 0.0;

  for( ns=0 ; ns<nvs ; ns++ )
    iv->VS[ns] = 0.0;
   
  for( ne=0 ; ne<nve ; ne++ ) {
    for( ns=0 ; ns<nvs ; ns++ ) {
      stm_modiv_setis( iv, ne, ns, 0.0 );
    }
  }

  return iv;
}

/******************************************************************************\
Allocate a model
\******************************************************************************/

void stm_modiv_destroy( timing_iv *iv )
{
  mbkfree( iv->VE );
  mbkfree( iv->VS );
  mbkfree( iv->IS );
  mbkfree( iv );
}

/******************************************************************************\
Parameter to evaluate conflict capacitance
\******************************************************************************/

void stm_modiv_set_cf( timing_iv *iv, 
                       float ci, 
                       float p0, 
                       float p1, 
                       float irap 
                     )
{
  iv->IV_CONF.CI     = ci;
  iv->IV_CONF.PCONF0 = p0;
  iv->IV_CONF.PCONF1 = p1;
  iv->IV_CONF.IRAP   = irap;
}

/******************************************************************************\
Parameter to evaluate input slope
\******************************************************************************/

void stm_modiv_set_in( timing_iv *iv,
                       float      vt,
                       float      vi,
                       float      vf,
                       float      vth
                     )
{
  iv->IV_INPUT.VI  = vi ;
  iv->IV_INPUT.VF  = vf ;
  iv->IV_INPUT.VTH = vth ;
  iv->IV_INPUT.VT  = vt ;
}

float stm_modiv_in_vi( timing_iv *iv )
{
  return iv->IV_INPUT.VI;
}

float stm_modiv_in_vf( timing_iv *iv )
{
  return iv->IV_INPUT.VF;
}

float stm_modiv_in_vth( timing_iv *iv )
{
  return iv->IV_INPUT.VTH;
}

float stm_modiv_in_vt( timing_iv *iv )
{
  return iv->IV_INPUT.VT;
}

/******************************************************************************\
Parameter to evaluate initial state
\******************************************************************************/

void stm_modiv_set_ti( timing_iv *iv, float vi )
{
  iv->IV_INIT.VI = vi;
}

float stm_modiv_ti_vi( timing_iv *iv )
{
  return iv->IV_INIT.VI ;
}
