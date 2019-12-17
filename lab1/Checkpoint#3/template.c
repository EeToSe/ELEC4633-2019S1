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

/* Store data needed for the thread */
RT_TASK thread_data1;
RT_TASK thread_data2;
RT_TASK thread_data3;

/* Task 1 */
void thread_code1(long arg)
{
	rt_task_suspend(&thread_data3);
	printk("Task1!\n\n");
	rt_task_resume(&thread_data2);
	rt_task_wait_period();
}

/* Task 2 */
void thread_code2(long arg)
{
	printk("Task2!\n\n");
	rt_task_resume(&thread_data3);
}

/* Task 3 */
void thread_code3(long arg)
{	
	rt_task_suspend(&thread_data2);
	printk("Task3!\n\n");
}

/* Called when "insmod" is used */
static int __init template_init(void)
{   
  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD);

  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&thread_data1, thread_code1,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data1, NOW, PERIOD*n);

  rt_task_init(&thread_data2, thread_code2,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
	       
  rt_task_init(&thread_data3, thread_code3,
	       ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&thread_data1);
  rt_task_delete(&thread_data2);
  rt_task_delete(&thread_data3);
}

module_init(template_init);
module_exit(template_exit);
