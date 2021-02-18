/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : ZEN Version 1.00                                            */
/*    Fichier : zen_util.c                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Stephane Picault                                        */
/*                  Antony PINTO                                            */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"


/****************************************************************************/
/*                       zen_loadPath()                                     */
/* charge un chemin depuis le point .inf, la syntaxe generee par Marc KUOCH */
/* est:                                                                     */
/*                                                                          */
/* PATHSIGS                                                                 */
/* <nom_de_signal1> <slope1>                                                */
/* <nom_de_signal2> <slope2>                                                */
/*      : : :         :::                                                   */
/*      : : :         :::                                                   */
/* <nom_de_signaln> <slopen>                                                */
/* END                                                                      */
/****************************************************************************/
ptype_list *zen_loadPath(FileName)
char        *FileName;
{
  ptype_list    *newPt;
  FILE          *File;
  char           Buffer[255];
  char           slope;
  char          *signame;
  int            i = 0;
  char           top[64];
  int            nameLenght;
  
  newPt = NULL;
 
  while (1)
  {
    nameLenght = strlen(FileName);
    if (!strcmp(FileName+(nameLenght-4),".inf"))
      FileName[nameLenght-4] = '\0';
    else break;
  }
  sprintf(Buffer,"%s.inf",FileName);
  if (!ZEN_TESTMODE) ZEN_TRACE(1,Buffer);
  if (!(File=fopen(Buffer,"r")))
  {
    ZEN_WARNING(2,Buffer);
    if (!(File=fopen("file.inf","r"))) return NULL;
  }
  
  while (fgets(Buffer,255,File))
  {
    Buffer[strlen(Buffer)-1] = '\0';
    if (!i)
      if (!strcmp(Buffer,"PATHSIGS"))
      {
        i++;
        continue;
      }
    if (strlen(Buffer)<=1) continue;
    if (!strcmp(Buffer,"END")) break;
    sscanf(Buffer,"%s",Buffer);
    slope = zen_getSlope(Buffer);
    signame = zen_traductName(Buffer);
    sprintf(Buffer,"%-15s -> %c",signame,slope);
    if (i==1)
    { 
      sprintf(top,"%s",signame);
      i++;
    }
    ZEN_TST(0,Buffer);    
    newPt = addptype(newPt,slope,signame);
  }
  fclose(File);
  sprintf(Buffer,"path from %s to %s loaded",top,signame);
  ZEN_TRACE(0,Buffer);
  return (newPt);
}

/****************************************************************************/
/*                       zen_getSlope()                                     */
/*                                                                          */
/* recupere le front                                                        */
/****************************************************************************/
char zen_getSlope(line)
char        *line;
{
  int        i;
  
  for (i=0;line[i]!='<';i++);
  return line[i+2];
}

/****************************************************************************/
/*                       zen_traduct_name()                                 */
/*                                                                          */
/* traduit un nom en supprimant les [, ] et les ., mets _ a la place        */
/****************************************************************************/
char *zen_traductName(name)
char    *name;
{ 
  int        i,j; 
  char       NewName[255];
 
  for(i=0,j=0;name[i]!='\0';i++)
    if ((name[i]=='.')||(name[i]=='[')) NewName[j++] = '_';
    else if (name[i]!=']') NewName[j++] = name[i];
  NewName[j] = '\0';
  
  return (namealloc(NewName));
}

