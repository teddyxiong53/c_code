#include <glib.h>
#include <glib/gprintf.h>
#include <unistd.h>
#include <sys/syscall.h>

struct test_struct {
    gint i;
    gfloat k;
};

void destroy_notify(gpointer a)
{
	g_printf ("destroy_notify in\n");
	//此处应该执行释放动态内存动作
	g_printf ("destroy_notify out\n");
}


static GPrivate private_key = G_PRIVATE_INIT (destroy_notify);

gint count = 0;

gpointer thread_func1 (gpointer data)
{
	struct test_struct *struct_data = g_new(struct test_struct, 1);
	struct_data->i = 10;
    struct_data->k = 3.1415;

	g_private_set (&private_key, (gpointer)(struct_data));
	g_printf ("%s struct_data addr: 0x%p\n", __func__, struct_data);
    g_printf ("%s g_private_get(key) return addr: 0x%p\n", __func__, (struct test_struct *)g_private_get (&private_key));
    g_printf ("%s g_private_get(key) into struct_data.i: %d\nstruct_data.k: %f\n", __func__, ((struct test_struct *)g_private_get (&private_key))->i, ((struct test_struct *)g_private_get(&private_key))->k);
	g_usleep (2000000);
}

gpointer thread_func2 (gpointer data)
{
	gint temp = 20;
    g_usleep (1000000);

	g_printf ("%s temp addr: 0x%p\n", __func__, &temp);
    g_private_set (&private_key, GINT_TO_POINTER (temp));
    g_printf ("%s g_private_get(key): 0x%p\n", __func__, (gint *)g_private_get(&private_key));
    g_printf ("%s g_private_get(key) temp: %d\n", __func__, GPOINTER_TO_INT (g_private_get(&private_key)));
}


int main(int argc, char **argv)
{
	g_printf ("main in\n");

	GThread *gthread1 = NULL, *gthread2 = NULL;
	gthread1 = g_thread_new ("func1", thread_func1, NULL);
	gthread2 = g_thread_new ("func2", thread_func2, NULL);
	g_thread_join (gthread1);
	g_thread_join (gthread2);

	g_printf ("main out\n");
    return 0;
}
 