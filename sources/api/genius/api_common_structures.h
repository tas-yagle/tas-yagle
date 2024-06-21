typedef struct
{
  float ctk_ratio;
} ctk_VictimVSAgressorParam;

typedef struct
{
  ht_v2 *agressor_ctk_info;
} ctk_PerVictimParam;

typedef struct
{
  int crosstalk_to_ground;
  double crosstalk_to_ground_ratio;
  double external_crosstalk_to_ground_ratio;
  ht_v2 *victim_ctk_info;
} ctk_struct;
