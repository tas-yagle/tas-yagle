/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : CBH Version 1.00                                            */
/*    Fichier : cbh_seq.c                                                   */
/*                                                                          */
/*    (c) copyright 2001 AVERTEC                                            */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) :   Anthony LESTER                                          */
/*                                                                          */
/****************************************************************************/

#include MUT_H
#include LOG_H
#include BEH_H
#include BHL_H
#include BEF_H
#include BVL_H
#include CBH_H
#include AVT_H

// this file is not to date

static void
usage(char *progname)
{
  avt_errmsg(CBH_ERRMSG,"002",AVT_FATAL,progname);
//  fprintf(stderr, "%s needs a file as argument\n", progname);
//  EXIT(1);
}

int
main(int argc, char *argv[])
{
    lofig_list  *ptlofig;
    befig_list  *ptcellbefig;
    beout_list  *ptbeout;
    bebus_list  *ptbebus;
    cbhseq      *ptcbhseq;
    cbhcomb     *ptcbhcomb;
    char         buffer[1024];

#ifdef MACOS
    argc = ccommand(&argv);
    initmacenv(argv[0]);
#endif

    avtenv();

	mbkenv();
    fclenv();

    if (argc != 2) usage("cbhtest");
    ptcellbefig = loadbefig(NULL, argv[1], BVL_KEEPAUX);
    initializeBdd(0);
    ptcbhseq = cbh_getseqfunc(ptcellbefig, CBH_LIB_MODE);
    if (ptcbhseq != NULL) {
        switch (ptcbhseq->SEQTYPE) {
        case CBH_LATCH:
            printf("\tLATCH:\n");
            break;
        case CBH_FLIPFLOP:
            printf("\tFLIP-FLOP:\n");
            break;
        case CBH_UNKNOWN:
            printf("\tUNKNOWN:\n");
            break;
        }
        if (ptcbhseq->NAME != NULL) printf("\t\tNAME = %s\n", ptcbhseq->NAME);
        if (ptcbhseq->NEGNAME != NULL) printf("\t\tNEGNAME = %s\n", ptcbhseq->NEGNAME);
        if (ptcbhseq->PIN != NULL) printf("\t\tPIN = %s\n", ptcbhseq->PIN);
        if (ptcbhseq->NEGPIN != NULL) printf("\t\tNEGPIN = %s\n", ptcbhseq->NEGPIN);
        printf("\t\tCLOCK = ");
        cbh_writeabl(stdout, ptcbhseq->CLOCK);
        printf("\n");
        printf("\t\tSLAVECLOCK = ");
        cbh_writeabl(stdout, ptcbhseq->SLAVECLOCK);
        printf("\n");
        printf("\t\tDATA = ");
        cbh_writeabl(stdout, ptcbhseq->DATA);
        printf("\n");
        printf("\t\tRESET = ");
        cbh_writeabl(stdout, ptcbhseq->RESET);
        printf("\n");
        printf("\t\tSET = ");
        cbh_writeabl(stdout, ptcbhseq->SET);
        printf("\n");
        printf("\t\tRSCONF = ");
        cbh_writeabl(stdout, ptcbhseq->RSCONF);
        printf("\n");
        printf("\t\tRSCONFNEG = ");
        cbh_writeabl(stdout, ptcbhseq->RSCONFNEG);
        printf("\n");
        printf("\n");
    }
    for (ptbeout = ptcellbefig->BEOUT; ptbeout; ptbeout = ptbeout->NEXT) {
        ptcbhcomb = cbh_getcombfunc(ptcellbefig, ptcbhseq, ptbeout->NAME);
        if (ptcbhcomb != NULL) {
            printf("\t%s: ",ptbeout->NAME);
            cbh_writeabl(stdout, ptcbhcomb->FUNCTION);
            printf("\n");
        }
    }
    for (ptbebus = ptcellbefig->BEBUS; ptbebus; ptbebus = ptbebus->NEXT) {
        ptcbhcomb = cbh_getcombfunc(ptcellbefig, ptcbhseq, ptbebus->NAME);
        if (ptcbhcomb != NULL) {
            printf("\t%s: \n",ptbebus->NAME);
            printf("\t\tFUNCTION = ");
            cbh_writeabl(stdout, ptcbhcomb->FUNCTION);
            printf("\n");
            printf("\t\tHZFUNC = ");
            cbh_writeabl(stdout, ptcbhcomb->HZFUNC);
            printf("\n");
            printf("\t\tCONFLICT = ");
            cbh_writeabl(stdout, ptcbhcomb->CONFLICT);
            printf("\n");
            printf("\n");
        }
    }
    sprintf(buffer, "cbh_%s", ptcellbefig->NAME);
    ptcellbefig->NAME = namealloc(buffer);
    savebefig(ptcellbefig, 0);

    EXIT(0);
}
