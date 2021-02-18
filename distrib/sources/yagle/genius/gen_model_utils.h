/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_model_utils.h                                           */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Francois DONNET                        le : 09/02/1999    */
/*                                                                          */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*    Modifie par :                                     le : ../../....     */
/*                                                                          */
/****************************************************************************/



#include GEN_H



/****************************************************************************/
/*               put a new elem in DATA field of head                       */
/****************************************************************************/
extern mark_list* addmark(ptype_list *head, int numbit, losig_list *losig, int visited, locon_list *me);

/****************************************************************************/
/*              erase recursively from memory a list of marks               */
/****************************************************************************/
extern void freemark(mark_list *top);

/***************************************************************************/
/*        add to USER field of locon the generic signals linked to it      */
/* gen is top of list, vectcon is bit number of connector and vectsig bit  */
/* number of losig                                                         */
/***************************************************************************/
extern genconchain_list* addgenconchain(genconchain_list *gen, 
                                        tree_list *vectcon,
                                       losig_list *losig, tree_list *vectsig);
                                        
/***************************************************************************/
/*              erase recursively a list of gencon                         */
/***************************************************************************/
extern void freegenconchain(genconchain_list *gencon) ;

/***************************************************************************/
/*     add to USER field of losig the generic connectors linked to it      */
/* gen is top of list, vectsig is bit number of signal and vectcon bit     */
/* number of locon                                                         */
/***************************************************************************/
extern gensigchain_list* addgensigchain(gensigchain_list *gen, 
                                        tree_list *vectsig,
                                       locon_list *locon, tree_list *vectcon);

/***************************************************************************/
/*              erase recursively a list of gensig                         */
/***************************************************************************/
extern void freegensigchain(gensigchain_list *gensig);

/***************************************************************************/
/*        return the model named name, NULL is returned if not found       */
/***************************************************************************/
extern model_list *getmodel(char *name);

/***************************************************************************/
/* add to the list of model the model named fig->NAME with fig,gen and c   */
/***************************************************************************/
extern void addmodel(lofig_list *fig, chain_list *variables, tree_list *gen,
                     char *c, char *regexp);

/***************************************************************************/
/*           erase all models but not the lofig (i.e LOFIG field)          */
/***************************************************************************/
extern void freemodel();

/***************************************************************************/
/*                    Dump all the model list                              */
/***************************************************************************/
extern void dumpmodel();

/***************************************************************************/
/*                            dump a generic ABL                           */
/***************************************************************************/
extern void Dump_Generic(tree_list *gen);

/***************************************************************************/
/*                            dump a generic model                         */
/***************************************************************************/
extern void Dump_Model(model_list *model);


extern void InitModels();
extern void CleanMainLofig(lofig_list *lf);

extern lofig_list *gns_build_netlist(lofig_list *lf, chain_list *loins, chain_list *lotrs, char flat, char rc);
extern void setglobalvariables(lofig_list *lf, chain_list *loins, chain_list *lotrs, ptype_list *env, chain_list *h_lotrs, chain_list *swaps);

//extern chain_list *PRAGMA_SPLIT(char *line);



extern char *gen_canonize_trname (char* trname);
extern int CheckModelInstances(model_list *model);
extern void setALIMSignalType(lofig_list *lf);
extern int gen_is_token_to (int token);
extern int gen_is_token_downto (int token);

extern lofig_list *GENIUS_HEAD_LOFIG;
extern ht *GENIUS_HT_LOFIG;
void SwitchLOFIGContext();
void CheckLoinsLofigInconsistancy(loins_list *li, lofig_list *lf);
void DelContext();

char *completename(char *geniusmodel, char *model, char *archi);
