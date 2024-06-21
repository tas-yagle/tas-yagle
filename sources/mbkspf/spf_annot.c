/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : SPF Version 1.00                                            */
/*    Fichier : spf_annot.c                                                 */
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

#include AVT_H
#include "spf_annot.h"
#include "spf_actions.h"
#include "spf_util.h"

/****************************************************************************/
/*     globals                                                              */
/****************************************************************************/

lofig_list *spf_Lofig ;
int         spf_Line ;
int         spf_Mode ;

/****************************************************************************/
/*     externs                                                              */
/****************************************************************************/

extern int   spfparse() ;
extern FILE *spfin ;
extern int spf_missing_signals;
int /*SPF_KEEPM, SPF_KEEPX,*/ SPF_PREVERSE_RC, SPF_CARDS;

/****************************************************************************/
/*     functions                                                            */
/****************************************************************************/

static void spf_checkenv()
{
  char *env;
  int SPF_KEEPM, SPF_KEEPX;
  mbk_options_pack_struct spf_device_opack[] = {
        {"Transistor", KEEP__TRANSISTOR},
        {"Resistance", KEEP__RESISTANCE},
        {"Instance", KEEP__INSTANCE},
        {"Capacitance", KEEP__CAPA},
        {"Diode", KEEP__DIODE},
        {"None", KEEP__NONE},
        {"All", KEEP__ALL}
  };

  SPF_KEEPM=1;
  SPF_KEEPX=1;

                 
  SPF_PREVERSE_RC=0;

      SPF_KEEPM=V_BOOL_TAB[__ANNOT_KEEP_M].VALUE;
      SPF_KEEPX=V_BOOL_TAB[__ANNOT_KEEP_X].VALUE;
      SPF_PREVERSE_RC=V_BOOL_TAB[__DSPF_PRESERVE_RC].VALUE;

  SPF_CARDS =
       mbk_ReadFlags (__ANNOT_KEEP_CARDS, spf_device_opack,
                      sizeof (spf_device_opack) / sizeof (*spf_device_opack), 1, 0);
   
  if (SPF_CARDS!=0)
  {
    if (SPF_CARDS & KEEP__NONE)
      SPF_CARDS = 0;

    if (SPF_CARDS & KEEP__TRANSISTOR) SPF_KEEPM=1; else SPF_KEEPM=0;
    if (SPF_CARDS & KEEP__INSTANCE) SPF_KEEPX=1; else SPF_KEEPX=0;
  }
  else
  {
     if (SPF_KEEPM) SPF_CARDS|=KEEP__TRANSISTOR;
     if (SPF_KEEPX) SPF_CARDS|=KEEP__INSTANCE;
  }
}


void spf_Annotate(lofig_list *ptlofig)
{
    char buf[1024];
    groundlosig = NULL;

    spf_checkenv();

    if ((spfin = mbkfopen(ptlofig->NAME, IN_PARASITICS, READ_TEXT))!=NULL)
      {
        spf_Lofig = ptlofig;
        spf_Line = 1;
        sprintf(buf,"%s.%s",ptlofig->NAME, IN_PARASITICS);
        spf_ParsedFile=buf;

//        spf_destroyexistingRC(spf_Lofig);
        spf_initBUSDelimiters("", ""); // no delimiter
        spf_setDelimiter('.');
        spf_setDivider('|');
        spf_missing_signals=0;

        spfparse();

        spf_destroy_lotrs_htable();
        spf_destroy_loins_htable();
        spf_destroy_losig_htable();
        spf_finish_pending_capacitances();

        if (spf_missing_signals>0) 
          {
            avt_errmsg(SPF_ERRMSG, "021", AVT_WARNING, spf_missing_signals);
          }
        spf_checklofigsignal(spf_Lofig);
        fclose(spfin); 
        mbk_commit_errors(spf_ParsedFile);
      }
}

void parsespf(char *filename)
{
    char buf[1024], *fp;
    groundlosig = NULL;

    spf_checkenv();

    if ((spfin = mbkfopen(filename, NULL, READ_TEXT))!=NULL)
      {
        spf_Lofig = NULL;
        spf_Line = 1;
        if ((fp=filepath(filename, NULL))!=NULL)        
          strcpy(buf,filepath(filename, NULL));
        else
          strcpy(buf,filename);
        spf_ParsedFile=buf;

//        spf_destroyexistingRC(spf_Lofig);
        spf_initBUSDelimiters("", ""); // no delimiter
        spf_setDelimiter('.');
        spf_setDivider('|');
        spf_missing_signals=0;

        spfparse();

        spf_destroy_lotrs_htable();
        spf_destroy_loins_htable();
        spf_destroy_losig_htable();
        spf_finish_pending_capacitances();

        if (spf_missing_signals>0) 
          {
            avt_errmsg(SPF_ERRMSG, "021", AVT_WARNING, spf_missing_signals);
          }
        spf_checklofigsignal(spf_Lofig);
        fclose(spfin); 
        mbk_commit_errors(spf_ParsedFile);
      }
    else
      avt_errmsg(SPF_ERRMSG, "024", AVT_FATAL, filename);
    
}
