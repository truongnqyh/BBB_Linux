#include <linux/module.h> /* Needed by all modules */
#include <linux/kernel.h> /* Needed for KERN_INFO */
#include <linux/time.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/mod_devicetable.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

static const struct of_device_id blink_led_of_match[] = {
    {.compatible = "led-example0"},
    {},
};

int blink_led_driver_probe(struct platform_device *pdev)
{
    struct device_node *of_node = NULL;
    uint32_t led_config[2];
    int ret = -1;

    pr_info("Hello device tree start\n");

    of_node = pdev->dev.of_node;
    ret = of_property_read_u32_array(of_node, "led_config", led_config, 2);
    if (ret == 0) {
        pr_info("led_config: <%x %x>\n", led_config[0], led_config[1]);
    } else {
        pr_info("can not get property with label as led_config\n");
    }

    return 0;
}

int blink_led_driver_remove(struct platform_device *pdev)
{
    pr_info("Good bye device tree\n");
    return 0;
}

static struct platform_driver blink_led_driver = {
    .probe = blink_led_driver_probe,
    .remove = blink_led_driver_remove,
    .driver = {
        .name = "blink_led",
        .of_match_table = blink_led_of_match,
    },
};

module_platform_driver(blink_led_driver);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Blink Led kernel module");