#include <dbus/dbus.h>
#include <stdio.h>
#include "mylog.h"
#include <stdint.h>

int send_a_signal(char *sigvalue)
{
    DBusError err;
    DBusConnection *connection;
    DBusMessage *msg;
    DBusMessageIter arg;
    uint32_t serial = 0;
    int ret;

    dbus_error_init(&err);
    connection = dbus_bus_get(DBUS_BUS_SESSION, &err);
    if(dbus_error_is_set(&err)) {
        myloge("connect err:%s", err.message);
        dbus_error_free(&err);
    }
    if(connection == NULL) {
        return -1;
    }

    //allocate a name
    ret = dbus_bus_request_name(connection, "com.xxx.signal.src", DBUS_NAME_FLAG_ALLOW_REPLACEMENT, &err);
    if(dbus_error_is_set(&err)) {
        myloge("name error:%s", err.message);
        dbus_error_free(&err);
    }
    if(ret != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)  {
        return -1;
    }
    //send a signal
    msg = dbus_message_new_signal("/com/xxx/signal/Object", "com.xxx.signal.Type", "Test");
    if(msg == NULL) {
        myloge("get message fail");
        return -1;
    }
    //add content to message
    dbus_message_iter_init_append(msg, &arg);
    dbus_bool_t bool_ret;
    bool_ret = dbus_message_iter_append_basic(&arg, DBUS_TYPE_STRING, &sigvalue);
    if(!bool_ret) {
        myloge("allocate mem fail");
        return -1;
    }
    //send signal
    bool_ret = dbus_connection_send(connection, msg, &serial);
    if(!bool_ret) {
        myloge("send fail");
        return -1;
    }
    dbus_connection_flush(connection);
    mylogd("signal send");
    dbus_message_unref(msg);
    return 0;

}
int main()
{
    send_a_signal("hello dbus signal");
    return 0;
}
