
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <netinet/tcp.h>

#include <log.h>
#include <conn.h>
#include <status.h>
#include <net.h>



#define _EL_ADD_READ(ctx) do { \
        if ((ctx)->ev.addRead) (ctx)->ev.addRead((ctx)->ev.data); \
    } while(0)
#define _EL_DEL_READ(ctx) do { \
        if ((ctx)->ev.delRead) (ctx)->ev.delRead((ctx)->ev.data); \
    } while(0)
#define _EL_ADD_WRITE(ctx) do { \
        if ((ctx)->ev.addWrite) (ctx)->ev.addWrite((ctx)->ev.data); \
    } while(0)
#define _EL_DEL_WRITE(ctx) do { \
        if ((ctx)->ev.delWrite) (ctx)->ev.delWrite((ctx)->ev.data); \
    } while(0)
#define _EL_CLEANUP(ctx) do { \
        if ((ctx)->ev.cleanup) (ctx)->ev.cleanup((ctx)->ev.data); \
    } while(0);



static btch_conn_t* btch_conn_init(){
     
    btch_conn_t* c = calloc(1,sizeof(btch_conn_t));
    if (c == NULL)
        return NULL;

    c->err = 0;
    
    c->tcp.host = NULL;
    c->tcp.source_addr = NULL;
    c->timeout = NULL;

    c->fd =  socket(AF_INET, SOCK_STREAM, 0);

    return c;
}

static void btch_conn_close_fd(btch_conn_t *c) {
    if (c && c->fd >= 0) {
        close(c->fd);
        c->fd = -1;
    }
}


static int btch_conn_set_nodelay(btch_conn_t *c) {
    int yes = 1;
    if (setsockopt(c->fd, IPPROTO_TCP, TCP_NODELAY, &yes, sizeof(yes)) == -1) {
        log.warn("setsockopt(TCP_NODELAY) failed");
        return BTCH_FAIL;
    }
    return BTCH_OK;
}

static int btch_connect_tcp(btch_conn_t *c, const char* ip, int port){
   
    c->connection_type = BTCH_CONN_TCP;
    c->tcp.port = port;

    int s = c->fd;

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_port = htons(port);

    socklen_t  len = sizeof(struct sockaddr_in);

    if (connect(s,&addr, &len) == -1) {
        if (c->flags & BTCH_CONN_BLOCK != 0)
        {
            if (errno == EHOSTUNREACH)
            {
                btch_conn_close_fd(c);
                return BTCH_ERROR;
            }
        }
    }

    if (c->tcp.host)
        free(c->tcp.host);

    c->tcp.host = strdup(ip);
    c->tcp.port = port;
    
    return BTCH_OK;
}


btch_conn_t *btch_connect(const char *ip, int port)
{
    btch_conn_t *c = btch_conn_init();
    c->flags |= BTCH_CONN_BLOCK;
    btch_connect_tcp(c, ip, port);
    return c;
}

btch_conn_t *btch_connect_with_timeout(const char *ip, int port, const struct timeval tv)
{
    btch_conn_t *c = btch_conn_init();
    
}

btch_conn_t *btch_connect_nonblock(const char *ip, int port)
{
    btch_conn_t *c = btch_conn_init();
    c->flags &= ~BTCH_CONN_BLOCK;
    btch_connect_tcp(c, ip, port);
    return c;
}

static int btch_shift_callback(btch_callback_list *list, btch_callback *target) {
    btch_callback *cb = list->head;
    if (cb != NULL) {
        list->head = cb->next;
        if (cb == list->tail)
            list->tail = NULL;

        /* Copy callback from heap to stack */
        if (target != NULL)
            memcpy(target,cb,sizeof(*cb));
        free(cb);
        return BTCH_OK;
    }
    return BTCH_ERROR;
}

static void btch_run_callback(btch_conn_t *c, btch_callback *cb) {
    if (cb->fn != NULL) {
        cb->fn(c,NULL,cb->privdata);
    }
}

static void btch_conn_free(btch_conn_t *c){
    if (c == NULL)
        return;
    if (c->fd > 0)
        close(c->fd);
    if (c->tcp.host)
        free(c->tcp.host);
    if (c->tcp.source_addr)
        free(c->tcp.source_addr);
    free(c);
}

void btch_connect_free(btch_conn_t *c)
{
   
    btch_callback cb;
   

    /* Execute pending callbacks with NULL reply. */
    while (btch_shift_callback(&c->replies,&cb) == BTCH_OK)
        btch_run_callback(c,&cb);

    _EL_CLEANUP(c);

    /* Execute disconnect callback. When redisAsyncFree() initiated destroying
     * this context, the status will always be REDIS_OK. */
    if (c->onDisconnect && (c->flags & BTCH_CONN_CONNECTED)) {
        c->onDisconnect(c, BTCH_OK);
    }

    /* Cleanup self */
    btch_conn_free(c);
}

int btch_conn_blocking(btch_conn_t* c, int blocking){
    int flags;

    /* Set the socket nonblocking.
     * Note that fcntl(2) for F_GETFL and F_SETFL can't be
     * interrupted by a signal. */
    if ((flags = fcntl(c->fd, F_GETFL)) == -1) {
        log.warn("fcntl get failed, conn:%d", c->fd);
        btch_connect_free(c);
        return BTCH_ERROR;
    }

    if (blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;

    if (fcntl(c->fd, F_SETFL, flags) == -1) {
        log.warn("fcntl set failed, conn:%d", c->fd);
        btch_connect_free(c);
        return BTCH_ERROR;
    }

    
    return BTCH_OK;
}

void btch_conn_resuse(btch_conn_t* c)
{
    char n = 1;
    if (setsockopt(c->fd, SOL_SOCKET, SO_REUSEADDR, (char *)&n,
                   sizeof(n)) < 0)
    {
        log.warn("conn reuse failed, conn:", c->fd);
    }
}