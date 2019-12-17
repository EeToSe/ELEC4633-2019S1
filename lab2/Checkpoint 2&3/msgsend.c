#include <stdio.h>
#include <sys/msg.h>

struct mymsgbuf {
  long mtype;
  int data[10];
};

struct mymsgbuf message;

int main(void)
{
  int status, i;
  int queue;
  key_t msgkey;

  // generate a key to obtain a queue id
  msgkey = ftok(".",'m');

  // obtain a queue identifier
  queue = msgget(msgkey, IPC_CREAT | 0660);

  // set up message
  // set the message type to 1 - can be any positive number
  message.mtype = 1;

  // set up message data - Fibonacci sequence
  message.data[0] = 1;
  message.data[1] = 0;
  for (i=2; i<10; i++) message.data[i] = message.data[i-1] + message.data[i-2];

  // send message
  status = msgsnd(queue, &message, sizeof(message.data), 0);

  return 0;
}


