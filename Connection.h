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

//封装http所需要的一些信息
class HttpInformation : noncopyable {
    public:
        enum State {
            kExpectRequest,
            kExpectHeader,
            kExpectBody,
            kFinished,
        };

        HttpInformation() 
            :   state_(kExpectRequest)
        {

        }

        //设置各个http信息
        void setState(enum State state) {
            state_ = state;
        }
        enum State getState() const{
            return state_;
        }
        void setMethod(std::string method) {
            method_ = std::move(method);
        }
        std::string getMethod() const {
            return method_;
        }
        void setUri(std::string uri) {
            uri_ = std::move(uri);
        }
        std::string getUri() const {
            return uri_;
        }
        void setVerison(std::string verison) {
            verison_ = verison;
        }
        std::string getVerison() const {
            return verison_;
        }
        void setHeaders(std::string key, std::string val) {
            headers_[key] = val;
        }
        std::map<std::string, std::string> getHeaders() const{
            return headers_;
        }
    private:
        enum State state_;
        std::string method_;
        std::string uri_;
        std::string verison_;
        std::map<std::string, std::string> headers_; //
};

//用于封装每个连接需要的信息和操作

class Connection : noncopyable, 
                                        public std::enable_shared_from_this<Connection>
{
    public:
        enum State {kconnected, kdisconnecting, kdisconnected}; //用于关闭链接识别状态

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
        enum State getState() const {
            return state_;
        }
        //for HttpServer
        HttpInformation* getHttpinfo() {
            return &httpinfo_;
        }

        //发送数据封装
        void send(const std::string& message);
        void shutdown(); //关闭链接
    private:
        void handleMessage();
        void handleWrite(); //发送不完，channel写回调
        void handleClose();
        void sendInLoop(const std::string& message);//在ioloop中发送，供send使用
        void shutdownInLoop();

        EventLoop* loop_;
        int connfd_;
        std::unique_ptr<Channel>  channel_;
        Buffer inbuffer_;
        Buffer outbuffer_;
        enum State state_;
        HttpInformation httpinfo_;


        MessageCallback message_callback_;
        ConnectionCallback connection_callback_;
        CloseCallback close_callback_;
};

#endif