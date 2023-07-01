# adding more space to your image
Add the following to your vmx file so that you can hit escape and boot the live
file system in the gparted iso you have mounted in your vm seen here
(https://alexanderzeitler.com/articles/resizing-a-vmware-workstation-partition-using-gparted/#:~:text=To%20manage%20the%20partitions%2C%20we,after%20powering%20on%20the%20VM.&text=This%20will%20delay%20the%20boot,ms%20and%20show%20the%20options.)

bios.bootDelay = "2000"

Then boot and resize in gparted

then boot the machine and run
sudo lvextend -L +20G /dev/ubuntu-vg/ubuntu-lv
to extend the filesystem by +20 G (assuming this is the amount you want to
extend by)

then run
   sudo resize2fs /dev/ubuntu-vg/ubuntu-lv
to get the logical partition to resize to the filesystem size

now it should be good

## matlab

dont forget to edit the codetools edit.m file so that xterm is replaced with
rxvt
