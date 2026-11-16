# Lab 11 — The Race and the Semaphore (Linux)

## Part 1 — See the race

1. **Three operations behind `counter++`:** read the current counter, add one, then write the new value back.
2. **How one increment is lost:** two threads can read the same old value, each add one, and both store the same updated value. Two attempts then produce only one increase.
3. **Why outcomes vary:** different thread interleavings can overwrite different numbers of updates. Actual untimed runs: **1,312,396; 2,000,000; 2,000,000**. The timed race run printed **1,000,000**.

## Part 2 — Mutex and semaphore

1. **Were the mutex and binary-semaphore results always 2,000,000?** Yes, all three observed executions of `mutex` and all three of `sem` initialized to one returned **2,000,000**.
2. **What happens when `sem_init(&s, 0, 2)`?** Two permits allow both threads to enter, so the shared counter can race. Actual outputs: **1,956,869; 1,920,314; 1,961,886**.
3. **Why is locking slower?** It adds lock/permit synchronization and waiting overhead. Actual real times: unprotected race **0.034 s** (incorrect result), mutex **0.140 s** (correct result), semaphore **0.243 s** (correct result). These are individual-run measurements, not universal benchmarks.

## Part 3 — Semaphore as a signal

1. **Start at one:** in the observed run B read an empty string before A wrote `hello`, since it did not need to wait for a post.
2. **Start at zero:** no initial permit exists; B waits until A posts after writing the message. In the observed run B then read `hello`.

## Part 4 — Parking lot

1. **Largest `inside` value and guarantee:** the corrected program now prints the mutex-protected `inside` value. The maximum observed value was **3**, and the final value was **0**. `sem_init(&places, 0, PLACES)` with `PLACES = 3` and `sem_wait(&places)` before entry limit the number of cars; `pthread_mutex_lock(&lock)` protects changes to `inside`.
2. **Changing places to one:** the semaphore becomes a single-permit (binary) admission semaphore. The observed `inside` values alternated between **1** on entry and **0** on departure, and all six cars completed.
