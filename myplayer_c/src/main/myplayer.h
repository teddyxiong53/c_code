#ifndef __MY_PLAYER_H__
#define __MY_PLAYER_H__

#include <stdint.h>

#define URI_LEN 1024
struct myplayer {
    char uri[1024];
    void *priv;
    int (*init)(struct player *player);
    void (*deinit)();
    void (*set_uri)( char *uri);
    int (*start)();
    int (*stop)();
    int (*pause)();
    int (*resume)();
    int (*seek)( int64_t pos);
    int (*set_speed)( float speed);

};
#endif
