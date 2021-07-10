
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <unistd.h>
#define PORT 8000

int main(int argc, char const *argv[])
{
	int server_fd=0,opt;
	struct sockaddr_in address;
	int addrlen = sizeof(address);
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
    opt =1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
	{
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
    address.sin_family = AF_INET;
    address.sin_port = htons( PORT );
    address.sin_addr.s_addr = INADDR_ANY;
	if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
	{
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
    int new_socket;
	if (listen(server_fd, 5) == -1)
	{
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
    new_socket = 0;
    int value;
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
	{
		perror("accept");
		exit(EXIT_FAILURE);
	}
    
    int t =1;
	while(t >0)
	{
		char * input =(char *) calloc (256,sizeof(char));
		value = read(new_socket , input, 256);
		char * split = strtok(input," \n\t\r");
		int j=0;
        char * array[50];
		while(split != NULL)
        {
			array[j]=split;
			split=strtok(NULL, " \n\t\r");
            j++;
		}
		if(strcmp(array[0],"get")==0)
        {
            int i = 1;
			while(i<j)
            {
				int total=0,sent=0,fp;
				char *sendchar,*ftrans;
                fp = open(array[i],O_RDONLY);
				sendchar= (char *) calloc (256,sizeof(char));
				ftrans = (char *) calloc (256,sizeof(char));
				if(fp<0)
                {
                    total--;
                    sprintf(sendchar,"%d",total);
                    i++;
					write(new_socket,sendchar,strlen(sendchar));
					read(new_socket,sendchar,256);
					memset(sendchar,0,sizeof(sendchar));
				}
				else
                {
					total=lseek(fp,0,SEEK_END);
					sprintf(sendchar,"%d",total);
                    lseek(fp,0,SEEK_SET);
					write(new_socket,sendchar,strlen(sendchar));
					read(new_socket,sendchar,256);
					memset(sendchar,0,sizeof(sendchar));
					while(1 && total)
                    {
                        int rchck=read(fp,ftrans,256);
						if(rchck > 0)
                        {
							write(new_socket,ftrans,rchck);
							memset(ftrans,0,sizeof(ftrans));
							if(rchck<256)
                            {
                                read(new_socket,ftrans,sizeof(ftrans));
                                memset(ftrans,0,sizeof(ftrans));
								i++;
								close(fp);
								break;
							}
						}
						else
                        {
                            read(new_socket,ftrans,sizeof(ftrans));
                            memset(ftrans,0,sizeof(ftrans));
							i++;
							close(fp);
							break;
						}
					}
				}
			}
		}
		else if (strcmp("exit",array[0])==0)
        {
			printf("exit and disconnect\n");
            return 0;
		}
		else
        {
			printf("Files format should be as get <file1>\n");
        }
	}
	close(new_socket);
	return 0;
}
