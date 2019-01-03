#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "freq_list.h"
#include "worker.h"


int main(int argc, char **argv) {
    Node *head = NULL;
    char **filenames = init_filenames();
    char *listfile = ".";
    char *namefile = ".";
    char *word = "";

    if (argc != 4)
    {
    	fprintf(stderr, "Usage: task1 INDEX_FILE_PATH FILENAMES_FILE_PATH WORD\n");
        exit(1);
    }else{
    	listfile = argv[1];
    	namefile = argv[2];
    	word = argv[3];
    }

    read_list(listfile, namefile, &head, filenames);
    //display_list(head, filenames);
    FreqRecord *fr = get_word(word, head, filenames);
	print_freq_records(fr);
    return 0;
}
