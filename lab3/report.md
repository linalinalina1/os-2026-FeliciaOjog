# Laboratory Work 3
## The Command Line: Shell, Pipes and Text Filters

## Part 1: Move Fast, Find Help

### Results

Tab completion expanded an unambiguous prefix automatically and displayed alternatives when several paths matched. `Ctrl+R` searched command history.

```bash
type cd ; type ls ; type echo ; which ls
echo $PATH
echo $PATH | tr ':' '\n'
nosuchprog ; echo "exit code: $?"
```

Observed results:

- `cd` and `echo` are shell built-ins.
- `ls` was configured as an alias, while `which ls` located `/usr/bin/ls`.
- The observed `PATH` contained 10 directory entries.
- A command that was not found returned exit code `127`.
- `ls -t` sorts entries by modification time.

### Observe answers

1. `cd` and `echo` are built-ins. `cd` must be a built-in because it has to change the working directory of the current shell process. If it ran only as a child process, the shell directory would not change.
2. The observed `PATH` contained 10 directories. `ls` is located in `/usr/bin`.
3. A command that is not found returns exit code `127`.
4. The `ls` option that sorts by modification time is `-t`.

## Part 2: What the Shell Does to Your Line

Test files:

```bash
mkdir -p ~/os-lab3/play && cd ~/os-lab3/play
touch a.c b.c file1.txt file2.txt notes.txt "alfa beta.txt"
```

### Predictions and results

| Command | Prediction | Real output | Explanation |
|---|---|---|---|
| `echo *.c` | `a.c b.c` | `a.c b.c` | Bash expanded `*.c` before running `echo`. |
| `echo file?.txt` | `file1.txt file2.txt` | `file1.txt file2.txt` | `?` matched one character. |
| `echo [a-b]*` | `a.c alfa beta.txt b.c` | `a.c alfa beta.txt b.c` | The character class matched names starting with `a` or `b`. |
| `echo *.xyz` | No matching files | `*.xyz` | With no match, Bash kept the pattern unchanged. |
| `ls alfa beta.txt` | The spaced filename | Errors for `alfa` and `beta.txt` | Without quotes, the shell passed two arguments. |
| `ls "alfa beta.txt"` | `alfa beta.txt` | `alfa beta.txt` | Quotes preserved the space inside one argument. |
| `echo '$HOME' "$HOME" \$HOME` | `$HOME /home/lina $HOME` | `$HOME /home/lina $HOME` | Single quotes and backslash suppress expansion, while double quotes allow it. |
| `echo {1..5}; echo $((7 * 6))` | `1 2 3 4 5` and `42` | `1 2 3 4 5` and `42` | Brace and arithmetic expansion worked as expected. |
| `echo "today is $(date +%A), I am $(whoami)"` | Current day and user | `today is Monday, I am lina` | Command substitution inserted command output. |
| `touch log{1..3}.txt && ls log*` | `log1.txt log2.txt log3.txt` | `log1.txt log2.txt log3.txt` | Brace expansion created the files and globbing listed them. |

### Observe answers

1. The predictions and real outputs are shown in the table above.
2. The shell produced the filenames for `echo *.c`. `echo` itself does not search the filesystem.
3. `ls alfa beta.txt` failed because the shell split the name into two arguments. Two fixes are:

```bash
ls "alfa beta.txt"
ls alfa\ beta.txt
```

4. Single quotes preserve text literally and suppress expansion. Double quotes preserve spaces but still allow variable, command, and arithmetic expansion.

## Part 3: Redirection, Chaining and Jobs

Commands used:

```bash
cd ~/os-lab3
ls /etc /nope > out.txt 2> err.txt
wc -l out.txt ; cat err.txt
ls /etc /nope > all.txt 2>&1 ; tail -2 all.txt
ls /etc /nope 2>&1 > all.txt
echo first > notes.md ; echo second >> notes.md ; cat notes.md
wc -l < access.log
ls /nope 2> /dev/null || echo "failed with code $?"
mkdir -p tmp && cd tmp && echo "I am in $(pwd)" ; cd ~/os-lab3
sleep 100 &
jobs ; kill %1 ; jobs
```

### Observe answers

1. The normal `/etc` listing was written to `out.txt`. The error for `/nope` was written to `err.txt`.
2. In `> all.txt 2>&1`, stdout is redirected first and stderr is then made a copy of the redirected stdout, so both go to the file. In `2>&1 > all.txt`, stderr is first connected to the terminal and only stdout is redirected afterward, so the error remains visible in the terminal.
3. `wc -l < access.log` reads from standard input and receives no filename argument, so it prints only the number. `wc -l access.log` receives the filename as an argument and prints it too.
4. `;` runs the next command regardless of success. `&&` runs the next command only if the previous command succeeds. `||` runs the next command only if the previous command fails.

## Part 4: Answer Questions with One-Liners

### Q1. How many requests failed?

```bash
grep FAIL access.log | wc -l
```

Result: `100`

### Q2. How many different pages were requested, and which?

```bash
cut -d' ' -f5 access.log | sort -u ; echo "count: $(cut -d' ' -f5 access.log | sort -u | wc -l)"
```

Result: `/page0`, `/page1`, `/page2`, `/page3`, count `4`.

### Q3. How many requests did each page get?

```bash
cut -d' ' -f5 access.log | sort | uniq -c
```

Result: `125` requests for each page.

### Q4. Which user or users have the most failed requests, and how many?

```bash
grep FAIL access.log | cut -d' ' -f3 | sort | uniq -c | sort -rn
```

Result: `user3` and `user5`, with `15` failed requests each.

### Q5. How many requests did user3 make, and how many failed?

```bash
echo "total: $(grep ' user3 ' access.log | wc -l), failed: $(grep ' user3 ' access.log | grep -c FAIL)"
```

Result: `72` total requests and `15` failed requests.

### Q6. Print the last 3 failed requests, showing only time and user.

```bash
grep FAIL access.log | tail -3 | cut -d' ' -f2,3
```

Result:

```text
10:10 user0
10:15 user5
10:20 user3
```

### Q7. Which login shells appear in /etc/passwd, and how many accounts use each?

```bash
cut -d':' -f7 /etc/passwd | sort | uniq -c | sort -rn
```

Result:

```text
40 /usr/sbin/nologin
5 /bin/false
2 /bin/bash
1 /bin/sync
```

### Q8. Why do the two line counts differ by exactly one?

```bash
ls /etc | wc -l ; ls -l /etc | wc -l
```

Observed result: `236` and `237`.

The long listing from `ls -l` adds a leading `total` line, so its output contains one extra line.

## Part 5: Your First Script

The script was made executable and tested with:

```bash
chmod +x report.sh
./report.sh access.log
./report.sh nope.log ; echo "exit code: $?"
```

Observed result for `access.log`:

```text
500 requests, 100 failed
user0: 14 failures
user1: 14 failures
user2: 14 failures
user3: 15 failures
user4: 14 failures
user5: 15 failures
user6: 14 failures
```

### Observe answers

1. `#!/bin/bash` tells the operating system to use `/bin/bash` as the interpreter. Before `chmod +x`, direct execution with `./report.sh` fails because the file is not executable, although `bash report.sh` can still run it.
2. `./report.sh` is needed because the current directory is normally not part of `PATH`. The `./` explicitly gives the path to the script.
3. `>&2` sends the error message to standard error. `exit 1` indicates that the script finished with an error.

## Stretch Tasks

### grep -E

```bash
grep -E '10:[0-5]0 .* FAIL ' access.log
grep -E '10:[0-5]0 .* FAIL ' access.log | wc -l
```

Result: `50` matching failed requests.

### cron

The script was scheduled to run every minute with:

```text
* * * * * /home/lina/os-lab3/report.sh /home/lina/os-lab3/access.log >> /home/lina/os-lab3/report.log 2>&1
```

The repeated output appeared in `report.log`, confirming that cron ran the script automatically. The temporary cron entry was removed after the test.
