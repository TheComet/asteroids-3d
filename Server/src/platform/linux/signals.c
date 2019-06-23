#include "Server/signals.h"
#include <signal.h>
#include <stddef.h>

static volatile int g_exit_requested = 0;

// ----------------------------------------------------------------------------
static void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        g_exit_requested = 1;
    }
}

// ----------------------------------------------------------------------------
void signals_register(void)
{
    struct sigaction act;
    act.sa_handler = sig_handler;
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
}

// ----------------------------------------------------------------------------
int signals_exit_requested(void)
{
    return g_exit_requested;
}
