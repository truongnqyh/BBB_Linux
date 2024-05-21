#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/platform_device.h>
#include "platform.h"
#include <linux/slab.h>
#include <linux/mod_devicetable.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Truong Nguyen Van");
///< The description -- see modinfo
MODULE_DESCRIPTION("plarform device driver");
///< The version of the module
MODULE_VERSION("0.1");

#define MAX_DEVICES 10
#undef pr_fmt
#define pr_fmt(fmt) "%s : " fmt,__func__

enum pcdev_names
{
    PCD_DEV_1,
    PCD_DEV_2,
    PCD_DEV_3,
    PCD_DEV_4
}
struct device_config {
	int pcd_data1;
	int pcd_data2;
};

struct device_config pcdev_config[] = {
	[PCD_DEV_1] = {.pcd_data1 = 25, .pcd_data2 = 60},
	[PCD_DEV_2] = {.pcd_data1 = 10, .pcd_data2 = 50},
    [PCD_DEV_3] = {.pcd_data1 = 15, .pcd_data2 = 40},
	[PCD_DEV_4] = {.pcd_data1 = 20, .pcd_data2 = 30},
};


/* Device private data structure */
struct pcd_dev_private_data
{
    struct pcdev_platform_data pdata; /* platform data */
    char *buffer;
    dev_t device_number;
    struct cdev cdev;
};

/* Driver private data structure */
struct pcd_drv_private_data
{
    int total_devices;
    dev_t device_number_base;
    struct class *class_pcd;
    struct device *device_pcd;
};

struct pcd_drv_private_data pcd_drv_data;
int check_permission(int dev_perm, int acc_mode)
{
    if(dev_perm == RDWR)
    {
        return 0;
    }
    if((dev_perm == RDONLY) && ((acc_mode & FMODE_READ) && !(acc_mode & FMODE_WRITE)))
    {
        return 0;
    }
    if((dev_perm == WRONLY) && (!(acc_mode & FMODE_READ) && (acc_mode & FMODE_WRITE)))
    {
        return 0;
    }

    return -EPERM;
}

int pcd_open(struct inode *inode, struct file *filp)
{
    pr_info("Open successful \n");
    return 0;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    return 0;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    return -ENOMEM;
}

loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
    return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("Release successful \n");
    return 0;
}

/* File operations of the driver */

struct file_operations pcd_fops = {
    .open    = pcd_open,
    .release = pcd_release,
    .read    = pcd_read,
    .write   = pcd_write,
    .llseek  = pcd_lseek,
    .owner   = THIS_MODULE
};

/* Get called when the device is inserted */
int pcd_platform_driver_probe(struct platform_device *pdev)
{
    int ret;
    struct pcd_dev_private_data *pcd_dev_data = NULL;
    struct pcdev_platform_data *pdata = NULL;

    pr_info("A device is detected\n");
    /*1. Get the platform data */
    /* pdata = pdev->dev.platform_data */
    pdata = (struct pcdev_platform_data *)dev_get_platdata(&pdev->dev);
    if(!pdata){
        pr_info("No platform data found for the device\n");
        return -ENODEV;
    }
    /*2. Dynamically allocate memory for device private data */
    pcd_dev_data = devm_kzalloc(&pdev->dev, sizeof(pcd_dev_data), GFP_KERNEL);
    if(!pcd_dev_data){
        pr_info("Failed to allocate memory for device private data\n");
        return -ENOMEM;
    }

    /* Use dev_set_driverdata to save the device private data to driver_data of pdev */
    /* pdev->dev.driver_data = pcd_dev_data; */
    dev_set_drvdata(&pdev->dev, pcd_dev_data);
    /****** Copy data from platform data to device private data ******/
    pcd_dev_data->pdata.perm = pdata->perm;
    pcd_dev_data->pdata.size = pdata->size;
    pcd_dev_data->pdata.serial_number = pdata->serial_number;
    
    pr_info("perm: %d\n", pcd_dev_data->pdata.perm);
    pr_info("size: %d\n", pcd_dev_data->pdata.size);
    pr_info("serial_number: %s\n", pcd_dev_data->pdata.serial_number);

    pr_info("Config item 1 = %d\n", pcdev_config[pdev->id_entry->driver_data].pcd_data1);
    pr_info("Config item 2 = %d\n", pcdev_config[pdev->id_entry->driver_data].pcd_data2);
    /*3. Dynamically allocate memory for device buffer using size information from 
    the platform data */
    pcd_dev_data->buffer = devm_kzalloc(&pdev->dev, pcd_dev_data->pdata.size, GFP_KERNEL);
    if(!pcd_dev_data){
        pr_info("Failed to allocate memory for buffer\n");
        return -ENOMEM;
    }
    /*4. Get the device number */
    pcd_dev_data->device_number = pcd_drv_data.device_number_base + pdev->id;
    /*5. Cdev init and cdev add once platform device is detected */
    cdev_init(&pcd_dev_data->cdev, &pcd_fops);
    pcd_dev_data->cdev.owner = THIS_MODULE;
    ret = cdev_add(&pcd_dev_data->cdev, pcd_dev_data->device_number, 1);
    if(ret < 0){
        pr_info("Failed to add cdev\n");
        return ret;
    }
    /*6. Create device file for the detected platform device */
    pcd_drv_data.device_pcd = device_create(pcd_drv_data.class_pcd, NULL, \
                              pcd_dev_data->device_number, NULL, "pcd_dev-%d", pdev->id);
    if(IS_ERR(pcd_drv_data.device_pcd)){
        pr_info("Failed to create device\n");
        ret = PTR_ERR(pcd_drv_data.device_pcd);
        return ret;
    }
    /* Number of devices was detected */
    pcd_drv_data.total_devices++;
    pr_info("The probe was successful\n");

    return 0;

}

/* Get called when the device is removed */
int pcd_platform_driver_remove(struct platform_device *pdev)
{
    /* Use dev_get_drvdata to get the device private data from driver_data of pdev */
    struct pcd_dev_private_data *pcd_dev_data = dev_get_drvdata(&pdev->dev);
    /*1. Remove a device that was created with func device_create() */
    device_destroy(pcd_drv_data.class_pcd, pcd_drv_data.device_number_base + pdev->id);
    /*2. Remove a cdev entry from the system */
    cdev_del(&pcd_dev_data->cdev); 
    /* Number of devices was removed */
    pcd_drv_data.total_devices--;
    pr_info("A device is removed\n");
    
    return 0;
}

struct platform_device_id pcdevs_ids[] = {
	[0] = {.name = "pcd_1",.driver_data = PCD_DEV_1},
	[1] = {.name = "pcd_2",.driver_data = PCD_DEV_2},
    [2] = {.name = "pcd_3",.driver_data = PCD_DEV_3},
	[3] = {.name = "pcd_4",.driver_data = PCD_DEV_4},
};

struct platform_driver pcd_platform_driver = {
    .probe = pcd_platform_driver_probe,
   .remove = pcd_platform_driver_remove,
   .id_table = pcdevs_ids,
   .driver = {
       .name = "pcd-char-device",
    }
};

static int __init pcd_platform_driver_init(void)
{
    int ret;

    /*1. Dynamically allocate a device number */
    ret = alloc_chrdev_region(&pcd_drv_data.device_number_base, 0, MAX_DEVICES, "pcd_devs");
    if(ret < 0){
        pr_err("Alloc chrdev fail\n");
        return ret;
    }
    /*2. Create a device class -> /sys/class */
    pcd_drv_data.class_pcd = class_create(THIS_MODULE, "pcd_class");
    if(IS_ERR(pcd_drv_data.class_pcd)){
        pr_err("Failed to create class\n");
        unregister_chrdev_region(pcd_drv_data.device_number_base, MAX_DEVICES);
        return PTR_ERR(pcd_drv_data.class_pcd);
    }
    /*3. Register platform devices */
    platform_driver_register(&pcd_platform_driver);
    pr_info("Pcd platform driver loaded \n");

    return 0;
}

static void __exit pcd_platform_driver_exit(void)
{
    /*1. Unregister platform devices */
    platform_driver_register(&pcd_platform_driver);
    /*2. Destroy device class */
    class_destroy(pcd_drv_data.class_pcd);
    /*3. Unregister device numbers for MAX_DEVICES */
    unregister_chrdev_region(pcd_drv_data.device_number_base, MAX_DEVICES);

    pr_info("Pcd platform driver unloaded \n");

}

module_init(pcd_platform_driver_init);
module_exit(pcd_platform_driver_exit);
