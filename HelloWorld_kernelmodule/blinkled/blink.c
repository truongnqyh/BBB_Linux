#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/init.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Truong Nguyen Van");
///< The description -- see modinfo
MODULE_DESCRIPTION("A simple Hello world LKM!");
///< The version of the module
MODULE_VERSION("0.1");

#define GPIO_ADDR_BASE (0x44E07000U)
#define GPIO_ADDR_SIZE (0x1000U)
#define GPIO_SETDO_OFFSET (0x194U)
#define GPIO_CLEARDO_OFFSET (0x190U)
#define GPIO_OE_OFFSET (0x134U)
#define LED ~(1 << 31)
#define DATA_OUT (1 << 31)

void __iomem *base_addr;
struct timer_list my_timer;

static void timer_function(struct timer_list *t)
{
    int index = 0;

    for (index = 0; index < 10; index++)
    {
        printk(KERN_INFO "turn on LED...\n");  
        writel_relaxed(DATA_OUT, base_addr + GPIO_SETDO_OFFSET);
        msleep(1000);
        printk(KERN_INFO "turn off LED...\n"); 
        writel_relaxed(DATA_OUT, base_addr + GPIO_CLEARDO_OFFSET);
        msleep(1000);
    }
}
static int blink_init(void) 
{ 
    uint32_t register_data = 0;
    printk(KERN_INFO "Loading hello module...\n"); 
    printk(KERN_INFO "Hello world\n"); 

    base_addr = ioremap(GPIO_ADDR_BASE, GPIO_ADDR_SIZE);
    register_data = readl_relaxed(base_addr + GPIO_OE_OFFSET);
    register_data &= LED;
    writel_relaxed(register_data, base_addr + GPIO_OE_OFFSET);

    timer_setup(&my_timer, timer_function, 0);
    add_timer(&my_timer);

    return 0; 
} 
  
static void blink_exit(void) 
{ 
    del_timer_sync(&my_timer);
    printk(KERN_INFO "Goodbye Mr.\n"); 
} 

module_init(blink_init);
module_exit(blink_exit);