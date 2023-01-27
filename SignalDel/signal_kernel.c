#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <asm/siginfo.h>	//siginfo
#include <linux/rcupdate.h>	//rcu_read_lock
#include <linux/sched.h>	//find_task_by_pid_type
#include <linux/debugfs.h>
#include <linux/uaccess.h>


#define SIG_TEST 44	// we choose 44 as our signal number (real-time signals are in the range of 33 to 64)


static uint64_t rdtsc(void)
{
	uint32_t lo, hi;
	/* We cannot use "=A", since this would use %rax on x86_64 */
	__asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
	return (uint64_t)hi << 32 | lo;
}
uint64_t vals[16];

struct dentry *file;

static ssize_t write_pid(struct file *file, const char __user *buf,
                                size_t count, loff_t *ppos)
{
	char mybuf[10];
	int pid = 0;
	int ret;
	struct siginfo info;
	struct task_struct *t;
	int i=5;

	/* read the value from user space */
	if(count > 10)
		return -EINVAL;
	copy_from_user(mybuf, buf, count);
	sscanf(mybuf, "%d", &pid);
	printk("pid = %d\n", pid);

	/* send the signal */
	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = SIG_TEST;
	info.si_code = SI_QUEUE;	// this is bit of a trickery: SI_QUEUE is normally used by sigqueue from user space,
					// and kernel space should use SI_KERNEL. But if SI_KERNEL is used the real_time data 
					// is not delivered to the user space signal handler function. 
	info.si_int = 1234;  		//real time signals may have 32 bits of data.

	rcu_read_lock();
	t = pid_task(find_pid_ns(pid, &init_pid_ns), PIDTYPE_PID);	
	if(t == NULL){
		printk("no such pid\n");
		rcu_read_unlock();
		return -ENODEV;
	}
	rcu_read_unlock();

	while (i--) {
		vals[i]=rdtsc();
		ret = send_sig_info(SIG_TEST, &info, t);    //send the signal
		if (ret < 0) {
			printk("error sending signal\n");
			return ret;
		}
	}
	for(i=0;i<6;i++)
		printk("Send %d at %llu\n", i, vals[i]);
	
	return count;
}

static const struct file_operations my_fops = {
	.write = write_pid,
};

static int __init signalexample_module_init(void)
{
	/* we need to know the pid of the user space process
 	 * -> we use debugfs for this. As soon as a pid is written to 
 	 * this file, a signal is sent to that pid
 	 */
	/* only root can write to this file (no read) */
	file = debugfs_create_file("signalconfpid", 0666, NULL, NULL, &my_fops);
	return 0;
}
static void __exit signalexample_module_exit(void)
{
	debugfs_remove(file);

}

module_init(signalexample_module_init);
module_exit(signalexample_module_exit);
MODULE_LICENSE("GPL");
