// SpeechServer.cpp: 定义应用程序的入口点。
//

#include "SpeechServer.h"

#include <time.h>
#include <string.h>
#include <stdio.h>

#if !defined(_MSC_VER)/*&&  !defined(__APPLE__)  &&  && !defined(__CYGWIN__)*/
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
#else
    #include <winsock2.h>
    #pragma comment(lib,"ws2_32.lib")
#endif

using namespace std;

#define PORT 10000
#define BUFFSIZE 1024

void StartServer();


int main()
{
	cout << "Hello CMake." << endl;
    StartServer();
	return 0;
}

#if !defined(_MSC_VER)/*&&  !defined(__APPLE__)  &&  && !defined(__CYGWIN__)*/
void StartServer()
{
    int					listenfd, connfd;
    struct sockaddr_in	servaddr;
    char				buff[BUFFSIZE];
    time_t				ticks;

    //  TCP 套接字的创建和客户端一样
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    //    使用 memset 把这个servaddr套接字地址清零
    memset(&servaddr, '0', sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);	/* daytime server */

    //  调用 bind 函数在 listenfd 套接字上分配(赋值)一些详情，这些详情包括 AF_INET(domain)、host、PORT
    bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    //   调用 listen 函数把 listenfd 套接字转换为监听套接字，这样来自于客户的外来连接就可在该套接字上由内核接受
    //   1024 系统内核允许这个监听套接字描述符上排队的最大客户连接数
    //   通过调用 listen, listenfd 变成有丰富功能的监听套接字
    listen(listenfd, BUFFSIZE);
    fprintf(stderr, "the server is running, the port is: %d, this socket listenfd is %d \n", PORT, listenfd);
    while (1){
        fprintf(stderr, "the server is blocking the accept invoke \r");
        //      通常情况下，服务器进程会阻塞于 accept 调用中，等待某个客户连接并被内核接受。tcp 使用三次握手来建立连接。
        //      握手完毕后 accept 返回，返回值是一个新的描述符，称为 已连接描述符，已连接描述符与客户端通信。
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);

        //       最后写入数据，返回 coofd
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        write(connfd, buff, strlen(buff));

        close(connfd);
    }
}
#else
void StartServer()
{
    //创建套接字
    WORD myVersionRequest;
    WSADATA wsaData;                    //包含系统所支持的WinStock版本信息
    myVersionRequest = MAKEWORD(1, 1);  //初始化版本1.1
    int err;
    err = WSAStartup(myVersionRequest, &wsaData);
    if (!err) {
        printf("已打开套接字\n");
    }
    else {
        //进一步绑定套接字
        printf("套接字未打开!");
        return;
    }
    SOCKET serSocket = socket(AF_INET, SOCK_STREAM, 0);//创建了可识别套接字
    //需要绑定的参数
    SOCKADDR_IN addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ip地址
    addr.sin_port = htons(PORT);//绑定端口
    //将套接字绑定到指定的网络地址
    bind(serSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));//绑定完成
    listen(serSocket, BUFFSIZE);                              //第二个参数代表能够接收的最多的连接数
    SOCKADDR_IN clientsocket;
    int len = sizeof(SOCKADDR);
    SOCKET serConn;
    //等待客户端的连接
    serConn = accept(serSocket, (SOCKADDR*)&clientsocket, &len);
    cout << "客户端" << inet_ntoa(clientsocket.sin_addr) << "已连接" << endl;             //客户端已连接
    while (1) {
        char sendBuf[BUFFSIZE];
        sprintf(sendBuf, "server : welcome %s to server.", inet_ntoa(clientsocket.sin_addr));
        //在对应的IP处并且将这行字打印到那里
        send(serConn, sendBuf, strlen(sendBuf) + 1, 0);
        char receiveBuf[BUFFSIZE];
        //接收客户端传来的信息
        recv(serConn, receiveBuf, strlen(receiveBuf) + 1, 0);
        char quit[] = "quit";
        //如果客户端传来了quit信号，则服务端关闭，客户端也关闭
        if (!strcmp(receiveBuf, quit)) {
            break;
        }
        printf("%s\n", receiveBuf);
    }
    closesocket(serConn);   //关闭
    WSACleanup();           //释放资源的操作
}
#endif