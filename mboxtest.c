#include "mailbox.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    const char *name = "mboxtest";
    // Define array of messages
    const char *messages[] = {"Message 1", "Message 2", "Message 3", "Message 4", "Message 5", "Message 6", "Message 7", "Message 8", "Message 8"};

    int shm_fd;
    mailbox_t *mbox;
	// void *ptr;
    int size = sizeof(mailbox_t);

    /* create the shared memory segment */
	shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

	/* configure the size of the shared memory segment */
	ftruncate(shm_fd, size);

    mbox = mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); // Maps shared memory in address space with read and write access
	// Error handling
    if (mbox == MAP_FAILED){ 
		printf("Map failed.\n");
		return -1;
	}
    // Call mailbox_init function on mbox
    mailbox_init(mbox);
    // Create fork
    pid_t pid = fork();
    // Error handling
    if(pid < 0){
        perror("Pipe Error.");
        exit(1);
    }
    // Child
    if(pid == 0){
        char buf[MAILBOX_MAX_MSG]; // Declare array of MAILBOX_MAX_MSG number elements
        size_t outlen; // Declare variable for message length

        for(int i = 0; i < MAILBOX_CAPACITY; i++){ // Loop MAILBOX_CAPACITY times
            int result = receive_mailbox(mbox, buf, sizeof(buf), &outlen); // Call receive_mailbox() on mbox, buffer, buffer size, and message length, define returned value as result
            if(result == MBOX_SUCCESS){ // If successfully received message
                printf("[Receiver] Message %d: %s\n", (i + 1), buf); // Output received message
            } else if(result == MBOX_ERR_EMPTY){ // Or else there were no messages to receive
                printf("[Receiver] Mailbox empty.\n"); // Output error statement statement
            }
        }
    // Parent
    }else{
        for(int i = 0; i < MAILBOX_CAPACITY; i++){ // Loop MAILBOX_CAPACITY times
            int result = send_mailbox(mbox, messages[i], strlen(messages[i])); // Call send_mailbox() on mbox, message at index i, buffer size, and message a index i's length, define returned value as result
            if(result == MBOX_SUCCESS){ // If successfully sent message
                printf("[Sender] Message %d: %s\n", (i + 1), messages[i]); // Output sent message
            } else if(result == MBOX_ERR_FULL){ // Or else if mailbox was full and sender was unable to send message
                printf("[Sender] Mailbox full. Unable to send %s.\n", messages[i]); // Output error statement
            }
        }
        waitpid(pid, NULL, 0); // Wait for child
        /* remove the shared memory segment */
        if (shm_unlink(name) == -1) {
            printf("Error removing %s\n.", name);
            exit(-1);
        }
    }
    return 0;
}
