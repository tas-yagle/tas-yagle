/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   Alliance                     */
/*                                                                          */
/*    Produit : TAS Verison 5                                               */
/*    Fichier : tas_err.c                                                   */
/*                                                                          */
/*    (c) copyright 1991-1998 Laboratoire LIP6 equipe ASIM                  */
/*    Tous droits reserves                                                  */
/*    Support : e-mail alliance-support@asim.lip6.fr                        */
/*                                                                          */
/*    Auteur(s) : Payam KIANI                                               */
/*                Karim DIOURY                                              */
/*                                                                          */
/****************************************************************************/
/* traitement des erreurs fatales et avertissements                         */
/****************************************************************************/

#include "tas.h"

/****************************************************************************/
/*                          fonction tas_error()                            */
/* traitement des erreurs et avertissements.                                */
/****************************************************************************/
int tas_error( short numero, char  *message, long  type) 
{
    long avttype ;

    if( type == TAS_ERROR ) avttype = AVT_ERROR ;
    else avttype = AVT_WARNING ;

    switch (numero) {
    /* option -h:affichage des options */
    case 0 : 
    fflush(stdout) ;
    fprintf(stdout,"\nUsage   : %s [options] <filename>",TAS_CONTEXT->TAS_TOOLNAME) ;
    fprintf(stdout,"\nOptions : -t[$]           intermediate perfmodule file, with '$' exits\n") ;
    fprintf(stdout,"                          after generation\n") ;
    fprintf(stdout,"          -e              generates a slope file ('.slo')\n") ;
    fprintf(stdout,"          -c              generates a cone netlist file ('.cns')\n") ;
    fprintf(stdout,"          -d              all latches are differential\n") ;
    fprintf(stdout,"          -a              all pathes are saved\n") ;
    fprintf(stdout,"          -u[=<flatname>] black boxes are treated hirarchicaly\n") ;
    fprintf(stdout,"          -y              black boxes are ignored \n") ;
    fprintf(stdout,"          -q              flat cells in timing view \n") ;
    fprintf(stdout,"          -tec=<name>     technology file name\n") ;
    fprintf(stdout,"          -out=<name>     output file name\n") ;
    fprintf(stdout,"          -fig=<name>     input figure name\n") ;
    fprintf(stdout,"          -in=<format>    input format (al, fne, fdn, hns, hdn, spi, sp, cns)\n") ;
    fprintf(stdout,"          -s              silent mode\n") ;
    fprintf(stdout,"          -p=<n>          depth level of functional analysis\n") ;
    fprintf(stdout,"          -b              transistor orientation\n") ;
    fprintf(stdout,"          -z              functional analysis through HZ nodes\n") ;
    fprintf(stdout,"          -nv             no vectorisation\n") ;
    fprintf(stdout,"          -o              takes the '_s' convention into account\n") ;
    fprintf(stdout,"          -fcl            transistor netlist recognition\n") ;
    fprintf(stdout,"          -xfcl           only transistor netlist recognition\n") ;
    fprintf(stdout,"          -gns            hierarchical recognition\n") ;
    fprintf(stdout,"          -xg             only hierarchical recognition\n") ;
    fprintf(stdout,"          -cl             allows latches to share command\n") ;
    fprintf(stdout,"          -fl             flipflop detection\n") ;
    fprintf(stdout,"          -la             automatic latch detection\n") ;
    fprintf(stdout,"          -ls             chooses slave as flipflop\n") ;
    fprintf(stdout,"          -nl             no latch detection\n") ;
    fprintf(stdout,"          -rpt            removes parallel transistors\n") ;
    fprintf(stdout,"          -str            stability analysis\n") ;
    fprintf(stdout,"          -pcd            preserve connectors direction\n") ;
    fprintf(stdout,"          -x=<min>:<max>  detail of critical pathes for which delay is between min\n") ;
    fprintf(stdout,"                          and max, all pathes if no argument\n") ;
    fprintf(stdout,"          -pch            precharged signals taked into account\n") ;
    fprintf(stdout,"          -lv             last level of timing analysis\n") ;
    fprintf(stdout,"          -carac=lut_int  lookup-tables of interface delays.\n") ;
    fprintf(stdout,"                 lut_cpl  lookup-tables of all delays.\n") ;
    fprintf(stdout,"                 scm_int  scm models of interface delays.\n") ;
    fprintf(stdout,"                 scm_cpl  scm models of all delays.\n") ;
    fprintf(stdout,"          -max            no minimum path analysis\n") ;
    fprintf(stdout,"          -opc=<f>        out of path capacitance factor\n") ;
    fprintf(stdout,"          -swc=<f>        input pass-transistor capacitance factor\n") ;
    fprintf(stdout,"          -ctc=<f>        crosstalk capacitance factor\n") ;
    fprintf(stdout,"          -cout=<val>     output terminals charge (pf)\n") ;
    fprintf(stdout,"          -slope=<val>    connectors input slope (ps)\n") ;
    fprintf(stdout,"          -xin=<\"name\">   input or latch concerned by analysis\n") ;
    fprintf(stdout,"          -xout=<\"name\">  output or latch concerned by analysis\n") ;
    fprintf(stdout,"          -n              old perfmodules (ttv and dtv) are saved\n") ;
    fprintf(stdout,"          -nvx            new and old perfmodules are saved\n") ;
    fprintf(stdout,"          -f=<type|value> path factorisation with I points\n") ;
    fprintf(stdout,"          -hr             hierarchical mode\n") ;
    fprintf(stdout,"          -rc             saves RC file\n") ;
    fprintf(stdout,"          -w              checks Elmore delays\n") ;
    fprintf(stdout,"          -nr             mode without RC\n") ;
    fprintf(stdout,"          -mg             merges RC and gate delays\n") ;
    fprintf(stdout,"          -nm             removes lookup-tables\n") ;
    fprintf(stdout,"          -bk             breaks loops in RC networks\n") ;
    fprintf(stdout,"          -lm=<value>     limits memory size (Mo)\n") ;
    fprintf(stdout,"          -pwl[$]         generates PwlFall and PwlRise, with '$' exits\n") ;
    fprintf(stdout,"                          after generation\n") ;
    fprintf(stdout,"          -uk             english mode\n") ;
    fprintf(stdout,"          -h              help option, shows the current page\n") ;
    break ;

    case  1 : avt_errmsg( TAS_ERRMSG, "001", avttype ); break ;
    case  2 : avt_errmsg( TAS_ERRMSG, "002", avttype, message ); break ;
    case  3 : avt_errmsg( TAS_ERRMSG, "003", avttype, message ); break ;
    case  4 : avt_errmsg( TAS_ERRMSG, "004", avttype, message ); break ;
    case  5 : avt_errmsg( TAS_ERRMSG, "005", avttype, message ); break ;
    case  6 : avt_errmsg( TAS_ERRMSG, "006", avttype, message ); break ;
    case  7 : avt_errmsg( TAS_ERRMSG, "007", avttype, message ); break ;
    case  8 : avt_errmsg( TAS_ERRMSG, "008", avttype, message ); break ;
    case  9 : avt_errmsg( TAS_ERRMSG, "009", avttype, message ); break ;
    case 10 : avt_errmsg( TAS_ERRMSG, "010", avttype, message ); break ;
    case 11 : avt_errmsg( TAS_ERRMSG, "011", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 12 : avt_errmsg( TAS_ERRMSG, "012", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 13 : avt_errmsg( TAS_ERRMSG, "013", avttype, message ); break ;
    case 14 : avt_errmsg( TAS_ERRMSG, "014", avttype, message ); break ;
    case 15 : avt_errmsg( TAS_ERRMSG, "015", avttype ); break ;
    case 16 : avt_errmsg( TAS_ERRMSG, "016", avttype ); break ;
    case 17 : avt_errmsg( TAS_ERRMSG, "017", avttype ); break ;
    case 18 : avt_errmsg( TAS_ERRMSG, "018", avttype, message ); break ;
    case 19 : avt_errmsg( TAS_ERRMSG, "019", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 20 : avt_errmsg( TAS_ERRMSG, "020", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 21 : avt_errmsg( TAS_ERRMSG, "021", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 22 : avt_errmsg( TAS_ERRMSG, "022", avttype, message ); break ;
    case 23 : avt_errmsg( TAS_ERRMSG, "023", avttype, message ); break ;
    case 24 : avt_errmsg( TAS_ERRMSG, "024", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 25 : avt_errmsg( TAS_ERRMSG, "025", avttype, message ); break ;

    case 26 : 
    fprintf(stderr,"\nUsage: %s ",TAS_CONTEXT->TAS_TOOLNAME) ;
    fprintf(stderr,"[-abcdeinoqsuwyz ") ;
    fprintf(stderr,"-h -pch -pwl($) -uk -fr -rpt -hr -lv -max\n");
    fprintf(stderr,"       -t($) -nvx -nv -rc -nr -str -pcd -fcl -xfcl -gns -xg -cl -fl -la -ls\n");
    fprintf(stderr,"       -nl -mg -nm -carac=<lut_int|lut_cpl|scm_int|scm_cpl>\n");
    fprintf(stderr,"       -tec=<tecfile> -slope=<slope> ") ;
    fprintf(stderr,"-opc=<factor> -ctc=<factor> -p=<depth>\n") ;
    fprintf(stderr,"       -f=<type|value> -cout=<capa> -swc=<factor> ");
    fprintf(stderr,"-lm=<size> -x=<min>:<max>\n");
    fprintf(stderr,"       -xin=<\"name\"> -xout=<\"name\"> ") ;
    fprintf(stderr,"-in=<format> -out=<name> -fig=<name>] <file>\n\n") ;
    break ;

    case 27 : avt_errmsg( TAS_ERRMSG, "027", avttype ); break ;
    case 28 : avt_errmsg( TAS_ERRMSG, "028", avttype ); break ;
    case 29 : avt_errmsg( TAS_ERRMSG, "029", avttype, (long)message ); break ;
    case 30 : avt_errmsg( TAS_ERRMSG, "030", avttype ); break ;
    case 31 : avt_errmsg( TAS_ERRMSG, "031", avttype, message ); break ;
    case 32 : avt_errmsg( TAS_ERRMSG, "032", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 33 : avt_errmsg( TAS_ERRMSG, "033", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 34 : avt_errmsg( TAS_ERRMSG, "034", avttype, ((cone_list *)message)->INDEX, ((cone_list *)message)->NAME ); break ;
    case 35 : avt_errmsg( TAS_ERRMSG, "035", avttype, message ); break ;
    case 36 : avt_errmsg( TAS_ERRMSG, "036", avttype, message ); break ;
    case 37 : avt_errmsg( TAS_ERRMSG, "037", avttype, message ); break ;
    case 38 : avt_errmsg( TAS_ERRMSG, "038", avttype ); break ;
    case 39 : avt_errmsg( TAS_ERRMSG, "039", avttype, message ); break ;
    case 41 : avt_errmsg( TAS_ERRMSG, "041", avttype ); break ;
    case 42 : avt_errmsg( TAS_ERRMSG, "042", avttype, message ); break ;
    case 43 : avt_errmsg( TAS_ERRMSG, "043", avttype, message ); break ;
    case 44 : avt_errmsg( TAS_ERRMSG, "044", avttype ); break ;
    case 45 : avt_errmsg( TAS_ERRMSG, "045", avttype, message ); break ;
    case 46 : avt_errmsg( TAS_ERRMSG, "046", avttype, message ); break ;
    case 47 : avt_errmsg( TAS_ERRMSG, "047", avttype, message ); break ;
    case 48 : avt_errmsg( TAS_ERRMSG, "048", avttype, message ); break ;
    case 50 : avt_errmsg( TAS_ERRMSG, "050", avttype, message ); break ;
    case 51 : avt_errmsg( TAS_ERRMSG, "051", avttype, message ); break ;
    case 52 : avt_errmsg( TAS_ERRMSG, "052", avttype, message ); break ;
    case 53 : avt_errmsg( TAS_ERRMSG, "053", avttype, message ); break ;
    case 54 : avt_errmsg( TAS_ERRMSG, "054", avttype, message ); break ;
    case 55 : avt_errmsg( TAS_ERRMSG, "055", avttype, message ); break ;
    case 56 : avt_errmsg( TAS_ERRMSG, "056", avttype, message ); break ;
    case 57 : avt_errmsg( TAS_ERRMSG, "057", avttype, message ); break ;
    case 58 : avt_errmsg( TAS_ERRMSG, "058", avttype, message ); break ;
    case 59 : avt_errmsg( TAS_ERRMSG, "059", avttype, message ); break ;
    case 60 : avt_errmsg( TAS_ERRMSG, "060", avttype, message ); break ;
    case 61 : avt_errmsg( TAS_ERRMSG, "061", avttype ); break ;
    case 62 : avt_errmsg( TAS_ERRMSG, "062", avttype, message ); break ;
    case 63 : avt_errmsg( TAS_ERRMSG, "063", avttype, message ); break ;
    case 64 : avt_errmsg( TAS_ERRMSG, "064", avttype, message ); break ;
    case 65 : avt_errmsg( TAS_ERRMSG, "065", avttype, message ); break ;
    case 66 : avt_errmsg( TAS_ERRMSG, "066", avttype, message ); break ;
    case 67 : avt_errmsg( TAS_ERRMSG, "067", avttype, message ); break ;
    case 68 : avt_errmsg( TAS_ERRMSG, "068", avttype ); break ;
    case 69 : avt_errmsg( TAS_ERRMSG, "069", avttype, message ); break ;
    case 70 : avt_errmsg( TAS_ERRMSG, "070", avttype, message ); break ;
    case 71 : avt_errmsg( TAS_ERRMSG, "071", avttype, message ); break ;
    }

    if(type == TAS_NULL) {
        tas_GetKill(numero) ;
    }

    return 0 ;
}
