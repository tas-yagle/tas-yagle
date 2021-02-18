/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : genius.c                                                    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 11/08/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include MUT_H
#include MLO_H
#include MLU_H
#include "gen_tree_utils.h"
#include "gen_env.h"
#include "gen_tree_errors.h"
#include "gen_model_utils.h"
#include "gen_model_global.h"
#include "gen_verif_global.h"
#include "gen_execute_C.h"


#ifndef GENIUS_VERSION
#define GENIUS_VERSION "0.8"
#endif

#define PRIME 211 /* undivisable number */

/* LEX&YACC functions parser */
/* from gen_library_parser.yac */
extern lib_entry *Read_Library(char* library,char* path);
/* from gen_tree_parser.yac */
extern tree_list *Read_All();



extern int main(int argc,char *argv[]){
   int err;
   ptype_list *p,*bi_list;
   chain_list *l,*a,*fcl=NULL,*genius=NULL;
   ptype_list *env;
   
   /* init environment */
   mbkenv();
   genius_env(NULL);
   control_env();
   srand(PRIME);
   
   /* program */
   alliancebanner("genius",GENIUS_VERSION,"GEneric Netlist Identification for User Specification","98-99","3.2");

   /* ----> GENIUS_PRIORITY */
   GENIUS_PRIORITY = Read_Library(GENIUS_LIB_NAME,GENIUS_LIB_PATH); 
   if (!GENIUS_PRIORITY) EXIT(0);
   err = Get_Error();
   if (err) {
       fprintf(stderr,"\n%d error(s) detected,  I can't get farther!!\n",err);
       EXIT(err);
   }
   
   GENIUS_TREE = Read_All(GENIUS_PRIORITY);  /* ----> GENIUS_TREE */
   err = Get_Error();   
   if (err) {
       fprintf(stderr,"\n%d error(s) detected,  I can't get farther!!\n",err);
       EXIT(err);
   }
   if (!GENIUS_TREE) EXIT(0);

   /*get a ptype_list for GENIUS and FCL */
   bi_list=Verif_All(GENIUS_TREE);
   for (p=bi_list; p; p=p->NEXT) {
      if (p->TYPE==FCL) fcl=(chain_list*)p->DATA;
      if (p->TYPE==GENIUS) genius=(chain_list*)p->DATA;
   }
   freeptype(bi_list);
   err = Get_Error();   
   if (err) {
       fprintf(stderr,"\n%d error(s) detected,  I can't get farther!!\n",err);
       EXIT(err);
   }
   Build_All_Models(GENIUS_TREE,fcl,genius);
   
   /*free mem */
   Free_Tree(GENIUS_TREE);
   GENIUS_TREE=NULL;
   
   dumpmodel();

   for (a=fcl; a; a=a->NEXT) {
      model_list* model=getmodel((char*)a->DATA);
      if (model->C) {
         fprintf(GENIUS_OUTPUT,
         "\n*************** Action of %s *****************\n",
         model->NAME);
         Exec_C(model->C,NULL);
      }   
   }
   
   for (a=genius; a; a=a->NEXT) {
      model_list* model=getmodel((char*)a->DATA);
      if (model->C) {
         env=NULL;
         for (l=model->VAR; l; l=l->NEXT) env=addptype(env,0,l->DATA);      
         env=addptype(env,(int)vectorradical(model->NAME),
                     vectorradical(model->NAME));
         fprintf(GENIUS_OUTPUT,
         "\n*************** Action of %s *****************\n",
         model->NAME);
         Exec_C(model->C,env);
         freeptype(env);
      }   
   }
   
   Free_Exec_C();
   freemodel();
   freechain(fcl);
   freechain(genius);   

   /*close file report .gen*/
   if (GENIUS_OUTPUT!=stdout && GENIUS_OUTPUT!=stderr) fclose(GENIUS_OUTPUT);

   /*file names are used for message and idetification in GENIUS*/
   for (p=GENIUS_PRIORITY;p;p=p->NEXT) mbkfree(p->DATA);
   freeptype(GENIUS_PRIORITY);
   GENIUS_PRIORITY=NULL;

   EXIT(0);
}


