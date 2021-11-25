#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

int process;

void sigusr1_handler(int sig)
{
    printf("I'am process with id %d. I have signal %d from process %d\n", getpid(), sig, process);
    sleep(1);
}

int main()
{
    int child = 0;

    child = fork();

    struct sigaction sa;

	sa.sa_handler = sigusr1_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);

    if ( child > 0)
    {
        if (sigaction(SIGUSR2, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);

        }
        process = child;
        printf("I'am parent. My id is %d\n", getpid());
        while(1) {
            sleep(5);
            kill(process, SIGUSR1);
        }
    }
    else
    {
        if (sigaction(SIGUSR1, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }
        process = getppid();
        printf("I'am child. My id is %d. My parent id is %d\n", getpid(), getppid());
        while(1) {
            sleep(5);
            kill(process, SIGUSR2);
        }
    }
    return 0;
}
