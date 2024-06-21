
typedef struct _ApiType
{
  char *TYPE_NAME;
  int TYPE;
  char *varname;
  int cst;
} _ApiType;

typedef struct defines
{
  char *VARNAME;
  char type;
  union 
  {
    int I;
    double D;
  } T;
} defines;

typedef struct
{
  char *funcname;
  char *desc;
  char *retval;
  char *errors;
  char *seealso;
  char *uppersection;
  char *categ;
  char *synop;
} functionman;
#define T_STRING 123489
