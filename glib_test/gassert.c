#include <glib.h>
#include <glib/gprintf.h>

#include <glib/gstdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

void gprint_func (const gchar *string)
{
	gint fd = g_open ("warnasser.txt", O_RDWR|O_APPEND, 0666);
	write(fd, string, strlen (string));
	g_close (fd, NULL);
}

void return_if_fail(gboolean expr)
{
	g_return_if_fail(expr);
	g_printf ("g_return_if_fail is true\n");
}

gint32 return_val_if_fail(gboolean expr)
{
	g_return_val_if_fail(expr, 22);
	g_printf ("g_return_val_if_fail is true\n");
	return 11;
}

void return_if_reached()
{
	g_return_if_reached ();
}

gint32 return_val_if_reached()
{
	g_return_val_if_reached (33);
}

int main(int argc, char **argv)
{
	g_printf ("main in\n");

	g_set_print_handler (gprint_func);
	g_print ("g_print %d %s\n", __LINE__, "line");

	g_printerr ("g_printerr %d %s\n", __LINE__, "line");

	return_if_fail(FALSE);

	g_printf ("return_val_if_fail() return %d\n", return_val_if_fail(FALSE));

	return_if_reached();

	g_printf ("return_val_if_reached() return %d\n", return_val_if_reached());

	//为实现，暂时搁置
	//g_set_prgname ("gwarn");
	//G_BREAKPOINT();
	//g_on_error_query (g_get_prgname ());

	g_printf ("main out\n");
    return 0;
}