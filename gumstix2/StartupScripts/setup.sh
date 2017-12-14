echo "Mknods"
mknod /dev/myled c 61 0
mknod /dev/mymotion c 62 0
echo "Inserting Modules"
insmod myled.ko
insmod mymotion.ko
./gumstix2
