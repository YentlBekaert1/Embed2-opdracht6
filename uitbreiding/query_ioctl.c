#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h> 

#include <linux/gpio.h>
#include <linux/kthread.h>
#include <linux/delay.h>

#include "query_ioctl.h"
 
#define FIRST_MINOR 0
#define MINOR_CNT 1
 
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

//instellen parameters
static int outputs[2] = {5,12}, level[2] = {1,0}, togglespeed[2] = {0,5};
int i = 0;

// ----------------------------- code uit deel A -------------------------------------------
/* Initial blink delay */
static int blink_delay1 = 100;
static int blink_delay2 = 100;

/* Task handle to identify thread */
static struct task_struct *ts1 = NULL;
static struct task_struct *ts2 = NULL;

static struct gpio leds[] = {
		{  4, GPIOF_OUT_INIT_HIGH, "LED 1" },
		{ 25, GPIOF_OUT_INIT_LOW, "LED 2" },
};

static int output1_thread(void *data){
    printk(KERN_INFO "%s\n=============\n", __func__);
    while(!kthread_should_stop()){
        gpio_set_value(leds[0].gpio, !gpio_get_value(leds[0].gpio));
        mdelay(blink_delay1);
        printk(KERN_INFO "%s ============= %d \n",__func__,!gpio_get_value(leds[0].gpio));
    }
    return 0;
}

static int output2_thread(void *data){
    printk(KERN_INFO "%s\n=============\n", __func__);
    while(!kthread_should_stop()){
        gpio_set_value(leds[1].gpio, !gpio_get_value(leds[1].gpio));
        mdelay(blink_delay2);
        printk(KERN_INFO "%s ============= %d \n",__func__,!gpio_get_value(leds[1].gpio));
    }
    return 0;
}

//---------------------------------- ioctl vb --------------------------------------- 
static int my_open(struct inode *i, struct file *f)
{
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    return 0;
}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
    int gp;
    query_arg_t q;
    
    switch (cmd)
    {
        case QUERY_GET_VARIABLES:
            
            for( i = 0; i<2; i ++){
                q.outputs[i] = outputs[i] ;
                q.level[i]  = level[i] ;
                q.togglespeed[i]  = togglespeed[i] ;
            }
           
            if (copy_to_user((query_arg_t *)arg, &q, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            break;
        case QUERY_CLR_VARIABLES:
             i = 0;
            for( i = 0; i<2; i ++){
                q.outputs[i] = 0 ;
                q.level[i]  = 0;
                q.togglespeed[i]  =0;
            }
            break;
        case QUERY_SET_VARIABLES:
            if (copy_from_user(&q, (query_arg_t *)arg, sizeof(query_arg_t)))
            {
                return -EACCES;
            }
            i = 0;
            for( i = 0; i<2; i ++){
                outputs[i] = q.outputs[i];
                level[i] = q.level[i];
                togglespeed[i] = q.togglespeed[i];
            }
            if(ts1) {
                kthread_stop(ts1);
            }
            if(ts2) {
                kthread_stop(ts2);
            }
            gpio_free_array(leds, ARRAY_SIZE(leds));
            //hier komt de code voor als de waarden binnen komen
            i = 0;
            for( i = 0; i<2; i ++){
                leds[i].gpio =  outputs[i];
                printk(KERN_INFO "output[%d] = %d\n", i, leds[i].gpio);
            }
            gp = gpio_request_array(leds, ARRAY_SIZE(leds));
            if(gp) {
                printk(KERN_ERR "Unable to request GPIOs: %d\n", gp);
            }  
            i = 0;
            for( i = 0; i<2; i ++){
                printk(KERN_INFO "level[%d] = %d\n", i, level[i]);
                gpio_set_value(leds[i].gpio, level[i]);
            }   
            for( i = 0; i<2; i ++){
                togglespeed[i] = q.togglespeed[i];
                if(togglespeed[i] != 0){
                    if(i == 0){
                        blink_delay1 = togglespeed[i]*100;
                        ts1 = kthread_create(output1_thread, NULL, "output1_thread");
                        if(ts1) {
                            wake_up_process(ts1);
                            printk(KERN_INFO "created output_thread1 met delay %d\n", blink_delay1);
                        }
                        else {
                            printk(KERN_ERR "Unable to create output_thread1\n");
                        }
                    }
                    if(i == 1){
                        blink_delay2 = togglespeed[i]*100;
                        ts2 = kthread_create(output2_thread, NULL, "output_thread");
                        if(ts2) {
                            wake_up_process(ts2);
                            printk(KERN_INFO "created output_thread2\n");
                        }
                        else {
                            printk(KERN_ERR "Unable to create output_thread2 met delay %d\n",blink_delay2);
                        }
                    }
                }
            }
            
            printk(KERN_INFO "created values\n");
            break;
        default:
            return -EINVAL;
    }
 
    return 0;
}
 
static struct file_operations query_fops =
{
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    .ioctl = my_ioctl
#else
    .unlocked_ioctl = my_ioctl
#endif
};
 
static int __init query_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "%s\n=============\n", __func__);
 
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "query_ioctl")) < 0)
    {
        return ret;
    }
 
    cdev_init(&c_dev, &query_fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "query")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
 
    return 0;
}
 
static void __exit query_ioctl_exit(void)
{
    printk(KERN_INFO "%s\n=============\n", __func__);
    int i;
    // turn all LEDs off
	for(i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0); 
	}
	
	// unregister all GPIOs
	gpio_free_array(leds, ARRAY_SIZE(leds));
        if(ts1) {
            kthread_stop(ts1);
        }
        if(ts2) {
            kthread_stop(ts2);
        }
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
}
 
module_init(query_ioctl_init);
module_exit(query_ioctl_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Anil Kumar Pugalia <email_at_sarika-pugs_dot_com>");
MODULE_DESCRIPTION("Query ioctl() Char Driver");
