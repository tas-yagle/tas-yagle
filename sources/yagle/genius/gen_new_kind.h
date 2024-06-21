typedef struct ExpandableArray
{
  int StartIndex;
  int EndIndex;
  void **Array;
} ExpandableArray;

typedef void (*ExpandableArrayUpdateFunc)(void **arraypos);
extern void EAHeap_Manage(int mode);
extern ExpandableArray *NewExpandableArray();
extern void DeleteExpandableArray(ExpandableArray *ea);
extern void **GetExpandableArrayItem(ExpandableArray *ea, int index, ExpandableArrayUpdateFunc eaupfunc);
