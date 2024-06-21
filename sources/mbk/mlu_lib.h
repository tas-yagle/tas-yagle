/* 
 * This file is part of the Alliance CAD System
 * Copyright (C) Laboratoire LIP6 - Département ASIM
 * Universite Pierre et Marie Curie
 * 
 * Home page          : http://www-asim.lip6.fr/alliance/
 * E-mail support     : mailto:alliance-support@asim.lip6.fr
 * 
 * This library is free software; you  can redistribute it and/or modify it
 * under the terms  of the GNU Library General Public  License as published
 * by the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * Alliance VLSI  CAD System  is distributed  in the hope  that it  will be
 * useful, but WITHOUT  ANY WARRANTY; without even the  implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy  of the GNU General Public License along
 * with the GNU C Library; see the  file COPYING. If not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* 
 * Purpose : logical utility functions
 * Date    : 05/08/93
 * Author  : Frederic Petrot <Frederic.Petrot@lip6.fr>
 * Modified by Czo <Olivier.Sirol@lip6.fr> 1997,98
 * $Id: mlu_lib.h,v 1.42 2009/05/20 12:22:04 fabrice Exp $
 */

#ifndef _MLU_H_
#define _MLU_H_

#ifndef __P
# if defined(__STDC__) ||  defined(__GNUC__)
#  define __P(x) x
# else
#  define __P(x) ()
# endif
#endif

#include EQT_H

#define MBK_TRANS_PARALLEL (long)20034401
#define MBK_TRANS_MARK     (long)20034402

extern eqt_param *MBK_GLOBALPARAMS;
extern chain_list *MBK_GLOBALFUNC;
extern ht *MBK_MONTE_MODEL_PARAMS;
extern ptype_list *MBK_MONTE_DIST;
extern ptype_list *MBK_MONTE_PARAMS;

extern void (*FlattenOnCreateLOTRS)(lotrs_list *tr);
extern void (*FlattenOnCreateLOINS)(loins_list *li);
extern           void  flattenlofig __P((lofig_list *ptfig, char *insname, char concat));
extern void flattenloinslist(lofig_list * ptfig, chain_list *cl, char concat);
extern void mbk_transfert_loins_params(loins_list *li, lofig_list *origlf, lofig_list *newlf);
extern void flattenlofig_bypointer( lofig_list *ptfig, chain_list *ptinslist, AdvancedNameAllocator *ana, char concat );
extern void flatten_setup_realname_from_hiername(lofig_list *lf, AdvancedNameAllocator *ana);
extern    lofig_list * unflatOutsideList __P((lofig_list *ptfig, char *figname, char *insname));
extern    lofig_list * flatOutsideList __P((lofig_list *ptfig));
extern            int  IsInBlackList (char *figname);
extern           void  loadBlackList ();
extern           void  deleteBlackList ();
extern    lofig_list * unflattenlofig __P((lofig_list *ptfig, char *figname, char *insname, chain_list *list));
extern           void  rflattenlofig __P((lofig_list *ptfig, char concat, char catal));
extern           void  loadlofig __P((lofig_list *ptfig, char *name, char mode));
extern           void  savelofig __P((lofig_list *ptfig));
extern           void  mlodebug __P((void *head_pnt, char *stru_name));
extern    losig_list * duplosiglst __P((losig_list *losig_ptr, lofig_list *lf));
extern    void         duplosiglim __P((losig_list *losig_ptr, losig_list *losig_rpt));
extern    losig_list * duplosig __P((losig_list *losig_ptr, lofig_list *lf));
extern    chain_list * dupchainlst __P((chain_list *chain_ptr));
extern    optparam_list * dupoptparamlst __P((optparam_list *optparam_ptr));
extern    optparam_list * dupoptparam __P((optparam_list *optparam_ptr));
extern    locon_list * duploconlst __P((locon_list *locon_ptr));
extern    locon_list * duplocon __P((locon_list *locon_ptr));
extern    loins_list * duploinslst __P((loins_list *loins_ptr));
extern    loins_list * duploins __P((loins_list *loins_ptr));
extern    lotrs_list * duplotrslst __P((lotrs_list *lotrs_ptr));
extern    lotrs_list * duplotrs __P((lotrs_list *lotrs_ptr));
extern    lotrs_list * rduplotrs __P((lotrs_list *lotrs_ptr));
loins_list *rduploins (loins_list *loins_ptr);
extern    lofig_list * duplofiglst __P((lofig_list *lofig_ptr));
extern    lofig_list * duplofig __P((lofig_list *lofig_ptr));
extern    lofig_list * rduplofig __P((lofig_list *lofig_ptr));
extern            void sortlocon __P((locon_list **connectors));
extern            void sortlosig __P((losig_list **signals));
extern    losig_list * givelosig __P((lofig_list *ptfig, long index));
extern             int checkloconorder __P((locon_list *c));
extern            int  restoredirvbe __P((lofig_list *lof));
extern            int  guessextdir __P((lofig_list *lof));
extern            int  restorealldir __P((lofig_list *lf));
extern    chain_list * sort_sigs __P((losig_list *sig_list, ht *endht));
extern    void         mbk_createsigtable __P((lofig_list *lofig));
extern    losig_list * mbk_getlosigfromtab __P((char *name));
extern    void         mbk_destroysigtable __P(());
extern    void         vectorbounds __P((lofig_list *ptfig, char *radical, int *begin, int *end));
extern    int          mbk_checklofig __P((lofig_list *fig));
extern    void         savelofig_recursive __P((lofig_list *ptfig));
extern    char        *mbk_LosigTestREGEX(losig_list *testlosig, char *refname);
extern void            flatten_parameters (lofig_list * fig, loins_list *ptfatherloins, eqt_param * param_tab, int x, int y, 
                                           int Tx, int Ty, int R, int A, eqt_ctx **mc_ctx);

void freeflatmodel (lofig_list *fig);
void ClearLOTRSLoconFlags(lotrs_list *tr);
void ClearAllLOTRSLoconFlags(lotrs_list *tr);
void ClearAllLoconFlags(locon_list *lc);
void ClearAllLOINSLoconFlags(loins_list *li);
void ClearLOFIGLoconFlags(lofig_list *lf);

long mbk_removeparallel(lofig_list *ptlofig, int remove);
void mbk_restoreparallel(lofig_list *ptlofig);
void mbk_markparallel(lofig_list *ptlofig);
void mbk_unmarkparallel(lofig_list *ptlofig);
void mbk_freeparallel(lofig_list *ptlofig);

void SetBlackList(chain_list *list);

char *mbk_get_subckt_name(lotrs_list *tr);
void mbk_set_montecarlo_global_seed(unsigned int value);
unsigned int mbk_get_montecarlo_global_seed(int reset);
void mbk_init_montecarlo_global_seed(unsigned int value);
unsigned int mbk_get_a_seed();

int mbk_recur_Eval(eqt_ctx *ctx, ht *allparams, char *name, char *where);
void mbk_EvalGlobalMonteCarloDistributions(eqt_ctx *ctx);
void mbk_recur_Eval_Start(eqt_ctx *ctx, ht *allparams, char *where);

unsigned int mbk_signlofig( lofig_list *ptfig );
void mbk_set_transistor_instance_connector_orientation_info (lofig_list * lf);

typedef struct vcardnodeinfo
{
  char *name;
  float value;
  int rcn;
} vcardnodeinfo;

void mbk_addvcardnode(losig_list *ls, char *name, float v, long rcn);
void mbk_CheckPowerSupplies(lofig_list *lf);

#endif /* !MLU */

