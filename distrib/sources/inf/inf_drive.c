/****************************************************************************/
/*                                                                          */
/*                      Chaine de CAO & VLSI   AVERTEC                      */
/*                                                                          */
/*    Produit : YAGLE/TAS                                                   */
/*    Fichier : inf_drive.c                                                 */
/*                                                                          */
/*    © copyright 2000 AVERTEC                                              */
/*    Tous droits reserves                                                  */
/*                                                                          */
/*    Auteur(s) : Caroline BLED                                             */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include AVT_H
#include MUT_H
#include INF_H

static int print_mode=0;

void inf_set_print_mode(int val)
{
  print_mode=val;
}

static void inf_fprintf(FILE *f, char *fmt, ...)
{
  va_list pa;
  char buf[32000];
  va_start (pa, fmt);
  vsprintf(buf, fmt, pa);
  va_end(pa);

  if (!print_mode)
   fprintf(f, "%s", buf);
  else
   avt_log(LOGCONFIG, print_mode, "%s", buf); 
}

/* Functions to format the inf file */
/****************************************************************************/
/*      FUNCTION NAME :  inf_leaveLine                                      */
/*             ARG(1) :  Current file descriptor.                           */
/*             ARG(2) :  Number of lines you want to leave.                 */
/*             RETURN :  NONE.                                              */
/*      FUNCTIONALITY :  Leave lines in the current file.                   */
/****************************************************************************/
void inf_leaveLines (FILE * f, int nb)
{
  int i;

  for (i = 0; i < nb; i++) {
    inf_fprintf (f, "\n");
  }
}

/****************************************************************************/
/*      FUNCTION NAME :  inf_tabs                                           */
/*             ARG(1) :  Current file descriptor.                           */
/*             ARG(2) :  Number of tabs you want to have.                   */
/*             RETURN :  NONE.                                              */
/*      FUNCTIONALITY :  prints tabs in the current file.                   */
/****************************************************************************/
void inf_tabs (FILE * f, int nb)
{
  int i;

  for (i = 0; i < nb; i++) {
    inf_fprintf (f, "    ");
  }
}

/****************************************************************************/
/*      FUNCTION NAME :  inf_spaces                                         */
/*             ARG(1) :  Current file descriptor.                           */
/*             ARG(2) :  Number of spaces you want to have.                 */
/*             RETURN :  NONE.                                              */
/*      FUNCTIONALITY :  prints spaces in the current file.                 */
/****************************************************************************/
void inf_spaces (FILE * f, int nb)
{
  int i;

  for (i = 0; i < nb; i++) {
    inf_fprintf (f, " ");
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_reVectName                                           */
/*          ARG(1) : name to revectorize.                                     */
/*          RETURN : name revectorized.                                       */
/*  FUNCTIONNALITY : Searches the space caracter and replace it by braces in  */
/*                   the string given in argument.                            */
/******************************************************************************/
char *inf_reVectName (char *name)
{
  return infTasVectName(name);
  /*
  static char namex[512];
  short i = strlen (name);
  short j = i - 1;
  char vect = 'N';

  strcpy (namex, name);

  do {
    while ((--i != -1) && (namex[i] != ' '))
      if (namex[i] == '.')
        j = i - 1;

    if (i != -1) {
      short k;

      vect = 'Y';
      namex[i] = '[';

      for (k = strlen (namex); k != j; k--)
        namex[k + 1] = namex[k];

      namex[j + 1] = ']';
      j = i - 1;
    }
  }
  while (i != -1);

  if (vect == 'N')
    return (name);
  else
    return (namex);
*/
}


void inf_driveSigChain (FILE * f, char *section, chain_list * lst)
{
  inf_fprintf (f, "%s\nBegin\n", section);
  while (lst) {
    inf_tabs (f, 1);
    inf_fprintf (f, "\"%s\" ;\n", inf_reVectName ((char *)lst->DATA));
    lst = lst->NEXT;
  }
  inf_fprintf (f, "End;\n");
  inf_leaveLines (f, 1);
}


/******************************************************************************/
/*   FUNCTION NAME : inf_driveRename                                          */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the RENAME section of the INF file from the       */
/*                   INF_RENAME list.                                         */
/******************************************************************************/
void inf_driveRename (FILE * f, inffig_list * ifl)
{
  chain_list *list0, *rename;
  inf_assoc *assoc;
  if (inf_GetPointer (ifl, INF_RENAME, "", (void **)&list0)) {
    inf_fprintf (f, "Rename\nBegin\n");
    for (rename = list0; rename; rename = rename->NEXT) {
      inf_tabs (f, 1);
      assoc = (inf_assoc *) rename->DATA;
      inf_fprintf (f, "\"%s\" : \"%s\" ;\n", inf_reVectName (assoc->orig), inf_reVectName (assoc->dest));
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}
void inf_driveMemsym (FILE * f, inffig_list * ifl)
{
  chain_list *list0, *rename;
  inf_assoc *assoc;
  if (inf_GetPointer (ifl, INF_MEMSYM, "", (void **)&list0)) {
    inf_fprintf (f, "Memsym\nBegin\n");
    for (rename = list0; rename; rename = rename->NEXT) {
      inf_tabs (f, 1);
      assoc = (inf_assoc *) rename->DATA;
      inf_fprintf (f, "\"%s\" : \"%s\" ;\n", inf_reVectName (assoc->orig), inf_reVectName (assoc->dest));
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveCklatch                                         */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the CKLATCH section of the INF file from the      */
/*                   INF_CKLATCH list.                                        */
/******************************************************************************/
void inf_driveBinary (FILE * f, inffig_list * ifl, char *section, char *infotype)
{
  chain_list *ch;
  chain_list *lst0, *lst1;

  lst0 = inf_GetEntriesByType (ifl, infotype, INF_YES);
  lst1 = inf_GetEntriesByType (ifl, infotype, INF_NO);
  if (lst0 || lst1) {
    inf_fprintf (f, "%s\nBegin\n", section);
    for (ch = lst0; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_fprintf (f, "  \"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
    }
    for (ch = lst1; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_fprintf (f, "~ \"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (lst0);
    freechain (lst1);
  }
}

void inf_driveSigListSection (FILE * f, inffig_list * ifl, char *section_name, char *infotype, char *val)
{
  chain_list *lst0;

  lst0 = inf_GetEntriesByType (ifl, infotype, val);
  if (lst0) {
    inf_driveSigChain (f, section_name, lst0);
    freechain (lst0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveCkprech                                         */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the CKPRECH section of the INF file from the      */
/*                   INF_CKPRECH list.                                        */
/******************************************************************************/

chain_list *inf_mergeclist (chain_list * l1, chain_list * l2)
{
  chain_list *cl, *ch;
  for (cl = l1; cl; cl = cl->NEXT) {
    for (ch = l2; ch; ch = ch->NEXT)
      if (cl->DATA == ch->DATA)
        break;
    if (ch == NULL)
      l2 = addchain (l2, cl->DATA);
  }
  freechain (l1);
  return l2;
}



void inf_driveCkprech (FILE * f, inffig_list * ifl)
{
  chain_list *ch;
  chain_list *lst0;

  lst0 = inf_GetEntriesByType (ifl, INF_CKPRECH, INF_ANY_VALUES);
  if (lst0) {
    inf_fprintf (f, "CkPrech\nBegin\n");
    for (ch = lst0; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (lst0);
  }
}

void inf_drivePinSlew (FILE * f, inffig_list * ifl)
{
  chain_list *ch;
  chain_list *lst0, *lst1;
  double rise, fall, hv, lv;
  char subbuf[64];
  lst0 = inf_GetEntriesByType (ifl, INF_PIN_RISING_SLEW, INF_ANY_VALUES);
  lst1 = inf_GetEntriesByType (ifl, INF_PIN_HIGH_VOLTAGE, INF_ANY_VALUES);

  lst0 = inf_mergeclist (lst0, lst1);
  lst1 = inf_GetEntriesByType (ifl, INF_PIN_FALLING_SLEW, INF_ANY_VALUES);
  lst0 = inf_SortEntries(inf_mergeclist (lst0, lst1));

  if (lst0) {
    inf_fprintf (f, "PinSlew\nBegin\n");
    for (ch = lst0; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      rise=fall=-1;
      inf_GetDouble (ifl, (char *)ch->DATA, INF_PIN_RISING_SLEW, &rise);
      inf_GetDouble (ifl, (char *)ch->DATA, INF_PIN_FALLING_SLEW, &fall);
      if (!inf_GetDouble (ifl, (char *)ch->DATA, INF_PIN_LOW_VOLTAGE, &lv))
        lv = -DBL_MAX;
      if (!inf_GetDouble (ifl, (char *)ch->DATA, INF_PIN_HIGH_VOLTAGE, &hv))
        hv = -DBL_MAX;

      if (hv == -DBL_MAX)
        strcpy (subbuf, "");
      else if (lv == -DBL_MAX)
        sprintf (subbuf, "Supply=%g ", hv);
      else
        sprintf (subbuf, "Supply=(%g,%g) ", lv, hv);

      inf_fprintf (f, "\"%s\" : %sRise=%.1f Fall=%.1f;\n", inf_reVectName ((char *)ch->DATA), subbuf,
               rise<0?-1:rise*1e12, fall<0?-1:fall*1e12);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (lst0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveDirout                                          */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the DIROUT section of the INF file from the       */
/*                   INF_DIROUT and INF_NOTDIROUT lists.                      */
/******************************************************************************/
void inf_driveDirout (FILE * f, inffig_list * ifl)
{
  chain_list *ch, *list0;
  int val;

  list0 = inf_GetEntriesByType (ifl, INF_DIROUT, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "DirOut\nBegin\n");

    for (ch = list0; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_GetInt (ifl, (char *)ch->DATA, INF_DIROUT, &val);
      switch (val) {
      case -1:
        inf_fprintf (f, "~ \"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
        break;
      case 0:
        inf_fprintf (f, "  \"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
        break;
      default:
        inf_fprintf (f, "  \"%s\" : %d ;\n", inf_reVectName ((char *)ch->DATA), val);
        break;
      }
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain(list0);
  }
}

void inf_driveNoCheck (FILE * f, inffig_list * ifl)
{
  chain_list *ch, *list0;
  int val;
  char *st;

  list0 = inf_GetEntriesByType (ifl, INF_NOCHECK, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "NoCheck\nBegin\n");

    for (ch = list0; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_GetInt (ifl, (char *)ch->DATA, INF_NOCHECK, &val);
      if ((val & (INF_NOCHECK_SETUP | INF_NOCHECK_HOLD)) == (INF_NOCHECK_SETUP | INF_NOCHECK_HOLD))
        st = "All";
      else if (val & INF_NOCHECK_SETUP)
        st = "Setup";
      else if (val & INF_NOCHECK_HOLD)
        st = "Hold";
      else
        st = "";

      if (st != "")
        inf_fprintf (f, "  \"%s\" %s;\n", inf_reVectName ((char *)ch->DATA), st);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain(list0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveMutex                                           */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the MUTEX section of the INF file from the        */
/*                   INF_MUTU, INF_MUTD, INF_CMPU and INF_CMPD lists.         */
/******************************************************************************/
void inf_driveOneMutex (FILE * f, char *header, chain_list * cl)
{
  inf_tabs (f, 1);
  inf_fprintf (f, "%s  {", header);
  while (cl) {
    inf_fprintf (f, " \"%s\"", inf_reVectName ((char *)cl->DATA));
    if (cl->NEXT)
      inf_fprintf (f, ",");
    cl = cl->NEXT;
  }
  inf_fprintf (f, " };\n");
}


void inf_driveMutex (FILE * f, inffig_list * myfig)
{
  chain_list *mutex;
  chain_list *head0 = NULL, *head1 = NULL, *head2 = NULL, *head3 = NULL;

  inf_GetPointer (myfig, INF_MUXU, "", (void **)&head0);
  inf_GetPointer (myfig, INF_MUXD, "", (void **)&head1);
  inf_GetPointer (myfig, INF_CMPU, "", (void **)&head2);
  inf_GetPointer (myfig, INF_CMPD, "", (void **)&head3);

  if (head0 || head1 || head2 || head3) {
    inf_fprintf (f, "Mutex\nBegin\n");

    for (mutex = head0; mutex; mutex = mutex->NEXT) {
      inf_driveOneMutex (f, "MuxUP", (chain_list *) mutex->DATA);
    }
    for (mutex = head1; mutex; mutex = mutex->NEXT) {
      inf_driveOneMutex (f, "MuxDN", (chain_list *) mutex->DATA);
    }
    for (mutex = head2; mutex; mutex = mutex->NEXT) {
      inf_driveOneMutex (f, "CmpUP", (chain_list *) mutex->DATA);
    }
    for (mutex = head3; mutex; mutex = mutex->NEXT) {
      inf_driveOneMutex (f, "CmpDN", (chain_list *) mutex->DATA);
    }

    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

void inf_driveCrosstalkMutex (FILE * f, inffig_list * myfig)
{
  chain_list *mutex;
  chain_list *head0 = NULL, *head1 = NULL;

  inf_GetPointer (myfig, INF_CROSSTALKMUXU, "", (void **)&head0);
  inf_GetPointer (myfig, INF_CROSSTALKMUXD, "", (void **)&head1);

  if (head0 || head1) {
    inf_fprintf (f, "Crosstalk Mutex\nBegin\n");

    for (mutex = head0; mutex; mutex = mutex->NEXT) {
      inf_driveOneMutex (f, "MuxUP", (chain_list *) mutex->DATA);
    }
    for (mutex = head1; mutex; mutex = mutex->NEXT) {
      inf_driveOneMutex (f, "MuxDN", (chain_list *) mutex->DATA);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_delSig                                               */
/*          ARG(1) : structure of the signal to delete from the INF_SIGLIST.  */
/*          ARG(2) : head of the INF_SIGLIST.                                 */
/*          ARG(3) : structure before signal to delete.                       */
/*          RETURN : new head of the INF_SIGLIST.                             */
/*  FUNCTIONNALITY : delete a element form the INF_SIGLIST.                   */
/******************************************************************************/
list_list *inf_delSig (list_list * sig, list_list * head, list_list * previous)
{
  if (sig == head)
    head = sig->NEXT;
  else
    previous->NEXT = sig->NEXT;

  mbkfree (sig);

  return head;
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveConstraint                                      */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the CONSTRAINT section of the INF file from the   */
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_driveConstraint (FILE * f, inffig_list * ifl)
{
  chain_list *list0, *cl;
  int val;
  char *name;

  list0 = inf_GetEntriesByType (ifl, INF_STUCK, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "Constraint\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_GetInt (ifl, name, INF_STUCK, &val);
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\" : %d ;\n", inf_reVectName (name), val);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_drivePrecharge                                       */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the PRECHARGE section of the INF file from the    */
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_drivePrecharge (FILE * f, INF_INFOSET * is)
{
  list_list *l;

  inf_fprintf (f, "Precharge\nBegin\n");
  for (l = is->INF_SIGLIST; l; l = l->NEXT) {
    if (l->TYPE == INF_LL_PRECHARGE) {
      inf_tabs (f, 1);
      inf_fprintf (f, "  \"%s\" ;\n", inf_reVectName ((char *)l->DATA));
    }
    else if (l->TYPE == INF_LL_NOTPRECHARGE) {
      inf_tabs (f, 1);
      inf_fprintf (f, "~ \"%s\" ;\n", inf_reVectName ((char *)l->DATA));
    }
  }
  inf_fprintf (f, "End;\n");
  inf_leaveLines (f, 1);
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveBypass                                          */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the BYPASS section of the INF file from the       */
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_driveBypass (FILE * f, inffig_list * ifl)
{
  chain_list *cl, *list0;
  char *name, *val, *code;

  list0 = inf_GetEntriesByType (ifl, INF_BYPASS, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "Bypass\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_GetString (ifl, name, INF_BYPASS, &val);
      if (strcmp (val, INF_ALL) == 0)
        code = "";
      else if (strcmp (val, INF_IN) == 0)
        code = " < ";
      else if (strcmp (val, INF_OUT) == 0)
        code = " > ";
      else if (strcmp (val, INF_ONLYEND) == 0)
        code = " ! ";
      else
        code = " ? ";

      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\"%s;\n", inf_reVectName (name), code);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveSigSimple                                       */
/*          ARG(1) : file descriptor.                                         */
/*          ARG(2) : INF TYPE.                                                */
/*          ARG(3) : name of the section to drive.                            */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the corresponding section of the INF file from the*/
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_driveSigSimple (FILE * f, long type, char *section, INF_INFOSET * is)
{
  list_list *l;

  inf_fprintf (f, "%s\nBegin\n", section);
  for (l = is->INF_SIGLIST; l; l = l->NEXT) {
    if (l->TYPE == type) {
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\" ;\n", inf_reVectName ((char *)l->DATA));
    }
  }
  inf_fprintf (f, "End;\n");
  inf_leaveLines (f, 1);
}

/******************************************************************************/
/*   FUNCTION NAME : inf_drivePathsigs                                        */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the PATHSIGS section of the INF file from the     */
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_drivePathsigs (FILE * f, INF_INFOSET * is)
{
  list_list *l;
  ptype_list *p;

  inf_fprintf (f, "PathSigs\nBegin\n");
  for (l = is->INF_SIGLIST; l; l = l->NEXT) {
    if (l->TYPE == INF_LL_PATHSIGS) {
      inf_tabs (f, 1);
      if (l->USER == NULL)
        inf_fprintf (f, "\"%s\" ;\n", inf_reVectName ((char *)l->DATA));
      else {
        p = (ptype_list *) l->USER;
        if (p->TYPE == INF_UP)
          inf_fprintf (f, "\"%s\" < UP >;\n", inf_reVectName ((char *)l->DATA));
        else if (p->TYPE == INF_DOWN)
          inf_fprintf (f, "\"%s\" < DOWN >;\n", inf_reVectName ((char *)l->DATA));
      }
    }
  }
  inf_fprintf (f, "End;\n");
  inf_leaveLines (f, 1);
}

/******************************************************************************/
/*   FUNCTION NAME : inf_getRange                                             */
/*          ARG(1) : inf_carac structure with values.                         */
/*          RETURN : string with range values.                                */
/*  FUNCTIONNALITY : Check if initially, values was enumerated or specified   */
/*                   with bound in the INF file.                              */
/******************************************************************************/
char *inf_getRange (chain_list * cl, char *s, float factor)
{
  double step;
  int i;
  char bound = 'Y';
  char val[50];
  double *VALUES;
  int NVALUES;

  NVALUES = countchain (cl);
  VALUES = mbkalloc (sizeof (double) * NVALUES);
  for (i = 0; i < NVALUES; i++, cl = cl->NEXT)
    VALUES[i] = ((inf_assoc *) cl->DATA)->dval;

  step = VALUES[1] - VALUES[0];
  for (i = 2; bound == 'Y' && i < NVALUES; i++) {
    if (fabs (step - (VALUES[i] - VALUES[i - 1])) > 1e-18)
      bound = 'N';
  }

  if (bound == 'Y') {            /*str = "(low : upper : stp)" */
    sprintf (s, "(%g : %g : %g)", VALUES[0] * factor, VALUES[NVALUES - 1] * factor, step * factor);
  }
  else {                        /* str = "(val1, val2,..., valn)" */
    sprintf (s, "(%g", VALUES[0] * factor);
    for (i = 1; i < NVALUES; i++) {
      strcat (s, ", ");
      sprintf (val, "%g", VALUES[i] * factor);
      strcat (s, val);
    }
    strcat (s, ")");
  }
  mbkfree (VALUES);
  return s;
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveSlopein                                         */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the SLOPEIN section of the INF file from the      */
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_driveSlopein (FILE * f, inffig_list * ifl)
{
  char buf[2048];
  chain_list *list0, *cl, *ch;
  char *name;

  list0 = inf_GetEntriesByType (ifl, INF_SLOPEIN, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "SlopeIN\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_tabs (f, 1);
      inf_GetPointer (ifl, name, INF_SLOPEIN, (void **)&ch);
      inf_getRange (ch, buf, 1e12);
      inf_fprintf (f, "\"%s\" : %s ;\n", inf_reVectName (name), buf);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveCapaout                                         */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the CAPAOUT section of the INF file from the      */
/*                   INF_SIGLIST list.                                        */
/******************************************************************************/
void inf_driveCapaout (FILE * f, inffig_list * ifl)
{
  char buf[2048], *name, wl[128];
  double lw, l;
  chain_list *list0, *cl, *ch;

  list0 = inf_GetEntriesByType (ifl, INF_CAPAOUT, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "CapaOut\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_tabs (f, 1);
      inf_GetPointer (ifl, name, INF_CAPAOUT, (void **)&ch);
      if (inf_GetDouble (ifl, name, INF_CAPAOUT_LW, &lw)) {
        inf_GetDouble (ifl, name, INF_CAPAOUT_L, &l);
        sprintf (wl, "L=%g W=%g", l, (l / lw));
      }
      else
        strcpy (wl, "");

      inf_getRange (ch, buf, 1e15);
      inf_fprintf (f, "\"%s\" : %s %s;\n", inf_reVectName (name), buf, wl);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

void inf_driveOutputCapacitance (FILE * f, inffig_list * ifl)
{
  char *name;
  double c;
  chain_list *list0, *cl;

  list0 = inf_GetEntriesByType (ifl, INF_OUTPUT_CAPACITANCE, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "Output Capacitance\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_tabs (f, 1);
      inf_GetDouble (ifl, name, INF_OUTPUT_CAPACITANCE, &c);
      inf_fprintf (f, "\"%s\" : %g;\n", inf_reVectName (name), c*1e15);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveFalsepath                                       */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the FALSEPATH section of the INF file from the    */
/*                   INF_FALSEPATH list.                                      */
/******************************************************************************/
void inf_driveFalsepath (FILE * f, INF_INFOSET * is)
{
  chain_list *ch;
  ptype_list *p;
  char *name;

  inf_fprintf (f, "FalsePath\nBegin\n");
  for (ch = is->INF_FALSEPATH; ch; ch = ch->NEXT) {
    inf_tabs (f, 1);
    for (p = (ptype_list *) ch->DATA; p; p = p->NEXT) {
      name=(char *)p->DATA;
      switch (p->TYPE) {
      case INF_UPDOWN:
        inf_fprintf (f, "\"%s\"", inf_reVectName (name));
        break;
      case INF_UP:
        inf_fprintf (f, "\"%s\" < UP >", inf_reVectName (name));
        break;
      case INF_DOWN:
        inf_fprintf (f, "\"%s\" < DOWN >", inf_reVectName (name));
        break;
      case INF_NOTHING:
        inf_fprintf (f, "< VOID >");
        break;
      case INF_CK:
        if (name[0]=='^')
          inf_fprintf (f, ": < HZ >");
        else if (name[0]=='°')
          inf_fprintf (f, ": < NOTHZ >");
        else
          inf_fprintf (f, ": \"%s\"", inf_reVectName (name));
        break;
      }
      inf_spaces (f, 1);
    }
    inf_fprintf (f, ";\n");
  }
  inf_fprintf (f, "End;\n");
  inf_leaveLines (f, 1);
}

void inf_driveFalseslack (FILE * f, INF_INFOSET * is)
{
  chain_list *ch;
  ptype_list *p;
  int type;
  char *prev="";

  inf_fprintf (f, "FalseSlack\nBegin\n");
  for (ch = is->INF_FALSESLACK; ch; ch = ch->NEXT) {
    inf_tabs (f, 1);
    type=((ptype_list *) ch->DATA)->TYPE & (INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD|INF_FALSESLACK_LATCH);
    if (type & INF_FALSESLACK_LATCH) { inf_fprintf (f, "LATCH"); prev="-"; }
    if (type & INF_FALSESLACK_PRECH) { inf_fprintf (f, "%sPRECH", prev); prev="-"; }
    if ((type & (INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD))!=(INF_FALSESLACK_SETUP|INF_FALSESLACK_HOLD))
    {
      if (type & INF_FALSESLACK_SETUP) inf_fprintf (f, "%sSETUP", prev);
      else inf_fprintf (f, "%sHOLD", prev);
      prev="-";
    }
    if (strlen(prev)!=0) inf_fprintf (f, " :");

    for (p = (ptype_list *) ch->DATA; p; p = p->NEXT) {
      inf_fprintf (f, " \"%s\"", inf_reVectName ((char *)p->DATA));
      if ((p->TYPE & (INF_FALSESLACK_UP|INF_FALSESLACK_DOWN))!=(INF_FALSESLACK_UP|INF_FALSESLACK_DOWN))
      {
        if (p->TYPE & INF_FALSESLACK_UP) inf_fprintf (f, " < UP >");
        else inf_fprintf (f, " < DOWN >");
      }
      if ((p->TYPE & (INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ))!=(INF_FALSESLACK_HZ|INF_FALSESLACK_NOTHZ))
      {
        if (p->TYPE & INF_FALSESLACK_HZ) inf_fprintf (f, " < HZ >");
        else if (p->TYPE & INF_FALSESLACK_NOTHZ) inf_fprintf (f, " < NOTHZ >");
      }
    }
    inf_fprintf (f, ";\n");
  }
  inf_fprintf (f, "End;\n");
  inf_leaveLines (f, 1);
}

void inf_driveProba (FILE * f, inffig_list * ifl)
{
  char *name;
  double val;
  chain_list *list0, *cl;

  list0 = inf_GetEntriesByType (ifl, INF_SWITCHING_PROBABILITY, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "SwitchingProbability\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_GetDouble (ifl, name, INF_SWITCHING_PROBABILITY, &val);
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\" : %g;\n", inf_reVectName (name), val);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveDelay                                           */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the DELAY section of the INF file from the        */
/*                   INF_DELAY list.                                          */
/******************************************************************************/
void inf_driveDelay (FILE * f, inffig_list * ifl)
{
  chain_list *delay, *ch;
  inf_assoc *assoc;

  if (inf_GetPointer (ifl, INF_DELAY, "", (void **)&delay)) {
    inf_fprintf (f, "Delay\nBegin\n");
    for (ch = delay; delay; delay = delay->NEXT) {
      inf_tabs (f, 1);
      assoc = (inf_assoc *) delay->DATA;
      inf_fprintf (f, "\"%s\" : \"%s\" : %.1fps ;\n", inf_reVectName (assoc->orig), inf_reVectName (assoc->dest),
               assoc->dval * 1e12);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveDlatch                                          */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives the DLATCH section of the INF file from the       */
/*                   INF_DLATCH and INF_NOTDLATCH lists.                      */
/******************************************************************************/
void inf_driveDlatch (FILE * f, inffig_list * ifl)
{
  chain_list *ch;
  chain_list *list0, *list1;

  list0 = inf_GetEntriesByType (ifl, INF_DLATCH, INF_YES);
  list1 = inf_GetEntriesByType (ifl, INF_DLATCH, INF_NO);

  if (list0 || list1) {
    inf_fprintf (f, "Dlatch\nBegin\n");
    for (ch = list1; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_fprintf (f, "~ \"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
    }

    for (ch = list0; ch; ch = ch->NEXT) {
      inf_tabs (f, 1);
      inf_fprintf (f, "  \"%s\" ;\n", inf_reVectName ((char *)ch->DATA));
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain(list0);
    freechain(list1);
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_driveSiglist                                         */
/*          ARG(1) : file descriptor.                                         */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY :                                        */
/******************************************************************************/
void inf_driveSiglist (FILE * f, INF_INFOSET * is)
{
  list_list *l;
  int inf_driven[INF_NB_SIGTYPE];
  int i;


  for (i = 0; i < INF_NB_SIGTYPE; i++)
    inf_driven[i] = 0;

  for (l = is->INF_SIGLIST; l; l = l->NEXT) {
    switch (l->TYPE) {
    case INF_LL_PATHSIGS:
      if (inf_driven[INF_LL_PATHSIGS] == 0) {
        inf_drivePathsigs (f, is);
        inf_driven[INF_LL_PATHSIGS] = 1;
      }
      break;
    default:
      is->INF_SIGLIST = is->INF_SIGLIST->NEXT;
      break;
    }
  }
}

void infDriveStbDefault (FILE * f, inffig_list * myfig)
{
  double val = -1;
  if (inf_GetDouble (myfig, INF_STB_HEADER, INF_DEFAULT_PERIOD, &val))
    inf_fprintf (f, "Period %.1f;\n", val * 1e12);
  if (inf_GetDouble (myfig, INF_STB_HEADER, INF_SETUPMARGIN, &val))
    inf_fprintf (f, "SetupTime %.1f;\n", val * 1e12);
  if (inf_GetDouble (myfig, INF_STB_HEADER, INF_HOLDMARGIN, &val))
    inf_fprintf (f, "HoldTime %.1f;\n", val * 1e12);
  if (val != -1)
    inf_fprintf (f, "\n");
}


void infDriveLatencies (FILE * f, inffig_list * ifl)        
{
  chain_list *list, *chainx;
  double val1=0, val2=0, val3=0, val4=0;
  char *name;
  // clock
  list          = inf_GetEntriesByType(ifl,INF_LATENCY_FALL_MIN,INF_ANY_VALUES);
  list=inf_mergeclist(list, inf_GetEntriesByType(ifl,INF_LATENCY_FALL_MAX,INF_ANY_VALUES));
  list=inf_mergeclist(list, inf_GetEntriesByType(ifl,INF_LATENCY_RISE_MIN,INF_ANY_VALUES));
  list=inf_mergeclist(list, inf_GetEntriesByType(ifl,INF_LATENCY_RISE_MAX,INF_ANY_VALUES));
  if (list!=NULL)
  {
     inf_fprintf (f, "ClockLatencies\nBegin\n");
     for (chainx = list; chainx; chainx = delchain(chainx,chainx))
     {
       val1=0, val2=0, val3=0, val4=0;
       name=(char *)chainx->DATA;
       inf_GetDouble(ifl, name, INF_LATENCY_FALL_MIN, &val3);
       inf_GetDouble(ifl, name, INF_LATENCY_FALL_MAX, &val4);
       inf_GetDouble(ifl, name, INF_LATENCY_RISE_MIN, &val1);
       inf_GetDouble(ifl, name, INF_LATENCY_RISE_MAX, &val2);
       inf_tabs (f, 1);
       inf_fprintf (f, "\"%s\" : %.1f  %.1f  %.1f  %.1f;\n", inf_reVectName (name), val1*1e12, val2*1e12, val3*1e12, val4*1e12);

     }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

void infDriveStbClocks (FILE * f, inffig_list * myfig)
{
  chain_list *lst0, *cl;
  char *name, *inverted, *virt, *ideal, *master;
  int edges;
  double minr, maxr, minf, maxf, per;
  int strip, val;
  lst0 = inf_GetEntriesByType (myfig, INF_CLOCK_TYPE, INF_ANY_VALUES);
  if (lst0) {
    inf_fprintf (f, "Clock Connectors\nBegin\n");
    for (cl = lst0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inverted = virt = ideal = "";
      if (inf_GetInt (myfig, name, INF_CLOCK_TYPE, &val)) {
        if ((val & INF_CLOCK_INVERTED) != 0)
          inverted = "! ";
        if ((val & INF_CLOCK_VIRTUAL) != 0)
          virt = "Virtual ";
        if ((val & INF_CLOCK_IDEAL) != 0)
          ideal = "Ideal ";
      }

      if (!inf_GetDouble (myfig, name, INF_MIN_RISE_TIME, &minr)
          || !inf_GetDouble (myfig, name, INF_MAX_RISE_TIME, &maxr)
          || !inf_GetDouble (myfig, name, INF_MIN_FALL_TIME, &minf)
          || !inf_GetDouble (myfig, name, INF_MAX_FALL_TIME, &maxf))
        strip = 1;
      else
        strip = 0;
      if (!inf_GetDouble (myfig, name, INF_CLOCK_PERIOD, &per))
        per = -1;
      inf_tabs (f, 1);
      if (!strip) {
        inf_fprintf (f, "%s%s%s\"%s\" : Up", virt, ideal, inverted, inf_reVectName (name));
        if (fabs (minr - maxr) > 1e-17)
          inf_fprintf (f, " (%.1f:%.1f); Down", minr * 1e12, maxr * 1e12);
        else
          inf_fprintf (f, " %.1f; Down", minr * 1e12);
        if (fabs (minf - maxf) > 1e-17)
          inf_fprintf (f, " (%.1f:%.1f);", minf * 1e12, maxf * 1e12);
        else
          inf_fprintf (f, " %.1f;", minf * 1e12);
        if (per != -1)
          inf_fprintf (f, " Period %.1f;", per * 1e12);

        if (inf_GetString (myfig, name, INF_MASTER_CLOCK, &master) && master!=NULL)
        {
           if (!inf_GetInt (myfig, name, INF_MASTER_CLOCK_EDGES, &edges)) edges=0;
           inf_fprintf (f, " Clock \"%s\" %s %s ;", inf_reVectName (master), (edges & 2)?"UP":"DOWN", (edges & 1)?"UP":"DOWN");
        }
      }
      else
        inf_fprintf (f, "%s%s%s\"%s\";", virt, ideal, inverted, inf_reVectName (name));
      inf_fprintf (f, "\n");
    }
    inf_fprintf (f, "End;\n\n");
  }
  freechain (lst0);
}

static char *__getedge (char val)
{
  if (val == INF_STB_RISING)
    return " Rising";
  if (val == INF_STB_FALLING)
    return " Falling";
  if (val == INF_STB_SLOPEALL)
    return "";
  return "?";
}

static char *__getstab (char val)
{
  if (val == INF_STB_STABLE)
    return "Stable";
  if (val == INF_STB_UNSTABLE)
    return "Unstable";
  return "?";
}

static char *__getrel (char val)
{
  if (val == INF_STB_BEFORE)
    return "Before";
  if (val == INF_STB_AFTER)
    return "After";
  return "?";
}

static char *__gethz (char val)
{
  if (val == INF_STB_NOTHING)
    return "";
  if (val == INF_STB_HZ_NO_PRECH)
    return " Without Precharge";
  if (val == INF_STB_HZ_NO_PRECH)
    return " Without Evaluate";
  return "?";
}

void infDriveStbSpec (FILE * f, inf_stb_p_s * isps, char *signame)
{
  inf_stb_p_s_stab *stab;
  char *rel;

  while (isps) {
    inf_tabs (f, 1);
    inf_fprintf (f, "\"%s\"%s", inf_reVectName (signame), __getedge (isps->DATAEDGE));
    if (isps->SPECTYPE == INF_STB_SPECOUT)
      rel = "For";
    else
      rel = "From";

    if (isps->CKNAME)
      inf_fprintf (f, " %s \"%s\" %s", rel, inf_reVectName (isps->CKNAME), __getedge (isps->CKEDGE));
    inf_fprintf (f, "%s:\n", __gethz (isps->HZOPTION));

    for (stab = isps->STABILITY; stab; stab = stab->NEXT) {
      inf_tabs (f, 2);
      if (stab->CKNAME == NULL) {
        chain_list *cl;
        inf_fprintf (f, "%s", __getstab (stab->STABILITY));
        for (cl = stab->TIME_LIST; cl; cl = cl->NEXT)
          inf_fprintf (f, " %.1f", *(double *)cl->DATA * 1e12);
        inf_fprintf (f, ";\n");
      }
      else
        inf_fprintf (f, "%s %.1f %s \"%s\" %s;\n",
                 __getstab (stab->STABILITY),
                 stab->DELAY * 1e12,
                 __getrel (stab->RELATIVITY),
                 stab->CKNAME ? inf_reVectName (stab->CKNAME) : "?", __getedge (stab->CKEDGE));
    }
    isps = isps->NEXT;
  }
}

void infDriveStbSpecSection (FILE * f, inffig_list * myfig, char *section, char *header)
{
  chain_list *lst0, *cl;
  char *name;
  inf_stb_p_s *head;

  lst0 = inf_GetEntriesByType (myfig, section, INF_ANY_VALUES);
  if (lst0) {
    inf_fprintf (f, "%s\nBegin\n", header);
    for (cl = lst0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      if (inf_GetPointer (myfig, name, section, (void **)&head)) {
        infDriveStbSpec (f, head, name);
      }
    }

    inf_fprintf (f, "End;\n\n");
    freechain(lst0);
  }
}

void infDriveStbGroups (FILE * f, inffig_list * myfig, char *header, char *section, char *periodsection)
{
  chain_list *list, *chainx, *namelist;
  char *domainname;
  double val;
  list = inf_GetValuesByType (myfig, section);
  if (list) {
    inf_fprintf (f, "%s\nBegin\n", header);
    for (chainx = list; chainx; chainx = delchain (chainx, chainx)) {
      domainname = (char *)chainx->DATA;
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\":", domainname);
      for (namelist = inf_GetEntriesByType (myfig, section, domainname);
           namelist; namelist = delchain (namelist, namelist)) {
        inf_fprintf (f, " \"%s\"", inf_reVectName ((char *)namelist->DATA));
        if (namelist->NEXT)
          inf_fprintf (f, ",");
      }
      inf_fprintf (f, ";");
      if (periodsection && inf_GetDouble (myfig, periodsection, domainname, &val))
        inf_fprintf (f, " Period %.1f;", val * 1e12);
      inf_fprintf (f, "\n");
    }
    inf_fprintf (f, "End;\n\n");
  }
}

void infDriveStbPriorityClock (FILE * f, inffig_list * myfig)
{
  // priority clock
  char *def;
  chain_list *chainx, *list;

  list = inf_GetEntriesByType (myfig, INF_PREFERED_CLOCK, INF_ANY_VALUES);
  if (list) {
    inf_fprintf (f, "Multiple Clock Priority\nBegin\n");
    for (chainx = list, list = NULL; chainx; chainx = delchain (chainx, chainx)) {
      inf_GetString (myfig, (char *)chainx->DATA, INF_PREFERED_CLOCK, &def);
      inf_tabs (f, 1);
      inf_fprintf (f, " \"%s\" : \"%s\";\n", inf_reVectName ((char *)chainx->DATA), inf_reVectName (def));
    }
    inf_fprintf (f, "End;\n\n");
  }
}

void infDriveStbDisable (FILE * f, inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  if (inf_GetPointer (myfig, INF_DISABLE_PATH, "", (void **)&list)) {
    inf_fprintf (f, "Disable\nBegin\n");
    for (chainx = list; chainx; chainx = chainx->NEXT) {
      assoc = (inf_assoc *) chainx->DATA;
      inf_tabs (f, 1);
      if (assoc->orig)
        inf_fprintf (f, "From \"%s\"", inf_reVectName (assoc->orig));
      if (assoc->dest)
        inf_fprintf (f, "%sTo \"%s\"", assoc->orig ? " " : "", inf_reVectName (assoc->dest));
      inf_fprintf (f, ";\n");
    }
    inf_fprintf (f, "End;\n\n");
  }
}

void infDriveStbCommandState (FILE * f, inffig_list * myfig)
{
  chain_list *chainx, *list;
  char *name, *def, *state;
  list = inf_GetEntriesByType (myfig, INF_VERIF_STATE, INF_ANY_VALUES);
  if (list) {
    inf_fprintf (f, "Conditioned Command States\nBegin\n");
    for (chainx = list; chainx; chainx = delchain (chainx, chainx)) {
      name = (char *)chainx->DATA;
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\" : ", inf_reVectName (name));
      inf_GetString (myfig, name, INF_VERIF_STATE, &def);
      if (strcmp (def, INF_VERIFUP) == 0)
        state = "Up";
      else if (strcmp (def, INF_VERIFDOWN) == 0)
        state = "Down";
      else if (strcmp (def, INF_VERIFRISE) == 0)
        state = "Rising";
      else if (strcmp (def, INF_VERIFFALL) == 0)
        state = "Falling";
      else if (strcmp (def, INF_NOVERIF) == 0)
        state = "NoVerif";
      else {
        inf_fprintf (f, "?");
        continue;
      }

      inf_fprintf (f, "%s;\n", state);
    }
    inf_fprintf (f, "End;\n\n");
  }
}

void infDriveDirectives (FILE * f, inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  splitint *si;
  int val;

  if (inf_GetPointer (myfig, INF_DIRECTIVES, "", (void **)&list)) {
    inf_fprintf (f, "Directives\nBegin\n");
    for (chainx = list; chainx; chainx = chainx->NEXT) {
      assoc = (inf_assoc *) chainx->DATA;
      val=assoc->lval;
      si=(splitint *)&val;
      
      inf_tabs (f, 1);
      if (si->cval.a & INF_DIRECTIVE_FILTER)
        inf_fprintf (f, "Filter: ");
      else
        inf_fprintf (f, "Check: ");

      
      if (si->cval.b & INF_DIRECTIVE_CLOCK) inf_fprintf (f, "Clock ");
      inf_fprintf (f, "\"%s\" ", inf_reVectName (assoc->orig));
      if (si->cval.b & INF_DIRECTIVE_UP) inf_fprintf (f, "Up ");
      else if (si->cval.b & INF_DIRECTIVE_DOWN) inf_fprintf (f, "Down ");
      else if (si->cval.b & INF_DIRECTIVE_RISING) inf_fprintf (f, "Rising ");
      else if (si->cval.b & INF_DIRECTIVE_FALLING) inf_fprintf (f, "Falling ");

      if (si->cval.c & INF_DIRECTIVE_BEFORE) inf_fprintf (f, "Before ");
      else if (si->cval.c & INF_DIRECTIVE_AFTER) inf_fprintf (f, "After ");
      else inf_fprintf (f, "With ");
      
      if ((si->cval.d & INF_DIRECTIVE_DELAY)==0)
      {
        if (si->cval.d & INF_DIRECTIVE_CLOCK) inf_fprintf (f, "Clock ");
        inf_fprintf (f, "\"%s\" ", inf_reVectName (assoc->dest));

        if (assoc->dval!=0) inf_fprintf (f, "Margin %.1f", assoc->dval* 1e12);
      }
      else
      {
        inf_fprintf (f, "Delay %.1f From \"%s\" ", assoc->dval* 1e12, assoc->dest);
      }
      if (si->cval.d & INF_DIRECTIVE_UP) inf_fprintf (f, "Up ");
      else if (si->cval.d & INF_DIRECTIVE_DOWN) inf_fprintf (f, "Down ");
      else if (si->cval.d & INF_DIRECTIVE_RISING) inf_fprintf (f, "Rising ");
      else if (si->cval.d & INF_DIRECTIVE_FALLING) inf_fprintf (f, "Falling ");
      
      inf_fprintf (f, ";\n");
    }
    inf_fprintf (f, "End;\n\n");
  }
}


void infDriveStb (FILE * f, inffig_list * myfig)
{
//  inf_fprintf (f, "#-=> parameters\n\n");
  infDriveStbDefault (f, myfig);
//  inf_fprintf (f, "#-=> clock definitions\n\n");
  infDriveStbClocks (f, myfig);
  infDriveLatencies (f, myfig);
//  inf_fprintf (f, "#-=> asynchronous clock groups\n\n");
  infDriveStbGroups (f, myfig, "Asynchronous Clock Groups", INF_ASYNC_CLOCK_GROUP, INF_ASYNC_CLOCK_GROUP_PERIOD);
//  inf_fprintf (f, "#-=> equivalent clock groups\n\n");
  infDriveStbGroups (f, myfig, "Equivalent Clock Groups", INF_EQUIV_CLOCK_GROUP, NULL);
//  inf_fprintf (f, "#-=> prefered clock\n\n");
  infDriveStbPriorityClock (f, myfig);
//  inf_fprintf (f, "#-=> command state\n\n");
  infDriveStbCommandState (f, myfig);
//  inf_fprintf (f, "#-=> disabled paths\n\n");
  infDriveStbDisable (f, myfig);
//  inf_fprintf (f, "#-=> input specifications\n\n");
  infDriveStbSpecSection (f, myfig, INF_SPECIN, "Specify Input Connectors");
//  inf_fprintf (f, "#-=> output specifications\n\n");
  infDriveStbSpecSection (f, myfig, INF_SPECOUT, "Verify Output Connectors");
//  inf_fprintf (f, "#-=> input connector stability\n\n");
  infDriveStbSpecSection (f, myfig, INF_STBOUT_SPECIN, "Input Connectors Stability");
//  inf_fprintf (f, "#-=> output connector stability\n\n");
  infDriveStbSpecSection (f, myfig, INF_STBOUT_SPECOUT, "Output Connectors Stability");
//  inf_fprintf (f, "#-=> memory stability\n\n");
  infDriveStbSpecSection (f, myfig, INF_STBOUT_SPECMEM, "Memory Nodes Stability");
//  inf_fprintf (f, "#-=> node stability\n\n");
  infDriveStbSpecSection (f, myfig, INF_STBOUT_SPECINODE, "Internal Nodes Stability");
}


void infDriveHeader (FILE * f, inffig_list * ifl)
{
  inf_fprintf (f, "Name \"%s\";\n\n", ifl->NAME);
}

void infDriveOperatingCondition (FILE * f, inffig_list * ifl)
{
  int hastemp;
  double val;
  chain_list *cl, *list0;
  char *name;

  list0 = inf_GetEntriesByType (ifl, INF_POWER, INF_ANY_VALUES);
  hastemp = inf_GetDouble (ifl, INF_OPERATING_CONDITION, INF_TEMPERATURE, &val);
  if (hastemp || list0) {
    inf_fprintf (f, "Operating Conditions\nBegin\n");
    if (hastemp) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Temp = %g;\n", val);
    }
    for (cl = list0; cl; cl = cl->NEXT) {
      inf_tabs (f, 1);
      name = (char *)cl->DATA;
      inf_GetDouble (ifl, name, INF_POWER, &val);
      inf_fprintf (f, "Supply : \"%s\" = %g;\n", inf_reVectName (name), val);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
  freechain(list0);
}


static char *getdelaynodetype (int val)
{
  if ((val & INF_MARGIN_ON_ALL) == INF_MARGIN_ON_ALL)
    return "Any";
  if (val & INF_MARGIN_ON_LATCH)
    return "Latch";
  if (val & INF_MARGIN_ON_FLIPFLOP)
    return "FlipFlop";
  if (val & INF_MARGIN_ON_BREAK)
    return "Break";
  if (val & INF_MARGIN_ON_CONNECTOR)
    return "Con";
  if (val & INF_MARGIN_ON_PRECHARGE)
    return "Prech";
  if (val & INF_MARGIN_ON_CMD)
    return "Cmd";
  return "?";
}


static void __infDrivePathDelayMargin (FILE * f, inffig_list * myfig, char *name)
{
  // priority clock
  inf_miscdata *imd0;
  int i, j, k;
  struct {
    char *o;
    char *s;
  } run[] = {
    {
      "Min", INF_PATHDELAYMARGINMIN}, {
        "Max", INF_PATHDELAYMARGINMAX}, {
          "ClockPath", INF_PATHDELAYMARGINCLOCK}, {
            "DataPath", INF_PATHDELAYMARGINDATA}, {
              "Rise", INF_PATHDELAYMARGINRISE}, {
                "Fall", INF_PATHDELAYMARGINFALL}
  };
  char section[128];

  for (i = 0; i < 2; i++)
    for (j = 2; j < 4; j++)
      for (k = 4; k < 6; k++) {
        sprintf (section, INF_PATHDELAYMARGINPREFIX "|%s,%s,%s", run[i].s, run[j].s, run[k].s);
        if (inf_GetPointer (myfig, name, section, (void **)&imd0)) {
          inf_tabs (f, 1);
          inf_fprintf (f, "%s \"%s\" : Factor=%g Delta=%g %s %s %s;\n", getdelaynodetype (imd0->lval), name,
                   imd0->dval, imd0->dval1 * 1e12, run[i].o, run[j].o, run[k].o);
        }
      }


#if 0
  char *def;
  chain_list *chainx, *list;
  r inf_miscdata *imd0, *imd1, *imd2, *imd3;
  int has0, has1, has2, has3;

  has0 = inf_GetPointer (myfig, name, INF_PATHDELAYMARGINCLOCKMIN, (void **)&imd0);
  has1 = inf_GetPointer (myfig, name, INF_PATHDELAYMARGINDATAMIN, (void **)&imd1);
  has2 = inf_GetPointer (myfig, name, INF_PATHDELAYMARGINCLOCKMAX, (void **)&imd2);
  has3 = inf_GetPointer (myfig, name, INF_PATHDELAYMARGINDATAMAX, (void **)&imd3);

  if (has0 && has1 && has2 && has3 &&
      imd0->dval == imd1->dval && imd2->dval == imd3->dval && imd0->dval == imd2->dval &&
      imd0->dval1 == imd1->dval1 && imd2->dval1 == imd3->dval1 && imd0->dval1 == imd2->dval1) {
    inf_tabs (f, 1);
    inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld ;\n", getdelaynodetype (imd0->lval), name, imd0->dval,
             mbk_long_round (imd0->dval1 * 1e12));
  }
  else {
    if (has0 && has1 && imd0->dval == imd1->dval && imd0->dval1 == imd1->dval1) {
      inf_tabs (f, 1);
      inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld Min;\n", getdelaynodetype (imd0->lval), name, imd0->dval,
               mbk_long_round (imd0->dval1 * 1e12));
    }
    else {
      if (has0) {
        inf_tabs (f, 1);
        inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld ClockPath Min;\n", getdelaynodetype (imd0->lval), name,
                 imd0->dval, mbk_long_round (imd0->dval1 * 1e12));
      }
      if (has1) {
        inf_tabs (f, 1);
        inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld DataPath Min;\n", getdelaynodetype (imd1->lval), name,
                 imd1->dval, mbk_long_round (imd1->dval1 * 1e12));
      }

    }

    if (has2 && has3 && imd2->dval == imd3->dval && imd2->dval1 == imd3->dval1) {
      inf_tabs (f, 1);
      inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld Max;\n", getdelaynodetype (imd2->lval), name, imd2->dval,
               mbk_long_round (imd2->dval1 * 1e12));
    }
    else {
      if (has2) {
        inf_tabs (f, 1);
        inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld ClockPath Max;\n", getdelaynodetype (imd2->lval), name,
                 imd2->dval, mbk_long_round (imd2->dval1 * 1e12));
      }
      if (has3) {
        inf_tabs (f, 1);
        inf_fprintf (f, "%s \"%s\" Factor=%g Delta=%ld DataPath Max;\n", getdelaynodetype (imd3->lval), name,
                 imd3->dval, mbk_long_round (imd3->dval1 * 1e12));
      }
    }
  }
#endif
}

void infDrivePathDelayMargin (FILE * f, inffig_list * myfig)
{
  ht *tempht;
  chain_list *list, *cl;
  char *run[] = {
    INF_PATHDELAYMARGINMIN, INF_PATHDELAYMARGINMAX,
    INF_PATHDELAYMARGINCLOCK, INF_PATHDELAYMARGINDATA,
    INF_PATHDELAYMARGINRISE, INF_PATHDELAYMARGINFALL
  };
  int i, j, k;
  char section[128];

  tempht = addht (128);

  for (i = 0; i < 2; i++)
    for (j = 2; j < 4; j++)
      for (k = 4; k < 6; k++) {
        sprintf (section, INF_PATHDELAYMARGINPREFIX "|%s,%s,%s", run[i], run[j], run[k]);
        list = inf_GetEntriesByType (myfig, section, INF_ANY_VALUES);
        for (cl = list; cl; cl = cl->NEXT)
          addhtitem (tempht, cl->DATA, 0);
        freechain (list);
      }

  list = GetAllHTKeys (tempht);
  delht (tempht);

  if (list) {
    inf_fprintf (f, "Path Delay Margin\nBegin\n");
    for (cl = list; cl; cl = cl->NEXT) {
      __infDrivePathDelayMargin (f, myfig, (char *)cl->DATA);
    }
    inf_fprintf (f, "End;\n\n");
    freechain (list);
  }
}


void infDriveMulticyclePath (FILE * f, inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  if (inf_GetPointer (myfig, INF_MULTICYCLE_PATH, "", (void **)&list)) {
    inf_fprintf (f, "MultiCycle Path\nBegin\n");
    for (chainx = list; chainx; chainx = chainx->NEXT) {
      assoc = (inf_assoc *) chainx->DATA;
      inf_tabs (f, 1);
      if ((assoc->lval & (INF_MULTICYCLE_SETUP | INF_MULTICYCLE_HOLD)) !=
          (INF_MULTICYCLE_SETUP | INF_MULTICYCLE_HOLD)) {
        if (assoc->lval & INF_MULTICYCLE_SETUP)
          inf_fprintf (f, "For SETUP ");
        else
          inf_fprintf (f, "For HOLD ");
      }
      if (!
          (strcmp (assoc->dest, "*") == 0
           && (assoc->lval & (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL)) ==
           (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL))) {
        inf_fprintf (f, "\"%s\" ", inf_reVectName (assoc->dest));
        if ((assoc->lval & (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL)) !=
            (INF_MULTICYCLE_RISE | INF_MULTICYCLE_FALL)) {
          if (assoc->lval & INF_MULTICYCLE_RISE)
            inf_fprintf (f, "Rising ");
          else
            inf_fprintf (f, "Falling ");
        }
      }

      inf_fprintf (f, "After %g ", assoc->dval);
      if (assoc->lval & INF_MULTICYCLE_END)
        inf_fprintf (f, "End Cycles");
      else
        inf_fprintf (f, "Start Cycles");

      if (strcmp (assoc->orig, "*") != 0)
        inf_fprintf (f, " From \"%s\"", inf_reVectName (assoc->orig));

      inf_fprintf (f, ";\n");
    }
    inf_fprintf (f, "End;\n\n");
  }
}

void inf_driveIgnore (FILE * f, inffig_list * myfig)
{
  chain_list *mutex;
  chain_list *head0 = NULL, *head1 = NULL, *head2 = NULL, *head3 = NULL, *head4 = NULL, *head5 = NULL, *head6=NULL;

  inf_GetPointer (myfig, INF_IGNORE_INSTANCE, "", (void **)&head0);
  inf_GetPointer (myfig, INF_IGNORE_TRANSISTOR, "", (void **)&head1);
  inf_GetPointer (myfig, INF_IGNORE_RESISTANCE, "", (void **)&head2);
  inf_GetPointer (myfig, INF_IGNORE_CAPACITANCE, "", (void **)&head3);
  inf_GetPointer (myfig, INF_IGNORE_DIODE, "", (void **)&head6);
  inf_GetPointer (myfig, INF_IGNORE_PARASITICS, "", (void **)&head4);
  inf_GetPointer (myfig, INF_IGNORE_NAMES, "", (void **)&head5);

  if (head0 || head1 || head2 || head3 || head5 || head6) {
    inf_fprintf (f, "Ignore\nBegin\n");

    if (head0) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Instances:\n");
    }
    for (mutex = head0; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }

    if (head1) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Transistors:\n");
    }
    for (mutex = head1; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }

    if (head2) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Resistances:\n");
    }
    for (mutex = head2; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }

    if (head3) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Capacitances:\n");
    }
    for (mutex = head3; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }
    if (head6) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Diodes:\n");
    }
    for (mutex = head6; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }

    if (head4) {
      inf_tabs (f, 1);
      inf_fprintf (f, "Parasitics:\n");
    }
    for (mutex = head4; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }

    if (head5) {
      inf_tabs (f, 1);
      inf_fprintf (f, "SignalNames:\n");
    }
    for (mutex = head5; mutex; mutex = mutex->NEXT) {
      inf_tabs (f, 2);
      inf_fprintf (f, "\"%s\"%s\n", ((inf_assoc *) mutex->DATA)->orig, mutex->NEXT ? "," : ";");
    }

    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

void inf_driveConnectorDirections (FILE * f, inffig_list * myfig)
{
  chain_list *cl;
  int has = 0;
  unsigned int i;
  struct {
    char *txt;
    char *value;
    chain_list *head;
  } tab[] = {
    {
      "Input", "I", NULL}, {
        "Output", "O", NULL}, {
          "InOut", "B", NULL}, {
            "Tristate", "T", NULL}, {
              "HZ", "Z", NULL}, {
                "Unknown", "X", NULL}
  };

  for (i = 0; i < sizeof (tab) / sizeof (*tab); i++)
    if ((tab[i].head = inf_GetEntriesByType (myfig, INF_CONNECTOR_DIRECTION, tab[i].value)))
      has++;

  if (has) {
    inf_fprintf (f, "Connector Directions\nBegin\n");

    for (i = 0; i < sizeof (tab) / sizeof (*tab); i++) {
      if (tab[i].head) {
        inf_tabs (f, 1);
        inf_fprintf (f, "%s:\n", tab[i].txt);
      }
      for (cl = tab[i].head; cl; cl = cl->NEXT) {
        inf_tabs (f, 2);
        inf_fprintf (f, "\"%s\"%s\n", (char *)cl->DATA, cl->NEXT ? "," : ";");
      }
      freechain (tab[i].head);
    }

    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
  }
}

void infDriveDisableGateDelay (FILE * f, inffig_list * myfig)
{
  chain_list *list, *cl;
  inf_assoc *assoc;
  int dir1, dir2;

  list = inf_GetEntriesByType (myfig, INF_DISABLE_GATE_DELAY, INF_ANY_VALUES);

  if (list!=NULL)
    {
      inf_fprintf (f, "Disable Gate Delay\nBegin\n");
      while (list!=NULL)
        { 
          inf_GetPointer (myfig, list->DATA, INF_DISABLE_GATE_DELAY, (void **)&cl);
          while (cl!=NULL)
            {
              assoc = (inf_assoc *) cl->DATA;
              inf_tabs (f, 1);
              dir1=assoc->lval>>4;
              dir2=assoc->lval & 0x0f;

              inf_fprintf (f, "\"%s\" ", inf_reVectName (assoc->orig));              
              if (dir1!=3)
                {
                  if (dir1 & 0x1) inf_fprintf (f, "< DOWN > ");
                  else if (dir1 & 0x2) inf_fprintf (f, "< UP > ");
                }
              inf_fprintf (f, "\"%s\" ", inf_reVectName ((char *)list->DATA));              
              if (dir2!=3)
                {
                  if (dir2 & 0x1) inf_fprintf (f, "< DOWN > ");
                  else if (dir2 & 0x2) inf_fprintf (f, "< UP > ");
                }
              inf_fprintf (f, ";\n");
              cl=cl->NEXT;
            }
          list=delchain(list, list);
        }
      inf_fprintf (f, "End;\n\n");
    }
}

void inf_driveRSTYPE (FILE * f, inffig_list * ifl)
{
  chain_list *list0, *cl;
  int val;
  char *name, *type;

  list0 = inf_GetEntriesByType (ifl, INF_MARKRS, INF_ANY_VALUES);
  if (list0) {
    inf_fprintf (f, "RSType\nBegin\n");
    for (cl = list0; cl; cl = cl->NEXT) {
      name = (char *)cl->DATA;
      inf_GetInt (ifl, name, INF_MARKRS, &val);
      switch(val)
      {
        case INF_RS_LEGAL: type="Legal"; break;
        case INF_RS_ILLEGAL: type="Illegal"; break;
        case INF_RS_MARKONLY: type="Mark_Only"; break;
      }
      inf_tabs (f, 1);
      inf_fprintf (f, "\"%s\" : \"%s\" ;\n", inf_reVectName (name), type);
    }
    inf_fprintf (f, "End;\n");
    inf_leaveLines (f, 1);
    freechain (list0);
  }
}

void infDriveClockUncertainty (FILE * f, inffig_list * myfig)
{
  chain_list *chainx, *list;
  inf_assoc *assoc;
  if (inf_GetPointer (myfig, INF_CLOCK_UNCERTAINTY, "", (void **)&list)) {
    inf_fprintf (f, "ClockUncertainty\nBegin\n");
    for (chainx = list; chainx; chainx = chainx->NEXT) {
      assoc = (inf_assoc *) chainx->DATA;
      inf_tabs (f, 1);
      if ((assoc->lval & (INF_CLOCK_UNCERTAINTY_SETUP | INF_CLOCK_UNCERTAINTY_HOLD)) !=
          (INF_CLOCK_UNCERTAINTY_SETUP | INF_CLOCK_UNCERTAINTY_HOLD)) {
        if (assoc->lval & INF_CLOCK_UNCERTAINTY_SETUP)
          inf_fprintf (f, "For SETUP");
        else
          inf_fprintf (f, "For HOLD");
      }
      
      inf_fprintf (f, " From \"%s\"", inf_reVectName (assoc->orig));
      
      if ((assoc->lval & (INF_CLOCK_UNCERTAINTY_START_RISE | INF_CLOCK_UNCERTAINTY_START_FALL)) !=
          (INF_CLOCK_UNCERTAINTY_START_RISE | INF_CLOCK_UNCERTAINTY_START_FALL))
        {
          if (assoc->lval & INF_CLOCK_UNCERTAINTY_START_RISE)
            inf_fprintf (f, " Rising ");
          else
            inf_fprintf (f, " Falling ");
        }

      inf_fprintf (f, " To \"%s\"", inf_reVectName (assoc->dest));
      
      if ((assoc->lval & (INF_CLOCK_UNCERTAINTY_END_RISE | INF_CLOCK_UNCERTAINTY_END_FALL)) !=
          (INF_CLOCK_UNCERTAINTY_END_RISE | INF_CLOCK_UNCERTAINTY_END_FALL))
        {
          if (assoc->lval & INF_CLOCK_UNCERTAINTY_END_RISE)
            inf_fprintf (f, " Rising ");
          else
            inf_fprintf (f, " Falling ");
        }

      inf_fprintf (f, ": %g ", assoc->dval*1e12);
      inf_fprintf (f, ";\n");
    }
    inf_fprintf (f, "End;\n\n");
  }
}

/******************************************************************************/
/*   FUNCTION NAME : inf_drive                                                */
/*          ARG(1) : file name.                                               */
/*          RETURN : NONE.                                                    */
/*  FUNCTIONNALITY : Drives a inf file from inf lists.                        */
/******************************************************************************/
void infDrive_filtered (inffig_list * myfig, char *filename, int locations, FILE * outputfile, char *section)
{
  FILE *f;
  int oldmode;
  char *c, *tok;
  char buf[1024];
  int found=0;

  strcpy(buf, section);

  if (outputfile == NULL) {
    if (filename == NULL && myfig)
      filename = myfig->NAME;
    
    if (filename == NULL)
      return;
    
    f = mbkfopen (filename, NULL, WRITE_TEXT);
  }
  else
    f = outputfile;
  
  if (f) {
    if (myfig) {
      oldmode = inf_StuckSection (locations);
      
      if (outputfile == NULL)
        avt_printExecInfo (f, "#", "", "");
      
      inf_fprintf (f, "\n");
      
      infDriveHeader (f, myfig);

      tok=strtok_r(buf, " ", &c);
      while (tok!=NULL)
        {
          found=0;

          if (mbk_TestREGEX("OperatingCondition",tok))
          {found=1; infDriveOperatingCondition (f, myfig);}

          if (mbk_TestREGEX("PinSlew",tok))
            {found=1; inf_drivePinSlew (f, myfig);}

          if (mbk_TestREGEX("Rename",tok))
            {found=1; inf_driveRename (f, myfig);}

          if (mbk_TestREGEX("Memsym",tok))
            {found=1; inf_driveMemsym (f, myfig);}

          if (mbk_TestREGEX("Stop",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Stop", INF_STOP, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Sensitive",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Sensitive", INF_SENSITIVE, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Suppress",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Suppress", INF_SUPPRESS, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Inputs",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Inputs", INF_INPUTS, INF_ANY_VALUES);}

          if (mbk_TestREGEX("NotLatch",tok))
            {found=1; inf_driveSigListSection (f, myfig, "NotLatch", INF_NOTLATCH, INF_ANY_VALUES);}

          if (mbk_TestREGEX("KeepTristateBehaviour",tok))
            {found=1; inf_driveSigListSection (f, myfig, "KeepTristateBehaviour", INF_KEEP_TRISTATE_BEHAVIOUR, INF_ANY_VALUES);}

          if (mbk_TestREGEX("CkLatch",tok))
            {found=1; inf_driveBinary (f, myfig, "CkLatch", INF_CKLATCH);}

          if (mbk_TestREGEX("Ckprech",tok))
            {found=1; inf_driveCkprech (f, myfig);}

          if (mbk_TestREGEX("Precharge",tok))
            {found=1; inf_driveBinary (f, myfig, "Precharge", INF_PRECHARGE);}

          if (mbk_TestREGEX("ModelLoop",tok))
            {found=1; inf_driveBinary (f, myfig, "ModelLoop", INF_MODELLOOP);}

          if (mbk_TestREGEX("RSType",tok))
            {found=1; inf_driveRSTYPE(f, myfig);}

          if (mbk_TestREGEX("Dirout",tok))
            {found=1; inf_driveDirout (f, myfig);}

          if (mbk_TestREGEX("Mutex",tok))
            {found=1; inf_driveMutex (f, myfig);}

          if (mbk_TestREGEX("CrosstalkMutex",tok))
            {found=1; inf_driveCrosstalkMutex (f, myfig);}

          if (mbk_TestREGEX("Constraint",tok))
            {found=1; inf_driveConstraint (f, myfig);}

          if (mbk_TestREGEX("ConnectorDirections",tok))
            {found=1; inf_driveConnectorDirections (f, myfig);}
          //          inf_driveSigListSection (f, myfig, "Clock", INF_CLOCK_TYPE);

          if (mbk_TestREGEX("PathIN",tok))
            {found=1; inf_driveSigListSection (f, myfig, "PathIN", INF_PATHIN, INF_ANY_VALUES);}

          if (mbk_TestREGEX("PathOUT",tok))
            {found=1; inf_driveSigListSection (f, myfig, "PathOUT", INF_PATHOUT, INF_ANY_VALUES);}

          if (mbk_TestREGEX("PathDelayMargin",tok))
            {found=1; infDrivePathDelayMargin (f, myfig);}

          if (mbk_TestREGEX("MulticyclePath",tok))
            {found=1; infDriveMulticyclePath (f, myfig);}

          if (mbk_TestREGEX("ClockUncertainty",tok))
            {found=1; infDriveClockUncertainty (f, myfig);}

          if (mbk_TestREGEX("Ignore",tok))
            {found=1; inf_driveIgnore (f, myfig);}

          if (mbk_TestREGEX("NoCheck",tok))
            {found=1; inf_driveNoCheck (f, myfig);}

          if (mbk_TestREGEX("Bypass",tok))
            {found=1; inf_driveBypass (f, myfig);}

          if (mbk_TestREGEX("DisableGateDelay",tok))
            {found=1; infDriveDisableGateDelay (f, myfig);}
              
         if (mbk_TestREGEX("NoRising",tok))
            {found=1; inf_driveSigListSection (f, myfig, "NoRising", INF_NORISING, INF_ANY_VALUES);}

          if (mbk_TestREGEX("NoFalling",tok))
            {found=1; inf_driveSigListSection (f, myfig, "NoFalling", INF_NOFALLING, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Break",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Break", INF_BREAK, INF_ANY_VALUES);}

          if (mbk_TestREGEX("StrictSetup",tok))
            {found=1; inf_driveSigListSection (f, myfig, "StrictSetup", INF_STRICT_SETUP, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Inter",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Inter", INF_INTER, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Asynchron",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Asynchron", INF_ASYNCHRON, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Transparent",tok))
            {found=1; inf_driveSigListSection (f, myfig, "Transparent", INF_TRANSPARENT, INF_ANY_VALUES);}

          if (mbk_TestREGEX("DoNotCross",tok))
            {found=1; inf_driveSigListSection (f, myfig, "DoNotCross", INF_DONTCROSS, INF_ANY_VALUES);}

          if (mbk_TestREGEX("RC",tok))
            {found=1; inf_driveSigListSection (f, myfig, "RC", INF_RC, INF_YES);}

          if (mbk_TestREGEX("NORC",tok))
            {found=1; inf_driveSigListSection (f, myfig, "NORC", INF_RC, INF_NO);}

          if (mbk_TestREGEX("SIGLIST",tok))
            { found=1;
              if (myfig->LOADED.INF_SIGLIST)
              inf_driveSiglist (f, &myfig->LOADED);
            }
          
          if (mbk_TestREGEX("Falsepath",tok))
            { found=1;
              if (myfig->LOADED.INF_FALSEPATH)
              inf_driveFalsepath (f, &myfig->LOADED);
            }

          if (mbk_TestREGEX("Falseslack",tok))
            { found=1;
              if (myfig->LOADED.INF_FALSESLACK)
              inf_driveFalseslack (f, &myfig->LOADED);
            }        

          if (mbk_TestREGEX("Delay",tok))
            {found=1; inf_driveDelay (f, myfig);}

          if (mbk_TestREGEX("Dlatch",tok))
            {found=1; inf_driveDlatch (f, myfig);}

          if (mbk_TestREGEX("FlipFlop",tok))
            {found=1; inf_driveSigListSection (f, myfig, "FlipFlop", INF_FLIPFLOP, INF_ANY_VALUES);}

          if (mbk_TestREGEX("Slopein",tok))
            {found=1; inf_driveSlopein (f, myfig);}

          if (mbk_TestREGEX("Capaout",tok))
            {found=1; inf_driveCapaout (f, myfig);}

          if (mbk_TestREGEX("OutputCapacitance",tok))
            {found=1; inf_driveOutputCapacitance(f, myfig);}

          if (mbk_TestREGEX("Stuck",tok))
            {found=1; inf_StuckSection (oldmode);}

          if (mbk_TestREGEX("Directives",tok))
            {found=1; infDriveDirectives(f, myfig);}

          if (mbk_TestREGEX("SwitchingProbability",tok))
            {found=1; inf_driveProba (f, myfig);}

          // stb informations
          if (mbk_TestREGEX("Stb",tok))
            {
              found=1;
              inf_fprintf (f, "\n# -=> Stability Informations <=-\n\n");
              infDriveStb (f, myfig);
            }
          if (!found) avt_errmsg (INF_ERRMSG, "021", AVT_ERR, tok);

          tok=strtok_r(NULL, " ", &c);
        }
    }
    if (outputfile == NULL)
      fclose (f);
  }
  else
    avt_errmsg (INF_ERRMSG, "018", AVT_ERR, filename);
  // avt_error("inf", 3, AVT_ERR, "could not create file '%s.inf'\n", filename);
}

void infDrive (inffig_list * myfig, char *filename, int locations, FILE * outputfile)
{
  infDrive_filtered (myfig, filename, locations, outputfile, "*");
}
