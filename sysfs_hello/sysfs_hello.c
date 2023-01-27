
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/sysfs.h>
#include <linux/string.h>

static struct kobject *hello_kobject;
static int localvalue=0;

static ssize_t drhello_show(struct kobject *kobj, struct kobj_attribute *attr,
                      char *buf)
{
        return sprintf(buf, "%d\n", localvalue);
}

static ssize_t drhello_store(struct kobject *kobj, struct kobj_attribute *attr,
                      const char *buf, size_t count)
{
        sscanf(buf, "%du", &localvalue);
        return count;
}

static struct kobj_attribute hello_attribute = __ATTR(drhello, 0660, drhello_show,
							drhello_store);

static int sysfs_hello_init(void)
{
	int rv=0;

	printk("sysfs_hello: %s \n", __FUNCTION__);
	hello_kobject = kobject_create_and_add("hello", kernel_kobj);
	if (!hello_kobject)
		return -ENOMEM;

	rv = sysfs_create_file(hello_kobject, &hello_attribute.attr);
	if (rv) {
		printk("Error: creating sysfs file\n");
	}
	return rv;
}

static void sysfs_hello_cleanup(void)
{
	printk("sysfs_hello: %s \n", __FUNCTION__);
	kobject_put(hello_kobject);
}

module_init(sysfs_hello_init);
module_exit(sysfs_hello_cleanup);

MODULE_LICENSE("GPL");
