/**************************/
/*     eqt_main.c         */
/**************************/

/**********************************************************************/
/*     includes                                                       */
/**********************************************************************/

#include EQT_H
#include "eqt_util.h"

/**********************************************************************/
/*     extern variable                                                */
/**********************************************************************/

/**********************************************************************/
/*    main                                                            */
/**********************************************************************/

/* bench for test                                                     */

static char *eqt_bench[] =
{
  "h+i*64+j*128+320-(h+i*64+j*128+64)"  ,"256"                  ,
  "f*1"                                 ,"f"                    ,
  "f*0"                                 ,"0"                    ,
  "h+i*64+j*128+64-0+4*k"               ,"h+i*64+j*128+k*4+64"  ,
  "(i-1)*4+j"                           ,"j+(i-1)*4"            ,
  "(-i+j*1024+5119)-(-i+j*1024+1023)"   ,"4096"                 ,
  "1588211555"                          ,"1588211555"           ,
  "(-i+j*1024+k*4096+l*524288+1.0496e+06)-(-i+j*1024+k*4096+l*524288+1023)", "1048577",
  "(i+j+384)-(i+j+640)"                 ,"-256"                 ,
  "(i+j+512)-(i+j+256)"                 ,"256"                  ,
  "i"                                   ,"i"                    ,
  0
};

/***** New function from USER  ****************************************/

double trunc (double p) 
{
  return (int)(p) ;
}

double sum (double p, double q) 
{
  return p + q ;
}


double max (double p, double q) 
{
  double r ;

  if ( p >= q )
    r = p ;
  else 
    r = q ;
  return r ;
}

double ln (double p)
{
  return log (p);
}

double mycos (double p)
{
  return cos (p);
}

/********  Main function ***********************************/
int main (int argc, char **argv)
{
  char          *str, *str2;
  //    char *string ;
  //    chain_list *abl ;
  //    chain_list *expr1;
  //    chain_list *expr2;
  eqt_node      *node = NULL ;
  int            doTree, i;
  eqt_ctx *ecx;

  mbkenv () ;
  
  ecx=eqt_init (EQT_NB_VARS);

  eqt_addfunction2(ecx, "max", &max) ;
  eqt_addfunction (ecx, "ln", &ln) ;
  eqt_addfunction (ecx, "cos", &mycos) ;

  /******* Initialisation des parametres generaux  *******/


  if (argc > 1)
  {
    str       = argv[1];
    if (argc > 2)
    {
      str2    = argv[2];
      doTree  = !strcmp(str2,"tree");
    }
    else
      doTree  = 0;

    /*- Test arithmetique -*/
    node      = eqt_create(ecx, str) ;
    if (!node)
      printf("error empty node!!!\n") ;
    else
    {
      printf("\noriginal\n");
      eqt_drive(ecx,node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      printf("\ndelmindiv\n");
      node = eqt_DelMinDiv(ecx,node);
      eqt_drive(ecx, node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      printf("\nlinearisation\n");
      eqt_linearise(ecx,node);
      eqt_drive(ecx,node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      printf("\nadd neutral\n");
      eqt_addNeutral(ecx,node);
      eqt_drive(ecx,node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      printf("\nsorting\n");
      eqt_sortBranch(ecx,node);
      eqt_drive(ecx,node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      printf("\nassociation\n");
      node = eqt_associate(ecx,node);
      eqt_drive(ecx,node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      eqt_assocVar(ecx,node);
      printf("\nresult\n");
      eqt_reduce(ecx,node);
      node = eqt_DelNeutral(ecx,node);
      if (doTree)
        eqt_printTree(ecx,node,0);
      printf("\ndrived\n");
      eqt_drive(ecx,node);
      //eqt_print(node) ;
      eqt_eval (ecx, str,EQTFAST);
    }
    //    fprintf (stdout, "\n*** Normal = %g ***\n", eqt_eval (str, EQTNORMAL)) ;
    //    fprintf (stdout, "\n*** Fast = %g ***\n", eqt_eval (str, EQTFAST)) ;
    eqt_freenode(node);
    eqt_term (ecx);

    str       = eqt_getSimpleEquation(str);
    printf("eqt_getSimpleEquation :\n%s\n",str);
    mbkfree(str);

    EXIT(0);
    return 1 ;
  }
  else
    for (i = 0; eqt_bench[i]; i += 2)
    {
      fprintf(stdout,"Testing     : %s\n",eqt_bench[i]);
      fprintf(stdout,"-- Expected : %s\n",eqt_bench[i+1]);
      str     = eqt_getSimpleEquation(eqt_bench[i]);
      fprintf(stdout,"-- Result   : %s\n",str);
      if (strcmp(str,eqt_bench[i+1]))
        eqt_error(NULL);
      else
        fprintf(stdout,"OK\n");
      mbkfree(str);
    }
      
  eqt_term (ecx);
  EXIT(0) ;
}
