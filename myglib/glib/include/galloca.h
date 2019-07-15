#ifndef __G_ALLOCA_H__
#define __G_ALLOCA_H__

#include "gtypes.h"

# undef alloca
# define alloca(size)   __builtin_alloca (size)

#define g_alloca(size)		 alloca (size)

#define g_newa(struct_type, n_structs)	((struct_type*) g_alloca (sizeof (struct_type) * (gsize) (n_structs)))


#endif