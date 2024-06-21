/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_ttv.c                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
/*                                                                          */
/*    Modified by : Mathieu OKUYAMA                   Date : 05/21/1998     */
/*    Modified by : Stephane PICAULT                  Date : 09/10/1998     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/

/*--------------------------------------------------------------------------*/
/*                             INCLUDE FILES                                */
/*--------------------------------------------------------------------------*/
#include "xtas.h"

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasFreePathTabIndex(TabIndex)
chain_list *TabIndex;
{
 chain_list          *head;
 XtasPathIndexTabElt *index;

 if((head = TabIndex) == NULL)
    return; 

 while(head != NULL)
  {
   index = head->DATA;
   mbkfree( index );
   head = head->NEXT;
  }

 freechain( TabIndex );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
chain_list *XtasCreatePathTabIndex( XtasPathSet )
XtasPathListSetStruct *XtasPathSet;
{
 int                   position;
 XtasPathIndexTabElt  *index;
 ttvpath_list         *path;
 chain_list           *head; 

 XtasFreePathTabIndex( XtasPathSet->INDEX_TAB );
 XtasPathSet->INDEX_TAB = NULL ; 

 if(XtasPathSet->CUR_PATH_WIN == NULL)
    return(NULL) ;

 if((path = XtasPathSet->CUR_PATH_WIN->DATA) == NULL)
    return(NULL) ;

 index = (XtasPathIndexTabElt *)mbkalloc(sizeof(XtasPathIndexTabElt)) ;

 index->PATH_SET = XtasPathSet;
 index->PATH     = path;
 head = addchain(NULL,index) ;
 
 if ( path->NEXT == NULL ) return( head );
 path = path->NEXT;

 for(position = 2 ; position <= XtasPathSet->CUR_PATH_WIN->SIZE ; position++)
  {
    index = (XtasPathIndexTabElt *)mbkalloc(sizeof(XtasPathIndexTabElt)) ;
    index->PATH_SET = XtasPathSet;
    index->PATH     = path;
    head            = addchain(head,index) ;
    path            = path->NEXT;
  }
 return( reverse(head) );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
int XtasCleanTtvSigList(XtasPathSet)
XtasPathListSetStruct *XtasPathSet;
{
 chain_list *head;
 XtasChainJmpList *jmplist;
 int n = 0;

 head = XtasPathSet->SIGNAL_LIST_SET->SIG_LIST;

 while(head != NULL)
  {
   XtasPathSet->PARAM->CUR_SIG = head ;

   if(XtasTtvGetPathList(XtasPathSet) == NULL) 
    {
     head = head->NEXT ;
    }
   else
    {
     n = 1 ; 
     break ;
    }
  }

if(n == 0)  
 {
  XtasFreePathListSet(XtasPathSet) ;
 }
else
 {
  XtasPathSet->PARAM->CUR_SIG = XtasPathSet->SIGNAL_LIST_SET->SIG_LIST;
  jmplist = XtasNewChainjmplist( XtasPathSet->SIGNAL_LIST_SET->SIG_LIST,
                                 XtasPathSet->PARAM->ROOT_SIG_EDGE->ITEMS );
  XtasPathSet->SIGNAL_LIST_SET->SIG_LIST_WIN = jmplist;
  XtasPathSet->SIGNAL_LIST_SET->CUR_SIG_WIN = jmplist;
 } 

 return(n); 
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasSearchSigParamStruct *XtasNewSearchSigParam()
{
 XtasSearchSigParamStruct *Signal;

 Signal = (XtasSearchSigParamStruct *)
           mbkalloc(sizeof(XtasSearchSigParamStruct)) ;

 Signal->TYPE      = TTV_SIG_C | TTV_SIG_L | TTV_SIG_R | TTV_SIG_Q | TTV_SIG_B ;
 Signal->MASK      = NULL ;
 Signal->TOP_FIG   = XtasMainParam->ttvfig ;
 Signal->LOCAL_FIG = XtasMainParam->ttvfig ;
 Signal->ITEMS     = 10;

 return(Signal);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasSearchSigParamStruct *XtasCopySearchSigParam(param)
XtasSearchSigParamStruct *param;
{
 XtasSearchSigParamStruct *Signal;
 chain_list *chain ;

 Signal = (XtasSearchSigParamStruct *)
          mbkalloc(sizeof(XtasSearchSigParamStruct)) ;

 Signal->TYPE      = param->TYPE ;
 Signal->MASK      = NULL ;
 for(chain = param->MASK ; chain != NULL ; chain = chain->NEXT)
    Signal->MASK = addchain(Signal->MASK,chain->DATA) ;
 Signal->TOP_FIG   = param->TOP_FIG ;
 Signal->LOCAL_FIG = param->LOCAL_FIG ;
 Signal->ITEMS     = param->ITEMS ;

 return(Signal) ;
}
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasSigListSetStruct *XtasNewSigListSet(signal)
XtasSearchSigParamStruct *signal ;
{
 XtasSigListSetStruct *set;

 set = (XtasSigListSetStruct *)mbkalloc(sizeof(XtasSigListSetStruct)) ;

 if(signal == NULL)
 {
  if(!(set->SIGNAL = XtasNewSearchSigParam())) 
   {
     mbkfree(set) ;
     return(NULL) ;
   }
 }
else
 {
  if(!(set->SIGNAL = XtasCopySearchSigParam(signal)))
   {
    mbkfree( set ) ;
    return( NULL ) ;
   }
 }

 set->TOP_LEVEL      = NULL ;
 set->MASK           = NULL ;
 set->ITEMS          = NULL ;
 set->SIGLIST_SCROLL = NULL ;
 set->INFO_PAGE      = NULL ;
 set->INFO_HELP      = NULL ;
 set->INFO_USER      = NULL ;
 set->SIG_LIST       = NULL ;
 set->SIG_LIST_WIN   = NULL ;
 set->CUR_SIG_WIN    = NULL ;

 return(set);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasFreeSearchSigParam(Signal)
XtasSearchSigParamStruct *Signal ;
{
 if (Signal != NULL)
  {
   freechain(Signal->MASK) ;
   mbkfree( Signal ) ;
  }
} 

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasFreeSigListSet(set)
XtasSigListSetStruct *set;
{
 if(set != NULL)
  {
   if((set->SIGNAL->TYPE & TTV_SIG_S) == TTV_SIG_S)
     ttv_freenamelist(set->SIG_LIST) ;
   else
     freechain (set->SIG_LIST );
   XtasFreeSearchSigParam(set->SIGNAL);
   XtasFreeChainjmplist(set->SIG_LIST_WIN);
   mbkfree(set);
  } 
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasSearchPathParamStruct *XtasNewSearchPathParam(sig_par)
XtasSearchSigParamStruct *sig_par;
{
 XtasSearchPathParamStruct *param;

 param = (XtasSearchPathParamStruct *)
         mbkalloc(sizeof(XtasSearchPathParamStruct)) ;

 if(sig_par != NULL) 
  {
   if(!(param->ROOT_SIG_EDGE = XtasCopySearchSigParam(sig_par)))
      { 
       mbkfree(param) ;
       return( NULL );
      }
  }
 else
  {
   if(!(param->ROOT_SIG_EDGE = XtasNewSearchSigParam())) 
     { 
       mbkfree(param); 
       return(NULL); 
     }
  }

 param->NODE_MASK     = NULL;      
 param->CK_MASK       = NULL;      
 param->CUR_SIG       = NULL;
 param->LOOK_PATH     = NULL;
 param->DELAY_MIN     = TTV_DELAY_MIN;
 param->DELAY_MAX     = TTV_DELAY_MAX;
 param->SCAN_TYPE     = TTV_FIND_MAX;
 param->REQUEST_TYPE  = XTAS_NOTYPE ;
 param->CRITIC        = 'N' ;
 param->PATH_ITEM     = 10 ;
 param->MASK          = NULL ;

 return( param );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasSearchPathParamStruct *XtasCopySearchPathParam(param)
XtasSearchPathParamStruct *param; 
{
 XtasSearchPathParamStruct *param_tmp;
 chain_list *chain ;

 if(!(param_tmp = XtasNewSearchPathParam(param->ROOT_SIG_EDGE)))
    return( NULL );

 param_tmp->NODE_MASK    = NULL ;
 for(chain = param->NODE_MASK ; chain != NULL ; chain = chain->NEXT)
    param_tmp->NODE_MASK = addchain(param_tmp->NODE_MASK,chain->DATA) ;
 param_tmp->CK_MASK    = NULL ;
 for(chain = param->CK_MASK ; chain != NULL ; chain = chain->NEXT)
    param_tmp->CK_MASK = addchain(param_tmp->CK_MASK,chain->DATA) ;
 param_tmp->CUR_SIG       = param->CUR_SIG ;
 param_tmp->LOOK_PATH     = param->LOOK_PATH ;
 param_tmp->DELAY_MIN     = param->DELAY_MIN ;
 param_tmp->DELAY_MAX     = param->DELAY_MAX ;
 param_tmp->SCAN_TYPE     = param->SCAN_TYPE ;
 param_tmp->REQUEST_TYPE  = param->REQUEST_TYPE ;
 param_tmp->CRITIC        = param->CRITIC ;
 param_tmp->PATH_ITEM     = param->PATH_ITEM ;
 for(chain = param->MASK ; chain != NULL ; chain = chain->NEXT)
    param_tmp->MASK = addchain(param_tmp->MASK,chain->DATA) ;

 return( param_tmp );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasFreeSearchPathParam(param)
XtasSearchPathParamStruct *param;
{
 if(param != NULL)
  {
   freechain(param->NODE_MASK) ;
   XtasFreeSearchSigParam(param->ROOT_SIG_EDGE) ;
   mbkfree( param );
  }
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasFreePathListSet(set)
XtasPathListSetStruct *set ;
{
 if(set != NULL)
  {
   XtasFreeSigListSet(set->SIGNAL_LIST_SET) ;
   XtasFreeChainjmplist(set->PATH_LIST_WIN) ;
   XtasFreeSearchPathParam(set->PARAM) ;
   XtasFreePathTabIndex(set->INDEX_TAB) ;
   ttv_freepathlist(set->PATH_LIST) ;
   freechain(set->PARA_SESSION) ;
   freechain(set->DETAIL_SESSION) ;
   mbkfree(set) ;
  }
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasPathListSetStruct *XtasNewPathListSet(param)
XtasSearchPathParamStruct *param;
{
 XtasPathListSetStruct *set;

 set = (XtasPathListSetStruct *)mbkalloc( sizeof( XtasPathListSetStruct)) ;

 if(!(set->SIGNAL_LIST_SET = XtasNewSigListSet(param->ROOT_SIG_EDGE))) 
  { 
   XtasFreePathListSet(set); 
   return(NULL) ; 
  }

 if(!(set->PARAM = XtasCopySearchPathParam(param))) 
  { 
   XtasFreePathListSet(set); 
   return(NULL) ; 
  }

 set->TOP_LEVEL       = NULL ;
 set->INFO_HELP       = NULL ;
 set->INFO_USER       = NULL ;
 set->PATHLIST_SCROLL = NULL ;
 set->PATHLIST_ROW    = NULL ;
 set->INFO_PAGE       = NULL ;
 set->INFO_CUR_SIG    = NULL ;
 set->PATH_LIST       = NULL ;
 set->PATH_LIST_WIN   = NULL ;
 set->CUR_PATH_WIN    = NULL ;
 set->INDEX_TAB       = NULL ;
 set->PARENT          = NULL ;
 set->DETAIL_SESSION  = NULL ;
 set->PARA_SESSION    = NULL ;
   
 return(set) ;
}
   
/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasDetailPathSetStruct *XtasNewDetailPathSet(detail_list,items)
chain_list   *detail_list;
int           items;
{
 XtasDetailPathSetStruct   *head;
 XtasChainJmpList          *jmplist;

 head = (XtasDetailPathSetStruct *)mbkalloc(sizeof(XtasDetailPathSetStruct)) ;

 jmplist = XtasNewChainjmplist(detail_list,items) ;

 head->TOP_LEVEL     = NULL ;
 head->SCROLL_DETAIL = NULL ;
 head->ROW_DETAIL    = NULL ;
 head->INFO_USER     = NULL ;
 head->INFO_HELP     = NULL ;
 head->INFO_PAGE     = NULL ;
 head->ROOT_NAME     = NULL ;
 head->NODE_NAME     = NULL ;
 head->TOTAL_DELAY   = NULL ;
 head->DETAIL_LIST   = jmplist ;
 head->CUR_DETAIL    = jmplist ;
 head->PARENT        = NULL ;

 return(head) ;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
void XtasFreeDetailPathSetStruct(set)
XtasDetailPathSetStruct *set ;
{
 chain_list *chain ;

 if(set != NULL)
  {
   if(set->DETAIL_LIST != NULL)
    {
     if(set->DETAIL_LIST->DATA != NULL)
      {
       chain = set->DETAIL_LIST->DATA ;
       if(chain->NEXT != NULL)
        {
         ttv_freecriticpara(chain) ;
        }
       else
        {
         ttv_freecriticlist((ttvcritic_list *)chain->DATA) ;
         freechain(chain) ;
        }
      }
    }
   XtasFreeChainjmplist(set->DETAIL_LIST) ;
   mbkfree(set) ;
  }
}

/****************************************************************************/
/*                                                                          */
/*                                                                          */
/*                                                                          */ 
/*                                                                          */ 
/*                                                                          */ 
/*                                                                          */ 
/*******************************i********************************************/
void XtasFreeChainjmplist(jmplist)
 XtasChainJmpList *jmplist;
{
 XtasChainJmpList *ptl1, *ptl2;

 if(jmplist != NULL) 
   {
    if(jmplist->NEXT == NULL )
      {
       mbkfree( jmplist );
       return;
      } 
    else
      {
       for(ptl1  = jmplist ; ptl1 != NULL ; ptl1 = ptl2)
         {
          ptl2 = ptl1->NEXT ;
          mbkfree( ptl1 ) ; 
         }
      }
   }      
}

/****************************************************************************/
/*  									    */ 
/*  									    */ 
/*  									    */ 
/*  									    */ 
/****************************************************************************/
XtasChainJmpList *XtasNewPathjmplist(liste,winsize)
ttvpath_list *liste;
int   winsize;
{
 XtasChainJmpList   *jmplist;
 XtasChainJmpList   *headlist;
 int                        index_g;
 int                        index_l;

 if(liste == NULL) 
   { 
    return NULL ;
   }

 jmplist = (XtasChainJmpList * )mbkalloc(sizeof(XtasChainJmpList)) ;

 jmplist->NEXT = NULL;
 jmplist->PREV = NULL;
 jmplist->DATA = liste;
 jmplist->INDEX = 1;
 headlist = jmplist;

 index_l = 0 ;

 for (index_g = 2 ; liste != NULL ; liste = liste->NEXT)
   {
    index_l++;
    if(index_l == (winsize+1))
      {
       jmplist->NEXT = (XtasChainJmpList *)mbkalloc(sizeof(XtasChainJmpList)) ;
       index_l = 1;
	
       jmplist->NEXT->PREV = jmplist;
       jmplist->SIZE       = winsize;
       jmplist             = jmplist->NEXT;
       jmplist->NEXT       = NULL;
       jmplist->INDEX      = index_g;
       jmplist->DATA       = liste;
       index_g++ ;
      }    
   }

 jmplist->SIZE = index_l;
 return(headlist);
}

/****************************************************************************/
/*  									    */ 
/*  									    */ 
/*  									    */ 
/*  									    */ 
/****************************************************************************/
XtasChainJmpList *XtasNewChainjmplist(liste, winsize)
chain_list *liste;
int   winsize;
{
 XtasChainJmpList  *jmplist;
 XtasChainJmpList  *headlist;
 int                index_g, index_l;

 if(liste == NULL)return(NULL) ;
 jmplist = (XtasChainJmpList * )mbkalloc(sizeof(XtasChainJmpList)) ;

 jmplist->NEXT = NULL;
 jmplist->PREV = NULL;
 jmplist->DATA = liste;
 jmplist->INDEX = 1;
 headlist = jmplist;

 index_l = 0;

 for(index_g = 2 ; liste != NULL ; liste = liste->NEXT )
  {
   index_l++;
   if(index_l == (winsize+1))
      {
       jmplist->NEXT = (XtasChainJmpList * )mbkalloc(sizeof(XtasChainJmpList)) ;
       index_l = 1;
	
       jmplist->NEXT->PREV = jmplist;
       jmplist->SIZE       = winsize;
       jmplist             = jmplist->NEXT;
       jmplist->NEXT       = NULL;
       jmplist->INDEX      = index_g;
       jmplist->DATA       = liste;
       index_g++;
      }    
  }

 jmplist->SIZE = index_l;
 return (headlist);
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasChainJmpList *XtasTtvGetSigList(param)
XtasSigListSetStruct *param;
{
 XtasChainJmpList *winjmplist;
 chain_list *head = NULL;

 XtasPasqua();

 if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
   if((param->SIGNAL->TYPE & TTV_SIG_S) == TTV_SIG_S)
    {
     head = ttv_getsignamelist(param->SIGNAL->TOP_FIG,
                               param->SIGNAL->LOCAL_FIG,
                               param->SIGNAL->MASK);
     if(head != NULL)
       if(param->SIG_LIST != NULL)
         ttv_freenamelist(param->SIG_LIST) ;
    }
   else
    {
     head = ttv_getsigbytype( param->SIGNAL->TOP_FIG,
			      param->SIGNAL->LOCAL_FIG, 
			      param->SIGNAL->TYPE, 
			      param->SIGNAL->MASK);
     if(head != NULL)
       if(param->SIG_LIST != NULL)
         freechain(param->SIG_LIST) ;
    }
  }
 else
  return(NULL) ;

 XtasGetWarningMess() ;

 XtasFirePasqua();
 
 if ( head != NULL )
  {
   param->SIG_LIST = head;
   XtasFreeChainjmplist( param->SIG_LIST_WIN );
  }
 else
   return( NULL );

 winjmplist = XtasNewChainjmplist(head,param->SIGNAL->ITEMS);
 param->SIG_LIST_WIN = winjmplist;
 param->CUR_SIG_WIN = winjmplist;
 return( winjmplist );
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasChainJmpList *XtasTtvGetParaList(pathset)
XtasPathListSetStruct *pathset;
{
 XtasChainJmpList *pathjmplist;
 ttvpath_list *head;
 XtasSearchPathParamStruct *param;
 long   type;

 param = pathset->PARAM;

 XtasPasqua();

 if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
   type = param->SCAN_TYPE | (TTV_FIND_ACCESS & param->LOOK_PATH->TYPE);       
   head = ttv_getpara(param->ROOT_SIG_EDGE->TOP_FIG,
                      param->ROOT_SIG_EDGE->LOCAL_FIG,
                      param->LOOK_PATH->ROOT,
                      param->LOOK_PATH->NODE,
                      param->DELAY_MAX, 
                      param->DELAY_MIN,
                      type,
                      param->MASK,10000 * param->PATH_ITEM) ;
  }
 else
  return(NULL) ;

 XtasGetWarningMess() ;

 XtasFirePasqua();

 if(head == NULL) 
   {
    return(NULL);
   }

 XtasFreeChainjmplist(pathset->PATH_LIST_WIN);
 if(pathset->PATH_LIST != NULL)
   ttv_freepathlist(pathset->PATH_LIST) ;

 pathset->PATH_LIST = head;

 pathjmplist = XtasNewPathjmplist(head,param->PATH_ITEM) ;
 pathset->PATH_LIST_WIN = pathjmplist ;
 pathset->CUR_PATH_WIN = pathjmplist ;

 return(pathjmplist) ;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasDetailPathSetStruct *XtasTtvGetDelayList(pathset)
XtasPathListSetStruct *pathset;
{
 XtasSearchPathParamStruct *param;
 ttvcritic_list *detail_list ;
 chain_list     *head = NULL;
 XtasDetailPathSetStruct *detail_set ;
 int items = 1 ;
 long mask1,mask2 ;

 param = pathset->PARAM;

 if(param->CRITIC != 'D')
  {
   if ( ( param->SCAN_TYPE & TTV_FIND_DUAL ) == TTV_FIND_DUAL )
     {
       mask1= param->LOOK_PATH->TYPE  | TTV_FIND_DUAL | TTV_FIND_LINE;
       mask2= (param->LOOK_PATH->TYPE | TTV_FIND_DUAL | TTV_FIND_LINE)
         & ~(TTV_FIND_PATH);
     }

   else
     {
       mask1 = (param->LOOK_PATH->TYPE | TTV_FIND_LINE) & ~(TTV_FIND_DUAL) ;
       mask2= (param->LOOK_PATH->TYPE | TTV_FIND_LINE)
         & ~(TTV_FIND_PATH) & ~(TTV_FIND_DUAL) ;
     }
  }

 XtasPasqua();

 if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
   if ( param->CRITIC == 'D' )
     {
      head = ttv_getdelay(param->ROOT_SIG_EDGE->TOP_FIG,
                          param->ROOT_SIG_EDGE->LOCAL_FIG,
                          param->ROOT_SIG_EDGE->MASK,
                          param->NODE_MASK,
                          param->DELAY_MAX,
                          param->DELAY_MIN,
                          param->SCAN_TYPE) ;
      if (head!=NULL)
          detail_list = head->DATA;
      else detail_list = NULL;
      items = param->PATH_ITEM ;
     }
   else if ((param->LOOK_PATH->TYPE & TTV_FIND_ALL)!=0 || (param->CRITIC == 'P') || ((param->REQUEST_TYPE & XTAS_ALL) == XTAS_ALL))
     {
       head  = ttv_getcriticpara(param->ROOT_SIG_EDGE->TOP_FIG,
                                 param->LOOK_PATH->FIG,
                                 param->LOOK_PATH->ROOT,
                                 param->LOOK_PATH->NODE,
                                 param->LOOK_PATH->DELAY,
                                 mask1, param->MASK, param->LOOK_PATH->TTV_MORE_SEARCH_OPTIONS) ;
       if (head!=NULL)
          detail_list = head->DATA;
       else detail_list = NULL;
     }
   else
     {
       detail_list = ttv_getcritic( param->ROOT_SIG_EDGE->TOP_FIG,
                                    param->LOOK_PATH->FIG,
                                    param->LOOK_PATH->ROOT,
                                    param->LOOK_PATH->NODE,
                                    param->LOOK_PATH->LATCH,
                                    param->LOOK_PATH->CMDLATCH,
                                    mask2
                                    ) ;
       if(detail_list != NULL) 
         head = addchain( head, detail_list );
     }
    }
 else
  return(NULL) ;

 XtasGetWarningMess() ; 

 XtasFirePasqua();

 if (detail_list == NULL) return NULL;

 detail_set = XtasNewDetailPathSet(head,items);

 return( detail_set ) ;
}

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*                                                                          */
/*                                                                          */
/*--------------------------------------------------------------------------*/
XtasChainJmpList *XtasTtvGetPathList(pathset)
XtasPathListSetStruct *pathset;
{
 XtasChainJmpList *pathjmplist;
 ttvpath_list *head;
 XtasSearchPathParamStruct *param;


 param = pathset->PARAM;

 XtasPasqua();

 if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
   if ((param->REQUEST_TYPE & XTAS_SBS) == XTAS_SBS )
    {
      if ((param->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS) {
          if ((param->REQUEST_TYPE & XTAS_ALL) == XTAS_ALL)
            head = ttv_getpathsig(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                        (ttvsig_list *)param->CUR_SIG->DATA,
                         param->NODE_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
     	                 param->SCAN_TYPE);
          else 
            head = ttv_getpath(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                        (ttvsig_list *)param->CUR_SIG->DATA,
                         param->NODE_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
     	                 param->SCAN_TYPE);              
      }
      else {  /* access */
          if ((param->REQUEST_TYPE & XTAS_ALL) == XTAS_ALL)
            head = ttv_getsigaccess(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                        (ttvsig_list *)param->CUR_SIG->DATA,
                         param->NODE_MASK,
                         param->CK_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
     	                 param->SCAN_TYPE);
          else
            head = ttv_getaccess(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                        (ttvsig_list *)param->CUR_SIG->DATA,
                         param->NODE_MASK,
                         param->CK_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
     	                 param->SCAN_TYPE);
      }
    }
    else if ((param->REQUEST_TYPE & XTAS_ALL) == XTAS_ALL )
    {
      if ((param->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS) {
          if((param->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            head = ttv_getallpath(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                         param->ROOT_SIG_EDGE->MASK, 
                         param->NODE_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
                         (long)param->PATH_ITEM,
     	                 param->SCAN_TYPE);
          else
            head = ttv_getallpath(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                         param->NODE_MASK,
                         param->ROOT_SIG_EDGE->MASK, 
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
                         (long)param->PATH_ITEM,
     	                 param->SCAN_TYPE);
      }
      else {  /* access */
          if((param->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
            head = ttv_getallaccess(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                         param->ROOT_SIG_EDGE->MASK, 
                         param->NODE_MASK,
                         param->CK_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
                         (long)param->PATH_ITEM,
     	                 param->SCAN_TYPE);
          else
            head = ttv_getallaccess(param->ROOT_SIG_EDGE->TOP_FIG,
                         param->ROOT_SIG_EDGE->LOCAL_FIG,
                         param->NODE_MASK,
                         param->ROOT_SIG_EDGE->MASK, 
                         param->CK_MASK,
                         param->DELAY_MAX,
                         param->DELAY_MIN, 
                         (long)param->PATH_ITEM,
     	                 param->SCAN_TYPE);              
      }
    }
   else if ((param->REQUEST_TYPE & XTAS_CRITIC) == XTAS_CRITIC )
      {  
        if ((param->REQUEST_TYPE & XTAS_PATHS) == XTAS_PATHS) {
            if((param->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
             head = ttv_getcriticpath(param->ROOT_SIG_EDGE->TOP_FIG,
                                   param->ROOT_SIG_EDGE->LOCAL_FIG,
                                   param->ROOT_SIG_EDGE->MASK, 
                                   param->NODE_MASK, 
                                   param->DELAY_MAX,
                                   param->DELAY_MIN,
                                   (long)param->PATH_ITEM,
                                   param->SCAN_TYPE
                                  );
        
            else
             head = ttv_getcriticpath(param->ROOT_SIG_EDGE->TOP_FIG,
                                   param->ROOT_SIG_EDGE->LOCAL_FIG,
                                   param->NODE_MASK, 
                                   param->ROOT_SIG_EDGE->MASK, 
                                   param->DELAY_MAX,
                                   param->DELAY_MIN,
                                   (long)param->PATH_ITEM,
                                   param->SCAN_TYPE
                                  );
        }
        else { /*access */
             if((param->SCAN_TYPE & TTV_FIND_DUAL) == TTV_FIND_DUAL)
                head = ttv_getcriticaccess(param->ROOT_SIG_EDGE->TOP_FIG,
                                   param->ROOT_SIG_EDGE->LOCAL_FIG,
                                   param->ROOT_SIG_EDGE->MASK, 
                                   param->NODE_MASK, 
                                   param->CK_MASK, 
                                   param->DELAY_MAX,
                                   param->DELAY_MIN,
                                   (long)param->PATH_ITEM,
                                   param->SCAN_TYPE );
             else
                head = ttv_getcriticaccess(param->ROOT_SIG_EDGE->TOP_FIG,
                                   param->ROOT_SIG_EDGE->LOCAL_FIG,
                                   param->NODE_MASK, 
                                   param->ROOT_SIG_EDGE->MASK, 
                                   param->CK_MASK, 
                                   param->DELAY_MAX,
                                   param->DELAY_MIN,
                                   (long)param->PATH_ITEM,
                                   param->SCAN_TYPE );            
        }
      }
  }
 else
  return(NULL) ;
 
 XtasGetWarningMess() ;

 XtasFirePasqua();

 if (head == NULL) 
   {
    return( NULL );
   }

 XtasFreeChainjmplist(pathset->PATH_LIST_WIN) ;
 if (pathset->PATH_LIST != NULL)
     ttv_freepathlist(pathset->PATH_LIST) ;
 pathset->PATH_LIST = head ;

 if (XTAS_DISPLAY_NEW)
     pathjmplist = XtasNewPathjmplist(head,param->PATH_ITEM) ;
 else
     pathjmplist = XtasNewPathjmplist(head,50) ;
 pathset->PATH_LIST_WIN = pathjmplist ;
 pathset->CUR_PATH_WIN = pathjmplist ;

 return( pathjmplist ) ;
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
ttvfig_list *XtasLoadTtvfig(path_name)
char *path_name;
{
 ttvfig_list *ttvfig ;
 chain_list *chain ;
 chain_list *chainfig ;
 char *file_name;
 char *ext_name;
 char *work_lib;
 cnsfig_list    *ptcnsfig;
 inffig_list    *ifl=NULL;

 if ((file_name = strrchr(path_name,'/')) == NULL)
   file_name = path_name;
 else
  {
   *file_name = '\0';
   file_name++;
   if (strlen(file_name)==0)
       return NULL;
   work_lib = (char *)mbkalloc(strlen(path_name)+1);
   strcpy (work_lib, path_name);
  }  

 /* recherche de l'extension complete (dtx/ttx + filtre) */
// if((ext_name = strchr(file_name, '.')) != NULL)
//    *ext_name = '\0';
  if (FILTER_SFX) {
      if ((ext_name = strstr(file_name, FILTER_SFX)) != NULL)
      if (*(ext_name+strlen (FILTER_SFX)) == '\0')
          *ext_name = '\0';
  }

 if((ext_name = strrchr(file_name, '.')) != NULL)
    *ext_name = '\0';
 
 WORK_LIB = work_lib ;


 XtasPasqua();
 if(sigsetjmp( XtasMyEnv , 1 ) == 0)
  {
     if(XtasMainParam->ttvfig != NULL )
       {
        if (getloadedlofig(XtasMainParam->ttvfig->INFO->FIGNAME)) {
            dellofig (XtasMainParam->ttvfig->INFO->FIGNAME);
        }
        for (ptcnsfig = CNS_HEADCNSFIG; ptcnsfig != NULL; ptcnsfig = ptcnsfig->NEXT) {
            if (strcmp(ptcnsfig->NAME, XtasMainParam->ttvfig->INFO->FIGNAME) == 0) break;
        }
        if (ptcnsfig) {
            CNS_HEADCNSFIG = delcnsfig (CNS_HEADCNSFIG, ptcnsfig);
            ptcnsfig = NULL;
        }
        XtasFreeAllCells (XtasMainParam->ttvfig);
        if(XtasMainParam->stbfig != NULL){
          stb_ctk_clean(XtasMainParam->stbfig);
          stb_delstbfig(XtasMainParam->stbfig) ;
          XtasMainParam->stbfig= NULL;
          XTAS_CTX_LOADED = XTAS_NOT_LOADED;
        }
        chainfig = ttv_getttvfiglist(XtasMainParam->ttvfig) ;
        for(chain = chainfig ; chain != NULL ; chain = chain->NEXT)
          if((ttvfig_list *)chain->DATA == ttvfig)
              break ;
        if(chain == NULL)
          ttv_freeallttvfig( XtasMainParam->ttvfig );
        freechain(chainfig) ;
       }
     ttvfig =  ttv_getttvfig(file_name, TTV_FILE_INF) ;
     XtasMainParam->ttvfig = ttvfig ;
     if (XtasMainParam->ttvfig)
        ifl = getloadedinffig (XtasMainParam->ttvfig->INFO->FIGNAME);
     if (ifl)   tas_update_mcctemp (ifl);
  }
 else
  return NULL;

 XtasGetWarningMess() ;

 XtasFirePasqua();
   
 return(ttvfig) ;
}
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void XtasLoadCtxOkCallback( widget, client_data, call_data)
Widget      widget;
XtPointer   client_data;
XtPointer   call_data;
{    
    XtUnmanageChild(widget);
    XtDestroyWidget (widget);
    XalSetCursor( XtasTopLevel, WAIT ); 
    XtasSetLabelString(XtasDeskMessageField,
                     "Loading CTX File...");
    XalForceUpdate( XtasTopLevel );

    XtasPasqua();

    if(sigsetjmp( XtasMyEnv , 1 ) == 0) {
        if(ttv_ctxparse((ttvfig_list *)client_data) == 1) 
            XTAS_CTX_LOADED = XTAS_FROM_FILE;
        else 
            XTAS_CTX_LOADED = XTAS_NOT_LOADED;
    }
    else 
        return ;

    XtasGetWarningMess() ;
    XtasFirePasqua();
 
    XalSetCursor( XtasTopLevel, NORMAL ); 
    if((XTAS_CTX_LOADED & XTAS_FROM_FILE) == XTAS_FROM_FILE) {
        XtasSetLabelString(XtasDeskMessageField,
                         "CTX File Successfully Loaded");
        XalForceUpdate( XtasTopLevel );
    }
    else {
        XtasSetLabelString(XtasDeskMessageField,
                         "CTX File Load Failed");
        XalForceUpdate( XtasTopLevel );
        
    }
        
//    XalLeaveLimitedLoop();
}

/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*                                                                         */
/*-------------------------------------------------------------------------*/
void XtasLoadCtxCancelCallback (widget, client_data, call_data)
Widget  widget;
XtPointer   client_data;
XtPointer   call_data;
{
    XtUnmanageChild (widget);
    XtDestroyWidget (widget);
}

void XtasLoadCtx(ttvfig)
ttvfig_list *ttvfig;
{
    Widget   XtasLoadCtxQuestion = NULL;
    int             n;
    Arg             args[10];
    XmString        motif_string, textOk, textCancel;
    char            question[] = "Do you want to load the netlist\nwith crosstalk analysis results?";
    Atom            WM_DELETE_WINDOW;

//    if(!XtasLoadCtxQuestion) {
        motif_string = XmStringCreateLtoR(question, XmSTRING_DEFAULT_CHARSET);
        textOk = XmStringCreateSimple("   Yes   ");
        textCancel = XmStringCreateSimple("   No   ");
        
        n = 0;
        XtSetArg( args[n], XmNmessageString,        motif_string            ); n++;
        XtSetArg( args[n], XmNtitle,                XTAS_NAME": Question"   ); n++;
        XtSetArg( args[n], XmNokLabelString,        textOk                  ); n++;
        XtSetArg( args[n], XmNcancelLabelString,    textCancel              ); n++;
        XtSetArg( args[n], XmNdialogStyle,          XmDIALOG_APPLICATION_MODAL); n++;
        XtasLoadCtxQuestion = XmCreateQuestionDialog(XtasMainWindow, "XtasLoadCtxQuestionWidget", args, n);
        
        XtUnmanageChild(XmMessageBoxGetChild(XtasLoadCtxQuestion, XmDIALOG_HELP_BUTTON));
        XmStringFree(motif_string);
        XmStringFree(textOk);
        XmStringFree(textCancel);

        XtAddCallback(XtasLoadCtxQuestion, XmNokCallback, XtasLoadCtxOkCallback, (XtPointer)ttvfig);
        XtAddCallback(XtasLoadCtxQuestion, XmNcancelCallback, XtasLoadCtxCancelCallback, (XtPointer)XtasLoadCtxQuestion);
        WM_DELETE_WINDOW = XmInternAtom(XtDisplay(XtasMainWindow), "WM_DELETE_WINDOW", False);
        XmAddWMProtocolCallback(XtParent(XtasLoadCtxQuestion), WM_DELETE_WINDOW, XtasLoadCtxCancelCallback, (XtPointer )XtasLoadCtxQuestion);
//    }

    XtManageChild(XtasLoadCtxQuestion);
//    XalLimitedLoop(XtasLoadCtxQuestion);

}
