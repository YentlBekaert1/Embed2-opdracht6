#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xbd97005b, "module_layout" },
	{ 0xc65b2c7a, "device_destroy" },
	{ 0xafe18e20, "class_destroy" },
	{ 0xfada38fe, "device_create" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xf72f2e16, "cdev_del" },
	{ 0xd9379699, "__class_create" },
	{ 0x4a91e5dd, "cdev_add" },
	{ 0xc72c11da, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x977035dc, "wake_up_process" },
	{ 0x31556c9c, "kthread_create_on_node" },
	{ 0x86332725, "__stack_chk_fail" },
	{ 0x51a910c0, "arm_copy_to_user" },
	{ 0x8574ca6c, "gpio_request_array" },
	{ 0x9dfdf722, "gpio_free_array" },
	{ 0x223fc350, "kthread_stop" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xba4c293, "gpiod_set_raw_value" },
	{ 0x8f411754, "gpiod_get_raw_value" },
	{ 0xe30324cf, "gpio_to_desc" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0x8e865d3c, "arm_delay_ops" },
	{ 0xc5850110, "printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "96B114DF2F48A078984B555");
