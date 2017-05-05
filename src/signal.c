#include "sfish.h"

extern int sec;

void sig_hder(int signal_num){

	if(signal_num == SIGALRM){

		fprintf(stdout, "\nYour %d second timer has finished.\n", sec);
		char* title = prmpt_name();
		fprintf(stdout, "%s", title);
		fflush(stdout);
	}

	else if(signal_num == SIGCHLD){

		fprintf(stdout, "check");
		fflush(stdout);
	}
}

void alarm_handler(){

	got_interrupt = 1;

	fprintf(stdout, "\nYour %d second timer has finished.\n", sec);
	char* title = prmpt_name();
	fprintf(stdout, "%s", title);
	fflush(stdout);
}

void kill_handler(int sig, siginfo_t *siginfo, void *context){

	//fprintf(stdout, "Child with PID %d has died. It spent %f milliseconds utilizing the CPU.\n",
	//siginfo->si_pid, (siginfo->si_utime + siginfo->si_stime)/(sysconf(_SC_CLK_TCK)/1000.0));
	fflush(stdout);
}

void siguros_handler(){

	fprintf(stdout, "\nWell that was easy...\n");
	fflush(stdout);
	kill(getpid(), SIGKILL);
}
