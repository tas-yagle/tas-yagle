
extern char *ccname(locon_list *lc);

extern void add_pending_con(locon_list *lc, chain_list *cl);
extern void remove_pending_con(locon_list *lc);
extern void exchange_pending_con(locon_list *lc0, locon_list *lc1);

extern int losigisspedup(losig_list *ls);
extern void createlofigchainhash(losig_list *ls);
extern void cleanalllofigchainhash();
extern int fastremovelofigchainconnector(losig_list *ls, locon_list *lc);
extern int fastaddlofigchainconnector(losig_list *ls, locon_list *lc);
extern void fastswaplofigchain(losig_list *ls, locon_list *lc, locon_list *newcl, int nooptim);
extern void checklofig(losig_list *ls);
extern void fastremovestaycoherent(chain_list *me, void *pred, void **head);
//int updatefastlofigchain(losig_list *ls);

extern void UpdateSignalStatistics(losig_list *ls, locon_list *lc, int mode);
extern void ProcessSignalStatistics();
extern void SignalStatisticsRemove(losig_list *ls, locon_list *lc);
extern void EraseSignalStatistics();

extern chain_list *RemoveLofigChainLocon(chain_list *cl, locon_list *lc);

#define STAT_COUNT_SUCESS  0
#define STAT_COUNT_FAILURE 1
#define STAT_FORCE_CHANGE  2

extern void ClearQuickMatchChanceHashTables();
extern int QuickMatchChance(loins_list *model_ins, losig_list *ls, ptype_list *env);
extern void AddQuickMatchChance(losig_list *ls, long key);
extern long TransistorKey(lotrs_list *lt);
extern long InstanceKey(loins_list *li);

#define QUICK_NONE_MATCH         0
#define QUICK_ONE_OR_MORE_MATCH  1
#define QUICK_FAKE_MATCH         2


extern int SameRadical(char *a, char *b);

extern void TransistorKeyV2(lotrs_list *lt, long long *key0, long long *key1);

extern long long KeyContrib2ValueFor(losig_list *ls);
extern void SetPNODEtoNULL(loins_list *li);
