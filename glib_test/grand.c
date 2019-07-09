#include <gmodule.h>
#include <glib.h>
#include <glib/gprintf.h>

int main(int argc, char **argv)
{
 g_printf ("main in\n");

 GRand *r1;
 r1 = g_rand_new_with_seed (0);
 g_printf ("g_rand_new_with_seed:%d\n", g_rand_int (r1));
 g_printf ("g_rand_boolean:%d\n", g_rand_boolean(r1));
 g_printf ("g_rand_int_range:%d\n", g_rand_int_range(r1, 3, 8));
 g_printf ("g_rand_double:%f\n", g_rand_double(r1));
 g_printf ("g_rand_double_range:%f\n", g_rand_double_range (r1, 2.4, 9.5));

 GRand *rc;
 rc = g_rand_copy (r1);

 g_rand_set_seed (r1, 1);
 g_printf ("g_rand_set_seed:%d\n", g_rand_int (r1));

 g_rand_set_seed (rc, 0);
 g_printf ("g_rand_copy->r1:%d\n", g_rand_int (rc));

 g_rand_free (r1);
 g_rand_free (rc);

 GRand *r2;
 r2 = g_rand_new ();
 g_printf ("g_rand_new:%d\n", g_rand_int (r2));
 g_printf ("g_rand_boolean:%d\n", g_rand_boolean(r2));
 g_printf ("g_rand_int_range:%d\n", g_rand_int_range(r2, 3, 8));
 g_printf ("g_rand_double:%f\n", g_rand_double(r2));
 g_printf ("g_rand_double_range:%f\n", g_rand_double_range (r2, 2.4, 9.5));
 g_rand_free (r2);

 GRand *r3;
 const guint32 seed[] = {0, 1, 2, 3, 4};
 r3 = g_rand_new_with_seed_array (seed, (sizeof(seed)/sizeof(guint32)));
 g_printf ("g_rand_new_with_seed_array:%d\n", g_rand_int (r3));

 const guint32 seed1[] = {0, 1, 2, 3};
 g_rand_set_seed_array (r3, seed1, (sizeof(seed)/sizeof(guint32)));
 g_printf ("g_rand_set_seed_array:%d\n", g_rand_int (r3));
 g_rand_free (r3);

 g_printf ("g_random_int:%d\n", g_random_int ());
 g_printf ("g_random_boolean:%d\n", g_random_boolean());
 g_printf ("g_random_int_range:%d\n", g_random_int_range(3, 8));
 g_printf ("g_random_double:%f\n", g_random_double ());
 g_printf ("g_random_double:%f\n", g_random_double_range (2.4, 9.5));

 g_printf ("after g_random_set_seed (0)\n");
 g_random_set_seed (0);
 g_printf ("g_random_int:%d\n", g_random_int ());


    return 0;
}