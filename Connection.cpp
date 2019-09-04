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
    connection_callback_(shared_from_this());
}

void Connection::handleMessage() {
    char buf[65536];
    ssize_t n = read(connfd_, buf, sizeof(buf));
    if(n > 0) {
        inbuffer_ += Buffer(buf, buf + n);
        message_callback_(shared_from_this());
    } else if(n == 0) { //关闭
        handleClose();
    }
}

void Connection::handleClose() {
    channel_->disableAll(); //自动更新移除poller
    close(connfd_); //此时关，新的Channel有可能立马占用这个，close回调后channel将自动析构
    close_callback_(shared_from_this());
}
