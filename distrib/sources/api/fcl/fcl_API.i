

int fclMarkCorrespondingSignal(char *signame, char *marks);
int fclMarkCorrespondingTransistor(char *transname, char *marks);
void fclOrientCorrespondingSignal(char *signame, int level);
void fclCmpUpConstraint(StringList *siglist);
void fclCmpDnConstraint(StringList *siglist);
void fclMuxUpConstraint(StringList *siglist);
void fclMuxDnConstraint(StringList *siglist);
void fclAllowShare(Transistor *transistor);

