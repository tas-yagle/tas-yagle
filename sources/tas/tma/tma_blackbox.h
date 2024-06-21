/****************************************************************************/
/*                                                                          */
/*                 Chaine de CAO & VLSI   Alliance                          */
/*                                                                          */
/*   Produit : TMA Version 1                                                */
/*   Fichier : tma_blackbox.h                                               */
/*                                                                          */
/*   (c) copyright 1997-1998 Laboratoire LIP6 equipe ASIM                   */
/*   Tous droits reserves                                                   */
/*   Support : e-mail support@avertec.com                                   */
/*                                                                          */
/*   Auteur(s) : Gilles AUGUSTINS                                           */
/*                                                                          */
/****************************************************************************/

extern char          *tma_GenerateName      (ttvfig_list *fig, ttvevent_list *ev2, ttvevent_list *ev1, unsigned int type, char minmax);
extern void           tma_DupConnectorList  (ttvfig_list *newfig, ttvfig_list *fig, chain_list *filter);
extern ttvfig_list   *tma_DupTtvFigHeader   (char *newfigname, ttvfig_list *fig);
extern int            tma_IsUp              (long type); 
extern int            tma_IsDown            (long type); 
extern void           tma_CreateAccess      (ttvfig_list *fig, char *nodename, long nodetype, char *rootname, long roottype, timing_model *mdmin, timing_model *mdmax, timing_model *msmin, timing_model *msmax, long dmin, long dmax, long smin, long smax);
extern void           tma_CreatePath        (ttvfig_list *fig, char *nodename, long nodetype, char *rootname, long roottype, timing_model *mdmin, timing_model *mdmax, timing_model *msmin, timing_model *msmax, long dmin, long dmax, long smin, long smax, long type);
extern void           tma_CreateSetup       (ttvfig_list *fig, char *nodename, long nodetype, char *ckname, long cktype, timing_model *cmodel, long setup); 
extern void           tma_CreateHold        (ttvfig_list *fig, char *nodename, long nodetype, char *ckname, long cktype, timing_model *cmodel, long hold); 
extern ttvpath_list  *tma_GetPath           (ttvfig_list *fig, ttvevent_list *node, ttvevent_list *root, long minmax);
extern void           tma_ExpanseBreak      (ttvfig_list *fig, ttvsig_list *sig); 
extern chain_list    *tma_AddMaxList        (chain_list *maxchain, ttvevent_list *event, long dmax, timing_model *mdclock, timing_model *msclock, timing_model *mddata, timing_model *msdata, long cstr, timing_model *mdcstr, timing_model *mscstr, long smax);
extern chain_list    *tma_AddMinList        (chain_list *minchain, ttvevent_list *event, long dmin, timing_model *mdclock, timing_model *msclock, timing_model *mddata, timing_model *msdata, long cstr, timing_model *mdcstr, timing_model *mscstr, long smin);
extern void           tma_FreeList          (chain_list *chain);
extern void           tma_ConvSList         (ttvfig_list *ofig, ttvfig_list *fig, ttvevent_list *node, chain_list *setuplist);
extern void           tma_ConvHList         (ttvfig_list *ofig, ttvfig_list *fig, ttvevent_list *node, chain_list *holdlist);
extern chain_list    *tma_UpdateHList       (chain_list *holdlist, chain_list *maxcklist, long dmin, timing_model *mdmin, timing_model *msmin, long hold, timing_model *hmodel);
extern chain_list    *tma_UpdateSList       (chain_list *setuplist, chain_list *mincklist, long dmax, timing_model *mdmax, timing_model *msmax, long setup, timing_model *smodel);
extern int            tma_IsLatchCell       (ttvfig_list *fig);
extern int            tma_IsFlipFlopCell    (ttvfig_list *fig);
extern void           tma_ConvAList         (ttvfig_list *ofig, ttvfig_list *fig, ttvevent_list *node, chain_list *aminlist, chain_list *amaxlist);
extern int            tma_IsHoldedBreak     (ttvevent_list *event);
extern int            tma_IsSetUpBreak      (ttvevent_list *event);
extern int            tma_IsBreakCmd        (ttvevent_list *event);
extern ttvevent_list *tma_OppositeEvent     (ttvevent_list *event);
extern void           tma_AddLatchSig       (ttvfig_list *fig);
extern void           tma_AddBreakSig       (ttvfig_list *fig); 
extern void           tma_FreeModelList     ();
extern void           tma_AddModelList      (timing_model *model);
extern void           tma_InitModelList     ();
extern int            tma_IsAsynchronous    (ttvsig_list *sig);
extern int            tma_IsClock           (ttvsig_list *sig);
extern int            tma_IsEvLine          (ttvevent_list *node, ttvevent_list *root);
extern int            tma_IsPrLine          (ttvevent_list *node, ttvevent_list *root);
extern chain_list    *tma_AddHList          (chain_list *holdlist, long hold, timing_model *hmodel, long dmin, timing_model *mdmin, timing_model *msmin, ttvpath_list *cklist, ttvevent_list *event); 
extern chain_list    *tma_AddSList          (chain_list *setuplist, long setup, timing_model *smodel, long dmax, timing_model *mdmax, timing_model *msmax, ttvpath_list *cklist, ttvevent_list *event); 
extern chain_list    *tma_AddAMaxList       (chain_list *accesslist, long access, timing_model *mdaccess, timing_model *msaccess, long d, timing_model *md, long s, timing_model *ms, ttvpath_list *cklist, ttvevent_list *event); 
extern chain_list    *tma_AddAMinList       (chain_list *accesslist, long access, timing_model *mdaccess, timing_model *msaccess, long d, timing_model *md, long s, timing_model *ms, ttvpath_list *cklist, ttvevent_list *event); 
extern void           tma_FreePathList      (ttvpath_list *path);
extern void           tma_MarkCmd           (ttvfig_list *fig);
extern void           tma_CreateBreakSH     (ttvfig_list *fig, ttvevent_list *cmd, ttvevent_list *root, timing_model *mdmin, timing_model *mdmax, long dmin, long dmax, timing_model *msmin, timing_model *msmax, chain_list **setuplist, chain_list **holdlist);
extern void           tma_CreateBreakAccess (ttvfig_list *fig, ttvevent_list *node, timing_model *mdmin, timing_model *mdmax, timing_model *msmin, timing_model *msmax, long dmin, long dmax, long smin, long smax, chain_list **aminlist, chain_list **amaxlist, long deltamin, long deltamax);
extern ttvfig_list   *tma_CreateBlackBox    (ttvfig_list *fig, char *suffix);
extern void           tma_AddInsertDelays   (ttvfig_list *blackbox, ttvfig_list *fig);
extern int            tma_IsInList          (chain_list *list, char *name);
extern void           tma_DetectClocksFromBeh (ttvfig_list *fig, befig_list *befig);
extern void           tma_UpdateSetReset    (ttvfig_list *fig, befig_list *befig);
extern void           tma_GetSTBdelta       (stbfig_list *stbfig, ttvevent_list *latch, long *deltamin, long *deltamax) ;
