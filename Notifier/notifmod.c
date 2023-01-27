/*
 * notifmod.c
 * Created: Fri Jul 27 15:46:29 2007
 * Comment: Template for a simple char driver for poking in there
 */



#include <linux/module.h>
#include <linux/init.h>
#include <linux/cpu.h>

#define NOTIFMOD_VERSION	"0.01"

static char version[] __initdata = NOTIFMOD_VERSION __DATE__;
static int start_cnt;
static int stop_cnt;

static int notifmod_idle_notifier(struct notifier_block *nb,
				  unsigned long val,
				  void *data)
{
	printk("idle notify data %p\n", data);
	switch (val) {
	case IDLE_START:
		start_cnt++;
		break;
	case IDLE_END:
		stop_cnt++;
		break;
	default:
		printk("ERROR: Unhandled notification\n");
	}
	return 0;
}

static struct notifier_block notifmod_nb = {
	.notifier_call = notifmod_idle_notifier,
};


static int __init notifmod_init (void)
{
	printk(KERN_INFO "notifmod: %s\n", version);
	start_cnt = stop_cnt = 0;
	idle_notifier_register(&notifmod_nb);

	return 0;
}

static void __exit notifmod_exit (void)
{
	idle_notifier_unregister(&notifmod_nb);
	printk("start_cnt = %d , stop_cnt = %d\n", start_cnt, stop_cnt);
	printk(KERN_INFO "notifmod: module unloaded\n");
	return;
}

module_init (notifmod_init);
module_exit (notifmod_exit);

MODULE_LICENSE("GPL");
