#include <stdio.h>

#include <rtai.h>
#include <rtai_shm.h>
#include <sys/msg.h>

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
  
  /* Display to the window */
  while (1)
  {
	printf("-------CHUNK OF DATA-------\n"); //for our own benefit
	/* Read values from FIFO and print them out*/
	for (i=0; i < 10; i++) {
  		read(fifo, &data[i], sizeof(int));
		printf("%d\n", data[i]);
		message.data[i] = data[i];
	}

	/* send message after the loop */
  	status = msgsnd(queue, &message, sizeof(message.data), 0);

    /* retrieve message from queue */
	//last arg is msgflg, if 0 it is blocking, only -1 works (that we have found)
    status_receive = msgrcv(queue_receive, &message_receive, sizeof(message_receive.data), 0, -1); 
	if ((message_receive.mtype == 1) && (prev != message_receive.data[0])) {
      printf("-------SETPOINT-------\nsetpoint = %d\n", message_receive.data[0]);
      prev = message_receive.data[0];
      write(fifo_send, &prev, sizeof(int));
    }

	sleep(1);
  }

  return 0;
}

