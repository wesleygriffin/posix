#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static void handler(int sig, siginfo_t* si, void* uc) {
	timer_t const* pid = (timer_t*)si->si_value.sival_ptr;
	int const or = timer_getoverrun(*pid);
	if (or == -1) err(EXIT_FAILURE, "timer_getoverrun");
	printf("%d signal received (SIGPROF = %d) with %d overruns\n", sig, SIGPROF, or);
}

int main() {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = handler;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGPROF, &sa, NULL) == -1) err(EXIT_FAILURE, "sigact");

	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGPROF);
	if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1) err(EXIT_FAILURE, "sigprocmask");

	timer_t id;
	struct sigevent se;
	se.sigev_notify = SIGEV_SIGNAL;
	se.sigev_signo = SIGPROF;
	se.sigev_value.sival_ptr = &id;
	if (timer_create(CLOCK_REALTIME, &se, &id) == -1) err(EXIT_FAILURE, "timer_create");

	unsigned long const fns = 100;
	struct itimerspec it;
	it.it_value.tv_sec = it.it_interval.tv_sec = fns / 1000000000;
	it.it_value.tv_nsec = it.it_interval.tv_nsec = fns % 1000000000;
	if (timer_settime(id, 0, &it, NULL) == -1) err(EXIT_FAILURE, "timer_settime");

	sleep(1);
	if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1) err(EXIT_FAILURE, "sigprocmask");
	exit(EXIT_SUCCESS);
}

