#include <stdio.h>
#include "avt_API_types.h"

/*
  MAN fclMarkCorrespondingSignal
  CATEG gns
  DESCRIPTION
  fclMarkSignal marks a signal in the circuit according to the given marking string.
  RETURN VALUE
  1 on success, 0 on failure.
  ERRORS
  Fails if mark string is not legal.
*/

int fclMarkCorrespondingSignal(char *signame, char *marks);

/*
  MAN fclMarkCorrespondingTransistor
  CATEG gns
  DESCRIPTION
  fclMarkTransistor marks a transistor in the circuit according to the given marking string.
  RETURN VALUE
  1 on success, 0 on failure.
  ERRORS
  Fails if mark string is not legal.
*/

int fclMarkCorrespondingTransistor(char *transname, char *marks);

/*
  MAN fclOrientCorrespondingSignal
  CATEG gns
  DESCRIPTION
  fclOrientSignal orients a signal according to the given output level.
  RETURN VALUE
  none.
  ERRORS
  displays a warning if a signal is given multiple orientations.
*/

void fclOrientCorrespondingSignal(char *signame, int level);

/*
  MAN fclCmpUpConstraint
  CATEG gns
  DESCRIPTION
  Specifies that one and only one of the signals in the list can be high.
  RETURN VALUE
  none.
  ERRORS
  nothing happens if the list is empty.
*/

void fclCmpUpConstraint(StringList *siglist);

/*
  MAN fclCmpDnConstraint
  CATEG gns
  DESCRIPTION
  Specifies that one and only one of the signals in the list can be low.
  RETURN VALUE
  none.
  ERRORS
  nothing happens if the list is empty.
*/

void fclCmpDnConstraint(StringList *siglist);

/*
  MAN fclMuxUpConstraint
  CATEG gns
  DESCRIPTION
  Specifies that at most one of the signals in the list can be high.
  RETURN VALUE
  none.
  ERRORS
  nothing happens if the list is empty.
*/

void fclMuxUpConstraint(StringList *siglist);

/*
  MAN fclMuxDnConstraint
  CATEG gns
  DESCRIPTION
  Specifies that at most one of the signals in the list can be low.
  RETURN VALUE
  none.
  ERRORS
  nothing happens if the list is empty.
*/

void fclMuxDnConstraint(StringList *siglist);

/*
  MAN fclAllowShare
  CATEG gns
  DESCRIPTION
  Specifies that the given transistor can be shared with another transistor level model.
  RETURN VALUE
  none.
  ERRORS
  none.
*/

void fclAllowShare(Transistor *transistor);

