/*
 * bypass_strict_devmem.c
 *
 * This simple kernel module override the CONFIG_STRICT_DEVMEM check
 * and always return 1 so userspace application can mmap to /dev/mem
 * to debug with physical memory.
 *
 * usage: insmod bypass_strict_devmem.ko
 *
 * For more information on theory of operation of kretprobes, see
 * Documentation/kprobes.txt
 *
 * Author: Patrick Lu <patrick.lu@intel.com>
 *
 * Inspire by:
 * https://www.redhat.com/archives/crash-utility/2008-March/msg00036.html
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/ktime.h>
#include <linux/limits.h>
#include <linux/sched.h>

static char func_name[NAME_MAX] = "devmem_is_allowed";
module_param_string(func, func_name, NAME_MAX, S_IRUGO);
MODULE_PARM_DESC(func, "This module will bypass CONFIG_STRICT_DEVMEM and"
                 " allow userspace tools to mmap to /dev/mem");

/*
 * Return 1 for devmem_is_allowed
 */
static int ret_handler(struct kretprobe_instance *ri, struct pt_regs *regs)
{
  int retval = regs_return_value(regs);

  printk(KERN_INFO "%s[%d] called %s returned %d override to 1\n",
         current->comm, current->pid, func_name, retval);

  regs->ax = 1;
  return 0;
}

static struct kretprobe my_kretprobe = {
  .handler= ret_handler,
  /* Probe up to 20 instances concurrently. */
  .maxactive= 20,
};

static int __init kretprobe_init(void)
{
  int ret;

  my_kretprobe.kp.symbol_name = func_name;
  ret = register_kretprobe(&my_kretprobe);
  if (ret < 0) {
    printk(KERN_INFO "register_kretprobe failed, returned %d\n",
           ret);
    return -1;
  }
  printk(KERN_INFO "Planted return probe at %s: %p\n",
         my_kretprobe.kp.symbol_name, my_kretprobe.kp.addr);
  return 0;
}

static void __exit kretprobe_exit(void)
{
  unregister_kretprobe(&my_kretprobe);
  printk(KERN_INFO "kretprobe at %p unregistered\n",
         my_kretprobe.kp.addr);

  /* nmissed > 0 suggests that maxactive was set too low. */
  printk(KERN_INFO "Missed probing %d instances of %s\n",
         my_kretprobe.nmissed, my_kretprobe.kp.symbol_name);
}

module_init(kretprobe_init)
module_exit(kretprobe_exit)
MODULE_LICENSE("GPL");
