/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : SIM Version 1                                               */
/*    Fichier : sim_genspi.c                                                */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/

extern double sim_calcdelayslope (double *tab, int tabsize, int begin, double seuil, double sfl, double sfh, double last, double *delay, double *slope, int *type, double step, int *indice, char *transition);
extern void sim_GetDelaySlopeByIndex (double *tab, double *delay, double *slope, double vthr, double vsthrl, double vsthrh, double time, double step, int *indice, char *transition);
extern char  *sim_getnode (char *line, char *node);
extern double sim_getspidouble (char *param, char **ptres);
extern int    sim_getspiparam (char *str, char *name, char *param);
extern char  *sim_getspiline (FILE *file, char *str);
extern void   sim_fclose (FILE *file, char *filename);
extern void   sim_readspifile (char *fileout, char *argv[], int nbx, int nby, double **tab, double limit, double simstep);
extern void   sim_readspifiletab (char *fileout, char *argv[], int nbx, int nby, double **tab, double limit, double simstep);
extern int    sim_execspice (char *filename, int silent, char *spicename, char *spicestring, char *spicestdout);
extern char  *sim_getjoker (char *fileout, char *filename);
extern char  *sim_getfilename (char *filename);
extern char  *sim_getarg (char *str, char *filename, int nb, char *buf, int *bufidx);
extern int sim_getallslopes(double *tabx, double *taby, int tabsize, double seuil, double sfl, double sfh, sim_slopes *slopes);

