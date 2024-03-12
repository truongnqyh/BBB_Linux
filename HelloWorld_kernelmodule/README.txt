sudo su
modinfo hello.ko
sudo insmod hello.ko -> load file

rmmod -> remove file

To see the message, we need to read the kern.log in /var/log directory.
akshat@gfg:~$ tail /var/log/kern.log // dmesg