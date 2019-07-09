#include <glib.h>

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
    {9,"nine"}
};

typedef struct map map;

#define NUMS    (sizeof(m)/sizeof(m[0]))

static void
test_relation(void)
{
// GRelation* g_relation_new(gint fields);      创建新的关系表，fields域的数量，现在这个值必须是2，也就是一个key域，一个value域
    GRelation *relation = g_relation_new(2);

// void g_relation_index(GRelation *relation, gint field, GHashFunc hash_func, GEqualFunc key_equal_func);创建一个域的索引
    g_relation_index(relation, 0, g_int_hash, g_int_equal);
    g_relation_index(relation, 1, g_str_hash, g_str_equal);

// void g_relation_insert(GRelatioin *relation, ...);   插入记录到关系表中，...的参数必须与域对应
    gint i;
    for (i = 0; i < NUMS; i++)
        g_relation_insert(relation, &m[i].key, m[i].value);

// gint g_relation_count(GRelation *relation, gconstpointer key, gint fields);  在域fields中key值对应的元组值
    g_printf("The '%d' should be exist '%d' times now.\t\tResult: %d.\n",
            m[1].key, 1, g_relation_count(relation, &m[1].key, 0));

// gboolean g_relation_exists(GRelation *relation, ...);    被给的键值如果在关系表中存在则返回true
    gboolean b = g_relation_exists(relation, &m[1].key, m[1].value);
    g_printf("The key: '%d' and value: '%s' should be %sfound now.\n",
            m[1].key, m[1].value, b ? "" : "not ");

// gint g_relation_delete(GRelation *relation, gconstpointer key, gint field);  在域field中删除所有的key，返回删除的个数
    g_printf("The key: '%d' has been found '%d' times and deleted now.\n",
            m[1].key, g_relation_delete(relation, &m[1].key, 0));

// GTuples* g_relation_select(GRelation *relation, gconstpointer key, gint field);  返回所有的key对应的元组，配合g_tuples_index使用取出记录值
// gpointer g_tuples_index(GTuples *tuples, gint index_, gint field);   从g_relation_select的返回中取出field中的元组值
// void g_tuples_destroy(GTuples *tuples);  销毁元组
    g_printf("The all records now:\n");
    for (i = 0; i < NUMS; i++) {
        GTuples *tuples = g_relation_select(relation, m[i].value, 1);
        gint j;
        for (j = 0; j < tuples->len; j++)
            g_printf("Key: %d\t\tValue: %s\n",
                    *(gint *)g_tuples_index(tuples, j, 0),
                    (gchar *)g_tuples_index(tuples, j, 1));
        g_tuples_destroy(tuples);
    }
    g_printf("\n");
// void g_relation_print(GRelation *relation);
//    g_relation_print(relation);

// void g_relation_destroy(GRelation *relation);    销毁关系表
    g_relation_destroy(relation);
}

int
main(void)
{
    g_printf("BEGIN:\n************************************************************\n");
    test_relation();
    g_printf("\n************************************************************\nDONE\n");

    return 0;
}