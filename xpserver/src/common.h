/* common variables to all routines */

/* we use the Version 2 API */
#define XPLM200 1

/* print messages to log file in X-Plane root path: xpserver.log */
/* 0 (no output), 1 (some output) or 2 (pretty much output) */
extern int verbose;

/* log file descriptor */
extern FILE *logfileptr;     


/* missing values: UCHAR has no missing value. FLT and DBL are the same */
#define INT_MISS -2000000000
#define FLT_MISS -2000000000.0
#define DBL_MISS -2000000000.0

/* x-plane variable types: */
#define XPTYPE_UNKNOWN            0 // unknown 
#define XPTYPE_INT                1 // int
#define XPTYPE_FLT                2 // float
#define XPTYPE_DBL                4 // double (2+4=6 can concur!)
#define XPTYPE_FLT_ARR            8 // float array
#define XPTYPE_INT_ARR           16 // int array
#define XPTYPE_BYTE_ARR          32 // byte/char data
#define XPTYPE_CMD_ONCE          64 // one-shot command
#define XPTYPE_CMD_HOLD         128 // on/off command

/* clientdata element status */
#define XPSTATUS_UNINITIALIZED    0 // clientdata structure for dataref uninitialized in server
#define XPSTATUS_ALLOC            1 // clientdata memory for dataref allocated in server
#define XPSTATUS_DEALLOC          2 // clientdata memory for dataref deallocated in server
#define XPSTATUS_LINK             3 // client sent dataref link request to x-plane
#define XPSTATUS_UNLINK           4 // client sent dataref unlink request to x-plane
#define XPSTATUS_VALID            5 // x-plane confirmed dataref validity to client

