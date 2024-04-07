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
MODULE_DESCRIPTION("Pcd multiple devices driver");
///< The version of the module
MODULE_VERSION("0.1");

#define MAX_DEVICES 4
#define DEV_MEM_SIZE_PCD1 1024
#define DEV_MEM_SIZE_PCD2 512
#define DEV_MEM_SIZE_PCD3 1024
#define DEV_MEM_SIZE_PCD4 512
/* Pseudo device's memory */
char device_buf_pcd1[DEV_MEM_SIZE_PCD1];
char device_buf_pcd2[DEV_MEM_SIZE_PCD2];
char device_buf_pcd3[DEV_MEM_SIZE_PCD3];
char device_buf_pcd4[DEV_MEM_SIZE_PCD4];

/* Device private data structure */
struct pcddevice_data_structure
{
    char *buffer; /* start address of the device's memory */
    unsigned size; /* size of the device's memory */
    const char *serial_number; /* serial number of the device */
    int perm; 
    struct cdev cdev; /* character device structure */
};

/* Driver private data struct*/
struct pcddriver_data_structure 
{
    int num_devices;
    /* Device number */
    dev_t device_number;
    struct class *class_pcd;
    struct device *device_pcd;
    struct pcddevice_data_structure dev_data[MAX_DEVICES];
};

#define RDONLY 0x1
#define WRONLY 0x10
#define RDWR  0x11
struct pcddriver_data_structure pcdrv_data =
{
   .num_devices = MAX_DEVICES,
   .dev_data = 
   {
        [0] = {
            .buffer = device_buf_pcd1,
            .size = DEV_MEM_SIZE_PCD1,
            .serial_number = "PCD1",
            .perm = RDONLY, /* RDONLY*/
        },
        [1] = {
            .buffer = device_buf_pcd2,
            .size = DEV_MEM_SIZE_PCD2,
            .serial_number = "PCD2",
            .perm = WRONLY, /* WRONLY*/
        },
        [2] = {
            .buffer = device_buf_pcd3,
            .size = DEV_MEM_SIZE_PCD3,
            .serial_number = "PCD3",
            .perm = RDWR, /* RDWR*/
        },
        [3] = {
            .buffer = device_buf_pcd4,
            .size = DEV_MEM_SIZE_PCD4,
            .serial_number = "PCD4",
            .perm = RDWR, /* RDWR*/
        }
   }
};

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
    int ret = 0;
    int minor = 0;
    struct pcddevice_data_structure *pcddevice_data = NULL;

    /* Get minor number*/
    minor = inode->i_rdev;
    pr_info("Minor accses: %d \n", minor);
    /* Get device private data */
    pcddevice_data = container_of(inode->i_cdev, struct pcddevice_data_structure, cdev);
    /* Save device private data for other methods */
    filp->private_data = pcddevice_data;
    /* Check permission */
    ret = check_permission(pcddevice_data->perm, filp->f_mode);

    (!ret)?pr_info("Open successful \n"):pr_info("Open failed \n");

    return 0;
}

int pcd_release(struct inode *inode, struct file *filp)
{
    pr_info("Release successful \n");
    return 0;
}

ssize_t pcd_write(struct file *filp, const char __user *buff, size_t count, loff_t *f_pos)
{
    struct pcddevice_data_structure *pcddevice_data = NULL;
    int maxsize = 0;

    pcddevice_data = filp->private_data;
    maxsize = pcddevice_data->size;
    pr_info("Read requested for %zu bytes \n", count);
    pr_info("Current file position: %lld \n", *f_pos);
    /* Update "count" */
    if((*f_pos + count) > maxsize)
    {
        count = maxsize - *f_pos;
    }
    if(!count)
    {
        pr_err("No space left on the device \n");
        return -ENOMEM;
    }
    /* Copy data to user buffer */
    if(copy_from_user(pcddevice_data->buffer + *f_pos, buff, count))
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
    struct pcddevice_data_structure *pcddevice_data = NULL;
    int maxsize = 0;

    pcddevice_data = filp->private_data;
    maxsize = pcddevice_data->size;

    pr_info("Read requested for %zu bytes \n", count);
    pr_info("Current file position: %lld \n", *f_pos);
    /* Update "count" */
    if((*f_pos + count) > maxsize)
    {
        count = maxsize - *f_pos;
    }
    /* Copy data to user buffer */
    if(copy_to_user(buff, pcddevice_data->buffer + *f_pos, count))
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
    struct pcddevice_data_structure *pcddevice_data = NULL;
    int maxsize = 0;

    pcddevice_data = filp->private_data;
    maxsize = pcddevice_data->size;
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
            if((offset > maxsize) || (offset < 0))
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
            if((temp > maxsize) || (temp < 0))
            {
                return -EINVAL;
            }
            else
            {
                filp->f_pos = temp;
            }
            break;
        case SEEK_END:
            temp = maxsize - offset;
            if((temp > maxsize) || (temp < 0))
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

#undef pr_fmt
#define pr_fmt(fmt) "%s: " fmt, __func__

static int pcd_init(void)
{ 

    int ret_val;
    int index = 0;

    pr_info("Loading pcd module...\n"); 
    /* Dynamically register pcd char device */
    ret_val = alloc_chrdev_region(&pcdrv_data.device_number, 0, 4, "pcd");
    if(ret_val < 0) 
    {
        pr_err("Alloc chrdev fail\n");
        goto alloc_fail;
    }
    for (index = 0; index < MAX_DEVICES; index++)
    {
        pr_info("Device number of PCD-%d <major>:<minor> = %d:%d\n", \
        index + 1, MAJOR(pcdrv_data.device_number + index), MINOR(pcdrv_data.device_number + index)); 
    }
    /* Create class for pcd */
    pcdrv_data.class_pcd = class_create(THIS_MODULE,"pcd_class");
    if(IS_ERR(pcdrv_data.class_pcd))
    {
        pr_err("Failed to create class\n");
        ret_val = PTR_ERR(pcdrv_data.class_pcd);
        goto unregister_chrdev;
    }

    for (index = 0; index < MAX_DEVICES; index++)
    {
        /* Initialize cdev structure with fops*/
        cdev_init(&pcdrv_data.dev_data[index].cdev, &pcd_fops);
        /* Initialize owner for cdev, to prevent the module from being unloaded while the structure in use */
        pcdrv_data.dev_data[index].cdev.owner = THIS_MODULE;
        /* Make pcd device registration with the VFS */
        ret_val = cdev_add(&pcdrv_data.dev_data[index].cdev, pcdrv_data.device_number + index, 1);
        if(ret_val < 0) 
        {
            pr_err("Add cdev fail\n");
            goto cdev_del;
        }
        /* Populate the sysfs with device information */
        pcdrv_data.device_pcd = device_create(pcdrv_data.class_pcd, NULL, pcdrv_data.device_number + index, NULL, "pcd-%d", index + 1);
        if(IS_ERR(pcdrv_data.device_pcd))
        {
            pr_err("Failed to create device\n");
            ret_val = PTR_ERR(pcdrv_data.device_pcd);
            goto class_del;
        }
    }
    return 0;

cdev_del:
    for (index = 0; index < MAX_DEVICES; index++)
    {
        cdev_del(&pcdrv_data.dev_data[index].cdev);
    }
class_del:
    for (index = 0; index < MAX_DEVICES; index++)
    {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + index);
    }
    class_destroy(pcdrv_data.class_pcd);
unregister_chrdev:
    unregister_chrdev_region(pcdrv_data.device_number, MAX_DEVICES);
alloc_fail:
    pr_info("Module load failed\n");
    return ret_val;
} 
  
static void pcd_exit(void)
{ 
    int index = 0;

    for (index = 0; index < MAX_DEVICES; index++)
    {
        device_destroy(pcdrv_data.class_pcd, pcdrv_data.device_number + index);
    }
    class_destroy(pcdrv_data.class_pcd);
    for (index = 0; index < MAX_DEVICES; index++)
    {
        cdev_del(&pcdrv_data.dev_data[index].cdev);
    }
    unregister_chrdev_region(pcdrv_data.device_number + index, MAX_DEVICES);
    pr_info("Module unloaded\n");
} 

module_init(pcd_init);
module_exit(pcd_exit);
