# Laboratory Work 9: Watching System Calls with strace

## Development environment

The experiments were performed in Ubuntu Linux using `strace` version **6.19** and GCC. The source file was stored in `~/os-lab9/syscalls.c`. The program creates `note.txt` in its working directory.

## Part 1. See the calls of a real command

Command executed:

```bash
strace -f -e trace=openat,read,write,close ls -l
```

The terminal trace contained `openat`, `read`, `close` and `write` operations. The directory listing was displayed using:

```text
write(1, "total 60\ndrwxr-xr-x 2 lina lina "..., 745) = 745
+++ exited with 0 +++
```

**Observe: Which system call does `ls` use to print to the screen? To which descriptor does it write?**

`ls` uses the `write()` system call with file descriptor **1** (`stdout`). In this run, it successfully wrote **745 bytes** to the terminal.

## Part 2. A program that calls the kernel directly

The source program `syscalls.c` opens `note.txt`, writes `hello, kernel\n`, closes the file, reopens it and reads the bytes back.

Commands executed:

```bash
gcc -Wall -Wextra -o syscalls syscalls.c
./syscalls
strace -e trace=openat,write,read,close ./syscalls
```

Actual program output:

```text
open returned descriptor 3
write sent 14 bytes
read got 14 bytes: hello, kernel
```

Relevant observed system-call sequence:

```text
openat(AT_FDCWD, "note.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644) = 3
write(3, "hello, kernel\n", 14) = 14
close(3) = 0
openat(AT_FDCWD, "note.txt", O_RDONLY) = 3
read(3, "hello, kernel\n", 63) = 14
close(3) = 0
```

The trace also includes dynamic-loader and C-library startup calls, and `write(1, ...)` calls for the `printf()` messages.

**Observe: Why is the descriptor returned by the first `openat()` 3, not 0, 1 or 2?**

File descriptors **0**, **1** and **2** are normally assigned to standard input (`stdin`), standard output (`stdout`) and standard error (`stderr`). Therefore, the next free descriptor is **3**. When the file is closed, descriptor 3 becomes available for reuse.

## Part 3. Count one kind of call (supplementary experiment)

The provided Lab 9 handout names Part 3 but does not supply its detailed instructions. The following summary command was additionally executed:

```bash
strace -c -e trace=openat,read,write,close ./syscalls
```

Actual summary from this execution:

| System call | Calls | Time (%) |
|---|---:|---:|
| `close` | 4 | 59.39 |
| `openat` | 4 | 37.71 |
| `read` | 2 | 1.77 |
| `write` | 4 | 1.13 |
| **Total** | **14** | **100.00** |

Total reported tracing time: **0.003620 seconds**. These timings are specific to this execution and may vary between runs. The four `write()` calls include one file write and three messages written to standard output.

## Conclusion

The experiments demonstrate the relationship between C file operations and Linux system calls. The `ls` trace identified file descriptor 1 as the terminal output stream. The custom program wrote and read back 14 bytes using descriptor 3, and the supplementary summary recorded 14 selected calls. All stated measurements were taken from the observed Ubuntu terminal output.
