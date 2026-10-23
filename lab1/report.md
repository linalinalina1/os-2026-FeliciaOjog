# Operating Systems: Laboratory Work 1
## The OS as a Resource Manager

## Initial System Information

Working directory:

```text
/home/lina/os-lab1
```

Current user:

```text
lina
```

The laboratory was performed in Ubuntu Linux running in Oracle VirtualBox.

## Part 1: Files and Directories

Commands used included:

```bash
pwd
ls -la /
ls -la /home/lina
cd /etc
ls | head
cd /home/lina/os-lab1
mkdir demo
cd demo
echo "hello operating systems" > note.txt
cp note.txt copy.txt
mv copy.txt renamed.txt
ls -l
rm renamed.txt
ls -l note.txt
chmod 600 note.txt
ls -l note.txt
chmod 644 note.txt
```

The created files were owned by user `lina` and group `lina`.

After running:

```bash
chmod 600 note.txt
```

the permission string became:

```text
-rw-------
```

Meaning:

- `-`: regular file;
- `rw-`: the owner can read and write;
- `---`: the group has no permissions;
- `---`: other users have no permissions.

Two directories directly under `/` are:

- `/etc`: contains system-wide configuration files;
- `/home`: contains user home directories.

Interesting command and output:

```bash
ls -l note.txt
```

The important observed fields were:

```text
-rw-------    lina    lina    note.txt
```

This showed the file type, permissions, owner, and group after `chmod 600`.

## Part 2: Processes

Commands used included:

```bash
ps aux | head
ps aux | wc -l
top
sleep 300 &
jobs
ps -ef | grep sleep
kill 9039
sleep 300 &
PID=$!
ls /proc/$PID/
cat /proc/$PID/status | head -20
kill $PID
```

Process ID 1 was:

```text
systemd
```

`systemd` is the init and system manager process of Ubuntu.

The idle virtual machine had approximately:

```text
203 processes
```

The first test `sleep` process received PID `9039`.

A second `sleep` process was created for `/proc` inspection and had PID `9072`.

Interesting command and output:

```bash
cat /proc/$PID/status | head -20
```

The process state was:

```text
State: S (sleeping)
```

This means that the process was waiting and was not actively using the CPU.

## Part 3: Memory

Commands used included:

```bash
free -h
cat /proc/meminfo | head -6
sleep 300 &
PID=$!
grep VmRSS /proc/$PID/status
kill $PID
```

The `free -h` command showed approximately:

- Total RAM: `3.3 GiB`
- Used RAM: `1.5 GiB`
- Free RAM: `190 MiB`
- Available RAM: `1.8 GiB`
- Swap: `0 B`

Swap is disk space that can be used as additional virtual memory when needed. No swap space was configured on this virtual machine.

Interesting command and output:

```bash
grep VmRSS /proc/$PID/status
```

Output:

```text
VmRSS: 7700 kB
```

This value was not surprising because `sleep` performs almost no active computation and uses only a small amount of physical memory.

## Part 4: Devices and Storage

Commands used included:

```bash
df -h
lsblk
du -sh /home/lina/os-lab1
ls -l /dev | head
mount | head
```

The root filesystem `/` was mounted on:

```text
/dev/sda2
```

The filesystem type was `ext4`.

The virtual disk appeared as `sda`, with the root filesystem located on partition `sda2`.

The size of the Lab 1 working directory was approximately:

```text
12K
```

One example from `/dev` was:

```text
/dev/cdrom -> sr0
```

This represents the virtual CD/DVD device.

The expression **"everything is a file"** means that Linux represents many hardware devices and system resources through file-like interfaces, so they can be accessed using common operations.

Interesting command and output:

```bash
mount | head
```

The relevant observed output was:

```text
/dev/sda2 on / type ext4
```

This confirmed that `/dev/sda2` was mounted as the root filesystem using `ext4`.

## Closing Synthesis

The four main operating-system resources investigated were files, processes, memory, and devices/storage. Files were observed with `ls` and `chmod`, processes with `ps`, `top`, and `/proc`, memory with `free` and `/proc/meminfo`, and devices/storage with `lsblk`, `df`, `/dev`, and `mount`. These commands demonstrated how Linux exposes and manages its main system resources.
