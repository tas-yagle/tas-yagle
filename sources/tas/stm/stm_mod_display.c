/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Fichier : stm_mod_display.c                                           */
/*                                                                          */
/*    © copyright 2003 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "stm.h"

/****************************************************************************/
/*{{{                    stm_displayTable()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_displayTable(char *str, timing_table *table)
{
  if (table)
  {
    if (table->SET2D)
    {
      int        i, j, k, nb = 8, x;

      for (k = 0; k <= (table->NX)/nb ; k ++)
      {
        x = k*nb + ((k < (table->NX)/nb) ? nb : (table->NX - k*nb)%nb);
        avt_log (LOGSTM, 1, "/¯¯¯¯¯¯¯¯");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "|¯¯¯¯¯¯¯¯¯");
        avt_log (LOGSTM, 1, "\\\n");

        avt_log (LOGSTM, 1, "|%8s|",str);
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "%+1.2e|",table->XRANGE[i]);
        avt_log (LOGSTM, 1, "\n");

        avt_log (LOGSTM, 1, ">~~~~~~~~");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "+~~~~~~~~~");
        avt_log (LOGSTM, 1, "<\n");

        for (j = 0; j < table->NY; j ++)
        {
          avt_log (LOGSTM, 1, "|%1.2e|",table->YRANGE[j]);
          for (i = k*nb; i < x; i ++)
          {
            if (table->SET2D[i][j] == STM_NOVALUE)
              avt_log (LOGSTM, 1, "         |");
            else
              avt_log (LOGSTM, 1, "%+1.2e|",table->SET2D[i][j]);
          }
          avt_log (LOGSTM, 1, "\n");
        }

        avt_log (LOGSTM, 1, "\\________");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "|_________");
        avt_log (LOGSTM, 1, "/\n");
      }
    }
    else if (table->SET1D)
    {
      int        i, k, nb = 8, x;

      for (k = 0; k <= (table->NX)/nb ; k ++)
      {
        x = k*nb + ((k < (table->NX)/nb) ? nb : (table->NX - k*nb)%nb);
        
        avt_log (LOGSTM, 1, "/¯¯¯¯¯");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "|¯¯¯¯¯¯¯¯¯");
        avt_log (LOGSTM, 1, "\\\n");

        avt_log (LOGSTM, 1, "|  X  ");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "|%+1.2e",table->XRANGE[i]);
        avt_log (LOGSTM, 1, "|\n");

        avt_log (LOGSTM, 1, "+~~~~~");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "+~~~~~~~~~");
        avt_log (LOGSTM, 1, "<\n");

        avt_log (LOGSTM, 1, "| Val ");
        for (i = k*nb; i < x; i ++)
        {
          if (table->SET1D[i] == STM_NOVALUE)
            avt_log (LOGSTM, 1, "|         ");
          else
            avt_log (LOGSTM, 1, "|%+1.2e",table->SET1D[i]);
        }
        avt_log (LOGSTM, 1, "|\n");

        avt_log (LOGSTM, 1, "\\_____");
        for (i = k*nb; i < x; i ++)
          avt_log (LOGSTM, 1, "|_________");
        avt_log (LOGSTM, 1, "/\n");
      }
    }
  }
}

/*}}}************************************************************************/
/*{{{                    stm_mod_display()                                  */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void stm_mod_display(char *str,timing_model *stm)
{
  if (stm)
    switch (stm->UTYPE)
    {
      case STM_MOD_MODTBL :
           stm_displayTable(str,stm->UMODEL.TABLE);
           break;
      default :
           break;
    }
}

/*}}}************************************************************************/
