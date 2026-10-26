# Laboratory Work 8: The Same Calls, Inside Your Own OS (xv6)

## Purpose

To repeat process creation, program replacement, and waiting inside xv6, then use pipes for communication between processes. The optional task constructs a chain of processes that finds prime numbers.

## Part 1: `run` inside xv6

The program was added to the xv6 build and tested by running `make qemu` in Ubuntu followed by `run` at the xv6 shell prompt. The original program uses `fork()`, `exec("echo", args)`, and `wait(&status)`.

Recorded output with `echo`:

```text
$ run
parent: my pid is 3
child: my pid is 4
hello from the child
parent: child 4 ended with status 0
$
```

The `ls` variant printed the xv6 directory contents and returned status **0**. The nonexistent-program variant produced:

```text
$ run
parent: my pid is 3
child: my pid is 4
exec failed
parent: child 4 ended with status 1
$
```

After these tests, `run.c` was restored to the original `echo` variant.

### Observe

**1. Write the three calls that `run.c` uses in xv6 and the three calls `run.c` used on Linux in Lab 7. What is the same?**

In xv6, the three calls are **`fork()`**, **`exec()`**, and **`wait()`**. In Linux Lab 7, they are **`fork()`**, **`execlp()`**, and **`waitpid()`**. The overall sequence is the same: the parent creates a child with `fork()`, the child replaces its program with another executable, and the parent waits to collect the child's termination result. The interfaces differ because the Linux example uses `execlp()` to search for an executable and `waitpid()` to wait for a specific PID, while the xv6 example uses its smaller `exec()` and `wait()` interfaces.

**2. Who prints `hello from the child`: the code of `run`, or `echo`?**

The **`echo` program** prints the message. `run` creates the child and builds the argument array, then `exec("echo", args)` replaces the child's current code with `echo`. Because successful `exec()` does not return to `run`, the printed words come from the new program rather than from another `printf()` in `run.c`.

**3. Why is the status `1` here and `127` on Linux? (Look at the exit line after `exec` in both programs.)**

The different status values come from the **explicit exit statements in the two examples**. When xv6 cannot execute `nosuchprog`, `exec()` returns, the child prints `exec failed`, and then calls **`exit(1)`**. In Lab 7, a failed Linux `execlp()` is followed by `perror("exec")` and **`exit(127)`**. The waiting parents report the codes selected by their respective programs; this is not a fundamental difference in what `fork()` does.

## Part 2: `pingpong`

`pingpong.c` creates two pipes: `p2c` for parent-to-child communication and `c2p` for child-to-parent communication. Each side closes the ends it does not need. The program passes one byte from parent to child and returns it to the parent.

Recorded result:

```text
$ pingpong
4: received ping
3: received pong
$
```

### Observe

**1. Why do you need TWO pipes, and not one for both directions?**

A normal pipe carries a byte stream in one direction, from its writing end to its reading end. The experiment requires two directions of communication. The **`p2c`** pipe carries the initial byte from the parent to the child, and the **`c2p`** pipe carries the reply from the child back to the parent. Keeping them separate ensures that each process reads the intended message and avoids accidentally reading its own data.

**2. Nothing in your code says "wait until the child has replied". Which call makes the parent wait?**

The parent's **`read(c2p[0], &b, 1)`** blocks until one byte becomes available in the child-to-parent pipe. The child writes that byte only after receiving and printing the ping, so the parent's `received pong` line appears afterward. The later **`wait(0)`** has a different role: it waits for the child process to terminate and collects its status, rather than waiting for the reply byte itself.

## Optional exercise: `primes`

`primes.c` implements a process-based sieve for numbers from 2 through 35. Each stage reads the first remaining number, reports it as prime, and forwards numbers not divisible by that prime to the next stage using a new pipe. Unused descriptors are closed to avoid blocking and descriptor exhaustion.

The first recursive implementation was rejected by the compiler because warnings were treated as errors (`-Werror=infinite-recursion`). It was replaced by an iterative process chain. The final program compiled and ran in xv6:

```text
$ primes
prime 2
prime 3
prime 5
prime 7
prime 11
prime 13
prime 17
prime 19
prime 23
prime 29
prime 31
$
```

The output lists all **11** prime numbers in the specified range and returns to the shell prompt.

## Conclusion

The laboratory demonstrated that the core mechanisms studied on Linux can also be used directly in xv6. The `run` experiments showed the relationship between `fork()`, `exec()`, and `wait()`, including how explicit failure exit codes are propagated to the parent. Testing `echo`, `ls`, and a nonexistent program made the success and error paths distinguishable.

The `pingpong` exercise demonstrated controlled bidirectional communication and showed that a blocking `read()` can synchronize the exchange of messages without an explicit delay. The optional sieve expanded the same pipe-based approach into a chain of processes. Successful termination depended on closing unnecessary file descriptors and waiting for child processes. The final files retain the working `echo` variant of `run.c` and the successfully tested `pingpong` and `primes` implementations.
