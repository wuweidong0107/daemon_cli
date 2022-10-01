TARGET=daemon

all: $(TARGET)

daemon: daemon.c cmdsocket.c command.c log.c
	gcc $^ -o $@

clean:
	rm -f daemon