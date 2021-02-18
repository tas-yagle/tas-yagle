/****************************************************************************/
/*                                                                          */
/* file         : bsc_drive.c                                               */
/* date         : March 2003                                                */
/* version      : v100                                                      */
/* author(s)    : PINTO A.                                                  */
/*                                                                          */
/* description  : SystemC HDL driver                                        */
/* contents     : defines and structure definitions used in BSC library     */
/*                                                                          */
/****************************************************************************/

#ifndef BSC_LIB
#define BSC_LIB

/****************************************************************************/
/*{{{                    Defines                                            */
/****************************************************************************/
#define  BSC_TRACE            0x00000001   /* print messages when parsing   */
#define  BSC_KEEPAUX          0x00000002   /* keep internal signals         */
#define  BSC_CHECKEMPTY       0x00000004   /* check for empty architecture  */

#define  BSC_NEWDFN           1

/*}}}************************************************************************/
/*{{{                    Functions                                          */
/****************************************************************************/
void            bscsavebefig (befig_list *ptbefig, unsigned int trace_mode);

//bepor_list     *print_one_bepor (FILE *fp, bepor_list *ptbepor);
//void            print_one_bebux (FILE *fp, bebux_list *ptbebux, int *numprocess);
//void            print_one_bebus (FILE *fp, bebus_list *ptbebus, int *numprocess);
//void            print_one_beaux (FILE *fp, beaux_list *ptbeaux);
//void            print_one_beout (FILE *fp, beout_list *ptbeout);
//void            print_one_bereg (FILE *fp, bereg_list *ptbereg, int *numprocess);
//void            settimeunit (unsigned char time_unit_char);
//void            print_one_bevectpor (FILE *fp, bevectpor_list *ptbevectpor);
//void            print_one_bevectbux (FILE *fp, bevectbux_list *ptbevectbux, int *numprocess);
//void            print_one_bevectbus (FILE *fp, bevectbus_list *ptbevectbus, int *numprocess);
//void            print_one_bevectaux (FILE *fp, bevectaux_list *ptbevectaux);
//void            print_one_bevectout (FILE *fp, bevectout_list *ptbevectout);
//void            print_one_bevectreg (FILE *fp, bevectreg_list *ptbevectreg, int *numprocess);
//char           *bsc_name (char *name);

/*}}}************************************************************************/

#endif
