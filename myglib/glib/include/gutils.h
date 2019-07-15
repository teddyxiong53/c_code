#ifndef __G_UTILS_H__
#define __G_UTILS_H__

#include "gtypes.h"

#include <stdarg.h>

GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_user_name        (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_real_name        (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_home_dir         (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_tmp_dir          (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_host_name	     (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_prgname          (void);
GLIB_AVAILABLE_IN_ALL
void                  g_set_prgname          (const gchar *prgname);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_application_name (void);
GLIB_AVAILABLE_IN_ALL
void                  g_set_application_name (const gchar *application_name);

GLIB_AVAILABLE_IN_ALL
void      g_reload_user_special_dirs_cache     (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_user_data_dir      (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_user_config_dir    (void);
GLIB_AVAILABLE_IN_ALL
const gchar *         g_get_user_cache_dir     (void);
GLIB_AVAILABLE_IN_ALL
const gchar * const * g_get_system_data_dirs   (void);

GLIB_AVAILABLE_IN_ALL
const gchar * const * g_get_system_config_dirs (void);

GLIB_AVAILABLE_IN_ALL
const gchar * g_get_user_runtime_dir (void);

typedef enum {
  G_USER_DIRECTORY_DESKTOP,
  G_USER_DIRECTORY_DOCUMENTS,
  G_USER_DIRECTORY_DOWNLOAD,
  G_USER_DIRECTORY_MUSIC,
  G_USER_DIRECTORY_PICTURES,
  G_USER_DIRECTORY_PUBLIC_SHARE,
  G_USER_DIRECTORY_TEMPLATES,
  G_USER_DIRECTORY_VIDEOS,

  G_USER_N_DIRECTORIES
} GUserDirectory;

GLIB_AVAILABLE_IN_ALL
const gchar * g_get_user_special_dir (GUserDirectory directory);

typedef struct _GDebugKey GDebugKey;
struct _GDebugKey
{
  const gchar *key;
  guint	       value;
};


GLIB_AVAILABLE_IN_ALL
guint                 g_parse_debug_string (const gchar     *string,
					    const GDebugKey *keys,
					    guint            nkeys);

GLIB_AVAILABLE_IN_ALL
gint                  g_snprintf           (gchar       *string,
					    gulong       n,
					    gchar const *format,
					    ...) G_GNUC_PRINTF (3, 4);
GLIB_AVAILABLE_IN_ALL
gint                  g_vsnprintf          (gchar       *string,
					    gulong       n,
					    gchar const *format,
					    va_list      args)
					    G_GNUC_PRINTF(3, 0);

GLIB_AVAILABLE_IN_ALL
void                  g_nullify_pointer    (gpointer    *nullify_location);


typedef enum
{
  G_FORMAT_SIZE_DEFAULT     = 0,
  G_FORMAT_SIZE_LONG_FORMAT = 1 << 0,
  G_FORMAT_SIZE_IEC_UNITS   = 1 << 1
} GFormatSizeFlags;


gchar *g_format_size_full   (guint64          size,
                             GFormatSizeFlags flags);

gchar *g_format_size        (guint64          size);


gchar *g_format_size_for_display (goffset size);

typedef void (*GVoidFunc) (void);
#define ATEXIT(proc) g_ATEXIT(proc)

//void	g_atexit		(GVoidFunc    func);

#define g_atexit(func) atexit(func)
gchar*  g_find_program_in_path  (const gchar *program);


#define g_bit_nth_lsf(mask, nth_bit) g_bit_nth_lsf_impl(mask, nth_bit)
#define g_bit_nth_msf(mask, nth_bit) g_bit_nth_msf_impl(mask, nth_bit)
#define g_bit_storage(number)        g_bit_storage_impl(number)

GLIB_AVAILABLE_IN_ALL
gint    (g_bit_nth_lsf)         (gulong mask,
                                 gint   nth_bit);
GLIB_AVAILABLE_IN_ALL
gint    (g_bit_nth_msf)         (gulong mask,
                                 gint   nth_bit);
GLIB_AVAILABLE_IN_ALL
guint   (g_bit_storage)         (gulong number);

static inline gint
g_bit_nth_lsf_impl (gulong mask,
                    gint   nth_bit)
{
  if (G_UNLIKELY (nth_bit < -1))
    nth_bit = -1;
  while (nth_bit < ((GLIB_SIZEOF_LONG * 8) - 1))
    {
      nth_bit++;
      if (mask & (1UL << nth_bit))
        return nth_bit;
    }
  return -1;
}

static inline gint
g_bit_nth_msf_impl (gulong mask,
                    gint   nth_bit)
{
  if (nth_bit < 0 || G_UNLIKELY (nth_bit > GLIB_SIZEOF_LONG * 8))
    nth_bit = GLIB_SIZEOF_LONG * 8;
  while (nth_bit > 0)
    {
      nth_bit--;
      if (mask & (1UL << nth_bit))
        return nth_bit;
    }
  return -1;
}

static inline guint
g_bit_storage_impl (gulong number)
{
#if defined(__GNUC__) && (__GNUC__ >= 4) && defined(__OPTIMIZE__)
  return G_LIKELY (number) ?
           ((GLIB_SIZEOF_LONG * 8U - 1) ^ (guint) __builtin_clzl(number)) + 1 : 1;
#else
  guint n_bits = 0;

  do
    {
      n_bits++;
      number >>= 1;
    }
  while (number);
  return n_bits;
#endif
}

#  define g_abort() abort ()


#endif