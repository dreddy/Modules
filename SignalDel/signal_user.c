#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdint.h>

#define SIG_TEST 44 /* we define our own signal, hard coded since SIGRTMIN is different in user and in kernel space */ 

uint64_t counter=0xFFFFFFFFFFFF;
volatile uint64_t iter=0;
volatile uint64_t timestamps[32];

static uint64_t rdtsc(void)
{
        uint32_t lo, hi;
        /* We cannot use "=A", since this would use %rax on x86_64 */
        __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
        return (uint64_t)hi << 32 | lo;
}

void receiveData(int n, siginfo_t *info, void *unused) {
	timestamps[iter++] = rdtsc();

	write(stdout, "receiveData\n", 12);
	if(iter > 4)
		counter=0;	
}

int main ( int argc, char **argv )
{
	int configfd;
	char buf[10];
	int i;
	/* setup the signal handler for SIG_TEST 
 	 * SA_SIGINFO -> we want the signal handler function with 3 arguments
 	 */
	struct sigaction sig;
	sig.sa_sigaction = receiveData;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_TEST, &sig, NULL);

#if 0
	/* kernel needs to know our pid to be able to send us a signal ->
 	 * we use debugfs for this -> do not forget to mount the debugfs!
 	 */
	configfd = open("/sys/kernel/debug/signalconfpid", O_WRONLY);
	if(configfd < 0) {
		perror("open");
		return -1;
	}
	sprintf(buf, "%i", getpid());
	if (write(configfd, buf, strlen(buf) + 1) < 0) {
		perror("fwrite"); 
		return -1;
	}
#endif
	
	while(counter)
		;
	for(i=0; i<32; i++)
		fprintf(stdout, "%d %llu\n", i, timestamps[i]);

	return 0;
}



