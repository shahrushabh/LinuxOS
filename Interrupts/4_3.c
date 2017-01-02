#include <signal.h>

void my_routine();

int main() {

	// Adding the signal statement for catching the floating point error.
	signal(SIGFPE,my_routine);
	int a = 4;
	a = a / 0;
	exit(0);
}

void my_routine() {
	printf("Caught a SIGFPE.\n");
}
