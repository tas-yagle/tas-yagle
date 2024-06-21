/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TMA Version 1                                               */
/*    Fichier : tma_files.c                                                 */
/*                                                                          */
/*    (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Gilles AUGUSTINS                                          */
/*                                                                          */
/****************************************************************************/

#include "tma.h"

/****************************************************************************/

chain_list *tma_ReadFileNames (char *filein) 
{
   chain_list *filechain = NULL;
   char  buf[1024];
   char  mode[1024];
   int   c;
   FILE *file;
   
   if (!(file = mbkfopen (filein, NULL, READ_TEXT))) {
      fprintf (stderr, "WARNING : no file %s\n", filein);
      return NULL;
   }
   
   while (1) {
      c = fscanf (file, "%s%s\n", buf, mode);
      if ((char)c != EOF) {
         if (c)
            filechain = addchain (filechain, namealloc (buf));
         else {
            fprintf (stderr, "WARNING : syntax error in file %s\n", filein);
            return NULL;
         }
      } else
         break;
   }
   
   fclose (file);

   return filechain;
}

/****************************************************************************/

chain_list *tma_ReadFileBehModes (char *filein) 
{
   chain_list *behchain = NULL;
   char  buf[1024];
   char  mode[1024];
   int   c, t, r, w;
   FILE *file;
   
   if (!(file = mbkfopen (filein, NULL, READ_TEXT))) {
      fprintf (stderr, "WARNING : no file %s\n", filein);
      return NULL;
   }
   
   while (1) {
      c = fscanf (file, "%s%s\n", buf, mode);
      if ((char)c != EOF) {
         if (c) {
            switch (mode[0]) {
               case 'T' :
               case 't' :
                  t = 'T';
                  behchain = addchain (behchain, (void*)(long)t);
                  break;
               case 'W' :
               case 'w' :
                  w = 'W';
                  behchain = addchain (behchain, (void*)(long)w);
                  break;
               case 'R' :
               case 'r' :
                  r = 'R';
                  behchain = addchain (behchain, (void*)(long)r);
                  break;
               default :
                  fprintf (stderr, "WARNING : syntax error in file %s, ", filein);
                  fprintf (stderr, "mode T, W, R expected\n");
                  behchain = addchain (behchain, (void*)(long)t);
            }
         } else {
            fprintf (stderr, "WARNING : syntax error in file %s\n", filein);
            return NULL;
         }
      } else
         break;
   }
   
   fclose (file);

   return behchain;
}
