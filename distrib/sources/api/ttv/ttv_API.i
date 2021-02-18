%rename (ttv_GetTimingFigure_sub) ttv_GetTimingFigure;

/* DATABASE GENERATION                                       */
void ttv_CreateTimingFigure(Netlist *lf);
void ttv_EditTimingFigure(TimingFigure *tvf);
TimingSignal *ttv_AddConnector(char *name, char dir);
TimingSignal *ttv_AddCommand(char *name);
TimingSignal *ttv_AddLatch(char *name);
TimingSignal *ttv_AddFlipFlop(char *name);
TimingSignal *ttv_AddPrecharge(char *name);
TimingSignal *ttv_AddBreakpoint(char *name);
TimingLine *ttv_AddTiming(char *input, char *output, TimeValue max_delay, TimeValue max_slope, TimeValue min_delay, TimeValue min_slope, char *dir);
TimingLine *ttv_AddHZTiming(char *input, char *output, TimeValue max_delay, TimeValue min_delay, char *dir);
TimingLine *ttv_AddSetup(char *input, char *output, TimeValue delay, char *dir);
TimingLine *ttv_AddAccess(char *input, char *output, TimeValue max_delay, TimeValue max_slope, TimeValue min_delay, TimeValue min_slope, char *dir);
TimingLine *ttv_AddHZAccess(char *input, char *output, TimeValue max_delay, TimeValue min_delay, char *dir);
TimingLine *ttv_AddHold(char *input, char *output, TimeValue delay, char *dir);
void ttv_SetLineCommand(TimingLine *tl, char *max_command, char *min_command, char *dir);
TimingFigure *ttv_FinishTimingFigure();
void ttv_DriveTimingFigure(TimingFigure *tvf, char *filename, char *format);
TimingFigure *ttv_LoadSDF (Netlist *fig, char *sdf_file);

/* DATABASE LOADING                                          */
TimingFigure *ttv_LoadSpecifiedTimingPathFigure(char *name);
TimingFigure *ttv_LoadSpecifiedTimingFigure(char *name);
int ttv_LoadCrosstalkFile(TimingFigure *tvf);
void ttv_RemoveTimingFigure(TimingFigure *tf);

/* DATABASE PROPERTIES                                        */
Property *ttv_GetTimingFigureProperty (TimingFigure *fig, char *code);
double ttv_GetTimingFigureTemperature(TimingFigure *tf);
double ttv_GetTimingFigureSupply(TimingFigure *tf);
double ttv_GetTimingFigureOutputCapacitance(TimingFigure *tf);
double ttv_GetTimingFigureInputSlope(TimingFigure *tf);
double ttv_GetTimingFigureLowThreshold(TimingFigure *tf);
double ttv_GetTimingFigureHighThreshold(TimingFigure *tf);
double ttv_GetTimingFigureThreshold(TimingFigure *tf);
char *ttv_GetTimingFigureTechno(TimingFigure *tf);
char *ttv_GetTimingFigureGenerationDate(TimingFigure *tf);
char *ttv_GetTimingFigureGenerationTime(TimingFigure *tf);
void ttv_SetTimingFigureName( TimingFigure *tvf, char *name );

/* PATH BROWSING                                               */
TimingPathList *ttv_GetPaths_sub (TimingFigure *tf, char *clock, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax);
TimingPathList *ttv_GetCriticPathList(TimingFigure *tf, char *start, char *end, char *dir, long number, char *minmax);
TimingPathList *ttv_GetCriticMaxPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetCriticMinPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetUnsortedCriticMaxPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetUnsortedCriticMinPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetParallelPaths(TimingPath * ta, long num);
TimingPathList *ttv_GetCriticMaxAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList *ttv_GetCriticMinAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList *ttv_GetAllMaxAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList *ttv_GetAllMinAccessList(TimingFigure *tf, char *start, char *end, char *clock, char *dir, long number);
TimingPathList *ttv_GetMaxAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir );
TimingPathList *ttv_GetMinAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir );
TimingPathList *ttv_GetAllMaxAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir, long number);
TimingPathList *ttv_GetAllMinAccess(TimingSignal *sig, TimingSignal *end, char *clock, char *dir, long number );
TimingPathList *ttv_GetCriticMaxPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_GetCriticMinPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_GetAllMaxPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetAllMinPathList(TimingFigure *tf, char *start, char *end, char *dir, long number);
TimingPathList *ttv_GetAllMaxPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_GetAllMinPath(TimingSignal *start, TimingSignal *end, char *dir, long number);
TimingPathList *ttv_RemoveIncludedSmallerPath(TimingPathList *lpl, TimingPathList *spl);
TimingPathList *ttv_SortPathList( TimingPathList*, char *order);
TimingPathList *ttv_GetMaxTimingPathFrom(TimingSignal *start, char dir);
TimingPathList *ttv_GetMaxTimingPathTo(TimingSignal *end, char dir);
TimingPathList *ttv_GetMaxTimingPath(TimingSignal *start,TimingSignal *end, char *dir);
TimingPathList *ttv_GetMinTimingPathFrom(TimingSignal *start, char dir);
TimingPathList *ttv_GetMinTimingPathTo(TimingSignal *dest, char dir);
TimingPathList *ttv_GetMinTimingPath(TimingSignal *start,TimingSignal *end, char *dir);
TimingPathList *ttv_RemoveDuplicatedPath(TimingPathList *talist );
void ttv_FreePathList(TimingPathList *lst);
void ttv_SearchExcludeNodeType(char *conf);
TimingSignalList *ttv_GetMatchingSignal(TimingFigure *tf, char *name, char *type);

/* PATH REPORTS                                               */
void ttv_DisplayPathListDetail(FILE *f, TimingPathList *tpl);
void ttv_DisplayPathDetail(FILE *f, int num, TimingPath *tp);
void ttv_DisplayPathList(FILE *f, TimingPathList *tpl);
void ttv_DisplayPathDetailShowColumn(char *conf);
void ttv_DisplayPathDetailHideColumn(char *conf);
void ttv_DisplayCompletePathDetail(FILE *f, int num, TimingPath *tp, TimingDetailList *detail);
void ttv_SetReportUnit(char *val); // obsolete
void ttv_SetupReport(char *val);
void ttv_DisplayConnectorToLatchMargin (FILE *f, TimingFigure *tf, char *inputconnector, char *mode);
void ttv_PlotPathDetail(FILE *f, TimingPath *tp);

/* PATH PROPERTIES                                              */
Property *ttv_GetTimingPathProperty (TimingPath *path, char *code);
double ttv_GetPathDelay(TimingPath *ta);
double ttv_GetPathRefDelay(TimingPath *ta);
double ttv_GetPathDataLag(TimingPath *ta);
double ttv_GetPathSlope(TimingPath *ta);
double ttv_GetPathRefSlope(TimingPath *ta);
double ttv_GetPathStartTime(TimingPath *ta);
double ttv_GetPathStartSlope(TimingPath * ta);
char ttv_GetPathStartDirection(TimingPath *ta);
char ttv_GetPathEndDirection(TimingPath *ta);
TimingSignal *ttv_GetPathStartSignal(TimingPath *ta);
TimingSignal *ttv_GetPathEndSignal(TimingPath *ta);
TimingEvent *ttv_GetPathCommand(TimingPath *ta);
TimingEvent *ttv_GetPathAccessLatchCommand(TimingPath *ta);

/* PATH DETAIL BROWSING                                       */
char *ttv_AutomaticDetailBuild(char *mode);
extern TimingDetailList *ttv_GetParallelPathDetail (TimingPath *ta);
extern TimingDetailList *ttv_GetPathDetail (TimingPath *ta);
void ttv_FreePathDetail(TimingDetailList *cl);
void ttv_FreeParallelPathDetail(TimingDetailList *cl);

/* PATH DETAIL PROPERTIES                                     */
Property *ttv_GetTimingDetailProperty (TimingDetail *det, char *property);
extern char *ttv_GetDetailNodeName (TimingDetail *td);
extern char *ttv_GetDetailSignalName (TimingDetail *td);
extern double ttv_GetDetailDelay (TimingDetail *td);
extern double ttv_GetDetailSlope (TimingDetail *td);
extern char *ttv_GetDetailType (TimingDetail *td);
extern char ttv_GetDetailDirection (TimingDetail *td);
extern DoubleList *ttv_GetSignalCapaList (TimingSignal *tvs);

/* SIGNAL BROWSING                                            */
TimingSignalList *ttv_GetTimingSignalList(TimingFigure *tvf, char *type, char *location);
TimingSignalList *ttv_GetTimingSignalListByNet(TimingFigure *tvf, char *name);
TimingSignalList *ttv_GetInterfaceBreakpointList(TimingFigure *tvf);
TimingSignalList *ttv_GetInterfaceLatchList(TimingFigure *tvf);
TimingSignalList *ttv_GetInterfaceCommandList(TimingFigure *tvf);
TimingSignalList *ttv_GetInterfacePrechargeList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalBreakpointList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalLatchList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalCommandList(TimingFigure *tvf);
TimingSignalList *ttv_GetInternalPrechargeList(TimingFigure *tvf);
TimingSignalList *ttv_GetConnectorList(TimingFigure *tf);
TimingSignalList *ttv_GetClockList (TimingFigure *tvf);
TimingSignal *ttv_GetTimingSignal(TimingFigure *tvf, char *name);

/* SIGNAL PROPERTIES                                          */
Property *ttv_GetTimingSignalProperty (TimingSignal *fig, char *code);
char *ttv_GetSignalNetName(TimingSignal *tvs);
char *ttv_GetSignalName(TimingSignal *tvs);
char *ttv_GetFullSignalNetName(TimingFigure *tvf, TimingSignal *tvs);
char *ttv_GetFullSignalName(TimingFigure *tvf, TimingSignal *tvs);
char *ttv_GetSignalType(TimingSignal *tvs);
char *ttv_GetConnectorName(TimingSignal *con);
char ttv_GetConnectorDirection(TimingSignal *ts);
double ttv_GetSignalCapacitance(TimingSignal *tvs);
double ttv_GetConnectorCapacitance(TimingSignal *tvs);
int ttv_GetNameIndex(char *sig);
char *ttv_GetNameRadical(char *sig);
int ttv_GetVectorConnectorLeftBound(TimingSignal *tvs);
int ttv_GetVectorConnectorRightBound(TimingSignal *tvs);
TimingEventList *ttv_GetLatchCommands(TimingSignal *tvs);
TimingEventList *ttv_GetLatchEventCommands(TimingSignal *tvs, char dir);
double ttv_GetLatchAccess(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command, char *minmax);
double ttv_GetLatchMaxAccess(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);
double ttv_GetLatchMinAccess(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);
double ttv_GetLatchSetup(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);
double ttv_GetLatchHold(TimingFigure *tvf, TimingSignal *latch, char dir, TimingSignal *command);

/* EVENT PROPERTIES                                           */
Property *ttv_GetTimingEventProperty (TimingEvent *ev, char *code);
TimingSignal *ttv_GetTimingEventSignal(TimingEvent *tve);
extern char ttv_GetTimingEventDirection(TimingEvent *tve);
TimingSignal *ttv_GetLatchCommandSignal(TimingEvent *tve);
char ttv_GetLatchCommandDirection(TimingEvent *tve);

/* PVT                                                        */
extern void ttv_TerminateRun();
extern void ttv_InitializeNewRun();
extern TimingFigure *ttv_LoadTimingFigure(char *name);
extern TimingFigure *ttv_LoadTimingPathFigure(char *name);
extern TimingFigure *ttv_GetTimingFigure(char *name, float temp, float alim);
extern double ttv_ComputeMaxPathDelay(char *figure, char *start, char *end, char *dir, TimeValue input_slope, CapaValue output_capa, double VDD, double temperature);
extern double ttv_ComputeMaxPathSlope(char *figure, char *start, char *end, char *dir, TimeValue input_slope, CapaValue output_capa, double VDD, double temperature);
extern double ttv_ComputeMinPathDelay(char *figure, char *start, char *end, char *dir, TimeValue input_slope, CapaValue output_capa, double VDD, double temperature);
extern double ttv_ComputeMinPathSlope(char *figure, char *start, char *end, char *dir, TimeValue input_slope, CapaValue output_capa, double VDD, double temperature);
extern double ttv_GetPathMinSlope(char *figure, char *start, char *end, char *dir, double VDD, double temperature);
extern double ttv_GetPathMinDelay(char *figure, char *start, char *end, char *dir, double VDD, double temperature);
extern double ttv_GetPathMaxSlope(char *figure, char *start, char *end, char *dir, double VDD, double temperature);
extern double ttv_GetPathMaxDelay(char *figure, char *start, char *end, char *dir, double VDD, double temperature);

/* PATH DETAIL SIMULATION                                     */
int ttv_SimulatePath_sub(TimingFigure *tvf, TimingPath *tp, char *mode, int mc);
//int ttv_SimulatePathDetail(TimingFigure *tvf, TimingPath *tp, char *mode);
void ttv_DisplayActivateSimulation(char mode);
int ttv_DriveSpiceDeck(TimingFigure *tvf, TimingPath *tp, char *filename);
int ttv_DriveSetupHoldSpiceDeck(TimingFigure *tvf, TimingPathList *detdata, TimingPathList *detck, char *filename);
/* CONSTRAINT PROPERTIES                                      */
Property *ttv_GetTimingConstraintProperty (TimingConstraint *co, char *property);
TimingConstraintList *ttv_GetConstraints (TimingFigure *tf, char *inputconnector, char *towhat);
void ttv_FreeConstraints_sub(TimingConstraintList *allobj, TimingFigure *tf, int autoclean);

/* GNS RELATED                                                         */


void ttv_SetBaseNetlist(Netlist *lf);
void ttv_BuildTiming();
void ttv_TasBuiltTiming(Netlist *lf);
void ttv_SetFunctionsFile (char *functions_file);
void ttv_Config(double value, double out_capa);
void ttv_Associate_Model(TimingLine *tvl, void *func);
void ttv_Associate_Sim(TimingLine *tvl, void *func);
void ttv_Associate_Env(TimingLine *tvl, void *func);
void ttv_Associate_Ctk_Env(TimingLine *tvl, void *func);
int ttv_MinDelay();
int ttv_MaxDelay();
void ttv_UseInstanceMode();
void ASSOCIATE_TIMING(void *func);

/* ???                                                         */
void ttv_SetSearchMode(char *mode);

void ttv_DisplayClockPathReport(FILE *f, TimingFigure *tf, char *clock, char *minmax, int number);
void ttv_SetOutputSwing(double vss, double vdd);

//
void ttv_DetectFalseClockPath(TimingFigure *tvf);
void ttv_DetectFalsePath(TimingFigure *tvf, char *start, char *end);
//void ttv_DetectFalseParallelClockPath(TimingFigure *tvf);
// devel

TimingPathList *ttv_ProbeDelay_sub(TimingFigure *tvf, TimeValue slopein, StringList *nodenamelist, char *dir, int nbpath, char *path_or_access, char *minmax, int nosync, int noprop);

TimingPathList *ttv_ProbeDelay_v2(TimingFigure *tvf, TimeValue slopein, char *start, char *end, char *dir, int nbpath, char *minmax);
TimingPathList *ttv_CharacPaths(TimingFigure *tf, TimeValue slopein, char *start, char *end, char *dir, long number, char *all, char *path, char *minmax, CapaValue capaout, int propagate);
ListOfDoubleTable *ttv_CharacPathTables(TimingPath *pth, DoubleListTimeValue *slopes, DoubleListCapaValue *capas, int propagate);

void ttv_RecomputeDelays(TimingFigure *tvf);
TimingLineList *ttv_GetLines(TimingFigure *tvf, char *start, char *end, char *dir, char *linetype);
Property *ttv_GetTimingLineProperty (TimingLine *tl, char *code);
DoubleTable *ttv_CharacTimingLineModel(TimingFigure *tf, char *name, DoubleListTimeValue *input_slope, DoubleListCapaValue *output_capa, char *type);
double ttv_ComputeLineDelay(TimingLine *tl, TimeValue slope_in, CapaValue output_capa, char *delayslope, char *maxmin);
void ttv_SetTimingLineDelay(TimingLine *tl, char *prop, char *sub_prop, TimeValue value);
void ttv_DumpHeader(FILE *f, TimingFigure *tvf);

void ttv_CreateTimingTableModel(TimingFigure *tvf, char *name, DoubleListTimeValue *input_slope, DoubleListCapaValue *output_capa, DoubleTable *values, char *type);
void ttv_CreateEnergyTableModel(TimingFigure *tvf, char *name, DoubleListTimeValue *input_slope, DoubleListCapaValue *output_capa, DoubleTable *values, char *type);
void ttv_SetLineModel(TimingLine *tl, char *modelname, char *where);

TimingPathList *ttv_GetGeneratedClockPaths(TimingFigure *tvf, TimingEvent *tve, char *minmax);
TimingPathTable *ttv_SimulateCharacPathTables(TimingFigure *tvf, TimingPath *tp, DoubleListTimeValue *slopes, DoubleListCapaValue *capas, int maxsim);
void ttv_SetFigureFlag(TimingFigure *tvf, char *mode);
void ttv_Simulate_AddDelayToVT(TimingFigure *tvf, TimingPath *pth, TimingEvent *latch);
int ttv_Simulate_FoundSolutions();
TimingPathList *ttv_internal_GetPaths_EXPLICIT(TimingFigure *tf, TimingSignalList *clock, TimingSignalList *start, TimingSignalList *end, char *dir, long number, char *all, char *path, char *minmax);

int ttv_LoadSSTAResults(TimingFigure *tvf, StringList *filenames, IntegerTable *order);
int ttv_SetSSTARunNumber(TimingFigure *tvf, int num);
int ttv_BuildSSTALineStats(TimingFigure *tvf);
String *ttv_GetClockInfos(TimingFigure *tvf, char *name, char *minmax);
void ttv_UpdateInf(TimingFigure *tvf);
void ttv_ChangePathStartTime(TimingPath *tp, TimeValue time);
void ttvapi_setdislaytab(int nb);

