# Haustix
Home automation on gumstix using Power Line Communication.
Compilation command for bluetooth files.
- arm-linux-gcc -o bluetooth bluetooth.c -lbluetooth -static
- arm-linux-gcc ethserver.c -o ethserver -static
- arm-linux-gcc ethclient.c -o ethclient -static


# Steps:
- Set IP address on ethernet server using "ip ad add 10.0.0.20/24 dev eth0" and set 10.0.0.20 for ethernet client. 
- Start the ethernet server on one gumstix.
- Start Bluetooth server and ethernet clinet on another gumstix.
-  Connect andriod app to gumstix.
