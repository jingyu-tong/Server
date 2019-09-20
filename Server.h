//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef SERVER_H_
#define SERVER_H_

#include "base/noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"

#include <netdb.h>
#include <netinet/in.h>
#include <memory>
#include <set>

class Connection;
typedef std::string Buffer;    //用于Buffer
typedef std::shared_ptr<Connection> ConnectionPointer;
typedef std::function<void (const ConnectionPointer&)> ConnectionCallback;
typedef std::function<void (const ConnectionPointer&, Buffer&)> MessageCallback;
typedef std::function<void (const ConnectionPointer&)> CloseCallback;

//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

// INADDR_ANY: 0.0.0.0 Address to accept any incoming messages
// INADDR_LOOPBACK: 127.0.0.1 local

//IPv4 only now
//Server管理Connection类，提供设置回调以及链接建立的功能
//具体的处理都在Connection类完成
class Server : noncopyable {
    public: 

        Server(EventLoop* loop, int port, int thread_num); 
        ~Server() {}
 
        void start(); //启动Server

        //设置新建链接回调，处理消息回调
        void handleConnection();
        void setConnectionCallback(ConnectionCallback callback) {
            connection_callback_ = callback;
        }
        void setMessageCallback(MessageCallback callback) {
            message_callback_ = callback;
        }
        
    private:
        void handleClose(const ConnectionPointer& conn);
        void handleCloseInLoop(const ConnectionPointer& conn);
        bool started_;
        EventLoop* loop_;
        int port_;
        int listenfd_;
        static const int kMaxFds = 100000; //最大描述符个数
        std::unique_ptr<Channel> accept_channel_;
        std::set<ConnectionPointer> connections_;
        std::unique_ptr<EventLoopThreadPool> thread_pool_; 

        ConnectionCallback connection_callback_;
        MessageCallback message_callback_;

};







#endif