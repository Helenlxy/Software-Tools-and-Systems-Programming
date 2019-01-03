#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv) {

	int i;
	int n;
	int iterations;

	if(argc != 2) {
		fprintf(stderr, "Usage: forkloop <iterations>\n");
		exit(1);
	}

	iterations = atoi(argv[1]);

	for(i = 0; i < iterations; i++) {
		n = fork();
		if(n < 0) {
			perror("fork");
			exit(1);
		}
		//printf("pid = %d, i = %d\n", getpid(), i);
		printf("ppid = %d, pid = %d, i = %d\n", getppid(), getpid(), i);
	}

	return 0;
}
