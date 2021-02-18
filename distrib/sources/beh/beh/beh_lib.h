
/* ###--------------------------------------------------------------### */
/* file  : beh110.h                                                     */
/* date  : Oct 30 1995                                                  */
/* version : v110                                                       */
/* author(s): Pirouz BAZARGAN SABET                                     */
/*          : Antony PINTO                                              */
/* contents : low level library for behavioural description             */
/* ###--------------------------------------------------------------### */

#ifndef BEH_BEHDEF
#define BEH_BEHDEF

/****************************************************************************/
/*{{{                    include                                            */
/****************************************************************************/
#include MUT_H
#include LOG_H

/*}}}************************************************************************/
/*{{{                    define                                             */
/****************************************************************************/
#define BEH_GENERIC            203698 /* generic ptype code                 */
#define BEH_STABLE               0x01 /* description using STABLE           */

#define BEH_NODE_BDD             0x01 /* fig TYPE : expr. BDD               */
#define BEH_NODE_GEX             0x02 /* fig TYPE : expr. GEX               */
#define BEH_NODE_VARLIST         0x04 /* fig TYPE : list of variables       */
#define BEH_NODE_DERIVATE        0x08 /* fig TYPE : derivatives             */
#define BEH_NODE_USER            0x10 /* fig TYPE : user expression         */
#define BEH_NODE_QUAD            0x20 /* fig TYPE : QUAD structure          */

#define BEH_CHK_DRIVERS    0x00000001 /* check output drivers               */
#define BEH_CHK_EMPTY      0x00000002 /* check for empty architecture       */

#define BEH_ARC_VHDL             0x01 /* VHDL architecture                  */
#define BEH_ARC_C                0x02 /* C    architecture                  */

#define BEH_CND_UNKNOWN    0x00000000 /* cond. precedence is unknown        */
#define BEH_CND_PRECEDE    0x00000001 /* cond. has a precedence             */
#define BEH_CND_NOPRECEDE  0x00000002 /* cond. has no precedence            */
#define BEH_CND_LAST       0x00000080 /* cond. is the else cnd              */
#define BEH_CND_SELECT     0x00000004 /* cond. is vectorial, same           */
#define BEH_CND_SEL_OTHERS 0x00000008 /* for multiple values                */
#define BEH_CND_CMPLX      0x00000010 /* ...................                */
#define BEH_CND_LOOP       0x00000020 /* for loop                           */
#define BEH_CND_WEAK       0x00000100 /* weak assign                        */
#define BEH_CND_STRONG     0x00000200 /* strong assign                      */
#define BEH_CND_PULL       0x00000400 /* pull assign                        */

#define BEH_FIG__TIMEUNIT        0x07 /* time unit                          */
#define BEH_TU__FS                  1 /* time unit : fs                     */
#define BEH_TU__PS                  2 /* time unit : ps                     */
#define BEH_TU__NS                  3 /* time unit : ns                     */
#define BEH_TU__US                  4 /* time unit : us                     */
#define BEH_TU__MS                  5 /* time unit : ms                     */

#define BEH_TYPE_CND   (long) 0xc1c11A20
#define BEH_TYPE_LOOP  (long) 0xc1c20B2F

#define BEH_FLAG_NORMAL             1
#define BEH_FLAG_FORCEBUS           2

//#define BEH_WARMSG_AUX_NOT_EMPTY       300
//#define BEH_WARMSG_BUS_NOT_EMPTY       301
//#define BEH_WARMSG_BUX_NOT_EMPTY       302    
//#define BEH_WARMSG_DER_NOT_EMPTY       303
//#define BEH_WARMSG_FIG_NOT_EMPTY       304
//#define BEH_WARMSG_GEN_NOT_EMPTY       305
//#define BEH_WARMSG_MSG_NOT_EMPTY       306
//#define BEH_WARMSG_OUT_NOT_EMPTY       307
//#define BEH_WARMSG_QUAD_NOT_EMPTY      308
//#define BEH_WARMSG_REG_NOT_EMPTY       309
//#define BEH_WARMSG_BIABL_NOT_EMPTY     310
//#define BEH_WARMSG_BINODE_NOT_EMPTY    311
//#define BEH_WARMSG_BI                  312
//#define BEH_WARMSG_SINGLE              313
//#define BEH_WARMSG_VAUX_NOT_EMPTY      315
//#define BEH_WARMSG_VOUT_NOT_EMPTY      316
//#define BEH_WARMSG_VBUX_NOT_EMPTY      317
//#define BEH_WARMSG_VBUS_NOT_EMPTY      318
//#define BEH_WARMSG_VREG_NOT_EMPTY      319
//#define BEH_WARMSG_VBIABL_NOT_EMPTY    320
//

#ifdef LEFT
#undef LEFT
#endif

#ifdef RIGHT
#undef RIGHT
#endif

/*}}}************************************************************************/
/*{{{                    globals variables                                  */
/****************************************************************************/
extern unsigned int  BEH_LINNUM  ;

extern unsigned char BEH_TIMEUNIT;
extern float         BEH_CNV_FS  ;
extern float         BEH_CNV_PS  ;
extern float         BEH_CNV_NS  ;
extern float         BEH_CNV_US  ;
extern float         BEH_CNV_MS  ;

/*}}}************************************************************************/
/*{{{                    structures                                         */
/****************************************************************************/
typedef struct bedrd              /* dictionary record                      */
{
  struct bedrd      *NEXT       ; /* next record                            */
  char              *KEY        ; /* context key                            */
  short              FD0_VAL    ; /* data field                             */
  short              FD1_VAL    ; /* data field                             */
  short              FD2_VAL    ; /* data field                             */
  short              FD3_VAL    ; /* data field                             */
  short              FD4_VAL    ; /* data field                             */
  short              FD5_VAL    ; /* data field                             */
  short              FD6_VAL    ; /* data field                             */
  long               PNT_VAL    ; /* data field                             */
}
bedrd_list;

typedef struct beder
{
  struct beder      *NEXT       ; /* next derivative                        */
  pNode              BDD        ; /* derivative BDD                         */
  short              INDEX      ; /* index of the variable                  */
}
beder_list;

typedef struct begex
{
  chain_list        *OPERAND    ; /* list of operands or NULL for term      */
  unsigned int       TERM       ; /* operator or terminal's index           */
  unsigned short     TYPE       ; /* expression's type index                */
}
begex_list;

typedef struct beden                      /* dictionary entry point                 */
{
  struct beden      *NEXT       ; /* next entry                             */
  bedrd_list        *DATA       ; /* data                                   */
  char              *KEY        ; /* key                                    */
}
beden_list;

typedef struct beh_loop
{
  struct beh_loop   *NEXT       ;
  char              *INDEX      ;
  int                LEFT       ;
  int                RIGHT      ;
  char              *VARNAME    ;
}
loop_list;

typedef struct biabl              /* couple of ABL                          */
{
  struct biabl      *NEXT       ; /* next couple                            */
  char              *LABEL      ; /* driver's label                         */
  chain_list        *CNDABL     ; /* driver's connection condition (ABL)    */
  chain_list        *VALABL     ; /* equation of driven value (ABL)         */
  loop_list         *LOOP       ; /* loop                                   */
  ptype_list        *USER       ; /* user field                             */
  unsigned int       TIME       ; /* driver's delay                         */
  unsigned int       TIMER       ; /* driver's delay                         */
  unsigned int       TIMEF       ; /* driver's delay                         */
  char              *TIMEVAR    ; /* driver's delay                         */
  long               FLAG       ; /* condition's flag                       */
}
biabl_list;

typedef struct binode             /* couple of BDD                          */
{
  struct binode     *NEXT       ; /* next figure                            */
  pNode              CNDNODE    ; /* driver's connection condition (BDD)    */
  pNode              VALNODE    ; /* equation of driven value (BDD)         */
  unsigned int       TIME       ; /* driver's delay                         */
  char              *TIMEVAR    ; /* driver's delay                         */
}
binode_list;

typedef struct vectbiabl
{
  struct vectbiabl  *NEXT       ;
  int                LEFT       ;
  int                RIGHT      ;
  biabl_list        *BIABL      ;
}
vectbiabl_list;

typedef struct bereg              /* register                               */
{
  struct bereg      *NEXT       ; /* next register                          */
  char              *NAME       ; /* register's name                        */
  biabl_list        *BIABL      ; /* list of drivers (ABL)                  */
  binode_list       *BINODE     ; /* list of drivers (BDD)                  */
  char               FLAGS;
}
bereg_list;

typedef struct bemsg              /* assert instruction                     */
{
  struct bemsg      *NEXT       ; /* next assertion                         */
  char              *LABEL      ; /* instruction's label                    */
  char              *MESSAGE    ; /* assertion's message                    */
  chain_list        *ABL        ; /* assertion's condition (ABL)            */
  pNode              NODE       ; /* assertion's condition (BDD)            */
  char               LEVEL      ; /* severity level of the assertion        */
}
bemsg_list;

typedef struct beout              /* simple output port                     */
{
  struct beout      *NEXT       ; /* next output                            */
  char              *NAME       ; /* port's name                            */
  chain_list        *ABL        ; /* port's equation (ABL)                  */
  pNode              NODE       ; /* port's equation (BDD)                  */
  unsigned int       TIME       ; /* driver's delay                         */
  unsigned int       TIMER      ; /* driver's delay                         */
  unsigned int       TIMEF      ; /* driver's delay                         */
  char              *TIMEVAR    ; /* driver's delay                         */
  long              FLAGS;
}
beout_list;

typedef struct bebus              /* bussed output port                     */
{
  struct bebus      *NEXT       ; /* next bus port                          */
  char              *NAME       ; /* port's name                            */
  biabl_list        *BIABL      ; /* list of drivers (ABL)                  */
  binode_list       *BINODE     ; /* list of drivers (BDD)                  */
  char               TYPE       ; /* port's type : W or M                   */
  char               FLAGS;
}
bebus_list;

typedef struct beaux              /* simple internal signal                 */
{
  struct beaux      *NEXT       ; /* next signal                            */
  char              *NAME       ; /* signal's name                          */
  chain_list        *ABL        ; /* signal's equation (ABL)                */
  pNode              NODE       ; /* signal's equation (BDD)                */
  unsigned int       TIME       ; /* driver's delay                         */
  unsigned int       TIMER      ; /* driver's delay                         */
  unsigned int       TIMEF      ; /* driver's delay                         */
  char              *TIMEVAR    ; /* driver's delay                         */
  long              FLAGS;
}
beaux_list;

typedef struct bebux              /* bussed internal signal                 */
{
  struct bebux      *NEXT       ; /* next signal                            */
  char              *NAME       ; /* signal's name                          */
  biabl_list        *BIABL      ; /* list of drivers (ABL)                  */
  binode_list       *BINODE     ; /* list of drivers (BDD)                  */
  char               TYPE       ; /* signal's type : W or M                 */
  char               FLAGS;
}
bebux_list;

typedef struct bepor              /* port                                   */
{
  struct bepor      *NEXT       ; /* next port                              */
  char              *NAME       ; /* port's name                            */
  char               DIRECTION  ; /* port's mode (I, O, B, Z, T)            */
  char               TYPE       ; /* port's type (B, M or W)                */
}
bepor_list;

typedef struct berin              /* primary inputs                         */
{
  struct berin      *NEXT       ; /* next input                             */
  char              *NAME       ; /* input's name                           */
  int                LEFT       ;
  int                RIGHT      ;
  chain_list        *OUT_REF    ; /* dependent simple output                */
  chain_list        *OUT_VEC    ; /* dependent simple output                */
  chain_list        *BUS_REF    ; /* dependent bussed output                */
  chain_list        *BUS_VEC    ; /* dependent bussed output                */
  chain_list        *AUX_REF    ; /* dependent simple internal signal       */
  chain_list        *AUX_VEC    ; /* dependent simple internal signal       */
  chain_list        *REG_REF    ; /* dependent internal register            */
  chain_list        *REG_VEC    ; /* dependent internal register            */
  chain_list        *BUX_REF    ; /* dependent bussed internal signal       */
  chain_list        *BUX_VEC    ; /* dependent bussed internal signal       */
  chain_list        *MSG_REF    ; /* dependent assertion                    */
  chain_list        *DLY_REF    ; /* dependent delayed internal signal      */
}
berin_list;

typedef struct begen              /* generic                                */
{
  struct begen      *NEXT       ; /* next generic                           */
  char              *NAME       ; /* generic`s name                         */
  char              *TYPE       ; /* generic type`s name                    */
  void              *VALUE      ; /* generic`s value                        */
}
begen_list;

typedef struct bequad
{
  pNode              BDD        ; /* expression using BDD format            */
  begex_list        *GEX        ; /* expression using GEX format            */
  chain_list        *VARLIST    ; /* variable list                          */
  beder_list        *DERIVATE   ; /* list of derivate BDDs                  */
  void              *USER       ; /* user defined form of the expression    */
}
bequad_list;

typedef struct bevectreg          /* register                               */
{
  struct bevectreg  *NEXT       ; /* next register                          */
  char              *NAME       ; /* register's name                        */
  vectbiabl_list    *VECTBIABL  ; /* list of drivers (ABL)                  */
  int                LEFT       ;
  int                RIGHT      ;
  char               FLAGS;
}
bevectreg_list;

typedef struct bevectout          /* simple output port                     */
{
  struct bevectout  *NEXT       ; /* next output                            */
  char              *NAME       ; /* port's name                            */
  chain_list        *ABL        ; /* port's equation (ABL)                  */
  int                LEFT       ;
  int                RIGHT      ;
  unsigned int       TIME       ; /* driver's delay                         */
  char              *TIMEVAR    ; /* driver's delay                         */
  char               FLAGS;
}
bevectout_list;

typedef struct bevectbus          /* bussed output port                     */
{
  struct bevectbus  *NEXT       ; /* next bus port                          */
  char              *NAME       ; /* port's name                            */
  vectbiabl_list    *VECTBIABL  ; /* list of drivers (ABL)                  */
  int                LEFT       ;
  int                RIGHT      ;
  char               TYPE       ; /* port's type (B, M or W)                */
  char               FLAGS;
}
bevectbus_list;

typedef struct bevectaux          /* simple internal signal                 */
{
  struct bevectaux  *NEXT       ; /* next signal                            */
  char              *NAME       ; /* signal's name                          */
  chain_list        *ABL        ; /* signal's equation (ABL)                */
  int                LEFT       ;
  int                RIGHT      ;
  unsigned int       TIME       ; /* driver's delay                         */
  char              *TIMEVAR    ; /* driver's delay                         */
  char               FLAGS;
}
bevectaux_list;

typedef struct bevectbux          /* bussed internal signal                 */
{
  struct bevectbux  *NEXT       ; /* next signal                            */
  char              *NAME       ; /* signal's name                          */
  vectbiabl_list    *VECTBIABL  ; /* list of drivers (ABL)                  */
  int                LEFT       ;
  int                RIGHT      ;
  char               TYPE       ; /* port's type (B, M or W)                */
  char               FLAGS;
}
bevectbux_list;

typedef struct bevectpor          /* port                                   */
{
  struct bevectpor  *NEXT       ; /* next port                              */
  char              *NAME       ; /* port's name                            */
  char               DIRECTION  ; /* port's mode (I, O, B, Z, T)            */
  int                LEFT       ;
  int                RIGHT      ;
  char               TYPE       ; /* port's type (B, M or W)                */
}
bevectpor_list;

typedef struct befig              /* behaviour figure                       */
{
  struct befig      *NEXT       ; /* next figure                            */
  char              *NAME       ; /* figure's name                          */
  bereg_list        *BEREG      ; /* list of internal signals (REGISTER)    */
  bevectreg_list    *BEVECTREG  ; /* list of internal signals (REGISTER)    */
  bemsg_list        *BEMSG      ; /* list of assert instructions            */
  berin_list        *BERIN      ; /* list of inputs (registers and ports)   */
  beout_list        *BEOUT      ; /* list of output ports                   */
  bevectout_list    *BEVECTOUT  ; /* list of output ports                   */
  bebus_list        *BEBUS      ; /* list of output ports (BUS)             */
  bevectbus_list    *BEVECTBUS  ; /* list of output ports (BUS)             */
  beaux_list        *BEAUX      ; /* list of internal signals               */
  bevectaux_list    *BEVECTAUX  ; /* list of internal signals               */
  beaux_list        *BEDLY      ; /* internal signals used for 'DELAYED     */
  bebux_list        *BEBUX      ; /* list of internal signals (BUS)         */
  bevectbux_list    *BEVECTBUX  ; /* list of internal signals (BUS)         */
  bepor_list        *BEPOR      ; /* list of ports in reverse order         */
  bevectpor_list    *BEVECTPOR  ; /* list of ports in reverse order         */
  begen_list        *BEGEN      ; /* list of generic data                   */
  pCircuit           CIRCUI     ; /* Pointer on Circuit for BDDs            */
  ptype_list        *USER       ; /* reserved for user's applications       */
  char               ERRFLG     ; /* error flag                             */
  char               TYPE       ; /* description type                       */
  unsigned char      FLAG       ; /* description flag                       */
  unsigned char      TIME_UNIT  ; /* time unit                              */
}
befig_list;

/*}}}************************************************************************/
/*{{{                    functions                                          */
/****************************************************************************/
beaux_list      *beh_addbeaux           ( beaux_list        *lastbeaux,
                                          char              *name,
                                          chain_list        *abl_expr,
                                          pNode              bdd_expr,
                                          long flags);
bebus_list      *beh_addbebus           ( bebus_list        *lastbebus,
                                          char              *name,
                                          biabl_list        *biabl,
                                          binode_list       *binode,
                                          char               type,
                                          char               flags);
bebux_list      *beh_addbebux           ( bebux_list        *lastbebux,
                                          char              *name,
                                          biabl_list        *biabl,
                                          binode_list       *binode,
                                          char               type,
                                          char               flags);
beder_list      *beh_addbeder           ( beder_list        *last_beder,
                                          pNode              bdd_expr ,
                                          short              index );
befig_list      *beh_addbefig           ( befig_list        *lastbefig,
                                          char              *name );
begen_list      *beh_addbegen           ( begen_list        *lastbegen,
                                          char              *name,
                                          char              *type,
                                          void              *value);
bemsg_list      *beh_addbemsg           ( bemsg_list        *lastbemsg,
                                          char              *label,
                                          char               level,
                                          char              *message,
                                          chain_list        *abl_expr,
                                          pNode              bdd_expr);
beout_list      *beh_addbeout           ( beout_list        *lastbeout,
                                          char              *name,
                                          chain_list        *abl_expr,
                                          pNode              bdd_expr,
                                          long flags);
bepor_list      *beh_addbepor           ( bepor_list        *lastbepor,
                                          char              *name,
                                          char               dir,
                                          char               type);
bequad_list     *beh_addbequad          ( pNode              bdd_expr,
                                          begex_list        *gex_expr,
                                          chain_list        *var_list,
                                          beder_list        *der_list);
bereg_list      *beh_addbereg           ( bereg_list        *lastbereg,
                                          char              *name,
                                          biabl_list        *biabl,
                                          binode_list       *binode,
                                          char               flags);
berin_list      *beh_addberin           ( berin_list        *lastberin,
                                          char              *name);
biabl_list      *beh_addbiabl           ( biabl_list        *lastbiabl,
                                          char              *label,
                                          chain_list        *condition,
                                          chain_list        *value);
binode_list     *beh_addbinode          ( binode_list       *lastbinode,
                                          pNode              condition,
                                          pNode              value);
beaux_list      *beh_delbeaux           ( beaux_list        *listbeaux,
                                          beaux_list        *ptbeaux,
                                          char               mode);
bebus_list      *beh_delbebus           ( bebus_list        *listbebus,
                                          bebus_list        *ptbebus,
                                          char               mode);
bebux_list      *beh_delbebux           ( bebux_list        *listbebux,
                                          bebux_list        *ptbebux,
                                          char               mode);
beder_list      *beh_delbeder           ( beder_list        *listbeder,
                                          beder_list        *ptbeder ,
                                          char               mode );
befig_list      *beh_delbefig           ( befig_list        *listbefig,
                                          befig_list        *ptbefig,
                                          char               mode);
begen_list      *beh_delbegen           ( begen_list        *listbegen,
                                          begen_list        *ptbegen,
                                          char               mode);
bemsg_list      *beh_delbemsg           ( bemsg_list        *listbemsg,
                                          bemsg_list        *ptbemsg,
                                          char               mode);
beout_list      *beh_delbeout           ( beout_list        *listbeout,
                                          beout_list        *ptbeout,
                                          char               mode);
bepor_list      *beh_delbepor           ( bepor_list        *listbepor,
                                          bepor_list        *ptbepor);
void             beh_delbequad          ( bequad_list       *ptbequad,
                                          char               mode);
bereg_list      *beh_delbereg           ( bereg_list        *listbereg,
                                          bereg_list        *ptbereg,
                                          char               mode);
berin_list      *beh_delberin           ( berin_list        *listberin,
                                          berin_list        *ptberin);
biabl_list      *beh_delbiabl           ( biabl_list        *listbiabl,
                                          biabl_list        *ptbiabl,
                                          char               mode);
binode_list     *beh_delbinode          ( binode_list       *listbinode,
                                          binode_list       *ptbinode,
                                          char               mode);
beden_list     **beh_initab             ( void );
void             beh_addtab             ( beden_list       **head,
                                          char              *key_str,
                                          char              *ctx_str,
                                          int                field,
                                          long               valu);
long             beh_chktab             ( beden_list       **head ,
                                          char              *key_str,
                                          char              *ctx_str,
                                          int                field );
void             beh_fretab             ( beden_list       **pt_hash);
void             beh_biterize           ( befig_list        *befig);
befig_list      *beh_duplicate          ( befig_list        *befig);
biabl_list      *beh_dupbiabl           ( biabl_list        *biabl);
biabl_list      *beh_explodeBiabl       ( biabl_list        *biabl,
                                          int                index,
                                          char              *name,
                                          int                left,
                                          int                right,
                                          void             (*func)(char*,
                                                                   int,
                                                                   biabl_list*));
void             beh_explodeLoop        ( loop_list         *loop,
                                          biabl_list        *biabl,
                                          char              *name,
                                          void             (*func)(char*,
                                                                   int,
                                                                   biabl_list*));
unsigned int     beh_error              ( int                code,
                                          char              *str1);
unsigned int     beh_warning            ( int                code,
                                          char              *str1,
                                          char              *str2);
void             beh_frebeaux           ( beaux_list        *listbeaux);
void             beh_frebebus           ( bebus_list        *listbebus);
void             beh_frebebux           ( bebux_list        *listbebux);
void             beh_frebeder           ( beder_list        *listbeder);
void             beh_frebefig           ( befig_list        *listbefig);
void             beh_frebegen           ( begen_list        *listbegen);
void             beh_frebemsg           ( bemsg_list        *listbemsg);
void             beh_frebeout           ( beout_list        *listbeout);
void             beh_frebepor           ( bepor_list        *listbepor);
void             beh_frebereg           ( bereg_list        *listbereg);
void             beh_freberin           ( berin_list        *listberin);
void             beh_frebiabl           ( biabl_list        *listbiabl);
void             beh_frebinode          ( binode_list       *listbinode);
void            *beh_getgenval          ( begen_list        *listbegen,
                                          char              *begenname);
void             beh_message            ( int                code,
                                          char              *str1);
beaux_list      *beh_rmvbeaux           ( beaux_list        *listbeaux,
                                          char              *beauxname,
                                          char               mode);
bebus_list      *beh_rmvbebus           ( bebus_list        *listbebus,
                                          char              *bebusname,
                                          char               mode);
bebux_list      *beh_rmvbebux           ( bebux_list        *listbebux,
                                          char              *bebuxname,
                                          char               mode);
beder_list      *beh_rmvbeder           ( beder_list        *listbeder,
                                          short              bederindex);
befig_list      *beh_rmvbefig           ( befig_list        *listbefig,
                                          char              *befigname,
                                          char               mode);
begen_list      *beh_rmvbegen           ( begen_list        *listbegen,
                                          char              *begenname,
                                          char               mode);
bemsg_list      *beh_rmvbemsg           ( bemsg_list        *listbemsg,
                                          char              *bemsglabl,
                                          char               mode);
beout_list      *beh_rmvbeout           ( beout_list        *listbeout,
                                          char              *beoutname,
                                          char               mode);
bepor_list      *beh_rmvbepor           ( bepor_list        *listbepor,
                                          char              *beporname);
bereg_list      *beh_rmvbereg           ( bereg_list        *listbereg,
                                          char              *beregname,
                                          char               mode);
berin_list      *beh_rmvberin           ( berin_list        *listberin,
                                          char              *berinname);
void            beh_toolbug             ( int                code,
                                          char              *str1,
                                          char              *str2,
                                          int                nbr1);
bevectaux_list  *beh_addbevectaux       ( bevectaux_list    *lastbevectaux,
                                          char              *name,
                                          chain_list        *abl,
                                          int                left,
                                          int                right,
                                          long flags);
bevectout_list  *beh_addbevectout       ( bevectout_list    *lastbevectout,
                                          char              *name,
                                          chain_list        *abl,
                                          int                left,
                                          int                right,
                                          long flags);
bevectpor_list  *beh_addbevectpor       ( bevectpor_list    *lastbevectpor,
                                          char              *name,
                                          char               dir,
                                          char               type,
                                          int                left,
                                          int                right);
bevectbux_list  *beh_addbevectbux       ( bevectbux_list    *lastbevectbux,
                                          char              *name,
                                          vectbiabl_list    *vectbiabl,
                                          int                left,
                                          int                right,
                                          char               type,
                                          char               flags);
bevectbus_list  *beh_addbevectbus       ( bevectbus_list    *lastbevectbus,
                                          char              *name,
                                          vectbiabl_list    *vectbiabl,
                                          int                left,
                                          int                right,
                                          char               type,
                                          char               flags);
bevectreg_list  *beh_addbevectreg       ( bevectreg_list    *lastbevectreg,
                                          char              *name,
                                          vectbiabl_list    *vectbiabl,
                                          int                left,
                                          int                right,
                                          char               flags);
bevectaux_list  *beh_delbevectaux       ( bevectaux_list    *listbevectaux,
                                          bevectaux_list    *ptbevectaux,
                                          char               mode);
bevectout_list  *beh_delbevectout       ( bevectout_list    *listbevectout,
                                          bevectout_list    *ptbevectout,
                                          char               mode);
bevectpor_list  *beh_delbevectpor       ( bevectpor_list    *listbevectpor,
                                          bevectpor_list    *ptbevectpor);
bevectbux_list  *beh_delbevectbux       ( bevectbux_list    *listbevectbux,
                                          bevectbux_list    *ptbevectbux,
                                          char               mode);
bevectbus_list  *beh_delbevectbus       ( bevectbus_list    *listbevectbus,
                                          bevectbus_list    *ptbevectbus,
                                          char               mode);
bevectreg_list  *beh_delbevectreg       ( bevectreg_list    *listbevectreg,
                                          bevectreg_list    *ptbevectreg,
                                          char               mode);
void             beh_frebevectaux       ( bevectaux_list    *listbevectaux);
void             beh_frebevectout       ( bevectout_list    *listbevectout);
void             beh_frebevectpor       ( bevectpor_list    *listbevectpor);
void             beh_frebevectbux       ( bevectbux_list    *listbevectbux);
void             beh_frebevectbus       ( bevectbus_list    *listbevectbus);
void             beh_frebevectreg       ( bevectreg_list    *listbevectreg);
vectbiabl_list  *beh_addvectbiabl       ( vectbiabl_list    *lastvectbiabl,
                                          int                left,
                                          int                right,
                                          biabl_list        *biabl);
biabl_list      *beh_copybiabl          ( biabl_list        *biabl);
vectbiabl_list  *beh_delvectbiabl       ( vectbiabl_list    *listvectbiabl,
                                          vectbiabl_list    *ptvectbiabl,
                                          char               mode);
void             beh_invertvectbiabl    ( vectbiabl_list    *vbiabl);
void             beh_invertbiabl        ( biabl_list        *biabl);
void             beh_frevectbiabl       ( vectbiabl_list    *listvectbiabl);
void             beh_addcndvect2biabl   ( biabl_list        *biabl,
                                          char              *cndVect);
void             beh_addcnddefault2biabl( biabl_list        *biabl);
char            *beh_getcndvectfbiabl   ( biabl_list        *biabl);
void             beh_delcndvectfbiabl   ( biabl_list        *biabl);
int              beh_simplifybiabl      ( biabl_list        *biabl);
int              beh_addothersseltobiabl( biabl_list        *biabl,
                                          chain_list        *abl);
char            *beh_buildvhdvectname   ( char              *name,
                                          int                left,
                                          int                right);
char            *beh_buildvlgvectname   ( char              *name,
                                          int                left,
                                          int                right);
loop_list       *beh_newloop            ( loop_list         *next,
                                          char              *varname,
                                          char              *index,
                                          int                begin,
                                          int                end);
int              beh_isSameLoop         ( loop_list         *loop1,
                                          loop_list         *loop2);
int              beh_isloop             ( biabl_list        *biabl);
char            *beh_addloop            ( biabl_list        *biabl,
                                          char              *index,
                                          char              *varname,
                                          int                begin,
                                          int                end);
loop_list       *beh_getloop            ( biabl_list        *biabl);
void             beh_delloop            ( biabl_list        *biabl);

void beh_createName(ht *signalht, char*(*func)(char*,chain_list*));
void beh_changeNameInside(ht *signalht, befig_list *fig);
void beh_changeNameInterface(ht *signalht, befig_list *fig);
void beh_addRefName(ht **signalht, befig_list *fig);


extern ht *BEH_TIMEVARS;

/*}}}************************************************************************/

#endif
