# Intro
A simple daemon example.

Features:
- Run as daemon process.
- Write pid file.
- Read commands from socket.
- Log to file or stdout
- Handle signal

# Usage
```
$ sudo ./daemon --socket /var/run/dameon.socket --verbose
11:37:36 DEBUG daemon.c:155: msg1
11:37:36 INFO  daemon.c:156: msg2
11:37:36 WARN  daemon.c:157: msg3

$ sudo ./daemon --socket /var/run/dameon.socket --log /var/log/daemon.log --verbose
$ cat /var/log/daemon.log
2022-10-01 11:36:32 DEBUG daemon.c:155: msg1
2022-10-01 11:36:32 INFO  daemon.c:156: msg2
2022-10-01 11:36:32 WARN  daemon.c:157: msg3

```