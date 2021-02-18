
/*
MAN dtb_Load
CATEG gns
DESCRIPTION
dtb_Load(<database>) creates a new data base named <database>. If the database was already created, it will be cleaned.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_Save, dtb_Clean, dtb_Create
*/
int dtb_Load(char *name);


/*
MAN dtb_Save
CATEG gns
DESCRIPTION
dtb_Save(<database>) saves the <database> to a file. The file name will be .<database>.dtb and can be found in the current directory.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_Clean, dtb_Load, dtb_Create
*/
void dtb_Save(char *name);


/*
MAN dtb_Clean
CATEG gns
DESCRIPTION
dtb_Clean(<database>) removes of entries in the <database>.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_Save, dtb_Load, dtb_Create
*/
void dtb_Clean(char *name);


/*
MAN dtb_SetChar
CATEG gns
DESCRIPTION
dtb_SetChar(<database>, <varname>, <value>) creates or updates the entry <varname> in the <database> with the value <value> expressed as a character. In case of type mismatch, the old type will be overriden.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_SetString, dtb_SetLong, dtb_SetDouble, dtb_SetInt
*/
void dtb_SetChar(char *dbtname, char *name, char value);


/*
MAN dtb_SetString
CATEG gns
DESCRIPTION
dtb_SetString(<database>, <varname>, <value>) creates or updates the entry <varname> in the <database> with the value <value> expressed as a string. In case of type mismatch, the old type will be overriden.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_SetChar, dtb_SetLong, dtb_SetDouble, dtb_SetInt
*/
void dtb_SetString(char *dbtname, char *name, char *value);


/*
MAN dtb_SetLong
CATEG gns
DESCRIPTION
dtb_SetLong(<database>, <varname>, <value>) creates or updates the entry <varname> in the <database> with the value <value> expressed as a long integer. In case of type mismatch, the old type will be overriden.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_SetChar, dtb_SetString, dtb_SetDouble, dtb_SetInt
*/
void dtb_SetLong(char *dbtname, char *name, long value);


/*
MAN dtb_SetInt
CATEG gns
DESCRIPTION
dtb_SetInt(<database>, <varname>, <value>) creates or updates the entry <varname> in the <database> with the value <value> expressed as an integer. In case of type mismatch, the old type will be overriden.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_SetChar, dtb_SetString, dtb_SetLong, dtb_SetDouble
*/
void dtb_SetInt(char *dbtname, char *name, int value);


/*
MAN dtb_SetDouble
CATEG gns
DESCRIPTION
dtb_SetDouble(<database>, <varname>, <value>) creates or updates the entry <varname> in the <database> with the value <value> expressed as a double. In case of type mismatch, the old type will be overriden.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_SetChar, dtb_SetString, dtb_SetLong, dtb_SetInt
*/
void dtb_SetDouble(char *dtbname, char *name, double value);


/*
MAN dtb_GetDouble
CATEG gns
DESCRIPTION
dtb_GetDouble(<database>, <varname>) returns the double value of the entry <varname> in the <database>. If the entry does not exist or the type mismatchs, 0.0 is returned.
RETURN VALUE
resurns a double.
ERRORS
none.
SEE ALSO
dtb_GetChar, dtb_GetString, dtb_GetLong, dtb_GetInt
*/
double dtb_GetDouble(char *dtbname, char *name);


/*
MAN dtb_GetInt
CATEG gns
DESCRIPTION
dtb_GetInt(<database>, <varname>) returns the integer value of the entry <varname> in the <database>. If the entry does not exist or the type mismatchs, 0 is returned.
RETURN VALUE
resurns an int.
ERRORS
none.
SEE ALSO
dtb_GetChar, dtb_GetString, dtb_GetLong, dtb_GetDouble
*/
int dtb_GetInt(char *dtbname, char *name);


/*
MAN dtb_GetLong
CATEG gns
DESCRIPTION
dtb_GetLong(<database>, <varname>) returns the long integer value of the entry <varname> in the <database>. If the entry does not exist or the type mismatchs, 0 is returned.
RETURN VALUE
resurns a long int.
ERRORS
none.
SEE ALSO
dtb_GetChar, dtb_GetString, dtb_GetDouble, dtb_GetInt
*/
long dtb_GetLong(char *dtbname, char *name);


/*
MAN dtb_GetString
CATEG gns
DESCRIPTION
dtb_GetString(<database>, <varname>) returns the string value of the entry <varname> in the <database>. If the entry does not exist or the type mismatchs, NULL is returned.
RETURN VALUE
resurns a string.
ERRORS
none.
SEE ALSO
dtb_GetChar, dtb_GetLong, dtb_GetDouble, dtb_GetInt
*/
char *dtb_GetString(char *dtbname, char *name);


/*
MAN dtb_GetChar
CATEG gns
DESCRIPTION
dtb_Getchar(<database>, <varname>) returns the character value of the entry <varname> in the <database>. If the entry does not exist or the type mismatchs, ' ' is returned.
RETURN VALUE
resurns a char.
ERRORS
none.
SEE ALSO
dtb_GetString, dtb_GetLong, dtb_GetDouble, dtb_GetInt
*/
char dtb_GetChar(char *dtbname, char *name);


/*
MAN dtb_RemoveEntry
CATEG gns
DESCRIPTION
dtb_RemoveEntry(<database>, <varname>) removes the entry <varname> from the <database>. If the entry does not exist, nothing is done.
RETURN VALUE
none.
ERRORS
none.
*/
void dtb_RemoveEntry(char *dtbname, char *name);

/*
MAN dtb_Create
CATEG gns
DESCRIPTION
dtb_Create(<database>) creates a new database named <database>. If the database already exits, the function call has no effects.
RETURN VALUE
none.
ERRORS
none.
SEE ALSO
dtb_Load, dtb_Save, , dtb_Clean
*/
void dtb_Create(char *name);

void database_API_AtLoad_Initialize();
