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
    {9,"nine"},
};
typedef struct map map;

static gint
myCompare(gconstpointer p1, gconstpointer p2)
{
    const char *a = p1;
    const char *b = p2;

    return *a - *b;
}

static gint
mySearch(gconstpointer p1, gconstpointer p2)
{
    return myCompare(p1, p2);
}

static gint
myTraverse(gpointer key, gpointer value, gpointer fmt)
{
    g_printf(fmt, *(gint*)key, (gchar*)value);

    return FALSE;
}

static void
test_avl_tree(void)
{
    GTree *tree;
    gint i;

// GTree* g_tree_new(GCompareFunc key_compare_func);    用来创建一个平衡二叉树，key_compare_func函数用来排序节点规则
    tree = g_tree_new(myCompare);

// void g_tree_insert(GTree *tree, gpointer key, gpointer value);   插入一个键值到树中
// 需要注意的是如果key值重复的话会将以前的值进行去除然后将新的键值加入树中，去除的包括key值
// 如果使用的是new_full函数并且实现了销毁key和value的函数，在插入重复key时会新的key被销毁
    for (i = 0; i < sizeof(m)/sizeof(m[0]); i++)
        g_tree_insert(tree, &m[i].key, m[i].value);

// void g_tree_foreach(GTree *tree, GTraverseFunc func, gpointer user_data);    遍历树，将键值依次传入func函数中
    g_printf("Now the tree:\n");
    g_tree_foreach(tree, myTraverse, "Key:\t%d\t\tVaule:\t%s\n");
// gint g_tree_nnodes(GTree *tree);     取得树中的节点数
    g_printf("The tree should have '%d' items now.\t\tResult: %d.\n", 10, g_tree_nnodes(tree));
// gint g_tree_height(GTree *tree);     取得树的高度，如果是空树将返回0，如果只有根节点返回1，以此类推
    g_printf("The height of tree is '%d' now.\n", g_tree_height(tree));

// void g_tree_replace(GTree *tree, gpointer key, gpointer value);  将key的值用value替换，功能和g_tree_insert很相似，只是在销毁的时候会去销毁旧的key，而不是新的
    g_tree_replace(tree, &m[3].key, "3333");
    g_printf("Now the vaule of '%d' should be '3333' now.\n", m[3].key);
    g_tree_foreach(tree, myTraverse, "Key:\t%d\t\tVaule:\t%s\n");

    gchar *tmp = NULL;
// gpointer g_tree_lookup(GTree *tree, gconstpointer key);  查找key的值
    g_printf("Now the vaule of '%d' should be '%s' now[lookup].\n", m[3].key, (tmp = (gchar *)g_tree_lookup(tree, &m[3].key)) != NULL ? tmp : NULL);

// gboolean g_tree_remove(GTree *tree, gconstpointer key);  移除key对应的键值，需要注意的是在2.0版本中这个函数返回类型是void，在之后的版本中返回是bool，代表是否成功
//    gboolean b = g_tree_remove(tree, &m[3].key);
    g_tree_remove(tree, &m[3].key);
    g_printf("The key '%d' has been found and removed now.\n", m[3].key);

// gpointer g_tree_search(GTree *tree, GCompareFunc search_func, gconstpointer user_data);  通过search_func形式去查找树，可以使用这种方式暂时改变查找方式，比如正序或倒序
    g_printf("Now the vaule which should be removed of '%d' should be '%s' now[search].\n",
            m[3].key,
            (tmp = (gchar *)g_tree_search(tree, mySearch, &m[3].key)) != NULL ? tmp : NULL);

    g_printf("Now the tree look like:\n");
    g_tree_foreach(tree, myTraverse, "Key:\t%d\t\tVaule:\t%s\n");

// void g_tree_destroy(GTree *tree);释放树，如果使用g_tree_new_full()函数创建的树会使用销毁函数同时销毁键值
    g_tree_destroy(tree);
}

int
main(void)
{
    g_printf("BEGIN:\n************************************************************\n");

    test_avl_tree();

    g_printf("\n************************************************************\nDONE\n");

    return 0;
}