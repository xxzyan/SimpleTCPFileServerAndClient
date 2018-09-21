#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

//文件描述信息
#pragma pack(1)
typedef struct _file_info
{
	char name[51];
	unsigned int size;
}file_info; 
#pragma pack()

int main(int argc, char const *argv[])
{


	file_info fi;
	char buf[1024];
	//第一步：创建套接字
	int sock;
	sock = socket(AF_INET,	SOCK_STREAM, 0);

	//第三步：连接服务器
	//指定服务器地址
	struct sockaddr_in srvaddr;
	srvaddr.sin_family = AF_INET;
	srvaddr.sin_addr.s_addr = inet_addr(argv[1]);
	srvaddr.sin_port = htons(atoi(argv[2]));

	if(connect(sock,(struct sockaddr*)&srvaddr, sizeof(srvaddr)) == -1)
	{
		perror("connect error");
		exit(1);
	}

	//第四步：收发数据

	
	int reg;
	FILE* fp = NULL;

	reg = recv(sock, &fi, sizeof(fi),0);
	if(reg < sizeof(fi))
	{
		printf("接收错误\n");
		exit(1);
	}
	printf("%d\n", fi.size);
	fp = fopen(fi.name,"wb");
	if(fp == NULL)
	{
		perror("打开文件失败\n");
		exit(1);
	}

	while(1)
	{

		reg = recv(sock, buf, sizeof(buf),0);
		if(reg==0)
			break;
		fwrite(buf,1,reg,fp);
		
	}
	
	
	//第五步：断开连接（关闭套接字）
	close(sock);
	return 0;
}