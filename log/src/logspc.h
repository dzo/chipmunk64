/* Header for module logspc, generated by p2c */
#ifndef LOGSPC_H
#define LOGSPC_H




#ifndef LOGDEF_H
#include "logdef.h"
#endif

#ifndef LOGHIER_H
#include "loghier.h"
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

#ifndef NEWCRT_H
#include <p2c/newcrt.h>
#endif

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif

#ifndef REGEX_H
#include <p2c/regex.h>
#endif


#ifdef LOGSPC_G
# define vextern
#else
# define vextern extern
#endif






extern Void Log_logspc_proc PP((log_action *act));


#undef vextern

#endif /*LOGSPC_H*/

/* End. */