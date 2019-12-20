#include <gst/gst.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>


pthread_t g_player_pid;
gchar *g_filename;
GstElement *g_pipeline;
GMainContext *g_workerContext;
GMainLoop *g_mainLoop;

struct player_info {
    int64_t pos;
    int64_t duration;
};

void print_usage()
{
    printf("用法：./mygstplayer filename\n");
}
void check_args(int argc, char **argv)
{
    if(argc < 2) {
        print_usage();
        exit(1);
    }
    g_filename = argv[1];
    if(access(g_filename, F_OK) ) {
        printf("%s file not exists\n", g_filename);
        exit(1);
    }
}
void print_menu()
{
    printf("操作说明：\n");
    printf("b -- 播放\n");
    printf("z -- 暂停\n");
    printf("j -- 快进5s\n");
    printf("t -- 快退5s\n");
    printf("k -- 加快25%\n");
    printf("m -- 减慢25%\n");
    printf("x -- 查看文件信息\n");
}
gboolean   player_bus_callback (
    GstBus * bus, GstMessage * msg, gpointer user_data)
{
    switch(GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            g_print("play error\n");
            GError *err = NULL;
            gchar *dbg_info = NULL;
            gst_message_parse_error(msg, &err, &dbg_info);
            gst_element_set_state(g_pipeline, GST_STATE_READY);
            g_warning("error from element %s : %s\n", GST_OBJECT_NAME(msg->src), err->message);
            g_warning("debug info is: %s\n", dbg_info? dbg_info:"none");
            g_error_free(err);
            if(dbg_info) {
                g_free(dbg_info);
            }

            break;
        case GST_MESSAGE_EOS:
            g_print("end of stream\n");
            break;
        case GST_MESSAGE_STREAM_START:
            g_print("start stream\n");
            break;
        case GST_MESSAGE_STATE_CHANGED:
            // g_print("state changed\n");
            if(GST_MESSAGE_SRC(msg) == GST_OBJECT_CAST(g_pipeline)) {
                GstState oldState, newState;
                gst_message_parse_state_changed(msg, &oldState, &newState, NULL);
                g_print("element %s change state from %s to %s\n",
                    GST_OBJECT_NAME(msg->src),
                    gst_element_state_get_name(oldState),
                    gst_element_state_get_name(newState));

            }
            break;
        default:
            break;
    }
    return TRUE;
}

void* player_thread_proc(void *arg)
{
    gst_init(NULL, NULL);
    GstElement  *source, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;
    #if 0
    g_pipeline = gst_pipeline_new("my-pipeline");
    source = gst_element_factory_make("filesrc", "src");
    g_object_set(source, "location", g_filename, NULL);
    sink = gst_element_factory_make("autoaudiosink", "snk");
    gst_bin_add_many(GST_BIN(g_pipeline), source, sink, NULL);
    if(!gst_element_link_many(source, sink, NULL)) {
        g_print("link many fail\n");
    }
    #endif
    g_pipeline = gst_element_factory_make("playbin", "my-pipeline");
    char buf[1024] = {0};
    strcpy(buf, "file://");
    realpath(g_filename, buf+strlen("file://"));
    g_print("abs path:%s\n", buf);
    g_object_set(g_pipeline, "uri", buf, NULL);

    //可以开始播放了，但是不要播放，靠键盘输入来控制。
    //gst_element_set_state(pipeline, GST_STATE_PLAYING);
    //bus = gst_element_get_bus(pipeline);
    //gst_bus_add_watch(bus, player_bus_callback, NULL);
    g_workerContext = g_main_context_new();
    //循环阻塞在这里
    g_mainLoop = g_main_loop_new(g_workerContext, FALSE);

    g_main_context_push_thread_default(g_workerContext);
    bus = gst_pipeline_get_bus(GST_PIPELINE(g_pipeline));
    gst_bus_add_watch(bus, player_bus_callback, NULL);
    gst_object_unref(bus);
    g_main_loop_run(g_mainLoop);//这里阻塞
    g_main_context_pop_thread_default(g_workerContext);
    g_print("end of player thread\n");
    return NULL;
}
void player_play()
{
    if(g_pipeline) {
        gst_element_set_state( g_pipeline, GST_STATE_PLAYING);
    }
}
void player_pause()
{
    if(g_pipeline) {
        gst_element_set_state( g_pipeline, GST_STATE_PAUSED);
    }
}
void player_get_info(struct player_info *info)
{
    int64_t pos = 0;
    gboolean ret = gst_element_query_position(g_pipeline, GST_FORMAT_TIME, &pos);
    if(!ret) {
        g_print("query position fail\n");
        return;
    }
    g_print("current position:%lld\n", pos);
    int64_t duration = 0;
    ret = gst_element_query_duration(g_pipeline, GST_FORMAT_TIME, &duration);
    if(!ret) {
        g_print("get duration fail\n");
        return ;
    }
    g_print("duration is:%lld\n", duration);
    info->pos = pos;
    info->duration = duration;

}

void player_step(int64_t secs)
{
    struct player_info info;
    player_get_info(&info);
    GstQuery *query;
    int64_t start, end;
    query = gst_query_new_seeking(GST_FORMAT_TIME);
    gboolean seekable;
    if(gst_element_query(g_pipeline, query)) {
        gst_query_parse_seeking(query, NULL, &seekable, &start, &end);
        if(seekable) {
            g_print("seeking is enabled from %"GST_TIME_FORMAT"to %"GST_TIME_FORMAT" \n", GST_TIME_ARGS(start), GST_TIME_ARGS(end));
            int64_t target = 0;
            target = info.pos + secs*GST_SECOND;
            if(target >= end) {
                g_print("target is over end, set it to end\n");
                target = end;
            }
            if(target <start) {
                g_print("target is before start, set to to start\n");
                target = start;
            }
            gst_element_seek(g_pipeline, 1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
                GST_SEEK_TYPE_SET, target, GST_SEEK_TYPE_NONE, GST_CLOCK_TIME_NONE);
        } else {
            g_print("file is not seekable\n");
        }
    } else {
        g_print("seeking query fail\n");
    }
}

/*
    速度控制。
*/
void player_speed(int spd)
{

}
void create_player_thread()
{
    pthread_create(&g_player_pid, NULL, player_thread_proc, NULL);
}

void wait_for_player_thread()
{
    pthread_join(g_player_pid, NULL);
}
int main(int argc, char **argv)
{
    char c ;
    check_args(argc, argv);
    print_menu();

    create_player_thread();
    while(c=getchar()) {
        if(c == 'p') {
            player_play();
        } else if(c == 'e') {
            break;
        } else if(c == 's') {
            player_pause();
        } else if(c == 'j') {
            player_step(5);
        } else if(c == 't') {
            player_step(-5);
        } else if(c == 'k') {
            player_speed(25);
        } else if(c == 'm') {
            player_speed(-25);
        }
    }
    g_print("out of main loop\n");
    wait_for_player_thread();
    g_print("player end\n");
    return 0;
}
