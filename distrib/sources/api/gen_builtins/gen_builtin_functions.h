// standard C functions
#undef strcpy


extern char *namealloc(char *name);



// genius functions

extern FILE *avtfopen(char *name, char *ext, char *mode);

/*
  MAN char_to_string
  CATEG gns
  DESCRIPTION
  char_to_string create a string which length is 'size'. The string is filled with the caracter 'caract'.
  RETURN VALUE
  char_to_string return a string
  ERRORS
  if 'size' is lower than 1, the function aborts the program.
*/
extern char *char_to_string(int size, char caract);

/*
  MAN onehot_to_bit
  CATEG gns
  DESCRIPTION
  onehot_to_bit(size, bitnum) returns a binary string whose length is 'size'. The string represents a binary value where bit number 'bitnum' is set to 1 and the others to 0. The 'bitnum' is little endian oriented.
  RETURN VALUE
  onehot_to_bit return a string
  ERRORS
  The function ends the program if bitnum<1 or bitnum>size.
  SEE ALSO
  onehot_to_hexa, onehot_to_octa, onecold_to_bit, onecold_to_hexa, onecold_to_octa
*/
extern char *onehot_to_bit(int size, int bitnum);
/*
  MAN onehot_to_hexa
  CATEG gns
  DESCRIPTION
  onehot_to_hexa(size, bitnum) returns a hexadecimal string whose length is 'size'. The string represents a hexadecimal value where bit number 'bitnum' is set to 1 and the others to 0. The 'bitnum' is little endian oriented.
  RETURN VALUE
  onehot_to_hexa return a string
  ERRORS
  The function ends the program if bitnum<1 or bitnum>size.
  SEE ALSO
  onehot_to_bit, onehot_to_octa, onecold_to_bit, onecold_to_hexa, onecold_to_octa
*/
extern char *onehot_to_hexa(int size, int bitnum);
/*
  MAN onehot_to_octa
  CATEG gns
  DESCRIPTION
  onehot_to_octa(size, bitnum) returns an octal string whose length is 'size'. The string represents an octal value where bit number 'bitnum' is set to 1 and the others to 0. The 'bitnum' is little endian oriented.
  RETURN VALUE
  onehot_to_octa return a string
  ERRORS
  The function ends the program if bitnum<1 or bitnum>size.
  SEE ALSO
  onehot_to_hexa, onehot_to_bit, onecold_to_bit, onecold_to_hexa, onecold_to_octa
*/
extern char *onehot_to_octa(int size, int bitnum);
/*
  MAN onecold_to_bit
  CATEG gns
  DESCRIPTION
  onecold_to_bit(size, bitnum) returns a binary string whose length is 'size'. The string represents a binary value where bit number 'bitnum' is set to 0 and the others to 1. The 'bitnum' is little endian oriented.
  RETURN VALUE
  onecold_to_bit return a string
  ERRORS
  The function ends the program if bitnum<1 or bitnum>size.
  SEE ALSO
  onehot_to_hexa, onehot_to_octa, onehot_to_bit, onecold_to_hexa, onecold_to_octa
*/
extern char *onecold_to_bit(int size, int bitnum);
/*
  MAN onecold_to_hexa
  CATEG gns
  DESCRIPTION
  onecold_to_hexa(size, bitnum) returns a hexadecimal string whose length is 'size'. The string represents a hexadecimal value where bit number 'bitnum' is set to 0 and the others to 1. The 'bitnum' is little endian oriented.
  RETURN VALUE
  onecold_to_hexa return a string
  ERRORS
  The function ends the program if bitnum<1 or bitnum>size.
  SEE ALSO
  onehot_to_hexa, onehot_to_octa, onehot_to_bit, onecold_to_bit, onecold_to_octa
*/
extern char *onecold_to_hexa(int size, int bitnum);
/*
  MAN onecold_to_octa
  CATEG gns
  DESCRIPTION
  onecold_to_octa(size, bitnum) returns an octal string whose length is 'size'. The string represents an octal value where bit number 'bitnum' is set to 0 and the others to 1. The 'bitnum' is little endian oriented.
  RETURN VALUE
  onecold_to_octa return a string
  ERRORS
  The function ends the program if bitnum<1 or bitnum>size.
  SEE ALSO
  onehot_to_hexa, onehot_to_octa, onehot_to_bit, onecold_to_bit, onecold_to_hexa
*/
extern char *onecold_to_octa(int size, int bitnum);

/*
  MAN genius_date
  CATEG gns
  DESCRIPTION
  genius_date() return a string containing the current date and time.
  RETURN VALUE
  genius_date() return a string
  ERRORS
  none.
*/
extern char *genius_date();

/*
  MAN gns_ModelVisited
  CATEG gns
  DESCRIPTION
  gns_ModelVisited(<name>) returns 0 if the model <name> has been set as visited thru the function gns_MarkModelVisited.
  RETURN VALUE
  gns_ModelVisited returns an integer
  ERRORS
  none.
  SEE ALSO
  gns_MarkModelVisited
*/

extern int gns_ModelVisited(char *name);

/*
  MAN gns_MarkModelVisited
  CATEG gns
  DESCRIPTION
  gns_MarkModelVisited(<name>) adds <name> in the list of model already visited.
  RETURN VALUE
  none.
  ERRORS
  no errors.
  SEE ALSO
  gns_ModelVisited
*/

extern void gns_MarkModelVisited(char *name);

char *bitstring_to_hexastring(char *val);


long genius_top();
