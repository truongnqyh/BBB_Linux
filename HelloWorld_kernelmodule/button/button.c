#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/time.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Truong Nguyen Van");
///< The description -- see modinfo
MODULE_DESCRIPTION("A simple Hello world LKM!");
///< The version of the module
MODULE_VERSION("0.1");

#define LED_PIN 31
#define BUTTON_PIN 30
#define BUTTON_DEV "exampleIRQ"
#define GPIO_ADDR_BASE (0x44E07000U)
#define GPIO_ADDR_SIZE (0x1000U)
#define GPIO_SETDO_OFFSET (0x194U)
#define GPIO_CLEARDO_OFFSET (0x190U)
#define GPIO_OE_OFFSET (0x134U)
#define LED_OUTPUT ~(1 << LED_PIN)
#define BUTTON_INPUT (1 << BUTTON_PIN)
#define LED_DATA_OUT (1 << LED_PIN)

void __iomem *base_addr;
struct timer_list my_timer;
int irq_num = 0;

static irqreturn_t button_isr(int irq, void *data)
{
    static int count;

    if (count % 2 == 0)
    {
        pr_info("button pressed\n");
        writel_relaxed(LED_DATA_OUT, base_addr + GPIO_SETDO_OFFSET);
    }
    else
    {
        pr_info("button released\n");
        writel_relaxed(LED_DATA_OUT, base_addr + GPIO_CLEARDO_OFFSET);
    }

    count++;

    return IRQ_HANDLED;
}

static int button_init(void) 
{ 
    uint32_t register_data = 0;
    int ret = 0;

    printk(KERN_INFO "Loading hello module...\n"); 
    printk(KERN_INFO "Hello world\n"); 
    pr_info("irq button driver is loaded\n");
    /* Virtual address mapping */
    base_addr = ioremap(GPIO_ADDR_BASE, GPIO_ADDR_SIZE);
    /* Configure LED pin as output */
    register_data = readl_relaxed(base_addr + GPIO_OE_OFFSET);
    register_data &= LED_OUTPUT;
    writel_relaxed(register_data, base_addr + GPIO_OE_OFFSET);

    /* Configure BUTTON pin as input */
    register_data = readl_relaxed(base_addr + GPIO_OE_OFFSET);
    register_data |= BUTTON_INPUT;
    writel_relaxed(register_data, base_addr + GPIO_OE_OFFSET);
 /* Get the irq number */
    irq_num = gpio_to_irq(BUTTON_PIN);
    ret = request_irq(irq_num, button_isr, IRQF_TRIGGER_RISING, "button_gpio30", BUTTON_DEV);

    return 0; 
} 
  
static void button_exit(void) 
{ 
    printk(KERN_INFO "Goodbye Mr.\n"); 
    pr_info("hello world driver is unloaded\n");
    writel_relaxed(LED_DATA_OUT, base_addr + GPIO_CLEARDO_OFFSET);
    free_irq(irq_num, BUTTON_DEV);
    gpio_free(BUTTON_PIN);
    gpio_free(LED_PIN);
} 

module_init(button_init);
module_exit(button_exit);
