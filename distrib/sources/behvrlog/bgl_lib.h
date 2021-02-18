/*                                                                          */
/*    Produit : Structural Verilog Compiler                                 */
/*    Fichier : bgl100.h                                                    */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Anthony LESTER                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/
#ifndef BEH_BGLDEF
#define BEH_BGLDEF

#define BGL_CASEHT_PTYPE   ((long) 0x66000018 )

extern int           BGL_CASE_SENSITIVE;

void   bgl_delcaseht(ht *caseht);
char  *bgl_getcasename(ht *caseht, char *text);

   /* ###--------------------------------------------------------### */
   /*    defines                                                     */
   /* ###--------------------------------------------------------### */

#define  BGL_TRACE            0x00000001   /* print messages when parsing     */
#define  BGL_KEEPAUX          0x00000002   /* keep internal signals           */
#define  BGL_CHECKEMPTY       0x00000004   /* check for empty architecture    */

   /* ###--------------------------------------------------------### */
   /*    global variables                                            */
   /* ###--------------------------------------------------------### */

extern int BGL_DRIVE_PRIMITIVES;
extern int BGL_ONLY_PRIMITIVES;

   /* ###--------------------------------------------------------### */
   /*    functions                                                   */
   /* ###--------------------------------------------------------### */

char *vlogfindbefig(char *name);
struct befig *vlogloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
void          vlogsavebefig (befig_list *ptbefig, unsigned int trace_mode);
char              *bgl_vlogname (char *name);
#endif

