# Operating Systems: Laboratory Work 2
## Meet the OS You Will Build (xv6)

## Part 2: Using xv6

### 1. List three programs xv6 ships with.

Three programs used during the laboratory were:

- `ls`
- `cat`
- `echo`

### 2. Which two OS features must exist for a pipe to work?

A pipe requires:

- process management;
- inter-process communication.

For a command such as:

```text
ls | grep c
```

the shell runs separate processes and the operating system connects the output of one process to the input of another through a kernel-managed pipe.

### 3. How does the xv6 shell compare to the Linux shell from Lab 1?

The xv6 shell provides the same basic Unix concepts, such as executing commands, working with files, and using pipes, but it is much simpler and has fewer commands and features than a modern Linux shell.

## Part 3: Reading the Source

### 1. Which system calls does `user/cat.c` use, and what does each one ask the kernel to do?

The program uses system-call interfaces including:

- `read()`: reads data from a file descriptor;
- `write()`: writes data to a file descriptor;
- `open()`: opens a file;
- `close()`: closes a file descriptor;
- `exit()`: terminates the process.

These calls allow the user program to request protected file and device operations from the kernel.

### 2. Where is `sys_read` implemented?

In the repository version used for this laboratory:

```text
kernel/sysfile.c
```

`sys_read` was located at line:

```text
69
```

`sys_write` was located at line:

```text
83
```

### 3. What is the difference between `kernel/` and `user/`?

The `kernel/` directory contains privileged operating-system code, including scheduling, memory management, filesystem functionality, and system-call handlers.

The `user/` directory contains ordinary user-space programs such as `ls`, `cat`, `echo`, and the shell. These programs request operating-system services through system calls instead of directly accessing privileged kernel functionality.

## Big Idea

In Linux, the operating system was mainly observed from the outside using commands and interfaces such as `/proc`. In xv6, both sides can be examined directly: the user program that requests a service and the kernel code that implements it.

## Sleep Program Note

A new xv6 user command named `sleep` was added in `user/sleep.c` and included in the `UPROGS` list in the Makefile.

The current xv6 repository revision used during the laboratory exposes the delay interface as:

```c
pause(int);
```

instead of the older user-space `sleep(int)` interface shown in the laboratory sheet. Therefore, the command remained named `sleep`, while the implementation used:

```c
pause(atoi(argv[1]));
```

The program was successfully rebuilt and tested.

Running:

```text
sleep 10
```

paused and then returned to the xv6 shell.

Running:

```text
sleep
```

without an argument displayed:

```text
usage: sleep <ticks>
```

The optional test suite:

```text
usertests -q
```

also completed successfully with:

```text
ALL TESTS PASSED
```
