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
static char eth_in_str[1024];
static char eth_out_str[1024];
static int ethsocket = 0;
static struct sockaddr_in serv_addr;



int readpir()
{
    return 0;
}
int readservo()
{
    return 0;
}
int readldr()
{
    return 0;
}

void setLed()
{

}
void setServo()
{

}

void setAuto()
{
    
}
void ethernet_get()
{
    int n=0;
    memset(eth_out_str,0,sizeof(eth_out_str));
    strcpy(eth_out_str,"g\r\n");
    if( send(ethsocket , eth_out_str , strlen(eth_out_str) , 0) < 0)
    {
        puts("Send failed");
        return ;
    }
    memset(eth_in_str,0,sizeof(bt_out_str));
    while ( (n = read(ethsocket, eth_in_str, sizeof(eth_in_str)-1)) > 0)
    {
        eth_in_str[n] = 0;
        printf("%s\n",eth_in_str );
    }

}

void ethernet_set(int ledNo, char state)
{
    int out;
    if(state=='0')
        out=0;
    else if(state=='1')
        out=1;
    memset(eth_out_str,0,sizeof(eth_out_str));
    sprintf(eth_out_str,"s%d%d\r\n",ledNo,out);
    if( send(ethsocket , eth_out_str , strlen(eth_out_str) , 0) < 0)
    {
        puts("Send failed");
        return ;
    }
}

void process_get()
{
    //TODO CHECK MAJOR AND MINOR FOR MULTIPLE 
        int ldr=readldr();
        int servo=readservo();
        int pir=readpir();
        memset(bt_out_str,0,sizeof(bt_out_str));
        //sprintf(bt_out_str, "r,1,%d,%d,%d",ldr,servo,pir);
        ethernet_get();

}

void process_set()
{
    //SET for LED, SERVO, AUTOMATIC/MANUAL CONTROL
    //led = 0, servo = 1, auto/manual = 2
    //So final string liek : s101, s201
    if(bt_in_str[1]=='1')//Room Number
    {
        switch(bt_in_str[2])
        {
            case '0':
            setLed(bt_in_str[3]);
            break;
            case '1':
            setServo(bt_in_str[3]);
            break;
            case '2':
            setAuto(bt_in_str[3]);
            break;
            default:
            printf("U dun goofed\n");
        }
    }
    else if(bt_in_str[1]=='2')
    {
        switch(bt_in_str[2])
        {
            case '0':
            ethernet_set(0,bt_in_str[3]);
            break;
            case '1':
            ethernet_set(1,bt_in_str[3]);
            break;
            case '2':
            ethernet_set(2,bt_in_str[3]);
            break;
            case '3':
            ethernet_set(3,bt_in_str[3]);
            break;
            default:
            printf("U dun goofed p2\n");
        }

    }

}
int main(int argc, char **argv)
{
    struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
    memset(bt_in_str,0,sizeof(bt_in_str));
    int btsocket, btclient, bytes_read,disc_flag=0; //To Disconnect
    socklen_t opt = sizeof(rem_addr);

    //Ethernet Init
    int n = 0;
    memset(eth_in_str, '0',sizeof(eth_in_str));
    if((ethsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000); 

    if(inet_pton(AF_INET, "10.0.0.20", &serv_addr.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    }
    
    // BT INIT
    btsocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    // bind socket to port 1 of the first available 
    // local bluetooth adapter
    loc_addr.rc_family = AF_BLUETOOTH;
    loc_addr.rc_bdaddr = *BDADDR_ANY;
    loc_addr.rc_channel = (uint8_t) 1;
    bind(btsocket, (struct sockaddr *)&loc_addr, sizeof(loc_addr));
    //Connect to Gumstix2
    if( connect(ethsocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

while(1){  //Will re-enter when disconnected
    printf("BT Listening\n" );
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
        if( bytes_read > 0 ) 
        {
            bt_in_str[bytes_read]=0;
            printf("received [%s]\n", bt_in_str);
            if(bt_in_str[0]=='g')
                process_get(bt_in_str);
            else if(bt_in_str[0]=='s')
                process_set(bt_in_str);
        }
        sleep(1);
    }
    // close connection
    close(btclient);
    }
    close(btsocket);
    return 0;
}

