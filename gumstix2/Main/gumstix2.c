#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

char eth_in_str[1024];
char eth_out_str[1024];


struct states {
   int led[4];
   int pir;
   
} state;


void setLed(int num,int val)
{
    state.led[num]=val;
    int pFile;
    char line[10];    
	pFile = open("/dev/myled", O_RDWR);
    memset(line,0,sizeof(line));
	if (pFile < 0) {
		fputs("myled not found\n",stderr);
		return;
	}
    sprintf(line,"%d%d%d%d\r\n",state.led[0],state.led[1],state.led[2],state.led[3]);
	write(pFile,line,sizeof(line));
    close(pFile);
}

int readPir()
{
    int pFile;
    char line[2];    
	pFile = open("/dev/mymotion", O_RDWR);
    memset(line,0,sizeof(line));
	if (pFile < 0) {
		fputs("mymotion not found\n",stderr);
		return 1;
	}
	read(pFile,line,sizeof(line));
    if(line[0] == '1'){
        close(pFile);
        state.pir=1;
        return 1;
        }
    else if (line[0] == '0'){
    close(pFile);
    state.pir=0;
    return 0;    
    }
}
void process_get()
{
    readPir();
    memset(eth_out_str,0,sizeof(eth_out_str));
    sprintf(eth_out_str, "r%d%d%d%d%d\r\n",state.led[0],state.led[1],state.led[2],state.led[3],state.pir); //r-> reply
}


void process_set(){
    int ledno = atoi(eth_in_str[1]);
    int onoff = atoi(eth_in_str[2]);
    setLed(ledno,onoff);
    state.led[ledno]=onoff;

}
int main(int argc, char *argv[])
{
    int listenfd = 0, connfd = 0,n=0;
    struct sockaddr_in serv_addr; 
    time_t ticks; 

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000); 

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1)
    {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL); 
        while(1)
    {
        memset(eth_in_str,0,sizeof(eth_in_str));
        memset(eth_out_str,0,sizeof(eth_out_str));
        read(connfd, eth_in_str, sizeof(eth_in_str));
        printf("Received %s\n",eth_in_str );
        if(eth_in_str[0]=='g')
        {
            process_get();
            write(connfd , eth_out_str , sizeof(eth_out_str));
        }
        else if(eth_in_str[0]=='s')
        {
            process_set();
        }
        
        //close(connfd);
        sleep(1);
     }
    }
}
