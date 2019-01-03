#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>

#include "freq_list.h"
#include "worker.h"

/* 
* Return an array of FreqRecord elements for the word which records its frequency in each file whenever the frequency is non-zero.
*/
FreqRecord *get_word(char *word, Node *head, char **file_names) {
	FreqRecord *fr = malloc(sizeof(FreqRecord)*(MAXFILES+1));
	Node *curr = head;
	int isFound = 0;
	int index = 0;
	while (curr != NULL){
		if (strcmp(curr->word, word) == 0)
		{
			for (int i = 0; i < MAXFILES; i++)
			{
				if (curr->freq[i] != 0)
				{
					fr[index].freq = curr->freq[i];
					strcpy(fr[index].filename, file_names[i]);
					index++;
				}
				
			}
			fr[index].freq = 0;
			strcpy(fr[index].filename, "");
			isFound = 1;
			break;
		}
		curr = curr->next;
	}
	
	if (isFound == 0)
	{
		fr[0].freq = 0;
		strcpy(fr[0].filename, "");
	}
    return fr;

}

/* Print to standard output the frequency records for a word.
* Use this for your own testing and also for query.c
*/
void print_freq_records(FreqRecord *frp) {
    int i = 0;

    while (frp != NULL && frp[i].freq != 0) {
        printf("%d    %s\n", frp[i].freq, frp[i].filename);
        i++;
    }
}

/*Load the index into a data structure. 
Read one word at a time from the file descriptor in until the file descriptor is closed. 
Write to the file descriptor out one FreqRecord for each file in which the word has a non-zero frequency.
*/
void run_worker(char *dirname, int in, int out) {
	Node *head = NULL;
    char **filenames = init_filenames();
    char listfile[PATHLENGTH];
    strncpy(listfile, dirname, PATHLENGTH);
    strcat(listfile, "/index");
    char namefile[PATHLENGTH];
    strncpy(namefile, dirname, PATHLENGTH);
    strcat(namefile, "/filenames");

    read_list(listfile, namefile, &head, filenames);

    char word[MAXWORD];

    while(read(in, word, MAXWORD) > 0){
    	for (int i = 0; i < strlen(word); ++i)
    	{
    		if (word[i] == '\n')
    		{
    			word[i] = '\0';
    			break;
    		}
    	}

    	FreqRecord *frp = get_word(word, head, filenames);

    	int i = 0;

    	while (frp != NULL && frp[i].freq != 0) {
        	if(write(out, &frp[i], sizeof(FreqRecord)) == -1){
        		perror("write to pipe");
        		exit(1);
        	}
        	i++;
    	}
    	FreqRecord final;
    	final.freq = 0;
		strcpy(final.filename, "");
		if(write(out, &final, sizeof(FreqRecord)) == -1){
        	perror("write to pipe");
        	exit(1);
        }
        free(frp);
    }
    for (int i = 0; i < MAXFILES; i++)
    {
        free(filenames[i]);
    }
    free(filenames);
    Node *curr = head;
    while(curr != NULL){
        free(curr);
        curr = curr->next;
    }
    
}
