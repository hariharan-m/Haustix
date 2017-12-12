echo "Mknods"
mknod /dev/myI2C c 61 0
mknod /dev/myled c 61 0
mknod /dev/mymotion c 61 0
echo "Inserting Modules"
insmod myI2C.ko
insmod myled.ko
insmod mymotion.ko
