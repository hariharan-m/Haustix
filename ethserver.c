#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

char eth_in_str[1024];
char eth_out_str[1024];


struct states {
   int led[4];
   int pir;
   
} state;


void setLed(int num,char val)
{
//TODO set state too
}

void readPir()
{
    //TODO
}
void process_get()
{
    readPir();
    memset(eth_out_str,0,sizeof(eth_out_str));
    sprintf(eth_out_str, "r,%d,%d,%d,%d,%d\n",state.led[0],state.led[1],state.led[2],state.led[3],state.pir); //r-> reply
}


void process_set(){
    int i;
    for(i=2;i<6;i++)
    {
        setLed(i-2,eth_in_str[i]);
    }

}
int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0,n=0;
    struct sockaddr_in serv_addr; 


    char sendBuff[1025];
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        memset(eth_in_str,0,sizeof(eth_in_str));
        memset(eth_out_str,0,sizeof(eth_out_str));
        while ( (n = read(connfd, eth_in_str, sizeof(eth_in_str)-1)) > 0)
        {
            eth_in_str[n] = 0;
            printf("Received %s\n",eth_in_str );
        }
        strcpy(sendBuff,"Reply");
        write(connfd, sendBuff, strlen(sendBuff)); 

        close(connfd);
        sleep(1);
     }
}
