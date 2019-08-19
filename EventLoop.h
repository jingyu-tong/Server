//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include "base/Thread.h"
#include "base/CurrentThread.h"
#include "base/noncopyable.h"
#include "Epoll.h"

#include <assert.h>
#include <memory>
#include <vector>

class Epoll;
class Channel;

//封装Reactor 
class EventLoop : noncopyable
{
public:
    EventLoop();
    ~EventLoop();
    //main loop
    void loop();
    bool isInLoopThread() {
        return threadID_ == CurrentThread :: tid();
    }
    void assertInLoopThread() {
        assert(isInLoopThread());
    }
    void quit() {
        quit_ = true;
    }

    //更新事件
    void updateChannel(Channel* channel);

private:
    typedef std::vector<Channel*> ChannelList;

    bool looping_; 
    bool quit_;
    const pid_t threadID_;
    std::shared_ptr<Epoll> poller_; 
    ChannelList active_channels_;
};


#endif