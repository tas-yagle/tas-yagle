 
/* 10/23/97 Cone Netlist Structure #defines: cnsd.h                           */

/* note: the first nibble of any USER field is used as a tool signature,      */
/*       0 is for CNS, 1 is for YAGLE, 2 is for TAS and 3 is for SIMULATOR    */

/* CNS environment #defines */

#define CNS_SILENT         ((long) 0x00000000)
#define CNS_TRACE          ((long) 0x00000001)
#define CNS_DEBUG          ((long) 0x00000002)


/* CNS general #defines: user oriented */

#define CNS_UNKNOWN        ((long) 0x08000000)
#define CNS_MULTI_PURPOSE  ((long) 0x04000000)
#define CNS_SEEN           ((long) 0x02000000)
#define CNS_INDEX          ((long) 0x01000000)
#define CNS_POWER          ((long) 0x00000001)
#define CNS_VDD            ((long) 0x00000003)
#define CNS_GND            ((long) 0x00000005)
#define CNS_VSS            ((long) 0x00000009)


/* types for the DIRECTION field of connectors */

#define CNS_I              ((char) 'I')         /* I */
#define CNS_O              ((char) 'O')         /* O */
#define CNS_B              ((char) 'B')         /* B */
#define CNS_Z              ((char) 'Z')         /* Z */
#define CNS_T              ((char) 'T')         /* T */
#define CNS_X              ((char) 'X')         /* X */
#define CNS_VDDC           ((char) 'D')         /* Vdd */
#define CNS_GNDC           ((char) 'G')         /* Gnd */
#define CNS_VSSC           ((char) 'S')         /* Vss */


/* types for the USER field of connectors */

#define CNS_INDEX          ((long) 0x01000000)
#define CNS_SIGINDEX       ((long) 0x00200000)
#define CNS_NODELIST       ((long) 0x00400000)
#define CNS_EXT            ((long) 0x00001000)
#define CNS_CONE           ((long) 0x00000100)
#define CNS_INVLOCON       ((long) 0x00100000)
#define CNS_TYPELOCON      ((long) 0x00800000)


/* types for the TYPE field of signals */

#define CNS_SIGINT         ((char) 'I' )
#define CNS_SIGEXT         ((char) 'E' )
#define CNS_SIGINS         ((char) 'X' )
#define CNS_SIGVDD         ((char) 'D' )
#define CNS_SIGVSS         ((char) 'S' )
#define CNS_SIGTRANSP      ((char) 'T' )


/* types for the TYPE field of transistors */

#define CNS_TE             ((char) TRANSN)      /* E */
#define CNS_TD             ((char) TRANSP)      /* D */
#define CNS_TN             ((char) TRANSN)      /* N */
#define CNS_TP             ((char) TRANSP)      /* P */

/* types for the USER field of transistors */

#define CNS_INDEX          ((long) 0x01000000)
#define CNS_LINKTYPE       ((long) 0x00000001)
#define CNS_DRIVINGCONE    ((long) 0x00000004)
#define CNS_CONE           ((long) 0x00000100)
#define CNS_SWITCH         ((long) 0x00000040)
#define CNS_UNUSED         ((long) 0x00000080)
#define CNS_SUPPLY         ((long) 0x00001300)
#define CNS_TEMP_SUPPLY    ((long) 0x00002300)

/* types for the TYPE field of links */
                                                                                                                                                                 /* mask from right to left: power type, number of equipipotentials, active or *//* passive type */
#define CNS_IN             ((long) 0x00003000)  /* In (input connector) */
#define CNS_INOUT          ((long) 0x00005000)  /* InOut (output connector) */
#define CNS_2EQUIP         ((long) 0x00000010)
#define CNS_3EQUIP         ((long) 0x00000020)
#define CNS_SWITCH         ((long) 0x00000040)  /* _s (CMOS switch) */
#define CNS_COMMAND        ((long) 0x00000080)  /* _c (command) */
#define CNS_ACTIVE         ((long) 0x00000100)
#define CNS_PASSIVE        ((long) 0x00000200)
#define CNS_UP             ((long) 0x00000400)
#define CNS_DOWN           ((long) 0x00000800)
#define CNS_SW             ((long) 0x00010120)  /* S (switch) */
#define CNS_PULL           ((long) 0x00020210)  /* P (pull up or down) */
#define CNS_DRIV_PULL      ((long) 0x00040120)  /* p (driven pull up or down */
#define CNS_DIODE_UP       ((long) 0x00080610)  /* DU (diode orientation) */
#define CNS_DIODE_DOWN     ((long) 0x00080a10)  /* DD (diode orientation) */
#define CNS_RESIST         ((long) 0x00100200)  /* R (resistor) */
#define CNS_CAPA           ((long) 0x00200210)  /* C (capacitor) */
#define CNS_DIPOLE         ((long) 0x00400210)  /* d (dipole) */
#define CNS_TNLINK         ((long) 0x80000000)
#define CNS_TPLINK         ((long) 0x40000000)
#define CNS_SHORT          ((long) 0x20000000)
#define CNS_ASYNCLINK      ((long) 0x00800000)


/* types for the TYPE field of branches */

#define CNS_VDD            ((long) 0x00000003)  /* Vdd */
#define CNS_GND            ((long) 0x00000005)  /* Gnd */
#define CNS_VSS            ((long) 0x00000009)  /* Vss */
#define CNS_EXT            ((long) 0x00001000)  /* Ext */
#define CNS_VDD_DEGRADED   ((long) 0x00000050)  /* Dd */
#define CNS_VSS_DEGRADED   ((long) 0x000000c0)  /* Ds */
#define CNS_DEGRADED       ((long) 0x00000040)  /* D */
#define CNS_PARALLEL       ((long) 0x00000100)  /* P (branch model) */
#define CNS_PARALLEL_INS   ((long) 0x00000300)  /* p (branch instance) */
#define CNS_FEEDBACK       ((long) 0x00000400)  /* F */
#define CNS_NOT_FUNCTIONAL ((long) 0x00010000)  /* Nf */
#define CNS_BLEEDER        ((long) 0x00030000)  /* B */
#define CNS_RESBRANCH      ((long) 0x00008000)
#define CNS_IGNORE         ((long) 0x00004000)
#define CNS_NOTCONFLICTUAL ((long) 0x00002000)
#define CNS_NOT_UP         ((long) 0x00040000)
#define CNS_NOT_DOWN       ((long) 0x00080000)

#define CNS_MASK_PARA (CNS_PARALLEL_INS & ~CNS_PARALLEL)

/* types for the TYPE field of edges */

#define CNS_VDD            ((long) 0x00000003)
#define CNS_GND            ((long) 0x00000005)
#define CNS_VSS            ((long) 0x00000009)
#define CNS_EXT            ((long) 0x00001000)
#define CNS_CONE           ((long) 0x00000100)
#define CNS_BLEEDER        ((long) 0x00030000)  /* bleeder */
#define CNS_COMMAND        ((long) 0x00000080)  /* commmand */
#define CNS_LOOP           ((long) 0x00000200)  /* loop */
#define CNS_MEMSYM         ((long) 0x00004000)  /* memsym loop */
#define CNS_FEEDBACK       ((long) 0x00000400)  /* feedback */
#define CNS_HZCOM          ((long) 0x00000800)  /* tristate command */
#define CNS_ASYNC          ((long) 0x00080000)  /* async command */

/* types for the USER field of edges */

#define CNS_SWITCHPAIR     ((long) 0x00000040)

/* types for the TYPE field of cones */

#define CNS_VDD            ((long) 0x00000003)  /* Vdd */
#define CNS_GND            ((long) 0x00000005)  /* Gnd */
#define CNS_VSS            ((long) 0x00000009)  /* Vss */
#define CNS_EXT            ((long) 0x00001000)  /* Ext */
#define CNS_CONFLICT       ((long) 0x00000010)  /* C */
#define CNS_TRI            ((long) 0x00000020)  /* T */
#define CNS_MEMSYM         ((long) 0x00004000)  /* M */
#define CNS_LATCH          ((long) 0x00000080)  /* L */
#define CNS_RS             ((long) 0x00000100)  /* S */
#define CNS_FLIP_FLOP      ((long) 0x00000200)  /* F */
#define CNS_MASTER         ((long) 0x00000400)  /* m */
#define CNS_SLAVE          ((long) 0x00000800)  /* s */
#define CNS_PRECHARGE      ((long) 0x00002000)  /* P */


/* types for the TECTYPE field of cones */

#define CNS_CMOS           ((long) 0x00000001)  /* Cm */
#define CNS_DUAL_CMOS      ((long) 0x00000003)  /* dC */
#define CNS_VDD_DEGRADED   ((long) 0x00000050)  /* VddD */
#define CNS_GND_DEGRADED   ((long) 0x00000060)  /* GndD */
#define CNS_VSS_DEGRADED   ((long) 0x000000c0)  /* VssD */
#define CNS_NOR            ((long) 0x00000100)  /* Nor */
#define CNS_OR             ((long) 0x00000200)  /* Or */
#define CNS_PR_NOR         ((long) 0x00000400)  /* Pr_Nor */
#define CNS_NAND           ((long) 0x00000800)  /* Pr_Nor */
#define CNS_SBE            ((long) 0x00001000)  /* Sbe */
#define CNS_SBD            ((long) 0x00002000)  /* Sbd */
#define CNS_TRISTATE_O     ((long) 0x00004000)  /* Tristate_o */
#define CNS_SQUIRT_O       ((long) 0x00008000)  /* Squirt_o */
#define CNS_LATCH_O        ((long) 0x00010000)  /* Latch_o */
#define CNS_PR_BUS         ((long) 0x00020000)  /* Pr_Bus */
#define CNS_ZERO           ((long) 0x00040000)  /* Zero */
#define CNS_ONE            ((long) 0x00080000)  /* One */
#define CNS_STATE_ZERO     ((long) 0x00100000)  /* State Zero */
#define CNS_STATE_ONE      ((long) 0x00200000)  /* State One */
#define CNS_STATE_UNKNOWN  ((long) 0x00400000)  /* State Unknown */


/* types for the typelocon user of connectors */
#define CNS_ZERO           ((long) 0x00040000)  /* Zero */
#define CNS_ONE            ((long) 0x00080000)  /* One */
#define CNS_STATE_ZERO     ((long) 0x00100000)  /* State Zero */
#define CNS_STATE_ONE      ((long) 0x00200000)  /* State One */
#define CNS_INIT_ZERO      ((long) 0x01000000)  /* Init Zero */
#define CNS_INIT_ONE       ((long) 0x02000000)  /* Init One */

/* types for the USER field of cones */

#define CNS_BLEEDER        ((long) 0x00030000)
#define CNS_SWITCH         ((long) 0x00000040)
#define CNS_PARALLEL       ((long) 0x00000100)
#define CNS_EXT            ((long) 0x00001000)
#define CNS_LOCON          ((long) 0x00000080)
#define CNS_LOTRSGRID      ((long) 0x00000200)
#define CNS_LOTRS          ((long) 0x00000400)
#define CNS_SIGNATURE      ((long) 0x00000800)
#define CNS_UPEXPR         ((long) 0x00002000)
#define CNS_DNEXPR         ((long) 0x00004000)
#define CNS_SIGINDEX       ((long) 0x00200000)
#define CNS_SIGNAL         ((long) 0x00400000)
#define CNS_PARATRANS      ((long) 0x00800000)
#define CNS_INVCONE        ((long) 0x00100000)
#define CNS_SUPPLY         ((long) 0x00001300)
#define CNS_CONEFUNCTION_SIMPLIFIED ((long) 0xfab60421)

/* types for the TYPE field of cells */

#define CNS_SUPER_BUFFER   ((long) 0x00000001)  /* Super_Buffer */
#define CNS_TRISTATE_E     ((long) 0x00000002)  /* Tristate */
#define CNS_SQUIRT_BUFFER  ((long) 0x00000004)  /* Squirt_Buffer */
#define CNS_LATCH_E        ((long) 0x00000008)  /* Latch */
#define CNS_READ           ((long) 0x00000010)  /* Read */
#define CNS_MS_FF          ((long) 0x00000020)  /* Ms_Ff */

/* types for the USER field of cells */

#define CNS_CELL_NAME      ((long) 0x00001111)

/* types for the USER field of cns */

#define CNS_POWER_SUPPLIES ((long) 0x00000787)
#define CNS_CONE_HASHTABLE ((long) 0x00fab007)

/* types for the USER field of signals */

#define CNS_SUPPLY         ((long) 0x00001300)

