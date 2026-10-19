# Laboratory Work 5 — The File System with Your Own Hands

**Course:** Operating Systems  
**Group:** FAF-243  
**Environment:** Ubuntu in Oracle VirtualBox, normal user `lina`  
**Working directory:** `/home/lina/os-lab5`

## Part 1 — The Tree and File Types

### Commands
```bash
whoami
mkdir -p ~/os-lab5 && cd ~/os-lab5
ls /
ls -ld /etc /etc/passwd /bin /dev/null /tmp
ls -l /dev | head
file /usr/bin/ls /etc/passwd /bin
du -sh /var/log 2>/dev/null
du -sh /usr 2>/dev/null
```

### Observe answers
1. The first character of `ls -l` indicates the file type: `/etc` and `/tmp` are directories (`d`); `/etc/passwd` is a regular file (`-`); `/bin` is a symbolic link (`l`); and `/dev/null` is a character device (`c`).
2. `/bin` is a symbolic link pointing to `usr/bin`, which resolves to `/usr/bin`.
3. `/dev/autofs` and `/dev/null` are character devices. `/dev/sda` is a block device, identified in the disk listing.
4. `du -sh` reported **296M** for `/var/log` and **5.3G** for `/usr`. Thus `/usr` used approximately **5.0 GB more** disk space (using rounded, human-readable figures).

## Part 2 — Inodes and Links

### Commands
```bash
echo hello > notes.txt
stat notes.txt
ls -li notes.txt
ln notes.txt hard.txt
ls -li notes.txt hard.txt
rm notes.txt
cat hard.txt
ls -li hard.txt
ln -s hard.txt soft.txt
ls -li hard.txt soft.txt
cat soft.txt
rm hard.txt
cat soft.txt
ls -l soft.txt
mkdir d
ls -ld d
mkdir d/x d/y
ls -ld d
echo x > one.txt
ls -l one.txt
du -h one.txt
truncate -s 1G big.img
ls -lh big.img
du -h big.img
rm big.img
```

### Observe answers
1. `stat` displays the inode number in the **Inode** field and the hard-link count in **Links**.
2. `hard.txt` remained readable after `notes.txt` was deleted because the two names referenced the same inode. `rm` removed the directory entry, not the data while another hard link existed.
3. `soft.txt` became a dangling symbolic link after its target `hard.txt` was removed. Its size was **8 bytes**, the length of the stored target name `hard.txt`.
4. Directory `d` initially had two links: one from its parent directory and its own `.` entry. Each child directory (`x` and `y`) added a `..` reference to `d`, increasing the count to four.
5. `one.txt` had two bytes of content (including a newline) but typically consumed one 4.0K disk block. `big.img` had a logical size of 1 GB but zero allocated data blocks because `truncate` created a sparse file.

## Part 3 — Permissions

### Commands
```bash
echo secret > s.txt
chmod 600 s.txt
ls -l s.txt
chmod 000 s.txt
cat s.txt
chmod u+r s.txt
cat s.txt
umask
touch new.txt
mkdir newdir
ls -ld new.txt newdir
mkdir box
echo hi > box/f
chmod u=r,go= box
ls box
cat box/f
chmod u=x,go= box
ls box
cat box/f
chmod 755 box
ls -l /usr/bin/passwd
ls -ld /tmp
cat /etc/shadow
ls -l /etc/shadow
```

### Observe answers
1. `600` = `rw-------` (private file); `644` = `rw-r--r--` (readable document); `755` = `rwxr-xr-x` (executable or shared directory); `700` = `rwx------` (private directory).
2. Owning a file does not bypass its permissions. `chmod 000` removed ordinary access bits, including those of the owner.
3. The observed umask was **0002**. `new.txt` received mode **664** (`-rw-rw-r--`) and `newdir` mode **775** (`drwxrwxr-x`).
4. Directory `r` allows listing the entry names. Directory `x` allows searching/traversing a known path. This explains why a known `box/f` file could be read with execute-only directory access even when `ls box` failed.
5. The `s` in `/usr/bin/passwd` indicates **setuid**, allowing its privileged password-update operation. The `t` in `/tmp` indicates the **sticky bit**, restricting who can remove entries from the shared directory.

## Part 4 — Disks and the Kernel as Files

### Commands
```bash
lsblk
df -h
mount | grep " / "
grep -c processor /proc/cpuinfo
head -3 /proc/meminfo
head -5 /proc/self/status
ls -l /proc/self/fd
```

### Observe answers
1. The VirtualBox disk was **`/dev/sda` (25G)**, with **`/dev/sda1` (1M)** and **`/dev/sda2` (25G)**. Partition `/dev/sda2` was mounted at `/` using **ext4**. `lsblk` also listed `/dev/sr0` (50.7M, mounted at `/run/media/lina/VBox_GAs_7.2.20`) and various loop devices for Snap packages.
2. `df -h` reported `/dev/sda2` mounted at `/`: **25G total, 8.3G used, 15G available, 36% used**.
3. `/proc/cpuinfo` reported **2 logical processors**. `/proc/meminfo` displayed **MemTotal: 3480328 kB** (about 3.32 GiB), **MemFree: 171616 kB**, and **MemAvailable: 682868 kB** at the time of measurement. These values describe the virtual machine, not necessarily the physical host.
4. `/proc/self/status` showed **head** as the process name because `/proc/self` refers to the process reading it.

## Optional Exercises — Bonuses

### Bonus A: Magic Numbers
```bash
cd ~/os-lab5
gzip -c /etc/passwd > notes.txt
file notes.txt
xxd notes.txt | head -1
```
The `file` utility identified `notes.txt` as gzip-compressed data despite its `.txt` extension. The first bytes, **`1f 8b 08`**, contain the gzip magic number (`1f 8b`) and compression-method byte (`08`).

### Bonus B: Find Setuid Programs
```bash
find / -xdev -perm -4000 -type f 2>/dev/null
ls -l /usr/bin/passwd
```
The search listed the following **16 setuid files** on the scanned root filesystem (as shown in the screenshot):

```text
/usr/bin/chfn
/usr/bin/su
/usr/bin/mount
/usr/bin/chsh
/usr/bin/gpasswd
/usr/bin/fusermount3
/usr/bin/passwd
/usr/bin/sudo.ws
/usr/bin/pkexec
/usr/bin/ntfs-3g
/usr/bin/umount
/usr/lib/dbus-1.0/dbus-daemon-launch-helper
/usr/lib/cargo/bin/su
/usr/lib/cargo/bin/sudo
/usr/lib/openssh/ssh-keysign
/usr/sbin/pppd
```

The screenshot shows these **16** paths (the result is not a complete inventory beyond this captured command). The permissions of `/usr/bin/passwd` included `s` in the owner's execute position. A precise comparison with the lecturer's slide requires that slide's list.

## Conclusion
The laboratory connected Linux filesystem theory with observed file types, inode identity, links, permissions, disk allocation, and virtual kernel interfaces. Hard links demonstrated that file names are separate from inodes; symbolic links could become dangling; and sparse files illustrated the difference between logical size and allocated blocks. Permission experiments using a normal user, including the observed umask `0002`, confirmed how ownership, access bits, and special permissions affect operations. Disk and `/proc` inspection connected the filesystem hierarchy to the virtual machine's storage and runtime state. The two optional experiments further showed how file signatures identify data independently of extensions and how setuid is used for privileged executables.

**Evidence:** The completed Lab 5 Word report contains supporting screenshots, captions, and detailed explanations of these observations and bonuses.
