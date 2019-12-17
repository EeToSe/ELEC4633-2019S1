#include <stdio.h>
#include <sys/msg.h>

struct mymsgbuf {
  long mtype;
  int data[1];
};

struct mymsgbuf message;

int main(void)
{
  int status;
  int queue;
  key_t msgkey;

  // generate a key to obtain a queue id
  msgkey = ftok(".",'p');

  // obtain a queue identifier
  queue = msgget(msgkey, IPC_CREAT | 0660);

  // set up message
  // set the message type to 1 - can be any positive number
  message.mtype = 1;

  // prompt user to enter setpoint
  int setpoint = 0;

  while (1) {
	printf("Enter setpoint: ");
	scanf("%d", &setpoint);
	if (setpoint >= 0 && setpoint <= 360) {
      // set up message data
      message.data[0] = setpoint;
      // send message
      status = msgsnd(queue, &message, sizeof(message.data), 0);
    } else {
      printf("Setpoint must be between 0 and 360\n");
    }
  }

  return 0;
}


