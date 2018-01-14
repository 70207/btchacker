TS1_SRC = ./test/ts1.c
TS1_OBJ = ./test/ts1.o

EVENT_SRC = $(wildcard event/*.c)
EVENT_OBJ = $(patsubst %.c, %.o, $(EVENT_SRC))

UTIL_SRC = $(wildcard util/*.c)
UTIL_OBJ = $(patsubst %.c, %.o, $(UTIL_SRC))

NET_SRC = $(wildcard net/*.c)
NET_OBJ = $(patsubst %.c, %.o, $(NET_SRC))


INC = -I./event -I./util -I./net
CFLAGS += $(INC)


ts1: $(TS1_OBJ) $(EVENT_OBJ) $(UTIL_OBJ) $(NET_OBJ)
	gcc -o $@ $^


./event/%.o:./event/%.c
	gcc -c -o $@ $^ $(INC)

./util/%.o:./util/%.c
	gcc -c -o $@ $^ $(INC)

./net/%.o:./net/%.c
	gcc -c -o $@ $^ $(INC)

clean:
	rm -rf ts1
	rm -rf $(EVENT_OBJ)
	rm -rf $(UTIL_OBJ)
	rm -rf $(TS1_OBJ)
	rm -rf $(NET_OBJ)