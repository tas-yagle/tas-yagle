#include MUT_H
#include MLO_H
#include MLU_H
#include BEG_H
#include ELP_H

#include API_H

#ifdef AVERTEC
#include AVT_H
#endif


#include GEN_H
#include "gen_env.h"
#include "gen_model_utils.h"
#include "gen_model_global.h"
#include "gen_verif_global.h"
#include "gen_search_global.h"
#include "gen_search_utils.h"
#include "gen_new_kind.h"
#include "gen_main.h"
#include "gen_display.h"
#include "gen_globals.h"

extern ht *gns_create_template_hierarchy(lib_entry *files_list);
extern tree_list *Read_All(lib_entry *);

int main()
{
  int err;
  ptype_list *bi_list;

  avtenv();
  mbkenv();
  fclenv();

  elpenv();
//  mccenv();

  elpLoadOnceElp();

  GEN_DEBUG_LEVEL=1;

  genius_ec=APINewExecutionContext();

  APIInit();

  genius_env(NULL, NULL, NULL);

  GEN_DEBUG_LEVEL=1;

  GENIUS_PRIORITY = APIReadLibrary(GENIUS_LIB_NAME,GENIUS_LIB_PATH, stdout);
  err = Get_Error();
  if (err) 
    {
      avt_errmsg(GNS_ERRMSG, "039", AVT_ERR, err);
      //fprintf(stderr,"*** %d error(s) detected,  I can't get farther!! ***\n",err);
      return err;
    }
  if (!GENIUS_PRIORITY) return 0;
   
  LoadSpiceFCLFiles(GENIUS_PRIORITY);

  GNS_TEMPLATE_HT=gns_create_template_hierarchy(GENIUS_PRIORITY);

  APIParseFile_SetTemplateInfo(GNS_TEMPLATE_HT);

  GENIUS_TREE = Read_All(GENIUS_PRIORITY);  /* ----> GENIUS_TREE */

  APIParseFile_SetTemplateInfo(NULL);

  if (!GENIUS_TREE) return 0;
  APIVerify_C_Functions(genius_ec);

  err = Get_Error();
  if (err)
    {
      avt_errmsg(GNS_ERRMSG, "039", AVT_ERR, err);
      //fprintf(stderr,"*** %d error(s) detected,  I can't get farther!! ***\n",err);
      return err;
    }

  Build_All_Transistor_Models();
  
  /*get a tree_list for GENIUS and FCL */
  bi_list=Verif_All(GENIUS_TREE);
  
  freeptype(bi_list);
  err = Get_Error();   
  if (err) 
    {
      avt_errmsg(GNS_ERRMSG, "039", AVT_ERR, err);
      //fprintf(stderr,"\n%d error(s) detected,  I can't get farther!!\n",err);
      return err;
    }
  
  if (genius_ec->ALL_USED_FUNCTIONS!=NULL)
    {
      LoadDynamicLibraries(stdout);
      
      if (APICheckCFunctions(genius_ec)) return 1;
    }
  
  return 0;
}
