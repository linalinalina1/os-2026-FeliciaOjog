# Laboratory Work 4
## The xv6 Shell from the Outside

## Part 1: Fix sleep for the Current xv6

The current xv6 tree declares:

```c
int pause(int);
```

The existing `user/sleep.c` already used:

```c
pause(atoi(argv[1]));
```

so no code change was required. The Makefile already contained the `_sleep` entry in `UPROGS`.

Tests in xv6:

```text
$ sleep 20
$ sleep
usage: sleep <ticks>
```

The valid command paused and returned to the prompt. Running `sleep` without an argument printed the expected usage message.

## Part 2: bash versus the xv6 Shell

| Command | Tests | bash result | xv6 result | Explanation |
|---|---|---|---|---|
| `echo hello > f ; cat < f` | redirection | `hello` | `hello` | Both shells support input and output redirection. |
| `ls \| grep READ` | pipe | `README` | README line from xv6 `ls` output | Both shells support pipes. |
| `(echo a; echo b) \| wc` | grouping, list | `2 2 4` | `2 2 4` | Both support the grouping and list syntax used here and piping into `wc`. |
| `echo one ; echo two` | sequence | `one`, `two` | `one`, `two` | Both support commands separated by `;`. |
| `echo bg &` | background | background job starts | `bg` is printed and the prompt returns | Both support background execution with `&`. |
| `ls *` | globbing | pattern is expanded before `ls` runs | `ls: cannot open *` | bash expands `*`; xv6 passes it literally. |
| `echo $HOME` | variables | `/home/lina` | `$HOME` | bash expands variables; xv6 does not. |
| `echo "a b"` | quotes | `a b` | `"a b"` | Bash preserves the space inside the quoted argument and removes the quote characters; xv6 prints the quotes literally. |
| `mkdir d ; cd d ; ls ..` | PATH | `README`, `d`, `f` | parent directory listing | Both can change directory and access `..`, but xv6 does not search a `PATH` for executables. |
| arrow up, Tab, Ctrl+R | history and completion | supported | not supported | These interactive features are provided by bash. |

### Observe answers

1. Both shells support basic program execution, redirection, pipes, command sequences with `;`, background execution, and `cd`. bash also provides pathname globbing, variable expansion, quote processing, command history, Tab completion, and Ctrl+R reverse search.
2. In xv6, `ls` stops working after `cd d` because xv6 does not search a `PATH` for executables. The user programs are stored in the parent/root directory. From inside `d`, `../ls` explicitly names the program and works.
3. The missing feature that would be most useful in daily work is command history and interactive completion because they reduce repeated typing. A small command-history mechanism would likely be one of the easier features to add because it could be implemented inside the shell without changing the kernel.

## Part 3: Who Expands *? A Program that Prints argv

The same argument-printing idea was implemented for xv6 and Linux.

Commands tested in both shells:

```text
args one two three
args *.c "x y"
args $HOME
args 'single quoted'
```

### Observe answers

1. For:

```text
args *.c "x y"
```

xv6 reported `argc = 4` and passed:

```text
argv[0] = [args]
argv[1] = [*.c]
argv[2] = ["x]
argv[3] = [y"]
```

bash reported:

```text
argc = 5
argv[0] = [./args]
argv[1] = [args_linux.c]
argv[2] = [one.c]
argv[3] = [two.c]
argv[4] = [x y]
```

This matches the screenshot: `*.c` expanded to three matching source files, while `"x y"` became one argument without the quote characters.

2. The shell performs wildcard expansion and quote processing. The program and the kernel do not turn `*.c` into filenames. bash performs these transformations before starting the program.

3. Extra spaces between unquoted words act only as separators. They are not preserved as part of the arguments passed in `argv`.

The same experiment also showed that bash expands `$HOME` to `/home/lina`, while xv6 passes `$HOME` literally.

## Part 4: Read the Shell

The source file `user/sh.c` was inspected with:

```bash
wc -l user/sh.c
grep -n "#define" user/sh.c
grep -n "case " user/sh.c
grep -n "cd" user/sh.c | head
```

### Observe answers

1. The xv6 shell knows five command kinds:

| Kind | Meaning | Character or syntax |
|---|---|---|
| `EXEC` | ordinary program execution | normal command |
| `REDIR` | input/output redirection | `<` or `>` |
| `PIPE` | pipe | `\|` |
| `LIST` | sequential command list | `;` |
| `BACK` | background execution | `&` |

2. `cd` is handled directly by the shell, around line 170 in the inspected `user/sh.c`. It cannot be run only as a separate child program because the current working directory is process state. If only the child changed directory, the parent shell would remain in the previous directory.

3. Searching `user/sh.c` found no pathname-globbing implementation for `*` and no variable-expansion implementation for `$`. The `$` occurrence relevant to the shell is the code that prints the prompt. This explains why xv6 passes patterns such as `*.c` and values such as `$HOME` literally instead of expanding them.
