#ifndef GEN_EXPANSION
#define GEN_EXPANSION

/* Symmetry gestion */
/* known FOR statement feature */
//extern loins_list *AddLoinsToModel(loins_list *li, ptype_list *env, char *toappend);
loins_list *AddLoinsToModel(loins_list *li, ptype_list *env, char *toappend, int index);

void ExpandFOR(tree_list* tree, lofig_list *circuit, ptype_list *env, chain_list **expansedloins, chain_list **originalloins, char *nameadd, int depart, int oldrange);

//extern void ExpandFOR(tree_list* tree, lofig_list *circuit, ptype_list *env, chain_list **expansedloins, chain_list **originalloins, char *nameadd);

extern void search_FOR(tree_list* tree, chain_list **vars, int *launchexpansion);

extern int CheckGenericValues(chain_list *instances, chain_list *forvars, ptype_list *env);

extern void getallloins(tree_list* tree, chain_list **allloins);

extern void FinishExpansion(chain_list *originalloins);

extern void UndoExpansion(chain_list *expansedloins);

extern int GenerateNextValueConfiguration(ptype_list *env, chain_list *for_vars);
extern void SetVariableToFindtoUNDEF(ptype_list *env);

extern void FreeConfigurationsData();

#endif
