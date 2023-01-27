/*
 * send_ipi.c
 */

#include <linux/module.h>
#include <linux/smp.h>
#include <asm/apic.h>
#include <asm/msr.h>
#include <asm/irq_vectors.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

#define X2APIC 1

struct dentry *file;

static ssize_t send_spurious_ipi(struct file *file, const char __user *buf,
				 size_t count, loff_t *ppos)
{
	char mybuf[32];
	int  command, rv;
	u64  ts;
	unsigned int id, cfg;
	
	if (count > 10)
		return -EINVAL;

	rv=copy_from_user(mybuf, buf, count);
	if (rv != 0)
		return -EINVAL;
	
	sscanf(mybuf, "%d", &command);

	switch(command) {
	case 10:
		id = smp_processor_id();
#ifdef XAPIC
		__xapic_wait_icr_idle();
		cfg = __prepare_ICR(APIC_DEST_SELF, SPURIOUS_APIC_VECTOR, id);
		ts = rdtsc_ordered();
		native_apic_mem_write(APIC_ICR, cfg);
#endif
#ifdef X2APIC
		ts=rdtsc_ordered();
		apic_write(APIC_SELF_IPI, SPURIOUS_APIC_VECTOR);
#endif
		break;
	default:
		break;
	}
	printk("spurious sent at %llu\n", ts);
	
	return count;
}

static const struct file_operations sendipi_fops = {
	.write = send_spurious_ipi,
};

int sp_init(void)
{
	file = debugfs_create_file("dr_send_spurious", 0666, NULL, NULL, &sendipi_fops);
	return 0;
}

void sp_cleanup(void)
{
	debugfs_remove(file);
}

MODULE_LICENSE("GPL");
module_init(sp_init);
module_exit(sp_cleanup);
