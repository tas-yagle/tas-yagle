/*------------------------------------------------------------\
|                                                             |
| Tool    :                    XYAG                           |
|                                                             |
| File    :                  Share.c                          |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <X11/Xresource.h>

#include AVT_H
#include MUT_H
#include SLB_H
#include MLO_H
#include BEH_H
#include BEF_H
#include BVL_H
#include XSB_H
#include XMX_H
#include CGV_H

#include "XSB_share.h"


char           *XYAG_LAYER_NAME_TABLE[XYAG_MAX_LAYER][3] =
{
    {"Gates", "cyan", "Black"},
    {"Inputs", "red", "Black"},
    {"Outputs", "green", "Black"},
    {"Signals", "blue", "Black"},
    {"Debug", "green", "Black"}
};

char           *XYAG_CURSOR_COLOR_NAME = "Black";
char           *XYAG_BACKGROUND_COLOR_NAME = "Black";
char           *XYAG_FOREGROUND_COLOR_NAME = "White";
char           *XYAG_ACCEPT_COLOR_NAME = "magenta";
char           *XYAG_CONNECT_COLOR_NAME = "White";

long            XYAG_UNIT = 1;

char            XYAG_ACTIVE_NAME_TABLE[XYAG_MAX_ACTIVE_NAME] =
{
    1,                          /* CONNECTOR */
    1                           /* INSTANCE  */
};

char            XYAG_ACTIVE_LAYER_TABLE[XYAG_MAX_LAYER] =
{
    1, 1, 1, 1, 1
};

char           *XyagFirstFileName = NULL;
char           *XYAG_XMS_FILE_NAME = NULL;
char            XYAGLE_IN_CGV[5];
int             XyagCnsMode = 0;

char            XYAG_FORCE_DISPLAY = XYAG_FALSE;
char            XYAG_XOR_CURSOR = XYAG_FALSE;
char            XYAG_SWITCH_COLOR_MAP = XYAG_FALSE;

library        *XYAG_SYMBOL_LIBRARY = NULL;

//#ifdef xcones
//char XYAGLE_NAME[] = "XCones";
//#else
//char XYAGLE_NAME[] = "Xyagle";
//#endif

char *
XyagPostTreatString(Text)
    char           *Text;
{
    char            OnlySpace;
    int             Counter;

    if (Text != (char *) NULL) {
        OnlySpace = 1;

        for (Counter = 0; Text[Counter]; Counter++) {
            if (Text[Counter] != ' ')
                OnlySpace = 0;
        }
        if (OnlySpace)
            Text = (char *) NULL;
    }
    return (Text);
}

/*------------------------------------------------------------\
|                        Xyaggetenv                           |
\------------------------------------------------------------*/

extern void drive_slib(FILE *f);
extern void drive_corresp(FILE *f);

mbkContext *XYAG_ctx;
cnsContext *CNS_ctx;

void addlocked(lofig_list *lf)
{
  while (lf!=NULL)
    {
      addhtitem(HT_LOFIG, lf->NAME, (long)lf);
      lf=lf->NEXT;
    }
}

void
Xyaggetenv()
{
    char           *Env;
    FILE *f;
    library        *mine;
    char name[100];
    lofig_list *lf;

    Env = getenv("HOME");

    if (Env == NULL) {
        XYAG_XMS_FILE_NAME = XMS_FILE_NAME;
    }
    else {
        XYAG_XMS_FILE_NAME = (char *)mbkalloc(strlen(Env) + strlen(XMS_FILE_NAME) + 2);
        strcpy(XYAG_XMS_FILE_NAME, Env);
        strcat(XYAG_XMS_FILE_NAME, "/");
        strcat(XYAG_XMS_FILE_NAME, XMS_FILE_NAME);
    }

   

    Env = getenv("XYAGLE_IN_CGV");
    if (Env == NULL) {
        strcpy(XYAGLE_IN_CGV, "cns");
        XyagCnsMode = 1;
    }
    else {
        strcpy(XYAGLE_IN_CGV, Env);
        if (Env != NULL) {
            if (!strcmp(Env, "cns")) {
                (void) strcpy(XYAGLE_IN_CGV, "cns");
                XyagCnsMode = 1;
            }
            else {
                (void) fflush(stdout);
                (void) fprintf(stderr, "*** xyag error ***\n");
                (void) fprintf(stderr, "Input format '%s' not supported\n", Env);
                XyagExit(1);
            }
        }
    }

    Env = V_STR_TAB[__BEH_IN_FORMAT].VALUE;
    if (Env != NULL)
        BEH_IN = namealloc(Env);
    else
        BEH_IN = namealloc("vhd");

    Env = V_STR_TAB[__MBK_IN_LO].VALUE;
    if (Env != NULL)
        strcpy(IN_LO, Env);
    else
        strcpy(IN_LO, "spi");

//    XYAG_SYMBOL_LIBRARY =  slib_get_library();
    sprintf(name,"/tmp/default.slib%ld",time(NULL));
    if ((f=fopen(name,"wt"))==NULL)
    {
        fprintf(stderr,"could not write file '%s'\n",name);
        XyagExit(1);
    }
    drive_slib(f);
    fclose(f);
    mine=XYAG_SYMBOL_LIBRARY =  slib_load_library(name);
    unlink(name);
    cgv_SetDefaultLibrary(XYAG_SYMBOL_LIBRARY);

    SLIB_LIBRARY=NULL;
    XYAG_SYMBOL_LIBRARY =  slib_get_library();
    if (XYAG_SYMBOL_LIBRARY!=NULL)
      {
        XYAG_SYMBOL_LIBRARY->GRID=mine->GRID;
      }

    sprintf(name,"/tmp/slib.corresp%ld",time(NULL));

    f=fopen(name,"wt");
    if ((f=fopen(name,"wt"))==NULL)
    {
        fprintf(stderr,"could not write file '%s'\n",name);
        XyagExit(1);
    }
    drive_corresp(f);
    fclose(f);
    parsecorresp(name);
    unlink(name);


    // gestion d'un context perso pour les lofigs
    XYAG_ctx=mbkCreateContext();
    lf=HEAD_LOFIG;
    mbkSwitchContext(XYAG_ctx);
    addlocked(lf);
    mbkSwitchContext(XYAG_ctx);
    // gestion d'un context perso pour les cnss
    CNS_ctx=cnsCreateContext();

}

void
XyagLoadColors()
{
    char            buffer[256];
    XrmDatabase     db;
    Display         *display;
    XrmValue        value;
    char            *type[20];

    display = XtDisplay(XyagTopLevel);
    db = XrmGetDatabase(display);
    
    if (XrmGetResource(db,"xyagle.schematicCursor","Xyagle.schematicCursor",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
        XYAG_CURSOR_COLOR_NAME = namealloc(buffer);
    }
    if (XrmGetResource(db,"xyagle.schematicBackground","Xyagle.schematicBackground",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
        XYAG_BACKGROUND_COLOR_NAME = namealloc(buffer);
        XYAG_LAYER_NAME_TABLE[0][2] = XYAG_BACKGROUND_COLOR_NAME;
        XYAG_LAYER_NAME_TABLE[1][2] = XYAG_BACKGROUND_COLOR_NAME;
        XYAG_LAYER_NAME_TABLE[2][2] = XYAG_BACKGROUND_COLOR_NAME;
        XYAG_LAYER_NAME_TABLE[3][2] = XYAG_BACKGROUND_COLOR_NAME;
        XYAG_LAYER_NAME_TABLE[4][2] = XYAG_BACKGROUND_COLOR_NAME;
    }
    if (XrmGetResource(db,"xyagle.schematicForeground","Xyagle.SchematicForeground",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
        XYAG_FOREGROUND_COLOR_NAME = namealloc(buffer);
    }
    if (XrmGetResource(db,"xyagle.schematicAccept","Xyagle.SchematicAccept",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
        XYAG_ACCEPT_COLOR_NAME = namealloc(buffer);
    }
    if (XrmGetResource(db,"xyagle.schematicHilite","Xyagle.SchematicHilite",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
        XYAG_CONNECT_COLOR_NAME = namealloc(buffer);
    }
    if (XrmGetResource(db,"xyagle.schematicGates","Xyagle.SchematicGates",type,&value) == True) {
         strncpy(buffer,value.addr,(int) value.size);
         XYAG_LAYER_NAME_TABLE[0][1] = namealloc(buffer);
    }
        if (XrmGetResource(db,"xyagle.schematicInputs","Xyagle.SchematicInputs",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
         XYAG_LAYER_NAME_TABLE[1][1] = namealloc(buffer);
    }
        if (XrmGetResource(db,"xyagle.schematicOutputs","Xyagle.SchematicOutputs",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
         XYAG_LAYER_NAME_TABLE[2][1] = namealloc(buffer);
    }
        if (XrmGetResource(db,"xyagle.schematicSignals","Xyagle.SchematicSignals",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
         XYAG_LAYER_NAME_TABLE[3][1] = namealloc(buffer);
    }
        if (XrmGetResource(db,"xyagle.schematicDebug","Xyagle.SchematicDebug",type,&value) == True) {
        strncpy(buffer,value.addr,(int) value.size);
         XYAG_LAYER_NAME_TABLE[4][1] = namealloc(buffer);
    }
}
