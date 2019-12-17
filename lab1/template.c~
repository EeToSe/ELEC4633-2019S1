#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_sched.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()
#define PERIOD nano2count(1e9)
#define n 1
#define m 2

/* Store data needed for the thread really important!!! */
RT_TASK thread_data1;
RT_TASK thread_data2;

/* Define the integer value */
static int i = 1;

/* Task1 will increment the integer every n seconds */
void thread_code1(long arg)
{
  while (1)
  {
    /* */
    i++;
    rt_task_wait_period();
  }
}

/* Task2 will display a message every m seconds */
void thread_code2(long arg)
{
  while (1)
  {
    /* */
    printk("The integer value: %d\n", i);
    rt_task_wait_period();
  }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD);

  /* Initialise the data associated with task1 and make it periodic */
  rt_task_init(&thread_data1, thread_code1,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data1, NOW, PERIOD*n);
  
  /* Initialise the data associated with task2 and make it periodic */
  rt_task_init(&thread_data2, thread_code2,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data2, NOW, PERIOD*m);
  
  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&thread_data1);
  rt_task_delete(&thread_data2);
}

module_init(template_init);		
module_exit(template_exit);
