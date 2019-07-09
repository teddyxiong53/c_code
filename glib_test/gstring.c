
#include <glib.h>

int main(int argc,char **argv) {

//    GString* g_string_new(const gchar *init);   根据所给的字符串建立适当长度的GString，并自动创建适当长度的allocated_len，创建的时候将字符串init复制到GString中。
    GString *string = g_string_new("Justin");
    g_print("g_string_new(\"Justin\"); len = %d, allocated_len = %d\n",
            string->len, string->allocated_len);

//    若不使用GString时，可以使用g_string_free()释放，第二个参数如果为TRUE，会连同字符串也是放掉。
    g_string_free(string, TRUE);

//    GString* g_string_new_len(const gchar *init, gssize len);     指定len来建立GString，因为是自行制定的长度，所以len必须超过init的长度。
    string = g_string_new_len("Justin", 32);
    g_print("g_string_new_len(\"Justin\", 32); len = %d, allocated_len = %d\n",
            string->len, string->allocated_len);
    g_string_free(string, TRUE);

//    GString* g_string_sized_new(gsize dfl_size);      指定allocated_len建立GString，函数会根据需要的长度值自动分配长度，也就是不一定会分配设置的长度，但一定会比设定的长度大。
    string = g_string_sized_new(32);
    g_printf("g_string_sized_new(32); len = %d, allocated_len = %d\n",
            string->len, string->allocated_len);
    g_string_free(string, TRUE);


    GString *t=g_string_new("Hello Laomeng.");
    GString *s=g_string_new("Hello Laoli.");

//    g_string_equal()    判断两个GString的字符串内容是否相同
    if(g_string_equal(s,t))
    {
        g_print("g_string_equal(s,t); %s  == %s\n",s->str,t->str);
    }
    else
    {
        g_print("g_string_equal(s,t); %s != %s\n",s->str,t->str);
    }
    g_string_free(s, TRUE);
    g_string_free(t, TRUE);

    string = g_string_new("first str!");
//     GString* g_string_append (GString *string,const gchar *val);     在字符串string后面追加字符串val
    g_string_append(string, "second str!");
    g_print("g_string_append(string, \"second str!\"); %s\n", string->str);

//    GString* g_string_truncate(GString *string,gsize len); 截断字符串，保留前len个字符
    g_string_truncate(string,5);
    g_print("g_string_truncate(string,5);  string:  %s, len = %d, allocated_len = %d\n",string->str, string->len , string->allocated_len);

//     GString* g_string_prepend (GString *string,const gchar *val);     在字符串string前面追加字符串val
    g_string_prepend(string, "prepend str!");
    g_print("g_string_prepend(string, \"prepend str!\"); string:%s\n", string->str);

//     GString* g_string_insert (GString *string,gssize pos,const gchar *val);     将字符串插入到pos处
    g_string_insert(string, sizeof("prepend str!")-1 , "insert str!");
    g_print("g_string_insert(string, sizeof(\"prepend str!\") , \"insert str!\"); %s\n", string->str);

//    void g_string_printf(GString *string, const gchar *format, ...);  格式化一个字符串，和sprintf用法一样，只是赋值给一个GString，string里面以前的数据被销毁了
    g_string_printf(string,"%d + %d = %d", 100, 200, 100+200);
    g_print("g_string_printf(); GString: %s, len = %d, allocated_len = %d \n",string->str, string->len , string->allocated_len);

//    void g_string_append_printf(GString *string, const gchar *format, ...);  格式化一个字符串，和g_string_printf很相似，只是不销毁string里面的数据，而是在后面追加
    g_string_append_printf(string,"\t %d * %d = %d", 100,200,100*200);
    g_print("g_string_append_printf(); GString: %s, len = %d, allocated_len = %d \n",string->str, string->len , string->allocated_len);

    g_string_free(string,TRUE);

    string = g_string_new("abcdefgABCDEFG");

//    g_string_ascii_up()或g_utf8_strup() 转换GString中的字符串为小写
    g_string_ascii_up(string);
    g_printf("Upper: %s\n", string->str);
//    g_string_ascii_down()或g_utf8_strdown()转换GString中的字符串为大写
    g_string_ascii_down(string);
    g_printf("Down: %s\n", string->str);

    g_string_free(string,TRUE);

    return 0;
}