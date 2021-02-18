/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_debug.h                                                 */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

#define STB_NODE_DELAY     ((long) 0x50000000 ) /* structure delay           */
#define STB_NODE_DELAYUP   ((long) 0x50000001 ) /* structure delay up        */
#define STB_NODE_DELAYDN   ((long) 0x50000002 ) /* structure delay dn        */
#define STB_NODE_HZUP      ((long) 0x50000004 ) /* structure delay hz up     */
#define STB_NODE_HZDN      ((long) 0x50000008 ) /* structure delay hz dn     */

typedef struct stbdelay
    {                       
     struct stbdelay  *NEXT ;
     ttvevent_list    *NODE ;
     long              DELAYMAX ;
     long              DELAYMIN ;
     long              MINSTART, MAXSTART ;
     long              TYPE ;
     ptype_list         *PHASELISTMAX ;
     ptype_list         *PHASELISTMIN ;
     ptype_list         *STARTMIN_PP ;
     ptype_list         *STARTMAX_PP ;
     ttvevent_list    *CMD, *ACCESSCMD, *CLOCK ;
    }
stbdelay_list ;
typedef long datamoved_type[4][256];
typedef struct 
{
  long total;
  long period;
  int nb_setup_cycle, nb_hold_cycle;
  long mc_setup_period, mc_hold_period;
} EventPeriodeHold_type;

/* declarations des fonctions */

stbdelay_list *stb_getstbdelay __P((ttvevent_list *root,ttvevent_list *node,ttvevent_list *cmd,long type,ttvevent_list *accesscmd, ttvevent_list *clock)) ;
stbdebug_list *stb_sortstbdebug __P((stbdebug_list *debug)) ;
void stb_delstbdelay __P((ttvevent_list *root)) ;
stbdelay_list *stb_addstbdelay __P((ttvevent_list *root,ttvevent_list *node,long delaymax,long delaymin,long minstart,long maxstart,long type,ttvevent_list *cmd,ptype_list *lmax, ptype_list* lmin,ttvevent_list *accesscmd, ttvevent_list *clock)) ;
stbchrono_list *stb_addstbchrono __P((stbchrono_list *head,char verif,char type,char slope, long ckupmin, long ckupmax, long ckdnmin, long ckdnmax)) ;
stbdebug_list *stb_addstbdebug(stbdebug_list *head,
                               ttvsig_list *sig1,
                               ttvevent_list *sig1event,
                               ttvsig_list *sig2,
                               long per,
                               char *name,
                               char *cmd,
                               ttvevent_list *cmdev,
                               long ckrup_min,
                               long ckrup_max,
                               long ckrdn_min,
                               long ckrdn_max,
                               long ckup_min,
                               long ckup_max,
                               long ckdn_min,
                               long ckdn_max,
                               long ockup_min,
                               long ockup_max,
                               long ockdn_min,
                               long ockdn_max,
                               long ockperiod,
                               long ms,
                               long mh,
                               long s,
                               long h,
                               long holdperiod,
                               long updelta,
                               long downdelta,
                               Setup_Hold_Computation_Detail *detail,
                               ttvevent_list *start_cmd_event,
                               ttvevent_list *start_clock
                               );
void stb_freestbdebuglist __P((stbdebug_list *head)) ;
char *getckname __P((stbfig_list *stbfig,char index)) ;
//void stb_addprechrono __P((stbfig_list *stbfig,stbdebug_list *debug,stbpair_list **tab,stbpair_list **tabout,stbnode *node,char slope,ttvsig_list *ptsig, long *periodemax,long *periodemin)) ;
stbdebug_list *stb_debugstberror __P((stbfig_list *stbfig,ttvsig_list *sig,long margin, long computeflags)) ; 
chain_list *stb_debug_retreive_delays(stbfig_list *stbfig, ttvsig_list *sig, int dir, int forceclockordata, int falseaccessmode, long compflags);
void stb_compute_common_clock_gap(inffig_list *ifl, stbfig_list *sf, ttvsig_list *input, stbck *inputck, stbnode *node, int trans_involved[4], stbpair_list *start_stab[2][256], stbpair_list **end_stab, stbpair_list **end_stabhz, datamoved_type datamoved, Extended_Setup_Hold_Computation_Data *eshcd, ttvevent_list *directive_setup_ev, int flags, ptype_list *geneclockinfo);
void stb_assign_operations(int i, Setup_Hold_Computation_Detail *detail, datamoved_type datamoved, EventPeriodeHold_type *period_stuff);
void stb_assign_operations_low(Setup_Hold_Computation_Detail_INFO *detail, long datamoved, EventPeriodeHold_type *period_stuff);
void stb_add_infos(stbfig_list *stbfig, ttvevent_list *output, int i, Setup_Hold_Computation_Detail *detail, stbck *inputck, ttvevent_list *input, stbdelay_list **useddelay);
long stb_getcross_PP(ptype_list *list, char phase);
void stb_getslacktopclocks(ttvevent_list *directive_setup_ev, stbnode *output, int outhz, stbck *inputck, stbck *outputck, ttvevent_list **startclock, ttvevent_list **setupclock, ttvevent_list **holdclock);

