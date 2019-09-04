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

        //发送数据封装
        void send(const std::string& message);

        //在ioloop中发送，供send使用
        void sendInLoop(const std::string& message);

    private:
        void handleMessage();
        void handleWrite(); //发送不完，channel写回调
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