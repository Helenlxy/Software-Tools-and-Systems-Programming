#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc == 1)
	{
		char line[1024];
		char uid[31], prev_uid[31], max_uid[31];
		int max = 0;
		int curr = 0;
		sscanf(" ", "%s", prev_uid);
		while(fgets(line, 1024, stdin)){
			sscanf(line, "%s", uid);
			if (strcmp(uid, "") != 0)
			{
				if (strcmp(uid, prev_uid) == 0)
				{
					curr++;
					
				}else{
					
					curr = 1;
					sscanf(uid, "%s", prev_uid);
				}
				if (curr > max)
				{
					max = curr;
					sscanf(prev_uid, "%s", max_uid);
				}
			}
		}
		if (max != 0)
		{
			printf("%s %d\n", max_uid, max);
		}
		
		return 0;
	}else if(argc == 2){
		
		char line[1024];
		char uid[31], prev_uid[31], max_uid[31];
		int pid, ppid;
		int max = 0;
		int curr = 0;
		int command;
		char *temp;
		command = strtol(argv[1], &temp, 10);
		sscanf(" ", "%s", prev_uid);
		while(fgets(line, 1024, stdin)){
			sscanf(line, "%s %d %d", uid, &pid, &ppid);
			if (ppid == command)
			{
				if (strcmp(uid, prev_uid) == 0)
				{
					curr++;
					
				}else{
					curr = 1;
					sscanf(uid, "%s", prev_uid);
				}
				if (curr > max)
				{
					max = curr;
					sscanf(prev_uid, "%s", max_uid);
				}
			}	
		}
		if (max != 0)
		{
			printf("%s %d\n", max_uid, max);
		}
		return 0;
	}else{
		printf("USAGE: most_processes [ppid]\n");
		return 1;
	}
	//printf("%d\n", argc);
	//printf("%s\n", argv[1]);
	//return 0;
}