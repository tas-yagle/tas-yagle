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

#define STM_UNDEF ((float)1.0e20)
//#define STM_SOLVE_I_TRACE
#define STM_CALC_WITH_VBS

/******************************************************************************\
Global definition
\******************************************************************************/

stm_solver_parameter stm_solver_standard={

  /* DEFAULT_TIME_STEP      */      10.0 ,       //  10 FS
  /* MIN_TIME_STEP          */       1.0 ,       //   1 FS
  /* MAX_TIME_STEP          */     100.0 ,       // 100 Fs
  /* MAX_DELTA_CURRENT      */       0.000100 ,  //  100uA
  /* MIN_DELTA_CURRENT      */       0.000001 ,  //    1uA
  /* MAX_DELTA_VOLTAGE      */       0.001000 ,  // 1000uV
  /* MIN_DELTA_VOLTAGE      */       0.000010 ,  //   10uV
  /* MAX_ITER               */  100000 ,
  /* VARIABLE_STEP          */       1 ,
  /* PLOTFILE               */    NULL ,
  /* STAT_ITER              */       0 ,
  /* STAT_CHANGE_STEP       */       0 ,
  /* STAT_MIN_TIME_STEP     */       0.0 ,
  /* STAT_MAX_TIME_STEP     */       0.0 ,
  /* STAT_MIN_DELTA_VOLTAGE */       0.0 ,
  /* STAT_MAX_DELTA_VOLTAGE */       0.0
};

stm_solver_i_param stm_solver_i_standard={
  /* I_EPSILON              */       1.0e-7 ,    // 0,1uA
  /* V_EPSILON              */       1.0e-5 ,    // 10uV
  /* MAXITER                */      30 ,
  /* DICHOTOMIE             */       5 ,
  /* MINDELTAVOLTAGE        */       1e-4,       // 1mV
  /* MINCURRENT             */       1e-25        // 0.1uA  
};

/******************************************************************************\
stm_solver_calc_vds()
Calcule le vds d'un transistor.
*vds contient initialement la polarisation maximum de la branche
\******************************************************************************/
char stm_solver_calc_vds( stm_solver_maillon *trans, 
                          float vs,
                          float ids, 
                          float *vds 
                        )
{
  char r;
  #ifndef STM_CALC_WITH_VBS
  vbs=0.0;
  #endif
 
  if( ( ids > 0.0 && ( *vds <= 0.0 ) ) || 
      ( ids < 0.0 && ( *vds >= 0.0 ) )    ) {
    *vds = 10000;
    r = 1;
  }
  else {
    r = trans->FN_VDS( trans->MODEL_VDS, 
                       vs,
                       ids,
                       vds
                     );
  }

  return r;
}

/******************************************************************************\
stm_solver_calc_ids()
Calcule le ids d'un transistor.
\******************************************************************************/
char stm_solver_calc_ids( stm_solver_maillon *trans, 
                          float vs, 
                          float vd,
                          float *ids
                        )
{
  char r;
  
  r = trans->FN_IDS( trans->MODEL_IDS, 
                     vs,
                     vd,
                     ids
                   );
  if( r ) {
    if( !finite(*ids) )
      r=0;
  }

  return r ;
}

/******************************************************************************\
stm_solver_estim_imax()
Calcule une estimation du courant dans une branche lorsque celle ci est 
soumise à une tension vmax. Thèse Amjad, pIV/3 (4-2).
\******************************************************************************/
char stm_solver_estim_imax( stm_solver_maillon_list *branch, 
                            float vmax,
                            float *imax
                           )
{
  float                    un_sur_imax ;
  stm_solver_maillon_list *scan;
  stm_solver_maillon      *maillon;
  float                    ids;
  float                    vbase;

  un_sur_imax = 0.0;
  
  for( scan = branch ; scan->NEXT ; scan = scan->NEXT );
  vbase = scan->MAILLON->VS;
  
  for( scan = branch ; scan ; scan = scan->NEXT ) {
    maillon = scan->MAILLON; 
    if( !stm_solver_calc_ids( maillon, 
                              vbase, 
                              vmax, 
                              &ids 
                            )
      )
      return 0;
    un_sur_imax = un_sur_imax + 1.0 / ids ;
  }

  *imax = 1.0 / un_sur_imax ;

  return 1;
}

/******************************************************************************\
stm_solver_i()
Détermine le courant circulant dans une branche.
vout est la tension en sortie de la branche.
Toutes les tensions doivent être données par rapport à vss.
Le courant est un flottant relatif, correspondant à la convention générateur
pour la charge de la porte. i>0, transition montante, i<0, transition 
descendante.
\******************************************************************************/
char stm_solver_i( stm_solver_maillon_list *head, 
                   float  vout,
                   float *ibr
                 )
{
  static int               fordebug=0;
  stm_solver_maillon_list *scan;
  stm_solver_maillon_list *base;
  stm_solver_maillon      *last;
  stm_solver_maillon      *maillon;
  float                    vds;
  int                      iter=0;
  float                    imax;
  float                    imin;
  float                    vdmax;
  float                    vdmin;
  float                    i;
  float                    iestim;
  float                    vbase;
  char                     bloque;
  stm_solver_i_param      *param;
  float                    vbr;
  float                    bkimax;
  char                     ret;
  
  fordebug++;

  param = & stm_solver_i_standard;

  #ifdef STM_SOLVE_I_TRACE
  printf( "solver vout=%g\n", vout );
  #endif
  
  // Récupère la fin de la chaine.
  for( base = head ; base->NEXT ; base = base->NEXT );

  // Polarisation de la branche
  vbase = base->MAILLON->VS ;

  // Différence de potentiel absolue dans la branche
  vbr = fabs( vout - vbase );

  if( vbr < param->MINDELTAVOLTAGE ) {
    *ibr = 0.0;
    return 1;
  }

  // Récupère le courant estimé dans la branche.
  if( !stm_solver_estim_imax( head, vout, &iestim ) )
    return 0;

  i = iestim ;

  // Teste si tous les transistors sont passant
  if( fabs(i) < param->MINCURRENT ) {
    *ibr=0.0;
    return 1;
  }

  param->I_EPSILON = i/1000.0 ;
  param->V_EPSILON = vbr/1000.0 ;

  vdmin = vbase ;
  imin  = 0.0 ;
  

  vdmax = STM_UNDEF ;
  imax  = STM_UNDEF ;
  
  bkimax = STM_UNDEF ;

  do 
  {
    // Propage les tensions
    last              = NULL;
    bloque            = 0;

    #ifdef STM_SOLVE_I_TRACE
    printf( "%3d i=%g ", iter, i );
    #endif
    
    for( scan = base ; scan ; scan = scan->PREV ) {
    
      maillon      = scan->MAILLON ;
      if( last )
        maillon->VS = last->VD;
      
      /* tension vds maximum */
      vds = vout - maillon->VS ;
      if( !stm_solver_calc_vds( maillon, maillon->VS, i, &vds ) ) return 0;

      #ifdef STM_SOLVE_I_TRACE
      printf( "(vs=%g -> vds=%g) ", maillon->VS, vds );
      #endif

      if( fabs(vds) > 10.0*vbr ) {
        // Il y a un transistor qui n'est pas passant...
        bloque=1;
        break;
      }
      
      maillon->VD  = maillon->VS + vds;

      maillon->IDS = i;
      last         = maillon;
    }

    // Vérification de la solution

    if( bloque == 1 ) {
      // Convergence sur i
      if( imax != STM_UNDEF ) {
        #ifdef STM_SOLVE_I_TRACE
        printf( "\n" );
        #endif
        fprintf( stderr, "stm_solve_i() fatal error\n" );
        return 0;
      }
      bkimax = i;
      i = (i+imin)/2.0;
      #ifdef STM_SOLVE_I_TRACE
      printf( "bk " );
      #endif
    }
    else {
      #ifdef STM_SOLVE_I_TRACE
      printf( "vd=%g ", maillon->VD );
      #endif
      // Convergence sur v
      if( i > 0.0 ) {
        if( maillon->VD > vout ) {
          vdmax = maillon->VD ;
          imax = i ;
        }
        else {
          vdmin = maillon->VD ;
          imin = i;
        }
      }
      else {
        if( maillon->VD < vout ) {
          vdmax = maillon->VD ;
          imax = i ;
        }
        else {
          vdmin = maillon->VD ;
          imin = i;
        }
      }

      if( vdmax != STM_UNDEF ) {
        if( iter < param->DICHOTOMIE )
          i = imin + (imax-imin)*(vout-vdmin)/(vdmax-vdmin);
        else
          i = (imax+imin)/2.0;
      }
      else {
        if( bkimax != STM_UNDEF )
          i = (bkimax+imin)/2.0;
        else {
            /*
            Ici, on a qu'une borne minimum connue.
            Théoriquement, on trouve directement i par extrapolation. Mais
            il y a des cas où la convergence est extrèmement lente : on converge
            vers la solution sans jamais la dépasser, on a donc jamais de borne 
            MAX. Pour accélérer cette converge en obtenant une solution MAX, on
            ajoute au i extrapolé 20% du courant estimé initialement. */
            i = i * (vout-vbase)/(maillon->VD-vbase) + 0.2 * iestim ;
        }
      }
    }

    #ifdef STM_SOLVE_I_TRACE
    printf( "min=%g imin=%g max=%g imax=%g\n", vdmin, imin, vdmax, imax );
    #endif

    iter++;
  }
  while( (  bloque==1 ||
            (  bloque==0                                      &&
               fabs( imax-imin ) > param->I_EPSILON           &&
               fabs( maillon->VD - vout ) > param->V_EPSILON     )
         ) && iter < param->MAXITER 
       );

  if( iter >= param->MAXITER ) {
    /* grosse bidouille : Test si on a raté la convergence sur imax. Ca arrive
       lorsqu'on a des très faibles courants en bsim3 car on calcule vds avec
       une précision de 10-3 */
    if( imax == STM_UNDEF && fabs( imin-i ) < param->I_EPSILON )
      ret = 1;
    else
      ret = 0;
  }
  else 
    ret = 1;

  *ibr = -base->MAILLON->IDS;
  return ret;
}

/******************************************************************************\Fonctions de manipulation des données
\******************************************************************************/

stm_solver_maillon* stm_solver_new_maillon( void )
{
  stm_solver_maillon *m;

  m = (stm_solver_maillon*)mbkalloc( sizeof( stm_solver_maillon ) );
  m->FN_VDS = NULL ;
  m->FN_IDS = NULL ;
  m->MODEL_VDS = NULL ;
  m->MODEL_IDS = NULL ;
  m->VD  = 0.0;
  m->VS  = 0.0;
  m->IDS = 0.0;

  return m;
}

void stm_solver_free_maillon( stm_solver_maillon *maillon )
{
  freechain( maillon->MODEL_VDS );
  freechain( maillon->MODEL_IDS );
  mbkfree( maillon );
}

/******************************************************************************/

void stm_solver_add_model( stm_solver_maillon *maillon,
                           char (*fids)( void*, float, float, float*),
                           char (*fvds)( void*, float, float, float*),
                           chain_list *datids,
                           chain_list *datvds
                         )
{
  maillon->FN_VDS = fvds ;
  maillon->FN_IDS = fids ;
  maillon->MODEL_VDS = dupchainlst( datvds );
  maillon->MODEL_IDS = dupchainlst( datids );
}

/******************************************************************************/

stm_solver_maillon_list * stm_solver_maillon_addchain( 
                                        stm_solver_maillon_list *head,
                                        stm_solver_maillon      *elem
                                                     )
{
  stm_solver_maillon_list *m;
  
  m = (stm_solver_maillon_list*)mbkalloc( sizeof( stm_solver_maillon_list ) );
  
  m->NEXT               = head;
  m->PREV               = NULL;
  if( head ) head->PREV = m;
  m->MAILLON            = elem;
  return m;
}

void stm_solver_maillon_freechain( stm_solver_maillon_list *head )
{
  stm_solver_maillon_list *next;

  while( head ) {
    stm_solver_free_maillon( head->MAILLON );
    next = head->NEXT;
    mbkfree( head );
    head = next;
  }
}

stm_solver_maillon_list* stm_solver_maillon_reverse( 
                                                   stm_solver_maillon_list *head
                                                   )
{
  stm_solver_maillon_list *scan;
  stm_solver_maillon_list *last;
  stm_solver_maillon_list *next;
  stm_solver_maillon_list *prev;


  for( scan = head ; scan ; scan = next ) {
    prev = scan->PREV;
    next = scan->NEXT;
    scan->NEXT = prev;
    scan->PREV = next;

    last = scan;
  }

  return last;
}

/******************************************************************************\
stm_drive_ids()
Sort dans un fichier la caracteristique IDS d'un maillon
\******************************************************************************/
void stm_drive_ids( stm_solver_maillon *maillon,
                    float               vgs,
                    float               vdsmax,
                    float               vdsstep,
                    char               *filename
                  )
{
  FILE *file;
  float v;
  float i;
  
  file = mbkfopen( filename, "plot", "w" );
  for( v=0.0; v <= vdsmax ; v=v+vdsstep ) {
    if( stm_solver_calc_ids( maillon, vgs, v, &i ) )
      fprintf( file, "%g %g\n", v, i );
  }
  fclose( file );
}

void stm_drive_i_branch( stm_solver_maillon_list *head, 
                         float                    voutmax,
                         char                    *filename
                       )
{
  FILE *file ;
  float v ;
  float step ;
  float i ;
  int   r ;

  file = mbkfopen( filename, "plot", "w" );
  if( !file ) 
    return ;
    
  step = voutmax/1000.0 ;
  
  for( v=0.0 ; v<=voutmax ; v=v+step ) {
    r = stm_solver_i( head, v, &i );
    if( !r )
      i = 0.0 ;
    fprintf( file, "%g %g\n", v, i );
  }

  fclose( file );
}

