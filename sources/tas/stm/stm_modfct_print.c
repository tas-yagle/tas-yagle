/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STM Version 1.00                                            */
/*    Fichier : stm_modfct_print.c                                          */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Gilles Augustins                                          */
/*                                                                          */
/****************************************************************************/

/****************************************************************************/
/*     includes                                                             */
/****************************************************************************/

#include "stm.h" 

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

/****************************************************************************/
/* function stm_modfct_print()                                              */
/****************************************************************************/

void stm_modfct_print (FILE *f, timing_function *fct)
{
  t_arg *ta;
  chain_list *cl;
  fprintf (f, "\t\tfct (\n");
//    if (fct->FIL) fprintf (f, "\t\t\tfile (%s)\n", fct->FIL);
    if (fct->FCT)
      {
        fprintf (f, "\t\t\tfunction (%s", fct->FCT->NAME);
        for (cl=fct->FCT->ARGS; cl!=NULL; cl=cl->NEXT)
          {
            ta=(t_arg *)cl->DATA;
            if (ta->POINTER==0)
              {
                if (strcmp(ta->TYPE, "char")==0)
                  fprintf(f," '%c'",*(char *)ta->VALUE);
                else if (strcmp(ta->TYPE, "int")==0)
                  fprintf(f," %d",*(int *)ta->VALUE);
                else if (strcmp(ta->TYPE, "double")==0)
                  fprintf(f," %e",*(double *)ta->VALUE);
                else
                  {
                    fprintf(stderr,"can't handle type '%s' in stm functions\n",ta->TYPE);
                    fprintf(f," ?");
                  }
              }
            else
              {
                if (strcmp(ta->TYPE, "char")==0)
                  {
                    fprintf(f," \"%s\"",*(char **)ta->VALUE);
                  }
                else
                  {
                    fprintf(stderr,"can't handle pointer type '%s' in stm functions\n",ta->TYPE);
                    fprintf(f," ?");
                  }
              }
          }
        fprintf (f, ")\n");
      }
    

    if (fct->INS) fprintf (f, "\t\t\tinstance (%s)\n", fct->INS);
    if (fct->LOCALNAME) fprintf (f, "\t\t\tlocalname (%s)\n", fct->LOCALNAME);

    fprintf (f, "\t\t)\n");

}


