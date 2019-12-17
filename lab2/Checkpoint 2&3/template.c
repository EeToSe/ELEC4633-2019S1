#include <linux/module.h>
#include <linux/init.h>

#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>

#include <linux/comedi.h>
#include <linux/comedilib.h>

#define ARG 0
#define STACK_SIZE 1024
#define PRIORITY RT_SCHED_HIGHEST_PRIORITY
#define USE_FPU 1
#define NOW rt_get_time()
#define PERIOD nano2count(1e9) //1sec
#define PERIOD_10HZ nano2count(1e8) //10Hz
#define ERROR -1
#define SETPOINT 90

#define READ_SUBDEVICE 0
#define WRITE_SUBDEVICE 1
#define READ_CHANNEL 0
#define WRITE_CHANNEL 0
#define RANGE 0
#define AREF AREF_GROUND


/* Store data needed for the thread */
RT_TASK thread_data;

/* Data needed by comedi */
comedi_t *comedi_dev;

/* Circular buffer */
int buffer[100];
int in = 0;	//for write
int out = 0; //for read
int prev = ERROR; //for scaling

/* Write to the circular buffer */
int write (int* value) {
	buffer[in] = *value;
	prev = in;
	in++;
	if (in == 100) in = 0;
}

/* Read from the circular buffer  */
int read () {
	int result;
	if (in == out) {
		result = ERROR; //underflow (error defined as -1)
	} else {
		result = buffer[out];
		out++;
		if(out == 100) out = 0;
	}
	return result;
}

/* Scaling functions degrees to A2D/D2A value */
int daq_to_deg (int read_value) {
	int read_deg = 0;
	if (read_value >= 0 && read_value < 2048) {
		read_deg = 180 - (read_value*360/4096);
	} else {
		read_deg = 180 - ((read_value-4096)*360/4096);
	}
	// fix the 180deg issue
	if (prev != ERROR ) {
		int prev_deg = buffer[prev];
		int diff = read_deg - prev_deg;
		if (diff > 170 && diff < 300) {
			read_deg = 180;
		}
	}
	write(&read_deg);
	return read_deg;
}

int deg_to_daq (int deg) {
	return deg;
}

/* The code for thread0 */
void thread_code(long arg)
{
  while (1)
  {
    int read_value, write_value = 2047; /* What value should write_value be? --> to stop it's 2047 i.e. speed = 0*/
    
	/* To read analogue data from a A/D converter on the DAQ */
	comedi_data_read(comedi_dev, READ_SUBDEVICE, READ_CHANNEL, RANGE, AREF, &read_value); 

	/* To write data to a D/A converter on the DAQ */
    comedi_data_write(comedi_dev, WRITE_SUBDEVICE, WRITE_CHANNEL, RANGE, AREF, deg_to_daq(write_value));
	
	/* Print the read_value and scale it to degrees */
	int result = daq_to_deg(read_value);
    printk("Read: 12bit = %d degrees = %d\n", read_value, result);
	
	/* Implement the proportional control algorithm and scale it */

	/* Communicate the data using RT-FIFO */
	rtf_put(0, &result, sizeof(int));
    rt_task_wait_period();
  }
}

/* Called when "insmod" is used */
static int __init template_init(void)
{
  /* Create a RT-FIFO of specified size */	
  rtf_create(0, 10*sizeof(int)); //to hold 10 ints

  /* Start the RT timer, NB only needs to be done once */
  rt_set_periodic_mode();
  start_rt_timer(PERIOD_10HZ);
  
  /* To open DAQ for use*/
  comedi_dev = comedi_open("/dev/comedi0");

  /* Initialise the data associated with a thread and make it periodic */
  rt_task_init(&thread_data, thread_code, ARG, STACK_SIZE, PRIORITY, USE_FPU, NULL);
  rt_task_make_periodic(&thread_data, NOW, PERIOD_10HZ);

  /* Return success */
  return 0;
}

/* Called when "rmmod" is used */
static void __exit template_exit(void)
{
  rt_task_delete(&thread_data);
  comedi_close(comedi_dev);
}

module_init(template_init);
module_exit(template_exit);
