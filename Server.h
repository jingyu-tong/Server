//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef SERVER_H_
#define SERVER_H_

#include "base/noncopyable.h"
#include "EventLoop.h"

#include <netdb.h>
#include <netinet/in.h>
#include <memory>

//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

// INADDR_ANY: 0.0.0.0 Address to accept any incoming messages
// INADDR_LOOPBACK: 127.0.0.1 local

//IPv4 only now
//TODO(jingyu): 
//1. 新建链接 2. 处理消息 
//之后需要限制最大描述符个数
class Server : noncopyable {
    public:
        typedef std::function<void ()> MessageCallback;
        typedef std::shared_ptr<Channel> ChannelPointer;

        Server(EventLoop* loop, int port); 
        ~Server() {}
 
        void start(); //启动Server

        //设置新建链接回调，处理消息回调
        void handleConnection();
        void setMessageCallback(MessageCallback callback) {
            message_callback_ = callback;
        }
        
    private:
        bool started_;
        EventLoop* loop_;
        int port_;
        int listenfd_;
        std::shared_ptr<Channel> accept_channel_;
        std::vector<ChannelPointer> accept_channels_;

        MessageCallback message_callback_;

};







#endif