#include AVT_H
#include GEN_H
#include MLU_H


void gen_update_transistors (lofig_list *fig, corresp_t *table)
{
    lotrs_list *tr, *assoc_tr;
    ptype_list *pt, *assocpt;

    for (tr = fig->LOTRS; tr; tr = tr->NEXT) {
        if ((assoc_tr = gen_corresp_trs (tr->TRNAME, table))) {
            tr->WIDTH    = assoc_tr->WIDTH;
            tr->LENGTH   = assoc_tr->LENGTH;
            tr->PS       = assoc_tr->PS;
            tr->PD       = assoc_tr->PD;    
            tr->XS       = assoc_tr->XS;
            tr->XD       = assoc_tr->XD;    
            tr->X        = assoc_tr->X;
            tr->Y        = assoc_tr->Y;     
            tr->MODINDEX = assoc_tr->MODINDEX;
            tr->TYPE     = assoc_tr->TYPE;
            assocpt=getptype ( assoc_tr->USER, TRANS_FIGURE );
            pt=getptype ( tr->USER, TRANS_FIGURE );
            if (pt!=NULL && assocpt==NULL)
              tr->USER=delptype(tr->USER, TRANS_FIGURE );
            else if (assocpt!=NULL && pt==NULL)
              tr->USER=addptype(tr->USER, TRANS_FIGURE, assocpt->DATA );
            else
              if (pt!=NULL && assocpt!=NULL) pt->DATA=assocpt->DATA;
            
            if (getptype (tr->USER, PARAM_CONTEXT)==NULL && (pt = getptype (assoc_tr->USER, PARAM_CONTEXT)) != NULL)
            {
              tr->USER = addptype (tr->USER, PARAM_CONTEXT, eqt_dupvars((eqt_param *) pt->DATA));
            }
        }
        else 
            avt_errmsg(GNS_ERRMSG, "038", AVT_WARNING, tr->TRNAME);
            //avt_error ("GNS", 0, AVT_WAR, "no corresponding transistor for %s\n", tr->TRNAME);
    }
}

//_____________________________________________________________________________

void gen_update_loins (lofig_list *fig, corresp_t *table)
{
  subinst_t *sins;
  ht *tempht;
  loins_list *li;

  tempht=addht(512);
  for (li=fig->LOINS; li!=NULL; li=li->NEXT)
    addhtitem(tempht, li->INSNAME, (long)li);

  for (sins=table->SUBINSTS; sins!=NULL; sins=sins->NEXT)
    {
      li=(loins_list *)gethtitem(tempht, sins->INSNAME);
      if ((long)li==EMPTYHT)
        {
          avt_errmsg(GNS_ERRMSG, "005", AVT_WARNING, 8);
          /*avt_error ("GNS", 0, AVT_WAR, 
                  "gen_update_loins called with wrong correspondance table. Instance '%s' not found in model '%s'\n",
                  sins->INSNAME, fig->NAME);*/
          delht(tempht);
          return;
        }
      li->FIGNAME=sins->CRT->GENIUS_FIGNAME;
    }
  delht(tempht);
}

lofig_list *__gns_GetNetlist ()
{
  gen_update_loins (CUR_HIER_LOFIG, CUR_CORRESP_TABLE);
  gen_update_transistors (CUR_HIER_LOFIG, CUR_CORRESP_TABLE);
  return CUR_HIER_LOFIG;
}
