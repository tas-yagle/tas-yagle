
/* ###--------------------------------------------------------------### */
/* file		: beh110.h						                            */
/* date		: Oct 30 1995						                        */
/* version	: v110							                            */
/* author(s): Pirouz BAZARGAN SABET					                    */
/*          : Antony PINTO                          					*/
/* contents	: low level library for behavioural description		        */
/* ###--------------------------------------------------------------### */

#ifndef BEH_BEHDEF
#define BEH_BEHDEF

	/* ###------------------------------------------------------### */
	/*    defines							                        */
	/* ###------------------------------------------------------### */

#define BEH_GENERIC            203698	/* generic ptype code		*/
#define BEH_STABLE               0x01	/* description using STABLE	*/

#define BEH_NODE_BDD             0x01	/* fig TYPE : expr. BDD		*/
#define BEH_NODE_GEX             0x02	/* fig TYPE : expr. GEX		*/
#define BEH_NODE_VARLIST         0x04	/* fig TYPE : list of variables	*/
#define BEH_NODE_DERIVATE        0x08	/* fig TYPE : derivatives	*/
#define BEH_NODE_USER            0x10	/* fig TYPE : user expression	*/
#define BEH_NODE_QUAD            0x20	/* fig TYPE : QUAD structure	*/

#define BEH_CHK_DRIVERS    0x00000001	/* check output drivers		*/
#define BEH_CHK_EMPTY      0x00000002	/* check for empty architecture	*/

#define BEH_ARC_VHDL             0x01	/* VHDL architecture		*/
#define BEH_ARC_C                0x02	/* C    architecture		*/

#define BEH_CND_UNKNOWN    0x00000000	/* cond. precedence is unknown	*/
#define BEH_CND_PRECEDE    0x00000001	/* cond. has a precedence	*/
#define BEH_CND_NOPRECEDE  0x00000002	/* cond. has no precedence	*/
#define BEH_CND_SELECT     0x00000004	/* cond. is vectorial, same */
#define BEH_CND_SEL_OTHERS 0x00000008   /* for multiple values	    */
#define BEH_CND_CMPLX      0x00000010   /* ...................	    */
#define BEH_CND_LOOP       0x00000020   /* for loop                 */

#define BEH_FIG__TIMEUNIT   0x07	/* time unit			*/
#define BEH_TU__FS		    1	/* time unit : fs		*/
#define BEH_TU__PS		    2	/* time unit : ps		*/
#define BEH_TU__NS		    3	/* time unit : ns		*/
#define BEH_TU__US		    4	/* time unit : us		*/
#define BEH_TU__MS		    5	/* time unit : ms		*/

#define BEH_TYPE_CND        0xc1c11A02

#ifdef LEFT
#undef LEFT
#endif

#ifdef RIGHT
#undef RIGHT
#endif

	/* ###------------------------------------------------------### */
	/*    global variables						*/
	/* ###------------------------------------------------------### */

extern unsigned int   BEH_LINNUM  ;

extern unsigned char  BEH_TIMEUNIT;
extern float          BEH_CNV_FS  ;
extern float          BEH_CNV_PS  ;
extern float          BEH_CNV_NS  ;
extern float          BEH_CNV_US  ;
extern float          BEH_CNV_MS  ;

	/* ###------------------------------------------------------### */
	/*    structure definitions					*/
	/* ###------------------------------------------------------### */

struct beden			/* dictionary entry point		*/
  {
  struct beden *NEXT;		/* next entry				*/
  struct bedrd *DATA;		/* data					*/
  char         *KEY ;		/* key					*/
  };

struct bedrd			/* dictionary record			*/
  {
  struct bedrd *NEXT   ;	/* next record				*/
  char         *KEY    ;	/* context key				*/
  short         FD0_VAL;	/* data field				*/
  short         FD1_VAL;	/* data field				*/
  short         FD2_VAL;	/* data field				*/
  short         FD3_VAL;	/* data field				*/
  short         FD4_VAL;	/* data field				*/
  short         FD5_VAL;	/* data field				*/
  short         FD6_VAL;	/* data field				*/
  long          PNT_VAL;	/* data field				*/
  };

typedef struct befig		/* behaviour figure			*/
  {
  struct befig      *NEXT     ;	/* next figure				*/
  char              *NAME     ;	/* figure's name			*/
  struct bereg      *BEREG    ;	/* list of internal signals (REGISTER)	*/
  struct bevectreg  *BEVECTREG;	/* list of internal signals (REGISTER)	*/
  struct bemsg      *BEMSG    ;	/* list of assert instructions		*/
  struct berin      *BERIN    ;	/* list of inputs (registers and ports)	*/
  struct beout      *BEOUT    ;	/* list of output ports			*/
  struct bevectout  *BEVECTOUT;	/* list of output ports			*/
  struct bebus      *BEBUS    ;	/* list of output ports (BUS)		*/
  struct bevectbus  *BEVECTBUS;	/* list of output ports (BUS)		*/
  struct beaux      *BEAUX    ;	/* list of internal signals		*/
  struct bevectaux  *BEVECTAUX;	/* list of internal signals		*/
  struct beaux      *BEDLY    ;	/* internal signals used for 'DELAYED	*/
  struct bebux      *BEBUX    ;	/* list of internal signals (BUS)	*/
  struct bevectbux  *BEVECTBUX;	/* list of internal signals (BUS)	*/
  struct bepor      *BEPOR    ;	/* list of ports in reverse order	*/
  struct bevectpor  *BEVECTPOR;	/* list of ports in reverse order	*/
  struct begen      *BEGEN    ;	/* list of generic data			*/
  struct circuit    *CIRCUI   ;	/* Pointer on Circuit for BDDs		*/
  ptype_list        *USER     ;	/* reserved for user's applications	*/
  char               ERRFLG   ;	/* error flag				*/
  char               TYPE     ;	/* description type			*/
  unsigned char      FLAG     ;	/* description flag			*/
  unsigned char      TIME_UNIT;	/* time unit				*/
  }
befig_list;

typedef struct bereg		/* register				*/
  {
  struct bereg  *NEXT  ;	/* next register			*/
  char          *NAME  ;	/* register's name			*/
  struct biabl  *BIABL ;	/* list of drivers (ABL)		*/
  struct binode *BINODE;	/* list of drivers (BDD)		*/
  }
bereg_list;

typedef struct bemsg		/* assert instruction			*/
  {
  struct bemsg	*NEXT   ;	/* next assertion			*/
  char		*LABEL  ;	/* instruction's label			*/
  char		*MESSAGE;	/* assertion's message			*/
  struct chain	*ABL    ;	/* assertion's condition (ABL)		*/
  struct node	*NODE   ;	/* assertion's condition (BDD)		*/
  char           LEVEL  ;	/* severity level of the assertion	*/
  }
bemsg_list;

typedef struct beout		/* simple output port			*/
  {
  struct beout	*NEXT;		/* next output				*/
  char		*NAME;		/* port's name				*/
  struct chain	*ABL ;		/* port's equation (ABL)		*/
  struct node	*NODE;		/* port's equation (BDD)		*/
  unsigned int   TIME;		/* driver's delay			*/
  }
beout_list;

typedef struct bebus		/* bussed output port			*/
  {
  struct bebus	*NEXT  ;	/* next bus port			*/
  char		*NAME  ;	/* port's name				*/
  struct biabl	*BIABL ;	/* list of drivers (ABL)		*/
  struct binode	*BINODE;	/* list of drivers (BDD)		*/
  char           TYPE  ;	/* port's type : W or M			*/
  }
bebus_list;

typedef struct beaux		/* simple internal signal		*/
  {	
  struct beaux	*NEXT;		/* next signal				*/
  char		*NAME;		/* signal's name			*/
  struct chain	*ABL ;		/* signal's equation (ABL)		*/
  struct node	*NODE;		/* signal's equation (BDD)		*/
  unsigned int   TIME;		/* driver's delay			*/
  }
beaux_list;

typedef struct bebux		/* bussed internal signal		*/
  {	
  struct bebux	*NEXT  ;	/* next signal				*/
  char		*NAME  ;	/* signal's name			*/
  struct biabl	*BIABL ;	/* list of drivers (ABL)		*/
  struct binode	*BINODE;	/* list of drivers (BDD)		*/
  char           TYPE  ;	/* signal's type : W or M		*/
  }
bebux_list;

typedef struct bepor		/* port					*/
  {
  struct bepor *NEXT     ;	/* next port				*/
  char         *NAME     ;	/* port's name				*/
  char          DIRECTION;	/* port's mode (I, O, B, Z, T)		*/
  char          TYPE     ;	/* port's type (B, M or W)		*/
  }
bepor_list;

typedef struct biabl		/* couple of ABL			*/
  {
  struct biabl	*NEXT  ;	/* next couple				*/
  char          *LABEL ;	/* driver's label			*/
  struct chain	*CNDABL;	/* driver's connection condition (ABL)	*/
  struct chain	*VALABL;	/* equation of driven value (ABL)	*/
  ptype_list  *USER  ;	/* user field				*/
  unsigned int   TIME  ;	/* driver's delay			*/
  long           FLAG  ;	/* condition's flag			*/
  }
biabl_list;

typedef struct binode		/* couple of BDD			*/
  {
  struct binode	*NEXT   ;	/* next figure				*/
  struct node	*CNDNODE;	/* driver's connection condition (BDD)	*/
  struct node	*VALNODE;	/* equation of driven value (BDD)	*/
  unsigned int   TIME   ;	/* driver's delay			*/
  }
binode_list;

typedef struct berin		/* primary inputs			*/
  {
  struct berin *NEXT;		/* next input				*/
  char         *NAME;		/* input's name				*/
  struct chain *OUT_REF;        /* dependent simple output		*/
  struct chain *BUS_REF;        /* dependent bussed output		*/
  struct chain *AUX_REF;        /* dependent simple internal signal	*/
  struct chain *REG_REF;        /* dependent internal register		*/
  struct chain *BUX_REF;        /* dependent bussed internal signal	*/
  struct chain *MSG_REF;        /* dependent assertion			*/
  struct chain *DLY_REF;        /* dependent delayed internal signal	*/
  }
berin_list;

typedef struct begen		/* generic				*/
  {
  struct begen *NEXT ;		/* next generic				*/
  char         *NAME ;		/* generic`s name			*/
  char         *TYPE ;		/* generic type`s name			*/
  void         *VALUE;		/* generic`s value			*/
  }
begen_list;

typedef struct bequad
  {
  struct node  *BDD     ;	/* expression using BDD format		*/
  struct begex *GEX     ;	/* expression using GEX format		*/
  struct chain *VARLIST ;	/* variable list			*/
  struct beder *DERIVATE;	/* list of derivate BDDs		*/
  void         *USER    ;	/* user defined form of the expression	*/
  }
bequad_list;

typedef struct beder
  {
  struct beder *NEXT ;		/* next derivative			*/
  struct node  *BDD  ;		/* derivative BDD			*/
  short         INDEX;		/* index of the variable		*/
  }
beder_list;

typedef struct begex
  {
  struct chain   *OPERAND;	/* list of operands or NULL for term	*/
  unsigned int    TERM   ;	/* operator or terminal's index		*/
  unsigned short  TYPE   ;	/* expression's type index		*/
  }
begex_list;
typedef struct bevectreg		    /* register				        */
{
  struct bevectreg  *NEXT       ;	/* next register			    */
  char              *NAME       ;	/* register's name			    */
  struct vectbiabl  *VECTBIABL  ;	/* list of drivers (ABL)		*/
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
} bevectreg_list;

typedef struct bevectout		    /* simple output port			*/
{
  struct bevectout	*NEXT       ;	/* next output				    */
  char		        *NAME       ;	/* port's name				    */
  struct chain	    *ABL        ;	/* port's equation (ABL)		*/
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
  unsigned int  	 TIME		;	/* driver's delay			*/
} bevectout_list;

typedef struct bevectbus		    /* bussed output port			*/
{
  struct bevectbus	*NEXT       ;	/* next bus port			    */
  char		        *NAME       ;	/* port's name				    */
  struct vectbiabl  *VECTBIABL  ;	/* list of drivers (ABL)		*/
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
  char         		 TYPE     	;	/* port's type (B, M or W)		*/
} bevectbus_list;

typedef struct bevectaux		    /* simple internal signal		*/
{	
  struct bevectaux	*NEXT       ;	/* next signal				    */
  char		        *NAME       ;	/* signal's name			    */
  struct chain	    *ABL        ;	/* signal's equation (ABL)		*/
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
  unsigned int   	 TIME		;	/* driver's delay			*/
} bevectaux_list;

typedef struct bevectbux		    /* bussed internal signal		*/
{	
  struct bevectbux	*NEXT       ;	/* next signal				    */
  char		        *NAME       ;	/* signal's name			    */
  struct vectbiabl  *VECTBIABL  ;	/* list of drivers (ABL)		*/
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
  char         		 TYPE     	;	/* port's type (B, M or W)		*/
} bevectbux_list;

typedef struct bevectpor		    /* port					        */
{
  struct bevectpor  *NEXT       ;	/* next port				    */
  char              *NAME       ;	/* port's name				    */
  char               DIRECTION  ;	/* port's mode (I, O, B, Z, T)	*/
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
  char               TYPE       ;	/* port's type (B, M or W)		*/
} bevectpor_list;

typedef struct vectbiabl
{
  struct vectbiabl  *NEXT       ;
  unsigned short     LEFT       ;
  unsigned short     RIGHT      ;
  struct biabl      *BIABL      ;
} vectbiabl_list;

	/* ###------------------------------------------------------### */
	/*    functions							*/
	/* ###------------------------------------------------------### */

extern struct beaux  *beh_addbeaux  ();
extern struct bebux  *beh_addbebux  ();
extern struct bebus  *beh_addbebus  ();
extern struct befig  *beh_addbefig  ();
extern struct begen  *beh_addbegen  ();
extern struct bemsg  *beh_addbemsg  ();
extern struct beout  *beh_addbeout  ();
extern struct bepor  *beh_addbepor  ();
extern struct bequad *beh_addbequad ();
extern struct bereg  *beh_addbereg  ();
extern struct berin  *beh_addberin  ();
extern struct biabl  *beh_addbiabl  ();
extern struct binode *beh_addbinode ();
extern struct beder  *beh_addbeder  ();
extern struct beaux  *beh_delbeaux  ();
extern struct bebux  *beh_delbebux  ();
extern struct bebus  *beh_delbebus  ();
extern struct befig  *beh_delbefig  ();
extern struct begen  *beh_delbegen  ();
extern struct bemsg  *beh_delbemsg  ();
extern struct beout  *beh_delbeout  ();
extern struct bepor  *beh_delbepor  ();
extern struct bereg  *beh_delbereg  ();
extern struct berin  *beh_delberin  ();
extern struct biabl  *beh_delbiabl  ();
extern struct binode *beh_delbinode ();
extern struct beder  *beh_delbeder  ();
extern void           beh_delbequad ();
extern void           beh_frebeaux  ();
extern void           beh_frebebux  ();
extern void           beh_frebebus  ();
extern void           beh_frebefig  ();
extern void           beh_frebegen  ();
extern void           beh_frebemsg  ();
extern void           beh_frebeout  ();
extern void           beh_frebepor  ();
extern void           beh_frebereg  ();
extern void           beh_freberin  ();
extern void           beh_frebiabl  ();
extern void           beh_frebinode ();
extern void           beh_frebeder  ();
extern struct beaux  *beh_rmvbeaux  ();
extern struct bebux  *beh_rmvbebux  ();
extern struct bebus  *beh_rmvbebus  ();
extern struct befig  *beh_rmvbefig  ();
extern struct begen  *beh_rmvbegen  ();
extern struct bemsg  *beh_rmvbemsg  ();
extern struct beout  *beh_rmvbeout  ();
extern struct bepor  *beh_rmvbepor  ();
extern struct bereg  *beh_rmvbereg  ();
extern struct berin  *beh_rmvberin  ();
extern struct beder  *beh_rmvbeder  ();

extern struct beden **beh_initab    ();
extern void           beh_addtab    ();
extern long           beh_chktab    ();
extern void           beh_fretab    ();

extern unsigned int   beh_error     ();
extern void           beh_message   ();
extern void           beh_toolbug   ();

extern bevectaux_list   *beh_addbevectaux   ();
extern bevectbux_list   *beh_addbevectbux   ();
extern bevectbus_list   *beh_addbevectbus   ();
extern bevectout_list   *beh_addbevectout   ();
extern bevectpor_list   *beh_addbevectpor   ();
extern bevectreg_list   *beh_addbevectreg   ();
 
extern bevectaux_list   *beh_delbevectaux   ();
extern bevectbux_list   *beh_delbevectbux   ();
extern bevectbus_list   *beh_delbevectbus   ();
extern bevectout_list   *beh_delbevectout   ();
extern bevectpor_list   *beh_delbevectpor   ();
extern bevectreg_list   *beh_delbevectreg   ();

extern void              beh_frebevectaux   ();
extern void              beh_frebevectbux   ();
extern void              beh_frebevectbus   ();
extern void              beh_frebevectout   ();
extern void              beh_frebevectpor   ();
extern void              beh_frebevectreg   ();

extern vectbiabl_list   *beh_addvectbiabl   ( vectbiabl_list    *lastvectbiabl,
                                              unsigned short     left,
                                              unsigned short     right,
                                              biabl_list        *biabl);
extern vectbiabl_list   *beh_delvectbiabl   ( vectbiabl_list    *headabl,
                                              vectbiabl_list    *ptlastabl,
                                              char               mode);
extern void              beh_frevectbiabl       ( vectbiabl_list    *listvectbiabl);
extern void              beh_addcndvect2biabl   ( biabl_list        *biabl,
                                                  char              *cndVect);
extern char             *beh_getcndvectfbiabl   ( biabl_list        *biabl);
extern void              beh_delcndvectfbiabl   ( biabl_list        *biabl);
extern int               beh_simplifybiabl      ( biabl_list        *biabl);
extern char             *beh_getvectname        ( char          *name,
                                                  int            left,
                                                  int            right);
extern int               beh_addothersseltobiabl( biabl_list    *biabl,
                                                  struct chain  *abl);

extern void              beh_delloopvarname( biabl_list         *biabl);
extern char             *beh_getloopvarname( biabl_list         *biabl);
extern void              beh_addloopvarname( biabl_list         *biabl,
                                             char               *varname);

#endif
