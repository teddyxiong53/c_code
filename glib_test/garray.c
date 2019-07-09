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
    gint32 a, b;

    a = *(gint*)(p1);
    b = *(gint*)(p2);

    return (a > b ? +1 : a == b ? 0 : -1);
}

static gint
sort_r(gconstpointer p1, gconstpointer p2, gpointer user_data)
{
    gint32 a, b;

    a = *(gint*)(p1);
    b = *(gint*)(p2);

    return (a < b ? +1 : a == b ? 0 : -1);
}

static void
print(GArray *array)
{
    gint i;
    for (i = 0; i < array->len; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");
}

static void
test_array(void)
{
    GArray *array = NULL;
    gint i;

// GArray* g_array_new(gboolean zero_terminated, gboolean clear_, guint element_size);
//创建动态数组指针，zero_terminated如果为true表示使用额外的一个元素0作为数组的结尾
//clear_为true表示再分配元素的时候清0分配空间，element_size表示需要保存的一个元素占用多少字节空间
    array = g_array_new(FALSE, FALSE, sizeof(int));
// #define g_array_append_val(a,v)  添加v到数组a的后面，需要注意的是这个是一个宏，所以v不能是一个常量，必须要一个变量
    for (i = 0; i < sizeof(m)/sizeof(m[0]); i++)
        g_array_append_val(array, m[i].key);

    g_printf("There should be '%d' items now.\t\tResult: %d.\n", 10, array->len);
    g_printf("All of items:\n");
// #define g_array_index(a, t, i)       返回数组a中第i个的值，t是值的类型
    for (i = 0; i < sizeof(m)/sizeof(m[0]); i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

// GArray* g_array_remove_index(GArray *array, guint index_);   删除数组中的第index_值，并将后面的元素向前移动，对应的g_array_remove_index_fast函数的删除元素，然后使用最后一个元素填充，其他不移动，这样会执行的很快
    array = g_array_remove_index(array, 1);
    g_printf("All of items[exclude the second item]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

// GArray* g_array_remove_index_fast(GArray *array, guint index_);  删除元素，然后使用最后一个元素填充，其他不移动
    array = g_array_remove_index_fast(array, 1);
    g_printf("All of items[exclude the second item]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

// GArray* g_array_remove_range(GArray *array, guint index_, guint length);     删除数组中从第index_开始的length多个元素，后面的元素被顺序下移
    array = g_array_remove_range(array, 2, 2);
    g_printf("All of items[after remove 2 items from the third item]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1-2; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

// void g_array_sort(GArray *array, GCompareFunc compare_func); 通过compare_func函数规则进行排序
    g_array_sort(array, sort);
    g_printf("All of items[sorted]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1-2; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

// void g_array_sort_with_data(GArray *array, GCompareDataFunc compare_func, gpointer user_data); compare_func函数带输入参数  user_data的排序
    g_array_sort_with_data(array, sort_r, NULL);
    g_printf("All of items[sorted reversed]:\n");
    for (i = 0; i < sizeof(m)/sizeof(m[0])-1-1-2; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

    int x[4] = {2,3,4,5};
// GArray* g_array_append_vals(GArray *array, gconstpointer data, guint len);   在数组后面添加长度为len的数据，data是数据的首地址指针
    array = g_array_append_vals(array, x, 4);
    g_printf("All of items[after append all 2,3,4,5]:\n");
    for (i = 0; i < array->len; i++)
        g_printf("%d,", g_array_index(array, int, i));
    g_printf("\n");

    g_printf("All of items[after prepend one by one 2,3,4,5]:\n");
// #define g_array_prepend_val(a,v) 与g_array_append_val相似，在数组最前面插入一个数据
    for (i = 0; i < 4; i++)
        g_array_prepend_val(array, x[i]);
    print(array);

    g_printf("All of items[after prepend all 2,3,4,5]:\n");
// GArray* g_array_prepend_vals(GArray *array, gconstpointer data, guint len); 与g_array_append_vals相似，在数组前面插入len多个的数据
    array = g_array_prepend_vals(array, x, 4);
    print(array);

    int t = 0;
// #define g_array_insert_val(a, i, v)  在数组a中第i个位置插入数据v，由于是宏定义，所以v必须是变量
    g_array_insert_val(array, 0, t);
    g_printf("All of items[after insert 0 at the first index]:\n");
    print(array);

    g_array_sort(array, sort);
    g_printf("All of items[sorted]:\n");
    print(array);

// gchar* g_array_free(GArray *array, gboolean free_segment);   释放动态数组，如果free_segment是true会同时把数据也释放掉
    g_array_free(array, TRUE);
}

int
main(void)
{
    g_printf("BEGIN:\n************************************************************\n");
    test_array();
    g_printf("\n************************************************************\nDONE\n");

    return 0;
}