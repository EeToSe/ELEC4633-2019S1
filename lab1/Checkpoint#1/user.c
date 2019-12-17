#include <stdio.h>

#include <rtai.h>
#include <rtai_nam2num.h>
#include <rtai_shm.h>

int *mem;

int main(void)
{
  mem = rtai_malloc(nam2num("shmem"), sizeof(int));
  /* Add code */
  rtai_free(nam2num("shmem"));

  return 0;
}

