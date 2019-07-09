#include <glib.h>

struct map {
    int key;
    char *value;
} m[10] = {
    {1,"one"},
    {2,"two"},
    {3,"three"},
    {4,"four"},
    {5,"five"},
    {6,"six"},
    {7,"seven"},
    {8,"eight"},
    {9,"nine"},
    {10,"ten"}
};
typedef struct map map;

static gint
sort(gconstpointer p1, gconstpointer p2)
{
    char * a, *b;

    a = *(char **)(p1);
    b = *(char **)(p2);

    return (*a > *b ? +1 : *a == *b ? 0 : -1);
}

static gint
sort_r(gconstpointer p1, gconstpointer p2, gpointer user_data)
{
    char * a, *b;

    a = *(char **)(p1);
    b = *(char **)(p2);

    return (*a < *b ? +1 : *a == *b ? 0 : -1);
}

static void
print(gpointer data, gpointer user_data)
{
    g_printf("%s,", data);
    g_printf("\n");
}

static void
test_array(void)
{
    GPtrArray *gparray = NULL;
    gint i;

// GPtrArray* g_ptr_array_new();    创建动态指针数组指针
    gparray = g_ptr_array_new ();
// void g_ptr_array_add(GPtrArray *array,gpointer data);  添加data到数组array的后面
    for (i = 0; i < sizeof(m)/sizeof(m[0]); i++)
         g_ptr_array_add(gparray, (gpointer)m[i].value);

    g_printf("There should be '%d' items now.\t\tResult: %d.\n", 10, gparray->len);
    g_printf("All of items:\n");
// #define g_ptr_array_index(array,index_)      返回数组array中第index_个的值
    for (i = 0; i < sizeof(m)/sizeof(m[0]); i++)
        g_printf("%s,", g_ptr_array_index(gparray, i));
    g_printf("\n");

// gpointer g_ptr_array_remove_index(GPtrArray *array, guint index_);   删除数组中的第index_值，并将后面的元素向前移动，对应的g_ptr_array_remove_index_fast函数的删除元素，然后使用最后一个元素填充，其他不移动，这样会执行的很快
    g_ptr_array_remove_index(gparray, 1);
    g_printf("All of items[exclude the second item]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1; i++)
        g_printf("%s,", g_ptr_array_index(gparray, i));
    g_printf("\n");

// gpointer g_ptr_array_remove_index_fast(GPtrArray *array, guint index_);  删除元素，然后使用最后一个元素填充，其他不移动
    g_ptr_array_remove_index_fast(gparray, 1);
    g_printf("All of items[exclude the second item]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1; i++)
        g_printf("%s,", g_ptr_array_index(gparray, i));
    g_printf("\n");

// void g_ptr_array_remove_range(GPtrArray *array, guint index_);     删除数组中从第index_开始的length多个元素，后面的元素被顺序下移
    g_ptr_array_remove_range(gparray, 2, 2);
    g_printf("All of items[after remove 2 items from the third item]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1-2; i++)
        g_printf("%s,", g_ptr_array_index(gparray, i));
    g_printf("\n");

// void g_ptr_array_sort(GPtrArray *array, GCompareFunc compare_func); 通过compare_func函数规则进行排序
    g_ptr_array_sort(gparray, sort);
    g_printf("All of items[sorted]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1-2; i++)
        g_printf("%s,", g_ptr_array_index(gparray, i));
    g_printf("\n");

// void g_ptr_array_sort_with_data(GPtrArray *array, GCompareDataFunc compare_func, gpointer user_data); compare_func函数带输入参数  user_data的排序
    g_ptr_array_sort_with_data(gparray, sort_r, NULL);
    g_printf("All of items[sorted reversed]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1-2; i++)
        g_printf("%s,", g_ptr_array_index(gparray, i));
    g_printf("\n");


//void g_ptr_array_foreach(GPtrArray *array,GFunc func,gpointer user_data); 遍历数组，将数据元素传递给func

g_ptr_array_foreach (gparray, print, (gpointer) NULL);


// gchar* g_ptr_array_free(GPtrArray *array, gboolean free_segment);   释放动态数组，如果free_segment是true会同时把数据也释放掉
    g_ptr_array_free(gparray, TRUE);
}

int
main(void)
{
    g_printf("BEGIN:\n************************************************************\n");
    test_array();
    g_printf("\n************************************************************\nDONE\n");

    return 0;
}
 