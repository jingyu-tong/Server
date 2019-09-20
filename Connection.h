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
        typedef std::weak_ptr<Timer> WeakTimer;
        enum State {
            kExpectRequest,
            kExpectHeader,
            kExpectBody,
            kFinished,
        };

        HttpInformation() 
            :   state_(kExpectRequest),
                error_(false)
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
            verison_ = std::move(verison);
        }
        std::string getVerison() const {
            return verison_;
        }
        void setHeaders(std::string key, std::string val) {
            headers_[std::move(key)] = std::move(val);
        }
        std::map<std::string, std::string> getHeaders() const{
            return headers_;
        }
        std::string getHeaders(std::string key) {
            return headers_.count(key) == 0 ? "" : headers_[key];
        }
        WeakTimer getTimer() {
            return  timer_;
        }
        void setTimer(WeakTimer timer) {
            timer_ = timer;
        }
        void setError(bool error) {
            error_ = error;
        }
        bool getError() {
            return error_;
        }
    private:
        enum State state_;
        std::string method_;
        std::string uri_;
        std::string verison_;
        bool error_;
        std::map<std::string, std::string> headers_; //
        WeakTimer timer_; //给一个timer用于超时关闭
};

//用于封装每个连接需要的信息和操作

class Connection : noncopyable, 
                                        public std::enable_shared_from_this<Connection>
{
    public:
        enum State {kconnected, kdisconnecting, kdisconnected}; //用于关闭链接识别状态

        Connection(EventLoop* loop, int connfd);
        ~Connection();

        void setMessageCallback(MessageCallback mb) {
            message_callback_ = std::move(mb);
        } 
        void setConnectionCallback(ConnectionCallback cb) {
            connection_callback_ = std::move(cb);
        }
        void setCloseCallback(CloseCallback cb) {
            close_callback_ = std::move(cb);
        }
        void settingDone();
        int getFd() const {
            return connfd_;
        }
        enum State getState() const {
            return state_;
        }
        void setState(enum State state) {
            state_ = state;
        }
        //for HttpServer
        HttpInformation* getHttpinfo() {
            return &httpinfo_;
        }

        void send(const std::string& message);//发送数据封装
        void shutdown(); //关闭链接
        void forceClose() { //强制关闭链接读写端，这回删除这个链接
            if (state_ == kconnected){ //还在链接中
                loop_->runInLoop(std::bind(&Connection::handleClose, shared_from_this()));
            }
        }
        void destroyed();
        EventLoop* getLoop() const {
            return loop_;
        }
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