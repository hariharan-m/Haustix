#include <stdio.h>
#include <stlib.h>
void main()
{
	int pFile;
	int on=1;
	int off=0;
	pFile = open("/dev/motor", O_RDWR);
	if (pFile < 0) {
		fputs("File not found\n",stderr);
		return 1;
	}
	while(1)
	{
	write(pFile,on,sizeof(on));
	delay(1);
	write(pFile,off,sizeof(on));
	delay(19);
	}
	close(pFile);	
}