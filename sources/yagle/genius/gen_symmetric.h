#include GEN_H



typedef struct
{
  short index;
  short radindex;
} Pnode2Radical;

extern void SwapLoconWithSym(locon_list *syms_locon, locon_list *circuit_con);
extern int TryToSwapWithAConnectorWithTheSameName(locon_list **circuit_c, locon_list *model_con, SymInfoItem *syms, int bit_wanted);
extern int TryToSwapConnectors(locon_list *circuit_con, locon_list *sym);
extern locon_list *GetNextSymmetric(locon_list *me, locon_list *last);
extern locon_list *GetNextSymmetric2(locon_list *me, locon_list *last);
extern int TryToSwapWithAConnectorWithTheSameSignalAndWithOneMark(locon_list *circuit_con, losig_list *ls,mark_list **newmark);
extern void PushPendingLofigChain(chain_list *cl);
extern void PopPendingLofigChain();
extern chain_list *CIRCUIT_LOFIGCHAIN; 
extern int TryToSwapWithAConnectorWithTheMarkedSignal(locon_list *circuit_con, mark_list *failedmark, locon_list **last, SymInfoItem *syms);
extern void *AddSwap(locon_list *lc, locon_list *sym);
extern void UndoAndDeleteSwap(chain_list *cl);
extern void JustFreeSwap(void *item);

extern void AddSymsFlagInLoinsLocon(loins_list *li);
extern void InitSymmetricAndCoupledInfoMecanism();
extern void RemoveSymmetricAndCoupledInfoMecanism();
extern void addSymmetricInfo(char *figname, char *conname, SymInfoItem *syms);
extern void addCoupledInfo(char *figname, char *conname, SymInfoItem *coupl);
extern SymInfoItem *GetSymmetricChain(locon_list *lc);
extern SymInfoItem *OldFashionGetSymmetricChain(locon_list *lc);
extern SymInfoItem *GetCoupledChain(locon_list *lc);
extern void AddCoupledOfLofig(lofig_list *lf);
extern void AddSymsOfLofig(lofig_list *lf);

extern void add_pending_con(locon_list *lc, chain_list *cl);
extern void remove_pending_con(locon_list *lc);

extern SymInfoItem *AddSymInfoItem(SymInfoItem *head);
extern int getradindex(char *name);
extern void AddRadicalInfoInPNODE(loins_list *li);
extern void AddRadicalInfoInLOCON(locon_list *lc, int radindex, int index);

extern char **radtable;
inline char *fastradical(locon_list *lc);
inline int fastindex(locon_list *lc);
/*
#define fastradical(lc) radtable[((Pnode2Radical *)&((lc)->PNODE))->radindex]
#define fastindex(lc) ((int)((Pnode2Radical *)&((lc)->PNODE))->index)
*/

void TrytoArrangeSymmetricConnectors(loins_list *li);
void ArrangeInstanceConnectors(loins_list *li);
