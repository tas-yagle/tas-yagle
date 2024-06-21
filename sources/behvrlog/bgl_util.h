char              *bgl_vlogname (char *name);
char              *bgl_getRadical(char *name, int *res);
char              *bgl_vectorize (char *name);
char              *bgl_vectorize_sub (char *name, char *buf);
void              *bgl_vectnam (void *pt_list, int *left, int *right, char **name, char type);
char              *bgl_printabl (char *chaine);
char              *bgl_abl2str (chain_list *expr, char *chaine, int *size_pnt);
char              *bgl_abl2strbool (chain_list *expr, char *chaine, int *size_pnt);
char              *bgl_oper2char (short oper);
char              *bgl_oper2charbool (short oper);
int                bgl_writeprimitives (FILE *fp, char *name, chain_list *ptabl, unsigned int delay);
//struct begen      *bgl_addgen (struct begen *lastgeneric, chain_list *nat_lst, chain_list *nam_lst, char *type, short left, short right, bgl_bcompcontext *context);
struct chain      *bgl_cpyabllst (chain_list *abllst);
bgl_expr           bgl_cpyablstr (bgl_expr ablstr);
void bgl_freenames(void);


char              *bgl_getBitStr(char *bitStr, char *buf);
