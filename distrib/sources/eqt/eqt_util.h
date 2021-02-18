/*****************************************************************************/
/*     functions                                                             */
/*****************************************************************************/

extern int         eqt_var_involved  (eqt_ctx *ctx);
extern eqt_node   *eqt_addunarynode  (long operator, eqt_node *operand) ;
extern eqt_node   *eqt_addbinarynode (long operator, eqt_node *operand1, eqt_node *operand2);
extern eqt_node *eqt_addternarynode  (long operator, eqt_node *operand1, eqt_node *operand2, eqt_node *operand3);
extern void        eqt_freenode      (eqt_node *node);
extern eqt_node   *eqt_addvarname    (char *name);
extern eqt_node   *eqt_addvalue      (double value);
extern void        eqt_addunit       (eqt_ctx *ctx, char *unit_name, double factor);
extern double      eqt_getunit       (eqt_ctx *ctx, char *unit_name);
extern void        eqt_import_vars   (eqt_ctx *to, eqt_param *from);

extern int         eqt_getindex      (eqt_ctx *ctx, char *name);
extern chain_list *eqt_NodeToAbl     (eqt_node *node);
extern chain_list *eqt_StrToAbl      (eqt_ctx *ctx, char *str);
extern char       *eqt_ReduceSpace   (char *str);
extern int         eqt_IsSpecialCar  (char str, char pos);
extern char       *eqt_ConvertStr    (char *str);
double eqt_execfunc123(eqt_ctx *ctx, int index, void *arg1, void *arg2, void *arg3, int quick);

