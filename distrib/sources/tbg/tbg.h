#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include MUT_H
#include "tbg100.h"

typedef struct t_port {
    struct t_port *NEXT;
    char          *NAME;
    char           FLAG;
    char           DIRECTION;
    int            B0;
    int            B1;
    char          *VALUE;
} t_port;

extern t_port     *HEAD_PORT;
extern FILE       *PAT_FILE;
extern char       *PAT_FILE_NAME;
extern char       *OUT_FILE_NAME;
extern char       *DESIGN_NAME;
extern int         LINE;
extern ht         *GENERICS_HT;
extern chain_list *GENERICS_CHAIN;
extern chain_list *VALUES_CHAIN;
extern int         PERIOD;
extern char        UNIT;

extern t_port *tbg_GetPort (char *port_name);
extern unsigned long log2 (unsigned long n);
extern unsigned long exp2 (unsigned long n);
extern void dec_to_bin (char *bin, unsigned long dec, int bits);
extern void reverse_endianess (char *bin);

#define T_IN 1
#define T_INOUT 2
#define T_OUT 3
