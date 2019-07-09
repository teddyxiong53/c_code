
#include <stdio.h>
#include <glib.h>
//#include <glib/gprintf.h>

struct map {
    int key;
    char *value;
} m[10] = {
    {0,"zero"},
    {1,"one"},
    {2,"two"},
    {3,"three"},
    {4,"four"},
    {5,"five"},
    {6,"six"},
    {7,"seven"},
    {8,"eight"},
    {9,"nine"},
};
typedef struct map map;

static void
myPrintInt(gpointer p1, gpointer fmt)
{
    g_printf(fmt, *(gint *)p1);
}

static void
myPrintStr(gpointer p1, gpointer fmt)
{
    g_printf(fmt, (gchar *)p1);
}

static void
test_queue_1(void)
{
// GQueue* g_queue_new(void);   创建队列
    GQueue *queue = g_queue_new();
// gboolean g_queue_is_empty(GQueue *queue);    判断队列是否为空，空返回1
    gboolean b = g_queue_is_empty(queue);
    g_printf("The queue should be empty now.\t\tResult: %s.\n", b ? "YES" : "NO");

// void g_queue_push_tail(GQueue *queue, gpointer data);    将数据data压入队列尾部
    gint i;
    for (i = 0; i < sizeof (m) / sizeof (m[0]); i++)
        g_queue_push_tail(queue, m[i].value);

// void g_queue_foreach(GQueue *queue, GFunc func, gpointer user_data);     调用函数func遍历队列元素，user_data是函数的参数
    g_printf("Now the queue[after push tail] :\n");
    g_queue_foreach(queue, myPrintStr, "%s, ");//打印字符串的形式输出队列的数据，因为之前是将map中的value，即字符串数据保存到了队列中
    g_printf("\n");

// guint g_queue_get_length(GQueue *queue);     得到队列元素的个数
    g_printf("The lenght should be '%d' now.\t\tResult: %d.\n", 10, g_queue_get_length(queue));

// void g_queue_reverse(GQueue *queue);     翻转队列的元素
    g_queue_reverse(queue);
    g_printf("Now the queue[after reverse] :\n");
    g_queue_foreach(queue, myPrintStr, "%s, ");
    g_printf("\n");

// gpointer g_queue_pop_head(GQueue *queue);    从头部取出一个数据
    g_printf("The head should be '%s' now.\t\tResult: %s.\n", "nine", (gchar *)g_queue_pop_head(queue));

// gpointer g_queue_pop_tail(GQueue *queue);    从尾部取出一个数据
    g_printf("The tail should be '%s' now.\t\tResult: %s.\n", "zero", (gchar *)g_queue_pop_tail(queue));
    g_printf("Now the queue[after pop head and tail] :\n");
    g_queue_foreach(queue, myPrintStr, "%s, ");
    g_printf("\n");

// gpointer g_queue_pop_nth(GQueue *queue, guint n);    将第n个数据取出
    g_printf("The head should be '%s' now.\t\tResult: %s.\n", "eight", (gchar *)g_queue_pop_nth(queue, 0));

// void g_queue_push_head(GQueue *queue, gpointer data);    将数据data压入队列头
    g_queue_push_head(queue, "zero");
    g_queue_push_head(queue, "eight");

// void g_queue_push_nth(GQueue *queue, gpointer data, gint n);     将data插入第n个位置
    g_queue_push_nth(queue, "nine", 2);

    g_printf("Now the queue[after push 'zero' 'eight' 'nine'] :\n");
    g_queue_foreach(queue, myPrintStr, "%s, ");
    g_printf("\n");

// gpointer g_queue_peek_head(GQueue *queue);       查看头数据，不取出
    g_printf("The head should be '%s' now.\t\tResult: %s.\n", "eight", (gchar *)g_queue_peek_head(queue));

// gpointer g_queue_peek_tail(GQueue *queue);       查看尾数据，不取出
    g_printf("The tail should be '%s' now.\t\tResult: %s.\n", "one", (gchar *)g_queue_peek_tail(queue));

// gpointer g_queue_peek_nth(GQueue *queue, guint n);   查看第n个数据，不取出
    g_printf("The head should be '%s' now.\t\tResult: %s.\n", "eight", (gchar *)g_queue_peek_nth(queue, 0));

/*
// void g_queue_clear(GQueue *queue);   清理队列，如果数据是动态申请的， 在调用这个函数之前需要先释放数据，这个函数是与init配套使用的，new配套使用free
    g_queue_clear(queue);
    g_printf("Now the queue[after clear] :\n");
    g_queue_foreach(queue, myPrintStr, "%s, ");
    g_printf("\n");
*/

// void g_queue_free(GQueue *queue);    释放队列
    g_queue_free(queue);
}

static gint
sort(gconstpointer p1, gconstpointer p2, gpointer user_data)//正向排序规则
{
    gint32 a, b;

    a = *(gint*)(p1);
    b = *(gint*)(p2);

    return (a > b ? +1 : a == b ? 0 : -1);
}

static gint
myCompareInt(gconstpointer p1, gconstpointer p2)//比较大小
{
    const int *a = p1;
    const int *b = p2;

    return *a - *b;
}


static gint
sort_r(gconstpointer p1, gconstpointer p2, gpointer user_data)//逆向排序规则
{
    gint32 a, b;

    a = *(gint*)(p1);
    b = *(gint*)(p2);

    return (a < b ? +1 : a == b ? 0 : -1);
}

static void
test_queue_2(void)
{
    GQueue *queue = NULL;
/*
// void g_queue_init(GQueue *queue);    在栈中申请队列
    g_queue_init(queue);
*/
    queue = g_queue_new();

// void g_queue_insert_sorted(GQueue *queue, gpointer data, GCompareDataFunc func gpointer user_data);  按照func规则进行插入队列
    gint i;
    for (i = 0; i < sizeof (m) / sizeof (m[0]); i++)
        g_queue_insert_sorted(queue, &m[i].key, sort_r, NULL);

    g_printf("Now the queue[after insert sorted] :\n");
    for (i = 0; i < queue->length; i++)
        g_printf("%d, ", *(gint *)g_queue_peek_nth(queue, i));
    g_printf("\n");

// void g_queue_remove(GQueue *queue, gconstpointer data);      将队列中的data数据移除，只移除第一次出现的数据
    g_queue_remove(queue, &m[3].key);
    g_printf("Now the queue[after remove '%d'] :\n", m[3].key);
    for (i = 0; i < queue->length; i++)
        g_printf("%d, ", *(gint *)g_queue_peek_nth(queue, i));
    g_printf("\n");

// GList* g_queue_find_custom(GQueue *queue, gconstpointer data, GCompareFunc func);    通过比较函数func查找数据data
// void g_queue_insert_before(GQueue *queue, GList *sibling, gpointer data);            将数据data插入到sibling前面
// void g_queue_insert_after(GQueue *queue, GList *sibling, gpointer data);             将数据data插入到sibling后面
    g_queue_insert_before(queue, g_queue_find_custom(queue, &m[5].key, myCompareInt), &m[3].key);
    g_queue_insert_after(queue, g_queue_find_custom(queue, &m[5].key, myCompareInt), &m[3].key);

    g_printf("Now the queue[after insert '%d' around '%d'] :\n", m[3].key, m[5].key);
    g_queue_foreach(queue, myPrintInt, "%d, ");
    g_printf("\n");

// void g_queue_sort(GQueue *queue, GCompareDataFunc compare, gpointer user_data);  通过比较函数compare进行排序
    g_queue_sort(queue, sort, NULL);

    g_printf("Now the queue[sorted] :\n");
    g_queue_foreach(queue, myPrintInt, "%d, ");
    g_printf("\n");

// GQueue* g_queue_copy(GQueue *queue);     复制队列，同样，也只是指针的复制，并没有实际复制数据
    GQueue *q = g_queue_copy(queue);
    g_printf("Now the queue[copy] :\n");
    g_queue_foreach(q, myPrintInt, "%d, ");
    g_printf("\n");

// void g_queue_remove_all(GQueue *queue, gconstpointer data);      删除队列中所有的data数据
    g_queue_remove_all(q, &m[3].key);
    g_printf("Now the queue[remove '%d'] :\n", m[3].key);
    g_queue_foreach(q, myPrintInt, "%d, ");
    g_printf("\n");

    g_queue_free(q);
    g_queue_free(queue);
}

int
main(void)
{
    printf("BEGIN:\n************************************************************\n");
    printf("\n------------------------------------------------------------\ntest_queue_1:\n");
    test_queue_1();
    printf("------------------------------------------------------------\n");
    printf("\n------------------------------------------------------------\ntest_queue_2:\n");
    test_queue_2();
    printf("------------------------------------------------------------\n");
    printf("\n************************************************************\nDONE\n");

    return 0;

}