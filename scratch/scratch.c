
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>

#define DEVNAME		"scratch"
#define NR_MINORS	1


static ssize_t scr_read(struct file *file, char __user *buf, size_t size,
			loff_t *poff)
{
	return 0;
}

static ssize_t scr_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *poff)
{
	int rv;
	u32 cmd=0;

	rv = copy_from_user(&cmd, buf, count);

	switch(cmd) {
	case 0x10:
	{
		int cpu;
		u32 lo, hi;
		for_each_online_cpu(cpu) {
			rdmsr_on_cpu(cpu, 0x1B, &lo, &hi);
			printk("scratch: MSR 0x1B at %d = %x %x\n",
			       cpu, lo, hi);
		}
		break;
	}
	default:
		printk("scratch: Unknown cmd 0x%x\n", cmd);
	}
	return count-rv;
}

static struct file_operations scr_fops = {
	.owner = THIS_MODULE,
	.read  = scr_read,
	.write = scr_write,
};

static struct miscdevice scr_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DEVNAME,
	.fops  = &scr_fops,
};


static int scratch_init(void)
{
	int rv;
	printk("scratch: %s \n", __FUNCTION__);
	rv = misc_register(&scr_device);
	if (rv) {
		printk("scratch: misc_register failed:: 0x%x\n", rv);
	}
	return 0;
}

static void scratch_cleanup(void)
{
	printk("scratch: %s \n", __FUNCTION__);
	misc_deregister(&scr_device);
}

module_init(scratch_init);
module_exit(scratch_cleanup);

MODULE_LICENSE("GPL");
