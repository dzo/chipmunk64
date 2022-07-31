/* Header for module logcom, generated by p2c */
#ifndef LOGCOM_H
#define LOGCOM_H



#ifndef LOGDEF_H
#include "logdef.h"
#endif

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

#ifndef NEWASM_H
#include <p2c/newasm.h>
#endif

#ifndef NEWKBD_H
#include <p2c/newkbd.h>
#endif

#ifndef NEWCI_H
#include <p2c/newci.h>
#endif

#ifndef SYSDEVS_H
#include <p2c/sysdevs.h>
#endif


#ifdef LOGCOM_G
# define vextern
#else
# define vextern extern
#endif



extern Void Log_0_proc PP((log_action *act));
extern Void Log_1_proc PP((log_action *act));
/*obsolete*/
extern Void Log_2_proc PP((log_action *act));
extern Void Log_7_proc PP((log_action *act));

extern Void Log_17_proc PP((log_action *act));



extern Void Log_7_time PP((log_action *act));


#undef vextern

#endif /*LOGCOM_H*/

/* End. */
