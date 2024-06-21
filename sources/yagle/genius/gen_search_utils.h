/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_search_utils.h                                          */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 19/06/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/



#define UNDEF 0x0FFFFFFF       /*improbable value: variable not defined*/

/* possible values of FLAG field in foundins for phase 2*/
#define ALONE 1 //0x97555        
#define SELECT 2 //0x97123        
#define UNSELECT 3 //0x52001
#define SELECT_HEAD 4 //0x21235
#define HEAD 5 //0x57132          /* head of chain */
#define PRESEL_HEAD 6 //0x455677    /*during algorithm in phase 1*/
#define PRESEL  7 //0x142361     /*during algorithm in phase 1*/
#define CHOICE 8 //0x45679       /*after phase 1*/
#define CHAIN  9 //0x12315       /*after phase 1*/


#define TO_DELETE_MARK_PTYPE 0x1f2e3a




/****************************************************************************/
/*     return 0 if not match, 1 if already visited, 2 if just visited       */
/****************************************************************************/
extern mark_list *GetFailedMark();
extern int controlmark(int visited, losig_list *model_sig, int numbit, losig_list *circuit_losig, locon_list *me);

/****************************************************************************/
/*           LEX&YAC searching:  Refresh the Stack                          */
/****************************************************************************/
extern void Remove(model_list *model);

/****************************************************************************/
/*           LEX&YAC searching:  increase the counter of Stack              */
/****************************************************************************/
extern void Protect();

/****************************************************************************/
/* LEX&YAC searching:  Put at this level of counter this foundins(see above)*/
/****************************************************************************/
extern int Shift(foundins_list* foundins, loins_list* model_ins);

/****************************************************************************/
/*    LEX&YAC searching:  Put at this level of counter this lotrs in Stack  */
/****************************************************************************/
extern int Shift_lotrs(lotrs_list* circuit_ins, loins_list* model_ins);

/****************************************************************************/
/*  LEX&YAC searching:  Concate this level of counter with the last one     */
/****************************************************************************/
extern void Reduce();

/****************************************************************************/
/*           LEX&YAC searching:  Erase last level of Stack                  */
/****************************************************************************/
extern void Backward();

/****************************************************************************/
/*   put on top a new element of searching, return the new foundins         */
/****************************************************************************/
extern foundins_list* new_foundins(foundins_list *top, loins_list *loins, ptype_list *var);

/****************************************************************************/
/*                put a new element of searching in a global variable       */
/****************************************************************************/
extern void add_foundins(model_list *ml, loins_list *loins, ptype_list *var);

/****************************************************************************/
/*  free recursively from top a list of foundins, all non zero if you want  */
/*  to erase also loins attached                                            */
/****************************************************************************/
extern void free_foundins(foundins_list *top, int all);

/****************************************************************************/
/*             search in a global variable a foundins                       */
/****************************************************************************/
extern foundins_list* getfoundins(char* name);

/****************************************************************************/
/*                 erase one foundins which point to elem                   */
/****************************************************************************/
extern void erase_foundins(model_list *ml, loins_list *elem);

/****************************************************************************/
/*                put a new element of searching in a global variable       */
/****************************************************************************/
extern void add_foundmodel(foundins_list *liste_Ins, 
                                 model_list *model);
                                 
/****************************************************************************/
/*       free global variable, all non zero if you to erase loins           */
/****************************************************************************/
extern void free_foundmodel(int all);

/****************************************************************************/
/*               return non zero if model as been already searched          */
/****************************************************************************/
extern int Check_Model(char *model);

/****************************************************************************/
/*           return the number of loins model found in circuit              */
/****************************************************************************/
extern int Count_Model(char *model);

/****************************************************************************/
/*             increase the counter of instance from count                  */
/****************************************************************************/
extern void AddCount_Model(char *model, int count);

/****************************************************************************/
/*                   put model flag to check                                */
/****************************************************************************/
extern void PutCheck_Model(char *model);

/****************************************************************************/
/* build a new loins and delete all marks and all loins contained inside    */
/****************************************************************************/
extern void Build_loins(model_list *model, ptype_list *env); 

/****************************************************************************/
/*   final loins list for FCL  almost change names and execute C program    */
/****************************************************************************/
extern chain_list* Build_loins_list();

// zinaps
extern void displaymodels();

/****************************************************************************/
/* Add a list of signal names to each connector having symetric connectors  */
/****************************************************************************/

extern void finish_erase_foundins();
extern void intermediate_finish_erase_foundins();

extern void setfixed(locon_list *lc);
extern int isfixed(locon_list *lc);
extern void addloconmark(locon_list *lc, mark_list *mark);
extern mark_list *getloconmark(locon_list *lc);
extern void addswap(locon_list *lc, locon_list *sym);
extern void addwithoutcount(loins_list *li, long oldcount);

extern void SearchInit();
extern void FinishCleanLofigMarks(lofig_list *lf);
extern void CleanUPMarkEA(lofig_list *lf);

extern char tabs[200]; // pour indenter les traces

extern char *modelradical(char *name);
extern void UpdateNewInstanceLofigChain();
extern void GenPostOp(loins_list *li);

extern int CountFakeInstanceLocon(locon_list *ptcon, ptype_list *env);
extern void CleanLofigForWithouts(model_list *model);

extern long ComputeModelInstanceKey(locon_list *ptcon, ptype_list *env);
extern long KeyContribValueFor(losig_list *ls);

extern chain_list *GetFixedSignalList(loins_list *li, ptype_list *env, ptype_list *livar);
extern int IsFORparallal(loins_list *li, ptype_list *env, lofig_list *model_lofig, int *smartselect);

extern void ComputeModelTransistorKeyV2(locon_list *ptcon, ptype_list *env, long long *key, long long *mask);

extern int isparal, isparal_count, smartselect;
extern ptype_list *GEN_USER;

chain_list *GrabExistingLoins(lofig_list *lf, char *modelname, char *regexp);
void CleanLoconFlags(loins_list *li);
foundins_list *GetLastAddedFoundins();
void RestoreRealLoinsPnodes(loins_list *li);
void reset_existing_loins_pnodes();

typedef struct all_loins_heap_struct
{
  struct all_loins_heap_struct *NEXT;
  model_list *model;
  char *instance_name;
  corresp_t *CorrespondanceTable;
} all_loins_heap_struct;

extern all_loins_heap_struct *ALL_LOINS_FOUND;
extern chain_list *ALL_KIND_OF_MODELS;
void add_loins_in_all_loins_found(char *name, model_list *model, corresp_t *table);

void UpdateTOPInstancesCorrespondanceTable();
extern lofig_list *CUR_HIER_LOFIG;
extern corresp_t *CUR_CORRESP_TABLE;
int gns_isforcematch(char *cirname, char *modelname);

chain_list *GrabBlackboxAsUnused(char *modelname, lofig_list *model, lofig_list *netlist);
int IsModelFullyConnected(lofig_list *lf);

