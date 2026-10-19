#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "user/user.h"

void
find(char *path, char *target, int show_inode)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if (stat(path, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        return;
    }

    if (st.type != T_DIR) {
        char *name = path;

        for (char *s = path; *s; s++) {
            if (*s == '/')
                name = s + 1;
        }

        if (strcmp(name, target) == 0) {
            if (show_inode)
                printf("%s inode %d\n", path, st.ino);
            else
                printf("%s\n", path);
        }

        return;
    }

    if ((fd = open(path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf)) {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';

    while (read(fd, &de, sizeof(de)) == sizeof(de)) {
        if (de.inum == 0)
            continue;

        char name[DIRSIZ + 1];
        memmove(name, de.name, DIRSIZ);
        name[DIRSIZ] = 0;

        if (strcmp(name, ".") == 0 ||
            strcmp(name, "..") == 0)
            continue;

        strcpy(p, name);

        if (stat(buf, &st) < 0) {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }

        if (strcmp(name, target) == 0) {
            if (show_inode)
                printf("%s inode %d\n", buf, st.ino);
            else
                printf("%s\n", buf);
        }

        if (st.type == T_DIR)
            find(buf, target, show_inode);
    }

    close(fd);
}

int
main(int argc, char *argv[])
{
    if (argc != 3 &&
        !(argc == 4 && strcmp(argv[3], "-n") == 0)) {
        fprintf(2, "usage: find <dir> <name> [-n]\n");
        exit(1);
    }

    int show_inode = (argc == 4);

    find(argv[1], argv[2], show_inode);

    exit(0);
}
