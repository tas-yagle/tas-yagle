

void bgl_NewOperation(bgl_expr *out, short oper, bgl_expr *expr1, bgl_expr *expr2);
int AddaNewSignalorSignals(bgl_bcompcontext *context, char *name, int left, int right, char dir);
void setsignalflag(bgldecl_list   *sig, chain_list *pile_context);
void bgl_generateBEH(bgl_bcompcontext *context);
int bgl_search_stable(chain_list *ptabl, chain_list **found, int *nb);
void UnaryBitwise(int op, bgl_expr *out, bgl_expr *in);

inline void bgl_CheckType(bgl_expr *me,bgl_bcompcontext *context)
{
  if (me->TYPE != BGL_EXPR && me->TYPE != BGL_IDENT) 
    bgl_error(90,"",context);
}
inline void bgl_CheckSize(bgl_expr *me,bgl_expr *you,bgl_bcompcontext *context)
{
  if (you->WIDTH!=me->WIDTH) 
    bgl_error(38,"",context);
}

int bgl_compare(int op, bgl_expr *out, bgl_expr *a, bgl_expr *b);
void bgl_NewSimpleOperation(bgl_expr *out, short oper, bgl_expr *expr1); // NOT or -1=buff
int countchain(chain_list *doubl);
int CheckSize(chain_list *cl, int skeepfirst);

void AddPrimitiveToBEFIG(chain_list *allprims, chain_list *allcont);
void SimpleAffect(chain_list *out, bgl_expr *in, int delay, bgl_bcompcontext *context);
void DirectIFAffect(chain_list *out, bgl_expr *cond, bgl_expr *sivrai, bgl_expr *sifaux, int delay, bgl_bcompcontext *context);
