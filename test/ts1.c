#include <ae.h>
#include <log.h>

static aeEventLoop *loop;

#define EVENT_SIZE 64

int main(int argc, char** argv){
    loop = aeCreateEventLoop(EVENT_SIZE);
    log.debug("hello xiaofei\n");


}