//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#include "Connection.h"

#include <memory>

Connection::Connection(EventLoop* loop, int connfd)
    :   loop_(loop),
        connfd_(connfd),
        channel_(new Channel(loop_, connfd_))
{
    
}

void Connection::settingDone() {
    channel_->setReadCallback(std::bind(&Connection::handleMessage, this));
    channel_->enableReading();
    channel_->setWriteCallback(std::bind(&Connection::handleWrite, this));
    connection_callback_(shared_from_this());
}

void Connection::handleMessage() {
    char buf[65536];
    ssize_t n = read(connfd_, buf, sizeof(buf));
    if(n > 0) {
        inbuffer_ += Buffer(buf, buf + n);
        message_callback_(shared_from_this(), inbuffer_);
    } else if(n == 0) { //关闭
        handleClose();
    }
}

void Connection::handleClose() {
    channel_->disableAll(); //自动更新移除poller
    close(connfd_); //此时关，新的Channel有可能立马占用这个，close回调后channel将自动析构
    close_callback_(shared_from_this());
}

//发送，判断是否在IO线程，不在就调用runinloop跨线程调用
void Connection::send(const std::string& message) {
    if(loop_->isInLoopThread()) {
        sendInLoop(message);
    } else {
        loop_->runInLoop(std::bind(&Connection::sendInLoop, this, message));
    }
}

//IO线程发送数据
void Connection::sendInLoop(const std::string& message) {
    loop_->assertInLoopThread();
    ssize_t nwrite = 0; //记录已经写的个数

    if(!channel_->isWriting() && outbuffer_.size() == 0) { //没有需要发送的数据
        nwrite = write(channel_->getFd(), message.data(), message.size());
        if(nwrite < 0) {
            //error
        } else {
            
        }
    }

    //没有写完，说明暂时不可写了，开启channel的
    if(nwrite < message.size()) {
        outbuffer_  = message.substr(nwrite);
        if(!channel_->isWriting()) {
            channel_->enableWriting();
        }
    }
}

//写回调
void Connection::handleWrite() {
    loop_->assertInLoopThread();
    if(channel_->isWriting()) {
        ssize_t n = write(channel_->getFd(), outbuffer_.data(), outbuffer_.size());
        if(n <= 0) {
            //error
        } else {
            if(n == outbuffer_.size()) { //写完，可以关闭写回调
                channel_->disableWriting();
            }
            outbuffer_ = outbuffer_.substr(n);
        }
    }
}