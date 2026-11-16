# Operating Systems: Laboratory Work 12
## Producer, Consumer and the Other Tools

Execution environment: Ubuntu Linux, `~/os-lab12`; GCC with `-Wall -pthread`.
All results below are taken from the recorded Ubuntu execution session.

## Part 1: Producer and consumer

**1. The producer is fast, yet "in buffer" never goes above 4. Which call stops it?**

`sem_wait(&empty)` blocks the producer when the four available buffer slots have all been occupied. A consumed item causes `sem_post(&empty)`, permitting another insertion. The recorded normal run never exceeded an occupancy of four and ended at zero after the eight items were consumed.

**2. Who waits at the start: the consumer or the producer? Which call makes it wait?**

The consumer can be the first to wait because `full` starts at zero. Its `sem_wait(&full)` blocks until the producer creates an item and calls `sem_post(&full)`.

**3. What happens if `producer()` locks the mutex before `sem_wait(&empty)`? Why?**

In the modified run, four items were inserted, then the program stopped making progress; it was interrupted with Ctrl+C. With the buffer full, the producer blocks on `empty` while still holding the mutex. The consumer needs that same mutex to remove an item and release a free slot, creating deadlock.

## Part 2: Condition variable

**1. In `order.c` (Lab 11) the semaphore remembered that the message was ready. Here, which variable remembers it?**

The integer `ready` represents the application predicate; the condition variable `changed` only supports waiting and notification.

**2. After replacing `while (!ready)` with `if (!ready)`, does the example still work?**

Yes, in the recorded run both versions printed `B: not ready, I sleep`, followed by `A: message written` and `B: I read "hello"`. A successful run does not prove that `if` is safe for all schedules.

**3. Why use `while`? Give one reason a thread can wake but still find `ready == 0`.**

Condition waits may return spuriously without the predicate becoming true. In more complex programs another participant may change the predicate before the awakened thread reacquires the mutex. A `while` loop checks `ready` after each wakeup and waits again if necessary.

## Part 3: Readers and writers

**1. Which two lines of `reader()` allow three readers to be inside while a writer remains alone?**

`if (++readers == 1) sem_wait(&room);` and `if (--readers == 0) sem_post(&room);`. The first reader closes the room to writers, and the last reader releases it. In the recorded output, all three readers read value 10 with `readers inside: 3`.

**2. Why does `readers` need its own lock (`rlock`)? What would happen without it?**

The reader count and first/last-reader decisions are shared state. Without `rlock`, overlapping updates could lose changes or incorrectly acquire/release `room`, potentially allowing unsafe writer overlap or blocking progress.

**3. If readers keep arriving without a pause, what happens to writer 2?**

The room may never become free because the number of active readers may stay above zero. The waiting writer can experience **writer starvation**.

## Part 4: Build a deadlock, then remove it

**1. In the stuck run, which thread holds which mutex and waits for which other mutex? Draw the circle.**

Thread one holds A and requests B; thread two holds B and requests A. Circular wait: **thread one → B (held by thread two) → thread two → A (held by thread one) → thread one**. The recorded deadlock experiment ended with timeout exit code 124.

**2. Remove both `sleep(1)` calls from the original version and run it ten times. Does it always hang? Why is that worse?**

No. In the recorded ten-run test without sleeps, runs **1, 5, 6** timed out (code 124), while runs **2, 3, 4, 7, 8, 9, 10** completed (code 0). The failure is worse to diagnose because it is timing-dependent and can disappear during one test while remaining possible in another.

**3. Why does taking locks in the same order prevent this deadlock?**

When both threads acquire A before B, no thread can hold B while waiting for A, so the circular wait involving these two mutexes cannot form. The recorded corrected version finished with `done` and exit code 0.

## Recorded results summary

- Producer-consumer normal run: eight items produced and eight consumed; maximum buffer occupancy 4; final occupancy 0.
- Producer-consumer reversed lock order: blocked after `put 4`; stopped with Ctrl+C.
- Condition variable using `while` and `if`: both recorded runs printed `B: I read "hello"`.
- Readers-writers: writer 1 wrote 10; three readers observed 10; writer 2 wrote 20.
- Original deadlock: exit code 124; fixed lock order: exit code 0.
- No-sleep deadlock: seven successful completions and three timeouts across ten runs.

Source: user's supplied Ubuntu screenshots and recorded terminal transcripts; Operating Systems Lab 12 instructions (UTM, 2026–2027).
