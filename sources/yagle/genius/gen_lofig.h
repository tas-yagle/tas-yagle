#ifndef GEN_LOFIG_H 
#define GEN_LOFIG_H

#include <stdio.h>
#include <string.h>
#include MUT_H
#include MLO_H
#include ELP_H
#include RCN_H
#include MLU_H
#include BEH_H
#include BVL_H
#include FCL_H
#include GEN_H

extern void        gen_clean_RC       (lofig_list *fig);
extern lofig_list *gen_build_netlist  (char *modelname, corresp_t *crt);
extern int         gen_endianess      (losig_list *sig);
extern void        gen_sort_sigs      (lofig_list *fig, ht *endht);
extern void        gen_update_sigcon  (lofig_list *fig);
extern lofig_list *gen_build_from_GNS (lofig_list *lf, chain_list *loins, chain_list *lotrs, char *modelname);
extern lofig_list *gen_build_from_FCL (lofig_list *lf, char *modelname);

#endif
