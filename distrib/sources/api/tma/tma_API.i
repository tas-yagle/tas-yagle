/*extern void tma (void);*/
extern BehavioralFigure *beh_getbefig (char *name);
extern TimingFigure *tma_abstract_old (TimingFigure *fig, BehavioralFigure *befig);
TimingFigure *tma_DuplicateInterface (TimingFigure *fig, char *newname, List *argv);

DoubleList *tma_GetConnectorAxis(TimingFigure *fig, char *type, char *name);

void tma_DetectClocksFromBeh (TimingFigure *fig, BehavioralFigure *befig);
void tma_AddInsertDelays (TimingFigure *blackbox, TimingFigure *fig);
void tma_UpdateSetReset (TimingFigure *fig, BehavioralFigure *befig);
void tma_SetMaxCapacitance(TimingFigure *bbox, char *name, CapaValue value);
void tma_SetGeneratedClockInfo(TimingFigure *bbox, char *name, char *string);
void tma_TransfertSignalInformation(TimingSignal *src, TimingSignal *dst);
void tma_SetEnergyInformation(TimingFigure *bbox, char *name, char dir, char *modelname);

