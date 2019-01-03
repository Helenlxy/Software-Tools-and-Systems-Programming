#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void write_to_parent(int fd);
void read_from_child(int fd); 

int main() {
	int ret;
    int fd_A[2];

    if (pipe(fd_A) == -1) {
        perror("pipe");
        exit(1);
    }

	ret = fork();
	if (ret < 0) {
		perror("fork");
		exit(1);

	} else if (ret == 0) {
		/* I am the child and I will write only, so close reading end of pipe */
        // STEP 1: Add the close call below 
        if XXX  {
            perror("close");
            exit(1);
        }
        write_to_parent(fd_A[1]);
        exit(0);

	} 
    /* I am the parent only so close writing end of pipe to child A */
    // STEP 1: Add the close call below 



	read_from_child(fd_A[0]);
	return 0;
}

void write_to_parent(int fd) {
    char s[14] = "hello Toronto";

    // STEP 2: Write the string "hello" to the pipe following the instructions:
    // First write the first 5 characters of s.


    // Then explicitly write a null terminator.


/*
    // STEP 4: Change the write statements to write all of s including
    // the original null terminator in a single statement.
 
*/
    // Close the writing end of the pipe.
    if (close(fd) != 0) {
        perror("close");
        exit(1);
    }
}

void read_from_child(int fd) {
    char buf[6];


    // STEP 3:  Read 6 chars from pipe. This will include the null terminator.


    // STEP 3: Print out what was read. Use printf since it is a string.


/*
    // STEP 5: Change the read so that it keeps reading up to 6 characters
    // at a time until it finds that the pipe is closed and there is no more
    // to read. 

    // Read up to 6 characters in a loop. Keep going until read returns 0
    int count;

    while XXX STEP 5 XXX  {
         // write to stdout, the chars read this iteration 
        

         // to make it easlier to debug print a  "|\n" between writes
         printf("|\n");
    }

*/

    /* Close the reading end of the pipe because we are done  */
    if (close(fd) != 0) {
        perror("close");
        exit(1);
    }
}
