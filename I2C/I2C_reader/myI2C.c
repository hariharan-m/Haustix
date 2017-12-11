#ifndef __KERNEL__
#define __KERNEL__
#endif

#ifndef MODULE
#define MODULE
#endif

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */
#include <asm-arm/arch-pxa/gpio.h>
#include <linux/delay.h> /* MSLEEP */
#include <linux/interrupt.h>
#include <asm/arch/pxa-regs.h>
#include <asm-arm/arch/hardware.h>

/************************************
 * Set Module Info
 ************************************/
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("I2C testing");
MODULE_AUTHOR("Vikram, Hariharan M ");

static char *value; 

/* Major number */
static int mygpio_major = 61;
/* Declaration of memory.c functions */
static int mygpio_open(struct inode *inode, struct file *filp);
static int mygpio_release(struct inode *inode, struct file *filp);
static ssize_t mygpio_read(struct file *filp,
		char *buf, size_t count, loff_t *f_pos);
static ssize_t mygpio_write(struct file *filp,
		const char *buf, size_t count, loff_t *f_pos);

struct file_operations mygpio_fops = {
	read: mygpio_read,
	write: mygpio_write,
	open: mygpio_open,
	release: mygpio_release
};

static int my_init_module(void)
{
    int result;
    int ret = 0;
	printk("Hello I2C world!\n");	
	/* Registering device */
	result = register_chrdev(mygpio_major, "mygpio", &mygpio_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain major number %d\n", mygpio_major);
		return result;
	}
    
    value = (char *)kmalloc( sizeof(char)*10,GFP_KERNEL);
    if (!value) {
      ret = -ENOMEM;
    } 
    else {
    memset( value, 0,sizeof(char)*10 );
	return 0;
}
}

static void my_cleanup_module(void)
{
	unregister_chrdev(mygpio_major, "mygpio");
    kfree(value);
	printk("Bye I2C world!\n");
}

static ssize_t mygpio_write(struct file *filp, const char *buf,
							size_t count, loff_t *f_pos)
{
	  if (copy_from_user( value, buf, count )) {
    return -EFAULT;
  }


return count;

}


static ssize_t mygpio_read(struct file *filp, char *buf, 
							size_t count, loff_t *f_pos)
{ 
	char* output_buffer=(char *) kmalloc((size_t)250,GFP_KERNEL);
    int outcount=0; int ret;
	memset(output_buffer,0,(size_t)1024);

	outcount=sprintf(output_buffer,"%s\n",value);
	ret = copy_to_user(buf, output_buffer , outcount);
	if (ret)
	{
		return -EFAULT;
	}
	return outcount; 
}

static int mygpio_open(struct inode *inode, struct file *filp)
{

	return 0;
}

static int mygpio_release(struct inode *inode, struct file *filp)
{

	return 0;
}



module_init(my_init_module);
module_exit(my_cleanup_module);

