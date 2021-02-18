
typedef struct {
  size_t    SIZE ;
  int       NB ;
  int       SIZENAME ;
  size_t    SIZERCXNAME ;
  int       SIZEPTYPE ;
  int       SIZEPNODENAME ;
} sizelocon ;

typedef struct {
  size_t    SIZE  ;
  int       NB ;
  int       SIZENAME ;
  int       SIZEPTYPE ;
  sizelocon LOCON ;
} sizelotrs ;

typedef struct {
  sizelotrs   SIZELOTRS ;
} sizecns ;
