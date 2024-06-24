#include <stdio.h>
#include <string.h>
#include "avt_API_types.h"

#ifndef AVTWIG_AVOID_CONFLICT

extern FILE *fopen(const char *name, const char *mode);
/*
    MAN fopen
    CATEG tcl+gns+files
    DESCRIPTION
    Opens a file in the specified mode and returns a pointer
    ARGS
    name % Name of the file to be opened
    mode % Available modes are {r} for reading, {w} for writing and {a} for appending
    EXAMPLE % {set file [fopen "design.stat" w]}
*/
extern int fclose(FILE *f);
/*
    MAN fclose
    CATEG tcl+gns+files
    DESCRIPTION
    closes a file
    ARGS
    f % Pointer on the file to be closed
    EXAMPLE % {fclose $file}
*/

extern void  free   (void *s);
extern char *strcpy (char *dest, const char *src);
extern char *strstr (const char *dest, const char *src);
extern char *strcat (char *dest, const char *src);
extern int   strcmp (const char *dest, const char *src);
extern char *strdup (const char *src);
extern int   strlen (const char *src);
int          fputs  (const char *s, FILE *stream);

#endif

extern int myfflush(FILE *f);

/*
    MAN runStatHiTas
    CATEG tcl+montecarlo
    SYNOPSIS
    runStatHiTas <numberofruns> [-slavescript <name>] [-jobscript <name>] [-datafile <name>] [-incremental | -override]\$
    DESCRIPTION
    Initiates monte-carlo statistical timing runs through the call of several independent hitas (slave) runs.\$
    This command creates a master process which will centralize all the different monte-carlo results.\$
    The TCL variable {ssta_MasterPath} is available in the slave runs and indicates the directory where the master script is run.
    ARGS
    numberofruns % Scheduling description of the jobs define as {<total number of runs>:<maximum number of parallel runs>} to run. Or simply the number of run desired if no parallelism is wanted.
    -jobscript <name> % Script file name. This script is called to spawn each slave runs. By default the slave scripts run on the same machine in separate sub-directories.
    -slavescript <name> % The slave monte-carlo script name which run a single monte carlo job. This argument can be used if the slave script is not the one using the API runStatHiTas. By default it's the current script.
    -datafile <name> % Result file name. If the file already exists, a number is added to the filename if neither {-incremental} nor {-override} are specified. By default the name is set to "ssta_data.log".
    -incremental % Force all the results to be added at the end of the data file preserving previous results.
    -override % Overrides existing datafile. 
    EXAMPLE % {runStatHiTas 500:2 -slavescript slave-script.tcl -datafile monte-data.log}\$Runs {500} {slave-script.tcl} ({2} max in parallel) with the result file {monte-data.log}.
*/
void runStatHiTas(void);
extern void runStatHiTas_sub( char *numrun, char *script, char *tool, char *fname, int incremental, char *resdir );

/*
    MAN avt_McPostData
    CATEG tcl+montecarlo
    DESCRIPTION
    Used by a slave monte-carlo run, tells the master to write the given message (result) to the result file.\$
    This function exits the slave script. 
    ARGS
    msg % Message to send.
    EXAMPLE % {avt_McPostData [list $myworstsetupslack $myworstholdslack]}
*/
extern void avt_McPostData( char *msg );

/*
    MAN avt_McInfo
    CATEG tcl+montecarlo
    DESCRIPTION
    Used by a slave monte-carlo run, tells the master to display/log the given message.\$
    The message does not go to the result file. 
    ARGS
    msg % Message to send.
    EXAMPLE % {avt_McInfo "Running into STA stage"}
*/
void avt_McInfo( char *msg );
extern Netlist *mbk_GetNetlist (char *figname);


//_____________________________________________________________________________
/*
MAN mbk_Sort
CATEG gns
DESCRIPTION
Sorts the array of values 'value_array' according to the 
indexes stored in the array 'index_array'. 'nbelem' is number of elements of 
both the arrays 'index_array' and 'value_array'. 
RETURN VALUE
none.
ERRORS
none.
*/

void mbk_Sort (int *index_array, void **value_array, int nbelem);

//_____________________________________________________________________________
/*
MAN mbk_FreeList
CATEG gns
DESCRIPTION
Frees a List but not the items of the list.
RETURN VALUE
none.
ERRORS
none.
*/

void mbk_FreeList(List *lst);

//_____________________________________________________________________________
/*
MAN mbk_GetListItem
CATEG gns
DESCRIPTION
Returns the current list item. The item is a pointer on void 
that must be casted to the desired type.
RETURN VALUE
mbk_GetListItem returns a pointer.
ERRORS
none.
*/

void *mbk_GetListItem(List *lst);

//_____________________________________________________________________________
/*
MAN mbk_AddListItem
CATEG gns
DESCRIPTION
Adds an item at the begining of a list. The new list head is
returned.
RETURN VALUE
mbk_AddListItem returns a 'List'.
ERRORS
none.
*/

List *mbk_AddListItem (List *lst, void *item);

//_____________________________________________________________________________
/*
MAN mbk_AppendList
CATEG gns
DESCRIPTION
Appends list <l2> at the end of list <l1>.
The new list head is returned.
NB: <l2> should not be used anymore.
RETURN VALUE
mbk_AppendList returns a 'List'.
ERRORS
none.
*/
List *mbk_AppendList(List *lst1, List *lst2);

//_____________________________________________________________________________
/*
MAN mbk_GetListNext
CATEG gns
DESCRIPTION
Returns the next node of the list.
RETURN VALUE
mbk_GetListNext returns a 'List'.
ERRORS
none.
*/

List *mbk_GetListNext(List *lst);

//_____________________________________________________________________________
/*
MAN mbk_EndofList
CATEG gns
DESCRIPTION
Returns 0 if lst is not the end of the list else a value different 
from 0. The end of the list can also be tested with (lst == NULL).
RETURN VALUE
mbk_EndofList returns an integer representing a boolean value.
ERRORS
none.
*/

int mbk_EndofList(List *lst);

//_____________________________________________________________________________
//_____________________________________________________________________________
/*
MAN mbk_NewHashTable
CATEG gns
DESCRIPTION
Creates a hash table with the initial size 'size'. However, if
the hash table becomes too crowded, it is automatically resized.
RETURN VALUE
mbk_NewHashTable returns a pointer on the newly created hash table.
ERRORS
none.
SEE ALSO
mbk_IsEmptyHashItem, mbk_AddHashItem, mbk_GetHashItem, mbk_FreeHashTable
*/

HashTable *mbk_NewHashTable    (int size);
//_____________________________________________________________________________
/*
MAN mbk_FreeHashTable
CATEG gns
DESCRIPTION
Deletes the hash table 'htable'. 
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
mbk_IsEmptyHashItem, mbk_AddHashItem, mbk_GetHashItem, mbk_NewHashTable
*/

void       mbk_FreeHashTable (HashTable *htable);
//_____________________________________________________________________________
/*
MAN mbk_AddHashItem
CATEG gns
DESCRIPTION
Stores the element 'value' in the hash table 'htable', according 
to the key 'key'.
RETURN VALUE
mbk_AddHashItem returns the stored value.
ERRORS
none.
SEE ALSO
mbk_GetHashItem, mbk_IsEmptyHashItem, mbk_FreeHashTable, mbk_NewHashTable
*/

long       mbk_AddHashItem         (HashTable *htable, void *key, long value);
//_____________________________________________________________________________
/*
MAN mbk_GetHashItem
CATEG gns
DESCRIPTION
Retrieves the element stored according to the key 'key' in the 
hash table 'htable'
RETURN VALUE
mbk_GetHashItem returns the stored element if it exists, an empty code otherwise.
mbk_GetHashItem return value must be tested with the 'mbk_IsEmptyHashItem' function.
ERRORS
none.
SEE ALSO
mbk_IsEmptyHashItem, mbk_AddHashItem, mbk_FreeHashTable, mbk_NewHashTable
*/

long       mbk_GetHashItem         (HashTable *htable, void *key);

//_____________________________________________________________________________
/*
MAN mbk_IsEmptyHashItem
CATEG gns
DESCRIPTION
Tests the returned value of the functions mbk_AddHashItem and 
mbk_GetHashItem.
RETURN VALUE
mbk_IsEmptyHashItem returns 1 if the tested value corresponds to an empty slot
in the hash table.
ERRORS
none.
SEE ALSO
mbk_GetHashItem, mbk_AddHashItem, mbk_FreeHashTable, mbk_NewHashTable
*/

int        mbk_IsEmptyHashItem     (long value);

int mbk_FindNumber(char *name, char *rule, int *index);


FILE *myfopen(char *name, const char *mode);
int myfputs (const char *s, FILE *stream);
int myfclose(FILE *f);

char *avt_McAsk( char *msg );
int avt_McIsSlave();

QuickList *mbk_QuickListCreate();
void mbk_QuickListAppend(QuickList *ql, char *item);
StringList *mbk_QuickListToTCL(QuickList *ql);
int mbk_QuickListLength(QuickList *ql);
void mbk_QuickListAppendDouble(QuickList *ql, double value);
DoubleList *mbk_QuickListComputeMeanVarMedian(QuickList *ql);
void mbk_QuickListDoubleFree(QuickList *ql);

//HashTable *mbk_NewHashTable (int size);
void mbk_AddStringHashItem (HashTable *htable, char *key, char *value);
char *mbk_GetStringHashItem (HashTable *htable, char *key);
void mbk_FreeStringHashTable (HashTable *htable);
StringList *mbk_GetStringHashTableKeys (HashTable *htable);

