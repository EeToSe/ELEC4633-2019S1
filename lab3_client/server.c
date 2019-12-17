#include <stdio.h>
#include <rtai.h>
#include <rtai_shm.h>
#include <rtai_nam2num.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int *mem;

/* Struct for sending message to display */
struct mymsgbuf {
  long mtype;
  int data[10];
};
struct mymsgbuf message;

/* Struct for receive the setpoint */
struct mymsgbuf_receive {
  long mtype;
  int data[1];
};
struct mymsgbuf_receive message_receive;

int main(void)
{ 
  int status, status_receive, i;
  int queue, queue_receive;
  key_t msgkey, msgkey_receive;

  // generate a key to obtain a queue id
  msgkey = ftok(".",'n'); 			// this queue is for sending data to be displayed
  msgkey_receive = ftok(".",'p');	// this queue is for receiving the setpoint

  // obtain a queue identifier
  queue = msgget(msgkey, IPC_CREAT | 0660);
  queue_receive = msgget(msgkey_receive, IPC_CREAT | 0660);

  // set up message
  // set the message type to 1 - can be any positive number
  message.mtype = 1;

  /* Define two variables for retrieving data */
  int data[10], prev = 0;
  int fifo, fifo_send;
  
  /* Open the file for reading only */
  fifo=open("/dev/rtf0", O_RDONLY);
  fifo_send=open("/dev/rtf1", O_WRONLY);

  /* Establish TCP to client (THIS IS THE SERVER) */
  	int serverfd = 0, clientfd = 0;
    struct sockaddr_in serv_addr;

    char sendBuff[1025];
	char recvBuff[1025];
    time_t ticks;

    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
	memset(recvBuff, '0', sizeof(recvBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);
	
    bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	listen(serverfd, 1);
	clientfd = accept(serverfd, (struct sockaddr*)NULL, NULL);

	/* create shared memory */
	mem = rtai_malloc(nam2num("shmem"), sizeof(int));
	
  /* Display to the window */
  while (1)
  {
	/* TALKING TO the CLIENT */
	if (read(clientfd , recvBuff, sizeof(recvBuff)-1) <= 0) {
		printf("Client lost\n");
		rtai_free(nam2num("shmem"), *mem);
  		return 0;
	}
	printf("message: %s\n", recvBuff);
	if (strlen(recvBuff) <= 3) {
		// Transfrom the string to integer
		prev = atoi(recvBuff);
	} else {
		printf("read: %d\n",*mem);
		snprintf(sendBuff, sizeof(sendBuff), "%d\n%c", *mem, '\0');
    	write(clientfd, sendBuff, strlen(sendBuff)+1);
	}

	/* SEND SETPOINT TO TEMPLATE.KO */
	write(fifo_send, &prev, sizeof(int));
  }

  rtai_free(nam2num("shmem"), *mem);
  close(clientfd);
  return 0;
}