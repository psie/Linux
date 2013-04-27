// a kernel module adding sleep to do_fork() with the use of tracepoints 

#include <linux/module.h>
#include <linux/delay.h>
#include <trace/events/sched.h>

static void probe_sched_process_fork(void *ignore, struct task_struct *parent, struct task_struct *child)
{
	if(strcmp("bash", child->comm) != 0 && strcmp("sshd", child->comm) != 0)
	{	
		printk(KERN_INFO "do_fork(): child:%s\n", child->comm); 
		msleep(300);
	}
	else
	{
		printk(KERN_INFO "do_fork(): child:%s (won't be delayed)\n", child->comm);
	}
}

static int __init tp_sample_trace_init(void)
{
	int ret;

	ret = register_trace_sched_process_fork(probe_sched_process_fork, NULL);
	WARN_ON(ret);

	return 0;
}

module_init(tp_sample_trace_init);

static void __exit tp_sample_trace_exit(void)
{
	unregister_trace_sched_process_fork(probe_sched_process_fork, NULL);
	tracepoint_synchronize_unregister();
}

module_exit(tp_sample_trace_exit);

MODULE_LICENSE("GPL");
