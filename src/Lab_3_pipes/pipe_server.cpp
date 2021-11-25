/* The server program  pipe_server.cpp */
#include"pipe_local.h"
#include <sys/wait.h>

int publicfifo;

void sigint_handler(int sig) {
    if (sig == SIGINT) {
        close(publicfifo);
        remove(PUBLIC);
        exit(0);
    }
}
int main(void)
{
    const int max_forks = 10;
int	n, done, dummyfifo, privatefifo;
static char buffer[PIPE_BUF];
FILE *fin;
struct message msg;
        /* Generate the public FIFO */
mknod(PUBLIC, S_IFIFO | 0666, 0);
    struct sigaction sa;

	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("sigaction");
            exit(1);
        }
	/* OPEN the public FIFO for reading and writing */
if ((publicfifo=open(PUBLIC, O_RDONLY))==-1 ||
     (dummyfifo=open(PUBLIC, O_WRONLY | O_NDELAY))==-1){
    perror(PUBLIC);
    exit(1);
    }
    /* Message can be read from the PUBLIC pipe */
    int fork_count = 0;
    int pid = 0;
while(read(publicfifo, (char *) &msg, sizeof(msg))>0){
    if (fork_count < max_forks) {
        pid = fork();
        if (pid == 0) {
            fork_count++;

            n = done = 0; /* Clear counters | flags */
            printf("Received client request: fifo_name = %s, cmd_line = %s", msg.fifo_name, msg.cmd_line);
            do{		/* Try OPEN of private FIFO */
            if ((privatefifo=open(msg.fifo_name, O_WRONLY | O_NDELAY))==-1)
            sleep(3);	/* Sleep a while  */
            else{	/* OPEN succesful */
                fin = popen(msg.cmd_line, "r");	/* Execute the cmd */
                write(privatefifo, "\n", 1);	/* Keep output pretty */
                while((n=read(fileno(fin), buffer, PIPE_BUF))>0){
                write(privatefifo, buffer, n);	/*to private FIFO */
                memset(buffer, 0x0, PIPE_BUF);	/* Clear it out */
                }
                pclose(fin);
                close(privatefifo);
                done = 1;	/* Record succes */
                }
            }while(++n<5 && !done);

            if(!done)		/* Indicate failure */
                write(fileno(stderr), "\nNOTE: SERVER ** NEVER ** accessed private FIFO\n", 48);
            } else {
                wait(0);
                fork_count--;
            }
    }
}
    return 0;
}
