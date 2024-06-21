/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : zen                                                         */
/*    Fichier : zen_main.c                                                  */
/*                                                                          */
/*    (c) copyright 2002 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Stephane Picault                                        */
/*                  Antony   PINTO                                          */
/*                                                                          */
/****************************************************************************/

#include "zen_main.h"
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
#include EQT_H

struct stat *st;
/****************************************************************************/
/*{{{                    zen_the_question()                                 */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_the_question(ZenFig)
zenfig_list *ZenFig;
{
  fprintf(stderr,"\x1B[1m");
  switch (zen_algo1(ZenFig))
  {
    case 0 : fprintf(stdout,"ALGO 1 -> FALSE\n"); break;
    default: fprintf(stdout,"ALGO 1 -> TRUE\n") ; break;
  }
  fprintf(stderr,"\x1B[0m");
}


/*}}}************************************************************************/
/*{{{                    zen_decOpt()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_decOpt(opt)
char    *opt;
{
  switch (opt[0])
  {
    case 'f' : ZEN_FAST = 1                         ; break;
    case 'l' : ZEN_LEVEL = atoi(opt+1)              ; break;
  }
}

/*}}}************************************************************************/
/*{{{                    zen_getOpt()                                       */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
void zen_getOpt(argc,argv,chip,inf)
int      argc;
char   **argv;
char   **chip;
char   **inf;
{
  int        i;
  
  for (i=argc;i>1;i--)
    if (argv[i-1][0]=='-') zen_decOpt(argv[i-1]+1);
    else if (!*inf)
    {
      *inf = argv[i-1];
      *chip = *inf;
    }
    else *chip = argv[i-1];
}

/****************************************************************************/
/*}}}                                                                       */
/*{{{                    main Fonction                                      */
/*                                                                          */
/****************************************************************************/
int main(argc,argv)
int     argc;
char  **argv;
{
  zenfig_list   *ZenFig;
  befig_list    *BeFigure;
  char          *BeNameFile = NULL;
  char          *PaNameFile = NULL;
//  char           buf[256];
//  FILE          *file;
  ptype_list    *path;
//  char          *top;
//  int            i;
//  char           key[8];

//  pCircuit cct;
//  chain_list *abl;
//  chain_list *abl2;
//  pNode bdd;
//  pNode bdd2;

#ifdef AVERTEC
  avtenv() ;
#endif
  mbkenv ();

  zen_getOpt(argc,argv,&BeNameFile,&PaNameFile);
  
  if (getenv("ZEN_TRACE_MODE")) ZEN_TRACEMODE = 1;
  if (argc==1)                  ZEN_ERROR(3,NULL);
  
  zen_setDelay(70);

  initializeBdd(LARGE_BDD);
  
  BeFigure = loadbefig(NULL,BeNameFile,BVL_TRACE|BVL_KEEPAUX);
  
  if (!beh_chkbefig(BeFigure,0))
  {
    // on cree les dependances in>>out (berin) liste des sorties qui dependent
    // des entrees
    beh_depend(BeFigure);
    // on construit tous les bdds pour chaque expression
    zen_makbehbdd(BeFigure);
  }
//  st = mbkalloc(sizeof(struct stat));
  ZEN_TRACE(0,"loaded");
//  while (1)
//  {
//    fgets(key,8,stdin);
//    fread(key,1,1,stdin);
//    if (key[0]=='q') EXIT(1);
//    if (getch()=='q') EXIT(1);    
//    if ((file=fopen(PaNameFile,"r")))
//    {
//      sleep(2);
//    stat("/users/disk01/antony/dev/test/zen/file.inf",st);
//    if (st->st_size!=0){
//      fclose(file);
      path = zen_loadPath(PaNameFile);
//      if (path)
//      {
      zen_remplacebebux(BeFigure);
      zen_remplacebebus(BeFigure);

      ZenFig = zen_createzenfig(BeFigure,path);
      zen_the_question(ZenFig);
      zen_freezenfig(ZenFig);
//      unlink(PaNameFile);
//      }}
//    }
//  }
  beh_frebefig(BeFigure);
  EXIT(0);
}
