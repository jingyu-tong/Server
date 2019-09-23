//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Server.h"
#include "Channel.h"
#include "Connection.h"

#include <sys/socket.h>
#include <string.h>
#include <functional>

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

Server::Server(EventLoop* loop, int port, int thread_num) 
    :   started_(false),
        loop_(loop),
        port_(port),
        listenfd_(createNonblockingSocketsFd()),
        accept_channel_(new Channel(loop_, listenfd_)),
        thread_pool_(new EventLoopThreadPool(loop_, thread_num))
{
    //bind the sockets
    bindSocketsAndPort(listenfd_, port_);
}

void Server::start() {
    //注测处理新连接函数
    //accept_channel_->setReadCallback(std::bind(&Server::handleConnection, this));
    accept_channel_->setReadCallback([this]() {this->handleConnection(); });
    accept_channel_->enableReading();
    started_  = true;
    thread_pool_->start();
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
    
    int connfd = 0;
    while( (connfd = accept(listenfd_, (struct sockaddr*) &client_addr, &clien)) > 0) { //成功，返回新的描述符
        EventLoop* conn_loop = thread_pool_->getNextLoop();
        ConnectionPointer new_connection(new Connection(conn_loop, connfd));
        connections_.insert(new_connection);
        //通过Connection类设置各种回调
        new_connection->setMessageCallback(message_callback_);
        new_connection->setConnectionCallback(connection_callback_);
        //new_connection->setCloseCallback(std::bind(&Server::handleClose, this, std::placeholders::_1));
        new_connection->setCloseCallback([this](const ConnectionPointer & conn) {this->handleClose(conn); });
        //conn_loop->runInLoop(std::bind(&Connection::settingDone, new_connection)); //必须，因为只允许自己本身线程更改channe信息
        conn_loop->runInLoop([new_connection]() {new_connection->settingDone();});
        if(connfd >= kMaxFds) {
            new_connection->forceClose();
            continue;
        }
    }
}

//删除链接函数
//在Connection中注册，用于移除map中的链接
void Server::handleClose(const ConnectionPointer& conn) {
    //loop_->runInLoop(std::bind(&Server::handleCloseInLoop, this, conn));
    loop_->runInLoop([this, conn]() {this->handleCloseInLoop(conn); });
}
 void Server::handleCloseInLoop(const ConnectionPointer& conn) {
     loop_->assertInLoopThread();
     connections_.erase(conn);
     EventLoop* conn_loop = conn->getLoop();
     //conn_loop->runInLoop(std::bind(&Connection::destroyed, conn));
     conn_loop->runInLoop([conn]() {conn->destroyed(); });
 }