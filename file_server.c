#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>          /* See NOTES */
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


int main(int argc, char** argv)
{
	if(argc!=3)
	{
		fprintf(stderr, "Usage error!\n");
         exit(1);
	}

	struct stat st;
	if(stat(argv[2],&st) == -1)
	{
		perror("Get file info fail");
		exit(1);
	}
	if(!S_ISREG(st.st_mode))
	{
		fprintf(stderr,"Sended file is not a regular file\n");
		exit(1);
	}	
	file_info fi;
	fi.size = st.st_size;
	char* p  = NULL;
	p = strrchr(argv[2],'/');
	if(p == NULL)
	{
		strcpy(fi.name,argv[2]);
	}
	else
	{
		strcpy(fi.name,p+1);
	}

	int sock_listen;
	sock_listen = socket(AF_INET, SOCK_STREAM, 0);

	//setsockopt函数：设置套接字属性
    //将套接字的SO_REUSEADDR属性设置为1，即允许地址复用

	int optval = 1;
	setsockopt(sock_listen,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof(optval));

	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET;//指定地址家族为Internet地址家族
	myaddr.sin_addr.s_addr = INADDR_ANY; //本机任意地址
	//myaddr.sin_addr.s_addr = inet_addr("118.25.105.34");//指定IP地址为本机的某一个具体IP地址
	//inet_addr:将字符串形式的IP地址转换为一个无符号32位整数（网络字节序）
	myaddr.sin_port = htons(atoi(argv[1]));//指定端口号
	//htons函数：将主机字节序表示的short类型数据转化为网络字节序

	if(-1 == bind(sock_listen, (struct sockaddr*)&myaddr,sizeof(myaddr)))
	{
		perror("bind error");
		exit(1);
	}
	listen(sock_listen, 5);

	struct sockaddr_in clnaddr;
	socklen_t len;
	while(1)
	{


		int sock_conn;//连接套接字,用于和相应的客户端通信

		//accept 函数，接收一个客户端连接请求，如果调用该函数时没有客户端连接请求到来，他将会阻塞，直到成功接收请求，或者出错才返回。
		sock_conn = accept(sock_listen,NULL,NULL);
		len = sizeof(clnaddr);
		sock_conn = accept(sock_listen,(struct sockaddr*)&clnaddr,&len);


		if(-1 == sock_conn)
		{
			perror("连接错误");
			continue;
		}

		printf("客户端%s：%d已经连接！\n",inet_ntoa(clnaddr.sin_addr), clnaddr.sin_port);
		send(sock_conn, &fi, sizeof(fi), 0); //最后一个参数为0，表示使用默认的发送方式

		FILE* fp = NULL;
		int ret;
		char buff[1024];
		fp = fopen(argv[2],"rb");
		if(fp == NULL)
		{
			perror("打开文件失败\n");
			exit(1);
		}

		while(!feof(fp))
		{
			ret = fread(buff,1,sizeof(buff),fp);
			send(sock_conn,buff,ret,0);
		}
		fclose(fp);

		//第六步：断开连接
		close(sock_conn);
	}

	//第七步：关闭监听套接字
	close(sock_listen);


	return 0;
}
