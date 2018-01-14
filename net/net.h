#ifndef __BTCH_NET_H__
#define __BTCH_NET_H__


#define BTCH_CONN_BLOCK                 0x1
#define BTCH_CONN_CONNECTED             0x2
#define BTCH_REUSE_ADDR                 0x80


enum btch_conn_type {
    BTCH_CONN_TCP,
    BTCH_CONN_UNIX
};



#endif