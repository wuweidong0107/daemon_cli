# Intro
A simple daemon example.

Features:
- Run as daemon process.
- Write pid file.
- Read commands from socket.
- Log to file or stdout
- Handle signal

# Usage
```Bash
$ sudo ./daemon -h
Usage:
  --daemon           Run as daemon process
  --socket <socket>  Read commands from socket
  --log <filename>   Log to file
  --verbose          Enable verbose mode

sudo ./daemon-client -h
Usage:
  --socket <socket>
  --socket <socket> --cmd1
  --socket <socket> --cmd2 <arg1> <arg2> <...>
  --socket <socket> --quit
```

### About log
```Bash
$ sudo ./daemon --socket /var/run/daemon.socket --verbose
11:37:36 DEBUG daemon.c:155: msg1
11:37:36 INFO  daemon.c:156: msg2
11:37:36 WARN  daemon.c:157: msg3

$ sudo ./daemon --socket /var/run/daemon.socket --log /var/log/daemon.log --verbose
$ cat /var/log/daemon.log
2022-10-01 11:36:32 DEBUG daemon.c:155: msg1
2022-10-01 11:36:32 INFO  daemon.c:156: msg2
2022-10-01 11:36:32 WARN  daemon.c:157: msg3
```

### About server / client
```Bash
$ sudo ./daemon --socket /var/run/daemon.socket --verbose
15:48:43 DEBUG daemon.c:156: msg1
15:48:43 INFO  daemon.c:157: msg2
15:48:43 WARN  daemon.c:158: msg3
15:48:55 INFO  command.c:32: cmd: CMD_1
15:49:00 INFO  command.c:37: cmd=CMD_2, param=arg1
15:49:00 INFO  command.c:37: cmd=CMD_2, param=arg2
15:49:47 INFO  command.c:44: cmd=CMD_QUIT, param=
15:49:47 INFO  daemon.c:49: cleanup...

$ sudo ./daemon-client --socket /var/run/daemon.socket --cmd1
$ sudo ./daemon-client --socket /var/run/daemon.socket --cmd2 arg1 arg2
$ sudo ./daemon-client --socket /var/run/daemon.socket --quit
```

### About pid file lock
```Bash
$ sudo ./daemon --socket /var/run/daemeon.socket --pid /var/run/daemon.pid --verbose
16:39:29 DEBUG daemon.c:131: msg1
16:39:29 INFO  daemon.c:132: msg2
16:39:29 WARN  daemon.c:133: msg3

$ sudo ./daemon --socket /var/run/daemeon.socket --pid /var/run/daemon.pid --verbose
16:40:09 DEBUG daemon.c:131: msg1
16:40:09 INFO  daemon.c:132: msg2
16:40:09 WARN  daemon.c:133: msg3
16:40:09 ERROR pid.c:60: PID file '/var/run/daemon.pid' is locked. ./daemon is already running
```