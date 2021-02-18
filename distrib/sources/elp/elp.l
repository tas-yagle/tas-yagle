%{
#include <stdio.h>
#include <stdlib.h>
#include "y.tab.h"
#include ELP_H
#define ECHO /* fprintf(stderr,"\ntoken %s\n",yytext) */
#define YY_NO_UNPUT

extern double atof() ;
extern int ELPLINE  ;
%}

integer               [0-9]+
dble                  (("."{integer}+)|({integer}+(("."{integer}+))?))([e|E][+|-]?{integer}+)?
number                {integer}|{dble}
best                  [bB][eE][sS][tT]
worst                 [wW][oO][rR][sS][tT]
typical               [tT][yY][pP][iI][cC][aA][lL]

%%
^#.*$                       {ECHO ;                   }
[ \t;]+                     {ECHO ;                   }
\n                          {ECHO ; ELPLINE++        ;}
[=:-]                       {ECHO ; return *yytext   ;} 
[\[\]]                      {ECHO ; return *yytext   ;} 
[t,T]echnologie             {ECHO ; return T_TECNAME ;}
[v,V]ersion                 {ECHO ; return T_TECVER  ;}
ESIM                        {ECHO ; return T_ESIM    ;}
MODEL                       {ECHO ; return T_MODEL   ;}
TECHNO                      {ECHO ; return T_TECHNO  ;}
LEVEL                       {ECHO ; return T_LEVEL   ;}
ELEVEL                      {ECHO ; return T_ELEVEL  ;}
ACM                         {ECHO ; return T_ACM     ;}
TEMP                        {ECHO ; return T_TEMP    ;}
SLOPE                       {ECHO ; return T_SLOPE   ;}
VDDmax                      {ECHO ; return T_VDDmax  ;}
VDDBest                     {ECHO ; return T_VDDBEST ;}
VDDWorst                    {ECHO ; return T_VDDWORST;}
BEGIN                       {ECHO ; return T_BEGIN   ;}
END                         {ECHO ; return T_END     ;}
CASE                        {ECHO ; return T_CASE    ;}
{best}                      {ECHO ; return T_BEST    ;}
{worst}                     {ECHO ; return T_WORST   ;}
{typical}                   {ECHO ; return T_TYPICAL ;}
INDEX                       {ECHO ; return T_INDEX   ;}
TYPE                        {ECHO ; return T_TYPE    ;}
LMIN                        {ECHO ; return T_LMIN    ;}
LMAX                        {ECHO ; return T_LMAX    ;}
WMIN                        {ECHO ; return T_WMIN    ;}
WMAX                        {ECHO ; return T_WMAX    ;}
LMLT                        {ECHO ; return T_LMLT    ;}
WMLT                        {ECHO ; return T_WMLT    ;}
DL                          {ECHO ; return T_DL      ;}
DW                          {ECHO ; return T_DW      ;}
DLC                         {ECHO ; return T_DLC     ;}
DWC                         {ECHO ; return T_DWC     ;}
DWCJ                        {ECHO ; return T_DWCJ    ;}
VT                          {ECHO ; return T_VT      ;}
VT0                         {ECHO ; return T_VT0     ;}
VBULK                       {ECHO ; return T_VBULK   ;}
MULU0                       {ECHO ; return T_MULU0   ;}
DELVT0                      {ECHO ; return T_DELVT0  ;}
SA                          {ECHO ; return T_SA      ;}
SB                          {ECHO ; return T_SB      ;}
SD                          {ECHO ; return T_SD      ;}
SC                          {ECHO ; return T_SC      ;}
SCA                         {ECHO ; return T_SCA     ;}
SCB                         {ECHO ; return T_SCB     ;}
SCC                         {ECHO ; return T_SCC     ;}
NF                          {ECHO ; return T_NF      ;}
NRS                         {ECHO ; return T_NRS     ;}
NRD                         {ECHO ; return T_NRD     ;}
KT                          {ECHO ; return T_KT      ;}
A                           {ECHO ; return T_A       ;}
M                           {ECHO ; return T_M       ;}
B                           {ECHO ; return T_B       ;}
RT                          {ECHO ; return T_RT      ;}
RS                          {ECHO ; return T_RS      ;}
KRT                         {ECHO ; return T_KRT     ;}
KRS                         {ECHO ; return T_KRS     ;}
KS                          {ECHO ; return T_KS      ;}
KR                          {ECHO ; return T_KR      ;}
RACCS                       {ECHO ; return T_RACCS   ;}
RACCD                       {ECHO ; return T_RACCD   ;}
VDDdeg                      {ECHO ; return T_VDDdeg  ;}
VSSdeg                      {ECHO ; return T_VSSdeg  ;}
VDEG                        {ECHO ; return T_VDEG    ;}
VTI                         {ECHO ; return T_VTI     ;}
VTHR                        {ECHO ; return T_VTHR    ;}
DTHR                        {ECHO ; return T_DTHR    ;}
SHTHR                       {ECHO ; return T_SHTHR   ;}
SLTHR                       {ECHO ; return T_SLTHR   ;}
CGS                         {ECHO ; return T_CGS     ;}
CGS0                        {ECHO ; return T_CGS0    ;}
CGSU                        {ECHO ; return T_CGSU    ;}
CGSU0                       {ECHO ; return T_CGSU0   ;}
CGSUMIN                     {ECHO ; return T_CGSUMIN ;}
CGSUMAX                     {ECHO ; return T_CGSUMAX ;}
CGSD                        {ECHO ; return T_CGSD    ;}
CGSD0                       {ECHO ; return T_CGSD0   ;}
CGSDMIN                     {ECHO ; return T_CGSDMIN ;}
CGSDMAX                     {ECHO ; return T_CGSDMAX ;}
CGP                         {ECHO ; return T_CGP     ;}
CGPUMIN                     {ECHO ; return T_CGPUMIN ;}
CGPUMAX                     {ECHO ; return T_CGPUMAX ;}
CGPDMIN                     {ECHO ; return T_CGPDMIN ;}
CGPDMAX                     {ECHO ; return T_CGPDMAX ;}
CGD                         {ECHO ; return T_CGD     ;}
CGD0                        {ECHO ; return T_CGD0    ;}
CGD1                        {ECHO ; return T_CGD1    ;}
CGD2                        {ECHO ; return T_CGD2    ;}
CGDC                        {ECHO ; return T_CGDC    ;}
CGDC0                       {ECHO ; return T_CGDC0   ;}
CGDC1                       {ECHO ; return T_CGDC1   ;}
CGDC2                       {ECHO ; return T_CGDC2   ;}
CGSI                        {ECHO ; return T_CGSI    ;}
CGSIC                       {ECHO ; return T_CGSIC   ;}
CDS                         {ECHO ; return T_CDS     ;}
CDSU                        {ECHO ; return T_CDSU    ;}
CDSD                        {ECHO ; return T_CDSD    ;}
CDP                         {ECHO ; return T_CDP     ;}
CDPU                        {ECHO ; return T_CDPU    ;}
CDPD                        {ECHO ; return T_CDPD    ;}
CDW                         {ECHO ; return T_CDW     ;}
CDWU                        {ECHO ; return T_CDWU    ;}
CDWD                        {ECHO ; return T_CDWD    ;}
CSS                         {ECHO ; return T_CSS     ;}
CSSU                        {ECHO ; return T_CSSU    ;}
CSSD                        {ECHO ; return T_CSSD    ;}
CSP                         {ECHO ; return T_CSP     ;}
CSPU                        {ECHO ; return T_CSPU    ;}
CSPD                        {ECHO ; return T_CSPD    ;}
CSW                         {ECHO ; return T_CSW     ;}
CSWU                        {ECHO ; return T_CSWU    ;}
CSWD                        {ECHO ; return T_CSWD    ;}
MOS                         {ECHO ; return T_MOS     ;}
NMOS                        {ECHO ; return T_NMOS    ;}
PMOS                        {ECHO ; return T_PMOS    ;}
NNAME                       {ECHO ; return T_NNAME   ;}
PNAME                       {ECHO ; return T_PNAME   ;}
DLN                         {ECHO ; return T_DLN     ;}
DLP                         {ECHO ; return T_DLP     ;}
DWN                         {ECHO ; return T_DWN     ;}
DWP                         {ECHO ; return T_DWP     ;}
VTN                         {ECHO ; return T_VTN     ;}
VTP                         {ECHO ; return T_VTP     ;}
BN                          {ECHO ; return T_BN      ;}
BP                          {ECHO ; return T_BP      ;}
AN                          {ECHO ; return T_AN      ;}
AP                          {ECHO ; return T_AP      ;}
RNT                         {ECHO ; return T_RNT     ;}
RPT                         {ECHO ; return T_RPT     ;}
CGSN                        {ECHO ; return T_CGSN    ;}
CGSP                        {ECHO ; return T_CGSP    ;}
CGPN                        {ECHO ; return T_CGPN    ;}
CGPP                        {ECHO ; return T_CGPP    ;}
CDSN                        {ECHO ; return T_CDSN    ;}
CDSP                        {ECHO ; return T_CDSP    ;}
CDPN                        {ECHO ; return T_CDPN    ;}
CDPP                        {ECHO ; return T_CDPP    ;}
CDWN                        {ECHO ; return T_CDWN    ;}
CDWP                        {ECHO ; return T_CDWP    ;}
CSSN                        {ECHO ; return T_CSSN    ;}
CSSP                        {ECHO ; return T_CSSP    ;}
CSPN                        {ECHO ; return T_CSPN    ;}
CSPP                        {ECHO ; return T_CSPP    ;}
CSWN                        {ECHO ; return T_CSWN    ;}
CSWP                        {ECHO ; return T_CSWP    ;}
ALIAS                       {ECHO ; return T_NAME_ALIAS ;}
{number}                    {
                            ECHO ;
                            elplval.elpdbl = atof(yytext) ;
                            return T_NUMBER ;
                            }
[a-zA-Z&_!?$%][^ =:\t\n()]* {
                            ECHO ;
                            elplval.elpstr = namealloc(yytext) ;
                            return T_STRING ;
                            }
%%

int yywrap() { return 1 ; }

