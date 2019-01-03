#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hcq.h"


#ifndef PORT
  #define PORT 52773
#endif
#define MAX_BACKLOG 5
#define BUF_SIZE 60
#define MAX_COMMAND 31
#define OUT_BUF_SIZE 1024


// Use global variables so we can have exactly one TA list and one student list and one connection list.
Ta *ta_list = NULL;
Student *stu_list = NULL;
Sockname *soc_list = NULL;

Course *courses;  
int num_courses = 3;


/*
 * Search the first n characters of buf for a network newline (\r\n).
 * Return one plus the index of the '\n' of the first network newline,
 * or -1 if no network newline is found.
 * Definitely do not use strchr or other string functions to search here. (Why not?)
 */
int find_network_newline(const char *buf, int n) {

    for(int i = 0; i < n - 1; i++){
	if(buf[i] == '\r' && buf[i+1] == '\n'){
	    return i+2;
	}
    }
    return -1;
}

/*
 * Return a pointer to the struct sockname with name soc_name who is a student
 * or NULL if no student with this name exists in the soc_list
 */
Sockname *find_socket_as_stu(Sockname *soc_list, char *soc_name) {
    while (soc_list != NULL) {
        if ((strcmp(soc_list->username, soc_name) == 0) && (soc_list->ista == 0)) {
            return soc_list;
        }
        soc_list = soc_list->next;
    }
    return NULL;
}

/* 
 * Read and process commands
 * Return a Sockname if it should be closed, otherwise return NULL
 *
 */
Sockname *process_cmds(Sockname *socket, char *cmd) {

	if(socket->ista == 0){
		if(strcmp(cmd, "stats") == 0){
			char *buf = print_currently_serving(ta_list);
			write(socket->sock_fd, buf, OUT_BUF_SIZE);
			free(buf);
		}else{
			write(socket->sock_fd, "Incorrect syntax.\r\n", sizeof("Incorrect syntax.\r\n"));
		}
	}else{
		if(strcmp(cmd, "stats") == 0){
			char *buf = print_full_queue(stu_list);
			write(socket->sock_fd, buf, OUT_BUF_SIZE);
			free(buf);
		}else if(strcmp(cmd, "next") == 0){
			Student *to_be_released = next_overall(socket->username, &ta_list, &stu_list);
			if(to_be_released != NULL){
				Sockname *soc = find_socket_as_stu(soc_list, to_be_released->name);
				write(soc->sock_fd, "Your turn to see the TA.\r\nWe are disconnecting you from the server now. Press Ctrl-C to close nc\r\n", sizeof("Your turn to see the TA.\r\nWe are disconnecting you from the server now. Press Ctrl-C to close nc\r\n"));
				close(soc->sock_fd);
				soc->state = -1;
				return soc;
			}
				
		}else{
			write(socket->sock_fd, "Incorrect syntax.\r\n", sizeof("Incorrect syntax.\r\n"));

		}
	}
	
    return NULL;
}

/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
Sockname *accept_connection(int fd, Sockname **soc_list_ptr) {
    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    struct sockname *new = malloc(sizeof(struct sockname));
	
    new->sock_fd = client_fd;
    new->username = malloc(MAX_COMMAND);
    new->ista = 2;
    new->next = NULL;
	memset(new->buf, '\0', BUF_SIZE);
    new->inbuf = 0;           // How many bytes currently in buffer?
    new->room = sizeof(new->buf);  // How many bytes remaining in buffer?
    new->after = new->buf;       // Pointer to position after the data in buf
	new->state = 0;


    if (*soc_list_ptr == NULL) {
        // there is currently no socket in the list at all, so special case
        *soc_list_ptr = new;
    } else {
        // we have at least one socket in the list so find last socket
        Sockname *last = *soc_list_ptr;
        while (last->next != NULL) {
            last = last->next;
        }
        // at this point last is the last real socket in the list
        last->next = new;
    }
	write(client_fd, "Welcome to the Help Centre, what is your name? \r\n", sizeof("Welcome to the Help Centre, what is your name? \r\n"));

    return new;
}

Sockname *read_from(Sockname *soc, Sockname **soc_list_ptr) {
	int fd = soc->sock_fd;
	Sockname *to_be_released = NULL;
	
	// Receive messages
    
    int nbytes;
		

    if((nbytes = read(fd, soc->after, soc->room)) > 0) {
        //update inbuf
	    soc->inbuf += nbytes;
        int where = 0;

        // the loop condition below calls find_network_newline
        // to determine if a full line has been read from the client.
        //
        // Note: we use a loop here because a single read might result in
        // more than one full line.
        while ((where = find_network_newline(soc->buf, soc->inbuf)) > 0) {
            // Output the full line, not including the "\r\n",
		    soc->buf[where-2] = '\0';
			if(where-1 > MAX_COMMAND){
				close(soc->sock_fd);
				soc->state = -1;
				printf("??");
				return soc;
			}
            printf("Next message: %s\n", soc->buf);
			if(soc->state == 0){
				strcpy(soc->username, soc->buf);
				soc->state = 1;
				write(fd, "Are you a TA or a Student (enter T or S)?\r\n", sizeof("Are you a TA or a Student (enter T or S)?\r\n"));
			}else if(soc->state == 1){
				if(strcmp(soc->buf, "S") && strcmp(soc->buf, "T")){
					write(fd, "Invalid role (enter T or S)\r\n", sizeof("Invalid role (enter T or S)\r\n"));
				}else{
					if(!strcmp(soc->buf, "S")){
						soc->ista = 0;
						soc->state = 2;
						write(fd, "Valid courses: CSC108, CSC148, CSC209\r\nWhich course are you asking about?\r\n", sizeof("Valid courses: CSC108, CSC148, CSC209\r\nWhich course are you asking about?\r\n"));
					}else{
						soc->ista = 1;
						soc->state = 3;
						add_ta(&ta_list, soc->username);
						write(fd, "Valid commands for TA:\r\n	stats\r\n	next\r\n	(or use Ctrl-C to leave)\r\n", sizeof("Valid commands for TA:\r\n	stats\r\n	next\r\n	(or use Ctrl-C to leave)\r\n"));
					}
				}
			}else if(soc->state == 2){
				if(strcmp(soc->buf, "CSC108") && strcmp(soc->buf, "CSC148") && strcmp(soc->buf, "CSC209")){
					write(fd, "This is not a valid course. Good-bye.\r\n", sizeof("This is not a valid course. Good-bye.\r\n"));
					close(soc->sock_fd);
					soc->state = -1;
					return soc;
				}else{
					//strcpy(soc->course, soc->buf);
					soc->state = 3;
					if(add_student(&stu_list, soc->username, soc->buf, courses, num_courses) == 1){
						write(fd, "You are already in the queue and cannot be added again for any course. Good-bye.\r\n", sizeof("You are already in the queue and cannot be added again for any course. Good-bye.\r\n"));
						close(soc->sock_fd);
						soc->state = -1;
						return soc;
					}else{
						write(fd, "You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n", sizeof("You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n"));
					}
				}
			}else if(soc->state == 3){
				to_be_released = process_cmds(soc, soc->buf);
			}
            // update inbuf and remove the full line from the buffer

            // move the stuff after the full line to the beginning
            // of the buffer.  
		    soc->inbuf -= where;
		    for(int i = 0; i < soc->inbuf; i++){
		        soc->buf[0 + i] = soc->buf[where + i];
		    }

        }
        //update after and room, in preparation for the next read.
	    soc->after = &(soc->buf)[soc->inbuf];
	    soc->room = sizeof(soc->buf) -  soc->inbuf;

    }else{
		close(soc->sock_fd);
		soc->state = -1;
		return soc;
	}
	if(to_be_released != NULL){
		return to_be_released;
	}else{
		return soc;
	}
}

/*
 * Helper method 
 */
void route_around(Sockname **soc_list_ptr, Sockname *thissocket) {
    // find the previous socket overall -- route around this removing one
    Sockname *current = *soc_list_ptr;
    if (current == thissocket) {
        // this socket is first in list
        *soc_list_ptr = thissocket->next;
    } else {
        while (current->next != thissocket) {
            current = current->next;
        }
        // now current points to previous socket 
        current->next = thissocket->next;
   }
}



int main(void) {

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    //The port will be released as soon as the server process terminates.
    int on = 1;
    int status = setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &on, sizeof(on));
    if(status == -1) {
        perror("setsockopt -- REUSEADDR");
    }

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);


	//Inicialize the course list.
	if ((courses = malloc(sizeof(Course) * 3)) == NULL) {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");

    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            Sockname *soc = accept_connection(sock_fd, &soc_list);
			int client_fd = soc->sock_fd;
            if (client_fd > max_fd) {
               max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            printf("Accepted connection\n");
        }


		Sockname *curr = soc_list;
		while(curr != NULL){
			Sockname *client_closed = NULL;
			if (FD_ISSET(curr->sock_fd, &listen_fds)) {
				client_closed = read_from(curr, &soc_list);
            	if (client_closed != NULL && client_closed->state < 0) {
                	FD_CLR(client_closed->sock_fd, &all_fds);
					route_around(&soc_list, client_closed);
					if(client_closed->ista == 0){
						give_up_waiting(&stu_list, client_closed->username);
					}else if(client_closed->ista == 1){
						remove_ta(&ta_list, client_closed->username);
					}
                	printf("Client %d disconnected\n", client_closed->sock_fd);
					break;
            	}
			}
			curr = curr->next;
			if(client_closed != NULL && client_closed->state < 0){
				free(client_closed->username);
				free(client_closed);
			}
		}

    }

    // Should never get here.
    return 1;
}

