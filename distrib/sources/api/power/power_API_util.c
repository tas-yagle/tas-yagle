/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : AVERTEC global tools                                        */
/*    Fichier : power_API_util.c                                            */
/*                                                                          */
/*    © copyright 2009 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Olivier BICHAUT                                           */
/*                                                                          */
/****************************************************************************/

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>

#include BCK_H
#include STM_H
#include TTV_H
#include MLU_H
#include MUT_H
#include INF_H
#include EFG_H
#include TAS_H
#include TRC_H
#include YAG_H
#include MCC_H
#include INF_H
#include VCD_H
#define API_USE_REAL_TYPES
#include "power_API.h"
#include "power_API_util.h"

void SwitchingPower (TimingFigure *tf)
{
    chain_list *chainevents, *chain, *chvcdsig, *ch, *chainsigs, *chsig /*, *chainsigs2, *chsig2 */;
    char *signame, *name;
    char value, test=1;
    ttvsig_list *ttvsig, *prevsig;
    ptype_list *ptype, *ptypenodevcdinfo;
    ttvline_list *line;
    ttvevent_list *node;
    timing_model *modmax;
    double nrjmax, nrjtime, nrjsig, consototale=0.0;
    float slope;
    ht *htsig=NULL;
    long search;
    long slew;
    ttvtrans_vcd_power *transvcdpower, *tvp;
    FILE *ptf;
    ttvnode_vcd_info *ttvnodevcdinfo;

    ptf = mbkfopen( "toto4", "dat", WRITE_TEXT );
    ttv_disablecache(tf);
    vcd_goto_first_time ();
    while(test){
        chainevents = cdl_get_current_events();
        nrjtime = 0.0;
        for (chain = chainevents; chain; chain = chain->NEXT){
          for (chvcdsig = cdl_geteventsig (chain); chvcdsig; chvcdsig = chvcdsig->NEXT){
//            signame = cdl_geteventsig (chain);
            signame = (char*)chvcdsig->DATA;
            name = mbk_devect(signame, "(", ")");
            signame = ttv_revect(name);
            value = cdl_geteventval (chain);
            if(value=='0' || value=='1'){
                nrjsig = 0.0;
                chainsigs = ttv_get_signals_from_netname(tf, &htsig, signame);
                for(chsig = chainsigs; chsig; chsig = chsig->NEXT){
                    nrjmax = 0.0;
                    ttvsig = (ttvsig_list*)chsig->DATA;
                    if(!ttvsig) continue;
                    ttv_expfigsig (tf, ttvsig, ttvsig->ROOT->INFO->LEVEL, tf->INFO->LEVEL, TTV_STS_DUAL, TTV_FILE_DTX);
                    if(value=='0')
                        node = &ttvsig->NODE[0];
                    else if(value=='1')
                        node = &ttvsig->NODE[1];
                    if((ptypenodevcdinfo = getptype(node->USER, TTV_NODE_VCD_INFO)) != NULL){
                        prevsig = ((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->PREVSIG;
                    }else{
                        prevsig = NULL;
                    }
                    for(line = node->INLINE; line; line = line->NEXT){
                        if(line->NODE->ROOT == prevsig){
                            modmax = stm_getmodel( tf->INFO->FIGNAME, line->MDMAX );
                            if(modmax){

                                if((ptypenodevcdinfo = getptype(line->NODE->USER, TTV_NODE_VCD_INFO)) != NULL){
                                    slope = ((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->SLOPE;
                                }else{
                                    if( line->TYPE & ( TTV_LINE_F | TTV_LINE_E | TTV_LINE_D ) )
                                        search = TTV_FIND_LINE ;
                                    else
                                        search = TTV_FIND_PATH ;
                                    ttv_getnodeslope(tf,NULL,line->NODE, &slew,search);
                                    slope = slew/TTV_UNIT;
                                }
                                
                                nrjmax += stm_energy_eval (modmax, ttvsig->CAPA , stm_mod_shrinkslew_thr2scm( modmax, slope ));
                            }
                        }
                    }

                    if(nrjmax > 0.0){
                        fprintf(ptf,"%ld %s %s %g\n", vcd_get_time(), prevsig->NAME, ttvsig->NAME, nrjmax) ;

                        transvcdpower = (ttvtrans_vcd_power *)mbkalloc(sizeof (struct ttvtrans_vcd_power));
                        transvcdpower->NEXT = NULL;
                        transvcdpower->PREV = NULL;
                        transvcdpower->SIG = ttvsig;
                        transvcdpower->TIME = vcd_get_time ();
                        transvcdpower->NRJ = (float)nrjmax;
                        if((ptype = getptype(ttvsig->USER, TTV_SIG_VCDPOWER)) != NULL){
                            for(tvp = (ttvtrans_vcd_power*)ptype->DATA; tvp->NEXT; tvp = tvp->NEXT);
                            transvcdpower->PREV = tvp;
                            tvp->NEXT = transvcdpower;
                        }else{
                            ttvsig->USER = addptype(ttvsig->USER, TTV_SIG_VCDPOWER, transvcdpower);
                        }
                        if((ptype = getptype(ttvsig->ROOT->USER, TTV_FIG_VCDPOWER)) != NULL){
                            ptype->DATA = (void*)addchain((chain_list *)ptype->DATA, transvcdpower);
                        }else{
                            ttvsig->ROOT->USER = addptype(ttvsig->ROOT->USER, TTV_FIG_VCDPOWER, addchain(NULL, transvcdpower));
                        }
                    }

                    consototale += nrjmax;
                    nrjtime += nrjmax;
                    nrjsig += nrjmax;
//                    fprintf(stdout, "%s : %c : %g\n", ttvsig->NAME, value, nrjmax);
                }
//                vcd_add_trans_nrj((trans_list*)chain->DATA, nrjsig);
            }
          }
        }
        vcd_add_nrj((float)nrjtime);
        for (chain = chainevents; chain; chain = chain->NEXT){
          for (chvcdsig = cdl_geteventsig (chain); chvcdsig; chvcdsig = chvcdsig->NEXT){
//            signame = cdl_geteventsig (chain);
            signame = (char*)chvcdsig->DATA;
            name = mbk_devect(signame, "(", ")");
            signame = ttv_revect(name);
            value = cdl_geteventval (chain);
            if(value=='0' || value=='1'){
                chainsigs = ttv_get_signals_from_netname(tf, &htsig, signame);
                for(chsig = chainsigs; chsig; chsig = chsig->NEXT){
                    ttvsig = (ttvsig_list*)chsig->DATA;
                    if(!ttvsig) continue;
                    ttv_expfigsig (tf, ttvsig, ttvsig->ROOT->INFO->LEVEL, tf->INFO->LEVEL, TTV_STS_DUAL, TTV_FILE_DTX);
                    if(value=='0')
                        node = &ttvsig->NODE[0];
                    else if(value=='1')
                        node = &ttvsig->NODE[1];
                    ptype = getptype (node->USER, TTV_NODE_DUALLINE);
                    if (ptype) {
                        for (ch = (chain_list*)ptype->DATA; ch; ch = ch->NEXT) {
                            line = (ttvline_list*)ch->DATA;
                            if (((line->TYPE & TTV_LINE_U) != TTV_LINE_U) && ((line->TYPE & TTV_LINE_O) != TTV_LINE_O) && ((line->TYPE & TTV_LINE_RC) != TTV_LINE_RC)) {
                                if((ptypenodevcdinfo = getptype(line->ROOT->USER, TTV_NODE_VCD_INFO)) != NULL){
                                    if((((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->TIME != vcd_get_time())
                                    || ((((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->TIME == vcd_get_time()) && (line->FMAX/TTV_UNIT > ((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->SLOPE))){
                                        ((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->PREVSIG = ttvsig;
                                        ((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->SLOPE = (float)(line->FMAX/TTV_UNIT);
                                        ((ttvnode_vcd_info*)ptypenodevcdinfo->DATA)->TIME = vcd_get_time();
                                    }
                                }else{
                                    ttvnodevcdinfo = (ttvnode_vcd_info *)mbkalloc(sizeof (struct ttvnode_vcd_info));
                                    ttvnodevcdinfo->TIME = vcd_get_time();
                                    ttvnodevcdinfo->PREVSIG = ttvsig;
                                    ttvnodevcdinfo->SLOPE = (float)(line->FMAX/TTV_UNIT);
                                    line->ROOT->USER = addptype(line->ROOT->USER, TTV_NODE_VCD_INFO, ttvnodevcdinfo);
                                }

                                
/*                                chainsigs2 = ttv_get_signals_from_netname(tf, &htsig, line->ROOT->ROOT->NETNAME);
                                for(chsig2 = chainsigs2; chsig2; chsig2 = chsig2->NEXT){
                                    if((ptypeprevsig = getptype(((ttvsig_list*)chsig2->DATA)->USER, TTV_SIG_VCD_PREV_SIG)) != NULL){
                                        ptypeprevsig->DATA = ttvsig;
                                    }else{
                                        ((ttvsig_list*)chsig2->DATA)->USER = addptype(((ttvsig_list*)chsig2->DATA)->USER, TTV_SIG_VCD_PREV_SIG, ttvsig);
                                    }
                                    if((ptslope = getptype(((ttvsig_list*)chsig2->DATA)->USER, TTV_SIG_VCD_SLOPE)) != NULL){
                                        *(float *)&ptslope->DATA=(float)(line->FMAX/TTV_UNIT);
                                    }else{
                                        ptslope = ((ttvsig_list*)chsig2->DATA)->USER = addptype(((ttvsig_list*)chsig2->DATA)->USER, TTV_SIG_VCD_SLOPE, NULL);
                                        *(float *)&ptslope->DATA=(float)(line->FMAX/TTV_UNIT);
                                    }
                                }*/
                            }
                        }
                    }
                }
            }
        }
      }
      test = vcd_goto_next_time ();
    }
    if((ptype = getptype(tf->USER, TTV_FIG_VCDPOWER)) != NULL) ptype->DATA = reverse((chain_list*)ptype->DATA);
    ttv_free_signals_from_netname(htsig);
    htsig = NULL;
    fclose(ptf);
//    fprintf(stdout, "\nCONSO TOTALE: %g\n\n", consototale);
    
}

void DisplaySwitchingPower (void)
{
    int i;
    char buf[512];
    int date;
    float nrj, total = 0.0;

    Board *b;
    b=Board_CreateBoard();
    Board_SetSize(b, 0, 10, 'l');
    Board_SetSize(b, 1, 10, 'l');
    Board_NewLine(b);
    Board_SetValue(b, 0, "Time Date");
    Board_SetValue(b, 1, "Switching Power");
    Board_NewSeparation(b);

    for (i = 1; i <= vcd_get_max_index(); i++){
        Board_NewLine(b);
        date = vcd_get_time_from_index(i);
        nrj = vcd_get_nrj_from_index(i);
        sprintf(buf,"%d", date);
        Board_SetValue(b, 0, buf);
        sprintf(buf,"%g", nrj);
        Board_SetValue(b, 1, buf);
        total += nrj;
    }
    
    Board_NewSeparation(b);
    Board_NewLine(b);
    Board_SetValue(b, 0, "Total");
    sprintf(buf,"%g", total);
    Board_SetValue(b, 1, buf);

    Board_Display(stdout, b, "");
    Board_FreeBoard(b);


}

void DisplaySwitchingPowerSignal (TimingFigure *tf, char *signame)
{
    ttvtrans_vcd_power *trans;
    ptype_list *ptype;
    char buf[512];
    int date;
    float nrj;
    ht *htsig=NULL;
    chain_list *chainsigs, *chsig;
    ttvsig_list *ttvsig;
    
    Board *b;
    b=Board_CreateBoard();
    Board_SetSize(b, 0, 10, 'l');
    Board_SetSize(b, 1, 10, 'l');
    Board_NewSeparation(b);
    Board_NewLine(b);
    Board_SetValue(b, 0, signame);
    Board_SetValue(b, 1, "");
    Board_NewSeparation(b);
    Board_NewLine(b);
    Board_SetValue(b, 0, "Time Date");
    Board_SetValue(b, 1, "Switching Power");
    Board_NewSeparation(b);
    
    chainsigs = ttv_get_signals_from_netname(tf, &htsig, namealloc(signame));
    for(chsig = chainsigs; chsig; chsig = chsig->NEXT){
        ttvsig = (ttvsig_list*)chsig->DATA;
        if((ptype = getptype(ttvsig->USER, TTV_SIG_VCDPOWER)) != NULL){
            for(trans = (ttvtrans_vcd_power*)ptype->DATA; trans; trans = trans->NEXT){
                Board_NewLine(b);
                date = trans->TIME;
                nrj = trans->NRJ;
                sprintf(buf,"%d", date);
                Board_SetValue(b, 0, buf);
                sprintf(buf,"%g", nrj);
                Board_SetValue(b, 1, buf);
            }
        }
    }
    Board_NewSeparation(b);
    Board_Display(stdout, b, "");
    Board_FreeBoard(b);
    
    ttv_free_signals_from_netname(htsig);
    htsig = NULL;
}

float GetSwitchingPowerSignal_sub (TimingFigure *tf, char *signame, double interval_d, double begindate_d, double enddate_d, char *plot)
{
    ttvtrans_vcd_power *trans;
    ptype_list *ptype;
    char buf[512];
    long date, prevdate = -1;
    double nrj, sumnrj, totalnrj;
    ht *htsig=NULL;
    chain_list *chainsigs, *chsig, *chain;
    ttvsig_list *ttvsig;
    FILE    *ptf=NULL ;
    long btime;
    long begindate;
    long interval;
    long enddate;
    float *nrjtab;
    long index = 0;
    long nbelem;
    long timemax = 0;
    
    begindate = mbk_long_round(begindate_d/(vcd_get_ts()));
    if(interval_d > 0)
        interval = mbk_long_round(interval_d/(vcd_get_ts()));
    else
        interval = 1;
    enddate = mbk_long_round(enddate_d/(vcd_get_ts()));
            
    btime = begindate + interval;
    Board *b;
    b=Board_CreateBoard();
    Board_SetSize(b, 0, 10, 'l');
    Board_SetSize(b, 1, 10, 'l');

    if(!strcmp(signame, "*")){
        if((ptype = getptype(tf->USER, TTV_FIG_VCDPOWER)) != NULL){
            sumnrj = 0.0;
            totalnrj = 0.0;
            for(chain = (chain_list*)ptype->DATA; chain; chain = chain->NEXT){
                trans = (ttvtrans_vcd_power*)chain->DATA;
                date = trans->TIME;
                if((interval == 1) && (date == prevdate)){
                    nrj += trans->NRJ;
                }else{
                    nrj = trans->NRJ;
                }
                if(date <= begindate) continue;
                if((btime - interval) < begindate || (enddate > 0 && (btime - interval) > enddate)){
                    btime += interval;
                    continue;
                }
                if(date <= btime){
                    sumnrj += nrj;
                }else if(((interval == 1) || sumnrj != 0.0 || (sumnrj == 0.0 && interval != 1)) 
                      && (chain->NEXT && ((ttvtrans_vcd_power*)chain->NEXT->DATA)->TIME != date)){
                    Board_NewLine(b);
                    if(interval == 1){
                        sprintf(buf,"%ld", date);
                    }else{
                        sprintf(buf,"%ld", btime - interval/2);
                    }
                    Board_SetValue(b, 0, buf);
                    if(interval == 1){
                        sprintf(buf,"%g", nrj);
                    }else{
                        sprintf(buf,"%g", sumnrj / interval);
                    }
                    Board_SetValue(b, 1, buf);
                    if(interval == 1){
                        totalnrj += nrj;
                    }else{
                        totalnrj += sumnrj;
                        sumnrj = nrj;
                        btime += interval;
                    }
                }
                if(!chain->NEXT || (enddate > 0 && date > enddate)){
                    Board_NewLine(b);
                    if(interval == 1){
                        totalnrj += nrj;
                        sprintf(buf,"%ld", date);
                    }else{
                        sprintf(buf,"%ld", btime - interval/2);
                        totalnrj += sumnrj;
                    }
                    Board_SetValue(b, 0, buf);
                    if(interval == 1){
                        sprintf(buf,"%g", nrj);
                    }else{
                        sprintf(buf,"%g", sumnrj / interval);
                    }
                    Board_SetValue(b, 1, buf);
                    break;
                }
                prevdate = date;
            }
        }
    }else{
        chainsigs = ttv_get_signals_from_netname(tf, &htsig, namealloc(signame));
        for(chsig = chainsigs; chsig; chsig = chsig->NEXT){
            ttvsig = (ttvsig_list*)chsig->DATA;
            if((ptype = getptype(ttvsig->USER, TTV_SIG_VCDPOWER)) != NULL){
                for(trans = (ttvtrans_vcd_power*)ptype->DATA; trans; trans = trans->NEXT){
                    if(trans->TIME > timemax)
                        timemax = trans->TIME;
                }
            }
        }
        nbelem = (long)(timemax / interval) + 1;
        nrjtab = (float*)mbkalloc (nbelem * sizeof (float));
        memset(nrjtab, 0, nbelem * sizeof (float));
        for(chsig = chainsigs; chsig; chsig = chsig->NEXT){
            ttvsig = (ttvsig_list*)chsig->DATA;
            if((ptype = getptype(ttvsig->USER, TTV_SIG_VCDPOWER)) != NULL){
                for(trans = (ttvtrans_vcd_power*)ptype->DATA; trans; trans = trans->NEXT){
                    date = trans->TIME;
                    nrj = trans->NRJ;
                    nrjtab[(long)(date / interval)] += nrj;
                }
            }
        }
        totalnrj = 0.0;
        for(index = 0; index < nbelem; index++){
            if((index * interval) < begindate || (enddate > 0 && (index * interval) > enddate) || (nrjtab[index] == 0 && interval == 1)) continue;
            Board_NewLine(b);
            if(interval == 1){
                sprintf(buf,"%ld", index);
            }else{
                sprintf(buf,"%ld", index * interval + interval/2);
            }
            Board_SetValue(b, 0, buf);
            sprintf(buf,"%g", nrjtab[index] / interval);
            Board_SetValue(b, 1, buf);
            totalnrj += nrjtab[index];
        }
        mbkfree(nrjtab);
    }
    if(strcmp(plot, "NULL")){
//        sprintf(buf,"%s_%ld", signame, interval);
        ptf = mbkfopen( plot, "plt", WRITE_TEXT );
        fprintf(ptf,"set origin 0,0\n") ;
        fprintf(ptf,"set xrange [0.0:*]\n") ;
        fprintf(ptf,"set yrange [0.0:*]\n") ;
        fprintf(ptf,"set xlabel \"TIME (PS)\"\n") ;
        if(interval == 1){
            fprintf(ptf,"set ylabel \"ENERGY (J)\"\n") ;
            fprintf(ptf,"plot '%s.dat' with impulses\n", plot) ; 
        }else{
            fprintf(ptf,"set ylabel \"POWER (W)\"\n") ;
            fprintf(ptf,"plot '%s.dat' with boxes\n", plot) ; 
        }
        fprintf(ptf,"pause -1 'Hit CR to finish'\n") ;
        
        fclose(ptf) ;
        ptf = mbkfopen( plot, "dat", WRITE_TEXT );
        Board_Display(ptf, b, "");
        fclose(ptf) ;
    }
    Board_FreeBoard(b);
    
    ttv_free_signals_from_netname(htsig);
    htsig = NULL;
    
    return totalnrj / interval;
}

