#define INPUT_CLOCK 1
#define OUTPUT_CLOCK 2
#define MAIN_CLOCK 3

/* Timing Figure -> */

extern StabilityFigure    *stb                                  (TimingFigure *tf);
extern StabilityFigure    *stb_LoadSwitchingWindows             (TimingFigure *tvf, char *filename);
                                                                
/* Stability Figure -> */                                       
                                                                
extern void                stb_FreeStabilityFigure              (StabilityFigure *sf);
extern TimingSignalList   *stb_GetErrorList                     (StabilityFigure *sf, TimeValue margin, int nberror);
extern double              stb_GetSetupSlack                    (StabilityFigure *fig, char *signame, char dir);
extern double              stb_GetHoldSlack                     (StabilityFigure *fig, char *signame, char dir);
extern StabilityPathList  *stb_GetSignalStabilityPaths          (StabilityFigure *stbfig, char *output);
extern void                stb_DisplaySignalStabilityReport     (FILE *f, StabilityFigure *stbfig, char *name);
extern void                stb_DisplayErrorList                 (FILE *f, StabilityFigure *stbfig, TimeValue margin, int nberror);
extern void                stb_DisplaySlackReport_sub               (FILE *f, StabilityFigure *tf, char *start, char *end, char *dir, int number, char *mode, TimeValue margin);
void stb_UpdateSlacks (StabilityFigure *fig);
void stb_DisplayCoverage_sub(FILE *f, StabilityFigure *sf, int detail);
void stb_DriveReport (StabilityFigure *fig, char *filename);

/* StabilityPath -> */

extern Property           *stb_GetStabilityPathProperty         (StabilityPath *path, char *property);
extern double              stb_GetClockTime                     (StabilityPath *path, int clock, char dir);
extern double              stb_GetClockDelta                    (StabilityPath *path, int clock, char dir);
extern void                stb_DisplayClock                     (FILE *f, StabilityPath *path, int clock);
extern void                stb_DisplayInfos                     (FILE *f, StabilityPath *path);
extern void                stb_DisplayInputInfos                (FILE *f, StabilityPath *path);
extern StabilityRangeList *stb_GetInputInstabilityRanges        (StabilityPath *path, char dir);
extern StabilityRangeList *stb_GetOutputInstabilityRanges       (StabilityPath *path, char dir);
extern StabilityRangeList *stb_GetOutputSpecificationRanges     (StabilityPath *path, char dir);
extern double              stb_GetInstabilityRangeStart         (StabilityRange *range);
extern double              stb_GetInstabilityRangeEnd           (StabilityRange *range);
extern void                stb_DisplayInputInstabilityRanges    (FILE *f, StabilityPath *path);
extern void                stb_DisplayOutputInstabilityRanges   (FILE *f, StabilityPath *path);
extern void                stb_DisplayOutputSpecificationRanges (FILE *f, StabilityPath *path);

/* StabilityPathList -> */

extern void                stb_FreeStabilityPathList            (StabilityPathList *path_list);

StabilitySlackList *stb_GetSlacks_sub(StabilityFigure *stbfig, char *start, char *end, char *dir, int number, char *mode, TimeValue margin, char *thru, int nolagprech);
void stb_FreeSlackList(StabilitySlackList *cl);
Property *stb_GetSlackProperty (StabilitySlack *so, char *property);

// test
void stb_compute_local_margins(StabilityFigure *stbfig);
void stb_trackstbpath (StabilityFigure *stbfig, char *nodename, char dir, TimeValue timeo);
StabilitySlack *stb_ComputeOneSlack(int setup, TimingPath *data, TimingPath *clock, TimeValue margin, int nextcycle, int nosync);
void stb_DisplaySlackReport_sub2(FILE *f, StabilitySlackList *solist, int number, char *mode);
Property *stb_GetStabilityFigureProperty (StabilityFigure *sf, char *property);

int path_false_slack_check(TimingPath *tp, TimingEvent *opencmd, TimingEvent *topopenclock);
double stb_synchronized_slopes_move(StabilityFigure *ptstbfig, TimingEvent *startnode, TimingEvent *endclock);

