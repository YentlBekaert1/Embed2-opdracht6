#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h> //om gebruik te maken van module parameters
#include <linux/kthread.h>
#include <linux/delay.h>
/*
 * Struct defining pins, direction and inital state 
 */
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

//bepalen van de argumenten
static int outputs[2] = { -1, -1 };
static int arr_argc_outputs = 0;
module_param_array(outputs, int, &arr_argc_outputs,0000);
MODULE_PARM_DESC(outputs, "outputs");

static int level[2] = { -1, -1 };
static int arr_argc_level = 0;
module_param_array(level, int, &arr_argc_level, 0000);
MODULE_PARM_DESC(level, "level");

static int togglespeed[2] = { -1, -1 };
static int arr_argc_toggle = 0;
module_param_array(togglespeed, int, &arr_argc_toggle, 0000);
MODULE_PARM_DESC(togglespeed, "togglespeed");

static int output1_thread(void *data){
    printk(KERN_INFO "%s\n=============\n", __func__);
    while(!kthread_should_stop()){
        gpio_set_value(leds[0].gpio, !gpio_get_value(leds[0].gpio));
        mdelay(blink_delay1);
    }
    return 0;
}

static int output2_thread(void *data){
    printk(KERN_INFO "%s\n=============\n", __func__);
    while(!kthread_should_stop()){
        gpio_set_value(leds[1].gpio, !gpio_get_value(leds[1].gpio));
        mdelay(blink_delay2);
    }
    return 0;
}
/*
 * Module init function
 */
static int __init gpiomod_init(void)
{
    
	int ret = 0;
    int i;

    printk(KERN_INFO "%s\n=============\n", __func__);

    //overloop alle argumenten voor outputs
    for (i = 0; i < (sizeof outputs / sizeof (int)); i++)
	{
       leds[i].gpio =  outputs[i];
	   printk(KERN_INFO "outputs[%d] = %d\n", i, outputs[i]);
       printk(KERN_INFO "gpio_struct.gpio[%d] = %ld\n", i, leds[i].flags); 
	}
	printk(KERN_INFO "got %d arguments for outputs.\n", arr_argc_outputs);

    // register output GPIOs, turn initial off
    ret = gpio_request_array(leds, ARRAY_SIZE(leds));
    if (ret) {
		printk(KERN_ERR "Unable to request GPIOs: %d\n", ret);
	}

    i = 0;

    //overloop alle argumenten voor levels
    for (i = 0; i < (sizeof level / sizeof (int)); i++)
	{
		printk(KERN_INFO "level[%d] = %d\n", i, level[i]);
        gpio_set_value(leds[i].gpio, level[i]);
	}
	printk(KERN_INFO "got %d arguments for level.\n", arr_argc_level);

    i = 0;

    //overloop alle argumenten voor toggle
    for (i = 0; i < (sizeof level / sizeof (int)); i++)
	{
		printk(KERN_INFO "toggle[%d] = %d\n", i, togglespeed[i]);
	}
	printk(KERN_INFO "got %d arguments for toggle speed.\n", arr_argc_toggle);

    //create threads for blinking
    if(togglespeed[0] != 0){
        blink_delay1 = togglespeed[0]*100;
        ts1 = kthread_create(output1_thread, NULL, "output1_thread");
        if(ts1) {
            wake_up_process(ts1);
             printk(KERN_INFO "created output_thread1\n");
        }
        else {
            printk(KERN_ERR "Unable to create output_thread1\n");
        }
    }
    if(togglespeed[1] != 0){
        blink_delay2 = togglespeed[1] * 100;
            ts2 = kthread_create(output2_thread, NULL, "output_thread");
            if(ts2) {
                wake_up_process(ts2);
                printk(KERN_INFO "created output_thread2\n");
            }
            else {
                printk(KERN_ERR "Unable to create output_thread2\n");
            }
    }
	
    
	return ret;
}

/*
 * Module exit function
 */
static void __exit gpiomod_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n=============\n", __func__);
    // terminate thread
	if(ts1) {
		kthread_stop(ts1);
	}
    if(ts2) {
		kthread_stop(ts2);
	}

	// turn all LEDs off
	for(i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0); 
	}
	
	// unregister all GPIOs
	gpio_free_array(leds, ARRAY_SIZE(leds));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yentl Bekaert");
MODULE_DESCRIPTION("Basic Linux Kernel module using GPIO outputs");

module_init(gpiomod_init);
module_exit(gpiomod_exit);