//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Server.h"
#include "Channel.h"

#include <sys/socket.h>
#include <string.h>

//From unp:
//服务器调用一般顺序socket->bind->listen->accept->process

//创建非阻塞套接字
int createNonblockingSocketsFd() {
    //IPv4, 非阻塞, TCP
    int sockfd =  socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);

    if(sockfd < 0) {
        //failed
    }
    return sockfd;
}

//将端口地址绑定到listenfd
void bindSocketsAndPort(int sockfd, int port) {
    struct sockaddr_in serve_addr;
    bzero(&serve_addr, sizeof(serve_addr));
    serve_addr.sin_family = AF_INET;
    serve_addr.sin_addr.s_addr = htonl(INADDR_ANY); //主机字节序换成网络字节序
    serve_addr.sin_port = htons(port);

    int ret = bind(sockfd, (struct sockaddr*) &serve_addr, sizeof(serve_addr));
    
    if(ret < 0) {
        //failed bind
    } else {
        listen(sockfd, SOMAXCONN);
    }
}

Server::Server(EventLoop* loop, int port) 
    :   started_(false),
        loop_(loop),
        port_(port),
        listenfd_(createNonblockingSocketsFd()),
        accept_channel_(new Channel(loop_, listenfd_))
{
    //bind the sockets
    bindSocketsAndPort(listenfd_, port_);

    //注测处理新连接
    accept_channel_->setReadCallback(std::bind(&Server::handleConnection, this));
    accept_channel_->enableReading();
}

void Server::start() {
    started_  = true;
}

//accept 一个
//TODO(jingyu): 尝试其他方法
//1. 循环accept，直到没有新连接
//2. 每次accept N（一般为10）个链接
void Server::handleConnection() {
    loop_->assertInLoopThread();
    struct sockaddr_in client_addr;
    bzero(&client_addr, sizeof(client_addr));
    socklen_t clien = sizeof(client_addr);

    int connfd = accept(listenfd_, (struct sockaddr*) &client_addr, &clien);
    printf("the new connfd is: %d", connfd);
    if(connfd > 0) { //成功，返回新的描述符
        ChannelPointer new_channel(new Channel(loop_, connfd));
        fd_channels_[connfd] = new_channel;
        new_channel->setReadCallback(std::bind(&Server::handleMessage, this, new_channel->getFd()));
        new_channel->enableReading();
        new_channel->setCloseCallback(std::bind(&Server::handleClose, this, new_channel->getFd()));
    }
}

void Server::handleMessage(int connfd) {
    char buf[65536];
    ssize_t n = read(connfd, buf, sizeof(buf));
    if(n > 0) {
        inbuffer_ += Buffer(buf, buf + n);
        message_callback_(connfd, inbuffer_);
    } else if(n == 0) { //关闭
        handleClose(connfd);
    }
    
}

//删除链接函数
//需要在列表中释放并删除持有的channel
//并关闭描述符
void Server::handleClose(int closefd) {
    ChannelPointer close_channel = fd_channels_[closefd];
    fd_channels_.erase(closefd);
    close_channel->disableAll(); //poller会自动移除出关注事件
    close(closefd);
    printf("close fd\n");
}