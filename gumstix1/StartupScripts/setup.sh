killall rfcomm
ip ad add 10.0.0.10/24 dev eth0
echo "Mknods"
mknod /dev/myI2C c 61 0
mknod /dev/myled c 62 0
mknod /dev/mymotion c 63 0
mknod /dev/mymotor c 64 0
echo "Inserting Modules"
insmod myI2C.ko
insmod myled.ko
insmod mymotion.ko
insmod mymotor.ko
./light_sensor&
./server
