#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h> //om gebruik te maken van module parameters

/*
 * Struct defining pins, direction and inital state 
 */
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


	// register LED GPIOs, turn LEDs on
	
    
	
	return ret;
}

/*
 * Module exit function
 */
static void __exit gpiomod_exit(void)
{
	int i;

	printk(KERN_INFO "%s\n=============\n", __func__);

	// turn all LEDs off
	for(i = 0; i < ARRAY_SIZE(leds); i++) {
		gpio_set_value(leds[i].gpio, 0); 
	}
	
	// unregister all GPIOs
	gpio_free_array(leds, ARRAY_SIZE(leds));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stefan Wendler");
MODULE_DESCRIPTION("Basic Linux Kernel module using GPIOs to drive LEDs");

module_init(gpiomod_init);
module_exit(gpiomod_exit);