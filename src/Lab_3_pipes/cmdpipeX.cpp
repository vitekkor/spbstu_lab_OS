/*
** pipex.c - multipipes support
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

/*
 * loop over commands by sharing
 * pipes.
 */
static void
pipeline(char ***cmd)
{
    int fd[2];
    pid_t pid;
    int fdd = 0;                /* Backup */

    while (*cmd != NULL) {
        pipe(fd);
        if ((pid = fork()) == -1) {
            perror("fork");
            exit(1);
        }
        else if (pid == 0) {
            dup2(fdd, 0);
            if (*(cmd + 1) != NULL) {
                dup2(fd[1], 1);
            }
            close(fd[0]);
            execvp((*cmd)[0], *cmd);
            exit(1);
        }
        else {
            wait(NULL);         /* Collect childs */
            close(fd[1]);
            fdd = fd[0];
            cmd++;
        }
    }
}

/*
 * Compute multi-pipeline based
 * on a command list.
 */
int
main(int argc, char *argv[])
{
    char *temp[argc][2];
    for (int i = 0; i < argc; i++)
    {
        temp[i][0] = argv[i];
        temp[i][1] = NULL;
    }

    char *ls[] = {"ls", NULL};
    char *rev[] = {"rev", NULL};
    char *nl[] = {"nl", NULL};
    char *cat[] = {"cat", "-e", NULL};
    char **cmd[argc + 1];

    for (int i = 0; i < argc; i++)
    {
        cmd[i] = temp[i];
    }
    cmd[argc] = NULL;


    pipeline(cmd);
    return (0);
}

