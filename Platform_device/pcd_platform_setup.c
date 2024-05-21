#include <linux/module.h>
#include <linux/platform_device.h>
#include "platform.h"

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Truong Nguyen Van");
///< The description -- see modinfo
MODULE_DESCRIPTION("plarform device setup");
///< The version of the module
MODULE_VERSION("0.1");

#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

void pcdev_release(struct device *dev)
{
    pr_info("pcdev_release\n");
}
/* Create 2 platform data structures */
struct pcdev_platform_data pcdev_pdata[4] = {
    [0] = {.size = 512,  .perm = RDWR, .serial_number = "PCD1111"},
    [1] = {.size = 1024, .perm = RDWR, .serial_number = "PCD2222"},
    [2] = {.size = 64,   .perm = RDONLY, .serial_number = "PCD3333"},
    [3] = {.size = 256,  .perm = RDWR, .serial_number = "PCD4444"}
};

/* Create 2 platform devices */ 
struct platform_device platform_pcdev[4] = {
    [0] = {
        .name = "pcd_1",
        .id = 0,
        .num_resources = 0,
        .resource = NULL,
        .dev = {
           .platform_data = &pcdev_pdata[0],
           .release = pcdev_release /* Need to initialize platform device as callback */
        }
    },
    [1] = {
        .name = "pcd_2",
        .id = 1,
        .num_resources = 0,
        .resource = NULL,
        .dev = {
           .platform_data = &pcdev_pdata[1],
           .release = pcdev_release
        }
    },
    [2] = {
        .name = "pcd_3",
        .id = 2,
        .num_resources = 0,
        .resource = NULL,
        .dev = {
            .platform_data = &pcdev_pdata[2],
            .release = pcdev_release
        }
    },
    [3] = {
        .name = "pcd_4",
        .id = 3,
        .num_resources = 0,
        .resource = NULL,
        .dev = {
            .platform_data = &pcdev_pdata[3],
            .release = pcdev_release
        }
    }
};

static int __init pcdev_platform_init(void)
{
    /* Register platform devices */
    platform_device_register(&platform_pcdev[0]);
    platform_device_register(&platform_pcdev[1]);
    platform_device_register(&platform_pcdev[2]);
    platform_device_register(&platform_pcdev[3]);
    pr_info("Device setup inserted \n");

    return 0;
}

static void __exit pcdev_platform_exit(void)
{
    /* Unregister platform devices */
    platform_device_unregister(&platform_pcdev[0]);
    platform_device_unregister(&platform_pcdev[1]);
    platform_device_unregister(&platform_pcdev[2]);
    platform_device_unregister(&platform_pcdev[3]);
    pr_info("Device setup removed \n");
}

module_init(pcdev_platform_init);
module_exit(pcdev_platform_exit);