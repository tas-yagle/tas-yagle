
typedef int (*API_COMMINUCATION)(int op, void *data);

extern API_COMMINUCATION TTV_COM;
extern API_COMMINUCATION STB_COM;
extern API_COMMINUCATION CTK_API_COM;

// --- TTV ---

#define TTV_getcurrentline 1
#define TTV_getdelaytype   2
#define TTV_getdelaytype_min   0
#define TTV_getdelaytype_max   1
#define TTV_getttvfig      3


// --- STB ---

#define STB_getstbfig      1

// --- CTK API ---
#define CTK_getctkstruct       1
#define CTK_rundefaultaction   2
#define CTK_getctknetlist      3
