#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
int main()
{
	int pFile;
	char on[]="1";
	char off[]="0";
	pFile = open("/dev/motor", O_RDWR);
	if (pFile < 0) {
		fputs("File not found\n",stderr);
		return 1;
	}
	while(1)
	{
	write(pFile,on,sizeof(on));
	usleep(2000);
	write(pFile,off,sizeof(on));
	usleep(18000);
	}
	close(pFile);	
    return 0;
}
