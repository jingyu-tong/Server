//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "base/noncopyable.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Server.h"

#include <memory>
#include <functional>

//用于封装每个连接需要的信息和操作

class Connection : noncopyable, 
                                        public std::enable_shared_from_this<Connection>
{
    public:

        Connection(EventLoop* loop, int connfd);

        void setMessageCallback(MessageCallback mb) {
            message_callback_ = mb;
        } 
        void setConnectionCallback(ConnectionCallback cb) {
            connection_callback_ = cb;
        }
        void setCloseCallback(CloseCallback cb) {
            close_callback_ = cb;
        }
        void settingDone();
        int getFd() const {
            return connfd_;
        }
    private:
        void handleMessage();
        void handleClose();
        EventLoop* loop_;
        int connfd_;
        std::unique_ptr<Channel>  channel_;
        Buffer inbuffer_;
        Buffer outbuffer_;

        MessageCallback message_callback_;
        ConnectionCallback connection_callback_;
        CloseCallback close_callback_;
};

#endif