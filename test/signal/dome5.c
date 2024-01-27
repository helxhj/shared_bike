#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void myhandle(int sig) 
{
		printf("Catch a signal : %d\n", sig);
		printf("Catch end.%d\n", sig);
}

int main(void) 
{
		struct sigaction act, act2;

		act.sa_handler = myhandle;
		sigemptyset(&act.sa_mask);
		act.sa_flags = 0;
		sigaction(SIGINT, &act, 0);

		sigset_t proc_sig_msk, old_mask;
		sigemptyset(&proc_sig_msk);
		sigaddset(&proc_sig_msk, SIGINT);

		sigprocmask(SIG_BLOCK, &proc_sig_msk, &old_mask);
		sleep(5);
		printf("had delete SIGINT from process sig mask\n");
		sigprocmask(SIG_UNBLOCK, &proc_sig_msk, &old_mask);

		while (1) {

		}

		return 0;
}

