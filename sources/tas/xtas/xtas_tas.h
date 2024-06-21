/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : XTAS Version 5                                              */
/*    Fichier : xtas_tas.h                                                  */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Author(s) : Lionel PROTZENKO                    Date : 01/08/1997     */
/*                                                                          */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*    Modified by :                                   Date : ../../....     */
/*                                                                          */
/****************************************************************************/
/*                                                                          */
/*                                                                          */
/****************************************************************************/


#define XTAS_NETLIST_FILE    0
#define XTAS_TECHNO_FILE     1
#define XTAS_FILEIN_FILE     2
#define XTAS_READAREA_FILE   3
#define XTAS_TMA_OUTPUT_FILE 4
#define XTAS_TAS_OUTPUT_FILE 5

typedef struct xtas_tas_param
{
   char inpath[256];
   char infile[256];
   char tec[256];
   char in[4];
   char ttx;
   char dtx;
   char ext[10];
   char c;
   char e;
   char s;
   char outpath[256];
   char outfile[256];
   char hr;
   char min;
   char pch;
   char f;
   char use_cout;
   char cout[32];
   char use_opc;
   char opc[32];
   char use_slope;
   char slope[32];
   char use_swc;
   char swc[32];
//   char nm;
   char y;
   char q;
   char mg;
   char rc;
   char bk;
   char carac;
   char fcl;
   char xfcl;
   char ls;
   char nl;
   char cl;
   char fl;
   char gns;
   char xg;
   char rpt;
   char la;
   char o;
   char z;
   char b;
   char rcn;
   char p[32];
   char i;
   char nv;
   char stb;
} xtas_tas_param_struct;

typedef struct xtas_tma_param
{
   char ttxin;
   char dtxin;
   char in[4];
   char ext[10];
   char inpath[256];
   char namein[256];
   long out;
   char outpath[256];
   char nameout[256];
   char otock;
   char mod;
   char fin[256];
   char mrg[32];
   char v;
   char i;
   char tu;
   char cu;
   char ra[256];
} xtas_tma_param_struct;

extern XtasWindowStruct *tas_paths;
extern xtas_tas_param_struct *tas_tmppar;
extern xtas_tma_param_struct *tma_tmppar;

extern void XtasCleanStates                 __P(()) ;
extern void XtasNetTypeCallback             __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasOptTypeCallback             __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasOutFilesCallback            __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasSessionTreat                __P(()) ;
extern void XtasSessionOptionsOkCallback    __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasSessionOkCallback           __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasSessionOptionsCallback      __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasNewSessionCallback          __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer 
                                               )) ;
extern void XtasSessionFileOkCallback       __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasTmaInputFileOkCallback     __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasTmaInputFileCallback       __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasSessionFileOutOkCallback       __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;
extern void XtasSessionFileCallback         __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;

extern void XtasSessionFileOutCallback         __P(( 
                                                  Widget ,
                                                  XtPointer ,
                                                  XtPointer
                                               )) ;

extern void XtasFreeAllCells                 __P(( 
                                                  ttvfig_list *
                                               )) ;

