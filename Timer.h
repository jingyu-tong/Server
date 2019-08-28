//@author Jingyu Tong
//@email: jingyutong0806@gmail.com

#ifndef TIMER_H_
#define TIMER_H_

#include "base/noncopyable.h"
#include "Channel.h"

#include <memory>
#include <queue>
#include <deque>
#include <functional>

class EventLoop;
class Channel;

//定时器
class Timer {
    public:
        typedef std::function<void()> TimerCallback;

        Timer(TimerCallback callBack, int timeout);
        ~Timer();
        
        void update(int timeout);
        bool isValid();
        void setDelete() { delete_ = false; }
        bool isDelete() { return delete_; }
        size_t getExpTime() const { return expired_time_; } 
        void run() {timer_callback_(); }

    private:
        bool delete_; //待删除标志
        size_t expired_time_; //到期时间
        TimerCallback timer_callback_; //到期回调，被删除不执行
};

//用于Timer到期之间的比较
struct TimerCmp {
        typedef std::shared_ptr<Timer> TimerPointer;
        bool operator()(TimerPointer a, TimerPointer b) {
            return a->getExpTime() > b->getExpTime();
        }
};

//管理Timer
class TimerManager {
    public:
        typedef std::function<void()> TimerCallback;
         typedef std::shared_ptr<Timer> TimerPointer;
         
        TimerManager(EventLoop* loop);
        ~TimerManager();
        void addTimer(TimerCallback callback, int timeout); //添加timer
        void handleExpiredEvent(); //处理到期事件
        void addTimerInLoop(TimerPointer timer); //把修改列表转移到IO线程

    private:
        EventLoop* loop_;
        std::priority_queue<TimerPointer,  std::deque<TimerPointer>, TimerCmp> timer_queue_;
        int timerfd_; //timerfd
        std::unique_ptr<Channel> timer_channel_; //根据timerfd构建channel
};



#endif