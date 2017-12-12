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
MODULE_DESCRIPTION("LEDs gumstix One");
MODULE_AUTHOR("Vikram, Hariharan M ");


/* Major number */
static int myled_major = 61;
//static int catflag = 0;
static unsigned led = 29;
static struct timer_list * count_timer;
static int led_value = 0;
/* Declaration of memory.c functions */
static int myled_open(struct inode *inode, struct file *filp);
static int myled_release(struct inode *inode, struct file *filp);
static ssize_t myled_read(struct file *filp,
		char *buf, size_t count, loff_t *f_pos);
static ssize_t myled_write(struct file *filp,
		const char *buf, size_t count, loff_t *f_pos);

struct file_operations myled_fops = {
	read: myled_read,
	write: myled_write,
	open: myled_open,
	release: myled_release
};

static int my_init_module(void)
{
    int result;
	printk("Hello led world!\n");	
	/* Registering device */
	result = register_chrdev(myled_major, "myled", &myled_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"myled: cannot obtain major number %d\n", myled_major);
		return result;
	}
	gpio_direction_output(led,0);
	return 0;
}

static void my_cleanup_module(void)
{
	unregister_chrdev(myled_major, "myled");
	printk("Bye led world!\n");
}

static ssize_t myled_write(struct file *filp, const char *buf,
							size_t count, loff_t *f_pos)
{
	char *name=(char *) kmalloc((size_t)1200,GFP_KERNEL);
	memset(name,0,sizeof(name));
    if(buf[0] == '1'){
        led_value =1;
        gpio_set_value(led,led_value);
        }
    else if(buf[0] == '0'){
        led_value = 0;
        gpio_set_value(led,led_value);
        }
	return count;

}


static ssize_t myled_read(struct file *filp, char *buf, 
							size_t count, loff_t *f_pos)
{ 
	char* output_buffer=(char *) kmalloc((size_t)250,GFP_KERNEL);
    int outcount=0; int ret;
	memset(output_buffer,0,(size_t)1024);

	outcount=sprintf(output_buffer,"%d\n",led_value);
	ret = copy_to_user(buf, output_buffer , outcount);
	if (ret)
	{
		return -EFAULT;
	}
	return outcount; 
}

static int myled_open(struct inode *inode, struct file *filp)
{

	/* Success */
	//catflag=0;
	return 0;
}

static int myled_release(struct inode *inode, struct file *filp)
{

	/* Success */
	//catflag=0;
	return 0;
}



module_init(my_init_module);
module_exit(my_cleanup_module);

