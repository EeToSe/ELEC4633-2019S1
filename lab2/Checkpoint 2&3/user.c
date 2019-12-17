#include <stdio.h>

#include <rtai.h>
#include <rtai_shm.h>

int main(void)
{ 
  /* Define two variables for retrieving data */
  int data[10];
  int fifo;
  
  /* Open the file for reading only */
  fifo=open("/dev/rtf0", O_RDONLY);
  
  /* Display to the window */
  while (1)
  {
	printf("-------CHUNK OF DATA-------\n"); //for our own benefit
	int i;
	
	/* Read values from FIFO and print them out*/
	for (i=0; i < 10; i++) {
  		read(fifo, &data[i], sizeof(int));
		printf("%d\n", data[i]);
	}
	sleep(1);
  }

  return 0;
}

