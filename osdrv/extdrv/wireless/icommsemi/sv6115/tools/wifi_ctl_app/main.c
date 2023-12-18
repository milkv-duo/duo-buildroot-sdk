#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/signal.h>

#include "app_ctl.h"


static void sig_handler(int signum)
{
    app_ctl_stop(signum);
}

int main(void)
{
    signal(SIGHUP, sig_handler);
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    if(-1 == app_ctl_start())
    {
        return -1;
    }
    printf("\n");

    return 0;
}
