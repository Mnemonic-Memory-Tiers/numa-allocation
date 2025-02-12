CC=clang
# CFLAGS=-std=c99

# MacOS needs -I/opt/local/include
CFLAGS=-W -Wall -I/opt/local/include

#CFLAGS+=-O3
# CFLAGS+=-O0 -g -ggdb
# CFLAGS+=-MMD  # generate dependency .d files
# LDLIBS=

# idk what is needed here ...
# LDFLAGS=-lnuma
LDFLAGS=-L/opt/local/lib -lhwloc -lnuma

SRCS=hwloc_alloc.c
EXEC=hwloc_alloc

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
