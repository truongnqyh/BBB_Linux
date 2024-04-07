#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kdev_t.h>

///< The license type -- this affects runtime behavior
MODULE_LICENSE("GPL");
///< The author -- visible when you use modinfo
MODULE_AUTHOR("Truong Nguyen Van");
///< The description -- see modinfo
MODULE_DESCRIPTION("Pcd module");
///< The version of the module
MODULE_VERSION("0.1");

#define DEV_MEM_SIZE 512
/* Pseudo device's memory */
char device_buf[DEV_MEM_SIZE];

int pcd_open(struct inode *inode, struct file *filp)
{
    pr_info("Open successful \n");
    return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("Release successful \n");
    return 0;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("Read requested for %zu bytes \n", count);
    pr_info("Current file position: %lld \n", *f_pos);
    /* Update "count" */
    if((*f_pos + count) > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *f_pos;
    }
    if(!count)
    {
        pr_err("No space left on the device \n");
        return -ENOMEM;
    }
    /* Copy data to user buffer */
    if(copy_from_user(device_buf + *f_pos, buff, count))
    {
        return -EFAULT;
    }
    /* Update the current file position */
    *f_pos += count;
    pr_info("Number of bytes read: %zu \n", count);
    pr_info("Update file position: %lld \n", *f_pos);

    /* Return number of bytes write */
    return count;
}

ssize_t pcd_read(struct file *filp, char __user *buff, size_t count, loff_t *f_pos)
{
    pr_info("Read requested for %zu bytes \n", count);
    pr_info("Current file position: %lld \n", *f_pos);
    /* Update "count" */
    if((*f_pos + count) > DEV_MEM_SIZE)
    {
        count = DEV_MEM_SIZE - *f_pos;
    }
    /* Copy data to user buffer */
    if(copy_to_user(buff, device_buf + *f_pos, count))
    {
        return -EFAULT;
    }
    /* Update the current file position */
    *f_pos += count;
    pr_info("Number of bytes read: %zu \n", count);
    pr_info("Update file position: %lld \n", *f_pos);

    /* Return number of bytes read */
    return count;
}

loff_t pcd_lseek(struct file *filp, loff_t offset, int whence)
{
    loff_t temp = 0;

    pr_info("Lseek requested \n");
    /*
    *if whence = SEEK_SET =====>>>>>> filp->f_pos = off;
    *if whence = SEEK_CUR =====>>>>>> filp->f_pos += off;
    *if whence = SEEK_END =====>>>>>> filp->f_pos = DEV_MEM_SIZE - off;
    * include/uapi/asm-generic/errno-base.h to track error codes and descriptions
    */
    switch(whence)
    {
        case SEEK_SET:
            if((offset > DEV_MEM_SIZE) || (offset < 0))
            {
                return -EINVAL;
            }
            else
            {
                filp->f_pos = offset;
            }
            break;
        case SEEK_CUR:
            temp = filp->f_pos + offset;
            if((temp > DEV_MEM_SIZE) || (temp < 0))
            {
                return -EINVAL;
            }
            else
            {
                filp->f_pos = temp;
            }
            break;
        case SEEK_END:
            temp = DEV_MEM_SIZE - offset;
            if((temp > DEV_MEM_SIZE) || (temp < 0))
            {
                return -EINVAL;
            }
            else
            {
                filp->f_pos = temp;
            }
            break;
        default:
            pr_info("Invalid whence \n");
            return -EINVAL;
    }

    pr_info("New current file position: %lld \n", filp->f_pos);
    return filp->f_pos;
}

/* Device number */
dev_t device_number;
/* Cdev */
struct cdev pcd_cdev;

/* file operation of driver */
struct file_operations pcd_fops =
{
    .open = pcd_open,
    .release = pcd_release,
    .write = pcd_write,
    .read = pcd_read,
    .llseek = pcd_lseek,
    .owner = THIS_MODULE
};

struct class *class_pcd;
struct device *device_pcd;
#undef pr_fmt
#define pr_fmt(fmt) "%s: " fmt, __func__

static int pcd_init(void)
{ 
    int ret_val;
    pr_info("Loading pcd module...\n"); 
    /* Dynamically register pcd char device */
    ret_val = alloc_chrdev_region(&device_number, 0, 1, "pcd");
    if(ret_val < 0) 
    {
        pr_err("Alloc chrdev fail\n");
        goto alloc_fail;
    }
    pr_info("Device number <major>:<minor> = %d:%d\n", MAJOR(device_number), MINOR(device_number)); 
    /* Initialize cdev structure with fops*/
    cdev_init(&pcd_cdev, &pcd_fops);
    /* Initialize owner for cdev, to prevent the module from being
    unloaded while the structure in use */
    pcd_cdev.owner = THIS_MODULE;
    /* Make pcd device registration with the VFS */
    ret_val = cdev_add(&pcd_cdev, device_number, 1);
    if(ret_val < 0) 
    {
        pr_err("Add cdev fail\n");
        goto unregister_chrdev;
    }
    /* Create device class under /sys/class/ */   
    class_pcd = class_create(THIS_MODULE,"pcd_class");
    if(IS_ERR(class_pcd))
    {
        /* IS_ERR to check if the pointer is an error */
        /* ERR_PTR to get the error code(int) to pointer */
        /* PTR_ERR to get the error pointer to number */
        pr_err("Failed to create class\n");
        ret_val = PTR_ERR(class_pcd);
        goto cdev_del;
    }
    /* Populate the sysfs with device information */
    device_pcd = device_create(class_pcd, NULL, device_number, NULL, "pcd");
    if(IS_ERR(device_pcd))
    {
        pr_err("Failed to create device\n");
        ret_val = PTR_ERR(device_pcd);
        goto class_del;
    }
    return 0;

class_del:
    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
cdev_del:
    cdev_del(&pcd_cdev);
unregister_chrdev:
    unregister_chrdev_region(device_number, 1);
alloc_fail:
    pr_info("Module load failed\n");
    return ret_val;
} 
  
static void pcd_exit(void)
{ 
    device_destroy(class_pcd, device_number);
    class_destroy(class_pcd);
    cdev_del(&pcd_cdev);
    unregister_chrdev_region(device_number, 1);
    pr_info("Module unloaded\n");
} 

module_init(pcd_init);
module_exit(pcd_exit);
