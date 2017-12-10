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
#define PWM_PIR GPIO16_PWM0
#define CK_EN CKEN0_PWM0
void setDirs(void);
void setGpio(void);
static void count_handler(unsigned long);
/* Major number */
static int mygpio_major = 61;
static int catflag = 0;
static struct timer_list * count_timer;

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
void setGpio(void)
{
    //setting checking PIRs 
    //enabling the LED On and OFF
    PWM_CTRL0=2; //Freq/PreScale + 1
    PWM_PWDUTY0=0x0FF;
	PWM_PERVAL0=0x1f4;
}


void setDirs(void)
{
	pxa_gpio_mode( GPIO16_PWM0_MD);
	pxa_set_cken(CK_EN,1); //pxa_set_cken(CK_EN,1);
}
static void count_handler(unsigned long data) {

		//mod_timer(count_timer, jiffies + msecs_to_jiffies(speeds[slevel]));	
		//printk("Handler %d %d\n",_count,dir);
}


static int my_init_module(void)
{
	printk("Hello world!\n");
	int result;
	/* Registering device */
	result = register_chrdev(mygpio_major, "mygpio", &mygpio_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"mygpio: cannot obtain major number %d\n", mygpio_major);
		return result;
	}
	count_timer = (struct timer_list *) kmalloc(sizeof(struct timer_list), GFP_KERNEL);
    //setting PIR as an interrupt	
	setDirs();
	setGpio();
	return 0;
}

static void my_cleanup_module(void)
{
	unregister_chrdev(mygpio_major, "mygpio");
    PWM_CTRL0=0;
	PWM_PWDUTY0=0x000;
	PWM_PERVAL0=0x000;
	printk("Bye world!\n");
}
static ssize_t mygpio_write(struct file *filp, const char *buf,
							size_t count, loff_t *f_pos)
{
	char *name=(char *) kmalloc((size_t)1200,GFP_KERNEL);
	memset(name,0,sizeof(name));
	return count;

}
char *sstrs[]={"H","M","L"};
char *bstrs[]={"L","M","H"};
static ssize_t mygpio_read(struct file *filp, char *buf, 
							size_t count, loff_t *f_pos)
{ 
	char* output_buffer=(char *) kmalloc((size_t)250,GFP_KERNEL);
	memset(output_buffer,0,(size_t)1024);

	int outcount=0;
	outcount=sprintf(output_buffer,"HELLO \n");

	if(catflag==0)
	{
	int ret = copy_to_user(buf, output_buffer , outcount);
	if (ret)
	{
		return -EFAULT;
	}
	catflag=1;
	return outcount; 

	}
	else if(catflag==1)
	{
	return 0;
	}
}

static int mygpio_open(struct inode *inode, struct file *filp)
{

	/* Success */
	catflag=0;
	return 0;
}

static int mygpio_release(struct inode *inode, struct file *filp)
{

	/* Success */
	catflag=0;
	return 0;
}



module_init(my_init_module);
module_exit(my_cleanup_module);

