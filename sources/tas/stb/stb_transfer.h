/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : STB Version 1.00                                            */
/*    Fichier : stb_transfert.h                                             */
/*                                                                          */
/*    (c) copyright 2000 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Karim DIOURY                                              */
/*                Anthony LESTER                                            */
/*                                                                          */
/****************************************************************************/

/* declarations des fonctions */

void            stb_getstblatchclose __P((stbfig_list *stbfig, ttvevent_list *latch, ttvevent_list *cmd, long *closemin, long *closemax, ttvevent_list **clockevent));
void            stb_getstblatchaccess __P((stbfig_list *stbfig, ttvevent_list *latch, ttvevent_list *cmd, long *accessmin, long *accessmax, ttvevent_list **clockevent));
void            stb_getstbdelta __P((stbfig_list*, ttvevent_list *latch, long *deltamin, long *deltamax, ttvevent_list *cmd));
int             stb_interstbpairlist __P((stbpair_list *list1, stbpair_list *list2));
stbpair_list   *stb_globalstbtab __P((stbpair_list **tab, char size));
stbpair_list   *stb_globalstbpairnode __P((stbnode *node));
int             stb_interstbpair __P((stbpair_list *elm1, stbpair_list *elm2));
int             stb_compstbpairlist __P((stbpair_list *list1,stbpair_list *list2));
stbpair_list   *stb_beforestbpair __P((stbpair_list *list, stbpair_list *elm));
stbpair_list   *stb_afterstbpair __P((stbpair_list *list,stbpair_list *elm));
stbpair_list   *stb_globalstbpair __P((stbpair_list *head));
void            stb_copystbtabpair __P((stbpair_list **tabs, stbpair_list **tabd, long periode, long *periodemax, long *periodemin, char size, char index, char periodindex,int trans));
stbpair_list   *stb_nodeglobalpair __P((stbpair_list **tabpair, long periode, char size, char index));
stbpair_list   **stb_mergestbtab __P((stbpair_list **tab1, stbpair_list **tab2, char size));
stbpair_list   *stb_mergestbpairlist __P((stbpair_list *list1, stbpair_list *list2));
stbpair_list   *stb_fusestbpair __P((stbpair_list *list));
stbpair_list   *stb_adddelaystbpair __P((stbpair_list *list, long delaymax, long delaymin, long periodeSetup, long periodeHold));
stbpair_list   *stb_transferstbpair __P((stbpair_list *lists, stbpair_list *listd, long delaymax, long delaymin, long periodeSetup, long periodeHold));
int             stb_calcsetuphold __P((stbfig_list *stbfig, stbpair_list **tabpair, stbpair_list **tabhz, stbnode *node, long *s, long *h,int flagindex, long EventHoldPeriod, Extended_Setup_Hold_Computation_Data *eshcd, Setup_Hold_Computation_Detail *detail,int what, stbck *endnodeck, int specout)); 
stbpair_list   *stb_clockmaskstblist __P((stbfig_list *stbfig, stbpair_list *list, stbck *clock, int init,char index,int filtermode,ttvevent_list *tve));
void            stb_initstbnode __P((stbfig_list *stbfig, ttvevent_list *event));
void            stb_transferstbline __P((stbfig_list *stbfig, ttvevent_list *event, stbnode *node, stbpair_list **tabpair, stbpair_list **tabhz, char flagrc, int justretransfer, ttvevent_list *pathstartnode, stbck *ck, int quiet, int nofiltering));
void            stb_transferstbnode __P((stbfig_list *stbfig, ttvevent_list *event, char flagrc));
void            stb_geteventphase( stbfig_list*, ttvevent_list*, char*, char *, char*, int clock, int data );
void            stb_getdelayedge( stbfig_list*, ttvevent_list*, char, long*, long*, char, int, char*, char *);
void            stb_getlatchslope( stbfig_list*, ttvevent_list*, ttvevent_list*, long*, long*, long*, long*, long*, int, char, long *);
void stb_setdetail(Setup_Hold_Computation_Detail_INFO *detail, long val, long instab, long clock, long margin, long period, long misc, Extended_Setup_Hold_Computation_Data_Item *eshcdi, short flags, unsigned char phase, long movedperiod);
void stb_set_speed_up(int val);
