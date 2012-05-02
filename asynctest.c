#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>

int gotdata=0;

void sighandler(int signo)
{
    if (signo==SIGIO)
        gotdata++;
    return;
}
 
char buffer[4096];
 
int main(int argc, char **argv)
{
    int count;
    struct sigaction action;
 
    memset(&action, 0, sizeof(action));
    action.sa_handler = sighandler;
    action.sa_flags = 0;

    //signal -> process
    sigaction(SIGIO, &action, NULL);    //SIGIO -> sigaction

    fcntl(STDIN_FILENO, F_SETOWN, getpid());    //process -> file
    //fasync is here, afterwards read() below will be blocked; 
    //if O_NONBLOCK is set, read() below is non-blocked
    fcntl(STDIN_FILENO, F_SETFL, fcntl(STDIN_FILENO, F_GETFL) | FASYNC);    

    while(1) {
        /*  Description: sleep() makes the calling thread sleep until seconds seconds 
         *  have elapsed or a signal arrives which is not ignored.  
         *  After sighandler is executed, gotdata != 0 and it goes to read() below */
        sleep(86400); /* one day */
        if (!gotdata)
            continue;
        count=read(0, buffer, 4096);
        printf("read OK!\n");
        /* buggy: if avail data is more than 4kbytes... */
        write(1,buffer,count);  //1: standard output
        printf("write OK!\n");
        gotdata=0;
    }
}
