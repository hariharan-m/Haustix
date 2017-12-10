#include <arpa/inet.h> 
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static char bt_out_str[1024];
static char bt_in_str[1024];
void process_get(char* msg)
{
    //TODO CHECK MAJOR AND MINOR FOR MULTIPLE 
    if(msg[1]=='0')
    {
        int ldr=readldr();
        int servo=readservo();
        int pir=readpir();
        sprintf(bt_out_str, "0,0,%d,%d,%d",ldr,servo,pir);
    }
    else
    {
        ethernet();
    }
}

int main(int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    memset(bt_in_str,0,sizeof(bt_in_str));
    int btsocket, btclient, bytes_read,disc_flag=0; //To Disconnect
    socklen_t opt = sizeof(rem_addr);

    // allocate socket
    btsocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(btsocket, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
while(1){  //Will re-enter when disconnected

    // put socket into listening mode
    listen(btsocket, 1);

    // accept one connection NOTE:BLOCKING
    btclient = accept(btsocket, (struct sockaddr *)&rem_addr, &opt);
    ba2str( &rem_addr.rc_bdaddr, bt_in_str );

    printf("accepted connection from %s\n", bt_in_str);
    disc_flag=0;
    while(disc_flag!=1)
    {
    // read data from the client
        memset(bt_in_str, 0, sizeof(bt_in_str));
        bytes_read = read(btclient, bt_in_str, sizeof(bt_in_str)-1);
        bt_in_str[bytes_read]=0;
        if( bytes_read > 0 ) {
            printf("received [%s]\n", bt_in_str);
            if(bt_in_str[0]=='0')
                process_get(bt_in_str);
            else
                process_set(bt_in_str);
        }
    }
    // close connection
    close(btclient);
    }
    close(btsocket);
    return 0;
}

