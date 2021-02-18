
#include <ctype.h>
#include "avt_env.h"
#include MUT_H
#include "avt_init_funcs.h"

int setnewcurrentmodel( char *var, char *val, int *result );

//======================================================================
//  GLOBALS 
//======================================================================

int                AVT_VALID_TOKEN;
int                AVT_COL       = 0;
char              *AVERTEC_VERSION = NULL;
char              *AVT_FULLVERSION = NULL;
static ptype_list *AVT_VARENV_LIST = NULL;
static char        AVT_VARENV_FILE = 'Y';
void *TCL_INTERPRETER=NULL;
//======================================================================
//  TOKEN LIST
//======================================================================

char *AVT_PRODUCT_TOKEN_MATCH[] =
{
    "avt",        "AVT",
    "avt_shell",  "AVT",
    "basic_api",  "AVT",
    "cnsdiff",    "AVT",
    "cpe",        "AV1400",
    "ctk",        "AV1200",
    "ctk_api",    "AV1200",
    "etas",       "AV1000",
    "func_api",   "AV2600",
    "genapi",     "AVT",
    "genelp",     "AVT",
    "gns",        "AV1600",
    "bbox",       "AV1600",
    "hitas",      "AV1000",
    "netutil",    "AVT",
    "spi_api",    "AV1400",
    "stb",        "AV1000",
    "ttvdiff",    "AVT",
    "ttvren",     "AVT",
    "tma",        "AV1300",
    "timing_api", "AV1000",
    "x2v",        "AV1000",
    "xtas",       "AV1010",
    "xyagle",     "AV2010",
    "yagle",      "AV2000",
    "yagle_api",  "AV2000",
    "yagtime",    "AV2300",
    NULL
};

char *AVT_SUPER_TOKEN_MATCH[] =
{
    "AV1000","SP_HITAS",
    "AV1010","SP_XTAS",
    "AV1200","SP_HITAS",
    "AV1300","SP_HITAS",
    "AV1400","SP_HITAS",
    "AV1600","SP_HITAS",
    "AV2000","SP_YAGLE",
    "AV2010","SP_XYAGLE",
    "AV2300","SP_YAGLE",
    "AV2600","SP_YAGLE",
    NULL
};

//======================================================================
//
// Tables Init
//
//======================================================================

//----------------------------------------------------------------------
// BOOLEAN
//----------------------------------------------------------------------

int bool_init_check (char *var, char *val, int *result) 
{ 
  if (!strcasecmp (val, "yes")) *result=1;
  else if (!strcasecmp (val, "no")) *result=0;
  else {
      avt_errmsg (AVT_ERRMSG, "040", AVT_WARNING, val, var);
      return 0;
  }
  return 1;
}

V_BOOL V_BOOL_TAB[] = {
//   Env_Variable                      Index                            Val Doc Set Variable
    {"ANNOT_KEEP_M"                   , __ANNOT_KEEP_M                  , 1, 1, 0, "avtAnnotationKeepM"                      , NULL, bool_init_check},
    {"ANNOT_KEEP_X"                   , __ANNOT_KEEP_X                  , 1, 1, 0, "avtAnnotationKeepX"                      , NULL, bool_init_check}, 
    {"API_DRIVE_CORRESP"              , __API_DRIVE_CORRESP             , 0, 1, 0, "apiDriveCorrespondenceTable"             , NULL, bool_init_check}, 
    {"API_USE_CORRESP"                , __API_USE_CORRESP               , 0, 1, 0, "apiUseCorrespondenceTable"               , NULL, bool_init_check}, 
    {"TUT_CALC_DERATE"                , __TUT_CALC_DERATE               , 0, 1, 0, "avtCalcDerating"                         , NULL, bool_init_check}, //ok
    {"MCC_RESIZE_RANGE"               , __MCC_RESIZE_RANGE              , 0, 0, 0, "avtCharacAllowRangeMargin"               , NULL, bool_init_check}, 
    {"MCC_CALC_VT"                    , __MCC_CALC_VT                   , 1, 0, 0, "avtCharacCalcVth"                        , NULL, bool_init_check}, 
    {"MCC_OPTIM_MODE"                 , __MCC_OPTIM_MODE                , 1, 0, 0, "avtCharacFastMode"                       , NULL, bool_init_check}, 
    {"MCC_EQUATE_RANGE"               , __MCC_EQUATE_RANGE              , 1, 0, 0, "avtCharacForEachSize"                    , NULL, bool_init_check}, 
    {"AVT_COLOR"                      , __AVT_COLOR                     , 0, 0, 0, "avtColor"                                , NULL, bool_init_check}, 
    {"AVT_ENABLE_CORE"                , __AVT_ENABLE_CORE               , 0, 0, 0, "avtEnableCore"                           , NULL, bool_init_check},
    {"AVT_TRACE_FATAL"                , __AVT_TRACE_FATAL               , 0, 0, 0, "avtTraceFatalError"                      , NULL, bool_init_check},
    {"ELP_DEDUCE_DIFFSIZE"            , __ELP_DEDUCE_DIFFSIZE           , 0, 0, 0, "avtDeduceDiffusionSize"                  , NULL, bool_init_check}, 
    {"DSPF_PRESERVE_RC"               , __DSPF_PRESERVE_RC              , 0, 1, 0, "avtDspfPreserveExistingParasitics"       , "avtAnnotationPreserveExistingParasitics", bool_init_check},  
    {"MBK_DUP_PNAME_FOR_FLAT"         , __MBK_DUP_PNAME_FOR_FLAT        , 1, 0, 0, "avtDuppNameForFlat"                      , NULL, bool_init_check}, 
    {"ELP_DRV_FILE"                   , __ELP_DRV_FILE                  , 0, 1, 0, "avtElpDriveFile"                         , NULL, bool_init_check}, //ok
    {"MBK_FAST_MODE"                  , __MBK_FAST_MODE                 , 0, 0, 0, "avtFastMode"                             , NULL, bool_init_check}, 
    {"MBK_FLATTEN_FOR_PARA"           , __MBK_FLATTEN_FOR_PARA          , 0, 1, 0, "avtFlattenForParasitic"                  , NULL, bool_init_check}, 
    {"MBK_KEEP_ALL_SIGNAL_NAMES"      , __MBK_KEEP_ALL_SIGNAL_NAMES     , 0, 1, 0, "avtFlattenKeepsAllSignalNames"           , NULL, bool_init_check}, 
    {"AVT_FLEX_LICENSE"               , __AVT_FLEX_LICENSE              , 1, 0, 0, "avtFlexLicense"                          , NULL, bool_init_check}, 
    {"AVT_ALL_TRANS_SUPPLIES"         , __AVT_ALL_TRANS_SUPPLIES        , 0, 1, 0, "avtTraceAllTrsSupplies"                  , NULL, bool_init_check},
    {"MBK_SPI_NAMENODES"              , __MBK_SPI_NAMENODES             , 1, 1, 0, "avtSpiNameNodes"                         , NULL, bool_init_check}, 
    {"MBK_SPI_AUTO_LOAD"              , __MBK_SPI_AUTO_LOAD             , 0, 0, 0, "avtSpiLoadCellsFromLib"                  , NULL, bool_init_check}, 
    {"MBK_SPI_NOBULK"                 , __MBK_SPI_NOBULK                , 0, 0, 0, "avtSpiNoBulk"                            , NULL, bool_init_check}, 
    {"MBK_SPI_MERGE"                  , __MBK_SPI_MERGE                 , 1, 1, 0, "avtSpiMergeConnector"                    , NULL, bool_init_check}, 
    {"MBK_SPI_MERGE_DIODES"           , __MBK_SPI_MERGE_DIODES          , 0, 1, 0, "avtSpiMergeDiodes"                       , NULL, bool_init_check}, 
    {"SPI_DRIVE_PARASITICS"           , __SPI_DRIVE_PARASITICS          , 0, 1, 0, "avtSpiDriveParasitics"                   , NULL, bool_init_check}, 
    {"MBK_SPI_CREATE_TOP_FIGURE"      , __MBK_SPI_CREATE_TOP_FIGURE     , 1, 1, 0, "avtSpiCreateTopFigure"                   , NULL, bool_init_check}, 
    {"MBK_LOAD_PARASITICS"            , __MBK_LOAD_PARASITICS           , 0, 1, 0, "avtLoadParasitics"                       , NULL, bool_init_check}, 
    {"MBK_SPI_DRV_TRSPARAM"           , __MBK_SPI_DRV_TRSPARAM          , 1, 1, 0, "avtSpiDriveTrsInstanceParams"            , NULL, bool_init_check}, 
    {"SPI_DSPF_BUILD_POWER"           , __SPI_DSPF_BUILD_POWER          , 0, 1, 0, "avtSpiDspfBuildPower"                    , NULL, bool_init_check}, 
    {"SPI_DSPF_LINK_EXTERNAL"         , __SPI_DSPF_LINK_EXTERNAL        , 0, 1, 0, "avtSpiDspfLinkExternal"                  , NULL, bool_init_check}, 
    {"MBK_HANDLE_GLOBAL_NODES"        , __MBK_HANDLE_GLOBAL_NODES       , 1, 1, 0, "avtSpiHandleGlobalNodes"                 , NULL, bool_init_check}, 
    {"MBK_SPI_IGNORE_CAPA"            , __MBK_SPI_IGNORE_CAPA           , 0, 0, 0, "avtSpiIgnoreCapa"                        , NULL, bool_init_check}, 
    {"MBK_SPI_IGNORE_DIODES"          , __MBK_SPI_IGNORE_DIODES         , 0, 1, 0, "avtSpiIgnoreDiode"                       , NULL, bool_init_check}, 
    {"MBK_SPI_IGNORE_MODELS"          , __MBK_SPI_IGNORE_MODELS         , 0, 1, 0, "avtSpiIgnoreModel"                       , NULL, bool_init_check}, 
    {"MBK_SPI_IGNORE_RESI"            , __MBK_SPI_IGNORE_RESI           , 0, 0, 0, "avtSpiIgnoreResistance"                  , NULL, bool_init_check}, 
    {"MBK_SPI_IGNORE_VOLTAGE"         , __MBK_SPI_IGNORE_VOLTAGE        , 0, 1, 0, "avtSpiIgnoreVoltage"                     , NULL, bool_init_check}, 
    {"MBK_SPI_IGNORE_CRYPT"           , __MBK_SPI_IGNORE_CRYPT          , 0, 1, 0, "avtSpiIgnoreCrypt"                       , NULL, bool_init_check}, 
    {"MBK_SPI_JFET_RESI"              , __MBK_SPI_JFET_RESI             , 0, 1, 0, "avtSpiJFETisResistance"                  , NULL, bool_init_check}, 
    {"MBK_SPI_NO_SIG_PREFIX"          , __MBK_SPI_NO_SIG_PREFIX         , 1, 0, 0, "avtSpiNoSigPrefix"                       , NULL, bool_init_check}, 
    {"MBK_SPI_ONE_NODE_NORC"          , __MBK_SPI_ONE_NODE_NORC         , 0, 1, 0, "avtSpiOneNodeNoRc"                       , NULL, bool_init_check}, 
    {"SPI_ORDER_PIN_POWER"            , __SPI_ORDER_PIN_POWER           , 0, 1, 0, "avtSpiOrderPinPower"                     , NULL, bool_init_check}, 
    {"SPI_PIN_DSPF_ORDER"             , __SPI_PIN_DSPF_ORDER            , 0, 1, 0, "avtSpiPinDspfOrder"                      , NULL, bool_init_check}, 
    {"MGL_PARSE_ESC_VECTORS"          , __MGL_PARSE_ESC_VECTORS         , 0, 1, 0, "avtStructuralVerilogVectors"             , NULL, bool_init_check}, 
    {"MBK_SPI_INSTANCE_MULTINODE"     , __MBK_SPI_INSTANCE_MULTINODE    , 1, 0, 0, "avtSpiInstanceMultiNode"                 , NULL, bool_init_check}, 
    {"MBK_SPI_ZEROVOLT_RESI"          , __MBK_SPI_ZEROVOLT_RESI         , 0, 1, 0, "avtSpiShortCircuitZeroVolts"             , NULL, bool_init_check}, 
    {"MVL_CONFIGURE"                  , __MVL_CONFIGURE                 , 0, 1, 0, "avtStructuralVhdlConfigure"              , NULL, bool_init_check}, 
    {"ELP_GEN_PARAM"                  , __ELP_GEN_PARAM                 , 1, 0, 0, "avtUseOnlySpecifiedElpFile"              , NULL, bool_init_check}, 
    {"MGL_USE_LIBRARY"                , __MGL_USE_LIBRARY               , 0, 1, 0, "avtUseVerilogLibrary"                    , NULL, bool_init_check}, 
    {"CNS_DRIVE_VERBOSE"              , __CNS_DRIVE_VERBOSE             , 0, 1, 0, "avtVerboseConeFile"                      , NULL, bool_init_check}, //ok
    {"INF_CONF_CHECK"                 , __INF_CONF_CHECK                , 1, 0, 0, "avtVerifyInformationFile"                , NULL, bool_init_check}, 
    {"CNS_DRIVE_NORMAL"               , __CNS_DRIVE_NORMAL              , 1, 1, 0, "avtNormalConeFile"                       , NULL, bool_init_check}, //ok
    {"FCL_USECUT"                     , __FCL_USECUT                    , 0, 1, 0, "fclCutMatchedTransistors"                , NULL, bool_init_check}, 
    {"FCL_FILE"                       , __FCL_FILE                      , 0, 1, 0, "fclWriteReport"                          , NULL, bool_init_check}, 
    {"GEN_KEEP_ALL"                   , __GEN_KEEP_ALL                  , 0, 1, 0, "gnsKeepAllCells"                         , NULL, bool_init_check}, 
    {"STB_CTK"                        , __STB_CTK                       , 0, 1, 0, "stbCrosstalkMode"                        , NULL, bool_init_check}, 
    {"RCX_USE_MATRIX_LOAD"            , __RCX_USE_MATRIX_LOAD           , 0, 1, 0, "rcxUseMatrixForLoad"                     , NULL, bool_init_check}, 
    {"SIM_OVR_FILES"                  , __SIM_OVR_FILES                 , 1, 1, 0, "simAllowOverwriteFile"                   , NULL, bool_init_check}, 
    {"RCX_FASTMODE"                   , __RCX_FASTMODE                  , 1, 1, 0, "rcxFastMode"                             , NULL, bool_init_check}, 
    {"SIM_REMOVE_FILES"               , __SIM_REMOVE_FILES              , 0, 1, 0, "simRemoveFiles"                          , NULL, bool_init_check}, 
    {"EFG_CORRESP_ALIAS"              , __EFG_CORRESP_ALIAS             , 0, 1, 0, "simDriveAliasCorrespondance"             , NULL, bool_init_check}, 
    {"SIM_USE_MEAS"                   , __SIM_USE_MEAS                  , 1, 1, 0, "simUseMeasure"                           , NULL, bool_init_check}, 
    {"SIM_USE_PRINT"                  , __SIM_USE_PRINT                 , 0, 1, 0, "simUsePrint"                             , NULL, bool_init_check}, 
    {"STB_CTK_FASTMODE"               , __STB_CTK_FASTMODE              , 1, 1, 0, "stbCtkFastMode"                          , NULL, bool_init_check}, 
    {"STB_CTK_LINE"                   , __STB_CTK_LINE                  , 0, 1, 0, "stbCtkLineMode"                          , NULL, bool_init_check}, 
    {"STB_CTK_NOINFO_ACTIF"           , __STB_CTK_NOINFO_ACTIF          , 0, 1, 0, "stbCtkNoInfoActif"                       , NULL, bool_init_check}, 
    {"STB_CTK_OBSERVABLE"             , __STB_CTK_OBSERVABLE            , 1, 1, 0, "stbCtkObservableMode"                    , NULL, bool_init_check}, 
    {"STB_CTK_REPORT"                 , __STB_CTK_REPORT                , 0, 1, 0, "stbCtkReportFile"                        , NULL, bool_init_check}, 
    {"STB_CTK_WORST"                  , __STB_CTK_WORST                 , 0, 1, 0, "stbCtkWorstBeginCondition"               , NULL, bool_init_check}, 
    {"STB_DET_ANALYSIS"               , __STB_DET_ANALYSIS              , 0, 1, 0, "stbDetailedAnalysis"                     , NULL, bool_init_check}, 
    {"STB_DET_GRAPH"                  , __STB_DET_GRAPH                 , 0, 1, 0, "stbDetailedGraph"                        , NULL, bool_init_check}, 
    {"STB_MULTIPLE_COMMAND"           , __STB_MULTIPLE_COMMAND          , 1, 0, 0, "stbMultipleMemoryCommand"                , NULL, bool_init_check}, 
    {"STB_STABILITY_HOLD"             , __STB_STABILITY_HOLD            , 0, 1, 0, "stbHoldOnly"                             , NULL, bool_init_check}, 
    {"STB_OUT_FILE"                   , __STB_OUT_FILE                  , 1, 1, 0, "stbOutFile"                              , NULL, bool_init_check}, 
    {"STB_REPORT_FILE"                , __STB_REPORT_FILE               , 1, 1, 0, "stbReportFile"                           , NULL, bool_init_check}, 
    {"STB_SAVE_STE"                   , __STB_SAVE_STE                  , 0, 1, 0, "stbSaveErrors"                           , NULL, bool_init_check}, 
    {"STB_STABILITY_SETUP"            , __STB_STABILITY_SETUP           , 0, 1, 0, "stbSetupOnly"                            , NULL, bool_init_check}, 
    {"STB_SILENT"                     , __STB_SILENT                    , 0, 1, 0, "stbSilentMode"                           , NULL, bool_init_check}, 
    {"STB_STABILITY_LAST"             , __STB_STABILITY_LAST            , 0, 1, 0, "stbTopLevelPath"                         , NULL, bool_init_check}, 
    {"STB_TRACE_MODE"                 , __STB_TRACE_MODE                , 0, 1, 0, "stbTraceMode"                            , NULL, bool_init_check}, 
    {"STB_STABILITY_WORST"            , __STB_STABILITY_WORST           , 0, 1, 0, "stbWorstCaseAnalysis"                    , NULL, bool_init_check}, 
    {"STM_SHARE_MODEL"                , __STM_SHARE_MODEL               , 0, 1, 0, "stmShareModel"                           , NULL, bool_init_check}, //ok
    {"TAS_BREAKLOOP"                  , __TAS_BREAKLOOP                 , 0, 0, 0, "tasBreakLoops"                           , NULL, bool_init_check}, 
    {"TAS_CALCRCN"                    , __TAS_CALCRCN                   , 1, 1, 0, "tasCalcRCDelays"                         , NULL, bool_init_check}, 
    {"TAS_DELAY_PROP"                 , __TAS_DELAY_PROP                , 1, 1, 0, "tasDelayPropagation"                     , NULL, bool_init_check}, 
    {"TAS_INT_END"                    , __TAS_INT_END                   , 1, 1, 0, "tasExitAfterDetailTimingFile"            , NULL, bool_init_check}, 
    {"TAS_FLATCELLS"                  , __TAS_FLATCELLS                 , 0, 1, 0, "tasFlatcells"                            , NULL, bool_init_check}, 
    {"TAS_CNS_FILE"                   , __TAS_CNS_FILE                  , 1, 1, 0, "tasGenerateConeFile"                     , NULL, bool_init_check}, 
    {"TAS_PERFINT"                    , __TAS_PERFINT                   , 1, 0, 0, "tasGenerateDetailTimingFile"             , NULL, bool_init_check}, 
    {"TAS_HIER"                       , __TAS_HIER                      , 0, 1, 0, "tasHierarchicalMode"                     , NULL, bool_init_check}, 
    {"TAS_IGNBLACKB"                  , __TAS_IGNBLACKB                 , 0, 1, 0, "tasBlackboxRequiresTimings"              , "tasIgnoreBlackbox", bool_init_check}, 
    {"TAS_CARAC_MEMORY"               , __TAS_CARAC_MEMORY              , 1, 1, 0, "tasMemoryCharacterization"               , NULL, bool_init_check}, 
    {"TAS_MERGERCN"                   , __TAS_MERGERCN                  , 0, 1, 0, "tasMergeRCAndGateDelays"                 , NULL, bool_init_check}, 
    {"TAS_FIND_MIN"                   , __TAS_FIND_MIN                  , 1, 0, 0, "tasMinimumPathAnalysis"                  , NULL, bool_init_check}, 
    {"TMA_TTXIN"                      , __TMA_TTXIN                     , 0, 0, 0, "tmaTtxInput"                             , NULL, bool_init_check}, 
    {"TMA_DTXIN"                      , __TMA_DTXIN                     , 0, 0, 0, "tmaDtxInput"                             , NULL, bool_init_check},
    {"TAS_NO_PROP"                    , __TAS_NO_PROP                   , 0, 0, 0, "tasNoSlopePropagation"                   , NULL, bool_init_check}, 
    {"TAS_FACTORISE"                  , __TAS_FACTORISE                 , 0, 1, 0, "tasPathFactorisation"                    , NULL, bool_init_check}, 
    {"TAS_SAVE_BEFIG"                 , __TAS_SAVE_BEFIG                , 0, 1, 0, "tasBefig"                                , NULL, bool_init_check}, 
    {"TAS_PRES_CON_DIR"               , __TAS_PRES_CON_DIR              , 0, 1, 0, "tasPreserveConnectorsDirection"          , NULL, bool_init_check}, 
    {"TAS_SHORT_MODELNAME"            , __TAS_SHORT_MODELNAME           , 0, 1, 0, "tasShortNamesForModels"                  , NULL, bool_init_check}, 
    {"TAS_SILENT_MODE"                , __TAS_SILENT_MODE               , 0, 1, 0, "tasSilentMode"                           , NULL, bool_init_check}, 
    {"TAS_SLOFILE"                    , __TAS_SLOFILE                   , 0, 0, 0, "tasSlopeFile"                            , NULL, bool_init_check}, 
    {"TAS_SIMU_CONE"                  , __TAS_SIMU_CONE                 , 0, 0, 0, "tasSpiceSimulation"                      , NULL, bool_init_check}, 
    {"TAS_SUPBLACKB"                  , __TAS_SUPBLACKB                 , 0, 1, 0, "tasTreatBlackboxHierarchically"          , NULL, bool_init_check}, 
    {"TAS_DIF_LATCH"                  , __TAS_DIF_LATCH                 , 0, 0, 0, "tasTreatDifferentialLatches"             , NULL, bool_init_check}, 
    {"TAS_TREATPRECH"                 , __TAS_TREATPRECH                , 0, 1, 0, "tasTreatPrecharge"                       , NULL, bool_init_check}, 
    {"STM_USE_MSC"                    , __STM_USE_MSC                   , 0, 0, 0, "tasUseMultipleSlopeConstant"             , NULL, bool_init_check}, //ok
    {"TAS_STABILITY"                  , __TAS_STABILITY                 , 0, 0, 0, "tasWithStability"                        , NULL, bool_init_check}, 
    {"TMA_CTXIN"                      , __TMA_CTXIN                     , 0, 0, 0, "tmaCtxInput"                             , NULL, bool_init_check}, 
    {"TMA_DRIVECAPAOUT"               , __TMA_DRIVECAPAOUT              , 0, 1, 0, "tmaDriveCapaout"                         , NULL, bool_init_check}, 
    {"TMA_VERBOSE"                    , __TMA_VERBOSE                   , 0, 1, 0, "tmaVerboseMode"                          , NULL, bool_init_check}, 
    {"XTAS_TEXT_DISPLAY"              , __XTAS_TEXT_DISPLAY             , 1, 1, 0, "xtasTextualDisplay"                      , NULL, bool_init_check}, 
    {"CGV_MAKE_CELLS"                 , __CGV_MAKE_CELLS                , 1, 1, 0, "xyagMakeCells"                           , NULL, bool_init_check}, 
    {"YAGLE_BEH_ASSUME_PRECEDE"       , __YAGLE_BEH_ASSUME_PRECEDE      , 0, 1, 0, "yagAssumeExpressionPrecedence"         , "yagleAssumeExpressionPrecedence", bool_init_check}, 
    {"YAGLE_LOOP_ANALYSIS"            , __YAGLE_LOOP_ANALYSIS           , 1, 1, 0, "yagAutomaticLatchDetection"            , "yagleAutomaticLatchDetection"   , bool_init_check}, 
    {"YAGLE_BLEEDER_PRECHARGE"        , __YAGLE_BLEEDER_PRECHARGE       , 0, 1, 0, "yagBleederIsPrecharge"                 , "yagleBleederIsPrecharge"        , bool_init_check}, 
    {"YAGLE_BLOCK_BIDIR"              , __YAGLE_BLOCK_BIDIR             , 0, 1, 0, "yagBlockBidirectional"                 , "yagleBlockBidirectional"        , bool_init_check}, 
    {"YAGLE_BUS_ANALYSIS"             , __YAGLE_BUS_ANALYSIS            , 0, 1, 0, "yagBusAnalysis"                        , "yagleBusAnalysis"               , bool_init_check}, 
    {"YAGLE_LEVELHOLD_ANALYSIS"       , __YAGLE_LEVELHOLD_ANALYSIS      , 0, 1, 0, "yagLevelHoldAnalysis"                  , NULL, bool_init_check}, 
    {"YAGLE_COMPACT_BEHAVIOUR"        , __YAGLE_COMPACT_BEHAVIOUR       , 0, 1, 0, "yagCompactBehavior"                    , "yagleCompactBehavior"           , bool_init_check}, 
    {"YAGLE_CAPACITANCE_CONES"        , __YAGLE_CAPACITANCE_CONES       , 1, 1, 0, "yagCapacitanceCones"                   , "yagleCapacitanceCones"          , bool_init_check}, 
    {"YAGLE_DIODE_TRANS"              , __YAGLE_DIODE_TRANS             , 1, 1, 0, "yagTestTransistorDiodes"               , "yagleTestTransistorDiodes"      , bool_init_check}, 
    {"YAGLE_DETECT_PRECHARGE"         , __YAGLE_DETECT_PRECHARGE        , 0, 1, 0, "yagDetectPrecharge"                    , "yagleDetectPrecharge"           , bool_init_check}, 
    {"YAGLE_DETECT_REDUNDANT"         , __YAGLE_DETECT_REDUNDANT        , 1, 1, 0, "yagDetectRedundant"                    , "yagleDetectRedundant"           , bool_init_check}, 
    {"YAGLE_ELP"                      , __YAGLE_ELP                     , 0, 1, 0, "yagElpCorrection"                      , "yagleElpCorrection"             , bool_init_check}, 
    {"YAGLE_MAKE_CELLS"               , __YAGLE_MAKE_CELLS              , 1, 0, 0, "yagFlipFlopDetection"                  , "yagleFlipFlopDetection"         , bool_init_check}, 
    {"YAGLE_VBE"                      , __YAGLE_VBE                     , 1, 1, 0, "yagGenerateBehavior"                   , "yagleGenerateBehavior"          , bool_init_check}, 
    {"YAGLE_FILE"                     , __YAGLE_FILE                    , 1, 1, 0, "yagGenerateConeFile"                   , "yagleGenerateConeFile"          , bool_init_check}, 
    {"YAGLE_CONE_NETLIST"             , __YAGLE_CONE_NETLIST            , 0, 1, 0, "yagGenerateConeNetList"                , "yagleGenerateConeNetList"       , bool_init_check}, 
    {"YAGLE_GEN_SIGNATURE"            , __YAGLE_GEN_SIGNATURE           , 1, 1, 0, "yagGenSignature"                       , "yagleGenSignature"              , bool_init_check}, 
    {"YAGLE_HIERARCHICAL_MODE"        , __YAGLE_HIERARCHICAL_MODE       , 0, 1, 0, "yagHierarchicalMode"                   , "yagleHierarchicalMode"          , bool_init_check}, 
    {"YAGLE_PROP_HZ"                  , __YAGLE_PROP_HZ                 , 0, 1, 0, "yagHzAnalysis"                         , "yagleHzAnalysis"                , bool_init_check}, 
    {"YAGLE_BLACKBOX_IGNORE"          , __YAGLE_BLACKBOX_IGNORE         , 0, 1, 0, "yagIgnoreBlackboxes"                   , "yagleIgnoreBlackboxes"          , bool_init_check}, 
    {"YAGLE_CELL_SHARE"               , __YAGLE_CELL_SHARE              , 0, 0, 0, "yagIntersectingFlipFlops"              , "yagleIntersectingFlipFlops"     , bool_init_check}, 
    {"YAGLE_KEEP_REDUNDANT"           , __YAGLE_KEEP_REDUNDANT          , 0, 1, 0, "yagKeepRedundantBranches"              , "yagleKeepRedundantBranches"     , bool_init_check}, 
    {"YAGLE_LATCH_REQUIRE_CLOCK"      , __YAGLE_LATCH_REQUIRE_CLOCK     , 0, 1, 0, "yagLatchesRequireClocks"               , "yagleLatchesRequireClocks"      , bool_init_check}, 
    {"YAGLE_MARK_TRISTATE_MEMORY"     , __YAGLE_MARK_TRISTATE_MEMORY    , 0, 1, 0, "yagDetectDynamicLatch"                 , "yagMarkTristateMemory"          , bool_init_check}, 
    {"YAGLE_AUTO_FLIPFLOP"            , __YAGLE_AUTO_FLIPFLOP           , 1, 0, 0, "yagAutomaticFlipFlopDetection"         , NULL, bool_init_check}, 
    {"YAGLE_MINIMISE_CONES"           , __YAGLE_MINIMISE_CONES          , 0, 0, 0, "yagMinimizeCones"                      , "yagleMinimizeCones"             , bool_init_check}, 
    {"YAGLE_MINIMISE_INVERTORS"       , __YAGLE_MINIMISE_INVERTORS      , 0, 1, 0, "yagMinimizeInvertors"                  , "yagleMinimizeInvertors"         , bool_init_check}, 
    {"YAGLE_NOTSTRICT"                , __YAGLE_NOTSTRICT               , 0, 1, 0, "yagNotStrict"                          , "yagleNotStrict"                 , bool_init_check}, 
    {"YAGLE_ONE_SUPPLY"               , __YAGLE_ONE_SUPPLY              , 0, 1, 0, "yagOneSupply"                          , "yagleOneSupply"                 , bool_init_check}, 
    {"YAGLE_NO_SUPPLY"                , __YAGLE_NO_SUPPLY               , 0, 1, 0, "yagNoSupply"                           , "yagleNoSupply"                  , bool_init_check}, 
    {"YAGLE_RELAX_ALGO"               , __YAGLE_RELAX_ALGO              , 0, 1, 0, "yagRelaxationAnalysis"                 , "yagleRelaxationAnalysis"        , bool_init_check}, 
    {"YAGLE_NORC"                     , __YAGLE_NORC                    , 0, 1, 0, "yagRemoveInterconnects"                , "yagleRemoveInterconnects"       , bool_init_check}, 
    {"YAGLE_REMOVE_PARATRANS"         , __YAGLE_REMOVE_PARATRANS        , 1, 0, 0, "yagRemoveParallelTrans"                , "yagleRemoveParallelTrans"       , bool_init_check}, 
    {"YAGLE_LOOP_MODE"                , __YAGLE_LOOP_MODE               , 0, 1, 0, "yagSearchLoops"                        , "yagleSearchLoops"               , bool_init_check}, 
    {"YAGLE_SILENT_MODE"              , __YAGLE_SILENT_MODE             , 0, 1, 0, "yagSilentMode"                         , "yagleSilentMode"                , bool_init_check}, 
    {"YAGLE_ORIENT"                   , __YAGLE_ORIENT                  , 0, 1, 0, "yagSimpleOrientation"                  , "yagleSimpleOrientation"         , bool_init_check}, 
    {"YAGLE_SIMPLIFY_EXPRESSIONS"     , __YAGLE_SIMPLIFY_EXPRESSIONS    , 0, 1, 0, "yagSimplifyExpressions"                , "yagleSimplifyExpressions"       , bool_init_check}, 
    {"YAGLE_SIMPLIFY_PROCESSES"       , __YAGLE_SIMPLIFY_PROCESSES      , 0, 1, 0, "yagSimplifyProcesses"                  , "yagleSimplifyProcesses"         , bool_init_check}, 
    {"YAGLE_DETECT_LATCHES"           , __YAGLE_DETECT_LATCHES          , 1, 1, 0, "yagStandardLatchDetection"             , "yagleStandardLatchDetection"    , bool_init_check}, 
    {"YAGLE_STRICT_CKLATCH"           , __YAGLE_STRICT_CKLATCH          , 0, 0, 0, "yagStrictCkLatch"                      , "yagleStrictCkLatch"             , bool_init_check}, 
    {"YAGLE_STUCK_LATCH"              , __YAGLE_STUCK_LATCH             , 1, 0, 0, "yagStuckLatch"                         , "yagleStuckLatch"                , bool_init_check}, 
    {"YAGLE_ANALYSE_STUCK"            , __YAGLE_ANALYSE_STUCK           , 1, 0, 0, "yagStuckAnalysis"                      , "yagleStuckAnalysis"             , bool_init_check}, 
    {"YAGLE_DELAYED_RS"               , __YAGLE_DELAYED_RS              , 1, 1, 0, "yagDetectDelayedRS"                    , "yagleDetectDelayedRS"           , bool_init_check}, 
    {"YAGLE_MEMSYM_HEURISTIC"         , __YAGLE_MEMSYM_HEURISTIC        , 0, 0, 0, "yagMemsymHeuristic"                    , "yagleMemsymHeuristic"           , bool_init_check}, 
    {"YAGLE_ASYNC_COMMAND"            , __YAGLE_ASYNC_COMMAND           , 0, 0, 0, "yagAsyncIsCommand"                     , "yagAsyncIsCommand"              , bool_init_check}, 
    {"YAGLE_BLACKBOX_SUPPRESS"        , __YAGLE_BLACKBOX_SUPPRESS       , 0, 1, 0, "yagSuppressBlackboxes"                 , "yagleSuppressBlackboxes"        , bool_init_check}, 
    {"YAGLE_USE_CONNECTOR_DIRECTION"  , __YAGLE_USE_CONNECTOR_DIRECTION , 0, 0, 0, "yagUseConnectorDirection"              , "yagUseConnectorDirection"       , bool_init_check}, 
    {"YAGLE_TRISTATE_MEMORY"          , __YAGLE_TRISTATE_MEMORY         , 0, 1, 0, "yagTristateIsMemory"                   , "yagleTristateIsMemory"          , bool_init_check}, 
    {"YAGLE_FCL_DETECT"               , __YAGLE_FCL_DETECT              , 0, 0, 0, "yagUseFcl"                             , "yagleUseFcl"                    , bool_init_check}, 
    {"YAGLE_GENIUS"                   , __YAGLE_GENIUS                  , 0, 1, 0, "yagUseGenius"                          , "yagleUseGenius"                 , bool_init_check}, 
    {"YAGLE_HELP_S"                   , __YAGLE_HELP_S                  , 0, 1, 0, "yagUseNameOrientation"                 , "yagleUseNameOrientation"        , bool_init_check}, 
    {"YAGLE_ONLY_FCL"                 , __YAGLE_ONLY_FCL                , 0, 0, 0, "yagUseOnlyFcl"                         , "yagleUseOnlyFcl"                , bool_init_check}, 
    {"YAGLE_ONLY_GENIUS"              , __YAGLE_ONLY_GENIUS             , 0, 1, 0, "yagUseOnlyGenius"                      , "yagleUseOnlyGenius"             , bool_init_check}, 
    {"YAGLE_USESTMSOLVER"             , __YAGLE_USESTMSOLVER            , 0, 1, 0, "yagUseStmSolver"                       , "yagleUseStmSolver"              , bool_init_check}, 
    {"YAGLE_INTERFACE_VECTORS"        , __YAGLE_INTERFACE_VECTORS       , 0, 1, 0, "yagReorderInterfaceVectors"            , NULL                             , bool_init_check}, 
    {"YAGLE_STAT_MODE"                , __YAGLE_STAT_MODE               , 1, 1, 0, "yagWriteStatistics"                    , "yagleWriteStatistics"           , bool_init_check},
    {"YAGLE_GUESS_MUTEX"              , __YAGLE_GUESS_MUTEX             , 0, 1, 0, "yagMutexHelp"                          , NULL                             , bool_init_check},
    {"YAGLE_HIER_LOTRS_GROUP"         , __YAGLE_HIER_LOTRS_GROUP        , 0, 1, 0, "yagHierarchyGroupTransistors"          , NULL                             , bool_init_check},
    {"VERILOG_KEEP_NAMES"             , __VERILOG_KEEP_NAMES            , 0, 1, 0, "avtVerilogKeepNames"                   , NULL                             , bool_init_check},
    {"STB_SHOWME"                     , __STB_SHOWME                    , 0, 1, 0, "stbHelpForSetup"                       , NULL                             , bool_init_check}, 
    {"STB_ENBALE_COMMAND_CHECK"       , __STB_ENBALE_COMMAND_CHECK      , 0, 1, 0, "stbEnableCommandCheck"                 , NULL                             , bool_init_check}, 
    {"STB_SETUP_HOLD_UPDATE"          , __STB_SETUP_HOLD_UPDATE         , 1, 0, 0, "stbSetupHoldUpdate"                    , NULL                             , bool_init_check}, 
    {"AVT_NEW_SWITCH_MODEL"           , __AVT_NEW_SWITCH_MODEL          , 0, 0, 0, "avtNewSwitchModel"                     , NULL                             , bool_init_check},
    {"AVT_SYMETRICAL_SWITCH"          , __AVT_SYMETRICAL_SWITCH         , 0, 0, 0, "avtSymetricalSwitch"                   , NULL                             , bool_init_check},
    {"AVT_USEHT_V2"                   , __AVT_USEHT_V2                  , 0, 0, 0, "avtUseHtV2"                            , NULL                             , bool_init_check},
    {"TAS_REFINE_DELAYS"              , __TAS_RECOMPUTE_DELAYS          , 0, 1, 0, "tasRefineDelays"                       , NULL                             , bool_init_check},
    {"MBK_FILTER_MASK_ERROR"          , __MBK_FILTER_MASK_ERROR         , 1, 0, 0, "avtFilterMaskError"                    , NULL                             , bool_init_check},
    {"MCC_ALLOW_NEGATIVE_B"           , __MCC_ALLOW_NEGATIVE_B          , 0, 0, 0, "mccAllowNegativeB"                     , NULL                             , bool_init_check},
    {"MCC_ALLOW_NEGATIVE_B_BRANCH"    , __MCC_ALLOW_NEGATIVE_B_BRANCH   , 0, 0, 0, "mccAllowNegativeBForBranch"            , NULL                             , bool_init_check},
    {"STM_RDRIVER_FROM_SLOPE"         , __STM_RDRIVER_FROM_SLOPE        , 1, 0, 0, "stmRdriverFromSlope"                   , NULL                             , bool_init_check},
    {"STM_CEQRSAT_NUMERIC"            , __STM_CEQRSAT_NUMERIC           , 0, 0, 0, "stmCeqrsatNumeric"                     , NULL                             , bool_init_check},
    {"MCC_CALCUL_VT_NEW"              , __MCC_CALCUL_VT_NEW             , 0, 0, 0, "mccCalculVtNew"                        , NULL                             , bool_init_check},
    {"STM_ENABLE_DV"                  , __STM_ENABLE_DV                 , 1, 0, 0, "stmEnableDv"                           , NULL                             , bool_init_check},
    {"STM_NEW_THRESHOLD"              , __STM_NEW_THRESHOLD             , 0, 0, 0, "stmNewThreshold"                       , NULL                             , bool_init_check},
    {"MCC_ACCURATE_RLIN_THRESHOLD"    , __MCC_ACCURATE_RLIN_THRESHOLD   , 1, 0, 0, "stmAccurateRlinThreshold"              , NULL                             , bool_init_check},
    {"AVT_BUG_RDRIVER"                , __AVT_BUG_RDRIVER               , 1, 0, 0, "avtBugRdriver"                         , NULL                             , bool_init_check},
    {"AVT_RDRIVER_UNDER_VSAT"         , __AVT_RDRIVER_UNDER_VSAT        , 0, 0, 0, "avtRdriverUnderVsat"                   , NULL                             , bool_init_check},
    {"STM_NUMERICAL_INTEGRATION"      , __STM_NUMERICAL_INTEGRATION     , 0, 0, 0, "stmNumericalIntegration"               , NULL                             , bool_init_check},
    {"STM_QSAT_ENHANCED"              , __STM_QSAT_ENHANCED             , 0, 0, 0, "stmQsatEnhanced"                       , NULL                             , bool_init_check},
    {"STM_CHECK_SOLUTION"             , __STM_CHECK_SOLUTION            , 0, 0, 0, "stmCheckSolution"                      , NULL                             , bool_init_check},
    {"TMA_CHARAC_PRECISION"           , __TMA_CHARAC_PRECISION          , 0, 1, 0, "tmaCharacPrecision"                    , NULL                             , bool_init_check},
    {"AVT_CTK_ON_INPUT"               , __AVT_CTK_ON_INPUT              , 1, 0, 0, "avtCtkModifyInputSlope"                , NULL                             , bool_init_check},
    {"AVT_NEW_CTK_ON_INPUT"           , __AVT_NEW_CTK_ON_INPUT          , 0, 0, 0, "avtNewCtkModifyInputSlope"             , NULL                             , bool_init_check},
    {"AVT_CTK_ON_INPUT_WITH_NET"      , __AVT_CTK_ON_INPUT_NET          , 0, 0, 0, "avtCtkModifyInputSlopeWithNet"         , NULL                             , bool_init_check},
    {"AVT_CTK_DEBUG"                  , __AVT_CTK_DEBUG                 , 0, 0, 0, "avtCtkDebug"                           , NULL                             , bool_init_check},
    {"STB_STABILITY_CORRECTION"       , __STB_STABILITY_CORRECTION      , 1, 1, 0, "stbStabilityCorrection"                , NULL                             , bool_init_check},
    {"LIB_DRIVE_COMB_AS_ACCESS"       , __LIB_DRIVE_COMB_AS_ACCESS      , 1, 0, 0, "tmaLibDriveCombAsAccess"               , NULL                             , bool_init_check},
    {"TTV_MAX_PATH_PERIOD_PRECHARGE"  , __TTV_MAX_PATH_PERIOD_PRECHARGE , 0, 1, 0, "avtTransparentPrecharge"               , NULL                             , avtTransparentPrecharge_init},  
    {"AVT_BUG_RSAT"                   , __AVT_BUG_RSAT                  , 1, 0, 0, "avtBugRsat"                            , NULL                             , bool_init_check},
    {"AVT_CAPAO_FOR_CTK"              , __AVT_CAPAO_FOR_CTK             , 0, 0, 0, "avtCapaoForCtk"                        , NULL                             , bool_init_check},
    {"AVT_PRECISE_PILOAD"             , __AVT_PRECISE_PILOAD            , 1, 0, 0, "avtPrecisePiLoad"                      , NULL                             , bool_init_check},
    {"AVT_CEQRSAT_FOR_PILOAD"         , __AVT_CEQRSAT_FOR_PILOAD        , 1, 0, 0, "avtCeqrsatForPiload"                   , NULL                             , bool_init_check},
    {"TAS_PROPAGATE_RC_ACTIVE"        , __TAS_PROPAGATE_RC_ACTIVE       , 0, 0, 0, "tasPropagateRcActive"                  , NULL                             , bool_init_check},
    {"AVT_RC_BY_TRANSITION"           , __AVT_RC_BY_TRANSITION          , 1, 0, 0, "avtRcByTransition"                     , NULL                             , bool_init_check},
    {"AVT_NUMSOL_FOR_PILOAD"          , __AVT_NUMSOL_FOR_PILOAD         , 0, 0, 0, "avtNumericalSolutionForPiLoad"         , NULL                             , bool_init_check},
    {"AVT_HACK_NETLIST"               , __AVT_HACK_NETLIST              , 0, 0, 0, "avtHackNetlist"                        , NULL                             , bool_init_check},
    {"AVT_CAPAI_KF_RC"                , __AVT_CAPAI_KF_RC               , 0, 0, 0, "avtUseKfForRcInCapai"                  , NULL                             , bool_init_check},
    {"TAS_USE_FINAL_CAPACITANCE"      , __TAS_USE_FINAL_CAPACITANCE     , 1, 0, 0, "tasUseFinalCapacitance"                , NULL                             , bool_init_check},
    {"TAS_USE_KF"                     , __TAS_USE_KF                    , 1, 0, 0, "tasUseKf"                              , NULL                             , bool_init_check},
    {"TAS_ENHANCED_CAPAI"             , __TAS_ENHANCED_CAPAI            , 0, 0, 0, "tasEnhancedCapaI"                      , NULL                             , bool_init_check},
    {"STM_QINIT_FOR_DT_CONF"          , __STM_QINIT_FOR_DT_CONF         , 0, 0, 0, "stmQinitForDtConf"                     , NULL                             , bool_init_check},
    {"STM_QINIT_FOR_OVERSHOOT_CONF"   , __STM_QINIT_FOR_OVERSHOOT_CONF  , 0, 0, 0, "stmQinitForOvershootConf"              , NULL                        , bool_init_check},
    {"STM_ENHANCED_CAPAEQ_FOR_PILOAD" , __STM_ENHANCED_CAPAEQ_FOR_PILOAD, 1, 0, 0, "stmEnhancedCapaEqForPiload"            , NULL                             , bool_init_check},
    {"STM_PILOAD_SATURATION"          , __STM_PILOAD_SATURATION         , 1, 0, 0, "stmPiloadSaturation"                   , NULL                             , bool_init_check},
    {"STM_PRECISION_WARNING"          , __STM_PRECISION_WARNING         , 0, 0, 0, "stmPrecisionWarning"                   , NULL                             , bool_init_check},
    {"CPE_PRECHARGED_MEMSYM"          , __CPE_PRECHARGED_MEMSYM         , 0, 1, 0, "cpePrechargedMemsym"                   , NULL                             , bool_init_check},
    {"AVT_ENBALE_MULTI_CON_ON_NET"    , __AVT_ENBALE_MULTI_CON_ON_NET   , 0, 1, 0, "avtEnableMultipleConnectorsOnNet"      , NULL                             , bool_init_check},
    {"MCC_USE_CACHE_PSP"              , __AVT_USE_CACHE_PSP             , 1, 1, 0, "avtUseCachePsp"                        , NULL                             , bool_init_check},
    {"TMA_ALLOW_ACCESS_AS_CLOCKPATH"  , __TMA_ALLOW_ACCESS_AS_CLOCKPATH , 0, 0, 0, "tmaAllowAccessAsClockPath"             , NULL                             , bool_init_check},
    {"LIB_DRIVE_TABLE_INDEX"          , __LIB_DRIVE_TABLE_INDEX         , 0, 0, 0, "tmaLibDriveTableIndex"                 , NULL                             , bool_init_check},
    {"AVT_ENABLE_STAT"                , __AVT_ENABLE_STAT               , 0, 0, 0, "spiActivateStatisticalFunctions"       , NULL                             , bool_init_check},
    {"STB_SYNC_SLOPES"                , __STB_SYNC_SLOPES               , 1, 0, 0, "stbSyncEdges"                          , NULL                             , bool_init_check},
    {"SPI_DRIVE_USE_UNITS"            , __SPI_DRIVE_USE_UNITS           , 1, 1, 0, "avtSpiUseUnits"                        , NULL                             , bool_init_check},
    {"TAS_ALWAYS_USE_CACHE"           , __TAS_ALWAYS_USE_CACHE          , 0, 1, 0, "tasAlwaysUseCache"                     , NULL                             , bool_init_check},
    {"TAS_STAT_HITAS_BREAK_ON_ERROR"  , __TAS_STAT_HITAS_BREAK_ON_ERROR , 0, 1, 0, "tasStatHiTasBreakOnError"              , NULL                             , bool_init_check},
    {"TAS_CHECK_IMAX"                 , __TAS_CHECK_IMAX                , 0, 1, 0, "tasCheckImax"                          , NULL                             , bool_init_check},
    {"AVT_RST_BETTER"                 , __AVT_RST_BETTER                , 0, 1, 0, "avtRstBetter"                          , NULL                             , bool_init_check},
    {"MCC_PRECISE_VT"                 , __MCC_PRECISE_VT                , 0, 1, 0, "mccPreciseVt"                          , NULL                             , bool_init_check},
    {"MCC_NEW_BEST_ABR"               , __MCC_NEW_BEST_ABR              , 0, 1, 0, "mccNewBestAbr"                         , NULL                             , bool_init_check},
    {"MCC_FIT_VT_THRESHOLD"           , __MCC_FIT_VT_THRESHOLD          , 0, 1, 0, "mccFitVtThreshold"                     , NULL                             , bool_init_check},
    {"MCC_NEW_DIFFMODELSAT"           , __MCC_NEW_DIFFMODELSAT          , 0, 1, 0, "mccNewDiffModelSat"                    , NULL                             , bool_init_check},
    {"MCC_PRECISE_K"                  , __MCC_PRECISE_K                 , 0, 1, 0, "mccPreciseK"                           , NULL                             , bool_init_check},
    {"MCC_TEST_B_NEG"                 , __MCC_TEST_B_NEG                , 0, 1, 0, "mccTestBNeg"                           , NULL                             , bool_init_check},
    {"TAS_USE_BSIM_CURRENT"           , __TAS_USE_BSIM_CURRENT          , 0, 1, 0, "tasUseBsimCurrent"                     , NULL                             , bool_init_check},
    {"MCC_NEW_IDS_SAT"                , __MCC_NEW_IDS_SAT               , 0, 1, 0, "mccNewIdsSat"                          , NULL                             , bool_init_check},
    {"AVT_ALLOW_OVERWRITE_FILE"       , __AVT_ALLOW_OVERWRITE_FILE      , 0, 0, 0, "avtAllowOverwriteFile"                 , NULL                             , bool_init_check},
    {"TAS_CHECK_BRANCH_MODEL"         , __TAS_CHECK_BRANCH_MODEL        , 0, 0, 0, "tasCheckBranchModel"                   , NULL                             , bool_init_check},
    {"TAS_NEW_CURRENT_MODEL"          , __TAS_NEW_CURRENT_MODEL         , 0, 0, 0, "tasNewCurrentModel"                    , NULL                             , setnewcurrentmodel},
    {"AVT_OLD_FEATURE_27"             , __AVT_OLD_FEATURE_27            , 0, 0, 0, "avtOldFeature27"                       , NULL                             , bool_init_check},
    {"AVT_OLD_FEATURE_28"             , __AVT_OLD_FEATURE_28            , 0, 0, 0, "avtOldFeature28"                       , NULL                             , bool_init_check},
    {"AVT_OLD_FEATURE_29"             , __AVT_OLD_FEATURE_29            , 1, 0, 0, "avtOldFeature29"                       , NULL                             , bool_init_check},
    {"AVT_OLD_FEATURE_30"             , __AVT_OLD_FEATURE_30            , 0, 0, 0, "avtOldFeature30"                       , NULL                             , bool_init_check},
    {"AVT_FALSEPATH_OPTIM"            , __AVT_FALSEPATH_OPTIM           , 1, 0, 0, "avtFalsepathOptim"                     , NULL                             , bool_init_check},
    {"TAS_USE_ALL_CAPAI"              , __TAS_USE_ALL_CAPAI             , 0, 0, 0, "tasUseAllCapai"                        , NULL                             , bool_init_check},
    {"STB_CREATE_EQUIV_GROUPS"        , __STB_CREATE_EQUIV_GROUPS       , 1, 0, 0, "stbCreateEquivGroups"                  , NULL                             , bool_init_check},
    {"STB_HANDLE_FALSE_ACCESS"        , __STB_HANDLE_FALSE_ACCESS       , 1, 0, 0, "stbCheckFalseAccess"                   , NULL                             , bool_init_check},
    {"AVT_CORRECT_CGP"                , __AVT_CORRECT_CGP               , 0, 0, 0, "avtCorrectCgp"                         , NULL                             , bool_init_check},
    {"SPI_REPLACE_V_IN_EXPR"          , __SPI_REPLACE_V_IN_EXPR         , 0, 0, 0, "avtSpiReplaceTensionInExpressions"     , "avtSpiReplaceVoltageInExpressions" , bool_init_check},
    {"STM_NEW_OVERSHOOT"              , __STM_NEW_OVERSHOOT             , 0, 0, 0, "stmNewOvershoot"                       , NULL                             , bool_init_check},
    {"SIM_ALLOW_THREADS"              , __SIM_ALLOW_THREADS             , 1, 0, 0, "simAllowThreads"                       , NULL                             , bool_init_check},
    {"SIM_USE_SYSTEM_CMD"             , __SIM_USE_SYSTEM_CMD            , 0, 0, 0, "simUseSystemCommand"                   , NULL                             , bool_init_check},
    {"MGL_NONBLOKING_ASSIGN"          , __MGL_NONBLOKING_ASSIGN         , 1, 1, 0, "avtNonBlockingVerilogAssignment"       , NULL                             , bool_init_check}, 
    {"MGL_OLD_STYLE_BUS"              , __MGL_OLD_STYLE_BUS             , 1, 0, 0, "avtVerilogOldStyleBus"                 , NULL                             , bool_init_check}, 
    {"MGL_TRISTATE_IS_MEMORY"         , __MGL_TRISTATE_IS_MEMORY        , 0, 0, 0, "avtVerilogTristateIsMemory"            , NULL                             , bool_init_check}, 
    {"MGL_INERTIAL_MEMORY"            , __MGL_INERTIAL_MEMORY           , 0, 0, 0, "avtVerilogInertialMemory"              , NULL                             , bool_init_check}, 
    {"MGL_DRIVE_CONFLICT"             , __MGL_DRIVE_CONFLICT            , 0, 0, 0, "avtVerilogDriveConflict"               , NULL                             , bool_init_check}, 
    {"YAG_DRIVE_ALIASES"              , __YAG_DRIVE_ALIASES             , 0, 1, 0, "yagDriveAliases"                       , NULL                             , bool_init_check}, 
    {"TRC_FORCE_PILOAD"               , __TRC_FORCE_PILOAD              , 0, 0, 0, "tasForcePILoad"                        , NULL                             , bool_init_check}, 
    {"TAS_MODEL_MEMSYM"               , __TAS_MODEL_MEMSYM              , 1, 0, 0, "tasModelMemsym"                        , NULL                             , bool_init_check}};

#define V_BOOL_TAB_SIZE (sizeof(V_BOOL_TAB)/(sizeof(V_BOOL))) 

//----------------------------------------------------------------------
// STR
//----------------------------------------------------------------------
extern int mbk_decodvectorconfig( char *env );

int vectorize_init_check (char *var, char *val, char *result) 
{ 
  if (!mbk_decodvectorconfig(val))
  {
      avt_errmsg (AVT_ERRMSG, "040", AVT_WARNING, val, var);
      return 0;
  }
  return 1;
}

int tasSetupLatchingVoltage_init_check (char *var, char *val, char *result)
{
    if (!strcmp(val, "vdd") || !strcmp(val, "vdd-div-2") || !strcmp(val, "max-vt-vdd-div-2") || !strcmp(val, "vt")){
        return 1;
    }else{
        avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
        return 0;
    }
}

int tma_Leakage_Power_Unit_init_check (char *var, char *val, char *result)
{
    if (!strcmp(val, "1mW")
     || !strcmp(val, "100uW")
     || !strcmp(val, "10uW")
     || !strcmp(val, "1uW")
     || !strcmp(val, "100nW")
     || !strcmp(val, "10nW")
     || !strcmp(val, "1nW")
     || !strcmp(val, "100pW")
     || !strcmp(val, "10pW")
     || !strcmp(val, "1pW")){
        return 1;
    }else{
        avt_errmsg (AVT_ERRMSG, "063", AVT_WARNING, val, var);
        return 0;
    }
}

V_STR V_STR_TAB[] = {
// Index Value Doc Set Variable
    {"AVT_ERROR_POLICY"           , __AVT_ERROR_POLICY           , "strict"                        , 1, 0, "avtErrorPolicy"                  , NULL, NULL},
    {"API_LIBS"                   , __API_LIBS                   , "*.so"                          , 0, 0, "apiDynamicLibraries"             , NULL, NULL},
    {"API_LIB_PATH"               , __API_LIB_PATH               , "nodefaultvalue"                , 0, 0, "apiDynamicLibrariesPath"         , NULL, NULL},
    {"BGL_FILE_SUFFIX"            , __BGL_FILE_SUFFIX            , "v"                             , 0, 0, "avtBehavioralVerilogSuffix"      , NULL, NULL},
    {"MBK_SPI_PARSE_FIRSTLINE"    , __MBK_SPI_PARSE_FIRSTLINE    , "include"                       , 1, 0, "avtSpiParseFirstLine"            , NULL, NULL}, 
    {"BVL_FILE_SUFFIX"            , __BVL_FILE_SUFFIX            , "vhd"                           , 0, 0, "avtBehavioralVhdlSuffix"         , NULL, NULL},
    {"MBK_BLACKBOX_NAME"          , __MBK_BLACKBOX_NAME          , "BLACKBOX"                      , 1, 0, "avtBlackboxFile"                 , NULL, NULL},
    {"MBK_CATAL_NAME"             , __MBK_CATAL_NAME             , "CATAL"                         , 1, 0, "avtCatalogueName"                , NULL, NULL},
    {"MBK_INPUT_VECTOR"           , __MBK_INPUT_VECTOR           , "[],<>"                         , 1, 0, "avtVectorize"                    , NULL, vectorize_init_check},
    {"ELP_GEN_TECHNO_NAME"        , __ELP_GEN_TECHNO_NAME        , "techno.elp"                    , 1, 0, "avtElpGenTechnoName"             , NULL, NULL},
    {"ELP_TECHNO_NAME"            , __ELP_TECHNO_NAME            , "/etc/prol05.elp"               , 0, 0, "avtElpTechnoName"                , NULL, NULL},
    {"AVT_GNUPLOT_TERM"           , __AVT_GNUPLOT_TERM           , "x11"                           , 0, 0, "avtGnuplotTerm"                  , NULL, NULL},
    {"MBK_FILTER_SFX"             , __MBK_FILTER_SFX             , NULL                            , 1, 0, "avtFilterSuffix"                 , NULL, NULL},
    {"MBK_GLOBAL_VDD"             , __MBK_GLOBAL_VDD             , NULL                            , 1, 0, "avtGlobalVddName"                , NULL, NULL},
    {"MBK_GLOBAL_VSS"             , __MBK_GLOBAL_VSS             , NULL                            , 1, 0, "avtGlobalVssName"                , NULL, NULL},
    {"BEH_IN_FORMAT"              , __BEH_IN_FORMAT              , "vhd"                           , 0, 0, "avtInputBehaviorFormat"          , NULL, NULL},
    {"MBK_IN_FILTER"              , __MBK_IN_FILTER              , NULL                            , 1, 0, "avtInputFilter"                  , NULL, NULL},
    {"MBK_IN_LO"                  , __MBK_IN_LO                  , "spi"                           , 0, 0, "avtInputNetlistFormat"           , NULL, NULL},
    {"MBK_IN_PARASITICS"          , __MBK_IN_PARASITICS          , "dspf"                          , 0, 0, "avtInputParasiticNetlistFormat"  , NULL, NULL},
    {"MBK_SEPAR"                  , __MBK_SEPAR                  , "."                             , 1, 0, "avtInstanceSeparator"            , NULL, NULL},
    {"BEG_USER_WAY"               , __BEG_USER_WAY               , "downto"                        , 1, 0, "avtOutputBehaviorVectorDirection", NULL, NULL},
    {"AVT_LANGUAGE"               , __AVT_LANGUAGE               , "english"                       , 0, 0, "avtLanguage"                     , NULL, NULL},
    {"MBK_CATA_LIB"               , __MBK_CATA_LIB               , "."                             , 1, 0, "avtLibraryDirs"                  , NULL, NULL},
    {"MBK_LIB_FILE"               , __MBK_LIB_FILE               , NULL                            , 0, 0, "avtLibraryFile"                  , NULL, NULL},
    {"MBK_PASSWORD"               , __MBK_PASSWORD               , NULL                            , 0, 0, "avtPassword"                     , NULL, NULL},
    {"AVT_LICENSE_FILE"           , __AVT_LICENSE_FILE           , "avtlicense.lic"                , 1, 0, "avtLicenseFile"                  , NULL, NULL},
    {"LM_PROJECT"                 , __LM_PROJECT                 , "UNKN"                          , 1, 0, "avtLicenseProject"               , NULL, NULL},
    {"API_DRIVE_ALL_BEH"          , __API_DRIVE_ALL_BEH          , "no"                            , 1, 0, "apiDriveAllBehavior"             , NULL, NULL},
    {"MBK_CASE_SENSITIVE"         , __MBK_CASE_SENSITIVE         , "preserve"                      , 1, 0, "avtCaseSensitive"                , NULL, NULL},
    {"API_FLAGS"                  , __API_FLAGS                  , NULL                            , 1, 0, "apiFlags"                        , NULL, NULL},
    {"AVT_LICENSE_SERVER"         , __AVT_LICENSE_SERVER         , NULL                            , 1, 0, "avtLicenseServer"                , NULL, NULL},
    {"ELP_LOAD_FILE"              , __ELP_LOAD_FILE              , "no"                            , 0, 0, "avtLoadSpecifiedElpFile"         , NULL, NULL},
    {"AVT_GLOBALLOGFILE"          , __AVT_GLOBALLOGFILE          , NULL                            , 1, 0, "avtLogFile"                      , NULL, NULL},
    {"AVT_LOGLEVEL"               , __AVT_LOGLEVEL               , NULL                            , 1, 0, "avtLogEnable"                    , NULL, NULL},
    {"AVT_WARN_FILTER"            , __AVT_WARN_FILTER            , NULL                            , 1, 0, "avtWarningFilter"                , NULL, NULL},
    {"BEH_OUT_FORMAT"             , __BEH_OUT_FORMAT             , "vhd"                           , 0, 0, "avtOutputBehaviorFormat"         , NULL, NULL},
    {"MBK_OUT_FILTER"             , __MBK_OUT_FILTER             , NULL                            , 1, 0, "avtOutputFilter"                 , NULL, NULL},
    {"MBK_OUT_LO"                 , __MBK_OUT_LO                 , "spi"                           , 1, 0, "avtOutputNetlistFormat"          , NULL, NULL},
    {"AVT_INF"                    , __AVT_INF                    , "$.spice.inf,$.sdc.inf,$.inf"   , 0, 0, "avtReadInformationFile"          , NULL, NULL},
    {"SIM_SPICE_OUT"              , __SIM_SPICE_OUT              , "$.out"                         , 1, 0, "avtSpiceOutFile"                 , NULL, NULL},
    {"SIM_SPICE_STDOUT"           , __SIM_SPICE_STDOUT           , "$.out"                         , 1, 0, "avtSpiceStdoutFile"              , NULL, NULL},
    {"SIM_SPICE_STRING"           , __SIM_SPICE_STRING           , "spice $"                       , 1, 0, "avtSpiceString"                  , NULL, NULL},
    {"MBK_SPI_SEPAR"              , __MBK_SPI_SEPAR              , "."                             , 1, 0, "avtSpiConnectorSeparator"        , NULL, NULL},
    {"MBK_SPI_DN"                 , __MBK_SPI_DN                 , NULL                            , 1, 0, "avtSpiDnModelName"               , NULL, avt_init_model_dn},
    {"MBK_SPI_DP"                 , __MBK_SPI_DP                 , NULL                            , 1, 0, "avtSpiDpModelName"               , NULL, avt_init_model_dp},
    {"MBK_SPICE_DRIVER_FLAGS"     , __MBK_SPICE_DRIVER_FLAGS     , NULL                            , 1, 0, "avtSpiFlags"                     , NULL, NULL},
    {"MBK_DRIVE_DEFAULT_UNITS"    , __MBK_DRIVE_DEFAULT_UNITS    , NULL                            , 1, 0, "avtSpiDriveDefaultUnits"         , NULL, NULL},
    {"RCN_CACHESIZE"              , __RCN_CACHESIZE              , "10Mb"                          , 1, 0, "avtParasiticCacheSize"           , NULL, avtParasiticCacheSize_init},
    {"MBK_SPICE_KEEP_CARDS"       , __MBK_SPICE_KEEP_CARDS       , NULL                            , 1, 0, "avtSpiKeepCards"                 , NULL, NULL},
    {"MBK_SPICE_KEEP_NAMES"       , __MBK_SPICE_KEEP_NAMES       , "Transistor"                    , 1, 0, "avtSpiKeepNames"                 , NULL, NULL},
    {"MGL_FILE_SUFFIX"            , __MGL_FILE_SUFFIX            , "v"                             , 0, 0, "avtStructuralVerilogSuffix"      , NULL, NULL},
    {"MBK_SPI_TOLERANCE"          , __MBK_SPI_TOLERANCE          , "low"                           , 1, 0, "avtSpiTolerance"                 , NULL, NULL},
    {"MBK_SPI_NETNAME"            , __MBK_SPI_NETNAME            , "NET"                           , 0, 0, "avtSpiNetName"                   , NULL, NULL},
    {"MBK_SPI_COMPOSE_NAME_SEPAR" , __MBK_SPI_COMPOSE_NAME_SEPAR , "_"                             , 1, 0, "avtSpiNodeSeparator"             , NULL, NULL},
    {"MBK_SPI_SUFFIX"             , __MBK_SPI_SUFFIX             , "spi"                           , 0, 0, "avtSpiSuffix"                    , NULL, NULL},
    {"MBK_SPI_TN"                 , __MBK_SPI_TN                 , NULL                            , 1, 0, "avtSpiTnModelName"               , NULL, avt_init_model_tn},
    {"MBK_SPI_TP"                 , __MBK_SPI_TP                 , NULL                            , 1, 0, "avtSpiTpModelName"               , NULL, avt_init_model_tp},
    {"MBK_SPI_VECTOR"             , __MBK_SPI_VECTOR             , "_"                             , 1, 0, "avtSpiVector"                    , NULL, NULL},
    {"MVL_FILE_SUFFIX"            , __MVL_FILE_SUFFIX            , "vhd"                           , 1, 0, "avtStructuralVhdlSuffix"         , NULL, NULL},
    {"MCC_MODEL_FILE"             , __MCC_MODEL_FILE             , NULL                            , 0, 0, "avtTechnologyName"               , NULL, NULL},
    {"MCC_MODEL_FILE_BEST"        , __MCC_MODEL_FILE_BEST        , NULL                            , 0, 0, "avtTechnoBestCornerName"         , NULL, NULL},
    {"MCC_MODEL_FILE_WORST"       , __MCC_MODEL_FILE_WORST       , NULL                            , 0, 0, "avtTechnoWorstCornerName"        , NULL, NULL},
    {"MCC_EXTTECHNO"              , __MCC_EXTTECHNO              , "AVTCMI"                        , 0, 0, "avtExternalTechnology"           , NULL, NULL},
    {"MBK_VSS"                    , __MBK_VSS                    , NULL                            , 1, 0, "avtVssName"                      , NULL, NULL},
    {"MBK_WORK_LIB"               , __MBK_WORK_LIB               , "."                             , 0, 0, "avtWorkDir"                      , NULL, NULL},
    {"FCL_ANYNULL "               , __FCL_ANY_NMOS               , "tn:TN"                         , 1, 0, "fclGenericNMOS"                  , NULL, NULL},
    {"FCL_ANY_PMOS"               , __FCL_ANY_PMOS               , "tp:TP"                         , 1, 0, "fclGenericPMOS"                  , NULL, NULL},
    {"FCL_LIB_PATH"               , __FCL_LIB_PATH               , NULL                            , 1, 0, "fclLibraryDir"                   , NULL, NULL},
    {"FCL_LIB_NAME"               , __FCL_LIB_NAME               , "LIBRARY"                       , 1, 0, "fclLibraryName"                  , NULL, NULL},
    {"MCC_MOD_SEPAR"              , __MCC_MOD_SEPAR              , "."                             , 1, 0, "avtTechnoModelSeparator"         , NULL, NULL},
    {"MBK_VDD"                    , __MBK_VDD                    , NULL                            , 1, 0, "avtVddName"                      , NULL, NULL},
    {"GENIUS_LIB_PATH"            , __GENIUS_LIB_PATH            , NULL                            , 1, 0, "gnsLibraryDir"                   , NULL, NULL},
    {"GENIUS_LIB_NAME"            , __GENIUS_LIB_NAME            , "LIBRARY"                       , 1, 0, "gnsLibraryName"                  , NULL, NULL},
    {"AVT_TEMPLATE_DIR"           , __AVT_TEMPLATE_DIR           , NULL                            , 1, 0, "gnsTemplateDir"                  , NULL, NULL},
    {"GEN_DEBUG_MODE"             , __GEN_DEBUG_MODE             , "stdout"                        , 1, 0, "gnsTraceFile"                    , NULL, NULL},
    {"GEN_SPY"                    , __GEN_SPY                    , NULL                            , 1, 0, "gnsTraceModel"                   , NULL, NULL},
    {"RCX_CTK_MODEL"              , __RCX_CTK_MODEL              , "MILLER_NOMINAL"                , 1, 0, "rcxCtkModel"                     , NULL, NULL},
    {"RCX_CTK_NOISE"              , __RCX_CTK_NOISE              , "NOISE_FINE"                    , 1, 0, "rcxCtkNoise"                     , NULL, NULL},
    {"RCX_CTK_SLOPE_DELAY"        , __RCX_CTK_SLOPE_DELAY        , "SLOPE_DELAY_CTK"               , 1, 0, "rcxCtkSlopeDelay"                , NULL, NULL},
    {"RCX_CTK_SLOPE_NOISE"        , __RCX_CTK_SLOPE_NOISE        , "SLOPE_CTK"                     , 1, 0, "rcxCtkSlopeNoise"                , NULL, NULL},
    {"RCX_DELAY_CACHE"            , __RCX_DELAY_CACHE            , "10mb"                          , 1, 0, "rcxDelayCacheSize"               , NULL, NULL},
    {"SIM_OUTLOAD"                , __SIM_OUTLOAD                , "transistor"                    , 1, 0, "simOutLoad"                      , NULL, NULL},
    {"EFG_SIG_ALIAS"              , __EFG_SIG_ALIAS              , NULL                            , 1, 0, "simSignalAlias"                  , NULL, NULL},
    {"SIM_SPICE_OPTIONS"          , __SIM_SPICE_OPTIONS          , NULL                            , 1, 0, "simSpiceOptions"                 , NULL, NULL},
    {"SIM_TECH_FILE"              , __SIM_TECH_FILE              , "avtdefault.tec"                , 1, 0, "simTechnologyName"               , NULL, NULL},
    {"TMA_DRIVEDELAY"             , __TMA_DRIVEDELAY             , "max"                           , 1, 0, "tmaDriveDelay"                   , NULL, NULL},
    {"STB_FILE_FORMAT"            , __STB_FILE_FORMAT            , "stb"                           , 0, 0, "stbFileFormat"                   , NULL, NULL},
    {"STB_MONOPHASE"              , __STB_MONOPHASE              , "transparent"                   , 1, 0, "stbMonoPhase"                    , NULL, NULL},
    {"STB_FOREIGN_TIME_UNIT"      , __STB_FOREIGN_TIME_UNIT      , "1ps 1pf"                       , 1, 0, "sdcUnits"                        , "stbForeignTimeUnit", NULL},
    {"STB_FOREIGN_CONSTRAINT_FILE", __STB_FOREIGN_CONSTRAINT_FILE, ""                              , 0, 0, "stbForeignConstraintFile"        , NULL, NULL},
    {"TAS_FIG_NAME"               , __TAS_FIG_NAME               , NULL                            , 1, 0, "tasFigName"                      , NULL, NULL},
    {"TAS_FILE_NAME"              , __TAS_FILE_NAME              , NULL                            , 0, 0, "tasFileName"                     , NULL, NULL},
    {"TAS_DELAY_SWITCH"           , __TAS_DELAY_SWITCH           , "no"                            , 0, 0, "tasDelaySwitch"                  , NULL, NULL},
    {"TAS_RCX_DRIVER"             , __TAS_RCX_DRIVER             , NULL                            , 1, 0, "tasRCDriverCalcMode"             , NULL, NULL},
    {"TMA_CUNIT"                  , __TMA_CUNIT                  , "ff"                            , 1, 0, "tmaCapacitanceUnit"              , NULL, NULL},
    {"TMA_FILEIN"                 , __TMA_FILEIN                 , NULL                            , 0, 0, "tmaInputFile"                    , NULL, NULL},
    {"TMA_READFILE"               , __TMA_READFILE               , NULL                            , 1, 0, "tmaLibraryFile"                  , NULL, NULL},
    {"TMA_NAMEIN"                 , __TMA_NAMEIN                 , NULL                            , 0, 0, "tmaInputName"                    , NULL, NULL},
    {"LIB_BUS_DELIMITER"          , __LIB_BUS_DELIMITER          , "[]"                            , 1, 0, "tmaLibBusDelimiter"              , NULL, NULL},
    {"TMA_LIBRARY"                , __TMA_LIBRARY                , NULL                            , 1, 0, "tmaLibraryName"                  , NULL, NULL},
    {"TMA_OUTPUT"                 , __TMA_OUTPUT                 , "lib"                           , 0, 0, "tmaOutputFile"                   , NULL, NULL},
    {"TMA_TUNIT"                  , __TMA_TUNIT                  , "ps"                            , 1, 0, "tmaTimeUnit"                     , NULL, NULL},
    {"TMA_PUNIT"                  , __TMA_PUNIT                  , "1uW"                           , 1, 0, "tmaLeakagePowerUnit"                    , NULL, tma_Leakage_Power_Unit_init_check},
    {"EFG_TRS_ALIAS"              , __EFG_TRS_ALIAS              , NULL                            , 1, 0, "simTransistorAlias"              , NULL, NULL},
    {"SIM_MEAS_CMD"               , __SIM_MEAS_CMD               , NULL                            , 1, 0, "simMeasCmd"                      , NULL, NULL},
    {"YAGLE_DEBUG_CONE"           , __YAGLE_DEBUG_CONE           , NULL                            , 1, 0, "yagDebugCone"                    , "yagleDebugCone", NULL},
    {"YAGLE_AUTO_ASYNC"           , __YAGLE_AUTO_ASYNC           , "no"                            , 1, 0, "yagSetResetDetection"            , NULL, NULL}, 
    {"YAGLE_AUTO_RS"              , __YAGLE_AUTO_RS              , "yes"                           , 1, 0, "yagAutomaticRSDetection"         , NULL, NULL}, 
    {"YAGLE_AUTO_MEMSYM"          , __YAGLE_AUTO_MEMSYM          , "no"                            , 1, 0, "yagAutomaticMemsymDetection"     , NULL, NULL}, 
    {"YAGLE_SIMPLE_LATCH"         , __YAGLE_SIMPLE_LATCH         , "no"                            , 1, 0, "yagSimpleLatchDetection"         , NULL, yagSimpleLatchDetection_init}, 
    {"YAGLE_CLOCK_GATE"           , __YAGLE_CLOCK_GATE           , "no"                            , 1, 0, "yagDetectClockGating"            , NULL, yagDetectClockGating_init}, 
    {"YAGLE_FIGNAME"              , __YAGLE_FIGNAME              , NULL                            , 0, 0, "yagFigureName"                   , "yagleFigureName", NULL},
    {"YAGLE_FILENAME"             , __YAGLE_FILENAME             , NULL                            , 0, 0, "yagFileName"                     , "yagleFileName", NULL},
    {"YAGLE_GNSROOTNAME"          , __YAGLE_GNSROOTNAME          , NULL                            , 1, 0, "yagGeniusTopName"                , "yagleGeniusTopName", NULL},
    {"YAGLE_OUTNAME"              , __YAGLE_OUTNAME              , "$"                             , 1, 0, "yagOutputName"                   , "yagleOutputName", NULL},
    {"GENIUS_FLAGS"               , __GENIUS_FLAGS               , NULL                            , 1, 0, "gnsFlags"                        , NULL, NULL},
    {"FCL_SHARE"                  , __FCL_SHARE                  , "?"                             , 1, 0, "fclAllowSharing"                 , NULL, NULL},
    {"RCX_USING_AWEMATRIX"        , __RCX_USING_AWEMATRIX        , NULL                            , 1, 0, "rcxAweMatrix"                    , NULL, NULL},
    {"SLIB_LIBRARY_NAME"          , __SLIB_LIBRARY_NAME          , ""                              , 1, 0, "xyagIconLibrary"                 , NULL, NULL},
    {"SIM_EXTRACT_RULE"           , __SIM_EXTRACT_RULE           , NULL                            , 1, 0, "simExtractRule"                  , NULL, NULL},
    {"STM_CACHESIZE"              , __STM_CACHESIZE              , NULL                            , 1, 0, "stmCacheSize"                    , NULL, NULL},  
    {"ANNOT_KEEP_CARDS"           , __ANNOT_KEEP_CARDS           , "all"                           , 1, 0, "avtAnnotationKeepCards"          , NULL, NULL},
    {"ANNOT_CONNECTOR_SETTING"    , __ANNOT_CONNECTOR_SETTING    , NULL                            , 1, 0, "avtAnnotationDeviceConnectorSetting", NULL, avtAnnotationDeviceConnectorSetting_init},
    {"TAS_DEBUG_LOTRS"            , __TAS_DEBUG_LOTRS            , NULL                            , 0, 0, "tasDebugLotrs"                   , NULL, NULL},
    {"SIMUINV"                    , __TAS_SIMU_INVERTER          , NULL                            , 0, 0, "tasSimulateInverter"             , NULL, tasSimulateInverter_init},
    {"SIMUINV_CONFIG"             , __TAS_SIMU_INVERTER_CONFIG   , NULL                            , 0, 0, "tasSimulateInverterConfig"       , NULL, tpiv_inverter_config_init},
    {"DEBUG_PWL"                  , __TAS_DEBUG_PWL              , NULL                            , 0, 0, "tasDebugPwl"                     , NULL, NULL},
    {"AVT_HACK_NETLIST_CONFIG"    , __AVT_HACK_NETLIST_CONFIG    , NULL                            , 0, 0, "avtHackNetlistConfig"            , NULL, NULL},
    {"TTV_IGNORE_MAX_FILTER"      , __TTV_IGNORE_MAX_FILTER      , NULL                            , 0, 0, "ttvIgnoreMaxFilter"              , NULL, ttvIgnoreMaxFilter_init},
    {"AVT_EXCLUDE_COMPRESSION"    , __AVT_EXCLUDE_COMPRESSION    , NULL                            , 0, 0, "avtDisableCompression"           , NULL, NULL},
    {"YAGLE_TAS_TIMING"           , __YAGLE_TAS_TIMING           , "0"                             , 1, 0, "yagTasTiming"                    , "yagleTasTiming", NULL},
    {"TAS_SETUP_LATCHING_VOLTAGE" , __TAS_SETUP_LATCHING_VOLTAGE , "vt"                            , 1, 0, "tasSetupLatchingVoltage"         , NULL, tasSetupLatchingVoltage_init_check}};

#define V_STR_TAB_SIZE (sizeof(V_STR_TAB)/(sizeof(V_STR))) 

//----------------------------------------------------------------------
// FLOAT
//----------------------------------------------------------------------

int margin_check (char *var, char *val, float *result) 
{
  *result=atof(val);
  if (*result>= 0.0 && *result <= 1.0) return 1;

  avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
  return 0;
}

int libslewderate_check (char *var, char *val, float *result) 
{
  *result=atof(val);
  if (*result> 0.0 && *result <= 1.0) return 1;

  avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
  return 0;
}

int yagpullupratio_check (char *var, char *val, float *result) 
{
  *result=atof(val);
  if (*result> 1.0) return 1;

  avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
  return 0;
}

V_FLOAT V_FLOAT_TAB[] = {
// Index Value Doc Set Variable
    {"MCC_TEMP_BEST"              , __MCC_TEMP_BEST              , 70.0        ,             0, 0, "avtCharacBestTemp"                         , NULL, NULL, ""},
    {"MCC_VDD_BEST"               , __MCC_VDD_BEST               , 5.0         ,             0, 0, "avtCharacBestVdd"                          , NULL, NULL, ""},  
    {"MCC_RANGE_MARGIN"           , __MCC_RANGE_MARGIN           , 0.0         ,             0, 0, "avtCharacRangeMargin"                      , NULL, NULL, ""},  
    {"MCC_TEMP_WORST"             , __MCC_TEMP_WORST             , 70.0        ,             0, 0, "avtCharacWorstTemp"                        , NULL, NULL, ""},  
    {"MCC_VDD_WORST"              , __MCC_VDD_WORST              , 5.0         ,             0, 0, "avtCharacWorstVdd"                         , NULL, NULL, ""},  
    {"TUT_MAX_TEMP"               , __TUT_MAX_TEMP               , -1000.0     ,             1, 0, "avtDeratingMaxTemperature"                 , NULL, NULL, ""},  //ok 
    {"TUT_MAX_VOLT"               , __TUT_MAX_VOLT               , -1000.0     ,             1, 0, "avtDeratingMaxVoltage"                     , NULL, NULL, ""},  //ok
    {"TUT_MIN_TEMP"               , __TUT_MIN_TEMP               , -1000.0     ,             1, 0, "avtDeratingMinTemperature"                 , NULL, NULL, ""},  //ok
    {"TUT_MIN_VOLT"               , __TUT_MIN_VOLT               , -1000.0     ,             1, 0, "avtDeratingMinVoltage"                     , NULL, NULL, ""},  //ok
    {"ELP_DELVT0_MARGIN"          , __ELP_DELVT0_MARGIN          , -1.0        ,             0, 0, "avtElpDelVtMargin"                         , NULL, margin_check, ""},  //ok
    {"ELP_MULU0_MARGIN"           , __ELP_MULU0_MARGIN           , -1.0        ,             0, 0, "avtElpMuluMargin"                          , NULL, margin_check, ""},  //ok
    {"ELP_SA_MARGIN"              , __ELP_SA_MARGIN              , -1.0        ,             0, 0, "avtElpSaMargin"                            , NULL, margin_check, ""},  //ok 
    {"ELP_SB_MARGIN"              , __ELP_SB_MARGIN              , -1.0        ,             0, 0, "avtElpSbMargin"                            , NULL, margin_check, ""},  //ok
    {"ELP_SD_MARGIN"              , __ELP_SD_MARGIN              , -1.0        ,             0, 0, "avtElpSdMargin"                            , NULL, margin_check, ""},  //ok
    {"ELP_SC_MARGIN"              , __ELP_SC_MARGIN              , -1.0        ,             0, 0, "avtElpScMargin"                            , NULL, margin_check, ""},  //ok
    {"ELP_SCA_MARGIN"             , __ELP_SCA_MARGIN             , -1.0        ,             0, 0, "avtElpScaMargin"                           , NULL, margin_check, ""},  //ok
    {"ELP_SCB_MARGIN"             , __ELP_SCB_MARGIN             , -1.0        ,             0, 0, "avtElpScbMargin"                           , NULL, margin_check, ""},  //ok
    {"ELP_SCC_MARGIN"             , __ELP_SCC_MARGIN             , -1.0        ,             0, 0, "avtElpSccMargin"                           , NULL, margin_check, ""},  //ok
    {"TAS_MAXSCALE_CAPAFACTOR"    , __TAS_MAXSCALE_CAPAFACTOR    , 1.0         ,             0, 0, "avtMaxScaleCapaFactor"                     , NULL, NULL, ""},  
    {"TAS_MAXSCALE_RESIFACTOR"    , __TAS_MAXSCALE_RESIFACTOR    , 1.0         ,             0, 0, "avtMaxScaleResiFactor"                     , NULL, NULL, ""},  
    {"TAS_MINSCALE_CAPAFACTOR"    , __TAS_MINSCALE_CAPAFACTOR    , 1.0         ,             0, 0, "avtMinScaleCapaFactor"                     , NULL, NULL, ""},  
    {"TAS_MINSCALE_RESIFACTOR"    , __TAS_MINSCALE_RESIFACTOR    , 1.0         ,             0, 0, "avtMinScaleResiFactor"                     , NULL, NULL, ""},  
    {"MBK_SPI_DRIVE_CAPA_MINI"    , __MBK_SPI_DRIVE_CAPA_MINI    , 0           ,             1, 0, "avtSpiDriveCapaMini"                       , NULL, NULL, ""},  
    {"MBK_SPI_DRIVE_RESI_MINI"    , __MBK_SPI_DRIVE_RESI_MINI    , 0.001       ,             1, 0, "avtSpiDriveResiMini"                       , NULL, NULL, ""},  
    {"MBK_SPI_MAX_RESI"           , __MBK_SPI_MAX_RESI           , 0.0         ,             1, 0, "avtSpiMaxResistance"                       , NULL, NULL, ""},  
    {"MBK_SPI_MIN_CAPA"           , __MBK_SPI_MIN_CAPA           , 0.0         ,             1, 0, "avtSpiMinCapa"                             , NULL, NULL, ""},  
    {"MBK_SPI_MIN_RESI"           , __MBK_SPI_MIN_RESI           , 0.0         ,             1, 0, "avtSpiMinResistance"                       , NULL, NULL, ""},  
    {"MBK_SPI_SCALE_CAPAFACTOR"   , __MBK_SPI_SCALE_CAPAFACTOR   , 1.0         ,             0, 0, "avtSpiScaleCapaFactor"                     , NULL, NULL, ""},  
    {"MBK_SPI_SCALE_DIODEFACTOR"  , __MBK_SPI_SCALE_DIODEFACTOR  , 1.0         ,             0, 0, "avtSpiScaleDiodeFactor"                    , NULL, NULL, ""},  
    {"MBK_SPI_SCALE_RESIFACTOR"   , __MBK_SPI_SCALE_RESIFACTOR   , 1.0         ,             0, 0, "avtSpiScaleResiFactor"                     , NULL, NULL, ""},  
    {"MBK_SPI_SCALE_TRANSFACTOR"  , __MBK_SPI_SCALE_TRANSFACTOR  , 1.0         ,             0, 0, "avtSpiScaleTransistorFactor"               , NULL, NULL, ""},  
    {"SIM_TECHNO_SIZE"            , __SIM_TECHNO_SIZE            , 1.0         ,             1, 0, "avtTechnologySize"                         , NULL, NULL, ""},  
    {"MBK_VDD_VSS_THRESHOLD"      , __MBK_VDD_VSS_THRESHOLD      , 0.5         ,             1, 0, "avtVddVssThreshold"                        , NULL, NULL, ""},  
    {"STB_NOISE_DEFAULT_RESI"     , __STB_NOISE_DEFAULT_RESI     , 1000.0      ,             1, 0, "ctkNoiseDefaultResi"                       , NULL, NULL, ""},  
    {"CTK_REPORT_CTK_MIN"         , __CTK_REPORT_CTK_MIN         , 0.0         ,             1, 0, "ctkCapaMin"                                , NULL, NULL, ""},  
    {"RCX_CAPALOAD_MAXRC"         , __RCX_CAPALOAD_MAXRC         , 5.0         ,             1, 0, "rcxCapaLoadMaxRC"                          , NULL, NULL, ""},  
    {"RCX_MAXDELTALOAD"           , __RCX_MAXDELTALOAD           , 1.0         ,             1, 0, "rcxMaxDeltaLoad"                           , NULL, NULL, ""},  
    {"RCX_MINRCSIGNAL"            , __RCX_MINRCSIGNAL            , 1.0         ,             1, 0, "rcxMinRCSignal"                            , NULL, NULL, ""},  
    {"RCX_MINRELCTKFILTER"        , __RCX_MINRELCTKFILTER        , 0.05        ,             1, 0, "rcxMinRelCtkFilter"                        , NULL, NULL, ""},  
    {"RCX_MINRELCTKSIGNAL"        , __RCX_MINRELCTKSIGNAL        , 0.05        ,             1, 0, "rcxMinRelCtkSignal"                        , NULL, NULL, ""},  
    {"SIM_DC_STEP"                , __SIM_DC_STEP                , 0.001       ,             1, 0, "simDcStep"                                 , NULL, NULL, ""},  
    {"SIM_INPUT_START"            , __SIM_INPUT_START            , 0.0         ,             1, 0, "simInputStartTime"                         , NULL, avt_parse_time, ""},  
    {"SIM_OUT_CAPA_VAL"           , __SIM_OUT_CAPA_VAL           , -1          ,             0, 0, "simOutCapaValue"                           , NULL, NULL/*avt_parse_capa*/, ""},  
    {"SIM_POWER_SUPPLY"           , __SIM_POWER_SUPPLY           , 5.0         ,             1, 0, "simPowerSupply"                            , NULL, NULL, ""},  
    {"SIM_SIMU_STEP"              , __SIM_SIMU_STEP              , 1e-12       ,             1, 0, "simSimulationStep"                         , NULL, avt_parse_time, ""},  
    {"SIM_INPUT_SLOPE"            , __SIM_INPUT_SLOPE            , 200.0e-12   ,             1, 0, "simSlope"                                  , NULL, avt_parse_time, ""},  
    {"SIM_TEMP"                   , __SIM_TEMP                   , 70.0        ,             1, 0, "simTemperature"                            , NULL, NULL, ""},  
    {"SIM_TNOM"                   , __SIM_TNOM                   , 27.0        ,             0, 0, "simNominalTemperature"                     , NULL, NULL, ""},  
    {"SIM_TRAN_STEP"              , __SIM_TRAN_STEP              , 1e-12       ,             1, 0, "simTransientStep"                          , NULL, NULL/*avt_parse_time*/, ""},  
    {"SIM_TIME"                   , __SIM_TIME                   , 1e-9        ,             1, 0, "simTransientTime"                          , NULL, NULL/*avt_parse_time*/, ""},  
    {"SIM_VTH"                    , __SIM_VTH                    , 0.5         ,             0, 0, "simVth"                                    , NULL, NULL, ""},  
    {"SIM_VTH_HIGH"               , __SIM_VTH_HIGH               , 0.8         ,             1, 0, "simVthHigh"                                , NULL, NULL, ""},  
    {"SIM_VTH_LOW"                , __SIM_VTH_LOW                , 0.2         ,             1, 0, "simVthLow"                                 , NULL, NULL, ""},  
    {"STM_QSAT_RATIO"             , __STM_QSAT_RATIO             , 0.8         ,             0, 0, "stmQsatRatio"                              , NULL, NULL, ""},  
    {"FRONT_CON"                  , __FRONT_CON                  , 200         ,             0, 0, "tasConnectorsInputSlope"                   , NULL, NULL, ""},  
    {"TAS_CAPAOUT"                , __TAS_CAPAOUT                , 0.0         ,             0, 0, "tasOutputCharge"                           , NULL, NULL, ""},  
    {"TAS_CAPARAPREC"             , __TAS_CAPARAPREC             , 100         ,             1, 0, "tasPathCapacitanceFactor"                  , NULL, NULL, ""},  
    {"TAS_CAPASWITCH"             , __TAS_CAPASWITCH             , 100         ,             1, 0, "tasSwitchCapacitanceFactor"                , NULL, NULL, ""},  
    {"STB_MIN_PROBABILITY"        , __STB_MIN_PROBABILITY        , 0           ,             1, 0, "stbCtkMinOccurenceProbability"             , NULL, NULL, ""},  
    {"YAGLE_SPLITTIMING_RATIO"    , __YAGLE_SPLITTIMING_RATIO    , 0           ,             1, 0, "yagSplitTimingRatio"                       , "yagleSplitTimingRatio"    , NULL, ""},  
    {"YAGLE_SENSITIVE_RATIO"      , __YAGLE_SENSITIVE_RATIO      , 0.0         ,             1, 0, "yagSensitiveTimingRatio"                   , "yagleSensitiveTimingRatio", NULL, ""},  
    {"YAGLE_THRESHOLD"            , __YAGLE_THRESHOLD            , 3.0         ,             1, 0, "yagElectricalThreshold"                    , "yagleElectricalThreshold" , NULL, ""},
    {"YAGLE_PULL_RATIO"           , __YAGLE_PULL_RATIO           , 10.0        ,             1, 0, "yagPullupRatio"                            , NULL , yagpullupratio_check, ""},
    {"YAGLE_LATCHLOOP_RATIO"      , __YAGLE_LATCHLOOP_RATIO      , 0.0         ,             1, 0, "yagLoopOrientRatio"                        , NULL , NULL, ""},
    {"AVT_OPTIM_STEP"             , __AVT_OPTIM_STEP             , 20.0        ,             1, 0, "avtOptimStep"                              , NULL, NULL, ""},
    {"LIB_SLEW_DERATE"            , __LIB_SLEW_DERATE            , 1.0         ,             1, 0, "tmaLibSlewDerate"                          , NULL , libslewderate_check, ""},
    {"TTV_SSTA_CACHE_SIZE"        , __TTV_SSTA_CACHE_SIZE        , 524288000   ,             1, 0, "avtSSTACacheSize"                          , NULL , avt_parse_mem, ""},
    {"OVERSHOOT_K_HVT"            , __STM_OVERSHOOT_K_HVT        , 0.9         ,             1, 0, "stmOvershootKHvt"                          , NULL , NULL, ""},
    {"OVERSHOOT_K_LVT"            , __STM_OVERSHOOT_K_LVT        , 1.5         ,             1, 0, "stmOvershootKLvt"                          , NULL , NULL, ""},
    {"AVT_HACK_COEF_CCTK"         , __AVT_HACK_COEF_CCTK         , 1.0         ,             1, 0, "avtHackCoefCctc"                           , NULL , NULL, ""},
    {"YAG_MIN_RC"                 , __YAG_MIN_RC                 , 1e-12       ,             1, 0, "yagMinimumRCDelay"                         , NULL , avt_parse_time, ""},
    {"YAG_DELTA_DELAY"            , __YAG_DELTA_DELAY            , 1e-12       ,             1, 0, "yagDeltaDelay"                             , NULL , avt_parse_time, ""},
    {"YAG_RC_STEP"                , __YAG_RC_STEP                , 1e-12       ,             1, 0, "yagRCDelayStep"                            , NULL , avt_parse_time, ""},
    {"AVT_LEAKAGE_RATIO"          , __AVT_LEAKAGE_RATIO          , 0.0         ,             0, 0, "avtLeakageRatio"                           , NULL , NULL, ""},
    {"YAG_SINGLE_DELAY_RATIO"     , __YAG_SINGLE_DELAY_RATIO     , 0           ,             0, 0, "yagSingleDelayRatio"                       , NULL , NULL, ""},
    {"AVT_HACK_COEF_CGND"         , __AVT_HACK_COEF_CGND         , 1.0         ,             1, 0, "avtHackCoefCgnd"                           , NULL , NULL, ""}};


#define V_FLOAT_TAB_SIZE (sizeof(V_FLOAT_TAB)/(sizeof(V_FLOAT))) 

//----------------------------------------------------------------------
// INT
//----------------------------------------------------------------------

int level_check (char *var, char *val, int *result)
{
  *result=atoi(val);
  if (*result == 0 || *result == 1 || *result == 2) return 1;
  avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
  return 0;
}

int stbCorrelatedSkewAnalysisDepth_InitCheck (char *var, char *val, int *result)
{
  if (!strcasecmp(val,"full")) *result=1000000;
  else *result=atoi(val);
  if (*result>=0) return 1;
  avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
  return 0;
}
int stbPrechargeDataHeuristic_Check(char *var, char *val, int *result)
{
  if (!strcasecmp(val,"clock")) *result=1;
  else if (!strcasecmp(val,"notclockdata")) *result=2;
  else *result=atoi(val);
  if (*result<=2) return 1;
  avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
  return 0;
}
int stbSuppressLag_Check(char *var, char *val, int *result)
{
  char buf[1024];
  char *c, *tok;
  *result=0;
  
  strcpy(buf, val);
  tok=strtok_r(buf, "+", &c);
  while (tok!=NULL)
    {
      if (strcasecmp(tok,"latch")==0)
        *result|=AVT_STB_SUPPRESS_LAG_LATCH;
      else if (strcasecmp(tok,"precharge")==0)
        *result|=AVT_STB_SUPPRESS_LAG_PRECH;
      else if (strcasecmp(tok,"all")==0 || strcasecmp(tok,"yes")==0)
        *result|=AVT_STB_SUPPRESS_LAG_PRECH|AVT_STB_SUPPRESS_LAG_LATCH;
      else
       {
         avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, tok, var);
         return 0;
       }
      tok=strtok_r(NULL, "+", &c);
    }
  return 1;
}
int simTransistorAsInstance_InitCheck (char *var, char *val, int *result)
{
  if (!strcasecmp(val,"yes")) *result=1;
  else if (!strcasecmp(val,"no")) *result=0;
  else if (!strcasecmp(val,"smart")) *result=2;
  else
  {
    avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
    return 0;
  }
  
  return 1;
}

int tasStrictPathCapacitance_InitCheck (char *var, char *val, int *result)
{
  if (!strcasecmp(val,"yes")) *result=1;
  else if (!strcasecmp(val,"no")) *result=0;
  else if (!strcasecmp(val,"latch")) *result=2;
  else
  {
    avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
    return 0;
  }
  
  return 1;
}

int yagDriveConflictCondition_InitCheck (char *var, char *val, int *result)
{
  if (!strcasecmp(val,"yes")) *result=1;
  else if (!strcasecmp(val,"no")) *result=0;
  else if (!strcasecmp(val,"full")) *result=2;
  else
  {
    avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
    return 0;
  }
  
  return 1;
}

int avtPowerCalculation_InitCheck (char *var, char *val, int *result)
{
  if (!strcasecmp(val,"yes")) *result=1;
  else if (!strcasecmp(val,"no")) *result=0;
  else if (!strcasecmp(val,"leakage_without_patterns")) *result=2;
  else if (!strcasecmp(val,"leakage_with_patterns")) *result=3;
  else if (!strcasecmp(val,"leakage")) *result=4;
  else if (!strcasecmp(val,"switching")) *result=5;
  else
  {
    avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
    return 0;
  }
  
  return 1;
}

int yagDetectGlitchers_InitCheck (char *var, char *val, int *result)
{
  if (!strcasecmp(val,"nondual")) *result=1;
  else if (!strcasecmp(val,"no")) *result=0;
  else if (!strcasecmp(val,"yes")) *result=2;
  else
  {
    avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
    return 0;
  }
  
  return 1;
}

static char *simToolModelEnum[] = {"spice","hspice","eldo","titan",NULL};

int simToolModel_InitCheck (char *var, char *val, int *result)
{
    int i;
    
    *result = -1;
    for (i=0; simToolModelEnum[i]; i++) {
        if (!strcasecmp(val,simToolModelEnum[i])) *result=i;
    }
    if (*result == -1) {
        avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
        return 0;
    }
    return 1;
}

char *simToolModel_EnumStr (int val)
{
    return simToolModelEnum[val];
}

static char *simToolEnum[] = {"spice","hspice","eldo","titan","titanv7","ltspice","mspice","ngspice",NULL};

int simTool_InitCheck (char *var, char *val, int *result)
{
    int i;
    
    *result = -1;
    for (i=0; simToolEnum[i]; i++) {
        if (!strcasecmp(val,simToolEnum[i])) *result=i;
    }
    if (*result == -1) {
        avt_errmsg (AVT_ERRMSG, "041", AVT_WARNING, val, var);
        return 0;
    }
    return 1;
}

char *simTool_EnumStr (int val)
{
    return simToolEnum[val];
}

V_INT V_INT_TAB[] = {
// Index Value Doc Set Variable
    {"VERILOG_MAXERR"                 , __VERILOG_MAXERR                 , 30         ,        0, 0, "avtVerilogMaxError"                     , NULL, NULL, NULL, ""},      
    {"VHDL_MAXERR"                    , __VHDL_MAXERR                    , 30         ,        0, 0, "avtVhdlMaxError"                        , NULL, NULL, NULL, ""},  
    {"ELP_CAPA_LEVEL"                 , __ELP_CAPA_LEVEL                 , 1          ,        1, 0, "avtElpCapaLevel"                        , NULL, level_check, NULL, ""},  //ok
    {"AVT_LICENSE_RESERVE"            , __AVT_LICENSE_RESERVE            , 0          ,        0, 0, "avtLicenseReserve"                      , NULL, NULL, NULL, ""},  
    {"MBK_MAX_CACHE_FILE"             , __MBK_MAX_CACHE_FILE             , 128        ,        1, 0, "avtMaxCacheFile"                        , NULL, NULL, NULL, ""},  
    {"TTV_MAX_PATH_PERIOD"            , __TTV_MAX_PATH_PERIOD            , 0          ,        1, 0, "avtMaxPathPeriodDepth"                  , NULL, NULL, NULL, ""},  
    {"TTV_PRECISION_LEVEL"            , __TTV_PRECISION_LEVEL            , 0          ,        1, 0, "avtPrecisionLevel"                      , NULL, NULL, NULL, ""},  
    {"SPI_RC_MEMORY_LIMIT"            , __SPI_RC_MEMORY_LIMIT            , 100        ,        1, 0, "avtSpiRCMemoryLimit"                    , NULL, NULL, NULL, ""},  
    {"CTK_REPORT_DELTA_DELAY_MIN"     , __CTK_REPORT_DELTA_DELAY_MIN     , 0          ,        1, 0, "ctkDeltaDelayMin"                       , NULL, NULL, NULL, ""},  
    {"CTK_REPORT_DELTA_SLOPE_MIN"     , __CTK_REPORT_DELTA_SLOPE_MIN     , 0          ,        1, 0, "ctkDeltaSlopeMin"                       , NULL, NULL, NULL, ""},  
    {"CTK_REPORT_NOISE_MIN"           , __CTK_REPORT_NOISE_MIN           , 0.0        ,        1, 0, "ctkNoiseMin"                            , NULL, NULL, NULL, ""},  
    {"MBK_SCALE_X"                    , __MBK_SCALE_X                    , 10000      ,        0, 0, "avtScaleX"                              , NULL, NULL, NULL, ""},  
    {"FCL_DEBUG_MODE"                 , __FCL_DEBUG_MODE                 , 0          ,        1, 0, "fclDebugMode"                           , NULL, NULL, NULL, ""},  
    {"FCL_SIZE_TOLERANCE"             , __FCL_SIZE_TOLERANCE             , 0          ,        1, 0, "fclMatchSizeTolerance"                  , NULL, NULL, NULL, ""},  
    {"FCL_TRACE_LEVEL"                , __FCL_TRACE_LEVEL                , 0          ,        1, 0, "fclTraceLevel"                          , NULL, NULL, NULL, ""},  
    {"GEN_DEBUG_LEVEL"                , __GEN_DEBUG_LEVEL                , 0          ,        1, 0, "gnsTraceLevel"                          , NULL, NULL, NULL, ""},  
    {"MBK_TRACE_LEVEL"                , __MBK_TRACE_LEVEL                , 0          ,        0, 0, "mbkTraceLevel"                          , NULL, NULL, NULL, ""},  
    {"STB_CTK_COEF_ACTIVITY"          , __STB_CTK_COEF_ACTIVITY          , 1          ,        1, 0, "stbCtkCoefActivity"                     , NULL, NULL, NULL, ""},  
    {"STB_CTK_COEF_CTK"               , __STB_CTK_COEF_CTK               , 2          ,        1, 0, "stbCtkCoefCtk"                          , NULL, NULL, NULL, ""},  
    {"STB_CTK_COEF_INTERVAL"          , __STB_CTK_COEF_INTERVAL          , 3          ,        1, 0, "stbCtkCoefInterval"                     , NULL, NULL, NULL, ""},  
    {"STB_CTK_COEF_NOISE"             , __STB_CTK_COEF_NOISE             , 4          ,        1, 0, "stbCtkCoefNoise"                        , NULL, NULL, NULL, ""},  
    {"STB_CTK_MARGIN"                 , __STB_CTK_MARGIN                 , 0          ,        1, 0, "stbCtkMargin"                           , NULL, NULL, NULL, ""},  
    {"STB_CTK_MAXLASTITER"            , __STB_CTK_MAXLASTITER            , 3          ,        1, 0, "stbCtkMaxLastIter"                      , NULL, NULL, NULL, ""},  
    {"STB_CTK_MIN_ACTIVITY"           , __STB_CTK_MIN_ACTIVITY           , 0          ,        1, 0, "stbCtkMinActivity"                      , NULL, NULL, NULL, ""},  
    {"STB_CTK_MIN_CTK"                , __STB_CTK_MIN_CTK                , 0          ,        1, 0, "stbCtkMinCtk"                           , NULL, NULL, NULL, ""},  
    {"STB_CTK_MIN_INTERVAL"           , __STB_CTK_MIN_INTERVAL           , 0          ,        1, 0, "stbCtkMinInterval"                      , NULL, NULL, NULL, ""},  
    {"STB_CTK_MIN_NOISE"              , __STB_CTK_MIN_NOISE              , 3          ,        1, 0, "stbCtkMinNoise"                         , NULL, NULL, NULL, ""},  
    {"STB_CTK_MINSLOPECHANGE"         , __STB_CTK_MINSLOPECHANGE         , 2          ,        1, 0, "stbCtkminSlopeChange"                   , NULL, NULL, NULL, ""},  
    {"EFG_MAX_DEPTH"                  , __EFG_MAX_DEPTH                  , 1          ,        1, 0, "simAnalysisDepth"                       , NULL, NULL, NULL, ""},  
    {"YAGLE_DEPTH"                    , __YAGLE_DEPTH                    , 9          ,        1, 0, "yagAnalysisDepth"                     , "yagleAnalysisDepth"   , NULL, NULL, ""},  
    {"YAGLE_AUTOLOOP_CEILING"         , __YAGLE_AUTOLOOP_CEILING         , 15         ,        0, 0, "yagAutomaticCeiling"                  , "yagleAutomaticCeiling", NULL, NULL, ""},  
    {"YAGLE_BDDCEILING"               , __YAGLE_BDDCEILING               , 10000      ,        1, 0, "yagBddCeiling"                        , "yagleBddCeiling"      , NULL, NULL, ""},  
    {"TMA_MARGIN"                     , __TMA_MARGIN                     , 0          ,        1, 0, "tmaConstraintMargin"                    , NULL, NULL, NULL, ""},  
    {"YAGLE_RELAX_LINKS"              , __YAGLE_RELAX_LINKS              , 6          ,        1, 0, "yagRelaxationMaxBranchLinks"          , "yagleRelaxationMaxBranchLinks"  , NULL, NULL, ""},  
    {"YAGLE_MAX_LINKS"                , __YAGLE_MAX_LINKS                , 6          ,        1, 0, "yagMaxBranchLinks"                    , "yagleMaxBranchLinks"            , NULL, NULL, ""},  
    {"YAGLE_GLITCH_LINKS"             , __YAGLE_GLITCH_LINKS             , 3          ,        1, 0, "yagMaxGlitcherLinks"                  , NULL, NULL, NULL, ""},  
    {"YAGLE_AUTOLOOP_DEPTH"           , __YAGLE_AUTOLOOP_DEPTH           , 9          ,        0, 0, "yagLatchAnalysisDepth"                , "yagleLatchAnalysisDepth"        , NULL, NULL, ""},  
    {"YAGLE_BLEEDER_STRICTNESS"       , __YAGLE_BLEEDER_STRICTNESS       , 1          ,        1, 0, "yagBleederStrictness"                 , "yagleBleederStrictness"         , NULL, NULL, ""},  
    {"YAGLE_BUS_DEPTH"                , __YAGLE_BUS_DEPTH                , 9          ,        0, 0, "yagBusAnalysisDepth"                  , "yagleBusAnalysisDepth"          , NULL, NULL, ""},  
    {"YAGLE_SENSITIVE_MAX"            , __YAGLE_SENSITIVE_MAX            , 0          ,        1, 0, "yagSensitiveTimingDriverLimit"        , "yagleSensitiveTimingDriverLimit", NULL, NULL, ""},
    {"YAGLE_DRIVE_CONFLICT"           , __YAGLE_DRIVE_CONFLICT           , 0          ,        1, 0, "yagDriveConflictCondition"            , NULL, yagDriveConflictCondition_InitCheck, NULL, ""},
    {"YAGLE_KEEP_GLITCHERS"           , __YAGLE_KEEP_GLITCHERS           , 2          ,        1, 0, "yagDetectGlitchers"                   , NULL, yagDetectGlitchers_InitCheck, NULL, ""},
    {"AVT_POWER_CALCULATION"          , __AVT_POWER_CALCULATION          , 0          ,        1, 0, "avtPowerCalculation"                  , NULL, avtPowerCalculation_InitCheck, NULL, ""},
    {"STB_PRECHARGE_DATA_HEURISTIC"   , __STB__PRECHARGE_DATA_HEURISTIC  , 0          ,        0, 0, "stbPrechargeDataHeuristic"            , NULL, stbPrechargeDataHeuristic_Check, NULL, ""}, 
    {"STM_CTK_MODEL"                  , __STM_CTK_MODEL                  , 2          ,        0, 0, "stmCtkModel"                          , NULL, NULL, NULL, ""},
    {"STM_OVERSHOOT_LEVEL"            , __STM_OVERSHOOT_LEVEL            , 1          ,        0, 0, "stmOvershootLevel"                    , NULL, NULL, NULL, ""},
    {"STM_INPUT_NB_PWL"               , __STM_INPUT_NB_PWL               , 40         ,        0, 0, "stmInputNbPwl"                        , NULL, NULL, NULL, ""},
    {"STM_QSAT_LEVEL"                 , __STM_QSAT_LEVEL                 , 0          ,        0, 0, "stmQsatLevel"                         , NULL, NULL, NULL, ""},
    {"TAS_SIMULATION_LEVEL"           , __TAS_SIMULATION_LEVEL           , 1          ,        0, 0, "tasSimulationLevel"                   , NULL, NULL, NULL, ""},
    {"TAS_CAPARA_DEPTH"               , __TAS_CAPARA_DEPTH               , 1          ,        1, 0, "tasPathCapacitanceDepth"              , NULL, NULL, NULL, ""},
    {"TAS_CAPARA_STRICT"              , __TAS_CAPARA_STRICT              , 2          ,        1, 0, "tasStrictPathCapacitance"             , NULL, tasStrictPathCapacitance_InitCheck, NULL, ""},
    {"TAS_CAPARA_FANOUT"              , __TAS_CAPARA_FANOUT              , 15         ,        1, 0, "tasMaxPathCapacitanceFanout"          , NULL, NULL, NULL, ""},
    {"STB_CTK_MAX_ITER"               , __STB_CTK_MAX_ITER               , 999        ,        1, 0, "stbCtkMaxIteration"                   , NULL, NULL, NULL, ""},
    {"STB_CTK_MAX_REPORTED_SIGNAL"    , __STB_CTK_MAX_REPORTED_SIGNAL    , 0          ,        1, 0, "stbCtkMaxReportedSignals"             , NULL, NULL, NULL, ""},
    {"STM_PRECISION_THRESHOLD"        , __STM_PRECISION_THRESHOLD        , 10         ,        0, 0, "stmPrecisionThreshold"                , NULL, NULL, NULL, ""},
    {"CPE_MAX_VARIABLES"              , __CPE_MAX_VARIABLES              , 20         ,        1, 0, "cpeMaxVariables"                      , NULL, NULL, NULL, ""},
    {"EQT_STATISTICAL_DISCRETISATION" , __EQT_STATISTICAL_DISCRETISATION , 10         ,        1, 0, "avtStatisticalDiscretisation"         , NULL, NULL, NULL, ""},
    {"AVT_RST_N"                      , __AVT_RST_N                      , 3          ,        1, 0, "avtRstN"                              , NULL, NULL, NULL, ""},
    {"MCC_SAT_N"                      , __MCC_SAT_N                      , 10         ,        1, 0, "mccSatN"                              , NULL, NULL, NULL, ""},
    {"TAS_USE_ENHANCED_BRANCH_FIT"    , __TAS_USE_ENHANCED_BRANCH_FIT    , 0          ,        1, 0, "UseEnhancedBranchFit"                 , NULL, NULL, NULL, ""},
    {"STB_SUPPRESS_LAG"               , __STB_SUPPRESS_LAG               , 0          ,        1, 0, "stbSuppressLag"                       , NULL, stbSuppressLag_Check, NULL, ""},
    {"SIM_TOOLMODEL"                  , __SIM_TOOLMODEL                  , 1          ,        1, 0, "simToolModel"                         , NULL, simToolModel_InitCheck, simToolModel_EnumStr, ""},
    {"SIM_TOOL"                       , __SIM_TOOL                       , 1          ,        1, 0, "simTool"                              , NULL, simTool_InitCheck, simTool_EnumStr, ""},
    {"SIM_TRANSISTOR_AS_INSTANCE"     , __SIM_TRANSISTOR_AS_INSTANCE     , 0          ,        1, 0, "simTransistorAsInstance"              , NULL, simTransistorAsInstance_InitCheck, NULL, ""}, 
    {"YAG_MAX_SPLIT_CMD_TIMING"       , __YAG_MAX_SPLIT_CMD_TIMING       , 0          ,        1, 0, "yagMaxSplitCmdTiming"                 , NULL, NULL, NULL, ""}, 
    {"STB_COREL_SKEW_ANA_DEPTH"       , __STB_COREL_SKEW_ANA_DEPTH       , 1000000    ,        0, 0, "stbCorrelatedSkewAnalysisDepth"       , NULL, stbCorrelatedSkewAnalysisDepth_InitCheck, NULL, ""}};

#define V_INT_TAB_SIZE (sizeof(V_INT_TAB)/(sizeof(V_INT))) 

//======================================================================
//
// Functions
//
//======================================================================

static int bool_comp  (const void *el1, const void *el2) { return ((V_BOOL*)el1)->INDEX  - ((V_BOOL*)el2)->INDEX; }
static int str_comp   (const void *el1, const void *el2) { return ((V_STR*)el1)->INDEX   - ((V_STR*)el2)->INDEX; }
static int int_comp   (const void *el1, const void *el2) { return ((V_INT*)el1)->INDEX   - ((V_INT*)el2)->INDEX; }
static int float_comp (const void *el1, const void *el2) { return ((V_FLOAT*)el1)->INDEX - ((V_FLOAT*)el2)->INDEX; }


int setnewcurrentmodel( char *var, char *val, int *result )
{
  int res;
  if( (res=bool_init_check( var, val, result ))!=0 ) {
    if( *result ) {

      V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL       ].VALUE = 1 ;
      V_BOOL_TAB[ __TAS_NEW_CURRENT_MODEL       ].SET   = 1 ;

      /* transistor configuration */
      V_BOOL_TAB[ __MCC_NEW_DIFFMODELSAT        ].VALUE = 1 ;
      V_BOOL_TAB[ __MCC_NEW_DIFFMODELSAT        ].SET   = 1 ;
      V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B        ].VALUE = 1 ;
      V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B        ].SET   = 1 ;
      V_BOOL_TAB[ __MCC_NEW_IDS_SAT             ].VALUE = 1 ;
      V_BOOL_TAB[ __MCC_NEW_IDS_SAT             ].SET   = 1 ;
      V_BOOL_TAB[ __AVT_RST_BETTER              ].VALUE = 1 ;
      V_BOOL_TAB[ __AVT_RST_BETTER              ].SET   = 1 ;
      V_BOOL_TAB[ __MCC_PRECISE_K               ].VALUE = 1 ;
      V_BOOL_TAB[ __MCC_PRECISE_K               ].SET   = 1 ;
      
      /* branch configuration */
      V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B_BRANCH ].VALUE = 1 ;
      V_BOOL_TAB[ __MCC_ALLOW_NEGATIVE_B_BRANCH ].SET   = 1 ;
      V_INT_TAB [ __TAS_USE_ENHANCED_BRANCH_FIT ].VALUE = 3 ;
      V_INT_TAB [ __TAS_USE_ENHANCED_BRANCH_FIT ].SET   = 1 ;

      /* charge and capacitance configuration */
      V_BOOL_TAB[ __TAS_USE_ALL_CAPAI           ].VALUE = 1 ;
      V_BOOL_TAB[ __TAS_USE_ALL_CAPAI           ].SET   = 1 ;
      V_BOOL_TAB[ __AVT_CORRECT_CGP             ].VALUE = 1 ;
      V_BOOL_TAB[ __AVT_CORRECT_CGP             ].SET   = 1 ;
      V_BOOL_TAB[ __STM_NEW_OVERSHOOT           ].VALUE = 1 ;
      V_BOOL_TAB[ __STM_NEW_OVERSHOOT           ].SET   = 1 ;
    }
  }
  return res;
}
//======================================================================
static char *checknull(char *str)
{
  if (str==NULL) return "(null)";
  return str;
}

static int avt_sethashvar_sub_bool(char *var, char *value, int warn, int set, int index)
{
    int start=0, end=V_BOOL_TAB_SIZE, oldval, oldset, i, i_val;

    /* ------ BOOLEAN VARIABLES --------------------------------------*/
    
  if (index>=0) start=index, end=index+1;
  
  for (i = start; i < end; i++)
    {
      if (index>=0 || (!strcasecmp (V_BOOL_TAB[i].VAR, var) || (V_BOOL_TAB[i].VAR2 && !strcasecmp (V_BOOL_TAB[i].VAR2, var))))
        {
          if (!(V_BOOL_TAB[i].SET & AVT_VAR_SET_BY_ENV))
            {
              oldval=V_BOOL_TAB[i].VALUE;
              oldset=V_BOOL_TAB[i].SET;
              if (V_BOOL_TAB[i].INIT_CHECK_FUNC)
                {
                  if (V_BOOL_TAB[i].INIT_CHECK_FUNC(V_BOOL_TAB[i].VAR, value, &i_val))
                    {
                      V_BOOL_TAB[i].VALUE = i_val;
                      V_BOOL_TAB[i].SET = set;
                    }
                  else warn=0;
                }
              else if (!strcasecmp (value, "yes"))
                {
                  V_BOOL_TAB[i].VALUE = 1;
                  V_BOOL_TAB[i].SET = set;
                }
              else if (!strcasecmp (value, "no"))
                {
                  V_BOOL_TAB[i].VALUE = 0;
                  V_BOOL_TAB[i].SET = set;
                }

              if (warn && oldset) {
                if (oldval == 1)
                  avt_errmsg (AVT_ERRMSG, "044", AVT_WARNING, var, "yes", value);
                else
                  avt_errmsg (AVT_ERRMSG, "044", AVT_WARNING, var, "no", value);
              }
            }
          return 1;
        }
    }
  return 0;
}

static int avt_sethashvar_sub_string(char *var, char *value, int warn, int set, int index)
{
  int start=0, end=V_STR_TAB_SIZE, oldset, i;
  char *oldval=NULL;

  if (index>=0) start=index, end=index+1;
  
  for (i = start; i < end; i++)
    {
      if (index>=0 || (!strcasecmp (V_STR_TAB[i].VAR, var) || (V_STR_TAB[i].VAR2 && !strcasecmp (V_STR_TAB[i].VAR2, var))))
        {
          if (!(V_STR_TAB[i].SET & AVT_VAR_SET_BY_ENV))
            {
              oldset=V_STR_TAB[i].SET;
              if (warn)
                oldval=(V_STR_TAB[i].VALUE==NULL)?NULL:strdup(V_STR_TAB[i].VALUE);
              if (!value)
                {
                  V_STR_AFFECT(V_STR_TAB[i].VALUE, NULL);
                  V_STR_TAB[i].SET = set;
                }
              else if (V_STR_TAB[i].INIT_CHECK_FUNC)
                {
                  if (V_STR_TAB[i].INIT_CHECK_FUNC(V_STR_TAB[i].VAR, value, NULL))
                    {
                      V_STR_AFFECT(V_STR_TAB[i].VALUE, value);
                      V_STR_TAB[i].SET = set;
                    }
                  else warn=0;
                }
              else
                {
                  V_STR_AFFECT(V_STR_TAB[i].VALUE, value);
                  V_STR_TAB[i].SET = set; 
                }
              if (warn && oldset)
                {
                  avt_errmsg (AVT_ERRMSG, "044", AVT_WARNING, var, checknull(oldval), checknull(value));
                }
              if (oldval!=NULL) free(oldval);
            }
          return 1;
        }
    }
  return 0;
}

static int avt_sethashvar_sub_integer(char *var, char *value, int warn, int set, int index)
{
  int start=0, end=V_INT_TAB_SIZE, oldval, oldset, i, i_val;
  char buffer[512];

  if (index>=0) start=index, end=index+1;
  
  for (i = start; i < end; i++)
    {
      if (index>=0 || (!strcasecmp (V_INT_TAB[i].VAR, var) || (V_INT_TAB[i].VAR2 && !strcasecmp (V_INT_TAB[i].VAR2, var))))
        {
          if (!(V_INT_TAB[i].SET & AVT_VAR_SET_BY_ENV))
            {
              oldval=V_INT_TAB[i].VALUE;
              oldset=V_INT_TAB[i].SET;
              if (warn && V_INT_TAB[i].SET) {
                sprintf (buffer, "%d", V_INT_TAB[i].VALUE);
                avt_errmsg (AVT_ERRMSG, "044", AVT_WARNING, var, buffer, value);
              }
              if (V_INT_TAB[i].INIT_CHECK_FUNC) 
                {
                  if (V_INT_TAB[i].INIT_CHECK_FUNC(V_INT_TAB[i].VAR, value, &i_val))
                    {
                      V_INT_TAB[i].VALUE = i_val;
                      V_INT_TAB[i].SET = set;
                    }
                  else warn=0;
                }
              else
                {
                  i_val = atoi (value);
                  V_INT_TAB[i].VALUE = i_val;
                  V_INT_TAB[i].SET = set;
                }

              if (warn && oldset) {
                sprintf (buffer, "%d", oldval);
                avt_errmsg (AVT_ERRMSG, "044", AVT_WARNING, var, buffer, value);
              }
            }
          return 1;
        }
    }
  return 0;
}
static int avt_sethashvar_sub_float(char *var, char *value, int warn, int set, int index)
{
  int start=0, end=V_FLOAT_TAB_SIZE, oldset, i;
  char buffer[512];
  float oldval, f_val;

  if (index>=0) start=index, end=index+1;
  
  for (i = start; i < end; i++)
    {
      if (index>=0 || (!strcasecmp (V_FLOAT_TAB[i].VAR, var) || (V_FLOAT_TAB[i].VAR2 && !strcasecmp (V_FLOAT_TAB[i].VAR2, var))))
        {
          if (!(V_FLOAT_TAB[i].SET & AVT_VAR_SET_BY_ENV))
            {
              oldval=V_FLOAT_TAB[i].VALUE;
              oldset=V_FLOAT_TAB[i].SET;
              if (V_FLOAT_TAB[i].INIT_CHECK_FUNC) 
                {
                  if (V_FLOAT_TAB[i].INIT_CHECK_FUNC(V_FLOAT_TAB[i].VAR, value, &f_val)) 
                    {
                      V_FLOAT_TAB[i].VALUE = f_val;
                      V_FLOAT_TAB[i].SET = set;
                    }
                  else warn=0;
                }
              else
                {
                  f_val = strtod (value, (char**)NULL);
                  V_FLOAT_TAB[i].VALUE = f_val;
                  V_FLOAT_TAB[i].SET = set;
                }
              if (warn && oldset) 
                {
                  sprintf (buffer, "%g", V_FLOAT_TAB[i].VALUE);
                  avt_errmsg (AVT_ERRMSG, "044", AVT_WARNING, var, buffer, value);
                }
            }
          return 1;
        }
    }
  return 0;
}

void avt_init_v ()
{
  unsigned int i;
  char *env;

  qsort (V_BOOL_TAB,  V_BOOL_TAB_SIZE,  sizeof (V_BOOL),  bool_comp); 
  qsort (V_STR_TAB,   V_STR_TAB_SIZE,   sizeof (V_STR),   str_comp); 
  qsort (V_INT_TAB,   V_INT_TAB_SIZE,   sizeof (V_INT),   int_comp); 
  qsort (V_FLOAT_TAB, V_FLOAT_TAB_SIZE, sizeof (V_FLOAT), float_comp); 

  for (i=0;i<V_BOOL_TAB_SIZE;i++)
    if ((env=getenv(V_BOOL_TAB[i].VARENV))!=NULL)
      avt_sethashvar_sub_bool(NULL, env, 0, AVT_VAR_SET_BY_ENV, i);

  for (i=0;i<V_STR_TAB_SIZE;i++) // const => malloc
    {
      if (V_STR_TAB[i].VALUE!=NULL) V_STR_TAB[i].VALUE=strdup(V_STR_TAB[i].VALUE);
      if ((env=getenv(V_STR_TAB[i].VARENV))!=NULL)
        avt_sethashvar_sub_string(NULL, env, 0, AVT_VAR_SET_BY_ENV, i);
    }

  for (i=0;i<V_INT_TAB_SIZE;i++)
    if ((env=getenv(V_INT_TAB[i].VARENV))!=NULL)
      avt_sethashvar_sub_integer(NULL, env, 0, AVT_VAR_SET_BY_ENV, i);

  for (i=0;i<V_FLOAT_TAB_SIZE;i++)
    if ((env=getenv(V_FLOAT_TAB[i].VARENV))!=NULL)
      avt_sethashvar_sub_float(NULL, env, 0, AVT_VAR_SET_BY_ENV, i);

  /* check coherence */
  for( i=0 ; i<V_BOOL_TAB_SIZE ; i++ ) 
    if( V_BOOL_TAB[ i ].INDEX != (int)i )
      avt_errmsg (AVT_ERRMSG, "058", AVT_WARNING, "V_BOOL_TAB", i, V_BOOL_TAB[ i ].INDEX );
  
  for( i=0 ; i<V_STR_TAB_SIZE ; i++ ) 
    if( V_STR_TAB[ i ].INDEX != (int)i )
      avt_errmsg (AVT_ERRMSG, "058", AVT_WARNING, "V_STR_TAB", i, V_STR_TAB[ i ].INDEX );
  
  for( i=0 ; i<V_INT_TAB_SIZE ; i++ ) 
    if( V_INT_TAB[ i ].INDEX != (int)i )
      avt_errmsg (AVT_ERRMSG, "058", AVT_WARNING, "V_INT_TAB", i, V_INT_TAB[ i ].INDEX );
  
  for( i=0 ; i<V_FLOAT_TAB_SIZE ; i++ ) 
    if( V_FLOAT_TAB[ i ].INDEX != (int)i )
      avt_errmsg (AVT_ERRMSG, "058", AVT_WARNING, "V_FLOAT_TAB", i, V_FLOAT_TAB[ i ].INDEX );
}

void avt_sethashvar_sub (char *var, char *value, int warn, int set)
{
  if (!avt_sethashvar_sub_bool(var, value, warn, set, -1))
    if (!avt_sethashvar_sub_string(var, value, warn, set, -1))
      if (!avt_sethashvar_sub_float(var, value, warn, set, -1))
        if (!avt_sethashvar_sub_integer(var, value, warn, set, -1))
          {
            avt_errmsg (AVT_ERRMSG, "003", AVT_WARNING, var);
          }
}

void avt_sethashvar (char *var, char *value)
{
  avt_sethashvar_sub(var, value, 1, AVT_VAR_SET_BY_TCLCONFIG);
}

/*****************************************************************************/
/*                        function avt_gethashvar ()                         */
/*****************************************************************************/

char *avt_gethashvar_sub (char *var, int intable)
{
  unsigned int i;

  for (i = 0; i < V_BOOL_TAB_SIZE; i++) 
    if (!strcasecmp (V_BOOL_TAB[i].VARENV, var) || !strcasecmp (V_BOOL_TAB[i].VAR, var) || (V_BOOL_TAB[i].VAR2 && !strcasecmp (V_BOOL_TAB[i].VAR2, var))) {
      if (V_BOOL_TAB[i].SET || intable) {
        if (V_BOOL_TAB[i].VALUE)
          return "yes";
        else
          return "no";
      }
      return getenv (var); // Environment variable if not set
    }

  for (i = 0; i < V_STR_TAB_SIZE; i++) 
    if (!strcasecmp (V_STR_TAB[i].VARENV, var) || !strcasecmp (V_STR_TAB[i].VAR, var) || (V_STR_TAB[i].VAR2 && !strcasecmp (V_STR_TAB[i].VAR2, var))) {
      if (V_STR_TAB[i].SET || intable) {
        return V_STR_TAB[i].VALUE;
      }
      return getenv (var);// Environment variable if not set
    }

  for (i = 0; i < V_INT_TAB_SIZE; i++) 
    if (!strcasecmp (V_INT_TAB[i].VARENV, var) || !strcasecmp (V_INT_TAB[i].VAR, var) || (V_INT_TAB[i].VAR2 && !strcasecmp (V_INT_TAB[i].VAR2, var))) {
      if (V_INT_TAB[i].SET || intable) {
        sprintf (V_INT_TAB[i].STRBUF, "%d", V_INT_TAB[i].VALUE);
        return V_INT_TAB[i].STRBUF;
      }
      return getenv (var);// Environment variable if not set
    }

  for (i = 0; i < V_FLOAT_TAB_SIZE; i++) 
    if (!strcasecmp (V_FLOAT_TAB[i].VARENV, var) || !strcasecmp (V_FLOAT_TAB[i].VAR, var) || (V_FLOAT_TAB[i].VAR2 && !strcasecmp (V_FLOAT_TAB[i].VAR2, var))) {
      if (V_FLOAT_TAB[i].SET || intable) {
        sprintf (V_FLOAT_TAB[i].STRBUF, "%g", V_FLOAT_TAB[i].VALUE);
        return V_FLOAT_TAB[i].STRBUF;
      }
      return getenv (var);// Environment variable if not set
    }

  return getenv (var);// Environment variable if not defined
}

char *avt_gethashvar (char *var)
{
   return avt_gethashvar_sub(var, 0);
}
//======================================================================

//======================================================================

void avt_parsavtfile ()
{
  FILE *file;
  char buf[8192];
  char config[1024];
  char *name;
  char *param;
  char *pt;
  int res = 1;
  int size = 8192;
  int len;
  char flag = 'N';
  int line = 0;

  sprintf (config, "avttools.conf");
  file = fopen (config, "r");

  if (!file) {
    AVT_VARENV_FILE = 'N';
    return;
  }

  while (fgets (buf, size, file)) {
    line++;
    buf[size - 1] = '\0';
    while ((pt = strrchr (buf, (int)'\\')) && (*(pt + 1) == '\0' || *(pt + 1) == '\n')) {
      *pt = '\0';
      len = size - strlen (buf);
      if (!fgets (pt, len, file)) {
        avt_errmsg(AVT_ERRMSG,"003",AVT_WARNING,line);
        flag = 'Y';
        break;
      }
      else {
        buf[size - 1] = '\0';
        name = pt;
        while ((isspace ((int)(*name)) != 0) && (*name != '\0')) name++;
        if (name != pt) {
          while (*name != '\0') *pt++ = *name++;
          *pt++ = *name++;
        }
      }
    }
    if (flag == 'Y')
      break;
    name = buf;
    while ((isspace ((int)(*name))) && (*name != '\0')) name++;
    if (!isalpha ((int)(*name))) continue;
    else {
      param = name;
      for( param=name ; *param != ' ' && *param != '=' && *param != '\0' && *param != '\t' ; param++ );
      pt = strchr (param, '=');
      if (!pt) res = 0;
      else {
        *param = '\0';
        param++;
        while ((isspace ((int)(*param)) || (*param == '=')) && (*param != '\0')) param++;
        pt = param;
        if (*pt == '"') {
          *param = '\0';
          param++;
          while (*pt != '"') {
            if (*pt == '\n') { res = 0; break; }
            else pt++;
          }
        }
        else
          while ((*pt != '\n') && (*pt != '\0') && (isspace ((int)(*pt)) == 0)) pt++;
        *pt = '\0';
      }
    }

    if (res) {
      pt = mbkalloc (strlen (name) + 1);
      pt = strcpy (pt, name);
      name = pt;
      pt = mbkalloc (strlen (param) + 1);
      pt = strcpy (pt, param);
      param = pt;
      if (*param != '\0')
        AVT_VARENV_LIST = addptype (AVT_VARENV_LIST, (long)name, param);
      else {
        free (name);
        free (param);
      }
    }
    else {
      avt_errmsg(AVT_ERRMSG,"004",AVT_WARNING, line);
      break;
    }
  }

  if (!AVT_VARENV_LIST) {
    AVT_VARENV_FILE = 'N';
    return;
  }
    
  AVT_VARENV_LIST = (ptype_list*)reverse ((chain_list*)AVT_VARENV_LIST);

  fclose (file);
}

//======================================================================

void avtenv ()
{
    char *str, *var, *value;
    ptype_list *ptype;
    static int done = 0;
    static char buf[32];

    srand ((int) time (NULL));
    AVT_VALID_TOKEN = rand ();
    
    if (done || AVT_VARENV_FILE == 'N') return;

    done = 1;
    
    sprintf(buf,"%s%s",AVT_VERSION,PATCH_NUM);
    AVT_FULLVERSION = buf;

    avt_init_v ();

    if (!AVT_VARENV_LIST) avt_parsavtfile ();

    for (ptype = AVT_VARENV_LIST; ptype; ptype = ptype->NEXT)
      {        
        var = (char*)ptype->TYPE;
        value = (char *)ptype->DATA;
        avt_sethashvar_sub (var, value, 1, AVT_VAR_SET_BY_TOOLSCONF);
      }

    AVT_COL = V_BOOL_TAB[__AVT_COLOR].VALUE;
}

//======================================================================

int avt_is_default_technoname ( char *name )
{
    return !strcasecmp (name, "avtdefault.tec");
}

//======================================================================
static void avt_dumpconfig()
{
    unsigned int i;

    avt_log(LOGCONFIG, 1, " -- Current configuration --\n"); 
    for (i = 0; i < V_BOOL_TAB_SIZE; i++) 
      if (V_BOOL_TAB[i].SET) {
          avt_log(LOGCONFIG, 1, " %-25s = %s\n", V_BOOL_TAB[i].VAR, V_BOOL_TAB[i].VALUE?"yes":"no"); 
      }

    for (i = 0; i < V_STR_TAB_SIZE; i++) 
      if (V_STR_TAB[i].SET) {
          avt_log(LOGCONFIG, 1, " %-25s = \"%s\"\n", V_STR_TAB[i].VAR, V_STR_TAB[i].VALUE); 
      }

    for (i = 0; i < V_INT_TAB_SIZE; i++) 
      if (V_INT_TAB[i].SET) {
          avt_log(LOGCONFIG, 1, " %-25s = %d\n", V_INT_TAB[i].VAR, V_INT_TAB[i].VALUE); 
      }

    for (i = 0; i < V_FLOAT_TAB_SIZE; i++) 
      if (V_FLOAT_TAB[i].SET) {
          avt_log(LOGCONFIG, 1, " %-25s = %d\n", V_FLOAT_TAB[i].VAR, V_FLOAT_TAB[i].VALUE); 
      }
    avt_log(LOGCONFIG, 1, " --         End           --\n"); 
}

void avt_LogConfig ()
{
    char  config[1024];
    FILE *file;
    static int avt_alreadylogged = 0;
  
    if(!AVTLOGFILE || avt_alreadylogged) return;
    
    sprintf (config, "avttools.conf");
    file = fopen (config, "r");
    if (file) {
        avt_log(LOGCONFIG, 9, "Loading '%s'\n", config); 
        mbk_dumpfile (file, AVTLOGFILE, 9);
        fclose (file);
    }
    else
        avt_log(LOGCONFIG, 1, "No '%s' file\n", config); 

    avt_dumpconfig();

    avt_alreadylogged = 1;
}

//======================================================================

void avt_man (int all)
{
    unsigned int i, j;
    unsigned int emax = 0, vmax = 0;
    char *str;
    
    for (i = 0; i < V_BOOL_TAB_SIZE; i++) {
        vmax = vmax > strlen (V_BOOL_TAB[i].VAR) ? vmax: strlen (V_BOOL_TAB[i].VAR);
        emax = emax > strlen (V_BOOL_TAB[i].VARENV) ? emax: strlen (V_BOOL_TAB[i].VARENV);
    }

    for (i = 0; i < V_STR_TAB_SIZE; i++) {
        vmax = vmax > strlen (V_STR_TAB[i].VAR) ? vmax: strlen (V_STR_TAB[i].VAR);
        emax = emax > strlen (V_STR_TAB[i].VARENV) ? emax: strlen (V_STR_TAB[i].VARENV);
    }

    for (i = 0; i < V_INT_TAB_SIZE; i++) {
        vmax = vmax > strlen (V_INT_TAB[i].VAR) ? vmax: strlen (V_INT_TAB[i].VAR);
        emax = emax > strlen (V_INT_TAB[i].VARENV) ? emax: strlen (V_INT_TAB[i].VARENV);
    }

    for (i = 0; i < V_FLOAT_TAB_SIZE; i++) {
        vmax = vmax > strlen (V_FLOAT_TAB[i].VAR) ? vmax: strlen (V_FLOAT_TAB[i].VAR);
        emax = emax > strlen (V_FLOAT_TAB[i].VARENV) ? emax: strlen (V_FLOAT_TAB[i].VARENV);
    }


    for (i = 0; i < V_BOOL_TAB_SIZE; i++) {
        if (!all && !V_BOOL_TAB[i].DOC) continue;
        fprintf (stdout, "[BOOL ] %s", V_BOOL_TAB[i].VAR);
        for (j = 0; j < vmax - strlen (V_BOOL_TAB[i].VAR); j++) fprintf (stdout, " ");
        fprintf (stdout, " | %s", V_BOOL_TAB[i].VARENV);
        for (j = 0; j < emax - strlen (V_BOOL_TAB[i].VARENV); j++) fprintf (stdout, " ");
        if (V_BOOL_TAB[i].SET) {
            if (V_BOOL_TAB[i].VALUE == 1)
                fprintf (stdout, " | yes\n");
            else 
            if (V_BOOL_TAB[i].VALUE == 0)
                fprintf (stdout, " | no\n");
            else
                fprintf (stdout, " | INVALID_VALUE %d\n", V_BOOL_TAB[i].VALUE);
        } 
        else {
            if ((str = getenv (V_BOOL_TAB[i].VARENV)))
                fprintf (stdout, " | %s (setenv)\n", str);
            else if (V_BOOL_TAB[i].VALUE == 1)
                fprintf (stdout, " | UNSET (yes)\n");
            else if (V_BOOL_TAB[i].VALUE == 0)
                fprintf (stdout, " | UNSET (no)\n");
            else
                fprintf (stdout, " | UNSET\n");
        }
    }
    
    
    for (i = 0; i < V_STR_TAB_SIZE; i++) {
        if (!all && !V_STR_TAB[i].DOC) continue;
        fprintf (stdout, "[STR  ] %s", V_STR_TAB[i].VAR);
        for (j = 0; j < vmax - strlen (V_STR_TAB[i].VAR); j++) fprintf (stdout, " ");
        fprintf (stdout, " | %s", V_STR_TAB[i].VARENV);
        for (j = 0; j < emax - strlen (V_STR_TAB[i].VARENV); j++) fprintf (stdout, " ");
        if (V_STR_TAB[i].SET) {
            if (V_STR_TAB[i].VALUE)
                fprintf (stdout, " | %s\n", V_STR_TAB[i].VALUE);
            else
                fprintf (stdout, " | NULL\n");
        } 
        else {
            if ((str = getenv (V_STR_TAB[i].VARENV)))
                fprintf (stdout, " | %s (setenv)\n", str);
            else if (V_STR_TAB[i].VALUE)
                fprintf (stdout, " | UNSET (\"%s\")\n", V_STR_TAB[i].VALUE);
            else
                fprintf (stdout, " | UNSET\n");
        }
    }

    for (i = 0; i < V_INT_TAB_SIZE; i++) {
        if (!all && !V_INT_TAB[i].DOC) continue;
        fprintf (stdout, "[INT  ] %s", V_INT_TAB[i].VAR);
        for (j = 0; j < vmax - strlen (V_INT_TAB[i].VAR); j++) fprintf (stdout, " ");
        fprintf (stdout, " | %s", V_INT_TAB[i].VARENV);
        for (j = 0; j < emax - strlen (V_INT_TAB[i].VARENV); j++) fprintf (stdout, " ");
        if (V_INT_TAB[i].SET) {
            if (V_INT_TAB[i].ENUMSTR_FUNC) {
                fprintf (stdout, " | \"%s\"\n", V_INT_TAB[i].ENUMSTR_FUNC(V_INT_TAB[i].VALUE));
            }
            else fprintf (stdout, " | %d\n", V_INT_TAB[i].VALUE);
        }
        else {
            if ((str = getenv (V_INT_TAB[i].VARENV)))
                fprintf (stdout, " | %s (setenv)\n", str);
            else {
                if (V_INT_TAB[i].ENUMSTR_FUNC) {
                    fprintf (stdout, " | UNSET (\"%s\")\n", V_INT_TAB[i].ENUMSTR_FUNC(V_INT_TAB[i].VALUE));
                }
                else fprintf (stdout, " | UNSET (%d)\n", V_INT_TAB[i].VALUE);
            }
        }
    }

    for (i = 0; i < V_FLOAT_TAB_SIZE; i++) {
        if (!all && !V_FLOAT_TAB[i].DOC) continue;
        fprintf (stdout, "[FLOAT] %s", V_FLOAT_TAB[i].VAR);
        for (j = 0; j < vmax - strlen (V_FLOAT_TAB[i].VAR); j++) fprintf (stdout, " ");
        fprintf (stdout, " | %s", V_FLOAT_TAB[i].VARENV);
        for (j = 0; j < emax - strlen (V_FLOAT_TAB[i].VARENV); j++) fprintf (stdout, " ");
        if (V_FLOAT_TAB[i].SET)
            fprintf (stdout, " | %g\n", V_FLOAT_TAB[i].VALUE);
        else {
            if ((str = getenv (V_FLOAT_TAB[i].VARENV)))
                fprintf (stdout, " | %s (setenv)\n", str);
            else
                fprintf (stdout, " | UNSET (%g)\n", V_FLOAT_TAB[i].VALUE);
        }
    }

}

void V_STR_AFFECT_F(char **x, char *v)
{
  if (*x!=NULL) free(*x);
  *x=(v==NULL)?NULL:strdup(v);
}


