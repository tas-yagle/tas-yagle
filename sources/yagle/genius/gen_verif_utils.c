/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_verif_utils.c                                           */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 10/06/1999    */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include MUT_H
#include MLO_H
#include API_H
#include AVT_H
#include "gen_verif_utils.h"



/***************************************************************************/
/* put ident at the end of the list, this element must be unique           */
/* comparison is supposed to be on string                                  */
/* if an elem has the same key, an error is dumped                         */
/***************************************************************************/
extern chain_list *put_inlist(ident,list) 
   tree_list *ident;
   chain_list *list;
{
   chain_list *p;
   char *name;
   if (!ident) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 252);
//      fprintf(stderr,"put_inlist: NULL pointer\n");
      EXIT(1);
   }   
   name=getname(ident);
   
   for (p=list;p;p=p->NEXT) {
      if (!p->DATA) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 253);
//         fprintf(stderr,"put_inlist: NULL pointer\n");
         EXIT(1);
      }  
      if (getname(p->DATA)==name) {
         avt_errmsg(GNS_ERRMSG, "163", AVT_ERROR, FILE_NAME(ident),LINE(ident), name, LINE((tree_list*)p->DATA), FILE_NAME((tree_list*)p->DATA));
         /*fprintf(stderr,"%s:%d: '%s' used several times,  primary use line %d in file %s\n",
                 FILE_NAME(ident),LINE(ident), name, LINE((tree_list*)p->DATA), FILE_NAME((tree_list*)p->DATA));*/
         Inc_Error();
         return list;
      }
      if (!p->NEXT) {p->NEXT=addchain(NULL,ident);return list;}
   }  
   return addchain(NULL,ident);
}


/***************************************************************************/
/* return the element of list wich DATA field equals DATA field of ident   */
/* comparison is supposed to be on string                                  */
/* if not found NULL is returned and an error is dumped                    */
/***************************************************************************/
extern tree_list *fetch_inlist(ident,list) 
   tree_list *ident;
   chain_list *list;
{
   chain_list *p;
   char *name;
   if (!ident) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 254);
//      fprintf(stderr,"fetch_inlist: NULL pointer\n");
      EXIT(1);
   }  
   name=getname(ident);
   
   for (p=list;p;p=p->NEXT) {
      if (!p->DATA) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 254);
//         fprintf(stderr,"fetch_inlist: NULL pointer\n");
         EXIT(1);
      }  
      if (strcasecmp(getname(p->DATA),name)==0) {
      return p->DATA;   }
   }
   
    avt_errmsg(GNS_ERRMSG, "164", AVT_ERROR, FILE_NAME(ident),LINE(ident), name);
//   fprintf(stderr,"%s:%d: '%s' not defined\n",FILE_NAME(ident),LINE(ident), name);
   Inc_Error();
   return NULL;
}

        
/***************************************************************************/
/* put ident with a value at the TOP of the list, this element must unique */
/* comparison is supposed to be on string                                  */
/* if an elem has the same key, an error is dumped                         */
/***************************************************************************/
extern ptype_list *put_value(ident,value,list) 
   int value;
   tree_list *ident;
   ptype_list *list;
{
   ptype_list *p;
   char *name;
   if (!ident) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 255);
//      fprintf(stderr,"put_value: NULL pointer\n");
      EXIT(1);
   }   
   name=getname(ident);
   
   for (p=list;p;p=p->NEXT) {
      if (!p->DATA) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 256);
//         fprintf(stderr,"put_value: NULL pointer\n");
         EXIT(1);
      }  
      if (getname(p->DATA)==name) {
         fprintf(stderr,"%s:%d: '%s' used several times,  primary use line %d in file %s\n",
                 FILE_NAME(ident),LINE(ident), name, LINE((tree_list*)p->DATA), FILE_NAME((tree_list*)p->DATA));
         Inc_Error();
         return list;
      }
      if (!p->NEXT) {return addptype(list,value,ident);}
   }  
   return addptype(NULL,value,ident);
}


/***************************************************************************/
/* return the value of the element of list wich DATA field equals DATA     */
/* field of ident                                                          */
/* comparison is supposed to be on string                                  */
/* if not found NULL is returned and an error is dumped                    */
/***************************************************************************/
extern int fetch_value(ident,list) 
   tree_list *ident;
   ptype_list *list;
{
   ptype_list *p;
   char *name;
   if (!ident) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 257);
//      fprintf(stderr,"fetch_value: NULL pointer\n");
      EXIT(1);
   }  
   name=getname(ident);
   
   for (p=list;p;p=p->NEXT) {
      if (!p->DATA) {
    avt_errmsg(GNS_ERRMSG, "004", AVT_FATAL, 258);
//         fprintf(stderr,"fetch_value: NULL pointer\n");
         EXIT(1);
      }  
      if (getname(p->DATA)==name) {
      return p->TYPE;}
   }
   
    avt_errmsg(GNS_ERRMSG, "164", AVT_ERROR, FILE_NAME(ident),LINE(ident), name);
   //fprintf(stderr,"%s:%d: '%s' not defined (v2)\n",FILE_NAME(ident),LINE(ident), name);
   Inc_Error();
   return -1;
}



