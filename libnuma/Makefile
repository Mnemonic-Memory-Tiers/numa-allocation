CC=clang
# CFLAGS=-std=c99
CFLAGS=-W -Wall
#CFLAGS+=-O3
# CFLAGS+=-O0 -g -ggdb
# CFLAGS+=-MMD  # generate dependency .d files
# LDLIBS=
LDFLAGS=-lnuma

SRCS=numa_alloc.c
EXEC=numa_alloc

OBJS=$(SRCS:.c=.o)
DEPS=$(OBJS:.o=.d)

.DEFAULT_GOAL=all
.PHONY: all
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(DEPS)

%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -f $(OBJS) $(EXEC) $(DEPS)

run: all
	./$(EXEC)
