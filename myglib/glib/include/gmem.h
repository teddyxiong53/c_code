#ifndef __G_MEM_H__
#define __G_MEM_H__

#include "gutils.h"

typedef struct _GMemVTable GMemVTable;

#  define G_MEM_ALIGN	GLIB_SIZEOF_LONG

void	 g_free	          (gpointer	 mem);

void     g_clear_pointer  (gpointer      *pp,
                           GDestroyNotify destroy);


GLIB_AVAILABLE_IN_ALL
gpointer g_malloc         (gsize	 n_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE(1);
GLIB_AVAILABLE_IN_ALL
gpointer g_malloc0        (gsize	 n_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE(1);
GLIB_AVAILABLE_IN_ALL
gpointer g_realloc        (gpointer	 mem,
			   gsize	 n_bytes) G_GNUC_WARN_UNUSED_RESULT;
GLIB_AVAILABLE_IN_ALL
gpointer g_try_malloc     (gsize	 n_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE(1);
GLIB_AVAILABLE_IN_ALL
gpointer g_try_malloc0    (gsize	 n_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE(1);
GLIB_AVAILABLE_IN_ALL
gpointer g_try_realloc    (gpointer	 mem,
			   gsize	 n_bytes) G_GNUC_WARN_UNUSED_RESULT;

GLIB_AVAILABLE_IN_ALL
gpointer g_malloc_n       (gsize	 n_blocks,
			   gsize	 n_block_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE2(1,2);
GLIB_AVAILABLE_IN_ALL
gpointer g_malloc0_n      (gsize	 n_blocks,
			   gsize	 n_block_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE2(1,2);
GLIB_AVAILABLE_IN_ALL
gpointer g_realloc_n      (gpointer	 mem,
			   gsize	 n_blocks,
			   gsize	 n_block_bytes) G_GNUC_WARN_UNUSED_RESULT;
GLIB_AVAILABLE_IN_ALL
gpointer g_try_malloc_n   (gsize	 n_blocks,
			   gsize	 n_block_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE2(1,2);
GLIB_AVAILABLE_IN_ALL
gpointer g_try_malloc0_n  (gsize	 n_blocks,
			   gsize	 n_block_bytes) G_GNUC_MALLOC G_GNUC_ALLOC_SIZE2(1,2);
GLIB_AVAILABLE_IN_ALL
gpointer g_try_realloc_n  (gpointer	 mem,
			   gsize	 n_blocks,
			   gsize	 n_block_bytes) G_GNUC_WARN_UNUSED_RESULT;


#define g_clear_pointer(pp, destroy) \
  G_STMT_START {                                                               \
    G_STATIC_ASSERT (sizeof *(pp) == sizeof (gpointer));                       \
    /* Only one access, please */                                              \
    gpointer *_pp = (gpointer *) (pp);                                         \
    gpointer _p;                                                               \
    /* This assignment is needed to avoid a gcc warning */                     \
    GDestroyNotify _destroy = (GDestroyNotify) (destroy);                      \
                                                                               \
    _p = *_pp;                                                                 \
    if (_p) 								       \
      { 								       \
        *_pp = NULL;							       \
        _destroy (_p);                                                         \
      }                                                                        \
  } G_STMT_END


static inline gpointer
g_steal_pointer (gpointer pp)
{
  gpointer *ptr = (gpointer *) pp;
  gpointer ref;

  ref = *ptr;
  *ptr = NULL;

  return ref;
}

#define g_steal_pointer(pp) \
  (0 ? (*(pp)) : (g_steal_pointer) (pp))

#  define _G_NEW(struct_type, n_structs, func) \
	(struct_type *) (G_GNUC_EXTENSION ({			\
	  gsize __n = (gsize) (n_structs);			\
	  gsize __s = sizeof (struct_type);			\
	  gpointer __p;						\
	  if (__s == 1)						\
	    __p = g_##func (__n);				\
	  else if (__builtin_constant_p (__n) &&		\
	           (__s == 0 || __n <= G_MAXSIZE / __s))	\
	    __p = g_##func (__n * __s);				\
	  else							\
	    __p = g_##func##_n (__n, __s);			\
	  __p;							\
	}))
#  define _G_RENEW(struct_type, mem, n_structs, func) \
	(struct_type *) (G_GNUC_EXTENSION ({			\
	  gsize __n = (gsize) (n_structs);			\
	  gsize __s = sizeof (struct_type);			\
	  gpointer __p = (gpointer) (mem);			\
	  if (__s == 1)						\
	    __p = g_##func (__p, __n);				\
	  else if (__builtin_constant_p (__n) &&		\
	           (__s == 0 || __n <= G_MAXSIZE / __s))	\
	    __p = g_##func (__p, __n * __s);			\
	  else							\
	    __p = g_##func##_n (__p, __n, __s);			\
	  __p;							\
	}))



#define g_new(struct_type, n_structs)			_G_NEW (struct_type, n_structs, malloc)


#define g_new0(struct_type, n_structs)			_G_NEW (struct_type, n_structs, malloc0)

#define g_renew(struct_type, mem, n_structs)		_G_RENEW (struct_type, mem, n_structs, realloc)


#define g_try_new(struct_type, n_structs)		_G_NEW (struct_type, n_structs, try_malloc)

#define g_try_new0(struct_type, n_structs)		_G_NEW (struct_type, n_structs, try_malloc0)

#define g_try_renew(struct_type, mem, n_structs)	_G_RENEW (struct_type, mem, n_structs, try_realloc)


struct _GMemVTable {
  gpointer (*malloc)      (gsize    n_bytes);
  gpointer (*realloc)     (gpointer mem,
			   gsize    n_bytes);
  void     (*free)        (gpointer mem);
  /* optional; set to NULL if not used ! */
  gpointer (*calloc)      (gsize    n_blocks,
			   gsize    n_block_bytes);
  gpointer (*try_malloc)  (gsize    n_bytes);
  gpointer (*try_realloc) (gpointer mem,
			   gsize    n_bytes);
};



void	 g_mem_set_vtable (GMemVTable	*vtable);

gboolean g_mem_is_system_malloc (void);

GLIB_VAR gboolean g_mem_gc_friendly;


GLIB_VAR GMemVTable	*glib_mem_profiler_table;

void	g_mem_profile	(void);















#endif