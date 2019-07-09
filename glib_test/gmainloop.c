#include <glib.h>
#include <glib/gprintf.h>

gboolean gsource_func (gpointer user_data)
{
    g_printf ("%s() in\n", __func__);
    GTimeVal result;
    g_get_current_time (&result);
    g_printf ("gsource_func %ld.%lds\n", result.tv_sec, result.tv_usec);
    int *data = (int *)user_data;
    (*data)++;
    g_printf ("user_data = %d\n", (*data));
    g_printf ("%s() out\n", __func__);
    if ((*data) == 10)
        return FALSE;
    return TRUE;
}

gboolean gsource_func1 (gpointer user_data)
{
    g_printf ("%s() in\n", __func__);
    GTimeVal result;
    g_get_current_time (&result);
    g_printf ("gsource_func1 %ld.%lds\n", result.tv_sec, result.tv_usec);
    g_printf ("%s() out\n", __func__);
    return TRUE;
}

gboolean gsource_idle (gpointer user_data)
{
    g_printf ("%s() in\n", __func__);

    g_printf ("%s() out\n", __func__);
    return FALSE;
}

void gdestroy_notify (gpointer data)
{
   g_printf ("%s() in\n", __func__);
   g_free (data);
   g_printf ("%s() out\n", __func__);
}

gpointer thread_func (gpointer data)
{
	g_printf ("%s() in\n", __func__);
    GMainContext *context = g_main_context_new ();
    GMainLoop *loop = g_main_loop_new (context, FALSE);

    g_printf ("context is %sowner\n", g_main_context_is_owner (context)?"":"not ");
    g_main_context_acquire (context);
    g_printf ("context is %sowner\n", g_main_context_is_owner (context)?"":"not ");

    GSource *source = g_timeout_source_new_seconds (2);
    guint sourceID = g_source_attach (source, context);
    g_source_set_name (source, "thread_timeout");
    g_printf ("source_get_name:%s ID:%d\n", g_source_get_name (source), sourceID);
    int *data1 = g_new0(int, 1);
    g_source_set_callback (source, gsource_func, data1, gdestroy_notify);

    g_main_loop_run (loop);

    g_printf ("%s() out\n", __func__);
}

gpointer thread_func1 (gpointer data)
{
	g_printf ("%s() in\n", __func__);
    GMainContext *context = g_main_context_new ();
    GMainLoop *loop = g_main_loop_new (context, FALSE);

    g_printf ("context is %sowner\n", g_main_context_is_owner (context)?"":"not ");
    g_main_context_acquire (context);
    g_printf ("context is %sowner\n", g_main_context_is_owner (context)?"":"not ");

    GSource *source = g_timeout_source_new_seconds (2);
    guint sourceID = g_timeout_add (2500, gsource_func1, NULL);
    g_source_set_name (source, "thread_timeout1");
    g_printf ("source_get_name:%s ID:%d\n", g_source_get_name (source), sourceID);

    g_main_loop_run (loop);

    g_printf ("%s() out\n", __func__);
}

int main(int argc, char **argv)
{
    g_printf ("%s() in\n", __func__);
    GThread *gthread = g_thread_new ("func", thread_func, NULL);
    GThread *gthread1 = g_thread_new ("func1", thread_func1, NULL);

    GMainLoop *loop = g_main_loop_new (NULL, FALSE);

    GSource *source = g_timeout_source_new (1500);//1.5s
    guint sourceID = g_source_attach (source, NULL);
    g_printf ("source_get_ID:%d\n", sourceID);
    g_source_set_callback (source, gsource_func1, NULL, NULL);

    g_idle_add (gsource_idle, NULL);
    g_main_loop_run (loop);
    g_printf ("%s() out\n", __func__);
}