#ifndef __GLIB_INIT_H__
#define __GLIB_INIT_H__

#include "gmessages.h"

extern GLogLevelFlags g_log_always_fatal;
extern GLogLevelFlags g_log_msg_prefix;

void glib_init (void);
void g_quark_init (void);



#endif