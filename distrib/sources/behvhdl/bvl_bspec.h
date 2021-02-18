/* ###--------------------------------------------------------------### */
/*									*/
/* file		: bvl_bspec.h						*/
/* date		: Jun 15 1992						*/
/* author	: TABUSSE L.A.						*/
/* content	: declaration of functions and global variables used by	*/
/*		  bvl_bspec.c						*/
/*									*/
/* ###--------------------------------------------------------------### */

struct begen      *bvl_addgen (struct begen *lastgeneric, chain_list *nat_lst, chain_list *nam_lst, char *type, short left, short right);
struct chain      *bvl_cpyabllst (chain_list *abllst);
bvlexpr            bvl_cpyablstr (bvlexpr ablstr);
bvlexpr            bvl_crtabl (short oper, bvlexpr expr1, bvlexpr expr2, int left, int right);
void               bvl_select (bvlexpr *result, chain_list *ptstr, pNode *ptbdd, bvlexpr ptablstr);
int                bvl_tobin (char *trg, char *src, int left, int right);

