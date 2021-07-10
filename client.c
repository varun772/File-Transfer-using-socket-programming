#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include  <unistd.h>
#include <arpa/inet.h>
#define PORT 8000

int main(int argc, char const *argv[])
{
	struct sockaddr_in address;
    int sock = 0;
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Socket creation error \n");
		return -1;
	}
    struct sockaddr_in serv_addr;
	memset(&serv_addr, '0', sizeof(serv_addr)); 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
    char input[1024];
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
    int t;
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nConnection Failed \n");
		return -1;
	}
    t =1;
	while(t >0)
    {
        printf("enter the input\n");
		fgets(input,1024,stdin);
        int len = strlen(input);
		input[len-1]='\0';
		write(sock,input,len);
		char * split = strtok(input," \n\t\r");
		int j=0;
        char * array[256];
		while(split != NULL)
		{
			array[j]=split;
			j++;
			split=strtok(NULL, " \n\t\r");
		}
        char * recvd = (char *) calloc(256,sizeof(char));
        char * ftrans = (char *) calloc(256,sizeof(char));
		if(strcmp(array[0],"get")==0)
        {
            int i = 1;
			while(i<j)
            {
				int total;
				read(sock,recvd,256);
				total=atoi(recvd);
				write(sock,recvd,strlen(recvd));
				memset(recvd,0,sizeof(recvd));
                if(total < 0)
                {
                    printf("\nFile doesnt exist %s\n",array[i]);
                    i++;
                }
				else
				{
					int fp = open(array[i],O_WRONLY | O_TRUNC | O_CREAT,0644);
					if(fp < 0)
					{
						printf("Download error exiting and disconecting\n");
						exit(0);
					}
					if(total)
					{
                        int t =1;
                        int currcnt,count =0;
                        float statusn;
						while(t > 0)
						{
                            currcnt = read(sock,ftrans,256);
							write(fp,ftrans,currcnt);
                            memset(ftrans,0,sizeof(ftrans));
							count=count + currcnt;
							statusn = (((float) count )/total);
							printf("\rDownloading %.5f%%",statusn*100);
							if(currcnt<256 || currcnt==0 ||total == count)
							{
								printf("\nDownloaded\n");
                                i++;
								write(sock,ftrans,sizeof(ftrans));
								close(fp);
								break;
							}
						}
					}
					else
					{
						printf("\nDownloaded it's an empty file\n");
						i++;
						close(fp);
					}
				}
			}
		}
		else if (strcmp(array[0],"exit")==0)
        {
			printf("exit and disconnect\n");
			return 0;
		}
	}
	return 0;
}
