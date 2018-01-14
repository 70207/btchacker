#ifndef __BTCH_CONN_H
#define __BTCH_CONN_H

#include <netinet/in.h>
#include <sys/types.h>  
#include <sys/socket.h>

typedef struct __btch_conn btch_conn_t;

/* Reply callback prototype and container */
typedef void (btch_callback_fn)(struct btch_conn_t*, void*, void*);
typedef struct btch_callback {
    struct btch_callback *next; /* simple singly linked list */
    btch_callback_fn *fn;
    void *privdata;
} btch_callback;

/* List of callbacks for either regular replies or pub/sub */
typedef struct btch_callback_list {
    btch_callback *head, *tail;
} btch_callback_list;

typedef void (btch_disconnect_callback)(const struct btch_conn_t*, int status);
typedef void (btch_connect_callback)(const struct btch_conn_t*, int status);

struct __btch_conn{
    int         fd;
    int         flags;
    int         connection_type;
    struct {
        struct sockaddr *host;
        struct sockaddr *source_addr;
        int port;
    } tcp;

     /* Not used by hiredis */
    void *data;

    /* Event library data and hooks */
    struct {
        void *data;

        /* Hooks that are called when the library expects to start
         * reading/writing. These functions should be idempotent. */
        void (*addRead)(void *privdata);
        void (*delRead)(void *privdata);
        void (*addWrite)(void *privdata);
        void (*delWrite)(void *privdata);
        void (*cleanup)(void *privdata);
    } ev;

    int err;
    int timeout;

    /* Called when either the connection is terminated due to an error or per
     * user request. The status is set accordingly (REDIS_OK, REDIS_ERR). */
    btch_disconnect_callback *onDisconnect;

    /* Called when the first write event was received. */
    btch_connect_callback *onConnect;

    /* Regular command callbacks */
    btch_callback_list replies;



    
};



btch_conn_t *btch_connect(const char *ip, int port);
btch_conn_t *btch_connect_with_timeout(const char *ip, int port, const struct timeval tv);
btch_conn_t *btch_connect_nonblock(const char *ip, int port);

int btch_conn_blocking(btch_conn_t* c, int blocking);
void btch_conn_reuse(btch_conn_t* c);
void btch_connect_free(btch_conn_t *c);



#endif