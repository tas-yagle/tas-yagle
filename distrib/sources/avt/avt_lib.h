/****************************************************************************/
/*                                                                          */
/*                      Chaine de verification                              */
/*                                                                          */
/*    Produit : AVT Version 1                                               */
/*    Fichier : avt100.h                                                    */
/*                                                                          */
/*    (c) copyright 1998-1999 AVERTEC                                       */
/*    Tous droits reserves                                                  */
/*    Support : e-mail Karim.Dioury@asim.lip6.fr                            */
/*                                                                          */
/*    Auteur(s) : DIOURY Karim                                              */
/*                                                                          */
/****************************************************************************/

#ifndef AVT
#define AVT

#include <stdarg.h>
#include <stdio.h>
#include MUT_H

#ifndef GRAB_IT
# define GRAB_IT
#endif

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif


#define AVT_ERR  1
#define AVT_WAR  2
#define AVT_INFO 3
#define AVT_INIT_KEY {21,34,241,66,171,27,33,65,161,204,23,128,19,14,199,'\0'}
#define AVT_INIT_SEED (long)6970

extern int   AVT_COL;
extern char *AVERTEC_VERSION ;
extern char *AVT_FULLVERSION ;
extern char *AVT_SUPER_TOKEN_MATCH[];
extern char *AVT_PRODUCT_TOKEN_MATCH[];
extern char **AVTENV;
extern int   AVT_VALID_TOKEN;

extern char *avt_tokenmatch __P((char *));
extern char *avt_supertokenmatch __P((char *));

extern void  avt_sethashvar (char *var, char *val);
void avt_sethashvar_sub (char *var, char *value, int warn, int set);

extern char *avt_gethashvar (char *var);
char *avt_gethashvar_sub (char *var, int intable);

extern void avt_banner    __P((char *tool,
                               char *comment,
                               char *date
                             ));
extern int avt_givetoken  __P((char *,
                               char *
                             ));
extern int avt_deltoken   __P((char *,
                               char *
                             ));
extern int avt_inftool    __P((char *,
                               char *
                             ));

extern char *avt_getenv   __P((char *)) ;
extern void avtenv        __P(()) ;
extern void avt_setvar    __P((char *,char *));
extern void avt_trace     __P((int level, FILE *output, char *fmt, ...));
extern void avt_trace_va  __P((int level, FILE *output, char *fmt, va_list pa));
extern void avt_date      __P((char *date));
extern void avt_error     __P((char *lib, int code, int severity, char *fmt, ...));

// printf a colored printf, use '¤<#>" eg. toto = ¤3value
// value will be colored with AVT_COLOR_3
extern void avt_fprintf(FILE *output, char *fmt, ...);
int avt_text_real_length(char *buf);
void avt_format_text(char *resbuf, char *origbuf, int decal, int max);
extern void avt_back_fprintf(FILE *output, int length);
extern int avt_terminal(FILE *output) ;

// print like the avtbanner the tool info
// take comment flag on and comment flag off
// exemple : avt_printExecInfo(stdout, "/*", "test/n" "*/");
extern void avt_printExecInfo(FILE *fd, char *flagcomon, char *txt, char *flagcomoff);

// this one could generate first and last header line with a flourish style
extern void avt_printExecInfoFlourish(FILE *fd, char *fon, char *txt, char *foff);


// this one allow to use custom print function
extern void avt_printExecInfoCustom(void *file, char *fon, char *txt, char *foff, void (*prn)(void *, ...));

extern void avt_TrapSegV ();
extern void avt_UnTrapSegV ();
extern void avt_TrapFPE ();
extern void avt_UnTrapFPE ();
extern void avt_TrapKill ();
GRAB_IT extern void avt_PushSegVExit (void (*fexit)(int), int code);
GRAB_IT extern void avt_PopSegVExit ();
GRAB_IT extern void avt_PushFPEExit (void (*fexit)(int), int code);
GRAB_IT extern void avt_PopFPEExit ();
GRAB_IT extern void avt_PushKillExit (void (*fexit)(int), int code);
GRAB_IT extern void avt_PopKillExit ();
GRAB_IT extern void avt_PushSegVMessage (char *message);
GRAB_IT extern void avt_PopSegVMessage ();
GRAB_IT extern void avt_PushFPEMessage (char *message);
GRAB_IT extern void avt_PopFPEMessage ();
GRAB_IT extern void avt_PushKillMessage (char *message);
GRAB_IT extern void avt_PopKillMessage ();
    
extern int   avt_is_default_technoname ( char *name );
extern char *avt_getusername(char *buf);


/****************************************************************************/
/*  ERROR FUNCTIONS AND MESSAGES                                            */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#define API_ERRMSG ((long)1)
#define AVT_ERRMSG ((long)2)
#define BCK_ERRMSG ((long)3)
#define BEF_ERRMSG ((long)4)
#define BEG_ERRMSG ((long)5)
#define BEH_ERRMSG ((long)6)
#define BGL_ERRMSG ((long)7)
#define BHL_ERRMSG ((long)8)
#define BVL_ERRMSG ((long)9)
#define CBH_ERRMSG ((long)10)
#define CGV_ERRMSG ((long)11)
#define CNS_ERRMSG ((long)12)
#define EFG_ERRMSG ((long)13)
#define ELP_ERRMSG ((long)14)
#define EQT_ERRMSG ((long)15)
#define FCL_ERRMSG ((long)16)
#define GNS_ERRMSG ((long)17)
#define GSP_ERRMSG ((long)18)
#define INF_ERRMSG ((long)19)
#define LIB_ERRMSG ((long)20)
#define MAL_ERRMSG ((long)21)
#define MCC_ERRMSG ((long)22)
#define MCL_ERRMSG ((long)23)
#define MCP_ERRMSG ((long)24)
#define MEL_ERRMSG ((long)25)
#define MGL_ERRMSG ((long)26)
#define MHL_ERRMSG ((long)27)
#define MLO_ERRMSG ((long)28)
#define MLU_ERRMSG ((long)29)
#define MPH_ERRMSG ((long)30)
#define MPU_ERRMSG ((long)31)
#define MSL_ERRMSG ((long)32)
#define MUT_ERRMSG ((long)33)
#define MVL_ERRMSG ((long)34)
#define RCN_ERRMSG ((long)35)
#define SDC_ERRMSG ((long)36)
#define SIM_ERRMSG ((long)37)
#define SLB_ERRMSG ((long)38)
#define SPE_ERRMSG ((long)39)
#define SPF_ERRMSG ((long)40)
#define STB_ERRMSG ((long)41)
#define STM_ERRMSG ((long)42)
#define TAS_ERRMSG ((long)43)
#define TBG_ERRMSG ((long)44)
#define TLC_ERRMSG ((long)45)
#define TLF_ERRMSG ((long)46)
#define TMA_ERRMSG ((long)47)
#define TRC_ERRMSG ((long)48)
#define TTV_ERRMSG ((long)49)
#define TUT_ERRMSG ((long)50)
#define VCD_ERRMSG ((long)51)
#define XAL_ERRMSG ((long)52)
#define XYA_ERRMSG ((long)53)
#define YAG_ERRMSG ((long)54)
#define ZEN_ERRMSG ((long)55)
#define AVTWIG_ERRMSG ((long)56)
#define LOG_ERRMSG ((long)57)
#define MBK_ERRMSG ((long)58)
#define SPI_ERRMSG ((long)59)
#define AVT_API_ERRMSG ((long)60)
#define GNS_API_ERRMSG ((long)61)
#define INF_API_ERRMSG ((long)62)
#define TMA_API_ERRMSG ((long)63)
#define TTV_API_ERRMSG ((long)64)
#define YAG_API_ERRMSG ((long)65)
#define TAS_API_ERRMSG ((long)66)
#define SIM_API_ERRMSG ((long)67)
#define HPE_API_ERRMSG ((long)68)
#define FCL_API_ERRMSG ((long)69)
#define CTK_API_ERRMSG ((long)70)
#define BEG_API_ERRMSG ((long)71)
#define STB_API_ERRMSG ((long)72)
#define CNS_API_ERRMSG ((long)73)
#define STM_API_ERRMSG ((long)74)

//struct msgtab *errmsg_tab;

#define AVT_ERROR 1
#define AVT_WARNING 2
#define AVT_FATAL 3
#define AVT_INTERNAL 0

extern char *AVT_BINARY_NAME;

void avt_initerrmsg( char *);
char *avt_geterrmsg( long , char * );
void avt_set_encrypted_mode(int mode);
void avt_errmsg(long , char *, int , ... );

struct msgtab {
    long    type;
    char   **data;
    char   *lib;
} ;


#ifdef Solaris
extern char *strtok_r(char *s1, const char *s2, char **lasts);
#endif

void avt_LogConfig();

#define LOGMCC           0
#define LOGTRC           1
#define LOGYAG           2
#define LOGMBK           3
#define LOGFILEACCESS    4
#define LOGMBKCACHE      5
#define LOGRCN           6
#define LOGEFG           7
#define LOGGSP           8
#define LOGLOG           9
#define LOGTLF           10
#define LOGLIB           11
#define LOGZEN           12
#define LOGSTATPRS       13
#define LOGERROR         14
#define LOGTMA           15
#define LOGCTK           16
#define LOGTAS           17
#define LOGSTM           18
#define LOGFALSEPATH     19
#define LOGGNS           20
#define LOGBEG           21
#define LOGCONFIG        22
#define LOGSTABILITY     23
#define LOGDEBUG         24
#define LOGMBKCOM        25
#define LOGSPI           26

#define LOGMAX           27


#define AVT_VAR_SET_BY_ENV       0x1
#define AVT_VAR_SET_BY_TOOLSCONF 0x2
#define AVT_VAR_SET_BY_TCLCONFIG 0x4

//======================================================================
//
// Tables Definition
//
//======================================================================

//----------------------------------------------------------------------
// Tables
//----------------------------------------------------------------------


typedef struct V_BOOL {
    char *VARENV;
    int   INDEX;
    int   VALUE;
    int   DOC;
    int   SET;
    char *VAR;
    char *VAR2;
    int  (*INIT_CHECK_FUNC)(char*var, char *val,int *result);
} V_BOOL;

typedef struct V_STR {
    char *VARENV;
    int   INDEX;
    char *VALUE;
    int   DOC;
    int   SET;
    char *VAR;
    char *VAR2;
    int  (*INIT_CHECK_FUNC)(char*var, char *val, char *result);
} V_STR;

typedef struct V_FLOAT {
    char *VARENV;
    int   INDEX;
    float VALUE;
    int   DOC;
    int   SET;
    char *VAR;
    char *VAR2;
    int  (*INIT_CHECK_FUNC)(char *var, char *val, float *result);
    char  STRBUF[15];
} V_FLOAT;

typedef struct V_INT {
    char *VARENV;
    int   INDEX;
    int   VALUE;
    int   DOC;
    int   SET;
    char *VAR;
    char *VAR2;
    int  (*INIT_CHECK_FUNC)(char *var, char *val, int *result);
    char *(*ENUMSTR_FUNC)(int val);
    char  STRBUF[15];
} V_INT;

extern V_BOOL V_BOOL_TAB[];
extern V_STR V_STR_TAB[];
extern V_INT V_INT_TAB[];
extern V_FLOAT V_FLOAT_TAB[];

//----------------------------------------------------------------------
// BOOLEAN
//----------------------------------------------------------------------

enum V_BOOL_LIST {
    __ANNOT_KEEP_M,                     // avtAnnotationKeepM 
    __ANNOT_KEEP_X,                     // avtAnnotationKeepX 
    __API_DRIVE_CORRESP,                // apiDriveCorrespondenceTable 
    __TUT_CALC_DERATE,                  // avtCalcDerating 
    __MCC_RESIZE_RANGE,                 // avtCharacAllowRangeMargin 
    __MCC_CALC_VT,                      // avtCharacCalcVth 
    __MCC_OPTIM_MODE,                   // avtCharacFastMode 
    __MCC_EQUATE_RANGE,                 // avtCharacForEachSize 
    __AVT_COLOR,                        // avtColor 
    __AVT_ENABLE_CORE,                  // avtEnableCore 
    __AVT_TRACE_FATAL,                  // avtTraceFatalError 
    __ELP_DEDUCE_DIFFSIZE,              // avtDeduceDiffusionSize 
    __DSPF_PRESERVE_RC,                 // avtDspfPreserveExistingParasitics  
    __MBK_DUP_PNAME_FOR_FLAT,           // avtDuppNameForFlat 
    __ELP_DRV_FILE,                     // avtElpDriveFile 
    __MBK_FAST_MODE,                    // avtFastMode 
    __MBK_FLATTEN_FOR_PARA,             // avtFlattenForParasitic 
    __MBK_KEEP_ALL_SIGNAL_NAMES,        // avtFlattenKeepsAllSignalNames 
    __AVT_FLEX_LICENSE,                 // avtFlexLicense 
    __AVT_ALL_TRANS_SUPPLIES,           // avtTraceAllTrsSupplies
    __MBK_SPI_NAMENODES,                // avtSpiNameNodes 
    __MBK_SPI_AUTO_LOAD,                // avtSpiLoadCellsFromLib 
    __MBK_SPI_NOBULK,                   // avtSpiNoBulk 
    __MBK_SPI_MERGE,                    // avtSpiMergeConnector 
    __MBK_SPI_MERGE_DIODES,             // avtSpiMergeDiodes 
    __SPI_DRIVE_PARASITICS,             // avtSpiDriveParasitics 
    __MBK_SPI_CREATE_TOP_FIGURE,        // avtSpiCreateTopFigure 
    __MBK_LOAD_PARASITICS,              // avtLoadParasitics 
    __MBK_SPI_DRV_TRSPARAM,             // avtSpiDriveTrsInstanceParams 
    __SPI_DSPF_BUILD_POWER,             // avtSpiDspfBuildPower 
    __SPI_DSPF_LINK_EXTERNAL,           // avtSpiDspfLinkExternal 
    __MBK_HANDLE_GLOBAL_NODES,          // avtSpiHandleGlobalNodes 
    __MBK_SPI_IGNORE_CAPA,              // avtSpiIgnoreCapa 
    __MBK_SPI_IGNORE_DIODES,            // avtSpiIgnoreDiode 
    __MBK_SPI_IGNORE_MODELS,            // avtSpiIgnoreModel 
    __MBK_SPI_IGNORE_RESI,              // avtSpiIgnoreResistance 
    __MBK_SPI_IGNORE_VOLTAGE,           // avtSpiIgnoreVoltage 
    __MBK_SPI_IGNORE_CRYPT,             // avtSpiIgnoreCrypt
    __MBK_SPI_JFET_RESI,                // avtSpiJFETisResistance 
    __MBK_SPI_NO_SIG_PREFIX,            // avtSpiNoSigPrefix 
    __MBK_SPI_ONE_NODE_NORC,            // avtSpiOneNodeNoRc 
    __SPI_ORDER_PIN_POWER,              // avtSpiOrderPinPower 
    __SPI_PIN_DSPF_ORDER,               // avtSpiPinDspfOrder 
    __MGL_PARSE_ESC_VECTORS,            // avtStructuralVerilogVectors 
    __MBK_SPI_INSTANCE_MULTINODE,       // avtSpiInstanceMultiNode 
    __MBK_SPI_ZEROVOLT_RESI,            // avtSpiShortCircuitZeroVolts 
    __MVL_CONFIGURE,                    // avtStructuralVhdlConfigure 
    __ELP_GEN_PARAM,                    // avtUseOnlySpecifiedElpFile 
    __MGL_USE_LIBRARY,                  // avtUseVerilogLibrary 
    __CNS_DRIVE_VERBOSE,                // avtVerboseConeFile 
    __INF_CONF_CHECK,                   // avtVerifyInformationFile 
    __CNS_DRIVE_NORMAL,                 // avtNormalConeFile 
    __FCL_USECUT,                       // fclCutMatchedTransistors 
    __FCL_FILE,                         // fclWriteReport 
    __GEN_KEEP_ALL,                     // gnsKeepAllCells 
    __STB_CTK,                          // stbCrosstalkMode 
    __RCX_USE_MATRIX_LOAD,              // rcxUseMatrixForLoad 
    __SIM_OVR_FILES,                    // simAllowOverwriteFile 
    __RCX_FASTMODE,                     // rcxFastMode 
    __SIM_REMOVE_FILES,                 // simRemoveFiles 
    __EFG_CORRESP_ALIAS,                // simDriveAliasCorrespondance 
    __SIM_USE_MEAS,                     // simUseMeasure 
    __SIM_USE_PRINT,                    // simUsePrint 
    __STB_CTK_FASTMODE,                 // stbCtkFastMode 
    __STB_CTK_LINE,                     // stbCtkLineMode 
    __STB_CTK_NOINFO_ACTIF,             // stbCtkNoInfoActif 
    __STB_CTK_OBSERVABLE,               // stbCtkObservableMode 
    __STB_CTK_REPORT,                   // stbCtkReportFile 
    __STB_CTK_WORST,                    // stbCtkWorstBeginCondition 
    __STB_DET_ANALYSIS,                 // stbDetailedAnalysis 
    __STB_DET_GRAPH,                    // stbDetailedGraph 
    __STB_MULTIPLE_COMMAND,             // stbMultipleMemoryCommand 
    __STB_STABILITY_HOLD,               // stbHoldOnly 
    __STB_OUT_FILE,                     // stbOutFile 
    __STB_REPORT_FILE,                  // stbReportFile 
    __STB_SAVE_STE,                     // stbSaveErrors 
    __STB_STABILITY_SETUP,              // stbSetupOnly 
    __STB_SILENT,                       // stbSilentMode 
    __STB_STABILITY_LAST,               // stbTopLevelPath 
    __STB_TRACE_MODE,                   // stbTraceMode 
    __STB_STABILITY_WORST,              // stbWorstCaseAnalysis 
    __STM_SHARE_MODEL,                  // stmShareModel 
    __TAS_BREAKLOOP,                    // tasBreakLoops 
    __TAS_CALCRCN,                      // tasCalcRCDelays 
    __TAS_DELAY_PROP,                   // tasDelayPropagation 
    __TAS_INT_END,                      // tasExitAfterDetailTimingFile 
    __TAS_FLATCELLS,                    // tasFlatcells 
    __TAS_CNS_FILE,                     // tasGenerateConeFile 
    __TAS_PERFINT,                      // tasGenerateDetailTimingFile 
    __TAS_HIER,                         // tasHierarchicalMode 
    __TAS_IGNBLACKB,                    // tasIgnoreBlackbox 
    __TAS_CARAC_MEMORY,                 // tasMemoryCharacterization 
    __TAS_MERGERCN,                     // tasMergeRCAndGateDelays 
    __TAS_FIND_MIN,                     // tasMinimumPathAnalysis 
    __TMA_TTXIN,                        // tmaTtxInput 
    __TMA_DTXIN,                        // tmaDtxInput  
    __TAS_NO_PROP,                      // tasNoSlopePropagation 
    __TAS_FACTORISE,                    // tasPathFactorisation 
    __TAS_SAVE_BEFIG,                   // tasBefig 
    __TAS_PRES_CON_DIR,                 // tasPreserveConnectorsDirection 
    __TAS_SHORT_MODELNAME,              // tasShortNamesForModels 
    __TAS_SILENT_MODE,                  // tasSilentMode 
    __TAS_SLOFILE,                      // tasSlopeFile 
    __TAS_SIMU_CONE,                    // tasSpiceSimulation 
    __TAS_SUPBLACKB,                    // tasTreatBlackboxHierarchically 
    __TAS_DIF_LATCH,                    // tasTreatDifferentialLatches 
    __TAS_TREATPRECH,                   // tasTreatPrecharge 
    __STM_USE_MSC,                      // tasUseMultipleSlopeConstant 
    __TAS_STABILITY,                    // tasWithStability 
    __TMA_CTXIN,                        // tmaCtxInput 
    __TMA_DRIVECAPAOUT,                 // tmaDriveCapaout 
    __TMA_VERBOSE,                      // tmaVerboseMode 
    __XTAS_TEXT_DISPLAY,                // xtasTextualDisplay 
    __CGV_MAKE_CELLS,                   // xyagMakeCells 
    __YAGLE_BEH_ASSUME_PRECEDE,         // yagleAssumeExpressionPrecedence 
    __YAGLE_LOOP_ANALYSIS,              // yagleAutomaticLatchDetection 
    __YAGLE_BLEEDER_PRECHARGE,          // yagleBleederIsPrecharge 
    __YAGLE_BLOCK_BIDIR,                // yagleBlockBidirectional 
    __YAGLE_BUS_ANALYSIS,               // yagleBusAnalysis 
    __YAGLE_LEVELHOLD_ANALYSIS,         // yagLevelHoldAnalysis 
    __YAGLE_COMPACT_BEHAVIOUR,          // yagleCompactBehavior 
    __YAGLE_CAPACITANCE_CONES,          // yagleCapacitanceCones 
    __YAGLE_DIODE_TRANS,                // yagleTestTransistorDiodes 
    __YAGLE_DETECT_PRECHARGE,           // yagleDetectPrecharge 
    __YAGLE_ELP,                        // yagleElpCorrection 
    __YAGLE_MAKE_CELLS,                 // yagleFlipFlopDetection 
    __YAGLE_VBE,                        // yagleGenerateBehavior 
    __YAGLE_FILE,                       // yagleGenerateConeFile 
    __YAGLE_CONE_NETLIST,               // yagleGenerateConeNetList 
    __YAGLE_GEN_SIGNATURE,              // yagleGenSignature 
    __YAGLE_HIERARCHICAL_MODE,          // yagleHierarchicalMode 
    __YAGLE_PROP_HZ,                    // yagleHzAnalysis 
    __YAGLE_BLACKBOX_IGNORE,            // yagleIgnoreBlackboxes 
    __YAGLE_CELL_SHARE,                 // yagleIntersectingFlipFlops 
    __YAGLE_KEEP_REDUNDANT,             // yagleKeepRedundantBranches 
    __YAGLE_LATCH_REQUIRE_CLOCK,        // yagleLatchesRequireClocks 
    __YAGLE_MARK_TRISTATE_MEMORY,       // yagleMarkTristateMemory 
    __YAGLE_AUTO_FLIPFLOP,              // yagleAutomaticFlipFlopDetection 
    __YAGLE_MINIMISE_CONES,             // yagleMinimizeCones 
    __YAGLE_MINIMISE_INVERTORS,         // yagleMinimizeInvertors 
    __YAGLE_NOTSTRICT,                  // yagleNotStrict 
    __YAGLE_ONE_SUPPLY,                 // yagleOneSupply 
    __YAGLE_NO_SUPPLY,                  // yagleNoSupply 
    __YAGLE_RELAX_ALGO,                 // yagleRelaxationAnalysis 
    __YAGLE_NORC,                       // yagleRemoveInterconnects 
    __YAGLE_REMOVE_PARATRANS,           // yagleRemoveParallelTrans 
    __YAGLE_LOOP_MODE,                  // yagleSearchLoops 
    __YAGLE_SILENT_MODE,                // yagleSilentMode 
    __YAGLE_ORIENT,                     // yagleSimpleOrientation 
    __YAGLE_SIMPLIFY_EXPRESSIONS,       // yagleSimplifyExpressions 
    __YAGLE_SIMPLIFY_PROCESSES,         // yagleSimplifyProcesses 
    __YAGLE_DETECT_LATCHES,             // yagleStandardLatchDetection 
    __YAGLE_STRICT_CKLATCH,             // yagleStrictCkLatch 
    __YAGLE_BLACKBOX_SUPPRESS,          // yagleSuppressBlackboxes 
    __YAGLE_TRISTATE_MEMORY,            // yagleTristateIsMemory 
    __YAGLE_FCL_DETECT,                 // yagleUseFcl 
    __YAGLE_GENIUS,                     // yagleUseGenius 
    __YAGLE_HELP_S,                     // yagleUseNameOrientation 
    __YAGLE_USE_CONNECTOR_DIRECTION,    // yagUseConnectorDirection
    __YAGLE_ONLY_FCL,                   // yagleUseOnlyFcl 
    __YAGLE_ONLY_GENIUS,                // yagleUseOnlyGenius 
    __YAGLE_USESTMSOLVER,               // yagleUseStmSolver 
    __YAGLE_INTERFACE_VECTORS,          // yagleVectorizeInterface 
    __YAGLE_STAT_MODE,                  // yagleWriteStatistics                      
    __YAGLE_GUESS_MUTEX,                // yagMutexHelp                      
    __YAGLE_DETECT_REDUNDANT,           // yagDetectRedundant
    __YAGLE_MEMSYM_HEURISTIC,           // yagMemsymHeuristic
    __YAGLE_ASYNC_COMMAND,              // yagAsyncIsCommand
    __YAGLE_STUCK_LATCH,                // yagStuckLatch
    __YAGLE_ANALYSE_STUCK,              // yagStuckAnalysis
    __YAGLE_DELAYED_RS,                 // yagDetectDelayedRS
    __YAGLE_HIER_LOTRS_GROUP,           // yagHierarchyGroupTransistors
    __VERILOG_KEEP_NAMES,               // avtVerilogKeepNames
    __STB_SHOWME,                       // stbHelpForSetup
    __STB_ENBALE_COMMAND_CHECK,         // stbEnableCommandCheck
    __AVT_NEW_SWITCH_MODEL,             // avtNewSwitchModel
    __AVT_SYMETRICAL_SWITCH,            // avtSymetricalSwitch
    __AVT_USEHT_V2,                     // avtUseHtV2
    __TAS_RECOMPUTE_DELAYS,             // tasRefineDelays
    __MBK_FILTER_MASK_ERROR,            // avtFilterMaskError
    __MCC_ALLOW_NEGATIVE_B,             // avtAllowNegativeB
    __MCC_ALLOW_NEGATIVE_B_BRANCH,      // avtAllowNegativeB
    __STM_RDRIVER_FROM_SLOPE,
    __STM_CEQRSAT_NUMERIC,
    __MCC_CALCUL_VT_NEW,
    __STM_ENABLE_DV,
    __STM_NEW_THRESHOLD,
    __STM_PRECISION_WARNING,
    __MCC_ACCURATE_RLIN_THRESHOLD,
    __AVT_BUG_RDRIVER,
    __AVT_RDRIVER_UNDER_VSAT,
    __STM_NUMERICAL_INTEGRATION,
    __STM_QSAT_ENHANCED,
    __STM_CHECK_SOLUTION,
    __AVT_CTK_ON_INPUT,                 // avtCtkModifyInputSlope
    __AVT_NEW_CTK_ON_INPUT,             // avtCtkModifyInputSlope
    __AVT_CTK_ON_INPUT_NET,
    __AVT_CTK_DEBUG,
    __TMA_CHARAC_PRECISION,             // tmaCharacPrecision
    __STB_STABILITY_CORRECTION,
    __STB_SETUP_HOLD_UPDATE,
    __LIB_DRIVE_COMB_AS_ACCESS,
    __TTV_MAX_PATH_PERIOD_PRECHARGE,    // avtTransparentPrecharge
    __AVT_BUG_RSAT,                     // some minor bugs in rsat/rlin/krs/krt determination
    __AVT_CAPAO_FOR_CTK,                // use better capacitance to determine crosstalk impact from output to input
    __AVT_PRECISE_PILOAD,
    __TAS_PROPAGATE_RC_ACTIVE,
    __AVT_RC_BY_TRANSITION,
    __AVT_NUMSOL_FOR_PILOAD,
    __AVT_HACK_NETLIST,
    __AVT_CAPAI_KF_RC,
    __TAS_USE_FINAL_CAPACITANCE,
    __TAS_ENHANCED_CAPAI,
    __STM_QINIT_FOR_DT_CONF,
    __STM_QINIT_FOR_OVERSHOOT_CONF,
    __TAS_USE_KF,
    __STM_ENHANCED_CAPAEQ_FOR_PILOAD,
    __STM_PILOAD_SATURATION,
    __AVT_CEQRSAT_FOR_PILOAD,
    __CPE_PRECHARGED_MEMSYM,
    __AVT_ENBALE_MULTI_CON_ON_NET,
    __AVT_USE_CACHE_PSP,
    __TMA_ALLOW_ACCESS_AS_CLOCKPATH,
    __LIB_DRIVE_TABLE_INDEX,
    __AVT_ENABLE_STAT,
    __STB_SYNC_SLOPES,
    __SPI_DRIVE_USE_UNITS,
    __TAS_ALWAYS_USE_CACHE,
    __TAS_STAT_HITAS_BREAK_ON_ERROR,
    __TAS_CHECK_IMAX,
    __AVT_RST_BETTER,
    __MCC_PRECISE_VT,
    __MCC_PRECISE_K,
    __MCC_FIT_VT_THRESHOLD,
    __MCC_NEW_DIFFMODELSAT,
    __MCC_TEST_B_NEG,
    __MCC_NEW_BEST_ABR,
    __TAS_USE_BSIM_CURRENT,
    __AVT_ALLOW_OVERWRITE_FILE,
    __MCC_NEW_IDS_SAT,
    __TAS_CHECK_BRANCH_MODEL,
    __TAS_NEW_CURRENT_MODEL,
    __AVT_OLD_FEATURE_27,
    __AVT_OLD_FEATURE_28,
    __AVT_OLD_FEATURE_29,
    __AVT_OLD_FEATURE_30,
    __AVT_FALSEPATH_OPTIM,
    __TAS_USE_ALL_CAPAI,
    __STB_CREATE_EQUIV_GROUPS,
    __STB_HANDLE_FALSE_ACCESS,
    __AVT_CORRECT_CGP,
    __SPI_REPLACE_V_IN_EXPR,
    __STM_NEW_OVERSHOOT,
    __SIM_ALLOW_THREADS,
    __SIM_USE_SYSTEM_CMD,
    __MGL_NONBLOKING_ASSIGN,
    __MGL_OLD_STYLE_BUS,
    __MGL_TRISTATE_IS_MEMORY,
    __MGL_INERTIAL_MEMORY,
    __MGL_DRIVE_CONFLICT,
    __API_USE_CORRESP,
    __TAS_MODEL_MEMSYM,
    __TRC_FORCE_PILOAD,
    __YAG_DRIVE_ALIASES
};

//----------------------------------------------------------------------
// STR
//----------------------------------------------------------------------

enum V_STR_LIST {
    __AVT_ERROR_POLICY,                     // avtErrorPolicy
    __API_LIBS,                             // apiDynamicLibraries                         
    __API_LIB_PATH,                         // apiDynamicLibrariesPath  
    __BGL_FILE_SUFFIX,                      // avtBehavioralVerilogSuffix  
    __BVL_FILE_SUFFIX,                      // avtBehavioralVhdlSuffix  
    __MBK_BLACKBOX_NAME,                    // avtBlackboxFile  
    __MBK_CATAL_NAME,                       // avtCatalogueName  
    __MBK_INPUT_VECTOR,                     // avtVectorize  
    __ELP_GEN_TECHNO_NAME,                  // avtElpGenTechnoName  
    __ELP_TECHNO_NAME,                      // avtElpTechnoName  
    __AVT_GNUPLOT_TERM,                     // avtGnuplotTerm  
    __MBK_FILTER_SFX,                       // avtFilterSuffix  
    __MBK_GLOBAL_VDD,                       // avtGlobalVddName  
    __MBK_GLOBAL_VSS,                       // avtGlobalVssName  
    __BEH_IN_FORMAT,                        // avtInputBehaviorFormat  
    __MBK_IN_FILTER,                        // avtInputFilter  
    __MBK_IN_LO,                            // avtInputNetlistFormat  
    __MBK_IN_PARASITICS,                    // avtInputParasiticNetlistFormat  
    __MBK_SEPAR,                            // avtInstanceSeparator  
    __BEG_USER_WAY,                         // avtOutputBehaviorVectorDirection  
    __AVT_LANGUAGE,                         // avtLanguage  
    __MBK_CATA_LIB,                         // avtLibraryDirs  
    __MBK_LIB_FILE,                         // avtLibraryFile  
    __MBK_PASSWORD,                         // avtPassword  
    __AVT_LICENSE_FILE,                     // avtLicenseFile  
    __LM_PROJECT,                           // avtLicenseProject  
    __API_DRIVE_ALL_BEH,                    // apiDriveAllBehavior  
    __MBK_CASE_SENSITIVE,                   // avtCaseSensitive  
    __API_FLAGS,                            // apiFlags  
    __AVT_LICENSE_SERVER,                   // avtLicenseServer  
    __ELP_LOAD_FILE,                        // avtLoadSpecifiedElpFile  
    __AVT_GLOBALLOGFILE,                    // avtLogFile  
    __AVT_LOGLEVEL,                         // avtLogEnable  
    __BEH_OUT_FORMAT,                       // avtOutputBehaviorFormat  
    __MBK_OUT_FILTER,                       // avtOutputFilter  
    __MBK_OUT_LO,                           // avtOutputNetlistFormat  
    __AVT_INF,                              // avtReadInformationFile  
    __SIM_SPICE_OUT,                        // avtSpiceOutFile  
    __SIM_SPICE_STDOUT,                     // avtSpiceStdoutFile  
    __SIM_SPICE_STRING,                     // avtSpiceString  
    __MBK_SPI_SEPAR,                        // avtSpiConnectorSeparator  
    __MBK_SPI_DN,                           // avtSpiDnModelName  
    __MBK_SPICE_DRIVER_FLAGS,               // avtSpiFlags  
    __MBK_SPI_DP,                           // avtSpiDpModelName  
    __MBK_DRIVE_DEFAULT_UNITS,              // avtSpiDriveDefaultUnits  
    __RCN_CACHESIZE,                        // avtParasiticCacheSize  
    __MBK_SPICE_KEEP_CARDS,                 // avtSpiKeepCards  
    __MBK_SPICE_KEEP_NAMES,                 // avtSpiKeepNames  
    __MGL_FILE_SUFFIX,                      // avtStructuralVerilogSuffix  
    __MBK_SPI_TOLERANCE,                    // avtSpiTolerance  
    __MBK_SPI_NETNAME,                      // avtSpiNetName  
    __MBK_SPI_COMPOSE_NAME_SEPAR,           // avtSpiNodeSeparator  
    __MBK_SPI_SUFFIX,                       // avtSpiSuffix  
    __MBK_SPI_TN,                           // avtSpiTnModelName  
    __MBK_SPI_TP,                           // avtSpiTpModelName  
    __MBK_SPI_VECTOR,                       // avtSpiVector  
    __MBK_SPI_PARSE_FIRSTLINE,              // avtSpiParseFirstLine 
    __MVL_FILE_SUFFIX,                      // avtStructuralVhdlSuffix  
    __MCC_MODEL_FILE,                       // avtTechnologyName  
    __MCC_MODEL_FILE_BEST,                  // avtTechnoBestCornerName  
    __MCC_MODEL_FILE_WORST,                 // avtTechnoWorstCornerName  
    __MCC_EXTTECHNO,                        // avtExternalTechnology  
    __MBK_VSS,                              // avtVssName  
    __MBK_WORK_LIB,                         // avtWorkDir  
    __FCL_ANY_NMOS,                         // fclGenericNMOS  
    __FCL_ANY_PMOS,                         // fclGenericPMOS  
    __FCL_LIB_PATH,                         // fclLibraryDir  
    __FCL_LIB_NAME,                         // fclLibraryName  
    __MCC_MOD_SEPAR,                        // avtTechnoModelSeparator  
    __MBK_VDD,                              // avtVddName  
    __GENIUS_LIB_PATH,                      // gnsLibraryDir  
    __GENIUS_LIB_NAME,                      // gnsLibraryName  
    __AVT_TEMPLATE_DIR,                     // gnsTemplateDir  
    __GEN_DEBUG_MODE,                       // gnsTraceFile  
    __GEN_SPY,                              // gnsTraceModel  
    __RCX_CTK_MODEL,                        // rcxCtkModel  
    __RCX_CTK_NOISE,                        // rcxCtkNoise  
    __RCX_CTK_SLOPE_DELAY,                  // rcxCtkSlopeDelay  
    __RCX_CTK_SLOPE_NOISE,                  // rcxCtkSlopeNoise  
    __RCX_DELAY_CACHE,                      // rcxDelayCacheSize  
    __SIM_OUTLOAD,                          // simOutLoad  
    __EFG_SIG_ALIAS,                        // simSignalAlias  
    __SIM_SPICE_OPTIONS,                    // simSpiceOptions  
    __SIM_TECH_FILE,                        // simTechnologyName  
    __TMA_DRIVEDELAY,                       // tmaDriveDelay  
    __STB_FILE_FORMAT,                      // stbFileFormat  
    __STB_MONOPHASE,                        // stbMonoPhase  
    __STB_FOREIGN_TIME_UNIT,                // stbTimeUnit  
    __STB_FOREIGN_CONSTRAINT_FILE,          // stbForeignConstraintFile 
    __TAS_FIG_NAME,                         // tasFigName  
    __TAS_FILE_NAME,                        // tasFileName  
    __TAS_DELAY_SWITCH,                     // tasDelaySwitch  
    __TAS_RCX_DRIVER,                       // tasRCDriverCalcMode  
    __TMA_CUNIT,                            // tmaCapacitanceUnit  
    __TMA_FILEIN,                           // tmaInputFile  
    __TMA_READFILE,                         // tmaLibraryFile  
    __TMA_NAMEIN,                           // tmaInputName  
    __LIB_BUS_DELIMITER,                    // tmaLibBusDelimiter  
    __TMA_LIBRARY,                          // tmaLibraryName  
    __TMA_OUTPUT,                           // tmaOutputFile  
    __TMA_TUNIT,                            // tmaTimeUnit  
    __TMA_PUNIT,                            // tmaPowerUnit  
    __EFG_TRS_ALIAS,                        // simTransistorAlias  
    __SIM_MEAS_CMD,                         // simMeasCmd  
    __YAGLE_DEBUG_CONE,                     // yagleDebugCone  
    __YAGLE_AUTO_ASYNC,                     // yagSetResetDetection 
    __YAGLE_AUTO_RS,                        // yagAutomaticRSDetection 
    __YAGLE_AUTO_MEMSYM,                    // yagAutomaticMemsymDetection 
    __YAGLE_SIMPLE_LATCH,                   // yagleSimpleLatchDetection 
    __YAGLE_CLOCK_GATE,                     // yagDetectClockGating
    __YAGLE_FIGNAME,                        // yagleFigureName  
    __YAGLE_FILENAME,                       // yagleFileName  
    __YAGLE_GNSROOTNAME,                    // yagleGeniusTopName  
    __YAGLE_OUTNAME,                        // yagleOutputName  
    __GENIUS_FLAGS,                         // gnsFlags  
    __FCL_SHARE,                            // fclAllowSharing  
    __RCX_USING_AWEMATRIX,                  // rcxAweMatrix  
    __SLIB_LIBRARY_NAME,                    // xyagIconLibrary  
    __SIM_EXTRACT_RULE,                     // simExtractRule  
    __YAGLE_TAS_TIMING,                     // yagleTasTiming         
    __STM_CACHESIZE,                        // stmCacheSize  
    __ANNOT_KEEP_CARDS,                     // avtAnnotationKeepCards
    __ANNOT_CONNECTOR_SETTING,              // avtAnnotationDeviceConnectorSetting
    __TAS_DEBUG_LOTRS,                      //tasDebugLotrs
    __AVT_WARN_FILTER,                      // avtWarningFilter         
    __TAS_SIMU_INVERTER,                    // tasSimulateInverter
    __TAS_SIMU_INVERTER_CONFIG,             // tasSimulateInverterConfig
    __AVT_HACK_NETLIST_CONFIG,
    __TTV_IGNORE_MAX_FILTER,                // ttvIgnoreMaxFilter
    __AVT_EXCLUDE_COMPRESSION,              // avtDisableCompression
    __TAS_DEBUG_PWL,                        // tasDebugPwl
    __TAS_SETUP_LATCHING_VOLTAGE            // tasSetupLatchingVoltage
};

//----------------------------------------------------------------------
// INT
//----------------------------------------------------------------------

enum V_INT_LIST {
    __VERILOG_MAXERR,                // avtVerilogMaxError                  
    __VHDL_MAXERR,                   // avtVhdlMaxError  
    __ELP_CAPA_LEVEL,                // avtElpCapaLevel  
    __AVT_LICENSE_RESERVE,           // avtLicenseReserve  
    __MBK_MAX_CACHE_FILE,            // avtMaxCacheFile  
    __TTV_MAX_PATH_PERIOD,           // avtMaxPathPeriodDepth  
    __TTV_PRECISION_LEVEL,           // avtPrecisionLevel  
    __SPI_RC_MEMORY_LIMIT,           // avtSpiRCMemoryLimit  
    __CTK_REPORT_DELTA_DELAY_MIN,    // ctkDeltaDelayMin  
    __CTK_REPORT_DELTA_SLOPE_MIN,    // ctkDeltaSlopeMin  
    __CTK_REPORT_NOISE_MIN,          // ctkNoiseMin  
    __MBK_SCALE_X,                   // avtScaleX  
    __FCL_DEBUG_MODE,                // fclDebugMode  
    __FCL_SIZE_TOLERANCE,            // fclMatchSizeTolerance  
    __FCL_TRACE_LEVEL,               // fclTraceLevel  
    __GEN_DEBUG_LEVEL,               // gnsTraceLevel  
    __MBK_TRACE_LEVEL,               // mbkTraceLevel  
    __STB_CTK_COEF_ACTIVITY,         // stbCtkCoefActivity  
    __STB_CTK_COEF_CTK,              // stbCtkCoefCtk  
    __STB_CTK_COEF_INTERVAL,         // stbCtkCoefInterval  
    __STB_CTK_COEF_NOISE,            // stbCtkCoefNoise  
    __STB_CTK_MARGIN,                // stbCtkMargin  
    __STB_CTK_MAXLASTITER,           // stbCtkMaxLastIter  
    __STB_CTK_MIN_ACTIVITY,          // stbCtkMinActivity  
    __STB_CTK_MIN_CTK,               // stbCtkMinCtk  
    __STB_CTK_MIN_INTERVAL,          // stbCtkMinInterval  
    __STB_CTK_MIN_NOISE,             // stbCtkMinNoise  
    __STB_CTK_MINSLOPECHANGE,        // stbCtkminSlopeChange  
    __EFG_MAX_DEPTH,                 // simAnalysisDepth  
    __YAGLE_DEPTH,                   // yagleAnalysisDepth  
    __YAGLE_AUTOLOOP_CEILING,        // yagleAutomaticCeiling  
    __YAGLE_BDDCEILING,              // yagleBddCeiling  
    __TMA_MARGIN,                    // tmaConstraintMargin  
    __YAGLE_RELAX_LINKS,             // yagRelaxationMaxBranchLinks  
    __YAGLE_MAX_LINKS,               // yagMaxBranchLinks  
    __YAGLE_GLITCH_LINKS,            // yagMaxGlitcherLinks  
    __YAGLE_AUTOLOOP_DEPTH,          // yagLatchAnalysisDepth  
    __YAGLE_BLEEDER_STRICTNESS,      // yagBleederStrictness  
    __YAGLE_BUS_DEPTH,               // yagBusAnalysisDepth  
    __YAGLE_SENSITIVE_MAX,           // yagSensitiveTimingDriverLimit
    __YAGLE_DRIVE_CONFLICT,          // yagDriveConflictCondition
    __YAGLE_KEEP_GLITCHERS,          // yagDetectGlitchers 
    __STB__PRECHARGE_DATA_HEURISTIC, // stbPrechargeDataHeuristic
    __STM_CTK_MODEL,
    __STM_OVERSHOOT_LEVEL,
    __STM_INPUT_NB_PWL,
    __STM_QSAT_LEVEL,
    __TAS_SIMULATION_LEVEL,
    __TAS_CAPARA_DEPTH,              // tasPathCapacitanceDepth
    __TAS_CAPARA_STRICT,             // tasStrictPathCapacitance
    __TAS_CAPARA_FANOUT,             // tasMaxPathCapacitanceFanout
    __STB_CTK_MAX_ITER,              // stbCtkMaxIteration
    __STB_CTK_MAX_REPORTED_SIGNAL,   // stbCtkMaxReportedSignals
    __STM_PRECISION_THRESHOLD,
    __CPE_MAX_VARIABLES,             // cpeMaxVariables
    __EQT_STATISTICAL_DISCRETISATION,
    __AVT_RST_N,
    __TAS_USE_ENHANCED_BRANCH_FIT,
    __STB_COREL_SKEW_ANA_DEPTH,      // stbCorrelatedSkewAnalysisDepth
    __YAG_MAX_SPLIT_CMD_TIMING,
    __MCC_SAT_N,
    __SIM_TOOLMODEL,                   // simToolModel  
    __SIM_TOOL,                         // simTool  
    __SIM_TRANSISTOR_AS_INSTANCE,       // simTransistorAsInstance 
    __STB_SUPPRESS_LAG,
    __AVT_POWER_CALCULATION
};

//----------------------------------------------------------------------
// FLOAT
//----------------------------------------------------------------------

enum V_FLOAT_LIST {
    __MCC_TEMP_BEST,              // avtCharacBestTemp             
    __MCC_VDD_BEST,               // avtCharacBestVdd  
    __MCC_RANGE_MARGIN,           // avtCharacRangeMargin  
    __MCC_TEMP_WORST,             // avtCharacWorstTemp  
    __MCC_VDD_WORST,              // avtCharacWorstVdd  
    __TUT_MAX_TEMP,               // avtDeratingMaxTemperature  
    __TUT_MAX_VOLT,               // avtDeratingMaxVoltage  
    __TUT_MIN_TEMP,               // avtDeratingMinTemperature  
    __TUT_MIN_VOLT,               // avtDeratingMinVoltage  
    __ELP_DELVT0_MARGIN,          // avtElpDelVtMargin  
    __ELP_MULU0_MARGIN,           // avtElpMuluMargin  
    __ELP_SA_MARGIN,              // avtElpSaMargin  
    __ELP_SB_MARGIN,              // avtElpSbMargin  
    __ELP_SD_MARGIN,              // avtElpSdMargin  
    __ELP_SC_MARGIN,
    __ELP_SCA_MARGIN,
    __ELP_SCB_MARGIN,
    __ELP_SCC_MARGIN,
    __TAS_MAXSCALE_CAPAFACTOR,    // avtMaxScaleCapaFactor  
    __TAS_MAXSCALE_RESIFACTOR,    // avtMaxScaleResiFactor  
    __TAS_MINSCALE_CAPAFACTOR,    // avtMinScaleCapaFactor  
    __TAS_MINSCALE_RESIFACTOR,    // avtMinScaleResiFactor  
    __MBK_SPI_DRIVE_CAPA_MINI,    // avtSpiDriveCapaMini  
    __MBK_SPI_DRIVE_RESI_MINI,    // avtSpiDriveResiMini  
    __MBK_SPI_MAX_RESI,           // avtSpiMaxResistance  
    __MBK_SPI_MIN_CAPA,           // avtSpiMinCapa  
    __MBK_SPI_MIN_RESI,           // avtSpiMinResistance  
    __MBK_SPI_SCALE_CAPAFACTOR,   // avtSpiScaleCapaFactor  
    __MBK_SPI_SCALE_DIODEFACTOR,  // avtSpiScaleDiodeFactor  
    __MBK_SPI_SCALE_RESIFACTOR,   // avtSpiScaleResiFactor  
    __MBK_SPI_SCALE_TRANSFACTOR,  // avtSpiScaleTransistorFactor  
    __SIM_TECHNO_SIZE,            // avtTechnologySize  
    __MBK_VDD_VSS_THRESHOLD,      // avtVddVssThreshold  
    __STB_NOISE_DEFAULT_RESI,     // ctkNoiseDefaultResi  
    __CTK_REPORT_CTK_MIN,         // ctkCapaMin  
    __RCX_CAPALOAD_MAXRC,         // rcxCapaLoadMaxRC  
    __RCX_MAXDELTALOAD,           // rcxMaxDeltaLoad  
    __RCX_MINRCSIGNAL,            // rcxMinRCSignal  
    __RCX_MINRELCTKFILTER,        // rcxMinRelCtkFilter  
    __RCX_MINRELCTKSIGNAL,        // rcxMinRelCtkSignal  
    __SIM_DC_STEP,                // simDcStep  
    __SIM_INPUT_START,            // simInputStartTime  
    __SIM_OUT_CAPA_VAL,           // simOutCapaValue  
    __SIM_POWER_SUPPLY,           // simPowerSupply  
    __SIM_SIMU_STEP,              // simSimulationStep  
    __SIM_INPUT_SLOPE,            // simSlope  
    __SIM_TEMP,                   // simTemperature  
    __SIM_TNOM,                   // simNominalTemperature  
    __SIM_TRAN_STEP,              // simTransientStep  
    __SIM_TIME,                   // simTransientTime  
    __SIM_VTH,                    // simVth  
    __SIM_VTH_HIGH,               // simVthHigh  
    __SIM_VTH_LOW,                // simVthLow  
    __STM_QSAT_RATIO,
    __FRONT_CON,                  // tasConnectorsInputSlope  
    __TAS_CAPAOUT,                // tasOutputCharge  
    __TAS_CAPARAPREC,             // tasPathCapacitanceFactor  
    __TAS_CAPASWITCH,             // tasSwitchCapacitanceFactor  
    __STB_MIN_PROBABILITY,        // stbCtkMinOccurenceProbability
    __YAGLE_SPLITTIMING_RATIO,    // yagSplitTimingRatio  
    __YAGLE_SENSITIVE_RATIO,      // yagSensitiveTimingRatio  
    __YAGLE_THRESHOLD,            // yagElectricalThreshold                    
    __YAGLE_PULL_RATIO,           // yagPullupRatio                    
    __YAGLE_LATCHLOOP_RATIO,      // yagLoopOrientRatio                    
    __AVT_OPTIM_STEP,
    __LIB_SLEW_DERATE,            // tmaLibSlewDerate                    
    __STM_OVERSHOOT_K_HVT,
    __STM_OVERSHOOT_K_LVT,
    __AVT_HACK_COEF_CCTK,
    __YAG_MIN_RC,
    __YAG_DELTA_DELAY,
    __YAG_RC_STEP,
    __YAG_SINGLE_DELAY_RATIO,
    __TTV_SSTA_CACHE_SIZE,
    __AVT_HACK_COEF_CGND,
    __AVT_LEAKAGE_RATIO
};

typedef struct {
  char *NAME  ;
  char *SHORTNAME ;
  int   LEVEL ;
} libloginfo ;

typedef struct {
  char *NAME ;
  int   LIBLIST[10];
} liblogpool ;

extern FILE *AVTLOGFILE ;
void avt_initlog();
void avt_log( int lib, int level, char *fmt, ... );
void avt_logenterfunction( int lib, int level,char *fnname );
void avt_logexitfunction( int lib, int level);
int avt_getliblogmax();
char* avt_getliblogname( int lib );
char* avt_getliblogshortname( int lib );
int avt_getlibloglevel (int lib);
void avt_setlibloglevel (int lib, int level);
int avt_islog ( int level, int lib );
void avt_PrintErrorSummary();
double avt_parse_unit(char *str, char type);
void V_STR_AFFECT_F(char **x, char *v);

// ------------ configuration variable values -------------
extern char *ANNOT_T_D, *ANNOT_T_S, *ANNOT_T_G, *ANNOT_T_B;
extern char *ANNOT_R_POS, *ANNOT_R_NEG;
extern char *ANNOT_D_POS, *ANNOT_D_NEG;
extern char *ANNOT_C_POS, *ANNOT_C_NEG;
extern char env_SIMUINV, *SIMUINVCONENAME, *SIMUINV_PREFIX;
extern int tpiv_inverter_config_reverse;
extern float tpiv_inverter_config_t0r, tpiv_inverter_config_t0f;
extern float tpiv_inverter_config_tmax;
extern int TTV_MaxPathPeriodPrecharge, TTV_MaxPathPeriodLatch;
extern int TTV_IgnoreMaxFilter;
extern unsigned long int RCN_CACHE_SIZE;

extern void *TCL_INTERPRETER;

#define AVT_STB_SUPPRESS_LAG_LATCH 1
#define AVT_STB_SUPPRESS_LAG_PRECH 2

#define V_STR_AFFECT(x,v) V_STR_AFFECT_F(&x, v)

#if 1
#define BUGERROR

//#define ELPONLY
//#define ELPONLYNOCNS

//#define OPTIM0
//#define OPTIM1
//
//#define OPTIM2
//#define OPTIM3 4
////define OPTIM4 
//#define OPTIM5
////
#define OPTIM6
// //////#define OPTIM7 10
////
//#define OPTIM8
//#define OPTIM9
#define OPTIM10
#define OPTIM11
////
//#define OPTIM12
//#define OPTIM13 1
////
#define OPTIM14
#define OPTIM15
#define OPTIM16
#else
//------------------------------------
//#define BUGERROR

//#define ELPONLY
//#define ELPONLYNOCNS

#define OPTIM0
#define OPTIM1

#define OPTIM2
#define OPTIM3 4
//#define OPTIM4 
#define OPTIM5
////
#define OPTIM6
// //////#define OPTIM7 10
////
#define OPTIM8
#define OPTIM9
#define OPTIM10
#define OPTIM11
////
#define OPTIM12
#define OPTIM13
////
#define OPTIM14
#define OPTIM15
#define OPTIM16
#endif
#endif
