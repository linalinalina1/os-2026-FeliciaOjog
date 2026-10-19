# Laboratory Work 6 — Inside the xv6 File System

**Course:** Operating Systems  
**Group:** FAF-243  
**Environment:** Ubuntu (VirtualBox), `~/xv6-riscv` and xv6 running in QEMU

## Preparation
```bash
cd ~/Downloads
bash os2026-setup.sh --check
cd ~/xv6-riscv
make qemu
```
The setup check verified the required toolchain and QEMU environment.

## Part 1 — Read What xv6 Shows

### xv6 shell commands
```text
ls
mkdir a
ls a
```

### Observe answers
1. `kernel/stat.h` defines type **1 (`T_DIR`)** as directory, **2 (`T_FILE`)** as regular file, and **3 (`T_DEVICE`)** as device.
2. The root directory has inode **1**. At the root, `.` and `..` both refer to that inode because root is its own parent.
3. An empty directory `a` has size **32 bytes** and contains `.` and `..`, so each `struct dirent` occupies **16 bytes**.

## Part 2 — Write `mystat`

### Ubuntu commands
```bash
cd ~/xv6-riscv
nano user/mystat.c
nano Makefile
make qemu
```
The new program was added to the `UPROGS` list as `$U/_mystat`.

### xv6 shell commands
```text
mystat README . console
ln README r2
mystat README r2
rm README
mystat r2
cat r2 | wc
```

### Source inspection (Ubuntu)
```bash
grep -n "nlink" kernel/sysfile.c
grep -A 15 "struct dinode" kernel/fs.h
```

### Observe answers
1. After `ln README r2`, both names referred to the same inode, with `nlink = 2`, and the contents and size remained unchanged. After `rm README`, `r2` remained readable with `nlink = 1`. The actual recorded output was:

```text
$ mystat README . console
README: file, inode 2, links 1, size 2441 bytes
.: directory, inode 1, links 1, size 1024 bytes
console: device, inode 26, links 1, size 0 bytes
$ ln README r2
$ mystat README r2
README: file, inode 2, links 2, size 2441 bytes
r2: file, inode 2, links 2, size 2441 bytes
$ rm README
$ mystat r2
r2: file, inode 2, links 1, size 2441 bytes
$ cat r2 | wc
48 336 2441
```
2. In `kernel/sysfile.c`, `sys_unlink` decreases `ip->nlink` after removing the directory entry. The inode's blocks are freed once its link count is zero and the last active reference has been released.
3. `struct dinode` in `kernel/fs.h` includes **type, major, minor, nlink, size, addrs** (block addresses).

## Part 3 — Measure the Limits

### Ubuntu commands
```bash
grep -n "define BSIZE\|define NDIRECT\|define NINDIRECT\|define DIRSIZ" kernel/fs.h
make clean
make fs.img 2>&1 | grep nmeta
make qemu
```

### xv6 shell commands
```text
mkdir averyveryverylongname
ls
```

### Observe answers
1. The long name appeared as **`averyveryveryl`** because `DIRSIZ = 14` permits fourteen name bytes in each directory entry.
2. In the single-indirect-block arrangement specified by the exercise, `BSIZE = 1024`, `NDIRECT = 12`, and `NINDIRECT = 1024 / 4 = 256`. Thus the maximum file size is **`(12 + 256) * 1024 = 274432 bytes` (268 KiB)**.
3. The image-builder output reported **2000 total blocks**, **47 metadata blocks** (31 log, 13 inode, 1 bitmap, 1 boot and 1 superblock) and **1953 data blocks**.

## Part 4 — Write `find`

### Ubuntu commands
```bash
nano user/find.c
nano Makefile
make clean
make qemu
```
The program was added to `UPROGS` as `$U/_find`.

### xv6 shell commands
```text
mkdir a
mkdir a/b
echo x > a/b/r2
echo y > a/r2
ln cat r2
find . r2
find a b
```

### Results
`find . r2` found all three paths (order may vary):
```text
./a/b/r2
./a/r2
./r2
```
`find a b` found:
```text
a/b
```
The implementation recursively reads directory entries, skips unused entries (`inum == 0`) and `.`/`..`, compares names with `strcmp`, and closes open directories.

## Optional Exercise — `find -n` Inode Numbers

### Ubuntu command
```bash
nano user/find.c
make qemu
```
The updated `find` accepts an optional `-n` flag, passes a `show_inode` argument through recursive calls, and uses `stat` to display inode numbers for matching paths.

### xv6 shell commands
```text
find . r2
find . r2 -n
mystat cat r2 a/r2 a/b/r2
find
find . r2 -x
```

### Results and bonus answer
The ordinary `find . r2` still found the same three paths. The recorded `-n` output and `mystat` comparison were:

```text
$ find . r2
./a/b/r2
./a/r2
./r2
$ find . r2 -n
./a/b/r2 inode 30
./a/r2 inode 31
./r2 inode 3
$ mystat cat r2 a/r2 a/b/r2
cat: file, inode 3, links 2, size 36728 bytes
r2: file, inode 3, links 2, size 36728 bytes
a/r2: file, inode 31, links 1, size 2 bytes
a/b/r2: file, inode 30, links 1, size 2 bytes
```

**Only the root-level `./r2` shared inode 3 with `cat`**, because it was created using `ln cat r2`. The other two paths had distinct inodes 31 and 30. The invalid-argument tests printed the expected usage message:
```text
usage: find <dir> <name> [-n]
```
The inode numbers above are the actual values in the submitted bonus screenshot; they can differ after rebuilding a fresh filesystem image.

## Conclusion
The lab connected xv6 shell output with internal filesystem data structures, system calls, and user-program implementation. `mystat` demonstrated inode identity and link-count changes; filesystem constants explained filename and file-size limits; and `find` showed how recursive directory traversal works. The optional inode-output flag verified that identical names do not imply identical files, while the top-level hard link to `cat` shared its inode. These experiments reinforced careful handling of directory entries, file descriptors, error conditions, and build configuration.

**Evidence:** The completed Lab 6 Word report contains supporting screenshots, figures, and detailed explanations of all required observations and the bonus.
