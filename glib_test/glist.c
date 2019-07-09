
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
//#include <glib/gprintf.h>

static gint
sort(gconstpointer p1, gconstpointer p2) //排序函数，正向排序
{
    gint32 a, b;

    a = GPOINTER_TO_INT(p1);
    b = GPOINTER_TO_INT(p2);

    return (a > b ? +1 : a == b ? 0 : -1);
}

static gint
sort_r(gconstpointer p1, gconstpointer p2) //逆向排序
{
    gint32 a, b;

    a = GPOINTER_TO_INT(p1);
    b = GPOINTER_TO_INT(p2);

    return (a < b ? +1 : a == b ? 0 : -1);
}

static void
print(gpointer p1, gpointer p2) //打印函数，只打印P1
{
    g_printf("%d,", *(gint *)p1);
}

static void
test_list(void)
{
    GList *list = NULL;
    gint nums[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // GList* g_list_append(GList *list, gpointer data);//尾加，和单链表是一样的功能
    list = g_list_append(list, &nums[1]);
    g_printf("The first item should be '%d' now.\t\tResult: %d.\n", nums[1], *(gint *)list->data);
    // GList* g_list_prepend(GList *list, gpointer data);//头加
    list = g_list_prepend(list, &nums[0]);
    // GList* g_list_first(GList *list);//获得头指针
    g_printf("The first item should be '%d' now.\t\tResult: %d.\n", nums[0], *(gint *)g_list_first(list)->data);
    // GList* g_list_insert(GList *list, gpointer data, gint position);//在position位置插入data数据
    list = g_list_insert(list, &nums[2], 2);
    // GList* g_list_last(GList *list);         返回链表最后一个的指针
    g_printf("The last item should be '%d' now.\t\tResult: %d.\n", nums[2], *(gint *)g_list_last(list)->data);
    // GList* g_list_insert_before(GList *list, GList *sibling, gpointer data);     在链表指针sibling处前面插入data数据，NULL是最后一个的指针
    list = g_list_insert_before(list, NULL, &nums[3]);
    g_printf("The last item should be '%d' now.\t\tResult: %d.\n", nums[3], *(gint *)g_list_last(list)->data);
    // #define g_list_next (list)           在list指针处的下一个指针，这个和单链表的next是一样的
    g_printf("The second item should be '%d' now.\t\tResult: %d.\n", nums[1], *(gint *)g_list_next(list)->data);
    // #define g_list_previous (list)       这个是list指针处的前一个指针，这个是单链表无法容易做到的，双向链表中的双向优势
    g_printf("The first item should be '%d' now.\t\tResult: %d.\n", nums[0], *(gint *)g_list_previous(g_list_next(list))->data);
    // gint g_list_index(GList *list, gconstpointer data);  查找data首次出现的位置
    g_printf("The index of '%d' should be '%d' now.\t\tResult: %d.\n", nums[2], 2, g_list_index(list, &nums[2]));
    // gint g_list_position(GList *list, GList *llink);     返回link在list链表中的位置
    g_printf("The position of the third item should be 2 now.\t\tResult: %d.\n", g_list_position(list, g_list_next(list)->next));
    // guint g_list_length(GList *list);        链表长度
    g_printf("The length of list should be 4 now.\t\tResult: %d.\n", g_list_length(list));

    GList *lt = NULL;
    gint i;

    // GList* g_list_insert_sorted(GList *list, gpointer data, GCompareFunc func);      按照func的规则排序插入
    for (i = 4; i < 10; i++)
        lt = g_list_insert_sorted(lt, &nums[i], sort_r); //逆向排序插入，9,8,7,6,5,4
                                                         // GList* g_list_reverse(GList *list);      翻转链表，翻转后是正序了，4,5,6,7，8,9
    lt = g_list_reverse(lt);

    g_printf("The second half of list should be sored now.\nResult:");
    // gpointer g_list_nth_data(GList *list, guint n);      返回在位置n的数据
    for (i = 4; i < 10; i++)
        g_printf("%d,", *(gint *)(g_list_nth_data(lt, i - 4)));
    g_printf("\n");

    // GList* g_list_concat(GList *list1, GList *list2);       拼接list1和list2，这里的list2的内容并不是复制的，而是直接连接的
    list = g_list_concat(list, lt);
    g_printf("The list should have all items which should be sored now.\nResult:");
    // void g_list_foreach(GList *list, GFunc func, gpointer user_data);    调用func函数去操作list的每个元素，前面print函数是打印了list中的元素，user_data是func的第二个参数
    g_list_foreach(list, print, NULL);
    g_printf("\n");

    // GList* g_list_sort(GList *list, GCompareFunc compare_func);      将链表中的元素按照func的规则进行排序
    list = g_list_sort(list, sort_r); //逆向排序
    g_printf("The list should have all items which should be sored reversed now.\nResult:");
    g_list_foreach(list, print, NULL);
    g_printf("\n");

    GList *lb = NULL;
    // GList* g_list_copy(GList *list);     复制链表，仅仅是指针的复制，并没有进行数据的复制
    lb = g_list_copy(list);
    g_printf("The backup list should have the same item and sequence now.\nResult:");
    // GList* g_list_nth(GList *list, guint n);     返回位置n的链表指针
    for (i = 0; i < 10; i++)
    {
        GList *ltmp = g_list_nth(lb, i);
        g_printf("%d,", *(gint *)ltmp->data);
    }
    g_printf("\n");

    // GList* g_list_sort_with_data(GList *list, GCompareDataFunc compare_func, gpointer user_data);    和g_list_sort()函数一样，只是能够接受一个用户参数user_data
    lb = g_list_sort_with_data(lb, (GCompareDataFunc)sort, NULL);
    g_printf("The backup list should have all items which should be sored now.\nResult:");
    g_list_foreach(lb, print, NULL);
    g_printf("\n");

    GList *lall = NULL;
    lall = g_list_concat(list, lb);
    g_printf("The concated list should have all items now.\nResult:");
    g_list_foreach(lall, print, NULL);
    g_printf("\n");

    // GList* g_list_remove(GList *list, gconstpointer data);   删除数据data，只删除第一个出现的元素
    lall = g_list_remove(lall, &nums[0]);
    g_printf("The list should have only one '%d' item now.\nResult:", nums[0]);
    g_list_foreach(lall, print, NULL);
    g_printf("\n");

    // GList* g_list_remove_all(GList *list, gconstpointer data);   删除data，不仅仅是第一个，所有的都删除
    lall = g_list_remove_all(lall, &nums[9]);
    g_printf("The list should not have '%d' item now.\nResult:", nums[9]);
    g_list_foreach(lall, print, NULL);
    g_printf("\n");

    GList *ll = NULL;
    // GList* g_list_find(GList *list, gconstpointer data);         找到数据data在链表中的位置，返回元素指针
    g_printf("The list should find '%d' now.\t\tResutl: %d.\n", nums[0], (ll = g_list_find(lall, &nums[0])) ? *(gint *)ll->data : -1);
    // GList* g_list_find_custom(GList *list, gconstpointer data, GCompareFunc func);       根据func条件查找data，如果没找到返回null
    g_printf("The list should not find '%d' now.\t\tResutl: %d.\n", nums[9], (ll = g_list_find_custom(lall, &nums[9], sort)) ? *(gint *)ll->data : -1);

    // void g_list_free(GList *list);
    g_list_free(lall);
}

int main(void)
{
    printf("BEGIN:\n************************************************************\n");
    test_list();
    printf("\n************************************************************\nDONE\n");
    return 0;
}