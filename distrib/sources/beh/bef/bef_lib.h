
/* ###--------------------------------------------------------------### */
/* file		: bef100.h						*/
/* version	: v100							*/
/* ###--------------------------------------------------------------### */

#ifndef BEH_BEFDEF
#define BEH_BEFDEF

	/* ###------------------------------------------------------### */
	/*    defines							*/
	/* ###------------------------------------------------------### */

#define BEF_KEEPAUX          0x00000002   /* keep internal signals           */

	/* ###------------------------------------------------------### */
	/*    structure definitions					*/
	/* ###------------------------------------------------------### */

   /* ###--------------------------------------------------------### */
   /*    global variables                                            */
   /* ###--------------------------------------------------------### */

extern char *BEH_IN;                    /* input file format   vbe/vhd/vlg   */
extern char *BEH_OUT;                   /* output file format  vbe/vhd/vlg   */

	/* ###------------------------------------------------------### */
	/*    functions							*/
	/* ###------------------------------------------------------### */

extern void          savebefig      ();
extern struct befig *loadbefig      ();
extern char *findbefig(char *name);

extern char         *bef_getsuffix  ();

typedef struct bef_abl
{
  char          *(*TOBOOLSTR)   (chain_list*,char*,int*);
}
bef_abl;

typedef struct bef_por
{
  ht            *PORHT;
  void           (*POR)         (FILE*,char*,char,int,int);
  void           (*START)       (FILE*);
  void           (*END)         (FILE*);
}
bef_por;

typedef struct bef_process
{
//  void           (*PROCESS)     (FILE*,biabl_list*,char*,char*,int,int*,char*,int);
  chain_list    *(*DECLAR)      (FILE*,biabl_list*,char*,int,char*,char**,ptype_list*);
  void           (*WITHSELECT)  (FILE*,biabl_list*,char*,int,char**,int*);
  void           (*BEGIN)       (FILE*,chain_list*);
  void           (*END)         (FILE*);
  void           (*IF)          (FILE*,char*);
  void           (*ELSIF)       (FILE*,char*);
  int            (*ELSE)        (FILE*);
  int            (*ENDIF)       (FILE*,char);
  int            (*EDGE)        (char*,char*,char,char*);
}
bef_process;

typedef struct bef_name
{
  char          *(*NAME)        (char*); 
  char          *(*VNAME)       (char*,int,int); 
  char          *(*VECTOR)      (char*, char*);
  char          *(*BITSTR)      (char*,char*);
  char          *(*BEBUS)       (char*); 
  char          *(*BEREG)       (char*); 
}
bef_name;

typedef struct bef_driver
{
  FILE          *FP;
  befig_list    *FIG;
  char         **ABUFF;
  int           *ABUFS;
  bef_por        POR;
  bef_process    PROC;
  void           (*TIME)        (FILE*,int,int,int);
  void           (*END_LOOP)    (FILE*,char,char*,int,int,int,loop_list*);
  void           (*LOOP_LIST)   (FILE*,char,char*,int,int*,int,loop_list*);
  void           (*LOOP)        (FILE*,char,char*,int,int*,loop_list*);
  void           (*VALUE)       (FILE*,chain_list*,char,int,int,int,char**,int*);
  void           (*AFFECTED)    (FILE*,char,char,int,int,int,char*,char*,loop_list*,long);
  void           (*SIGNAL)      (FILE*,char*,char,int,int);
  void           (*INTER_ASS)   (FILE*,char*,char*,int);
  int            (*ISONE)       (char*);
  bef_abl        ABL;
  bef_name       STR;
  int VLG;
}
bef_driver;

chain_list  *bef_preprocess_abl(chain_list *abl);

char        *bef_gettimeunit(unsigned char time_unit_char);
void         iprint(FILE *fp,char mode,char *str, ... );
void         bef_print_one_beobj(bef_driver *driver, void *be, int *np,long type);
chain_list  *bef_strip_ckedge(chain_list *ptabl, char *clock);
int          bef_search_stable(chain_list *ptabl, char **clock);
int          bef_search_notck(chain_list * ptabl, char *clock);
//void bef_drive_vpor(bef_driver *driver, bevectpor_list *vpor,int cpt);
//void bef_drive_por(bef_driver *driver, bepor_list *por);
void bef_initMaxname(void);
void bef_drive_porDeclaration(bef_driver *driver);
void bef_drive_sigDeclaration(bef_driver *driver);

int          bef_get_stable_attribute(chain_list *ptabl, char **clock);

void         bef_drive_aux(bef_driver *driver);
void         bef_drive_out(bef_driver *driver);
void         bef_drive_bux(bef_driver *driver);
void         bef_drive_bus(bef_driver *driver);
void         bef_drive_reg(bef_driver *driver);

void bef_driverSetAbl(bef_driver *driver,
                      char *(*abl2strBool)(chain_list*,char*,int*));
void bef_driverSet(bef_driver   *driver,
                   FILE         *fp,
                   befig_list   *fig,
                   char        **buffer,
                   int          *bufsize,
                   void          (*time)        (FILE*,int,int,int),
                   void          (*end_loop)    (FILE*,char,char*,int,int,int,loop_list*),
                   void          (*looplist)    (FILE*,char,char*,int,int*,int,loop_list*),
                   void          (*loop)        (FILE*,char,char*,int,int*,loop_list*),
                   void          (*value)       (FILE*,chain_list*,char,int,int,int,char**,int*),
                   void          (*affected)    (FILE*,char,char,int,int,int,char*,char*,loop_list*,long),
                   void          (*signal)      (FILE*,char*,char,int,int),
                   void          (*inter_ass)   (FILE*,char*,char*,int),
                   int           (*isone)       (char*));
void bef_driverSetProcess(bef_driver    *driver,
                          void           (*withselect)  (FILE*,biabl_list*,char*,int,char**,int*),
                          chain_list    *(*declar)      (FILE*,biabl_list*,char*,int,char*,char**,ptype_list *),
                          void           (*begin)       (FILE*,chain_list*),
                          void           (*end)         (FILE*),
                          void           (*if_)         (FILE*,char*),
                          void           (*elsif)       (FILE*,char*),
                          int            (*else_)       (FILE*),
                          int            (*endif)       (FILE*,char),
                          int            (*edge)        (char*,char*,char,char*));
void bef_driverSetStr(bef_driver    *driver,
                      char          *(*name)        (char*),
                      char          *(*vname)       (char*,int,int),
                      char          *(*vector)      (char*, char*),
                      char          *(*bitstr)      (char*,char*),
                      char          *(*bebus)       (char*),
                      char          *(*bereg)       (char*));
void bef_driverSetPor(bef_driver    *driver,
                      void           (*por)     (FILE*,char*,char,int,int),
                      void           (*end)     (FILE*),
                      void           (*start)   (FILE*),
                      ht            *porht);

#endif
