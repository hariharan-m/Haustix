# Haustix
Home automation on gumstix using Power Line Communication.
Compilation command for bluetooth files.
- arm-linux-gcc -o bluetooth bluetooth.c -lbluetooth -static
- arm-linux-gcc ethserver.c -o ethserver -static
- arm-linux-gcc ethclient.c -o ethclient -static
