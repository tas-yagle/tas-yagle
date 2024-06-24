#ifdef SWIGTCL
/*  PROPERTY                                                                          */
%typemap (out) Property* {
    ptype_list  *pl;

    pl=(ptype_list *)$1;
    if (pl!=NULL)
    {
      switch (pl->TYPE) {
          case TYPE_TIMING_SIGNAL:
              Tcl_SetObjResult (interp, SWIG_NewPointerObj((TimingSignal *) pl->DATA, $descriptor(TimingSignal *), 0));
              break;
          case TYPE_TIMING_EVENT:
              Tcl_SetObjResult (interp, SWIG_NewPointerObj((TimingEvent *) pl->DATA, $descriptor(TimingEvent *), 0));
              break;
          case TYPE_TIMING_PATH:
              Tcl_SetObjResult (interp, SWIG_NewPointerObj((TimingPath *) pl->DATA, $descriptor(TimingPath *), 0));
              break;
          case TYPE_TIMING_FIGURE:
              Tcl_SetObjResult (interp, SWIG_NewPointerObj((TimingFigure *) pl->DATA, $descriptor(TimingFigure *), 0));
              break;
          case TYPE_STABILITY_FIGURE:
              Tcl_SetObjResult (interp, SWIG_NewPointerObj((StabilityFigure *) pl->DATA, $descriptor(StabilityFigure *), 0));
              break;
          case TYPE_CHAR:
              Tcl_SetObjResult (interp, Tcl_NewStringObj ((char*)pl->DATA, -1));
              free ((char*)pl->DATA);
              break;
          case TYPE_INT:
              Tcl_SetObjResult (interp, Tcl_NewIntObj ((long)pl->DATA));
              break;
          case TYPE_DOUBLE:
              Tcl_SetObjResult (interp, Tcl_NewDoubleObj (*(float *)&pl->DATA));
              break;
          case TYPE_DOUBLE_TABLE:
          {
              chain_list  *cl, *ch0;
              Tcl_Obj     *mainlist=Tcl_NewObj(), *list0;

              for (ch0=(chain_list *)pl->DATA; ch0!=NULL; ch0=ch0->NEXT) {
                list0=Tcl_NewObj();
                for (cl=(chain_list *)ch0->DATA; cl!=NULL; cl=cl->NEXT) {
                  Tcl_ListObjAppendElement (interp, list0, Tcl_NewDoubleObj(*(float *)&cl->DATA));
                }
                freechain((chain_list *)ch0->DATA);
                Tcl_ListObjAppendElement (interp, mainlist, list0);
              }
              freechain((chain_list *)pl->DATA);
              Tcl_SetObjResult (interp, mainlist);
          }
          break;
          case TYPE_TABLE:
          {
              chain_list  *cl;
              Tcl_Obj     *list0;

              list0=Tcl_NewObj();
              for (cl=(chain_list *)pl->DATA; cl!=NULL; cl=cl->NEXT) {
                Tcl_ListObjAppendElement (interp, list0, Tcl_NewDoubleObj(*(float *)&cl->DATA));
              }
              freechain((chain_list *)pl->DATA);
              Tcl_SetObjResult (interp, list0);
          }
          break;
      }
    }
    else
      Tcl_SetObjResult (interp, Tcl_NewStringObj ("error", -1));

    freeptype((ptype_list *)$1);
}

/*  SLACK LIST                                                                      */
%typemap (out) StabilitySlackList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((StabilitySlack *) ch->DATA, $descriptor(StabilitySlack *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) StabilitySlackList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (StabilitySlackList *)reverse (cl);
}
%typemap(freearg) StabilitySlackList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  TIMING SIGNAL LIST                                                                      */
%typemap (out) TimingSignalList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((TimingSignal *) ch->DATA, $descriptor(TimingSignal *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) TimingSignalList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (TimingSignalList *)reverse (cl);
}
%typemap(freearg) TimingSignalList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  TIMING EVENT LIST                                                                      */
%typemap (out) TimingEventList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((TimingEvent *) ch->DATA, $descriptor(TimingEvent *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) TimingEventList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (TimingEventList *)reverse (cl);
}
%typemap(freearg) TimingEventList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  TIMING DETAIL LIST                                                                      */
%typemap (out) TimingDetailList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((TimingDetail *) ch->DATA, $descriptor(TimingDetail *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) TimingDetailList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (TimingDetailList *)reverse (cl);
}
%typemap(freearg) TimingDetailList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  TIMING PATH LIST                                                                     */
%typemap (out) TimingPathList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((TimingPath *) ch->DATA, $descriptor(TimingPath *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) TimingPathList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (TimingPathList *)reverse (cl);
}
%typemap(freearg) TimingPathList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  StabilityPath LIST                                                                   */
%typemap (out) StabilityPathList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((StabilityPath *) ch->DATA, $descriptor(StabilityPath *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) StabilityPathList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (StabilityPathList*)reverse (cl);
}
%typemap(freearg) StabilityPathList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  StabilityRange LIST                                                                   */
%typemap (out) StabilityRangeList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((StabilityRange *) ch->DATA, $descriptor(StabilityRange *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) StabilityRangeList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (StabilityRangeList *)reverse (cl);
}
%typemap(freearg) StabilityRange *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  TIMINGLINE LIST                                                                          */
%typemap (out) TimingLineList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((TimingLine *) ch->DATA, $descriptor(TimingLine *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) TimingLineList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (TimingLineList *)reverse (cl);
}
%typemap(freearg) TimingLineList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  TimingConstraint LIST                                                                          */
%typemap (out) TimingConstraintList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((TimingConstraint *) ch->DATA, $descriptor(TimingConstraint *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) TimingConstraintList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (TimingConstraintList *)reverse (cl);
}
%typemap(freearg) TimingConstraintList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  VOID LIST                                                                          */
%typemap (out) List* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((void *) ch->DATA, $descriptor(void *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) List* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (List *)reverse (cl);
}
%typemap(freearg) List *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/*  DOUBLE LIST                                                                          */
%typemap (out) DoubleList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, Tcl_NewDoubleObj((double)(*((float *)&ch->DATA))));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap(freearg) DoubleList* {
     freechain((chain_list *)$1);
}

%typemap(in) DoubleListTimeValue* {
     Tcl_Obj **listobjv;
     int       nitems;
     int       i;
     chain_list *cl;
     
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     cl=NULL;
     for (i = 0; i < nitems; i++) {
       cl=addchain(cl, NULL);
       *(float *)&cl->DATA=avt_parse_unit(Tcl_GetStringFromObj(listobjv[i],0), 't');
     }
     $1=(DoubleListTimeValue *)reverse(cl);
}
%typemap(freearg) DoubleListTimeValue* {
     freechain((chain_list *)$1);
}
%typemap(in) DoubleListCapaValue* {
     Tcl_Obj **listobjv;
     int       nitems;
     int       i;
     chain_list *cl;
     
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     cl=NULL;
     for (i = 0; i < nitems; i++) {
       cl=addchain(cl, NULL);
       *(float *)&cl->DATA=avt_parse_unit(Tcl_GetStringFromObj(listobjv[i],0), 'c');
     }
     $1=(DoubleListCapaValue *)reverse(cl);
}
%typemap(freearg) DoubleListCapaValue* {
     freechain((chain_list *)$1);
}

%typemap(in) DoubleList* {
     Tcl_Obj **listobjv;
     int       nitems;
     int       i;
     chain_list *cl;
     double val;
     
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     cl=NULL;
     for (i = 0; i < nitems; i++) {
       cl=addchain(cl, NULL);
       Tcl_GetDoubleFromObj(interp, listobjv[i], &val);
       *(float *)&cl->DATA=val;
     }
     $1=(DoubleList *)reverse(cl);
}

/*  LIST OF DOUBLE LIST                                                                  */
%typemap (out) ListOfDoubleTable* {
    chain_list  *ch, *cl,*ch0;
    Tcl_Obj     *mainlist=Tcl_NewObj(), *list, *list0;

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        list=Tcl_NewObj();
        for (ch0=(chain_list *)ch->DATA; ch0!=NULL; ch0=ch0->NEXT) {
          list0=Tcl_NewObj();
          for (cl=(chain_list *)ch0->DATA; cl!=NULL; cl=cl->NEXT) {
            Tcl_ListObjAppendElement (interp, list0, Tcl_NewDoubleObj(*(float *)&cl->DATA));
          }
          freechain((chain_list *)ch0->DATA);
          Tcl_ListObjAppendElement (interp, list, list0);
        }
        freechain((chain_list *)ch->DATA);
        Tcl_ListObjAppendElement (interp, mainlist, list);
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, mainlist);
}

/*  LIST OF TIMING PATH LIST                                                                  */
%typemap (out) TimingPathTable* {
    chain_list  *cl, *ch0;
    Tcl_Obj     *mainlist=Tcl_NewObj(), *list0;

    for (ch0=(chain_list *)$1; ch0!=NULL; ch0=ch0->NEXT) {
      list0=Tcl_NewObj();
      for (cl=(chain_list *)ch0->DATA; cl!=NULL; cl=cl->NEXT) {
        Tcl_ListObjAppendElement (interp, list0, SWIG_NewPointerObj((TimingPath *) cl->DATA, $descriptor(TimingPath *),0));
      }
      freechain((chain_list *)ch0->DATA);
      Tcl_ListObjAppendElement (interp, mainlist, list0);
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, mainlist);
}
/*  TABLE OF DOUBLE                                                                  */
%typemap (out) DoubleTable* {
    chain_list  *cl, *ch0;
    Tcl_Obj     *mainlist=Tcl_NewObj(), *list0;

    for (ch0=(chain_list *)$1; ch0!=NULL; ch0=ch0->NEXT) {
      list0=Tcl_NewObj();
      for (cl=(chain_list *)ch0->DATA; cl!=NULL; cl=cl->NEXT) {
        Tcl_ListObjAppendElement (interp, list0, Tcl_NewDoubleObj(*(float *)&cl->DATA));
      }
      freechain((chain_list *)ch0->DATA);
      Tcl_ListObjAppendElement (interp, mainlist, list0);
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, mainlist);
}
%typemap(in) DoubleTable* {
     Tcl_Obj **listobjv, **listobjv0;
     int       nitems, nitems0;
     int       i, j;
     chain_list *cl, *ch0;
     double val;
     
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     ch0=NULL;
     for (i = 0; i < nitems; i++) {
       if (Tcl_ListObjGetElements(interp, listobjv[i], &nitems0, &listobjv0) == TCL_ERROR) {
        return TCL_ERROR;
       }
       cl=NULL;
       for (j = 0; j < nitems0; j++) {
         cl=addchain(cl, NULL);
         Tcl_GetDoubleFromObj(interp, listobjv0[j], &val);
         *(float *)&cl->DATA=val;
       }
       ch0=addchain(ch0, reverse(cl));
     }
       
     $1=(DoubleTable *)reverse(ch0);
}

%typemap(freearg) DoubleTable* {
    chain_list *cl;
    for (cl=(chain_list *)$1; cl!=NULL; cl=delchain(cl, cl))
      freechain((chain_list *)cl->DATA);
}

/*  TABLE OF INT                                                                      */
%typemap(in) IntegerTable* {
     Tcl_Obj **listobjv, **listobjv0;
     int       nitems, nitems0;
     int       i, j;
     chain_list *cl, *ch0;
     int val;
     
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     ch0=NULL;
     for (i = 0; i < nitems; i++) {
       if (Tcl_ListObjGetElements(interp, listobjv[i], &nitems0, &listobjv0) == TCL_ERROR) {
        return TCL_ERROR;
       }
       cl=NULL;
       for (j = 0; j < nitems0; j++) {
         cl=addchain(cl, NULL);
         Tcl_GetIntFromObj(interp, listobjv0[j], &val);
         cl->DATA=(void *)(long)val;
       }
       ch0=addchain(ch0, reverse(cl));
     }
       
     $1=(IntegerTable *)reverse(ch0);
}
%typemap(freearg) IntegerTable* {
    chain_list *cl;
    for (cl=(chain_list *)$1; cl!=NULL; cl=delchain(cl, cl))
      freechain((chain_list *)cl->DATA);
}

/*  FILE                                                                               */
%typemap (in) FILE* {
    char    *str;
    FILE    *ptr;
    
    if ((str = Tcl_GetString ($input))) {
        if (strcmp (str, "stdout") == 0)
            $1 = stdout;
        else if (strcmp (str, "stderr") == 0)
            $1 = stderr;
        else if (strcmp (str, "stdin") == 0)
            $1 = stdin;
        else {
            SWIG_ConvertPtr ($input, (void **)&ptr, NULL, 0);
            $1 = ptr;
        }
    }
}

/*  STRING TAB                                                                          */
%typemap(in) char **argv {
     Tcl_Obj **listobjv;
     int       nitems;
     int       i;
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     $1 = (char **) malloc((nitems+1)*sizeof(char *));
     for (i = 0; i < nitems; i++) {
        $1[i] = Tcl_GetStringFromObj(listobjv[i],0);
     }
     $1[i] = 0;
}

%typemap(freearg) char **argv{
     free($1);
}

%typemap(in) List *argv {
     Tcl_Obj **listobjv;
     int       nitems;
     int       i;
     chain_list *cl;
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     cl=NULL;
     for (i = 0; i < nitems; i++) {
       cl=addchain(cl, Tcl_GetStringFromObj(listobjv[i],0));
     }
     $1=(List *)reverse(cl);
}

%typemap (out) StringList * {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, Tcl_NewStringObj((char *)ch->DATA, strlen((char *)ch->DATA)));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (out) StringListF * {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, Tcl_NewStringObj((char *)ch->DATA, strlen((char *)ch->DATA)));
        free((char *)ch->DATA);
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap(freearg) List *argv{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

%typemap(in) StringList * {
     Tcl_Obj **listobjv;
     int       nitems;
     int       i;
     chain_list *cl;
     if (Tcl_ListObjGetElements(interp, $input, &nitems, &listobjv) == TCL_ERROR) {
        return TCL_ERROR;
     }
     cl=NULL;
     for (i = 0; i < nitems; i++) {
       cl=addchain(cl, Tcl_GetStringFromObj(listobjv[i],0));
     }
     $1=(StringList *)reverse(cl);
}

%typemap(freearg) StringList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}

/* Aggressor List */
%typemap (out) AggressorList* {
    chain_list  *ch;
    Tcl_Obj     *list=Tcl_NewObj();

    for (ch=(chain_list *)$1 ; ch ; ch=ch->NEXT) {
        Tcl_ListObjAppendElement (interp, list, SWIG_NewPointerObj((Aggressor *) ch->DATA, $descriptor(Aggressor *),0));
    }
    freechain((chain_list *)$1);
    Tcl_SetObjResult (interp, list);
}

%typemap (in) AggressorList* {
    int          nbelem, i;
    Tcl_Obj     **list;
    void        *ptr;
    chain_list *cl;
    
    cl = NULL;
    if (Tcl_ListObjGetElements (interp, $input, &nbelem, &list) == TCL_ERROR)
        return TCL_ERROR;

    for (i=0; i<nbelem ; i++) {
        SWIG_ConvertPtr (list[i], (void **)&ptr, NULL, 0);
        cl = addchain (cl, ptr);
    }
    $1 = (AggressorList *)reverse (cl);
}
%typemap(freearg) AggressorList *{
     if ($1) {
        freechain((chain_list *)$1);
     }
}
/*  UNITS                                                                               */
%typemap (in) TimeValue {
    char    *str;
    double val;

    if ((str = Tcl_GetString ($input))) {
        val=avt_parse_unit(str, 't');
        $1=val;
    } else {
        return TCL_ERROR;
    }
}

%typemap (in) CapaValue {
    char    *str;
    double val;

    if ((str = Tcl_GetString ($input))) {
        val=avt_parse_unit(str, 'c');
        $1=val;
    } else {
        return TCL_ERROR;
    }
}

%typemap (in) VoltageValue {
    char    *str;
    double val;

    if ((str = Tcl_GetString ($input))) {
        val=avt_parse_unit(str, 'v');
        $1=val;
    } else {
        return TCL_ERROR;
    }
}

/*  String to free                                                                      */
%typemap (out) String* {
    Tcl_SetObjResult(interp,Tcl_NewStringObj((char *)$1,-1));
    free($1);
}
#endif
