

losig_list* rcx_loinsaddlosig     ( void );
locon_list* addrcxlocon           ( void );
void        freercxlocon          ( locon_list* );
void        rcx_cleanloins        ( loins_list* );
losig_list* rcx_loinsgetvssni     ( loins_list* );


float rcx_getloinsresi( locon_list *locon );
void rcx_getloinscapa( locon_list *locon, float *cm, float *ccl, float *ccg );
