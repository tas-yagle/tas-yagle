/* ###--------------------------------------------------------------### */
/* file	   : bvl202.h                                                   */
/* date	   : Oct 1999                                                   */
/* version	: v201                                                  */
/* author   : Pirouz BAZARGAN SABET  v113                               */
/* modif      Stephane PICAULT, Anthony LESTER                          */
/* contents	: defines and structure definitions used in BVL library */
/* ###--------------------------------------------------------------### */

#ifndef BEH_BVLDEF
#define BEH_BVLDEF

   /* ###--------------------------------------------------------### */
   /*    defines                                                     */
   /* ###--------------------------------------------------------### */

#define  BVL_TRACE            0x00000001   /* print messages when parsing     */
#define  BVL_KEEPAUX          0x00000002   /* keep internal signals           */
#define  BVL_CHECKEMPTY       0x00000004   /* check for empty architecture    */

   /* ###--------------------------------------------------------### */
   /*    functions                                                   */
   /* ###--------------------------------------------------------### */

/* New interface */
char *vhdfindbefig(char *name);
struct befig *vhdloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
void          vhdsavebefig (befig_list *ptbefig, unsigned int trace_mode);
struct befig *vbeloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
void          vbesavebefig (befig_list *ptbefig, unsigned int trace_mode);

/* Old interface */

struct befig *vhdlloadbefig (befig_list *ptbefig, char *figname, unsigned int trace_mode);
void          vhdlsavebefig (befig_list *ptbefig, unsigned int trace_mode);

/* For display of individual behavioural objects */

bepor_list     *print_one_bepor (FILE *fp, bepor_list *ptbepor);
void            print_one_bebux (FILE *fp, bebux_list *ptbebux, int *numprocess);
void            print_one_bebus (FILE *fp, bebus_list *ptbebus, int *numprocess);
void            print_one_beaux (FILE *fp, beaux_list *ptbeaux);
void            print_one_beout (FILE *fp, beout_list *ptbeout);
void            print_one_bereg (FILE *fp, bereg_list *ptbereg, int *numprocess);
void            settimeunit (unsigned char time_unit_char);
void            print_one_bevectpor (FILE *fp, bevectpor_list *ptbevectpor);
void            print_one_bevectbux (FILE *fp, bevectbux_list *ptbevectbux, int *numprocess);
void            print_one_bevectbus (FILE *fp, bevectbus_list *ptbevectbus, int *numprocess);
void            print_one_bevectaux (FILE *fp, bevectaux_list *ptbevectaux);
void            print_one_bevectout (FILE *fp, bevectout_list *ptbevectout);
void            print_one_bevectreg (FILE *fp, bevectreg_list *ptbevectreg, int *numprocess);
char       	   *bvl_vhdlname (char *name);
char       	   *bvl_vectorize (char *name);
void bvl_drivecorresp(char *name, befig_list *ptbefig, int vlog);
ht *bvl_buildfinalcorrespht(befig_list *ptbefig);

#endif

