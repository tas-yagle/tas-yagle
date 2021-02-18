/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : GENIUS   v1.00                                              */
/*    Fichier : gen_env.h                                                   */
/*                                                                          */
/*    (c) copyright 1999 Laboratoire MASI equipe CAO & VLSI                 */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*                                                                          */
/****************************************************************************/

#include GEN_H
#include API_H

#define BUFSIZE0 4096
extern chain_list *blocks_to_free;
void free_all_blocks_to_free();

extern char *GENIUS_LIB_NAME;        /* file with all models names */
extern char *GENIUS_LIB_PATH;        /* directory */
extern tree_list  *GENIUS_TREE;      /* syntaxical result of model files */
extern lib_entry *GENIUS_PRIORITY;  /* syntaxical result of library file */
extern FILE* GENIUS_OUTPUT;

extern int GEN_DEBUG_LEVEL;
extern char *FAKE_TRAN_NAME, *FAKE_INS_NAME;
extern char GENIUS_BUILD_NETLIST;
#define ZINAPS_DUMP // enables GEN_DEBUG_LEVEL

extern int GEN_OPTIONS_PACK;
#define GEN_NODEFAULT_ACTIONS 1
#define GEN_ENABLE_CORE 2
#define GEN_NO_GNS 4
#define GEN_VERBOSE_GNS 8
#define GEN_NO_ORDERING 16
#define GEN_DEBUG_REMAPPING 32
#define GEN_STRICT 64

/****************************************************************************/
/*               Set variables with the environment                         */
/****************************************************************************/
extern void genius_env(lofig_list *circuit, char *celdir, char *libname);
extern void genius_print_time(struct rusage *start, struct rusage *end, time_t rstart, time_t rend);
extern void genius_chrono(struct rusage *t, time_t *rt);
char *genius_getutime(struct rusage *start, struct rusage *end);

extern ExecutionContext *genius_ec;
void clean_gen_env();

#define NB_TRAN_PARAM 2
extern char *transistor_params[NB_TRAN_PARAM];
