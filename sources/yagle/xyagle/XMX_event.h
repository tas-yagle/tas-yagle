/*------------------------------------------------------------\
|                                                             |
| Tool    :                   XYAG                            |
|                                                             |
| File    :                   Event.h                         |
|                                                             |
| Authors :              Jacomme Ludovic                      |
|                        Picault Stephane                     |
|                        Lester Anthony                       |
|                                                             |
\------------------------------------------------------------*/

# ifndef XYAG_EVENT
# define XYAG_EVENT

/*------------------------------------------------------------\
|                           Constants                         |
\------------------------------------------------------------*/

# define XYAG_B1DN         0
# define XYAG_B3DN         1
# define XYAG_MOTION       2
# define XYAG_ENTER        3
# define XYAG_LEAVE        4

# define XYAG_KEY_UP       5
# define XYAG_KEY_DN       6
# define XYAG_KEY_LEFT     7
# define XYAG_KEY_RIGHT    8

# define XYAG_KEY_BACKSPACE    9


# define XYAG_MAX_EVENT_BUFFER 128

/*------------------------------------------------------------\
|                          Functions                          |
\------------------------------------------------------------*/
  
void XyagInitializeEvent();
void XyagDisplayEditMeasure();

# endif 
