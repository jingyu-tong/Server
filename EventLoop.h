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
    typedef std::function<void()> Functor;

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
    void quit();

    //更新事件
    void updateChannel(Channel* channel);

    //定时器封装
    void runAfter(TimerCallback callback, int timeout);

    //跨线程调用
    void runInLoop(Functor callback); 
    void queueInLoop(Functor callback);

private:
    typedef std::vector<Channel*> ChannelList;

    void handleEventfd(); //eventfd callback
    void doPendingFunctors(); //处理回调
    void wakeup(); //唤醒

    bool looping_; 
    bool quit_;
    const pid_t threadID_;
    bool calling_pending_; //标志是否在处理用户回调
    int wakeup_fd_; //eventfd

    std::unique_ptr<Channel> wakeup_channel_; //eventfd的channel
    MutexLock mutex_; //用于临界区加锁
    std::shared_ptr<Epoll> poller_; 
    ChannelList active_channels_;
    std::unique_ptr<TimerManager> timer_queue_; //一个reactor持有一个timerqueue
    std::vector<Functor> pending_functors_; //回调地址向量

};


#endif