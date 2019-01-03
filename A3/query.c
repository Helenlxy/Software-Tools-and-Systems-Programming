#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

int main(int argc, char **argv) {
	char ch;
    char path[PATHLENGTH];
    char *startdir = ".";

    /* this models using getopt to process command-line flags and arguments */
    while ((ch = getopt(argc, argv, "d:")) != -1) {
        switch (ch) {
        case 'd':
            startdir = optarg;
            break;
        default:
            fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME]\n");
            exit(1);
        }
    }

    // Open the directory provided by the user (or current working directory)
    DIR *dirp;
    if ((dirp = opendir(startdir)) == NULL) {
        perror("opendir");
        exit(1);
    }

    /* For each entry in the directory, eliminate . and .., and check
     * to make sure that the entry is a directory, then call run_worker
     * to process the index file contained in the directory.
     * Note that this implementation of the query engine iterates
     * sequentially through the directories, and will expect to read
     * a word from standard input for each index it checks.
     */
    struct dirent *dp;
    int pipe_fds[MAXWORKERS][2][2];
    int pipe_index = 0;
    while ((dp = readdir(dirp)) != NULL) {
    	//Check if the number of subdirectories is too large.
    	if (pipe_index >= MAXWORKERS)
    	{
    		fprintf(stderr, "%s", "The number of subdirectories is larger than MAXWORKERS.\n");
    		exit(1);
    	}

        if (strcmp(dp->d_name, ".") == 0 ||
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0 ||
            strcmp(dp->d_name, ".git") == 0) {
                continue;
        }

        strncpy(path, startdir, PATHLENGTH);
        strncat(path, "/", PATHLENGTH - strlen(path));
        strncat(path, dp->d_name, PATHLENGTH - strlen(path));
        path[PATHLENGTH - 1] = '\0';

        struct stat sbuf;
        if (stat(path, &sbuf) == -1) {
            // This should only fail if we got the path wrong
            // or we don't have permissions on this entry.
            perror("stat");
            exit(1);
        }

        // Only call run_worker if it is a directory
        // Otherwise ignore it.
        if (S_ISDIR(sbuf.st_mode)) {
        	//Create two pipes.
        	if (pipe(pipe_fds[pipe_index][0]) == -1)
        	{
        		perror("pipe");
        		exit(1);
        	}
        	if (pipe(pipe_fds[pipe_index][1]) == -1)
        	{
        		perror("pipe");
        		exit(1);
        	}
        	
        	int result = fork();
        	if (result < 0)
        	{
        		perror("fork");
        		exit(1);
        	}else if(result == 0){
        		//Chile process
        		//Close unuseful ends.
        		if (close(pipe_fds[pipe_index][0][1]) == -1) {
        			perror("close writing end from inside child for the first pipe");
        			exit(1);
        		}
        		if (close(pipe_fds[pipe_index][1][0]) == -1) {
       				perror("close reading end from inside child for the second pipe");
       				exit(1);
       			}
                //Close previous unused pipes.
                for (int i = 0; i < pipe_index; ++i)
                {
                    if (close(pipe_fds[i][0][1]) == -1) {
                        perror("close writing end from inside child for the first pipe");
                        exit(1);
                    }
                    if (close(pipe_fds[i][1][0]) == -1) {
                        perror("close reading end from inside child for the second pipe");
                        exit(1);
                    }
                }
        		run_worker(path, pipe_fds[pipe_index][0][0], pipe_fds[pipe_index][1][1]);
        		//Close used pipes.
        		if (close(pipe_fds[pipe_index][0][0]) == -1) {
        			perror("close reading end from inside child for the first pipe");
        			exit(1);
        		}
        		if (close(pipe_fds[pipe_index][1][1]) == -1) {
        			perror("close wrintng end from inside child for the second pipe");
        			exit(1);
        		}
        		exit(0);
        	}else{
                //Close unuseful parent pipes.
        		if (close(pipe_fds[pipe_index][0][0]) == -1) {
        			perror("close reading end from parent for the first pipe");
        			exit(1);
        		}
        		if (close(pipe_fds[pipe_index][1][1]) == -1) {
        			perror("close wrintng end from parent for the second pipe");
        			exit(1);
        		}
        		pipe_index++;
        	}
        }
    }

    char word[MAXWORD];
    FreqRecord master[MAXRECORDS+1];
    
    //Read from stdin
    while(fgets(word, MAXWORD, stdin) != NULL){
    	master[0].freq = 0;
    	strcpy(master[0].filename, "");    	
   		FreqRecord temp;

   		//Write to each work processes.
    	for (int i = 0; i < pipe_index; i++)
    	{
    		if(write(pipe_fds[i][0][1],  word, MAXWORD) == -1){
    			perror("write to pipe");
        		exit(1);
        	};
    	}

    	//Read from each work processes.
    	for (int i = 0; i < pipe_index; i++)
    	{
    		while(read(pipe_fds[i][1][0], &temp, sizeof(FreqRecord)) > 0){
    			if (temp.freq > 0)
    			{
    				//Sort the master frequency list.
    				int master_index = 0;
    				while(master[master_index].freq > temp.freq){
    					master_index++;
    				}
    				for (int i = MAXRECORDS; i > master_index; i--)
    				{
    					master[i] = master[i-1];
    				}
    				master[master_index] = temp;
    				master[MAXRECORDS].freq = 0;
    				strcpy(master[MAXRECORDS].filename, "");
    			}else{
    				break;
    			}
    		}
    	}
    	print_freq_records(master);
    }


    if (closedir(dirp) < 0)
        perror("closedir");

    return 0;
}