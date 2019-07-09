
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>
//#include <glib/gprintf.h>

/*
 * g_slist_append
 * g_slist_prepend
 * g_slist_reverse
 */
static void test_slist_1(void)
{
    GSList *slist = NULL;
    GSList *st;
    gint nums[10] = {0,1,2,3,4,5,6,7,8,9};
    gint i;

    for (i = 0; i < 10; i++)
// GSList* g_slist_append(GSList *list, gpointer data);
        slist = g_slist_append(slist, &nums[i]);

    g_printf("The result should be: 0,1,2,3,4,5,6,7,8,9\n");
    g_printf("Begin:\n");
    for (i = 0; i < 10; i++) {
        st = g_slist_nth(slist, i);
        g_printf("%d,", *(gint*)st->data);
    }
    g_printf("\nDone\n");

// GSList* g_slist_reverse(GSList *list);
    slist = g_slist_reverse(slist);

    g_printf("The result should be: 9,8,7,6,5,4,3,2,1,0\n");
    g_printf("Begin:\n");
    for (i = 0; i < 10; i++) {
        st = g_slist_nth(slist, i);
        g_printf("%d,", *(gint*)st->data);
    }
    g_printf("\nDone\n");

    for (i = 0; i < 10; i++)
// GSList* g_slist_prepend(GSList *list, gpointer data);
        slist = g_slist_prepend(slist, &nums[i]);

    g_printf("The result should be: 9,8,7,6,5,4,3,2,1,0,9,8,7,6,5,4,3,2,1,0\n");
    g_printf("Begin:\n");
    for (i = 0; i < 20; i++) {
        st = g_slist_nth(slist, i);
        g_printf("%d,", *(gint*)st->data);
    }
    g_printf("\nDone\n");

    g_slist_free(slist);
}

/*
 * g_slist_insert_before
 * g_slist_insert
 * g_slist_nth
 * g_slist_copy
 */
static void
test_slist_2(void)
{
    GSList *slist = NULL;
    GSList *st;
    GSList *st2;
    gint nums[10] = {0,1,2,3,4,5,6,7,8,9};
    gint i;
// GSList* g_slist_insert_before(GSList *slist, GSList *sibling, gpointer data);//在sibling节点之前插入包含data数据的节点，返回新的新的slist链表
    slist = g_slist_insert_before(NULL, NULL, &nums[1]);//在Null中插入1，相当于创建了一个链表slist，并且其中第一个数据是1

// GSList* g_slist_insert(GSList *list, gpointer data, gint position);//插入position位置data数据，如果位置为负数或者大于数据项则尾插
    slist = g_slist_insert(slist, &nums[3], 1);//尾插3，链表数据:1->3
    slist = g_slist_insert(slist, &nums[4], -1);//尾插4，链表数据:1->3->4
    slist = g_slist_insert(slist, &nums[0], 0);//0位置插0，链表数据:0->1->3->4
    slist = g_slist_insert(slist, &nums[5], 100);//尾插5，链表数据:0->1->3->4->5
    slist = g_slist_insert_before(slist, NULL, &nums[6]);//最后一项的指针是null，所以是尾插6，数据:0->1->3->4->5->6
    slist = g_slist_insert_before(slist, slist->next->next, &nums[2]);//在slist的next的next也就是第2个链表项的前面加入2，数据:0->1->2->3->4->5->6

    slist = g_slist_insert(slist, &nums[9], 7);//在第七项插入9，相当于尾插，因为链表中只有6项，数据:0->1->2->3->4->5->6->9
    slist = g_slist_insert(slist, &nums[8], 7);//在第七项插入8，数据:0->1->2->3->4->5->6->8->9
    slist = g_slist_insert(slist, &nums[7], 7);//在第七项插入8，数据:0->1->2->3->4->5->6->7->8->9

    g_printf("The result should be: 0,1,2,3,4,5,6,7,8,9\n");
    g_printf("Begin:\n");
    for (i = 0; i < 10; i++) {
// GSList* g_slist_nth(GSList *list, guint n);
        st = g_slist_nth(slist, i);
        g_printf("%d,", *(gint*)st->data);
    }
    g_printf("\nDone\n");

// GSList* g_slist_copy(GSList *list);//复制链表，但是注意这个复制只是指针的复制，并没有真正的复制
    st = g_slist_copy(slist);
    g_printf("The result should be: 0,1,2,3,4,5,6,7,8,9\n");
    g_printf("Begin:\n");
    for (i = 0; i < 10; i++) {
        st2 = g_slist_nth(st, i);
        g_printf("%d,", *(gint*)st2->data);
    }
    g_printf("\nDone\n");

    *(gint*)st->next->data = 99;

    g_printf("The result should be: 0,99,2,3,4,5,6,7,8,9\n");
    g_printf("Begin:\n");
    for (i = 0; i < 10; i++) {
        st2 = g_slist_nth(slist, i);
        g_printf("%d,", *(gint*)st2->data);
    }
    g_printf("\nDone\n");

    g_slist_free(st);
    g_slist_free(slist);
}


/*
 * g_slist_lenth
 * g_slist_remove
 * g_slist_remove_all
 * g_slist_last
 * g_slist_next
 */
static void
test_slist_3(void)
{
    GSList *slist = NULL;
    GSList *st;
    gint nums[10] = {0,1,2,3,4,5,6,7,8,9};
    gint i;

    for (i = 0; i < 10; i++) {
        slist = g_slist_append(slist, &nums[i]);
        slist = g_slist_append(slist, &nums[i]);//每个数重复添加两次，链表数据为00112233445566778899
    }
// guint g_slist_lenth(GSList *list);//返回列表元素个数
    g_printf("The length should be 20.\nResult:%d\n", g_slist_length(slist));

    for (i = 0; i < 10; i++)
    {
// GSList* g_slist_remove_all(GSList *list, gconstpointer data);
        slist = g_slist_remove_all(slist, &nums[i]);//清空所有元素
    }
    g_printf("The length should be 0.\nResult:%d\n", g_slist_length(slist));

    for (i = 0; i < 10; i++) {
        slist = g_slist_append(slist, &nums[i]);
        slist = g_slist_append(slist, &nums[i]);
    }
    for (i = 0; i < 10; i++)
    {
// GSList* g_slist_remove(GSList *list, gconstpointer data);
        slist = g_slist_remove(slist, &nums[i]);//挨个删除,删除前10个,只删除第一次出现的元素
    }
    g_printf("The length should be 10.\nResult:%d\n", g_slist_length(slist));
    g_printf("The first should be 0.\nResult:%d\n", *(gint*)slist->data);//数据0123456789，第一个是0
// GSList* g_slist_last(GSList *list);
    g_printf("The last should be 9.\nResult:%d\n", *(gint*)g_slist_last(slist)->data);//最后一个是9

    g_printf("The first:");
    g_printf("%d\n", *(gint*)slist->data);
    st = slist;
    for (i = 0; i < 9; i++) {
// #define g_slist_next (slist)
        st = g_slist_next(st);    //通过next向下一个找元素
        g_printf("The next:%d\n", *(gint*)st->data);
    }

    g_slist_free(slist);
}

static gint
find_num(gconstpointer l, gconstpointer data)
{
    return *(gint*)l - GPOINTER_TO_INT(data);
}

/*
 * g_slist_index
 * g_slist_position
 * g_slist_find_custom
 */
static void
test_slist_4(void)
{
    GSList *slist = NULL;
    GSList *st;
    gint nums[10] = {0,1,2,3,4,5,6,7,8,9};
    gint i;

    for (i = 0; i < 10; i++) {
        slist = g_slist_append(slist, &nums[i]);
    }
// gint g_slist_index(GSList *list, gconstpointer data);//得到data数据所在位置，位置从0开始，如果数据没找到返回-1
    g_printf("The index should be -1.\nResult:%d\n", g_slist_index(slist, NULL));
// gint g_slist_position(GSList *list, GSList *llink);//获得被给的元素位置，从0开始，如果没有找到返回-1
    g_printf("The position should be -1.\nResult:%d\n", g_slist_position(slist, NULL));

    for (i = 0; i < 10; i++) {
// GSList* g_slist_find_custom(GSList *list, gconstpointer data, GCompareFunc func);
        st = g_slist_find_custom(slist, GINT_TO_POINTER(i), find_num);//Finds an element in a GSList, using a supplied function to find the desired element.
        g_printf("The position should be %d.\nResult:%d\n", i, g_slist_position(slist, st));
    }

    g_slist_free(slist);
}

#define SIZE        10
#define NUMBER_MAX    99

static guint32 array[SIZE];

static gint
sort(gconstpointer p1, gconstpointer p2)
{
    gint32 a, b;
    a = GPOINTER_TO_INT (p1);
    b = GPOINTER_TO_INT (p2);

    return (a > b ? +1 : a == b ? 0 : -1);
}

static gint
sort_r(gconstpointer p1, gconstpointer p2)
{
    gint32 a, b;
    a = GPOINTER_TO_INT (p1);
    b = GPOINTER_TO_INT (p2);

    return (a < b ? +1 : a == b ? 0 : -1);
}

/*
 * g_slist_sort
 * g_slist_sort_with_data
 * g_slist_nth_data
 */
static void
test_slist_5(void)
{
    GSList *slist = NULL;
    gint i;

    for (i = 0; i < SIZE; i++)
        slist = g_slist_append(slist, GINT_TO_POINTER(array[i]));//把数组放进链表75,91,53,85,63,80,10,19,41,14,

// GSList* g_slist_sort(GSList *list, GCompareFunc compare);
    slist = g_slist_sort(slist, sort);//使用sort函数排列链表

    g_printf("The result should be sorted.\nResult:");
    for (i = 0; i < SIZE; i++) {
// gpointer g_slist_nth_data(GSList *list, guint n);
        gpointer p = g_slist_nth_data(slist, i);//返回gpointer类型数据
        g_printf("%d,", GPOINTER_TO_INT(p));
    }
    g_printf("\n");

// GSList* g_slist_sort_with_data(GSList *list, GCompareDataFunc compare_func, gpinter user_data);
    slist = g_slist_sort_with_data(slist, (GCompareDataFunc)sort_r, NULL);
    g_printf("The result should be sorted[reversed].\nResult:");
    for (i = 0; i < SIZE; i++) {
        gpointer p = g_slist_nth_data(slist, i);
        g_printf("%d,", GPOINTER_TO_INT(p));
    }
    g_printf("\n");

    g_slist_free(slist);
}

static void
print(gpointer p1, gpointer p2)
{
    g_printf("%d,", GPOINTER_TO_INT(p1));
}

/*
 * g_slist_insert_sorted
 * g_slist_insert_sorted_with_data
 * g_slist_concat
 * g_slist_foreach
 */
static void
test_slist_6(void)
{
    GSList *slist = NULL;
    GSList *st = NULL;
    GSList *sc = NULL;
    gint i;

    for (i = 0; i < SIZE; i++) {
// GSList* g_slist_insert_sorted(GSList *list, gpointer data, GCompareFunc func);
        slist = g_slist_insert_sorted(slist, GINT_TO_POINTER(array[i]), sort);//按照排序规则插入，也就是插入的时候进行排序
// GSList* g_slist_insert_sorted_with_data(GSList *list, gpointer data,
//                            GCompareDataFunc func,
//                            gpinter user_data);
        st = g_slist_insert_sorted_with_data(st, GINT_TO_POINTER(array[i]),
                            (GCompareDataFunc)sort_r,
                            NULL);
    }

    g_printf("The result should be sorted.\nResult:");
    for (i = 0; i < SIZE; i++) {
        gpointer p = g_slist_nth_data(slist, i);
        g_printf("%d,", GPOINTER_TO_INT(p));
    }
    g_printf("\n");

    g_printf("The result should be sorted[reversed].\nResult:");
    for (i = 0; i < SIZE; i++) {
        gpointer p = g_slist_nth_data(st, i);
        g_printf("%d,", GPOINTER_TO_INT(p));
    }
    g_printf("\n");

// GSList* g_slist_concat(GSList *list1, *list2);
    sc = g_slist_concat(slist, st);

    g_printf("The result should be concated.\nResult:");
// void g_slist_foreach(GSList *list, GFunc func, gpointer user_data);
    g_slist_foreach(sc, (GFunc)print, NULL);
    g_printf("\n");

    g_slist_free(slist);
    g_slist_free(st);

}

int
main(void)
{
    printf("BEGIN:\n************************************************************\n");
    printf("\n------------------------------------------------------------\ntest_slist_1:\n");
    printf("------------------------------------------------------------\n");
    test_slist_1();
    printf("\n------------------------------------------------------------\ntest_slist_2:\n");
    printf("------------------------------------------------------------\n");
    test_slist_2();
    printf("\n------------------------------------------------------------\ntest_slist_3:\n");
    printf("------------------------------------------------------------\n");
    test_slist_3();
    printf("\n------------------------------------------------------------\ntest_slist_4:\n");
    printf("------------------------------------------------------------\n");
    test_slist_4();

    int i;
    srand((unsigned)time(0));
    printf("The init array is:\n");
    for (i = 0; i < SIZE; i++) {
        array[i] = rand() % (NUMBER_MAX+1);
        printf("%d,", array[i]);
    }
    printf("\n");

    printf("\n------------------------------------------------------------\ntest_slist_5:\n");
    printf("------------------------------------------------------------\n");
    test_slist_5();

    printf("\n------------------------------------------------------------\ntest_slist_6:\n");
    printf("------------------------------------------------------------\n");
    test_slist_6();

    printf("\n************************************************************\nDONE\n");

    return 0;

}