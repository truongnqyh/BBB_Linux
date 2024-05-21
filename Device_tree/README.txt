->>> Device tree directory
  /home/truong/workspace/kernelbuildscripts/KERNEL/arch/arm/boot/dts

->>> From .dtb to .dts
dtc -I dtb -O dts am335x-boneblack.dtb > /home/truong/workspace/Linux_Aphu/BBB_Linux/Device_tree/bone_black.dts



->>> Clear non-empty directory
Rm -r <dir> -> Clear non-empty directory

->>> device tree folder in BBB while runtime
ls /proc/device_tree

grep -nrwI --include=*.dts --include=*.dtsi i2c0

LCD Nokia 5110