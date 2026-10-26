# Laboratory Work 7: Three Small Programs (Linux)

## Purpose

To investigate process creation and replacement, zombie processes, file-descriptor redirection, and pipes on Linux, using small C programs. All commands were run in Ubuntu. The recorded results below come from the laboratory run.

## Part 1: `fork`, `exec`, and `wait`

`run.c` creates a child with `fork()`, runs another program using `execlp()`, and collects the child's termination status with `waitpid()`. It was compiled with:

```bash
gcc -Wall -Wextra -o run run.c
./run
```

The successful `ls -l` test showed parent PID **7234**, child PID **7235** (whose parent PID was **7234**), and exit status **0**. The `date` variant also completed successfully. A nonexistent program was then tested by replacing the `execlp()` call with `execlp("nosuchprog", "nosuchprog", (char *)NULL);`.

### Observe

**1. The line `perror("exec")` is in the child, right after `execlp`. Why is it printed only with `nosuchprog`?**

When `execlp()` succeeds, it replaces the child process's current program with the requested executable. Control does not return to the next statement of `run.c`, so `perror("exec")` is never reached during a successful `ls` or `date` execution. When `nosuchprog` cannot be found, `execlp()` returns an error and execution continues at `perror("exec")`, which prints `No such file or directory`.

**2. Which number does the child print as "my parent", and whose PID is it?**

In the recorded successful run, the child printed **7234** as `my parent`. This is the process ID of the original `run` program, which created the child with `fork()`. The child itself had PID **7235**, so the two values correctly identify the parent-child relationship.

**3. What status does the parent see with `ls`, and what status with `nosuchprog`?**

The parent received status **0** when the child successfully executed `ls`. With `nosuchprog`, it received status **127**. This was not an exit code supplied by `execlp()` directly: after `execlp()` failed, the child's code explicitly called `exit(127)`, and `waitpid()` collected that result. The actual failed execution was:

```text
child: my PID is 7606, my parent is 7605
exec: No such file or directory
parent: child 7606 ended with status 127
```

The final submitted `run.c` was restored to execute `ls -l`.

## Part 2: A zombie, on purpose

`zombie.c` is based on the original `run.c` and includes `sleep(20);` immediately before `waitpid()`. It was tested with:

```bash
gcc -Wall -Wextra -o zombie zombie.c
./zombie > /dev/null &
sleep 2; ps -o pid,ppid,stat,cmd
```

### Observe

**1. Which letter does `ls` have in the `STAT` column, and what does it mean?**

The child appeared with state **`Z`**, meaning *zombie*. Its `ls` program had already finished, but the parent was still sleeping and had not yet collected its termination status. Linux therefore retained a process-table entry for the terminated child:

```text
7964    5697 S    ./zombie
7966    7964 Z    [ls] <defunct>
```

Here PID **7966** belonged to `ls`, and **7964** was its parent. The `S` state indicates that the parent was sleeping.

**2. After 20 seconds, is the `[ls] <defunct>` line still there? Which line of `zombie.c` made it disappear?**

No. On the subsequent `ps -o pid,ppid,stat,cmd` check, the zombie entry was absent and Bash reported `[1]+ Done ./zombie > /dev/null`. After `sleep(20)` ended, the parent executed `waitpid(pid, &status, 0);`, collecting the child's exit status. That call allowed the operating system to release the zombie entry. The program was not terminated manually for this observation.

## Part 3: Redirection of standard output

`redirect.c` reproduces `ls -l > out.txt` by closing descriptor `1` in the child and opening `out.txt` before executing `ls`.

```bash
gcc -Wall -Wextra -o redirect redirect.c
./redirect
head -3 out.txt
```

The recorded terminal messages were `open gave descriptor 1` and `done: the listing is in out.txt`. The displayed first lines of the saved listing began with `total 68`, followed by entries for `out.txt` and `redirect`.

### Observe

**1. Which number did `open()` give, and why exactly that one?**

`open()` returned **1** because `close(1)` had released the standard-output file descriptor. Linux assigns the lowest available file-descriptor number to a newly opened file, so `out.txt` took descriptor `1`. The result was confirmed by the program's diagnostic message.

**2. `ls` was not changed at all. Why does its listing not appear on the screen?**

The `ls` executable still writes to standard output, but standard output now refers to `out.txt`, not to the terminal. Consequently, its listing is written into the file and can be inspected with `head -3 out.txt`. The message `open gave descriptor 1` remains visible because it is printed to standard error (`stderr`, descriptor `2`), which was not redirected.

**3. To redirect the errors instead (like `2> err.txt`), which descriptor would you close before `open()`?**

Close descriptor **2**, which represents standard error. If the file is opened immediately afterward and no lower-numbered descriptor is free, `open()` takes descriptor `2`; programs writing to `stderr` then write to that file instead of the terminal.

## Part 4: Two-process pipe

`pipeline.c` creates a pipe connecting the standard output of `ls /etc` to the standard input of `wc -l`. The parent must close both pipe ends when it no longer uses them.

```bash
gcc -Wall -Wextra -o pipeline pipeline.c
./pipeline
ls /etc | wc -l
```

Both commands printed **236** in the recorded run.

For the deliberate error test, the parent's `close(p[1]);` was temporarily commented out, and `./pipeline` was run again. The saved screenshot shows the command without a returned shell prompt, consistent with the program being blocked. The source code explains which pipe descriptor causes the block. The line was restored afterward.

### Observe

**1. Did `./pipeline` and `ls /etc | wc -l` give the same number?**

Yes. Both printed **236**. This confirms that the C program passed the directory listing through its pipe to `wc -l` in the same way as the shell pipeline in this environment.

**2. After the change, which of the two children never ends, `ls` or `wc`?**

The `wc` child remains blocked. The screenshot captures `./pipeline` without a returning shell prompt; examining the C code identifies `wc` as the process unable to finish. `ls` can write its data and terminate, but the parent still holds the pipe's write descriptor open.

**3. Why, in one sentence: what is `wc` waiting for?**

`wc` is waiting for EOF on the pipe, which cannot occur while the parent keeps the pipe's writing end (`p[1]`) open.

The final submitted version closes **both** `p[0]` and `p[1]` in the parent.

## Optional exercise: Three-stage pipeline

`pipeline3.c` uses **two pipes and three children** to implement `ls /usr/bin | grep x | wc -l`. Every process closes unused pipe ends so the next stage can detect EOF.

```bash
gcc -Wall -Wextra -o pipeline3 pipeline3.c
./pipeline3
ls /usr/bin | grep x | wc -l
```

Both commands printed **224**, confirming that the additional pipeline stage operated correctly.

## Conclusion

The laboratory connected the visible behavior of Linux commands with the process and file-descriptor operations used to implement them. The experiments showed that `fork()` creates a child, `execlp()` replaces its program, and `waitpid()` collects its exit status. The failure test demonstrated why code following `execlp()` runs only after an error.

The zombie experiment illustrated how a terminated process can temporarily remain in the process table until its parent waits for it. The redirection exercise demonstrated that changing file descriptor `1` changes where an unchanged program sends its output. The pipe exercises extended this idea to communication between processes. In particular, the deliberately unclosed writing end explained why closing unused descriptors is necessary for EOF and successful termination. The optional three-stage pipeline confirmed the same principles with an additional process.
