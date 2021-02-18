/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : vcd_parse_core.c                                            */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Antony PINTO                                              */
/*                                                                          */
/****************************************************************************/

#include "vcd_parse_core.h"

static ht   *CDL_TO_VCD = NULL;
static ht   *VCD_TO_CDL = NULL;
static int   PARSE_DONE = 0;

static int   readline(FILE *fd);

/****************************************************************************/
/*{{{                    Internal functions                                 */
/****************************************************************************/
/*{{{                    readline()                                         */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
static inline int readline(FILE *fd)
{
  char       bufcdl[1024], bufvcd[1024], *cdl, *vcd;

  if (fscanf(fd,"%s -> %s\n",bufvcd,bufcdl) == 2)
  {
    vcd = namealloc(bufvcd);
    cdl = namealloc(bufcdl);
    vcd_newVCDToCDLName(vcd,cdl);
    vcd_newCDLToVCDName(cdl,vcd);
  }
  
  return !feof(fd);
}

/*}}}************************************************************************/
/*}}}************************************************************************/
/*{{{                    External functions                                 */
/****************************************************************************/
/*{{{                    vcd_parse_core()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vcd_parse_core(char *filename, char *ext)
{
  FILE      *fd;

  if (VCD_TO_CDL)
    delht(VCD_TO_CDL);
  if (CDL_TO_VCD)
    delht(CDL_TO_VCD);

  VCD_TO_CDL    = addht(100);
  CDL_TO_VCD    = addht(100);
  fd            = mbkfopen(filename,ext,"r");

  while (readline(fd))
    ;
  PARSE_DONE    = 1;
}

/*}}}************************************************************************/
/*{{{                    vcd_cdl_parsed()                                   */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int vcd_cdl_parsed(void)
{
  return PARSE_DONE; 
}

/*}}}************************************************************************/
/*{{{                    vcd_newCDLToVCDName()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vcd_newCDLToVCDName(char *cdl, char *vcd)
{
  addhtitem(CDL_TO_VCD,cdl,(long)vcd);
}

/*}}}************************************************************************/
/*{{{                    vcd_getVCDFromCDLName()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *vcd_getVCDFromCDLName(char *cdl)
{
  char      *res;

  if (! CDL_TO_VCD ||
      (res = (char *)gethtitem(CDL_TO_VCD,cdl)) == (char *)EMPTYHT)
    return NULL;
  else
    return res;
}

/*}}}************************************************************************/
/*{{{                    vcd_newVCDToCDLName()                              */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void vcd_newVCDToCDLName(char *vcd, char *cdl)
{
  addhtitem(VCD_TO_CDL,vcd,(long)cdl);
}

/*}}}************************************************************************/
/*{{{                    vcd_getCDLFromVCDName()                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
char *vcd_getCDLFromVCDName(char *vcd)
{
  char      *res;

  if (!VCD_TO_CDL || 
      (res = (char *)gethtitem(VCD_TO_CDL,vcd)) == (char *)EMPTYHT)
    return vcd;
  else
    return res;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
#if 0
/*{{{                    Test functions                                     */
/****************************************************************************/
/*{{{                                                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
int main(int argc, char *argv[])
{
  vcd_parse_core(argv[1]);

  return 0;
}

/*}}}************************************************************************/
/*}}}************************************************************************/
#endif
