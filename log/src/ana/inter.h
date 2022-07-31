/* Header for module INTER, generated by p2c */
#ifndef INTER_H
#define INTER_H


#ifndef LOGDEF_H
#include "logdef.h"
#endif

#ifndef NEWKBD_H
#include <p2c/newkbd.h>
#endif

#ifndef SYSGLOBALS_H
#include <p2c/sysglobals.h>
#endif

#ifndef NEWASM_H
#include <p2c/newasm.h>
#endif

#ifndef MYLIB_H
#include <p2c/mylib.h>
#endif




#ifndef MAIN_H
#include "main.h"
#endif

#ifndef GLOBALS_H
#include "globals.h"
#endif

#ifndef NSOLVER_H
#include "nsolver.h"
#endif

#ifndef NEWCI_H
#include <p2c/newci.h>
#endif


#ifdef INTER_G
# define vextern
#else
# define vextern extern
#endif

/*$If Float$
$float_hdw on$
$End$*/
/*$If Debugflag$
$Debug$
$End$*/


extern Void Log_32_proc PP((log_action *act));


extern Void Log_33_proc PP((log_action *act));


#undef vextern

#endif /*INTER_H*/

/* End. */