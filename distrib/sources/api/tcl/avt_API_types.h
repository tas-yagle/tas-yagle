#ifndef API_TYPES
#define API_TYPES

#ifdef API_USE_REAL_TYPES
#define TimingLine            ttvline_list
#define Netlist               lofig_list
#define Signal                losig_list
#define Instance              loins_list
#define TimingFigure          ttvfig_list
#define List                  chain_list
#define TimingSignalList      chain_list
#define TimingEventList       chain_list
#define TimingPathList        chain_list
#define TimingDetailList      chain_list
#define Property              ptype_list
#define TimingSignal          ttvsig_list
#define TimingEvent           ttvevent_list
#define TimingPath            ttvpath_list
#define TimingDetail          ttvcritic_list
#define DoubleList            chain_list
#define DoubleListTimeValue   chain_list
#define DoubleListCapaValue   chain_list
#define DoubleTable           chain_list
#define StringList            chain_list
#define StringListF           chain_list
#define BehavioralFigure      befig_list
#define StabilityPathList     chain_list
#define StabilityRangeList    chain_list
#define StabilityFigure       stbfig_list
#define StabilityRange        stbpair_list
#define StabilityPath         stbdebug_list
#define ConeFigure            cnsfig_list
#define Aggressor             ctk_aggressor
#define AggressorList         chain_list
#define Connector             locon_list
#define SimulationContext     sim_model
#define CtkContext            ctk_struct
#define CorrespondingInstance subinst_t
#define HashTable             ht
#define GNSRun                ALL_FOR_GNS
#define Transistor            lotrs_list
#define ListOfDoubleTable     chain_list
#define TimingPathTable       chain_list
#define TimingLineList        chain_list
#define StabilitySlackList    chain_list
#define StabilitySlack        slackobject
#define Board                 Board
#define BoardColumn           BoardColumn
#define TimingConstraint      ConstraintObject
#define TimingConstraintList  chain_list
#define IntegerTable          chain_list
#define QuickList             quicklist
#define String                char

#else
typedef struct TimingLine         TimingLine;
typedef struct Netlist            Netlist;
typedef struct TimingFigure       TimingFigure;
typedef struct List               List;
typedef struct TimingPathList     TimingPathList;
typedef struct TimingDetailList   TimingDetailList;
typedef struct TimingSignalList   TimingSignalList;
typedef struct TimingEventList    TimingEventList;
typedef struct Property           Property;
typedef struct TimingSignal       TimingSignal;
typedef struct TimingEvent        TimingEvent;
typedef struct TimingPath         TimingPath;
typedef struct TimingDetail       TimingDetail;
typedef struct DoubleList         DoubleList;
typedef struct DoubleListTimeValue  DoubleListTimeValue;
typedef struct DoubleListCapaValue  DoubleListCapaValue;
typedef struct DoubleTable        DoubleTable;
typedef struct StringList         StringList;
typedef struct StringListF        StringListF;
typedef struct BehavioralFigure   BehavioralFigure;
typedef struct StabilityPathList  StabilityPathList;
typedef struct StabilityRangeList StabilityRangeList;
typedef struct StabilityFigure    StabilityFigure;
typedef struct StabilityRange     StabilityRange;
typedef struct StabilityPath      StabilityPath;
typedef struct ConeFigure         ConeFigure;
typedef struct ListOfDoubleTable  ListOfDoubleTable;
typedef struct TimingPathTable    TimingPathTable;
typedef struct TimingLineList     TimingLineList;
typedef struct StabilitySlackList StabilitySlackList;
typedef struct StabilitySlack StabilitySlack;
typedef struct TimingConstraint TimingConstraint;
typedef struct TimingConstraintList TimingConstraintList;
typedef struct IntegerTable IntegerTable;
typedef struct QuickList QuickList;

#define Aggressor         void
#define Instance          void
#define AggressorList     void
#define Connector         void
#define SimulationContext void 
#define CtkContext        void
#define Signal            void
#define CorrespondingInstance void
#define HashTable             void
#define GNSRun                void
#define Transistor            void
#define String char
#endif

#define TYPE_TIMING_FIGURE    1
#define TYPE_TIMING_SIGNAL    2
#define TYPE_TIMING_EVENT     3
#define TYPE_DOUBLE           4
#define TYPE_CHAR             5
#define TYPE_INT              6
#define TYPE_TIMING_PATH      7
#define TYPE_DOUBLE_TABLE     8
#define TYPE_TABLE            9
#define TYPE_STABILITY_FIGURE 10

#define TimeValue double
#define CapaValue double
#define VoltageValue double
#endif
