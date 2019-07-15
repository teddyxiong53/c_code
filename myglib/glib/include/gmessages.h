#ifndef __G_MESSAGES_H__
#define __G_MESSAGES_H__

#include "gtypes.h"
#include "gmacros.h"
// #include "gvariant.h"
#include <stdarg.h>
G_BEGIN_DECLS

gsize	g_printf_string_upper_bound (const gchar* format,
				     va_list	  args) G_GNUC_PRINTF(1, 0);


#define G_LOG_LEVEL_USER_SHIFT  (8)

typedef enum
{
  /* log flags */
  G_LOG_FLAG_RECURSION          = 1 << 0,
  G_LOG_FLAG_FATAL              = 1 << 1,

  /* GLib log levels */
  G_LOG_LEVEL_ERROR             = 1 << 2,       /* always fatal */
  G_LOG_LEVEL_CRITICAL          = 1 << 3,
  G_LOG_LEVEL_WARNING           = 1 << 4,
  G_LOG_LEVEL_MESSAGE           = 1 << 5,
  G_LOG_LEVEL_INFO              = 1 << 6,
  G_LOG_LEVEL_DEBUG             = 1 << 7,

  G_LOG_LEVEL_MASK              = ~(G_LOG_FLAG_RECURSION | G_LOG_FLAG_FATAL)
} GLogLevelFlags;

#define G_LOG_FATAL_MASK        (G_LOG_FLAG_RECURSION | G_LOG_LEVEL_ERROR)

typedef void            (*GLogFunc)             (const gchar   *log_domain,
                                                 GLogLevelFlags log_level,
                                                 const gchar   *message,
                                                 gpointer       user_data);




GLIB_AVAILABLE_IN_ALL
guint           g_log_set_handler       (const gchar    *log_domain,
                                         GLogLevelFlags  log_levels,
                                         GLogFunc        log_func,
                                         gpointer        user_data);

guint           g_log_set_handler_full  (const gchar    *log_domain,
                                         GLogLevelFlags  log_levels,
                                         GLogFunc        log_func,
                                         gpointer        user_data,
                                         GDestroyNotify  destroy);
GLIB_AVAILABLE_IN_ALL
void            g_log_remove_handler    (const gchar    *log_domain,
                                         guint           handler_id);
GLIB_AVAILABLE_IN_ALL
void            g_log_default_handler   (const gchar    *log_domain,
                                         GLogLevelFlags  log_level,
                                         const gchar    *message,
                                         gpointer        unused_data);
GLIB_AVAILABLE_IN_ALL
GLogFunc        g_log_set_default_handler (GLogFunc      log_func,
					   gpointer      user_data);
GLIB_AVAILABLE_IN_ALL
void            g_log                   (const gchar    *log_domain,
                                         GLogLevelFlags  log_level,
                                         const gchar    *format,
                                         ...) G_GNUC_PRINTF (3, 4);
GLIB_AVAILABLE_IN_ALL
void            g_logv                  (const gchar    *log_domain,
                                         GLogLevelFlags  log_level,
                                         const gchar    *format,
                                         va_list         args) G_GNUC_PRINTF(3, 0);
GLIB_AVAILABLE_IN_ALL
GLogLevelFlags  g_log_set_fatal_mask    (const gchar    *log_domain,
                                         GLogLevelFlags  fatal_mask);
GLIB_AVAILABLE_IN_ALL
GLogLevelFlags  g_log_set_always_fatal  (GLogLevelFlags  fatal_mask);



typedef enum
{
  G_LOG_WRITER_HANDLED = 1,
  G_LOG_WRITER_UNHANDLED = 0,
} GLogWriterOutput;

typedef struct _GLogField GLogField;
struct _GLogField
{
  const gchar *key;
  gconstpointer value;
  gssize length;
};


typedef GLogWriterOutput (*GLogWriterFunc)     (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields,
                                                gpointer         user_data);


void             g_log_structured              (const gchar     *log_domain,
                                                GLogLevelFlags   log_level,
                                                ...);

void             g_log_structured_array        (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields);


// void             g_log_variant                 (const gchar     *log_domain,
//                                                 GLogLevelFlags   log_level,
//                                                 GVariant        *fields);


void             g_log_set_writer_func         (GLogWriterFunc   func,
                                                gpointer         user_data,
                                                GDestroyNotify   user_data_free);


gboolean         g_log_writer_supports_color   (gint             output_fd);

gboolean         g_log_writer_is_journald      (gint             output_fd);


gchar           *g_log_writer_format_fields    (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields,
                                                gboolean         use_color);


GLogWriterOutput g_log_writer_journald         (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields,
                                                gpointer         user_data);

GLogWriterOutput g_log_writer_standard_streams (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields,
                                                gpointer         user_data);

GLogWriterOutput g_log_writer_default          (GLogLevelFlags   log_level,
                                                const GLogField *fields,
                                                gsize            n_fields,
                                                gpointer         user_data);


void	_g_log_fallback_handler	(const gchar   *log_domain,
						 GLogLevelFlags log_level,
						 const gchar   *message,
						 gpointer       unused_data);

/* Internal functions, used to implement the following macros */
GLIB_AVAILABLE_IN_ALL
void g_return_if_fail_warning (const char *log_domain,
			       const char *pretty_function,
			       const char *expression) G_ANALYZER_NORETURN;
GLIB_AVAILABLE_IN_ALL
void g_warn_message           (const char     *domain,
                               const char     *file,
                               int             line,
                               const char     *func,
                               const char     *warnexpr) G_ANALYZER_NORETURN;

void g_assert_warning         (const char *log_domain,
			       const char *file,
			       const int   line,
		               const char *pretty_function,
		               const char *expression) G_GNUC_NORETURN;
#ifndef G_LOG_DOMAIN
#define G_LOG_DOMAIN    ((gchar*) 0)
#endif  /* G_LOG_DOMAIN */


#define g_error(...)  G_STMT_START {                 \
                        g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_ERROR,    \
                               __VA_ARGS__);         \
                        for (;;) ;                   \
                      } G_STMT_END
#define g_message(...)  g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_MESSAGE,  \
                               __VA_ARGS__)
#define g_critical(...) g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_CRITICAL, \
                               __VA_ARGS__)
#define g_warning(...)  g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_WARNING,  \
                               __VA_ARGS__)
#define g_info(...)     g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_INFO,     \
                               __VA_ARGS__)
#define g_debug(...)    g_log (G_LOG_DOMAIN,         \
                               G_LOG_LEVEL_DEBUG,    \
                               __VA_ARGS__)




typedef void    (*GPrintFunc)           (const gchar    *string);
GLIB_AVAILABLE_IN_ALL
void            g_print                 (const gchar    *format,
                                         ...) G_GNUC_PRINTF (1, 2);
GLIB_AVAILABLE_IN_ALL
GPrintFunc      g_set_print_handler     (GPrintFunc      func);
GLIB_AVAILABLE_IN_ALL
void            g_printerr              (const gchar    *format,
                                         ...) G_GNUC_PRINTF (1, 2);
GLIB_AVAILABLE_IN_ALL
GPrintFunc      g_set_printerr_handler  (GPrintFunc      func);


#define g_warn_if_reached() \
  do { \
    g_warn_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, NULL); \
  } while (0)

#define g_warn_if_fail(expr) \
  do { \
    if G_LIKELY (expr) ; \
    else g_warn_message (G_LOG_DOMAIN, __FILE__, __LINE__, G_STRFUNC, #expr); \
  } while (0)



#define g_return_if_fail(expr)		G_STMT_START{			\
     if G_LIKELY(expr) { } else       					\
       {								\
	 g_return_if_fail_warning (G_LOG_DOMAIN,			\
		                   G_STRFUNC,				\
		                   #expr);				\
	 return;							\
       };				}G_STMT_END

#define g_return_val_if_fail(expr,val)	G_STMT_START{			\
     if G_LIKELY(expr) { } else						\
       {								\
	 g_return_if_fail_warning (G_LOG_DOMAIN,			\
		                   G_STRFUNC,				\
		                   #expr);				\
	 return (val);							\
       };				}G_STMT_END

#define g_return_if_reached()		G_STMT_START{			\
     g_log (G_LOG_DOMAIN,						\
	    G_LOG_LEVEL_CRITICAL,					\
	    "file %s: line %d (%s): should not be reached",		\
	    __FILE__,							\
	    __LINE__,							\
	    G_STRFUNC);							\
     return;				}G_STMT_END

#define g_return_val_if_reached(val)	G_STMT_START{			\
     g_log (G_LOG_DOMAIN,						\
	    G_LOG_LEVEL_CRITICAL,					\
	    "file %s: line %d (%s): should not be reached",		\
	    __FILE__,							\
	    __LINE__,							\
	    G_STRFUNC);							\
     return (val);			}G_STMT_END













G_END_DECLS

#endif