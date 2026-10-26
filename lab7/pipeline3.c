#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* does the same as: ls /usr/bin | grep x | wc -l */
int main(void)
{
    int p1[2], p2[2];

    if (pipe(p1) < 0) {
        perror("pipe");
        return 1;
    }

    if (pipe(p2) < 0) {
        perror("pipe");
        close(p1[0]);
        close(p1[1]);
        return 1;
    }

    if (fork() == 0) {                          /* first child: ls */
        close(1);
        dup(p1[1]);                             /* stdout becomes p1[1] */

        close(p1[0]);
        close(p1[1]);
        close(p2[0]);
        close(p2[1]);

        execlp("ls", "ls", "/usr/bin", (char *)NULL);
        perror("exec ls");
        exit(127);
    }

    if (fork() == 0) {                          /* second child: grep */
        close(0);
        dup(p1[0]);                             /* stdin becomes p1[0] */

        close(1);
        dup(p2[1]);                             /* stdout becomes p2[1] */

        close(p1[0]);
        close(p1[1]);
        close(p2[0]);
        close(p2[1]);

        execlp("grep", "grep", "x", (char *)NULL);
        perror("exec grep");
        exit(127);
    }

    if (fork() == 0) {                          /* third child: wc */
        close(0);
        dup(p2[0]);                             /* stdin becomes p2[0] */

        close(p1[0]);
        close(p1[1]);
        close(p2[0]);
        close(p2[1]);

        execlp("wc", "wc", "-l", (char *)NULL);
        perror("exec wc");
        exit(127);
    }

    close(p1[0]);
    close(p1[1]);
    close(p2[0]);
    close(p2[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
