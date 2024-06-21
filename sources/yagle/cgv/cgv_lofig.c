#include MUT_H
#include MLO_H
#include CGV_H



symbol_list *cgv_getlofigcellsymbol(library *ptlib, char *symname)
{
    symbol_list    *ptsym;

    if (ptlib == NULL) return NULL;

    if ((ptsym = slib_getsymbol(ptlib, symname)) == NULL) {
      if (SLIB_LIBRARY==NULL) return NULL;
        ptsym = slib_get_flat_symbol(SLIB_LIBRARY, symname);
        if (ptsym != NULL) {
            slib_put_symbol(ptlib, ptsym);
            ptsym->MOTIF = slib_motifconvert(ptsym);
        }
    }
    return ptsym;
}

symbol_list *cgv_getselfcellsymbol(library *ptlib, char *symname)
{
    symbol_list    *ptsym;

    if (ptlib == NULL) return NULL;

    if ((ptsym = slib_getsymbol(ptlib, symname)) == NULL) {
      if (CGV_LIB==NULL) return NULL;
        ptsym = slib_get_flat_symbol(CGV_LIB, symname);
        if (ptsym != NULL) {
            slib_put_symbol(ptlib, ptsym);
            ptsym->MOTIF = slib_motifconvert(ptsym);
        }
    }
    return ptsym;
}


cgvfig_list *getcgvfig_from_lofig(char *Name, char *filename)
{
    lofig_list     *LoFigure, /*oldhead,*/ *insfig;
    locon_list     *LoCon, *lc;
   loins_list     *LoIns;
    losig_list     *LoSig;
    cgvcon_list    *CgvCon;
    cgvbox_list    *CgvBox;
    cgvnet_list    *CgvNet;
    cgvfig_list    *CgvFigure;
    ptype_list     *PType;
    unsigned char   CgvDir;
    int scale, oldloadparasiticstate;
//    ht *oldht;
    lotrs_list *tr;
    char temp[100], *n, *dn, *sn;
    locon_list *g, *d, *s;
    chain_list *cl;
    lofig_list *temp0;


    CgvFigure=getcgvfile(Name, CGV_SOURCE_LOFIG, 0, filename);
    CGV_WAS_PRESENT=1;
    if (CgvFigure!=NULL) return CgvFigure;
  
    CGV_WAS_PRESENT=0;
    if (filename!=NULL) temp0=NULL;
    else temp0=getloadedlofig(Name);
    if (temp0==NULL)
      {
        char *suffix;
/*        oldhead=HEAD_LOFIG; oldht=HT_LOFIG;
        HEAD_LOFIG=NULL; HT_LOFIG=NULL;*/
//        addlocked(oldhead);
        suffix=IN_LO;
        if (strcmp(IN_LO,"v")==0 || strcmp(IN_LO,"vlg")==0)
          {
            suffix = V_STR_TAB[__MGL_FILE_SUFFIX].VALUE;
            if (suffix == NULL) suffix = namealloc("v");
          }
        else if (strcmp(IN_LO,"vhd")==0 || strcmp(IN_LO,"vst")==0)
          {
            suffix = V_STR_TAB[__MVL_FILE_SUFFIX].VALUE;
            if (suffix == NULL) suffix = IN_LO;
          }
        if (filepath(Name, suffix)==NULL) return NULL;
        oldloadparasiticstate=MBK_LOAD_PARA;
        MBK_LOAD_PARA=1;
        LoFigure = getlofig(Name, 'A');
        MBK_LOAD_PARA=oldloadparasiticstate;
      }
    else
      {
        LoFigure=temp0;
      }

    lofigchain(LoFigure);

    if (CGV_LIB || SLIB_LIBRARY) 
      { 
	if (SLIB_LIBRARY)  scale = SLIB_LIBRARY->GRID;
	else scale = CGV_LIB->GRID;
      }
    else
      scale = 1024;

//    printf("scale=%d\n",scale);
    CgvFigure = addcgvfig(LoFigure->NAME,scale);
    CgvFigure->SOURCE = (void *) LoFigure;
    CgvFigure->SOURCE_TYPE = CGV_SOURCE_LOFIG;
    CgvFigure->data0=CgvFigure->data1=CgvFigure->data2=CgvFigure->data_ZOOM=NULL;

   if (CGV_LIB || SLIB_LIBRARY) { 
     if (SLIB_LIBRARY)
       {
        CgvFigure->LIBRARY = slib_create_lib(LoFigure->NAME);
        CgvFigure->LIBRARY->REFHEIGHT = SLIB_LIBRARY->REFHEIGHT; 
       }
     else
       {
	 CgvFigure->LIBRARY = slib_create_lib(LoFigure->NAME);
//	 CgvFigure->LIBRARY->GRID = CGV_LIB->GRID;
	 CgvFigure->LIBRARY->REFHEIGHT = CGV_LIB->REFHEIGHT; 
       }
     CgvFigure->LIBRARY->GRID = scale;
    }

    for (LoSig = LoFigure->LOSIG; LoSig; LoSig = LoSig->NEXT) {
	  CgvNet = addcgvnet(CgvFigure);
	  CgvNet->SOURCE = (void *) LoSig;
	  CgvNet->SOURCE_TYPE = CGV_SOURCE_LOSIG;
	  if (LoSig->NAMECHAIN!=NULL)
	    CgvNet->NAME=(char *)LoSig->NAMECHAIN->DATA;
	  else
	    CgvNet->NAME=namealloc("?");
	  LoSig->USER = addptype(LoSig->USER, CGV_NET_TYPE, CgvNet);
    }

    for (LoIns = LoFigure->LOINS; LoIns; LoIns = LoIns->NEXT) 
      {
	insfig=getloadedlofig(LoIns->FIGNAME);
//	printf("%s %s\n",LoIns->INSNAME, LoIns->FIGNAME);
	if (insfig!=NULL)
	  {
	    locon_list *lc0, *lc1;
	    for (lc0=LoIns->LOCON; lc0; lc0=lc0->NEXT)
	      {
		if (lc0->DIRECTION==UNKNOWN)                  
		  {
                    for (lc1=insfig->LOCON; lc1!=NULL && lc1->NAME!=lc0->NAME; lc1=lc1->NEXT) ;
                    if (lc0->NAME==NULL) continue;
                      
//		    printf("    %s %s '%c' '%c'\n",lc0->NAME, lc1->NAME, lc0->DIRECTION, lc1->DIRECTION);
		    lc0->DIRECTION=lc1->DIRECTION;
		  }
	      }
	  }
      }


    for (LoCon = LoFigure->LOCON; LoCon; LoCon = LoCon->NEXT) {
        if (!((mbk_LosigIsVDD(LoCon->SIG)) || (mbk_LosigIsVSS(LoCon->SIG)))) {
	  
	  int nbin=0, nbout=0, hastran=0;
	  if (LoCon->DIRECTION == IN)
	    CgvDir = CGV_CON_IN;
	  else
	    CgvDir = CGV_CON_OUT;
	  
	    if (LoCon->DIRECTION==UNKNOWN && LoFigure->LOTRS)
	      {
		PType=getptype(LoCon->SIG->USER, LOFIGCHAIN);
		for (cl=(chain_list *)PType->DATA; cl!=NULL; cl=cl->NEXT)
		  {
		    lc=(locon_list *)cl->DATA;
		    if (lc->TYPE=='T')
		      {
			if (lc->NAME!=MBK_GRID_NAME) nbout++;
			else nbin++;
			hastran++;
		      }
		    else 
		      {
			if (lc->DIRECTION==IN) nbin++;
			else nbout++;
		      }		      
		  }
		if (hastran)
		  {
		    if (nbin!=0) CgvDir = CGV_CON_IN;
		  }
		else
		  {
		    if (nbin!=0 && nbout==0) CgvDir = CGV_CON_IN;
		    else if (nbin==0 && nbout!=0) CgvDir = CGV_CON_OUT;
		  }
	      }

            CgvCon = addcgvfigcon(CgvFigure, LoCon->NAME, CgvDir);
            CgvCon->SOURCE = (void *) LoCon;
            CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
            LoSig = LoCon->SIG;
            PType = getptype(LoSig->USER, CGV_NET_TYPE);
            CgvNet = (cgvnet_list *) PType->DATA;
            addcgvnetcon(CgvNet, CgvCon);
        }
    }

    for (LoIns = LoFigure->LOINS; LoIns; LoIns = LoIns->NEXT) {
        CgvBox = addcgvbox(CgvFigure, LoIns->INSNAME);
        CgvBox->SOURCE = (void *) LoIns;
        CgvBox->SOURCE_TYPE = CGV_SOURCE_LOINS;

	n=getcorrespgate(LoIns->FIGNAME);
//        printf("%s is %s\n",LoIns->FIGNAME, n!=NULL?n:"?");
        if (n==NULL) n=LoIns->FIGNAME;
        CgvBox->SYMBOL = cgv_getlofigcellsymbol(CgvFigure->LIBRARY, n);

	if (CgvBox->SYMBOL==NULL)
          CgvBox->SYMBOL = cgv_getselfcellsymbol(CgvFigure->LIBRARY, n);
//	printf("i:%s\n",CgvBox->NAME);
        for (LoCon = LoIns->LOCON; LoCon; LoCon = LoCon->NEXT) {
            if (!((mbk_LosigIsVDD(LoCon->SIG)) || (mbk_LosigIsVSS(LoCon->SIG)))) {
                if (LoCon->DIRECTION == IN)
                    CgvDir = CGV_CON_IN;
                else
                    CgvDir = CGV_CON_OUT;

                CgvCon = addcgvboxcon(CgvFigure, CgvBox, LoCon->NAME, CgvDir);
                CgvCon->SOURCE = (void *) LoCon;
                CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
                if (n!=LoIns->FIGNAME)
                  {
                    if (CgvBox->SYMBOL)
                      CgvCon->SUB_NAME=getcorrespgatepin(LoIns->FIGNAME, LoCon->NAME);
                    else
                      avt_errmsg(CGV_ERRMSG, "002", AVT_WARNING, 2);
                      // printf("warning: can find corresponding pin for %s.%s\n",LoIns->FIGNAME,LoCon->NAME);
                  }
                LoSig = LoCon->SIG;
                PType = getptype(LoSig->USER, CGV_NET_TYPE);
                CgvNet = (cgvnet_list *) PType->DATA;
                addcgvnetcon(CgvNet, CgvCon);
//		printf("\t%s => %s | %s => %s\n",CgvCon->NAME, CgvNet->NAME, LoCon->NAME, LoSig->NAMECHAIN->DATA);
            }
        }
    }

    for (tr = LoFigure->LOTRS; tr!=NULL; tr = tr->NEXT) 
      {
	if (tr->TRNAME!=NULL)
	  CgvBox = addcgvbox(CgvFigure, tr->TRNAME);
	else
	  CgvBox = addcgvbox(CgvFigure, namealloc("?"));
			     
	CgvBox->SOURCE = (void *) tr;
        CgvBox->SOURCE_TYPE = CGV_SOURCE_LOTRS;
	
	if (MLO_IS_TRANSN(tr->TYPE)) strcpy(temp,"tn");
	else strcpy(temp,"tp");

	s=tr->SOURCE; // sn=(char *)s->SIG->NAMECHAIN->DATA;
	d=tr->DRAIN; // dn=(char *)d->SIG->NAMECHAIN->DATA;
	g=tr->GRID;
	
	if (!mbk_LosigIsVSS(d->SIG) && !mbk_LosigIsVDD(d->SIG))
	  {
	    CgvCon = addcgvboxcon(CgvFigure, CgvBox, d->NAME, CGV_CON_OUT);
	    CgvCon->SOURCE = (void *) d;
	    CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
	    LoSig = d->SIG;
	    PType = getptype(LoSig->USER, CGV_NET_TYPE);
	    if (PType!=NULL)
	      CgvNet = (cgvnet_list *) PType->DATA;
	    else 
	      {
		CgvNet = addcgvnet(CgvFigure);
		CgvNet->SOURCE = (void *) d->SIG;
		CgvNet->SOURCE_TYPE = CGV_SOURCE_LOSIG;
		if (d->SIG->NAMECHAIN!=NULL)
		  CgvNet->NAME=(char *)d->SIG->NAMECHAIN->DATA;
		else
		  CgvNet->NAME=namealloc("?");
	      }
	    addcgvnetcon(CgvNet, CgvCon);
	    strcat(temp,"_n");
	  }
	else
	  {
	    if (mbk_LosigIsVSS(d->SIG)) strcat(temp,"_vss");
	    else strcat(temp,"_vdd");
	  }
	
	if (!mbk_LosigIsVSS(s->SIG) && !mbk_LosigIsVDD(s->SIG))
	  {
	    CgvCon = addcgvboxcon(CgvFigure, CgvBox, s->NAME, CGV_CON_OUT);
	    CgvCon->SOURCE = (void *) s;
	    CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
	    LoSig = s->SIG;
	    PType = getptype(LoSig->USER, CGV_NET_TYPE);
	    if (PType!=NULL)
	      CgvNet = (cgvnet_list *) PType->DATA;
	    else 
	      {
		CgvNet = addcgvnet(CgvFigure);
		CgvNet->SOURCE = (void *) s->SIG;
		CgvNet->SOURCE_TYPE = CGV_SOURCE_LOSIG;
		if (s->SIG->NAMECHAIN!=NULL)
		  CgvNet->NAME=(char *)s->SIG->NAMECHAIN->DATA;
		else
		  CgvNet->NAME=namealloc("?");
	      }
	    addcgvnetcon(CgvNet, CgvCon);
	   strcat(temp,"_n");
	  }
	else
	  {
	    if (mbk_LosigIsVSS(s->SIG)) strcat(temp,"_vss");
	    else strcat(temp,"_vdd");
	  }

        if (strcmp(&temp[2],"_vss_n")==0) 
          { 
            strcpy(&temp[2],"_n_vss");
            CgvCon->NAME=d->NAME;
          }
        else
           if (strcmp(&temp[2],"_n_vdd")==0) 
             { 
               strcpy(&temp[2],"_vdd_n");
               CgvCon->NAME=s->NAME;
             }

	CgvCon = addcgvboxcon(CgvFigure, CgvBox, g->NAME, CGV_CON_IN);
	CgvCon->SOURCE = (void *) g;
	CgvCon->SOURCE_TYPE = CGV_SOURCE_LOCON;
	LoSig = g->SIG;
	PType = getptype(LoSig->USER, CGV_NET_TYPE);
	if (PType!=NULL)
	  CgvNet = (cgvnet_list *) PType->DATA;
	else 
	  {
	    CgvNet = addcgvnet(CgvFigure);
	    CgvNet->SOURCE = (void *) g->SIG;
	    CgvNet->SOURCE_TYPE = CGV_SOURCE_LOSIG;
	    if (g->SIG->NAMECHAIN!=NULL)
	      CgvNet->NAME=(char *)g->SIG->NAMECHAIN->DATA;
	    else
	      CgvNet->NAME=namealloc("?");
	  }
	addcgvnetcon(CgvNet, CgvCon);

	CgvBox->SYMBOL = cgv_getselfcellsymbol(CgvFigure->LIBRARY, namealloc(temp));
//	printf("%s %p\n",temp,CgvBox->SYMBOL);
      }
    
    for (LoSig = LoFigure->LOSIG; LoSig; LoSig = LoSig->NEXT) {
      if (getptype(LoSig->USER, CGV_NET_TYPE)!=NULL)
      LoSig->USER = delptype(LoSig->USER, CGV_NET_TYPE);
    }

    addcgvfile(LoFigure->NAME, CGV_SOURCE_LOFIG, 0, filename, CgvFigure, LoFigure, NULL, temp0!=NULL);

    if (temp0==NULL)
      {
/*        delht(HT_LOFIG);
        HEAD_LOFIG=oldhead; HT_LOFIG=oldht;*/
      }
    buildcgvfig(CgvFigure);
    strcpy(getcgvfileext(CgvFigure), IN_LO);
    return (CgvFigure);
}
