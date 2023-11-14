#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>
#include<sys/shm.h>

#define MAX_GRAPH_SIZE 256

// Define the message format
typedef struct Request {
    long sequence_number;
    int operation_number;
    char graph_file_name[100];
}Request;
typedef struct LBRequest_t {
    long mtype;
    long sequence_number;
    int operation_number;
    char graph_file_name[100];
}LBRequest;
// Message Queue key and ID
#define MSG_KEY 1234
int msgid;

#define SHM_KEY 1234
int shmid;

int main() {
    Request request;
    LBRequest lbRequest;
    int num_nodes;
	key_t keyq;
    //msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    //if (msgid == -1) {
      //  fprintf(stderr,"msgget");
        //exit(1);
    //}
    //Creating message queue
    if ((keyq = ftok("LoadBalancer.c", 1000)) == -1)
    {
        perror("ftok");
        exit(1);
    }

    // Create or get the message queue
    msgid = msgget(keyq, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    //shmid = shmget(SHM_KEY, MAX_GRAPH_SIZE * MAX_GRAPH_SIZE * sizeof(int), 0666);
    //if (shmid == -1) {
     //   perror("shmget");
       // exit(1);
    //}

    // Attach shared memory to the process
    //int *shared_memory = shmat(shmid, NULL, 0);
    //if (shared_memory == (int *)-1) {
      //  perror("shmat");
        //exit(1);
    //}

    while (1) {
    	printf("Load Balancer is Listening: ");
    	fflush(stdout);
        if (msgrcv(msgid, &request, sizeof(request),-101, 0) == -1) {
            fprintf(stderr,"msgrcv");
            exit(1);
        }
        
	lbRequest.sequence_number = request.sequence_number;
	lbRequest.operation_number = request.operation_number;
	strcpy(lbRequest.graph_file_name,request.graph_file_name);
	
        if(request.sequence_number == 101){
        	printf("terminating\n");
        	lbRequest.mtype = 201;
        	if(msgsnd(msgid,&lbRequest,sizeof(lbRequest),0)==-1){
                        fprintf(stderr,"\nError in msgsnd");
                        exit(1);
            	}
        	break;
        }
        
        if(request.operation_number == 1 || request.operation_number ==2){
            printf("\n%ld",request.sequence_number);
            printf("\n%s",request.graph_file_name);

            

            fflush(stdout);
		
		
            lbRequest.mtype = 201;
           

            // Send the message to the message queue.
            if(msgsnd(msgid,&lbRequest,sizeof(lbRequest),0)==-1){
                        fprintf(stderr,"\nError in msgsnd");
                        exit(1);
            }
        }
    }
    
    if (msgctl(msgid, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
        exit(1);
    }
    
    return 0;
}
