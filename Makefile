TARGET=daemon daemon-client

all: $(TARGET)

daemon: daemon.c cmdsocket.c command.c log.c pid.c
	gcc $^ -o $@

daemon-client: daemon-client.c cmdsocket.c log.c
	gcc $^ -o $@

clean:
	rm -f ./daemon ./daemon-client