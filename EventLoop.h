//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include "base/Thread.h"
#include "base/CurrentThread.h"
#include "base/noncopyable.h"
#include "Epoll.h"
#include "Timer.h"

#include <assert.h>
#include <memory>
#include <vector>

class Epoll;
class Channel;
class TimerManager;

//封装Reactor 
class EventLoop : noncopyable
{
public:
    typedef std::function<void()> TimerCallback;

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

    //定时器封装
    void runAfter(TimerCallback callback, int timeout);

private:
    typedef std::vector<Channel*> ChannelList;

    bool looping_; 
    bool quit_;
    const pid_t threadID_;
    std::shared_ptr<Epoll> poller_; 
    ChannelList active_channels_;
    std::unique_ptr<TimerManager> timer_queue_; //一个reactor持有一个timerqueue

};


#endif