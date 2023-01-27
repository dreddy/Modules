
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/mm.h>

static unsigned long kpage;

struct idtr_t {
  void *base;
  unsigned short limit;
} idtr;

static int dumpidt_init(void)
{
	printk("dumpidt: %s \n", __FUNCTION__);
	kpage = __get_free_page(GFP_KERNEL);
	if (!kpage)
		return -ENOMEM;
	printk("dumpidt: kpage : 0x%lx \n", kpage);

	asm volatile ("sidt %0" : "=m"(idtr));
	printk("idt.limit = %x idt.base = %p\n", idtr.limit, idtr.base);
	return 0;
}

static void dumpidt_cleanup(void)
{
	printk("dumpidt: %s \n", __FUNCTION__);
	if (kpage)
		free_page(kpage);
}

module_init(dumpidt_init);
module_exit(dumpidt_cleanup);

MODULE_LICENSE("GPL");
