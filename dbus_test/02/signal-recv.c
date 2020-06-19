#include <dbus/dbus.h>
#include <stdio.h>
#include "mylog.h"
#include <stdint.h>
#include <unistd.h>

void listen_signal()
{
    DBusMessage *msg;
    DBusMessageIter arg;
    DBusConnection *connection;
    DBusError err;
    int ret;
    char *sigvalue;
    dbus_error_init(&err);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err)) {
        myloge("connect fail,%s", err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL) {

        return;
    }
    ret = dbus_bus_request_name(connection, "com.xxx.signal.dst", DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if(dbus_error_is_set(&err)) {
        myloge("name error:%s", err.message);
        dbus_error_free(&err);
    }
    if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER) {
        return;
    }
    //tell dbus daemon, I want to match
    dbus_bus_add_match(connection, "type='signal',interface='com.xxx.signal.Type'", &err);
    //flush to dbus daemon
    dbus_connection_flush(connection);
    if(dbus_error_is_set(&err)) {
        myloge("match fail, %s", err.message);
        dbus_error_free(&err);
    }
    dbus_bool_t bool_ret;
    while(1) {
        dbus_connection_read_write(connection, 0);
        msg = dbus_connection_pop_message(connection);
        if(msg == NULL) {
            sleep(1);
            mylogd("message is null");
            continue;
        }
        if(dbus_message_is_signal(msg, "com.xxx.signal.Type", "Test")) {
            bool_ret = dbus_message_iter_init(msg, &arg);
            if(!bool_ret) {
                myloge("no param");

            } else if(dbus_message_iter_get_arg_type(&arg) != DBUS_TYPE_STRING) {
                myloge("param is not string");
            } else {
                dbus_message_iter_get_basic(&arg, &sigvalue);
            }
            mylogd("get signal:%s", sigvalue);
        }
        dbus_message_unref(msg);
    }
}
int main()
{
    listen_signal();
}
